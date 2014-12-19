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

#ifdef __KERNEL__

#include <linux/module.h>
#include <linux/semaphore.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <urt.h>
#include <urt_internal.h>
#include <urt_sys_internal.h>
#include <urt_reserved.h>

URT_MODULE_LICENSE("GPL");
URT_MODULE_AUTHOR("Shahbaz Youssefi");
URT_MODULE_DESCRIPTION("Unified Real-Time Interface");

#if URT_BI_SPACE
/* setup sysfs for user-space global sem wait/post and time calibration */
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

static ssize_t _current_time(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%lld\n", urt_get_time());
}

static struct kobj_attribute _time_attr = __ATTR(current_time, 0444, _current_time, NULL);

#endif

/* setup urt_global_sem for other kernel modules or user applications to use */
struct semaphore urt_global_sem;
URT_EXPORT_SYMBOL(urt_global_sem);

static int __init _urt_main_init(void)
{
	int ret;

	/* make urt_global_sem ready */
	sema_init(&urt_global_sem, 1);

	ret = urt_init();

	if (ret != 0 && ret != EALREADY)
	{
		urt_err("Could not initialize URT\n");
		urt_exit();
		return -ENOMEM;
	}

#if URT_BI_SPACE
	/* create sysfs file */
	if (_kobj)
		kobject_put(_kobj);
	_kobj = kobject_create_and_add("urt"URT_SUFFIX, NULL);	/* TODO: anyway to get a reference to /sys/module/urt? */
	if (!_kobj)
	{
		urt_err("error: failed to create /sys directory for URT.  User-space applications won't run\n");
		goto no_sysfs;
	}
	if (sysfs_create_file(_kobj, &_lock_attr.attr))
		urt_err("error: could not create /sys file for global sem.  User-space applications won't run\n");
	if (sysfs_create_file(_kobj, &_time_attr.attr))
		urt_err("error: could not create /sys file for current time.  User-space applications may be badly synchronized\n");
no_sysfs:
#endif

	urt_out("Loaded\n");
	return 0;
}

static void __exit _urt_main_exit(void)
{
#if URT_BI_SPACE
	if (_kobj)
		kobject_put(_kobj);
	_kobj = NULL;
#endif
	urt_exit();

	/* cleanup global memory and semaphore since they are not cleaned on urt_exit */
	if (urt_global_mem)
		urt_global_mem_free(URT_GLOBAL_MEM_NAME);

	urt_out("Unloaded\n");
}

module_init(_urt_main_init);
module_exit(_urt_main_exit);

#endif
