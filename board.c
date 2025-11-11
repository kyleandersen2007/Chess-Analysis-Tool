#include "board.h"
#include <stdio.h>

// yo we gotta use static so we can internally link the functions

static inline int absint(int v) { return (v < 0) ? -v : v; }

static inline bool straight_check(const struct chess_board *board, int from_row_index, int from_column_index, int to_row_index, int to_column_index)
{
    if (from_row_index == to_row_index && from_column_index == to_column_index)
    {
        return false;
    }

    if (from_row_index == to_row_index)
    {
        int step = (to_column_index > from_column_index) ? 1 : -1;
        for (int column = from_column_index + step; column != to_column_index; column += step)
        {
            if (board->squares[from_row_index][column].has_piece)
            {
                return false;
            }
        }
        return true;
    }
    if (from_column_index == to_column_index)
    {
        int step = (to_row_index > from_row_index) ? 1 : -1;
        for (int row = from_row_index + step; row != to_row_index; row += step)
        {
            if (board->squares[row][from_column_index].has_piece)
            {
                return false;
            }
        }
        return true;
    }
    return false;
}

static inline bool diag_check(const struct chess_board *board, int from_row_index, int from_column_index, int to_row_index, int to_column_index)
{
    int delta_row = to_row_index - from_row_index;
    int delta_col = to_column_index - from_column_index;
    int abs_delta_row = absint(delta_row);
    int abs_delta_col = absint(delta_col);

    if (abs_delta_row == 0 && abs_delta_col == 0)
    {
        return false;
    }
    if (abs_delta_row == 0 || abs_delta_col == 0)
    {
        return false;
    }
    if (abs_delta_row != abs_delta_col)
    {
        return false;
    }

    int row_step = (delta_row > 0) ? 1 : -1;
    int column_step = (delta_col > 0) ? 1 : -1;
    int row = from_row_index + row_step;
    int column = from_column_index + column_step;

    while (row != to_row_index)
    {
        if (board->squares[row][column].has_piece)
        {
            return false;
        }
        row += row_step;
        column += column_step;
    }

    return true;
}

static inline bool pawn_reach(const struct chess_board *board, int from_row_index, int from_column_index, int to_row_index, int to_column_index, enum chess_player mover)
{
    int forward_direction = (mover == PLAYER_WHITE) ? -1 : +1;
    int start_row_index = (mover == PLAYER_WHITE) ? 6 : 1;

    int delta_row = to_row_index - from_row_index;
    int delta_col = to_column_index - from_column_index;

    if (delta_row == forward_direction && (delta_col == 1 || delta_col == -1))
    {
        const struct square *destination = &board->squares[to_row_index][to_column_index];
        return destination->has_piece && destination->owner != mover;
    }
    if (delta_col == 0 && delta_row == forward_direction)
        return !board->squares[to_row_index][to_column_index].has_piece;

    if (delta_col == 0 && delta_row == 2 * forward_direction && from_row_index == start_row_index)
    {
        int intermediate_row = from_row_index + forward_direction;
        return !board->squares[intermediate_row][from_column_index].has_piece && !board->squares[to_row_index][to_column_index].has_piece;
    }

    return false;
}

const char *player_string(enum chess_player player)
{
    switch (player)
    {
    case PLAYER_WHITE:
        return "white";
    case PLAYER_BLACK:
        return "black";
    }
    return "unknown";
}

const char *piece_string(enum chess_piece piece)
{
    switch (piece)
    {
    case PIECE_PAWN:
        return "pawn";
    case PIECE_KNIGHT:
        return "knight";
    case PIECE_BISHOP:
        return "bishop";
    case PIECE_ROOK:
        return "rook";
    case PIECE_QUEEN:
        return "queen";
    case PIECE_KING:
        return "king";
    }
    return "unknown";
}

void board_initialize(struct chess_board *board)
{
    for (int row = 0; row < BOARD_SIZE; row++)
    {
        for (int col = 0; col < BOARD_SIZE; col++)
        {
            board->squares[row][col].has_piece = false;
            board->squares[row][col].piece = PIECE_PAWN;
            board->squares[row][col].owner = PLAYER_WHITE;
        }
    }

    board->squares[0][0] = (struct square){true, PIECE_ROOK, PLAYER_BLACK};
    board->squares[0][1] = (struct square){true, PIECE_KNIGHT, PLAYER_BLACK};
    board->squares[0][2] = (struct square){true, PIECE_BISHOP, PLAYER_BLACK};
    board->squares[0][3] = (struct square){true, PIECE_QUEEN, PLAYER_BLACK};
    board->squares[0][4] = (struct square){true, PIECE_KING, PLAYER_BLACK};
    board->squares[0][5] = (struct square){true, PIECE_BISHOP, PLAYER_BLACK};
    board->squares[0][6] = (struct square){true, PIECE_KNIGHT, PLAYER_BLACK};
    board->squares[0][7] = (struct square){true, PIECE_ROOK, PLAYER_BLACK};

    for (int col = 0; col < 8; col++)
        board->squares[1][col] = (struct square){true, PIECE_PAWN, PLAYER_BLACK};

    for (int col = 0; col < 8; col++)
        board->squares[6][col] = (struct square){true, PIECE_PAWN, PLAYER_WHITE};

    board->squares[7][0] = (struct square){true, PIECE_ROOK, PLAYER_WHITE};
    board->squares[7][1] = (struct square){true, PIECE_KNIGHT, PLAYER_WHITE};
    board->squares[7][2] = (struct square){true, PIECE_BISHOP, PLAYER_WHITE};
    board->squares[7][3] = (struct square){true, PIECE_QUEEN, PLAYER_WHITE};
    board->squares[7][4] = (struct square){true, PIECE_KING, PLAYER_WHITE};
    board->squares[7][5] = (struct square){true, PIECE_BISHOP, PLAYER_WHITE};
    board->squares[7][6] = (struct square){true, PIECE_KNIGHT, PLAYER_WHITE};
    board->squares[7][7] = (struct square){true, PIECE_ROOK, PLAYER_WHITE};

    board->next_move_player = PLAYER_WHITE;

    board->rights.white_kingside = true;
    board->rights.white_queenside = true;
    board->rights.black_kingside = true;
    board->rights.black_queenside = true;

    board->ep_row = -1;
    board->ep_col = -1;
}

