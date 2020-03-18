/**
 * @file phemap_as_babel_chain.c
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
#include <string.h>
#include "phemap_as_babel_chain.h"
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
static int32_t PHEMAP_AS_checkBCSetupRequest(
		PHEMAP_Chain_t * const chain,
		PHEMAP_Message_t * const message,
        PHEMAP_Device_ID_t * dest_id,
		uint32_t * carnet_length)
{
	// validazione del messaggio
	if (bc_setup_request != message->type)
		return -1;
    
    if (0 != memcmp(&message->payload.carnet_req.l_i,
                    &chain->links[0], 
                    sizeof(PHEMAP_Link_t)))
    {
		return -1;
    }

	*carnet_length = message->payload.carnet_req.carnet_length;
	*dest_id = message->payload.carnet_req.device_ID;

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
static void PHEMAP_AS_buildBCSetupReply(
		PHEMAP_Chain_t * const chainA,
		PHEMAP_Chain_t * const chainB,
		PHEMAP_Message_t * const message,
		uint32_t carnet_length)                 
{
	memset(message, 0, sizeof(PHEMAP_Message_t));

	message->type = bc_setup_reply;

	// Copia dell'attuale link della chain
	memcpy( &message->payload.carnet_reply.l_i, 
			&chainA->links[0], 
			sizeof(PHEMAP_Link_t));
    
    for (int i = 0; i < carnet_length; i++)
    {
        memcpy( &message->payload.carnet_reply.carnet.tickets[i],
                &chainA->links[i+1],
                sizeof(PHEMAP_Link_t));
        memxor( &message->payload.carnet_reply.carnet.tickets[i],
                &chainB->links[i+1],
                sizeof(PHEMAP_Link_t));
    }
    message->payload.carnet_reply.carnet.length = carnet_length;
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
static void PHEMAP_AS_buildBCSetupNotify(
		PHEMAP_Chain_t * const chainB,
		PHEMAP_Message_t * const message,
		uint32_t carnet_length)                 
{
	memset(message, 0, sizeof(PHEMAP_Message_t));

	message->type = bc_setup_request;

	// Copia dell'attuale link della chain
	memcpy( &message->payload.carnet_req.l_i, 
			&chainB->links[0], 
			sizeof(PHEMAP_Link_t));
    
    message->payload.carnet_req.carnet_length = carnet_length;
    message->payload.carnet_req.device_ID = 0;  //sostituire con define AS_ID
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
static int32_t PHEMAP_AS_checkBCSetupAck(
		PHEMAP_Chain_t * const chainB,
		PHEMAP_Message_t * const message,
		uint32_t carnet_length) 
{
    // validazione del messaggio
	if (bc_setup_ack != message->type)
		return -1;
    
    if (0 != memcmp(&message->payload.carnet_ack.l_carlen,
                    &chainB->links[carnet_length+1], 
                    sizeof(PHEMAP_Link_t)))
    {
		return -1;
    }

    return 0;
}

int32_t PHEMAP_AS_BabelChainSetup(
		PHEMAP_AS_t * const as, 
		PHEMAP_Device_ID_t device_id)
{
    ch_msg_t msg;
	msg.cli_id = device_id;				//mappare phy_id con PHEMAP_id in modo migliore
	// msg.size = sizeof(PHEMAP_Message_t);
    
	printf("attendo la richiesta Babel Chain dal device\n");			//DELME
	// ricezione della richiesta
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

	PHEMAP_Chain_t chainA;
    PHEMAP_Device_ID_t dest_id;
    uint32_t carnet_length;

	printf("estraggo il link\n");			//DELME
	// estraggo la chain del device con cui voglio parlare
	if (-1 == PHEMAP_Chain_getNextLink(
						as->database_name, 
						device_id, 
						0,
						&chainA.links[0]))
	{
		return -1;
	}	

	// controllo il messaggio di BabelChain Setup request
    PHEMAP_Message_t * message;
    message = (PHEMAP_Message_t*)msg.buff;	//casto a questo tipo così da risparmiare una copia
	if (-1 == 	PHEMAP_AS_checkBCSetupRequest(&chainA, message, &dest_id, &carnet_length))
	{	
		return -1;
	}

    // estraggo carnet_length+1 link dalla chain del device A
    if (-1 == PHEMAP_Chain_peekLink(
						as->database_name, 
						device_id, 
						0,
						(carnet_length+1),
						RET_NO_SENTINEL,
						&chainA))
	{
		return -1;
	}	

    // estraggo carnet_length link dalla chain del device B
	PHEMAP_Chain_t chainB;
    if (-1 == PHEMAP_Chain_peekLink(
						as->database_name, 
						dest_id, 
						0,
						(carnet_length+2), //carnet_length link per il carnet, e 2 per la mutua autenticazione della notify
						RET_NO_SENTINEL,
						&chainB))
	{
		return -1;
	}	

	printf("creo il carnet Babel chain di risposta\n");			//DELME
    PHEMAP_AS_buildBCSetupReply(&chainA, &chainB, message, carnet_length);

	printf("invio la risposta Babel Chain Setup\n");			//DELME
	// invio del messaggio di Babel Chain Setup reply
    if(ch_msg_push(&as->chnl, &msg)!=0)
	{
		return -1;
	}

    // avviso il device destinatario che deve aggiornare il suo registro Q
    msg.cli_id = dest_id;
    PHEMAP_AS_buildBCSetupNotify(&chainB, message, carnet_length);

    printf("invio la notify Babel Chain Setup\n");			//DELME
	// invio del messaggio di Babel Chain Setup notify
    if(ch_msg_push(&as->chnl, &msg)!=0)
	{
		return -1;
	}

    elaps = 0;
	res = 1;
	// Si aspetta la ricezione dell'ack
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

    if (-1 == PHEMAP_AS_checkBCSetupAck(&chainB, message, carnet_length))
	{	
		return -1;
	}

	return 0;
}