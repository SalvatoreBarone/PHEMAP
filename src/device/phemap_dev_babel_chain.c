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
	device->send_ent.entity.counter = device->verifier_ent.counter;

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
    // printf("init messaggio di BCSetup Ack\n");			//DELME
    //inizializzo il messaggio
	memset(message, 0, sizeof(PHEMAP_Message_t));

    message->type = bc_setup_ack;

	// printf("scrivo i campi del messaggio\n");			//DELME
    PHEMAP_Device_getNextLink(device, 
								&device->verifier_ent,
								RET_NO_SENTINEL, 
								&message->payload.carnet_ack.l_carlen);
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
static int32_t PHEMAP_Device_buildBCVerificationRequest(
		PHEMAP_Device_t * const device,
		PHEMAP_Message_t * const message,
		const PHEMAP_Link_t * const data)                     //questo tipo ovviamente va cambiatoTODO
{
	// printf("init messaggio\n");			//DELME
    //inizializzo il messaggio
	memset(message, 0, sizeof(PHEMAP_Message_t));

	message->type = bc_verification_request;

	if ((device->send_ent.carnet.count + 2) >= device->send_ent.carnet.length)
	{						//controllo che ci siano ticket disponibili fino all'ack
		return -1;
	}

	// printf("scrivo i campi del messaggio\n");			//DELME
	PHEMAP_Link_t link;
	memcpy( &link, 							//prendo il count-esimo elemento del carnet
			&device->send_ent.carnet.tickets[device->send_ent.carnet.count++],
			sizeof(PHEMAP_Link_t));
// //-------DELME--------------------------------------------//
// 	printf("CARNET link\n");			//DELME
// 	uint8_t * print_ptr;
	
// 	print_ptr = (uint8_t*) &link;
// 	for (int k = 0; k < sizeof(PHEMAP_Link_t); k++)
// 	{
// 		printf("%02x ", print_ptr[k]);
// 	}
// 	printf("\n");
// 	//---------------------------------------------------//

	PHEMAP_Device_getNextLink(device, 
								&device->send_ent.entity, 
								RET_NO_SENTINEL, 
								&message->payload.verify_req.l_1);
	// //---DELME------------------------------------------------//
	
	// print_ptr = (uint8_t*) &message->payload.verify_req.l_1;
	// for (int k = 0; k < sizeof(PHEMAP_Link_t); k++)
	// {
	// 	printf("%02x ", print_ptr[k]);
	// }
	// printf("\n");
	// // ---------------------------------------------------//
	memxor(&message->payload.verify_req.l_1, &link, sizeof(PHEMAP_Link_t));

	memcpy( &link, 							//prendo il count-esimo elemento del carnet
			&device->send_ent.carnet.tickets[device->send_ent.carnet.count++],
			sizeof(PHEMAP_Link_t));
	memcpy(&message->payload.verify_req.body, data, sizeof(PHEMAP_Link_t));
	memxor(&message->payload.verify_req.body, &link, sizeof(PHEMAP_Link_t));
	PHEMAP_Device_getNextLink(device, 
								&device->send_ent.entity, 
								RET_NO_SENTINEL, 
								&link);
	memxor(&message->payload.verify_req.body, &link, sizeof(PHEMAP_Link_t));
	
	return 0;
}

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
static int32_t PHEMAP_Device_checkBCVerificationRequest(
		PHEMAP_Device_t * const device,
		PHEMAP_Message_t * const message,
		PHEMAP_Link_t * const data)                 //questo tipo ovviamente va cambiatoTODO
{
	// validazione del messaggio
	if (bc_verification_request != message->type)
		return -1;
    
	PHEMAP_Link_t link;
	PHEMAP_Device_getNextLink(device, 
								&device->recv_ent, 
								RET_NO_SENTINEL, 
								&link);

// //------DELME---------------------------------------------//
// 	uint8_t * print_ptr;
	
// 	print_ptr = (uint8_t*) &link;
// 	for (int k = 0; k < sizeof(PHEMAP_Link_t); k++)
// 	{
// 		printf("%02x ", print_ptr[k]);
// 	}
// 	printf("\n");
// //---------------------------------------------------//

    if (0 != memcmp(&message->payload.verify_req.l_1,
                    &link, 
                    sizeof(PHEMAP_Link_t)))
    {
		return -1;
    }

	PHEMAP_Device_getNextLink(device, 
								&device->recv_ent, 
								RET_NO_SENTINEL, 
								&link);

	//valutare se fare inizializzazione di data 
	memcpy( data,
            &message->payload.verify_req.body,
            sizeof(PHEMAP_Link_t));  
	memxor(data, &link, sizeof(PHEMAP_Link_t));
	
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
static void PHEMAP_Device_buildBCVerificationAck(
		PHEMAP_Device_t * const device,
		PHEMAP_Message_t * const message)
{
	memset(message, 0, sizeof(PHEMAP_Message_t));

	message->type = bc_verification_ack;

	PHEMAP_Link_t link;
	PHEMAP_Device_getNextLink(device, 
								&device->recv_ent, 
								RET_NO_SENTINEL, 
								&link);

	// Copia dell'attuale link della chain
	memcpy( &message->payload.verify_ack.l_2, 
			&link, 
			sizeof(PHEMAP_Link_t));
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
 * @return Upon successful completion, 0 shall be returned; otherwise, -1 shall 
 * be returned.
 */
static int32_t PHEMAP_Device_checkBCVerificationAck(
		PHEMAP_Device_t * const device,
		PHEMAP_Message_t * const message)
{
	if (bc_verification_ack != message->type)
		return -1;

    PHEMAP_Link_t link[2];
	memcpy( &link[0], 							//prendo il count-esimo elemento del carnet
			&device->send_ent.carnet.tickets[device->send_ent.carnet.count++],
			sizeof(PHEMAP_Link_t));
	PHEMAP_Device_getNextLink(device, 
								&device->send_ent.entity, 
								RET_NO_SENTINEL, 
								&link[1]);
	memxor(&link[0], &link[1], sizeof(PHEMAP_Link_t));

    if (0 != memcmp(&message->payload.verify_ack.l_2,
                    &link[0], 
                    sizeof(PHEMAP_Link_t)))
    {
        return -1;
    }

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

	while (ch_msg_pop(&device->chnl, &msg)!=0);	

    // /* timer creation */
    // linux_timer_t tim;
    // linux_create_timer(&tim, 100);			//fai una define diviso 5
	// int32_t elaps = 0;
	// int32_t res = 1;
	// // Si aspetta la ricezione della risposta 
	// while (res!=0 && elaps!=5)				//così conti 5 volte e sei più responsive
	// {
    //     linux_wait_period(&tim);
	// 	res = ch_msg_pop(&device->chnl, &msg);
	// 	elaps++;
	// }	
	// if(res != 0)
	// {
	// 	return -1;
	// }

    printf("controllo la risposta Babel Chain\n");			//DELME
	// Se la risposta viene ricevuta, si passa alla sua analisi per
	// completare la procedura di installazione del carnet
	if (0 !=  PHEMAP_Device_checkBCSetupReply(device, message))
	{
		return -1;
	}

    // Se l'installazione del carnet è andata a buon fine si 
    // allinea il registro Q del verifier al nuovo link
    int senti_num = PHEMAP_Device_peekLink( device, 
											&device->verifier_ent,
											max_messages, 
											RET_NO_SENTINEL, 
											&device->verifier_ent.Q);
    device->verifier_ent.counter = device->verifier_ent.counter + max_messages + senti_num;
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
		PHEMAP_Device_t * const device)
{
    ch_msg_t msg;
	msg.cli_id = 0;				//qua va data una macro tipo VERIF_ADDR
	msg.size = sizeof(PHEMAP_Message_t);

    printf("ricevo la notify Babel Chain Setup\n");			//DELME
	while (ch_msg_pop(&device->chnl, &msg)!=0);	

    // /* timer creation */
    // linux_timer_t tim;
    // linux_create_timer(&tim, 100);			//fai una define diviso 5
	// int32_t elaps = 0;
	// int32_t res = 1;
	// // Si aspetta la ricezione della risposta 
    // res = ch_msg_pop(&device->chnl, &msg);
	// while (res!=0 && elaps!=5)				//così conti 5 volte e sei più responsive
	// {
    //     linux_wait_period(&tim);
	// 	res = ch_msg_pop(&device->chnl, &msg);
	// 	elaps++;
	// }	
	// if(res != 0)
	// {
	// 	return -1;
	// }

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

    // copio il valore del registro Q del verifier in quello per la ricezione
    memcpy( &device->recv_ent.Q,
			&device->verifier_ent.Q,
			sizeof(PHEMAP_Link_t));
	device->recv_ent.counter = device->verifier_ent.counter;

	// allineo il registro Q del verifier al nuovo link
    uint32_t senti_num = PHEMAP_Device_peekLink(device, 
												&device->verifier_ent,
												carnet_length, 
												RET_NO_SENTINEL, 
												&device->verifier_ent.Q);
    device->verifier_ent.counter = device->verifier_ent.counter + carnet_length + senti_num;

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

int32_t PHEMAP_Device_BCsend(
		PHEMAP_Device_t * const device,
		PHEMAP_Device_ID_t dest_id,
		PHEMAP_Link_t * const data)
{
    ch_msg_t msg;
	msg.cli_id = dest_id;
	msg.size = sizeof(PHEMAP_Message_t);

	PHEMAP_Message_t * message;
    message = (PHEMAP_Message_t*)msg.buff;	//casto a questo tipo così da risparmiare una copia

    printf("costruisco la Babel Chain verify\n");			//DELME
	// Si costruisce la richiesta di verifica	
    PHEMAP_Device_buildBCVerificationRequest(device, message, data);

    printf("invio la richiesta Babel Chain verify\n");			//DELME
	// Si invia la richiesta al verifier
    // while (ch_msg_push(&device->chnl, &msg)!=0);
    if(ch_msg_push(&device->chnl, &msg)!=0)
	{
		return -1;
	}

	// Si aspetta la ricezione della risposta 
    printf("attendo l'ack Babel Chain verify\n");			//DELME
	while (ch_msg_pop(&device->chnl, &msg)!=0);	

    // /* timer creation */
    // linux_timer_t tim;
    // linux_create_timer(&tim, 100);			//fai una define diviso 5
	// int32_t elaps = 0;
	// int32_t res = 1;
	// // Si aspetta la ricezione della risposta 
	// while (res!=0 && elaps!=5)				//così conti 5 volte e sei più responsive
	// {
    //     linux_wait_period(&tim);
	// 	res = ch_msg_pop(&device->chnl, &msg);
	// 	elaps++;
	// }	
	// if(res != 0)
	// {
	// 	return -1;
	// }

    printf("controllo l'ack Babel Chain verify\n");			//DELME
	// Se la risposta viene ricevuta, si passa alla sua analisi per
	// completare la procedura di installazione del carnet
	if (0 !=  PHEMAP_Device_checkBCVerificationAck(device, message))
	{
		return -1;
	}

    return 0;
}

int32_t PHEMAP_Device_BCrecv(
		PHEMAP_Device_t * const device,
		PHEMAP_Device_ID_t src_id,
		PHEMAP_Link_t * const data)
{
    ch_msg_t msg;
	msg.cli_id = src_id;
	msg.size = sizeof(PHEMAP_Message_t);

	PHEMAP_Message_t * message;
    message = (PHEMAP_Message_t*)msg.buff;	//casto a questo tipo così da risparmiare una copia

	// Si aspetta la ricezione della risposta 
    printf("attendo la Babel Chain verify req\n");			//DELME
	while (ch_msg_pop(&device->chnl, &msg)!=0);	

    // /* timer creation */
    // linux_timer_t tim;
    // linux_create_timer(&tim, 100);			//fai una define diviso 5
	// int32_t elaps = 0;
	// int32_t res = 1;
	// // Si aspetta la ricezione della risposta 
	// while (res!=0 && elaps!=5)				//così conti 5 volte e sei più responsive
	// {
    //     linux_wait_period(&tim);
	// 	res = ch_msg_pop(&device->chnl, &msg);
	// 	elaps++;
	// }	
	// if(res != 0)
	// {
	// 	return -1;
	// }

    printf("controllo la Babel Chain verify req\n");			//DELME
	// Se la risposta viene ricevuta, si passa alla sua analisi per
	// completare la procedura di installazione del carnet
	if (0 !=  PHEMAP_Device_checkBCVerificationRequest(device, message, data))
	{
		return -1;
	}

	printf("costruisco il Babel Chain verify ack\n");			//DELME
	// Si costruisce l'ack	
    PHEMAP_Device_buildBCVerificationAck(device, message);

    printf("invio il Babel Chain verify ack\n");			//DELME
	// Si invia l'ack al device
    // while (ch_msg_push(&device->chnl, &msg)!=0);
    if(ch_msg_push(&device->chnl, &msg)!=0)
	{
		return -1;
	}

    return 0;
}
