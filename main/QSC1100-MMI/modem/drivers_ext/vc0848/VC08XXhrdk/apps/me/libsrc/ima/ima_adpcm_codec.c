#include "ima_adpcm_codec.h"
//#include <string.h>

/* Table of index changes */
//static const signed char IndexTable[16] = {
const int IndexTable[16] = {
	    -1, -1, -1, -1, 2, 4, 6, 8,
		-1, -1, -1, -1, 2, 4, 6, 8
};
/* Quantizer step size lookup table */
const int StepSizeTable[89] = {
	     7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
		19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
		50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
		130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
		337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
		876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
		2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
		5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
		15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
};

static __inline short CLIPTOSHORT(int x)
{
	int sign;
	
	/* clip to [-32768, 32767] or [ -pow(2,15), pow(2,15)-1] */
	sign = x >> 31;
	if (sign != (x >> 15))
		//x = sign ^ ((1 << 15) - 1);
		x = sign ^ (32767);
	
	return (short)x;
}

//#ifndef __ARMCC_VERSION
short ima_adpcm_decode_sample(char code, ima_ADPCMstate *state)
{
	int step;
	int predsample;
	int diffq;
	int index;
	int n_code;
	short value;
	
	
	predsample = (int)(state->valprev);
	index = (int)state->index;
	n_code = (int)code;
	
	step = StepSizeTable[index];
	
	diffq = step >> 3;
	if( n_code & 4 )
		diffq += step;
	if( n_code & 2 )
		diffq += step >> 1;
	if( n_code & 1 )
		diffq += step >> 2;
	
	/*if( n_code & 8 )
		predsample -= diffq;
	else
		predsample += diffq;*/
	predsample += diffq;
	if(n_code & 8)
       predsample -= (diffq<<1);	
	
	/*if( predsample > 32767 )
		predsample = 32767;
	else if( predsample < -32768 )
		predsample = -32768;*/
	value=CLIPTOSHORT(predsample);
	
	index += IndexTable[n_code];
	
	if( index < 0 )
		index = 0;
	if( index > 88 )
		index = 88;
	
	//state->valprev = (short)predsample;
	state->valprev=value;
	state->index = (char)index;
	
	//return (short)predsample;	
	return value;	
}
//#endif

