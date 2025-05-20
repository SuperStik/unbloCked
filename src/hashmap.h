#ifndef HASHMAP_H
#define HASHMAP_H 1

#include "cxxjunk.h"

BEGIN_DECL

struct hashentry {
	char *key;
	void *value;
	unsigned char used : 1;
};

struct hashmap {
	void *fallback;
	size_t count;
	struct hashentry entries[] __attribute__((counted_by(count)));
};

void HMAP_clean(struct hashmap *);
void HMAP_destroy(struct hashmap *);
__attribute__((malloc))
struct hashmap *HMAP_create(size_t, void *fallback);

void *HMAP_get(const struct hashmap *restrict, const char *restrict key);
int HMAP_add(struct hashmap *restrict, const char *restrict key, void *restrict
		value);

END_DECL

#endif
