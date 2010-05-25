/*=============================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	dref.c  
* User for : 	mp4 demux mux, dref atom is leaf atom of dinf,  it is mandatory
*			dref atom is data reference box, declares sources of midia data in track
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/10/04
* Last modify : Amanda Deng 2006-xx-xx     
*
* relase history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=============================================================================*/

#include "mp4_parser.h"
/*******************************************************************************************
Description:
	initialize dref table information form file
Parameters:	
	table: point of the dref table
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_DEMUX_READ_FILE_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
void mp4_dref_table_init(mp4_dref_table_t *table)
{
	table->size = 0;
	table->type[0] = 'a';
	table->type[1] = 'l';
	table->type[2] = 'i';
	table->type[3] = 's';
	
	table->type[0] = 'u';
	table->type[1] = 'r';
	table->type[2] = 'l';
	table->type[3] = ' ';
	table->version = 0;
	table->flags = 0x0001;
	table->data_reference = mp4_malloc(256);
	table->data_reference[0] = 0;
}

void mp4_dref_table_delete(mp4_dref_table_t *table)
{
	if(table->data_reference!=NULL)
		mp4_free(table->data_reference);
	table->data_reference = NULL;
}


void mp4_dref_table_dump(mp4_dref_table_t *table)
{
	MP4_DEMUX_MSG((" data reference table (dref)\n"));
	MP4_DEMUX_MSG((" type %c%c%c%c\n", table->type[0], table->type[1], table->type[2], table->type[3]));
	MP4_DEMUX_MSG((" version %d\n", table->version));
	MP4_DEMUX_MSG((" flags %d\n", table->flags));
	MP4_DEMUX_MSG((" data %s\n", table->data_reference));
}

/*******************************************************************************************
Description:
	initialize dref head information form file
Parameters:	
	dref: point of the dref structor
Return:
	void
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
void mp4_dref_init(mp4_dref_t *dref)
{
	dref->version = 0;
	dref->flags = 0;
	dref->total_entries = 0;
	dref->table = 0;
}
/*******************************************************************************************
Description:
	initialize dref information form file, include dref table
Parameters:	
	dref: point of the dref structor
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_MUX_DEMUX_MALLOC_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
void mp4_dref_init_all(mp4_dref_t *dref)
{
	if(!dref->total_entries)
	{
		dref->total_entries = 1;
		dref->table = (mp4_dref_table_t *)mp4_malloc(sizeof(mp4_dref_table_t) * dref->total_entries);
		mp4_dref_table_init(&(dref->table[0]));
	}
}
/*******************************************************************************************
Description:
	read atom type and size information form file, nomal need read 8 bytes
Parameters:	
	dref: point of the dref structor
	void
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
void mp4_dref_delete(mp4_dref_t *dref)
{
	if(dref->table!=NULL)
	{
		uint32 i;
		for(i = 0; i < dref->total_entries; i++)
			mp4_dref_table_delete(&(dref->table[i]));
		mp4_free(dref->table);
	}
	dref->total_entries = 0;
}

void mp4_dref_dump(mp4_dref_t *dref)
{
	uint32 i;
	
	MP4_DEMUX_MSG(("data reference (dref)\n"));
	MP4_DEMUX_MSG(("version %d\n", dref->version));
	MP4_DEMUX_MSG(("flags %d\n", dref->flags));
	for(i = 0; i < dref->total_entries; i++)
	{
		mp4_dref_table_dump(&(dref->table[i]));
	}
}
/*******************************************************************************************
Description:
	read dref table information form file
Parameters:	
	file: mp4 infor stuctor point (include file handle)
	atom: point of the atom
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_DEMUX_READ_FILE_ERROR
	MP4_MUX_DEMUX_MALLOC_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_read_dref_table(mp4_infor_t *file, mp4_dref_table_t *table)
{
	uint32 result;
	
	/*read dref table size */
	result = mp4_read_uint32(file, &table->size);
	if(result) return result;
	
	/*read dref table type */
	result = mp4_read_char32(file, table->type);
	if(result) return result;
	
	/*read dref tableversion */
	result = mp4_read_char(file, &table->version);
	if(result) return result;
	
	/*read dref table flags */
	result = mp4_read_uint24(file, &table->flags);
	if(result) return result;
	
	if(table->data_reference) 
		mp4_free(table->data_reference);
	
	/*read dref table*/
	table->data_reference = mp4_malloc(table->size);
	if(table->size > 12)
	{
		result = mp4_read_data(file, table->data_reference, table->size - 12);
		if(result != table->size - 12) 
			return MP4_DEMUX_READ_FILE_ERROR;
	}
	table->data_reference[table->size - 12] = 0;
	return MP4_MUX_DEMUX_RET_NOERR;
}
/*******************************************************************************************
Description:
	write dref table information to file
Parameters:	
	file: mp4 infor stuctor point (include file handle)
	atom: point of the atom
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_DEMUX_READ_FILE_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_write_dref_table(mp4_infor_t *file, mp4_dref_table_t *table)
{
	uint32 len = strlen(table->data_reference);
	uint32 result;
	
	/*write dref table type */
	result = mp4_write_uint32(file, 12 + len);
	if(result)
		return result;
	/*write dref table type */
	result = mp4_write_char32(file, table->type);
	if(result)
		return result;
	/*write dref table version */
	result = mp4_write_char(file, table->version);
	if(result)
		return result;
	/*write dref table flags */
	result = mp4_write_uint24(file, table->flags);
	if(result)
		return result;
	/*write dref table */
	if(len)
	{
		result = mp4_write_data(file, table->data_reference, len);
		if(result!=len)
			return MP4_MUX_WRITE_FILE_ERROR;
	}
	return MP4_MUX_DEMUX_RET_NOERR;
}
/*******************************************************************************************
Description:
	read dref information form file, include dref table
Parameters:	
	file: mp4 infor stuctor point (include file handle)
	dref: point of the dref structor
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_DEMUX_READ_FILE_ERROR
	MP4_MUX_DEMUX_MALLOC_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_read_dref(mp4_infor_t *file, mp4_dref_t *dref)
{
	uint32 i;
	uint32 result;
	/*read dref atom version */
	result = mp4_read_char(file, &dref->version);
	if(result) return result;
	
	/*read dref atom flags */
	result = mp4_read_uint24(file, &dref->flags);
	if(result) return result;
	
	/*read dref atom total entries */
	result = mp4_read_uint32(file, &dref->total_entries);
	if(result) return result;
	
	/*read eatch dref table */
	dref->table = (mp4_dref_table_t*)mp4_malloc(sizeof(mp4_dref_table_t) * dref->total_entries);
	for(i = 0; i < dref->total_entries; i++)
	{
		mp4_dref_table_init(&(dref->table[i]));
		result = mp4_read_dref_table(file, &(dref->table[i]));
		if(result) return result;
	}
	return MP4_MUX_DEMUX_RET_NOERR;
}
/*******************************************************************************************
Description:
	write dref information to file, include dref table
Parameters:	
	file: mp4 infor stuctor point (include file handle)
	dref: point of the dref structor
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_MUX_WRITE_FILE_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_write_dref(mp4_infor_t *file, mp4_dref_t *dref)
{
	uint32 i;
	uint32 result;
	mp4_atom_t atom;
	
	/*write dref atom header , size =0 */
	result = mp4_atom_write_header(file, &atom, "dref");
	if(result)
		return result;
	/*write dref atom version */
	result = mp4_write_char(file, dref->version);
	if(result)
		return result;
	/*write dref atom flags*/
	result = mp4_write_uint24(file, dref->flags);
	if(result)
		return result;
	/*write dref atom total entries*/
	result = mp4_write_uint32(file, dref->total_entries);
	if(result)
		return result;
	/*write each dref table*/
	for(i = 0; i < dref->total_entries; i++)
	{
		result = mp4_write_dref_table(file, &(dref->table[i]));
		if(result)
			return result;
	}
	/*update dref atom size*/
	return mp4_atom_write_footer(file, &atom);
}
