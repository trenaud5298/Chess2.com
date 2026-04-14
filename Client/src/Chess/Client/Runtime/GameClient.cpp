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
ClientCommandResult GameClient::tick() {
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    try {
        if (isSingleplayerState()) {
            m_singleplayerClient.tick(now);
        }
        if (isMultiplayerState()) {
            // m_multiplayerClient.tick(now);
        }
    } catch (const std::exception& e) {
        setFatalError(ClientErrorCode::RuntimeException, e.what());
        return ClientCommandResult::Fatal(ClientErrorCode::RuntimeException, e.what());
    }

    return ClientCommandResult::Success();
}


ClientCommandResult GameClient::shutdown() {
    //TODO: Need To Implement Shutdown
    return ClientCommandResult::Error(ClientErrorCode::NotImplemented, "Shutdown Not Properly Implemented");
}

ClientCommandResult GameClient::returnToIdle() {
    try {
        m_singleplayerClient.stop();
        // m_multiplayerClient.stop();
    } catch (const std::exception& e) {
        setFatalError(ClientErrorCode::RuntimeException, e.what());
        return ClientCommandResult::Fatal(ClientErrorCode::RuntimeException, e.what());
    }
    transitionTo(ClientState::Idle);
    return ClientCommandResult::Success();
}


// Game Client Errors
std::optional<ClientCommandResult> GameClient::consumeFatalError() {
    std::lock_guard lock(m_fatalErrorMutex);
    auto out = std::move(m_fatalError);
    m_fatalError.reset();
    return out;
}

void GameClient::recoverFromFatalError() noexcept {
    try {m_singleplayerClient.stop();} catch (...) {}
    // try {m_multiplayerClient.stop();} catch (...) {}

    transitionTo(ClientState::Idle);
}

// Singleplayer Commands
ClientCommandResult GameClient::enterSingleplayerSetup() {
    if (m_state.load() != ClientState::Idle) {
        return ClientCommandResult::Warn(ClientErrorCode::InvalidState, "Client must be idle");
    }
    transitionTo(ClientState::SingleplayerSetup);
    return ClientCommandResult::Success();
}

ClientCommandResult GameClient::startSingleplayer(const SingleplayerConfig& config) {
    if (m_state.load() != ClientState::SingleplayerSetup) {
        return ClientCommandResult::Warn(ClientErrorCode::InvalidState, "Not in singleplayer setup");
    }

    try {
        m_singleplayerClient.start(config, std::chrono::steady_clock::now());
        transitionTo(ClientState::SingleplayerInGame);
        return ClientCommandResult::Success();
    } catch (const std::exception& e) {
        setFatalError(ClientErrorCode::StartupFailed, e.what());
        return ClientCommandResult::Fatal(ClientErrorCode::StartupFailed, e.what());
    }
}

ClientCommandResult GameClient::submitSingleplayerMove(ID from, Pos to) {
    if (m_state.load() != ClientState::SingleplayerInGame) {
        return ClientCommandResult::Warn(ClientErrorCode::InvalidState, "Singleplayer not active");
    }

    bool validMove = false;
    try {
        validMove = m_singleplayerClient.tryMove(from, to, std::chrono::steady_clock::now());
    } catch (const std::exception& e) {
        setFatalError(ClientErrorCode::RuntimeException, e.what());
        return ClientCommandResult::Fatal(ClientErrorCode::RuntimeException, e.what());
    }

    if (!validMove) {
        return ClientCommandResult::Reject(ClientErrorCode::InvalidMove, "Invalid move");
    }
    return ClientCommandResult::Success();
}

ClientCommandResult GameClient::resignSingleplayer() {
    if (m_state.load() != ClientState::SingleplayerInGame) {
        return ClientCommandResult::Warn(ClientErrorCode::InvalidState, "Singleplayer not in game");
    }

    try {
        m_singleplayerClient.resign(std::chrono::steady_clock::now());
    } catch (const std::exception& e) {
        setFatalError(ClientErrorCode::RuntimeException, e.what());
        return ClientCommandResult::Fatal(ClientErrorCode::RuntimeException, e.what());
    }
    return ClientCommandResult::Success();
}

