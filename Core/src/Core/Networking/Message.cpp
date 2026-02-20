/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Core/Networking/Message.hpp>

// ASIO Includes

// C++ Includes



namespace Chess {


Message::Message(MessageType type)
: m_readOffset(0) {
    m_header.validation = PROTOCOL_VALIDATION;
    m_header.bodyLength = 0;
    m_header.type = static_cast<std::uint32_t>(type);
    m_header.reserved = 0;
}

// Header Access
MessageHeader& Message::header() noexcept { return m_header; }
const MessageHeader& Message::header() const noexcept { return m_header; }
MessageType Message::type() const noexcept { return static_cast<MessageType>(m_header.type); }

// Body
std::uint8_t* Message::bodyData() noexcept { return m_body.data(); }
const std::uint8_t* Message::bodyData() const noexcept { return m_body.data(); }
std::size_t Message::bodySize() const noexcept { return m_body.size(); }
bool Message::empty() const noexcept { return m_body.empty(); }

// Size Related
void Message::reserve(std::size_t size) { m_body.reserve(size); }
void Message::resize(std::size_t size) {
    if (size > MAX_BODY_SIZE) {
        throw std::runtime_error("Message resize overflow");
    }
    m_body.resize(size);
    m_header.bodyLength = static_cast<std::uint32_t>(size);
}
std::size_t Message::capacity() const noexcept { return m_body.capacity(); }
std::size_t Message::totalSize() const noexcept { return sizeof(MessageHeader) + m_body.size(); }

// Clear and Reset
void Message::clear() noexcept {
    m_body.clear();
    m_readOffset = 0;
    m_header.bodyLength = 0;
}
void Message::readReset() noexcept { m_readOffset = 0; }
void Message::readSet(std::size_t offset) {
    if (offset > m_body.size()) {
        throw std::runtime_error("Message read overflow");
    }
    m_readOffset = offset;
}

// Push
void Message::pushBytes(const void* data, std::size_t size) {
    if (size == 0) { return; }

    const std::size_t oldSize = m_body.size();
    m_body.resize(oldSize + size);
    std::memcpy(m_body.data() + oldSize, data, size);
}

void Message::pushString(const std::string& str) {
    const std::uint32_t length = static_cast<std::uint32_t>(str.size());
    push(length);
    pushBytes(str.data(), str.size());
}

// Read
void Message::readBytes(void* dest, std::size_t size) {
    if (size > m_body.size() - m_readOffset)
        throw std::runtime_error("Message read overflow");

    std::memcpy(dest, m_body.data() + m_readOffset, size);
    m_readOffset += size;
}

std::string Message::readString() {
    const std::uint32_t length = read<std::uint32_t>();
    if (length > m_body.size() - m_readOffset) {
        throw std::runtime_error("Message read overflow");
    }

    std::string result(reinterpret_cast<const char*>(m_body.data() + m_readOffset), length);
    m_readOffset += length;
    return result;
}

} // namespace Chess