/**
 * @file linux_timer.h
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
#ifndef LINUX_TIMER_HEADER
#define LINUX_TIMER_HEADER

#include <inttypes.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>

typedef struct
{
	timer_t		descriptor;
	sigset_t	signal_set;
}
linux_timer_t;

void linux_create_timer(
		linux_timer_t * const	timer,
		int32_t 				period_ms);
								
int32_t linux_wait_period(const linux_timer_t * const	timer);

#endif
