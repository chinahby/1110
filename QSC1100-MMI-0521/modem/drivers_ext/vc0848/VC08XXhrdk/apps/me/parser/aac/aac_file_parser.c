	
#include "aac_file_parser.h"

aac_infor_t* aac_init(void) 
{
	aac_infor_t *aac_file = aac_parser_malloc(sizeof(aac_infor_t));
	AAC_PARSER_TRACE_MSG();	//add by amanda deng for testing memory lost
	return aac_file; 
}

unsigned int aac_close(aac_infor_t *file)
{
	aac_parser_free(file);
	AAC_PARSER_TRACE_MSG();	//add by amanda deng for testing memory lost
	return MEERR_AAC_NONE;
} 

unsigned int aac_read_info(aac_infor_t *file)
{
	HAACParser hAACParser;
	unsigned int nRead,framecnt;
	long nFileSize,cur_filebytes;
	int bytesLeft;
	int err;
	double audioFrameSecs,audioSecs,rate;
	char *readBuf;
	char *readPtr;
	
	
//total bytes of stream file
	file->seek_func(file->stream, 0, SEEK_END);
	nFileSize = file->tell_func(file->stream);
	file->file_length = nFileSize;
	file->file_end = 0;
	file->seek_func(file->stream, 0, SEEK_SET);
	
//get stream file information	
	readBuf = (char *)me_malloc(AAC_READBUF_SIZE);
	readPtr = readBuf;
	aac_readaudio(file,readPtr,&nRead,AAC_READBUF_SIZE);
	bytesLeft = nRead;
	hAACParser = (HAACParser)AACParserLib_Create();
	err = AACParserLib_Open(hAACParser, (unsigned char **)&readPtr, &bytesLeft);
	if(err)
	{
		file->seek_func(file->stream, 0, SEEK_SET);
		me_free(readBuf);
		return err;
	}
//get aac_infor_t information
	file->aac_format = hAACParser->format;
	file->a_bitrate = hAACParser->bitRate;
	file->a_chn = hAACParser->nChans;
	file->a_smplrate = hAACParser->sampRate;
//get total time of aac file
	if(file->aac_format == AAC_FF_ADIF)
	{//format is ADIF
		me_printf("aac_format: ADIF\n");
		//get totaltime. bitstream type: 0 = CBR(bits/sec)		1=VBR(peak bits/frame).if bitstream type is VBR, the total time will be small than the truth.
		file->a_totaltime = nFileSize*8/hAACParser->fhADIF.bitRate;//bitRate: CBR = bits/sec
		file->a_totaltime*=1000;	//totaltime(s)->totaltime(ms)
		file->seek_func(file->stream, 0, SEEK_SET);
		AACParserLib_Release(hAACParser);
		me_free(readBuf);
		return MEERR_AAC_NONE;
	}
	else
	{//format is ADTS
		me_printf("aac_format: ADTS\n");
		nFileSize-=(nRead-bytesLeft);//ÐÞÕýÎÄ¼þ³¤¶È£¬È¥µôID3Í
		for(;;)
		{
			err = AACParserLib_ADTSParsing(hAACParser,(unsigned char **)&readPtr, (int *)&bytesLeft);
			if(err!=0)
			{
				me_memcpy(readBuf,readPtr,bytesLeft);
				aac_readaudio(file,readBuf+bytesLeft,(unsigned int *)&nRead,AAC_READBUF_SIZE-bytesLeft);
				if(nRead == 0) 
					break;
				readPtr = readBuf;
				bytesLeft = AAC_READBUF_SIZE;
			}
			if(hAACParser->frameCount>PARSER_TOTALTIME_FRAME)
			{
				break;
			}
		}
		//time of a frame	
		audioFrameSecs = ((float)hAACParser->outputSamps) / ((float)hAACParser->sampRate * hAACParser->nChans);	//the length of frame don't change
		cur_filebytes=hAACParser->nDataCount;
		framecnt=hAACParser->frameCount;
		audioSecs = (float)framecnt * audioFrameSecs; //s
		rate	= (double)cur_filebytes/audioSecs; //Byte/s
		file->a_bitrate=(unsigned int)rate*8;//bit/s
		file->a_totaltime = (double)nFileSize*1000/rate; //ms
		file->seek_func(file->stream, 0, SEEK_SET);
		AACParserLib_Release(hAACParser);	
		me_free(readBuf);
		return MEERR_AAC_NONE;
	}
}

unsigned int aac_readaudio(aac_infor_t *file,char *audio_buffer, unsigned int *readbytes,unsigned int bufferlength)
{
	
	*readbytes = file->read_func (file->stream,audio_buffer,bufferlength);
	if(*readbytes==0)
		return MEERR_AAC_READ_AUDIO ;//file over
	else
		return MEERR_AAC_NONE;
}

