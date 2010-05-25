/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	me_id3.c
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2007/01/16
* Last modify : Amanda Deng 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/

#include "me_id3.h"

#if ME_KM_OBJ_STACK
static me_id3_t		_g_id3_unknow;
#endif

ME_U32	id3_unknow_init(void *fs_filehandle, fs_callbacks *fs_filefunc,void **id3infor)
{
#if ME_KM_FUNNAME
	ME_MESSAGE(("->id3_unknow_init()"));
#endif
	(*id3infor)=NULL;
	return ME_SUCCESS;
}

ME_U32	id3_unknow_release(void *id3infor)
{
#if ME_KM_FUNNAME
	ME_MESSAGE(("->id3_unknow_release()"));
#endif
	return ME_SUCCESS;
}

ME_U32	id3_unknow_get_property(void *id3infor, me_property*property)
{
#if ME_KM_FUNNAME
	ME_MESSAGE(("->id3_unknow_get_property()"));
#endif
	property->size = 0;
	return ME_SUCCESS;
}

ME_U32	unknow_id3_register(void)
{
#if ME_KM_PARSER_3GP_EN	
	ME_S32 i;
#if ME_KM_OBJ_STACK
	me_id3_t		* pthis	= &_g_id3_unknow;
#else
	me_id3_t 		* pthis	= (me_id3_t*)me_malloc( sizeof(me_id3_t));
	if(pthis==NULL)
	{	
		return	ME_ERR_MALLOC_MEMORY_ERROR;	
	}
#endif

	#if ME_KM_FUNNAME
	ME_MESSAGE(("->unknow_id3_register()"));
	#endif
	
	for(i = 0; i < 4; i++) pthis->id[i] = 0;
	ME_SPRINTF(pthis->id, "unkn");
	
	pthis->init					  	= id3_unknow_init;
	pthis->release				  	= id3_unknow_release;
	pthis->get_property			  	= id3_unknow_get_property;
	

	return me_id3_register(pthis);		/*×¢²áunkn Parser*/
#else
	return ME_SUCCESS;
#endif
}


/*******************************************************************************************
Description:
	according file type select parser form parser list.
Parameters:
       me_node_list *pthis:	parser list point
       char* filetype:			file type (4 bytes length)
       me_id3_t **id3: 		selected id3 for user
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.16		create first version
********************************************************************************************/
ME_U32 me_id3_select(me_node_list *pthis,char* filetype, me_id3_t **id3)
{
	me_node_t			*pnod	= NULL;
	me_id3_t				*pid3_cls	= NULL;
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->me_parser_select()"));
	#endif
	
	(*id3) = NULL;
	ME_ASSERT_PARAM_RETURN((pthis!=NULL));
	ME_ASSERT_PARAM_RETURN((filetype!=NULL));

	pnod = pthis->first;
	while(pnod)
	{
		pid3_cls = (me_id3_t*)pnod->content;
		if(me_id3_check_id(pid3_cls,filetype))
		{
			(*id3) = pid3_cls;
			return ME_SUCCESS;
		}
		pnod = pnod->next;
	}
	pnod = pthis->first;
	while(pnod)
	{
		pid3_cls = (me_id3_t*)pnod->content;
		if(me_id3_check_id(pid3_cls,"unkn"))
		{
			(*id3) = pid3_cls;
			return ME_SUCCESS;
		}
		pnod = pnod->next;
	}
	return ME_FAILE;
}

/*******************************************************************************************
Description:
	id3 initialize for player
Parameters:
       me_id3_t *pthis:		id3 interface structor point
       void *mediafile:		file handle(point)
       fs_callbacks *fs_filefunc:		file operate interface structor point
       void **fileinfor:		file information structor create by parser
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.16		create first version
********************************************************************************************/
ME_U32 me_id3_open(me_id3_t *pthis, void *fs_filehandle, fs_callbacks *fs_filefunc,void **id3infor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->me_id3_open()"));
	#endif
	
	(*id3infor)=NULL;
	ME_ASSERT_PARAM_RETURN((pthis!=NULL));
	ME_ASSERT_PARAM_RETURN((fs_filehandle!=NULL));
	ME_ASSERT_PARAM_RETURN((fs_filefunc!=NULL));
	
	return pthis->init(fs_filehandle,fs_filefunc,id3infor);
}
/*******************************************************************************************
Description:
	id3 close by player
Parameters:
      me_id3_t *pthis: 		id3 interface structor point
      void **fileinfor:		file information structor create by parser
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.16		create first version
********************************************************************************************/
ME_U32	me_id3_close(me_id3_t *pthis, void *id3infor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->me_id3_close()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((pthis!=NULL));

	return pthis->release(id3infor);
}


