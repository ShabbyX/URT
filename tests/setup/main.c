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
URT_MODULE_DESCRIPTION("setup test: main");

char *sem_name = NULL;

URT_MODULE_PARAM_START()
URT_MODULE_PARAM(sem_name, charp, "sem name")
URT_MODULE_PARAM_END()

static int test_start(int *unused);
static void test_body(int *unused);
static void test_end(int *unused);

URT_GLUE_NO_INTERRUPT(test_start, test_body, test_end, int)

static urt_sem *sem = NULL;
static int done = 0;

static int test_start(int *unused)
{
	int ret;
	int exit_status = 0;

	if (sem_name == NULL)
	{
		urt_out("Missing obligatory argument <sem_name=name>\n");
		return EXIT_FAILURE;
	}

	urt_out("starting test...\n");
	ret = urt_init();
	if (ret)
	{
		urt_out("init returned %d\n", ret);
		exit_status = EXIT_FAILURE;
		goto exit_no_init;
	}
	sem = urt_shsem_new(sem_name, 1);
	if (sem == NULL)
	{
		urt_out("no shared sem\n");
		exit_status = EXIT_FAILURE;
		goto exit_no_sem;
	}
	urt_out("sem allocated\n");
exit_no_init:
	return exit_status;
exit_no_sem:
	urt_exit();
	goto exit_no_init;
}

static void test_body(int *unused)
{
	urt_out("Sleeping for 2 seconds...\n");
	urt_out("Time before sleep: %lld\n", urt_get_time());
	urt_sleep(2000000000ll);
	urt_out("Time after sleep:  %lld\n", urt_get_time());
	done = 1;
}

static void test_end(int *unused)
{
	while (!done)
		urt_sleep(10000000);
	urt_shsem_delete(sem);
	urt_exit();
	urt_out("test done\n");
}
