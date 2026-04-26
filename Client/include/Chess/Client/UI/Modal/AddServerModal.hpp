#ifndef CHESS_CLIENT_UI_MODAL_ADDSERVERMODAL_HPP
#define CHESS_CLIENT_UI_MODAL_ADDSERVERMODAL_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Modal/ModalInterface.hpp>
#include <Chess/Client/Common/ServerInfo.hpp>
#include <Chess/Core/UI/Form/TextField.hpp>
#include <Chess/Core/UI/ConditionalButton.hpp>

// FTXUI Includes
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

// C++ Includes
#include <functional>


namespace Chess {

class ClientPanel;

class AddServerModal : public ModalInterface {
public:
    AddServerModal(ClientPanel& clientPanel, const std::function<void(ServerInfo)>& onAdd);
    ~AddServerModal() override;

    [[nodiscard]] ftxui::Component getComponent() override;

private:
    [[nodiscard]] bool canSubmit() const;
    void submit();

private:
    std::function<void(ServerInfo)> m_onAdd;

    TextField m_serverNameField;
    TextField m_ipField;
    TextField m_passwordField;

    ftxui::Component m_confirmButton;
    ftxui::Component m_cancelButton;

    ftxui::Component m_form;
    ftxui::Component m_component;
};

}

#endif
