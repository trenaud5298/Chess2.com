#ifndef CHESS_SERVER_GAMESERVER_H
#define CHESS_SERVER_GAMESERVER_H

// Chess Includes

// ASIO Includes
#include <asio.hpp>

// C++ Includes
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <unordered_map>
#include <memory>

namespace Chess {
    class Session;

    class GameServer {
    public:
        GameServer(std::string address, std::uint16_t port, std::size_t threadCount);
        ~GameServer();

        void start();
        void stop();

        void message(std::uint64_t sessionID, std::string message);
        void messageAll(std::string message);
        void stopSession(std::uint64_t sessionID);

    private:
        void doAccept();
        void handleAccept(asio::error_code ec, asio::ip::tcp::socket socket);

        void messageOnIO(std::uint64_t sessionID, const std::string& message);
        void messageAllOnIO(const std::string& message);
        void stopSessionOnIO(std::uint64_t sessionID);

        void onSessionStopped(std::uint64_t sessionID);

        void stopOnIO();

    private:
        asio::io_context m_context;
        asio::ip::tcp::acceptor m_acceptor;
        asio::executor_work_guard<asio::io_context::executor_type> m_workGuard;

        std::vector<std::thread> m_threads;
        std::size_t m_threadCount;

        const std::string m_address;
        const std::uint16_t m_port;

        std::atomic_bool m_running{false};
        std::uint64_t m_nextSessionID{1};

        std::unordered_map<std::uint64_t, std::shared_ptr<Session>> m_sessions;

        friend class Session;
    };

    class Session;
}

#endif