/**
 * @file config.h
 *
 * @author Salvatore Barone <salvator.barone@gmail.com>
 * @author Alfonso Fezza <alfonsofezza93@gmail.com>
 *
 * Copyright 2019 Salvatore Barone <salvator.barone@gmail.com>
 * Copyright 2019 Alfonso Fezza <alfonsofezza93@gmail.com>
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

#ifndef PHEMAP_DEVICE_CONFIG_H
#define PHEMAP_DEVICE_CONFIG_H

// #include "config.h"

/**
 * @brief SRAM PUF size,in byte
 */
#define SRAMPUF_SIZE 128 

/**
 * @brief RC5 encryption rounds.
 *
 * @details
 * In order to obtain a strong PUF froma  weak one, this implementation of 
 * PHEMAP makes use of the RC5 encryption algorithm.
 */
#define RC5_ENCRYPTION_ROUNDS 12

/**
 * @brief RC5 encryption key length, in byte
 *
 * @warning
 * DO NOT EDIT THIS VALUE
 */
#define RC5_KEY_LENGTH	32 

#define BC_DEV_NUM  3//(DEV_FANIN + DEV_FANOUT)

#endif
