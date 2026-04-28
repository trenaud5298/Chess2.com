/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Modal/ConfirmModal.hpp>
#include <Chess/Client/UI/ClientPanel.hpp>
#include <utility>
// ASIO Includes

// FTXUI Includes

// C++ Includes


namespace Chess {

ConfirmModal::ConfirmModal(ClientPanel& clientPanel, std::string message, std::function<void()> onConfirm, std::function<void()> onCancel)
: ModalInterface(clientPanel), m_message(std::move(message)), m_onConfirm(std::move(onConfirm)), m_onCancel(std::move(onCancel)) {
    auto yesButton = ftxui::Button("  Yes  ", [this]() {
        m_clientPanel.popModal();
        if (m_onConfirm) { m_onConfirm(); }
    }, ftxui::ButtonOption::Simple());

    auto noButton = ftxui::Button("  No   ", [this]() {
        m_clientPanel.popModal();
        if (m_onCancel) { m_onCancel(); }
    }, ftxui::ButtonOption::Simple());

    m_buttons = ftxui::Container::Horizontal({ yesButton, noButton });

    m_component = ftxui::Renderer(m_buttons, [this, yesButton, noButton]() {
        return ftxui::vbox({
            ftxui::text(m_message) | ftxui::center,
            ftxui::separator(),
            ftxui::hbox({
                ftxui::filler(),
                yesButton->Render(),
                ftxui::text("   "),
                noButton->Render(),
                ftxui::filler(),
            }),
        }) | ftxui::size(ftxui::WIDTH, ftxui::GREATER_THAN, 36);
    });
}


ConfirmModal::~ConfirmModal() {

}


void ConfirmModal::onEnter() {

}


void ConfirmModal::onLeave() {

}

void ConfirmModal::requestDismiss() {
    m_clientPanel.popModal();
    if (m_onCancel) { m_onCancel(); }
}


[[nodiscard]] ftxui::Component ConfirmModal::getComponent() {
    return m_component;
}



}
