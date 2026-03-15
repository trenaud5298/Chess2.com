/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Server/Runtime/Logging/LoggingManager.hpp>
#include <Chess/Server/Runtime/GameServer.hpp>

// ASIO Includes

// C++ Includes

namespace Chess {

LoggingManager::LoggingManager(GameServer& gameServer) : m_gameServer(gameServer) {

}

LoggingManager::~LoggingManager() {

}

void LoggingManager::log(const LogEntry& entry) {
    std::shared_lock<std::shared_mutex> lock(m_handlerMutex);
    for (const LogHandler& handler : m_handlers) {
        handler.dispatch(entry);
    }
}

std::uint64_t LoggingManager::addHandler(std::uint64_t mask, std::function<void(const LogEntry&)> handler) {
    std::unique_lock<std::shared_mutex> lock(m_handlerMutex);
    std::uint64_t handlerId = m_nextHandlerID++;
    m_handlers.emplace_back(handlerId, mask, std::move(handler));
    return handlerId;
}

bool LoggingManager::removeHandler(std::uint64_t handlerId) {
    std::unique_lock<std::shared_mutex> lock(m_handlerMutex);
    for (auto it = m_handlers.begin(); it != m_handlers.end(); ++it) {
        if (it->m_id == handlerId) {
            m_handlers.erase(it);
            return true;
        }
    }
    return false;
}


}
