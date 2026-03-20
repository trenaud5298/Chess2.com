#ifndef CHESS_SERVER__SESSION_SESSION_HPP
#define CHESS_SERVER__SESSION_SESSION_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Core/Networking/Message.hpp>
#include <Chess/Server/Runtime/Common/Types.hpp>
#include <Chess/Server/Runtime/Common/LifecycleState.hpp>

// ASIO Includes
#include <asio.hpp>

// C++ Includes
#include <deque>
#include <vector>
#include <memory>
#include <atomic>

namespace Chess {

class GameServer;

struct SessionInfo {
    SessionID id;
    std::string name;
};

enum SessionState {
    LOGIN_REQUIRED,
    IDLE,
    IN_MATCH
};

class Session : public std::enable_shared_from_this<Session> {
public:
    explicit Session(GameServer& gameServer, asio::ip::tcp::socket&& socket, SessionID id);
    ~Session();

    Session(const Session&) = delete;
    Session& operator=(const Session&) = delete;
    Session(Session&&) = delete;
    Session& operator=(Session&&) = delete;

    // Lifecycle (Destruction after Stop through Shared Pointers; Can not be restarted)
    void start();
    void stop();

    // Controls
    void send(std::shared_ptr<const Message> message);

    // Queries
    SessionID getId() const;
    SessionInfo getInfo() const;

private:
    void doReadHeader();
    void doReadBody();
    void doWrite();
    void handleError();
    void processMessage();
        void handleLogin();
        void handleChat();
        void handleCreateRoom();
        void handleJoinRoom();
        void handleLeaveRoom();
        void handleMakeMove();
private:
    // Server
    GameServer& m_gameServer;

    // State
    std::atomic<LifecycleState> m_state{LifecycleState::STOPPED};
    std::atomic<SessionState> m_sessionState{SessionState::LOGIN_REQUIRED};

    // Networking
    asio::ip::tcp::socket m_socket;
    asio::strand<asio::any_io_executor> m_strand;

    // Info
    SessionInfo m_sessionInfo;

    // Reading
    Message m_incomingMessage{MessageType::NONE};

    // Writing
    std::deque<std::shared_ptr<const Message>> m_writeQueue;
    constexpr static std::size_t MAX_WRITE_QUEUE_LENGTH = 128;
};


}

#endif