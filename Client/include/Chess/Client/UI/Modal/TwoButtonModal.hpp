#ifndef CHESS_CLIENT_UI_MODAL_TWOBUTTONMODALMODAL_HPP
#define CHESS_CLIENT_UI_MODAL_TWOBUTTONMODALMODAL_HPP

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

class TwoButtonModal : public ModalInterface {
public:
    TwoButtonModal(ClientPanel& clientPanel, std::string title, std::string message, std::string buttonText1, std::function<void()> onButton1, std::string buttonText2, std::function<void()> onButton2);
    ~TwoButtonModal() override;

    [[nodiscard]] ftxui::Component getComponent() override;

private:
    ftxui::Component m_component;
};

}

#endif
