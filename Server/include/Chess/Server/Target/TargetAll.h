#ifndef CHESS_SERVER_TARGET_TARGETALL_H
#define CHESS_SERVER_TARGET_TARGETALL_H

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Server/Target/TargetBase.h>

// ASIO Includes

// C++ Includes
#include <cstdint>
#include <memory>
#include <vector>
#include <cstddef>
#include <concepts>
#include <functional>

namespace Chess {

class Session;



class Target::All {
public:
    class Iterator;
    class Range;

    All() noexcept = default;

    Range range(const std::vector<std::uint32_t>& idToSession, const std::vector<std::shared_ptr<Session>>& sessions) const;

    template <typename Func>
    requires std::invocable<Func, Session&>
    void forEach(const std::vector<std::uint32_t>& idToSessionIndex, const std::vector<std::shared_ptr<Session>>& sessions, Func&& func) const {
        Func&& f = std::forward<Func>(func);
        for (const std::shared_ptr<Session>& session : sessions) {
            std::invoke(f, *session);
        }
    }
};




class Target::All::Range {
public:
    Range(const std::vector<std::shared_ptr<Session>>& sessions);
    Iterator begin() const noexcept;
    Iterator end() const noexcept;
private:
    const std::vector<std::shared_ptr<Session>>& m_sessions;

};




class Target::All::Iterator {
public:
    Iterator(const std::vector<std::shared_ptr<Session>>& sessions, std::size_t index);

    Session& operator*() const;
    Session* operator->() const;
    Iterator& operator++() noexcept;
    Iterator operator++(int) noexcept;
    bool operator==(const Iterator& other) const noexcept;
    bool operator!=(const Iterator& other) const noexcept;

private:
    const std::vector<std::shared_ptr<Session>>& m_sessions;
    std::size_t m_index;
};
}

#endif
