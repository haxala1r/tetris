#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "headers/tetris.h"
#include "headers/ai.h"

#define abs(x) ((x > 0) ? (x) : -(x))

/* This file controls the bot */

/* Copies the contents of b to nb. nb and nb->blocks must be allocated
 * before the function is called, for optimization reasons.
 * It is simply too slow to allocate another board and free it in every
 * iteration in the loop of get_best_move(), so we just reuse the same
 * memory each time.
 */
int copy_board(struct board *nb, struct board *b) {
	struct block *tmp = nb->blocks;
	memcpy(nb, b, sizeof(struct board));
	nb->blocks = tmp;
	memcpy(nb->blocks, b->blocks, nb->w * nb->h * sizeof(struct block));
	return 0;
}


/* Check if a coordinate is part of the current shape */
int check_shape_coordinate(struct board *b, int x, int y) {
	for (int i = 0; i < 4; i++) {
		if ((x == b->cur_shape.blocks[i][0]) && (y == b->cur_shape.blocks[i][1])) return 1;
	}
	return 0;
}

/* This is currently unused. I haven't yet decided if I'll add this as an extra
 * modifier later on, so this is just unused code for now.
 */
int get_bumpiness(struct board *b) {
	int max_bump = 0;
	int prev_height = 0;
	for (int i = 0; i < b->w; i++) {
		int cur_height = 0;
		for (int j = 0;  j < b->h; j++) {
			if (check_shape_coordinate(b, i, j)) continue;
			if (memcmp(b->blocks + i + j * b->w, &empty_block, sizeof(struct block))) {
				cur_height = b->h - j;
				break;
			}
		}
		int cur_bump = abs(cur_height - prev_height);
		if (cur_bump > max_bump) max_bump = cur_bump;
		prev_height = cur_height;
	}
	return max_bump;
}

/* Check if a given block is a surrounded empty space */
int check_if_surrounded_space(struct board *b, int x, int y) {
	if (memcmp(b->blocks + x + y * b->w, &empty_block, sizeof(struct block))) {
		return 0;
	}

	/* Check left */
	if ((x != 0) && (!memcmp(b->blocks + x - 1 + y * b->w, &empty_block, sizeof(struct block)))) {
		return 0;
	}

	if ((x != (b->w - 1)) && (!memcmp(b->blocks + x + 1 + y * b->w, &empty_block, sizeof(struct block)))) {
		return 0;
	}
	return 1;
}

/* Counts the amount of surrounded pillars that are taller than 2 blocks. */
int get_pillars(struct board *b) {
	int pillar_count = 0;
	for (int i = 0; i < b->w; i++) {
		int space_count = 0;
		for (int j = b->h - 1; j >= 0; j--) {
			if (space_count >= 2) {
				pillar_count++;
			}
			if (check_shape_coordinate(b, i, j)) continue;
			if (check_if_surrounded_space(b, i, j)) {
				space_count++;
			} else {
				space_count = 0;
			}
		}
	}
	return pillar_count;
}

/* Returns the maximum height of the blocks in the given board */
int get_height(struct board *b) {
	int max_h = 0;
	for (int i = 0; i < b->w; i++) {
		int cur_height = 0;
		for (int j = 0; j < b->h; j++) {
			if (check_shape_coordinate(b, i, j)) continue;
			if (memcmp(b->blocks + i + j * b->w, &empty_block, sizeof(struct block))) {
				cur_height = b->h - j;
				break;
			}
		}
		if (cur_height > max_h) {
			max_h = cur_height;
		}
	}
	return max_h;
}


/* This counts all holes. A hole is defined as any empty block with at least one block above it. */
int get_holes(struct board *b) {
	int holes = 0;
	for (int i = 0; i < b->w; i++) {
		int block_found = 0;
		for (int j = 0; j < b->h; j++) {
			if (check_shape_coordinate(b, i, j)) continue;
			if (memcmp(b->blocks + i + j * b->w, &empty_block, sizeof(struct block))) {
				block_found = 1;
			} else if (block_found) {
				holes++;
			}
		}
	}
	return holes;
}

/* Get the amount of blocks in the right-most column */
int get_right_blocks(struct board *b) {
	int blocks = 0;

	for (int i = 0; i < b->h; i++) {
		if (memcmp(b->blocks + i * b->w + b->w - 1, &empty_block, sizeof(struct block))) {
			blocks++;
		}
	}
	return blocks;
}

