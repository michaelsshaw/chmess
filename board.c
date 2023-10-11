#include <stdio.h>
#include <string.h>
#include <pieces.h>

#define MAP_DECL(_fen, _id)                              \
	struct piece_mapping map_##_fen = { .id = _id }; \
	mappings[(int)#_fen[0]] = map_##_fen

#define STARTING_FEN "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2"

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
			board->en_passant = 0xFF;
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

void board_init()
{
	memset(&board, 0, 64);

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
