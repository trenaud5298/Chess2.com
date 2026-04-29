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

namespace {

std::string pieceGlyph(PromotionPiece piece, bool isWhite) {
    switch (piece) {
        case PromotionPiece::Rook: return isWhite ? "♖" : "♜";
        case PromotionPiece::Bishop: return isWhite ? "♗" : "♝";
        case PromotionPiece::Knight: return isWhite ? "♘" : "♞";
        case PromotionPiece::Queen: return isWhite ? "♕" : "♛";
        case PromotionPiece::None: return "";
    }
    return "";
}

std::string pieceLabel(PromotionPiece piece) {
    switch (piece) {
        case PromotionPiece::Rook: return "Rook";
        case PromotionPiece::Bishop: return "Bishop";
        case PromotionPiece::Knight: return "Knight";
        case PromotionPiece::Queen: return "Queen";
        case PromotionPiece::None: return "Unknown";
    }
    return "Unknown";
}

ftxui::ButtonOption makePieceButtonOption(PromotionPiece piece, bool isWhite) {
    ftxui::ButtonOption option = ftxui::ButtonOption::Animated();
    auto originalTransform = option.transform;
    option.transform = [piece, isWhite, originalTransform](const ftxui::EntryState& state) {
        ftxui::Element glyphElement = ftxui::text(pieceGlyph(piece, isWhite)) | ftxui::center | ftxui::bold;
        ftxui::Element labelElement = ftxui::text(pieceLabel(piece)) | ftxui::center | ftxui::dim;

        if (isWhite) {
            glyphElement |= ftxui::color(ftxui::Color::White);
        } else {
            glyphElement |= ftxui::color(ftxui::Color::Black);
        }

        ftxui::Element element = ftxui::vbox({
            ftxui::filler(),
            glyphElement,
            labelElement,
            ftxui::filler()
        }) | ftxui::borderRounded | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 12) | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 7);

        if (state.focused) {
            element |= ftxui::inverted;
        }

        return element;
    };
    return option;
}

}


PromotionModal::PromotionModal(ClientPanel& clientPanel, bool isWhite, std::function<void(PromotionPiece)> onChoice)
: ModalInterface(clientPanel), m_onChoice(std::move(onChoice)), m_isWhite(isWhite) {
    auto cancelPieceButtonOption = ftxui::ButtonOption::Animated();
    auto originalTransform = cancelPieceButtonOption.transform;
    cancelPieceButtonOption.transform = [this, originalTransform](const ftxui::EntryState& state) {
        return originalTransform(state) | ftxui::center;
    };


    auto makePieceButton = [this](PromotionPiece piece) {
        return ftxui::Button("", [this, piece]() {
            if (m_onChoice) {
                m_onChoice(piece);
            }
            requestDismiss();
        }, makePieceButtonOption(piece, m_isWhite));
    };


    auto rookButton = makePieceButton(PromotionPiece::Rook);
    auto bishopButton = makePieceButton(PromotionPiece::Bishop);
    auto knightButton = makePieceButton(PromotionPiece::Knight);
    auto queenButton = makePieceButton(PromotionPiece::Queen);

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
            ftxui::text("Choose a Piece") | ftxui::dim | ftxui::center,
            ftxui::separator(),

            ftxui::hbox({
                rookButton->Render(),
                ftxui::text(" "),
                bishopButton->Render(),
                ftxui::text(" "),
                knightButton->Render(),
                ftxui::text(" "),
                queenButton->Render()
            }) | ftxui::center,

            ftxui::separatorEmpty(),

            cancelButton->Render() | ftxui::center,

        }) | ftxui::center | ftxui::size(ftxui::WIDTH, ftxui::GREATER_THAN, 62) | ftxui::size(ftxui::HEIGHT, ftxui::GREATER_THAN, 16);
    });
}


PromotionModal::~PromotionModal() {

}

ftxui::Component PromotionModal::getComponent() {
    return m_component;
}




}
