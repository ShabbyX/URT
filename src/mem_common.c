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

#include "urt_internal.h"

#ifdef __KERNEL__
# include <linux/string.h>
# include <linux/vmalloc.h>
#else
# include <stdlib.h>
#endif

void *(urt_mem_new)(size_t size, int *error, ...)
{
	void *mem = NULL;

	URT_CHECK_NONRT_CONTEXT();

#ifdef __KERNEL__
	mem = vmalloc(size);
#else
	mem = malloc(size);
#endif
	if (URT_UNLIKELY(mem == NULL))
		if (error)
			*error = ENOMEM;
	return mem;
}
URT_EXPORT_SYMBOL(urt_mem_new);

void *(urt_mem_resize)(void *old_mem, size_t old_size, size_t size, int *error, ...)
{
	void *mem = NULL;

	URT_CHECK_NONRT_CONTEXT();

	/* size == 0 is invalid.  size < old_size is useless */
	if (size == 0 || size < old_size)
		return old_mem;

#ifdef __KERNEL__
	mem = vmalloc(size);
#else
	mem = realloc(old_mem, size);
#endif
	if (URT_UNLIKELY(mem == NULL))
	{
		if (error)
			*error = ENOMEM;
	}

#ifdef __KERNEL__
	else
	{
		memcpy(mem, old_mem, old_size);
		vfree(old_mem);
	}
#endif

	return mem;
}
URT_EXPORT_SYMBOL(urt_mem_resize);
