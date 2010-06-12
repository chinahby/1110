#ifndef RFICAP_MM_CDMA1X_H
#define RFICAP_MM_CDMA1X_H

/*===========================================================================

             R F   C a p a b i l i t i e s   H e a d e r   F i l e

DESCRIPTION
  This header file that defines the band/mode capabilities and macros which
  are used by the RF unit.  This header file is internal to the RF unit.

Copyright (c) 1997, 1998, 1999 by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2000, 2001, 2002 by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2003, 2004, 2005 by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2006, 2007, 2008 by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2009             by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //source/qcom/qct/modem/rfa/rf/common/main/qsc1100/src/rf/rficap_mm_cdma1x.h#23 $
$DateTime: 2009/01/12 19:03:06 $
$Author: vboda $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/12/09   vb      Disabled the feature RF_HAS_SUPPORT_FOR_LO_PWC_CAL
01/09/09   vb      Added the feature RF_HAS_RXADC_VOLTAGE_SWITCH
12/10/08   lp      Removed BC6 Band support for CS Release
12/03/08   vm      Added the Support for Temperature based Device
                   Configration change
11/18/08   lp      Added support for variable SMPS level
11/14/08   vm      Added the support for Variable Switcher Size and Frequency
11/07/08   vm      Enabled the support for BC6 Band
10/12/08   vm      Added feature for different PA Switchpoints in Access Probes
10/12/08   vm      Enabled the Feature RF_HAS_SUB_BLOCK_SUPPORT
10/12/08   vm      Added support for PDET Waveform Compensation and Variable 
                   PMIC Switcher Size
10/12/08   vm      Added support for Tx DAC Backoff
10/01/08   vm      Disabled the Feature RF_HAS_SUB_BLOCK_SUPPORT for time
                   being until tested
09/30/08   vm/vb   Added Support for Band Class Sub Block Implementation
09/28/08   vm      Undefined the support for BC6 Band and Tx DAc Backoff
09/14/08   vm      Added the support for TCXO Shutdown
09/14/08   vm      Added the support for BC6 Band
09/14/08   vm      Added the support for Tx DAC Backoff
08/29/08   vm      Added Feature for LO PWC Cal
08/19/08   sl      Enabled LO PWC feature.
08/06/08   vm      Mainline feature RF_HAS_SUPPORT_FOR_RC_PROCESS_ERROR_CAL
07/25/08   sl      Enabled RC Tuner feature. Added feature RF_HAS_COMMON_NV_TABLE.
06/13/08   vm      Enable PDET on QSC1100 and add feature for its 16 bit
05/20/08   vm      Define RF_SUPPORTS_BYPASS_PA_FOR_BC0 for QSC1100
05/05/08   vm      undefine RF_SUPPORTS_BYPASS_PA_FOR_BC0 for qsc1100
05/01/08   sl      Added entries for RF_HAS_SUPPORT_FOR_LO_PWC_CAL feature.
04/16/08   vm      Added support for BC5 Band and new Bringup feature for QSC1100
03/28/08   vm      Added the support for Tx AGC interface with MDSP on QSC1100
03/21/08   sl      Added RF_HAS_SUPPORT_FOR_RC_PROCESS_ERROR_CAL feature under T_QSC1100.
				   Removed FEATURE_SC2X_SW_PORT feature.
03/14/08   vm      Change the RFT SSBi Bus to Modem Bus
03/05/08   vm      Added new feature for RICO Bringup
                   Disable the feature set not needed on RICO or Napolean
12/08/07   vm      Added RF_HAS_1X_PA_SWITCH_POINTS_DIFF_FROM_NV and 
                   RF_HAS_HDR_PA_SWITCH_POINTS_DIFF_FROM_NV for Sc2x
11/15/07   ems     Support for QSC1100
11/10/07   adk     Defined RF_HAS_CPP_SUPPORT_FOR_RVTC_2_2 and
                   RF_HAS_RFCONFIG_RX_LNA for T_MSM7600 (and not for T_MSM7500).
11/07/07   adk     Defined RF_HAS_CPP_SUPPORT_FOR_RVTC_2_2 for T_MSM7500 (and T_MSM7600)
10/27/07   ans     Changed FEATURE_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST name to 
                   RF_HAS_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST.
                   Changed FEATURE_HDR_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST name to 
                   RF_HAS_HDR_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST.
                   Enabled RF_HAS_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST.
                   Enabled RF_HAS_HDR_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST.
10/26/07   ans     Disabled FEATURE_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST for SC2x.
                   Added FEATURE_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST for backoffs
                   for R2 and R3 PA states.
10/02/07   dyc     For SC2x added:
                      RF_HAS_BC_SPECIFIC_TX_PA_MAP_VALUE
                      Enabled RF_HAS_RFCONFIG_RX_LNA
10/01/07   adk     T_MSM7600: undef RF_HAS_HW_TRF6285, def RF_HAS_INTERNAL_HDET_SUPPORT
09/27/07   ans     Removed feature RF_HAS_SIGMA_DELTA_MAX_LEVEL_170, 
                   instead added runtime support for this feature.
09/26/07   dyc     Disable RF_HAS_RFCONFIG_RX_LNA for SC2x
09/23/07   dyc     Added SC2x LINK FFA configuration.
                   Added additional bands for SC2x in FTM HWTC
09/21/07   vm/dyc  undef RF_HAS_NO_HDR_HDET_COMP to enable
09/10/07   dyc     Support for RF_HAS_RFA_POWER_COLLAPSE
09/09/07   dyc     SC2x support for RF_HAS_SIGMA_DELTA_MAX_LEVEL_170,
                   RF_HAS_LOAD_IM_CTLS
08/28/07   dyc     Update band class support for SC2x sample code.
08/28/07   vm      Added the feature to have different PA switchpoints for 
                   1X and HDR 0n Sc2x 
08/24/07   vm      Eliminate the feature RF_HAS_PA_BYPASS for Sc2x
                   Undef the feature RF_HAS_ENC_TX_ENABLE_CALL_AS_LEGACY for Sc2x
08/22/07   ans     Added RF_HAS_PA_AND_TX_WARMUP_TIMES_DIFF_FROM_LEGACY for all 
                   targets.
08/20/07   ka      Added RF_HAS_MSMA_CURRENT_REDUCTION_CONTROL.
08/15/07   ka      Added features for internal therm.
08/13/07   cv      Defined the feature RF_HAS_HDET_COMP_FOR_RC_AND_TEMP for UNDP/MDM1000
08/10/07   avi     Added support for QRF7600
08/10/07   vm       Defined the feature to program the default PA Offsets to 0
                    for Sc2x
08/10/07   vm      Changed the feature name from RF_HAS_DIFF_PA_HYST_TIMER_ACC_TRAFFIC
                   to RF_HAS_PA_ACCESS_PROBE_CONFIG to be generic
08/07/07   vm      Added the feature RF_HAS_DIFF_PA_HYST_TIMER_ACC_TRAFFIC
                   for PA Hyst timer to be different in Access and Traffic 
                   for Sc2x
08/06/07   jfc     Featurized code for EDGE-only builds
08/01/07   dyc     Remove RF_HAS_RFCAL_NOT_LOADING_PWR_BASED_TX_COMP_NV 
                   Undef RF_HAS_NV_SUPPORT_FOR_8_THERM_BINS until needed
07/28/07   vm      Added the following fetaures for Sc2X
                   Feature RF_HAS_INTERNAL_HDET_SUPPORT
                   Feature RF_HAS_TX_TEMP_FREQ_COMP_NV_BASED_ON_PWR for 
                   Temperature Compensation on Sc2X
                   Feature RF_HAS_RFCAL_NOT_LOADING_PWR_BASED_TX_COMP_NV 
                   temporarily defined until RF Cal does not load Frequency compensation 
                   through new NV
                   Feature RF_HAS_NV_SUPPORT_FOR_8_THERM_BINS to support 8 Thermistor
                   Bins in NV rathyer than legacy 2 bining                                       
07/17/07   sar     Fixed VREG and Int HDET Featurization for MSM7500I/J targets.
07/16/07   hkk     Tweaks for DO Rev0 compesation
07/14/07   dyc     Undef RF_HAS_HW_TRF6285 for SC2x
07/12/07   vm      -Added the feature for setting Spectral inversion for 
                   card based on device info
                   -Added the featutre so that HDR MDSP Variables for Tx 
                   Linearizers use 1X memory
                   -Added the feature so that Tx and PA warmup times can be
                   different from legacy
                   -Added the fature for chnaging PLL Lock Wait time to 
                    600us
07/13/07   sar     Added support for TRF6285.
06/29/07   rmd     undef RF_HAS_SYNTH_LOCK for SURF7500-90 + TRF6285. Synth lock
                   pin is not used on this target.
05/29/07   rmd     Added support for TRF6285.
06/29/07   vm      Defined the feature for management of TX_ON_EN by
                   Encoder for Sc2x
06/27/07   vm      Disable the HDET Compensation for HDR until tested
06/26/07   adk     Disable RF_HAS_RFCONFIG_RX_LNA; it needs further testing.
06/25/07   adk     Added feature RF_HAS_PDM_LIN_TBL_SIZ_48 so that PDM and
                   linearizer tables of size 48 can be used in Beta Scaling.
06/25/07   vm      Undefine the Hidden registers Cal for Dragon-3
06/25/07   vm      Define the feature RF_HAS_PDM_VS_POWER_TX_CAL for 
                   Dragon-3 support
06/25/07   dyc     Featurize Beta Scaling support for QSC6055 1.1 and later
05/24/07   Vish    Added RF_HAS_RFCONFIG_RX_LNA
06/15/07   vm      Enable BC15 support for NON Bypass PA
06/07/07   ans     Enable BC15 support for MSM6800B.
06/03/07   hkk     1x, DO and temp waveform compensation
06/01/07   vm      Added the support for Beta Scaling for SC2X
06/01/07   aak     Removal of JIDZRF card support
05/16/07   aak     Define FEATURE_MSM_ADIE_D2D_PAD3_OPTIMIZATION
05/08/07   ra      disable RF_HAS_ALTERNATE_GPS_ASIC 
05/03/07   adk     Enable Beta Scaling only for MDM1000 (T_UNDP).
05/03/07   sar     Relocated feature block for T_MSM7500I and added definitions 
                   for WDZRF and DZRF6285 card files.
04/30/07   vm      Added support for New Tx Cal for RF chips with 
                   Tx Compensation registers in RFT
04/25/07   hkk     Removed WDZRF and RTR6285
04/05/07   ra      Added  RF_HAS_ALTERNATE_GPS_ASIC
04/03/07   vm      Added the support for Notch filters for
                   1x and diversity
04/02/07   sar     Enabled RF_HAS_POWER_MODE_LIB, RF_HAS_HW_TRF6500, and 
                   RF_HAS_HW_QSC60X5 for MSM7500/MSM7600.
03/30/07   bmg     Added RF_HAS_INTERNAL_HDET_SUPPORT for all targets
03/30/07   dyc     Added RF_HAS_SUPPORT_FOR_BAND_CLASS_15 for SC2x.
                   Re-organize SC2x featurization section.
03/28/07   sar     Fixed featurization for MSM7500/MSM7600.
03/12/07   vin/hkk Added FEATURE_WLAN_MODIFIED_FFA
03/26/07   bmg     Removed QSC6055 RF cards from MSM* targets, it will still
                   be enabled in the T_QSC60X5 section
03/25/07   dyc     Remove RF_HAS_DEDICATED_RF_CARD implementation and assign
                   default RF configuration for QSC60x5 targets.
                   Added RF_HAS_BYPASS_BAND_CHANNEL_CHECK for QSC60X5 and 7600.
03/22/07   dyc     Restore RF_DISABLE_MDSP_TX_UNUSED_REGS for QSC60x5 
03/19/07   dyc     Support featurization for all RF card types and configure
                   for QSC60x5 and 7500.
03/16/07   sar     Featurized out Power Mode for MSM7500.
03/16/07   dyc     Enable RF_HAS_HDET_COMP_FOR_RC_AND_TEMP for QSC60x5
03/14/07   jfc     Removed RF_HAS_HDET_BYPASS to enable HDET feature
03/14/07   vm/dyc  Removed the feature for support for BC5 and BC11 bands.
03/08/07   adk     Enabled Beta Scaling via flag RF_HAS_BETA_SCALING_TX_AGC.
                   Made SSBI port value sent to MDSP target-dependent.
                   Removed #defines for SSBI port values. Used
                   a function-call to provide this information.
03/04/07   dyc     Add upport for QSC60x5
02/27/07   go      Enabled RF_HAS_HDET_BYPASS for MSM7600
02/15/07   sar/go  added feature RF_HAS_MSM7600
02/09/07   vm/adk  Added support for firmware TX AGC and PA bypass
02/08/07   go      SSBI-RTR 7600/7500A fix
01/18/07   adk     Moved RF_HAS_LINEAR_INTERPOLATION to rficap_mm.h
12/12/06   hkk     BC15 support for 6800 6.0 release
11/28/06   adk     Commented out #define RF_HAS_FATAL_ERRORS (for now).
11/15/06   ra      Moved GPS support to new header file rficap_mm.h
                   because GPS feature check should not be in a 1x header file.
11/14/06   aak     Added support for BC 15 (RF_HAS_SUPPORT_FOR_BAND_CLASS_15)
10/17/06   ycl     Modifications for initial MSM7600 build.
08/16/06   bmg     Adjust the behavior of RFT6150 based cards depending on
                   SURF/FFA configuration.  If it is acceptable to disable
                   the PA therm VREG, define RF_HAS_DISABLEABLE_THERM_VREG.
07/11/06   lp      Added support for dynamic intelliceiver GPIO configuration
                   for MSM7500
07/11/06   giri    Core services build support. To get rid to compilation error
                   of not including the cdma feature file and including this 
                   header file.
07/11/06   hkk     Changed default RF card to DZRF
06/15/06   sq      Added RF_HAS_HDR_AGC_OPTIMIZATION_P1
06/12/06   hkk     Added RF_HAS_SUPPORT_FOR_FULL_POWER_COLLAPSE 
06/06/06   vm      Added the support for new feature RF_HAS_HDR_REVA_TX_POWER_BACK_OFF
04/05/06   hkk     Updated RFT_SSBI_BUS for MSM6800 65nm 
03/29/06   rmd     Updated the INTELLICEIVER_GPIO definition for the MSM6500.
03/21/06   lp      Fixed INTELLICEIVER_GPIO for MSM7500
03/20/06   bmg     Changed INTELLICEIVER_GPIO to GPIO header file definitions
                   to fix CR #89080
03/07/06   hkk     Added RF_HAS_TX_AGC_RATE_ADJ_READY_BIT
03/03/06   hkk     Updates for having RF on SSBI for Dora65
01/18/06   sar     Enabled feature RF_HAS_BYPASS_RF_STATE_GPS_CHECK.
01/16/06   rmd     Implemented BC11 and BC14 code review results. Including:
                   * Mainline BC11 and BC14 code.
01/13/06   lp      Added SSBI support for DZRF6500/JTDZRF6500. Created RF_HAS_SSBI
                   feature that supports SBI/SSBI which is triggered of 
                   FEATURE_SSBI in custrf.h
12/15/05   sar     Set RF_DEF_HW to RF_HW_ZRF, from 1 to remove lint error. 
11/14/05   rmd     Added initial support to BC11.
10/27/05   bmg     Added RF_ASSERT and RF_FATAL_ERROR support
10/21/05   rmd     Added initial support to BC14.
09/28/05   dyc     Enabled RF_HAS_WU_TIMED for 6800.
09/15/05   dyc     Support for FEATURE_RF_WU_OPTIMIZATION.
08/11/05   ra      fixed bug for DORA and Phoenix regarding intelliceiver GPIO definition
08/08/05   ra      moved intelliceiver GPIO definition here.
07/19/05   hkk     Added support for park mode
07/12/05   hkk     Moved support for RF_HDR_USE_ZERO_TX_DB_OFFSET to Jaguar baseline.
07/07/05   sar     Mainlined feature RF_HAS_ENHANCED_BAND_CLASS.
07/07/05   sar     Eliminated support for legacy bands.
07/05/05   lp      Added RF_USE_GET_IMAGE_PTR and RF_HAS_NO_EXT_SYNTH
                   for T_MSM7500
06/13/05   dbc     Removed support for RF_HAS_MFLO
05/30/05   ra      Added RF_HDR_USE_ZERO_TX_DB_OFFSET
05/20/05   dyc     Enable RF_HAS_HDR_NV_PA_MAPPING as default.
05/09/05   ra      Added RF_HAS_RUNTIME_GPIO_CONFIG to Eagle and newer targets
                   Added the following definitions to DORA:
                   RF_BYPASS_SYSTEM_CTL,RF_HAS_MODULATOR_TX_CTL,RF_HAS_MODEM_PA_CTL,
                   RF_HAS_MODEM_GENERIC_CTL,RF_BYPASS_HDR_OVERDRIVE_DELAY, 
                   and RF_USE_TRAMP_PIC_ISR          
05/02/05   dbc     Define RF_HAS_MFLO when FEATURE_MFLO is defined.
04/22/05   dyc     Added RF_HAS_HDR_NV_PA_MAPPING for HDR PA mapping
04/21/05   lp      Added RF_HAS_BIO_ENCAPSULATION for MSM7500
04/06/05   ra      Added RF_HAS_NO_RTC_DEPENDENCY.
03/30/05   lp      Added T_MSM7500 and T_RUMI support.
03/21/05   bmg     Enabled NV support for FTM Cal V2.
                   Added feature checking for RF_HAS_POWER_MODE_LIB.
02/21/05   dyc     Support for RF_HAS_PA_DVS_NV
01/19/05   sar     Updated band class mask setting for GZRF6500.
09/08/04   sar     Added RF_HAS_SUPPRESS_BAND_REVERTING for msm6550 targets.
08/29/04   dyc     Mainline RF_HAS_DECOUPLED_PA_CTL
                   Remove RF_HAS_IDLE_N usage. (msm6000)
                   Make T_MSM6000 an un-supported configuration.
08/19/04   dbc     Added support for ZRF617X (CDMA 450) RF Card
08/16/04   dyc     Removed RF_HAS_FM component.
08/05/04   bmg     Enabled RF_HAS_ENHANCED_BAND_CLASS for T_MSM6550, supporting
                   old non-FEBC builds that still need band class NV
07/20/04   bmg     Added hardware case supporting FEATURE_ENHANCED_BAND_CLASS
07/13/04   sar     Added default Band Mask RF_DEF_BC.
06/15/04    et     restored RF_HAS_LINEAR_INTERPOLATION internal feature
                   to be defined under FEATURE_LIN_INTERPOLATION_FREQ_COMP
06/08/04   sar     Added condition to enable RF_HAS_ENHANCED_BAND_CLASS
                   when FEATURE_ENHANCED_BAND_CLASS is defined.
04/29/04   dyc     Added RF_HAS_RXF_DF2 definition.
04/09/04   jac     Added KPZRF defines
03/19/04   et      added interpolation (frequency comp) support
03/04/04   dbc     Remove usage of RF_HAS_RX1_PLL_GPIO_CONTROL. Use 
                   RF_HAS_CUSTOMER_DEFINED_SYNTH instead to determine if GPIOs
                   for external synth are configured.                   
02/24/04   dyc     Enable RF_HAS_MONOTONIC_LNA_GAIN_LINEUP
02/20/04   kss     Added RF_HAS_HDR.
02/18/04   aks     Add flag to featurize the use of HDET. Leave HDET enabled
                   for current RF cards.
02/10/04   dyc     Initially undef RF_HAS_MONOTONIC_LNA_GAIN_LINEUP feature for CZRF
                   Remove RF_HAS_DIFF_RXMOD_CLK, higher level feature defined in custrf.h
01/30/04   et      removed the usage of RF_HAS_TRIMODE
01/08/04   dyc     Support for RF_HAS_RX1_PLL_GPIO_CONTROL
12/08/03   ra      Added definition, RF_HAS_SINGLE_BAND_PCS, to JIZRF6500
11/12/03   dyc     Qualify RF_HAS_OLTA_BACKOFFS_IN_NV with target level feature.
11/05/03   dyc     Enable RF_HAS_SYNTH_LOCK for 6500 RF cards (surfs)
10/21/03   dyc     Remove FEATURE_HWTC_RF_HAS_GPS
10/21/03   sar     Added definition RF_HAS_NV_BANDS to select Band Specific 
                   NV Driver.
10/21/03   sar     Added definition for RF_HAS_BC0 and RF_HAS_BC1.
10/15/03   et      removed RF_HAS_PA_DVS and added sanity check for 
                   new FEATURE_RF_PA_DVS
10/13/03   ra      Added support for JIZRF6500 card
10/08/03   dyc     Disable RF_HAS_SYNTH_LOCK for GZRF (Tx gated)
10/08/03   zg      Added RF_HAS_MULTI_RX_CHAIN for multiple chain bbrx.
10/08/03   et      defined RF_HAS_PA_DVS under FEATURE_PMIC_MANAGED_LDO
                   if FEATURE_PMIC_VARIABLE_PA_VDD is turned on
10/07/03   et      defined/undefined RF_HAS_SYNTH_LOCK for various cards.
10/02/03   rs/dyc  Added GZRF6500 section
08/12/03   zg      Added RF_HAS_GPS_CLK_CHECK.
04/21/03   dyc     Support for ZRF6500 RF card.
03/03/03   dbc     Define RF_DSP_HAS_RX_SPECTRAL_INV_CONFIG for T_MSM6100 only.
02/21/03   dbc     Undefine RF_DSP_HAS_RX_SPECTRAL_INV_CONFIG for all targets.
02/21/03   aks     Separated out the CDMA1X and GSM1X flags.  Added another
                   API flag to allow for soft transition of interface usage.
02/21/03   aks     Changed to use target level FEATURE_CDMA1X, to turn on
                   GSM under 1X.
02/06/03   dbc     Added RF_DSP_HAS_RX_SPECTRAL_INV_CONFIG.  Changed order of
                   defines based on T_MSM6xxx. 
01/30/03   dbc     Added RF_HAS_NV_CAGC_LINEARIZER_RANGE_CONFIG.  It is #undef
                   by default.
01/29/03   dbc     Remove RF_USE_SLEEP_N
01/24/03   aks     Added RF_HAS_MULTIMODE_ARCH, based on target feature.
01/24/03   dbc     Added RF_HAS_VCO_CONFIG
01/21/03   dbc     Added RF_USE_SLEEP_N  
01/08/03   dbc     added RF_HAS_NV_VCO_CONFIG 
12/02/02   dbc     added RF_HAS_AUX_SBI_MUX_SEL under !FEATURE_FFA for ZRF612X
11/02/02   et      removed RF_HAS_FAST_AUX_SBI
11/27/02   et      added RF_HAS_AUX_SBI_MUX_SEL under !FEATURE_FFA for ZRF6000
11/22/02   aks     Add support for RF_HAS_GSM1X for MSM6300.
11/14/02   dyc     Support for RF_HAS_DFM_AGC_COMP
11/11/02   dbc     Added RF_HAS_RFR_VCO_COARSE_TUNING define
11/11/02   dyc     Support for RF_HAS_SURF_AUX_SBI.
11/05/02   dyc     Added RF_HAS_BYPASS_MIS_COMP, RF_HAS_DIGITAL_VGA_DELAY and
                   RF_HAS_DIFF_RXMOD_CLK support.
11/01/02   aks     Added RF_HAS_MULTIMODE_BBRX support.
10/30/02   dbc     Removed (mainlined) RF_HAS_GPIO_TLMM_CONFIG. Added
                   RFT_PCS_SUPPORTS_BAND_SELECTION define. 
10/02/02   dbc     Added support for T_RF_ZRF612X
09/04/02   dbc     Removed legacy defines.
08/29/02   dbc     Cleanup.
08/28/02   aks     Added RF_HAS_ANTENNA_MUX config.
08/26/02   zg      Removed testing feature FEATURE_GPSONE_DISABLE_SBI.
08/24/02   dyc     added FEATURE_HWTC_RF_HAS_GPS def for HWTC usage.
08/15/02   dbc     added support for RF_MANAGE_SYNTH_LDO.
08/07/02   et      added support for different TX and PA warmup times
                   w/ RF_HAS_DIFF_PA_TX_WARMUP
08/05/02    et     added RF_HAS_MULTIMODE_SBI under FEATURE_MULTIMODE_SBI
07/17/02   dyc     Removed RF_HAS_102_DB_DYNAMIC_RANGE and RF_RESCALE_TO_85_DB_RANGE
                   defines.
                   (merged from dbc rf.01.00.13)
07/09/02   dyc     Support for T_IO_CARD definition of LNA backwards mode
                   Added RF_HAS_GPIO_TLMM_CONFIG to Cougar target
                   Cougar Port baseline.
                   Added RF_HAS_DACC_REG_ENHANCE

06/20/02   zg      Added FEATURE_GPSONE_DISABLE_SBI for test only.
06/06/02   bgc     Added RF_HAS_FAST_AUX_SBI.
05/30/02   dbc     Cleanup.
05/22/02   dbc     Added RF_HAS_102_DB_DYNAMIC_RANGE and RF_RESCALE_TO_85_DB_RANGE
                   defines.
04/08/02   dbc     Defined RF_GATE_TX_ON_TO_IDLE_N for MSM6000
03/11/02   aks     Define Trimode if either CDMA_800 or CDMA_1900 exists.
                   Temporary solution until Trimode, Cell-only and PCS-only
                   modes are reviewed more thoroughly.
02/13/02   aks     Add definition for ZRF6000 card with Low-Tier PM6000.
02/13/02    et     defined RF_HAS_IM2
02/06/02   aks     Change format from T_MSM_* to T_MSM*.
01/14/02   dbc     Moved RF_CAGC_DEBUG flag to rf.c.
12/20/01   dbc     Added RF_CAGC_DEBUG flag which is used to enable CAGC
                   debugging.
12/14/01   dyc     Added RF_HAS_SYSTEM_MODE_REG under T_MSM_6050
11/21/01   dbc     RF_HAS_NV_DEBUG flag added to allow loading of default RF NV 
                   items.
11/14/01   aks     Change to T_RF_ZRF6000HT for High Tier PMIC.  
11/13/01   aks     Use 15 bit PDM.  Add ZRF6000 definitions.
11/02/01   tn      Added #define RF_HAS_PDM_CLOCK_SRC_SEL under T_MSM6050.
11/02/01   dbc     Removed backwards compatible flag RF_HAS_MSM6000_CAGC_REGS. 
                   Removed other non MSM6000+ flags.
10/25/01   dyc     Remove RF_HAS_SW_SLOTTING capability from MSM6000, MSM6050
10/25/01   dbc     Added support for TX_WARMUP (MSM6050) and mux of 
                   RX_AGC_ADJ/MIX_RANGE (MSM6000) 
10/09/01   dbc     RF_HAS_PA_RANGE_CAL support removed. 
                   FEATURE_PA_RANGE_TEMP_FREQ_COMP mainlined.
                   RF_HAS_DECOUPLED_PA_CTL, RF_HAS_IDLE_N, RF_GATE_TX_ON_TO_IDLE_N,
                   and RF_HAS_SINGLE_5_STAGE_LNA.
10/08/01   aks     Added features for FM software slotting, SuperHet and
                   Not-Ported to aid in porting.
10/01/01   dbc     Added RF_HAS_MSM6000_CAGC_REGS and RF_HAS_5_STAGE_CAGC_LNA_CTL
                   (FEATURE_5_STAGE_LNA) for MSM6000 port.
09/19/01   dbc     Removed RF_HAS_RFR3100 dependencies - SW LNA Control removed 
                   for CDMA and FM. NV Items introduced w/RF_HAS_RFR3100 mainlined.   
08/17/01   dbc     Added support for QRF5600 card. Removed FEATURE_RFT3100. 
                   RF_HAS_RFT3100 is defined depending on RF hardware.
07/20/01    et     made a temporary hack for which RF features should be defined
                   for the RFT5600 and RFR5600.  They will need to be moved.
07/03/01   aks     Added RF_HAS_RFT3100 configuration.  Required to distinguish
                   it from other RFT hardware.
05/18/01   dd      Added consistency check to detect both FM and IFR3500 being enabled.
                   Added #undef RF_HAS_IFR3500
03/19/01   tn      Enabled RF_APP_NOTE_CL93_V1562_1_X2 feature.
02/07/01   day     Change IFR reference for FFA.
01/15/01   fas     Added Cellular MSM5105 FFA.
10/18/00   fas     Copied entirely from PPG:
                   L:/src/asw/MSM3300/VCS/rficap.h_v   1.15   29 Sep 2000 15:10:26   dbrackma
10/06/00   dsb     Removed legacy defines that were unused.  Removed TG== 
                   code.
09/29/00   dsb     Removed support for RF_RFT_USES_IDLE.
08/28/00   dsb     Removed support for FEATURE_FFA3300 as an "RF card".
                   Removed RF_HAS_FREQPLAN2.  RF_HAS_TRIMODE support 
                   reinstated.
08/21/00   cah     Temporarily don't define RF_HAS_TRIMODE as it is not
                   supported yet.
07/25/00   dsb     Deleted #undef's for RF_HAS_BB2 and 
                   RF_CODEC_USES_BB2_IDLE, as these features no longer exist.
                   Added support for FEATURE_QRF3300.
06/26/00   dd      Merged with PP 3020 rf driver.
06/08/00   dsb     SURF3300 & FFA3300 support.
04/04/00   dd      Added PRF3300/IFR3300 support.
01/21/99   tn      added RF_HAS_RFT3100_A2. Removed all un-used target definitions.
01/14/00   tn      FFA support.
01/06/00   rv      Trimode support.
09/16/99   ymc     Integrated r1.2 change: introduced FEATURE_SURF3100 and deleted 
                   all PRODUCTION_SURF2000_xxx definitions.
09/15/99   ymc     Changed FEATURE_JSTD008 to FEATURE_CDMA_1900;
                   Undefined RF_HAS_PA_RANGE_CAL for PRF3000.  PRF3000 doesn't
                   have PDM1 and PDM2 either.
09/09/99   jc      Added PRODUCTION_SURF2000_REV2
07/15/99   tn/mk   Added RF_HAS_INTERNAL_CODEC, RF_HAS_IFR, RF_HAS_IFT,
                   RF_HAS_RFT defs. Also using PRODUCTION_SURF2000_REV0(1).
05/19/99   tn      Added support for IFR and IFT devices using RF_HAS_IFRIFT.
01/06/99   ychan   Modified for targets NGP, SURF, Q-1900.
01/06/99   ychan   Updated Copyright date.
06/21/98   jjn     Updated from code review.
04/09/98   thh     Added retune at RxTx transition.  Added a table
                   describing all current RF capability.
03/17/98   thh     Rx AGC limits and PDM1 and PDM2 in Tx will be used;
                   so the following the defined:
                     RF_LIMIT_RX_AGC_OUTPUT
                     RF_CAP_PDM1_FOR_TX
                     RF_CAP_PDM2_FOR_TX
02/10/98   thh     Added Q-target support.
                   Added new capability definition RF_CAP_RETUNE_RXTX.  When
                   RF_CAP_RXTX_RETUNE is defined RF card will get a request to
                   re-tune at RX/RXTX transition.
02/08/98   jjn     For targets that do not use RF_CONFIG NV item, defined
                   RF_TARGET_LACKS_RF_CONFIG_NV
02/03/98   kss     Moved Module RF to new RF architecture
11/17/97   jjn     Partitioned ISS2 RF to conform to the new RF architecture
08/14/97   thh     Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"     /* Target definitions   */
#include "customer.h"   /* Customer definitions */
#include "rficap_mm.h"  /* Need INTERPOLATION definition */


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

                                COMMON MACROS

