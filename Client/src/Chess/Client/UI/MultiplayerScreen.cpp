/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/MultiplayerScreen.hpp>
#include <Chess/Client/UI/ClientPanel.hpp>

// FTXUI Includes
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

// C++ Includes

namespace Chess {

MultiplayerScreen::MultiplayerScreen(ClientPanel& clientPanel)
: m_clientPanel(clientPanel) {
    auto boardComponent = ftxui::Make<ChessBoardDisplay>();
    m_boardDisplay = boardComponent;

    m_boardDisplay->onMove = [&](Pos from, Pos to) {
        const auto& raw = m_testBoard.getBoard();
        ID id = raw[from[ROW] * 8 + from[COL]];

        // Guard: don't call move with an empty square
        // (shouldn't happen given handleSelect's guard, but be safe)
        if (id == ID::EMPTY) return;

        m_testBoard.move(id, to);
        m_boardDisplay->updateBoard(m_testBoard.getBoard());
    };

    m_boardDisplay->updateBoard(m_testBoard.getBoard());
    auto backButton = ftxui::Button("  Back  ", [&] {
        m_clientPanel.navigateBack();
    }, ftxui::ButtonOption::Animated());

    auto layout = ftxui::Container::Vertical({
        boardComponent,
        backButton,
    });

    m_component = ftxui::Renderer(layout, [&, boardComponent, backButton] {
        return ftxui::vbox({
            // Title bar
            ftxui::text("Single Player") | ftxui::bold | ftxui::center,
            ftxui::separator(),

            // Board centered in remaining space
            ftxui::vbox({
                ftxui::filler(),
                ftxui::hbox({
                    ftxui::filler(),
                    boardComponent->Render(),
                    ftxui::filler(),
                }),
                ftxui::filler(),
            }) | ftxui::flex,

            // Footer
            ftxui::separator(),
            ftxui::hbox({
                ftxui::filler(),
                backButton->Render(),
            }),
        });
    });
}

MultiplayerScreen::~MultiplayerScreen() {}

ftxui::Component MultiplayerScreen::getComponent() {
    return m_component;
}

void MultiplayerScreen::onEnter() {}

void MultiplayerScreen::onLeave() {}

} // namespace Chess