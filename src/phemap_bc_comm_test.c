#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <pthread.h>
#include <unistd.h>

#include "typedefs.h"
#include "channel.h"
#include "transport.h"

#include "as.h"
#include "device.h"

#include "phemap_as_init.h"
#include "phemap_dev_init.h"

#include "phemap_as_verification.h"
#include "phemap_dev_verification.h"

#include "phemap_as_babel_chain.h"
#include "phemap_dev_babel_chain.h"

#include "phemap_dev_phi.h"


int main(int argc, char** argv){

    if (argv[1][0] == 'a')
    {    
        //-----------------------VERIFIER MAIN-----------------------//
        printf("VERIFIER MAIN\n");
        
		PHEMAP_AS_t as_inst;
        

        //---------------------------------------------------------------------------------
        //questo va fuori da questa funzione per essere indipendente dal lev trasporto
        /* creating the structure for the transport level */
        udp_info_t udp_cfg;
        const char *addr[] = {"172.17.0.2",
                            "172.17.0.3",
                            "172.17.0.4",
                            "172.17.0.5",
                            "172.17.0.6"};
        udp_cfg.addr = addr;                        /*TODO check this assignement */
        as_inst.chnl.cfg.cose = &udp_cfg;
	    //----------------------------------------------------------------------------------
        PHEMAP_AS_init(&as_inst, "./dbchain/");

        int res;
        /* Faccio la init PHEMAP con il dispositivo 1 */
        printf("avvio la PHEMAP Init 1\n");
        res = PHEMAP_AS_PHEMAPInit(&as_inst, 1);
        if (res<0)
        {
            printf("Error during PHEMAP Init 1\n");
        }

        /* Faccio la init PHEMAP con il dispositivo 2 */
        printf("avvio la PHEMAP Init 2\n");
        // do
        // {
        //     res = PHEMAP_AS_PHEMAPInit(&as_inst, 2);
        //     sleep(1);
        // } while (res<0);
        res = PHEMAP_AS_PHEMAPInit(&as_inst, 2);
        if (res<0)
        {
            printf("Error during PHEMAP Init 2\n");
        }
        printf("PHEMAP Init 2 completata\n");

        /* Attendo la richiesta PHEMAP BabelChain dal dispositivo 1 */
        printf("avvio la babelchainPHEMAP Setup 1\n");
        res = PHEMAP_AS_BabelChainSetup(&as_inst, 1);
        if (res<0)
        {
            printf("Error during PHEMAP BabelChain setup\n");
        }
        // do
        // {
        //     res = PHEMAP_AS_BabelChainSetup(&as_inst, 1);
        //     sleep(1);
        // } while (res<0);
        

        // PHEMAP_Link_t as_link;
        // // PHEMAP_Chain_getNextLink(as_inst.database_name,1,0,&as_link);

        // res = PHEMAP_AS_VerifiedRecv(&as_inst,1,&as_link);
        // if (res<0)
        // {
        //     printf("Error during PHEMAP Receive\n");
        // }
        // //---------------------------------------------------//
        // uint8_t * print_ptr;
        
        // print_ptr = (uint8_t*) &as_link;
        // for (int k = 0; k < sizeof(PHEMAP_Link_t); k++)
        // {
        //     printf("%02x ", print_ptr[k]);
        // }
        // printf("\n");
        // //---------------------------------------------------//
        printf("Comicio a looppare\n");
        
        while(1);

        PHEMAP_AS_Deinit(&as_inst);
        //---------------------VERIFIER MAIN END---------------------//
    }
    else
    {
        //------------------------DEVICE MAIN------------------------//
        printf("DEVICE MAIN\n");
        
        PHEMAP_Device_t dev_inst;

        //---------------------------------------------------------------------------------
        //questo va fuori da questa funzione per essere indipendente dal lev trasporto
        /* creating the structure for the transport level */
        udp_info_t udp_cfg;
        const char *addr[] = {"172.17.0.2",
                            "172.17.0.3",
                            "172.17.0.4",
                            "172.17.0.5",
                            "172.17.0.6"};
        udp_cfg.addr = addr;                        /*TODO check this assignement */
        dev_inst.chnl.cfg.cose = &udp_cfg;
        //----------------------------------------------------------------------------------
        
        /* Faccio la device init */
        if(argv[1][1] == '1')
        {
            printf("avvio la Init del device 1\n");
            PHEMAP_Device_Init(&dev_inst, 1, 0);
        }
        else
        {
            printf("avvio la Init del device 2\n");
            PHEMAP_Device_Init(&dev_inst, 2, 0);
        }

        int res;
        /* Faccio la init PHEMAP con il verifier */
        printf("avvio la PHEMAP Init del device\n");
        // do
        // {
        //     res = PHEMAP_Device_PHEMAPInit(&dev_inst); 
        //     sleep(1);
        // } while (res < 0);
        res = PHEMAP_Device_PHEMAPInit(&dev_inst); 
        if (res<0)
        {
            printf("Error during PHEMAP Init\n");
        }
        printf("PHEMAP Init completata\n");

        /* Faccio la device init */
        // do{
        PHEMAP_Link_t dev_msg;
        char * cose = (uint8_t*)&dev_msg;
        if(argv[1][1] == '1')
        {
            sleep(4);
            printf("avvio la BabelChain setup\n");
            res = PHEMAP_Device_BCconnectToDev(&dev_inst, 2, 5);
            if (res < 0)
            {
                printf("Error during device connection\n");
            }
            
            printf("Connessione effettuata\n");
            strcpy(cose,"quellacavallali");
            sleep(2);
            PHEMAP_Device_BCsend(&dev_inst, 2, &dev_msg);
        }
        else
        {
            sleep(2);
            printf("avvio l'attesa della Notify\n");
            res = PHEMAP_Device_BCNotifyRecv(&dev_inst);
            if (res < 0)
            {
                printf("Error during device connection\n");
            }
            
            printf("Connessione effettuata\n");
            res = PHEMAP_Device_BCrecv(&dev_inst, 1, &dev_msg);
            if (res < 0)
            {
                printf("Error during device receive\n");
            }
            else
            {
                printf("Ricevuto: %s\n", cose);
            }
        }
        // }while(res<0);
        // {
        //     printf("Some Error was happened \n");
        // }

        // res = PHEMAP_Device_VerifiedSend(&dev_inst, &dev_link);
        // if (res<0)
        // {
        //     printf("Error during Verify\n");
        // }

        // //---------------------------------------------------//
        // uint8_t * print_ptr;
        
        // print_ptr = (uint8_t*) &dev_link;
        // for (int k = 0; k < sizeof(PHEMAP_Link_t); k++)
        // {
        //     printf("%02x ", print_ptr[k]);
        // }
        // printf("\n");
        
        // //---------------------------------------------------//
        printf("Comicio a looppare\n");
        
        while(1);

        PHEMAP_Device_Deinit(&dev_inst);
        //----------------------DEVICE MAIN END----------------------//
    }
    
    fflush(stdout);
    
    while(1);

    return 0;
}


