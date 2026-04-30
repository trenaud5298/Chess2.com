#ifndef CHESS_SERVER_UI_LOGTAB_HPP
#define CHESS_SERVER_UI_LOGTAB_HPP

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
#include <deque>
#include <cstdint>
#include <mutex>


namespace Chess {

class GameServer;
class ScrollableListView;
struct LogEntry;

class LogTab {

public:
    explicit LogTab(GameServer& gameServer);
    ftxui::Component getComponent();
    void log(const LogEntry& entry);

private:
    void build();

    // GameServer
    GameServer& m_gameServer;

    // Log Data
    static constexpr std::size_t MAX_LOG_LINES = 1000;
    mutable std::mutex m_logLinesMutex;
    std::deque<std::string> m_logLines;

    // UI Components
    std::shared_ptr<ScrollableListView> m_logView;
    ftxui::Component m_component;

    std::uint64_t m_handlerID;
};

}


#endif