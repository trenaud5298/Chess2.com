/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Server/UI/LogTab.hpp>
#include <Chess/Core/UI/ScrollableTextWindow.hpp>
#include <Chess/Server/Runtime/GameServer.hpp>
#include <Chess/Server/Common/TimeFormat.hpp>

// FTXUI Includes
#include <ftxui/component/event.hpp>

// C++ Includes
#include <format>

namespace Chess {


// Log Tab
LogTab::LogTab(GameServer& gameServer) : m_gameServer(gameServer){
    build();
    m_handlerID = m_gameServer.loggingManager().addHandler(LogType::LOG_ALL, std::bind_front(&LogTab::log, this));
}

ftxui::Component LogTab::getComponent() {
    return m_component;
}

void LogTab::build() {
    m_logWindow = std::make_shared<ScrollableTextWindow>();
    m_logWindow->addText("Test 1");

    m_logOptions = ftxui::Button("Test", [&]() {

    });
    m_component = ftxui::Renderer(ftxui::Container::Horizontal({m_logWindow, m_logOptions}), [&]() {
        return ftxui::hbox({
            m_logWindow->Render() | ftxui::yframe | ftxui::flex,
            ftxui::separator(),
            m_logOptions->Render(),
        });
    });
}

void LogTab::log(const LogEntry& entry) {
    m_logWindow->addText(std::format("[{}][{}] {}",
        formatHHMMSS(m_gameServer.totalUptimeAtPoint(entry.timestamp)),
        logTypeAsString(entry.type),
        entry.message
    ));
}

}
