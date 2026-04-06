#ifndef CHESS_CLIENT_UI_MULTIPLAYERSELECTSCREEN_HPP
#define CHESS_CLIENT_UI_MULTIPLAYERSELECTSCREEN_HPP


/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Screen/ScreenInterface.hpp>
#include <Chess/Client/Common/ServerInfo.hpp>

// ASIO Includes

// FTXUI Includes
#include <ftxui/component/component.hpp>
#include <ftxui/screen/box.hpp>

// C++ Includes
#include <cstdint>
#include <optional>
#include <vector>

namespace Chess {

class ClientPanel;

class MultiplayerSelectScreen : public ScreenInterface {
public:
    explicit MultiplayerSelectScreen(ClientPanel& clientPanel);
    ~MultiplayerSelectScreen();
    ftxui::Component getComponent() override;

    void onEnter() override;
    void onLeave() override;

    bool canRequestExit() const override {return true;}
    std::string exitLabel() const override {return "Main Menu";}
    void requestExit() override;


private:
    void addServerEntry(ServerInfo serverInfo);
    void removeSelectedServer();
    void joinSelectedServer();
    ftxui::Component buildComponent();

    // Entry Rendering Helpers
    void rebuildServerEntries();
    ftxui::Element renderServerEntry(const ftxui::EntryState& state);

    // Entry Selection Helpers
    bool hasSelectedServerEntry() const;
    void activateCursor();
private:
    // Uses Placeholder Strings For Custom Rendering
    std::vector<ServerInfo> m_servers;
    std::vector<std::string> m_entries;
    int m_cursor{0};
    std::optional<int> m_selectedServer{std::nullopt};


    ftxui::Component m_component;
    static constexpr Screen SCREEN_TYPE = Screen::Multiplayer_Select;


};

}
#endif