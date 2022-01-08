#ifndef TETRIS_H
#define TETRIS_H

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


#endif
