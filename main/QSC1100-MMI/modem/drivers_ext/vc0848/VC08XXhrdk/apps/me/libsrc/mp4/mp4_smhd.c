/*=============================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	smhd.c  
* User for : 	mp4 demux mux, smhd atom is leaf atom of minf atom, it is mandatory
*			sound media header, include sound track information
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/10/04
* Last modify : Amanda Deng 2006-xx-xx     
*
* relase history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=============================================================================*/

#include "mp4_parser.h"


void mp4_smhd_init(mp4_smhd_t *smhd)
{
	smhd->version = 0;
	smhd->flags = 0;
	smhd->balance = 0;
	smhd->reserved = 0;
}

void mp4_smhd_delete(mp4_smhd_t *smhd)
{
	
}

void mp4_smhd_dump(mp4_smhd_t *smhd)
{
	MP4_DEMUX_MSG(("sound media header\n"));
	MP4_DEMUX_MSG(("version %d\n", smhd->version));
	MP4_DEMUX_MSG(("flags %d\n", smhd->flags));
	MP4_DEMUX_MSG(("balance %d\n", smhd->balance));
	MP4_DEMUX_MSG(("reserved %d\n", smhd->reserved));
}

uint32 mp4_read_smhd(mp4_infor_t *file, mp4_smhd_t *smhd)
{
	uint32 result;
	result = mp4_read_char(file, &smhd->version);
	if(result) return result;
	result = mp4_read_uint24(file, &smhd->flags);
	if(result) return result;
	result = mp4_read_uint16(file, &smhd->balance);
	if(result) return result;
	return mp4_read_uint16(file, &smhd->reserved);
}

uint32 mp4_write_smhd(mp4_infor_t *file, mp4_smhd_t *smhd)
{
	mp4_atom_t atom;
	uint32 result;
	
	result = mp4_atom_write_header(file, &atom, "smhd");
	if(result) return result;
	
	result = mp4_write_char(file, smhd->version);
	if(result) return result;
	result = mp4_write_uint24(file, smhd->flags);
	if(result) return result;
	
	if(file->audio_type == MP4_TYPE_AUDIO_IS_MP4A)
	{
		result = mp4_write_uint32(file, 0x00000000);
		if(result) return result;
	} 
	else 
	{
		result = mp4_write_uint16(file, smhd->balance);
		if(result) return result;
		result = mp4_write_uint16(file, smhd->reserved);
		if(result) return result;
	}

	return  mp4_atom_write_footer(file, &atom);

}
