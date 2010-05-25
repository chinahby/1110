/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	me_object.c 
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/12/31
* Last modify : Amanda Deng 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/

#include "me_comhead.h"
#include "me_object.h"

/*******************************************************************************************
Description:
       judge the parser type is our need,
Parameters:	
	parser: point of the parser
	type: our need parser type point(4 bytes length)
Return:	
	TRUE : the parser is our need
	FALSE: the parser is not our need
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.11		create first version
********************************************************************************************/
ME_U32 me_parser_check_id(me_parser_t *parser, char *type)
{
	if(parser->id[0] == type[0]  && parser->id[1] == type[1] 
      && parser->id[2] == type[2]  && parser->id[3] == type[3])
		return ME_TRUE;
	else
		return ME_FALSE;
}

/*******************************************************************************************
Description:
       judge the muxer type is our need,
Parameters:	
	parser: point of the muxer
	type: our need muxer type point(4 bytes length)
Return:	
	TRUE : the muxer is our need
	FALSE: the muxer is not our need
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.11		create first version
********************************************************************************************/
ME_U32 me_muxer_check_id(me_muxer_t *muxer, char *type)
{
	if(muxer->id[0] == type[0]  && muxer->id[1] == type[1] 
      && muxer->id[2] == type[2]  && muxer->id[3] == type[3])
		return ME_TRUE;
	else
		return ME_FALSE;
}

/*******************************************************************************************
Description:
       judge the decoder type is our need,
Parameters:	
	parser: point of the decoder 
	type: our need decoder type point(4 bytes length)
Return:	
	TRUE : the decoder  is our need
	FALSE: the decoder  is not our need
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.11		create first version
********************************************************************************************/
ME_U32 me_decoder_check_id(me_decoder_t *decoder , char *type)
{
	if(decoder ->id[0] == type[0]  && decoder ->id[1] == type[1] 
      && decoder ->id[2] == type[2]  && decoder ->id[3] == type[3])
		return ME_TRUE;
	else
		return ME_FALSE;
}


/*******************************************************************************************
Description:
       judge the encoder type is our need,
Parameters:	
	parser: point of the encoder
	type: our need encoder type point(4 bytes length)
Return:	
	TRUE : the encoder is our need
	FALSE: the encoder is not our need
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.11		create first version
********************************************************************************************/
ME_U32 me_encoder_check_id(me_encoder_t *encoder, char *type)
{
	if(encoder->id[0] == type[0]  && encoder->id[1] == type[1] 
      && encoder->id[2] == type[2]  && encoder->id[3] == type[3])
		return ME_TRUE;
	else
		return ME_FALSE;
}


/*******************************************************************************************
Description:
       judge the id3 parser type is our need,
Parameters:	
	parser: point of the id3 parser
	type: our need id3 parser type point(4 bytes length)
Return:	
	TRUE : the id3 parser is our need
	FALSE: the id3 parser is not our need
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.11		create first version
********************************************************************************************/
ME_U32 me_id3_check_id(me_id3_t *id3, char *type)
{
	if(id3->id[0] == type[0]  && id3->id[1] == type[1] 
      && id3->id[2] == type[2]  && id3->id[3] == type[3])
		return ME_TRUE;
	else
		return ME_FALSE;
}

/*******************************************************************************************
Description:
       judge the parser type is our need,
Parameters:	
	parser: point of the parser
	type: our need atom type point(4 bytes length)
Return:	
	TRUE : the parser is our need
	FALSE: the parser is not our need
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.11		create first version
********************************************************************************************/
ME_U32 me_effectproc_check_id(me_effectproc_t *postproc, char *type)
{
	if(postproc->id[0] == type[0]  && postproc->id[1] == type[1] 
      && postproc->id[2] == type[2]  && postproc->id[3] == type[3])
		return ME_TRUE;
	else
		return ME_FALSE;
}

ME_U32 me_match_char32(char *input, char *output)
{
	if(input[0] == output[0] &&input[1] == output[1] 
	&& input[2] == output[2] &&input[3] == output[3])
		return ME_TRUE;
	else 
		return ME_FALSE;
}

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
   ==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						no use function define for decoder. Amanda Deng
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
ME_U32 decoder_fornothing_init(void *srcinfor, void *outinfor, void **decodeinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_fornothing_init()"));
	#endif
	(*decodeinfor) = NULL;
	ME_ASSERT_PARAM_RETURN((srcinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((outinfor!=NULL));
	return ME_SUCCESS;
}

ME_U32 decoder_fornothing_release(void *decodeinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_fornothing_release()"));
	#endif
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
	return ME_SUCCESS;
}

ME_U32 decoder_fornothing_setoutparam(void *decodeinfor, void *outinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_fornothing_setoutparam()"));
	#endif
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((outinfor!=NULL));
	return ME_SUCCESS;
}

ME_U32 decoder_fornothing_getoutparam(void *decodeinfor, void *outinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_fornothing_setoutparam()"));
	#endif
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((outinfor!=NULL));
	return ME_SUCCESS;
}

ME_U32 decoder_fornothing_seek(void *decodeinfor, ME_U32 pos)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_fornothing_seek()"));
	#endif
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
	return ME_SUCCESS;
}

ME_U32 decoder_fornothing_start(void *decodeinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_samr_start()"));
	#endif
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
	return ME_SUCCESS;
}

ME_U32 decoder_fornothing_stop(void *decodeinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_fornothing_stop()"));
	#endif
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
	return ME_SUCCESS;
}

ME_U32 decoder_fornothing_resume(void *decodeinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_fornothing_resume()"));
	#endif
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
	return ME_SUCCESS;
}

ME_U32 decoder_fornothing_pause(void *decodeinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_fornothing_pause()"));
	#endif
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
	return ME_SUCCESS;
}

ME_U32 decoder_fornothing_dst_buffersize(void *decodeinfor, ME_U32 maxsrcsize, ME_U32 *maxdstsize)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_fornothing_dst_buffersize()"));
	#endif
	*maxdstsize = 64*1024;
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
	return ME_SUCCESS;
}

