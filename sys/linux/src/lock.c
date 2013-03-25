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
#include <errno.h>
#include <urt_internal.h>
#include <urt_lock.h>
#include <urt_mem.h>
#include "names.h"
#include "mem_internal.h"
#include "time_internal.h"

urt_sem *(urt_sem_new)(unsigned int init_value, int *error, ...)
{
	urt_sem *sem = urt_mem_new(sizeof(*sem), error);
	if (sem == NULL)
		goto exit_fail;

	if (sem_init(sem, 0, init_value))
		goto exit_bad_init;

	return sem;
exit_bad_init:
	if (error)
	{
		if (errno == EINVAL)
			*error = URT_BAD_VALUE;
		else
			*error= URT_FAIL;
	}
	urt_mem_delete(sem);
	goto exit_fail;
exit_fail:
	return NULL;
}

void urt_sem_delete(urt_sem *sem)
{
	if (URT_LIKELY(sem != NULL))
		sem_destroy(sem);
	urt_mem_delete(sem);
}

static urt_sem *_shsem_common(const char *name, unsigned int init_value, int *error, int flags)
{
	char n[URT_SYS_NAME_LEN];
	urt_sem *sem = NULL;

	if (urt_convert_name(n, name) != URT_SUCCESS)
		goto exit_bad_name;

	sem = sem_open(n, flags, S_IRWXU | S_IRWXG | S_IRWXO, init_value);
	if (sem == SEM_FAILED)
		goto exit_bad_open;

	return sem;
exit_bad_open:
	if (error)
	{
		switch (errno)
		{
		case EEXIST:
			*error = URT_EXISTS;
			break;
		case EINVAL:
			*error = URT_BAD_VALUE;
			break;
		case ENOMEM:
			*error = URT_NO_MEM;
			break;
		default:
			*error = URT_FAIL;
		}
	}
	goto exit_fail;
exit_bad_name:
	if (error)
		*error= URT_BAD_NAME;
	goto exit_fail;
exit_fail:
	return NULL;
}

urt_sem *urt_global_sem_get(const char *name, int *error)
{
	return _shsem_common(name, 1, error, O_CREAT);
}

urt_sem *urt_sys_shsem_new(const char *name, unsigned int init_value, int *error)
{
	return _shsem_common(name, init_value, error, O_CREAT | O_EXCL);
}

void urt_global_sem_free(const char *name)
{
	char n[URT_SYS_NAME_LEN];

	sem_close(urt_global_sem);
	if (urt_convert_name(n, name) == URT_SUCCESS)
		sem_unlink(n);
}

urt_sem *urt_sys_shsem_attach(const char *name, int *error)
{
	return _shsem_common(name, 0, error, 0);	/* TODO: I expect 0 for flags to only try to attach and not create. Must be tested */
}

void urt_shsem_detach(urt_sem *sem)
{
	sem_close(sem);
	urt_deregister_addr(sem);
}

void urt_shsem_delete(urt_sem *sem)
{
	char n[URT_SYS_NAME_LEN];
	urt_registered_object *ro;

	sem_close(sem);
	ro = urt_get_object_by_addr(sem);
	if (ro == NULL)
		return;
	if (urt_convert_name(n, ro->name) == URT_SUCCESS)
		sem_unlink(n);
	urt_deregister(ro);
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
				return URT_NOT_LOCKED;

			t += URT_LOCK_STOP_MAX_DELAY;
			tp.tv_sec = t / 1000000000ll;
			tp.tv_nsec = t % 1000000000ll;
		} while ((res = sem_timedwait(sem, &tp)) == -1 && errno == ETIMEDOUT);
	}
	else
		/* if sem_wait interrupted, retry */
		while ((res = sem_wait(sem)) == -1 && errno == EINTR);

	return res == 0?URT_SUCCESS:URT_FAIL;
}

int urt_sem_timed_wait(urt_sem *sem, urt_time max_wait)
{
	int res;
	struct timespec tp;
	urt_time t = urt_get_time_epoch();

	t += max_wait;
	tp.tv_sec = t / 1000000000ll;
	tp.tv_nsec = t % 1000000000ll;

	while ((res = sem_timedwait(sem, &tp)) == -1 && errno == EINTR);

	if (res == 0)
		return URT_SUCCESS;
	else if (errno == ETIMEDOUT)
		return URT_NOT_LOCKED;
	return URT_FAIL;
}

int urt_sem_try_wait(urt_sem *sem)
{
	if (sem_trywait(sem) == -1)
		return URT_FAIL;
	if (errno == EAGAIN)
		return URT_NOT_LOCKED;
	return URT_SUCCESS;
}

void urt_sem_post(urt_sem *sem)
{
	sem_post(sem);
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
	{
		if (err == EAGAIN)
			*error = URT_AGAIN;
		else if (err == ENOMEM)
			*error = URT_NO_MEM;
		else
			*error= URT_FAIL;
	}
	pthread_rwlockattr_destroy(&attr);
	return -1;
}

