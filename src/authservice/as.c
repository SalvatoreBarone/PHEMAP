/**
 * @file as.c
 *
 * @author Salvatore Barone <salvator.barone@gmail.com>
 * @author Alfonso Fezza <alfonsofezza93@gmail.com>
 *
 * Copyright 2019 Salvatore Barone <salvator.barone@gmail.com>
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
#include <stdlib.h>
#include <string.h>

#include "as.h"

#include "channel.h"
#include "transport.h"
/**
 * @brief Authentication service setup.
 *
 * @todo redo the doc
 * @details
 * This function sets up the Authentication service for the extended PHEMAP
 * protocol.
 *
 * @param [in]	as_instance
 * 				A pointer to the  PHEMAP_AS_t instance to be initialized.
 *
 * @oparam [in]	database_name
 * 				Name of the sqlite3 database containing enrolled chains for each
 * 				device.
 *
 * @param [in[	port
 * 				The TCP port on which the AS will accept connections from 
 * 				devices.
 *
 * @return On success, the function returns 0, otherwise it returns -1.
 */
int32_t PHEMAP_AS_init(
		PHEMAP_AS_t * const as_inst,
		const char * const database_name)
{   

    /* initialize the transport function of the channel */
    ch_phy_fun_set(&as_inst->chnl.function,INIT_F,&udp_server_init);
    ch_phy_fun_set(&as_inst->chnl.function,DEINIT_F,&udp_server_deinit);
    ch_phy_fun_set(&as_inst->chnl.function,DATA_SEND_F,&udp_data_send);
    ch_phy_fun_set(&as_inst->chnl.function,DATA_RECV_F,&udp_data_recv);

    /* initialize the queue of the channel */
    int msg_max_num = 10;           //maybe change with a define
    int msg_len = sizeof(ch_msg_t);
    ch_phy_que_set(&as_inst->chnl.queue, msg_max_num, msg_len);

	// //---------------------------------------------------------------------------------
    // //questo va fuori da questa funzione per essere indipendente dal lev trasporto
	// /* creating the structure for the transport level */
    // udp_info_t udp_cfg;
    // const char *addr[] = {"172.17.0.2",
    //                     "172.17.0.3",
    //                     "172.17.0.4",
    //                     "172.17.0.5",
    //                     "172.17.0.6"};
    // udp_cfg.addr = addr;                        /*TODO check this assignement */
    // as_inst->chnl.cfg.cose = &udp_cfg;
	// //----------------------------------------------------------------------------------
    /* initialize the transport */ 
    ch_init(&as_inst->chnl);


	as_inst->database_name = malloc(strlen(database_name));		//l'istanza as deve tenere un vettore di dim fissata
	strcpy(as_inst->database_name, database_name);

	return 0;		//on success
}

int32_t PHEMAP_AS_Deinit(
		PHEMAP_AS_t * const as_inst)
{
    /* initialize the transport */ 
    ch_deinit(&as_inst->chnl);

	free(as_inst->database_name);

    /* clear all the data */
	memset(as_inst, 0, sizeof(PHEMAP_AS_t));

	return 0;		//on success
}