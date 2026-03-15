#ifndef CHESS_SERVER_UI_STATUSTAB_HPP
#define CHESS_SERVER_UI_STATUSTAB_HPP
#include "ftxui/component/loop.hpp"

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes

// ASIO Includes

// FTXUI Includes
#include <ftxui/component/component.hpp>

// C++ Includes


namespace Chess {

class GameServer;

class StatusTab {

public:
    explicit StatusTab(GameServer& gameServer);
    ftxui::Component getComponent();

private:
    void build();

    // Game Server
    GameServer& m_gameServer;

    // Left Hand Side Component With General Info
    ftxui::Component m_infoComponent;

    // Right Hand Side Component With Control Buttons
    ftxui::Component m_controlComponent;
        ftxui::Component m_startButton;
        ftxui::Component m_stopButton;
        ftxui::Component m_buttons;

    // Overall Component
    ftxui::Component m_component;
};

}


#endif