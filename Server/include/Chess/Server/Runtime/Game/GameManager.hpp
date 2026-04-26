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
#include <Chess/Core/Common/LifecycleState.hpp>
#include <Chess/Core/Common/Types.hpp>
#include <Chess/Core/Game/ChessGame.hpp>
#include <Chess/Server/Runtime/Game/GameRoom.hpp>

// ASIO Includes
#include <asio/strand.hpp>
#include <asio/any_io_executor.hpp>
#include <asio/steady_timer.hpp>

// C++ Includes
#include <atomic>
#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Chess/Core/Networking/MessagePayloads.hpp"

namespace Chess {

class GameServer;
class Message;

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

    // Thread Safe Request Functions
    void requestCreateRoom(SessionID sessionID, RoomCreateConfig config);
    void requestJoinRoom(SessionID sessionID, RoomID roomID, bool spectator, std::string password);
    void requestListRooms(SessionID sessionID);
    void requestLeaveRoom(SessionID sessionID);
    void requestMove(SessionID sessionID, std::uint8_t from, std::uint8_t to, PromotionPiece promotion);
    void requestGameChat(SessionID sessionID, std::shared_ptr<const Message> message);

private:
    // Strand Version Of Public Request Functions
    void doRequestCreateRoom(SessionID sessionID, RoomCreateConfig config);
    void doRequestJoinRoom(SessionID sessionID, RoomID roomID, bool spectator, std::string password);
    void doRequestListRooms(SessionID sessionID);
    void doRequestLeaveRoom(SessionID sessionID);
    void doRequestMove(SessionID sessionID, std::uint8_t from, std::uint8_t to, PromotionPiece promotion);
    void doRequestGameChat(SessionID sessionID, std::shared_ptr<const Message> message);

    // Timer Based Ticks
    void scheduleTick();
    void onTick();

    // Strand Only Helpers
    void sendToSession(SessionID sessionID, std::shared_ptr<const Message> message);
    void sendToSessions(const std::vector<SessionID>& sessionIDs, std::shared_ptr<const Message> message);
    void sendToRoomPlayers(const GameRoom& room, std::shared_ptr<const Message> message);
    void sendToRoomAll(const GameRoom& room, std::shared_ptr<const Message> message);

    void sendCreateRoomResponse(SessionID sessionID, bool success, RoomID roomID, RoomMemberType memberType, COLOR color, std::string reason);
    void sendJoinRoomResponse(SessionID sessionID, bool success, RoomID roomID, RoomMemberType memberType, COLOR color, std::string reason);
    void sendListRoomsResponse(SessionID sessionID, std::vector<RoomSummary> rooms);
    void sendLeaveRoomResponse(SessionID sessionID, bool success, RoomID roomID, std::string reason);
    void sendRoomError(SessionID sessionID, std::string reason);
    void sendGameUpdate(const GameRoom& room);

    // Response Helpers
    [[nodiscard]] RoomSummary makeRoomSummary(const GameRoom& room) const;
    [[nodiscard]] GameUpdate makeGameUpdate(const GameRoom& room) const;
    [[nodiscard]] std::string sessionName(SessionID sessionID) const;

    // Room and ID Helpers
    [[nodiscard]] std::shared_ptr<GameRoom> roomByID(RoomID roomID);
    [[nodiscard]] std::shared_ptr<GameRoom> roomBySession(SessionID sessionID);
    [[nodiscard]] bool sessionAlreadyInRoom(SessionID sessionID) const noexcept;
    void bindSessionToRoom(SessionID sessionID, RoomID roomID);
    void unbindSessionFromRoom(SessionID sessionID);
    void removeRoomIfEmpty(RoomID roomID);

private:
    GameServer& m_gameServer;
    std::atomic<LifecycleState> m_state{LifecycleState::STOPPED};
    asio::strand<asio::any_io_executor> m_strand;
    asio::steady_timer m_tickTimer;

    std::atomic<RoomID> m_nextRoomID{1};

    // Strand Owned
    std::unordered_map<RoomID, std::shared_ptr<GameRoom>> m_roomIDToRoom;
    std::unordered_map<SessionID, RoomID> m_sessionIDToRoomID;

    // Future If I Have Time After Gettings Rooms Working (Should Be Pretty Easy)
    // std::dequeu<SessionID> m_matchmakingQueue

    static constexpr std::chrono::milliseconds TICK_INTERVAL{250};
};
}

#endif