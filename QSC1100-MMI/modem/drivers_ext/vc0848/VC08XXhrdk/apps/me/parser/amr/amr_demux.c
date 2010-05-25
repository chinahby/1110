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
#include "amr_demux.h"
#include "Parser_amr.h"

#define AMR_MAGIC_NUMBER 		"#!AMR\n"
#define BUFFERLEN 				512

static unsigned short  packed_size[16] = {12, 13, 15, 17, 19, 20, 26, 31, 5, 5, 5, 5, 0, 0, 0, 0};

int amr_read_data(amr_infor_t *file, char *data, int size)
{
	int readbytes=0;
	
	if(!file->preload_size)
	{
		if(file->tell_func(file->stream) != file->file_position)
			file->seek_func(file->stream, file->file_position, SEEK_SET);
		readbytes = file->read_func(file->stream, data, size);
	}
	file->file_position += readbytes;
	return readbytes;
}


unsigned int amr_read_info(amr_infor_t *file)
{
	char	amrCodec[4] = {'a','m','r',0};
	//char	magic[6];
	char *bufferpoint;//preloadbuffer pointer
	unsigned char framhead;
	unsigned short bufferseek=0;
	unsigned short count=0;
	unsigned short ft = 0;
	unsigned short tmp = 0; 
	unsigned int samplelen = 0;
	unsigned int seekerror=0;
	char magic[10];

	//me_memset(()magic,0,sizeof(magic));
	if((seekerror=file->seek_func(file->stream,0,SEEK_END))>0)
		return seekerror;
	file->file_length=file->tell_func(file->stream);
	file->file_end = 0;
	if(file->file_length==6)
	{
		return ME_RTN_ERROR;
	}
		
	file->file_position=6;
	#if 0
	if((seekerror=file->seek_func(file->stream,file->file_position,SEEK_SET))>0)
		return seekerror;
	#else
	file->seek_func(file->stream, 0, SEEK_SET);

	file->read_func(file->stream, magic, file->file_position);
	if(magic[5] != '\n')
	{
		me_printf("\n########This is not an AMR-NB file!!!\n");
		return ME_ERR_READ_FILE_ERROR;
	}
	#endif
	/*if((readerror=amr_read_data(file,magic,6))==0)
		return ME_ERR_READ_FILE_ERROR;
	
	if (!strncmp((const char *)magic, AMR_MAGIC_NUMBER, strlen(AMR_MAGIC_NUMBER)))
	{
		file->is_amr = 1;
	}
	else
	{
		file->is_amr = 0;
		return 1;
	}*/
	file->is_amr = 1;

	file->preload_buffer=me_malloc(BUFFERLEN);
	me_memset((unsigned char *)file->preload_buffer,0,BUFFERLEN);
	bufferpoint=file->preload_buffer;

	while((count=amr_read_data(file,file->preload_buffer,BUFFERLEN))==BUFFERLEN)
	{
		while(bufferseek<BUFFERLEN)
		{
			framhead=bufferpoint[bufferseek];
			ft = (framhead>> 3) & 0x0f;
			ft = packed_size[ft];
			samplelen+=ft;
			if(ft>tmp)
	  	         tmp = ft;
			file->framecnt++;
			//bufferpoint+=(ft+1);
			bufferseek+=(ft+1);
		}
		//bufferpoint-=bufferseek	;
		file->file_position-=count;
        file->file_position+=bufferseek;
		file->seek_func(file->stream,file->file_position,SEEK_SET);
		bufferseek=0;
	}
	if(count==0)
	{
		if(file->file_position!=file->file_length)
		{
			me_free(file->preload_buffer);
			file->preload_buffer=NULL;
			return ME_ERR_READ_FILE_ERROR;
		}
	}
	else
	{
		while(bufferseek<count)
		{
			framhead=bufferpoint[bufferseek];
			ft = (framhead>> 3) & 0x0f;
			ft = packed_size[ft];
			samplelen+=ft;
			if(ft>tmp)
	  		tmp = ft;
			file->framecnt++;
			//bufferpoint+=(ft+1);
			bufferseek+=(ft+1);
		}
	}

	me_free(file->preload_buffer);
	file->preload_buffer=NULL;
	file->file_position=6;
	file->seek_func(file->stream,file->file_position,SEEK_SET);	
	file->maxframesize = tmp+1;
	AMR_PARSER_MESSAGE(("the maxframesize is %d",file->maxframesize));
	AMR_PARSER_MESSAGE(("the framecount is %d",file->framecnt));
	AMR_PARSER_MESSAGE(("the time is %ds",file->framecnt*20/1000));
	file->a_chn = 1;
	file->a_smplrate = 8000;
	file->a_bits = 16;
	file->a_smpllen = samplelen/file->framecnt;
	me_memcpy(file->a_codec,amrCodec,sizeof(amrCodec));
	return 0;	
}
amr_infor_t* amr_init(void)
{
	amr_infor_t *amr_file = me_malloc(sizeof(amr_infor_t));
	AMR_PARSER_TRACE_MSG();
	return amr_file;
}
unsigned int amr_close(amr_infor_t *file)
{
	me_free(file);
	AMR_PARSER_TRACE_MSG();
	return 0;
}
unsigned int amr_check_sig(amr_infor_t *file)
{
	unsigned int seekerror=0;
	unsigned int readerror;
	char	magic[6];
	
	if((seekerror=file->seek_func(file->stream,file->file_position,SEEK_SET))>0)
		return seekerror;
		
	readerror = file->read_func(file->stream, magic,6);
	if(readerror!=6)
		return ME_FALSE;
	
	if (!strncmp((const char *)magic, AMR_MAGIC_NUMBER, strlen(AMR_MAGIC_NUMBER)-1))
	{
		return ME_TRUE;
	}
	else
	{
		return ME_FALSE;
	}
}

