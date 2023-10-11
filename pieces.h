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

struct board {
	char board[64];
	char turn;
	char castling;
	char en_passant;
	char halfmove;
	char fullmove;
};

extern struct board board;
extern char turn;

void fen_to_arr(const char *fen, struct board *board);
void board_init();

#endif /* _PIECES_H_ */
