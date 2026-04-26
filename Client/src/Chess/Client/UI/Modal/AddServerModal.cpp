/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Modal/AddServerModal.hpp>
#include <Chess/Client/UI/ClientPanel.hpp>


// ASIO Includes

// FTXUI Includes

// C++ Includes
#include <utility>
#include <cctype>

namespace Chess {

namespace {

std::optional<std::string> validateServerIp(std::string_view text) {
    if (text.empty()) {
        return std::string("Server IP is required");
    }

    for (char c : text) {
        if (!std::isdigit(static_cast<unsigned char>(c)) && c != '.' && c != ':') {
            return std::string("Only digits, '.' and ':' are allowed");
        }
    }
    return std::nullopt;
}

TextField::Config makeServerNameConfig() {
    return TextField::Config{
        .label = "Server Name",
        .initialValue = "",
        .placeholder = "My Server",
        .maxLength = 64,
        .allowEmpty = true,
        .secret = false
    };
}

TextField::Config makeServerIpConfig() {
    return TextField::Config{
        .label = "Server IP",
        .initialValue = "",
        .placeholder = "127.0.0.1",
        .maxLength = 45,
        .allowEmpty = false,
        .secret = false,
        .validator = validateServerIp
    };
}

TextField::Config makePasswordConfig() {
    return TextField::Config{
        .label = "Password",
        .initialValue = "",
        .placeholder = "Optional",
        .maxLength = 64,
        .allowEmpty = true,
        .secret = true,
    };
}

ftxui::ButtonOption makeButtonOption() {
    ftxui::ButtonOption buttonOption = ftxui::ButtonOption::Animated();
    auto original = buttonOption.transform;
    buttonOption.transform = [original](const ftxui::EntryState& state) {
        return original(state) | ftxui::center | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 18);
    };
    return buttonOption;
}



}
AddServerModal::AddServerModal(ClientPanel &clientPanel, const std::function<void(ServerInfo)>& onAdd)
: ModalInterface(clientPanel), m_onAdd(onAdd), m_serverNameField(makeServerNameConfig()),
m_ipField(makeServerIpConfig()), m_passwordField(makePasswordConfig()),
m_confirmButton(MakeConditionalButton({
    .label = "Confirm",
    .onPress = std::bind_front(&AddServerModal::submit, this),
    .isEnabled = std::bind_front(&AddServerModal::canSubmit, this),
    .width = 18,
    .centerLabel = true
})),
m_cancelButton(MakeConditionalButton({
    .label = "Cancel",
    .onPress = std::bind_front(&AddServerModal::requestDismiss, this),
    .width = 18,
    .centerLabel = true
})) {
    auto buttonRow = ftxui::Container::Horizontal({
        m_confirmButton,
        m_cancelButton,
    });

    m_form = ftxui::Container::Vertical({
        m_serverNameField.component(),
        m_ipField.component(),
        m_passwordField.component(),
        buttonRow,
    });

    m_component = ftxui::Renderer(m_form, [this]() {
        return ftxui::vbox({
            ftxui::text("Add Server") | ftxui::bold | ftxui::center,
            ftxui::separator(),
            m_serverNameField.renderRow(),
            m_ipField.renderRow(),
            m_passwordField.renderRow(),
            ftxui::separator(),
            ftxui::hbox({
                m_confirmButton->Render(),
                ftxui::text("  "),
                m_cancelButton->Render()
            }) | ftxui::center
        }) | ftxui::size(ftxui::WIDTH, ftxui::GREATER_THAN, 48);
    });
}


bool AddServerModal::canSubmit() const {
    return m_ipField.isValid();
}

void AddServerModal::submit() {
    if (!canSubmit()) {
        return;
    }

    if (m_onAdd) {
        m_onAdd(ServerInfo{
            .serverName = m_serverNameField.text(),
            .ip = m_ipField.text(),
            .password = m_passwordField.text(),
            .uuid = 0
        });
    }

    requestDismiss();
}


AddServerModal::~AddServerModal() {

}

ftxui::Component AddServerModal::getComponent() {
    return m_component;
}


}
