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
#include <urt.h>

int main()
{
	int exit_status = 0;
	int ret;
	urt_sem *req = NULL;
	urt_sem *res = NULL;
	int *mem = NULL;

	urt_log("add: spawned\n");

	ret = urt_init();	/* tests race condition for urt_init */
	usleep(100000);		/* add for main to create semaphores and shared memory */
	if (ret)
	{
		urt_log("add: init returned %d\n", ret);
		exit_status = EXIT_FAILURE;
		goto exit_no_init;
	}
	req = urt_shsem_attach("TSTREQ");
	res = urt_shsem_attach("TSTRES");
	if (req == NULL || res == NULL)
	{
		urt_log("add: no shared sem\n");
		exit_status = EXIT_FAILURE;
		goto exit_no_sem;
	}
	urt_log("add: sem attached\n");
	mem = urt_shmem_attach("TSTMEM");
	if (mem == NULL)
	{
		urt_log("add: no shared mem\n");
		exit_status = EXIT_FAILURE;
		goto exit_no_mem;
	}
	urt_log("add: mem attached\n");
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
	urt_shmem_detach(mem);
exit_no_mem:
	if (req)
		urt_shsem_detach(req);
	if (res)
		urt_shsem_detach(res);
exit_no_sem:
	urt_free();
	urt_log("add: test done\n");

exit_no_init:
	return exit_status;
}
