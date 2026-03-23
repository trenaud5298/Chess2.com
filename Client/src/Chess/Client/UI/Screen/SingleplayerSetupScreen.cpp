/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Screen/SingleplayerSetupScreen.hpp>
#include <Chess/Client/UI/ClientPanel.hpp>
#include <Chess/Client/Runtime/GameClient.hpp>

// FTXUI Includes

// C++ Includes

namespace Chess {

const std::vector<std::string> SingleplayerSetupScreen::TIME_LABELS = {
    "1 min", "3 min", "5 min", "10 min", "15 min", "30 min"
};
const std::vector<std::chrono::seconds> SingleplayerSetupScreen::TIME_VALUES = {
    std::chrono::seconds{60},
    std::chrono::seconds{180},
    std::chrono::seconds{300},
    std::chrono::seconds{600},
    std::chrono::seconds{900},
    std::chrono::seconds{1800},
};

const std::vector<std::string> SingleplayerSetupScreen::INCREMENT_LABELS = {
    "None", "1 sec", "2 sec", "3 sec", "5 sec", "10 sec"
};
const std::vector<std::chrono::seconds> SingleplayerSetupScreen::INCREMENT_VALUES = {
    std::chrono::seconds{0},
    std::chrono::seconds{1},
    std::chrono::seconds{2},
    std::chrono::seconds{3},
    std::chrono::seconds{5},
    std::chrono::seconds{10},
};

const std::vector<std::string> SingleplayerSetupScreen::COLOR_LABELS = {
    "White", "Black", "Random"
};

SingleplayerSetupScreen::SingleplayerSetupScreen(ClientPanel& clientPanel) : ScreenInterface(clientPanel) {
    auto colorRadio = ftxui::Radiobox(&COLOR_LABELS, &m_colorIndex);
    auto timeRadio = ftxui::Radiobox(&TIME_LABELS, &m_timeIndex);
    auto incrementRadio = ftxui::Radiobox(&INCREMENT_LABELS, &m_incrementIndex);

    auto startButton = ftxui::Button("  Start Game  ", [this]() {
        SingleplayerConfig config{COLOR::EMPTY,TIME_VALUES[m_timeIndex],INCREMENT_VALUES[m_incrementIndex],};
        switch (m_colorIndex) {
            case 0: config.playerColor = COLOR::WHITE; break;
            case 1: config.playerColor = COLOR::BLACK; break;
            case 2: config.playerColor = (std::rand() % 2 == 0) ? COLOR::WHITE : COLOR::BLACK; break;
            default: config.playerColor = COLOR::WHITE; break;
        }
        m_clientPanel.gameClient().startSingleplayer(config);
        m_clientPanel.navigateTo(Screen::Singleplayer_Game);
    }, ftxui::ButtonOption::Simple());

    auto controls = ftxui::Container::Vertical({
        colorRadio,
        timeRadio,
        incrementRadio,
        startButton,
    });

    m_component = ftxui::Renderer(controls, [colorRadio, timeRadio, incrementRadio, startButton]() {
        return ftxui::vbox({
            ftxui::text("New Singleplayer Game") | ftxui::bold | ftxui::center,
            ftxui::separator(),
            ftxui::filler(),

            ftxui::hbox({
                ftxui::filler(),
                ftxui::vbox({
                    ftxui::text("Play as") | ftxui::bold,
                    ftxui::separator(),
                    colorRadio->Render(),
                }) | ftxui::border | ftxui::flex,
                ftxui::text("  "),
                ftxui::vbox({
                    ftxui::text("Time per side") | ftxui::bold,
                    ftxui::separator(),
                    timeRadio->Render(),
                }) | ftxui::border | ftxui::flex,
                 ftxui::text("  "),
                ftxui::vbox({
                    ftxui::text("Increment") | ftxui::bold,
                    ftxui::separator(),
                    incrementRadio->Render(),
                }) | ftxui::border | ftxui::flex,
                ftxui::filler(),
            }) | ftxui::flex,
            ftxui::filler(),
            ftxui::separator(),
            ftxui::hbox({
                ftxui::filler(),
                startButton->Render() | ftxui::bold,
                ftxui::filler(),
            }),
            ftxui::filler(),
        });
    });
}

SingleplayerSetupScreen::~SingleplayerSetupScreen() {}

ftxui::Component SingleplayerSetupScreen::getComponent() {
    return m_component;
}

void SingleplayerSetupScreen::onEnter() {
    m_colorIndex = 0;
    m_timeIndex = 2;
    m_incrementIndex = 0;
}

} // namespace Chess