/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*



                            FTM PMIC   

GENERAL DESCRIPTION
  This is the header file for the embedded FTM PMIC commands comming from the diag service 

Copyright (c) 1994, 1995, 1996 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1997, 1998, 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2000, 2001, 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2003, 2004, 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2006, 2007       by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/pmic/common/main/latest/ftm/inc/ftm_pmic.h#3 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/29/07   apu     Added a command to manipulate the fuse-register.
07/26/07   apu	   Added a command index "FTM_PMIC_GENERIC_RW = 14" for peek-poke functionality.		
06/15/07   cng     Added pm_mpp_aout_level_type to ftm_pmic_mpp_analog_out_cmd_type
05/18/07   hs      Updated ftm_pmic_mpp_digital_in_cmd_type.
04/26/07   ab      Removed et vreg enum ftm api since we are getting errors on string fns. 
                     It will be re added again once we figure out the issue
04/20/07   ab      Added ftm_pmic_speaker_HL_mux_cntl2_cmd_type for speaker High level mux control
10/11/06   jf      Added FEATURE_FTM_PMIC_BYPASS
08/04/06   ab      Added get vreg value ftm api, get vreg enum ftm api 
                     and get pmic model ftm api support
05/31/06   hs      Added mpp FTM support.
05/24/06   hs      Added feature to include header file "ftmdiag.h"
01/04/06   vk      Added speaker low level & high level configuration support
                   Added speaker, video amplifier, mic control feature
09/02/05   cng     Removed legacy code
07/01/05   cng     Added the DVT feature 
10/27/04   rmd     Changed the naming conversion of the pulse charger code 
                   to reflect that the PMIC pulse charger API supports  
                   applies for both wall and usb chargers.
10/12/04   jac     Mainlined ASYNC packet code.
4/26/04    dp      Created
===========================================================================*/

#ifndef FTM_PMIC_H
#define FTM_PMIC_H


#ifdef FEATURE_FACTORY_TESTMODE
#ifdef FEATURE_PMIC
#ifndef FEATURE_FTM_PMIC_BYPASS

#include "comdef.h"
#include "target.h"
#include "ftm.h"
#ifndef T_QSC60X0
#include "ftmdiag.h"
#endif
#include "diagpkt.h"

#include "pm.h"
#include "pm66XXvregs.h"


/* pmic request/response type */
/* Response type is same as request type for "get" type requests */
/* such as "Get RTC time" */

typedef PACKED struct
{

  diagpkt_subsys_header_type   header;
  ftm_cmd_header_type          ftm_cmd_header;
  void                         *pmic_params;

} ftm_pmic_pkt_type;


typedef PACKED struct
{
    uint16                     rsp_id;
    uint16                     rsp_cid;
    uint16                     rsp_scid;

} ftm_pmic_rsp_header_type;


/* Used when response to FTM PMIC command requires sending back */
/* command status info */

typedef PACKED struct
{

    diagpkt_subsys_header_type   header;
    ftm_pmic_rsp_header_type     ftm_rsp_header;
    uint16                       status;

} ftm_pmic_gen_rsp_pkt_type;


/* Response pkt for get_vreg_value API*/
typedef PACKED struct
{
    diagpkt_subsys_header_type   header;
    ftm_pmic_rsp_header_type     ftm_rsp_header;
    uint16                       level;
    uint16                       status;
} ftm_pmic_get_value_rsp_pkt_type;




ftm_rsp_pkt_type ftm_pmic_dispatch( ftm_pmic_pkt_type *cmd_ptr);



/* this type specifies the command functional block of the PMIC test to 
 * be performed 
 */
 
typedef enum
{
    
    FTM_PMIC_TEST_RTC       = 0,
    FTM_PMIC_TEST_CHG       = 1,
    FTM_PMIC_TEST_USB       = 2,
    FTM_PMIC_TEST_AMUX      = 3, 
    FTM_PMIC_TEST_VREG      = 4,
    FTM_PMIC_TEST_INT       = 5,
    FTM_PMIC_TEST_UI        = 6,
    FTM_PMIC_TEST_DVT       = 7,
    FTM_PMIC_TEST_SPKR      = 8,
    FTM_PMIC_TEST_VID       = 9,
    FTM_PMIC_TEST_MIC       = 10,
    FTM_PMIC_TEST_RESET     = 11,
    FTM_PMIC_TEST_MPP       = 12,
    FTM_PMIC_TEST_GEN       = 13,
    FTM_PMIC_FUSE_CMDS      = 14,	
    FTM_PMIC_TEST_VREG_EXT  = 15 /* extended VREG commands */
} ftm_pmic_cmd_id_type;


/* This type specifies the response ids for the FTM PMIC commands */

typedef enum
{
    
    FTM_PMIC_GEN_RESPONSE      = 0x1000

} ftm_pmic_rsp_id_type;



/* This type specifies the RTC commands for the PMIC */

typedef enum
{
    
    FTM_PMIC_RTC_SET_GET_TIME           = 0,
    FTM_PMIC_RTC_SET_GET_ALARM_TIME     = 1,
    FTM_PMIC_RTC_STOP                   = 2,
    FTM_PMIC_RTC_GET_ALARM_STATUS       = 3,
    FTM_PMIC_RTC_DISABLE_ALARM          = 4,
    FTM_PMIC_RTC_SET_GET_TIME_ADJUST    = 5

} ftm_pmic_rtc_cmd_id_type;


typedef enum
{
    FTM_PMIC_GET_MODEL             = 0 /*used for get_pmic_model API */

    /* Add common APIs here .... */

} ftm_pmic_gen_cmd_id_type;



