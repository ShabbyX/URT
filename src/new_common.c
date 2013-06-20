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

#include <urt_mem.h>
#include "urt_internal.h"
#include "urt_sys_internal.h"

#define SHOBJ_NEW(name, error, typ, sz, call, TYPE, proc)	\
do {								\
	typ obj = NULL;						\
	urt_registered_object *ro = NULL;			\
	ro = urt_reserve_name(name, error);			\
	if (ro == NULL)						\
		goto exit_fail;					\
	obj = call;						\
	if (obj == NULL)					\
		goto exit_fail;					\
	obj = proc(obj, ro - urt_global_mem->objects);		\
	ro->type = URT_TYPE_##TYPE;				\
	ro->address = obj;					\
	ro->size = sz;						\
	urt_inc_name_count(ro);					\
	return obj;						\
exit_fail:							\
	if (ro)							\
		urt_deregister(ro, ro->address, ro->size,	\
				ro->release, ro->user_data);	\
	return NULL;						\
} while (0)

#define SHOBJ_ATTACH(name, error, typ, call, proc)		\
do {								\
	typ obj = NULL;						\
	urt_registered_object *ro = NULL;			\
	ro = urt_get_object_by_name_and_inc_count(name);	\
	if (ro == NULL)						\
		goto exit_no_obj;				\
	obj = call;						\
	if (obj == NULL)					\
		goto exit_fail;					\
	obj = proc(obj, ro - urt_global_mem->objects);		\
	return obj;						\
exit_no_obj:							\
	if (error)						\
		*error = URT_NO_OBJ;				\
exit_fail:							\
	if (ro)							\
		urt_deregister(ro, ro->address, ro->size,	\
				ro->release, ro->user_data);	\
	return NULL;						\
} while (0)

void *(urt_shmem_new)(const char *name, size_t size, int *error, ...)
{
	/*
	 * Note: there would be 16 bytes book keeping to keep
	 * information such as index in registry
	 */
	SHOBJ_NEW(name, error, void *, size + 16, urt_sys_shmem_new(name, size + 16, error), MEM, urt_sys_add_mem_book_keeping);
}
URT_EXPORT_SYMBOL(urt_shmem_new);

void *(urt_shmem_attach)(const char *name, int *error, ...)
{
	SHOBJ_ATTACH(name, error, void *, urt_sys_shmem_attach(name, error), urt_sys_add_mem_book_keeping);
}
URT_EXPORT_SYMBOL(urt_shmem_attach);

urt_sem *(urt_shsem_new)(const char *name, unsigned int init_value, int *error, ...)
{
	SHOBJ_NEW(name, error, urt_sem *, sizeof(urt_sem), urt_sys_shsem_new(name, init_value, error), SEM, urt_sys_add_sem_book_keeping);
}
URT_EXPORT_SYMBOL(urt_shsem_new);

urt_sem *(urt_shsem_attach)(const char *name, int *error, ...)
{
	SHOBJ_ATTACH(name, error, urt_sem *, urt_sys_shsem_attach(name, error), urt_sys_add_sem_book_keeping);
}
URT_EXPORT_SYMBOL(urt_shsem_attach);

urt_mutex *(urt_shmutex_new)(const char *name, int *error, ...)
{
	SHOBJ_NEW(name, error, urt_mutex *, sizeof(urt_mutex), urt_sys_shmutex_new(name, error), MUTEX, urt_sys_add_mutex_book_keeping);
}
URT_EXPORT_SYMBOL(urt_shmutex_new);

urt_mutex *(urt_shmutex_attach)(const char *name, int *error, ...)
{
	SHOBJ_ATTACH(name, error, urt_mutex *, urt_sys_shmutex_attach(name, error), urt_sys_add_mutex_book_keeping);
}
URT_EXPORT_SYMBOL(urt_shmutex_attach);

urt_rwlock *(urt_shrwlock_new)(const char *name, int *error, ...)
{
	SHOBJ_NEW(name, error, urt_rwlock *, sizeof(urt_rwlock), urt_sys_shrwlock_new(name, error), RWLOCK, urt_sys_add_rwlock_book_keeping);
}
URT_EXPORT_SYMBOL(urt_shrwlock_new);

urt_rwlock *(urt_shrwlock_attach)(const char *name, int *error, ...)
{
	SHOBJ_ATTACH(name, error, urt_rwlock *, urt_sys_shrwlock_attach(name, error), urt_sys_add_rwlock_book_keeping);
}
URT_EXPORT_SYMBOL(urt_shrwlock_attach);
