#ifndef CHESS_CORE_GAME_BOARD_HPP
#define CHESS_CORE_GAME_BOARD_HPP

#include <Chess/Core/declarations.hpp>

#include <vector>
#include <array>
#include <string>
#include <set>


namespace Chess {
    class Board {
        public:
            Board();
            Board(const std::array<ID, 64>& board, const std::array<Piece, 32>& pieces);
            ~Board();
            Board& operator=(const Board& rhs);

            Pos& getPos(const ID& id);

            void move(const ID& id, const Pos& pos);

            // Public Method Integration
            // These Methods Are Called By The Singleplayer
            // Client At The Correct Times.
            // Call Order On Every Move Attempts:
            /*
             * if (isValidMove()) {
             *     move();
             *     nextTurn();
             *
             */
            const std::array<ID, 64>& getBoard() const;
            bool isValidMove(const ID& id, const Pos& target);
            bool getTurn() const;
            static int posTranslate(const Pos& pos);
            static std::array<Piece, 32> genPieces( std::vector<IdPos>& in);
            static std::array<ID, 64> genBoardLiteral(std::vector<IdPos>& in);
            static std::string idToString(const ID& id);

            void setTurn(bool isWhite);
            void nextTurn();
            void genMoves();
            void genChecked();
            void genPinned();

            std::ios &(* isValidMove())(std::ios &);

            COLOR winner();
            /* Testing functions */
            void printMoveOffset();
            void printMoves();
            void displayBoard();
            static void printBoard(const std::array<ID, 64>&);
            void displayBoardUnicode();
            void printPieceIdxs();
            void printPinnedArr();
            void printPinnedSet();
            void printCheckedPiece();
            static void printBool(const bool b);
            static void printColor(const COLOR& color);
            static void printPiecesPos(std::array<Piece, 32>& pieceArr);
            static void printId(const ID& id);
            static void printPosition(const Piece& piece);
            static void printPosition(const Pos& pos);
            static void sortById(std::vector<IdPos>& in);
            static void sortByPos(std::vector<IdPos>& in);
            static void printIdPosVec(std::vector<IdPos>& in);
            bool isMatchingType(const ID& id, const Type type);


        private:
            std::array<ID, 64> m_board;
            //Piece position is indexed at m_pieces[Piece-1]
            std::array<Piece, 32> m_pieceArr;
            std::array<Pos, MAX_MOVES> m_moves;
            std::array<int, 32> m_moveOffset;

            std::vector<bool> m_defended;
            std::vector<bool> m_attackedWhite;
            std::vector<bool> m_attackedBlack;
            std::set<ID> m_pinnedIdSet;
            // the valid moves for a pinned piece are now stored here,
            //  since there may only be maximum 6 valid moves for a piece
            std::array<Pos, 32*6> m_pinnedArr;

            std::array<std::pair<int,int>, 8> m_mods;
            std::array<std::pair<int,int>, 2> m_pawnMods;

            const std::set<Type> m_diagonalSet;
            const std::set<Type> m_cardinalSet;

            bool m_isWhiteTurn;
            bool m_isWhiteChecked;
            bool m_isBlackChecked;
            CriticalPiece m_checked;

            bool isInBoard(const Pos& pos);
            ID& getIdAt(const Pos& pos);
            ID getKingId();
            Type getTypeAt(const Pos& pos);
            COLOR getColor(const ID& id);
            static std::string colorToString(const COLOR& color);
            static std::string typeToString(const Type& type);
            int getPawnRow(const COLOR& color);
            std::pair<int,int> getMod(const Direction& direction);
            Direction directionCast(const int& i);
            std::vector<bool>& getAttackedVec();
            bool isAttackedAt(const Pos& pos);
            bool isDefendedAt(const Pos& pos);
            bool isValidMod(const int& val, const int& mod);
            std::uint8_t castHelper(const std::uint8_t dim, const int& mod);
            const std::set<Type>* getMatchingSet(const Direction& direction);

            void setIdAt(const Pos& pos, const ID& id);
            void setMoveAt(const ID& id, const Pos& pos, const int& i);
            void setMovesIdx(const ID& id, const int& i);
            void setAttackedAt(const Pos& pos);
            void setDefendedAt(const Pos& pos);
            void setChecked(const ID& checked, const Direction direction);

            void diagonalHelper(const Pos& initial, const Direction& direction, int& i);
            void addDiagonalMoves(const Pos& initial);
            void cardinalHelper(const Pos& initial, const Direction& direction, int& i);
            void addCardinalMoves(const Pos& initial);
            void addKnightMoves(const Pos& initial);
            void addKingMoves(const Pos& initial);
            void addQueenMoves(const Pos& initial);
            void addPawnMoves(const Pos& initial);

            char32_t getGlyphUnicode(const ID& id);
            char getGlyph(const ID& id);

    };
}
#endif
