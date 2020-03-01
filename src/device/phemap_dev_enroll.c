/**
 * @file phemap_dev_enrollment.c
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
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include "phemap_dev_enroll.h"
// #include "phemap_message.h"
// #include "device.h"
// #include "channel.h"

/**
 * @brief PUF chain generation, to enroll a device.
 * 
 * @todo redo the doc of this function using the correct parameters
 * @details
 * The function queries the device and generates a PUF chain of, at most, 
 * M links, starting from a random challenge.
 *
 * @param [in]	remote_server_ip
 * 				IP address of the Remote TCP server.
 *
 * @param [in]	remote_server_port
 * 				TCP port on which the remote server is listening.
 *
 * @param [in]	maxChainLength 
 * 				Maximum chain length
 * 
 * @param [out]	chain
 * 				Memory area in which generated chain links will be stored. This
 * 				area must be pre-allocated and its size must be able to contain
 * 				a number of links equal to maxChainLength. The link in
 * 				position 0 will be the initial random challenge @$c_0@$, 
 * 				the link in position 1 will be @$l_1 = \psi_D(c_0)@$,
 * 				the element in position 2 will be 
 * 				@$l_2 = \psi_D(l_1) = \psi_D (\psi_D(c_0))@$, and so on.
 * 
 * @return The chain length or -1, if an error occurs (for example, when the
 * response received from the device is not an "enrollment reply").
 */
int32_t PHEMAP_BuildChain(  
	channel_t * chnl,
	PHEMAP_Device_ID_t device_id,
	const int32_t maxChainLength)
{
	ch_msg_t in_msg;
	in_msg.cli_id = (uint32_t)device_id;				//mappare phy_id con PHEMAP_id in modo migliore
	in_msg.size = sizeof(PHEMAP_Message_t);

	ch_msg_t out_msg;
	out_msg.cli_id = device_id;				//mappare phy_id con PHEMAP_id in modo migliore
	out_msg.size = sizeof(PHEMAP_Message_t);
	
	int32_t i = 1;
	do
	{
		// Ricezione della richiesta
		while (ch_msg_pop(chnl, &in_msg)!=0);

		PHEMAP_Message_t *request;  
		request = (PHEMAP_Message_t*)in_msg.buff;	
		
		// Controllo che la richiesta ricevuta sia di tipo "enrollment request" 
		// Se non lo è restituisco -1
		if (request->type != enrollment_request){
			printf("not enrollment req");
			fflush(stdout);
			return -1;
		}

		//--------------------------DEBUG PRINT--------------------------//
		uint8_t * print_ptr = (uint8_t*) &request->payload.link[0];
		printf("ricevuto -> ");
		for (int k = 0; k < sizeof(PHEMAP_Link_t); k++)
		{
			printf("%02x ", print_ptr[k]);
		}
		printf("\n");
		//--------------------------DEBUG PRINT--------------------------//

		// Costruzione la risposta
		PHEMAP_Message_t *response;
		response = (PHEMAP_Message_t*)out_msg.buff;	//casto a questo tipo così da risparmiare una copia
		response->type = enrollment_reply;

		// a questo punto va generata la PUF, richiamiamo una funzione 
		// response->payload.link = PHEMAP_PUF_LINK_GEN(request->payload.link); //vedere nome
		// ----------------------------------------------------------- //
        PHEMAP_DEV_linkGen(NULL,0,&request->payload.link,&response->payload.link);
		// ----------------------------------------------------------- //
		// invio la risposta 
		while (ch_msg_push(chnl, &out_msg)!=0);
		print_ptr = (uint8_t*) &response->payload.link[0];
		printf("inviato -> ");
		for (int k = 0; k < sizeof(PHEMAP_Link_t); k++)
		{
			printf("%02x ", print_ptr[k]);
		}
		printf("\n");
		// printf("i: %d\n", i);
	}
	while (++i < maxChainLength);

	return  0;	//on success
}
