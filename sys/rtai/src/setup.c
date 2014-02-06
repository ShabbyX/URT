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

#include <rtai_lxrt.h>
#include <rtai_shm.h>
#include <rtai_registry.h>
#include <urt_sys_internal.h>
#include <urt_sys_lock.h>
#include <urt_mem.h>
#include <urt_log.h>
#include <urt_sys_setup.h>

int urt_sys_init(void)
{
#ifndef __KERNEL__
	rt_allow_nonroot_hrt();
#endif
	if (!rt_is_hard_timer_running())
	{
		rt_set_oneshot_mode();
		start_rt_timer(nano2count(100000));
	}
#ifndef __KERNEL__
	mlockall(MCL_CURRENT | MCL_FUTURE);
#endif
	return 0;
}

void urt_sys_force_clear_name(urt_registered_object *ro)
{
#ifndef __KERNEL__
	void *adr;
	rt_allow_nonroot_hrt();
	switch (ro->type)
	{
	case URT_TYPE_MEM:
		while ((adr = rt_get_adr(nam2num(ro->name))))
			rt_shm_free(nam2num(ro->name));
		break;
	case URT_TYPE_SEM:
	case URT_TYPE_MUTEX:
		while ((adr = rt_get_adr(nam2num(ro->name))))
			rt_named_sem_delete(adr);
		break;
	case URT_TYPE_RWLOCK:
		while ((adr = rt_get_adr(nam2num(ro->name))))
			rt_named_rwl_delete(adr);
		break;
	default:
		urt_err("internal error: bad object type %d\n", ro->type);
	}
#else
	while (rt_get_adr(nam2num(ro->name)))
		rt_drg_on_name(nam2num(ro->name));
#endif
}
