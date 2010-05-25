/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	media_comhead.h
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/12/31
* Last modify : Amanda Deng 2007-xx-xx     
*
* version 0.1: 
*	define all include head file in this file, then other user include this one file only 
*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/

#ifndef _MEDIA_COMMON_H_
#define _MEDIA_COMMON_H_


#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************************************/
/*******************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

/*******************************************************************************************/
/*******************************************************************************************/
#ifdef _WIN32
#include "../me/me_comhead.h"
#include "../me/me_interface.h"	
#else
//#include "nucleus.h"
#include "mmd_sys_driver.h"
/*******************************************************************************************/
/*******************************************************************************************/
#include "../me/me_comhead.h"
#include "../me/me_interface.h"	
#endif
/*******************************************************************************************/
/*******************************************************************************************/
#include "media_keymacro.h"			//this file add by amanda deng
#include "media_typedef.h"				//this file add by amanda deng
#include "media_rtnflag.h"				//this file add by amanda deng
#include "media_utils.h"					//this file add by amanda deng
/*******************************************************************************************/
/*******************************************************************************************/
#include "media_player.h"				//this file add by amanda deng
//#include "interrupt_audio.h"				//this file add by amanda deng
//#include "interrupt_video.h"				//this file add by amanda deng
#include "mplayer_internal.h"
#include "mrecorder_internal.h"

/*******************************************************************************************/
/*******************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif 
