#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "hashmap.h"

__attribute__((const))
static inline size_t maxof(size_t a, size_t b) {
	return a > b ? a : b;
}

/* FNV-1a hash */
__attribute__((const))
static inline uint64_t hash(const char *str) {
	uint64_t hash = 0xcbf29ce484222325; /* FNV-1a 64-bit basis */
	while (*str) {
		hash *= 0x100000001b3; /* FNV-1a 64-bit prime */
		hash ^= *str;
		++str;
	}
	return hash;
}

struct hashmap *HMAP_create(size_t count, void *fallback) {
	if (count == 0) {
		errno = EINVAL;
		return NULL;
	}

	/* optimal flexible array allocation */
	size_t size = maxof(sizeof(struct hashmap), offsetof(struct hashmap,
				entries) + sizeof(struct hashentry) *
			count);
	struct hashmap *map;
	if (NULL == (void*)0)
		map = calloc(1, size);
	else
		map = malloc(size);

	if (map == NULL)
		return NULL;

	map->fallback = fallback;
	map->count = count;
	if (NULL != (void*)0) {
		for (size_t i = 0; i < count; ++i) {
			map->entries[i].key = NULL;
			map->entries[i].value = NULL;
			map->entries[i].used = 0;
		}
	}
	return map;
}

void HMAP_clean(struct hashmap *map) {
	if (map == NULL)
		return;

	for (size_t i = 0; i < map->count; ++i) {
		free(map->entries[i].key);
		map->entries[i].key = NULL;
		map->entries[i].value = NULL;
		map->entries[i].used = 0;
	}
}

void HMAP_destroy(struct hashmap *map) {
	HMAP_clean(map);
	free(map);
}

void *HMAP_get(const struct hashmap *restrict map, const char *restrict key) {
	if (map == NULL || key == NULL) {
		errno = EINVAL;
		return NULL;
	}

	void *value = map->fallback;
	uint64_t keyhash = hash(key) % map->count;
	for (size_t i = 0; i < map->count; ++i) {
		if (map->entries[keyhash].used) {
			int cmp = strcmp(map->entries[keyhash].key, key);
			if (cmp == 0)
				value = map->entries[keyhash].value;
		} else
			break;
		keyhash = (keyhash + 1) % map->count;
	}

	if (value == map->fallback)
		errno = EINVAL;

	return value;
}

int HMAP_add(struct hashmap *restrict map, const char *restrict key,
		void *restrict value) {
	if (map == NULL || key == NULL) {
		errno = EINVAL;
		return -1;
	}

	if (key[0] == '\0') {
		errno = EINVAL;
		return -1;
	}	

	uint64_t keyhash = hash(key) % map->count;
	for (size_t i = 0; i < map->count; ++i) {
		if (!(map->entries[keyhash].used)) {
			char *dupkey = strdup(key);

			if (dupkey == NULL)
				return -1;
			map->entries[keyhash].key = dupkey;
			map->entries[keyhash].value = value;
			map->entries[keyhash].used = 1;
			return 0;
		}
		keyhash = (keyhash + 1) % map->count;
	}

	errno = ENOMEM;
	return -1;
}
