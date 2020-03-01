#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <pthread.h>
#include <unistd.h>

#include "channel.h"
#include "transport.h"


int main(int argc, char** argv){
/*____ PHEMAP init__________________________________________________________ */    
    /* create a new channel */
    channel_t chnl;

    /* initialize the transport function of the channel */
    ch_phy_fun_set(&chnl.function,INIT_F,&udp_server_init);
    ch_phy_fun_set(&chnl.function,DEINIT_F,&udp_server_deinit);
    ch_phy_fun_set(&chnl.function,DATA_SEND_F,&udp_data_send);
    ch_phy_fun_set(&chnl.function,DATA_RECV_F,&udp_data_recv);

    /* initialize the queue of the channel */
    int msg_max_num = 10;
    int msg_len = sizeof(ch_msg_t);
    ch_phy_que_set(&chnl.queue, msg_max_num, msg_len);

    /* creating the structure for the transport level */
    udp_info_t udp_cfg;
    const char *addr[] = {"172.17.0.2",
                        "172.17.0.3",
                        "172.17.0.4",
                        "172.17.0.5",
                        "172.17.0.6"};
    udp_cfg.addr = addr;                        /*TODO check this assignement */
    chnl.cfg.cose = &udp_cfg;

    /* initialize the transport */ 
    ch_init(&chnl);

    // printf("PEER.C sock: %d\n",udp_cfg.sockfd);

// /*__________________________________________________________________________ */    

    if (argv[1][0] == 's')
    {    
        printf("Comincio a poppare\n");
        
        /* create a message */
        ch_msg_t msg;
        msg.cli_id = 2;
        msg.size = MSG_SIZE;

        uint32_t cose = 0;

        while(1){
            ch_msg_pop(&chnl,&msg);
            cose = msg.buff[3]<< 24 | msg.buff[2]<< 16 | msg.buff[1]<< 8 | msg.buff[0];
            printf("ho letto cose: %x\n", cose);
            sleep(1);
        }
    }
    else
    {
        printf("Comincio a pushare\n");
        fflush(stdout);

        /* create a message */
        ch_msg_t msg;
        msg.cli_id = 0;
        msg.size = MSG_SIZE;
        
        uint32_t cose = 0;
        srand(time(NULL));  

        while (1)
        {
            /* send a message */
            cose = rand();
            msg.buff[0] = cose;
            msg.buff[1] = cose >> 8;
            msg.buff[2] = cose >> 16;
            msg.buff[3] = cose >> 24;

            ch_msg_push(&chnl,&msg);
            printf("ho scritto: %x\n", cose);

            sleep(5);
        }
    }
    
    fflush(stdout);
    
    while(1);

    /* deinitialize the transport */ 
    ch_deinit(&chnl);

    return 0;
}

