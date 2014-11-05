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

#ifndef URT_UTILS_H
#define URT_UTILS_H

#include "urt_stdtypes.h"
#include "urt_compiler.h"
#include "urt_consts.h"
#include "urt_sys_utils.h"

URT_DECL_BEGIN

/* fills `name` with an available name and reserves it so others can't take it */
int urt_get_free_name(char *name);
/* bool urt_is_rt_context(void); */
/* int urt_make_rt_context(int *prev); */
/* void urt_unmake_rt_context(int prev); */

URT_DECL_END

#endif
