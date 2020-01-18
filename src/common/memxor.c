/**
 * @file memxor.c
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

#include "memxor.h"
#include <inttypes.h>

inline static void memxor_align1(uint8_t * const dest, const uint8_t * const src, size_t len)
{
	uint8_t *dest_ptr = dest, *src_ptr = (uint8_t*) src;
	while (len)
	{
		*dest_ptr++ ^= *src_ptr++;
		len--;
	}
}

inline static void memxor_align2(uint16_t * const dest, const uint16_t * const src, size_t len)
{
	uint16_t *dest_ptr = dest, *src_ptr = (uint16_t*) src;
	while (len)
	{
		*dest_ptr++ ^= *src_ptr++;
		len-=2;
	}
}

inline static void memxor_align4(uint32_t * const dest, const uint32_t * const src, size_t len)
{
	uint32_t *dest_ptr = dest, *src_ptr = (uint32_t*) src;
	while (len)
	{
		*dest_ptr++ ^= *src_ptr++;
		len -= 4;
	}
}

inline static void memxor_align8(uint64_t * const dest, const uint64_t * const src, size_t len)
{
	uint64_t *dest_ptr = dest, *src_ptr = (uint64_t*) src;
	while (len)
	{
		*dest_ptr++ ^= *src_ptr++;
		len-=8;
	}
}

void * memxor(void * const dest, const void * const src, size_t len)
{
	if (0 ==(len & 7))
	{
		memxor_align8(dest, src, len);
		return dest;
	}
	
	if (0 ==(len & 3))
	{
		memxor_align4(dest, src, len);
		return dest;
	}

	if (0 ==(len & 1))
	{
		memxor_align2(dest, src, len);
		return dest;
	}
	
	memxor_align1(dest, src, len);
	return dest;
}

