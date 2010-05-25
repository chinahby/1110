/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	jpg_demux.c
* User for : 	820
* Creat By : 	Zhou Jian()                                                                   
* CrateDate: 	2007/07/19
* Last modify : Zhou Jian 2007-07-19     
*
* release history:
* 		version 0.1 release by Zhou Jian (Zhang Zhongliang AE)
=======================================================================*/
#include "jpg_demux.h"
#include "Parser_jpg.h"

#define JPEGBUFFERLEN 				1024
#define JPEGREMARKLENGTH		128
#define JPGGETWORD(base,offset)  (((unsigned short)(*(base+offset))<< 8) + (unsigned short)*(base+offset+1))

unsigned int jpg_get_SOS(jpg_infor_t *file, unsigned char * ptr)
{
	
	unsigned char i, k, n, ac, dc, count, *pbuf = ptr+1,temp;	
	
	
	count = ptr[0];
	temp=pbuf[0];	//angela 2007-1-11
	for(i = 0; i < count; i++)
	{
		k = i << 1;
		n = pbuf[k]-temp;
		if(n > 3)
			return ME_RTN_ERROR;
		ac = pbuf[k+1] & 0xf;
		dc = pbuf[k+1] & 0xf0;
		if(!ac)
			file->Comp[n] |= 0x2;
		if(!dc)
			file->Comp[n] |= 0x1;
	}
	return 0;
}

unsigned int jpg_get_DHT(jpg_infor_t *file, unsigned char * ptr,unsigned short len)
{
	unsigned short j, n, k, HTLen;
	unsigned char *pd = ptr;
	unsigned char *HTaddr;
	
	j = 0;
	//file->HTCount=0;

	while(j < len-1)
	{
		HTLen = 0;
		n = pd[j] & 0xf;
		k = pd[j] & 0xf0;
		n <<= 1;
		n += k >> 4;
		if(n > 3)
			return ME_RTN_ERROR;
		j++;
		//file->HT[n] = pd+j;
		k = j + 16;
		while(j < k)
		{
			HTLen = HTLen + (unsigned short)(pd[j]);
			j++;
		}
		file->HTLen[n] = (unsigned char)(16 + HTLen);
		HTaddr=file->HT[n];
		me_memset(HTaddr,0,256);
		me_memcpy((char *)HTaddr,(char *)(pd+j-16),file->HTLen[n]);
		j = j+HTLen;
		file->HTCount++;
	}
	return 0;
}


unsigned int jpg_get_DQT(jpg_infor_t *file, unsigned char * ptr,unsigned short len)
{
	unsigned short i = 0;
	unsigned char	n;
	
	//file->QTCount=0;
	while(i < len)
	{
		if(ptr[i] & 0xf0)		//16 bit precision
		{
			n = ptr[i] & 0xf;
			if(n > 3)
				return ME_RTN_ERROR;
			file->QTPrecision[n] = 16;
			me_memset(file->QT[n],0,64 << 1);
			me_memcpy((char *)file->QT[n],(char *)ptr+i+1,64 << 1);
			i += 1 + (64 << 1);
		}
		else		//8 bit precision
		{
			n = ptr[i] & 0xf;
			if(n > 3)
				return ME_RTN_ERROR;
			file->QTPrecision[n] = 8;
			me_memset(file->QT[n],0,64);
			me_memcpy((char *)file->QT[n],(char *)ptr+i+1,64);
			i += 1 + 64;
		}
		file->QTCount++;
	}
	return 0 ;
}

