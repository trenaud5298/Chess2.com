/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <stdexcept>
#include <Chess/Core/Networking/Message.hpp>

// ASIO Includes

// C++ Includes
#include <bit>

namespace {


__forceinline [[nodiscard]] std::uint32_t toBigEndian(std::uint32_t value) noexcept {
    std::uint32_t result = value;
    if constexpr (std::endian::native == std::endian::little) {
        result = ((0xFF000000 & value) >> 24) | ((0x00FF0000 & value) >> 8)  |  ((0x0000FF00 & value) << 8)  | ((0x000000FF & value) << 24);
    }
    return result;
}

std::uint32_t toLittleEndian(std::uint32_t value) {

}

std::uint32_t fromBigEndian(std::uint32_t value) {

}

std::uint32_t fromLittleEndian(std::uint32_t value) {

}


}


namespace Chess {

Message::Message(MessageType type, std::string data)  {
    if (data.size() > Message::MAX_MESSAGE_LENGTH) {
        throw std::runtime_error("Message Exceeded Max Length Of: " + Message::MAX_MESSAGE_LENGTH);
    }
    m_header = {Message::PROTOCOL_VALIDATION, static_cast<std::uint32_t>(data.size()), type, 0};
    m_data = std::vector<std::uint8_t>((std::uint8_t*)data.c_str(), (std::uint8_t*)data.c_str() + data.size());
}


Message::Message(MessageType type, std::vector<std::uint32_t> data) {
    if (data.size() > Message::MAX_MESSAGE_LENGTH) {
        throw std::runtime_error("Message Exceeded Max Length Of: " + Message::MAX_MESSAGE_LENGTH);
    }
    m_header = {Message::PROTOCOL_VALIDATION, static_cast<std::uint32_t>(data.size()), type, 0};
    m_data=std::move(data);
}


}
