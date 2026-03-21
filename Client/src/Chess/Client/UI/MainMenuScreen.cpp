/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/MainMenuScreen.hpp>
#include <Chess/Client/UI/ClientPanel.hpp>

// FTXUI Includes
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

// C++ Includes

namespace Chess {


MainMenuScreen::MainMenuScreen(ClientPanel &clientPanel) : m_clientPanel(clientPanel) {
    auto buttonOption = ftxui::ButtonOption::Animated();
    buttonOption.transform = [](const ftxui::EntryState& s) {
        auto label = ftxui::text(s.label) | ftxui::center | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 20) | ftxui::borderEmpty;
        if (s.focused) {label |= ftxui::bold;}

        return label;
    };

    auto buttons = ftxui::Container::Vertical({
        ftxui::Button("  Singleplayer  ", [&] {
            // m_clientPanel.navigateTo(Screen::Singleplayer);
        }, buttonOption),
        ftxui::Button("  Multiplayer   ", [&] {
            m_clientPanel.navigateTo(Screen::Multiplayer);
        }, buttonOption),
        ftxui::Button("    Settings    ", [&] {
            m_clientPanel.navigateTo(Screen::Settings);
        }, buttonOption),
        ftxui::Button("      Quit      ", [&] {
            m_clientPanel.quit();
        }, buttonOption)
    });

    m_component = ftxui::Renderer(buttons, [&, buttons] {
        auto title = ftxui::vbox({
            ftxui::text("♔  C H E S S  ♚") | ftxui::bold | ftxui::center,
            ftxui::text("a chess client") | ftxui::dim | ftxui::center,
        });

        auto statusBar = ftxui::hbox({
            ftxui::text(" ♟ Username") | ftxui::dim,
            ftxui::filler(),
        });

        return ftxui::vbox({
            title,
            ftxui::separator(),
            buttons->Render() | ftxui::center | ftxui::flex,
            ftxui::separator(),
            statusBar,
        });
    });
}

MainMenuScreen::~MainMenuScreen() {

}


ftxui::Component MainMenuScreen::getComponent() {
    return m_component;
}

void MainMenuScreen::onEnter() {

}

void MainMenuScreen::onLeave() {

}


}
