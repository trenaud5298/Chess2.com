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
}

void MultiplayerSelectScreen::onLeave() {

}

void MultiplayerSelectScreen::requestExit() {
    m_clientPanel.gameClient().returnToIdle();
}


void MultiplayerSelectScreen::addServerEntry(ServerInfo serverInfo) {
    // Add Server Entry Here
    rebuildServerEntries();
}

void MultiplayerSelectScreen::removeServerEntry(std::uint64_t index) {
    // Remove Server Entry Here
    rebuildServerEntries();
}

ftxui::Component MultiplayerSelectScreen::buildComponent() {
    rebuildServerEntries();

    ftxui::MenuOption menuOption;
    menuOption.entries_option.transform = std::bind_front(&MultiplayerSelectScreen::renderServerEntry, this);;

    auto menu = ftxui::Menu(&m_entries, &m_selected, menuOption);

    ftxui::ButtonOption serverRequiredButtonOption = ftxui::ButtonOption::Animated();
    auto originalRequiredTransform = serverRequiredButtonOption.transform;
    serverRequiredButtonOption.transform = [this, originalRequiredTransform](const ftxui::EntryState& state) {
        if (m_servers.empty()) {
            return ftxui::text(state.label) | ftxui::center | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 20) | ftxui::borderEmpty
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
        m_clientPanel.pushModal(std::make_unique<ConfirmModal>(m_clientPanel, "Are you sure you want to delete this server? This will permanently remove any saved data associated with this server.", [this]() {
            removeServerEntry(m_selected);
        }));
    }, serverRequiredButtonOption);

    auto joinButton = ftxui::Button("Join Server", [this]() {
        // TODO: Implement Joining Button
    }, serverRequiredButtonOption);


    auto buttons = ftxui::Container::Horizontal({addButton, removeButton, joinButton, removeButton});


    auto layout = ftxui::Container::Vertical({menu, buttons});


    auto renderer = ftxui::Renderer(layout, [this, menu, addButton, removeButton, joinButton]() {
        ftxui::Element serverList;
        if (!m_servers.empty()) {
            serverList = menu->Render() | ftxui::frame | ftxui::flex | ftxui::vscroll_indicator;
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

        return ftxui::vbox({serverList, ftxui::separator(), buttonRow});
    });


    return renderer;
}


void MultiplayerSelectScreen::rebuildServerEntries() {
    m_servers = m_clientPanel.gameClient().persistenceManager().settings().getServers();
    m_entries.assign(m_servers.size(), "");
    m_selected = std::clamp(m_selected, 0, std::max(0, (int)m_servers.size()-1));
}

ftxui::Element MultiplayerSelectScreen::renderServerEntry(const ftxui::EntryState& state) {
    ServerInfo& serverInfo = m_servers[state.index];

    auto left =  ftxui::vbox({
        ftxui::text(serverInfo.serverName) | ftxui::bold,
        ftxui::text(serverInfo.ip) | ftxui::dim
    });

    auto right = ftxui::vbox({
        ftxui::text("Placeholder 1"),
        ftxui::text("Placeholder 2")
    });

    auto row = ftxui::hbox({
        left,
        ftxui::filler(),
        right
    });

    if (state.focused || state.active) {
        row |= ftxui::inverted;
    }

    return ftxui::vbox({row, ftxui::separatorLight()});
}
} // namespace Chess