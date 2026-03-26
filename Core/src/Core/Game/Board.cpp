#include <Chess/Core/Game/Board.hpp>
#include <Chess/Core/declarations.hpp>

#include <iostream>
#include <vector>
#include <set>
#include <string>

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
                {Pos({0,0}), Type::W_ROOK, MAX_MOVES_ROOK, 0},
                {Pos({0,7}), Type::W_ROOK, MAX_MOVES_ROOK, 0},
                {Pos({0,1}), Type::W_KNIGHT, MAX_MOVES_KNIGHT, 0},
                {Pos({0,6}), Type::W_KNIGHT, MAX_MOVES_KNIGHT, 0},
                {Pos({0,2}), Type::W_BISHOP, MAX_MOVES_BISHOP, 0},
                {Pos({0,5}), Type::W_BISHOP, MAX_MOVES_BISHOP, 0},
                {Pos({0,3}), Type::W_QUEEN, MAX_MOVES_QUEEN, 0},
                {Pos({0,4}), Type::W_KING, MAX_MOVES_KING, 0},

                {Pos({1,0}), Type::W_PAWN, MAX_MOVES_PAWN, 0},
                {Pos({1,1}), Type::W_PAWN, MAX_MOVES_PAWN, 0},
                {Pos({1,2}), Type::W_PAWN, MAX_MOVES_PAWN, 0},
                {Pos({1,3}), Type::W_PAWN, MAX_MOVES_PAWN, 0},
                {Pos({1,4}), Type::W_PAWN, MAX_MOVES_PAWN, 0},
                {Pos({1,5}), Type::W_PAWN, MAX_MOVES_PAWN, 0},
                {Pos({1,6}), Type::W_PAWN, MAX_MOVES_PAWN, 0},
                {Pos({1,7}), Type::W_PAWN, MAX_MOVES_PAWN, 0},

                {Pos({7,0}), Type::B_ROOK, MAX_MOVES_ROOK, 0},
                {Pos({7,7}), Type::B_ROOK, MAX_MOVES_ROOK, 0},
                {Pos({7,1}), Type::B_KNIGHT, MAX_MOVES_KNIGHT, 0},
                {Pos({7,6}), Type::B_KNIGHT, MAX_MOVES_KNIGHT, 0},
                {Pos({7,2}), Type::B_BISHOP, MAX_MOVES_BISHOP, 0},
                {Pos({7,5}), Type::B_BISHOP, MAX_MOVES_BISHOP, 0},
                {Pos({7,3}), Type::B_QUEEN, MAX_MOVES_QUEEN, 0},
                {Pos({7,4}), Type::B_KING, MAX_MOVES_KING, 0},

                {Pos({6,0}), Type::B_PAWN, MAX_MOVES_PAWN, 0},
                {Pos({6,1}), Type::B_PAWN, MAX_MOVES_PAWN, 0},
                {Pos({6,2}), Type::B_PAWN, MAX_MOVES_PAWN, 0},
                {Pos({6,3}), Type::B_PAWN, MAX_MOVES_PAWN, 0},
                {Pos({6,4}), Type::B_PAWN, MAX_MOVES_PAWN, 0},
                {Pos({6,5}), Type::B_PAWN, MAX_MOVES_PAWN, 0},
                {Pos({6,6}), Type::B_PAWN, MAX_MOVES_PAWN, 0},
                {Pos({6,7}), Type::B_PAWN, MAX_MOVES_PAWN, 0},
        }),
        m_moves({8,8}),
        m_moveOffset({0}),
        m_defended(64, false),
        m_attackedWhite(64, false),
        m_attackedBlack(64, false),
        m_pinnedVec(),
        m_pinnedIdSet(),
        m_diagonalSet({Type::W_BISHOP, Type::B_BISHOP, Type::W_QUEEN, Type::B_QUEEN}),
        m_cardinalSet({Type::W_ROOK, Type::B_ROOK, Type::W_QUEEN, Type::B_QUEEN}),
        m_mods({
                std::pair(1,0),
                std::pair(0,1),
                std::pair(-1,0),
                std::pair(0,-1),
                std::pair(1,1),
                std::pair(1,-1),
                std::pair(-1,1),
                std::pair(-1,-1),
                })
        {
            m_moveOffset[0] = 0;
            for(int i = 1; i < m_pieceArr.size(); i++) {
                m_moveOffset[i] = m_moveOffset[i-1] + m_pieceArr[i-1].reserved;
            }
            m_pinnedVec.reserve(MAX_PINNED);
            m_isWhiteTurn = true;
            m_isWhiteChecked = false;
            m_isBlackChecked = false;
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
        // I added a -1 here since ID starts at 1 instead
        // of 0 creating a small issue of being off by one piece
        // in the array
        return m_pieceArr[((int)id)-1].position;
    }

    void Board::setIdAt(const Pos& pos, const ID& id){
         m_board[posTranslate(pos)] = id;
    }

    Type Board::getTypeAt(const Pos& pos) {
        return m_pieceArr[(int)getIdAt(pos)-1].type;
    }

    void Board::move(const ID& id, const Pos& pos) {
        // I was having issues with the move function and noticed an issue
        // or two with an indexing mismatch.
        // The IDs start at 1, but are used as an index
        // into an array and this results in an off by one error.
        // I have a temporary quick fix thing below for current demo.
        // I also had to change getPos() for the same reason and noted it there as well
        // int castedId = (int) id;
        // Pos old = pos;
        // m_pieceArr[castedId - 1].position = pos;
        // setIdAt(pos, id);
        // setIdAt(old, ID::EMPTY);

        int castedId = (int)id - 1;
        Pos old = m_pieceArr[castedId].position;
        m_pieceArr[castedId].position = pos;
        setIdAt(pos, id);
        setIdAt(old, ID::EMPTY);
    }

    bool Board::isValidMove(const ID& id, const Pos& target) {
        const int castedId = (int)id -1,
                  idx = m_pieceArr[castedId].movesIdx;
        if( idx == 0 ) 
            return false;

        const int offset = m_moveOffset[castedId];
        bool flag = false;
        Pos temp;
        
        for(int i = 0; i < idx; i++) {
            temp = m_moves[offset + i];
            if( temp[ROW] == target[ROW] && temp[COL] == target[COL]  )
                flag = true;
        }
        return flag;
    }

    void Board::setTurn(bool isWhite) {
        m_isWhiteTurn = isWhite;
    }

    bool Board::getTurn() const {
        return m_isWhiteTurn;
    }

    void Board::nextTurn() {
        m_isWhiteTurn = !m_isWhiteTurn;
    }


    const std::array<ID, 64> &Board::getBoard() {
        return m_board;
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
        if( m_isWhiteTurn ) {
            return W_KING;
        } else {
            return B_KING;
        }
    }

    std::uint8_t Board::castHelper(const std::uint8_t dim, const int& mod) {
        return static_cast<std::uint8_t>((int)dim + mod);
    }

    std::vector<bool>& Board::getAttackedVec() {
        if( m_isWhiteTurn ) {
            return m_attackedBlack;
        } else {
            return m_attackedWhite;
        }
    }

    void Board::setAttackedAt(const Pos& pos) {
        getAttackedVec()[posTranslate(pos)] = true;
    }

    void Board::setDefendedAt(const Pos& pos) {
        m_defended[posTranslate(pos)] = true;
    }

    void Board::setMovesIdx(const ID& id, const int& i) {
        const int castedId = (int) id-1;
        m_pieceArr[castedId].movesIdx = i;
    }

    void Board::setMoveAt(const ID& id, const Pos& pos, const int& i) {
        const int castedId = (int) id-1,
              idx = m_moveOffset[castedId] + i -1;
        m_moves[idx] = pos;
    }

    void Board::setChecked(const ID& id, const Direction direction) {
        m_checked.id = id;
        m_checked.direction = direction;
    }

    Direction Board::directionCast(const int& i) {
        return static_cast<Direction>(i);
    }

    int Board::getPawnRow() {
        if( m_isWhiteTurn ) {
            return WHITE_PAWN_ROW;
        } else {
            return BLACK_PAWN_ROW;
        }
    }

    std::pair<int,int> Board::getMod(const Direction& direction) {
        using enum Direction;
        std::pair<int,int> res;
        const int inc = 1, 
              dec = -1, 
              z = 0;
        switch(direction) {
            case(NORTH):
                res = std::pair(dec, z);
                break;
            case(SOUTH):
                res = std::pair(inc, z);
                break;
            case(EAST):
                res = std::pair(z, inc);
                break;
            case(WEST):
                res = std::pair(z, dec);
                break;
            case(NORTHEAST):
                res = std::pair(dec, inc);
                break;
            case(SOUTHEAST):
                res = std::pair(inc, inc);
                break;
            case(NORTHWEST):
                res = std::pair(dec, dec);
                break;
            case(SOUTHWEST):
                res = std::pair(inc, dec);
                break;
        }
        return res;
    }

    const std::set<Type>* Board::getMatchingSet(const Direction& direction) {
        using enum Direction;
        const std::set<Type>* res;
        if( direction == NORTH || direction == SOUTH || direction == EAST || direction == WEST ) {
            res = &m_cardinalSet;
        } else {
            res = &m_diagonalSet;
        }
        return res;
    }

    void Board::genChecked() {
        using enum Direction;
        const ID kingId = getKingId();
        const Pos kingPos = getPos(kingId);
        const COLOR kingColor = getColor(kingId);
        const std::set<Type>* set;
        Pos pos = kingPos;
        ID tempId;
        Direction direction;
        m_checked.direction = NORTH; //default direction

        //Knight squares
        int x, y;
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
            temp = Pos({castHelper(kingPos[ROW], x), castHelper(kingPos[COL], y)});
            tempColor = getColor(getIdAt(temp));
            tempId = getIdAt(temp);
            if( isInBoard(temp) ) {
                if( kingColor != tempColor && getTypeAt(temp) == Type::W_KNIGHT || getTypeAt(temp) == Type::B_KNIGHT) {
                    setChecked(tempId, m_checked.direction);
                    return;
                }
            }
            temp = Pos({castHelper(kingPos[ROW], y), castHelper(kingPos[COL], x)});
            tempColor = getColor(getIdAt(temp));
            tempId = getIdAt(temp);
            if( isInBoard(temp) ) {
                if( kingColor != tempColor && getTypeAt(temp) == Type::W_KNIGHT || getTypeAt(temp) == Type::B_KNIGHT) {
                    setChecked(tempId, m_checked.direction);
                    return;
                }
            }
        }
        //King squares
        for(const std::pair<int,int>& mod : m_mods) {
            temp = Pos({castHelper(kingPos[ROW], mod.first), castHelper(kingPos[COL], mod.second)});
            tempColor = getColor(getIdAt(temp));
            tempId = getIdAt(temp);
            if( isInBoard(temp) ) {
                if( kingColor != tempColor && getTypeAt(temp) == Type::W_KING || getTypeAt(temp) == Type::B_KING) {
                    setChecked(tempId, m_checked.direction);
                    return;
                }
            }
        }
        //Pawn squares
        if( kingColor == COLOR::WHITE ) {
            m_pawnMods[0] = m_mods[6];
            m_pawnMods[1] = m_mods[7];
        } else {
            m_pawnMods[0] = m_mods[4];
            m_pawnMods[1] = m_mods[5];
        }
        for(const std::pair<int,int>& mod : m_pawnMods) {
            temp = Pos({castHelper(kingPos[ROW], mod.first), castHelper(kingPos[COL], mod.second)});
            tempColor = getColor(getIdAt(temp));
            tempId = getIdAt(temp);
            if( isInBoard(temp) ) {
                if( kingColor != tempColor && getTypeAt(temp) == Type::W_PAWN || getTypeAt(temp) == Type::B_PAWN ) {
                    setChecked(tempId, m_checked.direction);
                    return;
                }
            }
            //Diagonal & Cardinal squares
            for(int i = 1; i < 5; i++) {
                direction = directionCast(i);
                std::pair<int,int> mod = getMod(direction);
                set = getMatchingSet(direction);
                temp = kingPos;
                temp[ROW] += mod.first;
                temp[COL] += mod.second;
                tempColor = getColor(tempId);
                tempId = getIdAt(temp);
                while( isInBoard(pos) ) {
                    if( kingColor != tempColor ) {
                        if( tempColor != COLOR::EMPTY && set->contains(getTypeAt(temp))) {
                            setChecked(tempId, directionCast(-i));
                            return;
                        }
                        temp[ROW] += mod.first;
                        temp[COL] += mod.second;
                        tempId = getIdAt(pos);
                        tempColor = getColor(tempId);
                    } else {
                        break;
                    }
                }
                direction = directionCast(-i);
                temp = kingPos;
                temp[ROW] += mod.first;
                temp[COL] += mod.second;
                tempColor = getColor(tempId);
                tempId = getIdAt(temp);
                while( isInBoard(pos) ) {
                    if( kingColor != tempColor ) {
                        if( tempColor != COLOR::EMPTY && set->contains(getTypeAt(temp))) {
                            setChecked(tempId, directionCast(i));
                            return;
                        }
                        temp[ROW] += mod.first;
                        temp[COL] += mod.second;
                        tempId = getIdAt(pos);
                        tempColor = getColor(tempId);
                    } else {
                        break;
                    }
                }
            }
        }
    }

    void Board::genPinned() {
        using enum Direction;
        const ID kingId = getKingId();
        const Pos kingPos = getPos(kingId);
        const COLOR kingColor = getColor(kingId);
        const Pos pos = kingPos;
        const std::set<Type>* set;
        ID tempId;
        Direction direction;
        int matchingOnLane; // counts the pieces matching king color on lane
        Pos temp;
        COLOR tempColor;

        for(int i = 1; i < 5; i++) {
            direction = directionCast(i);
            std::pair<int,int> mod = getMod(direction);
            set = getMatchingSet(direction); // only needs to be called once since -Direction is still the same axis
            temp = kingPos;
            temp[ROW] += mod.first;
            temp[COL] += mod.second;
            tempColor = getColor(tempId);
            tempId = getIdAt(temp);
            while( isInBoard(pos) && matchingOnLane < 2) {
                if( kingColor != tempColor ) {
                    if( tempColor != COLOR::EMPTY && matchingOnLane == 0 ) {
                        break;
                    } else if( tempColor != COLOR::EMPTY && matchingOnLane == 1 && set->contains(getTypeAt(temp))) {
                        m_pinnedIdSet.emplace(tempId);
                        m_pinnedVec.push_back({tempId, directionCast(-i)});
                        break;
                    }
                    temp[ROW] += mod.first;
                    temp[COL] += mod.second;
                    tempId = getIdAt(pos);
                    tempColor = getColor(tempId);
                } else {
                    matchingOnLane++;
                    break;
                }
            }
            direction = directionCast(-i);
            temp = kingPos;
            temp[ROW] += mod.first;
            temp[COL] += mod.second;
            tempColor = getColor(tempId);
            tempId = getIdAt(temp);
            while( isInBoard(pos) && matchingOnLane < 2) {
                if( kingColor != tempColor ) {
                    if( tempColor != COLOR::EMPTY && matchingOnLane == 0 ) {
                        break;
                    } else if( tempColor != COLOR::EMPTY && matchingOnLane == 1 && set->contains(getTypeAt(temp))) {
                        m_pinnedIdSet.emplace(tempId);
                        m_pinnedVec.push_back({tempId, directionCast(-i)});
                        break;
                    }
                    temp[ROW] += mod.first;
                    temp[COL] += mod.second;
                    tempId = getIdAt(pos);
                    tempColor = getColor(tempId);
                } else {
                    matchingOnLane++;
                    break;
                }
            }
        }
    }

    void Board::diagonalHelper(const Pos& initial, const Direction& direction, int& i) {
        using enum Direction;
        Pos pos = initial;
        const ID initialId = getIdAt(initial);
        const COLOR color = getColor(initialId);
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
        while( isInBoard(pos) ) {
            if( color != posColor ) {
                setMoveAt(initialId, pos, i);
                setAttackedAt(pos);
                i++;
                if( posColor != COLOR::EMPTY ) {
                    return;
                }
                pos[ROW] += x;
                pos[COL] += y;
                posId = getIdAt(pos);
                posColor = getColor(posId);
            } else {
                setDefendedAt(pos);
                return;
            }
        }
    }

    void Board::addDiagonalMoves(const Pos& initial) {
        using enum Direction;
        int i = 0;
        diagonalHelper(initial, NORTHEAST, i);
        diagonalHelper(initial, SOUTHWEST, i);
        diagonalHelper(initial, NORTHWEST, i);
        diagonalHelper(initial, SOUTHEAST, i);
        setMovesIdx(getIdAt(initial), i);
    }

    void Board::cardinalHelper(const Pos& initial, const Direction& direction, int& i) {
        using enum Direction;
        const ID initialId = getIdAt(initial);
        const COLOR color = getColor(initialId);
        Pos pos = initial;
        int card, dim, inc;
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
        if( !((card>>1) == 1) ) { //determine the dimension of the initial pos we are increasing
            dim = ROW;
        }
        if( ((card<<1)>>1) == 1 ) { //determine the direction we are incrementing the dimension in
            inc = -1;
        }
        pos[dim] += inc;
        posColor = getColor(posId);
        while(isInBoard(pos)) {
            if( color != posColor ) {
                setAttackedAt(pos);
                setMoveAt(initialId, pos, i);
                i++;
                if(posColor != COLOR::EMPTY) {
                    return;
                }
                pos[dim] += inc;
                posId = getIdAt(pos);
                posColor = getColor(posId);
            } else {
                setDefendedAt(pos);
                return;
            }
        } 
    }
    

    void Board::addCardinalMoves(const Pos& initial) {
        using enum Direction;
        int i = 0;
        cardinalHelper(initial, NORTH, i);
        cardinalHelper(initial, SOUTH, i);
        cardinalHelper(initial, EAST, i);
        cardinalHelper(initial, WEST, i);
        setMovesIdx(getIdAt(initial), i);
    }

    void Board::addQueenMoves(const Pos& initial) {
        using enum Direction;
        int i = 0;
        cardinalHelper(initial, NORTH, i);
        cardinalHelper(initial, SOUTH, i);
        cardinalHelper(initial, EAST, i);
        cardinalHelper(initial, WEST, i);
        diagonalHelper(initial, NORTHEAST, i);
        diagonalHelper(initial, SOUTHWEST, i);
        diagonalHelper(initial, NORTHWEST, i);
        diagonalHelper(initial, SOUTHEAST, i);
        setMovesIdx(getIdAt(initial), i);
    }

    void Board::addKnightMoves(const Pos& initial) {
        int x, y, i = 0;
        const ID initialId = getIdAt(initial);
        const COLOR color = getColor(initialId);
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
            temp = {castHelper(initial[ROW], x), castHelper(initial[COL], y)};
            if( isInBoard(temp) ) {
                tempColor = getColor(getIdAt(temp));
                if( color != tempColor ) {
                    setAttackedAt(temp);
                    setMoveAt(initialId, temp, i);
                    i++;
                } else {
                    setDefendedAt(temp);
                }
            }
            temp = {castHelper(initial[ROW], y), castHelper(initial[COL], x)};
            if( isInBoard(temp) ) {
                tempColor = getColor(getIdAt(temp));
                if( color != tempColor ) {
                    setAttackedAt(temp);
                    setMoveAt(initialId, temp, i);
                    i++;
                } else {
                    setDefendedAt(temp);
                }
            }
        }
    }

    void Board::addKingMoves(const Pos& initial) {
        const ID initialId = getIdAt(initial);
        const COLOR color = getColor(initialId);
        int i = 0;
        Pos temp;
        ID tempId;
        COLOR tempColor;

        for(const std::pair<int,int>& mod : m_mods) {
            temp = {castHelper(initial[ROW], mod.first), castHelper(initial[COL], mod.second)};
            if( isInBoard(temp) ) {
                tempId = getIdAt(temp);
                tempColor = getColor(tempId);
                if( color != tempColor ) {
                    setAttackedAt(temp);
                    setMoveAt(initialId, temp, i);
                    i++;
                } else {
                    setDefendedAt(temp);
                }
            }
        }
    }

    void Board::addPawnMoves(const Pos& initial) {
        const ID initialId = getIdAt(initial);
        const COLOR color = getColor(initialId);
        const int pawnRow = getPawnRow();
        int i = 0;
        Pos temp;
        ID tempId;
        COLOR tempColor;

        if( m_isWhiteTurn ) {
            m_pawnMods[0] = m_mods[6];
            m_pawnMods[1] = m_mods[7];
        } else {
            m_pawnMods[0] = m_mods[4];
            m_pawnMods[1] = m_mods[5];
        }
        if( initial[ROW] == pawnRow) {
            temp = {castHelper(initial[ROW], 2*m_pawnMods[0].first), initial[COL]};
            if( isInBoard(temp) ) {
                tempId = getIdAt(temp);
                tempColor = getColor(tempId);
                if( color == COLOR::EMPTY ) {
                    setMoveAt(initialId, temp, i);
                    i++;
                }
            }
        }
        temp = {castHelper(initial[ROW], m_pawnMods[0].first), initial[COL]};
        if( isInBoard(temp) ) {
            tempId = getIdAt(temp);
            tempColor = getColor(tempId);
            if( color == COLOR::EMPTY ) {
                setMoveAt(initialId, temp, i);
                i++;
            }
        }
        temp = {castHelper(initial[ROW], m_pawnMods[0].first), castHelper(initial[COL], m_pawnMods[0].second)};
        if( isInBoard(temp) ) {
            tempId = getIdAt(temp);
            tempColor = getColor(tempId);
            if( color != tempColor ) {
                setAttackedAt(temp);
                setMoveAt(initialId, temp, i);
                i++;
            } else {
                setDefendedAt(temp);
            }
        }
        temp = {castHelper(initial[ROW], m_pawnMods[1].first), castHelper(initial[COL], m_pawnMods[1].second)};
        if( isInBoard(temp) ) {
            tempId = getIdAt(temp);
            tempColor = getColor(tempId);
            if( color != tempColor ) {
                setAttackedAt(temp);
                setMoveAt(initialId, temp, i);
                i++;
            } else {
                setDefendedAt(temp);
            }
        }
    }

    void Board::genMoves() {
        addCardinalMoves(getPos(W_ROOK1));
        addCardinalMoves(getPos(W_ROOK1));
        addDiagonalMoves(getPos(W_BISHOP1));
        addDiagonalMoves(getPos(W_BISHOP2));
        addKnightMoves(getPos(W_KNIGHT1)); 
        addKnightMoves(getPos(W_KNIGHT2));
        addKingMoves(getPos(W_KING));
        addQueenMoves(getPos(W_QUEEN));

        addPawnMoves(getPos(W_PAWN1));
        addPawnMoves(getPos(W_PAWN2));
        addPawnMoves(getPos(W_PAWN3));
        addPawnMoves(getPos(W_PAWN4));
        addPawnMoves(getPos(W_PAWN5));
        addPawnMoves(getPos(W_PAWN6));
        addPawnMoves(getPos(W_PAWN7));
        addPawnMoves(getPos(W_PAWN8));

        addCardinalMoves(getPos(B_ROOK2));
        addCardinalMoves(getPos(B_ROOK2));
        addDiagonalMoves(getPos(B_BISHOP1));
        addDiagonalMoves(getPos(B_BISHOP2));
        addKnightMoves(getPos(B_KNIGHT1));
        addKnightMoves(getPos(B_KNIGHT2));
        addKingMoves(getPos(B_KING));
        addQueenMoves(getPos(B_QUEEN));

        //std::cout << "HERE" << std::endl;
        addPawnMoves(getPos(B_PAWN1));
        addPawnMoves(getPos(B_PAWN2));
        addPawnMoves(getPos(B_PAWN3));
        addPawnMoves(getPos(B_PAWN4));
        addPawnMoves(getPos(B_PAWN5));
        addPawnMoves(getPos(B_PAWN6));
        addPawnMoves(getPos(B_PAWN7));
        addPawnMoves(getPos(B_PAWN8));
    }

