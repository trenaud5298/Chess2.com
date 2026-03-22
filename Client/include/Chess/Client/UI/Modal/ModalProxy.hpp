#ifndef CHESS_CLIENT_UI_MODAL_MODALPROXY_HPP
#define CHESS_CLIENT_UI_MODAL_MODALPROXY_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Modal/ModalInterface.hpp>

// ASIO Includes

// FTXUI Includes
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>

// C++ Includes
#include <cstdint>

namespace Chess {

class ModalProxy : public ftxui::ComponentBase {
public:
    explicit ModalProxy(std::vector<std::unique_ptr<ModalInterface>>& stack)
        : m_stack(stack) {}

    ftxui::Element OnRender() override {
        if (m_stack.empty()) { return ftxui::text(""); }
        return m_stack.back()->getComponent()->Render()
               | ftxui::border
               | ftxui::clear_under
               | ftxui::center;
    }

    bool OnEvent(ftxui::Event event) override {
        if (m_stack.empty()) { return false; }
        return m_stack.back()->getComponent()->OnEvent(event);
    }

    bool Focusable() const override {return true;}

private:
    std::vector<std::unique_ptr<Chess::ModalInterface>>& m_stack;
};

}

#endif