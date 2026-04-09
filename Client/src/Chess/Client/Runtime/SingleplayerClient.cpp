/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */


// Chess Includes
#include <Chess/Client/Runtime/SingleplayerClient.hpp>
#include <Chess/Client/Runtime/GameClient.hpp>

// ASIO Includes

// C++ Includes
#include <iostream>

namespace Chess {

SingleplayerClient::SingleplayerClient() {

}

SingleplayerClient::~SingleplayerClient() {

}

void SingleplayerClient::start(const SingleplayerConfig& config, std::chrono::steady_clock::time_point now) {
    m_config = config;
    m_board = Board{};
    m_board.genMoves();
    m_currentTurn = COLOR::WHITE;
    m_state = SingleplayerState::INGAME;
    m_result = {COLOR::EMPTY, GameOverReason::RESIGN};
    m_whiteTime = config.timePerSide;
    m_blackTime = config.timePerSide;
    m_turnStart = now;
}

void SingleplayerClient::restart(std::chrono::steady_clock::time_point now) {
    stop();
    start(m_config, now);
}

void SingleplayerClient::stop() {
    if (m_state == SingleplayerState::IDLE) { return; }
    m_state = SingleplayerState::IDLE;
    m_board = Board{};
    m_currentTurn = COLOR::WHITE;
    m_whiteTime = std::chrono::milliseconds{0};
    m_blackTime = std::chrono::milliseconds{0};
    m_turnStart = {};
    m_result = {COLOR::EMPTY, GameOverReason::RESIGN};
}


void SingleplayerClient::pause(std::chrono::steady_clock::time_point now) {
    if (m_state != SingleplayerState::INGAME) { return; }
    commitElapsedToActiveSide(now);
    m_state = SingleplayerState::PAUSED;
}

void SingleplayerClient::resume(std::chrono::steady_clock::time_point now) {
    if (m_state != SingleplayerState::PAUSED) { return; }
    m_turnStart = now;
    m_state = SingleplayerState::INGAME;
}

bool SingleplayerClient::tryMove(ID from, Pos to, std::chrono::steady_clock::time_point now) {
    if (m_state != SingleplayerState::INGAME) { return false; }
    if (from==ID::EMPTY) { return false; }
    if (timeRemaining(m_currentTurn, now) <= std::chrono::milliseconds{0}) { return false; }
    if (!m_board.isValidMove(from, to)) { return false; }

    m_board.move(from, to);
    advanceTurn(now);
    return true;
}

void SingleplayerClient::resign(std::chrono::steady_clock::time_point now) {
    if (m_state != SingleplayerState::INGAME) { return; }
    commitElapsedToActiveSide(now);
    recordResult((m_currentTurn == COLOR::WHITE) ? COLOR::BLACK : COLOR::WHITE, GameOverReason::RESIGN);
}


void SingleplayerClient::tick(std::chrono::steady_clock::time_point now) {
    if (m_state != SingleplayerState::INGAME) { return; }
    if (timeRemaining(m_currentTurn, now) <= std::chrono::milliseconds{0}) {
        commitElapsedToActiveSide(now);
        recordResult(m_currentTurn == COLOR::WHITE ? COLOR::BLACK : COLOR::WHITE, GameOverReason::TIMEOUT);
    }
}


[[nodiscard]] SingleplayerView SingleplayerClient::view(std::chrono::steady_clock::time_point now) const {
    return {
        .state = m_state,
        .playerColor = m_config.playerColor,
        .currentTurn = m_currentTurn,
        .board = &m_board,
        .whiteTimeRemaining = timeRemaining(COLOR::WHITE, now),
        .blackTimeRemaining = timeRemaining(COLOR::BLACK, now),
        .result = (m_state == SingleplayerState::RESULT ? std::make_optional(m_result) : std::nullopt)
    };
}

[[nodiscard]] const SingleplayerConfig& SingleplayerClient::config() const noexcept {
    return m_config;
}

[[nodiscard]] const GameResult& SingleplayerClient::result() const noexcept {
    return m_result;
}


[[nodiscard]] CallbackRegistry<const GameResult&>& SingleplayerClient::resultRegistry() {
    return m_resultRegistry;
}



// Helpers
std::chrono::milliseconds SingleplayerClient::timeRemaining(COLOR side, std::chrono::steady_clock::time_point now) const {
    std::chrono::milliseconds sideTime = (side == COLOR::WHITE ? m_whiteTime : m_blackTime);
    if (m_state == SingleplayerState::INGAME && m_currentTurn == side) {
        sideTime -= std::chrono::duration_cast<std::chrono::milliseconds>(now - m_turnStart);
    }
    return std::max(std::chrono::milliseconds{0}, sideTime);
}

bool SingleplayerClient::isColor(ID id, COLOR color) const noexcept {
    if (id == ID::EMPTY) { return false; }
    int castedId = static_cast<int>(id);
    if (color == COLOR::WHITE) { return castedId < BLACK_BOUND; }
    if (color == COLOR::BLACK) { return castedId >= BLACK_BOUND; }
    return false;
}

void SingleplayerClient::advanceTurn(std::chrono::steady_clock::time_point now) {
    commitElapsedToActiveSide(now);
    if (m_currentTurn == COLOR::WHITE) {
        m_whiteTime += m_config.increment;
        m_currentTurn = COLOR::BLACK;
    } else {
        m_blackTime += m_config.increment;
        m_currentTurn = COLOR::WHITE;
    }
    m_board.genMoves();
}

void SingleplayerClient::commitElapsedToActiveSide(std::chrono::steady_clock::time_point now) {
    if (m_currentTurn == COLOR::WHITE) {
        m_whiteTime -= std::chrono::duration_cast<std::chrono::milliseconds>(now - m_turnStart);
    } else {
        m_blackTime -= std::chrono::duration_cast<std::chrono::milliseconds>(now - m_turnStart);
    }
    m_turnStart = now;
}


void SingleplayerClient::recordResult(COLOR winner, GameOverReason reason) {
    if (m_state == SingleplayerState::RESULT) {return;}
    m_result = { winner, reason };
    m_state  = SingleplayerState::RESULT;
    m_resultRegistry.fire(m_result);
}

}
