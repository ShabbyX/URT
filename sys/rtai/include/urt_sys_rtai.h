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

#ifndef URT_SYS_RTAI_H
#define URT_SYS_RTAI_H

/*
 * Note: C99 forbids usage of `static` functions in non-static `inline` functions.
 * If RTAI is configured with eager-inlining, this is what would happen with inline
 * URT functions.  This file overrides the configuration of RTAI made by user to
 * avoid this situation.
 *
 * This override is done by defining CONFIG_RTAI_LXRT_INLINE before including RTAI
 * headers.  A value of 1 corresponds to the functions being defined as `static inline`,
 * a value of 2 corresponds to `extern inline` and a value of 3 corresponds to no
 * inline.
 */

#ifndef CONFIG_RTAI_LXRT_INLINE
# define CONFIG_RTAI_LXRT_INLINE 3
#endif
#include <rtai_lxrt.h>

#endif
