#include <SDL2/SDL.h> 
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SHAPE_I 0
#define SHAPE_J 1
#define SHAPE_L 2
#define SHAPE_O 3
#define SHAPE_S 4
#define SHAPE_T 5
#define SHAPE_Z 6


struct block {
	int color[3];
};

struct shape {
	/* This stuff helps to rotate the shape. Otherwise not necessary */
	int cur_direction; 
	int box_width;    /* The dimensions of box containing shape */
	int box_x, box_y; /* the corner of the box containing shape */

	int blocks[4][2];
};


int w=10, h=20;   /* Tetris game sizes */
int bw=20, bh=20; /* Block sizes       */

struct block empty_block = {
	.color = { 255, 255, 255 }
};

struct block orange_block = { .color = { 255, 142,   0 } };
struct block cyan_block   = { .color = {   0, 180, 255 } };
struct block blue_block   = { .color = {   0,   0, 255 } };
struct block yellow_block = { .color = { 255, 255,   0 } };
struct block green_block  = { .color = {   0, 255,   0 } };
struct block purple_block = { .color = { 180,   0, 255 } };
struct block red_block    = { .color = { 255,   0,   0 } };

struct shape cur_shape;

void make_shape(struct block *board, int shape) {
	switch (shape) {
	case SHAPE_I:
		for (int i = 0; i < 4; i++) {
			int x = w/2, y = i;
			cur_shape.blocks[i][0] = x;
			cur_shape.blocks[i][1] = y;
			board[y * w + x] = cyan_block;
		}
		cur_shape.box_width = 4;
		cur_shape.box_x = w/2 - 2;
		cur_shape.box_y = 0;
		break;
	case SHAPE_J:
		for (int i = 0; i < 3; i++) {
			int x = w/2, y = i;
			cur_shape.blocks[i][0] = x;
			cur_shape.blocks[i][1] = y;
			board[y * w + x] = blue_block;
		}
		board[w/2 + 1] = blue_block;
		cur_shape.blocks[3][0] = w/2 + 1;
		cur_shape.blocks[3][1] = 0;
		cur_shape.box_width = 3;
		cur_shape.box_x = w/2 - 1;
		cur_shape.box_y = 0;
		break;
	case SHAPE_L:
		for (int i = 0; i < 3; i++) {
			int x = cur_shape.blocks[i][0] = w/2;
			int y = cur_shape.blocks[i][1] = i;
			board[y * w + x] = orange_block;
		}
		board[w/2 + 1 + 2 * w] = orange_block;
		cur_shape.blocks[3][0] = w/2 + 1;
		cur_shape.blocks[3][1] = 2;
		cur_shape.box_width = 3;
		cur_shape.box_x = w/2 - 1;
		cur_shape.box_y = 0;
		break;
	case SHAPE_O:
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				int x = cur_shape.blocks[i * 2 + j][0] = w/2 + i;
				int y = cur_shape.blocks[i * 2 + j][1] = j;
				
				board[x + y * w] = yellow_block;
			}
		}
		cur_shape.box_width = 2;
		cur_shape.box_x = w/2;
		cur_shape.box_y = 0;
		break;
	case SHAPE_S:
		for (int i = 0; i < 4; i++) {
			int x = cur_shape.blocks[i][0] = w/2 + i/2;
			int y = cur_shape.blocks[i][1] = i/2 + i%2;
			board[x + y * w] = green_block;
		}
		cur_shape.box_width = 3;
		cur_shape.box_x = w/2 - 1;
		cur_shape.box_y = 0;
		break;
	case SHAPE_T:
		for (int i = 0; i < 3; i++) {
			int x = cur_shape.blocks[i][0] = w/2;
			int y = cur_shape.blocks[i][1] = i;
			board[x + y * w] = purple_block;
		}
		board[w/2 + 1 + w] = purple_block;
		cur_shape.blocks[3][0] = w/2 + 1;
		cur_shape.blocks[3][1] = 1;
		cur_shape.box_width = 3;
		cur_shape.box_x = w/2 - 1;
		cur_shape.box_y = 0;
		break;
	case SHAPE_Z:
		for (int i = 0; i < 4; i++) {
			int x = cur_shape.blocks[i][0] = w/2 + i/2;
			int y = cur_shape.blocks[i][1] = 2 - (i + 1)/2;
			board[x + y * w] = red_block;
		}
		cur_shape.box_width = 3;
		cur_shape.box_x = w/2 - 1;
		cur_shape.box_y = 0;
		break;
	default:
		printf("FUCK\n");
		return;
	}
}

