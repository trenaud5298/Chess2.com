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


GameClient::GameClient()
: m_startTime(std::chrono::steady_clock::now()), m_loggingManager(*this), m_persistenceManager(*this),
m_singleplayerClient(std::bind_front(&GameClient::publishEvent, this)),
m_multiplayerClient(m_context, std::bind_front(&GameClient::publishEvent, this)) {
    m_workGuard.emplace(asio::make_work_guard(m_context));
    m_asioThread = std::thread([this]() {
        m_context.run();
    });
}

GameClient::~GameClient() {
    if (m_workGuard.has_value()) {
        m_workGuard->reset();
    }

    m_context.stop();

    if (m_asioThread.joinable()) {
        m_asioThread.join();
    }
}

// GameClient Controls
ClientStatus GameClient::tick() {
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    try {
        if (isSingleplayerState()) {
            m_singleplayerClient.tick(now);
        }

        for (ClientEvent& event : m_eventQueue.drain()) {
            logEvent(event);
            handleEvent(event);
            m_eventRegistry.fire(event);
        }

        return ClientStatus::Success();
    } catch (const std::exception& e) {
        setFatalError(StatusCode::RuntimeException, e.what());
        return ClientStatus::Fatal(StatusCode::RuntimeException, e.what());
    }
}

ClientStatus GameClient::returnToIdle() {
    try {
        if (isSingleplayerState()) {
            ClientStatus status = m_singleplayerClient.stop();
            if (!status) {
                return status;
            }
            transitionTo(ClientState::Idle);
            return ClientStatus::Success();
        }

        if (isMultiplayerState()) {
            ClientStatus status = m_multiplayerClient.requestDisconnect();
            if (!status) {
                return status;
            }
            transitionTo(ClientState::Idle);
            return ClientStatus::Success();
        }

        // This Should Only Happen When Already Idle
        transitionTo(ClientState::Idle);
        return ClientStatus::Success();
    } catch (const std::exception& e) {
        setFatalError(StatusCode::RuntimeException, e.what());
        return ClientStatus::Fatal(StatusCode::RuntimeException, e.what());
    }
}


// Game Client Errors
std::optional<ClientStatus> GameClient::consumeFatalError() {
    std::lock_guard lock(m_fatalErrorMutex);
    auto out = std::move(m_fatalError);
    m_fatalError.reset();
    return out;
}

void GameClient::recoverFromFatalError() noexcept {
    try {
        (void)m_singleplayerClient.stop();
    } catch (...) {
    }

    try {
        (void)m_multiplayerClient.requestDisconnect();
    } catch (...) {
    }

    transitionTo(ClientState::Idle);
}

// Singleplayer Commands
ClientStatus GameClient::enterSingleplayerSetup() {
    if (m_state.load() != ClientState::Idle) {
        return ClientStatus::Error(StatusCode::InvalidState, "Client must be idle");
    }
    transitionTo(ClientState::SingleplayerSetup);
    return ClientStatus::Success();
}

ClientStatus GameClient::startSingleplayer(const SingleplayerConfig& config) {
    if (m_state.load() != ClientState::SingleplayerSetup) {
        return ClientStatus::Error(StatusCode::InvalidState, "Not in singleplayer setup");
    }

    try {
        ClientStatus status = m_singleplayerClient.start(config, std::chrono::steady_clock::now());
        if (status) {
            transitionTo(ClientState::SingleplayerInGame);
        }
        return status;
    } catch (const std::exception& e) {
        setFatalError(StatusCode::StartupFailed, e.what());
        return ClientStatus::Fatal(StatusCode::StartupFailed, e.what());
    }
}

ClientStatus GameClient::stopSingleplayer() {
    if (!isSingleplayerState()) {
        return ClientStatus::Error(StatusCode::InvalidState, "Singleplayer not active");
    }

    try {
        ClientStatus status = m_singleplayerClient.stop();
        if (status) {
            transitionTo(ClientState::Idle);
        }
        return status;
    } catch (const std::exception& e) {
        setFatalError(StatusCode::RuntimeException, e.what());
        return ClientStatus::Fatal(StatusCode::RuntimeException, e.what());
    }
}

ClientStatus GameClient::restartSingleplayer() {
    if (m_state.load() != ClientState::SingleplayerResult) {
        return ClientStatus::Error(StatusCode::InvalidState, "Not in singleplayer result");
    }

    try {
        ClientStatus status = m_singleplayerClient.restart(std::chrono::steady_clock::now());
        if (status) {
            transitionTo(ClientState::SingleplayerInGame);
        }
        return status;
    } catch (const std::exception& e) {
        setFatalError(StatusCode::RuntimeException, e.what());
        return ClientStatus::Fatal(StatusCode::RuntimeException, e.what());
    }
}


