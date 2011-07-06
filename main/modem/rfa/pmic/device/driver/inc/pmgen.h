#if !defined(PMGEN_H)
#define PMGEN_H
/*===========================================================================


                  P M G E N   H E A D E R    F I L E

GENERAL DESCRIPTION
  This file contains generic, (ex: Small drivers like speaker driver,
  vibrator, etc that are too small to require their own file.), functions
  and variable prototypes to support interaction with the Qualcomm
  Power Management ICs.

Copyright (c) 2003,2004 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2005,2006 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2007      by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/drivers/rf/branch/ULC/CR259554/pmgen.h#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/3/10    nv      Added API for Configuring minimum P_ON Time for requested Buck SMPS
04/08/09   jtn     Added pm_tcxo2_set_drive_strength() API.  Added two new
                   drive strength levels for PMIC4
02/24/09   APU     RPCed all audio APIs as requested by the audio-team. 
11/26/08   jtn     Added PM_SPKR_SET_PDM_MODE, PM_SPKR_SET_PWM_MODE commands 
                   to pm_speaker_cmd() API
10/22/08   jtn     Added enum and APIs for HSED module type, APIs for
                   pm_hsed_status_type_1_get() and pm_hsed_get_module_type()
10/18/08   jtn     Added API to return PMIC chip revision pm_get_pmic_revision(void)
10/03/08   jtn     Added pm_hsed_set_current_threshold() API for PMIC4 HSED
06/27/08   jtn     Merge changes from QSC1100 branch
06/20/08   jtn     Added  pm_32k_clk_app_vote_type
06/16/08   jtn     Correct compiler warning for superfluous comma in enum
02/01/08   jtn     Added TCXO_XO_GP2 functionality to pm_tcxo2_cmd()
01/07/08   jtn     Merged from QSC6270 branch
(begin QSC6270 changes) 
10/30/07   jnoblet Added enums and function prototypes for USIM_AUX controller
                   for HAN
08/10/07   jnoblet Added enums and function prototypes for PA_ON controller
                   for HAN
(end QSC6270 changes)
11/13/07   vk      Added meta comments to some speaker APIs
11/08/07   vk      Added API pm_usb_ovp_is_supported
06/18/07   cng     Added meta comments to miscellaneous MIC APIs
04/13/07   cng     Added API pm_vid_load_detect_en() to enable/disable load
                   detect of video amplifier 
02/27/07   cng     Added APIs for PM6658
                   secondary TCXO controller: pm_tcxo2_cmd
                   USB over-voltage protection (OVP):  pm_usb_ovp_enable
                                                       pm_usb_ovp_set_threshold
                                                       pm_usb_ovp_set_hystersis
                   PWM generator: pm_pwm_generator_select_clock 
                                  pm_pwm_generator_select_pre_divider
                                  pm_pwm_generator_set
                   Expanded pm_hsed_period_time_type and pm_hsed_period_time_type
                   for full bit resolution 
02/05/07   vk      Added metacommands for remoted APIs
01/29/07   cng     Added API pm_get_power_on_status and pm_clear_power_on_status 
12/07/06   cng     Added API pm_tcxo_set_drive_strength and
                   enum pm_tcxo_drive_strength_type to configure TCXO buffer
                   output drive strength  
                   Added API pm_xo_boost_enable to allow boost of XO core AGC gain 
10/05/06   hs      Added API pm_get_vbatdet_trim_limits() to get vbatdet trim config;
                   Added API pm_get_vbatdet_trim() to get current vbatdet trim val;
                   Added API pm_vbatdet_trim_test_mode_switch() to enable/disable
                   VBATDET trim test mode;
10/02/06   cng     Changed first parameter type and updated documentation for API 
                   pm_clockless_pfm_enable() 
07/26/06   cng     Removed duplicate declaration of pm_hard_reset 
06/12/06   cng     Added enums and APIs for headset send/end detect for PM6640
03/14/06   cng     Corrected API name to pm_mbg_lpm_force_nbias_sleep;
                   Added API to pm_get_pmic_model to return PMIC's chip revision
02/14/06   cng     Added APIs pm_mbg_lpm_dec_ref_byp_r() and 
                   pm_mbg_lpm_forcce_nbias_sleep() needed for MBG low power mode
01/24/06   vk      Removed argument PMIC_TYPE from pm_init_comm() 
01/03/06   vk      Added High level speaker mode configuration support
                   Added left/right speaker gain,delay,mute enable/disable support
                   Added speaker set/get-freq, vid on/off, mic on/off set-volt 
                   Added hard reset command
12/15/05   cng     Extern'ed pm_app_diag_lib_diag_i
12/08/05   cng     Revised XO trimming control API for searcher use
11/09/05   cng     Added pm_speaker_1k6_zin_enable() API for PM6640 and Panoramix
10/31/05   VK      Added pm_set_vbatdet_trim() API
10/24/05   cng     Added Panoramix support for XO control and PM6640 support
                   for MBG LPM control and clockless PFM mode control function 
                   prototypes
10/07/05   fpe     Added pm_hard_reset prototype per CR78648.
02/04/05   rmd     Corrected minor comments mistakes on cable power up API.
02/01/05   rmd     Added support for the power key pin and cable power On pins
                   pull up.
08/19/04   Vish    Fixed power-on key hysteresis delay configuration in 
                   pm_power_key_setup().
08/09/04   rmd     * Added the ability to enable/disable the PMIC's start up 
                     (power on) abort timer feature.
                   * Added the ability to enable/disable the PMIC's watchdog 
                     detect feature.
06/07/04   rmd     Added the ability to read the LCD and KBD settings.
05/20/04   rmd     Added support for the Flash LED.
05/16/04   rmd     Added support for cable power ON.
03/18/04   rmd     Added support for the vibrator motor.
03/11/04   rmd     Added intensity settings for LCD and KBD LEDs.
03/08/04   rmd     Added support for SMPL.
01/28/04   rmd     Added initial support for multiple PMIC models/tiers.
11/14/03   rmd     Made pm_soft_reset() a dummy fumction. This function
                   is not longer supported.
11/07/03   Vish    Added support for sleep clock driver.
11/03/03   rmd     Added support for PMIC's internal over temperature
                   protection.
10/16/03   rmd     Added support for the pm_soft_reset().
10/02/03   rmd     Added support for the Speaker driver.
09/04/03   rmd     Moved the charger function into the pmapp.* files.
08/28/03   rmd     Moved pm_ruim_init() prototype definition to pmapp.h.
                   Please note that the function was renamed
                   pm_uim_init().
07/24/03   rmd     Added support LEDs and CHG init.
07/21/03   rmd     Added support for the power key hysteresis.
05/29/03   rmd     Created.
============================================================================*/

/*===========================================================================

                     GLOBAL TYPE DEFINITIONS

===========================================================================*/

/* Use this enum to set PMIC pins active high/low */
typedef enum
{
    PM_POLARITY_ACTIVE_HIGH,
    PM_POLARITY_ACTIVE_LOW,
    PM_POLARITY_INVALID
}pm_polarity_type;

/*========================== TCXO Controller/Buffer =======================*/
typedef enum
{
  PM_TCXO_MANUAL_MODE_CMD,    // configure the controller to be operable in
                              // MANUAL mode wherein the switching is done in
                              // software using APIs provided in this file
  PM_TCXO_AUTOMATIC_MODE_CMD, // configure the controller to be operable in
                              // AUTOMATIC mode wherein the switching is done
                              // in hardware using TCXO_EN pin

  PM_TCXO_BUFFER_ON_CMD,      // enable TCXO buffer
  PM_TCXO_BUFFER_OFF_CMD,     // disable TCXO buffer

  PM_TCXO_EMULATE_TCXO_EN_ACTIVE_CMD,   // this MANUAL mode command has the same
                                        // effect as that of driving TCXO_EN pin
                                        // active in AUTOMATIC mode, i.e. it turns
                                        // TCXO LDO on and after the programmed
                                        // delay, enables the TCXO buffer
  PM_TCXO_EMULATE_TCXO_EN_INACTIVE_CMD, // this MANUAL mode command has the same
                                        // effect as that of driving TCXO_EN pin
                                        // inactive in AUTOMATIC mode, i.e. it
                                        // turns TCXO LDO off and disables the
                                        // TCXO buffer
  PM_TCXO_XO_GP2_OUT_ENABLE,            // enable sine wave output
  PM_TCXO_XO_GP2_OUT_DISABLE            // disable sine wave output
}pm_tcxo_cmd_type;

/* in order of relative drive strength, LOW-LOW2-HIGH2-HIGH */
/* PMIC3 only supports LOW and HIGH */
/* PMIC4 supports all 4 settings */
typedef enum
{
    PM_TCXO_DRIVE_STRENGTH_LOW,
    PM_TCXO_DRIVE_STRENGTH_HIGH,
    PM_TCXO_DRIVE_STRENGTH_LOW2,  
    PM_TCXO_DRIVE_STRENGTH_HIGH2,                            
    PM_TCXO_DRIVE_STRENGTH_INVALID,
    PM_TCXO_DRIVE_STRENGTH_PMIC3_INVALID = PM_TCXO_DRIVE_STRENGTH_HIGH + 1
} pm_tcxo_drive_strength_type;

/*============== XO Controller/Buffer SLEEP CLOCK SOURCE ================*/
typedef enum
{
  PM_XO_32KHZ_SLEEP_CLOCK_XTAL_OSC,
  PM_XO_19_2MHZ_XTAL_OSC,
  PM_XO_INVALID_SLEEP_SRC
} pm_xo_sleep_clock_src_type;

/*============== POWER ON CONTROL TYPE DEFINITIONS ====================*/

/* Power Key hysteresis settings */
typedef enum
{
  PM_PWR_KEY_DELAY_EQ_0_msecs,         /*  0.00  ms */
  PM_PWR_KEY_DELAY_EQ_0p97_msecs,      /*  0.97  ms */
  PM_PWR_KEY_DELAY_EQ_1p95_msecs,      /*  1.95  ms */
  PM_PWR_KEY_DELAY_EQ_3p90_msecs,      /*  3.90  ms */
  PM_PWR_KEY_DELAY_EQ_7p81_msecs,      /*  7.81  ms */
  PM_PWR_KEY_DELAY_EQ_15p62_msecs,     /* 15.62  ms */
  PM_PWR_KEY_DELAY_EQ_31p25_msecs,     /* 31.25  ms */
  PM_PWR_KEY_DELAY_EQ_62p50_msecs,     /* 62.50  ms */
  PM_NUM_OF_PWR_KEY_DELAY              /* Number of power key hysteresis
                                          settings */
}pm_pwr_key_delay_type;

/* Event definition for PMIC power on status */
#define  PM_PWR_ON_EVENT_KEYPAD     0x1
#define  PM_PWR_ON_EVENT_RTC        0x2
#define  PM_PWR_ON_EVENT_CABLE      0x4
#define  PM_PWR_ON_EVENT_SMPL       0x8
#define  PM_PWR_ON_EVENT_WDOG       0x10
#define  PM_PWR_ON_EVENT_USB_CHG    0x20
#define  PM_PWR_ON_EVENT_WALL_CHG   0x40

/*======================= Vib Motor Definitions ===========================*/
typedef enum
{
  PM_VIB_MOT_MODE__MANUAL,
  PM_VIB_MOT_MODE__DBUS1,
  PM_VIB_MOT_MODE__DBUS2,
  PM_VIB_MOT_MODE__DBUS3,
  PM_VIB_MOT_MODE__INVALID
}pm_vib_mot_mode_type;

typedef enum
{
  PM_VIB_MOT_POL__ACTIVE_HIGH,
  PM_VIB_MOT_POL__ACTIVE_LOW,
  PM_VIB_MOT_POL__INVALID
}pm_vib_mot_pol_type;

/*======================= End of Vib Motor  ===============================*/

/*======================= Cable power On definitions ======================*/
typedef enum
{
  PM_CBL_PWR_ON__ENABLE,
  PM_CBL_PWR_ON__DISABLE,
  PM_CBL_PWR_ON__DIS_PERMANENTLY,
  PM_CBL_PWR_ON__INVALID
}pm_cbl_pwr_on_switch_type;

typedef enum
{
  PM_CBL_PWR_ON_PIN__0,
  PM_CBL_PWR_ON_PIN__1,
  PM_CBL_PWR_ON_PIN__INVALID
}pm_cbl_pwr_on_pin_type;

/*======================= End of Cable power on ===========================*/

/*======================= Headset Send End Detect Definitions ======================*/
typedef enum
{
  PM_HSED_SC_SWITCH_TYPE,
  PM_HSED_OC_SWITCH_TYPE,
  PM_HSED_SWITCH_TYPE__INVALID
}pm_hsed_switch_type;

typedef enum
{
  PM_HSED_SC_IDAC_300uA = 0,
  PM_HSED_SC_IDAC_400uA = 1,
  PM_HSED_SC_IDAC_500uA = 2,
  PM_HSED_SC_IDAC_600uA = 3,
  PM_HSED_SC_IDAC_700uA = 4,
  PM_HSED_SC_IDAC_800uA = 5,
  PM_HSED_SC_IDAC_900uA = 6,
  PM_HSED_SC_IDAC_1000uA = 7,
  PM_HSED_OC_IDAC_10uA = 0,
  PM_HSED_OC_IDAC_20uA = 1,
  PM_HSED_OC_IDAC_30uA = 2,
  PM_HSED_OC_IDAC_40uA = 3,
  PM_HSED_OC_IDAC_50uA = 4,
  PM_HSED_OC_IDAC_60uA = 5,
  PM_HSED_OC_IDAC_70uA = 6,
  PM_HSED_OC_IDAC_80uA = 7,
  PM_HSED_IDAC_TYPE__INVALID
}pm_hsed_idac_type;

typedef enum
{
  PM_HSED_ENABLE_OFF,
  PM_HSED_ENABLE_TCXO,
  PM_HSED_ENABLE_PWM_TCXO,
  PM_HSED_ENABLE_ALWAYS,
  PM_HSED_ENABLE_TYPE__INVALID
}pm_hsed_enable_type;

typedef enum
{
  PM_HSED_HYST_PRE_DIV_1,
  PM_HSED_HYST_PRE_DIV_2,
  PM_HSED_HYST_PRE_DIV_4,
  PM_HSED_HYST_PRE_DIV_8,
  PM_HSED_HYST_PRE_DIV_16,
  PM_HSED_HYST_PRE_DIV_32,
  PM_HSED_HYST_PRE_DIV_64,
  PM_HSED_HYST_PRE_DIV_128,
  PM_HSED_HYST_PRE_DIV_TYPE__INVALID
}pm_hsed_hyst_pre_div_type;

typedef enum
{
  PM_HSED_HYST_TIME_1_CLK_CYCLES,
  PM_HSED_HYST_TIME_2_CLK_CYCLES,
  PM_HSED_HYST_TIME_3_CLK_CYCLES,
  PM_HSED_HYST_TIME_4_CLK_CYCLES,
  PM_HSED_HYST_TIME_5_CLK_CYCLES,
  PM_HSED_HYST_TIME_6_CLK_CYCLES,
  PM_HSED_HYST_TIME_7_CLK_CYCLES,
  PM_HSED_HYST_TIME_8_CLK_CYCLES,
  PM_HSED_HYST_TIME_9_CLK_CYCLES,
  PM_HSED_HYST_TIME_10_CLK_CYCLES,
  PM_HSED_HYST_TIME_11_CLK_CYCLES,
  PM_HSED_HYST_TIME_12_CLK_CYCLES,
  PM_HSED_HYST_TIME_13_CLK_CYCLES,
  PM_HSED_HYST_TIME_14_CLK_CYCLES,
  PM_HSED_HYST_TIME_15_CLK_CYCLES,
  PM_HSED_HYST_TIME_16_CLK_CYCLES,
  PM_HSED_HYST_TIME_TYPE__INVALID
}pm_hsed_hyst_time_type;

typedef enum
{
  PM_HSED_PERIOD_PRE_DIV_2,
  PM_HSED_PERIOD_PRE_DIV_4,
  PM_HSED_PERIOD_PRE_DIV_8,
  PM_HSED_PERIOD_PRE_DIV_16,
  PM_HSED_PERIOD_PRE_DIV_32,
  PM_HSED_PERIOD_PRE_DIV_64,
  PM_HSED_PERIOD_PRE_DIV_128,
  PM_HSED_PERIOD_PRE_DIV_256,
  PM_HSED_PERIOD_PRE_DIV_TYPE__INVALID
}pm_hsed_period_pre_div_type;

typedef enum
{
  PM_HSED_PERIOD_TIME_1_CLK_CYCLES,
  PM_HSED_PERIOD_TIME_2_CLK_CYCLES,
  PM_HSED_PERIOD_TIME_3_CLK_CYCLES,
  PM_HSED_PERIOD_TIME_4_CLK_CYCLES,
  PM_HSED_PERIOD_TIME_5_CLK_CYCLES,
  PM_HSED_PERIOD_TIME_6_CLK_CYCLES,
  PM_HSED_PERIOD_TIME_7_CLK_CYCLES,
  PM_HSED_PERIOD_TIME_8_CLK_CYCLES,
  PM_HSED_PERIOD_TIME_9_CLK_CYCLES,
  PM_HSED_PERIOD_TIME_10_CLK_CYCLES,
  PM_HSED_PERIOD_TIME_11_CLK_CYCLES,
  PM_HSED_PERIOD_TIME_12_CLK_CYCLES,
  PM_HSED_PERIOD_TIME_13_CLK_CYCLES,
  PM_HSED_PERIOD_TIME_14_CLK_CYCLES,
  PM_HSED_PERIOD_TIME_15_CLK_CYCLES,
  PM_HSED_PERIOD_TIME_16_CLK_CYCLES,
  PM_HSED_PERIOD_TIME_TYPE__INVALID
}pm_hsed_period_time_type;

