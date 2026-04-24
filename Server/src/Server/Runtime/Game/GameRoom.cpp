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

GameRoom::GameRoom(RoomID roomID, SessionID creatorSessionID)
: m_roomID(roomID), m_player1(creatorSessionID) {

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
    return {m_player1, m_player2};
}

std::vector<SessionID> GameRoom::spectatorSessionIDs() const {
    return m_spectators;
}

std::vector<SessionID> GameRoom::allSessionsIDs() const {
    std::vector<SessionID> out = m_spectators;
    out.push_back(m_player1);
    out.push_back(m_player2);
    return out;
}

GameRoomView GameRoom::view() const {
    return {
        .roomID = m_roomID,
        .state = m_state,
        .player1 = m_player1,
        .player2 = m_player2,
        .spectators = m_spectators,
        .whiteTurnToMove = m_board.getTurn() // May Need To Change This To Manual Tracking
    };
}

JoinRoomResult GameRoom::joinPlayer(SessionID sessionID) {
    if (contains(sessionID)) { return JoinRoomResult::AlreadyInRoom; }
    if (m_state == GameRoomState::GameOver) { return JoinRoomResult::InvalidState; }

    if (m_player1 == 0) {
        m_player1 = sessionID;
    } else if (m_player2 == 0) {
        m_player2 = sessionID;
    } else {
        return JoinRoomResult::RoomFull;
    }

    startGameIfReady();
    return JoinRoomResult::JoinedAsPlayer;
}

JoinRoomResult GameRoom::joinSpectator(SessionID sessionID) {
    if (contains(sessionID)) { return JoinRoomResult::AlreadyInRoom; }
    if (m_state == GameRoomState::GameOver) { return JoinRoomResult::InvalidState; }

    m_spectators.push_back(sessionID);
    return JoinRoomResult::JoinedAsSpectator;
}


LeaveRoomResult GameRoom::leave(SessionID sessionID) {
    switch (roleOf(sessionID)) {
        case GameRoomRole::Player1:
            m_player1 = 0;
            if (m_state == GameRoomState::InProgress) {
                m_state = GameRoomState::GameOver;
            }
            return LeaveRoomResult::Left;
        case GameRoomRole::Player2:
            m_player2 = 0;
            if (m_state == GameRoomState::InProgress) {
                m_state = GameRoomState::GameOver;
            }
            return LeaveRoomResult::Left;
        case GameRoomRole::Spectator:
            m_spectators.erase(std::remove(m_spectators.begin(), m_spectators.end(), sessionID),m_spectators.end());
            return LeaveRoomResult::Left;
        case GameRoomRole::None:
            return LeaveRoomResult::NotInRoom;
    }
    return LeaveRoomResult::NotInRoom;
}

MoveResult GameRoom::submitMove(SessionID sessionID, std::uint8_t from, std::uint8_t to) {
    if (!contains(sessionID)) { return MoveResult::NotInRoom; }
    if (roleOf(sessionID) == GameRoomRole::Spectator) { return MoveResult::NotAPlayer; }
    if (m_state != GameRoomState::InProgress) { return MoveResult::GameNotActive; }
    if (from >= 64 || to >= 64) { return MoveResult::InvalidMove; }
    if (!isPlayersTurn(sessionID)) { return MoveResult::NotYourTurn; }

    ID piece = pieceAtSquare(from);
    if (!isPlayersPiece(sessionID, piece)) { return MoveResult::InvalidMove; }

    m_board.genMoves();
    if (!m_board.isValidMove(piece, posFromSquare(to))) {return MoveResult::InvalidMove;}

    m_board.move(piece, posFromSquare(to));
    m_board.nextTurn();
    return MoveResult::Success;
}


bool GameRoom::canStartGame() const noexcept {
    return m_state == GameRoomState::WaitingForPlayers && m_player1 != 0 && m_player2 != 0;
}

void GameRoom::startGameIfReady() {
    if (canStartGame()) {
        m_state = GameRoomState::InProgress;
    }
}

bool GameRoom::isPlayersTurn
}
