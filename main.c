/* SPDX-License-Identifier: GPL-2.0-only */
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>

#include <pieces.h>
#include <stdio.h>

#define NULLCHECK(x)                                                                                           \
	if (x == NULL) {                                                                                       \
		fprintf(stderr, "%s:%d: %s is null\nSDL Error: %s\n", __FILE__, __LINE__, #x, SDL_GetError()); \
		return 1;                                                                                      \
	}

void render(SDL_Renderer *renderer);
void render_init(SDL_Renderer *renderer);

int main(int argc, char *argv[])
{
	SDL_Window *window =
		SDL_CreateWindow("chmess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 800, SDL_WINDOW_ALLOW_HIGHDPI);
	NULLCHECK(window);

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	NULLCHECK(renderer);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

	SDL_Event event;

	board_init();
	render_init(renderer);

	while (1) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				goto out;
			}
		}

		SDL_RenderClear(renderer);

		render(renderer);

		SDL_RenderPresent(renderer);
	}

out:
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
