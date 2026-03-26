#include <Chess/Core/Game/Board.hpp>
#include <iostream>


int main() {
    Chess::Board board;
    //board.printMoveOffset();
    board.genMoves();
    return 0;
}