typedef enum 
{
    PM_HSED_MODULE_TYPE__0,
    PM_HSED_MODULE_TYPE__1,
    PM_HSED_MODULE_TYPE__2,
    PM_HSED_MODULE_TYPE__INVALID
} pm_hsed_module_type;

typedef enum
{
    PM_HSED_STATE_TYPE1_PRESS,
    PM_HSED_STATE_TYPE1_RELEASE,
    PM_HSED_STATE_TYPE1_INSERT,
    PM_HSED_STATE_TYPE1_REMOVE,
    PM_HSED_STATE_TYPE1_INVALID
} pm_hsed_state_type_1;

/*======================= End of Headset Send End Detect Definitions ===============*/
/*======================= PA_ON Controller Definitions =============================*/
typedef enum
{
    PM_PA_ON_CTRL_ENABLE_OFF,
    PM_PA_ON_CTRL_ENABLE_ON,
    PM_PA_ON_CTRL_ENABLE__INVALID
}pm_pa_on_cntrl_enable_type;

typedef enum
{
    PM_PA_ON_CTRL_CHANSET_WCDMA_PA_ON_0 = 0,
    PM_PA_ON_CTRL_CHANSET_WCDMA_PA_ON_1,
    PM_PA_ON_CTRL_CHANSET_WCDMA_PA_ON_2,
    PM_PA_ON_CTRL_CHANSET_WCDMA_PA_ON_3,
    PM_PA_ON_CTRL_CHANSET_GSM_DUAL_PA_ON_0,
    PM_PA_ON_CTRL_CHANSET_GSM_DUAL_PA_ON_1,
    PM_PA_ON_CTRL_CHANSET_GSM_BAND_SEL_LOW_AND_PA_ON,
    PM_PA_ON_CTRL_CHANSET_GSM_BAND_SEL_HIGH_AND_PA_ON,
    PM_PA_ON_CTRL_CHANSET__INVALID
}pm_pa_on_cntrl_channel_set_type;

typedef enum
{
    PM_PA_ON_CTRL_VLEVEL_1P8 = 0,
    PM_PA_ON_CTRL_VLEVEL_2P6,
    PM_PA_ON_CTRL_VLEVEL_2P85,
    PM_PA_ON_CTRL_VLEVEL__INVALID
}pm_pa_on_cntrl_vlevel_set_type;

/*======================= End of PA_ON Controller ===============================*/

/*======================= USIM_AUX Controller Definitions =======================*/
typedef enum
{
    PM_USIM_AUX_CTRL_VREG_MSME,
	PM_USIM_AUX_CTRL_VREG_RUIM,
    PM_USIM_AUX_CTRL_VREG__INVALID
}pm_usim_aux_cntrl_vreg_type;
/*======================= End of USIM_AUX Controller Definitions ================*/

/*======================= USB Overvoltage Protection (OVP) ======================*/
typedef enum
{
    PM_USB_OVP_THRESHOLD_5V5,
    PM_USB_OVP_THRESHOLD_6V,
    PM_USB_OVP_THRESHOLD_6V5,
    PM_USB_OVP_THRESHOLD_7V,
    PM_USB_OVP_THRESHOLD_INVALID
} pm_usb_ovp_threshold_type;

typedef enum
{
    PM_USB_OVP_HYSTERSIS_0ms,
    PM_USB_OVP_HYSTERSIS_20ms,
    PM_USB_OVP_HYSTERSIS_40ms,
    PM_USB_OVP_HYSTERSIS_80ms,
    PM_USB_OVP_HYSTERSIS_INVALID
} pm_usb_ovp_hystersis_type;

/*===================== End of USB Overvoltage Protection (OVP) ===================*/

/*============================ PWM Generation ====================================*/

typedef enum
{
    PM_PWM_GENERATOR_1,
    PM_PWM_GENERATOR_2,
    PM_PWM_GENERATOR_3,
    PM_PWM_GENERATOR_INVALID
} pm_pwm_generator_type;

typedef enum
{
    PM_PWM_GENERATOR_CLOCK_OFF,
    PM_PWM_GENERATOR_CLOCK_1KHz,
    PM_PWM_GENERATOR_CLOCK_32KHz,
    PM_PWM_GENERATOR_CLOCK_19_2MHz,
    PM_PWM_GENERATOR_CLOCK_INVALID
} pm_pwm_generator_clock_type;

typedef enum
{
    PM_PWM_GENERATOR_PRE_DIV_BY_2,
    PM_PWM_GENERATOR_PRE_DIV_BY_3,
    PM_PWM_GENERATOR_PRE_DIV_INVALID
} pm_pwm_generator_pre_div_type;

typedef enum
{
    PM_PWM_GENERATOR_PRE_DIV_EXP_0,
    PM_PWM_GENERATOR_PRE_DIV_EXP_1,
    PM_PWM_GENERATOR_PRE_DIV_EXP_2,
    PM_PWM_GENERATOR_PRE_DIV_EXP_3,
    PM_PWM_GENERATOR_PRE_DIV_EXP_4,
    PM_PWM_GENERATOR_PRE_DIV_EXP_5,
    PM_PWM_GENERATOR_PRE_DIV_EXP_6,
    PM_PWM_GENERATOR_PRE_DIV_EXP_7,
    PM_PWM_GENERATOR_PRE_DIV_EXP_INVALID
} pm_pwm_generator_pre_div_exp_type;

typedef enum
{
    PM_PWM_GENERATOR_SIZE_6_BIT,
    PM_PWM_GENERATOR_SIZE_9_BIT,
    PM_PWM_GENERATOR_SIZE_INVALID
} pm_pwm_generator_size_type;

/*========================= End of PWM Generation ================================*/

/*========================= 32 KHz clock Def =====================================*/
typedef enum
{
	PM_32K_CLK_BT_APP,   //vote used by Bluetooth app to vote 32kHz PMIC clock on or off
	PM_32K_CLK_FM_APP,
	PM_32K_CLK_RESERVED1_APP,
	PM_32K_CLK_RESERVED2_APP,
	PM_32K_CLK_INVALID_APP
} pm_32k_clk_app_vote_type;
/*========================= End 32 KHz clock Def ================================*/

/*========================== LEDs Type Def ================================*/
/* LCD LED Settings. */
/* 0 mA */
#define PM_LED_LCD_SETTING__LEVEL0    0
/* -10 mA */
#define PM_LED_LCD_SETTING__LEVEL1    1
/* -20 mA */
#define PM_LED_LCD_SETTING__LEVEL2    2
/* -30 mA */
#define PM_LED_LCD_SETTING__LEVEL3    3
/* -40 mA */
#define PM_LED_LCD_SETTING__LEVEL4    4

/* KBD LED Settings. */
/* 0 mA */
#define PM_LED_KBD_SETTING__LEVEL0    0
/* -10 mA */
#define PM_LED_KBD_SETTING__LEVEL1    1
/* -20 mA */
#define PM_LED_KBD_SETTING__LEVEL2    2
/* -30 mA */
#define PM_LED_KBD_SETTING__LEVEL3    3
/* -40 mA */
#define PM_LED_KBD_SETTING__LEVEL4    4

typedef enum
{
  PM_LCD_LED,
  PM_KBD_LED,
  PM_INVALID
}pm_led_intesity_type;

/*======================= End of LEDs Type Def ============================*/


/*======================= Flash LEDs Type Def =============================*/
typedef enum
{
  PM_FLASH_LED_MODE__MANUAL,
  PM_FLASH_LED_MODE__DBUS1,
  PM_FLASH_LED_MODE__DBUS2,
  PM_FLASH_LED_MODE__DBUS3,
  PM_FLASH_LED_MODE__INVALID
}pm_flash_led_mode_type;

typedef enum
{
  PM_FLASH_LED_POL__ACTIVE_HIGH,
  PM_FLASH_LED_POL__ACTIVE_LOW,
  PM_FLASH_LED_POL__INVALID
}pm_flash_led_pol_type;

/*=====================End of Flash LEDs Type Def =========================*/

/*====================== SPEAKER type definitions =========================*/
/* Turn the speaker on or off and enables or disables mute.*/
typedef enum
{
    PM_SPKR_DISABLE,  /* Enable Speaker                                 */
    PM_SPKR_ENABLE,   /* Disable Speaker                                */
    PM_SPKR_MUTE_OFF, /* turn speaker mute off, SOUND ON                */
    PM_SPKR_MUTE_ON,  /* turn speaker mute on, SOUND OFF                */
    PM_SPKR_OFF,      /* turn speaker OFF (speaker disable and mute on) */
    PM_SPKR_ON,        /* turn speaker ON (speaker enable and mute off)  */
    PM_SPKR_SET_FREQ_CMD,    /* set speaker frequency */
    PM_SPKR_GET_FREQ_CMD,    /* get speaker frequency */
    PM_SPKR_SET_GAIN_CMD,    /* set speaker gain */
    PM_SPKR_GET_GAIN_CMD,    /* get speaker gain */
    PM_SPKR_SET_DELAY_CMD,   /* set speaker delay */
    PM_SPKR_GET_DELAY_CMD,   /* get speaker delay */
    PM_SPKR_SET_PDM_MODE,
    PM_SPKR_SET_PWM_MODE,
    PM_SPKR_CMD_OUT_OF_RANGE /* Not valid */

}pm_spkr_cmd_type;

/* Valid gain values for the PMIC Speaker */
typedef enum
{
    PM_SPKR_GAIN_MINUS16DB,      /* -16 db */
    PM_SPKR_GAIN_MINUS12DB,      /* -12 db */
    PM_SPKR_GAIN_MINUS08DB,      /* -08 db */
    PM_SPKR_GAIN_MINUS04DB,      /* -04 db */
    PM_SPKR_GAIN_00DB,           /*  00 db */
    PM_SPKR_GAIN_PLUS04DB,       /* +04 db */
    PM_SPKR_GAIN_PLUS08DB,       /* +08 db */
    PM_SPKR_GAIN_PLUS12DB,       /* +12 db */
    PM_SPKR_GAIN_OUT_OF_RANGE    /* Not valid */
}pm_spkr_gain_type;

/* Valid delay values for the PMIC Speaker */
typedef enum
{
    PM_SPKR_DLY_10MS,            /* ~10  ms delay */
    PM_SPKR_DLY_100MS,           /* ~100 ms delay */
    PM_SPKR_DLY_OUT_OF_RANGE     /* Not valid */
}pm_spkr_dly_type;

/* Valid frequency values for the PMIC Speaker */
typedef enum
{
    PM_SPKR_FREQ_1_39KHZ,         /* 1.39 kHz */
    PM_SPKR_FREQ_0_64KHZ,         /* 0.64 kHz */
    PM_SPKR_FREQ_0_86KHZ,         /* 0.86 kHz */
    PM_SPKR_FREQ_0_51KHZ,         /* 0.51 kHz */
    PM_SPKR_FREQ_1_06KHZ,         /* 1.06 kHz */
    PM_SPKR_FREQ_0_57KHZ,         /* 0.57 kHz */
    PM_SPKR_FREQ_0_73KHZ,         /* 0.73 kHz */
    PM_SPKR_FREQ_0_47KHZ,         /* 0.47 kHz */
    PM_SPKR_FREQ_1_20KHZ,         /* 1.20 kHz */
    PM_SPKR_FREQ_0_60KHZ,         /* 0.60 kHz */
    PM_SPKR_FREQ_0_76KHZ,         /* 0.76 kHz */
    PM_SPKR_FREQ_0_49KHZ,         /* 0.49 kHz */
    PM_SPKR_FREQ_0_95KHZ,         /* 0.95 kHz */
    PM_SPKR_FREQ_0_54KHZ,         /* 0.54 kHz */
    PM_SPKR_FREQ_0_68KHZ,         /* 0.68 kHz */
    PM_SPKR_FREQ_0_45KHZ,         /* 0.45 kHz */
    PM_SPKR_FREQ_OUT_OF_RANGE     /* Not valid */
}pm_spkr_hpf_corner_freq_type;

typedef enum
{
    PM_LEFT_SPKR,
    PM_RIGHT_SPKR,
    PM_SPKR_OUT_OF_RANGE          /* Not valid */
}pm_spkr_left_right_type;


/* This type specifies Speaker higher level configuration settings*/

typedef PACKED struct
{
    boolean is_right_chan_en;

    boolean is_left_chan_en;

    boolean is_right_left_chan_added;

    boolean is_stereo_en;

    boolean is_usb_with_hpf_20hz;

    boolean is_mux_bypassed;

    boolean is_hpf_en;

    boolean is_sink_curr_from_ref_volt_cir_en;
 
} pm_spkr_config_mode_type;


/*===================== End of SPEAKER type defs ==========================*/


/*====================== MIC type definitions =========================*/
/* Valid voltage values for the PMIC MIC */
typedef enum
{
    PM_MIC_VOLT_2_00V,            /*  2.00 V  */
    PM_MIC_VOLT_1_93V,            /*  1.93 V  */
    PM_MIC_VOLT_1_80V,            /*  1.80 V  */
    PM_MIC_VOLT_1_73V,            /*  1.73 V  */
    PM_MIC_VOLT_OUT_OF_RANGE      /* Not valid */
}pm_mic_volt_type;

/*===================== End of MIC type defs ==========================*/

/*================= TEMPERATURE PROTECTION type definitions ===============*/
typedef enum
{
    PM_ITEMP_STAGE0,
    PM_ITEMP_STAGE1,
    PM_ITEMP_STAGE2,
    PM_ITEMP_STAGE3
}pm_item_stage_type;

typedef enum
{
    PM_ITEMP_ORIDE_STAGE2,
    PM_ITEMP_ORIDE_OUT_OF_RANGE
}pm_item_oride_type;

/*================= End of TEMPERATURE PROTECTION type defs ===============*/

/*======================== SMPL type definitions ==========================*/
/* Valid SMPL timeout values */
typedef enum
{
   PM_SMPL_TIMER__0500msec,
   PM_SMPL_TIMER__1000msec,
   PM_SMPL_TIMER__1500msec,
   PM_SMPL_TIMER__2000msec,
   PM_SMPL_TIMER__INVALID
}pm_smpl_timer_type;

/*========================== End of SMPL type defs =======================*/
/*========================== MINIMUM P ON TIME type definitions =======================*/
typedef enum
{
   PM_MIN_P_ON_TIME_35, // Min time is 35 ns
   PM_MIN_P_ON_TIME_46, // Min time is 46 ns
   PM_MIN_P_ON_TIME_68, 
   PM_MIN_P_ON_TIME_119, 
   PM_MIN_P_ON_TIME_155, 
   PM_MIN_P_ON_TIME_188, 
   PM_MIN_P_ON_TIME_251, 
   PM_MIN_P_ON_TIME_307,
   PM_MIN_P_ON_TIME_OUT_OF_RANGE
}pm_min_on_time_type;
/*========================== End of MINIMUM P ON TIME type definitions =======================*/
/*========================== BUCK SMPS TYPE  =======================*/
typedef enum
{
  BUCK_SMPS_S1,     // BUCK SMPS 1
  BUCK_SMPS_S2,
  BUCK_SMPS_INVALID  
}buck_smps_type;




/*========================== End of BUCK SMPS TYPE  =======================*/



/*===========================================================================

                     GLOBAL VARIABLE PROTOTYPES

===========================================================================*/


/*===========================================================================

                      FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================

FUNCTION pm_init_comm                                EXTERNAL FUNCTION

DESCRIPTION
    This function initializes the communication interface with PMIC.

INPUT PARAMETERS
  1) Parameter name: pm_model
     - Which PMIC

     Parameter type: pm_model_type
     - Valid Inputs:
            PMIC_IS_PM6000
            PMIC_IS_PM6050

RETURN VALUE
  Return value type: pm_err_flag_type

    PM_ERR_FLAG__SUCCESS            = We were successful setting the PMIC
                                      communication interface.
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE  = Input Parameter one is out of range.
    PM_ERR_FLAG__SBI_OPT_ERR,       = The SBI driver failed to communicate
                                      with the PMIC.
    PM_ERR_FLAG__DEV_MISMATCH       = There is a mismatch between the PMIC
                                      selected and the one detected.

DEPENDENCIES
  Call the functions listed bellow before calling this function.
    1) rflib_init()

  THIS FUNCTION MUST BE THE FIRST FUNCTION CALLED INSIDE "pm_init()" !!!!!

SIDE EFFECTS
  None

===========================================================================*/
pm_err_flag_type pm_init_comm(void);

/*===========================================================================

FUNCTION pm_soft_reset                                EXTERNAL FUNCTION

DESCRIPTION
  NO LONGER SUPPORTED.
    
INPUT PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  NO LONGER SUPPORTED. 

SIDE EFFECTS
  None

===========================================================================*/
void pm_soft_reset(void);

/*===========================================================================

FUNCTION pm_hard_reset                                EXTERNAL FUNCTION

DESCRIPTION
  This function will reset all the PMIC HW blocks except the RTC.
    
INPUT PARAMETERS
  None.

RETURN VALUE
  Return value type: pm_err_flag_type.
    - PM_ERR_FLAG__SUCCESS               = SUCCESS.
    - PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to 
                                           communicate with the PMIC.    

DEPENDENCIES
  Call the functions listed bellow before calling this function.
    1) pm_init_comm()

SIDE EFFECTS
  None
===========================================================================*/
pm_err_flag_type pm_hard_reset(void);

