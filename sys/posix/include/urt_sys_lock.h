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

#ifndef URT_SYS_LOCK_H
#define URT_SYS_LOCK_H

#include <semaphore.h>
#include <pthread.h>

URT_DECL_BEGIN

typedef struct urt_sem
{
	sem_t sem;		/* private semaphore requires object (sem_ptr will point here) */
	sem_t *sem_ptr;		/* shared semaphore requires pointer (sem is unused) */
	unsigned int id;
} urt_sem;
typedef pthread_mutex_t urt_mutex;
typedef pthread_rwlock_t urt_rwlock;
typedef pthread_cond_t urt_cond;

URT_DECL_END

#endif
