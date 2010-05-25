/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	encoder_jpg.h
* User for : 	820
* Creat By : 	Zhou Jian                                                                 
* CrateDate: 	2007/11/20
* Last modify : Amanda Deng 2007-xx-xx     
*
* release history:
* 		
=======================================================================*/
#include "encoder_jpg.h"

#if ME_KM_ENCODE_JPG_EN
#include "..\..\..\..\Drivers\include\VIM_Drivers.h"

#if ME_KM_OBJ_STACK
static me_encoder_t 	_g_encoder_jpg;
#endif

typedef struct
{
	VIM_HAPI_SAVE_MODE	savemode;			/* bit length */
	ME_U32		bufferlength;				/* 1: mono, 2: stereo */
	VIM_HAPI_CallBack	jpegcapturecallback;		/* sample rate */
} jpg_encinfor_t;

//extern ME_U32 jpeg_capture_callback(ME_U32 flag, ME_U32 size);

/*******************************************************************************************
Description:
	amr encoder initialize
Parameters:
       void *headinfor:		audio information structor point
       void **encodeinfor:	encoder information structor point's point
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.08.14		create 
********************************************************************************************/
ME_U32 encoder_jpg_init(void *recinfor, void* inputinfor, void **encodeinfor)
{
	me_video_infor 	*video_infor;
	jpg_encinfor_t 	*jpg_infor;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->encoder_jpg_init()"));
	#endif

	ME_ASSERT_PARAM_RETURN((recinfor!=NULL));
	video_infor =(me_video_infor *)recinfor;
	jpg_infor = me_malloc(sizeof(jpg_encinfor_t));
	if(jpg_infor!=NULL)
	{
		jpg_infor->bufferlength=120*1024;
		jpg_infor->savemode=VIM_HAPI_RAM_SAVE;
//#if ME_CAPTURE_TEST
		//jpg_infor->jpegcapturecallback=(VIM_HAPI_CallBack)video_infor->v_others;  
//#else
		//jpg_infor->jpegcapturecallback=(VIM_HAPI_CallBack)jpeg_capture_callback;
//#endif
		jpg_infor->jpegcapturecallback=NULL;
		(*encodeinfor)=(void *)jpg_infor;
		return ME_SUCCESS;
	}
	else
	{
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		(*encodeinfor) = NULL;
		return ME_ERR_MALLOC_MEMORY_ERROR;
	}
}

/*******************************************************************************************
Description:
	amr encoder release
Parameters:
       void *encodeinfor:	encoder information structor point
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.08.14		create 
********************************************************************************************/
ME_U32 encoder_jpg_release(void *encodeinfor)
{
	jpg_encinfor_t *jpg_infor;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->encoder_jpg_release()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((encodeinfor!=NULL));

	jpg_infor = (jpg_encinfor_t *)encodeinfor;
	me_free(jpg_infor);
	return ME_SUCCESS;
}

/*******************************************************************************************
Description:
	amr encoder start
Parameters:
       void *encodeinfor:	encoder information structor point
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.08.14		create 
********************************************************************************************/
ME_U32 encoder_jpg_start(void *encodeinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->encoder_jpg_start()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((encodeinfor!=NULL));
	
	return ME_SUCCESS;
}

/*******************************************************************************************
Description:
	amr encoder stop
Parameters:
       void *encodeinfor:	encoder information structor point
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.07.19		create first version
********************************************************************************************/
ME_U32 encoder_jpg_stop(void *encodeinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->encoder_jpg_stop()"));
	#endif
	ME_ASSERT_PARAM_RETURN((encodeinfor!=NULL));
	VIM_HAPI_CaptureJpeg_Release();
	return ME_SUCCESS;
}

/*******************************************************************************************
Description:
	amr encoder resume
Parameters:
       void *encodeinfor:	encoder information structor point
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.07.19		create first version
********************************************************************************************/
ME_U32 encoder_jpg_resume(void *encodeinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->encoder_jpg_resume()"));
	#endif
	ME_ASSERT_PARAM_RETURN((encodeinfor!=NULL));
	return ME_SUCCESS;
}

/*******************************************************************************************
Description:
	amr encoder pause
Parameters:
       void *decodeinfor:	encoder information structor point
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.07.19		create first version
********************************************************************************************/
ME_U32 encoder_jpg_pause(void *encodeinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->encoder_jpg_pause()"));
	#endif
	ME_ASSERT_PARAM_RETURN((encodeinfor!=NULL));
	return ME_SUCCESS;
}


