/**
 * @file phemap_init.c
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

#include "typedefs.h"

#include "phemap_dev_init.h"
#include "phemap_message.h"
#include "phemap_dev_phi.h"
#include "memxor.h"

/**
 * @brief Checks if an init request is well formed. 
 *
 * @param [in]	device	
 * 				Pointer to the PHEMAP_Device_t instance which collect all
 * 				the device-related protocol information.
 *
 * @param [in]	message
 * 				Pointer to the received message.
 *
 * @return Upon successful completion, 0 shall be returned; otherwise, -1 shall 
 * be returned.
 */
static int32_t PHEMAP_Device_checkInitReq(
		PHEMAP_Device_t * const device,
		PHEMAP_Message_t * const message)
{
	// validazione del messaggio
	if (init_request != message->type)
		return -1;
	
	printf("faccio la xor del messaggio ricevuto\n");			//DELME

    PHEMAP_Link_t gen_link, recv_link, l_i;
	memcpy(&recv_link, &message->payload.init_req.v_1, sizeof(PHEMAP_Link_t));  //forse serve il pointer
	memxor(&recv_link, &message->payload.init_req.v_2, sizeof(PHEMAP_Link_t));

	printf("prendo la challenge dal messaggio\n");			//DELME

	memcpy(&l_i, &message->payload.init_req.l_i, sizeof(PHEMAP_Link_t));
	memset(&gen_link, 0, sizeof(PHEMAP_Link_t));
	
	printf("faccio la xor di SENTINEL link\n");			//DELME

	// Calcolo i link successivi ed effettuo la XOR
	for (uint32_t j = 1; j < SENTINEL; j++)
	{
		PHEMAP_Device_peekLink(device, &device->verifier_ent, 1, RET_SENTINEL,&l_i);
		memxor(&gen_link, l_i, sizeof(PHEMAP_Link_t));
	}

	printf("comparo il link generato con quello ricevuto\n");			//DELME

	// verifico che tutto combaci
	if (0 != memcmp(&gen_link, &recv_link, sizeof(PHEMAP_Link_t)))
		return -1;

	return 0;
}

/**
 * @brief Builds a PHEMAP init reply message
 *
 * @param [in]	device	
 * 				Pointer to the PHEMAP_Device_t instance which collect all
 * 				the device-related protocol information.
 *
 * @param [in]	message
 * 				Pointer to the received init-request message. It will contain
 * 				the init-reply message.
 *
 * @param [in]	d_2
 * 				This information will be used later, during the initialization
 * 				procedure, to check the init ack message in order to 
 * 				authenticate the AS.
 */
static void PHEMAP_Device_buildInitReply(
		PHEMAP_Device_t * const device,
		PHEMAP_Message_t * const message,
		PHEMAP_Link_t * const d_2)
{
	printf("copio la challenge\n");			//DELME

	PHEMAP_Link_t d_1, nonce;
    //prendo il root link dal messaggio
	memcpy(&d_1, &message->payload.init_req.l_i, sizeof(PHEMAP_Link_t));   //le & di riferimento
	memcpy(d_2, &message->payload.init_req.l_i, sizeof(PHEMAP_Link_t));   //le & di riferimento

	printf("genero la nonce\n");			//DELME

	// Generazione del nonce. Viene depositato all'interno del messaggio.
    uint8_t * tmp_ptr = (uint8_t*) &nonce;
	srand(time(NULL));
	for (size_t i = 0; i < sizeof(PHEMAP_Link_t); i++)
		tmp_ptr[i] = rand();

	printf("ottengo d1 e d2 in xor con la nonce\n");			//DELME

	PHEMAP_Device_peekLink(device, &device->verifier_ent, SENTINEL, RET_SENTINEL, &d_1);
	PHEMAP_Device_peekLink(device, &device->verifier_ent,SENTINEL + 1, RET_SENTINEL, d_2);
	memxor(&d_1, &nonce, sizeof(PHEMAP_Link_t));                              //le & di riferimento
	memxor(d_2, &nonce, sizeof(PHEMAP_Link_t));                              //le & di riferimento

	printf("init messaggio\n");			//DELME

    //inizializzo il messaggio
	memset(message, 0, sizeof(PHEMAP_Message_t));

	printf("scrivo i campi del messaggio\n");			//DELME

	message->type = init_reply;
	memcpy(&message->payload.init_reply.d_1, &d_1, sizeof(PHEMAP_Link_t));    //le & di riferimento
	memcpy(&message->payload.init_reply.d_2, d_2, sizeof(PHEMAP_Link_t));    //le & di riferimento
}


/**
 * @brief Checks if the intt ack message is well formed.
 *
 * @param [in]	device	
 * 				Pointer to the PHEMAP_Device_t instance which collect all
 * 				the device-related protocol information.
 *
 * @param [in]	message
 * 				Pointer to the received init-request message. It will contain
 * 				the init-reply message.
 *
 * @param [in] 	l_0
 * 				It is the chain root contained into the init-request received
 * 				from the AS.
 *
 * @param [in]	d_2
 * 				This information has been generated during the init-reply 
 * 				building. It will be used to check the init ack message, in 
 * 				order to authenticate the AS.
 *
 * @return Upon successful completion, 0 shall be returned; otherwise, -1 shall 
 * be returned.
 */
