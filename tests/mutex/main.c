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

URT_MODULE_LICENSE("GPL");
URT_MODULE_AUTHOR("Shahbaz Youssefi");
URT_MODULE_DESCRIPTION("mutex test: main");

URT_MODULE_PARAM_START()
URT_MODULE_PARAM_END()

static int test_start(int *unused);
static void test_body(int *unused);
static void test_end(int *unused);

URT_GLUE(test_start, test_body, test_end, int, interrupted, done)

static urt_mutex *sync_mutex = NULL;
static urt_sem *done_sem = NULL;
static urt_task *tasks[10] = {NULL};
static urt_task *check_task = NULL;
static int tasks_made = 0;
static int num = 0;

static void _task(urt_task *task, void *data)
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

static void _check_task(urt_task *task, void *data)
{
	int i;
	for (i = 0; i < tasks_made; ++i)
		urt_sem_wait(done_sem);
	done = 1;
}

static void _cleanup(void)
{
	int i;
	for (i = 0; i < 10; ++i)
		urt_task_delete(tasks[i]);
	urt_task_delete(check_task);
	urt_mutex_delete(sync_mutex);
	urt_sem_delete(done_sem);
	urt_exit();
}

static int test_start(int *unused)
{
	int ret;

	urt_out("starting test...\n");
	ret = urt_init();
	if (ret)
	{
		urt_out("init returned %d\n", ret);
		goto exit_no_init;
	}
	sync_mutex = urt_mutex_new();
	done_sem = urt_sem_new(0);
	if (sync_mutex == NULL || done_sem == NULL)
	{
		urt_out("no sem/mutex\n");
		goto exit_no_sem;
	}
	urt_out("sem allocated\n");

	return 0;
exit_no_init:
exit_no_sem:
	_cleanup();
	return EXIT_FAILURE;
}

static void test_body(int *unused)
{
	int i;
	for (i = 0; i < 10; ++i)
	{
		tasks[i] = urt_task_new(_task);
		if (tasks[i] == NULL)
		{
			urt_out("failed to create tasks\n");
			goto exit_no_task;
		}
	}
	for (i = 0; i < 10; ++i)
		if (urt_task_start(tasks[i]))
			urt_out("failed to start task %d\n", i);
		else
			++tasks_made;

	check_task = urt_task_new(_check_task);
	if (check_task == NULL)
	{
		urt_out("failed to create check task\n");
		goto exit_no_task;
	}
	if (urt_task_start(check_task))
		urt_out("failed to start check task\n");

	return;
exit_no_task:
	done = 1;
}

static void test_end(int *unused)
{
	_cleanup();
	if (num != tasks_made * 20)
		urt_out("Wrong synchronization. Num is %d instead of %d\n", num, tasks_made * 20);
	else
		urt_out("Successful synchronization\n");
	urt_out("test done\n");
}
