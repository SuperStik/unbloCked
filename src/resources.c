#include <err.h>
#include <fcntl.h>
#include <unistd.h>

#include <SDL3/SDL_filesystem.h>

#include "resources.h"

int UBLC_resource_fd = -1;

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

	UBLC_resource_fd = res_fd;

	if (close(base))
		warn("close: %i", base);
}

/* fds are closed on exit */
/*
__attribute__((destructor)) static void resource_close(void) {
	if (close(UBLC_resource_fd))
		warn("close: %i", UBLC_resource_fd);
}
*/
