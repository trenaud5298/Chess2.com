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



namespace Chess {

struct MoveRequest {
    std::uint8_t originalPos;
    std::uint8_t newPos;
};

struct JoinRequest {
    std::uint32_t sessionId;
    std::uint32_t gameId;
    std::uint32_t passcode;
    bool spectator;
};


}


#endif