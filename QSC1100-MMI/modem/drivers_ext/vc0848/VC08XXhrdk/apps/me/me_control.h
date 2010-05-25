/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	me_control.h 
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/12/31
* Last modify : Amanda Deng 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/

#ifndef _ME_CONTROL_
#define _ME_CONTROL_


#ifdef __cplusplus
extern "C" {
#endif

#include "me_comhead.h"

extern vm_media_engine 	_g_media_engine;						// media engine structor(parser,muxer,decoder,encoder list)

/**************************** standard interface for MediaEngine ****************************************/
ME_U32 	media_engine_creat(void);
ME_U32 	media_engine_destroy(void);

ME_U32 	me_parser_register(me_parser_t *parser);
ME_U32 	me_muxer_register(me_muxer_t *muxer);
ME_U32 	me_decoder_register(me_decoder_t *decoder);
ME_U32 	me_encoder_register(me_encoder_t *encoder);
ME_U32 	me_id3_register(me_id3_t *id3);

/***************************** standard interface for player ********************************************/
ME_U32 	me_player_open(me_player_infor* ply_infor, MP_FILE **mpfile);
ME_U32 	me_player_close(MP_FILE *mpfile);
ME_U32	me_audio_postproc_register(MP_FILE *mpfile, me_effectproc_t *postproc);
ME_U32	me_video_postproc_register(MP_FILE *mpfile, me_effectproc_t *postproc);
/***************************** standard interface for recorder ******************************************/
ME_U32 	me_recorder_open(me_record_infor* rec_infor, MR_FILE **recfile);
ME_U32	me_recorder_close(MR_FILE *recfile);
ME_U32	me_audio_preproc_register(MR_FILE *mrfile, me_effectproc_t *preproc);
ME_U32	me_video_preproc_register(MR_FILE *mrfile, me_effectproc_t *preproc);
/*************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif 

