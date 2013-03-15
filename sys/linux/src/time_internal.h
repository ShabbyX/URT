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

#ifndef URT_TIME_INTERNAL_H
#define URT_TIME_INTERNAL_H

#include <sys/time.h>
#include <urt_time.h>

/* posix functions taking abs_timeout require time since epoch */
static inline urt_time urt_get_time_epoch(void)
{
#ifdef CLOCK_REALTIME
	struct timespec t;
	clock_gettime(CLOCK_REALTIME, &t);
	return t.tv_sec * 1000000000ll + t.tv_nsec;
#else
# warning CLOCK_REALTIME is not implemented
	struct timeval t;
	gettimeofday(&t, NULL);
	return t.tv_sec * 1000000000ll + t.tv_usec * 1000;
#endif
}

#endif