urt_rwlock *(urt_rwlock_new)(int *error, ...)
{
	urt_rwlock *rwl = urt_mem_new(sizeof(*rwl), error);
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
	if (URT_LIKELY(rwl != NULL))
		while (pthread_rwlock_destroy(rwl) == EBUSY)
			if (pthread_rwlock_unlock(rwl))
				break;
	urt_mem_delete(rwl);
}

urt_rwlock *urt_sys_shrwlock_new(const char *name, int *error)
{
#if !defined(_POSIX_THREAD_PROCESS_SHARED) || _POSIX_THREAD_PROCESS_SHARED <= 0
	if (error)
		*error = URT_NO_SUPPORT;
	return NULL;
#else
	urt_rwlock *rwl = urt_sys_shmem_new(name, sizeof(*rwl), error);
	if (rwl == NULL)
		goto exit_fail;

	if (_shrwlock_common(rwl, error, PTHREAD_PROCESS_SHARED))
		goto exit_bad_init;

	return rwl;
exit_bad_init:
	urt_shmem_delete(rwl);
exit_fail:
	return NULL;
#endif
}

urt_rwlock *urt_sys_shrwlock_attach(const char *name, int *error)
{
	return urt_sys_shmem_attach(name, error);
}

static void _delete_rwlock_callback(void *mem)
{
	urt_rwlock *rwl = mem;

	while (pthread_rwlock_destroy(rwl) == EBUSY)
		if (pthread_rwlock_unlock(rwl))
			break;
}

void urt_shrwlock_detach(urt_rwlock *rwl)
{
	if (rwl == NULL)
		return;
	urt_shmem_detach_with_callback(rwl, _delete_rwlock_callback);
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
				return URT_NOT_LOCKED;

			t += URT_LOCK_STOP_MAX_DELAY;
			tp.tv_sec = t / 1000000000ll;
			tp.tv_nsec = t % 1000000000ll;
		} while ((res = pthread_rwlock_timedrdlock(rwl, &tp)) == ETIMEDOUT);
	}
	else
		res = pthread_rwlock_rdlock(rwl);

	return res == 0?URT_SUCCESS:URT_FAIL;
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
				return URT_NOT_LOCKED;

			t += URT_LOCK_STOP_MAX_DELAY;
			tp.tv_sec = t / 1000000000ll;
			tp.tv_nsec = t % 1000000000ll;
		} while ((res = pthread_rwlock_timedwrlock(rwl, &tp)) == ETIMEDOUT);
	}
	else
		res = pthread_rwlock_wrlock(rwl);

	return res == 0?URT_SUCCESS:URT_FAIL;
}

int urt_rwlock_timed_read_lock(urt_rwlock *rwl, urt_time max_wait)
{
	int res;
	struct timespec tp;
	urt_time t = urt_get_time_epoch();

	t += max_wait;
	tp.tv_sec = t / 1000000000ll;
	tp.tv_nsec = t % 1000000000ll;

	res = pthread_rwlock_timedrdlock(rwl, &tp);

	if (res == 0)
		return URT_SUCCESS;
	else if (res == ETIMEDOUT)
		return URT_NOT_LOCKED;
	return URT_FAIL;
}

int urt_rwlock_timed_write_lock(urt_rwlock *rwl, urt_time max_wait)
{
	int res;
	struct timespec tp;
	urt_time t = urt_get_time_epoch();

	t += max_wait;
	tp.tv_sec = t / 1000000000ll;
	tp.tv_nsec = t % 1000000000ll;

	res = pthread_rwlock_timedwrlock(rwl, &tp);

	if (res == 0)
		return URT_SUCCESS;
	else if (res == ETIMEDOUT)
		return URT_NOT_LOCKED;
	return URT_FAIL;
}

int urt_rwlock_try_read_lock(urt_rwlock *rwl)
{
	int res = pthread_rwlock_tryrdlock(rwl);
	if (res == 0)
		return URT_SUCCESS;
	else if (res == EBUSY)
		return URT_NOT_LOCKED;
	return URT_FAIL;
}

int urt_rwlock_try_write_lock(urt_rwlock *rwl)
{
	int res = pthread_rwlock_trywrlock(rwl);
	if (res == 0)
		return URT_SUCCESS;
	else if (res == EBUSY)
		return URT_NOT_LOCKED;
	return URT_FAIL;
}

int urt_rwlock_read_unlock(urt_rwlock *rwl)
{
	return pthread_rwlock_unlock(rwl) == 0?URT_SUCCESS:URT_FAIL;
}

int urt_rwlock_write_unlock(urt_rwlock *rwl)
{
	return pthread_rwlock_unlock(rwl) == 0?URT_SUCCESS:URT_FAIL;
}
