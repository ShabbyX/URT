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

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <urt_internal.h>
#include <urt_mem.h>
#include "names.h"
#include "mem_internal.h"

static void *_shmem_common(const char *name, size_t size, int *error, int flags)
{
	char n[URT_SYS_NAME_LEN];
	int fd = -1;
	void *mem = NULL;

	if (urt_convert_name(n, name))
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
		*error = errno;
	goto exit_fail;
exit_bad_name:
	if (error)
		*error = EINVAL;
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
	munmap(urt_global_mem, sizeof *urt_global_mem);
	/* Note: it's easier if urt_global_mem is never `shm_unlink`ed */
}

#include <urt_log.h>

static void _shmem_detach(struct urt_registered_object *ro, void *address, void *user_data)
{
	char n[URT_SYS_NAME_LEN];
	void (*callback)(void *, unsigned int) = user_data;

	if (callback)
		callback((char *)address + 16, ro->count);

	/* FIXME: return value of munmap for debug */
	if (munmap(address, ro->size))
		urt_err("munmap failed with error: %d\n", errno);
	if (ro->count == 0 && urt_convert_name(n, ro->name) == 0)
		shm_unlink(n);
}

void urt_shmem_detach(void *mem)
{
	urt_shmem_detach_with_callback(mem, NULL);
}

void urt_shmem_detach_with_callback(void *mem, void (*f)(void *, unsigned int))
{
	urt_registered_object *ro;

	if (mem == NULL)
		return;

	mem = (char *)mem - 16;
	ro = urt_get_object_by_id(*(unsigned int *)mem);
	if (ro == NULL)
		return;
	urt_deregister(ro, _shmem_detach, mem, f);
}
