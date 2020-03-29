/**
 * @file linux_queue.c
 *
 * @author Alfonso Fezza <alfonsofezza93@gmail.com>
 *
 * Copyright 2019 Alfonso Fezza <alfonsofezza93@gmail.com>
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

#include "linux_queue.h"

mqd_t linux_queue_create(
		char * const queue_name,
		const uint32_t msg_max_num,
		const uint32_t msg_len)
{
	/* check if the queue are initialized */
    if(queue_name[0] != '/'){    
        return (mqd_t)(-1); 
    }
	
	/* setting the queue attributes */
    struct mq_attr q_attr;
    q_attr.mq_flags = 0;
    q_attr.mq_maxmsg = msg_max_num;
    q_attr.mq_msgsize = msg_len;
    q_attr.mq_curmsgs = 0;

	mqd_t lin_queue = mq_open(queue_name, 
								O_CREAT | O_NONBLOCK | O_RDWR, 
								0644, 
								&q_attr); 
    
	return lin_queue;
}

int linux_queue_delete(
		mqd_t queue,
		char * const queue_name
)
{
    int res = 0;
	res = mq_close(queue);
    res = mq_unlink(queue_name);
    
	return res;
}
