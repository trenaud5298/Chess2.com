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
#include <Chess/Core/Game/ChessGame.hpp>
#include <Chess/Core/Common/Types.hpp>

// ASIO Includes

// C++ Includes
#include <cstdint>
#include <optional>
#include <vector>

#include "Chess/Core/Networking/MessagePayloads.hpp"

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

enum class GameRoomMoveStatus : std::uint8_t {
    Success = 0,
    NotInRoom = 1,
    SpectatorCannotMove = 2,
    GameRejected = 3
};

struct GameRoomMoveResult {
    GameRoomMoveStatus status{GameRoomMoveStatus::GameRejected};
    ChessGameMoveResult gameResult{};

    [[nodiscard]] explicit operator bool() const noexcept {
        return status == GameRoomMoveStatus::Success && static_cast<bool>(gameResult);
    }
};

struct GameRoomView {
    RoomID roomID{0};
    GameRoomState state{GameRoomState::WaitingForPlayers};
    SessionID player1{0};
    SessionID player2{0};
    std::vector<SessionID> spectators;
    COLOR currentTurn{COLOR::WHITE};
    COLOR winner{COLOR::EMPTY};
    ChessGameState gameState{ChessGameState::NotStarted};
    std::uint64_t roomVersion{0};
    std::uint64_t gameVersion{0};
};

class GameRoom {
public:
    explicit GameRoom(RoomID roomID, SessionID creatorSessionID, RoomCreateConfig config = {});
    ~GameRoom();

    GameRoom(const GameRoom&) = delete;
    GameRoom& operator=(const GameRoom&) = delete;
    GameRoom(GameRoom&&) = delete;
    GameRoom& operator=(GameRoom&&) = delete;

    // View Helpers
    [[nodiscard]] RoomID roomID() const noexcept;
    [[nodiscard]] GameRoomState state() const noexcept;
    [[nodiscard]] const RoomCreateConfig& config() const noexcept;
    [[nodiscard]] PublicRoomConfig publicConfig() const noexcept;
    [[nodiscard]] std::uint64_t roomVersion() const noexcept;
    [[nodiscard]] GameRoomRole roleOf(SessionID sessionID) const noexcept;
    [[nodiscard]] PlayerSide sideOf(SessionID sessionID) const noexcept;
    [[nodiscard]] COLOR colorOf(SessionID sessionID) const noexcept;
    [[nodiscard]] bool contains(SessionID sessionID) const noexcept;
    [[nodiscard]] bool empty() const noexcept;

    [[nodiscard]] SessionID player1() const noexcept;
    [[nodiscard]] SessionID player2() const noexcept;
    [[nodiscard]] std::vector<SessionID> playerSessionIDs() const;
    [[nodiscard]] std::vector<SessionID> spectatorSessionIDs() const;
    [[nodiscard]] std::vector<SessionID> allSessionIDs() const;

    [[nodiscard]] GameRoomView view() const;
    [[nodiscard]] const ChessGame& game() const noexcept;
    [[nodiscard]] ChessGame& game() noexcept;

    // Controls
    [[nodiscard]] JoinRoomResult joinPlayer(SessionID sessionID, std::chrono::steady_clock::time_point now);
    [[nodiscard]] JoinRoomResult joinSpectator(SessionID sessionID);
    [[nodiscard]] LeaveRoomResult leave(SessionID sessionID, std::chrono::steady_clock::time_point now);

    [[nodiscard]] GameRoomMoveResult submitMove(SessionID sessionID, std::uint8_t from, std::uint8_t to, PromotionPiece promotion, std::chrono::steady_clock::time_point now);

    [[nodiscard]] bool resign(SessionID sessionID, std::chrono::steady_clock::time_point now);
    void tick(std::chrono::steady_clock::time_point now);

private:
    void incrementRoomVersion();
    [[nodiscard]] bool canStartGame() const noexcept;
    void startGameIfReady(std::chrono::steady_clock::time_point now);
    void updateRoomStateFromGame() noexcept;

private:
    RoomID m_roomID;
    RoomCreateConfig m_config;
    GameRoomState m_state{GameRoomState::WaitingForPlayers};
    std::uint64_t m_roomVersion{0};

    SessionID m_player1{0};
    SessionID m_player2{0};
    std::vector<SessionID> m_spectators;

    ChessGame m_game;
};
}

#endif