/**
 * @file rm17.c
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

#include "rm17.h"
#include <string.h>
#include <stdio.h>

#define testBit(row, n) (((row[15-(n)/8] & (((uint8_t)1)<<((n)%8)))==0) ? 0 : 1)

#define clearBit(var, n) vari &= ~(((uint8_t)1)<<(n))

#define setBit(var, n) var |=(((uint8_t)1)<<(n))

const RM17_Codeword_t genMatrix[8] = {
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
	{0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55},
	{0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33},
	{0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f},
	{0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff},
	{0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff},
	{0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}
};

static void RM17_CodewordShift(
		const RM17_Codeword_t input, 
		RM17_Codeword_t output,
		uint8_t nbit);

static uint8_t RM17_Codeword1Count(const RM17_Codeword_t codeword);

void RM17_Encode(const RM17_Word_t byte, RM17_Codeword_t codeword)
{
	int i, j;
	for (i=0; i < 16; i++)
		codeword[i]=((byte&0x80)==0 ? 0 : genMatrix[0][i]);
	for (i=6; i!=-1; i--)
		if ((byte&(1<<i))!=0)
			for (j=0; j<16; j++)
				codeword[j]^=genMatrix[7-i][j];
}

RM17_Word_t RM17_Decode(const RM17_Codeword_t codeword)
{
	RM17_Word_t word = 0;
	RM17_Codeword_t shifted[9]; 
	memcpy(shifted[0], codeword, sizeof(RM17_Codeword_t));
	memcpy(shifted[8], codeword, sizeof(RM17_Codeword_t));

//	printf("\ni=0\n\t");
//	for (uint8_t j = 0; j < sizeof(RM17_Codeword_t); j++)
//		printf("%02x ", shifted[0][j]);


	for (int i = 1; i < 8; i++)
	{
		RM17_CodewordShift(shifted[0], shifted[i], (1<<(i-1)));
		RM17_Codeword_t xored;
		for (uint8_t j = 0; j < sizeof(RM17_Codeword_t); j++)
			xored[j] = (shifted[i][j] ^ shifted[0][j]) & ~genMatrix[i][j];

//		printf("\ni=%d\nsh=%d\ns0=\t", i, (1<<(i-1)));
//		for (uint8_t j = 0; j < sizeof(RM17_Codeword_t); j++)
//			printf("%02x ", shifted[0][j]);
//		printf("\ns=\t");
//		for (uint8_t j = 0; j < sizeof(RM17_Codeword_t); j++)
//			printf("%02x ", shifted[i][j]);
//		printf("\n\t");
//		for (uint8_t j = 0; j < sizeof(RM17_Codeword_t); j++)
//			printf("%02x ", shifted[i][j] ^ shifted[0][j]);
//		printf("\ng=\t");
//		for (uint8_t j = 0; j < sizeof(RM17_Codeword_t); j++)
//			printf("%02x ", (uint8_t)~genMatrix[i][j]);
//		printf("\nx=\t");
//		for (uint8_t j = 0; j < sizeof(RM17_Codeword_t); j++)
//			printf("%02x ", xored[j]);

		uint8_t one_count = RM17_Codeword1Count(xored);
		if (one_count > (1<<5))
		{
			word |= (1<<(7-i));
			for (uint8_t j = 0; j < sizeof(RM17_Codeword_t); j++)
				shifted[8][j] ^= genMatrix[i][j];
		}

//		printf("\none_count=%d, word=%02x\n", one_count, word);
	}

	uint8_t one_count = RM17_Codeword1Count(shifted[8]);
	if (one_count > (1<<6))
		word |= 0x80;

//	printf("\n\ns7=\t");
//	for (uint8_t j = 0; j < sizeof(RM17_Codeword_t); j++)
//		printf("%02x ", shifted[7][j]);
//	printf("\none_count=%d, word=%02x\n", one_count, word);

	return word;
}


static void RM17_CodewordShift(
		const RM17_Codeword_t input, 
		RM17_Codeword_t output,
		uint8_t nbit)
{
	// nbit is always a power of 2
	if (nbit < 8)
	{
		uint8_t lsb = input[0] >> (8-nbit);
		uint8_t last = sizeof(RM17_Codeword_t) - 1;
		for (uint8_t i = 0; i < last; i++)
			output[i] = ((input[i] << nbit) | (input[i+1] >> (8-nbit)));
		output[last] = ((input[last] << nbit) | lsb);
	}
	else
	{
		uint8_t nbyte = nbit >> 3;
		memcpy(output, &input[nbyte], sizeof(RM17_Codeword_t) - nbyte);
		memcpy(&output[sizeof(RM17_Codeword_t) - nbyte], input, nbyte);
	}
}

static uint8_t RM17_Codeword1Count(const RM17_Codeword_t codeword)
{	
	uint8_t one_count = 0;
	for (uint8_t i = 0; i < sizeof(RM17_Codeword_t); i++)
		for (uint8_t j = 0; j < 8; j++)
			if ((codeword[i] & (1<<j)) != 0)
				one_count++;
	return one_count;
}
