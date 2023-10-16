/* SPDX-License-Identifier: GPL-2.0-only */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pieces.h>

#define MAP_DECL(_fen, _id)                              \
	struct piece_mapping map_##_fen = { .id = _id }; \
	mappings[(int)#_fen[0]] = map_##_fen

#define STARTING_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

struct piece_mapping {
	char id;
};

/* Wasting space because fast */
static struct piece_mapping mappings[115] = { { 0 } };

struct board board;

static int fen_stage_1(const char *fen, struct board *board)
{
	int i = 0;
	int j = 0;

	while (fen[i] != '\0') {
		if (fen[i] == '/') {
			i++;
			continue;
		}

		if (fen[i] == ' ') {
			return i + 1;
		}

		if (fen[i] >= '1' && fen[i] <= '8') {
			for (int k = 0; k < fen[i] - '0'; k++) {
				board->board[j++] = 0;
			}
		} else {
			board->board[j++] = mappings[(int)fen[i]].id;
		}

		i++;
	}

	return -1;
}

static int fen_stage_2(const char *fen, struct board *board, int i)
{
	while (fen[i] != '\0') {
		if (fen[i] == ' ') {
			return i + 1;
		}

		if (fen[i] == 'w') {
			board->turn = PIECE_WHITE;
		} else if (fen[i] == 'b') {
			board->turn = PIECE_BLACK;
		} else {
			return -1;
		}

		i++;
	}

	return -1;
}

static int fen_stage_3(const char *fen, struct board *board, int i)
{
	while (fen[i] != '\0') {
		if (fen[i] == ' ') {
			return i + 1;
		}

		if (fen[i] == '-') {
			board->castling = 0;
		} else {
			switch (fen[i]) {
			case 'K':
				board->castling |= CASTLE_WHITE_KING;
				break;
			case 'Q':
				board->castling |= CASTLE_WHITE_QUEEN;
				break;
			case 'k':
				board->castling |= CASTLE_BLACK_KING;
				break;
			case 'q':
				board->castling |= CASTLE_BLACK_QUEEN;
				break;
			}
		}

		i++;
	}

	return -1;
}

static int fen_stage_4(const char *fen, struct board *board, int i)
{
	/* convert en passant square string to index */
	while (fen[i] != '\0') {
		if (fen[i] == ' ') {
			return i + 1;
		}

		if (fen[i] == '-') {
			board->en_passant = -1;
		} else {
			/* error checking */
			if (fen[i + 1] < '1' || fen[i + 1] > '8' || fen[i + 1] == '\0') {
				return -1;
			}

			int square = (fen[i] - 'a') + ((7 - (fen[i + 1] - '1')) * 8);

			board->en_passant = square;
		}

		i++;
	}

	return -1;
}

static int fen_stage_5(const char *fen, struct board *board, int i, int *out)
{
	int j = 0;
	while (fen[i] != '\0') {
		if (fen[i] == ' ') {
			*out = j;
			return i + 1;
		}

		if (fen[i] < '0' || fen[i] > '9') {
			return -1;
		}

		/* parse string to int */
		j *= 10;
		j += fen[i] - '0';

		i++;
	}

	*out = j;
	return -1;
}

void fen_to_arr(const char *fen, struct board *board)
{
	int i = 0;

	int stage = 1;

	while (fen[i] != '\0' && i != -1) {
		switch (stage) {
		case 1: /* board pieces */
			i = fen_stage_1(fen, board);
			stage++;
			break;
		case 2: /* turn */
			i = fen_stage_2(fen, board, i);
			stage++;
			break;
		case 3: /* castling */
			i = fen_stage_3(fen, board, i);
			stage++;
			break;
		case 4: /* en passant(TM) square */
			i = fen_stage_4(fen, board, i);
			stage++;
			break;
		case 5: /* halfmove clock */
			i = fen_stage_5(fen, board, i, &board->halfmove);
			stage++;
			break;
		case 6: /* fullmove number */
			i = fen_stage_5(fen, board, i, &board->fullmove);
			stage++;
			break;
		default:
			i = -1;
			break;
		}
	}
}

int rmoves(struct board *board, char square, char *out, int n)
{
	int mov_vecs[4] = { 8, -8, 1, -1 };

	for (int i = 0; i < 4; i++) {
		int mov_vec = mov_vecs[i];

		int j = square + mov_vec;

		while (j >= 0 && j < 64) {
			/* check if square is not on same rank or file */
			if ((j & 7) != (square & 7) && (j >> 3) != (square >> 3))
				break;

			/* check if square is occupied */
			if (board->board[j] != 0) {
				/* check if square is occupied by same color */
				if ((board->board[j] & COL_MASK) == (board->board[(int)square] & COL_MASK) &&
				    board->board[j] != PIECE_NONE)
					break;

				out[n++] = j;
				break;
			} else {
				out[n++] = j;
			}

			j += mov_vec;
		}
	}

	return n;
}

/* calculate bishop moves */
int bmoves(struct board *board, char square, char *out, int n)
{
	int mov_vecs[4] = { 9, -9, 7, -7 };

	for (int i = 0; i < 4; i++) {
		int mov_vec = mov_vecs[i];

		int j = square + mov_vec;

		while (j >= 0 && j < 64) {
			/* check if not on same diagonal in both directions */
			if ((j & 7) - (square & 7) != (j >> 3) - (square >> 3) &&
			    (j & 7) - (square & 7) != (square >> 3) - (j >> 3))
				break;

			/* check if square is occupied */
			if (board->board[j] != 0) {
				/* check if square is occupied by same color */
				if ((board->board[j] & COL_MASK) == (board->board[(int)square] & COL_MASK) &&
				    board->board[j] != PIECE_NONE)
					break;

				out[n++] = j;
				break;
			} else {
				out[n++] = j;
			}

			j += mov_vec;
		}
	}

	return n;
}

/* calculate queen moves */
int qmoves(struct board *board, char square, char *out, int n)
{
	n = rmoves(board, square, out, n);
	n = bmoves(board, square, out, n);

	return n;
}

/* calculate king moves */
int kmoves(struct board *board, char square, char *out, int n)
{
	int mov_vecs[8] = { 8, -8, 1, -1, 9, -9, 7, -7 };

	for (int i = 0; i < 8; i++) {
		int mov_vec = mov_vecs[i];

		int j = square + mov_vec;

		/* check if square is occupied */
		if (j >= 0 && j < 64) {
			/* check if square is occupied by same color */
			if ((board->board[j] & COL_MASK) != (board->board[(int)square] & COL_MASK) || board->board[j] == PIECE_NONE)
				out[n++] = j;
		}
	}

	return n;
}

/* calculate knight moves */
int nmoves(struct board *board, char square, char *out, int n)
{
	int mov_vecs[8] = { 17, -17, 15, -15, 10, -10, 6, -6 };

	for (int i = 0; i < 8; i++) {
		int mov_vec = mov_vecs[i];

		int j = square + mov_vec;

		/* check if square is occupied */
		if (j >= 0 && j < 64) {
			/* check if square is occupied by same color */
			if ((board->board[j] & COL_MASK) != (board->board[(int)square] & COL_MASK) || board->board[j] == PIECE_NONE)
				out[n++] = j;
		}
	}

	return n;
}

/* calculate pawn moves */
int pmoves(struct board *board, char square, char *out, int n)
{
	int mov_vecs[2] = { 8, 16 };
	int take_vecs[2] = { 7, 9 };

	for (int i = 0; i < 2; i++) {
		int mov_vec = mov_vecs[i];

		if ((board->board[(int)square] & COL_MASK) == PIECE_WHITE) {
			mov_vec = -mov_vec;
		}

		int j = square + mov_vec;

		/* check if square is occupied */
		if (j >= 0 && j < 64) {
			/* check if square is occupied by any piece */
			if (board->board[j] == PIECE_NONE)
				out[n++] = j;
			else
				break;

			/* do not continue if pawn is not on starting rank */
			if (!(square > 7 && square < 16) && !(square > 47 && square < 56))
				break;
		}
	}

	for (int i = 0; i < 2; i++) {
		int take_vec = take_vecs[i];

		if ((board->board[(int)square] & COL_MASK) == PIECE_WHITE) {
			take_vec = -take_vec;
		}

		int j = square + take_vec;

		/* check if square is occupied by piece of opposite color */
		if (j >= 0 && j < 64) {
			if ((board->board[j] & COL_MASK) != (board->board[(int)square] & COL_MASK) && board->board[j] != PIECE_NONE)
				out[n++] = j;
			if (j == board->en_passant)
				out[n++] = j;
		}
	}

	return n;
}

int legalmoves(struct board *board, char square, char *out, int n)
{
	if (board->board[(int)square] == PIECE_NONE)
		return 0;

	switch (board->board[(int)square] & PIECE_MASK) {
	case PIECE_PAWN:
		n = pmoves(board, square, out, n);
		break;
	case PIECE_KNIGHT:
		n = nmoves(board, square, out, n);
		break;
	case PIECE_BISHOP:
		n = bmoves(board, square, out, n);
		break;
	case PIECE_ROOK:
		n = rmoves(board, square, out, n);
		break;
	case PIECE_QUEEN:
		n = qmoves(board, square, out, n);
		break;
	case PIECE_KING:
		n = kmoves(board, square, out, n);
		break;
	}

	return n;
}

void lmbdown(int x, int y)
{
	/* mouse returning from off screen edge case */
	if (board.held)
		return;

	int bx = (board.width - board.size) >> 1;
	int by = (board.height - board.size) >> 1;

	if (x < bx || x > bx + board.size || y < by || y > by + board.size)
		return;

	/* square index of hovered square */
	/* a8 is 0, h1 is 63 */
	int hov_square = ((x - bx) / (board.size >> 3)) + (((y - by) / (board.size >> 3)) << 3);

	if (hov_square < 0 || hov_square > 63)
		return;

	if (board.legal_moves == NULL)
		board.legal_moves = malloc(64);

	memset(board.legal_moves, 0, 64);

	board.legal_moves_count = legalmoves(&board, hov_square, board.legal_moves, 0);

	board.held = board.board[hov_square];
	board.held_origin = hov_square;
	board.board[hov_square] = 0;
}

void lmbup(int x, int y)
{
	int bx = (board.width - board.size) >> 1;
	int by = (board.height - board.size) >> 1;

	if (x < bx || x > bx + board.size || y < by || y > by + board.size)
		goto invalid;

	/* square index of hovered square */
	/* a8 is 0, h1 is 63 */
	int hov_square = ((x - bx) / (board.size >> 3)) + (((y - by) / (board.size >> 3)) << 3);

	if (hov_square < 0 || hov_square > 63)
		return;

	if (SAMECOLOR(board.held, board.board[hov_square]))
		goto invalid;

	if (board.held != PIECE_NONE) {
		for (int i = 0; i < board.legal_moves_count; i++) {
			if (board.legal_moves[i] == hov_square) {
				board.board[hov_square] = board.held;
				board.held = 0;
				board.held_origin = -1;
				goto valid;
			}
		}

		/* goto considered useful */
		goto invalid;
	}

valid:
	if (board.legal_moves != NULL) {
		free(board.legal_moves);
		board.legal_moves = NULL;
		board.legal_moves_count = 0;
	}

	return;

invalid:

	if (board.legal_moves != NULL) {
		free(board.legal_moves);
		board.legal_moves = NULL;
		board.legal_moves_count = 0;
	}
	board.board[(int)board.held_origin] = board.held;
	board.held = 0;
	board.held_origin = -1;
}

void board_init()
{
	memset(&board, 0, sizeof(board));

	MAP_DECL(P, PIECE_WHITE | PIECE_PAWN);
	MAP_DECL(N, PIECE_WHITE | PIECE_KNIGHT);
	MAP_DECL(B, PIECE_WHITE | PIECE_BISHOP);
	MAP_DECL(R, PIECE_WHITE | PIECE_ROOK);
	MAP_DECL(Q, PIECE_WHITE | PIECE_QUEEN);
	MAP_DECL(K, PIECE_WHITE | PIECE_KING);
	MAP_DECL(p, PIECE_BLACK | PIECE_PAWN);
	MAP_DECL(n, PIECE_BLACK | PIECE_KNIGHT);
	MAP_DECL(b, PIECE_BLACK | PIECE_BISHOP);
	MAP_DECL(r, PIECE_BLACK | PIECE_ROOK);
	MAP_DECL(q, PIECE_BLACK | PIECE_QUEEN);
	MAP_DECL(k, PIECE_BLACK | PIECE_KING);

	fen_to_arr(STARTING_FEN, &board);
}
