/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Server/ServerApplication.hpp>

// ASIO Includes

// C++ Includes

namespace Chess {

ServerApplication::ServerApplication() : m_gameServer(), m_serverPanel(m_gameServer) {

}

void ServerApplication::run() {
    m_serverPanel.run();
}
}
