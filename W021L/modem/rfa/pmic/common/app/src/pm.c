/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             P M 6 6 X X   S T A R T   U P   S E R V I C E S

GENERAL DESCRIPTION
  This file contains initialization functions and corresponding variable
  declarations to support interaction with the Qualcomm PM66XX chips.

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2002,2003,2004 by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2005,2006,2007 by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2008           by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/common/main/latest/app/src/pm.c#15 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/21/08   aab     Added FEATURE_PMIC_SELECT_USB_LDO_INPUT_BASED_ON_USB_DEVICE_CONNECTION
                   this feature is used to select USB_LDO power input(VREG_5V or USB VBus) based 
                   on USB Vbus session valid interrupt.
09/20/08   jtn     Re-added MSM6290/6246 power optimization settings, removed 
                   VREG_GP1 LPM setting
08/21/08   jtn     Removed pm_pcil.h, it is included from pm.h
08/01/08   jtn     Move all proc comm functions to pm_pcil.c
07/29/08   jtn     Merge changes from Gobi2000/UNDP
07/11/08   jtn     Add BUILD_BOOT_CHAIN around MPP8 configuration in pm_init()
07/11/08   jtn     Undid removal of pm_vreg_pa_dvs_work_around()--required for 
                   DZRF6500RF card
07/01/08   jtn     Added proc comm registration for SMEM_PROC_COMM_VREG_PULLDOWN
06/24/08   vk      Added MPP to the list of shunt capacitor to ground before
                   configuring as analog output.
06/26/08   jtn     Remove all pm_vreg_set_level() and pm_lp_mode_control()
                   for 6246/6290
(begin QSC1100 changes)
06/03/08   jtn     Added #if defined (T_QSC1100) around #include "tramp.h"
05/15/08   jtn     Move shadow register initialization to library
05/08/08   jtn     Initialize shadow for VREG control registers
05/07/08   jtn     Added unit test code for Kip.  Removed un-needed VREG 
                   initialization
(end QSC1100 changes)
06/23/08   jtn     Removed VREG_WLAN from DEFAULT_VREGS_ENA
06/16/08   jtn     Remove call to pm_vreg_set_level( PM_VREG_RFTX_ID, 2100 )
                   for 6246/6290 targets
06/08/08   jtn     Remove VREG_WLAN LP mode enable for MSM6246 and MSM6290
05/14/08   jtn     Add target-specific initialization for MSM6246 and MSM6290
05/13/08   jtn     Added feature FEATURE_PMIC_PROCCOMM_SUPPORT.  This feature
                   should be defined in custpmic3.h for builds that require
                   proc_comm access to PMIC VREG and MPP APIs.
05/07/08    vk     Added video amplifier initialization in pm_init()
04/25/08    jtn    Remove MSME and MSMC initialization when T_PMIC_PM6640 is 
                   defined on QSC6270/WSC6240 build
04/22/08   jtn     Removed pm_clockless_pfm_enable() settings from pm_init(),
                   moved to pm_init_comm()
04/16/08   jtn     Removed VREG MSMC and MSME initialization for QSC6270/QSC6240
03/21/08   jtn     Removed FEATURE_PMIC_LOW_POWER_MODE for DEFAULT_VREG_RUIM_LPM
                   and DEFAULT_VREG_WLAN_LPM
03/18/08   jtn     Removed unused #define PMIC_TYPE
                   Register proc_comm function in interrupt context
                   Removed reference to MSMC_VOLTAGE_HIGH_V 
                   Add #ifdef (BUILD_BOOT_CHAIN) around pm_hsed_init()
03/14/08   jtn     Add #if defined (FEATURE_MPROC_PKG2) to include proc comm
                   related features only on builds that support MPROC Package 2
