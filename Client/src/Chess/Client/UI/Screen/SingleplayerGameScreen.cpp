/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Screen/SingleplayerGameScreen.hpp>
#include <Chess/Client/UI/ClientPanel.hpp>

// FTXUI Includes

// C++ Includes

namespace Chess {

SingleplayerGameScreen::SingleplayerGameScreen(ClientPanel& clientPanel) : ScreenInterface(clientPanel) {


    m_component = ftxui::Renderer([] {
        return ftxui::text("SingleplayerGameScreen");
    });
}

SingleplayerGameScreen::~SingleplayerGameScreen() {}

ftxui::Component SingleplayerGameScreen::getComponent() {
    return m_component;
}

void SingleplayerGameScreen::onEnter() {}

void SingleplayerGameScreen::onLeave() {}

void SingleplayerGameScreen::onResume() {}

bool SingleplayerGameScreen::onBackRequested() { return false; }

} // namespace Chess