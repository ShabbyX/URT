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

#ifndef URT_SYS_SETUP_H
#define URT_SYS_SETUP_H

#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <urt_internal.h>

URT_DECL_BEGIN

static inline int urt_sys_init(void) { return URT_SUCCESS; }
static inline void urt_sys_exit(void) {}
void urt_sys_force_clear_name(urt_registered_object *ro);

#define URT_GLUE(init, body, exit, interrupted, done)		\
static volatile sig_atomic_t interrupted = 0;			\
static int done = 0;						\
static void urt_sig_handler_(int signum)			\
{								\
	interrupted = 1;					\
}								\
int main(int argc, char **argv)					\
{								\
	int urt_err_;						\
	/* set signal handler */				\
	struct sigaction urt_sa_;				\
	urt_sa_ = (struct sigaction){.sa_handler = NULL};	\
	urt_sa_.sa_handler = urt_sig_handler_;			\
	sigemptyset(&urt_sa_.sa_mask);				\
	sigaction(SIGINT, &urt_sa_, NULL);			\
	sigaction(SIGHUP, &urt_sa_, NULL);			\
	sigaction(SIGTERM, &urt_sa_, NULL);			\
	sigaction(SIGQUIT, &urt_sa_, NULL);			\
	sigaction(SIGUSR1, &urt_sa_, NULL);			\
	sigaction(SIGUSR2, &urt_sa_, NULL);			\
	urt_err_ = init();					\
	if (urt_err_)						\
		return urt_err_;				\
	body();							\
	/* wait for done */					\
	while (!done)						\
		usleep(10000);					\
	exit();							\
	return 0;						\
}

#define URT_GLUE_NO_INTERRUPT(init, body, exit)			\
int main(int argc, char **argv)					\
{								\
	int urt_err_ = init();					\
	if (urt_err_)						\
		return urt_err_;				\
	body();							\
	exit();							\
	return 0;						\
}

URT_DECL_END

#endif
