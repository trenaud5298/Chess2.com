/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Core/Game/ChessGame.hpp>

// ASIO Includes

// C++ Includes
#include <algorithm>
#include <utility>


namespace {

[[nodiscard]] bool isPlayableSide(COLOR side) noexcept {
    return side == COLOR::WHITE || side == COLOR::BLACK;
}

[[nodiscard]] COLOR oppositeColor(COLOR side) noexcept {
    switch (side) {
        case COLOR::EMPTY: return COLOR::EMPTY;
        case COLOR::WHITE: return COLOR::BLACK;
        case COLOR::BLACK: return COLOR::WHITE;
    }
    return COLOR::EMPTY;
}

}

namespace Chess {


ChessGame::ChessGame() {}

ChessGame::ChessGame(ChessClockConfig clockConfig)
: m_clockConfig(std::move(clockConfig)) {

}

ChessGame::ChessGame(const ChessGameSnapshot &snapshot, std::chrono::steady_clock::time_point now) {
    applySnapshot(snapshot, now);
}

ChessGame::~ChessGame() {

}


void ChessGame::reset() {
    m_board = Board{};
    m_clockConfig = {};
    m_state = ChessGameState::NotStarted;
    m_currentTurn = COLOR::WHITE;
    m_winner = COLOR::EMPTY;
    m_endReason = ChessGameEndReason::None;
    m_whiteTimeRemaining = std::chrono::milliseconds{0};
    m_blackTimeRemaining = std::chrono::milliseconds{0};
    m_turnStart = {};
    m_version = 0;

    refreshBoardDerivedState();
}


void ChessGame::configureClock(ChessClockConfig clockConfig) {
    m_clockConfig = clockConfig;
}

void ChessGame::start(std::chrono::steady_clock::time_point now) {
    m_board = Board{};
    m_state = ChessGameState::InProgress;
    m_currentTurn = COLOR::WHITE;
    m_winner = COLOR::EMPTY;
    m_endReason = ChessGameEndReason::None;
    m_whiteTimeRemaining = m_clockConfig.enabled ? m_clockConfig.initialTime : std::chrono::milliseconds{0};
    m_blackTimeRemaining = m_clockConfig.enabled ? m_clockConfig.initialTime : std::chrono::milliseconds{0};
    m_turnStart = now;
    m_version = 1;

    refreshBoardDerivedState();
    refreshTerminalStateFromBoard(now);
}

void ChessGame::tick(std::chrono::steady_clock::time_point now) {
    if (m_state != ChessGameState::InProgress) {
        return;
    }
    refreshTerminalStateFromBoard(now);
}

void ChessGame::applySnapshot(const ChessGameSnapshot &snapshot, std::chrono::steady_clock::time_point now) {
    m_state = snapshot.state;
    m_currentTurn = snapshot.currentTurn;
    m_winner = snapshot.winner;
    m_endReason = snapshot.endReason;
    m_clockConfig = snapshot.clockConfig;
    m_whiteTimeRemaining = snapshot.whiteTimeRemaining;
    m_blackTimeRemaining = snapshot.blackTimeRemaining;
    m_version = snapshot.version;
    m_turnStart = (snapshot.state == ChessGameState::InProgress) ? now : std::chrono::steady_clock::time_point{};

    rebuildBoardFromRaw(snapshot.board);
    refreshBoardDerivedState();
}

ChessGameMoveResult ChessGame::submitMove(COLOR side, std::uint8_t from, std::uint8_t to, std::chrono::steady_clock::time_point now) {
    ChessGameState preState = m_state;
    std::uint64_t preVersion = m_version;

    tick(now);

    if (m_state == ChessGameState::NotStarted) {
        return {
            .status = ChessGameMoveStatus::GameNotStarted,
            .stateChanged = (m_state != preState || m_version != preVersion),
            .turnAdvanced = false,
            .gameFinished = (m_state == ChessGameState::Finished),
            .winner = m_winner,
            .endReason = m_endReason
        };
    }

    if (m_state == ChessGameState::Finished) {
        return {
            .status = ChessGameMoveStatus::GameAlreadyFinished,
            .stateChanged = (m_state != preState || m_version != preVersion),
            .turnAdvanced = false,
            .gameFinished = (m_state == ChessGameState::Finished),
            .winner = m_winner,
            .endReason = m_endReason
        };
    }

    if (!isPlayableSide(side)) {
        return {
            .status = ChessGameMoveStatus::InvalidSide,
            .stateChanged = (m_state != preState || m_version != preVersion),
            .turnAdvanced = false,
            .gameFinished = (m_state == ChessGameState::Finished),
            .winner = m_winner,
            .endReason = m_endReason
        };
    }

    if (side != m_currentTurn) {
        return {
            .status = ChessGameMoveStatus::WrongTurn,
            .stateChanged = (m_state != preState || m_version != preVersion),
            .turnAdvanced = false,
            .gameFinished = (m_state == ChessGameState::Finished),
            .winner = m_winner,
            .endReason = m_endReason
        };
    }

    if (from >= 64 || to >= 64) {
        return {
            .status = ChessGameMoveStatus::SquareOutOfBounds,
            .stateChanged = (m_state != preState || m_version != preVersion),
            .turnAdvanced = false,
            .gameFinished = (m_state == ChessGameState::Finished),
            .winner = m_winner,
            .endReason = m_endReason
        };
    }

    ID piece = m_board.getBoard()[from];
    if (piece == ID::EMPTY) {
        return {
            .status = ChessGameMoveStatus::EmptySquare,
            .stateChanged = (m_state != preState || m_version != preVersion),
            .turnAdvanced = false,
            .gameFinished = (m_state == ChessGameState::Finished),
            .winner = m_winner,
            .endReason = m_endReason
        };
    }

    if (!isColor(piece, side)) {
        return {
            .status = ChessGameMoveStatus::WrongPieceColor,
            .stateChanged = (m_state != preState || m_version != preVersion),
            .turnAdvanced = false,
            .gameFinished = (m_state == ChessGameState::Finished),
            .winner = m_winner,
            .endReason = m_endReason
        };
    }

    m_board.setTurn(side == COLOR::WHITE);
    m_board.genMoves();

    Pos target = posFromSquare(to);
    if (!m_board.isValidMove(piece, target)) {
        return {
            .status = ChessGameMoveStatus::InvalidMove,
            .stateChanged = (m_state != preState || m_version != preVersion),
            .turnAdvanced = false,
            .gameFinished = (m_state == ChessGameState::Finished),
            .winner = m_winner,
            .endReason = m_endReason
        };
    }

    m_board.move(piece, target);
    ++m_version;

    advanceTurn(now);
    refreshTerminalStateFromBoard(now);

    return {
        .status = ChessGameMoveStatus::Success,
        .stateChanged = (m_state != preState || m_version != preVersion),
        .turnAdvanced = true,
        .gameFinished = (m_state == ChessGameState::Finished),
        .winner = m_winner,
        .endReason = m_endReason
    };
}


bool ChessGame::resign(COLOR side, std::chrono::steady_clock::time_point now) {
    if (m_state != ChessGameState::InProgress) {
        return false;
    }

    if (!isPlayableSide(side)) {
        return false;
    }

    if (m_clockConfig.enabled) {
        commitElapsedToActiveSide(now);
    }

    finish(oppositeColor(side), ChessGameEndReason::Resign, now);
    return true;
}

ChessGameState ChessGame::state() const noexcept {
    return m_state;
}

bool ChessGame::isStarted() const noexcept {
    return m_state != ChessGameState::NotStarted;
}

bool ChessGame::isInProgress() const noexcept {
    return m_state == ChessGameState::InProgress;
}

bool ChessGame::isFinished() const noexcept {
    return m_state == ChessGameState::Finished;
}

COLOR ChessGame::currentTurn() const noexcept {
    return m_currentTurn;
}

COLOR ChessGame::winner() const noexcept {
    return m_winner;
}

ChessGameEndReason ChessGame::endReason() const noexcept {
    return m_endReason;
}

std::uint64_t ChessGame::version() const noexcept {
    return m_version;
}

const ChessClockConfig& ChessGame::clockConfig() const noexcept {
    return m_clockConfig;
}

bool ChessGame::clocksEnabled() const noexcept {
    return m_clockConfig.enabled;
}

const Board& ChessGame::board() const noexcept {
    return m_board;
}

const std::array<ID, 64>& ChessGame::boardRaw() const noexcept {
    return m_board.getBoard();
}

std::chrono::milliseconds ChessGame::whiteTimeRemaining(std::chrono::steady_clock::time_point now) const {
    return timeRemaining(COLOR::WHITE, now);
}

std::chrono::milliseconds ChessGame::blackTimeRemaining(std::chrono::steady_clock::time_point now) const {
    return timeRemaining(COLOR::BLACK, now);
}

std::chrono::milliseconds ChessGame::timeRemaining(COLOR side, std::chrono::steady_clock::time_point now) const {
    if (!m_clockConfig.enabled) {
        return std::chrono::milliseconds(0);
    }

    if (!isPlayableSide(side)) {
        return std::chrono::milliseconds(0);
    }

    std::chrono::milliseconds remaining = (side == COLOR::WHITE) ? m_whiteTimeRemaining : m_blackTimeRemaining;
    if (m_state == ChessGameState::InProgress && side == m_currentTurn) {
        remaining -= std::chrono::duration_cast<std::chrono::milliseconds>(now - m_turnStart);
    }
    return std::max(std::chrono::milliseconds(0), remaining);
}


ChessGameSnapshot ChessGame::snapshot(std::chrono::steady_clock::time_point now) const {
    return {
        .board = m_board.getBoard(),
        .state = m_state,
        .currentTurn = m_currentTurn,
        .winner = m_winner,
        .endReason = m_endReason,
        .clockConfig = m_clockConfig,
        .whiteTimeRemaining = whiteTimeRemaining(now),
        .blackTimeRemaining = blackTimeRemaining(now),
        .version = m_version
    };
}

void ChessGame::rebuildBoardFromRaw(const std::array<ID, 64> &boardRaw) {
    std::vector<IdPos> pieces = collectPieces(boardRaw);
    std::array<Piece, 32> pieceArray = Board::genPieces(pieces);
    m_board = Board(boardRaw, pieceArray);
}


void ChessGame::refreshBoardDerivedState() {
    m_board.setTurn(m_currentTurn == COLOR::WHITE);
    m_board.genMoves();
}

void ChessGame::refreshTerminalStateFromBoard(std::chrono::steady_clock::time_point now) {
    if (m_state != ChessGameState::InProgress) {
        return;
    }


    if (m_clockConfig.enabled && timeRemaining(m_currentTurn, now) <= std::chrono::milliseconds{0}) {
        commitElapsedToActiveSide(now);
        finish(oppositeColor(m_currentTurn), ChessGameEndReason::Timeout, now);
        return;
    }

    // TODO: Need winner() function here
    // COLOR checkMateWinner = m_board.winner();
    // if (isPlayableSide(checkMateWinner)) {
    //     finish(checkMateWinner, ChessGameEndReason::Checkmate, now);
    // }
}

void ChessGame::finish(COLOR winner, ChessGameEndReason reason, std::chrono::steady_clock::time_point now) {
    if (m_state == ChessGameState::Finished) {
        return;
    }

    m_state = ChessGameState::Finished;
    m_winner = winner;
    m_endReason = reason;
    ++m_version;
}

void ChessGame::advanceTurn(std::chrono::steady_clock::time_point now) {
    commitElapsedToActiveSide(now);
    if (m_clockConfig.enabled) {
        if (m_currentTurn == COLOR::WHITE) {
            m_whiteTimeRemaining += m_clockConfig.incrememnt;
        } else {
            m_blackTimeRemaining += m_clockConfig.incrememnt;
        }
    }
    m_currentTurn = oppositeColor(m_currentTurn);
    refreshBoardDerivedState();
}

void ChessGame::commitElapsedToActiveSide(std::chrono::steady_clock::time_point now) {
    if (!m_clockConfig.enabled || m_state != ChessGameState::InProgress) {
        m_turnStart = now;
        return;
    }

    std::chrono::milliseconds elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_turnStart);

