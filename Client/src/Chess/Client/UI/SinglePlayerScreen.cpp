/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/SinglePlayerScreen.hpp>
#include <Chess/Client/UI/ClientPanel.hpp>

// FTXUI Includes
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

// C++ Includes

namespace Chess {

    SinglePlayerScreen::SinglePlayerScreen(ClientPanel& clientPanel)
    : m_clientPanel(clientPanel) {
        auto backButton = ftxui::Button("  Back  ", [&] {
            m_clientPanel.navigateBack();
        }, ftxui::ButtonOption::Animated());

        m_component = ftxui::Renderer(backButton, [&, backButton] {
            return ftxui::vbox({
                ftxui::filler(),
                ftxui::text("Single Player") | ftxui::bold | ftxui::center,
                ftxui::text("Not yet implemented") | ftxui::dim | ftxui::center,
                ftxui::separatorEmpty(),
                backButton->Render() | ftxui::center,
                ftxui::filler(),
            });
        });
    }

    SinglePlayerScreen::~SinglePlayerScreen() {}

    ftxui::Component SinglePlayerScreen::getComponent() {
        return m_component;
    }

    void SinglePlayerScreen::onEnter() {}

    void SinglePlayerScreen::onLeave() {}

} // namespace Chess