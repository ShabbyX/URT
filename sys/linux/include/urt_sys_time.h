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

#include <time.h>
#include <unistd.h>
#include <urt_stdtypes.h>
#include <urt_compiler.h>

URT_DECL_BEGIN

typedef long long urt_time;

static inline urt_time urt_get_time(void)
{
	struct timespec t;
#ifdef CLOCK_MONOTONIC_RAW
	clock_gettime(CLOCK_MONOTONIC_RAW, &t);
#else
	clock_gettime(CLOCK_MONOTONIC, &t);
#endif
	return t.tv_sec * 1000000000ll + t.tv_nsec;
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

	sleep(ts);
	usleep(tu);
}

static inline urt_time urt_get_exec_time(void)
{
#ifdef CLOCK_THREAD_CPUTIME_ID
	struct timespec t;
	/* Note: task migration may result in bad exec-time calculation by glibc */
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t);
	return t.tv_sec * 1000000000ll + t.tv_nsec;
#else
	return urt_get_time();
#endif
}

URT_DECL_END

#endif
