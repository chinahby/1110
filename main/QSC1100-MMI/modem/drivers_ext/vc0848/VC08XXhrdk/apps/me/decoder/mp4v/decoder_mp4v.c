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
#include "decoder_mp4v.h"

#if ME_KM_DECODE_MP4V_EN
#if ME_KM_OBJ_STACK
static me_decoder_t 	_g_decoder_mp4v;
#endif
extern UINT32 g_rotation;


#if HARDWARE_V0820_VERSION==V0820_MPW_SV2
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
********************************************************************************************/
ME_U32 decoder_mp4v_init(void *srcinfor, void **, void **decodeinfor)
{
	me_video_infor *video_infor;
	me_video_outinfor *video_outinfor;
	ME_U32  result;
	ME_U32  display_width,display_height;
	ME_U32  display_xpos,display_ypos;
	ME_U32  display_mode;
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_mp4v_init()"));
	#endif

	(*decodeinfor) = NULL;
	ME_ASSERT_PARAM_RETURN((srcinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((outinfor!=NULL));
	
	video_infor = (me_video_infor *)srcinfor;
	video_outinfor = (me_video_outinfor*)outinfor;
#ifndef _WIN32
	mpeg4_infor.mpeg4_height = video_infor->v_imageheight;
	mpeg4_infor.mpeg4_width = video_infor->v_imagewidth;
	mpeg4_infor.mpeg4_disxpos = video_outinfor->v_outxpos;
	mpeg4_infor.mpeg4_disypos = video_outinfor->v_outypos;
	mpeg4_infor.mpeg4_diswidth = video_outinfor->v_outwidth;
	mpeg4_infor.mpeg4_disheight = video_outinfor->v_outheight;
	
	mpeg4_infor.mpeg4_err_limit = 0x9fffffff; 
	mpeg4_infor.mpeg4_disp_mode = 0;		// 1, MBL MODE 0, frame mode
	mpeg4_infor.mpeg4_pingpong = 1;			//pingpang mode
	mpeg4_infor.mpeg4_err_mask = 1;		
	mpeg4_infor.mpeg4_inaddrtype = 0;
	
	result = mpeg4_decoder_init(video_infor->v_imagewidth,video_infor->v_imageheight);
	if(result)
	{
		ME_ERROR(("mpeg4_decoder_init return = 0x%08", result));
		(*decodeinfor) = NULL;
		return result;
	}
	// conifg LCD and mp4 decode infor
	display_width = mpeg4_infor.mpeg4_diswidth;
	display_height = mpeg4_infor.mpeg4_disheight;
	if (video_infor->v_imagewidth<display_width)
		display_width = video_infor->v_imagewidth;
	if(video_infor->v_imageheight<display_height)
		display_height = video_infor->v_imageheight;
	display_xpos = mpeg4_infor.mpeg4_disxpos +(mpeg4_infor.mpeg4_diswidth-display_width)/2;
	display_ypos = mpeg4_infor.mpeg4_disypos +(mpeg4_infor.mpeg4_disheight-display_height)/2;
	display_xpos &= 0xfffffffc;
	display_ypos &= 0xfffffffc;
	ME_ERROR((" video diaplay x pos is %d !",display_xpos));
	ME_ERROR((" video diaplay y pos is %d !",display_ypos));
	//ipp,lcdc,lbuf module initialize
	if(mpeg4_infor.mpeg4_disp_mode==1) 
	  	display_mode=2;
	else
		display_mode=3;
	display_set_view_path( mpeg4_infor.mpeg4_out0_addr, 
						    mpeg4_infor.mpeg4_out1_addr, 
						    display_mode, 
						    video_infor->v_imagewidth,video_infor->v_imageheight, 
						    display_xpos, display_ypos, 
						    display_width, display_height );
	disp_hide_win( LAYER_A0, DISP_DISABLE );
	(*decodeinfor) = (void *)&mpeg4_infor;
#endif
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
ME_U32 decoder_mp4v_release(void *decodeinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_mp4v_release()"));
	#endif
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
#ifndef _WIN32
	//display_view_init( 0, 0, 0,0, 0, 0, 0, 0, 0 );
	return mpeg4_decoder_release();
#else
	return ME_SUCCESS;
#endif
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
	   0.1			Amanda Deng			2007.09.19		create first version
********************************************************************************************/
ME_U32 decoder_mp4v_setoutparam(void *decodeinfor, void *outinfor)
{
	me_video_outinfor *vout_infor;
	ME_U32  display_width,display_height;
	ME_U32  display_xpos,display_ypos;
	ME_U32  display_mode;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_mp4v_setoutparam()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((outinfor!=NULL));
#ifndef _WIN32	
	vout_infor	= (me_video_outinfor*)outinfor;
	mpeg4_infor.mpeg4_disxpos = vout_infor->v_outxpos;
	mpeg4_infor.mpeg4_disypos = vout_infor->v_outypos;
	mpeg4_infor.mpeg4_diswidth = vout_infor->v_outwidth;
	mpeg4_infor.mpeg4_disheight = vout_infor->v_outheight;

	// conifg LCD and mp4 decode infor
	display_width = mpeg4_infor.mpeg4_diswidth;
	display_height = mpeg4_infor.mpeg4_disheight;
	if (mpeg4_infor.mpeg4_width<display_width)
		display_width = mpeg4_infor.mpeg4_width;
	if(mpeg4_infor.mpeg4_height<display_height)
		display_height = mpeg4_infor.mpeg4_height;
	display_xpos = mpeg4_infor.mpeg4_disxpos +(mpeg4_infor.mpeg4_diswidth-display_width)/2;
	display_ypos = mpeg4_infor.mpeg4_disypos +(mpeg4_infor.mpeg4_disheight-display_height)/2;
	display_xpos &= 0xfffffffc;
	display_ypos &= 0xfffffffc;
	ME_ERROR((" video diaplay x pos is %d !",display_xpos));
	ME_ERROR((" video diaplay y pos is %d !",display_ypos));
	//ipp,lcdc,lbuf module initialize
	if(mpeg4_infor.mpeg4_disp_mode==1) 
	  	display_mode=2;
	else
		display_mode=3;
	display_set_view_path( mpeg4_infor.mpeg4_out0_addr, 
						    mpeg4_infor.mpeg4_out1_addr, 
						    display_mode, 
						    mpeg4_infor.mpeg4_width,mpeg4_infor.mpeg4_height, 
						    display_xpos, display_ypos, 
						    display_width, display_height );
	disp_hide_win( LAYER_A0, DISP_DISABLE );
#endif
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
ME_U32 decoder_mp4v_seek(void *decodeinfor, ME_U32 pos)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_mp4v_seek()"));
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
ME_U32 decoder_mp4v_start(void *decodeinfor)
{
	ME_U32  display_width,display_height;
	ME_U32  display_xpos,display_ypos;
	ME_U32  display_mode;
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_mp4v_start()"));
	#endif	
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
#ifndef _WIN32
	mpeg4_restart_decode();
	// conifg LCD and mp4 decode infor
	display_width = mpeg4_infor.mpeg4_diswidth;
	display_height = mpeg4_infor.mpeg4_disheight;
	if (mpeg4_infor.mpeg4_width<display_width)
		display_width = mpeg4_infor.mpeg4_width;
	if(mpeg4_infor.mpeg4_height<display_height)
		display_height = mpeg4_infor.mpeg4_height;
	display_xpos = mpeg4_infor.mpeg4_disxpos +(mpeg4_infor.mpeg4_diswidth-display_width)/2;
	display_ypos = mpeg4_infor.mpeg4_disypos +(mpeg4_infor.mpeg4_disheight-display_height)/2;
	display_xpos &= 0xfffffffc;
	display_ypos &= 0xfffffffc;
	ME_ERROR((" video diaplay x pos is %d !",display_xpos));
	ME_ERROR((" video diaplay y pos is %d !",display_ypos));
	//ipp,lcdc,lbuf module initialize
	if(mpeg4_infor.mpeg4_disp_mode==1) 
	  	display_mode=2;
	else
		display_mode=3;
	display_set_view_path( mpeg4_infor.mpeg4_out0_addr, 
						    mpeg4_infor.mpeg4_out1_addr, 
						    display_mode, 
						    mpeg4_infor.mpeg4_width,mpeg4_infor.mpeg4_height, 
						    display_xpos, display_ypos, 
						    display_width, display_height );
	disp_hide_win( LAYER_A0, DISP_DISABLE );
#endif
	return ME_SUCCESS;
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
ME_U32 decoder_mp4v_stop(void *decodeinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_mp4v_stop()"));
	#endif
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
#ifndef _WIN32
	mpeg4_pause_decode();
#endif
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
ME_U32 decoder_mp4v_resume(void *decodeinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_mp4v_resume()"));
	#endif
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
#ifndef _WIN32
	mpeg4_resume_decode();
#endif
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
ME_U32 decoder_mp4v_pause(void *decodeinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_mp4v_pause()"));
	#endif
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
#ifndef _WIN32
	mpeg4_pause_decode();
#endif
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
ME_U32 decoder_mp4v_dst_buffersize(void *decodeinfor, ME_U32 maxsrcsize, ME_U32 *maxdstsize)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_mp4v_get_decode_dst_buffersize()"));
	#endif
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
#ifndef _WIN32
	*maxdstsize =mpeg4_infor.mpeg4_height * mpeg4_infor.mpeg4_width *2;
#else
	*maxdstsize =320*240*2;
#endif
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
ME_U32 decoder_mp4v_decode(void *decodeinfor, char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen, ME_U32*decodelen)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_mp4v_decode()"));
	#endif
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((src!=NULL));
#ifndef _WIN32
	if(mpeg4_infor.block_start_flag)
		{mpeg4_set_source_stream((ME_U32)src,srclen);}
	mpeg4_start_decoder();
#endif
	return ME_SUCCESS;
}

ME_U32 decoder_mp4v_decode_frames(void *decodeinfor, void *src, void *dst)
{
	video_stream_infor	*src_buffer;

	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_mp4v_decode()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((src!=NULL));
#ifndef _WIN32	
	src_buffer  = (video_stream_infor	*)src;
	if(mpeg4_infor.block_start_flag)
	{
		//check if is last block
		if(src_buffer->is_over)
			mpeg4_infor.last_block_flag = TRUE;
		mpeg4_set_source_stream((ME_U32)src_buffer->buffer_address,
									src_buffer->data_length);
	}
	mpeg4_start_decoder();
#endif
	return ME_SUCCESS;
}
#endif

#if HARDWARE_V0820_VERSION==V0820_FPGA
//extern void video_decode_callback(ME_U32 flag);

mp4v_decinfor_t  mp4v_decinfor;

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
	   0.1			Amanda Deng			2007.10.30		create first version
	   0.11			Amanda Deng			2007.11.22		modify input parameter
********************************************************************************************/
ME_U32 decoder_mp4v_init(void *srcinfor, void *outinfor, void **decodeinfor)
{
	me_video_infor *video_infor;
	me_video_outinfor *video_outinfor;
#ifndef WIN32
	VIM_MPEG4INFO info;
#endif

	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_mp4v_init()"));
	#endif

	(*decodeinfor) = NULL;
	ME_ASSERT_PARAM_RETURN((srcinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((outinfor!=NULL));
	
	video_infor = (me_video_infor *)srcinfor;
	video_outinfor = (me_video_outinfor*)outinfor;
	
	mp4v_decinfor.dec_start_flag = TRUE;		//when need start decode mp4 set this flag
	mp4v_decinfor.vop_start_flag 	= TRUE;		//when one frame end, need set this flag
	mp4v_decinfor.block_start_flag	= TRUE;		//when data block end, need set this flag
	mp4v_decinfor.block_last_flag 	= FALSE;		//when the data block is last one set this flag

	video_outinfor->v_srcwidth = video_infor->v_imagewidth;
	video_outinfor->v_srcheight = video_infor->v_imageheight;
	video_outinfor->v_srcxpos = 0;
	video_outinfor->v_srcypos = 0;

	if(video_outinfor->v_outrotation%2)
	{
		if (video_infor->v_imagewidth<video_outinfor->v_outwidth)
			video_outinfor->v_outwidth = video_infor->v_imagewidth;
		if(video_infor->v_imageheight<video_outinfor->v_outheight)
			video_outinfor->v_outheight = video_infor->v_imageheight;
	}
	mp4v_decinfor.v_height		= video_infor->v_imageheight;
	mp4v_decinfor.v_width 		= video_infor->v_imagewidth;
	mp4v_decinfor.disp_xpos		= video_outinfor->v_outxpos;
	mp4v_decinfor.disp_ypos		= video_outinfor->v_outypos;
	mp4v_decinfor.disp_width		= video_outinfor->v_outwidth;
	mp4v_decinfor.disp_height 	= video_outinfor->v_outheight;
	mp4v_decinfor.disp_rotation 	= video_outinfor->v_outrotation;
	mp4v_decinfor.disp_effect 		= video_outinfor->v_outeffect;

#ifndef WIN32	
	info.SorHeigth		=mp4v_decinfor.v_height;
	info.SorWidth		=mp4v_decinfor.v_width;
	info.DecPoint_x	=mp4v_decinfor.disp_xpos;
	info.DecPoint_y	=mp4v_decinfor.disp_ypos	;
	info.DecWidth		=mp4v_decinfor.disp_width;
	info.DecHeigth		=mp4v_decinfor.disp_height;
	info.FrameRate=30;
#endif
	
//	if(mp4v_decinfor.callback!=NULL)
//		info.Mp4DecCallback= mp4v_decinfor.callback;
//	else
//		info.Mp4DecCallback= video_decode_callback;
	
	(*decodeinfor) = (void *)&mp4v_decinfor;
	VIM_HAPI_SetLCDMirror((VIM_HAPI_ROTATEMODE)mp4v_decinfor.disp_rotation, VIM_HAPI_A_LAYER);
	VIM_HAPI_SetEffect((VIM_HAPI_SPECIAL_EFFECT)mp4v_decinfor.disp_effect);
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
	   0.1			Amanda Deng			2007.10.30		create first version
********************************************************************************************/
ME_U32 decoder_mp4v_release(void *decodeinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_mp4v_release()"));
	#endif
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
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
	   0.1			Amanda Deng			2007.10.30		create first version
********************************************************************************************/
ME_U32 decoder_mp4v_setoutparam(void *decodeinfor, void *outinfor)
{
	me_video_outinfor *video_outinfor;
#ifndef WIN32
	VIM_MPEG4INFO info;
#endif
		
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_mp4v_setoutparam()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((outinfor!=NULL));
#ifndef _WIN32	
	video_outinfor	= (me_video_outinfor*)outinfor;

	if(video_outinfor->v_outrotation%2)
	{
		if (mp4v_decinfor.v_width<video_outinfor->v_outwidth)
			video_outinfor->v_outwidth = mp4v_decinfor.v_width;
		if(mp4v_decinfor.v_height<video_outinfor->v_outheight)
			video_outinfor->v_outheight = mp4v_decinfor.v_height;
	}
	mp4v_decinfor.disp_xpos		= video_outinfor->v_outxpos;
	mp4v_decinfor.disp_ypos		= video_outinfor->v_outypos;
	mp4v_decinfor.disp_width		= video_outinfor->v_outwidth;
	mp4v_decinfor.disp_height 	= video_outinfor->v_outheight;
	mp4v_decinfor.disp_rotation 	= video_outinfor->v_outrotation;
	mp4v_decinfor.disp_effect 		= video_outinfor->v_outeffect;

	info.SorHeigth		=mp4v_decinfor.v_height;
	info.SorWidth		=mp4v_decinfor.v_width;
	info.DecPoint_x	= video_outinfor->v_outxpos;
	info.DecPoint_y	= video_outinfor->v_outypos;
	info.DecWidth		= video_outinfor->v_outwidth;
	info.DecHeigth		= video_outinfor->v_outheight;
	info.FrameRate=30;
	
//	if(mp4v_decinfor.callback!=NULL)
//		info.Mp4DecCallback= mp4v_decinfor.callback;
//	else
//		info.Mp4DecCallback= video_decode_callback;
	
	VIM_HAPI_SetLCDMirror((VIM_HAPI_ROTATEMODE)mp4v_decinfor.disp_rotation, VIM_HAPI_A_LAYER);
	VIM_HAPI_SetEffect((VIM_HAPI_SPECIAL_EFFECT)mp4v_decinfor.disp_effect);
	VIM_HAPI_VideoResize(&info);
#endif
	
	return ME_SUCCESS;
}

ME_U32 decoder_mp4v_getoutparam(void *decodeinfor, void *outinfor)
{
	me_video_outinfor *video_outinfor;
#ifndef WIN32
	VIM_VIDEO_OUTINFO info;
#endif
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_mp4v_getoutparam()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((outinfor!=NULL));
#ifndef _WIN32	
	video_outinfor	= (me_video_outinfor*)outinfor;
	VIM_HAPI_GetDecInfo(&info);
	mp4v_decinfor.v_height		= video_outinfor->v_srcheight	=info.v_srcheight;
	mp4v_decinfor.v_width 		= video_outinfor->v_srcwidth	=info.v_srcwidth;
	mp4v_decinfor.disp_xpos		= video_outinfor->v_outxpos	=info.v_outxpos;
	mp4v_decinfor.disp_ypos		= video_outinfor->v_outypos	=info.v_outypos;
	mp4v_decinfor.disp_width		= video_outinfor->v_outwidth	=info.v_outwidth;
	mp4v_decinfor.disp_height 	= video_outinfor->v_outheight	=info.v_outheight;
	mp4v_decinfor.disp_rotation 	= video_outinfor->v_outrotation	=info.v_outrotation;
	mp4v_decinfor.disp_effect 		= video_outinfor->v_outeffect	=info.v_outeffect;
#endif
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
	   0.1			Amanda Deng			2007.10.30		create first version
********************************************************************************************/
ME_U32 decoder_mp4v_seek(void *decodeinfor, ME_U32 pos)
{
#ifndef WIN32
	VIM_MPEG4INFO info;
#endif
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_mp4v_seek()"));
	#endif
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
#ifndef WIN32
	//stop video decode, because after seek all data is reset
	VIM_HAPI_VideoStopDec();
	//initialize video decode module
	mp4v_decinfor.dec_start_flag 	= TRUE;		//when need start decode mp4 set this flag
	mp4v_decinfor.vop_start_flag 	= TRUE;		//when one frame end, need set this flag
#if VIM_USER_MPEG4_VOPMODE	
	mp4v_decinfor.block_start_flag	= FALSE;		//when data block end, need set this flag
#else
	mp4v_decinfor.block_start_flag	= TRUE;		//when data block end, need set this flag
#endif	
	mp4v_decinfor.block_last_flag 	= FALSE;		//when the data block is last one set this flag
	
	info.SorHeigth		=mp4v_decinfor.v_height;
	info.SorWidth		=mp4v_decinfor.v_width;
	info.DecPoint_x	=mp4v_decinfor.disp_xpos;
	info.DecPoint_y	=mp4v_decinfor.disp_ypos;
	info.DecWidth		=mp4v_decinfor.disp_width;
	info.DecHeigth		=mp4v_decinfor.disp_height;
	
	info.FrameRate=30;
	
//	if(mp4v_decinfor.callback!=NULL)
//		info.Mp4DecCallback= mp4v_decinfor.callback;
//	else
//		info.Mp4DecCallback= video_decode_callback;

//	VIM_HAPI_SetLCDMirror((VIM_HAPI_ROTATEMODE)mp4v_decinfor.disp_rotation, VIM_HAPI_A_LAYER);
//	VIM_HAPI_SetEffect((VIM_HAPI_SPECIAL_EFFECT)mp4v_decinfor.disp_effect);
//	VIM_HAPI_VideoDecInit(&info);
//	VIM_HAPI_VideoResize(&info);
#endif

	return ME_SUCCESS;
}

VIM_MPEG4INFO mp4v_start_info;

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
	   0.1			Amanda Deng			2007.10.30		create first version
********************************************************************************************/
ME_U32 decoder_mp4v_start(void *decodeinfor)
{
	ME_U32 ret=0;
#ifndef WIN32
	//VIM_MPEG4INFO info;
#endif
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_mp4v_start()"));
	#endif	
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
#ifndef _WIN32
	
	mp4v_decinfor.dec_start_flag 	= TRUE;		//when need start decode mp4 set this flag
	mp4v_decinfor.vop_start_flag 	= TRUE;		//when one frame end, need set this flag
#if VIM_USER_MPEG4_VOPMODE	
	mp4v_decinfor.block_start_flag	= FALSE;		//when data block end, need set this flag
#else
	mp4v_decinfor.block_start_flag	= TRUE;		//when data block end, need set this flag
#endif	
	mp4v_decinfor.block_last_flag 	= FALSE;		//when the data block is last one set this flag
	
	mp4v_start_info.SorHeigth		=mp4v_decinfor.v_height;
	mp4v_start_info.SorWidth		=mp4v_decinfor.v_width;
	
	if(mp4v_decinfor.disp_rotation)
	{
		mp4v_start_info.DecWidth	=MMD_LCD_HEIGHT;//mp4v_decinfor.disp_height;
		mp4v_start_info.DecHeigth=MMD_LCD_WIDTH;//mp4v_decinfor.disp_width;
		mp4v_start_info.DecPoint_x	=0;//mp4v_decinfor.disp_xpos;
	   mp4v_start_info.DecPoint_y	=0;//mp4v_decinfor.disp_ypos;
	}
	else
	{
	   mp4v_start_info.DecWidth		=mp4v_decinfor.disp_width;
	   mp4v_start_info.DecHeigth		=mp4v_decinfor.disp_height;
      mp4v_start_info.DecPoint_x	=mp4v_decinfor.disp_xpos;
	   mp4v_start_info.DecPoint_y	=mp4v_decinfor.disp_ypos;
      
	}
	
	mp4v_start_info.FrameRate=30;
	
//	if(mp4v_decinfor.callback!=NULL)
//		info.Mp4DecCallback= mp4v_decinfor.callback;
//	else
//		info.Mp4DecCallback= video_decode_callback;

	
	ret = VIM_HAPI_VideoDecInit(&mp4v_start_info);
	if(ret)
	{
		VIM_HAPI_VideoStopDec();
		return ret;
	}
	VIM_HAPI_VideoResize(&mp4v_start_info);
	//VIM_DISP_ResetState();
	//VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,ENABLE);
	VIM_HAPI_SetMPEGDecIntStatus(0);
	
//	gtmp_frmind=0;
#endif
	return ME_SUCCESS;
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
	   0.1			Amanda Deng			2007.10.30		create first version
********************************************************************************************/
ME_U32 decoder_mp4v_stop(void *decodeinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_mp4v_stop()"));
	#endif
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
#ifndef _WIN32
	VIM_HAPI_VideoStopDec();
#endif
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
	   0.1			Amanda Deng			2007.10.30		create first version
********************************************************************************************/
ME_U32 decoder_mp4v_resume(void *decodeinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_mp4v_resume()"));
	#endif
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
#ifndef _WIN32
	VIM_HAPI_EnableStuckError();
#endif
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
	   0.1			Amanda Deng			2007.10.30		create first version
********************************************************************************************/
ME_U32 decoder_mp4v_pause(void *decodeinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_mp4v_pause()"));
	#endif
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
#ifndef _WIN32
	VIM_HAPI_DisableStuckError();
#endif
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
	   0.1			Amanda Deng			2007.10.30		create first version
********************************************************************************************/
ME_U32 decoder_mp4v_dst_buffersize(void *decodeinfor, ME_U32 maxsrcsize, ME_U32 *maxdstsize)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_mp4v_get_decode_dst_buffersize()"));
	#endif
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
#ifndef _WIN32
	*maxdstsize =mp4v_decinfor.v_height * mp4v_decinfor.v_width *2;
#else
	*maxdstsize =320*240*2;
#endif
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
	   0.1			Amanda Deng			2007.10.30		create first version
********************************************************************************************/
ME_U32 decoder_mp4v_decode(void *decodeinfor, char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen, ME_U32*decodelen)
{
	ME_U32 dec_flag = 0;
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_mp4v_decode()"));
	#endif
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((src!=NULL));
#ifndef _WIN32
	//check decode flag
	if (mp4v_decinfor.dec_start_flag)
		dec_flag|=VIM_MPEG4_MP4_START;
	if (mp4v_decinfor.vop_start_flag)
		dec_flag|=VIM_MPEG4_VOP_START;
	if (mp4v_decinfor.block_start_flag)
		dec_flag|=VIM_MPEG4_BLOCK_START;
	if (mp4v_decinfor.block_last_flag)
		dec_flag|=VIM_MPEG4_LAST_BLOCK;						
 	//reset decod configuration
	mp4v_decinfor.dec_start_flag 	= FALSE;		//when need start decode mp4 set this flag
	mp4v_decinfor.vop_start_flag 	= FALSE;		//when one frame end, need set this flag
	mp4v_decinfor.block_start_flag	= FALSE;		//when data block end, need set this flag
	mp4v_decinfor.block_last_flag 	= FALSE;		//when the data block is last one set this flag
	//start decode done
	VIM_HAPI_StartNewBlockVop((ME_U32)src, srclen, dec_flag);
#endif
	return ME_SUCCESS;
}

ME_U32 decoder_mp4v_decode_frames(void *decodeinfor, void *src, void *dst)
{
	video_stream_infor	*src_buffer;
	ME_U32 dec_flag = 0;
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_mp4v_decode()"));
	#endif
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((src!=NULL));
#ifndef _WIN32	
	src_buffer  = (video_stream_infor	*)src;
	//check if is last block
#if	!(VIM_USER_MPEG4_VOPMODE)
	if(src_buffer->is_over)
		mp4v_decinfor.block_last_flag = TRUE;
#endif
	//check decode flag
	if (mp4v_decinfor.dec_start_flag)
		dec_flag|=VIM_MPEG4_MP4_START;
	if (mp4v_decinfor.vop_start_flag)
		dec_flag|=VIM_MPEG4_VOP_START;
	if (mp4v_decinfor.block_start_flag)
		dec_flag|=VIM_MPEG4_BLOCK_START;
	if (mp4v_decinfor.block_last_flag)
		dec_flag|=VIM_MPEG4_LAST_BLOCK;	
 	//reset decod configuration
	mp4v_decinfor.dec_start_flag 	= FALSE;		//when need start decode mp4 set this flag
	mp4v_decinfor.vop_start_flag 	= FALSE;		//when one frame end, need set this flag
	mp4v_decinfor.block_start_flag	= FALSE;		//when data block end, need set this flag
	mp4v_decinfor.block_last_flag 	= FALSE;		//when the data block is last one set this flag
	//start decode done
//	VIM_HAPI_StartNewBlockVop((ME_U32)src_buffer->buffer_address, src_buffer->data_length, dec_flag);
	if(g_rotation)
	{	
		g_rotation = 0;
		//VIM_DISP_ResetState();
		//VIM_DISP_SetLayerEnable(VIM_DISP_ALAYER,ENABLE);
		
	}
	VIM_HAPI_StartNewBlockVop((ME_U32)src_buffer->buffer_address+src_buffer->cursor_read, src_buffer->data_length, dec_flag);
#endif
	return ME_SUCCESS;
}
#endif

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
ME_U32 decoder_mp4v_register(void)
{
#if ME_KM_DECODE_MP4V_EN
	me_decoder_t	* pthis;

	#if ME_KM_FUNNAME
	ME_MESSAGE( ("\n->decoder_mp4v_register()") );
	#endif

#if ME_KM_OBJ_STACK
	pthis	= &_g_decoder_mp4v;
#else
	pthis	= (me_decoder_t *)me_malloc( sizeof(me_decoder_t) );
	if(pthis==NULL)
	{	
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		return	ME_ERR_MALLOC_MEMORY_ERROR;	
	}
#endif

	decoder_register_initialize(pthis);
	ME_SPRINTF(pthis->id, "mp4v");
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
	pthis->get_outputparam	= decoder_mp4v_getoutparam;
	return me_decoder_register(pthis);	/*×¢²ámp4v decoder*/
#else
	return ME_SUCCESS;
#endif
}
#endif	//#if ME_KM_DECODE_MP4V_EN

