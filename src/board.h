#ifndef APSC143__BOARD_H
#define APSC143__BOARD_H

#define BOARD_SIZE 8

enum chess_player
{
    PLAYER_WHITE,
    PLAYER_BLACK,
};

// Gets a lowercase string denoting the player.
const char *player_string(enum chess_player player);

enum chess_piece
{
    PIECE_NONE = -1,
    PIECE_PAWN = 0,
    PIECE_KNIGHT,
    PIECE_BISHOP,
    PIECE_ROOK,
    PIECE_QUEEN,
    PIECE_KING,
};

struct square {
    enum chess_piece type;
    enum chess_player owner;
    int column;
    int row;
};

// Gets a lowercase string denoting the piece type.
const char *piece_string(enum chess_piece piece);

struct chess_board
{
    enum chess_player next_move_player;
    struct square squares[BOARD_SIZE][BOARD_SIZE];

    // TODO: what other fields are needed?
};

struct chess_move
{
    enum chess_piece piece_type; // what type of piece
    enum chess_player player;    // who owns the move
    int targetRow, targetColumn;
    int previousRow, previousColumn;
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
void board_summarize(const struct chess_board *board);

#endif
