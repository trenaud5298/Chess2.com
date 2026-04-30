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

    // checked
    std::vector<IdPos> state = { 
                                IdPos({ID::W_KING, Pos{3,3}}),
                                IdPos({ID::B_KING, Pos{5,4}}),
                                //IdPos({ID::B_BISHOP1, Pos{7,7}}),
                                IdPos({ID::B_PAWN1, Pos{4,4}}),
                                IdPos({ID::W_PAWN1, Pos{4,5}}),
                                };
    std::vector<IdPos> state = { 
                                IdPos({ID::W_KING, Pos{3,3}}),
                                IdPos({ID::B_QUEEN, Pos{0,5}}),
                                IdPos({ID::B_ROOK1, Pos{4,2}}),
                                IdPos({ID::B_ROOK2, Pos{2,2}}),
                                };

    std::vector<IdPos> state = { 
                                IdPos({ID::W_KING, Pos{3,3}}),
                                IdPos({ID::B_QUEEN, Pos{0,5}}),
                                IdPos({ID::B_ROOK1, Pos{4,2}}),
                                IdPos({ID::B_ROOK2, Pos{2,2}}),
                                IdPos({ID::B_PAWN1, Pos{1,3}}),
                                IdPos({ID::B_KING, Pos{1,4}}),
                                IdPos({ID::W_PAWN1, Pos{6,3}}),
                                };


*/
    // pinned and checked
    std::vector<IdPos> state = { 
                                IdPos({ID::W_KING, Pos{1,1}}),
                                //IdPos({ID::W_BISHOP1, Pos{2,2}}),
                                IdPos({ID::B_PAWN1, Pos{6,7}}),
                                IdPos({ID::B_BISHOP1, Pos{7,7}}),
                                IdPos({ID::W_BISHOP2, Pos{2,1}}),
                                IdPos({ID::B_ROOK1, Pos{7,0}}),
                                IdPos({ID::W_BISHOP1, Pos{2,2}}),
                                IdPos({ID::B_QUEEN, Pos{0,7}}),
                                IdPos({ID::W_QUEEN, Pos{0,5}}),
                                IdPos({ID::W_ROOK1, Pos{2,6}}),
                                };
    std::array<ID, 64> boardLiteral = Chess::Board::genBoardLiteral(state);
    std::array<Piece, 32> piecesLiteral = Chess::Board::genPieces(state);
    board = Chess::Board(boardLiteral, piecesLiteral);
    const bool BLACK_TURN = false;
    board.setTurn(BLACK_TURN);
    
    board.printTurn();
    board.displayBoards();
    while( board.isValidState() ) {
        board.testTurn();
        board.nextTurn();
    }

    /*
    board.genMoves();
    board.printTurn();
    board.displayBoard();
    board.displayAttacked();
    board.displayDefended();
    board.filterPinned();
    if( board.isInCheck() ) {
        board.filterChecked();
    }
    if( board.isValidMove(ID::W_PAWN1, Pos{7,3}) ) {
        board.move(ID::W_PAWN1, Pos{7,3});
    } else {std::cout << "INVALID MOVE" << std::endl;}
    board.filterKingMoves();
    std::cout << "isPawnPromotable: " << (board.isPawnPromotable() ? "true\n" : "false\n");
    if( board.isPawnPromotable() ) {
        board.promotePawn();
    }
    board.setGameState();
    board.printLog();
    board.nextTurn();

    board.printState();
    board.genMoves();
    board.printTurn();
    board.displayBoard();
    board.displayAttacked();
    board.displayDefended();
    board.printMoves();
    board.printMovesIdxs();
    board.printPinnedArr();
    board.filterPinned();
    if( board.isInCheck() ) {
        board.filterChecked();
    }
    board.setGameState();
    board.filterKingMoves();
    board.printMovesIdxs();
    board.printCheckedPiece();
    board.printPinnedSet();
    //std::cout << "HERE" << std::endl;
    board.printMoves();
    board.printCheckedArr();

    std::cout << "isStalemate: " << (board.isStalemate() ? "true" : "false");
    std::cout << "\nisCheckmate: " << (board.isCheckmate() ? "true" : "false");
    std::cout << '\n' << std::endl;
    */


    return 0;
}
