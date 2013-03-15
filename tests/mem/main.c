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
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <urt.h>

int main()
{
	int ret;
	int exit_status = 0;
	int i;
	int *mem = NULL;
	urt_sem *req = NULL;
	urt_sem *res = NULL;

	urt_log("main: starting test...\n");

	for (i = 0; i < 20; ++i)
		if (fork() == 0)
		{
			execl("./add", "./add", (void *)NULL);
			_Exit(EXIT_FAILURE);
		}

	ret = urt_init();
	if (ret)
	{
		urt_log("main: init returned %d\n", ret);
		exit_status = EXIT_FAILURE;
		goto exit_no_init;
	}
	req = urt_shsem_new("TSTREQ", 0);
	res = urt_shsem_new("TSTRES", 0);
	if (req == NULL || res == NULL)
	{
		urt_log("main: no shared sem\n");
		exit_status = EXIT_FAILURE;
		goto exit_no_sem;
	}
	urt_log("main: sem allocated\n");
	mem = urt_shmem_new("TSTMEM", 4 * sizeof(*mem));
	if (mem == NULL)
	{
		urt_log("main: no shared mem\n");
		exit_status = EXIT_FAILURE;
		goto exit_no_mem;
	}
	urt_log("main: mem allocated\n");
	mem[0] = mem[1] = mem[2] = mem[3] = 0;
	for (i = 0; i < 20; ++i)
		urt_sem_post(req);
	for (i = 0; i < 20; ++i)
		urt_sem_wait(res);
	if (!(mem[0] == 20 && mem[1] == 40 && mem[2] == 60 && mem[3] == -20))
	{
		urt_log("main: bad synchronization (wrong results)");
		exit_status = EXIT_FAILURE;
	}
	for (i = 0; i < 20; ++i)
		wait(NULL);
	urt_shmem_delete(mem);
exit_no_mem:
	if (req)
		urt_shsem_delete(req);
	if (res)
		urt_shsem_delete(res);
exit_no_sem:
	urt_free();
	urt_log("main: test done\n");

exit_no_init:
	return exit_status;
}
