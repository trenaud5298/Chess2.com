/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Server/UI/StatusTab.hpp>
#include <Chess/Server/Runtime/GameServer.hpp>
#include <Chess/Server/Common/TimeFormat.hpp>

// FTXUI Includes
#include <ftxui/dom/elements.hpp>

// ASIO Includes

// C++ Includes
#include <iostream>



namespace Chess {

StatusTab::StatusTab(GameServer& gameServer) : m_gameServer(gameServer) {
    build();
}

ftxui::Component StatusTab::getComponent() {
    return m_component;
}

void StatusTab::build() {
    m_infoComponent = ftxui::Renderer([&]() {
        std::string serverStatus = "Unknown";
        switch (m_gameServer.state()) {
            case LifecycleState::STOPPED:
                serverStatus = "Stopped";
                break;
            case LifecycleState::STARTING:
                serverStatus = "Starting";
                break;
            case LifecycleState::RUNNING:
                serverStatus = "Running";
                break;
            case LifecycleState::STOPPING:
                serverStatus = "Stopping";
                break;
        }
        std::size_t serverSessions = m_gameServer.sessionManager().sessionCount();
        std::string serverUptime = formatHHMMSS(m_gameServer.uptime());
        return ftxui::vbox({
            ftxui::text("Server Info") | ftxui::bold | ftxui::center,
            ftxui::text("Status: " + serverStatus) | ftxui::bold | ftxui::center,
            ftxui::text("Sessions: " + std::to_string(serverSessions)) | ftxui::bold | ftxui::center,
            ftxui::text("Uptime: " + serverUptime) | ftxui::bold | ftxui::center
        });
    });

    m_startButton = ftxui::Button("Start", [&] {
        m_gameServer.start();
    });
    m_stopButton = ftxui::Button("Stop", [&] {
        m_gameServer.stop();
    });
    m_buttons = ftxui::Container::Horizontal({m_startButton, m_stopButton});
    m_controlComponent = ftxui::Renderer(m_buttons, [&]() {
        return ftxui::vbox({
            ftxui::text("Server Control") | ftxui::bold | ftxui::center,
            m_buttons->Render(),
        }) | ftxui::flex;
    });

    m_component = ftxui::Renderer(m_controlComponent, [&](){
        return ftxui::hbox({
            m_infoComponent->Render() | ftxui::flex_grow,
            ftxui::separator() | ftxui::yflex_grow,
            m_controlComponent->Render() | ftxui::flex_grow
        });
    });
}



}