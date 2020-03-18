/**
 * @file phemap_babel_chain.c
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
#include "phemap_dev_babel_chain.h"
#include "phemap_message.h"
#include "phemap_dev_phi.h"
#include "memxor.h"
#include "linux_timer.h"

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
static void PHEMAP_Device_buildBCSetupRequest(
		PHEMAP_Device_t * const device,
		PHEMAP_Message_t * const message,
		PHEMAP_Device_ID_t dest_id,
		uint32_t max_messages)                     
{
    printf("init messaggio di BCSetup\n");			//DELME
    //inizializzo il messaggio
	memset(message, 0, sizeof(PHEMAP_Message_t));

    message->type = bc_setup_request;

	printf("scrivo i campi del messaggio\n");			//DELME
    message->payload.carnet_req.device_ID = dest_id;	
    message->payload.carnet_req.carnet_length = max_messages;
	PHEMAP_Device_getNextLink(device, 
								&device->verifier_ent, 
								RET_NO_SENTINEL, 
								&message->payload.carnet_req.l_i);

	device->send_ent.entity.dev_id = dest_id;
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
static int32_t PHEMAP_Device_checkBCSetupReply(
		PHEMAP_Device_t * const device,
		PHEMAP_Message_t * const message)
{
	if (bc_setup_reply != message->type)
		return -1;

    PHEMAP_Link_t link;
	PHEMAP_Device_getNextLink(device, &device->verifier_ent, RET_NO_SENTINEL, &link);

    if (0 != memcmp(&message->payload.carnet_reply.l_i,
                    &link, 
                    sizeof(PHEMAP_Link_t)))
    {
        return -1;
    }

    memcpy( &device->send_ent.carnet,
            &message->payload.carnet_reply.carnet,
            sizeof(PHEMAP_Carnet_t));
    memcpy( &device->send_ent.entity.Q,
            &link, 
            sizeof(PHEMAP_Link_t));

	return 0;
}

static int32_t PHEMAP_Device_checkBCSetupNotify(
		PHEMAP_Device_t * const device,
		PHEMAP_Message_t * const message,
        uint32_t * carnet_length)
{
	if (bc_setup_request != message->type)
		return -1;

    PHEMAP_Link_t link;
	PHEMAP_Device_getNextLink(device, &device->verifier_ent, RET_NO_SENTINEL, &link);

    if (0 != memcmp(&message->payload.carnet_req.l_i,
                    &link, 
                    sizeof(PHEMAP_Link_t)))
    {
        return -1;
    }

    *carnet_length = message->payload.carnet_req.carnet_length;
    
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
static void PHEMAP_Device_buildBCSetupAck(
		PHEMAP_Device_t * const device,
		PHEMAP_Message_t * const message)                     
{
    printf("init messaggio di BCSetup Ack\n");			//DELME
    //inizializzo il messaggio
	memset(message, 0, sizeof(PHEMAP_Message_t));

    message->type = bc_setup_ack;

	printf("scrivo i campi del messaggio\n");			//DELME
    PHEMAP_Device_getNextLink(device, 
								&device->verifier_ent,
								RET_NO_SENTINEL, 
								&message->payload.carnet_ack.l_carlen);
}

/**
 * @brief Create a new connection with a peer device.
 *
 * @details
 * This function starts the Babel-chain PHEMAP protocol to allow two peer device
 * to mutually authenticate each other while communicating.
 *
 * @param [in]	device	
 * 				Pointer to the PHEMAP_Device_t instance which collect all
 * 				the device-related protocol information.
 *
 * @param [in]	dest_id
 * 				ID of the destination device.
 *
 * @param [in]	max_messages
 * 				Expected number of exchanged messages. It determines the amount
 * 				of tickets in the Babel-chain PHEMAP carnet.
 * 				
 * @return Upon successful completion, 0 shall be returned; otherwise, -1 shall 
 * be returned.
 */
