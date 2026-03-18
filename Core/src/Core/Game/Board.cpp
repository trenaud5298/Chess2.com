#include <Chess/Core/Game/Board.hpp>
#include <Chess/Core/declarations.hpp>

#include <iostream>
#include <vector>

// TODO 
//      * implement "piece collision" in addMoves methods
//      * implement a way to castle
//      * implement an isChecked method and getChecking method
//      * implement a way to promote pawns
//      * implement a way to en passant
//      * implement checking logic
//
//      * start a game class

namespace Chess {
    using enum ID;
    Board::Board() : 
        m_board({
            // m_board[0] corresponds to a1, m_board[1] -> b1,..., m_board[63] -> f8
            W_ROOK1, W_KNIGHT1, W_BISHOP1, W_QUEEN, W_KING, W_BISHOP2, W_KNIGHT2, W_ROOK2,
            W_PAWN1, W_PAWN2, W_PAWN3, W_PAWN4, W_PAWN5, W_PAWN6, W_PAWN7, W_PAWN8, 
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, 
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, 
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, 
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, 
            B_PAWN1, B_PAWN2, B_PAWN3, B_PAWN4, B_PAWN5, B_PAWN6, B_PAWN7, B_PAWN8, 
            B_ROOK1, B_KNIGHT1, B_BISHOP1, B_QUEEN, B_KING, B_BISHOP2, B_KNIGHT2, B_ROOK2
        }),
        m_pieceArr({
                {Pos({0,0}), Type::W_ROOK, MAX_MOVES, 0},
                {Pos({0,7}), Type::W_ROOK, MAX_MOVES, 0},
                {Pos({0,1}), Type::W_KNIGHT, MAX_MOVES, 0},
                {Pos({0,6}), Type::W_KNIGHT, MAX_MOVES, 0},
                {Pos({0,2}), Type::W_BISHOP, MAX_MOVES, 0},
                {Pos({0,5}), Type::W_BISHOP, MAX_MOVES, 0},
                {Pos({0,3}), Type::W_QUEEN, MAX_MOVES, 0},
                {Pos({0,4}), Type::W_KING, MAX_MOVES, 0},

                {Pos({1,0}), Type::W_PAWN, MAX_MOVES, 0},
                {Pos({1,1}), Type::W_PAWN, MAX_MOVES, 0},
                {Pos({1,2}), Type::W_PAWN, MAX_MOVES, 0},
                {Pos({1,3}), Type::W_PAWN, MAX_MOVES, 0},
                {Pos({1,4}), Type::W_PAWN, MAX_MOVES, 0},
                {Pos({1,5}), Type::W_PAWN, MAX_MOVES, 0},
                {Pos({1,6}), Type::W_PAWN, MAX_MOVES, 0},
                {Pos({1,7}), Type::W_PAWN, MAX_MOVES, 0},

                {Pos({7,0}), Type::B_ROOK, MAX_MOVES, 0},
                {Pos({7,7}), Type::B_ROOK, MAX_MOVES, 0},
                {Pos({7,1}), Type::B_KNIGHT, MAX_MOVES, 0},
                {Pos({7,6}), Type::B_KNIGHT, MAX_MOVES, 0},
                {Pos({7,2}), Type::B_BISHOP, MAX_MOVES, 0},
                {Pos({7,5}), Type::B_BISHOP, MAX_MOVES, 0},
                {Pos({7,3}), Type::B_QUEEN, MAX_MOVES, 0},
                {Pos({7,4}), Type::B_KING, MAX_MOVES, 0},

                {Pos({6,0}), Type::B_PAWN, MAX_MOVES, 0},
                {Pos({6,1}), Type::B_PAWN, MAX_MOVES, 0},
                {Pos({6,2}), Type::B_PAWN, MAX_MOVES, 0},
                {Pos({6,3}), Type::B_PAWN, MAX_MOVES, 0},
                {Pos({6,4}), Type::B_PAWN, MAX_MOVES, 0},
                {Pos({6,5}), Type::B_PAWN, MAX_MOVES, 0},
                {Pos({6,6}), Type::B_PAWN, MAX_MOVES, 0},
                {Pos({6,7}), Type::B_PAWN, MAX_MOVES, 0},
        }),
        m_moves({8,8}),
        m_moveOffset({0})
        {
            m_moveOffset[0] = m_pieceArr[0].reserved;
            for(int i = 1; i < m_pieceArr.size(); i++) {
                m_moveOffset[i] = m_moveOffset[i-1] + m_pieceArr[i-1].reserved;
            }
        }

