/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	me_muxer.h
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/12/31
* Last modify : Amanda Deng 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/

#ifndef _ME_MUXER_
#define _ME_MUXER_

#ifdef __cplusplus
extern "C" {
#endif

#include "..\me_comhead.h"

ME_U32 me_muxer_select(me_node_list *pthis, char* filetype, me_muxer_t **muxer);
ME_U32 me_muxer_open(me_muxer_t *pthis,  void *fs_filehandle, fs_callbacks *fs_filefunc,void **muxinfor);
ME_U32 me_muxer_close(me_muxer_t *pthis, void *muxinfor);

#ifdef __cplusplus
}
#endif

#endif 



