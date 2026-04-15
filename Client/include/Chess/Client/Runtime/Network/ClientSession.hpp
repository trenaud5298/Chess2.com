#ifndef CHESS_CLIENT_RUNTIME_NETWORK_CLIENTSESSION_HPP
#define CHESS_CLIENT_RUNTIME_NETWORK_CLIENTSESSION_HPP

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
#include <Chess/Core/Networking/Message.hpp>

// ASIO Includes
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/strand.hpp>

// C++ Includes
#include <atomic>
#include <deque>
#include <functional>
#include <memory>
#include <optional>
#include <string>

namespace Chess {

enum class ClientSessionEventType : std::uint8_t {
    Connected = 0,
    ConnectFailed,
    MessageReceived,
    Disconnected
};

struct ClientSessionEvent {
    ClientSessionEventType type{ClientSessionEventType::Disconnected};
    MultiplayerErrorCode errorCode{MultiplayerErrorCode::None};
    std::string message;
    std::optional<Message> incomingMessage;
};

class ClientSession {
public:
    ClientSession(asio::io_context& ioContext, std::function<void(ClientSessionEvent)> eventCallback);
    ~ClientSession();

    ClientSession(const ClientSession&) = delete;
    ClientSession& operator=(const ClientSession&) = delete;
    ClientSession(ClientSession&&) = delete;
    ClientSession& operator=(ClientSession&&) = delete;

    [[nodiscard]] MultiplayerStatus requestConnect(const ServerInfo& server);
    [[nodiscard]] MultiplayerStatus requestDisconnect();
    [[nodiscard]] MultiplayerStatus requestSend(std::shared_ptr<const Message> message);

    [[nodiscard]] bool isConnected() const noexcept { return m_isConnected.load(); }

private:
    void doReadHeader();
    void doReadBody();
    void doWrite();

    void emitEvent(ClientSessionEvent event);
    void abortSession(MultiplayerErrorCode code, std::string message);

private:
    std::function<void(ClientSessionEvent)> m_eventCallback;

    std::atomic<bool> m_isConnected{false};

    asio::ip::tcp::socket m_socket;
    asio::strand<asio::any_io_executor> m_strand;

    Message m_incomingMessage{MessageType::None};

    std::deque<std::shared_ptr<const Message>> m_writeQueue;
    static constexpr std::size_t MAX_WRITE_QUEUE_LENGTH = 128;
};

} // namespace Chess

#endif
