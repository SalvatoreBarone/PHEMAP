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
    
        int res = PHEMAP_AS_PHEMAPInit(&as_inst, 1);
        if (res<0)
        {
            printf("Error during PHEMAP Init\n");
        }

        PHEMAP_Link_t as_link;
        // PHEMAP_Chain_getNextLink(as_inst.database_name,1,0,&as_link);

        res = PHEMAP_AS_VerifiedRecv(&as_inst,1,&as_link);
        if (res<0)
        {
            printf("Error during PHEMAP Receive\n");
        }
        //---------------------------------------------------//
        uint8_t * print_ptr;
        
        print_ptr = (uint8_t*) &as_link;
        for (int k = 0; k < sizeof(PHEMAP_Link_t); k++)
        {
            printf("%02x ", print_ptr[k]);
        }
        printf("\n");
        //---------------------------------------------------//
        
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
        
        PHEMAP_Device_Init(&dev_inst, 1, 0);

        int res = PHEMAP_Device_PHEMAPInit(&dev_inst); 
        if (res<0)
        {
            printf("Error during Init\n");
        }

        PHEMAP_Link_t dev_link;
        // PHEMAP_Device_getNextLink(&dev_inst, 
        //                             &dev_inst.verifier_ent, 
        //                             RET_NO_SENTINEL, 
        //                             &dev_link);

        char * cose = (uint8_t*)&dev_link;
        cose[0] = 104;
        cose[1] = 101;
        cose[2] = 108;
        cose[3] = 108;
        cose[4] = 111;
        cose[5] = 032;
        cose[6] = 119;
        cose[7] = 111;
        cose[8] = 114;
        cose[9] = 108;
        cose[10] = 100;

        res = PHEMAP_Device_VerifiedSend(&dev_inst, &dev_link);
        if (res<0)
        {
            printf("Error during Verify\n");
        }

        // //---------------------------------------------------//
        // uint8_t * print_ptr;
        
        // print_ptr = (uint8_t*) &dev_link;
        // for (int k = 0; k < sizeof(PHEMAP_Link_t); k++)
        // {
        //     printf("%02x ", print_ptr[k]);
        // }
        // printf("\n");
        
        // //---------------------------------------------------//
        
        while(1);

        PHEMAP_Device_Deinit(&dev_inst);
        //----------------------DEVICE MAIN END----------------------//
    }
    
    fflush(stdout);
    
    while(1);

    return 0;
}


