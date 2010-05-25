/*=============================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	matrix.c  
* User for : 	mp4 demux mux, 
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/10/04
* Last modify : Amanda Deng 2006-xx-xx     
*
* relase history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=============================================================================*/

#include "mp4_parser.h"


void mp4_matrix_init(mp4_matrix_t *matrix)
{
	uint32 i;
	for(i = 0; i < 9; i++) matrix->values[i] = 0;
	matrix->values[0] = matrix->values[4] = 1;
	matrix->values[8] = 16384;
}

void mp4_matrix_delete(mp4_matrix_t *matrix)
{

}

void mp4_matrix_dump(mp4_matrix_t *matrix)
{
	uint32 i;
	MP4_DEMUX_MSG((" matrix"));
	for(i = 0; i < 9; i++) 
		MP4_DEMUX_MSG((" %f", matrix->values[i]));
	MP4_DEMUX_MSG(("\n"));
}

uint32 mp4_read_matrix(mp4_infor_t *file, mp4_matrix_t *matrix)
{
	uint32 result ;

	result =mp4_read_fixed32(file, &matrix->values[0]);
	if(result) return result;
	result = mp4_read_fixed32(file, &matrix->values[1]);
	if(result) return result;
	result = mp4_read_fixed32(file, &matrix->values[2]);
	if(result) return result;
	result = mp4_read_fixed32(file, &matrix->values[3]);
	if(result) return result;
	result = mp4_read_fixed32(file, &matrix->values[4]);
	if(result) return result;
	result = mp4_read_fixed32(file, &matrix->values[5]);
	if(result) return result;
	result = mp4_read_fixed32(file, &matrix->values[6]);
	if(result) return result;
	result = mp4_read_fixed32(file, &matrix->values[7]);
	return  mp4_read_fixed32(file, &matrix->values[8]);
}


uint32 mp4_write_matrix(mp4_infor_t *file, mp4_matrix_t *matrix)
{
	uint32 result;

	result = mp4_write_fixed32(file, matrix->values[0]);
	if(result) return result;
	result = mp4_write_fixed32(file, matrix->values[1]);
	if(result) return result;
	result = mp4_write_fixed32(file, matrix->values[2]);
	if(result) return result;
	result = mp4_write_fixed32(file, matrix->values[3]);
	if(result) return result;
	result = mp4_write_fixed32(file, matrix->values[4]);
	if(result) return result;
	result = mp4_write_fixed32(file, matrix->values[5]);
	if(result) return result;
	result = mp4_write_fixed32(file, matrix->values[6]);
	if(result) return result;
	result = mp4_write_fixed32(file, matrix->values[7]);
	return  mp4_write_fixed32(file, matrix->values[8]);
}

