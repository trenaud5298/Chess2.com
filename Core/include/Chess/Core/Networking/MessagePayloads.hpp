#ifndef CHESS_CORE_NETWORKING_MESSAGEPAYLOADS_HPP
#define CHESS_CORE_NETWORKING_MESSAGEPAYLOADS_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Core/Networking/Message.hpp>

// ASIO Includes

// C++ Includes
#include <cstdint>
#include <optional>
#include <string>
#include <memory>

namespace Chess {

struct LoginRequest {
    static constexpr MessageType type = MessageType::LoginRequest;

    std::string username;
    std::string password;

    Message toMessage() const;
    std::shared_ptr<Message> toSharedMessage() const;
    static std::optional<LoginRequest> fromMessage(Message& msg);
};

struct LoginResponse {
    static constexpr MessageType type = MessageType::LoginResponse;

    bool accepted;
    std::string reason;

    Message toMessage() const;
    std::shared_ptr<Message> toSharedMessage() const;
    static std::optional<LoginResponse> fromMessage(Message& msg);
};

struct Chat {
    static constexpr MessageType type = MessageType::Chat;

    std::string message;

    Message toMessage() const;
    std::shared_ptr<Message> toSharedMessage() const;
    static std::optional<Chat> fromMessage(Message& msg);
};

struct Command {
    static constexpr MessageType type = MessageType::Command;

    std::string command;

    Message toMessage() const;
    std::shared_ptr<Message> toSharedMessage() const;
    static std::optional<Command> fromMessage(Message& msg);
};

struct CreateRoomRequest {
    static constexpr MessageType type = MessageType::CreateRoomRequest;

    std::uint64_t roomID;
    std::string password;

    Message toMessage() const;
    std::shared_ptr<Message> toSharedMessage() const;
    static std::optional<CreateRoomRequest> fromMessage(Message& msg);
};

struct CreateRoomResponse {
    static constexpr MessageType type = MessageType::CreateRoomResponse;

    bool success;
    std::uint64_t roomID;
    std::string reason;

    Message toMessage() const;
    std::shared_ptr<Message> toSharedMessage() const;
    static std::optional<CreateRoomResponse> fromMessage(Message& msg);
};

struct JoinRoomRequest {
    static constexpr MessageType type = MessageType::JoinRoomRequest;

    std::uint64_t roomID;
    std::string password;

    Message toMessage() const;
    std::shared_ptr<Message> toSharedMessage() const;
    static std::optional<JoinRoomRequest> fromMessage(Message& msg);
};

struct JoinRoomResponse {
    static constexpr MessageType type = MessageType::JoinRoomResponse;

    bool success;
    std::string reason;

    Message toMessage() const;
    std::shared_ptr<Message> toSharedMessage() const;
    static std::optional<JoinRoomResponse> fromMessage(Message& msg);
};

struct LeaveRoom {
    static constexpr MessageType type = MessageType::LeaveRoom;

    Message toMessage() const;
    std::shared_ptr<Message> toSharedMessage() const;
    static std::optional<LeaveRoom> fromMessage(Message& msg);
};

struct MakeMove {
    static constexpr MessageType type = MessageType::MakeMove;

    std::uint8_t from;
    std::uint8_t to;

    Message toMessage() const;
    std::shared_ptr<Message> toSharedMessage() const;
    static std::optional<MakeMove> fromMessage(Message& msg);
};

struct GameUpdate {
    static constexpr MessageType type = MessageType::GameUpdate;

    // define later

    Message toMessage() const;
    std::shared_ptr<Message> toSharedMessage() const;
    static std::optional<GameUpdate> fromMessage(Message& msg);
};

struct ErrorMessage {
    static constexpr MessageType type = MessageType::ErrorMessage;

    std::uint32_t errorCode;
    std::string message;

    Message toMessage() const;
    std::shared_ptr<Message> toSharedMessage() const;
    static std::optional<ErrorMessage> fromMessage(Message& msg);
};

using ClientToServerPayloads = std::tuple<
    LoginRequest,
    Chat,
    Command,
    CreateRoomRequest,
    JoinRoomRequest,
    LeaveRoom,
    MakeMove,
    ErrorMessage
>;

using ServerToClientPayloads = std::tuple<
    LoginResponse,
    Chat,
    Command,
    CreateRoomResponse,
    JoinRoomResponse,
    GameUpdate,
    ErrorMessage
>;
}




#endif