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

#ifndef URT_MODULE_H
#define URT_MODULE_H

#include "urt_compiler.h"

URT_DECL_BEGIN

/*
 * This file unifies the way kernel modules and user space applications receive arguments.
 * In kernel space, this is already done through the `module_param` macro and friends.  This
 * file, based on the behavior of kernel/params.c taken from the Linux kernel, exposes the
 * same interface for user space applications.
 *
 * The interface is slightly augmented for helping user-space applications store the parameter
 * definitions in an array.  Usage is:
 *
 * // define the parameter variables
 * type1 param1;
 * type2 param2[N];
 * unsigned int param2_count;
 * type3 param3;
 *
 * // declare them to URT
 * URT_MODULE_PARAM_START()
 * URT_MODULE_PARAM(param1, type1, "Description of param1")
 * URT_MODULE_PARAM_ARRAY(param2, type2, &param2_count, "Description of param2")
 * URT_MODULE_PARAM(param3, type3, "Description of param3")
 * URT_MODULE_PARAM_END()
 *
 * Note that the types are those defined by the linux kernel (see documentation for supported
 * types).  Note also the lack of semicolons.
 *
 * This method also implies that all modules, even if they don't have parameters should include
 * URT_MODULE_PARAM_START and URT_MODULE_PARAM_END.
 */

#ifdef __KERNEL__

#define URT_MODULE_PARAM_START()
#define URT_MODULE_PARAM(name, type, desc)			\
	module_param(name, type, S_IRUGO);			\
	MODULE_PARM_DESC(name, desc);
#define URT_MODULE_PARAM_ARRAY(name, type, nump, desc)		\
	module_param_array(name, type, nump, S_IRUGO);		\
	MODULE_PARM_DESC(name, desc);
#define URT_MODULE_PARAM_END()

#else /* !__KERNEL__ */

struct urt_module_param
{
	const char *name;		/* name of the parameter */
	const char *type;		/* type of the parameter */
	const char *desc;		/* description of the parameter */
	size_t max;			/* maximum number of elements, if array or string */
	void *var;			/* variable to put the data */
	unsigned int num;		/* size of array if array */
	unsigned int *nump;		/* where to store size of array if array (optional) */
	char is_array;			/* whether it's an array */
};

#define URT_MODULE_PARAM_START()				\
	static struct urt_module_param urt_app_params_[] = {	\
		{0},	/* one fake in case no params */
#define URT_MODULE_PARAM_COMMON(nam, v, typ, num, dsc, ia, d)	\
		{ /* avoid designated initializer for C++ */	\
			#nam,					\
			#typ,					\
			dsc,					\
			sizeof nam / d,				\
			v,					\
			0,					\
			num,					\
			ia,					\
		},
#define URT_MODULE_PARAM(name, type, desc)			\
	URT_MODULE_PARAM_COMMON(name, &name, type, NULL, desc, false, 1)
#define URT_MODULE_PARAM_ARRAY(name, type, nump, desc)		\
	URT_MODULE_PARAM_COMMON(name, name, type, nump, desc, true, sizeof *name)
#define URT_MODULE_PARAM_END()					\
	};

int urt_parse_args(struct urt_module_param *params, size_t params_count, int argc, char **argv, int *err);
void urt_free_args(struct urt_module_param *params, size_t params_count);

#endif /* __KERNEL__ */

URT_DECL_END

#endif
