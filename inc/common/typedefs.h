/**
 * @file d_config.h
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
#ifndef PHEMAP_TYPEDEFS_H
#define PHEMAP_TYPEDEFS_H

#include <inttypes.h>
#include "device/fuzzy.h"

/**
 * @brief Device ID typedef.
 */
typedef uint64_t PHEMAP_Device_ID_t;


/**
 * @brief PHEMAP Device fingerprint typedef
 */
typedef SHA256_Hashsum_t PHEMAP_Device_Fingerprint_t;

/**
 * @brief PHEMAP Chain link typedef
 *
 * @note The struct definition allows PHEMAP_Device_getFollowingLink and
 * PHEMAP_Device_getNextLink to have a link as return type. 
 */
typedef RC5_64RB_DataBlock_t PHEMAP_Link_t;

#endif

