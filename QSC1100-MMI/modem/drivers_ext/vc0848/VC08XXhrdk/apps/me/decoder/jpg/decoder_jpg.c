/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	decoder_mp4v.c
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2007/03/28
* Last modify : Amanda Deng 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/
#include "decoder_jpg.h"

#if ME_KM_DECODE_JPG_EN
#include "..\..\parser\jpg\jpg_demux.h"

#if ME_KM_OBJ_STACK
static me_decoder_t 	_g_decoder_jpg;
#endif

#include "..\..\..\..\Drivers\include\VIM_Drivers.h"
//extern ME_U32 jpeg_display_callback(ME_U32 flag, ME_U32 size);
/*******************************************************************************************
Description:
	mp4 video decoder initialize
Parameters:
       void *headinfor:		video information structor point
       void **decodeinfor:	decoder information structor point's point
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.03.28		create first version
	   0.11			Amanda Deng			2007.11.22		modify input parameter
********************************************************************************************/
ME_U32 decoder_jpg_init(void *srcinfor, void *outinfor, void **decodeinfor)
{
	me_video_infor	*video_infor;
	me_video_outinfor	*video_outinfor;
	jpg_infor_t		*jpeg_file_info;
	PVIM_JPEG_DECODE_INFO jpeg_decode_infor;
	ME_U32	i;

	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_jpg_init()"));
	#endif

	(*decodeinfor) = NULL;
	ME_ASSERT_PARAM_RETURN((srcinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((outinfor!=NULL));
	
	video_infor = (me_video_infor *)srcinfor;
	video_outinfor = (me_video_outinfor*)outinfor;
	
	jpeg_decode_infor=me_malloc(sizeof(VIM_JPEG_DECODE_INFO));
	if(jpeg_decode_infor==NULL)
	{
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		return ME_ERR_MALLOC_MEMORY_ERROR;
	}	
	me_memset((ME_PU08)jpeg_decode_infor, 0, sizeof(VIM_JPEG_DECODE_INFO));
	jpeg_file_info=(jpg_infor_t *)video_infor->v_others;
	
	jpeg_decode_infor->ImageInfo.YUVType		=jpeg_file_info->YUVType;
	jpeg_decode_infor->ImageInfo.CompCount	=jpeg_file_info->CompCount;
	jpeg_decode_infor->ImageInfo.data_precision	=jpeg_file_info->data_precision;
	jpeg_decode_infor->ImageInfo.ImageSize.cx	=jpeg_file_info->ImageWidth;
	jpeg_decode_infor->ImageInfo.ImageSize.cy	=jpeg_file_info->ImageHeight;
	jpeg_decode_infor->ImageInfo.JpgSize.cx		=jpeg_file_info->JpgWidth;
	jpeg_decode_infor->ImageInfo.JpgSize.cy		=jpeg_file_info->JpgHeight;
	jpeg_decode_infor->ImageInfo.offset			=jpeg_file_info->offset;
	jpeg_decode_infor->ImageInfo.HTCount		=jpeg_file_info->HTCount;
	jpeg_decode_infor->ImageInfo.QTCount		=jpeg_file_info->QTCount;
	
	for(i=0;i<4;i++)
	{
		jpeg_decode_infor->ImageInfo.Comp[i]=jpeg_file_info->Comp[i];
		jpeg_decode_infor->ImageInfo.HTLen[i]=jpeg_file_info->HTLen[i];
		jpeg_decode_infor->ImageInfo.HT[i]=jpeg_file_info->HT[i];
		jpeg_decode_infor->ImageInfo.QTPrecision[i]=jpeg_file_info->QTPrecision[i];
		jpeg_decode_infor->ImageInfo.QT[i]=jpeg_file_info->QT[i];
	}
	
	video_outinfor->v_srcwidth = video_infor->v_imagewidth;
	video_outinfor->v_srcheight = video_infor->v_imageheight;
	video_outinfor->v_srcxpos = 0;
	video_outinfor->v_srcypos = 0;
	
	jpeg_decode_infor->DisplayMode	= VIM_HAPI_DISPLAY_JPEG;
	if(video_infor->v_jpgmode)
	{
		jpeg_decode_infor->DecodeMode = VIM_DECODE_FRAME;
		jpeg_decode_infor->JpegDecodeBufAddr = video_infor->v_jpgdecodeaddr;
		jpeg_decode_infor->JpegDecodeBufLen  = video_infor->v_jpgdecodelen;
	}
	else
	{
		jpeg_decode_infor->DecodeMode = VIM_DECODE_NONE;
		jpeg_decode_infor->JpegDecodeBufAddr = 0;
		jpeg_decode_infor->JpegDecodeBufLen  =  0;
	}
	jpeg_decode_infor->EffectMode 		= (VIM_HAPI_SPECIAL_EFFECT)video_outinfor->v_outeffect;
	jpeg_decode_infor->RotationMode 	= (VIM_HAPI_ROTATEMODE)video_outinfor->v_outrotation;
	jpeg_decode_infor->JpegWinSize.cx   =video_outinfor->v_srcwidth=jpeg_file_info->ImageWidth;
	jpeg_decode_infor->JpegWinSize.cy   =video_outinfor->v_srcheight=jpeg_file_info->ImageHeight;
	jpeg_decode_infor->JpegOffsetPt.x   =video_outinfor->v_srcxpos=0;
	jpeg_decode_infor->JpegOffsetPt.y   =video_outinfor->v_srcypos=0;
	jpeg_decode_infor->LcdWantSize.cx	=video_outinfor->v_outwidth;
	jpeg_decode_infor->LcdWantSize.cy	=video_outinfor->v_outheight;
	jpeg_decode_infor->LcdWantPt.x		=video_outinfor->v_outxpos;
	jpeg_decode_infor->LcdWantPt.y		=video_outinfor->v_outypos;
	jpeg_decode_infor->JpegCallBack	=NULL;

	(*decodeinfor) = (void *)jpeg_decode_infor;
	return ME_SUCCESS;
}

/*******************************************************************************************
Description:
	mp4 video decoder release
Parameters:
       void *decodeinfor:	decoder information structor point
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.03.28		create first version
********************************************************************************************/
ME_U32 decoder_jpg_release(void *decodeinfor)
{
	PVIM_JPEG_DECODE_INFO jpeg_decode_info;
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_jpg_release()"));
	#endif
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
	jpeg_decode_info=(PVIM_JPEG_DECODE_INFO)decodeinfor;
	me_free(jpeg_decode_info);
	return ME_SUCCESS;
}

/*******************************************************************************************
Description:
	mp4 video decoder output parameter set and ipp,lcdc,lbuf config
Parameters:
      void *decodeinfor:	decoder information structor point
      void *outinfor:		decoder output information structor point
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.11.19		create first version
********************************************************************************************/
ME_U32 decoder_jpg_setoutparam(void *decodeinfor, void *outinfor)
{
	me_video_outinfor *video_outinfor;
	PVIM_JPEG_DECODE_INFO jpeg_decode_infor;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_mp4v_setoutparam()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((outinfor!=NULL));
	
	video_outinfor		= (me_video_outinfor*)outinfor;
	jpeg_decode_infor	=(PVIM_JPEG_DECODE_INFO)decodeinfor;
	
	jpeg_decode_infor->EffectMode 		= (VIM_HAPI_SPECIAL_EFFECT)video_outinfor->v_outeffect;
	jpeg_decode_infor->RotationMode 	= (VIM_HAPI_ROTATEMODE)video_outinfor->v_outrotation;
	jpeg_decode_infor->LcdWantSize.cx	=video_outinfor->v_outwidth;
	jpeg_decode_infor->LcdWantSize.cy	=video_outinfor->v_outheight;
	jpeg_decode_infor->LcdWantPt.x		=video_outinfor->v_outxpos;
	jpeg_decode_infor->LcdWantPt.y		=video_outinfor->v_outypos;
	jpeg_decode_infor->JpegWinSize.cx   =video_outinfor->v_srcwidth;
	jpeg_decode_infor->JpegWinSize.cy	=video_outinfor->v_srcheight;
	jpeg_decode_infor->JpegOffsetPt.x	=video_outinfor->v_srcxpos;
	jpeg_decode_infor->JpegOffsetPt.y	=video_outinfor->v_srcypos;
	
	return ME_SUCCESS;
}

/*******************************************************************************************
Description:
	mp4 video decoder seek
Parameters:
       void *decodeinfor:	decoder information structor point
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.03.28		create first version
********************************************************************************************/
ME_U32 decoder_jpg_seek(void *decodeinfor, ME_U32 pos)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_jpg_seek()"));
	#endif
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
	return ME_SUCCESS;
}

/*******************************************************************************************
Description:
	mp4 video decoder start
Parameters:
       void *decodeinfor:	decoder information structor point
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.03.28		create first version
********************************************************************************************/
ME_U32 decoder_jpg_start(void *decodeinfor)
{
	ME_U32 result;
	PVIM_JPEG_DECODE_INFO jpeg_decode_infor;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_jpg_start()"));
	#endif	
	
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
	VIM_HAPI_SetWorkMode(VIM_HAPI_MODE_PHOTOPROCESS);
	
	jpeg_decode_infor	=(PVIM_JPEG_DECODE_INFO)decodeinfor;

	if(jpeg_decode_infor->DecodeMode==VIM_DECODE_NONE)
	{
	result = VIM_HAPI_ReadyToDisplayJpeg((PVIM_JPEG_DECODE_INFO)decodeinfor);
	if(!result)
	{
		VIM_HAPI_StartToDisplayJpeg((PVIM_JPEG_DECODE_INFO)decodeinfor);
	}
	} 
	else
	{
		result = VIM_HAPI_ReadyToDecodeJpeg((PVIM_JPEG_DECODE_INFO)decodeinfor);
		if(!result)
		{
			VIM_HAPI_StartDecodeJpeg();
		}
	}
	return result;
}

/*******************************************************************************************
Description:
	mp4 video decoder stop
Parameters:
       void *decodeinfor:	decoder information structor point
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.03.28		create first version
********************************************************************************************/
ME_U32 decoder_jpg_stop(void *decodeinfor)
{
	PVIM_JPEG_DECODE_INFO jpeg_decode_infor;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_jpg_stop()"));
	#endif
	
	jpeg_decode_infor	=(PVIM_JPEG_DECODE_INFO)decodeinfor;
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
	
	if(jpeg_decode_infor->DecodeMode==VIM_DECODE_NONE)
	{
		VIM_HAPI_DisplayJpeg_Release();
	}
	else
	{
		VIM_HAPI_DecodeJpeg_Release();
	}
	
	return ME_SUCCESS;
}

/*******************************************************************************************
Description:
	mp4 video decoder resume
Parameters:
       void *decodeinfor:	decoder information structor point
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.03.28		create first version
********************************************************************************************/
ME_U32 decoder_jpg_resume(void *decodeinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_jpg_resume()"));
	#endif
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));

	return ME_SUCCESS;
}

