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

#include <urt_setup.h>
#include <rtai_lxrt.h>

int urt_sys_init(void)
{
#ifndef __KERNEL__
	rt_allow_nonroot_hrt();
#endif
	if (!rt_is_hard_timer_running())
	{
		rt_set_oneshot_mode();
		start_rt_timer(nano2count(100000));
	}
	mlockall(MCL_CURRENT | MCL_FUTURE);
	return 0;
}
