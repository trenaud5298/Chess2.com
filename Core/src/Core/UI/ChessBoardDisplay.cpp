/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */


// Chess Includes
#include <Chess/Core/UI/ChessBoardDisplay.hpp>

// ASIO Includes

// FTXUI Includes
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>

// C++ Includes
#include <algorithm>
#include <string>
#include <cstdint>

namespace Chess {

ChessBoardDisplay::ChessBoardDisplay() {
    m_board.fill(ID::EMPTY);
}

void ChessBoardDisplay::updateBoard(const std::array<ID, 64> &board) {
    m_board = board;
    m_selected.reset();
}

void ChessBoardDisplay::setFlipped(bool flipped) {
    m_flipped = flipped;
}

void ChessBoardDisplay::setTheme(const BoardTheme& theme) {
    m_theme = theme;
}

Pos ChessBoardDisplay::screenToBoard(int displayRow, int displayCol) const {
    std::uint8_t boardRow = m_flipped ? displayRow : (7-displayRow);
    std::uint8_t boardCol = m_flipped ? (7-displayCol) : displayCol;
    return { boardRow, boardCol };
}

ID ChessBoardDisplay::getAt(int boardRow, int boardCol) const {
    return m_board[static_cast<std::size_t>(boardRow * 8 + boardCol)];
}

bool ChessBoardDisplay::isEmpty(ID id) const {
    return id == ID::EMPTY;
}

bool ChessBoardDisplay::isWhitePiece(ID id) const {
    return static_cast<std::uint8_t>(id) >= 1 && static_cast<std::uint8_t>(id) <= WHITE_BOUND;
}

std::string ChessBoardDisplay::toGlyph(ID id) {
    switch (id) {
        case ID::W_KING:
            return "♔";
        case ID::B_KING:
            return "♚";
        case ID::W_QUEEN:
            return "♕";
        case ID::B_QUEEN:
            return "♛";
        case ID::W_ROOK1:
        case ID::W_ROOK2:
            return "♖";
        case ID::B_ROOK1:
        case ID::B_ROOK2:
            return "♜";
        case ID::W_BISHOP1:
        case ID::W_BISHOP2:
            return "♗";
        case ID::B_BISHOP1:
        case ID::B_BISHOP2:
            return "♝";
        case ID::W_KNIGHT1:
        case ID::W_KNIGHT2:
            return "♘";
        case ID::B_KNIGHT1:
        case ID::B_KNIGHT2:
            return "♞";
        case ID::W_PAWN1: case ID::W_PAWN2: case ID::W_PAWN3: case ID::W_PAWN4:
        case ID::W_PAWN5: case ID::W_PAWN6: case ID::W_PAWN7: case ID::W_PAWN8:
            return "♙";
        case ID::B_PAWN1: case ID::B_PAWN2: case ID::B_PAWN3: case ID::B_PAWN4:
        case ID::B_PAWN5: case ID::B_PAWN6: case ID::B_PAWN7: case ID::B_PAWN8:
            return "♟";
        default:
            return " ";
    }
}

void ChessBoardDisplay::handleSelect(int displayRow, int displayCol) {
    if (displayRow < 0 || displayRow > 7 || displayCol < 0 || displayCol > 7) {return;}

    Pos boardPos = screenToBoard(displayRow, displayCol);
    ID id = getAt(boardPos[ROW], boardPos[COL]);

    if (!m_selected) {
        if (!isEmpty(id)) { m_selected = boardPos;}
    } else {
        Pos from = m_selected.value();
        if (from != boardPos && onMove) {
            // TODO: Pass in if promotion is required here
            onMove(from, boardPos, false);
        }
        m_selected.reset();
    }
}

void ChessBoardDisplay::moveCursor(int dRow, int dCol) {
    m_cursorRow = std::clamp(m_cursorRow + dRow, 0, 7);
    m_cursorCol = std::clamp(m_cursorCol + dCol, 0, 7);
}

ChessBoardDisplay::RenderMetrics ChessBoardDisplay::computeRenderMetrics() const {
    int outerWidth = 0;
    int outerHeight = 0;

    if (m_outerBox.x_max >= m_outerBox.x_min) {
        outerWidth = m_outerBox.x_max - m_outerBox.x_min + 1;
    }
    if (m_outerBox.y_max >= m_outerBox.y_min) {
        outerHeight = m_outerBox.y_max - m_outerBox.y_min + 1;
    }

    if (outerWidth <= 0 || outerHeight <= 0) {
        return {
            .cellWidth = DEFAULT_CELL_WIDTH,
            .cellHeight = DEFAULT_CELL_HEIGHT,
        };
    }

    int availableWidth = std::max(0, outerWidth - RANK_LABEL_WIDTH);
    int availableHeight = std::max(0, outerHeight - FILE_LABEL_HEIGHT);

    int widthScale = availableWidth / (8 * 2);
    int heightScale = availableHeight / 8;
    int scale = std::max(1, std::min(widthScale,heightScale));

    return {
        .cellWidth = 2*scale,
        .cellHeight = scale,
    };
}

ChessBoardDisplay::RenderMetrics ChessBoardDisplay::renderedMetrics() const {
    int width = 0;
    int height = 0;

    if (m_boardBox.x_max >= m_boardBox.x_min) {
        width = m_boardBox.x_max - m_boardBox.x_min + 1;
    }
    if (m_boardBox.y_max >= m_boardBox.y_min) {
        height = m_boardBox.y_max - m_boardBox.y_min + 1;
    }

    if (width <= 0 || height <= 0) {
        return computeRenderMetrics();
    }

    return {
        .cellWidth = std::max(1, (width - RANK_LABEL_WIDTH) / 8),
        .cellHeight = std::max(1, (height - FILE_LABEL_HEIGHT) / 8)
    };
}

int ChessBoardDisplay::boardWidth(const RenderMetrics &metrics) const {
    return RANK_LABEL_WIDTH + (metrics.cellWidth * 8);
}

int ChessBoardDisplay::boardHeight(const RenderMetrics &metrics) const {
    return FILE_LABEL_HEIGHT + (metrics.cellHeight * 8);
}

ftxui::Element ChessBoardDisplay::renderCell(int displayRow, int displayCol, const RenderMetrics& metrics) const {
    Pos boardPos = screenToBoard(displayRow, displayCol);
    std::uint8_t boardRow = boardPos[ROW];
    std::uint8_t boardCol = boardPos[COL];

    ID id = getAt(boardRow, boardCol);

    bool isLight = (boardRow + boardCol) % 2 == 0;
    bool isCursor = (displayRow == m_cursorRow && displayCol == m_cursorCol);
    bool isSelected = m_selected.has_value() &&
                      m_selected->at(ROW) == boardRow &&
                      m_selected->at(COL) == boardCol;

    ftxui::Color backgroundColor = isLight ? m_theme.lightSquare.toFTXUIColor() : m_theme.darkSquare.toFTXUIColor();
    if (isCursor) {backgroundColor = m_theme.cursorSquare.toFTXUIColor();}
    if (isSelected) {backgroundColor = m_theme.selectedSquare.toFTXUIColor();}

    std::string glyph = toGlyph(id);
    ftxui::Color glyphColor = isWhitePiece(id) ? m_theme.whitePiece.toFTXUIColor() : m_theme.blackPiece.toFTXUIColor();

    auto cell = ftxui::text(glyph) | ftxui::hcenter | ftxui::vcenter | ftxui::color(glyphColor) | ftxui::bold
        | ftxui::size(ftxui::WIDTH,  ftxui::EQUAL, metrics.cellWidth)
        | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, metrics.cellHeight)
        | ftxui::bgcolor(backgroundColor);

