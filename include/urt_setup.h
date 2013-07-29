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
#include <urt_sys_setup.h>

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

/*
 * helper macros for abstracting from kernel space and user space execution model.
 *
 * The URT_GLUE macro takes five parameters:
 * - init: A small function to be called on program start.  In kernel space, this is
 *   the __init function.
 * - body: A non-real-time thread (which is the main thread in user space) that handles
 *   allocation of resources and various other operations.  The purpose of this function
 *   is to allow the init function to finish, which in kernel space signals the end of insmod.
 *   This way, rmmod can be called asynchronous to the execution of body.
 *   After the execution of body, the program waits for interrupt (user space) or
 *   rmmod (kernel space) before calling the exit function.
 * - exit: A function to be called on program exit.  In kernel space, this is
 *   the __exit funcion.
 * - interrupted: the name of a variable (defined by this macro) that would become non-zero
 *   when the program is interrupted (user space) or rmmod'ed (kernel space).
 * - done: the name of a variable (defined by this macro) that should be set by the user
 *   (in body for example) which indicates that it is safe to call the exit function.
 *
 * URT_GLUE_NO_INTERRUPT is similar to URT_GLUE, except that it does not install an interrupt
 * handler.  This is useful if the user wants to install her own interrupt handler.
 */
/* #define URT_GLUE(init, body, exit, interrupted, done) */
/* #define URT_GLUE_NO_INTERRUPT(init, body, exit) */

/*
 * TODO: add URT_DEFINE_ARG(type, name) so in kernel it would define a parameter and in user space it
 * would parse --name value from argv.  Not all types need to be supported, the ones supported by
 * kernel are enough
 */

URT_DECL_END

#endif