/* This type specifies the VREG commands for the PMIC */

typedef enum
{

    FTM_PMIC_VREG_CONTROL             = 0,
    FTM_PMIC_VREG_SET_LEVEL           = 1,
    FTM_PMIC_VREG_CONFIG_SMPS_MODE    = 2,
    FTM_PMIC_VREG_CONFIG_LDO_PD_SW    = 3,
    FTM_PMIC_VREG_LP_MODE_CONTROL     = 4,
    FTM_PMIC_VREG_SMPS_CLK_CONFIG     = 5,
    FTM_PMIC_VREG_GET_LEVEL		      = 6,
/*    FTM_PMIC_VREG_GET_ENUM		  = 7 */
/* This is the last enum and the size cannot exceed 7 */
} ftm_pmic_vreg_cmd_id_type;


/* This type specifies the Charger commands for the PMIC */

typedef enum
{
    FTM_PMIC_CHG_SET_SWITCH_STATE    = 0,
    FTM_PMIC_CHG_SET_CONFIG          = 1

} ftm_pmic_chg_cmd_id_type;

/* This type specifies the Analog Mux (AMUX) commands for the PMIC */

typedef enum
{

    FTM_PMIC_AMUX_CONFIG             = 0

} ftm_pmic_amux_cmd_id_type;



/* This type specifies the Int commands for the PMIC */

typedef enum
{
    FTM_PMIC_INT_GET_RT_STATUS    = 0,
    FTM_PMIC_INT_CLR_IRQ          = 1

} ftm_pmic_int_cmd_id_type;


/* This type specifies the UI commands for the PMIC */

typedef enum
{
    FTM_PMIC_UI_SET_DRIVE_LEVEL    = 0,
    FTM_PMIC_UI_CONFIG_SPKR        = 1
} ftm_pmic_ui_cmd_id_type;


/* This type specifies the MEM commands for the PMIC */
typedef enum
{
    FTM_PMIC_MEM_READ     = 0,
    FTM_PMIC_MEM_WRITE    = 1
} ftm_pmic_mem_cmd_id_type;

/* This type specifies the charger switch types available to control */

typedef enum
{

    FTM_PMIC_CHG_SWITCH_WALL_CHGR            = 0,
    FTM_PMIC_CHG_SWITCH_BATTERY_TRANSISTOR   = 1,
    FTM_PMIC_CHG_SWITCH_WALL_PULSE_CHGR      = 2,
    FTM_PMIC_CHG_SWITCH_VCP                  = 3,
    FTM_PMIC_CHG_SWITCH_BATT_ALARM           = 4,
    FTM_PMIC_CHG_SWITCH_COIN_CELL            = 5,
    FTM_PMIC_CHG_SWITCH_USB_CHGR             = 6,
    FTM_PMIC_CHG_SWITCH_CHG_APP              = 7,      /* Charger Application task */
    FTM_PMIC_CHG_SWITCH_NUM_SWITCHES        

} ftm_pmic_chg_switch_type;


/* This type specifies the charger type available for configuration */

typedef enum
{
    
    FTM_PMIC_CHG_CONFIG_PULSE_CHGR                 = 0,
    FTM_PMIC_CHG_CONFIG_PULSE_CHGR_VBAT_DET        = 1,
    FTM_PMIC_CHG_CONFIG_WALL_CHGR_TRANISTOR_LIMITS = 2,
    FTM_PMIC_CHG_CONFIG_TRICKLE_CHARGER            = 3,
    FTM_PMIC_CHG_CONFIG_COIN_CELL_CHGR             = 4,
    FTM_PMIC_CHG_CONFIG_BATT_ALARM                 = 5,
    FTM_PMIC_CHG_CONFIG_USB_TRANSISTOR_LIMITS      = 6,
    FTM_PMIC_CHG_CONFIG_PWR_CTRL_LIMIT             = 7

} ftm_pmic_chg_set_config_type;

/* This type specifies the charger types available for power control limiting configuration */

typedef enum
{

    FTM_PMIC_CHG_PWR_CTRL_LIMIT_WALL = 0,
    FTM_PMIC_CHG_PWR_CTRL_LIMIT_USB  = 1

} ftm_pmic_chg_pwr_ctrl_limit_select_type;

/* This type spcifies the time format for the RTC commands */

typedef enum
{

    FTM_PMIC_RTC_MODE_12_HOUR_AM  = 0,
    FTM_PMIC_RTC_MODE_12_HOUR_PM  = 1,
    FTM_PMIC_RTC_MODE_24_HOUR     = 2

} ftm_pmic_rtc_mode_type;

/* This type specifies the SMPS Clock configuration commands available */

typedef enum
{

    FTM_PM_VREG_SMPS_CLK_SEL_CMD       = 0,
    FTM_PM_VREG_SMPS_CLK_TCXO_DIV_CMD  = 1,
    FTM_PM_VREG_SMPS_CLK_INVALID_CMD

} ftm_pmic_vreg_smps_config_clk_cmd;

/* This type specifies the Pins available for high drive mode */

typedef enum
{

    FTM_PMIC_UI_DRV_SEL_LCD = 0,
    FTM_PMIC_UI_DRV_SEL_KPD = 1,
    FTM_PMIC_UI_DRV_SEL_VIB = 2

} ftm_pmic_ui_high_drive_pin_id_type;

	
/* This type specifies the SPKR commands for the PMIC */
	
