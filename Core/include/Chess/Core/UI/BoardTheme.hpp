#ifndef CHESS_CORE_UI_BOARDTHEME_HPP
#define CHESS_CORE_UI_BOARDTHEME_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// FTXUI Includes
#include <ftxui/screen/color.hpp>

// C++ Includes
#include <cstdint>


namespace Chess {

struct RGBColor {
    std::uint8_t red{0};
    std::uint8_t green{0};
    std::uint8_t blue{0};

    [[nodiscard]] ftxui::Color toFTXUIColor() const {
        // Note:: ftxui::Color::RGB has an issue on windows
        // due to a macro but wrapping it in () for constructor
        // call fixes compile issue.
        return (ftxui::Color::RGB)(red, green, blue);
    }
};

struct BoardTheme {
    RGBColor lightSquare{203, 213, 224};
    RGBColor darkSquare{117, 171, 188};
    RGBColor whitePiece{251, 254, 255};
    RGBColor blackPiece{28, 40, 58};
    RGBColor cursorSquare{90, 156, 235};
    RGBColor selectedSquare{78, 184, 146};
};




}


#endif