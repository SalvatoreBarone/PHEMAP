/**
 * @file phemap_verification.c
 *
 * @author Salvatore Barone <salvator.barone@gmail.com>
 *
 * Copyright 2019 Salvatore Barone <salvator.barone@gmail.com>
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

#include "phemap_dev_verification.h"
#include "phemap_message.h"
#include "phemap_dev_phi.h"
#include "memxor.h"

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
static void PHEMAP_Device_buildVerificationRequest(
		PHEMAP_Device_t * const device,
		PHEMAP_Message_t * const message,
		const PHEMAP_Link_t * const data)                     //questo tipo ovviamente va cambiatoTODO
{
	printf("init messaggio\n");			//DELME
    //inizializzo il messaggio
	memset(message, 0, sizeof(PHEMAP_Message_t));

	message->type = verification_request;

	printf("scrivo i campi del messaggio\n");			//DELME
	PHEMAP_Device_getNextLink(device, RET_NO_SENTINEL, &message->payload.verify_req.l_1);
	PHEMAP_Device_getNextLink(device, RET_NO_SENTINEL, &message->payload.verify_req.body);
	memxor(&message->payload.verify_req.body, data, sizeof(PHEMAP_Link_t));
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
static int32_t PHEMAP_Device_checkVerificationReply(
		PHEMAP_Device_t * const device,
		PHEMAP_Message_t * const message)
{
	if (verification_reply != message->type)
		return -1;

    PHEMAP_Link_t link;
	PHEMAP_Device_getNextLink(device, RET_NO_SENTINEL, &link);

    if (0 != memcmp(&message->payload.verify_reply.l_2,
                    &link, 
                    sizeof(PHEMAP_Link_t)))
    {
        return -1;
    }

	return 0;
}


/**
 * @brief Perform the mutual authentication protocol with the AS 
 *
 * @param [in]	device	
 * 				Pointer to the PHEMAP_Device_t instance which collect all
 * 				the device-related protocol information.
 *
 * @return Upon successful completion, 0 shall be returned; otherwise, -1 shall 
 * be returned.
 */
int32_t PHEMAP_Device_VerifiedSend( PHEMAP_Device_t * const device,
									const PHEMAP_Link_t * const data)
{
	ch_msg_t msg;
	msg.cli_id = 0;				//qua va data una macro tipo VERIF_ADDR
	msg.size = sizeof(PHEMAP_Message_t);

	PHEMAP_Message_t * message;
    message = (PHEMAP_Message_t*)msg.buff;	//casto a questo tipo così da risparmiare una copia

	printf("costruisco la richiesta\n");			//DELME
	// Si costruisce la richiesta al verifier
	PHEMAP_Device_buildVerificationRequest(device, message, data);

	printf("invio la richiesta\n");			//DELME
	// Si invia la richiesta al verifier
    while (ch_msg_push(&device->chnl, &msg)!=0);

	printf("attendo la ricezione dell'ack\n");			//DELME
	// Si aspetta la ricezione dell'ack per autorizzare il verifier
	while (ch_msg_pop(&device->chnl, &msg)!=0);	

	printf("controllo l'ack\n");			//DELME
	// Se l'ack viene ricevuto, si passa alla sua analisi per
	// completare la procedura di verifica
	if (0 !=  PHEMAP_Device_checkVerificationReply(device, message))
	{
		return -1;
	}

	return 0;
}