typedef enum
{
    FTM_PMIC_SPKR_SET_GET_HPF_CORNER_FREQ  = 0, /* set/get speaker frequency*/
	FTM_PMIC_SPKR_LL_CONFIG                = 1, /* set/get speaker low level configuration*/
	FTM_PMIC_SPKR_HL_CONFIG                = 2, /*set different High level speaker config modes*/
	FTM_PMIC_SPKR_LEFT_RIGHT_CONFIG        = 3, /*set/get left/right speaker*/
    FTM_PMIC_SPKR_OUT_OF_RANGE             /* Not valid */

} ftm_pmic_spkr_cmd_id_type;
	
/* This type specifies the VIDEO AMPLIFIER commands for the PMIC */
	
typedef enum
{
    FTM_PMIC_VID_DISABLE            = 0, /*disable video amplifier*/
	FTM_PMIC_VID_ENABLE             = 1, /*enable video amplifier*/
	FTM_PMIC_VID_GET_STATUS         = 2  /*get enable/diable staus of video amplifier*/
	   
} ftm_pmic_vid_cmd_id_type;
	
/* This type specifies the MIC commands for the PMIC */
typedef enum
{
	
    FTM_PMIC_MIC_DISABLE            = 0, /*disable mic*/
	FTM_PMIC_MIC_ENABLE             = 1, /*enable mic*/
	FTM_PMIC_MIC_GET_STATUS         = 2, /*get enable/disable mic status*/
    FTM_PMIC_MIC_SET_LEVEL          = 3, /*set mic voltage level*/
	FTM_PMIC_MIC_GET_LEVEL          = 4  /*get mic voltage*/
	   
} ftm_pmic_mic_cmd_id_type;

/* This type specifies the MPP commands for the PMIC */
typedef enum
{
	FTM_PMIC_MPP_CONFIG_DIGITAL_IN      = 0, /*configure selected mpp be digital input*/
    FTM_PMIC_MPP_CONFIG_DIGITAL_OUT     = 1, /*configure selected mpp be digital output*/
    FTM_PMIC_MPP_CONFIG_DIGITAL_INOUT   = 2, /*configure selected mpp be digital in and output*/
    FTM_PMIC_MPP_CONFIG_ANALOG_IN       = 3, /*configure selected mpp be analog input*/
    FTM_PMIC_MPP_CONFIG_ANALOG_OUT      = 4, /*configure selected mpp be analog output*/
    FTM_PMIC_MPP_CONFIG_I_SINK          = 5  /*configure selected mpp be to a current sink*/
	   
} ftm_pmic_mpp_cmd_id_type;

/* This type specifies the RESET commands for the PMIC */
typedef enum
{
	
    FTM_PMIC_HARD_RESET            = 0,
    FTM_PMIC_RESET_OUT_OF_RANGE    /* Not valid */
	   
} ftm_pmic_reset_cmd_id_type;

/* This type specifies the SPEAKER lower level commands for the PMIC */
typedef enum
{
	
    FTM_PMIC_SPKR_SINK_SET        = 0, /*bit 0*/
    FTM_PMIC_SPKR_SINK_GET        = 1,
	FTM_PMIC_SPKR_FILTERDIS_SET   = 2, /*bit 1*/
	FTM_PMIC_SPKR_FILTERDIS_GET   = 3,
	FTM_PMIC_SPKR_BYPASS_SET      = 4, /*bit 2*/
	FTM_PMIC_SPKR_BYPASS_GET      = 5,
	FTM_PMIC_SPKR_USB_SET         = 6, /*bit 3*/
	FTM_PMIC_SPKR_USB_GET         = 7,
	FTM_PMIC_SPKR_STEREO_SET      = 8, /*bit 4*/
	FTM_PMIC_SPKR_STEREO_GET      = 9,
	FTM_PMIC_SPKR_EN_L_R_SET      = 10, /*bit 5*/
	FTM_PMIC_SPKR_EN_L_R_GET      = 11,
	FTM_PMIC_SPKR_EN_L_SET        = 12, /*bit 6*/
	FTM_PMIC_SPKR_EN_L_GET        = 13,
	FTM_PMIC_SPKR_EN_R_SET        = 14, /*bit 7*/
	FTM_PMIC_SPKR_EN_R_GET        = 15,
    FTM_PMIC_SPKR_CMD_OUT_OF_RANGE    /* Not valid */
	   
} ftm_pmic_speaker_LL_config_cmd_id_type;

/* This type specifies the SPEAKER mode higher level commands for the PMIC */

typedef enum
{
	
    FTM_PMIC_SPKR_STEREO_MODE_SET        	= 0, //STEREO
	FTM_PMIC_SPKR_STEREO_MODE_GET        	= 1,
	FTM_PMIC_SPKR_LEFT_MONO_SET   	        = 2, //LEFT_MONO
	FTM_PMIC_SPKR_LEFT_MONO_GET   	        = 3,
	FTM_PMIC_SPKR_RIGHT_MONO_SET            = 4, //RIGHT_MONO
	FTM_PMIC_SPKR_RIGHT_MONO_GET            = 5,
	FTM_PMIC_SPKR_DUAL_MONO_SET         	= 6, //DUAL_MONO
	FTM_PMIC_SPKR_DUAL_MONO_GET         	= 7,
	FTM_PMIC_SPKR_LEFT_8ohm_RECEIVER_SET    = 8, //LEFT_8ohm_RECEIVER
	FTM_PMIC_SPKR_LEFT_8ohm_RECEIVER_GET    = 9,
	FTM_PMIC_SPKR_RIGHT_8ohm_RECEIVER_SET   = 10, //RIGHT_8ohm_RECEIVER
	FTM_PMIC_SPKR_RIGHT_8ohm_RECEIVER_GET   = 11,
	FTM_PMIC_SPKR_SUMMED_MONO_LEFT_SET      = 12, //SUMMED_MONO_LEFT
	FTM_PMIC_SPKR_SUMMED_MONO_LEFT_GET      = 13,
	FTM_PMIC_SPKR_SUMMED_MONO_RIGHT_SET     = 14, //SUMMED_MONO_RIGHT
	FTM_PMIC_SPKR_SUMMED_MONO_RIGHT_GET     = 15,
	FTM_PMIC_SPKR_USB_CARKIT_SET            = 16, //USB_CARKIT
	FTM_PMIC_SPKR_USB_CARKIT_GET            = 17,
    FTM_PMIC_SPKR_CONFIG_OUT_OF_RANGE      // Not valid 
	   
} ftm_pmic_speaker_HL_config_cmd_id_type;

	

