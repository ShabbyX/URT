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
#include <urt_task.h>
#include <urt_mem.h>

void urt_task_delete(urt_task *task)
{
	if (task != NULL)
		pthread_join(task->tid, NULL);
	urt_mem_delete(task);
}

static void *_task_wrapper(void *t)
{
	urt_task *task = t;
	urt_time now = urt_get_time();

	if (task->attr.start_time == 0)
		task->attr.start_time = now;
	else
		urt_sleep(task->attr.start_time - now);

	task->func(task, task->data);

	return NULL;
}

int urt_task_start(urt_task *task)
{
	pthread_attr_t attr;
	int ret;

	if ((ret = pthread_attr_init(&attr)))
		return ret;

	pthread_attr_setstacksize(&attr, task->attr.stack_size);

	if ((ret = pthread_create(&task->tid, &attr, _task_wrapper, task)))
		goto exit_bad_create;

	pthread_attr_destroy(&attr);

	return 0;
exit_bad_create:
	pthread_attr_destroy(&attr);
	return ret;
}

static void _update_start_time(urt_task *task)
{
	urt_time cur;
	urt_task_attr *attr = &task->attr;

	/* make sure task is periodic */
	if (URT_UNLIKELY(attr->period <= 0))
		return;

	/* if already calculated next period, ignore it until urt_task_wait_period is called */
	if (task->next_period_calculated)
		return;

	cur = urt_get_time();
	if (cur > attr->start_time)
		attr->start_time += attr->period;
	task->next_period_calculated = true;
}

void urt_task_wait_period(urt_task *task)
{
	_update_start_time(task);
	urt_sleep(task->attr.start_time - urt_get_time());
	task->next_period_calculated = false;
}

urt_time urt_task_next_period(urt_task *task)
{
	_update_start_time(task);
	return task->attr.start_time;
}
