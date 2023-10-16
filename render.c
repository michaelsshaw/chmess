/* SPDX-License-Identifier: GPL-2.0-only */
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <SDL2/SDL_mouse.h>
#include <pieces.h>

#define DARK_SQUARE 0x769656
#define LIGHT_SQUARE 0xEEEED2

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define PIECE_DECL(_name, _res, _id) struct piece _name = { .path = _res, .texture = NULL, .id = _id }

struct piece {
	const char *path;
	SDL_Texture *texture;
	int id;
};

PIECE_DECL(wp, "res/P.png", PIECE_WHITE | PIECE_PAWN);
PIECE_DECL(wn, "res/N.png", PIECE_WHITE | PIECE_KNIGHT);
PIECE_DECL(wb, "res/B.png", PIECE_WHITE | PIECE_BISHOP);
PIECE_DECL(wr, "res/R.png", PIECE_WHITE | PIECE_ROOK);
PIECE_DECL(wq, "res/Q.png", PIECE_WHITE | PIECE_QUEEN);
PIECE_DECL(wk, "res/K.png", PIECE_WHITE | PIECE_KING);

PIECE_DECL(bp, "res/p.png", PIECE_BLACK | PIECE_PAWN);
PIECE_DECL(bn, "res/n.png", PIECE_BLACK | PIECE_KNIGHT);
PIECE_DECL(bb, "res/b.png", PIECE_BLACK | PIECE_BISHOP);
PIECE_DECL(br, "res/r.png", PIECE_BLACK | PIECE_ROOK);
PIECE_DECL(bq, "res/q.png", PIECE_BLACK | PIECE_QUEEN);
PIECE_DECL(bk, "res/k.png", PIECE_BLACK | PIECE_KING);

struct piece *pieces[] = { NULL, &wp, &wn, &wb, &wr, &wq, &wk, NULL, NULL, &bp, &bn, &bb, &br, &bq, &bk, NULL };

static void rect(SDL_Renderer *renderer, int x, int y, int w, int h)
{
	SDL_Rect rect = { x, y, w, h };
	SDL_RenderFillRect(renderer, &rect);
}

static void color(SDL_Renderer *renderer, unsigned int color)
{
	SDL_SetRenderDrawColor(renderer, color >> 16, color >> 8, color, 255);
}

static void drawboard(SDL_Renderer *renderer, int x, int y, int size)
{
	int square = size >> 3;

	for (int j = 0; j < 8; j++) {
		for (int i = 0; i < 8; i++) {
			color(renderer, (j + i) & 1 ? DARK_SQUARE : LIGHT_SQUARE);
			rect(renderer, x + i * square, y + j * square, square, square);
		}
	}

	color(renderer, 0);
}

static void drawpiece(SDL_Renderer *renderer, int x, int y, int size, int id)
{
	/* Draw the surface */
	struct piece *piece = pieces[id];
	if (piece == NULL)
		return;

	SDL_Rect rect = { x, y, size, size };
	SDL_RenderCopy(renderer, piece->texture, NULL, &rect);
}

static void drawpieces(SDL_Renderer *renderer, int x, int y, int size)
{
	for (int i = 0; i < 64; i++) {
		int x2 = (i & 7) * size;
		int y2 = (i >> 3) * size;
		drawpiece(renderer, x + x2, y + y2, size, board.board[i]);
	}

	if (board.held) {
		int mx, my;
		SDL_GetMouseState(&mx, &my);

		drawpiece(renderer, mx - (size >> 1), my - (size >> 1), size, board.held);
	}
}

void render_init(SDL_Renderer *renderer)
{
	for (int i = 0; i < ARRAY_SIZE(pieces); i++) {
		struct piece *piece = pieces[i];
		if (piece == NULL)
			continue;

		/* Load image with SDL */
		piece->texture = IMG_LoadTexture(renderer, piece->path);
	}
}

void render(SDL_Renderer *renderer)
{
	SDL_GetRendererOutputSize(renderer, &board.width, &board.height);

	/* Board position and size */
	board.size = MIN(board.width, board.height);
	int x = (board.width - board.size) >> 1;
	int y = (board.height - board.size) >> 1;

	drawboard(renderer, x, y, board.size);
	drawpieces(renderer, x, y, board.size >> 3);
}

