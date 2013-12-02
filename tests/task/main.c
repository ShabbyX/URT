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
URT_MODULE_DESCRIPTION("task test: main");

URT_MODULE_PARAM_START()
URT_MODULE_PARAM_END()

static int test_start(int *unused);
static void test_body(int *unused);
static void test_end(int *unused);

URT_GLUE(test_start, test_body, test_end, int, interrupted, done)

static urt_sem *sync_sem = NULL;
static urt_sem *done_sem = NULL;
static urt_task *tp = NULL, *tn = NULL;
static bool stop_other_thread = false;

static void periodic(urt_task *task, void *data)
{
	int i;
	urt_sem *sem = data;
	urt_time start_time = urt_get_time();

	for (i = 0; i < 20 && !interrupted; ++i)
	{
		urt_out("Periodic: %lld: Time to next period: %lld (exec time: %lld)\n", urt_get_time() - start_time,
				urt_task_period_time_left(task), urt_get_exec_time());
		urt_task_wait_period(task);
		urt_sem_post(sem);
	}

	stop_other_thread = true;
	urt_sem_post(done_sem);
}

static void normal(urt_task *task, void *data)
{
	int i;

	for (i = 0; i < 20; ++i)
	{
		if (urt_sem_wait(sync_sem, &stop_other_thread))
		{
			urt_out("Normal: Canceled by periodic task\n");
			break;
		}
		else
			urt_out("Normal: Signaled by periodic task\n");
		urt_sleep(1000000);
	}

	urt_sem_wait(done_sem);
	done = 1;
}

static void _cleanup(void)
{
	urt_task_delete(tn);
	urt_task_delete(tp);
	urt_sem_delete(sync_sem);
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
	sync_sem = urt_sem_new(0);
	done_sem = urt_sem_new(0);
	if (sync_sem == NULL || done_sem == NULL)
	{
		urt_out("no sem\n");
		goto exit_no_sem;
	}
	urt_out("sem allocated\n");
	return 0;
exit_no_sem:
	_cleanup();
exit_no_init:
	return EXIT_FAILURE;
}

static void test_body(int *unused)
{
	int ret;
	urt_task_attr attr;

	tn = urt_task_new(normal);
	attr = (urt_task_attr){
		.period = 500000000,
		.start_time = urt_get_time() + 3000000000ll
	};
	tp = urt_task_new(periodic, sync_sem, &attr, &ret);
	if (tn == NULL || tp == NULL)
	{
		urt_out("failed to create tasks\n");
		if (tp == NULL)
			urt_out("periodic task creation returned %d\n", ret);
		goto exit_no_task;
	}

	urt_task_start(tn);
	urt_task_start(tp);
	return;
exit_no_task:
	done = 1;
}

static void test_end(int *unused)
{
	_cleanup();
	urt_out("test done\n");
}
