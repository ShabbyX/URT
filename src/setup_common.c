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

urt_sem *urt_global_sem = NULL;
urt_internal *urt_global_mem = NULL;

int urt_init(void)
{
	int error = urt_sys_init();
	if (error != URT_SUCCESS)
		return error;

	/* get global lock */
	if (urt_global_sem != NULL || urt_global_mem != NULL)
		return URT_ALREADY;
	urt_global_sem = urt_global_sem_get(URT_GLOBAL_LOCK_NAME, &error);
	if (urt_global_sem == NULL)
		goto exit_no_sem;

	urt_sem_wait(urt_global_sem);

	/* get global memory */
	urt_global_mem = urt_global_mem_get(URT_GLOBAL_MEM_NAME, sizeof(*urt_global_mem), &error);
	if (urt_global_mem == NULL)
		goto exit_no_mem;

	urt_registry_init();

exit_no_mem:
	urt_sem_post(urt_global_sem);
exit_no_sem:
	return error;
}
URT_EXPORT_SYMBOL(urt_init);

void urt_exit(void)
{
	if (urt_global_sem == NULL || urt_global_mem == NULL)
		return;

	urt_global_mem_free(URT_GLOBAL_MEM_NAME);
	urt_global_sem_free(URT_GLOBAL_LOCK_NAME);

	urt_sys_exit();
}
URT_EXPORT_SYMBOL(urt_exit);

void urt_recover(void)
{
	if (urt_sys_init() != URT_SUCCESS)
		return;

	urt_global_sem = urt_global_sem_get(URT_GLOBAL_LOCK_NAME, NULL);
	if (urt_global_sem == NULL)
		return;

	urt_sem_try_wait(urt_global_sem);
	urt_sem_post(urt_global_sem);
	urt_global_sem_free(URT_GLOBAL_LOCK_NAME);

	urt_sys_exit();
}
URT_EXPORT_SYMBOL(urt_recover);
