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

#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <urt.h>

int main()
{
	int ret;
	int exit_status = 0;
	int i;
	urt_sem *sem = NULL;

	urt_out("main: starting test...\n");

	for (i = 0; i < 20; ++i)
		if (fork() == 0)
		{
			execl("./wait", "./wait", (void *)NULL);
			_Exit(EXIT_FAILURE);
		}

	ret = urt_init();
	if (ret)
	{
		urt_out("main: init returned %d\n", ret);
		exit_status = EXIT_FAILURE;
		goto exit_no_init;
	}
	sem = urt_shsem_new("TSTSEM", 5);
	if (sem == NULL)
	{
		urt_out("main: no shared sem\n");
		exit_status = EXIT_FAILURE;
		goto exit_no_sem;
	}
	urt_out("main: sem allocated\n");
	urt_out("main: waiting for 3 seconds\n");
	urt_sleep(3000000000ll);
	for (i = 0; i < 15; ++i)
		urt_sem_post(sem);
	urt_out("main: waiting for children\n");
	for (i = 0; i < 20; ++i)
		wait(NULL);
	urt_out("main: cleaning up\n");
	urt_shsem_delete(sem);
exit_no_sem:
	urt_exit();
	urt_out("main: test done\n");
exit_no_init:
	return exit_status;
}
