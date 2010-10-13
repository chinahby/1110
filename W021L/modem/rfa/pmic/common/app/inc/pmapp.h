#ifndef PMAPP_H
#define PMAPP_H
/*===========================================================================


               P M A P P   H E A D E R    F I L E

DESCRIPTION
    This file contains functions prototypes and variable/type/constant 
  declarations for applications developed for the Qualcomm Power
  Manager Chip Set.
    
Copyright (c) 2003, 2004, 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2006, 2007, 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2009             by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/pmic/common/main/latest/app/inc/pmapp.h#12 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/11/09   vk      Removed T_MSM7500 Featurization from .h file for RPC. This 
                   API can be available for all targets.
11/04/08   vk      Added new votes for PM_VOTE_VREG_RFUBM_APP__GPS,
                   PM_VOTE_VREG_RFUBM_APP__RF_CHAIN0,
                   PM_VOTE_VREG_RFUBM_APP__RF_CHAIN1
09/30/08   jtn     Moved PMIC headset functions to pmapp_hsed.h
08/01/08   jtn     Move all proc comm functions to pmapp_pcil.c
07/11/08   jtn     Moved pm_vote_vreg_switch_pc() from library to common to
                   avoid link error in boot build.
07/11/08   jtn     Undid removal of pm_vreg_pa_dvs_work_around()--required for 
                   DZRF6500RF card
07/02/08   jtn     Added pm_vote_vreg_request_vote() API
06/23/08   jtn     Added VREG vote PM_VOTE_VREG_SYNTH_APP__RF_GPS 
06/16/08   jtn     Removed prototype for pm_variable_pa_vdd_init()
05/26/08   jtn     Added votes PM_VOTE_VREG_MPLL_APP__RF, PM_VOTE_VREG_MPLL_APP__CODEC,
                   PM_VOTE_VREG_MPLL_APP__MODEM
05/13/08   jtn     Added votes PM_VOTE_VREG_CDC2_APP__HKADC, 
                   PM_VOTE_VREG_CDC2_APP__CODEC
05/13/08   jtn     Added vote for PM_VOTE_VREG_RFUBM_APP__RF
05/07/08   vk      Added video load/unload detection
04/18/08   jtn     Added vote PM_VOTE_VREG_CDC2_APP__RF
03/31/08   jtn     Added vote PM_VOTE_VREG_GP1_APP__BT
03/11/08   jtn     Bug fix for ADS120 compiler, integrated VREG vote changes 
                   from RF sandbox build
02/05/08   jtn     Added vreg vote for Linux apps
02/01/08   jtn     Added vote PM_VOTE_VREG_BOOST_APP__ETHERNET
02/25/08   jtn     Added votes for VREG_MMC: PM_VOTE_VREG_MMC_APP__SDC1,
                   PM_VOTE_VREG_MMC_APP__SDC2, PM_VOTE_VREG_MMC_APP__SDC3, 
                   PM_VOTE_VREG_MMC_APP__SDC4
02/01/08   jtn     Added vote PM_VOTE_VREG_BOOST_APP__ETHERNET
01/30/08   jtn     Added votes for VREG_WLAN: PM_VOTE_VREG_WLAN_APP__SDC1,
                   PM_VOTE_VREG_WLAN_APP__SDC2, PM_VOTE_VREG_WLAN_APP__SDC3, 
                   PM_VOTE_VREG_WLAN_APP__SDC4
01/28/08   jtn     Added votes for VREG GP2:  PM_VOTE_VREG_GP2_APP__SDC1,
                       PM_VOTE_VREG_GP2_APP__SDC2, PM_VOTE_VREG_GP2_APP__SDC3,
                       PM_VOTE_VREG_GP2_APP__SDC4, PM_VOTE_VREG_GP2_APP__LCD_MAIN,
                       PM_VOTE_VREG_GP2_APP__LCD_SUB
01/07/08   jtn     Merged from QSC6270 branch
(begin QSC6270 changes) 
08/08/07   jnoblet Added RFTX2 and RFA votes for LCU/HAN
(end QSC6270 changes) 
01/18/08   jtn     (from RMD) Added VREG APPs to support power consumption on the MSM6800b + TRF6285.
12/11/07   jtn     Added (from HK) vreg votes for PM_VOTE_VREG_RFRX2_APP__RF_CHAIN0,
                   PM_VOTE_VREG_RFRX2_APP__RF_CHAIN1, PM_VOTE_VREG_RFRX2_APP__RF_MISC
12/10/07   jtn     Added (from HK) vreg votes for PM_VOTE_VREG_MMC_APP__TX1-4,
                   PM_VOTE_VREG_RFTX_APP__RF_GPS
11/27/07   jtn     Added vreg votes for PM_VOTE_VREG_GP1_APP__HSUSB, 
                   PM_VOTE_VREG_GP1_APP__BBRX, PM_VOTE_VREG_GP1_APP__CODEC,
                   PM_VOTE_VREG_GP1_APP__HKADC, PM_VOTE_VREG_GP2_APP__HSUSB,
                   PM_VOTE_VREG_GP2_APP__BBRX, PM_VOTE_VREG_GP2_APP__CODEC,
                   PM_VOTE_VREG_GP2_APP__HKADC
11/15/07   jtn     Added vreg vote for PM_VOTE_VREG_WLAN_APP__RF_GPS
11/02/07   jtn     Added vreg votes for PM_VOTE_VREG_GP2_APP__RF and
                   PM_VOTE_VREG_MMC_APP__RF
10/25/07   jtn     Added vreg votes for PM_VOTE_VREG_CAM_APP__LCD, 
                   PM_VOTE_VREG_AUX2_APP__LCD, and PM_VOTE_VREG_MSME2_APP__LCD
08/17/07   jnoblet Added voting enums for MSMA, RFRX1 and RFRX2
06/28/07   cng     Added pm_register_vreg_switch_callback and 
                   pm_deregister_vreg_switch_callback APIs to allow callback
                   upon VREG state change 
06/13/07   cng     Added enum PM_VOTE_VREG_WLAN_APP__RF
05/02/07   cng     Added PM_VOTE_VREG_RFRX1_APP__MBP and PM_VOTE_VREG_PA_APP__MBP 
02/27/07   cng     Reassigned some VREG APP IDs for uniqueness 
                   Added API pm_hsed_get_key_pressed and
                   pm_hsed_clear_key_pressed to return latchd status of HSED
                   key
02/02/07   vk      Added metacomments for the remoted APIs
10/07/06   hs      Updated API pm_perform_vbatdet_calibration()
10/04/06   rmd     Added the ability for the RF GSM code to vote to 
                   enable/disable the RF VREGs.
09/12/06   Vish    Added PM_VOTE_VREG_MMC_APP__MISC_SENSOR vote to MMC_VREG.
08/18/06   Vish    Integrated the following MediaFLO FFA changes
   IPS:
   07/18/05   jlh     Added PM_VOTE_VREG_WLAN_APP__BT to move BT to WLAN VREG.
   07/13/05   jlh     Added PM_VOTE_VREG_MMC_APP__LCD 
06/14/06   cng     Added init routine API for headset send/end detect feature
06/13/06   hs      Added PA thermistor client to RFTX VREG     
02/14/06   cng     Added enums in pm_vote_vreg_app_type for VREG voting
01/03/06   VK      Added High level speaker configuration settings
12/21/05   Vish    Added two APIs - pm_get_latched_pwr_key_pressed_status() and
                   pm_clear_latched_pwr_key_pressed_status() for accessing the
                   latched status of "power key pressed" condition.
10/31/05   VK      Minor changes in vbatdet calibration routine prototype
10/10/05   fpe     Removed unused enumeration PM_VOTE_VREG_PA_APP__GPS and 
                   PM_VOTE_VREG_RFTX_APP__GPS because GPS is a receiver entity 
                   only.
10/04/05   VK      added vbat calibration routine prototype and constants
07/25/05   jhs     Support for 7500 PWR key.
05/17/05   rmd     Added the ability for chain 1 to vote to enable/disable
                   RFRX1 VREG.
01/17/05   rmd     Added voting apps for RFRX2, SYNTH, RFTX, PA VREG.
10/26/04   rmd     Added voting apps for RFRX1 VREG.
08/30/04   rmd     Removed obsolete UIM init function.
05/16/04   rmd     Added pm_app_clk_busy_wait().
04/20/04   rmd     Added parameter in pm_variable_pa_vdd_init() to allow 
                   the caller to enable/disable PMIC support to PA DVS.
03/18/04   rmd     Register RF CHAIN0 and CHAIN1 to vote to enable/disable
                   SYNTH VREG.
03/05/04   Vish    Added voting apps for BOOST VREG.
02/03/04   rmd     Added pm_vote_vreg_switch() which allows multiple 
                   applications to enable/disable the same VREG.
11/14/03   rmd     Moved the temporary charger code into the charger VU.
11/12/03   Vish    Added sleep xtal osc management.
11/03/03   rmd     Added support for PMIC's internal over temperature
                   protection.
10/16/03   rmd     Configured PA_FET_N to be always low if PA DVS feature
                   is disable. Updated comments while i was at it.
09/04/03   rmd     Added initial support for the charger.
                   Added support for variable PA voltage.
09/02/03   rmd     Added initialization for PM66XX AMUX.
08/28/03   rmd     Added init function for UIM.
06/22/03   rmd     Initial revision.
===========================================================================*/

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
#define PM_VOTE_MAX_SHIFT 30

