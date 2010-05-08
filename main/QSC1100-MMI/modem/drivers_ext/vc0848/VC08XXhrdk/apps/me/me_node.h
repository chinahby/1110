/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	me_node.h
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/12/31
* Last modify : Amanda Deng 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/

#ifndef _ME_NODE_
#define _ME_NODE_


///////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif
///////////////////////////////////////////////////////////////////////////////
#include "me_comhead.h"


me_node_list	*me_list_new(ME_VOID);
ME_U32 	me_list_free(me_node_list *l);
ME_VOID	*me_list_first_content (me_node_list *l);
ME_VOID	*me_list_next_content (me_node_list *l);
ME_U32 	me_list_is_empty (me_node_list *l);
ME_U32 	me_list_append_priority_content (me_node_list *l, ME_VOID *content, ME_S32 priority);
ME_U32 	me_list_append_content (me_node_list *l, ME_VOID *content);

ME_U32 me_effectprclist_free(me_node_list *l) ;

///////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
#endif
///////////////////////////////////////////////////////////////////////////////

#endif //_ME_NODE_

