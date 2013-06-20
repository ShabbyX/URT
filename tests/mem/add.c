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
#include <urt.h>

int main()
{
	int exit_status = 0;
	int ret;
	urt_sem *req = NULL;
	urt_sem *res = NULL;
	int *mem = NULL;
	urt_time start;

	urt_out("add: spawned\n");

	ret = urt_init();	/* tests race condition for urt_init */
	if (ret)
	{
		urt_out("add: init returned %d\n", ret);
		exit_status = EXIT_FAILURE;
		goto exit_no_init;
	}
	start = urt_get_time();
	do
	{
		if (!req)
			req = urt_shsem_attach("TSTREQ");
		if (!res)
			res = urt_shsem_attach("TSTRES");
		urt_sleep(10000000);
	} while ((req == NULL || res == NULL) && urt_get_time() - start < 1000000000ll);
	if (req == NULL || res == NULL)
	{
		urt_out("add: no shared sem\n");
		exit_status = EXIT_FAILURE;
		goto exit_no_sem;
	}
	urt_out("add: sem attached\n");
	start = urt_get_time();
	do
	{
		mem = urt_shmem_attach("TSTMEM");
		urt_sleep(10000000);
	} while (mem == NULL && urt_get_time() - start < 1000000000ll);
	if (mem == NULL)
	{
		urt_out("add: no shared mem\n");
		exit_status = EXIT_FAILURE;
		goto exit_no_mem;
	}
	urt_out("add: mem attached\n");
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
	urt_exit();
	urt_out("add: test done\n");
exit_no_init:
	return exit_status;
}