ME_U32	decoder_fornothing_get_frameduration(void *decodeinfor, ME_U32 * duration)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_fornothing_get_frameduration()"));
	#endif
	*duration = 0;
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
	return ME_SUCCESS;
}

ME_U32 decoder_fornothing_decode(void *decodeinfor, char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen, ME_U32*decodelen)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_fornothing_decode()"));
	#endif
	*dstlen = *decodelen = 0;
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((src!=NULL));
	ME_ASSERT_PARAM_RETURN((dst!=NULL));
	return ME_SUCCESS;
}

ME_U32 decoder_fornothing_decode_frames(void *decodeinfor, void *src, void *dst)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->decoder_fornothing_decode()"));
	#endif
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((src!=NULL));
	ME_ASSERT_PARAM_RETURN((dst!=NULL));
	return ME_SUCCESS;
}


ME_U32 decoder_register_initialize(me_decoder_t* decoder)
{
	decoder->id[0]=decoder->id[1]=decoder->id[2]=decoder->id[3]=0;
	decoder->init				= decoder_fornothing_init;
	decoder->release				= decoder_fornothing_release;
	decoder->dst_buffersize		= decoder_fornothing_dst_buffersize;
	decoder->seek				= decoder_fornothing_seek;
	decoder->start				= decoder_fornothing_start;
	decoder->stop				= decoder_fornothing_stop;
	decoder->pause				= decoder_fornothing_pause;
	decoder->resume			= decoder_fornothing_resume;
	decoder->set_outputparam		= decoder_fornothing_setoutparam;
	decoder->get_outputparam		= decoder_fornothing_getoutparam;
	decoder->get_frameduration	= decoder_fornothing_get_frameduration;
	decoder->decode_oneframe	= decoder_fornothing_decode;
	decoder->decode_frames		= decoder_fornothing_decode_frames;
	return ME_SUCCESS;
}

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						no use function define for encoder. Amanda Deng
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
ME_U32 encoder_fornothing_init(void *recinfor, void* inputinfor, void **encodeinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->encoder_fornothing_init()"));
	#endif
	(*encodeinfor) = NULL;
	ME_ASSERT_PARAM_RETURN((recinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((inputinfor!=NULL));
	return ME_SUCCESS;
}

ME_U32 encoder_fornothing_release(void *encodeinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->encoder_fornothing_release()"));
	#endif
	ME_ASSERT_PARAM_RETURN((encodeinfor!=NULL));
	return ME_SUCCESS;
}

ME_U32 encoder_fornothing_set_inputparam(void *encodeinfor, void *inputinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->encoder_fornothing_set_inputparam()"));
	#endif
	ME_ASSERT_PARAM_RETURN((encodeinfor!=NULL));
	return ME_SUCCESS;
}

ME_U32 encoder_fornothing_start(void *encodeinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->encoder_fornothing_start()"));
	#endif
	ME_ASSERT_PARAM_RETURN((encodeinfor!=NULL));
	return ME_SUCCESS;
}

ME_U32 encoder_fornothing_stop(void *encodeinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->encoder_fornothing_stop()"));
	#endif
	ME_ASSERT_PARAM_RETURN((encodeinfor!=NULL));
	return ME_SUCCESS;
}

ME_U32 encoder_fornothing_resume(void *encodeinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->encoder_fornothing_resume()"));
	#endif
	ME_ASSERT_PARAM_RETURN((encodeinfor!=NULL));
	return ME_SUCCESS;
}

ME_U32 encoder_fornothing_pause(void *encodeinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->encoder_fornothing_pause()"));
	#endif
	ME_ASSERT_PARAM_RETURN((encodeinfor!=NULL));
	return ME_SUCCESS;
}

ME_U32 encoder_fornothing_src_buffersize(void *encodeinfor, ME_U32 *maxsrcsize)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->encoder_fornothing_src_buffersize()"));
	#endif
	*maxsrcsize = 64*1024;
	ME_ASSERT_PARAM_RETURN((encodeinfor!=NULL));
	return ME_SUCCESS;
}

ME_U32 encoder_fornothing_dst_buffersize(void *encodeinfor, ME_U32 *maxdstsize)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->encoder_fornothing_dst_buffersize()"));
	#endif
	*maxdstsize = 32*1024;
	ME_ASSERT_PARAM_RETURN((encodeinfor!=NULL));
	return ME_SUCCESS;
}