/*===========================================================================
FUNCTION pm_start_up_abort_timer_switch            EXTERNAL FUNCTION  

DESCRIPTION
    This function enables/disables the PMIC's start up abort feature. The 
  start up abort feature allows the PMIC to give up trying to power on the 
  phone if it is not able to do so (MSM will not bring PS_HOLD high) 
  after 1 to 2 seconds.
  EXAMPLE:
  // Disable the start up abort feature. Let the PMIC keep trying to
  // power up the handset. even if it has not been successful after 2 secs.
  err |= pm_start_up_abort_timer_switch(PM_OFF_CMD);
  // Find out if there was an PMIC API error. 
  if (err != PM_ERR_FLAG__SUCCESS) 
  {
      MSG_ERROR(" PMIC API ERROR(0x%x).",err,0,0);
  }
INPUT PARAMETERS
  Parameter name: OnOff.
  - Enable/disable start up abort feature.
  Parameter type: pm_switch_cmd_type.
  - Valid inputs:
        PM_ON_CMD  = Enable (default).
        PM_OFF_CMD = Disable.
RETURN VALUE
  Return value type: pm_err_flag_type.
    - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter 1 is out of range.
    - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                           version of the PMIC.               
    - PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to 
                                           communicate with the PMIC.    
    - PM_ERR_FLAG__SUCCESS               = SUCCESS.
DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()
SIDE EFFECTS
  Interrupts are LOCKED while we are communicating with the PMIC.
===========================================================================*/
pm_err_flag_type pm_start_up_abort_timer_switch( \
                                                pm_switch_cmd_type OnOff);
/*===========================================================================
FUNCTION pm_watchdog_reset_detect_switch            EXTERNAL FUNCTION  
DESCRIPTION
    This function enables/disables the PMIC's watchdog reset detect 
  feature. If this feature is enabled then the PMIC resets and restarts
  if PS_HOLD pin goes low. It will also trigger an IRQ if the Watchdog 
  Timeout IRQ (PM_WDOG_TOUT_IRQ_HDL) is enabled. If this feature is 
  disabled then the PMIC shuts off if PS_HOLD pin goes low. The IRQ will 
  not trigger even if enabled.
  EXAMPLE:
  // Enable/Disable the watchdog reset detect feature. 
  err |= pm_watchdog_reset_detect_switch(PM_ON_CMD);
  // Find out if there was an PMIC API error. 
  if (err != PM_ERR_FLAG__SUCCESS) 
  {
      MSG_ERROR(" PMIC API ERROR(0x%x).",err,0,0);
  }
INPUT PARAMETERS
  Parameter name: OnOff.
  - Enable/disable wathdog reset detect feature.
  Parameter type: pm_switch_cmd_type.
  - Valid inputs:
        PM_ON_CMD  = Enable.
        PM_OFF_CMD = Disable (default).
RETURN VALUE
  Return value type: pm_err_flag_type.
    - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter 1 is out of range.
    - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                           version of the PMIC.               
    - PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to 
                                           communicate with the PMIC.    
    - PM_ERR_FLAG__SUCCESS               = SUCCESS.
DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()
SIDE EFFECTS
  Interrupts are LOCKED while we are communicating with the PMIC.
===========================================================================*/
pm_err_flag_type pm_watchdog_reset_detect_switch( \
                                                  pm_switch_cmd_type OnOff);
/*===========================================================================
                        TCXO CONTROL DRIVER PROTOTYPES
===========================================================================*/

/*===========================================================================

FUNCTION FUNCTION pm_config_tcxo_ctrl                     EXTERNAL FUNCTION

DESCRIPTION
    This function selects TCXO buffer turn ON delay and the polarity of
  TCXO_EN pin.

INPUT PARAMETERS
  1) Parameter name: polarity.
     - Set TCXO_EN pin polarity.

     - Valid Input:
         PM_POLARITY_ACTIVE_HIGH = Set TCXO_EN pin to active high.
         PM_POLARITY_ACTIVE_LOW  = Set TCXO_EN pin to active Low.

  2) Parameter name: delay.
     - The number of sleep clock periods before the TCXO turns ON.

       Parameter type: uint16.
     - Valid Range:
       0x003 to 0x802

RETURN VALUE
  PM_ERR_FLAG__SUCCESS               = If no error were detected
                                       during the pfunction exec.
  PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = The TCXO buffer delay value is invalid.
  PM_ERR_FLAG__PAR2_OUT_OF_RANGE     = The polarity value is invalid.
  PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                       version of the PMIC.  
  PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to
                                       communicate with the PMIC.

DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
  Interrupts are disabled while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type pm_config_tcxo_ctrl(pm_polarity_type polarity,
                                            uint16           delay_cycles);

/*===========================================================================

FUNCTION FUNCTION pm_tcxo_set_drive_strength                EXTERNAL FUNCTION

DESCRIPTION
    This function sets the TCXO buffer output drive strenght to either high
or low 

INPUT PARAMETERS
  1) Parameter name: drive_strength

     - Valid Input:
            See type definition for pm_tcxo_drive_strength_type

RETURN VALUE
  PM_ERR_FLAG__SUCCESS               = If no error were detected
                                       during the pfunction exec.
  PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = drive_strength is invalid 
  PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                       version of the PMIC.  
  PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to
                                       communicate with the PMIC.

DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
  Interrupts are disabled while communicating with the PMIC.

===========================================================================*/
/* set drive strength for XO_OUT */
pm_err_flag_type pm_tcxo_set_drive_strength(pm_tcxo_drive_strength_type drive_strength);

/* set drive strength for XO_GP1_OUT */
pm_err_flag_type pm_tcxo2_set_drive_strength(pm_tcxo_drive_strength_type drive_strength);

/*===========================================================================

FUNCTION FUNCTION pm_tcxo_cmd                          EXTERNAL FUNCTION

DESCRIPTION
  This function allows the user to:

  1) Set the TCXO controller in Manual mode:
        * User has control over turning TCXO buffer ON/OFF
          ex: "pm_tcxo_cmd(PM_TCXO_BUFFER_ON_CMD);"
        * User has control over enabling/disabling TCXO LDO
          ex: "pm_ldo_control(PM_ON_CMD, PM_TCXO_LDO);"
        * User can also choose to emulate TCXO_EN pin control
          available in AUTOMATIC mode
          ex: "pm_tcxo_cmd(PM_TCXO_EMULATE_TCXO_EN_ON_CMD);"
  2) Set the TCXO controller in Automatic mode
        * TCXO is controlled by the TCXO_EN pin which controls
          TCXO LDO as well as the buffer automatically
        * TCXO buffer gets enabled upon the expiry of 'delay_cycles'
          number of sleep clock cycles since driving the TCXO_EN pin
  3) Turn TCXO buffer ON/OFF: If MANUAL mode has been selected,
     it allows the user to turn the TCXO buffer ON/OFF
  4) Emulate hardware control of TCXO_EN pin in software when
     MANUAL mode has been chosen.

INPUT PARAMETERS
  1) Parameter name: cmd
     Parameter type: pm_tcxo_cmd_type
     - Valid Input:

         PM_TCXO_MANUAL_MODE_CMD          = TCXO Controller is controlled
                                            by software.
         PM_TCXO_AUTOMATIC_MODE_CMD       = TCXO Controller is controlled
                                            by TCXO_EN pin.
         PM_TCXO_BUFFER_ON_CMD            = Turn the TCXO buffer ON.
         PM_TCXO_BUFFER_OFF_CMD           = Turn the TCXO buffer OFF.
         PM_TCXO_EMULATE_TCXO_EN_ACTIVE_CMD  = this MANUAL mode command has the same
                                               effect as that of driving TCXO_EN pin
                                               active in AUTOMATIC mode, i.e. it turns
                                               TCXO LDO on and after the programmed
                                               delay, enables the TCXO buffer
         PM_TCXO_EMULATE_TCXO_EN_INACTIVE_CMD  = this MANUAL mode command has the same
                                                 effect as that of driving TCXO_EN pin
                                                 inactive in AUTOMATIC mode, i.e. it
                                                 turns TCXO LDO off and disables the
                                                 TCXO buffer
     2) Parameter name: delay_cycles
      Parameter type: uint16
      Valid range: 0..2047


RETURN VALUE
  PM_ERR_FLAG__SUCCESS               = The operation was successful.
  PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Parameter 1 is out of range.
  PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                       version of the PMIC.
  PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to
                                       communicate with the PMIC.

DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
  Interrupts are disabled while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type pm_tcxo_cmd(pm_tcxo_cmd_type cmd);

/*===========================================================================

FUNCTION FUNCTION pm_tcxo2_cmd                          EXTERNAL FUNCTION

DESCRIPTION
  This function allows the user to configure the second TCXO controller:

  1) Set the second TCXO controller in Manual mode:
        * User has control over turning TCXO buffer ON/OFF
          ex: "pm_tcxo_cmd(PM_TCXO_BUFFER_ON_CMD);"
        * User has control over enabling/disabling TCXO LDO
          ex: "pm_ldo_control(PM_ON_CMD, PM_TCXO_LDO);"
        * User can also choose to emulate TCXO_EN pin control
          available in AUTOMATIC mode
          ex: "pm_tcxo_cmd(PM_TCXO_EMULATE_TCXO_EN_ON_CMD);"
  2) Set the second TCXO controller in Automatic mode
        * TCXO is controlled by the TCXO_EN pin which controls
          TCXO LDO as well as the buffer automatically
        * TCXO buffer gets enabled upon the expiry of 'delay_cycles'
          number of sleep clock cycles since driving the TCXO_EN pin
  3) Turn TCXO buffer ON/OFF: If MANUAL mode has been selected,
     it allows the user to turn the TCXO buffer ON/OFF
  4) Emulate hardware control of TCXO_EN pin in software when
     MANUAL mode has been chosen.

INPUT PARAMETERS
  1) Parameter name: cmd
     Parameter type: pm_tcxo_cmd_type
     - Valid Input:

         PM_TCXO_MANUAL_MODE_CMD          = TCXO Controller is controlled
                                            by software.
         PM_TCXO_AUTOMATIC_MODE_CMD       = TCXO Controller is controlled
                                            by TCXO_EN pin.
         PM_TCXO_BUFFER_ON_CMD            = Turn the TCXO buffer ON.
         PM_TCXO_BUFFER_OFF_CMD           = Turn the TCXO buffer OFF.
         PM_TCXO_EMULATE_TCXO_EN_ACTIVE_CMD  = this MANUAL mode command has the same
                                               effect as that of driving TCXO_EN pin
                                               active in AUTOMATIC mode, i.e. it turns
                                               TCXO LDO on and after the programmed
                                               delay, enables the TCXO buffer
         PM_TCXO_EMULATE_TCXO_EN_INACTIVE_CMD  = this MANUAL mode command has the same
                                                 effect as that of driving TCXO_EN pin
                                                 inactive in AUTOMATIC mode, i.e. it
                                                 turns TCXO LDO off and disables the
                                                 TCXO buffer
     2) Parameter name: delay_cycles
      Parameter type: uint16
      Valid range: 0..2047


RETURN VALUE
  PM_ERR_FLAG__SUCCESS               = The operation was successful.
  PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Parameter 1 is out of range.
  PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                       version of the PMIC.
  PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to
                                       communicate with the PMIC.

DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
  Interrupts are disabled while communicating with the PMIC.
===========================================================================*/
pm_err_flag_type pm_tcxo2_cmd(pm_tcxo_cmd_type cmd);

/*===========================================================================
                 XO CONTROL FUNCTION PROTOTYPES
===========================================================================*/
/*===========================================================================

FUNCTION FUNCTION pm_xo_enable                     EXTERNAL FUNCTION

DESCRIPTION
    This function enables/disables the 19.2MHz Crystal Oscillator

INPUT PARAMETERS
  1) Parameter name: xo_enable.
     - Type: Boolean
     - Valid Input:
         TRUE = enable the 19.2MHz Crystal Oscillator
         FALSE = disable the 19.2MHz Crystal Oscillator

RETURN VALUE
  PM_ERR_FLAG__SUCCESS               = The operation was successful.
  PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Parameter 1 is out of range.
  PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                       version of the PMIC.
  PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to

DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
  Interrupts are disabled while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type pm_xo_enable(boolean xo_enable);

/*===========================================================================

FUNCTION FUNCTION pm_xo_sel_alt_sleep_clk_src               EXTERNAL FUNCTION

DESCRIPTION
    This function selects alternate sleep clock, or use the 19.2MHz crystal
    oscillator to generate a sleep clock 

INPUT PARAMETERS
  1) Parameter name: alt_clk_src.
     - Type: pm_xo_sleep_clock_src_type
     - Valid Input:
         PM_XO_32KHZ_SLEEP_CLOCK_XTAL_OSC = use the low power 32kHz sleep clock 
                                            crystal oscillator 
         PM_XO_19_2MHZ_XTAL_OSC = use the 19.2MHz XO to derive the sleep clock

RETURN VALUE
  PM_ERR_FLAG__SUCCESS               = The operation was successful.
  PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Parameter 1 is out of range.
  PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                       version of the PMIC.
  PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to

DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
  Interrupts are disabled while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type pm_xo_sel_alt_sleep_clk_src(pm_xo_sleep_clock_src_type alt_clk_src);

/*===========================================================================

FUNCTION FUNCTION pm_xo_rfbuf_enable                     EXTERNAL FUNCTION

DESCRIPTION
    This function enables/disables the RF TCXO buffer 

INPUT PARAMETERS
  1) Parameter name: rfbuf_enable.
     - Type: Boolean
     - Valid Input:
         TRUE = enable the RF TXCO buffer 
         FALSE = disable the RF TXCO buffer

RETURN VALUE
  PM_ERR_FLAG__SUCCESS               = The operation was successful.
  PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Parameter 1 is out of range.
  PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                       version of the PMIC.
  PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to

DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
  Interrupts are disabled while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type pm_xo_rfbuf_enable(boolean rfbuf_enable);

/*===========================================================================

FUNCTION FUNCTION pm_xo_boost_enable                     EXTERNAL FUNCTION

DESCRIPTION
    This function enables/disables the boost of XO core AGC gain

INPUT PARAMETERS
  1) Parameter name: boost_enable.
     - Type: Boolean
     - Valid Input:
         TRUE = boost the XO core AGC gain
         FALSE = don't boost the XO core AGC gain 

RETURN VALUE
  PM_ERR_FLAG__SUCCESS               = The operation was successful.
  PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Parameter 1 is out of range.
  PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                       version of the PMIC.
  PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to

DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
  Interrupts are disabled while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type pm_xo_boost_enable(boolean boost_enable);

/*===========================================================================

FUNCTION FUNCTION pm_xo_set_xo_trim                     EXTERNAL FUNCTION

DESCRIPTION
    This function trims the 19.2MHz XO load capacitances 

INPUT PARAMETERS
  1) Parameter name: trim_value
     - Type: uint8
     - Raw trim value to be written in the range of [0, 63] 

RETURN VALUE
  PM_ERR_FLAG__SUCCESS               = The operation was successful.
  PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                       version of the PMIC.
  PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to

DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
  Interrupts are disabled while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type
pm_xo_set_xo_trim(uint8 trim_value);

/*===========================================================================

FUNCTION FUNCTION pm_xo_get_xo_trim                     EXTERNAL FUNCTION

DESCRIPTION
    This function gets the raw trim value of the load capacitances for
the 19.2MHz XO

INPUT PARAMETERS
  None.

RETURN VALUE
  1) Raw trim value
     - Type: uint8
     - Valid value in the range of [0, 63] 
     - Value of 0xFF is an error code that indicates a read failure

DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
  Interrupts are disabled while communicating with the PMIC.

===========================================================================*/
uint8 pm_xo_get_xo_trim(void);

/*===========================================================================

FUNCTION FUNCTION pm_xo_get_cap_step_size                  EXTERNAL FUNCTION

DESCRIPTION
    This function gets the step size of the load capacitances in femto (10^-15)
Faraday for the 19.2MHz XO

INPUT PARAMETERS
  None.

RETURN VALUE
  1) Step size of load capacitance in femto (10^-15) Faraday
     - Type: uint8

DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
  Interrupts are disabled while communicating with the PMIC.

===========================================================================*/
uint8 pm_xo_get_cap_step_size(void);

