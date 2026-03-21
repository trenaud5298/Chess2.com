// #include <asio.hpp>
// #include <iostream>
// #include <string>
// #include <optional>
// #include <thread>
// #include <vector>
// #include <cctype>
//
// // Chess
// #include <Chess/Core/Networking/Message.hpp>
// #include "Chess/Core/Networking/MessagePayloads.hpp"
// #include <Chess/Client/ClientApplication.hpp>
//
// static constexpr std::uint16_t SERVER_PORT = 24377;
//
// void doReadHeader(asio::ip::tcp::socket& socket, Chess::Message& msg);
// void doReadBody(asio::ip::tcp::socket& socket, Chess::Message& msg);
//
// void doReadHeader(asio::ip::tcp::socket& socket, Chess::Message& msg)
// {
//     msg.clear();
//
//     asio::async_read(socket, msg.headerBuffer(),
//         [&](std::error_code ec, std::size_t)
//         {
//             if (ec) {
//                 std::cout << "Disconnected from server.\n";
//                 return;
//             }
//
//             if (!msg.validateHeader()) {
//                 std::cout << "Invalid header from server.\n";
//                 return;
//             }
//
//             msg.resize(msg.header().bodyLength);
//             doReadBody(socket, msg);
//         });
// }
//
// void doReadBody(asio::ip::tcp::socket& socket, Chess::Message& msg)
// {
//     asio::async_read(socket, msg.bodyBuffer(),
//         [&](std::error_code ec, std::size_t)
//         {
//             if (ec) {
//                 std::cout << "Disconnected from server.\n";
//                 return;
//             }
//
//             // Example: handle Chat and Error messages
//             switch (msg.type()) {
//                 case Chess::MessageType::Chat: {
//                     auto chat = Chess::Chat::fromMessage(msg);
//                     if (chat) std::cout << "\n[Server] " << chat->message << "\n> " << std::flush;
//                     break;
//                 }
//                 case Chess::MessageType::LoginResponse: {
//                     auto loginResponse = Chess::LoginResponse::fromMessage(msg);
//                     if (loginResponse) {
//                         auto payload = *loginResponse;
//                         if (payload.accepted) {
//                             std::cout<<"[Server] Login Successful" << std::endl;
//                         } else {
//                             std::cout<<"[Server] Login Failed: " << payload.reason << std::endl;
//                         }
//                     }
//                     break;
//                 }
//                 case Chess::MessageType::ErrorMessage: {
//                     auto err = Chess::ErrorMessage::fromMessage(msg);
//                     if (err) std::cout << "\n[Server][Error " << err->errorCode << "] "
//                                        << err->message << "\n> " << std::flush;
//                     break;
//                 }
//                 default:
//                     std::cout << "Received unhandled message type.\n";
//                     break;
//             }
//
//             doReadHeader(socket, msg);
//         });
// }
//
// // Parse tokens respecting quoted strings
// static std::vector<std::string> splitArgsRespectQuotes(const std::string& s)
// {
//     std::vector<std::string> out;
//     std::size_t i = 0, n = s.size();
//
//     while (i < n) {
//         while (i < n && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
//         if (i >= n) break;
//
//         if (s[i] == '"') {
//             ++i;
//             std::string token;
//             while (i < n && s[i] != '"') {
//                 if (s[i] == '\\' && i + 1 < n) {
//                     ++i;
//                     token.push_back(s[i]);
//                 } else token.push_back(s[i]);
//                 ++i;
//             }
//             ++i;
//             out.push_back(std::move(token));
//         } else {
//             std::string token;
//             while (i < n && !std::isspace(static_cast<unsigned char>(s[i]))) {
//                 token.push_back(s[i]);
//                 ++i;
//             }
//             out.push_back(std::move(token));
//         }
//     }
//
//     return out;
// }
//
// int main()
// {
//     try {
//         asio::io_context io_context;
//         auto work = asio::make_work_guard(io_context);
//         std::optional<asio::ip::tcp::socket> socket;
//
//         Chess::Message incoming(Chess::MessageType::None);
//
//         std::thread ioThread([&]() { io_context.run(); });
//
//         std::cout << "Client ready.\nCommands:\n"
//                   << "  connect <ip> <name> <password>   (password/name may be quoted)\n"
//                   << "  disconnect\n"
//                   << "  quit\n"
//                   << "  <any other text sends a chat message>\n";
//
//         while (true) {
//             std::cout << "> ";
//             std::string input;
//             if (!std::getline(std::cin, input)) break;
//
//             if (input == "quit") {
//                 if (socket && socket->is_open()) socket->close();
//                 io_context.stop();
//                 break;
//             }
//
//             if (input.rfind("connect ", 0) == 0) {
//                 if (socket && socket->is_open()) {
//                     std::cout << "Already connected.\n";
//                     continue;
//                 }
//
//                 auto tokens = splitArgsRespectQuotes(input.substr(8));
//                 if (tokens.size() < 3) {
//                     std::cout << "Usage: connect <ip> <password> <name>\n";
//                     continue;
//                 }
//
//                 std::string ip = tokens[0];
//                 std::string name = tokens[1];
//                 std::string password = tokens[2];
//
//                 try {
//                     asio::ip::tcp::endpoint endpoint(asio::ip::make_address(ip), SERVER_PORT);
//                     socket.emplace(io_context);
//                     socket->connect(endpoint);
//
//                     std::cout << "Connected to " << ip << ":" << SERVER_PORT << "\n";
//
//                     doReadHeader(*socket, incoming);
//
//                     // Use payload struct to construct LOGIN message
//                     Chess::LoginRequest login{ name, password };
//                     auto loginMsg = login.toMessage();
//                     asio::write(*socket, loginMsg.buffers());
//
//                     std::cout << "Sent LOGIN (name: " << name << ")\n";
//                 }
//                 catch (const std::exception& e) {
//                     socket.reset();
//                     std::cout << "Connection failed: " << e.what() << "\n";
//                 }
//
//                 continue;
//             }
//
//             if (input == "disconnect") {
//                 if (!socket || !socket->is_open()) {
//                     std::cout << "Not connected.\n";
//                     continue;
//                 }
//
//                 socket->close();
//                 socket.reset();
//                 std::cout << "Disconnected.\n";
//                 continue;
//             }
//
//             if (!socket || !socket->is_open()) {
//                 std::cout << "Not connected.\n";
//                 continue;
//             }
//
//             try {
//                 // Use Chat payload struct
//                 Chess::Chat chat{ input };
//                 auto msg = chat.toMessage();
//                 asio::write(*socket, msg.buffers());
//
//                 std::cout << "Sent chat (" << msg.bodySize() << " bytes)\n";
//             }
//             catch (const std::exception& e) {
//                 std::cout << "Send failed: " << e.what() << "\n";
//                 if (socket && socket->is_open()) socket->close();
//                 socket.reset();
//             }
//         }
//
//         work.reset();
//         io_context.stop();
//         ioThread.join();
//     }
//     catch (const std::exception& e) {
//         std::cerr << "Client exception: " << e.what() << "\n";
//     }
//
//     return 0;
// }



#include <Chess/Client/ClientApplication.hpp>

int main() {
    Chess::ClientApplication app;
    app.run();
    return 0;
}
