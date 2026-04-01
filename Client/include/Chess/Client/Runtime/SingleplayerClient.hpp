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

#include "Callback/CallbackRegistry.hpp"

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

struct SingleplayerView {
    SingleplayerState state{SingleplayerState::IDLE};
    COLOR playerColor{COLOR::WHITE};
    COLOR currentTurn{COLOR::WHITE};
    const Board* board{nullptr};
    std::chrono::milliseconds whiteTimeRemaining{0};
    std::chrono::milliseconds blackTimeRemaining{0};
    std::optional<GameResult> result{std::nullopt};
};

class GameClient;

class SingleplayerClient {

public:
    explicit SingleplayerClient();
    ~SingleplayerClient();

    SingleplayerClient(const SingleplayerClient&) = delete;
    SingleplayerClient& operator=(const SingleplayerClient&) = delete;
    SingleplayerClient(SingleplayerClient&&) = delete;
    SingleplayerClient& operator=(SingleplayerClient&&) = delete;

    void start(const SingleplayerConfig& config, std::chrono::steady_clock::time_point now);
    void restart(std::chrono::steady_clock::time_point now);
    void stop();

    void pause(std::chrono::steady_clock::time_point now);
    void resume(std::chrono::steady_clock::time_point now);

    bool tryMove(ID from, Pos to, std::chrono::steady_clock::time_point now);
    void resign(std::chrono::steady_clock::time_point now);

    void tick(std::chrono::steady_clock::time_point now);

    [[nodiscard]] SingleplayerView view(std::chrono::steady_clock::time_point now) const;
    [[nodiscard]] const SingleplayerConfig& config() const noexcept;
    [[nodiscard]] const GameResult& result() const noexcept;
    [[nodiscard]] CallbackRegistry<const GameResult&>& resultRegistry();

private:
    [[nodiscard]] std::chrono::milliseconds timeRemaining(COLOR side, std::chrono::steady_clock::time_point now) const;
    [[nodiscard]] bool isColor(ID id, COLOR color) const noexcept;
    void advanceTurn(std::chrono::steady_clock::time_point now);
    void commitElapsedToActiveSide(std::chrono::steady_clock::time_point now);
    void recordResult(COLOR winner, GameOverReason reason);

private:
    SingleplayerState m_state{SingleplayerState::IDLE};
    SingleplayerConfig m_config{};

    Board m_board;
    COLOR m_currentTurn{COLOR::WHITE};

    std::chrono::milliseconds m_whiteTime{0};
    std::chrono::milliseconds m_blackTime{0};
    std::chrono::steady_clock::time_point m_turnStart;

    GameResult m_result{COLOR::EMPTY, GameOverReason::RESIGN};
    CallbackRegistry<const GameResult&> m_resultRegistry;
};

}

#endif
