/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	me_decoder.c 
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/12/31
* Last modify : Amanda Deng 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/

#include "me_decoder.h"

/*******************************************************************************************
Description:
	according codec id select decoder form decoder list.
Parameters:
       me_node_list *pthis:	decoder list point
       char* codec_id:		decode id (4 bytes length)
       me_decoder_t** decoder: selected decoder for user
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.12.31		create first version
********************************************************************************************/
ME_U32	me_decoder_select(me_node_list *pthis, char* codec_id, me_decoder_t** decoder)
{
	me_node_t			*pnod	= NULL;
	me_decoder_t			*pdecoder_cls	= NULL;
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->me_decoder_select()"));
	#endif
	
	(*decoder) = NULL;
	ME_ASSERT_PARAM_RETURN((pthis!=NULL));
	ME_ASSERT_PARAM_RETURN((codec_id!=NULL));
	
	pnod = pthis->first;
	while(pnod)
	{
		pdecoder_cls = (me_decoder_t*)pnod->content;
		if(me_decoder_check_id(pdecoder_cls,codec_id))
		{
			(*decoder) = pdecoder_cls;
			return ME_SUCCESS;
		}
		pnod = pnod->next;
	}
	ME_ERROR(("ME_ERR_UNSUPPORT_DECODER %c%c%c%c", codec_id[0],codec_id[1],codec_id[2],codec_id[3]));
	return ME_ERR_UNSUPPORT_DECODER;
}
/*******************************************************************************************
Description:
	decoder initialize for player
Parameters:
       me_decoder_t *pthis:	decoder interface structor point
       void *mediafile:		file handle(point)
       fs_callbacks *fs_filefunc:		file operate interface structor point
       void **decodeinfor:		decode information structor create by decoder
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.12.31		create first version
********************************************************************************************/
ME_U32	me_decoder_open(me_decoder_t *pthis, void *srcinfor, void *outinfor, void **decodeinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->me_decoder_open()"));
	#endif
	
	*decodeinfor=NULL;
	ME_ASSERT_PARAM_RETURN((pthis!=NULL));
	ME_ASSERT_PARAM_RETURN((srcinfor!=NULL));
	
	return pthis->init(srcinfor, outinfor, decodeinfor);
}
/*******************************************************************************************
Description:
	decoder close by player
Parameters:
      me_decoder_t *pthis: decoder interface structor point
      void *decodeinfor:	   decode information structor create by decoder
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.12.31		create first version
********************************************************************************************/
ME_U32	me_decoder_close(me_decoder_t *pthis, void *decodeinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->me_decoder_close()"));
	#endif
	
	if (!pthis) 
  	{     //maybe have no decode of this file (such as have audio decoder but no video decoder)
    		ME_MESSAGE(("me_decoder_close( pthis ==NULL ) !"));
 		return ME_SUCCESS;
  	}
	
	ME_ASSERT_PARAM_RETURN((decodeinfor!=NULL));
	return pthis->release(decodeinfor);
}
