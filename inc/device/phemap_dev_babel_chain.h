/**
 * @file phemap_babel_chain.h
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
#ifndef PHEMAP_DEV_BabelChain_H
#define PHEMAP_DEV_BabelChain_H

#include "device.h"

int32_t PHEMAP_Device_BCconnectToDev(
		PHEMAP_Device_t * const device,
		PHEMAP_Device_ID_t dest_ID,
		uint32_t max_messages);

int32_t PHEMAP_Device_BCNotifyRecv(
		PHEMAP_Device_t * const device);

// int32_t PHEMAP_Device_closePeerConnection(PHEMAP_Device_t * const device);

// ssize_t PHEMAP_Device_sendToPeer(
// 		PHEMAP_Device_t * const device,
// 		const uint8_t * const data,
// 		size_t size);

// ssize_t PHEMAP_Device_recvFromPeer(
// 		PHEMAP_Device_t * const device,
// 		uint8_t * const data,
// 		size_t size);

#endif
