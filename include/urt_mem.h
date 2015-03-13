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

#ifndef URT_MEM_H
#define URT_MEM_H

#include "urt_stdtypes.h"
#include "urt_compiler.h"
#include "urt_defaults.h"
#include "urt_sys_mem.h"

URT_DECL_BEGIN

/* unnamed memory */
URT_ATTR_MALLOC URT_ATTR_WARN_UNUSED void *(urt_mem_new)(size_t size, int *error, ...);
/* void urt_mem_delete(void *mem); */
URT_ATTR_WARN_UNUSED void *(urt_mem_resize)(void *old_mem, size_t old_size, size_t size, int *error, ...);

/* shared memory */
URT_ATTR_MALLOC URT_ATTR_WARN_UNUSED void *(urt_shmem_new)(const char *name, size_t size, int *error, ...);
URT_ATTR_WARN_UNUSED void *(urt_shmem_attach)(const char *name, int *error, ...);
void urt_shmem_detach(void *mem);
URT_INLINE void urt_shmem_delete(void *mem) { urt_shmem_detach(mem); }

URT_DECL_END

#endif
