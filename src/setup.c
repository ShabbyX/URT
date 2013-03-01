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

#include <urt_common.h>
#include "reserved.h"
#include "urt_internal.h"

urt_sem *urt_global_sem;
urt_internal *urt_global_mem;

int urt_init(void)
{
	/*
	 * TODO
	 * - Get global lock (start name with `$`)
	 * - Get global memory (start name with `$`)
	 * - Initialize global memory
	 */
	return URT_FAIL;
}

void urt_free(void)
{
	/* TODO */
}

void urt_recover(void)
{
	/* TODO: get the global lock, try_lock and then unluck it */
	/* since it's a mutex, just unlocking it would be ok too */
}
