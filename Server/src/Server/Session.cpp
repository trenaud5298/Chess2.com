// Server/Session.cpp

// Chess Includes
#include <Chess/Server/Session.h>
#include <Chess/Server/GameServer.h>
// ASIO Includes

// C++ Includes
#include <iostream>
#include <functional>

namespace Chess {
    Session::Session(asio::ip::tcp::socket socket, std::uint64_t id, GameServer* server)
        : m_socket(std::move(socket)), m_id(id), m_server(server) {}

    Session::~Session() {

    }

    void Session::start() {
        asio::post(m_socket.get_executor(), std::bind_front(&Session::doRead, this));
    }

    bool Session::sendMessage(const std::string& message) {
        if (m_stopping) {
            return false;
        }
        asio::post(m_socket.get_executor(), std::bind_front(&Session::enqueueMessageOnIO, this, message));
        return true;
    }

    void Session::requestStop() {
        bool expected = false;
        if (!m_stopping.compare_exchange_strong(expected, true, std::memory_order_acq_rel)) {
            return;
        }

        asio::post(m_socket.get_executor(), std::bind_front(&Session::doStopOnIO, this));
    }

    // -----------------ASIO Thread Functions-----------------

    void Session::doRead() {
        if (m_stopping) {
            return;
        }

        m_socket.async_read_some(asio::buffer(m_readBuffer), std::bind_front(&Session::handleRead, this));
    }

    void Session::handleRead(asio::error_code ec, std::size_t size) {
        if (ec) {
            // Dead Connection Terminate Session
            requestStop();
            return;
        }

        std::string msg(m_readBuffer.data(), size);
        std::cout << "[Session " <<m_id<<"] Received: " << msg << std::endl;
        sendMessage("Server Received Your Message!");
        doRead();
    }

    void Session::enqueueMessageOnIO(std::string message) {
        if (m_stopping) {
            return;
        }

        const bool writeInProfess = !m_writeQueue.empty();
        m_writeQueue.push_back(std::move(message));
        if (!writeInProfess) {
            writeNextMessage();
        }
    }

    void Session::writeNextMessage() {
        if (m_writeQueue.empty()) {
            return;
        }
        asio::async_write(m_socket, asio::buffer(m_writeQueue.front()), std::bind_front(&Session::handleWrite, this));
    }

    void Session::handleWrite(asio::error_code ec, size_t size) {
        if (ec) {
            // Dead Connection Terminate Session
            requestStop();
            return;
        }

        m_writeQueue.pop_front();
        if (!m_writeQueue.empty()) {
            writeNextMessage();
        }
    }

    void Session::doStopOnIO() {
        asio::error_code ec;
        m_socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
        m_socket.close(ec);

        // Call Server To Erase Session
        m_server->onSessionStopped(m_id);
    }
}