ClientStatus GameClient::submitSingleplayerMove(ID from, Pos to) {
    if (m_state.load() != ClientState::SingleplayerInGame) {
        return ClientStatus::Error(StatusCode::InvalidState, "Singleplayer not active");
    }
    m_loggingManager.log(LogEntry::Debug("TEST 1!"));
    try {
        return m_singleplayerClient.tryMove(from, to, std::chrono::steady_clock::now());
    } catch (const std::exception& e) {
        setFatalError(StatusCode::RuntimeException, e.what());
        return ClientStatus::Fatal(StatusCode::RuntimeException, e.what());
    }
    m_loggingManager.log(LogEntry::Debug("TEST 2!"));
}

ClientStatus GameClient::resignSingleplayer() {
    if (m_state.load() != ClientState::SingleplayerInGame) {
        return ClientStatus::Error(StatusCode::InvalidState, "Singleplayer not in game");
    }

    try {
        return m_singleplayerClient.resign(std::chrono::steady_clock::now());
    } catch (const std::exception& e) {
        setFatalError(StatusCode::RuntimeException, e.what());
        return ClientStatus::Fatal(StatusCode::RuntimeException, e.what());
    }
}

ClientStatus GameClient::pauseSingleplayer() {
    if (m_state.load() != ClientState::SingleplayerInGame) {
        return ClientStatus::Error(StatusCode::InvalidState, "Singleplayer not in game");
    }

    try {
        return m_singleplayerClient.pause(std::chrono::steady_clock::now());
    } catch (const std::exception& e) {
        setFatalError(StatusCode::RuntimeException, e.what());
        return ClientStatus::Fatal(StatusCode::RuntimeException, e.what());
    }
}

ClientStatus GameClient::resumeSingleplayer() {
    if (m_state.load() != ClientState::SingleplayerInGame) {
        return ClientStatus::Error(StatusCode::InvalidState, "Singleplayer not in game");
    }

    try {
        return m_singleplayerClient.resume(std::chrono::steady_clock::now());
    } catch (const std::exception& e) {
        setFatalError(StatusCode::RuntimeException, e.what());
        return ClientStatus::Fatal(StatusCode::RuntimeException, e.what());
    }
}

// Singleplayer Info
SingleplayerView GameClient::singleplayerView() const {
    return m_singleplayerClient.view(std::chrono::steady_clock::now());
}


// Multiplayer Commands
ClientStatus GameClient::enterMultiplayerSetup() {
    if (m_state.load() != ClientState::Idle) {
        return ClientStatus::Error(StatusCode::InvalidState, "Client must be idle");
    }
    transitionTo(ClientState::MultiplayerSetup);
    return ClientStatus::Success();
}

ClientStatus GameClient::requestMultiplayerConnect(const ServerInfo& server) {
    if (m_state.load() != ClientState::MultiplayerSetup) {
        return ClientStatus::Error(StatusCode::InvalidState, "Not in multiplayer setup");
    }

    try {
        std::string username = m_persistenceManager.settings().getUsername();
        ClientStatus status = m_multiplayerClient.requestConnect(server, username);
        if (status) {
            transitionTo(ClientState::MultiplayerConnecting);
        }
        return status;
    } catch (const std::exception& e) {
        setFatalError(StatusCode::RuntimeException, e.what());
        return ClientStatus::Fatal(StatusCode::RuntimeException, e.what());
    }
}

ClientStatus GameClient::requestMultiplayerDisconnect() {
    if (!isMultiplayerState()) {
        return ClientStatus::Error(StatusCode::InvalidState, "Not in multiplayer");
    }

    try {
        return m_multiplayerClient.requestDisconnect();
    } catch (const std::exception& e) {
        setFatalError(StatusCode::RuntimeException, e.what());
        return ClientStatus::Fatal(StatusCode::RuntimeException, e.what());
    }
}

ClientStatus GameClient::submitMultiplayerGlobalChat(std::string text) {
    if (m_state != ClientState::MultiplayerLobby && m_state != ClientState::MultiplayerInGame) {
        return ClientStatus::Error(StatusCode::InvalidState, "Global Chat Unavailable");
    }

    try {
        return m_multiplayerClient.requestSendGlobalChat(std::move(text));
    } catch (const std::exception& e) {
        setFatalError(StatusCode::RuntimeException, e.what());
        return ClientStatus::Fatal(StatusCode::RuntimeException, e.what());
    }
}

