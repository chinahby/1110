/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	me_encoder.c 
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/12/31
* Last modify : Amanda Deng 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/

#include "me_encoder.h"

/*******************************************************************************************
Description:
	according codec id select encoder form encoder list.
Parameters:
       me_node_list *pthis:		encoder list point
       char* codec_id:			encode id (4 bytes length)
       me_encoder_t** decoder: 	selected encoder for user
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.12.31		create first version
********************************************************************************************/
ME_U32	me_encoder_select(me_node_list *pthis, char* codec_id, me_encoder_t**encoder)
{
	me_node_t			*pnod	= NULL;
	me_encoder_t			*pencoder_cls	= NULL;
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->me_encoder_select()"));
	#endif
	
	(*encoder) = NULL;
	ME_ASSERT_PARAM_RETURN((pthis!=NULL));
	ME_ASSERT_PARAM_RETURN((codec_id!=NULL));
	
	pnod = pthis->first;
	while(pnod)
	{
		pencoder_cls = (me_encoder_t*)pnod->content;
		if(me_encoder_check_id(pencoder_cls,codec_id))
		{
			(*encoder) = pencoder_cls;
			return ME_SUCCESS;
		}
		pnod = pnod->next;
	}
	ME_ERROR(("ME_ERR_UNSUPPORT_ENCODER %c%c%c%c", codec_id[0],codec_id[1],codec_id[2],codec_id[3]));
	return ME_ERR_UNSUPPORT_ENCODER;
}

/*******************************************************************************************
Description:
	encoder initialize for recorder
Parameters:
       me_encoder_t *pthis:	encoder interface structor point
       void *mediafile:		file handle(point)
       fs_callbacks *fs_filefunc:		file operate interface structor point
       void **encodeinfor:	encode information structor create by encoder
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.12.31		create first version
********************************************************************************************/
ME_U32	me_encoder_open(me_encoder_t *pthis, void *recinfor, void* inputinfor, void **encodeinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->me_encoder_open()"));
	#endif
	
	*encodeinfor = NULL;
	ME_ASSERT_PARAM_RETURN((pthis!=NULL));
	ME_ASSERT_PARAM_RETURN((recinfor!=NULL));
	
	return pthis->init(recinfor, inputinfor, encodeinfor);
}
/*******************************************************************************************
Description:
	encoder close by recorder
Parameters:
      me_encoder_t *pthis: encoder interface structor point
      void *encodeinfor:	   encode information structor create by encoder
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.12.31		create first version
********************************************************************************************/
ME_U32	me_encoder_close(me_encoder_t *pthis, void *encodeinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->me_encoder_close()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((pthis!=NULL));
	ME_ASSERT_PARAM_RETURN((encodeinfor!=NULL));
	
	return pthis->release(encodeinfor);
}
