/**
 * @file phemap_chain.c
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
#include <stdio.h> 
#include <stdlib.h>

#include "phemap_chain.h"

/**
 * @brief Chain database creation.
 * 
 * @details
 * The function create a database if this not exisisting yet.
 *
 * @param [in]	db_name
 * 				Name of chains database. 
 *  
 * @return 0 on succes or -1, if an error occurs (for example, when the
 * database already exist).
 */
int32_t PHEMAP_Chain_createDatabase(
		const char * const db_name)
{
	FILE * fp;
	fp = fopen (db_name,"r");
	
	if(fp!=NULL)
	{
		fclose (fp);
		printf("Database already exist.");
		return -1;
	}
	else
	{
		fp = fopen (db_name,"w");
		fclose (fp);
		printf("Database created.");
	}
	
	return 0;
}

/**
 * @brief Chain load, to get a chain.
 * 
 * @details
 * The function queries the chain database and get a chain, if exist.
 *
  * @param [in]	db_name
 * 				Name of chains database. 
 *  
 * @param [in]	device_id
 * 				Device identifier of required chain.
 *
 * @param [in]	chain_id 
 * 				boh //TODO
 * 
 * @param [out]	chain
 * 				Memory area in which obtained chain links will be stored. This
 * 				area must be pre-allocated and its size must be able to contain
 * 				a number of links equal to maxChainLength. The link in
 * 				position 0 will be the initial random challenge @$c_0@$, 
 * 				the link in position 1 will be @$l_1 = \psi_D(c_0)@$,
 * 				the element in position 2 will be 
 * 				@$l_2 = \psi_D(l_1) = \psi_D (\psi_D(c_0))@$, and so on.
 * 
 * @return Number of chain read, on succes, or -1, if an error occurs (for example, when the
 * database does not exist).
 */
int32_t PHEMAP_Chain_load(
		const char * const db_name,
		PHEMAP_Device_ID_t device_id,
		uint32_t chain_id,
		PHEMAP_Chain_t * const chain)
{
	// int fseek(FILE *fp, long distanza, int partenza)
	// distanza indica di quanti byte il file pointer deve essere spostato 
	// partenza indica da quale posizione deve essere spostato.

	int res = 0;
	char buff[50];

	res = sprintf(buff, "%016lx", device_id);
	char dbase[strlen(db_name)+16+1];

	strcpy(dbase, db_name);
	strcat(dbase, buff);

	FILE * fp;
	fp = fopen (dbase,"r");
	if(fp==NULL)
	{
		return -1;
	}

	//leggo la lunghezza della chain 
	res = fscanf(fp, "%x", &chain->length);	
	printf("lenght: %x\n", chain->length);	
	if(res == EOF) {
		fclose (fp);
		return -1;	
	}

	//leggo il link a cui sono arrivato 
	res = fscanf(fp, "%x", &chain->current);
	printf("current: %x\n", chain->current);	
	if(res == EOF) {
		fclose (fp);
		return -1;	
	}

	//leggo i link della chain
	char buff_b[3];
	buff_b[2] = '\n';
	uint8_t * read_ptr;
	int32_t byte_read;

	printf("Enter the chain link reading\n");
	for(int i = 0; i < chain->length+1; i++){
		read_ptr = (uint8_t*) &chain->links[i];
		
		res = fscanf(fp, "%s", buff);
		if(res == EOF) {
			fclose (fp);
			return -1;	
		}

		for (int j = 0; j < sizeof(PHEMAP_Link_t); j++)
		{
			memcpy(buff_b,&buff[2*j],2);
			sscanf(buff_b,"%x", &byte_read);
			read_ptr[j] = (uint8_t)(byte_read & 0x000000FF);
		}
	}

	fclose (fp);
	return 0;	//on success
}

