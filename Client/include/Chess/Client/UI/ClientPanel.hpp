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

    void run();
    void quit();

    // Post Tick
    void setTickRate(std::optional<std::chrono::milliseconds> rate);

    // Screen Navigation
    void navigateTo(Screen screen);
    void resetTo(Screen screen);
    void navigateBack();

    void navigateToForce(Screen screen);
    void resetToForce(Screen screen);
    void navigateBackForce();

    [[nodiscard]] bool canNavigateBack() const;
    [[nodiscard]] Screen currentScreen() const;

    // Modal Controls
    void pushModal(std::unique_ptr<ModalInterface> modal);
    void popModal();
    void popAllModals();
    [[nodiscard]] bool hasModal() const;

    // Client Reference
    [[nodiscard]] GameClient& gameClient() { return m_gameClient; }
    [[nodiscard]] const GameClient& gameClient() const { return m_gameClient; }

private:
    void setActiveScreen(Screen screen);

    ftxui::Component buildMainComponent();

    void tickLoop();

    void onTick();

private:
    // Game Client
    GameClient& m_gameClient;

    // FTXUI Screen
    ftxui::ScreenInteractive m_screen;

    // Main Component
    ftxui::Component m_mainComponent;

    // Screen Sub-Classes
    std::map<Screen, std::unique_ptr<ScreenInterface>> m_screens;
    std::stack<Screen> m_screenHistory;
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
