#ifndef CHESS_SERVER_GAME_GAMEMANAGER_HPP
#define CHESS_SERVER_GAME_GAMEMANAGER_HPP

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

class GameServer;
class GameRoom;
class GameSettings;
struct JoinRequest;
struct MoveRequest;

class GameManager {

public:
    explicit GameManager(GameServer* gameServer);
    ~GameManager();

    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;

    [[nodiscard]] std::uint32_t createGame(const GameSettings& settings);
    bool joinGame(const JoinRequest& request);
    void onPlayerMove(std::uint32_t sessionId, MoveRequest move);
    void onPlayerLeave(std::uint32_t sessionId);
    void onSessionDisconnect(std::uint32_t sessionId);

private:

};


}


#endif