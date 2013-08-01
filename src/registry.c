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

#include <urt_string.h>
#include <urt_log.h>
#include <urt_sys_setup.h>
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

void urt_registry_init(void)
{
/*	unsigned int i; */

	if (urt_global_mem->initialized)
		return;
	*urt_global_mem = (urt_internal){
		.initialized = true
	};

/*	for (i = 0; i < URT_MAX_OBJECTS; ++i)
		urt_global_mem->objects[i] = (urt_registered_object){
			.name[0] = '\0',
			.reserved = false,
			.count = 0,
			.address = NULL
		};*/

	urt_global_mem->next_free_name[0] = '_';
	urt_global_mem->next_free_name[1] = '_';
	urt_global_mem->next_free_name[2] = '_';
	urt_global_mem->next_free_name[3] = '_';
	urt_global_mem->next_free_name[4] = '_';
	urt_global_mem->next_free_name[5] = '$';
}

static urt_registered_object *_find_by_name(const char *name)
{
	unsigned int i;
	urt_registered_object *ro = NULL;
	unsigned int max_index = urt_global_mem->objects_max_index;

	for (i = 0; i <= max_index; ++i)
	{
		ro = &urt_global_mem->objects[i];
		if ((ro->reserved || ro->count != 0) && _name_eq(ro->name, name))
			break;
	}

	return i > max_index?NULL:ro;
}

urt_registered_object *urt_reserve_name(const char *name, int *error)
{
	unsigned int i;
	urt_registered_object *obj = NULL;

	urt_global_sem_wait();

	/* make sure name doesn't exist */
	if (_find_by_name(name) != NULL)
		goto exit_exists;

	/* find a free space for the name */
	for (i = 0; i < URT_MAX_OBJECTS; ++i)
	{
		obj = &urt_global_mem->objects[i];
		if (!obj->reserved && obj->count == 0)
			break;
	}
	if (i == URT_MAX_OBJECTS)
		goto exit_max_reached;

	*obj = (urt_registered_object){
		.reserved = true
	};
	_name_cpy(obj->name, name);
	if (i > urt_global_mem->objects_max_index)
		urt_global_mem->objects_max_index = i;

	urt_global_sem_post();

	return obj;
exit_exists:
	if (error)
		*error = URT_EXISTS;
	goto exit_fail;
exit_max_reached:
	if (error)
		*error = URT_MAX_REACHED;
exit_fail:
	urt_global_sem_post();
	return NULL;
}

void urt_inc_name_count(urt_registered_object *ro)
{
	urt_global_sem_wait();
	++ro->count;
	ro->reserved = false;
	urt_global_sem_post();
}

#ifndef NDEBUG
static void _internal_mem_check(void)
{
	unsigned int i;
	bool error_given = false;

	for (i = 0; i < URT_MAX_OBJECTS; ++i)
	{
		urt_registered_object *obj = &urt_global_mem->objects[i];
		if ((obj->reserved || obj->count > 0) && i > urt_global_mem->objects_max_index)
		{
			urt_global_mem->objects_max_index = i;
			if (!error_given)
			{
				urt_err("internal error: objects_max_index too low");
				error_given = true;
			}
		}
	}
}
#else
# define _internal_mem_check() ((void)0)
#endif

static inline void _dec_count_common(urt_registered_object *ro)
{
	unsigned int index = ro - urt_global_mem->objects;
	if (URT_LIKELY(ro->count > 0))
		--ro->count;
	ro->reserved = false;

	/* take care of object cleanup */
	if (ro->release)
		ro->release(ro);
	ro->release = NULL;	/* make NULL since it should be called only once */

	/* if removing max_index used, lower max_index used */
	if (URT_UNLIKELY(index == urt_global_mem->objects_max_index))
	{
		while (index > 0 && !urt_global_mem->objects[index].reserved
			&& urt_global_mem->objects[index].count == 0)
			--index;
		urt_global_mem->objects_max_index = index;
	}

	_internal_mem_check();
}

void urt_dec_name_count(urt_registered_object *ro,
		void *address, size_t size,
		void (*release)(struct urt_registered_object *),
		void *user_data)
{
	urt_global_sem_wait();
	ro->address = address;
	if (size > 0)
		ro->size = size;
	ro->release = release;
	ro->user_data = user_data;
	_dec_count_common(ro);
	urt_global_sem_post();
}

void urt_force_clear_name(const char *name)
{
	urt_registered_object *ro = NULL;

	urt_global_sem_wait();
	ro = _find_by_name(name);
	/* cleanup the related resources, too */
	if (ro)
	{
		urt_sys_force_clear_name(ro);
		do
		{
			_dec_count_common(ro);
		} while (ro->count > 0);
	}
	urt_global_sem_post();
}

