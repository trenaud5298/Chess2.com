#ifndef CHESS_SERVER_SERVERAPPLICATION_HPP
#define CHESS_SERVER_SERVERAPPLICATION_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Server/Runtime/GameServer.hpp>
#include <Chess/Server/UI/ServerPanel.hpp>

// ASIO Includes

// C++ Includes


namespace Chess {

class ServerApplication {

public:
    ServerApplication();
    ~ServerApplication() = default;
    void run();

private:
    GameServer m_gameServer;
    ServerPanel m_serverPanel;
};

}

#endif