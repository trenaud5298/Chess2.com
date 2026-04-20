#ifndef CHESS_SERVER_RUNTIME_GAME_GAMEROOM_HPP
#define CHESS_SERVER_RUNTIME_GAME_GAMEROOM_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Core/Game/Board.hpp>
#include <Chess/Core/Common/Types.hpp>

// ASIO Includes

// C++ Includes
#include <cstdint>
#include <optional>
#include <vector>

namespace Chess {

enum class GameRoomState : std::uint8_t {
    WaitingForPlayers = 0,
    InProgress = 1,
    GameOver = 2
};

enum class GameRoomRole : std::uint8_t {
    None = 0,
    Player1 = 1,
    Player2 = 2,
    Spectator = 3
};

enum class PlayerSide : std::uint8_t {
    None = 0,
    White = 1,
    Black = 2
};

enum class JoinRoomResult : std::uint8_t {
    JoinedAsPlayer = 0,
    JoinedAsSpectator = 1,
    AlreadyInRoom = 2,
    RoomFull = 3,
    InvalidState = 4
};

enum class LeaveRoomResult : std::uint8_t {
    Left = 0,
    NotInRoom = 1,
};

enum class MoveResult : std::uint8_t {
    Success = 0,
    NotInRoom = 1,
    NotAPlayer = 2,
    NotYourTurn = 3,
    InvalidMove = 4,
    GameNotActive = 5
};

struct GameRoomView {
    RoomID roomID;
    GameRoomState state;
    SessionID player1;
    SessionID player2;
    std::vector<SessionID> spectators;
    bool whiteTurnToMove;
};

class GameRoom {
public:
    explicit GameRoom(RoomID roomID, SessionID creatorSessionID);
    ~GameRoom() = default;

    GameRoom(const GameRoom&) = delete;
    GameRoom& operator=(const GameRoom&) = delete;
    GameRoom(GameRoom&&) = delete;
    GameRoom& operator=(GameRoom&&) = delete;

    // View Helpers
    [[nodiscard]] RoomID roomID() const noexcept;
    [[nodiscard]] GameRoomState state() const noexcept;
    [[nodiscard]] GameRoomRole roleOf(SessionID sessionID) const noexcept;
    [[nodiscard]] PlayerSide sideOf(SessionID sessionID) const noexcept;
    [[nodiscard]] bool contains(SessionID sessionID) const noexcept;
    [[nodiscard]] bool empty() const noexcept;

    [[nodiscard]] SessionID player1() const noexcept;
    [[nodiscard]] SessionID player2() const noexcept;
    [[nodiscard]] std::vector<SessionID> spectators() const;

    [[nodiscard]] std::vector<SessionID> playerSessionIDs() const;
    [[nodiscard]] std::vector<SessionID> spectatorSessionIDs() const;

    [[nodiscard]] GameRoomView view() const;

    // Controls
    [[nodiscard]] JoinRoomResult joinPlayer(SessionID sessionID);
    [[nodiscard]] JoinRoomResult joinSpectator(SessionID sessionID);
    [[nodiscard]] LeaveRoomResult leave(SessionID sessionID);

    [[nodiscard]] MoveResult submitMove(SessionID sessionID, std::uint8_t from, std::uint8_t to);

private:
    [[nodiscard]] bool canStartGame() const noexcept;
    void startGameIfReady();
    [[nodiscard]] bool isPlayersTurn(SessionID sessionID) const noexcept;
    [[nodiscard]] Pos posFromSquare(std::uint8_t square) const;
    [[nodiscard]] ID pieceAtSquare(std::uint8_t square) const;
    [[nodiscard]] bool isPlayersPiece(SessionID sessionID, ID piece) const noexcept;

private:
    RoomID m_roomID;
    GameRoomState m_state{GameRoomState::WaitingForPlayers};

    SessionID m_player1{0};
    SessionID m_player2{0};
    std::vector<SessionID> m_spectators;

    Board m_board;
};
}

#endif