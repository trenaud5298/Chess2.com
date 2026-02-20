#ifndef CHESS_CORE_NETWORKING_MESSAGE_HPP
#define CHESS_CORE_NETWORKING_MESSAGE_HPP

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
#include <string>
#include <cstdint>
#include <vector>

namespace Chess {

enum MessageType : std::uint8_t {
    NONE = 0,
    CHAT = 1,
};

struct MessageHeader {
    std::uint32_t validation;
    std::uint32_t bodyLength;
    std::uint32_t type;
    std::uint32_t reserved;
};


class Message {

public:
    explicit Message(MessageType type, std::size_t bodySize);

    // Data Access & Info
    [[nodiscard]] std::uint8_t* data();
    [[nodiscard]] const std::uint8_t *data() const;
    [[nodiscard]] std::uint8_t* body();
    [[nodiscard]] const std::uint8_t* body() const;

    // Size Returns Used Bytes
    [[nodiscard]] std::size_t size() const;
    [[nodiscard]] std::size_t bodySize() const;

    // Body Capacity Returns Total Body Capacity Including Unused Memory
    [[nodiscard]] std::size_t bodyCapacity() const;

    // Memory Management
    void changeCapacity(std::size_t newCapacity);

    // Data Write
    void writeHeader(MessageHeader& h);
    void writeBody(void* src, std::size_t size, std::size_t offset = 0);
    template <typename T>
    requires std::is_trivially_copyable_v<T>
    void push(T& value) { writeBody(&value, sizeof(value), m_pushOffset); m_pushOffset += sizeof(T); }

    // Data Read
    [[nodiscard]] MessageHeader readHeader() const;
    [[nodiscard]] std::uint8_t* safeRead(std::size_t size, std::size_t offset) const;

    template <typename T>
    requires std::is_trivially_copyable_v<T>
    [[nodiscard]] const T& readAs(std::size_t offset = 0) const { return *reinterpret_cast<T*>(safeRead(sizeof(T), offset));}


private:
    std::uint32_t m_pushOffset;
    std::vector<std::uint8_t> m_data;
    constexpr static std::uint32_t MESSAGE_HEADER_SIZE = sizeof(std::uint32_t);
};


}


#endif