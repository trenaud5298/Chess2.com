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


GameClient::GameClient() : m_startTime(std::chrono::steady_clock::now()), m_loggingManager(*this), m_persistenceManager(*this), m_singleplayerClient(), m_multiplayerClient(*this) {
    m_singleplayerClient.resultRegistry().subscribe([this](const GameResult&) {
        transitionTo(ClientState::SingleplayerResult);
    });
}

GameClient::~GameClient() {

}


// GameClient Controls
void GameClient::tick() {
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    if (isSingleplayerState()) {
        m_singleplayerClient.tick(now);
    }
    if (isMultiplayerState()) {
        // m_multiplayerClient.tick(now);
    }
}


// Singleplayer Commands
ClientCommandResult GameClient::enterSingleplayerSetup() {
    if (m_state.load() != ClientState::Idle) {
        return ClientCommandResult::Failure(ClientError::InvalidState, "Client must be idle");
    }
    transitionTo(ClientState::SingleplayerSetup);
    return ClientCommandResult::Success();
}

ClientCommandResult GameClient::startSingleplayer(const SingleplayerConfig& config) {
    if (m_state.load() != ClientState::SingleplayerSetup) {
        return ClientCommandResult::Failure(ClientError::InvalidState, "Not in singleplayer setup");
    }

    try {
        m_singleplayerClient.start(config, std::chrono::steady_clock::now());
        transitionTo(ClientState::SingleplayerInGame);
        return ClientCommandResult::Success();
    } catch (const std::exception& e) {
        transitionTo(ClientState::Error);
        return ClientCommandResult::Failure(ClientError::StartupFailed, e.what());
    }
}

ClientCommandResult GameClient::submitSingleplayerMove(ID from, Pos to) {
    if (m_state.load() != ClientState::SingleplayerInGame) {
        return ClientCommandResult::Failure(ClientError::InvalidState, "Singleplayer not active");
    }

    bool validMove = false;
    try {
        validMove = m_singleplayerClient.tryMove(from, to, std::chrono::steady_clock::now());
    } catch (const std::exception& e) {
        transitionTo(ClientState::Error);
        return ClientCommandResult::Failure(ClientError::RuntimeException, e.what());
    }

    if (!validMove) {
        return ClientCommandResult::Failure(ClientError::CommandRejected, "Invalid move");
    }
    return ClientCommandResult::Success();
}

ClientCommandResult GameClient::resignSingleplayer() {
    if (m_state.load() != ClientState::SingleplayerInGame) {
        return ClientCommandResult::Failure(ClientError::InvalidState, "Singleplayer not in game");
    }

    try {
        m_singleplayerClient.resign(std::chrono::steady_clock::now());
    } catch (const std::exception& e) {
        transitionTo(ClientState::Error);
        return ClientCommandResult::Failure(ClientError::RuntimeException, e.what());
    }
    return ClientCommandResult::Success();
}

ClientCommandResult GameClient::stopSingleplayer() {
    if (!isSingleplayerState()) {
        return ClientCommandResult::Failure(ClientError::InvalidState, "Singleplayer not active");
    }

    try {
        m_singleplayerClient.stop();
    } catch (const std::exception& e) {
        transitionTo(ClientState::Error);
        return ClientCommandResult::Failure(ClientError::RuntimeException, e.what());
    }
    transitionTo(ClientState::Idle);
    return ClientCommandResult::Success();
}

ClientCommandResult GameClient::pauseSingleplayer() {
    if (m_state.load() != ClientState::SingleplayerInGame) {
        return ClientCommandResult::Failure(ClientError::InvalidState, "Singleplayer not in game");
    }

    try {
        m_singleplayerClient.pause(std::chrono::steady_clock::now());
    } catch (const std::exception& e) {
        transitionTo(ClientState::Error);
        return ClientCommandResult::Failure(ClientError::RuntimeException, e.what());
    }
    return ClientCommandResult::Success();
}

ClientCommandResult GameClient::resumeSingleplayer() {
    if (m_state.load() != ClientState::SingleplayerInGame) {
        return ClientCommandResult::Failure(ClientError::InvalidState, "Singleplayer not in game");
    }

    try {
        m_singleplayerClient.resume(std::chrono::steady_clock::now());
    } catch (const std::exception& e) {
        transitionTo(ClientState::Error);
        return ClientCommandResult::Failure(ClientError::RuntimeException, e.what());
    }
    return ClientCommandResult::Success();
}

// Singleplayer Info
SingleplayerView GameClient::singleplayerView(std::chrono::steady_clock::time_point now) const {
    return m_singleplayerClient.view(now);
}


// Multiplayer Commands
ClientCommandResult GameClient::startMultiplayer(const ServerInfo &server) {
    return ClientCommandResult::Failure(ClientError::RuntimeException, "NOT IMPLEMENTED");
}

ClientCommandResult GameClient::stopMultiplayer() {
    return ClientCommandResult::Failure(ClientError::RuntimeException, "NOT IMPLEMENTED");
}



// Helpers
void GameClient::transitionTo(ClientState newState) {
    ClientState oldState = m_state.exchange(newState);
    if (oldState == newState) {
        return;
    }

    m_loggingManager.log(LogEntry::Info("Client State Transition: " + toString(oldState) + " -> " + toString(newState)));
    m_stateRegistry.fire(newState);
}
}
