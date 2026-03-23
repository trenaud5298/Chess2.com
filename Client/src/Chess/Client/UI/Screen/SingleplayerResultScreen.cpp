/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Screen/SingleplayerResultScreen.hpp>
#include <Chess/Client/UI/ClientPanel.hpp>

// FTXUI Includes

// C++ Includes

namespace Chess {

SingleplayerResultScreen::SingleplayerResultScreen(ClientPanel& clientPanel)
: ScreenInterface(clientPanel) {
    m_component = ftxui::Renderer([] {
        return ftxui::text("SingleplayerResultScreen");
    });
}

SingleplayerResultScreen::~SingleplayerResultScreen() {}

ftxui::Component SingleplayerResultScreen::getComponent() {
    return m_component;
}



} // namespace Chess