int32_t PHEMAP_Device_BCconnectToDev(
		PHEMAP_Device_t * const device,
		PHEMAP_Device_ID_t dest_id,
		uint32_t max_messages)
{
    ch_msg_t msg;
	msg.cli_id = 0;				//qua va data una macro tipo VERIF_ADDR
	msg.size = sizeof(PHEMAP_Message_t);

	PHEMAP_Message_t * message;
    message = (PHEMAP_Message_t*)msg.buff;	//casto a questo tipo così da risparmiare una copia

    printf("costruisco la richiesta Babel Chain \n");			//DELME
	// Si costruisce la richiesta al verifier	
    PHEMAP_Device_buildBCSetupRequest(device, message, dest_id, max_messages);

    printf("invio la richiesta Babel Chain\n");			//DELME
	// Si invia la richiesta al verifier
    // while (ch_msg_push(&device->chnl, &msg)!=0);
    if(ch_msg_push(&device->chnl, &msg)!=0)
	{
		return -1;
	}

    /* timer creation */
    linux_timer_t tim;
    linux_create_timer(&tim, 100);			//fai una define diviso 5
	int32_t elaps = 0;
	int32_t res = 1;
	// Si aspetta la ricezione della risposta 
	while (res!=0 && elaps!=5)				//così conti 5 volte e sei più responsive
	{
        linux_wait_period(&tim);
		res = ch_msg_pop(&device->chnl, &msg);
		elaps++;
	}	
	if(res != 0)
	{
		return -1;
	}

    printf("controllo la risposta Babel Chain\n");			//DELME
	// Se la risposta viene ricevuta, si passa alla sua analisi per
	// completare la procedura di installazione del carnet
	if (0 !=  PHEMAP_Device_checkBCSetupReply(device, message))
	{
		return -1;
	}

    // Se l'installazione del carnet è andata a buon fine si 
    // allinea il registro Q del verifier al nuovo link
    PHEMAP_Device_peekLink(device, 
							&device->verifier_ent,
							max_messages, 
							RET_NO_SENTINEL, 
							&device->verifier_ent.Q);
    device->verifier_ent.counter += max_messages;
    return 0;
}

/**
 * @brief Create a new connection with a peer device.
 *
 * @details
 * This function starts the Babel-chain PHEMAP protocol to allow two peer device
 * to mutually authenticate each other while communicating.
 *
 * @param [in]	device	
 * 				Pointer to the PHEMAP_Device_t instance which collect all
 * 				the device-related protocol information.
 *
 * @param [in]	dest_id
 * 				ID of the destination device.
 *
 * @param [in]	max_messages
 * 				Expected number of exchanged messages. It determines the amount
 * 				of tickets in the Babel-chain PHEMAP carnet.
 * 				
 * @return Upon successful completion, 0 shall be returned; otherwise, -1 shall 
 * be returned.
 */
int32_t PHEMAP_Device_BCNotifyRecv(
		PHEMAP_Device_t * const device,
		PHEMAP_Device_ID_t dest_id)
{
    ch_msg_t msg;
	msg.cli_id = 0;				//qua va data una macro tipo VERIF_ADDR
	msg.size = sizeof(PHEMAP_Message_t);

    printf("ricevo la notify Babel Chain Setup\n");			//DELME
    /* timer creation */
    linux_timer_t tim;
    linux_create_timer(&tim, 100);			//fai una define diviso 5
	int32_t elaps = 0;
	int32_t res = 1;
	// Si aspetta la ricezione della risposta 
    res = ch_msg_pop(&device->chnl, &msg);
	while (res!=0 && elaps!=5)				//così conti 5 volte e sei più responsive
	{
        linux_wait_period(&tim);
		res = ch_msg_pop(&device->chnl, &msg);
		elaps++;
	}	
	if(res != 0)
	{
		return -1;
	}

    uint32_t carnet_length = 0;
    PHEMAP_Message_t * message;
    message = (PHEMAP_Message_t*)msg.buff;	//casto a questo tipo così da risparmiare una copia

    printf("controllo la notify Babel Chain\n");			//DELME
	// Se si riceve la notify, si passa alla sua analisi per
	// completare la procedura di migrazione del registro Q
	if (0 !=  PHEMAP_Device_checkBCSetupNotify(device, message, &carnet_length))
	{
		return -1;
	}

    // copio il valore del registro Q del verifier in in quello per la ricezione
    memcpy( &device->recv_ent.Q,
			&device->verifier_ent.Q,
			sizeof(PHEMAP_Link_t));
	device->recv_ent.counter = device->verifier_ent.counter;

    // allineo il registro Q del verifier al nuovo link
    PHEMAP_Device_peekLink(device, 
							&device->verifier_ent,
							carnet_length, 
							RET_NO_SENTINEL, 
							&device->verifier_ent.Q);
    device->verifier_ent.counter += carnet_length;

    printf("costruisco l'ack Babel Chain \n");			//DELME
	// Si costruisce la richiesta al verifier	
    PHEMAP_Device_buildBCSetupAck(device, message);

    printf("invio l'ack Babel Chain\n");			//DELME
	// Si invia l'ack al verifier
    // while (ch_msg_push(&device->chnl, &msg)!=0);
    if(ch_msg_push(&device->chnl, &msg)!=0)
	{
		return -1;
	}

    return 0;
}

