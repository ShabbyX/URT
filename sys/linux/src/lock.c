/*
 * Copyright (C) 2013-2014  Shahbaz Youssefi <ShabbyX@gmail.com>
 *
 * This file is part of URT, Unified Real-Time Interface.
 *
 * URT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * URT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with URT.  If not, see <http://www.gnu.org/licenses/>.
 */

/* pull in newer features of POSIX */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <urt_internal.h>
#include <urt_lock.h>
#include <urt_mem.h>
#include "names.h"
#include "mem_internal.h"
#include "time_internal.h"

/* in POSIX systems, the native semaphore type is the urt_sem type */
static urt_sem *urt_global_sem;

urt_sem *(urt_sem_new)(unsigned int init_value, int *error, ...)
{
	urt_sem *sem = urt_mem_new(sizeof *sem, error);
	if (sem == NULL)
		goto exit_fail;

	if (sem_init(&sem->sem, 0, init_value))
		goto exit_bad_init;

	sem->sem_ptr = &sem->sem;
	return sem;
exit_bad_init:
	if (error)
		*error = errno;
	urt_mem_delete(sem);
	goto exit_fail;
exit_fail:
	return NULL;
}

void urt_sem_delete(urt_sem *sem)
{
	if (sem != NULL)
		sem_destroy(&sem->sem);
	urt_mem_delete(sem);
}

static urt_sem *_shsem_common(const char *name, unsigned int init_value, int *error, int flags)
{
	char n[URT_SYS_NAME_LEN];
	urt_sem *sem = urt_mem_new(sizeof *sem, error);
	if (sem == NULL)
		goto exit_fail;

	if (urt_convert_name(n, name))
		goto exit_bad_name;

	sem->sem_ptr = sem_open(n, flags, S_IRWXU | S_IRWXG | S_IRWXO, init_value);
	if (sem->sem_ptr == SEM_FAILED)
		goto exit_bad_open;

	return sem;
exit_bad_open:
	if (error)
		*error = errno;
	goto exit_fail;
exit_bad_name:
	if (error)
		*error= EINVAL;
	goto exit_fail;
exit_fail:
	urt_mem_delete(sem);
	return NULL;
}

int urt_global_sem_get(const char *name)
{
	int error;
	urt_global_sem = _shsem_common(name, 1, &error, O_CREAT);
	return urt_global_sem?0:error;
}

urt_sem *urt_sys_shsem_new(const char *name, unsigned int init_value, int *error)
{
	return _shsem_common(name, init_value, error, O_CREAT | O_EXCL);
}

urt_sem *urt_sys_shsem_attach(const char *name, int *error)
{
	return _shsem_common(name, 0, error, 0);	/* TODO: I expect 0 for flags to only try to attach and not create. Must be tested */
}

void urt_global_sem_free(const char *name)
{
	sem_close(urt_global_sem->sem_ptr);
	/* Note: it's easier if urt_global_sem is never `sem_unlink`ed */
	urt_mem_delete(urt_global_sem);
	urt_global_sem = NULL;
}

void urt_global_sem_wait(void)
{
	urt_sem_wait(urt_global_sem);
}

void urt_global_sem_try_wait(void)
{
	urt_sem_try_wait(urt_global_sem);
}

void urt_global_sem_post(void)
{
	urt_sem_post(urt_global_sem);
}

static void _shsem_detach(struct urt_registered_object *ro)
{
	char n[URT_SYS_NAME_LEN];
	urt_sem *sem = ro->address;

	sem_close(sem->sem_ptr);
	if (ro->count == 0 && urt_convert_name(n, ro->name) == 0)
		sem_unlink(n);
	urt_mem_delete(sem);
}

void urt_shsem_detach(urt_sem *sem)
{
	urt_registered_object *ro;

	if (sem == NULL)
		return;

	ro = urt_get_object_by_id(sem->id);
	if (ro == NULL)
		return;
	urt_deregister(ro, sem, ro->size, _shsem_detach, NULL);
}

int (urt_sem_waitf)(urt_sem *sem, bool (*stop)(volatile void *), volatile void *data, ...)
{
	int res;
	if (stop)
	{
		struct timespec tp;
		urt_time t = urt_get_time_epoch();

		do
		{
			if (stop(data))
				return ECANCELED;

			t += URT_LOCK_STOP_MAX_DELAY;
			tp.tv_sec = t / 1000000000ll;
			tp.tv_nsec = t % 1000000000ll;
		} while ((res = sem_timedwait(sem->sem_ptr, &tp)) == -1 && errno == ETIMEDOUT);
	}
	else
		/* if sem_wait interrupted, retry */
		while ((res = sem_wait(sem->sem_ptr)) == -1 && errno == EINTR);

	return res == 0?0:errno;
}

