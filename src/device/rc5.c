/**
 * @file rc5.c
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

#include "rc5.h"
#include <string.h>

static const RC5_32RB_Word_t P32 = 0xB7E15163;
static const RC5_32RB_Word_t Q32 = 0x9E3779B9;
static const RC5_64RB_Word_t P64 = 0xb7e151628ead2a6b;
static const RC5_64RB_Word_t Q64 = 0x9e3779b97f4a7c15;

// Right rotate x (which is w bit long) of y bit
#define rotR(x, y, w) 	(((x)>>((y)&((w)-1))) | ((x)<<((w)-((y)&((w)-1)))))

// Right rotate x (which is w bit long) of y bit
#define rotL(x, y, w) 	(((x)<<((y)&((w)-1))) | ((x)>>((w)-((y)&((w)-1)))))

/**
 * @brief RC5-32/R/B subkeys generator.
 * 
 * @param [in]	R	
 *				number of encription rounds; it affects the number of subkeys
 *				to be generated, which is @$2(N+1)@$;
 *
 * @param [in]	B
 *				sizeof the key, in byte;
 *
 * @param [in]	key
 *				byte array containing the key to be used;
 *
 * @param [out]	subkeys	
 *				memory buffer where the subkeys will be placed; this buffer 
 *				must be pre-allocated and its size must be greater or equal
 *				to @$8(N+1)@$ byte (because each key is 4 byte long).
 */
void RC5_32RB_SubkeyGenerator(
		uint8_t	R, 
		uint8_t	B, 
		const uint8_t * const key, 
		RC5_32RB_Word_t * const subkeys)
{
	const int32_t 	
	// u e' la lunghezza di una word in byte
	u = sizeof(RC5_32RB_Word_t),
	// w e' la lunghezza di una word in bit
	w = (u << 3),
	// c e' il numero di words di L usati
	c = (B / u) + ((B & (u-1)) == 0 ? 0 : 1),
	// t e' il numero di entry del vettore subkeys
	t = (R + 1) << 1,
	// n e' il numero di mix delle sottochiavi
	n = 3 * (t > c ? t : c);

	RC5_32RB_Word_t L[256 / u];
	memset(L, 0, 256);
	
	int32_t i, j, k;

	// copio la chiave nel vettore L
	for (i = B - 1, L[c - 1] = 0; i != -1; i--)
		L[i/u] = rotL(L[i/u], 8, w) + key[i];

	// inizializzo la tabella delle chiavi
	subkeys[0] = P32;
	for (i = 1; i < t; i++)
		subkeys[i] = subkeys[i - 1] + Q32;

	// mixing delle chiavi
	RC5_32RB_Word_t a = 0, b = 0;
	for (k = i = j = 0; k < n; k++, i = (i + 1) % t, j = (j + 1) % c) 
	{
		a = subkeys[i] = rotL(subkeys[i] + a + b, 3, w);
		b = L[j] = rotL(L[j] + a + b, a + b, w);
	}
}

/**
 * @brief RC5-32/R/B encryption
 * 
 * @param [in]	R
 *				number of encription rounds; it affects the number of subkeys
 *				required, which is @$2(N+1)@$.
 *
 * @param [in]	subkeys
 *				subkeys to be used during the encryption process. They must be
 *				generated using the RC5_32RB_SubkeyGenerator() function.
 *
 * @param [in]	plaintext
 *				data to be encrypted.
 *
 * @param [out]	ciphertext
 *				encrypted data.
 */
void RC5_32RB_Encrypt(
		uint8_t R, 
		const RC5_32RB_Word_t * const subkeys, 
		const RC5_32RB_DataBlock_t plaintext, 
		RC5_32RB_DataBlock_t ciphertext)
{
	uint8_t				w = (sizeof(RC5_32RB_Word_t) << 3);
	RC5_32RB_Word_t 	A = plaintext[0] + subkeys[0],
						B = plaintext[1] + subkeys[1];
	
	for (uint8_t i = 1; i <= R; i++) 
	{
		A = rotL(( A ^ B), B, w)+subkeys[ i << 1];
		B = rotL(( A ^ B), A, w)+subkeys[(i << 1) +1];
	}
	
	ciphertext[0] = A;
	ciphertext[1] = B;
}

/**
 * @brief RC5-32/R/B Decrypt
 *
 * @param [in]	R
 *				number of encription rounds; it affects the number of subkeys
 *				required, which is @$2(N+1)@$.
 *
 * @param [in]	subkeys
 *				subkeys to be used during the encryption process. They must be
 *				generated using the RC5_32RB_SubkeyGenerator() function.
 *
 * @param [in]	ciphertext
 *				data to be dencrypted.
 *
 * @param [out]	plaintext
 *				dencrypted data.
 */