urt_registered_object *urt_get_object_by_name_and_inc_count(const char *name)
{
	urt_registered_object *ro = NULL;

	urt_global_sem_wait();
	ro = _find_by_name(name);
	if (ro)
	{
		++ro->count;
		ro->reserved = false;
	}
	urt_global_sem_post();

	return ro;
}

static int _increment_name(char *name)
{
	int i;
	char digit;

	for (i = URT_NAME_LEN - 2; i >= 0; --i)
	{
		bool finished = true;
		digit = name[i];

		if (digit == '_')
			digit = '0';
		else if (digit >= '0' && digit < '9')
			++digit;
		else if (digit == '9')
			digit = 'A';
		else if (digit >= 'A' && digit < 'Z')
			++digit;
		else if (digit == 'Z')
		{
			digit = '_';
			finished = false;
		}
		else
			goto exit_internal;

		name[i] = digit;
		if (finished)
			break;
	}

	if (URT_UNLIKELY(i < 0))
	{
		name[0] = '_';
		name[1] = '_';
		name[2] = '_';
		name[3] = '_';
		name[4] = '_';
		name[5] = '$';
		return 1;
	}

	return 0;
exit_internal:
	urt_err("internal error: next_free_name contains invalid character '%c' (%d)\n", digit, digit);
	name[0] = '_';
	name[1] = '_';
	name[2] = '_';
	name[3] = '_';
	name[4] = '_';
	name[5] = '$';
	return -1;
}

static void _check_and_get_free_name(char *name)
{
	char *next_name;
	int ret;

	next_name = urt_global_mem->next_free_name;

	do
	{
		strncpy(name, next_name, URT_NAME_LEN);
		ret = _increment_name(next_name);
	} while (ret < 0 || _find_by_name(name) != NULL);
}

/*
 * free names have 5 characters in [A-Z0-9_] and a terminating $.
 * Since users are not supposed to use $, this will not collide with
 * user names. Therefore, no actual checking with the registry is made.
 */
int urt_get_free_name(char *name)
{
	int ret;
	char *next_name;

	urt_global_sem_wait();

	if (URT_UNLIKELY(urt_global_mem->names_exhausted))
		_check_and_get_free_name(name);
	else
	{
		next_name = urt_global_mem->next_free_name;
		strncpy(name, next_name, URT_NAME_LEN);
		ret = _increment_name(next_name);
		if (URT_UNLIKELY(ret))
			urt_global_mem->names_exhausted = true;
		if (URT_UNLIKELY(ret < 0))
			goto exit_fail;
	}

	urt_global_sem_post();
	return URT_SUCCESS;
exit_fail:
	urt_global_sem_post();
	return URT_FAIL;
}
URT_EXPORT_SYMBOL(urt_get_free_name);

void urt_print_names(void)
{
	unsigned int i;
	urt_registered_object *obj = NULL;

	urt_global_sem_wait();

	urt_out_cont("  index  | ");
	for (i = 0; i < URT_NAME_LEN / 2 - 2; ++i)
		urt_out_cont(" ");
	urt_out_cont("name");
	for (i = 0; i < URT_NAME_LEN - URT_NAME_LEN / 2 - 2; ++i)
		urt_out_cont(" ");
	urt_out_cont(" |  count  | reserved |      address       | size (bytes) |  type\n");
	urt_out_cont("---------+-");
	for (i = 0; i < URT_NAME_LEN; ++i)
		urt_out_cont("-");
	urt_out_cont("-+---------+----------+--------------------+--------------+--------\n");
	for (i = 0; i < URT_MAX_OBJECTS; ++i)
	{
		obj = &urt_global_mem->objects[i];
		if (!obj->reserved && obj->count == 0)
			continue;
		urt_out_cont(" %7u | %*s | %7u | %8s | %18p | %12zu | %6s\n", i, URT_NAME_LEN, obj->name, obj->count,
				obj->reserved?"Yes":"No", obj->address, obj->size,
				obj->type == URT_TYPE_MEM?"MEM ":
				obj->type == URT_TYPE_SEM?"SEM ":
				obj->type == URT_TYPE_MUTEX?"MUTEX":
				obj->type == URT_TYPE_RWLOCK?"RWLOCK":"??????");
	}
	urt_out_cont("\nmax index: %u\n", urt_global_mem->objects_max_index);
	urt_out_cont("next free name: %*s%s\n", URT_NAME_LEN, urt_global_mem->next_free_name,
			urt_global_mem->names_exhausted?" (cycled)":"");

	urt_global_sem_post();
}
