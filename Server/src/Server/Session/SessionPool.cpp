/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Server/Session/SessionPool.hpp>
#include <Chess/Server/Session/Session.hpp>

// ASIO Includes

// C++ Includes


namespace Chess {

SessionPool::SessionPool() : m_nextSessionId(0) {
    m_idToIndex.reserve(128);
    m_sessions.reserve(128);
}

SessionPool::~SessionPool() {
}

std::uint32_t SessionPool::addSession(asio::ip::tcp::socket socket) {
    std::unique_lock lock(m_sessionMutex);
    std::uint32_t newSessionId = m_nextSessionId++;
    std::uint32_t newSessionIndex = m_sessions.size();
    std::shared_ptr<Session> newSession = std::make_shared<Session>(std::move(socket), std::weak_ptr<SessionPool>(this), newSessionId);
    m_idToIndex.emplace(newSessionId, newSessionIndex);
    m_sessions.emplace_back(std::move(newSession));
    return newSessionId;
}


std::uint32_t SessionPool::sessionCount() const
{
    return 0;
}

void SessionPool::message(const Target& target, const Message& msg) {
}


void SessionPool::removeSession(std::uint32_t sessionId)
{
}

    bool SessionPool::hasSession(const Target& target) const
{
    return false;
}

    bool SessionPool::empty() const
{
    return true;
}

std::vector<std::uint32_t> SessionPool::idSnapshot(const Target& target) const
{
    return {};
}

std::vector<SessionInfo> SessionPool::infoSnapshot(const Target& target) const
{
    return {};
}


} // namespace Chess
