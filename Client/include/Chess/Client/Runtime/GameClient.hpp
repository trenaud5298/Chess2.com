#ifndef CHESS_CLIENT_RUNTIME_GAMECLIENT_HPP
#define CHESS_CLIENT_RUNTIME_GAMECLIENT_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Core/Common/LifecycleState.hpp>
#include <Chess/Client/Runtime/Logging/LoggingManager.hpp>
#include <Chess/Client/Runtime/Persistence/PersistenceManager.hpp>

// ASIO Includes
#include <asio/io_context.hpp>

// C++ Includes
#include <memory>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <cstdint>

namespace Chess {


class GameClient {
public:
    GameClient();
    ~GameClient();

    GameClient(const GameClient&) = delete;
    GameClient& operator=(const GameClient&) = delete;
    GameClient(GameClient&&) = delete;
    GameClient& operator=(GameClient&&) = delete;

    // Lifetime Control
    void start();
    void stop();

    // Subsystems
    LoggingManager& loggingManager() {return m_loggingManager;}
    PersistenceManager& persistenceManager() {return m_persistenceManager;}
    [[nodiscard]] const LoggingManager& loggingManager() const {return m_loggingManager;}
    [[nodiscard]] const PersistenceManager& persistenceManager() const {return m_persistenceManager;}

    // Other Accessors
    [[nodiscard]] asio::io_context& ioContext() {return m_context;}
    [[nodiscard]] const asio::io_context& ioContext() const {return m_context;}
    [[nodiscard]] LifecycleState state() const {return m_state;}

    // Client Level Stats
    [[nodiscard]] std::chrono::steady_clock::time_point startTime() const;

    [[nodiscard]] std::chrono::milliseconds uptimeCurrent() const;
    [[nodiscard]] std::chrono::milliseconds uptimeAtPoint(std::chrono::steady_clock::time_point point) const;


private:
    // Core Client System
    asio::io_context m_context;
    std::atomic<LifecycleState> m_state{LifecycleState::STOPPED};
    std::chrono::steady_clock::time_point m_startTime;

    // Subsystems
    LoggingManager m_loggingManager;
    PersistenceManager m_persistenceManager;
};
}

#endif