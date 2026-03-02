#ifndef CHESS_SERVER_SERVERPANEL_HPP
#define CHESS_SERVER_SERVERPANEL_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Server/Panel/StatusTab.hpp>
#include <Chess/Server/Panel/LogTab.hpp>

// ASIO Includes

// FTXUI Includes
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

// C++ Includes
#include <memory>
#include <string>

#include "Panel/LogTab.hpp"

namespace Chess {

class GameServer;

class ServerPanel {

public:
    ServerPanel();
    ~ServerPanel();

    void run();

private:
    void initializeUI();

    // Game Server
    std::unique_ptr<GameServer> m_gameServer;

    // FTXUI Screen
    ftxui::ScreenInteractive m_screen;

    // Tabs
    StatusTab m_statusTab;
    LogTab m_logTab;

    // Tab UI
    int m_selectedTab = 0;
    std::vector<std::string> m_tabTitles;
    ftxui::Component m_toggle;
    ftxui::Component m_tabContent;
    ftxui::Component m_tabEvent;
    ftxui::Component m_tabRenderer;

    // Command Input UI
    std::string m_commandBuffer;
    ftxui::Component m_commandInput;
    ftxui::Component m_commandEvent;
    ftxui::Component m_commandRenderer;
    bool m_commandVisible = false;

    ftxui::Component m_mainEventCatcher;
    ftxui::Component m_mainComponent;
};



}


#endif