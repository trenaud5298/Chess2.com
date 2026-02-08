/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Server/Target/TargetPredicate.h>

// ASIO Includes

// C++ Includes

namespace Chess {

//--------------------------------------------------------------------
// Target::Predicate
//--------------------------------------------------------------------

Target::Predicate::Predicate(std::function<bool(const Session&)> predicate) noexcept
    : m_predicate(std::move(predicate)) {}

Target::Predicate::Range Target::Predicate::range(const std::vector<std::uint32_t>& idsToSession, const std::vector<std::shared_ptr<Session>>& sessions) const {
    return Range(m_predicate, sessions);
}

//--------------------------------------------------------------------
// Target::Predicate::Range
//--------------------------------------------------------------------

Target::Predicate::Range::Range(const std::function<bool(const Session&)>& predicate, const std::vector<std::shared_ptr<Session>>& sessions) noexcept
    : m_predicate(predicate)
    , m_sessions(sessions) {}

Target::Predicate::Iterator Target::Predicate::Range::begin() const noexcept {
    return Iterator(m_predicate, m_sessions, 0);
}

Target::Predicate::Iterator Target::Predicate::Range::end() const noexcept {
    return Iterator(m_predicate, m_sessions, m_sessions.size());
}

//--------------------------------------------------------------------
// Target::Predicate::Iterator
//--------------------------------------------------------------------

Target::Predicate::Iterator::Iterator(const std::function<bool(const Session&)>& predicate, const std::vector<std::shared_ptr<Session>>& sessions, std::size_t index) noexcept
    : m_predicate(predicate)
    , m_sessions(sessions)
    , m_index(index)
{
    skipInvalid();
}

Session& Target::Predicate::Iterator::operator*() const {
    return *m_sessions[m_index];
}

Session* Target::Predicate::Iterator::operator->() const {
    return m_sessions[m_index].get();
}

Target::Predicate::Iterator& Target::Predicate::Iterator::operator++() noexcept {
    ++m_index;
    skipInvalid();
    return *this;
}

Target::Predicate::Iterator Target::Predicate::Iterator::operator++(int) noexcept {
    Iterator temp = *this;
    ++(*this);
    return temp;
}

bool Target::Predicate::Iterator::operator==(const Iterator& other) const noexcept {
    return m_index == other.m_index;
}

bool Target::Predicate::Iterator::operator!=(const Iterator& other) const noexcept {
    return !(*this == other);
}

void Target::Predicate::Iterator::skipInvalid() noexcept {
    while (m_index < m_sessions.size()) {
        if (m_predicate(*m_sessions[m_index])) {
            break;
        }
        m_index++;
    }
}

} // namespace Chess
