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

// FTXUI Includes
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

// C++ Includes
#include <functional>
#include <string>


namespace Chess {

class ClientPanel;

class AddServerModal : public ModalInterface {
public:
    AddServerModal(ClientPanel& clientPanel, const std::function<void(ServerInfo)>& onAdd);
    ~AddServerModal() override;

    [[nodiscard]] ftxui::Component getComponent() override;

private:
    std::string m_serverName; static constexpr std::uint64_t MAX_SERVERNAME_LENGTH = 64;
    std::string m_ip; static constexpr std::uint64_t MAX_IP_LENGTH = 45;
    std::string m_password; static constexpr std::uint64_t MAX_PASSWORD_LENGTH = 64;
    ftxui::Component m_component;
};

}

#endif
