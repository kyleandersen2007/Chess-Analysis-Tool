#include "parser.h"
#include <stdio.h>
#include "panic.h"

bool parse_move(struct chess_move *move)
{
    char c;

    // Get the first character of the move, ignoring any initial spaces.
    do
    {
        c = getc(stdin);
    } while (c == ' ');

    // Check if we are at the end of input.
    if (c == '\n' || c == '\r')
        return false;

    switch (c)
    {

    default:
        panicf("parse error at character '%c'\n", c);
    }

    move->piece_type = PIECE_PAWN; // default to pawn for now
    move->from_row = -1;
    move->from_col = -1;
    // to be implemented
}
