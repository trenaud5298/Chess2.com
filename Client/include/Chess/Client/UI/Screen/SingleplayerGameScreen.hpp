#ifndef CHESS_CLIENT_UI_SINGLEPLAYERGAMESCREEN_HPP
#define CHESS_CLIENT_UI_SINGLEPLAYERGAMESCREEN_HPP


/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Screen/ScreenInterface.hpp>
#include <Chess/Client/Runtime/Callback/CallbackRegistry.hpp>

// ASIO Includes

// FTXUI Includes
#include <ftxui/component/component.hpp>

// C++ Includes
#include <cstdint>
#include <chrono>
#include <string>


namespace Chess {

class ClientPanel;
class ChessBoardDisplay;
class GameResult;

class SingleplayerGameScreen : public ScreenInterface {
public:
    explicit SingleplayerGameScreen(ClientPanel& clientPanel);
    ~SingleplayerGameScreen();
    ftxui::Component getComponent() override;

    void onEnter() override;
    void onLeave() override;

    void onPause() override;
    void onResume() override;

    void onTick() override;

    void onLeaveRequest(const std::function<void()>& confirm) override;

private:
    ftxui::Component buildComponent();
    ftxui::Element renderClock(std::chrono::milliseconds remaining, bool isActive, const std::string& label) const;
    void onResult(const GameResult& result);
private:
    ftxui::Component m_component;
    static constexpr Screen SCREEN_TYPE = Screen::Singleplayer_Game;

    std::shared_ptr<ChessBoardDisplay> m_boardDisplay;

    SubscriptionID m_resultSubscription{0};
    bool m_resultModalShown{false};
};

}
#endif
