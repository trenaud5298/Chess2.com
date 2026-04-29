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
#include <CHess/Client/UI/Modal/PromotionModal.hpp>

// FTXUI Includes

// C++ Includes

namespace Chess {

namespace {

std::uint8_t squareFromPos(const Pos& pos) {
    return static_cast<std::uint8_t>(pos[0] * 8 + pos[1]);
}


}

SingleplayerGameScreen::SingleplayerGameScreen(ClientPanel& clientPanel) : ScreenInterface(clientPanel), m_boardDisplay(std::make_shared<ChessBoardDisplay>()) {
    m_boardDisplay->onMove = [this](Pos from, Pos to, bool promotionRequired) {
        SingleplayerView view = m_clientPanel.gameClient().singleplayerView();
        bool isWhite = view.currentTurn == COLOR::WHITE;

        if (promotionRequired) {
            m_clientPanel.pushModal(std::make_unique<PromotionModal>(m_clientPanel, isWhite, [this, from, to](PromotionPiece promotion) {
                ClientStatus result = m_clientPanel.gameClient().submitSingleplayerMove(squareFromPos(from), squareFromPos(to), promotion);
                if (!m_clientPanel.handleStatus(result, "Failed To Make Move")) {
                    return;
                }
                SingleplayerView updated = m_clientPanel.gameClient().singleplayerView();
                m_boardDisplay->updateBoard(updated.board->getBoard());
            }));
        } else {
            ClientStatus result = m_clientPanel.gameClient().submitSingleplayerMove(squareFromPos(from), squareFromPos(to), PromotionPiece::None);
            if (!m_clientPanel.handleStatus(result, "Failed To Make Move")) {
                return;
            }
            SingleplayerView updated = m_clientPanel.gameClient().singleplayerView();
            m_boardDisplay->updateBoard(updated.board->getBoard());
        }
    };

    m_component = buildComponent();
}

SingleplayerGameScreen::~SingleplayerGameScreen() {}

ftxui::Component SingleplayerGameScreen::getComponent() {
    return m_component;
}

void SingleplayerGameScreen::onEnter() {
    SingleplayerView view = m_clientPanel.gameClient().singleplayerView();

    m_boardDisplay->setTheme(m_clientPanel.gameClient().persistenceManager().settings().getBoardTheme());

    m_boardDisplay->setFlipped(view.playerColor == COLOR::BLACK);
    m_boardDisplay->updateBoard(view.board->getBoard());
    m_clientPanel.setTickRate(std::chrono::milliseconds(100));
}

void SingleplayerGameScreen::onLeave() {
    m_clientPanel.setTickRate(std::nullopt);
}

void SingleplayerGameScreen::onPause() {
    m_clientPanel.handleStatus(
        m_clientPanel.gameClient().pauseSingleplayer(),
        "Unable to pause singleplayer"
    );
    m_clientPanel.setTickRate(std::nullopt);
}

void SingleplayerGameScreen::onResume() {
    m_boardDisplay->setTheme(m_clientPanel.gameClient().persistenceManager().settings().getBoardTheme());

    m_clientPanel.setTickRate(std::chrono::milliseconds(100));
    m_clientPanel.handleStatus(
        m_clientPanel.gameClient().resumeSingleplayer(),
        "Unable to resume singleplayer"
    );
}

void SingleplayerGameScreen::requestExit() {
    m_clientPanel.pushModal(std::make_unique<ConfirmModal>(m_clientPanel, "Are you sure you would like to quit?",[this]() {
        m_clientPanel.handleStatus(
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
