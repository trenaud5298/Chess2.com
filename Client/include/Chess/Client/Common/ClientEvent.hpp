#ifndef CHESS_CLIENT_COMMON_CLIENTEVENT_HPP
#define CHESS_CLIENT_COMMON_CLIENTEVENT_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/Common/ClientStatus.hpp>

// C++ Includes
#include <cstdint>
#include <utility>

namespace Chess {

enum class EventSource : std::uint8_t {
    None = 0,
    GameClient = 1,
    Singleplayer = 2,
    Multiplayer = 3,
    Persistence = 4,
    UI = 5
};

enum class EventKind : std::uint8_t {
    Debug = 0,
    Info = 1,
    Result = 2
};

enum class EventType : std::uint8_t {
    None = 0,

    // Singleplayer Event Types
    SingleplayerStart = 10,
    SingleplayerMove = 11,
    SingleplayerGameEnd = 12,

    // Multiplayer Event Types
    MultiplayerConnect = 20,
    MultiplayerTransport = 21,
    MultiplayerLogin = 22,
    MultiplayerDisconnect = 23,
    MultiplayerRoomsRefresh = 24,
    MultiplayerRoomCreate = 25,
    MultiplayerRoomJoin = 26,
    MultiplayerRoomLeave = 27,
    MultiplayerMove = 28,
    MultiplayerGameSync = 29,
    MultiplayerServerError = 30,


    // Persistence Event Types
    SettingsSave = 40
};

constexpr std::string_view toString(EventSource source) noexcept {
    switch (source) {
        case EventSource::None: return "None";
        case EventSource::GameClient: return "GameClient";
        case EventSource::Singleplayer: return "Singleplayer";
        case EventSource::Multiplayer: return "Multiplayer";
        case EventSource::Persistence: return "Persistence";
        case EventSource::UI: return "UI";
    }
    return "Unknown";
}

constexpr std::string_view toString(EventKind kind) noexcept {
    switch (kind) {
        case EventKind::Debug: return "Debug";
        case EventKind::Info: return "Info";
        case EventKind::Result: return "Result";
    }
    return "Unknown";
}

constexpr std::string_view toString(EventType type) noexcept {
    switch (type) {
        case EventType::None: return "None";
        case EventType::SingleplayerStart: return "SingleplayerStart";
        case EventType::SingleplayerMove: return "SingleplayerMove";
        case EventType::SingleplayerGameEnd: return "SingleplayerGameEnd";
        case EventType::MultiplayerConnect: return "MultiplayerConnect";
        case EventType::MultiplayerTransport: return "MultiplayerTransport";
        case EventType::MultiplayerLogin: return "MultiplayerLogin";
        case EventType::MultiplayerDisconnect: return "MultiplayerDisconnect";
        case EventType::MultiplayerRoomsRefresh: return "MultiplayerRoomsRefresh";
        case EventType::MultiplayerRoomCreate: return "MultiplayerRoomCreate";
        case EventType::MultiplayerRoomJoin: return "MultiplayerRoomJoin";
        case EventType::MultiplayerRoomLeave: return "MultiplayerRoomLeave";
        case EventType::MultiplayerMove: return "MultiplayerMove";
        case EventType::MultiplayerGameSync: return "MultiplayerGameSync";
        case EventType::MultiplayerServerError: return "MultiplayerServerError";
        case EventType::SettingsSave: return "SettingsSave";
    }
    return "Unknown";
}


struct ClientEvent {
    EventSource source{EventSource::None};
    EventKind kind{EventKind::Info};
    EventType type{EventType::None};
    ClientStatus status{ClientStatus::Info("")};

    [[nodiscard]] bool isInfo() const noexcept {
        return kind == EventKind::Info;
    }

    [[nodiscard]] bool isResult() const noexcept {
        return kind == EventKind::Result;
    }

    [[nodiscard]] bool isFailure() const noexcept {
        return status.isFailure();
    }

    [[nodiscard]] bool isSuccess() const noexcept {
        return status.isSuccess();
    }

    [[nodiscard]] const std::string& message() const noexcept {
        return status.message;
    }

    static ClientEvent Debug(EventSource source, EventType type, std::string message) {
        return {
            .source = source,
            .kind = EventKind::Debug,
            .type = type,
            .status = ClientStatus::Debug(std::move(message))
        };
    }

    static ClientEvent Info(EventSource source, EventType type, std::string message) {
        return {
            .source = source,
            .kind = EventKind::Info,
            .type = type,
            .status = ClientStatus::Info(std::move(message))
        };
    }

    static ClientEvent Result(EventSource source, EventType type, ClientStatus status) {
        return {
            .source = source,
            .kind = EventKind::Result,
            .type = type,
            .status = std::move(status)
        };
    }
};


}


#endif