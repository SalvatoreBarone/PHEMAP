/**
 * @file rm17.h
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

#ifndef REED_MULLER_ECC_H
#define REED_MULLER_ECC_H

#include <inttypes.h>

/**
 * @defgroup ecc ECC
 * @{
 * @brief Reed-Muller Error Correcting Codes implementaion
 *
 * @details
 *
 */

typedef uint8_t RM17_Word_t;
typedef uint8_t RM17_Codeword_t[16];

/**
 * @brief RM(1,7) encoding.
 *
 * @param [in]	word		
 *				word to be encoded (8 bit)
 *
 * @param [out]	codeword
 *				buffer in which the encoded word will be placed.
 */
void RM17_Encode(
		const RM17_Word_t word, 
		RM17_Codeword_t codeword);

/**
 * @brief RM(1,7) decoding.
 *
 * @param [in]	codeword	
 *				Codeword to be decoded 
 *
 * @return The decoded 8-bit long word.
 *
 */
RM17_Word_t RM17_Decode(const RM17_Codeword_t codeword);

#endif

/**
 * @}
 */
