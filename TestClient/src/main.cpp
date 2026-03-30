#include <Chess/Core/Game/Board.hpp>
#include <iostream>


int main() {
    Chess::Board board;
    //board.printMoveOffset();
    board.genMoves();
    board.printMoves();
    board.move(ID::W_PAWN1, Pos{3,0});
    board.move(ID::W_ROOK1, Pos{2,0});
    board.move(ID::W_ROOK1, Pos{2,1});
    board.move(ID::W_KNIGHT1, Pos{2,2});
    board.displayBoard();
    board.genMoves();
    board.printMoves();

    return 0;
}