int ima_adpcm_decode(ima_adpcm_decodeinf *decodeinfor, char *src, int srclen, char *dst, int *dstlen, int *decodelen)
{

	char *src_buffer;		/* Input buffer pointer */
    short *dst_buffer;		/* output buffer pointer */
    char codeL,codeR;			/* Current adpcm output value */
	int length;
	ima_ADPCMstate stateL,stateR;
	int i;

	src_buffer  = (char	*)src;
	dst_buffer  = (short *)dst;

	if (srclen != decodeinfor->nBytesPerBlock) 
	{
		return 0;
	}

	*decodelen=srclen;
	*dstlen=decodeinfor->nSamplesPerBlock*decodeinfor->src_chn*2;//decoder to 16bit pcm 	

	if (decodeinfor->src_chn == 1)  //MONO
	{
		stateL.valprev = *(short*)src_buffer;
		stateL.index = *(src_buffer+2);
		src_buffer += 4;
		length = decodeinfor->nBytesPerBlock;
		length -= 4;
		*dst_buffer++=stateL.valprev;
		for (i=0; i<length; i++)
		{
			codeL = *src_buffer++;			
			*dst_buffer++ = ima_adpcm_decode_sample(codeL&0x0f, &stateL);
			*dst_buffer++ = ima_adpcm_decode_sample((codeL>>4)&0x0f, &stateL);			
		}
	}
	else                             //STEREO
	{
		*dst_buffer++=stateL.valprev = *(short*)src_buffer;
		stateL.index =*(src_buffer+2);
		src_buffer += 4;
		*dst_buffer++=stateR.valprev = *(short*)src_buffer;
		stateR.index =*(src_buffer+2);
		src_buffer += 4;
		length = decodeinfor->nBytesPerBlock;
		length -= 8;
		for (i=0; i<length; i+=8) 
		{
				codeL = *(src_buffer);
				codeR = *(src_buffer+4);
				*dst_buffer++    = ima_adpcm_decode_sample(codeL&0x0f, &stateL);
				*dst_buffer++    = ima_adpcm_decode_sample(codeR&0x0f, &stateR);
				*dst_buffer++    = ima_adpcm_decode_sample((codeL>>4)&0x0f, &stateL);
				*dst_buffer++    = ima_adpcm_decode_sample((codeR>>4)&0x0f, &stateR);
                src_buffer++;
				
				codeL = *(src_buffer);
				codeR = *(src_buffer+4);
				*dst_buffer++    = ima_adpcm_decode_sample(codeL&0x0f, &stateL);
				*dst_buffer++    = ima_adpcm_decode_sample(codeR&0x0f, &stateR);
				*dst_buffer++    = ima_adpcm_decode_sample((codeL>>4)&0x0f, &stateL);
				*dst_buffer++    = ima_adpcm_decode_sample((codeR>>4)&0x0f, &stateR);
                src_buffer++;               

				codeL = *(src_buffer);
				codeR = *(src_buffer+4);
				*dst_buffer++    = ima_adpcm_decode_sample(codeL&0x0f, &stateL);
				*dst_buffer++    = ima_adpcm_decode_sample(codeR&0x0f, &stateR);
				*dst_buffer++    = ima_adpcm_decode_sample((codeL>>4)&0x0f, &stateL);
				*dst_buffer++    = ima_adpcm_decode_sample((codeR>>4)&0x0f, &stateR);
                src_buffer++;

				codeL = *(src_buffer);
				codeR = *(src_buffer+4);
				*dst_buffer++    = ima_adpcm_decode_sample(codeL&0x0f, &stateL);
				*dst_buffer++    = ima_adpcm_decode_sample(codeR&0x0f, &stateR);
				*dst_buffer++    = ima_adpcm_decode_sample((codeL>>4)&0x0f, &stateL);
				*dst_buffer++    = ima_adpcm_decode_sample((codeR>>4)&0x0f, &stateR);
                src_buffer++;

				src_buffer+=4;
		}
	}

	return 1;
}

char ima_adpcm_encode_sample(short input, ima_ADPCMstate *state) 
{
    int sign;			/* Current adpcm sign bit */
    int delta;			/* Current adpcm output value */
    int diff;			/* Difference between val and valprev */
    int step;			/* Stepsize */
    int valpred;		/* Predicted output value */
    int vpdiff;			/* Current change to valpred */
    int index;			/* Current step change index */
	char code;

    valpred = (int)state->valprev;
    index = (int)state->index;
    step = StepSizeTable[index];
    
	/* Step 1 - compute difference with previous value */
	diff = input - valpred;
	sign = (diff < 0) ? 8 : 0;
	if ( sign ) diff = (-diff);

	/* Step 2 - Divide and clamp */
	/* Note:
	** This code *approximately* computes:
	**    delta = diff*4/step;
	**    vpdiff = (delta+0.5)*step/4;
	** but in shift step bits are dropped. The net result of this is
	** that even if you have fast mul/div hardware you cannot put it to
	** good use since the fixup would be too expensive.
	*/
	delta = 0;
	vpdiff = (step >> 3);
	
	if ( diff >= step ) {
	    delta = 4;
	    diff -= step;
	    vpdiff += step;
	}
	step >>= 1;
	if ( diff >= step  ) {
	    delta |= 2;
	    diff -= step;
	    vpdiff += step;
	}
	step >>= 1;
	if ( diff >= step ) {
	    delta |= 1;
	    vpdiff += step;
	}

	/* Step 3 - Update previous value */
	if ( sign )
	  valpred -= vpdiff;
	else
	  valpred += vpdiff;

	/* Step 4 - Clamp previous value to 16 bits */
	if ( valpred > 32767 )
	  valpred = 32767;
	else if ( valpred < -32768 )
	  valpred = -32768;

	/* Step 5 - Assemble value, update index and step values */
	delta |= sign;
	
	index += IndexTable[delta];
	if ( index < 0 ) index = 0;
	if ( index > 88 ) index = 88;
	step = StepSizeTable[index];

	/* Step 6 - Output value */
    code = (char)(delta&0x0f);
    state->valprev = (short)valpred;
    state->index = (char)index;

	return code;
}

