#ifndef CHESS_SERVER_GAME_GAMEROOM_HPP
#define CHESS_SERVER_GAME_GAMEROOM_HPP

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
#include <vector>

namespace Chess {

struct GameSettings;
class GameServer;

class GameRoom {

public:
    explicit GameRoom(const GameSettings& settings, GameServer* gameServer);
    ~GameRoom();
    


private:
    std::uint32_t m_whiteSessionId;
    std::uint32_t m_blackSessionId;
    std::vector<std::uint32_t> m_spectatorSessionIds;
};

}


#endif