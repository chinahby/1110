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
//extern void uart_printf(char *fmt,...);
/*extern void* SD_Malloc( unsigned int size);
extern void SD_Free(void* pBuf);*/

void *jpg_parser_malloc( unsigned int size);
void jpg_parser_free( void *memblock );
//#if(CHIP_ID==ID_VC0878)
//#define   JPG_PARSER_VC0878_DEBUG			1
//#endif

#define   JPG_PARSER_MALLOC_DEBUG			0
#define   JPG_PARSER_MESSAGE_DEBUG			1
#define   JPG_PARSER_ERROR_DEBUG			0
#define   JPG_PARSER_ALLSOFT_DEBUG			0

#if	JPG_PARSER_MALLOC_DEBUG
#define 	JPG_PARSER_MALLOC_MSG(str)		{me_printf("\nJPG_PARSER_MALLOC: ");me_printf str;}
#define 	JPG_PARSER_FREE_MSG(str)		{me_printf("\nJPG_PARSER_FREE:    ");me_printf str;}
#define 	JPG_PARSER_TRACE_MSG()			{me_printf("\n-*-%s,line=%d\n",__FILE__,__LINE__);}
#else
#define 	JPG_PARSER_MALLOC_MSG(str)	
#define 	JPG_PARSER_FREE_MSG(str)	
#define 	JPG_PARSER_TRACE_MSG()			
#endif

#if	JPG_PARSER_MESSAGE_DEBUG
#define 	JPG_PARSER_MESSAGE(str)		{me_printf("\nJPG_PARSER_MESSAGE: ");me_printf str;}
#else
#define 	JPG_PARSER_MESSAGE(str)			
#endif

#if	JPG_PARSER_ERROR_DEBUG
#define 	JPG_PARSER_ERROR_MSG(str)	{me_printf("\nJPG_PARSER_ERROR_MESSAGE: ");me_printf str;}
#else
#define 	JPG_PARSER_ERROR_MSG(str)		
#endif

enum JPEG_MARKER{			
/* JPEG marker codes */
  SOF0   = 0xc0,
  SOF1   = 0xc1,
  SOF2   = 0xc2,
  SOF3   = 0xc3,

  SOF5   = 0xc5,
  SOF6   = 0xc6,
  SOF7   = 0xc7,

  JPG    = 0xc8,
  SOF9   = 0xc9,
  SOF10  = 0xca,
  SOF11  = 0xcb,

  SOF13  = 0xcd,
  SOF14  = 0xce,
  SOF15  = 0xcf,

  DHT    = 0xc4,

  DAC    = 0xcc,

  RST0   = 0xd0,
  RST1   = 0xd1,
  RST2   = 0xd2,
  RST3   = 0xd3,
  RST4   = 0xd4,
  RST5   = 0xd5,
  RST6   = 0xd6,
  RST7   = 0xd7,

  SOI    = 0xd8,
  EOI    = 0xd9,
  SOS    = 0xda,
  DQT    = 0xdb,
  DNL    = 0xdc,
  DRI    = 0xdd,
  DHP    = 0xde,
  EXP    = 0xdf,

  APP0   = 0xe0,
  APP1   = 0xe1,
  APP2   = 0xe2,
  APP3   = 0xe3,
  APP4   = 0xe4,
  APP5   = 0xe5,
  APP6   = 0xe6,
  APP7   = 0xe7,
  APP8   = 0xe8,
  APP9   = 0xe9,
  APP10  = 0xea,
  APP11  = 0xeb,
  APP12  = 0xec,
  APP13  = 0xed,
  APP14  = 0xee,
  APP15  = 0xef,

  JPG0   = 0xf0,
  JPG13  = 0xfd,
  COM    = 0xfe
};

typedef enum JPEG_YUVMODE	//this enum must same as lbuf format order
{
	JPG_422 = 0,
	JPG_420,
	JPG_411,
	JPG_400,
	JPG_444,
	JPG_UNKNOWN,
	JPG_ERROR=0x7FFFFFFF
}JPEG_YUVMODE;


