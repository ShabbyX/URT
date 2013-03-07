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
#include <errno.h>
#include <urt_internal_config.h>
#include <urt_internal.h>
#include <urt_lock.h>
#include <urt_mem.h>
#include "names.h"

urt_sem *(urt_sem_new)(unsigned int init_value, int *error, ...)
{
	urt_sem *sem = urt_mem_alloc(sizeof(*sem));
	if (URT_UNLIKELY(sem == NULL))
		goto exit_no_mem;

	if (URT_UNLIKELY(sem_init(sem, 0, init_value)))
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
	urt_mem_free(sem);
	goto exit_fail;
exit_no_mem:
	if (error)
		*error = URT_NO_MEM;
exit_fail:
	return NULL;
}

void urt_sem_delete(urt_sem *sem)
{
	if (URT_LIKELY(sem != NULL))
		sem_destroy(sem);
	urt_mem_free(sem);
}

static urt_sem *_shsem_common(const char *name, unsigned int init_value, int *error, int flags)
{
	char n[8];
	urt_sem *sem = NULL;

	if (URT_UNLIKELY(urt_convert_name(n, name) != URT_SUCCESS))
		goto exit_bad_name;

	sem = sem_open(n, flags, S_IRWXU | S_IRWXG | S_IRWXO, init_value);
	if (URT_UNLIKELY(sem == SEM_FAILED))
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

urt_sem *(urt_shsem_new)(const char *name, unsigned int init_value, int *error, ...)
{
	urt_sem *sem = NULL;
	urt_registered_object *ro = NULL;

	ro = urt_reserve_name(name);
	if (URT_UNLIKELY(ro == NULL))
		goto exit_used_name;

	sem = _shsem_common(name, init_value, error, O_CREAT | O_EXCL);
	if (URT_UNLIKELY(sem == NULL))
		goto exit_fail;

	ro->address = sem;
	urt_inc_name_count(ro);

	return sem;
exit_used_name:
	if (error)
		*error = URT_EXISTS;
exit_fail:
	if (ro)
		urt_deregister(ro);
	return NULL;
}

void urt_shsem_delete(urt_sem *sem)
{
	urt_registered_object *ro;

	sem_close(sem);
	ro = urt_get_object_by_addr(sem);
	if (ro == NULL)
		return;
	sem_unlink(ro->name);
	urt_deregister(ro);
}

urt_sem *(urt_shsem_attach)(const char *name, int *error, ...)
{
	urt_sem *sem = NULL;
	urt_registered_object *ro = NULL;

	ro = urt_get_object_by_name(name);
	if (URT_UNLIKELY(ro == NULL))
		goto exit_no_name;

	sem = _shsem_common(name, 0, error, 0);	/* TODO: I expect 0 for flags to only try to attach and not create. Must be tested */
	if (URT_UNLIKELY(sem == NULL))
		goto exit_fail;

	urt_inc_name_count(ro);

	return sem;
exit_no_name:
	if (error)
		*error = URT_NO_NAME;
exit_fail:
	if (ro)
		urt_deregister(ro);
	return NULL;
}

void urt_shsem_detach(urt_sem *sem)
{
	sem_close(sem);
	urt_deregister_addr(sem);
}

int (urt_sem_wait)(urt_sem *sem, bool *stop, ...)
{
	int res;
	if (stop)
	{
		struct timespec tp;
		urt_time t = urt_get_time();

		do
		{
			if (URT_UNLIKELY(*stop))
				return URT_NOT_LOCKED;

			t += URT_LOCK_STOP_MAX_DELAY;
			tp.tv_sec = t / 1000000000ll;
			tp.tv_nsec = t % 1000000000ll;
		} while ((res = sem_timedwait(sem, &tp)) == -1 && errno == ETIMEDOUT);
	}
	else
		/* if sem_wait interrupted, retry */
		while ((res = sem_wait(sem)) == -1 && errno == EINTR);

	if (URT_LIKELY(res == 0))
		return URT_SUCCESS;
	return URT_FAIL;
}

int urt_sem_timed_wait(urt_sem *sem, urt_time max_wait)
{
	int res;
	struct timespec tp;
	urt_time t = urt_get_time();

	t += max_wait;
	tp.tv_sec = t / 1000000000ll;
	tp.tv_nsec = t % 1000000000ll;

	while ((res = sem_timedwait(sem, &tp)) == -1 && errno == EINTR);

	if (res == 0)
		return URT_SUCCESS;
	else if (errno == ETIMEDOUT)
		return URT_TIMEOUT;
	return URT_FAIL;
}

int urt_sem_try_wait(urt_sem *sem)
{
	if (URT_UNLIKELY(sem_trywait(sem) == -1))
		return URT_FAIL;
	if (errno == EAGAIN)
		return URT_NOT_LOCKED;
	return URT_SUCCESS;
}

void urt_sem_post(urt_sem *sem)
{
	sem_post(sem);
}
