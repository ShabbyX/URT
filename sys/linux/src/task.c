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

#include <errno.h>
#include <urt_internal.h>
#include <urt_task.h>
#include <urt_mem.h>

urt_task *(urt_task_new)(void (*func)(urt_task *, void *), void *data, urt_task_attr *attr, int *error, ...)
{
	urt_task *task;

	task = urt_mem_new(sizeof(*task), error);
	if (task == NULL)
		goto exit_fail;

	*task = (urt_task){
		.attr = {
			.stack_size = URT_DEFAULT_STACK_SIZE,
			.priority = URT_MIN_PRIORITY,
		},
		.func = func,
		.data = data
	};

	if (attr != NULL)
	{
		if (attr->period > 0)
			task->attr.period = attr->period;
		if (attr->start_time > 0)
			task->attr.start_time = attr->start_time;
		if (attr->stack_size > 0)
			task->attr.stack_size = attr->stack_size;
		if (urt_priority_is_valid(attr->priority))
			task->attr.priority = attr->priority;
		if (attr->uses_fpu)
			task->attr.uses_fpu = 1;
	}

	return task;
exit_fail:
	return NULL;
}

void urt_task_delete(urt_task *task)
{
	if (URT_LIKELY(task != NULL))
		pthread_join(task->tid, NULL);
	urt_mem_delete(task);
}

static void *_task_wrapper(void *t)
{
	urt_task *task = t;

	if (task->attr.start_time == 0)
		task->attr.start_time = urt_get_time();
	else
		urt_sleep(task->attr.start_time - urt_get_time());

	task->func(task, task->data);

	return NULL;
}

int urt_task_start(urt_task *task)
{
	pthread_attr_t attr;

	if (pthread_attr_init(&attr))
		return URT_NO_MEM;

	pthread_attr_setstacksize(&attr, task->attr.stack_size);

	if (pthread_create(&task->tid, &attr, _task_wrapper, task))
		goto exit_bad_create;

	pthread_attr_destroy(&attr);

	return URT_SUCCESS;
exit_bad_create:
	pthread_attr_destroy(&attr);
	return errno == EAGAIN?URT_AGAIN:URT_FAIL;
}

static void _update_start_time(urt_task_attr *attr)
{
	urt_time cur;

	/* make sure task is periodic */
	if (URT_UNLIKELY(attr->period <= 0))
		return;

	cur = urt_get_time();
	if (cur > attr->start_time)
		attr->start_time += attr->period;

	/* the following if only happens if the user has missed period */
	if (URT_UNLIKELY(cur > attr->start_time))
		attr->start_time += (cur - attr->start_time + attr->period - 1) / attr->period * attr->period;
}

void urt_task_wait_period(urt_task *task)
{
	_update_start_time(&task->attr);
	urt_sleep(task->attr.start_time - urt_get_time());
}

urt_time urt_task_next_period(urt_task *task)
{
	_update_start_time(&task->attr);
	return task->attr.start_time;
}
