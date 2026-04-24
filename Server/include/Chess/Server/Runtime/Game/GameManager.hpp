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
#include <Chess/Server/Runtime/Game/GameRoom.hpp>

// ASIO Includes
#include <asio/strand.hpp>
#include <asio/any_io_executor.hpp>

// C++ Includes
#include <atomic>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>



namespace Chess {

class GameServer;
class Message;

enum class GameManagerEventKind : std::uint8_t {
    RoomJoined,
    RoomLeft,
    GameStarted,
    GameEnded
};

struct GameManagerEvent {
    GameManagerEventKind kind;
    SessionID sessionID;
    RoomID roomID;
};

using SessionNotifyCallback = std::function<void(const GameManagerEvent&)>;

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
    void requestCreateRoom(SessionID sessionID, SessionNotifyCallback callback = {});
    void requestJoinRoom(SessionID sessionID, RoomID roomID, bool spectator, SessionNotifyCallback callback = {});
    void requestLeaveRoom(SessionID sessionID, SessionNotifyCallback callback = {});
    void requestGameChat(SessionID sessionID, std::string message, SessionNotifyCallback callback = {});
    void requestMove(SessionID sessionID, std::uint8_t from, std::uint8_t to, SessionNotifyCallback callback = {});

private:
    // Strand Version Of Public Request Functions
    void doRequestCreateRoom(SessionID sessionID, SessionNotifyCallback callback = {});
    void doRequestJoinRoom(SessionID sessionID, RoomID roomID, bool spectator, SessionNotifyCallback callback = {});
    void doRequestLeaveRoom(SessionID sessionID, SessionNotifyCallback callback = {});
    void doRequestMove(SessionID sessionID, std::uint8_t from, std::uint8_t to, SessionNotifyCallback callback = {});
    void doRequestGameChat(SessionID sessionID, std::string message, SessionNotifyCallback callback = {});

    // Strand Only Helpers
    void sendToSession(SessionID sessionID, std::shared_ptr<const Message> message);
    void sendToSessions(const std::vector<SessionID>& sessionIDs, std::shared_ptr<const Message> message);
    void sendToRoomPlayers(const GameRoom& room, std::shared_ptr<const Message> message);
    void sendToRoomAll(const GameRoom& room, std::shared_ptr<const Message> message);

    void sendCreateRoomResponse(SessionID sessionID, bool success, RoomID roomID, std::string reason);
    void sendJoinRoomResponse(SessionID sessionID, bool success, RoomID roomID, std::string reason);
    void sendRoomError(SessionID sessionID, std::string reason);
    void sendGameUpdate(const GameRoom& room);

    // Room and ID Helpers
    [[nodiscard]] std::shared_ptr<GameRoom> roomByID(RoomID roomID);
    [[nodiscard]] std::shared_ptr<GameRoom> roomBySession(SessionID sessionID);
    void removeRoomIfEmpty(RoomID roomID);

private:
    GameServer& m_gameServer;
    std::atomic<LifecycleState> m_state{LifecycleState::STOPPED};
    asio::strand<asio::any_io_executor> m_strand;

    std::atomic<RoomID> m_nextRoomID{1};

    // Strand Owned
    std::unordered_map<RoomID, std::shared_ptr<GameRoom>> m_roomIDToRoom;
    std::unordered_map<SessionID, RoomID> m_sessionIDToRoomID;

    // Future If I Have Time After Gettings Rooms Working (Should Be Pretty Easy)
    // std::dequeu<SessionID> m_matchmakingQueue
};
}

#endif