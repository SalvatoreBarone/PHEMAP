/**
 * @file linux_timer.c
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
#include "linux_timer.h"
#include <assert.h>
#include <sys/syscall.h>
#include <unistd.h>

/**
 * @brief Create a new timer, with a given period.
 * 
 * @details
 * The timer can be used to generate an interrupt on a periodic basis, so it
 * can be used to implement a periodic thread.
 * 
 * @param [in]	timer
 * 				Pointer to a nilux_timer_t instance, representing the software 
 * 				timer descriptor to be initialized.
 * 
 * @param [in]	period_ms
 * 				Period, in milliseconds.
 * 
 */
void linux_create_timer(	
		linux_timer_t * const	timer,
		int32_t 				period_ms)
{
	struct sigevent sevent;
	struct sigevent * sevent_ptr = &sevent;
	/*
	 * NOTE: Is' safe to use the + instead of |, because SIGEV_SIGNAL and
	 * SIGEV_THREAD_ID are bitmask.
	 */
	sevent.sigev_notify = SIGEV_SIGNAL + SIGEV_THREAD_ID;
	sevent.sigev_signo = SIGALRM;
	sevent._sigev_un._tid = syscall(__NR_gettid);
		
	struct itimerspec new_value, old_value;
	struct itimerspec * new_value_ptr = &new_value, * old_value_ptr = &old_value;
	
	new_value.it_interval.tv_sec = period_ms / 1000;	
	new_value.it_value.tv_sec = period_ms / 1000;
	new_value.it_interval.tv_nsec = (period_ms % 1000) * 1000;
	new_value.it_value.tv_nsec = (period_ms % 1000) * 1000;
	
	assert(0 == timer_create(CLOCK_MONOTONIC, sevent_ptr, &timer->descriptor));
	assert(0 == timer_settime(timer->descriptor, 0, new_value_ptr, old_value_ptr));
	assert(0 == sigemptyset(&timer->signal_set));
	assert(0 == sigaddset(&timer->signal_set, SIGALRM));
	assert(0 == sigprocmask(SIG_BLOCK, &timer->signal_set, NULL));
} 

/**
 * @brief Suspend a thread until the beginning of the next period.
 * 
 * @param [in]	timer
 * 				Pointer to a nilux_timer_t instance, representing a Software 
 * 				timer descriptor.
 * 
 * @return the "overrun count" of the timer referenced by the timer_descriptor
 * parameter.
 */
int32_t linux_wait_period(const linux_timer_t * const	timer)
{
	assert(timer != NULL);
	int32_t	wake_up_signal_number = 0;
	assert(0 == sigwait (&timer->signal_set, &wake_up_signal_number));
	int32_t overrun = timer_getoverrun (timer->descriptor);
	assert(overrun >= 0);	
	return overrun;
}