02/06/08   jtn     Added proc comm function initialization
02/27/08   jtn     Added support for Kip PMIC/QSC1100
01/16/08   jtn     Remove call to pm_cbl_pwr_on_pull_up_switch() in pm_init()
01/08/08   jtn     Merged QSC6270 branch
(begin QSC6270 changes)
10/26/07   jnoblet Added FEATURE_PMIC_TEMP_HAN_TRIM_FIX
                   Initial QSC6270 voltage settings for MSMC and MSME
10/18/07   jnoblet Added temporary fix for untrimmed HAN PMICs
10/09/07   jnoblet Added PMIC interrupt initialization for QSC6270
(end QSC6270 changes)
09/20/07   cng     Removed all MPP configuration code for MSM7500
09/14/07   cng     Do not define DEFAULT_VREGS_ENA for MSM7500
07/27/07   cng     Do not turn on VREG_BOOST for LCD apps
                   Do not disable cable power on during init
06/18/07   cng     Changed API interface to pm_mpp_config_analog_output
                   Removed HW specific function pm_sbi_gpio_config 
03/07/07   aab     Moved pm_rtc_pwr_rst_isr(void) to pmapp_rtc.c
02/27/07   cng     Registered PMIC ISR handler with tramp for QSC6055
02/27/07   cng     Fixed lint warnings
10/02/06   cng     Enabled clockless PFM mode for PM7500 buck VREGs
09/08/06   cng     LINT warning fixes
09/05/06   cng     Removed active pull-down for VREGs
06/15/06   cng     Added init code for headset send/end detect feature
06/14/06   cng     Added featurization for VREG management
06/14/06   cng     Enabled active pull-down for several voltage regulators
06/14/06   cng     Changed list of VREGs enabled during software init for MSM7500
06/12/06   cng     Changed MSMC volatage for QSC and MSME for MSM6100
04/20/06   cng     Changed MSMA voltage level for QSC
03/14/06   cng     Corrected API name to pm_mbg_lpm_force_nbias_sleep
02/14/06   cng     Implemented proper sequence of enabling MBG low power mode
01/24/06   vk      Removed FEATURE_PMIC_DIAG_SERVICES as it should be enabled by 
                   default for all the builds
01/24/06   vk      Removed argument PMIC_TYPE from call to pm_init_comm()				   
11/09/05   cng     Added PM6620 support
11/08/05   cng     More support for Panoramix
10/24/05   cng     Added Panoramix and PM6640 LPM support. Corrected some VREG
                   settings for PM6640
10/10/05   cng     Added calls in pm_init to set up VREG_MDDI and VREG_AUX2 for PM7500
10/05/05   fpe     Enabled the pull down resistor for MSMA and MSMP LDOs in pm_vreg_init
09/20/05   fpe     Added MSM6050 support
09/09/05   fpe     Added PM6650 and PM6640 support for MSM6025 and MSM6000
08/29/05   fpe     PM6640 support
07/25/05   cng     PM7500 support 
07/01/05   cng     Pulled the cable power on pins (CBLxPWR_N) low during init
06/20/05   jac     Added featurization for MSM6550 so that the PMIC AUX2
                   SBI assignments are made through the SBI API function, 
                   similar to what is done for Saber.
04/13/05   cng     Added the feature pm_watchdog_reset_detect_switch
02/15/05   rmd     Made sure that pm_init() and pm_init_delayed() get
                   called once.
02/04/05   rmd     Added support to FEATURE_CUSTDRIVERS_HAS_MSMC_V
10/19/04   jac     removed FEATURE_PMIC_DISABLE_CABLE_POWER_ON
                   and RUIM LPM
10/13/04   rmd     Featurized the cable power on function call in pm_init()
                   under FEATURE_PMIC_DISABLE_CABLE_POWER_ON.
09/02/04   rmd     For the MSM6250, We do not longer enable LPM mode for the 
                   WLAN VREG during boot up.
