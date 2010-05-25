/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	media_utils.h
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2007/04/02
* Last modify : Amanda Deng 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/
#ifndef _MEDIA_SYSTEM_UTILS_H_
#define _MEDIA_SYSTEM_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "media_comhead.h"			//this file add by amanda deng

#define mp_printf	MMDPrintf

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
	  base function define(include system function and debug message function)
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
#ifdef _WIN32
#define 		MEDIA_FOPEN_READ_BINARY 			"rb"
#define 		MEDIA_FOPEN_WRITE_BINARY 		"wb"
#else
#define 		MEDIA_FOPEN_READ_BINARY 			"r"
#define 		MEDIA_FOPEN_WRITE_BINARY 		"w"
#endif

#define 		MEDIA_PLAYER_MALLOC_DEBUG		0
#define 		MEDIA_PLAYER_PRINT				0
#define 		MEDIA_PLAYER_WARNING			0
#define 		MEDIA_PLAYER_DEBUG				0

#define 		mp_error(str)			{mp_printf("\nMP_ERROR:	");mp_printf str;}		


#if	MEDIA_PLAYER_MALLOC_DEBUG
#define 		mp_malloc_print(str)		{mp_printf("\nMP_MALLOC:	");mp_printf str;}
#define 		mp_free_print(str)		{mp_printf("\nMP_FREE:	");mp_printf str;}
#define 		mp_malloc_h2d_print(str)		{mp_printf("\nMP_MALLOC_H2D:	");mp_printf str;}
#define 		mp_free_h2d_print(str)		{mp_printf("\nMP_FREE_H2D:	");mp_printf str;}
#else
#define 		mp_malloc_print(str)		
#define 		mp_free_print(str)		
#define 		mp_malloc_h2d_print(str)		
#define 		mp_free_h2d_print(str)		
#endif

#if MEDIA_PLAYER_PRINT
#define 		mp_print(str)			{mp_printf("\nMP_MESSAGE:	");mp_printf str;}
#else
#define 		mp_print(str)			
#endif

#if MEDIA_PLAYER_WARNING
#define 		mp_warning(str)			{mp_printf("\nMP_WARNING:	");mp_printf str;}
#else
#define 		mp_warning(str)	
#endif

#if MEDIA_PLAYER_DEBUG
#define 		mp_debug(str)			{mp_printf("\nMP_DEBUG:	");mp_printf str;}
#else
#define 		mp_debug(str)
#endif

void*		mp_malloc(unsigned int size);
void*		mp_calloc(unsigned int num, unsigned int size);
void			mp_free(void* buf);
char* 		mp_memcpy(char*  str_dst, const char* str_src, unsigned int len);
void			mp_memset(unsigned char* buf, unsigned char val, unsigned int size);
unsigned int	mp_strlen(unsigned char*  str);

void 		mp_memory_debug(void);
void 		mp_memory_clear(void);
void 		mp_memory_printf( char* buf, ... );

int 			mp_id_match(char *input, char *output);

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
							file operation function define(2007.5.16 )
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
#define 		MEDIA_VIM_MEDIA_PLAYER_FILE_DEBUG		0

typedef struct _MEMORY_FILE_  MEMORY_FILE;
struct _MEMORY_FILE_{
	char* 	FileAddress;
	int 		FileReadCur;
	int 		FileWriteCur;
	int 		FileTotalLength;
};

#if 0
int memory_fileread(void* file, char* buffer, int count);
int memory_filewrite(void* file, char* buffer, int count);
int memory_fileseek(void* file, int offset, int mode);
int memory_filetell(void* file);
int memory_fileeof(void* file);
int memory_fileclose(void* file);
int memory_filedelete(void *address);
void*memory_fileopen(void *address, void *size);
#endif

int filesys_fileread(void* file, char* buffer, int count);
int filesys_filewrite(void* file, char* buffer, int count);
int filesys_fileseek(void* file, int offset, int mode);
int filesys_filetell(void* file);
int filesys_fileeof(void* file);
int filesys_fileclose(void* file);
int filesys_filedelete(void *filename);
void*filesys_fileopen(void *filename,void *mode);

extern fs_callbacks  g_filesys_file;
extern fs_callbacks  g_memory_file;
#if ME_VERSION2_FOURHANDLE
extern fs_callbacks	 g_memory_baseband_file;
#endif
extern fs_callbacks	 g_filesys_baseband_file;

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
							function define for media player/recorder(2007.8.7 )
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
typedef struct _MEDIA_FIFO_  MEDIA_FIFO;
struct _MEDIA_FIFO_{
	char* 	address;
	int 		size;
	int 		pwrite;
	int 		pread;
};

void mp_writetofifo(MEDIA_FIFO*  dst, const char* src, unsigned int len);
int 	property_type_is(char *property_type, char *type);

MP_U32 media_get_systime_ticks_10ms(void);
#define get_system_time_ticks  media_get_systime_ticks_10ms


/////////////////////////////////////////////////////////////
/*	       	memory interfaces added by dnn 2008/4/11		     */
/////////////////////////////////////////////////////////////
#define ADD_AUDIO_CODE		0
#if ME_VERSION2_FOURHANDLE
void* mp_malloc_h2d(unsigned int size);
void	mp_free_h2d(void*  buf);
MP_U32 mp_memcpy_h2d(MP_PS08 str_dst, MP_PCS08 str_src, MP_U32 len);
MP_U32 mp_memcpy_d2h(MP_PS08 str_dst, MP_PCS08 str_src, MP_U32 len);
MP_U32 mp_memset_h2d(char* buf, char val, unsigned int size);
int filesys_block_filewrite(void* file, char* buffer, int count,unsigned int reg, unsigned int bit);
#endif


#ifdef __cplusplus
}
#endif

#endif /*_MEDIA_PLAYER_H_AMANDA_*/
