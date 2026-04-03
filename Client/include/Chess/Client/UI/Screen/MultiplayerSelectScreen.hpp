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

// C++ Includes
#include <cstdint>


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
    void removeServerEntry(std::uint64_t index);

    ftxui::Component buildComponent();
    // Entry Helpers
    void rebuildServerEntries();
    ftxui::Element renderServerEntry(const ftxui::EntryState& state);
private:
    // Uses Placeholder Strings For Custom Rendering
    std::vector<ServerInfo> m_servers;
    std::vector<std::string> m_entries;
    int m_selected;


    ftxui::Component m_component;
    static constexpr Screen SCREEN_TYPE = Screen::Multiplayer_Select;


};

}
#endif