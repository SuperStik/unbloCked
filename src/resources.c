#include <err.h>
#include <fcntl.h>
#include <unistd.h>

#include <SDL3/SDL_filesystem.h>

#include "resources.h"

struct UBLC_fs UBLC_fs = {-1, -1};

__attribute__((constructor)) static void resource_init(void) {
	const char *path = SDL_GetBasePath();
	if (path == NULL)
		errx(2, "%s", SDL_GetError());

	int base = open(path, O_SEARCH | O_DIRECTORY);
	if (base < 0)
		err(2, "open: %s", path);

	int res_fd = openat(base, "resources", O_SEARCH | O_DIRECTORY);
	if (res_fd < 0)
		err(2, "openat: %i %s", base, "resources");

	UBLC_fs.resources = res_fd;

	if (close(base))
		warn("close: %i", base);

	char *pref = SDL_GetPrefPath("devStik", "unbloCked");
	if (pref == NULL)
		errx(2, "%s", SDL_GetError());

	int pref_fd = open(pref, O_SEARCH | O_DIRECTORY);
	if (pref_fd < 0)
		err(2, "open: %s", pref);

	UBLC_fs.pref = pref_fd;

	SDL_free(pref);
}

/* fds are closed on exit */
/*
__attribute__((destructor)) static void resource_close(void) {
	if (close(UBLC_fs.resources))
		warn("close: %i", UBLC_fs.resources);

	if (close(UBLC_fs.pref))
		warn("close: %i", UBLC_fs.pref);
}
*/
