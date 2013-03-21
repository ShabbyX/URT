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

#ifndef URT_SYS_THREAD_H
#define URT_SYS_THREAD_H

#include <pthread.h>

URT_DECL_BEGIN

typedef struct urt_task
{
	urt_task_attr attr;
	void (*func)(struct urt_task *, void *);
	void *data;
	pthread_t tid;
} urt_task;

#define URT_MAX_PRIORITY 0
#define URT_MIN_PRIORITY 0
#define URT_MORE_PRIORITY 0

static inline bool urt_priority_is_valid(int p)
{
	return p == 0;
}

static inline bool urt_priority_is_higher(int a, int b)
{
	return a > b;
}

urt_time urt_task_next_period(urt_task *task);
static inline urt_time urt_task_period_time_left(urt_task *task) { return urt_task_next_period(task) - urt_get_time(); }

URT_DECL_END

#endif
