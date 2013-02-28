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

#ifndef URT_COMPILER_H
#define URT_COMPILER_H

#ifdef __GNUC__
# define URT_PRINTF_STYLE(format_index, args_index) __attribute__ ((format (printf, format_index, args_index)))
#else
# define URT_PRINTF_STYLE(format_index, args_index)
#endif

#ifdef __cplusplus
# define URT_DECL_BEGIN extern "C" {
# define URT_DECL_END }
# define URT_CPP
#else
# define URT_DECL_BEGIN
# define URT_DECL_END
#endif

#endif
