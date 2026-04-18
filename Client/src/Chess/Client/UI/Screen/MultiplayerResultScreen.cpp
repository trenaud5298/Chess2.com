/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Screen/MultiplayerResultScreen.hpp>
#include <Chess/Client/UI/ClientPanel.hpp>

// FTXUI Includes

// C++ Includes

namespace Chess {

MultiplayerResultScreen::MultiplayerResultScreen(ClientPanel& clientPanel)
: ScreenInterface(clientPanel) {
    m_component = ftxui::Renderer([] {
        return ftxui::text("MultiplayerResultScreen");
    });
}

MultiplayerResultScreen::~MultiplayerResultScreen() {}


void MultiplayerResultScreen::onEnter() {

}

void MultiplayerResultScreen::requestExit() {
    m_clientPanel.handleStatus(
        m_clientPanel.gameClient().returnToIdle(),
        "Unable to return to main menu"
    );
}

ftxui::Component MultiplayerResultScreen::getComponent() {
    return m_component;
}


} // namespace Chess