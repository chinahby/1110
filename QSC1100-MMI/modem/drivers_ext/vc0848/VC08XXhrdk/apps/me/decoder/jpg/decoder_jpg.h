/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	decoder_jpg.h
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2007/03/28
* Last modify : Amanda Deng 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/
#ifndef _ME_DECODER_JPG_
#define _ME_DECODER_JPG_

#ifdef __cplusplus
extern "C" {
#endif

#include "..\..\me_comhead.h"

ME_U32 decoder_jpg_init(void *srcinfor, void *outinfor, void **decodeinfor);
ME_U32 decoder_jpg_release(void *decodeinfor);
ME_U32 decoder_jpg_seek(void *decodeinfor, ME_U32 pos);
ME_U32 decoder_jpg_start(void *decodeinfor);
ME_U32 decoder_jpg_stop(void *decodeinfor);
ME_U32 decoder_jpg_resume(void *decodeinfor);
ME_U32 decoder_jpg_pause(void *decodeinfor);
ME_U32 decoder_jpg_dst_buffersize(void *decodeinfor, ME_U32 maxsrcsize, ME_U32 *maxdstsize);
ME_U32 decoder_jpg_decode(void *decodeinfor, char *src, ME_U32 srclen, char *dst, ME_U32 *dstlen, ME_U32*decodelen);
ME_U32 decoder_jpg_decode_frames(void *decodeinfor, void *src, void *dst);
ME_U32 decoder_jpg_setoutparam(void *decodeinfor, void *outinfor);

ME_U32 decoder_jpg_register(void);

#ifdef __cplusplus
}
#endif

#endif 
