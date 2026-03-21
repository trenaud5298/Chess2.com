#ifndef CHESS_CLIENT_UI_SINGLEPLAYERSCREEN_HPP
#define CHESS_CLIENT_UI_SINGLEPLAYERSCREEN_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/ScreenInterface.hpp>
#include <Chess/Core/Game/Board.hpp>
#include <Chess/Core/UI/ChessBoardDisplay.hpp>

// ASIO Includes

// FTXUI Includes
#include <ftxui/component/component.hpp>

// C++ Includes


namespace Chess {

class ClientPanel;

class SingleplayerScreen : public ScreenInterface {
public:
    explicit SingleplayerScreen(ClientPanel& clientPanel);
    ~SingleplayerScreen();
    ftxui::Component getComponent() override;
    void onEnter() override;
    void onLeave() override;

private:
    ClientPanel& m_clientPanel;
    ftxui::Component m_component;

    static constexpr Screen SCREEN_TYPE = Screen::Singleplayer;
};

}


#endif