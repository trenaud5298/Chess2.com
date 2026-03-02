/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Server/Panel/LogTab.hpp>
#include <Chess/Core/UI/ScrollableTextWindow.hpp>

// ASIO Includes

// C++ Includes
#include <iostream>

#include "ftxui/component/event.hpp"


namespace Chess {


// Log Tab
LogTab::LogTab() {
    build();
}

ftxui::Component LogTab::getComponent() {
    return m_component;
}

void LogTab::build() {
    m_logWindow = std::make_shared<ScrollableTextWindow>();
    m_logWindow->addText("Test 1");

    m_logOptions = ftxui::Button("Test", [&]() {
        log("Test Button");
    });
    m_component = ftxui::Renderer(ftxui::Container::Horizontal({m_logWindow, m_logOptions}), [&]() {
        return ftxui::hbox({
            m_logWindow->Render() | ftxui::yframe | ftxui::flex,
            ftxui::separator(),
            m_logOptions->Render(),
        });
    });
}

void LogTab::log(const std::string& log) {
    m_logWindow->addText(log);
}

}
