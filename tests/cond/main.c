/*
 * Copyright (C) 2013-2014  Shahbaz Youssefi <ShabbyX@gmail.com>
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
URT_MODULE_DESCRIPTION("cond test: main");

char *write_cond_name = NULL;
char *read_cond_name = NULL;
char *mem_name = NULL;
char *mutex_name = NULL;

URT_MODULE_PARAM_START()
URT_MODULE_PARAM(write_cond_name, charp, "producer cond name")
URT_MODULE_PARAM(read_cond_name, charp, "consumer cond name")
URT_MODULE_PARAM(mem_name, charp, "mem name")
URT_MODULE_PARAM(mutex_name, charp, "mutex name")
URT_MODULE_PARAM_END()

static int test_start(int *unused);
static void test_body(int *unused);
static void test_end(int *unused);

URT_GLUE(test_start, test_body, test_end, int, interrupted, done)

static urt_cond *write_cnd = NULL;
static urt_cond *read_cnd = NULL;
static urt_mutex *mutex = NULL;
static int *mem = NULL;
static urt_task *check_task = NULL;

static void _check(urt_task *task, void *data)
{
	int i;
	int ret;
	for (i = 0; i < 1000; ++i)
	{
		urt_mutex_lock(mutex);
		while (mem[11] == 10)
			if ((ret = urt_cond_wait(write_cnd, mutex)))
				urt_err("main: cond_wait returned %d\n", ret);
		if (mem[11] < 0 || mem[11] >= 10)
			urt_err("main: count is invalid (%d)\n", mem[11]);
		mem[(mem[10] + mem[11]) % 10] = i;
		++mem[11];
		urt_cond_broadcast(read_cnd);
		urt_mutex_unlock(mutex);
	}
	/* wait for data to be completely consumed */
	urt_mutex_lock(mutex);
	while (mem[11] != 0)
		urt_cond_wait(write_cnd, mutex);
	urt_mutex_unlock(mutex);
	done = 1;
}

static void _cleanup(void)
{
	urt_task_delete(check_task);
	urt_shcond_delete(write_cnd);
	urt_shcond_delete(read_cnd);
	urt_shmutex_delete(mutex);
	urt_shmem_delete(mem);
	urt_exit();
}

static int test_start(int *unused)
{
	int ret;

	if (write_cond_name == NULL || read_cond_name == NULL || mem_name == NULL || mutex_name == NULL)
	{
		urt_out("Missing obligatory argument <write_cond_name=name> <read_cond_name=name> <mem_name=name> <mutex_name=name>\n");
		return EXIT_FAILURE;
	}

	urt_out("main: starting test...\n");
	ret = urt_init();
	if (ret)
	{
		urt_out("main: init returned %d\n", ret);
		goto exit_no_init;
	}
	mem = urt_shmem_new(mem_name, 13 * sizeof *mem);	/* first 10 are the queue, then it's start, count and sum */
	if (mem == NULL)
	{
		urt_out("main: no shared mem\n");
		goto exit_no_mem;
	}
	memset(mem, 0, 13 * sizeof *mem);
	urt_out("main: mem allocated\n");
	mutex = urt_shmutex_new(mutex_name);
	if (mutex == NULL)
	{
		urt_out("main: no shared mutex\n");
		goto exit_no_mutex;
	}
	urt_out("main: mutex allocated\n");
	write_cnd = urt_shcond_new(write_cond_name);
	read_cnd = urt_shcond_new(read_cond_name);
	if (write_cnd == NULL || read_cnd == NULL)
	{
		urt_out("main: no shared cond\n");
		goto exit_no_cnd;
	}
	urt_out("main: cond allocated\n");
	return 0;
exit_no_cnd:
exit_no_mem:
exit_no_mutex:
	_cleanup();
exit_no_init:
	return EXIT_FAILURE;
}

static void test_body(int *unused)
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

static void test_end(int *unused)
{
	if (mem)
		if (mem[12] != 499500)
			urt_out("main: bad synchronization (wrong results)\n");

	_cleanup();
	urt_out("main: test done\n");
}
