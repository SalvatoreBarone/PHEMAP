/**
 * @file phemap_as_verification.c
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
#include "phemap_as_verification.h"
#include "typedefs.h"
#include "phemap_message.h"
#include "memxor.h"
#include "linux_timer.h"

/**
 * @brief Checks if a received verification request is well formed. 
 *
 * @param [in]	chain
 * 				Pointer to the currently in use PHEMAP chain instance.
 *
 * @param [in]	message
 * 				Pointer to the received message.
 *
 * @param [in]	nonce
 * 				Memory area in which the nonce, extracted from the init reply
 * 				message, will be placed.
 *
 * @return Upon successful completion, 0 shall be returned; otherwise, -1 shall 
 * be returned.
 */
static int32_t PHEMAP_AS_checkVerificationRequest(
		PHEMAP_Chain_t * const chain,
		PHEMAP_Message_t * const message,
		PHEMAP_Link_t * const data)                 //questo tipo ovviamente va cambiatoTODO
{
	// validazione del messaggio
	if (verification_request != message->type)
		return -1;
    
    if (0 != memcmp(&message->payload.verify_req.l_1,
                    &chain->links[0], 
                    sizeof(PHEMAP_Link_t)))
    {
		return -1;
    }

	//valutare se fare inizializzazione di data 
	memcpy( data,
            &message->payload.verify_req.body,
            sizeof(PHEMAP_Link_t));  
	memxor(data, &chain->links[1], sizeof(PHEMAP_Link_t));
	
    return 0;
}

/**
 * @brief Create a new verification reply message. 
 *
 * @param [in]	chain
 * 				Pointer to the currently in use PHEMAP chain instance.
 *
 * @param [in]	message
 * 				Pointer to the PHEMAP message instance.
 * 
 * @param [in]	data
 * 				Pointer to the data buffer container.
 */
static void PHEMAP_AS_buildVerificationReply(
		PHEMAP_Chain_t * const chain,
		PHEMAP_Message_t * const message)                           //questo tipo ovviamente va cambiatoTODO
{
	memset(message, 0, sizeof(PHEMAP_Message_t));

	message->type = verification_reply;

	// Copia dell'attuale link della chain
	memcpy( &message->payload.verify_reply.l_2, 
			&chain->links[2], 
			sizeof(PHEMAP_Link_t));
}



int32_t PHEMAP_AS_VerifiedRecv(
		PHEMAP_AS_t * const as, 
		PHEMAP_Device_ID_t device_id,
		PHEMAP_Link_t * data)
{
    ch_msg_t msg;
	msg.cli_id = device_id;				//mappare phy_id con PHEMAP_id in modo migliore
	// msg.size = sizeof(PHEMAP_Message_t);
    
	printf("attendo la richiesta dal device\n");			//DELME
	// ricezione della richiesta
	// while (ch_msg_pop(&as->chnl, &msg)!=0);				//così è bloccante
	/* timer creation */
    linux_timer_t tim;
    linux_create_timer(&tim, 100);			//fai una define diviso 5
	int32_t elaps = 0;
	int32_t res = 1;
	// Si aspetta la ricezione della richiesta
	res = ch_msg_pop(&as->chnl, &msg);
	while (res!=0 && elaps!=5)
	{
        linux_wait_period(&tim);
		res = ch_msg_pop(&as->chnl, &msg);
		elaps++;
	}	
	if(res != 0)
	{
		return -1;
	}

	PHEMAP_Chain_t chain;

	printf("estraggo la chain\n");			//DELME
	// estraggo la chain del device con cui voglio parlare
	if (-1 == PHEMAP_Chain_peekLink(
						as->database_name, 
						device_id, 
						0,
						3,//di 3 elementi
						RET_NO_SENTINEL,
						&chain))
	{
		return -1;
	}	

	// controllo il messaggio di verification request
    PHEMAP_Message_t * message;
    message = (PHEMAP_Message_t*)msg.buff;	//casto a questo tipo così da risparmiare una copia
	if (-1 == 	PHEMAP_AS_checkVerificationRequest(&chain, message, data))
	{	
		return -1;
	}

	printf("creo l'ack\n");			//DELME
	PHEMAP_AS_buildVerificationReply(&chain, message);

	printf("invio la risposta\n");			//DELME
	// invio del messaggio di init request
    if(ch_msg_push(&as->chnl, &msg)!=0)
	{
		return -1;
	}

	return 0;
}