#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "headers/tetris.h"


int main(void) {
	srand(time(NULL));
	if (SDL_Init(SDL_INIT_VIDEO) < 0) return -1;
	if (TTF_Init() == 1) return -2;

	/* Make these part of the config perhaps?*/
	int sw=600, sh=600; /* Window sizes      */

	SDL_Window *window = SDL_CreateWindow("Tetris", 0, 0, sw, sh, 0);
	if (window == NULL) return -3;
	SDL_Renderer *renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) return -3;

	/* Allocate the board and clear it. */
	struct board tetris_board;
	memset(&tetris_board, 0, sizeof(struct board));

	/* TODO: read the dimensions from a config file */
	tetris_board.w = 10;
	tetris_board.h = 20;

	tetris_board.blocks = malloc(tetris_board.w * tetris_board.h * sizeof(struct block));
	for (int i = 0; i < (tetris_board.w * tetris_board.h); i++)
		tetris_board.blocks[i] = empty_block;
	make_shape(&tetris_board, rand() % 6);

	tetris_board.bw = tetris_board.bh = 20;
	int board_x = (sw / 2) - (tetris_board.w * tetris_board.bw / 2);
	int board_y = (sh / 2) - (tetris_board.h * tetris_board.bh / 2);

	unsigned int cur_tick = 0;
	int move_freq = 64; /* Pressing down speeds the falling of the piece */
	SDL_Delay(100);
	SDL_Event event;

	/* Main loop */
	while (1) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				goto quit;
			case SDL_KEYDOWN:
				/* Handle key presses */
				switch(event.key.keysym.sym) {
				case SDLK_LEFT:
					move_shape(&tetris_board, -1, 0);
					break;
				case SDLK_RIGHT:
					move_shape(&tetris_board, 1, 0);
					break;
				case SDLK_DOWN:
					move_freq = 2;
					break;
				case SDLK_UP:
					rotate_shape(&tetris_board);
					break;
				default:
					break;
				}
				break;
			case SDL_KEYUP:
				switch(event.key.keysym.sym) {
				case SDLK_DOWN:
					move_freq = 64;
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
		}
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);

		/* Advance the game. */
		if ((cur_tick & (move_freq - 1)) == 0) {
			move_shape(&tetris_board, 0, 1);
		}

		draw_board(renderer, &tetris_board, board_x, board_y);
		
		/* Draw the score at the top */
		char s[64];
		sprintf(s, "SCORE: %d", tetris_board.score);
		draw_text(renderer, s, sw/2 - 100, 0, 200, 40);

		SDL_RenderPresent(renderer);
		SDL_Delay(10);
		cur_tick++;
	}
quit:
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();
	return 0;
}