08/30/04   rmd     In pm_init(), Disabled Cable Power On.
08/17/04   rmd     For the MSM6250, enable LPM mode for RUIM and WLAN VREGs.
08/09/04   rmd     Updated RFRX1 and RFTX to 2.65 Volts for PM6610 + RFCMOS 
                   targets.
06/23/04   rmd     For the VREF_OUT, replaced T_PMIC_MSM6250_CONFIG with 
                   FEATURE_GSM. 
06/07/04   rmd     Added support for the MSM6100 + PM6610.
05/17/04   rmd     Removed "Disable Cable Power On during boot up". This 
                   change was not necessary. 
05/16/04   rmd     Disable Cable Power On during boot up.
05/04/04   rmd     * Added configuration settings for the MSM6000.
                   * Added support for the MSMC0 VREG.
04/20/04   rmd     * Added parameter in pm_variable_pa_vdd_init() to allow 
                     the caller to enable/disable PMIC support to PA DVS.
                   * Changed the code so that we enable the output for
                     the bandgap voltage (VREF_OUT) in GSM targets only.
04/05/04   rmd     Featurized PMIC DIAG services under 
                   "FEATURE_PMIC_DIAG_SERVICES".
03/24/04   rmd     In pm_init(), Added init code for PMIC PMDIAG services.
03/08/04   rmd     * Added support for SMPL.
                   * Provided the option to program MSMC to 1.375 volts if
                     feature FEATURE_PMIC_MSMC_1p375V is defined.
                   * Moved VREG LPM mode init code to pm_vreg_init().
                   * Enabled the Boost VREG through pm_vote_vreg_switch() 
                     instead of pm_vreg_control() to allow LCD and USB
                     to vote it ON/OFF.
02/27/04   rmd     Left SMPS registers in their default state (auto).
02/17/04   rmd     Added support for low power mode.
02/13/04   rmd     Add error detection code in pm_init() to detect if
                   we were successful initializing communication with
                   the PMIC.  
01/19/04   rmd     Added support for the PM6610.
11/21/03   rmd     Added SBI GPIO configuration.
11/14/03   rmd     Moved temp charger functions into the charger driver.
11/12/03   Vish    Added PM_OSC_HALT_IRQ_HDL ISR initialization in
                   pm_init_delayed().
11/11/03   Vish    Added a minor comment in pm_rtc_pwr_rst_isr().
11/07/03   rmd     Enabled MMC for MSM6100.
11/03/03   rmd     Added support for PMIC's internal over temperature
                   protection.
10/31/03   Vish    Added support for RTC initialization after a power reset.
10/16/03   rmd     Added support for the pm_soft_reset().
10/13/03   rmd     Added support for the MSM6100.
10/09/03   rmd     - Changed the Settings for the Voltage regulators
                     from a value calculated from an equation to mV according
                     to the changes done to the Voltage Regulators API.
                   - I do not longer enable the RUIM VREG by default. This
                     now done by the RUIM application.
09/25/03   Vish    Added RTC initialization (pm_rtc_init()) to PMIC's
                   delayed init routine.
09/09/03   rmd     Enabled the output for the bandgap voltage (VREF_OUT).
09/04/03   rmd     Updated the value for MSME to 1.85 volts.
                   Added support for variable PA voltage.
09/02/03   rmd     Added initialization for PM66XX AMUX.
08/28/03   rmd     Renamed pm_ruim_init() -> pm_uim_init().
08/25/03   rmd     Initialized the BUCK Voltage regulators to PWM mode
                   (pulse-width modulation).
08/14/03   rmd     Enhanced the Charger Initialization
08/13/03   rmd     To allow Automatic TCXO control to work, Removed the code
                   that enables the TCXO VREG during boot up.
                   VREG_TCXO is enabled by default.
07/29/03   rmd     Initialized the TCXO Controller to automatic mode.
07/24/03   rmd     Added Charger Initialization.
07/21/03   rmd     Added support for PMIC IRQs.
07/15/03   rmd     Updated the voltage setting for MSMA and MSMP
07/15/03   rmd     Updated the voltage setting for MSMC1 and MSMA
05/29/03   rmd     Created.
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"

