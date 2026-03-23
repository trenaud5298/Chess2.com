#ifndef CHESS_CLIENT_RUNTIME_SINGLEPLAYERCLIENT_HPP
#define CHESS_CLIENT_RUNTIME_SINGLEPLAYERCLIENT_HPP

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
#include <chrono>
#include <optional>

namespace Chess {

struct SingleplayerConfig {
    COLOR playerColor{COLOR::WHITE};
    std::chrono::seconds timePerSide{300};
    std::chrono::seconds increment{0};
};

enum class GameOverReason {
    CHECKMATE,
    TIMEOUT,
    RESIGN,
};

struct GameResult {
    COLOR winner;
    GameOverReason reason;
};

enum class SingleplayerState {
    IDLE,
    INGAME,
    PAUSED,
    RESULT,
};

class GameClient;

class SingleplayerClient {

public:
    explicit SingleplayerClient(GameClient& gameClient);
    ~SingleplayerClient();

    SingleplayerClient(const SingleplayerClient&) = delete;
    SingleplayerClient& operator=(const SingleplayerClient&) = delete;
    SingleplayerClient(SingleplayerClient&&) = delete;
    SingleplayerClient& operator=(SingleplayerClient&&) = delete;

    void start(const SingleplayerConfig& config);
    void stop();
    void pause();
    void resume();

    bool tryMove(ID from, Pos to);

    void resign();

    void checkTimeout();

    [[nodiscard]] SingleplayerState state() const noexcept { return m_state; }
    [[nodiscard]] COLOR currentTurn() const noexcept { return m_currentTurn; }
    [[nodiscard]] COLOR playerColor() const noexcept { return m_config.playerColor; }
    [[nodiscard]] const Board& board() const noexcept { return m_board; }
    [[nodiscard]] Board& board() noexcept { return m_board; }
    [[nodiscard]] const SingleplayerConfig& config()  const noexcept { return m_config; }

    [[nodiscard]] const GameResult& result() const noexcept { return m_result; }

    [[nodiscard]] std::chrono::milliseconds whiteTimeRemaining() const;
    [[nodiscard]] std::chrono::milliseconds blackTimeRemaining() const;

private:
    [[nodiscard]] bool isColor(ID id, COLOR color) const noexcept;
    void advanceTurn();
    void recordResult(COLOR winner, GameOverReason reason);
    [[nodiscard]] std::chrono::milliseconds timeRemaining(COLOR side, std::chrono::milliseconds elapsed) const;

private:
    GameClient& m_gameClient;
    SingleplayerState m_state{SingleplayerState::IDLE};
    SingleplayerConfig m_config{};

    Board m_board;
    COLOR m_currentTurn{COLOR::WHITE};

    std::chrono::milliseconds m_whiteTime{0};
    std::chrono::milliseconds m_blackTime{0};

    std::chrono::steady_clock::time_point m_turnStart;
    std::chrono::steady_clock::time_point m_pauseStart;

    GameResult m_result{COLOR::EMPTY, GameOverReason::RESIGN};
};

}

#endif