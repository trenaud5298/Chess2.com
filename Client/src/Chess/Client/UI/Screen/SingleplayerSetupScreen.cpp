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
#include <Chess/Client/UI/Modal/ErrorModal.hpp>

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
        m_clientPanel.handleStatus(
            m_clientPanel.gameClient().startSingleplayer(config),
            "Unable to start singleplayer"
        );
    }, ftxui::ButtonOption::Simple());

    auto optionMenus = ftxui::Container::Horizontal({
        colorRadio,
        timeRadio,
        incrementRadio
    });

    auto layout = ftxui::Container::Vertical({
        optionMenus,
        startButton,
    });


    m_component = ftxui::Renderer(layout, [this, colorRadio, timeRadio, incrementRadio, startButton]() {
        auto colorMenu = ftxui::vbox({
            ftxui::text("Play as") | ftxui::bold,
            ftxui::separator(),
            colorRadio->Render(),
        }) | ftxui::border | ftxui::flex;

        auto timeMenu = ftxui::vbox({
            ftxui::text("Time per side") | ftxui::bold,
            ftxui::separator(),
            timeRadio->Render(),
        }) | ftxui::border | ftxui::flex;

        auto incrementMenu = ftxui::vbox({
            ftxui::text("Increment") | ftxui::bold,
            ftxui::separator(),
            incrementRadio->Render(),
        }) | ftxui::border | ftxui::flex;

        auto menus = ftxui::hbox({
            colorMenu,
            timeMenu,
            incrementMenu
        });

        COLOR col;
        switch (m_colorIndex) {
            case 0: col = COLOR::WHITE; break;
            case 1: col = COLOR::BLACK; break;
            case 2: col = (std::rand() % 2 == 0) ? COLOR::WHITE : COLOR::BLACK; break;
            default: col = COLOR::WHITE; break;
        }

        auto debugInfo = ftxui::hbox({
            ftxui::filler(),
            ftxui::vbox({
                ftxui::text("Color Index: " + std::to_string(m_colorIndex)),
                ftxui::text("Color Value: " + std::to_string((int)col))
            }),
            ftxui::filler(),
            ftxui::vbox({
                ftxui::text("Time Index: " + std::to_string(m_timeIndex)),
                ftxui::text("Time Value: " + std::to_string(TIME_VALUES[m_timeIndex].count()) + " s")
            }),
            ftxui::filler(),
            ftxui::vbox({
                ftxui::text("Increment Index: " + std::to_string(m_incrementIndex)),
                ftxui::text("Increment Value: " + std::to_string(INCREMENT_VALUES[m_incrementIndex].count()) + " s"),
            }),
            ftxui::filler()
        });

        return ftxui::vbox({
            ftxui::filler(),
            menus,
            ftxui::filler(),
            startButton->Render() | ftxui::center,
            ftxui::filler(),
            debugInfo,
            ftxui::filler()
        }) | ftxui::frame;
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

void SingleplayerSetupScreen::requestExit() {
    m_clientPanel.handleStatus(
        m_clientPanel.gameClient().returnToIdle(),
        "Unable to return to main menu"
    );
}
} // namespace Chess