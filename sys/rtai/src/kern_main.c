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

#ifdef __KERNEL__

#include <linux/kernel.h>
#include <linux/module.h>
#include <urt.h>

static int __init _urt_rtai_init(void)
{
	int ret = urt_init();
	if (ret == URT_SUCCESS || ret == URT_ALREADY)
		return 0;
	urt_err("Could not initialize URT\n");
	urt_exit();
	return -ENOMEM;
}

static void __exit _urt_rtai_exit(void)
{
	urt_exit();
}

module_init(_urt_rtai_init);
module_exit(_urt_rtai_exit);

#endif
