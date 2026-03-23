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


namespace Chess {

SingleplayerClient::SingleplayerClient(GameClient& gameClient) : m_gameClient(gameClient) {

}

SingleplayerClient::~SingleplayerClient() {

}

void SingleplayerClient::start(const SingleplayerConfig& config) {
    m_config = config;
    m_board = Board{};
    m_currentTurn = COLOR::WHITE;
    m_state = SingleplayerState::INGAME;

    m_whiteTime = std::chrono::duration_cast<std::chrono::milliseconds>(config.timePerSide);
    m_blackTime = std::chrono::duration_cast<std::chrono::milliseconds>(config.timePerSide);

    m_turnStart = std::chrono::steady_clock::now();
}

void SingleplayerClient::stop() {

}

bool SingleplayerClient::tryMove(ID from, Pos to) {
    if (m_state != SingleplayerState::INGAME) { return false; }
    if (from==ID::EMPTY) { return false; }
    if (!isColor(from, m_currentTurn)) { return false; }
    if (!m_board.isValidMove(from, to)) { return false; }

    m_board.move(from, to);
    advanceTurn();
    return true;
}

void SingleplayerClient::resign() {
    if (m_state != SingleplayerState::INGAME) { return; }

    recordResult((m_currentTurn == COLOR::WHITE) ? COLOR::BLACK : COLOR::WHITE, GameOverReason::RESIGN);
}

void SingleplayerClient::checkTimeout() {
    if (m_state != SingleplayerState::INGAME) { return; }

    if (whiteTimeRemaining() <= std::chrono::milliseconds{0}) {
        recordResult(COLOR::BLACK, GameOverReason::TIMEOUT);
    } else if (blackTimeRemaining() <= std::chrono::milliseconds{0}) {
        recordResult(COLOR::WHITE, GameOverReason::TIMEOUT);
    }
}

std::chrono::milliseconds SingleplayerClient::whiteTimeRemaining() const {
    std::chrono::milliseconds elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - m_turnStart
    );
    return timeRemaining(COLOR::WHITE, elapsed);
}

std::chrono::milliseconds SingleplayerClient::blackTimeRemaining() const {
    std::chrono::milliseconds elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - m_turnStart
    );
    return timeRemaining(COLOR::BLACK, elapsed);
}


bool SingleplayerClient::isColor(ID id, COLOR color) const noexcept {
    if (id == ID::EMPTY) { return false; }
    int castedId = static_cast<int>(id);
    if (color == COLOR::WHITE) { return castedId < BLACK_BOUND; }
    if (color == COLOR::BLACK) { return castedId >= BLACK_BOUND; }
    return false;
}

void SingleplayerClient::advanceTurn() {
    std::chrono::milliseconds elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - m_turnStart);
    std::chrono::milliseconds increment = std::chrono::duration_cast<std::chrono::milliseconds>(
        m_config.increment
    );

    if (m_currentTurn == COLOR::WHITE) {
        m_whiteTime = m_whiteTime - elapsed + increment;
        m_currentTurn = COLOR::BLACK;
    } else {
        m_blackTime = m_blackTime - elapsed + increment;
        m_currentTurn = COLOR::WHITE;
    }

    if (m_whiteTime < std::chrono::milliseconds{0}) { m_whiteTime = std::chrono::milliseconds{0}; }
    if (m_blackTime < std::chrono::milliseconds{0}) { m_blackTime = std::chrono::milliseconds{0}; }

    m_turnStart = std::chrono::steady_clock::now();
}

void SingleplayerClient::recordResult(COLOR winner, GameOverReason reason) {
    m_result = { winner, reason };
    m_state  = SingleplayerState::RESULT;
}

std::chrono::milliseconds SingleplayerClient::timeRemaining(COLOR side, std::chrono::milliseconds elapsed) const {
    if (side == m_currentTurn) {
        std::chrono::milliseconds remaining = (side == COLOR::WHITE ? m_whiteTime : m_blackTime) - elapsed;
        return remaining < std::chrono::milliseconds{0} ? std::chrono::milliseconds{0} : remaining;
    }
    return side == COLOR::WHITE ? m_whiteTime : m_blackTime;
}

}
