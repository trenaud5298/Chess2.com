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
#include <Chess/Core/Common/Types.hpp>
#include <Chess/Core/Game/ChessGame.hpp>

// ASIO Includes

// C++ Includes
#include <cstdint>
#include <optional>
#include <string>
#include <memory>

namespace Chess {

enum class RoomMemberType : std::uint8_t {
    None = 0,
    Player = 1,
    Spectator = 2
};

struct RoomSummary {
    RoomID roomID{0};
    std::string whitePlayerName;
    std::string blackPlayerName;
    std::uint16_t spectatorCount{0};
    bool hasOpenPlayerSeat{false};
    bool inProgress{false};
};


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

enum class ChatScope : std::uint8_t {Global = 0, Game = 1};
struct Chat {
    static constexpr MessageType type = MessageType::Chat;

    ChatScope scope{ChatScope::Global};
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

    Message toMessage() const;
    std::shared_ptr<Message> toSharedMessage() const;
    static std::optional<CreateRoomRequest> fromMessage(Message& msg);
};

struct CreateRoomResponse {
    static constexpr MessageType type = MessageType::CreateRoomResponse;

    bool success{false};
    RoomID roomID;
    RoomMemberType memberType{RoomMemberType::None};
    COLOR color{COLOR::EMPTY};
    std::string reason;

    Message toMessage() const;
    std::shared_ptr<Message> toSharedMessage() const;
    static std::optional<CreateRoomResponse> fromMessage(Message& msg);
};

struct ListRoomsRequest {
    static constexpr MessageType type = MessageType::ListRoomsRequest;

    Message toMessage() const;
    std::shared_ptr<Message> toSharedMessage() const;
    static std::optional<ListRoomsRequest> fromMessage(Message& msg);
};

struct ListRoomsResponse {
    static constexpr MessageType type = MessageType::ListRoomsResponse;

    std::vector<RoomSummary> rooms;

    Message toMessage() const;
    std::shared_ptr<Message> toSharedMessage() const;
    static std::optional<ListRoomsResponse> fromMessage(Message& msg);
};

struct JoinRoomRequest {
    static constexpr MessageType type = MessageType::JoinRoomRequest;

    RoomID roomID;
    bool spectator{false};

    Message toMessage() const;
    std::shared_ptr<Message> toSharedMessage() const;
    static std::optional<JoinRoomRequest> fromMessage(Message& msg);
};

struct JoinRoomResponse {
    static constexpr MessageType type = MessageType::JoinRoomResponse;

    bool success{false};
    RoomID roomID{0};
    RoomMemberType memberType{RoomMemberType::None};
    COLOR color{COLOR::EMPTY};
    std::string reason;

    Message toMessage() const;
    std::shared_ptr<Message> toSharedMessage() const;
    static std::optional<JoinRoomResponse> fromMessage(Message& msg);
};

struct LeaveRoomRequest {
    static constexpr MessageType type = MessageType::LeaveRoomRequest;

    Message toMessage() const;
    std::shared_ptr<Message> toSharedMessage() const;
    static std::optional<LeaveRoomRequest> fromMessage(Message& msg);
};

struct LeaveRoomResponse {
    static constexpr MessageType type = MessageType::LeaveRoomResponse;

    bool success{false};
    RoomID roomID;
    std::string reason;

    Message toMessage() const;
    std::shared_ptr<Message> toSharedMessage() const;
    static std::optional<LeaveRoomResponse> fromMessage(Message& msg);
};

struct MakeMove {
    static constexpr MessageType type = MessageType::MakeMove;

    std::uint8_t from;
    std::uint8_t to;
    PromotionPiece promotion{PromotionPiece::None};

    Message toMessage() const;
    std::shared_ptr<Message> toSharedMessage() const;
    static std::optional<MakeMove> fromMessage(Message& msg);
};

struct GameUpdate {
    static constexpr MessageType type = MessageType::GameUpdate;

    // define later
    RoomID roomID;
    std::uint64_t roomVersion{0};
    std::string whitePlayerName;
    std::string blackPlayerName;
    std::uint16_t spectatorCount{0};
    ChessGameSnapshot snapshot{};

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
    ListRoomsRequest,
    JoinRoomRequest,
    LeaveRoomRequest,
    MakeMove,
    ErrorMessage
>;

using ServerToClientPayloads = std::tuple<
    LoginResponse,
    Chat,
    Command,
    CreateRoomResponse,
    ListRoomsResponse,
    JoinRoomResponse,
    LeaveRoomResponse,
    GameUpdate,
    ErrorMessage
>;
}




#endif