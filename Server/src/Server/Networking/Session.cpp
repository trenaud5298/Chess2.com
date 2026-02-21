/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Server/Networking/Session.hpp>
#include <Chess/Server/Networking/Target.hpp>
#include <Chess/Server/Networking/SessionPool.hpp>
#include <Chess/Server/GameServer.hpp>

// ASIO Includes

// C++ Includes
#include <iostream>


namespace Chess {

Session::Session(asio::ip::tcp::socket socket, GameServer* gameServer, std::uint32_t id)
: m_socket(std::move(socket)), m_strand(asio::make_strand(m_socket.get_executor())), m_sessionInfo(id), m_stopping(false), m_incomingMessage(MessageType::NONE), m_gameServer(gameServer) {
}

Session::~Session() {
}

void Session::start() {
    auto self = shared_from_this();
    asio::dispatch(m_strand, [self]() {
        self->doReadHeader();
    });
}

void Session::stop() {
    auto self = shared_from_this();
    asio::post(m_strand, [self]() {
        if (self->m_stopping) { return; }
        self->m_stopping = true;

        asio::error_code ec;
        self->m_socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
        self->m_socket.close(ec);
    });
}

void Session::send(std::shared_ptr<const Message> message) {
    auto self = shared_from_this();
    asio::dispatch(m_strand, [this, self, message]() {
        if (m_stopping) {return;}
        if (m_writeQueue.size() >= MAX_WRITE_QUEUE_LENGTH) {
            handleError();
            return;
        }
        bool idle = m_writeQueue.empty();
        m_writeQueue.push_back(message);
        if (idle) {
            doWrite();
        }
    });
}

void Session::doWrite() {
    if (m_writeQueue.empty()) {return;}
    auto self = shared_from_this();
    asio::async_write(m_socket, m_writeQueue.front()->buffers(), asio::bind_executor(m_strand, [this, self](std::error_code ec, std::size_t length) {
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
    asio::async_read(m_socket, m_incomingMessage.headerBuffer(), asio::bind_executor(m_strand, [this, self](std::error_code ec, std::size_t length) {
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
    asio::async_read(m_socket, m_incomingMessage.bodyBuffer(), asio::bind_executor(m_strand, [this, self](std::error_code ec, std::size_t length) {
        if (ec) {
            handleError();
            return;
        }
        processMessage();
        doReadHeader();
    }));
}

void Session::processMessage() {
    std::string msg = m_incomingMessage.readString();
    std::cout<<"Session["<<m_sessionInfo.id<<"]"<<msg<<std::endl;
    std::shared_ptr<Message> message = std::make_shared<Message>(MessageType::CHAT);
    message->pushString("I got your message");
    send(message);
}

void Session::handleError() {
    if (!m_stopping) {
        m_gameServer->removeSession(Target::Id({m_sessionInfo.id}));
    }
}


std::uint32_t Session::getId() {
    return m_sessionInfo.id;
}

SessionInfo Session::getInfo() {
    return m_sessionInfo;
}


}
