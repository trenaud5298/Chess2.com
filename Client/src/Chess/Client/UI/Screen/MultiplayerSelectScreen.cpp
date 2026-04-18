/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Screen/MultiplayerSelectScreen.hpp>
#include <Chess/Client/UI/ClientPanel.hpp>
#include <Chess/Client/UI/Modal/AddServerModal.hpp>
#include <Chess/Client/UI/Modal/ConfirmModal.hpp>

// FTXUI Includes

// C++ Includes

namespace Chess {

MultiplayerSelectScreen::MultiplayerSelectScreen(ClientPanel& clientPanel)
: ScreenInterface(clientPanel) {
    m_component = buildComponent();
}

MultiplayerSelectScreen::~MultiplayerSelectScreen() {}

ftxui::Component MultiplayerSelectScreen::getComponent() {
    return m_component;
}

void MultiplayerSelectScreen::onEnter() {
    rebuildServerEntries();
    m_clientPanel.setTickRate(std::chrono::milliseconds(50));
}

void MultiplayerSelectScreen::onLeave() {
    m_clientPanel.setTickRate(std::nullopt);
}

bool MultiplayerSelectScreen::canRequestExit() const {
    if (m_clientPanel.gameClient().state() == ClientState::MultiplayerConnecting) {
        return false;
    }
    return true;
}

void MultiplayerSelectScreen::requestExit() {
    if (m_clientPanel.gameClient().state() == ClientState::MultiplayerConnecting) {
        m_clientPanel.handleStatus(
            m_clientPanel.gameClient().requestMultiplayerDisconnect(),
            "Unable to cancel multiplayer connection"
        );
        return;
    }

    m_clientPanel.handleStatus(
        m_clientPanel.gameClient().returnToIdle(),
        "Unable to return to main menu"
    );
}


void MultiplayerSelectScreen::addServerEntry(ServerInfo serverInfo) {
    // Add Server Entry Here
    m_servers.push_back(serverInfo);
    m_entries.push_back("");
    m_clientPanel.gameClient().persistenceManager().settings().setServers(m_servers);
    rebuildServerEntries();
    m_selectedServer = m_servers.size()-1;
}

void MultiplayerSelectScreen::removeSelectedServer() {
    if (!hasSelectedServerEntry()) {
        return;
    }
    m_servers.erase(m_servers.begin()+*m_selectedServer);
    m_entries.erase(m_entries.begin()+*m_selectedServer);
    m_clientPanel.gameClient().persistenceManager().settings().setServers(m_servers);
    rebuildServerEntries();
}

void MultiplayerSelectScreen::joinSelectedServer() {
    if (!hasSelectedServerEntry()) {
        return;
    }

    m_clientPanel.handleStatus(
        m_clientPanel.gameClient().requestMultiplayerConnect(m_servers[*m_selectedServer]),
        "Unable to join server",
        ResultPolicy::Modal
    );
}

ftxui::Component MultiplayerSelectScreen::buildComponent() {
    rebuildServerEntries();

    ftxui::MenuOption menuOption;
    menuOption.entries_option.transform = std::bind_front(&MultiplayerSelectScreen::renderServerEntry, this);;
    menuOption.focused_entry = &m_cursor;
    menuOption.on_enter = [this]() {activateCursor();};
    auto menuCore = ftxui::Menu(&m_entries, &m_cursor, menuOption);

    auto menuEventCatcher = ftxui::CatchEvent(menuCore, [this, menuCore](ftxui::Event event) {
        if (event.is_mouse() && event.mouse().button == ftxui::Mouse::Left && event.mouse().motion == ftxui::Mouse::Pressed) {
            bool handled = menuCore->OnEvent(event);
            if (handled) {
                activateCursor();
            } else {
                m_selectedServer = std::nullopt;
            }
            return handled;
        }
        return false;
    });

    ftxui::ButtonOption serverRequiredButtonOption = ftxui::ButtonOption::Animated();
    auto originalRequiredTransform = serverRequiredButtonOption.transform;
    serverRequiredButtonOption.transform = [this, originalRequiredTransform](const ftxui::EntryState& state) {
        if (!hasSelectedServerEntry()) {
            return ftxui::text(state.label) | ftxui::center | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 20)
                | ftxui::color(ftxui::Color::GrayDark) | ftxui::bgcolor(ftxui::Color::Black);
        }
        return originalRequiredTransform(state) | ftxui::center | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 20);
    };

    ftxui::ButtonOption serverConstantButtonOption = ftxui::ButtonOption::Animated();
    auto originalConstantTransform = serverConstantButtonOption.transform;
    serverConstantButtonOption.transform = [originalConstantTransform](const ftxui::EntryState& state) {
        return originalConstantTransform(state) | ftxui::center | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 20);
    };

    auto addButton = ftxui::Button("Add Server", [this]() {
        m_clientPanel.pushModal(std::make_unique<AddServerModal>(m_clientPanel, std::bind_front(&MultiplayerSelectScreen::addServerEntry, this)));
    }, serverConstantButtonOption);


    auto removeButton = ftxui::Button("Remove Server", [this]() {
        if (!hasSelectedServerEntry()) {
            return;
        }
        m_clientPanel.pushModal(std::make_unique<ConfirmModal>(m_clientPanel, "Are you sure you want to delete this server? This will permanently remove any saved data associated with this server.", [this]() {
            removeSelectedServer();
        }));
    }, serverRequiredButtonOption);

    auto joinButton = ftxui::Button("Join Server", [this]() {
        if (!hasSelectedServerEntry()) {
            return;
        }
        joinSelectedServer();
    }, serverRequiredButtonOption);

    auto buttons = ftxui::Container::Horizontal({addButton, removeButton, joinButton});

    auto layout = ftxui::Container::Vertical({menuEventCatcher, buttons});
    auto layoutEventCatcher = ftxui::CatchEvent(layout, [this, menuCore, buttons](ftxui::Event event) {
        if (event == ftxui::Event::Tab || event == ftxui::Event::TabReverse) {
            if (menuCore->Focused()) {
                buttons->TakeFocus();
                return true;
            }
            if (buttons->Focused()) {
                menuCore->TakeFocus();
                return true;
            }
        }
        return false;

    });

    auto statusText = [this]() -> std::string {
        switch (m_clientPanel.gameClient().multiplayerState()) {
            case MultiplayerState::Idle: return "Idle";
            case MultiplayerState::ConnectingNetwork: return "Connecting...";
            case MultiplayerState::AwaitingLogin: return "Awaiting Login...";
            case MultiplayerState::Connected: return "Connected";
        }
        return "";
    };

    auto renderer = ftxui::Renderer(layoutEventCatcher, [this, menuEventCatcher, addButton, removeButton, joinButton, statusText]() {
        ftxui::Element serverList;
        if (!m_servers.empty()) {
            serverList = menuEventCatcher->Render() | ftxui::frame | ftxui::flex | ftxui::vscroll_indicator;
        } else {
            serverList = ftxui::text("No servers added. Try adding a server.") | ftxui::dim | ftxui::center | ftxui::flex;
        }


        ftxui::Element buttonRow = ftxui::hbox({
            addButton->Render(),
            ftxui::filler() | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 4),
            removeButton->Render(),
            ftxui::filler() | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 4),
            joinButton->Render()
        }) | ftxui::center;

        return ftxui::vbox({serverList, ftxui::separator(), buttonRow, ftxui::text(statusText())});
    });


    return renderer;
}

