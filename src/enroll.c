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
#include "phemap_as_enroll.h"
#include "phemap_dev_enroll.h"


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
    int msg_max_num = 10;           //maybe change with a define
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


/*_____________________________ENROLLMENT_____________________________ */    

    int32_t maxChainLength = 20;

    if (argv[1][0] == 'a')
    {    
        printf("AS ENROLLMENT\n");
        
        PHEMAP_Chain_t chain;
        bzero(&chain, sizeof(chain));

        int res = PHEMAP_GenerateChain(&chnl,(uint64_t)1, maxChainLength, &chain);
        if (res<0)
        {
            printf("Error on Chain Generation\n");
        }

        res = PHEMAP_Chain_store("./dbchain/",1,0, &chain);
        if (res<0)
        {
            printf("Error on Chain Storing\n");
        }

        // PHEMAP_Chain_update("./dbchain/",2,0, &chain);
        // chain.current = 3;
        // res = PHEMAP_Chain_update("./dbchain/",1,0, &chain);
        // if (res<0)
        // {
        //     printf("Error on Chain Updating\n");
        // }

        PHEMAP_Chain_t chain_read;

        for (size_t i = 1; i < 5; i++)
        {
            bzero(&chain_read, sizeof(chain_read));

            printf("loading chain %d\n",i);
            res = PHEMAP_Chain_load("./dbchain/",i,0, &chain_read);
            if (res < 0)
            {
                printf("chain not found\n");
                continue;
            }
            

            //---------------------------------------------------//
            printf("\nread lenght: %x\n", chain_read.length);	
			printf("read current: %x\n", chain_read.current);	
            uint8_t * print_ptr;
            for (size_t j = 0; j < chain_read.length+1; j++)
            {
                print_ptr = (uint8_t*) chain_read.links[j];
                for (int k = 0; k < sizeof(PHEMAP_Link_t); k++)
                {
                    printf("%02x ", print_ptr[k]);
                }
                printf("\n");
            }
            printf("lenght: %x\n", chain.length);	
			printf("current: %x\n", chain.current);	
            for (size_t j = 0; j < chain.length+1; j++)
            {
                print_ptr = (uint8_t*) chain.links[j];
                for (int k = 0; k < sizeof(PHEMAP_Link_t); k++)
                {
                    printf("%02x ", print_ptr[k]);
                }
                printf("\n");
            }
            //---------------------------------------------------//
            res = memcmp(
                    &chain_read,
                    &chain,
                    sizeof(PHEMAP_Chain_t));
            printf("res: %d", res);
            fflush(stdout);
            if (res == 0)
            {
                printf("Stored correctly\n");
            }
            else
            {
                printf("Stored incorrectly\n");
            }
        }
    }
    else
    {
        printf("DEVICE ENROLLMENT\n");
        
        int res = PHEMAP_BuildChain(&chnl,(uint64_t)0, maxChainLength);
        printf("\nres %d", res);
        fflush(stdout);
    }
    
    fflush(stdout);
    
    while(1);

    /* deinitialize the transport */ 
    ch_deinit(&chnl);

    return 0;
}