/*===========================================================================
                     RTC DEFINITIONS
===========================================================================*/

#if defined( FEATURE_PMIC_RTC)
typedef PACKED struct
{

    diagpkt_subsys_header_type             header;
  
    ftm_cmd_header_type                    ftm_cmd_header;

    ftm_pmic_rtc_cmd_id_type               rtc_cmd_id;


} ftm_pmic_rtc_header_type;


/* This type specifies the interface for the set time rtc cmd */

typedef PACKED struct
{
 
    byte      ftm_rtc_month;
    byte      ftm_rtc_day;
    word      ftm_rtc_year;
    byte      ftm_rtc_hour;
    byte      ftm_rtc_min;
    byte      ftm_rtc_sec;

} ftm_pmic_rtc_set_time_cmd_type;


/* This type specifies the interface for the set time rtc cmd */

typedef PACKED struct
{

    diagpkt_subsys_header_type             header;
  
    ftm_cmd_header_type                    ftm_cmd_header;

    ftm_pmic_rtc_cmd_id_type               rtc_cmd_id;

    pm_rtc_cmd_type                        ftm_rtc_rw;

    ftm_pmic_rtc_mode_type                 time_mode;
    
    ftm_pmic_rtc_set_time_cmd_type         rtc_time;

} ftm_pmic_rtc_set_get_time_cmd_type;


/* This type specifies the interface for the set alarm time rtc cmd */

typedef PACKED struct
{

    diagpkt_subsys_header_type             header;
  
    ftm_cmd_header_type                    ftm_cmd_header;

    ftm_pmic_rtc_cmd_id_type               rtc_cmd_id;

    pm_rtc_cmd_type                        ftm_rtc_rw;

    pm_rtc_alarm_type                      alarm_id;

    ftm_pmic_rtc_mode_type                 time_mode;
    
    ftm_pmic_rtc_set_time_cmd_type         alarm_time;

} ftm_pmic_rtc_set_get_alarm_time_cmd_type;


/* This type specifies the interface for the stop rtc cmd */

typedef PACKED struct
{

    diagpkt_subsys_header_type             header;
  
    ftm_cmd_header_type                    ftm_cmd_header;

    ftm_pmic_rtc_cmd_id_type               rtc_cmd_id;

} ftm_pmic_rtc_stop_cmd_type;


/* This type specifies the interface for the get alarm status cmd */

typedef PACKED struct
{

    diagpkt_subsys_header_type             header;
  
    ftm_cmd_header_type                    ftm_cmd_header;

    ftm_pmic_rtc_cmd_id_type               rtc_cmd_id;

    uint8                                  alarm_status;
   
} ftm_pmic_rtc_get_alarm_status_cmd_type;


/* This type specifies the interface for the disable alarm cmd */

typedef PACKED struct
{
    
    diagpkt_subsys_header_type             header;
  
    ftm_cmd_header_type                    ftm_cmd_header;

    ftm_pmic_rtc_cmd_id_type               rtc_cmd_id;

    pm_rtc_alarm_type                      alarm_id;

} ftm_pmic_rtc_disable_alarm_cmd_type;



/* this type specifies the interface for the rtc set/get time adjust cmd */

typedef PACKED struct
{


    diagpkt_subsys_header_type             header;
  
    ftm_cmd_header_type                    ftm_cmd_header;

    ftm_pmic_rtc_cmd_id_type               rtc_cmd_id;

    pm_rtc_cmd_type                        ftm_rtc_rw;

    uint8                                  time_adjust;

} ftm_pmic_rtc_set_get_time_adjust_cmd_type;


#endif /* FEATURE_PMIC_RTC */

/*===========================================================================
                     CHARGER DEFINITIONS
===========================================================================*/






typedef pm_err_flag_type (*ftm_pmic_chg_switch_func_type)(pm_switch_cmd_type);


typedef PACKED struct
{

    diagpkt_subsys_header_type                  header;

    ftm_cmd_header_type                         ftm_cmd_header;

    ftm_pmic_chg_cmd_id_type                    chg_cmd_id;

} ftm_pmic_chg_header_type;


typedef PACKED struct
{

    diagpkt_subsys_header_type                  header;

    ftm_cmd_header_type                         ftm_cmd_header;

    ftm_pmic_chg_cmd_id_type                    chg_cmd_id;

    ftm_pmic_chg_set_config_type                chg_config_type;

} ftm_pmic_chg_set_config_header_type;



/* This type specifies the charger set switch command stucture */

typedef PACKED struct
{

    diagpkt_subsys_header_type                  header;

    ftm_cmd_header_type                         ftm_cmd_header;

    ftm_pmic_chg_cmd_id_type                    chg_cmd_id;

    ftm_pmic_chg_switch_type                    chg_switch_type;

    pm_switch_cmd_type                          chg_switch_cmd;

} ftm_pmic_chg_set_switch_cmd_type;