/*******************************************************************************************
Description:
	amr encoder get max input data size(bytes)
Parameters:
       void *decodeinfor:		encoder information structor point
       ME_U32 *maxsrcsize: 	audio encoder input max PCM data length(bytes)
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.07.19		create first version
********************************************************************************************/
ME_U32 encoder_jpg_src_buffersize(void *encodeinfor, ME_U32 *maxsrcsize)
{
	//jpg_encinfor_t *jpg_infor;
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->encoder_jpg_get_max_src_size()"));
	#endif
	*maxsrcsize = 64*1024;
	ME_ASSERT_PARAM_RETURN((encodeinfor!=NULL));
	//jpg_infor = (jpg_encinfor_t *)encodeinfor;
	return ME_SUCCESS;
}

/*******************************************************************************************
Description:
	amr encoder get max output data size(bytes)
Parameters:
       void *decodeinfor:		encoder information structor point
       ME_U32 *maxdstsize: 	audio encoder output max amr data length(bytes)
Return:	
      ME_SUCCESS:		no error;
      Other Value: 		error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.07.19		create first version
********************************************************************************************/
ME_U32 encoder_jpg_dst_buffersize(void *encodeinfor, ME_U32 *maxdstsize)
{
		
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->encoder_jpg_get_decode_dst_buffersize()"));
	#endif
#if 1//(CHIP_ID!=ID_VC0878)	
	*maxdstsize = 100*1024*5;
#else
	*maxdstsize = 80*1024;
#endif
	ME_ASSERT_PARAM_RETURN((encodeinfor!=NULL));
	
	return ME_SUCCESS;
}

/*******************************************************************************************
Description:
	amr encoder do API(enocde one or more frame audio)
Parameters:
       void *encodeinfor:	encoder information structor point
      	char *src:		src stream data buffer point	 
      	ME_U32 srclen:	src stream data length(bytes)	 
      	char *dst:		dst amr data buffer point	 
      	ME_U32 *dstlen:	dst amr data length(bytes)	 
      	ME_U32*encodelen: encoder used src data length(bytes)
Return:	
      ME_SUCCESS:		no error;
      Other Value: 		error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.07.19		create first version
********************************************************************************************/
ME_U32 encoder_jpg_encode_frames(void *encodeinfor, void *src, void *dst)
{
	jpg_encinfor_t 	*jpg_infor;
	video_stream_infor	*dst_buffer;
	ME_U32  	dst_address, dstbuffer_length;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->encoder_jpg_decode()"));
	#endif

	ME_ASSERT_PARAM_RETURN((encodeinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((dst!=NULL));
	
	jpg_infor = (jpg_encinfor_t *)encodeinfor;
	dst_buffer  = (video_stream_infor	*)dst;
	//get src address, data length and dst address
	dst_address	= dst_buffer->buffer_address + dst_buffer->cursor_write;
	dstbuffer_length = dst_buffer->buffer_size -dst_buffer->cursor_write;
#if 1
	jpg_infor->savemode = VIM_HAPI_RAM_SAVE;
	return VIM_HAPI_CaptureStill(jpg_infor->savemode,(void*)dst_address,dstbuffer_length, jpg_infor->jpegcapturecallback);
#else
	jpg_infor->savemode = VIM_HAPI_ROM_SAVE;
	return VIM_HAPI_CaptureStill(jpg_infor->savemode,NULL, 0, jpg_infor->jpegcapturecallback);
#endif
}

/*******************************************************************************************
Description:
	amr encoder object register
Parameters:
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.07.19		create first version
********************************************************************************************/
ME_U32 encoder_jpg_register(void)
{
	me_encoder_t	* pthis;

	#if ME_KM_FUNNAME
	ME_MESSAGE(("->encoder_jpg_register()"));
	#endif
	
#if ME_KM_OBJ_STACK
	pthis	= &_g_encoder_jpg;
#else
	pthis	= (me_encoder_t *)me_malloc( sizeof(me_encoder_t));
	if(pthis==NULL)
	{	
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		return	ME_ERR_MALLOC_MEMORY_ERROR;	
	}
#endif
	
	encoder_register_initialize(pthis);
	ME_SPRINTF(pthis->id, "jpg");
	pthis->init				= encoder_jpg_init;
	pthis->release			= encoder_jpg_release;
	pthis->start				= encoder_jpg_start;
	pthis->stop				= encoder_jpg_stop;
	pthis->pause				= encoder_jpg_pause;
	pthis->resume			= encoder_jpg_resume;
	pthis->src_buffersize		= encoder_jpg_src_buffersize;
	pthis->dst_buffersize		= encoder_jpg_dst_buffersize;
	pthis->encode_frames		= encoder_jpg_encode_frames;
	return me_encoder_register(pthis);	/*×¢²ájpg encoder*/

}
#endif	//#if ME_KM_ENCODE_JPG_EN