=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/* EJECT  */
/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/

#define RF_HAS_LINEAR_INTERPOLATION                               

/* The target has TRIMODE capability if FEATURE_CDMA1900 and FEATURE_CDMA_800
** are defined.
*/

#if defined (T_MSM6050) || defined (T_MSM6000)
  #define RF_NOT_PORTED /* porting wrapper  */
#endif //6050 or 6000 MSMs

/* Default to TX_WARMUP available */
#define RF_HAS_TX_WARMUP

/* Default to using TX warmup time and PA warmup time 
   values from  RF-ZIF library*/
#define RF_HAS_PA_AND_TX_WARMUP_TIMES_DIFF_FROM_LEGACY 

/* Use this switch to choose between reading the RF NV items from
** NV or using the default values populated in the structures 
** containing the RF NV items.  Enclose the data item you want
** the default value for in rfnv_get_nv_data() within this define
*/
#undef RF_HAS_NV_DEBUG

#undef RF_LIB_REV_DEBUG

/*
** 5 Stage LNA with Fixed Mixer gain
*/
#undef RF_HAS_SINGLE_5_STAGE_LNA

/* Define this if NV items are to be used to set the CAGC linearizer range */
#undef RF_HAS_NV_CAGC_LINEARIZER_RANGE_CONFIG