/* This type specifies the interface for the Wall Pulse charger config command */

typedef PACKED struct
{

    diagpkt_subsys_header_type                  header;

    ftm_cmd_header_type                         ftm_cmd_header;

    ftm_pmic_chg_cmd_id_type                    chg_cmd_id;

    ftm_pmic_chg_set_config_type                chg_config_type;

    pm_pulse_chg_q_side_type                    p_type;
   
    pm_pulse_chg_ton_type                       t_on;
   
    pm_pulse_chg_toff_type                      t_off;
    
    pm_pulse_chg_tdone_type                     t_done;

} ftm_pmic_chg_pulse_chg_config_type;



/* This type specifies the interface for the VBATT detect voltage config command */

/* This type specifies the vbatt det voltage */
typedef uint16 ftm_pmic_chg_vbatt_det_vlt_type;


typedef PACKED struct
{

    diagpkt_subsys_header_type                  header;

    ftm_cmd_header_type                         ftm_cmd_header;

    ftm_pmic_chg_cmd_id_type                    chg_cmd_id;

    ftm_pmic_chg_set_config_type                chg_config_type;

    ftm_pmic_chg_vbatt_det_vlt_type             vbatt_det_vlt;

} ftm_pmic_chg_wchg_vbatt_det_config_type;


/* This type specifies the interface for the Wall Charger Transistor Limits config command */



typedef PACKED struct
{

    diagpkt_subsys_header_type                  header;

    ftm_cmd_header_type                         ftm_cmd_header;

    ftm_pmic_chg_cmd_id_type                    chg_cmd_id;

    ftm_pmic_chg_set_config_type                chg_config_type;
    
    pm_chg_wchg_q_imax_type                     wchg_q_imax;
    
    pm_chg_q_vmax_type                          wchg_q_vmax;

} ftm_pmic_chg_wchg_transistor_lim_config_type; 



/* This type specifies the interface for the Trickle Charger config command */

typedef PACKED struct
{

    diagpkt_subsys_header_type                  header;

    ftm_cmd_header_type                         ftm_cmd_header;

    ftm_pmic_chg_cmd_id_type                    chg_cmd_id;

    ftm_pmic_chg_set_config_type                chg_config_type;

    pm_chg_tr_imax_type                         tr_imax;

} ftm_pmic_chg_tr_imax_type;


/* This type specifies the interface for the Coin Cell Charger config command */

typedef PACKED struct
{

    diagpkt_subsys_header_type                  header;

    ftm_cmd_header_type                         ftm_cmd_header;

    ftm_pmic_chg_cmd_id_type                    chg_cmd_id;

    ftm_pmic_chg_set_config_type                chg_config_type;

    pm_coin_cell_vset_type                      coin_cell_vset;

    pm_coin_cell_rset_type                      coin_cell_rset;

} ftm_pmic_chg_coin_cell_config_type;


/* This type specifies the interface for the Battery Alarm Config command */

/* This type specifies the battery alarm voltage */

typedef uint16 ftm_pmic_chg_batt_alarm_vlt_type;

typedef PACKED struct
{

    diagpkt_subsys_header_type                  header;

    ftm_cmd_header_type                         ftm_cmd_header;

    ftm_pmic_chg_cmd_id_type                    chg_cmd_id;

    ftm_pmic_chg_set_config_type                chg_config_type;

    ftm_pmic_chg_batt_alarm_vlt_type            upper_lim;

    ftm_pmic_chg_batt_alarm_vlt_type            lower_lim;

    pm_chg_batt_alarm_hyst_type                 batt_alarm_hyst;


} ftm_pmic_chg_batt_alarm_config_type;


/* This type specifies the interface for the USB Charger Transistor Limit Config command */

typedef PACKED struct
{

    diagpkt_subsys_header_type                  header;

    ftm_cmd_header_type                         ftm_cmd_header;

    ftm_pmic_chg_cmd_id_type                    chg_cmd_id;

    ftm_pmic_chg_set_config_type                chg_config_type;

    pm_chg_uchg_q_imax_type                     usb_trans_lim_config;

} ftm_pmic_chg_uchg_q_imax_config_type;


/* This type specifies the interface for the Charger Power control limit config command */

typedef PACKED struct
{

    diagpkt_subsys_header_type                  header;

    ftm_cmd_header_type                         ftm_cmd_header;

    ftm_pmic_chg_cmd_id_type                    chg_cmd_id;

    ftm_pmic_chg_set_config_type                chg_config_type;

    ftm_pmic_chg_pwr_ctrl_limit_select_type     select;

    pm_chg_pmaxsel_type                         pmax;

} ftm_pmic_chg_pwr_ctrl_limit_config_type;





/*===========================================================================
                      AMUX DEFINITIONS
===========================================================================*/

typedef PACKED struct
{

    diagpkt_subsys_header_type                  header;

    ftm_cmd_header_type                         ftm_cmd_header;

    ftm_pmic_amux_cmd_id_type                   amux_cmd_id;
    
} ftm_pmic_amux_header_type;


/* This type specifies the interface for the Configure Analog Mux command */

typedef PACKED struct
{

    diagpkt_subsys_header_type                  header;

    ftm_cmd_header_type                         ftm_cmd_header;

    ftm_pmic_amux_cmd_id_type                   amux_cmd_id;

    pm_adc_muxsel_type                          mux_sel;

    pm_adc_muxscale_type                        mux_scale;

} ftm_pmic_amux_config_type;


/*===========================================================================
                      VREG DEFINITIONS
===========================================================================*/

