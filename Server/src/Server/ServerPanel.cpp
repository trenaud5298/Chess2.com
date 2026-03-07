/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <iostream>
#include <Chess/Server/ServerPanel.hpp>
#include <Chess/Server/GameServer.hpp>

// ASIO Includes

// C++ Includes



namespace Chess {
ServerPanel::ServerPanel()
: m_screen(ftxui::ScreenInteractive::FullscreenAlternateScreen()), m_gameServer(std::make_unique<GameServer>(GameServerConfig())),
m_statusTab(m_gameServer.get()), m_logTab(m_gameServer.get()) {
    initializeUI();
}


ServerPanel::~ServerPanel() {

}

void ServerPanel::run() {
    try {
        m_screen.Loop(m_mainComponent);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

void ServerPanel::initializeUI() {
    // Tab UI
    m_tabTitles = {"Status", "Log"};
    m_toggle = ftxui::Toggle(&m_tabTitles, &m_selectedTab);
    m_tabContent = ftxui::Container::Tab({m_statusTab.getComponent(), m_logTab.getComponent()}, &m_selectedTab);
    m_tabEvent = ftxui::CatchEvent(ftxui::Container::Vertical({m_toggle, m_tabContent}), [&](ftxui::Event event) {
        if (event.is_character()) {
            const std::string& str = event.character();
            if (!str.empty() && std::isdigit(str[0])) {
                int value = str[0] - '0';
                if (value >= 1 && value <= m_tabTitles.size()) {
                    m_selectedTab = value - 1;
                    return true;
                }
            }
        }
        return false;
    });
    m_tabRenderer = ftxui::Renderer(m_tabEvent, [&]() {
        return ftxui::vbox({
            m_toggle->Render(),
            ftxui::separator(),
            m_tabContent->Render()
        });
    });

    // Command Input
    m_commandBuffer.clear();
    m_commandInput = ftxui::Input(&m_commandBuffer, "command");
    m_commandEvent = ftxui::CatchEvent(m_commandInput, [&](ftxui::Event event) {
        if (event == ftxui::Event::Return) {
            m_logTab.log(m_commandBuffer);
            m_commandBuffer.clear();
            return true;
        }
        return false;
    });
    m_commandRenderer = ftxui::Renderer(m_commandEvent, [&]() {
        return ftxui::vbox({
            ftxui::separator(),
            m_commandEvent->Render()
        });
    });

    m_mainEventCatcher = ftxui::CatchEvent(ftxui::Container::Vertical({m_tabRenderer, m_commandRenderer}), [&](ftxui::Event event) {
        return false;
    });
    m_mainComponent = ftxui::Renderer(m_mainEventCatcher, [&]() {
        auto start = std::chrono::high_resolution_clock::now();
        ftxui::Element result = ftxui::vbox({
            ftxui::text("Server Control Panel") | ftxui::bold | ftxui::center,
            m_tabRenderer->Render() | ftxui::flex,
            m_commandRenderer->Render()
        }) | ftxui::border;
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end-start);
        std::string timeLog = std::to_string(elapsed.count());
        m_logTab.log(timeLog);
        return result;
    });
}

}
