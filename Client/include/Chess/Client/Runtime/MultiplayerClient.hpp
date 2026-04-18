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
#include <Chess/Client/Common/ClientStatus.hpp>
#include <Chess/Client/Common/ClientEvent.hpp>
#include <Chess/Client/Common/ServerInfo.hpp>
#include <Chess/Client/Runtime/MultiplayerTypes.hpp>
#include <Chess/Core/Networking/Message.hpp>

// ASIO Includes
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/strand.hpp>

// C++ Includes
#include <deque>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <mutex>

namespace Chess {

class MultiplayerClient {
public:
    explicit MultiplayerClient(asio::io_context& context, std::function<void(ClientEvent)> emitEvent);
    ~MultiplayerClient();

    MultiplayerClient(const MultiplayerClient&) = delete;
    MultiplayerClient& operator=(const MultiplayerClient&) = delete;
    MultiplayerClient(MultiplayerClient&&) = delete;
    MultiplayerClient& operator=(MultiplayerClient&&) = delete;

    // Commands
    [[nodiscard]] ClientStatus requestConnect(const ServerInfo& serverInfo, std::string username);
    [[nodiscard]] ClientStatus requestDisconnect();

    // View
    [[nodiscard]] MultiplayerView view() const;
    [[nodiscard]] MultiplayerState state() const;

private:
    // Thread Safe Functions
    void emitEvent(ClientEvent event);
    void emitInfo(EventType type, std::string message);
    void emitResult(EventType type, ClientStatus status);

    // Snapshot Helpers
    void refreshViewSnapshot();

    // Strand-Only Lifecycle Helpers
    void transitionTo(MultiplayerState newState);
    void clearConnectionState();
    void closeTransport();
    void terminateSession(EventType type, ClientStatus status, MultiplayerState nextState = MultiplayerState::Idle);

    // Strand Version Of Public Functions (Public Functions Publish To Strand)
    void doRequestConnect(ServerInfo serverInfo, std::string username);
    void doRequestDisconnect();
    ClientStatus queueSend(std::shared_ptr<const Message> message);

    // Strand Async Transport Functions
    void doReadHeader();
    void doReadBody();
    void doWrite();

    // Strand Protocol Handling
    void onTransportConnected();
    void onIncomingMessage(Message message);
    void onLoginResponse(Message& message);

private:
    std::function<void(ClientEvent)> m_emitEvent;

    asio::io_context& m_context;
    asio::ip::tcp::socket m_socket;
    asio::strand<asio::any_io_executor> m_strand;

    // Strand-Owned Mutable State (Avoids Need For A Mutex)
    MultiplayerState m_state{MultiplayerState::Idle};
    std::optional<ServerInfo> m_serverInfo;
    std::string m_username;
    bool m_socketConnected{false};
    bool m_loginAccepted{false};

    Message m_incomingMessage{MessageType::None};
    std::deque<std::shared_ptr<const Message>> m_writeQueue;
    static constexpr std::size_t MAX_WRITE_QUEUE_LENGTH = 128;

    // Main Thread Snapshot (Mutex Protected, Updated By Strand)
    mutable std::mutex m_viewMutex;
    MultiplayerView m_viewSnapshot;
};

} // namespace Chess

#endif
