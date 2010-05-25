/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	encoder_samr.h
* User for : 	820
* Creat By : 	An Xinfang                                                                  
* CrateDate: 	2007/07/17
* Last modify : An Xinfang 2007-xx-xx     
*
* release history:
* 		
=======================================================================*/

#ifndef _ME_ENCODER_S263_
#define _ME_ENCODER_S263_

#ifdef __cplusplus
extern "C" {
#endif

#include "..\..\me_comhead.h"
ME_U32 encoder_s263_init(void *recinfor, void* inputinfor, void **encodeinfor);
ME_U32 encoder_s263_release(void *encodeinfor);
ME_U32 encoder_s263_set_inputparam(void *encodeinfor, void *inputinfor);
ME_U32 encoder_s263_start(void *encodeinfor);
ME_U32 encoder_s263_stop(void *encodeinfor);
ME_U32 encoder_s263_resume(void *encodeinfor);
ME_U32 encoder_s263_pause(void *encodeinfor);
ME_U32 encoder_s263_src_buffersize(void *encodeinfor, ME_U32 *maxsrcsize);
ME_U32 encoder_s263_dst_buffersize(void *encodeinfor, ME_U32 *maxdstsize);
ME_U32 encoder_s263_encodeframes(void *encodeinfor, void *src, void *dst);
ME_U32 encoder_s263_register(void);

#ifdef __cplusplus
}
#endif

#endif 
