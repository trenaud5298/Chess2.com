#ifndef CHESS_SERVER_RUNTIME_PERSISTENCE_PERSISTENCEMANAGER_HPP
#define CHESS_SERVER_RUNTIME_PERSISTENCE_PERSISTENCEMANAGER_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Server/Runtime/Common/LifecycleState.hpp>
#include <Chess/Server/Runtime/Persistence/Settings.hpp>
#include <Chess/Server/Runtime/Persistence/LogFile.hpp>

// ASIO Includes

// C++ Includes
#include <atomic>

namespace Chess {

class GameServer;

class PersistenceManager {
public:
    PersistenceManager(GameServer& gameServer);
    ~PersistenceManager();

    PersistenceManager(const PersistenceManager&) = delete;
    PersistenceManager& operator=(const PersistenceManager&) = delete;
    PersistenceManager(PersistenceManager&&) = delete;
    PersistenceManager& operator=(PersistenceManager&&) = delete;


    Settings& settings();
    LogFile& logFile();

private:
    GameServer& m_gameServer;

    // Seperate Persistent Systems
    LogFile m_logFile;
    Settings m_settings;
    // Will have more
};

}

#endif