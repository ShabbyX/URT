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

#ifndef URT_LOG_H
#define URT_LOG_H

#include "urt_config.h"
#include <urt_sys_log.h>

#ifndef URT_LOG_PREFIX
# define URT_LOG_PREFIX "URT: "
#endif

#ifndef NDEBUG
# define URT_LOG_PREFIX_FORMAT URT_LOG_PREFIX"%s:%u: "
# define URT_LOG_PREFIX_PARAMS , __FILE__ + (sizeof __FILE__ < 25?0:sizeof __FILE__ - 25), __LINE__
# define urt_dbg(f, ...) urt_log(f, __VA_ARGS__)
# define urt_dbg_cont(f, ...) urt_log_cont(f, __VA_ARGS__)
#else
# define URT_LOG_PREFIX_FORMAT URT_LOG_PREFIX
# define URT_LOG_PREFIX_PARAMS
# define urt_dbg(f, ...) ((void)0)
# define urt_dbg_cont(f, ...) ((void)0)
#endif

#define urt_log(f, ...)									\
	do {										\
		urt_log_cont(f, URT_LOG_PREFIX_FORMAT URT_LOG_PREFIX_PARAMS);		\
		urt_log_cont(f, __VA_ARGS__);						\
	} while (0)

#ifdef __KERNEL__
# define URT_OUT_FILE NULL
# define URT_ERR_FILE NULL
#else
# define URT_OUT_FILE stderr
# define URT_ERR_FILE stdout
#endif

# define urt_out(...) urt_log(URT_OUT_FILE, __VA_ARGS__)
# define urt_err(...) urt_log(URT_ERR_FILE, __VA_ARGS__)
# define urt_out_cont(...) urt_log_cont(URT_OUT_FILE, __VA_ARGS__)
# define urt_err_cont(...) urt_err_cont(URT_ERR_FILE, __VA_ARGS__)

#endif
