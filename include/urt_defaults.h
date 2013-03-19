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

#ifndef URT_DEFAULTS_H
#define URT_DEFAULTS_H

/*
 * This file is a set of macros that sets default values to certain functions,
 * such as new/attach functions that take an optional `int *error` parameter,
 * or lock/wait functions that take an optional `bool *stop` parameter.
 *
 * These functions are originally defined with variadic arguments.
 */

/*
 * Note: ##__VA_ARGS__ is a gcc feature.  If this feature is not supported by
 * any of the other supported compilers, I should find a more standard way of
 * doing this.
 */

/* Thread module */

/* Memory module */
#define urt_mem_new(...) urt_mem_new(__VA_ARGS__, NULL)
#define urt_shmem_new(...) urt_shmem_new(__VA_ARGS__, NULL)
#define urt_shmem_attach(...) urt_shmem_attach(__VA_ARGS__, NULL)

/* Lock module */
#define urt_sem_new(...) urt_sem_new(__VA_ARGS__, NULL)
#define urt_shsem_new(...) urt_shsem_new(__VA_ARGS__, NULL)
#define urt_shsem_attach(...) urt_shsem_attach(__VA_ARGS__, NULL)
#define urt_sem_wait(...) urt_sem_wait(__VA_ARGS__, NULL)
/* a helper macro is used for `, ##__VA_ARGS__` to correctly expand */
#define urt_rwlock_new(...) urt_rwlock_new_(0, ##__VA_ARGS__, NULL)
#define urt_rwlock_new_(n, ...) urt_rwlock_new(__VA_ARGS__)
#define urt_shrwlock_new(...) urt_shrwlock_new(__VA_ARGS__, NULL)
#define urt_shrwlock_attach(...) urt_shrwlock_attach(__VA_ARGS__, NULL)
#define urt_rwlock_read_lock(...) urt_rwlock_read_lock(__VA_ARGS__, NULL)
#define urt_rwlock_write_lock(...) urt_rwlock_write_lock(__VA_ARGS__, NULL)

#endif
