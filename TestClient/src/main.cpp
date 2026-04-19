#include <Chess/Core/Game/Board.hpp>
#include <iostream>


int main() {
    Chess::Board board;
    /*
    board.genMoves();
    board.move(ID::W_PAWN1, Pos{3,0});
    board.move(ID::W_ROOK1, Pos{2,0});
    board.move(ID::W_ROOK1, Pos{2,1});
    board.move(ID::W_KNIGHT1, Pos{2,2});
    board.move(ID::B_PAWN2, Pos{4,1});
    board.move(ID::B_PAWN2, Pos{3,1});
    board.move(ID::W_PAWN4, Pos{2,3});
    board.move(ID::W_BISHOP1, Pos{5,7});
    board.move(ID::B_PAWN5, Pos{4,4});
    board.move(ID::B_BISHOP2, Pos{6,4});
    board.move(ID::W_KING, Pos{1,3});
    board.displayBoard();
    board.nextTurn();
    board.genMoves();
    board.printMoves();
    */

    std::vector<IdPos> state = { IdPos({ID::W_KING, Pos{0,0}}) };
    //std::array<ID, 64> boardLiteral = Chess::Board::genBoardLiteral(state);
    std::array<Piece, 32> piecesLiteral = Chess::Board::genPieces(state);
    std::cout << "HERE" << std::endl;
    /*
    board = Chess::Board(boardLiteral, piecesLiteral);
    board.displayBoard();
    board.genPinned();
    */

    return 0;
}
