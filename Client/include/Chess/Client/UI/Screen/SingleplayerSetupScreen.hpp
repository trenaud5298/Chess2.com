#ifndef CHESS_CLIENT_UI_SINGLEPLAYERSETUPSCREEN_HPP
#define CHESS_CLIENT_UI_SINGLEPLAYERSETUPSCREEN_HPP


/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Screen/ScreenInterface.hpp>

// ASIO Includes

// FTXUI Includes
#include <ftxui/component/component.hpp>

// C++ Includes
#include <cstdint>
#include <vector>
#include <string>

namespace Chess {

class ClientPanel;

class SingleplayerSetupScreen : public ScreenInterface {
public:
    explicit SingleplayerSetupScreen(ClientPanel& clientPanel);
    ~SingleplayerSetupScreen();
    ftxui::Component getComponent() override;
    void onEnter() override;

    bool canRequestExit() const override {return true;}
    std::string exitLabel() const override {return "Main Menu";}
    void requestExit() override;

private:
    ;
    ftxui::Component m_component;
    static constexpr Screen SCREEN_TYPE = Screen::Singleplayer_Setup;

    static const std::vector<std::string> TIME_LABELS;
    static const std::vector<std::chrono::seconds> TIME_VALUES;

    static const std::vector<std::string> INCREMENT_LABELS;
    static const std::vector<std::chrono::seconds> INCREMENT_VALUES;

    static const std::vector<std::string> COLOR_LABELS;

    int m_colorIndex{0};       // 0 = White, 1 = Black, 2 = Random
    int m_timeIndex{2};        // default: 5 min
    int m_incrementIndex{0};   // default: 0 sec
};
}
#endif