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

#include <urt_internal.h>
#include <urt_lock.h>
#include <urt_mem.h>
#include <rtai_registry.h>

#ifdef __KERNEL__
# define rt_typed_named_sem_init(name, value, type) _rt_typed_named_sem_init(nam2num(name), value, type, NULL)
# define rt_named_rwl_init(name) _rt_named_rwl_init(nam2num(name))
#endif

urt_sem *_sem_new_common(unsigned int init_value, int type, int *error)
{
#ifdef __KERNEL__
	urt_sem *sem = urt_mem_new(sizeof(*sem), error);
	if (sem == NULL)
		return NULL;

	rt_typed_sem_init(sem, init_value, type);
	return sem;
#else
	urt_sem *sem = rt_typed_sem_init(0, init_value, type);
	if (sem)
		return sem;
	if (error)
		*error = URT_NO_MEM;
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
	if (URT_LIKELY(sem != NULL))
		rt_sem_delete(sem);
#ifdef __KERNEL__
	urt_mem_delete(sem);
#endif
}
URT_EXPORT_SYMBOL(urt_sem_delete);

static urt_sem *_shsem_common(const char *name, unsigned int init_value, int type, int *error)
{
	urt_sem *sem = rt_typed_named_sem_init(name, init_value, type);
	if (sem)
		return sem;
	if (error)
		*error = URT_NO_MEM;
	return NULL;
}

urt_sem *urt_global_sem_get(const char *name, int *error)
{
	return _shsem_common(name, 1, BIN_SEM, error);
}

urt_sem *urt_sys_shsem_new(const char *name, unsigned int init_value, int *error)
{
	return _shsem_common(name, init_value, CNT_SEM, error);
}

urt_sem *urt_sys_shsem_attach(const char *name, int *error)
{
	urt_sem *sem = rt_get_adr(nam2num(name));
	if (sem)
		return sem;
	if (error)
		*error = URT_NO_OBJ;
	return NULL;
}

void urt_global_sem_free(const char *name)
{
	rt_named_sem_delete(urt_global_sem);
}

void urt_shsem_detach(urt_sem *sem)
{
	if (sem == NULL)
		return;
	rt_named_sem_delete(sem);
	urt_deregister_addr(sem);
}
URT_EXPORT_SYMBOL(urt_shsem_detach);

int (urt_sem_wait)(urt_sem *sem, bool *stop, ...)
{
	int res;
	if (stop)
	{
		while ((res = rt_sem_wait_timed(sem, nano2count(URT_LOCK_STOP_MAX_DELAY))) == RTE_TIMOUT)
			if (*stop)
				return URT_NOT_LOCKED;
	}
	else
		res = rt_sem_wait(sem);

	return res > RTE_BASE?URT_FAIL:URT_SUCCESS;
}
URT_EXPORT_SYMBOL(urt_sem_wait);

int urt_sem_timed_wait(urt_sem *sem, urt_time max_wait)
{
	int res = rt_sem_wait_timed(sem, nano2count(max_wait));
	return res == RTE_TIMOUT?URT_NOT_LOCKED:res > RTE_BASE?URT_FAIL:URT_SUCCESS;
}
URT_EXPORT_SYMBOL(urt_sem_timed_wait);

int urt_sem_try_wait(urt_sem *sem)
{
	int res = rt_sem_wait_if(sem);
	return res == 0?URT_NOT_LOCKED:res > RTE_BASE?URT_FAIL:URT_SUCCESS;
}
URT_EXPORT_SYMBOL(urt_sem_try_wait);

void urt_sem_post(urt_sem *sem)
{
	rt_sem_signal(sem);
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
#ifdef __KERNEL__
	urt_rwlock *rwl = urt_mem_new(sizeof(*rwl), error);
	if (rwl == NULL)
		return NULL;

	rt_rwl_init(rwl);
	return rwl;
#else
	urt_rwlock *rwl = rt_rwl_init(0);
	if (rwl)
		return rwl;
	if (error)
		*error = URT_NO_MEM;
	return NULL;
#endif
}
URT_EXPORT_SYMBOL(urt_rwlock_new);

