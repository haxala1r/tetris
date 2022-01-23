#ifndef TETRIS_H
#define TETRIS_H

#include <SDL2/SDL.h>

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
	/* This stuff makes it easier to rotate a shape */
	int box_width;
	int box_x, box_y;

	/* Current coordinates of the blocks making  up the shape */
	int blocks[4][2];
};

struct board {
	struct block *blocks;
	/* Dimensions of the board in blocks  */
	int w, h;
	/* Dimensions of each block */
	int bw, bh;

	struct shape cur_shape;

	int bag[7];
	int cur_piece;
	int held_piece;
	
	/* Incremented every time a line is cleared */
	int score;
};

struct move {
	int x_offset;
	int rotation;
	int value;
};

static struct block empty_block  = { .color = { 255, 255, 255 } };
static struct block orange_block = { .color = { 255, 142,   0 } };
static struct block cyan_block   = { .color = {   0, 180, 255 } };
static struct block blue_block   = { .color = {   0,   0, 255 } };
static struct block yellow_block = { .color = { 255, 255,   0 } };
static struct block green_block  = { .color = {   0, 255,   0 } };
static struct block purple_block = { .color = { 180,   0, 255 } };
static struct block red_block    = { .color = { 255,   0,   0 } };

/* Creates a shape on the given board */
int make_shape(struct board *board, int shape);
int get_next_shape(struct board *b);
int hold_piece(struct board *b);

/* Rotates the current shape of the board
 * once, 90 degrees to the right.
 */
int rotate_shape(struct board *board);

int move_shape(struct board *board, int x_offset, int y_offset);
int reset_board(struct board *board);
int hard_drop(struct board *b);

void draw_board(SDL_Renderer *r, struct board *b, int x, int y);
void draw_held(SDL_Renderer *r, struct board *b, int x, int y);
void draw_text(SDL_Renderer *,char *, int x, int y, int w, int h);

#endif