int ima_adpcm_encode_init(ima_adpcm_encodeinf *encodeinfor,int nSampleRate,int nChannel)
{
	int i;
	char *pTemp=(char*)encodeinfor;
		
	//if(encodeinfor)
	//	memset(encodeinfor,0,sizeof(ima_adpcm_encodeinf));
	for(i=0;i<sizeof(ima_adpcm_encodeinf);i++)
		*pTemp++=0;
	
	
	if(nChannel==1) //mono channel
	{
		if((nSampleRate<=48000)&&(nSampleRate>=44100))
		{
            encodeinfor->nSamplesPerBlock=2041;
			encodeinfor->nBytesPerBlock=1024;
		}
		else if((nSampleRate<=32000)&&(nSampleRate>=22050))
		{
            encodeinfor->nSamplesPerBlock=1017;
			encodeinfor->nBytesPerBlock=512;
		}
		else if((nSampleRate<=16000)&&(nSampleRate>=8000))
		{
            encodeinfor->nSamplesPerBlock=505;
			encodeinfor->nBytesPerBlock=256;
		}
		else
			return 0;//error
	}
	else if(nChannel==2) //stereo channel
	{
		if((nSampleRate<=48000)&&(nSampleRate>=44100))
		{
            encodeinfor->nSamplesPerBlock=2041;
			encodeinfor->nBytesPerBlock=2048;
		}
		else if((nSampleRate<=32000)&&(nSampleRate>=22050))
		{
            encodeinfor->nSamplesPerBlock=1017;
			encodeinfor->nBytesPerBlock=1024;
		}
		else if((nSampleRate<=16000)&&(nSampleRate>=11025))
		{
            encodeinfor->nSamplesPerBlock=505;
			encodeinfor->nBytesPerBlock=512;
		}
		else if(nSampleRate==8000)
		{
            encodeinfor->nSamplesPerBlock=249;
			encodeinfor->nBytesPerBlock=256;			
		}
		else
			return 0;//error
	}
	else
		return 0;//error

	encodeinfor->nSampleRate=nSampleRate;
	encodeinfor->nChannel=nChannel;

	return 1;
}

void ima_adpcm_encode_release(ima_adpcm_encodeinf *encodeinfor)
{
	//int i;
	//char *pTemp=(char*)encodeinfor;
	
	if(encodeinfor)
	   VM_IMA_memset((unsigned char*)encodeinfor,0,sizeof(ima_adpcm_encodeinf));
	//for(i=0;i<sizeof(ima_adpcm_encodeinf);i++)
	//	*pTemp++=0;	
}

