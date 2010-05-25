/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	me_decoder.h
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/12/31
* Last modify : Amanda Deng 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/

#ifndef _ME_DECODER_
#define _ME_DECODER_

#ifdef __cplusplus
extern "C" {
#endif
#include "..\me_comhead.h"

ME_U32	me_decoder_select(me_node_list *pthis, char* codec_id, me_decoder_t** decoder);
ME_U32	me_decoder_open(me_decoder_t *pthis, void *srcinfor, void *outinfor, void **decodeinfor);
ME_U32	me_decoder_close(me_decoder_t *pthis, void *decodeinfor);

#ifdef __cplusplus
}
#endif

#endif 
