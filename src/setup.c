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

#include <urt_common.h>
#include "reserved.h"
#include "urt_internal.h"

urt_sem *urt_global_sem = NULL;
urt_internal *urt_global_mem = NULL;
static bool _initialized_by_me = false;

int urt_init(void)
{
	int error = URT_SUCCESS;

	/* get global lock */
	if (urt_global_sem != NULL || urt_global_mem != NULL)
		return URT_ALREADY;
	urt_global_sem = urt_shsem_attach(URT_GLOBAL_LOCK_NAME, &error);
	if (urt_global_sem == NULL)
	{
		urt_global_sem = urt_shsem_new(URT_GLOBAL_LOCK_NAME, 1, &error);
		_initialized_by_me = true;
	}
	if (urt_global_sem == NULL)
		goto exit_no_sem;

	urt_sem_wait(urt_global_sem);

	/* get global memory */
	urt_global_mem = urt_shmem_attach(URT_GLOBAL_MEM_NAME, &error);
	if (urt_global_mem == NULL)
		urt_global_mem = urt_shmem_alloc(sizeof(*urt_global_mem), &error);
	if (urt_global_mem == NULL)
		goto exit_no_mem;

	urt_init_registry();

exit_no_mem:
	urt_sem_post(urt_global_sem);
exit_no_sem:
	return error;
}

void urt_free(void)
{
	if (_initialized_by_me)
	{
		_initialized_by_me = false;
		urt_shmem_free(urt_global_mem);
		urt_shsem_delete(urt_global_sem);
	}
	else
	{
		urt_shmem_detach(urt_global_mem);
		urt_shsem_detach(urt_global_sem);
	}
}

void urt_recover(void)
{
	urt_sem *global_sem = urt_shsem_attach(URT_GLOBAL_LOCK_NAME);
	if (global_sem == NULL)
		return;

	urt_sem_try_wait(global_sem);
	urt_sem_post(global_sem);
	urt_shsem_detach(global_sem);
}
