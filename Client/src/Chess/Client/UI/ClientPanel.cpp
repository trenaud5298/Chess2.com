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
#include <Chess/Client/UI/Modal/ErrorModal.hpp>

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

    subscribeToClientCallbacks();
    m_selectedScreen = screenForState(m_gameClient.state());
    m_selectedIndex = static_cast<int>(m_selectedScreen);

    m_mainComponent = buildMainComponent();

    activeScreen().onEnter();

    m_tickRunning = true;
    m_tickThread = std::thread(&ClientPanel::tickLoop, this);
}

ClientPanel::~ClientPanel() {
    m_tickRunning = false;
    m_tickCondition.notify_one();
    if (m_tickThread.joinable()) { m_tickThread.join(); }
    cleanupAfterLoop();
    unsubscribeFromClientCallbacks();
}

void ClientPanel::run() {
    ftxui::Loop loop(&m_screen, m_mainComponent);
    while (!loop.HasQuitted()) {
        loop.RunOnceBlocking();
        m_modalGraveyard.clear();
    }
}

void ClientPanel::quit() {
    m_screen.ExitLoopClosure()();
}


void ClientPanel::setTickRate(std::optional<std::chrono::milliseconds> rate) {
    // Enforce Minimum Tick Interval (>=1ms)
    if (rate && *rate <= std::chrono::milliseconds::zero()) {
        rate = std::chrono::milliseconds{1};
    }
    {
        std::lock_guard lock(m_tickMutex);
        m_tickRate = rate;
        ++m_tickRevision;
    }
    m_tickCondition.notify_one();
}

void ClientPanel::pushModal(std::unique_ptr<ModalInterface> modal) {
    if (!hasModal()) {
        activeScreen().onPause();
    }
    modal->onEnter();
    m_modalStack.push_back(std::move(modal));
    m_showModal = true;
}

void ClientPanel::popModal() {
    if (m_modalStack.empty()) { return; }

    m_modalStack.back()->onLeave();
    m_modalGraveyard.push_back(std::move(m_modalStack.back()));
    m_modalStack.pop_back();

    if (m_modalStack.empty()) {
        m_showModal = false;
        activeScreen().onResume();
    }
}

void ClientPanel::popAllModals() {
    if (m_modalStack.empty()) { return; }

    while (!m_modalStack.empty()) {
        m_modalStack.back()->onLeave();
        m_modalGraveyard.push_back(std::move(m_modalStack.back()));
        m_modalStack.pop_back();
    }
    m_showModal = false;
    activeScreen().onResume();
}


bool ClientPanel::handleStatus(const ClientStatus& status, std::string_view action, ResultPolicy policy) {
    if (status) {
        return true;
    }

    ResultPolicy effectivePolicy = policy;
    if (policy == ResultPolicy::Auto) {
        effectivePolicy = DefaultResultPolicy(status);
    }

    if (effectivePolicy == ResultPolicy::Silent) {
        return false;
    }

    std::string message = std::string(action);
    if (!message.empty() && !status.message.empty()) {
        message += ": ";
    }
    message += status.message.empty() ? "Unknown error" : status.message;
    switch (status.severity) {
        case Severity::Debug:
            m_gameClient.loggingManager().log(LogEntry::Debug(message));
            break;
        case Severity::Info:
            m_gameClient.loggingManager().log(LogEntry::Info(message));
            break;
        case Severity::Warning:
            m_gameClient.loggingManager().log(LogEntry::Warning(message));
            break;
        case Severity::Error:
        case Severity::Fatal:
            m_gameClient.loggingManager().log(LogEntry::Error(message));
            break;
    }

    if (status.isFatal()) {
        return false;
    }

    if (effectivePolicy == ResultPolicy::Modal) {
        pushModal(std::make_unique<ErrorModal>(*this, message));
    }
    return false;
}


// Helpers

void ClientPanel::subscribeToClientCallbacks() {
    m_clientStateSubscription = m_gameClient.stateRegistry().subscribe([this](ClientState newState) {
        m_screen.PostEvent(StateChangeEvent);
    });
    m_clientEventSubscription = m_gameClient.eventRegistry().subscribe([this](ClientEvent event) {
        if (event.status.severity == Severity::Error) {
            std::string message =
                "[" + std::string(toString(event.source)) + "]" +
                "[" + std::string(toString(event.type)) + "]" +
                "[" + std::string(toString(event.status.severity)) + "]" +
                "[" + std::string(toString(event.kind)) + "]";

            if (!event.message().empty()) {
                message += "\n\n" + event.message();
            }

            pushModal(std::make_unique<ErrorModal>(*this, std::move(message)));
        }
    });
}

void ClientPanel::unsubscribeFromClientCallbacks() {
    if (m_clientStateSubscription == 0) { return; }
    m_gameClient.stateRegistry().unsubscribe(m_clientStateSubscription);
    m_clientStateSubscription = 0;

    if (m_clientEventSubscription == 0) { return; }
    m_gameClient.eventRegistry().unsubscribe(m_clientEventSubscription);
    m_clientEventSubscription = 0;
}

