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

#ifndef URT_SYS_LOCK_H
#define URT_SYS_LOCK_H

#include <semaphore.h>
#include <pthread.h>
#include <urt_stdtypes.h>
#include <urt_compiler.h>
#include <urt_defaults.h>
#include <urt_time.h>

URT_DECL_BEGIN

typedef sem_t urt_sem;
typedef urt_sem urt_mutex;
typedef pthread_rwlock_t urt_rwlock;

#define urt_mutex_new(...) urt_sem_new(1, ##__VA_ARGS__)
#define urt_mutex_delete(m) urt_sem_delete(m)

#define urt_shmutex_new(n, ...) urt_shsem_new(n, 1, ##__VA_ARGS__)
#define urt_shmutex_attach(...) urt_shsem_attach(__VA_ARGS__)
#define urt_shmutex_detach(m) urt_shmutex_detach(m)
#define urt_shmutex_delete(m) urt_shsem_delete(m)

#define urt_mutex_lock(...) urt_sem_wait(__VA_ARGS__)
#define urt_mutex_timed_lock(m, t) urt_sem_timed_wait(m, t)
#define urt_mutex_try_lock(m) urt_sem_try_lock(m)
#define urt_mutex_unlock(m) urt_sem_post(m)

void urt_shrwlock_detach(urt_rwlock *rwl);
static inline void urt_shrwlock_delete(urt_rwlock *rwl) { urt_shrwlock_detach(rwl); }

URT_DECL_END

#endif