double evaluate_board(struct AI *ai, struct board *b, double piece_height) {
	/* Count the number of holes. Each hole is -2 points.  */
	double holes = get_holes(b);

	/* Moves are punished if they're too high */
	double max_height = get_height(b);

	double pillar_count = get_pillars(b);

	double right_blocks = get_right_blocks(b);

	double score = holes * ai->modifiers[0]
		    + (max_height + piece_height) * ai->modifiers[1]
		    + pillar_count * ai->modifiers[2]
		    + right_blocks * ai->modifiers[3]
		    + b->score * 0;
	return score;
}

/* Doesn't drop the piece. Keep in mind */
void apply_move(struct board *b, struct move *m) {
	/* Move to the left side of the board */
	while (1) {
		int stat = move_shape(b, -1, 0);
		if (stat == 0) continue;
		if (stat == 1) return;
		break;
	}
	/* Bring the piece to the desired rotation */
	for (int i = 0; i < m->rotation; i++) {
		rotate_shape(b);
	}
	/* Bring the piece to the desired point on the X axis */
	move_shape(b, m->x_offset, 0);
}

struct move *get_best_move(struct AI *ai, struct board *b) {
	/* First, generate all possible board positions from the current one
	 * by moving the piece from the left to the right end of the board,
	 * and hard-dropping it from each position.
	 */
	double best_value = -0xFFFFFF;
	struct move *move = malloc(sizeof(*move));

	struct board *cb = malloc(sizeof(*b));
	cb->blocks = malloc(b->w * b->h * sizeof(struct block));
	for (int i = 0; i < b->w; i++) {
		for (int j = 0; j < 4; j++) {
			/* i represents the current x of the piece.
			 * j represents the rotation of the piece.
			 * Make a copy of the board, then move the shape to x=i
			 */
			copy_board(cb, b);

			struct move m = { .x_offset = i, .rotation = j };
			apply_move(cb, &m);

			/* Hard_drop also returns the height of the piece */
			int piece_height = hard_drop(cb);
			if (piece_height == -1) {
				/* This indicates that the move results in a loss for the player.
				 * We don't need to evalue it if it results in loss.
				 * This also means that the initial value of 0xFFFFFF for best_value
				 * won't be changed if all moves result in loss.
				*/
				continue;
			}

			/* Determine the value of this position. If it is greater than best_value,
			 * save the move needed to reach the current board state.
			 */
			double val = evaluate_board(ai, cb, (double)piece_height);
			if (val > best_value) {
				best_value = val;
				move->x_offset = i;
				move->rotation = j;
				move->value = best_value;
			}
		}
	}
	free(cb->blocks);
	free(cb);

	/* Return NULL if ALL possible moves result in loss. */
	if (best_value == -0xFFFFFF) {
		free(move);
		return NULL;
	}
	return move;
}

/* NOTE: The initial modifiers I gave for the main AI were { -10, -1, -2 }
 * but after a little bit of training, I reached the modifiers of { -9.5, -1.9, -2.5}
 * but after more, and efficient training I reached { -10.1, -2.5, -2.7, -4.4 }
 * Since training takes a long time, it's currently off-limits and we just start with these
 * hard-coded modifiers insted.
 * TODO: maybe add the option to train? You can currently still train the ai by
 * calling train_ai() from main, but maybe it would be better to make it a togglable option
 */
struct AI main_ai = { .modifiers = { -10.1, -2.5, -2.7, -4.4 } };

/* Returns NULL when the AI loses */
struct move *ai_pick_move(struct AI *ai, struct board *b) {
	if (ai == NULL) ai = &main_ai;

	/* Calculate the end positions for both the current and the held piece  */
	struct move *m1 = get_best_move(ai, b);
	if (m1 == NULL) return NULL;

	hold_piece(b);
	struct move *m2 = get_best_move(ai, b);
	if (m2 == NULL) return NULL;

	/* Return whichever one is better */
	struct move *final_move = m1;
	if (m2->value > m1->value) {
		final_move = m2;
		free(m1);
	} else {
		hold_piece(b);
		free(m2);
	}
	return final_move;
}

/* Randomly mutates the given AI and returns a new one */
struct AI randomly_mutate(struct AI *ai) {
	double mutation_rate = 0.1;
	struct AI new;
	new.score = 0;
	for (int i = 0; i < 4; i++) {
		int sign = rand() % 2;
		double mod = (double)(rand() % 11) * mutation_rate;
		new.modifiers[i] = ai->modifiers[i] + (sign ? -mod : mod);
	}
	return new;
}

