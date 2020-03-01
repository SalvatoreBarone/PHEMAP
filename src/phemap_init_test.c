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
            printf("Error during Init\n");
        }

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

        while(1);

        PHEMAP_Device_Deinit(&dev_inst);
        //----------------------DEVICE MAIN END----------------------//
    }
    
    fflush(stdout);
    
    while(1);

    return 0;
}


