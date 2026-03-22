// #ifndef CHESS_CLIENT_RUNTIME_NETWORK_CLIENTSESSION_HPP
// #define CHESS_CLIENT_RUNTIME_NETWORK_CLIENTSESSION_HPP
//
// /*
//  * Chess
//  * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
//  *
//  * This source code is licensed under the BSD 3-Clause License.
//  * See the LICENSE file in the root of the source tree for details.
//  */
//
// // Chess Includes
// #include <Chess/Client/Common/ServerInfo.hpp>
// #include <Chess/Core/Networking/Message.hpp>
// #include <Chess/Core/Networking/MessagePayloads.hpp>
//
// // ASIO Includes
// #include <asio/ip/tcp.hpp>
// #include <asio/strand.hpp>
// #include <asio/steady_timer.hpp>
//
// // C++ Includes
// #include <atomic>
// #include <memory>
// #include <deque>
//
// namespace Chess {
//
// class GameClient;
// class Message;
//
// class ClientSession {
//
// public:
//     explicit ClientSession(GameClient& gameClient);
//     ~ClientSession();
//
//     ClientSession(const ClientSession&) = delete;
//     ClientSession& operator=(const ClientSession&) = delete;
//     ClientSession(ClientSession&&) = delete;
//     ClientSession& operator=(ClientSession&&) = delete;
//
//     bool connectAndLogin(ServerInfo info);
//     void disconnect();
//
//     void send(std::shared_ptr<const Message> msg);
//
//     [[nodiscard]] bool isConnected() const noexcept { return m_isConnected.load(); }
//
// private:
//     void doConnect();
//
//     void doReadHeader();
//     void doReadBody();
//     void doWrite();
//
//     void dispatchIncomingMessage();
//     template <typename T>
//     bool dispatchAs() {
//         if (auto payload = T::fromMessage(m_incomingMessage)) {
//             handle(*payload);
//             return true;
//         }
//         abortSession();
//         return false;
//     }
//
//     void handle(const LoginResponse& payload);
//     void handle(const Chat& payload);
//     void handle(const CreateRoomResponse& payload);
//     void handle(const JoinRoomResponse& payload);
//     void handle(const GameUpdate& payload);
//     void handle(const ErrorMessage& payload);
//
//     void abortSession();
//
//
// private:
//     GameClient& m_gameClient;
//
//     std::atomic<bool> m_isConnected{false};
//
//     asio::ip::tcp::socket m_socket;
//     asio::strand<asio::io_context::executor_type> m_strand;
//     asio::steady_timer m_connectTimer;
//
//     // Reading
//     Message m_incomingMessage{MessageType::None};
//
//     // Writing
//     std::deque<std::shared_ptr<const Message>> m_writeQueue;
//     constexpr static std::size_t MAX_WRITE_QUEUE_LENGTH = 128;
// };
//
//
// }
//
// #endif