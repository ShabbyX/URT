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
	urt_sem *sem = NULL;

	urt_log("wait: spawned\n");

	ret = urt_init();	/* tests race condition for urt_init */
	urt_sleep(100000000);		/* wait for main to create semaphore */
	if (ret)
	{
		urt_log("wait: init returned %d\n", ret);
		exit_status = EXIT_FAILURE;
		goto exit_no_init;
	}
	sem = urt_shsem_attach("TSTSEM");
	if (sem == NULL)
	{
		urt_log("wait: no shared sem\n");
		exit_status = EXIT_FAILURE;
		goto exit_no_sem;
	}
	urt_log("wait: sem attached\n");
	urt_sem_wait(sem);
	urt_shsem_detach(sem);
exit_no_sem:
	urt_exit();
	urt_log("wait: test done\n");
exit_no_init:
	return exit_status;
}