/**
 * @brief Chain store, to store a chain.
 * 
 * @details
 * The function append the chain in the chains database.
 *
 * @param [in]	db_name
 * 				Name of chains database. 
 *  
 * @param [in]	device_id
 * 				Device identifier of the chain to be stored.
 *
 * @param [in]	chain_id 
 * 				boh //TODO
 * 
 * @param [out]	chain
 * 				Memory area in which chain links will be stored. This
 * 				area must be pre-allocated and its size must be able to contain
 * 				a number of links equal to maxChainLength. The link in
 * 				position 0 will be the initial random challenge @$c_0@$, 
 * 				the link in position 1 will be @$l_1 = \psi_D(c_0)@$,
 * 				the element in position 2 will be 
 * 				@$l_2 = \psi_D(l_1) = \psi_D (\psi_D(c_0))@$, and so on.
 * 
 * @return 0, on succes, or -1, if an error occurs (for example, when the
 * the write fail).
 */
int32_t PHEMAP_Chain_store(		
		const char * const db_name,
		PHEMAP_Device_ID_t device_id,
		uint32_t chain_id,		//è necessario?
		const PHEMAP_Chain_t * const chain)
{
	int res = 0;
	char buff[50];

	res = sprintf(buff, "%016lx", device_id);
	char dbase[strlen(db_name)+16+1];

	strcpy(dbase, db_name);
	strcat(dbase, buff);
	printf("database name: %s\n", dbase);

	FILE * fp;
	fp = fopen (dbase,"w");
	if(fp==NULL)
	{
		return -1;
	}

	//scrivo length e current 
	res = sprintf(buff, "%08x %08x\n", chain->length, chain->current);
	res = fprintf(fp, "%s",buff);
	if(res < 0) {
		fclose (fp);
		return -1;	//controllare che il ritorno sia 1
	}

	int k;
	uint8_t * print_ptr;
	printf("Enter the chain link copy\n");
	for(int i = 0; i < chain->length+1; i++){
		k = 0;
		print_ptr = (uint8_t*) &chain->links[i];
		printf("Copying the %d-th link\n", i);
		for (int j = 0; j < sizeof(PHEMAP_Link_t); j++)
		{
			k = k + sprintf(&buff[k], "%02x", print_ptr[j]);
		}
		sprintf(&buff[k], "\n");
		res = fprintf(fp, "%s",buff);
	}
	fclose (fp);

	return 0;
}

/**
 * @brief Chain update, to update a stored chain.
 * 
 * @details
 * The function update the chain in the chains database, if exist.
 *
 * @param [in]	db_name
 * 				Name of chains database. 
 *  
 * @param [in]	device_id
 * 				Device identifier of the chain to be stored.
 *
 * @param [in]	chain_id 
 * 				boh //TODO
 * 
 * @param [out]	chain
 * 				Memory area in which chain links will be stored. This
 * 				area must be pre-allocated and its size must be able to contain
 * 				a number of links equal to maxChainLength. The link in
 * 				position 0 will be the initial random challenge @$c_0@$, 
 * 				the link in position 1 will be @$l_1 = \psi_D(c_0)@$,
 * 				the element in position 2 will be 
 * 				@$l_2 = \psi_D(l_1) = \psi_D (\psi_D(c_0))@$, and so on.
 * 
 * @return 0, on succes, or -1, if an error occurs (for example, when the chain
 * does not exist in database).
 */
int32_t PHEMAP_Chain_update(
		const char * const db_name,
		PHEMAP_Device_ID_t device_id,
		uint32_t chain_id,
		PHEMAP_Chain_t * const chain)
{
	int res = 0;
	char buff[50];

	res = sprintf(buff, "%016lx", device_id);
	char dbase[strlen(db_name)+16];

	strcpy(dbase, db_name);
	strcat(dbase, buff);

	FILE * fp;
	fp = fopen (dbase,"r+");
	if(fp==NULL)
	{
		return -1;
	}

	//aggiorno i campi della chain
	res = sprintf(buff, "%08x %08x\n", chain->length, chain->current);
	res = fprintf(fp, "%s",buff);
	if(res < 0) {
		fclose (fp);
		return -1;	
	}

	fclose (fp);
	return 0;	//on success
}