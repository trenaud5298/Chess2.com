#ifndef CHESS_CLIENT_UI_MULTIPLAYERLOBBYSCREEN_HPP
#define CHESS_CLIENT_UI_MULTIPLAYERLOBBYSCREEN_HPP


/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Screen/ScreenInterface.hpp>
#include <Chess/Core/Common/Types.hpp>
#include <Chess/Client/Runtime/MultiplayerClient.hpp>

// ASIO Includes

// FTXUI Includes
#include <ftxui/component/component.hpp>

// C++ Includes
#include <cstdint>
#include <memory>

namespace Chess {

class ClientPanel;
class ChatPane;

class MultiplayerLobbyScreen : public ScreenInterface {
public:
    explicit MultiplayerLobbyScreen(ClientPanel& clientPanel);
    ~MultiplayerLobbyScreen();

    ftxui::Component getComponent() override;

    void onEnter() override;
    void onLeave() override;
    void onTick() override;

    bool canRequestExit() const override {return true;}
    std::string exitLabel() const override { return "Disconnect";}
    void requestExit() override;

private:
    void refreshSnapshot();
    void rebuildRoomEntries();

    bool hasHoveredRoom() const;
    RoomID hoveredRoomID() const;

    void selectHoveredRoom();

    std::optional<RoomSummary> hoveredRoom() const;
    std::optional<RoomSummary> selectedRoom() const;

    bool canRefreshRooms() const;
    bool canCreateRoom() const;
    bool canJoinAsPlayer() const;
    bool canJoinAsSpectator() const;

    ftxui::Element renderRoomEntry(const ftxui::EntryState& state);
    ftxui::Component buildComponent();

private:
    std::unique_ptr<ChatPane> m_globalChatPane;
    MultiplayerView m_view{};

    std::vector<RoomID> m_roomIDs;
    std::vector<std::string> m_roomEntries;
    int m_roomCursor{0};
    std::optional<RoomID> m_selectedRoom;

    ftxui::Component m_component;
    static constexpr Screen SCREEN_TYPE = Screen::Multiplayer_Lobby;
};

}
#endif