void jpg_get_JpegSize(jpg_infor_t *file)
{
	switch(file->YUVType)
	{
	case JPG_422:
		if(file->is_v_sample)
		{
			file->JpgHeight= ((file->ImageHeight+ 15) >> 4) << 4;
			file->JpgWidth = ((file->ImageWidth+ 7) >> 3) << 3;
		}
		else
		{
			file->JpgWidth= ((file->ImageWidth+ 15) >> 4) << 4;
			file->JpgHeight= ((file->ImageHeight+ 7) >> 3) << 3;
		}
		break;
	case JPG_420:
		file->JpgWidth= ((file->ImageWidth+ 15) >> 4) << 4;
		file->JpgHeight = ((file->ImageHeight+ 15) >> 4) << 4;
		break;
	case JPG_411:
		if(file->is_v_sample)
		{
			file->JpgHeight= ((file->ImageHeight+ 31) >> 5) << 5;
			file->JpgWidth= ((file->ImageWidth+ 7) >> 3) << 3;
		}
		else
		{
			file->JpgWidth= ((file->ImageWidth+ 31) >> 5) << 5;
			file->JpgHeight= ((file->ImageHeight+ 7) >> 3) << 3;
		}	
		break;
	case JPG_400:
	case JPG_444:
		file->JpgWidth= ((file->ImageWidth + 7) >> 3) << 3;
		file->JpgHeight = ((file->ImageHeight+ 7) >> 3) << 3;
		break;
	default:
		break;
	}
}

unsigned int jpg_get_YuvSize(jpg_infor_t *file)
{
	unsigned int len;

	len = (unsigned int)file->JpgWidth & 0xffff;
	len *= (unsigned int)file->JpgHeight& 0xffff;
	switch(file->YUVType)
	{
	case JPG_422:
		len <<= 1;
		break;
	case JPG_420:
	case JPG_411:
		len *= 3;
		len >>= 1;
		break;
	case JPG_444:
		len *= 3;
		break;
	case JPG_400:
	default:
		break;
	}
	return len;
}

unsigned short jpg_get_CompFrmSOF(unsigned char * buf)
{
	unsigned char	h, v, n;
	unsigned short	val = 0;
	
	h = (buf[0] & 0x70) >> 4;
	v = buf[0] & 0x3;
	n = buf[1];
	val = (unsigned short)(h | (v << 4) | (n << 6)) << 8;
	n = h*v << 2;
	val |= (unsigned short)n;
	return val;
}

unsigned int jpg_get_SOF0(jpg_infor_t *file, unsigned char * ptr)
{
	unsigned char temp;

	file->data_precision = ptr[0];
	file->ImageHeight= JPGGETWORD(ptr,1);
	file->ImageWidth= JPGGETWORD(ptr,3);
	file->CompCount= ptr[5];
	temp=ptr[6];//angela 2007-1-11
	if(file->CompCount == 1)
	{
		if((ptr[6]-temp) > 3)
			return ME_RTN_ERROR;
		file->YUVType = JPG_400;
		file->Comp[ ptr[6]-temp ] = jpg_get_CompFrmSOF(ptr+7);;
	}
	else
	{
		if( ((ptr[9] - temp) > 3) || ((ptr[12] - temp) > 3) || ((ptr[6] - temp) > 3) )
			return ME_RTN_ERROR;
		file->Comp[ ptr[6]-temp ] = jpg_get_CompFrmSOF(ptr+7);
		if((ptr[7] == 0x21) )//|| (ptr[9] == 0x12))	//angela 2007-3-6
		{
			file->YUVType = JPG_422;
			file->is_v_sample=0;
		}
		else if(ptr[7] == 0x22)
			file->YUVType = JPG_420;			
		else if(ptr[7] == 0x41)
		{
			file->YUVType = JPG_411;
			file->is_v_sample=0;
		}
		else if(ptr[7] == 0x11)
			file->YUVType = JPG_444;
		else if(ptr[7] == 0x12)
		{
			file->YUVType = JPG_422;
			file->is_v_sample=1;
		}
		else if(ptr[7] == 0x14)
		{
			file->YUVType = JPG_411;
			file->is_v_sample=1;
		}
		else
			return ME_RTN_ERROR;
		file->Comp[ ptr[9]-temp ] = jpg_get_CompFrmSOF(ptr+10);
		file->Comp[ ptr[12]-temp] = jpg_get_CompFrmSOF(ptr+13);
	}
	
	jpg_get_JpegSize(file);
       //VIM_JPEG_GetJpegSize((VIM_JPEG_YUVMODE)pinfo->YUVType, pinfo->JpgSize, &size1);
	//file->vwc =jpg_get_YuvSize(file) >> 2;
	//file->blkcount = jpg_get_JpegBlockNum(file);

	return 0;
}

