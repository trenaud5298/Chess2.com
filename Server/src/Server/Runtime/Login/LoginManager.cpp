/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Server/Runtime/Login/LoginManager.hpp>
#include <Chess/Server/Runtime/GameServer.hpp>

// ASIO Includes

// C++ Includes

namespace Chess {

LoginManager::LoginManager(GameServer& gameServer) : m_gameServer(gameServer) {

}

LoginManager::~LoginManager() {

}

void LoginManager::start() {
    LifecycleState expectedState = LifecycleState::STOPPED;
    if (!m_state.compare_exchange_strong(expectedState, LifecycleState::STARTING)) {
        return;
    }

    m_gameServer.loggingManager().log(LogEntry::Info("Login Manager Started"));

    m_state = LifecycleState::RUNNING;
}

void LoginManager::stop() {
    LifecycleState expectedState = LifecycleState::RUNNING;
    if (!m_state.compare_exchange_strong(expectedState, LifecycleState::STOPPING)) {
        return;
    }



    m_state = LifecycleState::STOPPED;
    m_gameServer.loggingManager().log(LogEntry::Info("Login Manager Stopped"));
}


}