int move_shape_x(struct block *, int);

int rotate_shape(struct block *board) {
	if (cur_shape.box_x < 0) {
		move_shape_x(board, -cur_shape.box_x);
	} 
	if ((cur_shape.box_x + cur_shape.box_width) > w) {
		move_shape_x(board, -1);
	}
	struct block tmp; /* Store the shape while copy/pasting it */
	/* Source coordinate array  */
	int source[4][2];
	/* Target coordinate array  */
	int target[4][2];
	for (int i = 0; i < 4; i++) {
		int x = cur_shape.blocks[i][0];
		int y = cur_shape.blocks[i][1];
		source[i][0] = x;
		source[i][1] = y;
		
		/* Empty source coordinates */
		tmp = board[x + y * w];
		board[x + y * w] = empty_block; 

		target[i][1] = (x - cur_shape.box_x) + cur_shape.box_y;
		target[i][0] = (cur_shape.box_width - 1) - (y - cur_shape.box_y);
		target[i][0] += cur_shape.box_x;
	}
	/* Check the target area */
	for (int i = 0; i < 4; i++) {
		if (memcmp(board + target[i][0] + target[i][1] * w, &empty_block, sizeof(*board))) goto fail;
	}
	/* Copy block to target */
	for (int i = 0; i < 4; i++) {
		board[target[i][0] + target[i][1] * w] = tmp;
		cur_shape.blocks[i][0] = target[i][0];
		cur_shape.blocks[i][1] = target[i][1];
	}
	return 0;
fail:
	for (int i = 0; i < 4; i++) {
		int x = cur_shape.blocks[i][0];
		int y = cur_shape.blocks[i][1];
		board[x + y * w] = tmp;
	}
	return 0;
}

int check_clears(struct block *board) {
	for (int i = h - 1; i >= 0; i--) {
		int j = 0;
		for (; j < w; j++) {
			if (!memcmp(board + i * w + j, &empty_block, sizeof(struct block))) break;
		}
		if (j == w) {
			/* A line clear!!! */
			for (int k = i - 1; k >= 0; k--) {
				for (j = 0; j < w; j++) {
					board[(k + 1) * w + j] = board[k * w + j];
				}
			}
			for (j = 0; j < w; j++) {
				board[j] = empty_block;
			}
			i = h;
		}
	}
}

/* Moves the current shape down by one block */
int move_shape(struct block *board) {
	/* TODO: fix this to be more like move_shape.x (i.e. make it work) */
	struct block tmp; /* Store the block */ 
	int target[4][2];
	
	/* Momentarily delete piece. We need to do this,
	 * because when checking whether the target area is empty
	 * there is a chance the target area may overlap with the current
	 * area.
	 */
	for (int i = 0; i < 4; i++) {
		int x =  cur_shape.blocks[i][0];
		int y = cur_shape.blocks[i][1];
		target[i][0] = x;
		target[i][1] = y + 1;
		tmp = board[x + y * w];
		board[x + y * w] = empty_block;
	}
	/* Check if target is empty */
	for (int i = 0; i < 4; i++) {
		if (memcmp(board + target[i][0] + target[i][1] * w, &empty_block, sizeof(struct block))) {
			goto collision;
		}
	}
	/* if so, paste piece to target */
	for (int i = 0; i < 4; i++) {
		cur_shape.blocks[i][0] = target[i][0];
		cur_shape.blocks[i][1] = target[i][1];
		board[target[i][0] + target[i][1] * w] = tmp;
	}
	
	cur_shape.box_y++;
	return 0;
collision:
	/* restore piece */
	for (int i = 0; i < 4; i++) {
		int x = cur_shape.blocks[i][0];
		int y = cur_shape.blocks[i][1];
		board[x + y * w] = tmp;
	}
	/* First, check for line clears */
	check_clears(board);
	/* Immediately create new shape. */
	make_shape(board, rand() % 7);
	return 0;
}

