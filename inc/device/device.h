/**
 * @file device.h
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

#ifndef PHEMAP_DEVICE_H
#define PHEMAP_DEVICE_H

#include <inttypes.h>
#include <stdlib.h>

#include "d_config.h"

#include "typedefs.h"
#include "phemap_carnet.h"
#include "channel.h"

#include "transport.h"

/**
 * @ingroup phemap
 * @{
 * @defgroup phemap-device PHEMAP: device side
 * @{
 * @brief Device-side implementation of the PHEMAP protocol
 *
 * @details
 *
 */

/**
 * @brief PHEMAP device status.
 */
typedef enum PHEMAP_DEVICE_STATUS
{
	inactive, /**< The device is inactive, not initialized. This  means it 
				   cannot participate to any mutual authentication operation. */

	initialization,	/**< Unstable state reached when an initialization request 
					  	 is received from the authentication service. */

	initialized, /**< The device has been correctly initialized and 
				      authenticated from the authentication service */

	salt_installation, /**< Unstable state reached when a request for a salt 
							installation phase is sent to the authentication 
							service. */

	salted,	/**< When the device operates in salted mode, performing the 
				Salted-PHEMAP mutual authentication protocol, it is in this 
				 state. */

	retry_sy, /**< Retry Salt installation phase after re-initialization */

	babel_installation,	/**< Unstable state reached when a Babel-chain PHEMAP 
						     setup request is received by the device */ 

	babel_chain, /**< When the PHEMAP Babel-chain protocol has been set up 
				      correctly, the device is in this state. */

	retry_bs /**< Retry Babelchain setup procedure after re-initialization */
} 
PHEMAP_Device_Status_t;


typedef struct PHEMAP_BC_DEVICE
{
	PHEMAP_Device_ID_t dev_id; /**< Each device has a unique idenfifier
		used by the AS in order to distinguish devices each other. Device
		fingerprint must be kept secret, so this implementation does not
   		make use of it to distinguish devices.  */

	PHEMAP_Link_t Q; /**< Pointer to the secure register Q. It holds the last
						computed link on the chain. 
						@note This memory area must ne inside a secure
						perimeter.*/

	PHEMAP_Carnet_t carnet; /**< Babel-chain PHEMAP carnet used to communicate
								with a peer device */
} 
PHEMAP_Bcdev_t;

typedef struct PHEMAP_DEVICE
{
	PHEMAP_Device_ID_t device_id; /**< Each device has a unique idenfifier
		used by the AS in order to distinguish devices each other. Device
		fingerprint must be kept secret, so this implementation does not
   		make use of it to distinguish devices.  */

	PHEMAP_Device_Fingerprint_t fingerprint; /**< Device fingerprint */

	RC5_64RB_Word_t subkeys[(RC5_ENCRYPTION_ROUNDS + 1) << 1]; /**<
		Subkeys used by the RC5 encryption alghorithm */

	PHEMAP_Device_Status_t status; /**< PHEMAP protocol device status */

	uint32_t counter;	/**< Link counter */

	PHEMAP_Link_t Q; /**< Pointer to the secure register Q. It holds the last
						computed link on the chain. 
						@note This memory area must ne inside a secure
						perimeter.*/

	channel_t chnl;	/**< Channel used to communicate with other device */

	PHEMAP_Bcdev_t BC_device[BC_DEV_NUM]; /**< Babel-chain PHEMAP carnet used to communicate
								with a peer device */

	// client_t gateway; /**< TCP client to communicate with the reference
	// 					gateway */

} 
PHEMAP_Device_t;

// void PHEMAP_Device_InitEnrollment(
// 		PHEMAP_Device_t * const device,
// 		const SecretSymbol_t cs,
// 		HelperData_t helper_data);

int32_t PHEMAP_Device_Init(
		PHEMAP_Device_t * const device,
		PHEMAP_Device_ID_t device_id,
		HelperData_t helper_data);

int32_t PHEMAP_Device_Deinit(
		PHEMAP_Device_t * const device);

int32_t PHEMAP_DEV_linkGen(
		PHEMAP_Device_t * const device,
		HelperData_t helper_data,
		PHEMAP_Link_t * const challenge,
		PHEMAP_Link_t * response);


#endif
