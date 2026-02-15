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
    : m_socket(std::move(socket)), m_sessionInfo(id), m_stopping(false), m_gameServer(gameServer) {
        std::cout << "Session::Session()" << std::endl;
    }

    Session::~Session() {
        std::cout << "Session::~Session()" << std::endl;
    }

    void Session::start() {
        doRead();
    }

    void Session::stop() {
        auto self = shared_from_this();
        asio::post(m_socket.get_executor(), [self]() {
            if (self->m_stopping) {
                return;
            }

            self->m_stopping = true;

            asio::error_code ec;
            self->m_socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
            self->m_socket.close(ec);
            std::cout << "Session::stop()" << std::endl;
        });
    }

    std::uint32_t Session::getId() {
        return m_sessionInfo.id;
    }


    void Session::doRead() {
        std::cout << "Session::doRead() Start" << std::endl;
        auto self = shared_from_this();
        m_socket.async_read_some(asio::buffer(m_readBuffer),[self](std::error_code ec, std::size_t length) {
            if (ec) {
               self->handleError();
               return;
            }

            std::string msg(self->m_readBuffer.data(), self->m_readBuffer.data() + length);
            std::cout << "[Session " << self->m_sessionInfo.id << "] Received: " << msg << std::endl;

            self->doRead();

        });
        std::cout << "Session::doRead() End" << std::endl;
    }

    void Session::handleError() {
        // Call RemoveSession To GameServer (Not Implemented Yet)
        if (!m_stopping) {
            m_gameServer->removeSession(Target::Id({m_sessionInfo.id}));
        }
    }


}