int move_shape_x(struct block *board, int x_offset) {
	/* First, momentarily delete the blocks making up the shape. This makes
	 * comparing and moving easier.
	 */
	struct block temp[4];
	for (int i = 0; i < 4; i++) {
		int x = cur_shape.blocks[i][0];
		int y = cur_shape.blocks[i][1];
		temp[i] = board[y * w + x];
		board[y * w + x] = empty_block;
	}
	/* Check if all target blocks are empty. */
	for (int i = 0; i < 4; i++) {
		int x = cur_shape.blocks[i][0] + x_offset;
		if ((x < 0) || (x >= w)) goto fail;
		int y = cur_shape.blocks[i][1];
		/* Return if the target block isn't empty */
		if (memcmp(board + y * w + x, &empty_block, sizeof(struct block))) goto fail;
	}
	/* Paste the blocks to target */
	for (int i = 0; i < 4; i++) {
		int x = (cur_shape.blocks[i][0] += x_offset);
		int y = cur_shape.blocks[i][1];
		board[y * w + x] = temp[i];
	}
	cur_shape.box_x += x_offset;
	return 0;
fail:
	/* Paste the blocks to their original places */
	for (int i = 0; i < 4; i++) {
		int x = cur_shape.blocks[i][0];
		int y = cur_shape.blocks[i][1];
		board[y * w + x] = temp[i];
	}
	return -1;
}	

void draw_board(SDL_Renderer *r, struct block *board, int x, int y) {
	if (board == NULL) return;
	struct SDL_Rect in_rect, outer_rect;
	in_rect.w = bw - 2;
	in_rect.h = bh - 2;
	outer_rect.w = bw;
	outer_rect.h = bh;
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			struct block *b = board + i * w + j;
			outer_rect.x = x + bw * j;
			in_rect.x = x + bw * j + 1;
			outer_rect.y = y + bh * i;
			in_rect.y = y + bh * i + 1;
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
	struct block *tetris_board = malloc(w * h * sizeof(struct block));
	for (int i = 0; i < (w * h); i++)
		tetris_board[i] = empty_block;
	make_shape(tetris_board, 6);	
	int board_x = (sw / 2) - (w * bw / 2);
	int board_y = (sh / 2) - (h * bh / 2);
	
	unsigned int cur_tick = 0;
	int move_freq = 64; /* Pressing down speeds the falling of the piece */
	SDL_Delay(100);
	SDL_Event event;
	while (1) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT: 
				goto quit;
			case SDL_KEYDOWN:
				/* Handle key presses */
				switch(event.key.keysym.sym) {
				case SDLK_LEFT:
					move_shape_x(tetris_board, -1);
					break;
				case SDLK_RIGHT:
					move_shape_x(tetris_board, 1);
					break;
				case SDLK_DOWN:
					move_freq = 2;
					break;
				case SDLK_UP:
					rotate_shape(tetris_board);
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
			move_shape(tetris_board);
		}

		/* Draw shit? */
		draw_board(renderer, tetris_board, board_x, board_y);

		SDL_RenderPresent(renderer);
		SDL_Delay(10);
		cur_tick++;
	}
quit:
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	return 0;
}
