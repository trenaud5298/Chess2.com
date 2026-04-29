/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */


// Chess Includes
#include <Chess/Client/Runtime/SingleplayerClient.hpp>

// ASIO Includes

// C++ Includes

namespace Chess {

namespace {

GameOverReason toGameOverReason(ChessGameEndReason reason) {
    switch (reason) {
        case ChessGameEndReason::Checkmate: return GameOverReason::CHECKMATE;
        case ChessGameEndReason::Timeout: return GameOverReason::TIMEOUT;
        case ChessGameEndReason::Resign: return GameOverReason::RESIGN;
        case ChessGameEndReason::Draw: return GameOverReason::DRAW;
        case ChessGameEndReason::None: break;
    }
    return GameOverReason::DRAW;
}

}

SingleplayerClient::SingleplayerClient(std::function<void(ClientEvent)> emitEvent)
: m_emitEvent(std::move(emitEvent)) {

}


SingleplayerClient::~SingleplayerClient() {

}

[[nodiscard]] ClientStatus SingleplayerClient::start(const SingleplayerConfig& config, std::chrono::steady_clock::time_point now) {
    if (m_state != SingleplayerState::IDLE) {
        return ClientStatus::Warning(StatusCode::InvalidState, "SingleplayerClient Must Be Idle To Start");
    }

    m_config = config;
    m_game = ChessGame(ChessClockConfig{
        .enabled = true,
        .initialTime = std::chrono::duration_cast<std::chrono::milliseconds>(config.timePerSide),
        .increment = std::chrono::duration_cast<std::chrono::milliseconds>(config.increment),
    });
    m_game.start(now);

    m_pausedSnapshot.reset();
    m_state = SingleplayerState::INGAME;
    m_result = {COLOR::EMPTY, GameOverReason::RESIGN};

    return ClientStatus::Success();
}

[[nodiscard]] ClientStatus SingleplayerClient::restart(std::chrono::steady_clock::time_point now) {
    if (m_state != SingleplayerState::RESULT) {
        return ClientStatus::Warning(StatusCode::InvalidState, "SingleplayerClient Must Be In Result To Restart");
    }
    ClientStatus stopStatus = stop();
    if (stopStatus.isFailure()) { return stopStatus; }
    return start(m_config, now);
}

[[nodiscard]] ClientStatus SingleplayerClient::stop() {
    if (m_state == SingleplayerState::IDLE) {
        return ClientStatus::Success();
    }

    m_state = SingleplayerState::IDLE;
    m_game.reset();
    m_pausedSnapshot.reset();
    m_result = {COLOR::EMPTY, GameOverReason::RESIGN};

    return ClientStatus::Success();
}


[[nodiscard]] ClientStatus SingleplayerClient::pause(std::chrono::steady_clock::time_point now) {
    if (m_state != SingleplayerState::INGAME) {
        return ClientStatus::Warning(StatusCode::InvalidState, "SingleplayerClient Must Be In-Game To Pause");
    }
    m_pausedSnapshot = m_game.snapshot(now);
    m_state = SingleplayerState::PAUSED;
    return ClientStatus::Success();
}

[[nodiscard]] ClientStatus SingleplayerClient::resume(std::chrono::steady_clock::time_point now) {
    if (m_state != SingleplayerState::PAUSED) {
        return ClientStatus::Warning(StatusCode::InvalidState, "SingleplayerClient Must Be Paused To Resume");
    }

    if (m_pausedSnapshot.has_value()) {
        m_game.applySnapshot(*m_pausedSnapshot, now);
        m_pausedSnapshot.reset();
    }

    m_state = SingleplayerState::INGAME;
    return ClientStatus::Success();
}

[[nodiscard]] ClientStatus SingleplayerClient::tryMove(std::uint8_t from, std::uint8_t to, PromotionPiece promotion, std::chrono::steady_clock::time_point now) {
    if (m_state != SingleplayerState::INGAME) {
        return ClientStatus::Warning(StatusCode::InvalidState, "SingleplayerClient Must Be In-Game To Move");
    }

    ChessGameMoveResult result = m_game.submitMove(m_game.currentTurn(), from, to, promotion, now);

    if (!result) {
        return ClientStatus::Warning(StatusCode::InvalidMove, "Singleplayer Move Rejected");
    }

    recordFinishedGame();
    return ClientStatus::Success();
}

[[nodiscard]] ClientStatus SingleplayerClient::resign(std::chrono::steady_clock::time_point now) {
    if (m_state != SingleplayerState::INGAME) {
        return ClientStatus::Warning(StatusCode::InvalidState, "SingleplayerClient Must Be In-Game To Resign");
    }

    if (!m_game.resign(m_game.currentTurn(), now)) {
        return ClientStatus::Warning(StatusCode::InvalidState, "SingleplayerClient Failed To Resign");
    }

    recordFinishedGame();
    return ClientStatus::Success();
}


void SingleplayerClient::tick(std::chrono::steady_clock::time_point now) {
    if (m_state != SingleplayerState::INGAME) { return; }

    m_game.tick(now);
    recordFinishedGame();
}


[[nodiscard]] SingleplayerView SingleplayerClient::view(std::chrono::steady_clock::time_point now) const {
    bool paused = (m_state == SingleplayerState::PAUSED) && m_pausedSnapshot.has_value();

    std::chrono::milliseconds whiteRemaining = paused ? m_pausedSnapshot->whiteTimeRemaining : m_game.whiteTimeRemaining(now);
    std::chrono::milliseconds blackRemaining = paused ? m_pausedSnapshot->blackTimeRemaining : m_game.blackTimeRemaining(now);

    return {
        .state = m_state,
        .playerColor = m_config.playerColor,
        .currentTurn = paused ? m_pausedSnapshot->currentTurn : m_game.currentTurn(),
        .board = &m_game.board(),
        .whiteTimeRemaining = whiteRemaining,
        .blackTimeRemaining = blackRemaining,
        .result = (m_state == SingleplayerState::RESULT ? std::make_optional(m_result) : std::nullopt)
    };
}

[[nodiscard]] const SingleplayerConfig& SingleplayerClient::config() const noexcept {
    return m_config;
}

[[nodiscard]] const GameResult& SingleplayerClient::result() const noexcept {
    return m_result;
}



// Helpers
void SingleplayerClient::recordResult(COLOR winner, GameOverReason reason) {
    if (m_state == SingleplayerState::RESULT) {
        return;
    }

    m_result = { winner, reason };
    m_state  = SingleplayerState::RESULT;

    emitEvent(ClientEvent::Result(
        EventSource::Singleplayer,
        EventType::SingleplayerGameEnd,
        ClientStatus::Success("SingleplayerClient Game Ended")
    ));
}

void SingleplayerClient::emitEvent(ClientEvent event) {
    if (m_emitEvent) {
        m_emitEvent(std::move(event));
    }
}

void SingleplayerClient::recordFinishedGame() {
    if (!m_game.isFinished()) {
        return;
    }

    recordResult(m_game.winner(), toGameOverReason(m_game.endReason()));
}
}
