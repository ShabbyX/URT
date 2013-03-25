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
#include <urt_internal.h>
#include <urt_mem.h>
#include "names.h"
#include "mem_internal.h"

void *(urt_mem_new)(size_t size, int *error, ...)
{
	void *mem = malloc(size);
	if (URT_UNLIKELY(mem == NULL))
		if (error)
			*error = URT_NO_MEM;
	return mem;
}

static void *_shmem_common(const char *name, size_t size, int *error, int flags)
{
	char n[URT_SYS_NAME_LEN];
	int fd = -1;
	void *mem = NULL;

	if (urt_convert_name(n, name) != URT_SUCCESS)
		goto exit_bad_name;

	fd = shm_open(n, O_RDWR | flags, S_IRWXU | S_IRWXG | S_IRWXO);
	if (fd == -1)
		goto exit_bad_open;

	if (flags & O_CREAT)
	{
		if (ftruncate(fd, size) == -1)
			goto exit_bad_truncate;
	}
	else
	{
		struct stat ms;
		if (fstat(fd, &ms) == -1)
			goto exit_bad_stat;
		size = ms.st_size;
	}

	mem = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (mem == MAP_FAILED)
		goto exit_bad_map;

	close(fd);

	return mem;
exit_bad_open:
exit_bad_truncate:
exit_bad_stat:
exit_bad_map:
	if (error)
	{
		switch (errno)
		{
		case EEXIST:
			*error = URT_EXISTS;
			break;
		case EINVAL:
			*error = URT_BAD_VALUE;
			break;
		case ENOMEM:
			*error = URT_NO_MEM;
			break;
		default:
			*error = URT_FAIL;
		}
	}
	goto exit_fail;
exit_bad_name:
	if (error)
		*error= URT_BAD_NAME;
	goto exit_fail;
exit_fail:
	if (fd != -1)
	{
		close(fd);
		shm_unlink(n);
	}
	return NULL;
}

void *urt_global_mem_get(const char *name, size_t size, int *error)
{
	return _shmem_common(name, size, error, O_CREAT);
}

void *urt_sys_shmem_new(const char *name, size_t size, int *error)
{
	return _shmem_common(name, size, error, O_CREAT | O_EXCL);
}

void *urt_sys_shmem_attach(const char *name, int *error)
{
	return _shmem_common(name, 0, error, 0);
}

void urt_global_mem_free(const char *name)
{
	char n[URT_SYS_NAME_LEN];

	if (urt_convert_name(n, name) == URT_SUCCESS)
		shm_unlink(n);
}

void urt_shmem_detach(void *mem)
{
	urt_shmem_detach_with_callback(mem, NULL);
}

void urt_shmem_detach_with_callback(void *mem, void (*f)(void *))
{
	char n[URT_SYS_NAME_LEN];
	urt_registered_object *ro;

	ro = urt_get_object_by_addr(mem);
	if (ro == NULL)
		return;
	if (f && ro->count == 1)
		f(mem);
	if (urt_convert_name(n, ro->name) == URT_SUCCESS)
		shm_unlink(n);
	urt_deregister(ro);
}
