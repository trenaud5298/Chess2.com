/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Server/Target/TargetAll.h>

// ASIO Includes

// C++ Includes

namespace Chess {

//--------------------------------------------------------------------
// Target::All
//--------------------------------------------------------------------
Target::All::Range Target::All::range(const std::vector<std::uint32_t>& idToSession, const std::vector<std::shared_ptr<Session>>& sessions) const {
    return Range(sessions);
}



//--------------------------------------------------------------------
// Target::All::Range
//--------------------------------------------------------------------

Target::All::Range::Range(const std::vector<std::shared_ptr<Session>>& sessions) : m_sessions(sessions) {}

Target::All::Iterator Target::All::Range::begin() const noexcept {
    return Iterator(m_sessions, 0);
}

Target::All::Iterator Target::All::Range::end() const noexcept {
    return Iterator(m_sessions, m_sessions.size());
}




//--------------------------------------------------------------------
// Target::All::Iterator
//--------------------------------------------------------------------

Target::All::Iterator::Iterator(const std::vector<std::shared_ptr<Session>>& sessions, std::size_t index) : m_sessions(sessions), m_index(index) {}

Session& Target::All::Iterator::operator*() const {
    return *m_sessions[m_index];
}

Session* Target::All::Iterator::operator->() const {
    return m_sessions[m_index].get();
}

Target::All::Iterator& Target::All::Iterator::operator++() noexcept {
    ++m_index;
    return *this;
}

Target::All::Iterator Target::All::Iterator::operator++(int) noexcept {
    Iterator temp = *this;
    ++(*this);
    return temp;
}

bool Target::All::Iterator::operator==(const Iterator& other) const noexcept {
    return m_index == other.m_index;
}

bool Target::All::Iterator::operator!=(const Iterator& other) const noexcept {
    return !(*this == other);
}

} // namespace Chess
