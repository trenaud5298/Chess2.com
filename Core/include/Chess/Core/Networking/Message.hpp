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

// Note: I used AI to assist with generating the below summary
// for MessageType and refined it to match my protocol.

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
 * - std::string    -> [uint32_t length][char data...]
 * - std::vector<T> -> [uint32_t count][T item1][T item2]...[T itemN]
 * - bool / enums   -> Raw byte form of the underlying type
 * - Integers       -> Raw byte form (Little-Endian)
 * - Durations      -> int64_t millisecond count
 * - Arrays         -> Serialized element-by-element in fixed order
 * - All fields     -> Tightly packed and ordered (no padding)
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
 *   - std::string password
 *
 * @c MessageType::LoginResponse        (2)  | Direction: Server -> Client
 *   - bool accepted
 *   - std::string reason
 *
 * @c MessageType::Chat                 (3)  | Direction: Both
 *   - ChatScope scope
 *   - std::string message
 *
 * @c MessageType::Command              (4)  | Direction: Client -> Server
 *   - std::string command
 *
 * @c MessageType::CreateRoomRequest    (5)  | Direction: Client -> Server
 *   - No payload
 *
 * @c MessageType::CreateRoomResponse   (6)  | Direction: Server -> Client
 *   - bool success
 *   - RoomID roomID
 *   - RoomMemberType memberType
 *   - COLOR color
 *   - std::string reason
 *
 * @c MessageType::ListRoomsRequest     (7)  | Direction: Client -> Server
 *   - No payload
 *
 * @c MessageType::ListRoomsResponse    (8)  | Direction: Server -> Client
 *   - uint32_t roomCount
 *   - Repeated RoomSummary:
 *     - RoomID roomID
 *     - std::string whitePlayerName
 *     - std::string blackPlayerName
 *     - std::uint16_t spectatorCount
 *     - bool hasOpenPlayerSeat
 *     - bool inProgress
 *
 * @c MessageType::JoinRoomRequest      (9)  | Direction: Client -> Server
 *   - RoomID roomID
 *   - bool spectator
 *
 * @c MessageType::JoinRoomResponse     (10) | Direction: Server -> Client
 *   - bool success
 *   - RoomID roomID
 *   - RoomMemberType memberType
 *   - COLOR color
 *   - std::string reason
 *
 * @c MessageType::LeaveRoomRequest     (11) | Direction: Client -> Server
 *   - No payload
 *
 * @c MessageType::LeaveRoomResponse    (12) | Direction: Server -> Client
 *   - bool success
 *   - RoomID roomID
 *   - std::string reason
 *
 * @c MessageType::MakeMove             (13) | Direction: Client -> Server
 *   - std::uint8_t from
 *   - std::uint8_t to
 *   - PromotionPiece promotion
 *
 * @c MessageType::GameUpdate           (14) | Direction: Server -> Client
 *   - RoomID roomID
 *   - std::uint64_t roomVersion
 *   - std::string whitePlayerName
 *   - std::string blackPlayerName
 *   - std::uint16_t spectatorCount
 *   - ChessGameSnapshot snapshot:
 *     - ID board[64]
 *     - ChessGameState state
 *     - COLOR currentTurn
 *     - COLOR winner
 *     - ChessGameEndReason endReason
 *     - bool clockEnabled
 *     - int64_t initialTimeMs
 *     - int64_t incrementMs
 *     - int64_t whiteTimeRemainingMs
 *     - int64_t blackTimeRemainingMs
 *     - std::uint64_t version
 *
 * @c MessageType::ErrorMessage         (15) | Direction: Both
 *   - std::uint32_t errorCode
 *   - std::string message
 *
 * ---------------------------------------------------------------------------
 *
 * @warning Any mismatch in ordering or type will result in protocol failure.
 *
 * @remark The receiver MUST deserialize fields in the exact order listed.
 * @remark This enum is tightly coupled with MessagePayloads serialization logic.
 * @remark Request/response pairs should be handled together at the application level.
 */
enum class MessageType : std::uint16_t {
    None,
    LoginRequest,
    LoginResponse,
    Chat,
    Command,
    CreateRoomRequest,
    CreateRoomResponse,
    ListRoomsRequest,
    ListRoomsResponse,
    JoinRoomRequest,
    JoinRoomResponse,
    LeaveRoomRequest,
    LeaveRoomResponse,
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
        case MessageType::ListRoomsRequest: return "ListRoomsRequest";
        case MessageType::ListRoomsResponse: return "ListRoomsResponse";
        case MessageType::LeaveRoomRequest: return "LeaveRoomRequest";
        case MessageType::LeaveRoomResponse: return "LeaveRoomResponse";
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