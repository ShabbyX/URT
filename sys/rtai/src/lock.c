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

#include <urt_internal.h>
#include <urt_sys_internal.h>
#include <urt_lock.h>
#include <urt_log.h>
#include <urt_mem.h>
#include <rtai_registry.h>

#ifdef __KERNEL__
# define rt_typed_named_sem_init(name, value, type) _rt_typed_named_sem_init(nam2num(name), value, type, NULL)
# define rt_named_rwl_init(name) _rt_named_rwl_init(nam2num(name))
#endif

urt_sem *_sem_new_common(unsigned int init_value, int type, int *error)
{
	urt_sem *sem = urt_mem_new(sizeof *sem, error);
	if (sem == NULL)
		return NULL;
#ifdef __KERNEL__
	rt_typed_sem_init(&sem->sem, init_value, type);
	sem->sem_ptr = &sem->sem;
	return sem;
#else
	sem->sem_ptr = rt_typed_sem_init(0, init_value, type);
	if (sem->sem_ptr)
		return sem;
	if (error)
		*error = ENOMEM;
	urt_mem_delete(sem);
	return NULL;
#endif
}

urt_sem *(urt_sem_new)(unsigned int init_value, int *error, ...)
{
	return _sem_new_common(init_value, CNT_SEM, error);
}
URT_EXPORT_SYMBOL(urt_sem_new);

void urt_sem_delete(urt_sem *sem)
{
	if (sem != NULL)
		rt_sem_delete(sem->sem_ptr);
	urt_mem_delete(sem);
}
URT_EXPORT_SYMBOL(urt_sem_delete);

static urt_sem *_shsem_common(const char *name, unsigned int init_value, int type, int *error)
{
	urt_sem *sem = urt_mem_new(sizeof *sem, error);
	if (sem == NULL)
		goto exit_no_mem;
	sem->sem_ptr = rt_typed_named_sem_init(name, init_value, type);
	if (sem->sem_ptr == NULL)
		goto exit_no_mem;
	return sem;
exit_no_mem:
	if (error)
		*error = ENOMEM;
	urt_mem_delete(sem);
	return NULL;
}

urt_sem *urt_sys_shsem_new(const char *name, unsigned int init_value, int *error)
{
	return _shsem_common(name, init_value, CNT_SEM, error);
}

urt_sem *urt_sys_shsem_attach(const char *name, int *error)
{
	urt_sem *sem = urt_mem_new(sizeof *sem, error);
	if (sem == NULL)
		goto exit_no_mem;
	sem->sem_ptr = rt_get_adr(nam2num(name));
	if (sem->sem_ptr == NULL)
		goto exit_no_obj;
	return sem;
exit_no_mem:
	if (error)
		*error = ENOMEM;
	goto exit_fail;
exit_no_obj:
	if (error)
		*error = ENOENT;
	goto exit_fail;
exit_fail:
	urt_mem_delete(sem);
	return NULL;
}

void urt_shsem_detach(urt_sem *sem)
{
	urt_registered_object *ro;
	if (sem == NULL)
		return;
	ro = urt_get_object_by_id(sem->id);
	rt_named_sem_delete(sem->sem_ptr);
	urt_deregister(ro, ro->address, ro->size, NULL, NULL);
}
URT_EXPORT_SYMBOL(urt_shsem_detach);

int (urt_sem_waitf)(urt_sem *sem, bool (*stop)(volatile void *), volatile void *data, ...)
{
	int res;
	if (stop)
	{
		while ((res = rt_sem_wait_timed(sem->sem_ptr, nano2count(URT_LOCK_STOP_MAX_DELAY))) == RTE_TIMOUT)
			if (stop(data))
				return ECANCELED;
	}
	else
		res = rt_sem_wait(sem->sem_ptr);

	return res >= RTE_BASE?EINVAL:0;
}
URT_EXPORT_SYMBOL(urt_sem_waitf);

int urt_sem_timed_wait(urt_sem *sem, urt_time max_wait)
{
	int res = rt_sem_wait_timed(sem->sem_ptr, nano2count(max_wait));
	return res == RTE_TIMOUT?ETIMEDOUT:res >= RTE_BASE?EINVAL:0;
}
URT_EXPORT_SYMBOL(urt_sem_timed_wait);

int urt_sem_try_wait(urt_sem *sem)
{
	int res = rt_sem_wait_if(sem->sem_ptr);
	return res <= 0?EBUSY:res >= RTE_BASE?EINVAL:0;
}
URT_EXPORT_SYMBOL(urt_sem_try_wait);

void urt_sem_post(urt_sem *sem)
{
	rt_sem_signal(sem->sem_ptr);
}
URT_EXPORT_SYMBOL(urt_sem_post);

urt_mutex *(urt_mutex_new)(int *error, ...)
{
	return _sem_new_common(1, BIN_SEM, error);
}
URT_EXPORT_SYMBOL(urt_mutex_new);

urt_mutex *urt_sys_shmutex_new(const char *name, int *error)
{
	return urt_sys_shsem_new(name, 1, error);
}

urt_mutex *urt_sys_shmutex_attach(const char *name, int *error)
{
	return urt_sys_shsem_attach(name, error);
}

