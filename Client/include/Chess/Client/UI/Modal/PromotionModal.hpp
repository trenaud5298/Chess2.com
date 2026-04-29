#ifndef CHESS_CLIENT_UI_MODAL_PROMOTIONMODAL_HPP
#define CHESS_CLIENT_UI_MODAL_PROMOTIONMODAL_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Modal/ModalInterface.hpp>
#include <Chess/Core/UI/Form/TextField.hpp>
#include <Chess/Core/UI/ConditionalButton.hpp>
#include <Chess/Core/Networking/MessagePayloads.hpp>

// FTXUI Includes
#include <ftxui/component/component.hpp>


namespace Chess {

class ClientPanel;

class PromotionModal : public ModalInterface {
public:
    PromotionModal(ClientPanel& clientPanel, bool isWhite, std::function<void(PromotionPiece)> onChoice);
    ~PromotionModal() override;

    [[nodiscard]] ftxui::Component getComponent() override;

private:
    bool m_isWhite{true};
    std::function<void(PromotionPiece)> m_onChoice;
    ftxui::Component m_component;
};

}

#endif
