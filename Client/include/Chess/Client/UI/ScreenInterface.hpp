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
    Singleplayer = 1,
    Multiplayer = 2,
    Settings = 3
};

class ScreenInterface {

public:
    virtual ~ScreenInterface() = default;
    virtual ftxui::Component getComponent() = 0;
    virtual void onEnter() {}
    virtual void onLeave() {}
private:

};

}

#endif