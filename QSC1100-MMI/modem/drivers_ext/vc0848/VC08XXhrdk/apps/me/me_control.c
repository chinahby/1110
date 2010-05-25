/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	me_control.c 
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/12/31
* Last modify : Amanda Deng 2007-xx-xx     
*
* version 0.1: 
*	have 10 interface API for user .
*	ME_U32 	media_engine_creat(void);
*	ME_U32 	media_engine_destroy(void);
*	ME_U32 	me_parser_register(me_parser_t *parser);
*	ME_U32 	me_decoder_register(me_decoder_t *decoder);
*	ME_U32 	me_encoder_register(me_encoder_t *encoder);

*
*	ME_U32 	me_player_open(void* fs_filehandle, 
*				       fs_callbacks* fs_filefunc, 
*						    char* fs_filetype,
*					    MP_FILE** mpfile);
*	ME_U32 	me_player_close(MP_FILE *mpfile);
*	ME_U32	me_audio_postproc_register(MP_FILE *mpfile, me_effectproc_t *postproc);
*	ME_U32	me_video_postproc_register(MP_FILE *mpfile, me_effectproc_t *postproc);
*	
*
*	ME_U32 	me_recorder_open(void* fs_filehandle,
*								 fs_callbacks *fs_filefunc, 
*							  	 me_record_infor* rec_infor,
*							 	 MR_FILE **recfile);
*	ME_U32  me_recorder_close(MR_FILE *recfile);
*	ME_U32	me_audio_preproc_register(MR_FILE *mrfile, me_effectproc_t *preproc);
*	ME_U32	me_video_preproc_register(MR_FILE *mrfile, me_effectproc_t *preproc);*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/

#include "me_control.h"
#include "me_interface.h"
#include	"..\drivers_ext\vc0848\VC08XXhrdk\apps\media\media_player.h"

#include "Davincicode.h"
#include "err.h"
/*******************************************************************************************/
vm_media_engine 	_g_media_engine;						// media engine structor(parser,muxer,decoder,encoder list)
extern VIM_MEDIA_SYSTEM_INFOR* 		media_sysstatus;
#if ME_KM_OBJ_STACK
static me_node_list			_g_parser_list;		// parser list(for user register their parser function)
static me_node_list			_g_muxer_list;		// muxer list(for user register their muxer  function)
static me_node_list			_g_decoder_list;		// decoder list(for user register their decoder function)
static me_node_list			_g_encoder_list;		// encoder list(for user register their encoder function)
static me_node_list			_g_id3_list;			// id3 list(for user register their id3 function)
#endif

#define	MAX_VIDEO_SIZE    0x6800

