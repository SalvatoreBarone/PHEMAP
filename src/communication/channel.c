// /**
//  * @file channel.c
//  *
//  * @author Alfonso Fezza <alfonsofezza93@gmail.com>
//  *
//  * @todo Think about the possibility to implement an error code and using errno lib.
//  * Copyright 2019 Alfonso Fezza <alfonsofezza93@gmail.com>
//  *
//  * This file is part of libPHEMAP.
//  *
//  * libPHEMAP is free software; you can redistribute it and/or modify it under 
//  * the terms of the GNU General Public License as published by the Free Software
//  * Foundation; either version 3 of the License, or any later version.
//  *
//  * libPHEMAP is distributed in the hope that it will be useful, but WITHOUT ANY
//  * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
//  * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//  *
//  * You should have received a copy of the GNU General Public License along with
//  * this project; if not, write to the Free Software Foundation, Inc., 51
//  * Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//  */
#include <unistd.h>
#include <inttypes.h>
#include <mqueue.h>
#include <pthread.h>

#include <errno.h>
extern int errno;

#include "channel.h"
#include "typedefs.h"
#include "config.h"
#include "linux_thread.h"
#include "linux_timer.h"

/**
 * @brief Channel initializzation   
 *
 * @param [in]	chnl
 * 				Pointer to structure containing channel information to initialize. 
 *
 * @return Upon successful completion, the function shall return a correctly initialized communication channel. 
 * Otherwise, -1 shall be returned.
 */
int32_t ch_init(
        channel_t * chnl)
{
    printf("CH_PRINT: Enter channel init\n");                 //DELME
/*____ initialize the transport */
    /* check if the function are initialized */
    if(chnl->function.phy_init == NULL){
        return -1; //errore transport_func_init
    }
    if(chnl->function.phy_deinit == NULL){
        return -1; //errore transport_func_init
    }
    if(chnl->function.phy_data_send == NULL){
        return -1; //errore transport_func_init
    }
    if(chnl->function.phy_data_recv == NULL){
        return -1; //errore transport_func_init
    }

    /* call init low level transport function */
    int res;
    res = chnl->function.phy_init(chnl->cfg.cose, chnl->cfg.phy_ids); //salvo le info sui client nella configurazione
    if(res < 0){
        return -1; //errore transport_init_error
    }
    
/*____ initialize the queue */
    /* check if the queue are initialized */
    if((chnl->queue.r_q_name[DEVICE_NUM-1][0] != '/') && (chnl->queue.s_q_name[0] != '/')){    
        return -1; //errore queue_notinit_error
    }
    printf("CH_PRINT: channel queue init \n");                 //DELME

    /* setting the queue attributes */
    struct mq_attr q_attr;
    q_attr.mq_flags = 0;
    q_attr.mq_maxmsg = chnl->queue.msg_max_num;
    q_attr.mq_msgsize = chnl->queue.msg_len;
    q_attr.mq_curmsgs = 0;

    /* create the sending message queue */
    chnl->queue.send_q = mq_open(chnl->queue.s_q_name, O_CREAT | O_NONBLOCK | O_RDWR, 0644, &q_attr); //check permission //deve essere non bloccante
    if(chnl->queue.send_q == (mqd_t)-1){
        return -1; //return error code or errno //maybe need to deallocate physical channel
    }

    /* create the receiving message queue */ //TODO check if correctly opened
    char * queue_name;
    for (int i = 0; i < DEVICE_NUM; i++)
    {    
        queue_name = &chnl->queue.r_q_name[i][0];
        chnl->queue.recv_q[i] = mq_open(queue_name, O_CREAT | O_NONBLOCK | O_RDWR, 0644, &q_attr);//check permission //deve essere non bloccante
        if(chnl->queue.recv_q[i]  == (mqd_t)-1){    
            return -2; //return error code or errno //maybe need to deallocate physical channel
        }
    }
/*____ lunch the sender and receiver listener periodic thread */        
    printf("CH_PRINT: sender and receiver thread create \n");                 //DELME

    // pthread_attr_t th_attr;
    // pthread_attr_init(&th_attr);
    // res = pthread_attr_setdetachstate(&th_attr, PTHREAD_CREATE_DETACHED);
    // res = pthread_create(&chnl->cfg.send_thid, &th_attr, &ch_msg_send,(void*) chnl);
    // res = pthread_create(&chnl->cfg.recv_thid, &th_attr, &ch_msg_recv,(void*) chnl);

    linux_thread_create(&chnl->cfg.send_thid, 10, ch_msg_send, (void*) chnl);
    linux_thread_create(&chnl->cfg.recv_thid, 10, ch_msg_recv, (void*) chnl);

    printf("CH_PRINT: Exit channel init\n");                 //DELME

    return 0;
}

