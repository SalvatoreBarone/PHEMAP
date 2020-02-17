/**
 * @file phemap_chain.c
 *
 * @author Salvatore Barone <salvator.barone@gmail.com>
 * @author Alfonso Fezza <alfonsofezza93@gmail.com>
 *
 * Copyright 2019 Salvatore Barone <salvator.barone@gmail.com>
 * Copyright 2019 Alfonso Fezza <alfonsofezza93@gmail.com>
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
#include <stdio.h> 
#include <stdlib.h>

#include "phemap_chain.h"
// #include <sqlite3.h>

int32_t PHEMAP_Chain_createDatabase(
		const char * const databasename)
{
	FILE * fp;
	fp = fopen (databasename,"rb");
	
	if(fp!=NULL)
	{
		fclose (fp);
		printf("Database already exist.");
		return -1;
	}
	else
	{
		fp = fopen (databasename,"wb");
		fclose (fp);
		printf("Database created.");
	}
	
	return 0;
}

int32_t PHEMAP_Chain_load(
		const char * const databasename,
		PHEMAP_Device_ID_t device_id,
		uint32_t chain_id,
		PHEMAP_Chain_t * const chain)
{
	// int fseek(FILE *fp, long distanza, int partenza)
	// distanza indica di quanti byte il file pointer deve essere spostato 
	// partenza indica da quale posizione deve essere spostato.

	FILE * fp;
	fp = fopen (databasename,"rb");

	PHEMAP_Device_ID_t read_dev_id = 0;
	int res = 0;

	//finchè non raggiungo la fine del file 
	while(!feof(fp)) {
		
		//leggo device_id
		res = fread(&read_dev_id, sizeof(PHEMAP_Device_ID_t), 1, fp);
		if(res != 1) return -1;	//controllare che il ritorno sia 1

		//controllo che corrisponda a quello che voglio
		if (read_dev_id == device_id)
		{
			// se si leggo la chain
			res = fread(chain, sizeof(PHEMAP_Chain_t), 1, fp);
			if(res != 1) return -1;	//controllare che il ritorno sia 1
			return 0;	//on success
		}
		else
		{
			// se no mi sposto avanti di sizeof(PHEMAP_Chain_t) byte
			res = fseek(fp, sizeof(PHEMAP_Chain_t), SEEK_CUR);
			if(res != 0) return res;	//controllare che il ritorno sia 1
		}
	}
}

//questa la uso per salvare la chain la prima volta
//per aggiornare il link a cui si è arrivati è meglio usare 
//una funzione diversa che fa l'update oppure faccio dei controlli
// in questa e la uso anche come update?
int32_t PHEMAP_Chain_store(		
		const char * const databasename,
		PHEMAP_Device_ID_t device_id,
		uint32_t chain_id,		//è necessario?
		const PHEMAP_Chain_t * const chain)
{
	FILE * fp;
	fp = fopen (databasename,"ab");

	int res = 0;
	res = fwrite(&device_id, sizeof(PHEMAP_Device_ID_t), 1, fp);
	if(res != 1) return -1;	//controllare che il ritorno sia 1

	res = fwrite(chain, sizeof(PHEMAP_Chain_t), 1, fp);
	if(res != 1) return -1;	//controllare che il ritorno sia 1

	fclose (fp);

	return 0;
}
