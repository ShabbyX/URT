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

#ifndef URT_MEM_H
#define URT_MEM_H

#include <stdlib.h>
#include <urt_stdtypes.h>
#include <urt_compiler.h>

URT_DECL_BEGIN

#define urt_mem_alloc(...) urt_mem_alloc(__VA_ARGS__, (int *)NULL)
URT_ATTR_MALLOC URT_ATTR_WARN_UNUSED void *(urt_mem_alloc)(size_t size, int *error, ...);
static inline void urt_mem_free(void *mem) { free(mem); }

#define urt_shmem_alloc(...) urt_shmem_alloc(__VA_ARGS__, (int *)NULL)
URT_ATTR_MALLOC URT_ATTR_WARN_UNUSED void *(urt_shmem_alloc)(const char *name, size_t size, int *error, ...);
#define urt_shmem_attach(...) urt_shmem_attach(__VA_ARGS__, (int *)NULL)
URT_ATTR_WARN_UNUSED void *(urt_shmem_attach)(const char *name, int *error, ...);
void urt_shmem_detach(void *mem);
static inline void urt_shmem_free(void *mem) { urt_shmem_detach(mem); }

URT_DECL_END

#endif
