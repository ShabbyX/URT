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
	urt_time start;

	urt_out("read: spawned\n");

	ret = urt_init();
	if (ret)
	{
		urt_out("read: init returned %d\n", ret);
		exit_status = EXIT_FAILURE;
		goto exit_no_init;
	}
	start = urt_get_time();
	do
	{
		rwl = urt_shrwlock_attach("TSTRWL");
		urt_sleep(10000000);
	} while (rwl == NULL && urt_get_time() - start < 1000000000ll);
	if (rwl == NULL)
	{
		urt_out("read: no shared rwl\n");
		exit_status = EXIT_FAILURE;
		goto exit_no_rwl;
	}
	urt_out("read: rwl attached\n");
	urt_sleep(500000000);
	ret = urt_rwlock_try_read_lock(rwl);
	urt_out("read: try read lock returned: %d\n", ret);
	if (ret == URT_SUCCESS)
		urt_rwlock_read_unlock(rwl);
	ret = urt_rwlock_try_write_lock(rwl);
	urt_out("read: try write lock returned: %d\n", ret);
	if (ret == URT_SUCCESS)
		urt_rwlock_write_unlock(rwl);
	urt_out("read: timed read lock for 1 second\n");
	ret = urt_rwlock_timed_read_lock(rwl, 1000000000);
	urt_out("read: timed read lock returned: %d\n", ret);
	if (ret == URT_SUCCESS)
		urt_rwlock_read_unlock(rwl);
	urt_out("read: timed read lock for 4 second\n");
	ret = urt_rwlock_timed_read_lock(rwl, 4000000000ll);
	urt_out("read: timed read lock returned: %d\n", ret);
	urt_out("read: waiting for 3s\n");
	urt_sleep(3000000000ll);
	if (ret == URT_SUCCESS)
	{
		urt_out("read: read unlock\n");
		urt_rwlock_read_unlock(rwl);
	}
	urt_shrwlock_detach(rwl);
exit_no_rwl:
	urt_exit();
	urt_out("read: test done\n");
exit_no_init:
	return exit_status;
}
