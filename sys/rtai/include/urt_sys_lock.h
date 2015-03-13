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
#include "urt_sys_rtai.h"
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
typedef struct urt_cond
{
	CND cond;		/* similar to */
	CND *cond_ptr;		/* urt_sem */
	unsigned int id;
} urt_cond;

URT_DECL_END

#endif
