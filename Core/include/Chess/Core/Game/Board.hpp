#ifndef CHESS_CORE_GAME_BOARD_HPP
#define CHESS_CORE_GAME_BOARD_HPP

#include <Chess/Core/declarations.hpp>

#include <vector>
#include <array>


namespace Chess {
    class Board {
        public:
            Board();
            Board(const std::array<ID, 64>& board, const std::array<Piece, 32>& pieces);
            ~Board();
            Board& operator=(const Board& rhs);

            Pos& getPos(const ID& id);
            std::vector<Pos> getPossibleMoves(const ID& piece);

            void move(const ID& id, const Pos& pos);
        private:
            std::array<ID, 64> m_board;
            //Piece position is indexed at m_pieces[Piece-1]
            std::array<Piece, 32> m_pieces;

            bool isInBoard(const Pos& pos);
            int posTranslate(const Pos& pos);
            ID& getIdAt(const Pos& pos);
            COLOR getColor(const ID& id);
            void setIdAt(const Pos& pos, const ID& id);

            void diagonalHelper(std::vector<Pos>& moves, const Pos& initial, const Direction& direction);
            void addDiagonalMoves(std::vector<Pos>& moves, const Pos& initial);
            void cardinalHelper(std::vector<Pos>& moves, const Pos& pos, Direction direction);
            void addCardinalMoves(std::vector<Pos>& moves, const Pos&);
            void addKnightMoves(std::vector<Pos>& moves, const Pos& initial);
            void addKingMoves(std::vector<Pos>& moves, const Pos&);
            void addPawnMoves(std::vector<Pos>& moves, const Pos& initial, const ID& id);
    };
}
#endif