typedef PACKED struct
{

    diagpkt_subsys_header_type             header;
  
    ftm_cmd_header_type                    ftm_cmd_header;

    ftm_pmic_vreg_cmd_id_type              vreg_cmd_id;


} ftm_pmic_vreg_header_type;


/* Header type for general pmic ftm api calls */
typedef PACKED struct
{
    diagpkt_subsys_header_type             header;
  
    ftm_cmd_header_type                    ftm_cmd_header;

    ftm_pmic_gen_cmd_id_type               gen_cmd_id;

} ftm_pmic_gen_header_type;


/* This type specifies the Vreg control command packet */
typedef PACKED struct
{

    diagpkt_subsys_header_type                  header;

    ftm_cmd_header_type                         ftm_cmd_header;

    ftm_pmic_vreg_cmd_id_type                   vreg_cmd_id;

    pm_switch_cmd_type                          vreg_cmd;

    pm_vreg_masked_type                         vregs;

} ftm_pmic_vreg_control_cmd_type;


/* This type specifies the extended VREG control command packet */
typedef PACKED struct
{

    diagpkt_subsys_header_type                  header ;

    ftm_cmd_header_type                         ftm_cmd_header ;

    ftm_pmic_vreg_cmd_id_type                   vreg_cmd_id ;

    pm_switch_cmd_type                          vreg_cmd ;

    uint16                                      vregs ;

} ftm_pmic_vreg_control_ext_cmd_type;



/* This type specifies the interface for the set voltage level command */


/* This type specifies the pmic voltage regulator level (in mV) */
typedef uint16 ftm_pmic_vreg_level;

typedef PACKED struct
{

    diagpkt_subsys_header_type                  header;

    ftm_cmd_header_type                         ftm_cmd_header;

    ftm_pmic_vreg_cmd_id_type                   vreg_cmd_id;

    pm_vreg_id_type                             vreg;
   
    ftm_pmic_vreg_level                         level;

} ftm_pmic_vreg_set_level_cmd_type;


/* Request packet for for get vreg level API */
typedef PACKED struct
{
    diagpkt_subsys_header_type                  header;
    ftm_cmd_header_type                         ftm_cmd_header;
    ftm_pmic_vreg_cmd_id_type                   vreg_cmd_id;
    pm_vreg_id_type                             vreg;
    ftm_pmic_vreg_level                         level; 
 } ftm_pmic_vreg_get_level_cmd_type;



/* This type specifies the interface for the config SMPS mode command */
typedef PACKED struct
{
    diagpkt_subsys_header_type                  header;

    ftm_cmd_header_type                         ftm_cmd_header;

    ftm_pmic_vreg_cmd_id_type                   vreg_cmd_id;

    pm_vreg_smps_id_type                        vreg_smps;
    
    pm_vreg_smps_mode_type                      mode;

} ftm_pmic_vreg_config_smps_mode_cmd_type;


/* This type specifies the interface for the LDO pulldown switch config command */

typedef PACKED struct
{

    diagpkt_subsys_header_type                  header;

    ftm_cmd_header_type                         ftm_cmd_header;

    ftm_pmic_vreg_cmd_id_type                   vreg_cmd_id;

    pm_switch_cmd_type                          vreg_cmd;
   
    pm_vreg_pdown_id_type                       vreg;

} ftm_pmic_vreg_config_ldo_pd_sw_cmd_type;


/* This type specifies the interface for the low power mode config command */

typedef PACKED struct
{

    diagpkt_subsys_header_type                  header;

    ftm_cmd_header_type                         ftm_cmd_header;

    ftm_pmic_vreg_cmd_id_type                   vreg_cmd_id;

    pm_switch_cmd_type                          vreg_cmd;
   
    pm_vreg_lp_id_type                          vreg;

} ftm_pmic_vreg_lp_mode_control_cmd_type;


/* This type specifies the interface for the config SMPS clock command */

typedef PACKED struct
{

    diagpkt_subsys_header_type                  header;

    ftm_cmd_header_type                         ftm_cmd_header;

    ftm_pmic_vreg_cmd_id_type                   vreg_cmd_id;

    ftm_pmic_vreg_smps_config_clk_cmd           vreg_smps_clk_cmd;

    PACKED union
    {
       
        pm_vreg_smps_clk_sel_type    vreg_smps_clk_sel;
        pm_vreg_smps_tcxo_div_type   vreg_smps_tcxo_clk_div;

    } smps_clk_config_union;

} ftm_pmic_vreg_smps_config_clk_cmd_type;








/*===========================================================================
                     INT DEFINITIONS
===========================================================================*/

typedef PACKED struct
{

    diagpkt_subsys_header_type                  header;

    ftm_cmd_header_type                         ftm_cmd_header;

    ftm_pmic_int_cmd_id_type                    int_cmd_id;

} ftm_pmic_int_header_type;

/* This type specifies the interface for the get Real Time Status command */

typedef PACKED struct
{

    diagpkt_subsys_header_type                  header;

    ftm_cmd_header_type                         ftm_cmd_header;

    ftm_pmic_int_cmd_id_type                    int_cmd_id;

    pm_rt_status_type                           status_id;

    boolean                                     status;

} ftm_pmic_int_get_rt_status_cmd_type;



/* This type specifies the interface for the Clear IRQ command */

typedef PACKED struct
{


    diagpkt_subsys_header_type                  header;

    ftm_cmd_header_type                         ftm_cmd_header;

    ftm_pmic_int_cmd_id_type                    int_cmd_id;

    pm_irq_hdl_type                             irq_handle;

} ftm_pmic_int_clr_irq_cmd_type;



