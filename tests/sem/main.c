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

static int _start(void);
static void _body(void);
static void _end(void);

URT_GLUE(_start, _body, _end, interrupted, done)

static urt_sem *sem = NULL;
static urt_task *check_task = NULL;

static void _check(urt_task *task, void *data)
{
	int i;
	urt_out("main: waiting for 3 seconds\n");
	urt_sleep(3000000000ll);
	for (i = 0; i < 15; ++i)
		urt_sem_post(sem);
	done = 1;
}

static void _cleanup(void)
{
	urt_task_delete(check_task);
	urt_shsem_delete(sem);
	urt_exit();
}

static int _start(void)
{
	int ret;
	urt_out("main: starting test...\n");
	ret = urt_init();
	if (ret)
	{
		urt_out("main: init returned %d\n", ret);
		goto exit_no_init;
	}
	sem = urt_shsem_new("TSTSEM", 5);
	if (sem == NULL)
	{
		urt_out("main: no shared sem\n");
		goto exit_no_sem;
	}
	urt_out("main: sem allocated\n");
	return 0;
exit_no_sem:
	_cleanup();
exit_no_init:
	return EXIT_FAILURE;
}

static void _body(void)
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

static void _end(void)
{
	_cleanup();
	urt_out("main: test done\n");
}
