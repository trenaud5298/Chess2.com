#ifndef CHESS_CLIENT_UI_MODAL_ERRORMODAL_HPP
#define CHESS_CLIENT_UI_MODAL_ERRORMODAL_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Modal/ModalInterface.hpp>

// FTXUI Includes
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

// C++ Includes
#include <functional>
#include <string>


namespace Chess {

class ClientPanel;

class ErrorModal : public ModalInterface {
public:
    ErrorModal(ClientPanel& clientPanel, std::string message);
    ~ErrorModal() override;

    [[nodiscard]] ftxui::Component getComponent() override;

private:
    std::string m_message;
    std::function<void()> m_onConfirm;
    std::function<void()> m_onCancel;

    ftxui::Component m_component;
};

}

#endif