/*===========================================================================
                     UI DEFINITIONS
===========================================================================*/

typedef PACKED struct
{

    diagpkt_subsys_header_type                  header;

    ftm_cmd_header_type                         ftm_cmd_header;

    ftm_pmic_ui_cmd_id_type                     ui_cmd_id;

} ftm_pmic_ui_header_type;


/* This type specifies the interface for the UI Set Drive Level command */

typedef PACKED struct
{

    diagpkt_subsys_header_type                  header;

    ftm_cmd_header_type                         ftm_cmd_header;

    ftm_pmic_ui_cmd_id_type                     ui_cmd_id;
  
    ftm_pmic_ui_high_drive_pin_id_type          high_drive_id;

    uint8                                       drive_level;
     
} ftm_pmic_ui_set_drive_level_cmd_type;

/* This type specifies the interface for the UI Speaker Config command */

typedef PACKED struct
{

    diagpkt_subsys_header_type                  header;

    ftm_cmd_header_type                         ftm_cmd_header;

    ftm_pmic_ui_cmd_id_type                     ui_cmd_id;

    pm_spkr_cmd_type                            spkr_cmd;

    pm_spkr_gain_type                           spkr_gain;

    pm_spkr_dly_type                            spkr_dly;

} ftm_pmic_ui_spkr_config_cmd_type;


/*===========================================================================
					 SPEAKER DEFINITIONS
===========================================================================*/


typedef PACKED struct
{

	diagpkt_subsys_header_type             		header;
  
	ftm_cmd_header_type                    		ftm_cmd_header;

	ftm_pmic_spkr_cmd_id_type              		spkr_cmd_id;

} ftm_pmic_spkr_header_type;


/* This type specifies the interface for the spkr set get mux hpf corner freq cmd */
typedef PACKED struct
{

	diagpkt_subsys_header_type             		header;
  
	ftm_cmd_header_type                    		ftm_cmd_header;

	ftm_pmic_spkr_cmd_id_type              		spkr_cmd_id;

	pm_spkr_cmd_type                       		ftm_spkr_rw;

	pm_spkr_hpf_corner_freq_type           		freq;

} ftm_pmic_spkr_set_get_corner_freq_cmd_type;  


/*===========================================================================
					 SPEAKER CONFIG DEFINITIONS
===========================================================================*/

//speaker mode low level mux control
typedef PACKED struct
{

	diagpkt_subsys_header_type             		header;
  
	ftm_cmd_header_type                    		ftm_cmd_header;

	ftm_pmic_spkr_cmd_id_type              		spkr_cmd_id;

	ftm_pmic_speaker_LL_config_cmd_id_type     	spkr_LL_config_cntl2_cmd_id;

	boolean                        			    enable_disable;

} ftm_pmic_speaker_LL_mux_cntl2_cmd_type;

//speaker mode high level mux control
typedef PACKED struct
{

	diagpkt_subsys_header_type             		header;
  
	ftm_cmd_header_type                    		ftm_cmd_header;

	ftm_pmic_spkr_cmd_id_type              		spkr_cmd_id;

	ftm_pmic_speaker_HL_config_cmd_id_type      spkr_HL_config_cntl2_cmd_id;

	boolean                        			    enable_disable;

} ftm_pmic_speaker_HL_mux_cntl2_cmd_type;

/*===========================================================================
					 SPEAKER LEFT RIGHT CONFIG DEFINITIONS
===========================================================================*/
typedef PACKED struct
{

	diagpkt_subsys_header_type             		header;
  
	ftm_cmd_header_type                    		ftm_cmd_header;

	ftm_pmic_spkr_cmd_id_type              		spkr_cmd_id;

	pm_spkr_left_right_type                     left_right;

	pm_spkr_cmd_type                       		spkr_cmd;

    pm_spkr_gain_type                    		gain;

} ftm_pmic_spkr_left_right_gain_cmd_type;  

typedef PACKED struct
{

	diagpkt_subsys_header_type             		header;
  
	ftm_cmd_header_type                    		ftm_cmd_header;

	ftm_pmic_spkr_cmd_id_type              		spkr_cmd_id;

	pm_spkr_left_right_type                     left_right;

	pm_spkr_cmd_type                       		spkr_cmd;

} ftm_pmic_spkr_left_right_en_mute_cmd_type;

typedef PACKED struct
{

	diagpkt_subsys_header_type             		header;
  
	ftm_cmd_header_type                    		ftm_cmd_header;

	ftm_pmic_spkr_cmd_id_type              		spkr_cmd_id;

	pm_spkr_left_right_type                     left_right;

	pm_spkr_cmd_type                       		spkr_cmd;

	pm_spkr_dly_type                       	    delay;


} ftm_pmic_spkr_left_right_delay_cmd_type;


/*===========================================================================
					 VIDEO AMPLIFIER DEFINITIONS
===========================================================================*/


typedef PACKED struct
{

	diagpkt_subsys_header_type             		header;
  
	ftm_cmd_header_type                    		ftm_cmd_header;

	ftm_pmic_vid_cmd_id_type               		vid_cmd_id;

} ftm_pmic_vid_header_type;


/* This type specifies the VID control command packet */

typedef PACKED struct
{

	diagpkt_subsys_header_type                  header;

	ftm_cmd_header_type                         ftm_cmd_header;

	ftm_pmic_vid_cmd_id_type                   	vid_cmd_id;

	boolean                                     enable_disable;
    
} ftm_pmic_vid_status_cmd_type;

/*===========================================================================
					 MIC DEFINITIONS
===========================================================================*/


