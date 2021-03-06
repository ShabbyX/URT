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
URT_MODULE_DESCRIPTION("mem test: add");

char *req_sem_name = NULL;
char *res_sem_name = NULL;
char *mem_name = NULL;

URT_MODULE_PARAM_START()
URT_MODULE_PARAM(req_sem_name, charp, "request sem name")
URT_MODULE_PARAM(res_sem_name, charp, "result sem name")
URT_MODULE_PARAM(mem_name, charp, "memory name")
URT_MODULE_PARAM_END()

static int test_start(int *unused);
static void test_body(int *unused);
static void test_end(int *unused);

URT_GLUE(test_start, test_body, test_end, int, interrupted, done)

static urt_sem *req = NULL;
static urt_sem *res = NULL;
static int *mem = NULL;
static urt_task *test_task = NULL;

static void _test(urt_task *task, void *data)
{
	urt_sem_wait(req);
	mem[0] += 1;
	urt_sem_post(req);
	urt_sem_wait(req);
	mem[1] += 2;
	urt_sem_post(req);
	urt_sem_wait(req);
	mem[2] += 3;
	urt_sem_post(req);
	urt_sem_wait(req);
	mem[3] += -1;
	urt_sem_post(res);
	done = 1;
}

static void _cleanup(void)
{
	urt_task_delete(test_task);
	urt_shsem_delete(req);
	urt_shsem_delete(res);
	urt_shmem_delete(mem);
	urt_exit();
}

static int test_start(int *unused)
{
	int ret;
	urt_time start;

	if (req_sem_name == NULL || res_sem_name == NULL || mem_name == NULL)
	{
		urt_out("Missing obligatory arguments <req_sem_name=name> <res_sem_name=name> <mem_name=name>\n");
		return EXIT_FAILURE;
	}

	urt_out("add: spawned\n");
	ret = urt_init();	/* tests race condition for urt_init */
	if (ret)
	{
		urt_out("add: init returned %d\n", ret);
		goto exit_no_init;
	}
	start = urt_get_time();
	do
	{
		if (!req)
			req = urt_shsem_attach(req_sem_name);
		if (!res)
			res = urt_shsem_attach(res_sem_name);
		urt_sleep(10000000);
	} while ((req == NULL || res == NULL) && urt_get_time() - start < 1000000000ll);
	if (req == NULL || res == NULL)
	{
		urt_out("add: no shared sem\n");
		goto exit_no_sem;
	}
	urt_out("add: sem attached\n");
	start = urt_get_time();
	do
	{
		mem = urt_shmem_attach(mem_name);
		urt_sleep(10000000);
	} while (mem == NULL && urt_get_time() - start < 1000000000ll);
	if (mem == NULL)
	{
		urt_out("add: no shared mem\n");
		goto exit_no_mem;
	}
	urt_out("add: mem attached\n");
	return 0;
exit_no_sem:
exit_no_mem:
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
	urt_out("add: test done\n");
}
