#ifndef CHESS_CLIENT_UI_MULTIPLAYERRESULTSCREEN_HPP
#define CHESS_CLIENT_UI_MULTIPLAYERRESULTSCREEN_HPP


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

namespace Chess {

class ClientPanel;

class MultiplayerResultScreen : public ScreenInterface {
public:
    explicit MultiplayerResultScreen(ClientPanel& clientPanel);
    ~MultiplayerResultScreen();

    void onEnter() override;

    bool canRequestExit() const override {return true;}
    std::string exitLabel() const override {return "Main Menu";}
    void requestExit() override;

    ftxui::Component getComponent() override;
private:
    ftxui::Component m_component;
    static constexpr Screen SCREEN_TYPE = Screen::Multiplayer_Result;
};

}


#endif