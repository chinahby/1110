/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	me_muxer.c 
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/12/31
* Last modify : Amanda Deng 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/

#include "me_muxer.h"

/*******************************************************************************************
Description:
	according file type select muxer form muxer list.
Parameters:
       me_node_list *pthis:	muxer list point
       char* filetype:			file type (4 bytes length)
       me_muxer_t** muxer: 	selected muxer for user
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.12.31		create first version
********************************************************************************************/
ME_U32 me_muxer_select(me_node_list *pthis, char* filetype, me_muxer_t **muxer)
{
	me_node_t			*pnod	= NULL;
	me_muxer_t			*muxer_cls	= NULL;
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->me_muxer_select()"));
	#endif
	
	(*muxer) = NULL;
	ME_ASSERT_PARAM_RETURN((pthis!=NULL));
	ME_ASSERT_PARAM_RETURN((filetype!=NULL));
	
	pnod = pthis->first;
	while(pnod)
	{
		muxer_cls = (me_muxer_t*)pnod->content;
		if(me_muxer_check_id(muxer_cls,filetype))
		{
			(*muxer) = muxer_cls;
			return ME_SUCCESS;
		}
		pnod = pnod->next;
	}
	ME_ERROR(("ME_ERR_UNSUPPORT_FILETYPE %c%c%c%c", filetype[0],filetype[1],filetype[2],filetype[3]));
	return ME_ERR_UNSUPPORT_FILETYPE;
}

/*******************************************************************************************
Description:
	muxer initialize for recorder
Parameters:
       me_muxer_t *pthis:	muxer interface structor point
       void *mediafile:		file handle(point)
       fs_callbacks *fs_filefunc:		file operate interface structor point
       void **fileinfor:		file information structor create by muxer
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.12.31		create first version
********************************************************************************************/
ME_U32 me_muxer_open(me_muxer_t *pthis, void *fs_filehandle, fs_callbacks *fs_filefunc,void **muxinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->me_muxer_open()"));
	#endif
	
	(*muxinfor)=NULL;
	ME_ASSERT_PARAM_RETURN((pthis!=NULL));
	ME_ASSERT_PARAM_RETURN((fs_filehandle!=NULL));
	ME_ASSERT_PARAM_RETURN((fs_filefunc!=NULL));
	
	return pthis->init(fs_filehandle,fs_filefunc,muxinfor);
}
/*******************************************************************************************
Description:
	muxer close by recorder
Parameters:
      me_muxer_t *pthis: muxer interface structor point
      void *muxinfor: file information structor create by muxer
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.12.31		create first version
********************************************************************************************/
ME_U32	me_muxer_close(me_muxer_t *pthis, void *muxinfor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->me_muxer_close()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((pthis!=NULL));
	ME_ASSERT_PARAM_RETURN((muxinfor!=NULL));
	
	return pthis->release(muxinfor);
}

