/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Screen/SingleplayerGameScreen.hpp>
#include <Chess/Client/UI/ClientPanel.hpp>
#include <Chess/Core/UI/ChessBoardDisplay.hpp>
#include <Chess/Client/Runtime/GameClient.hpp>
#include <Chess/Core/Common/TimeFormat.hpp>
#include <Chess/Client/UI/Modal/ConfirmModal.hpp>
#include <Chess/Client/UI/Modal/ErrorModal.hpp>
#include <Chess/Client/UI/Modal/TwoButtonModal.hpp>

// FTXUI Includes

// C++ Includes

namespace Chess {

SingleplayerGameScreen::SingleplayerGameScreen(ClientPanel& clientPanel) : ScreenInterface(clientPanel), m_boardDisplay(std::make_shared<ChessBoardDisplay>()) {
    m_boardDisplay->onMove = [this](ID from, Pos to) {
        SingleplayerClient& singlePlayerClient = m_clientPanel.gameClient().singleplayerClient();
        if (!singlePlayerClient.tryMove(from, to)) { return; }
        m_boardDisplay->updateBoard(singlePlayerClient.board().getBoard());
    };

    m_component = buildComponent();
}

SingleplayerGameScreen::~SingleplayerGameScreen() {}

ftxui::Component SingleplayerGameScreen::getComponent() {
    return m_component;
}

void SingleplayerGameScreen::onEnter() {
    SingleplayerClient& singlePlayerClient = m_clientPanel.gameClient().singleplayerClient();
    m_boardDisplay->setFlipped(singlePlayerClient.playerColor() == COLOR::BLACK);
    m_boardDisplay->updateBoard(singlePlayerClient.board().getBoard());
    m_resultModalShown = false;
    m_resultSubscription = singlePlayerClient.resultRegistry().subscribe(std::bind_front(&SingleplayerGameScreen::onResult,this));
    m_clientPanel.setTickRate(std::chrono::milliseconds(100));
}

void SingleplayerGameScreen::onLeave() {
    if (m_resultSubscription != 0) {
        m_clientPanel.gameClient().singleplayerClient().resultRegistry().unsubscribe(m_resultSubscription);
        m_resultSubscription = 0;
    }
    m_clientPanel.gameClient().singleplayerClient().resign();
    auto result = m_clientPanel.gameClient().stopSingleplayer();
    if (!result) {
        m_clientPanel.pushModal(std::make_unique<ErrorModal>(m_clientPanel, result.message));
    }
    m_clientPanel.setTickRate(std::nullopt);
}

void SingleplayerGameScreen::onPause() {
    m_clientPanel.gameClient().singleplayerClient().pause();
    m_clientPanel.setTickRate(std::nullopt);
}

void SingleplayerGameScreen::onResume() {
    m_clientPanel.setTickRate(std::chrono::milliseconds(100));
    m_clientPanel.gameClient().singleplayerClient().resume();
}

void SingleplayerGameScreen::onLeaveRequest(const std::function<void()>& confirm) {
    if (m_clientPanel.gameClient().singleplayerClient().state() != SingleplayerState::RESULT) {
        m_clientPanel.pushModal(std::make_unique<ConfirmModal>(m_clientPanel, "Resign and return to menu?", [this, confirm]() {
            confirm();
        }));
    } else {
        confirm();
    }
}

ftxui::Component SingleplayerGameScreen::buildComponent() {
    auto withTick = ftxui::CatchEvent(m_boardDisplay, [this](ftxui::Event event) {
        if (event == TickEvent) {
            onTick();
            return true;
        }
        return false;
    });

    // Full screen renderer: board on the left, clocks on the right.
    return ftxui::Renderer(withTick, [this, withTick]() {
        auto& sp = m_clientPanel.gameClient().singleplayerClient();

        const bool whiteActive = (sp.currentTurn() == COLOR::WHITE);
        const bool blackActive = (sp.currentTurn() == COLOR::BLACK);

        auto whiteClock = renderClock(sp.whiteTimeRemaining(), whiteActive, "White");
        auto blackClock = renderClock(sp.blackTimeRemaining(), blackActive, "Black");

        const std::string turnText = whiteActive ? "White's Turn" : "Black's Turn";

        auto sidebar = ftxui::vbox({
            ftxui::filler(),
            blackClock,
            ftxui::filler(),
            ftxui::text(turnText) | ftxui::bold | ftxui::center,
            ftxui::filler(),
            whiteClock,
            ftxui::filler(),
        }) | ftxui::border | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 22);

        return ftxui::hbox({
            withTick->Render() | ftxui::flex,
            ftxui::text(" "),
            sidebar,
        });
    });
}

void SingleplayerGameScreen::onTick() {
    m_clientPanel.gameClient().singleplayerClient().checkTimeout();
}

ftxui::Element SingleplayerGameScreen::renderClock(std::chrono::milliseconds remaining, bool isActive, const std::string& label) const {
    std::string timeStr = formatHHMMSS(remaining);
    bool urgent = (remaining <= std::chrono::seconds{30}) && isActive;

    auto clockText = ftxui::text(timeStr)
        | ftxui::bold
        | ftxui::center;

    if (urgent) {
        clockText = clockText | ftxui::color(ftxui::Color::Red);
    } else if (isActive) {
        clockText = clockText | ftxui::color(ftxui::Color::Green);
    } else {
        clockText = clockText | ftxui::dim;
    }

    return ftxui::vbox({
        ftxui::text(label) | ftxui::center | ftxui::dim,
        clockText,
    }) | ftxui::border;
}

void SingleplayerGameScreen::onResult(const GameResult& result) {
    if (m_resultModalShown) { return; }
    m_resultModalShown = true;

    std::string title = "Game Over";
    std::string message;
    switch (result.winner) {
        case COLOR::WHITE:
            message = "White wins";
            break;
        case COLOR::BLACK:
            message = "Black wins";
            break;
        default:
            message = "Draw";
            break;
    }

    switch (result.reason) {
        case GameOverReason::TIMEOUT:
            message += " by timeout.";
            break;
        case GameOverReason::RESIGN:
            message += " by resignation.";
            break;
        case GameOverReason::CHECKMATE:
            message += " by checkmate.";
            break;
    }

    m_clientPanel.pushModal(std::make_unique<TwoButtonModal>(m_clientPanel,title,message,
        "Play Again", [this]() {
            m_clientPanel.navigateBack();
        },
        "Main Menu", [this]() {
            m_clientPanel.resetScreen(Screen::MainMenu);
        }
    ));
}

}
