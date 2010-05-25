/*======================================================================
*                                                                       
*                Copyright (C) 2007 Vimicro CO.,LTD     		 
* File Name: 	mplayer_internal.h 
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2007/04/02
* Last modify : Amanda Deng 2007-xx-xx     
*
* version 0.1: 
*	have 1 interface API for user .
*	void video_decode_interrupt_init(void);
*	void video_syn_timer_start(void);
*	MMD_U32 get_system_time_ticks(void);
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/

#ifndef _MPLAYER_INTERNAL_H_
#define _MPLAYER_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "media_comhead.h"			//this file add by amanda deng
//#define VIDEODECODE_VOPMODE 0
//#define VIDEONORMAL_VOPIN848 1
void media_player_timer_creat(void);
void media_player_timer_release(void);
void media_player_timer_start(void);
void media_player_timer_stop(void);
void media_player_jpgdis_timer_creat(void);
void media_player_jpgdis_timer_release(void);
void media_player_jpgdis_timer_start(void);
void media_player_jpgdis_timer_stop(void);
/*void media_recorder_jpgcap_timer_creat(void);
void media_recorder_jpgcap_timer_release(void);
void media_recorder_jpgcap_timer_start(void);
void media_recorder_jpgcap_timer_stop(void);*/


#ifdef __cplusplus
}
#endif

#endif /*_MPLAYER_INTERNAL_H_*/