#if defined (FEATURE_PMIC)

#include "comdef.h"
#include "pm.h"
#include "err.h"
#include "gpio_int.h"


#if defined(T_PMIC_MSM6250_CONFIG) || (defined(T_MSM6550) && defined(FEATURE_GSM))
#error code not present
#endif

#if defined(T_QSC60X0)|| defined(T_QSC6055) || defined (T_QSC1100) || defined(T_QSC6270)
#include "tramp.h"
#endif

#include "pmapp_diag.h"

/*===========================================================================

                 LOCAL CONSTANT AND MACRO DEFINITIONS

===========================================================================*/
/******************************************************************************
- Valid Voltage settings for the different VREGs:

            VREG: PA, MSME, MSMC1 and MSMC2
              750  mV to 1525 mV, in 25 mV steps.
              1550 mV to 3050 mV, in 50 mV steps.

            VREG: BOOST, USB
              3000 mV to 6100 mV in 100 mV steps.

            VREG: MSMA, MSMP, MSMS, MEM, GP, CAM, TCXO, PA, RFTX, RFRX1
                  RFRX2, SYNTH, WLAN, MMC, RUIM.
              1500 mV to 3050 mV in 50 mV steps.

     NOTE: Voltage values between voltage steps will be rounded up to
           the next voltage setting. ex:

            // Because 1760 mV is not a multiple of 25 mV, the
            // PA VREG voltage setting will be rounded up to
            // the next setting (1775 mV).
            pm_vreg_set_level(PM_VREG_PA_ID, 1760);

******************************************************************************/

/*****************************************************************************
                        MSM SPECIFIC VREG SETTINGS
******************************************************************************/

/******************   VREG settings for the SC1X ***************************/
/*lint -esym(750, MSMP_LEVEL, MSME_LEVEL, MSME1_LEVEL, MSMC1_LEVEL, MSMA_LEVEL, TCXO_LEVEL) */
/*lint -esym(750, RFTX_LEVEL, RFRX1_LEVEL, RFRX2_LEVEL, SYNTH_LEVEL) */
/*lint -esym(750, MDDI_LEVEL, AUX2_LEVEL) */
#if defined(T_QSC60X0)

  #if defined(T_PMIC_PANORAMIX)
#error code not present
  #endif /* T_PMIC_PANORAMIX */

/******************   VREG settings for the MSM7500 ***************************/
#elif defined(T_MSM7500)
#error code not present
#elif defined(T_MSM6500)

  /* MSMP Voltage setting  = 2.6 Volts */
  #define MSMP_LEVEL        2600

  /* MSME Voltage setting  = 1.85 Volts*/
  #define MSME_LEVEL        1850

  #if defined(FEATURE_PMIC_MSMC_1p375V)
  /* MSMC1 Voltage setting = 1.375 Volts */
  #define MSMC1_LEVEL       1375
  #else
  /* MSMC1 Voltage setting = 1.400 Volts */
  #define MSMC1_LEVEL       1400
  #endif  

  /* MSMA Voltage setting  = 2.6 Volts */
  #define MSMA_LEVEL        2600

  /* Which VREGs we need to enable by default. */
  #define DEFAULT_VREGS_ENA  (PM_VREG_WLAN_M)
  /* Which VREGs we need to disable by default. */
  #define DEFAULT_VREGS_DIS  0
  
  #if !defined(FEATURE_PMIC_MANAGED_LDO)
  /* Which VREGs we need to enable if MANAGED LDOs Feature is disable */
  #define DEFAULT_RF_VREGS_ENA  (PM_VREG_RFTX_M  | PM_VREG_RFRX1_M | \
                                 PM_VREG_RFRX2_M | PM_VREG_SYNT_M)
  #endif /* FEATURE_PMIC_MANAGED_LDO */

