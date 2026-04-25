#ifndef CHESS_CORE_GAME_CHESSGAME_HPP
#define CHESS_CORE_GAME_CHESSGAME_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Core/Game/Board.hpp>

// ASIO Includes

// C++ Includes
#include <array>
#include <chrono>
#include <cstdint>
#include <vector>

namespace Chess {

enum class ChessGameState : std::uint8_t {
    NotStarted = 0,
    InProgress = 1,
    Finished = 2
};

enum class ChessGameEndReason : std::uint8_t {
    None = 0,
    Checkmate = 1,
    Timeout = 2,
    Resign = 3,
    Draw = 4
};

enum class PromotionPiece : std::uint8_t {
    None = 0,
    Queen = 1,
    Rook = 2,
    Bishop = 3,
    Knight = 4,
};

enum class ChessGameMoveStatus : std::uint8_t {
    Success = 0,
    GameNotStarted = 1,
    GameAlreadyFinished = 2,
    InvalidSide = 3,
    WrongTurn = 4,
    SquareOutOfBounds = 5,
    EmptySquare = 6,
    WrongPieceColor = 7,
    InvalidMove = 8,
    PromotionRequired = 9,
    PromotionNotSupported = 10
};

struct ChessClockConfig {
    bool enabled{false};
    std::chrono::milliseconds initialTime{0};
    std::chrono::milliseconds increment{0};
};

struct ChessGameSnapshot {
    std::array<ID, 64> board{};
    ChessGameState state{ChessGameState::NotStarted};
    COLOR currentTurn{COLOR::WHITE};
    COLOR winner{COLOR::EMPTY};
    ChessGameEndReason endReason{ChessGameEndReason::None};
    ChessClockConfig clockConfig{};
    std::chrono::milliseconds whiteTimeRemaining{0};
    std::chrono::milliseconds blackTimeRemaining{0};
    std::uint64_t version{0};
};

struct ChessGameMoveResult {
    ChessGameMoveStatus status{ChessGameMoveStatus::GameNotStarted};
    bool stateChanged{false};
    bool turnAdvanced{false};
    bool gameFinished{false};
    COLOR winner{COLOR::EMPTY};
    ChessGameEndReason endReason{ChessGameEndReason::None};

    [[nodiscard]] explicit operator bool() const noexcept {
        return status == ChessGameMoveStatus::Success;
    }
};



class ChessGame {

public:

    ChessGame();
    explicit ChessGame(ChessClockConfig clockConfig);
    ChessGame(const ChessGameSnapshot& snapshot, std::chrono::steady_clock::time_point now);
    ~ChessGame();

    // Controls
    void reset();
    void configureClock(ChessClockConfig clockConfig);
    void start(std::chrono::steady_clock::time_point now);
    void tick(std::chrono::steady_clock::time_point now);
    void applySnapshot(const ChessGameSnapshot& snapshot, std::chrono::steady_clock::time_point now);

    [[nodiscard]] ChessGameMoveResult submitMove(COLOR side, std::uint8_t from, std::uint8_t to, PromotionPiece promotion, std::chrono::steady_clock::time_point now);
    [[nodiscard]] bool resign(COLOR side, std::chrono::steady_clock::time_point now);

    // View
    [[nodiscard]] ChessGameState state() const noexcept;
    [[nodiscard]] bool isStarted() const noexcept;
    [[nodiscard]] bool isInProgress() const noexcept;
    [[nodiscard]] bool isFinished() const noexcept;

    [[nodiscard]] COLOR currentTurn() const noexcept;
    [[nodiscard]] COLOR winner() const noexcept;
    [[nodiscard]] ChessGameEndReason endReason() const noexcept;
    [[nodiscard]] std::uint64_t version() const noexcept;

    [[nodiscard]] const ChessClockConfig& clockConfig() const noexcept;
    [[nodiscard]] bool clocksEnabled() const noexcept;

    [[nodiscard]] const Board& board() const noexcept;
    [[nodiscard]] const std::array<ID, 64>& boardRaw() const noexcept;

    [[nodiscard]] std::chrono::milliseconds whiteTimeRemaining(std::chrono::steady_clock::time_point now) const;
    [[nodiscard]] std::chrono::milliseconds blackTimeRemaining(std::chrono::steady_clock::time_point now) const;
    [[nodiscard]] std::chrono::milliseconds timeRemaining(COLOR side, std::chrono::steady_clock::time_point now) const;

    [[nodiscard]] ChessGameSnapshot snapshot(std::chrono::steady_clock::time_point now) const;

private:
    void rebuildBoardFromRaw(const std::array<ID, 64>& boardRaw);
    void refreshBoardDerivedState();
    void refreshTerminalStateFromBoard(std::chrono::steady_clock::time_point now);
    void finish(COLOR winner, ChessGameEndReason reason, std::chrono::steady_clock::time_point now);
    void advanceTurn(std::chrono::steady_clock::time_point now);
    void commitElapsedToActiveSide(std::chrono::steady_clock::time_point now);

    [[nodiscard]] bool isColor(ID piece, COLOR side) const noexcept;
    [[nodiscard]] bool requiresPromotion(ID piece, COLOR side, const Pos& target) const noexcept;
    [[nodiscard]] static Pos posFromSquare(std::uint8_t square) noexcept;
    [[nodiscard]] static std::vector<IdPos> collectPieces(const std::array<ID, 64>& boardRaw);
    [[nodiscard]] static bool isPawn(ID piece) noexcept;
    [[nodiscard]] static bool isPromotionRank(COLOR side, const Pos& target) noexcept;
private:

    Board m_board{};
    ChessClockConfig m_clockConfig{};

    ChessGameState m_state{ChessGameState::NotStarted};
    COLOR m_currentTurn{COLOR::WHITE};
    COLOR m_winner{COLOR::EMPTY};
    ChessGameEndReason m_endReason{ChessGameEndReason::None};

    std::chrono::milliseconds m_whiteTimeRemaining{0};
    std::chrono::milliseconds m_blackTimeRemaining{0};
    std::chrono::steady_clock::time_point m_turnStart{};

    std::uint64_t m_version{0};
};





}

#endif