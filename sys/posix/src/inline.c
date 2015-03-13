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

#include "urt_mem.h"
#include "urt_task.h"
#include "urt_time.h"
#include "urt_utils.h"

extern inline void urt_mem_delete(void *mem);
URT_EXPORT_SYMBOL(urt_mem_delete);

extern inline bool urt_priority_is_valid(int p);
URT_EXPORT_SYMBOL(urt_priority_is_valid);
extern inline bool urt_priority_is_higher(int a, int b);
URT_EXPORT_SYMBOL(urt_priority_is_higher);
extern inline urt_time urt_task_period_time_left(urt_task *task);
URT_EXPORT_SYMBOL(urt_task_period_time_left);

extern inline urt_time urt_get_time(void);
URT_EXPORT_SYMBOL(urt_get_time);
extern inline void urt_sleep(urt_time t);
URT_EXPORT_SYMBOL(urt_sleep);
extern inline urt_time urt_get_exec_time(void);
URT_EXPORT_SYMBOL(urt_get_exec_time);

extern inline bool urt_is_rt_context(void);
URT_EXPORT_SYMBOL(urt_is_rt_context);
extern inline bool urt_is_nonrt_context(void);
URT_EXPORT_SYMBOL(urt_is_nonrt_context);
extern inline int urt_make_rt_context(int *prev);
URT_EXPORT_SYMBOL(urt_make_rt_context);
extern inline void urt_unmake_rt_context(int prev);
URT_EXPORT_SYMBOL(urt_unmake_rt_context);
