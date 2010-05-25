/*=============================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	atom.c  
* User for : 	mp4 demux mux, atom is basic member in mp4 file, atom have 4 bytes type flag and 4bytes length infor                                            
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/10/04
* Last modify : Amanda Deng 2006-xx-xx     
*
* relase history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=============================================================================*/

#include <ctype.h>
#include "mp4_parser.h"

 /*******************************************************************************************
Description:
	reset atom type and end position
Parameters:
       atom: point of the atom
Return:	
       void
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
void mp4_atom_reset(mp4_atom_t *atom)
{
	atom->end = atom->start + 8;
	atom->type[0] = atom->type[1] = atom->type[2] = atom->type[3] = 0;
}
 /*******************************************************************************************
Description:
	get atom size form source 4 bytes data
Parameters:
	data: point of souce data form file
Return:	
	32 bit unsigned length value
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32  mp4_atom_read_size(char *data)
{
	uint32 result;
	uint8 a, b, c, d;
	
	a = (uint8)data[0];
	b = (uint8)data[1];
	c = (uint8)data[2];
	d = (uint8)data[3];

	result = (a<<24) | (b<<16) | (c<<8) | d;
	if(result > 0 && result < HEADER_LENGTH) 
		result = HEADER_LENGTH;
	return (uint32 )result;
}

 /*******************************************************************************************
Description:
	get extended size form source 8 bytes data
Parameters:	
	data: point of souce data form file
Return:	
	64 bit unsigned length value
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint64 mp4_atom_read_size64(char *data)
{
	uint64 result = 0;
	uint32 i;

	for (i = 0; i < 8; i++) 
	{
		result |=(uint64)data[i];
		if (i < 7) 
		{
			result <<= 8;
		}
	}

	if(result < HEADER_LENGTH) 
		result = HEADER_LENGTH;
	
	return result;
}

 /*******************************************************************************************
Description:
	get atom type, and judge the type data is ok?
Parameters:
	data: point of souce data form file
	type: point of atom type
Return:	
	TRUE : the atom type is ok
	FALSE: the atom type is error
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_atom_read_type(char *data, char *type)
{
	type[0] = data[4];
	type[1] = data[5];
	type[2] = data[6];
	type[3] = data[7];

	if( isalnum(type[0])&&isalnum(type[1])
		&&isalnum(type[2])&&isalnum(type[3]))
	{
		return MP4_MUX_DEMUX_RET_NOERR;
	}
	else
	{
		return MP4_MUX_DEMUX_RET_NOERR;
		//return MP4_DEMUX_ATOM_TYPE_ERROR;
	}
}


 /*******************************************************************************************
Description:
	read atom type and size information form file, nomal need read 8 bytes
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
uint32 mp4_atom_read_header(mp4_infor_t *file, mp4_atom_t *atom)
{
	char header[10];
	uint32 result;
	
	atom->start = mp4_get_file_position(file);
	mp4_atom_reset(atom);
	
	/* read atom header */
	result =mp4_read_data(file, header, HEADER_LENGTH); 
	if(result!=HEADER_LENGTH)
	{
		MP4_DEMUX_DBG(("mp4_atom_read_header return = %d!", result));
		return MP4_DEMUX_READ_FILE_ERROR;
	}
	
	/* Get new atom size */
	atom->size = mp4_atom_read_size(header);
	if (!atom->size) 
	{
		atom->size = file->total_length - atom->start;
	}
	atom->end = atom->start + atom->size;
	
	/* Get new atom type */
	if(mp4_atom_read_type(header, atom->type))
	{
		MP4_DEMUX_DBG(("mp4_atom_read_type, atom->type=%c%c%c%c", atom->type[0],atom->type[1],atom->type[2],atom->type[3]));
		return MP4_DEMUX_ATOM_TYPE_ERROR;
	}
	
	/* Get extended type */
	if(mp4_match_char32(atom->type, "wide"))
	{	
		/* Skip placeholder atom */
		atom->start = mp4_get_file_position(file);
		mp4_atom_reset(atom);

		/* read atom header */
		result =mp4_read_data(file, header, HEADER_LENGTH); 
		if(result!=HEADER_LENGTH)
		{
			MP4_DEMUX_DBG(("mp4_atom_read_header return = %d!", result));
			return MP4_DEMUX_READ_FILE_ERROR;
		}

		/*check atom size*/
		if(!atom->size)
		{
			MP4_DEMUX_DBG(("MP4_DEMUX_ATOM_SIZE_ERROR: atom->size= %d!", atom->size));
			return MP4_DEMUX_ATOM_SIZE_ERROR;
		}
			
		atom->size -= 8;
		if(!atom->size)
		{
			/* Wrapper ended.  Get new atom size */
			atom->size = mp4_atom_read_size(header);
			if (!atom->size) 
			{
				MP4_DEMUX_DBG(("MP4_DEMUX_ATOM_SIZE_ERROR: atom->size= %d!", atom->size));
				/*adjust atom size*/
				atom->size = file->total_length - atom->start;
				return MP4_DEMUX_ATOM_SIZE_ERROR;
			}
		}
		atom->end = atom->start + atom->size;

		/* Get new atom type */
		if(mp4_atom_read_type(header, atom->type))
		{
			MP4_DEMUX_DBG(("mp4_atom_read_type, atom->type=%c%c%c%c", atom->type[0],atom->type[1],atom->type[2],atom->type[3]));
			return MP4_DEMUX_ATOM_TYPE_ERROR;
		}
	}
	else	 if(atom->size == 1)	
	{	
		/* Read extended size */
		result =mp4_read_data(file, header, HEADER_LENGTH);
		if(result!=HEADER_LENGTH)
		{
			MP4_DEMUX_DBG(("mp4_atom_read_header return = %d!", result));
			return MP4_DEMUX_READ_FILE_ERROR;
		}
		/* Get extended size */
		atom->size = mp4_atom_read_size64(header);
		if (!atom->size) 
		{
			MP4_DEMUX_DBG(("MP4_DEMUX_ATOM_SIZE_ERROR: atom->size= %d!", atom->size));
			/*adjust atom size*/
			atom->size = file->total_length - atom->start;
			return MP4_DEMUX_ATOM_SIZE_ERROR;
		}
	}
	return MP4_MUX_DEMUX_RET_NOERR;
}
 /*******************************************************************************************
Description:
       write atom type information to mux file, size initialize to 0 temporarily
Parameters:	
	file: mp4 infor stuctor point (include file handle)
	atom: point of the atom
	text: atom type point(4 bytes length)
Return:	
	void
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_atom_write_header(mp4_infor_t *file, mp4_atom_t *atom, char *text)
{
	uint32 result;
	
	atom->start = mp4_get_file_position(file);
	
	/* write atom size, now is 0, then updata in mp4_atom_write_footer() */
	result = mp4_write_uint32(file, 0);
	if(result)
		return result;
	
	/* write atom ID */
	return mp4_write_char32(file, text);

}
 /*******************************************************************************************
Description:
       write atom really size to mux file 
Parameters:	
	file: mp4 infor stuctor point (include file handle)
	atom: point of the atom
Return:	
	void
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_atom_write_footer(mp4_infor_t *file, mp4_atom_t *atom)
{
	uint32 result;
	/* get atom end offset*/
	atom->end = mp4_get_file_position(file);
	/* seek file position to atom start offset*/
	result = mp4_set_file_position(file, atom->start);
	if(result)
		return result;
	/* write atom size = atom->end - atom->start */
	result = mp4_write_uint32(file, atom->end - atom->start);
	if(result)
		return result;
	/* seek file position to atom end offset*/
	return mp4_set_file_position(file, atom->end);
}
 /*******************************************************************************************
Description:
       judge the atom type is our need,
Parameters:	
	atom: point of the atom
	type: our need atom type point(4 bytes length)
Return:	
	TRUE : the atom is our need
	FALSE: the atom is not our need
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_atom_is(mp4_atom_t *atom, char *type)
{
	if(atom->type[0] == type[0]  && atom->type[1] == type[1] 
      && atom->type[2] == type[2]  && atom->type[3] == type[3])
		return TRUE;
	else
		return FALSE;
}
 /*******************************************************************************************
Description:
	skip the atom, file point to the atom end position
Parameters:
	file: mp4 infor stuctor point (include file handle)
	atom: point of the atom
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_MUX_DEMUX_POINT_OVERFLOW
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_atom_skip(mp4_infor_t *file, mp4_atom_t *atom)
{
	return mp4_set_file_position(file, atom->end);
}
 
