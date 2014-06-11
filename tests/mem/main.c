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
URT_MODULE_DESCRIPTION("mem test: main");

URT_MODULE_PARAM_START()
URT_MODULE_PARAM_END()

static int test_start(int *unused);
static void test_body(int *unused);
static void test_end(int *unused);

URT_GLUE(test_start, test_body, test_end, int, interrupted, done)

static urt_sem *req = NULL;
static urt_sem *res = NULL;
static int *mem = NULL;
static urt_task *check_task = NULL;

static void _check(urt_task *task, void *data)
{
	int i;
	for (i = 0; i < 20; ++i)
		urt_sem_post(req);
	for (i = 0; i < 20; ++i)
		urt_sem_wait(res);
	done = 1;
}

static void _cleanup(void)
{
	urt_task_delete(check_task);
	urt_shsem_delete(req);
	urt_shsem_delete(res);
	urt_shmem_delete(mem);
	urt_exit();
}

static int test_start(int *unused)
{
	int ret;
	urt_out("main: starting test...\n");
	ret = urt_init();
	if (ret)
	{
		urt_out("main: init returned %d\n", ret);
		goto exit_no_init;
	}
	req = urt_shsem_new("TSTREQ", 0);
	res = urt_shsem_new("TSTRES", 0);
	if (req == NULL || res == NULL)
	{
		urt_out("main: no shared sem\n");
		goto exit_no_sem;
	}
	urt_out("main: sem allocated\n");
	mem = urt_shmem_new("TSTMEM", 4 * sizeof *mem);
	if (mem == NULL)
	{
		urt_out("main: no shared mem\n");
		goto exit_no_mem;
	}
	urt_out("main: mem allocated\n");
	mem[0] = mem[1] = mem[2] = mem[3] = 0;
	return 0;
exit_no_sem:
exit_no_mem:
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
	if (!(mem[0] == 20 && mem[1] == 40 && mem[2] == 60 && mem[3] == -20))
		urt_out("main: bad synchronization (wrong results)\n");
	_cleanup();
	urt_out("main: test done\n");
}
