/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Server/Runtime/GameServer.hpp>

#include "asio/post.hpp"

// ASIO Includes

// C++ Includes

namespace Chess {

GameServer::GameServer()
: m_initializationStartTime(std::chrono::steady_clock::now()), m_loggingManager(*this), m_persistenceManager(*this), m_loginManager(*this), m_gameManager(*this), m_sessionManager(*this) {
}

GameServer::~GameServer() {

}

void GameServer::start() {
    LifecycleState expectedState = LifecycleState::STOPPED;
    if (!m_state.compare_exchange_strong(expectedState, LifecycleState::STARTING)) {
        return;
    }

    m_loginManager.start();
    m_gameManager.start();
    m_sessionManager.start();

    m_startTime = std::chrono::steady_clock::now();
    ++m_startCount;

    std::size_t threadCount = std::thread::hardware_concurrency();
    threadCount = threadCount > 0 ? threadCount : 1;
    for (std::size_t i = 0; i < threadCount; ++i) {
        m_threads.emplace_back([this]() {
            m_context.run();
        });
    }

    m_loggingManager.log(LogEntry::Info("Server Started"));
    m_state = LifecycleState::RUNNING;
}

void GameServer::stop() {
    LifecycleState expectedState = LifecycleState::RUNNING;
    if (!m_state.compare_exchange_strong(expectedState, LifecycleState::STOPPING)) {
        return;
    }

    m_sessionManager.stop();
    m_gameManager.stop();
    m_loginManager.stop();

    m_stopTime = std::chrono::steady_clock::now();
    m_state = LifecycleState::STOPPED;
    m_loggingManager.log(LogEntry::Info("Server Stopped"));
}



// Stats
std::chrono::steady_clock::time_point GameServer::startTime() const {
    return m_startTime;
}

std::chrono::steady_clock::time_point GameServer::stopTime() const {
    return m_stopTime;
}

std::chrono::milliseconds GameServer::uptime() const {
    if (m_state == LifecycleState::RUNNING) {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - m_startTime
        );
    }
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        m_stopTime-m_startTime
    );
}

std::chrono::milliseconds GameServer::totalUptime() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now()-m_initializationStartTime
    );
}

std::chrono::milliseconds GameServer::totalUptimeAtPoint(std::chrono::steady_clock::time_point point) const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        point-m_initializationStartTime
    );
}

std::uint64_t GameServer::startCount() const {
    return m_startCount;
}

std::size_t GameServer::threadCount() const {
    return m_threads.size();
}

}