/* Enable support for 1x NV PA Range mapping for HDR */
#define RF_HAS_HDR_NV_PA_MAPPING

/*  Alow/Disallow SYNTH LDO to be managed when FEATURE_PMIC_MANAGED_LDO is defined */
#if defined (FEATURE_PMIC_MANAGED_LDO)

#define RF_MANAGE_SYNTH_LDO

/* sanity check to make sure FEATURE_PMIC_VARIABLE_PA_VDD is defined
   when trying to execute code under FEATURE_RF_PA_DVS */

#if defined(FEATURE_RF_PA_DVS) && !defined(FEATURE_PMIC_VARIABLE_PA_VDD)
#error PMIC PA DVS API not available
#endif

#ifdef FEATURE_RF_PA_DVS
/* 
* Support for controlling and setting the PA DVS voltage
* from NV.
*/
#ifndef FEATURE_WLAN_MODIFIED_FFA
#define RF_HAS_PA_DVS_NV
#endif
#endif

#endif /* FEATURE_PMIC_MANAGED_LDO */

/* Support for DFM related registers. DFM may not be enabled, but
** associate registers need to be configured for 1x operation.
*/
//To do 7600
#ifndef T_MSM7600
#define RF_HAS_RXF_DF2
#endif

#define RF_HAS_TX_AGC_RATE_ADJ_READY_BIT