ClientCommandResult GameClient::stopSingleplayer() {
    if (!isSingleplayerState()) {
        return ClientCommandResult::Warn(ClientErrorCode::InvalidState, "Singleplayer not active");
    }

    try {
        m_singleplayerClient.stop();
    } catch (const std::exception& e) {
        setFatalError(ClientErrorCode::RuntimeException, e.what());
        return ClientCommandResult::Fatal(ClientErrorCode::RuntimeException, e.what());
    }
    transitionTo(ClientState::Idle);
    return ClientCommandResult::Success();
}

ClientCommandResult GameClient::restartSingleplayer() {
    if (m_state.load() != ClientState::SingleplayerResult) {
        return ClientCommandResult::Warn(ClientErrorCode::InvalidState, "Not in singleplayer result");
    }

    try {
        m_singleplayerClient.restart(std::chrono::steady_clock::now());
    } catch (const std::exception& e) {
        setFatalError(ClientErrorCode::RuntimeException, e.what());
        return ClientCommandResult::Fatal(ClientErrorCode::RuntimeException, e.what());
    }
    transitionTo(ClientState::SingleplayerInGame);
    return ClientCommandResult::Success();
}

ClientCommandResult GameClient::pauseSingleplayer() {
    if (m_state.load() != ClientState::SingleplayerInGame) {
        return ClientCommandResult::Warn(ClientErrorCode::InvalidState, "Singleplayer not in game");
    }

    try {
        m_singleplayerClient.pause(std::chrono::steady_clock::now());
    } catch (const std::exception& e) {
        setFatalError(ClientErrorCode::RuntimeException, e.what());
        return ClientCommandResult::Fatal(ClientErrorCode::RuntimeException, e.what());
    }
    return ClientCommandResult::Success();
}

ClientCommandResult GameClient::resumeSingleplayer() {
    if (m_state.load() != ClientState::SingleplayerInGame) {
        return ClientCommandResult::Warn(ClientErrorCode::InvalidState, "Singleplayer not in game");
    }

    try {
        m_singleplayerClient.resume(std::chrono::steady_clock::now());
    } catch (const std::exception& e) {
        setFatalError(ClientErrorCode::RuntimeException, e.what());
        return ClientCommandResult::Fatal(ClientErrorCode::RuntimeException, e.what());
    }
    return ClientCommandResult::Success();
}

// Singleplayer Info
SingleplayerView GameClient::singleplayerView() const {
    return m_singleplayerClient.view(std::chrono::steady_clock::now());
}



// Multiplayer Commands
ClientCommandResult GameClient::enterMultiplayerSetup() {
    if (m_state.load() != ClientState::Idle) {
        return ClientCommandResult::Warn(ClientErrorCode::InvalidState, "Client must be idle");
    }
    transitionTo(ClientState::MultiplayerSetup);
    return ClientCommandResult::Success();
}

ClientCommandResult GameClient::requestMultiplayerConnect(const ServerInfo &server) {
    return ClientCommandResult::Error(ClientErrorCode::NotImplemented, "NOT IMPLEMENTED");
}

ClientCommandResult GameClient::stopMultiplayer() {
    return ClientCommandResult::Error(ClientErrorCode::NotImplemented, "NOT IMPLEMENTED");
}



// Helpers
void GameClient::transitionTo(ClientState newState) {
    ClientState oldState = m_state.exchange(newState);
    if (oldState == newState) {
        return;
    }

    m_loggingManager.log(LogEntry::Info("Client State Transition: " + std::string(toString(oldState)) + " -> " + std::string(toString(newState))));
    m_stateRegistry.fire(newState);
}

void GameClient::setFatalError(ClientErrorCode error, std::string message) noexcept {
    {
        std::lock_guard lock(m_fatalErrorMutex);
        m_fatalError = ClientCommandResult::Fatal(error, std::move(message));
    }
    transitionTo(ClientState::Error);
}
}
