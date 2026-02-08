#ifndef CHESS_SERVER_TARGET_TARGETID_H
#define CHESS_SERVER_TARGET_TARGETID_H

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

class Target::Id {
public:
    class Iterator;
    class Range;

    explicit Id(std::vector<std::uint32_t> targetIds) noexcept;

    Range range(const std::vector<std::uint32_t>& idToSessionIndex, const std::vector<std::shared_ptr<Session>>& sessions) const;

    template <typename Func>
    requires std::invocable<Func, Session&>
    void forEach(const std::vector<std::uint32_t>& idToSessionIndex, const std::vector<std::shared_ptr<Session>>& sessions, Func&& func) const {
        const std::size_t idMapSize   = idToSessionIndex.size();
        const std::size_t sessionSize = sessions.size();
        Func&& f = std::forward<Func>(func);

        for (const std::uint32_t targetId : m_targetIds) {
            if (targetId >= idMapSize) continue;

            const std::uint32_t sessionIndex = idToSessionIndex[targetId];
            if (sessionIndex >= sessionSize) continue;

            std::invoke(f, *sessions[sessionIndex]);
        }
    }
private:
    const std::vector<std::uint32_t> m_targetIds;
};


class Target::Id::Range {
public:
    Range(const std::vector<std::uint32_t>& targetIds, const std::vector<std::uint32_t>& idToSessionIndex, const std::vector<std::shared_ptr<Session>>& sessions);
    Iterator begin() const noexcept;
    Iterator end() const noexcept;
private:
    const std::vector<std::uint32_t>& m_targetIds;
    const std::vector<std::uint32_t>& m_idToSessionIndex;
    const std::vector<std::shared_ptr<Session>>& m_sessions;
};



class Target::Id::Iterator {
public:
    Iterator(const std::vector<std::uint32_t>& targetIds, const std::vector<std::uint32_t>& idToSessionIndex, const std::vector<std::shared_ptr<Session>>& sessions, std::size_t index);

    Session& operator*() const;
    Session* operator->() const;
    Iterator& operator++() noexcept;
    Iterator operator++(int) noexcept;
    bool operator==(const Iterator& other) const noexcept;
    bool operator!=(const Iterator& other) const noexcept;

private:
    void skipInvalid() noexcept;

    const std::vector<std::uint32_t>& m_targetIds;
    const std::vector<std::uint32_t>& m_idToSessionIndex;
    const std::vector<std::shared_ptr<Session>>& m_sessions;
    std::size_t m_index;
    std::size_t m_sessionIndex;
};
}

#endif
