#ifndef CHESS_CORE_DECLARATIONS_HPP
#define CHESS_CORE_DECLARATIONS_HPP

#include <array>
#include <cstdint>

#define WHITE_BOUND 16
#define BLACK_BOUND 17

#define ROW 0
#define COL 1
#define ROW_LOWER_BOUND -1
#define ROW_UPPER_BOUND 8
#define COL_LOWER_BOUND -1
#define COL_UPPER_BOUND 8
#define RIGHT 1
#define LEFT -1
#define WHITE_PAWN_ROW 1
#define BLACK_PAWN_ROW 6

#define MAX_MOVES_PAWN 3
#define MAX_MOVES_KNIGHT 8
#define MAX_MOVES_BISHOP 13
#define MAX_MOVES_ROOK 14
#define MAX_MOVES_QUEEN 27
#define MAX_MOVES_KING 8
#define MAX_PINNED 8
#define MAX_MOVES (3 + 8 + 13 + 14 + 27 + 8)

// Pos is {row, col} or {a-f(0-7), 0-7}
using Pos = std::array<std::uint8_t, 2>;


enum class Type : std::uint8_t{
    W_ROOK = 1,
    W_KNIGHT = 2,
    W_BISHOP = 3,
    W_QUEEN = 4,
    W_KING = 5,
    B_KNIGHT = 6,
    B_BISHOP = 7,
    B_ROOK = 8,
    B_QUEEN = 9,
    B_KING = 10,
    B_PAWN = 11,
    W_PAWN = 12,
};

enum class COLOR : std::uint8_t {
    EMPTY = 0,
    WHITE = 1,
    BLACK = 2,
};

enum class ID : std::uint8_t {
    EMPTY = 0,
    W_ROOK1 = 1,
    W_ROOK2 = 2,
    W_KNIGHT1 = 3,
    W_KNIGHT2 = 4,
    W_BISHOP1 = 5,
    W_BISHOP2 = 6,
    W_QUEEN = 7,
    W_KING = 8,
    W_PAWN1 = 9,
    W_PAWN2 = 10,
    W_PAWN3 = 11,
    W_PAWN4 = 12,
    W_PAWN5 = 13,
    W_PAWN6 = 14,
    W_PAWN7 = 15,
    W_PAWN8 = 16,
    B_KNIGHT1 = 17,
    B_KNIGHT2 = 18,
    B_BISHOP1 = 19,
    B_BISHOP2 = 20,
    B_ROOK1 = 21,
    B_ROOK2 = 22,
    B_QUEEN = 23,
    B_KING = 24,
    B_PAWN1 = 25,
    B_PAWN2 = 26,
    B_PAWN3 = 27,
    B_PAWN4 = 28,
    B_PAWN5 = 29,
    B_PAWN6 = 30,
    B_PAWN7 = 31,
    B_PAWN8 = 32
};

/* NOTE: since the m_board is flipped vertically, NORTH and SOUTH are inverted
 * NOTE: any time you see cardinal, it means the 4 cardinal directions */
enum class Direction {
    NORTH = 1,
    EAST = 2,
    SOUTH = -1,
    WEST = -2,
    NORTHEAST = 3,
    SOUTHEAST = 4,
    SOUTHWEST= -3,
    NORTHWEST = -4
};

struct Piece {
    Pos position;
    Type type;
    int reserved;
    int movesIdx;
};

struct CriticalPiece {
    ID id;
    Direction direction;
};

#endif