int jpg_read_data(jpg_infor_t *file, char *data, int size)
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

unsigned int jpg_read_info(jpg_infor_t *file)
{
	char	jpgCodec[4] = {'j','p','g',0};

	char	sjpgCodec[4] = {'s','j','p','g'};
	unsigned char *bufferpoint;//preloadbuffer pointer
	//int i=0;
	unsigned char bufferexceed=0;
	unsigned char nouseddinfo=0;
	unsigned int bufferseek=0;
	unsigned int count=0;
	unsigned int seekerror=0;
	unsigned int ret=0;
	//unsigned int readerror;
	unsigned int len;
	
	
	seekerror=file->seek_func(file->stream,0,SEEK_END);
	
	/*if(seekerror>0)
		return seekerror;*/
	file->file_length=file->tell_func(file->stream);
		
	if((seekerror=file->seek_func(file->stream,file->file_position,SEEK_SET))>0)
		return seekerror;
	
	file->preload_buffer=me_malloc(JPEGBUFFERLEN);
	me_memset((unsigned char *)file->preload_buffer,0,JPEGBUFFERLEN);
	bufferpoint=file->preload_buffer;

	while((count=jpg_read_data(file,(char *)file->preload_buffer,JPEGBUFFERLEN))==JPEGBUFFERLEN)
	{
		while(bufferseek<JPEGBUFFERLEN)
		{
			if((bufferpoint[bufferseek]==0xff)&&((bufferseek+1)<JPEGBUFFERLEN))
			{
				if(bufferpoint[bufferseek+1]!=0xff)
				{
					if((bufferseek+3)<JPEGBUFFERLEN)
					{
						switch(bufferpoint[bufferseek+1])
						{
							case SOI:								/* jpeg start header */
								file->sop = bufferseek;
								bufferseek++;
								break;
							case APP0:
								len = JPGGETWORD(bufferpoint+bufferseek+2,0);/* App0 is used for JFIF */
								if(len>(JPEGBUFFERLEN-bufferseek-2))
								{
									bufferexceed=1;
									break;
								}
								else
								{
									bufferseek+=(len+1);							
								}
								break;
							case SOF0:							/* Baseline */
								len = JPGGETWORD(bufferpoint+bufferseek+2,0);
								if(len>(JPEGBUFFERLEN-bufferseek-2))
								{
									bufferexceed=1;
									break;
								}
								else
								{
									ret=jpg_get_SOF0(file, bufferpoint+bufferseek+4);
									if(ret)
									{
										me_free(file->preload_buffer);
										file->preload_buffer=NULL;
										return ME_RTN_ERROR;
									} 
									bufferseek+=(len+1);							
								}
								break;
							case DQT:								/* segment DQT */
								len = JPGGETWORD(bufferpoint+bufferseek+2,0);/* App0 is used for JFIF */
								if(len>(JPEGBUFFERLEN-bufferseek-2))
								{
									bufferexceed=1;
									break;
								}
								else
								{
									ret=jpg_get_DQT(file, bufferpoint+bufferseek+4,len-2);
									if(ret)
									{
										me_free(file->preload_buffer);
										file->preload_buffer=NULL;
										return ME_RTN_ERROR;
									}
									bufferseek+=(len+1);	
								}
								break;
							case DHT:								/* segment DHT */
								len = JPGGETWORD(bufferpoint+bufferseek+2,0);/* App0 is used for JFIF */
								if(len>(JPEGBUFFERLEN-bufferseek-2))
								{
									bufferexceed=1;
									break;
								}
								else
								{
									ret=jpg_get_DHT(file, bufferpoint+bufferseek+4,len-2);
									if(ret)
									{
										me_free(file->preload_buffer);
										file->preload_buffer=NULL;
										return ME_RTN_ERROR;
									}
									bufferseek+=(len+1);	
								}
								break;
							case SOS:								/* segment SOS */
								len=JPGGETWORD(bufferpoint+bufferseek+2,0);
								if(len>(JPEGBUFFERLEN-bufferseek-2))
								{
									bufferexceed=1;
									break;
								}
								else
								{
									ret=jpg_get_SOS(file,bufferpoint+bufferseek+4);
									if(ret)
									{
										me_free(file->preload_buffer);
										file->preload_buffer=NULL;
										return ME_RTN_ERROR;
									}
									bufferseek+=(len+2);
									me_free(file->preload_buffer);
									file->preload_buffer=NULL;
									if((file->ImageWidth>4064)||(file->ImageHeight>4080))
									{
										/*file->offset=0;
										file->file_position=file->offset;
										file->seek_func(file->stream,file->file_position,SEEK_SET);
										file->file_rawdatalength=0;
										me_memcpy(file->v_codec,sjpgCodec,sizeof(sjpgCodec));*/
										return ME_ERR_FILE_IS_UNSUPPORT;
									}
									else if((((file->ImageWidth%16)||(file->ImageHeight%8))&&(file->YUVType==JPG_400))||(file->is_v_sample==1))
									{
										file->offset=0;
										file->file_position=file->offset;
										file->seek_func(file->stream,file->file_position,SEEK_SET);
										file->file_rawdatalength=0;
										me_memcpy(file->v_codec,sjpgCodec,sizeof(sjpgCodec));	
							        }
									else
									{
#if JPG_PARSER_ALLSOFT_DEBUG
										file->offset=0;
										file->file_position=file->offset;
										file->seek_func(file->stream,file->file_position,SEEK_SET);
										file->file_rawdatalength=0;
										me_memcpy(file->v_codec,sjpgCodec,sizeof(sjpgCodec));	
#else
										file->offset=file->file_position-JPEGBUFFERLEN+bufferseek;
										file->file_position=file->offset;
										file->seek_func(file->stream,file->file_position,SEEK_SET);
										len=file->file_length-file->offset;
										if(len&0x7f)
										{
											len>>=7;
											len+=1;
											len<<=7;		
										}
										file->file_rawdatalength=len;
										me_memcpy(file->v_codec,jpgCodec,sizeof(jpgCodec));
#endif
									}
#if JPG_PARSER_MESSAGE_DEBUG
									JPG_PARSER_MESSAGE(("adj width is %d,height is %d\n",file->JpgWidth,file->JpgHeight));
									JPG_PARSER_MESSAGE(("baseline compress mode\n"));
									switch(file->YUVType)
									{
										case 0:
											JPG_PARSER_MESSAGE(("YUV mode is YUV422\n"));
											break;
										case 1:
											JPG_PARSER_MESSAGE(("YUV mode is YUV420\n"));
											break;
										case 2:
											JPG_PARSER_MESSAGE(("YUV mode is YUV411\n"));
											break;
										case 3:
											JPG_PARSER_MESSAGE(("YUV mode is YUV400\n"));
											break;
										case 4:
											JPG_PARSER_MESSAGE(("YUV mode is YUV444\n"));
											break;
										default:
											JPG_PARSER_MESSAGE(("YUV mode is unkown\n"));
											break;
									}
#endif
									return 0;
								}								
							/*case M_EOI:
								file->eop = i+1;
								//file->frmEnd = TRUE;
								return;*/
							case SOF1:		/* Extended sequential, Huffman */
							case SOF2:		/* Progressive, Huffman */
							case SOF9:		/* Extended sequential, arithmetic */
							case SOF10:		/* Progressive, arithmetic */
							case SOF3:		/* Lossless, Huffman */
							case SOF5:		/* Differential sequential, Huffman */
							case SOF6:		/* Differential progressive, Huffman */
							case SOF7:		/* Differential lossless, Huffman */
							case SOF11:		/* Lossless, arithmetic */
							case SOF13:		/* Differential sequential, arithmetic */
							case SOF14:		/* Differential progressive, arithmetic */
							case SOF15:		/* Differential lossless, arithmetic */
							case JPG:			/* Reserved for JPEG extensions */
								/* Currently unsupported SOFn types */
								/* set YUV_TYPE = unknown */
								len = JPGGETWORD(bufferpoint+bufferseek+2,0);
								if(len>(JPEGBUFFERLEN-bufferseek-2))
								{
									bufferexceed=1;
									break;
								}
								else
								{
									ret=jpg_get_SOF0(file, bufferpoint+bufferseek+4);
									if(ret)
									{
										me_free(file->preload_buffer);
										file->preload_buffer=NULL;
										return ME_RTN_ERROR;
									}
									bufferseek+=(len+1);							
								}
								me_free(file->preload_buffer);
								file->preload_buffer=NULL;
#if	(CHIP_ID==ID_VC0878)
//#if JPG_PARSER_VC0878_DEBUG
								if(file->ImageWidth>472)
								{
									return ME_ERR_FILE_IS_UNSUPPORT;
								}
#endif
								if((file->ImageWidth>4064)||(file->ImageHeight>4080))
								{
									return ME_ERR_FILE_IS_UNSUPPORT;
								}
								
								//file->JpgHeight=file->ImageHeight;
								//file->JpgWidth=file->ImageWidth;

								file->offset=0;
								file->file_position=file->offset;
								file->seek_func(file->stream,file->file_position,SEEK_SET);
								file->file_rawdatalength=0;
								me_memcpy(file->v_codec,sjpgCodec,sizeof(sjpgCodec));
#if JPG_PARSER_MESSAGE_DEBUG
								JPG_PARSER_MESSAGE(("not baseline compress mode\n"));
								switch(file->YUVType)
								{
									case 0:
										JPG_PARSER_MESSAGE(("YUV mode is YUV422\n"));
										break;
									case 1:
										JPG_PARSER_MESSAGE(("YUV mode is YUV420\n"));
										break;
									case 2:
										JPG_PARSER_MESSAGE(("YUV mode is YUV411\n"));
										break;
									case 3:
										JPG_PARSER_MESSAGE(("YUV mode is YUV400\n"));
										break;
									case 4:
										JPG_PARSER_MESSAGE(("YUV mode is YUV444\n"));
										break;
									default:
										JPG_PARSER_MESSAGE(("YUV mode is unkown\n"));
										break;
								}
#endif					
								return 0;
							case COM:			/* Comment */
							case DAC:			/* Define arithmetic coding conditioning */
							case DRI:			/* Define restart interval */
							case APP2:		/* Reserved for application segments */
							case APP1:
							case APP3:
							case APP4:
							case APP5:
							case APP6:
							case APP7:
							case APP8:
							case APP9:
							case APP10:
							case APP11:
							case APP12:
							case APP13:
							case APP14:
							case APP15:
							case RST0:		/* these are all parameterless */
							case RST1:
							case RST2:
							case RST3:
							case RST4:
							case RST5:
							case RST6:
							case RST7:
							case DNL:			/* Ignore DNL ... perhaps the wrong thing */
								/* Currently unsupported, bypass! */
								len =JPGGETWORD(bufferpoint+bufferseek+2,0);
								if(len>(JPEGBUFFERLEN-bufferseek-2))
								{
									nouseddinfo=1;
									break;
								}
								else
								{
									bufferseek+=(len+1);							
								}
								break;
							default:								/* error! You could show error here */
								break;	/* set YUVType = Unknown, and return */					
						}
						if(1==bufferexceed)
						{
							bufferexceed=0;
							file->file_position-=(JPEGBUFFERLEN-bufferseek);
							file->seek_func(file->stream,file->file_position,SEEK_SET);
							break;
						}
						if(1==nouseddinfo)
						{
							nouseddinfo=0;
							file->file_position-=(JPEGBUFFERLEN-bufferseek);
							file->file_position+=(len+2);
							file->seek_func(file->stream,file->file_position,SEEK_SET);
							break;
						}

					}
					else
					{
						file->file_position-=(JPEGBUFFERLEN-bufferseek);
						file->seek_func(file->stream,file->file_position,SEEK_SET);
						break;
					}
				}
			}
			else
			{
				file->file_position-=1;
				file->seek_func(file->stream,file->file_position,SEEK_SET);
				break;
			}
			bufferseek++;
		}
		bufferseek=0;
	}
	if(count==file->file_length)
	{
		while(bufferseek<count)
		{
			if(bufferpoint[bufferseek] == 0xff)
			{
				do
				{
					bufferseek++;
				}
				while(bufferpoint[bufferseek]==0xff);
				bufferseek--;
				switch(bufferpoint[bufferseek+1])
				{
				case SOI:								/* jpeg start header */
					file->sop = bufferseek;
					bufferseek++;
					break;
				case APP0:
					len = JPGGETWORD(bufferpoint+bufferseek+2,0);/* App0 is used for JFIF */
					bufferseek+=(len+1);							
					break;
				case SOF0:							/* Baseline */
					len = JPGGETWORD(bufferpoint+bufferseek+2,0);
				   	ret=jpg_get_SOF0(file, bufferpoint+bufferseek+4);
					if(ret)
					{
						me_free(file->preload_buffer);
						file->preload_buffer=NULL;
						return ME_RTN_ERROR;
					}
					bufferseek+=(len+1);							
					break;
				case DQT:								/* segment DQT */
					len = JPGGETWORD(bufferpoint+bufferseek+2,0);/* App0 is used for JFIF */
					ret=jpg_get_DQT(file, bufferpoint+bufferseek+4,len-2);
					if(ret)
					{
						me_free(file->preload_buffer);
						file->preload_buffer=NULL;
						return ME_RTN_ERROR;
					}
					bufferseek+=(len+1);	
					break;
				case DHT:								/* segment DHT */
					len = JPGGETWORD(bufferpoint+bufferseek+2,0);/* App0 is used for JFIF */
					ret=jpg_get_DHT(file, bufferpoint+bufferseek+4,len-2);
					if(ret)
					{
						me_free(file->preload_buffer);
						file->preload_buffer=NULL;
						return ME_RTN_ERROR;
					}
					bufferseek+=(len+1);	
					break;
				case SOS:								/* segment SOS */
					len=JPGGETWORD(bufferpoint+bufferseek+2,0);
					ret=jpg_get_SOS(file,bufferpoint+bufferseek+4);
					if(ret)
					{
						me_free(file->preload_buffer);
						file->preload_buffer=NULL;
						return ME_RTN_ERROR;
					}
					bufferseek+=(len+2);
					me_free(file->preload_buffer);
					file->preload_buffer=NULL;
					if((file->ImageWidth>4064)||(file->ImageHeight>4080))
					{
						return ME_ERR_FILE_IS_UNSUPPORT;
					}
					else if((((file->ImageWidth%16)||(file->ImageHeight%8))&&(file->YUVType==JPG_400))||(file->is_v_sample==1))
					{
						file->offset=0;
						file->file_position=file->offset;
						file->seek_func(file->stream,file->file_position,SEEK_SET);
						file->file_rawdatalength=0;
						me_memcpy(file->v_codec,sjpgCodec,sizeof(sjpgCodec));	
					}
					else
					{
#if JPG_PARSER_ALLSOFT_DEBUG
						file->offset=0;
						file->file_position=file->offset;
						file->seek_func(file->stream,file->file_position,SEEK_SET);
						file->file_rawdatalength=0;
						me_memcpy(file->v_codec,sjpgCodec,sizeof(sjpgCodec));	
#else
						file->offset=bufferseek;
						file->file_position=file->offset;
						file->seek_func(file->stream,file->file_position,SEEK_SET);
						len=file->file_length-file->offset;
						if(len&0x7f)
						{
							len>>=7;
							len+=1;
							len<<=7;		
						}
						file->file_rawdatalength=len;
						me_memcpy(file->v_codec,jpgCodec,sizeof(jpgCodec));
#endif
					}

#if JPG_PARSER_MESSAGE_DEBUG
					JPG_PARSER_MESSAGE(("adj width is %d,height is %d\n",file->JpgWidth,file->JpgHeight));
					JPG_PARSER_MESSAGE(("baseline compress mode\n"));
					switch(file->YUVType)
					{
						case 0:
							JPG_PARSER_MESSAGE(("YUV mode is YUV422\n"));
							break;
						case 1:
							JPG_PARSER_MESSAGE(("YUV mode is YUV420\n"));
							break;
						case 2:
							JPG_PARSER_MESSAGE(("YUV mode is YUV411\n"));
							break;
						case 3:
							JPG_PARSER_MESSAGE(("YUV mode is YUV400\n"));
							break;
						case 4:
							JPG_PARSER_MESSAGE(("YUV mode is YUV444\n"));
							break;
						default:
							JPG_PARSER_MESSAGE(("YUV mode is unkown\n"));
							break;
					}
#endif					
					return 0;
				case SOF1:		/* Extended sequential, Huffman */
				case SOF2:		/* Progressive, Huffman */
				case SOF9:		/* Extended sequential, arithmetic */
				case SOF10:		/* Progressive, arithmetic */
				case SOF3:		/* Lossless, Huffman */
				case SOF5:		/* Differential sequential, Huffman */
				case SOF6:		/* Differential progressive, Huffman */
				case SOF7:		/* Differential lossless, Huffman */
				case SOF11:		/* Lossless, arithmetic */
				case SOF13:		/* Differential sequential, arithmetic */
				case SOF14:		/* Differential progressive, arithmetic */
				case SOF15:		/* Differential lossless, arithmetic */
				case JPG:			/* Reserved for JPEG extensions */
							/* Currently unsupported SOFn types */
								/* set YUV_TYPE = unknown */
					len = JPGGETWORD(bufferpoint+bufferseek+2,0);
					ret=jpg_get_SOF0(file, bufferpoint+bufferseek+4);
					if(ret)
					{
						me_free(file->preload_buffer);
						file->preload_buffer=NULL;
						return ME_RTN_ERROR;
					}
					me_free(file->preload_buffer);
					file->preload_buffer=NULL;
#if	(CHIP_ID==ID_VC0878)
//#if JPG_PARSER_VC0878_DEBUG
					if(file->ImageWidth>472)
					{
						return ME_ERR_FILE_IS_UNSUPPORT;
					}
#endif
					if((file->ImageWidth>4064)||(file->ImageHeight>4080))
					{
						return ME_ERR_FILE_IS_UNSUPPORT;
					}
					//file->JpgHeight=file->ImageHeight;
					//file->JpgWidth=file->ImageWidth;
					file->offset=0;
					file->file_position=file->offset;
					file->seek_func(file->stream,file->file_position,SEEK_SET);
					file->file_rawdatalength=0;
					me_memcpy(file->v_codec,sjpgCodec,sizeof(sjpgCodec));
#if JPG_PARSER_MESSAGE_DEBUG
					JPG_PARSER_MESSAGE(("not baseline compress mode\n"));
					switch(file->YUVType)
					{
						case 0:
							JPG_PARSER_MESSAGE(("YUV mode is YUV422\n"));
							break;
						case 1:
							JPG_PARSER_MESSAGE(("YUV mode is YUV420\n"));
							break;
						case 2:
							JPG_PARSER_MESSAGE(("YUV mode is YUV411\n"));
							break;
						case 3:
							JPG_PARSER_MESSAGE(("YUV mode is YUV400\n"));
							break;
						case 4:
							JPG_PARSER_MESSAGE(("YUV mode is YUV444\n"));
							break;
						default:
							JPG_PARSER_MESSAGE(("YUV mode is unkown\n"));
							break;
					}
#endif
					return 0;
				case COM:			/* Comment */
				case DAC:			/* Define arithmetic coding conditioning */
				case DRI:			/* Define restart interval */
				case APP2:		/* Reserved for application segments */
				case APP1:
				case APP3:
				case APP4:
				case APP5:
				case APP6:
				case APP7:
				case APP8:
				case APP9:
				case APP10:
				case APP11:
				case APP12:
				case APP13:
				case APP14:
				case APP15:
				case RST0:		/* these are all parameterless */
				case RST1:
				case RST2:
				case RST3:
				case RST4:
				case RST5:
				case RST6:
				case RST7:
				case DNL:			/* Ignore DNL ... perhaps the wrong thing */
								/* Currently unsupported, bypass! */
					len =JPGGETWORD(bufferpoint+bufferseek+2,0);
					bufferseek+=(len+1);							
					break;
				default:								/* error! You could show error here */
					break;	/* set YUVType = Unknown, and return */					
				}
			}
			bufferseek++;
		}
	}
	else
	{
		me_free(file->preload_buffer);
		file->preload_buffer=NULL;
		return ME_ERR_READ_FILE_ERROR;
	}
	return 0;	
}
jpg_infor_t* jpg_init(void)
{
	jpg_infor_t *jpg_file = me_malloc(sizeof(jpg_infor_t));
	me_memset((unsigned char *)jpg_file,0,sizeof(jpg_infor_t));
	JPG_PARSER_TRACE_MSG();
	return jpg_file;
}
unsigned int jpg_close(jpg_infor_t *file)
{
	me_free(file);
	JPG_PARSER_TRACE_MSG();
	return 0;
}
unsigned int jpg_check_sig(jpg_infor_t *file)
{
	unsigned int seekerror=0;
	unsigned int readerror;
	char	jpegremark[JPEGREMARKLENGTH];
	short i=0,ishead=0,istail=0;

	//if((seekerror=file->seek_func(file->stream,0,SEEK_SET))>0)
			//return seekerror;
	seekerror=file->seek_func(file->stream,0,SEEK_SET);

	readerror=file->read_func(file->stream, jpegremark, JPEGREMARKLENGTH);
	
	if(readerror!=JPEGREMARKLENGTH)
		return ME_FALSE;

	while(i < JPEGREMARKLENGTH)
	{
		if((jpegremark[i]==0xff)&&(jpegremark[i+1]==0xd8))
		{
			ishead=1;
			break;
		}
		i++;
	}

	seekerror=file->seek_func(file->stream,0,SEEK_END);
	file->file_length=file->tell_func(file->stream);
	file->file_position=file->file_length-JPEGREMARKLENGTH;
	seekerror=file->seek_func(file->stream,file->file_position,SEEK_SET);
	
	readerror=file->read_func(file->stream, jpegremark, JPEGREMARKLENGTH);
	
	if(readerror!=JPEGREMARKLENGTH)
			return ME_FALSE;
	
	i=0;
	
	while(i < JPEGREMARKLENGTH)
	{
		if((jpegremark[i]==0xff)&&(jpegremark[i+1]==0xd9))
		{
			istail=1;
			break;
		}
		i++;
	}

	if((ishead==1)&&(istail==1))
	{
		return ME_TRUE;
	}
	else
	{
		return ME_FALSE;
	}

}
/*unsigned int jpg_getmaxframesize(jpg_infor_t *file,unsigned int *framesize)
{
       *framesize = file->maxframesize*MIN_FRAME_NUMBER;
  	return 0;	
}
unsigned int jpg_getaudioduration(amr_infor_t *fileinfor, unsigned int frame, unsigned int * duration)
{
	*duration = 20;
	
	return 0;
	
}*/

