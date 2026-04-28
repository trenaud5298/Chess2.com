/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Core/Networking/MessagePayloads.hpp>

// ASIO Includes

// C++ Includes



namespace Chess {


namespace {

void pushDurationMs(Message& msg, std::chrono::milliseconds value) {
    msg.push<std::int64_t>(value.count());
}

bool tryReadDurationMs(Message& msg, std::chrono::milliseconds& value) {
    std::int64_t raw{0};
    if (!msg.tryRead(raw)) {
        return false;
    }
    value = std::chrono::milliseconds(raw);
    return true;
}

void pushSpectatorConfig(Message& msg, const RoomSpectatorConfig& value) {
    msg.push(value.allowSpectators);
    msg.push(value.spectatorsCanChat);
    msg.push(value.maxSpectators);
    msg.push(value.allowMidgameJoin);
}

bool tryReadSpectatorConfig(Message& msg, RoomSpectatorConfig& value) {
    return msg.tryRead(value.allowSpectators) &&
           msg.tryRead(value.spectatorsCanChat) &&
           msg.tryRead(value.maxSpectators) &&
           msg.tryRead(value.allowMidgameJoin);
}

void pushRoomGameConfig(Message& msg, const RoomGameConfig& value) {
    msg.push(value.clock.enabled);
    pushDurationMs(msg, value.clock.initialTime);
    pushDurationMs(msg, value.clock.increment);
}

bool tryReadRoomGameConfig(Message& msg, RoomGameConfig& value) {
    return msg.tryRead(value.clock.enabled) &&
           tryReadDurationMs(msg, value.clock.initialTime) &&
           tryReadDurationMs(msg, value.clock.increment);
}

void pushRoomPresentationConfig(Message& msg, const RoomPresentationConfig& value) {
    msg.pushString(value.name);
}

bool tryReadRoomPresentationConfig(Message& msg, RoomPresentationConfig& value) {
    return msg.tryReadString(value.name);
}

void pushRoomAccessConfig(Message& msg, const RoomAccessConfig& value) {
    msg.pushString(value.password);
    msg.push(value.visibleInLobby);
}

bool tryReadRoomAccessConfig(Message& msg, RoomAccessConfig& value) {
    return msg.tryReadString(value.password) &&
           msg.tryRead(value.visibleInLobby);
}

void pushRoomCreateConfig(Message& msg, const RoomCreateConfig& value) {
    pushSpectatorConfig(msg, value.spectator);
    pushRoomGameConfig(msg, value.game);
    pushRoomPresentationConfig(msg, value.presentation);
    pushRoomAccessConfig(msg, value.access);
}

bool tryReadRoomCreateConfig(Message& msg, RoomCreateConfig& value) {
    return tryReadSpectatorConfig(msg, value.spectator) &&
           tryReadRoomGameConfig(msg, value.game) &&
           tryReadRoomPresentationConfig(msg, value.presentation) &&
           tryReadRoomAccessConfig(msg, value.access);
}

void pushPublicRoomConfig(Message& msg, const PublicRoomConfig& value) {
    pushSpectatorConfig(msg, value.spectator);
    pushRoomGameConfig(msg, value.game);
    pushRoomPresentationConfig(msg, value.presentation);
    msg.push(value.passwordProtected);
    msg.push(value.visibleInLobby);
}

bool tryReadPublicRoomConfig(Message& msg, PublicRoomConfig& value) {
    return tryReadSpectatorConfig(msg, value.spectator) &&
           tryReadRoomGameConfig(msg, value.game) &&
           tryReadRoomPresentationConfig(msg, value.presentation) &&
           msg.tryRead(value.passwordProtected) &&
           msg.tryRead(value.visibleInLobby);
}

void pushRoomSummary(Message& msg, const RoomSummary& value) {
    msg.push(value.roomID);
    pushPublicRoomConfig(msg, value.config);
    msg.pushString(value.whitePlayerName);
    msg.pushString(value.blackPlayerName);
    msg.push(value.spectatorCount);
    msg.push(value.hasOpenPlayerSeat);
    msg.push(value.inProgress);
}

bool tryReadRoomSummary(Message& msg, RoomSummary& value) {
    return msg.tryRead(value.roomID) &&
           tryReadPublicRoomConfig(msg, value.config) &&
           msg.tryReadString(value.whitePlayerName) &&
           msg.tryReadString(value.blackPlayerName) &&
           msg.tryRead(value.spectatorCount) &&
           msg.tryRead(value.hasOpenPlayerSeat) &&
           msg.tryRead(value.inProgress);
}

void pushChessGameSnapshot(Message& msg, const ChessGameSnapshot& value) {
    for (const ID square : value.board) {
        msg.push(square);
    }

    msg.push(value.state);
    msg.push(value.currentTurn);
    msg.push(value.winner);
    msg.push(value.endReason);
    msg.push(value.clockConfig.enabled);
    pushDurationMs(msg, value.clockConfig.initialTime);
    pushDurationMs(msg, value.clockConfig.increment);
    pushDurationMs(msg, value.whiteTimeRemaining);
    pushDurationMs(msg, value.blackTimeRemaining);
    msg.push(value.version);
}

bool tryReadChessGameSnapshot(Message& msg, ChessGameSnapshot& value) {
    for (ID& square : value.board) {
        if (!msg.tryRead(square)) { return false; }
    }

    if (!msg.tryRead(value.state) ||
        !msg.tryRead(value.currentTurn) ||
        !msg.tryRead(value.winner) ||
        !msg.tryRead(value.endReason) ||
        !msg.tryRead(value.clockConfig.enabled) ||
        !tryReadDurationMs(msg, value.clockConfig.initialTime) ||
        !tryReadDurationMs(msg, value.clockConfig.increment) ||
        !tryReadDurationMs(msg, value.whiteTimeRemaining) ||
        !tryReadDurationMs(msg, value.blackTimeRemaining) ||
        !msg.tryRead(value.version)) { return false; }

    return true;
}

}




// LoginRequest
Message LoginRequest::toMessage() const {
    Message msg(MessageType::LoginRequest);
    msg.pushString(username);
    msg.pushString(password);
    return msg;
}

std::shared_ptr<Message> LoginRequest::toSharedMessage() const {
    std::shared_ptr<Message> msg = std::make_shared<Message>(MessageType::LoginRequest);
    msg->pushString(username);
    msg->pushString(password);
    return msg;
}

std::optional<LoginRequest> LoginRequest::fromMessage(Message& msg) {
    if (msg.type() != MessageType::LoginRequest)
        return std::nullopt;

    std::size_t start = msg.getReadOffset();
    LoginRequest data;

    if (!msg.tryReadString(data.username) || !msg.tryReadString(data.password)) {
        msg.setReadOffset(start);
        return std::nullopt;
    }

    return data;
}


// LoginResponse
Message LoginResponse::toMessage() const {
    Message msg(MessageType::LoginResponse);
    msg.push(accepted);
    msg.pushString(reason);
    return msg;
}

std::shared_ptr<Message> LoginResponse::toSharedMessage() const {
    std::shared_ptr<Message> msg = std::make_shared<Message>(MessageType::LoginResponse);
    msg->push(accepted);
    msg->pushString(reason);
    return msg;
}

std::optional<LoginResponse> LoginResponse::fromMessage(Message& msg) {
    if (msg.type() != MessageType::LoginResponse)
        return std::nullopt;

    std::size_t start = msg.getReadOffset();
    LoginResponse data;

    if (!msg.tryRead(data.accepted) || !msg.tryReadString(data.reason)) {
        msg.setReadOffset(start);
        return std::nullopt;
    }

    return data;
}


// Chat
Message Chat::toMessage() const {
    Message msg(MessageType::Chat);
    msg.push(scope);
    msg.pushString(message);
    return msg;
}

std::shared_ptr<Message> Chat::toSharedMessage() const {
    std::shared_ptr<Message> msg = std::make_shared<Message>(MessageType::Chat);
    msg->push(scope);
    msg->pushString(message);
    return msg;
}

std::optional<Chat> Chat::fromMessage(Message& msg) {
    if (msg.type() != MessageType::Chat)
        return std::nullopt;

    std::size_t start = msg.getReadOffset();
    Chat data;

    if (!msg.tryRead(data.scope) || !msg.tryReadString(data.message)) {
        msg.setReadOffset(start);
        return std::nullopt;
    }

    return data;
}


// Command
Message Command::toMessage() const {
    Message msg(MessageType::Command);
    msg.pushString(command);
    return msg;
}

std::shared_ptr<Message> Command::toSharedMessage() const {
    std::shared_ptr<Message> msg = std::make_shared<Message>(MessageType::Command);
    msg->pushString(command);
    return msg;
}

std::optional<Command> Command::fromMessage(Message& msg) {
    if (msg.type() != MessageType::Command)
        return std::nullopt;

    std::size_t start = msg.getReadOffset();
    Command data;

    if (!msg.tryReadString(data.command)) {
        msg.setReadOffset(start);
        return std::nullopt;
    }

    return data;
}


// CreateRoomRequest
Message CreateRoomRequest::toMessage() const {
    Message msg(MessageType::CreateRoomRequest);
    pushRoomCreateConfig(msg, config);
    return msg;
}

std::shared_ptr<Message> CreateRoomRequest::toSharedMessage() const {
    std::shared_ptr<Message> msg = std::make_shared<Message>(MessageType::CreateRoomRequest);
    pushRoomCreateConfig(*msg, config);
    return msg;
}

std::optional<CreateRoomRequest> CreateRoomRequest::fromMessage(Message& msg) {
    if (msg.type() != MessageType::CreateRoomRequest)
        return std::nullopt;

    std::size_t start = msg.getReadOffset();
    CreateRoomRequest data;
    if (!tryReadRoomCreateConfig(msg, data.config)) {
        msg.setReadOffset(start);
        return std::nullopt;
    }
    return data;
}

// CreateRoomResponse

Message CreateRoomResponse::toMessage() const {
    Message msg(MessageType::CreateRoomResponse);
    msg.push(success);
    msg.push(roomID);
    msg.push(memberType);
    msg.push(color);
    msg.pushString(reason);
    return msg;
}

std::shared_ptr<Message> CreateRoomResponse::toSharedMessage() const {
    std::shared_ptr<Message> msg = std::make_shared<Message>(MessageType::CreateRoomResponse);
    msg->push(success);
    msg->push(roomID);
    msg->push(memberType);
    msg->push(color);
    msg->pushString(reason);
    return msg;
}

std::optional<CreateRoomResponse> CreateRoomResponse::fromMessage(Message& msg) {
    if (msg.type() != MessageType::CreateRoomResponse) {
        return std::nullopt;
    }

    std::size_t start = msg.getReadOffset();
    CreateRoomResponse data;

    if (!msg.tryRead(data.success) || !msg.tryRead(data.roomID) ||
        !msg.tryRead(data.memberType) || !msg.tryRead(data.color) ||
        !msg.tryReadString(data.reason)) {
        msg.setReadOffset(start);
        return std::nullopt;
    }

    return data;
}


// ListRoomsRequest
Message ListRoomsRequest::toMessage() const {
    return Message(MessageType::ListRoomsRequest);
}

std::shared_ptr<Message> ListRoomsRequest::toSharedMessage() const {
    return std::make_shared<Message>(MessageType::ListRoomsRequest);
}

std::optional<ListRoomsRequest> ListRoomsRequest::fromMessage(Message& msg) {
    if (msg.type() != MessageType::ListRoomsRequest) {
        return std::nullopt;
    }

    return ListRoomsRequest{};
}


// ListRoomsResponse
Message ListRoomsResponse::toMessage() const {
    Message msg(MessageType::ListRoomsResponse);

    msg.push<std::uint32_t>(static_cast<std::uint32_t>(rooms.size()));
    for (const RoomSummary& room : rooms) {
        pushRoomSummary(msg, room);
    }

    return msg;
}

std::shared_ptr<Message> ListRoomsResponse::toSharedMessage() const {
    std::shared_ptr<Message> msg = std::make_shared<Message>(MessageType::ListRoomsResponse);

    msg->push<std::uint32_t>(static_cast<std::uint32_t>(rooms.size()));
    for (const RoomSummary& room : rooms) {
        pushRoomSummary(*msg, room);
    }

    return msg;
}

std::optional<ListRoomsResponse> ListRoomsResponse::fromMessage(Message& msg) {
    if (msg.type() != MessageType::ListRoomsResponse) {
        return std::nullopt;
    }

    std::size_t start = msg.getReadOffset();
    ListRoomsResponse data;
    std::uint32_t count;

    if (!msg.tryRead(count)) {
        msg.setReadOffset(start);
        return std::nullopt;
    }

    data.rooms.reserve(count);
    for (std::uint32_t i = 0; i < count; ++i) {
        RoomSummary room;
        if (!tryReadRoomSummary(msg, room)) {
            msg.setReadOffset(start);
            return std::nullopt;
        }
        data.rooms.push_back(std::move(room));
    }

    return data;
}



// JoinRoomRequest
Message JoinRoomRequest::toMessage() const {
    Message msg(MessageType::JoinRoomRequest);
    msg.push(roomID);
    msg.push(spectator);
    msg.pushString(password);
    return msg;
}

std::shared_ptr<Message> JoinRoomRequest::toSharedMessage() const {
    std::shared_ptr<Message> msg = std::make_shared<Message>(MessageType::JoinRoomRequest);
    msg->push(roomID);
    msg->push(spectator);
    msg->pushString(password);
    return msg;
}

std::optional<JoinRoomRequest> JoinRoomRequest::fromMessage(Message& msg) {
    if (msg.type() != MessageType::JoinRoomRequest)
        return std::nullopt;

    std::size_t start = msg.getReadOffset();
    JoinRoomRequest data;

    if (!msg.tryRead(data.roomID) || !msg.tryRead(data.spectator) || !msg.tryReadString(data.password)) {
        msg.setReadOffset(start);
        return std::nullopt;
    }

    return data;
}


// JoinRoomResponse
Message JoinRoomResponse::toMessage() const {
    Message msg(MessageType::JoinRoomResponse);
    msg.push(success);
    msg.push(roomID);
    msg.push(memberType);
    msg.push(color);
    msg.pushString(reason);
    return msg;
}

std::shared_ptr<Message> JoinRoomResponse::toSharedMessage() const {
    std::shared_ptr<Message> msg = std::make_shared<Message>(MessageType::JoinRoomResponse);
    msg->push(success);
    msg->push(roomID);
    msg->push(memberType);
    msg->push(color);
    msg->pushString(reason);
    return msg;
}

std::optional<JoinRoomResponse> JoinRoomResponse::fromMessage(Message& msg) {
    if (msg.type() != MessageType::JoinRoomResponse) {
        return std::nullopt;
    }

    std::size_t start = msg.getReadOffset();
    JoinRoomResponse data;

    if (!msg.tryRead(data.success) || !msg.tryRead(data.roomID) ||
        !msg.tryRead(data.memberType) || !msg.tryRead(data.color) ||
        !msg.tryReadString(data.reason)) {
        msg.setReadOffset(start);
        return std::nullopt;
    }

    return data;
}


// LeaveRoom
Message LeaveRoomRequest::toMessage() const {
    return Message(MessageType::LeaveRoomRequest);
}

std::shared_ptr<Message> LeaveRoomRequest::toSharedMessage() const {
    return std::make_shared<Message>(MessageType::LeaveRoomRequest);
}

std::optional<LeaveRoomRequest> LeaveRoomRequest::fromMessage(Message& msg) {
    if (msg.type() != MessageType::LeaveRoomRequest)
        return std::nullopt;

    return LeaveRoomRequest{};
}


// LeaveRoomResponse
Message LeaveRoomResponse::toMessage() const {
    Message msg(MessageType::LeaveRoomResponse);
    msg.push(success);
    msg.push(roomID);
    msg.pushString(reason);
    return msg;
}

std::shared_ptr<Message> LeaveRoomResponse::toSharedMessage() const {
    std::shared_ptr<Message> msg = std::make_shared<Message>(MessageType::LeaveRoomResponse);
    msg->push(success);
    msg->push(roomID);
    msg->pushString(reason);
    return msg;
}

std::optional<LeaveRoomResponse> LeaveRoomResponse::fromMessage(Message& msg) {
    if (msg.type() != MessageType::LeaveRoomResponse) {
        return std::nullopt;
    }

    std::size_t start = msg.getReadOffset();
    LeaveRoomResponse data;

    if (!msg.tryRead(data.success) || !msg.tryRead(data.roomID) || !msg.tryReadString(data.reason)) {
        msg.setReadOffset(start);
        return std::nullopt;
    }

    return data;
}

// MakeMove
Message MakeMove::toMessage() const {
    Message msg(MessageType::MakeMove);
    msg.push(from);
    msg.push(to);
    msg.push(promotion);
    return msg;
}

std::shared_ptr<Message> MakeMove::toSharedMessage() const {
    std::shared_ptr<Message> msg = std::make_shared<Message>(MessageType::MakeMove);
    msg->push(from);
    msg->push(to);
    msg->push(promotion);
    return msg;
}

std::optional<MakeMove> MakeMove::fromMessage(Message& msg) {
    if (msg.type() != MessageType::MakeMove)
        return std::nullopt;

    std::size_t start = msg.getReadOffset();
    MakeMove data;

    if (!msg.tryRead(data.from) || !msg.tryRead(data.to) || !msg.tryRead(data.promotion)) {
        msg.setReadOffset(start);
        return std::nullopt;
    }

    return data;
}


// GameUpdate
Message GameUpdate::toMessage() const {
    Message msg(MessageType::GameUpdate);
    msg.push(roomID);
    pushPublicRoomConfig(msg, config);
    msg.push(roomVersion);
    msg.pushString(whitePlayerName);
    msg.pushString(blackPlayerName);
    msg.push(spectatorCount);
    pushChessGameSnapshot(msg, snapshot);
    return msg;
}

std::shared_ptr<Message> GameUpdate::toSharedMessage() const {
    std::shared_ptr<Message> msg = std::make_shared<Message>(MessageType::GameUpdate);
    msg->push(roomID);
    pushPublicRoomConfig(*msg, config);
    msg->push(roomVersion);
    msg->pushString(whitePlayerName);
    msg->pushString(blackPlayerName);
    msg->push(spectatorCount);
    pushChessGameSnapshot(*msg, snapshot);
    return msg;
}

std::optional<GameUpdate> GameUpdate::fromMessage(Message& msg) {
    if (msg.type() != MessageType::GameUpdate) {
        return std::nullopt;
    }

    std::size_t start = msg.getReadOffset();
    GameUpdate data;

    if (!msg.tryRead(data.roomID) || !tryReadPublicRoomConfig(msg, data.config) ||
        !msg.tryRead(data.roomVersion) || !msg.tryReadString(data.whitePlayerName) ||
        !msg.tryReadString(data.blackPlayerName) || !msg.tryRead(data.spectatorCount) ||
        !tryReadChessGameSnapshot(msg, data.snapshot)) {
        msg.setReadOffset(start);
        return std::nullopt;
    }

    return data;
}


// ErrorMessage
Message ErrorMessage::toMessage() const {
    Message msg(MessageType::ErrorMessage);
    msg.push(errorCode);
    msg.pushString(message);
    return msg;
}

std::shared_ptr<Message> ErrorMessage::toSharedMessage() const {
    std::shared_ptr<Message> msg = std::make_shared<Message>(MessageType::ErrorMessage);
    msg->push(errorCode);
    msg->pushString(message);
    return msg;
}

std::optional<ErrorMessage> ErrorMessage::fromMessage(Message& msg) {
    if (msg.type() != MessageType::ErrorMessage)
        return std::nullopt;

    std::size_t start = msg.getReadOffset();
    ErrorMessage data;

    if (!msg.tryRead(data.errorCode) ||
        !msg.tryReadString(data.message)) {
        msg.setReadOffset(start);
        return std::nullopt;
    }

    return data;
}

}