void RC5_32RB_Decrypt(
		uint8_t R, 
		const RC5_32RB_Word_t * const subkeys, 
		const RC5_32RB_DataBlock_t ciphertext,
		RC5_32RB_DataBlock_t plaintext)
{
	uint8_t			w = (sizeof(RC5_32RB_Word_t) << 3);
	RC5_32RB_Word_t	A = ciphertext[0],
					B = ciphertext[1];

	for (uint8_t i = R; i > 0; i--) 
	{
		B = rotR((B - subkeys[(i<<1) + 1]), A, w) ^ A;
		A = rotR((A - subkeys[i<<1]), B, w) ^ B;
	}

	plaintext[1] = B - subkeys[1];
	plaintext[0] = A - subkeys[0];
}

/**
 * @brief RC5-64/R/B subkeys generator.
 * 
 * @param [in]	R	
 *				number of encription rounds; it affects the number of subkeys
 *				to be generated, which is @$2(N+1)@$;
 *
 * @param [in]	B
 *				sizeof the key, in byte;
 *
 * @param [in]	key
 *				byte array containing the key to be used;
 *
 * @param [out]	subkeys	
 *				memory buffer where the subkeys will be placed; this buffer 
 *				must be pre-allocated and its size must be greater or equal
 *				to @$16(N+1)@$ byte (because each key is 8 byte long).
 */
void RC5_64RB_SubkeyGenerator(
		uint8_t	R, 
		uint8_t	B, 
		const uint8_t * const key, 
		RC5_64RB_Word_t * const subkeys)
{
	const int32_t 	
	// u e' la lunghezza di una word in byte
	u = sizeof(RC5_64RB_Word_t),
	// w e' la lunghezza di una word in bit
	w = (u << 3),
	// c e' il numero di words di L usati
	c = (B / u) + ((B & (u-1)) == 0 ? 0 : 1),
	// t e' il numero di entry del vettore subkeys
	t = (R + 1) << 1,
	// n e' il numero di mix delle sottochiavi
	n = 3 * (t > c ? t : c);

	RC5_64RB_Word_t L[256 / u];
	memset(L, 0, 256);
	
	int32_t i, j, k;

	// copio la chiave nel vettore L
	for (i = B - 1, L[c - 1] = 0; i != -1; i--)
		L[i/u] = rotL(L[i/u], 8, w) + key[i];

	// inizializzo la tabella delle chiavi
	subkeys[0] = P64;
	for (i = 1; i < t; i++)
		subkeys[i] = subkeys[i - 1] + Q64;

	// mixing delle chiavi
	RC5_64RB_Word_t a = 0, b = 0;
	for (k = i = j = 0; k < n; k++, i = (i + 1) % t, j = (j + 1) % c) 
	{
		a = subkeys[i] = rotL(subkeys[i] + a + b, 3, w);
		b = L[j] = rotL(L[j] + a + b, a + b, w);
	}
}

/**
 * @brief RC5-64/R/B encryption
 * 
 * @param [in]	R
 *				number of encription rounds; it affects the number of subkeys
 *				required, which is @$2(N+1)@$.
 *
 * @param [in]	subkeys
 *				subkeys to be used during the encryption process. They must be
 *				generated using the RC5_64RB_SubkeyGenerator() function.
 *
 * @param [in]	plaintext
 *				data to be encrypted.
 *
 * @param [out]	ciphertext
 *				encrypted data.
 */
void RC5_64RB_Encrypt(
		uint8_t R, 
		const RC5_64RB_Word_t * const subkeys, 
		const RC5_64RB_DataBlock_t plaintext, 
		RC5_64RB_DataBlock_t ciphertext)
{
	uint8_t				w = (sizeof(RC5_64RB_Word_t) << 3);
	RC5_64RB_Word_t 	A = plaintext[0] + subkeys[0],
						B = plaintext[1] + subkeys[1];
	
	for (uint8_t i = 1; i <= R; i++) 
	{
		A = rotL(( A ^ B), B, w)+subkeys[ i << 1];
		B = rotL(( A ^ B), A, w)+subkeys[(i << 1) +1];
	}
	
	ciphertext[0] = A;
	ciphertext[1] = B;
}

/**
 * @brief RC5-64/R/B Decrypt
 *
 * @param [in]	R
 *				number of encription rounds; it affects the number of subkeys
 *				required, which is @$2(N+1)@$.
 *
 * @param [in]	subkeys
 *				subkeys to be used during the encryption process. They must be
 *				generated using the RC5_64RB_SubkeyGenerator() function.
 *
 * @param [in]	ciphertext
 *				data to be dencrypted.
 *
 * @param [out]	plaintext
 *				dencrypted data.
 */
void RC5_64RB_Decrypt(
		uint8_t R, 
		const RC5_64RB_Word_t * const subkeys, 
		const RC5_64RB_DataBlock_t ciphertext,
		RC5_64RB_DataBlock_t plaintext)
{
	uint8_t			w = (sizeof(RC5_64RB_Word_t) << 3);
	RC5_64RB_Word_t	A = ciphertext[0],
					B = ciphertext[1];

	for (uint8_t i = R; i > 0; i--) 
	{
		B = rotR((B - subkeys[(i<<1) + 1]), A, w) ^ A;
		A = rotR((A - subkeys[i<<1]), B, w) ^ B;
	}

	plaintext[1] = B - subkeys[1];
	plaintext[0] = A - subkeys[0];
}