#if defined (T_MSM6000)
#error code not present
#endif  /* T_MSM6000 */

#if defined (T_MSM6050)
#define RF_HAS_PDM_CLOCK_SRC_SEL
#define RF_HAS_SYSTEM_MODE_REG   /* SYSTEM_MODE reg controls DFM, CDMA, GPS modes */
#define RF_HAS_DIFF_PA_TX_WARMUP /* different values for the TX and PA warmup times */
#undef RF_DSP_HAS_RX_SPECTRAL_INV_CONFIG /* MDSP does not have rx spectral inversion config */ 
#endif /* T_MSM6050 */

#if defined (T_MSM6100)
#define RF_HAS_DACC_REG_ENHANCE   /* Supports new DACC fields for sbi, separate gain for I/Q */
#define RF_HAS_DIGITAL_VGA_DELAY  /* Supports RXF digital_vga_delay field */
#define RF_HAS_SURF_AUX_SBI       /* Supports AUX SBI on surf */
#define RF_DSP_HAS_RX_SPECTRAL_INV_CONFIG  /* MDSP has rx spectral inversion config */
#endif /* T_MSM6100 */

#if defined (T_MSM6100B)
#define RF_HAS_BYPASS_MIS_COMP     /* Supports new RXF bypass misc comp bit */ 
#undef  RF_HAS_DIGITAL_VGA_DELAY   /* Removed in MSM6100B */
#endif