    if (isCursor) { cell |= ftxui::focus; }

    return cell;
}

ftxui::Element ChessBoardDisplay::renderBoard(const RenderMetrics& metrics) const {
    static constexpr char FILES[] = "abcdefgh";

    ftxui::Elements rows;
    rows.reserve(9);

    for (int displayRow = 0; displayRow < 8; displayRow++) {
        int rankNumber = m_flipped ? (displayRow + 1) : (8-displayRow);
        auto rankLabel = ftxui::text(std::to_string(rankNumber))
               | ftxui::center
               | ftxui::size(ftxui::WIDTH,  ftxui::EQUAL, RANK_LABEL_WIDTH)
               | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, metrics.cellHeight);

        ftxui::Elements cells;
        cells.reserve(9);
        cells.push_back(rankLabel);
        for (int displayCol = 0; displayCol < 8; displayCol++) {
            cells.push_back(renderCell(displayRow, displayCol, metrics));
        }
        rows.push_back(ftxui::hbox(std::move(cells)));
    }

    ftxui::Elements fileLabels;
    fileLabels.reserve(9);
    fileLabels.push_back(ftxui::text(std::string(RANK_LABEL_WIDTH, ' ')) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, RANK_LABEL_WIDTH));
    for (int i = 0; i < 8; i++) {
        int fileIdx = m_flipped ? (7 - i) : i;
        std::string label(1, FILES[fileIdx]);
        fileLabels.push_back(
            ftxui::text(label) | ftxui::center | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, metrics.cellWidth) | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, FILE_LABEL_HEIGHT)
        );
    }
    rows.push_back(ftxui::hbox(std::move(fileLabels)));
    return ftxui::vbox(std::move(rows));
}

