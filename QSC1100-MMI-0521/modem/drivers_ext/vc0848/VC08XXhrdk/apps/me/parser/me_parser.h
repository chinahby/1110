/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	me_parser.h
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/12/31
* Last modify : Amanda Deng 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/

#ifndef _ME_PARSER_
#define _ME_PARSER_

#ifdef __cplusplus
extern "C" {
#endif
#include "..\me_comhead.h"

void me_parser_quick_select(me_node_list *pthis,char* filetype, me_parser_t **parser);
ME_U32 me_parser_check_select(me_node_list *pthis,void *mediafile, fs_callbacks *fs_filefunc, me_parser_t **parser);
ME_U32 me_parser_open(me_parser_t *pthis, void *mediafile, fs_callbacks *fs_filefunc,void **fileinfor);
ME_U32 me_parser_close(me_parser_t *pthis, void *fileinfor, me_audio_infor *audio_infor, me_video_infor *video_infor);

#ifdef __cplusplus
}
#endif

#endif //_ME_PARSER_

