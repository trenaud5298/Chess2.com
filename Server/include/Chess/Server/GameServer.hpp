#ifndef CHESS_SERVER_GAMESERVER_HPP
#define CHESS_SERVER_GAMESERVER_HPP
#include "Session/SessionPool.hpp"
#include "Session/Target.hpp"

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */


// Chess Includes

// ASIO Includes

// C++ Includes


namespace Chess {

class Target;

class GameServer {
public:
    enum class ConnectionType {
        LOCAL_AREA_NETWORK,
        PUBLIC
    };

    struct Config {
        ConnectionType connectionType;
        std::uint32_t connectionPort;
    };

    explicit GameServer(Config config);
    ~GameServer();

    GameServer(const GameServer&) = delete;
    GameServer& operator=(const GameServer&) = delete;

private:


private:
    asio::io_context m_context;
};

}
#endif