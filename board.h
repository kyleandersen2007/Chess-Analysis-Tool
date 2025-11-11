#ifndef APSC143__BOARD_H
#define APSC143__BOARD_H

#include <stdio.h>
#include <stdbool.h>
#include "panic.h"

#define BOARD_SIZE 8
#define PRINT_COORD(col, row) ('a' + (col)), ('1' + (8 - (row) - 1))

enum chess_piece
{
    PIECE_PAWN = 0,
    PIECE_KNIGHT = 1,
    PIECE_BISHOP = 2,
    PIECE_ROOK = 3,
    PIECE_QUEEN = 4,
    PIECE_KING = 5,
};
enum chess_player
{
    PLAYER_WHITE = 0,
    PLAYER_BLACK = 1,
};

struct square
{
    bool has_piece;
    enum chess_piece piece;
    enum chess_player owner;
    int col, row;
};

struct castling_rights
{
    bool white_kingside;  // O-O
    bool white_queenside; // O-O-O
    bool black_kingside;
    bool black_queenside;
};

struct chess_board
{
    enum chess_player next_move_player;
    struct square squares[BOARD_SIZE][BOARD_SIZE];
    struct castling_rights rights;
};

struct chess_move
{
    enum chess_player player;
    enum chess_piece piece_type;

    int to_row, to_col;
    int from_row, from_col;

    bool is_capture;
    bool is_promotion;
    enum chess_piece promo_piece;

    bool is_castle;
    bool castle_kingside;
};

// Initializes the state of the board for a new chess game.
void board_initialize(struct chess_board *board);

// Determine which piece is moving, and complete the move data accordingly.
// Panics if there is no piece which can make the specified move, or if there
// are multiple possible pieces.
void board_complete_move(const struct chess_board *board, struct chess_move *move);

// Apply move to the board. The move must already be complete, i.e., the initial
// square must be known. Panics if the move is not legal in the current board
// position.
void board_apply_move(struct chess_board *board, const struct chess_move *move);

// Classify the state of the board, printing one of the following:
// - game incomplete
// - white wins by checkmate
// - black wins by checkmate
// - draw by stalemate
void board_summarize(const struct chess_board *board); // (stub for now)

#endif