static int32_t PHEMAP_Device_checkInitAck(
		PHEMAP_Device_t * const device,
		PHEMAP_Message_t * const message,
		PHEMAP_Link_t * const d_2)
{
	if (init_ack != message->type)
		return -1;

	memxor(d_2, &message->payload.init_ack.v_3, sizeof(PHEMAP_Link_t));

	PHEMAP_Link_t s_1, s_2;
	memcpy(&s_1, &message->payload.init_ack.l_2 , sizeof(PHEMAP_Link_t));
	memcpy(&s_2, &message->payload.init_ack.l_2 , sizeof(PHEMAP_Link_t));
	
	PHEMAP_Device_peekLink(device, &device->verifier_ent, SENTINEL + 1, RET_SENTINEL, &s_1);
	PHEMAP_Device_peekLink(device, &device->verifier_ent, SENTINEL + 2, RET_SENTINEL, &s_2);
	memxor(&s_1, &s_2, sizeof(PHEMAP_Link_t));

	if (0 != memcmp(&s_1, d_2, sizeof(PHEMAP_Link_t)))
		return -1;

	memcpy(d_2, &s_2, sizeof(PHEMAP_Link_t));	//potrei salvare direttamente sul device.Q

	return 0;
}

/**
 * @brief PHEMAP protocol initialization - Device side.
 *
 * @details
 * This function performs PHEMAP protocol initialization in order to achieve
 * mutual authentication betbeew device and authentication service.
 *
 * @note
 * This function must be called after the device initialization (after a call
 * to the PHEMAP_Device_Init() function) in order to allow mutual authentication
 * between the device and the authentication service. Moreover, this function
 * must be caled every time a "retry" reply is received as response to a 
 * babel-chain or salted carnet request.
 *
 * @param [in]	device	
 * 				Pointer to the PHEMAP_Device_t instance which collect all
 * 				the device-related protocol information.
 *
 * @return Upon successful completion, 0 shall be returned; otherwise, -1 shall 
 * be returned.
 */
int32_t PHEMAP_Device_PHEMAPInit(PHEMAP_Device_t * const device)
{

	ch_msg_t msg;
	msg.cli_id = 0;				//qua va data una macro tipo VERIF_ADDR
	msg.size = sizeof(PHEMAP_Message_t);
    
	PHEMAP_Message_t * message;
    message = (PHEMAP_Message_t*)msg.buff;	//casto a questo tipo così da risparmiare una copia
	
	printf("attendo il messaggio di init\n");			//DELME

	// Attendo la ricezione, da parte dell'authentication service, del messaggio
	// init request. Questa richiesta viene inviata dall'authentication service
	// in tre occasioni:
	// 1) appena il device crea la connessione TCP con l'authentication service;
	// 2) dopo una risposta "retry" dovuta all'esaurimento della chain; questo
	//    evento si può manifestare se:
	//     - il device richiede un babel-chain carnet, ma l'authentication
	//       service non è in grado di servire la richiesta con i link rimasti
	//       sulla chain;
	//     - il device richiede un salt ed un salted carnet, ma l'authentication
	//       service non è in grado di servire la richiesta con i link rimasti
	//       sulla chain;
	//
	// È ovvio che, qualora venga ricevuta una risposta retry, è doveroso
	// chiamate questa funzione.
	while (ch_msg_pop(&device->chnl, &msg)!=0);	

	printf("controllo il messaggio\n");			//DELME

	// Se la ricezione è avvenuta correttamente si procede all'autenticazione 
	// dell'AS attraverso l'analisi della richiesta.
	if (0 != PHEMAP_Device_checkInitReq(device, message))	
		return -1;

	printf("costruisco la risposta\n");			//DELME

	// Se la verifica del messaggio ricevuto dall'authentication service
	// è andata a buon fine, il device NON è in grado di autenticare AS
	// (il messaggio ricevuto potrebbe essere un replay). Viene, però, salvato
	// il link l_0 e viene preparata una init reply da inviare all'AS
	PHEMAP_Link_t d_2;
	PHEMAP_Device_buildInitReply(device, message, &d_2);

	printf("invio la risposta\n");			//DELME

	// Si invia la risposta all'authentication service
    // while (ch_msg_push(&device->chnl, &msg)!=0);
    if (ch_msg_push(&device->chnl, &msg)!=0);

	printf("attendo la ricezione dell'ack\n");			//DELME

	// Si aspetta la ricezione dell'ack per completare l'autenticazione
	while (ch_msg_pop(&device->chnl, &msg)!=0);				

	printf("controllo l'ack\n");			//DELME

	// Se l'ack viene ricevuto correttamente, si passa alla sua analisi per
	// completare la procedura di autenticazione.
	if (0 !=  PHEMAP_Device_checkInitAck(device, message, &d_2))
		return -1;
	
	printf("aggiorno Q\n");			//DELME
	// Se la verifica del messaggio di init ack va a buon fine, il device
	// aggiorna il registro Q
	memcpy(&device->verifier_ent.Q, &d_2, sizeof(PHEMAP_Link_t));
	device->verifier_ent.counter = 0;

	// Il link dovrebbe essere SENTINEL+2, ma questo link è la root-sentinel,
	// che non deve mai essere scambiata, motivo per cui si avanza direttamente
	// al link successivo.
	// PHEMAP_Device_updateQ(device, SENTINEL + 3);//TODO

	return 0;
}

