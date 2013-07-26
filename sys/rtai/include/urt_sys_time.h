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
# include <asm/div64.h>
#else
# include <unistd.h>
#endif
#include <rtai_lxrt.h>
#include "urt_sys_utils.h"

URT_DECL_BEGIN

typedef int64_t urt_time;

#define urt_get_time rt_get_time_ns
static inline void urt_sleep(urt_time t)
{
	if (urt_is_rt_context())
		rt_sleep(nano2count(t));
	else
	{
#ifdef __KERNEL__
		if (t <= 0)
			return;

		t += 999;
		do_div(t, 1000);	/* round up to micro seconds */

		/* per kernel doc recommendations: Documentation/timers/timers-howto.txt */
		if (t < 10)
			udelay(t);
		else if (t < 15000)
			/* a ~1.5% slack given, just for the heck of it */
			usleep_range(t, t + t / 64);
		else
		{
			/* the time is above 10s of ms.  The us part of it can be ignored and the time rounded up */
			t += 999;
			do_div(t, 1000);
			msleep(t);
		}
#else
		/* Note: this part is the same as POSIX's urt_sleep */
		urt_time ts, tu;
		if (t <= 0)
			return;

		/* Note: on some systems, usleep cannot be called for values greater than 1,000,000 */
		t = (t + 999) / 1000;	/* round up to micro seconds */
		ts = t / 1000000;
		tu = t % 1000000;

		sleep(ts);
		usleep(tu);
#endif
	}
}

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
