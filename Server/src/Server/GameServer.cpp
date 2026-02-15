/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Server/GameServer.hpp>
#include <Chess/Server/Networking/Target.hpp>
// ASIO Includes

// C++ Includes
#include <iostream>


namespace Chess {

GameServer::GameServer(GameServerConfig config)
: m_acceptor(m_context), m_sessionPool(this) {
    if (config.threadCount < 1) { config.threadCount = 1; }

    asio::ip::tcp::endpoint endpoint = asio::ip::tcp::endpoint(asio::ip::tcp::v4(), config.port);

    asio::error_code ec;
    m_acceptor.open(endpoint.protocol(), ec);
    if (ec) throw std::runtime_error("GameServer::GameServer(): Failed To Open Acceptor. Error: " + ec.message());
    m_acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true), ec);
    if (ec) throw std::runtime_error("GameServer::GameServer(): Failed To Set Acceptor Option. Error: " + ec.message());
    m_acceptor.bind(endpoint, ec);
    if (ec) throw std::runtime_error("GameServer::GameServer(): Failed To Bind Acceptor. Error: " + ec.message());
    m_acceptor.listen(asio::socket_base::max_listen_connections, ec);
    if (ec) throw std::runtime_error("GameServer::GameServer(): Failed To Listen On Acceptor. Error: " + ec.message());

    doAccept();

    for (std::size_t i = 0; i < config.threadCount; i++) {
        m_threads.emplace_back([this]() {
            m_context.run();
        });
    }
    std::cout << "[GameServer] Starting GameServer On Port: "<<config.port << std::endl;
}
GameServer::~GameServer() {
    asio::error_code ec;
    m_acceptor.close(ec);
    m_sessionPool.removeSession(Target::All());
    m_context.stop();
    for (std::thread& t : m_threads) {
        if (t.joinable()) {t.join();}
    }

    std::cout << "[GameServer] Shutdown complete" << std::endl;
}

void GameServer::removeSession(const Target &target) {
    m_sessionPool.removeSession(target);
}



void GameServer::doAccept() {
    m_acceptor.async_accept([this](asio::error_code ec, asio::ip::tcp::socket socket) {
        if (ec==asio::error::operation_aborted) {
            return;
        }
        if (ec) {
            std::cout<<"Error On Accpet()"<<ec.message()<<std::endl;
            doAccept();
        }

        m_sessionPool.addSession(std::move(socket));

    });
}


}
