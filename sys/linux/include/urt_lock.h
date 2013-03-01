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

#ifndef URT_LOCK_H
#define URT_LOCK_H

#include <semaphore.h>
#include <urt_stdtypes.h>
#include <urt_compiler.h>
#include "urt_time.h"

URT_DECL_BEGIN

typedef sem_t urt_sem;

/* unnamed semaphore */
#define urt_sem_new(...) urt_sem_new(__VA_ARGS__, (int *)NULL)
URT_ATTR_WARN_UNUSED urt_sem *(urt_sem_new)(unsigned int init_value, int *error, ...);
void urt_sem_delete(urt_sem *sem);

/* shared semaphore */
#define urt_shsem_new(...) urt_shsem_new(__VA_ARGS__, (int *)NULL)
URT_ATTR_WARN_UNUSED urt_sem *(urt_shsem_new)(const char *name, unsigned int init_value, int *error, ...);
void urt_shsem_delete(urt_sem *sem);
#define urt_shsem_attach(...) urt_shsem_attach(__VA_ARGS__, (int *)NULL)
URT_ATTR_WARN_UNUSED urt_sem *(urt_shsem_attach)(const char *name, int *error, ...);
void urt_shsem_detach(urt_sem *sem);

/* common semaphore operations */
#define urt_sem_wait(...) urt_sem_wait(__VA_ARGS__, (bool *)NULL)
int (urt_sem_wait)(urt_sem *sem, bool *stop, ...);
int urt_sem_timed_wait(urt_sem *sem, urt_time max_wait);
int urt_sem_try_wait(urt_sem *sem);
void urt_sem_post(urt_sem *sem);

URT_DECL_END

#endif