/******************   VREG settings for the MSM6250 ***************************/
#elif defined(T_PMIC_MSM6250_CONFIG)
#error code not present
#elif defined(T_MSM6100)
#error code not present
#elif defined(T_MSM6025) || defined(T_MSM6050)
#error code not present
#elif defined(T_MSM6000)
#error code not present
#else
#error code not present
#endif


/*****************************************************************************
                        RF CARD SPECIFIC VREG SETTINGS
******************************************************************************/
#if defined(T_RF_ZRF6155)

/* RFRX1 Voltage setting = 2.60 Volts */
#define RFRX1_LEVEL         2600

/* RFRX2 Voltage setting = 2.85 Volts */
#define RFRX2_LEVEL         2850

/* TCXO Voltage setting  = 2.85 Volts */
#define TCXO_LEVEL          2850

/* RFTX Voltage setting  = 2.85 Volts */
#define RFTX_LEVEL          2850

/* SYNTH Voltage setting = 2.85 Volts */
#define SYNTH_LEVEL         2850

#elif defined(T_PMIC_PANORAMIX)
#error code not present
#elif defined(T_PMIC_PM7500)
#error code not present
#elif defined(T_PMIC_PM6650)

/* RFRX1 Voltage setting = 2.85 Volts */
#define RFRX1_LEVEL         2850

/* RFRX2 Voltage setting = 2.85 Volts */
#define RFRX2_LEVEL         2850

/* TCXO Voltage setting  = 2.85 Volts */
#define TCXO_LEVEL          2850

/* RFTX Voltage setting  = 2.85 Volts */
#define RFTX_LEVEL          2850

/* SYNTH Voltage setting = 2.85 Volts */
#define SYNTH_LEVEL         2850


#elif defined(T_PMIC_PM6640)
#error code not present
#elif defined(T_PMIC_PM6620)
#error code not present
#elif defined(T_PMIC_PM6610)
#error code not present
#else

 #error: This version of the PMIC code does not support this MSM.

#endif

/* =================== END of PMIC Voltage Level definitions ============= */

/*===========================================================================

                VARIABLES DEFINITIONS

===========================================================================*/

/*===========================================================================

                LOCAL FUNCTION PROTOTYPES

===========================================================================*/
void pm_vreg_init(void);

/*===========================================================================

                FUNCTION DEFINITIONS

===========================================================================*/
 


