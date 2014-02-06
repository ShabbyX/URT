/*
 * Copyright (C) 2013  Shahbaz Youssefi <ShabbyX@gmail.com>
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

int (urt_sem_wait)(urt_sem *sem, bool *stop, ...)
{
	int res;
	if (stop)
	{
		struct timespec tp;
		urt_time t = urt_get_time_epoch();

		do
		{
			if (*stop)
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

void urt_sem_post(urt_sem *sem)
{
	sem_post(sem->sem_ptr);
}

urt_mutex *urt_sys_shmutex_new(const char *name, int *error)
{
	return urt_sys_shsem_new(name, 1, error);
}

urt_mutex *urt_sys_shmutex_attach(const char *name, int *error)
{
	return urt_sys_shsem_attach(name, error);
}

static int _shrwlock_common(urt_rwlock *rwl, int *error, int flags)
{
	int err;
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

urt_rwlock *(urt_rwlock_new)(int *error, ...)
{
	urt_rwlock *rwl = urt_mem_new(sizeof *rwl, error);
	if (rwl == NULL)
		goto exit_fail;

	if (_shrwlock_common(rwl, error, PTHREAD_PROCESS_PRIVATE))
		goto exit_bad_init;

	return rwl;
exit_bad_init:
	urt_mem_delete(rwl);
exit_fail:
	return NULL;
}

void urt_rwlock_delete(urt_rwlock *rwl)
{
	if (rwl != NULL)
		while (pthread_rwlock_destroy(rwl) == EBUSY)
			if (pthread_rwlock_unlock(rwl))
				break;
	urt_mem_delete(rwl);
}

urt_rwlock *urt_sys_shrwlock_new(const char *name, int *error)
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
	urt_rwlock *rwl = urt_sys_shmem_new(name, sizeof *rwl + 16, error);
	if (rwl == NULL)
		goto exit_fail;
	rwl = (void *)((char *)rwl + 16);

	if (_shrwlock_common(rwl, error, PTHREAD_PROCESS_SHARED))
		goto exit_bad_init;

	/* Note: once returned, the book keeping in new_common.c will again add the +16 */
	return (void *)((char *)rwl - 16);
exit_bad_init:
	/* Note: urt_shmem_delete will remove the +16 */
	urt_shmem_delete(rwl);
exit_fail:
	return NULL;
#endif
}

urt_rwlock *urt_sys_shrwlock_attach(const char *name, int *error)
{
	return urt_sys_shmem_attach(name, error);
}

static void _shrwlock_detach(void *rwl)
{
	while (pthread_rwlock_destroy(rwl) == EBUSY)
		if (pthread_rwlock_unlock(rwl))
			break;
}

void urt_shrwlock_detach(urt_rwlock *rwl)
{
	/* Note: unmap needs the correct allocated size of memory */
	urt_registered_object *ro;

	if (rwl == NULL)
		return;

	ro = urt_get_object_by_id(*(unsigned int *)((char *)rwl - 16));
	if (ro == NULL)
		return;
	ro->size = sizeof *rwl + 16;
	urt_shmem_detach_with_callback(rwl, _shrwlock_detach);
}

int (urt_rwlock_read_lock)(urt_rwlock *rwl, bool *stop, ...)
{
	int res;
	if (stop)
	{
		struct timespec tp;
		urt_time t = urt_get_time_epoch();

		do
		{
			if (*stop)
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

int (urt_rwlock_write_lock)(urt_rwlock *rwl, bool *stop, ...)
{
	int res;
	if (stop)
	{
		struct timespec tp;
		urt_time t = urt_get_time_epoch();

		do
		{
			if (*stop)
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
