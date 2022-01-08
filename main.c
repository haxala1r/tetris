#include <SDL2/SDL.h> 
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "headers/tetris.h"

struct block empty_block  = { .color = { 255, 255, 255 } };
struct block orange_block = { .color = { 255, 142,   0 } };
struct block cyan_block   = { .color = {   0, 180, 255 } };
struct block blue_block   = { .color = {   0,   0, 255 } };
struct block yellow_block = { .color = { 255, 255,   0 } };
struct block green_block  = { .color = {   0, 255,   0 } };
struct block purple_block = { .color = { 180,   0, 255 } };
struct block red_block    = { .color = { 255,   0,   0 } };

void make_shape(struct board *board, int shape) {
	switch (shape) {
	case SHAPE_I:
		for (int i = 0; i < 4; i++) {
			int x = board->w/2, y = i;
			board->cur_shape.blocks[i][0] = x;
			board->cur_shape.blocks[i][1] = y;
			board->blocks[y * board->w + x] = cyan_block;
		}
		board->cur_shape.box_width = 4;
		board->cur_shape.box_x = board->w/2 - 2;
		board->cur_shape.box_y = 0;
		break;
	case SHAPE_J:
		for (int i = 0; i < 3; i++) {
			int x = board->w/2, y = i;
			board->cur_shape.blocks[i][0] = x;
			board->cur_shape.blocks[i][1] = y;
			board->blocks[y * board->w + x] = blue_block;
		}
		board->blocks[board->w/2 + 1] = blue_block;
		board->cur_shape.blocks[3][0] = board->w/2 + 1;
		board->cur_shape.blocks[3][1] = 0;
		board->cur_shape.box_width = 3;
		board->cur_shape.box_x = board->w/2 - 1;
		board->cur_shape.box_y = 0;
		break;
	case SHAPE_L:
		for (int i = 0; i < 3; i++) {
			int x = board->cur_shape.blocks[i][0] = board->w/2;
			int y = board->cur_shape.blocks[i][1] = i;
			board->blocks[y * board->w + x] = orange_block;
		}
		board->blocks[board->w/2 + 1 + 2 * board->w] = orange_block;
		board->cur_shape.blocks[3][0] = board->w/2 + 1;
		board->cur_shape.blocks[3][1] = 2;
		board->cur_shape.box_width = 3;
		board->cur_shape.box_x = board->w/2 - 1;
		board->cur_shape.box_y = 0;
		break;
	case SHAPE_O:
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				int x = board->cur_shape.blocks[i * 2 + j][0] = board->w/2 + i;
				int y = board->cur_shape.blocks[i * 2 + j][1] = j;
				
				board->blocks[x + y * board->w] = yellow_block;
			}
		}
		board->cur_shape.box_width = 2;
		board->cur_shape.box_x = board->w/2;
		board->cur_shape.box_y = 0;
		break;
	case SHAPE_S:
		for (int i = 0; i < 4; i++) {
			int x = board->cur_shape.blocks[i][0] = board->w/2 + i/2;
			int y = board->cur_shape.blocks[i][1] = i/2 + i%2;
			board->blocks[x + y * board->w] = green_block;
		}
		board->cur_shape.box_width = 3;
		board->cur_shape.box_x = board->w/2 - 1;
		board->cur_shape.box_y = 0;
		break;
	case SHAPE_T:
		for (int i = 0; i < 3; i++) {
			int x = board->cur_shape.blocks[i][0] = board->w/2;
			int y = board->cur_shape.blocks[i][1] = i;
			board->blocks[x + y * board->w] = purple_block;
		}
		board->blocks[board->w/2 + 1 + board->w] = purple_block;
		board->cur_shape.blocks[3][0] = board->w/2 + 1;
		board->cur_shape.blocks[3][1] = 1;

		board->cur_shape.box_width = 3;
		board->cur_shape.box_x = board->w/2 - 1;
		board->cur_shape.box_y = 0;
		break;
	case SHAPE_Z:
		for (int i = 0; i < 4; i++) {
			int x = board->cur_shape.blocks[i][0] = board->w/2 + i/2;
			int y = board->cur_shape.blocks[i][1] = 2 - (i + 1)/2;
			board->blocks[x + y * board->w] = red_block;
		}
		board->cur_shape.box_width = 3;
		board->cur_shape.box_x = board->w/2 - 1;
		board->cur_shape.box_y = 0;
		break;
	default:
		return;
	}
}

