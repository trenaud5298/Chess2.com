#ifndef CHESS_CLIENT_RUNTIME_CALLBACK_CALLBACKREGISTRY_HPP
#define CHESS_CLIENT_RUNTIME_CALLBACK_CALLBACKREGISTRY_HPP

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
#include <cstdint>
#include <mutex>
#include <shared_mutex>
#include <functional>
#include <unordered_map>

namespace Chess {

using SubscriptionID = uint32_t;

template<typename... Args>
class CallbackRegistry {
public:
    using Callback = std::function<void(Args...)>;

    SubscriptionID subscribe(Callback cb) {
        std::unique_lock lock(m_mutex);
        SubscriptionID id = m_nextId++;
        m_callbacks[id] = std::move(cb);
        return id;
    }

    void unsubscribe(SubscriptionID id) {
        std::unique_lock lock(m_mutex);
        m_callbacks.erase(id);
    }

    void fire(Args... args) {
        std::vector<Callback> snapshot;
        {
            std::shared_lock lock(m_mutex);
            for (auto& [id, cb] : m_callbacks) {
                snapshot.push_back(cb);
            }
        }
        for (auto& cb : snapshot) { cb(args...); }
    }

private:
    std::shared_mutex m_mutex;
    std::unordered_map<SubscriptionID, Callback> m_callbacks;
    SubscriptionID m_nextId{1};
};

}

#endif