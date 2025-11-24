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

//stupid helper function because we can't use abs
int get_absolute_value(int value);

const char *piece_string(enum chess_piece piece);
const char *player_string(enum chess_player player);

void board_initialize(struct chess_board *board);
void board_complete_move(const struct chess_board *board, struct chess_move *move);
void board_apply_move(struct chess_board *board, const struct chess_move *move);
void board_summarize(const struct chess_board *board);

bool board_in_check(const struct chess_board *board, enum chess_player player);
bool board_in_checkmate(const struct chess_board *board, enum chess_player player);
bool board_can_pawn_reach(const struct chess_board *board, int from_row, int from_col, int to_row, int to_col, enum chess_player player);
bool board_diagonal_check(const struct chess_board *board, int from_row, int from_col, int to_row, int to_col);
bool board_straight_check(const struct chess_board *board, int from_row, int from_col, int to_row, int to_col);
bool board_can_castle(const struct chess_board *board, enum chess_player player, bool kingside);
bool board_in_stalemate(const struct chess_board * board, enum chess_player player);
bool board_is_legal_move(const struct chess_board *board, int from_row, int from_col, int to_row, int to_col);
bool board_recommend_move(const struct chess_board *board, struct chess_move *best_move);

#endif