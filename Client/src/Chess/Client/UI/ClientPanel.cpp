/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/ClientPanel.hpp>

#include <Chess/Client/UI/Screen/MainMenuScreen.hpp>
#include <Chess/Client/UI/Screen/SingleplayerSetupScreen.hpp>
#include <Chess/Client/UI/Screen/SingleplayerGameScreen.hpp>
#include <Chess/Client/UI/Screen/SingleplayerResultScreen.hpp>
#include <Chess/Client/UI/Screen/MultiplayerSelectScreen.hpp>
#include <Chess/Client/UI/Screen/MultiplayerLobbyScreen.hpp>
#include <Chess/Client/UI/Screen/MultiplayerGameScreen.hpp>
#include <Chess/Client/UI/Screen/MultiplayerResultScreen.hpp>

#include <Chess/Client/UI/Modal/ModalProxy.hpp>
#include <Chess/Client/UI/Modal/SettingsModal.hpp>

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
    m_screens[Screen::Singleplayer_Setup] = std::make_unique<SingleplayerSetupScreen>(*this);
    m_screens[Screen::Singleplayer_Game] = std::make_unique<SingleplayerGameScreen>(*this);
    m_screens[Screen::Singleplayer_Result] = std::make_unique<SingleplayerResultScreen>(*this);
    m_screens[Screen::Multiplayer_Select] = std::make_unique<MultiplayerSelectScreen>(*this);
    m_screens[Screen::Multiplayer_Lobby] = std::make_unique<MultiplayerLobbyScreen>(*this);
    m_screens[Screen::Multiplayer_Game] = std::make_unique<MultiplayerGameScreen>(*this);
    m_screens[Screen::Multiplayer_Result] = std::make_unique<MultiplayerResultScreen>(*this);


    m_mainComponent = buildMainComponent();
}

ClientPanel::~ClientPanel() {

}

void ClientPanel::run() {
    m_screen.Loop(m_mainComponent);
}

void ClientPanel::quit() {
    m_screen.ExitLoopClosure()();
}

void ClientPanel::tick() {
    m_screen.PostEvent(TickEvent);
}

void ClientPanel::navigateTo(Screen screen) {
    if (screen == m_selectedScreen) { return; }
    m_screens[m_selectedScreen]->onLeaveRequest([this, screen]() {
        m_screens[m_selectedScreen]->onLeave();
        m_screenHistory.push(m_selectedScreen);
        setActiveScreen(screen);
    });
}

void ClientPanel::navigateBack() {
    if (m_screenHistory.empty()) { return; }
    m_screens[m_selectedScreen]->onLeaveRequest([this]() {
        m_screens[m_selectedScreen]->onLeave();
        Screen previous = m_screenHistory.top();
        m_screenHistory.pop();
        setActiveScreen(previous);
    });
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

void ClientPanel::pushModal(std::unique_ptr<ModalInterface> modal) {
    m_screens[m_selectedScreen]->onPause();
    modal->onEnter();
    m_modalStack.push_back(std::move(modal));
    m_showModal = true;
}

void ClientPanel::popModal() {
    if (m_modalStack.empty()) { return; }

    m_modalStack.back()->onLeave();
    m_modalStack.pop_back();

    if (m_modalStack.empty()) {
        m_showModal = false;
        m_screens[m_selectedScreen]->onResume();
    }
}

void ClientPanel::popAllModals() {
    if (m_modalStack.empty()) { return; }

    while (!m_modalStack.empty()) {
        m_modalStack.back()->onLeave();
        m_modalStack.pop_back();
    }
    m_showModal = false;
    m_screens[m_selectedScreen]->onResume();
}

bool ClientPanel::hasModal() const {
    return !m_modalStack.empty();
}

ftxui::Component ClientPanel::buildMainComponent() {
    auto screenContainer = ftxui::Container::Tab({
        m_screens[Screen::MainMenu]->getComponent(),
        m_screens[Screen::Singleplayer_Setup]->getComponent(),
        m_screens[Screen::Singleplayer_Game]->getComponent(),
        m_screens[Screen::Singleplayer_Result]->getComponent(),
        m_screens[Screen::Multiplayer_Select]->getComponent(),
        m_screens[Screen::Multiplayer_Lobby]->getComponent(),
        m_screens[Screen::Multiplayer_Game]->getComponent(),
        m_screens[Screen::Multiplayer_Result]->getComponent()
    }, &m_selectedIndex);

    // Status Bar
    auto backButtonOption = ftxui::ButtonOption::Simple();
    backButtonOption.transform = [this](const ftxui::EntryState& s) {
        auto label = ftxui::text(s.label);
        if (!canNavigateBack()) { label |= ftxui::dim; }
        else if (s.focused) { label |= ftxui::bold; }
        return label;
    };
    auto backButton = ftxui::Button("← Back    ", [this]() {
        if (canNavigateBack()) { navigateBack(); }
    }, backButtonOption);

    auto settingsButtonOption = ftxui::ButtonOption::Simple();
    settingsButtonOption.transform = [](const ftxui::EntryState& s) {
        auto label = ftxui::text(s.label);
        if (s.focused) label = label | ftxui::bold;
        return label;
    };
    auto settingsButton = ftxui::Button("⚙ Settings", [this]() {
        pushModal(std::make_unique<SettingsModal>(*this));
    }, settingsButtonOption);

    auto statusBarButtons = ftxui::Container::Horizontal({
        backButton,
        settingsButton,
    });

    auto statusBar = ftxui::Renderer(statusBarButtons, [this, backButton, settingsButton]() {
        return ftxui::hbox({
            backButton->Render(),
            ftxui::filler(),
            ftxui::text(toString(currentScreen())) | ftxui::bold | ftxui::center,
            ftxui::filler(),
            hasModal() ? ftxui::text("          ") : settingsButton->Render()
        });
    });

    auto baseLayout = ftxui::Container::Vertical({statusBar, screenContainer});
    auto baseRenderer = ftxui::Renderer(baseLayout, [this, statusBar, screenContainer]() {
        return ftxui::vbox({
            statusBar->Render(),
            ftxui::separator(),
            screenContainer->Render() | ftxui::flex
        }) | (m_showModal ? ftxui::dim : ftxui::nothing);
    });

    auto modalProxy = ftxui::Make<ModalProxy>(m_modalStack);

    auto modalLayout = ftxui::Modal(baseRenderer, modalProxy, &m_showModal);

    auto mainEventCatcher = ftxui::CatchEvent(modalLayout, [this](ftxui::Event event) {
        if (!hasModal() && event == ftxui::Event::Escape && canNavigateBack()) {
            navigateBack();
            return true;
        }

        return false;
    });

    return ftxui::Renderer(mainEventCatcher, [mainEventCatcher]() {
        return ftxui::window(ftxui::text("Chess2.com Client") | ftxui::bold, mainEventCatcher->Render() | ftxui::flex);
    });
}

}