#if defined (T_MSM6300)
#error code not present
#endif /* T_MSM6300 */

#if defined (T_MSM6500)
#undef RF_DSP_HAS_RX_SPECTRAL_INV_CONFIG  /* MDSP has rx spectral inversion config */
#define RF_HAS_GPS_CLK_CHECK              /* To check RXF clk before reading GPS AGC */
#undef RF_HAS_CUSTOMER_DEFINED_SYNTH      /* Configure gpio pins for external synth */

#if !defined (T_FFA)
#if (!defined(FEATURE_TRF6285) && !defined(FEATURE_TRF6500))
#define RF_HAS_SYNTH_LOCK
#endif
#endif /* T_FFA */

#if defined (FEATURE_OLTA_BACKOFFS_IN_NV)
#error code not present
#endif
#define RF_HDR_USE_ZERO_TX_DB_OFFSET
#endif /* T_MSM6500 */

#if defined (T_MSM6550)
#undef RF_HAS_RXF_DF2
#define RF_HAS_SUPPRESS_BAND_REVERTING
#define RF_HAS_RUNTIME_GPIO_CONFIG
#endif

#if defined (T_MSM6700) || defined(T_MSM6800)
#define RF_BYPASS_SYSTEM_CTL
#define RF_HAS_MODULATOR_TX_CTL
#define RF_HAS_MODEM_PA_CTL
#define RF_HAS_MODEM_GENERIC_CTL
#define RF_BYPASS_HDR_OVERDRIVE_DELAY
#define RF_USE_TRAMP_PIC_ISR
#define RF_HAS_HDET_COMP_FOR_RC_AND_TEMP   /* enable Tx peak detector temp comp tables.*/
#define RF_HAS_WU_TIMED      /* Enable self RF WU timing */
#ifdef FEATURE_HDR_REVA_L1
#error code not present
#endif 
#define RF_HAS_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST
#define RF_HAS_HDR_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST
#endif 

#if defined (T_MSM6800B)
#undef RF_HAS_TX_AGC_RATE_ADJ_READY_BIT
//BC15 support should be turned on in Dora 6800 builds once the C2k support
// is introduced.
#define RF_HAS_SUPPORT_FOR_BAND_CLASS_15
#define RF_HAS_BYPASS_BAND_CHANNEL_CHECK   /* always tune */
#endif

#if defined (T_MSM7500)
#error code not present
#endif /* T_MSM7500 */

#if defined (T_RUMI)
#error code not present
#endif /* T_RUMI */


/* -----------------------------------------------------------------------
**                               SURF
** ----------------------------------------------------------------------- */

/* Hardware based on RF Card */

/* Defaults for RF card, set to ZRF6500 */
#define RF_HAS_IM2
#define RF_HAS_SINGLE_BAND_PCS
#define RF_HAS_MONOTONIC_LNA_GAIN_LINEUP 
  

/* Temporary for now until PLT is defined a supported RF card */
#ifdef T_IO_CARD
#endif

