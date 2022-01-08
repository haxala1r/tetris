#include "headers/tetris.h"
#include <stdlib.h>

/* This file contains functions dealing with piece rotations, movement
 * boards etc.
 */

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
			board->score++;
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

int reset_board(struct board *b) {
	for (int i = 0; i < (b->w * b->h); i++) {
		b->blocks[i] = empty_block;
	}
	make_shape(b, rand() % 7);
}

