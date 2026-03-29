/*
* Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Modal/TwoButtonModal.hpp>
#include <Chess/Client/UI/ClientPanel.hpp>
#include <utility>
// ASIO Includes

// FTXUI Includes

// C++ Includes


namespace Chess {

TwoButtonModal::TwoButtonModal(ClientPanel& clientPanel, std::string title, std::string message, std::string buttonText1, std::function<void()> onButton1, std::string buttonText2, std::function<void()> onButton2)
: ModalInterface(clientPanel) {

    ftxui::ButtonOption buttonOption = ftxui::ButtonOption::Animated();
    buttonOption.transform = [](const ftxui::EntryState& s) {
        auto label = ftxui::text(s.label) | ftxui::center | ftxui::borderEmpty;
        if (s.focused) {label |= ftxui::bold;}
        return label;
    };
    auto button1 = ftxui::Button(buttonText1, [this, onButton1]() {
        m_clientPanel.popModal();
        if (onButton1) { onButton1(); }
    }, buttonOption);
    auto button2 = ftxui::Button(buttonText2, [this, onButton2]() {
        m_clientPanel.popModal();
        if (onButton2) { onButton2(); }
    }, buttonOption);
    auto buttons = ftxui::Container::Horizontal({ button1, button2 });

    m_component = ftxui::Renderer(buttons, [button1, button2, title, message]() {
        return ftxui::vbox({
            ftxui::text(title) | ftxui::center,
            ftxui::separator(),
            ftxui::paragraph(message),
            ftxui::filler(),
            ftxui::hbox({
                ftxui::filler(),
                button1->Render(),
                ftxui::text("   "),
                button2->Render(),
                ftxui::filler(),
            }),
        }) | ftxui::size(ftxui::WIDTH, ftxui::GREATER_THAN, 36);
    });
}


TwoButtonModal::~TwoButtonModal() {

}




[[nodiscard]] ftxui::Component TwoButtonModal::getComponent() {
    return m_component;
}



}