    Board::Board(const std::array<ID, 64>& board, const std::array<Piece, 32>& pieces) : m_board(board), m_pieceArr(pieces) {}

    Board::~Board() {}

    Board& Board::operator=(const Board& other) {
        if (this == &other) {
            return *this;
        }
        m_board = other.m_board;
        m_pieceArr = other.m_pieceArr;
        return *this;
    }

    Pos& Board::getPos(const ID& id) {
        return m_pieceArr[((int) id)].position;
    }

    void Board::setIdAt(const Pos& pos, const ID& id){
         m_board[posTranslate(pos)] = id;
    }

    void Board::move(const ID& id, const Pos& pos) {
        int castedId = (int) id;
        Pos old = pos;
        m_pieceArr[castedId].position = pos;
        setIdAt(pos, id);
        setIdAt(old, ID::EMPTY);
    }

    bool Board::isInBoard(const Pos& pos) {
        int castedRow = (int) pos[ROW];
        int castedCol = (int) pos[COL];
        return ROW_LOWER_BOUND < castedRow && castedRow < ROW_UPPER_BOUND && COL_LOWER_BOUND < castedCol && castedCol < COL_UPPER_BOUND;
    }

    int Board::posTranslate(const Pos& pos) {
        return 8 * pos[ROW] + pos[COL];
    }

    ID& Board::getIdAt(const Pos& pos) {
        return m_board[posTranslate(pos)];
    }

    COLOR Board::getColor(const ID& id) {
        int castedId = (int) id;
        COLOR color;
        if( id == EMPTY ) {
            color = COLOR::EMPTY;
        } else if( castedId < BLACK_BOUND ) {
            color = COLOR::WHITE;
        } else {
            color = COLOR::BLACK;
        }
        return color;
    }

    ID Board::getKingId() {
        if( isWhiteTurn ) {
            return W_KING;
        } else {
            return B_KING;
        }
    }

    std::vector<bool>& Board::getAttackedVec() {
        if( isWhiteTurn ) {
            return m_attackedBlack;
        } else {
            return m_attackedWhite;
        }
    }

    void Board::diagonalHelper(const Pos& initial, const Direction& direction, int& i) {
        using enum Direction;
        Pos pos = initial;
        COLOR color = getColor(getIdAt(initial));
        ID& posId = getIdAt(pos);
        int card, x, y;
        COLOR posColor;
        switch (direction) {
            case(NORTHEAST):
                card = 0b10;
                break;
            case(SOUTHWEST):
                card = 0b01;
                break;
            case(NORTHWEST):
                card = 0b11;
                break;
            case(SOUTHEAST):
                card = 0b00;
                break;
            default:
                std::cout << "default in diagonalDirHelper switch" << std::endl;
                break;
        }
            x = y = 1;
            // check first bit
            if( (card>>1) == 1 ) {
                x = -x;
            }
            // check second bit
            if( ((card<<1)>>1) == 1 ) {
                y = -y;
            }
            pos[ROW] += x;
            pos[COL] += y;
            posColor = getColor(posId);
            while(isInBoard(pos) && color != posColor) {
                moves.push_back(pos);
                if(posColor != COLOR::EMPTY) {
                    return;
                }
                pos[ROW] += x;
                pos[COL] += y;
                posId = getIdAt(pos);
                posColor = getColor(posId);
            } 
    }

    void Board::addDiagonalMoves(const Pos& initial) {
        using enum Direction;
        int i = 0;
        diagonalHelper(initial, NORTHEAST, i);
        diagonalHelper(initial, SOUTHWEST, i);
        diagonalHelper(initial, NORTHWEST, i);
        diagonalHelper(initial, SOUTHEAST, i);
    }

    void Board::cardinalHelper(const Pos& initial, const Direction direction, int& i) {
        using enum Direction;
        Pos pos = initial;
        int card, dim, inc;
        COLOR color = getColor(getIdAt(initial));
        ID& posId = getIdAt(pos);
        COLOR posColor;
        switch (direction) {
            case (NORTH):
                card = 0b01;
                break;
            case (SOUTH):
                card = 0b00;
                break;
            case (EAST):
                card = 0b10;
                break;
            case (WEST):
                card = 0b11;
                break;
            default:
                std::cout << "default in cardinalHelper switch" << std::endl;
                break;
        };
        dim = inc = COL; // NOTE: COL = 1
        if( !((card>>1) == 1) ) {
            dim = ROW;
        }
        if( ((card<<1)>>1) == 1 ) {
            inc = -1;
        }
        pos[dim] += inc;
        posColor = getColor(posId);
        while(isInBoard(pos) && color != posColor) {
            moves.push_back(pos);
            if(posColor != COLOR::EMPTY) {
                return;
            }
            pos[dim] += inc;
            posId = getIdAt(pos);
            posColor = getColor(posId);
        }
    }