/*===========================================================================

                 MBG LOW POWER MODE CONTROL FUNCTION PROTOTYPES
                 
===========================================================================*/
/*===========================================================================

FUNCTION FUNCTION pm_mbg_lpm_enable                     EXTERNAL FUNCTION

DESCRIPTION
    This function enables/disables MBG low power mode 

INPUT PARAMETERS
  1) Parameter name: lp_en.
     - Type: Boolean

  2) Parameter name: lp_txco_en.
     - Type: Boolean

    lp_en   lp_txco_en  |   Function
      0         X       |   low power bandgap is off 
                        |   normal power bandgap is on 
                        |   reference outputs from normal bandgap
    --------------------------------------------------------------------------------
      1         0       |   low power bandgap is on
                        |   normal power bandgap is off
                        |   reference outputs from low power bandgap
    --------------------------------------------------------------------------------
      1         1       |   low power bandgap is on
                        |   normal bandgap is on when tcxo_en is high, off otherwise
                        |   refernce outputs from low power bandgap if
                        |   tcxo_en is low; from normal bandgap otherwise

RETURN VALUE
  PM_ERR_FLAG__SUCCESS               = The operation was successful.
  PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Parameter 1 is out of range.
  PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                       version of the PMIC.
  PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to

DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
  Interrupts are disabled while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type pm_mbg_lpm_enable(boolean lp_en, boolean lp_tcxo_en);

/*===========================================================================

FUNCTION FUNCTION pm_mbg_lpm_trim                     EXTERNAL FUNCTION

DESCRIPTION
    This function controls the trimming of the low power bandgap.
    Requires both lp_en and tcxo_en high.

INPUT PARAMETERS
    NONE

RETURN VALUE
  PM_ERR_FLAG__SUCCESS               = The operation was successful.
  PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Parameter 1 is out of range.
  PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                       version of the PMIC.
  PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to

DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
  Interrupts are disabled while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type pm_mbg_lpm_trim(void);

/*===========================================================================

FUNCTION FUNCTION pm_mbg_lpm_dec_ref_byp_r                 EXTERNAL FUNCTION

DESCRIPTION
    This function decreases/increases the series resistor with the reference 
bypass capacitor for MBG low power mode 

INPUT PARAMETERS
  1) Parameter name: decrease_R.
     - Type: Boolean
     - Valid Input:
         TRUE = decreases the series resistor to 75k Ohms
         FALSE = increases the series resistor to 500k Ohms

DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
  Interrupts are disabled while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type pm_mbg_lpm_dec_ref_byp_r(boolean decrease_R);

/*===========================================================================

FUNCTION FUNCTION pm_mbg_lpm_force_nbias_sleep           EXTERNAL FUNCTION

DESCRIPTION
    This function forces/don't force nbias for MBG low power mode when the
phone is in sleep mode. Forcing nbias is necessary if either MPP, LED
drivers, or vibration motor driver will be used in sleep mode.

INPUT PARAMETERS
  1) Parameter name: force_nbias.
     - Type: Boolean
     - Valid Input:
         TRUE = Force nbias for MBG LPM in sleep mode 
         FALSE = Don't force nbias for MBG LPM in sleep mode 

DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
  Interrupts are disabled while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type pm_mbg_lpm_force_nbias_sleep(boolean force_nbias);

/*===========================================================================

FUNCTION FUNCTION pm_clockless_pfm_enable                  EXTERNAL FUNCTION

DESCRIPTION
    This function enables/disables clockless PFM mode, for PM6640 or 
    Panoramix only 

INPUT PARAMETERS
  1) Parameter name: Buck VREG ID 
     - Type: pm_vreg_buck_id_type
        For PM7500: PM_VREG_BUCK_MSMC1_ID, PM_VREG_BUCK_MSMC2_ID,
                    PM_VREG_BUCK_MSME_ID, PM_VREG_BUCK_PA_ID
        For PM6640: PM_VREG_BUCK_MSMC1_ID, PM_VREG_BUCK_MSME_ID
        For Panoramix: PM_VREG_BUCK_MSMC1_ID

  2) Parameter name: enable
     - Type: Boolean
        True = Enable clockless PFM mode
        False = Disable clockless PFM mode

RETURN VALUE
  PM_ERR_FLAG__SUCCESS               = The operation was successful.
  PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Parameter 1 is out of range.
  PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                       version of the PMIC.
  PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to

DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
  Interrupts are disabled while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type pm_clockless_pfm_enable(pm_vreg_buck_id_type buck_vreg,
                                                boolean enable);


/*===========================================================================
                 POWER ON CONTROL FUNCTION PROTOTYPES
===========================================================================*/
/*===========================================================================

FUNCTION FUNCTION pm_power_key_setup                   EXTERNAL FUNCTION

DESCRIPTION
     Use this function to configure the power key pin.

INPUT PARAMETERS
  1) Parameter name: delay.
     - Power key state change IRQ hysteresis value.

     Parameter type: pm_pwr_key_delay_type.
        PM_PWR_KEY_DELAY_EQ_0_msecs          =  0.00  ms
        PM_PWR_KEY_DELAY_EQ_0p97_msecs       =  0.97  ms
        PM_PWR_KEY_DELAY_EQ_1p95_msecs       =  1.95  ms
        PM_PWR_KEY_DELAY_EQ_3p90_msecs       =  3.90  ms
        PM_PWR_KEY_DELAY_EQ_7p81_msecs       =  7.81  ms
        PM_PWR_KEY_DELAY_EQ_15p62_msecs      = 15.62  ms
        PM_PWR_KEY_DELAY_EQ_31p25_msecs      = 31.25  ms
        PM_PWR_KEY_DELAY_EQ_62p50_msecs      = 62.50  ms
        
  2) Parmeter name: pull_up_en
     - Enable/disable the power key pull up.
     
     Parameter type: pm_switch_cmd_type (enum)
       PM_OFF_CMD = Disable Pull Up
       PM_ON_CMD  = Enable Pull Up
             

RETURN VALUE
  Return Value Type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__PAR2_OUT_OF_RANGE     = Input Parameter two is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
  Interrupts are disabled while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type pm_power_key_setup(pm_pwr_key_delay_type delay,
                                           pm_switch_cmd_type    pull_up_en);

/*===========================================================================

FUNCTION pm_get_power_on_status                   EXTERNAL FUNCTION

DESCRIPTION
    This function returns the phone power-on status, and it should be called
at the earliest possible moment at boot up

INPUT PARAMETERS
  1) Parameter name: *pwr_on_status
     - pointer to 32-bit unsigned integer that stores the power on status
    including keypad power on, RTC alarm trigger, cable power on, SMPL, 
    watchdog timeout, wall charger and USB charger with definitions as the 
    following: 

        PM_PWR_ON_EVENT_KEYPAD     0x1
        PM_PWR_ON_EVENT_RTC        0x2
        PM_PWR_ON_EVENT_CABLE      0x4
        PM_PWR_ON_EVENT_SMPL       0x8
        PM_PWR_ON_EVENT_WDOG       0x10
        PM_PWR_ON_EVENT_USB_CHG    0x20
        PM_PWR_ON_EVENT_WALL_CHG   0x40


RETURN VALUE
  Return Value Type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
  Interrupts are disabled while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type pm_get_power_on_status(uint32* pwr_on_status);

/*===========================================================================

FUNCTION pm_clear_power_on_status                   EXTERNAL FUNCTION

DESCRIPTION
    This function clears the phone power-on status, and it should be called
before the phone powers down.

INPUT PARAMETERS: None

RETURN VALUE
  Return Value Type: pm_err_flag_type.

    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
  Interrupts are disabled while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type pm_clear_power_on_status(void);

/*===========================================================================

                 LED DRIVER FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================

FUNCTION pm_set_led_intensity                    EXTERNAL FUNCTION

DESCRIPTION
    This function sets the selected LED driver signal to one of fifteen
  intensities.

INPUT PARAMETERS
  1) Parameter name: led.
     -  Which LED

     Parameter type: pm_led_intesity_type (enum).
     -  Valid inputs:
            PM_LCD_LED = LCD LED.
            PM_KBD_LED = Keyboard LED.

  2) Parameter name: val.
     -  0 -> 15, where 0 disables the driver and 15 is the highest intensity.

     Parameter type: uint8

      Value Current Units
        0   0       mA
        1   -10     mA
        2   -20     mA
        3   -30     mA
        4   -40     mA
        5   -50     mA
        6   -60     mA
        7   -70     mA
        8   -80     mA
        9   -90     mA
        10  -100    mA
        11  -110    mA
        12  -120    mA
        13  -130    mA
        14  -140    mA
        15  -150    mA

RETURN VALUE
  return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__PAR2_OUT_OF_RANGE     = Input Parameter two is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC.     
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type pm_set_led_intensity(pm_led_intesity_type led,
                                             uint8                val);
/*~ FUNCTION pm_set_led_intensity */

/*===========================================================================
FUNCTION pm_get_led_intensity                    EXTERNAL FUNCTION

DESCRIPTION
    This function returns the selected LED driver setting.
    
INPUT PARAMETERS
  1) Parameter name: led.
     -  Which LED

     Parameter type: pm_led_intesity_type (enum).
     -  Valid inputs:
            PM_LCD_LED = LCD LED.
            PM_KBD_LED = Keyboard LED.

  2) Parameter name: val.
     -  The selected LED driver setting.

     Parameter type: uint8 (pointer)

RETURN VALUE
  return value type: pm_err_flag_type.

    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC.     
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter 1 is out of range.
    PM_ERR_FLAG__PAR2_OUT_OF_RANGE     = Invalid pointer.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  rflib_init() and pm_init() must have been called.

SIDE EFFECTS
   None
===========================================================================*/
pm_err_flag_type pm_get_led_intensity(pm_led_intesity_type led, 
                                             uint8 *val);
/*===========================================================================

               FLASH LED DRIVER FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION pm_flash_led_set_current                       EXTERNAL FUNCTION

DESCRIPTION
  This function configures the current setting for the Flash LED.
  
  EXAMPLE:
     // Configure the Flash LED current to 280 mA.
     err = pm_flash_led_set_volt(280);     
  
INPUT PARAMETERS  
  1) Parameter name: current
     - current setting in mA.

     Parameter type: uint16.
     -  Valid inputs:
        1) 0 mA  (off, default).
        2) 0 mA to 300 mA, in 20 mA steps.                     

RETURN VALUE
  return value type: pm_err_flag_type.
  
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC. 
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.                                                                            
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init() 
  2) pm_init()

SIDE EFFECTS
  Interrupts are disabled while writing and reading to/from PMIC.

===========================================================================*/
pm_err_flag_type pm_flash_led_set_current (uint16 current);
/*~ FUNCTION pm_flash_led_set_current */

/*===========================================================================

FUNCTION pm_flash_led_set_mode                          EXTERNAL FUNCTION

DESCRIPTION
    This function allows the user to configure the Flash LED to manual 
  mode or automatic mode, where:
  
    1) Manual Mode: 
           1.1 Disable by setting the Flash LED current to 0 mA. 
           1.2 Enable by setting the Flash LED current to a setting
               within the valid current range. 
               
                                      
    2) Automatic Mode:
           2.1 The Flash LED gets enable. Where the current = 
               pm_flash_led_set_current() setting.
           2.2 Disable depending on the logic state of the 
               corresponding DBUS line. 0 mA regardless of the 
               pm_flash_led_set_current() setting.               
                           
    NOTES: 
          Flash LED current: 
               Please refer to the function description of 
               "pm_flash_led_set_current()".
                         
          DBUS polarity:
               Please refer to the function description of 
               "pm_flash_led_set_polarity()".
               
          DBUS: 
               The user can configure a MPP to drive a DBUS line. 
               Please refer to the function description of
               "pm_mpp_config_digital_input()".
               
    
    EXAMPLE:

        // Disable the Flash LED.
        err  = pm_flash_led_set_current(0);
                
        // Configure MPP5 to be a digital input, set the voltage reference
        // to MSMP VREG and drive data bus 2 (DBUS2).
        err |= pm_mpp_config_digital_input(PM_MPP_5, 
                                              PM_MPP__DLOGIC__LVL_MSMP,
                                                  PM_MPP__DLOGIC_IN__DBUS2);
        
        // Configure Flash LED to automatic and use DBUS2 as the 
        // control line.
        err |= pm_flash_led_set_mode(PM_FLASH_LED_MODE__DBUS2);
        
        // Configure DBUS polarity to active high. 
        err |= pm_flash_led_set_polarity(PM_FLASH_LED_POL__ACTIVE_HIGH);
        
        // Configure the Flash LED to 280 mA.
        err |= pm_flash_led_set_current(280);        
        
        // At this point, the Flash LED will turn on (280 mA) if MPP5 is
        // high and turn off if MPP5 is low.
                
        
INPUT PARAMETERS  
  1) Parameter name: mode 
     - Configure the Flash LED mode

     Parameter type: pm_flash_led_mode_type.
     - Valid inputs:
     
        PM_FLASH_LED_MODE__MANUAL = Manual Mode.
        PM_FLASH_LED_MODE__DBUS1  = Automatic Mode, Controlled by DBUS1 line.
        PM_FLASH_LED_MODE__DBUS2  = Automatic Mode, Controlled by DBUS2 line.
        PM_FLASH_LED_MODE__DBUS3  = Automatic Mode, Controlled by DBUS3 line.     

RETURN VALUE
  return value type: pm_err_flag_type.  
  
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC. 
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.                                                                            
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.
    
DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init() 
  2) pm_init()
  3) pm_flash_led_set_current()
  4) pm_flash_led_set_polarity()
  5) pm_mpp_config_digital_input()
                                       
SIDE EFFECTS
  Interrupts are disabled while writing and reading to/from PMIC.

===========================================================================*/
pm_err_flag_type pm_flash_led_set_mode (pm_flash_led_mode_type mode);
/*~ FUNCTION pm_flash_led_set_mode */

/*===========================================================================

FUNCTION pm_flash_led_set_polarity                       EXTERNAL FUNCTION

DESCRIPTION
    If the Flash LED is in automatic mode (refer to 
  pm_flash_led_set_mode()) then this function configures the polarity of 
  the dbus line.
  
    EXAMPLE:

        // Disable the Flash LED.
        err  = pm_flash_led_set_current(0);
                
        // Configure MPP3 to be a digital input, set the voltage reference
        // to MSMP VREG and drive data bus 1 (DBUS1).
        err |= pm_mpp_config_digital_input(PM_MPP_3, 
                                              PM_MPP__DLOGIC__LVL_MSMP,
                                                  PM_MPP__DLOGIC_IN__DBUS1);
        
        // Configure Flash LED to automatic and use DBUS1 as the 
        // control line.
        err |= pm_flash_led_set_mode(PM_FLASH_LED_MODE__DBUS1);
        
        // Configure DBUS1 polarity to active low. 
        err |= pm_flash_led_set_polarity(PM_FLASH_LED_POL__ACTIVE_LOW);
        
        // Configure the Flash LED to 280 mA.
        err |= pm_flash_led_set_current(280);        
        
        // At this point, the Flash LED will turn on (280 mA) if MPP3 is
        // low and turn off if MPP3 is high.
            
INPUT PARAMETERS  
  1) Parameter name: pol
     -  DBUS polarity setting.

     Parameter type: pm_flash_led_pol_type.
     -  Valid inputs:
            PM_FLASH_LED_POL__ACTIVE_HIGH
            PM_FLASH_LED_POL__ACTIVE_LOW

RETURN VALUE
  return value type: pm_err_flag_type.
  
  return value type: pm_err_flag_type.  
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC. 
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.                                                                            
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init() 
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while writing and reading to/from PMIC.

===========================================================================*/
pm_err_flag_type pm_flash_led_set_polarity (pm_flash_led_pol_type pol);
/*~ FUNCTION pm_flash_led_set_polarity */

/*===========================================================================

                        SPEAKER DRIVER

===========================================================================*/
/*===========================================================================

FUNCTION pm_speaker_cmd                           EXTERNAL FUNCTION

DESCRIPTION
  This function turns the speaker ON or OFF and enables or disables mute.

INPUT PARAMETERS
  Parameter name: cmd

  Parameter type:pm_spkr_cmd_type (enum).
    - Valid Inputs:
            PM_SPKR_DISABLE  = Enable Speaker
            PM_SPKR_ENABLE   = Disable Speaker
            PM_SPKR_MUTE_OFF = turn speaker mute off
            PM_SPKR_MUTE_ON  = turn speaker mute on
            PM_SPKR_OFF      = turn speaker OFF (speaker disable and mute on)
            PM_SPKR_ON       = turn speaker ON (speaker enable and mute off)

RETURN VALUE
  return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

  Note: You may not enable/use the ringer and speaker driver at
        the same time.

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type 
pm_speaker_cmd ( pm_spkr_cmd_type cmd ) ;
/*~ FUNCTION pm_speaker_cmd */

/*===========================================================================

FUNCTION pm_set_speaker_gain                           EXTERNAL FUNCTION

DESCRIPTION
  This function configures the PMIC speaker gain.

INPUT PARAMETERS
  Parameter name: speaker_gain
    - Speaker gain

  Parameter type: pm_spkr_gain_type (enum).
    - Valid Inputs:
            PM_SPKR_GAIN_MINUS16DB  = -16 db
            PM_SPKR_GAIN_MINUS12DB  = -12 db
            PM_SPKR_GAIN_MINUS08DB  = -08 db
            PM_SPKR_GAIN_MINUS04DB  = -04 db
            PM_SPKR_GAIN_00DB       =  00 db
            PM_SPKR_GAIN_PLUS04DB   = +04 db
            PM_SPKR_GAIN_PLUS08DB   = +08 db
            PM_SPKR_GAIN_PLUS12DB   = +12 db
            PM_SPKR_GAIN_OUT_OF_RANGE

RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type 
pm_set_speaker_gain ( pm_spkr_gain_type speaker_gain ) ;
/*~ FUNCTION pm_set_speaker_gain */

/*===========================================================================

FUNCTION pm_set_speaker_delay                           EXTERNAL FUNCTION

DESCRIPTION
  This function configures the PMIC speaker ENABLE/ON delay.

INPUT PARAMETERS
  Parameter name: speaker_delay.
    - Speaker delay.

  Parameter type: pm_spkr_dly_type.
    - Valid Inputs:
                PM_SPKR_DLY_10MS  = ~10  ms delay
                PM_SPKR_DLY_100MS = ~100 ms delay

RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type 
pm_set_speaker_delay ( pm_spkr_dly_type speaker_delay ) ;
/*~ FUNCTION pm_set_speaker_delay */

/*===========================================================================

FUNCTION pm_speaker_1k6_zin_enable                         EXTERNAL FUNCTION

DESCRIPTION
  This function enables/disables an extra 1.6k Ohm resistor in parallel to
the input.

**Note: This function only applies to PM6640 and Panoramix.

INPUT PARAMETERS
  Parameter name: enable

  Parameter type: boolean
    - Valid Inputs:
                TRUE = Enable 1.6k Ohm resistor in parallel to the input 
                FALSE = Disable 1.6k Ohm resistor in parallel to the input 

RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type 
pm_speaker_1k6_zin_enable ( boolean enable ) ;
/*~ FUNCTION pm_speaker_1k6_zin_enable */

/*===========================================================================

                 VIBRATOR DRIVER FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================

FUNCTION pm_vib_mot_set_volt                            EXTERNAL FUNCTION

DESCRIPTION
  This function configures the voltage setting for the Vibrator Motor.
  
  EXAMPLE:
     // Configure the vibrator motor to 3 Volts.
     err = pm_vib_mot_set_volt(3000);     
  
INPUT PARAMETERS  
  1) Parameter name: vlt
     - Voltage setting in mV.

     Parameter type: uint16.
     -  Valid inputs:
        1) 0 mV  (off, default).
        2) 1200 mV to 3100mV, in 100 mV steps.                     

RETURN VALUE
  return value type: pm_err_flag_type.
  
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC. 
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.                                                                            
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init() 
  2) pm_init()

SIDE EFFECTS
  Interrupts are disabled while writing and reading to/from PMIC.

===========================================================================*/
pm_err_flag_type pm_vib_mot_set_volt (uint16 vlt);
/*~ FUNCTION pm_vib_mot_set_volt */

