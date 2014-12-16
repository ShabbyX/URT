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
URT_MODULE_DESCRIPTION("cond test: read");

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
static urt_task *test_task = NULL;

static void _test(urt_task *task, void *data)
{
	unsigned int read = 0;
	while (true)
	{
		urt_mutex_lock(mutex);
		while (mem[11] == 0)
		{
			if (mem[10] == 1000)
			{
				urt_mutex_unlock(mutex);
				goto exit_done;
			}
			urt_cond_wait(read_cnd, mutex);
		}
		if (mem[11] <= 0)
			urt_err("take: count is negative (%d)\n", mem[11]);
		if (mem[10] >= 1000)
		{
			urt_err("take: start is above 1000 (%d)\n", mem[10]);
			break;
		}
		mem[12] += mem[mem[10] % 10];
		++mem[10];
		--mem[11];
		++read;
		urt_cond_signal(write_cnd);
		urt_mutex_unlock(mutex);
	}
exit_done:
	urt_err("take: read %u numbers\n", read);
	done = 1;
}

static void _cleanup(void)
{
	urt_task_delete(test_task);
	urt_shcond_detach(write_cnd);
	urt_shcond_detach(read_cnd);
	urt_shmutex_delete(mutex);
	urt_shmem_delete(mem);
	urt_exit();
}

static int test_start(int *unused)
{
	int ret;
	urt_time start;

	if (write_cond_name == NULL || read_cond_name == NULL || mem_name == NULL || mutex_name == NULL)
	{
		urt_out("Missing obligatory argument <write_cond_name=name> <read_cond_name=name> <mem_name=name> <mutex_name=name>\n");
		return EXIT_FAILURE;
	}

	urt_out("take: spawned\n");
	ret = urt_init();
	if (ret)
	{
		urt_out("take: init returned %d\n", ret);
		goto exit_no_init;
	}
	start = urt_get_time();
	do
	{
		mem = urt_shmem_attach(mem_name);
		urt_sleep(10000000);
	} while (mem == NULL && urt_get_time() - start < 1000000000ll);
	if (mem == NULL)
	{
		urt_out("take: no shared mem\n");
		goto exit_no_mem;
	}
	start = urt_get_time();
	do
	{
		mutex = urt_shmutex_attach(mutex_name);
		urt_sleep(10000000);
	} while (mutex == NULL && urt_get_time() - start < 1000000000ll);
	if (mutex == NULL)
	{
		urt_out("take: no shared mutex\n");
		goto exit_no_mutex;
	}
	start = urt_get_time();
	do
	{
		if (!write_cnd)
			write_cnd = urt_shcond_attach(write_cond_name);
		if (!read_cnd)
			read_cnd = urt_shcond_attach(read_cond_name);
		urt_sleep(10000000);
	} while ((write_cnd == NULL || read_cnd == NULL) && urt_get_time() - start < 1000000000ll);
	if (write_cnd == NULL || read_cnd == NULL)
	{
		urt_out("take: no shared cond\n");
		goto exit_no_cnd;
	}
	urt_out("take: cnd attached\n");
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
	test_task = urt_task_new(_test);
	if (test_task == NULL)
	{
		urt_out("take: failed to create task\n");
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
	urt_out("take: test done\n");
}
