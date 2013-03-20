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

#ifndef URT_THREAD_H
#define URT_THREAD_H

#include <urt_stdtypes.h>
#include <urt_compiler.h>
#include <urt_time.h>

URT_DECL_BEGIN

typedef struct urt_task_attr
{
	urt_time period;
	urt_time start_time;
	size_t stack_size;
	int priority;
	char uses_fpu;
} urt_task_attr;

typedef struct urt_task
{
	urt_task_attr attr;
	pthread_t tid;
} urt_task;

#define URT_MAX_PRIORITY 0
#define URT_MIN_PRIORITY 0
#define URT_MORE_PRIORITY 0

static inline bool urt_priority_is_valid(int p)
{
	return p == 0;
}

/*
 * If data is not provided, NULL will be used.
 *
 * If attr is NULL or any of the attributes are set to zero, they will be set to default.
 * If error is to be provided, the attr parameter needs to be provided too, even if it's NULL.
 *
 */
URT_ATTR_WARN_UNUSED urt_task *(urt_task_new)(void (*func)(urt_task *, void *), void *data, urt_task_attr *attr, int *error, ...);
void urt_task_delete(urt_task *task);

int urt_task_start(urt_task *task);
urt_task *urt_task_self(void);
static inline bool urt_task_is_rt_context(void) { return urt_task_self() != NULL; }
void urt_task_on_start(urt_task *task);
void urt_task_wait_period(urt_task *task);
void urt_task_on_stop(urt_task *task);
urt_time urt_task_next_period(urt_task *task);	/* TODO: in implementation do a `while (cur > start_time) start_time += period;` before returning start_time */
static inline urt_task_period_time_left(urt_task *task) { return urt_task_next_period(task) - urt_get_time(); }

URT_DECL_END

#endif
