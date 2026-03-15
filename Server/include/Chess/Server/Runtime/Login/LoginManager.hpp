#ifndef CHESS_SERVER_RUNTIME_LOGIN_LOGINMANAGER_HPP
#define CHESS_SERVER_RUNTIME_LOGIN_LOGINMANAGER_HPP

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

class LoginManager {
public:
    explicit LoginManager(GameServer& gameServer);
    ~LoginManager();

    LoginManager(const LoginManager&) = delete;
    LoginManager& operator=(const LoginManager&) = delete;
    LoginManager(LoginManager&&) = delete;
    LoginManager& operator=(LoginManager&&) = delete;

    // Lifetime Control
    void start();
    void stop();

private:
    GameServer& m_gameServer;
    std::atomic<LifecycleState> m_state{LifecycleState::STOPPED};
};

}

#endif