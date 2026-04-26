/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Server/UI/LogTab.hpp>
#include <Chess/Core/UI/ScrollableListView.hpp>
#include <Chess/Server/Runtime/GameServer.hpp>
#include <Chess/Core/Common/TimeFormat.hpp>

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
    m_logView = std::make_shared<ScrollableListView>(
        [this]() {
            std::lock_guard lock(m_logLinesMutex);
            return static_cast<int>(m_logLines.size());
        },
        [this](int index) {
            std::lock_guard lock(m_logLinesMutex);
            return ftxui::text(m_logLines[index]);
        }
    );

    m_logOptions = ftxui::Button("Test", [&]() {

    });

    auto layout = ftxui::Container::Horizontal({
        m_logView,
        m_logOptions
    });

    m_component = ftxui::Renderer(layout, [this]() {
        return ftxui::hbox({
            m_logView->Render() | ftxui::flex | ftxui::yflex,
            ftxui::separator(),
            m_logOptions->Render() | ftxui::yflex,
        }) | ftxui::flex;
    });
}

void LogTab::log(const LogEntry& entry) {
    {
        std::lock_guard lock(m_logLinesMutex);
        m_logLines.push_back(std::format("[{}][{}] {}",
            formatHHMMSS(m_gameServer.totalUptimeAtPoint(entry.timestamp)),
            logTypeAsString(entry.type),
            entry.message
        ));

        if (m_logLines.size() > MAX_LOG_LINES) {
            m_logLines.pop_front();
        }
    }

}

}
