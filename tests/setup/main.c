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

static int exit_status;
static urt_sem *sem = NULL;

static int _start(void)
{
	int ret;

	urt_out("starting test...\n");
	ret = urt_init();
	if (ret)
	{
		urt_out("init returned %d\n", ret);
		exit_status = EXIT_FAILURE;
	}
	return exit_status;
}

static void _body(void)
{
	sem = urt_shsem_new("TSTSEM", 1);
	if (sem == NULL)
	{
		urt_out("no shared sem\n");
		exit_status = EXIT_FAILURE;
		goto exit_fail;
	}
	urt_out("sem allocated\n");
	urt_out("Sleeping for 5 seconds...\n");
	urt_out("Time before sleep: %llu\n", urt_get_time());
	urt_sleep(5000000000ll);
	urt_out("Time after sleep: %llu\n", urt_get_time());
exit_fail:
	done = 1;
}

static void _end(void)
{
	if (sem)
		urt_shsem_delete(sem);
	urt_exit();
	urt_out("test done\n");
}

#if 0
int main(void)
{
	int ret;
	int exit_status = 0;
	urt_sem *sem = NULL;

	urt_out("starting test...\n");
	ret = urt_init();
	if (ret)
	{
		urt_out("init returned %d\n", ret);
		exit_status = EXIT_FAILURE;
		goto exit_no_init;
	}
	sem = urt_shsem_new("TSTSEM", 1);
	if (sem == NULL)
	{
		urt_out("no shared sem\n");
		exit_status = EXIT_FAILURE;
		goto exit_no_sem;
	}
	urt_out("sem allocated\n");
	urt_out("Sleeping for 5 seconds...\n");
	urt_out("Time before sleep: %llu\n", urt_get_time());
	urt_sleep(5000000000ll);
	urt_out("Time after sleep: %llu\n", urt_get_time());
	urt_shsem_delete(sem);
exit_no_sem:
	urt_exit();
	urt_out("test done\n");
exit_no_init:
	return exit_status;
}
#endif
