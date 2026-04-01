/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/ClientApplication.hpp>

// ASIO Includes

// C++ Includes


namespace Chess {


ClientApplication::ClientApplication() : m_gameClient(), m_clientPanel(m_gameClient) {

}

ClientApplication::~ClientApplication() {

}

void ClientApplication::run() {
    m_clientPanel.run();
    m_gameClient.shutdown();
}


}
