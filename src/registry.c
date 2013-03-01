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

#include "urt_internal.h"

static bool _name_eq(const char *n1, const char *n2)
{
	while (*n1 != '\0' && *n1 == *n2)
	{
		++n1;
		++n2;
	}

	return *n1 == '\0' && *n2 == '\0';
}

static void _name_cpy(char *to, const char *from)
{
	unsigned int i;

	for (i = 0; i < URT_NAME_LEN; ++i)
		if (from[i] == '\0')
			break;
		else
			to[i] = from[i];

	to[i] = '\0';
}

static urt_registered_object *_find_by_name(const char *name)
{
	unsigned int i;
	urt_registered_object *ro = NULL;

	for (i = 0; i < URT_MAX_OBJECTS; ++i)
	{
		ro = &urt_global_mem.objects[i];
		if ((ro->reserved || ro->count != 0) && _name_eq(ro->name, name))
			break;
	}

	if (i == URT_MAX_OBJECTS)
		return NULL;
	return ro;
}

static urt_registered_object *_find_by_addr(void *address)
{
	unsigned int i;
	urt_registered_object *ro = NULL;

	for (i = 0; i < URT_MAX_OBJECTS; ++i)
	{
		ro = &urt_global_mem.objects[i];
		if ((ro->reserved || ro->count != 0) && ro->address == address)
			break;
	}

	if (i == URT_MAX_OBJECTS)
		return NULL;
	return ro;
}

urt_registered_object *urt_reserve_name(const char *name)
{
	unsigned int i;
	urt_registered_object *obj = NULL;

	urt_sem_wait(urt_global_sem);

	/* make sure name doesn't exist */
	if (_find_by_name(name) != NULL)
		return NULL;

	/* find a free space for the name */
	for (i = 0; i < URT_MAX_OBJECTS; ++i)
	{
		obj = &urt_global_mem.objects[i];
		if (!obj->reserved && obj->count == 0)
			break;
	}
	if (i == URT_MAX_OBJECTS)
		return NULL;

	obj->reserved = true;
	_name_cpy(obj->name, name);
	
	urt_sem_post(urt_global_sem);

	return obj;
}

void urt_inc_name_count(urt_registered_object *ro)
{
	urt_sem_wait(urt_global_sem);
	++ro->count;
	urt_sem_post(urt_global_sem);
}

static inline void _dec_count_common(urt_registered_object *ro)
{
	if (URT_LIKELY(ro->count > 0))
		--ro->count;
	ro->reserved = false;
}

void urt_dec_name_count(urt_registered_object *ro)
{
	urt_sem_wait(urt_global_sem);
	_dec_count_common(ro);
	urt_sem_post(urt_global_sem);
}

void urt_deregister_name(const char *name)
{
	urt_registered_object *ro = NULL;

	urt_sem_wait(urt_global_sem);
	ro = _find_by_name(name);
	if (ro)
		_dec_count_common(ro);
	urt_sem_post(urt_global_sem);
}

void urt_deregister_addr(void *address)
{
	unsigned int i;
	urt_registered_object *ro = NULL;

	urt_sem_wait(urt_global_sem);
	ro = _find_by_addr(address);
	if (ro)
		_dec_count_common(ro);
	urt_sem_post(urt_global_sem);
}

urt_registered_object *urt_get_object_by_name(const char *name)
{
	urt_registered_object *ro = NULL;

	urt_sem_wait(urt_global_sem);
	ro = _find_by_name(name);
	urt_sem_post(urt_global_sem);

	return ro;
}

urt_registered_object *urt_get_object_by_addr(void *address)
{
	urt_registered_object *ro = NULL;

	urt_sem_wait(urt_global_sem);
	ro = _find_by_addr(address);
	urt_sem_post(urt_global_sem);

	return ro;
}
