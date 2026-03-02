/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Server/Panel/StatusTab.hpp>

// ASIO Includes

// C++ Includes
#include <iostream>



namespace Chess {

StatusTab::StatusTab() {
    build();
}

ftxui::Component StatusTab::getComponent() {
    return m_component;
}

void StatusTab::build() {
    m_startButton = ftxui::Button("Start", [&] {
        std::cout<<"Super simple start button"<<std::endl;
    });
    m_stopButton = ftxui::Button("Stop", [&] {
        std::cout<<"Super simple stop button"<<std::endl;
    });
    m_buttons = ftxui::Container::Horizontal({m_startButton, m_stopButton});

    m_component = ftxui::Renderer(m_buttons, [&] {
        return ftxui::vbox({
            ftxui::text("This is a test") | ftxui::bold ,
            m_buttons->Render()
        });
    });
}



}