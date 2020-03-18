/**
 * @file message.h
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

#ifndef PHEMAP_MESSAGE_H
#define PHEMAP_MESSAGE_H

#include <inttypes.h>
#include "typedefs.h"
#include "phemap_carnet.h"

/**
 * @brief PHEMAP protocol message type
 */
typedef enum PHEMAP_PROTOCOL_MESSAGE_TYPE_T
{
	enrollment_request = 0,	/**< Request sent by the AS to D during the 
 							     enrollment phase */

	enrollment_reply = 1, /**< Reply for an enrollment request, sent by D to 
							   the AS */
	
	init_request = 2, /**< Request sent by the AS to D in order to start a new 
					       initialization procedure */

	init_reply = 3, /**< Reply for an initialization request, sent by D to 
					     the AS */

	init_ack = 4, /**< Ack message, sent by AS to D when the initialization
					   procedure is successfully completed. */

	verification_request = 5, /**< Request sent by the AS or the D in order to
							       authenticate the counterpart */

	verification_ack = 6, /**< Reply for a verification request */

	bc_setup_request = 7, /**< Babel-chain PHEMAP setup request, sent 
									    by D1 to the AS. */

	bc_setup_reply = 8, /**< Babel-chain PHEMAP carnet, sent by AS to D1 */

	bc_setup_ack = 9, /**< Babel-chain PHEMAP carnet, sent by D2 to AS */

	bc_verification_request = 10, /**< Babel-chain verification request */

	bc_verification_ack = 11, /**< Babel-chain verification ack */

	salt_request = 12, /**< Salted PHEMAP carnet request, sent by D to AS */

	salt_reply = 13, /**< Salted PHEMAP reply, sent by AS to D */

	salt_ack = 14, /**< Salted PHEAMP ack, sent by D to AS, while entering in
					   salted mode */

	salted_carnet = 15, /**< Salted PHEMAP carnet. Sent by AS to G */

	salt_invalidation = 16, /**< Salt invalidation message, sent bu D to AS */

	application = 17 /**< Application message, built on top of PHEMAP */
}
PHEMAP_Message_Type_t;


typedef struct
{
	PHEMAP_Link_t l_i;
	PHEMAP_Link_t v_1;
	PHEMAP_Link_t v_2;
}
PHEMAP_Init_Requ_t;

typedef struct
{
	PHEMAP_Link_t d_1;
	PHEMAP_Link_t d_2;
}
PHEMAP_Init_Reply_t;

typedef struct
{
	PHEMAP_Link_t l_2;
	PHEMAP_Link_t v_3;
}
PHEMAP_Init_Ack_t;

typedef struct
{
	PHEMAP_Link_t l_1;
	PHEMAP_Link_t body;
}
PHEMAP_Verification_Requ_t;

typedef struct
{
	PHEMAP_Link_t l_2;
}
PHEMAP_Verification_Ack_t;

typedef struct
{
	PHEMAP_Link_t l_i;
	PHEMAP_Device_ID_t device_ID;
	uint32_t carnet_length;
}
PHEMAP_Carnet_Requ_t;

typedef struct
{
	PHEMAP_Link_t l_i;
	PHEMAP_Carnet_t carnet;
}
PHEMAP_Carnet_Reply_t;

typedef struct
{
	PHEMAP_Link_t l_carlen;
}
PHEMAP_Carnet_Ack_t;

typedef struct
{
	PHEMAP_Link_t l_1;
	PHEMAP_Link_t l_2;
}
PHEMAP_Salt_Reply_t;

/**
 * @brief PHEMAP message.
 *
 * @details 
 * Communication between Devices, Gateways and the Authentication Service makes
 * use of PHEMAP messages. 
 */
typedef struct PHEMAP_PROTOCOL_MESSAGE
{
	PHEMAP_Message_Type_t type; /**< Message type */

	/**
	  * Depending on the message type, the meaning of this field may vary.
	  */
	union 
	{
		PHEMAP_Link_t link; /**< 
			A single chain link if the message is a verification request/reply 
			or a salted installation ack, 
			a challenge/response, if the message is an enrollment 
			request/reply,  */

		PHEMAP_Init_Requ_t init_req; /**<
			@f$\lbrace l_i; \; v_1; \; v_2 \rbrace = 
			\lbrace l_i; \;\; n \oplus \bigoplus_{j=0}^{S-3}l_{i+j+1}; \;\; l_{i+S-1} \oplus n \rbrace@f$,
		    if the message is an initialization request; */

		PHEMAP_Init_Reply_t init_reply; /**<
			@f\lbrace d_1; \; d_2 \rbrace = 
			\lbrace m \oplus \psi_{D}^{S}(l_i); \;\; m \oplus \psi_{D}^{S+1}(l_i) \rbrace@f$,
		   if the message is an initialization reply; */

		PHEMAP_Init_Ack_t init_ack; /**< 
			@f$\lbrace l_2; \; v_3 \rbrace = \lbrace l_2; m \oplus l_{i+S+2} \rbrace@f$,
			if the message is an initialization ack; */

		PHEMAP_Verification_Requ_t verify_req; /**< 
			@f$\lbrace l_1; \; l_2 \rbrace = \lbrace l_2; m \oplus l_{i+S+2} \rbrace@f$,
			if the message is an initialization ack; */ //TODO

		PHEMAP_Verification_Ack_t verify_ack; /**< 
			@f$\lbrace l_2; \; v_3 \rbrace = \lbrace l_2; m \oplus l_{i+S+2} \rbrace@f$,
			if the message is an initialization ack; */ //TODO

		PHEMAP_Carnet_Requ_t carnet_req; /**<
			@f$\lbrace G;l_i \rbrace@f$ if the message is a salt-installation
			request, or @f$\lbrace a_i; B, T \rbrace@f$, if the message is a 
			Babel-chain setup request;
			*/

		PHEMAP_Carnet_Reply_t carnet_reply; /**< A PHEMAP salted carnet
			@f$\lbrace D; X_{D,x_0,T} \rbrace@f$, or a babel-chain carnet
			@f$\lbrace \tau_{A,a_0,T,B,b_0,T} \rbrace@f$ */

		PHEMAP_Carnet_Ack_t carnet_ack; /**< 
			@f$\lbrace l_2; \; v_3 \rbrace = \lbrace l_2; m \oplus l_{i+S+2} \rbrace@f$,
			if the message is an initialization ack; */ //TODO

		PHEMAP_Salt_Reply_t salt_inst_reply; /**<
			@f$\lbrace l_{i+1}; l_{i+2} \oplus S_D \rbrace@f$, if the message is
			a salt-installation reply;*/

	} payload;
}
PHEMAP_Message_t;

#endif
