#ifndef CHESS_CLIENT_UI_SETTINGSSCREEN_HPP
#define CHESS_CLIENT_UI_SETTINGSSCREEN_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/ScreenInterface.hpp>

// ASIO Includes

// FTXUI Includes
#include <ftxui/component/component.hpp>

// C++ Includes


namespace Chess {

class ClientPanel;

class SettingsScreen : public ScreenInterface {
public:
    explicit SettingsScreen(ClientPanel& clientPanel);
    ~SettingsScreen();
    ftxui::Component getComponent() override;
    void onEnter() override;
    void onLeave() override;

private:
    ClientPanel& m_clientPanel;
    ftxui::Component m_component;
    static constexpr Screen SCREEN_TYPE = Screen::Settings;
};

}


#endif