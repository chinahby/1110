#ifndef AAC_FILEDECODE_H
#define AAC_FILEDECODE_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdlib.h>
#include "..\..\me_comhead.h"
#include "..\..\libsrc\aac\aacparselib.h"
#if defined (__arm) && defined (__ARMCC_VERSION)
#define ARMULATE_MUL_FACT	1
#define MAX_FRAMES	-1 
#else
#define ARMULATE_MUL_FACT	1
#define MAX_FRAMES	-1
#endif

#define READBUF_SIZE	(2 * AAC_MAINBUF_SIZE * AAC_MAX_NCHANS)	/* pick something big enough to hold a bunch of frames */
#define AAC_READBUF_SIZE 4096
//add
#define PARSER_TOTALTIME_FRAME  5000//2500 //越大测试音频文件的长度越准确，代表总共parser的音频的帧数

enum
{
	MEERR_AAC_NONE =0,
	MEERR_AAC_READ_AUDIO=1
};

typedef struct
{
	void *stream;/*文件指针*/
	int (*read_func)(void *file, char* buffer, int readbytes);
	int (*write_func)(void *file, char* buffer, int writebytes);
	int (*seek_func)(void *file, int i_offset, int mode);
	int (*tell_func)(void *file);
	unsigned int		aac_format;		/*ADIF or ADTS*/
	unsigned int		a_isvalid;		/* 1: have audio, 0 no audio */
	unsigned int		a_bitrate;
	unsigned int		a_chn;			/* 1: mono, 2: stereo */
	unsigned int		a_smplrate;		/* sample rate */
	unsigned int		a_totaltime;
	unsigned int		file_length;
	unsigned int		file_end;
//	char *	readBuf;
//	unsigned int		a_smpllen ;     /* bytes of one audio frame*/
//	unsigned int		a_totalframe;   /* total frames of audio */
//	unsigned int		a_format;
//	unsigned int		a_frmsize;		/* frame size */

} aac_infor_t;
unsigned int aac_readaudio(aac_infor_t *file,char *audio_buffer, unsigned int *readbytes,unsigned int bufferlength);
aac_infor_t* aac_init(void);
unsigned int aac_close(aac_infor_t *file);
unsigned int aac_read_info(aac_infor_t *file);

#ifdef __cplusplus 
}
#endif
#endif