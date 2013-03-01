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

#define URT_SUCCESS 0
#define URT_FAIL -1
#define URT_NO_MEM -2
#define URT_NOT_LOCKED -3
#define URT_TIMEOUT -4
#define URT_BAD_NAME -5
#define URT_BAD_VALUE -6
#define URT_EXISTS -7
#define URT_NO_OBJ -8		/* returned if trying to attach to a name that is not shared */
#define URT_INTR -9		/* call was interrupted due to signal */

#endif
