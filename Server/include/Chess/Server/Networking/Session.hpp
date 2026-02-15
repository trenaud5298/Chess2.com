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
#include <array>

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

    std::uint32_t getId();

private:
    void doRead();
    void handleError();

private:
    asio::ip::tcp::socket m_socket;
    SessionInfo m_sessionInfo;
    bool m_stopping;
    std::array<std::uint8_t, 4096> m_readBuffer{};

    GameServer* m_gameServer;
};


}

#endif