void board_complete_move(const struct chess_board *board, struct chess_move *move)
{
    move->player = board->next_move_player;

    if (move->to_row < 0 || move->to_row >= BOARD_SIZE || move->to_col < 0 || move->to_col >= BOARD_SIZE)
    {
        panicf("illegal move: destination out of bounds\n");
    }
    if (board->squares[move->to_row][move->to_col].has_piece &&
        board->squares[move->to_row][move->to_col].owner == move->player)
    {
        panicf("illegal move: destination occupied by own piece\n");
    }

    int candidate_from_rows[16];
    int candidate_from_cols[16];
    int candidate_count = 0;

    for (int from_row = 0; from_row < BOARD_SIZE; ++from_row)
    {
        for (int from_col = 0; from_col < BOARD_SIZE; ++from_col)
        {
            const struct square *source_square = &board->squares[from_row][from_col];
            if (!source_square->has_piece)
                continue;
            if (source_square->owner != move->player)
                continue;
            if (source_square->piece != move->piece_type)
                continue;

            if (move->hint_from_row >= 0 && from_row != move->hint_from_row)
                continue;
            if (move->hint_from_col >= 0 && from_col != move->hint_from_col)
                continue;

            int delta_row = move->to_row - from_row;
            int delta_col = move->to_col - from_col;
            int absolute_delta_row = absint(delta_row);
            int absolute_delta_col = absint(delta_col);

            bool can_reach = false;

            switch (source_square->piece)
            {
            case PIECE_PAWN:
                can_reach = pawn_reach(board, from_row, from_col, move->to_row, move->to_col, move->player);
                break;

            case PIECE_KNIGHT:
                can_reach = (absolute_delta_row == 2 && absolute_delta_col == 1) || (absolute_delta_row == 1 && absolute_delta_col == 2);
                break;

            case PIECE_BISHOP:
                if (absolute_delta_row == absolute_delta_col && absolute_delta_row != 0)
                {
                    can_reach = diag_check(board, from_row, from_col, move->to_row, move->to_col);
                }
                break;

            case PIECE_ROOK:
                if ((delta_row == 0 && delta_col != 0) || (delta_col == 0 && delta_row != 0))
                {
                    can_reach = straight_check(board, from_row, from_col, move->to_row, move->to_col);
                }
                break;

            case PIECE_QUEEN:
                can_reach = straight_check(board, from_row, from_col, move->to_row, move->to_col) || diag_check(board, from_row, from_col, move->to_row, move->to_col);
                break;

            case PIECE_KING:
                can_reach = (absolute_delta_row <= 1 && absolute_delta_col <= 1 && (absolute_delta_row + absolute_delta_col) > 0);
                break;
            }

            if (!can_reach)
                continue;

            if (candidate_count < (int)(sizeof(candidate_from_rows) / sizeof(candidate_from_rows[0])))
            {
                candidate_from_rows[candidate_count] = from_row;
                candidate_from_cols[candidate_count] = from_col;
                candidate_count++;
            }
        }
    }

    if (candidate_count == 0)
    {
        panicf("illegal move: no %s can reach the destination\n", piece_string(move->piece_type));
    }
    if (candidate_count > 1)
    {
        panicf("parse error: ambiguous move; provide from-row/from-col hints\n");
    }

    move->from_row = candidate_from_rows[0];
    move->from_col = candidate_from_cols[0];
    move->is_capture = board->squares[move->to_row][move->to_col].has_piece && board->squares[move->to_row][move->to_col].owner != move->player;
}

void board_apply_move(struct chess_board *board, const struct chess_move *move)
{
    if (move->from_row < 0 || move->from_row >= BOARD_SIZE || move->from_col < 0 || move->from_col >= BOARD_SIZE || move->to_row < 0 || move->to_row >= BOARD_SIZE || move->to_col < 0 || move->to_col >= BOARD_SIZE)
    {
        panicf("illegal move: source or destination out of bounds\n");
    }

    struct square *src = &board->squares[move->from_row][move->from_col];
    struct square *dst = &board->squares[move->to_row][move->to_col];

    if (!src->has_piece || src->owner != move->player || src->piece != move->piece_type)
    {
        panicf("illegal move: source square does not contain the expected piece\n");
    }

    *dst = *src;
    src->has_piece = false;

    board->next_move_player = (board->next_move_player == PLAYER_WHITE) ? PLAYER_BLACK : PLAYER_WHITE;

    board->ep_row = -1;
    board->ep_col = -1;
}

void board_summarize(const struct chess_board *board)
{
}