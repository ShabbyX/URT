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

#ifndef URT_SYS_INTERNAL_H
#define URT_SYS_INTERNAL_H

#include <urt_internal.h>

void urt_sys_force_clear_name(urt_registered_object *ro);

static inline void *urt_sys_add_mem_book_keeping(void *mem, unsigned int id, urt_registered_object *ro)
{
	*(unsigned int *)mem = id;
	ro->has_bookkeeping = true;
	return (char *)mem + 16;
}

static inline urt_sem *urt_sys_add_sem_book_keeping(urt_sem *sem, unsigned int id, urt_registered_object *ro)
{
	sem->id = id;
	return sem;
}

static inline urt_mutex *urt_sys_add_mutex_book_keeping(urt_mutex *mutex, unsigned int id, urt_registered_object *ro)
{
	return urt_sys_add_mem_book_keeping(mutex, id, ro);
}

static inline urt_rwlock *urt_sys_add_rwlock_book_keeping(urt_rwlock *rwl, unsigned int id, urt_registered_object *ro)
{
	return urt_sys_add_mem_book_keeping(rwl, id, ro);
}

static inline urt_cond *urt_sys_add_cond_book_keeping(urt_cond *cond, unsigned int id, urt_registered_object *ro)
{
	return urt_sys_add_mem_book_keeping(cond, id, ro);
}

#endif
