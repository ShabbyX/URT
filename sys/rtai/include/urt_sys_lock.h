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

#ifdef __KERNEL__
# include <linux/kernel.h>
#endif
#include <rtai_sem.h>
#include <rtai_rwl.h>

URT_DECL_BEGIN

typedef struct urt_sem
{
	SEM sem;		/* kernel semaphore requires object (sem_ptr will point here) */
	SEM *sem_ptr;		/* user semaphore requires pointer (sem is unused) */
	unsigned int id;
} urt_sem;
typedef urt_sem urt_mutex;
typedef struct urt_rwlock
{
	RWL rwl;		/* similar to */
	RWL *rwl_ptr;		/* urt_sem */
	unsigned int id;
} urt_rwlock;

/* a helper macro is used for `, ##__VA_ARGS__` to correctly expand */
#define urt_mutex_new(...) urt_mutex_new_(unused, ##__VA_ARGS__, NULL)
#define urt_mutex_new_(unused, ...) urt_mutex_new(__VA_ARGS__)
URT_ATTR_WARN_UNUSED urt_mutex *(urt_mutex_new)(int *error, ...);
#define urt_mutex_delete(m) urt_sem_delete(m)

#define urt_shmutex_detach(m) urt_shsem_detach(m)

#define urt_mutex_lock(...) urt_sem_wait(__VA_ARGS__)
#define urt_mutex_timed_lock(m, t) urt_sem_timed_wait(m, t)
#define urt_mutex_try_lock(m) urt_sem_try_wait(m)
#define urt_mutex_unlock(m) urt_sem_post(m)

URT_DECL_END

#endif
