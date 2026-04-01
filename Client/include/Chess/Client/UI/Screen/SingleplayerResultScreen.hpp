#ifndef CHESS_CLIENT_UI_SINGLEPLAYERRESULTSCREEN_HPP
#define CHESS_CLIENT_UI_SINGLEPLAYERRESULTSCREEN_HPP


/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Screen/ScreenInterface.hpp>

// ASIO Includes

// FTXUI Includes
#include <ftxui/component/component.hpp>

// C++ Includes
#include <cstdint>

namespace Chess {

class ClientPanel;

class SingleplayerResultScreen : public ScreenInterface {
public:
    explicit SingleplayerResultScreen(ClientPanel& clientPanel);
    ~SingleplayerResultScreen();
    ftxui::Component getComponent() override;

private:
    ftxui::Component m_component;
    static constexpr Screen SCREEN_TYPE = Screen::Singleplayer_Result;
};

}
#endif