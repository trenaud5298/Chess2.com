/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Server/Runtime/Game/GameManager.hpp>
#include <Chess/Server/Runtime/GameServer.hpp>

// ASIO Includes

// C++ Includes

namespace Chess {

GameManager::GameManager(GameServer& gameServer) : m_gameServer(gameServer) {

}

GameManager::~GameManager() {

}

void GameManager::start() {
    LifecycleState expectedState = LifecycleState::STOPPED;
    if (!m_state.compare_exchange_strong(expectedState, LifecycleState::STARTING)) {
        return;
    }

    m_gameServer.loggingManager().log(LogEntry::Info("Game Manager Started"));


    m_state = LifecycleState::RUNNING;
}

void GameManager::stop() {
    LifecycleState expectedState = LifecycleState::RUNNING;
    if (!m_state.compare_exchange_strong(expectedState, LifecycleState::STOPPING)) {
        return;
    }


    m_state = LifecycleState::STOPPED;
    m_gameServer.loggingManager().log(LogEntry::Info("Game Manager Stopped"));
}


}


