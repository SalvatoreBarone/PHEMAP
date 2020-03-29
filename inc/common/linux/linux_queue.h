/**
 * @file linux_queue.h
 *
 * @author Alfonso Fezza <alfonsofezza93@gmail.com>
 *
 * @todo Think about the possibility to implement an error code and using errno lib.
 * Copyright 2019 Alfonso Fezza <alfonsofezza93@gmail.com>
 *
 * This file is part of libPHEMAP.
 *
 * libPHEMAP is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 3 of the License, or any later version.
 *
 * libPHEMAP is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this project; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef LINUX_QUEUE_HEADER
#define LINUX_QUEUE_HEADER

#include <mqueue.h>
#include <inttypes.h>

mqd_t linux_queue_create(
		char * const queue_name,
		const uint32_t msg_max_num,
		const uint32_t msg_len);

int linux_queue_delete(
		mqd_t queue,
		char * const queue_name);

#endif
