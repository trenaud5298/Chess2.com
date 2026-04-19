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
}

void MultiplayerLobbyScreen::onLeave() {
    if (m_globalChatPane) {
        m_globalChatPane->onLeave();
    }
    m_clientPanel.setTickRate(std::nullopt);
}


void MultiplayerLobbyScreen::requestExit() {
    m_clientPanel.handleStatus(
        m_clientPanel.gameClient().requestMultiplayerDisconnect(),
        "Unable to disconnect from server",
        ResultPolicy::Modal
    );
}


ftxui::Component MultiplayerLobbyScreen::buildComponent() {

    auto rightPane = ftxui::Renderer([]() {
        return ftxui::vbox({
            ftxui::text("Server Lobby") | ftxui::bold,
            ftxui::separator(),
            ftxui::text("placeholder"),
            ftxui::separator(),
        }) | ftxui::border | ftxui::flex | ftxui::yflex;
    });

    auto layout = ftxui::Container::Horizontal({
        m_globalChatPane->component(),
        rightPane
    });

    return ftxui::Renderer(layout, [this, rightPane]() {
        return ftxui::hbox({
            ftxui::vbox({
                ftxui::text("Global Chat") | ftxui::bold,
                ftxui::separator(),
                m_globalChatPane->component()->Render() | ftxui::flex | ftxui::yflex
            }) | ftxui::border | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 38) | ftxui::yflex,
            ftxui::text(" "),
            rightPane -> Render() | ftxui::flex | ftxui::yflex
        }) | ftxui::flex;
    });
}



} // namespace Chess