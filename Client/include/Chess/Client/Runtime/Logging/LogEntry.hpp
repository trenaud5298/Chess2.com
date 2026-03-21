#ifndef CHESS_CLIENT_RUNTIME_LOGGING_LOGENTRY_HPP
#define CHESS_CLIENT_RUNTIME_LOGGING_LOGENTRY_HPP

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
    LOG_TRACE   = 1 << 0,
    LOG_DEBUG   = 1 << 1,
    LOG_INFO    = 1 << 2,
    LOG_MESSAGE = 1 << 3,
    LOG_COMMAND = 1 << 4,
    LOG_WARNING = 1 << 5,
    LOG_ERROR   = 1 << 6,
    LOG_FATAL   = 1 << 7,
    LOG_ALL     = 0xFFFFFFFFFFFFFFFF
};

inline std::string logTypeAsString(LogType type) {
    switch (type) {
        case LogType::LOG_TRACE:   return "TRACE";
        case LogType::LOG_DEBUG:   return "DEBUG";
        case LogType::LOG_INFO:    return "INFO ";
        case LogType::LOG_MESSAGE: return "MSG  ";
        case LogType::LOG_COMMAND: return "CMD  ";
        case LogType::LOG_WARNING: return "WARN ";
        case LogType::LOG_ERROR:   return "ERROR";
        case LogType::LOG_FATAL:   return "FATAL";
        case LogType::LOG_ALL:     return "ALL  ";
        default:               return "UNKWN";
    }
}

struct LogEntry {
    LogType type;
    std::string message;
    std::chrono::steady_clock::time_point timestamp;

    explicit LogEntry(LogType type_, const std::string& msg)
    : type(type_), message(std::move(msg)), timestamp(std::chrono::steady_clock::now()) {}

    static LogEntry Trace(const std::string& msg) {return LogEntry(LogType::LOG_TRACE, std::move(msg));}
    static LogEntry Debug(const std::string& msg) {return LogEntry(LogType::LOG_DEBUG, std::move(msg));}
    static LogEntry Info(const std::string& msg) {return LogEntry(LogType::LOG_INFO, std::move(msg));}
    static LogEntry Message(const std::string& msg) {return LogEntry(LogType::LOG_MESSAGE, std::move(msg));}
    static LogEntry Command(const std::string& msg) {return LogEntry(LogType::LOG_COMMAND, std::move(msg));}
    static LogEntry Warning(const std::string& msg) {return LogEntry(LogType::LOG_WARNING, std::move(msg));}
    static LogEntry Error(const std::string& msg) {return LogEntry(LogType::LOG_ERROR, std::move(msg));}
    static LogEntry Fatal(const std::string& msg) {return LogEntry(LogType::LOG_FATAL, std::move(msg));}
};


}

#endif