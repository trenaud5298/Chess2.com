/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Screen/MainMenuScreen.hpp>
#include <Chess/Client/UI/ClientPanel.hpp>

// FTXUI Includes
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

// C++ Includes

namespace Chess {


MainMenuScreen::MainMenuScreen(ClientPanel &clientPanel) : ScreenInterface(clientPanel) {
    auto buttonOption = ftxui::ButtonOption::Animated();
    buttonOption.transform = [](const ftxui::EntryState& s) {
        auto label = ftxui::text(s.label) | ftxui::center | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 20) | ftxui::borderEmpty;
        if (s.focused) {label |= ftxui::bold;}

        return label;
    };

    auto buttons = ftxui::Container::Vertical({
        ftxui::Button("  Singleplayer  ", [&] {
            m_clientPanel.handleStatus(
                m_clientPanel.gameClient().enterSingleplayerSetup(),
                "Unable to open singleplayer setup"
            );
        }, buttonOption),
        ftxui::Button("  Multiplayer   ", [&] {
            m_clientPanel.handleStatus(
                m_clientPanel.gameClient().enterMultiplayerSetup(),
                "Unable to open multiplayer setup"
            );
        }, buttonOption),
        ftxui::Button("      Quit      ", [&] {
            m_clientPanel.quit();
        }, buttonOption)
    });

    m_component = ftxui::Renderer(buttons, [&, buttons] {
        auto statusBar = ftxui::hbox({
            ftxui::text(" ♟ Username") | ftxui::dim,
            ftxui::filler(),
        });

        return ftxui::vbox({
            buttons->Render() | ftxui::center | ftxui::flex,
            ftxui::separator(),
            statusBar,
        });
    });
}

MainMenuScreen::~MainMenuScreen() {

}

void MainMenuScreen::requestExit() {
    m_clientPanel.quit();
}

ftxui::Component MainMenuScreen::getComponent() {
    return m_component;
}



}
