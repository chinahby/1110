
#ifndef CUSTPMIC3_H
#define CUSTPMIC3_H
/*===========================================================================

DESCRIPTION
  Configuration for PMIC3 used by MSM6800.

  Copyright (c) 2005  by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/pmic/target/main/qsc1100/build/cust/src/custpmic3.h#16 $ $DateTime: 2009/01/08 18:40:29 $ $Author: jnoblet $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/08/09   jtn     Removed #define FEATURE_BATTERY_CHARGER_A0_WORKAROUND
11/26/08   aab	   Removed features related to detection USB device via VCHG pin
                           #define FEATURE_PMIC_USB_DETECTION_USING_VCHG
                           #define FEATURE_PMIC_USB_VBUS_POWER_UNAVAILABLE
                           #define FEATURE_PMIC_USB_VBUS_POWER_UNAVAILABLE_BUT_USB_POWER_MGMT_REQD
                           #undef FEATURE_PMIC_NONFUNCTIONAL_VREG_USB

11/26/08   aab	   Added   #define FEATURE_PMIC_USB_DETECTION_USING_VCHG
                           #define FEATURE_PMIC_USB_VBUS_POWER_UNAVAILABLE
                           #define FEATURE_PMIC_USB_VBUS_POWER_UNAVAILABLE_BUT_USB_POWER_MGMT_REQD
                           #undef FEATURE_PMIC_NONFUNCTIONAL_VREG_USB
                           #define FEATURE_PMIC_KIP_HAN
11/21/08   jtn     Added #define FEATURE_PMIC_MPP4_USED_AS_AMUX_INPUT
10/14/08   aab     added #define FEATURE_PMIC_USB_DETECTION_USING_VCHG
10/09/08   jtn     Added #define FEATURE_PMIC_PMIC4_HSED
09/23/08   vk      Added #define FEATURE_BATTERY_CHARGER_A0_WORKAROUND
08/08/08   aab     Added #define FEATURE_PMIC_PERIPHERAL_MODE_ONLY_OPERATION for QSC1100 target
07/29/08   jtn     Added #define FEATURE_PMIC_WDOG
07/02/08   hs      Removed #define FEATURE_BATTERY_CHARGER
                   Removed #define CHG_TARGET_H "chg_pm6650_config.h"
06/17/08   jtn     Removed #define FEATURE_PMIC_VARIABLE_PA_VDD
05/21/06   aab     For QSC1100 target only,  Added #define FEATURE_PMIC_USB_ID_SIGNALING_UNAVAILABLE
01/19/06   cng     Initial release for QSC6055 (derived from Dora)
11/08/05   Vish    Initial Version.

===========================================================================*/

/* -----------------------------------------------------------------------
** drivers/pm
** ----------------------------------------------------------------------- */
#define FEATURE_PMIC
#define T_PMIC_PM6650


// ******************************************************
// Features that are PMIC3 specific are listed below ...
// ******************************************************
#if defined(FEATURE_PMIC)
  /* This feature specifies that we are using the Multi-Mode PMIC VU */
  #undef FEATURE_PMIC_MM
  #undef FEATURE_PMIC_RINGER
  // Enable only LDOs on PM1000
  #undef  FEATURE_PM1000_LDO_ONLY
  


#if !defined(BUILD_BOOT_CHAIN) && \
    !defined(BUILD_JNAND)
  /* Enable the 32K external crystal. RTC will not work
  with the internal RC clock. */
  #define FEATURE_PMIC_32K_XTAL

  /* Enable RTC */

  #define FEATURE_PMIC_RTC 

  #define FEATURE_PMIC_NO_RTC_ALARM 
#endif
  /* Enables the PMIC TCXO management. It allows DMSS to turn TCXO ON/OFF by
  toggling the GPIO connected to PMIC TCXO_EN pin */
  #define FEATURE_PMIC_TCXO_CONTROL

  /* Enable DMSS to turn the voltage regulators ON/OFF
  */
  #define FEATURE_PMIC_MANAGED_LDO
  
  #define FEATURE_PMIC_LCDKBD_LED_DRIVER

  #define FEATURE_PMIC_HS_SEND_END_DETECT

  #define FEATURE_PMIC_HAS_SSBI  

#ifndef BUILD_BOOT_CHAIN
  #define FEATURE_PMIC_INTERNAL_IC_TEMPERATURE
#endif
   
  /* Voltage setting for PMIC on boot up
  */
  #define FEATURE_PMIC_MSMC_1p375V

  #define FEATURE_PMIC_LOW_POWER_MODE

  #define FEATURE_PMIC_USB_OTG
  
  
  #ifdef T_FFA  
#error code not present
  #else  
    // ******************************************************
    // Features that are PMIC3-SURF specific are listed below ...
    // ******************************************************
    #undef FEATURE_PMIC_TEMP_CHG
    // FEATURE_PMIC_MPP2_CONTROLS_USB_VBUS_BOOST_ASSIST requires modified external PMIC d/c circuitry
    #define FEATURE_PMIC_MPP2_CONTROLS_USB_VBUS_BOOST_ASSIST
  #endif


  #ifdef BUILD_BOOT_CHAIN
    // ******************************************************
    // Features that are PMIC3-Bootloader specific are listed below ...
    // ******************************************************
    #undef FEATURE_PMIC_DIAG_SERVICES
  #else
//    #define FEATURE_PMIC_DIAG_SERVICES
  #endif  
  // **********************************************************
  // WARNING!   The above shall be the last section and no feature
  //            definitions be made after #ifdef BUILD_BOOT_CHAIN block
  // **********************************************************
#if defined(T_QSC6055) || defined(T_QSC6085)  
/***********************************************************/
  #define FEATURE_VBATDET_CALIBRATION
  #define FEATURE_PMIC_MOMENTARY_SUDDEN_PWR_LOSS
  #define FEATURE_BATTERY_CHARGER_SINGLE_CHARGING_PATH

/***********************************************************/
#endif //#if defined(T_QSC6055) || defined(T_QSC6085)  
#endif /* FEATURE_PMIC */

/*  Bypass setting VREG voltages in PMIC */
#define FEATURE_PMIC_VREG_INIT_BYPASS

#define FEATURE_PMIC_USB_ID_SIGNALING_UNAVAILABLE
#define FEATURE_PMIC_PERIPHERAL_MODE_ONLY_OPERATION
#define FEATURE_PMIC_KIP_HAN

//Features related to detection USB device via VCHG pin:
//#define FEATURE_PMIC_USB_DETECTION_USING_VCHG
//#define FEATURE_PMIC_USB_VBUS_POWER_UNAVAILABLE
//#define FEATURE_PMIC_USB_VBUS_POWER_UNAVAILABLE_BUT_USB_POWER_MGMT_REQD
//#undef FEATURE_PMIC_NONFUNCTIONAL_VREG_USB

#define FEATURE_PMIC_WDOG 

#endif /* CUSTPMIC3_H */


