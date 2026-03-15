/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Server/Runtime/Persistence/PersistenceManager.hpp>

// ASIO Includes

// C++ Includes

namespace Chess {

PersistenceManager::PersistenceManager(GameServer& gameServer)
: m_gameServer(gameServer),
m_settings(),
m_logFile(m_gameServer) {

}

PersistenceManager::~PersistenceManager() {

}


}
