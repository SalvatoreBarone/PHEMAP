/**
 * @file fuzzy.h
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

#ifndef FUZZYEXTRACTOR_H
#define FUZZYEXTRACTOR_H

#include <inttypes.h>
#include "d_config.h"
#include "sha256.h"
#include "rc5.h"

/**
 * @defgroup fuzzy Fuzzy-Extractor
 * @{
 * @brief Fuzzy extractor schema implementation
 *
 * @details
 *
 */

/**
 * @brief ECC word-codeword size ratio
 */
#define SRAMPUF_ECC_CW_R 16

/**
 * @brief Secret symbol size, in byte
 */
#define SRAMPUF_SECSIZE (SRAMPUF_SIZE/SRAMPUF_ECC_CW_R)

/**
 * @brief Data structure for the helper data.
 */
typedef uint8_t HelperData_t[SRAMPUF_SIZE];

/**
 * Data structure for the secret symbol to be used during the enrollment.
 */
typedef uint8_t SecretSymbol_t[SRAMPUF_SECSIZE];

void FUZZYEX_GenerateFingerprint (
		const SecretSymbol_t cs,
		SHA256_Hashsum_t fingerprint,
		HelperData_t helperData);

void FUZZYEX_RecoverFingerprint (
		const HelperData_t helperData,
		SHA256_Hashsum_t fingerprint);

#endif
/**
 * @}
 */
