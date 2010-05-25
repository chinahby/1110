/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	encoder_jpg.h
* User for : 	820
* Creat By : 	Zhou Jian                                                                 
* CrateDate: 	2007/11/20
* Last modify : Zhou Jian 2007-xx-xx     
*
* release history:
* 		
=======================================================================*/

#ifndef _ME_ENCODER_JPG_
#define _ME_ENCODER_JPG_

#ifdef __cplusplus
extern "C" {
#endif

#include "..\..\me_comhead.h"
#define ME_CAPTURE_TEST 0
ME_U32 encoder_jpg_init(void *recinfor, void* inputinfor, void **encodeinfor);
ME_U32 encoder_jpg_release(void *encodeinfor);
ME_U32 encoder_jpg_start(void *encodeinfor);
ME_U32 encoder_jpg_stop(void *encodeinfor);
ME_U32 encoder_jpg_resume(void *encodeinfor);
ME_U32 encoder_jpg_pause(void *encodeinfor);
ME_U32 encoder_jpg_src_buffersize(void *encodeinfor, ME_U32 *maxsrcsize);
ME_U32 encoder_jpg_dst_buffersize(void *encodeinfor, ME_U32 *maxdstsize);
ME_U32 encoder_jpg_encode_frames(void *encodeinfor, void *src, void *dst);

ME_U32	encoder_jpg_register(void);

#ifdef __cplusplus
}
#endif

#endif 