/*===========================================================================

FUNCTION pm_vib_mot_set_mode                          EXTERNAL FUNCTION

DESCRIPTION
    This function allows the user to configure the vibrator motor to manual 
  mode or automatic mode, where:
  
    1) Manual Mode: 
           1.1 Disable by setting the Vibrator Motor Voltage to 0 mV.
           1.2 Enable by setting the Vibrator Motor Voltage to a setting
               within the valid range.
                                      
    2) Automatic Mode:
           The Vibrator Motor gets enable/disable depending on the logic
           state of the corresponding DBUS line. 
                           
    NOTES: 
          Vibrator Motor Voltage: 
               Please refer to the function description of 
               "pm_vib_mot_set_volt()".
                         
          DBUS polarity:
               Please refer to the function description of 
               "pm_vib_mot_set_polarity()".
               
          DBUS: 
               The user can configure a MPP to drive a DBUS line. 
               Please refer to the function description of
               "pm_mpp_config_digital_input()".
               
    
    EXAMPLE:

        // Disable the vibrator motor.
        err  = pm_vib_mot_set_volt(0);
                
        // Configure MPP5 to be a digital input, set the voltage reference
        // to MSMP VREG and drive data bus 2 (DBUS2).
        err |= pm_mpp_config_digital_input(PM_MPP_5, 
                                              PM_MPP__DLOGIC__LVL_MSMP,
                                                  PM_MPP__DLOGIC_IN__DBUS2);
        
        // Configure vibrator motor to automatic and use DBUS2 as the 
        // control line.
        err |= pm_vib_mot_set_mode(PM_VIB_MOT_MODE__DBUS2);
        
        // Configure DBUS polarity to active high. 
        err |= pm_vib_mot_set_polarity(PM_VIB_MOT_POL__ACTIVE_HIGH);
        
        // Configure the vibrator motor to 3 Volts.
        err |= pm_vib_mot_set_volt(3000);        
        
        // At this point, the Vibrator will turn on (3V) if MPP5 is
        // high and turn off if MPP5 is low.
                
        
INPUT PARAMETERS  
  1) Parameter name: mode 
     - Configure the vibrator motor mode

     Parameter type: pm_vib_mot_mode_type.
     - Valid inputs:
     
        PM_VIB_MOT_MODE__MANUAL = Manual Mode.
        PM_VIB_MOT_MODE__DBUS1  = Automatic Mode, Controlled by DBUS1 line.
        PM_VIB_MOT_MODE__DBUS2  = Automatic Mode, Controlled by DBUS2 line.
        PM_VIB_MOT_MODE__DBUS3  = Automatic Mode, Controlled by DBUS3 line.     

RETURN VALUE
  return value type: pm_err_flag_type.  
  
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC. 
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.                                                                            
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.
    
DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init() 
  2) pm_init()
  3) pm_vib_mot_set_volt()
  4) pm_vib_mot_set_polarity()
  5) pm_mpp_config_digital_input()
                                       
SIDE EFFECTS
  Interrupts are disabled while writing and reading to/from PMIC.

===========================================================================*/
pm_err_flag_type pm_vib_mot_set_mode (pm_vib_mot_mode_type mode);
/*~ FUNCTION pm_vib_mot_set_mode */

/*===========================================================================

FUNCTION pm_vib_mot_set_polarity                       EXTERNAL FUNCTION

DESCRIPTION
    If the vibrator motor is in automatic mode (refer to 
  pm_vib_mot_set_mode()) then this function configures the polarity of 
  the dbus line.
    
INPUT PARAMETERS  
  1) Parameter name: pol
     -  DBUS polarity setting.

     Parameter type: pm_vib_mot_pol_type.
     -  Valid inputs:
            PM_VIB_MOT_POL__ACTIVE_HIGH
            PM_VIB_MOT_POL__ACTIVE_LOW

RETURN VALUE
  return value type: pm_err_flag_type.
  
  return value type: pm_err_flag_type.  
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC. 
     PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.                                                                            
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init() 
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while writing and reading to/from PMIC.

===========================================================================*/
pm_err_flag_type pm_vib_mot_set_polarity (pm_vib_mot_pol_type pol);
/*~ FUNCTION pm_vib_mot_set_polarity */

/*===========================================================================

                 TEMPERATURE PROTECTION FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================

FUNCTION pm_itemp_get_stage                           EXTERNAL FUNCTION

DESCRIPTION
    This function returns the current over temperature protection stage
  of the PMIC.

    Note that a change of stage can be detected by enabling or pulling
   the Temperature Status Changed IRQ (refer to PM_T_STAT_CHANGED_IRQ_HDL
   and PM_T_STAT_CHANGED_RT_ST).

    Temperature Protection Stages:

        - STAGE0:
            * Temperature Range = T < 110C.
            * Normal Condition.

        - STAGE1:
            * Temperature Range = 110C > T < 130C.
            * Over Temperature Warning. No action.

        - STAGE2:
            * Temperature Range = 130C > T < 150C.
            * Shuts down high current drivers such as speaker and LED
              drivers automatically. Can be override through
              "pm_itemp_stage_override()".

        - STAGE3:
            * Temperature Range = T > 150C.
            * Powers down the PMIC.


INPUT PARAMETERS
  Parameter name: *itemp_stage.
    - Use this pointer to collect the current over temperature stage
      of the PMIC.

  Parameter type: pm_item_stage_type.
    - Valid Outputs:
        PM_ITEMP_STAGE0
        PM_ITEMP_STAGE1
        PM_ITEMP_STAGE2
        PM_ITEMP_STAGE3

RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC. 
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.
    
DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type pm_itemp_get_stage( pm_item_stage_type *itemp_stage );

/*===========================================================================

FUNCTION pm_itemp_stage_override                       EXTERNAL FUNCTION

DESCRIPTION
    This function overrides the automatic shut-down of the temperature
  protection stage.

INPUT PARAMETERS
  1) Parameter name: oride_cmd.
     - Enable/Disable automatic shut-down.

     Parameter type: pm_switch_cmd_type.
     - Valid Inputs:
        PM_OFF_CMD (default) = Do not override automatic shut-down.
        PM_ON_CMD            = Override automatic shut-down.

  2) Parameter name: oride_stage.
     - Which stage we want to configure.

     Parameter type: pm_switch_cmd_type.
     - Valid Inputs:
          PM_ITEMP_ORIDE_STAGE2: Only stage 2 can be override.

RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR2_OUT_OF_RANGE     = Input Parameter two is out of range.
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC.     
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type pm_itemp_stage_override(
                                            pm_switch_cmd_type oride_cmd,
                                            pm_item_oride_type oride_stage);


/*===========================================================================
                       32 kHz Sleep Clock
===========================================================================*/
/*===========================================================================
FUNCTION    pm_xtal_sleep_osc_cmd                          EXTERNAL FUNCTION

DESCRIPTION
   This function enables/disables the external sleep crystal oscillator. If
   the xtal oscillator is turned ON, a certain amount of warm-up time needs
   to be waited for before the oscillator output can be deemed to be stable.
   It is advisable to verify that the xtal osc has indeed become stable by
   reading the real-time status of the "xtal osc halted" interrupt at the end
   of the warm-up period.

PARAMETERS
   1) Name: cmd
      - Command to turn the sleep crystal oscillator ON/OFF
      Type: pm_switch_cmd_type
      - Valid inputs:
           PM_ON_CMD

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     -> Invalid command 'cmd'
     - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED -> Feature not available on this 
                                             version of the PMIC.      
     - PM_ERR_FLAG__SBI_OPT_ERR           -> Communication with PMIC failed
     - PM_ERR_FLAG__SUCCESS               -> SUCCESS

DEPENDENCIES
   The following function(s) must have been called:
   1) rflib_init()
   2) pm_init()

SIDE EFFECTS
   Interrupts are locked while communicating with the PMIC.
===========================================================================*/
pm_err_flag_type pm_xtal_sleep_osc_cmd(pm_switch_cmd_type cmd);

/*===========================================================================

                 SMPL DRIVER FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================

FUNCTION pm_smpld_switch                           EXTERNAL FUNCTION

DESCRIPTION
  This function turns the Sudden Momentary Power Loss Detection on or off.

  Note: The SMPL must be set to OFF during normal power down.
  
INPUT PARAMETERS
  Parameter name: OnOff.

  Parameter type: pm_switch_cmd_type.
  - Valid inputs:
        PM_ON_CMD  = Turns Sudden Momentary Power Loss Detection ON.
        PM_OFF_CMD = Turns Sudden Momentary Power Loss Detection OFF.

RETURN VALUE
  Description:
  - Error Flag.
  
  Return value type: pm_err_flag_type.        
  - Valid output:
  
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC. 
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.  
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = The input parameter is invalid.
    PM_ERR_FLAG__SUCCESS               = Operation was successful.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init() 
  2) pm_init()

    Configure the SMPL timeout value (see pm_set_smpld_timer()) before
  calling this function.
     
SIDE EFFECTS
  Interrupts are disabled while writing and reading to/from PMIC.

===========================================================================*/
pm_err_flag_type pm_smpld_switch (pm_switch_cmd_type OnOff);

/*===========================================================================

FUNCTION pm_set_smpld_timer                           EXTERNAL FUNCTION

DESCRIPTION
    This function selects the Sudden Momentary Power Loss feature timeout
  value. If the battery or external power is reapplied prior to the 
  expiration of the timer then the PMIC will initiate a power on sequence.

INPUT PARAMETERS
  Parameter name: time.
  - smpl time out value.
  
  Parameter type: pm_smpl_timer_type.
  - Valid inputs:
        PM_SMPL_TIMER__0500msec
        PM_SMPL_TIMER__1000msec
        PM_SMPL_TIMER__1500msec
        PM_SMPL_TIMER__2000msec
        
RETURN VALUE
  Description:
  - Error Flag.
  
  Return value type: pm_err_flag_type.        
  - Valid output:
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC. 
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.  
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = The input parameter is invalid.
    PM_ERR_FLAG__SUCCESS               = Operation was successful.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init() 
  2) pm_init()

SIDE EFFECTS
  Interrupts are disabled while writing and reading to/from PMIC.

===========================================================================*/
pm_err_flag_type pm_set_smpld_timer (pm_smpl_timer_type time);

/*===========================================================================

                 CABLE POWER ON DRIVER FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION pm_cbl_pwr_on_switch                       EXTERNAL FUNCTION

DESCRIPTION
    This function allows the user to enable, disable and permanently disable
  cable power ON.
  
  EXAMPLE:
     
    // Blow the fuse so that cable power ON is always disable. Even after
    // power is removed. 
    err = pm_cbl_pwr_on_switch(PM_CBL_PWR_ON__DIS_PERMANENTLY);
    
INPUT PARAMETERS  
  1) Parameter name: cmd
     - Enable, Disable or Blow fuse (to disable for ever) cable power on.  

     Parameter type: pm_cbl_pwr_on_switch_type
     - Valid inputs:
            PM_CBL_PWR_ON__ENABLE (default)= Enable Cable Power ON.
            PM_CBL_PWR_ON__DISABLE         = Disable Cable Power ON.
            PM_CBL_PWR_ON__DIS_PERMANENTLY = Blow fuse. Calling this function 
                                             once will disable cable power
                                             on for ever. This option is
                                             only available on PM6650
                                             Rev G and higher.
            

RETURN VALUE
  return value type: pm_err_flag_type.
  
  return value type: pm_err_flag_type.  
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC. 
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.                                                                            
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init() 
  2) pm_init()

SIDE EFFECTS
  1) Interrupts are disabled while communicating with PMIC.
  2) The microprocessor will be halted for 1 msec while blowing the fuse.
     Option: PM_CBL_PWR_ON__DIS_PERMANENTLY.

===========================================================================*/
pm_err_flag_type pm_cbl_pwr_on_switch(pm_cbl_pwr_on_switch_type cmd);

/*===========================================================================

FUNCTION pm_cbl_pwr_on_pull_up_switch                     EXTERNAL FUNCTION

DESCRIPTION
    This function allows the user to enable and disable CBL0PWR_N and 
    CBL1PWR_N pin pull up.  The pull up will is only effective when
    cable power on is enabled.  
  
  EXAMPLE:
     
    // Enable cable power on and disable CBL1PWR_N pin pull up. 
    err = pm_cbl_pwr_on_pull_up_switch(PM_ON_CMD, PM_CBL_PWR_ON__1);
    
    if (err != PM_ERR_FLAG__SUCCESS)
    {
        MSG_ERROR("PMIC API ERROR %d",err,0,0);
    }

INPUT PARAMETERS  
  1) Parameter name: OnOff.
     - Enable/Disable Cable Power On Pull Up.

     Parameter type: pm_switch_cmd_type (enum).
     - Valid Inputs:
            PM_OFF_CMD
            PM_ON_CMD

  2) Parameter name: cable.
     - For which Cable Power On pin should we enable/disable the pull up. 

     Parameter type: pm_cbl_pwr_on_type (enum).
     - Valid Inputs:
        For PM7500 and PM6650:
            PM_CBL_PWR_ON__0 = Cable Power On pin 0.
            PM_CBL_PWR_ON__1 = Cable Power On pin 1.
        For Panoramix:
            PM_CBL_PWR_ON_PIN__0 = Cable Power On pin 0.
        For all others:
            Cable power on feature is not supported.
            
RETURN VALUE
  return value type: pm_err_flag_type.
  
  return value type: pm_err_flag_type.  
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC. 
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.                              
                                              
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__PAR2_OUT_OF_RANGE     = Input Parameter two is out of range.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init() 
  2) pm_init()

SIDE EFFECTS
  Interrupts are disabled while communicating with PMIC.

===========================================================================*/
pm_err_flag_type pm_cbl_pwr_on_pull_up_switch(
                                  pm_switch_cmd_type      OnOff,
                                  pm_cbl_pwr_on_pin_type  cable);

/*===========================================================================

FUNCTION pm_model_specific_init                           EXTERNAL FUNCTION

DESCRIPTION
This function initializes model specific parametes such as no. of MPP's,
chip rev id and no. of defined IRQ's 
  
INPUT PARAMETERS: pm_model

    Parameter type: pm_model_type
    Valid inputs: PMIC_IS_PM7500, PMIC_IS_PM6650, PMIC_IS_PM6610
    
RETURN VALUE
    NONE

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init() 
  2) pm_init()

SIDE EFFECTS
    NONE

===========================================================================*/
void pm_model_specific_init(void);

/*===========================================================================

FUNCTION pm_get_pmic_model                         EXTERNAL FUNCTION

DESCRIPTION
    This function returns the PMIC's chip revision, e.g.: PM6610, PM6650, etc.

INPUT PARAMETER
  None.

RETURN VALUE
  Description:
  - PMIC model/tier.

  Return value type: pm_model_type.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
pm_model_type pm_get_pmic_model(void);

/*===========================================================================

FUNCTION pm_get_pmic_revision                         EXTERNAL FUNCTION

DESCRIPTION
    This function returns the PMIC's chip revision, e.g.: A0, B1, etc.

INPUT PARAMETER
  None.

RETURN VALUE
  Description:
  - A number representing the PMIC revision.
  See the Objective Spec to find the revision number for this PMIC model.

  Return value type: uint8

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 pm_get_pmic_revision(void);

/*===========================================================================

FUNCTION pm_get_temperature_adj                         EXTERNAL FUNCTION

DESCRIPTION
    This function returns the die temperature compensation in degC that
should be made to the PMIC with integrated charger transistor and sense
resistor.

- Applicable to PM6658 

INPUT PARAMETER
  None.

RETURN VALUE
  Description:
  - temperature in degree Celsius that should be corrected from the PMIC
    die temperature 

  Return value type: uint8.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 pm_get_temperature_adj(void);

/*===========================================================================
FUNCTION pm_set_vbatdet_trim                              EXTERNAL FUNCTION

DESCRIPTION
    This function sets the vbatdet trim value

PARAMETERS
  Parameter name: vbatdet_trim_value
     - indicates the vbatdet trim value
  Parameter type: uint8.
     - Valid Inputs:
       0x00 to 0x0F
	 
RETURN VALUE
  return value type: pm_err_flag_type.  
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC. 
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.                              
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  The functions bellow most be called before calling this function:
  1) rflib_init()
  2) pm_init()
  3) pm_vbatdet_trim_test_mode_switch()

SIDE EFFECTS
  none

===========================================================================*/
pm_err_flag_type pm_set_vbatdet_trim ( uint8 vbatdet_trim_value );

/*===========================================================================

FUNCTION pm_get_vbatdet_trim                              EXTERNAL FUNCTION

DESCRIPTION
    This function gets the current vbatdet trim value

PARAMETERS
  Parameter name: vbatdet_trim_value
     - returns the vbatdet trim value
	 
  Parameter type: uint8*.
     - Valid values:
       0x00 to 0x0F
  
RETURN VALUE
  return value type: pm_err_flag_type.  
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC. 
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.                              
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = NULL pointer passed by param one.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  The functions bellow most be called before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type pm_get_vbatdet_trim ( uint8 *vbatdet_trim_value );

/*===========================================================================
FUNCTION pm_get_vbatdet_trim_limits                         EXTERNAL FUNCTION

DESCRIPTION
  This function gets the vbatdet trim value boundaries

PARAMETERS
  Parameter name: min_val
     - returns the lower boundary of vbatdet trim value
  Parameter name: max_val
     - returns the upper boundary of vbatdet trim value
  Parameter type: uint8*
  
RETURN VALUE
  return value type: pm_err_flag_type.  
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC. 
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = NULL pointer passed through Input Parameter one.
    PM_ERR_FLAG__PAR2_OUT_OF_RANGE     = NULL pointer passed through Input Parameter two.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.
  
DEPENDENCIES
  The functions bellow most be called before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  none
===========================================================================*/
pm_err_flag_type pm_get_vbatdet_trim_limits ( uint8 *min_val, uint8 *max_val );

