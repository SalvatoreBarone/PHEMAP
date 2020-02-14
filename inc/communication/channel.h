/**
 * @file channel.h
 *
 * @author Alfonso Fezza <alfonsofezza93@gmail.com>
 *
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
#ifndef CHANNEL_H
#define CHANNEL_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mqueue.h>
#include <pthread.h>

#include "config.h"

// #define SEND_QUEUE  "/send_queue"
// #define RECV_QUEUE  "/recv_queue"
#define QUEUE_NAME_DIM  11

#define INIT_F        0
#define DEINIT_F      1
#define DATA_SEND_F   2
#define DATA_RECV_F   3

/* Structure for generic message. */
typedef struct
{
    uint32_t cli_id;                    //id of destination
    int size;                           //length of the buffer
	uint8_t buff[MSG_SIZE];             //pointer of the buffer
}
ch_msg_t;

/* Structure for sending and receiving queue. */
typedef struct
{
	mqd_t send_q;
    mqd_t recv_q[DEVICE_NUM];
    int msg_max_num;         
    int msg_len;
    char s_q_name[QUEUE_NAME_DIM];     
    char r_q_name[DEVICE_NUM][QUEUE_NAME_DIM];
}
ch_que_t;

/* Structure for low level transport information. */
typedef struct
{
    uint32_t phy_ids[DEVICE_NUM];        //pointer to the array of entity that can communicate
    // int phy_cli_num;           //number of clients 
    void * cose;               //pointer to any kind of user data
    pthread_t recv_thid;       //
    pthread_t send_thid;       //
}
ch_phy_cfg;

/* Structure for low level transport function pointer. */
typedef struct
{
	int (*phy_init)(void *, uint32_t *);
	int (*phy_deinit)(void *);
	int (*phy_data_send)(void *, uint32_t, uint8_t *, int);
	int (*phy_data_recv)(void *, uint32_t *, uint8_t *, int );
}
ch_phy_fun;

/* Structure containing all necessary things */
typedef struct
{
    ch_phy_cfg cfg;
    ch_phy_fun function;
    ch_que_t queue;
}
channel_t;

int32_t ch_init(channel_t * chnl);
int32_t ch_deinit(channel_t * chnl);

int32_t ch_msg_push(channel_t * chnl, const ch_msg_t * const msg);
int32_t ch_msg_pop(channel_t * chnl, ch_msg_t * msg);

void* ch_msg_recv(void * args);
void* ch_msg_send(void * args);

int32_t ch_phy_fun_set(ch_phy_fun * functions, uint8_t op, void * fun);       //è necessario capire se passare il tipo specifico oppure la struttura generale
int32_t ch_phy_que_set(ch_que_t * que, int msg_max_num, int msg_len);

#endif 