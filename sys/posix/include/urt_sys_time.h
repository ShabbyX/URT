/*
 * Copyright (C) 2013-2014  Shahbaz Youssefi <ShabbyX@gmail.com>
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

#include <time.h>
#include <unistd.h>
#ifdef __MACH__
# include <mach/mach_time.h>
# include <mach/mach_init.h>
# include <mach/thread_act.h>
# include <mach/mach_port.h>
#endif
#include <urt_stdtypes.h>
#include <urt_compiler.h>
#include <urt_debug.h>

URT_DECL_BEGIN

typedef long long urt_time;
extern urt_time urt_time_offset;
#ifdef __MACH__
extern mach_timebase_info_data_t urt_time_mach_timebase;
#endif

static inline urt_time urt_get_time(void)
{
#ifdef __MACH__
	return mach_absolute_time() * urt_time_mach_timebase.numer / urt_time_mach_timebase.denom;
#else
	struct timespec t;
# ifdef CLOCK_MONOTONIC_RAW
	clock_gettime(CLOCK_MONOTONIC_RAW, &t);
# else
	clock_gettime(CLOCK_MONOTONIC, &t);
# endif
	return t.tv_sec * 1000000000ll + t.tv_nsec + urt_time_offset;
#endif
}

static inline void urt_sleep(urt_time t)
{
	urt_time ts, tu;
	if (t <= 0)
		return;

	/* Note: on some systems, usleep cannot be called for values greater than 1,000,000 */
	t = (t + 999) / 1000;	/* round up to micro seconds */
	ts = t / 1000000;
	tu = t % 1000000;

	/* Note: nanosleep is not used since it can be interrupted, which unnecessarily complicates things! */
	sleep(ts);
	usleep(tu);
}

static inline urt_time urt_get_exec_time(void)
{
#ifdef __MACH__
	thread_port_t thread = mach_thread_self();
	thread_basic_info_data_t info;
	mach_msg_type_number_t count = THREAD_BASIC_INFO_COUNT;

	if (thread_info(thread, THREAD_BASIC_INFO, (thread_info_t)&info, &count) != 0)
		return urt_get_time();

	mach_port_deallocate(mach_task_self(), thread);

	return (info.user_time.seconds + info.system_time.seconds) * 1000000000llu
		+ (info.user_time.microseconds + info.system_time.microseconds) * 1000llu;
#else
# ifdef CLOCK_THREAD_CPUTIME_ID
	struct timespec t;
# endif

	URT_CHECK_RT_CONTEXT();

# ifdef CLOCK_THREAD_CPUTIME_ID
	/* Note: task migration may result in bad exec-time calculation by glibc */
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t);
	return t.tv_sec * 1000000000ll + t.tv_nsec;
# else
	return urt_get_time();
# endif
#endif
}

URT_DECL_END

#endif