typedef enum 
{
    /* use the same vote ID for all VREGs controlled from Linux APP */
    PM_VOTE_VREG_LINUX_APP__ALL_VREGS   = (1 << PM_VOTE_MAX_SHIFT),
    /* MMC VREG APPs. */
    PM_VOTE_VREG_MMC_APP__MINI_SD       = (1 << 0),
    PM_VOTE_VREG_MMC_APP__LCD           = (1 << 1),
    PM_VOTE_VREG_MMC_APP__BT            = (1 << 3),
    PM_VOTE_VREG_MMC_APP__MISC_SENSOR   = (1 << 2),
    PM_VOTE_VREG_MMC_APP__MMC           = (1 << 4),
    PM_VOTE_VREG_MMC_APP__RF            = (1 << 5),
    PM_VOTE_VREG_MMC_APP__TX1           = (1 << 6),
    PM_VOTE_VREG_MMC_APP__TX2           = (1 << 7),
    PM_VOTE_VREG_MMC_APP__TX3           = (1 << 8),
    PM_VOTE_VREG_MMC_APP__TX4           = (1 << 9),
    PM_VOTE_VREG_MMC_APP__RF_GSM        = (1 << 10),
    PM_VOTE_VREG_MMC_APP__SDC1          = (1 << 11),
    PM_VOTE_VREG_MMC_APP__SDC2          = (1 << 12),
    PM_VOTE_VREG_MMC_APP__SDC3          = (1 << 13), 
    PM_VOTE_VREG_MMC_APP__SDC4          = (1 << 14),

    /* BOOST_5V VREG Apps. */
    PM_VOTE_VREG_BOOST_APP__LCD         = (1 << 0),
    PM_VOTE_VREG_BOOST_APP__OTG         = (1 << 1),
    PM_VOTE_VREG_BOOST_APP__ETHERNET    = (1 << 2),

    /* SYNTH VREG Apps. */
    PM_VOTE_VREG_SYNTH_APP__CHAIN0      = (1 << 0),
    PM_VOTE_VREG_SYNTH_APP__CHAIN1      = (1 << 1),
    PM_VOTE_VREG_SYNTH_APP__RF_GSM      = (1 << 2),
    PM_VOTE_VREG_SYNTH_APP__RF_GPS      = (1 << 3), 
    PM_VOTE_VREG_SYNTH_APP_GPS__CHAIN0  = (1 << 4),
    PM_VOTE_VREG_SYNTH_APP_GPS__CHAIN1  = (1 << 5),

    /* RFRX1 VREG Apps. */
    PM_VOTE_VREG_RFRX1_APP__RF_CHAIN0    = (1 << 0),
    PM_VOTE_VREG_RFRX1_APP__RF = PM_VOTE_VREG_RFRX1_APP__RF_CHAIN0,
    PM_VOTE_VREG_RFRX1_APP__RF_CHAIN1    = (1 << 1),
    PM_VOTE_VREG_RFRX1_APP__GPS          = (1 << 2),
    PM_VOTE_VREG_RFRX1_APP__RF_GSM       = (1 << 3),
    PM_VOTE_VREG_RFRX1_APP__MBP          = (1 << 4),
    PM_VOTE_VREG_RFRX1_APP__FTM          = (1 << 5),
    
    /* RFRX2 VREG Apps. */
    PM_VOTE_VREG_RFRX2_APP__RF          = (1 << 0),
    PM_VOTE_VREG_RFRX2_APP__GPS         = (1 << 1),    
    PM_VOTE_VREG_RFRX2_APP__RF_CHAIN0   = (1 << 2),
    PM_VOTE_VREG_RFRX2_APP__RF_CHAIN1   = (1 << 3),
    PM_VOTE_VREG_RFRX2_APP__RF_MISC     = (1 << 4),
    PM_VOTE_VREG_RFRX2_APP__FTM         = (1 << 5),
    PM_VOTE_VREG_RFRX2_APP__RF_GSM      = (1 << 6),

    /* RFUBM VREG apps */
    PM_VOTE_VREG_RFUBM_APP__RF          = (1 << 0),
    PM_VOTE_VREG_RFUBM_APP__GPS         = (1 << 1),    
    PM_VOTE_VREG_RFUBM_APP__RF_CHAIN0   = (1 << 2),
    PM_VOTE_VREG_RFUBM_APP__RF_CHAIN1   = (1 << 3),

    /* VREG CDC2 Apps. */
    PM_VOTE_VREG_CDC2_APP__RF           = (1 << 0),
    PM_VOTE_VREG_CDC2_APP__HKADC        = (1 << 1),
    PM_VOTE_VREG_CDC2_APP__CODEC        = (1 << 2),

    /* VREG MPLL Apps */
    PM_VOTE_VREG_MPLL_APP__RF           = (1 << 0),
    PM_VOTE_VREG_MPLL_APP__CODEC        = (1 << 1),
    PM_VOTE_VREG_MPLL_APP__MODEM        = (1 << 2),

    /* VREG PA Apps. */
    PM_VOTE_VREG_PA_APP__RF             = (1 << 0),
    PM_VOTE_VREG_PA_APP__MBP            = (1 << 1),
    PM_VOTE_VREG_PA_APP__RF_GSM         = (1 << 2), 
    
    /* VREG RFTX Apps. */
    PM_VOTE_VREG_RFTX_APP__RF           = (1 << 0),
    PM_VOTE_VREG_RFTX_APP__PA_THERM     = (1 << 1),
    PM_VOTE_VREG_RFTX_APP__RF_GSM       = (1 << 2),
    PM_VOTE_VREG_RFTX_APP__RF_GPS       = (1 << 3),
    PM_VOTE_VREG_RFTX_APP__RF_CHAIN0    = (1 << 4),

    /* VREG RFTX2 Apps. */
    PM_VOTE_VREG_RFTX2_APP__RF          = (1 << 0),

    /* VREG RFA Apps. */
    PM_VOTE_VREG_RFA_APP__RF            = (1 << 0),

    /* WLAN VREG Apps. */
    PM_VOTE_VREG_WLAN_APP__BT           = (1 << 0),
    PM_VOTE_VREG_WLAN_APP__RF           = (1 << 1),
    PM_VOTE_VREG_WLAN_APP__RF_GPS       = (1 << 2),
    PM_VOTE_VREG_WLAN_APP__RF_GSM       = (1 << 3),
    PM_VOTE_VREG_WLAN_APP__SDC1         = (1 << 4),
    PM_VOTE_VREG_WLAN_APP__SDC2         = (1 << 5),
    PM_VOTE_VREG_WLAN_APP__SDC3         = (1 << 6),
    PM_VOTE_VREG_WLAN_APP__SDC4         = (1 << 7),
    PM_VOTE_VREG_WLAN_APP__RF_CHAIN0    = (1 << 8),

    /* VREG RUIM Apps. */
    PM_VOTE_VREG_RUIM_APP__RUIM         = (1 << 0),

    /* VREG RUIM2 Apps. */
    PM_VOTE_VREG_RUIM2_APP__CAMIF       = (1 << 0),

    /* VREG MDDI Apps. */
    PM_VOTE_VREG_MDDI_APP__CAMIF        = (1 << 0),

    /* VREG CAMIF Apps. */
    PM_VOTE_VREG_CAM_APP__CAMIF         = (1 << 0),
    PM_VOTE_VREG_CAM_APP__LCD           = (1 << 1),

    /* VREG AUX Apps. */
    PM_VOTE_VREG_AUX_APP__AUX           = (1 << 0),

    /* VREG AUX2 Apps. */
    PM_VOTE_VREG_AUX2_APP__AUX2         = (1 << 0),
    PM_VOTE_VREG_AUX2_APP__LCD          = (1 << 1),

    /* VREG BT Apps. */
    PM_VOTE_VREG_BT_APP__BT             = (1 << 0),

    /* VREG MSME2 Apps. */
    PM_VOTE_VREG_MSME2_APP__LCD         = (1 << 0),

    /* VREG MSMA Apps. */
    PM_VOTE_VREG_MSMA_APP__CODEC        = (1 << 0),
    PM_VOTE_VREG_MSMA_APP__HKADC        = (1 << 1),
    PM_VOTE_VREG_MSMA_APP__RF           = (1 << 2),
    PM_VOTE_VREG_MSMA_APP__MODEM        = (1 << 3),
    PM_VOTE_VREG_MSMA_APP__GP1          = (1 << 4),
    PM_VOTE_VREG_MSMA_APP__GP2          = (1 << 5),

    /* VREG GP1 Apps. */
    PM_VOTE_VREG_GP1_APP__HSUSB         = (1 << 1),
    PM_VOTE_VREG_GP1_APP__BBRX          = (1 << 2),
    PM_VOTE_VREG_GP1_APP__CODEC         = (1 << 3),
    PM_VOTE_VREG_GP1_APP__HKADC         = (1 << 4),
    PM_VOTE_VREG_GP1_APP__BT            = (1 << 5),

	/* VREG GP2 Apps. */
    PM_VOTE_VREG_GP2_APP__RF            = (1 << 0),
    PM_VOTE_VREG_GP2_APP__HSUSB         = (1 << 1),
    PM_VOTE_VREG_GP2_APP__BBRX          = (1 << 2),
    PM_VOTE_VREG_GP2_APP__CODEC         = (1 << 3),
    PM_VOTE_VREG_GP2_APP__HKADC         = (1 << 4),
    PM_VOTE_VREG_GP2_APP__RF_GSM        = (1 << 5),
    PM_VOTE_VREG_GP2_APP__SDC1          = (1 << 6),
    PM_VOTE_VREG_GP2_APP__SDC2          = (1 << 7),
    PM_VOTE_VREG_GP2_APP__SDC3          = (1 << 8),
    PM_VOTE_VREG_GP2_APP__SDC4          = (1 << 9),
    PM_VOTE_VREG_GP2_APP__LCD_MAIN      = (1 << 10),
    PM_VOTE_VREG_GP2_APP__LCD_SUB       = (1 << 11),

    /* VREG S2 Apps. */
    PM_VOTE_VREG_S2_APP__RF             = (1 << 0),
    PM_VOTE_VREG_S2_APP__GPS            = (1 << 1),
    PM_VOTE_VREG_S2_APP__RF_GSM         = (1 << 2),
    PM_VOTE_VREG_S2_APP__MBP            = (1 << 3),
    PM_VOTE_VREG_S2_APP__FTM            = (1 << 4),

    /* VREG S3 Apps. */
    PM_VOTE_VREG_S3_APP__RF             = (1 << 0),
    PM_VOTE_VREG_S3_APP__GPS            = (1 << 1),
    PM_VOTE_VREG_S3_APP__RF_GSM         = (1 << 2),
    PM_VOTE_VREG_S3_APP__MBP            = (1 << 3),
    PM_VOTE_VREG_S3_APP__FTM            = (1 << 4)
}pm_vote_vreg_app_type;

