#ifndef CHESS_CORE_UI_CHESSBOARDDISPLAY_HPP
#define CHESS_CORE_UI_CHESSBOARDDISPLAY_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Core/declarations.hpp>
#include <Chess/Core/UI/BoardTheme.hpp>

// ASIO Includes

// FTXUI Includes
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>

// C++ Includes
#include <array>
#include <functional>
#include <optional>
#include <string>

namespace Chess {

class ChessBoardDisplay : public ftxui::ComponentBase {

public:
    ChessBoardDisplay();

    void updateBoard(const std::array<ID, 64>& board);
    void setFlipped(bool flipped);
    void setTheme(const BoardTheme& theme);

    std::function<void(Pos from, Pos to, bool promotion)> onMove;

    ftxui::Element OnRender() override;
    bool OnEvent(ftxui::Event e) override;
    bool Focusable() const override { return true; }

private:
    struct RenderMetrics {
        int cellWidth{6};
        int cellHeight{3};
    };

private:
    // Helpers
    [[nodiscard]] Pos screenToBoard(int displayRow, int displayCol) const;
    [[nodiscard]] ID getAt(int boardRow, int boardCol) const;
    [[nodiscard]] bool isEmpty(ID id) const;
    [[nodiscard]] bool isWhitePiece(ID id) const;
    [[nodiscard]] static std::string toGlyph(ID id);
    [[nodiscard]] static bool isWhitePawn(ID id);
    [[nodiscard]] static bool isBlackPawn(ID id);
    [[nodiscard]] static bool requiresPromotion(ID id, const Pos& to);

    // Event Handling Functions
    void handleSelect(int displayRow, int displayCol);
    void moveCursor(int dRow, int dCol);

    [[nodiscard]] RenderMetrics computeRenderMetrics() const;
    [[nodiscard]] RenderMetrics renderedMetrics() const;
    [[nodiscard]] int boardWidth(const RenderMetrics& metrics) const;
    [[nodiscard]] int boardHeight(const RenderMetrics& metrics) const;

    // Rendering Functions
    ftxui::Element renderCell(int displayRow, int displayCol, const RenderMetrics& metrics) const;
    ftxui::Element renderBoard(const RenderMetrics& metrics) const;

private:
    std::array<ID, 64> m_board{};
    int m_cursorRow{0};
    int m_cursorCol{0};
    std::optional<Pos> m_selected;
    bool m_flipped{false};
    BoardTheme m_theme{};

    ftxui::Box m_outerBox;
    ftxui::Box m_boardBox;

    static constexpr int RANK_LABEL_WIDTH = 2;
    static constexpr int FILE_LABEL_HEIGHT = 1;
    static constexpr int DEFAULT_CELL_WIDTH = 6;
    static constexpr int DEFAULT_CELL_HEIGHT = 3;
};

// Ftxui Style Helper
ftxui::Component MakeChessBoardDisplay();

}

#endif