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

#ifndef URT_SYS_TIME_H
#define URT_SYS_TIME_H

#ifdef __KERNEL__
# include <linux/kernel.h>
#endif
#include <rtai_lxrt.h>

URT_DECL_BEGIN

typedef int64_t urt_time;

#define urt_get_time rt_get_time_ns
#define urt_sleep(t) rt_sleep(nano2count(t))

static inline urt_time urt_get_exec_time(void)
{
	RT_TASK *task;
#ifdef __KERNEL__
	task = rt_whoami();
	if (task == NULL)
		return 0;
	return count2nano(task->exectime[0]);
#else
	RTIME exectime[3];
	task = rt_buddy();
	if (task == NULL)
		return 0;
	rt_get_exectime(task, exectime);
	return count2nano(exectime[0]);
#endif
}

URT_DECL_END

#endif
