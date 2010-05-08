/*=============================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	userdef.c  
* User for : 	mp4 demux mux, 
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/10/04
* Last modify : Amanda Deng 2007-xx-xx     
*
* relase history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=============================================================================*/
#include "mmd_data_type.h"

//---------------------------error information define 2006.10.31---------------------------
#define MP4_MUX_DEMUX_BASE_RTN				0x00010000
#define MP4_MUX_DEMUX_RET_NOERR				0x00000000

#define MP4_MUX_DEMUX_RET_ERROR 				MP4_MUX_DEMUX_BASE_RTN+0x0000FFFF
#define MP4_MUX_DEMUX_OPEN_FILE_ERROR 		MP4_MUX_DEMUX_BASE_RTN+0x0000FFFE
#define MP4_MUX_DEMUX_FILE_POINT_OVERFLOW 	MP4_MUX_DEMUX_BASE_RTN+0x0000FFFD
#define MP4_MUX_DEMUX_MALLOC_ERROR 			MP4_MUX_DEMUX_BASE_RTN+0x0000FFFC
#define MP4_MUX_DEMUX_HAVE_NO_AUDIO 		MP4_MUX_DEMUX_BASE_RTN+0x0000FFFB
#define MP4_MUX_DEMUX_HAVE_NO_VIDEO 		MP4_MUX_DEMUX_BASE_RTN+0x0000FFFA
#define MP4_MUX_DEMUX_MEMORY_LESS_DATA 	MP4_MUX_DEMUX_BASE_RTN+0x0000FFF9
#define MP4_MUX_DEMUX_INVALID_PARAMETER 	MP4_MUX_DEMUX_BASE_RTN+0x0000FFF8
#define MP4_MUX_DEMUX_INVALID_INFOR 			MP4_MUX_DEMUX_BASE_RTN+0x0000FFF7

#define MP4_DEMUX_READ_FILE_ERROR			MP4_MUX_DEMUX_BASE_RTN+0x00000001

#define MP4_DEMUX_END_OF_AUDIO 				MP4_MUX_DEMUX_BASE_RTN+0x00000002
#define MP4_DEMUX_END_OF_VIDEO 				MP4_MUX_DEMUX_BASE_RTN+0x00000003
#define MP4_DEMUX_LAST_OF_AUDIO 				MP4_MUX_DEMUX_BASE_RTN+0x00000004
#define MP4_DEMUX_LAST_OF_VIDEO 				MP4_MUX_DEMUX_BASE_RTN+0x00000005

#define MP4_DEMUX_ATOM_TYPE_ERROR			MP4_MUX_DEMUX_BASE_RTN+0x00000010
#define MP4_DEMUX_ATOM_SIZE_ERROR			MP4_MUX_DEMUX_BASE_RTN+0x00000011

#define MP4_DEMUX_ESDS_NO_DECODE_CONFIG	MP4_MUX_DEMUX_BASE_RTN+0x00000100
#define MP4_DEMUX_ESDS_DESCR_TAG_ERROR		MP4_MUX_DEMUX_BASE_RTN+0x00000101
#define MP4_DEMUX_ESDS_DESCR_LEN_ERROR		MP4_MUX_DEMUX_BASE_RTN+0x00000102
#define MP4_DEMUX_ESDS_CONFI_TAG_ERROR		MP4_MUX_DEMUX_BASE_RTN+0x00000103
#define MP4_DEMUX_ESDS_SPECI_TAG_ERROR		MP4_MUX_DEMUX_BASE_RTN+0x00000104

#define MP4_MUX_WRITE_FILE_ERROR				MP4_MUX_DEMUX_BASE_RTN+0x00000003
#define MP4_MUX_INVALIDPARAM					MP4_MUX_DEMUX_BASE_RTN+0x00000004

#define MP4_THIS_VERSION_CANNOT_DEMUX		MP4_MUX_DEMUX_BASE_RTN+0x00008000


//---------------------------end error information define 2006.10.31---------------------------

