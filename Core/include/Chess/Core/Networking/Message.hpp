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

/**
 * @enum MessageType
 * @brief Defines all message types exchanged between client and server.
 *
 * Each message type has a strictly defined payload schema. The payload must be
 * serialized and deserialized in the exact order specified below. All fields
 * are required unless explicitly stated otherwise.
 *
 * ---------------------------------------------------------------------------
 * Payload Conventions:
 * ---------------------------------------------------------------------------
 * - std::string  -> [uint32_t length][char data...]
 * - Integers     -> Raw byte form (Little-Endian)
 * - All fields   -> Tightly packed and ordered (no padding)
 *
 * ---------------------------------------------------------------------------
 * Message Definitions:
 * ---------------------------------------------------------------------------
 *
 * @c MessageType::None                 (0)  | Direction: Both
 *   - No payload
 *
 * @c MessageType::LoginRequest         (1)  | Direction: Client -> Server
 *   - std::string username
 *   - std::string serverPassword
 *
 * @c MessageType::LoginResponse        (2)  | Direction: Server -> Client
 *   - bool accepted
 *   - std::string reason
 *
 * @c MessageType::Chat                 (3)  | Direction: Both
 *   - ChatScope scope  // (Global or Game)
 *   - std::string message // ([<Timestamp>][<Sender>] <Message>)
 *
 * @c MessageType::Command              (4)  | Direction: Client -> Server
 *   - std::string command
 *
 * @c MessageType::CreateRoomRequest    (5)  | Direction: Client -> Server
 *   - std::uint64_t roomID
 *   - std::string roomPassword
 *
 * @c MessageType::CreateRoomResponse   (6)  | Direction: Server -> Client
 *   - bool success
 *   - std::uint64_t roomID
 *   - std::string reason
 *
 * @c MessageType::JoinRoomRequest      (7)  | Direction: Client -> Server
 *   - std::uint64_t roomID
 *   - std::string roomPassword
 *
 * @c MessageType::JoinRoomResponse     (8)  | Direction: Server -> Client
 *   - bool success
 *   - std::string reason
 *
 * @c MessageType::LeaveRoom            (9)  | Direction: Both
 *   - No payload
 *
 * @c MessageType::MakeMove            (10)  | Direction: Both
 *   - std::uint8_t from  // (row * 8 + col)
 *   - std::uint8_t to    // (row * 8 + col)
 *
 * @c MessageType::GameUpdate          (11)  | Direction: Server -> Client (typically)
 *   - Implementation-defined (board state, turn, clocks, etc.)
 *
 * @c MessageType::ErrorMessage        (12)  | Direction: Both
 *   - std::uint32_t errorCode
 *   - std::string message
 *
 * ---------------------------------------------------------------------------
 *
 * @warning Any mismatch in ordering or type will result in undefined behavior.
 *
 * @remark - The receiver MUST deserialize fields in the exact order listed.
 *
 * @remark - This enum is tightly coupled with the serialization/deserialization logic.
 *
 * @remark - Request/Response pairs should be handled together at the application level.
 */
enum class MessageType : std::uint16_t {
    None,
    LoginRequest,
    LoginResponse,
    Chat,
    Command,
    CreateRoomRequest,
    CreateRoomResponse,
    JoinRoomRequest,
    JoinRoomResponse,
    LeaveRoom,
    MakeMove,
    GameUpdate,
    ErrorMessage
};

constexpr std::string_view toString(MessageType type) {
    switch (type) {
        case MessageType::None: return "None";
        case MessageType::LoginRequest: return "LoginRequest";
        case MessageType::LoginResponse: return "LoginResponse";
        case MessageType::Chat: return "Chat";
        case MessageType::Command: return "Command";
        case MessageType::CreateRoomRequest: return "CreateRoomRequest";
        case MessageType::CreateRoomResponse: return "CreateRoomResponse";
        case MessageType::JoinRoomRequest: return "JoinRoomRequest";
        case MessageType::JoinRoomResponse: return "JoinRoomResponse";
        case MessageType::LeaveRoom: return "LeaveRoom";
        case MessageType::MakeMove: return "MakeMove";
        case MessageType::GameUpdate: return "GameUpdate";
        case MessageType::ErrorMessage: return "ErrorMessage";
    }
    return "";
}


struct MessageHeader {
    std::uint32_t validation;
    std::uint32_t bodyLength;
    std::uint32_t type;
    std::uint32_t messageThreadID;
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
    std::size_t getReadOffset() const noexcept;
    void setReadOffset(std::size_t offset) noexcept;

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

    template <typename T>
    requires std::is_trivially_copyable_v<T>
    bool tryRead(T& value) noexcept {
        if (m_readOffset + sizeof(T) > m_body.size()) {
            return false;
        }
        std::memcpy(&value, m_body.data() + m_readOffset, sizeof(T));
        m_readOffset += sizeof(T);
        return true;
    }

    bool tryReadString(std::string& str) noexcept;



private:
    MessageHeader m_header;
    std::vector<std::uint8_t> m_body;
    std::size_t m_readOffset;

    constexpr static std::uint32_t MAX_BODY_SIZE = 64 * 1024;
    constexpr static std::uint32_t PROTOCOL_VALIDATION = 0x43485353;
};


}


#endif