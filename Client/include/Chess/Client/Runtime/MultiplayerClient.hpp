#ifndef CHESS_CLIENT_RUNTIME_MULTIPLAYERCLIENT_HPP
#define CHESS_CLIENT_RUNTIME_MULTIPLAYERCLIENT_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/Common/ServerInfo.hpp>
#include <Chess/Client/Runtime/MultiplayerTypes.hpp>
#include <Chess/Client/Runtime/Callback/CallbackRegistry.hpp>
#include <Chess/Client/Runtime/Network/ClientSession.hpp>
#include <Chess/Core/Networking/MessagePayloads.hpp>

// ASIO Includes
#include <asio/io_context.hpp>

// C++ Includes
#include <atomic>
#include <mutex>
#include <optional>
#include <string>

namespace Chess {

class MultiplayerClient {
public:
    explicit MultiplayerClient(asio::io_context& context);
    ~MultiplayerClient();

    MultiplayerClient(const MultiplayerClient&) = delete;
    MultiplayerClient& operator=(const MultiplayerClient&) = delete;
    MultiplayerClient(MultiplayerClient&&) = delete;
    MultiplayerClient& operator=(MultiplayerClient&&) = delete;

    // Commands
    [[nodiscard]] MultiplayerStatus requestConnect(const ServerInfo& serverInfo, std::string username);
    [[nodiscard]] MultiplayerStatus requestDisconnect();

    // Events
    void pump();
    [[nodiscard]] std::vector<MultiplayerEvent> drainEvents();

    //View
    [[nodiscard]] MultiplayerView view() const;
    [[nodiscard]] MultiplayerState state() const noexcept;

private:
    [[nodiscard]] MessageThreadID nextMessageThreadID();
    void transitionTo(MultiplayerState newState);
    void emitEvent(MultiplayerEvent event);
    void clearConnectionState();

    void handleSessionEvent(ClientSessionEvent event);
    void handleSessionConnected();
    void handleSessionConnectFailed(ClientSessionEvent event);
    void handleSessionDisconnected(ClientSessionEvent event);
    void handleIncomingMessage(Message message);
    void handleLoginResponse(Message& message);

private:
    asio::io_context& m_context;
    ClientSession m_clientSession;

    AsyncEventQueue<ClientSessionEvent> m_sessionEvents;
    AsyncEventQueue<MultiplayerEvent> m_events;

    std::atomic<MultiplayerState> m_state{MultiplayerState::Idle};
    std::atomic<MessageThreadID> m_nextThreadID{1};

    std::optional<ServerInfo> m_serverInfo;
    std::string m_username;
    MessageThreadID m_activeMessageThreadID{NO_MESSAGE_THREAD_ID};

    bool m_socketConnected{false};
    bool m_loginAccepted{false};
};

} // namespace Chess

#endif
