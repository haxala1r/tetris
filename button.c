#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include "headers/tetris.h"
#include "headers/button.h"

/* We store all buttons in a linked list */
struct button *first_button;


int create_button(char *name, int x, int y) {
	struct button *n = malloc(sizeof(*n));
	if (n == NULL) return -1;

	n->name = name;
	n->x = x;
	n->y = y;
	n->w = BUTTON_DEF_W;
	n->h = BUTTON_DEF_H;
	n->state = 0;

	n->next = first_button;
	first_button = n;

	return 0;
}

int button_get_state(char *name) {
	struct button *i = first_button;
	while (i != NULL) {
		if (!strcmp(i->name, name)) {
			/* Found the button */
			return i->state;
		}
		i = i->next;
	}
	return -1;
}

/* UGH... collision checking.  */
int click_button(int x, int y) {
	struct button *i = first_button;
	while (i != NULL) {
		if ((x > i->x) && (x < (i->x + i->w))) {
			if ((y > i->y) && (y < (i->y + i->h))) {
				/* Found the button it's clicking */
				i->state ^= 1;
			}
		}
		i = i->next;
	}
	return 0;
}

static int blue_color[3] = { 0, 120, 255 };
static int red_color[3] = { 255, 0, 80 };

void draw_buttons(SDL_Renderer *r) {
	struct button *i = first_button;
	while (i != NULL) {
		int *color = red_color;
		if (i->state) {
			color = blue_color;
		}

		struct SDL_Rect rect;
		rect.x = i->x;
		rect.y = i->y;
		rect.w = i->w;
		rect.h = i->h;

		SDL_SetRenderDrawColor(r, color[0], color[1], color[2], 0);
		SDL_RenderFillRect(r, &rect);
		draw_text(r, i->name, i->x + i->w/4, i->y + i->h/4, i->w/2, i->h/2);

		i = i->next;
	}
}


