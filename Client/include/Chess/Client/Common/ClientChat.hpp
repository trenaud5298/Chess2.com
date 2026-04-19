#ifndef CHESS_CLIENT_COMMON_CLIENTCHAT_HPP
#define CHESS_CLIENT_COMMON_CLIENTCHAT_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// C++ Includes
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include <mutex>

namespace Chess {

struct ClientChatEntry {
    std::uint64_t sequence{0};
    std::string text;
};

class ClientChatLog {

public:
    explicit ClientChatLog(std::size_t maxEntries = 500)
    : m_maxEntries(std::max<std::size_t>(1, maxEntries)) {}

    [[nodiscard]] std::size_t size() const noexcept {
        return m_entries.size();
    }

    [[nodiscard]] bool empty() const noexcept {
        return m_entries.empty();
    }

    [[nodiscard]] std::size_t maxEntries() const noexcept {
        return m_maxEntries;
    }

    void clear() noexcept {
        m_entries.clear();
    }

    [[nodiscard]] std::uint64_t nextSequence() const noexcept {
        return m_nextSequence;
    }

    [[nodiscard]] std::optional<std::uint64_t> firstSequence() const noexcept {
        if (m_entries.empty()) {
            return std::nullopt;
        }
        return m_entries.front().sequence;
    }

    [[nodiscard]] std::optional<std::uint64_t> lastSequence() const noexcept {
        if (m_entries.empty()) {
            return std::nullopt;
        }
        return m_entries.back().sequence;
    }

    [[nodiscard]] std::optional<std::size_t> indexOfSequence(std::uint64_t sequence) const noexcept {
        if (m_entries.empty()) {
            return std::nullopt;
        }

        std::uint64_t firstSequence = m_entries.front().sequence;
        std::size_t estimatedIndex = static_cast<std::size_t>(sequence - firstSequence);
        if (estimatedIndex >= m_entries.size()) {
            return std::nullopt;
        }

        if (m_entries[estimatedIndex].sequence == sequence) {
            return estimatedIndex;
        }

        // If Estimated Index Fails, We Will Assume It Does Not Exist.
        return std::nullopt;
    }

    std::uint64_t append(ClientChatEntry entry) {
        entry.sequence = m_nextSequence++;
        m_entries.push_back(std::move(entry));
        if (m_entries.size() > m_maxEntries) {
            m_entries.pop_front();
        }
        return m_entries.empty() ? 0 : m_entries.back().sequence;
    }

    [[nodiscard]] std::optional<ClientChatEntry> at(std::size_t index) const {
        if (index >= m_entries.size()) {
            return std::nullopt;
        }
        return m_entries[index];
    }

    [[nodiscard]] std::vector<ClientChatEntry> slice(std::size_t start, std::size_t count) const {
        if (start >= m_entries.size() || count == 0) {
            return {};
        }

        std::size_t end = std::min(start + count, m_entries.size());

        std::vector<ClientChatEntry> output;
        output.reserve(end - start);

        for (std::size_t i = start; i < end; ++i) {
            output.push_back(m_entries[i]);
        }

        return output;
    }

    [[nodiscard]] std::vector<ClientChatEntry> sliceFromSequence(std::uint64_t firstSequence, std::size_t count) const {
        std::optional<std::size_t> start = indexOfSequence(firstSequence);
        if (!start.has_value()) {
            return {};
        }
        return slice(*start, count);
    }



    [[nodiscard]] std::vector<ClientChatEntry> tail(std::size_t count) const {
        if (count == 0 || m_entries.empty()) {
            return {};
        }

        std::size_t start = (count >= m_entries.size()) ? 0 : (m_entries.size() - count);
        return slice(start, m_entries.size()-start);
    }


private:
    std::deque<ClientChatEntry> m_entries;
    std::size_t m_maxEntries{500};
    std::uint64_t m_nextSequence{1};

};







// Thread Safe Version Of Client Chat Log With Mutex Protection
class ThreadSafeClientChatLog {

public:
    explicit ThreadSafeClientChatLog(std::size_t maxEntries = 500)
    : m_log(maxEntries) {}

    [[nodiscard]] std::size_t size() const noexcept {
        std::lock_guard lock(m_mutex);
        return m_log.size();
    }

    [[nodiscard]] bool empty() const noexcept {
        std::lock_guard lock(m_mutex);
        return m_log.empty();
    }

    [[nodiscard]] std::size_t maxEntries() const noexcept {
        std::lock_guard lock(m_mutex);
        return m_log.maxEntries();
    }

    void clear() noexcept {
        std::lock_guard lock(m_mutex);
        return m_log.clear();
    }

    [[nodiscard]] std::uint64_t nextSequence() const noexcept {
        std::lock_guard lock(m_mutex);
        return m_log.nextSequence();
    }

    [[nodiscard]] std::optional<std::uint64_t> firstSequence() const noexcept {
        std::lock_guard lock(m_mutex);
        return m_log.firstSequence();
    }

    [[nodiscard]] std::optional<std::uint64_t> lastSequence() const noexcept {
        std::lock_guard lock(m_mutex);
        return m_log.lastSequence();
    }

    [[nodiscard]] std::optional<std::size_t> indexOfSequence(std::uint64_t sequence) const noexcept {
        std::lock_guard lock(m_mutex);
        return m_log.indexOfSequence(sequence);
    }

    std::uint64_t append(ClientChatEntry entry) {
        std::lock_guard lock(m_mutex);
        return m_log.append(std::move(entry));
    }

    [[nodiscard]] std::optional<ClientChatEntry> at(std::size_t index) const {
        std::lock_guard lock(m_mutex);
        return m_log.at(index);
    }

    [[nodiscard]] std::vector<ClientChatEntry> slice(std::size_t start, std::size_t count) const {
        std::lock_guard lock(m_mutex);
        return m_log.slice(start, count);
    }

    [[nodiscard]] std::vector<ClientChatEntry> sliceFromSequence(std::uint64_t firstSequence, std::size_t count) const {
        std::lock_guard lock(m_mutex);
        return m_log.sliceFromSequence(firstSequence, count);
    }

    [[nodiscard]] std::vector<ClientChatEntry> tail(std::size_t count) const {
        std::lock_guard lock(m_mutex);
        return m_log.tail(count);
    }

private:
    mutable std::mutex m_mutex;
    ClientChatLog m_log;

};


}


#endif