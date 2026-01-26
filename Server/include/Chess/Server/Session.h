#ifndef CHESS_SERVER_SESSION_H
#define CHESS_SERVER_SESSION_H

// Chess Includes

// ASIO Includes
#include <asio.hpp>

// C++ Includes
#include <deque>
#include <cstdint>
#include <array>
#include <atomic>
#include <deque>

namespace Chess {
    class GameServer;

    class Session {
    public:
        explicit Session(asio::ip::tcp::socket socket, std::uint64_t id, GameServer* server);
        ~Session();

        void start();
        bool sendMessage(const std::string& message);
        void requestStop();

    private:
        void doRead();
        void handleRead(asio::error_code ec, std::size_t size);

        void enqueueMessageOnIO(std::string message);
        void writeNextMessage();
        void handleWrite(asio::error_code ec, std::size_t size);

        void doStopOnIO();
    private:
        std::uint64_t m_id;
        asio::ip::tcp::socket m_socket;
        GameServer* m_server;

        std::array<char, 4096> m_readBuffer;
        std::deque<std::string> m_writeQueue;

        std::atomic_bool m_stopping{false};
    };

}

#endif