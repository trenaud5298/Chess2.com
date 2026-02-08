#ifndef CHESS_SERVER_TARGET_TARGETPREDICATE_H
#define CHESS_SERVER_TARGET_TARGETPREDICATE_H

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
#include <functional>

namespace Chess {

class Session;




class Target::Predicate {
public:
    class Iterator;
    class Range;

    explicit Predicate(std::function<bool(const Session&)> predicate) noexcept;
    
    Range range(const std::vector<std::uint32_t>& idsToSession, const std::vector<std::shared_ptr<Session>>& sessions) const;

    template <typename Func>
    requires std::invocable<Func, Session&>
    void forEach(const std::vector<std::uint32_t>& idToSessionIndex, const std::vector<std::shared_ptr<Session>>& sessions, Func&& func) const {
        Func&& f = std::forward<Func>(func);
        const std::function<bool(const Session&)>& pred = m_predicate;
        for (const std::shared_ptr<Session>& session : sessions) {
            Session& sessionRef = *session;
            if (pred(sessionRef)) {
                std::invoke(f, sessionRef);
            }
        }
    }

private:
    std::function<bool(const Session&)> m_predicate;
};





class Target::Predicate::Range {
public:
    Range(const std::function<bool(const Session&)>& predicate,
          const std::vector<std::shared_ptr<Session>>& sessions) noexcept;

    Iterator begin() const noexcept;
    Iterator end() const noexcept;

private:
    const std::function<bool(const Session&)>& m_predicate;
    const std::vector<std::shared_ptr<Session>>& m_sessions;
};




class Target::Predicate::Iterator {
public:
    Iterator(const std::function<bool(const Session&)>& predicate,
             const std::vector<std::shared_ptr<Session>>& sessions,
             std::size_t index) noexcept;

    Session& operator*() const;
    Session* operator->() const;
    Iterator& operator++() noexcept;
    Iterator operator++(int) noexcept;
    bool operator==(const Iterator& other) const noexcept;
    bool operator!=(const Iterator& other) const noexcept;

private:
    void skipInvalid() noexcept;
    const std::function<bool(const Session&)>& m_predicate;
    const std::vector<std::shared_ptr<Session>>& m_sessions;
    std::size_t m_index;
};
} // namespace Chess

#endif