/**
 * @brief Load the message to send in the sending message queue. 
 *
 * @param [in]	chnl
 * 				Pointer to structure containing channel information.
 *
 * @param [in]	msg
 * 				Pointer to the memory structure holding message to be sent.
 *
 * @return Upon successful completion, the function shall return 0. Otherwise, -1 shall be returned.
 */
int32_t ch_msg_push(
        channel_t * chnl,
        const ch_msg_t * const msg) //visto che non devo toccare l'oggetto passo sullo stack solo l'indirizzo
{
    // printf("CH_PRINT: Enter channel push message\n");                 //DELME
    // printf("CH_PRINT: send on channel\n");                            //DELME

    int res = mq_send(chnl->queue.send_q, (const char *)msg, sizeof(ch_msg_t), 0);
    if(res < 0){
        printf("errore: %s\n" ,strerror(errno));
        return -1;              //errore queue_sending_error
    }
    
    // printf("CH_PRINT: Exit channel push message\n");                  //DELME

    return res;
}

/**
 * @brief Take a message from the receiving message queue.
 *
 ** @param [in]	chnl
 * 				Pointer to structure containing channel information.
 *
 * @param [in]	msg
 * 				Pointer to the memory structure which will contain message received.
 *
 * @return Upon successful completion, the function shall return 0. Otherwise, -1 shall be returned.
 */
int32_t ch_msg_pop(
        channel_t * chnl,
        ch_msg_t * msg)
{
    // printf("CH_PRINT: Enter channel pop message\n");                 //DELME

    ssize_t bytes_read; 

    bytes_read = mq_receive(chnl->queue.recv_q[msg->cli_id], (char *)msg, sizeof(ch_msg_t), 0);
    if(bytes_read != sizeof(ch_msg_t)){
        // printf("errore: %s\n" ,strerror(errno));
        return -1;          
    }
    
    return 0;
}

/**
 * @brief Receive message from the phisical channel and load it into the receiving message queue. TODO
 *
 * @param [in]	args
 * 				Pointer to structure containing channel information.
 *
 * @return Upon successful completion, the function shall return 0. Otherwise, -1 shall be returned.
 */             
void* ch_msg_recv(      //task periodico di ricezione
        void * args)
{
    channel_t *chnl = (channel_t *) args;               /* cast back from void* to channel_t type */
    ssize_t res;
    ch_msg_t msg;

    printf("CH_PRINT: receiver started \n");                 //DELME
    fflush(stdout);

    /* timer creation */
    linux_timer_t tim;
    linux_create_timer(&tim, CH_TH_WAIT_PERIOD);

    while(1){    //need to check a variable to exit while and return
        
        linux_wait_period(&tim);

        /* get data from physical communication channel */
        msg.size = chnl->function.phy_data_recv(chnl->cfg.cose, &msg.cli_id, msg.buff, sizeof(msg.buff));  
        if(msg.size == sizeof(msg.buff))
        // if(msg.size > 0)
        {
            printf("CH_PRINT: ricevuto qualcosa da %d \n", msg.cli_id);                 //DELME
            /* sending received data to applicative receiving queue*/ 
            res = mq_send(chnl->queue.recv_q[msg.cli_id], (const char *)&msg, sizeof(ch_msg_t), 0);
            if(res < 0){//necessiti di chiudere e rillocare code più lunghe
                printf("Queue full, need to increase dimensions");
                exit(-1);       //maybe assert is better
                // return -1; //errore queue_sending_error
            }
        }
    }
    //morte del thread
    return 0;           //questa funzione deve essere un void come tipo, non ritorna nulla, bisogna segnalare eventuali errori che si verificano

}

/**
 * @brief Take a message from the sending message queue and sending on the phisical channel. TODO
 *
 * @param [in]	args
 * 				Pointer to structure containing channel information.
 *
 * @return Upon successful completion, the function shall return 0. Otherwise, -1 shall be returned.
 */
void* ch_msg_send(           //task periodico di invio
        void * args)
{
    channel_t *chnl = (channel_t *) args;               /* cast back from void* to channel_t type */
    ssize_t bytes_read;
    ch_msg_t msg;

    printf("CH_PRINT: sender started \n");                 //DELME
    fflush(stdout);
    
    /* timer creation */
    linux_timer_t tim;
    linux_create_timer(&tim, CH_TH_WAIT_PERIOD);

    while (1){    //need to check a variable to exit while and return
        
        linux_wait_period(&tim);

        /* receiving sended data from applicative sending queue*/
        bytes_read = mq_receive(chnl->queue.send_q, (char *)&msg, sizeof(ch_msg_t), 0);
        if(bytes_read == sizeof(ch_msg_t))
        {
            // printf("CH_PRINT: write on channel \n");                 //DELME
    
            /* put data on physical communication channel */
            int res = chnl->function.phy_data_send(chnl->cfg.cose, msg.cli_id, msg.buff, msg.size);
        }
    }
    //morte del thread
    return 0;
}

