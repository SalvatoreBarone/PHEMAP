/**
 * @file carnet.h
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
#ifndef PHEMAP_CARNET_H
#define PHEMAP_CARNET_H

#include <inttypes.h>
#include "typedefs.h"

/**
 * @brief Absolute maximum carnet length
 */
#define ABSOLUTE_MAX_CARNET_LENGTH 16

/**
 * @brief PHEMAP carnet.
 */
typedef struct PHEMAP_CARNET
{
	uint32_t length; /**< Carnet length */
	uint32_t counter; /**< Used tickets */

	PHEMAP_Link_t tickets[ABSOLUTE_MAX_CARNET_LENGTH]; /**< 
		Tickets. Only the first length-1 are valid tickets */
} 
PHEMAP_Carnet_t;
#endif
