#include "parser.h"
#include <stdio.h>
#include "panic.h"

bool parse_move(struct chess_move *move)
{
    char c;
    do
    {
        c = getc(stdin);
        if (c == EOF)
            return false;
    } while (c == ' ');

    if (c == '\n' || c == '\r')
        return false;

    char buf[32];
    int len = 0;

    if (c != ' ' && c != '\n' && c != '\r')
    {
        buf[len++] = c;
    }

    while (1)
    {
        c = getc(stdin);
        if (c == EOF || c == '\n' || c == '\r')
            break;
        if (c == ' ')
            continue;
        if (len < (int)(sizeof(buf) - 1))
            buf[len++] = c;
    }
    buf[len] = '\0';

    if (len == 0)
        return false;

    move->is_castle = false;
    move->castle_kingside = false;
    move->is_capture = false;
    move->is_promotion = false;
    move->promo_piece = PIECE_QUEEN;
    move->from_row = -1;
    move->from_col = -1;
    move->to_row = -1;
    move->to_col = -1;

    if (buf[0] == 'O')
    {
        if (buf[1] == '-' && buf[2] == 'O' && buf[3] == '\0')
        {
            move->is_castle = true;
            move->castle_kingside = true;
        }
        else if (buf[1] == '-' && buf[2] == 'O' && buf[3] == '-' && buf[4] == 'O' && buf[5] == '\0')
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

    switch (buf[i])
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
        if (buf[i] < 'a' || buf[i] > 'h')
        {
            panicf("parse error at character '%c'\n", buf[i]);
        }
        piece = PIECE_PAWN;
        break;
    }

    char disamb_file = 0;
    char disamb_rank = 0;
    char dest_file = 0;
    char dest_rank = 0;

    while (buf[i] != '\0')
    {
        char ch = buf[i];

        if (ch == 'x')
        {
            move->is_capture = true;
            i++;
            continue;
        }

        if (ch >= 'a' && ch <= 'h')
        {
            if (buf[i + 1] >= '1' && buf[i + 1] <= '8')
            {
                dest_file = ch;
                dest_rank = buf[i + 1];
                i += 2;
                break;
            }
            if (disamb_file != 0)
                panicf("parse error: multiple file disambiguators\n");
            disamb_file = ch;
            i++;
            continue;
        }

        if (ch >= '1' && ch <= '8')
        {
            if (buf[i + 1] >= 'a' && buf[i + 1] <= 'h' && buf[i + 2] >= '1' && buf[i + 2] <= '8' && dest_file == 0 && dest_rank == 0)
            {
                if (disamb_rank != 0)
                    panicf("parse error: multiple rank disambiguators\n");
                disamb_rank = ch;
                dest_file = buf[i + 1];
                dest_rank = buf[i + 2];
                i += 3;
                break;
            }
            if (disamb_rank != 0)
                panicf("parse error: multiple rank disambiguators\n");
            disamb_rank = ch;
            i++;
            continue;
        }

        break;
    }

    if (dest_file == 0 || dest_rank == 0)
    {
        if (buf[i] >= 'a' && buf[i] <= 'h' && buf[i + 1] >= '1' && buf[i + 1] <= '8')
        {
            dest_file = buf[i];
            dest_rank = buf[i + 1];
            i += 2;
        }
        else
            panicf("parse error: missing destination square\n");
    }

    move->piece_type = piece;
    move->to_col = dest_file - 'a';
    move->to_row = 8 - (dest_rank - '0');

    if (disamb_file != 0)
        move->from_col = disamb_file - 'a';
    else
        move->from_col = -1;
    if (disamb_rank != 0)
        move->from_row = 8 - (disamb_rank - '0');
    else
        move->from_row = -1;

    if (buf[i] != '\0')
    {
        char promo_ch = buf[i];
        if (promo_ch == '=')
            promo_ch = buf[++i];
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
        if (buf[i] != '\0')
            panicf("parse error at character '%c'\n", buf[i]);
    }

    return true;
}
