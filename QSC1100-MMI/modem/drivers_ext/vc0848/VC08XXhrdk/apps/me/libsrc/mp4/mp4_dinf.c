/*=============================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name:	dinf.c  
* User for :	mp4 demux mux, dinf atom is leaf atom of minf, it is mandatory (but now I don't know its use) 
*			dinf atom is data information box                                             
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/10/04
* Last modify : Amanda Deng 2006-xx-xx     
*
* relase history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=============================================================================*/

#include "mp4_parser.h"

void mp4_dinf_init(mp4_dinf_t *dinf)
{
	mp4_dref_init(&(dinf->dref));
}

void mp4_dinf_delete(mp4_dinf_t *dinf)
{
	mp4_dref_delete(&(dinf->dref));
}

void mp4_dinf_init_all(mp4_dinf_t *dinf)
{
	mp4_dref_init_all(&(dinf->dref));
}

void mp4_dinf_dump(mp4_dinf_t *dinf)
{
	MP4_DEMUX_MSG(("data information (dinf)\n"));
	mp4_dref_dump(&(dinf->dref));
}
/*******************************************************************************************
Description:
	read dinf atom from file
Parameters:
	file: mp4 infor stuctor point (include file handle)
       dinf_atom: point of the dinf atom information
       dinf: point of dinf stuctor
Return:	
       MP4_MUX_DEMUX_RET_NOERR
       MP4_DEMUX_READ_FILE_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_read_dinf(mp4_infor_t *file, mp4_dinf_t *dinf, mp4_atom_t *dinf_atom)
{
	mp4_atom_t leaf_atom;
	uint32 result;
	do
	{
		/*read atom header*/
		result = mp4_atom_read_header(file, &leaf_atom);
		if(result)
			return result;
		
		if(mp4_atom_is(&leaf_atom, "dref"))
		{ 
			result = mp4_read_dref(file, &(dinf->dref)); 
			if(result)
				return result;
		}
		else
		{
			result = mp4_atom_skip(file, &leaf_atom);
			if(result)
				return result;
		}
	}while(mp4_get_file_position(file) < dinf_atom->end);
	
	return MP4_MUX_DEMUX_RET_NOERR;
}
/*******************************************************************************************
Description:
	write dinf atom to mux file
Parameters:
	file: mp4 infor stuctor point (include file handle)
       dinf: point of dinf stuctor
Return:	
       MP4_MUX_DEMUX_RET_NOERR
       MP4_DEMUX_READ_FILE_ERROR
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.10.04		create first version
********************************************************************************************/
uint32 mp4_write_dinf(mp4_infor_t *file, mp4_dinf_t *dinf)
{
	mp4_atom_t atom;
	uint32 result;
	
	result = mp4_atom_write_header(file, &atom, "dinf");
	if(result)
		return result;
	
	result = mp4_write_dref(file, &(dinf->dref));
	if(result)
		return result;
	
	return mp4_atom_write_footer(file, &atom);
}
