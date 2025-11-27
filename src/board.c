#include "board.h"
#include <stdio.h>

const char *player_string(enum chess_player player)
{
    switch (player)
    {
    case PLAYER_WHITE:
        return "white";
    case PLAYER_BLACK:
        return "black";
    }
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

const char *unicode_piece(enum chess_piece piece, enum chess_player owner)
{
    if (owner == PLAYER_WHITE)
    {
        switch (piece)
        {
        case PIECE_KING:
            return "♔";
        case PIECE_QUEEN:
            return "♕";
        case PIECE_ROOK:
            return "♖";
        case PIECE_BISHOP:
            return "♗";
        case PIECE_KNIGHT:
            return "♘";
        case PIECE_PAWN:
            return "♙";
        }
    }
    else
    {
        switch (piece)
        {
        case PIECE_KING:
            return "♚";
        case PIECE_QUEEN:
            return "♛";
        case PIECE_ROOK:
            return "♜";
        case PIECE_BISHOP:
            return "♝";
        case PIECE_KNIGHT:
            return "♞";
        case PIECE_PAWN:
            return "♟";
        }
    }
    return ".";
}

int get_absolute_value(int value)
{
    return (value < 0) ? -value : value;
}

bool board_straight_check(const struct chess_board *board, int from_row, int from_col, int to_row, int to_col)
{
    if (from_row == to_row && from_col == to_col) // no movement
    {
        return false;
    }

    if (from_row == to_row) // horizontal movement (row)
    {
        int step = (to_col > from_col) ? 1 : -1; // determine direction (variable to increment and or decrement by)
        int column = from_col + step;            // start checking from the next column

        while (column != to_col)
        {
            if (board->squares[from_row][column].has_piece) // if there is a piece in the way, return false
            {
                return false;
            }
            column += step; // move to the next column
        }

        return true;
    }

    // same logic as above
    if (from_col == to_col) // vertical movement (col)
    {
        int step = (to_row > from_row) ? 1 : -1;
        int row = from_row + step;

        while (row != to_row)
        {
            if (board->squares[row][from_col].has_piece)
            {
                return false;
            }
            row += step;
        }

        return true;
    }
    return false;
}

bool board_diagonal_check(const struct chess_board *board, int from_row, int from_col, int to_row, int to_col)
{
    // get the changes in rows and columns as well as their absolute values
    int delta_row = to_row - from_row;
    int delta_col = to_col - from_col;
    int abs_delta_row = get_absolute_value(delta_row);
    int abs_delta_col = get_absolute_value(delta_col);

    if (abs_delta_row == 0 && abs_delta_col == 0) // no movement
    {
        return false;
    }
    if (abs_delta_row == 0 || abs_delta_col == 0) // not diagonal movement
    {
        return false;
    }
    if (abs_delta_row != abs_delta_col) // not diagonal movement
    {
        return false;
    }

    int row_step = (delta_row > 0) ? 1 : -1;    // determine direction (variable to increment and or decrement by)
    int column_step = (delta_col > 0) ? 1 : -1; // determine direction (variable to increment and or decrement by)
    int row = from_row + row_step;              // variable to track our current row
    int column = from_col + column_step;        // variable to track our current column

    while (row != to_row)
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

bool board_can_pawn_reach(const enum chess_player player, const struct chess_board *board, int from_row, int from_col, int to_row, int to_col)
{
    int forward_direction = (player == PLAYER_WHITE) ? -1 : +1;
    int start_row_index = (player == PLAYER_WHITE) ? 6 : 1;

    int delta_row = to_row - from_row;
    int delta_col = to_col - from_col;

    if (delta_row == forward_direction && (delta_col == 1 || delta_col == -1)) // this move is only legal if its a diagonal capture
    {
        const struct square *destination = &board->squares[to_row][to_col];
        return destination->has_piece && destination->owner != player;
    }
    if (delta_col == 0 && delta_row == forward_direction) // normal forward move by one
    {
        return !board->squares[to_row][to_col].has_piece; // only legal if the destination square is empty
    }

    if (delta_col == 0 && delta_row == 2 * forward_direction && from_row == start_row_index)
    {
        int intermediate_row = from_row + forward_direction;                                                   // the row the pawn jumps over (only used for the initial move)
        return !board->squares[intermediate_row][from_col].has_piece && !board->squares[to_row][to_col].has_piece; // both squares must be empty
    }

    return false;
}

bool board_is_legal_move(const struct chess_board *board, int from_row, int from_col, int to_row, int to_col)
{
    if (to_row < 0 || to_row >= BOARD_SIZE || to_col < 0 || to_col >= BOARD_SIZE) // are we in bounds
    {
        return false;
    }

    const struct square *src = &board->squares[from_row][from_col]; // represent our source square

    if (!src->has_piece) // if we are trying to move from a square with no piece, return false
    {
        return false;
    }

    enum chess_player player = src->owner; // get the player: REPETITIVE
    enum chess_piece piece = src->piece;   // what is the piece on the square

    const struct square *dst = &board->squares[to_row][to_col]; // where do we want to move to?
    if (dst->has_piece && dst->owner == player)                 // if our destination has our OWN piece, we can't move there.
    {
        return false;
    }

    int delta_row = to_row - from_row; // get the change in rows
    int delta_col = to_col - from_col; // get the change in columns
    // magnitude of change in rows/cols
    int absolute_delta_row = get_absolute_value(delta_row);
    int absolute_delta_col = get_absolute_value(delta_col);

    if (absolute_delta_row == 0 && absolute_delta_col == 0) // possible edge case: trying to move to the current square
    {
        return false; // no “move”
    }

    bool is_legal = false; // use this flag to keep track if the move is still legal

    switch (piece)
    {
    case PIECE_PAWN:
        is_legal = board_can_pawn_reach(player, board, from_row, from_col, to_row, to_col);
        break;

    case PIECE_KNIGHT:
        is_legal = (absolute_delta_row == 2 && absolute_delta_col == 1) || (absolute_delta_row == 1 && absolute_delta_col == 2); // L pattern check
        break;

    case PIECE_BISHOP:
        if (absolute_delta_row == absolute_delta_col && absolute_delta_row != 0) // if the length of the col "vector" is the same length as the row "vector", its a legal move
        {
            is_legal = board_diagonal_check(board, from_row, from_col, to_row, to_col);
        }
        break;

    case PIECE_ROOK:
        if ((delta_row == 0 && delta_col != 0) || (delta_col == 0 && delta_row != 0)) // as long as there IS movement happening in one or the other directions but NOT both
        {
            is_legal = board_straight_check(board, from_row, from_col, to_row, to_col);
        }
        break;

    case PIECE_QUEEN:
        is_legal = board_straight_check(board, from_row, from_col, to_row, to_col) || board_diagonal_check(board, from_row, from_col, to_row, to_col); // the queen can do both!
        break;

    case PIECE_KING:
        is_legal = (absolute_delta_row <= 1 && absolute_delta_col <= 1 && (absolute_delta_row + absolute_delta_col) > 0); // as long as the change in movement is 1
        break;
    }

    return is_legal;
}

bool board_in_check(const struct chess_board *board)
{
    for (int row = 0; row < BOARD_SIZE; row++)
    {
        for (int col = 0; col < BOARD_SIZE; col++)
        {
            if (board->squares[row][col].has_piece && board->squares[row][col].owner == board->next_move_player && board->squares[row][col].piece == PIECE_KING) // if the square holds the current player's king
            {
                // store the king's position
                int king_row = row;
                int king_col = col;

                for (int from_row = 0; from_row < BOARD_SIZE; from_row++)
                {
                    for (int from_col = 0; from_col < BOARD_SIZE; from_col++)
                    {
                        const struct square *source_square = &board->squares[from_row][from_col]; // get the current square we are checking
                        if (!source_square->has_piece)                                            // skip empty squares
                        {
                            continue;
                        }
                        if (source_square->owner == board->next_move_player) // skip all of our own pieces THUS we are checking all of the opponent's pieces to see if they can legally "take" our king
                        {
                            continue;
                        }
                        if (board_is_legal_move(board, from_row, from_col, king_row, king_col)) // if any opponent piece can legally move to the king's square, we are in check
                        {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool board_in_checkmate(const struct chess_board *board)
{
    if (!board_in_check(board))
    {
        return false;
    }

    for (int from_row = 0; from_row < BOARD_SIZE; from_row++)
    {
        for (int from_col = 0; from_col < BOARD_SIZE; from_col++)
        {
            const struct square *src = &board->squares[from_row][from_col];

            if (!src->has_piece || src->owner != board->next_move_player)
            {
                continue;
            }

            for (int to_row = 0; to_row < BOARD_SIZE; to_row++)
            {
                for (int to_col = 0; to_col < BOARD_SIZE; to_col++)
                {
                    const struct square *dst = &board->squares[to_row][to_col];

                    if (dst->has_piece && dst->owner == board->next_move_player)
                    {
                        continue;
                    }

                    if (!board_is_legal_move(board, from_row, from_col, to_row, to_col))
                    {
                        continue;
                    }

                    struct chess_move test_move = {0};

                    test_move.player = board->next_move_player;
                    test_move.piece_type = src->piece;
                    test_move.from_row = from_row;
                    test_move.from_col = from_col;
                    test_move.to_row = to_row;
                    test_move.to_col = to_col;
                    test_move.is_capture = (dst->has_piece && dst->owner != board->next_move_player);
                    test_move.is_promotion = false;

                    if (test_move.piece_type == PIECE_PAWN)
                    {
                        if ((board->next_move_player == PLAYER_WHITE && to_row == 0) ||
                            (board->next_move_player == PLAYER_BLACK && to_row == 7))
                        {
                            test_move.is_promotion = true;
                            test_move.promo_piece = PIECE_QUEEN;
                        }
                    }

                    struct chess_board test = *board;
                    board_apply_move(&test, &test_move);

                    test.next_move_player = test_move.player;

                    if (!board_in_check(&test))
                    {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

bool board_in_stalemate(const struct chess_board *board)
{
    if (board_in_check(board))
    {
        return false;
    }
    for (int from_row = 0; from_row < BOARD_SIZE; from_row++)
    {
        for (int from_col = 0; from_col < BOARD_SIZE; from_col++)
        {
            const struct square *src = &board->squares[from_row][from_col];
            if (!src->has_piece || src->owner != board->next_move_player)
            {
                continue;
            }

            for (int to_row = 0; to_row < BOARD_SIZE; to_row++)
            {
                for (int to_col = 0; to_col < BOARD_SIZE; to_col++)
                {
                    const struct square *dst = &board->squares[to_row][to_col];
                    if (dst->has_piece && dst->owner == board->next_move_player)
                    {
                        continue;
                    }

                    if (!board_is_legal_move(board, from_row, from_col, to_row, to_col))
                    {
                        continue;
                    }

                    struct chess_move test_move = {0};

                    test_move.player = board->next_move_player;
                    test_move.piece_type = src->piece;
                    test_move.from_row = from_row;
                    test_move.from_col = from_col;
                    test_move.to_row = to_row;
                    test_move.to_col = to_col;
                    test_move.is_capture = (dst->has_piece && dst->owner != board->next_move_player);
                    test_move.is_promotion = false;

                    if (test_move.piece_type == PIECE_PAWN)
                    {
                        if ((board->next_move_player == PLAYER_WHITE && to_row == 0) ||
                            (board->next_move_player == PLAYER_BLACK && to_row == 7))
                        {
                            test_move.is_promotion = true;
                            test_move.promo_piece = PIECE_QUEEN;
                        }
                    }

                    struct chess_board test = *board;
                    board_apply_move(&test, &test_move);
                    test.next_move_player = test_move.player;

                    if (!board_in_check(&test))
                    {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

bool board_can_castle(const struct chess_board *board, bool kingside)
{
    int row = (board->next_move_player == PLAYER_WHITE) ? 7 : 0;
    if (board->next_move_player == PLAYER_WHITE)
    {
        if (kingside && !board->rights.white_kingside)
            return false;
        if (!kingside && !board->rights.white_queenside)
            return false;
    }
    else
    {
        if (kingside && !board->rights.black_kingside)
            return false;
        if (!kingside && !board->rights.black_queenside)
            return false;
    }

    int king_from_col = 4;
    int rook_col = kingside ? 7 : 0;
    int king_to_col = kingside ? 6 : 2;

    const struct square *king_square = &board->squares[row][king_from_col];
    const struct square *rook_square = &board->squares[row][rook_col];

    if (!king_square->has_piece ||
        king_square->owner != board->next_move_player ||
        king_square->piece != PIECE_KING)
    {
        return false;
    }

    if (!rook_square->has_piece || rook_square->owner != board->next_move_player || rook_square->piece != PIECE_ROOK)
    {
        return false;
    }

    if (kingside)
    {
        if (board->squares[row][5].has_piece || board->squares[row][6].has_piece)
        {
            return false;
        }
    }
    else
    {
        if (board->squares[row][1].has_piece || board->squares[row][2].has_piece || board->squares[row][3].has_piece)
        {
            return false;
        }
    }

    if (board_in_check(board))
    {
        return false;
    }

    struct chess_board test_board = *board;
    struct chess_move test_move = {0};
    test_move.player = board->next_move_player;
    test_move.piece_type = PIECE_KING;
    test_move.is_castle = false;
    test_move.is_capture = false;
    test_move.is_promotion = false;

    int step = (king_to_col > king_from_col) ? 1 : -1;
    int king_col = king_from_col;

    while (king_col != king_to_col)
    {
        int next_col = king_col + step;

        test_move.from_row = row;
        test_move.from_col = king_col;
        test_move.to_row = row;
        test_move.to_col = next_col;

        board_apply_move(&test_board, &test_move);

        // board_apply_move flips next_move_player;
        // we want to check the king that just moved.
        test_board.next_move_player = test_move.player;

        king_col = next_col;

        if (board_in_check(&test_board))
        {
            return false;
        }
    }

    return true;
}

void board_initialize(struct chess_board *board)
{
    // initialize empty squares
    for (int row = 0; row < BOARD_SIZE; row++)
    {
        for (int col = 0; col < BOARD_SIZE; col++)
        {
            board->squares[row][col].has_piece = false;
            board->squares[row][col].owner = PLAYER_WHITE; // useless but whatever. we use the has_piece flag to check for empty squares
        }
    }
    // initialize black pieces
    board->squares[0][0] = (struct square){true, PIECE_ROOK, PLAYER_BLACK};
    board->squares[0][1] = (struct square){true, PIECE_KNIGHT, PLAYER_BLACK};
    board->squares[0][2] = (struct square){true, PIECE_BISHOP, PLAYER_BLACK};
    board->squares[0][3] = (struct square){true, PIECE_QUEEN, PLAYER_BLACK};
    board->squares[0][4] = (struct square){true, PIECE_KING, PLAYER_BLACK};
    board->squares[0][5] = (struct square){true, PIECE_BISHOP, PLAYER_BLACK};
    board->squares[0][6] = (struct square){true, PIECE_KNIGHT, PLAYER_BLACK};
    board->squares[0][7] = (struct square){true, PIECE_ROOK, PLAYER_BLACK};

    // pawns
    for (int col = 0; col < 8; col++)
    {
        board->squares[1][col] = (struct square){true, PIECE_PAWN, PLAYER_BLACK};
        board->squares[6][col] = (struct square){true, PIECE_PAWN, PLAYER_WHITE};
    }

    // initialize white pieces
    board->squares[7][0] = (struct square){true, PIECE_ROOK, PLAYER_WHITE};
    board->squares[7][1] = (struct square){true, PIECE_KNIGHT, PLAYER_WHITE};
    board->squares[7][2] = (struct square){true, PIECE_BISHOP, PLAYER_WHITE};
    board->squares[7][3] = (struct square){true, PIECE_QUEEN, PLAYER_WHITE};
    board->squares[7][4] = (struct square){true, PIECE_KING, PLAYER_WHITE};
    board->squares[7][5] = (struct square){true, PIECE_BISHOP, PLAYER_WHITE};
    board->squares[7][6] = (struct square){true, PIECE_KNIGHT, PLAYER_WHITE};
    board->squares[7][7] = (struct square){true, PIECE_ROOK, PLAYER_WHITE};

    // set next move player
    board->next_move_player = PLAYER_WHITE;

    // set castling rights (MIGHT REMOVED)
    board->rights.white_kingside = true;
    board->rights.white_queenside = true;
    board->rights.black_kingside = true;
    board->rights.black_queenside = true;
}

void board_complete_move(const struct chess_board *board, struct chess_move *move)
{
    move->player = board->next_move_player; // set the player making the move

    if (move->is_castle)
    {
        if (!board_can_castle(board, move->castle_kingside))
        {
            panicf("move completion error: illegal castling by %s\n", player_string(move->player));
        }

        int row = (move->player == PLAYER_WHITE) ? 7 : 0;

        move->from_row = row;
        move->from_col = 4;
        move->to_row = row;
        move->to_col = move->castle_kingside ? 6 : 2;

        move->piece_type = PIECE_KING;
        move->is_capture = false;
        move->is_promotion = false;

        return;
    }

    if (move->to_row < 0 || move->to_row >= BOARD_SIZE || move->to_col < 0 || move->to_col >= BOARD_SIZE) // are we in bounds
    {
        panicf("move completion error: %s %s to %c%c\n", player_string(move->player), piece_string(move->piece_type), 'a' + move->to_col, '1' + (8 - move->to_row - 1));
    }
    if (board->squares[move->to_row][move->to_col].has_piece && board->squares[move->to_row][move->to_col].owner == move->player) // if our destination has our OWN piece, we can't move there.
    {
        panicf("move completion error: %s %s to %c%c\n", player_string(move->player), piece_string(move->piece_type), 'a' + move->to_col, '1' + (8 - move->to_row - 1));
    }

    // we use arrays to store possible source squares. one for rows and one for columns
    // we also have a counter to keep track of how many possible moves we have found
    int possible_rows[16];
    int possible_cols[16];
    int possible_moves = 0;

    for (int from_row = 0; from_row < BOARD_SIZE; ++from_row)
    {
        for (int from_col = 0; from_col < BOARD_SIZE; ++from_col)
        {
            const struct square *source_square = &board->squares[from_row][from_col]; // current square
            if (!source_square->has_piece)
            {
                continue;
            }
            if (source_square->owner != move->player)
            {
                continue;
            }
            if (source_square->piece != move->piece_type)
            {
                continue;
            }

            if (move->from_row != -1 && move->from_row != from_row)
            {
                continue;
            }
            if (move->from_col != -1 && move->from_col != from_col)
            {
                continue;
            }

            if (!board_is_legal_move(board, from_row, from_col, move->to_row, move->to_col))
            {
                continue;
            }

            if (possible_moves < (int)(sizeof(possible_rows) / sizeof(possible_rows[0]))) // check if we have space to store another possible move
            {
                possible_rows[possible_moves] = from_row;
                possible_cols[possible_moves] = from_col;
                possible_moves++; // increment the count of possible moves
            }
        }
    }

    if (possible_moves == 0) // if we found no possible moves, the move is invalid
    {
        panicf("move completion error: %s %s to %c%c\n", player_string(move->player), piece_string(move->piece_type), 'a' + move->to_col, '1' + (8 - move->to_row - 1));
    }
    else if (possible_moves > 1) // if we found multiple possible moves, the move is ambiguous
    {
        panicf("parse error: ambiguous move\n"); // ambiguous move panic statement not outlined in rubric??? i will keep for my own sanity
    }

    // set the source square of the move
    move->from_row = possible_rows[0];
    move->from_col = possible_cols[0];

    // the beauty of the code is that we don't need to rely on the parser to tell us if it's a capture or promotion, we have flags for that lmao
    move->is_capture = board->squares[move->to_row][move->to_col].has_piece && board->squares[move->to_row][move->to_col].owner != move->player; // if the destination square has an opponent's piece, it's a capture

    if (move->piece_type == PIECE_PAWN)
    {
        if ((move->player == PLAYER_WHITE && move->to_row == 0) || (move->player == PLAYER_BLACK && move->to_row == 7)) // if the pawn reaches the end --> promote
        {
            move->is_promotion = true;
        }
        else
        {
            move->is_promotion = false;
        }
    }
}

void board_apply_move(struct chess_board *board, const struct chess_move *move)
{
    if (move->from_row < 0 || move->from_row >= BOARD_SIZE || move->from_col < 0 || move->from_col >= BOARD_SIZE || move->to_row < 0 || move->to_row >= BOARD_SIZE || move->to_col < 0 || move->to_col >= BOARD_SIZE)
    {
        panicf("move completion error: %s %s to %c%c\n", player_string(move->player), piece_string(move->piece_type), 'a' + move->to_col, '1' + (8 - move->to_row - 1));
    }

    // get the source and destination squares
    struct square *src = &board->squares[move->from_row][move->from_col];
    struct square *dst = &board->squares[move->to_row][move->to_col];

    if (!src->has_piece || src->owner != move->player || src->piece != move->piece_type) // if our source square does not have a piece or the owner if the source square is not the current player or the source piece does not equal the move piece type
    {
        panicf("move completion error: %s %s to %c%c\n", player_string(move->player), piece_string(move->piece_type), 'a' + move->to_col, '1' + (8 - move->to_row - 1));
    }

    if (move->is_castle)
    {
        // castle logic
        int row = (move->player == PLAYER_WHITE) ? 7 : 0;

        struct square *rook_src;
        struct square *rook_dst;

        if (move->castle_kingside)
        {
            rook_src = &board->squares[row][7];
            rook_dst = &board->squares[row][5];
        }
        else
        {
            rook_src = &board->squares[row][0];
            rook_dst = &board->squares[row][3];
        }

        if (!rook_src->has_piece || rook_src->owner != move->player || rook_src->piece != PIECE_ROOK)
        {
            panicf("move completion error: %s %s to %c%c\n", player_string(move->player), piece_string(move->piece_type), 'a' + move->to_col, '1' + (8 - move->to_row - 1));
        }

        *rook_dst = *rook_src;
        rook_src->has_piece = false;
    }

    *dst = *src; // apply the move by copying the struct from source to destination

    if (move->is_promotion)
    {
        dst->piece = move->promo_piece; // promote the pawn to the specified piece (PARSER WILL SET THIS LATER)
    }

    src->has_piece = false; // empty the source square

    board->next_move_player = (board->next_move_player == PLAYER_WHITE) ? PLAYER_BLACK : PLAYER_WHITE; // switch the next move player
}

int board_score_move(const struct chess_board *board, const struct chess_move *move)
{
    int score = 0;

    if (move->piece_type == PIECE_KING && !move->is_castle)
    {
        score -= 20;
    }
    if (move->is_castle)
    {
        score += 1000;
    }

    // capturing a piece affects the suggested move
    if (move->is_capture)
    {
        const struct square *dst = &board->squares[move->to_row][move->to_col];
        switch (dst->piece)
        {
        case PIECE_PAWN:
            score += 100;
            break;
        case PIECE_KNIGHT:
            score += 300;
            break;
        case PIECE_BISHOP:
            score += 300;
            break;
        case PIECE_ROOK:
            score += 500;
            break;
        case PIECE_QUEEN:
            score += 900;
            break;
        case PIECE_KING:
            score += 10000;
            break;
        }
    }
    // center advantage
    if ((move->to_row == 3 || move->to_row == 4) && (move->to_col == 3 || move->to_col == 4))
    {
        score += 30;
    }
    // moving pawns forward
    if (move->piece_type == PIECE_PAWN)
    {
        int direction = (move->player == PLAYER_WHITE) ? (7 - move->to_row) : move->to_row;
        score += direction * 5;
    }
    // getting ur knights and bishops in the game
    if (move->piece_type == PIECE_KNIGHT || move->piece_type == PIECE_BISHOP)
    {
        int start_row = (move->player == PLAYER_WHITE) ? 7 : 0;
        if (move->from_row == start_row)
        {
            score += 20;
        }
    }
    return score;
}

void board_recommend_move(const struct chess_board *board, struct chess_move *recommended_move)
{
    //the difference between a safe move and a legal move is that a safe move is legal and cannot force the player into checkmate on the next turn
    bool can_make_safe_move = false; // flag to track if we can make a safe move
    struct chess_move safe_move = {0};

    bool legal_move_exists = false; // flag we use as a fallback if no checkmate move is found
    struct chess_move legal_move = {0};

    int score_legal = -100000;
    int best_safe_score = -100000;

    // try castling moves explicitly
    for (int side = 0; side < 2; side++)
    {
        bool kingside = (side == 0);
        if (!board_can_castle(board, kingside))
        {
            continue;
        }

        struct chess_move move = {0};
        move.player = board->next_move_player;
        move.piece_type = PIECE_KING;
        move.is_castle = true;
        move.castle_kingside = kingside;
        move.is_capture = false;
        move.is_promotion = false;

        struct chess_board castle_board_copy = *board;
        board_complete_move(&castle_board_copy, &move);
        board_apply_move(&castle_board_copy, &move);

        castle_board_copy.next_move_player = move.player;
        if (board_in_check(&castle_board_copy))
        {
            continue;
        }

        int current_move_score = board_score_move(board, &move);

        if (!legal_move_exists || current_move_score > score_legal)
        {
            score_legal = current_move_score;
            legal_move = move;
            legal_move_exists = true;
        }

        castle_board_copy.next_move_player = (move.player == PLAYER_WHITE) ? PLAYER_BLACK : PLAYER_WHITE;
        if (board_in_checkmate(&castle_board_copy))
        {
            *recommended_move = move;
            return;
        }

        if (!can_make_safe_move || current_move_score > best_safe_score)
        {
            can_make_safe_move = true;
            best_safe_score = current_move_score;
            safe_move = move;
        }
    }

    // function is o(n^8) but whatever for now
    // MAIN SEARCH
    for (int from_row = 0; from_row < BOARD_SIZE; from_row++)
    {
        for (int from_col = 0; from_col < BOARD_SIZE; from_col++)
        {
            const struct square *src = &board->squares[from_row][from_col];

            if (!src->has_piece || src->owner != board->next_move_player)
            {
                continue;
            }

            for (int to_row = 0; to_row < BOARD_SIZE; to_row++)
            {
                for (int to_col = 0; to_col < BOARD_SIZE; to_col++)
                {
                    const struct square *dst = &board->squares[to_row][to_col];

                    if (dst->has_piece && dst->owner == board->next_move_player)
                    {
                        continue;
                    }

                    if (!board_is_legal_move(board, from_row, from_col, to_row, to_col))
                    {
                        continue;
                    }

                    struct chess_move move = {0}; // anti garbage
                    move.player = board->next_move_player;
                    move.piece_type = src->piece;
                    move.from_row = from_row;
                    move.from_col = from_col;
                    move.to_row = to_row;
                    move.to_col = to_col;
                    move.is_capture = (dst->has_piece && dst->owner != board->next_move_player);
                    move.is_castle = false;
                    move.is_promotion = false;

                    if (move.piece_type == PIECE_PAWN)
                    {
                        if ((move.player == PLAYER_WHITE && to_row == 0) || (move.player == PLAYER_BLACK && to_row == 7))
                        {
                            move.is_promotion = true;
                            move.promo_piece = PIECE_QUEEN;
                        }
                    }

                    struct chess_board board_copy = *board;
                    board_apply_move(&board_copy, &move);

                    board_copy.next_move_player = move.player;

                    if (board_in_check(&board_copy))
                    {
                        continue;
                    }
                    int current_move_score = board_score_move(board, &move);
                    if (!legal_move_exists || current_move_score > score_legal)
                    {
                        score_legal = current_move_score;
                        legal_move = move;
                        legal_move_exists = true;
                    }

                    board_copy.next_move_player = (move.player == PLAYER_WHITE) ? PLAYER_BLACK : PLAYER_WHITE;
                    if (board_in_checkmate(&board_copy))
                    {
                        *recommended_move = move;
                        return;
                    }

                    bool enemy_mate = false;
                    struct chess_board enemy_board = board_copy;

                    for (int enemy_from_row = 0; enemy_from_row < BOARD_SIZE && !enemy_mate; enemy_from_row++)
                    {
                        for (int enemy_from_column = 0; enemy_from_column < BOARD_SIZE && !enemy_mate; enemy_from_column++)
                        {
                            const struct square *enemy_src = &enemy_board.squares[enemy_from_row][enemy_from_column];
                            if (!enemy_src->has_piece || enemy_src->owner != enemy_board.next_move_player)
                            {
                                continue;
                            }

                            for (int enemy_to_row = 0; enemy_to_row < BOARD_SIZE && !enemy_mate; enemy_to_row++)
                            {
                                for (int enemy_to_column = 0; enemy_to_column < BOARD_SIZE && !enemy_mate; enemy_to_column++)
                                {
                                    const struct square *enemy_dst = &enemy_board.squares[enemy_to_row][enemy_to_column];

                                    if (enemy_dst->has_piece && enemy_dst->owner == enemy_board.next_move_player)
                                    {
                                        continue;
                                    }

                                    if (!board_is_legal_move(&enemy_board, enemy_from_row, enemy_from_column, enemy_to_row, enemy_to_column))
                                    {
                                        continue;
                                    }

                                    struct chess_move enemy_move = {0};

                                    enemy_move.player = enemy_board.next_move_player;
                                    enemy_move.piece_type = enemy_src->piece;
                                    enemy_move.from_row = enemy_from_row;
                                    enemy_move.from_col = enemy_from_column;
                                    enemy_move.to_row = enemy_to_row;
                                    enemy_move.to_col = enemy_to_column;
                                    enemy_move.is_capture = (enemy_dst->has_piece && enemy_dst->owner != enemy_board.next_move_player);
                                    enemy_move.is_castle = false;
                                    enemy_move.is_promotion = false;

                                    if (enemy_move.piece_type == PIECE_PAWN)
                                    {
                                        if ((enemy_move.player == PLAYER_WHITE && enemy_to_row == 0) || (enemy_move.player == PLAYER_BLACK && enemy_to_row == 7))
                                        {
                                            enemy_move.is_promotion = true;
                                            enemy_move.promo_piece = PIECE_QUEEN; // lets just promote to the queen to simplify
                                        }
                                    }

                                    struct chess_board reply = enemy_board; // Simulate enemy move on a copy to test for checkmate
                                    board_apply_move(&reply, &enemy_move);

                                    reply.next_move_player = enemy_move.player;
                                    if (board_in_check(&reply))
                                    {
                                        continue;
                                    }

                                    reply.next_move_player = (enemy_move.player == PLAYER_WHITE) ? PLAYER_BLACK : PLAYER_WHITE;

                                    if (board_in_checkmate(&reply))
                                    {
                                        enemy_mate = true;
                                    }
                                }
                            }
                        }
                    }

                    if (!enemy_mate)
                    {
                        if (!can_make_safe_move || current_move_score > best_safe_score) // !can_make_safe_move is for the first safe move we find
                        {
                            can_make_safe_move = true;
                            best_safe_score = current_move_score;
                            safe_move = move;
                        }
                    }
                }
            }
        }
    }

    if (can_make_safe_move)
    {
        *recommended_move = safe_move;
    }
    else if (legal_move_exists)
    {
        *recommended_move = legal_move;
    }
    else
    {
        panicf("move completion error: no legal moves\n");
    }
}

void board_print(const struct chess_board *board)
{
    printf("\n   a b c d e f g h\n");
    printf("  -----------------\n");

    for (int row = 0; row < 8; row++)
    {
        printf("%d| ", 8 - row);

        for (int col = 0; col < 8; col++)
        {
            const struct square sq = board->squares[row][col];

            if (sq.has_piece)
            {
                const char *p = ".";

                if (sq.owner == PLAYER_WHITE)
                {
                    switch (sq.piece)
                    {
                    case PIECE_KING:
                        p = "♔";
                        break;
                    case PIECE_QUEEN:
                        p = "♕";
                        break;
                    case PIECE_ROOK:
                        p = "♖";
                        break;
                    case PIECE_BISHOP:
                        p = "♗";
                        break;
                    case PIECE_KNIGHT:
                        p = "♘";
                        break;
                    case PIECE_PAWN:
                        p = "♙";
                        break;
                    }
                }
                else
                {
                    switch (sq.piece)
                    {
                    case PIECE_KING:
                        p = "♚";
                        break;
                    case PIECE_QUEEN:
                        p = "♛";
                        break;
                    case PIECE_ROOK:
                        p = "♜";
                        break;
                    case PIECE_BISHOP:
                        p = "♝";
                        break;
                    case PIECE_KNIGHT:
                        p = "♞";
                        break;
                    case PIECE_PAWN:
                        p = "♟";
                        break;
                    }
                }

                printf("%s ", p);
            }
            else
            {
                printf(". ");
            }
        }

        printf("|%d\n", 8 - row);
    }

    printf("  -----------------\n");
    printf("   a b c d e f g h\n\n");
}

void board_summarize(const struct chess_board *board)
{
    board_print(board);
    if (board_in_checkmate(board))
    {
        enum chess_player loser = board->next_move_player;
        enum chess_player winner = (loser == PLAYER_WHITE) ? PLAYER_BLACK : PLAYER_WHITE;
        printf("%s wins by checkmate\n", player_string(winner));
    }

    else if (board_in_stalemate(board))
    {
        printf("draw by stalemate\n");
    }
    else
    {
        printf("game incomplete\n");
        struct chess_move recommended_move;
        board_recommend_move(board, &recommended_move);
        printf("suggest: %s %s from %c%c to %c%c\n", player_string(recommended_move.player), piece_string(recommended_move.piece_type), 'a' + recommended_move.from_col, '1' + (8 - recommended_move.from_row - 1), 'a' + recommended_move.to_col, '1' + (8 - recommended_move.to_row - 1));
    }
}