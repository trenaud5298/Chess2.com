#ifndef CHESS_SERVER_GAMESERVER_HPP
#define CHESS_SERVER_GAMESERVER_HPP


/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */


// Chess Includes
#include <Chess/Server/Networking/SessionPool.hpp>
// ASIO Includes

// C++ Includes


namespace Chess {


struct GameServerConfig {
    std::uint16_t port = 24377;
    std::size_t threadCount = std::thread::hardware_concurrency();
};

class Target;

class GameServer {
public:
    GameServer(GameServerConfig config);
    ~GameServer();

    GameServer(const GameServer&) = delete;
    GameServer& operator=(const GameServer&) = delete;
    GameServer(GameServer&&) = delete;
    GameServer& operator=(GameServer&&) = delete;

    // Session management interface (for Sessions to call back)
    void removeSession(const Target& target);



private:
    void doAccept();

private:
    asio::io_context m_context;
    asio::ip::tcp::acceptor m_acceptor;
    std::vector<std::thread> m_threads;
    SessionPool m_sessionPool;
};

}
#endif