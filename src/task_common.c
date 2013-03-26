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

#include <urt_task.h>
#include <urt_mem.h>
#include "urt_internal.h"

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
URT_EXPORT_SYMBOL(urt_task_new);
