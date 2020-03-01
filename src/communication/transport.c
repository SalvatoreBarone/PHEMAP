/**
 * @file transport.c
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
#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "transport.h"
#include "config.h"

uint8_t exbuff[4+MSG_SIZE];

/*
    è necessario che la init ritorni gli identificativi dei client con cui può comunicare
*/
int udp_server_init(
        void * args,
        uint32_t * ent_ids,
        int *ent_num)
{ 
    printf("TRSPRT_PRINT: Enter phy init\n");                 //DELME

    udp_info_t *cfg_udp = (udp_info_t *)args;
    // char **sddr = (char **)args;

    // int sockfd;
    struct sockaddr_in servaddr;
    int res;

	// socket create and verification 
	// cfg_udp->sockfd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0); 
	// cfg_udp->sockfd = socket(AF_INET, SOCK_DGRAM, 0); 
	// if (cfg_udp->sockfd < 0) return -1;

    // printf("TRSPRT_PRINT_init: sockfd: %d\n",(int)cfg_udp->sockfd);                 //DELME

    // int flags = fcntl(cfg_udp->sockfd, F_GETFL, 0);
    // if (flags == -1) return -1;
    // flags = flags | O_NONBLOCK;
    // res = fcntl(cfg_udp->sockfd, F_SETFL, flags);
    // if(res != 0) return -1;

    // printf("TRSPRT_PRINT: socket opened \n");                 //DELME

	// // clean the mem area 
	// bzero(&servaddr, sizeof(servaddr)); 

	// // assign IP, PORT 
	// servaddr.sin_family = AF_INET; 
	// servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	// servaddr.sin_port = htons(PORT); 
	
    // // Binding newly created socket to given IP and verification 
	// if ((bind(cfg_udp->sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) < 0) return -1;
    cfg_udp->sockfd[0] = socket(AF_INET, SOCK_DGRAM, 0); 
    cfg_udp->sockfd[1] = socket(AF_INET, SOCK_DGRAM, 0); 
	if (cfg_udp->sockfd[1] < 0) return -1;
	if (cfg_udp->sockfd[0] < 0) return -1;

    printf("TRSPRT_PRINT_init: sockfd0: %d\n",(int)cfg_udp->sockfd[0]);                 //DELME
    printf("TRSPRT_PRINT_init: sockfd1: %d\n",(int)cfg_udp->sockfd[1]);                 //DELME

    int flags = fcntl(cfg_udp->sockfd[0], F_GETFL, 0);
    if (flags == -1) return -1;
    flags = flags | O_NONBLOCK;
    res = fcntl(cfg_udp->sockfd[0], F_SETFL, flags);
    if(res != 0) return -1;
    
    flags = fcntl(cfg_udp->sockfd[1], F_GETFL, 0);
    if (flags == -1) return -1;
    flags = flags | O_NONBLOCK;
    res = fcntl(cfg_udp->sockfd[1], F_SETFL, flags);
    if(res != 0) return -1;

    printf("TRSPRT_PRINT: socket opened \n");                 //DELME

	// clean the mem area 
	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(PORT); 
	
    // Binding newly created socket to given IP and verification 
	if ((bind(cfg_udp->sockfd[0], (const struct sockaddr *)&servaddr, sizeof(servaddr))) < 0) return -1;

	servaddr.sin_port = htons(PORT+1); 
	if ((bind(cfg_udp->sockfd[1], (const struct sockaddr *)&servaddr, sizeof(servaddr))) < 0) return -1;

    printf("TRSPRT_PRINT: socket binded \n");                 //DELME

    /* get own ip address */
    char hostbuffer[256]; 
    char *IPbuffer; 
    struct hostent *host_entry; 
    int hostname;

    hostname = gethostname(hostbuffer, sizeof(hostbuffer)); 
    if (hostname == -1) return -1;

    host_entry = gethostbyname(hostbuffer); 
    if (host_entry == NULL) return -1;

    exbuff[0] = (((struct in_addr*)host_entry->h_addr_list[0])->s_addr) >> 24;
    exbuff[1] = (((struct in_addr*)host_entry->h_addr_list[0])->s_addr) >> 16;
    exbuff[2] = (((struct in_addr*)host_entry->h_addr_list[0])->s_addr) >> 8;
    exbuff[3] = (((struct in_addr*)host_entry->h_addr_list[0])->s_addr);
    
    /* put all the things necessary to communicate in out parameter */
    for (int i = 0; i < DEVICE_NUM; i++)
    {
        ent_ids[i] = i;

        printf("TRSPRT_PRINT: client id %d copied \n",i);                 //DELME
    }

    printf("TRSPRT_PRINT: Exit phy init\n");                 //DELME

    return 0;
}

// void* acc_func(void* args){
//     udp_info_t *udp_sock = (udp_info_t *)args;  /* check this casting */
// 	struct sockaddr_in cli; 
//     int len = sizeof(cli); 
//     int clinum = 0;
    
