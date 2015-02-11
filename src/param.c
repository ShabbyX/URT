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

/*
 * parse arguments and assign them to variables defined by the user, similarly to
 * what is done in the kernel.  In the kernel, this is already done on module load,
 * so this file is only meaningful in user space.
 */

#ifndef __KERNEL__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <urt_param.h>
#include <urt_log.h>

static void show_help(struct urt_module_param *params, size_t params_count, const char *prog)
{
	size_t i;
	urt_out_cont("usage:          %s [OPTION[=VALUE]]\n", prog);
	for (i = 1; i < params_count; ++i)
	{
		struct urt_module_param *param = &params[i];
		urt_out_cont("parm:           %s:%s (%s%s)\n", param->name, param->desc,
				param->is_array?"array of ":"", param->type);
	}
}

static bool param_matches(const char *param_name, const char *value)
{
	while (*param_name == (*value == '-'?'_':*value) && *param_name != '\0' && *value != '\0' && *value != '=')
	{
		++param_name;
		++value;
	}
	return *param_name == *value || (*param_name == '\0' && *value == '=');
}

#define TO_X(name, type, bigtype, cnv)							\
static int to_##name(struct urt_module_param *param, const char *value, size_t index)	\
{											\
	bigtype temp;									\
	errno = 0;									\
	if (*value == '\0')								\
		return EINVAL;								\
	temp = cnv(value, NULL, 10);							\
	if (errno)									\
		return errno;								\
	if ((type)temp != temp)								\
		return ERANGE;								\
	((type *)param->var)[index] = temp;						\
	return 0;									\
}

TO_X(byte, unsigned char, unsigned long, strtoul)
TO_X(short, short, long, strtol)
TO_X(ushort, unsigned short, unsigned long, strtoul)
TO_X(int, int, long, strtol)
TO_X(uint, unsigned int, unsigned long, strtoul)
TO_X(long, long, long, strtol)
TO_X(ulong, unsigned long, unsigned long, strtoul)

static int to_charp(struct urt_module_param *param, const char *value, size_t index)
{
	size_t len;
	const char *next_comma = NULL;
	char *copy;

	if (param->is_array)
		next_comma = strchr(value, ',');

	if (next_comma == NULL)
		len = strlen(value);
	else
		len = next_comma - value;

	copy = malloc(len + 1);
	if (copy == NULL)
		return ENOMEM;
	strncpy(copy, value, len);
	copy[len] = '\0';

	((char **)param->var)[index] = copy;
	return 0;
}

static int extract_bool(const char *str, bool *var)
{
	if (*str == '\0')
		str = "1";
	else if (*str != '0' && *str != 1 && *str != 'y' && *str != 'n' && *str != 'Y' && *str != 'N')
		return EINVAL;
	*var = *str == '1' || *str == 'y' || *str == 'Y';
	return 0;
}

static int to_bool(struct urt_module_param *param, const char *value, size_t index)
{
	return extract_bool(value, &((bool *)param->var)[index]);
}

static int to_invbool(struct urt_module_param *param, const char *value, size_t index)
{
	bool var;
	int ret = extract_bool(value, &var);
	if (!ret)
		((bool *)param->var)[index] = !var;
	return ret;
}

static const char *get_next_value(const char *value, char delim)
{
	value = strchr(value, delim);
	if (value)
		++value;
	else
		value = "";
	return value;
}

static int parse_arg(struct urt_module_param *param, char *arg)
{
	const char *value;
	int err;
	unsigned int index;
	int (*convert)(struct urt_module_param *, const char *, size_t) = NULL;

	/* get a pointer to the value of the argument specified after = */
	value = get_next_value(arg, '=');

	/* if quoted, remove the quote */
	if (*value == '"')
	{
		size_t len = strlen(arg);
		if (len > 0 && arg[len - 1] == '"')
		{
			arg[len - 1] = '\0';
			++value;
		}
	}

	if (strcmp(param->type, "byte") == 0)		convert = to_byte;
	else if (strcmp(param->type, "short") == 0)	convert = to_short;
	else if (strcmp(param->type, "ushort") == 0)	convert = to_ushort;
	else if (strcmp(param->type, "int") == 0)	convert = to_int;
	else if (strcmp(param->type, "uint") == 0)	convert = to_uint;
	else if (strcmp(param->type, "long") == 0)	convert = to_long;
	else if (strcmp(param->type, "ulong") == 0)	convert = to_ulong;
	else if (strcmp(param->type, "charp") == 0)	convert = to_charp;
	else if (strcmp(param->type, "bool") == 0)	convert = to_bool;
	else if (strcmp(param->type, "_Bool") == 0)	convert = to_bool;	/* bool may be defined as _Bool instead of typedefed */
	else if (strcmp(param->type, "invbool") == 0)	convert = to_invbool;
	else
	{
		urt_err("invalid module argument type '%s' for parameter: '%s'\n", param->type, param->name);
		return EINVAL;
	}

	index = 0;
	do
	{
		/* make sure array doesn't overflow */
		if (index >= param->max)
		{
			err = E2BIG;
			urt_err("too many values for array parameter '%s'\n", param->name);
			break;
		}
		err = convert(param, value, index++);

		switch (err)
		{
		case 0:
			break;
		case EINVAL:
			urt_err("invalid value '%s' for parameter '%s'\n", value, param->name);
			break;
		case ERANGE:
			urt_err("out of range value '%s' for parameter '%s'\n", value, param->name);
			break;
		case E2BIG:
			urt_err("value '%s' too big for parameter '%s'\n", value, param->name);
			break;
		default:
			urt_err("error parsing value '%s' for parameter '%s': %s\n", value, param->name, strerror(errno));
		}

		value = get_next_value(value, ',');
	} while (!err && param->is_array && *value != '\0');

	if (!err)
	{
		param->num = index;
		if (param->nump)
			*param->nump = param->num;
	}

	return err;
}

int urt_parse_args(struct urt_module_param *params, size_t params_count, int argc, char **argv, int *err)
{
	int i;
	size_t j;
	*err = 0;
	/* first, reset `nump` of all arrays to zero, so they would be set even if argument not specified */
	for (j = 1; j < params_count; ++j)
	{
		struct urt_module_param *param = &params[j];
		if (param->is_array && param->nump)
			*param->nump = 0;
	}
	for (i = 1; i < argc; ++i)
	{
		bool matches_any = false;

		if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
		{
			show_help(params, params_count, argv[0]);
			/* make the program exit, but don't set error */
			return -1;
		}
		for (j = 1; j < params_count; ++j)
		{
			struct urt_module_param *param = &params[j];
			if (param_matches(param->name, argv[i]))
			{
				int e = parse_arg(param, argv[i]);
				if (e)
					*err = e;
				matches_any = true;
			}
		}
		if (!matches_any)
		{
			urt_out("invalid argument '%s'\n", argv[i]);
			*err = EINVAL;
		}
	}
	return *err;
}

void urt_free_args(struct urt_module_param *params, size_t params_count)
{
	size_t i;
	unsigned int j;
	for (i = 1; i < params_count; ++i)
	{
		struct urt_module_param *param = &params[i];
		if (strcmp(param->type, "charp") == 0)
			for (j = 0; j < param->num; ++j)
				free(((char **)param->var)[j]);
	}
}

#endif /* __KERNEL__ */
