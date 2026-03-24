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

// FTXUI Includes

// C++ Includes

namespace Chess {

SingleplayerGameScreen::SingleplayerGameScreen(ClientPanel& clientPanel) : ScreenInterface(clientPanel), m_boardDisplay(std::make_shared<ChessBoardDisplay>()) {
    m_boardDisplay->onMove = [this](ID from, Pos to) {
        SingleplayerClient& singlePlayerClient = m_clientPanel.gameClient().singleplayerClient();
        if (!singlePlayerClient.tryMove(from, to)) { return; }

        m_boardDisplay->updateBoard(singlePlayerClient.board().getBoard());
        if (singlePlayerClient.state() == SingleplayerState::RESULT) {
            m_clientPanel.navigateTo(Screen::Singleplayer_Result);
        }
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

    m_tickThread = std::jthread([this](std::stop_token stop) {
        while (!stop.stop_requested()) {
            m_clientPanel.tick();
            std::this_thread::sleep_for(TICK_INTERVAL);
        }
    });
}

void SingleplayerGameScreen::onLeave() {
    m_tickThread.request_stop();
    m_clientPanel.gameClient().stopSingleplayer();
}

void SingleplayerGameScreen::onPause() {
    m_clientPanel.gameClient().singleplayerClient().pause();
    m_tickThread.request_stop();
}

void SingleplayerGameScreen::onResume() {
    m_tickThread = std::jthread([this](std::stop_token stop) {
        while (!stop.stop_requested()) {
            m_clientPanel.tick();
            std::this_thread::sleep_for(TICK_INTERVAL);
        }
    });
    m_clientPanel.gameClient().singleplayerClient().resume();
}

void SingleplayerGameScreen::onLeaveRequest(std::function<void()> confirm) {
    if (m_clientPanel.gameClient().singleplayerClient().state() != SingleplayerState::RESULT) {
        m_clientPanel.pushModal(std::make_unique<ConfirmModal>(m_clientPanel, "Resign and return to menu?",
            confirm
        ));
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
    SingleplayerClient& singlePlayerClient = m_clientPanel.gameClient().singleplayerClient();
    singlePlayerClient.checkTimeout();

    if (singlePlayerClient.state() == SingleplayerState::RESULT) {
        m_clientPanel.navigateTo(Screen::Singleplayer_Result);
    }
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

} // namespace Chess