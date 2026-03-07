/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Core/UI/ScrollableTextWindow.hpp>

// FTXUI Includes
#include <ftxui/component/event.hpp>

// ASIO Includes

// C++ Includes


#include <iostream>

namespace Chess {


ftxui::Element ScrollableTextWindow::OnRender() {
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<ftxui::Element> elements;
    elements.push_back(ftxui::filler() | ftxui::yflex_grow);
    {
        std::lock_guard<std::mutex> textLock(m_textMutex);
        for (const std::string& m_text : m_texts) {
            elements.push_back(ftxui::paragraph(m_text));
        }
        if (m_selectedIndex < m_texts.size()) {
            (elements[m_selectedIndex] |= ftxui::focus) |= ftxui::inverted;
        } else {
            elements.push_back(ftxui::emptyElement() | ftxui::focus);
        }
    }
    ftxui::Element result = ftxui::vbox(std::move(elements)) | ftxui::yframe;
    if (Focused()) {
        result |= ftxui::inverted;
    }
    return std::move(result);
}

bool ScrollableTextWindow::OnEvent(ftxui::Event event) {
    if (!Focused())
        return false;

    if (event == ftxui::Event::Home) {
        m_selectedIndex = 0;
        return true;
    }

    if (event == ftxui::Event::PageUp) {
        if (m_selectedIndex > 0) {
            --m_selectedIndex;
        }
        return true;
    }

    if (event == ftxui::Event::End) {
        std::lock_guard<std::mutex> textLock(m_textMutex);
        m_selectedIndex = m_texts.size();
        return true;
    }

    if (event == ftxui::Event::PageDown) {
        std::lock_guard<std::mutex> textLock(m_textMutex);
        if (m_selectedIndex < m_texts.size()) {
            ++m_selectedIndex;
        }
        return true;
    }

    return false;
}


void ScrollableTextWindow::addText(const std::string& text) {
    std::lock_guard<std::mutex> textLock(m_textMutex);
    m_texts.push_back(text);
    if (m_texts.size() > MAX_TEXTS) {
        m_texts.pop_front();
    }
    if (m_selectedIndex == m_texts.size() - 1) {
        ++m_selectedIndex;
    }
}

}
