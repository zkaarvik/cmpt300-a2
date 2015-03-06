/*
 * NAME, etc.
 *
 * sync.c
 *
 */

#define _REENTRANT

#include "sync.h"


/*
 * Spinlock routines
 */

int my_spinlock_init(my_spinlock_t *mutex)
{
}

int my_spinlock_destroy(my_spinlock_t *mutex)
{
}

int my_spinlock_unlock(my_spinlock_t *mutex)
{
}

int my_spinlock_lockTAS(my_spinlock_t *mutex)
{
}


int my_spinlock_lockTTAS(my_spinlock_t *mutex)
{
}

int my_spinlock_trylock(my_spinlock_t *mutex)
{
}


/*
 * Mutex routines
 */

int my_mutex_init(my_mutex_t *mutex)
{
}

int my_mutex_destroy(my_mutex_t *mutex)
{
}

int my_mutex_unlock(my_mutex_t *mutex)
{
}

int my_mutex_lockTAS(my_mutex_t *mutex)
{
}


int my_mutex_lockTTAS(my_mutex_t *mutex)
{
}

int my_mutex_trylock(my_mutex_t *mutex)
{
}