bool MultiplayerSelectScreen::hasSelectedServerEntry() const {
    return m_selectedServer.has_value() &&
           *m_selectedServer >= 0 &&
           *m_selectedServer < static_cast<int>(m_servers.size());
}

void MultiplayerSelectScreen::activateCursor() {
    if (m_servers.empty()) {
        return;
    }

    if (hasSelectedServerEntry() && *m_selectedServer == m_cursor) {
        joinSelectedServer();
        return;
    }

    if (!m_servers.empty()) {
        m_selectedServer = m_cursor;
    }
}

void MultiplayerSelectScreen::rebuildServerEntries() {
    m_servers = m_clientPanel.gameClient().persistenceManager().settings().getServers();
    m_entries.assign(m_servers.size(), "");
    m_cursor = 0;
    m_selectedServer = std::nullopt;
}

ftxui::Element MultiplayerSelectScreen::renderServerEntry(const ftxui::EntryState& state) {
    ServerInfo& serverInfo = m_servers[state.index];

    auto left =  ftxui::vbox({
        ftxui::text(serverInfo.serverName) | ftxui::bold,
        ftxui::filler()  | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 1),
        ftxui::text(serverInfo.ip) | ftxui::dim,
    });

    auto right = ftxui::vbox({
        ftxui::text("Placeholder 1"),
        ftxui::filler() | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 1),
        ftxui::text("Placeholder 2"),
    });

    auto row = ftxui::hbox({
        left,
        ftxui::filler(),
        right
    });

    if (state.focused) {
        row |= ftxui::inverted;
    }

    if (hasSelectedServerEntry() && *m_selectedServer == state.index) {
        row |= ftxui::border;
    } else {
        row |= ftxui::borderEmpty;
    }

    return row;
}
} // namespace Chess