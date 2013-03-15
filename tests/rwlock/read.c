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
	urt_rwlock *rwl = NULL;

	urt_log("read: spawned\n");

	ret = urt_init();
	urt_sleep(100000000);	/* wait for main to create rwlock */
	if (ret)
	{
		urt_log("read: init returned %d\n", ret);
		exit_status = EXIT_FAILURE;
		goto exit_no_init;
	}
	rwl = urt_shrwlock_attach("TSTRWL");
	if (rwl == NULL)
	{
		urt_log("read: no shared rwl\n");
		exit_status = EXIT_FAILURE;
		goto exit_no_rwl;
	}
	urt_log("read: rwl attached\n");
	ret = urt_rwlock_try_read_lock(rwl);
	urt_log("read: try read lock returned: %d\n", ret);
	ret = urt_rwlock_try_write_lock(rwl);
	urt_log("read: try write lock returned: %d\n", ret);
	urt_log("read: timed read lock for 1 second\n");
	ret = urt_rwlock_timed_read_lock(rwl, 1000000000);
	urt_log("read: timed read lock returned: %d\n", ret);
	urt_log("read: timed read lock for 4 second\n");
	ret = urt_rwlock_timed_read_lock(rwl, 4000000000ll);
	urt_log("read: timed read lock returned: %d\n", ret);
	urt_log("read: waiting for 1s\n");
	urt_sleep(1000000000);
	urt_log("read: read unlock\n");
	urt_rwlock_read_unlock(rwl);
	urt_shrwlock_detach(rwl);
exit_no_rwl:
	urt_free();
	urt_log("read: test done\n");
exit_no_init:
	return exit_status;
}
