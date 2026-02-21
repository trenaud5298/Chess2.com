#ifndef CHESS_SERVER__SESSION_SESSION_HPP
#define CHESS_SERVER__SESSION_SESSION_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Core/Networking/Message.hpp>

// ASIO Includes
#include <asio.hpp>

// C++ Includes
#include <deque>
#include <vector>
#include <memory>

namespace Chess {

class GameServer;

struct SessionInfo {
    std::uint32_t id;
};

class Session : public std::enable_shared_from_this<Session> {

public:
    Session(asio::ip::tcp::socket socket, GameServer* gameServer, std::uint32_t id);
    ~Session();

    Session(const Session&) = delete;
    Session& operator=(const Session&) = delete;
    Session(Session&&) = delete;
    Session& operator=(Session&&) = delete;

    void start();
    void stop();

    void send(std::shared_ptr<const Message> message);

    std::uint32_t getId();
    SessionInfo getInfo();

private:
    void doReadHeader();
    void doReadBody();
    void doWrite();
    void processMessage();
    void handleError();

private:
    asio::ip::tcp::socket m_socket;
    asio::strand<asio::any_io_executor> m_strand;
    SessionInfo m_sessionInfo;
    GameServer* m_gameServer;
    bool m_stopping;

    // Reading
    Message m_incomingMessage;
    // Writing
    std::deque<std::shared_ptr<const Message>> m_writeQueue;
    constexpr static std::uint8_t MAX_WRITE_QUEUE_LENGTH = 128;
};


}

#endif