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

static int test_start(void);
static void test_body(void);
static void test_end(void);

URT_GLUE(test_start, test_body, test_end, interrupted, done)

static urt_rwlock *rwl = NULL;
static urt_task *check_task = NULL;

static void _check(urt_task *task, void *data)
{
	int ret;
	urt_rwlock_write_lock(rwl);
	urt_out("main: waiting for 3 seconds\n");
	urt_sleep(3000000000ll);
	urt_out("main: write unlock\n");
	urt_rwlock_write_unlock(rwl);
	urt_sleep(1000000000ll);
	urt_out("main: waiting for write lock\n");
	ret = urt_rwlock_write_lock(rwl);
	urt_out("main: write lock returned: %d\n", ret);
	urt_shrwlock_delete(rwl);
	done = 1;
}

static void _cleanup(void)
{
	urt_task_delete(check_task);
	urt_shrwlock_delete(rwl);
	urt_exit();
}

static int test_start(void)
{
	int ret;
	urt_out("main: starting test...\n");
	ret = urt_init();
	if (ret)
	{
		urt_out("main: init returned %d\n", ret);
		goto exit_no_init;
	}
	rwl = urt_shrwlock_new("TSTRWL");
	if (rwl == NULL)
	{
		urt_out("main: no shared rwl\n");
		goto exit_no_rwl;
	}
	urt_out("main: rwl allocated\n");
	return 0;
exit_no_rwl:
	_cleanup();
exit_no_init:
	return EXIT_FAILURE;
}

static void test_body(void)
{
	check_task = urt_task_new(_check);
	if (check_task == NULL)
	{
		urt_out("main: failed to create task\n");
		goto exit_no_task;
	}
	urt_task_start(check_task);
	return;
exit_no_task:
	done = 1;
}

static void test_end(void)
{
	_cleanup();
	urt_out("main: test done\n");
}