ME_U32 encoder_fornothing_encode_frames(void *encodeinfor, void *src, void *dst)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->encoder_fornothing_encode_frames()"));
	#endif
	ME_ASSERT_PARAM_RETURN((encodeinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((src!=NULL));
	ME_ASSERT_PARAM_RETURN((dst!=NULL));
	return ME_SUCCESS;
}

ME_U32 encoder_register_initialize(me_encoder_t* encoder)
{
	encoder->id[0]=encoder->id[1]=encoder->id[2]=encoder->id[3]=0;
	encoder->init				= encoder_fornothing_init;
	encoder->release				= encoder_fornothing_release;
	encoder->set_inputparam		= encoder_fornothing_set_inputparam;
	encoder->start				= encoder_fornothing_start;
	encoder->stop				= encoder_fornothing_stop;
	encoder->pause				= encoder_fornothing_pause;
	encoder->resume			= encoder_fornothing_resume;
	encoder->src_buffersize		= encoder_fornothing_src_buffersize;
	encoder->dst_buffersize		= encoder_fornothing_dst_buffersize;
	encoder->encode_frames		= encoder_fornothing_encode_frames;
	return ME_SUCCESS;
}

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						no use function define for parser. Amanda Deng
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
ME_U32	parser_fornothing_init(void *fs_filehandle, fs_callbacks *fs_filefunc,void **fileinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_fornothing_init()"));
	#endif	
	(*fileinfor)=NULL;
	ME_ASSERT_PARAM_RETURN((fs_filehandle!=NULL));
	ME_ASSERT_PARAM_RETURN((fs_filefunc!=NULL));
	return ME_SUCCESS;
}

ME_U32	parser_fornothing_release(void *fileinfor, me_audio_infor *audio_infor, me_video_infor *video_infor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_fornothing_release()"));
	#endif	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((audio_infor!=NULL));
	ME_ASSERT_PARAM_RETURN((video_infor!=NULL));
	return ME_SUCCESS;
}

ME_U32	parser_fornothing_check_filetype(void *fs_filehandle, fs_callbacks *fs_filefunc)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_fornothing_check_filetype()"));
	#endif	
	ME_ASSERT_PARAM_RETURN((fs_filehandle!=NULL));
	ME_ASSERT_PARAM_RETURN((fs_filefunc!=NULL));
	return ME_FALSE;
}

ME_U32	parser_fornothing_get_audio_infor(void *fileinfor, me_audio_infor *audio_infor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_fornothing_get_audio_infor()"));
	#endif
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((audio_infor!=NULL));
	audio_infor->a_isvalid 	= 0;
	return ME_SUCCESS;
}

ME_U32	parser_fornothing_get_audio_infor_formidi(void *fileinfor, me_audio_infor *audio_infor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_fornothing_get_audio_infor()"));
	#endif
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((audio_infor!=NULL));
	audio_infor->a_isvalid 	= 0;
	return ME_SUCCESS;
}

ME_U32	parser_fornothing_get_audio_streambuffersize(void *fileinfor, ME_U32 *framesize, void* audio_info)
	{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_fornothing_get_audio_streambuffersize()"));
	#endif
	ME_ASSERT_PARAM_RETURN((framesize!=NULL));
	*framesize = 1024*64;
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	return ME_SUCCESS;
}

ME_U32	parser_fornothing_get_audio_duration(void *fileinfor, ME_U32 frame, ME_U32 *duration)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_fornothing_get_audio_duration()"));
	#endif
	*duration = 20;
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((duration!=NULL));
    	return ME_SUCCESS;
}

ME_U32	parser_fornothing_read_audio(void *fileinfor, char *audio_buffer, ME_U32 *readbytes,ME_U32 bufferlength)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_fornothing_read_audio()"));
	#endif
	ME_ASSERT_PARAM_RETURN((readbytes!=NULL));
	*readbytes = 0;
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((audio_buffer!=NULL));
	return ME_SUCCESS;
}


ME_U32	parser_fornothing_read_audio_frames(void *fileinfor, audio_stream_infor *audio_buffer)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_fornothing_read_audio_frames()"));
	#endif
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((audio_buffer!=NULL));
	return ME_SUCCESS;
}
#if (ME_VERSION2_FOURHANDLE)
ME_U32	parser_fornothing_read_audio_frames_reg(void *fileinfor, audio_stream_infor *audio_buffer, ME_U32 reg, ME_U32 bit)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_fornothing_read_audio_frames()"));
	#endif
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((audio_buffer!=NULL));
	return ME_SUCCESS;
}
#endif


ME_U32	parser_fornothing_get_video_infor(void *fileinfor, me_video_infor *video_infor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_fornothing_get_video_infor()"));
	#endif
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((video_infor!=NULL));
	video_infor->v_isvalid = 0;
	return ME_SUCCESS;
}

ME_U32	parser_fornothing_get_video_streambuffersize(void *fileinfor, ME_U32 *framesize)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_fornothing_get_video_streambuffersize()"));
	#endif
	ME_ASSERT_PARAM_RETURN((framesize!=NULL));
	*framesize = 100*1024;
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	return ME_SUCCESS;
}

ME_U32	parser_fornothing_get_video_duration(void *fileinfor, ME_U32 frame, ME_U32 *duration)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_3gp_get_video_duration()"));
	#endif	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((duration!=NULL));
	return ME_SUCCESS;
}

