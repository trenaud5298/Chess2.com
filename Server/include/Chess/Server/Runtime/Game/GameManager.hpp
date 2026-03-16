#ifndef CHESS_SERVER_RUNTIME_GAME_GAMEMANAGER_HPP
#define CHESS_SERVER_RUNTIME_GAME_GAMEMANAGER_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Server/Runtime/Common/LifecycleState.hpp>
#include <Chess/Server/Runtime/Game/GameRoom.hpp>

// ASIO Includes

// C++ Includes
#include <atomic>
#include <cstdint>
#include <unordered_map>

namespace Chess {

class GameServer;
class Move;

class GameManager {
public:
    explicit GameManager(GameServer& gameServer);
    ~GameManager();

    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;
    GameManager(GameManager&&) = delete;
    GameManager& operator=(GameManager&&) = delete;

    // Lifetime Control
    void start();
    void stop();

    // Room Controls
    std::uint64_t createRoom(std::uint64_t sessionID);
    bool joinRoom(std::uint64_t sessionID, std::uint64_t roomID);
    bool spectateRoom(std::uint64_t sessionID, std::uint64_t roomID);
    void leaveRoom(std::uint64_t sessionID);
    void onMove(std::uint64_t sessionID, Move& move); //TODO: Sync With Board Move Function
private:
    GameServer& m_gameServer;
    std::atomic<LifecycleState> m_state{LifecycleState::STOPPED};

    std::unordered_map<std::uint64_t, GameRoom> m_roomIDToRoomMap;
    std::unordered_map<std::uint64_t, std::uint64_t> m_sessionIDToRoomIDMap;
};
}

#endif