#ifndef CHESS_SERVER_RUNTIME_SESSION_SESSIONMANAGER_HPP
#define CHESS_SERVER_RUNTIME_SESSION_SESSIONMANAGER_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Core/Common/LifecycleState.hpp>
#include <Chess/Core/Common/Types.hpp>

// ASIO Includes
#include <asio/ip/tcp.hpp>

// C++ Includes
#include <atomic>
#include <mutex>
#include <shared_mutex>

namespace Chess {

class GameServer;
class Session;
struct SessionView;
class Message;
class Target;

class SessionManager {
public:
    explicit SessionManager(GameServer& gameServer);
    ~SessionManager();

    SessionManager(const SessionManager&) = delete;
    SessionManager& operator=(const SessionManager&) = delete;
    SessionManager(SessionManager&&) = delete;
    SessionManager& operator=(SessionManager&&) = delete;

    // Lifetime Control
    void start();
    void stop();

    // Session Controls
    void removeSession(const Target& target);
    void messageSession(const Target& target, std::shared_ptr<const Message> message);

    // Session Queries
    [[nodiscard]] bool hasSession(const Target& target) const; // Returns True if at least one target exists
    [[nodiscard]] bool empty() const;
    [[nodiscard]] SessionID sessionCount() const;
    [[nodiscard]] std::vector<SessionID> idSnapshot(const Target& target) const;
    [[nodiscard]] std::vector<SessionView> viewSnapshot(const Target& target) const;

private:
    void startAccept();
    void createSession(asio::ip::tcp::socket&& socket);

private:
    GameServer& m_gameServer;
    std::atomic<LifecycleState> m_state{LifecycleState::STOPPED};
    asio::ip::tcp::acceptor m_acceptor;

    // Session Storage
    std::atomic<SessionID> m_nextSessionId{1};
    mutable std::shared_mutex m_sessionMutex;
    std::unordered_map<SessionID, std::uint64_t> m_idToIndex;
    std::vector<std::shared_ptr<Session>> m_sessions;
};

}

#endif