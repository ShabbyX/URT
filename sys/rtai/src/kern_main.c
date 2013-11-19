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

#include <linux/module.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <urt.h>
#include <urt_internal.h>
#include <urt_sys_internal.h>

URT_MODULE_LICENSE("GPL");
URT_MODULE_AUTHOR("Shahbaz Youssefi");

/* setup sysfs for user-space global sem wait/post */
static struct kobject *_kobj = NULL;

static ssize_t _lock_op(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	switch (buf[0])
	{
	case URT_GLOBAL_SEM_WAIT:
		urt_global_sem_wait();
		break;
	case URT_GLOBAL_SEM_TRY_WAIT:
		urt_global_sem_try_wait();
		break;
	case URT_GLOBAL_SEM_POST:
		urt_global_sem_post();
		break;
	default:
		urt_err("error: bad global sem command %d\n", buf[0]);
	}

	return count;
}

static struct kobj_attribute _lock_attr = __ATTR(global_sem, 0222, NULL, _lock_op);

/* setup urt_global_sem for other kernel modules or user applications to use */
struct semaphore urt_global_sem;
URT_EXPORT_SYMBOL(urt_global_sem);

static int __init _urt_rtai_init(void)
{
	int ret;

	/* make urt_global_sem ready */
	sema_init(&urt_global_sem, 1);

	ret = urt_init();

	if (ret != URT_SUCCESS && ret != URT_ALREADY)
	{
		urt_err("Could not initialize URT\n");
		urt_exit();
		return -ENOMEM;
	}

	/* create sysfs file */
	if (_kobj)
		kobject_put(_kobj);
	_kobj = kobject_create_and_add("urt", NULL);	/* TODO: anyway to get a reference to /sys/module/urt? */
	if (!_kobj)
	{
		urt_err("error: failed to create /sys directory for URT.  User-space applications won't run\n");
		goto no_sysfs;
	}
	if (sysfs_create_file(_kobj, &_lock_attr.attr))
	{
		urt_err("error: could not create /sys file for global sem.  User-space applications won't run\n");
		goto no_sysfs;
	}
no_sysfs:

	urt_out("Loaded\n");
	return 0;
}

static void __exit _urt_rtai_exit(void)
{
	if (_kobj)
		kobject_put(_kobj);
	_kobj = NULL;
	urt_exit();
	urt_out("Unloaded\n");
}

module_init(_urt_rtai_init);
module_exit(_urt_rtai_exit);

#endif
