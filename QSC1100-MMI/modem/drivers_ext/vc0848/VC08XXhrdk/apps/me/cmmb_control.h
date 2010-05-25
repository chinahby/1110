/*======================================================================
*                                                                       
*                Copyright (C) 2008 Vimicro CO.,LTD     		 
* File Name: 	cmmb_control.h 
* User for : 	820
* Creat By : 	anton liu                                                               
* CrateDate: 	2008/09/02
* Last modify : anton liu 2008-xx-xx     
*
* release history:
* 		version 0.1 release by anton liu
=======================================================================*/

#ifndef _CMMB_CONTROL_
#define _CMMB_CONTROL_

#include "mmp_rdk.h"

#ifdef __cplusplus
extern "C" {
#endif


enum {
	CMMB_CTL_NOERR 				= 0, 	/* operation successful*/
	CMMB_CTL_DOWNCORE_ERROR,
	CMMB_CTL_FILE_ERROR,
	CMMB_CTL_FILE_END,
	CMMB_CTL_MALLOC_ERROR
};


unsigned int ctl_CMMB_init(CMMB_INFO * pcmmb_info, unsigned int play_mode, unsigned int fw_option);
unsigned int ctl_CMMB_release(unsigned int play_mode);
unsigned int ctl_CMMB_file_open(CMMB_FILE* pcmmb_file, CMMB_INFO * pcmmb_info);
unsigned int ctl_CMMB_file_close(CMMB_FILE* pcmmb_file);
unsigned int ctl_CMMB_file_play(CMMB_FILE* pcmmb_file, CMMB_INFO* pcmmb_info);
unsigned int ctl_CMMB_file_stop(void);


#ifdef __cplusplus
}
#endif

#endif 

