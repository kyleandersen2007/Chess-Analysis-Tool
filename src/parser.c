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

    char buf[16]; // Buffer to hold the move string
    int len = 0;

    // Add the first character we already read
    buf[len++] = c;

    // Read the rest of the characters until a space or newline
    while (1) {
        c = getc(stdin);
        if (c == ' ' || c == '\n' || c == '\r' || c == EOF) {
            // Put the character back if it's not a space (so the next call can handle it if needed)
            // Actually for now, just breaking is fine as we handle spaces at start of next call
            break; 
        }
        if (len < 15) {
            buf[len++] = c;
        }
    }
    buf[len] = '\0'; // Null-terminate the string so it's a valid C string
    
// Reset move structure
    move->from_row = -1;
    move->from_col = -1;
    move->to_row = -1;
    move->to_col = -1;
    move->is_capture = false;
    move->is_promotion = false;
    move->is_castle = false;       // <--- CRITICAL
    move->piece_type = PIECE_PAWN;

    // Check for Castling
    if (buf[0] == '0') { // Starts with 0
        move->piece_type = PIECE_KING; // King is the one moving
        move->is_castle = true;

        if (len == 3 && buf[1] == '-' && buf[2] == '0') {
            move->castle_kingside = true; // 0-0
            return true;
        } else if (len == 5 && buf[1] == '-' && buf[2] == '0' && buf[3] == '-' && buf[4] == '0') {
            move->castle_kingside = false; // 0-0-0
            return true;
        } else {
            // Invalid castle string
             panicf("parse error: invalid castle syntax '%s'\n", buf);
        }
    }
    // 1. Identify the Piece
    if (buf[0] >= 'A' && buf[0] <= 'Z') { // Is it uppercase?
        switch (buf[0]) {
            case 'N': move->piece_type = PIECE_KNIGHT; break;
            case 'B': move->piece_type = PIECE_BISHOP; break;
            case 'R': move->piece_type = PIECE_ROOK; break;
            case 'Q': move->piece_type = PIECE_QUEEN; break;
            case 'K': move->piece_type = PIECE_KING; break;
            // Note: If it starts with '0', the Castling block below will handle it
        }
    } else {
        // If it starts with a-h, it's a pawn
        move->piece_type = PIECE_PAWN;
    }
    // 2. Identify the Destination
    // We only do this if it's NOT a castle (which starts with '0')
    if (buf[0] != '0') {
        char file_char = buf[len - 2]; //files are vertical columns a-h
        char rank_char = buf[len - 1]; //ranks are horizontal rows 1-8

        // 'a' becomes 0, 'b' becomes 1...
        move->to_col = file_char - 'a';

        // '1' becomes 7, '8' becomes 0 (Array index is flipped)
        move->to_row = 8 - (rank_char - '0');
    }
    

}