/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Screen/MultiplayerGameScreen.hpp>
#include <Chess/Client/UI/ClientPanel.hpp>

// FTXUI Includes

// C++ Includes

namespace Chess {

MultiplayerGameScreen::MultiplayerGameScreen(ClientPanel& clientPanel)
: ScreenInterface(clientPanel) {
    m_component = ftxui::Renderer([] {
        return ftxui::text("MultiplayerGameScreen");
    });
}

MultiplayerGameScreen::~MultiplayerGameScreen() {}

ftxui::Component MultiplayerGameScreen::getComponent() {
    return m_component;
}



} // namespace Chess