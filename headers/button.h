#ifndef BUTTON_H
#define BUTTON_H 1

#define BUTTON_DEF_W 100
#define BUTTON_DEF_H 70

#include <SDL2/SDL.h>

struct button {
	char *name;
	int x, y;
	int w, h;
	int state;

	struct button *next;
};

/* Creates a new button */
int create_button(char *, int, int);

/* Checks if a click triggers any buttons, and if so, triggers
 * said button
 */
int click_button(int, int);

/* Gets the current state of the button */
int button_get_state(char *);

/* Draws all buttons to the screen */
void draw_buttons(SDL_Renderer *r);



#endif
