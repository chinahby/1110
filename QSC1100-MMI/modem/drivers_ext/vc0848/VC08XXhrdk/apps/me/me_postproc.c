/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	me_postproc.c 
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2007/01/16
* Last modify : Amanda Deng 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/

#include "me_postproc.h"

/*******************************************************************************************
Description:
	enable one postproc in postlist
Parameters:
       me_node_list *pthis:	postproc list point
       char* post_id:		postproc id (4 bytes length)
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.16		create first version
********************************************************************************************/
ME_U32	me_effectproc_enable(me_node_list *pthis, char* post_id)
{
	me_node_t			*pnod	= NULL;
	me_effectproc_t		*postproc_cls	= NULL;
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->me_effectproc_enable()"));
	#endif

	ME_ASSERT_PARAM_RETURN((pthis!=NULL));
	ME_ASSERT_PARAM_RETURN((post_id!=NULL));

	pnod = pthis->first;
	while(pnod)
	{
		postproc_cls = (me_effectproc_t*)pnod->content;
		if(me_effectproc_check_id(postproc_cls,post_id))
		{
			return  postproc_cls->set_enable(postproc_cls);
		}
		pnod = pnod->next;
	}
	return ME_ERR_UNSUPPORT_POSTPRC;
}
/*******************************************************************************************
Description:
	disable one postproc in postlist
Parameters:
       me_node_list *pthis:	postproc list point
       char* post_id:		postproc id (4 bytes length)
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.01.16		create first version
********************************************************************************************/
ME_U32	me_effectproc_disable(me_node_list *pthis, char* post_id)
{
	me_node_t			*pnod	= NULL;
	me_effectproc_t		*postproc_cls	= NULL;
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->me_effectproc_disable()"));
	#endif

	ME_ASSERT_PARAM_RETURN((pthis!=NULL));
	ME_ASSERT_PARAM_RETURN((post_id!=NULL));

	pnod = pthis->first;
	while(pnod)
	{
		postproc_cls = (me_effectproc_t*)pnod->content;
		if(me_effectproc_check_id(postproc_cls,post_id))
		{
			return  postproc_cls->set_disable(postproc_cls);
		}
		pnod = pnod->next;
	}
	return ME_ERR_UNSUPPORT_POSTPRC;
}

/*******************************************************************************************
Description:
	init one postproc in postlist
Parameters:
       me_node_list *pthis:	postproc list point
       char* post_id:			postproc id (4 bytes length)
       void *param1:		audio infor if it is audio post process
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.03.20		create first version
********************************************************************************************/
ME_U32	me_effectproc_init(me_node_list *pthis, char* post_id, void *param1)
{
	me_node_t			*pnod	= NULL;
	me_effectproc_t		*postproc_cls	= NULL;
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->me_effectproc_init()"));
	#endif

	ME_ASSERT_PARAM_RETURN((pthis!=NULL));
	ME_ASSERT_PARAM_RETURN((post_id!=NULL));
	ME_ASSERT_PARAM_RETURN((param1!=NULL));
	
	pnod = pthis->first;
	while(pnod)
	{
		postproc_cls = (me_effectproc_t*)pnod->content;
		if(me_effectproc_check_id(postproc_cls,post_id))
		{
			return  postproc_cls->init(postproc_cls, param1);
		}
		
		pnod = pnod->next;
	}
	return ME_ERR_UNSUPPORT_POSTPRC;
}

/*******************************************************************************************
Description:
	init one postproc in postlist
Parameters:
       me_node_list *pthis:	postproc list point
       char* post_id:			postproc id (4 bytes length)
       void *param1:		audio infor if it is audio post process
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.03.20		create first version
********************************************************************************************/
ME_U32	me_effectproc_release(me_node_list *pthis, char* post_id)
{
	me_node_t			*pnod	= NULL;
	me_effectproc_t		*postproc_cls	= NULL;
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->me_effectproc_release()"));
	#endif

	ME_ASSERT_PARAM_RETURN((pthis!=NULL));
	ME_ASSERT_PARAM_RETURN((post_id!=NULL));
	
	pnod = pthis->first;
	while(pnod)
	{
		postproc_cls = (me_effectproc_t*)pnod->content;
		if(me_effectproc_check_id(postproc_cls,post_id))
		{
			return  postproc_cls->release(postproc_cls);
		}
		pnod = pnod->next;
	}
	return ME_ERR_UNSUPPORT_POSTPRC;
}

/*******************************************************************************************
Description:
	set parameter to one postprc
Parameters:
       me_node_list *pthis:	postproc list point
       char* post_id:			postproc id (4 bytes length)
       void *param1:		audio infor if it is audio post process
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.03.26		create first version
********************************************************************************************/
ME_U32	me_effectproc_set_param(me_node_list *pthis, char* post_id, void *param1)
{
	me_node_t			*pnod	= NULL;
	me_effectproc_t		*postproc_cls	= NULL;
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->me_effectproc_set_param()"));
	#endif

	ME_ASSERT_PARAM_RETURN((pthis!=NULL));
	ME_ASSERT_PARAM_RETURN((post_id!=NULL));
	ME_ASSERT_PARAM_RETURN((param1!=NULL));

	pnod = pthis->first;
	while(pnod)
	{
		postproc_cls = (me_effectproc_t*)pnod->content;
		if(me_effectproc_check_id(postproc_cls,post_id))
		{
			return  postproc_cls->set_param(postproc_cls, param1);
		}
		
		pnod = pnod->next;
	}
	return ME_ERR_UNSUPPORT_POSTPRC;
}

