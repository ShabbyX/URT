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

#include <urt_setup.h>
#include <urt_mem.h>
#include "reserved.h"
#include "urt_internal.h"
#include <urt_sys_setup.h>

urt_internal *urt_global_mem = NULL;

int urt_init(void)
{
	int error = urt_sys_init();
	if (error != URT_SUCCESS)
		return error;

	if (urt_global_mem != NULL)
#ifdef __KERNEL__
		/* in kernel space, urt_global_* variables are not per process, but reside in main kernel module */
		return URT_SUCCESS;
#else
		return URT_ALREADY;
#endif

	/* get global lock */
	error = urt_global_sem_get(URT_GLOBAL_LOCK_NAME);
	if (error)
		goto exit_no_sem;

	urt_global_sem_wait();

	/* get global memory */
	urt_global_mem = urt_global_mem_get(URT_GLOBAL_MEM_NAME, sizeof(*urt_global_mem), &error);
	if (urt_global_mem == NULL)
		goto exit_no_mem;

	urt_registry_init();

exit_no_mem:
	urt_global_sem_post();
exit_no_sem:
	return error;
}
URT_EXPORT_SYMBOL(urt_init);

void urt_exit(void)
{
	if (urt_global_mem == NULL)
		return;

	urt_global_mem_free(URT_GLOBAL_MEM_NAME);
	urt_global_sem_free(URT_GLOBAL_LOCK_NAME);

	urt_sys_exit();
}
URT_EXPORT_SYMBOL(urt_exit);

void urt_recover(void)
{
	unsigned int i;

	if (urt_sys_init() != URT_SUCCESS)
		return;

	if (urt_global_sem_get(URT_GLOBAL_LOCK_NAME))
		return;

	/* fix the global semaphore */
	urt_global_sem_try_wait();

	/* while the global semaphore is locked, fix any global memory problems too */
	urt_global_mem = urt_global_mem_get(URT_GLOBAL_MEM_NAME, sizeof(*urt_global_mem), NULL);
	if (urt_global_mem == NULL)
		goto no_mem;

	urt_global_mem->objects_max_index = 0;
	for (i = 0; i < URT_MAX_OBJECTS; ++i)
	{
		urt_registered_object *ro = &urt_global_mem->objects[i];
		ro->release = NULL;
		if ((ro->reserved || ro->count > 0) && urt_global_mem->objects_max_index < i)
			urt_global_mem->objects_max_index = i;
	}

	urt_global_mem_free(URT_GLOBAL_MEM_NAME);
no_mem:
	urt_global_sem_post();
	urt_global_sem_free(URT_GLOBAL_LOCK_NAME);

	urt_sys_exit();
}
URT_EXPORT_SYMBOL(urt_recover);