/*=============================================================================
	from here add for limited memory <1024*30 bytes 2006.10.25
=============================================================================*/
#define PRELOAD_BUFFER_SIZE 				1024

#define STTS_MAX_ENTRIES 					128//256//  				/*(MEMORY SIZE = 256*8 BYTES=2k)*/
#define STSS_MAX_ENTRIES 					256//512//				/*(MEMORY SIZE = 512*4 BYTES=2k)*/
#define STSC_MAX_ENTRIES 					128//256// 				/*(MEMORY SIZE = 256*12 BYTES=3k)*/
#define STCO_MAX_ENTRIES 					128//256// 				/*(MEMORY SIZE = 256*4 BYTES=1k)*/
#define STSZ_MAX_ENTRIES 					128//256// 				/*(MEMORY SIZE = 256*4 BYTES=1K)*/

/*=============================================================================
	from here add for 820 media player control(memory and frame counter control)
=============================================================================*/
#define MP4_AUDIO_MAX_BUFFER_SIZE 		1024*32			// at least can load 64 frames aac in buffer((max 1024 bytes/f)),  also at least load 2048 frames amr in buffer 	
#define MP4_VIDEO_MAX_BUFFER_SIZE 		1024*512		// at least can load 5 frames 320X240 video in buffer(max 20k/f)

#define MP4_AUDIO_MAX_FRAME_READ 		32				//max read 240 frames amr / 128 frames aac audio once time( 48000k aac means read data 128/48 s)			
#define MP4_AUDIO_MIN_FRAME_READ 		1//24				//mix read 120 frames amr / 64 frames aac audio once time( 48000k aac means read data 64/48 s)		

#define MP4_VIDEO_MAX_FRAME_READ 		10				//max read 30 frames video once time( 30fps means read data 1000 ms)		
#define MP4_VIDEO_MIN_FRAME_READ 		1//10				//min read 10 frames video once time( 30fps means read data 300 ms)			
		
#define MP4_IN_820_SYN_FRAME_TICKS		16//16//				//each 15 frame video have 1 syn frame, such as 1,16,31,46...is syn frame
#define MP4_IN_820_MEMORY_LIMITED		1				//=0, load all table to memory, =1, only can load part table to memory
#define MP4_IN_820_QUICK_GET_FRAMESIZE	0				//=0,find the max frame size of the file, =1, audio max farme size = 1024
/*=============================================================================
	from here add for message debug 2007/04/18
=============================================================================*/
#define 	MP4_DEMUX_MUX_MALLOC			0
#define 	MP4_DEMUX_MUX_MESSAGE			0
#define 	MP4_DEMUX_MUX_DEBUG				0
#define 	MP4_DEMUX_MUX_FILE_DEBUG		0


/*=============================================================================
	from here add for on windows testing . Amanda Deng
=============================================================================*/
#define mp4_printf MMDPrintf

#ifdef _WIN32
#define 	MP4_DEMUX_FOPEN_READ_BINARY 	"rb"
#define 	MP4_MUX_FOPEN_WRITE_BINARY 		"wb+"
#define 	MP4_IN_820_DECODE_HARDWARE		1
#define 	MP4_IN_820_ENCODE_HARDWARE		0
#define 	MP4_IN_820_MEMORY_LIMITED		0				
#define 	MP4_IN_820_QUICK_GET_FRAMESIZE	0				

#else	//#else _ARM

#define 	MP4_DEMUX_FOPEN_READ_BINARY 	"r"
#define 	MP4_MUX_FOPEN_WRITE_BINARY 		"w+"
/*=============================================================================
	from here add for on 820 testing . Amanda Deng
=============================================================================*/
#define 	MP4_IN_820_DECODE_HARDWARE		1
#define 	MP4_IN_820_ENCODE_HARDWARE		1
/*extern void* 	SD_Malloc(uint32 size);
extern void 	SD_Free(void* pBuf);*/
extern int32 filesys_fileread(void* file, char* buffer, int32 count);
extern int32 filesys_filewrite(void* file, char* buffer, int32 count);
extern int32 filesys_fileseek(void* file, int32 offset, int32 mode);
extern int32 filesys_filetell(void* file);
extern int32 filesys_fileeof(void* file);
extern int32 filesys_fileclose(void* file);
extern int32 filesys_filedelete(void *filename);
extern void*filesys_fileopen(void *filename,void *mode);

