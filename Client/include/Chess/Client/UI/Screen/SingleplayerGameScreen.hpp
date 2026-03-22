#ifndef CHESS_CLIENT_UI_SINGLEPLAYERGAMESCREEN_HPP
#define CHESS_CLIENT_UI_SINGLEPLAYERGAMESCREEN_HPP


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

class SingleplayerGameScreen : public ScreenInterface {
public:
    explicit SingleplayerGameScreen(ClientPanel& clientPanel);
    ~SingleplayerGameScreen();
    ftxui::Component getComponent() override;
    void onEnter() override;
    void onLeave() override;
    void onResume() override;
    bool onBackRequested() override;

private:
    ;
    ftxui::Component m_component;
    static constexpr Screen SCREEN_TYPE = Screen::Singleplayer_Game;
};

}
#endif