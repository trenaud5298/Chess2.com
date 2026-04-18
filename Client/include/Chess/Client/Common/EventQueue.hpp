#ifndef CHESS_CLIENT_COMMON_EVENTQUEUE_HPP
#define CHESS_CLIENT_COMMON_EVENTQUEUE_HPP


/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/Common/ClientEvent.hpp>

// C++ Includes
#include <cstddef>
#include <mutex>
#include <utility>
#include <vector>


namespace Chess {

template <typename EventType>
class EventQueue {
public:
    void push(EventType event) {
        std::lock_guard lock(m_mutex);
        m_queue.push_back(std::move(event));
    }

    [[nodiscard]] std::vector<EventType> drain() {
        std::lock_guard lock(m_mutex);
        std::vector<EventType> out;
        out.swap(m_queue);
        return out;
    }

    [[nodiscard]] bool empty() const {
        std::lock_guard lock(m_mutex);
        return m_queue.empty();
    }

    [[nodiscard]] std::size_t size() const {
        std::lock_guard lock(m_mutex);
        return m_queue.size();
    }

    void clear() {
        std::lock_guard lock(m_mutex);
        m_queue.clear();
    }

private:
    mutable std::mutex m_mutex;
    std::vector<EventType> m_queue;
};

}


#endif