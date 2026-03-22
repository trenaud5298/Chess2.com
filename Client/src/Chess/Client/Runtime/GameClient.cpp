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

GameClient::GameClient() : m_startTime(std::chrono::steady_clock::now()), m_loggingManager(*this), m_persistenceManager(*this), m_singleplayerClient(*this), m_multiplayerClient(*this) {
}

GameClient::~GameClient() {

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


void GameClient::startSingleplayer(const SingleplayerConfig& config) {
    if (m_state.load() != ClientState::IDLE) {
        m_loggingManager.log(LogEntry::Warning("startSingleplayer() called from non-IDLE state — ignored"));
        return;
    }

    m_loggingManager.log(LogEntry::Info("Starting singleplayer"));
    m_singleplayerClient.start(config);
    transitionTo(ClientState::SINGLEPLAYER);
}

void GameClient::stopSingleplayer() {
    if (m_state.load() != ClientState::SINGLEPLAYER) {
        m_loggingManager.log(LogEntry::Warning(
            "stopSingleplayer() called from non-SINGLEPLAYER state — ignored"));
        return;
    }

    m_loggingManager.log(LogEntry::Info("Stopping singleplayer"));
    m_singleplayerClient.stop();
    transitionTo(ClientState::IDLE);
}

void GameClient::startMultiplayer(const ServerInfo &server) {
    if (m_state.load() != ClientState::IDLE) {
        m_loggingManager.log(LogEntry::Warning(
            "startMultiplayer() called from non-IDLE state — ignored"));
        return;
    }
    m_loggingManager.log(LogEntry::Info("Starting multiplayer — connecting to " + server.ip));
    transitionTo(ClientState::MULTIPLAYER);
    // m_multiplayerClient.connect(server);
}

void GameClient::stopMultiplayer() {
    if (m_state.load() != ClientState::MULTIPLAYER) {
        m_loggingManager.log(LogEntry::Warning(
            "stopMultiplayer() called from non-MULTIPLAYER state — ignored"));
        return;
    }
    m_loggingManager.log(LogEntry::Info("Stopping multiplayer"));
    // m_multiplayerClient.disconnect();
}

void GameClient::onMultiplayerStopped() {
    if (m_state.load() != ClientState::MULTIPLAYER) { return; }
    m_loggingManager.log(LogEntry::Info("Multiplayer stopped — returning to IDLE"));
    transitionTo(ClientState::IDLE);
}


void GameClient::transitionTo(ClientState newState) {
    m_state = newState;
    m_stateRegistry.fire(newState);
}

}
