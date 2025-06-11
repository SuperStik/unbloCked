#define GL_GLEXT_PROTOTYPES 1
#include <err.h>
#include <math.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include "anon_sem.h"
#include "character/zombie.h"
#include "chronos.h"
#include "gutl.h"
#include "level/chunk.h"
#include "level/levelrenderer.h"
#include "level/level.h"
#include "resources.h"

struct threadinfo {
	anon_sem_t swapsem;
	SDL_Window *window;
	SDL_GLContext gl_context;
};

static void *render(void *sem);
static void *tick(void *_);
static void *framerate(void *_);

static int translatekey(SDL_Keycode);
static void movecameratoplayer(void);
static void setupcamera(void);

static int keyevent_down_handler(SDL_KeyboardEvent *, SDL_Window *);
static void keyevent_up_handler(SDL_KeyboardEvent *, SDL_Window *);

static void mousemotionevent_handler(SDL_MouseMotionEvent *, SDL_Window *);
static void mousedown_handler(SDL_MouseButtonEvent *, SDL_Window *);

static void setupfog(int fog);

static void buildshaders(unsigned *levelsh);

static size_t frames = 0;

static uint32_t swapwindow;

struct {
	float w;
	float h;
} winsize = {640.0f, 480.0f};

struct {
	int w;
	int h;
} pixels;

static char done = 0;

static struct UBLC_player player;

#define ZOMBIE_COUNT 10
static struct UBLC_zombie zombies[ZOMBIE_COUNT];

int main(void) {
	srand48(time(NULL));
	puts("Hello unbloCked!");
	SDL_Window *window;

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
			SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	window = SDL_CreateWindow("unbloCked", 640, 480, SDL_WINDOW_OPENGL |
			SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY |
			SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_INPUT_FOCUS);
	if (window == NULL)
		errx(2, "%s", SDL_GetError());

	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	if (gl_context == NULL)
		errx(2, "%s", SDL_GetError());

	SDL_GL_MakeCurrent(window, gl_context);

	if (!SDL_GL_SetSwapInterval(-1))
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

	if (!SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_SYSTEM_SCALE, "1"))
		warnx("%s", SDL_GetError());

	swapwindow = SDL_RegisterEvents(1);
	if (swapwindow == 0)
		errx(2, "No user events left", NULL);

	UBLC_levelrenderer_initstatic();
	UBLC_zombie_initstatic();

	if (UBLC_level_new(256, 256, 64))
		return 2;

	UBLC_player_init(&player);

	struct threadinfo info = {.window = window, .gl_context = gl_context};
	if (anon_sem_init(&info.swapsem, 1))
		err(2, NULL);

	pthread_t threads[3];
	pthread_create(&(threads[0]), NULL, render, &info);
	pthread_create(&(threads[1]), NULL, tick, NULL);
	pthread_create(&(threads[2]), NULL, framerate, NULL);

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
				case SDL_EVENT_MOUSE_BUTTON_DOWN:
					mousedown_handler(&event.button,
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
					break;
				case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
					pixels.w = event.window.data1;
					pixels.h = event.window.data2;
					break;
			}
		}
	}

	anon_sem_post(&(info.swapsem));

	for (int i = 0; i < 3; ++i)
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

	SDL_GL_MakeCurrent(window, gl_context);
	UBLC_levelrenderer_destroystatic();
	UBLC_zombie_destroystatic();

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

static void movecameratoplayer(void) {
	glTranslatef(0.0f, 0.0f, -0.3f);

	float pitch, yaw;
	UBLC_entity_getangles(&(player.ent), &pitch, &yaw);

	glRotatef(pitch, 1.0f, 0.0f, 0.0f);
	glRotatef(yaw, 0.0f, 1.0f, 0.0f);

	struct UBLC_entity_pos pos;
	UBLC_entity_getrenderpos(&player.ent, &pos);

	float d = UBLC_chronos_getdelta();

	float x = pos.xo + (pos.x - pos.xo) * d;
	float y = pos.yo + (pos.y - pos.yo) * d;
	float z = pos.zo + (pos.z - pos.zo) * d;

	glTranslatef(-x, -y, -z);
}

static void setupcamera(void) {
	glMatrixMode(GL_PROJECTION);

	float matrix[16];
	GUTL_perspectivef(matrix, 90.0f, winsize.w / winsize.h, 0.05f, 1000.0f);
	glLoadMatrixf(matrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	movecameratoplayer();
}

static void *render(void *i) {
	struct threadinfo *info = i;
	anon_sem_t *swapsem = &(info->swapsem);
	SDL_GL_MakeCurrent(info->window, info->gl_context);
	pthread_setname_np("unbloCked.render");

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

	/*glPolygonMode(GL_FRONT, GL_LINE);*/

	pthread_set_qos_class_self_np(QOS_CLASS_USER_INTERACTIVE, 1);

	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE, GL_EXP);

	int w = 0;
	int h = 0;

	unsigned levelsh;
	buildshaders(&levelsh);

	while (!done) {
		anon_sem_wait(swapsem);

		if (__builtin_expect(w != pixels.w || h != pixels.h, 0)) {
			w = pixels.w;
			h = pixels.h;
			glViewport(0, 0, w, h);
		}

		glUseProgram(levelsh);
		warnx("%u", levelsh);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		setupcamera();

		glEnable(GL_CULL_FACE);

		glEnable(GL_POLYGON_SMOOTH);

		setupfog(1);
		UBLC_levelrenderer_render(&player, 1);

		setupfog(0);
		UBLC_levelrenderer_render(&player, 0);

		glDisable(GL_TEXTURE_2D);

		for (int i = 0; i < ZOMBIE_COUNT; ++i)
			UBLC_zombie_render(zombies + i);

		glDisable(GL_POLYGON_SMOOTH);

		__atomic_add_fetch(&frames, 1, __ATOMIC_RELAXED);

		event.user.timestamp = SDL_GetTicks();
		SDL_PushEvent(&event);
	}

	return NULL;
}

