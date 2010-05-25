
#ifndef _IMA_ADPCM_CODEC_H
#define _IMA_ADPCM_CODEC_H

#include "mmd_user_depend.h"
#ifdef __cplusplus
extern "C" {
#endif
	
#define VM_IMA_malloc MMD_Malloc
#define VM_IMA_memset MMD_MemSet
#define VM_IMA_free   MMD_Free
#define VM_IMA_memcpy MMD_MemCpy
#define VM_IMA_memmove me_memmove
	
	typedef struct
	{
		int		 src_chn;			/* 1: mono, 2: stereo */
		int      nSamplesPerBlock;
		int      nBytesPerBlock;
	}ima_adpcm_decodeinf;
	/*
	*  decode one ima-adpcm block (input data include block header)
	*  decodeinfor: decode argument
	*  src: input adpcm buffer
	*  srclen: input buffer length (byte)
	*  dst: outpu pcm buffer
	*  dstlen: output buffer length (byte)
	*  decodelen: decoder cosuming bitstream length (byte)
	*  Return :  0: error    1: success
	*
	*/
	int ima_adpcm_decode(ima_adpcm_decodeinf *decodeinfor, char *src, int srclen, char *dst, int *dstlen, int *decodelen);
	
	typedef struct 
	{
		int nSampleRate;
		int nChannel;
		int nSamplesPerBlock;
		int nBytesPerBlock;
		int l_index;
		int r_index;
	}ima_adpcm_encodeinf;
	/*
	*	init ima-adpcm encoder 
	*  encodeinfor: encoder argument
	*  nSampleRate: sample rate of pcm data
	*  nChannel: channle number of pcm data
	*/
	int ima_adpcm_encode_init(ima_adpcm_encodeinf *encodeinfor,int nSampleRate,int nChannel);
	/* encode one ima-adpcm block (output data include block header)
	* encodeinfor: encoder argument
	* src: input pcm buffer
	* srclen: input buffer lenght(byte)
	* dst: output adpcm buffer
	* dstlen: output buffer legnth(byte) 
	* encodelen: encoder consuming pcm length (byte)
	* Return :  0: error    1: success
	*/
	int ima_adpcm_encode(ima_adpcm_encodeinf *encodeinfor, char *src, int srclen, char *dst, int *dstlen, int *encodelen);
	/*
	*	init ima-adpcm encoder 
	*  encodeinfor: encoder argument
	*  nSampleRate: sample rate of pcm data
	*  nChannel: channle number of pcm data
	*/
	void ima_adpcm_encode_release(ima_adpcm_encodeinf *encodeinfor);
	
	typedef struct
	{
		short valprev;/* Predicted sample */
		char index;/* Index into step size table */
		char reserved;
	}ima_ADPCMstate;
	/*
	*	decode one ima-adpcm sample( input in low 4bit of code , output 16bit)
	*  code: adpcm code byte( low 4bit)
	*  state: adpcm state  
	*  Return : decoding pcm sample 
	*/
	short ima_adpcm_decode_sample(char code, ima_ADPCMstate *state); //code must save in low 4 bits
	
    /*  encoder one ima-adpcm sample
	 *  input: input 16bit pcm sample
	 *	state: adpcm state
	 *  Return : encoding adpcm code (low 4bit)
	*/
	char ima_adpcm_encode_sample(short input, ima_ADPCMstate *state); 
	
   /*
	*  decode multiple ima-adpcm samples
	*  pPCMData: pcm data buffer
	*  pAdpcmData: adpcm data buffer
	*  adpcmlen: byte number of adpcm data
	*  state: adpcm state
	*  Return : sample number of pcm data (==(adpcmlen<<1))  
	*/
	int ima_adpcm_decode_multisamples(short *pPcmData, char *pAdpcmData, int adpcmlen,ima_ADPCMstate *state);
   /*
	*  encode multiple ima-adpcm samples
	*  pPCMData: pcm data buffer
	*  pAdpcmData: adpcm data buffer
	*  samplelen: sample number of pcm data
	*  state: adpcm state
	*  Return : byte number of adpcm data (==(samplelen>>1)+(samplelen-((samplelen>>1)<<1)))  
	*/
	int ima_adpcm_encode_multisamples(short *pPcmData, char *pAdpcmData, int samplelen,ima_ADPCMstate *state);

		
#ifdef __cplusplus
}
#endif

#endif
