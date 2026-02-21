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
#include <asio/buffer.hpp>

// C++ Includes
#include <cstdint>
#include <vector>
#include <cstring>
#include <type_traits>
#include <stdexcept>

namespace Chess {

enum MessageType : std::uint32_t {
    NONE = 0,
    CHAT = 1,
};

struct MessageHeader {
    std::uint32_t validation;
    std::uint32_t bodyLength;
    std::uint32_t type;
    std::uint32_t reserved;
};
static_assert(sizeof(MessageHeader) == 16);

class Message {

public:
    explicit Message(MessageType type);

    // Header Access
    [[nodiscard]] MessageHeader& header() noexcept;
    [[nodiscard]] const MessageHeader& header() const noexcept;
    [[nodiscard]] MessageHeader* headerData() noexcept;
    [[nodiscard]] const MessageHeader* headerData() const noexcept;
    [[nodiscard]] static std::size_t headerSize() noexcept;
    [[nodiscard]] MessageType type() const noexcept;
    [[nodiscard]] bool validateHeader() const noexcept;

    // Body Access
    [[nodiscard]] std::uint8_t* bodyData() noexcept;
    [[nodiscard]] const std::uint8_t* bodyData() const noexcept;
    [[nodiscard]] std::size_t bodySize() const noexcept;
    [[nodiscard]] bool empty() const noexcept;

    // Size Related Functions
    void reserve(std::size_t size);
    void resize(std::size_t size);
    [[nodiscard]] std::size_t capacity() const noexcept;  // Body Capacity
    [[nodiscard]] std::size_t totalSize() const noexcept; // Total Size

    // Clear/Reset
    void clear() noexcept;
    void readReset() noexcept;
    void readSet(std::size_t offset);

    // Buffers
    [[nodiscard]] asio::mutable_buffer headerBuffer() noexcept;
    [[nodiscard]] asio::const_buffer headerBuffer() const noexcept;
    [[nodiscard]] asio::mutable_buffer bodyBuffer() noexcept;
    [[nodiscard]] asio::const_buffer bodyBuffer() const noexcept;
    [[nodiscard]] std::array<asio::mutable_buffer, 2> buffers() noexcept;
    [[nodiscard]] std::array<asio::const_buffer, 2> buffers() const noexcept;

    // Push
    template <typename T>
    requires std::is_trivially_copyable_v<T>
    void push(const T& value) {
        pushBytes(&value, sizeof(T));
    }
    void pushBytes(const void* data, std::size_t size);
    void pushString(const std::string& str);

    // Read
    template <typename T>
    requires std::is_trivially_copyable_v<T>
    T read() {
        if (m_readOffset + sizeof(T) > m_body.size()) {
            throw std::runtime_error("Message read overflow");
        }
        T value;
        std::memcpy(&value, m_body.data() + m_readOffset, sizeof(T));
        m_readOffset += sizeof(T);
        return value;
    }

    void readBytes(void* dest, std::size_t size);
    std::string readString();



private:
    MessageHeader m_header;
    std::vector<std::uint8_t> m_body;
    std::size_t m_readOffset;

    constexpr static std::uint32_t MAX_BODY_SIZE = 64 * 1024;
    constexpr static std::uint32_t PROTOCOL_VALIDATION = 0x43485353;
};


}


#endif