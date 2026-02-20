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




enum MessageType : std::uint32_t {
    UNKNOWN = 0,
    CHAT = 1
};


class Message {

public:

    struct Header {
        std::uint32_t validation;
        std::uint32_t bodyLength;
        std::uint32_t type;
        std::uint32_t reserved;
    };

    static constexpr std::uint32_t PROTOCOL_VALIDATION = 0x43485353;
    static constexpr std::uint32_t MAX_BODY_LENGTH = 64*1024;
    static constexpr std::uint32_t HEADER_SIZE = sizeof(Header);

    explicit Message(const MessageType type)
    : m_buffer(HEADER_SIZE), m_readOffset(0), m_writeOffset(0) {
        header().validation = PROTOCOL_VALIDATION;
        header().bodyLength = 0;
        header().type       = static_cast<std::uint32_t>(type);
        header().reserved   = 0;
    }

    // Raw Data Access
    [[nodiscard]] std::uint8_t* data() { return m_buffer.data(); }
    [[nodiscard]] const std::uint8_t* data() const { return m_buffer.data(); }
    [[nodiscard]] std::uint8_t* body() { return m_buffer.data() + HEADER_SIZE; }
    [[nodiscard]] const std::uint8_t* body() const { return m_buffer.data() + HEADER_SIZE; }
    [[nodiscard]] Header& header() { return *reinterpret_cast<Header*>(m_buffer.data()); }
    [[nodiscard]] const Header& header() const { return *reinterpret_cast<const Header*>(m_buffer.data()); }
    [[nodiscard]] std::size_t size() const { return m_buffer.size(); }
    [[nodiscard]] std::size_t bodySize() const { return m_buffer.size() - HEADER_SIZE; }

    // Memory Management
    void ensureWriteCapacity(std::size_t offset, std::size_t length) {
        const std::size_t sizeNeeded = HEADER_SIZE + offset + length;
        if (sizeNeeded - HEADER_SIZE > MAX_BODY_LENGTH) {
            throw std::runtime_error("Message would exceed maximum size");
        }
        if (sizeNeeded > m_buffer.size()) {
            m_buffer.resize(sizeNeeded);
        }
    }


    // Validation
    [[nodiscard]] bool validateHeader() {
        const Header& h = header();
        if (h.validation != PROTOCOL_VALIDATION) {return false;}
        if (h.bodyLength > MAX_BODY_LENGTH) {return false;}
        return true;
    }

    [[nodiscard]] bool validateBody() {
        const Header& h = header();
        if (h.bodyLength != bodySize()) {return false;}
        if (h.bodyLength > MAX_BODY_LENGTH) {return false;}
        return true;
    }

    // Writing Options
    void write(std::size_t offset, void* src, std::size_t size) {
        ensureWriteCapacity(offset, size);
        std::memcpy(m_buffer.data() + offset + HEADER_SIZE, src, size);
        header().bodyLength = bodySize();
    }

    template <typename T>
    requires std::is_trivially_copyable_v<T>
    void push(T& value) {
        wrtie(m_writeOffset, value, sizeof(T));
    }



private:
    std::vector<std::uint8_t> m_buffer;
    std::size_t m_readOffset;
    std::size_t m_writeOffset;
};


}

#endif