static void *tick(void *_) {
	(void)_;

	pthread_setname_np("unbloCked.tick");
	pthread_set_qos_class_self_np(QOS_CLASS_USER_INITIATED, 0);

	for (int i = 0; i < ZOMBIE_COUNT; ++i)
		UBLC_zombie_init(zombies + i, 128.0f, 0.0f, 128.0f);

	while (!done) {
		UBLC_chronos_setstart();

		UBLC_player_tick(&player);
		for (int i = 0; i < ZOMBIE_COUNT; ++i)
			UBLC_zombie_tick(zombies + i);

		UBLC_chronos_sleeprate(20);
	}

	for (int i = 0; i < ZOMBIE_COUNT; ++i)
		UBLC_zombie_delete(zombies + i);

	return NULL;
}

static void *framerate(void *_) {
	pthread_setname_np("unbloCked.fpscount");

	while (!done) {
		size_t fps;
		size_t updates;
		fps = __atomic_exchange_n(&frames, 0ul, __ATOMIC_RELAXED);
		updates = __atomic_exchange_n(&UBLC_chunk_updates, 0ul,
				__ATOMIC_RELAXED);

		fprintf(stderr, "%zu fps, %zu\n", fps, updates);

		sleep(1u);
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

			break;
		case SDLK_RETURN:
			if (key->down) {
				UBLC_level_save();
			}

			break;
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
		case SDLK_V:
			if (!key->down || key->repeat)
				break;

			int plykey = translatekey(key->key);
			UBLC_player_setkeys(&player, plykey);

			break;
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
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
			;
			int plykey = translatekey(key->key);
			UBLC_player_unsetkeys(&player, plykey);
			break;
	}
}

static void mousemotionevent_handler(SDL_MouseMotionEvent *motion,
		SDL_Window *window) {
	UBLC_entity_turn(&(player.ent), motion->xrel, motion->yrel);
}

static void mousedown_handler(SDL_MouseButtonEvent *button, SDL_Window *window)
{
	unsigned x, y, z, f;
	switch(button->button) {
		case SDL_BUTTON_LEFT:
			pthread_rwlock_rdlock(&(player.ent.lock));

			if (!player.hasselect) {
				pthread_rwlock_unlock(&(player.ent.lock));
				return;
			}

			x = player.xb;
			y = player.yb;
			z = player.zb;

			pthread_rwlock_unlock(&(player.ent.lock));

			UBLC_level_settile(x, y, z, 0);

			break;
		case SDL_BUTTON_RIGHT:
			pthread_rwlock_rdlock(&(player.ent.lock));

			if (!player.hasselect) {
				pthread_rwlock_unlock(&(player.ent.lock));
				return;
			}

			x = player.xb;
			y = player.yb;
			z = player.zb;
			f = player.placeface;

			pthread_rwlock_unlock(&(player.ent.lock));

			switch(f) {
				case 0:
					--y;
					break;
				case 1:
					++y;
					break;
				case 2:
					--z;
					break;
				case 3:
					++z;
					break;
				case 4:
					--x;
					break;
				case 5:
					++x;
					break;
			}

			if (x >= UBLC_level_width || y >= UBLC_level_depth || z
					>= UBLC_level_height)
				return;

			UBLC_level_settile(x, y, z, 1);

			break;
	}
}

static void setupfog(int fog) {
	switch(fog) {
		case 0:
			glFogf(GL_FOG_DENSITY, 0.001f);

			const float color0[4] = {
				254.0f / 255.0f,
				251.0f / 255.0f,
				250.0f / 255.0f,
				1.0f
			};
			glFogfv(GL_FOG_COLOR, color0);

			glDisable(GL_LIGHTING);
			break;
		case 1:
			glFogf(GL_FOG_DENSITY, 0.06f);

			const float color1[4] = {
				14.0f / 255.0f,
				11.0f / 255.0f,
				10.0f / 255.0f,
				1.0f
			};
			glFogfv(GL_FOG_COLOR, color1);

			glEnable(GL_LIGHTING);
			glEnable(GL_COLOR_MATERIAL);

			const float light[4] = {0.6f, 0.6f, 0.6f, 1.0f};
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light);
			break;
	}
}

#define LEVEL_VERTSOURCE "shaders/level.vert"
#define LEVEL_FRAGSOURCE "shaders/level.frag"
static void buildshaders(unsigned *levelsh) {
	int lvfd = openat(UBLC_fs.resources, LEVEL_VERTSOURCE, O_RDONLY);
	if (lvfd < 0)
		err(2, "openat: %i %s", UBLC_fs.resources, LEVEL_VERTSOURCE);


	int lffd = openat(UBLC_fs.resources, LEVEL_FRAGSOURCE, O_RDONLY);
	if (lffd < 0)
		err(2, "openat: %i %s", UBLC_fs.resources, LEVEL_FRAGSOURCE);

	unsigned levelvert = GUTL_loadshaderfd(GL_VERTEX_SHADER, lvfd);
	unsigned levelfrag = GUTL_loadshaderfd(GL_FRAGMENT_SHADER, lffd);

	close(lvfd);
	close(lffd);

	unsigned prog = glCreateProgram();
	glAttachShader(prog, levelvert);
	glAttachShader(prog, levelfrag);
	if (GUTL_linkandcheckprog(prog))
		exit(1);

	glDeleteShader(levelvert);
	glDeleteShader(levelfrag);

	*levelsh = prog;
}