urt_rwlock *(urt_rwlock_new)(int *error, ...)
{
	urt_rwlock *rwl = urt_mem_new(sizeof *rwl, error);
	if (rwl == NULL)
	{
		if (error)
			*error = ENOMEM;
		return NULL;
	}
#ifdef __KERNEL__
	rt_rwl_init(&rwl->rwl);
	rwl->rwl_ptr = &rwl->rwl;
	return rwl;
#else
	rwl->rwl_ptr = rt_rwl_init(0);
	if (rwl->rwl_ptr)
		return rwl;
	if (error)
		*error = ENOMEM;
	urt_mem_delete(rwl);
	return NULL;
#endif
}
URT_EXPORT_SYMBOL(urt_rwlock_new);

void urt_rwlock_detach(urt_rwlock *rwl)
{
	if (rwl != NULL)
		rt_rwl_delete(rwl->rwl_ptr);
	urt_mem_delete(rwl);
}
URT_EXPORT_SYMBOL(urt_rwlock_detach);

urt_rwlock *urt_sys_shrwlock_new(const char *name, int *error)
{
	urt_rwlock *rwl = urt_mem_new(sizeof *rwl, error);
	if (rwl == NULL)
		goto exit_no_mem;
	rwl->rwl_ptr = rt_named_rwl_init(name);
	if (rwl->rwl_ptr == NULL)
		goto exit_no_mem;
	return rwl;
exit_no_mem:
	if (error)
		*error = ENOMEM;
	urt_mem_delete(rwl);
	return NULL;
}

urt_rwlock *urt_sys_shrwlock_attach(const char *name, int *error)
{
	urt_rwlock *rwl = urt_mem_new(sizeof *rwl, error);
	if (rwl == NULL)
		goto exit_no_mem;
	rwl->rwl_ptr = rt_get_adr(nam2num(name));
	if (rwl->rwl_ptr == NULL)
		goto exit_no_obj;
	return rwl;
exit_no_mem:
	if (error)
		*error = ENOMEM;
	goto exit_fail;
exit_no_obj:
	if (error)
		*error = ENOENT;
	goto exit_fail;
exit_fail:
	urt_mem_delete(rwl);
	return NULL;
}

void urt_shrwlock_detach(urt_rwlock *rwl)
{
	urt_registered_object *ro;
	if (rwl == NULL)
		return;
	ro = urt_get_object_by_id(rwl->id);
	rt_named_rwl_delete(rwl->rwl_ptr);
	urt_deregister(ro, ro->address, ro->size, NULL, NULL);
}
URT_EXPORT_SYMBOL(urt_shrwlock_detach);

int (urt_rwlock_read_lockf)(urt_rwlock *rwl, bool (*stop)(volatile void *), volatile void *data, ...)
{
	int res;
	if (stop)
	{
		while ((res = rt_rwl_rdlock_timed(rwl->rwl_ptr, nano2count(URT_LOCK_STOP_MAX_DELAY))) == RTE_TIMOUT)
			if (stop(data))
				return ECANCELED;
	}
	else
		res = rt_rwl_rdlock(rwl->rwl_ptr);

	return res == 0?0:EDEADLK;
}
URT_EXPORT_SYMBOL(urt_rwlock_read_lockf);

int (urt_rwlock_write_lockf)(urt_rwlock *rwl, bool (*stop)(volatile void *), volatile void *data, ...)
{
	int res;
	if (stop)
	{
		while ((res = rt_rwl_wrlock_timed(rwl->rwl_ptr, nano2count(URT_LOCK_STOP_MAX_DELAY))) == RTE_TIMOUT)
			if (stop(data))
				return ECANCELED;
	}
	else
		res = rt_rwl_wrlock(rwl->rwl_ptr);

	return res == 0?0:EDEADLK;
}
URT_EXPORT_SYMBOL(urt_rwlock_write_lockf);

int urt_rwlock_timed_read_lock(urt_rwlock *rwl, urt_time max_wait)
{
	int res = rt_rwl_rdlock_timed(rwl->rwl_ptr, nano2count(max_wait));
	return res == RTE_TIMOUT?ETIMEDOUT:res == 0?0:EINVAL;
}
URT_EXPORT_SYMBOL(urt_rwlock_timed_read_lock);

int urt_rwlock_timed_write_lock(urt_rwlock *rwl, urt_time max_wait)
{
	int res = rt_rwl_wrlock_timed(rwl->rwl_ptr, nano2count(max_wait));
	return res == RTE_TIMOUT?ETIMEDOUT:res == 0?0:EINVAL;
}
URT_EXPORT_SYMBOL(urt_rwlock_timed_write_lock);

int urt_rwlock_try_read_lock(urt_rwlock *rwl)
{
	return rt_rwl_rdlock_if(rwl->rwl_ptr) == 0?0:EBUSY;
}
URT_EXPORT_SYMBOL(urt_rwlock_try_read_lock);

int urt_rwlock_try_write_lock(urt_rwlock *rwl)
{
	return rt_rwl_wrlock_if(rwl->rwl_ptr) == 0?0:EBUSY;
}
URT_EXPORT_SYMBOL(urt_rwlock_try_write_lock);

int urt_rwlock_read_unlock(urt_rwlock *rwl)
{
	return rt_rwl_unlock(rwl->rwl_ptr) == 0?0:EINVAL;
}
URT_EXPORT_SYMBOL(urt_rwlock_read_unlock);

int urt_rwlock_write_unlock(urt_rwlock *rwl)
{
	return rt_rwl_unlock(rwl->rwl_ptr) == 0?0:EINVAL;
}
URT_EXPORT_SYMBOL(urt_rwlock_write_unlock);
