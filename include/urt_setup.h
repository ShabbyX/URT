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

#ifndef URT_SETUP_H
#define URT_SETUP_H

#include "urt_compiler.h"
#include "urt_consts.h"

URT_DECL_BEGIN

int URT_ATTR_WARN_UNUSED urt_init(void);
void urt_exit(void);
void urt_recover(void);

/*
 * helper functions for tools
 *
 * print_names		These functions output information regarding internal states of URT.
 *			`urt_init` and `urt_exit` need to be called by the tool itself.
 * force_clear_name	Make the name available, cleaning up as much as possible.
 */
void urt_print_names(void);
void urt_force_clear_name(const char *name);

URT_DECL_END

#endif
