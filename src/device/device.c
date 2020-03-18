/**
 * @file device.c
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

#include <stdlib.h>
#include <string.h>

#include "device.h"

#include "channel.h"
#include "transport.h"

#include "memxor.h"

/**
 * @brief Device initialization.
 *
 * @note This function is available only if the library is compiled using the
 * PHEMAP_ENROLLMENT compilation flag.
 *
 * @param [in]	device	
 * 				Pointer to the PHEMAP_Device_t instance which collect all
 * 				the device-related protocol information.
 *
 * @param [in]		cs
 * 					Pointer to the secret symbol.
 *
 * @param [in, out]	helper_data
 * 					Pointer to the memory buffer in which the helper data will
 * 					be placed.
 */
// void PHEMAP_Device_InitEnrollment(
// 		PHEMAP_Device_t * const device,
// 		const SecretSymbol_t cs,
// 		HelperData_t helper_data)
// {
// 	device->status = inactive;
// 	device->counter = 0;

// 	// Generazione della fingerprint
// 	FUZZYEX_GenerateFingerprint(cs, device->fingerprint, helper_data);

// 	// Generazione chiavi codifica RC5
// 	// Le chiavi vengono generate a partire dalla fingerprint del device.
// 	RC5_64RB_SubkeyGenerator(
// 			RC5_ENCRYPTION_ROUNDS, 
// 			RC5_KEY_LENGTH, 
// 			(const uint8_t * const) device->fingerprint, 
// 			device->subkeys);
// }

/**
 * @brief Device initialization.
 *
 * @details
 *
 * @param [in]	device	
 * 				Pointer to the PHEMAP_Device_t instance which collect all
 * 				the device-related protocol information.
 *
 * @param [in]		device_id
 * 					Unique device identifier. Each device has a unique 
 * 					idenfifier used by the AS in order to distinguish devices 
 * 					each other. Device fingerprint must be kept secret, so this 
 * 					implementation does not	make use of it to let the AS
 * 					distinguish devices.
 *
 * @param [in]		helper_data
 * 					Pointer to the memory buffer holding helper data.
 *
 * @return Upon successful completion, 0 shall be returned; otherwise, -1 shall 
 * be returned.
 */
int32_t PHEMAP_Device_Init(
		PHEMAP_Device_t * const device,
		PHEMAP_Device_ID_t device_id,
		HelperData_t helper_data)
{
	device->device_id = device_id;
	device->status = inactive;

	// Recovery della fingerprint
// 	FUZZYEX_RecoverFingerprint(helper_data, device->fingerprint);

// 	// Generazione chiavi codifica RC5
// 	// Le chiavi vengono generate a partire dalla fingerprint del device.
// 	RC5_64RB_SubkeyGenerator(
// 			RC5_ENCRYPTION_ROUNDS, 
// 			RC5_KEY_LENGTH, 
// 			(const uint8_t * const) device->fingerprint, 
// 			device->subkeys);

	int32_t status = 0;

	/* initialize the transport function of the channel */
    ch_phy_fun_set(&device->chnl.function,INIT_F,&udp_server_init);
    ch_phy_fun_set(&device->chnl.function,DEINIT_F,&udp_server_deinit);
    ch_phy_fun_set(&device->chnl.function,DATA_SEND_F,&udp_data_send);
    ch_phy_fun_set(&device->chnl.function,DATA_RECV_F,&udp_data_recv);

    /* initialize the queue of the channel */
    int msg_max_num = 10;           //maybe change with a define
    int msg_len = sizeof(ch_msg_t);
    ch_phy_que_set(&device->chnl.queue, msg_max_num, msg_len);

	// //---------------------------------------------------------------------------------
    // //questo va fuori da questa funzione per essere indipendente dal lev trasporto
	// /* creating the structure for the transport level */
    // udp_info_t udp_cfg;
    // const char *addr[] = {"172.17.0.2",
    //                     "172.17.0.3",
    //                     "172.17.0.4",
    //                     "172.17.0.5",
    //                     "172.17.0.6"};
    // udp_cfg.addr = addr;                        /*TODO check this assignement */
    // device->chnl.cfg.cose = &udp_cfg;
	// //----------------------------------------------------------------------------------
    /* initialize the transport */ 
    ch_init(&device->chnl);


	return status;
}

/**
 * @brief Device initialization.
 *
 * @details
 *
 * @param [in]	device	
 * 				Pointer to the PHEMAP_Device_t instance which collect all
 * 				the device-related protocol information.
 *
 * @param [in]		device_id
 * 					Unique device identifier. Each device has a unique 
 * 					idenfifier used by the AS in order to distinguish devices 
 * 					each other. Device fingerprint must be kept secret, so this 
 * 					implementation does not	make use of it to let the AS
 * 					distinguish devices.
 *
 * @param [in]		helper_data
 * 					Pointer to the memory buffer holding helper data.
 *
 * @return Upon successful completion, 0 shall be returned; otherwise, -1 shall 
 * be returned.
 */
int32_t PHEMAP_Device_Deinit(
		PHEMAP_Device_t * const device)
{
	int32_t status = 0;

	/* deinitialize the transport */ 
    ch_deinit(&device->chnl);

	/* clear all the data */
	memset(device, 0, sizeof(PHEMAP_Device_t));

	return status;
}

int32_t PHEMAP_DEV_linkGen(
		PHEMAP_Device_t * const device,
		HelperData_t helper_data,
		PHEMAP_Link_t * const challenge,
		PHEMAP_Link_t * response)
{
	uint8_t * seed_ptr = (uint8_t*) challenge;
	uint8_t * tmp_ptr = (uint8_t*) response;
	for (size_t k = 0; k < sizeof(PHEMAP_Link_t); k++){
		srand(seed_ptr[k]);
		tmp_ptr[k] = rand();
	}
}


