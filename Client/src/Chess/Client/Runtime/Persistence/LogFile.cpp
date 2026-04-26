/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/Runtime/Persistence/LogFile.hpp>
#include <Chess/Client/Runtime/GameClient.hpp>
#include <Chess/Core/Common/TimeFormat.hpp>

// TOML Includes
#include <chrono>
#include <fstream>
#include <toml++/toml.hpp>
#include <utility>
#include <format>
#include <iostream>

// C++ Includes

namespace {

// Returns formatted string "YYYY-MM-DD HH:MM:SS" (seconds precision)
inline std::string formatTimePointSeconds(const std::chrono::system_clock::time_point& tp) {
    std::chrono::zoned_time localTime(std::chrono::current_zone(), std::chrono::time_point_cast<std::chrono::seconds>(tp));
    return std::format("{:%Y-%m-%d %H-%M-%S}", localTime.get_local_time());
}

// Returns formatted string "YYYY-MM-DD HH:MM:SS.mmm" (milliseconds precision)
inline std::string formatTimePointMilliseconds(const std::chrono::system_clock::time_point& tp) {
    std::chrono::zoned_time localTime(std::chrono::current_zone(), std::chrono::time_point_cast<std::chrono::milliseconds>(tp));
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(localTime.get_local_time().time_since_epoch()) % 1000;
    return std::format("{:%Y-%m-%d %H:%M:%S}.{:03}", localTime.get_local_time(), ms.count());
}

}

namespace Chess {

LogFile::LogFile(GameClient &gameClient) : m_gameClient(gameClient) {
    std::filesystem::path logPath = std::filesystem::current_path() / "Client" / "logs" / formatTimePointSeconds(std::chrono::system_clock::now());
    std::filesystem::create_directories(logPath.parent_path());
    m_file.open(logPath, std::ios::out | std::ios::app);
    if (!m_file.is_open()) { throw std::runtime_error("FATAL! Can not open Logging File");}

    std::lock_guard<std::mutex> lock(m_mutex);
    m_file << "Start of Log\n";

    m_handlerID = m_gameClient.loggingManager().addHandler(LogType::LOG_ALL, [this](const LogEntry& entry) {
        std::string formatted = std::format("[{}][{}] {}", formatHHMMSS(m_gameClient.uptimeAtPoint(entry.timestamp)), logTypeAsString(entry.type), entry.message);
        std::lock_guard<std::mutex> lock(m_mutex);
        m_file << formatted << std::endl;
    });
}

LogFile::~LogFile() {
    m_gameClient.loggingManager().removeHandler(m_handlerID);
    if (m_file.is_open()) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_file << "End of Log\n";
    }
}
}