/*===========================================================================

FUNCTION pm_vbatdet_trim_test_mode_switch                   EXTERNAL FUNCTION

DESCRIPTION
  This function enables/disables vbatdet trim test mode

PARAMETERS
  Parameter name: OnOff.
  - Enable/disable vbatdet trim test mode

  Parameter type: pm_switch_cmd_type.
  - Valid inputs:
        PM_ON_CMD  = Enable.
        PM_OFF_CMD = Disable.
		
RETURN VALUE
  return value type: pm_err_flag_type.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC. 
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.                              
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  The functions bellow most be called before calling this function:
  1) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type pm_vbatdet_trim_test_mode_switch ( pm_switch_cmd_type OnOff);

/*===========================================================================
===========================================================================*/
void pm_app_diag_lib_diag_i(PACKED void* req_pkt,PACKED void* data);


/*===========================================================================

FUNCTION pm_spkr_set_mux_hpf_corner_freq                   EXTERNAL FUNCTION

DESCRIPTION
  This function configures the PMIC speaker frequency.

INPUT PARAMETERS
  Parameter name: freq
    - Speaker frequency

  Parameter type: pm_spkr_hpf_corner_freq_type (enum).
    - Valid Inputs:
            PM_SPKR_FREQ_1_39KHZ = 1.39 kHz 
            PM_SPKR_FREQ_0_64KHZ = 0.64 kHz 
            PM_SPKR_FREQ_0_86KHZ = 0.86 kHz
            PM_SPKR_FREQ_0_51KHZ = 0.51 kHz
            PM_SPKR_FREQ_1_06KHZ = 1.06 kHz
            PM_SPKR_FREQ_0_57KHZ = 0.57 kHz
            PM_SPKR_FREQ_0_73KHZ = 0.73 kHz
            PM_SPKR_FREQ_0_47KHZ = 0.47 kHz
            PM_SPKR_FREQ_1_20KHZ = 1.20 kHz
            PM_SPKR_FREQ_0_60KHZ = 0.60 kHz
            PM_SPKR_FREQ_0_76KHZ = 0.76 kHz
            PM_SPKR_FREQ_0_49KHZ = 0.49 kHz
            PM_SPKR_FREQ_0_95KHZ = 0.95 kHz
            PM_SPKR_FREQ_0_54KHZ = 0.54 kHz
            PM_SPKR_FREQ_0_68KHZ = 0.68 kHz
            PM_SPKR_FREQ_0_45KHZ = 0.45 kHz
            PM_SPKR_FREQ_OUT_OF_RANGE 

RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type 
pm_spkr_set_mux_hpf_corner_freq ( pm_spkr_hpf_corner_freq_type freq ) ;
/*~ FUNCTION pm_spkr_set_mux_hpf_corner_freq */

/*===========================================================================

FUNCTION pm_spkr_get_mux_hpf_corner_freq                   EXTERNAL FUNCTION

DESCRIPTION
  This function gets the PMIC speaker frequency.

INPUT PARAMETERS
  Parameter name: *freq
    - Use this pointer to get Speaker frequency

  Parameter type: pm_spkr_hpf_corner_freq_type (enum).
    - Valid Inputs:
            PM_SPKR_FREQ_1_39KHZ = 1.39 kHz 
            PM_SPKR_FREQ_0_64KHZ = 0.64 kHz 
            PM_SPKR_FREQ_0_86KHZ = 0.86 kHz
            PM_SPKR_FREQ_0_51KHZ = 0.51 kHz
            PM_SPKR_FREQ_1_06KHZ = 1.06 kHz
            PM_SPKR_FREQ_0_57KHZ = 0.57 kHz
            PM_SPKR_FREQ_0_73KHZ = 0.73 kHz
            PM_SPKR_FREQ_0_47KHZ = 0.47 kHz
            PM_SPKR_FREQ_1_20KHZ = 1.20 kHz
            PM_SPKR_FREQ_0_60KHZ = 0.60 kHz
            PM_SPKR_FREQ_0_76KHZ = 0.76 kHz
            PM_SPKR_FREQ_0_49KHZ = 0.49 kHz
            PM_SPKR_FREQ_0_95KHZ = 0.95 kHz
            PM_SPKR_FREQ_0_54KHZ = 0.54 kHz
            PM_SPKR_FREQ_0_68KHZ = 0.68 kHz
            PM_SPKR_FREQ_0_45KHZ = 0.45 kHz
            PM_SPKR_FREQ_OUT_OF_RANGE 

RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type 
pm_spkr_get_mux_hpf_corner_freq ( pm_spkr_hpf_corner_freq_type* freq) ; 
/*~ FUNCTION pm_spkr_get_mux_hpf_corner_freq */
/*~ PARAM OUT freq POINTER */

/*===========================================================================

FUNCTION pm_vid_en                                         EXTERNAL FUNCTION

DESCRIPTION
  This function allows the user to enable/disable PMIC video amplifier

INPUT PARAMETERS
  parameter name: enable_disable.
     - Enable/Disable PMIC video amplifier

     Parameter type: pm_switch_cmd_type (enum).
     - Valid Inputs:
            PM_OFF_CMD = 0
            PM_ON_CMD = 1
            

RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type pm_vid_en( boolean enable_disable);
/*~ FUNCTION pm_vid_en */

/*===========================================================================

FUNCTION pm_vid_is_en                                      EXTERNAL FUNCTION

DESCRIPTION
  This function checks if PMIC video amplifier is enabled/disabled

INPUT PARAMETERS
  parameter name: *enable_disable.
     - Use this pointer to get Enable/Disable status of PMIC video amplifier

     Parameter type: boolean
     
RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  None

===========================================================================*/
pm_err_flag_type pm_vid_is_en(boolean *enable_disable);
/*~ FUNCTION pm_vid_is_en */
/*~ PARAM OUT enable_disable POINTER */

/*===========================================================================

FUNCTION pm_vid_load_detect_en                            EXTERNAL FUNCTION

DESCRIPTION
  This function allows the user to enable/disable the PMIC video amplifier to
detect load 

INPUT PARAMETERS
  parameter name: enable_disable.

     Parameter type: pm_switch_cmd_type (enum).
     - Valid Inputs:
            PM_OFF_CMD = disable load detect of video amplifier
            PM_ON_CMD = enable load detectof video amplifier
            

RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type pm_vid_load_detect_en(boolean enable_disable);
/*~ FUNCTION pm_vid_load_detect_en */

/*===========================================================================

FUNCTION pm_mic_en                                         EXTERNAL FUNCTION

DESCRIPTION
  This function allows the user to enable/disable PMIC MIC

INPUT PARAMETERS
  parameter name: enable_disable.
     - Enable/Disable PMIC MIC

     Parameter type: pm_switch_cmd_type (enum).
     - Valid Inputs:
            PM_OFF_CMD = 0
            PM_ON_CMD = 1
            

RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type pm_mic_en( boolean enable_disable ) ;
/*~ FUNCTION pm_mic_en */

/*===========================================================================

FUNCTION pm_mic_is_en                                      EXTERNAL FUNCTION

DESCRIPTION
  This function checks if PMIC MIC is enabled/disabled

INPUT PARAMETERS
  parameter name: *enable_disable.
     - Use this pointer to get Enable/Disable status of PMIC MIC

     Parameter type: boolean
     
RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  None

===========================================================================*/
pm_err_flag_type 
pm_mic_is_en ( boolean *enable_disable ) ;
/*~ FUNCTION pm_mic_is_en */
/*~ PARAM OUT enable_disable POINTER */

/*===========================================================================

FUNCTION pm_mic_set_volt                                    EXTERNAL FUNCTION

DESCRIPTION
  This function configures the PMIC MIC  voltage.

INPUT PARAMETERS
  Parameter name: voltage
    - Speaker voltage

  Parameter type: pm_mic_volt_type (enum).
    - Valid Inputs:
            PM_MIC_VOLT_2_00V = 2.00 V 
            PM_MIC_VOLT_1_93V = 1.93 V
            PM_MIC_VOLT_1_80V = 1.80 V 
            PM_MIC_VOLT_1_73V = 1.73 V 
            PM_MIC_VOLT_OUT_OF_RANGE      

RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type 
pm_mic_set_volt ( pm_mic_volt_type voltage ) ;
/*~ FUNCTION pm_mic_set_volt */

/*===========================================================================

FUNCTION pm_mic_get_volt                                    EXTERNAL FUNCTION

DESCRIPTION
  This function gets the PMIC MIC voltage.

INPUT PARAMETERS
  Parameter name: *voltage
    - Use this pointer to get MIC voltage

  Parameter type: pm_mic_volt_type (enum).
    - Valid Inputs:
            PM_MIC_VOLT_2_00V = 2.00 V 
            PM_MIC_VOLT_1_93V = 1.93 V
            PM_MIC_VOLT_1_80V = 1.80 V 
            PM_MIC_VOLT_1_73V = 1.73 V 
            PM_MIC_VOLT_OUT_OF_RANGE      


RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type 
pm_mic_get_volt ( pm_mic_volt_type *voltage ) ;
/*~ FUNCTION pm_mic_get_volt */
/*~ PARAM OUT voltage POINTER */

/*===========================================================================

FUNCTION pm_spkr_en_right_chan                              EXTERNAL FUNCTION

DESCRIPTION
  This function allows the user to enable/disable PMIC SPEAKER right channel

INPUT PARAMETERS
  parameter name: enable_disable.
     - Enable/Disable PMIC SPEAKER right channel

     Parameter type: pm_switch_cmd_type (enum).
     - Valid Inputs:
            PM_OFF_CMD = 0
            PM_ON_CMD = 1
            

RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type 
pm_spkr_en_right_chan ( boolean enable_disable ) ;
/*~ FUNCTION pm_spkr_en_right_chan */

/*===========================================================================

FUNCTION pm_spkr_is_right_chan_en                           EXTERNAL FUNCTION

DESCRIPTION
  This function checks if PMIC SPEAKER right channel is enabled or disabled 

INPUT PARAMETERS
  parameter name: *enable_disable.
     - Use this pointer to get status of Enable/Disable PMIC SPEAKER right channel

     Parameter type: boolean
     function updates this value with the current status
     
RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  None

===========================================================================*/
pm_err_flag_type 
pm_spkr_is_right_chan_en ( boolean* enable_disable ) ;
/*~ FUNCTION pm_spkr_is_right_chan_en */
/*~ PARAM OUT enable_disable POINTER */

/*===========================================================================

FUNCTION pm_spkr_en_left_chan                              EXTERNAL FUNCTION

DESCRIPTION
  This function allows the user to enable/disable PMIC SPEAKER left channel

INPUT PARAMETERS
  parameter name: enable_disable.
     - Enable/Disable PMIC SPEAKER left channel

     Parameter type: pm_switch_cmd_type (enum).
     - Valid Inputs:
            PM_OFF_CMD = 0
            PM_ON_CMD = 1
            

RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type 
pm_spkr_en_left_chan ( boolean enable_disable ) ;
/*~ FUNCTION pm_spkr_en_left_chan */


/*===========================================================================

FUNCTION pm_spkr_is_left_chan_en                           EXTERNAL FUNCTION

DESCRIPTION
  This function checks if PMIC SPEAKER left channel is enabled or disabled 

INPUT PARAMETERS
  parameter name: *enable_disable.
     - Use this pointer to get status of Enable/Disable PMIC SPEAKER left channel

     Parameter type: boolean
     function updates this value with the current status
     
RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  None

===========================================================================*/
pm_err_flag_type 
pm_spkr_is_left_chan_en ( boolean* enable_disable ) ;
/*~ FUNCTION pm_spkr_is_left_chan_en */
/*~ PARAM OUT enable_disable POINTER */

/*===========================================================================

FUNCTION pm_spkr_add_right_left_chan                        EXTERNAL FUNCTION

DESCRIPTION
  This function allows the user to enable/disable PMIC SPEAKER left & 
  right channel

INPUT PARAMETERS
  parameter name: enable_disable.
     - Enable/Disable PMIC SPEAKER left & right channel

     Parameter type: pm_switch_cmd_type (enum).
     - Valid Inputs:
            PM_OFF_CMD = 0
            PM_ON_CMD = 1
            

RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type 
pm_spkr_add_right_left_chan ( boolean enable_disable ) ;
/*~ FUNCTION pm_spkr_add_right_left_chan */

/*===========================================================================

FUNCTION pm_spkr_is_right_left_chan_added                    EXTERNAL FUNCTION

DESCRIPTION
  This function checks if PMIC SPEAKER left& right channel is added 

INPUT PARAMETERS
  parameter name: *enable_disable.
     - Use this pointer to get status of Enable/Disable PMIC SPEAKER added left 
        & right channel

     Parameter type: boolean
     function updates this value with the current status
     
RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  None

===========================================================================*/
pm_err_flag_type 
pm_spkr_is_right_left_chan_added ( boolean* enable_disable ) ;
/*~ FUNCTION pm_spkr_is_right_left_chan_added */
/*~ PARAM OUT enable_disable POINTER */

/*===========================================================================

FUNCTION pm_spkr_en_stereo                        EXTERNAL FUNCTION

DESCRIPTION
  This function allows the user to enable/disable PMIC SPEAKER stereo

INPUT PARAMETERS
  parameter name: enable_disable.
     - Enable/Disable PMIC SPEAKER stereo

     Parameter type: pm_switch_cmd_type (enum).
     - Valid Inputs:
            PM_OFF_CMD = 0
            PM_ON_CMD = 1
            

RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type 
pm_spkr_en_stereo ( boolean enable_disable ) ;
/*~ FUNCTION pm_spkr_en_stereo */

/*===========================================================================

FUNCTION pm_spkr_is_stereo_en                    EXTERNAL FUNCTION

DESCRIPTION
  This function checks if PMIC SPEAKER stereo is added 

INPUT PARAMETERS
  parameter name: *enable_disable.
     - Use this pointer to get status of Enable/Disable PMIC SPEAKER stereo

     Parameter type: boolean
     function updates this value with the current status
     
RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  None

===========================================================================*/
pm_err_flag_type 
pm_spkr_is_stereo_en ( boolean* enable_disable ) ;
/*~ FUNCTION pm_spkr_is_stereo_en */
/*~ PARAM OUT enable_disable POINTER */

/*===========================================================================

FUNCTION pm_spkr_select_usb_with_hpf_20hz                   EXTERNAL FUNCTION

DESCRIPTION
  This function allows the user to enable/disable PMIC SPEAKER usb

INPUT PARAMETERS
  parameter name: enable_disable.
     - Enable/Disable PMIC SPEAKER usb

     Parameter type: pm_switch_cmd_type (enum).
     - Valid Inputs:
            PM_OFF_CMD = 0
            PM_ON_CMD = 1
            

RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type 
pm_spkr_select_usb_with_hpf_20hz( boolean enable_disable ) ;
/*~ FUNCTION pm_spkr_select_usb_with_hpf_20hz */

/*===========================================================================

FUNCTION pm_spkr_is_usb_with_hpf_20hz                    EXTERNAL FUNCTION

DESCRIPTION
  This function checks if PMIC SPEAKER usb is added 

INPUT PARAMETERS
  parameter name: *enable_disable.
     - Use this pointer to get status of Enable/Disable PMIC SPEAKER usb

     Parameter type: boolean
     function updates this value with the current status
     
RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  None

===========================================================================*/
pm_err_flag_type 
pm_spkr_is_usb_with_hpf_20hz ( boolean* enable_disable ) ;
/*~ FUNCTION pm_spkr_is_usb_with_hpf_20hz */
/*~ PARAM OUT enable_disable POINTER */

/*===========================================================================

FUNCTION pm_spkr_bypass_mux                        EXTERNAL FUNCTION

DESCRIPTION
  This function allows the user to enable/disable PMIC SPEAKER bypass mux

INPUT PARAMETERS
  parameter name: enable_disable.
     - Enable/Disable PMIC SPEAKER bypass mux

     Parameter type: pm_switch_cmd_type (enum).
     - Valid Inputs:
            PM_OFF_CMD = 0
            PM_ON_CMD = 1
            

RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type 
pm_spkr_bypass_mux ( boolean enable_disable ) ;
/*~ FUNCTION pm_spkr_bypass_mux */

/*===========================================================================

FUNCTION pm_spkr_is_mux_bypassed                    EXTERNAL FUNCTION

DESCRIPTION
  This function checks if PMIC SPEAKER bypass mux is added 

INPUT PARAMETERS
  parameter name: *enable_disable.
     - Use this pointer to get status of Enable/Disable PMIC SPEAKER bypass mux

     Parameter type: boolean
     function updates this value with the current status
     
RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  None

===========================================================================*/
pm_err_flag_type 
pm_spkr_is_mux_bypassed ( boolean* enable_disable ) ;
/*~ FUNCTION pm_spkr_is_mux_bypassed */
/*~ PARAM OUT enable_disable POINTER */

/*===========================================================================

FUNCTION pm_spkr_en_hpf                                   EXTERNAL FUNCTION

DESCRIPTION
  This function allows the user to enable/disable PMIC SPEAKER hpf

INPUT PARAMETERS
  parameter name: enable_disable.
     - Enable/Disable PMIC SPEAKER hpf

     Parameter type: pm_switch_cmd_type (enum).
     - Valid Inputs:
            PM_OFF_CMD = 0
            PM_ON_CMD = 1
            

RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type 
pm_spkr_en_hpf ( boolean enable_disable ) ;
/*~ FUNCTION pm_spkr_en_hpf */

/*===========================================================================

FUNCTION pm_spkr_is_hpf_en                    EXTERNAL FUNCTION

DESCRIPTION
  This function checks if PMIC SPEAKER hpf is added 

INPUT PARAMETERS
  parameter name: *enable_disable.
     - Use this pointer to get status of Enable/Disable PMIC SPEAKER hpf

     Parameter type: boolean
     function updates this value with the current status
     
RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  None

===========================================================================*/
pm_err_flag_type 
pm_spkr_is_hpf_en ( boolean* enable_disable ) ;
/*~ FUNCTION pm_spkr_is_hpf_en */
/*~ PARAM OUT enable_disable POINTER */

