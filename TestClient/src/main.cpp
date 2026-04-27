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

    std::vector<IdPos> state = { 
                                IdPos({ID::W_KING, Pos{3,3}}),
                                IdPos({ID::B_KING, Pos{5,4}}),
                                //IdPos({ID::B_BISHOP1, Pos{7,7}}),
                                IdPos({ID::B_PAWN1, Pos{4,4}}),
                                IdPos({ID::W_PAWN1, Pos{4,5}}),
                                };
    */
    std::vector<IdPos> state = { 
                                IdPos({ID::W_KING, Pos{1,1}}),
                                IdPos({ID::W_BISHOP1, Pos{2,2}}),
                                IdPos({ID::B_PAWN1, Pos{6,7}}),
                                IdPos({ID::B_BISHOP1, Pos{7,7}}),
                                IdPos({ID::W_BISHOP2, Pos{2,1}}),
                                IdPos({ID::B_ROOK1, Pos{7,1}}),
                                IdPos({ID::W_ROOK1, Pos{0,2}}),
                                IdPos({ID::B_QUEEN, Pos{1,4}}),
                                };

    std::array<ID, 64> boardLiteral = Chess::Board::genBoardLiteral(state);
    std::array<Piece, 32> piecesLiteral = Chess::Board::genPieces(state);
    board = Chess::Board(boardLiteral, piecesLiteral);
    
    board.genPinned();
    board.printPinnedArr();
    //board.printPiecesPos(piecesLiteral);
    //Chess::Board::printBoard(boardLiteral);

    board.displayBoard();
    board.printPinnedSet();
    board.genChecked();
    board.printCheckedPiece();
    board.genMoves();
    //std::cout << "HERE" << std::endl;
    board.printMoves();


    return 0;
}
