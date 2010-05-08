/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	decoder_amr.c
* User for : 	820
* Creat By : 	Zhou Jian()                                                                   
* CrateDate: 	2007/07/19
* Last modify : Zhou Jian 2007-07-19     
*
* release history:
* 		version 0.1 release by Zhou Jian (Zhang Zhongliang AE)
=======================================================================*/


#ifndef AMR_FILEDECODE_H
#define AMR_FILEDECODE_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdlib.h>

//---------------define for message debug for memory lost 2007/04/20---------------------------
/*extern void* SD_Malloc( unsigned int size);
extern void SD_Free(void* pBuf);*/

void *amr_parser_malloc( unsigned int size);
void amr_parser_free( void *memblock );

#define   MAX_FRAME_NUMBER  100
#define   MIN_FRAME_NUMBER  16
#define   AMR_PARSER_MALLOC_DEBUG			0
#define   AMR_PARSER_MESSAGE_DEBUG			1
#define   AMR_PARSER_ERROR_DEBUG			0
#define   AMR_PARSER_PRECITION				1


#if	AMR_PARSER_MALLOC_DEBUG
#define 	AMR_PARSER_MALLOC_MSG(str)		{me_printf("\nAMR_PARSER_MALLOC: ");me_printf str;}
#define 	AMR_PARSER_FREE_MSG(str)		{me_printf("\nAMR_PARSER_FREE:    ");me_printf str;}
#define 	AMR_PARSER_TRACE_MSG()			{me_printf("\n-*-%s,line=%d\n",__FILE__,__LINE__);}
#else
#define 	AMR_PARSER_MALLOC_MSG(str)	
#define 	AMR_PARSER_FREE_MSG(str)	
#define 	AMR_PARSER_TRACE_MSG()			
#endif

#if	AMR_PARSER_MESSAGE_DEBUG
#define 	AMR_PARSER_MESSAGE(str)		{me_printf("\nAMR_PARSER_MESSAGE: ");me_printf str;}
#else
#define 	AMR_PARSER_MESSAGE(str)			
#endif

#if	AMR_PARSER_ERROR_DEBUG
#define 	AMR_PARSER_ERROR_MSG(str)	{me_printf("\nAMR_PARSER_ERROR_MESSAGE: ");me_printf str;}
#else
#define 	AMR_PARSER_ERROR_MSG(str)		
#endif



typedef struct
{
	void *stream;/*ÎÄ¼þÖ¸Õë*/
	void *start_frame_stream;
	int (*read_func)(void *file, char* buffer, int readbytes);
	int (*write_func)(void *file, char* buffer, int writebytes);
	int (*seek_func)(void *file, int i_offset, int mode);
	int (*tell_func)(void *file);

	unsigned int		is_amr;
	unsigned int		framecnt;
	unsigned int		maxframesize;
	//unsigned int		*perframesize;

	unsigned int		a_isvalid;		/* 1: have audio, 0 no audio */
	unsigned int		a_bits;			/* bit length */
	unsigned int		a_chn;			/* 1: mono, 2: stereo */
	unsigned int		a_frmsize;		/* frame size */
	unsigned int		a_smplrate;		/* sample rate */
	unsigned int		a_smpllen;		/* byte number of one audio sample */
	char				a_codec[4];		/* audio codec type id */

	/* I/O */
	unsigned int  file_length; 
	unsigned int  file_position;      		/* Current position of file descriptor */
	
	/* Read ahead buffer */
	int  	preload_size;      		/* Enables preload when nonzero. */
	int  	preload_start;     		/* Start of preload_buffer in file */
	int  	preload_end;      			/* End of preload_buffer in file */
	int 	preload_ptr;       			/* Offset of preload_start in preload_buffer */
	char *preload_buffer;
	unsigned int	file_end;

} amr_infor_t;
unsigned int amr_read_info(amr_infor_t *file);
amr_infor_t* amr_init(void);
unsigned int amr_close(amr_infor_t *file);
unsigned int amr_check_sig(amr_infor_t *file);
unsigned int amr_getmaxframesize(amr_infor_t *file,unsigned int *framesize);
unsigned int amr_getaudioduration(amr_infor_t *fileinfor, unsigned int frame, unsigned int * duration);
unsigned int amr_readaudio(amr_infor_t *file,char *audio_buffer, unsigned int *readbytes,unsigned int bufferlength);
unsigned int amr_seek(void *fileinfor, unsigned int pos, unsigned int * accesspos);
int amr_read_data(amr_infor_t *file, char *data, int size);
#ifdef __cplusplus
}
#endif
#endif
