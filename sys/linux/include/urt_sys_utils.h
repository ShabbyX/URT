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

#ifndef URT_SYS_UTILS_H
#define URT_SYS_UTILS_H

#include <pthread.h>

URT_DECL_BEGIN

static inline bool urt_is_rt_context(void) { return true; }
static inline int urt_make_rt_context(int *state) { return URT_SUCCESS; }
static inline void urt_unmake_rt_context(int state) {}

URT_DECL_END

#endif
