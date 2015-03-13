/*
 * Copyright (C) 2015  Shahbaz Youssefi <ShabbyX@gmail.com>
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

/*
 * This file along with sys/ specific inline.c contain `extern` definitions of
 * inline functions, so that these functions would have external linkage.
 */

#ifndef __KERNEL__

#include "urt_mem.h"
#include "urt_lock.h"
#include "urt_task.h"

extern inline void urt_shmem_delete(void *mem);
URT_EXPORT_SYMBOL(urt_shmem_delete);

extern inline void urt_shsem_delete(urt_sem *sem);
URT_EXPORT_SYMBOL(urt_shsem_delete);
extern inline void urt_shmutex_delete(urt_mutex *mutex);
URT_EXPORT_SYMBOL(urt_shmutex_delete);
extern inline void urt_shrwlock_delete(urt_rwlock *rwl);
URT_EXPORT_SYMBOL(urt_shrwlock_delete);
extern inline void urt_shcond_delete(urt_cond *cond);
URT_EXPORT_SYMBOL(urt_shcond_delete);

extern inline int urt_priority(unsigned int n);
URT_EXPORT_SYMBOL(urt_priority);

#endif
