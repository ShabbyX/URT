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

int main(void)
{
	int ret;
	int exit_status = 0;
	int i;
	urt_rwlock *rwl = NULL;

	urt_out("main: starting test...\n");

	for (i = 0; i < 4; ++i)
		if (fork() == 0)
		{
			execl("./read", "./read", (void *)NULL);
			_Exit(EXIT_FAILURE);
		}

	ret = urt_init();
	if (ret)
	{
		urt_out("main: init returned %d\n", ret);
		exit_status = EXIT_FAILURE;
		goto exit_no_init;
	}
	rwl = urt_shrwlock_new("TSTRWL");
	if (rwl == NULL)
	{
		urt_out("main: no shared rwl\n");
		exit_status = EXIT_FAILURE;
		goto exit_no_rwl;
	}
	urt_out("main: rwl allocated\n");
	urt_rwlock_write_lock(rwl);
	urt_out("main: waiting for 3 seconds\n");
	urt_sleep(3000000000ll);
	urt_out("main: write unlock\n");
	urt_rwlock_write_unlock(rwl);
	urt_sleep(1000000000ll);
	urt_out("main: waiting for write lock\n");
	ret = urt_rwlock_write_lock(rwl);
	urt_out("main: write lock returned: %d\n", ret);
	urt_shrwlock_delete(rwl);
	urt_out("main: waiting for children\n");
	for (i = 0; i < 4; ++i)
		wait(NULL);
exit_no_rwl:
	urt_exit();
	urt_out("main: test done\n");
exit_no_init:
	return exit_status;
}
