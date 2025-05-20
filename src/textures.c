#include <err.h>
#include <stddef.h>

#include <OpenGL/gl.h>

#include "hashmap.h"
#include "textures.h"

struct hashmap *idmap;
size_t texcount;
static unsigned textures[4096];

__attribute__((constructor)) static void idmap_init(void) {
	idmap = HMAP_create(4096, NULL);

	if (idmap == NULL)
		err(2, "HMAP_create", NULL);
}

__attribute__((destructor)) static void idmap_delete(void) {
	HMAP_destroy(idmap);
}

long UBLC_textures_loadtexture(const char *resource, int mode) {
	return -1;
}
