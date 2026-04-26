/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Screen/MultiplayerLobbyScreen.hpp>
#include <Chess/Client/UI/ClientPanel.hpp>
#include <Chess/Client/UI/Component/ChatPane.hpp>
#include <Chess/Client/Runtime/GameClient.hpp>
#include <Chess/Client/UI/Modal/CreateRoomModal.hpp>
#include <Chess/Client/UI/Modal/JoinRoomModal.hpp>

// FTXUI Includes
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

// C++ Includes
#include <chrono>
#include <utility>


namespace Chess {

MultiplayerLobbyScreen::MultiplayerLobbyScreen(ClientPanel& clientPanel) : ScreenInterface(clientPanel) {
    m_globalChatPane = std::make_unique<ChatPane>(ChatPaneConfig{
        .logAccessor = [this]() -> const ThreadSafeClientChatLog& {
            return m_clientPanel.gameClient().multiplayerGlobalChat();
        },
        .submitFunction = [this](std::string text) {
            return m_clientPanel.handleStatus(
                m_clientPanel.gameClient().submitMultiplayerGlobalChat(std::move(text)),
                "Unable to send global chat",
                ResultPolicy::LogOnly
            );
        },
        .emptyText = "No global chat messages",
        .inputPlaceholder = "Send a global message..."
    });

    m_component = buildComponent();
}

MultiplayerLobbyScreen::~MultiplayerLobbyScreen() {

}

ftxui::Component MultiplayerLobbyScreen::getComponent() {
    return m_component;
}

void MultiplayerLobbyScreen::onEnter() {
    m_clientPanel.setTickRate(std::chrono::milliseconds(50));
    if (m_globalChatPane) {
        m_globalChatPane->onEnter();
    }

    refreshSnapshot();
    rebuildRoomEntries();

    m_clientPanel.handleStatus(
        m_clientPanel.gameClient().requestMultiplayerRefreshRooms(),
        "Unable To Refresh Room List", ResultPolicy::Modal
    );
}

void MultiplayerLobbyScreen::onLeave() {
    if (m_globalChatPane) {
        m_globalChatPane->onLeave();
    }
    m_clientPanel.setTickRate(std::nullopt);
}

void MultiplayerLobbyScreen::onTick() {
    refreshSnapshot();
    rebuildRoomEntries();
}

void MultiplayerLobbyScreen::requestExit() {
    m_clientPanel.handleStatus(
        m_clientPanel.gameClient().requestMultiplayerDisconnect(),
        "Unable to disconnect from server",
        ResultPolicy::Modal
    );
}

void MultiplayerLobbyScreen::refreshSnapshot() {
    m_view = m_clientPanel.gameClient().multiplayerView();
}

void MultiplayerLobbyScreen::rebuildRoomEntries() {
    RoomID previousHoveredRoomID = hoveredRoomID();
    RoomID previousSelectedRoomID = m_selectedRoom.has_value() ? *m_selectedRoom : 0;

    m_roomCursor = 0;
    m_selectedRoom = std::nullopt;
    m_roomIDs.clear();
    m_roomEntries.assign(m_view.lobby.rooms.size(), "");
    m_roomIDs.reserve(m_view.lobby.rooms.size());

    for (const RoomSummary& room : m_view.lobby.rooms) {
        m_roomIDs.push_back(room.roomID);
    }

    // Check For Selected Room & Previously Hovered Room
    for (std::size_t i = 0; i < m_roomIDs.size(); ++i) {
        if (m_roomIDs[i] == previousHoveredRoomID) {
            m_roomCursor = i;
        }
        if (m_roomIDs[i] == previousSelectedRoomID) {
            m_selectedRoom = m_roomIDs[i];
        }
    }
}

bool MultiplayerLobbyScreen::hasHoveredRoom() const {
    return m_roomCursor >= 0 && m_roomCursor < m_roomIDs.size();
}

RoomID MultiplayerLobbyScreen::hoveredRoomID() const {
    return hasHoveredRoom() ? m_roomIDs[m_roomCursor] : 0;
}

void MultiplayerLobbyScreen::selectHoveredRoom() {
    if (!hasHoveredRoom()) {
        return;
    }
    m_selectedRoom = hoveredRoomID();
}

std::optional<RoomSummary> MultiplayerLobbyScreen::hoveredRoom() const {
    if (!hasHoveredRoom()) {
        return std::nullopt;
    }

    RoomID roomID = hoveredRoomID();
    for (const RoomSummary& room : m_view.lobby.rooms) {
        if (room.roomID == roomID) {
            return room;
        }
    }
    return std::nullopt;
}

std::optional<RoomSummary> MultiplayerLobbyScreen::selectedRoom() const {
    if (!m_selectedRoom.has_value()) {
        return std::nullopt;
    }

    for (const RoomSummary& room : m_view.lobby.rooms) {
        if (room.roomID == *m_selectedRoom) {
            return room;
        }
    }
    return std::nullopt;
}

bool MultiplayerLobbyScreen::canRefreshRooms() const {
    return !m_view.lobby.refreshInProgress;
}

bool MultiplayerLobbyScreen::canCreateRoom() const {
    return !m_view.room.joined;
}

bool MultiplayerLobbyScreen::canJoinAsPlayer() const {
    if (m_view.room.joined) {
        return false;
    }
    std::optional<RoomSummary> room = selectedRoom();
    if (!room.has_value()) {
        return false;
    }
    return room->hasOpenPlayerSeat;
}

bool MultiplayerLobbyScreen::canJoinAsSpectator() const {
    if (m_view.room.joined) {
        return false;
    }
    auto room = selectedRoom();
    if (!room.has_value()) {
        return false;
    }
    if (!room->config.spectator.allowSpectators) {
        return false;
    }
    if (!room->config.spectator.allowMidgameJoin && room->inProgress) {
        return false;
    }
    if (room->config.spectator.maxSpectators != 0 && room->spectatorCount >= room->config.spectator.maxSpectators) {
        return false;
    }

    return true;
}

ftxui::Element MultiplayerLobbyScreen::renderRoomEntry(const ftxui::EntryState &state) {
    if (state.index < 0 || state.index >= m_view.lobby.rooms.size()) {
        return ftxui::text("");
    }

    const RoomSummary& room = m_view.lobby.rooms[state.index];

    bool isHovered = state.focused;
    bool isSelected = m_selectedRoom.has_value() && *m_selectedRoom == room.roomID;

    std::string roomLabel = std::to_string(room.roomID);
    std::string nameLabel = room.config.presentation.name.empty() ? "Untitled Room" : room.config.presentation.name;
    std::string accessLabel = room.config.passwordProtected ? "Private" : "Public";
    std::string statusLabel = room.inProgress ? "In Progress" : "Waiting";

    ftxui::Element row = ftxui::hbox({
        ftxui::text(roomLabel) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 8),
        ftxui::separatorEmpty() | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 1),
        ftxui::text(nameLabel) | ftxui::flex,
        ftxui::separatorEmpty() | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 1),
        ftxui::text(accessLabel) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 10) | ftxui::color((room.config.passwordProtected ? ftxui::Color::Yellow : ftxui::Color::Green)),
        ftxui::separatorEmpty() | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 1),
        ftxui::text(statusLabel) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 14) | ftxui::color((room.inProgress ? ftxui::Color::Yellow : ftxui::Color::Green)),
    });

    if (isHovered) {
        row |= ftxui::inverted;
    }

    if (isSelected) {
        return ftxui::vbox({ftxui::separator(), row, ftxui::separator()});
    } else {
        return ftxui::vbox({ftxui::separatorEmpty(), row, ftxui::separatorEmpty()});
    }
}


