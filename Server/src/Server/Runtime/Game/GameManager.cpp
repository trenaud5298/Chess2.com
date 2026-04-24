/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Server/Runtime/Game/GameManager.hpp>
#include <Chess/Server/Runtime/GameServer.hpp>
#include <Chess/Server/Runtime/Session/SessionManager.hpp>
#include <Chess/Server/Runtime/Session/Target.hpp>
#include <Chess/Core/Common/TimeFormat.hpp>
#include <Chess/Core/Networking/MessagePayloads.hpp>

// ASIO Includes
#include <asio/bind_executor.hpp>
#include <asio/dispatch.hpp>
#include <asio/post.hpp>

// C++ Includes
#include <format>
#include <ranges>
#include <utility>

#include "Chess/Server/Runtime/Session/Session.hpp"


namespace Chess {


namespace {

[[nodiscard]] std::string sideLabel(COLOR side) {
    switch (side) {
        case COLOR::WHITE: return "White";
        case COLOR::BLACK: return "Black";
        case COLOR::EMPTY: return "None";
    }
    return "None";
}

[[nodiscard]] std::string scopeLabel(ChatScope scope) {
    return scope == ChatScope::Global ? "Global" : "Game";
}

[[nodiscard]] std::string chessGameMoveStatusLabel(ChessGameMoveStatus status) {
    switch (status) {
        case ChessGameMoveStatus::Success: return "Success";
        case ChessGameMoveStatus::GameNotStarted: return "Game Not Started";
        case ChessGameMoveStatus::GameAlreadyFinished: return "Game Already Finished";
        case ChessGameMoveStatus::InvalidSide: return "Invalid Side";
        case ChessGameMoveStatus::WrongTurn: return "Wrong Turn";
        case ChessGameMoveStatus::SquareOutOfBounds: return "Square Out Of Bounds";
        case ChessGameMoveStatus::EmptySquare: return "Empty Square";
        case ChessGameMoveStatus::WrongPieceColor: return "Wrong Piece Color";
        case ChessGameMoveStatus::InvalidMove: return "Invalid Move";
    }
    return "";
}

}



GameManager::GameManager(GameServer& gameServer)
: m_gameServer(gameServer), m_strand(asio::make_strand(m_gameServer.ioContext())), m_tickTimer(m_gameServer.ioContext()) {

}

GameManager::~GameManager() {

}

void GameManager::start() {
    LifecycleState expectedState = LifecycleState::STOPPED;
    if (!m_state.compare_exchange_strong(expectedState, LifecycleState::STARTING)) {
        return;
    }

    m_roomIDToRoom.clear();
    m_sessionIDToRoomID.clear();

    m_gameServer.loggingManager().log(LogEntry::Info("Game Manager Started"));
    m_state = LifecycleState::RUNNING;
    scheduleTick();
}

void GameManager::stop() {
    LifecycleState expectedState = LifecycleState::RUNNING;
    if (!m_state.compare_exchange_strong(expectedState, LifecycleState::STOPPING)) {
        return;
    }

    asio::dispatch(m_strand, [this]() {
        m_tickTimer.cancel();

        m_roomIDToRoom.clear();
        m_sessionIDToRoomID.clear();

        m_state = LifecycleState::STOPPED;
        m_gameServer.loggingManager().log(LogEntry::Info("Game Manager Stopped"));
    });
}

void GameManager::requestCreateRoom(SessionID sessionID) {
    asio::post(m_strand, std::bind_front(&GameManager::doRequestCreateRoom, this, sessionID));
}

void GameManager::requestJoinRoom(SessionID sessionID, RoomID roomID, bool spectator) {
    asio::post(m_strand, std::bind_front(&GameManager::doRequestJoinRoom, this, sessionID, roomID, spectator));
}

void GameManager::requestLeaveRoom(SessionID sessionID) {
    asio::post(m_strand, std::bind_front(&GameManager::doRequestLeaveRoom, this, sessionID));
}

void GameManager::requestMove(SessionID sessionID, std::uint8_t from, std::uint8_t to) {
    asio::post(m_strand, std::bind_front(&GameManager::doRequestMove, this, sessionID, from, to));
}

void GameManager::requestGameChat(SessionID sessionID, std::string message) {
    asio::post(m_strand, std::bind_front(&GameManager::doRequestGameChat, this, sessionID, message));
}

void GameManager::doRequestCreateRoom(SessionID sessionID) {
    if (m_state != LifecycleState::RUNNING) {
        return;
    }

    if (sessionAlreadyInRoom(sessionID)) {
        sendCreateRoomResponse(sessionID, false, 0, "Session is already in a room");
        return;
    }

    RoomID roomID = m_nextRoomID++;
    std::shared_ptr<GameRoom> room = std::make_shared<GameRoom>(roomID, sessionID);

    m_roomIDToRoom.emplace(roomID, room);
    bindSessionToRoom(sessionID, roomID);

    m_gameServer.loggingManager().log(LogEntry::Info(
        "Game Manager Created Room " + std::to_string(roomID) + " for Session " + std::to_string(sessionID)
    ));

    sendCreateRoomResponse(sessionID, true, roomID, "");
}

void GameManager::doRequestJoinRoom(SessionID sessionID, RoomID roomID, bool spectator) {
    if (m_state != LifecycleState::RUNNING) {
        return;
    }

    if (sessionAlreadyInRoom(sessionID)) {
        sendJoinRoomResponse(sessionID, false, roomID, "Session is already in a room");
        return;
    }

    std::shared_ptr<GameRoom> room = roomByID(roomID);
    if (!room) {
        sendJoinRoomResponse(sessionID, false, roomID, "Room does not exist");
        return;
    }

    JoinRoomResult joinResult = spectator ? room->joinSpectator(sessionID) : room->joinPlayer(sessionID, std::chrono::steady_clock::now());

    switch (joinResult) {
        case JoinRoomResult::JoinedAsPlayer:
        case JoinRoomResult::JoinedAsSpectator:
            bindSessionToRoom(sessionID, roomID);
            sendJoinRoomResponse(sessionID, true, roomID, "");
            sendGameUpdate(*room);
            break;

        case JoinRoomResult::AlreadyInRoom:
            sendJoinRoomResponse(sessionID, false, roomID, "Room is already in a room");
            break;

        case JoinRoomResult::RoomFull:
            sendJoinRoomResponse(sessionID, false, roomID, "Room is already full");
            break;

        case JoinRoomResult::InvalidState:
            sendJoinRoomResponse(sessionID, false, roomID, "Room is not joinable");
            break;
    }
}

void GameManager::doRequestLeaveRoom(SessionID sessionID) {
    if (m_state != LifecycleState::RUNNING) {
        return;
    }

    std::shared_ptr<GameRoom> room = roomBySession(sessionID);
    if (!room) {
        sendRoomError(sessionID, "Sessions is not in a room");
        return;
    }

    RoomID roomID = room->roomID();
    LeaveRoomResult leaveResult = room->leave(sessionID, std::chrono::steady_clock::now());

    if (leaveResult == LeaveRoomResult::NotInRoom) {
        sendRoomError(sessionID, "Session is not in this room");
        return;
    }

    unbindSessionFromRoom(sessionID);

    if (!room->empty()) {
        sendGameUpdate(*room);
    }

    removeRoomIfEmpty(roomID);
}

void GameManager::doRequestMove(SessionID sessionID, std::uint8_t from, std::uint8_t to) {
    if (m_state != LifecycleState::RUNNING) {
        return;
    }

    std::shared_ptr<GameRoom> room = roomBySession(sessionID);
    if (!room) {
        sendRoomError(sessionID, "Session is not in a room");
        return;
    }

    GameRoomMoveResult result = room->submitMove(sessionID, from, to, std::chrono::steady_clock::now());
    switch (result.status) {
        case GameRoomMoveStatus::Success:
            sendGameUpdate(*room);
            break;
        case GameRoomMoveStatus::NotInRoom:
            sendRoomError(sessionID, "Session is not in this room");
            break;
        case GameRoomMoveStatus::SpectatorCannotMove:
            sendRoomError(sessionID, "Spectators Cannot Make Moves");
            break;
        case GameRoomMoveStatus::GameRejected:
            sendRoomError(sessionID, "Move Rejected: " + chessGameMoveStatusLabel(result.gameResult.status));
            break;
    }
}

void GameManager::doRequestGameChat(SessionID sessionID, std::string message) {
    if (m_state != LifecycleState::RUNNING) {
        return;
    }

    if (message.empty()) {
        return;
    }

    std::shared_ptr<GameRoom> room = roomBySession(sessionID);
    if (!room) {
        sendRoomError(sessionID, "Session is not in a room");
        return;
    }

    if (room->roleOf(sessionID) == GameRoomRole::Spectator) {
        sendRoomError(sessionID, "Spectators cannot send game chat");
        return;
    }

    std::string senderName;
    {
        std::vector<SessionView> views = m_gameServer.sessionManager().viewSnapshot(Target::Id({sessionID}));
        if (!views.empty()) {
            senderName = views.front().name;
        }
    }

    if (senderName.empty()) {
        senderName = "Unknown";
    }

    Chat chat{
        .scope = ChatScope::Game,
        .message = std::format("[{}][{}] {}", presentLocalTime(), senderName, message)
    };

    sendToRoomAll(*room, chat.toSharedMessage());

    m_gameServer.loggingManager().log(LogEntry::Message(
        "GameManager room " + std::to_string(room->roomID()) + " chat from session " +
        std::to_string(sessionID) + " [" + scopeLabel(chat.scope) + "] " + chat.message
    ));
}

void GameManager::scheduleTick() {
    if (m_state != LifecycleState::RUNNING) {
        return;
    }

    m_tickTimer.expires_after(TICK_INTERVAL);
    m_tickTimer.async_wait(asio::bind_executor(m_strand, [this](const std::error_code& ec) {
        if (ec) {
            return;
        }
        onTick();
    }));
}

void GameManager::onTick() {
    if (m_state != LifecycleState::RUNNING) {
        return;
    }

    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

    for (auto& room: m_roomIDToRoom | std::views::values) {
        std::uint64_t beforeVersion = room->game().version();
        GameRoomState beforeState = room->state();

        room->tick(now);

        if (room->game().version() != beforeVersion || room->state() != beforeState) {
            sendGameUpdate(*room);
        }
    }

    scheduleTick();
}

void GameManager::sendToSession(SessionID sessionID, std::shared_ptr<const Message> message) {
    sendToSessions({sessionID}, std::move(message));
}

void GameManager::sendToSessions(const std::vector<SessionID>& sessionIDs, std::shared_ptr<const Message> message) {
    if (!message || sessionIDs.empty()) {
        return;
    }

    m_gameServer.sessionManager().messageSession(Target::Id(sessionIDs), message);
}

void GameManager::sendToRoomPlayers(const GameRoom& room, std::shared_ptr<const Message> message) {
    sendToSessions(room.playerSessionIDs(), std::move(message));
}

void GameManager::sendToRoomAll(const GameRoom &room, std::shared_ptr<const Message> message) {
    sendToSessions(room.allSessionsIDs(), std::move(message));
}

void GameManager::sendCreateRoomResponse(SessionID sessionID, bool success, RoomID roomID, std::string reason) {
    JoinRoomResponse response{
        .success = success,
        .roomID = roomID,
        .reason = std::move(reason)
    };

    sendToSession(sessionID, response.toSharedMessage());
}

void GameManager::sendJoinRoomResponse(SessionID sessionID, bool success, RoomID roomID, std::string reason) {
    JoinRoomResponse response{
        .success = success,
        .roomID = roomID,
        .reason = std::move(reason)
    };

    sendToSession(sessionID, response.toSharedMessage());
}

void GameManager::sendRoomError(SessionID sessionID, std::string reason) {
    ErrorMessage response{
        .errorCode = 1000,
        .message = std::move(reason)
    };

    sendToSession(sessionID, response.toSharedMessage());
}

void GameManager::sendGameUpdate(const GameRoom &room) {
    //TODO: Implement with say
    // GameUpdate update{
    //     .roomID = room.roomId(),
    //     .snapshot = room.snapshow(now)
    // }
}

std::shared_ptr<GameRoom> GameManager::roomByID(RoomID roomID) {
    auto it = m_roomIDToRoom.find(roomID);
    if (it == m_roomIDToRoom.end()) {
        return nullptr;
    }
    return it->second;
}

std::shared_ptr<GameRoom> GameManager::roomBySession(SessionID sessionID) {
    auto it = m_sessionIDToRoomID.find(sessionID);
    if (it == m_sessionIDToRoomID.end()) {
        return nullptr;
    }
    return roomByID(it->second);
}

bool GameManager::sessionAlreadyInRoom(SessionID sessionID) const noexcept {
    return m_sessionIDToRoomID.contains(sessionID);
}

void GameManager::bindSessionToRoom(SessionID sessionID, RoomID roomID) {
    m_sessionIDToRoomID[sessionID] = roomID;
}

void GameManager::unbindSessionFromRoom(SessionID sessionID) {
    m_sessionIDToRoomID.erase(sessionID);
}

void GameManager::removeRoomIfEmpty(RoomID roomID) {
    auto it = m_roomIDToRoom.find(roomID);
    if (it == m_roomIDToRoom.end()) {
        return;
    }

    if (!it->second->empty()) {
        return;
    }

    m_roomIDToRoom.erase(it);

    m_gameServer.loggingManager().log(LogEntry::Info(
        "Game Manager Removed Empty Room " + std::to_string(roomID)
    ));
}
}