/*******************************************************************************************
Description:
	initialize media engine structor, include parser list, muxer list, decode list and encoder list
	all list be initialized to empty.
Parameters:
       No.
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.12.31		create first version
********************************************************************************************/
ME_U32 media_engine_creat(void)
{
	vm_media_engine *pthis = &_g_media_engine;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->media_engine_creat()"));
	#endif

#if ME_KM_OBJ_STACK
	pthis->parserlist	= &_g_parser_list;
	pthis->parserlist->counter	= 0;
	pthis->parserlist->cur		= NULL;
	pthis->parserlist->first		= NULL;
	pthis->parserlist->last		= NULL;
	
	pthis->muxerlist	= &_g_muxer_list;
	pthis->muxerlist->counter	= 0;
	pthis->muxerlist->cur		= NULL;
	pthis->muxerlist->first		= NULL;
	pthis->muxerlist->last		= NULL;
	
	pthis->decoderlist	= &_g_decoder_list;
	pthis->decoderlist->counter	= 0;
	pthis->decoderlist->cur		= NULL;
	pthis->decoderlist->first	= NULL;
	pthis->decoderlist->last	= NULL;
	
	pthis->encoderlist	= &_g_encoder_list;
	pthis->encoderlist->counter	= 0;
	pthis->encoderlist->cur		= NULL;
	pthis->encoderlist->first	= NULL;
	pthis->encoderlist->last	= NULL;
	
	pthis->id3list	= &_g_id3_list;
	pthis->id3list->counter	= 0;
	pthis->id3list->cur		= NULL;
	pthis->id3list->first	= NULL;
	pthis->id3list->last	= NULL;
#else
	pthis->parserlist	= me_list_new(ME_VOID);
	pthis->muxerlist	= me_list_new(ME_VOID);
	pthis->decoderlist	= me_list_new(ME_VOID);
	pthis->encoderlist	= me_list_new(ME_VOID);
	pthis->id3list		= me_list_new(ME_VOID);
#endif
	unknow_id3_register();
//	id3_mp3_register();
	return ME_SUCCESS;
}
/*******************************************************************************************
Description:
	free media engine's parser list, muxer list, decode list and encoder list, all list  to NULL.
Parameters:
       No.
Return:	
      ME_SUCCESS: no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.12.31		create first version
********************************************************************************************/
ME_U32 media_engine_destroy(void)
{
	#if ME_KM_FUNNAME
		ME_MESSAGE(("->media_engine_destroy()"));
	#endif
	me_list_free(_g_media_engine.parserlist);
	me_list_free(_g_media_engine.muxerlist);
	me_list_free(_g_media_engine.decoderlist);
	me_list_free(_g_media_engine.encoderlist);
	me_list_free(_g_media_engine.id3list);
	return ME_SUCCESS;
}
/*******************************************************************************************
Description:
	for user register their parser function
Parameters:
       me_parser_t *parser: user parser structor point;
Return:	
      ME_SUCCESS: no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.12.31		create first version
	   0.11			Amanda Deng			2007.03.02		add interface function address is NULL or not
********************************************************************************************/
ME_U32 me_parser_register(me_parser_t *parser)
{
	ME_S32 i, null_flag, true_flag;
	ME_U32 *function_address;
	#if ME_KM_FUNNAME
		ME_MESSAGE(("->me_parser_register()"));
	#endif

	if(parser==NULL)
		return ME_ERR_PARSER_IS_NULL;

	function_address = (ME_U32*)parser +1;
	//parser interface check
	for(i=0; i<ME_PEREMPTORY_PARSER_INTERFACE;i++)
	{
		if (*(function_address+i)==0)
		{
			ME_ERROR(("ME_ERR_PARSER_HAVE_NULL_FUNCTION -0-"));
			return ME_ERR_PARSER_HAVE_NULL_FUNCTION;
		}
	}
	//audio interface check
	function_address = function_address+ME_PEREMPTORY_PARSER_INTERFACE;
	if (*(function_address)==0)		//get audio infor is peremptory
	{
		ME_ERROR(("ME_ERR_PARSER_HAVE_NULL_FUNCTION -1-"));
		return ME_ERR_PARSER_HAVE_NULL_FUNCTION;
	}

	null_flag = true_flag =0;
	for(i=1; i<ME_PARSER_AUDIO_INTERFACE;i++)
	{
		if (*(function_address+i)!=0)
			true_flag=1;
		else
			null_flag=1;
	}
	if(true_flag&&null_flag)
	{
		ME_ERROR(("ME_ERR_PARSER_HAVE_NULL_FUNCTION -2-"));
		return ME_ERR_PARSER_HAVE_NULL_FUNCTION;
	}
	//video interface check
	function_address = function_address+ME_PARSER_AUDIO_INTERFACE;
	if (*(function_address)==0)		//get video infor is peremptory
	{
		ME_ERROR(("ME_ERR_PARSER_HAVE_NULL_FUNCTION -3-"));
		return ME_ERR_PARSER_HAVE_NULL_FUNCTION;
	}

	null_flag = true_flag =0;
	for(i=1; i<ME_PARSER_VIDEO_INTERFACE;i++)
	{
		if (*(function_address+i)!=0)
			true_flag=1;
		else
			null_flag=1;
	}
	if(true_flag&&null_flag)
	{
		ME_ERROR(("ME_ERR_PARSER_HAVE_NULL_FUNCTION -4-"));
		return ME_ERR_PARSER_HAVE_NULL_FUNCTION;
	}
	
	return me_list_append_content(_g_media_engine.parserlist, (void*)parser);
}
/*******************************************************************************************
Description:
	for user register their muxer function
Parameters:
       me_muxer_t *muxer: user muxer structor point;
Return:	
      ME_SUCCESS: no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.12.31		create first version
	   0.11			Amanda Deng			2007.03.02		add interface function address is NULL or not
********************************************************************************************/
ME_U32 me_muxer_register(me_muxer_t *muxer)
{
	ME_S32 i, null_flag, true_flag;
	ME_U32 *function_address;
	
	#if ME_KM_FUNNAME
		ME_MESSAGE(("->me_muxer_register()"));
	#endif

	if(muxer==NULL)
		return ME_ERR_MUXER_IS_NULL;

	function_address = (ME_U32*)muxer +1;
	//parser interface check
	for(i=0; i<ME_PEREMPTORY_MUXER_INTERFACE;i++)
	{
		if (*(function_address+i)==0)
		{
			ME_ERROR(("ME_ERR_MUXER_HAVE_NULL_FUNCTION -0-"));
			return ME_ERR_MUXER_HAVE_NULL_FUNCTION;
		}
	}
	//audio interface check
	function_address = function_address+ME_PEREMPTORY_MUXER_INTERFACE;
	null_flag = true_flag =0;
	for(i=0; i<ME_MUXER_AUDIO_INTERFACE;i++)
	{
		if (*(function_address+i)!=0)
			true_flag=1;
		else
			null_flag=1;
	}
	if(true_flag&&null_flag)
	{
		ME_ERROR(("ME_ERR_MUXER_HAVE_NULL_FUNCTION -1-"));
		return ME_ERR_MUXER_HAVE_NULL_FUNCTION;
	}
	//video interface check
	function_address = function_address+ME_MUXER_AUDIO_INTERFACE;
	null_flag = true_flag =0;
	for(i=0; i<ME_MUXER_VIDEO_INTERFACE;i++)
	{
		if (*(function_address+i)!=0)
			true_flag=1;
		else
			null_flag=1;
	}
	if(true_flag&&null_flag)
	{
		ME_ERROR(("ME_ERR_MUXER_HAVE_NULL_FUNCTION -2-"));
		return ME_ERR_MUXER_HAVE_NULL_FUNCTION;
	}
	
	return me_list_append_content(_g_media_engine.muxerlist, (void*)muxer);
}
/*******************************************************************************************
Description:
	for user register their decoder function
Parameters:
       me_decoder_t *decoder: user decoder structor point;
Return:	
      ME_SUCCESS: no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.12.31		create first version
	   0.11			Amanda Deng			2007.03.02		add interface function address is NULL or not
********************************************************************************************/
ME_U32 me_decoder_register(me_decoder_t *decoder)
{
	ME_S32 i;
	ME_U32 *function_address;
	
	#if ME_KM_FUNNAME
		ME_MESSAGE(("->me_decoder_register()"));
	#endif

	if(decoder==NULL)
		return ME_ERR_DECODER_IS_NULL;

	function_address = (ME_U32*)decoder +1;
	//decoder interface check
	for(i=0; i<ME_PEREMPTORY_DECODER_INTERFACE;i++)
	{
		if (*(function_address+i)==0)
		{
			ME_ERROR(("ME_ERR_DECODER_HAVE_NULL_FUNCTION -0-"));
			return ME_ERR_DECODER_HAVE_NULL_FUNCTION;
		}
	}
	
	return me_list_append_content(_g_media_engine.decoderlist, (void*)decoder);
}
/*******************************************************************************************
Description:
	for user register their encoder function
Parameters:
       me_encoder_t *encoder: user encoder structor point;
Return:	
      ME_SUCCESS: no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.12.31		create first version
	   0.11			Amanda Deng			2007.03.02		add interface function address is NULL or not
********************************************************************************************/
ME_U32 me_encoder_register(me_encoder_t *encoder)
{
	ME_S32 i;
	ME_U32 *function_address;
	
	#if ME_KM_FUNNAME
		ME_MESSAGE(("->me_encoder_register()"));
	#endif

	if(encoder==NULL)
		return ME_ERR_ENCODER_IS_NULL;

	function_address = (ME_U32*)encoder +1;
	//encoder interface check
	for(i=0; i<ME_PEREMPTORY_ENCODER_INTERFACE;i++)
	{
		if (*(function_address+i)==0)
		{
			ME_ERROR(("ME_ERR_ENCODER_HAVE_NULL_FUNCTION -0-"));
			return ME_ERR_ENCODER_HAVE_NULL_FUNCTION;
		}
	}
	
	return me_list_append_content(_g_media_engine.encoderlist, (void*)encoder);
}
/*******************************************************************************************
Description:
	for user register their id3 parser function
Parameters:
       me_id3_t *id3: user id3 parser structor point;
Return:	
      ME_SUCCESS: no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.16		create first version
	   0.11			Amanda Deng			2007.03.02		add interface function address is NULL or not
********************************************************************************************/
ME_U32 me_id3_register(me_id3_t *id3)
{
	ME_S32 i;
	ME_U32 *function_address;
	
	#if ME_KM_FUNNAME
		ME_MESSAGE(("->me_id3_register()"));
	#endif

	if(id3==NULL)
		return ME_ERR_ID3_IS_NULL;

	function_address = (ME_U32*)id3 +1;
	//id3 interface check
	for(i=0; i<ME_PEREMPTORY_ID3_INTERFACE;i++)
	{
		if (*(function_address+i)==0)
		{
			ME_ERROR(("ME_ERR_ID3_HAVE_NULL_FUNCTION -0-"));
			return ME_ERR_ID3_HAVE_NULL_FUNCTION;
		}
	}

	return me_list_append_content(_g_media_engine.id3list, (void*)id3);
}

