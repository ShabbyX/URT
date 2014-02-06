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

#include <urt_utils.h>
#include <urt_internal.h>

#define MADE_RT_CONTEXT 0x01

int urt_make_rt_context(int *prev)
{
#ifndef __KERNEL__
	char name[URT_NAME_LEN + 1] = {0};
	int ret;
#endif
	if (prev == NULL)
		return EINVAL;
	*prev = 0;

	if (urt_is_rt_context())
		return 0;
#ifdef __KERNEL__
	/* I don't think you can temporarily make current thread real-time in RTAI in kernel space */
	/* Note: or maybe you can. Take a look at rt_kthread_init */
	return ENOTSUP;
#else
	ret = urt_get_free_name(name);
	if (ret)
		return ret;
	if (rt_task_init_schmod(nam2num(name), RT_SCHED_LINUX_PRIORITY, URT_DEFAULT_STACK_SIZE, 0, SCHED_FIFO, 0xff) == NULL)
		return ENOSPC;
	*prev |= MADE_RT_CONTEXT;
	/* Note: no need to call rt_make_hard_real_time, because only real-time **context** is needed */
	return 0;
#endif
}
URT_EXPORT_SYMBOL(urt_make_rt_context);

void urt_unmake_rt_context(int prev)
{
#ifdef __KERNEL__
	return;
#else
	if (prev & MADE_RT_CONTEXT)
	{
		rt_make_soft_real_time();
		if (rt_buddy() != NULL)
			rt_task_delete(NULL);
	}
#endif
}
URT_EXPORT_SYMBOL(urt_unmake_rt_context);
