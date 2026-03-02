#ifndef CHESS_CORE_GAME_GAMESETTINGS_HPP
#define CHESS_CORE_GAME_GAMESETTINGS_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */


// Chess Includes

// ASIO Includes

// C++ Includes
#include <cstdint>
#include <chrono>

namespace Chess {

struct TimeSettings {
    std::uint32_t whiteInitialTimeSeconds;
    std::uint32_t blackInitialTimeSeconds;
    std::uint32_t whiteIncrementTimeSeconds;
    std::uint32_t blackIncrementTimeSeconds;
};

struct JoinSettings {
    std::uint32_t joinPasscode;
    std::uint32_t maxSpectators;
};

struct GameSettings {
    TimeSettings timeSettings;
    JoinSettings joinSettings;

    [[nodiscard]] static constexpr GameSettings Standard() noexcept {
        TimeSettings timeSettings{60*15,60*15,0,0};
        JoinSettings joinSettings{0,UINT32_MAX};
        return GameSettings{timeSettings,joinSettings};
    }
    [[nodiscard]] static constexpr GameSettings Rapid() noexcept {
        TimeSettings timeSettings{60*5,60*5,2,2};
        JoinSettings joinSettings{0,UINT32_MAX};
        return GameSettings{timeSettings,joinSettings};
    }
};



}


#endif