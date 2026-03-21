/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */


// Chess Includes
#include <Chess/Client/Runtime/GameClient.hpp>

// ASIO Includes

// C++ Includes


namespace Chess {

GameClient::GameClient() : m_startTime(std::chrono::steady_clock::now()), m_loggingManager(*this), m_persistenceManager(*this) {
}

GameClient::~GameClient() {

}

void GameClient::start() {

}

void GameClient::stop() {

}

std::chrono::steady_clock::time_point GameClient::startTime() const {
    return m_startTime;
}

std::chrono::milliseconds GameClient::uptimeCurrent() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - m_startTime
    );
}

std::chrono::milliseconds GameClient::uptimeAtPoint(std::chrono::steady_clock::time_point point) const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        point - m_startTime
    );
}

}