    if (m_currentTurn == COLOR::WHITE) {
        m_whiteTimeRemaining = std::max(std::chrono::milliseconds(0), m_whiteTimeRemaining - elapsed);
    } else {
        m_blackTimeRemaining = std::max(std::chrono::milliseconds(0), m_blackTimeRemaining - elapsed);
    }
    m_turnStart = now;
}


bool ChessGame::isColor(ID piece, COLOR side) const noexcept {
    if (piece == ID::EMPTY) {
        return false;
    }
    int value = static_cast<int>(piece);

    if (side == COLOR::WHITE) {
        return value < BLACK_BOUND;
    }

    if (side == COLOR::BLACK) {
        return value >= BLACK_BOUND;
    }

    return false;
}

Pos ChessGame::posFromSquare(std::uint8_t square) noexcept {
    return Pos{
        static_cast<std::uint8_t>(square / 8),
        static_cast<std::uint8_t>(square % 8)
    };
}

std::vector<IdPos> ChessGame::collectPieces(const std::array<ID, 64>& boardRaw) {
    std::vector<IdPos> out;
    out.reserve(32);

    for (std::size_t index = 0; index < boardRaw.size(); ++index) {
        ID id = boardRaw[index];
        if (id == ID::EMPTY) {
            continue;
        }

        out.push_back(IdPos{
            .id = id,
            .pos = Pos{
                static_cast<std::uint8_t>(index / 8),
                static_cast<std::uint8_t>(index % 8)
            }
        });
    }
    return out;
}

}
