/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Screen/SingleplayerResultScreen.hpp>
#include <Chess/Client/UI/ClientPanel.hpp>
#include <Chess/Client/Runtime/GameClient.hpp>

// FTXUI Includes

// C++ Includes


namespace {

std::string ResultTitle(const Chess::GameResult& result) {
    switch (result.winner) {
        case COLOR::WHITE: return "White Wins";
        case COLOR::BLACK: return "Black Wins";
        case COLOR::EMPTY: return "Draw";
    }
    return "Game Over";
}


std::string ResultReason(const Chess::GameResult& result) {
    switch (result.reason) {
        case Chess::GameOverReason::CHECKMATE: return "By checkmate";
        case Chess::GameOverReason::TIMEOUT: return "By timeout";
        case Chess::GameOverReason::RESIGN: return "By resignation";
    }
    return "";
}

}

namespace Chess {

SingleplayerResultScreen::SingleplayerResultScreen(ClientPanel& clientPanel)
: ScreenInterface(clientPanel) {

    ftxui::ButtonOption singleplayerResultButtonOption = ftxui::ButtonOption::Animated();
    auto originalTransform = singleplayerResultButtonOption.transform;
    singleplayerResultButtonOption.transform = [originalTransform](const ftxui::EntryState& state) {
        return originalTransform(state) | ftxui::center | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 20);
    };

    auto playAgainButton = ftxui::Button("Play Again", [this]() {
        m_clientPanel.handleCommandResult(
            m_clientPanel.gameClient().restartSingleplayer(),
            "Unable to restart singleplayer"
        );
    }, singleplayerResultButtonOption);

    auto mainMenuButton = ftxui::Button("Main Menu", [this]() {
        requestExit();
    }, singleplayerResultButtonOption);

    auto buttons = ftxui::Container::Horizontal({playAgainButton, mainMenuButton});

    m_component = ftxui::Renderer(buttons, [this, playAgainButton, mainMenuButton]() {
        return ftxui::vbox({
            ftxui::filler(),
            ftxui::text(m_title) | ftxui::bold | ftxui::center,
            ftxui::text(m_description) | ftxui::dim | ftxui::center,
            ftxui::separatorEmpty(),
            ftxui::hbox({
                playAgainButton->Render(),
                ftxui::filler() | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 4),
                mainMenuButton->Render()
            }) | ftxui::center,
            ftxui::filler()
        });
    });
}

SingleplayerResultScreen::~SingleplayerResultScreen() {}

void SingleplayerResultScreen::onEnter() {
    SingleplayerView view = m_clientPanel.gameClient().singleplayerView();
    if (!view.result) {
        m_clientPanel.gameClient().loggingManager().log(LogEntry::Error("Transitioned To Singleplayer Result Screen But Game Result Not Available"));
        m_clientPanel.handleCommandResult(
            m_clientPanel.gameClient().returnToIdle(),
            "Unable to recover from missing singleplayer result"
        );
        return;
    }

    m_title = ResultTitle(*view.result);
    m_description = ResultReason(*view.result);

}

void SingleplayerResultScreen::requestExit() {
    m_clientPanel.handleCommandResult(
        m_clientPanel.gameClient().returnToIdle(),
        "Unable to return to main menu"
    );
}


ftxui::Component SingleplayerResultScreen::getComponent() {
    return m_component;
}



} // namespace Chess