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
#include <expected>

namespace Chess {


enum class ClientMode {
    Idle,
    Singleplayer,
    Multiplayer
};

enum class ClientPhase {
    Idle,
    Starting,
    Active,
    Paused,
    Stopping,
    Error
};

struct ClientStatus {
    ClientMode mode{ClientMode::Idle};
    ClientPhase phase{ClientPhase::Idle};
    std::string message;
};

enum class ClientError {
    None,
    InvalidState,
    StartupFailed,
    ShutdownFailed,
    NetworkError
};

struct ClientCommandResult {
    bool ok{false};
    ClientError error{ClientError::None};
    std::string message;

    operator bool() const {return ok;}

    static ClientCommandResult Success() {
        return {true, ClientError::None, ""};
    }
    static ClientCommandResult Failure(ClientError error, std::string message) {
        return {false, error, std::move(message)};
    }
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
    CallbackRegistry<ClientMode>& callbackRegistry() {return m_stateRegistry;}
    [[nodiscard]] const LoggingManager& loggingManager() const {return m_loggingManager;}
    [[nodiscard]] const PersistenceManager& persistenceManager() const {return m_persistenceManager;}
    [[nodiscard]] const CallbackRegistry<ClientMode>& callbackRegistry() const {return m_stateRegistry;}

    // Singleplayer/Multiplayer Accessors (Undefined When Called On Mismatch State)
    MultiplayerClient&  multiplayerClient() {return m_multiplayerClient;}
    SingleplayerClient& singleplayerClient() {return m_singleplayerClient;}
    [[nodiscard]] const MultiplayerClient&  multiplayerClient() const {return m_multiplayerClient;}
    [[nodiscard]] const SingleplayerClient& singleplayerClient() const {return m_singleplayerClient;}

    // Other Accessors
    [[nodiscard]] asio::io_context& ioContext() {return m_context;}
    [[nodiscard]] const asio::io_context& ioContext() const {return m_context;}
    [[nodiscard]] ClientMode mode() const {return m_mode;}
    [[nodiscard]] ClientPhase phase() const {return m_phase;}

    // Client Level Stats
    [[nodiscard]] std::chrono::steady_clock::time_point startTime() const;

    [[nodiscard]] std::chrono::milliseconds uptimeCurrent() const;
    [[nodiscard]] std::chrono::milliseconds uptimeAtPoint(std::chrono::steady_clock::time_point point) const;

    // GameClient Controls
    ClientCommandResult startSingleplayer(const SingleplayerConfig& config);
    ClientCommandResult stopSingleplayer();

    ClientCommandResult startMultiplayer(const ServerInfo& server);
    ClientCommandResult stopMultiplayer();

private:
    void onMultiplayerStopped();

    void transitionTo(ClientMode newMode);
    void transitionTo(ClientPhase newPhase);

private:
    // Core Client System
    asio::io_context m_context;
    std::chrono::steady_clock::time_point m_startTime;
    std::atomic<ClientMode> m_mode{ClientMode::Idle};
    std::atomic<ClientPhase> m_phase{ClientPhase::Idle};

    // Subsystems
    LoggingManager m_loggingManager;
    PersistenceManager m_persistenceManager;
    CallbackRegistry<ClientMode> m_stateRegistry;
    CallbackRegistry<ClientPhase> m_phaseRegistry;

    // Singleplayer & Multiplayer Clients
    SingleplayerClient m_singleplayerClient;
    MultiplayerClient m_multiplayerClient;
};
}

#endif