/* Runs a single AI on a test board until it dies, and returns its score */
int test_ai(struct AI *ai) {
	int score;
	struct board *b = malloc(sizeof(*b));
	b->blocks = malloc(sizeof(struct block) * 20 * 10);
	b->w = 10;
	b->h = 20;
	b->bw = b->bh = 10;
	reset_board(b);

	while (1) {
		struct move *m;
		if ((m = ai_pick_move(ai, b)) == NULL) {
			break;
		} else {
			apply_move(b, m);
			hard_drop(b);
			free(m);
		}
		score = b->score;
	}
	return score;
}

/* Takes a list of all AIs, calls test_ai on all of them in parallel, and
 * writes their scores into the structs
 */
void score_ais(struct AI *ais, int ai_count) {
	int pids[10];
	int pipes[10][2];

	/* This loops over every AI, simulates them, and records their scores */
	for (int j = 0; j < 10; j++) {
		pipe(pipes[j]);

		/* We run each AI in a seperate process. */
		pids[j] = fork();

		if (pids[j] == 0) {
			close(pipes[j][0]); /* Close the read end of the pipe */
			int score = test_ai(ais + j);
			printf("AI #%d finished its run. Score: %d Modifiers: { %f, %f, %f, %f}\n", j, score, ais[j].modifiers[0], ais[j].modifiers[1], ais[j].modifiers[2], ais[j].modifiers[3]);

			/* We return the score to the main process via the pipe.
			 */
			write(pipes[j][1], &score, 4);
			exit(0);
		} else {
			close(pipes[j][1]); /* close the write end of the pipe */
		}
	}

	/* Wait for all processes to complete */
	for (int j = 0; j < 10; j++) {
		int status = 0;
		int pid = wait(&status);
		for (int k = 0; k < 10; k++) {
			if (pids[k] == pid) {
				int score;
				read(pipes[k][0], &score, 4);
				close(pipes[k][0]);
				printf("Recorded status of AI #%d as %d\n", k, score);
				ais[k].score = score;
				break;
			}
		}
	}
}


/* This... This is the ultimate function.
 * This is the concentration of all the pain and misery I felt during this project.
 * This is the function in which we determine the BEST AI AMONG THEM ALL.
 *
 * ... This is where we create the ultimate life form.
 *
 * Jokes aside, here's a summary of how it works:
 * This function essentially generates a bunch of AIs (sets of modifiers to use in evaluate_board)
 * and tests them all on an empty board. In each cycle there are 10 AIs, and
 * the best of them is chosen at the end of each cycle. Then, for the next cycle
 * the last best AI is randomly mutated 10 times to generate more AIs, and this repeats
 * until we have a really good AI (hopefully?). Normally, there should be more AIs per
 * cycle for there to be any "evolution" at all, but honestly? it took my computer
 * like 4 hours to run a single cycle of this, and I'm not a fan of that.
 *
 * NOTE: all of this code is currently unused. This is due to the fact that
 * the modifiers that I discovered recently seem to go practically
 * forever into the game, and if the game doesn't end, there's no genetic algorithm
 * to speak of. Okay, it doesn't go on literally forever, but this function is so
 * under-optimized that it takes AGES to actually "train" anything.
 *
 * Maybe I'll make it an option to train the AI in the future, if I ever optimize this thing
 * enough to the point it doesn't take FOUR HOURS TO RUN A SINGLE FUCKING CYCLE.
 * Multiprocessing can help with that, I hope.
 *
 * In the meantime, the current default modifiers should be enough to impress my friends
 * and family, at least.
 */
void train_ai(void) {
	/* Allocate space for some ai. We will be reusing this array  */
	struct AI *ais = malloc(sizeof(struct AI) * 10);

	/* This is the main loop. We have 10 cycles, or "generations" */
	for (int i = 0; i < 10; i++) {
		/* Now randomly mutate a bunch of AIs into the ais array */
		for (int j = 0; j < 10; j++) {
			ais[j] = randomly_mutate(&main_ai);
		}

		score_ais(ais, 10);

		/* Now find the best AI and replace main_ai with that */
		int best_index = 0;
		struct AI *best = NULL;
		for (int j = 0; j < 10; j++) {
			if ((best == NULL) || (best->score < ais[j].score)) {
				best = ais + j;
				best_index = j;
			}
		}
		if (best->score > main_ai.score) {
			printf("loop #%d, best: %d, score: %d, modifiers changed to: %f %f %f\n", i, best_index, best->score, best->modifiers[0], best->modifiers[1], best->modifiers[2]);
			memcpy(&main_ai, best, sizeof(struct AI));
		} else {
			printf("loop #%d, no changes made\n", i);
		}
	}
	/* and finally... WE HAVE THE PERFECT LIFE FORM... hopefully. */
}