int urt_sem_timed_wait(urt_sem *sem, urt_time max_wait)
{
	int res;
	struct timespec tp;
	urt_time t = urt_get_time_epoch();

	t += max_wait;
	tp.tv_sec = t / 1000000000ll;
	tp.tv_nsec = t % 1000000000ll;

	while ((res = sem_timedwait(sem->sem_ptr, &tp)) == -1 && errno == EINTR);

	return res == 0?0:errno;
}

int urt_sem_try_wait(urt_sem *sem)
{
	/* note: sem_trywait sets errno to EAGAIN if not locked, unlike EBUSY as with pthread_*_*lock */
	return sem_trywait(sem->sem_ptr) == 0?0:errno == EAGAIN?EBUSY:errno;
}

int urt_sem_post(urt_sem *sem)
{
	return sem_post(sem->sem_ptr);
}

/*
 * allocation and deallocation of pthread_*_t locks are similar, although the function names are different.
 * These functions are refactored here
 */
static void *_pthread_lock_new(int (*init)(void *l, int *e, int f), size_t size, int *error)
{
	void *lock = urt_mem_new(size, error);
	if (lock == NULL)
		goto exit_fail;

	if (init(lock, error, PTHREAD_PROCESS_PRIVATE))
		goto exit_bad_init;

	return lock;
exit_bad_init:
	urt_mem_delete(lock);
exit_fail:
	return NULL;
}

static void _pthread_lock_delete(void (*destroy)(void *l), void *lock)
{
	if (lock != NULL)
		destroy(lock);
	urt_mem_delete(lock);
}

static void *_pthread_shlock_new(int (*init)(void *l, int *e, int f), size_t size, const char *name, int *error)
{
#if !defined(_POSIX_THREAD_PROCESS_SHARED) || _POSIX_THREAD_PROCESS_SHARED <= 0
	if (error)
		*error = ENOTSUP;
	return NULL;
#else
	/*
	 * Note: take care of the book keeping space since I'm doing shared memory
	 * bypassing the size known in new_common.c
	 */
	void *lock = urt_sys_shmem_new(name, size + 16, error);
	if (lock == NULL)
		goto exit_fail;
	lock = (void *)((char *)lock + 16);

	if (init(lock, error, PTHREAD_PROCESS_SHARED))
		goto exit_bad_init;

	/* Note: once returned, the book keeping in new_common.c will again add the +16 */
	return (void *)((char *)lock - 16);
exit_bad_init:
	/* Note: urt_shmem_delete will remove the +16 */
	urt_shmem_delete(lock);
exit_fail:
	return NULL;
#endif
}

static void _pthread_shlock_detach(void (*destroy)(void *l), size_t size, void *lock)
{
	/* Note: unmap needs the correct allocated size of memory */
	urt_registered_object *ro;

	if (lock == NULL)
		return;

	ro = urt_get_object_by_id(*(unsigned int *)((char *)lock - 16));
	if (ro == NULL)
		return;
	ro->size = size + 16;
	urt_shmem_detach_with_callback(lock, destroy);
}

static int _shmutex_init(void *lock, int *error, int flags)
{
	int err;
	urt_mutex *mutex = lock;
	pthread_mutexattr_t attr;

	pthread_mutexattr_init(&attr);
	pthread_mutexattr_setpshared(&attr, flags);
	pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);

	if ((err = pthread_mutex_init(mutex, &attr)))
		goto exit_bad_init;

	pthread_mutexattr_destroy(&attr);
	return 0;
exit_bad_init:
	if (error)
		*error = err;
	pthread_mutexattr_destroy(&attr);
	return -1;
}

static void _shmutex_destroy(void *mutex)
{
	while (pthread_mutex_destroy(mutex) == EBUSY)
		if (pthread_mutex_unlock(mutex))
			break;
}

urt_mutex *(urt_mutex_new)(int *error, ...)
{
	return _pthread_lock_new(_shmutex_init, sizeof(urt_mutex), error);
}

void urt_mutex_delete(urt_mutex *mutex)
{
	_pthread_lock_delete(_shmutex_destroy, mutex);
}

