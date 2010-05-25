/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	me_parser.c
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/12/31
* Last modify : Amanda Deng 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/

#include "me_parser.h"

/*******************************************************************************************
Description:
	according file type select parser form parser list.
Parameters:
       me_node_list *pthis:	parser list point
       char* filetype:			file type (4 bytes length)
       me_parser_t** parser: 	selected parser for user
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.12.31		create first version
********************************************************************************************/
void me_parser_quick_select(me_node_list *pthis, char* filetype, me_parser_t **parser)
{
	me_node_t			*pnod	= NULL;
	me_parser_t			*parser_cls	= NULL;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->me_parser_quick_select()"));
	#endif
	
	(*parser) =NULL;
	
	ME_ASSERT_RETURN((pthis!=NULL));
	ME_ASSERT_RETURN((filetype!=NULL));

	pnod = pthis->first;
	while(pnod)
	{
		parser_cls = (me_parser_t*)pnod->content;
		if(me_parser_check_id(parser_cls,filetype))
		{
			(*parser) = parser_cls;
			return ;
		}
		pnod = pnod->next;
	}
}

ME_U32 me_parser_check_select(me_node_list *pthis,void *fs_filehandle, fs_callbacks *fs_filefunc, me_parser_t **parser)
{
	me_node_t			*pnod	= NULL;
	me_parser_t			*parser_cls	= NULL;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->me_parser_check_select()"));
	#endif
	
	(*parser) =NULL;
	ME_ASSERT_PARAM_RETURN((pthis!=NULL));
	ME_ASSERT_PARAM_RETURN((fs_filehandle!=NULL));
	ME_ASSERT_PARAM_RETURN((fs_filefunc!=NULL));
	
	pnod = pthis->first;
	while(pnod)
	{
		parser_cls = (me_parser_t*)pnod->content;
		if(parser_cls->check_file_type(fs_filehandle,fs_filefunc))
		{
			(*parser) = parser_cls;
			return ME_SUCCESS;
		}
		pnod = pnod->next;
	}
	return ME_ERR_UNSUPPORT_FILETYPE;
}

/*******************************************************************************************
Description:
	parser initialize for player
Parameters:
       me_encoder_t *pthis:	parser interface structor point
       void *mediafile:		file handle(point)
       fs_callbacks *fs_filefunc:		file operate interface structor point
       void **fileinfor:		file information structor create by parser
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.12.31		create first version
********************************************************************************************/
ME_U32 me_parser_open(me_parser_t *pthis, void *fs_filehandle, fs_callbacks *fs_filefunc,void **fileinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->me_parser_open()"));
	#endif
	
	(*fileinfor)=NULL;
	ME_ASSERT_PARAM_RETURN((pthis!=NULL));
	ME_ASSERT_PARAM_RETURN((fs_filehandle!=NULL));
	ME_ASSERT_PARAM_RETURN((fs_filefunc!=NULL));
	
	return pthis->init(fs_filehandle,fs_filefunc,fileinfor);
}
/*******************************************************************************************
Description:
	parser close by player
Parameters:
      me_encoder_t *pthis: parser interface structor point
      void **fileinfor:		file information structor create by parser
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.12.31		create first version
********************************************************************************************/
ME_U32	me_parser_close(me_parser_t *pthis, void *fileinfor, me_audio_infor *audio_infor, me_video_infor *video_infor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->me_parser_close()"));
	#endif
	
	if (!pthis) 
  	{     
    		ME_MESSAGE(("me_parser_close( pthis ==NULL ) !"));
 		return ME_SUCCESS;
  	}

	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	return pthis->release(fileinfor, audio_infor, video_infor);
}