/*===========================================================================

FUNCTION pm_init                                EXTERNAL FUNCTION

DESCRIPTION
    This function initializes the PM60X0 for operation.

    It does the following:
    1)  It initializes the PMIC software driver memory
    2)  It initializes the LDOs voltages.
    3)  It initializes the PMIC HKADC driver.
    4)  It enables and starts the 32Khz external sleep crystal

INPUT PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  rflib_init() must have been called.

SIDE EFFECTS
  Interrupts are disable while writing and reading to/from sbi.

===========================================================================*/
void pm_init(void)
{
  static boolean  call_once = FALSE;
  #if defined(FEATURE_GSM) && !defined(BUILD_BOOT_CHAIN)
#error code not present
  #endif
  /* Make sure that this function only gets call once */
  if (call_once == FALSE)
  {  
     call_once = TRUE;

     /* Start communication with PMIC. */
     if (pm_init_comm() == PM_ERR_FLAG__SUCCESS)
     {
        /* Init model specific parameters such as no of regs, mpps */
        pm_model_specific_init();

        /* Initialize the Voltage Regulators */
        pm_vreg_init();
        
        /* The following code does not apply to the PM6610. */
        #if !defined(T_PMIC_PM6610)
            
            /* Init the PMIC ADC */
            pm_adc_init();     

            #if defined(FEATURE_PMIC_VARIABLE_PA_VDD)
            pm_variable_pa_vdd_init( FALSE );
            #endif

            #if defined(FEATURE_GSM) && !defined(BUILD_BOOT_CHAIN)
#error code not present
            #endif            
        
        #endif /* !T_PMIC_PM6610 */

        #if defined(T_PMIC_PM6640) || defined(T_PMIC_PM6620) || defined(T_PMIC_PANORAMIX)
#error code not present
        #endif
        
        #if defined(FEATURE_PMIC_TCXO_CONTROL)
        /* Set TCXO Control to automatic mode (TCXO_EN pin control) */
        (void)pm_tcxo_cmd(PM_TCXO_AUTOMATIC_MODE_CMD);
        #endif               
        
        #if defined(FEATURE_PMIC_MOMENTARY_SUDDEN_PWR_LOSS)
        /* Configure the Sudden Momentary Power Loss feature timeout
           value */
        (void)pm_set_smpld_timer(PM_SMPL_TIMER__2000msec);
        /* Enable the Sudden Momentary Power Loss feature timeout
           value */
        (void)pm_smpld_switch(PM_ON_CMD);

        /* Enable Charging of the Coin Cell */
        (void)pm_coin_cell_chg_switch(PM_ON_CMD);
        #endif
        
        /* Start PMIC diagnostic services. */
        pm_app_diag_init();
              

        #if defined(FEATURE_PMIC_WDOG)
        /* Enable Watch Dog */
        (void)pm_watchdog_reset_detect_switch(PM_ON_CMD);
        #endif

        #if defined(FEATURE_PMIC_HS_SEND_END_DETECT) && !defined(BUILD_BOOT_CHAIN)
        pm_hsed_init();
        #endif
        #ifndef BUILD_BOOT_CHAIN 
        #if defined FEATURE_PMIC_PROCCOMM_SUPPORT
        (void)pm_register_pcil_cmds();
        #endif

        #endif
    
        #if defined(FEATURE_PMIC_VIDEO_AMPLIFIER)
            pm_vid_init();
        #endif

        #ifdef FEATURE_PMIC_SELECT_USB_LDO_INPUT_BASED_ON_USB_DEVICE_CONNECTION
#error code not present
        #endif

     }
     else
     {
         MSG_ERROR("FAILED TO COMM WITH PMIC",0,0,0);
     }
  } /* call_once? */

} /* end of pm_init() */

/*===========================================================================

FUNCTION pm_init_delayed                                   EXTERNAL FUNCTION

DESCRIPTION
    This function initializes the PM60X0 ISR services. It disables all PMIC
  IRQs and it registers the PMIC ISR with the GPIO software driver. It also
  hooks up ISR for RTC power reset IRQ.

INPUT PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
    The following functions need to be called before we call this
  function:

  1) pm_init()
  2) tramp_init()
  3) gpio_int_init()

SIDE EFFECTS
  None

===========================================================================*/
void pm_init_delayed(void)
{
  static boolean call_once = FALSE;
  /* Make sure that this function only gets call once */
  if (call_once == FALSE)
  {
    call_once = TRUE;
#if defined(T_QSC60X0) || defined(T_QSC6055) || defined (T_QSC1100)
    tramp_set_isr(TRAMP_PM_ISR, pm_isr);
#elif defined(T_QSC6270)
  tramp_set_isr(TRAMP_PMIC_IRQ, pm_isr);
#elif !defined(T_RUMI)
    /* Tell the MSM which GPIO the PMIC IRQ is at     */
    /* Tell the MSM that the PMIC IRQ is active low   */
    /* Enable the PMIC IRQ                            */
    /* Install the PMIC detect isr handler            */
    (void)gpio_int_set_detect  ( PM_INT, DETECT_LEVEL );
    (void)gpio_int_set_handler ( PM_INT, ACTIVE_LOW, pm_isr);
#endif /* !T_RUMI */

    #if defined(FEATURE_PMIC_RTC)
    // Enable the RTC power reset IRQ
    (void)pm_set_irq_handle(PM_PWR_RST_IRQ_HDL, pm_rtc_pwr_rst_isr);
    #endif //FEATURE_PMIC_RTC

    #if defined(FEATURE_PMIC_32K_XTAL)
    // enable sleep xtal osc halted IRQ
    (void)pm_set_irq_handle(PM_OSC_HALT_IRQ_HDL, pm_sleep_xtal_halted_isr);
    #endif //FEATURE_PMIC_32K_XTAL

    #if defined(FEATURE_PMIC_INTERNAL_IC_TEMPERATURE)
    /* Enable PMIC's internal temperature protection code. */
    pm_int_temp_protection_init();
    #endif /* FEATURE_PMIC_INTERNAL_IC_TEMPERATURE */

  } /* call_once? */

} /* end of pm_init_delayed() */