/* enum to specify whether a callback to be called before or after VREG state change */
typedef enum
{
    PM_VREG_CB_BEFORE,
    PM_VREG_CB_AFTER,
    PM_VREG_CB_INVALID
} pm_vreg_cb_when_type;

//Listing some commonly used speaker configuration settings

static const pm_spkr_config_mode_type pm_speaker_config__default = \
            { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};
static const pm_spkr_config_mode_type pm_speaker_config__stereo = \
            { TRUE, TRUE, FALSE, TRUE, FALSE, FALSE, FALSE, TRUE};
static const pm_spkr_config_mode_type pm_speaker_config__left_mono = \
            { FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};
static const pm_spkr_config_mode_type pm_speaker_config__right_mono = \
            { TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE}; 
static const pm_spkr_config_mode_type pm_speaker_config__dual_mono = \
            { TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};
static const pm_spkr_config_mode_type pm_speaker_config__left_8ohm_recv = \
            { FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE};
static const pm_spkr_config_mode_type pm_speaker_config__right_8ohm_recv = \
            { TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE};
static const pm_spkr_config_mode_type pm_speaker_config__summed_mono_left = \
            { FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, TRUE }; 
static const pm_spkr_config_mode_type pm_speaker_config__summed_mono_right = \
            { TRUE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, TRUE }; 