#define RF_HAS_FILTER_COARSE_TUNE_IN_SW

/* -----------------------------------------------------------------------
**            SOFTWARE FEATURES DEFINED AT THE TARGET-LEVEL
** ----------------------------------------------------------------------- */

/* Target uses the Multimode (mDSP and ARM shared) SBI driver
*/
#ifdef FEATURE_MULTIMODE_SBI
#error code not present
#endif

/* Target uses the multimode API architecture
*/
#ifdef FEATURE_MULTIMODE_ARCH
  #define RF_HAS_MULTIMODE_ARCH
#endif

/* Target specifically uses the RF related API functions.
** Allows for combination configurations for soft transition with the
** RF_HAS_MULTIMODE_ARCH flag.
*/
#ifdef FEATURE_MULTIMODE_RFAPI
  #define RF_HAS_MULTIMODE_RFAPI
#endif

/* Target uses multiple RX chains
*/
#ifdef FEATURE_MULTI_RX_CHAIN
  #define RF_HAS_MULTI_RX_CHAIN
#endif

/* Target has the nv driver that splits data structures and processing into band
   specific modules incapsulating each specific bands. 
*/
#define RF_HAS_NV_BANDS

/* The target has HDR capability if FEATURE_HDR is defined
*/
#ifdef FEATURE_HDR
#error code not present
#endif

#ifdef FEATURE_HDR_AGC_OPTIMIZATION_P1
#error code not present
#endif

/* Implement TX AGC in FW (instead of HW) */
#ifdef FEATURE_MDSP_SUPPORTS_TX_AGC
  #define RF_HAS_MDSP_TX_AGC
  #ifdef T_UNDP
    #define RF_HAS_BETA_SCALING_TX_AGC
//    #define RF_HAS_PDM_LIN_TBL_SIZ_48
  #endif
#endif

/* Hardware Support */
#define RF_HAS_HW_DZRF
#define RF_HAS_HW_JTDZRF
#define RF_HAS_HW_DZRFBC14
#define RF_HAS_HW_D450
#define RF_HAS_HW_WDZRF
#define RF_HAS_HW_DZRF6285
#define RF_HAS_HW_TRF6500
#define RF_HAS_HW_TRF6285
#define RF_HAS_HW_QRF7600

/* Enabled Bands in the RF Subsystem
*/
#ifdef T_QSC1100
#define RF_HAS_BC0_NV_TABLE    /* US CELL        */
#define RF_HAS_BC1_NV_TABLE    /* US PCS         */
#else
#define RF_HAS_BC0_NV_TABLE    /* US CELL        */
#define RF_HAS_BC1_NV_TABLE    /* US PCS         */
#define RF_HAS_BC3_NV_TABLE    /* JCDMA          */
#define RF_HAS_BC4_NV_TABLE    /* Korean PCS     */
#define RF_HAS_BC5_NV_TABLE    /* NMT            */
#define RF_HAS_BC6_NV_TABLE    /* IMT            */
#define RF_HAS_BC11_NV_TABLE   /* BC11           */
#define RF_HAS_BC14_NV_TABLE   /* US PCS + 5MHz. */
#define RF_HAS_BC15_NV_TABLE   /* AWS band       */
#define RF_HAS_GPS_NV_TABLE    /* GPS            */
#endif /* T_QSC1100 */

#undef RF_USE_EXT_VCO_FOR_CDMA

/* Default RF Band Assignment
*/
#if defined( FEATURE_CDMA_800 )
  #define RF_DEFAULT_BAND RF_BC0_BAND /* US CELL */
#elif defined( FEATURE_CDMA_1900 )
  #define RF_DEFAULT_BAND RF_BC1_BAND /* US PCS  */
#elif defined FEATURE_CORE_SERVICE_ONLY
 /* To get rid of compile errors when cdma is not included in build */
#else
 #error rf_curr_cdma_band must be defined
#endif


#define RF_HAS_FTM_V2_CAL_NV

#ifdef FEATURE_INTELLICEIVER
  #define RF_HAS_POWER_MODE_LIB
#endif /* FEATURE_INTELLICEIVER */

#ifdef RF_HAS_POWER_MODE_LIB
  #ifndef RF_HAS_LINEAR_INTERPOLATION
    #error RF Interpolation must be enabled for RF Power Mode support
  #endif /* !RF_HAS_LINEAR_INTERPOLATION */

  #ifndef FEATURE_MSM_LIBRARY
    #error MSM Library must be enabled for RF Power Mode support
  #endif /* !FEATURE_MSM_LIBRARY */
  #if defined (T_MSM7500)
#error code not present
  #elif defined (T_MSM6700) || defined(T_MSM6800)
    #ifdef T_FFA
#error code not present
    #else /* T_FFA */
      #define INTELLICEIVER_GPIO GPIO_INPUT_37
    #endif /* T_FFA */
  #else
    #ifdef T_FFA
#error code not present
    #else /* T_FFA */
      #define INTELLICEIVER_GPIO GPIO_IN_41
    #endif /* T_FFA */
  #endif 
#endif /* RF_HAS_POWER_MODE_LIB */

#ifdef FEATURE_SRCH_RF_HAS_NO_RTC_DEP
  #define RF_HAS_NO_RTC_DEPENDENCY
#endif 

#ifdef  FEATURE_RF_WU_OPTIMIZATION
#error code not present
#endif  /* FEATURE_RF_WU_OPTIMIZATION */

#ifdef FEATURE_FTM_HWTC
#error code not present
#endif

#ifdef FEATURE_SSBI
  #define RF_HAS_SSBI
#endif /* FEATURE_SSBI */

/* SSBI buses can be mapped differently for different targets */
#ifdef RF_HAS_SSBI
#if defined (T_QSC60X5) && !defined(T_QSC1100)
    #define RFR_SSBI_BUS 0
    #define RFT_SSBI_BUS 2
#elif defined (T_QSC1100)
    #define RFR_SSBI_BUS 0
    #define RFT_SSBI_BUS 0x12
#elif defined (T_MSM7600)
#error code not present
#elif defined (T_MSM7500)
#error code not present
#elif defined (T_MSM6800B)/* RF_HAS_SSBI */
    #define RFR_SSBI_BUS 0
    #define RFT_SSBI_BUS 0x11
    /* end T_MSM6800 */
#else
    #error must define RFR & RFT SSBI Ports
  #endif /* T_MSM6800 */
#endif /* RF_HAS_SSBI */

/* This feature eliminates the RF_STATE_GPS and treats GPS as a regular band.
   These changes facilitate the new GPS interactions with RF */ 
#define RF_HAS_BYPASS_RF_STATE_GPS_CHECK

#if defined (T_QSC60X5) && !defined(T_QSC1100)
/* Remove unsupported RF HW */
#undef RF_HAS_HW_DZRF
#undef RF_HAS_HW_JTDZRF
#undef RF_HAS_HW_DZRFBC14
#undef RF_HAS_HW_D450
#undef RF_HAS_HW_WDZRF
#undef RF_HAS_HW_DZRF6285
#undef RF_HAS_HW_TRF6500
#undef RF_HAS_HW_TRF6285
#undef RF_HAS_HW_QRF7600
#undef RF_HAS_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST
#undef RF_HAS_HDR_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST

/* Support RF HW and default configuration */
#define RF_HAS_HW_QSC60X5
#define RF_DEF_HW RF_HW_QSC60X5_DUAL_BAND

/* Supported/Tested Band Classes */
#define RF_HAS_BC0_NV_TABLE    /* US CELL        */
#define RF_HAS_BC1_NV_TABLE    /* US PCS         */
#define RF_HAS_BC15_NV_TABLE   /* AWS band       */
#define RF_HAS_SUPPORT_FOR_BAND_CLASS_15

/* Other band classes for reference code usage */
#define RF_HAS_BC14_NV_TABLE   /* US PCS + 5MHz. */