void urt_rwlock_detach(urt_rwlock *rwl)
{
	if (URT_LIKELY(rwl != NULL))
		rt_rwl_delete(rwl);
#ifdef __KERNEL__
	urt_mem_delete(rwl);
#endif
}
URT_EXPORT_SYMBOL(urt_rwlock_detach);

urt_rwlock *urt_sys_shrwlock_new(const char *name, int *error)
{
	urt_rwlock *rwl = rt_named_rwl_init(name);
	if (rwl)
		return rwl;
	if (error)
		*error = URT_NO_MEM;
	return NULL;
}

urt_rwlock *urt_sys_shrwlock_attach(const char *name, int *error)
{
	urt_rwlock *rwl = rt_get_adr(nam2num(name));
	if (rwl)
		return rwl;
	if (error)
		*error = URT_NO_OBJ;
	return NULL;
}

void urt_shrwlock_detach(urt_rwlock *rwl)
{
	if (rwl == NULL)
		return;
	rt_named_rwl_delete(rwl);
	urt_deregister_addr(rwl);
}
URT_EXPORT_SYMBOL(urt_shrwlock_detach);

int (urt_rwlock_read_lock)(urt_rwlock *rwl, bool *stop, ...)
{
	int res;
	if (stop)
	{
		while ((res = rt_rwl_rdlock_timed(rwl, nano2count(URT_LOCK_STOP_MAX_DELAY))) == RTE_TIMOUT)
			if (*stop)
				return URT_NOT_LOCKED;
	}
	else
		res = rt_rwl_rdlock(rwl);

	return res == 0?URT_SUCCESS:URT_FAIL;
}
URT_EXPORT_SYMBOL(urt_rwlock_read_lock);

int (urt_rwlock_write_lock)(urt_rwlock *rwl, bool *stop, ...)
{
	int res;
	if (stop)
	{
		while ((res = rt_rwl_wrlock_timed(rwl, nano2count(URT_LOCK_STOP_MAX_DELAY))) == RTE_TIMOUT)
			if (*stop)
				return URT_NOT_LOCKED;
	}
	else
		res = rt_rwl_wrlock(rwl);

	return res == 0?URT_SUCCESS:URT_FAIL;
}
URT_EXPORT_SYMBOL(urt_rwlock_write_lock);

int urt_rwlock_timed_read_lock(urt_rwlock *rwl, urt_time max_wait)
{
	int res = rt_rwl_rdlock_timed(rwl, nano2count(max_wait));
	return res == RTE_TIMOUT?URT_NOT_LOCKED:res == 0?URT_SUCCESS:URT_FAIL;
}
URT_EXPORT_SYMBOL(urt_rwlock_timed_read_lock);

int urt_rwlock_timed_write_lock(urt_rwlock *rwl, urt_time max_wait)
{
	int res = rt_rwl_wrlock_timed(rwl, nano2count(max_wait));
	return res == RTE_TIMOUT?URT_NOT_LOCKED:res == 0?URT_SUCCESS:URT_FAIL;
}
URT_EXPORT_SYMBOL(urt_rwlock_timed_write_lock);

int urt_rwlock_try_read_lock(urt_rwlock *rwl)
{
	return rt_rwl_rdlock_if(rwl) == 0?URT_SUCCESS:URT_NOT_LOCKED;
}
URT_EXPORT_SYMBOL(urt_rwlock_try_read_lock);

int urt_rwlock_try_write_lock(urt_rwlock *rwl)
{
	return rt_rwl_wrlock_if(rwl) == 0?URT_SUCCESS:URT_NOT_LOCKED;
}
URT_EXPORT_SYMBOL(urt_rwlock_try_write_lock);

int urt_rwlock_read_unlock(urt_rwlock *rwl)
{
	return rt_rwl_unlock(rwl) == 0?URT_SUCCESS:URT_FAIL;
}
URT_EXPORT_SYMBOL(urt_rwlock_read_unlock);

int urt_rwlock_write_unlock(urt_rwlock *rwl)
{
	return rt_rwl_unlock(rwl) == 0?URT_SUCCESS:URT_FAIL;
}
URT_EXPORT_SYMBOL(urt_rwlock_write_unlock);
