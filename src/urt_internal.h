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

typedef struct urt_registered_object
{
	char name[URT_NAME_LEN + 1];	/* name of object */
	bool reserved;			/* count could be zero, but name reserved */
	unsigned int count;		/* usage count */
	void *address;			/* address of the registered object */
} urt_registered_object;

typedef struct urt_internal
{
	urt_registered_object objects[URT_MAX_OBJECTS];
	unsigned int objects_max_index;	/* maximum index of `objects` that is used */
	char next_free_name[URT_NAME_LEN];	/* see implementation of urt_get_free_name */
	bool names_exhausted;		/* whether free names are exhausted */
	bool initialized;		/* whether registry is already initialized */
} urt_internal;

extern urt_sem *urt_global_sem;
extern urt_internal *urt_global_mem;

/* registry manipulation */
void urt_init_registry(void);
urt_registered_object *urt_reserve_name(const char *name);
void urt_inc_name_count(urt_registered_object *ro);
void urt_dec_name_count(urt_registered_object *ro);
static inline void urt_deregister(urt_registered_object *ro) { urt_dec_name_count(ro); }
void urt_deregister_name(const char *name);
void urt_deregister_addr(void *address);

/* registry lookup */
urt_registered_object *urt_get_object_by_name(const char *name);
urt_registered_object *urt_get_object_by_addr(void *address);

/* global sem and mem registry skip */
urt_sem *urt_global_sem_get(const char *name, int *error);
void *urt_global_mem_get(const char *name, size_t size, int *error);

#endif
