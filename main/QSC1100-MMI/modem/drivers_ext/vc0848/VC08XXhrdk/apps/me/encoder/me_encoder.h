/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	me_encoder.h
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/12/31
* Last modify : Amanda Deng 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/

#ifndef _ME_ENCODER_
#define _ME_ENCODER_

#ifdef __cplusplus
extern "C" {
#endif

#include "..\me_comhead.h"

ME_U32	me_encoder_select(me_node_list *pthis, char* codec_id, me_encoder_t** encoder);
ME_U32	me_encoder_open(me_encoder_t *pthis, void *recinfor, void* inputinfor, void **encodeinfor);
ME_U32	me_encoder_close(me_encoder_t *pthis, void *encodeinfor);

#ifdef __cplusplus
}
#endif

#endif 
