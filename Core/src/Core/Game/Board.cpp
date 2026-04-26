#include <Chess/Core/Game/Board.hpp>
#include <Chess/Core/declarations.hpp>

#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <algorithm> // Required for std::rotate

// TODO 
//      * implement a way to castle
//          - m_hasCastled boolean private members
//      * implement a way to promote pawns
//      * implement a way to en passant
//          - if last move was a pawn and it moved from its starting position
//              to be adjacent to a pawn of opposite color on an adjacent lane, then the
//              pawn of opposite color on the adjacent lane may en passant
//      * implement checking logic fully
//      * Fix the indexing mismatch in most of the functions
//      * Add "full" king movement logic with isAttacked() and isDefended()
//      * Gamestates
//          - implement a 50 move long move history array which contains entries describing
//              the moving piece, the old square, the new square, and if a "take" took place
//          - for the 50 move no take rule its self explanatory
//          - for threefold repetition rule it is also self explanatory
//
//      *   Testing TODO
//
//
//
//      Notes for game loop semantics
//
//      Flow will be generate pinned pieces
//                   generate checked pieces, 
//                   generate attacked vecs and defended member vector,
//                   if pieces are pinned, filter the pinned pieces moves
//                   if in check, filter all pieces of the checked color according to the moves of the checking piece


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
        m_moves(MAX_MOVES, Pos({8,8})),
        m_moveOffset({0}),
        m_defended(64, false),
        m_attackedWhite(64, false),
        m_attackedBlack(64, false),
        m_pinnedIdSet(),
        m_diagonalSet({Type::W_BISHOP, Type::B_BISHOP, Type::W_QUEEN, Type::B_QUEEN}),
        m_cardinalSet({Type::W_ROOK, Type::B_ROOK, Type::W_QUEEN, Type::B_QUEEN}),
        m_promotionWhite({Type::W_ROOK, Type::W_KNIGHT, Type::W_BISHOP, Type::W_QUEEN}),
        m_promotionBlack({Type::B_ROOK, Type::B_KNIGHT, Type::B_BISHOP, Type::B_QUEEN}),
        m_promotionMoves({MAX_MOVES_ROOK, MAX_MOVES_KNIGHT, MAX_MOVES_BISHOP, MAX_MOVES_QUEEN}),
        m_checked(CriticalPiece({EMPTY, Direction::NORTH})),
        m_moveLog(),
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
            m_moves.reserve(MAX_MOVES_THEORETICAL);
            genMoveOffsets();
            m_isWhiteTurn = true;
            m_isWhiteChecked = false;
            m_isBlackChecked = false;
            m_pinnedArr.fill({8,8});
            m_moveLog.reserve(50);
        }

    Board::Board(const std::array<ID, 64>& board, const std::array<Piece, 32>& pieces) : 
        m_board(board), 
        m_pieceArr(pieces),
        m_moves(MAX_MOVES, Pos({8,8})),
        m_moveOffset({0}),
        m_defended(64, false),
        m_attackedWhite(64, false),
        m_attackedBlack(64, false),
        m_pinnedIdSet(),
        m_diagonalSet({Type::W_BISHOP, Type::B_BISHOP, Type::W_QUEEN, Type::B_QUEEN}),
        m_cardinalSet({Type::W_ROOK, Type::B_ROOK, Type::W_QUEEN, Type::B_QUEEN}),
        m_promotionWhite({Type::W_ROOK, Type::W_KNIGHT, Type::W_BISHOP, Type::W_QUEEN}),
        m_promotionBlack({Type::B_ROOK, Type::B_KNIGHT, Type::B_BISHOP, Type::B_QUEEN}),
        m_promotionMoves({MAX_MOVES_ROOK, MAX_MOVES_KNIGHT, MAX_MOVES_BISHOP, MAX_MOVES_QUEEN}),
        m_checked(CriticalPiece({EMPTY, Direction::NORTH})),
        m_moveLog(),
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
            m_moves.reserve(MAX_MOVES_THEORETICAL);
            genMoveOffsets();
            m_isWhiteTurn = true;
            m_isWhiteChecked = false;
            m_isBlackChecked = false;
            m_pinnedArr.fill({8,8});
            m_moveLog.reserve(50);
    }

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
        int castedId = (int)id-1;
        return m_pieceArr[castedId].position;
    }

    void Board::setIdAt(const Pos& pos, const ID& id){
         m_board[posTranslate(pos)] = id;
    }

    Type Board::getTypeAt(const Pos& pos) {
        return m_pieceArr[(int)getIdAt(pos)-1].type;
    }

    void Board::genMoveOffsets() {
        m_moveOffset[0] = 0;
        for(int i = 1; i < m_pieceArr.size(); i++) {
            m_moveOffset[i] = m_moveOffset[i-1] + m_pieceArr[i-1].reserved;
        }
    }

    std::string typeToString(const Type& type) {
        std::string res;
        switch(type) {
            case( Type::W_ROOK):
                res = "W_ROOK";
                break;
            case( Type::W_KNIGHT):
                res = "W_KNIGHT";
                break;
            case( Type::W_BISHOP):
                res = "W_BISHOP";
                break;
            case( Type::W_QUEEN):
                res = "W_QUEEN";
                break;
            case( Type::W_KING):
                res = "W_KING";
                break;
            case( Type::B_KNIGHT):
                res = "B_KNIGHT";
                break;
            case( Type::B_BISHOP):
                res = "B_BISHOP";
                break;
            case( Type::B_ROOK):
                res = "B_ROOK";
                break;
            case( Type::B_QUEEN):
                res = "B_QUEEN";
                break;
            case( Type::B_KING):
                res = "B_KING";
                break;
            case( Type::B_PAWN):
                res = "B_PAWN";
                break;
            case( Type::W_PAWN):
                res = "W_PAWN";
                break;
        }
        return res;

    }

    void Board::move(const ID& id, const Pos& target) {
        const int castedId = (int)id - 1;
        const Pos old = m_pieceArr[castedId].position;
        const ID idToReplace = getIdAt(target);

        LogEntry entry;
        entry.id = id;
        entry.from = old;
        entry.to = target;
        if( idToReplace != EMPTY ) {
            const int castedOldId = (int) idToReplace - 1;
            m_pieceArr[castedOldId].position = Pos{8,8};
            entry.taken = true;
        } else {
            entry.taken = false;
        }
        m_pieceArr[castedId].position = target;
        setIdAt(target, id);
        setIdAt(old, ID::EMPTY);
        updateMoveLog(entry);
    }

    void Board::updateMoveLog(const LogEntry& entry) {
        const int size = m_moveLog.size();
        if( size != 0 && size == 50) {
            std::rotate(m_moveLog.begin(), m_moveLog.end()-1, m_moveLog.end());
            m_moveLog[0] = entry;
        } else if( size != 0 ) {
            m_moveLog.push_back(m_moveLog[size-1]);
            if( size > 1 ) {
                // Shift contents of m_moveLog to the right by one
                for(int i = size-2; i >= 0; i--) {
                    m_moveLog[i+1] = m_moveLog[i];
                }
            }
            m_moveLog[0] = entry;
        } else {
            m_moveLog.push_back(entry);
        }
    }

    bool Board::isPawnId(const ID& id) {
        bool flag = false;
        const int castedId = (int) id;
        if( 8 < castedId && castedId < 17 || 24 < castedId && castedId < 33 ) {
            flag = true;
        }
        return flag;
    }

    /* Returns true if the 50 move no take rule applies to the game state */
    bool Board::isFiftyMoves() {
        if( m_moveLog.size() != 50 ) {
            return false;
        }
        bool res = false;
        for(const LogEntry& entry : m_moveLog) {
            if( entry.taken == true ) {
                res = true;
            }
        }
        return !res;
    }

    /*
    bool Board::isThreefoldRepetition() {
        
    }
    */

    bool Board::isPawnPromotable() {
        bool flag = false;
        const LogEntry entry = m_moveLog[0];
        if( isPawnId(entry.id) ) {
            if( entry.to[ROW] == 0 || entry.to[COL] == 7 ) {
                flag = true;
            }
        }
        return flag;
    }

    void Board::promotePawn() {
        const LogEntry entry = m_moveLog[0];
        const ID id = entry.id;
        const TypeMoves promoteTo = getPromotionChoice(getColor(id));
        const int castedId = (int)id-1;
        m_pieceArr[castedId].type = promoteTo.type;
        m_pieceArr[castedId].reserved = promoteTo.maxMoves;
        m_promotedPawnSet.emplace(id);
        genMoveOffsets();
    }

    const std::array<Type, 4>* Board::getPromotionArr(const COLOR& color) {
        const std::array<Type, 4>* res;
        if( color == COLOR::WHITE ) {
            res = &m_promotionWhite;
        } else {
            res = &m_promotionBlack;
        }
        return res;
    }

    TypeMoves Board::getPromotionChoice(const COLOR& color) {
        TypeMoves res;
        int choice;
        const std::array<Type, 4>* promotionArr = getPromotionArr(color);
        do {
            std::cout << "Enter [1-4] to promote pawn:\n1 for Rook\n2 for Knight\n3 for Bishop\n4 for Queen" << std::endl;
            std::cin >> choice;
        } while( choice < 1 || choice > 4 );
        const int idx = choice-1;
        res.type = promotionArr->at(idx);
        res.maxMoves = m_promotionMoves[idx];
        return res;
    }

    bool Board::isValidMove(const ID& id, const Pos& target) {
        const int castedId = (int)id -1,
                  idx = m_pieceArr[castedId].movesIdx;
        if( idx == 0 ) {
            return false;
        }

        const int offset = m_moveOffset[castedId];
        bool flag = false;
        Pos temp;
        
        for(int i = 0; i < idx; i++) {
            temp = m_moves[offset + i];
            if( temp[ROW] == target[ROW] && temp[COL] == target[COL] )
                flag = true;
        }
        return flag;
    }

    bool Board::isValidMod(const int& val, const int& mod) {
        bool res = false;
        if( ROW_LOWER_BOUND < val + mod && val + mod < ROW_UPPER_BOUND ) {
            res = true;
        }
        return res;
    }

    void Board::setTurn(bool isWhite) {
        m_isWhiteTurn = isWhite;
    }

    bool Board::getTurn() const {
        return m_isWhiteTurn;
    }

    void Board::nextTurn() {
        m_isWhiteTurn = !m_isWhiteTurn;
        std::fill(m_moves.begin(), m_moves.end(), Pos{8,8});
        m_pinnedArr.fill(Pos{8,8});
        m_checked = CriticalPiece({EMPTY, Direction::NORTH});
        std::fill(m_defended.begin(), m_defended.end(), false);
        std::fill(m_attackedWhite.begin(), m_attackedWhite.end(), false);
        std::fill(m_attackedBlack.begin(), m_attackedBlack.end(), false);
    }


    const std::array<ID, 64> &Board::getBoard() const {
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
        const int idx = posTranslate(pos);
        return m_board[idx];
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

    std::string Board::colorToString(const COLOR& color) {
        std::string res;

        switch(color) {
            case(COLOR::BLACK):
                res = "BLACK";
                break;
            case(COLOR::WHITE):
                res = "WHITE";
                break;
            default:
                res = "EMPTY";
                break;
        }
        return res;
    }

    void Board::printColor(const COLOR& color) {
        std::cout << "Color: " << colorToString(color) << std::endl;
    }

    void Board::printId(const ID& id) {
        std::cout << "Id: " << idToString(id) << std::endl;
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

    /*
    std::vector<bool>& Board::getAttackedVec() {
        if( m_isWhiteTurn ) {
            return m_attackedBlack;
        } else {
            return m_attackedWhite;
        }
    }
    */

    /* Returns the opposite color m_attacked[] for color parameter */
    std::vector<bool>& Board::getAttackedVec(const COLOR& color) {
        if( color == COLOR::WHITE ) {
            return m_attackedWhite;
        } else {
            return m_attackedBlack;
        }
    }

    void Board::setAttackedAt(const Pos& pos, const COLOR& color ) {
        getAttackedVec(color)[posTranslate(pos)] = true;
    }

    bool Board::isAttackedAt(const Pos& pos, const COLOR& color) {
        return getAttackedVec(color)[posTranslate(pos)];
    }

    void Board::setDefendedAt(const Pos& pos) {
        m_defended[posTranslate(pos)] = true;
    }

    bool Board::isDefendedAt(const Pos& pos) {
        return m_defended[posTranslate(pos)];
    }

    void Board::setMovesIdx(const ID& id, const int& i) {
        const int castedId = (int) id-1;
        m_pieceArr[castedId].movesIdx = i;
    }

    void Board::setMoveAt(const ID& id, const Pos& pos, const int& i) {
        const int castedId = (int) id-1,
              idx = m_moveOffset[castedId] + i;
        m_moves[idx] = pos;
    }

    void Board::setChecked(const ID& checked, const Direction direction) {
        m_checked.id = checked;
        m_checked.direction = direction;
    }

    Direction Board::directionCast(const int& i) {
        return static_cast<Direction>(i);
    }

    int Board::getPawnRow(const COLOR& color) {
        if( color == COLOR::WHITE ) {
            return WHITE_PAWN_ROW;
        } else {
            return BLACK_PAWN_ROW;
        }
    }

    bool Board::isMatchingType(const ID& id, const Type type) {
        bool flag = false; 
        switch(id) {
            case(EMPTY):
                break;
            default:
                flag = getTypeAt(m_pieceArr[(int)id-1].position) == type;
        }
        return flag;
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

        //Knight squares
        int x, y;
        Pos temp;
        COLOR tempColor;
        for(int i = 0b00; i < 4; i++) {
            x = 2, y = 1;
            if( (i>>1) == 1 ) {
                x = -x;
            }
            if( i%2 == 1 ) {
                y = -y;
            }
            temp = kingPos;;
            if( isValidMod(temp[ROW], x) && isValidMod(temp[COL], y) ) {
                temp[ROW] += x;
                temp[COL] += y;
                tempColor = getColor(getIdAt(temp));
                tempId = getIdAt(temp);
                if( kingColor != tempColor && isMatchingType(tempId, Type::W_KNIGHT) || isMatchingType(tempId, Type::B_KNIGHT) ) {
                    setChecked(tempId, m_checked.direction);
                    return;
                }
            }
            temp = kingPos;
            if( isValidMod(temp[ROW], y) && isValidMod(temp[COL], x) ) {
                temp[ROW] += y;
                temp[COL] += x;
                tempColor = getColor(getIdAt(temp));
                tempId = getIdAt(temp);
                if( kingColor != tempColor && isMatchingType(tempId, Type::W_KNIGHT) || isMatchingType(tempId, Type::B_KNIGHT) ) {
                    setChecked(tempId, m_checked.direction);
                    return;
                }
            }
        }
        /* Don't need this but may keep for faster search on adjacent squares for certain pieces, i.e. queens, bishops, rooks
        //King squares
        for(const std::pair<int,int>& mod : m_mods) {
            temp = kingPos;
            if( isValidMod(temp[ROW], mod.first) && isValidMod(temp[COL], mod.second) ) {
                temp[ROW] += mod.first;
                temp[COL] += mod.second;
                tempColor = getColor(getIdAt(temp));
                tempId = getIdAt(temp);
                if( kingColor != tempColor && isMatchingType(tempId, Type::W_KING) || isMatchingType(tempId, Type::B_KING)) {
                    setChecked(tempId, m_checked.direction);
                    return;
                }
            }
        }
        */
        //Pawn squares
        if( kingColor == COLOR::BLACK ) {
            m_pawnMods[0] = m_mods[6];
            m_pawnMods[1] = m_mods[7];
        } else {
            m_pawnMods[0] = m_mods[4];
            m_pawnMods[1] = m_mods[5];
        }
        for(const std::pair<int,int>& mod : m_pawnMods) {
            temp = kingPos;
            if( isValidMod(temp[ROW], mod.first) && isValidMod(temp[COL], mod.second) ) {
                temp[ROW] += mod.first;
                temp[COL] += mod.second;
                tempColor = getColor(getIdAt(temp));
                tempId = getIdAt(temp);
                if( kingColor != tempColor && isMatchingType(tempId, Type::W_PAWN) || isMatchingType(tempId, Type::B_PAWN) ) {
                    setChecked(tempId, m_checked.direction);
                    return;
                }
            }
        }
        //Diagonal & Cardinal squares
        for(int i = 1; i < 5; i++) {
            direction = directionCast(i);
            std::pair<int,int> mod = getMod(direction);
            set = getMatchingSet(direction);
            temp = kingPos;

            while( isValidMod(temp[ROW], mod.first) && isValidMod(temp[COL], mod.second) ) {
                temp[ROW] += mod.first;
                temp[COL] += mod.second;
                tempId = getIdAt(temp);
                tempColor = getColor(tempId);
                if( kingColor != tempColor && tempColor != COLOR::EMPTY ) {
                    if( set->contains(getTypeAt(temp)) ) {
                        setChecked(tempId, directionCast(-i));
                        return;
                    } else {break;}
                } else if( kingColor == tempColor ) {
                    break;
                }
            }

            direction = directionCast(-i);
            mod = getMod(direction);
            temp = kingPos;
            while( isValidMod(temp[ROW], mod.first) && isValidMod(temp[COL], mod.second) ) {
                temp[ROW] += mod.first;
                temp[COL] += mod.second;
                tempId = getIdAt(temp);
                tempColor = getColor(tempId);
                if( kingColor != tempColor && tempColor != COLOR::EMPTY ) {
                    if( set->contains(getTypeAt(temp)) ) {
                        setChecked(tempId, directionCast(-i));
                        return;
                    } else {break;}
                } else if( kingColor == tempColor ) {
                    break;
                }
            }
        }
        //std::cout << "HERE" << std::endl;
    }

    void Board::genPinned() {
        using enum Direction;
        const ID kingId = getKingId();
        const Pos kingPos = getPos(kingId);
        const COLOR kingColor = getColor(kingId);
        const std::set<Type>* set;
        ID tempId;
        Direction direction;
        Pos temp;
        COLOR tempColor;

        for(int i = 1; i < 5; i++) {
            int matchingOnLane = 0; // counts the pieces matching king color on lane
            direction = directionCast(i);
            std::pair<int,int> mod = getMod(direction);
            set = getMatchingSet(direction); // only needs to be called once since -Direction is still the same axis
            temp = kingPos;
            if( isValidMod(temp[ROW], mod.first) && isValidMod(temp[COL], mod.second) ) {
                temp[ROW] += mod.first;
                temp[COL] += mod.second;
            } else { goto jump; }
            tempId = getIdAt(temp);
            tempColor = getColor(tempId);
            Pos pinnedPos;
            ID pinnedId;
            while( isInBoard(temp) && matchingOnLane < 2) {
                if( kingColor != tempColor ) {
                    if( matchingOnLane == 0 && tempColor != COLOR::EMPTY ) {
                        break;
                    } else if( matchingOnLane == 1 && tempColor != COLOR::EMPTY && set->contains(getTypeAt(temp))) {
                        m_pinnedIdSet.emplace(pinnedId);
                        Pos pinningPos = temp;
                        Direction tempDirection = directionCast(-i);
                        mod = getMod(tempDirection);
                        int j = 0;
                        while( pinnedPos != pinningPos ) {
                            m_pinnedArr[((int)pinnedId-1)*6 + j] = Pos(pinningPos);
                            pinningPos[ROW] += mod.first;
                            pinningPos[COL] += mod.second;
                            j++;
                        }
                        break;
                    }
                } else {
                    matchingOnLane++;
                    pinnedPos = temp;
                    pinnedId = getIdAt(pinnedPos);
                }
                if( isValidMod(temp[ROW], mod.first) && isValidMod(temp[COL], mod.second) ) {
                    temp[ROW] += mod.first;
                    temp[COL] += mod.second;
                    tempId = getIdAt(temp);
                    tempColor = getColor(tempId);
                } else { goto jump; }
            }
            // I couldn't find another simple way to implement this behavior, so
            //  this is reached either when the first part of the loop is done executing
            //  or when a position we are iterating over in the first part of the loop is
            //  out of bounds
        jump:
            direction = directionCast(-i);
            mod = getMod(direction);
            set = getMatchingSet(direction); 
            temp = kingPos;
            matchingOnLane = 0; 
            if( isValidMod(temp[ROW], mod.first) && isValidMod(temp[COL], mod.second) ) {
                temp[ROW] += mod.first;
                temp[COL] += mod.second;
            } else { continue; }
            tempId = getIdAt(temp);
            tempColor = getColor(tempId);
            bool validTemp = true;
            while( validTemp && matchingOnLane < 2) {
                if( kingColor != tempColor ) {
                    if( matchingOnLane == 0 && tempColor != COLOR::EMPTY ) {
                        break;
                    } else if( matchingOnLane == 1 && tempColor != COLOR::EMPTY && set->contains(getTypeAt(temp)) ) {
                        m_pinnedIdSet.emplace(pinnedId);
                        Pos pinningPos = temp;
                        Direction tempDirection = directionCast(i);
                        mod = getMod(tempDirection);
                        int j = 0;
                        while( pinnedPos != pinningPos ) {
                            m_pinnedArr[((int)pinnedId-1)*6 + j] = Pos(pinningPos);
                            pinningPos[ROW] += mod.first;
                            pinningPos[COL] += mod.second;
                            j++;
                        }
                        break;
                    }
                } else {
                    matchingOnLane++;
                    pinnedPos = temp;
                    pinnedId = getIdAt(pinnedPos);
                }
                if( isValidMod(temp[ROW], mod.first) && isValidMod(temp[COL], mod.second) ) {
                    temp[ROW] += mod.first;
                    temp[COL] += mod.second;
                    tempId = getIdAt(temp);
                    tempColor = getColor(tempId);
                } else {
                    validTemp = false;
                }
            }
        }
    }

    void Board::diagonalHelper(const Pos& initial, const Direction& direction, int& i) {
        using enum Direction;
        Pos pos = initial;
        const ID initialId = getIdAt(initial);
        const COLOR color = getColor(initialId);
        ID posId;
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
        if( card %2 == 1 ) {
            y = -y;
        }
        if( isValidMod(pos[ROW], x) && isValidMod(pos[COL], y) ) {
            pos[ROW] += x;
            pos[COL] += y;
        } else { return; }
        posId = getIdAt(pos);
        posColor = getColor(posId);
        while( isInBoard(pos) ) {
            if( color != posColor ) {
                setMoveAt(initialId, pos, i);
                setAttackedAt(pos, color);
                i++;
                if( posColor != COLOR::EMPTY ) {
                    return;
                }
        if( isValidMod(pos[ROW], x) && isValidMod(pos[COL], y) ) {
                    pos[ROW] += x;
                    pos[COL] += y;
                } else { return; }
                posId = getIdAt(pos);
                posColor = getColor(posId);
            } else {
                setDefendedAt(pos);
                return;
            }
        }
    }

    void Board::addDiagonalMoves(const Pos& initial) {
        if( posTranslate(initial) > 63 ) {
            return;
        }
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
        ID posId;
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
        if( card %2 == 1 ) { //determine the direction we are incrementing the dimension in
            inc = -1;
        }
        if( isValidMod(pos[dim], inc) ) {
            pos[dim] += inc;
        } else { return; }

        posColor = getColor(getIdAt(pos));
        while(isInBoard(pos)) {
            if( color != posColor ) {
                setAttackedAt(pos, color);
                setMoveAt(initialId, pos, i);
                i++;
                if(posColor != COLOR::EMPTY) {
                    return;
                }
                if( isValidMod(pos[dim], inc) ) {
                    pos[dim] += inc;
                } else { return; }
                posId = getIdAt(pos);
                posColor = getColor(posId);
            } else {
                setDefendedAt(pos);
                return;
            }
        } 
    }
    

    void Board::addCardinalMoves(const Pos& initial) {
        if( posTranslate(initial) > 63 ) {
            return;
        }
        using enum Direction;
        int i = 0;
        cardinalHelper(initial, NORTH, i);
        cardinalHelper(initial, SOUTH, i);
        cardinalHelper(initial, EAST, i);
        cardinalHelper(initial, WEST, i);
        setMovesIdx(getIdAt(initial), i);
    }

    void Board::addQueenMoves(const Pos& initial) {
        if( posTranslate(initial) > 63 ) {
            return;
        }
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
        if( posTranslate(initial) > 63 ) {
            return;
        }
        int x, y, idx = 0;
        std::uint8_t temp_row, temp_col;
        const ID initialId = getIdAt(initial);
        const COLOR color = getColor(initialId);
        Pos temp;
        COLOR tempColor;
        for(int i = 0b00; i < 4; i++) {
            x = 2, y = 1;
            if( (i>>1) == 1 ) {
                x = -x;
            }
            if( i%2 == 1 ) {
                y = -y;
            }

            if( (int) initial[ROW] + x > ROW_LOWER_BOUND && (int) initial[COL] + y > COL_LOWER_BOUND ) {
                temp = Pos{castHelper(initial[ROW], x), castHelper(initial[COL], y)};
                if( isInBoard(temp) ) {
                    tempColor = getColor(getIdAt(temp));
                    if( color != tempColor ) {
                        setAttackedAt(temp, color);
                        setMoveAt(initialId, temp, idx);
                        idx++;
                    } else {
                        setDefendedAt(temp);
                    }
                }
            }
            if( (int) initial[ROW] + y > ROW_LOWER_BOUND && (int) initial[COL] + x > COL_LOWER_BOUND ) {
                temp = Pos{castHelper(initial[ROW], y), castHelper(initial[COL], x)};
                if( isInBoard(temp) ) {
                    tempColor = getColor(getIdAt(temp));
                    if( color != tempColor ) {
                        setAttackedAt(temp, color);
                        setMoveAt(initialId, temp, idx);
                        idx++;
                    } else {
                        setDefendedAt(temp);
                    }
                }
            }
        }
        setMovesIdx(initialId, idx);
    }

    void Board::addKingMoves(const Pos& initial) {
        if( posTranslate(initial) > 63 ) {
            return;
        }
        const ID initialId = getIdAt(initial);
        const COLOR color = getColor(initialId);
        int i = 0;
        Pos temp;
        ID tempId;
        COLOR tempColor;

        for(const std::pair<int,int>& mod : m_mods) {
            int rowMod = mod.first;
            int colMod = mod.second;
            if( isValidMod(initial[ROW], rowMod) && isValidMod(initial[COL], colMod) ) {
                temp = Pos({castHelper(initial[ROW], rowMod), castHelper(initial[COL], colMod)});
                tempId = getIdAt(temp);
                tempColor = getColor(tempId);
                if( color != tempColor ) {
                    setAttackedAt(temp, color);
                    setMoveAt(initialId, temp, i);
                    i++;
                } else {
                    setDefendedAt(temp);
                }
            }
        }
        setMovesIdx(initialId, i);
    }

    void Board::addPawnMoves(const Pos& initial) {
        if( posTranslate(initial) > 63 ) {
            return;
        }
        const ID initialId = getIdAt(initial);
        const COLOR color = getColor(initialId);
        const int pawnRow = getPawnRow(color);
        int i = 0;
        Pos temp;
        ID tempId;
        COLOR tempColor;

        if( color == COLOR::BLACK ) {
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
                if( tempColor == COLOR::EMPTY ) {
                    setMoveAt(initialId, temp, i);
                    i++;
                }
            }
        }
        temp = {castHelper(initial[ROW], m_pawnMods[0].first), initial[COL]};
        if( isInBoard(temp) ) {
            tempId = getIdAt(temp);
            tempColor = getColor(tempId);
            if( tempColor == COLOR::EMPTY ) {
                setMoveAt(initialId, temp, i);
                i++;
            }
        }
        int tempRow = (int) initial[ROW] + m_pawnMods[0].first;
        int tempCol = (int) initial[COL] + m_pawnMods[0].second;
        if( ROW_LOWER_BOUND < tempRow && tempRow < ROW_UPPER_BOUND && COL_LOWER_BOUND < tempCol && tempCol < COL_UPPER_BOUND ) {
            temp = {castHelper(initial[ROW], m_pawnMods[0].first), castHelper(initial[COL], m_pawnMods[0].second)};
            if( isInBoard(temp) ) { // this is redundant but im keeping it for now
                tempId = getIdAt(temp);
                tempColor = getColor(tempId);
                if( color != tempColor && tempColor == COLOR::WHITE || tempColor == COLOR::BLACK ) {
                    setAttackedAt(temp, color);
                    setMoveAt(initialId, temp, i);
                    i++;
                } else {
                    setDefendedAt(temp);
                }
            }
        }
        tempRow = (int) initial[ROW] + m_pawnMods[1].first;
        tempCol = (int) initial[COL] + m_pawnMods[1].second;
        if( ROW_LOWER_BOUND < tempRow && tempRow < ROW_UPPER_BOUND && COL_LOWER_BOUND < tempCol && tempCol < COL_UPPER_BOUND ) {
            temp = {castHelper(initial[ROW], m_pawnMods[1].first), castHelper(initial[COL], m_pawnMods[1].second)};
            if( isInBoard(temp) ) {
                tempId = getIdAt(temp);
                tempColor = getColor(tempId);
                if( color != tempColor && tempColor == COLOR::WHITE || tempColor == COLOR::BLACK ) {
                    setAttackedAt(temp, color);
                    setMoveAt(initialId, temp, i);
                    i++;
                } else {
                    setDefendedAt(temp);
                }
            }
        }
        setMovesIdx(initialId, i);
    }

    void Board::genMoves() {
        addCardinalMoves(getPos(W_ROOK1));
        addCardinalMoves(getPos(W_ROOK2));
        addKnightMoves(getPos(W_KNIGHT1)); 
        addKnightMoves(getPos(W_KNIGHT2));
        addDiagonalMoves(getPos(W_BISHOP1));
        addDiagonalMoves(getPos(W_BISHOP2));
        addQueenMoves(getPos(W_QUEEN));
        addKingMoves(getPos(W_KING));

        addPawnMoves(getPos(W_PAWN1));
        addPawnMoves(getPos(W_PAWN2));
        addPawnMoves(getPos(W_PAWN3));
        addPawnMoves(getPos(W_PAWN4));
        addPawnMoves(getPos(W_PAWN5));
        addPawnMoves(getPos(W_PAWN6));
        addPawnMoves(getPos(W_PAWN7));
        addPawnMoves(getPos(W_PAWN8));

        addCardinalMoves(getPos(B_ROOK1));
        addCardinalMoves(getPos(B_ROOK2));
        addKnightMoves(getPos(B_KNIGHT1));
        addKnightMoves(getPos(B_KNIGHT2));
        addDiagonalMoves(getPos(B_BISHOP1));
        addDiagonalMoves(getPos(B_BISHOP2));
        addQueenMoves(getPos(B_QUEEN));
        addKingMoves(getPos(B_KING));

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
    char32_t Board::getGlyphUnicode(const ID& id) {
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

    char Board::getGlyph(const ID& id) {
        char32_t res;
        switch(id) {
            case(ID::EMPTY): {
                res =  ' ';
                break;
            }
            default:
                const Type type = getTypeAt(getPos(id));
                switch(type) {
                case(Type::W_KING):
                    res = 'K';
                    break;
                case(Type::W_QUEEN):
                    res = 'Q';
                    break;
                case(Type::W_ROOK):
                    res = 'R';
                    break;
                case(Type::W_BISHOP):
                    res = 'B';
                    break;
                case(Type::W_KNIGHT):
                    res = 'N';
                    break;
                case(Type::W_PAWN):
                    res = 'P';
                    break;
                case(Type::B_KING):
                    res = 'k';
                    break;
                case(Type::B_QUEEN):
                    res = 'q';
                    break;
                case(Type::B_ROOK):
                    res = 'r';
                    break;
                case(Type::B_BISHOP):
                    res = 'b';
                    break;
                case(Type::B_KNIGHT):
                    res = 'n';
                    break;
                case(Type::B_PAWN):
                    res = 'p';
                    break;
                }
        }
        return res;
    }

    void Board::displayBoard() {
        std::string str;
        str.append("------------------------\n");
        for(int i = 1; i < m_board.size()+1; i++) {
            str += '|';
            str += getGlyph(m_board[i-1]); 
            str += '|';
            if( i%8 == 0 )
                str.append("\n------------------------\n");
        }
        std::cout << str << std::endl; 
    }

    void Board::printBoard(const std::array<ID, 64>& board) {
        int i = 0;
        for( const ID& id : board ) {
            std::cout << "Pos: " << idToString(id) << '\n';
        }
    }

    void Board::displayBoardUnicode() {
        std::u32string str;
        str.append(U"------------------------\n");
        for(int i = 1; i < m_board.size()+1; i++) {
            str += U'|';
            str += getGlyphUnicode(m_board[i-1]);
            str += U'|';
            if( i%8 == 0 )
                str.append(U"\n------------------------\n");
        }
        //std::cout << std::string(str.begin(), str.end()) << std::endl; 
        std::wstring wstr(str.begin(), str.end());
        std::wcout << wstr << std::endl;
    }

    /*-------------------Testing Functions-------------------*/
    void Board::printMoveOffset() {
        for(int n : m_moveOffset) {
            std::cout << n << '\n';
            std::cout << std::endl;
        }
    }

    void Board::printBool(const bool b) {
        std::string toPrint = "FALSE";
        if(b) {
            toPrint = "TRUE";
        }
        std::cout << "Bool: " << toPrint << std::endl;
    }


    // This is a war crime
    std::string Board::idToString(const ID& id) {
        std::string res;
        switch( id ) {
            case( W_ROOK1 ):
                res = "W_ROOK1";
                break;
            case( W_ROOK2 ):
                res = "W_ROOK2";
                break;
            case( W_KNIGHT1 ):
                res = "W_KNIGHT1";
                break;
            case( W_KNIGHT2 ):
                res = "W_KNIGHT2";
                break;
            case( W_BISHOP1 ):
                res = "W_BISHOP1";
                break;
            case( W_BISHOP2 ):
                res = "W_BISHOP2";
                break;
            case( W_QUEEN ):
                res = "W_QUEEN";
                break; 
            case( W_KING ):
                res = "W_KING";
                break;
            case( W_PAWN1 ):
                res = "W_PAWN1";
                break;
            case( W_PAWN2 ):
                res = "W_PAWN2";
                break;
            case( W_PAWN3 ):
                res = "W_PAWN3";
                break;
            case( W_PAWN4 ):
                res = "W_PAWN4";
                break;
            case( W_PAWN5 ):
                res = "W_PAWN5";
                break;
            case( W_PAWN6 ):
                res = "W_PAWN6";
                break;
            case( W_PAWN7 ):
                res = "W_PAWN7";
                break;
            case( W_PAWN8 ):
                res = "W_PAWN8";
                break;
            case( B_ROOK1   ):
                res = "B_ROOK1";
                break;
            case( B_ROOK2   ):
                res = "B_ROOK2";
                break;
            case( B_KNIGHT1 ):
                res = "B_KNIGHT1";
                break;
            case( B_KNIGHT2 ):
                res = "B_KNIGHT2";
                break;
            case( B_BISHOP1 ):
                res = "B_BISHOP1";
                break;
            case( B_BISHOP2 ):
                res = "B_BISHOP2";
                break;
            case( B_QUEEN   ):
                res = "B_QUEEN";
                break;
            case( B_KING    ):
                res = "B_KING";
                break;
            case( B_PAWN1 ):
                res = "B_PAWN1";
                break;
            case( B_PAWN2 ):
                res = "B_PAWN2";
                break;
            case( B_PAWN3 ):
                res = "B_PAWN3";
                break;
            case( B_PAWN4 ):
                res = "B_PAWN4";
                break;
            case( B_PAWN5 ):
                res = "B_PAWN5";
                break;
            case( B_PAWN6 ):
                res = "B_PAWN6";
                break;
            case( B_PAWN7 ):
                res = "B_PAWN7";
                break;
            case( B_PAWN8 ):
                res = "B_PAWN8";
                break;
            default:
                res = "EMPTY";
                break;
        }
        return res;
    }

    void Board::printCheckedPiece() {
        std::cout << "Checking Piece: " << idToString(m_checked.id) << std::endl;
    }


    void Board::printMoves() {
        int offsetIdx = 0;
        for(int i = 0; i < m_moves.size(); i++) {
            if( offsetIdx < m_moveOffset.size() ) {
                if( i == m_moveOffset[offsetIdx] ) {
                    std::cout << idToString(static_cast<ID>(offsetIdx+1)) << '\n';
                    offsetIdx++;
                }
            }
            for(const int n : m_moves[i]) {
                std::cout << n << ' ';
            }
            std::cout << '\n';
        }
    }

    std::array<ID, 64> Board::genBoardLiteral(std::vector<IdPos>& in) { 
        sortByPos(in);
        std::array<ID, 64> res;
        const int resSize = res.size();
        const int inSize = in.size();
        if( inSize > 64 || inSize == 0) {
            for(int i = 0; i < resSize; i++) {
                res[i] = ID::EMPTY;
            }
        } else {
            int inIdx = 0;
            int nextIdx = posTranslate(in[inIdx].pos);
            for(int i = 0; i < res.size(); i++) {
                if( nextIdx == i ) {
                    res[i] = in[inIdx].id;
                    if( inIdx+1 != in.size() ) {
                        inIdx++;
                        nextIdx = posTranslate(in[inIdx].pos);
                    }
                } else {
                    res[i] = ID::EMPTY;
                }
            }
        }
        return res;
    }

    std::array<Piece, 32> Board::genPieces(std::vector<IdPos>& in) {
        std::array<Piece, 32> res( {
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
        });
        sortById(in); //sort the in vector by ID
        const int resSize = res.size();
        const int inLength = in.size();
        if( inLength > 0 && inLength <= 32 ) {
            int inIdx = 0;
            int nextIdx = (int) in[inIdx].id -1;
            for(int i = 0; i < resSize; i++) {
                if( i == nextIdx ) {
                    res[i].position = in[inIdx].pos;
                    inIdx++;
                    if( inIdx != inLength ) {
                        nextIdx = (int) in[inIdx].id -1;
                    }
                } else {
                    res[i].position = Pos{8,8}; // should hardcode these into res
                }
            }
        } else {
            for(int i = 0; i < resSize; i++) {
                res[i].position = Pos{8,8}; 
            }
        }
        return res;
    }

    void Board::printPieceIdxs() {
        for(Piece p : m_pieceArr) {
            std::cout << p.movesIdx << '\n';
        }
        std::cout << std::endl;
    }

    void Board::printPinnedSet() {
        std::cout << "\nPinned Set: ";
        for( const ID& id : m_pinnedIdSet ) {
            std::cout << idToString(id) << ", ";
        }
        std::cout << std::endl;
    }

    void Board::printPinnedArr() {
        int nextId = 0;
        for( int i = 0; i < m_pinnedArr.size(); i++ ) {
            if( i % 6 == 0 ) {
                std::cout << "ID: " << idToString(static_cast<ID>(nextId+1)) << '\n';
                nextId++;
            }
            printPosition(m_pinnedArr[i]);
        }
    }

    void Board::printPiecesPos(std::array<Piece, 32>& pieceArr) {
        for(int i = 0; i < pieceArr.size() ; ++i) {
            std::cout << idToString(static_cast<ID>(i+1)) << '\n';
            printPosition(pieceArr[i]);
        }
    }

    void Board::printPosition(const Piece& piece) {
        const Pos pos = piece.position;
        std::cout << '\t';
        for( const int& n : pos ) {
            std::cout << n << ' ';
        }
        std::cout << std::endl;
    }

    void Board::printPosition(const Pos& pos) {
        for( const int& n : pos ) {
            std::cout << n << ' ';
        }
        std::cout << std::endl;
    }

    /* Insertion sort implementation to sort a vector of IdPos objects by ID */
    void Board::sortById(std::vector<IdPos>& in) {
        const int n = in.size();
        for (int i = 1; i < n; ++i) {
            IdPos key = in[i];
            int j = i - 1;

            while (j >= 0 && (int)in[j].id > (int)key.id) {
                in[j + 1] = in[j];
                j = j - 1;
            }
            in[j + 1] = key;
        }
    }

    /* Insertion sort implementation to sort a vector of IdPos objects by Pos */
    void Board::sortByPos(std::vector<IdPos>& in) {
        std::vector<int> translated;
        const int inSize = in.size();
        translated.reserve(inSize);
        for(int i = 0; i < inSize; i++) {
            translated.push_back(posTranslate(in[i].pos));
        }
        for (int i = 1; i < inSize; ++i) {
            IdPos keyStruct = in[i];
            int key = translated[i];
            int j = i - 1;

            while (j >= 0 && translated[j] > key) {
                translated[j + 1] = translated[j];
                in[j + 1] = in[j];
                j = j - 1;
            }
            in[j + 1] = keyStruct;
            translated[j + 1] = key;
        }
    }

    void Board::printIdPosVec(std::vector<IdPos>& in) {
        for( const IdPos& current : in ) {
            std::cout << idToString(current.id) << '\n'; 
            printPosition(current.pos);
            std::cout << std::endl;
        }
    }

}