/* Uunsupported BC */
#undef RF_HAS_BC3_NV_TABLE
#undef RF_HAS_BC4_NV_TABLE
#undef RF_HAS_BC5_NV_TABLE
#undef RF_HAS_BC6_NV_TABLE
#undef RF_HAS_BC11_NV_TABLE

#ifdef FEATURE_FTM_HWTC
#error code not present
#endif /* FEATURE_FTM_HWTC */

/* QSC60X5 specific features */
#define RF_HAS_DVGA_1OTH_OF_DB_RESOLUTION   
#define RF_HAS_RXF_V2
#define RF_HAS_VREG_VOLTAGE_MANAGEMENT
#define RF_HAS_NO_EXT_SYNTH
#define RF_HAS_DEDICATED_TX_SBI
#define RF_DISABLE_MDSP_TX_UNUSED_REGS
#define RF_HAS_BC15_PA_NON_BYPASS
#define RF_HAS_SPECTRAL_INV_BASED_ON_DEVICE
#define RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO
#define RF_HAS_HDET_COMP_FOR_RC_AND_TEMP        /* enable Tx peak detector temp comp tables.*/
#define RF_HAS_BYPASS_BAND_CHANNEL_CHECK        /* always tune */
#define RF_HAS_1X_NOTCH_FILTERS_ENABLED  
#define RF_HAS_HDR_PA_SWITCHPOINTS_DIFF_FROM_1X
#define RF_HAS_PA_AND_TX_WARMUP_TIMES_DIFF_FROM_LEGACY
#define RF_HAS_RX_PLL_LOCK_WAIT_TIME_600US
#define RF_HAS_INTERNAL_HDET_SUPPORT
#define RF_HAS_TX_TEMP_FREQ_COMP_NV_BASED_ON_PWR
#define RF_HAS_DEFAULT_PA_OFFSETS_SET_TO_ZERO
#define RF_HAS_LOAD_IM_CTLS                     /* IM enable and load uses same API */
#define RF_HAS_RFCONFIG_RX_LNA                  /* Enable LNA configuration tables */
#define RF_HAS_BC_SPECIFIC_TX_PA_MAP_VALUE      /* Band class specific PA_R_MAP settings */

#undef RF_HAS_ENC_TX_ENABLE_CALL_AS_LEGACY

#ifdef FEATURE_SC2X_RAMSIS_POWER_COLLAPSE       
#define RF_HAS_RFA_POWER_COLLAPSE               /* Support RFA power collapse for TCXO shutdown */
#endif

#ifdef FEATURE_PA_ACCESS_PROBE_CONFIG
#define RF_HAS_PA_ACCESS_PROBE_CONFIG           /* PA configuration for Access Probes */
#endif

/* Enable to use nv configurable bins for thermistor */
#undef RF_HAS_NV_SUPPORT_FOR_8_THERM_BINS

#ifndef T_RUMI_SC2X
#define RF_DEDICATED_RF_CARD RF_HW_QSC60X5_DUAL_BAND
#else
#error code not present
#endif /* T_RUMI_SC2X */

#ifdef T_QSC60X5REV1_0
#undef RF_HAS_BETA_SCALING_TX_AGC
#else
#define RF_HAS_BETA_SCALING_TX_AGC
#endif /* T_QSC60X5REV1_0 */

#ifdef FEATURE_HDR
#error code not present
#endif
#define RF_HAS_1X_PA_SWITCH_POINTS_DIFF_FROM_NV
#ifdef FEATURE_HDR
#error code not present
#endif
#define MAX_NUM_MODEM_SSBI_BUSES  3

#undef RF_HAS_SYNTH_LOCK

/* Specific to LINK FFA */
#ifdef FEATURE_LINK_FFA
#define RF_HAS_BC0_PA_NON_BYPASS
#define RF_HAS_BC1_PA_NON_BYPASS
#endif /* FEATURE_LINK_FFA */

#endif /* T_QSC60X5 */

#if defined (T_QSC1100)

/* Remove unsupported RF HW */
#undef RF_HAS_HW_DZRF
#undef RF_HAS_HW_JTDZRF
#undef RF_HAS_HW_DZRFBC14
#undef RF_HAS_HW_D450
#undef RF_HAS_HW_WDZRF
#undef RF_HAS_HW_DZRF6285
#undef RF_HAS_HW_TRF6500
#undef RF_HAS_HW_TRF6285
#undef RF_HAS_HW_QRF7600
#undef RF_HAS_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST
#undef RF_HAS_HDR_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST
#undef RF_HAS_SUPPORT_FOR_BAND_CLASS_15

/* Support RF HW and default configuration */
#define RF_HAS_HW_QSC11X0
#define RF_DEF_HW RF_HW_QSC11X0_TRI_BAND

/* Supported/Tested Band Classes */
#define RF_HAS_COMMON_NV_TABLE /* COMMON NV      */
#define RF_HAS_BC0_NV_TABLE    /* US CELL        */
#define RF_HAS_BC1_NV_TABLE    /* US PCS         */
#define RF_HAS_BC5_NV_TABLE

/* Uunsupported BC */
#undef RF_HAS_BC3_NV_TABLE
#undef RF_HAS_BC10_NV_TABLE
#undef RF_HAS_BC4_NV_TABLE
#undef RF_HAS_BC11_NV_TABLE
#undef RF_HAS_BC14_NV_TABLE
#undef RF_HAS_BC15_NV_TABLE
#undef RF_HAS_BC6_NV_TABLE

#ifdef FEATURE_FTM_HWTC
#error code not present
#endif /* FEATURE_FTM_HWTC */

/* QSC11X0 specific features */
#define RF_HAS_DVGA_1OTH_OF_DB_RESOLUTION   
#define RF_HAS_RXF_V2
#define RF_HAS_VREG_VOLTAGE_MANAGEMENT
#define RF_HAS_NO_EXT_SYNTH
#define RF_HAS_DEDICATED_TX_SBI
#define RF_DISABLE_MDSP_TX_UNUSED_REGS
//#define RF_HAS_SPECTRAL_INV_BASED_ON_DEVICE
#define RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO
#define RF_HAS_HDET_COMP_FOR_RC_AND_TEMP        /* enable Tx peak detector temp comp tables.*/
#define RF_HAS_BYPASS_BAND_CHANNEL_CHECK        /* always tune */
#define RF_HAS_1X_NOTCH_FILTERS_ENABLED  
#define RF_HAS_PA_AND_TX_WARMUP_TIMES_DIFF_FROM_LEGACY
#define RF_HAS_RX_PLL_LOCK_WAIT_TIME_600US
#define RF_HAS_INTERNAL_HDET_SUPPORT
#define RF_HAS_TX_TEMP_FREQ_COMP_NV_BASED_ON_PWR
#define RF_HAS_DEFAULT_PA_OFFSETS_SET_TO_ZERO
#define RF_HAS_LOAD_IM_CTLS                     /* IM enable and load uses same API */
//#define RF_HAS_RFCONFIG_RX_LNA                  /* Enable LNA configuration tables */
#define RF_HAS_BC_SPECIFIC_TX_PA_MAP_VALUE      /* Band class specific PA_R_MAP settings */

#undef RF_HAS_ENC_TX_ENABLE_CALL_AS_LEGACY

/*#ifdef FEATURE_SC2X_RAMSIS_POWER_COLLAPSE       
#define RF_HAS_RFA_POWER_COLLAPSE*/               /* Support RFA power collapse for TCXO shutdown */
//#endif



/* Enable to use nv configurable bins for thermistor */
#undef RF_HAS_NV_SUPPORT_FOR_8_THERM_BINS

/*#ifndef T_RUMI_SC2X
#define RF_DEDICATED_RF_CARD RF_HW_QSC60X5_DUAL_BAND
#else
#define RF_DEDICATED_RF_CARD RF_HW_DZRF_SSBI
#endif*/ /* T_RUMI_SC2X */