/*===========================================================================

FUNCTION pm_vreg_init                            INTERNAL FUNCTION

DESCRIPTION
  This function initializes the PMIC voltage regulators.

INPUT PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.

===========================================================================*/
/*lint -esym(765, pm_vreg_init) */
void pm_vreg_init(void)
{
  #ifndef FEATURE_PMIC_VREG_INIT_BYPASS  
      /* Initialize the different VREGs Voltage settings. */
      (void)pm_vreg_set_level(PM_VREG_MSMP_ID,  MSMP_LEVEL);
      (void)pm_vreg_set_level(PM_VREG_MSMA_ID,  MSMA_LEVEL);

      #if defined(T_PMIC_PM6650) || defined(T_PMIC_PM7500) || defined (T_PMIC_PM6640)
        #ifndef T_QSC6270
        (void)pm_vreg_set_level(PM_VREG_MSME_ID,  MSME_LEVEL);
        (void)pm_vreg_set_level(PM_VREG_MSMC1_ID, MSMC1_LEVEL);
        #endif /* T_QSC6270 */
      #elif defined(T_PMIC_PANORAMIX)  
#error code not present
      #elif defined(T_PMIC_PM6620)
#error code not present
      #elif defined(T_PMIC_PM6610)  
#error code not present
      #endif
      (void)pm_vreg_set_level(PM_VREG_TCXO_ID,TCXO_LEVEL);
  #endif /* FEATURE_PMIC_VREG_INIT_BYPASS */

  #ifndef FEATURE_PMIC_RF_LDO_INIT_BYPASS  
      /* RF VREGs */
      (void)pm_vreg_set_level(PM_VREG_RFTX_ID,RFTX_LEVEL);
      (void)pm_vreg_set_level(PM_VREG_RFRX1_ID,RFRX1_LEVEL);

      #if defined(T_PMIC_PM6650) || defined(T_PMIC_PM7500)
        (void)pm_vreg_set_level(PM_VREG_RFRX2_ID,RFRX2_LEVEL);
        (void)pm_vreg_set_level(PM_VREG_SYNT_ID,SYNTH_LEVEL);
      #endif
  #endif /* FEATURE_PMIC_RF_LDO_INIT_BYPASS */

  #ifndef FEATURE_PMIC_MISC_LDO_INIT_BYPASS  
      #if defined(T_PMIC_PM7500)
#error code not present
      #endif
  #endif /* FEATURE_PMIC_MISC_LDO_INIT_BYPASS */



  /* Enable VREG active pull down */
  (void)pm_vreg_pull_down_switch( PM_ON_CMD, PM_VREG_PDOWN_MSMC1_ID );
  (void)pm_vreg_pull_down_switch( PM_ON_CMD, PM_VREG_PDOWN_MSME_ID );
  (void)pm_vreg_pull_down_switch( PM_ON_CMD, PM_VREG_PDOWN_MSMP_ID );
  (void)pm_vreg_pull_down_switch( PM_ON_CMD, PM_VREG_PDOWN_MSMA_ID );

  if (DEFAULT_VREGS_ENA != 0) /*lint !e506 !e774 */
  {
    /* The following VREGs need to be enable during boot up. */
    (void)pm_vreg_control(PM_ON_CMD,DEFAULT_VREGS_ENA);
  }

  if (DEFAULT_VREGS_DIS != 0) /*lint !e506 !e774 */
  {
    /* The following VREGs need to be disable during boot up. */
    (void)pm_vreg_control(PM_OFF_CMD,DEFAULT_VREGS_DIS);
  }

  #if !defined(FEATURE_PMIC_MANAGED_LDO)
    /* VREGs that need to be enabled if MANAGED LDOs Feature is disabled. */
    (void)pm_vreg_control(PM_ON_CMD,DEFAULT_RF_VREGS_ENA);
  #endif
    
  #if defined(FEATURE_PMIC_LOW_POWER_MODE)
    /* Enable low power mode for MSMP and MSMA */
  (void)pm_lp_mode_control(PM_ON_CMD, PM_VREG_LP_MSMA_ID);
  (void)pm_lp_mode_control(PM_ON_CMD, PM_VREG_LP_MSMP_ID);

  #endif

  #if defined (T_MSM6246) || defined (T_MSM6290)
  /* for 6246 power optimization, make the following settings */
    (void) pm_lp_mode_control( PM_ON_CMD, PM_VREG_LP_MSMP_ID );
    (void) pm_lp_mode_control( PM_ON_CMD, PM_VREG_LP_MSMA_ID );
    (void) pm_lp_mode_control( PM_ON_CMD, PM_VREG_LP_RUIM_ID );
    (void) pm_lp_mode_control( PM_ON_CMD, PM_VREG_LP_RFRX2_ID );
    (void) pm_lp_mode_control( PM_ON_CMD, PM_VREG_LP_RFRX1_ID );
    (void) pm_lp_mode_control( PM_ON_CMD, PM_VREG_LP_RFTX_ID );
    (void) pm_lp_mode_control( PM_ON_CMD, PM_VREG_LP_TCXO_ID );
    (void) pm_lp_mode_control( PM_ON_CMD, PM_VREG_LP_MMC_ID );
    (void) pm_lp_mode_control( PM_ON_CMD, PM_VREG_LP_GP3_ID );
    (void) pm_lp_mode_control( PM_ON_CMD, PM_VREG_LP_MSME_ID );
    (void) pm_lp_mode_control( PM_ON_CMD, PM_VREG_LP_GP2_ID );

    (void) pm_vreg_set_level( PM_VREG_MSMP_ID, 2600 );
    (void) pm_vreg_set_level( PM_VREG_MSMA_ID, 2600 );
    (void) pm_vreg_set_level( PM_VREG_RUIM_ID, 2600 );
    (void) pm_vreg_set_level( PM_VREG_WLAN_ID, 2600 );
    (void) pm_vreg_set_level( PM_VREG_RFRX1_ID, 2100 );
    (void) pm_vreg_set_level( PM_VREG_RFRX2_ID, 2700 );
    (void) pm_vreg_set_level( PM_VREG_TCXO_ID, 2850 );
    (void) pm_vreg_set_level( PM_VREG_USB_ID, 3300 );
    (void) pm_vreg_set_level( PM_VREG_MMC_ID, 2850 );
    (void) pm_vreg_set_level( PM_VREG_GP3_ID, 2800 );
    (void) pm_vreg_set_level( PM_VREG_MSME_ID, 1800 );
    (void) pm_vreg_set_level( PM_VREG_GP1_ID, 2850 );
    (void) pm_vreg_set_level( PM_VREG_GP2_ID, 2850 );
    //OVP
    pm_usb_ovp_enable(FALSE);
    pm_usb_ovp_set_threshold(PM_USB_OVP_THRESHOLD_6V);
    pm_usb_ovp_set_hystersis(PM_USB_OVP_HYSTERSIS_40ms);

  #endif
}

#endif /* FEATURE_PMIC */