int rotate_shape(struct board *board) {
	if (board->cur_shape.box_x < 0) {
		move_shape(board, -board->cur_shape.box_x, 0);
	} 
	if ((board->cur_shape.box_x + board->cur_shape.box_width) > board->w) {
		move_shape(board, -1, 0);
	}
	struct block tmp; /* Store the shape while copy/pasting it */
	/* Source coordinate array  */
	int source[4][2];
	/* Target coordinate array  */
	int target[4][2];
	for (int i = 0; i < 4; i++) {
		int x = board->cur_shape.blocks[i][0];
		int y = board->cur_shape.blocks[i][1];
		source[i][0] = x;
		source[i][1] = y;
		
		/* Empty source coordinates */
		tmp = board->blocks[x + y * board->w];
		board->blocks[x + y * board->w] = empty_block; 

		target[i][1] = (x - board->cur_shape.box_x) + board->cur_shape.box_y;
		target[i][0] = (board->cur_shape.box_width - 1) - (y - board->cur_shape.box_y);
		target[i][0] += board->cur_shape.box_x;
	}
	/* Check the target area */
	for (int i = 0; i < 4; i++) {
		if (memcmp(board->blocks + target[i][0] + target[i][1] * board->w, &empty_block, sizeof(struct block))) goto fail;
	}
	/* Copy block to target */
	for (int i = 0; i < 4; i++) {
		board->blocks[target[i][0] + target[i][1] * board->w] = tmp;
		board->cur_shape.blocks[i][0] = target[i][0];
		board->cur_shape.blocks[i][1] = target[i][1];
	}
	return 0;
fail:
	for (int i = 0; i < 4; i++) {
		int x = board->cur_shape.blocks[i][0];
		int y = board->cur_shape.blocks[i][1];
		board->blocks[x + y * board->w] = tmp;
	}
	printf("??");
	return 0;
}

/* Line checks are done automatically when a piece collides with anything */
static int check_clears(struct board *board) {
	for (int i = board->h - 1; i >= 0; i--) {
		int j = 0;
		for (; j < board->w; j++) {
			if (!memcmp(board->blocks + i * board->w + j, &empty_block, sizeof(struct block))) break;
		}
		if (j == board->w) {
			/* A line clear!!! */
			for (int k = i - 1; k >= 0; k--) {
				for (j = 0; j < board->w; j++) {
					board->blocks[(k + 1) * board->w + j] = board->blocks[k * board->w + j];
				}
			}
			for (j = 0; j < board->w; j++) {
				board->blocks[j] = empty_block;
			}
			i = board->h;
		}
	}
}

/* Moves the current shape down by one block */
int move_shape(struct board *board, int x_offset, int y_offset) {
	/* TODO: fix this to be more like move_shape.x (i.e. make it work) */
	struct block tmp; /* Store the block */ 
	int target[4][2];
	/* Check if coordinates are out of bounds */
	for (int i = 0; i < 4; i++) {
		int x = board->cur_shape.blocks[i][0];
		int y = board->cur_shape.blocks[i][1];
		if (((x + x_offset) < 0) || ((x + x_offset) >= board->w)) return -1;
		if ((y + y_offset) < 0) return -1;
		tmp = board->blocks[x + y * board->w];
		if ((y + y_offset) >= board->h) goto collision;
	}
	/* Momentarily delete piece. We need to do this,
	 * because when checking whether the target area is empty
	 * there is a chance the target area may overlap with the current
	 * area.
	 */
	for (int i = 0; i < 4; i++) {
		int x = board->cur_shape.blocks[i][0];
		int y = board->cur_shape.blocks[i][1];
		target[i][0] = x + x_offset;
		target[i][1] = y + y_offset;
		tmp = board->blocks[x + y * board->w];
		board->blocks[x + y * board->w] = empty_block;
	}
	/* Check if target is empty */
	for (int i = 0; i < 4; i++) {
		if (memcmp(board->blocks + target[i][0] + target[i][1] * board->w, &empty_block, sizeof(struct block))) {
			goto collision;
		}
	}
	/* if so, paste piece to target */
	for (int i = 0; i < 4; i++) {
		board->cur_shape.blocks[i][0] = target[i][0];
		board->cur_shape.blocks[i][1] = target[i][1];
		board->blocks[target[i][0] + target[i][1] * board->w] = tmp;
	}
	
	board->cur_shape.box_x += x_offset;
	board->cur_shape.box_y += y_offset;
	return 0;
collision:
	/* restore piece */
	for (int i = 0; i < 4; i++) {
		int x = board->cur_shape.blocks[i][0];
		int y = board->cur_shape.blocks[i][1];
		board->blocks[x + y * board->w] = tmp;
	}
	/* First, check for line clears */
	check_clears(board);
	/* Immediately create new shape. */
	make_shape(board, rand() % 7);
	return 0;
}

void draw_board(SDL_Renderer *r, struct board *board, int x, int y) {
	if (board == NULL) return;
	struct SDL_Rect in_rect, outer_rect;
	in_rect.w = board->bw - 2;
	in_rect.h = board->bh - 2;
	outer_rect.w = board->bw;
	outer_rect.h = board->bh;
	for (int i = 0; i < board->h; i++) {
		for (int j = 0; j < board->w; j++) {
			struct block *b = board->blocks + i * board->w + j;
			outer_rect.x = x + board->bw * j;
			in_rect.x = x + board->bw * j + 1;
			outer_rect.y = y + board->bh * i;
			in_rect.y = y + board->bh * i + 1;
			
			SDL_SetRenderDrawColor(r, 180, 180, 180, 0);
			SDL_RenderFillRect(r, &outer_rect);
			SDL_SetRenderDrawColor(r, b->color[0], b->color[1], b->color[2], 0);
			SDL_RenderFillRect(r, &in_rect);
		}
	}
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

		/* Draw shit? */
		draw_board(renderer, &tetris_board, board_x, board_y);

		SDL_RenderPresent(renderer);
		SDL_Delay(10);
		cur_tick++;
	}
quit:
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	return 0;
}
