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
    ClientCommandResult result = m_gameClient.shutdown();
    if (!result) {
        std::string message = result.message.empty() ? "Shutdown Failed" : "Shutdown Failed: " + result.message;
        m_gameClient.loggingManager().log(LogEntry::Error(message));
    }
}


}
