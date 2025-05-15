#include <err.h>
#include <stdio.h>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_video.h>

static int keyevent_handler(SDL_KeyboardEvent *, SDL_Window *);
static void mousemotionevent_handler(SDL_MouseMotionEvent *, SDL_Window *);

int main(void) {
	puts("Hello unbloCked!");
	SDL_Window *window;
	char done = 0;

	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow("unbloCked", 640, 480, SDL_WINDOW_OPENGL |
			SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
	if (window == NULL)
		errx(2, "%s", SDL_GetError());

	if (!SDL_SetWindowMinimumSize(window, 640, 480))
		warnx("%s", SDL_GetError());

	if (!SDL_SetWindowRelativeMouseMode(window, true))
		warnx("%s", SDL_GetError());

	while (!done) {
		SDL_Event event;

		while (!done && SDL_WaitEvent(&event)) {
			switch (event.type) {
				case SDL_EVENT_QUIT:
					done = 1;
					break;
				case SDL_EVENT_KEY_DOWN:
				case SDL_EVENT_KEY_UP:
					if (keyevent_handler(&event.key,
								window))
						done = 1;
					break;
				case SDL_EVENT_MOUSE_MOTION:
					mousemotionevent_handler(&event.motion,
							window);
					break;
			}
		}
	}

	SDL_SetWindowRelativeMouseMode(window, false);
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	w /= 2;
	h /= 2;
	SDL_WarpMouseInWindow(window, w, h);

	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
}

static int keyevent_handler(SDL_KeyboardEvent *key, SDL_Window *window) {
	if (key->down && key->key == SDLK_L) {
		bool relative = SDL_GetWindowRelativeMouseMode(window);
		if (!SDL_SetWindowRelativeMouseMode(window, !relative))
			warnx("%s", SDL_GetError());

		if (relative) {
			int w, h;
			SDL_GetWindowSize(window, &w, &h);
			w /= 2;
			h /= 2;
			SDL_WarpMouseInWindow(window, w, h);
		}
	}

	return key->down && key->key == SDLK_ESCAPE;
}

static void mousemotionevent_handler(SDL_MouseMotionEvent *motion,
		SDL_Window *window) {
	printf("xrel: %g yrel: %g\n", motion->xrel, motion->yrel);
	float x, y;
}
