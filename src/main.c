#include <err.h>
#include <stdio.h>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>

static int keyevent_handler(SDL_KeyboardEvent *);

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

	while (!done) {
		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_EVENT_QUIT:
					done = 1;
					break;
				case SDL_EVENT_KEY_DOWN:
				case SDL_EVENT_KEY_UP:
					if (keyevent_handler(&event.key))
						done = 1;
					break;
			}
		}
	}

	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
}

static int keyevent_handler(SDL_KeyboardEvent *key) {
	return key->down && key->key == SDLK_ESCAPE;
}
