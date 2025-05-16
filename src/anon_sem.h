#ifndef ANON_SEM_H
#define ANON_SEM_H 1

/* dealing with the fact that Apple has noncompliant anon semaphores */
/* this "library" is just a wrapper around the real stuff */
/* also yes, I, Davis Wood, made this */
/* please don't dock me points, it still uses posix style anon semaphores on
 * compliant systems */

#ifdef __APPLE__
/* apple's semaphores done through gcd */
#	include <dispatch/dispatch.h>
typedef dispatch_semaphore_t anon_sem_t;
#else /* __APPLE__ */
/* hope for the best that the system has POSIX anon semaphores */
#	include <semaphore.h>
typedef sem_t anon_sem_t;
#endif /* __APPLE__ */

int anon_sem_init(anon_sem_t *, unsigned value);

int anon_sem_post(anon_sem_t *);

int anon_sem_wait(anon_sem_t *);

int anon_sem_destroy(anon_sem_t *);

#endif /* ANON_SEM_H */
