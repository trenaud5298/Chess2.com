#include <Chess/Core/Game/Board.hpp>
#include <iostream>


int main() {
    Chess::Board board;
    std::cout<<board.isValidMove(ID::B_KING, Pos{0,0})<<std::endl;
    return 0;
}