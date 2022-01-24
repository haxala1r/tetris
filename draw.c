#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "headers/tetris.h"



/* This file contains mostly graphical functions that help with text, board
 * etc.
 */

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

void draw_held(SDL_Renderer *r, struct board *b, int x, int y) {
	/* Make a new empty board */
	struct board nb;
	nb.w = nb.h = 5;
	nb.bw = nb.bh = b->bw;
	nb.blocks = malloc(nb.w * nb.h * sizeof(struct block));
	for (int i = 0; i < (nb.w * nb.h); i++) {
		nb.blocks[i] = empty_block;
	}

	/* Put the current held piece in the empty board, then display it */
	if (b->held_piece != -1) {
		make_shape(&nb, b->held_piece);
	}
	draw_board(r, &nb, x, y);
	free(nb.blocks);
}

TTF_Font *font;

void draw_text(SDL_Renderer *r, char *s, int x, int y, int w, int h) {
	if (font == NULL) {
		font = TTF_OpenFont("./applestorm.ttf", 25);
		if (font == NULL) return;
	}
	SDL_Color color = {255, 255, 255};
	SDL_Surface *surf = TTF_RenderText_Solid(font, s, color);
	SDL_Texture *text = SDL_CreateTextureFromSurface(r, surf);
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;
	dest.w = w;
	dest.h = h;
	if (SDL_RenderCopy(r, text, NULL, &dest)) {
		printf("err");
	}
	
	SDL_DestroyTexture(text);
	SDL_FreeSurface(surf);
	return;
}

