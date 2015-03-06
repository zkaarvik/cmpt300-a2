/*
 * NAME, etc.
 *
 * sync.c
 *
 */

#define _REENTRANT

#include "sync.h"

#define INIT_DELAY 2
#define MAX_DELAY 4096

/*
 * Spinlock routines
 */

int my_spinlock_init(my_spinlock_t *lock)
{
	lock->isLocked = 0;
	lock->recursion_count = 0;
	lock->owner = 0;
	return 0;
}

int my_spinlock_destroy(my_spinlock_t *lock)
{
	//nothing to destroy
}

int my_spinlock_unlock(my_spinlock_t *lock)
{
	lock->recursion_count--;
	if(lock->recursion_count == 0)
	{
		lock->isLocked = 0;
		lock->owner = 0;
	}
	return 0;
}

int my_spinlock_lockTAS(my_spinlock_t *lock)
{
	pthread_t cur_thread_id = pthread_self();

	//If the owner relocks, increase resursion count
	if(pthread_equal(lock->owner, cur_thread_id))
	{
		lock->recursion_count++;
		return 0;
	}

	//Non-owner thread trying to lock
	while(tas(&lock->isLocked)) {}

	//Thread aquires lock for the first time
	lock->owner = cur_thread_id;
	lock->recursion_count = 1;

	return 0;
}


int my_spinlock_lockTTAS(my_spinlock_t *lock)
{
	pthread_t cur_thread_id = pthread_self();

	//If the owner relocks, increase resursion count
	if(pthread_equal(lock->owner, pthread_self()))
	{
		lock->recursion_count++;
		return 0;
	}

	while(1)
	{
		while(lock->isLocked == 0)
		{
			if(!tas(&lock->isLocked)) {
				//Thread aquires lock for the first time
				lock->owner = cur_thread_id;
				lock->recursion_count = 1;

				return 0;
			}
		}
	}
}

int my_spinlock_trylock(my_spinlock_t *lock)
{
	//Try lock, if obtained return 0, if not return -1
	pthread_t cur_thread_id = pthread_self();

	//If the owner relocks, increase resursion count
	if(pthread_equal(lock->owner, cur_thread_id))
	{
		lock->recursion_count++;
		return 0;
	}

	//Non-owner thread trying to lock
	if(tas(&lock->isLocked)) {
		return -1;
	}

	//Thread aquires 
	lock->owner = cur_thread_id;
	lock->recursion_count = 1;

	return 0;
}


/*
 * Mutex routines
 */

int my_mutex_init(my_mutex_t *mutex)
{
	mutex->isLocked = 0;
	mutex->recursion_count = 0;
	mutex->owner = 0;
	return 0;
}

int my_mutex_destroy(my_mutex_t *mutex)
{
	//nothing to destroy
}

int my_mutex_unlock(my_mutex_t *mutex)
{
	mutex->recursion_count--;
	if(mutex->recursion_count == 0)
	{
		mutex->isLocked = 0;
		mutex->owner = 0;
	}
	return 0;
}

int my_mutex_lockTAS(my_mutex_t *mutex)
{
	pthread_t cur_thread_id = pthread_self();
	int cur_delay = INIT_DELAY;

	//If the owner relocks, increase resursion count
	if(pthread_equal(mutex->owner, cur_thread_id))
	{
		mutex->recursion_count++;
		return 0;
	}

	//Non-owner thread trying to lock
	while(tas(&mutex->isLocked)) {
		usleep(cur_delay);
		if(cur_delay < MAX_DELAY) cur_delay *= 2;
	}

	//Thread aquires lock for the first time
	mutex->owner = cur_thread_id;
	mutex->recursion_count = 1;

	return 0;
}


int my_mutex_lockTTAS(my_mutex_t *mutex)
{
	pthread_t cur_thread_id = pthread_self();
	int cur_delay = INIT_DELAY;

	//If the owner relocks, increase resursion count
	if(pthread_equal(mutex->owner, pthread_self()))
	{
		mutex->recursion_count++;
		return 0;
	}

	while(1)
	{
		while(mutex->isLocked == 0)
		{
			if(!tas(&mutex->isLocked)) {
				//Thread aquires lock for the first time
				mutex->owner = cur_thread_id;
				mutex->recursion_count = 1;

				return 0;
			}
			usleep(cur_delay);
			if(cur_delay < MAX_DELAY) cur_delay *= 2;
		}
	}
}

int my_mutex_trylock(my_mutex_t *mutex)
{
	pthread_t cur_thread_id = pthread_self();

	//If the owner relocks, increase resursion count
	if(pthread_equal(mutex->owner, cur_thread_id))
	{
		//printf("Already locked.. \ttid: %02x\n", cur_thread_id);
		mutex->recursion_count++;
		return 0;
	}

	//Non-owner thread trying to lock
	if(tas(&mutex->isLocked)) {
		return -1;
	}

	//Thread aquires lock for the first time
	mutex->owner = cur_thread_id;
	mutex->recursion_count = 1;

	return 0;
}