urt_mutex *urt_sys_shmutex_new(const char *name, int *error)
{
	return _pthread_shlock_new(_shmutex_init, sizeof(urt_mutex), name, error);
}

urt_mutex *urt_sys_shmutex_attach(const char *name, int *error)
{
	return urt_sys_shmem_attach(name, error);
}

void urt_shmutex_detach(urt_mutex *mutex)
{
	_pthread_shlock_detach(_shmutex_destroy, sizeof(urt_mutex), mutex);
}

int (urt_mutex_lockf)(urt_mutex *mutex, bool (*stop)(volatile void *), volatile void *data, ...)
{
	int res;
	if (stop)
	{
		struct timespec tp;
		urt_time t = urt_get_time_epoch();

		do
		{
			if (stop(data))
				return ECANCELED;

			t += URT_LOCK_STOP_MAX_DELAY;
			tp.tv_sec = t / 1000000000ll;
			tp.tv_nsec = t % 1000000000ll;
		} while ((res = pthread_mutex_timedlock(mutex, &tp)) == ETIMEDOUT);
	}
	else
		res = pthread_mutex_lock(mutex);

	return res;
}

int urt_mutex_timed_lock(urt_mutex *mutex, urt_time max_wait)
{
	struct timespec tp;
	urt_time t = urt_get_time_epoch();

	t += max_wait;
	tp.tv_sec = t / 1000000000ll;
	tp.tv_nsec = t % 1000000000ll;

	return pthread_mutex_timedlock(mutex, &tp);
}

int urt_mutex_try_lock(urt_mutex *mutex)
{
	return pthread_mutex_trylock(mutex);
}

int urt_mutex_unlock(urt_mutex *mutex)
{
	return pthread_mutex_unlock(mutex);
}

static int _shrwlock_init(void *lock, int *error, int flags)
{
	int err;
	urt_rwlock *rwl = lock;
	pthread_rwlockattr_t attr;

	pthread_rwlockattr_init(&attr);
	pthread_rwlockattr_setpshared(&attr, flags);

	if ((err = pthread_rwlock_init(rwl, &attr)))
		goto exit_bad_init;

	pthread_rwlockattr_destroy(&attr);
	return 0;
exit_bad_init:
	if (error)
		*error = err;
	pthread_rwlockattr_destroy(&attr);
	return -1;
}

static void _shrwlock_destroy(void *rwl)
{
	while (pthread_rwlock_destroy(rwl) == EBUSY)
		if (pthread_rwlock_unlock(rwl))
			break;
}

urt_rwlock *(urt_rwlock_new)(int *error, ...)
{
	return _pthread_lock_new(_shrwlock_init, sizeof(urt_rwlock), error);
}

void urt_rwlock_delete(urt_rwlock *rwl)
{
	_pthread_lock_delete(_shrwlock_destroy, rwl);
}

urt_rwlock *urt_sys_shrwlock_new(const char *name, int *error)
{
	return _pthread_shlock_new(_shrwlock_init, sizeof(urt_rwlock), name, error);
}

urt_rwlock *urt_sys_shrwlock_attach(const char *name, int *error)
{
	return urt_sys_shmem_attach(name, error);
}

void urt_shrwlock_detach(urt_rwlock *rwl)
{
	_pthread_shlock_detach(_shrwlock_destroy, sizeof(urt_rwlock), rwl);
}

int (urt_rwlock_read_lockf)(urt_rwlock *rwl, bool (*stop)(volatile void *), volatile void *data, ...)
{
	int res;
	if (stop)
	{
		struct timespec tp;
		urt_time t = urt_get_time_epoch();

		do
		{
			if (stop(data))
				return ECANCELED;

			t += URT_LOCK_STOP_MAX_DELAY;
			tp.tv_sec = t / 1000000000ll;
			tp.tv_nsec = t % 1000000000ll;
		} while ((res = pthread_rwlock_timedrdlock(rwl, &tp)) == ETIMEDOUT);
	}
	else
		res = pthread_rwlock_rdlock(rwl);

	return res;
}

