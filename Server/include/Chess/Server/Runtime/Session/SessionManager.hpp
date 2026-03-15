#ifndef CHESS_SERVER_RUNTIME_SESSION_SESSIONMANAGER_HPP
#define CHESS_SERVER_RUNTIME_SESSION_SESSIONMANAGER_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Server/Runtime/Common/LifecycleState.hpp>

// ASIO Includes

// C++ Includes
#include <atomic>

namespace Chess {

class GameServer;

class SessionManager {
public:
    explicit SessionManager(GameServer& gameServer);
    ~SessionManager();

    SessionManager(const SessionManager&) = delete;
    SessionManager& operator=(const SessionManager&) = delete;
    SessionManager(SessionManager&&) = delete;
    SessionManager& operator=(SessionManager&&) = delete;

    // Lifetime Control
    void start();
    void stop();

private:
    GameServer& m_gameServer;
    std::atomic<LifecycleState> m_state{LifecycleState::STOPPED};
};

}

#endif