ME_U32	parser_fornothing_read_video(void *fileinfor, char *video_buffer, ME_U32 *readbytes,ME_U32 bufferlength)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_fornothing_read_video()"));
	#endif
	ME_ASSERT_PARAM_RETURN((readbytes!=NULL));
	*readbytes = 0;
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((video_buffer!=NULL));
	return ME_SUCCESS;
}

ME_U32	parser_fornothing_read_video_frames(void *fileinfor, video_stream_infor *video_buffer)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_3gp_read_video()"));
	#endif
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((video_buffer!=NULL));
	return ME_SUCCESS;
}
#if (ME_VERSION2_FOURHANDLE)
ME_U32	parser_fornothing_read_video_frames_reg(void *fileinfor, video_stream_infor *video_buffer, ME_U32 reg, ME_U32 bit)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_3gp_read_video()"));
	#endif
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((video_buffer!=NULL));
	return ME_SUCCESS;
}
#endif

ME_U32	parser_fornothing_read_key_video(void *fileinfor, char *video_buffer, ME_U32 *readbytes,ME_U32 bufferlength)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_fornothing_read_key_video()"));
	#endif
	ME_ASSERT_PARAM_RETURN((readbytes!=NULL));
	*readbytes = 0;
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((video_buffer!=NULL));
	return ME_SUCCESS;
}

ME_U32	parser_fornothing_read_key_videoframes(void *fileinfor, video_stream_infor *video_buffer)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_3gp_read_key_video()"));
	#endif
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((video_buffer!=NULL));
	return ME_SUCCESS;
}


ME_U32	parser_fornothing_seek(void *fileinfor, ME_U32 pos, ME_U32* accesspos, ME_U32 *videoframe)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_fornothing_seek()"));
	#endif
	* accesspos = pos;
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((videoframe!=NULL));
	return ME_SUCCESS;
}

ME_U32	parser_fornothing_seek_audio_syntime(void *fileinfor, ME_U32 pos)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_fornothing_seek_audio_syntime()"));
	#endif
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	return ME_SUCCESS;
}

ME_U32 parser_register_initialize(me_parser_t* parser)
{
	parser->id[0]=parser->id[1]=parser->id[2]=parser->id[3]=0;
	parser->init					  		= parser_fornothing_init;
	parser->release				  		= parser_fornothing_release;
	parser->check_file_type			  	= parser_fornothing_check_filetype;
	parser->seek				        	= parser_fornothing_seek;
	
	parser->get_audio_info			  	= parser_fornothing_get_audio_infor;
	parser->get_audio_info_formidi			= parser_fornothing_get_audio_infor_formidi;
	parser->get_audio_frame_buffersize		= parser_fornothing_get_audio_streambuffersize;
	parser->get_audio_frame_duration		= parser_fornothing_get_audio_duration;
	parser->read_audio_oneframe     		= parser_fornothing_read_audio;
	parser->read_audio_frames     			= parser_fornothing_read_audio_frames;

	parser->get_video_info		         	= parser_fornothing_get_video_infor;
   	parser->get_video_frame_buffersize		= parser_fornothing_get_video_streambuffersize;
   	parser->get_video_frame_duration		= parser_fornothing_get_video_duration;
	parser->read_video_oneframe     		= parser_fornothing_read_video;
	parser->read_video_keyframe     		= parser_fornothing_read_key_video;
	parser->read_video_frames     			= parser_fornothing_read_video_frames;
	parser->read_video_keyframes     		= parser_fornothing_read_key_videoframes;
	parser->seek_audio_syntime     		= parser_fornothing_seek_audio_syntime;
	return ME_SUCCESS;
}
/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						no use function define for muxer. Amanda Deng
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
ME_U32	muxer_fornothing_init(void *fs_filehandle, fs_callbacks *fs_filefunc,void **fileinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_fornothing_init()"));
	#endif	
	ME_ASSERT_PARAM_RETURN((fs_filehandle!=NULL));
	ME_ASSERT_PARAM_RETURN((fs_filefunc!=NULL));
	(*fileinfor)=NULL;
	return ME_SUCCESS;

}

ME_U32 muxer_fornothing_release(void *fileinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_fornothing_release()"));
	#endif	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	return ME_SUCCESS;
}


ME_U32	muxer_fornothing_set_audio_infor(void *fileinfor, me_audio_infor *audio_infor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_fornothing_set_audio_infor()"));
	#endif
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((audio_infor!=NULL));
	return ME_SUCCESS;
}

ME_U32	muxer_fornothing_write_audio(void *fileinfor,char *audio_buffer, ME_U32 bytes, ME_U32 duration)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_fornothing_write_audio()"));
	#endif
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((audio_buffer!=NULL));
	return ME_SUCCESS;
}

ME_U32	muxer_fornothing_write_audio_frames(void *fileinfor, audio_stream_infor *audio_buffer)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_fornothing_write_audio()"));
	#endif
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((audio_buffer!=NULL));
	return ME_SUCCESS;
}

ME_U32	muxer_fornothing_set_video_infor(void *fileinfor, me_video_infor *video_infor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_fornothing_set_video_infor()"));
	#endif
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((video_infor!=NULL));
	return ME_SUCCESS;
}

ME_U32	muxer_fornothing_write_video(void *fileinfor, char *video_buffer, ME_U32 bytes, ME_U32 duration,ME_U32 is_key_frame)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_3gp_write_video()"));
	#endif
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((video_buffer!=NULL));
	return ME_SUCCESS;
}