static const pm_spkr_config_mode_type pm_speaker_config__usb_carkit = \
            { FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, TRUE, FALSE };

/*===========================================================================

                      FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

FUNCTION pm_pwr_key_init                                EXTERNAL FUNCTION

DESCRIPTION
  This function initializes the power key ISR.
    
PARAMETERS
  None.  

RETURN VALUE
  None.

DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  3) pm_init_delayed() 
  
SIDE EFFECTS
  None
  
===========================================================================*/
extern void pm_pwr_key_init(void);

/*===========================================================================

FUNCTION pm_set_power_key_duration                  EXTERNAL FUNCTION

DESCRIPTION
    Use this function to set the amount of time that the power key is
    debounced.
       
PARAMETERS
  Value of the new debounce duration  

RETURN VALUE
  None

DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  3) pm_init_delayed()
  4) pm_pwr_key_init() 
  
SIDE EFFECTS
  Changes the amount of time that the power key will be debounce in
  software.
  
===========================================================================*/
extern void pm_set_power_key_duration( word duration);
/*~ FUNCTION pm_set_power_key_duration */

/*===========================================================================

FUNCTION pm_is_power_key_pressed                    EXTERNAL FUNCTION

DESCRIPTION
    Use this function to find out if the power key is pressed 
       
PARAMETERS
  None.  

RETURN VALUE
  Return type: Boolean.
    
        TRUE  = Power Key is pressed.
        FALSE = Power Key is not pressed.

DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  3) pm_init_delayed()
  4) pm_pwr_key_init() 
  
SIDE EFFECTS
  None
  
===========================================================================*/
extern boolean pm_is_power_key_pressed(void);


