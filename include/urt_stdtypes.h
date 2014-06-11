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

#ifndef URT_STDTYPES_H
#define URT_STDTYPES_H

#include "urt_config.h"

/* bring in fixed size integers, such as [u]int*_t */
#ifdef __KERNEL__
# include <linux/kernel.h>
#else
# include <stdint.h>
#endif

/* bring in bool */
#ifdef __KERNEL__
#else
# include <stdbool.h>
#endif

/* bring in sig_atomic_t */
#ifdef __KERNEL__
/*
 * Note: the kernel doesn't define sig_atomic_t.  Glibc on x86 seems to think int is fine.
 * I don't currently know about other architectures
 */
typedef int sig_atomic_t;
#else
# include <signal.h>
#endif

#endif
