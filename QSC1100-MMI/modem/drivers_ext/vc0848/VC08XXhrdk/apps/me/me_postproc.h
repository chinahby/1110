/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	me_postproc.h.h
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2007/01/16
* Last modify : Amanda Deng 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/

#ifndef _ME_POST_PROCESS_
#define _ME_POST_PROCESS_

#ifdef __cplusplus
extern "C" {
#endif
#include "me_comhead.h"

ME_U32	me_effectproc_enable(me_node_list *pthis, char* post_id);
ME_U32	me_effectproc_disable(me_node_list *pthis, char* post_id);
ME_U32	me_effectproc_init(me_node_list *pthis, char* post_id, void *param1);
ME_U32	me_effectproc_release(me_node_list *pthis, char* post_id);
ME_U32	me_effectproc_set_param(me_node_list *pthis, char* post_id, void *param1);

#ifdef __cplusplus
}
#endif

#endif 
