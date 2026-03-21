/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */


// Chess Includes
#include <Chess/Client/Runtime/Logging/LoggingManager.hpp>

// ASIO Includes

// C++ Includes


namespace Chess {

LoggingManager::LoggingManager(GameClient& gameClient) : m_gameClient(gameClient) {

}

LoggingManager::~LoggingManager() {

}


}
