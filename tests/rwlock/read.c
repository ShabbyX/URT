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

static urt_rwlock *rwl = NULL;
static urt_task *test_task = NULL;

static void _test(urt_task *task, void *data)
{
	int ret;
	urt_sleep(500000000);
	ret = urt_rwlock_try_read_lock(rwl);
	urt_out("read: try read lock returned: %d\n", ret);
	if (ret == URT_SUCCESS)
		urt_rwlock_read_unlock(rwl);
	ret = urt_rwlock_try_write_lock(rwl);
	urt_out("read: try write lock returned: %d\n", ret);
	if (ret == URT_SUCCESS)
		urt_rwlock_write_unlock(rwl);
	urt_out("read: timed read lock for 1 second\n");
	ret = urt_rwlock_timed_read_lock(rwl, 1000000000);
	urt_out("read: timed read lock returned: %d\n", ret);
	if (ret == URT_SUCCESS)
		urt_rwlock_read_unlock(rwl);
	urt_out("read: timed read lock for 4 second\n");
	ret = urt_rwlock_timed_read_lock(rwl, 4000000000ll);
	urt_out("read: timed read lock returned: %d\n", ret);
	urt_out("read: waiting for 3s\n");
	urt_sleep(3000000000ll);
	if (ret == URT_SUCCESS)
	{
		urt_out("read: read unlock\n");
		urt_rwlock_read_unlock(rwl);
	}
	done = 1;
}

static void _cleanup(void)
{
	urt_task_delete(test_task);
	urt_shrwlock_detach(rwl);
	urt_exit();
}

static int test_start(int *unused)
{
	int ret;
	urt_time start;
	urt_out("read: spawned\n");
	ret = urt_init();
	if (ret)
	{
		urt_out("read: init returned %d\n", ret);
		goto exit_no_init;
	}
	start = urt_get_time();
	do
	{
		rwl = urt_shrwlock_attach("TSTRWL");
		urt_sleep(10000000);
	} while (rwl == NULL && urt_get_time() - start < 1000000000ll);
	if (rwl == NULL)
	{
		urt_out("read: no shared rwl\n");
		goto exit_no_rwl;
	}
	urt_out("read: rwl attached\n");
	return 0;
exit_no_rwl:
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
	urt_out("read: test done\n");
}
