/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _PIECES_H_
#define _PIECES_H_

#define PIECE_NONE 0
#define PIECE_PAWN 1
#define PIECE_KNIGHT 2
#define PIECE_BISHOP 3
#define PIECE_ROOK 4
#define PIECE_QUEEN 5
#define PIECE_KING 6

#define PIECE_WHITE 0
#define PIECE_BLACK 8

#define CASTLE_WHITE_KING 1
#define CASTLE_WHITE_QUEEN 2
#define CASTLE_BLACK_KING 4
#define CASTLE_BLACK_QUEEN 8

#define COL_MASK 8
#define PIECE_MASK 7

#define SAMECOLOR(_a, _b) (((_a) & COL_MASK) == ((_b) & COL_MASK) && (_a) != 0 && (_b) != 0)
#define OPPCOLOR(_a, _b) ((((_a) & 8) ^ ((_b) & 8)) && ((_a) != 0) && ((_b) != 0))

struct board {
	char board[64];
	char turn;
	char castling;
	char en_passant;
	int halfmove;
	int fullmove;

	/* piece carried by mouse cursor */
	char held;
	char held_origin;

	int size; /* size of board */

	int width; /* width of viewport */
	int height; /* height of viewport */

	char *legal_moves;
	int legal_moves_count;
};

extern struct board board;
extern char turn;

void fen_to_arr(const char *fen, struct board *board);
void board_init();

#endif /* _PIECES_H_ */
