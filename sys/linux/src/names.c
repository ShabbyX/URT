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

#include <urt_compiler.h>
#include "names.h"

int urt_convert_name(char to[URT_NAME_LENGTH + 2], const char from[URT_NAME_LENGTH])
{
	unsigned int i;

	to[0] = '/';
	for (i = 0; i < URT_NAME_LENGTH; ++i)
		if (from[i] >= 'a' && from[i] <= 'z')
			to[i + 1] = from[i] - 'a' + 'A';
		else if (!((from[i] >= 'A' && from[i] <= 'Z')
			|| (from[i] >= '0' && from[i] <= '9')
			|| from[i] == '_' || from[i] == '$'))
			break;
		else
			to[i + 1] = from[i];
	if (URT_UNLIKELY(i == 0))
		goto exit_bad_name;
	to[i + 1] = '\0';

	return URT_SUCCESS;
exit_bad_name:
	return URT_BAD_NAME;
}
