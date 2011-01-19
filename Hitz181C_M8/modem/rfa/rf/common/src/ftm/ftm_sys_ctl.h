#ifndef FTM_SYS_CTL_H
#define FTM_SYS_CTL_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      FTM RF MODE INITILIZATION

GENERAL DESCRIPTION
  Routines for initializing the RF chipsets when switching modes.

Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/rf/common/main/qsc1100/src/ftm/ftm_sys_ctl.h#1 $ 
  $DateTime: 2008/04/11 05:11:57 $ 
  $Author: ppottier $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/13/07   anr	   Added the ftm_set_calibration_state() and ftm_get_calibration_state() APIs
					and ftm_cal_state_type enum
07/23/05   gco     Created.
===========================================================================*/

typedef enum 
{
	CAL_STATE_INACTIVE = 0,
    CAL_STATE_ACTIVE = 1,
} ftm_cal_state_type;


/* --- operational mode */
void ftm_rfmode_1x(void);
void ftm_rfmode_gsm(void);
void ftm_rfmode_wcdma(void);
void ftm_rfmode_hdr(void);

/* --- calibration mode */
void ftm_rfmode_1x_cal(void);
void ftm_rfmode_gsm_cal(void);
void ftm_rfmode_wcdma_cal(void);

/* --- online/non-FTM */
void ftm_rfmode_exit(void);

/*-- Manage calibration state*/
void ftm_set_calibration_state(ftm_cal_state_type on_off);
ftm_cal_state_type ftm_get_calibration_state(void);

#endif /* FTM_SYS_CTL_H */