// ♔♕♖♗♘♙♚♛♜♝♞♟
//\u2654 \u2655 \u2656 \u2657 \u2658 \u2659 \u265a \u265b \u265c \u265d \u265e \u265f 
    char32_t Board::getGlyph(const ID& id) {
        char32_t res;
        switch(id) {
            case(EMPTY): {
                res =  U' ';
                break;
            }
            default:
                const Type type = getTypeAt(getPos(id));
                switch(type) {
                case(Type::W_KING):
                    res = U'\u2654';
                    break;
                case(Type::W_QUEEN):
                    res = U'\u2655';
                    break;
                case(Type::W_ROOK):
                    res = U'\u2656';
                    break;
                case(Type::W_BISHOP):
                    res = U'\u2657';
                    break;
                case(Type::W_KNIGHT):
                    res = U'\u2658';
                    break;
                case(Type::W_PAWN):
                    res = U'\u2659';
                    break;
                case(Type::B_KING):
                    res = U'\u265a';
                    break;
                case(Type::B_QUEEN):
                    res = U'\u265b';
                    break;
                case(Type::B_ROOK):
                    res = U'\u265c';
                    break;
                case(Type::B_BISHOP):
                    res = U'\u265d';
                    break;
                case(Type::B_KNIGHT):
                    res = U'\u265e';
                    break;
                case(Type::B_PAWN):
                    res = U'\u265f';
                    break;
                }
        }
        return res;
    }

    void Board::displayBoard() {
        std::u32string str;
        str.append(U"-----------------\n");
        for(int i = 1; i < m_board.size()+1; i++) {
            str += U'|';
            str += getGlyph(m_board[i-1]);
            str += U'|';
            if( i%8 == 0 )
                str.append(U"\n-----------------\n");
        }
        std::cout << std::string(str.begin(), str.end()) << std::endl; }

    void Board::printMoveOffset() {
        for(int n : m_moveOffset) {
            std::cout << n << '\n';
            std::cout << std::endl;
        }
    }

}
