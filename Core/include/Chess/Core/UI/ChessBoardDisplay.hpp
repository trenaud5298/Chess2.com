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

// ASIO Includes

// FTXUI Includes
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>

// C++ Includes
#include <array>
#include <functional>
#include <optional>


namespace Chess {

class ChessBoardDisplay : public ftxui::ComponentBase {

public:
    ChessBoardDisplay();

    void updateBoard(const std::array<ID, 64>& board);
    void setFlipped(bool flipped);
    void setCellSize(int width, int height);
    std::function<void(ID from, Pos to)> onMove;

    ftxui::Element OnRender() override;
    bool OnEvent(ftxui::Event e) override;
    bool Focusable() const override { return true; }


private:
    // Helpers
    Pos screenToBoard(int displayRow, int displayCol) const;
    ID getAt(int boardRow, int boardCol) const;
    bool isEmpty(ID id) const;
    bool isWhitePiece(ID id) const;
    static std::string toGlyph(ID id);

    // Event Handling Functions
    void handleSelect(int displayRow, int displayCol);
    void moveCursor(int dRow, int dCol);

    // Rendering Functions
    ftxui::Element renderBoard() const;
    ftxui::Element renderCell(int displayRow, int displayCol) const;

private:
    std::array<ID, 64> m_board{};
    int m_cursorRow{0};
    int m_cursorCol{0};
    std::optional<Pos> m_selected;
    bool m_flipped{false};
    ftxui::Box m_box;
    int m_cellWidth{6};
    int m_cellHeight{3};

    static constexpr int RANK_LABEL_W = 2;
};

// Ftxui Style Helper
ftxui::Component MakeChessBoardDisplay();

}

#endif