/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */


// Chess Includes
#include <Chess/Client/Runtime/MultiplayerClient.hpp>
#include <Chess/Core/Networking/MessagePayloads.hpp>

// ASIO Includes
#include <asio/bind_executor.hpp>
#include <asio/ip/address.hpp>
#include <asio/post.hpp>
#include <asio/read.hpp>
#include <asio/write.hpp>

// C++ Includes
#include <system_error>
#include <utility>
#include <chrono>
#include <filesystem>
#include <format>

namespace {
    constexpr std::uint16_t DEFAULT_SERVER_PORT = 24377;
}

namespace Chess {
MultiplayerClient::MultiplayerClient(asio::io_context& context, std::function<void(ClientEvent)> emitEvent)
:m_emitEvent(std::move(emitEvent)), m_context(context), m_socket(context), m_strand(asio::make_strand(context)) {
    refreshViewSnapshot();
}


MultiplayerClient::~MultiplayerClient() {
    std::error_code ec;
    if (m_socket.is_open()) {
        m_socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
        m_socket.close(ec);
    }
}

// Commands
ClientStatus MultiplayerClient::requestConnect(const ServerInfo& serverInfo, std::string username) {
    if (state() != MultiplayerState::Idle) {
        return ClientStatus::Warning(StatusCode::InvalidState, "Multiplayer client is not idle");
    }

    if (serverInfo.ip.empty()) {
        return ClientStatus::Warning(StatusCode::InvalidArgument, "Server Ip is empty");
    }

    if (username.empty()) {
        return ClientStatus::Warning(StatusCode::InvalidArgument, "Username is empty");
    }

    asio::post(m_strand, std::bind_front(
        &MultiplayerClient::doRequestConnect, this, std::move(serverInfo), std::move(username)
    ));

    return ClientStatus::Success();
}

ClientStatus MultiplayerClient::requestDisconnect() {
    if (state() == MultiplayerState::Idle) {
        return ClientStatus::Success();
    }

    asio::post(m_strand, std::bind_front(
        &MultiplayerClient::doRequestDisconnect, this
    ));

    return ClientStatus::Success();
}

ClientStatus MultiplayerClient::requestSendGlobalChat(std::string text) {
    if (text.empty()) {
        return ClientStatus::Error(StatusCode::InvalidArgument, "Global Chat Message is empty");
    }

    if (state() != MultiplayerState::Connected) {
        return ClientStatus::Error(StatusCode::InvalidState, "Multiplayer client is not connected");
    }

    asio::post(m_strand, std::bind_front(
        &MultiplayerClient::doRequestSendGlobalChat, this, std::move(text)
    ));

    return ClientStatus::Success();
}

ClientStatus MultiplayerClient::requestSendGameChat(std::string text) {
    if (text.empty()) {
        return ClientStatus::Error(StatusCode::InvalidArgument, "Game Chat Message is empty");
    }

    if (state() != MultiplayerState::Connected) {
        return ClientStatus::Error(StatusCode::InvalidState, "Multiplayer client is not connected");
    }

    asio::post(m_strand, std::bind_front(
        &MultiplayerClient::doRequestSendGameChat, this, std::move(text)
    ));

    return ClientStatus::Success();
}

ClientStatus MultiplayerClient::requestCreateRoom(RoomCreateConfig config) {
    if (state() != MultiplayerState::Connected) {
        return ClientStatus::Error(StatusCode::InvalidState, "Multiplayer client is not connected");
    }
    asio::post(m_strand, std::bind_front(
        &MultiplayerClient::doRequestCreateRoom, this, config
    ));
    return ClientStatus::Success();
}

ClientStatus MultiplayerClient::requestRefreshRooms() {
    if (state() != MultiplayerState::Connected) {
        return ClientStatus::Error(StatusCode::InvalidState, "Multiplayer client is not connected");
    }
    asio::post(m_strand, std::bind_front(
        &MultiplayerClient::doRequestRefreshRooms, this
    ));
    return ClientStatus::Success();
}

ClientStatus MultiplayerClient::requestJoinRoomAsPlayer(RoomID roomID, std::string password) {
    if (state() != MultiplayerState::Connected) {
        return ClientStatus::Error(StatusCode::InvalidState, "Multiplayer client is not connected");
    }
    asio::post(m_strand, std::bind_front(
        &MultiplayerClient::doRequestJoinRoomAsPlayer, this, roomID, std::move(password)
    ));
    return ClientStatus::Success();
}

ClientStatus MultiplayerClient::requestJoinRoomAsSpectator(RoomID roomID, std::string password) {
    if (state() != MultiplayerState::Connected) {
        return ClientStatus::Error(StatusCode::InvalidState, "Multiplayer client is not connected");
    }
    asio::post(m_strand, std::bind_front(
        &MultiplayerClient::doRequestJoinRoomAsSpectator, this, roomID, std::move(password)
    ));
    return ClientStatus::Success();
}

ClientStatus MultiplayerClient::requestLeaveRoom() {
    if (state() != MultiplayerState::Connected) {
        return ClientStatus::Error(StatusCode::InvalidState, "Multiplayer client is not connected");
    }
    asio::post(m_strand, std::bind_front(
        &MultiplayerClient::doRequestLeaveRoom, this
    ));
    return ClientStatus::Success();
}

ClientStatus MultiplayerClient::requestSubmitMove(std::uint8_t from, std::uint8_t to, PromotionPiece promotion) {
    if (state() != MultiplayerState::Connected) {
        return ClientStatus::Error(StatusCode::InvalidState, "Multiplayer client is not connected");
    }
    asio::post(m_strand, std::bind_front(
        &MultiplayerClient::doRequestSubmitMove, this, from, to, promotion
    ));
    return ClientStatus::Success();
}


// View
MultiplayerView MultiplayerClient::view() const {
    std::lock_guard lock(m_viewMutex);
    return m_viewSnapshot;
}

MultiplayerState MultiplayerClient::state() const {
    std::lock_guard lock(m_viewMutex);
    return m_viewSnapshot.state;
}

const ThreadSafeClientChatLog& MultiplayerClient::globalChatLog() const {
    return m_globalChatLog;
}

const ThreadSafeClientChatLog& MultiplayerClient::gameChatLog() const {
    return m_gameChatLog;
}


// Event Helpers (Thread Safe Since m_emitEvent Does Not Change)
void MultiplayerClient::emitEvent(ClientEvent event) {
    if (m_emitEvent) {
        m_emitEvent(std::move(event));
    }
}

void MultiplayerClient::emitInfo(EventType type, std::string message) {
    emitEvent(ClientEvent::Info(
        EventSource::Multiplayer,
        type,
        std::move(message)
    ));
}

void MultiplayerClient::emitResult(EventType type, ClientStatus status) {
    emitEvent(ClientEvent::Result(
        EventSource::Multiplayer,
        type,
        std::move(status)
    ));
}

// All Of The Following Functions Are Strand Only And Will Always Be
// Called On An ASIO Thread Protected By The Strand

void MultiplayerClient::resetLobbyState() {
    m_rooms.clear();
    m_refreshRoomsInProgress = false;
}

void MultiplayerClient::resetRoomState() {
    m_joinedRoom = false;
    m_joinedRoomID = 0;
    m_memberType = RoomMemberType::None;
    m_memberColor = COLOR::EMPTY;
    m_latestGameUpdate.reset();
    resetChatsForGame();
}

void MultiplayerClient::applyRoomJoined(RoomID roomID, RoomMemberType memberType, COLOR color) {
    m_joinedRoom = true;
    m_joinedRoomID = roomID;
    m_memberType = memberType;
    m_memberColor = color;
}

void MultiplayerClient::applyGameUpdate(const GameUpdate& gameUpdate) {
    m_latestGameUpdate = gameUpdate;
}

void MultiplayerClient::refreshViewSnapshot() {
    std::lock_guard lock(m_viewMutex);
    m_viewSnapshot = {
        .state = m_state,
        .serverInfo = m_serverInfo,
        .socketConnected = m_socketConnected,
        .loginAccepted = m_loginAccepted,
        .lobby = MultiplayerLobbyView{
            .rooms = m_rooms,
            .refreshInProgress = m_refreshRoomsInProgress
        },
        .room = MultiplayerRoomView{
            .joined = m_joinedRoom,
            .roomID = m_joinedRoomID,
            .memberType = m_memberType,
            .color = m_memberColor,
            .whitePlayerName = m_latestGameUpdate ? m_latestGameUpdate->whitePlayerName : "",
            .blackPlayerName = m_latestGameUpdate ? m_latestGameUpdate->blackPlayerName : "",
            .spectatorCount = static_cast<std::uint16_t>(m_latestGameUpdate ? m_latestGameUpdate->spectatorCount : 0),
            .roomVersion = m_latestGameUpdate ? m_latestGameUpdate->roomVersion : 0,
        },
        .game = MultiplayerGameView{
            .hasSnapshot = m_latestGameUpdate.has_value(),
            .latestUpdate = m_latestGameUpdate
        }
    };
}

void MultiplayerClient::transitionTo(MultiplayerState newState) {
    m_state = newState;
    refreshViewSnapshot();
}


void MultiplayerClient::clearConnectionState() {
    m_serverInfo.reset();
    m_username.clear();
    m_socketConnected = false;
    m_loginAccepted = false;
    resetLobbyState();
    resetRoomState();
}

void MultiplayerClient::closeTransport() {
    std::error_code ec;
    if (m_socket.is_open()) {
        m_socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
        m_socket.close(ec);
    }

    m_socketConnected = false;
    m_writeQueue.clear();
    m_incomingMessage.clear();
}

void MultiplayerClient::terminateSession(EventType type, ClientStatus status, MultiplayerState nextState) {
    closeTransport();
    clearConnectionState();
    transitionTo(nextState);
    resetChatsForConnection();
    emitResult(type, std::move(status));
}


void MultiplayerClient::doRequestConnect(ServerInfo serverInfo, std::string username) {
    if (m_state != MultiplayerState::Idle) {
        emitResult(
            EventType::MultiplayerConnect,
            ClientStatus::Error(StatusCode::InvalidState, "MultiplayerClient Is Not Idle, Can Not Connect")
        );
        return;
    }

    std::error_code ec;
    asio::ip::address address = asio::ip::make_address(serverInfo.ip, ec);
    if (ec) {
        emitResult(
            EventType::MultiplayerConnect,
            ClientStatus::Error(StatusCode::InvalidArgument, "Invalid Server IP: " + ec.message())
        );
        return;
    }

    asio::ip::tcp::endpoint endpoint(address, DEFAULT_SERVER_PORT);

    m_serverInfo = std::move(serverInfo);
    m_username = std::move(username);
    m_socketConnected = false;
    m_loginAccepted = false;

    transitionTo(MultiplayerState::ConnectingNetwork);
    emitInfo(EventType::MultiplayerTransport, "Starting Transport Connection");

    m_socket.open(endpoint.protocol(), ec);
    if (ec) {
        terminateSession(
            EventType::MultiplayerConnect,
            ClientStatus::Error(StatusCode::ConnectFailed, "Failed to open socket")
        );
        return;
    }

    m_socket.async_connect(endpoint, asio::bind_executor(m_strand, [this](std::error_code ec) {
        if (ec == asio::error::operation_aborted) {
            return;
        }

        if (ec) {
            terminateSession(
                EventType::MultiplayerConnect,
                ClientStatus::Error(StatusCode::ConnectFailed, "Failed to connect: " + ec.message())
            );
            return;
        }
        onTransportConnected();
    }));
}

void MultiplayerClient::doRequestDisconnect() {
    if (m_state == MultiplayerState::Idle && !m_socket.is_open()) {
        return;
    }

    terminateSession(
        EventType::MultiplayerDisconnect,
        ClientStatus::Success("Disconnected")
    );
}


void MultiplayerClient::doRequestSendGlobalChat(std::string text) {
    if (m_state != MultiplayerState::Connected) {
        emitResult(
            EventType::MultiplayerTransport,
            ClientStatus::Error(StatusCode::InvalidState, "Not Connected to Multiplayer Server")
        );
        return;
    }

    Chat chat{
        .scope = ChatScope::Global,
        .message = std::move(text),
    };

    ClientStatus status = queueSend(chat.toSharedMessage());
    if (!status) {
        emitResult(EventType::MultiplayerTransport, std::move(status));
    }
}

void MultiplayerClient::doRequestSendGameChat(std::string text) {
    if (m_state != MultiplayerState::Connected) {
        emitResult(
            EventType::MultiplayerTransport,
            ClientStatus::Error(StatusCode::InvalidState, "Not Connected to Multiplayer Server")
        );
        return;
    }

    Chat chat{
        .scope = ChatScope::Game,
        .message = std::move(text),
    };

    ClientStatus status = queueSend(chat.toSharedMessage());
    if (!status) {
        emitResult(EventType::MultiplayerTransport, std::move(status));
    }
}


void MultiplayerClient::doRequestCreateRoom(RoomCreateConfig config) {
    if (m_state != MultiplayerState::Connected) {
        emitResult(EventType::MultiplayerRoomCreate, ClientStatus::Error(StatusCode::InvalidState, "Not Connected to Multiplayer Server"));
        return;
    }
    if (m_joinedRoom) {
        emitResult(EventType::MultiplayerRoomCreate, ClientStatus::Error(StatusCode::InvalidState, "Already In A Room"));
        return;
    }

    ClientStatus status = queueSend(CreateRoomRequest{.config = std::move(config)}.toSharedMessage());
    if (!status) {
        emitResult(EventType::MultiplayerRoomCreate, std::move(status));
    }
}

void MultiplayerClient::doRequestRefreshRooms() {
    if (m_state != MultiplayerState::Connected) {
        emitResult(EventType::MultiplayerRoomsRefresh, ClientStatus::Error(StatusCode::InvalidState, "Not Connected to Multiplayer Server"));
        return;
    }

    m_refreshRoomsInProgress = true;
    refreshViewSnapshot();

    ClientStatus status = queueSend(ListRoomsRequest{}.toSharedMessage());
    if (!status) {
        m_refreshRoomsInProgress = false;
        refreshViewSnapshot();
        emitResult(EventType::MultiplayerRoomsRefresh, std::move(status));
    }
}


void MultiplayerClient::doRequestJoinRoomAsPlayer(RoomID roomID, std::string password) {
    if (m_state != MultiplayerState::Connected) {
        emitResult(EventType::MultiplayerRoomJoin, ClientStatus::Error(StatusCode::InvalidState, "Not Connected to Multiplayer Server"));
        return;
    }

    if (m_joinedRoom) {
        emitResult(EventType::MultiplayerRoomJoin, ClientStatus::Error(StatusCode::InvalidState, "Already In A Room"));
        return;
    }

    JoinRoomRequest request{.roomID = roomID, .spectator = false, .password = std::move(password)};
    ClientStatus status = queueSend(request.toSharedMessage());
    if (!status) {
        emitResult(EventType::MultiplayerRoomJoin, std::move(status));
    }
}

void MultiplayerClient::doRequestJoinRoomAsSpectator(RoomID roomID, std::string password) {
    if (m_state != MultiplayerState::Connected) {
        emitResult(EventType::MultiplayerRoomJoin, ClientStatus::Error(StatusCode::InvalidState, "Not Connected to Multiplayer Server"));
        return;
    }

    if (m_joinedRoom) {
        emitResult(EventType::MultiplayerRoomJoin, ClientStatus::Error(StatusCode::InvalidState, "Already In A Room"));
        return;
    }

    JoinRoomRequest request{.roomID = roomID, .spectator = true, .password = std::move(password)};
    ClientStatus status = queueSend(request.toSharedMessage());
    if (!status) {
        emitResult(EventType::MultiplayerRoomJoin, std::move(status));
    }
}

void MultiplayerClient::doRequestLeaveRoom() {
    if (m_state != MultiplayerState::Connected) {
        emitResult(EventType::MultiplayerRoomLeave, ClientStatus::Error(StatusCode::InvalidState, "Not Connected to Multiplayer Server"));
        return;
    }

    if (!m_joinedRoom) {
        emitResult(EventType::MultiplayerRoomLeave, ClientStatus::Error(StatusCode::InvalidState, "Not In A Room"));
        return;
    }

    ClientStatus status = queueSend(LeaveRoomRequest{}.toSharedMessage());
    if (!status) {
        emitResult(EventType::MultiplayerRoomLeave, std::move(status));
    }
}

void MultiplayerClient::doRequestSubmitMove(std::uint8_t from, std::uint8_t to, PromotionPiece promotion) {
    if (m_state != MultiplayerState::Connected) {
        emitResult(EventType::MultiplayerMove, ClientStatus::Error(StatusCode::InvalidState, "Not Connected to Multiplayer Server"));
        return;
    }

    if (!m_joinedRoom) {
        emitResult(EventType::MultiplayerMove, ClientStatus::Error(StatusCode::InvalidState, "Not In A Room"));
        return;
    }

    MakeMove request = {.from=from, .to=to, .promotion=promotion};
    ClientStatus status = queueSend(request.toSharedMessage());
    if (!status) {
        emitResult(EventType::MultiplayerMove, std::move(status));
    }
}


ClientStatus MultiplayerClient::queueSend(std::shared_ptr<const Message> message) {
    if (!message) {
        return ClientStatus::Warning(StatusCode::InvalidArgument, "Cannot Send Null message");
    }
    if (!m_socketConnected) {
        return ClientStatus::Warning(StatusCode::InvalidState, "Cannot Send Message; Socket Not Connected");
    }
    if (m_writeQueue.size() >= MAX_WRITE_QUEUE_LENGTH) {
        return ClientStatus::Error(StatusCode::SendFailed, "Write Queue Overflow");
    }

    bool shouldStartWrite = m_writeQueue.empty();
    m_writeQueue.push_back(std::move(message));
    if (shouldStartWrite) {
        doWrite();
    }
    return ClientStatus::Success();
}

void MultiplayerClient::doReadHeader() {
    m_incomingMessage.clear();

    asio::async_read(m_socket, m_incomingMessage.headerBuffer(), asio::bind_executor(
    m_strand, [this](std::error_code ec, std::size_t length) {
        if (ec==asio::error::operation_aborted) {
            return;
        }
        if (ec) {
            terminateSession(
                EventType::MultiplayerDisconnect,
                ClientStatus::Error(StatusCode::ReadFailed, "Failed to read header: " + ec.message())
            );
            return;
        }
        if (!m_incomingMessage.validateHeader()) {
            terminateSession(
                EventType::MultiplayerDisconnect,
                ClientStatus::Error(StatusCode::ProtocolError, "Received invalid header")
            );
            return;
        }

        std::size_t bodyLength = m_incomingMessage.header().bodyLength;
        m_incomingMessage.resize(bodyLength);
        if (bodyLength == 0) {
            Message message = std::move(m_incomingMessage);
            m_incomingMessage.clear();
            onIncomingMessage(std::move(message));

            if (m_socket.is_open()) {
                doReadHeader();
            }
            return;
        }
        doReadBody();
    }));
}

void MultiplayerClient::doReadBody() {
    asio::async_read(m_socket, m_incomingMessage.bodyBuffer(), asio::bind_executor(m_strand, [this](std::error_code ec, std::size_t length) {
        if (ec == asio::error::operation_aborted) {
            return;
        }
        if (ec) {
            terminateSession(
                EventType::MultiplayerDisconnect,
                ClientStatus::Error(StatusCode::ReadFailed, "Failed to read body: " + ec.message())
            );
            return;
        }

        Message receivedMessage = std::move(m_incomingMessage);
        m_incomingMessage.clear();

        onIncomingMessage(std::move(receivedMessage));

        if (m_socket.is_open()) {
            doReadHeader();
        }
    }));
}

void MultiplayerClient::doWrite() {
    if (m_writeQueue.empty()) {
        return;
    }

    asio::async_write(m_socket, m_writeQueue.front()->buffers(), asio::bind_executor(m_strand, [this](std::error_code ec, std::size_t length) {
        if (ec == asio::error::operation_aborted) {
            return;
        }

        if (ec) {
            terminateSession(
                EventType::MultiplayerDisconnect,
                ClientStatus::Error(StatusCode::SendFailed, "Failed to write message: " + ec.message())
            );
            return;
        }

        m_writeQueue.pop_front();
        if (!m_writeQueue.empty()) {
            doWrite();
        }
    }));
}


void MultiplayerClient::onTransportConnected() {
    if (m_state != MultiplayerState::ConnectingNetwork) {
        terminateSession(
            EventType::MultiplayerConnect,
            ClientStatus::Error(StatusCode::InvalidState, "Transport Connected When Not Expecting Connection")
        );
        return;
    }

    if (!m_serverInfo.has_value()) {
        terminateSession(
            EventType::MultiplayerConnect,
            ClientStatus::Error(StatusCode::InvalidState, "Transport Connected But No Server Info Present")
        );
        return;
    }

    m_socketConnected = true;
    transitionTo(MultiplayerState::AwaitingLogin);

    emitInfo(EventType::MultiplayerTransport, "Transport Connected");
    emitInfo(EventType::MultiplayerTransport, "Sending Login Request");

    LoginRequest request{
        .username = m_username,
        .password = m_serverInfo->password
    };

    auto message = request.toSharedMessage();

    ClientStatus sendStatus = queueSend(message);
    if (!sendStatus) {
        terminateSession(
            EventType::MultiplayerConnect,
            ClientStatus::Error(sendStatus.code, std::move(sendStatus.message))
        );
        return;
    }

    doReadHeader();
}



void MultiplayerClient::onIncomingMessage(Message message) {
    emitInfo(
        EventType::MultiplayerTransport,
        "Received message type=" + std::string(toString(message.type()))
    );
    switch (message.type()) {
        case MessageType::LoginResponse:
            onLoginResponse(message);
            break;
        case MessageType::Chat:
            onChatMessage(message);
            break;
        case MessageType::CreateRoomResponse:
            onCreateRoomResponse(message);
            break;
        case MessageType::ListRoomsResponse:
            onListRoomsResponse(message);
            break;
        case MessageType::JoinRoomResponse:
            onJoinRoomResponse(message);
            break;
        case MessageType::LeaveRoomResponse:
            onLeaveRoomResponse(message);
            break;
        case MessageType::GameUpdate:
            onGameUpdate(message);
            break;
        case MessageType::ErrorMessage:
            onErrorMessage(message);
            break;
        default:
            terminateSession(
                EventType::MultiplayerDisconnect,
                ClientStatus::Error(StatusCode::ProtocolError, "Unsupported Message Type Received")
            );
            break;
    }
}

void MultiplayerClient::onLoginResponse(Message& message) {
    if (m_state != MultiplayerState::AwaitingLogin) {
        terminateSession(
            EventType::MultiplayerLogin,
            ClientStatus::Error(StatusCode::ProtocolError, "Received LoginResponse while not awaiting login")
        );
        return;
    }

    std::optional<LoginResponse> loginResponse = LoginResponse::fromMessage(message);
    if (!loginResponse.has_value()) {
        terminateSession(
            EventType::MultiplayerDisconnect,
            ClientStatus::Error(StatusCode::ProtocolError, "Failed To Parse Login Response")
        );
        return;
    }

    if (loginResponse->accepted) {
        m_loginAccepted = true;
        resetChatsForConnection();
        resetLobbyState();
        resetRoomState();
        transitionTo(MultiplayerState::Connected);
        emitResult(
            EventType::MultiplayerLogin,
            ClientStatus::Success("Login Accepted")
        );
        doRequestRefreshRooms();
        return;
    }

    terminateSession(
        EventType::MultiplayerLogin,
        ClientStatus::Error(
            StatusCode::LoginRejected,
            loginResponse->reason.empty() ? "Login Rejected" : loginResponse->reason
        )
    );
}


void MultiplayerClient::onChatMessage(Message& message) {
    if (m_state != MultiplayerState::Connected) {
        terminateSession(
            EventType::MultiplayerDisconnect,
            ClientStatus::Error(StatusCode::ProtocolError, "Received ChatMessage when Not Connected")
        );
        return;
    }

    std::optional<Chat> chat = Chat::fromMessage(message);
    if (!chat.has_value()) {
        terminateSession(
            EventType::MultiplayerDisconnect,
            ClientStatus::Error(StatusCode::ProtocolError, "Failed To Parse ChatMessage")
        );
        return;
    }

    ClientChatEntry chatEntry{
        .text = chat->message,
    };

    switch (chat->scope) {
        case ChatScope::Global:
            m_globalChatLog.append(std::move(chatEntry));
            break;
        case ChatScope::Game:
            m_gameChatLog.append(std::move(chatEntry));
            break;
    }
}

void MultiplayerClient::onCreateRoomResponse(Message &message) {
    if (m_state != MultiplayerState::Connected) {
        terminateSession(
            EventType::MultiplayerDisconnect,
            ClientStatus::Error(StatusCode::ProtocolError, "Received CreateRoomResponse Message when Not Connected")
        );
        return;
    }

    std::optional<CreateRoomResponse> response = CreateRoomResponse::fromMessage(message);
    if (!response.has_value()) {
        terminateSession(
            EventType::MultiplayerDisconnect,
            ClientStatus::Error(StatusCode::ProtocolError, "Failed To Parse CreateRoomResponse Message")
        );
        return;
    }

    if (!response->success) {
        emitResult(EventType::MultiplayerRoomCreate, ClientStatus::Error(StatusCode::InvalidState, response->reason.empty() ? "Create Room Rejected" : response->reason));
        return;
    }

    resetChatsForGame();
    applyRoomJoined(response->roomID, response->memberType, response->color);
    refreshViewSnapshot();
    emitResult(EventType::MultiplayerRoomCreate, ClientStatus::Success("Room Created"));
    doRequestRefreshRooms();
}

void MultiplayerClient::onListRoomsResponse(Message &message) {
    if (m_state != MultiplayerState::Connected) {
        terminateSession(
            EventType::MultiplayerDisconnect,
            ClientStatus::Error(StatusCode::ProtocolError, "Received ListRoomsResponse Message when Not Connected")
        );
        return;
    }

    std::optional<ListRoomsResponse> response = ListRoomsResponse::fromMessage(message);
    if (!response.has_value()) {
        terminateSession(
            EventType::MultiplayerDisconnect,
            ClientStatus::Error(StatusCode::ProtocolError, "Failed To Parse ListRoomsResponse Message")
        );
        return;
    }

    m_rooms = std::move(response->rooms);
    m_refreshRoomsInProgress = false;
    refreshViewSnapshot();
    emitResult(EventType::MultiplayerRoomsRefresh, ClientStatus::Success("Lobby room list updated"));
}

void MultiplayerClient::onJoinRoomResponse(Message &message) {
    if (m_state != MultiplayerState::Connected) {
        terminateSession(
            EventType::MultiplayerDisconnect,
            ClientStatus::Error(StatusCode::ProtocolError, "Received JoinRoomResponse Message when Not Connected")
        );
        return;
    }

    std::optional<JoinRoomResponse> response = JoinRoomResponse::fromMessage(message);
    if (!response.has_value()) {
        terminateSession(
            EventType::MultiplayerDisconnect,
            ClientStatus::Error(StatusCode::ProtocolError, "Failed To Parse JoinRoomResponse Message")
        );
        return;
    }

    if (!response->success) {
        emitResult(EventType::MultiplayerRoomJoin, ClientStatus::Error(StatusCode::InvalidState, response->reason.empty() ? "Join Room Rejected" : response->reason));
        return;
    }

    resetChatsForGame();
    applyRoomJoined(response->roomID, response->memberType, response->color);
    refreshViewSnapshot();
    emitResult(EventType::MultiplayerRoomJoin, ClientStatus::Success("Room Joined"));
    doRequestRefreshRooms();
}

void MultiplayerClient::onLeaveRoomResponse(Message &message) {
    if (m_state != MultiplayerState::Connected) {
        terminateSession(
            EventType::MultiplayerDisconnect,
            ClientStatus::Error(StatusCode::ProtocolError, "Received LeaveRoomResponse Message when Not Connected")
        );
        return;
    }

    std::optional<LeaveRoomResponse> response = LeaveRoomResponse::fromMessage(message);
    if (!response.has_value()) {
        terminateSession(
            EventType::MultiplayerDisconnect,
            ClientStatus::Error(StatusCode::ProtocolError, "Failed To Parse LeaveRoomResponse Message")
        );
        return;
    }

    if (!response->success) {
        emitResult(EventType::MultiplayerRoomLeave, ClientStatus::Error(StatusCode::InvalidState, response->reason.empty() ? "Leave Room Rejected" : response->reason));
        return;
    }

    resetRoomState();
    refreshViewSnapshot();
    emitResult(EventType::MultiplayerRoomLeave, ClientStatus::Success("Room Left"));
    doRequestRefreshRooms();
}

void MultiplayerClient::onGameUpdate(Message &message) {
    if (m_state != MultiplayerState::Connected) {
        terminateSession(
            EventType::MultiplayerDisconnect,
            ClientStatus::Error(StatusCode::ProtocolError, "Received GameUpdate Message when Not Connected")
        );
        return;
    }

    std::optional<GameUpdate> update = GameUpdate::fromMessage(message);
    if (!update.has_value()) {
        terminateSession(
            EventType::MultiplayerDisconnect,
            ClientStatus::Error(StatusCode::ProtocolError, "Failed To Parse GameUpdate Message")
        );
        return;
    }

    if (!m_joinedRoom || m_joinedRoomID != update->roomID) {
        terminateSession(EventType::MultiplayerDisconnect, ClientStatus::Error(StatusCode::ProtocolError, "Received GameUpdate For Unexpected Room"));
        return;
    }

    applyGameUpdate(*update);
    refreshViewSnapshot();

    emitInfo(
        EventType::MultiplayerGameSync,
        "Applied GameUpdate room=" + std::to_string(update->roomID) +
        " roomVersion=" + std::to_string(update->roomVersion) +
        " gameVersion=" + std::to_string(update->snapshot.version)
    );
}

void MultiplayerClient::onErrorMessage(Message &message) {
    std::optional<ErrorMessage> error = ErrorMessage::fromMessage(message);
    if (!error.has_value()) {
        terminateSession(
            EventType::MultiplayerDisconnect,
            ClientStatus::Error(StatusCode::ProtocolError, "Failed To Parse ErrorMessage Message")
        );
        return;
    }

    emitResult(EventType::MultiplayerServerError, ClientStatus::Error(StatusCode::UnknownError, error->message.empty() ? "Server Reported An Error" : error->message));
}

void MultiplayerClient::resetChatsForConnection() {
    m_globalChatLog.clear();
    m_gameChatLog.clear();
}

void MultiplayerClient::resetChatsForGame() {
    m_gameChatLog.clear();
}

std::string MultiplayerClient::chatTimestampNow() const {
    return std::format("{:%H:%M:%S}", std::chrono::system_clock::now());
}

void MultiplayerClient::appendGlobalSystemChat(std::string text) {
    m_globalChatLog.append(ClientChatEntry{
        .text = std::format("[{}][{}] {}", chatTimestampNow(), "System", text),
    });
}

void MultiplayerClient::appendGameSystemChat(std::string text) {
    m_gameChatLog.append(ClientChatEntry{
        .text = std::format("[{}][{}] {}", chatTimestampNow(), "System", text),
    });
}
}