void ClientPanel::handleClientStateChanged() {
    ClientState newState = m_gameClient.state();

    if (newState == ClientState::Error) {
        auto fatal = m_gameClient.consumeFatalError();

        while (!m_modalStack.empty()) {
            m_modalStack.back()->onLeave();
            m_modalGraveyard.push_back(std::move(m_modalStack.back()));
            m_modalStack.pop_back();
        }
        m_showModal = false;

        m_gameClient.recoverFromFatalError();
        setScreen(Screen::MainMenu);

        std::string message = "Unexpected client error";
        if (fatal && !fatal->message.empty()) {
            message = fatal->message;
        }

        pushModal(std::make_unique<ErrorModal>(*this, message));
        return;
    }

    setScreen(screenForState(newState));

}

void ClientPanel::setScreen(Screen screen) {
    if (screen == currentScreen()) {return;}

    activeScreen().onLeave();

    m_selectedScreen = screen;
    m_selectedIndex = static_cast<int>(screen);

    activeScreen().onEnter();
}

void ClientPanel::resetScreen(Screen screen) {
    popAllModals();
    setScreen(screen);
}

Screen ClientPanel::screenForState(ClientState state) const {
    switch (state) {
        case ClientState::Idle:
            return Screen::MainMenu;
        case ClientState::SingleplayerSetup:
            return Screen::Singleplayer_Setup;
        case ClientState::SingleplayerInGame:
            return Screen::Singleplayer_Game;
        case ClientState::SingleplayerResult:
            return Screen::Singleplayer_Result;
        case ClientState::MultiplayerSetup:
        case ClientState::MultiplayerConnecting:
            return Screen::Multiplayer_Select;
        case ClientState::MultiplayerLobby:
            return Screen::Multiplayer_Lobby;
        case ClientState::MultiplayerInGame:
        case ClientState::MultiplayerResult:
            return Screen::Multiplayer_Game;
        case ClientState::Error:
            return Screen::MainMenu;
    }

    return Screen::MainMenu;
}

void ClientPanel::onTick() {
    handleStatus(m_gameClient.tick(), "Tick GameClient");
    if (hasModal()) {
        m_modalStack.back()->onTick();
    } else {
        m_screens[m_selectedScreen]->onTick();
    }
}

void ClientPanel::tickLoop() {
    std::unique_lock lock(m_tickMutex);
    while (m_tickRunning) {
        m_tickCondition.wait(lock, [this]() {
            return !m_tickRunning || m_tickRate.has_value();
        });

        if (!m_tickRunning) {
            break;
        }

        std::chrono::milliseconds interval = *m_tickRate;
        std::uint64_t revision = m_tickRevision;

        bool interrupted = m_tickCondition.wait_for(lock, interval, [this, revision]() {
            return !m_tickRunning || !m_tickRate.has_value() || m_tickRevision != revision;
        });

        if (!m_tickRunning) {
            break;
        }

        if (interrupted) {
            continue;
        }

        lock.unlock();
        m_screen.PostEvent(TickEvent);
        lock.lock();
    }
}

void ClientPanel::cleanupAfterLoop() {
    popAllModals();
    activeScreen().onLeave();
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
    auto exitButtonOption = ftxui::ButtonOption::Simple();
    exitButtonOption.transform = [this](const ftxui::EntryState& state) {
        auto label = ftxui::text(activeScreen().exitLabel());
        if (!activeScreen().canRequestExit()) {label |= ftxui::dim;}
        else if (state.focused) {label |= ftxui::bold;}
        return label;
    };
    auto exitButton = ftxui::Button("", [this]() {
        if (!hasModal() && activeScreen().canRequestExit()) {
            activeScreen().requestExit();
        }
    }, exitButtonOption);

    auto settingsButtonOption = ftxui::ButtonOption::Simple();
    settingsButtonOption.transform = [](const ftxui::EntryState& state) {
        auto label = ftxui::text("Settings");
        if (state.focused) {label |= ftxui::bold;}
        return label;
    };
    auto settingsButton = ftxui::Button("", [this]() {
        pushModal(std::make_unique<SettingsModal>(*this));
    }, settingsButtonOption);

    auto statusBarButtons = ftxui::Container::Horizontal({
        exitButton,
        settingsButton,
    });

    auto statusBar = ftxui::Renderer(statusBarButtons, [this, exitButton, settingsButton]() {
        return ftxui::hbox({
            exitButton->Render(),
            ftxui::filler(),
            ftxui::text(toString(currentScreen())) | ftxui::bold | ftxui::center,
            ftxui::filler(),
            hasModal() ? ftxui::text("        ") : settingsButton->Render()
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
        if (event == TickEvent) {
            onTick();
            return true;
        }

        if (event == StateChangeEvent) {
            handleClientStateChanged();
            return true;
        }

        if (event == ftxui::Event::Escape) {
            if (hasModal()) {
                if (activeModal().canRequestDismiss()) {
                    activeModal().requestDismiss();
                }
                return true;
            } else if (activeScreen().canRequestExit()) {
                activeScreen().requestExit();
                return true;
            }
        }
        return false;
    });

    return ftxui::Renderer(mainEventCatcher, [mainEventCatcher]() {
        return ftxui::window(ftxui::text("Chess2.com Client") | ftxui::bold, mainEventCatcher->Render() | ftxui::flex);
    });
}



}