// /**
//  * @brief Close the connection with the peer.
//  *
//  * @param [in]	device	
//  * 				Pointer to the PHEMAP_Device_t instance which collect all
//  * 				the device-related protocol information.
//  *
//  * @return Upon successful completion, 0 shall be returned; otherwise, -1 shall 
//  * be returned.
//  */
// inline int32_t PHEMAP_Device_closePeerConnection(PHEMAP_Device_t * const device)
// {
// 	memset(&device->carnet, 0, sizeof(PHEMAP_Carnet_t));
// 	return client_close(&device->peer);
// }

// /**
//  * @brief Sends data to a peer device.
//  *
//  * @param [in]	device	
//  * 				Pointer to the PHEMAP_Device_t instance which collect all
//  * 				the device-related protocol information.
//  *
//  * @param [in]	data
//  * 				Pointer to the memory bufer holding data to be sent.
//  *
//  * @param [in]	size
//  * 				Size of the data to be sent..
//  *
//  * @return Upon successful completion, the function shall return the number of 
//  * bytes sent. Otherwise, -1 shall be returned.
//  */
// ssize_t PHEMAP_Device_sendToPeer(
// 		PHEMAP_Device_t * const device,
// 		const uint8_t * const data,
// 		size_t size)
// {
// 	if (device->carnet.counter == device->carnet.length)
// 		return -1;

// 	// Ottengo b_j dal carnet di tickets
// 	memxor(
// 			&device->carnet.tickets[device->counter], 
// 			device->Q, 
// 			sizeof(PHEMAP_Link_t));

	
// 	uint8_t * data_ptr = (uint8_t*) data;
// 	uint8_t * l = (uint8_t *) &device->carnet.tickets[device->counter];


// 	// Xor con il tichet b_j 
// 	for (size_t i = 0, j = 0; i < size; i++, j = (j + 1) % sizeof(PHEMAP_Link_t)) 
// 		data_ptr[i] ^= l[j];

// 	ssize_t status = client_send(&device->peer, data, size);

// 	// Ripristino dei dati
// 	for (size_t i = 0, j = 0; i < size; i++, j = (j + 1) % sizeof(PHEMAP_Link_t)) 
// 		data_ptr[i] ^= l[j];

// 	if ((-1 != status) && (0 != status))
// 	{
// 		PHEMAP_Device_updateQ(device, 1);
// 		device->carnet.counter++;
// 	}

// 	return status;
// }

// /**
//  * @brief Receive data from a peer device.
//  *
//  * @param [in]	device	
//  * 				Pointer to the PHEMAP_Device_t instance which collect all
//  * 				the device-related protocol information.
//  *
//  * @param [in]	data
//  * 				Pointer to the memory bufer in which (eventually) received data 
//  * 				have to be stored. This buffer must be pre-allocated and its
//  * 				size must be greater (or equal) than the size of the data.
//  *
//  * @param [in]	size
//  * 				Size of the data to be received.
//  *
//  * @return Upon successful completion, the function shall return the length of 
//  * the received message in bytes. If no messages are available to be received 
//  * and the peer has performed an orderly shutdown, the function shall return 0. 
//  * Otherwise, -1 shall be returned.
//  */
// ssize_t PHEMAP_Device_recvFromPeer(
// 		PHEMAP_Device_t * const device,
// 		uint8_t * const data,
// 		size_t size)
// {
// 	if (device->carnet.counter == device->carnet.length)
// 		return -1;

// 	ssize_t status = client_recv(&device->peer, data, size);

// 	if ((-1 != status) && (0 != status))
// 	{
// 		// Ottengo b_j dal carnet di tickets
// 		memxor(
// 				&device->carnet.tickets[device->counter], 
// 				device->Q, 
// 				sizeof(PHEMAP_Link_t));

// 		uint8_t * l = (uint8_t *) &device->carnet.tickets[device->counter];

// 		// Cedrittazione 
// 		for (size_t i = 0, j = 0; i < size; i++, j = (j + 1) % sizeof(PHEMAP_Link_t)) 
// 			data[i] ^= l[j];
		
// 		PHEMAP_Device_updateQ(device, 1);
// 		device->carnet.counter++;
// 	}

// 	return status;
// }