/*******************************************************************************************
Description:
	mp4 video decoder pause
Parameters:
       void *decodeinfor:	decoder information structor point
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.03.28		create first version
********************************************************************************************/
ME_U32 decoder_jpg_pause(void *decodeinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_jpg_pause()"));
	#endif
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));

	return ME_SUCCESS;
}

/*******************************************************************************************
Description:
	mp4 video decoder get max output data size(bytes)
Parameters:
       void *decodeinfor:	decoder information structor point
       ME_U32 maxsrcsize: video max src stream data length(bytes)
       ME_U32 *maxdstsize: video decoder output max yuv data length(bytes)
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.03.28		create first version
********************************************************************************************/
ME_U32 decoder_jpg_dst_buffersize(void *decodeinfor, ME_U32 maxsrcsize, ME_U32 *maxdstsize)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_jpg_get_decode_dst_buffersize()"));
	#endif
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
	*maxdstsize =192*1024;
	return ME_SUCCESS;
}

/*******************************************************************************************
Description:
	mp4 video decoder do API
Parameters:
       void *decodeinfor:	decoder information structor point
      	char *src:		src stream data buffer point	 
      	ME_U32 srclen:	src stream data length(bytes)	 
      	char *dst:		dst yuv data buffer point	 
      	ME_U32 *dstlen:	dst yuv data length(bytes)	 
      	ME_U32*decodelen: decoder used src data length(bytes)
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.03.28		create first version
********************************************************************************************/
ME_U32 decoder_jpg_decode(void *decodeinfor, char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen, ME_U32*decodelen)
{
	PVIM_JPEG_DECODE_INFO jpeg_decode_info;
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_jpg_decode()"));
	#endif
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((src!=NULL));
	jpeg_decode_info=(PVIM_JPEG_DECODE_INFO)decodeinfor;
	*decodelen = VIM_HAPI_DecodeJpeg((unsigned char *)src, srclen);
	ME_MESSAGE(("JBUFFER have %d bytes write-address: 0X%08X! \n",*decodelen, (ME_U32)src));
	return ME_SUCCESS;
}