/*===========================================================================
FUNCTION   pm_get_latched_pwr_key_pressed_status           EXTERNAL FUNCTION

DESCRIPTION
  Use this function to query the current latched status of "power key
  pressed" condition. Since this a latched status, it means that it will
  indicate if the power key was pressed any time prior to this fn call and
  so the latched status may not be the same as the real-time status returned 
  by pm_is_power_key_pressed(). After the power key pressed condition has been
  detected, this function will continue to indicate the latched condition
  until the latched condition is explicitly cleared using 
  pm_clear_latched_pwr_key_pressed_status().
       
PARAMETERS
  None.  

RETURN VALUE
  Return type: Boolean
    TRUE  --> Power Key pressed condition was detected            
    FALSE --> Power Key pressed condition has not been detected yet (or since
              the last pm_clear_latched_pwr_key_pressed_status() invocation)

DEPENDENCIES
  The following functions must have been invoked before:
  1) rflib_init() 
  2) pm_init()
  3) pm_init_delayed()
  
SIDE EFFECTS
  Interrupts are locked in this fn.  
===========================================================================*/
extern boolean pm_get_latched_pwr_key_pressed_status(void);


/*===========================================================================
FUNCTION   pm_clear_latched_pwr_key_pressed_status         EXTERNAL FUNCTION

DESCRIPTION
  Use this function to clear the latched status of power key pressed
  condition. This will result in subsequent pm_get_latched_pwr_key_pressed_status()
  calls to return FALSE until the power key is pressed again.
       
PARAMETERS
  None.  

RETURN VALUE
  None.

DEPENDENCIES
  The following functions must have been invoked before:
  1) rflib_init() 
  2) pm_init()
  3) pm_init_delayed()
  
SIDE EFFECTS
  Interrupts are locked in this fn.
===========================================================================*/
extern void pm_clear_latched_pwr_key_pressed_status(void);


