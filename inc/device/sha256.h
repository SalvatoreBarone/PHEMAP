/**
 * @file sha256.h
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

#ifndef SHA256_H
#define SHA256_H

#include <inttypes.h>
#include <string.h>

/**
 * @defgroup hashing Hashing
 * @{
 * @brief SHA-256 algorithm implementation
 *
 * @details
 *
 */

/**
 * @brief SHA-256 hashsum data type.
 *
 * @details 
 * It is a 32 byte array, organized in eight words, each of which is four byte 
 * long.
 */
typedef uint32_t SHA256_Hashsum_t[8]; 


/**
 * @brief Calcola l'hashsum SHA-256
 *
 * @param[in]	inputData
 *				Input data.
 *
 * @param[in]	size
 *				Size, in byte, of the input data.
 *
 * @param[out]	hashValue		
 *				Memory area in which the computed hashsum will be placed.
 *
 */
 void SHA256_ComputeHashsum(	
		const uint8_t * const	inputData, 
		uint64_t				size, 
		SHA256_Hashsum_t		hashValue);

/**
 * @brief Compare two hashsum each other
 *
 * @retval 0 if they are equal.
 */
#define SHA256_Compare(hash1, hash2) \
	memcmp(hash1,hash2, 32)


#endif

/**
 * @}
 */
