/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */


// Chess Includes
#include <Chess/Client/Runtime/MultiplayerClient.hpp>
#include <Chess/Client/Runtime/GameClient.hpp>

// ASIO Includes

// C++ Includes


namespace Chess {

MultiplayerClient::MultiplayerClient(asio::io_context& context) : m_context(context), m_clientSession(context, [this](ClientSessionEvent event) {
    m_sessionEvents.push(std::move(event));
}) {

}


MultiplayerClient::~MultiplayerClient() {

}

// Commands
MultiplayerStatus MultiplayerClient::requestConnect(const ServerInfo& serverInfo, std::string username) {
    if (m_state.load() != MultiplayerState::Idle) {
        return MultiplayerStatus::Failure(MultiplayerErrorCode::InvalidState, "Multiplayer client is not idle");
    }

    if (serverInfo.ip.empty()) {
        return MultiplayerStatus::Failure(MultiplayerErrorCode::InvalidArgument, "Server Ip is empty");
    }

    if (username.empty()) {
        return MultiplayerStatus::Failure(MultiplayerErrorCode::InvalidArgument, "Username is empty");
    }

    m_serverInfo = serverInfo;
    m_username = std::move(username);
    m_activeMessageThreadID = nextMessageThreadID();
    m_socketConnected = false;
    m_loginAccepted = false;

    transitionTo(MultiplayerState::ConnectingNetwork);

    MultiplayerStatus status = m_clientSession.requestConnect(serverInfo);
    if (!status) {
        clearConnectionState();
        transitionTo(MultiplayerState::Idle);
        return status;
    }
    return MultiplayerStatus::Success();
}

MultiplayerStatus MultiplayerClient::requestDisconnect() {
    if (m_state.load() == MultiplayerState::Idle) {
        return MultiplayerStatus::Success();
    }

    return m_clientSession.requestDisconnect();
}

// Events
void MultiplayerClient::pump() {
    for (ClientSessionEvent& event : m_sessionEvents.drain()) {
        handleSessionEvent(std::move(event));
    }
}

std::vector<MultiplayerEvent> MultiplayerClient::drainEvents() {
    return m_events.drain();
}


// View
MultiplayerView MultiplayerClient::view() const {
    return {
        .state = m_state.load(),
        .serverInfo = m_serverInfo,
        .socketConnected = m_socketConnected,
        .loginAccepted = m_loginAccepted,
    };
}

MultiplayerState MultiplayerClient::state() const noexcept {
    return m_state.load();
}

MessageThreadID MultiplayerClient::nextMessageThreadID() {
    return  m_nextThreadID.fetch_add(1);
}

void MultiplayerClient::transitionTo(MultiplayerState newState) {
    m_state.load(newState);
}



}
