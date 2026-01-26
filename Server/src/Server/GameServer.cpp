// GameServer.cpp

// Chess Includes
#include <iostream>
#include <Chess/Server/GameServer.h>
#include <Chess/Server/Session.h>

// ASIO Includes

// C++ Includes
namespace Chess {
   GameServer::GameServer(std::string address, std::uint16_t port, std::size_t threadCount)
      : m_context(), m_acceptor(m_context), m_workGuard(asio::make_work_guard(m_context)),
        m_threadCount(threadCount > 0 ? threadCount : std::thread::hardware_concurrency()),
        m_address(std::move(address)), m_port(port), m_running(false), m_nextSessionID(1)
   {
      asio::ip::tcp::endpoint endpoint(asio::ip::make_address(m_address), m_port);

      asio::error_code ec;
      if (m_acceptor.open(endpoint.protocol(), ec)) {
         throw std::runtime_error("Failed to open acceptor: " + ec.message());
      }

      if (m_acceptor.bind(endpoint, ec)) {
         throw std::runtime_error("Failed to bind acceptor: " + ec.message());
      }

      if (m_acceptor.listen(asio::socket_base::max_listen_connections, ec)) {
         throw std::runtime_error("Failed to listen on acceptor: " + ec.message());
      }
   }

   GameServer::~GameServer() {
      stop();
   }

   void GameServer::start() {
      bool expected = false;
      if (!m_running.compare_exchange_strong(expected, true)) {
         return;
      }

      doAccept();

      for (std::size_t i = 0; i < m_threadCount; ++i) {
         m_threads.emplace_back(std::bind_front(&asio::io_context::run, &m_context));
      }
   }

   void GameServer::stop() {
      bool expected = true;
      if (!m_running.compare_exchange_strong(expected, false)) {
         return;
      }

      asio::post(m_context, std::bind_front(&GameServer::stopOnIO, this));

      for (std::thread& thread : m_threads) {
         if (thread.joinable()) {
            thread.join();
         }
      }
      m_threads.clear();
   }

   void GameServer::doAccept() {
      m_acceptor.async_accept(std::bind_front(&GameServer::handleAccept, this));
   }

   void GameServer::handleAccept(asio::error_code ec, asio::ip::tcp::socket socket) {
      if (!m_running) {
         return;
      }

      if (!ec) {
         std::uint64_t id = m_nextSessionID++;
         std::unique_ptr<Session> newSession = std::make_unique<Session>(std::move(socket), id, this);
         newSession->start();
         m_sessions.emplace(id, std::move(newSession));
      } else {
         std::cerr << "Accept error: " << ec.message() << std::endl;
      }

      if (m_running) {
         doAccept();
      }
   }

   void GameServer::message(std::uint64_t sessionID, std::string message) {
      asio::post(m_context, std::bind_front(&GameServer::messageOnIO, this, sessionID, std::move(message)));
   }

   void GameServer::messageAll(std::string message) {
      asio::post(m_context, std::bind_front(&GameServer::messageAllOnIO, this, std::move(message)));
   }

   void GameServer::stopSession(std::uint64_t sessionID) {
      asio::post(m_context, std::bind_front(&GameServer::stopSessionOnIO, this, sessionID));
   }

   // -----------------ASIO Thread Functions-----------------

   void GameServer::messageOnIO(std::uint64_t sessionID, const std::string& message) {
      auto session = m_sessions.find(sessionID);
      if (session!=m_sessions.end()) {
         session->second->sendMessage(message);
      }
   }

   void GameServer::messageAllOnIO(const std::string& message) {
      for (auto& session : m_sessions) {
         session.second->sendMessage(message);
      }
   }

   void GameServer::stopSessionOnIO(std::uint64_t sessionID) {
      auto session = m_sessions.find(sessionID);
      if (session!=m_sessions.end()) {
         session->second->requestStop();
      }
   }

   void GameServer::onSessionStopped(std::uint64_t sessionID) {
      auto session = m_sessions.find(sessionID);
      if (session!=m_sessions.end()) {
         m_sessions.erase(session);
      }
   }

   void GameServer::stopOnIO() {
      asio::error_code ec;

      m_acceptor.cancel(ec);
      m_acceptor.close(ec);

      for (auto& session : m_sessions) {
         session.second->requestStop();
      }

      m_workGuard.reset();
   }

}