//#define 	MP4_IN_820_DECODE_HARDWARE		1
//#define 	MP4_IN_820_ENCODE_HARDWARE		1
//#define 	MP4_IN_820_SYN_FRAME_TICKS		16				//each 15 frame video have 1 syn frame, such as 1,16,31,46...is syn frame
//#define 	MP4_IN_820_MEMORY_LIMITED		1				//=0, load all table to memory, =1, only can load part table to memory
//#define 	MP4_IN_820_QUICK_GET_FRAMESIZE	0				//=0,find the max frame size of the file, =1, audio max farme size = 1024
/*=============================================================================
	from here add for on 578 testing . Amanda Deng
=============================================================================*/
#endif	

#define 	MP4_DEMUX_ERR(str)				{mp4_printf("\nMP4LIB_ERROR:	");mp4_printf str;}
#define 	MP4_DEMUX_WARNING(str)		{mp4_printf("\nMP4LIB_WARNING:	");mp4_printf str;}

#if	MP4_DEMUX_MUX_MALLOC
#define 	MP4_MALLOC_MSG(str)			{mp4_printf("\nMP4LIB_MALLOC: ");mp4_printf str;}
#define 	MP4_FREE_MSG(str)				{mp4_printf("\nMP4LIB_FREE:      ");mp4_printf str;}
#else
#define 	MP4_MALLOC_MSG(str)	
#define 	MP4_FREE_MSG(str)	
#endif

#if MP4_DEMUX_MUX_MESSAGE
#define 	MP4_DEMUX_MSG(str)			{mp4_printf("\nMP4LIB_MESSAGE:");mp4_printf str;}
#else
#define 	MP4_DEMUX_MSG(str)
#endif

#define	MP4_SPRINTF	sprintf
#if MP4_DEMUX_MUX_DEBUG
#define 	MP4_DEMUX_DBG(str)			{mp4_printf("\nMP4LIB_DEBUG:");mp4_printf str;}
#else
#define 	MP4_DEMUX_DBG(str)
#endif

#if MP4_DEMUX_MUX_FILE_DEBUG
#define 	MP4_DEMUX_FILE(str)			{mp4_printf("\nMP4LIB_FILE:");mp4_printf str;}
#else
#define 	MP4_DEMUX_FILE(str)
#endif				

//by dnn 2008/4/11
#define MP4LIB_VERSION2_FOREHANDLE	1
#if MP4LIB_VERSION2_FOREHANDLE
#define MAX_FILE_HANDLE_NUM		5
extern unsigned int me_memcpy_h2d(char * str_dst, const char * str_src, unsigned int len);
extern unsigned int me_memset_h2d(unsigned int addr, unsigned size);
extern uint32 MMD_BIU_Get_Device_Status(uint32 index, uint32* pstatus);
uint32 MMD_SYS_FillMemData(uint32 addr, uint32 size,uint32 data);

void mp4_free_h2d(void*  buf);
void* mp4_malloc_h2d(unsigned int size);
uint32 mp4_memcpy_h2d(char * str_dst, const char * str_src, unsigned int len);
uint32 mp4_memcpy_d2h(char * str_dst, const char * str_src, unsigned int len);
uint32 mp4_filewrite_core(void* file, char* buffer, uint32 count,unsigned int reg, unsigned int bit);
uint32 mp4_write_data_core(mp4_infor_t *file, char *data, uint32 size,unsigned int reg, unsigned int bit);

#define 	MP4_MEMCPY_H2D	me_memcpy_h2d
#define   MP4_MEMSET_H2D	me_memset_h2d
#endif