ME_U32	muxer_fornothing_write_video_frames(void *fileinfor, video_stream_infor *video_buffer)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_3gp_write_video()"));
	#endif
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((video_buffer!=NULL));
	return ME_SUCCESS;
}

ME_U32 muxer_register_initialize(me_muxer_t* muxer)
{
	muxer->id[0]=muxer->id[1]=muxer->id[2]=muxer->id[3]=0;
	muxer->init					= muxer_fornothing_init;
	muxer->release				= muxer_fornothing_release;
	muxer->set_audio_info		= muxer_fornothing_set_audio_infor;
	muxer->write_audio_oneframe	= muxer_fornothing_write_audio;
	muxer->write_audio_frames	= muxer_fornothing_write_audio_frames;
	muxer->set_video_info		= muxer_fornothing_set_video_infor;
 	muxer->write_video_oneframe	= muxer_fornothing_write_video;
	muxer->write_video_frames	= muxer_fornothing_write_video_frames;
	return ME_SUCCESS;
}

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						no use function define for effect process. Amanda Deng
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
ME_U32 effectprocess_fornothing_init(me_effectproc_t *pthis, void *param1)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_fornothing_init()"));
	#endif	
	ME_ASSERT_PARAM_RETURN((pthis!=NULL));
	pthis->enable = 0;
	pthis->infor = NULL;
	return ME_SUCCESS;
}

ME_U32 effectprocess_fornothing_release(me_effectproc_t *pthis)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_fornothing_init()"));
	#endif	
	ME_ASSERT_PARAM_RETURN((pthis!=NULL));
	return ME_SUCCESS;
}

ME_U32 effectprocess_fornothing_reset(me_effectproc_t *pthis)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_fornothing_init()"));
	#endif	
	ME_ASSERT_PARAM_RETURN((pthis!=NULL));
	return ME_SUCCESS;
}

ME_U32 effectprocess_fornothing_set_enable(me_effectproc_t *pthis)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->muxer_fornothing_init()"));
	#endif	
	ME_ASSERT_PARAM_RETURN((pthis!=NULL));
	pthis->enable = 1;
	return ME_SUCCESS;
}

ME_U32 effectprocess_fornothing_set_disable(me_effectproc_t *pthis)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->effectprocess_fornothing_set_disable()"));
	#endif	
	ME_ASSERT_PARAM_RETURN((pthis!=NULL));
	pthis->enable = 0;
	return ME_SUCCESS;
}

ME_U32 effectprocess_fornothing_set_param(me_effectproc_t *pthis, void *param1)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->effectprocess_fornothing_set_param()"));
	#endif	
	ME_ASSERT_PARAM_RETURN((pthis!=NULL));
	return ME_SUCCESS;
}

ME_U32 effectprocess_fornothing_data_process(me_effectproc_t *pthis, void *param1, void *param2, void *param3)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->effectprocess_fornothing_data_process()"));
	#endif	
	ME_ASSERT_PARAM_RETURN((pthis!=NULL));
	return ME_SUCCESS;
}

ME_U32 effectprocess_register_initialize(me_effectproc_t *effect)
{
	effect->id[0]=effect->id[1]=effect->id[2]=effect->id[3]=0;
	effect->infor				= NULL;
	effect->enable			= 0;
	effect->init				= effectprocess_fornothing_init;
	effect->release			= effectprocess_fornothing_release;
	effect->reset				= effectprocess_fornothing_release;
	effect->set_enable		=effectprocess_fornothing_set_enable;
	effect->set_disable		= effectprocess_fornothing_set_disable;
	effect->set_param		= effectprocess_fornothing_set_param;
	effect->data_process		= effectprocess_fornothing_data_process;
	return ME_SUCCESS;
}


/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						 function define for pcm convter. Amanda Deng
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
ME_U32 nomo8bit_to_nomo16bit(char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen)
{
	ME_U32  	src_address, dst_address;
	ME_PU08	conver_src8; 
	ME_PS16	conver_dst16;
	ME_U32  	i;
	
	ME_ASSERT_PARAM_RETURN((src!=NULL));
	ME_ASSERT_PARAM_RETURN((dst!=NULL));
	ME_ASSERT_PARAM_RETURN((srclen!=0));
	
	src_address	= (ME_U32)src;
	dst_address	= (ME_U32)dst;
	//out 16 bit nomo n samples(n*1*2 bytes) need 8 bit nomo n samples(n*1*1 bytes)
	conver_src8 	= (ME_PU08)(src_address+srclen);
	conver_dst16 	= (ME_PS16)(dst_address+srclen*2);
	conver_src8 --;	// last nomo sample address
	conver_dst16 --;	// last nomo sample address
	//conver 8 bit nomo to 16 bit nomo 
	for(i=0;i<srclen;i++)
	{
		//nomo sample = nomo 8 bit sample to 16 bit sample
		*conver_dst16-- =((*conver_src8--)<<8)^0x8000;
	}
	*dstlen = srclen*2;
	return ME_SUCCESS;
}

ME_U32 nomo16bit_to_nomo16bit(char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen)
{
	ME_ASSERT_PARAM_RETURN((src!=NULL));
	ME_ASSERT_PARAM_RETURN((dst!=NULL));
	ME_ASSERT_PARAM_RETURN((srclen!=0));

	*dstlen = srclen;
	if(src==dst)
		return ME_SUCCESS;
	//out 16 bit nomo n samples(n*1*2 bytes) need 16 bit nomo n samples(n*1*2 bytes)
	me_memcpy((ME_PS08)src, (ME_PCS08)dst, srclen);
	return ME_SUCCESS;
}