unsigned int amr_getmaxframesize(amr_infor_t *file,unsigned int *framesize)
{
       *framesize = file->maxframesize*MIN_FRAME_NUMBER;
  	return 0;	
}
unsigned int amr_getaudioduration(amr_infor_t *fileinfor, unsigned int frame, unsigned int * duration)
{
	*duration = 20;
	
	return 0;
	
}

unsigned int amr_readaudio(amr_infor_t *file,char *audio_buffer, unsigned int *readbytes,unsigned int bufferlength)
{
	char *buf;//,*ptr;
	unsigned short count=0;
	unsigned char framhead;
	unsigned short bufferseek=0;
	
	unsigned short ft = 0;
		
    if(audio_buffer != NULL)
	{
		buf = audio_buffer;
		me_memset((unsigned char *)buf,0,bufferlength);
	}
	else
		return 1;

	count=amr_read_data(file,buf,bufferlength);
	
	
	if(count<bufferlength)
	{
		*readbytes = count;
		if(count==0)
		{
			if(file->file_position==file->file_length)
			{
				return ME_FILE_END_OF_AUDIO;
			}
			else
			{
				return ME_ERR_READ_FILE_ERROR;	
			}
		}
		return ME_FILE_END_OF_AUDIO;
	}
	else
	{
		while(bufferseek<bufferlength)
		{
			framhead=buf[bufferseek];
			ft = (framhead>> 3) & 0x0f;
			ft = packed_size[ft];
			//buf+=(ft+1);
			bufferseek+=(ft+1);
		}
		//buf-=bufferseek;
		if(bufferseek>bufferlength)
		{
			bufferseek-=(ft+1);
			count-=bufferseek;
			file->file_position-=count;
		}
		file->seek_func(file->stream,file->file_position,SEEK_SET);
		*readbytes = bufferseek;
		return 0;
	}
}


unsigned int amr_seek(void *fileinfor, unsigned int pos, unsigned int * accesspos)
{
	amr_infor_t *file;
	unsigned short ft = 0;
	unsigned int seekfilepos=0;
	unsigned int position=0;
	unsigned int seekframecnt=0;
	char *bufferpoint;
	unsigned char framhead;
	unsigned short count;
	unsigned short bufferseek=0;
	unsigned int seekerror=0;

	file = fileinfor;
	seekframecnt =(pos/20);
	position=seekframecnt*20;

	AMR_PARSER_MESSAGE(("the seekframecount is %d",seekframecnt));
	
	if(position>(file->framecnt*20))
	{
		if((seekerror=file->seek_func(file->stream,0,SEEK_END))>0)
		return seekerror;
		file->file_position=file->tell_func(file->stream);
		return 0;
	}
	
	file->file_position=6;
	if((seekerror=file->seek_func(file->stream,file->file_position,SEEK_SET))>0)
		return seekerror;
	
	if(seekframecnt>0)
	{
		seekfilepos+=file->file_position;
		file->preload_buffer=me_malloc(BUFFERLEN);
		me_memset((unsigned char *)file->preload_buffer,0,BUFFERLEN);
		bufferpoint=file->preload_buffer;

		while((count=amr_read_data(file,file->preload_buffer,BUFFERLEN))==BUFFERLEN)
		{
			while(bufferseek<BUFFERLEN)
			{
				framhead=bufferpoint[bufferseek];
				ft = (framhead>> 3) & 0x0f;
				ft = packed_size[ft];
				seekframecnt--;
				seekfilepos+=(ft+1);
				bufferseek+=(ft+1);
				//bufferpoint+=(ft+1);
				if(seekframecnt==0)
				{
					file->seek_func(file->stream,seekfilepos,SEEK_SET);
					file->file_position=seekfilepos;
					*accesspos=position;
					me_free(file->preload_buffer);
					file->preload_buffer=NULL;
					return 0;
				}
			}
			//bufferpoint-=bufferseek	;
			file->file_position-=count;
        	file->file_position+=bufferseek;
			file->seek_func(file->stream,file->file_position,SEEK_SET);
			bufferseek=0;
		}
		if(count==0)
		{
			if(file->file_position!=file->file_length)
			{
				me_free(file->preload_buffer);
				file->preload_buffer=NULL;
				return ME_ERR_READ_FILE_ERROR;
			}	
		}
		else
		{
			while(bufferseek<count)
			{
				framhead=bufferpoint[bufferseek];
				ft = (framhead>> 3) & 0x0f;
				ft = packed_size[ft];
				seekframecnt--;
				seekfilepos+=ft+1;
				bufferseek+=(ft+1);
				//bufferpoint+=(ft+1);
				if(seekframecnt==0)
				{
					file->seek_func(file->stream,seekfilepos,SEEK_SET);
					file->file_position=seekfilepos;
					*accesspos=position;
					me_free(file->preload_buffer);
					file->preload_buffer=NULL;
					return 0;
				}
			}
		}
	}
	*accesspos=position;
	return 0;
	
}
