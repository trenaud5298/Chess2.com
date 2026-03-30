#ifndef CHESS_CLIENT_UI_MODAL_MODALINTERFACE_HPP
#define CHESS_CLIENT_UI_MODAL_MODALINTERFACE_HPP

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
#include <ftxui/dom/elements.hpp>

// C++ Includes
#include <cstdint>

namespace Chess {

class ClientPanel;

class ModalInterface {
public:
    explicit ModalInterface(ClientPanel& panel) : m_clientPanel(panel) {}
    virtual ~ModalInterface() = default;

    // Lifecycle
    virtual void onEnter() {}
    virtual void onLeave() {}
    virtual void onTick() {}

    // Dismiss affordance
    virtual bool canRequestDismiss() const { return true; }
    virtual void requestDismiss() {}

    [[nodiscard]] virtual ftxui::Component getComponent() = 0;
    [[nodiscard]] virtual ftxui::Element backdropElement() {
        return ftxui::text("") | ftxui::flex | ftxui::bgcolor(ftxui::Color::Black);
    }

protected:
    ClientPanel& m_clientPanel;
};


}

#endif
