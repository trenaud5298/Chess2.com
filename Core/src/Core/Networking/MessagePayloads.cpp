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
    msg.push(roomID);
    msg.pushString(password);
    return msg;
}

std::shared_ptr<Message> CreateRoomRequest::toSharedMessage() const {
    std::shared_ptr<Message> msg = std::make_shared<Message>(MessageType::CreateRoomRequest);
    msg->push(roomID);
    msg->pushString(password);
    return msg;
}

std::optional<CreateRoomRequest> CreateRoomRequest::fromMessage(Message& msg) {
    if (msg.type() != MessageType::CreateRoomRequest)
        return std::nullopt;

    std::size_t start = msg.getReadOffset();
    CreateRoomRequest data;

    if (!msg.tryRead(data.roomID) || !msg.tryReadString(data.password)) {
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
    msg.pushString(reason);
    return msg;
}

std::shared_ptr<Message> CreateRoomResponse::toSharedMessage() const {
    std::shared_ptr<Message> msg = std::make_shared<Message>(MessageType::CreateRoomResponse);
    msg->push(success);
    msg->push(roomID);
    msg->pushString(reason);
    return msg;
}

std::optional<CreateRoomResponse> CreateRoomResponse::fromMessage(Message& msg) {
    if (msg.type() != MessageType::CreateRoomResponse)
        return std::nullopt;

    std::size_t start = msg.getReadOffset();
    CreateRoomResponse data;

    if (!msg.tryRead(data.success) || !msg.tryRead(data.roomID) || !msg.tryReadString(data.reason)) {
        msg.setReadOffset(start);
        return std::nullopt;
    }

    return data;
}


// JoinRoomRequest
Message JoinRoomRequest::toMessage() const {
    Message msg(MessageType::JoinRoomRequest);
    msg.push(roomID);
    msg.pushString(password);
    return msg;
}

std::shared_ptr<Message> JoinRoomRequest::toSharedMessage() const {
    std::shared_ptr<Message> msg = std::make_shared<Message>(MessageType::JoinRoomRequest);
    msg->push(roomID);
    msg->pushString(password);
    return msg;
}

std::optional<JoinRoomRequest> JoinRoomRequest::fromMessage(Message& msg) {
    if (msg.type() != MessageType::JoinRoomRequest)
        return std::nullopt;

    std::size_t start = msg.getReadOffset();
    JoinRoomRequest data;

    if (!msg.tryRead(data.roomID) || !msg.tryReadString(data.password)) {
        msg.setReadOffset(start);
        return std::nullopt;
    }

    return data;
}


// JoinRoomResponse
Message JoinRoomResponse::toMessage() const {
    Message msg(MessageType::JoinRoomResponse);
    msg.push(success);
    msg.pushString(reason);
    return msg;
}

std::shared_ptr<Message> JoinRoomResponse::toSharedMessage() const {
    std::shared_ptr<Message> msg = std::make_shared<Message>(MessageType::JoinRoomResponse);
    msg->push(success);
    msg->pushString(reason);
    return msg;
}

std::optional<JoinRoomResponse> JoinRoomResponse::fromMessage(Message& msg) {
    if (msg.type() != MessageType::JoinRoomResponse)
        return std::nullopt;

    std::size_t start = msg.getReadOffset();
    JoinRoomResponse data;

    if (!msg.tryRead(data.success) || !msg.tryReadString(data.reason)) {
        msg.setReadOffset(start);
        return std::nullopt;
    }

    return data;
}


// LeaveRoom
Message LeaveRoom::toMessage() const {
    return Message(MessageType::LeaveRoom);
}

std::shared_ptr<Message> LeaveRoom::toSharedMessage() const {
    return std::make_shared<Message>(MessageType::LeaveRoom);
}

std::optional<LeaveRoom> LeaveRoom::fromMessage(Message& msg) {
    if (msg.type() != MessageType::LeaveRoom)
        return std::nullopt;

    return LeaveRoom{};
}


// MakeMove
Message MakeMove::toMessage() const {
    Message msg(MessageType::MakeMove);
    msg.push(from);
    msg.push(to);
    return msg;
}

std::shared_ptr<Message> MakeMove::toSharedMessage() const {
    std::shared_ptr<Message> msg = std::make_shared<Message>(MessageType::MakeMove);
    msg->push(from);
    msg->push(to);
    return msg;
}

std::optional<MakeMove> MakeMove::fromMessage(Message& msg) {
    if (msg.type() != MessageType::MakeMove)
        return std::nullopt;

    std::size_t start = msg.getReadOffset();
    MakeMove data;

    if (!msg.tryRead(data.from) || !msg.tryRead(data.to)) {
        msg.setReadOffset(start);
        return std::nullopt;
    }

    return data;
}


// GameUpdate
Message GameUpdate::toMessage() const {
    Message msg(MessageType::GameUpdate);
    // Define Later
    return msg;
}

std::shared_ptr<Message> GameUpdate::toSharedMessage() const {
    std::shared_ptr<Message> msg = std::make_shared<Message>(MessageType::GameUpdate);
    // Define Later
    return msg;
}

std::optional<GameUpdate> GameUpdate::fromMessage(Message& msg) {
    if (msg.type() != MessageType::GameUpdate)
        return std::nullopt;

    return GameUpdate{};
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