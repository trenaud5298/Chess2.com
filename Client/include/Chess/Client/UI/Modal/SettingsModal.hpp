#ifndef CHESS_CLIENT_UI_MODAL_SETTINGSMODAL_HPP
#define CHESS_CLIENT_UI_MODAL_SETTINGSMODAL_HPP

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


// C++ Includes


namespace Chess {

class ClientPanel;

class SettingsModal : public ModalInterface {
public:
    explicit SettingsModal(ClientPanel& panel);
    virtual ~SettingsModal();

    void onEnter() override;
    void onLeave() override;

    [[nodiscard]] ftxui::Component getComponent() override;
private:
    ftxui::Component m_component;
};

}


#endif