/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Server/Runtime/Session/SessionManager.hpp>
#include <Chess/Server/Runtime/GameServer.hpp>
#include <Chess/Server/Runtime/Session/Target.hpp>
#include <Chess/Server/Runtime/Session/Session.hpp>
// ASIO Includes

// C++ Includes

namespace Chess {

SessionManager::SessionManager(GameServer& gameServer) : m_gameServer(gameServer), m_acceptor(m_gameServer.ioContext()) {

}

SessionManager::~SessionManager() {

}

void SessionManager::start() {
    LifecycleState expectedState = LifecycleState::STOPPED;
    if (!m_state.compare_exchange_strong(expectedState, LifecycleState::STARTING)) {
        return;
    }

    asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), m_gameServer.persistenceManager().settings().network.port);

    m_acceptor.open(endpoint.protocol());
    m_acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    m_acceptor.bind(endpoint);
    m_acceptor.listen(asio::socket_base::max_listen_connections);
    m_gameServer.loggingManager().log(LogEntry::Info("Session Manager Started"));
    m_state = LifecycleState::RUNNING;

    startAccept();
}

void SessionManager::stop() {
    LifecycleState expectedState = LifecycleState::RUNNING;
    if (!m_state.compare_exchange_strong(expectedState, LifecycleState::STOPPING)) {
        return;
    }

    asio::error_code ec;
    m_acceptor.close(ec);

    removeSession(Target::All());

    m_state = LifecycleState::STOPPED;
    m_gameServer.loggingManager().log(LogEntry::Info("Session Manager Stopped"));
}


void SessionManager::startAccept() {
    if (m_state != LifecycleState::RUNNING)
        return;

    m_acceptor.async_accept([this](std::error_code ec, asio::ip::tcp::socket socket) {
        if (m_state != LifecycleState::RUNNING) {
            return;
        }
        if (!ec) {
            createSession(std::move(socket));
        }
        startAccept();
    });
}

void Chess::SessionManager::createSession(asio::ip::tcp::socket &&socket) {
    std::unique_lock lock(m_sessionMutex);
    SessionID newSessionId = m_nextSessionId++;
    std::uint64_t newSessionIndex = m_sessions.size();
    std::shared_ptr<Session> newSession = std::make_shared<Session>(m_gameServer, std::move(socket), newSessionId);
    newSession->start();
    m_idToIndex.emplace(newSessionId, newSessionIndex);
    m_sessions.emplace_back(std::move(newSession));
}


void SessionManager::removeSession(const Target& target) {
    std::unique_lock lock(m_sessionMutex);
    std::vector<SessionID> idsToRemove;
    target.forEach(m_idToIndex, m_sessions, [&](Session& session) {
        idsToRemove.push_back(session.getId());
        session.stop();
    });

    for (SessionID id : idsToRemove) {
        // Swap and Pop For Each Removed Session
        SessionID sessionIndex = m_idToIndex[id];
        SessionID lastIndex = static_cast<SessionID>(m_sessions.size() - 1);

        if (sessionIndex != lastIndex) {
            m_sessions[sessionIndex] = std::move(m_sessions[lastIndex]);
            SessionID movedSessionId = m_sessions[sessionIndex]->getId();
            m_idToIndex[movedSessionId] = sessionIndex;
        }

        m_sessions.pop_back();
        m_idToIndex.erase(id);
    }
}

void SessionManager::messageSession(const Target& target, std::shared_ptr<const Message> message) {
    std::shared_lock lock(m_sessionMutex);
    target.forEach(m_idToIndex, m_sessions, [&](Session& session) {
        session.send(message);
    });
}

bool SessionManager::hasSession(const Target& target) const{
    std::shared_lock lock(m_sessionMutex);
    bool result = false;
    target.forEach(m_idToIndex, m_sessions, [&](Session& session) {
        result = true;
    });
    return result;
}

bool SessionManager::empty() const{
    std::shared_lock lock(m_sessionMutex);
    return m_sessions.empty();
}

SessionID SessionManager::sessionCount() const {
    std::shared_lock lock(m_sessionMutex);
    return m_sessions.size();
}

std::vector<SessionID> SessionManager::idSnapshot(const Target& target) const {
    std::shared_lock lock(m_sessionMutex);
    std::vector<SessionID> ids;
    target.forEach(m_idToIndex, m_sessions, [&](Session& session) {
        ids.push_back(session.getId());
    });
    return ids;
}

std::vector<SessionView> SessionManager::infoSnapshot(const Target& target) const {
    std::shared_lock lock(m_sessionMutex);
    std::vector<SessionView> infos;
    target.forEach(m_idToIndex, m_sessions, [&](Session& session) {
        infos.push_back(session.getView());
    });
    return infos;
}

}






