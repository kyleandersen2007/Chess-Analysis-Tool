#include "parser.h"
#include <stdio.h>
#include "panic.h"

bool parse_move(struct chess_move *move)
{
    char current_char;
    do
    {
        current_char = getc(stdin);
        if (current_char == EOF)
        {
            return false;
        }
    } while (current_char == ' ');

    if (current_char == '\n' || current_char == '\r')
    {
        return false;
    }

    char input_buffer[32];
    int input_length = 0;

    if (current_char != ' ' && current_char != '\n' && current_char != '\r')
    {
        input_buffer[input_length++] = current_char;
    }

    while (1)
    {
        current_char = getc(stdin);
        if (current_char == EOF || current_char == '\n' || current_char == '\r')
        {
            break;
        }
        if (current_char == ' ')
        {
            continue;
        }
        if (input_length < (int)(sizeof(input_buffer) - 1))
        {
            input_buffer[input_length++] = current_char;
        }
    }

    input_buffer[input_length] = '\0';

    if (input_length == 0)
    {
        return false;
    }

    move->is_castle = false;
    move->castle_kingside = false;
    move->is_capture = false;
    move->is_promotion = false;
    move->promo_piece = PIECE_QUEEN;
    move->from_row = -1;
    move->from_col = -1;
    move->to_row = -1;
    move->to_col = -1;

    if (input_buffer[0] == 'O')
    {
        if (input_buffer[1] == '-' && input_buffer[2] == 'O' && input_buffer[3] == '\0')
        {
            move->is_castle = true;
            move->castle_kingside = true;
        }
        else if (input_buffer[1] == '-' && input_buffer[2] == 'O' && input_buffer[3] == '-' && input_buffer[4] == 'O' && input_buffer[5] == '\0')
        {
            move->is_castle = true;
            move->castle_kingside = false;
        }
        else
        {
            panicf("parse error: expected O-O or O-O-O\n");
        }
        move->piece_type = PIECE_KING;
        return true;
    }

    enum chess_piece piece = PIECE_PAWN;
    int i = 0;

    switch (input_buffer[i])
    {
    case 'K':
        piece = PIECE_KING;
        i++;
        break;
    case 'Q':
        piece = PIECE_QUEEN;
        i++;
        break;
    case 'R':
        piece = PIECE_ROOK;
        i++;
        break;
    case 'B':
        piece = PIECE_BISHOP;
        i++;
        break;
    case 'N':
        piece = PIECE_KNIGHT;
        i++;
        break;
    default:
        if (input_buffer[i] < 'a' || input_buffer[i] > 'h')
        {
            panicf("parse error at character '%c'\n", input_buffer[i]);
        }
        piece = PIECE_PAWN;
        break;
    }

    char disamb_file = 0;
    char disamb_rank = 0;
    char dest_file = 0;
    char dest_rank = 0;

    while (input_buffer[i] != '\0')
    {
        char ch = input_buffer[i];

        if (ch == 'x')
        {
            move->is_capture = true;
            i++;
            continue;
        }

        if (ch >= 'a' && ch <= 'h')
        {
            if (input_buffer[i + 1] >= '1' && input_buffer[i + 1] <= '8')
            {
                dest_file = ch;
                dest_rank = input_buffer[i + 1];
                i += 2;
                break;
            }
            if (disamb_file != 0)
            {
                return false;
            }
            disamb_file = ch;
            i++;
            continue;
        }

        if (ch >= '1' && ch <= '8')
        {
            if (input_buffer[i + 1] >= 'a' && input_buffer[i + 1] <= 'h' && input_buffer[i + 2] >= '1' && input_buffer[i + 2] <= '8' && dest_file == 0 && dest_rank == 0)
            {
                if (disamb_rank != 0)
                {
                    return false;
                }
                disamb_rank = ch;
                dest_file = input_buffer[i + 1];
                dest_rank = input_buffer[i + 2];
                i += 3;
                break;
            }
            if (disamb_rank != 0)
            {
                return false;
            }
            disamb_rank = ch;
            i++;
            continue;
        }

        break;
    }

    if (dest_file == 0 || dest_rank == 0)
    {
        if (input_buffer[i] >= 'a' && input_buffer[i] <= 'h' && input_buffer[i + 1] >= '1' && input_buffer[i + 1] <= '8')
        {
            dest_file = input_buffer[i];
            dest_rank = input_buffer[i + 1];
            i += 2;
        }
        else
        {
            return false;
        }
    }

    move->piece_type = piece;
    move->to_col = dest_file - 'a';
    move->to_row = 8 - (dest_rank - '0');

    if (disamb_file != 0)
    {
        move->from_col = disamb_file - 'a';
    }
    else
    {
        move->from_col = -1;
    }
    if (disamb_rank != 0)
    {
        move->from_row = 8 - (disamb_rank - '0');
    }
    else
    {
        move->from_row = -1;
    }

    if (input_buffer[i] != '\0')
    {
        char promo_ch = input_buffer[i];
        if (promo_ch == '=')
        {
            promo_ch = input_buffer[++i];
        }
        if (promo_ch == 'Q' || promo_ch == 'R' || promo_ch == 'B' || promo_ch == 'N')
        {
            move->is_promotion = true;
            switch (promo_ch)
            {
            case 'Q':
                move->promo_piece = PIECE_QUEEN;
                break;
            case 'R':
                move->promo_piece = PIECE_ROOK;
                break;
            case 'B':
                move->promo_piece = PIECE_BISHOP;
                break;
            case 'N':
                move->promo_piece = PIECE_KNIGHT;
                break;
            }
            i++;
        }
        if (input_buffer[i] != '\0')
        {
            return false;
        }
    }

    return true;
}
