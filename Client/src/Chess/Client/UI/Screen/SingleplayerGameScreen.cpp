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
        ClientCommandResult result = m_clientPanel.gameClient().submitSingleplayerMove(from, to);
        if (!m_clientPanel.handleCommandResult(result, "Failed To Make Move")) {
            return;
        }
        m_boardDisplay->updateBoard(m_clientPanel.gameClient().singleplayerView().board->getBoard());
    };

    m_component = buildComponent();
}

SingleplayerGameScreen::~SingleplayerGameScreen() {}

ftxui::Component SingleplayerGameScreen::getComponent() {
    return m_component;
}

void SingleplayerGameScreen::onEnter() {
    SingleplayerView view = m_clientPanel.gameClient().singleplayerView();
    m_boardDisplay->setFlipped(view.playerColor == COLOR::BLACK);
    m_boardDisplay->updateBoard(view.board->getBoard());
    m_clientPanel.setTickRate(std::chrono::milliseconds(100));
}

void SingleplayerGameScreen::onLeave() {
    m_clientPanel.setTickRate(std::nullopt);
}

void SingleplayerGameScreen::onPause() {
    m_clientPanel.handleCommandResult(
        m_clientPanel.gameClient().pauseSingleplayer(),
        "Unable to pause singleplayer"
    );
    m_clientPanel.setTickRate(std::nullopt);
}

void SingleplayerGameScreen::onResume() {
    m_clientPanel.setTickRate(std::chrono::milliseconds(100));
    m_clientPanel.handleCommandResult(
        m_clientPanel.gameClient().resumeSingleplayer(),
        "Unable to resume singleplayer"
    );
}

void SingleplayerGameScreen::requestExit() {
    m_clientPanel.pushModal(std::make_unique<ConfirmModal>(m_clientPanel, "Are you sure you would like to quit?",[this]() {
        m_clientPanel.handleCommandResult(
            m_clientPanel.gameClient().returnToIdle(),
            "Unable to return to main menu"
        );
    }));
}

ftxui::Component SingleplayerGameScreen::buildComponent() {
    // Full screen renderer: board on the left, clocks on the right.
    return ftxui::Renderer(m_boardDisplay, [this]() {
        SingleplayerView view = m_clientPanel.gameClient().singleplayerView();

        bool whiteActive = (view.currentTurn == COLOR::WHITE);
        bool blackActive = (view.currentTurn == COLOR::BLACK);

        auto whiteClock = renderClock(view.whiteTimeRemaining, whiteActive, "White");
        auto blackClock = renderClock(view.blackTimeRemaining, blackActive, "Black");

        std::string turnText = whiteActive ? "White's Turn" : "Black's Turn";

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
            m_boardDisplay->Render() | ftxui::flex,
            ftxui::text(" "),
            sidebar,
        });
    });
}

void SingleplayerGameScreen::onTick() {
    m_clientPanel.handleCommandResult(
        m_clientPanel.gameClient().tick(),
        "Tick Failed"
    );
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

}
