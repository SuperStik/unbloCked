#include <errno.h>
#include <stddef.h>
#include "anon_sem.h"

int anon_sem_init(anon_sem_t *sem, unsigned value) {
#ifdef __APPLE__
	/* dispatch returns a type-aliased pointer, rather than setting an
	 * object */
	*sem = dispatch_semaphore_create(value);
	if (*sem != NULL)
		return 0;
	else {
		errno = ENOSPC; /* emulating sem_init with what we've got */
		return -1;
	}
#else
	return sem_init(sem, 0, value);
#endif
}

int anon_sem_post(anon_sem_t *sem) {
	/* these are both incredibly similar, which is nice */
#ifdef __APPLE__
	dispatch_semaphore_signal(*sem);
	return 0;
#else
	return sem_post(sem); 
#endif
}

int anon_sem_wait(anon_sem_t *sem) {
#ifdef __APPLE__
	/* we have to specify a time for the semaphore waiting, and for this
	 * one, we make sure it lasts forever, just like the real sem_wait */
	return dispatch_semaphore_wait(*sem, DISPATCH_TIME_FOREVER);
#else
	return sem_wait(sem);
#endif
}

int anon_sem_destroy(anon_sem_t *sem) {
#ifdef __APPLE__
	/* dispatch uses a retain/release system, we only retain it once on
	 * init, so we only call this once (unless some idiot calls
	 * dispatch_release(3) on our object. */
	dispatch_release(*sem);
	/* it also always succeeds, so we always return 0 */
	return 0;
#else
	return sem_destroy(sem);
#endif
}
