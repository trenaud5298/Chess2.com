#ifndef CHESS_CLIENT_RUNTIME_PERSISTENCE_LOGFILE_HPP
#define CHESS_CLIENT_RUNTIME_PERSISTENCE_LOGFILE_HPP

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
#include <filesystem>
#include <mutex>
#include <fstream>

namespace Chess {

class GameClient;

class LogFile {
public:
    LogFile(GameClient& gameClient);
    ~LogFile();

    LogFile(const LogFile&) = delete;
    LogFile& operator=(const LogFile&) = delete;
    LogFile(LogFile&&) = delete;
    LogFile& operator=(LogFile&&) = delete;

private:
    GameClient& m_gameClient;
    std::ofstream m_file;
    std::mutex m_mutex;
    std::uint64_t m_handlerID{0};
};

}

#endif