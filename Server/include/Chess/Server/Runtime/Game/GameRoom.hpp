#ifndef CHESS_SERVER_RUNTIME_GAME_GAMEROOM_HPP
#define CHESS_SERVER_RUNTIME_GAME_GAMEROOM_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Core/Game/Board.hpp>

// ASIO Includes

// C++ Includes
#include <atomic>
#include <cstdint>
#include <unordered_map>

namespace Chess {

class GameServer;
class Move;
enum GameState {
    WAITING_FOR_PLAYERS,
    PLAYER1_TURN,
    PLAYER2_TURN,
    GAME_OVER
};

class GameRoom {
public:
    explicit GameRoom(GameServer& gameServer);
    ~GameRoom() = default;

    GameRoom(const GameRoom&) = delete;
    GameRoom& operator=(const GameRoom&) = delete;
    GameRoom(GameRoom&&) = delete;
    GameRoom& operator=(GameRoom&&) = delete;

    // Lifetime Control
    void start();
    void stop();

    bool joinRoom(std::uint64_t sessionID);
    bool spectateRoom(std::uint64_t sessionID);
    void leaveRoom(std::uint64_t sessionID);
    void onMove(std::uint64_t sessionID, Move& move); //TODO: Sync With Board Move Function
private:
    GameServer& m_gameServer;


};
}

#endif