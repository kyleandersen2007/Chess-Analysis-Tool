#ifndef APSC143__BOARD_H
#define APSC143__BOARD_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "panic.h"

#define BOARD_SIZE 8

enum chess_player
{
    PLAYER_WHITE = 0,
    PLAYER_BLACK = 1,
};

// Gets a lowercase string denoting the player.
const char *player_string(enum chess_player player);

enum chess_piece
{
    PIECE_PAWN = 0,
    PIECE_KNIGHT = 1,
    PIECE_BISHOP = 2,
    PIECE_ROOK = 3,
    PIECE_QUEEN = 4,
    PIECE_KING = 5,
};

// Gets a lowercase string denoting the piece type.
const char *piece_string(enum chess_piece piece);

struct square
{
    bool has_piece;
    enum chess_piece piece;
    enum chess_player owner;
};

#define FILE_TO_COL(file) ((file) - 'a')

#define RANK_TO_ROW(rank) (8 - ((rank) - '0'))

#define SQUARE_TO_COORD(square, row, col) \
    do                                    \
    {                                     \
        (col) = FILE_TO_COL((square)[0]); \
        (row) = RANK_TO_ROW((square)[1]); \
    } while (0)

struct castling_rights {
    bool white_kingside;   // O-O
    bool white_queenside;  // O-O-O
    bool black_kingside;
    bool black_queenside;
};

struct chess_board
{
    enum chess_player next_move_player;
    struct square squares[BOARD_SIZE][BOARD_SIZE];
    struct castling_rights rights;
    int ep_row, ep_col; // used for en passant
};

struct chess_move
{
    enum chess_player player;
    enum chess_piece piece_type;

    int8_t to_row, to_col;
    int8_t from_row, from_col;

    bool is_capture;
    bool is_en_passant;

    bool is_promotion;
    enum chess_piece promo_piece;

    bool is_castle;
    bool castle_kingside;

    int8_t hint_from_row;
    int8_t hint_from_col;
};

static inline int absolute_value_int(int value) { return (value < 0) ? -value : value; }

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
