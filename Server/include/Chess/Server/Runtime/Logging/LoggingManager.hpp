#ifndef CHESS_SERVER_RUNTIME_LOGGING_LOGGINGMANAGER_HPP
#define CHESS_SERVER_RUNTIME_LOGGING_LOGGINGMANAGER_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Server/Runtime/Common/LifecycleState.hpp>
#include <Chess/Server/Runtime/Logging/LogHandler.hpp>

// ASIO Includes

// C++ Includes
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <vector>
#include <cstdint>

namespace Chess {

class GameServer;

class LoggingManager {
public:
    explicit LoggingManager(GameServer& gameServer);
    ~LoggingManager();

    LoggingManager(const LoggingManager&) = delete;
    LoggingManager& operator=(const LoggingManager&) = delete;
    LoggingManager(LoggingManager&&) = delete;
    LoggingManager& operator=(LoggingManager&&) = delete;

    // Log
    void log(const LogEntry& entry);

    // Log Handlers
    std::uint64_t addHandler(std::uint64_t mask, std::function<void(const LogEntry&)> handler);
    bool removeHandler(std::uint64_t id);

private:
    GameServer& m_gameServer;

    std::vector<LogHandler> m_handlers;
    std::shared_mutex m_handlerMutex;
    std::uint64_t m_nextHandlerID{1};
};


}

#endif