#ifndef CHESS_CLIENT_CLIENTAPPLICATION_HPP
#define CHESS_CLIENT_CLIENTAPPLICATION_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/Runtime/GameClient.hpp>
#include <Chess/Client/UI/ClientPanel.hpp>

// ASIO Includes

// C++ Includes


namespace Chess {

class ClientApplication {

public:
    ClientApplication();
    ~ClientApplication();
    void run();

private:
    GameClient m_gameClient;
    ClientPanel m_clientPanel;
};

}

#endif