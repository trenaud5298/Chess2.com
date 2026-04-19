#ifndef CHESS_SERVER_SESSION_SESSION_HPP
#define CHESS_SERVER_SESSION_SESSION_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Core/Networking/Message.hpp>
#include <Chess/Core/Common/Types.hpp>
#include <Chess/Core/Common/LifecycleState.hpp>
#include <Chess/Core/Networking/MessagePayloads.hpp>

// ASIO Includes
#include <asio.hpp>

// C++ Includes
#include <deque>
#include <vector>
#include <memory>
#include <atomic>

namespace Chess {

class GameServer;

enum SessionState {
    LOGIN_REQUIRED,
    IDLE,
    IN_MATCH
};

struct SessionView {
    SessionID id;
    SessionState sessionState;
    std::string name;
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
    SessionState getSessionState() const;
    std::string getName() const;
    SessionView getView() const;
    bool isAuthenticated() const;

private:
    void doReadHeader();
    void doReadBody();
    void doWrite();
    void abortSession();
    void dispatchIncomingMessage();
    template <typename T>
    bool dispatchAs() {
        if (auto payload = T::fromMessage(m_incomingMessage)) {
            handle(*payload);
            return true;
        }
        abortSession();
        return false;
    }

    void handle(const LoginRequest& payload);
    void handle(const Chat& payload);
    void handle(const Command& payload);
    void handle(const CreateRoomRequest& payload);
    void handle(const JoinRoomRequest& payload);
    void handle(const LeaveRoom& payload);
    void handle(const MakeMove& payload);
    void handle(const ErrorMessage& payload);

private:
    // Server
    GameServer& m_gameServer;

    // State
    std::atomic<LifecycleState> m_state{LifecycleState::STOPPED};

    mutable std::mutex m_viewMutex;
    const SessionID m_id; // Id is constant and can thus not use mutex
    SessionState m_sessionState{SessionState::LOGIN_REQUIRED};
    std::string m_name{};

    // Networking
    asio::ip::tcp::socket m_socket;
    asio::strand<asio::any_io_executor> m_strand;


    // Reading
    Message m_incomingMessage{MessageType::None};

    // Writing
    std::deque<std::shared_ptr<const Message>> m_writeQueue;
    constexpr static std::size_t MAX_WRITE_QUEUE_LENGTH = 128;
};


}

#endif