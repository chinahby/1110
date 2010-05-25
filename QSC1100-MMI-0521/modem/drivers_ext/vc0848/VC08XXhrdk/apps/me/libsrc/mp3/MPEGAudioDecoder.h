/******************************************************************************
               (C) copyright (2006)  Vimicro  All Rights Reserved
 ******************************************************************************/

/******************************************************************************
 File:	mpegaudiodecoder.h
 Date:	2006-6-12   11:20
 ------------------------------------------------------------------------------
 Description:
	
 ******************************************************************************/

/******************************************************************************
                      C H A N G E   R E C O R D

 #   Date		     Author			Description
 --  ----------	 ------------------  --------------------------------------
 01  2006-6-12	Xiaocheng Wang		Created this file
 ******************************************************************************/

# ifndef _MPEG_AUDIO_DECODER_H_
# define _MPEG_AUDIO_DECODER_H_

#define MAX_SAMPLE_RATE		48000
#define MAX_CHANNLE_NUMBER	2
#define MAX_FRAME_SAMPLES	1152
#define MAX_FRAME_SIZE		1736	// the max 384kbps @ 32kHz framesize 

#define MPEG_AUDIO_LAYER_I		1
#define MPEG_AUDIO_LAYER_II		2
#define MPEG_AUDIO_LAYER_III	3

#define MPEG_AUDIO_VERSION_2_5	0
#define MPEG_AUDIO_VERSION_1	1
#define MPEG_AUDIO_VERSION_2	2

#define MPEG_LAYER1_FRAMELEN	384
#define MPEG_LAYER2_FRAMELEN	1152
#define MPEG1_LAYER3_FRAMELEN	1152
#define MPEG2_LAYER3_FRAMELEN	576

typedef void* HMPEGAudioDecoder;

typedef struct
{
	int nSampleRate;//Hz
	int nBitRate;//kbps
	int nChannels;
	int nLayer;
	int nVersion;
	int nFarmeSize;
}	MPEGAudioHeaderInfo;

enum errcode{
	ERR_OK					=	0,
	ERR_INDATA_UNDERFLOW	=   -1,
	ERR_MAINDATA_UNDERFLOW	=   -2,
	ERR_FREE_BITRATE_SYNC	=   -3,
	ERR_OUT_OF_MEMORY		=	-4,
	ERR_NULL_POINTER		=	-5,
	ERR_INVALID_FRAMEHEADER =	-6,
	ERR_INVALID_SIDEINFO	=   -7,
	ERR_INVALID_SCALEFACT	=   -8,
	ERR_INVALID_HUFFCODES	=   -9,
	ERR_INVALID_DEQUANTIZE	=   -10,
	ERR_INVALID_IMDCT		=   -11,
	ERR_INVALID_SUBBAND		=   -12,
	ERR_SYNC_FAILED			=   -13,
	
	ERR_UNKNOWN_ERR			=	-9999
};


/******************************************************************************
 -Func : MPEGAudioDecoder_HandlerSize

 -Desc : Return the size of the structure for allocate memory

 -Ret  : int 

*******************************************************************************/
int MPEGAudioDecoder_HandlerSize(void);

/******************************************************************************
 -Func : MPEGAudioDecoder_Init

 -Desc : initialize mp3 decoder handle and the local decoder handles

 -Ret  : int 

 -Arg  : hMPADecoder : decoder handle
*******************************************************************************/
int MPEGAudioDecoder_Init(HMPEGAudioDecoder hMPADecoder);

/******************************************************************************
 -Func : MPEGAudioDecoder_DecodeFrame

 -Desc : Decode one mpeg audio frame and output interleaved 16bit pcm data.

 -Ret  : int 

 -Arg  : hDecoder	: decoder handle
 -Arg  : pBitStream	: bitstream buffer
 -Arg  : pcmbuffer  : pcm buffer
 -Arg  : length		: length of bitstream buffer (bytes) and return the number of actuall used bytes
*******************************************************************************/
int MPEGAudioDecoder_DecodeFrame(HMPEGAudioDecoder hDecoder, 
								 unsigned char *pBitStream, 
								 void* pcmbuffer, 
								 int *length);

/******************************************************************************
 -Func : MPEGAudioDecoder_HeaderInfo

 -Desc : Decode MPEG audio frame header and get information.

 -Ret  : int 

 -Arg  : hDecoder	: decoder handle
 -Arg  : pBitStream	: bitstream buffer
 -Arg  : length		: length of bitstream buffer (bytes) and return the number of actuall used bytes
*******************************************************************************/
int MPEGAudioDecoder_HeaderInfo(MPEGAudioHeaderInfo *header, 
								unsigned char *pBitStream, 
								int *length);

#endif	// _MPEG_AUDIO_DECODER_H_

