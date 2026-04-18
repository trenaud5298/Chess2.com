#ifndef CHESS_CLIENT_RUNTIME_MULTIPLAYERTYPES_HPP
#define CHESS_CLIENT_RUNTIME_MULTIPLAYERTYPES_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/Common/ServerInfo.hpp>

// C++ Includes
#include <cstdint>
#include <mutex>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace Chess {

using MessageThreadID = std::uint32_t;
inline constexpr MessageThreadID NO_MESSAGE_THREAD_ID = 0;

enum class MultiplayerState : std::uint8_t {
    Idle = 0,
    ConnectingNetwork,
    AwaitingLogin,
    Connected
};

struct MultiplayerView {
    MultiplayerState state{MultiplayerState::Idle};
    std::optional<ServerInfo> serverInfo;
    bool socketConnected{false};
    bool loginAccepted{false};
};

} // namespace Chess

#endif
