/**
 * @file phemap_as_enrollment.h
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

#ifndef PHEMAP_AS_ENROLLMENT_H
#define PHEMAP_AS_ENROLLMENT_H

#include <inttypes.h>

#include "phemap_chain.h"
#include "channel.h"

int32_t PHEMAP_GenerateChain(  
	channel_t * chnl,
	PHEMAP_Device_ID_t device_id,
	const int32_t maxChainLength,
	PHEMAP_Chain_t * const chain);


#endif