/*===========================================================================

FUNCTION pm_spkr_en_sink_curr_from_ref_volt_cir             EXTERNAL FUNCTION

DESCRIPTION
  This function allows the user to enable/disable PMIC SPEAKER sink current

INPUT PARAMETERS
  parameter name: enable_disable.
     - Enable/Disable PMIC SPEAKER sink current

     Parameter type: pm_switch_cmd_type (enum).
     - Valid Inputs:
            PM_OFF_CMD = 0
            PM_ON_CMD = 1
            

RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type 
pm_spkr_en_sink_curr_from_ref_volt_cir ( boolean enable_disable ) ;
/*~ FUNCTION pm_spkr_en_sink_curr_from_ref_volt_cir */

/*===========================================================================

FUNCTION pm_spkr_is_sink_curr_from_ref_volt_cir_en          EXTERNAL FUNCTION

DESCRIPTION
  This function checks if PMIC SPEAKER sink current is added 

INPUT PARAMETERS
  parameter name: *enable_disable.
     - Use this pointer to get status of Enable/Disable PMIC SPEAKER sink current

     Parameter type: boolean
     function updates this value with the current status
     
RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  None

===========================================================================*/
pm_err_flag_type 
pm_spkr_is_sink_curr_from_ref_volt_cir_en ( boolean* enable_disable ) ;
/*~ FUNCTION pm_spkr_is_sink_curr_from_ref_volt_cir_en */
/*~ PARAM OUT enable_disable POINTER */

/*===========================================================================

FUNCTION pm_set_spkr_configuration                   EXTERNAL FUNCTION

DESCRIPTION
  This function sets different speaker modes.

INPUT PARAMETERS
  Parameter name: config
    - Speaker configuration

  Parameter type: pm_spkr_config_mode_type (struct).
    - Valid Inputs:
      PMIC_SPEAKER_STEREO_CONFIG               //STEREO
      PMIC_SPEAKER_LEFT_MONO_CONFIG            //LEFT_MONO
      PMIC_SPEAKER_RIGHT_MONO_CONFIG           //RIGHT_MONO
      PMIC_SPEAKER_DUAL_MONO_CONFIG            //DUAL_MONO
      PMIC_SPEAKER_LEFT_8OHM_RECEIVER_CONFIG   //LEFT_8OHM_RECEIVER
      PMIC_SPEAKER_RIGHT_8OHM_RECEIVER_CONFIG  //RIGHT_8OHM_RECEIVER
      PMIC_SPEAKER_SUMMED_MONO_LEFT_CONFIG     //SUMMED_MONO_LEFT
      PMIC_SPEAKER_SUMMED_MONO_RIGHT_CONFIG    //SUMMED_MONO_RIGHT
      PMIC_SPEAKER_USB_CARKIT_CONFIG           //USB_CARKIT

RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type 
pm_set_spkr_configuration ( const pm_spkr_config_mode_type* config ) ;
/*~ FUNCTION pm_set_spkr_configuration */
/*~ PARAM IN config POINTER */

/*===========================================================================

FUNCTION pm_get_spkr_configuration                        EXTERNAL FUNCTION

DESCRIPTION
  This function gets the PMIC mux cntl2 register configuration.

INPUT PARAMETERS
  Parameter name: *config
    - Use this pointer to get speaker mux cntl2 configuration value

  Parameter type: pm_spkr_config_mode_type (struct).
    - Valid Inputs:
      PMIC_SPEAKER_STEREO_CONFIG               //STEREO
      PMIC_SPEAKER_LEFT_MONO_CONFIG            //LEFT_MONO
      PMIC_SPEAKER_RIGHT_MONO_CONFIG           //RIGHT_MONO
      PMIC_SPEAKER_DUAL_MONO_CONFIG            //DUAL_MONO
      PMIC_SPEAKER_LEFT_8OHM_RECEIVER_CONFIG   //LEFT_8OHM_RECEIVER
      PMIC_SPEAKER_RIGHT_8OHM_RECEIVER_CONFIG  //RIGHT_8OHM_RECEIVER
      PMIC_SPEAKER_SUMMED_MONO_LEFT_CONFIG     //SUMMED_MONO_LEFT
      PMIC_SPEAKER_SUMMED_MONO_RIGHT_CONFIG    //SUMMED_MONO_RIGHT
      PMIC_SPEAKER_USB_CARKIT_CONFIG           //USB_CARKIT
            
RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  

===========================================================================*/

pm_err_flag_type 
pm_get_spkr_configuration ( pm_spkr_config_mode_type* config ) ;
/*~ FUNCTION pm_get_spkr_configuration */
/*~ PARAM OUT config POINTER */

/*===========================================================================

FUNCTION pm_spkr_set_gain                                  EXTERNAL FUNCTION

DESCRIPTION
  This function configures the PMIC speaker gain.

INPUT PARAMETERS
  1. Parameter name: left_right
    - Left / Right Speaker 

  Parameter type: pm_spkr_left_right_type (enum).
    - Valid Inputs:
            PM_LEFT_SPKR,
            PM_RIGHT_SPKR,
            PM_SPKR_OUT_OF_RANGE   
       
  2. Parameter name: gain
    - Left Right Speaker gain

  Parameter type: pm_spkr_gain_type (enum).
    - Valid Inputs:
            PM_SPKR_GAIN_MINUS16DB,     
            PM_SPKR_GAIN_MINUS12DB,      
            PM_SPKR_GAIN_MINUS08DB,      
            PM_SPKR_GAIN_MINUS04DB,     
            PM_SPKR_GAIN_00DB,          
            PM_SPKR_GAIN_PLUS04DB,       
            PM_SPKR_GAIN_PLUS08DB,       
            PM_SPKR_GAIN_PLUS12DB,       
            PM_SPKR_GAIN_OUT_OF_RANGE  
     

RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.

===========================================================================*/

pm_err_flag_type 
pm_spkr_set_gain ( pm_spkr_left_right_type left_right , 
				   pm_spkr_gain_type gain ) ;
/*~ FUNCTION pm_spkr_set_gain */

/*===========================================================================

FUNCTION pm_spkr_get_gain                                  EXTERNAL FUNCTION

DESCRIPTION
  This function configures the PMIC speaker gain.

INPUT PARAMETERS
  1. Parameter name: left_right
    - Left / Right Speaker 

  Parameter type: pm_spkr_left_right_type (enum).
    - Valid Inputs:
            PM_LEFT_SPKR,
            PM_RIGHT_SPKR,
            PM_SPKR_OUT_OF_RANGE   
       
  2. Parameter name: *gain
    - Use this pointer to get Left/ Right Speaker gain

  Parameter type: pm_spkr_gain_type (enum).
    - Valid Inputs:
            PM_SPKR_GAIN_MINUS16DB,     
            PM_SPKR_GAIN_MINUS12DB,      
            PM_SPKR_GAIN_MINUS08DB,      
            PM_SPKR_GAIN_MINUS04DB,     
            PM_SPKR_GAIN_00DB,          
            PM_SPKR_GAIN_PLUS04DB,       
            PM_SPKR_GAIN_PLUS08DB,       
            PM_SPKR_GAIN_PLUS12DB,       
            PM_SPKR_GAIN_OUT_OF_RANGE  
     

RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.

===========================================================================*/

pm_err_flag_type 
pm_spkr_get_gain ( pm_spkr_left_right_type left_right ,
                   pm_spkr_gain_type *gain ) ;
/*~ FUNCTION pm_spkr_get_gain */
/*~ PARAM OUT gain POINTER */

/*===========================================================================

FUNCTION pm_spkr_en                                  EXTERNAL FUNCTION

DESCRIPTION
  This function enable/disable the PMIC left/ right speaker.

INPUT PARAMETERS
  1. Parameter name: left_right
    - Left / Right Speaker 

  Parameter type: pm_spkr_left_right_type (enum).
    - Valid Inputs:
            PM_LEFT_SPKR,
            PM_RIGHT_SPKR,
            PM_SPKR_OUT_OF_RANGE   
       
  2. Parameter name: enable_disable
    - Left Right Speaker enable/disable flag

  Parameter type: boolean (enum).
    

RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type 
pm_spkr_en ( pm_spkr_left_right_type left_right , boolean enable_disable ) ;
/*~ FUNCTION pm_spkr_en */

/*===========================================================================

FUNCTION pm_spkr_is_en                                      EXTERNAL FUNCTION

DESCRIPTION
  This function checks if PMIC LEFT/RIGHT SPEAKER is enabled 

INPUT PARAMETERS
  1. Parameter name: left_right
    - Left / Right Speaker 

  Parameter type: pm_spkr_left_right_type (enum).
    - Valid Inputs:
            PM_LEFT_SPKR,
            PM_RIGHT_SPKR,
            PM_SPKR_OUT_OF_RANGE  
			
  2. parameter name: *enable_disable.
     - Use this pointer to get status of Enable/Disable PMIC LEFT/RIGHT SPEAKER 

     Parameter type: boolean
     function updates this value with the current status
     
RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  None

===========================================================================*/
pm_err_flag_type 
pm_spkr_is_en ( pm_spkr_left_right_type left_right ,
				boolean *enable_disable ) ;
/*~ FUNCTION pm_spkr_is_en */
/*~ PARAM OUT enable_disable POINTER */

/*===========================================================================

FUNCTION pm_spkr_en_mute                                  EXTERNAL FUNCTION

DESCRIPTION
  This function enable/disable the PMIC left/ right speaker mute function.

INPUT PARAMETERS
  1. Parameter name: left_right
    - Left / Right Speaker 

  Parameter type: pm_spkr_left_right_type (enum).
    - Valid Inputs:
            PM_LEFT_SPKR,
            PM_RIGHT_SPKR,
            PM_SPKR_OUT_OF_RANGE   
       
  2. Parameter name: enable_disable
    - Left Right Speaker enable/disable flag

  Parameter type: boolean (enum).
    

RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type 
pm_spkr_en_mute ( pm_spkr_left_right_type left_right , 
				  boolean enable_disable) ;
/*~ FUNCTION pm_spkr_en_mute */

/*===========================================================================

FUNCTION pm_spkr_is_mute_en                                      EXTERNAL FUNCTION

DESCRIPTION
  This function checks if PMIC LEFT/RIGHT SPEAKER mute is enabled 

INPUT PARAMETERS
  1. Parameter name: left_right
    - Left / Right Speaker 

  Parameter type: pm_spkr_left_right_type (enum).
    - Valid Inputs:
            PM_LEFT_SPKR,
            PM_RIGHT_SPKR,
            PM_SPKR_OUT_OF_RANGE  
			
  2. parameter name: *enable_disable.
     - Use this pointer to get status of mute Enable/Disable PMIC LEFT/RIGHT SPEAKER 

     Parameter type: boolean
     function updates this value with the current status
     
RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  None

===========================================================================*/
pm_err_flag_type 
pm_spkr_is_mute_en ( pm_spkr_left_right_type left_right , 
                     boolean *enable_disable ) ;
/*~ FUNCTION pm_spkr_is_mute_en */
/*~ PARAM OUT enable_disable POINTER */

/*===========================================================================

FUNCTION pm_spkr_set_delay                                  EXTERNAL FUNCTION

DESCRIPTION
  This function configures the PMIC speaker delay.

INPUT PARAMETERS
  1. Parameter name: left_right
    - Left / Right Speaker 

  Parameter type: pm_spkr_left_right_type (enum).
    - Valid Inputs:
            PM_LEFT_SPKR,
            PM_RIGHT_SPKR,
            PM_SPKR_OUT_OF_RANGE   
       
  2. Parameter name: delay
    - Left Right Speaker delay

  Parameter type: pm_spkr_dly_type (enum).
    - Valid Inputs:
            PM_SPKR_BYP_01uF_10ms_DELAY, // 10ms turn on delay use with 0.1uF 
            PM_SPKR_BYP_1uF_100ms_DELAY, // 100ms turn on delay use with 1uF  
            PM_SPKR_DELAY_OUT_OF_RANGE   // Not valid 
     

RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type 
pm_spkr_set_delay ( pm_spkr_left_right_type left_right , 
					pm_spkr_dly_type delay ) ;
/*~ FUNCTION pm_spkr_set_delay */ 

/*===========================================================================

FUNCTION pm_spkr_get_delay                                  EXTERNAL FUNCTION

DESCRIPTION
  This function configures the PMIC speaker delay.

INPUT PARAMETERS
  1. Parameter name: left_right
    - Left / Right Speaker 

  Parameter type: pm_spkr_left_right_type (enum).
    - Valid Inputs:
            PM_LEFT_SPKR,
            PM_RIGHT_SPKR,
            PM_SPKR_OUT_OF_RANGE   
       
  2. Parameter name: *delay
    - Use this pointer to get Left/ Right Speaker delay

  Parameter type: pm_spkr_gain_type (enum).
    - Valid Inputs:
            PM_SPKR_BYP_01uF_10ms_DELAY, // 10ms turn on delay use with 0.1uF 
            PM_SPKR_BYP_1uF_100ms_DELAY, // 100ms turn on delay use with 1uF  
            PM_SPKR_DELAY_OUT_OF_RANGE   // Not valid      

RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.

===========================================================================*/
pm_err_flag_type 
pm_spkr_get_delay ( pm_spkr_left_right_type left_right,
                    pm_spkr_dly_type *delay ) ;
/*~ FUNCTION pm_spkr_get_delay */
/*~ PARAM OUT delay POINTER */

/*===========================================================================

                 HEADSET SEND/END DETECT FUNCTION DEFINITIONS

===========================================================================*/

pm_err_flag_type pm_hsed_status_type_1_get(pm_hsed_state_type_1 *hsed_status);
/**
 * Function pm_hsed_get_module_type
 *   Call this function to determine which type of HSED module is supported
 *   by this PMIC
 * 
 * @param module_type PM_HSED_MODULE_TYPE__0,
 *                    PM_HSED_MODULE_TYPE__1,
 *                    PM_HSED_MODULE_TYPE__2,
 *                    PM_HSED_MODULE_TYPE__INVALID
 * 
 * @return pm_err_flag
 */
pm_err_flag_type pm_hsed_get_module_type(pm_hsed_module_type *module_type);
/*===========================================================================

FUNCTION pm_hsed_set_switch_type                     EXTERNAL FUNCTION

DESCRIPTION
    This function configures the the headset switch type and the corresponding
current threshold for button press detection
  
INPUT PARAMETERS  
  1. Parameter name: switch_type
    - Headset switch type: open circuit or short circuit 

  Parameter type: pm_hsed_switch_type (enum).
    - Valid Inputs:
        PM_HSED_SC_SWITCH_TYPE = short circuit type
        PM_HSED_OC_SWITCH_TYPE = open circuit type
       
  2. Parameter name: current_threshold
    - current threshold to detect a button press 

  Parameter type: pm_hsed_idac_type (enum).
    - Valid Inputs:
        - Short Circuit type:
            PM_HSED_SC_IDAC_300uA,
            PM_HSED_SC_IDAC_400uA,
            PM_HSED_SC_IDAC_500uA,
            PM_HSED_SC_IDAC_600uA,
            PM_HSED_SC_IDAC_700uA,
            PM_HSED_SC_IDAC_800uA,
            PM_HSED_SC_IDAC_900uA,
            PM_HSED_SC_IDAC_1000uA,
        - Open Circuit type:
            PM_HSED_OC_IDAC_10uA,
            PM_HSED_OC_IDAC_20uA,
            PM_HSED_OC_IDAC_30uA,
            PM_HSED_OC_IDAC_40uA,
            PM_HSED_OC_IDAC_50uA,
            PM_HSED_OC_IDAC_60uA,
            PM_HSED_OC_IDAC_70uA,
            PM_HSED_OC_IDAC_80uA,
            
RETURN VALUE
  return value type: pm_err_flag_type.
  
  return value type: pm_err_flag_type.  
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC. 
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.                              
                                              
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__PAR2_OUT_OF_RANGE     = Input Parameter two is out of range.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed below before calling this function:
  1) rflib_init() 
  2) pm_init()

SIDE EFFECTS
  Interrupts are disabled while communicating with PMIC.

===========================================================================*/
pm_err_flag_type pm_hsed_set_switch_type
(
    pm_hsed_switch_type switch_type,
    pm_hsed_idac_type current_threshold
);
/*===========================================================================

FUNCTION pm_hsed_set_current_threshold                     EXTERNAL FUNCTION

DESCRIPTION
    This function configures the the headset switch type and the corresponding
current threshold for button press detection
    Used for PMIC4 HSED only. 
  
INPUT PARAMETERS  
  1. Parameter name: switch_type
    - Headset switch type: open circuit or short circuit 

  Parameter type: pm_hsed_switch_type (enum).
    - Valid Inputs:
        PM_HSED_SC_SWITCH_TYPE = short circuit type
        PM_HSED_OC_SWITCH_TYPE = open circuit type
       
  2. Parameter name: current_threshold
    - current threshold to detect a button press 

  Parameter type: pm_hsed_idac_type (enum).
    - Valid Inputs:
        - Short Circuit type:
            200 uA to 1700 uA
         - Open Circuit type:
            10 uA to 160 uA            
RETURN VALUE
  return value type: pm_err_flag_type.
  
  return value type: pm_err_flag_type.  
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC. 
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.                              
                                              
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__PAR2_OUT_OF_RANGE     = Input Parameter two is out of range.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init() 
  2) pm_init()

SIDE EFFECTS
  Interrupts are disabled while communicating with PMIC.

===========================================================================*/
pm_err_flag_type pm_hsed_set_current_threshold
(
    pm_hsed_switch_type switch_type,
    uint32              current_threshold
);
/*===========================================================================

FUNCTION pm_hsed_enable                     EXTERNAL FUNCTION

DESCRIPTION
    This function enables/disables signal control for the HSED module
  
INPUT PARAMETERS  
  1. Parameter name: enable_type

  Parameter type: pm_hsed_enable_type (enum).
    - Valid Inputs:
        PM_HSED_ENABLE_OFF = off
        PM_HSED_ENABLE_TCXO = enabled if TCXO_EN is high
        PM_HSED_ENABLE_PWM_TCXO = enabled if PWM signal is high or TCXO_EN is high
        PM_HSED_ENABLE_ALWAYS = enabled always
            
RETURN VALUE
  return value type: pm_err_flag_type.
  
  return value type: pm_err_flag_type.  
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC. 
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.                              
                                              
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init() 
  2) pm_init()

SIDE EFFECTS
  Interrupts are disabled while communicating with PMIC.

===========================================================================*/
pm_err_flag_type pm_hsed_enable ( pm_hsed_enable_type enable_type );