/*******************************************************************************************
Description:
	creat player file interface structor (include parser and decoder)
Parameters:
       fs_callbacks *fs_filefunc: file operate function structor point
       void* mediafile: 	file handle(point)
       char* filetype:		file type(4bytes length)
       MP_FILE *playerfile: player file interface structor creat by media engine
Return:	
      ME_SUCCESS: no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.12.31		create first version
	   0.11			Amanda Deng			2007.11.22		modify input parameter
********************************************************************************************/
ME_U32 me_player_open(me_player_infor* ply_infor, MP_FILE **mpfile)
{
	ME_U32 		   result;
	MP_FILE		   *pthis;
	ME_U32 		   videosize;
	AUD_BUF_INFO Abuf_Info;
	
#if ME_AUDIO_POSTPROC_ENABLE
	me_node_t		*pnod	= NULL;
	me_effectproc_t	*effectproc= NULL;
#endif

#if (ME_VERSION2_FOURHANDLE)
	//ME_U32 i;
	//char tmp_audio_id[4];
#endif
	
#if ME_KM_FUNNAME
	ME_MESSAGE(("->me_player_open()"));
#endif
	
	/*input parameter check*/
	ME_ASSERT_PARAM_RETURN((ply_infor!=NULL));
	
	/*media engine file creat*/
	pthis= (MP_FILE*)me_malloc( sizeof(MP_FILE));
	MSG_FATAL("me_malloc() ", 0, 0, 0);
	
	if(!pthis)
	{
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		MSG_FATAL("ME_ERR_MALLOC_MEMORY_ERROR()  ", 0, 0, 0);
		return ME_ERR_MALLOC_MEMORY_ERROR;
	}
	me_memset((ME_PU08)pthis, 0, sizeof(MP_FILE));
	MSG_FATAL("me_memset() ", 0, 0, 0);
	
	/*input parameter backup*/
	pthis->fs_filefunc		= ply_infor->fs_filefunc;
	pthis->fs_filehandle	= ply_infor->fs_filehandle;		
	 					
	/*select id3 according file type . Amanda Deng 2007.8.27*/
	result = me_id3_select(_g_media_engine.id3list, ply_infor->fs_filetype, &(pthis->id3));
	MSG_FATAL("me_id3_select() ", 0, 0, 0);
	if(result) 
	{
		MSG_FATAL("me_id3_select()  ", 0, 0, 0);
		ME_ERROR(("me_id3_select() return = 0x%08X", result));
		me_free(pthis);
		return result;
	}
	result = me_id3_open(pthis->id3,ply_infor->fs_filehandle, ply_infor->fs_filefunc, &(pthis->id3infor));
	MSG_FATAL("me_id3_open() ", 0, 0, 0);
	if(result) 
	{
		MSG_FATAL("me_id3_open()  ", 0, 0, 0);
		ME_ERROR(("me_id3_open() return = 0x%08X", result));
		me_free(pthis);
		return result;
	}
	
	/*select parser according file type . Amanda Deng 2006.12.31*/
	me_parser_quick_select(_g_media_engine.parserlist, ply_infor->fs_filetype, &(pthis->parser));
	MSG_FATAL("me_parser_quick_select() ", 0, 0, 0);
	if(pthis->parser==NULL) 
	{
		MSG_FATAL("pthis->parser ==null", 0, 0, 0);
		/*select parser and init parser . Amanda Deng 2007.05.30*/
		result = me_parser_check_select(_g_media_engine.parserlist,
									     ply_infor->fs_filehandle, 
									     ply_infor->fs_filefunc, 
									     &(pthis->parser));
		
		MSG_FATAL("%d", result, 0, 0);
		
		if(result) 
		{
			ME_ERROR(("ME_ERR_UNSUPPORT_FILETYPE %c%c%c%c", ply_infor->fs_filetype[0],
															    ply_infor->fs_filetype[1],
															    ply_infor->fs_filetype[2],
															    ply_infor->fs_filetype[3]));
			me_id3_close(pthis->id3,pthis->id3infor);
			me_free(pthis);
			return result;
		}
	}
	else
	{
		/*check file is ok or not*/
		result = pthis->parser->check_file_type(ply_infor->fs_filehandle, ply_infor->fs_filefunc);
		MSG_FATAL("pthis->parser->check_file_type() ", 0, 0, 0);
		if(!result) 	
		{	
			/*false, user input file type is error, message out information*/
			ME_ERROR(("the file is not %c%c%c%c file!", ply_infor->fs_filetype[0],
												     ply_infor->fs_filetype[1],
												     ply_infor->fs_filetype[2],
												     ply_infor->fs_filetype[3]));
			
			MSG_FATAL("pthis->parser->check_file_type()  ", 0, 0, 0);
			result = 1;	//	Modified by antonliu, Do not check other parsers while extname not fit to others'
			if(result) 
			{
				/*error, we unsupport the file, return error code*/
				ME_ERROR(("ME_ERR_UNSUPPORT_FILETYPE, and file is not %c%c%c%c file!", ply_infor->fs_filetype[0],
															           ply_infor->fs_filetype[1],
															           ply_infor->fs_filetype[2],
															           ply_infor->fs_filetype[3]));
				me_id3_close(pthis->id3,pthis->id3infor);
				me_free(pthis);
				return result;
			}
		}
	}
	/*set filetype according parser id, add by Amanda Deng 2007/11/22*/
	me_memcpy((ME_PS08)pthis->fs_filetype, (ME_PCS08)pthis->parser->id, 4);	
	me_printf("ME_FILETYPE_IS: %c%c%c%c", pthis->fs_filetype[0],
										     pthis->fs_filetype[1],
										     pthis->fs_filetype[2],
										     pthis->fs_filetype[3]);

	/*initialize parser . Amanda Deng  2006.12.31*/
	result = me_parser_open(pthis->parser, ply_infor->fs_filehandle, ply_infor->fs_filefunc, &(pthis->parserinfor));
	if(result) 
	{
		/*error, parser initialize error, return error code*/
		ME_ERROR(("me_parser_open() return = 0x%08X", result));
		MSG_FATAL("me_parser_open()  ", 0, 0, 0);
		me_id3_close(pthis->id3,pthis->id3infor);
		me_free(pthis);
		return result;
	}
	pthis->parser->get_video_info(pthis->parserinfor, &(pthis->video_infor));
	if(pthis->video_infor.v_isvalid)
	{
		result=pthis->parser->get_video_frame_buffersize(pthis->parserinfor, &videosize);
		if((pthis->video_infor.v_imagewidth*pthis->video_infor.v_imageheight>177*144) && (pthis->fs_filetype[0] != 'j'))
		{
			me_parser_close(pthis->parser,pthis->parserinfor,&pthis->audio_infor,&pthis->video_infor);
			me_id3_close(pthis->id3,pthis->id3infor);
			me_free(pthis);
			return VIDEO_SIZE_RUN_OVER;
		}
		ME_DEBUG(("Get videosize (%d) !!!!!!!\n",videosize));
		if(videosize > MAX_VIDEO_SIZE && (pthis->fs_filetype[0] != 'j'))
		{
			ME_ERROR(("Get Video Buff Size Is (0x%x) filetype[0](%s) !\n",videosize,pthis->fs_filetype));
			result = me_parser_close(pthis->parser,pthis->parserinfor, &pthis->audio_infor, &pthis->video_infor);

			pthis->parser = NULL;

			result = me_id3_close(pthis->id3,pthis->id3infor);
			return VIDEO_SIZE_RUN_OVER;
		}
	}
	/*get audio information by parser . Amanda Deng 2006.12.31*/
	pthis->parser->get_audio_info(pthis->parserinfor, &(pthis->audio_infor));
//just for test by dnn 4/25
	if(pthis->audio_infor.a_isvalid)
	{	
		ME_DEBUG(("audio_infor.a_chn	= %d",pthis->audio_infor.a_chn));
		ME_DEBUG(("audio_infor.a_smplrate = %d",pthis->audio_infor.a_smplrate));
		ME_DEBUG(("audio_infor.a_bits	= %d",pthis->audio_infor.a_bits));
		ME_DEBUG(("audio_infor.a_bitrate	= %d",pthis->audio_infor.a_bitrate));
		ME_DEBUG(("audio_infor.a_frmsize	= %d",pthis->audio_infor.a_frmsize));
		ME_DEBUG(("audio_infor.a_totaltime	= %d",pthis->audio_infor.a_totaltime));
		ME_DEBUG(("audio_infor.a_codec : %c%c%c%c", pthis->audio_infor.a_codec[0],
										     	pthis->audio_infor.a_codec[1],
										     	pthis->audio_infor.a_codec[2],
										    	pthis->audio_infor.a_codec[3]));
#if !(ME_VERSION2_FOURHANDLE)		
		/*select audio decoder and init audio decoder . Amanda Deng 2006.12.31*/
		result = me_decoder_select(_g_media_engine.decoderlist, pthis->audio_infor.a_codec, &(pthis->audio_decoder));
		if(result) 
		{
			ME_ERROR(("me_decoder_select() return = 0x%08X", result));
			me_parser_close(pthis->parser,pthis->parserinfor,&pthis->audio_infor,&pthis->video_infor);
			me_id3_close(pthis->id3,pthis->id3infor);
			me_free(pthis);
			return result;
		}
		pthis->audio_outinfor.a_outchn		= ply_infor->a_outchn;
		pthis->audio_outinfor.a_outbits		= ply_infor->a_outbits;
		pthis->audio_outinfor.a_outsmplrate	= ply_infor->a_outsmplrate;
		pthis->audio_outinfor.a_outmode        = ply_infor->a_outmode;
		result = me_decoder_open(pthis->audio_decoder, (void*)(&pthis->audio_infor), (void*)(&pthis->audio_outinfor), &(pthis->audio_decinfor));
		if(result) 
		{
			ME_ERROR(("me_decoder_open() return = 0x%08X", result));
			me_parser_close(pthis->parser,pthis->parserinfor,&pthis->audio_infor,&pthis->video_infor);
			me_id3_close(pthis->id3,pthis->id3infor);
			me_free(pthis);
			return result;
		}
		//audio postprocess register by user, one or more be registered after open before playing
		pthis->audio_postproc= (me_node_list*)me_malloc( sizeof(me_node_list));
		if(!pthis->audio_postproc)
		{
			ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
			me_decoder_close(pthis->audio_decoder,pthis->audio_decinfor);
			me_parser_close(pthis->parser,pthis->parserinfor,&pthis->audio_infor,&pthis->video_infor);
			me_id3_close(pthis->id3,pthis->id3infor);
			me_free(pthis);
			return ME_ERR_MALLOC_MEMORY_ERROR;
		}
		pthis->audio_postproc->counter	= 0;
		pthis->audio_postproc->cur		= NULL;
		pthis->audio_postproc->first	= NULL;
		pthis->audio_postproc->last		= NULL;
#if ME_AUDIO_POSTPROC_ENABLE
		//register audio postprocess
		postproc_SRC_register(pthis);
		postproc_EQ_register(pthis);
		postproc_3D_register(pthis);
		postproc_MIR_register(pthis);
		postproc_Slowplay_register(pthis);
		pnod = pthis->audio_postproc->first;
		while(pnod)
		{
			effectproc = (me_effectproc_t*)pnod->content;
			effectproc->init(effectproc, &pthis->audio_infor);
			pnod = pnod->next;
		}
#endif
#else

		if((pthis->audio_infor.a_codec[0]=='m')&&(pthis->audio_infor.a_codec[1]=='p')
			&&(pthis->audio_infor.a_codec[2]=='4')&&(pthis->audio_infor.a_codec[3]=='a'))
		{
			if(media_sysstatus->work_mode & VIM_MEDIA_MODE_AAC_DISABLE)
			{
				ME_ERROR(("\naac or mp4a isn't supported\n"));
				pthis->audio_infor.a_isvalid = 0;
				result = MMD_SYS_Mode_Switch(SCENE_MEDIAPLAY);
				result = MMD_SYS_Codec_Switch(CODEC_MP4A);
				if(result) {
					ME_ERROR(("MMD_SYS_Mode_Switch=0x%08x", result));
					me_parser_close(pthis->parser,pthis->parserinfor,&pthis->audio_infor,&pthis->video_infor);
					me_id3_close(pthis->id3,pthis->id3infor);
					me_free(pthis);
					return result;
				}
				media_player_mp4_mp3_smtd(1);			
			}
			else
			{
				result = MMD_SYS_Mode_Switch(SCENE_MEDIAPLAY);
				result = MMD_SYS_Codec_Switch(CODEC_MP4A);
				if(result) {
					ME_ERROR(("MMD_SYS_Mode_Switch=0x%08x", result));
					me_parser_close(pthis->parser,pthis->parserinfor,&pthis->audio_infor,&pthis->video_infor);
					me_id3_close(pthis->id3,pthis->id3infor);
					me_free(pthis);
					return result;
				}
				media_player_mp4_mp3_smtd(1);				
			}	
			MMD_SYS_SetModeClk(CLK_CPU216M_BUS72M);
		}
		else if((pthis->audio_infor.a_codec[0]=='s')&&(pthis->audio_infor.a_codec[1]=='a')
			&&(pthis->audio_infor.a_codec[2]=='m')&&(pthis->audio_infor.a_codec[3]=='r'))
		{
			result = MMD_SYS_Mode_Switch(SCENE_MEDIAPLAY);
			result = MMD_SYS_Codec_Switch(CODEC_SAMR);
			if(result) {
				ME_ERROR(("MMD_SYS_Mode_Switch=0x%08x", result));
				me_parser_close(pthis->parser,pthis->parserinfor,&pthis->audio_infor,&pthis->video_infor);
				me_id3_close(pthis->id3,pthis->id3infor);
				me_free(pthis);
				return result;
			}
			MMD_SYS_SetModeClk(CLK_CPU216M_BUS72M);
			media_player_mp4_mp3_smtd(1);
		}
		else if((pthis->audio_infor.a_codec[0]=='m')&&(pthis->audio_infor.a_codec[1]=='p')
			&&(pthis->audio_infor.a_codec[2]=='3'))
		{
			result = MMD_SYS_Mode_Switch(SCENE_MEDIAPLAY);
			result = MMD_SYS_Codec_Switch(CODEC_MP3);
			if(result) {
				ME_ERROR(("MMD_SYS_Mode_Switch=0x%08x", result));
				me_parser_close(pthis->parser,pthis->parserinfor,&pthis->audio_infor,&pthis->video_infor);
				me_id3_close(pthis->id3,pthis->id3infor);
				me_free(pthis);
				return result;
			}
			//MMD_SYS_SetModeClk(CLK_CPU120M_BUS60M);
			MMD_SYS_SetModeClk(CLK_CPU216M_BUS72M);
			media_player_mp4_mp3_smtd(0);
		}
		else if((pthis->audio_infor.a_codec[0]=='m')&&(pthis->audio_infor.a_codec[1]=='i')
			&&(pthis->audio_infor.a_codec[2]=='d'))
		{
			result = MMD_SYS_Mode_Switch(SCENE_MEDIAPLAY);
			result = MMD_SYS_Codec_Switch(CODEC_MID);
			if(result) {
				ME_ERROR(("MMD_SYS_Mode_Switch=0x%08x", result));
				me_parser_close(pthis->parser,pthis->parserinfor,&pthis->audio_infor,&pthis->video_infor);
				me_id3_close(pthis->id3,pthis->id3infor);
				me_free(pthis);
				return result;
			}
			MMD_SYS_SetModeClk(CLK_CPU216M_BUS72M);
			media_player_mp4_mp3_smtd(0);
			pthis->parser->get_audio_info_formidi(pthis->parserinfor, (void*)(&pthis->audio_infor));
		}	
		else if((pthis->audio_infor.a_codec[0]=='a')&&(pthis->audio_infor.a_codec[1]=='m')
			&&(pthis->audio_infor.a_codec[2]=='r'))
		{
			result = MMD_SYS_Mode_Switch(SCENE_MEDIAPLAY);
			result = MMD_SYS_Codec_Switch(CODEC_AMR);
			if(result) {
				ME_ERROR(("MMD_SYS_Mode_Switch=0x%08x", result));
				me_parser_close(pthis->parser,pthis->parserinfor,&pthis->audio_infor,&pthis->video_infor);
				me_id3_close(pthis->id3,pthis->id3infor);
				me_free(pthis);
				return result;
			}

			MMD_SYS_SetModeClk(CLK_CPU120M_BUS60M);
	
			media_player_mp4_mp3_smtd(0);
		}
		else if((pthis->audio_infor.a_codec[0]=='a')&&(pthis->audio_infor.a_codec[1]=='a')
			&&(pthis->audio_infor.a_codec[2]=='c'))
		{
			result = MMD_SYS_Mode_Switch(SCENE_MEDIAPLAY);
			result = MMD_SYS_Codec_Switch(CODEC_AAC);
			if(result) {
				ME_ERROR(("MMD_SYS_Mode_Switch=0x%08x", result));
				me_parser_close(pthis->parser,pthis->parserinfor,&pthis->audio_infor,&pthis->video_infor);
				me_id3_close(pthis->id3,pthis->id3infor);
				me_free(pthis);
				return result;
			}
			MMD_SYS_SetModeClk(CLK_CPU120M_BUS60M);
			media_player_mp4_mp3_smtd(0);			
		}
		else if((pthis->audio_infor.a_codec[0]=='p')&&(pthis->audio_infor.a_codec[1]=='c')
			&&(pthis->audio_infor.a_codec[2]=='m'))
		{
			result = MMD_SYS_Mode_Switch(SCENE_MEDIAPLAY);
			result = MMD_SYS_Codec_Switch(CODEC_PCM);
			if(result) {
				ME_ERROR(("MMD_SYS_Mode_Switch=0x%08x", result));
				me_parser_close(pthis->parser,pthis->parserinfor,&pthis->audio_infor,&pthis->video_infor);
				me_id3_close(pthis->id3,pthis->id3infor);
				me_free(pthis);
				return result;
			}
			MMD_SYS_SetModeClk(CLK_CPU120M_BUS60M);
			media_player_mp4_mp3_smtd(0);
		}
		else if((pthis->audio_infor.a_codec[0]=='i')&&(pthis->audio_infor.a_codec[1]=='m')
			&&(pthis->audio_infor.a_codec[2]=='a'))
		{
			result = MMD_SYS_Mode_Switch(SCENE_MEDIAPLAY);
			result = MMD_SYS_Codec_Switch(CODEC_IMA);
			if(result) {
				ME_ERROR(("MMD_SYS_Mode_Switch=0x%08x", result));
				me_parser_close(pthis->parser,pthis->parserinfor,&pthis->audio_infor,&pthis->video_infor);
				me_id3_close(pthis->id3,pthis->id3infor);
				me_free(pthis);
				return result;
			}
			MMD_SYS_SetModeClk(CLK_CPU120M_BUS60M);
			media_player_mp4_mp3_smtd(0);
		}		
		//init decoder
	/*	result=pthis->parser->get_audio_frame_buffersize(pthis->parserinfor, &size, (void*)(&pthis->audio_infor) );
		if(result)
		{
			mp_error(("get_audio_frame_buffersize() retrun =0x%08X",result));
			return result;
		}
		pthis->audio_infor.a_maxfrmsize=size;*/
		pthis->audio_outinfor.a_outbits		= 16;//pthis->audio_infor.a_bits;
		//: Modified by antonliu, 2008/06/06
		pthis->audio_outinfor.a_outchn		= 2;//pthis->audio_infor.a_chn;//		
		pthis->audio_outinfor.a_outmode        = ply_infor->a_outmode;	
		if(pthis->audio_outinfor.a_outmode)
		{
			#if 1
			if(pthis->audio_infor.a_smplrate == 8000)
			{
				pthis->audio_outinfor.a_outsmplrate = 16000;
			}
			else
			{
				pthis->audio_outinfor.a_outsmplrate	= pthis->audio_infor.a_smplrate;
			}
			#else
				pthis->audio_outinfor.a_outsmplrate	= pthis->audio_infor.a_smplrate;
			#endif
		}		
		else
		{
			pthis->audio_outinfor.a_outsmplrate	= 8000;
		}
		Abuf_Info.AUDIO_CODEC = (pthis->audio_infor.a_codec[0])|(pthis->audio_infor.a_codec[1]<<8)|
							(pthis->audio_infor.a_codec[2]<<16)|(pthis->audio_infor.a_codec[3]<<24);
		result = media_player_audio_codecinit_smtd(&Abuf_Info);
		if(result) {
			MMD_ERROR_Deal(CMD_FAILED,MAJ_PLY);
			mp_error(("media_player_audio_bufinit_smtd() return = 0x%08X",result));
			return MP_ERR_MALLOC_MEMORY_ERROR;
		}
		result = media_player_audio_decinit_smtd(&(pthis->audio_infor), &(pthis->audio_outinfor), (pthis->audio_infor).a_otherslen);
		if(result) {
			MMD_ERROR_Deal(CMD_FAILED,MAJ_PLY);
			ME_ERROR(("media_player_audio_decinit_smtd() return = 0x%08X", result));
			return result;
		}
//		pthis->audio_outinfor.a_outsmplrate	= ply_infor->a_outsmplrate;
//		pthis->audio_outinfor.a_outsmplrate = (pthis->audio_infor.a_smplrate > 48000)?48000:pthis->audio_infor.a_smplrate;
#endif	
	}
	/*get video information by parser . Amanda Deng 2006.12.31*/
	pthis->parser->get_video_info(pthis->parserinfor, &(pthis->video_infor));
	if(pthis->video_infor.v_isvalid)
	{	
		ME_DEBUG(("video_infor.v_imagewidth	= %d",pthis->video_infor.v_imagewidth));
		ME_DEBUG(("video_infor.v_imageheight = %d",pthis->video_infor.v_imageheight));
		ME_DEBUG(("video_infor.v_fps	= %d",pthis->video_infor.v_fps));
		ME_DEBUG(("video_infor.v_bitrate	= %d",pthis->video_infor.v_bitrate));
		ME_DEBUG(("video_infor.v_totaltime	= %d",pthis->video_infor.v_totaltime));
		ME_DEBUG(("video_infor.v_codec : %c%c%c%c", pthis->video_infor.v_codec[0],
										     	pthis->video_infor.v_codec[1],
										     	pthis->video_infor.v_codec[2],
										    	pthis->video_infor.v_codec[3]));
		if(!pthis->audio_infor.a_isvalid)
		{
			if((pthis->video_infor.v_codec[0]=='m')&&(pthis->video_infor.v_codec[1]=='p')//add by wanglixin
			&&(pthis->video_infor.v_codec[2]=='4')&&(pthis->video_infor.v_codec[3]=='v'))
			{
				MMD_SYS_SetModeClk(CLK_CPU216M_BUS72M);
			}
			else if((pthis->video_infor.v_codec[0]=='s')&&(pthis->video_infor.v_codec[1]=='a')//add by wanglixin
			&&(pthis->video_infor.v_codec[2]=='m')&&(pthis->video_infor.v_codec[3]=='r'))
			{
				MMD_SYS_SetModeClk(CLK_CPU216M_BUS72M);
			}
			else if((pthis->video_infor.v_codec[0]=='s')&&(pthis->video_infor.v_codec[1]=='2')//add by wanglixin
			&&(pthis->video_infor.v_codec[2]=='6')&&(pthis->video_infor.v_codec[3]=='3'))
			{
				MMD_SYS_SetModeClk(CLK_CPU216M_BUS72M);
			}			
			ME_DEBUG(("\nset mode clk\n"));			
		}
		 if((pthis->video_infor.v_codec[0]=='j')&&(pthis->video_infor.v_codec[1]=='p')//add by wanglixin
			&&(pthis->video_infor.v_codec[2]=='g'))
		 {
		 	result = MMD_SYS_Mode_Switch(SCENE_MEDIAPLAY);
			result = MMD_SYS_Codec_Switch(CODEC_PCM);
			if(result) {
				ME_ERROR(("MMD_SYS_Mode_Switch=0x%08x", result));
				me_parser_close(pthis->parser,pthis->parserinfor,&pthis->audio_infor,&pthis->video_infor);
				me_id3_close(pthis->id3,pthis->id3infor);
				me_free(pthis);
				return result;
			}
			MMD_SYS_SetModeClk(CLK_CPU216M_BUS72M);
		 }
		pthis->video_infor.v_jpgmode = ply_infor->v_jpgmode;
		pthis->video_infor.v_jpgdecodeaddr = ply_infor->v_jpgdecodeaddr;
		pthis->video_infor.v_jpgdecodelen   = ply_infor->v_jpgdecodelen;
		/*select video decoder and init video decoder . Amanda Deng 2006.12.31*/
		result = me_decoder_select(_g_media_engine.decoderlist,pthis->video_infor.v_codec,&(pthis->video_decoder));
		if(result) 
		{
			ME_ERROR(("me_decoder_select() return = 0x%08X", result));
			MSG_FATAL("me_decoder_select()  ", 0, 0, 0);
			if(pthis->audio_infor.a_isvalid)
			{
				if(pthis->audio_postproc)
				{
				#if ME_AUDIO_POSTPROC_ENABLE
					pnod = pthis->audio_postproc->first;
					while(pnod)
					{
						effectproc = (me_effectproc_t*)pnod->content;
						effectproc->release(effectproc);
						pnod = pnod->next;
					}
				#endif
					me_effectprclist_free(pthis->audio_postproc);
				}
				me_decoder_close(pthis->audio_decoder,pthis->audio_decinfor);
			}
			me_parser_close(pthis->parser,pthis->parserinfor,&pthis->audio_infor,&pthis->video_infor);
			me_id3_close(pthis->id3,pthis->id3infor);
			me_free(pthis);
			return result;
		}
		pthis->video_outinfor.v_srcwidth = ply_infor->v_srcwidth;
		pthis->video_outinfor.v_srcheight = ply_infor->v_srcheight;
		pthis->video_outinfor.v_srcxpos = ply_infor->v_srcxpos;
		pthis->video_outinfor.v_srcypos = ply_infor->v_srcypos;
		pthis->video_outinfor.v_outxpos = ply_infor->v_outxpos;
		pthis->video_outinfor.v_outypos = ply_infor->v_outypos;
		pthis->video_outinfor.v_outwidth = ply_infor->v_outwidth;
		pthis->video_outinfor.v_outheight = ply_infor->v_outheight;
		pthis->video_outinfor.v_outeffect = ply_infor->v_outeffect;
		pthis->video_outinfor.v_outrotation = ply_infor->v_outrotation;
		result = me_decoder_open(pthis->video_decoder, (void*)(&pthis->video_infor), (void*)(&pthis->video_outinfor), &(pthis->video_decinfor));
		if(result) 
		{
			ME_ERROR(("me_decoder_open() return = 0x%08X", result));
			MSG_FATAL("me_decoder_open()  ", 0, 0, 0);
			if(pthis->audio_infor.a_isvalid)
			{
				if(pthis->audio_postproc)
				{
				#if ME_AUDIO_POSTPROC_ENABLE
					pnod = pthis->audio_postproc->first;
					while(pnod)
					{
						effectproc = (me_effectproc_t*)pnod->content;
						effectproc->release(effectproc);
						pnod = pnod->next;
					}
				#endif
					me_effectprclist_free(pthis->audio_postproc);
				}
				me_decoder_close(pthis->audio_decoder,pthis->audio_decinfor);
			}
			me_parser_close(pthis->parser,pthis->parserinfor,&pthis->audio_infor,&pthis->video_infor);
			me_id3_close(pthis->id3,pthis->id3infor);
			me_free(pthis);
			return result;
		}
		//video postprocess register by user, one or more be registered after open before playing
		pthis->video_postproc= (me_node_list*)me_malloc( sizeof(me_node_list));
		if(!pthis->video_postproc)
		{
			ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
			if(pthis->audio_infor.a_isvalid)
			{
				if(pthis->audio_postproc)
				{
				#if ME_AUDIO_POSTPROC_ENABLE
					pnod = pthis->audio_postproc->first;
					while(pnod)
					{
						effectproc = (me_effectproc_t*)pnod->content;
						effectproc->release(effectproc);
						pnod = pnod->next;
					}
				#endif
					me_effectprclist_free(pthis->audio_postproc);
				}
				me_decoder_close(pthis->audio_decoder,pthis->audio_decinfor);
			}
			me_decoder_close(pthis->video_decoder,pthis->video_decinfor);
			me_parser_close(pthis->parser,pthis->parserinfor,&pthis->audio_infor,&pthis->video_infor);
			me_id3_close(pthis->id3,pthis->id3infor);
			me_free(pthis);
			return ME_ERR_MALLOC_MEMORY_ERROR;
		}
		pthis->video_postproc->counter	= 0;
		pthis->video_postproc->cur		= NULL;
		pthis->video_postproc->first	= NULL;
		pthis->video_postproc->last		= NULL;
	}
	(*mpfile) = pthis;
	return ME_SUCCESS;
}
/*******************************************************************************************
Description:
	free player file interface structor (include parser, decoder, buffer)
Parameters:
       MP_FILE *file: player file interface structor creat by media engine
Return:	
      ME_SUCCESS: no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.12.31		create first version
********************************************************************************************/
ME_U32 me_player_close(MP_FILE *mpfile)
{
	ME_U32 result, return_value = ME_SUCCESS;
	
	#if ME_AUDIO_POSTPROC_ENABLE
	me_node_t		*pnod	= NULL;
	me_effectproc_t	*effectproc= NULL;
	#endif

#if (ME_VERSION2_FOURHANDLE)
	//ME_U32 i;
#endif
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->me_player_close()"));
	#endif
	
	if (!mpfile) 
  	{
    		ME_MESSAGE(("me_player_close( file ==NULL ) !"));
 		return ME_SUCCESS;
  	}
	
	// 1. free video decoder
	if(mpfile->video_infor.v_isvalid)
	{
		if(mpfile->video_postproc)
		{
			me_effectprclist_free(mpfile->video_postproc);
		}
		result = me_decoder_close(mpfile->video_decoder, mpfile->video_decinfor);
		if(result) 
		{	
			ME_ERROR(("me_decoder_close() return =  0x%08X!", result));
			return_value = result;
		}
		mpfile->video_decoder	= NULL;
		mpfile->video_decinfor	= NULL;
	}
	
	// 2. free audio decoder
	if(mpfile->audio_infor.a_isvalid )
	{
#if !(ME_VERSION2_FOURHANDLE)	//useless for 848 baseband	
		//free postprc list . add 2007/3/20 by Amanda Deng
		if(mpfile->audio_postproc)
		{
			#if ME_AUDIO_POSTPROC_ENABLE
				pnod = mpfile->audio_postproc->first;
				while(pnod)
				{
					effectproc = (me_effectproc_t*)pnod->content;
					effectproc->release(effectproc);
					pnod = pnod->next;
				}
			#endif
			me_effectprclist_free(mpfile->audio_postproc);
		}
		
		result = me_decoder_close(mpfile->audio_decoder, mpfile->audio_decinfor);
		if(result) 
		{	ME_ERROR(("me_decoder_close() return =  0x%08X!", result));
			return_value = result;
		}
		mpfile->audio_decoder	= NULL;
		mpfile->audio_decinfor	= NULL;
#else
		result = media_player_audio_decrelease_smtd();
		if(result) 
		{	
			MMD_ERROR_Deal(CMD_FAILED,MAJ_PLY);
			ME_ERROR(("media_player_audio_decrelease_smtd() return =  0x%08X!", result));
			return_value = result;
		}
		mpfile->audio_decoder	= NULL;
		mpfile->audio_decinfor	= NULL;
#endif		
	}
	
	// 3. free file parser
	result = me_parser_close(mpfile->parser,mpfile->parserinfor, &mpfile->audio_infor, &mpfile->video_infor);
	if(result) 
	{	ME_ERROR(("me_parser_close() return =  0x%08X!", result));
		return_value = result;
	}
	mpfile->parser = NULL;

	// 4. free file id3
	result = me_id3_close(mpfile->id3,mpfile->id3infor);
	if(result) 
	{	ME_ERROR(("me_id3_close() return =  0x%08X!", result));
		return_value = result;
	}
	
	mpfile->id3 = NULL;
//#if !(ME_VERSION2_FOURHANDLE)
	mpfile->fs_filehandle		= NULL; 
//#else
//	for(i=0;i<MAX_FILE_HANDLE_NUM;i++){
//		mpfile->fs_filehandle[i]	= NULL; 		
//	}
//#endif
	mpfile->fs_filefunc		= NULL;	
	me_free(mpfile);
	
	return return_value;
}
/*******************************************************************************************
Description:
	creat recorder file interface structor (include muxer and encoder)
Parameters:
       fs_callbacks *fs_filefunc: file operate function structor point
       void* mediafile: 	file handle(point)
       char* filetype:		file type(4bytes length)
       MR_FILE *recorderfile: recorder file interface structor creat by media engine
Return:	
      ME_SUCCESS: no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.12.31		create first version
	   0.11			Amanda Deng			2007.11.22		modify input parameter
********************************************************************************************/
ME_U32 me_recorder_open(me_record_infor* rec_infor, MR_FILE **recfile)
{
	ME_U32 	result;
	MR_FILE*pthis;
	#if ME_AUDIO_PREPROC_ENABLE
	me_node_t		*pnod	= NULL;
	me_effectproc_t	*effectproc= NULL;
	#endif
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->me_recorder_open()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((rec_infor!=NULL));

	pthis= (MR_FILE*)me_malloc( sizeof(MR_FILE));
	if(!pthis)
	{
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		return ME_ERR_MALLOC_MEMORY_ERROR;
	}
	me_memset((ME_PU08)pthis, 0, sizeof(MR_FILE));

	pthis->fs_filefunc		= rec_infor->fs_filefunc;		 		/*open时传入的文件操作结构体指针*/
	pthis->fs_filehandle	= rec_infor->fs_filehandle;		 		/*open时传入的文件指针（文件句柄）*/
	me_memcpy((ME_PS08)pthis->fs_filetype, (ME_PCS08)rec_infor->fs_filetype, 4);/*open时传入的文件类型 */

	pthis->audio_infor.a_isvalid		= rec_infor->a_recchn;				/* 1,2: have audio, 0 no audio */
	pthis->audio_infor.a_bits		= rec_infor->a_recbits;			/* bit length */
	pthis->audio_infor.a_chn		= rec_infor->a_recchn;			/* 1: mono, 2: stereo */
	pthis->audio_infor.a_smplrate	= rec_infor->a_recsmplrate;		/* sample rate */
	pthis->audio_infor.a_tickps		= rec_infor->a_recsmplrate;		/* tick number per second (timescale)*/
	pthis->audio_infor.a_bitrate		= rec_infor->a_recbitrate;
	/* audio codec type id */
	me_memcpy((ME_PS08)pthis->audio_infor.a_codec , (ME_PCS08)rec_infor->a_codec, 4);
	
	pthis->video_infor.v_isvalid		= rec_infor->v_recwidth *rec_infor->v_recheight;
	if(pthis->video_infor.v_isvalid)
	{
		if((rec_infor->v_codec[0]=='j')&&(rec_infor->v_codec[1]=='p')&&(rec_infor->v_codec[2]=='g'))
        {
        	pthis->video_infor.v_isvalid=3;
        }
        else
        {
        	pthis->video_infor.v_isvalid=1;
        }
	}
	pthis->video_infor.v_imagewidth		= rec_infor->v_recwidth;			/* video width */
	pthis->video_infor.v_imageheight	= rec_infor->v_recheight;		/* video height */
   	pthis->video_infor.v_fps		= rec_infor->v_recfps;			/* frame number per second(if changeless frame size, else =0) */
	pthis->video_infor.v_tickps		= 1000;						/* tick number per second (timescale)*/
	pthis->video_infor.v_duration	= 50;
	if(rec_infor->v_recfps)
	    pthis->video_infor.v_duration	= 1000/rec_infor->v_recfps;		/* frame duration(if changeless frame rate or first frame duration) */
	
   	/* video codec type id */
	me_memcpy((ME_PS08)pthis->video_infor.v_codec , (ME_PCS08)rec_infor->v_codec, 4);

	/*select muxer according file type and init parser . Amanda Deng 2006.12.31*/
	result = me_muxer_select(_g_media_engine.muxerlist, rec_infor->fs_filetype, &(pthis->muxer));
	if(result) 
	{
		ME_ERROR(("me_muxer_select() return = 0x%08X", result));
		me_free(pthis);
		return result;
	}
	result = me_muxer_open(pthis->muxer,rec_infor->fs_filehandle, rec_infor->fs_filefunc, &(pthis->muxerinfor));
	if(result) 
	{
		me_free(pthis);
		return result;
	}
	
	if(pthis->audio_infor.a_isvalid)
	{	/*select audio encoder according audio codec and init decoder . Amanda Deng 2006.12.31*/
		me_printf("a_codec (%c%c%c%c) \n",pthis->audio_infor.a_codec[0],pthis->audio_infor.a_codec[1],pthis->audio_infor.a_codec[2],pthis->audio_infor.a_codec[3]);
		if((pthis->audio_infor.a_codec[0]=='s')&&(pthis->audio_infor.a_codec[1]=='a')
				&&(pthis->audio_infor.a_codec[2]=='m')&&(pthis->audio_infor.a_codec[3]=='r') ||(pthis->audio_infor.a_codec[0]=='a')
				&&(pthis->audio_infor.a_codec[1]=='m')&&(pthis->audio_infor.a_codec[2]=='r'))//lily added mp3 support
		{
			result = MMD_SYS_Mode_Switch(SCENE_REC_AMR);
			if(result) {
				ME_ERROR(("MMD_SYS_Mode_Switch=0x%08x", result));
				//me_parser_close(pthis->parser,pthis->parserinfor,&pthis->audio_infor,&pthis->video_infor);
				//me_id3_close(pthis->id3,pthis->id3infor);
				me_free(pthis);
				return result;
				MMD_SYS_SetModeClk(CLK_CPU216M_BUS72M);//add by wanglixin
			}
			MMD_SYS_SetModeClk(CLK_CPU216M_BUS72M);
			pthis->audio_inputinfor.a_inputbits	= 16;//rec_infor->a_inputbits;
			pthis->audio_inputinfor.a_inputchn	= 1;//rec_infor->a_inputchn;
		}
		else if((pthis->audio_infor.a_codec[0]=='m')&&(pthis->audio_infor.a_codec[1]=='p')&&(pthis->audio_infor.a_codec[2]=='3'))//lily added mp3 support
		{
			result = MMD_SYS_Mode_Switch(SCENE_REC_MP3);
			if(result) {
				ME_ERROR(("MMD_SYS_Mode_Switch=0x%08x", result));
				//me_parser_close(pthis->parser,pthis->parserinfor,&pthis->audio_infor,&pthis->video_infor);
				//me_id3_close(pthis->id3,pthis->id3infor);
				me_free(pthis);
				return result;
			}
			MMD_SYS_SetModeClk(CLK_CPU216M_BUS72M);
			pthis->audio_inputinfor.a_inputbits	= rec_infor->a_inputbits;
			pthis->audio_inputinfor.a_inputchn	= rec_infor->a_inputchn;
		}
		pthis->audio_inputinfor.a_inputsmplrate=rec_infor->a_inputsmplrate;
	
	}
//#endif	
	if(pthis->video_infor.v_isvalid)
	{	/*select video encoder according video codec and init decoder . Amanda Deng 2006.12.31*/
		 if((pthis->video_infor.v_codec[0]=='j')&&(pthis->video_infor.v_codec[1]=='p')//add by wanglixin
			&&(pthis->video_infor.v_codec[2]=='g'))
		 {
		 	result = MMD_SYS_Mode_Switch(SCENE_MEDIAPLAY);
			result = MMD_SYS_Codec_Switch(CODEC_PCM);
			if(result) {
				ME_ERROR(("MMD_SYS_Mode_Switch=0x%08x", result));
				//me_parser_close(pthis->parser,pthis->parserinfor,&pthis->audio_infor,&pthis->video_infor);
				//me_id3_close(pthis->id3,pthis->id3infor);
				me_free(pthis);
				return result;
			}
			MMD_SYS_SetModeClk(CLK_CPU216M_BUS72M);
		 }
		result = me_encoder_select(_g_media_engine.encoderlist,pthis->video_infor.v_codec,&(pthis->video_encoder));
		if(result) 
		{
			ME_ERROR(("me_encoder_select() return = 0x%08X", result));
			if(pthis->audio_infor.a_isvalid)
				{/*me_encoder_close(pthis->audio_encoder,pthis->audio_encinfor);*/}
			me_muxer_close(pthis->muxer,pthis->muxerinfor);
			me_free(pthis);
			return result;
		}
		pthis->video_inputinfor.v_previewwidth	= rec_infor->v_previewwidth;
		pthis->video_inputinfor.v_previewheight	= rec_infor->v_previewheight;
		pthis->video_inputinfor.v_previewxpos	= rec_infor->v_previewxpos;
		pthis->video_inputinfor.v_previewypos	= rec_infor->v_previewypos;
		pthis->video_inputinfor.v_inputwidth		= rec_infor->v_inputwidth;
		pthis->video_inputinfor.v_inputheight		= rec_infor->v_inputheight;
		pthis->video_inputinfor.v_inputfps		= rec_infor->v_inputfps;
		pthis->video_inputinfor.v_inputrotation	= rec_infor->v_inputrotation;
		pthis->video_inputinfor.v_inputeffect		= rec_infor->v_inputeffect;
		result = me_encoder_open(pthis->video_encoder,(void*)(&pthis->video_infor), (void*)(&pthis->video_inputinfor), &(pthis->video_encinfor));
		if(result) 
		{
			ME_ERROR(("me_encoder_open() return = 0x%08X", result));
			if(pthis->audio_infor.a_isvalid)
				{/*me_encoder_close(pthis->audio_encoder,pthis->audio_encinfor);*/}
			me_muxer_close(pthis->muxer,pthis->muxerinfor);
			me_free(pthis);
			return result;
		}
	}

	(*recfile) = pthis;
	return ME_SUCCESS;
}
/*******************************************************************************************
Description:
	free recorder file interface structor (include muxer ,encoder, buffer)
Parameters:
       MR_FILE *file: recorder file interface structor creat by media engine
Return:	
      ME_SUCCESS: no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.12.31		create first version
********************************************************************************************/
ME_U32 me_recorder_close(MR_FILE *recfile)
{
	ME_U32 result, return_value = ME_SUCCESS;
	#if ME_AUDIO_PREPROC_ENABLE
	me_node_t		*pnod	= NULL;
	me_effectproc_t	*effectproc= NULL;
	#endif
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->vm_recorder_close()"));
	#endif

	ME_ASSERT_PARAM_RETURN((recfile!=NULL));
	
	/*if(recfile->audio_infor.a_isvalid )
	{
		//free postprc list . add 2008/01/21 by Amanda Deng
		if(recfile->audio_preproc)
		{
			#if ME_AUDIO_PREPROC_ENABLE
				pnod = recfile->audio_preproc->first;
				while(pnod)
				{
					effectproc = (me_effectproc_t*)pnod->content;
					effectproc->release(effectproc);
					pnod = pnod->next;
				}
			#endif
			me_effectprclist_free(recfile->audio_preproc);
		}
		result = me_encoder_close(recfile->audio_encoder,recfile->audio_encinfor);
		if(result) 
		{	
			ME_ERROR(("me_encoder_close() return =  0x%08X!", result));
			return_value = result;
		}
		recfile->audio_encoder= NULL;
		recfile->audio_encinfor= NULL;
	}*/
	
	if(recfile->video_infor.v_isvalid )
	{
		if(recfile->video_preproc)
		{
			me_effectprclist_free(recfile->video_preproc);
		}
		result = me_encoder_close(recfile->video_encoder,recfile->video_encinfor);
		if(result) 
		{	
			ME_ERROR(("me_encoder_close() return =  0x%08X!", result));
			return_value = result;
		}
		recfile->video_encoder = NULL;
		recfile->video_encinfor = NULL;
	}

	result = me_muxer_close(recfile->muxer,recfile->muxerinfor);
	if(result) 
	{	
		ME_ERROR(("me_encoder_close() return =  0x%08X!", result));
		return_value = result;
	}
	
	recfile->muxer		= NULL;
	recfile->muxerinfor	= NULL;
	recfile->fs_filehandle		= NULL; 
	recfile->fs_filefunc		= NULL;	
	me_free(recfile);			
	
	return return_value;
}

