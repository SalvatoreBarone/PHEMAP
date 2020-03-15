/**
 * @file phemap_phi.c
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
#include "phemap_dev_phi.h"

/**
 * @brief Get the next i-th non-sentinel link on the current chain.
 *
 * @details
 * Let us assume that D and AS are synchronized on the link 
 * @f$ l_{k} \in \gamma_{D,l0,M} @f$ after one of the PHEMAP functions - this 
 * means that @f$ l{k} @f$ is stored in Q - this function computes 
 * @f$ \phi_{D}^{i}(Q) = l_{k+i} @f$.
 *
 * @note
 * A call to this function DOES NOT alter the value of the Q secure register.
 *
 * @param [in]	device	
 * 				Pointer to the PHEMAP_Device_t instance which collect all
 * 				the device-related protocol information.
 *
 * @param [in]	i
 * 				Index of the requested link on the current chain.
 *
 * @param [in]	link
 * 				Address of the memory buffer in which the @f$ l_{k+i}i @f$ link 
 * 				on the chain will be placed.
 *
 * @note If the requested i-th link - @f$ l_{k+i} @f$ - is a sentinel link, the 
 * function will return @f$ l_{k+i+1} @f$.
 * 					
 * @return The function returns the index of the closest non-sentinel link to
 * the requested one. It is equal to the @ref i parameter, but if the requested 
 * i-th link - @f$ l_{k+i} @f$ - is a sentinel link, the function will return 
 * i+1.
 */
uint32_t PHEMAP_Device_peekLink(
		PHEMAP_Device_t * const device,			//necessario per la PUF
		uint32_t i,
		uint8_t senti_flag,
		PHEMAP_Link_t * const link)				//challenge e response
{
	uint32_t counter = device->counter;
	for (uint32_t k = 0; k < i; k++)
	{
		if (senti_flag != RET_SENTINEL) //non voglio le sentinelle 
		{
			if ((counter % SENTINEL) == 0)
			{
				k--;
				counter++;
			}
		}
		
		// RC5_64RB_Encrypt(
		// 		RC5_ENCRYPTION_ROUNDS, 
		// 		device->subkeys, 
		// 		link, 
		// 		link);
		PHEMAP_DEV_linkGen(device, 0, link, link);
		printf("genero il link %d-th\n",k);			//DELME

	}

	return 0;
}

uint32_t PHEMAP_Device_getNextLink(
		PHEMAP_Device_t * const device,			//necessario per la PUF
		uint8_t senti_flag,
		PHEMAP_Link_t * const link)				//contiene il successivo link nella chain
{
	if (senti_flag != RET_SENTINEL) //non voglio le sentinelle 
		{
			if ((device->counter % SENTINEL) == 0)
			{
				// RC5_64RB_Encrypt(
				// 		RC5_ENCRYPTION_ROUNDS, 
				// 		device->subkeys, 
				// 		link, 
				// 		link);
				PHEMAP_DEV_linkGen(device, 0, &device->Q, &device->Q);
				device->counter++;
			}
		}

	// RC5_64RB_Encrypt(
	// 		RC5_ENCRYPTION_ROUNDS, 
	// 		device->subkeys, 
	// 		link, 
	// 		link);	
	PHEMAP_DEV_linkGen(device, 0, &device->Q, &device->Q);
	device->counter++;

	memcpy(link, &device->Q,sizeof(PHEMAP_Link_t));
	printf("genero il link\n");	

	return 0;
}

/**
 * @brief Update Q with the i-th non-sentinel link on the current chain.
 *
 * @details
 * Let us assume that D and AS are synchronized on the link 
 * @f$ l_{k} \in \gamma_{D,l0,M} @f$ after one of the PHEMAP functions - this 
 * means that @f$ l{k} @f$ is stored in Q - this function computes 
 * @f$ Q \gets \phi_{D}^{i}(Q) = l_{k+i} @f$.
 *
 * @note
 * A call to this function DOES alter the value of the Q secure register.
 *
 * @param [in]	device	
 * 				Pointer to the PHEMAP_Device_t instance which collect all
 * 				the device-related protocol information.
 *
 * @param [in]	i
 * 				Index of the requested link on the current chain.
 *
 * @note If the requested i-th link - @f$ l_{k+i} @f$ - is a sentinel link, the 
 * Q register will be updated with @f$ l_{k+i+1} @f$.
 * 					
 * @return The function returns the index of the closest non-sentinel link to
 * the requested one. It is equal to the @ref i parameter, but if the requested 
 * i-th link - @f$ l_{k+i} @f$ - is a sentinel link, the function will return 
 * i+1.
 */
uint32_t PHEMAP_Device_updateQ(
		PHEMAP_Device_t * const device,
		uint32_t i)
{

	if (0 == ((device->counter + i) % SENTINEL))
		i++;

	device->counter+=i;

	for (uint32_t k = 0; k < i; k++)
		RC5_64RB_Encrypt(
				RC5_ENCRYPTION_ROUNDS, 
				device->subkeys, 
				device->Q, 
				device->Q);

	return i;
}
