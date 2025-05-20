#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <mach-o/dyld.h>
#include <string.h>
#include <unistd.h>

#include "resources.h"

int UBLC_resource_fd = -1;

__attribute__((constructor)) static void resource_init(void) {
	char execpath[PATH_MAX];
	uint32_t buflen = PATH_MAX;
	if (_NSGetExecutablePath(execpath, &buflen))
		errc(2, ENOMEM, "_NSGetExecutablePath");

	char path[PATH_MAX];
	dirname_r(execpath, path);
	strlcat(path, "/resources", PATH_MAX);

	int res_fd = open(path, O_SEARCH | O_DIRECTORY);
	if (res_fd < 0)
		err(2, "open: %s", path);

	UBLC_resource_fd = res_fd;
}

/* fds are closed on exit */
/*
__attribute__((destructor)) static void resource_close(void) {
	if (close(UBLC_resource_fd))
		warn("close: %i", UBLC_resource_fd);
}
*/
