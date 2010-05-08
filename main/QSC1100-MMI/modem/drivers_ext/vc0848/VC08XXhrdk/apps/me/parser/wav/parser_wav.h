#ifndef _ME_PARSER_WAV_
#define _ME_PARSER_WAV_

#ifdef __cplusplus
extern "C" {
#endif

#include "..\..\me_comhead.h"

//typedef unsigned int    uint32;
//typedef unsigned short  uint16;
//typedef unsigned char   uint8;

//fengyuhong add for ima-adpcm
typedef struct 
{
	ME_S32 nSamplesPerBlock;//IMA-adpcm samples number per block
	ME_S32 nBytesPerBlock; //IMA-adpcm bytes number per block
}wav_ima_adpcm;

typedef struct
{//file operate
	void *stream;	//file point
	ME_S32 (*me_read)(void *file, char* buffer, int readbytes);
	ME_S32 (*me_write)(void *file, char* buffer, int writebytes);
	ME_S32 (*me_seek)(void *file, int i_offset, int mode);
	ME_S32 (*me_tell)(void *file);
	
	ME_S32		(*read_block_func)(void *file, char* buffer, ME_S32 readbytes, ME_U32 reg, ME_U32 bit);
	ME_S32		(*write_block_func)(void *file, char* buffer, ME_S32 writebytes, ME_U32 reg, ME_U32 bit);
	ME_S32		(*get_offset_sector_func)(void *file, ME_U32 *sectorsize);
	
	ME_S32 WAV_Channels;
	ME_S32 WAV_SampleRate;
	ME_S32 WAV_BitsPerSample;
	ME_S32 WAV_TotalTime; 			//ms
	ME_S32 WAV_BitRate;

	ME_S32 WAV_FormatTag;			//data coding (0x01--PCM)
	ME_S64 WAV_DataLength;          //compress data length(bytes) for adpcm
	ME_S32 WAV_DataStartPoint;

	wav_ima_adpcm ima_adpcm; //IMA-adpcm compress parameter
}wav_infor_parser;


ME_U32	parser_wav_register(void);

#ifdef __cplusplus
}
#endif

#endif

