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

#ifndef URT_CONSTS_H
#define URT_CONSTS_H

#define URT_NAME_LEN 6

#define URT_SUCCESS 0		/* success! */
#define URT_FAIL -1		/* fail for various reasons */
#define URT_NO_MEM -2		/* not enough memory */
#define URT_NOT_LOCKED -3	/* try lock didn't lock */
#define URT_TIMEOUT -4		/* timed locks timed out */
#define URT_BAD_NAME -5		/* invalid name */
#define URT_BAD_VALUE -6	/* value out of range */
#define URT_EXISTS -7		/* object with that name already exists */
#define URT_NO_OBJ -8		/* returned if trying to attach to a name that is not shared */
#define URT_INTR -9		/* call was interrupted due to signal */
#define URT_ALREADY -10		/* returned if operations is already done (e.g. urt_init) */
#define URT_NO_NAME -11		/* no such name (e.g. in attach functions) */

#endif
