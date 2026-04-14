#ifndef CHESS_CLIENT_RUNTIME_NETWORK_CLIENTSESSION_HPP
#define CHESS_CLIENT_RUNTIME_NETWORK_CLIENTSESSION_HPP

// Chess Includes
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

// Debating if callbacks should have no parameters
// and if quereys are required for extra info.
// But that could basically just be an event based
// callback system like I already have.
struct ClientSessionCallbacks {
    std::function<void()> connectResult;
    std::function<void()> disconnectResult;
    std::function<void()> sendResult;
};

class ClientSession {
public:
    explicit ClientSession(asio::io_context& context);
    ~ClientSession();

    ClientSession(const ClientSession&) = delete;
    ClientSession& operator=(const ClientSession&) = delete;
    ClientSession(ClientSession&&) = delete;
    ClientSession& operator=(ClientSession&&) = delete;

    // Client Session Commands
    void requestConnect();
    void requestDisconnect();
    void requestSend(std::shared_ptr<const Message> message);

    // Client Session Info

private:
    // Might Use Event Or Callback based system
    void fireEvent();

private:
    asio::io_context& m_context;
    asio::ip::tcp::socket m_socket;
    asio::strand<asio::any_io_executor> m_strand;
    asio::steady_timer m_connectTimer;

    // Read & Write Data
    Message m_incomingMessage{MessageType::None};
    std::deque<std::shared_ptr<const Message>> m_writeQueue;
    static constexpr std::size_t MAX_WRITE_QUEUE_LENGTH = 128;
};

class ClientSessionOld {
public:
    ClientSession(asio::io_context& ioContext);
    ~ClientSession();

    ClientSession(const ClientSession&) = delete;
    ClientSession& operator=(const ClientSession&) = delete;
    ClientSession(ClientSession&&) = delete;
    ClientSession& operator=(ClientSession&&) = delete;

    [[nodiscard]] bool requestConnect(const ServerInfo& server);
    [[nodiscard]] bool requestDisconnect();
    [[nodiscard]] bool requestSend(std::shared_ptr<const Message> message, std::function<void()> onResponse);

    [[nodiscard]] bool isConnected() const noexcept { return m_isConnected.load(); }

private:
    void doConnect(asio::ip::tcp::endpoint endpoint);
    void doReadHeader();
    void doReadBody();
    void doWrite();

    void handleDisconnect(MultiplayerErrorCode code, std::string message);
    void closeSocket();

private:
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
