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

#ifndef URT_INTERNAL_H
#define URT_INTERNAL_H

#include <urt_consts.h>
#include <urt_stdtypes.h>
#include <urt_lock.h>
#include "urt_internal_config.h"

#ifdef __KERNEL__
# include <linux/version.h>
# if LINUX_VERSION_CODE < KERNEL_VERSION(3,2,0)
#  include <linux/module.h>
# else
#  include <linux/export.h>
# endif
#endif

typedef struct urt_registered_object
{
	char name[URT_NAME_LEN + 1];		/* name of object */
	bool reserved;				/* count could be zero, but name reserved */
	unsigned int count;			/* usage count */
	void *address;				/* address of the registered object */
	size_t size;				/* size of memory, if object is shared memory */
	void *user_data;			/* additional data to detach/free */
	void (*release)(struct urt_registered_object *);
						/* function to call on use count decrement (arg: the registry object) */
} urt_registered_object;

typedef struct urt_internal
{
	urt_registered_object objects[URT_MAX_OBJECTS];
	unsigned int objects_max_index;		/* maximum index of `objects` that is used */
	char next_free_name[URT_NAME_LEN];	/* see implementation of urt_get_free_name */
	bool names_exhausted;			/* whether free names are exhausted */
	bool initialized;			/* whether registry is already initialized */
} urt_internal;

extern urt_sem *urt_global_sem;
extern urt_internal *urt_global_mem;

/* registry manipulation */
void urt_registry_init(void);
urt_registered_object *urt_reserve_name(const char *name, int *error);
void urt_inc_name_count(urt_registered_object *ro);
void urt_dec_name_count(urt_registered_object *ro);
static inline void urt_deregister(urt_registered_object *ro) { urt_dec_name_count(ro); }
void urt_deregister_name(const char *name);
void urt_deregister_addr(void *address);
void urt_force_clear_name(const char *name);

/* registry lookup */
urt_registered_object *urt_get_object_by_name(const char *name);
urt_registered_object *urt_get_object_by_addr(void *address);

/* global sem and mem registry skip */
urt_sem *urt_global_sem_get(const char *name, int *error);
void *urt_global_mem_get(const char *name, size_t size, int *error);
void urt_global_sem_free(const char *name);
void urt_global_mem_free(const char *name);

/* system specific parts of object new/attach */
void *urt_sys_shmem_new(const char *name, size_t size, int *error);
void *urt_sys_shmem_attach(const char *name, int *error);
urt_sem *urt_sys_shsem_new(const char *name, unsigned int init_value, int *error);
urt_sem *urt_sys_shsem_attach(const char *name, int *error);
urt_mutex *urt_sys_shmutex_new(const char *name, int *error);
urt_mutex *urt_sys_shmutex_attach(const char *name, int *error);
urt_rwlock *urt_sys_shrwlock_new(const char *name, int *error);
urt_rwlock *urt_sys_shrwlock_attach(const char *name, int *error);

#ifdef __KERNEL__
# define URT_EXPORT_SYMBOL(s) EXPORT_SYMBOL_GPL(s)
#else
# define URT_EXPORT_SYMBOL(s)
#endif

#endif