/*******************************************************************************************
Description:
	for user register post data process function of player file
Parameters:
       MP_FILE *mpfile: player file interface structor creat by media engine
       me_effectproc_t *postproc: post data process function structor point
Return:	
      ME_SUCCESS: no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.15		create first version
	   0.11			Amanda Deng			2007.03.02		add interface function address is NULL or not
********************************************************************************************/
ME_U32 me_audio_postproc_register(MP_FILE *mpfile, me_effectproc_t *postproc)
{
	ME_S32 i;
	ME_U32 *function_address;
	
	#if ME_KM_FUNNAME
		ME_MESSAGE(("->me_audio_postproc_register()"));
	#endif

	if(postproc==NULL)
		return ME_ERR_POSTPROC_IS_NULL;

	function_address = (ME_U32*)postproc + 3;
	//id3 interface check
	for(i=0; i<ME_PEREMPTORY_POSTPROC_INTERFACE;i++)
	{
		if (*(function_address+i)==0)
		{
			ME_ERROR(("ME_ERR_POSTPROC_HAVE_NULL_FUNCTION -0-"));
			return ME_ERR_POSTPROC_HAVE_NULL_FUNCTION;
		}
	}

	return me_list_append_content(mpfile->audio_postproc, (void*)postproc);
}

ME_U32 me_video_postproc_register(MP_FILE *mpfile, me_effectproc_t *postproc)
{
	ME_S32 i;
	ME_U32 *function_address;
	
	#if ME_KM_FUNNAME
		ME_MESSAGE(("->me_video_postproc_register()"));
	#endif

	if(postproc==NULL)
		return ME_ERR_POSTPROC_IS_NULL;

	function_address = (ME_U32*)postproc + 3;
	//effectprocess interface check
	for(i=0; i<ME_PEREMPTORY_POSTPROC_INTERFACE;i++)
	{
		if (*(function_address+i)==0)
		{
			ME_ERROR(("ME_ERR_POSTPROC_HAVE_NULL_FUNCTION -0-"));
			return ME_ERR_POSTPROC_HAVE_NULL_FUNCTION;
		}
	}

	return me_list_append_content(mpfile->video_postproc, (void*)postproc);
}

