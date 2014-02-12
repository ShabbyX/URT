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

#ifndef URT_CONSTS_H
#define URT_CONSTS_H

#define URT_NAME_LEN 6

#ifndef __KERNEL__
# include <errno.h>
#else
# include <linux/errno.h>
/* note: linux defines ENOTSUPP instead of ENOTSUP */
# ifndef ENOTSUP
#  define ENOTSUP ENOTSUPP
# endif
#endif

#ifdef __KERNEL__
# ifndef EXIT_FAILURE
#  define EXIT_FAILURE -EINVAL	/* something wrong, but I wouldn't know.  Isn't there a generic EFAIL? */
# endif
# ifndef EXIT_SUCCESS
#  define EXIT_SUCCESS 0
# endif
#endif

#endif