/*===========================================================================

FUNCTION pm_adc_init                                     EXTERNAL FUNCTION

DESCRIPTION
  Initializes the PM66XX ADC. 
    
PARAMETERS
  None.  

RETURN VALUE
  None.

DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  
SIDE EFFECTS
  None  
  
===========================================================================*/
extern void pm_adc_init(void);

/*===========================================================================

FUNCTION pm_app_clk_busy_wait                            EXTERNAL FUNCTION

DESCRIPTION
  Halts the microprocessor execution for the specified amount of time. 
    
PARAMETERS
  1) Parameter name: dvs_enable.
     - How long we want to halt

     Parameter type: uint32
     - Valid Inputs: micro secounds.
     
RETURN VALUE
  None.

DEPENDENCIES
  THIS FUNCTION IS USED BY THE PMIC LIBRARY. PLEASE, DO NOT MODIFY!!!.
  
SIDE EFFECTS
  Halts the microprocessor execution for the specified amount of time. 
  
===========================================================================*/
extern void pm_app_clk_busy_wait(uint32 us);

#if defined(FEATURE_PMIC_VARIABLE_PA_VDD)
/*===========================================================================

FUNCTION pm_variable_pa_vdd_init                     EXTERNAL FUNCTION

DESCRIPTION
    This function initializes the PM66XX to support the
 PA Dynamic Voltage Setting.
  
  This function will:
  1) If dvs_enable (user parameter) == FALSE then
     it will configure PA_FET_N to low to keep the PA 
     in HIGH power mode.   
  2) If dvs_enable (user parameter) == TRUE then
     it will configure PA_FET_N to follow PA_R0.
     See figure bellow.
  3) If dvs_enable (user parameter) == TRUE, FEATURE_RF_PA_DVS 
     is defined and the RF driver is not managing the VREGs then
     this function will configure and enable the VREG PA. 
     
          
                **************************               ******
                *       PM66XX IC        *               *    *
                *                        *               *    *
      PA_R0 --->* PMIC MPPX     PMIC MPPX*- PA_FET_N --> * PA *
                * (INPUT)       (OUTPUT) *               *    *
                *                        *               *    *
                **************************               ******
                
     where:
           PA_FET_N * PA GAIN   * PA VDD
           ---------*-----------*-------
             HIGH   * LOW  GAIN * VREG PA
             LOW    * HIGH GAIN * VPH POWER
                
PARAMETERS
  1) Parameter name: dvs_enable.
     - Enable/Disable PMIC's support to PA DVS.

     Parameter type: boolean.
     - TRUE  = Enable.
     - FALSE = Disable.
     
RETURN VALUE
 None.
    
DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  
SIDE EFFECTS
  None  
  
===========================================================================*/
extern void pm_variable_pa_vdd_init( boolean dvs_enable );
#endif /* FEATURE_PMIC_VARIABLE_PA_VDD */

