/*=============================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	vmhd.c  
* User for : 	mp4 demux mux, vmhd atom is leaf atom of minf atom, it is mandatory
*			video media header, include video track information
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/10/04
* Last modify : Amanda Deng 2006-xx-xx     
*
* relase history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=============================================================================*/

#include "mp4_parser.h"


void mp4_vmhd_init(mp4_vmhd_t *vmhd)
{
	vmhd->version = 0;
	vmhd->flags = 1;
	vmhd->graphics_mode = 64;
	vmhd->opcolor[0] = 32768;
	vmhd->opcolor[1] = 32768;
	vmhd->opcolor[2] = 32768;
}

void mp4_vmhd_init_video(mp4_infor_t *file, 
							mp4_vmhd_t *vmhd, 
							uint32 frame_w,
							uint32 frame_h, 
							uint32  sample_duration)
{

}

void mp4_vmhd_delete(mp4_vmhd_t *vmhd)
{

}

void mp4_vmhd_dump(mp4_vmhd_t *vmhd)
{
	MP4_DEMUX_MSG(("    video media header\n"));
	MP4_DEMUX_MSG(("     version %d\n", vmhd->version));
	MP4_DEMUX_MSG(("     flags %d\n", vmhd->flags));
	MP4_DEMUX_MSG(("     graphics_mode %d\n", vmhd->graphics_mode));
	MP4_DEMUX_MSG(("     opcolor %d %d %d\n", vmhd->opcolor[0], vmhd->opcolor[1], vmhd->opcolor[2]));
}

uint32 mp4_read_vmhd(mp4_infor_t *file, mp4_vmhd_t *vmhd)
{
	uint32 i, result;
	
	result =mp4_read_char(file, &vmhd->version);
	if(result) return result;
	result =mp4_read_uint24(file, &vmhd->flags);
	if(result) return result;
	result =mp4_read_uint16(file, &vmhd->graphics_mode);
	if(result) return result;
	
	for(i = 0; i < 3; i++)
	{
		result = mp4_read_uint16(file, &vmhd->opcolor[i]);
		if(result) return result;
	}
	return 0;
}

uint32 mp4_write_vmhd(mp4_infor_t *file, mp4_vmhd_t *vmhd)
{
	mp4_atom_t atom;
	uint32 i, result;
	
	result =mp4_atom_write_header(file, &atom, "vmhd");
	if(result) return result;
	
	result = mp4_write_char(file, vmhd->version);
	if(result) return result;
	result =mp4_write_uint24(file, vmhd->flags);
	if(result) return result;

	if (file->video_type == MP4_TYPE_VIDEO_IS_MP4V) 
	{
		result =mp4_write_uint64(file, (uint64)0);
		if(result) return result;
	} 
	else 
	{
		result =mp4_write_uint16(file, vmhd->graphics_mode);
		if(result) return result;
		
		for(i = 0; i < 3; i++) 
		{
			result = mp4_write_uint16(file, vmhd->opcolor[i]);
			if(result) return result;
		}
	}

	return mp4_atom_write_footer(file, &atom);
}

