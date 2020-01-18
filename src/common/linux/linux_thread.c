/**
 * @file linux_thread.c
 *
 * @author Salvatore Barone <salvator.barone@gmail.com>
 *
 * @copyright
 * Copyright 2019 Salvatore Barone <salvator.barone@gmail.com>
 *
 * This file is part of Sputnik_v2
 *
 * Sputnik_v2 is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free 
 * Software Foundation; either version 3 of the License, or any later version.
 *
 * Sputnik_v2 is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more 
 * details.
 *
 * You should have received a copy of the GNU General Public License along with 
 * Sputnik_v2; if not, write to the Free Software Foundation, Inc., 51 Franklin 
 * Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "linux_thread.h"
#include <assert.h>
#include <sys/resource.h> 
#include <sys/syscall.h>
#include <sys/time.h>
#include <unistd.h>

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>

extern int errno;

void linux_thread_create(
		pthread_t * 	const 	thread_id,
		const int32_t			priority,
		void * 					(*starting_routine)(void * thr_args),
		void * 			const	args)
{
	assert(thread_id != NULL);
	assert((priority > 0) && (priority <= 99));
	assert(starting_routine != NULL);
	assert(args != NULL);

	struct rlimit resources_limits =
	{
		.rlim_cur = RLIM_INFINITY,
		.rlim_max = RLIM_INFINITY
	};

	assert(0 == setrlimit(RLIMIT_RTPRIO, &resources_limits));

	struct sched_param sched_parameter;
	pthread_attr_t attributes;
	pthread_attr_t * attributes_ptr = &attributes;

	assert(0 == pthread_attr_init(attributes_ptr));
	assert(0 == pthread_attr_setdetachstate(attributes_ptr, PTHREAD_CREATE_DETACHED));
	assert(0 ==  pthread_attr_setinheritsched(attributes_ptr, PTHREAD_EXPLICIT_SCHED));
	assert(0 ==  pthread_attr_setschedpolicy(attributes_ptr, SCHED_FIFO));
	sched_parameter.sched_priority = priority;
	assert(0 == pthread_attr_setschedparam(attributes_ptr, &sched_parameter));
	assert(0 ==  pthread_create(thread_id,	attributes_ptr,	starting_routine, args));
	
}

