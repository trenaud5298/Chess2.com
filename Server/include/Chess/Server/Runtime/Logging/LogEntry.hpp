#ifndef CHESS_SERVER_RUNTIME_LOGGING_LOGENTRY_HPP
#define CHESS_SERVER_RUNTIME_LOGGING_LOGENTRY_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes

// ASIO Includes

// C++ Includes
#include <string>
#include <chrono>
#include <thread>
#include <cstdint>
#include <format>

namespace Chess {

enum LogType : std::uint64_t {
    TRACE   = 1 << 0,
    DEBUG   = 1 << 1,
    INFO    = 1 << 2,
    MESSAGE = 1 << 3,
    COMMAND = 1 << 4,
    WARNING = 1 << 5,
    ERROR   = 1 << 6,
    FATAL   = 1 << 7,
    ALL     = 0xFFFFFFFFFFFFFFFF,
};

inline std::string logTypeAsString(LogType type) {
    switch (type) {
        case LogType::TRACE:   return "TRACE";
        case LogType::DEBUG:   return "DEBUG";
        case LogType::INFO:    return "INFO ";
        case LogType::MESSAGE: return "MSG  ";
        case LogType::COMMAND: return "CMD  ";
        case LogType::WARNING: return "WARN ";
        case LogType::ERROR:   return "ERROR";
        case LogType::FATAL:   return "FATAL";
        case LogType::ALL:     return "ALL  ";
        default:               return "UNKWN";
    }
}

struct LogEntry {
    LogType type;
    std::string message;
    std::chrono::steady_clock::time_point timestamp;

    explicit LogEntry(LogType type_, const std::string& msg)
    : type(type_), message(std::move(msg)), timestamp(std::chrono::steady_clock::now()) {}

    static LogEntry Trace(const std::string& msg) {return LogEntry(LogType::TRACE, std::move(msg));}
    static LogEntry Debug(const std::string& msg) {return LogEntry(LogType::DEBUG, std::move(msg));}
    static LogEntry Info(const std::string& msg) {return LogEntry(LogType::INFO, std::move(msg));}
    static LogEntry Message(const std::string& msg) {return LogEntry(LogType::MESSAGE, std::move(msg));}
    static LogEntry Command(const std::string& msg) {return LogEntry(LogType::COMMAND, std::move(msg));}
    static LogEntry Warning(const std::string& msg) {return LogEntry(LogType::WARNING, std::move(msg));}
    static LogEntry Error(const std::string& msg) {return LogEntry(LogType::ERROR, std::move(msg));}
    static LogEntry Fatal(const std::string& msg) {return LogEntry(LogType::FATAL, std::move(msg));}
};


}

#endif