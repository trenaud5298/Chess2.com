#ifndef CHESS_CLIENT_UI_CLIENTPANEL_HPP
#define CHESS_CLIENT_UI_CLIENTPANEL_HPP


/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Screen/ScreenInterface.hpp>
#include <Chess/Client/UI/Modal/ModalInterface.hpp>
#include <Chess/Client/Runtime/GameClient.hpp>

// ASIO Includes

// FTXUI Includes
#include <ftxui/component/screen_interactive.hpp>

// C++ Includes
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <stack>
#include <thread>
#include <vector>


namespace Chess {

class GameClient;
inline const ftxui::Event TickEvent = ftxui::Event::Special("tick");


class ClientPanel {

public:
    explicit ClientPanel(GameClient& gameClient);
    ~ClientPanel();

    ClientPanel(const ClientPanel&) = delete;
    ClientPanel& operator=(const ClientPanel&) = delete;
    ClientPanel(ClientPanel&&) = delete;
    ClientPanel& operator=(ClientPanel&&) = delete;

    void run();
    void quit();

    // Modal Navigation
    void pushModal(std::unique_ptr<ModalInterface> modal);
    void popModal();
    void popAllModals();

    // Screen and Modal Query
    [[nodiscard]] Screen currentScreen() const {return m_selectedScreen;}
    [[nodiscard]] ScreenInterface& activeScreen() {return *m_screens[m_selectedScreen];}
    [[nodiscard]] bool hasModal() const {return !m_modalStack.empty();}
    [[nodiscard]] ModalInterface& activeModal() {return *m_modalStack.back();}

    // Post Tick
    void setTickRate(std::optional<std::chrono::milliseconds> rate);

    // Client Reference
    [[nodiscard]] GameClient& gameClient() { return m_gameClient; }
    [[nodiscard]] const GameClient& gameClient() const { return m_gameClient; }

private:
    // Runtime Callbacks Helpers
    void subscribeToClientCallbacks();
    void unsubscribeFromClientCallbacks();
    void handleClientStateChanged(ClientState newState);

    // Screen Helpers
    void setScreen(Screen screen);
    void resetScreen(Screen screen);
    [[nodiscard]] Screen screenForState(ClientState state) const;


    // FTXUI Helpers
    void onTick();
    void tickLoop();
    void cleanupAfterLoop();
    ftxui::Component buildMainComponent();

private:
    // Game Client
    GameClient& m_gameClient;
    SubscriptionID m_clientStateSubscription{0};

    // FTXUI
    ftxui::ScreenInteractive m_screen;
    ftxui::Component m_mainComponent;

    // Screen Sub-Classes
    std::map<Screen, std::unique_ptr<ScreenInterface>> m_screens;
    Screen m_selectedScreen{Screen::MainMenu};
    int m_selectedIndex{static_cast<int>(Screen::MainMenu)};

    // Modals
    std::vector<std::unique_ptr<ModalInterface>> m_modalStack;
    std::vector<std::unique_ptr<ModalInterface>> m_modalGraveyard;
    bool m_showModal{false};

    // Tick
    static constexpr std::chrono::milliseconds MIN_TICK_RATE{ 500 };
    std::thread m_tickThread;
    std::mutex m_tickMutex;
    std::condition_variable m_tickCondition;
    std::atomic<bool> m_tickRunning{false};
    std::optional<std::chrono::milliseconds> m_tickRate{std::nullopt};
    std::uint64_t m_tickRevision{0};
};



}



#endif
