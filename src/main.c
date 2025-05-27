#include <err.h>
#include <math.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include "anon_sem.h"
#include "gutl.h"
#include "level/chunk.h"
#include "level/levelrenderer.h"
#include "level/level.h"
#include "timer.h"

struct threadinfo {
	anon_sem_t swapsem;
	SDL_Window *window;
	SDL_GLContext gl_context;
};

static void *render(void *sem);
static void *tick(void *_);

static int translatekey(SDL_Keycode);
static void movecameratoplayer(float a);
static void setupcamera(float a);

static int keyevent_down_handler(SDL_KeyboardEvent *, SDL_Window *);
static void keyevent_up_handler(SDL_KeyboardEvent *, SDL_Window *);

static void mousemotionevent_handler(SDL_MouseMotionEvent *, SDL_Window *);

static uint32_t swapwindow;

struct {
	float w;
	float h;
} winsize = {640.0f, 480.0f};

static char done = 0;

static struct UBLC_player player;

int main(void) {
	srand48(time(NULL));
	puts("Hello unbloCked!");
	SDL_Window *window;

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
			SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
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

	UBLC_player_init(&player);

	struct threadinfo info = {.window = window, .gl_context = gl_context};
	if (anon_sem_init(&info.swapsem, 1))
		err(2, NULL);

	pthread_t threads[2];
	pthread_create(&(threads[0]), NULL, render, &info);
	pthread_create(&(threads[1]), NULL, tick, NULL);

	while (!done) {
		SDL_Event event;

		while (!done && SDL_WaitEvent(&event)) {
			switch (event.type) {
				case SDL_EVENT_QUIT:
					done = 1;
					break;
				case SDL_EVENT_KEY_DOWN:
					if (keyevent_down_handler(&event.key,
								window))
						done = 1;
					break;

				case SDL_EVENT_KEY_UP:
					keyevent_up_handler(&event.key, window);
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
					break;
				case SDL_EVENT_WINDOW_RESIZED:
					winsize.w = (float)event.window.data1;
					winsize.h = (float)event.window.data2;
			}
		}
	}

	anon_sem_post(&(info.swapsem));

	for (int i = 0; i < 2; ++i)
		pthread_join(threads[i], NULL);

	anon_sem_post(&(info.swapsem));

	anon_sem_destroy(&info.swapsem);

	UBLC_player_delete(&player);

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

static int translatekey(SDL_Keycode key) {
	int plykey;
	switch (key) {
		case SDLK_UP:
			plykey = UBLC_KF_UP;
			break;
		case SDLK_DOWN:
			plykey = UBLC_KF_DOWN;
			break;
		case SDLK_LEFT:
			plykey = UBLC_KF_LEFT;
			break;
		case SDLK_RIGHT:
			plykey = UBLC_KF_RIGHT;
			break;
		case SDLK_W:
			plykey = UBLC_KF_W;
			break;
		case SDLK_S:
			plykey = UBLC_KF_S;
			break;
		case SDLK_A:
			plykey = UBLC_KF_A;
			break;
		case SDLK_D:
			plykey = UBLC_KF_D;
			break;
		case SDLK_SPACE:
			plykey = UBLC_KF_SPACE;
			break;
		case SDLK_R:
			plykey = UBLC_KF_R;
			break;
		case SDLK_LSHIFT:
			plykey = UBLC_KF_LSHIFT;
			break;
		case SDLK_RSHIFT:
			plykey = UBLC_KF_RSHIFT;
			break;
		case SDLK_V:
			plykey = UBLC_KF_V;
			break;
		default:
			plykey = 0;
	}

	return plykey;
}

static void movecameratoplayer(float a) {
	glTranslatef(0.0f, 0.0f, -0.3f);

	float pitch, yaw;
	UBLC_player_getangles(&player, &pitch, &yaw);

	glRotatef(pitch, 1.0f, 0.0f, 0.0f);
	glRotatef(yaw, 0.0f, 1.0f, 0.0f);

	float x = player.xo + (player.x - player.xo) * a;
	float y = player.yo + (player.y - player.yo) * a;
	float z = player.zo + (player.z - player.zo) * a;
	glTranslatef(-x, -y, -z);
}

static void setupcamera(float a) {
	glMatrixMode(GL_PROJECTION);

	float matrix[16];
	GUTL_perspectivef(matrix, 90.0f, winsize.w / winsize.h, 0.05f, 1000.0f);
	glLoadMatrixf(matrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	movecameratoplayer(a);
}

static void *render(void *i) {
	struct threadinfo *info = i;
	anon_sem_t *swapsem = &(info->swapsem);
	SDL_GL_MakeCurrent(info->window, info->gl_context);
	pthread_setname_np("unbloCked.renderthread");

	SDL_Event event;
	event.type = SDL_EVENT_USER;
	event.user.windowID = -1;
	event.user.code = swapwindow;
	event.user.data1 = NULL;
	event.user.data2 = NULL;

	const float fogcolor[4] = {
		(14.0f/255.0f),
		(11.0f/255.0f),
		(10.0f/255.0f),
		1.0f
	};

	UBLC_timer_init(60.0f);

	UBLC_chunk_initstatic();

	UBLC_levelrenderer_init();

	/*glPolygonMode(GL_FRONT, GL_LINE);*/

	while (!done) {
		anon_sem_wait(swapsem);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		setupcamera(UBLC_timer_a);

		glEnable(GL_CULL_FACE);

		glEnable(GL_POLYGON_SMOOTH);

		glEnable(GL_FOG);
		glFogi(GL_FOG_MODE, GL_EXP);
		glFogf(GL_FOG_DENSITY, 0.2f);
		glFogfv(GL_FOG_COLOR, fogcolor);
		UBLC_levelrenderer_render(&player, 1);
		glDisable(GL_FOG);
		UBLC_levelrenderer_render(&player, 0);
		glDisable(GL_TEXTURE_2D);

		glDisable(GL_POLYGON_SMOOTH);

		event.user.timestamp = SDL_GetTicks();
		SDL_PushEvent(&event);
	}

	UBLC_levelrenderer_delete();

	return NULL;
}

static void *tick(void *_) {
	(void)_;

	pthread_setname_np("unbloCked.tickthread");

	while (!done) {
		UBLC_timer_advancetime();
		UBLC_player_tick(&player);

		long sleeptime = 999999999 / 60;
		struct timespec sleeping = {.tv_sec = 0, .tv_nsec = sleeptime};
		nanosleep(&sleeping, NULL);
	}

	return NULL;
}

static int keyevent_down_handler(SDL_KeyboardEvent *key, SDL_Window *window) {
	if (!(key->down))
		goto keyevent_ret;

	switch(key->key) {
		case SDLK_L:;
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
			break;
		case SDLK_F11:
			if (key->down) {
				SDL_WindowFlags fl = (SDL_GetWindowFlags(
							window));

				bool fscreen = !(fl & SDL_WINDOW_FULLSCREEN);
				SDL_SetWindowFullscreen(window, fscreen);
			}
		case SDLK_UP:
		case SDLK_DOWN:
		case SDLK_LEFT:
		case SDLK_RIGHT:
		case SDLK_W:
		case SDLK_S:
		case SDLK_A:
		case SDLK_D:
		case SDLK_SPACE:
		case SDLK_R:
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
		case SDLK_V:;
			int plykey = translatekey(key->key);
			if (key->down)
				UBLC_player_setkeys(&player, plykey);
		}

keyevent_ret:
	return key->down && key->key == SDLK_ESCAPE;
}

static void keyevent_up_handler(SDL_KeyboardEvent *key, SDL_Window *window) {
	switch (key->key) {
		case SDLK_UP:
		case SDLK_DOWN:
		case SDLK_LEFT:
		case SDLK_RIGHT:
		case SDLK_W:
		case SDLK_S:
		case SDLK_A:
		case SDLK_D:
		case SDLK_SPACE:
		case SDLK_R:
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
		case SDLK_V:;
			    int plykey = translatekey(key->key);
			    UBLC_player_unsetkeys(&player, plykey);
			    break;
	}

}

static void mousemotionevent_handler(SDL_MouseMotionEvent *motion,
		SDL_Window *window) {
	UBLC_player_turn(&player, motion->xrel, motion->yrel);
}
