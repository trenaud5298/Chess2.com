/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <iostream>
#include <Chess/Server/Networking/SessionPool.hpp>
#include <Chess/Server/Networking/Session.hpp>
#include <Chess/Server/Networking/Target.hpp>

#include <Chess/Server/GameServer.hpp>

// ASIO Includes

// C++ Includes


namespace Chess {

SessionPool::SessionPool(GameServer* gameServer) : m_nextSessionId(0), m_gameServer(gameServer) {
    m_idToIndex.reserve(128);
    m_sessions.reserve(128);
}

SessionPool::~SessionPool() {
    if (!m_sessions.empty()) {
        std::cerr << "SessionPool::~SessionPool() Called When Session Cleanup Is Not Completed!" << std::endl;
    }
}

std::uint32_t SessionPool::addSession(asio::ip::tcp::socket socket) {
    std::unique_lock lock(m_sessionMutex);
    std::uint32_t newSessionId = m_nextSessionId++;
    std::uint32_t newSessionIndex = m_sessions.size();
    std::shared_ptr<Session> newSession = std::make_shared<Session>(std::move(socket), m_gameServer, newSessionId);
    newSession->start();
    m_idToIndex.emplace(newSessionId, newSessionIndex);
    m_sessions.emplace_back(std::move(newSession));
    return newSessionId;
}


void SessionPool::removeSession(const Target& target) {
    std::unique_lock lock(m_sessionMutex);
    std::vector<std::uint32_t> idsToRemove;
    target.forEach(m_idToIndex, m_sessions, [&](Session& session) {
        idsToRemove.push_back(session.getId());
        session.stop();
    });

    for (std::uint32_t id : idsToRemove) {
        // Swap and Pop For Each Removed Session
        std::uint32_t sessionIndex = m_idToIndex[id];
        std::uint32_t lastIndex = static_cast<std::uint32_t>(m_sessions.size() - 1);

        if (sessionIndex != lastIndex) {
            m_sessions[sessionIndex] = std::move(m_sessions[lastIndex]);
            std::uint32_t movedSessionId = m_sessions[sessionIndex]->getId();
            m_idToIndex[movedSessionId] = sessionIndex;
        }

        m_sessions.pop_back();
        m_idToIndex.erase(id);
    }
}


std::uint32_t SessionPool::sessionCount() const {
    std::shared_lock lock(m_sessionMutex);
    return m_sessions.size();
}

void SessionPool::message(const Target& target, const Message& msg) {
    std::shared_lock lock(m_sessionMutex);
    target.forEach(m_idToIndex, m_sessions, [&](Session& session) {
        // TODO: session.message(msg)
    });
}




bool SessionPool::hasSession(const Target& target) const{
    std::shared_lock lock(m_sessionMutex);

    return false;
}

bool SessionPool::empty() const{
    std::shared_lock lock(m_sessionMutex);
    return m_sessions.empty();
}

std::vector<std::uint32_t> SessionPool::idSnapshot(const Target& target) const {
    std::shared_lock lock(m_sessionMutex);
    std::vector<std::uint32_t> ids;
    target.forEach(m_idToIndex, m_sessions, [&](Session& session) {
        ids.push_back(session.getId());
    });
    return ids;
}

std::vector<SessionInfo> SessionPool::infoSnapshot(const Target& target) const {
    return {};
}


} // namespace Chess
