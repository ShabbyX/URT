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

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <urt_sys_setup.h>
#include "names.h"

void urt_sys_force_clear_name(urt_registered_object *ro)
{
	char n[URT_SYS_NAME_LEN];
	int fd = -1;
	urt_sem *sem = NULL;

	if (urt_convert_name(n, ro->name) != URT_SUCCESS)
		goto exit_bad_name;

	/* try cleaning name if it is shared memory */
	fd = shm_open(n, O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
	if (fd != -1)
	{
		close(fd);
		shm_unlink(n);
		return;
	}

	/* try cleaning name if it is semaphore */
	sem = sem_open(n, 0, S_IRWXU | S_IRWXG | S_IRWXO, 0);
	if (sem != SEM_FAILED)
	{
		sem_close(sem);
		sem_unlink(n);
		return;
	}

	/* mutex and rwlock have been accounted for in shared memory case */
exit_bad_name:
	return;
}