ME_U32 nomo24bit_to_nomo16bit(char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen)
{
	ME_PU08	conver_src8, conver_dst8; 
	ME_U32  	i;
	
	ME_ASSERT_PARAM_RETURN((src!=NULL));
	ME_ASSERT_PARAM_RETURN((dst!=NULL));
	ME_ASSERT_PARAM_RETURN((srclen!=0));
	
	//out 16 bit nomo n samples(n*1*2 bytes) need 24 bit nomo n samples(n*1*3 bytes)
	conver_src8 = (ME_PU08)src;
	conver_dst8 = (ME_PU08)dst;
	//conver 24 bit nomo to 16 bit nomo 
	for(i=0;i<srclen/3;i++)
	{
		// lose low 8 bit
		conver_src8++; 
		//nomo sample = nomo hight 16bit, lose low 8 bit
		*conver_dst8++ = *conver_src8++;
		*conver_dst8++ = *conver_src8++;
	}
	*dstlen = (srclen/3)*2;
	return ME_SUCCESS;
}

ME_U32 nomo32bit_to_nomo16bit(char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen)
{
	ME_PS16	conver_src16, conver_dst16;
	ME_U32  	i;
	
	ME_ASSERT_PARAM_RETURN((src!=NULL));
	ME_ASSERT_PARAM_RETURN((dst!=NULL));
	ME_ASSERT_PARAM_RETURN((srclen!=0));

	//out 16 bit nomo n samples(n*1*2 bytes) need 32 bit nomo n samples(n*1*4 bytes)
	conver_src16 = (ME_PS16)src;
	conver_dst16 = (ME_PS16)dst;
	//conver 32 bit nomo to 16 bit nomo 
	for(i=0;i<srclen/4;i++)
	{
		// lose low 16 bit
		conver_src16++;
		//nomo sample = nomo hight 16bit, lose low 16 bit
		*conver_dst16++ = *conver_src16++;
		
	}
	*dstlen = (srclen/4)*2;
	return ME_SUCCESS;
}

ME_U32 stereo8bit_to_nomo16bit(char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen)
{
	ME_PU08	conver_src8; 
	ME_PS16	conver_dst16;
	ME_S16	sample_l,sample_r;
	ME_U32  i;
	
	ME_ASSERT_PARAM_RETURN((src!=NULL));
	ME_ASSERT_PARAM_RETURN((dst!=NULL));
	ME_ASSERT_PARAM_RETURN((srclen!=0));
	
	//out 16 bit nomo n samples(n*1*2 bytes) need 8 bit stereo n samples(n*2*1 bytes)
	conver_src8 = (ME_PU08)src;
	conver_dst16 = (ME_PS16)dst;
	//conver 8 bit stereo to 16 bit nomo 
	for(i=0;i<srclen/2;i++)
	{
		//nomo sample = (right channel/2 + left channel/2)
		sample_l = (ME_S16)(((*conver_src8++)<<8)^0x8000);
		sample_r = (ME_S16)(((*conver_src8++)<<8)^0x8000);
		*conver_dst16++ = sample_l/2+sample_r/2;
	}
	*dstlen=srclen;
	return ME_SUCCESS;
}

ME_U32 stereo16bit_to_nomo16bit(char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen)
{
	ME_PS16	conver_src16, conver_dst16;
	ME_S16	sample_l,sample_r;
	ME_U32  i;
	
	ME_ASSERT_PARAM_RETURN((src!=NULL));
	ME_ASSERT_PARAM_RETURN((dst!=NULL));
	ME_ASSERT_PARAM_RETURN((srclen!=0));
	
	//out 16 bit nomo n samples(n*1*2 bytes) need 16 bit stereo n samples(n*2*2 bytes)
	conver_src16 = (ME_PS16)src;
	conver_dst16 = (ME_PS16)dst;
	//conver 16 bit stereo to 16 bit nomo
	for(i=0;i<srclen/4;i++)
	{
		//nomo sample = (right channel/2 + left channel/2)
		sample_l = *conver_src16++;
		sample_r = *conver_src16++;
		*conver_dst16++ = sample_l/2+sample_r/2;
	}
	*dstlen = (srclen/4)*2;
	return ME_SUCCESS;
}

ME_U32 stereo24bit_to_nomo16bit(char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen)
{
	ME_PU08	conver_src8, conver_dst8; 
	ME_U32  	i;
	
	ME_ASSERT_PARAM_RETURN((src!=NULL));
	ME_ASSERT_PARAM_RETURN((dst!=NULL));
	ME_ASSERT_PARAM_RETURN((srclen!=0));
	
	//out 16 bit nomo n samples(n*1*2 bytes) need 24 bit stereo n samples(n*2*3 bytes)
	conver_src8 = (ME_PU08)src;
	conver_dst8 = (ME_PU08)dst;
	//conver 24 bit stereo to 16 bit nomo
	for(i=0;i<srclen/6;i++)
	{
		// lose low 8 bit
		conver_src8++; 
		//nomo sample = left channel hight 16bit, lost right channel
		*conver_dst8++ = *conver_src8++;
		*conver_dst8++ = *conver_src8++;
		//lose right channel
		conver_src8++;
		conver_src8++;
		conver_src8++; 
	}
	*dstlen = (srclen/6)*2;
	return ME_SUCCESS;
}

