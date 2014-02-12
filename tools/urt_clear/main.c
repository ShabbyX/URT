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

#include <urt.h>

static void _print_help(const char *exe)
{
	urt_out("usage: %s names\n", exe);
}

int main(int argc, char **argv)
{
	int ret;
	int exit_status = 0;
	int i;
	int ignore_options = 0;

	if (argc < 2)
	{
		_print_help(argv[0]);
		return 0;
	}

	ret = urt_init();
	if (ret)
	{
		urt_out("unable to initialize URT: %d\n", ret);
		exit_status = EXIT_FAILURE;
		goto exit_no_init;
	}

	for (i = 1; i < argc; ++i)
	{
		if (!ignore_options)
		{
			if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
			{
				_print_help(argv[0]);
				goto exit_cleanup;
			}
			else if (strcmp(argv[i], "--") == 0)
				ignore_options = 1;
			else
				urt_force_clear_name(argv[i]);
		}
		else
			urt_force_clear_name(argv[i]);
	}

exit_cleanup:
	urt_exit();
exit_no_init:
	return exit_status;
}