int ima_adpcm_encode(ima_adpcm_encodeinf *encodeinfor, char *src, int srclen, char *dst, int *dstlen, int *encodelen)
{
	short *src_buffer;		/* Input buffer pointer */
    char  *dst_buffer;		/* output buffer pointer */
	char code;
	int length;
	ima_ADPCMstate stateL,stateR;
	int i;
	
	src_buffer  = (short *)src;
	dst_buffer  = (char *)dst;
	
	if (srclen != (encodeinfor->nSamplesPerBlock*encodeinfor->nChannel*2)) 
	{
		return 0;
	}
	
	*encodelen=srclen;
	*dstlen=encodeinfor->nBytesPerBlock;
	
	if (encodeinfor->nChannel == 1)  //MONO
	{
		stateL.valprev = *src_buffer++;
		stateL.index = encodeinfor->l_index;
		stateL.reserved=0;
		VM_IMA_memcpy(dst_buffer,(char*)&stateL,sizeof(ima_ADPCMstate));
		dst_buffer += sizeof(ima_ADPCMstate);
		length = encodeinfor->nSamplesPerBlock-1;//exclude the first sample in the head
		
		for (i=0; i<(length>>1); i++)
		{
			code=ima_adpcm_encode_sample(*src_buffer++,&stateL);
			*dst_buffer++=(ima_adpcm_encode_sample(*src_buffer++,&stateL)<<4)|code;
		}
        encodeinfor->l_index=stateL.index; 
	}
	else                             //STEREO
	{
		stateL.valprev = *src_buffer++;
		stateL.index = encodeinfor->l_index;
		stateL.reserved=0;
		VM_IMA_memcpy(dst_buffer,(char*)&stateL,sizeof(ima_ADPCMstate));
		dst_buffer += sizeof(ima_ADPCMstate);

		stateR.valprev = *src_buffer++;
		stateR.index = encodeinfor->r_index;
		stateR.reserved=0;
		VM_IMA_memcpy(dst_buffer,(char*)&stateR,sizeof(ima_ADPCMstate));
		dst_buffer += sizeof(ima_ADPCMstate);

		length = encodeinfor->nSamplesPerBlock-1;//exclude the first sample in the head
		
		for (i=0; i<(length>>3); i++) 
		{
            //encode the left channel
        	code=ima_adpcm_encode_sample(src_buffer[0],&stateL);
			*dst_buffer++=(ima_adpcm_encode_sample(src_buffer[2],&stateL)<<4)|code;
			code=ima_adpcm_encode_sample(src_buffer[4],&stateL);
			*dst_buffer++=(ima_adpcm_encode_sample(src_buffer[6],&stateL)<<4)|code;
			code=ima_adpcm_encode_sample(src_buffer[8],&stateL);
			*dst_buffer++=(ima_adpcm_encode_sample(src_buffer[10],&stateL)<<4)|code;
			code=ima_adpcm_encode_sample(src_buffer[12],&stateL);
			*dst_buffer++=(ima_adpcm_encode_sample(src_buffer[14],&stateL)<<4)|code;
			
			//encoder right channel
			code=ima_adpcm_encode_sample(src_buffer[1],&stateR);
			*dst_buffer++=(ima_adpcm_encode_sample(src_buffer[3],&stateR)<<4)|code;
			code=ima_adpcm_encode_sample(src_buffer[5],&stateR);
			*dst_buffer++=(ima_adpcm_encode_sample(src_buffer[7],&stateR)<<4)|code;
			code=ima_adpcm_encode_sample(src_buffer[9],&stateR);
			*dst_buffer++=(ima_adpcm_encode_sample(src_buffer[11],&stateR)<<4)|code;
			code=ima_adpcm_encode_sample(src_buffer[13],&stateR);
			*dst_buffer++=(ima_adpcm_encode_sample(src_buffer[15],&stateR)<<4)|code;
			
			src_buffer+=16;
		}
		encodeinfor->l_index=stateL.index; 
        encodeinfor->r_index=stateR.index; 		
	}
	
	return 1;
}

int ima_adpcm_encode_multisamples(short *pPcmData, char *pAdpcmData, int samplelen,ima_ADPCMstate *state)
{
    int i;
	char code;
	int remain;
	int adpcmlen;
	
	for(i=0;i<(samplelen>>1);i++)
	{
		code=ima_adpcm_encode_sample(*pPcmData++,state); 
		*pAdpcmData++=(ima_adpcm_encode_sample(*pPcmData++,state)<<4)|code;		
	}
	
	remain=samplelen-((samplelen>>1)<<1);//0 or 1
	if(remain)  *pAdpcmData++=ima_adpcm_encode_sample(*pPcmData++,state);
    adpcmlen=(samplelen>>1)+remain;	
	
	return adpcmlen;//adpcm byte number
}

int ima_adpcm_decode_multisamples(short *pPcmData, char *pAdpcmData, int adpcmlen,ima_ADPCMstate *state)
{
    int i;
	char code;
	int samplelen;
	
	for(i=0;i<adpcmlen;i++)
	{
		code = *pAdpcmData++;			
		*pPcmData++ = ima_adpcm_decode_sample(code&0x0f, state);
		*pPcmData++ = ima_adpcm_decode_sample((code>>4)&0x0f, state);			
	}
	samplelen=adpcmlen<<1;
	
	return samplelen;//pcm sample number
}














