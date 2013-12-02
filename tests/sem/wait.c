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

static int test_start(int *unused);
static void test_body(int *unused);
static void test_end(int *unused);

URT_GLUE(test_start, test_body, test_end, int, interrupted, done)

static urt_sem *sem = NULL;
static urt_task *test_task = NULL;

static void _test(urt_task *task, void *data)
{
	urt_sem_wait(sem);
	done = 1;
}

static void _cleanup(void)
{
	urt_task_delete(test_task);
	urt_shsem_detach(sem);
	urt_exit();
}

static int test_start(int *unused)
{
	int ret;
	urt_time start;
	urt_out("wait: spawned\n");
	ret = urt_init();	/* tests race condition for urt_init */
	if (ret)
	{
		urt_out("wait: init returned %d\n", ret);
		goto exit_no_init;
	}
	start = urt_get_time();
	do
	{
		sem = urt_shsem_attach("TSTSEM");
		urt_sleep(10000000);
	} while (sem == NULL && urt_get_time() - start < 1000000000ll);
	if (sem == NULL)
	{
		urt_out("wait: no shared sem\n");
		goto exit_no_sem;
	}
	urt_out("wait: sem attached\n");
	return 0;
exit_no_sem:
	_cleanup();
exit_no_init:
	return EXIT_FAILURE;
}

static void test_body(int *unused)
{
	test_task = urt_task_new(_test);
	if (test_task == NULL)
	{
		urt_out("wait: failed to create task\n");
		goto exit_no_task;
	}
	urt_task_start(test_task);
	return;
exit_no_task:
	done = 1;
}

static void test_end(int *unused)
{
	_cleanup();
	urt_out("wait: test done\n");
}
