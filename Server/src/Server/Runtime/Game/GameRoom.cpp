/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Server/Runtime/Game/GameRoom.hpp>

// ASIO Includes

// C++ Includes
#include <algorithm>

namespace Chess {

GameRoom::GameRoom(RoomID roomID, SessionID creatorSessionID, ChessClockConfig chessClockConfig)
: m_roomID(roomID), m_player1(creatorSessionID), m_game(std::move(chessClockConfig)) {
}

GameRoom::~GameRoom() {

}

RoomID GameRoom::roomID() const noexcept {
    return m_roomID;
}

GameRoomState GameRoom::state() const noexcept {
    return m_state;
}

GameRoomRole GameRoom::roleOf(SessionID sessionID) const noexcept {
    if (sessionID == m_player1) { return GameRoomRole::Player1; }
    if (sessionID == m_player2) { return GameRoomRole::Player2; }
    for (SessionID spectator : m_spectators) {
        if (spectator == sessionID) {return GameRoomRole::Spectator; }
    }
    return GameRoomRole::None;
}

PlayerSide GameRoom::sideOf(SessionID sessionID) const noexcept {
    if (sessionID == m_player1) { return PlayerSide::White; }
    if (sessionID == m_player2) { return PlayerSide::Black; }
    return PlayerSide::None;
}

COLOR GameRoom::colorOf(SessionID sessionID) const noexcept {
    switch (sideOf(sessionID)) {
        case PlayerSide::None: return COLOR::EMPTY;
        case PlayerSide::White: return COLOR::WHITE;
        case PlayerSide::Black: return COLOR::BLACK;
    }
    return COLOR::EMPTY;
}

bool GameRoom::contains(SessionID sessionID) const noexcept {
    return roleOf(sessionID) != GameRoomRole::None;
}

bool GameRoom::empty() const noexcept {
    return m_player1 == 0 && m_player2 == 0 && m_spectators.empty();
}

SessionID GameRoom::player1() const noexcept {
    return m_player1;
}

SessionID GameRoom::player2() const noexcept {
    return m_player2;
}

std::vector<SessionID> GameRoom::playerSessionIDs() const {
    std::vector<SessionID> out;
    if (m_player1 != 0) { out.push_back(m_player1); }
    if (m_player2 != 0) { out.push_back(m_player2); }
    return out;
}

std::vector<SessionID> GameRoom::spectatorSessionIDs() const {
    return m_spectators;
}

std::vector<SessionID> GameRoom::allSessionsIDs() const {
    std::vector<SessionID> out = m_spectators;
    if (m_player1 != 0) { out.push_back(m_player1); }
    if (m_player2 != 0) { out.push_back(m_player2); }
    return out;
}

GameRoomView GameRoom::view() const {
    return {
        .roomID = m_roomID,
        .state = m_state,
        .player1 = m_player1,
        .player2 = m_player2,
        .spectators = m_spectators,
        .currentTurn = m_game.currentTurn(),
        .winner = m_game.winner(),
        .gameState = m_game.state(),
        .gameVersion = m_game.version()
    };
}

ChessGame& GameRoom::game() noexcept {
    return m_game;
}

const ChessGame& GameRoom::game() const noexcept {
    return m_game;
}

JoinRoomResult GameRoom::joinPlayer(SessionID sessionID, std::chrono::steady_clock::time_point now) {
    if (contains(sessionID)) { return JoinRoomResult::AlreadyInRoom; }
    if (m_state == GameRoomState::GameOver) { return JoinRoomResult::InvalidState; }

    if (m_player1 == 0) {
        m_player1 = sessionID;
    } else if (m_player2 == 0) {
        m_player2 = sessionID;
    } else {
        return JoinRoomResult::RoomFull;
    }

    startGameIfReady(now);
    updateRoomStateFromGame();
    return JoinRoomResult::JoinedAsPlayer;
}

JoinRoomResult GameRoom::joinSpectator(SessionID sessionID) {
    if (contains(sessionID)) { return JoinRoomResult::AlreadyInRoom; }
    if (m_state == GameRoomState::GameOver) { return JoinRoomResult::InvalidState; }

    m_spectators.push_back(sessionID);
    return JoinRoomResult::JoinedAsSpectator;
}


LeaveRoomResult GameRoom::leave(SessionID sessionID, std::chrono::steady_clock::time_point now) {
    switch (roleOf(sessionID)) {
        case GameRoomRole::Player1:
            if (m_game.isInProgress()) {
                (void)m_game.resign(COLOR::WHITE, now);
            }
            m_player1 = 0;
            updateRoomStateFromGame();
            return LeaveRoomResult::Left;

        case GameRoomRole::Player2:
            if (m_game.isInProgress()) {
                (void)m_game.resign(COLOR::BLACK, now);
            }
            m_player2 = 0;
            updateRoomStateFromGame();
            return LeaveRoomResult::Left;
        case GameRoomRole::Spectator:
            m_spectators.erase(std::remove(m_spectators.begin(), m_spectators.end(), sessionID),m_spectators.end());
            return LeaveRoomResult::Left;
        case GameRoomRole::None:
            return LeaveRoomResult::NotInRoom;
    }
    return LeaveRoomResult::NotInRoom;
}

GameRoomMoveResult GameRoom::submitMove(SessionID sessionID, std::uint8_t from, std::uint8_t to, std::chrono::steady_clock::time_point now) {
    GameRoomRole role = roleOf(sessionID);

    if (role == GameRoomRole::None) {
        return {
            .status = GameRoomMoveStatus::NotInRoom,
            .gameResult = {}
        };
    }

    if (role == GameRoomRole::Spectator) {
        return {
            .status = GameRoomMoveStatus::SpectatorCannotMove,
            .gameResult = {}
        };
    }

    ChessGameMoveResult gameResult = m_game.submitMove(colorOf(sessionID), from, to, now);
    updateRoomStateFromGame();

    return {
        .status = static_cast<bool>(gameResult) ? GameRoomMoveStatus::Success : GameRoomMoveStatus::GameRejected,
        .gameResult = gameResult,
    };
}


bool GameRoom::resign(SessionID sessionID, std::chrono::steady_clock::time_point now) {
    GameRoomRole role = roleOf(sessionID);
    if (role != GameRoomRole::Player1 && role != GameRoomRole::Player2) {
        return false;
    }

    bool resigned = m_game.resign(colorOf(sessionID), now);
    updateRoomStateFromGame();
    return resigned;
}

void GameRoom::tick(std::chrono::steady_clock::time_point now) {
    m_game.tick(now);
    updateRoomStateFromGame();
}

bool GameRoom::canStartGame() const noexcept {
    return m_state == GameRoomState::WaitingForPlayers && m_player1 != 0 && m_player2 != 0 && !m_game.isStarted();
}

void GameRoom::startGameIfReady(std::chrono::steady_clock::time_point now) {
    if (canStartGame()) {
        m_game.start(now);
        updateRoomStateFromGame();
    }
}

void GameRoom::updateRoomStateFromGame() noexcept {
    if (m_game.isFinished()) {
        m_state = GameRoomState::GameOver;
        return;
    }

    if (m_game.isInProgress()) {
        m_state = GameRoomState::InProgress;
        return;
    }

    m_state = GameRoomState::WaitingForPlayers;
}
}