ME_U32 stereo32bit_to_nomo16bit(char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen)
{
	ME_PS16	conver_src16, conver_dst16;
	ME_U32  	i;
	
	ME_ASSERT_PARAM_RETURN((src!=NULL));
	ME_ASSERT_PARAM_RETURN((dst!=NULL));
	ME_ASSERT_PARAM_RETURN((srclen!=0));
	
	//out 16 bit nomo n samples(n*1*2 bytes) need 32 bit stereo n samples(n*2*4 bytes)
	conver_src16 = (ME_PS16)src;
	conver_dst16 = (ME_PS16)dst;
	//conver 32 bit nomo to 16 bit stereo 
	for(i=0;i<srclen/8;i++)
	{
		// lose right low 16 bit
		conver_src16++;
		//nomo sample = left channel hight 16bit, lost right channel
		*conver_dst16++ = *conver_src16++;
		// lose right channel
		conver_src16++;
		conver_src16++;
	}
	*dstlen = (srclen/8)*2;
	return ME_SUCCESS;
}

ME_U32 nomo8bit_to_stereo16bit(char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen)
{
	ME_U32  	src_address, dst_address;
	ME_PU08	conver_src8; 
	ME_PS16	conver_dst16;
	ME_U32  	i;
	
	ME_ASSERT_PARAM_RETURN((src!=NULL));
	ME_ASSERT_PARAM_RETURN((dst!=NULL));
	ME_ASSERT_PARAM_RETURN((srclen!=0));

	src_address	= (ME_U32)src;
	dst_address	= (ME_U32)dst;
	//out 16 bit stereo n samples(n*2*2 bytes) need 8 bit nomo n samples(n*1*1 bytes)
	conver_src8 	= (ME_PU08)(src_address+srclen);
	conver_dst16 	= (ME_PS16)(dst_address+srclen*2*2);
	conver_src8 --;	// last nomo bytes address
	conver_dst16 --;	// last stereo sample address
	//conver 8 bit nomo to 16 bit stereo 
	for(i=0;i<srclen;i++)
	{
		*conver_dst16-- = ((*conver_src8)<<8)^0x8000;
		*conver_dst16-- = ((*conver_src8--)<<8)^0x8000;
	}
	*dstlen = srclen*2*2;
	return ME_SUCCESS;
}

ME_U32 nomo16bit_to_stereo16bit(char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen)
{
	ME_U32  	src_address, dst_address;
	ME_PS16	conver_src16, conver_dst16;
	ME_U32  	i;
	
	ME_ASSERT_PARAM_RETURN((src!=NULL));
	ME_ASSERT_PARAM_RETURN((dst!=NULL));
	ME_ASSERT_PARAM_RETURN((srclen!=0));
	
	src_address	= (ME_U32)src;
	dst_address	= (ME_U32)dst;
	//out 16 bit stereo n samples(n*2*2 bytes) need 16 bit nomo n samples(n*1*2 bytes)
	conver_src16 = (ME_PS16)(src_address+srclen);
	conver_dst16 = (ME_PS16)(dst_address+srclen*2);
	conver_src16 --;	// last nomo bytes address
	conver_dst16 --;	// last stereo bytes address
	//conver 16 bit nomo to 16 bit stereo 
	for(i=0;i<srclen/2;i++)
	{
		//nomo sample to right channel
		*conver_dst16-- = *conver_src16;
		//nomo sample to left channel, and to next nomo sample
		*conver_dst16-- = *conver_src16--;
	}
	*dstlen = (srclen/2)*2*2;
	return ME_SUCCESS;
}

ME_U32 nomo24bit_to_stereo16bit(char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen)
{
	ME_U32  	src_address, dst_address;
	ME_PU08	conver_src8, conver_dst8; 
	ME_U32  	i;
	
	ME_ASSERT_PARAM_RETURN((src!=NULL));
	ME_ASSERT_PARAM_RETURN((dst!=NULL));
	ME_ASSERT_PARAM_RETURN((srclen!=0));
	
	src_address	= (ME_U32)src;
	dst_address	= (ME_U32)dst;
	//out 16 bit stereo n samples(n*2*2 bytes) need 16 bit nomo n samples(n*1*3 bytes)
	conver_src8 = (ME_PU08)(src_address+srclen);
	conver_dst8 = (ME_PU08)(dst_address+(srclen/3)*2*2);
	conver_src8 --;	// last nomo bytes address
	conver_dst8 --;	// last stereo bytes address
	//conver 24 bit nomo to 16 bit stereo 
	for(i=0;i<srclen/3;i++)
	{
		//copy src hight 16 bit for dst, lose low 8 bit
		*(conver_dst8-2) = *conver_src8;
		*conver_dst8-- = *conver_src8--;
		*(conver_dst8-2) = *conver_src8;
		*conver_dst8-- = *conver_src8--;
		conver_dst8 = conver_dst8-2;
		conver_src8--; // lose low 8 bit
	}
	*dstlen = (srclen/3)*2*2;
	return ME_SUCCESS;
}

