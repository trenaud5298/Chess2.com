/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Server/Runtime/Session/Session.hpp>
#include <Chess/Server/Runtime/GameServer.hpp>
#include <Chess/Server/Runtime/Session/SessionManager.hpp>
#include <Chess/Server/Runtime/Session/Target.hpp>

// ASIO Includes

// C++ Includes


namespace Chess {

Session::Session(GameServer& gameServer, asio::ip::tcp::socket&& socket, SessionID id)
: m_gameServer(gameServer), m_socket(std::move(socket)), m_strand(asio::make_strand(m_socket.get_executor())), m_sessionInfo(id) {
    m_gameServer.loggingManager().log(LogEntry::Info("Session[" + std::to_string(m_sessionInfo.id) + "] Connected"));
}

Session::~Session() {}


void Session::start() {
    LifecycleState expected = LifecycleState::STOPPED;
    if (!m_state.compare_exchange_strong(expected, LifecycleState::STARTING)) {
        return;
    }


    m_state = LifecycleState::RUNNING;
    doReadHeader();
}

void Session::stop() {
    auto self = shared_from_this();
    asio::dispatch(m_strand, [this, self]() {
        LifecycleState expected = LifecycleState::RUNNING;
        if (!m_state.compare_exchange_strong(expected, LifecycleState::STOPPING)) {
            return;
        }
        asio::error_code ec;
        m_socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
        m_socket.close(ec);
        m_writeQueue.clear();
        m_state = LifecycleState::STOPPED;
    });
}

void Session::send(std::shared_ptr<const Message> message) {
    auto self = shared_from_this();
    asio::dispatch(m_strand, [this, self, message]() {
        if (m_state != LifecycleState::RUNNING) { return; }
        if (m_writeQueue.size() >= MAX_WRITE_QUEUE_LENGTH) {
            handleError();
            return;
        }
        m_writeQueue.push_back(message);
        if (m_writeQueue.size() == 1) {
            doWrite();
        }
    });
}

void Session::doWrite() {
    if (m_writeQueue.empty()) { return; }
    auto self = shared_from_this();
    asio::async_write(m_socket, m_writeQueue.front()->buffers(), asio::bind_executor(
    m_strand, [this, self](std::error_code ec, std::size_t length) {
        if (ec) {
            handleError();
            return;
        }

        m_writeQueue.pop_front();
        if (!m_writeQueue.empty()) {
            doWrite();
        }
    }));
}

void Session::doReadHeader() {
    m_incomingMessage.clear();
    auto self = shared_from_this();
    asio::async_read(m_socket, m_incomingMessage.headerBuffer(), asio::bind_executor(
    m_strand, [this, self](std::error_code ec, std::size_t length) {
        if (ec) {
            handleError();
            return;
        }
        if (!m_incomingMessage.validateHeader()) {
            handleError();
            return;
        }
        const std::size_t bodyLen = m_incomingMessage.header().bodyLength;
        m_incomingMessage.resize(bodyLen);
        doReadBody();
    }));
}

void Session::doReadBody() {
    auto self = shared_from_this();
    asio::async_read(m_socket, m_incomingMessage.bodyBuffer(), asio::bind_executor(
    m_strand, [this, self](std::error_code ec, std::size_t length) {
        if (ec) {
            handleError();
            return;
        }
        processMessage();
        doReadHeader();
    }));
}

void Session::handleError() {
    if (m_state == LifecycleState::RUNNING) {
        m_gameServer.sessionManager().removeSession(Target::Id({m_sessionInfo.id}));
    }
}

SessionID Session::getId() const {
    return m_sessionInfo.id;
}

SessionInfo Session::getInfo() const {
    return m_sessionInfo;
}

// Below Starts Message Processing. It is safe to assume
// that this function will be called on a strand sequentially
// and that the Session will be active until at least the end
// of the function call.

void Session::processMessage() {
    switch (m_incomingMessage.type()) {
        case NONE:
            break;
        case LOGIN:
            handleLogin();
            break;
        case CHAT:
            handleChat();
            break;
    }
}

void Session::handleLogin() {
    if (m_sessionState != SessionState::LOGIN_REQUIRED) {
        m_gameServer.loggingManager().log(LogEntry::Message("Session[" + std::to_string(m_sessionInfo.id) + "] login attempt failed. Tried to login when already logged in."));
        handleError();
        return;
    }

    std::string loginServerPassword = m_incomingMessage.readString();
    std::string serverPassword = m_gameServer.persistenceManager().settings().general.serverPassword;
    if (loginServerPassword != serverPassword) {
        m_gameServer.loggingManager().log(LogEntry::Message("Session[" + std::to_string(m_sessionInfo.id) + "] login attempt failed. Bad Password: " + loginServerPassword));
        handleError();
        return;
    }
    m_gameServer.loggingManager().log(LogEntry::Message("Session[" + std::to_string(m_sessionInfo.id) + "] login attempt passed."));
    std::string playerName = m_incomingMessage.readString();
    m_sessionInfo.name = playerName;
    m_sessionState = SessionState::IDLE;
}

void Session::handleChat() {
    if (m_sessionState == SessionState::LOGIN_REQUIRED) {
        handleError();
        return;
    }

    std::string msg = m_incomingMessage.readString();
    m_gameServer.loggingManager().log(LogEntry::Message("Session[" + std::to_string(m_sessionInfo.id) + "]/" + m_sessionInfo.name + " " + msg));
    std::shared_ptr<Message> message = std::make_shared<Message>(MessageType::CHAT);
    message->pushString("Server received your message");
    send(message);
}


}
