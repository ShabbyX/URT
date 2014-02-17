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

#ifndef URT_LOCK_H
#define URT_LOCK_H

#include "urt_stdtypes.h"
#include "urt_compiler.h"
#include "urt_defaults.h"
#include "urt_time.h"
#include <urt_sys_lock.h>

URT_DECL_BEGIN

/* unnamed semaphore */
URT_ATTR_WARN_UNUSED urt_sem *(urt_sem_new)(unsigned int init_value, int *error, ...);
void urt_sem_delete(urt_sem *sem);

/* shared semaphore */
URT_ATTR_WARN_UNUSED urt_sem *(urt_shsem_new)(const char *name, unsigned int init_value, int *error, ...);
URT_ATTR_WARN_UNUSED urt_sem *(urt_shsem_attach)(const char *name, int *error, ...);
void urt_shsem_detach(urt_sem *sem);
static inline void urt_shsem_delete(urt_sem *sem) { urt_shsem_detach(sem); }

/* common semaphore operations */
int (urt_sem_wait)(urt_sem *sem, bool *stop, ...);
int (urt_sem_waitf)(urt_sem *sem, bool (*stop)(void *), void *data, ...);
int urt_sem_timed_wait(urt_sem *sem, urt_time max_wait);
int urt_sem_try_wait(urt_sem *sem);
void urt_sem_post(urt_sem *sem);

/* unnamed mutex */
/* URT_ATTR_WARN_UNUSED urt_mutex *(urt_mutex_new)(int *error, ...); */
/* void urt_mutex_delete(urt_mutex *mutex); */

/* shared mutex */
URT_ATTR_WARN_UNUSED urt_mutex *(urt_shmutex_new)(const char *name, int *error, ...);
URT_ATTR_WARN_UNUSED urt_mutex *(urt_shmutex_attach)(const char *name, int *error, ...);
/* void urt_shmutex_detach(urt_mutex *mutex); */
static inline void urt_shmutex_delete(urt_mutex *mutex) { urt_shmutex_detach(mutex); }

/* common mutex operations */
int (urt_mutex_lock)(urt_mutex *mutex, bool *stop, ...);
/* int (urt_mutex_lockf)(urt_mutex *mutex, bool (*stop)(void *), void *data, ...); */
/* int urt_mutex_timed_lock(urt_mutex *mutex, urt_time max_lock); */
/* int urt_mutex_try_lock(urt_mutex *mutex); */
/* void urt_mutex_unlock(urt_mutex *mutex); */

/* unnamed rwlock */
URT_ATTR_WARN_UNUSED urt_rwlock *(urt_rwlock_new)(int *error, ...);
void urt_rwlock_delete(urt_rwlock *rwl);

/* shared rwlock */
URT_ATTR_WARN_UNUSED urt_rwlock *(urt_shrwlock_new)(const char *name, int *error, ...);
URT_ATTR_WARN_UNUSED urt_rwlock *(urt_shrwlock_attach)(const char *name, int *error, ...);
void urt_shrwlock_detach(urt_rwlock *rwl);
static inline void urt_shrwlock_delete(urt_rwlock *rwl) { urt_shrwlock_detach(rwl); }

/* common rwlock operations */
int (urt_rwlock_read_lock)(urt_rwlock *rwl, bool *stop, ...);
int (urt_rwlock_read_lockf)(urt_rwlock *rwl, bool (*stop)(void *), void *data, ...);
int (urt_rwlock_write_lock)(urt_rwlock *rwl, bool *stop, ...);
int (urt_rwlock_write_lockf)(urt_rwlock *rwl, bool (*stop)(void *), void *data, ...);
int urt_rwlock_timed_read_lock(urt_rwlock *rwl, urt_time max_wait);
int urt_rwlock_timed_write_lock(urt_rwlock *rwl, urt_time max_wait);
int urt_rwlock_try_read_lock(urt_rwlock *rwl);
int urt_rwlock_try_write_lock(urt_rwlock *rwl);
int urt_rwlock_read_unlock(urt_rwlock *rwl);
int urt_rwlock_write_unlock(urt_rwlock *rwl);

URT_DECL_END

#endif
