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

enum class MultiplayerErrorCode : std::uint8_t {
    None = 0,
    InvalidState,
    InvalidArgument,
    ConnectFailed,
    SendFailed,
    ReadFailed,
    LoginRejected,
    ProtocolError,
    Disconnected
};


struct [[nodiscard]] MultiplayerStatus {
    bool ok{false};
    MultiplayerErrorCode code{MultiplayerErrorCode::None};
    std::string message;

    operator bool() const noexcept { return ok; }

    static MultiplayerStatus Success() {
        return {true, MultiplayerErrorCode::None, ""};
    }

    static MultiplayerStatus Failure(MultiplayerErrorCode code, std::string message) {
        return {false, code, std::move(message)};
    }
};

enum class MultiplayerEventType : std::uint8_t {
    ConnectFailed = 0,
    LoginAccepted,
    LoginRejected,
    Disconnected
};

struct MultiplayerEvent {
    MultiplayerEventType type{MultiplayerEventType::Disconnected};
    MultiplayerErrorCode errorCode{MultiplayerErrorCode::None};
    MessageThreadID messageThreadID{NO_MESSAGE_THREAD_ID};
    std::string message;
};

struct MultiplayerView {
    MultiplayerState state{MultiplayerState::Idle};
    std::optional<ServerInfo> serverInfo;
    bool socketConnected{false};
    bool loginAccepted{false};
};


template<typename T>
class AsyncEventQueue {
public:
    void push(T value) {
        std::lock_guard lock(m_mutex);
        m_queue.push_back(std::move(value));
    }

    [[nodiscard]] std::vector<T> drain() {
        std::lock_guard lock(m_mutex);
        std::vector<T> out;
        out.swap(m_queue);
        return out;
    }

private:
    std::mutex m_mutex;
    std::vector<T> m_queue;
};

} // namespace Chess

#endif