#if defined(FEATURE_PMIC_INTERNAL_IC_TEMPERATURE)
/*===========================================================================

FUNCTION pm_int_temp_protection_init                   EXTERNAL FUNCTION

DESCRIPTION
    This function
                     
PARAMETERS
  None.  

RETURN VALUE
  None.

DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  
SIDE EFFECTS
  None  
  
===========================================================================*/
extern void pm_int_temp_protection_init(void);
#endif /* FEATURE_PMIC_INTERNAL_IC_TEMPERATURE */

#if defined(FEATURE_PMIC_32K_XTAL)
extern void pm_sleep_xtal_halted_isr(void);
#endif //FEATURE_PMIC_32K_XTAL


/*===========================================================================

FUNCTION pm_vote_vreg_switch                          EXTERNAL FUNCTION

DESCRIPTION
      Use this function if 1 or more applications need to use a single
  voltage regulator. This function keeps track of which apps require
  a specific VREG to be enable. This function will only allow the VREG 
  to be disable, if all the apps that have previously enabled the VREG
  through this function, call this function to disable it. 
  
  WARNING!!!!: 
      Do not use this function in conjunction with "pm_vreg_control()". 
    
      Example: if you enable MMC through pm_vreg_control(), you should not 
               use this function to disable it.
  
                     
PARAMETERS
  1) Parameter name: cmd.
     - Turn the VREG ON or OFF

     Parameter type: pm_switch_cmd_type (enum).
     - Valid Inputs:
            PM_OFF_CMD = Disable the VREG.
            PM_ON_CMD  = Enable the VREG.

  2) Parameter name: vreg_id. 
     - Which VREG needs to be turn ON/OFF.

     Parameter type: pm_vreg_id_type (enum).
     - Valid Inputs:
        PM_VREG_MSMA_ID
        PM_VREG_MSMP_ID
        PM_VREG_MSME_ID
        PM_VREG_MSMC1_ID
        PM_VREG_MSMC2_ID
        PM_VREG_MSMS_ID
        PM_VREG_MEM_ID
        PM_VREG_GP_ID
        PM_VREG_CAM_ID
        PM_VREG_TCXO_ID
        PM_VREG_PA_ID
        PM_VREG_RFTX_ID
        PM_VREG_RFRX1_ID
        PM_VREG_RFRX2_ID
        PM_VREG_SYNT_ID
        PM_VREG_WLAN_ID
        PM_VREG_USB_ID
        PM_VREG_BOOST_ID
        PM_VREG_MMC_ID
        PM_VREG_RUIM_ID     
 
  3) Parameter name: pm_vote_vreg_app_type app_mask 
     - Which Application wants to enable/disable the VREG.

     Parameter type: pm_vote_vreg_app_type (enum).
       Use one of the constants defined in pm_vote_vreg_app_type or
       a vote returned from calling pm_vote_vreg_request_vote()
     
RETURN VALUE
  None.

DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  
SIDE EFFECTS
  None  
===========================================================================*/
extern void pm_vote_vreg_switch( pm_switch_cmd_type cmd, 
                                 pm_vreg_id_type vreg_id,
                                 pm_vote_vreg_app_type app_mask );
/*~ FUNCTION pm_vote_vreg_switch */

/**
 * pm_vote_vreg_request_vote(pm_vreg_id_type       vreg_id,
 *                           pm_vote_vreg_app_type *vote)
 * 
 * This function should be used to request a vote for use
 * with the pm_vote_vreg_switch() API.
 * 
 * Many votes are already defined in the pm_vote_vreg_app_type
 * enumeration, but as new votes are required, this API
 * allows the application to request votes dynamically
 * instead of requesting additional (constant) votes
 * in the PMIC driver.
 * 
 * @param vreg_id pm_vreg_id_type, this is the VREG for which the application
 *                code is requesting a vote
 * @param vote    A pointer to the vote.  If this function returns TRUE,
 *                vote will be set to the next available vote for vreg_id.
 * 
 * @return TRUE if a vote was available
 *         FALSE if a vote was not available or if vreg_id
 *         was out of range
 * @example 
 * ...
 *  pm_vote_vreg_app_type vote;
 *  boolean result;
 * 
 *  //request a vote for controlling VREG_GP2
 *  result = pm_vote_vreg_request_vote(PM_VREG_GP2_ID,
 *                                     &vote);
 * ...
 *  //vote to turn VREG_GP2 on
 *  (void) pm_vote_vreg_switch(PM_ON_CMD,
 *                             PM_VREG_GP2_ID,
 *                             vote);
 * ...
 */
