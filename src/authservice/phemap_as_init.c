/**
 * @file phemap_as_init.c
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
#include "phemap_as_init.h"
#include "typedefs.h"
#include "phemap_message.h"
#include "memxor.h"

/**
 * @brief Create a new init request message. 
 *
 * @param [in]	chain
 * 				Pointer to the currently in use PHEMAP chain instance.
 *
 * @param [in]	message
 * 				Pointer to the PHEMAP message instance.
 */
static void PHEMAP_AS_buildInitRequest(
		PHEMAP_Chain_t * const chain,
		PHEMAP_Message_t * const message)
{
	memset(message, 0, sizeof(PHEMAP_Message_t));

	message->type = init_request;

	// Copia del primo link della chain
	memcpy( &message->payload.init_req.l_i, 
			&chain->links[0], 
			sizeof(PHEMAP_Link_t));

	// Generazione del nonce. Viene depositato all'interno del messaggio.
    uint8_t * tmp_ptr1 = (uint8_t*) &message->payload.init_req.v_1[0];  //forse senza [0]
    uint8_t * tmp_ptr2 = (uint8_t*) &message->payload.init_req.v_2[0];
	srand(time(NULL));
	for (size_t i = 0; i < sizeof(PHEMAP_Link_t); i++)
		tmp_ptr1[i] = tmp_ptr2[i] = rand();

	for (uint32_t j = 1; j < SENTINEL-1; j++)
		memxor(	&message->payload.init_req.v_1, 
				&chain->links[j], 
				sizeof(PHEMAP_Link_t));

	memxor(	&message->payload.init_req.v_2, 
			&chain->links[SENTINEL-1],          
			sizeof(PHEMAP_Link_t));

}

/**
 * @brief Checks if a received init reply is well formed. 
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
static int32_t PHEMAP_AS_checkInitReply(
		PHEMAP_Chain_t * const chain,
		PHEMAP_Message_t * const message,
		PHEMAP_Link_t * const nonce)
{
	// validazione del messaggio
	if (init_reply != message->type)
		return -1;

    PHEMAP_Link_t gen_link, recv_link;

	memcpy(&gen_link, &chain->links[SENTINEL], sizeof(PHEMAP_Link_t));  
	memxor(&gen_link, &chain->links[SENTINEL+1], sizeof(PHEMAP_Link_t)); 

	memcpy(&recv_link, &message->payload.init_reply.d_1, sizeof(PHEMAP_Link_t)); 
	memxor(&recv_link, &message->payload.init_reply.d_2, sizeof(PHEMAP_Link_t));
	
    if (0 != memcmp(&gen_link, &recv_link, sizeof(PHEMAP_Link_t)))
		return -1;

	// solo quando tutto è andato bene estraggo il nonce dal messaggio
	memcpy(nonce, &message->payload.init_reply.d_1, sizeof(PHEMAP_Link_t));  
	memxor(nonce, &chain->links[SENTINEL], sizeof(PHEMAP_Link_t));   
	
    return 0;
}

/**
 * @brief Create a new init ack message. 
 *
 * @param [in]	chain
 * 				Pointer to the currently in use PHEMAP chain instance.
 *
 * @param [in]	message
 * 				Pointer to the PHEMAP message instance.
 *
 * @param [in]	nonce
 * 				Memory area in which the nonce, extracted from the init reply
 * 				message, is placed
 */
static void PHEMAP_AS_buildInitAck(
		PHEMAP_Chain_t * const chain,
		PHEMAP_Message_t * const message,
		const PHEMAP_Link_t * const nonce)
{
	printf("init messaggio\n");			//DELME

	memset(message, 0, sizeof(PHEMAP_Message_t));

	printf("scrivo i campi del messaggio\n");			//DELME

	message->type = init_ack;
	
	memcpy(	&message->payload.init_ack.l_2, 
			&chain->links[0],                    
			sizeof(PHEMAP_Link_t));

	memcpy(	&message->payload.init_ack.v_3, 
			nonce, 
			sizeof(PHEMAP_Link_t));
	memxor(	&message->payload.init_ack.v_3, 
			&chain->links[SENTINEL + 2], 
			sizeof(PHEMAP_Link_t));
}

/**
 * @brief PHEMAP protocolo initialization - AS side. 
 *
 * @param [in]	as
 *				Pointer to the PHEMAP_AS_t instance.
 *
 * @param [in]	chain
 * 				Pointer to the currently in use PHEMAP chain instance.
 *
 * @return Upon successful completion, 0 shall be returned; otherwise, -1 shall 
 * be returned.
 */
int32_t PHEMAP_AS_PHEMAPInit(
		PHEMAP_AS_t * const as, 
		PHEMAP_Device_ID_t device_id)
{
    PHEMAP_Chain_t chain;

	printf("estraggo la chain\n");			//DELME

	// estraggo la chain del device con cui voglio parlare
	if (-1 == PHEMAP_Chain_peekLink(
						as->database_name, 
						device_id, 
						0,
						SENTINEL+3,
						RET_SENTINEL,
						&chain))
		return -1;

	printf("creo il messaggio init req\n");			//DELME

	// costruzione del messaggio di init request
    ch_msg_t msg;
	msg.cli_id = device_id;				//mappare phy_id con PHEMAP_id in modo migliore
	msg.size = sizeof(PHEMAP_Message_t);
    
	// PHEMAP_Message_t * request;
    // request = (PHEMAP_Message_t*)msg.buff;	//casto a questo tipo così da risparmiare una copia
	// PHEMAP_AS_buildInitRequest(&chain, request);	
    PHEMAP_Message_t * message;
    message = (PHEMAP_Message_t*)msg.buff;	//casto a questo tipo così da risparmiare una copia
	PHEMAP_AS_buildInitRequest(&chain, message);	

	printf("invio la richiesta\n");			//DELME

	// invio del messaggio di init request
    while (ch_msg_push(&as->chnl, &msg)!=0);

	printf("attendo la risposta\n");			//DELME

	// ricezione della risposta
	while (ch_msg_pop(&as->chnl, &msg)!=0);				

    // PHEMAP_Message_t *response;
    // response = (PHEMAP_Message_t*)msg.buff;	//casto a questo tipo così da risparmiare una copia

	printf("controllo la risposta\n");			//DELME

	// Se la ricezione è andata a buon fine si analizza la risposta.
	// PHEMAP_Link_t nonce;
	// if (-1 == PHEMAP_AS_checkInitReply(&chain, response, &nonce))	
	// 	return -1;
	PHEMAP_Link_t nonce;
	if (-1 == PHEMAP_AS_checkInitReply(&chain, message, &nonce))	
		return -1;

	printf("creo l'ack\n");			//DELME

	// Se la verifica della risposta spedita dal device va a buon fine,
	// l'authentication service è in grado di autenticare il device, ma il 
	// viceversa non è vero (il device potrebbe pensare che l'init request
	// ricevuto sia un replay di un messaggio precedente). Per questo è
	// necessario costruire e spedire l'ack, in modo che anche il device possa
	// completare l'autenticazione dell'AS.
	PHEMAP_AS_buildInitAck(&chain, message, &nonce);
	
	printf("invio l'ack\n");			//DELME

    // invio del messaggio di init request
    while (ch_msg_push(&as->chnl, &msg)!=0);

	// il link S+3 è la root sentinel, che non deve essere usata. Per questo
	// motivo, se l'inizializzazione è avvenuta correttamente, si fa avanzare
	// il puntatore al link corrente di SENTINEL + 4 posizioni.
    
	return 0;
}