unsigned int jpg_read_image(jpg_infor_t *file,char *image_buffer, unsigned int *readbytes,unsigned int bufferlength)
{
	char *buf;
	unsigned int count=0;
		
    if(image_buffer != NULL)
	{
		buf = image_buffer;
		me_memset((unsigned char *)buf,0,bufferlength);
	}
	else
		return 1;

	count=jpg_read_data(file,buf,bufferlength);
	*readbytes = count;
	
	if(count<bufferlength)
	{
		if(count==0)
		{
			if(file->file_position==file->file_length)
			{
				return ME_FILE_END_OF_VIDEO;
			}
			else
			{
				return ME_ERR_READ_FILE_ERROR;	
			}
		}
		else
		{
			if(file->file_position==file->file_length)
			{
				count+=(file->file_rawdatalength-(file->file_length-file->offset));
				*readbytes = count;
				return ME_FILE_LAST_OF_VIDEO;
			}
			else
			{
				return ME_ERR_READ_FILE_ERROR;	
			}
		}
	}
	return 0;

}


unsigned int jpg_seek(void *fileinfor, unsigned int pos, unsigned int * accesspos)
{
	jpg_infor_t *file=(jpg_infor_t *)fileinfor;
	*accesspos = 0;
	file->file_position=file->offset;
	file->seek_func(file->stream,file->file_position,SEEK_SET);
	return 0;
	
}
