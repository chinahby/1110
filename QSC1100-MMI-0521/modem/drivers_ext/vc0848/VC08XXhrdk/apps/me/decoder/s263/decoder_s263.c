/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	decoder_s263.c
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2007/03/28
* Last modify : Amanda Deng 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/
#include "decoder_s263.h"

#if ME_KM_DECODE_S263_EN
#include "..\mp4v\decoder_mp4v.h"

#if ME_KM_OBJ_STACK
static me_decoder_t 	_g_decoder_s263;
#endif

/*******************************************************************************************
Description:
	mp4 video(s263) decoder object register
Parameters:
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.03.28		create first version
	   0.11			Amanda Deng			2007.11.22		modify input parameter
********************************************************************************************/
ME_U32 decoder_s263_register(void)
{
#if ME_KM_DECODE_S263_EN
	me_decoder_t	* pthis;

	#if ME_KM_FUNNAME
	ME_MESSAGE( ("\n->decoder_s263_register()") );
	#endif

#if ME_KM_OBJ_STACK
	pthis	= &_g_decoder_s263;
#else
	pthis	= (me_decoder_t *)me_malloc( sizeof(me_decoder_t) );
	if(pthis==NULL)
	{	
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		return	ME_ERR_MALLOC_MEMORY_ERROR;	
	}
#endif

	decoder_register_initialize(pthis);
	ME_SPRINTF(pthis->id, "s263");
	pthis->init				= decoder_mp4v_init;
	pthis->release			= decoder_mp4v_release;
	pthis->seek				= decoder_mp4v_seek;
	pthis->start				= decoder_mp4v_start;
	pthis->stop				= decoder_mp4v_stop;
	pthis->resume			= decoder_mp4v_resume;
	pthis->pause				= decoder_mp4v_pause;
	pthis->dst_buffersize		= decoder_mp4v_dst_buffersize;
	pthis->decode_oneframe	= decoder_mp4v_decode;
	pthis->decode_frames		= decoder_mp4v_decode_frames;
	pthis->set_outputparam	= decoder_mp4v_setoutparam;
	
	return	me_decoder_register(pthis);	/*×¢²ás263 decoder*/
#else
	return ME_SUCCESS;
#endif
}
#endif	//#if ME_KM_DECODE_S263_EN

