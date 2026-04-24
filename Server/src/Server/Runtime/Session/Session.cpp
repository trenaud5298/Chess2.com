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
#include <Chess/Core/Common/TimeFormat.hpp>

// ASIO Includes

// C++ Includes


namespace Chess {

Session::Session(GameServer& gameServer, asio::ip::tcp::socket&& socket, SessionID id)
: m_gameServer(gameServer), m_socket(std::move(socket)), m_strand(asio::make_strand(m_socket.get_executor())), m_id(id) {
    m_gameServer.loggingManager().log(LogEntry::Info("Session[" + std::to_string(getId()) + "] Connected"));
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

void Session::send(std::shared_ptr<const Message> message, PostWriteAction action, QueueWriteMethod queueWriteMethod) {
    auto self = shared_from_this();
    asio::dispatch(m_strand, [this, self, message, action, queueWriteMethod]() {
        if (m_state != LifecycleState::RUNNING) {
            return;
        }
        if (!message) {
            abortSession();
            return;
        }
        if (m_terminalWriteQueued) {
            return;
        }
        if (queueWriteMethod == QueueWriteMethod::Overwrite && !m_writeQueue.empty()) {
            m_writeQueue.erase(std::next(m_writeQueue.begin()), m_writeQueue.end());
        }
        if (m_writeQueue.size() >= MAX_WRITE_QUEUE_LENGTH) {
            abortSession();
            return;
        }
        if (action == PostWriteAction::AbortSession) {
            m_terminalWriteQueued = true;
        }

        m_writeQueue.push_back(OutboundWrite{
            .message = std::move(message),
            .action = action,
        });

        if (m_writeQueue.size() == 1) {
            doWrite();
        }
    });
}

void Session::doWrite() {
    if (m_writeQueue.empty()) { return; }
    auto self = shared_from_this();
    asio::async_write(m_socket, m_writeQueue.front().message->buffers(), asio::bind_executor(
    m_strand, [this, self](std::error_code ec, std::size_t length) {
        if (ec) {
            abortSession();
            return;
        }

        OutboundWrite completed = std::move(m_writeQueue.front());
        m_writeQueue.pop_front();

        switch (completed.action) {
            case PostWriteAction::AbortSession:
                m_terminalWriteQueued = false;
                abortSession();
                return;
            case PostWriteAction::None:
                break;
        }

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
            abortSession();
            return;
        }
        if (!m_incomingMessage.validateHeader()) {
            abortSession();
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
            abortSession();
            return;
        }
        dispatchIncomingMessage();
        doReadHeader();
    }));
}

void Session::abortSession() {
    if (m_state == LifecycleState::RUNNING) {
        m_gameServer.sessionManager().removeSession(Target::Id({getId()}));
    }
}

SessionID Session::getId() const {
    return m_id;
}

SessionState Session::getSessionState() const {
    std::lock_guard lock(m_viewMutex);
    return m_sessionState;
}

std::string Session::getName() const {
    std::lock_guard lock(m_viewMutex);
    return m_name;
}

SessionView Session::getView() const {
    std::lock_guard lock(m_viewMutex);
    return {
        .id = m_id,
        .sessionState = m_sessionState,
        .name = m_name
    };
}

bool Session::isAuthenticated() const {
    return getSessionState() != SessionState::LOGIN_REQUIRED;
}

// Below Starts Message Processing. It is safe to assume
// that this function will be called on a strand sequentially
// and that the Session will be active until at least the end
// of the function call.

void Session::dispatchIncomingMessage() {
    switch (m_incomingMessage.type()) {
        case MessageType::LoginRequest:
            dispatchAs<LoginRequest>();
            break;
        case MessageType::Chat:
            dispatchAs<Chat>();
            break;
        case MessageType::Command:
            dispatchAs<Command>();
            break;
        case MessageType::CreateRoomRequest:
            dispatchAs<CreateRoomRequest>();
            break;
        case MessageType::JoinRoomRequest:
            dispatchAs<JoinRoomRequest>();
            break;
        case MessageType::LeaveRoomRequest:
            dispatchAs<LeaveRoomRequest>();
            break;
        case MessageType::MakeMove:
            dispatchAs<MakeMove>();
            break;
        case MessageType::ErrorMessage:
            dispatchAs<ErrorMessage>();
            break;
        case MessageType::None:
        case MessageType::JoinRoomResponse:
        case MessageType::GameUpdate:
        case MessageType::CreateRoomResponse:
        case MessageType::LoginResponse:
            abortSession();
            break;
    }
}

void Session::handle(const LoginRequest& payload) {
    if (getSessionState() != SessionState::LOGIN_REQUIRED) {
        abortSession();
        return;
    }
    std::string serverPassword = m_gameServer.persistenceManager().settings().general.serverPassword;
    if (payload.password != serverPassword) {
        m_gameServer.loggingManager().log(LogEntry::Message("Session[" + std::to_string(getId()) + "] login attempt failed. Bad Password: " + payload.password));
        send(
            LoginResponse(false, "Incorrect Password").toSharedMessage(),
            PostWriteAction::AbortSession,
            QueueWriteMethod::Overwrite
        );
        return;
    }
    {
        std::lock_guard lock(m_viewMutex);
        m_name = payload.username;
        m_sessionState = SessionState::IDLE;
    }
    m_gameServer.loggingManager().log(LogEntry::Message("Session[" + std::to_string(getId()) + "]/" + payload.username + " login attempt passed."));
    send(LoginResponse(true, "").toSharedMessage());
}

void Session::handle(const Chat& payload) {
    SessionView view = getView();
    if (view.sessionState == SessionState::LOGIN_REQUIRED) {
        abortSession();
        return;
    }

    if (payload.message.empty()) {
        return;
    }

    Chat outbound{
        .scope = payload.scope,
        .message = std::format("[{}][{}] {}", presentLocalTime(), view.name, payload.message)
    };

    Target target = Target::Predicate([](const Session& session) {
        return session.isAuthenticated();
    });

    switch (outbound.scope) {
        case ChatScope::Global:
            m_gameServer.sessionManager().messageSession(target, outbound.toSharedMessage());
            break;

        case ChatScope::Game:
            m_gameServer.sessionManager().messageSession(target, outbound.toSharedMessage());
            break;
    }

    std::string scopeLabel = (outbound.scope == ChatScope::Global) ? "Global" : "Game";

    m_gameServer.loggingManager().log(LogEntry::Message(
        "Session[" + std::to_string(view.id) + "]/" +
        view.name + " [" + scopeLabel + "] " + outbound.message
    ));

}

void Session::handle(const Command& payload) {

}

void Session::handle(const CreateRoomRequest& payload) {

}

void Session::handle(const JoinRoomRequest& payload) {

}

void Session::handle(const LeaveRoomRequest& payload) {

}

void Session::handle(const MakeMove& payload) {

}

void Session::handle(const ErrorMessage& payload) {

}


}
