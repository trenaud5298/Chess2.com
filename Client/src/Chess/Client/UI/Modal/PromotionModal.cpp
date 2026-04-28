/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Modal/PromotionModal.hpp>
#include <Chess/Client/UI/ClientPanel.hpp>


// ASIO Includes

// FTXUI Includes

// C++ Includes
#include <utility>
#include <cctype>

namespace Chess {

PromotionModal::PromotionModal(ClientPanel& clientPanel, std::uint8_t from, std::uint8_t to, bool isWhite)
: ModalInterface(clientPanel), m_from(from), m_to(to), m_isWhite(isWhite) {
    auto chessPieceButtonOption = ftxui::ButtonOption::Animated();
    auto originalTransform = chessPieceButtonOption.transform;
    chessPieceButtonOption.transform = [this, originalTransform](const ftxui::EntryState& state) {
        return originalTransform(state) | ftxui::center | ftxui::color(m_isWhite ? ftxui::Color::White : ftxui::Color::Black);
    };

    auto cancelPieceButtonOption = ftxui::ButtonOption::Animated();
    auto originalTransform2 = cancelPieceButtonOption.transform;
    cancelPieceButtonOption.transform = [this, originalTransform2](const ftxui::EntryState& state) {
        return originalTransform2(state) | ftxui::center;
    };

    auto rookButton = ftxui::Button("♜", [this]() {
        m_promotionPiece = PromotionPiece::Rook;
        submitMove();
        requestDismiss();
    }, chessPieceButtonOption);

    auto bishopButton = ftxui::Button("♝", [this]() {
        m_promotionPiece = PromotionPiece::Bishop;
        submitMove();
        requestDismiss();
    }, chessPieceButtonOption);

    auto knightButton = ftxui::Button("♞", [this]() {
        m_promotionPiece = PromotionPiece::Knight;
        submitMove();
        requestDismiss();
    }, chessPieceButtonOption);

    auto queenButton = ftxui::Button("♛", [this]() {
        m_promotionPiece = PromotionPiece::Queen;
        submitMove();
        requestDismiss();
    }, chessPieceButtonOption);

    auto cancelButton = ftxui::Button("Cancel", [this]() {
        requestDismiss();
    }, ftxui::ButtonOption::Animated());

    auto layout = ftxui::Container::Vertical({
        ftxui::Container::Horizontal({
            rookButton,
            bishopButton,
            knightButton,
            queenButton,
        }),
        cancelButton
    });

    m_component = ftxui::Renderer(layout, [rookButton, bishopButton, knightButton, queenButton, cancelButton]() {
        return ftxui::vbox({
            ftxui::text("Pawn Promotion") | ftxui::bold | ftxui::center,
            ftxui::separator(),
            ftxui::hbox({
                rookButton->Render() | ftxui::size(ftxui::WIDTH, ftxui::GREATER_THAN, 3) | ftxui::size(ftxui::HEIGHT, ftxui::GREATER_THAN, 3),
                ftxui::text(" "),
                bishopButton->Render() | ftxui::size(ftxui::WIDTH, ftxui::GREATER_THAN, 3) | ftxui::size(ftxui::HEIGHT, ftxui::GREATER_THAN, 3),
                ftxui::text(" "),
                knightButton->Render() | ftxui::size(ftxui::WIDTH, ftxui::GREATER_THAN, 3) | ftxui::size(ftxui::HEIGHT, ftxui::GREATER_THAN, 3),
                ftxui::text(" "),
                queenButton->Render() | ftxui::size(ftxui::WIDTH, ftxui::GREATER_THAN, 3) | ftxui::size(ftxui::HEIGHT, ftxui::GREATER_THAN, 3)
            }) | ftxui::center,
            cancelButton->Render() | ftxui::center,
        }) | ftxui::center | ftxui::size(ftxui::WIDTH, ftxui::GREATER_THAN, 18) | ftxui::size(ftxui::HEIGHT, ftxui::GREATER_THAN, 14);
    });
}


PromotionModal::~PromotionModal() {

}

ftxui::Component PromotionModal::getComponent() {
    return m_component;
}

void PromotionModal::submitMove() {
    ClientStatus status = m_clientPanel.gameClient().submitMultiplayerMove(
        m_from, m_to, m_promotionPiece
    );
    m_clientPanel.handleStatus(
        status,
        "Failed To Make Move", ResultPolicy::Modal
    );
}




}