/**
 * @brief Deallocate the channel.
 *
 * @param [in]	chnl
 * 				Pointer to structure containing channel information to deallocate.
 *
 * @return Upon successful completion, the function shall return 0. Otherwise, -1 shall be returned.
 */
int32_t ch_deinit(
        channel_t * chnl) 
{
    int res;
    printf("CH_PRINT: Enter channel deinit\n");                 //DELME

/*____ deinitialize the transport */
    /* call deinit low level transport function */
    res = chnl->function.phy_deinit(chnl->cfg.cose); 
    if(res < 0){
        return -1; //errore transport_deinit_error
    }

/*____ deinitialize the queue */
    /* cleaning operation for deallocating queue */
    res = mq_close(chnl->queue.send_q);
    if((mqd_t)-1 == res){
        return -1;
    }

    for (int i = 0; i < DEVICE_NUM; i++)
    {    
        res = mq_close(chnl->queue.recv_q[i]); 
        if((mqd_t)-1 == res){
            return -1;
        }
    }
    printf("CH_PRINT: queue closed\n");                 //DELME

    res = mq_unlink(chnl->queue.s_q_name);
    if((mqd_t)-1 == res){
        return -1;
    }

    for (int i = 0; i < DEVICE_NUM; i++)
    {
        res = mq_unlink(chnl->queue.r_q_name[i]);      
        if((mqd_t)-1 == res){
            return -1;
        }
    }
    printf("CH_PRINT: queue unlinked\n");                 //DELME

/*____ kill the sender and receiver listener periodic thread */
    //change the variable that the thread check
    //use cancel to delete thread       //TODO

/*____ delete all channel informations */
    /* clean the channel information memory area */
    bzero(chnl, sizeof(channel_t)); 

    printf("CH_PRINT: Exit channel deinit\n");                 //DELME

    return 0;
}

// /**
//  * @brief Set the low level transport function.
//  *
//  * @param [in]	functions
//  * 				Pointer to the structure containing the pointers to the functions.
//  *
//  * @param [in]	op
//  * 				Low level transport operation selected to be assigned. 
//  *
//  * @param [in]	fun
//  * 				Pointer to the function that implement the selected operation.
//  *
//  * @return Upon successful completion, the function shall return 0. Otherwise, -1 shall be returned.
//  */
int32_t ch_phy_fun_set(
        ch_phy_fun * functions,
        uint8_t op, 
        void * fun)
{

    switch (op)
    {
    case INIT_F:
        functions->phy_init = fun;
        break;
    case DEINIT_F:
        functions->phy_deinit = fun;
        break;
    case DATA_SEND_F:
        functions->phy_data_send = fun;
        break;
    case DATA_RECV_F:
        functions->phy_data_recv = fun;
        break;

    default:
        functions->phy_init = NULL;
        functions->phy_deinit = NULL;
        functions->phy_data_send = NULL;
        functions->phy_data_recv = NULL;
        return -1;
    }

    return 0;
}

// /**
//  * @brief Set the queue information.
//  *
//  * @param [in]	que
//  * 				Structure containing the pointer to the functions.
//  *
//  * @param [in]	msg_max_num
//  * 				Maximum number of message queueable.
//  *
//  * @param [in]	msg_len
//  * 				Length in byte of one message.
//  *
//  * @return Upon successful completion, the function shall return 0. 
//  */
int32_t ch_phy_que_set(
        ch_que_t * que, 
        int msg_max_num, 
        int msg_len)
{
    int str_len = QUEUE_NAME_DIM - 1;

    que->msg_max_num = msg_max_num;
    que->msg_len = msg_len;

    static int ch_num = 0;
    int k = 0;
    k = sprintf(que->s_q_name, "/q_send%d", ch_num);
    // printf("\nsend name has %d char" , k);
    for (int i = 0; i < DEVICE_NUM; i++)
    {
        k = sprintf(&que->r_q_name[i][0], "/q_recv%d_%d", ch_num, i); 
        // printf("\n%s", &que->r_q_name[i][0]);
        // printf("\nrecv name %d has %d char" ,i , k);

    }
    ch_num++;

    return 0;
}