    void Board::addCardinalMoves(std::vector<Pos>& moves, const Pos& initial) {
        using enum Direction;
        cardinalHelper(moves, initial, NORTH);
        cardinalHelper(moves, initial, SOUTH);
        cardinalHelper(moves, initial, EAST);
        cardinalHelper(moves, initial, WEST);
    }

    void Board::addKnightMoves(std::vector<Pos>& moves, const Pos& initial) {
        int x, y;
        COLOR color = getColor(getIdAt(initial));
        Pos temp;
        COLOR tempColor;
        for(int i = 0b00; i < 4; i++) {
            x = 2, y = 1;
            if( (i>>1) == 1 ) {
                x = -x;
            }
            if( ((i<<1)>>1) == 1 ) {
                y = -y;
            }
            temp = Pos({static_cast<std::uint8_t>(initial[ROW] + x), static_cast<std::uint8_t>(initial[COL] + y)});
            tempColor = getColor(getIdAt(temp));
            if( isInBoard(temp) && color != tempColor ) {
                moves.push_back(temp);
            }
            temp = Pos({static_cast<std::uint8_t>(initial[ROW] + y), static_cast<std::uint8_t>(initial[COL] + x)});
            tempColor = getColor(getIdAt(temp));
            if( isInBoard(temp) ){
                moves.push_back(temp);
            }
        }
    }

    // TODO FIX
    void Board::addKingMoves(std::vector<Pos>& moves, const Pos& initial) {
        moves.push_back(Pos({static_cast<std::uint8_t>((int) initial[0] + 1), initial[1]}));
        moves.push_back(Pos({static_cast<std::uint8_t>((int) initial[0] - 1), initial[1]}));
        moves.push_back(Pos({initial[0], static_cast<std::uint8_t>((int) initial[1] + 1)}));
        moves.push_back(Pos({initial[0], static_cast<std::uint8_t>((int) initial[1] - 1)}));
        moves.push_back(Pos({ static_cast<std::uint8_t>((int) initial[0] + 1), static_cast<std::uint8_t>((int) initial[1] + 1)}));
        moves.push_back(Pos({ static_cast<std::uint8_t>((int) initial[0] - 1), static_cast<std::uint8_t>((int) initial[1] + 1)}));
        moves.push_back(Pos({ static_cast<std::uint8_t>((int) initial[0] + 1), static_cast<std::uint8_t>((int) initial[1] - 1)}));
        moves.push_back(Pos({ static_cast<std::uint8_t>((int) initial[0] - 1), static_cast<std::uint8_t>((int) initial[1] - 1)}));
    }

    void Board::addPawnMoves(std::vector<Pos>& moves, const Pos& initial, const ID& id) {
        bool isWhite = (int) id > 0 && (int) id < BLACK_BOUND;
        // TODO
    }

    std::vector<Pos> Board::getPossibleMoves(const ID& id) {
        using enum Type;
        std::vector<Pos> temp;
        const Type type = m_pieceArr[(int) id].type;
        const Pos& initial = m_pieceArr[(int) id].position;

        switch(type) {
            case W_BISHOP:
            case B_BISHOP:
                temp.reserve(MAX_MOVES_BISHOP);
                addDiagonalMoves(temp, initial);
                break;
            case W_ROOK:
            case B_ROOK:
                temp.reserve(MAX_MOVES_ROOK);
                addCardinalMoves(temp, initial);
                break;
            case W_KNIGHT:
            case B_KNIGHT:
                temp.reserve(MAX_MOVES_KNIGHT);
                addKnightMoves(temp, initial);
                break;
            case Type::W_QUEEN:
            case Type::B_QUEEN:
                addDiagonalMoves(temp, initial);
                addCardinalMoves(temp, initial);
                temp.reserve(MAX_MOVES_QUEEN);
                break;
            case Type::W_KING:
            case Type::B_KING:
                temp.reserve(MAX_MOVES_KING);
                addKingMoves(temp, initial);
                break;
            case W_PAWN:
            case B_PAWN:
                temp.reserve(MAX_MOVES_PAWN);
                addPawnMoves(temp, initial, id);
                break;
        }
        return std::move(temp);
    }
}
