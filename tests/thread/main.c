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

static urt_sem *sync_sem = NULL;
static urt_sem *done_sem = NULL;

static void periodic(urt_task *task, void *data)
{
	int i;
	urt_sem *sem = data;
	urt_time start_time = urt_get_time();

	urt_task_on_start(task);

	for (i = 0; i < 20; ++i)
	{
		urt_log("Periodic: %llu: Time to next period: %llu\n", urt_get_time() - start_time, urt_task_period_time_left(task));
		urt_task_wait_period(task);
		urt_sem_post(sem);
	}

	urt_task_on_stop(task);
	urt_sem_post(done_sem);
}

static void normal(urt_task *task, void *data)
{
	int i;

	urt_task_on_start(task);

	for (i = 0; i < 20; ++i)
	{
		urt_sem_wait(sync_sem);
		urt_sleep(1000000);
		urt_log("Normal: Signaled by periodic thread\n");
	}

	urt_task_on_stop(task);
	urt_sem_post(done_sem);
}

int main()
{
	int ret;
	int exit_status = 0;
	urt_task *tp, *tn;
	urt_task_attr attr;

	urt_log("starting test...\n");
	ret = urt_init();
	if (ret)
	{
		urt_log("init returned %d\n", ret);
		exit_status = EXIT_FAILURE;
		goto exit_no_init;
	}
	sync_sem = urt_sem_new(0);
	done_sem = urt_sem_new(0);
	if (sync_sem == NULL || done_sem == NULL)
	{
		urt_log("no sem\n");
		exit_status = EXIT_FAILURE;
		goto exit_no_sem;
	}
	urt_log("sem allocated\n");

	tn = urt_task_new(normal);
	attr = (urt_task_attr){
		.period = 500000000,
		.start_time = urt_get_time() + 1000000000
	};
	tp = urt_task_new(periodic, sync_sem, &attr, &ret);
	if (tn == NULL || tp == NULL)
	{
		urt_log("failed to create tasks\n");
		if (tp == NULL)
			urt_log("periodic task creation returned %d\n", ret);
		exit_status = EXIT_FAILURE;
		goto exit_no_task;
	}

	urt_task_start(tn);
	urt_task_start(tp);

	urt_sem_wait(done_sem);
	urt_sem_wait(done_sem);
exit_no_task:
	urt_task_delete(tn);
	urt_task_delete(tp);
exit_no_sem:
	urt_sem_delete(sync_sem);
	urt_sem_delete(done_sem);
	urt_exit();
	urt_log("test done\n");
exit_no_init:
	return exit_status;
}