#define RF_HAS_BETA_SCALING_TX_AGC

#define RF_DEDICATED_RF_CARD RF_HW_QSC11X0_TRI_BAND

#define MAX_NUM_MODEM_SSBI_BUSES  3

#undef RF_HAS_SYNTH_LOCK

/* Specific to LINK FFA */
/*#ifdef FEATURE_LINK_FFA
#define RF_HAS_BC0_PA_NON_BYPASS
#define RF_HAS_BC1_PA_NON_BYPASS
#endif *//* FEATURE_LINK_FFA */

#undef RF_HAS_SUPPORT_FOR_DC_OFFSET_CAL

#undef RF_HAS_FEATURE_GPS
#undef RF_HAS_HDR
#undef RF_HAS_RFCONFIG_RX_LNA

#define RF_HAS_SUPPORT_FOR_QSC11X0_BRINGUP
#define RF_HAS_VREG_VOLTAGE_MANAGEMENT
#define RF_SUPPORTS_32_BIT_TX_AGC_INTERFACE
#define RF_SUPPORTS_NON_BYPASS_PA_FOR_BC5
#ifdef CUST_EDITION
//#define RF_SUPPORTS_NON_BYPASS_PA_FOR_BC0
//#define RF_SUPPORTS_NON_BYPASS_PA_FOR_BC1
#endif

#if defined(T_QSC1100_RICO_VOLTRON)
#define RF_HAS_SUPPORT_FOR_RICO_BRINGUP
#undef RF_HAS_SUPPORT_FOR_QSC11X0_BRINGUP
#undef RF_HAS_VREG_VOLTAGE_MANAGEMENT
#undef RF_HAS_BETA_SCALING_TX_AGC
#undef RF_SUPPORTS_BYPASS_PA_FOR_BC0
#undef RF_SUPPORTS_32_BIT_TX_AGC_INTERFACE
#endif

#if defined(T_QSC1100_RICO_VOLTRON) || defined (RF_HAS_SUPPORT_FOR_QSC11X0_BRINGUP)
#undef FEATURE_PMIC_MANAGED_LDO
#undef RF_HAS_RFA_POWER_COLLAPSE 
#undef RF_HAS_HDET_COMP_FOR_RC_AND_TEMP
#undef RF_HAS_SUPPORT_FOR_DC_OFFSET_CAL
#undef RF_HAS_SUPPORT_FOR_RC_PROCESS_ERROR_CAL
#undef RF_HAS_SUPPORT_FOR_LO_PWC_CAL
#define RF_HAS_HDET_BYPASS
#endif

#undef FEATURE_PMIC_MANAGED_LDO

#ifdef FEATURE_SLEEP_MODEM_DISABLED
#define RF_HAS_TCXO_SHUTDOWN_DISABLED
#else
#undef RF_HAS_TCXO_SHUTDOWN_DISABLED
#endif

#if !defined(T_QSC1100_RICO_VOLTRON)
#undef RF_HAS_HDET_BYPASS
#define RF_HAS_HDET_UINT16
#define RF_HAS_SUPPORT_FOR_RC_PROCESS_ERROR_CAL
#undef RF_HAS_SUPPORT_FOR_LO_PWC_CAL
#define  RF_HAS_TX_DAC_BACKOFF_DIFF_FROM_LEGACY

#define RF_HAS_RXADC_VOLTAGE_SWITCH

#define RF_HAS_SUB_BLOCK_SUPPORT

#ifdef RF_HAS_SUB_BLOCK_SUPPORT
#undef RF_HAS_BCX_BLOCK_0_NV
#undef RF_HAS_BCX_BLOCK_1_NV
#endif
#define RF_HAS_HDET_COMP_FOR_RC_AND_TEMP

#if defined(QSC11X0_PG2_POWER_GRID)
#undef RF_HAS_VARIABLE_SMPS_LEVEL
#undef RF_HAS_VARIABLE_SMPS_SWITCHER_SIZE
#else
#define RF_HAS_VARIABLE_SMPS_LEVEL
#define RF_HAS_VARIABLE_SMPS_SWITCHER_SIZE
#endif /* QSC11X0_PG2_POWER_GRID */


#ifdef FEATURE_PA_ACCESS_PROBE_CONFIG
#define RF_HAS_PA_ACCESS_PROBE_CONFIG
#define RF_HAS_1X_PA_SWITCH_POINTS_DIFF_FROM_NV  
#endif

#define RF_HAS_INTERNAL_THERM_SUPPORT
#define RF_HAS_NV_SUPPORT_FOR_8_THERM_BINS
#define RF_HAS_SUPPORT_FOR_HIGH_RES_HKADC

#if defined(QSC11X0_PG2_POWER_GRID)
#undef RF_HAS_TEMP_BASED_DEVICE_CONFIG
#else
#define RF_HAS_TEMP_BASED_DEVICE_CONFIG
#endif

#endif
#endif /* T_QSC1100 */


#ifdef FEATURE_FULL_POWER_COLLAPSE
#error code not present
#endif 

#ifdef T_FFA
#error code not present
#endif /* T_FFA */

#ifdef T_MSM7600
#error code not present
#endif /* T_MSM7600 */

#ifdef T_MSM7500
#error code not present
#endif /* T_MSM7500 */

#ifdef T_MSM6800
  #undef RF_HAS_HW_WDZRF
  #undef RF_HAS_HW_DZRF6285
#endif /* T_MSM6800 */


/* Target has support for RFConfig database for LNA switch point configuration mgmt. */
//#define RF_HAS_RFCONFIG_RX_LNA

#if defined (RF_HAS_BETA_SCALING_TX_AGC) && defined(RF_HAS_PDM_VS_POWER_TX_CAL)
#error Both these features cannot be ON simutaneously
#endif

#if defined (T_MSM7500I) || defined (T_MSM7500J)
  #define RF_HAS_HW_WDZRF
  #define RF_HAS_HW_DZRF6285
  #define RF_HAS_INTERNAL_HDET_SUPPORT
  #define RF_HAS_VREG_VOLTAGE_MANAGEMENT
  #ifdef T_UNDP
    #define RF_HAS_HDET_COMP_FOR_RC_AND_TEMP
  #else
        #undef RF_HAS_HDET_COMP_FOR_RC_AND_TEMP
  #endif 
#endif 

#define FEATURE_MSM_ADIE_D2D_PAD3_OPTIMIZATION

#if defined (T_QSC60X5)
#undef FEATURE_MSM_ADIE_D2D_PAD3_OPTIMIZATION
#endif

#if defined (T_QSC1100)
#undef FEATURE_MSM_ADIE_D2D_PAD3_OPTIMIZATION
#endif

#ifdef FEATURE_MSM_ADIE_D2D_PAD3_OPTIMIZATION
    #define RF_HAS_MSM_ADIE_D2D_PAD3_OPTIMIZATION
    #define RF_HAS_PAD3_CURRENT_REDUCTION_CONTROL
#endif 

#ifdef FEATURE_ENC_CONTROL_TX_ON_EN
#define RF_TX_ON_EN_MANAGED_BY_ENC
#endif
#ifdef T_MSM7500
#error code not present
#endif

#ifdef T_MSM7600
#error code not present
#endif // T_MSM7600

#ifdef T_UNDP
  #define RF_HAS_RF_ASIC_THERM
  /* Enable to use nv configurable bins for thermistor */
  #define RF_HAS_NV_SUPPORT_FOR_8_THERM_BINS
  #define RF_HAS_MSMA_CURRENT_REDUCTION_CONTROL
#endif

/* Feature checks */

#if defined(RF_HAS_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST) && !defined(FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST)
#error "RF_HAS_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST can only be defined if FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST is defined"
#endif

#if defined(RF_HAS_HDR_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST) && !defined(FEATURE_HDR_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST)
#error code not present
#endif

#endif /* RFICAP_MM_CDMA1X_H */

