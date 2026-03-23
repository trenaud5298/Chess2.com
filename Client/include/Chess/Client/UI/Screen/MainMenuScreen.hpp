#ifndef CHESS_CLIENT_UI_MAINMENUSCREEN_HPP
#define CHESS_CLIENT_UI_MAINMENUSCREEN_HPP

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


namespace Chess {

class ClientPanel;

class MainMenuScreen : public ScreenInterface {
public:
    explicit MainMenuScreen(ClientPanel& clientPanel);
    ~MainMenuScreen();
    ftxui::Component getComponent() override;


private:
    ;
    ftxui::Component m_component;
    static constexpr Screen SCREEN_TYPE = Screen::MainMenu;
};

}


#endif