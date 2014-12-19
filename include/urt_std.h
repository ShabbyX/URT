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

#ifndef URT_STD_H
#define URT_STD_H

/*
 * this file should bring in as much as the standard C functions that are
 * available both in user and kernel spaces.  This way, less effort is
 * required trying to compile the same code in both spaces.
 */

#ifdef __KERNEL__
# include <linux/string.h>
# include <linux/ctype.h>
# include <linux/stddef.h>
# include <linux/bsearch.h>
# include <linux/sort.h>
# define qsort(b, n, s, c) sort(b, n, s, c, NULL)
#else
# include <string.h>
# include <ctype.h>
# include <stddef.h>
#endif

#endif
