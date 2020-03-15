/**
 * @file as_config.h
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
#ifndef PHEMAP_AS_CONFIG_H
#define PHEMAP_AS_CONFIG_H

#include "config.h"

/**
 * @brief Absolute number of enrolled chains, for each managed device
 */
#define NUMBER_OF_ENROLLED_CHAINS 100

/**
 * @brief Absolute maximum chain length
 */
#define ABSOLUTE_MAX_CHAIN_LENGTH 1000

#define AS_SENTINEL SENTINEL
#define AS_SENTINEL_ROOT AS_SENTINEL+3

#endif
