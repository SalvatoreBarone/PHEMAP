/**
 * @file phemap_chain.h
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

#ifndef PHEMAP_CHAIN_H
#define PHEMAP_CHAIN_H

#include <inttypes.h>
#include "as_config.h"
#include "as.h"
#include "typedefs.h"

#define RET_SENTINEL 0					//vedere se questo è il posto giusto
#define RET_NO_SENTINEL 1

typedef struct
{
	int32_t length; 								/**< Actual length of the chain */

	int32_t current;								/**< Index of the currently in use link on the chain */

	PHEMAP_Link_t links[ABSOLUTE_MAX_CHAIN_LENGTH]; /**< Chain */
}
PHEMAP_Chain_t;

int32_t PHEMAP_Chain_createDatabase(
		const char * const databasename);

// int32_t PHEMAP_Chain_load(
// 		const char * const databasename,
// 		PHEMAP_Device_ID_t device_id,
// 		uint32_t chain_id,
// 		PHEMAP_Chain_t * const chain);
int32_t PHEMAP_Chain_getNextLink(
		PHEMAP_AS_t * const as,
		PHEMAP_Device_ID_t device_id,
		uint32_t chain_id,
		PHEMAP_Link_t * const link);

int32_t PHEMAP_Chain_peekLink(
		PHEMAP_AS_t * const as,
		PHEMAP_Device_ID_t device_id,
		uint32_t chain_id,
		uint32_t chain_len,
		uint8_t sentinel,
		PHEMAP_Chain_t * const chain);

int32_t PHEMAP_Chain_store(
		const char * const databasename,
		PHEMAP_Device_ID_t device_id,
		uint32_t chain_id,
		const PHEMAP_Chain_t * const chain);

int32_t PHEMAP_Chain_update(
		const char * const databasename,
		PHEMAP_Device_ID_t device_id,
		uint32_t chain_id,
		PHEMAP_Chain_t * const chain);

#endif
