#ifndef CHESS_CLIENT_UI_SCREENINTERFACE_HPP
#define CHESS_CLIENT_UI_SCREENINTERFACE_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes

// ASIO Includes

// FTXUI Includes
#include <ftxui/component/component.hpp>

// C++ Includes
#include <cstdint>

namespace Chess {

enum class Screen : std::uint8_t {
    MainMenu = 0,
    Singleplayer_Setup = 1,
    Singleplayer_Game = 2,
    Singleplayer_Result = 3,
    Multiplayer_Select = 4,
    Multiplayer_Lobby = 5,
    Multiplayer_Game = 6,
    Multiplayer_Result = 7
};

constexpr std::string toString(Screen screen) {
    switch (screen) {
        case Screen::MainMenu:
            return "Main Menu";
        case Screen::Singleplayer_Setup:
            return "Singleplayer Setup";
        case Screen::Singleplayer_Game:
            return "Singleplayer Game";
        case Screen::Singleplayer_Result:
            return "Singleplayer Result";
        case Screen::Multiplayer_Select:
            return "Multiplayer Select";
        case Screen::Multiplayer_Lobby:
            return "Multiplayer Lobby";
        case Screen::Multiplayer_Game:
            return "Multiplayer Game";
        case Screen::Multiplayer_Result:
            return "Multiplayer Result";
        default:
            return "";
    }
    return "";
}

class ClientPanel;

class ScreenInterface {

public:
    explicit ScreenInterface(ClientPanel& panel) : m_clientPanel(panel) {}
    virtual ~ScreenInterface() = default;

    virtual void onEnter() {}
    virtual void onLeave() {}

    virtual void onResume() {}

    // Return True When Back Handled And Modem Pushed
    // Return False To Allow Back To Proceed
    virtual bool onBackRequested() { return false; }

    [[nodiscard]] virtual ftxui::Component getComponent() = 0;
protected:
    ClientPanel& m_clientPanel;
};

}

#endif