/*******************************************************************************************
Description:
	for user register preprocess data function of player file
Parameters:
       MR_FILE *mrfile: recorder file interface structor creat by media engine
       me_effectproc_t *postproc: post data process function structor point
Return:	
      ME_SUCCESS: no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.08.27		create first version
********************************************************************************************/
ME_U32 me_audio_preproc_register(MR_FILE *mrfile, me_effectproc_t *preproc)
{
	ME_S32 i;
	ME_U32 *function_address;
	
	#if ME_KM_FUNNAME
		ME_MESSAGE(("->me_audio_preproc_register()"));
	#endif

	if(preproc==NULL)
		return ME_ERR_POSTPROC_IS_NULL;

	function_address = (ME_U32*)preproc + 3;
	//id3 interface check
	for(i=0; i<ME_PEREMPTORY_POSTPROC_INTERFACE;i++)
	{
		if (*(function_address+i)==0)
		{
			ME_ERROR(("ME_ERR_POSTPROC_HAVE_NULL_FUNCTION -0-"));
			return ME_ERR_POSTPROC_HAVE_NULL_FUNCTION;
		}
	}

	return me_list_append_content(mrfile->audio_preproc, (void*)preproc);
}

ME_U32 me_video_preproc_register(MR_FILE *mrfile, me_effectproc_t *preproc)
{
	ME_S32 i;
	ME_U32 *function_address;
	
	#if ME_KM_FUNNAME
		ME_MESSAGE(("->me_video_preproc_register()"));
	#endif

	if(preproc==NULL)
		return ME_ERR_POSTPROC_IS_NULL;

	function_address = (ME_U32*)preproc + 3;
	//id3 interface check
	for(i=0; i<ME_PEREMPTORY_POSTPROC_INTERFACE;i++)
	{
		if (*(function_address+i)==0)
		{
			ME_ERROR(("ME_ERR_POSTPROC_HAVE_NULL_FUNCTION -0-"));
			return ME_ERR_POSTPROC_HAVE_NULL_FUNCTION;
		}
	}
	return me_list_append_content(mrfile->video_preproc, (void*)preproc);
}

