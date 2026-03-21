#ifndef CHESS_CLIENT_UI_MULTIPLAYERSCREEN_HPP
#define CHESS_CLIENT_UI_MULTIPLAYERSCREEN_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/ScreenInterface.hpp>
#include <Chess/Core/UI/ChessBoardDisplay.hpp>
#include <CHess/Core/Game/Board.hpp>

// ASIO Includes

// FTXUI Includes
#include <ftxui/component/component.hpp>

// C++ Includes


namespace Chess {

class ClientPanel;

class MultiplayerScreen : public ScreenInterface {
public:
    explicit MultiplayerScreen(ClientPanel& clientPanel);
    ~MultiplayerScreen();
    ftxui::Component getComponent() override;
    void onEnter() override;
    void onLeave() override;

private:
    ClientPanel& m_clientPanel;
    ftxui::Component m_component;

    Board m_testBoard;
    std::shared_ptr<ChessBoardDisplay> m_boardDisplay;

    static constexpr Screen SCREEN_TYPE = Screen::Multiplayer;
};

}


#endif