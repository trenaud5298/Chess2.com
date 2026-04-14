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
    explicit MultiplayerClient(asio::io_context& ioContext);
    ~MultiplayerClient();

    MultiplayerClient(const MultiplayerClient&) = delete;
    MultiplayerClient& operator=(const MultiplayerClient&) = delete;
    MultiplayerClient(MultiplayerClient&&) = delete;
    MultiplayerClient& operator=(MultiplayerClient&&) = delete;

    // High-level requests
    [[nodiscard]] MultiplayerStatus requestConnect(const ServerInfo& server, std::string username);
    [[nodiscard]] MultiplayerStatus requestDisconnect();

    // View / state
    [[nodiscard]] MultiplayerState state() const noexcept { return m_state.load(); }
    [[nodiscard]] MultiplayerView view() const;
    [[nodiscard]] bool isConnected() const noexcept;
    [[nodiscard]] bool isLoggedIn() const noexcept;

    // Callback registries
    [[nodiscard]] CallbackRegistry<MultiplayerState>& stateRegistry() { return m_stateRegistry; }
    [[nodiscard]] CallbackRegistry<const MultiplayerEvent&>& eventRegistry() { return m_eventRegistry; }

    // Session callbacks
    void onTransportConnected(MessageThreadID messageThreadID);
    void onConnectFailed(MessageThreadID messageThreadID, MultiplayerErrorCode code, std::string message);
    void onLoginResponse(MessageThreadID messageThreadID, const LoginResponse& response);
    void onDisconnected(MultiplayerErrorCode code, std::string message);

private:
    [[nodiscard]] MessageThreadID nextMessageThreadID() noexcept;
    void transitionTo(MultiplayerState next);
    void emitEvent(MultiplayerEvent event);

    [[nodiscard]] bool canBeginConnect() const noexcept;
    void clearConnectionState();

private:
    asio::io_context& m_ioContext;
    ClientSession m_clientSession;

    std::atomic<MultiplayerState> m_state{MultiplayerState::Idle};
    std::atomic<MessageThreadID> m_nextThreadID{1};

    mutable std::mutex m_mutex;
    std::optional<ServerInfo> m_serverInfo;
    bool m_socketConnected{false};
    bool m_loginAccepted{false};

    CallbackRegistry<MultiplayerState> m_stateRegistry;
    CallbackRegistry<const MultiplayerEvent&> m_eventRegistry;
};

} // namespace Chess

#endif
