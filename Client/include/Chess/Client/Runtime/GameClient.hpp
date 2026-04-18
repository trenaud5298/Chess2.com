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
#include <Chess/Client/Common/ClientStatus.hpp>
#include <Chess/Client/Common/ClientEvent.hpp>
#include <Chess/Client/Common/EventQueue.hpp>

// ASIO Includes
#include <asio/io_context.hpp>
#include <asio/executor_work_guard.hpp>

// C++ Includes
#include <atomic>
#include <chrono>
#include <cstdint>
#include <mutex>
#include <optional>
#include <string_view>
#include <thread>


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
    [[nodiscard]] CallbackRegistry<const ClientEvent&>& eventRegistry() {return m_eventRegistry;}

    [[nodiscard]] const LoggingManager& loggingManager() const {return m_loggingManager;}
    [[nodiscard]] const PersistenceManager& persistenceManager() const {return m_persistenceManager;}
    [[nodiscard]] const CallbackRegistry<ClientState>& stateRegistry() const {return m_stateRegistry;}
    [[nodiscard]] const CallbackRegistry<const ClientEvent&>& eventRegistry() const {return m_eventRegistry;}

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

    // GameClient Core Controls
    [[nodiscard]] ClientStatus tick();
    [[nodiscard]] ClientStatus returnToIdle();

    // GameClient Errors
    [[nodiscard]] std::optional<ClientStatus> consumeFatalError();
    void recoverFromFatalError() noexcept;

    // Singleplayer Controls
    [[nodiscard]] ClientStatus enterSingleplayerSetup();
    [[nodiscard]] ClientStatus startSingleplayer(const SingleplayerConfig& config);
    [[nodiscard]] ClientStatus stopSingleplayer();
    [[nodiscard]] ClientStatus restartSingleplayer();
    [[nodiscard]] ClientStatus submitSingleplayerMove(ID from, Pos to);
    [[nodiscard]] ClientStatus resignSingleplayer();
    [[nodiscard]] ClientStatus pauseSingleplayer();
    [[nodiscard]] ClientStatus resumeSingleplayer();

    // Singleplayer Info
    [[nodiscard]] SingleplayerView singleplayerView() const;

    // Multiplayer Controls
    [[nodiscard]] ClientStatus enterMultiplayerSetup();
    [[nodiscard]] ClientStatus requestMultiplayerConnect(const ServerInfo& server);
    [[nodiscard]] ClientStatus requestMultiplayerDisconnect();

    // Multiplayer Info
    [[nodiscard]] MultiplayerState multiplayerState() const noexcept {return m_multiplayerClient.state();}
    [[nodiscard]] MultiplayerView multiplayerView() const noexcept {return m_multiplayerClient.view();}

private:
    void transitionTo(ClientState newState);
    void setFatalError(StatusCode error, std::string message) noexcept;
    void publishEvent(ClientEvent event);
    void logEvent(const ClientEvent& event);
    void handleEvent(const ClientEvent& event);
        void handleSingleplayerEvent(const ClientEvent& event);
        void handleMultiplayerEvent(const ClientEvent& event);
            void handleMultiplayerConnectEvent(const ClientEvent& event);
            void handleMultiplayerTransportEvent(const ClientEvent& event);
            void handleMultiplayerLoginEvent(const ClientEvent& event);
            void handleMultiplayerDisconnectEvent(const ClientEvent& event);
        void handlePersistenceEvent(const ClientEvent& event);

private:
    // Core Client System
    asio::io_context m_context;
    std::optional<asio::executor_work_guard<asio::io_context::executor_type>> m_workGuard;
    std::thread m_asioThread;
    std::chrono::steady_clock::time_point m_startTime;
    std::atomic<ClientState> m_state{ClientState::Idle};

    // Errors
    std::mutex m_fatalErrorMutex;
    std::optional<ClientStatus> m_fatalError;

    // Subsystems
    LoggingManager m_loggingManager;
    PersistenceManager m_persistenceManager;
    CallbackRegistry<ClientState> m_stateRegistry;

    // Singleplayer & Multiplayer Clients
    SingleplayerClient m_singleplayerClient;
    MultiplayerClient m_multiplayerClient;

    // Events
    EventQueue<ClientEvent> m_eventQueue;
    CallbackRegistry<const ClientEvent&> m_eventRegistry;
};
}

#endif