//     printf("TRSPRT_PRINT: accept thread started \n");                 //DELME

//     do{
//         // Accept the data packet from client and verification 
//         udp_sock->servsock[clinum] = accept(udp_sock->accsock, (SA*)&cli, &len); 
//         if (udp_sock->servsock < 0) { 
//             clinum--;
//         } 
//         clinum++;

//     }while(clinum<5);
    
//     printf("TRSPRT_PRINT: accept thread ended \n");                 //DELME
    
//     while(1);
//     //thread return
// }

int udp_data_recv(
        void * args,
        uint32_t *cli_id,
        uint8_t *buff,
        uint32_t msg_size
        )
{ 
    udp_info_t *cfg_udp = (udp_info_t *)args;  

    // printf("TRSPRT_PRINT: Enter phy data receive\n");                 //DELME

    bzero(buff, msg_size);   //check if needed

    struct sockaddr_in cliaddr;
    bzero(&cliaddr, sizeof(cliaddr)); 

    int num_bytes, len = 0;

    // printf("TRSPRT_PRINT: sockfd: %d\n",(int)cfg_udp->sockfd[1]);                 //DELME
    // fflush(stdout);
    uint8_t exbuff[4+MSG_SIZE];
    num_bytes = recvfrom(cfg_udp->sockfd[1], exbuff, msg_size + 4, MSG_DONTWAIT, ( struct sockaddr *) &cliaddr, &len); 
    if(num_bytes<0) return num_bytes;// printf("TRSPRT_PRINT: %s\n",strerror(errno));                 //DELME
    // if (num_bytes == msg_size+4)
    // {
        uint32_t src_addr = exbuff[0] << 24 | exbuff[1] << 16 | exbuff[2] << 8 | exbuff[3];
        char ind[15];
        strcpy(ind,	inet_ntoa(*(struct in_addr*)&src_addr));
        printf("TRSPRT_PRINT: Receiving from: %s %d bytes\n",ind, num_bytes);                 //DELME
        for (int i = 0; i < DEVICE_NUM; i++)
        {
            if(strcmp(ind,cfg_udp->addr[i])==0)
            {
                *cli_id = i;
                memcpy(buff, &exbuff[4],MSG_SIZE);
                return num_bytes - 4;
            }
        }
    // }
    
    
    // for(int i=0; i<msg_size; i++){
    // printf("%x" ,buff[i]);
    // }
    // // printf(" from socket\n");
    // fflush(stdout);
    
    return -1;
}

int udp_data_send(
        void * args,
        uint32_t cli_id,
        uint8_t *buff,
        uint32_t msg_size
        )
{
    udp_info_t *cfg_udp = (udp_info_t *)args;  
    
    // printf("TRSPRT_PRINT: Enter phy data send\n");                 //DELME

    struct sockaddr_in cliaddr;
    cliaddr.sin_family = AF_INET; 
    cliaddr.sin_addr.s_addr = inet_addr(cfg_udp->addr[cli_id]); 
	cliaddr.sin_port = htons(PORT+1); 
    
    // printf("TRSPRT_PRINT: Sending to: %s\n",cfg_udp->addr[cli_id]);                 //DELME

    int err = 0;
    
    memcpy(	&exbuff[4], buff, MSG_SIZE);//forse è meglio msg_size
    
    // for (int i = 0; i < 4+MSG_SIZE; i++)
    // {
    //     printf("exbuff[%d]: %x\n", i, exbuff[i]); 
    // }
    
    // err = write(udp_sock->clisock[cli_id], buff, msg_size); 
    err = sendto(cfg_udp->sockfd[0], (const char *)exbuff, msg_size + 4, 0, (const struct sockaddr *) &cliaddr, sizeof(cliaddr)); 
    if(err<0) ;//printf("TRSPRT_PRINT: %s\n",strerror(errno));                 //DELME

    // printf("TRSPRT_PRINT: Exit phy data send\n");                 //DELME
    // fflush(stdout);

    return 0;
}

int udp_server_deinit(
    void * args
)
{ 
    udp_info_t *cfg_udp = (udp_info_t *)args;  
    int res = 0;

    printf("TRSPRT_PRINT: Enter phy deinit\n");                 //DELME

    /* close all open connections */
     /* close all open connections */
    // res = close(cfg_udp->sockfd);
    // if(res<0){
    //     return -1; //oppure ritornare errno
    // }
    res = close(cfg_udp->sockfd[0]);
    if(res<0){
        return -1; //oppure ritornare errno
    }
     /* close all open connections */
    res = close(cfg_udp->sockfd[1]);
    if(res<0){
        return -1; //oppure ritornare errno
    }
    printf("TRSPRT_PRINT: socket close\n");                 //DELME

    /* freeing the memory  */
    // free(udp_sock);              //non necessario perché è una variabile statica
    printf("TRSPRT_PRINT: Exit phy deinit\n");                 //DELME

    return 0;
}