typedef PACKED struct
{

	diagpkt_subsys_header_type             		header;
  
	ftm_cmd_header_type                    		ftm_cmd_header;

	ftm_pmic_mic_cmd_id_type               		mic_cmd_id;

} ftm_pmic_mic_header_type;


/* This type specifies the MIC control command packet */

typedef PACKED struct
{

	diagpkt_subsys_header_type                  header;

	ftm_cmd_header_type                         ftm_cmd_header;

	ftm_pmic_mic_cmd_id_type                    mic_cmd_id;

	boolean                                     enable_disable;

} ftm_pmic_mic_status_cmd_type;


/* This type specifies the mic voltage level */
typedef PACKED struct
{

	diagpkt_subsys_header_type                  header;

	ftm_cmd_header_type                         ftm_cmd_header;

	ftm_pmic_mic_cmd_id_type                    mic_cmd_id;
    
    pm_mic_volt_type                            volt_level;

} ftm_pmic_mic_set_level_cmd_type;


/*===========================================================================
					 MPP DEFINITIONS
===========================================================================*/
typedef PACKED struct
{

	diagpkt_subsys_header_type             		header;
  
	ftm_cmd_header_type                    		ftm_cmd_header;

	ftm_pmic_mpp_cmd_id_type               		mpp_cmd_id;

} ftm_pmic_mpp_header_type;


/* This type specifies the MPP digital in config command packet */

typedef PACKED struct
{

	diagpkt_subsys_header_type                  header;

	ftm_cmd_header_type                         ftm_cmd_header;

	ftm_pmic_mpp_cmd_id_type                    mpp_cmd_id;

	pm_mpp_which_type                           mpp;

    pm_mpp_dlogic_lvl_type                      level;

    pm_mpp_dlogic_in_dbus_type                  dbus;

} ftm_pmic_mpp_digital_in_cmd_type;


/* This type specifies the MPP digital out config command packet */

typedef PACKED struct
{

	diagpkt_subsys_header_type                  header;

	ftm_cmd_header_type                         ftm_cmd_header;

	ftm_pmic_mpp_cmd_id_type                    mpp_cmd_id;

	pm_mpp_which_type                           mpp;

    pm_mpp_dlogic_lvl_type                      level;

    pm_mpp_dlogic_out_ctrl_type                 ctrl;

} ftm_pmic_mpp_digital_out_cmd_type;


/* This type specifies the MPP digital in_out config command packet */

typedef PACKED struct
{

	diagpkt_subsys_header_type                  header;

	ftm_cmd_header_type                         ftm_cmd_header;

	ftm_pmic_mpp_cmd_id_type                    mpp_cmd_id;

	pm_mpp_which_type                           mpp;

    pm_mpp_dlogic_lvl_type                      level;

    pm_mpp_dlogic_inout_pup_type                pup;

} ftm_pmic_mpp_digital_inout_cmd_type;


/* This type specifies the MPP analog in config command packet */

typedef PACKED struct
{

	diagpkt_subsys_header_type                  header;

	ftm_cmd_header_type                         ftm_cmd_header;

	ftm_pmic_mpp_cmd_id_type                    mpp_cmd_id;

	pm_mpp_which_type                           mpp;

    pm_mpp_ain_ch_type                          channel;

} ftm_pmic_mpp_analog_in_cmd_type;


/* This type specifies the MPP analog out config command packet */

typedef PACKED struct
{

	diagpkt_subsys_header_type                  header;

	ftm_cmd_header_type                         ftm_cmd_header;

	ftm_pmic_mpp_cmd_id_type                    mpp_cmd_id;

	pm_mpp_which_type                           mpp;

    pm_mpp_aout_level_type                      level;

    pm_mpp_aout_switch_type                     _switch_;

} ftm_pmic_mpp_analog_out_cmd_type;


/* This type specifies the MPP analog out config command packet */

typedef PACKED struct
{

	diagpkt_subsys_header_type                  header;

	ftm_cmd_header_type                         ftm_cmd_header;

	ftm_pmic_mpp_cmd_id_type                    mpp_cmd_id;

	pm_mpp_which_type                           mpp;

    pm_mpp_i_sink_level_type                    level;

    pm_mpp_i_sink_switch_type                   _switch_;

} ftm_pmic_mpp_i_sink_cmd_type;

/*===========================================================================
					 RESET DEFINITIONS
===========================================================================*/


typedef PACKED struct
{

	diagpkt_subsys_header_type             		header;
  
	ftm_cmd_header_type                    		ftm_cmd_header;

	ftm_pmic_reset_cmd_id_type                 	reset_cmd_id;

} ftm_pmic_reset_header_type;


typedef enum
{
   FTM_PMIC_BLOW_FUSE            = 	0x12,		/* 1010 Command seq  	*/
   FTM_PMIC_CBL_PWR_ON_SWITCH	 =  0X34		/* FTM Command to issue pm_cbl_pwr_on_switch() */
} ftm_pmic_fuse_cmd_id_type ;

typedef PACKED struct
{

	diagpkt_subsys_header_type             		header ;

	ftm_cmd_header_type                    		ftm_cmd_header ;

	ftm_pmic_fuse_cmd_id_type                   ftm_pmic_fuse_cmd_id ;  

	byte		 								ftm_pmic_fuse_option_byte1 ;

	byte 										ftm_pmic_fuse_option_byte2 ;

	byte										ftm_pmic_fuse_option_byte3 ;

} ftm_pmic_process_fuse_commands_type ;


#endif /* FEATURE_PMIC_FTM_BYPASS */

#endif /* FEATURE_PMIC */

#endif /* FEATURE_FACTORY_TESTMODE */

#endif /* FTM_PMIC_H */
