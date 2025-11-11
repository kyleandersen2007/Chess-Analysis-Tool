#include "board.h"
#include <stdio.h>

int main()
{
    struct chess_board board;
    board_initialize(&board);
    board_summarize(&board);

    struct chess_move move;
    while (parse_move(&move))
    {
        printf("%d\n",move.to_row);
        printf("%d\n",move.to_col);
        board_complete_move(&board, &move);
        board_apply_move(&board, &move);
    }

    board_summarize(&board);

    return 0;
}
