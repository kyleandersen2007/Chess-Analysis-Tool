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
    // TODO: parse the move, starting from the first character. You are free to
    // start from this switch/case as a template or use a different approach.
    default:
        panicf("parse error at character '%c'\n", c);
    }
}
