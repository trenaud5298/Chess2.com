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
#include <string>


namespace Chess {

namespace {

std::string toString(ClientMode mode) {
    switch (mode) {
        case ClientMode::Idle: return "Idle";
        case ClientMode::Singleplayer: return "Singleplayer";
        case ClientMode::Multiplayer: return "Multiplayer";
        default: return "Unknown";
    }
}

std::string toString(ClientPhase phase) {
    switch (phase) {
        case ClientPhase::Idle: return "Idle";
        case ClientPhase::Starting: return "Starting";
        case ClientPhase::Active: return "Active";
        case ClientPhase::Paused: return "Paused";
        case ClientPhase::Stopping: return "Stopping";
        case ClientPhase::Error: return "Error";
        default: return "Unknown";
    }
}

}

GameClient::GameClient() : m_startTime(std::chrono::steady_clock::now()), m_loggingManager(*this), m_persistenceManager(*this), m_singleplayerClient(*this), m_multiplayerClient(*this) {
    m_loggingManager.log(LogEntry::Info("GameClient initialized: mode=" + toString(m_mode.load()) + ", phase=" + toString(m_phase.load())));
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


ClientCommandResult GameClient::startSingleplayer(const SingleplayerConfig& config) {
    if (m_mode.load() != ClientMode::Idle) {
        m_loggingManager.log(LogEntry::Warning("Rejected startSingleplayer(): mode=" + toString(m_mode.load()) + ", phase=" + toString(m_phase.load())));
        return ClientCommandResult::Failure(ClientError::InvalidState,
            "startSingleplayer() Requires Current Mode To Be Idle"
        );
    }

    m_loggingManager.log(LogEntry::Info("Starting singleplayer: playerColor=" + std::to_string(static_cast<int>(config.playerColor)) +", timePerSide=" + std::to_string(config.timePerSide.count()) + "s, increment=" + std::to_string(config.increment.count()) + "s"));
    transitionTo(ClientPhase::Starting);

    try {
        m_singleplayerClient.start(config);
        transitionTo(ClientMode::Singleplayer);
        transitionTo(ClientPhase::Active);
        m_loggingManager.log(LogEntry::Info("Singleplayer started successfully"));
        return ClientCommandResult::Success();
    } catch (const std::exception& e) {
        transitionTo(ClientMode::Idle);
        transitionTo(ClientPhase::Idle);
        m_loggingManager.log(LogEntry::Error("startSingleplayer() failed: " + std::string(e.what())));
        return ClientCommandResult::Failure(ClientError::StartupFailed, e.what());
    }
}

ClientCommandResult GameClient::stopSingleplayer() {
    if (m_mode.load() != ClientMode::Singleplayer) {
        m_loggingManager.log(LogEntry::Warning("Rejected stopSingleplayer(): mode=" + toString(m_mode.load()) + ", phase=" + toString(m_phase.load())));
        return ClientCommandResult::Failure(ClientError::InvalidState,
            "stopSingleplayer() Requires Current Mode To Be Singleplayer"
        );
    }

    m_loggingManager.log(LogEntry::Info("Stopping singleplayer"));
    transitionTo(ClientPhase::Stopping);

    try {
        m_singleplayerClient.stop();
        transitionTo(ClientMode::Idle);
        transitionTo(ClientPhase::Idle);
        m_loggingManager.log(LogEntry::Info("Singleplayer stopped successfully"));
        return ClientCommandResult::Success();
    } catch (const std::exception& e) {
        transitionTo(ClientPhase::Error);
        m_loggingManager.log(LogEntry::Error("stopSingleplayer() failed: " + std::string(e.what())));
        return ClientCommandResult::Failure(ClientError::ShutdownFailed, e.what());
    }
}

ClientCommandResult GameClient::startMultiplayer(const ServerInfo& server) {
    if (m_mode.load() != ClientMode::Idle) {
        m_loggingManager.log(LogEntry::Warning("Rejected startMultiplayer(): mode=" + toString(m_mode.load()) + ", phase=" + toString(m_phase.load())));
        return ClientCommandResult::Failure(ClientError::InvalidState,
            "startMultiplayer() Requires Current Mode To Be Idle"
        );
    }

    m_loggingManager.log(LogEntry::Info("Starting multiplayer: serverIp=" + server.ip));
    transitionTo(ClientMode::Multiplayer);
    transitionTo(ClientPhase::Starting);

    // auto result = m_multiplayerClient.connect(server);
    // if (!result) {
    //     transitionTo(ClientMode::Idle);
    //     transitionTo(ClientPhase::Idle);
    //     return result;
    // }

    m_loggingManager.log(LogEntry::Info("Multiplayer start accepted"));
    return ClientCommandResult::Success();
}

ClientCommandResult GameClient::stopMultiplayer() {
    if (m_mode.load() != ClientMode::Multiplayer) {
        m_loggingManager.log(LogEntry::Warning("Rejected stopMultiplayer(): mode=" + toString(m_mode.load()) + ", phase=" + toString(m_phase.load())));
        return ClientCommandResult::Failure(ClientError::InvalidState,
            "stopMultiplayer() Requires Current Mode To Be Multiplayer"
        );
    }

    m_loggingManager.log(LogEntry::Info("Stopping multiplayer"));
    transitionTo(ClientPhase::Stopping);

    // auto result = m_multiplayerClient.disconnect();
    // if (!result) {
    //     transitionTo(ClientPhase::Error;
    //     return result;
    // }

    m_loggingManager.log(LogEntry::Info("Multiplayer stop accepted"));
    return ClientCommandResult::Success();
}

void GameClient::onMultiplayerStopped() {
    if (m_mode.load() != ClientMode::Multiplayer) { return; }
    m_loggingManager.log(LogEntry::Info("Multiplayer stopped, returning to Idle"));
    transitionTo(ClientMode::Idle);
}


void GameClient::transitionTo(ClientMode newMode) {
    ClientMode oldState = m_mode.load();
    m_loggingManager.log(LogEntry::Info("Client mode transition: " + toString(oldState) + " -> " + toString(newMode)));
    m_mode = newMode;
    m_stateRegistry.fire(newMode);
}

void GameClient::transitionTo(ClientPhase newPhase) {
    ClientPhase oldPhase = m_phase.load();
    m_loggingManager.log(LogEntry::Info("Client phase transition: " + toString(oldPhase) + " -> " + toString(newPhase)));
    m_phase = newPhase;
    m_phaseRegistry.fire(newPhase);
}

}
