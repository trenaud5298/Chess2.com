#ifndef CHESS_CLIENT_RUNTIME_MULTIPLAYERCLIENT_HPP
#define CHESS_CLIENT_RUNTIME_MULTIPLAYERCLIENT_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/Runtime/Network/ClientSession.hpp>

// ASIO Includes

// C++ Includes
#include <atomic>

namespace Chess {

class GameClient;
struct ServerInfo;

enum class MultiplayerState {
    CONNECTING,
    IDLE,
    INGAME,
    DISCONNECTING
};

class MultiplayerClient {
public:
    explicit MultiplayerClient(GameClient& gameClient);

    // // Called by GameClient to initiate the full connect+login flow.
    // void connect(const ServerInfo& server);
    // void disconnect();
    //
    // // Called back by ClientSession (from ASIO strand)
    // void onLoginSuccess();
    // void onLoginFailure(const std::string& reason);
    // void onDisconnect();
    // void onMessage(const Message& msg);
    //
    // [[nodiscard]] MultiplayerState state() const { return m_state; }
    // ClientSession& clientSession() { return m_clientSession; }

private:
    void transitionTo(MultiplayerState next);

    GameClient&      m_gameClient;
    // ClientSession    m_clientSession;   // lives here now
    std::atomic<MultiplayerState> m_state{MultiplayerState::CONNECTING};
};
}

#endif