int (urt_rwlock_write_lockf)(urt_rwlock *rwl, bool (*stop)(volatile void *), volatile void *data, ...)
{
	int res;
	if (stop)
	{
		struct timespec tp;
		urt_time t = urt_get_time_epoch();

		do
		{
			if (stop(data))
				return ECANCELED;

			t += URT_LOCK_STOP_MAX_DELAY;
			tp.tv_sec = t / 1000000000ll;
			tp.tv_nsec = t % 1000000000ll;
		} while ((res = pthread_rwlock_timedwrlock(rwl, &tp)) == ETIMEDOUT);
	}
	else
		res = pthread_rwlock_wrlock(rwl);

	return res;
}

int urt_rwlock_timed_read_lock(urt_rwlock *rwl, urt_time max_wait)
{
	struct timespec tp;
	urt_time t = urt_get_time_epoch();

	t += max_wait;
	tp.tv_sec = t / 1000000000ll;
	tp.tv_nsec = t % 1000000000ll;

	return pthread_rwlock_timedrdlock(rwl, &tp);
}

int urt_rwlock_timed_write_lock(urt_rwlock *rwl, urt_time max_wait)
{
	struct timespec tp;
	urt_time t = urt_get_time_epoch();

	t += max_wait;
	tp.tv_sec = t / 1000000000ll;
	tp.tv_nsec = t % 1000000000ll;

	return pthread_rwlock_timedwrlock(rwl, &tp);
}

int urt_rwlock_try_read_lock(urt_rwlock *rwl)
{
	return pthread_rwlock_tryrdlock(rwl);
}

int urt_rwlock_try_write_lock(urt_rwlock *rwl)
{
	return pthread_rwlock_trywrlock(rwl);
}

int urt_rwlock_read_unlock(urt_rwlock *rwl)
{
	return pthread_rwlock_unlock(rwl);
}

int urt_rwlock_write_unlock(urt_rwlock *rwl)
{
	return pthread_rwlock_unlock(rwl);
}

static int _shcond_init(void *lock, int *error, int flags)
{
	int err;
	urt_cond *cond = lock;
	pthread_condattr_t attr;

	pthread_condattr_init(&attr);
	pthread_condattr_setpshared(&attr, flags);

	if ((err = pthread_cond_init(cond, &attr)))
		goto exit_bad_init;

	pthread_condattr_destroy(&attr);
	return 0;
exit_bad_init:
	if (error)
		*error = err;
	pthread_condattr_destroy(&attr);
	return -1;
}

static void _shcond_destroy(void *cond)
{
	while (pthread_cond_destroy(cond) == EBUSY)
		if (pthread_cond_broadcast(cond))
			break;
}

urt_cond *(urt_cond_new)(int *error, ...)
{
	return _pthread_lock_new(_shcond_init, sizeof(urt_cond), error);
}

void urt_cond_delete(urt_cond *cond)
{
	_pthread_lock_delete(_shcond_destroy, cond);
}

urt_cond *urt_sys_shcond_new(const char *name, int *error)
{
	return _pthread_shlock_new(_shcond_init, sizeof(urt_cond), name, error);
}

urt_cond *urt_sys_shcond_attach(const char *name, int *error)
{
	return urt_sys_shmem_attach(name, error);
}

void urt_shcond_detach(urt_cond *cond)
{
	_pthread_shlock_detach(_shcond_destroy, sizeof(urt_cond), cond);
}

int (urt_cond_waitf)(urt_cond *cond, urt_mutex *mutex, bool (*stop)(volatile void *), volatile void *data, ...)
{
	int res;
	if (stop)
	{
		struct timespec tp;
		urt_time t = urt_get_time_epoch();

		do
		{
			if (stop(data))
				return ECANCELED;

			t += URT_LOCK_STOP_MAX_DELAY;
			tp.tv_sec = t / 1000000000ll;
			tp.tv_nsec = t % 1000000000ll;
		} while ((res = pthread_cond_timedwait(cond, mutex, &tp)) == ETIMEDOUT);
	}
	else
		res = pthread_cond_wait(cond, mutex);

	return res;
}

int urt_cond_timed_wait(urt_cond *cond, urt_mutex *mutex, urt_time max_wait)
{
	struct timespec tp;
	urt_time t = urt_get_time_epoch();

	t += max_wait;
	tp.tv_sec = t / 1000000000ll;
	tp.tv_nsec = t % 1000000000ll;

	return pthread_cond_timedwait(cond, mutex, &tp);
}

int urt_cond_signal(urt_cond *cond)
{
	return pthread_cond_signal(cond);
}

int urt_cond_broadcast(urt_cond *cond)
{
	return pthread_cond_broadcast(cond);
}
