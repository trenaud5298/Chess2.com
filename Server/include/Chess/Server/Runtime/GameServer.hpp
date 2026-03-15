#ifndef CHESS_SERVER_RUNTIME_GAMESERVER_HPP
#define CHESS_SERVER_RUNTIME_GAMESERVER_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Server/Runtime/Common/LifecycleState.hpp>
#include <Chess/Server/Runtime/Logging/LoggingManager.hpp>
#include <Chess/Server/Runtime/Persistence/PersistenceManager.hpp>
#include <Chess/Server/Runtime/Login/LoginManager.hpp>
#include <Chess/Server/Runtime/Game/GameManager.hpp>
#include <Chess/Server/Runtime/Session/SessionManager.hpp>

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


class GameServer {
public:
    GameServer();
    ~GameServer();

    GameServer(const GameServer&) = delete;
    GameServer& operator=(const GameServer&) = delete;
    GameServer(GameServer&&) = delete;
    GameServer& operator=(GameServer&&) = delete;

    // Lifetime Control
    void start();
    void stop();

    // Subsystems
    LoggingManager& loggingManager() {return m_loggingManager;}
    PersistenceManager& persistenceManager() {return m_persistenceManager;}
    LoginManager& loginManager() {return m_loginManager;}
    GameManager& gameManager() {return m_gameManager;}
    SessionManager& sessionManager() {return m_sessionManager;}
    [[nodiscard]] const LoggingManager& loggingManager() const {return m_loggingManager;}
    [[nodiscard]] const PersistenceManager& persistenceManager() const {return m_persistenceManager;}
    [[nodiscard]] const LoginManager& loginManager() const {return m_loginManager;}
    [[nodiscard]] const GameManager& gameManager() const {return m_gameManager;}
    [[nodiscard]] const SessionManager& sessionManager() const {return m_sessionManager;}

    // Other Accessors
    [[nodiscard]] asio::io_context& ioContext() {return m_context;}
    [[nodiscard]] const asio::io_context& ioContext() const {return m_context;}
    [[nodiscard]] LifecycleState state() const {return m_state;}

    // Server Level Stats
    [[nodiscard]] std::chrono::steady_clock::time_point startTime() const;
    [[nodiscard]] std::chrono::steady_clock::time_point stopTime() const;

    [[nodiscard]] std::chrono::milliseconds uptime() const;
    [[nodiscard]] std::chrono::milliseconds totalUptime() const;
    [[nodiscard]] std::chrono::milliseconds totalUptimeAtPoint(std::chrono::steady_clock::time_point point) const;

    [[nodiscard]] std::uint64_t startCount() const;
    [[nodiscard]] std::size_t threadCount() const;

private:
    // Core Server System
    asio::io_context m_context;
    std::vector<std::thread> m_threads;
    std::atomic<LifecycleState> m_state{LifecycleState::STOPPED};
    std::chrono::steady_clock::time_point m_initializationStartTime;
    std::chrono::steady_clock::time_point m_startTime{};
    std::chrono::steady_clock::time_point m_stopTime{};
    std::atomic<std::uint64_t> m_startCount{0};

    // Subsystems
    LoggingManager m_loggingManager;
    PersistenceManager m_persistenceManager;
    LoginManager m_loginManager;
    GameManager m_gameManager;
    SessionManager m_sessionManager;
};
}

#endif