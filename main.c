#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "headers/tetris.h"
#include "headers/ai.h"
#include "headers/button.h"


int get_shape_x(struct board *b) {
	int x = b->w;
	for (int i = 0; i < 4; i++) {
		int cx = b->cur_shape.blocks[i][0];
		if (cx < x) x = cx;
	}
	return x;
}

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
	reset_board(&tetris_board);

	tetris_board.bw = tetris_board.bh = 20;
	int board_x = (sw / 2) - (tetris_board.w * tetris_board.bw / 2);
	int board_y = (sh / 2) - (tetris_board.h * tetris_board.bh / 2);
	
	create_button("AI", 50, 300);
	create_button("SPEED", 50, 380);

	unsigned int cur_tick = 0;
	int move_freq = 64; /* Pressing down speeds the falling of the piece */
	SDL_Delay(100);
	SDL_Event event;

	struct move *cur_move = NULL; /* Used to store the current move when the AI is enabled */ 

	/* Main loop */
	while (1) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				goto quit;
			case SDL_KEYDOWN:
				if (button_get_state("AI") == 1) break;
				int stat = 0;
				/* Handle key presses */
				switch(event.key.keysym.sym) {
				case SDLK_LEFT:
					stat = move_shape(&tetris_board, -1, 0);
					break;
				case SDLK_RIGHT:
					stat = move_shape(&tetris_board, 1, 0);
					break;
				case SDLK_DOWN:
					move_freq = 2;
					break;
				case SDLK_UP:
					rotate_shape(&tetris_board);
					break;
				case SDLK_SPACE:
					hold_piece(&tetris_board);
					break;
				case SDLK_RETURN:
					stat = (hard_drop(&tetris_board) >= 0) ? 0 : 1;
					break;
				default:
					break;
				}
				if (stat) reset_board(&tetris_board);
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
			case SDL_MOUSEBUTTONUP:
				click_button(event.button.x, event.button.y);
				break;
			default:
				break;
			}
		}
		/* Background */
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);
		
		/* This part's a bit complex. This essentially handles the switch between player and
		 * AI. Also handles the smooth movement of the AI
		 */
		if (button_get_state("AI") == 1) {
			if (cur_move == NULL) {
				cur_move = ai_pick_move(NULL, &tetris_board);
				if (cur_move == NULL) {
					printf("AI lost. Final score: %d\n", tetris_board.score);
					reset_board(&tetris_board);
				}
				struct move m = { .x_offset = 0, .rotation = 0 };
				apply_move(&tetris_board, &m);
				
				/* Make the move instantly if the speed button is toggled */
				if (button_get_state("SPEED") == 1) {
					apply_move(&tetris_board, cur_move);
					hard_drop(&tetris_board);
					free(cur_move);
					cur_move = NULL;
				}
			} 
			if (((cur_tick & (7)) == 0) && (cur_move != NULL)) {
				int x_offset = cur_move->x_offset;
				/* First, rotate the shape */
				if (cur_move->rotation == 0) {
					/* Then, move the shape to the desired x value */
					if (x_offset == 0) {
						/* ... Then, move the shape down. */
						if (move_shape(&tetris_board, 0, 1) == 2) {
							free(cur_move);
							cur_move = NULL;
						}
					} else {
						move_shape(&tetris_board, (x_offset > 0) ? 1 : -1, 0);
						cur_move->x_offset--;
					}
				} else {
					rotate_shape(&tetris_board);
					cur_move->rotation--;
				}
			}
		} else {
			if (cur_move != NULL) {
				free(cur_move);
				cur_move = NULL;
			}
			/* Move the piece down */
			if ((cur_tick & (move_freq - 1)) == 0) {
				if (move_shape(&tetris_board, 0, 1) == 1) {
					reset_board(&tetris_board);
				}
			}
		}
		
		
		draw_board(renderer, &tetris_board, board_x, board_y);
		draw_held(renderer, &tetris_board, 50, board_y);	
		draw_buttons(renderer);	
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
