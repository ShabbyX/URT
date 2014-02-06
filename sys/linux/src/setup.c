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

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <urt_log.h>
#include <urt_sys_internal.h>
#include <urt_sys_setup.h>
#include "names.h"

void urt_sys_force_clear_name(urt_registered_object *ro)
{
	char n[URT_SYS_NAME_LEN];

	if (urt_convert_name(n, ro->name))
		goto exit_bad_name;

	switch (ro->type)
	{
	case URT_TYPE_MEM:
	case URT_TYPE_MUTEX:
	case URT_TYPE_RWLOCK:
		shm_unlink(n);
		break;
	case URT_TYPE_SEM:
		sem_unlink(n);
		break;
	default:
		urt_err("internal error: bad object type %d\n", ro->type);
	}

exit_bad_name:
	return;
}
