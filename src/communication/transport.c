
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

    //ciclo NUMCLIENT volte e invio il mio "connect" message 
    // const char conn[] = "connect";
    // for (int i = 0; i < CLIENTNUM; i++)
    // {
    //     servaddr.sin_addr.s_addr = inet_addr(cfg_udp->addr[i]); 
    //     // sendto(sockfd, (const char *)"connect", strlen("connect"), 
    //     sendto(cfg_udp->sockfd, conn, strlen(conn), 0, (const struct sockaddr *) &servaddr, sizeof(servaddr)); 
    // }
    
    // //ciclo NUMCLIENT volte e ricevo i "connect" message
    // char recv_conn[sizeof(conn)+1];
    // int len;
    // for (int i = 0; i < CLIENTNUM; i++)
    // {
    //     int num_bytes = recvfrom(cfg_udp->sockfd, recv_conn, sizeof(conn), MSG_WAITALL, ( struct sockaddr *) &servaddr, &len); 
    //     printf("Client %s connected\n",inet_ntoa(servaddr.sin_addr)); 

    // }
    //     //eventualmente controllo se li ho ricevuti da tutti i client
    //         // servaddr.sin_addr.s_addr = inet_addr(addr[i]); 


    // printf("TRSPRT_PRINT: client connection ended \n");                 //DELME
    

    /* put all the things necessary to communicate in out parameter */
    *ent_num = CLIENTNUM;
    printf("TRSPRT_PRINT: clientnum copied \n");                 //DELME

    for (int i = 0; i < CLIENTNUM; i++)
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
        uint8_t msgsize
        )
{ 
    udp_info_t *cfg_udp = (udp_info_t *)args;  

    // printf("TRSPRT_PRINT: Enter phy data receive\n");                 //DELME

    bzero(buff, msgsize); 

    struct sockaddr_in cliaddr;
    int num_bytes, len = 0;

    // printf("TRSPRT_PRINT: sockfd: %d\n",(int)cfg_udp->sockfd[1]);                 //DELME
    // fflush(stdout);

    num_bytes = recvfrom(cfg_udp->sockfd[1], buff, msgsize, MSG_DONTWAIT, ( struct sockaddr *) &cliaddr, &len); 
    if(num_bytes<0);// printf("TRSPRT_PRINT: %s\n",strerror(errno));                 //DELME
    if (num_bytes == msgsize)
    {
        char ind[15];
        strcpy(ind,	inet_ntoa(cliaddr.sin_addr));
        printf("TRSPRT_PRINT: Receiving from: %s\n",ind);                 //DELME
        for (int i = 0; i < CLIENTNUM; i++)
        {
            if(strcmp(ind,cfg_udp->addr[i])==0)
            {
                *cli_id = i;
                break;
            }
        }
    }
    
    
    // for(int i=0; i<msgsize; i++){
    // printf("%x" ,buff[i]);
    // }
    // // printf(" from socket\n");
    // fflush(stdout);
    
    return num_bytes;
}

int udp_data_send(
        void * args,
        uint32_t cli_id,
        uint8_t *buff,
        uint8_t msgsize
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
    // get the message from server and send to the client 
    // printf("\npacket_send write: ");
    // for(int i=0; i<MSGSIZE; i++){
    //     printf("%x", data[i]);
    // }
    // err = write(udp_sock->clisock[cli_id], buff, msgsize); 
    err = sendto(cfg_udp->sockfd[0], (const char *)buff, msgsize, 0, (const struct sockaddr *) &cliaddr, sizeof(cliaddr)); 
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
