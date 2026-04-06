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
#include <utility>

// ASIO Includes

// FTXUI Includes

// C++ Includes


namespace Chess {
AddServerModal::AddServerModal(ClientPanel &clientPanel, const std::function<void(ServerInfo)>& onAdd) : ModalInterface(clientPanel) {

    ftxui::InputOption serverNameInputFilterOption;
    serverNameInputFilterOption.on_change = [this]() {
        if (m_serverName.size() > MAX_SERVERNAME_LENGTH) {m_serverName.resize(MAX_SERVERNAME_LENGTH);}
    };
    auto serverNameInput = ftxui::Input(&m_serverName, "Server Name", serverNameInputFilterOption);

    ftxui::InputOption ipInputFilterOption;
    ipInputFilterOption.on_change = [this]() {
        auto removedChars = std::remove_if(m_ip.begin(), m_ip.end(), [](auto c) {
            return !std::isdigit(c) && c != '.' && c != ':';
        });
        m_ip.erase(removedChars, m_ip.end());
        if (m_ip.size() > MAX_IP_LENGTH) {m_ip.resize(MAX_IP_LENGTH);}
    };
    auto serverIpInput = ftxui::Input(&m_ip, "127.0.0.1", ipInputFilterOption);

    ftxui::InputOption passwordInputFilterOption;
    passwordInputFilterOption.password = true;
    passwordInputFilterOption.on_change = [this]() {
        if (m_password.size() > MAX_PASSWORD_LENGTH) {m_password.resize(MAX_PASSWORD_LENGTH);}
    };
    auto serverPasswordInput = ftxui::Input(&m_password, "Optional", passwordInputFilterOption);


    ftxui::ButtonOption confirmButtonOption = ftxui::ButtonOption::Animated();
    auto originalConfirmTransformation = confirmButtonOption.transform;
    confirmButtonOption.transform = [this, originalConfirmTransformation](const ftxui::EntryState& state) {
        if (m_ip.empty()) {
            return ftxui::text(state.label) | ftxui::center | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 20) | ftxui::borderEmpty
                | ftxui::color(ftxui::Color::GrayDark) | ftxui::bgcolor(ftxui::Color::Black);
        }
        return originalConfirmTransformation(state) | ftxui::center | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 20);
    };
    auto confirmButton = ftxui::Button("Confirm", [this, onAdd]() {
        if (!m_ip.empty()) {
            onAdd(ServerInfo{.serverName = m_serverName, .ip = m_ip, .password =m_password});
            requestDismiss();
        }
    }, confirmButtonOption);

    ftxui::ButtonOption cancelButtonOption = ftxui::ButtonOption::Animated();
    auto originalCancelTransformation = cancelButtonOption.transform;
    cancelButtonOption.transform = [this, originalCancelTransformation](const ftxui::EntryState& state) {
        return originalCancelTransformation(state) | ftxui::center | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 20);
    };
    auto cancelButton = ftxui::Button("Cancel", [this]() {
        requestDismiss();
    }, ftxui::ButtonOption::Animated());

    auto inputs = ftxui::Container::Vertical({serverNameInput, serverIpInput, serverPasswordInput});

    auto buttons = ftxui::Container::Horizontal({confirmButton, cancelButton});

    auto layout = ftxui::Container::Vertical({inputs, buttons});

    m_component = ftxui::Renderer(layout, [serverNameInput, serverIpInput, serverPasswordInput, confirmButton, cancelButton]() {
        auto renderedInputs = ftxui::vbox({
            ftxui::hbox({ftxui::text("Server Name: "), serverNameInput->Render()}),
            ftxui::filler() | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 2),
            ftxui::hbox({ftxui::text("Server IP:   "), serverIpInput->Render()}),
            ftxui::filler() | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 2),
            ftxui::hbox({ftxui::text("Password:    "), serverPasswordInput->Render()})
        });

        auto renderedButtons = ftxui::hbox({
            confirmButton->Render(),
            ftxui::filler(),
            cancelButton->Render()
        });

        return ftxui::vbox({
            ftxui::text("Add Server") | ftxui::bold | ftxui::hcenter,
            ftxui::separator(),
            renderedInputs,
            ftxui::separator(),
            renderedButtons
        }) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 40);
    });
}


AddServerModal::~AddServerModal() {

}

ftxui::Component AddServerModal::getComponent() {
    return m_component;
}


}
