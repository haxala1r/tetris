#ifndef TETRIS_AI_H
#define TETRIS_AI_H 1

#include "tetris.h"

/* This struct defines the modifiers used in evaluate_board.
 * We need to have this as a struct because we will make lots of 
 * these and try to choose the best one, essentially training the AI.
 */
struct AI {
	double modifiers[3];
	int score;
};

int get_holes(struct board *);
int get_pillars(struct board *);
void apply_move(struct board *, struct move *);
struct move *ai_pick_move(struct AI *ai, struct board *);

void train_ai();


#endif