ftxui::Element ChessBoardDisplay::OnRender() {
    const RenderMetrics metrics = computeRenderMetrics();

    ftxui::Element board = renderBoard(metrics)
        | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, boardWidth(metrics))
        | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, boardHeight(metrics))
        | ftxui::reflect(m_boardBox);

    ftxui::Element centeredBoard = ftxui::vbox({
        ftxui::filler(),
        ftxui::hbox({
            ftxui::filler(),
            board,
            ftxui::filler()
        }),
        ftxui::filler()
    }) | ftxui::reflect(m_outerBox) | ftxui::flex;

    if (Focused()) { centeredBoard |= ftxui::focus; }

    return centeredBoard;
}

bool ChessBoardDisplay::OnEvent(ftxui::Event event) {
    // Keyboard Events
    if (event == ftxui::Event::ArrowUp)    { moveCursor(-1,  0); return true; }
    if (event == ftxui::Event::ArrowDown)  { moveCursor( 1,  0); return true; }
    if (event == ftxui::Event::ArrowLeft)  { moveCursor( 0, -1); return true; }
    if (event == ftxui::Event::ArrowRight) { moveCursor( 0,  1); return true; }

    if (event == ftxui::Event::Return) {
        handleSelect(m_cursorRow, m_cursorCol);
        return true;
    }

    // Mouse Events
    if (event.is_mouse()) {
        auto& mouse = event.mouse();
        const RenderMetrics metrics = renderedMetrics();

        int relX = mouse.x - m_boardBox.x_min - RANK_LABEL_WIDTH;
        int relY = mouse.y - m_boardBox.y_min;

        bool inBounds = relX >= 0 && relY >= 0 && relX < (metrics.cellWidth * 8) && relY < (metrics.cellHeight * 8);

        int displayCol = inBounds ? (relX / metrics.cellWidth) : -1;
        int displayRow = inBounds ? (relY / metrics.cellHeight) : -1;

        // Hover Event
        if (mouse.motion == ftxui::Mouse::Moved && inBounds) {
            m_cursorRow = displayRow;
            m_cursorCol = displayCol;
            return true;
        }

        // Click Event
        if (mouse.button == ftxui::Mouse::Left && mouse.motion == ftxui::Mouse::Pressed) {
            if (inBounds) {
                m_cursorRow = displayRow;
                m_cursorCol = displayCol;
                handleSelect(displayRow, displayCol);
            } else {
                m_selected.reset();
            }
            return true;
        }

        return false;
    }

    return false;
}

ftxui::Component MakeChessBoardDisplay() {
    return ftxui::Make<ChessBoardDisplay>();
}


}

