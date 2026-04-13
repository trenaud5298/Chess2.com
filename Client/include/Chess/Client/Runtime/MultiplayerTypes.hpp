#ifndef CHESS_CLIENT_RUNTIME_MULTIPLAYERTYPES_HPP
#define CHESS_CLIENT_RUNTIME_MULTIPLAYERTYPES_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/Common/ServerInfo.hpp>

// C++ Includes
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace Chess {

using MessageThreadID = std::uint32_t;
inline constexpr MessageThreadID NO_MESSAGE_THREAD_ID = 0;

enum class MultiplayerState : std::uint8_t {
    Idle = 0,
    ConnectingTransport,
    AwaitingLogin,
    Connected,
    Disconnecting
};

constexpr std::string_view toString(MultiplayerState state) noexcept {
    switch (state) {
        case MultiplayerState::Idle: return "Idle";
        case MultiplayerState::ConnectingTransport: return "ConnectingTransport";
        case MultiplayerState::AwaitingLogin: return "AwaitingLogin";
        case MultiplayerState::Connected: return "Connected";
        case MultiplayerState::Disconnecting: return "Disconnecting";
    }
    return "";
}

enum class MultiplayerErrorCode : std::uint8_t {
    None = 0,
    InvalidState,
    InvalidArgument,

    ResolveFailed,
    ConnectFailed,
    SendFailed,
    ReadFailed,
    Disconnected,

    LoginRejected,
    ProtocolError,
    InternalError
};

constexpr std::string_view toString(MultiplayerErrorCode code) noexcept {
    switch (code) {
        case MultiplayerErrorCode::None: return "None";
        case MultiplayerErrorCode::InvalidState: return "InvalidState";
        case MultiplayerErrorCode::InvalidArgument: return "InvalidArgument";
        case MultiplayerErrorCode::ResolveFailed: return "ResolveFailed";
        case MultiplayerErrorCode::ConnectFailed: return "ConnectFailed";
        case MultiplayerErrorCode::SendFailed: return "SendFailed";
        case MultiplayerErrorCode::ReadFailed: return "ReadFailed";
        case MultiplayerErrorCode::Disconnected: return "Disconnected";
        case MultiplayerErrorCode::LoginRejected: return "LoginRejected";
        case MultiplayerErrorCode::ProtocolError: return "ProtocolError";
        case MultiplayerErrorCode::InternalError: return "InternalError";
    }
    return "";
}

struct [[nodiscard]] MultiplayerStatus {
    bool ok{false};
    MultiplayerErrorCode code{MultiplayerErrorCode::None};
    std::string message;

    operator bool() const noexcept { return ok; }

    static MultiplayerStatus Success() {
        return {true, MultiplayerErrorCode::None, ""};
    }

    static MultiplayerStatus Failure(MultiplayerErrorCode code, std::string message) {
        return {false, code, std::move(message)};
    }
};

enum class MultiplayerEventType : std::uint8_t {
    ConnectStarted = 0,
    TransportConnected,
    ConnectFailed,
    LoginSent,
    LoginAccepted,
    LoginRejected,
    Disconnected
};

constexpr std::string_view toString(MultiplayerEventType type) noexcept {
    switch (type) {
        case MultiplayerEventType::ConnectStarted: return "ConnectStarted";
        case MultiplayerEventType::TransportConnected: return "TransportConnected";
        case MultiplayerEventType::ConnectFailed: return "ConnectFailed";
        case MultiplayerEventType::LoginSent: return "LoginSent";
        case MultiplayerEventType::LoginAccepted: return "LoginAccepted";
        case MultiplayerEventType::LoginRejected: return "LoginRejected";
        case MultiplayerEventType::Disconnected: return "Disconnected";
    }
    return "";
}

struct MultiplayerEvent {
    MultiplayerEventType type{MultiplayerEventType::Disconnected};
    MultiplayerState state{MultiplayerState::Idle};
    MultiplayerErrorCode errorCode{MultiplayerErrorCode::None};
    MessageThreadID messageThreadID{NO_MESSAGE_THREAD_ID};
    std::string message;
};

struct MultiplayerView {
    MultiplayerState state{MultiplayerState::Idle};
    std::optional<ServerInfo> serverInfo;
    MessageThreadID activeMessageThreadID{NO_MESSAGE_THREAD_ID};
    bool socketConnected{false};
    bool loginAccepted{false};
};

} // namespace Chess

#endif
