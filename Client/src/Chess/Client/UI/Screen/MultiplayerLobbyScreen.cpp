/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Screen/MultiplayerLobbyScreen.hpp>
#include <Chess/Client/UI/ClientPanel.hpp>

// FTXUI Includes

// C++ Includes

namespace Chess {

MultiplayerLobbyScreen::MultiplayerLobbyScreen(ClientPanel& clientPanel)
: ScreenInterface(clientPanel) {
    m_component = ftxui::Renderer([] {
        return ftxui::text("MultiplayerLobbyScreen");
    });
}

MultiplayerLobbyScreen::~MultiplayerLobbyScreen() {}

ftxui::Component MultiplayerLobbyScreen::getComponent() {
    return m_component;
}


} // namespace Chess