typedef struct
{
	void *stream;/*ÎÄ¼þÖ¸Õë*/
	void *start_frame_stream;
	int (*read_func)(void *file, char* buffer, int readbytes);
	int (*write_func)(void *file, char* buffer, int writebytes);
	int (*seek_func)(void *file, int i_offset, int mode);
	int (*tell_func)(void *file);

	unsigned int		is_jpg;
	unsigned int		is_v_sample;
	
	JPEG_YUVMODE   	YUVType;	//the format of jpeg file,such as YUV422\YUV420\YUV411\YUV400
	unsigned char	HTCount;		//the number of Huffman table
	unsigned char	QTCount;		//the number of Quantization table
	unsigned char	data_precision; //Specifies the precision in bits for the samples of the components in the frame.
	unsigned char	CompCount;		//the number of component
	unsigned char	qf; 			//Jpeg Q factor

	unsigned char	HTLen[4];		//the length of every Huffman table
	unsigned char	QTPrecision[4];	//the precision of quantization table 

	//unsigned short	frmcnt;			//frame count
	//unsigned short	blkcount;		//the number of block
	unsigned short	Comp[4];		//the component 
	
	unsigned int	offset;			//the offset from jpeg file begin to 
							//the postion of image data begin
	//unsigned int	vwc;			//video word count
	unsigned int	sop;			//the position of jpeg file begin(begin with FFD8)
	unsigned int	eop;			//the position of jpeg file end(end with FFD9)
	//BOOL 	frmEnd;			//frame end flag, use for add 0xffd9

	unsigned short ImageWidth;
	unsigned short ImageHeight;
	unsigned short JpgWidth;
	unsigned short JpgHeight;

	//TSize	ImageSize; 		//real image size
	//TSize	JpgSize;			//image size after compensation,if the image format is YUV422,
							//then,JpgSize.cx must be multiple of 16,and JpgSize.cy must be multiple of 8.

	unsigned char	HT[4][256]; 
	unsigned char	QT[4][128]; 

	//TThumbNail	thumbinfo;
	
	//unsigned int		*perframesize;

	char	v_codec[4];		/* audio codec type id */

	/* I/O */
	unsigned int  file_length; 
	unsigned int  file_rawdatalength;
	unsigned int  file_position;      		/* Current position of file descriptor */
	
	/* Read ahead buffer */
	int  	preload_size;      		/* Enables preload when nonzero. */
	int  	preload_start;     		/* Start of preload_buffer in file */
	int  	preload_end;      			/* End of preload_buffer in file */
	int 	preload_ptr;       			/* Offset of preload_start in preload_buffer */
	unsigned char    *preload_buffer;

} jpg_infor_t;


unsigned int jpg_read_info(jpg_infor_t *file);
jpg_infor_t* jpg_init(void);
unsigned int jpg_close(jpg_infor_t *file);
unsigned int jpg_check_sig(jpg_infor_t *file);
//unsigned int jpg_getmaxframesize(amr_infor_t *file,unsigned int *framesize);
//unsigned int jpg_getaudioduration(amr_infor_t *fileinfor, unsigned int frame, unsigned int * duration);
unsigned int jpg_read_image(jpg_infor_t *file,char *image_buffer, unsigned int *readbytes,unsigned int bufferlength);
unsigned int jpg_seek(void *fileinfor, unsigned int pos, unsigned int * accesspos);
unsigned int jpg_get_SOS(jpg_infor_t *file, unsigned char * ptr);
unsigned int jpg_get_DHT(jpg_infor_t *file, unsigned char * ptr,unsigned short len);
unsigned int jpg_get_DQT(jpg_infor_t *file, unsigned char * ptr,unsigned short len);
void jpg_get_JpegSize(jpg_infor_t *file);
unsigned int jpg_get_YuvSize(jpg_infor_t *file);
unsigned short jpg_get_CompFrmSOF(unsigned char * buf);
unsigned int jpg_get_SOF0(jpg_infor_t *file, unsigned char * ptr);

#ifdef __cplusplus
}
#endif
#endif