/*******************************************************************************************
Description:
	mp4 video decoder do API
Parameters:
       void *decodeinfor:	decoder information structor point
      	char *src:		src stream data buffer point	 
      	char *dst:		dst yuv data buffer point	 
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.03.28		create first version
********************************************************************************************/
ME_U32 decoder_jpg_decode_frames(void *decodeinfor, void *src, void *dst)
{
	PVIM_JPEG_DECODE_INFO jpeg_decode_info;
	video_stream_infor	*src_buffer;
	ME_U32 	src_length, used_length, src_address;

	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_jpg_decode_frames()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((src!=NULL));
	
	jpeg_decode_info=(PVIM_JPEG_DECODE_INFO)decodeinfor;
	src_buffer  = (video_stream_infor	*)src;
	src_length = src_buffer->data_length-src_buffer->cursor_read;
	src_address = src_buffer->buffer_address+src_buffer->cursor_read;
	used_length = VIM_HAPI_DecodeJpeg((unsigned char *)src_address, src_length);
	if(used_length>=src_length)
		used_length = src_length;
	src_buffer->cursor_read += used_length;
	ME_MESSAGE(("JBUFFER have %d bytes write-address: 0X%08X! \n",used_length, src_address));

	return ME_SUCCESS;
}

/*******************************************************************************************
Description:
	mp4 video decoder object register
Parameters:
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.03.28		create first version
********************************************************************************************/
ME_U32 decoder_jpg_register(void)
{
#if ME_KM_DECODE_JPG_EN
	me_decoder_t	* pthis;

	#if ME_KM_FUNNAME
	ME_MESSAGE( ("\n->decoder_jpg_register()") );
	#endif

#if ME_KM_OBJ_STACK
	pthis	= &_g_decoder_jpg;
#else
	pthis	= (me_decoder_t *)me_malloc( sizeof(me_decoder_t) );
	if(pthis==NULL)
	{	
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		return	ME_ERR_MALLOC_MEMORY_ERROR;	
	}
#endif

	decoder_register_initialize(pthis);
	ME_SPRINTF(pthis->id, "jpg");
	pthis->init				= decoder_jpg_init;
	pthis->release			= decoder_jpg_release;
	pthis->seek				= decoder_jpg_seek;
	pthis->start				= decoder_jpg_start;
	pthis->stop				= decoder_jpg_stop;
	pthis->resume			= decoder_jpg_resume;
	pthis->pause				= decoder_jpg_pause;
	pthis->dst_buffersize		= decoder_jpg_dst_buffersize;
	pthis->decode_oneframe	= decoder_jpg_decode;
	pthis->decode_frames		= decoder_jpg_decode_frames;
	pthis->set_outputparam	= decoder_jpg_setoutparam;

	return me_decoder_register(pthis);	/*×¢²ájpg hardware decoder*/
#else
	return ME_SUCCESS;
#endif
}
#endif	//#if ME_KM_DECODE_JPG_EN	

