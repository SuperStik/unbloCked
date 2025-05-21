#include <err.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>

#include <OpenGL/gl.h>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>

#include "anon_sem.h"
#include "level/chunk.h"
#include "level/level.h"

struct threadinfo {
	anon_sem_t swapsem;
	SDL_Window *window;
	SDL_GLContext gl_context;
};

static void *render(void *sem);

static int keyevent_handler(SDL_KeyboardEvent *, SDL_Window *);
static void mousemotionevent_handler(SDL_MouseMotionEvent *, SDL_Window *);

static uint32_t swapwindow;
static char done = 0;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

int main(void) {
	puts("Hello unbloCked!");
	SDL_Window *window;

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
			SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	window = SDL_CreateWindow("unbloCked", 640, 480, SDL_WINDOW_OPENGL |
			SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
	if (window == NULL)
		errx(2, "%s", SDL_GetError());

	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	if (gl_context == NULL)
		errx(2, "%s", SDL_GetError());

	SDL_GL_MakeCurrent(window, gl_context);
	SDL_GL_SetSwapInterval(1);

	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);

	glClearColor(0.5f, 0.8f, 1.0f, 0.0f);
	glClearDepth(1.0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	if (!SDL_SetWindowMinimumSize(window, 640, 480))
		warnx("%s", SDL_GetError());

	if (!SDL_SetWindowRelativeMouseMode(window, true))
		warnx("%s", SDL_GetError());

	swapwindow = SDL_RegisterEvents(1);
	if (swapwindow == 0)
		errx(2, "No user events left", NULL);

	if (UBLC_level_new(256, 256, 64))
		return 2;

	struct threadinfo info = {.window = window, .gl_context = gl_context};
	if (anon_sem_init(&info.swapsem, 1))
		err(2, NULL);

	pthread_t renderthread;
	pthread_create(&renderthread, NULL, render, &info);

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
				case SDL_EVENT_USER:
					if (event.type == swapwindow) {
						SDL_GL_SwapWindow(window);
						anon_sem_post(&info.swapsem);
					}
			}
		}
	}

	anon_sem_post(&info.swapsem);

	pthread_join(renderthread, NULL);

	anon_sem_destroy(&info.swapsem);

	UBLC_level_delete();

	SDL_SetWindowRelativeMouseMode(window, false);
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	w /= 2;
	h /= 2;
	SDL_WarpMouseInWindow(window, w, h);

	//SDL_GL_MakeCurrent(window, gl_context);

	SDL_GL_DestroyContext(gl_context);

	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
}

static void *render(void *i) {
	struct threadinfo *info = i;
	anon_sem_t *swapsem = &(info->swapsem);
	SDL_GL_MakeCurrent(info->window, info->gl_context);
	pthread_setname_np("unbloCked.renderthread");

	UBLC_chunk_initstatic();

	while (!done) {
		anon_sem_wait(swapsem);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		SDL_Event event;
		event.type = SDL_EVENT_USER;
		event.user.timestamp = SDL_GetTicks();
		event.user.windowID = -1;
		event.user.code = swapwindow;
		event.user.data1 = NULL;
		event.user.data2 = NULL;
		SDL_PushEvent(&event);
	}

	return NULL;
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

#pragma GCC diagnostic pop
