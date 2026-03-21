/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/ClientPanel.hpp>
#include <Chess/Client/UI/MainMenuScreen.hpp>
#include <Chess/Client/UI/SettingsScreen.hpp>
#include <Chess/Client/UI/MultiplayerScreen.hpp>
#include <Chess/Client/UI/SinglePlayerScreen.hpp>

// ASIO Includes

// FTXUI Includes
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/loop.hpp>

// C++ Includes
#include <atomic>
#include <thread>

namespace Chess {

ClientPanel::ClientPanel(GameClient& gameClient) : m_gameClient(gameClient), m_screen(ftxui::ScreenInteractive::FullscreenAlternateScreen()) {
    m_screens[Screen::MainMenu] = std::make_unique<MainMenuScreen>(*this);
    m_screens[Screen::Singleplayer] = std::make_unique<SingleplayerScreen>(*this);
    m_screens[Screen::Multiplayer] = std::make_unique<MultiplayerScreen>(*this);
    m_screens[Screen::Settings] = std::make_unique<SettingsScreen>(*this);

    auto screenContainer = ftxui::Container::Tab({
        m_screens[Screen::MainMenu]->getComponent(),
        m_screens[Screen::Singleplayer]->getComponent(),
        m_screens[Screen::Multiplayer]->getComponent(),
        m_screens[Screen::Settings]->getComponent()
    }, &m_selectedIndex);

    auto mainEventCatcher = ftxui::CatchEvent(screenContainer, [this](ftxui::Event event) {
        if (event == TickEvent) {
            return true;
        }

        if (event == ftxui::Event::Escape && canNavigateBack()) {
            navigateBack();
            return true;
        }

        return false;
    });

    m_mainComponent = ftxui::Renderer(mainEventCatcher, [mainEventCatcher]() {
        return ftxui::window(ftxui::text("Chess Client") | ftxui::bold, mainEventCatcher->Render() | ftxui::flex);
    });
}

ClientPanel::~ClientPanel() {

}

void ClientPanel::run() {
    m_screen.Loop(m_mainComponent);
}

void ClientPanel::quit() {
    m_screen.ExitLoopClosure()();
}

void ClientPanel::navigateTo(Screen screen) {
    if (screen == m_selectedScreen) { return; }

    m_screens[m_selectedScreen]->onLeave();
    m_screenHistory.push(m_selectedScreen);
    setActiveScreen(screen);
}

void ClientPanel::navigateBack() {
    if (m_screenHistory.empty()) { return; }

    m_screens[m_selectedScreen]->onLeave();
    Screen previous = m_screenHistory.top();
    m_screenHistory.pop();
    setActiveScreen(previous);
}

bool ClientPanel::canNavigateBack() const {
    return !m_screenHistory.empty();
}

Screen ClientPanel::currentScreen() const {
    return m_selectedScreen;
}

void ClientPanel::setActiveScreen(Screen screen) {
    m_selectedScreen = screen;
    m_selectedIndex = static_cast<int>(screen);
    m_screens[screen]->onEnter();
}
}