ME_U32 nomo32bit_to_stereo16bit(char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen)
{
	ME_PS16	conver_src16, conver_dst16;
	ME_U32  	i;
	
	ME_ASSERT_PARAM_RETURN((src!=NULL));
	ME_ASSERT_PARAM_RETURN((dst!=NULL));
	ME_ASSERT_PARAM_RETURN((srclen!=0));
	
	//out 16 bit stereo n samples(n*2*2 bytes) need 16 bit nomo n samples(n*1*4 bytes)
	conver_src16 = (ME_PS16)src;
	conver_dst16 = (ME_PS16)dst;
	//conver 32 bit nomo to 16 bit stereo 
	for(i=0;i<srclen/4;i++)
	{
		// lose low 16 bit
		conver_src16++;
		//copy nomo src hight 16 bit for left channel
		*conver_dst16++ = *conver_src16;
		//copy nomo src hight 16 bit for right channel
		*conver_dst16++ = *conver_src16++;
	}
	*dstlen = (srclen/4)*2*2;
	return ME_SUCCESS;
}

ME_U32 stereo8bit_to_stereo16bit(char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen)
{
	ME_U32  	src_address, dst_address;
	ME_PU08	conver_src8; 
	ME_PS16	conver_dst16;
	ME_U32  	i;
	
	ME_ASSERT_PARAM_RETURN((src!=NULL));
	ME_ASSERT_PARAM_RETURN((dst!=NULL));
	ME_ASSERT_PARAM_RETURN((srclen!=0));
	
	src_address	= (ME_U32)src;
	dst_address	= (ME_U32)dst;
	//out 16 bit stereo n samples(n*2*2 bytes) need 8 bit stereo n samples(n*2*1 bytes)
	conver_src8 = (ME_PU08)(src_address+srclen);
	conver_dst16 = (ME_PS16)(dst_address+srclen*2);
	conver_src8 --;	// last nomo bytes address
	conver_dst16 --;	// last stereo sample address
	//conver 8 bit stereo to 16 bit stereo 
	for(i=0;i<srclen/2;i++)
	{
		//right channel 8 bit to 16 bit
		*conver_dst16-- = ((*conver_src8--)<<8)^0x8000;
		//left channel 8 bit to 16 bit
		*conver_dst16-- = ((*conver_src8--)<<8)^0x8000;
	}
	*dstlen = (srclen/2)*2*2;
	return ME_SUCCESS;
}

ME_U32 stereo16bit_to_stereo16bit(char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen)
{
	ME_ASSERT_PARAM_RETURN((src!=NULL));
	ME_ASSERT_PARAM_RETURN((dst!=NULL));
	ME_ASSERT_PARAM_RETURN((srclen!=0));

	*dstlen = srclen;
	
	if(src==dst)
		return ME_SUCCESS;	
	//out 16 bit stereo n samples(n*2*2 bytes) need 16 bit stereo n samples(n*2*2 bytes)
	me_memcpy((ME_PS08)dst, (ME_PCS08)src, srclen);
	return ME_SUCCESS;
}

ME_U32 stereo24bit_to_stereo16bit(char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen)
{
	ME_PU08	conver_src8, conver_dst8; 
	ME_U32  	i;
	
	ME_ASSERT_PARAM_RETURN((src!=NULL));
	ME_ASSERT_PARAM_RETURN((dst!=NULL));
	ME_ASSERT_PARAM_RETURN((srclen!=0));
	
	//out 16 bit stereo n samples(n*2*2 bytes) need 24 bit stereo n samples(n*2*3 bytes)
	conver_src8 = (ME_PU08)src;
	conver_dst8 = (ME_PU08)dst;
	//conver 24 bit stereo to 16 bit stereo 
	for(i=0;i<srclen/6;i++)
	{
		 // lose left low 8 bit
		conver_src8++;
		// left channel copy hight 16 bit
		*conver_dst8++ = *conver_src8++;
		*conver_dst8++ = *conver_src8++;
		// lose right low 8 bit
		conver_src8++; 
		// right channel copy hight 16 bit
		*conver_dst8++ = *conver_src8++;
		*conver_dst8++ = *conver_src8++;
	}
	*dstlen = (srclen/6)*2*2;
	return ME_SUCCESS;
}

ME_U32 stereo32bit_to_stereo16bit(char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen)
{
	ME_PS16	conver_src16, conver_dst16;
	ME_U32  	i;
	
	ME_ASSERT_PARAM_RETURN((src!=NULL));
	ME_ASSERT_PARAM_RETURN((dst!=NULL));
	ME_ASSERT_PARAM_RETURN((srclen!=0));
	
	//out 16 bit stereo n samples(n*2*2 bytes) need 16 bit stereo n samples(n*2*4 bytes)
	conver_src16 = (ME_PS16)src;
	conver_dst16 = (ME_PS16)dst;
	//conver 32 bit stereo to 16 bit stereo 
	for(i=0;i<srclen/8;i++)
	{
		//lose left channel low 16 bit
		conver_src16++; 
		//left channel copy hight 16 bit
		*conver_dst16++ = *conver_src16++;
		//lose right channel low 16 bit
		conver_src16++; 
		//right channel copy hight 16 bit
		*conver_dst16++ = *conver_src16++;
	}
	*dstlen = (srclen/8)*2*2;
	return ME_SUCCESS;
}


