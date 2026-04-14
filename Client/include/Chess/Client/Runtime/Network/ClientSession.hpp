#ifndef CHESS_CLIENT_RUNTIME_NETWORK_CLIENTSESSION_HPP
#define CHESS_CLIENT_RUNTIME_NETWORK_CLIENTSESSION_HPP

// Chess Includes
#include <Chess/Client/Common/ServerInfo.hpp>
#include <Chess/Client/Runtime/MultiplayerTypes.hpp>
#include <Chess/Core/Networking/Message.hpp>

// ASIO Includes
#include <asio/ip/tcp.hpp>
#include <asio/strand.hpp>
#include <asio/steady_timer.hpp>

// C++ Includes
#include <atomic>
#include <deque>
#include <functional>
#include <memory>
#include <string>

namespace Chess {

// Might go with something else; undecided
struct ClientSessionCallbacks {
    std::function<void()> onConnected;
    std::function<void(MultiplayerErrorCode, std::string)> onConnectFailed;
    std::function<void(Message)> onMessageReceived;
    std::function<void(MultiplayerErrorCode, std::string)> onDisconnected;
};

class ClientSession {
public:
    ClientSession(asio::io_context& ioContext, ClientSessionCallbacks callbacks);
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
    void doConnect(asio::ip::tcp::endpoint endpoint);
    void doReadHeader();
    void doReadBody();
    void doWrite();

    void handleDisconnect(MultiplayerErrorCode code, std::string message);
    void closeSocket();

private:
    ClientSessionCallbacks m_callbacks;

    std::atomic<bool> m_isConnected{false};

    asio::ip::tcp::socket m_socket;
    asio::strand<asio::any_io_executor> m_strand;
    asio::steady_timer m_connectTimer;

    Message m_incomingMessage{MessageType::None};

    std::deque<std::shared_ptr<const Message>> m_writeQueue;
    static constexpr std::size_t MAX_WRITE_QUEUE_LENGTH = 128;
};

} // namespace Chess

#endif
