/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Screen/MultiplayerSelectScreen.hpp>
#include <Chess/Client/UI/ClientPanel.hpp>

// FTXUI Includes

// C++ Includes

namespace Chess {

MultiplayerSelectScreen::MultiplayerSelectScreen(ClientPanel& clientPanel)
: ScreenInterface(clientPanel) {
    m_component = ftxui::Renderer([] {
        return ftxui::text("MultiplayerSelectScreen");
    });
}

MultiplayerSelectScreen::~MultiplayerSelectScreen() {}

ftxui::Component MultiplayerSelectScreen::getComponent() {
    return m_component;
}

void MultiplayerSelectScreen::onEnter() {}

void MultiplayerSelectScreen::onLeave() {}

void MultiplayerSelectScreen::onResume() {}

bool MultiplayerSelectScreen::onBackRequested() { return false; }

} // namespace Chess