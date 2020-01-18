/**
 * @file rc5.h
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

#ifndef RC5_HEADER_H
#define RC5_HEADER_H

#include <inttypes.h>

/**
 * @defgroup encrypt Encryption
 * @{
 * @brief RC5 encryption algorithm implementation
 *
 * @details
 *
 */

/**
 * @brief RC5 32/R/B word data type
 */
typedef uint32_t RC5_32RB_Word_t;

/**
 * @brief RC5 32/R/B word data type
 */
typedef uint64_t RC5_64RB_Word_t;

/**
 * @brief RC5 32/R/B data block
 *
 * @details
 * RC5 32/R/B encripts two 32-bits long words
 */
typedef RC5_32RB_Word_t RC5_32RB_DataBlock_t[2];

/**
 * @brief RC5 64/R/B data block
 *
 * @details
 * RC5 64/R/B encripts two 64-bits long words
 */
typedef RC5_64RB_Word_t RC5_64RB_DataBlock_t[2];

void RC5_32RB_SubkeyGenerator(
		uint8_t	R, 
		uint8_t	B, 
		const uint8_t * const key, 
		RC5_32RB_Word_t * const subkeys); 

void RC5_32RB_Encrypt(
		uint8_t R, 
		const RC5_32RB_Word_t * const subkeys, 
		const RC5_32RB_DataBlock_t plaintext, 
		RC5_32RB_DataBlock_t ciphertext);

void RC5_32RB_Decrypt(
		uint8_t R, 
		const RC5_32RB_Word_t * const subkeys, 
		const RC5_32RB_DataBlock_t ciphertext,
		RC5_32RB_DataBlock_t plaintext);


void RC5_64RB_SubkeyGenerator(
		uint8_t	R, 
		uint8_t	B, 
		const uint8_t * const key, 
		RC5_64RB_Word_t * const subkeys); 

void RC5_64RB_Encrypt(
		uint8_t R, 
		const RC5_64RB_Word_t * const subkeys, 
		const RC5_64RB_DataBlock_t plaintext, 
		RC5_64RB_DataBlock_t ciphertext);

void RC5_64RB_Decrypt(
		uint8_t R, 
		const RC5_64RB_Word_t * const subkeys, 
		const RC5_64RB_DataBlock_t ciphertext,
		RC5_64RB_DataBlock_t plaintext);

#endif

/**
 * @}
 */
