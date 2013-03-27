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

#include <urt.h>
#include <stdlib.h>

static urt_mutex *sync_mutex = NULL;
static urt_sem *done_sem = NULL;
int num = 0;

static void task(urt_task *task, void *data)
{
	int i;

	for (i = 0; i < 20; ++i)
	{
		urt_mutex_lock(sync_mutex);
		++num;
		urt_mutex_unlock(sync_mutex);
	}

	urt_sem_post(done_sem);
}

int main()
{
	int ret;
	int exit_status = 0;
	int i;
	int tasks_made = 0;
	urt_task *tasks[10];

	urt_out("starting test...\n");
	ret = urt_init();
	if (ret)
	{
		urt_out("init returned %d\n", ret);
		exit_status = EXIT_FAILURE;
		goto exit_no_init;
	}
	sync_mutex = urt_mutex_new();
	done_sem = urt_sem_new(0);
	if (sync_mutex == NULL || done_sem == NULL)
	{
		urt_out("no sem/mutex\n");
		exit_status = EXIT_FAILURE;
		goto exit_no_sem;
	}
	urt_out("sem allocated\n");

	for (i = 0; i < 10; ++i)
	{
		tasks[i] = urt_task_new(task);
		if (tasks[i] == NULL)
		{
			urt_out("failed to create tasks\n");
			exit_status = EXIT_FAILURE;
			goto exit_no_task;
		}
	}
	for (i = 0; i < 10; ++i)
		if (urt_task_start(tasks[i]))
			urt_out("failed to start task %d\n", i);
		else
			++tasks_made;

	for (i = 0; i < tasks_made; ++i)
		urt_sem_wait(done_sem);

	if (num != 10 * 20)
		urt_out("Wrong synchronization. Num is %d instead of 200\n", num);
	else
		urt_out("Successful synchronization\n");
exit_no_task:
	for (i = 0; i < 10; ++i)
		urt_task_delete(tasks[i]);
exit_no_sem:
	urt_mutex_delete(sync_mutex);
	urt_sem_delete(done_sem);
	urt_exit();
	urt_out("test done\n");
exit_no_init:
	return exit_status;
}
