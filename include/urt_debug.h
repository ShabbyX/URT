/*
 * Copyright (C) 2015  Shahbaz Youssefi <ShabbyX@gmail.com>
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

#ifndef URT_DEBUG_H
#define URT_DEBUG_H

#include "urt_utils.h"
#include "urt_log.h"

#ifndef NDEBUG
# define URT_CHECK_RT_CONTEXT()							\
do {										\
	if (!urt_is_rt_context())						\
		urt_err("'%s' called from non-real-time context\n", __func__);	\
} while (0)
# define URT_CHECK_NONRT_CONTEXT()						\
do {										\
	if (!urt_is_nonrt_context())						\
		urt_err("'%s' called from real-time context\n", __func__);	\
} while (0)
#else
# define URT_CHECK_RT_CONTEXT() ((void)0)
# define URT_CHECK_NONRT_CONTEXT() ((void)0)
#endif

#endif
