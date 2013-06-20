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

#include <urt_internal.h>
#include <urt_mem.h>
#include <rtai_shm.h>
#include <rtai_registry.h>

void *(urt_mem_new)(size_t size, int *error, ...)
{
#ifdef __KERNEL__
	void *mem = vmalloc(size);
#else
	void *mem = malloc(size);
#endif
	if (URT_UNLIKELY(mem == NULL))
		if (error)
			*error = URT_NO_MEM;
	return mem;
}
URT_EXPORT_SYMBOL(urt_mem_new);

void *urt_global_mem_get(const char *name, size_t size, int *error)
{
	void *mem = rt_shm_alloc(nam2num(name), size, USE_VMALLOC);
	if (mem == NULL && error)
		*error = URT_NO_MEM;
	return mem;
}

void *urt_sys_shmem_new(const char *name, size_t size, int *error)
{
	unsigned long num = nam2num(name);
	void *mem;

	if (rt_get_adr(num))
		goto exit_exists;

	mem = rt_shm_alloc(num, size, USE_VMALLOC);
	if (mem == NULL && error)
		*error = URT_NO_MEM;
	return mem;
exit_exists:
	if (error)
		*error = URT_EXISTS;
	return NULL;
}

void *urt_sys_shmem_attach(const char *name, int *error)
{
	void *mem = rt_shm_alloc(nam2num(name), 0, USE_VMALLOC);
	if (mem == NULL && error)
		*error = URT_NO_OBJ;
	return mem;
}

void urt_global_mem_free(const char *name)
{
	rt_shm_free(nam2num(name));
}

void urt_shmem_detach(void *mem)
{
	urt_registered_object *ro;

	mem = (char *)mem - 16;
	ro = urt_get_object_by_id(*(unsigned int *)mem);
	if (ro == NULL)
		return;

	rt_shm_free(nam2num(ro->name));
	urt_deregister(ro, ro->address, ro->size, NULL, NULL);
}
URT_EXPORT_SYMBOL(urt_shmem_detach);
