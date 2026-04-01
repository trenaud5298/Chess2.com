#ifndef CHESS_CORE_GAME_BOARD_HPP
#define CHESS_CORE_GAME_BOARD_HPP

#include <Chess/Core/declarations.hpp>

#include <vector>
#include <array>
#include <complex.h>
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
            void setTurn(bool isWhite);
            bool getTurn() const;
            void nextTurn();

            std::ios &(* isValidMove())(std::ios &);

        private:
            std::array<ID, 64> m_board;
            //Piece position is indexed at m_pieces[Piece-1]
            std::array<Piece, 32> m_pieceArr;
            std::array<Pos, MAX_MOVES> m_moves;
            std::array<int, 32> m_moveOffset;

            std::vector<bool> m_defended;
            std::vector<bool> m_attackedWhite;
            std::vector<bool> m_attackedBlack;
            std::vector<CriticalPiece> m_pinnedVec;
            std::set<ID> m_pinnedIdSet;

            std::array<std::pair<int,int>, 8> m_mods;
            std::array<std::pair<int,int>, 2> m_pawnMods;

            const std::set<Type> m_diagonalSet;
            const std::set<Type> m_cardinalSet;

            bool m_isWhiteTurn;
            bool m_isWhiteChecked;
            bool m_isBlackChecked;
            CriticalPiece m_checked;

            bool isInBoard(const Pos& pos);
            int posTranslate(const Pos& pos);
            ID& getIdAt(const Pos& pos);
            ID getKingId();
            Type getTypeAt(const Pos& pos);
            COLOR getColor(const ID& id);
            int getPawnRow();
            std::pair<int,int> getMod(const Direction& direction);
            Direction directionCast(const int& i);
            std::vector<bool>& getAttackedVec();
            bool isAttackedAt(const Pos& pos);
            bool isDefendedAt(const Pos& pos);
            std::uint8_t castHelper(const std::uint8_t dim, const int& mod);
            const std::set<Type>* getMatchingSet(const Direction& direction);

            void setIdAt(const Pos& pos, const ID& id);
            void setMoveAt(const ID& id, const Pos& pos, const int& i);
            void setMovesIdx(const ID& id, const int& i);
            void setAttackedAt(const Pos& pos);
            void setDefendedAt(const Pos& pos);
            void setChecked(const ID& id, const Direction direction);

            void genChecked();
            void genPinned();
            void diagonalHelper(const Pos& initial, const Direction& direction, int& i);
            void addDiagonalMoves(const Pos& initial);
            void cardinalHelper(const Pos& initial, const Direction& direction, int& i);
            void addCardinalMoves(const Pos& initial);
            void addKnightMoves(const Pos& initial);
            void addKingMoves(const Pos& initial);
            void addQueenMoves(const Pos& initial);
            void addPawnMoves(const Pos& initial);
            void genMoves();

            char32_t getGlyph(const ID& id);
            void displayBoard();
    };
}
#endif
