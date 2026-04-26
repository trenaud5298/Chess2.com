/*
* Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Modal/ErrorModal.hpp>
#include <Chess/Client/UI/ClientPanel.hpp>
#include <utility>
// ASIO Includes

// FTXUI Includes

// C++ Includes


namespace Chess {

ErrorModal::ErrorModal(ClientPanel& clientPanel, std::string message)
: ModalInterface(clientPanel), m_message(std::move(message)) {
    auto okButton = ftxui::Button("  Ok  ", [this]() {
        m_clientPanel.popModal();
    }, ftxui::ButtonOption::Simple());

    m_component = ftxui::Renderer(okButton, [this, okButton]() {
        return ftxui::vbox({
            ftxui::text("Error") | ftxui::center,
            ftxui::separator(),
            ftxui::text(m_message) | ftxui::center,
            ftxui::filler(),
            ftxui::separator(),
            okButton->Render(),
        }) | ftxui::size(ftxui::WIDTH, ftxui::GREATER_THAN, 36);
    });
}


ErrorModal::~ErrorModal() {

}


[[nodiscard]] ftxui::Component ErrorModal::getComponent() {
    return m_component;
}



}
