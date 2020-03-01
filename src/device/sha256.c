/**
 * @file sha256.c
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
#include "sha256.h"

// shift circolare di x a destra di y-bit. Per uint32_t.
#define circularRightShift32(x, y) \
	(((x)>>((y)&0x1F)) | ((x)<<(32-((y)&0x1F))))

// shift circolare di x a sinistra di y-bit. Per uint32_t.
#define circularLeftShift32(x, y) \
	(((x)<<((y)&0x1F)) | ((x)>>(32-((y)&0x1F))))

// rotazione "A" usata nel uint32_t-scheduling in SHA256
#define sha256RotateWsA(x) \
	(circularRightShift32(x, 7) ^ circularRightShift32(x, 18) ^ (x>>3))

// rotazione "B" usata nel uint32_t-scheduling in SHA256
#define sha256RotateWsB(x) \
	(circularRightShift32(x, 17) ^ circularRightShift32(x, 19) ^ (x>>10))

// rotazione "A" usata in un round SHA256
#define sha256RotateRA(x) \
	(circularRightShift32(x, 2) ^\
	 circularRightShift32(x, 13) ^\
	 circularRightShift32(x, 22))

// rotazione "B" usata in un round SHA256
#define sha256RotateRB(x) \
	(circularRightShift32(x, 6) ^\
	 circularRightShift32(x, 11) ^\
	 circularRightShift32(x, 25))

// funzione "Ch" usata in un round SHA256
#define Ch(a, b, c)	\
	((a & b)^(~a & c))

// funzione "Maj" usata in un round SHA256
#define Maj(a, b, c) \
	((a & b) ^ (a & c) ^ (b & c))


const uint64_t sha256K[80] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 
	0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 
	0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 
	0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 
	0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 
	0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 
	0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 
	0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 
	0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

void sha256WordScheduling(
		uint32_t inputBlock[16], 
		uint32_t uint32_ts[64]) 
{
	int i;
	for (i = 0; i < 16; i++)
		uint32_ts[i] = inputBlock[i];
	for (i=16; i<64; i++)
		uint32_ts[i] =	sha256RotateWsB(uint32_ts[i-2]) + 
						uint32_ts[i-7] + 
						sha256RotateWsA(uint32_ts[i-15]) + 
						uint32_ts[i-16];
}

void sha256Round(
		uint32_t buffer[8], 
		uint32_t Wt, 
		uint32_t Kt) 
{
	uint32_t T1 =	buffer[7] + 
					Ch(buffer[4], buffer[5], buffer[6]) + 
					sha256RotateRB(buffer[4]) + 
					Wt + 
					Kt;

	uint32_t T2 =	sha256RotateRA(buffer[0]) + 
					Maj(buffer[0], buffer[1], buffer[2]);
	
	buffer[7] = buffer[6];
	buffer[6] = buffer[5];
	buffer[5] = buffer[4];
	buffer[4] = buffer[3] + T1;
	buffer[3] = buffer[2];
	buffer[2] = buffer[1];
	buffer[1] = buffer[0];
	buffer[0] = T1 + T2;
}

void sha256Fbox(
		uint32_t inputBlock[16], 
		uint32_t chainingVariable[8], 
		uint32_t hashValue[8]) 
{
	uint32_t W[64];
	sha256WordScheduling(inputBlock, W);
	int i;
	for (i = 0; i < 8; i++)
		hashValue[i] = chainingVariable[i];
	for (i = 0; i < 64; i++) {
		sha256Round(hashValue, W[i], sha256K[i]);
	}
	for (i = 0; i < 8; i++)
		hashValue[i] += chainingVariable[i];
}

 void SHA256_ComputeHashsum(	
		const uint8_t * const	inputData, 
		uint64_t				size, 
		SHA256_Hashsum_t		hashValue)
{
	uint32_t chainingVariable[8] = 
	{
		0x6A09E667,
		0xBB67AE85,
		0x3C6EF372,
		0xA54FF53A,
		0x510E527F,
		0x9B05688C,
		0x1F83D9AB,
		0x5BE0CD19
	};

	uint64_t uint8_tOfPad, totalLength;
	if (size % 64 == 56) 
	{
		uint8_tOfPad = 64;
	}
	else
	{
		uint8_tOfPad = 56 - (size % 64);
	}

	totalLength = size + uint8_tOfPad + 8;
	
	// vettore tampone, riempito progressivamente con i dati di input/padding, 
	// usato per il calcolo dell'hash
	uint32_t data[16];	

	// puntatore usato per scorrere nel vettore tampone un uint8_t per volta
	uint8_t *ptr = (uint8_t*)data;	
	uint64_t i;
	int j;
	
	// scorre il vettore dei dati di input di quattro uint8_t in quattro 
	// uint8_t
	for (i = 3; i < totalLength; i+=4) {			
		// copia i uint8_t dal vettore di input nel vettore tampone oppure 
		// effettua il padding on the fly
		for (j = 0; j< 4; j++, ptr++) 
		{
			if ((i-j) < size)
			{
				*ptr = inputData[i-j];
			}
			else
			{
				if ((i-j) == size)
				{
					*ptr = 0x80;
				}
				else
				{
					*ptr = 0x00;
				}
			}
		}
	
		// completato il riempimento del vettore tampone, esso pue' essere
		// usato per il calcolo dell'hash
		if ((i+1)%64==0)
		{
			// se si sta per processare l'ultima partizione, nelle ultime due
			// uint32_t viene inserita la dimensione del messaggio originale, 
			// senza padding, in bit	
			if (i == totalLength-1) 
			{					
					data[14] = (uint32_t)(size>>29);
					data[15] = (uint32_t)(size<<3);
			}
			else
			{
				// il puntatore viene riportato all'inizio del vettore tampone
				ptr = (uint8_t*)data;
			}
	
			// calcolo dell'hash parziale
			sha256Fbox(data, chainingVariable, hashValue);
	
			// copia dell'hash parziale nella chaining-variable
			for (j = 0; j < 8; j++)
				chainingVariable[j] = hashValue[j];
		}
	}
}
