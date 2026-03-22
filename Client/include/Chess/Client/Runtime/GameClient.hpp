#ifndef CHESS_CLIENT_RUNTIME_GAMECLIENT_HPP
#define CHESS_CLIENT_RUNTIME_GAMECLIENT_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/Runtime/Logging/LoggingManager.hpp>
#include <Chess/Client/Runtime/Persistence/PersistenceManager.hpp>
#include <Chess/Client/Runtime/SingleplayerClient.hpp>
#include <Chess/Client/Runtime/MultiplayerClient.hpp>
#include <Chess/Client/Runtime/Callback/CallbackRegistry.hpp>

// ASIO Includes
#include <asio/io_context.hpp>

// C++ Includes
#include <memory>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <cstdint>

namespace Chess {


enum class ClientState {
    IDLE,
    SINGLEPLAYER,
    MULTIPLAYER
};

class GameClient {
public:
    GameClient();
    ~GameClient();

    GameClient(const GameClient&) = delete;
    GameClient& operator=(const GameClient&) = delete;
    GameClient(GameClient&&) = delete;
    GameClient& operator=(GameClient&&) = delete;

    // Subsystems
    LoggingManager& loggingManager() {return m_loggingManager;}
    PersistenceManager& persistenceManager() {return m_persistenceManager;}
    CallbackRegistry<ClientState>& callbackRegistry() {return m_stateRegistry;}
    [[nodiscard]] const LoggingManager& loggingManager() const {return m_loggingManager;}
    [[nodiscard]] const PersistenceManager& persistenceManager() const {return m_persistenceManager;}
    [[nodiscard]] const CallbackRegistry<ClientState>& callbackRegistry() const {return m_stateRegistry;}

    // Singleplayer/Multiplayer Accessors (Undefined When Called On Mismatch State)
    MultiplayerClient&  multiplayerClient() {return m_multiplayerClient;}
    SingleplayerClient& singleplayerClient() {return m_singleplayerClient;}
    [[nodiscard]] const MultiplayerClient&  multiplayerClient() const {return m_multiplayerClient;}
    [[nodiscard]] const SingleplayerClient& singleplayerClient() const {return m_singleplayerClient;}

    // Other Accessors
    [[nodiscard]] asio::io_context& ioContext() {return m_context;}
    [[nodiscard]] const asio::io_context& ioContext() const {return m_context;}
    [[nodiscard]] ClientState state() const {return m_state;}

    // Client Level Stats
    [[nodiscard]] std::chrono::steady_clock::time_point startTime() const;

    [[nodiscard]] std::chrono::milliseconds uptimeCurrent() const;
    [[nodiscard]] std::chrono::milliseconds uptimeAtPoint(std::chrono::steady_clock::time_point point) const;

    // GameClient Controls
    void startSingleplayer(const SingleplayerConfig& config);
    void stopSingleplayer();

    void startMultiplayer(const ServerInfo& server);
    void stopMultiplayer();

private:
    void onMultiplayerStopped();
    void transitionTo(ClientState newState);

private:
    // Core Client System
    asio::io_context m_context;
    std::chrono::steady_clock::time_point m_startTime;
    std::atomic<ClientState> m_state{ClientState::IDLE};

    // Subsystems
    LoggingManager m_loggingManager;
    PersistenceManager m_persistenceManager;
    CallbackRegistry<ClientState> m_stateRegistry;

    // Singleplayer & Multiplayer Clients
    SingleplayerClient m_singleplayerClient;
    MultiplayerClient m_multiplayerClient;
};
}

#endif