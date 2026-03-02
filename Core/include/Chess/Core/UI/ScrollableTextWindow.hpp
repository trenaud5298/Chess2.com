#ifndef CHESS_CORE_UI_SCROLLABLETEXTWINDOW_HPP
#define CHESS_CORE_UI_SCROLLABLETEXTWINDOW_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes

// ASIO Includes

// FTXUI Includes
#include <ftxui/component/component.hpp>

// C++ Includes
#include <deque>
#include <cstdint>
#include <mutex>
#include <deque>


namespace Chess {

class ScrollableTextWindow : public ftxui::ComponentBase {

public:
    ScrollableTextWindow() = default;
    ftxui::Element OnRender() override;
    bool OnEvent(ftxui::Event event) override;
    bool Focusable() const override {return true;}

    void addText(const std::string& text);
private:
    std::deque<std::string> m_texts;
    std::mutex m_textMutex;
    std::size_t m_selectedIndex = 0;
    bool m_autoscroll = true;

    constexpr static std::size_t MAX_TEXTS_VISIBLE = 50;
    constexpr static std::size_t MAX_TEXTS = 1000;
};



}

#endif