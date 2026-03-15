#ifndef CHESS_SERVER_RUNTIME_LOGGING_LOGHANDLER_HPP
#define CHESS_SERVER_RUNTIME_LOGGING_LOGHANDLER_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Server/Runtime/Logging/LogEntry.hpp>

// ASIO Includes

// C++ Includes
#include <cstdint>
#include <functional>

namespace Chess {

struct LogHandler {
    std::uint64_t m_id;
    std::uint64_t m_mask;
    std::function<void(const LogEntry&)> m_callback;

    explicit LogHandler(std::uint64_t id, std::uint64_t mask, std::function<void(const LogEntry&)> callback) : m_id(id), m_mask(mask), m_callback(std::move(callback)) {}

    void dispatch(const LogEntry& entry) const {
        if (m_mask & entry.type) {
            m_callback(entry);
        }
    }
};

}

#endif