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


enum class ClientState : std::uint8_t {
    Idle = 0x00,
    Error = 0x01,

    SingleplayerSetup = 0x10,
    SingleplayerInGame = 0x11,
    SingleplayerResult = 0x12,

    MultiplayerSetup = 0x20,
    MultiplayerConnecting = 0x21,
    MultiplayerLobby = 0x22,
    MultiplayerInGame = 0x23,
    MultiplayerResult = 0x24
};

constexpr std::string_view toString(ClientState state) {
    switch (state) {
        case ClientState::Idle: return "Idle";
        case ClientState::SingleplayerSetup: return "SingleplayerSetup";
        case ClientState::SingleplayerInGame: return "SingleplayerInGame";
        case ClientState::SingleplayerResult: return "SingleplayerResult";
        case ClientState::MultiplayerSetup: return "MultiplayerSelect";
        case ClientState::MultiplayerConnecting: return "MultiplayerConnecting";
        case ClientState::MultiplayerLobby: return "MultiplayerLobby";
        case ClientState::MultiplayerInGame: return "MultiplayerInGame";
        case ClientState::MultiplayerResult: return "MultiplayerResult";
        case ClientState::Error: return "Error";
    }
    return "";
}

enum class ClientError {
    None,
    InvalidState,
    StartupFailed,
    ShutdownFailed,
    NetworkError,
    RuntimeException,
    CommandRejected
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
    [[nodiscard]] LoggingManager& loggingManager() {return m_loggingManager;}
    [[nodiscard]] PersistenceManager& persistenceManager() {return m_persistenceManager;}
    [[nodiscard]] CallbackRegistry<ClientState>& stateRegistry() {return m_stateRegistry;}
    [[nodiscard]] const LoggingManager& loggingManager() const {return m_loggingManager;}
    [[nodiscard]] const PersistenceManager& persistenceManager() const {return m_persistenceManager;}
    [[nodiscard]] const CallbackRegistry<ClientState>& stateRegistry() const {return m_stateRegistry;}

    // Other Accessors
    [[nodiscard]] asio::io_context& ioContext() {return m_context;}
    [[nodiscard]] const asio::io_context& ioContext() const {return m_context;}
    [[nodiscard]] ClientState state() const {return m_state.load();}
    [[nodiscard]] bool isSingleplayerState() const { return (static_cast<std::uint8_t>(m_state.load()) & 0x10);}
    [[nodiscard]] bool isMultiplayerState() const { return (static_cast<std::uint8_t>(m_state.load()) & 0x20);}

    // Client Level Stats
    [[nodiscard]] std::chrono::steady_clock::time_point startTime() const {return m_startTime;}
    [[nodiscard]] std::chrono::milliseconds uptimeCurrent() const {return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_startTime);}
    [[nodiscard]] std::chrono::milliseconds uptimeAtPoint(std::chrono::steady_clock::time_point point) const {return std::chrono::duration_cast<std::chrono::milliseconds>(point - m_startTime);}

    // GameClient Controls
    ClientCommandResult tick();
    ClientCommandResult shutdown();
    ClientCommandResult returnToIdle();

    // Singleplayer Controls
    ClientCommandResult enterSingleplayerSetup();
    ClientCommandResult startSingleplayer(const SingleplayerConfig& config);
    ClientCommandResult stopSingleplayer();
    ClientCommandResult submitSingleplayerMove(ID from, Pos to);
    ClientCommandResult resignSingleplayer();
    ClientCommandResult pauseSingleplayer();
    ClientCommandResult resumeSingleplayer();

    // Singleplayer Info
    [[nodiscard]] SingleplayerView singleplayerView() const;

    ClientCommandResult enterMultiplayerSetup();;
    ClientCommandResult startMultiplayer(const ServerInfo& server);
    ClientCommandResult stopMultiplayer();

private:
    void transitionTo(ClientState newState);

private:
    // Core Client System
    asio::io_context m_context;
    std::chrono::steady_clock::time_point m_startTime;
    std::atomic<ClientState> m_state{ClientState::Idle};

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
