/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	me_comhead.h
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2007/01/26
* Last modify : Amanda Deng 2007-xx-xx     
*
* version 0.1: 
*	define all include head file in this file, then other user include this one file only 
*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/

#ifndef _ME_INTERFACE_HEAD_
#define _ME_INTERFACE_HEAD_


#ifdef __cplusplus
extern "C" {
#endif

#include ".\parser\me_parser.h"
#include ".\parser\3gp\parser_3gp.h"
#include ".\parser\mp4\parser_mp4.h"
#include ".\parser\jpg\parser_jpg.h"
#include ".\parser\jpg\jpg_demux.h"
#include ".\parser\wav\parser_wav.h"
#include ".\parser\aac\parser_aac.h"
#include ".\parser\amr\parser_amr.h"
#include ".\parser\mp3\parser_mp3.h"
#include ".\parser\mid\parser_mid.h"
#if 0
#include ".\parser\aac\parser_aac.h"
#include ".\parser\amr\parser_amr.h"
#include ".\parser\mp3\parser_mp3.h"
#include ".\parser\avi\parser_avi.h"
#include ".\parser\flac\parser_flac.h"
#include ".\parser\ape\parser_ape.h"
#include ".\parser\rm\parser_rm.h"
#include ".\parser\ogg\parser_ogg.h"
#include ".\parser\wma\parser_wma.h"
#include ".\parser\wav\parser_wav.h"
#include ".\parser\gif\parser_gif.h"
#endif

#include ".\muxer\me_muxer.h"
#include ".\muxer\jpg\muxer_jpg.h"
#include ".\muxer\mp4\muxer_mp4.h"
#include ".\muxer\3gp\muxer_3gp.h"
#include ".\muxer\amr\muxer_amr.h"
#include ".\muxer\mp3\muxer_mp3.h"

#if 0
#include ".\muxer\3gp\muxer_3gp.h"
#include ".\muxer\amr\muxer_amr.h"
#endif

#include  ".\decoder\me_decoder.h"
#include  ".\decoder\s263\decoder_s263.h"
#include  ".\decoder\mp4v\decoder_mp4v.h"
#include  ".\decoder\jpg\decoder_jpg.h"

#if 0
#include  ".\decoder\samr\decoder_samr.h"
#include  ".\decoder\mp4a\decoder_mp4a.h"
#include  ".\decoder\aac\decoder_aac.h"
#include  ".\decoder\amr\decoder_amr.h"
#include  ".\decoder\mp3\decoder_mp3.h"
#include  ".\decoder\flac\decoder_flac.h"
#include  ".\decoder\ape\decoder_ape.h"
#include  ".\decoder\pcm\decoder_pcm.h"
#include  ".\decoder\ra\decoder_ra.h"
#include  ".\decoder\rv\decoder_rv.h"
#include ".\decoder\ogg\decoder_ogg.h"
#include ".\decoder\wma\decoder_wma.h"
#include  ".\decoder\sjpg\decoder_sjpg.h"
#include  ".\decoder\gif\decoder_gif.h"
#endif

#include  ".\encoder\me_encoder.h"
#include  ".\encoder\mp4v\encoder_mp4v.h"
#include  ".\encoder\jpg\encoder_jpg.h"
#include  ".\encoder\s263\encoder_s263.h"
#if 0
#include  ".\encoder\s263\encoder_s263.h"
#include  ".\encoder\samr\encoder_samr.h"
#include  ".\encoder\amr\encoder_amr.h"

#include ".\encoder\mp3\encoder_mp3.h"
#endif

#include "me_postproc.h"
#if 0
#include  ".\postprocess\EQ\postproc_EQ.h"
#include  ".\postprocess\SRC\postproc_SRC.h"
#include  ".\postprocess\3DSound\postproc_3D.h"
#include  ".\postprocess\Slowplay\postproc_Slowplay.h"
#include  ".\postprocess\mir\postproc_mir.h"

#include  ".\preprocess\DigitalAGC\preproc_agc.h"
#include  ".\preprocess\OneMicANC\preproc_onc.h"
#endif	

#include ".\id3\me_id3.h"
#if 0
#include ".\id3\mp3_id3.h"
#endif

#ifdef __cplusplus
}
#endif

#endif 