ClientStatus GameClient::submitMultiplayerGameChat(std::string text) {
    if (m_state != ClientState::MultiplayerInGame) {
        return ClientStatus::Error(StatusCode::InvalidState, "Game Chat Unavailable");
    }

    try {
        return m_multiplayerClient.requestSendGameChat(std::move(text));
    } catch (const std::exception& e) {
        setFatalError(StatusCode::RuntimeException, e.what());
        return ClientStatus::Fatal(StatusCode::RuntimeException, e.what());
    }
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

void GameClient::setFatalError(StatusCode error, std::string message) noexcept {
    {
        std::lock_guard lock(m_fatalErrorMutex);
        m_fatalError = ClientStatus::Fatal(error, std::move(message));
    }
    transitionTo(ClientState::Error);
}

void GameClient::publishEvent(ClientEvent event) {
    m_eventQueue.push(std::move(event));
}

void GameClient::logEvent(const ClientEvent& event) {
    std::string message =
        "[" + std::string(toString(event.source)) + "]" +
        "[" + std::string(toString(event.type)) + "]" +
        "[" + std::string(toString(event.kind)) + "]" +
        "[" + std::string(toString(event.status.severity)) + "]";

    if (!event.message().empty()) {
        message += " " + event.message();
    }

    switch (event.status.severity) {
        case Severity::Debug:
            m_loggingManager.log(LogEntry::Debug(message));
            break;
        case Severity::Info:
            m_loggingManager.log(LogEntry::Info(message));
            break;
        case Severity::Warning:
            m_loggingManager.log(LogEntry::Warning(message));
            break;
        case Severity::Error:
        case Severity::Fatal:
            m_loggingManager.log(LogEntry::Error(message));
            break;
    }
}

void GameClient::handleEvent(const ClientEvent& event) {
    switch (event.source) {
        case EventSource::Singleplayer:
            handleSingleplayerEvent(event);
            break;
        case EventSource::Multiplayer:
            handleMultiplayerEvent(event);
            break;
        case EventSource::Persistence:
            handlePersistenceEvent(event);
            break;
        case EventSource::GameClient:
        case EventSource::UI:
        case EventSource::None:
            break;
    }
}

void GameClient::handleSingleplayerEvent(const ClientEvent& event) {
    switch (event.type) {
        case EventType::SingleplayerGameEnd:
            if (event.isSuccess()) {
                transitionTo(ClientState::SingleplayerResult);
            }
            break;
        case EventType::None:
        case EventType::SingleplayerStart:
        case EventType::SingleplayerMove:
        case EventType::MultiplayerConnect:
        case EventType::MultiplayerTransport:
        case EventType::MultiplayerLogin:
        case EventType::MultiplayerDisconnect:
        case EventType::SettingsSave:
            break;
    }
}

void GameClient::handleMultiplayerEvent(const ClientEvent& event) {
    switch (event.type) {
        case EventType::MultiplayerConnect:
            handleMultiplayerConnectEvent(event);
            break;
        case EventType::MultiplayerTransport:
            handleMultiplayerTransportEvent(event);
            break;
        case EventType::MultiplayerLogin:
            handleMultiplayerLoginEvent(event);
            break;
        case EventType::MultiplayerDisconnect:
            handleMultiplayerDisconnectEvent(event);
            break;
        case EventType::None:
        case EventType::SingleplayerStart:
        case EventType::SingleplayerMove:
        case EventType::SingleplayerGameEnd:
        case EventType::SettingsSave:
            break;
    }
}

void GameClient::handleMultiplayerConnectEvent(const ClientEvent& event) {
    // Connect Event Does Not Mean Transition To Lobby Yet
    if (event.isFailure() && isMultiplayerState()) {
        transitionTo(ClientState::MultiplayerSetup);
    }
}

void GameClient::handleMultiplayerTransportEvent(const ClientEvent& event) {
    // Null For Now
}

void GameClient::handleMultiplayerLoginEvent(const ClientEvent& event) {
    if (event.isSuccess()) {
        transitionTo(ClientState::MultiplayerLobby);
        return;
    }

    if (isMultiplayerState()) {
        transitionTo(ClientState::MultiplayerSetup);
    }
}

void GameClient::handleMultiplayerDisconnectEvent(const ClientEvent& event) {
    if (isMultiplayerState()) {
        transitionTo(ClientState::MultiplayerSetup);
    }
}


void GameClient::handlePersistenceEvent(const ClientEvent &event) {
    // Null For Now
}
    
}
