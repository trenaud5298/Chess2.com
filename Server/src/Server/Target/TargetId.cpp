/*
* Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Server/Target/TargetId.h>

// ASIO Includes

// C++ Includes


namespace Chess {

//--------------------------------------------------------------------
// Target::Id
//--------------------------------------------------------------------

Target::Id::Id(std::vector<std::uint32_t> targetIds) noexcept
    : m_targetIds(std::move(targetIds)) {}

Target::Id::Range Target::Id::range(const std::vector<std::uint32_t>& idToSessionIndex, const std::vector<std::shared_ptr<Session>>& sessions) const {
    return Range(m_targetIds, idToSessionIndex, sessions);
}

//--------------------------------------------------------------------
// Target::Id::Range
//--------------------------------------------------------------------

Target::Id::Range::Range(const std::vector<std::uint32_t>& targetIds, const std::vector<std::uint32_t>& idToSessionIndex, const std::vector<std::shared_ptr<Session>>& sessions)
    : m_targetIds(targetIds)
    , m_idToSessionIndex(idToSessionIndex)
    , m_sessions(sessions) {}

Target::Id::Iterator Target::Id::Range::begin() const noexcept {
    return Iterator(m_targetIds, m_idToSessionIndex, m_sessions, 0);
}

Target::Id::Iterator Target::Id::Range::end() const noexcept {
    return Iterator(m_targetIds, m_idToSessionIndex, m_sessions, m_targetIds.size());
}

//--------------------------------------------------------------------
// Target::Id::Iterator
//--------------------------------------------------------------------

Target::Id::Iterator::Iterator(const std::vector<std::uint32_t>& targetIds, const std::vector<std::uint32_t>& idToSessionIndex, const std::vector<std::shared_ptr<Session>>& sessions, std::size_t index)
    : m_targetIds(targetIds)
    , m_idToSessionIndex(idToSessionIndex)
    , m_sessions(sessions)
    , m_index(index) {skipInvalid();}

Session& Target::Id::Iterator::operator*() const {
    return *m_sessions[m_sessionIndex];
}

Session* Target::Id::Iterator::operator->() const {
    return m_sessions[m_sessionIndex].get();
}

Target::Id::Iterator& Target::Id::Iterator::operator++() noexcept {
    m_index++;
    skipInvalid();
    return *this;
}

Target::Id::Iterator Target::Id::Iterator::operator++(int) noexcept {
    Iterator temp = *this;
    ++(*this);
    return temp;
}

bool Target::Id::Iterator::operator==(const Iterator& other) const noexcept {
    return m_index == other.m_index;
}

bool Target::Id::Iterator::operator!=(const Iterator& other) const noexcept {
    return !(*this == other);
}

void Target::Id::Iterator::skipInvalid() noexcept {
    while (m_index < m_targetIds.size()) {
        const std::uint32_t targetId = m_targetIds[m_index];
        if (targetId >= m_idToSessionIndex.size()) {
            m_index++;
            continue;
        }

        m_sessionIndex = m_idToSessionIndex[targetId];
        if (m_sessionIndex >= m_sessions.size()) {
            m_index++;
            continue;
        }

        // Valid
        break;
    }

}

} // namespace Chess