extern boolean pm_vote_vreg_request_vote(pm_vreg_id_type vreg_id, pm_vote_vreg_app_type *vote);
/*~ FUNCTION pm_vote_vreg_request_vote */
/*~ PARAM OUT vote POINTER */

/*===========================================================================

FUNCTION pm_register_vreg_switch_callback                    EXTERNAL FUNCTION

DESCRIPTION
    This function allows registration of a callback to be called upon state
change of a specified VREG. The callback can also specified to call before
or after the VREG state change happens.
  
                     
PARAMETERS

  1) Parameter name: vreg_id. 
     - Which VREG to be registered with a callback function 

     Parameter type: pm_vreg_id_type (enum).
 
  2) Parameter name: when_to_call 
     - whether the callback to be called before or after the VREG changes state

     Parameter type: pm_vreg_cb_when_type (enum).
     - Valid Inputs:
        PM_VREG_CB_BEFORE
        PM_VREG_CB_AFTER

  2) Parameter name: vreg_cb 
     - the function pointer of the callback to be registered

     
RETURN VALUE
  boolean: 
    TRUE - callback registeration succeeded
    FALSE - callback registeration failed

DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  
SIDE EFFECTS
  None  
===========================================================================*/
extern boolean pm_register_vreg_switch_callback( pm_vreg_id_type vreg_id,  
                                          pm_vreg_cb_when_type when_to_call,
                                          void (*vreg_cb)(pm_vreg_id_type, pm_switch_cmd_type) );

/*===========================================================================

FUNCTION pm_deregister_vreg_switch_callback                 EXTERNAL FUNCTION

DESCRIPTION
    This function allows deregistration of a callback to associated with a 
specified VREG. 
  
PARAMETERS

  1) Parameter name: vreg_id. 
     - Which VREG to be deregistered with a callback function 

     Parameter type: pm_vreg_id_type (enum).
 
  2) Parameter name: when_to_call 
     - whether the callback  is called before or after the VREG changes state

     Parameter type: pm_vreg_cb_when_type (enum).
     - Valid Inputs:
        PM_VREG_CB_BEFORE
        PM_VREG_CB_AFTER

  2) Parameter name: vreg_cb 
     - the function pointer of the callback to be deregistered

     
RETURN VALUE
  boolean: 
    TRUE - callback deregisteration succeeded
    FALSE - callback deregisteration failed

DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  
SIDE EFFECTS
  None  
===========================================================================*/
extern boolean pm_deregister_vreg_switch_callback( pm_vreg_id_type vreg_id,  
                                            pm_vreg_cb_when_type when_to_call,
                                            void (*vreg_cb)(pm_vreg_id_type, pm_switch_cmd_type) );

/*===========================================================================

FUNCTION pm_perform_vbatdet_calibration                        EXTERNAL FUNCTION

DESCRIPTION
      This function is used for calibrating vbatdet
  
  
PARAMETERS
  None   
 
RETURN VALUE
  Description:
  - Error Flag.
  
  Return value type: pm_err_flag_type.        
  - Valid output:
        PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to 
                                             communicate with the PMIC.        
        PM_ERR_FLAG__SUCCESS               = Operation was successful.

DEPENDENCIES
  1) Enable TX WAIT state( disable pulse charging) before calling calibration 
  routine and disable TX WAIT state( enable pulse charging) after calibration.
  eg.  //Enable TX WAIT state, disable pulse charging
       chg_tx_wait_vote(CHG_CMD_ENABLE, CHG_TX_VOTE__VBATDET);

       pm_perform_vbatdet_calibration();


       //disable TX WAIT state, enable pulse charging
       chg_tx_wait_vote(CHG_CMD_DISABLE, CHG_TX_VOTE__VBATDET);
  
  2) During the calibration procedure, VMAXSEL has been changed to 4.2V. So 
  make sure to restore the to original value after the calibration is finished.
  
SIDE EFFECTS
  None  
===========================================================================*/
extern pm_err_flag_type pm_perform_vbatdet_calibration(void);


#if defined(FEATURE_PMIC_VIDEO_AMPLIFIER)
/*===========================================================================

FUNCTION pm_vid_init                                EXTERNAL FUNCTION

DESCRIPTION
  This function initializes the video amplifier load/unload detect settings and ISR 
    
PARAMETERS
  None.  

RETURN VALUE
  None.

DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  3) pm_init_delayed() 
  
SIDE EFFECTS
  None
  
===========================================================================*/
extern void pm_vid_init(void);

#endif /* FEATURE_PMIC_VIDEO_AMPLIFIER */

#endif /* PMAPP_H */