/*===========================================================================

FUNCTION pm_hsed_set_hysteresis                         EXTERNAL FUNCTION

DESCRIPTION
    This function configures the hystersis clock pre-divider and the hystersis
clock.
  
INPUT PARAMETERS  
  1. Parameter name: hyst_pre_div
    - Clock input for the hystersis timer

  Parameter type: pm_hsed_hyst_pre_div_type (enum).
    - Valid Inputs:
        PM_HSED_HYST_PRE_DIV_1 = divide the 1.024kHz clock by 1
        PM_HSED_HYST_PRE_DIV_2 = divide the 1.024kHz clock by 2
        PM_HSED_HYST_PRE_DIV_4 = divide the 1.024kHz clock by 4
        PM_HSED_HYST_PRE_DIV_8 = divide the 1.024kHz clock by 8
        PM_HSED_HYST_PRE_DIV_16 = divide the 1.024kHz clock by 16
        PM_HSED_HYST_PRE_DIV_32 = divide the 1.024kHz clock by 32
        PM_HSED_HYST_PRE_DIV_64 = divide the 1.024kHz clock by 64
        PM_HSED_HYST_PRE_DIV_128 = divide the 1.024kHz clock by 128
       
  2. Parameter name: hyst_time
    - Hystersis time 

  Parameter type: pm_hsed_hyst_time_type (enum).
    - Valid Inputs:
        PM_HSED_HYST_TIME_1_CLK_CYCLES = 1 pre-divided clock cycles
        PM_HSED_HYST_TIME_2_CLK_CYCLES = 2 pre-divided clock cycles
        PM_HSED_HYST_TIME_3_CLK_CYCLES = 3 pre-divided clock cycles
        PM_HSED_HYST_TIME_4_CLK_CYCLES = 4 pre-divided clock cycles
        PM_HSED_HYST_TIME_5_CLK_CYCLES = 5 pre-divided clock cycles
        PM_HSED_HYST_TIME_6_CLK_CYCLES = 6 pre-divided clock cycles
        PM_HSED_HYST_TIME_7_CLK_CYCLES = 7 pre-divided clock cycles
        PM_HSED_HYST_TIME_8_CLK_CYCLES = 8 pre-divided clock cycles

            
RETURN VALUE
  return value type: pm_err_flag_type.
  
  return value type: pm_err_flag_type.  
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC. 
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.                              
                                              
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__PAR2_OUT_OF_RANGE     = Input Parameter two is out of range.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init() 
  2) pm_init()

SIDE EFFECTS
  Interrupts are disabled while communicating with PMIC.

===========================================================================*/
pm_err_flag_type pm_hsed_set_hysteresis
( 
    pm_hsed_hyst_pre_div_type hyst_pre_div,
    pm_hsed_hyst_time_type hyst_time 
);

/*===========================================================================

FUNCTION pm_hsed_set_period                         EXTERNAL FUNCTION

DESCRIPTION
    This function configures the period clock pre-divider and period clock.
  
INPUT PARAMETERS  
  1. Parameter name: period_pre_div
    - Clock input for the period timer

  Parameter type: pm_hsed_period_pre_div_type (enum).
    - Valid Inputs:
        PM_HSED_PERIOD_PRE_DIV_2 = divide the 1.024kHz clock by 2
        PM_HSED_PERIOD_PRE_DIV_4 = divide the 1.024kHz clock by 4
        PM_HSED_PERIOD_PRE_DIV_8 = divide the 1.024kHz clock by 8
        PM_HSED_PERIOD_PRE_DIV_16 = divide the 1.024kHz clock by 16
        PM_HSED_PERIOD_PRE_DIV_32 = divide the 1.024kHz clock by 32
        PM_HSED_PERIOD_PRE_DIV_64 = divide the 1.024kHz clock by 64
        PM_HSED_PERIOD_PRE_DIV_128 = divide the 1.024kHz clock by 128
        PM_HSED_PERIOD_PRE_DIV_256 = divide the 1.024kHz clock by 256
       
  2. Parameter name: period_time
    - Period clock period 

  Parameter type: pm_hsed_period_time_type (enum).
    - Valid Inputs:
        PM_HSED_PERIOD_TIME_1_CLK_CYCLES = 1 pre-divided clock cycles
        PM_HSED_PERIOD_TIME_2_CLK_CYCLES = 2 pre-divided clock cycles
        PM_HSED_PERIOD_TIME_3_CLK_CYCLES = 3 pre-divided clock cycles
        PM_HSED_PERIOD_TIME_4_CLK_CYCLES = 4 pre-divided clock cycles
        PM_HSED_PERIOD_TIME_5_CLK_CYCLES = 5 pre-divided clock cycles
        PM_HSED_PERIOD_TIME_6_CLK_CYCLES = 6 pre-divided clock cycles
        PM_HSED_PERIOD_TIME_7_CLK_CYCLES = 7 pre-divided clock cycles
        PM_HSED_PERIOD_TIME_8_CLK_CYCLES = 8 pre-divided clock cycles

            
RETURN VALUE
  return value type: pm_err_flag_type.
  
  return value type: pm_err_flag_type.  
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC. 
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.                              
                                              
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__PAR2_OUT_OF_RANGE     = Input Parameter two is out of range.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init() 
  2) pm_init()

SIDE EFFECTS
  Interrupts are disabled while communicating with PMIC.

===========================================================================*/
pm_err_flag_type pm_hsed_set_period
( 
    pm_hsed_period_pre_div_type period_pre_div,
    pm_hsed_period_time_type period_time 
);

/*===========================================================================

FUNCTION pm_usb_ovp_enable                         EXTERNAL FUNCTION

DESCRIPTION
    This function enables/disabled the USB overvoltage protection (OVP)
functionality 
  
INPUT PARAMETERS  

            
RETURN VALUE
  return value type: pm_err_flag_type.
  
  return value type: pm_err_flag_type.  
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC. 
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.                              
                                              
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__PAR2_OUT_OF_RANGE     = Input Parameter two is out of range.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init() 
  2) pm_init()

SIDE EFFECTS
  Interrupts are disabled while communicating with PMIC.

===========================================================================*/
pm_err_flag_type pm_usb_ovp_enable( boolean enable );

/*===========================================================================

FUNCTION pm_usb_ovp_set_threshold                         EXTERNAL FUNCTION

DESCRIPTION
    This function configures the voltage threshold that triggers the USB OVP
functionality
  
INPUT PARAMETERS  

            
RETURN VALUE
  return value type: pm_err_flag_type.
  
  return value type: pm_err_flag_type.  
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC. 
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.                              
                                              
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__PAR2_OUT_OF_RANGE     = Input Parameter two is out of range.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init() 
  2) pm_init()

SIDE EFFECTS
  Interrupts are disabled while communicating with PMIC.

===========================================================================*/
pm_err_flag_type pm_usb_ovp_set_threshold( pm_usb_ovp_threshold_type threshold );

/*===========================================================================

FUNCTION pm_usb_ovp_set_hystersis                         EXTERNAL FUNCTION

DESCRIPTION
    This function configures the hystersis for the voltage to exceed the
threshold before it triggers the USB OVP functionality
  
INPUT PARAMETERS  

            
RETURN VALUE
  return value type: pm_err_flag_type.
  
  return value type: pm_err_flag_type.  
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC. 
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.                              
                                              
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__PAR2_OUT_OF_RANGE     = Input Parameter two is out of range.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init() 
  2) pm_init()

SIDE EFFECTS
  Interrupts are disabled while communicating with PMIC.

===========================================================================*/
pm_err_flag_type pm_usb_ovp_set_hystersis( pm_usb_ovp_hystersis_type hystersis );

/*===========================================================================

FUNCTION pm_pwm_generator_select_clock                         EXTERNAL FUNCTION

DESCRIPTION
    This function selects the input clock source for the PWM generator 
  
INPUT PARAMETERS  

            
RETURN VALUE
  return value type: pm_err_flag_type.
  
  return value type: pm_err_flag_type.  
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC. 
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.                              
                                              
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__PAR2_OUT_OF_RANGE     = Input Parameter two is out of range.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init() 
  2) pm_init()

SIDE EFFECTS
  Interrupts are disabled while communicating with PMIC.

===========================================================================*/
pm_err_flag_type pm_pwm_generator_select_clock
( 
    pm_pwm_generator_type generator,
    pm_pwm_generator_clock_type clock 
);

/*===========================================================================

FUNCTION pm_pwm_generator_select_pre_divider                   EXTERNAL FUNCTION

DESCRIPTION
    This function selects the clock pre-divider frequency for the PWM generator 
  
INPUT PARAMETERS  

            
RETURN VALUE
  return value type: pm_err_flag_type.
  
  return value type: pm_err_flag_type.  
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC. 
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.                              
                                              
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__PAR2_OUT_OF_RANGE     = Input Parameter two is out of range.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init() 
  2) pm_init()

SIDE EFFECTS
  Interrupts are disabled while communicating with PMIC.

===========================================================================*/
pm_err_flag_type pm_pwm_generator_select_pre_divider
( 
    pm_pwm_generator_type generator,
    pm_pwm_generator_pre_div_type pre_div,
    pm_pwm_generator_pre_div_exp_type exp 
);

/*===========================================================================

FUNCTION pm_pwm_generator_set                   EXTERNAL FUNCTION

DESCRIPTION
    This function sets the PWM size (6-bit or 9-bit) and its associated 6-bit
or 9-bit value 
  
INPUT PARAMETERS  

            
RETURN VALUE
  return value type: pm_err_flag_type.
  
  return value type: pm_err_flag_type.  
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC. 
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.                              
                                              
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__PAR2_OUT_OF_RANGE     = Input Parameter two is out of range.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init() 
  2) pm_init()

SIDE EFFECTS
  Interrupts are disabled while communicating with PMIC.

===========================================================================*/
pm_err_flag_type pm_pwm_generator_set
( 
    pm_pwm_generator_type generator,
    pm_pwm_generator_size_type size,
    uint16 pwm_value 
);

/*===========================================================================

FUNCTION pm_pa_on_cntrl_enable               EXTERNAL FUNCTION

DESCRIPTION
    Enable or disable the PA_ON controller register on HAN PMIC.
      
INPUT PARAMETERS  
    pm_pa_on_cntrl_enable_type enable  
            
RETURN VALUE
  return value type: pm_err_flag_type.
  
  return value type: pm_err_flag_type.  
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC. 
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.                              
                                              
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__PAR2_OUT_OF_RANGE     = Input Parameter two is out of range.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init() 
  2) pm_init()

SIDE EFFECTS
  Interrupts are disabled while communicating with PMIC.
===========================================================================*/
pm_err_flag_type pm_pa_on_cntrl_enable
( 
    pm_pa_on_cntrl_enable_type enable
);
/*===========================================================================

FUNCTION pm_pa_on_cntrl_channel_set               EXTERNAL FUNCTION

DESCRIPTION
    Set the channel on the PA_ON controller register on HAN PMIC.
      
INPUT PARAMETERS  
    pm_pa_on_cntrl_channel_set_type channel;
            
RETURN VALUE
  return value type: pm_err_flag_type.
  
  return value type: pm_err_flag_type.  
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC. 
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.                              
                                              
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__PAR2_OUT_OF_RANGE     = Input Parameter two is out of range.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init() 
  2) pm_init()

SIDE EFFECTS
  Interrupts are disabled while communicating with PMIC.
===========================================================================*/
pm_err_flag_type pm_pa_on_cntrl_channel_set
(
    pm_pa_on_cntrl_channel_set_type channel
);

/*===========================================================================

FUNCTION pm_pa_on_cntrl_voltage_set               EXTERNAL FUNCTION

DESCRIPTION
    Set the voltage level on the PA_ON controller register on HAN PMIC.
      
INPUT PARAMETERS  
pm_err_flag_type pm_pa_on_cntrl_voltage_set
            
RETURN VALUE
  return value type: pm_err_flag_type.
  
  return value type: pm_err_flag_type.  
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                         version of the PMIC. 
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.                              
                                              
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__PAR2_OUT_OF_RANGE     = Input Parameter two is out of range.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init() 
  2) pm_init()

SIDE EFFECTS
  Interrupts are disabled while communicating with PMIC.
===========================================================================*/
pm_err_flag_type pm_pa_on_cntrl_voltage_set
(
    pm_pa_on_cntrl_vlevel_set_type vlevel
);
/*===========================================================================

FUNCTION pm_usb_ovp_is_supported                         EXTERNAL FUNCTION

DESCRIPTION
    This function checks if usb ovp feature is supported 
  
INPUT PARAMETERS  
NONE
            
RETURN VALUE
  return value type: boolean.
  
    TRUE  = PMIC supports USB OVP feature
    FALSE = PMIC does NOT supports USB OVP feature

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init() 
  2) pm_init()

SIDE EFFECTS
  Interrupts are disabled while communicating with PMIC.

===========================================================================*/
boolean pm_usb_ovp_is_supported( void );

/**
 * Function: API to enable/disable USIM AUX module
 *
 * @param on_off PM_ON_CMD  = Enable
 *               PM_OFF_CMD = Disable (default)
 * 
 * @return pm_err_flag_type
 */
pm_err_flag_type pm_usim_aux_enable(pm_switch_cmd_type on_off);
/**
 * Function:  API to set power supply for USIM
 * 0 = MSM_VDD_PAD is connected to VREG_MSME
 * 1 = MSM_VDD_PAD is connected to VREG_RUIM
 * 
 * @param vreg   PM_USIM_AUX_CTRL_VREG_MSME,
 *               PM_USIM_AUX_CTRL_VREG_RUIM,
 *               PM_USIM_AUX_CTRL_VREG__INVALID
 * 
 * @return pm_err_flag_type
 */
pm_err_flag_type pm_usim_aux_vreg_set(pm_usim_aux_cntrl_vreg_type vreg);

/**
 * pm_err_flag_type pm_mbg_iref_enable(boolean enable)
 * 
 * Enables or disables MBG iRef 
 * 
 * @param enable TRUE:  enable MBG IRef
 *               FALSE:  disable MBG Iref
 * 
 * @return pm_err_flag_type
 */
pm_err_flag_type pm_mbg_iref_enable(boolean enable);

/**
 * pm_vote_clk_32k(pm_switch_cmd_type cmd,
 *                 pm_32k_clk_app_vote_type vote)
 * 
 * Cast a vote to turn the 32 KHz FM radio clock on or off.
 * If any application has voted to turn on, the clock will 
 * be on.  Only when all applications that have voted on
 * subsequently vote to turn off will the clock be off.
 * 
 * @param cmd    Command to vote the 32 kHz FM radio clock on or off
 * @param vote   Which application is casting the vote to turn the clock
 *               on or off.
 * 
 * @return pm_err_flag_type
 * @example FM radio application votes to turn the clock on:
 * pm_vote_clk_32k(ON, PM_32K_CLK_FM_APP);
 * 
 * Bluetooth application votes to turn the clock off:
 * pm_vote_clk_32k(OFF, PM_32K_CLK_BT_APP);
 */
pm_err_flag_type pm_vote_clk_32k(pm_switch_cmd_type cmd, 
                                        pm_32k_clk_app_vote_type vote);

/*===========================================================================

FUNCTION pm_config_buck_min_p_on_time            EXTERNAL FUNCTION

DESCRIPTION
    Configures the minimum P_ON Time for requested Buck SMPS
    Example: For configuring minimum P_ON Time of 35 ns to BUSK SMPS S2, we should 
             call this function as
       pm_config_buck_min_p_on_time(BUCK_SMPS_S2, PM_MIN_P_ON_TIME_35);
                  
INPUT PARAMETERS  
    Buck_smps_type (enum)
     Valid parameters: 
      BUCK_SMPS_S1,
      BUCK_SMPS_S2
    
    pm_min_on_time_type (enum)
      Valid parameters:
      PM_MIN_P_ON_TIME_35 
      PM_MIN_P_ON_TIME_46 
      PM_MIN_P_ON_TIME_68 
      PM_MIN_P_ON_TIME_119 
      PM_MIN_P_ON_TIME_155 
      PM_MIN_P_ON_TIME_188 
      PM_MIN_P_ON_TIME_251 
      PM_MIN_P_ON_TIME_307            
	     
RETURN VALUE
  return value type: pm_err_flag_type.
  
  return value type: pm_err_flag_type.  
    PM_ERR_FLAG__SBI_OPT_ERR           =   Failed to communicate with the PMIC through SBI
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     =   Parameter 1 out of range
    PM_ERR_FLAG__PAR2_OUT_OF_RANGE     =   Parameter 2 out of range
    PM_ERR_FLAG__SUCCESS               =   SUCCESS 
      

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) rflib_init() 
  2) pm_init()

SIDE EFFECTS
  Interrupts are disabled while communicating with PMIC.
===========================================================================*/
pm_err_flag_type pm_config_buck_min_p_on_time(buck_smps_type smps, pm_min_on_time_type min_time);

#endif /* PMGEN_H */
