#ifndef CUSTRF_H
#define CUSTRF_H
/*===========================================================================

DESCRIPTION
  Configuration for RF

  Copyright (c) 2002,2003,2004  by QUALCOMM Incorporated. All Rights Reserved.
  Copyright (c) 2005            by QUALCOMM Incorporated. All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/target/main/qsc1100/build/cust/src/custrf.h#2 $ $DateTime: 2008/04/16 18:33:53 $ $Author: gswiech $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/14/08   vm      Disable the MDSP Based TX AGC feature for RICO bringup
11/29/06   vm      Added FEATURE_MDSP_SUPPORTS_TX_AGC for supporting
                   TX AGC in MDSP
04/06/05   dyc     Enable FEATURE_INTELLICEIVER
03/07/05   dyc     Enable FEATURE_HDR_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST
02/20/05   dyc     Provide FEATURE_HDR_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST definition
01/30/05   dyc     Provide FEATURE_RF_PA_DVS definition.
08/26/04   bmg/dyc Enabled FEATURE_RF_GET_BAND_CAPABILITY for
                   FEATURE_ENHANCED_BAND_CLASS.
06/24/04    st     Undefine FEATURE_TX_ACCESS_POWER_FIX for MSM6500.
04/22/04    et     removed CELL/PCS condition around INTERPOLATION feature.
04/05/04    et     added  FEATURE_LIN_INTERPOLATION_FREQ_COMP under CELL or PCS
01/07/04   dyc     Support for FEATURE_DIFFERENTIAL_RX_MOD_CLK
                   Update PMIC features.
11/05/03   bgc     Added PMIC_TCXO_CONTROL.
10/23/03   dnn     Removed T_FFA/undef FEATURE_PMIC_SPEAKER 
07/09/03   aks     Disable sub-features of PMIC, until they become available.
05/01/02   jct     Created
===========================================================================*/

/* ------------------------------------------------------------------------
** RF
** ------------------------------------------------------------------------ */
#ifdef FEATURE_GSM
#error code not present
#endif
/* Perform ZIF logging
*/
#define FEATURE_ZIF_LOGGING

/* RF Warmup reduction from 42ms.
*/
#define FEATURE_FAST_WARMUP

/* Enable TX ACCESS POWER Fix.
*/
#undef FEATURE_TX_ACCESS_POWER_FIX

/* Clock PA gain state machine at 800Hz (PCG) to enable Tx Gain Adjust to
** occur according to the tx data rate synchronized to tx frame boundaries.
*/
#define FEATURE_IS2000_CAGC

/* Enable NV item to calculate the PA warmup delay used for puncturing during
** transmit.
*/
#define FEATURE_SUBPCG_PA_WARMUP_DELAY

/* Enable bypass and powering down of PA during normal CDMA transmission.
** NOT YET FULLY SUPPORTED - DO NOT DEFINE.
*/
#undef FEATURE_PA_BYPASS_ENABLE

/* Support for RFCAL version.
*/
#define FEATURE_RFCAL_VERSION

/* Support for Rx modulation clock (analog bbrx) to be used in differential
** mode.  Otherwise, single ended mode configuration.
*/
#undef FEATURE_DIFFERENTIAL_RX_MOD_CLK

/* Interpolate various RF values such as lim_vs_freq, lna_offsets, 
   vga_gain_offset.. in order to get better more accurate Tx power.
   Supported for all bands */

#define FEATURE_LIN_INTERPOLATION_FREQ_COMP

/* ------------------------------------------------------------------------
** SBI
** ------------------------------------------------------------------------ */
#define FEATURE_SSBI
//#define FEATURE_SBI
#ifdef FEATURE_GSM
#error code not present
#endif
#define FEATURE_NEW_SBI

/* ------------------------------------------------------------------------
** PMIC
** ------------------------------------------------------------------------ */

#if defined(FEATURE_PMIC)

  /* Enables the PMIC TCXO management. It allows DMSS to turn TCXO ON/OFF by
  ** toggling the GPIO connected to PMIC TCXO_EN pin
  */
  //#define FEATURE_PMIC_TCXO_CONTROL

  /* Enables the PMIC ringer. It configures DMSS to use the PMIC ringer
  */
  #undef FEATURE_PMIC_RINGER

  /* Enable the 32K external crystal. RTC will not work
  ** with the internal RC clock.
  */
  //#undef FEATURE_PMIC_32K_XTAL

  /* Enable DMSS to turn the voltage regulators ON/OFF
  */
  #define FEATURE_PMIC_MANAGED_LDO
  
  /* Enables PA-DVS support
  */
  //#define FEATURE_RF_PA_DVS

#endif

/* CRF3100-PA and QRF3300-1 uses RFR3100
** Enables reading of Forward Mode Rx CAGC RF Cal items.
*/
#define FEATURE_RFR3100

/* Features to use the improved TX AGC supported by CRF3100-PA
** Features enable reading of Forward Mode Tx CAGC RF Cal items.
*/

#define FEATURE_PA_RANGE_TEMP_FREQ_COMP
#define FEATURE_4PAGE_TX_LINEARIZER
#define FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST
#define FEATURE_HDR_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST

#ifdef FEATURE_ENHANCED_BAND_CLASS
  #define FEATURE_RF_GET_BAND_CAPABILITY
#endif /* FEATURE_ENHANCED_BAND_CLASS */

/* Enable INTELLICEIVER functionality */

#define FEATURE_INTELLICEIVER

#define FEATURE_ADC_VOTE_FOR_SLEEP

#if defined(T_QSC6055) || defined(T_QSC6085)|| defined(T_QSC1100)
#if !defined(T_QSC1100_RICO_VOLTRON) 
#define FEATURE_MDSP_SUPPORTS_TX_AGC
#endif
#endif
 
#endif /* CUSTRF_H */

