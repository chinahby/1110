/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	me_id3.h
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2007/01/16
* Last modify : Amanda Deng 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/

#ifndef _ME_ID3_PARSER_
#define _ME_ID3_PARSER_

#ifdef __cplusplus
extern "C" {
#endif
#include "..\me_comhead.h"

ME_U32	unknow_id3_register(void);

ME_U32 me_id3_select(me_node_list *pthis,char* filetype, me_id3_t **id3);
ME_U32 me_id3_open(me_id3_t *pthis, void *mediafile, fs_callbacks *fs_filefunc,void **fileinfor);
ME_U32 me_id3_close(me_id3_t *pthis, void *fileinfor);

#ifdef __cplusplus
}
#endif

#endif 

