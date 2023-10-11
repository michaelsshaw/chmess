#include <string.h>
#include <pieces.h>

#define MAP_DECL(_fen, _id)                              \
	struct piece_mapping map_##_fen = { .id = _id }; \
	mappings[(int)#_fen[0]] = map_##_fen

#define STARTING_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"

struct piece_mapping {
	char id;
};

/* Wasting space because fast */
static struct piece_mapping mappings[115] = { { 0 } };

struct board board;

void fen_to_arr(const char *fen, struct board *board)
{
	int i = 0;
	int j = 0;

	while (fen[i] != '\0') {
		if (fen[i] == '/') {
			i++;
			continue;
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
