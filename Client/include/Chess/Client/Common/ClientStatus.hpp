#ifndef CHESS_CLIENT_COMMON_CLIENTSTATUS_HPP
#define CHESS_CLIENT_COMMON_CLIENTSTATUS_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// C++ Includes
#include <cstdint>
#include <string>
#include <utility>


namespace Chess {

enum class Severity : std::uint8_t {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Fatal = 4
};

constexpr std::string_view toString(Severity severity) noexcept {
    switch (severity) {
        case Severity::Debug: return "Debug";
        case Severity::Info: return "Info";
        case Severity::Warning: return "Warning";
        case Severity::Error: return "Error";
        case Severity::Fatal: return "Fatal";
    }
    return "Unknown";
}

enum class StatusCode : std::uint8_t {
    None = 0,

    InvalidState = 10,
    InvalidArgument = 11,
    InvalidMove = 12,
    NotImplemented =13,

    ConnectFailed = 20,
    SendFailed = 21,
    ReadFailed = 22,
    Disconnected = 23,
    ProtocolError = 24,
    LoginRejected = 25,

    StartupFailed = 30,
    ShutdownFailed = 31,
    PersistenceFailed = 32,
    RuntimeException = 33,
    UnknownError = 34
};

struct [[nodiscard]] ClientStatus {
    bool ok{false};
    StatusCode code{StatusCode::None};
    Severity severity{Severity::Error};
    std::string message;

    operator bool() const noexcept {
        return ok;
    }

    [[nodiscard]] bool isFailure() const noexcept {
        return !ok;
    }

    [[nodiscard]] bool isSuccess() const noexcept {
        return ok;
    }

    [[nodiscard]] bool isFatal() const noexcept {
        return !ok && severity == Severity::Fatal;
    }

    static ClientStatus Success() {
        return {true, StatusCode::None, Severity::Info, ""};
    }

    static ClientStatus Success(std::string message) {
        return {true, StatusCode::None, Severity::Info, std::move(message)};
    }

    static ClientStatus Debug(std::string message) {
        return {true, StatusCode::None, Severity::Debug, std::move(message)};
    }

    static ClientStatus Info(std::string message) {
        return {true, StatusCode::None, Severity::Info, std::move(message)};
    }

    static ClientStatus Failure(StatusCode code, Severity severity, std::string message) {
        return {false, code, severity, std::move(message)};
    }

    static ClientStatus Warning(StatusCode code, std::string message) {
        return Failure(code, Severity::Warning, std::move(message));
    }

    static ClientStatus Error(StatusCode code, std::string message) {
        return Failure(code, Severity::Error, std::move(message));
    }

    static ClientStatus Fatal(StatusCode code, std::string message) {
        return Failure(code, Severity::Fatal, std::move(message));
    }
};

}


#endif