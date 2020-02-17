/**
 * @file transport.h
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

#ifndef TCP_SERVER_H
#define TCP_SERVER_H


#include <inttypes.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX 80 
#define PORT 8080 


/* Structure for ucp sockets storage. */
typedef struct
{
    uint32_t sockfd[2];                   /* socket to accept connections from client */
    // uint32_t sockfd;                   /* socket to accept connections from client */
    char **addr;
}
udp_info_t;

int udp_server_init(void * args,uint32_t *, int *);
int udp_server_deinit(void * args);

int udp_data_recv(void * args, uint32_t *cli_id, uint8_t *buff, uint32_t msg_size);
int udp_data_send(void * args, uint32_t cli_id, uint8_t *buff, uint32_t msg_size);


#endif