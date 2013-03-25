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

#define SHOBJ_NEW(name, error, type, call)			\
do {								\
	type obj = NULL;					\
	urt_registered_object *ro = NULL;			\
	ro = urt_reserve_name(name, error);			\
	if (ro == NULL)						\
		goto exit_fail;					\
	obj = call;						\
	if (obj == NULL)					\
		goto exit_fail;					\
	ro->address = obj;					\
	urt_inc_name_count(ro);					\
	return obj;						\
exit_fail:							\
	if (ro)							\
		urt_deregister(ro);				\
	return NULL;						\
} while (0)

#define SHOBJ_ATTACH(name, error, type, call)			\
do {								\
	type obj = NULL;					\
	urt_registered_object *ro = NULL;			\
	ro = urt_get_object_by_name(name);			\
	if (ro == NULL)						\
		goto exit_no_obj;				\
	obj = call;						\
	if (obj == NULL)					\
		goto exit_fail;					\
	urt_inc_name_count(ro);					\
	return obj;						\
exit_no_obj:							\
	if (error)						\
		*error = URT_NO_OBJ;				\
exit_fail:							\
	if (ro)							\
		urt_deregister(ro);				\
	return NULL;						\
} while (0)

void *(urt_shmem_new)(const char *name, size_t size, int *error, ...)
{
	SHOBJ_NEW(name, error, void *, urt_sys_shmem_new(name, size, error));
}

void *(urt_shmem_attach)(const char *name, int *error, ...)
{
	SHOBJ_ATTACH(name, error, void *, urt_sys_shmem_attach(name, error));
}

urt_sem *(urt_shsem_new)(const char *name, unsigned int init_value, int *error, ...)
{
	SHOBJ_NEW(name, error, urt_sem *, urt_sys_shsem_new(name, init_value, error));
}

urt_sem *(urt_shsem_attach)(const char *name, int *error, ...)
{
	SHOBJ_ATTACH(name, error, urt_sem *, urt_sys_shsem_attach(name, error));
}

urt_mutex *(urt_shmutex_new)(const char *name, int *error, ...)
{
	SHOBJ_NEW(name, error, urt_mutex *, urt_sys_shmutex_new(name, error));
}

urt_mutex *(urt_shmutex_attach)(const char *name, int *error, ...)
{
	SHOBJ_ATTACH(name, error, urt_mutex *, urt_sys_shmutex_attach(name, error));
}

urt_rwlock *(urt_shrwlock_new)(const char *name, int *error, ...)
{
	SHOBJ_NEW(name, error, urt_rwlock *, urt_sys_shrwlock_new(name, error));
}

urt_rwlock *(urt_shrwlock_attach)(const char *name, int *error, ...)
{
	SHOBJ_ATTACH(name, error, urt_rwlock *, urt_sys_shrwlock_attach(name, error));
}