ftxui::Component MultiplayerLobbyScreen::buildComponent() {
    rebuildRoomEntries();

    // Menu Setup
    ftxui::MenuOption menuOption;
    menuOption.entries_option.transform = std::bind_front(&MultiplayerLobbyScreen::renderRoomEntry, this);
    menuOption.focused_entry = &m_roomCursor;
    menuOption.on_enter = [this]() {
        selectHoveredRoom();
    };
    auto roomMenu = ftxui::Menu(&m_roomEntries, &m_roomCursor, menuOption);

    auto roomMenuEventCatcher = ftxui::CatchEvent(roomMenu, [this, roomMenu](ftxui::Event event) {
        if (event.is_mouse() && event.mouse().button == ftxui::Mouse::Left && event.mouse().motion == ftxui::Mouse::Pressed) {
            bool handled = roomMenu->OnEvent(event);
            if (handled) {
                selectHoveredRoom();
            }
            return handled;
        }
        return false;
    });


    // Button Options
    ftxui::ButtonOption refreshButtonOption = ftxui::ButtonOption::Animated();
    auto originalRefreshTransform = refreshButtonOption.transform;
    refreshButtonOption.transform = [this, originalRefreshTransform](const ftxui::EntryState& state) {
        if (!canRefreshRooms()) {
            return ftxui::text(state.label) | ftxui::center | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 16) | ftxui::color(ftxui::Color::GrayDark);
        }
        return originalRefreshTransform(state) | ftxui::center | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 16);
    };

    ftxui::ButtonOption createButtonOption = ftxui::ButtonOption::Animated();
    auto originalCreateTransform = createButtonOption.transform;
    createButtonOption.transform = [this, originalCreateTransform](const ftxui::EntryState& state) {
        if (!canCreateRoom()) {
            return ftxui::text(state.label) | ftxui::center | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 16) | ftxui::color(ftxui::Color::GrayDark);
        }
        return originalCreateTransform(state) | ftxui::center | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 16);
    };

    ftxui::ButtonOption joinButtonOption = ftxui::ButtonOption::Animated();
    auto originalJoinTransform = joinButtonOption.transform;
    joinButtonOption.transform = [this, originalJoinTransform](const ftxui::EntryState& state) {
        if (!canJoinAsPlayer()) {
            return ftxui::text(state.label) | ftxui::center | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 16) | ftxui::color(ftxui::Color::GrayDark);
        }
        return originalJoinTransform(state) | ftxui::center | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 16);
    };

    ftxui::ButtonOption spectateButtonOption = ftxui::ButtonOption::Animated();
    auto originalSpectateTransform = spectateButtonOption.transform;
    spectateButtonOption.transform = [this, originalSpectateTransform](const ftxui::EntryState& state) {
        if (!canJoinAsSpectator()) {
            return ftxui::text(state.label) | ftxui::center | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 16) | ftxui::color(ftxui::Color::GrayDark);
        }
        return originalSpectateTransform(state) | ftxui::center | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 16);
    };

    auto refreshButton = ftxui::Button("Refresh", [this]() {
        if (!canRefreshRooms()) {
            return;
        }
        m_clientPanel.handleStatus(
            m_clientPanel.gameClient().requestMultiplayerRefreshRooms(),
            "Unable To Refresh Room List", ResultPolicy::Modal
        );
    }, refreshButtonOption);

    auto createButton = ftxui::Button("Create Room", [this]() {
        if (!canCreateRoom()) {
            return;
        }
        m_clientPanel.pushModal(std::make_unique<CreateRoomModal>(m_clientPanel));
    }, createButtonOption);

    auto joinButton = ftxui::Button("Join Player", [this]() {
        std::optional<RoomSummary> room = selectedRoom();
        if (!room.has_value()) {
            return;
        }

        if (room->config.passwordProtected) {
            m_clientPanel.pushModal(std::make_unique<JoinRoomModal>(m_clientPanel, *room, false));
            return;
        }

        m_clientPanel.handleStatus(
            m_clientPanel.gameClient().requestMultiplayerJoinRoomAsPlayer(room->roomID),
            "Unable To Join Room As Player", ResultPolicy::Modal
        );
    }, joinButtonOption);

    auto spectateButton = ftxui::Button("Join Spectator", [this]() {
        std::optional<RoomSummary> room = selectedRoom();
        if (!room.has_value()) {
            return;
        }

        if (room->config.passwordProtected) {
            m_clientPanel.pushModal(std::make_unique<JoinRoomModal>(m_clientPanel, *room, true));
            return;
        }

        m_clientPanel.handleStatus(
            m_clientPanel.gameClient().requestMultiplayerJoinRoomAsSpectator(room->roomID),
            "Unable To Join Room As Spectator", ResultPolicy::Modal
        );
    }, spectateButtonOption);

    auto buttonRow = ftxui::Container::Horizontal({
        refreshButton,
        createButton,
        joinButton,
        spectateButton
    });

    auto rightPaneContainer = ftxui::Container::Vertical({
        roomMenuEventCatcher,
        buttonRow,
    });

    auto layout = ftxui::Container::Horizontal({
        m_globalChatPane->component(),
        rightPaneContainer
    });


    return ftxui::Renderer(layout, [this, roomMenuEventCatcher, refreshButton, createButton, joinButton, spectateButton]() {
        std::optional<RoomSummary> room = selectedRoom();

        ftxui::Element roomTableHeader = ftxui::hbox({
            ftxui::text("Room") | ftxui::bold | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 8),
            ftxui::separatorEmpty() | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 1),
            ftxui::text("Name") | ftxui::bold | ftxui::flex,
            ftxui::separatorEmpty() | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 1),
            ftxui::text("Access") | ftxui::bold | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 10),
            ftxui::separatorEmpty() | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 1),
            ftxui::text("Status") | ftxui::bold | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 14)
        });

        ftxui::Element roomList;
        if (m_view.lobby.refreshInProgress) {
            roomList = ftxui::text("Loading...") | ftxui::dim | ftxui::center | ftxui::flex;
        } else if (m_view.lobby.rooms.empty()) {
            roomList = ftxui::text("No rooms available. Create one to start player") | ftxui::dim | ftxui::center | ftxui::flex;
        } else {
            roomList = roomMenuEventCatcher->Render() | ftxui::frame | ftxui::vscroll_indicator | ftxui::flex | ftxui::yflex;
        }

        ftxui::Element roomDetails;
        if (!room) {
            roomDetails = ftxui::vbox({
                ftxui::text("Room Details") | ftxui::bold | ftxui::center,
                ftxui::separatorEmpty(),
                ftxui::text("Highlight a room to inspect it") | ftxui::dim
            });
        } else {
            std::string roomName = room->config.presentation.name.empty() ? "Untitled Room" : room->config.presentation.name;
            std::string whiteName = room->whitePlayerName.empty() ? "Open Seat" : room->whitePlayerName;
            std::string blackName = room->blackPlayerName.empty() ? "Open Seat" : room->blackPlayerName;

            roomDetails = ftxui::hbox({
                ftxui::vbox({
                    ftxui::text("Room ID: " + std::to_string(room->roomID)),
                    ftxui::text("Name: " + roomName),
                    ftxui::text("White: " + whiteName),
                    ftxui::text("Black: " + blackName),
                }),
                ftxui::filler(),
                ftxui::vbox({
                    ftxui::text("Access: " + std::string(room->config.passwordProtected ? "Private" : "Public")),
                    ftxui::text("Spectators: " + std::to_string(room->spectatorCount)),
                    ftxui::text("Open Seat: " + std::string(room->hasOpenPlayerSeat ? "Yes" : "No")),
                    ftxui::text("Game: " + std::string(room->inProgress ? "In Progress" : "Waiting"))
                })
            });
        }

        auto rightPane = ftxui::vbox({
            roomTableHeader,
            ftxui::separator(),
            roomList | ftxui::flex | ftxui::yflex,
            ftxui::separator(),
            roomDetails,
            ftxui::separator(),
            ftxui::hbox({
                refreshButton->Render(),
                ftxui::filler() | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 2),
                createButton->Render(),
                ftxui::filler() | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 2),
                joinButton->Render(),
                ftxui::filler() | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 2),
                spectateButton->Render(),
            }) | ftxui::center,
        }) | ftxui::flex | ftxui::yflex;

        return ftxui::hbox({
            ftxui::vbox({
                ftxui::text("Global Chat") | ftxui::bold,
                ftxui::separator(),
                m_globalChatPane->component()->Render() | ftxui::flex | ftxui::yflex
            }) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 38) | ftxui::yflex,
            ftxui::separator(),
            rightPane
        }) | ftxui::flex;
    });
}



} // namespace Chess