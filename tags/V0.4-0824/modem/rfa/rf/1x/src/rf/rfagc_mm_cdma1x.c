 /*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                        RF AUTOMATIC GAIN CONTROL SERVICES

GENERAL DESCRIPTION
   This module contains the CDMA AGC control functions

EXTERNALIZED FUNCTIONS

rf_get_raw_cdma_rx_agc
  This function returns the CDMA Rx AGC setting in 8 bit two's complement
  format.

rf_get_raw_cdma_tx_agc
  This function returns the raw CDMA Tx AGC setting in 10 bit binary format.

rf_is_lna1_on
  This function returns the current LNA 1 on/off state

rf_islna2_on
  This function returns the current LNA 2 on/off state

rf_get_lna_state()
  Returns the current LNA state


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 1998,1999,2000 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2001,2002,2003 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2004,2005,2006 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2007           by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*=================================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rfagc_mm_cdma1x.c#14 $
  $DateTime: 2008/11/25 09:26:53 $
  $Author: lesp $

when       who     what, where, why
--------   ---     ---------------------------------------------------------------
11/24/08   lp      Fixed more Klocwork errors
11/17/08   lp      Fixed Klocwork errors
                   Fixed compiler warnings
                   Fixed Lint errors
10/08/08   sr      Lint error fixes
10/07/08   sr      Klocwork fixes
11/07/08   vm      Fixed putting the PA Range Pins to Low Logic in sleep
10/12/08   vm      Added support for different Access Probe Switchpoints
10/05/08   vm      Added more support for Band Class Sub Block implementation
09/30/08   vm/vb   Added Support for Band Class Sub Block Implementation
09/16/08   vm      Fixed the Featurization
09/14/08   vm      Added the fix for LNA Oscillation issue by progamming the
                   Bypass and Non Bypass Timer for G2/G3 Gain State
09/14/08   vm      Added the support for Tx DAC Backoff
08/06/08   vm      Changed the delay for PA switching from Low to High gain
                   state on QSC1100
06/20/08   vm      Added Fix for not passing the different PDMs to MDSP
                   for same Tx Power
06/18/08   vm      Added fix for LNA Oscillation by Loading IM Level 4
06/13/08   vm      Added suport for NBP timer and PDET for QSC1100
04/16/08   vm      Added support for programming LNA BPHT for QSC1100
03/28/08   vm      Added the support for Tx AGC interface with MDSP for QSC1100
12/06/07   ems     Support for QSC1100
12/04/07   vm      Support for HDR/1X ACESS Probe and PA Switchpoint backoff
12/02/07   ans     Added check for GPS band when programming LNA switching ctls.
12/02/07   ans     Added band as parameter to PDM_VS_PWR calibration update.
11/25/07   ans     Added PDM limiter based on the calibration data.
                   Moved RF_GET_TX_LIN_MIN and RF_GET_TX_LIN_MAX as they should be
                   outside library.
11/09/07   ans     Modified the blip delay settings for RTR6500 to 13usec.
11/09/07   vm      Added the support for Programmable Overdrive delay
11/05/07   ans     Moved RF_GET_TX_LIN_MIN and RF_GET_TX_LIN_MAX to RFT file.
11/07/07   ans     Added HDET compensation based on CW offset waveform.
11/07/07   ans     Updated TX compensation functions for PDM_VS_PWR Calibration.
                   Moved RF_GET_TX_LIN_MIN and RF_GET_TX_LIN_MAX to RFT file.
11/07/07   adk     Fixed compiler warning.
11/05/07   adk     Added function rf_hdr_mdsp_init_tx_agc_settings().
11/05/07   ans     Added functions to limit Tx linearizer PDM values based on chipset.
11/04/07   ans     Added calls to calibration functions for PDM_VS_PWR Calibration.
10/27/07   ans     Update LNA settings based on the device active.
10/27/07   ans     Changed FEATURE_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST name to 
                   RF_HAS_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST.
                   Changed FEATURE_HDR_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST name to 
                   RF_HAS_HDR_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST.
10/26/07   ans     Added FEATURE_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST for backoffs
                   for R2 and R3 PA states.
10/09/07   sar     Featurized out clock state checking in rf_get_path_1x_agc for 
                   MSM7500A+TRF6500.
10/02/07   dyc     Compilation fix for non-RF_HAS_RF_CONFIG_RX_LNA.
                   Wrapped OLTA message with RF_HAS_OLTA_DEBUG
09/28/07   ans     Changed blip delay settings for RTR6500.
                   Changed bypass timer settings for RTR6500 when RF_HAS_RFCONFIG_RX_LNA
                   feature is not defined.
09/27/07   ans     Change SIGMA_DELTA_MAX_LEVEL  for RTR6500.
09/24/07   dyc     Added rf_update_lna_offsets() for use of RF_HAS_LOAD_IM_CTLS
                   Compiler warning fix.
09/09/07   dyc     Update BPHT timers settings for SC2x and use of RF_HAS_LOAD_IM_CTLS
09/09/07   vm/dyc  Support RF_HAS_SIGMA_DELTA_MAX_LEVEL_170
08/30/07   ans     Changed MSG_FATAL to MSG_HIGH for debugging messages.
08/28/07   vm      Set up hooks in Code to have HDR PA switchpoints different from 1X 
                   for Sc2x
08/24/07   vm      Use the Value programmed in Card file to program PA
                   Hysterisis timer for 1X rather than Through Macro
                   Eliminate the use of feature RF_HAS_PA_BYPASS 
08/13/07   vm      Update the code so that PA switchpoint change for Access 
                   Probe only happen for 1X and not HDR on Sc2x
08/13/07   aak     Remove 3dB offset power difference in Tx for BC5
08/10/07   vm      Added the support for programming different PA switchpoints
                   and PA hysterisis timer during Access Probes different from 
                   traffic
08/05/03   vm      Added Interface API for programming of PA Hysterisis
                   timer by Encoder different in Traffic state from Access
                   State
08/02/07   dyc     Updated hdr_cal_items_ptr type assignment.
08/01/07   dyc     Support hdr specific im NV items for QSC60x5 and RTR6500
07/26/07   vm      Added the support for Tx Power Compensation NV based on
                   temperature and Frequency
07/22/07   dyc     Replaced usage of nv_common_items_ptr->rfnv_pa_r_map with
                   rf_current_pa_r_map to support band specific pa_r_map values
07/20/07   avi     Mainlined clockl regime management (previously featurized for
                   not MSM7600 targets)
07/15/07   dyc     Compiler warning fix.
07/12/07   vm      Changed the Tx and PA Warmup timers for Sc2x different 
                   from Legacy
07/03/07   adk     Updated function rf_cdma_mdsp_init_tx_agc_settings() so that
                   FW TX AGC can be initialized in the normal or override mode.
07/01/07   dyc     Added RF_HAS_HDRMPC_WAIT_DEBUG for compilation warning fix
06/26/07   vm      Added the support for Beta scaling for HDR
06/22/07   avi     Added macros to control BetaP control time update
06/20/07   Vish    Changed code to make use of the new RFCONFIG Rx LNA data
                   structures for primary and secondary chains.
06/19/07   jfc     Wrap rf_cdma_mdsp_set_beta_scaling_pdm_pwr_val() with #ifdef 
                    RF_HAS_BETA_SCALING_TX_AGC to fix compiler error.
05/18/07   kss     Added rf_hdr_set_tx_chip_delay().
06/12/07   avi     Added support for SSBI action time constants for BetaP
06/11/07   vm      Put back the support for Dynamic PA switchpoint for
                   Beta Scaling on SC2X
06/06/07   dyc     Compiler warning fix.
06/05/07   vm      Backed out the change for Dynamic PA switchpoint change 
                   for Sc2X
06/04/07   vm      Dynamic PA Switchpoint change for Beta Scaling on SCX
06/01/07   vm      Added the support for Tx Beta Scaling for SC2X
05/16/07   dyc     Re-position Tx interpolation check to avoid div by 0
05/14/07   adk     Added function rf_get_tx_min_max_pwr(). Added offsets to the
                   lin values sent to the MDSP when Beta Scaling is enabled.
05/11/07   vm      Hack for the function rf_set_tx_comp_regs() until the Hidden
                   registers overflow issue is fixed
05/09/07   vm      Fixed the function for computing Master Lin table 
                   for PDM vs Power Tx Cal
05/07/07   vm      Use Least Squares Algoritm to Extrapolate the uncalibarated
                   points to minimize the error for SC2X/RTR6500
05/03/07   adk     Comment out function-call CDMA_MDSP_DISABLE_TX_BETAP().
05/03/07   adk     Enable FW TX AGC without Beta Scaling.
05/02/07   adk     Fixed compiler errors.
05/01/07   adk     When Beta Scaling is enabled, send complete linearizer tables
                   to the MDSP, when temp or freq compensation is needed.
04/30/07   kss     Replaced HDR MDSP writes with the correct HDR macros.
04/30/07   vm      Added support for New Tx Cal for RF chips with 
                   Tx Compensation registers in RFT
04/27/07   dyc     Compilation warning fix.
04/25/07   adk     Corrected function rf_cdma_mdsp_init_tx_agc_settings().
04/23/07   adk     Updated function rf_cdma_mdsp_init_tx_agc_settings().
04/07/07   adk     Updated functions rf_cdma_mdsp_init_tx_agc_settings(),
                   rf_cdma_mdsp_update_tx_lin_tables(), and
                   rf_cdma_mdsp_update_tx_comp() for Beta Scaling.
04/06/07   bmg     Fixed HDR PA delay timer and time hysteresis implementation
03/28/07   adk     Updated NV read interface for Beta Scaling.
03/23/07   bmg     Added PA timer hysteresis band config items
03/16/07   adk     Fixed function rf_cdma_mdsp_init_tx_agc_settings().
03/12/07   adk     Minor fixes in FW TX AGC.
03/08/07   adk     Added support for FW TX AGC with Beta Scaling.
03/03/07   dyc     Added support for QSC60x5
02/27/07   go      Initialized PWR_CTL_EN 
02/08/07   vm/adk  Added support for firmware TX AGC and PA bypass.
01/25/07   jfc     Include high level rficap.h instead of rficap_mm_cdma1x.h
01/17/07   jfc     Added extra parens becuase of compiler error
01/09/07   adk     Updated RXFRONT clk regimes.
01/05/07   adk     Merged changes from 6800 Rel 6: Updated function rf_hdr_start_agc_acq().
                   Added functions rf_hdr_capture_agc() and rf_hdr_init_agc().
                   Updated RXFRONT clk regimes.
12/07/06   trc     Modifications for MSM7600 register name change
10/31/06   ycl     Include the correct rficap file.
10/25/06   kss     Add MPC command complete check in rf_hdr_init_agc().
10/23/06   kss     Remove enable delay due to mdsp support issue.
10/17/06   ycl     Modifications for initial MSM7600 build. 
10/13/06   kss     Use rxAgcEnableDelay allow 1 hs of good samples before agc.
09/20/06   kss     Initialize HDR AGC in RxF during warmup.
09/11/06   kss     Added rf_hdr_init_agc() to initial HDR AGC before wakeup.
08/15/06   kss     Added support for RESUME_AGC_ACQ_MODE in HDR.
08/09/06   kss     Added profiling strobes for HDR.
08/08/06   hkk     Fixed a compile warning
08/01/06   mt      Updated rf_hdr_start_agc_acq() function. 
07/17/06   hkk     Fix for linear interpolation in HDR TX comp
06/14/06   sq      Set HDR AGC mode
04/26/06   ra/kss  Fix HDR Tx rounding in rf_get_cdma_tx_agc_in_dBm().
                   Merge from //depot/asic/msm6800/drivers/rf/1x_mp_4.X/
03/30/06   zg      Revised RFR6185 BP timer again for better performance..
03/30/06   zg      Set RFR6185 specific BP timer values.
03/06/06   hkk     Added RF_HAS_TX_AGC_RATE_ADJ_READY_BIT check
03/03/06   hkk     Updates for having RF on SSBI for MSM6800
02/22/06   kss     Fix HDR Tx spike issue by changing pdm config and adjusting 
                   PA range delays.
01/30/06   ra      Added feature support for HDR to Enable/Enable TX_AGC
01/18/06   sar     Replaced reference to RF_STATE_GPS with DB_RF_GPS in 
                   rf_load_cagc_linearizers()
01/04/06   et      LINT fixes- unsupressed 734 and fixed it all over. some 737
01/04/06   et      LINT fixes- unsupressed 641 and fixed it all over.
12/15/05   et      LINT fixes
11/28/05   et      undo some lint fixes to maintain stability and added some 
                   comments
11/26/05   et      LINT fixes
11/09/05   kss     Use macros for gain logging.
10/25/05   vm      Modifies the function rf_chk_synth_lock()
10/24/05   sar     Fixed Lint warnings.
10/19/05   sar     Eliminated unused functions.
10/18/05   rmd     Updated code according to code review recommendations.
10/18/05   kss     Resolve MSM6500 compilation issue.
10/14/05   et      fixed LINT errors
10/12/05   kss     Fixed incorrect register access in rf_hdr_log_tx_agc().
10/05/05   bmg/hkk Set RFR6500 non-bypass hold timers to 0ms
10/04/05   kss     Added rf_hdr_log_tx_agc() to support new HDR log packets.
09/19/05   rmd     Added support for band class 14 (US PCS + 5MHz).
09/15/05   ra      Check modem clock,for HDR mode, in rf_get_path_lna_gain_state() before reading register
09/13/05   bmg     1ms bypass hold timer for RFR6500, BC0, HDR, G0 only.
09/02/05   rmd     Updated the code to support changing the type of
                   NV_RF_BC_CONFIG_I NV item from 32 bit 64 bit unsigned
                   number. 
09/01/05   dbc     Load LNA bypass timer 0 for RFR6175
08/13/05   ra      Check state of rf in rf_get_path_lna_gain_state()
08/04/05   jac     Added ifdef RF_HAS_POWER_MODE_LIB around the latest power mode 
                   reference addition.
07/20/05   bmg     Restrict use of hdr_g0_im* values to normal power mode only.
07/07/05   sar     Mainlined feature RF_HAS_ENHANCED_BAND_CLASS.
07/05/05   hkk/    Added support for RFR6525
           bmg     
06/17/05   kss/dyc Implement min/max values for Tx linearizer update for HDR.
06/06/05   dyc     Implemented support for hdr_g0_im* values.
05/30/05   ra      Added RF_HDR_USE_ZERO_TX_DB_OFFSET
05/16/05   dyc     Added sleep check for rf_log... functions.
05/17/05   hkk     Updated rfnv_hdr_olta_backoff and rfnv_olta_backoff to fix Birdie compile error
05/12/05   bmg     Fixed rf_get_lna_gain_state()
05/09/05   ra      For DORA: removed DORA obsolete variables initAgcPdm,initdcPdmI,initdcPdmQ
                           : added RF_HAS_MODULATOR_TX_CTL, RF_BYPASS_HDR_OVERDRIVE_DELAY and 
                             RF_BYPASS_SYSTEM_CTL
04/28/05   kss     Place PA range select in HDR mode only when in HDR Tx.
04/26/05   sar     Replaced call to obsolete function rf_get_band_mask with
                   rfm_get_band_mask().
04/22/05   jb/dyc  PA range mapping macro added to convert 1X PA map formats to HDR 
04/14/05   bmg     More special LNA bypass timer settings for RFR6500
04/11/05   bmg     Special LNA bypass/non-bypass timer settings for RFR6500
04/07/05   zg      Updated rf_get_path_1x_agc so that it returns min value based
                   on rf_is_sleep().
04/05/05   dyc     Added scope parentheses in rf_init_tx_cagc_delayed()
03/16/05   zg      Replaced the hardcoded GPS LNA state with a new function of
                   rf_determine_lna_high_gain_state().
03/10/05   ra      Added support for FEATURE_DUAL_CLK_RGM
02/23/05   jb      Changed LNA&DVGA offsets to non-reference scheme for binning.
02/14/05   kss     Added initial support for HDR dynamic PA switchpoints.
01/27/05   dyc     Fixed typo in if statements for non GPS.
01/25/05   dyc     Added path determination for clk_regime checks for rxf.
01/24/05   dyc     Added intlock protection for clk_regime checks.
                   Enable clk_regime checks for non-GPS builds.
01/17/05   dyc     Replace previous_lna_state with last_lna_state.  Modify 
                   last_rx_agc as array.  Add rf_get_path_1x_agc().
01/05/05   ra      DORA merge
12/14/04   dbc     Added ENC_SCH_FULL_RATE case to rf_notify_sch_data_rate() 
12/08/04   bmg     Removed un-executed calls to rf_load_agc_limits()
                   -- legacy function that was never used in MSM6500 and above.
11/08/04   kss     Clear DACC accumulators before enabling HDR AGC.
10/20/04   bmg     Fixed incorrect RX_AGCc_LNA_RANGE_DELAY setting for RFR6135
10/13/04   bmg     Added power mode support
10/08/04   abh     Removed obsolted functions related to GPS AGC control from FW.
09/30/04   whd     Added MSM6700 featurazation for new TX_AGC_ADJ_CTL register.
09/30/04   jac     Added HAS_GPS featurization to function rf_get_path_lna_gain_
                   to fix link error and make it consistant with the rest of the
09/30/04   aks     Merge in Hidra PLT RF changes.
09/28/04   ans     Added special setting for RFR6135 for RX_AGCc_LNA_BP_TIMER_n, 
                   RX_AGCc_LNA_RANGE_DELAY
09/21/04   zg      Fixed typo in rf_get_path_cdma_rx_agc().
09/13/04   dyc     rf_get_cdma_rx_agc() and rf_get_cagc_val() returns min RSSI 
                   when Rx chain is parked.
09/06/04   dbc     Set rf_exp_agc_table_ptr in rf_hdet_adj_calc()
09/03/04   dbc     Load expected TX AGC versus HDET table for each band.  The table
                   is now part of the NV digital items structure 
08/30/04   dyc     Support run-time rfr checks for RFR6125 bypass timer settings and
                   RFR6000 HDR IM NV updates.
08/16/04   dyc     Removed RF_HAD_FM component.
07/26/04    et     removed temporary code to disable interpolation for HDR
                   because interpolation is now working for HDR.
07/09/04   dyc     Added RF_HAS_HDR around HDR statements.
06/22/04   dyc     Added HDR support in rf_get_pa_state()
06/11/04    et     added !hdr_enabled condition to interpolation, because
                   interpolation is not yet supported for HDR.
06/10/04   sar     The first parameter in call to rf_set_cdma_lna_decision() was set
                   to wrong variable. Changed it to device.
06/08/04   zg      Fixed compiling err (when disabling GPS) in the function
                   rf_get_path_lna_gain_state().
06/08/04   dyc     Add enabling of RXF (RxAGC block) clocks.
05/25/04   dyc     Use rfnv_pa_r_map item instead of hard coded values.
                   Add DACC initialization in rf_hdr_start_agc_acq()
05/21/04   sar     Added support for Enhance Bands Type rf_get_cdma_tx_agc_in_dBm().
05/21/04   dbc     Updates for RF PA Backoff functionality
05/10/04   dyc     Update to use HDR IM rise/fall nv values. Remove hard coded HDR
                   IM2 rise/fall and LNA_BP_TIMER values.
04/22/04   AP/dyc  Use CLKRGM_H instead of target specific clkrgm.
04/09/04   dyc     Update rf_set_pa_offsets() remove hard coded addr
04/08/04   dyc     Updates to support removal of DFM VU dependency.
03/28/04   dyc     Support dbug app for HDR and 2nd chain agc.
                   Added rf_get_path_cdma_rx_agc_in_dBm()
                   Added rf_get_path_cagc_val()
03/19/04   et      added interpolation (frequency comp) support
03/09/04   dyc     Initialize TX_PDM_DELAY_VAL register.
03/04/04   dyc     Check clk regime and store old states in rf_get_path_lna_gain_state() 
02/26/04   kss     Corrected initial Tx power limit setting for HDR.
02/24/04   dyc     Set PA Range pins low in rf_disable_pa_ctl() for current savings.
02/24/04   kss     Enable new FLL tracking gains.
02/02/04   kss     Moved rf_hdr_set_tx_pwr_limit to rftxplim.c; added
                   rf_hdr_agc_update_max_power() as helper function for above.
01/30/04   zg      Fixed feature "RF_HAS_GPS"
01/27/04   sar     Added constant definitions for CAGC_SUBPCG_PA_WARMUP_DELAY, 
                   CAGC_DEFAULT_TX_WARMUP_DELAY, Bypass and Non-Bypass timers.
                   Added code to support chain specific LNA rise/fall thresholds 
                   and IM Levels. 
01/30/04    et     Removed usage of RF_HAS_TRIMODE...replaced by appropriate 
                   use of FEATURE_TRIMODE_ITEMS and RF_HAS_PCS and RF_HAS_CDMA
01/23/04   dyc     Need to restore backoff check qualifier for HDR hard coded values.
01/23/04   dyc     Remove backoff check qualifier for HDR hard coded values.
01/21/04   dyc     Restore HDR olta hard coded LNA2 rise/fall and LNA_BP_TIMER1 settings.
01/20/04   kss     Restore original FLL gains (again).
01/07/04   kss     Initialize HDR Rx AGC to lowest gain (highest power) value;
                   override AGC value when HDR AGC is disabled; restore 
                   FLL gains.
12/11/03   abh     Before accessing AGC block, cross check if the AGC Clock
                   source is on or not.
12/11/03   kss     Correct PA state mapping for HDR.
12/08/03   kss     Adjust BTF offset for HDR.
12/08/03   dyc     Remove specific HDR olta hard coded lna rise/fall settings.
12/03/03   dyc     Implement special case for HDR rfr6000 disabling of olta 1st sw pt
                   hard coded lna rise/fall settings.
11/11/03   dyc     Support for use of C1 default OLTA backoff values.
11/06/03   dbc     Fixed header of rf_get_path_cdma_rx_agc_full_scale()
11/05/03   ra      Removed check for FEATURE_GPSONE in rf_get_cdma_rx_agc_in_dBm,
                   rf_get_lna_gain_state and rf_get_path_cdma_rx_agc
11/05/03   ra      Added rf_get_cdma_tx_agc_in_dBm,rf_get_pa_state, and rf_get_ratchet_state
                   to support UI debug screen. Also modified rf_get_cagc_val, rf_get_cdma_rx_agc_in_dBm
                   and rf_get_lna_gain_state
11/05/03   dbc     Mainlined RF_HAS_OPTIMIZED_LNA_SWITCH_POINTS.  Get backoffs 
                   from digital items structure.
10/22/03   kss     Enable acq gains for AGC in HDR; init AGC/LNA to low
                   gain state on start.
10/20/03   dyc     Remove FEATURE_HWTC
10/13/03   zg      Fixed LNA_RANGE_OVERRIDE init for builds with T_IO_CARD_PANDA.
10/10/03   kss     Restored old FLL gain values for HDR.
10/08/03   dbc     Fixed syntax error.
10/08/03   dbc     Added rf_get_path_cdma_rx_agc_full_scale().  Changed 
                   rf_get_cdma_rx_agc_in_dBm() to take a uint16 and return a int16 
10/07/03   et      featurized synth_lock checking and added device param
                   to rf_chk_synth_lock()
10/07/03   dbc     Changes for RF_HAS_OPTIMIZED_LNA_SWITCH_POINTS.  Backoff is in
                   same units as LNA RISE/FALL.  Use a backoff of -128 to disable 
                   a switchpoint optimization. Use 511 (RF_RX_SIGNED_AGC_VAL_MAX_PWR) 
                   as AGC warmup value.                   
10/01/03   kss     Set HDR BTF based on 1x value in NV.  Enable HDR AGC 
                   acq/tracking gains.
10/01/03   dyc     Cleanup. Changed param variable name to be consistent with type.
09/30/03   dbc     Changed rf_get_cdma_rx_agc_in_dBm() to take unint8 as input
09/29/03   dbc     Updated rf_get_cdma_rx_agc_in_dBm() to calculate Rx power given
                   an AGC value.
09/19/03   dyc     Compiler warning fix.
09/18/03   dbc     Changed backoff values for cell mode in the optimized LNA switch 
                   points algorithm
09/15/03   dyc     Remove rflibi.h dependency.
09/12/03   sar     Added prototype for rf_get_path_lna_gain_state(). Added device 
                   parameter to rf_set_pa_offsets and rf_notify_sch_data_rate.
09/11/03   dbc     Added algorithm to compute optimized LNA switch points to 
                   rf_update_lna_offsets().  Code is featurized by 
                   RF_HAS_OPTIMIZED_LNA_SWITCH_POINTS
09/08/03   kss     Integrate HDR RF AGC interface and functionality.
09/05/03   sar     Added support for path parameter in rf_get_path_cdma_rx_agc().
09/04/03   et      replaced all the indirect uses of rfcom_to_path[device] with
                   local variable path
09/03/03   dbc     Use global RF variable rf_chain_status[] to access RF state
                   related variables
09/02/03   sar     Added rf_get_path_cdma_rx_agc() function that returns the rx agc 
                   value for the device parameter. Changed rf_get_cdma_rx_agc() into
                   a wrapper function that calls rf_get_path_cdma_rx_agc() with 
                   the primary transceiver as the input for backward compatibilty.
08/28/03   dyc     Updated rf_load_lna_switching_ctls() and rf_update_lna_offsets() 
                   for correct nv chain1 indexing.
08/27/03   dyc     Move Rx_AGCc_SBI_CTL usage to rflib.
08/26/03   sar     Added support for 2nd chain nv items.
08/25/03   zg      Fixed a bug in rf_restore_gps_agc().
08/12/03   aar/zg  Added check in rf_get_cdma_rx_agc to determine whether the clock
                   is ON or not. If it is on, then read directly otherwise return
                   the value from the last AGC reading.
08/11/03   dbc     Only init TX AGC for RF_TRANSCEIVER_0 in rf_init_cagc()
08/10/03   dyc     Added CAGC_RF_LNA_RANGE_DELAY_85CHIPX2 value.
08/06/03   sar     Updated functions to accept rfcom_device_enum_type.
07/26/03   dyc     updated macro parameters for chain 1 support
07/24/03   et      added additional chain 1 support
07/14/03   zg      Fix LNA override by using LNA_DECISION_OVERRIDE.
                   Choose LNA_STATE_1 for GPS on 6500.
07/11/03   sar     Added support for multipath chipset drivers.
07/07/03   bmg     Previous HWIO compile error now fixed
06/26/03   dyc     set TX_AGC_TEST_POINT_SEL to 0
                   removal of msm3000_mode, PA high gain state workaround
06/26/03   dyc     maintain PA in high gain state for support of msm3000_mode setting.
06/16/03   dyc     load AGC_MIN_VALUE along with setting LNA OFFSET values
06/13/03   dyc     added used of CAGC_RF_PA_R_MAP_WB__00001000
                   updated some writes to use mask
                   use RF_LATCH_TX_GAIN_CTL_RD() macro for latching data
06/11/03   et      cleaned up most magic numbers
06/11/03   dyc     temp mv assignment of AGC_MIN values to RXF for cell/pcs mode switch
                   invert tx adj polarity
06/05/03   et      added CAGC_RF_ before every #define that we use.
06/04/03   et      added masks to writes
06/02/03   dyc     added rflibi.h to support RX_AGCc_SBI_CTL
05/16/03   bmg     Removed T_QUASAR and fixed rf_set_cdma_lna_decision
05/14/03   et      ported additional register
05/13/03   et      fixed warnings
05/13/03   et      ported to MSM6500 registers
05/12/03   bmg     Ported rf_set_cdma_lna_decision function to Jaguar
04/14/04   et      merged with MSM6100 3030 build
03/31/03   et      took out overwriting of lna in rf_set_cdma_lna_decision
                   hacked out synth_lock check
                   merged back in renaming of RX_AGC register
03/04/03   dbc     Changes for FEATURE_TX_BACKOFF merge from MSM51xx code base
01/30/03   dbc     Make 102dB range for CAGC linearizer the default.  The NV items
                   are only used if RF_HAS_NV_CAGC_LINEARIZER_RANGE_CONFIG is defined.
01/13/03   aks     Changes for GSM PLT.
12/02/02   dbc     Initialize ATTEN_LIMIT_LIMIT register
12/01/02   dbc     Added support for FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST:
                   rf_notify_sch_data_rate(), rf_cagc_is_pa_thres_adjusted() and
                   rf_cagc_get_curr_pa_thres_adj() APIs were added.
                   Set ATTEN_LIMIT_RATCHET_MODE to CPU control.  Set default
                   cdma dynamic range to 102 dB
11/26/02   dyc     Include ts.h for ZIF LOGGING feature.
11/13/02   zg      Moved LNA lowest gain setting to pre_tune_to_cdma.
11/11/02   dbc     Move call of rf_unfreeze_cagc() in rf_load_cagc_linearizers()
                   so that it called whenever rf_freeze_cagc() is called
10/29/02   dyc     Corrected use of HWIO mask.
10/24/02   dyc     Updated for additional PLT support.
10/17/02   zg      Added support for GPS Baseband Test Mode with feature
                   T_IO_CARD_GPS_PBS.
09/20/02   dbc     Changed max synth out of lock counts for CDMA
09/04/02   dbc     Remove legacy code. Cleanup.
08/29/02   dbc     Cleanup.
08/28/02    et     added function rf_get_tx_warmup
08/08/02   dyc     Merged with RF.01.00.17.
08/08/02   dbc     Added rf_get_min_tx_pwr_in_dB() API function.
08/07/02   et      added support for different TX and PA warmup times
08/03/02   zg      Automatically determines the lowest implemented gain state in
                   rf_restore_cdma_agc().
08/02/02   dbc     Fixed bug in rf_rescale_tx_pwr_limit() where min/max TX power limits
                   were being incorrectly calculated.  Changed setting of min/max
                   Rx/Tx power levels for 102 dB to be dynamically determined using
                   the cdma_min_rx_rssi NV item. Added rf_get_cdma_power_rails()
07/26/02   dyc     PLT build support:
                   Wrap code for T_IO_CARD_PANDA
07/26/02   dyc     Changed the disabled value for the LNA bypass/nonbypass timers
                   from 0x0 to 0x04 in rf_disable_im_level_ctls()
                   (merged from dbc MSM6000_RF.01.00.15)
07/17/02   dyc     Replace RF_HIGH_POWER_R0R1_THRESHOLD with rfnv_fm_pa_mac_high nv item.
                   Changed detection/setting of CAGC dynamic range to be
                   dynamically determined by using an NV item. Rail IM Levels when
                   disabling the IM state machine.
                   (merged from dbc rf.01.00.13)
07/16/02   dyc     Missing ; in rf_restore_cdma_agc() MSM_OUTM usage.
07/15/02   dyc     Set AGC_VAL_CTL, TX_OPEN_CTL, and FREEZE_PA_STATE in AGC_RDWR_CTL
                   to the non-override states at initialization
                   (merged from dbc MSM6000_RF vul)
06/05/02   jwh     Wrapped rf_load_lna_switching_ctls() for HWTC.
05/26/02   jwh     Merge FEATURE_HWTC changes.
05/22/02   dbc     Added functions/data for 102 dB CAGC dynamic range
05/20/02   dbc     Added rf_load_im_level_ctls(), rf_disable_im_level_ctls(),
                   rf_disable_im_level_ctls(), rf_set_agc_val_to_low_gain(),
                   rf_set_lna_decision_to_low_gain_state(), and
                   rf_determine_lna_low_gain_state() functions.
05/01/02   zg      Cleaned up rf_restore_gps_agc_state() & rf_save_gps_agc_state().
04/18/02   aks     Wrap code for non-phone_t targets.
04/01/02   sar     changed message priority to med from error in
                   rf_save_gps_agc_state() and rf_restore_gps_agc_state().
03/29/02   zg      Featurized limiting AGC Value Max after GPS visit.
03/22/02   dbc     Changed rf_set_rx_agc_mode() to use library API
03/21/02   abh     Disable AGC Value Max for the duration of GPS visit.
03/18/02   dbc     Added stubbing of rx linearization tables in
                   rf_load_cagc_linearizers() since this is no longer used.
03/17/02   dbc     Added rf_set_rx_agc_mode() API function for setting rx AGC mode
03/16/02   dbc     Added rf_init_tx_cagc_delayed() and rf_init_rx_cagc_delayed()
                   API functions for initializing some CAGC items after NV has
                   been read. Added loading of base uncompensated LNA offsets in
                   rf_load_lna_switching_ctls()
03/08/02   zg      Added rf_restore_gps_agc() & rf_restore_cdma_agc(). Freezed
                   TX open loop in rf_freeze_agc().
03/08/02   dbc     Broke out the loading of some LNA switching controls into
                   rf_load_lna_switching_ctls()
02/22/02   dbc     Moved declaration of rf_init_hw_lna_ctl() to rfi.h
02/13/02   dbc     Added ZIF logging support. Removed freezing of TX Open Loop
                   during linearizer updates in ZIF
02/08/02   dbc     Set bits in RF_MODE_SEL register for CDMA in
                   rf_hw_lna_setup_digital().
02/06/02   aks     Changed T_MSM_6000 to T_MSM6000.
01/31/02   zg      Merged in GPS related changes from PhoneT.
01/29/02   dbc     Removed freezing/unfreezing of LNA state and AGC Value for forward
                   LNA control (ZIF mode)
01/14/02   dbc     TX PDM polarity mainlined to be inverted in rf_init_tx_cagc()
01/09/02   dbc     Added rf_get_tx_open_loop() and rf_get_tx_gain_adjust() API
                   functions.
01/07/02   dbc     Fixed syntax error in rf_hw_lna_setup_digital().
01/07/02   dbc     Fixed syntax error in rf_hw_lna_setup_digital().
12/20/01   dbc     AGC_DC_GAIN register setting in rf_init_rx_cagc() changed.
12/18/01   rmd     Removed support to the PM1000 and added support to the PM60X0.
12/18/01   aks     Made Tx AGC PDM polarity normal, instead of inverted.
11/21/01   dbc     RF Driver decomposition changes. RX/TX CAGC changes for ZIF and
                   Superhet. rf_set_cdma_lna_decision(), rf_set_cdma_lna_range()
                   functions added.
11/15/01   aks     Re-org changes.
11/14/01   aks     Logical wrap of lna_state_mixer_only var changed.
11/02/01   dbc     Removed unused function rf_set_rx_gain_comp().  Added more
                   initializations for MSM6000+ registers.
10/25/01   dbc     Added support for RX_AGC_ADJ/MIX_RANGE mux (MSM6000)
10/19/01   dbc     Added new support for MSM6000, MSM6050 forward and backward LNA
                   control. rf_get_lna_gain_state() added.
                   RF_CAP_PDM1_FOR_TX, RF_CAP_PDM2_FOR_TX, RF_HAS_PA_RANGE_CAL
                   support removed. FEATURE_PA_RANGE_TEMP_FREQ_COMP mainlined.
10/05/01   aks     Merged in phone_t changes.
10/01/01   dbc     Made changes required for port to MSM6000+ targets.  CAGC_RAS_WR_WH
                   macro added.  rf_hw_lna_setup_digital(), rf_init_hw_ln_ctl(),
                   rf_disable_hw_lna_ctl(), and rf_freeze_lna_state() were changed to
                   API functions to abstract LNA gain (3 stage versus 5 stage) mode
09/19/01   dbc     Removed RF_HAS_RFR3100 dependencies - SW LNA Control removed for
                   CDMA and FM.
09/14/01    dc     Moved DFM related routines to dfmagc.c
09/12/01   dbc     Merged global variables initAgcPdm, initdcPdmI, initdcPdmQ used
                   by GPSONE
09/11/01   aks     Merged FEATURE_TX_POWER_BACK_OFF changes.  Wrapped gspone only
                   code under RF_HAS_GPS.
07/02/01   aks     Created from re-organization of rfmsm.c, rev 1.10.
                   O:/src/asw/COMMON/vcs/rfmsm.c_v   1.10   21 May 2001 16:40:16
===================================================================================*/


#include "target.h"
#include "customer.h"

#include "comdef.h"
#include "rficap.h"
#include "db.h"
#include "err.h"
#include "msm.h"
#include "msm_drv.h"
#include "rfi.h"
#include "rf.h"
#include "adc.h"
#include "rflib.h"
#include "enc.h"
#include "synth.h"
#include "qdspext.h"
#include "rf_prof.h"

#ifdef RF_HAS_RXF_V2
#include "rxf.h"
#endif

#ifdef RF_HAS_MDSP_TX_AGC
#include "rf_mdsp_tx_agc.h"
#endif

#ifdef FEATURE_ZIF_LOGGING
  #include "log.h"
  #include "ts.h"
#endif

#ifdef CAGC_GEN_FRAME_SYNC
#error code not present
#endif

#ifdef RF_HAS_HDR
#error code not present
#endif 

#include CLKRGM_H

#ifdef FEATURE_FACTORY_TESTMODE
#include "ftm.h"
#endif
 
#include "rf_hal.h"

#ifdef RF_HAS_LOAD_IM_CTLS
extern void rf_load_im_level_ctls( rfcom_device_enum_type device );
#endif /* RF_HAS_LOAD_IM_CTS */

#ifdef RF_HAS_RFCONFIG_RX_LNA
#include "rfconfig_rx_lna.h"
#endif /* RF_HAS_RFCONFIG_RX_LNA */

#if defined (RF_HAS_PDM_VS_POWER_TX_CAL) || \
    defined (RF_HAS_PA_AND_TX_WARMUP_TIMES_DIFF_FROM_LEGACY)
#include "rft.h"
#endif

#include "rf_device_functions.h"

#ifdef RF_HAS_PDM_VS_POWER_TX_CAL
#include "rfagc_tx_pdm_vs_pwr_cal.h"
#endif /* RF_HAS_PDM_VS_POWER_TX_CAL */

/* Configuration of RF chains */
extern rf_path_0_config_type rf_path_0;
extern rf_path_1_config_type rf_path_1;
extern byte rf_current_pa_r_map;

/*
 * SSBI action time update constants: values are to be set in the card file
*/
#ifdef RF_HAS_BETA_SCALING_TX_AGC
uint16 c2k_tx_agc_update_l2h;
uint16 c2k_betap_time_delta;
#ifdef RF_HAS_HDR
#error code not present
#endif
#endif

#ifdef FEATURE_MDSP_SUPPORTS_TX_AGC
/*
 * Set the following var to 1 to run FW-based TX AGC in the override mode.
 * Useful for testing FW-based TX AGC and Beta Scaling algorithms.
 */
uint16 set_mdsp_tx_agc_in_override_mode = 0;
#endif

#if defined (RF_HAS_MDSP_TX_AGC) || defined(RF_HAS_PDM_VS_POWER_TX_CAL)
extern boolean rf_cdma_load_master_lin_tables;
#endif

#ifdef RF_HAS_MDSP_TX_AGC

#ifdef RF_HAS_BETA_SCALING_TX_AGC
/*
 * The following macro ensures that (1) lin values are sent to
 * the MDSP in -1/640 dBm units and (2) 32767 (0x7FFF) represents
 * the lowest TX power permissible for a given band.
 */
#define RF_CDMA_MDSP_CONVERT_BETAP_TX_LIN_VAL( val )  (32767 - 64*(val))

#ifdef RF_SUPPORTS_32_BIT_TX_AGC_INTERFACE
#define RF_CDMA_MDSP_CONVERT_BETAP_TX_PDM_VAL( val )  (uint32)val
#else
#define RF_CDMA_MDSP_CONVERT_BETAP_TX_PDM_VAL( val )  (((val) - 256)<<7)
#endif

#define RF_CDMA_TX_BETA_SCALING_PWR_NOT_USED_VAL  -32768
#define RF_CDMA_TX_BETA_SCALING_PDM_NOT_USED_VAL  32767
void rf_cdma_mdsp_set_beta_scaling_pdm_pwr_val(byte lin_index,byte index,uint32 temp_pdm_val,int temp_pwr_val);
#ifdef RF_HAS_HDR
#error code not present
#endif
#endif

#define RF_CDMA_MDSP_CONVERT_TX_LIN_VAL( val )        (((val) << 7) ^ 0x8000)
#define RF_CDMA_MDSP_8_TO_16_BIT( val )               ((val) << 8)
#define RF_CDMA_MDSP_9_TO_16_BIT( val )               ((val) << 7)
#define RF_CDMA_MDSP_10_TO_16_BIT( val )              ((val) << 6)
#define RF_CDMA_CONVERT_PA_THRESHOLD_VAL( val )       (((255 - (val)) ^ 0x80) << 8)
#define RF_CDMA_MDSP_TX_DB_OFFSET                 0
#define RF_CDMA_MDSP_TX_OPEN_LOOP_OFFSET       1024
#define RF_CDMA_MDSP_TX_OPEN_LOOP_MASK         0x3ff
#define RF_CDMA_MDSP_TX_TOTAL_GAIN_OFFSET      1024 
#define RF_CDMA_MDSP_TX_TOTAL_GAIN_MASK        0x3ff
#define RF_CDMA_MDSP_TX_CLOSED_LOOP_OFFSET     512
#define RF_CDMA_MDSP_TX_CLOSED_LOOP_MASK       0x1ff
#define RF_CDMA_MDSP_COMBINER_FREQ_VAL         0x3
#endif /* RF_HAS_MDSP_TX_AGC */

/* Number of TX AGC linearizer values (when Beta Scaling is not enabled) */
#define RF_TX_AGC_NUM_LINEARIZER_VAL            33
//#define RF_HAS_SC2X_BETA_SCALING_DEBUG
#ifdef RF_HAS_PA_ACCESS_PROBE_CONFIG
extern boolean rf_pa_config_is_traffic_state;
#ifdef RF_HAS_HDR
#error code not present
#endif
#endif


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

                                COMMON MACROS

=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/


/*===========================================================================

MACRO RF_PACK_WORD

DESCRIPTION
  This macro returns a word item equal to ((hbyte) << 8) + (lbyte).

PARAMETERS
  hbyte the byte value to be placed in the high byte of the word result
  lbyte the byte value to be placed in the low byte of the word result

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define RF_PACK_WORD( hbyte, lbyte ) ((((word)(hbyte)) << 8) | ((word)(lbyte)))



/*===========================================================================

MACRO CAGC_RAS_WR_WH

DESCRIPTION
  This macro writes 16 bits to current address pointed to by the RAS_RAM
  address pointer

PARAMETERS
  hbyte: the byte value to be placed in the high byte at the current RAS RAM
         address pointer
  lbyte: the byte value to be placed in the low byte at the current RAS RAM
         address pointer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The RAS_RAM address pointer is automatically incremented to point to the next
  RAS_RAM location (next write will occur at this address)

===========================================================================*/
#define CAGC_RAS_WR_WH( hbyte, lbyte) \
HWIO_OUT( RAS_RAM_DATA, RF_PACK_WORD( hbyte, lbyte ) )



#ifdef FEATURE_IS2000_CAGC
/*===========================================================================

MACRO CAGC_INIT_TX_2_EARLY_PCG

DESCRIPTION
    This macro initializes the TX_2_early_PCG_EN signal to be 64 chips
    advanced from PCG boundary (or pcg_enable boundary is delayed by 64 chip)
    Note that MSM6000 does not have period control bits and is only a
    byte register

DEPENDENCIES    None

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#ifndef T_MSM6000
#define CAGC_INIT_TX_2_EARLY_PCG( ) \
  HWIO_OUTM( TX_2_EARLY_PCG_CTL, \
                                       HWIO_FMSK(TX_2_EARLY_PCG_CTL,EARLY_2_PCG_EN_ADV) \
                                     | HWIO_FMSK(TX_2_EARLY_PCG_CTL,EARLY_2_PCG_EN_PER), \
                                                                    EARLY_2_PCG_EN_ADV_64CHIPS \
                                                                  | EARLY_2_PCG_EN_PER_EVERY_PCG)
#else
#error code not present
#endif /* !T_MSM6000 */



/*===========================================================================

MACRO CAGC_INIT_VERY_EARLY_FRAME

DESCRIPTION     This macro initializes the TX very early frame control
                very_early_frame is advanced 100 chips from the frame boundary
                early_frame is generated by delaying very_early_frame 0 chips

DEPENDENCIES    None

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/ 

#define CAGC_INIT_TX_VERY_EARLY_FRAME( ) \
  HWIO_OUTM( TX_VERY_EARLY_FRAME_CTL, \
                                          HWIO_FMSK(TX_VERY_EARLY_FRAME_CTL,VERY_EARLY_FRAME_ADV) \
                                        | HWIO_FMSK(TX_VERY_EARLY_FRAME_CTL,VERY_EARLY_FRAME_PER), \ CAGC_TX_VERY_EARLY_FRAME_ADV_100CHIPS \
                                                                           | CAGC_VERY_EARLY_FRAME_20MS); \
  HWIO_OUT( VERY_EARLY_FRAME_DELAY_REG, CAGC_AGC_VERY_EARLY_FRAME_DELAY_0CHIPS)

#endif /* FEATURE_IS2000_CAGC */

#ifdef RF_HAS_MDSP_TX_AGC
/* To Provide the PA Map to MDSP */
#define RF_CONVERT_1X_HW_TO_MDSP_PA_MAPPING( map_val ) \
  ((( map_val & 0xC0) >> 6) | (( map_val & 0x30) >> 2) | (( map_val & 0x0C) << 2) | (( map_val & 0x03) << 6))
#endif

#ifdef RF_HAS_HDR_NV_PA_MAPPING
/*===========================================================================

MACRO RF_CONVERT_1X_TO_HDR_PA_MAPPING

DESCRIPTION     This macro concerts the PA mapper from a 1x format to HDR format.
                This is required so that both modes can use the nv item successfully.
                Maps P3:P2:P1:P0 to P0:P1:P2:P3
                
DEPENDENCIES    None

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#ifdef RF_HAS_HDR
#error code not present
#endif
#endif /* RF_HAS_HDR_NV_PA_MAPPING */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

                    SCALE FACTORS FOR FIXED POINT MATH

=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/* Scale factor for FM HDET error slope vs. power level
*/
#define RF_FM_ERR_SLP_SCALE            64

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

                    RxAGC accumulator value for HDR AGC optimization 

=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#define HDR_AGC_OPT_ACCUM_VALUE        0x5555

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

                 Defines for performing 85/102 dB Dynamic Range

             The values that represent power have a resolution of
                    1/100 dB for increased precision
=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#define MAX_RX_PWR_102_DB   -1260
#define MIN_RX_PWR_102_DB   -11500
#define MAX_TX_PWR_102_DB   4200
#define MIN_TX_PWR_102_DB   -6040
#define RX_AGC_VAL_CNTS_PER_DB_102_DB 10
#define TX_AGC_VAL_CNTS_PER_DB_102_DB 10

#define MIN_RX_PWR_85_DB    -10600
#define MAX_RX_PWR_85_DB    -2075
#define MAX_TX_PWR_85_DB    3300
#define MIN_TX_PWR_85_DB    -5225
#define RX_AGC_VAL_CNTS_PER_DB_85_DB 12
#define TX_AGC_VAL_CNTS_PER_DB_85_DB 12

#define RF_RX_AGC_MAX 1023
#define RF_RX_AGC_MIN 0
#define RF_RX_AGC_VAL_OFFSET 512
#define RF_TX_AGC_VAL_OFFSET 512
#define RX_AGC_VAL_MASK 0x3ff
#define TX_AGC_VAL_MASK 0x3ff
#define RF_TX_GAIN_ADJ_OFFSET 256

#define SIGMA_DELTA_MAX_LEVEL_SCALE_FACTOR 10
#define RF_RX_SIGNED_AGC_VAL_MAX_PWR 511
#define LNA_THRESHOLD_ADJUST_DISABLE -128
#define LNA_THRESHOLD_DISABLE -128
#define LNA_OFFSET_SHIFT 2
#define VGA_GAIN_OFFSET_SHIFT 2
#define RF_RX_AGC_MIN_RD_VALUE 0x200

#ifdef RF_HAS_DVGA_1OTH_OF_DB_RESOLUTION   
#define VGA_GAIN_OFFSET_SCALE_FACTOR 10
#else 
#error code not present
#endif

#define RF_THRESH_SATURATE( val, numbits) \
    MAX( MIN( (val) , ( ( 1 << ( (numbits) - 1 ) ) - 1 ) ), -( 1 << ( (numbits) - 1 ) ) )
    
#define NUM_BITS_TO_LIMIT_RESULT 8


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

                      RF COMPENSATION CONSTANTS

=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/* -----------------------------------------------------------------------
** CDMA synth lock checks are performed on every valid PCG when synths
** are out of lock during transmit
** ----------------------------------------------------------------------- */
#define RF_NO_SYNTH_LOCK_MAX_CDMA      254    /* 2.5 secs @ 1/8 rate
                                                300 msec @ full rate */

/* -----------------------------------------------------------------------
** FM synth lock checks are performed evry 100 msec
** ----------------------------------------------------------------------- */
#define RF_NO_SYNTH_LOCK_MAX_FM        30     /* 3 seconds */



/* -------------------------------------------------------------------------

                            SHARED RF DRIVER DATA

---------------------------------------------------------------------------- */


LOCAL byte    rf_pwr_index;        /* NV table index for current power level */
                                   /*   rf_fm_agc_set_vs_pwr[] */
                                   /*   rf_fm_exp_hdet_vs_pwr[] */
                                   /*   rf_fm_err_slp_vs_pwr[] */

/* last software setting of TX_AGC_ADJ software
** in FM mode, this is 0..511 while the hardware is +255..-256
** in CDMA mode, this is 0..1023 while the hardware is +511..-512
*/
int2    rf_tx_agc_adj;

#ifdef RF_HAS_HDET_UINT16
uint16  rf_hdet_data;
#else
byte    rf_hdet_data;        /* last raw reading of HDET in CDMA mode */
#endif

LOCAL clk_cb_type rf_ena_hwlna_cbk;   /* clock call-back for enabling hw LNA control */

/* Array (table) that maps the 3 bit LNA Range values to their corresponding LNA Decision
** values when 5 stage LNA is used.  Note that the polarity of the LNA_RANGE pins
** must be taken into account before addressing this table. If SINGLE_5_STAGE_LNA_SEL
** is active then LNA_RANGE_POLARITY is used to determine the polarity of the LNA RANGE
** MSB, otherwise MIXER_RANGE_POLARITY is use to determine the polarity of the LNA RANGE
** MSB
*/
LOCAL const byte lna_range_map[] = {0x0, 0x1, 0x3, 0x7, 0x0, 0x0, 0x0, 0xf};

/* Array (table) that maps the 3 bit LNA Range values to their corresponding GAIN STATE
** values (as an enum) when 5 stage LNA is used.  Note that the polarity of the LNA_RANGE pins
** must be taken into account before addressing this table. If SINGLE_5_STAGE_LNA_SEL
** is active then LNA_RANGE_POLARITY is used to determine the polarity of the LNA RANGE
** MSB, otherwise MIXER_RANGE_POLARITY is use to determine the polarity of the LNA RANGE
** MSB
*/
LOCAL const lna_gain_state_type lna_gain_state[] = { LNA_GAIN_STATE_0,
                                                     LNA_GAIN_STATE_1,
                                                     LNA_GAIN_STATE_2,
                                                     LNA_GAIN_STATE_3,
                                                     INVALID_LNA_GAIN_STATE,
                                                     INVALID_LNA_GAIN_STATE,
                                                     INVALID_LNA_GAIN_STATE,
                                                     LNA_GAIN_STATE_4 };


/* cdma CAGC dynamic range setting */
rf_cagc_dynamic_range_type  cagc_dynamic_range_setting = RF_HAS_102DB_CDMA_DYNAMIC_RANGE;


/* PA offsets */
#define CAGC_NUM_PA_OFFSETS 4
const int2 cagc_pa_offsets[CAGC_NUM_PA_OFFSETS]= {0,0,0,0};

#ifdef T_MSM6050
    #define CAGC_SUBPCG_PA_WARMUP_DELAY_V  50
#else
#error code not present
#endif

/* default TX warmup delay 
*/
#define CAGC_DEFAULT_TX_WARMUP_DELAY  650

/* LNA bypass timer (5 Stage LNA Control) - multiples of 26us
*/
#define  CAGC_BYPASS_TIMER_0_V              0x0
#define  CAGC_BYPASS_TIMER_0_RFR6125_V      0x10D
#define  CAGC_BYPASS_TIMER_0_RFR61X5_V      0x10D

#define  CAGC_BYPASS_TIMER_1_V              0x10D
#define  CAGC_BYPASS_TIMER_1_RFR6135_V      0x27

#define  CAGC_BYPASS_TIMER_2_V              0x27
#define  CAGC_BYPASS_TIMER_3_V              0x27

#define  CAGC_BYPASS_TIMER_0_RFR6500_V 0x10D
#define  CAGC_BYPASS_TIMER_0_HDR_RFR6500_V 38 /* 0.99ms */
#define  CAGC_BYPASS_TIMER_1_RFR6500_V 0x10D
#define  CAGC_BYPASS_TIMER_2_RFR6500_V 0x27
#define  CAGC_BYPASS_TIMER_3_RFR6500_V 0x27

#define  CAGC_BYPASS_TIMER_0_QSC60X5_V      0x106  /* 6.8 ms */
#define  CAGC_BYPASS_TIMER_1_QSC60X5_V      0x106 
#define  CAGC_BYPASS_TIMER_1_QSC11X0_V      0x106 
#define  CAGC_BYPASS_TIMER_2_QSC11X0_V      0x106
#define  CAGC_NON_BYPASS_TIMER_2_QSC11X0_V  0x28
#define  CAGC_BYPASS_TIMER_3_QSC11X0_V      0x106
#define  CAGC_NON_BYPASS_TIMER_3_QSC11X0_V  0x28

#define  CAGC_BYPASS_TIMER_0_RTR6500_V 0x106
#define  CAGC_BYPASS_TIMER_1_RTR6500_V 0x106

/* LNA non bypass timer (5 Stage LNA Control)
*/
#define  CAGC_NON_BYPASS_TIMER_0_V  0
#define  CAGC_NON_BYPASS_TIMER_1_V  0
#define  CAGC_NON_BYPASS_TIMER_2_V  0
#define  CAGC_NON_BYPASS_TIMER_3_V  0
#define  CAGC_NON_BYPASS_TIMER_RFR6500_V 0

#ifdef FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST
LOCAL boolean pa_thres_adjusted = FALSE;
LOCAL int1 curr_offset1 = 0;
LOCAL int1 curr_offset2 = 0;
LOCAL int1 curr_offset3 = 0;
#endif /* FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST */

#ifdef CAGC_GEN_FRAME_SYNC
#error code not present
#endif

#ifdef RF_HAS_HDR
#error code not present
#endif

/* -----------------------------------------------------------------------

                        CALIBRATION RELATED DATA

-------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------

                            Local Data

---------------------------------------------------------------------------- */

/* ----------------------------------------------------------------------
** Threshold settings for PA_R1 and PA_R0 level transitions in CDMA mode.
** These values are 0..255 representing -52.25dBm..+32.75dBm.
** ---------------------------------------------------------------------- */

#define RF_PA_RN_MIN    0          /* Minimum value for PA_R*_RISE/FALL   */
#define RF_PA_RN_MAX    255        /* Maximum value for PA_R*_RISE/FALL   */

#define RF_LNA_RANGE_MAX 0x7f      /* Maximum value for LNA Range RISE/FALL */
#define RF_LNA_RANGE_MIN 0x80      /* Minimum value for LNA Range RISE/FALL */

int2 rf_r1_rise_val;         /* Cal value for PA R1 Rise */
int2 rf_r1_fall_val;         /* Cal value for PA R1 Fall */

int2 rf_r2_rise_val;         /* Cal value for PA R2 Rise */
int2 rf_r2_fall_val;         /* Cal value for PA R2 Fall */

int2 rf_r3_rise_val;         /* Cal value for PA R3 Rise */
int2 rf_r3_fall_val;         /* Cal value for PA R3 Fall */

/* Are we loading the linearizers?
*/
LOCAL boolean rf_loading_linearizers = FALSE;

#ifdef FEATURE_TX_POWER_BACK_OFF
  //interval to refresh pa_backoff module; each count corresponds to 10 seconds 
  #define RF_PA_CALL_INTERVAL 30 
  uint16 rf_backoff_refresh_counter ;
  extern boolean tx_power_limit_update_pending;

  rf_txc_chan_config_type agc_backoff_chan_config =
  {
   REFRESH_BACKOFF,
   FALSE,
   CAI_RC_3,
#ifdef FEATURE_IS2000_P2
   FALSE,
   CAI_RC_3,
#endif /* FEATURE_IS2000_P2 */
#ifdef FEATURE_IS2000_R_SCH
   FALSE,
   CAI_RC_3,
   ENC_SCH_8TH_RATE,
   FALSE
#endif /* FEATURE_IS2000_R_SCH */

  };
  
#endif

/* pa warmup value using new NV item (chips units) */
LOCAL word pa_warmup_ms=0;

#ifdef RF_HAS_DIFF_PA_TX_WARMUP
LOCAL word tx_warmup_ms=0;
#endif

/* used for scaling 102 dB values to 85 dB and for calculating Rx/Tx power
   in dBm. Initialized to default values  */
LOCAL int2 rx_102_dB_agc_min_val = RF_RX_AGC_MIN;
LOCAL int2 rx_102_dB_agc_max_val = RF_RX_AGC_MAX;
LOCAL int2 tx_102_dB_agc_min_val = RF_TX_AGC_MIN;
LOCAL int2 tx_102_dB_agc_max_val = RF_TX_AGC_MAX;

LOCAL int2 max_rx_pwr_102_db = MAX_RX_PWR_102_DB;
LOCAL int2 min_rx_pwr_102_db = MIN_RX_PWR_102_DB;
LOCAL int2 max_tx_pwr_102_db = MAX_TX_PWR_102_DB;
LOCAL int2 min_tx_pwr_102_db = MIN_TX_PWR_102_DB;

LOCAL int2 tx_102_dB_pwr_limit_max_val  = RF_TX_AGC_MAX;
LOCAL int2 tx_102_dB_pwr_limit_min_val  = RF_TX_AGC_MIN;

/* Cellular CDMA Rx/Tx power rails (default to 102 dB dynamic range) */
rf_cagc_pwr_rails_type cdma_cellular_pwr_rails = { -115, -13, -60, 42 };

/* Stores the last CDMA RX AGC value read by rf_get_cdma_rx_agc function */
LOCAL int2 last_rx_agc[2]  = {RF_RX_AGC_MIN_RD_VALUE, RF_RX_AGC_MIN_RD_VALUE};
LOCAL int2 last_rx_cagc[2] = {RF_RX_AGC_VAL_OFFSET, RF_RX_AGC_VAL_OFFSET};
LOCAL uint16 last_rx_agc_full_scale[2] = {0,0};
LOCAL int2 last_lna_state[2] = {0,0};

#ifdef FEATURE_ZIF_LOGGING

/*------------------------------------------------------------------------*/
/* Logging Structures                                                     */
/*------------------------------------------------------------------------*/

struct
{
  log_ext_agc_type log;

  int  inx;
    /* Index into the AGC log buffer */

  int  drops;
    /* Count of how many times we've dropped log packets for lack of a
       buffer */
}
rf_ext_agc_vars;

/*------------------------------------------------------------------------*/

struct
{
  log_tx_agc_type log;

  int  inx;
    /* Index into the AGC log buffer */

  int  drops;
    /* Count of how many times we've dropped log packets for lack of a
       buffer */
}
rf_tx_agc_vars;

#endif /* FEATURE_ZIF_LOGGING */



/* -------------------------------------------------------------------------

                          GLOBAL DATA (yuk!) needed by GPS Modules

---------------------------------------------------------------------------- */

#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */



/* -------------------------------------------------------------------------

                          LOCAL FUNCTION PROTOTYPES

---------------------------------------------------------------------------- */
byte rf_pwr_index_calc(byte pwr_level);

void rf_set_102dB_agc_value_limits( void );

lna_gain_state_type rf_determine_lna_low_gain_state( rfcom_device_enum_type device );

lna_gain_state_type rf_determine_lna_high_gain_state( rfcom_device_enum_type device );

#ifdef RF_HAS_RFCONFIG_RX_LNA
const rfconfig_rx_lna_linearity_info_type* rf_get_current_rfconfig_rx_lna_linearity_info(
   rfcom_device_enum_type device);
#endif /* RF_HAS_RFCONFIG_RX_LNA */

void rf_get_tx_min_max_pwr( rf_card_band_type  rf_curr_cdma_band,
                            int *p_tx_min_pwr, int *p_tx_max_pwr );

#ifndef FEATURE_GSM_PLT

#ifdef RF_HAS_QSC60X5
#error code not present
#endif /* RF_HAS_QSC60X5 */

#ifdef RF_HAS_QSC11X0
/*===========================================================================

FUNCTION RF_FREEZE_RX_AGC_ACCUM

DESCRIPTION
  This function disables the rx_agc accumultion in 
  rx_agc accumulator. Thus freezes the AGC at fixed value
  
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/

void rf_freeze_rx_agc_accum(rf_path_enum_type path)
{
  MSM_W_MASKED_i(RX_AGCc_DC_GAIN, AGC_LOOP_FREEZE_EN,CAGC_RF_AGC_VALUE_OVERRIDE_EN, (int)path );
}

/*===========================================================================

FUNCTION RF_UNFREEZE_RX_AGC_ACCUM

DESCRIPTION
  This function enables the normal operation of
  Rx AGC Accumulator
  
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/

void rf_unfreeze_rx_agc_accum(rf_path_enum_type path)
{
  MSM_W_MASKED_i(RX_AGCc_DC_GAIN, AGC_LOOP_FREEZE_EN, CAGC_RF_AGC_VALUE_ACCUM_NORM_V, (int)path );
}
#endif /* RF_HAS_QSC11X0 */

/*===========================================================================

FUNCTION RF_INIT_CAGC                                       EXTERNAL FUNCTION

DESCRIPTION
  Performs one-time unitialization of all CAGC registers (both TX and RX)

DEPENDENCIES
  Any actions performed by this function or calls within this function must
  not be dependent upon non-default NV items, i.e., items read from NV

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rf_init_cagc( rfcom_device_enum_type device )
{

  if ( device == RFCOM_TRANSCEIVER_0 ) 
  {
    /* ----------------------------------------------------
    ** Initialize the TX CAGC
    ** ---------------------------------------------------- */
    rf_init_tx_cagc( device );
  }

  /* ----------------------------------------------------
  ** Initialize the RX CAGC
  ** ---------------------------------------------------- */
  rf_init_rx_cagc( device );

}

/*===========================================================================

FUNCTION RF_INIT_CAGC_LINEARIZER_RANGE                               EXTERNAL

DESCRIPTION
  This function sets the global variable that determines the linearizer range
  of the CAGC section (RX & TX) in CDMA.  It relies on NV items in the
  "common" structure.  The values used to determine the Rx/Tx power rails
  for 102 dB dynamic range are also determined in this function.


DEPENDENCIES
  Should not be called until after NV is read

RETURN VALUE
  boolean : linearizer range selection success/fail

SIDE EFFECTS
  None
===========================================================================*/
boolean rf_init_cagc_linearizer_range( void )
{
#ifdef RF_HAS_NV_CAGC_LINEARIZER_RANGE_CONFIG
  /* determine the CDMA dynamic range to use using NV Item */
  cagc_dynamic_range_setting =
    ( rf_nv_common_items_ptr->rfnv_cdma_dynamic_range > 853 ) ?
      RF_HAS_102DB_CDMA_DYNAMIC_RANGE : RF_HAS_85DB_CDMA_DYNAMIC_RANGE;

  /*******************************************************
  ** for 102 dB dynamic range determine the min/max RX/TX
  ** power using the rfnv_cdma_min_rx_rssi NV item
  *******************************************************/
  if ( cagc_dynamic_range_setting == RF_HAS_102DB_CDMA_DYNAMIC_RANGE )
  {
      /*********************************************************
      ** if NV item >= -1060 (1/10 dB resolution) we will use
      ** the default values for 102 dB dynamic range
      ** (in 1/100 db resolution):
      ** MIN TX POWER = -6040
      ** MAX TX POWER =  4200
      ** MIN RX POWER = -11500
      ** MAX RX POWER = -1260
      **********************************************************/
      if ( rf_nv_common_items_ptr->rfnv_cdma_min_rx_rssi < -1060 )
      {
        /* calculate min/max Rx/Tx power limits in 1/100 dB resolution */
        min_rx_pwr_102_db = rf_nv_common_items_ptr->rfnv_cdma_min_rx_rssi * 10;
        max_rx_pwr_102_db = min_rx_pwr_102_db + 10240;
        max_tx_pwr_102_db = -7300 - min_rx_pwr_102_db;
        min_tx_pwr_102_db = -7300 - max_rx_pwr_102_db;
      }

      /* determine the power rails for Rx/Tx based upon the min rx rssi
      ** rounded to the closest integer dBm
      */
      cdma_cellular_pwr_rails.cdma_rx_pwr_min =
          ( ( (-min_rx_pwr_102_db) % 100 ) <  50 ) ? min_rx_pwr_102_db/100 : min_rx_pwr_102_db/100 -1;

      cdma_cellular_pwr_rails.cdma_rx_pwr_max =  cdma_cellular_pwr_rails.cdma_rx_pwr_min + 102;
      cdma_cellular_pwr_rails.cdma_tx_pwr_max =  -73 -  cdma_cellular_pwr_rails.cdma_rx_pwr_min;
      cdma_cellular_pwr_rails.cdma_tx_pwr_min =  -73 -  cdma_cellular_pwr_rails.cdma_rx_pwr_max;

  }
  else    /* 85 dB dynamic range */
  {
      cdma_cellular_pwr_rails.cdma_rx_pwr_max =  -21;
      cdma_cellular_pwr_rails.cdma_rx_pwr_min =  -106;
      cdma_cellular_pwr_rails.cdma_tx_pwr_max =  33;
      cdma_cellular_pwr_rails.cdma_tx_pwr_min =  -52;

  }
#endif /* RF_HAS_NV_CAGC_LINEARIZER_RANGE_CONFIG */

  return TRUE;
}

/*===========================================================================

FUNCTION RF_GET_CAGC_LINEARIZER_RANGE                               EXTERNAL

DESCRIPTION
  This function gets the current setting of the linearizer range of the
  CAGC section (RX & TX)


DEPENDENCIES
  Should not be called until after NV is read and
  rf_init_cagc_linearizer_range() is called

RETURN VALUE
  rf_cagc_dynamic_range_type : CAGC linearizer range setting

SIDE EFFECTS
  None
===========================================================================*/
rf_cagc_dynamic_range_type rf_get_cagc_linearizer_range( void )
{
    return  cagc_dynamic_range_setting;
}

/*===========================================================================

FUNCTION RF_SET_CAGC_LINEARIZER_RANGE                               EXTERNAL

DESCRIPTION
  This function sets the linearizer range of the CAGC section (RX & TX) to
  either 85 dB or 102 dB.


DEPENDENCIES
  Should not be called until after NV is read and a call to
  rf_init_cagc_linearizer_range() has been made.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rf_set_cagc_linearizer_range( void )
{
#ifndef RF_HAS_MDSP_TX_AGC
  if ( cagc_dynamic_range_setting == RF_HAS_102DB_CDMA_DYNAMIC_RANGE )
  {
    MSM_W_MASKED(TX_AGC_CTL, LINEARIZER_RANGE_SEL, CAGC_RF_LIN_RANGE_ENA_V);
  }
  else 
  {
    MSM_W_MASKED(TX_AGC_CTL, LINEARIZER_RANGE_SEL, CAGC_RF_LIN_RANGE_DIS_V);
  }
#endif
}

#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */


#ifdef RF_HAS_MDSP_TX_AGC

/*===========================================================================

FUNCTION RF_CDMA_MDSP_INIT_TX_AGC_SETTINGS              EXTERNALIZED FUNCTION

DESCRIPTION
This function initializes the SW-FW interface parameters needed by TX AGC.
This function must be called every time MDSP FW is downloaded, since
the initialization parameters are reset after each FW download.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rf_cdma_mdsp_init_tx_agc_settings (void)
{

  rf_mdsp_tx_set_port_reg_address();

#ifdef RF_HAS_BETA_SCALING_TX_AGC
  CDMA_MDSP_ENABLE_TX_BETAP();
  /* SSBI action control timing to minimize the PA glitch */
  CDMA_MDSP_SET_TX_AGC_UPDATE_L2H( c2k_tx_agc_update_l2h  );
  /* Betap time control */
  CDMA_MDSP_SET_BETAP_TIME_DELTA( c2k_betap_time_delta ); 
#else
//  CDMA_MDSP_DISABLE_TX_BETAP();   /* Enables FW TX AGC without Betap */
#endif

#ifdef RF_HAS_HDR
#error code not present
#endif

   /*
    * Run FW TX AGC in normal mode or override mode
    * Normal mode is the default
    * Override mode is for testing
    */
  if (set_mdsp_tx_agc_in_override_mode == 1)
  {
    /* FW TX AGC in override mode */
    CDMA_MDSP_ENABLE_TX_OPEN_LOOP_OVERRIDE();
    CDMA_MDSP_ENABLE_TX_CLOSED_LOOP_OVERRIDE();

    CDMA_MDSP_ENABLE_TX_AGC_ADJ_OVERRIDE();
    #ifdef RF_HAS_BETA_SCALING_TX_AGC
    CDMA_MDSP_ENABLE_TX_BETAP_OVERRIDE();
    #endif
    #ifdef RF_HAS_HDR
#error code not present
    #endif
  }
  else  /* FW TX AGC in normal mode */
  {
  CDMA_MDSP_SET_TX_OPEN_LOOP_NORMAL();
  CDMA_MDSP_SET_TX_CLOSED_LOOP_NORMAL();

    CDMA_MDSP_SET_TX_AGC_ADJ_NORMAL();
    #ifdef RF_HAS_BETA_SCALING_TX_AGC
    CDMA_MDSP_SET_TX_BETAP_NORMAL();
    #endif
    #ifdef RF_HAS_HDR
#error code not present
    #endif
  }

  CDMA_MDSP_TX_CAGC_DOWN_RATCHET_MODE_NORMAL();
  CDMA_MDSP_ENABLE_PA_RANGE_MAP();
  CDMA_MDSP_SET_PA_RANGE_NORMAL();
    CDMA_MDSP_SET_PA_STATE_NORMAL();

  MSM_W_MASKED(MOD_TX_AGC_ADJ_CTL, TX_AGC_ADJ_SEL, 2); //Select PDM Source to be Firmware

  CDMA_MDSP_WR_TX_GAIN_ADJ_STEP_SIZE( CAGC_RF_TX_STEP_FULL_DB_V );
  rf_load_pa_thres();
  CDMA_MDSP_NEW_TX_LIMIT_SLOT_PARAMS_AVAILABLE();


  /* From "rf_init_tx_cagc()" */
  CDMA_MDSP_SET_PA_STATE_NORMAL();
  CDMA_MDSP_SET_TX_MASK_DATA_DELAY( CAGC_RF_MASK_DELAY_2PCG_V + 1 );
  CDMA_MDSP_SET_COMBINER_FREQUENCY(0x3 /* RF_CDMA_MDSP_COMBINER_FREQ_VAL */);
//  CDMA_MDSP_SET_TX_OPEN_LOOP_NORMAL();


  /* From "rf_init_tx_cagc_delayed()" */
  CDMA_MDSP_TX_CAGC_DOWN_RATCHET_MODE_NORMAL();
  CDMA_MDSP_SET_TX_MIN_PWR((int2)(0x1ff <<6) /* tx_min_pwr */);
  CDMA_MDSP_NEW_TX_LIMIT_SLOT_PARAMS_AVAILABLE();

  /* From "rf_init_pa_ctl_1_txlinz()" */
  CDMA_MDSP_DISABLE_PA_RANGE_MAP();

/* From "rf_init_pa_ctl_4_txlinz()" */

//  CDMA_MDSP_SET_PA_L2H_GAIN_STATE_DLY( CAGC_RF_TX_BLIP_DELAY_13US_V );
  CDMA_MDSP_SET_RF_PN_ROLL_DELAY( 0 );
//  #ifdef RF_HAS_MDSP_TX_AGC_SSBI_INTERFACE
//  /* Required for SSBI Interface but not for PDM interface */
//  CDMA_MDSP_SET_PA_H2L_GAIN_STATE_DLY( CAGC_RF_TX_BLIP_DELAY_13US_V );
//  #endif

  /* PA Mapping and Temporal Hyst */
  CDMA_MDSP_ENABLE_PA_RANGE_MAP();

  /* Use the mapping provided in NV, there 4 PA states supported */
  /* 0xE4 (becomes 0x1B), 0x58 (becomes 0x25), 0xA8 (becomes 0x2A), 0xAA (becomes 0xAA) */
  CDMA_MDSP_SET_PA_RANGE_MAP(RF_CONVERT_1X_HW_TO_MDSP_PA_MAPPING
           ((rf_current_pa_r_map & 0xFF)));  /* Default 0xA8 */

//  #ifdef RF_HAS_PA_BYPASS
//  CDMA_MDSP_SET_PA_HYSTERISIS_TIMER_VAL(RF_CDMA_MDSP_PA_HYSTERISIS_TIMER_VAL);
//  CDMA_MDSP_ENABLE_PA_HYSTERISIS_TIMER();
//  #else
//  CDMA_MDSP_DISABLE_PA_HYSTERISIS_TIMER();
//  #endif

/* From "rf_enable_pa_ctl()" */
   CDMA_MDSP_SET_PA_RANGE_NORMAL();

//   CDMA_MDSP_SET_TX_AGC_ADJ_NORMAL();

} /* rf_cdma_mdsp_init_tx_agc_settings() */

/*===========================================================================

FUNCTION RF_CDMA_MDSP_UPDATE_TX_LIN_TABLES              EXTERNALIZED FUNCTION

DESCRIPTION
This function updates the TX linearizer tables for four PA gain states
for MDSP

DEPENDENCIES
  None

RETURN VALUE
None

SIDE EFFECTS
  None

===========================================================================*/
void rf_cdma_mdsp_update_tx_lin_tables(void)
{
  byte   index;
  uint16 umts_based_tx_card;
#ifdef RF_HAS_BETA_SCALING_TX_AGC
  byte rev_index;
  int2 trim_val[RF_NUM_TX_LINEARIZERS]; 
  int tx_min_pwr;
  int tx_max_pwr;
  int2 temp_pdm;
  int2 temp_pwr;
  int2 temp_max;
  int2 temp_min;
  byte i;
  int j;
  byte last_good_cal_index;
  boolean is_data_valid[RF_NUM_TX_LINEARIZERS] = {TRUE,TRUE,TRUE,TRUE};
  int2 pa_rise_val[RF_NUM_TX_LINEARIZERS - 1];
  byte max_pwr_index;
  int2 latest_valid_pwr;
#else
  int    temp;
#endif

  rf_chain_state_type *rf_chain_ptr =
          &rf_chain_status[ rfcom_to_path[ RFCOM_TRANSCEIVER_0 ] ];

  /*
   * "PDM vs P_out" mapping is different for 1X and UMTS.
   * 1X:   Increasing PDM Decreases P_out
   * UMTS: Increasing PDM Increases P_out
   *
   * Thus, for TX devices originally developed for UMTS and now used for 1X,
   * we need to send (511-PDM) to the MDSP. (For TX devices developed for 1X
   * and used for 1X, we just send "PDM" to the MDSP.)
   *
   * The flag umts_based_tx_card is used to send PDM or (511-PDM) to the MDSP.
   */
  switch (rf_card_in_system)
  {
    case RF_HW_WDZRF:
    case RF_HW_DZRF6285:
      umts_based_tx_card = 1;
      break;

    default:
      umts_based_tx_card = 0;
      break;
  }


#ifdef RF_HAS_BETA_SCALING_TX_AGC
/* Here Beta Scaling is enabled */

  rf_get_tx_min_max_pwr( rf_chain_ptr->rf_curr_cdma_band,
                         &tx_min_pwr, &tx_max_pwr );
  
  for (i = 0; i < RF_NUM_TX_LINEARIZERS; i++)
  {
    #ifdef RF_HAS_LINEAR_INTERPOLATION
    trim_val[i] = rf_freq_comp.i_lin_freq_comp[i];
    #else
    trim_val[i] = (int2)(rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_tx_comp_vs_freq[i][rf_chain_ptr->rf_freq_index]);
    #endif       
  }

  pa_rise_val [0] = (int2) ((word) rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_r1_rise);

  pa_rise_val [1] = (int2) ((word) rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_r2_rise);

  pa_rise_val [2] = (int2) ((word) rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_r3_rise);

  /* Check if the PA Switchpoint is equal to the Maximum Rise Value*/
  /* In this Case the PA state is not Valid and there is no need to */
  /* Provide the data to MDSP */

  for(i=0;i<RF_NUM_TX_LINEARIZERS - 1;i++) 
  {    
    if (pa_rise_val[i] == RF_CDMA_MDSP_PA_SWITCHPOINT_INVALID_VAL)
    {
      is_data_valid[i+1] = FALSE;
    }
  }

  /*
 * NOTE: 1X calibration data is stored in the NV from the largest TX
 * level to the smallest TX level. The Beta Scaling algorithm in MDSP
 * needs this data in the reverse order.
 *
 * Furthermore, the MDSP needs TX levels in a monotonically ascending order.
 *
 * The following code (1) searches for the largest TX power level in the
 * first 1/8 of the data (see RF_TX_CDMA_BETA_SCALING_PDM_TBL_SIZ >> 3).
 * The search is limited to this subset to save time. (It is reasonable
 * to assume that the largest TX power level will be found in this subset.).
 * It will only search the Maximum Value for the Valid Gain state for which
 * PA_Rise is not equal to 255. 
 * (2) Next, starting with this largest TX power level, (PWR,PDM) pairs
 * are sent to the MDSP such that the monotonicity of the power levels
 * is preserved; i.e., any PWR level value which breaks this monotonicity
 * is not sent to the MDSP. (3) Finally, due to items (1) and (2), the
 * MDSP might have received fewer than RF_TX_CDMA_BETA_SCALING_PDM_TBL_SIZ
 * pairs. Hence the last (PWR,PDM) pair is sent to the MDSP repeatedly
 * until the MDSP receives RF_TX_CDMA_BETA_SCALING_PDM_TBL_SIZ pairs.
 */

  /* Load TX linearizer */
  for (i = 0; i < RF_NUM_TX_LINEARIZERS; i++)
  {
    /* Find the largest TX power level. Limit the search to a quarter of the
     * PWR array: from index = RF_TX_CDMA_BETA_SCALING_PDM_TBL_SIZ >> 3 to 0
     */
     /*Check if the PA gain state is Valid*/
     if (is_data_valid[i] == TRUE) 
     {
       index = (byte)(RF_TX_CDMA_BETA_SCALING_PDM_TBL_SIZ >> 3);
       temp_pwr  = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_tx_lin_tbl[i][0];
       max_pwr_index =  0;

       for (rev_index = 1; rev_index <= index  ; rev_index++)
       {
         if (rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_tx_lin_tbl[i][rev_index] >= temp_pwr)
         {
           temp_pwr = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_tx_lin_tbl[i][rev_index];
           max_pwr_index = rev_index;
         }
       }

       /* Send (PWR,PDM) pairs to the MDSP starting with the largest PWR value */
       /* Add the corresponding Frequency Compensation */
       #ifdef RF_HAS_TX_TEMP_FREQ_COMP_NV_BASED_ON_PWR
       temp_pwr = temp_pwr + (int16)(((int32)(rf_cdma_tx_slp_vs_temp_now[i] * (tx_max_pwr - temp_pwr)))/(tx_max_pwr - tx_min_pwr));
       latest_valid_pwr = trim_val[i] + temp_pwr + rf_cdma_tx_lin_vs_temp_now[i];
       #else
       latest_valid_pwr = trim_val[i] + temp_pwr;
       #endif
       last_good_cal_index = max_pwr_index;

       rev_index = RF_TX_CDMA_BETA_SCALING_PDM_TBL_SIZ;       

       for (index = max_pwr_index; index < RF_TX_CDMA_BETA_SCALING_PDM_TBL_SIZ; index++)
       {
         if((rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_tx_lin_tbl[i][index] == RF_CDMA_TX_BETA_SCALING_PWR_NOT_USED_VAL) || 
            (rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_tx_pdm_tbl[i][index] == RF_CDMA_TX_BETA_SCALING_PDM_NOT_USED_VAL))
         {
           rev_index =  rev_index - 1;

           temp_min = rf_get_tx_lin_off_min();
           temp_max = rf_get_tx_lin_off_max();
           /* Limit the range of the PDM value for table 0 */
           temp_pdm = RF_SATURATE( rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_tx_pdm_tbl[i][last_good_cal_index],
                                   temp_min, temp_max  );

           //    temp = umts_based_tx_card ? ~temp : temp;
           temp_pdm = umts_based_tx_card ? (511-temp_pdm) : temp_pdm;

           temp_pwr = latest_valid_pwr;

           temp_pwr = RF_SATURATE( temp_pwr, tx_min_pwr, tx_max_pwr );

           rf_cdma_mdsp_set_beta_scaling_pdm_pwr_val(i,rev_index,(uint32)temp_pdm,temp_pwr - tx_min_pwr);
         }
         else
         {
           #ifdef RF_HAS_TX_TEMP_FREQ_COMP_NV_BASED_ON_PWR
           temp_pwr = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_tx_lin_tbl[i][index];           

           temp_pwr = temp_pwr + (int16)(((int32)(rf_cdma_tx_slp_vs_temp_now[i] * (tx_max_pwr - temp_pwr)))/(tx_max_pwr - tx_min_pwr));

           temp_pwr = temp_pwr + rf_cdma_tx_lin_vs_temp_now[i]+ trim_val[i]; 
           #else
           temp_pwr = trim_val[i] + rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_tx_lin_tbl[i][index];
           #endif

           temp_pwr = RF_SATURATE( temp_pwr, tx_min_pwr, tx_max_pwr );

           if (temp_pwr <= latest_valid_pwr)
           {
             temp_min = rf_get_tx_lin_off_min();
             temp_max = rf_get_tx_lin_off_max();

             if(temp_pwr == latest_valid_pwr)
             {             
               temp_pdm = RF_SATURATE( rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_tx_pdm_tbl[i][last_good_cal_index],
                                       temp_min, temp_max );
             }
             else
             {             
               temp_pdm = RF_SATURATE( rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_tx_pdm_tbl[i][index],
                                       temp_min, temp_max );

               last_good_cal_index = index;
             }
             //    temp = umts_based_tx_card ? ~temp : temp;
             temp_pdm = umts_based_tx_card ? (511-temp_pdm) : temp_pdm;
             
             rev_index =  rev_index - 1;             

             latest_valid_pwr = temp_pwr;

             rf_cdma_mdsp_set_beta_scaling_pdm_pwr_val(i,rev_index,(uint32)temp_pdm,temp_pwr - tx_min_pwr);            
           }
           else
           {
             #ifdef RF_HAS_SC2X_BETA_SCALING_DEBUG
#error code not present
             #endif
           }
         }
       }/*for (index = max_pwr_index; index < RF_TX_CDMA_BETA_SCALING_PDM_TBL_SIZ; index++)*/

       for (j= rev_index - 1 ; j >= 0 ; j--)
       {
         temp_min = rf_get_tx_lin_off_min();
         temp_max = rf_get_tx_lin_off_max();

         temp_pdm = RF_SATURATE( rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_tx_pdm_tbl[i][last_good_cal_index],
                                 temp_min, temp_max );
         //    temp = umts_based_tx_card ? ~temp : temp;
         temp_pdm = umts_based_tx_card ? (511- temp_pdm) : temp_pdm;

         temp_pwr = latest_valid_pwr;

         temp_pwr = RF_SATURATE( temp_pwr, tx_min_pwr, tx_max_pwr );

         rf_cdma_mdsp_set_beta_scaling_pdm_pwr_val(i,j,(uint32)temp_pdm,temp_pwr - tx_min_pwr);
      }
    } /*if (is_data_valid[i] == TRUE)*/
  }/*for (i = 0; i < RF_NUM_TX_LINEARIZERS; i++)*/

#else  /* RF_HAS_BETA_SCALING_TX_AGC */
/* Here Beta Scaling is not enabled */


  /* Load TX linearizer */
  for (index = 0; index < RF_TX_AGC_NUM_LINEARIZER_VAL; index++)
  {
    /* Limit the range of the PDM value for table 0 */
    temp = RF_SATURATE( rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_tx_lin_master_off[0][index+2],
                        rf_get_tx_lin_off_min(),
                        rf_get_tx_lin_off_max() );
//    temp = umts_based_tx_card ? ~temp : temp;
    temp = umts_based_tx_card ? (511-temp) : temp;
    CDMA_MDSP_SET_TX_LIN0_ENTRY( index, RF_CDMA_MDSP_CONVERT_TX_LIN_VAL((uint16)temp));


    /* Limit the range of the PDM value for table 1 */
    temp = RF_SATURATE( rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_tx_lin_master_off[1][index+2],
                        rf_get_tx_lin_off_min(),
                        rf_get_tx_lin_off_max() );
    temp = umts_based_tx_card ? (511-temp) : temp;
    CDMA_MDSP_SET_TX_LIN1_ENTRY( index, RF_CDMA_MDSP_CONVERT_TX_LIN_VAL((uint16)temp));

    /* Limit the range of the PDM value for table 2 */
    temp = RF_SATURATE( rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_tx_lin_master_off[2][index+2],
                        rf_get_tx_lin_off_min(),
                        rf_get_tx_lin_off_max() );
    temp = umts_based_tx_card ? (511-temp) : temp;
    CDMA_MDSP_SET_TX_LIN2_ENTRY( index, RF_CDMA_MDSP_CONVERT_TX_LIN_VAL((uint16)temp));

    /* Limit the range of the PDM value for table 3 */
    temp = RF_SATURATE( rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_tx_lin_master_off[3][index+2],
                        rf_get_tx_lin_off_min(),
                        rf_get_tx_lin_off_max() );
    temp = umts_based_tx_card ? (511-temp) : temp;
    CDMA_MDSP_SET_TX_LIN3_ENTRY( index, RF_CDMA_MDSP_CONVERT_TX_LIN_VAL((uint16)temp));
  }

#endif /* RF_HAS_BETA_SCALING_TX_AGC */

 } /* rf_cdma_mdsp_update_tx_lin_tables */

#ifdef RF_HAS_BETA_SCALING_TX_AGC
/*===========================================================================

FUNCTION RF_CDMA_MDSP_SET_BETA_SCALING_PDM_PWR_VAL      EXTERNALIZED FUNCTION

DESCRIPTION
This function provides the PDM and Power Values to MDSP

DEPENDENCIES
  None

RETURN VALUE
None

SIDE EFFECTS
  None

===========================================================================*/
 void rf_cdma_mdsp_set_beta_scaling_pdm_pwr_val(byte lin_index,byte index,uint32 temp_pdm_val,int temp_pwr_val)
 { 
   #ifdef RF_SUPPORTS_32_BIT_TX_AGC_INTERFACE   
   uint16 rf_mdsp_pdm_index;
   
   rf_chain_state_type *rf_chain_ptr =
           &rf_chain_status[ rfcom_to_path[ RFCOM_TRANSCEIVER_0 ] ];

   rf_mdsp_pdm_index = (uint16)(temp_pdm_val);

   temp_pdm_val = rft_get_tx_agc_adj_32_bit(lin_index, rf_mdsp_pdm_index, rf_chain_ptr->rf_curr_cdma_band);   
   #endif

   switch (lin_index)
   {
   case 0:
     CDMA_MDSP_SET_TX_PDM0_ENTRY( index, RF_CDMA_MDSP_CONVERT_BETAP_TX_PDM_VAL(temp_pdm_val));
     CDMA_MDSP_SET_TX_LIN0_ENTRY( index, RF_CDMA_MDSP_CONVERT_BETAP_TX_LIN_VAL(temp_pwr_val));
     break;

   case 1:
     CDMA_MDSP_SET_TX_PDM1_ENTRY( index, RF_CDMA_MDSP_CONVERT_BETAP_TX_PDM_VAL(temp_pdm_val));
     CDMA_MDSP_SET_TX_LIN1_ENTRY( index, RF_CDMA_MDSP_CONVERT_BETAP_TX_LIN_VAL(temp_pwr_val));
     break;

   case 2:
     CDMA_MDSP_SET_TX_PDM2_ENTRY( index, RF_CDMA_MDSP_CONVERT_BETAP_TX_PDM_VAL(temp_pdm_val));
     CDMA_MDSP_SET_TX_LIN2_ENTRY( index, RF_CDMA_MDSP_CONVERT_BETAP_TX_LIN_VAL(temp_pwr_val));   
     break;

   case 3:
     CDMA_MDSP_SET_TX_PDM3_ENTRY( index, RF_CDMA_MDSP_CONVERT_BETAP_TX_PDM_VAL(temp_pdm_val));
     CDMA_MDSP_SET_TX_LIN3_ENTRY( index, RF_CDMA_MDSP_CONVERT_BETAP_TX_LIN_VAL(temp_pwr_val));     
     break;

   default:
     RF_FATAL_ERROR("PA Gain State not supported");   
     break;
   }
 }
#endif /* RF_HAS_BETA_SCALING_TX_AGC */

/*===========================================================================

FUNCTION RF_CDMA_MDSP_UPDATE_TX_COMP             EXTERNALIZED FUNCTION

DESCRIPTION
This function updates the Linearizer Parameters to MDSP based 
upon the temperature and frequency change so that MDSP can 
regenerate the new Linearizer tables for the current band.

Note that the whole new set of Linearizer tables are 
Provided to MDSP on Band Change Only.

For Channel or temperature change it has to generate the 
new tables from these parameters passed.

DEPENDENCIES
  None

RETURN VALUE
None

SIDE EFFECTS
  None
===========================================================================*/
void rf_cdma_mdsp_update_tx_comp( void )
{       

#ifndef RF_HAS_BETA_SCALING_TX_AGC
/* Here Beta Scaling is not enabled */


  int table;                     /* Linearizer table index */
  int16 tx_freq_comp;            /* Comp value for linearizer tables. */

  #ifndef RF_HAS_LINEAR_INTERPOLATION
  /* use RF status structure for RFCOM_TRANSCEIVER_0 */
  rf_chain_state_type *rf_chain_ptr =
          &rf_chain_status[ rfcom_to_path[ RFCOM_TRANSCEIVER_0 ] ];
  #endif
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

   for (table=0; table < RF_NUM_TX_LINEARIZERS; table++)
   {
      #ifdef RF_HAS_LINEAR_INTERPOLATION
      tx_freq_comp = -rf_freq_comp.i_lin_freq_comp[table];
    #else
    tx_freq_comp = -((int2)rf_chain_ptr->rf_nv_dig_items_ptr->
       rfnv_cdma_tx_comp_vs_freq[table][rf_chain_ptr->rf_freq_index]);
      #endif /* RF_HAS_LINEAR_INTERPOLATION */

      CDMA_MDSP_SET_TX_LIN_X_OFFSET( table, RF_CDMA_MDSP_TX_DB_OFFSET + RF_CDMA_MDSP_10_TO_16_BIT(tx_freq_comp));
      CDMA_MDSP_SET_TX_LIN_Y_OFFSET( table, RF_CDMA_MDSP_9_TO_16_BIT(rf_cdma_tx_lin_vs_temp_now[table]));
      CDMA_MDSP_SET_TX_LIN_SLOPE( table, RF_CDMA_MDSP_9_TO_16_BIT(rf_cdma_tx_slp_vs_temp_now[table]/2));
  }
  CDMA_MDSP_NEW_TX_LIN_SLOT_PARAMS_AVAILABLE();

#endif /* RF_HAS_BETA_SCALING_TX_AGC */
}
#endif /* RF_HAS_MDSP_TX_AGC */

/*===========================================================================

FUNCTION RF_GET_MIN_TX_PWR_IN_DB                            EXTERNAL FUNCTION

DESCRIPTION
  Returns the minimum TX power rail for CDMA cellular (in dBm)

DEPENDENCIES
  Should be called after NV is read

RETURN VALUE
  int2

SIDE EFFECTS
  None

===========================================================================*/
int2 rf_get_min_tx_pwr_in_dB( void )
{
  return cdma_cellular_pwr_rails.cdma_tx_pwr_min;
}

/*===========================================================================

FUNCTION RF_GET_TX_LIN_MIN                                 INTERNAL FUNCTION

DESCRIPTION
  This function returns the minimum value for Tx linearizer offsets.

DEPENDENCIES
  RFT device and band in chain pointer must be set before calling this function.
RETURN VALUE
  Minimum value for offset of Tx linearizers.

SIDE EFFECTS
  None
===========================================================================*/
int2 rf_get_tx_lin_off_min(void)
{

   switch (rf_path_0.rftdev)
   {
   case RTR6500_TX :
#ifdef RF_HAS_PDM_VS_POWER_TX_CAL
/* Operating linear PDM values for RTR6500 are between 80 and 383 */ 
       return rfagc_get_tx_pdm_vs_pwr_lin_off_min();
#else
       return  RF_TX_LIN_OFF_MIN;
#endif   
   default:
      return  RF_TX_LIN_OFF_MIN;
   };
}

/*===========================================================================

FUNCTION RF_GET_TX_LIN_MAX                                 INTERNAL FUNCTION

DESCRIPTION
  This function returns the maximum value for Tx linearizer offsets.

DEPENDENCIES
  RFT device and band in chain pointer must be set before calling this function.

RETURN VALUE
  Maximum value for offset of Tx linearizers.

SIDE EFFECTS
  None
===========================================================================*/
int2 rf_get_tx_lin_off_max(void)
{

   switch (rf_path_0.rftdev)
   {
   case RTR6500_TX :
#ifdef RF_HAS_PDM_VS_POWER_TX_CAL
      return rfagc_get_tx_pdm_vs_pwr_lin_off_max();
#else
      switch (rf_chain_status[RF_PATH_0].rf_curr_cdma_band)
      {
      case  RF_BC0_BAND:
      case  RF_BC3_BAND:
      case  RF_BC10_BAND:
      /*  MIN LIN PDM value for RTR6500 due to requirement for GPS RX desense
          is 469 for low band */
         return 469;
      default:
         return  RF_TX_LIN_OFF_MAX;
      };
#endif
     break;

   case QSC11X0_TX:
     return 63;        

   default:
      return  RF_TX_LIN_OFF_MAX;
   };
}

/*===========================================================================

FUNCTION RF_GET_CAGC_VAL                                             EXTERNAL

DESCRIPTION
  This function can be used to calculate the power in dBm corresponding to
  the register value for any of the following in either the 85 dB or 102
  range: AGC_VALUE (RX RSSI), TX_OPEN_LOOP (TX estimation of RX RSSI),
  TX_GAIN_CTL (TX AGC), TX_GAIN_ADJ.

  A generic structure (passed via a pointer) is populated that allows
  the power to be calculated with one generic formula.


  Using this structure power can be calculated for any of the readings per the
  following:

  power ( 1/100 dB resolution ) =
    val_min + polarity * scale_factor_numer * val_index * 100/scale_factor_denom

  This is an unrounded result....there is maximum one count (1/100 db) error inherent
  in the calculation.  However, since the maximum resolution of any value read is
  1/12 dB (85 dB range) the rounding error is inconsequential.


DEPENDENCIES
  Should not be called until after NV is read

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rf_get_cagc_val( rf_cagc_val_type val_to_get, 
                             rf_cagc_vals_vars_type *val_struct )
{
    rf_get_path_cagc_val ( RFCOM_TRANSCEIVER_0, val_to_get, val_struct );
}

/*===========================================================================

FUNCTION RF_GET_PATH_CAGC_VAL                                         EXTERNAL

DESCRIPTION
  Multi-path version of rf_get_cagc_val()

DEPENDENCIES
  Should not be called until after NV is read

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rf_get_path_cagc_val( rfcom_device_enum_type device, 
                                  rf_cagc_val_type val_to_get , 
                                  rf_cagc_vals_vars_type *val_struct )
{
  int2 cagc_val;

  switch ( val_to_get ) {
  case RX_AGC_VAL :

      /* Get the RxAGC value */
      cagc_val = rf_get_path_1x_agc(device);

      val_struct->val_index =  ( cagc_val + RF_RX_AGC_VAL_OFFSET ) & RX_AGC_VAL_MASK;

      if ( cagc_dynamic_range_setting == RF_HAS_85DB_CDMA_DYNAMIC_RANGE )
      {
        val_struct->val_min = MIN_RX_PWR_85_DB;
        val_struct->val_max = MAX_RX_PWR_85_DB;
        val_struct->scale_factor_numer = 1;
        val_struct->scale_factor_denom = 12;
      }
      else
      {
        val_struct->val_min = min_rx_pwr_102_db;
        val_struct->val_max = max_rx_pwr_102_db;
        val_struct->scale_factor_numer = 1;
        val_struct->scale_factor_denom = 10;
      }

      val_struct->polarity = 1;
      break;

    case TX_OPEN_LOOP_VAL :

      #ifdef RF_HAS_MDSP_TX_AGC
      cagc_val = (int2)( CDMA_MDSP_RD_TX_OPEN_LOOP_VAL() ) >> 6;
      cagc_val =  (cagc_val + RF_CDMA_MDSP_TX_OPEN_LOOP_OFFSET) & RF_CDMA_MDSP_TX_OPEN_LOOP_MASK ;
      #else
      cagc_val = (int2)HWIO_IN( TX_OPEN_LOOP_RD );
      #endif

      val_struct->val_index =  ( cagc_val + RF_RX_AGC_VAL_OFFSET ) & RX_AGC_VAL_MASK;

      if ( cagc_dynamic_range_setting == RF_HAS_85DB_CDMA_DYNAMIC_RANGE )
      {
        val_struct->val_min = MIN_RX_PWR_85_DB;
        val_struct->val_max = MAX_RX_PWR_85_DB;
        val_struct->scale_factor_numer = 1;
        val_struct->scale_factor_denom = 12;
      }
      else
      {
        val_struct->val_min = min_rx_pwr_102_db;
        val_struct->val_max = max_rx_pwr_102_db;
        val_struct->scale_factor_numer = 1;
        val_struct->scale_factor_denom = 10;
      }

      val_struct->polarity = 1;
      break;

    case TX_AGC_VAL :

      #ifdef RF_HAS_MDSP_TX_AGC 
      cagc_val = (int2)(CDMA_MDSP_RD_TRANSMIT_GAIN()) >> 6 ;
      cagc_val =  (cagc_val + RF_CDMA_MDSP_TX_TOTAL_GAIN_OFFSET) & RF_CDMA_MDSP_TX_TOTAL_GAIN_MASK;
      #else
      RF_LATCH_TX_GAIN_CTL_RD();
      cagc_val = (int2)HWIO_INM(TX_GAIN_CTL_RD, TX_GAIN_CTL_M);
      #endif

      val_struct->val_index =  ( cagc_val + RF_TX_AGC_VAL_OFFSET ) & TX_AGC_VAL_MASK;

      if ( cagc_dynamic_range_setting == RF_HAS_85DB_CDMA_DYNAMIC_RANGE )
      {
        val_struct->val_min = MAX_TX_PWR_85_DB;
        val_struct->val_max = MIN_TX_PWR_85_DB;
        val_struct->scale_factor_numer = 1;
        val_struct->scale_factor_denom = 12;
      }
      else
      {
        val_struct->val_min = max_tx_pwr_102_db;
        val_struct->val_max = min_tx_pwr_102_db;
        val_struct->scale_factor_numer = 1;
        val_struct->scale_factor_denom = 10;
      }

      val_struct->polarity = -1;
      break;

    case TX_GAIN_ADJ_VAL :
      #ifdef RF_HAS_MDSP_TX_AGC
      cagc_val = (int2)(CDMA_MDSP_RD_TX_CLOSED_LOOP_VAL()) >> 7;
      cagc_val =  (cagc_val + RF_CDMA_MDSP_TX_CLOSED_LOOP_OFFSET) & RF_CDMA_MDSP_TX_CLOSED_LOOP_MASK;
      val_struct->val_index =  (int2)(( cagc_val + RF_TX_GAIN_ADJ_OFFSET ) & 0x1ff);
      #else
      cagc_val = (int2)HWIO_IN( TX_GAIN_ADJ_RD );
      val_struct->val_index =  (int2)(( cagc_val + RF_TX_GAIN_ADJ_OFFSET ) & HWIO_RMSK(TX_GAIN_ADJ_RD));
      #endif      
      val_struct->val_min = 6400;
      val_struct->val_max = -6400;
      val_struct->scale_factor_numer = 1;
      val_struct->scale_factor_denom = 4;
      val_struct->polarity = -1;
      break;

  default:
      MSG_ERROR("Invalid agc_val_type",0,0,0);
      break;
  }
}

/*____________________________________________________________________________
                       TX CAGC Functions
____________________________________________________________________________*/


/*===========================================================================
FUNCTION RF_GET_TX_MIN_MAX_PWR                             EXTERNAL FUNCTION

DESCRIPTION
  This function returns minimum and maximum TX power (in 1/10 dBm units)
  for a given band.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rf_get_tx_min_max_pwr (rf_card_band_type  rf_curr_cdma_band,
                            int *p_tx_min_pwr, int *p_tx_max_pwr)
{
  if ((rf_curr_cdma_band == RF_BC1_BAND) ||
      (rf_curr_cdma_band == RF_BC4_BAND) ||
      (rf_curr_cdma_band == RF_BC6_BAND) ||
      (rf_curr_cdma_band == RF_BC14_BAND)||
      (rf_curr_cdma_band == RF_BC15_BAND))
  {
    *p_tx_min_pwr = -634;  /* -63.4 dBm: PCS */
  }
  else
  {
    *p_tx_min_pwr = -604;  /* -60.4 dBm: Cell */
  }
  *p_tx_max_pwr = *p_tx_min_pwr + 1023; /* 102.4 dBm: TX dynamic range */
}


/*===========================================================================

FUNCTION RF_GET_TX_OPEN_LOOP                               EXTERNAL FUNCTION

DESCRIPTION
  Returns the 10 bit sign extended TX Open Loop Accumulator value (-512...511).
  The Tx Open Loop signal strength is calculated the same as Rx signal
  strength.

  Tx Open Loop strength is:

  Strength in dBm = -63.248 + (( rf_get_tx_open_loop * 883 ) / 10000 )

  That's -0.0833 dB per count referenced to -63.248dBm.  Since this returns
  int2, this is a numeric Rx signal strength from -106dBm to -20.83dBm.

  When 102 dB dynamic range is used the AGC value is scaled into the
  85 dB range.  AGC values for powers greater than -21 dBm are saturated
  to -21 dBm (511).  AGC values for powers less than -106 dBm are saturated
  to -106 dBm (-512).



DEPENDENCIES
  Should be called within an MSM_LOCK(), MSM_UNLOCK() pair

RETURN VALUE
  int16

SIDE EFFECTS
  None

===========================================================================*/
int16 rf_get_tx_open_loop( void )
{
  int16 tx_open_loop;

  #ifdef RF_HAS_MDSP_TX_AGC
  tx_open_loop = (int16)((int16)( CDMA_MDSP_RD_TX_OPEN_LOOP_VAL() ) >> 6);   
  tx_open_loop =  (tx_open_loop + RF_CDMA_MDSP_TX_OPEN_LOOP_OFFSET) & RF_CDMA_MDSP_TX_OPEN_LOOP_MASK ;
  #else
  tx_open_loop = (int16)HWIO_IN( TX_OPEN_LOOP_RD );
  #endif
  if ( cagc_dynamic_range_setting == RF_HAS_102DB_CDMA_DYNAMIC_RANGE )
  {
    /* Scale down to 85 dBm range if necessary.
    */

    /* convert to 0..1023 range */
    tx_open_loop =  ( tx_open_loop + RF_RX_AGC_VAL_OFFSET ) & RX_AGC_VAL_MASK;

    /* saturate to agc values corresponding to min/max for 85 dBm */
    tx_open_loop = RF_SATURATE( tx_open_loop, rx_102_dB_agc_min_val, rx_102_dB_agc_max_val );

    /* scale value and return as -512...511*/
    tx_open_loop = ( ( (tx_open_loop - rx_102_dB_agc_min_val) * RX_AGC_VAL_CNTS_PER_DB_85_DB / RX_AGC_VAL_CNTS_PER_DB_102_DB )
             + RF_RX_AGC_VAL_OFFSET ) & RX_AGC_VAL_MASK;
  }

  /* Bit 9 is sign bit...if it is set we need to sign extend */
  return (int16)(( tx_open_loop > 511 ) ? tx_open_loop | 0xfc00 : tx_open_loop);
}


/*===========================================================================
                                                                   EXTERNAL
FUNCTION RF_GET_RAW_CDMA_TX_AGC

DESCRIPTION
  This function returns the raw CDMA Tx AGC setting in 10 bit binary format
  in the range 0...1023.

  A return value of 1023 corresponds to the maximum Tx power. A return value
  of 0 corresponds to the minimum Tx power.

  Tx power is caluclated as follows:

  Tx power in dBm = MINIMUM_TX_POWER + (1/12) * rf_get_raw_tx_agc()

  MINIMUM_TX_POWER is defined by calibration.

DEPENDENCIES
  None

RETURN VALUE
  word = 10 bit Tx AGC value

SIDE EFFECTS
  None

===========================================================================*/
word rf_get_raw_cdma_tx_agc( void )
{
  int2 temp_int2;    /* temp for straightening out the 10 bit port data    */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* strobe the CAGC uP_READ_SYNC bit to capture the data
  */
  
  #ifdef RF_HAS_MDSP_TX_AGC 
  temp_int2 =  (int2)((int2)(CDMA_MDSP_RD_TRANSMIT_GAIN()) >> 6);
  temp_int2 =  (temp_int2 + RF_CDMA_MDSP_TX_TOTAL_GAIN_OFFSET) & RF_CDMA_MDSP_TX_TOTAL_GAIN_MASK;
  #else
  RF_LATCH_TX_GAIN_CTL_RD();
  temp_int2 = (int2)HWIO_INM(TX_GAIN_CTL_RD, TX_GAIN_CTL_M);
  #endif
  /* if packed data was a negative 10 bit number, sign extend it to 16 bits
  */
  if (temp_int2 > RF_TX_PWR_REG_MAX) {
    temp_int2 |= (int2)RF_TX_PWR_REG_MSBS;
  }

  /* convert to 0...1023 range which corresponds to :
  **  -52...33 dBm for 85 dB range
  **  ???...?? dBm for 102 dB range
  */
  return( (word)((int2)RF_TX_PWR_REG_MAX - temp_int2));

} /* rf_get_raw_cdma_tx_agc */



/*===========================================================================

FUNCTION RF_GET_CDMA_TX_AGC                             EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns the CDMA Tx AGC setting in 10 bit binary format
  where the ranger returned is 0...1023 (-52 dBm to +33 dBm).

    Tx signal strength is:

    Tx strength in dBm = -52.25 + (( rf_get_cdma_tx_agc * 0833 ) / 10000 )

    That's +0.085dB per count referenced to -52.25dBm.  Since this returns
    word, this is a numeric Tx signal strength from -52.25dBm to +32.97dBm.

  When 102 dB dynamic range is used the AGC value is scaled into the
  85 dB range.  AGC values for powers greater than +33 dBm are saturated
  to +33 dBm (1023).  AGC values for powers less than -52 dBm are saturated
  to -52 dBm (0).

DEPENDENCIES
  None

RETURN VALUE
  word = 10 bit Tx AGC value

SIDE EFFECTS
  None

===========================================================================*/
word rf_get_cdma_tx_agc( void )
{
  word tx_agc;
  #ifdef RF_HAS_MDSP_TX_AGC
  int2 tx_agc_1;
  #endif

  tx_agc = rf_get_raw_cdma_tx_agc();

  if ( cagc_dynamic_range_setting == RF_HAS_102DB_CDMA_DYNAMIC_RANGE )
  {
    /* Scale down to 85 dBm range if necessary.
    */
    #ifdef RF_HAS_MDSP_TX_AGC 
    tx_agc_1  =  (int2)(CDMA_MDSP_RD_TRANSMIT_GAIN()) >> 6  ;
    tx_agc = (word)(( tx_agc_1 + RF_CDMA_MDSP_TX_TOTAL_GAIN_OFFSET) & RF_CDMA_MDSP_TX_TOTAL_GAIN_MASK);
    #else
    RF_LATCH_TX_GAIN_CTL_RD();
    tx_agc = (word)HWIO_INM(TX_GAIN_CTL_RD, TX_GAIN_CTL_M);
    #endif
    /* convert to 0..1023 range where 0->MAX Power and 1023->MIN Power*/
    tx_agc =  ( tx_agc + RF_TX_AGC_VAL_OFFSET ) & TX_AGC_VAL_MASK;

    /* saturate to agc values corresponding to min/max for 85 dBm */
    tx_agc = RF_SATURATE( tx_agc, tx_102_dB_agc_min_val, tx_102_dB_agc_max_val );/*lint !e732 !e737 Loss of sign - MAX macro */

    /* scale value */
    tx_agc = (word)( (tx_agc - tx_102_dB_agc_min_val) * TX_AGC_VAL_CNTS_PER_DB_85_DB / TX_AGC_VAL_CNTS_PER_DB_102_DB );

    /* callers want 0->MIN Power and 1023->MAX Power */
    tx_agc = RF_TX_AGC_MAX - tx_agc;
  }

  return (tx_agc);
} /* rf_get_cdma_tx_agc */


/*===========================================================================

FUNCTION RF_RESCALE_TX_PWR_LIMT                             INTERNAL FUNCTION

DESCRIPTION
  This function rescales the power limit value used in the function
  rf_get_cdma_tx_pwr_limit() (see rftxplim.c) from 102 dB to 85 dB range.

DEPENDENCIES
  Should not be called until after NV is read

RETURN VALUE
  word : power limit value rescaled from 102 dB range to 85 dB range

SIDE EFFECTS
  None
===========================================================================*/
word rf_rescale_tx_pwr_limit(word tx_pwr_limit_to_scale)
{

  /* saturate to 85 dB range */
  tx_pwr_limit_to_scale = RF_SATURATE( tx_pwr_limit_to_scale,
                                       tx_102_dB_pwr_limit_min_val,
                                       tx_102_dB_pwr_limit_max_val);/*lint !e732 !e737 */

  /* scale down to 85 dB range */
  return (word)( (tx_pwr_limit_to_scale - tx_102_dB_pwr_limit_min_val) *  TX_AGC_VAL_CNTS_PER_DB_85_DB / TX_AGC_VAL_CNTS_PER_DB_102_DB );

}



/*===========================================================================

FUNCTION RF_TX_GAIN_ADJ_STEP_SIZE                       EXTERNALIZED FUNCTION

DESCRIPTION
  This function sets the power control step size for each up/down
  bit. The step size can be 1, 0.5 or 0.25 dB.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rf_tx_gain_adj_step_size( byte step_size )
{
  
  #ifdef RF_HAS_MDSP_TX_AGC
  uint16 dsp_step_size = 0x200;

/* Convert the HW Configured Step Size to the value in MDSP
   which is in units of 1/512 of dB */

  if(step_size == 0x0)
  {
    dsp_step_size = 0x200;
  }
  else if(step_size == 0x1)
  {
    dsp_step_size = 0x100;
  }
  else if(step_size == 0x2)
  {
    dsp_step_size = 0x80;
  }
  else
  {
    MSG_ERROR ("Step Size not configurable",0,0,0);
    return;
  }
  CDMA_MDSP_WR_TX_GAIN_ADJ_STEP_SIZE( dsp_step_size );
  #else        
   MSM_W_MASKED(TX_AGC_CTL, TX_GAIN_ADJ_STEP_SIZE, step_size);
  #endif
}



/*===========================================================================

FUNCTION RF_GET_TX_GAIN_ADJ                             EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns the current value of the Tx gain adjust register.
  If extended linearizers were used, the value is converted from the
  extended linearizer range to the expected 85 dB range (giving 1/2 dB
  resolution).

DEPENDENCIES
  None.

RETURN VALUE
  int1 - Tx gain adjust register value (8 bits @ 1/2 dB resolution).

SIDE EFFECTS
  None.

===========================================================================*/
int1 rf_get_tx_gain_adj( void )
{

  int2 tx_gain_adj;

  /* Read value from tx gain adjust register.
  */
  #ifdef RF_HAS_MDSP_TX_AGC
  tx_gain_adj = (int2)((int2)(CDMA_MDSP_RD_TX_CLOSED_LOOP_VAL() ) >> 7);     
  tx_gain_adj =  (int1)((tx_gain_adj + RF_CDMA_MDSP_TX_CLOSED_LOOP_OFFSET) & RF_CDMA_MDSP_TX_CLOSED_LOOP_MASK);
  #else
  tx_gain_adj = (int1)HWIO_IN( TX_GAIN_ADJ_RD );
  #endif

  return ((int1)tx_gain_adj);

} /* rf_get_tx_gain_adj */


/*===========================================================================

FUNCTION RF_GET_TX_GAIN_ADJUST                              EXTERNAL FUNCTION

DESCRIPTION
  Returns the 9 bit sign extended TX Gain Adjust register value

DEPENDENCIES
  Should be called within an MSM_LOCK(), MSM_UNLOCK() pair

RETURN VALUE
  int16

SIDE EFFECTS
  None

===========================================================================*/
int16 rf_get_tx_gain_adjust( void )
{
    int16 tx_gain_adj;

    #ifdef RF_HAS_MDSP_TX_AGC
    tx_gain_adj =  (int16)((int16)(CDMA_MDSP_RD_TX_CLOSED_LOOP_VAL()) >> 7);     
    tx_gain_adj =  (int16)((tx_gain_adj + RF_CDMA_MDSP_TX_CLOSED_LOOP_OFFSET) & RF_CDMA_MDSP_TX_CLOSED_LOOP_MASK);    
    #else
    tx_gain_adj = (int16)HWIO_IN( TX_GAIN_ADJ_RD );
    #endif
    /* Bit 8 is sign bit...if it is set we need to sign extend */
    return (int16)(( tx_gain_adj > 255 ) ? tx_gain_adj | 0xfe00 : tx_gain_adj);
}

#endif /* !FEATURE_GSM_PLT */


#ifdef FEATURE_SUBPCG_PA_WARMUP_DELAY
/*===========================================================================

FUNCTION RF_GET_PA_WARMUP                               EXTERNALIZED FUNCTION

DESCRIPTION    This function returns the new calculated PA WARMUP value
               based on the NV item in chips units.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/

word rf_get_pa_warmup (void)
{
   return (pa_warmup_ms);
}

#ifdef RF_HAS_DIFF_PA_TX_WARMUP
/*===========================================================================

FUNCTION RF_GET_TX_WARMUP                               EXTERNALIZED FUNCTION

DESCRIPTION    This function returns the new calculated TX WARMUP value
               based on the NV item in chips units.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
word rf_get_tx_warmup (void)
{
   return (tx_warmup_ms);
}
#endif
#endif /* FEATURE_SUBPCG_PA_WARMUP_DELAY */

#ifndef FEATURE_GSM_PLT

/*===========================================================================

FUNCTION RF_INIT_TX_CAGC                                   EXTERNAL FUNCTION

DESCRIPTION
  This function initializes the TX CAGC controls.

DEPENDENCIES
  rf_nv_dig_items_ptr must have been initialized but it is not required
  that RF NV has been read and the structures populated.  This function
  should only access the default values in the rf_nv_dig_items struct.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rf_init_tx_cagc(rfcom_device_enum_type device)
{

  /* Set TX_ON to inactive */
  MSM_W_MASKED( RXF_MODEM(PA_ON_CTL), TX_ON_EN, CAGC_RF_TX_ON_ENA_OFF_V);

#ifdef RF_HAS_MSM7600
#error code not present
#endif

  /* Set MASK_DELAY to 1 to get a delay of 2 pcg's for valid Reverse Power
     Control bits (for RC1 and RC2) as specified by the standard. */

  /*  disable PA state machine override mode in modulator section. This is a new register */
  HWIO_OUTM( MOD_TEST_CTL, HWIO_FMSK(MOD_TEST_CTL,PA_STATE_TEST_EN), MOD_TEST_PA_DIS_VAL ) ;

  #ifdef RF_HAS_MDSP_TX_AGC
  MSM_W_MASKED( TX_AGC_CTL, PA_R_OVERRIDE_N, CAGC_RF_PAR_CTL_OVERRIDE_AGC_V);
  MSM_W_MASKED( TX_AGC_CTL, PA_R_WR, CAGC_RF_PAR_00_V ); 
  CDMA_MDSP_SET_PA_R0_R1_OVERRIDE_VAL(CAGC_RF_PAR_00_V);
  CDMA_MDSP_ENABLE_PA_RANGE_OVERRIDE();
  #else
  /*  Disable PA state machine override, Integrator bypass*/
  MSM_W_MASKED(AGC_TEST_CTL, PA_STATE_WR, 0);
  MSM_W_MASKED(AGC_TEST_CTL, PA_STATE_OVERRIDE, 0);
  #endif

  #ifdef FEATURE_IS2000_CAGC
    /* Disable MSM3100 backward compatible mode and enable TX_RATE_ADJ for sampling */
    MSM_W_MASKED(TX_AGC_CTL, MASK_DELAY, CAGC_RF_MASK_DELAY_2PCG_V);
    #ifdef RF_HAS_MDSP_TX_AGC
    CDMA_MDSP_SET_TX_MASK_DATA_DELAY( CAGC_RF_MASK_DELAY_2PCG_V + 1 );
    #endif
    #ifndef RF_HAS_MDSP_TX_AGC
    MSM_W_MASKED(TX_AGC_CTL2, TX_MSM3100_MODE, CAGC_RF_TX_MSM3100_DIS_V);
    #endif
    #ifdef RF_HAS_TX_AGC_RATE_ADJ_READY_BIT
    #ifndef RF_HAS_MDSP_TX_AGC
      MSM_W_MASKED(TX_AGC_CTL2, RATE_ADJ_READY, CAGC_RF_RATE_ADJ_READY_YES_V);
    #endif
    #endif

  #else
    /* Enable MSM3100 backward compatibel mode */
    HWIO_OUTM (AGC_CTL8,
                              HWIO_FMSK(AGC_CTL8,MASK_DELAY)
                            | CAGC_TX_MSM3100_MODE_M
                            | HWIO_FMSK(AGC_CTL8,RATE_ADJ_READY),
                                                     CAGC_MASK_DELAY_2PCG_V
                                                   | CAGC_TX_MSM3100_EN_V
                                                   | CAGC_RATE_ADJ_READY_NO_V);
  #endif


  /*
  ** Initialize AGC_TX_RCCCH_FRAME_DELAY
  */
  MSM_W_MASKED(AGC_TX_MASK_DATA_SEL, DATA, CAGC_RF_TX_MASK_DATA_SEL_NORM_VAL);

  MSM_W_MASKED(AGC_TX_RCCCH_FRAME_DELAY, DATA, CAGC_RF_TX_RCCH_FRAME_DELAY_0PCGS);

  /* ----------------------------------------------------------------
  ** Configure TX AGC ADJ PDM
  ** ---------------------------------------------------------------- */
#ifndef T_IO_CARD_PANDA
 #ifdef RF_HAS_MODULATOR_TX_CTL
  MSM_W_MASKED(MOD_TX_AGC_ADJ_CTL, TX_AGC_ADJ_POLARITY, CAGC_RF_TX_AGC_POL_INV_V);
 #else
  MSM_W_MASKED(TX_AGC_CTL, TX_AGC_ADJ_POLARITY, CAGC_RF_TX_AGC_POL_INV_V);
 #endif
#endif /* T_IO_CARD_PANDA */

  /* Initialize the overdrive circuit. */
#ifdef RF_HAS_MODULATOR_TX_CTL
  HWIO_OUT(MOD_TX_AGC_ODRIVE_DLY, rft_get_tx_agc_odrive_delay());
#else
  HWIO_OUT(TX_PDM_DELAY_VAL, rft_get_tx_agc_odrive_delay());
#endif

#ifdef RF_HAS_MDSP_TX_AGC
CDMA_MDSP_SET_COMBINER_FREQUENCY(RF_CDMA_MDSP_COMBINER_FREQ_VAL);
#endif

  /* ----------------------------------------------------
  ** Initialize TX Linearizer
  ** ---------------------------------------------------- */
  #ifdef FEATURE_4PAGE_TX_LINEARIZER
    rf_init_pa_ctl_4_txlinz( device );
  #else
    rf_init_pa_ctl_1_txlinz();
  #endif

 #ifdef RF_HAS_MDSP_TX_AGC
 if( device ==  RFCOM_TRANSCEIVER_0 )
 {
   rf_cdma_mdsp_update_tx_lin_tables();
 }
 #endif

  /*------------------------------------------------------
  ** Start in the non-override mode
  **------------------------------------------------------*/
  #ifdef RF_HAS_MDSP_TX_AGC
  CDMA_MDSP_SET_TX_OPEN_LOOP_NORMAL();
  #endif

  MSM_W_MASKED(TX_AGC_CTL2, TX_OPEN_LOOP_OVERRIDE, CAGC_RF_TX_OL_NRM_V);

  #ifndef RF_HAS_MDSP_TX_AGC
  MSM_W_MASKED(TX_AGC_CTL2, FREEZE_PA_STATE, CAGC_RF_UNFREEZE_PA_STATE_V);
  #endif
}


/*===========================================================================

FUNCTION RF_INIT_TX_CAGC_DELAYED                         EXTERNAL FUNCTION

DESCRIPTION
  This function initializes the TX CAGC controls that require NV items. It
  should not be called until NV has been read

DEPENDENCIES
  rf_nv_dig_items_ptr must have been initialized and RF NV has been read
  and the structures populated.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rf_init_tx_cagc_delayed(void)
{

  #ifdef FEATURE_SUBPCG_PA_WARMUP_DELAY
    word local_rfnv_subpcg_pa_warmup_delay;
  #endif

  #ifdef RF_HAS_DIFF_PA_TX_WARMUP
    word local_rfnv_subpcg_tx_warmup_delay;
  #endif
  #ifdef RF_HAS_MDSP_TX_AGC
  int2 tx_min_pwr = 0x1ff;
  #endif

  #ifdef RF_HAS_SUB_BLOCK_SUPPORT
  rfnv_digital_items_type* nv_digital_items_ptr;
  int block_count;
  #endif

  /*
  ** Initialize TX_ATTEN_LIMIT register
  */
  #ifdef RF_HAS_MDSP_TX_AGC
  tx_min_pwr = (int2)RF_CDMA_MDSP_10_TO_16_BIT( tx_min_pwr );
  CDMA_MDSP_TX_CAGC_DOWN_RATCHET_MODE_NORMAL();     
  /* update the TX Min power limit */
  CDMA_MDSP_SET_TX_MIN_PWR(tx_min_pwr);    
  CDMA_MDSP_NEW_TX_LIMIT_SLOT_PARAMS_AVAILABLE();
  #else
  HWIO_OUTM(TX_ATTEN_LIMIT_WR, 0x7ff, 0x1ff );
  #endif
  /*
  HWIO_OUTM(TX_ATTEN_LIMIT_WR,
                                  HWIO_FMSK(TX_ATTEN_LIMIT_WR,ATTEN_RATCHET_MODE)
                                | HWIO_FMSK(TX_ATTEN_LIMIT_WR,TX_ATTEN_LIMIT),
                                                             CAGC_CPU_FORCE_ATTEN_RATCHET_MODE
                                                           | rf_nv_dig_items_ptr->rfnv_cdma_tx_atten_limit);
  */
  #ifdef FEATURE_SUBPCG_PA_WARMUP_DELAY
    /* PA warm-up is in an NV item, check for valid limits and calculate actual
       delay in msec to be loaded in the register */

    local_rfnv_subpcg_pa_warmup_delay = CAGC_SUBPCG_PA_WARMUP_DELAY_V ;
#ifdef RF_HAS_DIFF_PA_TX_WARMUP
    local_rfnv_subpcg_tx_warmup_delay = CAGC_DEFAULT_TX_WARMUP_DELAY ;
#endif

    /* check the lower limit of the nv item */
    if (local_rfnv_subpcg_pa_warmup_delay < MIN_PA_WARMUP_DELAY) /*lint !e774 */
    {
      MSG_LOW( "subpcg_pa_warmup_delay bad value %d (< %d )",
              local_rfnv_subpcg_pa_warmup_delay, MIN_PA_WARMUP_DELAY,0 ) ;
      local_rfnv_subpcg_pa_warmup_delay = MIN_PA_WARMUP_DELAY ;
    }

    /* check the upper limit of the nv item */
    if (local_rfnv_subpcg_pa_warmup_delay > DEFAULT_PA_WARMUP_DELAY)
    {
      MSG_LOW( "subpcg_pa_warmup_delay bad value %d ( > %d)",
             local_rfnv_subpcg_pa_warmup_delay, DEFAULT_PA_WARMUP_DELAY,0 ) ;
      local_rfnv_subpcg_pa_warmup_delay = DEFAULT_PA_WARMUP_DELAY ;
    }


#ifdef RF_HAS_DIFF_PA_TX_WARMUP 
    /* check the lower limit of the nv item */
    if (local_rfnv_subpcg_tx_warmup_delay < MIN_TX_WARMUP_DELAY)/*lint !e774 */
    {
      MSG_LOW( "subpcg_tx_warmup_delay bad value %d (< %d )",
              local_rfnv_subpcg_tx_warmup_delay, MIN_TX_WARMUP_DELAY,0 ) ;
      local_rfnv_subpcg_tx_warmup_delay = MIN_TX_WARMUP_DELAY ;
    }

    /* check the upper limit of the nv item */
    if (local_rfnv_subpcg_tx_warmup_delay > DEFAULT_TX_WARMUP_DELAY)
    {
      MSG_LOW( "subpcg_tx_warmup_delay bad value %d ( > %d)",
             local_rfnv_subpcg_tx_warmup_delay, CAGC_DEFAULT_TX_WARMUP_DELAY,0 ) ;
      local_rfnv_subpcg_tx_warmup_delay = DEFAULT_TX_WARMUP_DELAY ;
    }
#endif
    /* PA_WARMUP =  153.6 * WARMUP_DELAY + 64
    ** WARMUP_DELAY (in ms) = NV_SUBPCG_PA_WARMUP_DELAY (in chips)/1228.8.
    ** new PA_WARMUP = 153.6 * NV_SUBPCG_PA_WARMUP_DELAY/1228.8 + 64
    */
#ifndef RF_HAS_PA_AND_TX_WARMUP_TIMES_DIFF_FROM_LEGACY
    pa_warmup_ms = (word)(((local_rfnv_subpcg_pa_warmup_delay * 1536) / 12288) + 64);
#ifdef RF_HAS_DIFF_PA_TX_WARMUP
    tx_warmup_ms = (word)(((local_rfnv_subpcg_tx_warmup_delay * 1536) / 12288) + 64);
#endif
    HWIO_OUT(PA_WARMUP, pa_warmup_ms);

    #ifdef RF_HAS_TX_WARMUP
#ifdef RF_HAS_DIFF_PA_TX_WARMUP
      HWIO_OUT(TX_WARMUP, tx_warmup_ms);
#else
      /*  TX_WARMUP =  153.6 * WARMUP_DELAY + 64
      **  Use same value as for PA_WARMUP
      */
      HWIO_OUT(TX_WARMUP, pa_warmup_ms);
#endif  /* RF_HAS_DIFF_PA_TX_WARMUP */
    #endif  /* RF_HAS_TX_WARMUP */
#else
  tx_warmup_ms = rft_get_tx_warmup_timer_val();

  pa_warmup_ms = rft_get_pa_warmup_timer_val();

  HWIO_OUT(TX_WARMUP, tx_warmup_ms);

  HWIO_OUT(PA_WARMUP, pa_warmup_ms);
#endif /*RF_HAS_PA_AND_TX_WARMUP_TIMES_DIFF_FROM_LEGACY*/
  #endif  /*  FEATURE_SUBPCG_PA_WARMUP_DELAY */

  #ifdef FEATURE_IS2000_CAGC
    /* initialize the TX_2_early_PCG_EN signal  */
    CAGC_INIT_TX_2_EARLY_PCG();

    /* use 100 chips to advance the very early frame from the frame boundary*/
    MSM_W_MASKED( TX_VERY_EARLY_FRAME_CTL, VERY_EARLY_FRAME_ADV, CAGC_RF_TX_VERY_EARLY_FRAME_ADV_100CHIPS);
    MSM_W_MASKED( TX_VERY_EARLY_FRAME_CTL, VERY_EARLY_FRAME_PER, CAGC_RF_VERY_EARLY_FRAME_20MS);

  #endif /* FEATURE_IS2000_CAGC */
  

#if !defined(RF_HAS_HDET_BYPASS)

  /* Generate the TX AGC versus HDET tables for all supported bands */

  {
    rf_bc_and_mask_type valid_band;

    uint8 BCInx = 0;

    sys_band_mask_type band_mask = rfnv_get_band_mask();    

    /* Loop through for each band defined in the table and is enabled in the NV Band Mask */
    while(rf_get_valid_bc_and_mask(BCInx,&valid_band) == TRUE)
    {
      /* If the band class is equal to all 1s or the band mask is
         equal to 0 then the band class/mask value is invalid. 
         We should had exit the while loop. */
      ASSERT((valid_band.bc != (rf_card_band_type)(-1)) && \
             (valid_band.mask != 0));

      if( (valid_band.mask & band_mask ) != 0 )
      {
        #ifdef RF_HAS_SUB_BLOCK_SUPPORT
        nv_digital_items_ptr = rfnv_get_band_nv_ptr(valid_band.bc);
        rf_exp_agc_table_ptr = nv_digital_items_ptr->rfnv_cdma_exp_agc_table;
        rf_load_cdma_exp_agc_table(nv_digital_items_ptr);
        #else
        rf_exp_agc_table_ptr = rf_digital_nv_table[valid_band.bc]->rfnv_cdma_exp_agc_table;
        rf_load_cdma_exp_agc_table(  rf_digital_nv_table[valid_band.bc] );
        #endif
      }

      BCInx++;

    }

  }

  #ifdef RF_HAS_SUB_BLOCK_SUPPORT
  for(block_count=0; block_count<RF_BAND_MAX_BLOCKS; block_count++)
  {
    nv_digital_items_ptr = rfnv_get_block_nv_ptr(block_count);
    if(nv_digital_items_ptr != NULL)
    {
      rf_exp_agc_table_ptr = nv_digital_items_ptr->rfnv_cdma_exp_agc_table;
      rf_load_cdma_exp_agc_table(nv_digital_items_ptr);
    }
  }
  #endif

#endif /* !RF_HAS_HDET_BYPASS */

}



/*____________________________________________________________________________
                       RX CAGC Functions
____________________________________________________________________________*/

/*===========================================================================

FUNCTION RF_SET_102dB_AGC_VALUE_LIMITS                               INTERNAL

DESCRIPTION
  This function calculates the saturation points for the 10 bit AGC
  registers when 102 dB dynamic range values have to be rescaled to 85 dB
  dynamic range values.  This function should not be called until after
  rf_init_cagc_linearizer_range() has been called.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rf_set_102dB_agc_value_limits( void )
{
   /*  Calculate the RX AGC value that corresponds to MAX_RX_PWR_85_DB
   **  when using 102 dB dynamic range
   */
   rx_102_dB_agc_max_val = (int2)(( RF_RX_AGC_MAX*100 - ( max_rx_pwr_102_db - MAX_RX_PWR_85_DB )
                            * RX_AGC_VAL_CNTS_PER_DB_102_DB )/100);

   /*  Calculate the RX AGC value that corresponds to MIN_RX_PWR_85_DB
   **  when using 102 dB dynamic range
   */
   rx_102_dB_agc_min_val =  (int2)(( MIN_RX_PWR_85_DB - min_rx_pwr_102_db )
                            * RX_AGC_VAL_CNTS_PER_DB_102_DB/100);

   /*  Calculate the TX AGC value that corresponds to MIN_TX_PWR_85_DB
   **  when using 102 dB dynamic range
   */
   tx_102_dB_agc_max_val = (int2)(( RF_TX_AGC_MAX*100 - ( MIN_TX_PWR_85_DB - min_tx_pwr_102_db )
                            * TX_AGC_VAL_CNTS_PER_DB_102_DB )/100);

   /*  Calculate the TX AGC value that corresponds to MAX_TX_PWR_85_DB
   **  when using 102 dB dynamic range
   */
   tx_102_dB_agc_min_val =  (int2)(( max_tx_pwr_102_db - MAX_TX_PWR_85_DB )
                            * TX_AGC_VAL_CNTS_PER_DB_102_DB/100);

   /*  Calculate values needed by rf_rescale_tx_pwr_limit()
   **  when using 102 dB dynamic range. 0->1023 represent min->max power
   **  for rf_tx_pwr_limit but tx_102_dB_agc_min_val and
   **  tx_102_dB_agc_max_val (TX AGC) were calculated to represent max->min
   **  for 0->1023 so we need to "flip" them
   */
   tx_102_dB_pwr_limit_max_val =  (int2)(RF_TX_AGC_MAX - tx_102_dB_agc_min_val);

   tx_102_dB_pwr_limit_min_val =  (int2)(RF_TX_AGC_MAX - tx_102_dB_agc_max_val);

}



/*===========================================================================

FUNCTION RF_GET_CDMA_RX_AGC                             EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns the CDMA Rx AGC setting in 8 bit two's
  complement format (-128...127).

  Rx signal strength is:

  Rx strength in dBm = -63.248 + (( rf_get_cdma_rx_agc * 334 ) / 1000 )

  That's -0.334dB per count referenced to -63.248dBm.  Since this returns
  int1, this is a numeric Rx signal strength from -106dBm to -20.83dBm.

  When 102 dB dynamic range is used the AGC value is scaled into the
  85 dB range.  AGC values for powers greater than -21 dBm are saturated
  to -21 dBm (127).  AGC values for powers less than -106 dBm are saturated
  to -106 dBm (-128).

DEPENDENCIES
  None

RETURN VALUE
  int1 = 8 bit two's complement CDMA Rx AGC value

SIDE EFFECTS
  None

===========================================================================*/
int1 rf_get_cdma_rx_agc( void )
{
  return rf_get_path_cdma_rx_agc(RFCOM_TRANSCEIVER_0);

} /* rf_get_cdma_rx_agc */
/*===========================================================================

FUNCTION RF_GET_PATH_CDMA_RX_AGC                             EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns the CDMA Rx AGC setting in 8 bit two's
  complement format (-128...127).

  Rx signal strength is:

  Rx strength in dBm = -63.248 + (( rf_get_cdma_rx_agc * 334 ) / 1000 )

  That's -0.334dB per count referenced to -63.248dBm.  Since this returns
  int1, this is a numeric Rx signal strength from -106dBm to -20.83dBm.

  When 102 dB dynamic range is used the AGC value is scaled into the
  85 dB range.  AGC values for powers greater than -21 dBm are saturated
  to -21 dBm (127).  AGC values for powers less than -106 dBm are saturated
  to -106 dBm (-128).

DEPENDENCIES
  None

RETURN VALUE
  int1 = 8 bit two's complement CDMA Rx AGC value

SIDE EFFECTS
  None

===========================================================================*/
int1 rf_get_path_cdma_rx_agc( rfcom_device_enum_type device )
{
  int2 rx_agc;

  /* Get RxAGC value */
  rx_agc = rf_get_path_1x_agc(device);

  if ( cagc_dynamic_range_setting == RF_HAS_102DB_CDMA_DYNAMIC_RANGE )
  {
    /* Scale down to 85 dBm range if necessary.
    */

    /* convert to 0..1023 range */
    rx_agc =  ( rx_agc + RF_RX_AGC_VAL_OFFSET ) & RX_AGC_VAL_MASK;

    /* saturate to agc values corresponding to min/max for 85 dBm */
    rx_agc = RF_SATURATE( rx_agc, rx_102_dB_agc_min_val, rx_102_dB_agc_max_val );

    /* scale value and return as -512...511*/
    rx_agc = ( ( (rx_agc - rx_102_dB_agc_min_val) * RX_AGC_VAL_CNTS_PER_DB_85_DB / RX_AGC_VAL_CNTS_PER_DB_102_DB )
               + RF_RX_AGC_VAL_OFFSET ) & RX_AGC_VAL_MASK;
  }

  /* return only 8 MSBS of value read , MSB is sign bit */
  return ( (int1)( rx_agc >> AGC_VALUE_LSBS_TO_TRUNCATE) );/*lint !e702 */

} /* rf_get_path_cdma_rx_agc */
/* EJECT  */
/*===========================================================================

FUNCTION RF_GET_PATH_CDMA_RX_AGC_IN_DBM                  EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns the CDMA Rx RSSI in dBm for a given Rx path.  
  For 1x the input agc_val_to_convert must be the full scale (10 bit) Rx AGC value.

DEPENDENCIES
  None

RETURN VALUE
  Signed Rx power in dBm

SIDE EFFECTS
  None

===========================================================================*/

int16 rf_get_path_cdma_rx_agc_in_dBm( rfcom_device_enum_type device )
{
  rf_cagc_vals_vars_type agc_val;
  int16 rx_pwr;

#ifdef RF_HAS_HDR
#error code not present
#endif

  /* get required info to do calculation */
   rf_get_path_cagc_val( device, RX_AGC_VAL, &agc_val );

  /* calculate Rx power in 1/100 dB resolution.  There is a 1/10 
  ** (102.4 dB dynamic range) or 1/12 dB (85.3 dB dynamic range) rounding error.
  */
  rx_pwr = (int16)(agc_val.val_min + 
       ((int4)(agc_val.polarity * agc_val.scale_factor_numer * agc_val.val_index * 100 ))/agc_val.scale_factor_denom);

  /* return in 1/10 db resolution */
  return (int16) (rx_pwr/10);

}
/*===========================================================================

FUNCTION RF_GET_CDMA_TX_AGC_IN_DBM                     EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns the CDMA TX AGC in dBm.  Resolution is ~1/10 dB

DEPENDENCIES
  None

RETURN VALUE
  Signed Rx power in dBm

SIDE EFFECTS
  None

===========================================================================*/
int16 rf_get_cdma_tx_agc_in_dBm( void )
{
  rf_cagc_vals_vars_type agc_val;
  
  int16 tx_pwr;

#ifdef RF_HAS_HDR
#error code not present
#endif

   /* get required info to do calculation */
   rf_get_path_cagc_val( RFCOM_TRANSCEIVER_0, TX_AGC_VAL, &agc_val );/*lint !e539 */

   /* calculate Tx power in 1/100 dB resolution.  There is a 1/10 
   ** (102.4 dB dynamic range) or 1/12 dB (85.3 dB dynamic range) rounding error.
   */
   tx_pwr = (int16)(agc_val.val_min + 
     ((int4)(agc_val.polarity * agc_val.scale_factor_numer * agc_val.val_index * 100 ))/agc_val.scale_factor_denom);

    /* if we are in PCS band subtract 3 dB due to offset power difference, except for BC5 */
    if ( rf_chain_status[0].rf_mode == DB_RF_PCN ) 
    {
       if (rf_chain_status[0].rf_curr_cdma_band != RF_BC5_BAND)
       {
          tx_pwr -=300;
       }
    }

    /* return in 1/10 db resolution */
    return (int16) (tx_pwr/10);

}
/*===========================================================================

FUNCTION RF_SET_AGC_DC_LOOP_GAIN                            EXTERNAL FUNCTION

DESCRIPTION
  This function modifies the AGC_DC_GAIN register according to the specified
  input action:
  FREEZE_AGC_DC_LOOP_GAIN - causes 0s to be sent to the AGC Loop integrator
                            i.e., rssi values ignored

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rf_set_agc_dc_loop_gain(rf_path_enum_type path, agc_dc_loop_gain_type action)
{

  if ( action == FREEZE_AGC_DC_LOOP_GAIN ) 
  {
    MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_DC_GAIN), AGC_LOOP_FREEZE_EN, CAGC_RF_CPU_FREEZE_AGC_LOOP_VAL, (int)path );
    MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_DC_GAIN), AGC_DC_GAIN_EN, CAGC_RF_DC_GAIN_EN_ENABLE_VAL, (int)path );
    MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_DC_GAIN), AGC_DC_GAIN_VAL, CAGC_RF_DC_GAIN_MAX_VALUE, (int)path );
    MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_DC_GAIN), AGC_VALUE_MIN_EN, CAGC_RF_AGC_VALUE_MIN_EN_VAL, (int)path );
  }
  else 
  {
    ERR("Invalid agc dc loop action",0,0,0);
  }
}

void rxf_agc_additional_inits(rf_path_enum_type path)
{

  HWIO_OUTMI(RXF_CDMA(RX_AGCc_LNA_DATA), (int)path, HWIO_RMSK(RXF_CDMA(RX_AGCc_LNA_DATA)), CAGC_RF_LNA_DATA_V);

  HWIO_OUTMI(RXF_CDMA(RX_AGCc_VALUE_MAX), (int)path, HWIO_RMSK(RXF_CDMA(RX_AGCc_VALUE_MAX)), CAGC_RF_AGC_VAL_MAX_V);

  /* this is different for RFR612x */

  HWIO_OUTMI(RXF_CDMA(RX_AGCc_VREF_DELAY_TIMER), (int)path, HWIO_RMSK(RXF_CDMA(RX_AGCc_VREF_DELAY_TIMER)), CAGC_RF_VREF_DELAY_TIMER_V);

  HWIO_OUTMI(RXF_CDMA(RX_AGCc_VREF_VAL), (int)path, HWIO_RMSK(RXF_CDMA(RX_AGCc_VREF_VAL)), CAGC_RF_VREF_V);

  HWIO_OUTMI(RXF_CDMA(RX_AGCc_TEST_CTL), (int)path, HWIO_RMSK(RXF_CDMA(RX_AGCc_TEST_CTL)), CAGC_RF_TEST_CTL_V);

  /* loop gains for Fast LNA controller AGC mode for EV-DO  */
  HWIO_OUTMI(RXF_CDMA(RX_AGCc_LGLUT_LVAL), (int)path, HWIO_RMSK(RXF_CDMA(RX_AGCc_LGLUT_LVAL)), CAGC_RF_LGLUT_LVAL);

  HWIO_OUTMI(RXF_CDMA(RX_AGCc_LGLUT_HVAL), (int)path, HWIO_RMSK(RXF_CDMA(RX_AGCc_LGLUT_HVAL)), CAGC_RF_LGLUT_HVAL);

}

/*===================================================================+========
                                                                     EXTERNAL
FUNCTION RF_INIT_RX_CAGC()

DESCRIPTION
  This function performs the one time initialization of the RX CAGC registers
  that is called as part of the RF initialization sequence.

DEPENDENCIES
  Any code in this function should not have dependency on NV items because
  this function is meant be called before RF NV is available.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rf_init_rx_cagc( rfcom_device_enum_type device )
{

    rf_path_enum_type path = rfcom_to_path[ device ];

    if (path >= RF_PATH_MAX)
    {
       MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
       rf_error_flag = TRUE;
       return;
    }

    /* Enable RXF clocks for RxAGC block */
#ifndef RF_HAS_RXF_V2
#error code not present
#else /* RF_HAS_RXF_V2 */
    rxf_enable_rx_front_clocks_sc2x(path);
#endif /* !RF_HAS_RXF_V2 */

    /* Initialize LNA call back */
    clk_def( &rf_ena_hwlna_cbk );

    /* CAGC Gain Constants */
    MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_GAIN_CTL), GAIN_CONSTANT_IM, CAGC_RF_TCIM_0073_US, (int)path);
    MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_GAIN_CTL), GAIN_CONSTANT, CAGC_RF_TC_0392_US, (int)path);

#ifndef T_IO_CARD_PANDA
    /* -------------------------------------------------------
    ** Initialize AGC_DC_GAIN register
    ** Enable freeze on DC Acquisition
    ** Enable DC gain and set DC GAIN value to max value
    **------------------------------------------------------*/
    MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_DC_GAIN), AGC_LOOP_FREEZE_EN, CAGC_RF_CPU_FREEZE_AGC_LOOP_VAL, (int)path);
    MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_DC_GAIN), AGC_DC_GAIN_EN, CAGC_RF_DC_GAIN_EN_ENABLE_VAL, (int)path);
    MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_DC_GAIN), AGC_DC_GAIN_VAL, CAGC_RF_DC_GAIN_MAX_VALUE, (int)path);
    MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_DC_GAIN), AGC_VALUE_MIN_EN, CAGC_RF_AGC_VALUE_MIN_EN_VAL, (int)path);
#endif /* T_IO_CARD_PANDA */

    /*  ----------------------------------------------------------
    ** Initialize AGC limits
    ** ---------------------------------------------------------- */

    /*------------------------------------------------------
    ** Configure the LNA Controls
    **------------------------------------------------------*/
    rf_init_hw_lna_ctl(device);

    /*------------------------------------------------------
    ** Start in non-override mode
    **------------------------------------------------------*/
    #ifdef RF_HAS_MDSP_TX_AGC
    CDMA_MDSP_SET_TX_OPEN_LOOP_NORMAL();
    #endif

    MSM_W_MASKED(TX_AGC_CTL2, TX_OPEN_LOOP_OVERRIDE, CAGC_RF_TX_OL_NRM_V);

    rxf_agc_additional_inits( path );
}


/*===========================================================================

FUNCTION RF_INIT_RX_CAGC_DELAYED                         EXTERNAL FUNCTION

DESCRIPTION
  This function initializes the TX CAGC controls that require NV items. It
  should not be called until NV has been read

DEPENDENCIES
  rf_nv_dig_items_ptr must have been initialized and RF NV has been read
  and the structures populated.  A call to the rf_set_cagc_linearizer_range()
  must have been made to determine the CAGC dynamic range setting.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rf_init_rx_cagc_delayed( void )
{

    /* set the CDMA CAGC linearizer range */
    rf_set_cagc_linearizer_range( );

    /* determine AGC value limits for rescaling from 102 db to
    ** 85 dbm range
    */
    if ( cagc_dynamic_range_setting == RF_HAS_102DB_CDMA_DYNAMIC_RANGE ) {
      rf_set_102dB_agc_value_limits();
    }


}


/*============================================================================

FUNCTION RF_INIT_HW_LNA_CTL                                           EXTERNAL

DESCRIPTION
  This function will initialize and configure LNA bypass control.
  Note that NV items are not yet available.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void rf_init_hw_lna_ctl( rfcom_device_enum_type device )
{

    rf_path_enum_type path = rfcom_to_path[ device ];

    if (path >= RF_PATH_MAX)
    {
       MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
       rf_error_flag = TRUE;
       return;
    }

    /* set AGC to CDMA mode */
    rf_set_rx_agc_mode(path, DB_RF_CDMA, 0x0 );

    /* set LNA Control to internal (HW) */
    MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_LNA_CTL), LNA_RANGE_OVERRIDE_N, CAGC_RF_LNA_RG_OVRD_ENA_V, (int)path);

    /* clear LNA_RANGE, LNA_GAIN, LNA_FILT controls */
    MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_LNA_CTL), LNA_FILT_OVERRIDE, CAGC_RF_LNA_FILT_NRM_V, (int)path);
    MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_LNA_CTL), LNA_RANGE_FILT_SEL, CAGC_RF_LNA_RG_FILT_DIS_V, (int)path);


    /* LNA_GAIN, LNA_LIN, LNA_FILT not used */
    /* LIN_SEL was removed */
    MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_LNA_CTL), LNA_RANGE_FILT_SEL, CAGC_RF_LNA_FILT_DIS_V, (int)path);

    MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_LNA_RANGE_DELAY), DATA, CAGC_RF_LNA_RANGE_DELAY_85CHIPX2, (int)path);

    return;


}



/*============================================================================

FUNCTION RF_HW_LNA_SETUP_DIGITAL                                     EXTERNAL

DESCRIPTION
  This function will configure LNA to operate in digital mode after the phone
  wakes up from sleep. NV must have been read and rf_nv_dig_items_ptr should be
  pointing to the corrrect structure

DEPENDENCIES
  rf_init_hw_lna_ctl must have been called once.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void rf_hw_lna_setup_digital( rfcom_device_enum_type device)
{
    rf_path_enum_type path = rfcom_to_path[ device ];
    rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path];

    if (path >= RF_PATH_MAX)
    {
      MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
      rf_error_flag = TRUE;
      return;
    }

    /* set rx agc mode to CDMA */
    rf_set_rx_agc_mode( path, DB_RF_CDMA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_mixer_range_pol );

    /* load the LNA state machine switching controls */
    rf_load_lna_switching_ctls(device);

    /* load the IM Level controls */
    rf_load_im_level_ctls(device);

    /* load AGC Loop Gain Constants */
    MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_GAIN_CTL), GAIN_CONSTANT_IM, CAGC_RF_TCIM_0073_US, (int)path);
    MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_GAIN_CTL), GAIN_CONSTANT, CAGC_RF_TC_0392_US, (int)path);

#ifndef T_IO_CARD_PANDA
    /* Freeze AGC Loop during DC acqusition */
    MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_DC_GAIN), AGC_LOOP_FREEZE_EN, CAGC_RF_CPU_FREEZE_AGC_LOOP_VAL, (int)path);
    MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_DC_GAIN), AGC_DC_GAIN_EN, CAGC_RF_DC_GAIN_EN_ENABLE_VAL, (int)path);
    MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_DC_GAIN), AGC_DC_GAIN_VAL, CAGC_RF_DC_GAIN_MAX_VALUE, (int)path );
    MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_DC_GAIN), AGC_VALUE_MIN_EN, CAGC_RF_AGC_VALUE_MIN_EN_VAL, (int)path );
#endif /* T_IO_CARD_PANDA */

    /* enable HW LNA control */
    MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_LNA_CTL), LNA_RANGE_OVERRIDE_N, CAGC_RF_LNA_RG_OVRD_ENA_V, (int)path);
} /* rf_hw_lna_setup_digital */


/*============================================================================

FUNCTION RF_LOAD_LNA_SWITCHING_CTLS                                   EXTERNAL

DESCRIPTION
  This function will configure the CAGC registers that control the switching
  behavior of the HW LNA State Machine.
  Note: This will program only non-OLTA switch points since rise and fall
        values are set in NV only for FIXED switch points.

DEPENDENCIES
  rf_init_hw_lna_ctl must have been called once.
  rf_nv_digtial_items_ptr must be initialized and pointing to correct
  digital structure.
  It is expected that rf_update_lna_offsets() will be called sometime following
  this function to program offset, rise, fall and IM values for OLTA switch
  points (if any).

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void rf_load_lna_switching_ctls( rfcom_device_enum_type device )
{
  rf_path_enum_type path = rfcom_to_path[ device ];
  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];

   #ifdef RF_HAS_RFCONFIG_RX_LNA
   const rfconfig_rx_lna_linearity_info_type  *lna_linearity_info_ptr = rf_get_current_rfconfig_rx_lna_linearity_info(device);
   const rfconfig_rx_lna_switch_pt_type       *lna_switch_pt_ptr = NULL;

   if(path >= RF_PATH_MAX)
   {
     MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
     rf_error_flag = TRUE;
     return;
   }

   if ( rf_chain_ptr->rf_curr_cdma_band == RF_GPS_BAND )
   {
      /* GPS band does not use LNA and AGC state machine */
      return;
   }

   RF_ASSERT(NULL != lna_linearity_info_ptr);
   
   if(lna_linearity_info_ptr == NULL)
   {
       MSG_FATAL("lna_linearity_info_ptr is NULL",0,0,0);
       rf_error_flag = TRUE;
       return;
   }

   RF_ASSERT(lna_linearity_info_ptr->num_gain_steps == 4); // for configuring RISE/FALL levels for 4 LNA gain steps below

   /* load RISE/FALL and uncompensated offsets for non-OLTA (i.e. FIXED) switch points only */
   lna_switch_pt_ptr = rfconfig_rx_lna_get_switch_point_info_from_linearity(lna_linearity_info_ptr, 0);
   if (RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__FIXED == rfconfig_rx_lna_switch_pt_get_config_mode(lna_switch_pt_ptr))
   {
      MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_RISE), DATA, rfconfig_rx_lna_switch_pt_get_rise(lna_switch_pt_ptr), (int)path, 1); /*lint !e701*/
      MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_FALL), DATA, rfconfig_rx_lna_switch_pt_get_fall(lna_switch_pt_ptr), (int)path, 1); /*lint !e701*/
      MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_OFFSET), DATA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_1_offset[path], (int)path, 1); /*lint !e701*/
      MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_VALUE_n_MIN), DATA, RF_GET_RX_AGC_MIN_VAL(rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_1_offset[path]), (int)path, 1); /*lint !e701*/
   }

   lna_switch_pt_ptr = rfconfig_rx_lna_get_switch_point_info_from_linearity(lna_linearity_info_ptr, 1);
   if (RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__FIXED == rfconfig_rx_lna_switch_pt_get_config_mode(lna_switch_pt_ptr))
   {
      MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_RISE), DATA, rfconfig_rx_lna_switch_pt_get_rise(lna_switch_pt_ptr), (int)path, 2); /*lint !e701*/
      MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_FALL), DATA, rfconfig_rx_lna_switch_pt_get_fall(lna_switch_pt_ptr), (int)path, 2); /*lint !e701*/
      MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_OFFSET), DATA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_2_offset[path], (int)path, 2); /*lint !e701*/
      MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_VALUE_n_MIN), DATA, RF_GET_RX_AGC_MIN_VAL(rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_2_offset[path]), (int)path, 2); /*lint !e701*/
   }

   lna_switch_pt_ptr = rfconfig_rx_lna_get_switch_point_info_from_linearity(lna_linearity_info_ptr, 2);
   if (RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__FIXED == rfconfig_rx_lna_switch_pt_get_config_mode(lna_switch_pt_ptr))
   {
      MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_RISE), DATA, rfconfig_rx_lna_switch_pt_get_rise(lna_switch_pt_ptr), (int)path, 3); /*lint !e701*/
      MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_FALL), DATA, rfconfig_rx_lna_switch_pt_get_fall(lna_switch_pt_ptr), (int)path, 3); /*lint !e701*/
      MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_OFFSET), DATA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_3_offset[path], (int)path, 3); /*lint !e701*/
      MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_VALUE_n_MIN), DATA, RF_GET_RX_AGC_MIN_VAL(rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_3_offset[path]), (int)path, 3); /*lint !e701*/
   }

   lna_switch_pt_ptr = rfconfig_rx_lna_get_switch_point_info_from_linearity(lna_linearity_info_ptr, 3);
   if (RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__FIXED == rfconfig_rx_lna_switch_pt_get_config_mode(lna_switch_pt_ptr))
   {
      MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_RISE), DATA, rfconfig_rx_lna_switch_pt_get_rise(lna_switch_pt_ptr), (int)path, 4); /*lint !e701*/
      MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_FALL), DATA, rfconfig_rx_lna_switch_pt_get_fall(lna_switch_pt_ptr), (int)path, 4); /*lint !e701*/
      MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_OFFSET), DATA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_4_offset[path], (int)path, 4); /*lint !e701*/
      MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_VALUE_n_MIN), DATA, RF_GET_RX_AGC_MIN_VAL(rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_4_offset[path]), (int)path, 4); /*lint !e701*/
   }

   #else /* ! #ifdef RF_HAS_RFCONFIG_RX_LNA */

#ifdef RF_HAS_POWER_MODE_LIB
 rflib_power_mode_type power_mode = rflib_get_power_mode( path );
#endif /* RF_HAS_POWER_MODE_LIB */

  if(power_mode >= RFLIB_MAX_POWER_MODE)
  {
     MSG_FATAL("power mode >= RFLIB_MAX_POWER_MODE", 0, 0, 0);
     rf_error_flag = TRUE;
     return;
  }

  if(path < RF_PATH_MAX)
  {
    /* load RISE/FALL and uncompensated offsets */
#ifdef RF_HAS_POWER_MODE_LIB
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_RISE), DATA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_1_rise[path][power_mode], (int)path,1 ); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_FALL), DATA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_1_fall[path][power_mode], (int)path,1 ); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_OFFSET), DATA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_1_offset[path], (int)path,1 ); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_VALUE_n_MIN), DATA, RF_GET_RX_AGC_MIN_VAL(rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_1_offset[path]), (int)path,1); /*lint !e701*/
    
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_RISE), DATA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_2_rise[path][power_mode], (int)path,2 ); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_FALL), DATA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_2_fall[path][power_mode], (int)path,2 ); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_OFFSET), DATA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_2_offset[path], (int)path,2 ); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_VALUE_n_MIN), DATA, RF_GET_RX_AGC_MIN_VAL(rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_2_offset[path]), (int)path,2); /*lint !e701*/

    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_RISE), DATA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_3_rise[path][power_mode], (int)path,3 ); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_FALL), DATA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_3_fall[path][power_mode], (int)path,3 ); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_OFFSET), DATA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_3_offset[path], (int)path,3 ); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_VALUE_n_MIN), DATA, RF_GET_RX_AGC_MIN_VAL(rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_3_offset[path]), (int)path,3); /*lint !e701*/

    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_RISE), DATA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_4_rise[path][power_mode], (int)path,4 ); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_FALL), DATA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_4_fall[path][power_mode], (int)path,4 ); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_OFFSET), DATA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_4_offset[path], (int)path,4 ); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_VALUE_n_MIN), DATA, RF_GET_RX_AGC_MIN_VAL(rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_4_offset[path]), (int)path,4); /*lint !e701*/
#else
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_RISE), DATA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_1_rise[path], (int)path,1 ); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_FALL), DATA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_1_fall[path], (int)path,1 ); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_OFFSET), DATA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_1_offset[path], (int)path,1 ); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_VALUE_n_MIN), DATA, RF_GET_RX_AGC_MIN_VAL(rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_1_offset[path]), (int)path,1); /*lint !e701*/
  
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_RISE), DATA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_2_rise[path], (int)path,2 ); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_FALL), DATA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_2_fall[path], (int)path,2 ); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_OFFSET), DATA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_2_offset[path], (int)path,2 ); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_VALUE_n_MIN), DATA, RF_GET_RX_AGC_MIN_VAL(rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_2_offset[path]), (int)path,2); /*lint !e701*/

    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_RISE), DATA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_3_rise[path], (int)path,3 ); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_FALL), DATA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_3_fall[path], (int)path,3 ); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_OFFSET), DATA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_3_offset[path], (int)path,3 ); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_VALUE_n_MIN), DATA, RF_GET_RX_AGC_MIN_VAL(rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_3_offset[path]), (int)path,3); /*lint !e701*/

    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_RISE), DATA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_4_rise[path], (int)path,4 ); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_FALL), DATA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_4_fall[path], (int)path,4 ); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_OFFSET), DATA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_4_offset[path], (int)path,4 ); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_VALUE_n_MIN), DATA, RF_GET_RX_AGC_MIN_VAL(rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_4_offset[path]), (int)path,4); /*lint !e701*/
#endif /* RF_HAS_POWER_MODE_LIB */ /*lint !e701*/
  }
  else
  {
     MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
     rf_error_flag = TRUE;
     return;
  }
  #endif /* RF_HAS_RFCONFIG_RX_LNA */
}



/*============================================================================

FUNCTION RF_LOAD_IM_LEVEL_CTLS                                       EXTERNAL

DESCRIPTION
  This function will configure the CAGC registers that control the switching
  behavior of the HW LNA State Machine in the presence of IM
  Note: This will program only non-OLTA switch points since IM values are
        set in NV only for FIXED switch points.

DEPENDENCIES
  rf_init_hw_lna_ctl must have been called once.
  rf_nv_digtial_items_ptr must be initialized and pointing to correct
  digital structure.
  It is expected that rf_update_lna_offsets() will be called sometime following
  this function to program IM values for OLTA switch points (if any).

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void rf_load_im_level_ctls( rfcom_device_enum_type device )
{
  rf_path_enum_type path = rfcom_to_path[ device ];

   #ifdef RF_HAS_RFCONFIG_RX_LNA
   const rfconfig_rx_lna_linearity_info_type  *lna_linearity_info_ptr = rf_get_current_rfconfig_rx_lna_linearity_info(device);
   const rfconfig_rx_lna_switch_pt_type       *lna_switch_pt_ptr = NULL;

   RF_ASSERT(NULL != lna_linearity_info_ptr);

   if(lna_linearity_info_ptr == NULL)
   {
       MSG_FATAL("lna_linearity_info_ptr is NULL",0,0,0);
       rf_error_flag = TRUE;
       return;
   }

   RF_ASSERT(lna_linearity_info_ptr->num_gain_steps == 4); // for configuring IM levels for 4 LNA gain steps below

   lna_switch_pt_ptr = rfconfig_rx_lna_get_switch_point_info_from_linearity(lna_linearity_info_ptr, 0);
   if (RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__FIXED == rfconfig_rx_lna_switch_pt_get_config_mode(lna_switch_pt_ptr))
   {
      /* load IM LEVEL values  */
      MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_IM_LEVELn), DATA, rfconfig_rx_lna_switch_pt_get_im(lna_switch_pt_ptr), (int)path, 1); /*lint !e701*/
   }
   /* load bypass and non bypass timers */
   MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_BP_TIMER_n), DATA, rfconfig_rx_lna_switch_pt_get_bypass_hold_timer(lna_switch_pt_ptr), (int)path, 0); /*lint !e701*/
   MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_NBP_TIMER_n), DATA, rfconfig_rx_lna_switch_pt_get_non_bypass_hold_timer(lna_switch_pt_ptr),(int)path, 0); /*lint !e701*/

   lna_switch_pt_ptr = rfconfig_rx_lna_get_switch_point_info_from_linearity(lna_linearity_info_ptr, 1);
   if (RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__FIXED == rfconfig_rx_lna_switch_pt_get_config_mode(lna_switch_pt_ptr))
   {
      /* load IM LEVEL values  */
      MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_IM_LEVELn), DATA, rfconfig_rx_lna_switch_pt_get_im(lna_switch_pt_ptr), (int)path, 2); /*lint !e701*/
   }
   /* load bypass and non bypass timers */
   MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_BP_TIMER_n), DATA, rfconfig_rx_lna_switch_pt_get_bypass_hold_timer(lna_switch_pt_ptr), (int)path, 1); /*lint !e701*/
   MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_NBP_TIMER_n), DATA, rfconfig_rx_lna_switch_pt_get_non_bypass_hold_timer(lna_switch_pt_ptr),(int)path, 1); /*lint !e701*/

   lna_switch_pt_ptr = rfconfig_rx_lna_get_switch_point_info_from_linearity(lna_linearity_info_ptr, 2);
   if (RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__FIXED == rfconfig_rx_lna_switch_pt_get_config_mode(lna_switch_pt_ptr))
   {
      /* load IM LEVEL values  */
      MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_IM_LEVELn), DATA, rfconfig_rx_lna_switch_pt_get_im(lna_switch_pt_ptr), (int)path, 3); /*lint !e701*/
   }
   /* load bypass and non bypass timers */
   MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_BP_TIMER_n), DATA, rfconfig_rx_lna_switch_pt_get_bypass_hold_timer(lna_switch_pt_ptr), (int)path, 2); /*lint !e701*/
   MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_NBP_TIMER_n), DATA, rfconfig_rx_lna_switch_pt_get_non_bypass_hold_timer(lna_switch_pt_ptr),(int)path, 2); /*lint !e701*/

   lna_switch_pt_ptr = rfconfig_rx_lna_get_switch_point_info_from_linearity(lna_linearity_info_ptr, 3);
   if (RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__FIXED == rfconfig_rx_lna_switch_pt_get_config_mode(lna_switch_pt_ptr))
   {
      /* load IM LEVEL values  */
      MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_IM_LEVELn), DATA, rfconfig_rx_lna_switch_pt_get_im(lna_switch_pt_ptr), (int)path, 4); /*lint !e701*/
   }
   /* load bypass and non bypass timers */
   MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_BP_TIMER_n), DATA, rfconfig_rx_lna_switch_pt_get_bypass_hold_timer(lna_switch_pt_ptr), (int)path, 3); /*lint !e701*/
   MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_NBP_TIMER_n), DATA, rfconfig_rx_lna_switch_pt_get_non_bypass_hold_timer(lna_switch_pt_ptr),(int)path, 3); /*lint !e701*/

   #else /* ! #ifdef RF_HAS_RFCONFIG_RX_LNA */
  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];

  if (path >= RF_PATH_MAX)
  {
    MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
    rf_error_flag = TRUE;
    return;
  }
    /* load IM LEVEL values  */
  MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_IM_LEVELn), DATA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im_level1[path], (int)path, 1); /*lint !e701*/
  MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_IM_LEVELn), DATA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im_level2[path], (int)path, 2); /*lint !e701*/  
  if (rf_path_0.rfrdev != QSC11X0_RX) 
  {  
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_IM_LEVELn), DATA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im_level3[path], (int)path, 3); /*lint !e701*/
    //MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_IM_LEVELn), DATA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im_level4[path], (int)path, 4); /*lint !e701*/
  }
  MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_IM_LEVELn), DATA, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im_level4[path], (int)path, 4);
  

    /* load bypass and non bypass timers */

  /* Specific settings for RFR6125, RFR6175, RFR6500 and QSC60x5 */
  if ( ( path == RF_PATH_0 && ( ( rf_path_0.rfrdev == RFR6125 ) || 
                                ( rf_path_0.rfrdev == RFR6175 ) ) 
       ) ||
       ( path == RF_PATH_1 && ( ( rf_path_1.rfrdev == RFR6125 ) || 
                                ( rf_path_1.rfrdev == RFR6175 ) ) ) 
     )
  {
     MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_BP_TIMER_n), DATA, CAGC_BYPASS_TIMER_0_RFR6125_V, (int)path, 0); /*lint !e701*/
  }
  else if( (path == RF_PATH_0) && ( rf_path_0.rfrdev == QSC11X0_RX))
  {
     MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_BP_TIMER_n), DATA,  CAGC_BYPASS_TIMER_1_QSC11X0_V, (int)path, 0); /*lint !e701*/         
  }
  else if (( path == RF_PATH_0 && ( rf_path_0.rfrdev == RFR6500 || rf_path_0.rfrdev == RFR6500_DRX ||
                                    rf_path_0.rfrdev == RFR6525 || rf_path_0.rfrdev == RFR6525_DRX ||
                                    rf_path_0.rfrdev == QSC60X5_RX ||
                                    rf_path_0.rfrdev == RTR6500_RX ))                              ||
           ( path == RF_PATH_1 && ( rf_path_1.rfrdev == RFR6500 || rf_path_1.rfrdev == RFR6500_DRX ||
                                    rf_path_1.rfrdev == RFR6525 || rf_path_1.rfrdev == RFR6525_DRX ||
                                    rf_path_1.rfrdev == QSC60X5_RX ||
                                    rf_path_1.rfrdev == RTR6500_RX )))
  {
    if ((path == RF_PATH_0) && (rf_chain_ptr->rf_curr_cdma_band == RF_BC0_BAND)
#ifdef RF_HAS_HDR
#error code not present
#else
    )
#endif /* RF_HAS_HDR */
    {  /* 1x mode - BC0 */
       if (rf_path_0.rfrdev == QSC60X5_RX) 
       {
         /* QSC60X5 */
         MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_BP_TIMER_n), DATA, CAGC_BYPASS_TIMER_0_QSC60X5_V, (int)path, 0); /*lint !e701*/
       }
       else if ( ( path == RF_PATH_0 && rf_path_0.rfrdev == RTR6500_RX ) 
                 || ( path == RF_PATH_1 && rf_path_1.rfrdev == RTR6500_RX ) )
       {
           /* RTR6500 */
           MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_BP_TIMER_n), DATA, CAGC_BYPASS_TIMER_0_RTR6500_V, (int)path, 0); /*lint !e701*/
       }
       else
       {
         /* RFR65XX */
           MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_BP_TIMER_n), DATA, CAGC_BYPASS_TIMER_0_RFR6500_V, (int)path, 0); /*lint !e701*/
       }
    } /* 1x mode - BC0 */
  }
  else if (rf_path_0.rfrdev == RFR61X5) 
  {
     /* RFR61X5 */
     MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_BP_TIMER_n), DATA, CAGC_BYPASS_TIMER_0_RFR61X5_V, path, 0);
  }
  else
  {
     /* All other RFRs */
     MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_BP_TIMER_n), DATA, CAGC_BYPASS_TIMER_0_V, (int)path, 0); /*lint !e701*/
  }

  if (( path == RF_PATH_0 && ( rf_path_0.rfrdev == RFR6500 || rf_path_0.rfrdev == RFR6500_DRX ||
                               rf_path_0.rfrdev == RFR6525 || rf_path_0.rfrdev == RFR6525_DRX )) ||
      ( path == RF_PATH_1 && ( rf_path_1.rfrdev == RFR6500 || rf_path_1.rfrdev == RFR6500_DRX ||
                               rf_path_1.rfrdev == RFR6525 || rf_path_1.rfrdev == RFR6525_DRX )))
  {
    /* RFR65XX */
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_NBP_TIMER_n), DATA, CAGC_NON_BYPASS_TIMER_RFR6500_V,(int)path, 0); /*lint !e701*/
  }

  else if( (path == RF_PATH_0) && ( rf_path_0.rfrdev == QSC11X0_RX))
  {
     MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_NBP_TIMER_n), DATA,  0x28, (int)path, 0); /*lint !e701*/         
  }
  else
  {
    /* All other RFRs */
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_NBP_TIMER_n), DATA, CAGC_NON_BYPASS_TIMER_0_V,(int)path, 0); /*lint !e701*/
  }

  /* Specific settings for RF Chipsets  - RF_PATH_1 is handeled to support future diversity */
  if (( path == RF_PATH_0 && ( rf_path_0.rfrdev == RFR6135 )) ||
      ( path == RF_PATH_1 && ( rf_path_1.rfrdev == RFR6135 )))
  {
    /* RFR6135 */
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_BP_TIMER_n), DATA, CAGC_BYPASS_TIMER_1_RFR6135_V, (int)path, 1); /*lint !e701*/
  }
  else if (( path == RF_PATH_0 && ( rf_path_0.rfrdev == RFR6500 || rf_path_0.rfrdev == RFR6500_DRX ||
                                    rf_path_0.rfrdev == RFR6525 || rf_path_0.rfrdev == RFR6525_DRX )) ||
           ( path == RF_PATH_1 && ( rf_path_1.rfrdev == RFR6500 || rf_path_1.rfrdev == RFR6500_DRX ||
                                    rf_path_1.rfrdev == RFR6525 || rf_path_1.rfrdev == RFR6525_DRX )))
  {
    /* RFR65XX */
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_BP_TIMER_n), DATA, CAGC_BYPASS_TIMER_1_RFR6500_V, (int)path, 1); /*lint !e701*/
  }
  else if (( path == RF_PATH_0 && ( rf_path_0.rfrdev == QSC60X5_RX )) ||
           ( path == RF_PATH_1 && ( rf_path_1.rfrdev == QSC60X5_RX )))
  {
    /* QSC60X5 */
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_BP_TIMER_n), DATA, CAGC_BYPASS_TIMER_1_QSC60X5_V, (int)path, 1); /*lint !e701*/    
  }
  else if (( path == RF_PATH_0 && rf_path_0.rfrdev == RTR6500_RX ) ||
           ( path == RF_PATH_1 && rf_path_1.rfrdev == RTR6500_RX ))
  {
      /* RTR6500 */
      MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_BP_TIMER_n), DATA, CAGC_BYPASS_TIMER_1_RTR6500_V, (int)path, 1); /*lint !e701*/
  }
  else if( (path == RF_PATH_0) && ( rf_path_0.rfrdev == QSC11X0_RX))
  {
     MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_BP_TIMER_n), DATA,  CAGC_BYPASS_TIMER_1_QSC11X0_V, (int)path, 1); /*lint !e701*/
  }
  else
  {
    /* All other RFRs */
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_BP_TIMER_n), DATA, CAGC_BYPASS_TIMER_1_V, (int)path, 1); /*lint !e701*/
  }

  if (( path == RF_PATH_0 && ( rf_path_0.rfrdev == RFR6500 || rf_path_0.rfrdev == RFR6500_DRX ||
                               rf_path_0.rfrdev == RFR6525 || rf_path_0.rfrdev == RFR6525_DRX )) ||
      ( path == RF_PATH_1 && ( rf_path_1.rfrdev == RFR6500 || rf_path_1.rfrdev == RFR6500_DRX ||
                               rf_path_1.rfrdev == RFR6525 || rf_path_1.rfrdev == RFR6525_DRX )))
  {
    /* RFR65XX */
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_NBP_TIMER_n), DATA, CAGC_NON_BYPASS_TIMER_RFR6500_V, (int)path, 1); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_BP_TIMER_n), DATA, CAGC_BYPASS_TIMER_2_RFR6500_V, (int)path, 2); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_NBP_TIMER_n), DATA, CAGC_NON_BYPASS_TIMER_RFR6500_V,(int)path, 2); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_BP_TIMER_n), DATA, CAGC_BYPASS_TIMER_3_RFR6500_V, (int)path, 3); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_NBP_TIMER_n), DATA, CAGC_NON_BYPASS_TIMER_RFR6500_V,(int)path, 3); /*lint !e701*/
  }  
  else if( (path == RF_PATH_0) && ( rf_path_0.rfrdev == QSC11X0_RX))
  {
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_NBP_TIMER_n), DATA,  0x28, (int)path, 1); /* lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_BP_TIMER_n), DATA, CAGC_BYPASS_TIMER_2_QSC11X0_V, (int)path, 2); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_NBP_TIMER_n), DATA,CAGC_NON_BYPASS_TIMER_2_QSC11X0_V,(int)path, 2); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_BP_TIMER_n), DATA, CAGC_BYPASS_TIMER_3_QSC11X0_V, (int)path, 3); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_NBP_TIMER_n), DATA,CAGC_NON_BYPASS_TIMER_3_QSC11X0_V,(int)path, 3); /*lint !e701*/
  }
  else
  {
    /* All other RFRs */
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_NBP_TIMER_n), DATA, CAGC_NON_BYPASS_TIMER_1_V, (int)path, 1); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_BP_TIMER_n), DATA, CAGC_BYPASS_TIMER_2_V, (int)path, 2); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_NBP_TIMER_n), DATA, CAGC_NON_BYPASS_TIMER_2_V,(int)path, 2); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_BP_TIMER_n), DATA, CAGC_BYPASS_TIMER_3_V, (int)path, 3); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_NBP_TIMER_n), DATA, CAGC_NON_BYPASS_TIMER_3_V,(int)path, 3); /*lint !e701*/
  }

   #endif /* RF_HAS_RFCONFIG_RX_LNA */
}


/*============================================================================

FUNCTION  RF_DISABLE_IN_LEVEL_CTLS                                     EXTERNAL

DESCRIPTION
  This function will disable the switching behavior of the HW LNA State
  Machine in the presence of IM.  This can be accomplished by setting the
  bypass and nonbypass timers for each state to 0x0 and railing the IM
  levels.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void rf_disable_im_level_ctls( rfcom_device_enum_type device )
{
    rf_path_enum_type path = rfcom_to_path[ device ];

    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_BP_TIMER_n),  DATA , 0x04, (int)path, 0); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_NBP_TIMER_n), DATA , 0x04, (int)path, 0); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_BP_TIMER_n),  DATA , 0x04, (int)path, 1); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_NBP_TIMER_n), DATA , 0x04, (int)path, 1); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_BP_TIMER_n),  DATA , 0x04, (int)path, 2); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_NBP_TIMER_n), DATA , 0x04, (int)path, 2); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_BP_TIMER_n),  DATA , 0x04, (int)path, 3); /*lint !e701*/
    MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_NBP_TIMER_n), DATA , 0x04, (int)path, 3); /*lint !e701*/

}



/*============================================================================

FUNCTION  RF_ENABLE_IN_LEVEL_CTLS                                     EXTERNAL

DESCRIPTION
  This function will enable the switching behavior of the HW LNA State
  Machine in the presence of IM by setting the bypass timers, nonbypass timers
  and the IM Levels to their IM settings (previously read from NV)

DEPENDENCIES
  rf_init_hw_lna_ctl must have been called once.
  rf_nv_digtial_items_ptr must be initialized and pointing to correct
  digital structure.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void rf_enable_im_level_ctls( rfcom_device_enum_type device )
{
#ifdef RF_HAS_LOAD_IM_CTLS

    /* Setup im levels */
    rf_load_im_level_ctls ( device );

    /* Load LNA offsets, and update switch points if OLTA is enabled */
    rf_update_lna_offsets( device );

#else
#error code not present
#endif /* RF_HAS_LOAD_IM_CTLS */
  }


/*============================================================================

FUNCTION  RF_COMPUTE_LOW_GAIN_AGC_VAL                             INTERNAL

DESCRIPTION
  This function calculates and loads the AGC Value register with the value
  corresponding to the AGC value half way between LNA RISE and LNA FALL
  for the lowest gain state.

DEPENDENCIES
  rf_init_hw_lna_ctl must have been called once.
  rf_nv_digtial_items_ptr must be initialized and pointing to correct
  digital structure.
  AGC VALUE and HW LNA state machine should be frozen (CPU override mode)
  when this function is called. The HW LNA state machine should be in the
  low gain state.
  This function relies on the fact that the LNA RISE value for the gain state
  below the lowest gain state is set equal to 0x7f.  See
  rf_determine_lna_low_gain_state().

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
int2 rf_compute_low_gain_agc_val( rfcom_device_enum_type device )
{
   lna_gain_state_type low_gain_state = INVALID_LNA_GAIN_STATE;
   int2 result = 0;
   int1 lna_rise_val = RF_LNA_RANGE_MAX;
   int1 lna_fall_val = RF_LNA_RANGE_MAX;
   
   #ifdef RF_HAS_RFCONFIG_RX_LNA
   const rfconfig_rx_lna_linearity_info_type  *lna_linearity_info_ptr = rf_get_current_rfconfig_rx_lna_linearity_info(device);
   const rfconfig_rx_lna_switch_pt_type       *lna_switch_pt_ptr = NULL;
   uint32 gain_step_index = 0;  // index to get RISE and FALL values for 'low_gain_state'

   // at least one LNA gain step must have been configured for this linearity
   RF_ASSERT(NULL != lna_linearity_info_ptr);

   if(lna_linearity_info_ptr == NULL)
   {
       MSG_FATAL("lna_linearity_info_ptr is NULL",0,0,0);
       rf_error_flag = TRUE;
       return -1;
   }

   RF_ASSERT(lna_linearity_info_ptr->num_gain_steps > 0);
   // #gain_stages = num_gain_steps + 1; in enum 'lna_gain_state_type' the sequence of
   //  valid gain_stages start from index 0, and so subtract 1 from #gain_stages to compare
   //  against INVALID_LNA_GAIN_STATE
   RF_ASSERT(lna_linearity_info_ptr->num_gain_steps < INVALID_LNA_GAIN_STATE);

   /* determine the low gain state, and its rise/fall values */
   low_gain_state =  rf_determine_lna_low_gain_state( device );
   if ( (low_gain_state > LNA_GAIN_STATE_0)
        && (low_gain_state < INVALID_LNA_GAIN_STATE) )
   {
      gain_step_index = low_gain_state - 1;
      lna_switch_pt_ptr = rfconfig_rx_lna_get_switch_point_info_from_linearity(lna_linearity_info_ptr, gain_step_index);
      lna_rise_val = rfconfig_rx_lna_switch_pt_get_rise(lna_switch_pt_ptr);
      lna_fall_val = rfconfig_rx_lna_switch_pt_get_fall(lna_switch_pt_ptr);
   }

   #else /* ! #ifdef RF_HAS_RFCONFIG_RX_LNA */
   rf_path_enum_type path = rfcom_to_path[ device ];
   rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];

#ifdef RF_HAS_POWER_MODE_LIB
 rflib_power_mode_type power_mode = rflib_get_power_mode( path );
#endif /* RF_HAS_POWER_MODE_LIB */

   /* determine the low gain state */
   low_gain_state =  rf_determine_lna_low_gain_state( device );

   /* get rise/fall values for the low gain state */
#ifdef RF_HAS_POWER_MODE_LIB
   switch ( low_gain_state )
   {
   case LNA_GAIN_STATE_4 :
       lna_rise_val = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_4_rise[path][power_mode];
       lna_fall_val = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_4_fall[path][power_mode];
       break;
   case LNA_GAIN_STATE_3 :
       lna_rise_val = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_3_rise[path][power_mode];
       lna_fall_val = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_3_fall[path][power_mode];
       break;
   case LNA_GAIN_STATE_2 :
       lna_rise_val = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_2_rise[path][power_mode];
       lna_fall_val = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_2_fall[path][power_mode];
       break;
   case LNA_GAIN_STATE_1 :
       lna_rise_val = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_1_rise[path][power_mode];
       lna_fall_val = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_1_fall[path][power_mode];
       break;
   default :
       /* -----------------------------------------------------
       ** default value is AGC Value corresponding to highest
       ** power, i.e., LNA_RANGE_MAX
       ** ----------------------------------------------------- */
       break;
   }
#else
   switch ( low_gain_state )
   {
   case LNA_GAIN_STATE_4 :
       lna_rise_val = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_4_rise[path];
       lna_fall_val = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_4_fall[path];
       break;
   case LNA_GAIN_STATE_3 :
       lna_rise_val = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_3_rise[path];
       lna_fall_val = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_3_fall[path];
       break;
   case LNA_GAIN_STATE_2 :
       lna_rise_val = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_2_rise[path];
       lna_fall_val = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_2_fall[path];
       break;
   case LNA_GAIN_STATE_1 :
       lna_rise_val = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_1_rise[path];
       lna_fall_val = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_1_fall[path];
       break;
   default :
       /* -----------------------------------------------------
       ** default value is AGC Value corresponding to highest
       ** power, i.e., LNA_RANGE_MAX
       ** ----------------------------------------------------- */
       break;
   }
#endif /* RF_HAS_POWER_MODE_LIB */

   #endif /* RF_HAS_RFCONFIG_RX_LNA */

   /* --------------------------------------------------------------
   ** AGC VALUE register is 10 bits
   ** Find midpoint between 8 bit int1s and get 10 bit equivalent.
   ** Divide by two and multiply by four (to convert to 10 bits) is
   ** same as multiply by two.
   ** -------------------------------------------------------------- */
   result = ((int2)lna_rise_val + (int2)lna_fall_val ) * 2;

   return result;
}


/*============================================================================

FUNCTION  RF_SET_AGC_VAL_TO_LOW_GAIN                                  EXTERNAL

DESCRIPTION
  This function calculates and loads the AGC Value register with the value
  corresponding to the AGC value half way between LNA RISE and LNA FALL
  for the lowest gain state.

DEPENDENCIES
  rf_init_hw_lna_ctl must have been called once.
  rf_nv_digtial_items_ptr must be initialized and pointing to correct
  digital structure.
  AGC VALUE and HW LNA state machine should be frozen (CPU override mode)
  when this function is called. The HW LNA state machine should be in the
  low gain state.
  This function relies on the fact that the LNA RISE value for the gain state
  below the lowest gain state is set equal to 0x7f.  See
  rf_determine_lna_low_gain_state().

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void rf_set_agc_val_to_low_gain( rfcom_device_enum_type device )
{
   int2 result;

   rf_path_enum_type path = rfcom_to_path[ device ];

   if (path >= RF_PATH_MAX)
   {
      MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
      rf_error_flag = TRUE;
      return;
   }

   /* Override AGC value with MAX power setting */
   result = RF_RX_SIGNED_AGC_VAL_MAX_PWR;

   /* load AGC_VALUE register */
   /* in order to enable AGC_VALUE_WR value, must set LNA_CTL-AGC_VALUE_OVERRIDE bit */
   MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_LNA_CTL), AGC_VALUE_OVERRIDE, 
                  CAGC_RF_AGC_VALUE_OVERRIDE_EN, (int)path);
   HWIO_OUTMI(RXF_CDMA(RX_AGCc_VALUE_WR), (int)path, HWIO_RMSK(RXF_CDMA(RX_AGCc_VALUE_WR)), result); /*lint !e737 */ /* loss of sign- HWIO macro */

}



/*============================================================================

FUNCTION  RF_SET_LNA_DECISION_TO_LOW_GAIN_STATE                       EXTERNAL

DESCRIPTION
  This function overrides and forces the HW LNA state machine into the
  lowest gain state.

DEPENDENCIES
  rf_init_hw_lna_ctl must have been called once.
  rf_nv_digtial_items_ptr must be initialized and pointing to correct
  digital structure.
  This function relies on the fact that the LNA RISE value for the gain state
  below the lowest gain state is set equal to 0x7f.  See
  rf_determine_lna_low_gain_state().

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void rf_set_lna_decision_to_low_gain_state( rfcom_device_enum_type device )
{
  rf_set_cdma_lna_decision( device, rf_determine_lna_low_gain_state( device ));
}



/*============================================================================

FUNCTION  RF_DETERMINE_LNA_LOW_GAIN_STATE                       EXTERNAL

DESCRIPTION
  This function determines which LNA State is the lowest gain state. This
  function requires that the LNA RISE value for any gain state less than
  the lowest implemented gain state is set equal to RF_LNA_RANGE_MAX (0x7f)
  in either NV (read from RF Cal) or as the default value in the
  rf_nv_dig_items structure.

DEPENDENCIES
  rf_init_hw_lna_ctl must have been called once.
  rf_nv_digtial_items_ptr must be initialized and pointing to correct
  digital structure.
  This function relies on the fact that the LNA RISE value for the gain state
  below the lowest implemented gain state is set equal to 0x7f.

RETURN VALUE
  lna_gain_state_type: highest to lowest gain state is
                       LNA_GAIN_STATE_0->LNA_GAIN_STATE_4

SIDE EFFECTS
  None

=============================================================================*/
lna_gain_state_type rf_determine_lna_low_gain_state( rfcom_device_enum_type device )
{
   #ifdef RF_HAS_RFCONFIG_RX_LNA

   const rfconfig_rx_lna_linearity_info_type  *lna_linearity_info_ptr = rf_get_current_rfconfig_rx_lna_linearity_info(device);
   const rfconfig_rx_lna_switch_pt_type       *lna_switch_pt_ptr = NULL;
   int32                                       gain_step_index = 0;   // signed int32 to compare against 0
                                                                      //  post decrement in 'for' loop

   // at least one LNA gain step must have been configured for this linearity
   RF_ASSERT(NULL != lna_linearity_info_ptr);

   if(lna_linearity_info_ptr == NULL)
   {
       MSG_FATAL("lna_linearity_info_ptr is NULL",0,0,0);
       rf_error_flag = TRUE;
       return INVALID_LNA_GAIN_STATE;
   }

   RF_ASSERT(lna_linearity_info_ptr->num_gain_steps > 0);
   // #gain_stages = num_gain_steps + 1; in enum 'lna_gain_state_type' the sequence of
   //  valid gain_stages start from index 0, and so subtract 1 from #gain_stages to compare
   //  against INVALID_LNA_GAIN_STATE
   RF_ASSERT(lna_linearity_info_ptr->num_gain_steps < INVALID_LNA_GAIN_STATE);
   //  so whatever gain_step_index ends up being below will correspond to the lowest
   //  gain stage minus 1, as indexed through the enum 'lna_gain_state_type'
   for (
      gain_step_index = lna_linearity_info_ptr->num_gain_steps - 1;
      gain_step_index >= 0;
      gain_step_index --)
   {
      lna_switch_pt_ptr = rfconfig_rx_lna_get_switch_point_info_from_linearity(lna_linearity_info_ptr, gain_step_index);
      if ( ( RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__OLTA == rfconfig_rx_lna_switch_pt_get_config_mode(lna_switch_pt_ptr) )
           || ( RF_LNA_RANGE_MAX != rfconfig_rx_lna_switch_pt_get_rise(lna_switch_pt_ptr) ) )
      {
         break;
      }
   }

   return (lna_gain_state_type)(gain_step_index + 1); // add 1 to map it to the enum 'lna_gain_state_type'

   #else /* ! #ifdef RF_HAS_RFCONFIG_RX_LNA */
   rf_path_enum_type path = rfcom_to_path[ device ];
   rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];

#ifdef RF_HAS_POWER_MODE_LIB
   rflib_power_mode_type power_mode = rflib_get_power_mode( path );
   
   if (rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_4_rise[path][power_mode] != RF_LNA_RANGE_MAX) return LNA_GAIN_STATE_4;
   if (rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_3_rise[path][power_mode] != RF_LNA_RANGE_MAX) return LNA_GAIN_STATE_3;
   if (rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_2_rise[path][power_mode] != RF_LNA_RANGE_MAX) return LNA_GAIN_STATE_2;
   if (rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_1_rise[path][power_mode] != RF_LNA_RANGE_MAX) return LNA_GAIN_STATE_1;
#else
   if (rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_4_rise[path] != RF_LNA_RANGE_MAX) return LNA_GAIN_STATE_4;
   if (rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_3_rise[path] != RF_LNA_RANGE_MAX) return LNA_GAIN_STATE_3;
   if (rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_2_rise[path] != RF_LNA_RANGE_MAX) return LNA_GAIN_STATE_2;
   if (rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_1_rise[path] != RF_LNA_RANGE_MAX) return LNA_GAIN_STATE_1;
#endif /* RF_HAS_POWER_MODE_LIB */
   return LNA_GAIN_STATE_0;

   #endif /* RF_HAS_RFCONFIG_RX_LNA */
}


/*============================================================================

FUNCTION  RF_DETERMINE_LNA_HIGH_GAIN_STATE                       EXTERNAL

DESCRIPTION
  This function determines which LNA State is the highest gain state. This
  function requires that the LNA FALL value for every state other than the 
  highest implemented gain state is higher than RF_LNA_RANGE_MIN(0x80) in 
  either NV (read from RF Cal) or as the default value in the rf_nv_dig_items 
  structure.

DEPENDENCIES
  rf_init_hw_lna_ctl must have been called once.
  rf_nv_digtial_items_ptr must be initialized and pointing to correct
  digital structure.
  This function relies on the fact that the LNA FALL value for every state
  other than the highest implemented gain state is higher than 0x80.

RETURN VALUE
  lna_gain_state_type: highest to lowest gain state is
                       LNA_GAIN_STATE_0->LNA_GAIN_STATE_4

SIDE EFFECTS
  None

=============================================================================*/
lna_gain_state_type rf_determine_lna_high_gain_state( rfcom_device_enum_type device )
{
   #ifdef RF_HAS_RFCONFIG_RX_LNA

   const rfconfig_rx_lna_linearity_info_type  *lna_linearity_info_ptr = rf_get_current_rfconfig_rx_lna_linearity_info(device);
   const rfconfig_rx_lna_switch_pt_type       *lna_switch_pt_ptr = NULL;
   uint32                                      gain_step_index = 0;

   // at least one LNA gain step must have been configured for this linearity
   RF_ASSERT(NULL != lna_linearity_info_ptr);

   if(lna_linearity_info_ptr == NULL)
   {
       MSG_FATAL("lna_linearity_info_ptr is NULL",0,0,0);
       rf_error_flag = TRUE;
       return INVALID_LNA_GAIN_STATE;
   }

   RF_ASSERT(lna_linearity_info_ptr->num_gain_steps > 0);
   // #gain_stages = num_gain_steps + 1; in enum 'lna_gain_state_type' the sequence of
   //  valid gain_stages start from index 0, and so subtract 1 from #gain_stages to compare
   //  against INVALID_LNA_GAIN_STATE
   RF_ASSERT(lna_linearity_info_ptr->num_gain_steps < INVALID_LNA_GAIN_STATE);
   //  so whatever gain_step_index ends up being below will correspond to the highest
   //  gain stage as indexed through the enum 'lna_gain_state_type'
   for (
      gain_step_index = 0;
      gain_step_index < lna_linearity_info_ptr->num_gain_steps; 
      gain_step_index ++)
   {
      lna_switch_pt_ptr = rfconfig_rx_lna_get_switch_point_info_from_linearity(lna_linearity_info_ptr, gain_step_index);
      if ( ( RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__OLTA == rfconfig_rx_lna_switch_pt_get_config_mode(lna_switch_pt_ptr) )
           || ( RF_LNA_RANGE_MIN != rfconfig_rx_lna_switch_pt_get_rise(lna_switch_pt_ptr) ) )
      {
         break;
      }
   }

   return (lna_gain_state_type)gain_step_index;

   #else /* ! #ifdef RF_HAS_RFCONFIG_RX_LNA */
   rf_path_enum_type path = rfcom_to_path[ device ];
   rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];

#ifdef RF_HAS_POWER_MODE_LIB
   rflib_power_mode_type power_mode = rflib_get_power_mode( path );
   
   if (rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_1_rise[path][power_mode] != RF_LNA_RANGE_MIN) return LNA_GAIN_STATE_0;/*lint !e650 */
   if (rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_2_rise[path][power_mode] != RF_LNA_RANGE_MIN) return LNA_GAIN_STATE_1;/*lint !e650 */
   if (rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_3_rise[path][power_mode] != RF_LNA_RANGE_MIN) return LNA_GAIN_STATE_2;/*lint !e650 */
   if (rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_4_rise[path][power_mode] != RF_LNA_RANGE_MIN) return LNA_GAIN_STATE_3;/*lint !e650 */
#else
   if (rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_1_rise[path] != RF_LNA_RANGE_MIN) return LNA_GAIN_STATE_0;
   if (rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_2_rise[path] != RF_LNA_RANGE_MIN) return LNA_GAIN_STATE_1;
   if (rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_3_rise[path] != RF_LNA_RANGE_MIN) return LNA_GAIN_STATE_2;
   if (rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_4_rise[path] != RF_LNA_RANGE_MIN) return LNA_GAIN_STATE_3;
#endif /* RF_HAS_POWER_MODE_LIB */

   return LNA_GAIN_STATE_4;

   #endif /* RF_HAS_RFCONFIG_RX_LNA */
}
/* EJECT  */
/*============================================================================

FUNCTION RF_SET_CDMA_LNA_DECISION                                     EXTERNAL

DESCRIPTION
  This function allows direct control of the LNA Decision bits in the HW LNA
  state machine. These changes will propogate to the LNA RANGE and MIXER RANGE
  pins if they have not been overriden in AGC_CTL2.

 Input: lna_gain_state_type (valid gain states defined in rf.h)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If an invalid gain state is requested the gain is not changed
  If successful the LNA State machine is left in the override state

=============================================================================*/
void rf_set_cdma_lna_decision(rfcom_device_enum_type device,lna_gain_state_type state )
{
  byte lna_decision;
  rf_path_enum_type path = rfcom_to_path[ device ];

  if (path >= RF_PATH_MAX)
  {
     MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
     rf_error_flag = TRUE;
     return;
  }

  switch ( state ) {
    case LNA_GAIN_STATE_0:
      lna_decision = LNA_RANGE_DECISION_0000;
      break;
    case LNA_GAIN_STATE_1:
      lna_decision = LNA_RANGE_DECISION_0001;
      break;
    case LNA_GAIN_STATE_2:
      lna_decision = LNA_RANGE_DECISION_0011;
      break;
    case LNA_GAIN_STATE_3:
      lna_decision = LNA_RANGE_DECISION_0111;
      break;
    case LNA_GAIN_STATE_4:
      lna_decision = LNA_RANGE_DECISION_1111;
      break;
    default:
      ERR("Invalid LNA Decision requested", 0, 0, 0);
      return;
    }

    /* overide the LNA State machine */
    RF_OVERRIDE_LNA_STATE_MACHINE( device );

    /* write the decision bits to Range Force */
    MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_LNA_DATA), LNA_DECISION, lna_decision, (int)path);
}


/*============================================================================

FUNCTION RF_SET_CDMA_LNA_RANGE                                        EXTERNAL

DESCRIPTION
  This function allows direct control of the LNA RANGE and MIXER RANGE pins.

 Input: lna_gain_state_type (valid gain states defined in rf.h)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If an invalid gain state is requested the range pins are not changed
  If successful the LNA Range pins are left in the overiden state

=============================================================================*/
void rf_set_cdma_lna_range( rfcom_device_enum_type device, lna_gain_state_type state )
{
  byte lna_range;
  rf_path_enum_type path = rfcom_to_path[ device ];

  switch ( state ) {
    case LNA_GAIN_STATE_0:
      lna_range = LNA_RANGE_DATA_000;
      break;
    case LNA_GAIN_STATE_1:
      lna_range = LNA_RANGE_DATA_001;
      break;
    case LNA_GAIN_STATE_2:
      lna_range = LNA_RANGE_DATA_010;
      break;
    case LNA_GAIN_STATE_3:
      lna_range = LNA_RANGE_DATA_011;
      break;
    case LNA_GAIN_STATE_4:
      lna_range = LNA_RANGE_DATA_111;
      break;
    default:
      ERR("Invalid LNA Decision requested", 0, 0, 0);
      return;
    }

  if(path < RF_PATH_MAX)
  {
    /* overide the LNA Range pins */
    MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_LNA_CTL), LNA_RANGE_OVERRIDE_N, CAGC_RF_LNA_RG_OVRD_ENA_V, (int)path);

    /* write the range bits to Range Data */
    MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_LNA_DATA), LNA_RANGE, lna_range, (int)path);
  }
  else
  {
    RF_FATAL_ERROR("Invalid path");
    return;
  }
}
/* EJECT  */

/*============================================================================

FUNCTION rf_get_max_sigma_delta_level                                        INTERNAL

DESCRIPTION
  This function returns the maximum sigma delta level based on the RF chipset used.

 Input: rfcom_device_enum_type used.

DEPENDENCIES
  RF Device must have been set before.

RETURN VALUE
  Returns maximum sigma delta level.

SIDE EFFECTS

=============================================================================*/
int16 rf_get_max_sigma_delta_level(rfcom_device_enum_type device)
{
    rf_path_enum_type path = rfcom_to_path[ device ];
    rfr_device_enum rfr_device; 
    int16 sigma_delta_max_level_limit = 0;

    /* determine the MAX LEVEL for sigma delta based on the chipset used. */


    if ( path == RF_PATH_0 )
    {
      rfr_device = rf_path_0.rfrdev; 
    }
    else if ( path == RF_PATH_1 )
    {
      rfr_device = rf_path_1.rfrdev;
    }
    else
    {
      RF_FATAL_ERROR("Invalid path");
      return sigma_delta_max_level_limit;
    }

    switch (rfr_device)
    {
    case RTR6500_RX :
        sigma_delta_max_level_limit = 188;
        break;
    case QSC60X5_RX :
    case QSC11X0_RX:
       sigma_delta_max_level_limit = 170;
       break;
    default:
       sigma_delta_max_level_limit = (210 - 17);
       break;
    };

    return sigma_delta_max_level_limit;
}

/*============================================================================

FUNCTION RF_UPDATE_LNA_OFFSETS                                       INTERNAL

DESCRIPTION
  This function will load in LNA offsets compensated by the current frequency.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void rf_update_lna_offsets( rfcom_device_enum_type device )
{
   int16 lna_1_offset = 0;
   int16 lna_2_offset = 0;
   int16 lna_3_offset = 0;
   int16 lna_4_offset = 0;
 rf_path_enum_type path = rfcom_to_path[ device ];

#ifndef RF_HAS_RFCONFIG_RX_LNA
 rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];
#endif

#ifdef RF_HAS_POWER_MODE_LIB
 rflib_power_mode_type power_mode = rflib_get_power_mode( path );
 if (power_mode >= RFLIB_MAX_POWER_MODE) 
 {
     MSG_FATAL("power mode >= RFLIB_MAX_POWER_MODE", 0, 0, 0);
     rf_error_flag = TRUE;
     return;
 }
#endif /* RF_HAS_POWER_MODE_LIB */

 if (path >= RF_PATH_MAX)
 {
     MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
     rf_error_flag = TRUE;
     return;
 }

 /* Determine lna offset values */

#ifdef RF_HAS_LINEAR_INTERPOLATION
 #ifdef RF_HAS_POWER_MODE_LIB
 lna_1_offset=rf_freq_comp.i_lna_1_offset_vs_freq[path][power_mode];
 #else
 lna_1_offset=rf_freq_comp.i_lna_1_offset_vs_freq[path];
 #endif /* RF_HAS_POWER_MODE_LIB */
#else
 lna_1_offset=rf_chain_ptr->rf_nv_dig_items_ptr->
                rfnv_cdma_lna_1_offset_vs_freq[path][ rf_chain_ptr->
                rf_freq_index ];

#endif

 lna_1_offset = RF_SATURATE(lna_1_offset, LNA_OFFSET_MIN_V, \
                                          LNA_OFFSET_MAX_V);

#ifdef RF_HAS_LINEAR_INTERPOLATION
 #ifdef RF_HAS_POWER_MODE_LIB
 lna_2_offset=rf_freq_comp.i_lna_2_offset_vs_freq[path][power_mode];
 #else
 lna_2_offset=rf_freq_comp.i_lna_2_offset_vs_freq[path];
 #endif /* RF_HAS_POWER_MODE_LIB */
#else
 lna_2_offset= rf_chain_ptr->rf_nv_dig_items_ptr->
               rfnv_cdma_lna_2_offset_vs_freq[path][ rf_chain_ptr->
               rf_freq_index ];
#endif

 lna_2_offset = RF_SATURATE(lna_2_offset, LNA_OFFSET_MIN_V, \
                                            LNA_OFFSET_MAX_V);

#ifdef RF_HAS_LINEAR_INTERPOLATION
 #ifdef RF_HAS_POWER_MODE_LIB
 lna_3_offset=rf_freq_comp.i_lna_3_offset_vs_freq[path][power_mode];
 #else
 lna_3_offset=rf_freq_comp.i_lna_3_offset_vs_freq[path];
 #endif /* RF_HAS_POWER_MODE_LIB */
#else
 lna_3_offset=rf_chain_ptr->rf_nv_dig_items_ptr->
              rfnv_cdma_lna_3_offset_vs_freq[path][ rf_chain_ptr->
              rf_freq_index ];

#endif

 lna_3_offset = RF_SATURATE(lna_3_offset, LNA_OFFSET_MIN_V, \
                                          LNA_OFFSET_MAX_V);

#ifdef RF_HAS_LINEAR_INTERPOLATION
 #ifdef RF_HAS_POWER_MODE_LIB
 lna_4_offset=rf_freq_comp.i_lna_4_offset_vs_freq[path][power_mode];
 #else
 lna_4_offset=rf_freq_comp.i_lna_4_offset_vs_freq[path];
 #endif /* RF_HAS_POWER_MODE_LIB */
#else
 lna_4_offset= rf_chain_ptr->rf_nv_dig_items_ptr->
               rfnv_cdma_lna_4_offset_vs_freq[path][ rf_chain_ptr->
               rf_freq_index ];

#endif

 lna_4_offset = RF_SATURATE(lna_4_offset, LNA_OFFSET_MIN_V, \
                                          LNA_OFFSET_MAX_V);

 /* Load LNA offsets and their corresponding AGC MIN values */
 MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_OFFSET),DATA, lna_1_offset,(int)path,1); /*lint !e701*/
 MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_VALUE_n_MIN), DATA, RF_GET_RX_AGC_MIN_VAL(lna_1_offset), (int)path,1); /*lint !e701*/

 MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_OFFSET),DATA, lna_2_offset,(int)path,2); /*lint !e701*/
 MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_VALUE_n_MIN), DATA, RF_GET_RX_AGC_MIN_VAL(lna_2_offset), (int)path,2); /*lint !e701*/

 MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_OFFSET),DATA, lna_3_offset,(int)path,3); /*lint !e701*/
 MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_VALUE_n_MIN), DATA, RF_GET_RX_AGC_MIN_VAL(lna_3_offset), (int)path,3); /*lint !e701*/

 MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_OFFSET),DATA, lna_4_offset,(int)path,4); /*lint !e701*/
 MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_VALUE_n_MIN), DATA, RF_GET_RX_AGC_MIN_VAL(lna_4_offset), (int)path,4); /*lint !e701*/


 /* compute the optimized LNA Rise/Fall/IM thresholds */
 {
     int16 optimal_rise_val; 
     int16 optimal_fall_val;
     int16 optimal_im_val;
     int16 vga_gain_offset;
     int16 lsb_per_bit;
     int16 sigma_delta_max_level;
#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */

   #ifdef RF_HAS_RFCONFIG_RX_LNA
     const rfconfig_rx_lna_linearity_info_type  *lna_linearity_info_ptr = rf_get_current_rfconfig_rx_lna_linearity_info(device);
     const rfconfig_rx_lna_switch_pt_type       *lna_switch_pt_ptr = NULL;

     /* how many counts per dB */
     lsb_per_bit = ( cagc_dynamic_range_setting == RF_HAS_102DB_CDMA_DYNAMIC_RANGE ) ? RX_AGC_VAL_CNTS_PER_DB_102_DB : RX_AGC_VAL_CNTS_PER_DB_85_DB;

     /* calculate VGA gain offset for this frequency in 2/5 dB resolution*/
     #ifdef RF_HAS_LINEAR_INTERPOLATION

        #ifdef RF_HAS_POWER_MODE_LIB
     vga_gain_offset =  (int16)(( 
         rf_freq_comp.i_vga_gain_offset_vs_freq[path][power_mode]
         * lsb_per_bit/VGA_GAIN_OFFSET_SCALE_FACTOR )
         >> VGA_GAIN_OFFSET_SHIFT) ;/*lint !e702 */
        #else /* !defined(RF_HAS_POWER_MODE_LIB)*/
     vga_gain_offset =  (int16)(( 
         rf_freq_comp.i_vga_gain_offset_vs_freq[path]
         * lsb_per_bit/VGA_GAIN_OFFSET_SCALE_FACTOR )
         >> VGA_GAIN_OFFSET_SHIFT) ;
        #endif /* RF_HAS_POWER_MODE_LIB */

     #else /* !defined(RF_HAS_LINEAR_INTERPOLATION)*/

     vga_gain_offset =  (int16)(((rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_vga_gain_offset_vs_freq[path]
        [ rf_chain_ptr->rf_freq_index ])
        * lsb_per_bit/VGA_GAIN_OFFSET_SCALE_FACTOR )
        >> VGA_GAIN_OFFSET_SHIFT );

     #endif /* RF_HAS_LINEAR_INTERPOLATION */

     /* max Sigma Delta level in 2/5 dB resolution*/
     sigma_delta_max_level = (int16)(( ( rf_get_max_sigma_delta_level(device) * lsb_per_bit)/SIGMA_DELTA_MAX_LEVEL_SCALE_FACTOR ) >> 2);/*lint !e702 */

     RF_ASSERT(lna_linearity_info_ptr->num_gain_steps == 4);  // expecting configuration for exactly 4 LNA gain steps
     /* compute and load optimal LNA RISE/FALL/IM values for 1st switch point */
     lna_switch_pt_ptr = rfconfig_rx_lna_get_switch_point_info_from_linearity(lna_linearity_info_ptr, 0);
     if ( RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__OLTA == rfconfig_rx_lna_switch_pt_get_config_mode(lna_switch_pt_ptr) )
     {
        optimal_rise_val = ( sigma_delta_max_level
                             - rfconfig_rx_lna_switch_pt_get_backoff(lna_switch_pt_ptr)
                             - vga_gain_offset );
        optimal_fall_val = optimal_rise_val - rfconfig_rx_lna_switch_pt_get_hysteresis(lna_switch_pt_ptr);
        optimal_im_val   = optimal_rise_val;
        /* saturate to 8 bits */
        optimal_rise_val = RF_THRESH_SATURATE(optimal_rise_val, NUM_BITS_TO_LIMIT_RESULT);
        optimal_fall_val = RF_THRESH_SATURATE(optimal_fall_val, NUM_BITS_TO_LIMIT_RESULT);
        optimal_im_val   = RF_THRESH_SATURATE(optimal_im_val,   NUM_BITS_TO_LIMIT_RESULT);
        /* load the new Rise/Fall/IM levels */
        MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_RISE), DATA, optimal_rise_val, (int)path, 1); /*lint !e701*/
        MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_FALL), DATA, optimal_fall_val, (int)path, 1); /*lint !e701*/
        MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_IM_LEVELn),  DATA, optimal_im_val,   (int)path, 1); /*lint !e701*/
 #ifdef RF_HAS_OLTA_DEBUG
#error code not present
 #endif
     }
     /* compute and load optimal LNA RISE/FALL/IM values for 2nd switch point */
     lna_switch_pt_ptr = rfconfig_rx_lna_get_switch_point_info_from_linearity(lna_linearity_info_ptr, 1);
     if ( RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__OLTA == rfconfig_rx_lna_switch_pt_get_config_mode(lna_switch_pt_ptr) )
     {
        optimal_rise_val = (int16) ( sigma_delta_max_level
                                     - rfconfig_rx_lna_switch_pt_get_backoff(lna_switch_pt_ptr)
                                     - vga_gain_offset
                                     + (lna_1_offset >> LNA_OFFSET_SHIFT) );
        optimal_fall_val = optimal_rise_val - rfconfig_rx_lna_switch_pt_get_hysteresis(lna_switch_pt_ptr);
        optimal_im_val   = optimal_rise_val;
        /* saturate to 8 bits */
        optimal_rise_val = RF_THRESH_SATURATE(optimal_rise_val, NUM_BITS_TO_LIMIT_RESULT);
        optimal_fall_val = RF_THRESH_SATURATE(optimal_fall_val, NUM_BITS_TO_LIMIT_RESULT);
        optimal_im_val   = RF_THRESH_SATURATE(optimal_im_val,   NUM_BITS_TO_LIMIT_RESULT);
        /* load the new Rise/Fall/IM levels */
        MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_RISE), DATA, optimal_rise_val, (int)path, 2); /*lint !e701*/
        MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_FALL), DATA, optimal_fall_val, (int)path, 2); /*lint !e701*/
        MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_IM_LEVELn),  DATA, optimal_im_val,   (int)path, 2); /*lint !e701*/
 #ifdef RF_HAS_OLTA_DEBUG
#error code not present
 #endif
     }
     /* compute and load optimal LNA RISE/FALL/IM values for 3rd switch point */
     lna_switch_pt_ptr = rfconfig_rx_lna_get_switch_point_info_from_linearity(lna_linearity_info_ptr, 2);
     if ( RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__OLTA == rfconfig_rx_lna_switch_pt_get_config_mode(lna_switch_pt_ptr) )
     {
        optimal_rise_val = (int16) ( sigma_delta_max_level
                                     - rfconfig_rx_lna_switch_pt_get_backoff(lna_switch_pt_ptr)
                                     - vga_gain_offset
                                     + (lna_2_offset >> LNA_OFFSET_SHIFT) );
        optimal_fall_val = optimal_rise_val - rfconfig_rx_lna_switch_pt_get_hysteresis(lna_switch_pt_ptr);
        optimal_im_val   = optimal_rise_val;
        /* saturate to 8 bits */
        optimal_rise_val = RF_THRESH_SATURATE(optimal_rise_val, NUM_BITS_TO_LIMIT_RESULT);
        optimal_fall_val = RF_THRESH_SATURATE(optimal_fall_val, NUM_BITS_TO_LIMIT_RESULT);
        optimal_im_val   = RF_THRESH_SATURATE(optimal_im_val,   NUM_BITS_TO_LIMIT_RESULT);
        /* load the new Rise/Fall/IM levels */
        MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_RISE), DATA, optimal_rise_val, (int)path, 3); /*lint !e701*/
        MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_FALL), DATA, optimal_fall_val, (int)path, 3); /*lint !e701*/
        MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_IM_LEVELn),  DATA, optimal_im_val,   (int)path, 3); /*lint !e701*/
 #ifdef RF_HAS_OLTA_DEBUG
#error code not present
 #endif
     }
     /* compute and load optimal LNA RISE/FALL/IM values for 4th switch point */
     lna_switch_pt_ptr = rfconfig_rx_lna_get_switch_point_info_from_linearity(lna_linearity_info_ptr, 3);
     if ( RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__OLTA == rfconfig_rx_lna_switch_pt_get_config_mode(lna_switch_pt_ptr) )
     {
        optimal_rise_val = (int16) ( sigma_delta_max_level
                                     - rfconfig_rx_lna_switch_pt_get_backoff(lna_switch_pt_ptr)
                                     - vga_gain_offset
                                     + (lna_3_offset >> LNA_OFFSET_SHIFT) );
        optimal_fall_val = optimal_rise_val - rfconfig_rx_lna_switch_pt_get_hysteresis(lna_switch_pt_ptr);
        optimal_im_val   = optimal_rise_val;
        /* saturate to 8 bits */
        optimal_rise_val = RF_THRESH_SATURATE(optimal_rise_val, NUM_BITS_TO_LIMIT_RESULT);
        optimal_fall_val = RF_THRESH_SATURATE(optimal_fall_val, NUM_BITS_TO_LIMIT_RESULT);
        optimal_im_val   = RF_THRESH_SATURATE(optimal_im_val,   NUM_BITS_TO_LIMIT_RESULT);
        /* load the new Rise/Fall/IM levels */
        MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_RISE), DATA, optimal_rise_val, (int)path, 4); /*lint !e701*/
        MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_FALL), DATA, optimal_fall_val, (int)path, 4); /*lint !e701*/
        MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_IM_LEVELn),  DATA, optimal_im_val,   (int)path, 4); /*lint !e701*/
 #ifdef RF_HAS_OLTA_DEBUG
#error code not present
 #endif
     }

   #else /* ! #ifdef RF_HAS_RFCONFIG_RX_LNA */

     int8 *backoff_ptr;

     /* how many counts per dB */
     lsb_per_bit = ( cagc_dynamic_range_setting == RF_HAS_102DB_CDMA_DYNAMIC_RANGE ) ? RX_AGC_VAL_CNTS_PER_DB_102_DB : RX_AGC_VAL_CNTS_PER_DB_85_DB;

     /* calculate VGA gain offset for this frequency in 2/5 dB resolution*/
#ifdef RF_HAS_LINEAR_INTERPOLATION
  #ifdef RF_HAS_POWER_MODE_LIB
     vga_gain_offset =  (int16)(( 
         rf_freq_comp.i_vga_gain_offset_vs_freq[path][power_mode]
         * lsb_per_bit/VGA_GAIN_OFFSET_SCALE_FACTOR )
         >> VGA_GAIN_OFFSET_SHIFT) ;/*lint !e702 */
  #else
     vga_gain_offset =  (int16)(( 
         rf_freq_comp.i_vga_gain_offset_vs_freq[path]
         * lsb_per_bit/VGA_GAIN_OFFSET_SCALE_FACTOR )
         >> VGA_GAIN_OFFSET_SHIFT) ;
  #endif /* RF_HAS_POWER_MODE_LIB */
#else
     vga_gain_offset =  (int16)(((rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_vga_gain_offset_vs_freq[path]
        [ rf_chain_ptr->rf_freq_index ])
        * lsb_per_bit/VGA_GAIN_OFFSET_SCALE_FACTOR )
        >> VGA_GAIN_OFFSET_SHIFT );
#endif

     /* max Sigma Delta level in 2/5 dB resolution*/
     sigma_delta_max_level = (int16)(( (rf_get_max_sigma_delta_level(device) * lsb_per_bit)/SIGMA_DELTA_MAX_LEVEL_SCALE_FACTOR ) >> 2);/*lint !e702 */

#ifdef RF_HAS_POWER_MODE_LIB
  #ifdef RF_HAS_HDR
#error code not present
  #else
        backoff_ptr = &rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_olta_backoff[path][power_mode][0];
  #endif
#else
  #ifdef RF_HAS_HDR
#error code not present
  #else
        backoff_ptr = &rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_olta_backoff[path][0];
  #endif
#endif /* RF_HAS_POWER_MODE_LIB */

     /* LNA RISE/FALL */
     if ( ( backoff_ptr[0] != LNA_THRESHOLD_ADJUST_DISABLE ) && 
#ifdef RF_HAS_POWER_MODE_LIB
          ( rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_1_rise[path][power_mode] != LNA_THRESHOLD_DISABLE ) )
#else
          ( rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_1_rise[path] != LNA_THRESHOLD_DISABLE ) )
#endif /* RF_HAS_POWER_MODE_LIB */
     {

        /* compute optimal rise value */
        optimal_rise_val = (sigma_delta_max_level - (int16)backoff_ptr[0])  - vga_gain_offset;

        /* compute optimal fall value */    
#ifdef RF_HAS_POWER_MODE_LIB
        optimal_fall_val =  optimal_rise_val -
          ( (int16)rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_1_rise[path][power_mode]
             - (int16)rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_1_fall[path][power_mode] );
#else
        optimal_fall_val =  optimal_rise_val -
          ( (int16)rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_1_rise[path]
             - (int16)rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_1_fall[path] );
#endif /* RF_HAS_POWER_MODE_LIB */

        /* compute optimal IM Level */
#ifdef RF_HAS_POWER_MODE_LIB
        optimal_im_val = optimal_rise_val  -
          ( (int16)rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_1_rise[path][power_mode]
             - (int16)rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im_level1[path] );
#else
        optimal_im_val = optimal_rise_val  -
          ( (int16)rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_1_rise[path]
             - (int16)rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im_level1[path] );
#endif /* RF_HAS_POWER_MODE_LIB */
        
        /* saturate to 8 bits */
        optimal_rise_val = RF_THRESH_SATURATE(optimal_rise_val,NUM_BITS_TO_LIMIT_RESULT);
        optimal_fall_val = RF_THRESH_SATURATE(optimal_fall_val, NUM_BITS_TO_LIMIT_RESULT );
        optimal_im_val = RF_THRESH_SATURATE(optimal_im_val, NUM_BITS_TO_LIMIT_RESULT );

        /* load the new Rise/Fall/IM levels */
        MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_RISE), DATA, optimal_rise_val, (int)path, 1 ); /*lint !e701*/
        MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_FALL), DATA, optimal_fall_val, (int)path,1 ); /*lint !e701*/
        MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_IM_LEVELn), DATA, optimal_im_val, (int)path, 1); /*lint !e701*/
     }

#if defined (RF_HAS_HDR)
#error code not present
#endif /* RF_HAS_HDR */

     /* LNA 2 RISE/FALL */
     if ( ( backoff_ptr[1] != LNA_THRESHOLD_ADJUST_DISABLE ) && 
#ifdef RF_HAS_POWER_MODE_LIB
          ( rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_2_rise[path][power_mode] != LNA_THRESHOLD_DISABLE ) )
#else
          ( rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_2_rise[path] != LNA_THRESHOLD_DISABLE ) )
#endif /* RF_HAS_POWER_MODE_LIB */
     {

        /* compute optimal rise value */
        optimal_rise_val =  (int16)(((sigma_delta_max_level - (int16)backoff_ptr[1]) - vga_gain_offset) + (lna_1_offset >> LNA_OFFSET_SHIFT));/*lint !e702 */

        /* compute optimal fall value */    
#ifdef RF_HAS_POWER_MODE_LIB
        optimal_fall_val =  optimal_rise_val - 
          ( (int16)rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_2_rise[path][power_mode]
             - (int16)rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_2_fall[path][power_mode] );
#else
        optimal_fall_val =  optimal_rise_val - 
          ( (int16)rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_2_rise[path]
             - (int16)rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_2_fall[path] );
#endif /* RF_HAS_POWER_MODE_LIB */

        /* compute optimal IM Level */
#ifdef RF_HAS_POWER_MODE_LIB
        optimal_im_val = optimal_rise_val  - 
          ( (int16)rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_2_rise[path][power_mode]
             - (int16)rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im_level2[path] );
#else
        optimal_im_val = optimal_rise_val  - 
          ( (int16)rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_2_rise[path]
             - (int16)rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im_level2[path] );
#endif /* RF_HAS_POWER_MODE_LIB */

        /* saturate to 8 bits */
        optimal_rise_val = RF_THRESH_SATURATE(optimal_rise_val,NUM_BITS_TO_LIMIT_RESULT);
        optimal_fall_val = RF_THRESH_SATURATE(optimal_fall_val, NUM_BITS_TO_LIMIT_RESULT );
        optimal_im_val = RF_THRESH_SATURATE(optimal_im_val, NUM_BITS_TO_LIMIT_RESULT );

        /* load the new Rise/Fall/IM levels */
        MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_RISE), DATA, optimal_rise_val, (int)path, 2 ); /*lint !e701*/
        MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_FALL), DATA, optimal_fall_val, (int)path, 2 ); /*lint !e701*/
        MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_IM_LEVELn), DATA, optimal_im_val, (int)path, 2); /*lint !e701*/
     }

#if defined (RF_HAS_HDR)
#error code not present
#endif /* RF_HAS_HDR */

     /* LNA 3 RISE/FALL */
     if ( ( backoff_ptr[2] != LNA_THRESHOLD_ADJUST_DISABLE ) && 
#ifdef RF_HAS_POWER_MODE_LIB
          ( rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_3_rise[path][power_mode] != LNA_THRESHOLD_DISABLE ) )
#else
          ( rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_3_rise[path] != LNA_THRESHOLD_DISABLE ) )
#endif /* RF_HAS_POWER_MODE_LIB */
     {

        /* compute optimal rise value */
        optimal_rise_val =  (int16)(((sigma_delta_max_level - (int16)backoff_ptr[2]) - vga_gain_offset) + (lna_2_offset >> LNA_OFFSET_SHIFT));/*lint !e702 */

        /* compute optimal fall value */    
#ifdef RF_HAS_POWER_MODE_LIB
        optimal_fall_val =  optimal_rise_val - 
          ( (int16)rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_3_rise[path][power_mode]
             - (int16)rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_3_fall[path][power_mode] );
#else
        optimal_fall_val =  optimal_rise_val - 
          ( (int16)rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_3_rise[path]
             - (int16)rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_3_fall[path] );
#endif /* RF_HAS_POWER_MODE_LIB */

        /* compute optimal IM Level */
#ifdef RF_HAS_POWER_MODE_LIB
        optimal_im_val = optimal_rise_val  - 
          ( (int16)rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_3_rise[path][power_mode]
             - (int16)rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im_level3[path] );
#else
        optimal_im_val = optimal_rise_val  - 
          ( (int16)rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_3_rise[path]
             - (int16)rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im_level3[path] );
#endif /* RF_HAS_POWER_MODE_LIB */

        /* saturate to 8 bits */
        optimal_rise_val = RF_THRESH_SATURATE(optimal_rise_val,NUM_BITS_TO_LIMIT_RESULT);
        optimal_fall_val = RF_THRESH_SATURATE(optimal_fall_val, NUM_BITS_TO_LIMIT_RESULT );
        optimal_im_val = RF_THRESH_SATURATE(optimal_im_val, NUM_BITS_TO_LIMIT_RESULT );

        /* load the new Rise/Fall/IM levels */
        MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_RISE), DATA, optimal_rise_val, (int)path, 3 ); /*lint !e701*/
        MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_FALL), DATA, optimal_fall_val, (int)path, 3 ); /*lint !e701*/
        MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_IM_LEVELn), DATA, optimal_im_val, (int)path, 3); /*lint !e701*/
     }

     /* LNA 4 RISE/FALL */
     if ( ( backoff_ptr[3] != LNA_THRESHOLD_ADJUST_DISABLE ) && 
#ifdef RF_HAS_POWER_MODE_LIB
          ( rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_4_rise[path][power_mode] != LNA_THRESHOLD_DISABLE ) )
#else
          ( rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_4_rise[path] != LNA_THRESHOLD_DISABLE ) )
#endif /* RF_HAS_POWER_MODE_LIB */
     {

        /* compute optimal rise value */
        optimal_rise_val =  (int16)(((sigma_delta_max_level - (int16)backoff_ptr[3]) - vga_gain_offset) + (lna_3_offset >> LNA_OFFSET_SHIFT));/*lint !e702 */

        /* compute optimal fall value */    
#ifdef RF_HAS_POWER_MODE_LIB
        optimal_fall_val =  optimal_rise_val - 
          ( (int16)rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_4_rise[path][power_mode]
             - (int16)rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_4_fall[path][power_mode] );
#else
        optimal_fall_val =  optimal_rise_val - 
          ( (int16)rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_4_rise[path]
             - (int16)rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_4_fall[path] );
#endif /* RF_HAS_POWER_MODE_LIB */

        /* compute optimal IM Level */
#ifdef RF_HAS_POWER_MODE_LIB
        optimal_im_val = optimal_rise_val  - 
          ( (int16)rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_4_rise[path][power_mode]
             - (int16)rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im_level4[path] );
#else
        optimal_im_val = optimal_rise_val  - 
          ( (int16)rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_4_rise[path]
             - (int16)rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im_level4[path] );
#endif /* RF_HAS_POWER_MODE_LIB */

        /* saturate to 8 bits */
        optimal_rise_val = RF_THRESH_SATURATE(optimal_rise_val,NUM_BITS_TO_LIMIT_RESULT);
        optimal_fall_val = RF_THRESH_SATURATE(optimal_fall_val, NUM_BITS_TO_LIMIT_RESULT );
        optimal_im_val = RF_THRESH_SATURATE(optimal_im_val, NUM_BITS_TO_LIMIT_RESULT );

        /* load the new Rise/Fall/IM levels */
        MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_RISE), DATA, optimal_rise_val, (int)path, 4 ); /*lint !e701*/
        MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_LNA_n_FALL), DATA, optimal_fall_val, (int)path, 4 ); /*lint !e701*/
        MSM_W_MASKED_ij( RXF_CDMA(RX_AGCc_IM_LEVELn), DATA, optimal_im_val, (int)path, 4); /*lint !e701*/
     }

   #endif /* RF_HAS_RFCONFIG_RX_LNA */
  }
} /* rf_update_lna_offsets */


/*____________________________________________________________________________
                       PA Control Functions
____________________________________________________________________________*/

#ifndef FEATURE_4PAGE_TX_LINEARIZER
/*===========================================================================
FUNCTION RF_INIT_PA_CTL_1_TXLINZ                                     INTERNAL

DESCRIPTION
  This function will initialize and configure CAGC Tx PA control in MSM3000
backward compatible mode.  Only 1 Tx linearizer table is supported.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

============================================================================*/
void rf_init_pa_ctl_1_txlinz( void )
{
  MSM_W_MASKED(TX_AGC_TEST_POINT_SEL,DATA,0);

  #ifndef RF_HAS_MDSP_TX_AGC
  MSM_W_MASKED(TX_AGC_CTL2, TX_MSM3000_MODE, 0);
  #endif
  /* PP: Disable PA warmup and initialize PA warmup timer to 0. */
  MSM_W_MASKED(PA_WARMUP_TIMER, PA_WARMUPTIMER, 0x0);

  /* PP: PA_S0_OFFSET has to be set to 0 to disable digital adjustment of Tx IF VGA
         control voltage. */

  #ifndef RF_HAS_MDSP_TX_AGC  
  HWIO_OUTMI(PA_Sw_OFFSET, 0, 0x3FF,0x0);
  #endif

  /* PA Mapping disabled, Temporal Hyst enabled
  */
  #ifdef RF_HAS_MDSP_TX_AGC
  CDMA_MDSP_DISABLE_PA_RANGE_MAP();
  #else
  MSM_W_MASKED(TX_AGC_CTL2, PA_R_MAP_EN, CAGC_RF_PA_R_MAP_EN_DIS_V);
  MSM_W_MASKED(TX_AGC_CTL2, TEMPORAL_HYST, CAGC_RF_TEMPORAL_HYST_EN_V);
  #endif

  #ifdef RF_HAS_PA_ACCESS_PROBE_CONFIG
  rf_set_pa_access_probe_config(device,FALSE);
  #endif /*RF_HAS_DIFF_PA_HYST_TIMER_ACC_TRAFFIC*/

  #ifdef RF_HAS_MDSP_TX_AGC
  CDMA_MDSP_SET_PA_RANGE_MAP(RF_CONVERT_1X_HW_TO_MDSP_PA_MAPPING(rf_current_pa_r_map));
  CDMA_MDSP_DISABLE_PA_HYSTERISIS_TIMER();
  #else  /* RF_HAS_MDSP_TX_AGC */
  MSM_W_MASKED ( PA_R_MAP, DATA, rf_current_pa_r_map );
  HWIO_OUTM ( PA_R_TIMER, 0xFF, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_1x_pa_r_timer );
  #endif

#ifndef T_IO_CARD_PANDA
  HWIO_OUTM( AGC_CTL2, CAGC_PAR_POL_M, CAGC_PAR_POL_NORM_V );
#endif /* T_IO_CARD_PANDA */
} /* rf_init_pa_ctl_1_txlinz */

#endif /* !FEATURE_4PAGE_TX_LINEARIZER */


#ifdef FEATURE_4PAGE_TX_LINEARIZER
/*===========================================================================
FUNCTION RF_INIT_PA_CTL_4_TXLINZ                                     INTERNAL

DESCRIPTION
  This function will initialize and configure CAGC Tx PA control in MSM3100
mode.  4 Tx linearizer tables are supported.  It is not a must to utilize
digital compensation although 4 Tx linearizer tables are used.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

============================================================================*/
void rf_init_pa_ctl_4_txlinz( rfcom_device_enum_type device )/*lint -esym(715, device) unused */
{

#ifdef T_IO_CARD_PANDA
#error code not present
#endif
  #ifndef RF_HAS_MDSP_TX_AGC
  rf_chain_state_type *rf_chain_ptr = 
        &rf_chain_status[ rfcom_to_path[ RFCOM_TRANSCEIVER_0 ] ];
  #endif

  MSM_W_MASKED(TX_AGC_TEST_POINT_SEL,DATA,0);
  #ifndef RF_HAS_MDSP_TX_AGC
  MSM_W_MASKED(TX_AGC_CTL2, TX_MSM3000_MODE, 0);
  #endif

  /* PP: Disable PA warmup and initialize PA warmup timer to 0. */

  MSM_W_MASKED(PA_WARMUP_TIMER, PA_WARMUPTIMER, 0x0);

  #ifdef RF_HAS_MDSP_TX_AGC
  CDMA_MDSP_SET_PA_L2H_GAIN_STATE_DLY( CAGC_RF_TX_BLIP_DELAY_13US_V );
  CDMA_MDSP_SET_RF_PN_ROLL_DELAY( 0 );
  #ifdef RF_HAS_MDSP_TX_AGC_SSBI_INTERFACE
  /* Required for SSBI Interface but not for PDM interface */
  CDMA_MDSP_SET_PA_H2L_GAIN_STATE_DLY( CAGC_RF_TX_BLIP_DELAY_13US_V );
  #endif
  #else
  MSM_W_MASKED(TX_AGC_CTL, TX_GAIN_BLIP_DELAY, CAGC_RF_TX_BLIP_DELAY_13US_V);
  #endif /* RF_HAS_MDSP_TX_AGC */

  MSM_W_MASKED(TX_ALIGN_DELAY, DATA, CAGC_RF_TX_ALIGN_DELAY_223CX8_V);

  /* PP: PA_Sn_OFFSET has to be set to 0 to disable digital adjustment of Tx IF VGA
         control voltage in case if digital compensation is not utilized. */
  
  #ifndef RF_HAS_MDSP_TX_AGC /* They will be set to 0 in rf_set_pa_offsets called from rf_init() */  
  HWIO_OUTMI(PA_Sw_OFFSET, 0, 0x3FF,0x0);
  HWIO_OUTMI(PA_Sw_OFFSET, 1, 0x3FF,0x0);
  HWIO_OUTMI(PA_Sw_OFFSET, 2, 0x3FF,0x0);
  HWIO_OUTMI(PA_Sw_OFFSET, 3, 0x3FF,0x0);
  #endif

  /* PA Mapping and Temporal Hyst enabled
  */
  #ifdef RF_HAS_MDSP_TX_AGC
  CDMA_MDSP_ENABLE_PA_RANGE_MAP();
  #else
  MSM_W_MASKED(TX_AGC_CTL2, PA_R_MAP_EN, CAGC_RF_PA_R_MAP_EN_EN_V);
  MSM_W_MASKED(TX_AGC_CTL2, TEMPORAL_HYST, CAGC_RF_TEMPORAL_HYST_EN_V);
  #endif
  /* Use the mapping provided in NV, there 4 PA states supported */
  #ifdef RF_HAS_MDSP_TX_AGC
  CDMA_MDSP_SET_PA_RANGE_MAP(RF_CONVERT_1X_HW_TO_MDSP_PA_MAPPING(((rf_current_pa_r_map) & (0xFF))));
  #ifdef RF_HAS_PA_ACCESS_PROBE_CONFIG
  rf_set_pa_access_probe_config(device,FALSE);
  #endif
  #else
  MSM_W_MASKED ( PA_R_MAP, DATA,  rf_current_pa_r_map );
  HWIO_OUTM ( PA_R_TIMER, 0xFF, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_1x_pa_r_timer );
  #endif

 #ifndef T_IO_CARD_PANDA
 #ifdef RF_HAS_MODULATOR_TX_CTL
  MSM_W_MASKED(MOD_TX_AGC_ADJ_CTL, TX_AGC_ADJ_POLARITY, CAGC_RF_TX_AGC_POL_INV_V);
 #else
  MSM_W_MASKED(TX_AGC_CTL, TX_AGC_ADJ_POLARITY, CAGC_RF_TX_AGC_POL_INV_V);
 #endif
#else
#error code not present
#endif /* T_IO_CARD_PANDA */
} /* rf_init_pa_ctl_4_txlinz */

#endif /* FEATURE_4PAGE_TX_LINEARIZER */


/*===========================================================================
FUNCTION RF_DISABLE_PA_CTL                                          INTERNAL

DESCRIPTION
  This function will disable PA state control and force the state to operate
in idle mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

============================================================================*/
void rf_disable_pa_ctl( rfcom_device_enum_type device )/*lint -esym(754, device) unused */
{

#ifdef T_IO_CARD_PANDA
#error code not present
#endif
  /* enable CAGC PA range control from CPU and set R1,R0 to 1,1
  */
#ifndef T_IO_CARD_PANDA
  #ifdef RF_HAS_MDSP_TX_AGC
  CDMA_MDSP_SET_PA_R0_R1_OVERRIDE_VAL(CAGC_RF_PAR_00_V);
  CDMA_MDSP_ENABLE_PA_RANGE_OVERRIDE();
  #else
  MSM_W_MASKED(TX_AGC_CTL, PA_R_OVERRIDE_N, CAGC_RF_PAR_CTL_OVERRIDE_AGC_V);
  #endif
#else
#error code not present
#endif /* T_IO_CARD_PANDA */

#ifndef T_IO_CARD_GPS_PBS
  /* Set PA pins low for current savings */
  #ifndef RF_HAS_MDSP_TX_AGC   
  MSM_W_MASKED( TX_AGC_CTL, PA_R_OVERRIDE_N, 0);
  MSM_W_MASKED( TX_AGC_CTL, PA_R_WR, CAGC_RF_PAR_00_V );
  #endif
#endif /*T_IO_CARD_GPS_PBS*/

} /* rf_disable_pa_ctl */


/*===========================================================================
FUNCTION RF_ENABLE_PA_CTL                                           INTERNAL

DESCRIPTION
  This function will enable PA state control.  This is only for CDMA mode.
  There is no hardware PA state control machine in FM mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  rf_r[123]_rise_val and rf_r[123]_fall_val will be initialized.

============================================================================*/
void rf_enable_pa_ctl( rfcom_device_enum_type device )/*lint -esym(715, device) unused */
{

#ifdef T_IO_CARD_PANDA
#error code not present
#endif

  /* Enable CAGC PA range control from CAGC internal hardware
  ** Reload CAGC PA range control 1,2,3 rise,fall thresholds
  */
  rf_reset_pa_thres();
  rf_load_pa_thres();
  #ifdef RF_HAS_MDSP_TX_AGC
  CDMA_MDSP_NEW_PA_THRESH_SLOT_PARAMS_AVAILABLE();
  #endif  

#ifdef FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST
  pa_thres_adjusted = FALSE;
  curr_offset1 = 0;
  curr_offset2 = 0;
  curr_offset3 = 0;
#endif

#ifndef T_IO_CARD_PANDA
   #ifdef RF_HAS_MDSP_TX_AGC
   CDMA_MDSP_SET_PA_STATE_NORMAL();
   CDMA_MDSP_SET_PA_RANGE_NORMAL();
   #else
  MSM_W_MASKED(TX_AGC_CTL, PA_R_OVERRIDE_N, CAGC_RF_PAR_CTL_NONOVERRIDE_AGC_V);
   #endif
#else
#error code not present
#endif /* T_IO_CARD_PANDA */
} /* rf_enable_pa_ctl */


/*===========================================================================
FUNCTION RF_LOAD_PA_THRES                                         INTERNAL

DESCRIPTION
  This function will update PA rise/fall values.  Only useful for CDMA mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

============================================================================*/
void rf_load_pa_thres( void )
{
  #ifdef RF_HAS_MDSP_TX_AGC
  
  #if defined(RF_HAS_BETA_SCALING_TX_AGC)
  int2 pa_rise_val[RF_NUM_TX_LINEARIZERS - 1];
  int2 rf_pa_fall_val;
  int tx_max_pwr;
  int tx_min_pwr;
  int2 temp_level_sat;
  byte i;
  #ifdef RF_HAS_SC2X_BETA_SCALING_DEBUG  
#error code not present
  #endif
  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ rfcom_to_path[ RFCOM_TRANSCEIVER_0 ] ];
  
  pa_rise_val[0] = rf_r1_rise_val;

  pa_rise_val[1] = rf_r2_rise_val;

  pa_rise_val[2] = rf_r3_rise_val;
  
  rf_get_tx_min_max_pwr( rf_chain_ptr->rf_curr_cdma_band,
                         &tx_min_pwr, &tx_max_pwr );

  for(i=0;i<RF_NUM_TX_LINEARIZERS - 1;i++) 
  {
    if (pa_rise_val[i] != RF_CDMA_MDSP_PA_SWITCHPOINT_INVALID_VAL)
    {    
      #ifdef RF_HAS_LINEAR_INTERPOLATION
      temp_level_sat = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_tx_lin_tbl[i][0] + rf_freq_comp.i_lin_freq_comp[i];
      #else
      temp_level_sat = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_tx_lin_tbl[i][0] + 
                      ((int2) rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_tx_comp_vs_freq[i][rf_chain_ptr->rf_freq_index]);
      #endif
      temp_level_sat =  ((temp_level_sat - tx_min_pwr) >> 2) - 1;

      pa_rise_val[i] = RF_BETA_SCALING_MIN_VALUE(temp_level_sat, pa_rise_val[i]); 

    }
  }
  CDMA_MDSP_SET_PA_R1_RISE(RF_CDMA_CONVERT_PA_THRESHOLD_VAL( pa_rise_val[0]));

  rf_pa_fall_val = pa_rise_val[0] - (rf_r1_rise_val - rf_r1_fall_val);

  CDMA_MDSP_SET_PA_R1_FALL(RF_CDMA_CONVERT_PA_THRESHOLD_VAL( rf_pa_fall_val ));

  #ifdef RF_HAS_SC2X_BETA_SCALING_DEBUG
#error code not present
  #endif

  CDMA_MDSP_SET_PA_R2_RISE(RF_CDMA_CONVERT_PA_THRESHOLD_VAL( pa_rise_val[1]));

  rf_pa_fall_val = pa_rise_val[1] - (rf_r2_rise_val - rf_r2_fall_val);

  CDMA_MDSP_SET_PA_R2_FALL(RF_CDMA_CONVERT_PA_THRESHOLD_VAL( rf_pa_fall_val ));

  #ifdef RF_HAS_SC2X_BETA_SCALING_DEBUG
#error code not present
  #endif

  CDMA_MDSP_SET_PA_R3_RISE(RF_CDMA_CONVERT_PA_THRESHOLD_VAL( pa_rise_val[2]));

  rf_pa_fall_val = pa_rise_val[2] - (rf_r3_rise_val - rf_r3_fall_val);

  CDMA_MDSP_SET_PA_R3_FALL(RF_CDMA_CONVERT_PA_THRESHOLD_VAL( rf_pa_fall_val ));

  #ifdef RF_HAS_SC2X_BETA_SCALING_DEBUG
#error code not present
  #endif

  #else
  
  CDMA_MDSP_SET_PA_R1_RISE(  
   RF_CDMA_CONVERT_PA_THRESHOLD_VAL( rf_r1_rise_val));

  CDMA_MDSP_SET_PA_R2_RISE(  
   RF_CDMA_CONVERT_PA_THRESHOLD_VAL( rf_r2_rise_val ));

  CDMA_MDSP_SET_PA_R3_RISE(    
   RF_CDMA_CONVERT_PA_THRESHOLD_VAL( rf_r3_rise_val ));

  CDMA_MDSP_SET_PA_R1_FALL(  
    RF_CDMA_CONVERT_PA_THRESHOLD_VAL( rf_r1_fall_val ));

  CDMA_MDSP_SET_PA_R2_FALL(  
    RF_CDMA_CONVERT_PA_THRESHOLD_VAL(rf_r2_fall_val ));

  CDMA_MDSP_SET_PA_R3_FALL(  
   RF_CDMA_CONVERT_PA_THRESHOLD_VAL( rf_r3_fall_val ));
  #endif
   #else
  HWIO_OUTM( PA_R1_RISE, 0xFF, ((byte) RF_PA_RANGE_REG_MAX - rf_r1_rise_val) );
  HWIO_OUTM( PA_R1_FALL, 0xFF, ((byte) RF_PA_RANGE_REG_MAX - rf_r1_fall_val) );

  HWIO_OUTM( PA_R2_RISE, 0xFF, ((byte) RF_PA_RANGE_REG_MAX - rf_r2_rise_val) );
  HWIO_OUTM( PA_R2_FALL, 0xFF, ((byte) RF_PA_RANGE_REG_MAX - rf_r2_fall_val) );

  HWIO_OUTM( PA_R3_RISE, 0xFF, ((byte) RF_PA_RANGE_REG_MAX - rf_r3_rise_val) );
  HWIO_OUTM( PA_R3_FALL, 0xFF, ((byte) RF_PA_RANGE_REG_MAX - rf_r3_fall_val) );

  MSG_HIGH ("1X PA_R1_RISE: %d,PA_R1_FALL: %d",rf_r1_rise_val,rf_r1_fall_val,0);
  MSG_HIGH ("1X PA_R2_RISE: %d,PA_R2_FALL: %d",rf_r2_rise_val,rf_r2_fall_val,0);
  MSG_HIGH ("1X PA_R3_RISE: %d,PA_R3_FALL: %d",rf_r3_rise_val,rf_r3_fall_val,0);

  #endif /* RF_HAS_MDSP_TX_AGC */
} /* rf_load_pa_thres */


/*===========================================================================
FUNCTION RF_RESET_PA_THRES                                         INTERNAL

DESCRIPTION
  This function will reset rf_r[123]_rise_val and rf_r[123]_fall_val to the
NV configured rise/fall values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  rf_r[123]_rise_val and rf_r[123]_fall_val will be overwritten.


============================================================================*/
void rf_reset_pa_thres( void )
{
  #ifdef RF_HAS_1X_PA_SWITCH_POINTS_DIFF_FROM_NV
  int8 rf_1X_pa_rise_fall_diff_val[RF_NUM_TX_LINEARIZERS - 1];
  #endif
  #if defined(RF_HAS_PA_ACCESS_PROBE_CONFIG) || defined (RF_HAS_1X_PA_SWITCH_POINTS_DIFF_FROM_NV)
  int i;
  int8 rf_1X_pa_rise_fall_diff_val_calc;
  pa_switchpoint_type max_pa_switchpoint;
  int2 pa_rise_val[RF_NUM_TX_LINEARIZERS - 1];
  int2 rf_pa_fall_val[RF_NUM_TX_LINEARIZERS - 1];
  int2 pa_offset = 0;
  #endif
  
  rf_chain_state_type *rf_chain_ptr = 
        &rf_chain_status[ rfcom_to_path[ RFCOM_TRANSCEIVER_0 ] ];

  /* Initialize the calibration values for PA rise/fall
  */

  rf_r1_rise_val = (int2) ((word) rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_r1_rise);

  rf_r1_fall_val = (int2) ((word) rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_r1_fall);

  rf_r2_rise_val = (int2) ((word) rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_r2_rise);

  rf_r2_fall_val = (int2) ((word) rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_r2_fall);

  rf_r3_rise_val = (int2) ((word) rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_r3_rise);

  rf_r3_fall_val = (int2) ((word) rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_r3_fall);

  #if defined(RF_HAS_1X_PA_SWITCH_POINTS_DIFF_FROM_NV) || defined(RF_HAS_PA_ACCESS_PROBE_CONFIG)
  if(rft_has_pa_switchpoints_diff_from_NV() == TRUE)
  {  
    max_pa_switchpoint = rf_cagc_get_max_active_pa_switchpoint();

    if(max_pa_switchpoint == INVALID_PA_SWITCHPOINT)
    {
      RF_FATAL_ERROR("No PA switchpoint active");
      return;
    }

    pa_rise_val[0] = rf_r1_rise_val;
    pa_rise_val[1] = rf_r2_rise_val;
    pa_rise_val[2] = rf_r3_rise_val; 

    rf_pa_fall_val[0] = rf_r1_fall_val;
    rf_pa_fall_val[1] = rf_r2_fall_val;
    rf_pa_fall_val[2] = rf_r3_fall_val;

        
    #ifdef RF_HAS_1X_PA_SWITCH_POINTS_DIFF_FROM_NV
    for (i=0;i < RF_NUM_TX_LINEARIZERS - 1 ; i++) 
    {
      rf_1X_pa_rise_fall_diff_val[i] = 0; 
    }
    rft_get_1X_pa_rise_fall_diff_val(&rf_1X_pa_rise_fall_diff_val[0]);
    #endif

    #ifdef RF_HAS_PA_ACCESS_PROBE_CONFIG
    #ifdef RF_HAS_HDR
#error code not present
    #else
    if (rf_pa_config_is_traffic_state == FALSE)
    #endif
    {                              
      pa_offset = rft_get_1X_access_probe_pa_backoff(RFCOM_TRANSCEIVER_0,rf_chain_ptr->rf_curr_cdma_band); 
    }
    #endif /*RF_HAS_PA_ACCESS_PROBE_CONFIG*/

    if (max_pa_switchpoint >= PA_SWITCHPOINT_MAX) 
    {
        MSG_FATAL("invalid max_pa_switchpoint", 0, 0, 0);
        rf_error_flag = TRUE;
        return;
    }

    for (i= max_pa_switchpoint - 1; i >= 0; i--) 
    { 
      rf_1X_pa_rise_fall_diff_val_calc = pa_rise_val[i] - rf_pa_fall_val[i];
            
      if(i != (max_pa_switchpoint - 1))
      {      
        if (pa_rise_val[i]!= RF_PA_RN_MAX)
        {
          if((pa_rise_val[i] + 10 ) > rf_pa_fall_val[i + 1])
          {    
            pa_rise_val[i] =  rf_pa_fall_val[i + 1] - 10;
          }
        }
      }      

      if(i == (max_pa_switchpoint - 1))
      {      
        if (pa_rise_val[i] != RF_PA_RN_MAX) 
        {
          pa_rise_val[i] = pa_rise_val[i] - pa_offset;
        }
      }

     pa_rise_val[i] = RF_SATURATE(  pa_rise_val[i], RF_PA_RN_MIN, RF_PA_RN_MAX );

     #ifdef RF_HAS_1X_PA_SWITCH_POINTS_DIFF_FROM_NV
     if (rf_pa_fall_val[i] != RF_PA_RN_MAX) 
     {    
       rf_1X_pa_rise_fall_diff_val_calc = MAX(rf_1X_pa_rise_fall_diff_val[i],rf_1X_pa_rise_fall_diff_val_calc);    
     }  
     #endif
     if (rf_pa_fall_val[i] != RF_PA_RN_MAX) 
     {
       rf_pa_fall_val[i] =  pa_rise_val[i] - rf_1X_pa_rise_fall_diff_val_calc;
     }
     rf_pa_fall_val[i] =  RF_SATURATE( rf_pa_fall_val[i], RF_PA_RN_MIN, RF_PA_RN_MAX );
    }

    rf_r1_rise_val =pa_rise_val[0];
    rf_r2_rise_val =pa_rise_val[1];
    rf_r3_rise_val =pa_rise_val[2];

    rf_r1_fall_val = rf_pa_fall_val[0];
    rf_r2_fall_val = rf_pa_fall_val[1];
    rf_r3_fall_val = rf_pa_fall_val[2];
  }
  #endif
} /* rf_reset_pa_thres */


/*===========================================================================
FUNCTION RF_CAGC_GET_MAX_ACTIVE_PA_SWITCHPOINT                                    INTERNAL

DESCRIPTION
  This function returns the maximum PA switchpoint active

DEPENDENCIES
  rf_r[123]_rise_val and rf_r[123]_fall_val must be updated.

RETURN VALUE
  Max active PA switchpoint.

SIDE EFFECTS
  

============================================================================*/
pa_switchpoint_type rf_cagc_get_max_active_pa_switchpoint(void)
{

   rf_chain_state_type *rf_chain_ptr = 
         &rf_chain_status[ rfcom_to_path[ RFCOM_TRANSCEIVER_0 ] ];

   if (rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_r3_rise != RF_PA_RISE_DIS_V )
   {
      return PA_SWITCHPOINT_3;
   }
   else if (rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_r2_rise != RF_PA_RISE_DIS_V )
   {
      return PA_SWITCHPOINT_2;
   }
   else if (rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_r1_rise != RF_PA_RISE_DIS_V )
   {
      return PA_SWITCHPOINT_1;
   }
   else
   {
      RF_FATAL_ERROR("No PA switchpoint active");
      return INVALID_PA_SWITCHPOINT;
   }
} /* rf_get_max_active_pa_switchpoint */

/*===========================================================================
FUNCTION RF_ADJUST_PA_THRES                                         INTERNAL

DESCRIPTION
  This function will adjust all rf_r[123]_rise_val and rf_r[123]_fall_val by
a given amount.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  rf_r[123]_rise_val and rf_r[123]_fall_val will be updated.

============================================================================*/
void rf_adjust_pa_thres( int2 offset1, int2 offset2, int2 offset3 )
{
   
  #ifdef RF_HAS_1X_PA_SWITCH_POINTS_DIFF_FROM_NV
  int8 rf_1X_pa_rise_fall_diff_val[RF_NUM_TX_LINEARIZERS - 1];
  #endif
  #if defined(RF_HAS_PA_ACCESS_PROBE_CONFIG) || defined (RF_HAS_1X_PA_SWITCH_POINTS_DIFF_FROM_NV)
  int i;
  int8 rf_1X_pa_rise_fall_diff_val_calc;
  pa_switchpoint_type max_pa_switchpoint;
  int2 pa_rise_val[RF_NUM_TX_LINEARIZERS - 1];
  int2 rf_pa_fall_val[RF_NUM_TX_LINEARIZERS - 1];
  int2 pa_offset = 0;
  int2 offset_val[RF_NUM_TX_LINEARIZERS - 1];  
  #endif

  #ifdef RF_HAS_PA_ACCESS_PROBE_CONFIG  
  rf_chain_state_type *rf_chain_ptr = 
          &rf_chain_status[ rfcom_to_path[ RFCOM_TRANSCEIVER_0 ] ];
  #endif /*RF_HAS_PA_ACCESS_PROBE_CONFIG*/
  
    /* Initialize the calibration values for PA rise/fall
    */
   #if defined(RF_HAS_1X_PA_SWITCH_POINTS_DIFF_FROM_NV) || defined(RF_HAS_PA_ACCESS_PROBE_CONFIG)

  if(rft_has_pa_switchpoints_diff_from_NV() == TRUE)
  {   
    #ifdef RF_HAS_PA_ACCESS_PROBE_CONFIG
    if (rf_pa_config_is_traffic_state == FALSE)    
    {                              
      return;
    }
    #endif /*RF_HAS_PA_ACCESS_PROBE_CONFIG*/

    max_pa_switchpoint = rf_cagc_get_max_active_pa_switchpoint();

    if(max_pa_switchpoint == INVALID_PA_SWITCHPOINT)
    {
      RF_FATAL_ERROR("No PA switchpoint active");
      return;
    }
    pa_rise_val[0] = rf_r1_rise_val;
    pa_rise_val[1] = rf_r2_rise_val;
    pa_rise_val[2] = rf_r3_rise_val; 

    rf_pa_fall_val[0] = rf_r1_fall_val;
    rf_pa_fall_val[1] = rf_r2_fall_val;
    rf_pa_fall_val[2] = rf_r3_fall_val;

    offset_val[0]= offset1;
    offset_val[1]= offset2;
    offset_val[2]= offset3;


    #ifdef RF_HAS_1X_PA_SWITCH_POINTS_DIFF_FROM_NV
    for (i=0;i < RF_NUM_TX_LINEARIZERS - 1 ; i++) 
    {
      rf_1X_pa_rise_fall_diff_val[i] = 0; 
    }
    rft_get_1X_pa_rise_fall_diff_val(&rf_1X_pa_rise_fall_diff_val[0]);
    #endif

    #ifdef RF_HAS_PA_ACCESS_PROBE_CONFIG
    #ifdef RF_HAS_HDR
#error code not present
    #else
    if (rf_pa_config_is_traffic_state == FALSE)
    #endif
    {                              
      pa_offset = rft_get_1X_access_probe_pa_backoff(RFCOM_TRANSCEIVER_0,rf_chain_ptr->rf_curr_cdma_band); 
    }
    #endif /*RF_HAS_PA_ACCESS_PROBE_CONFIG*/

    for (i= max_pa_switchpoint - 1; i >= 0; i--) 
    { 
      rf_1X_pa_rise_fall_diff_val_calc = pa_rise_val[i] - rf_pa_fall_val[i];

      if (pa_rise_val[i] != RF_PA_RN_MAX) 
      {
        if(i == (max_pa_switchpoint - 1))
        {        
          pa_rise_val[i] = pa_rise_val[i] - pa_offset + offset_val[i];
        }
        else
        {
          pa_rise_val[i] = pa_rise_val[i] + offset_val[i];
        }

        #ifdef RF_HAS_PA_ACCESS_PROBE_CONFIG
        #ifdef RF_HAS_HDR
#error code not present
        #else
        if (rf_pa_config_is_traffic_state == FALSE)
        #endif
        {                              
         pa_rise_val[i] = pa_rise_val[i] - offset_val[i];
        }
        #endif
      }     
      if(i != (max_pa_switchpoint - 1))
      {      
        if (pa_rise_val[i]!= RF_PA_RN_MAX)
        {
          if((pa_rise_val[i] + 10 ) > rf_pa_fall_val[i + 1])
          {    
            pa_rise_val[i] =  rf_pa_fall_val[i + 1] - 10;
          }
        }
      }      

     pa_rise_val[i] = RF_SATURATE(  pa_rise_val[i], RF_PA_RN_MIN, RF_PA_RN_MAX );

     #ifdef RF_HAS_1X_PA_SWITCH_POINTS_DIFF_FROM_NV
     if (rf_pa_fall_val[i] != RF_PA_RN_MAX) 
     {    
       rf_1X_pa_rise_fall_diff_val_calc = MAX(rf_1X_pa_rise_fall_diff_val[i],rf_1X_pa_rise_fall_diff_val_calc);    
     }  
     #endif
     if (rf_pa_fall_val[i] != RF_PA_RN_MAX) 
     {
       rf_pa_fall_val[i] =  pa_rise_val[i] - rf_1X_pa_rise_fall_diff_val_calc;
     }
     rf_pa_fall_val[i] =  RF_SATURATE( rf_pa_fall_val[i], RF_PA_RN_MIN, RF_PA_RN_MAX );
    }

    rf_r1_rise_val =pa_rise_val[0];
    rf_r2_rise_val =pa_rise_val[1];
    rf_r3_rise_val =pa_rise_val[2];

    rf_r1_fall_val = rf_pa_fall_val[0];
    rf_r2_fall_val = rf_pa_fall_val[1];
    rf_r3_fall_val = rf_pa_fall_val[2];
  }
  else
  {
    rf_adjust_pa_thres_legacy(offset1);
  }
  #else
  rf_adjust_pa_thres_legacy(offset1);  
  #endif

/*  #ifdef RF_HAS_PA_ACCESS_PROBE_CONFIG
  if (rf_pa_config_is_traffic_state == TRUE)
  {
    if ( rf_r1_rise_val != RF_PA_RISE_DIS_V ) rf_r1_rise_val += offset1;
    if ( rf_r1_fall_val != RF_PA_FALL_DIS_V ) rf_r1_fall_val += offset1;

    if ( rf_r2_rise_val != RF_PA_RISE_DIS_V ) rf_r2_rise_val += offset2;
    if ( rf_r2_fall_val != RF_PA_FALL_DIS_V ) rf_r2_fall_val += offset2;

    if ( rf_r3_rise_val != RF_PA_RISE_DIS_V ) rf_r3_rise_val += offset3;
    if ( rf_r3_fall_val != RF_PA_FALL_DIS_V ) rf_r3_fall_val += offset3;

  }
  #else
  if ( rf_r1_rise_val != RF_PA_RISE_DIS_V ) rf_r1_rise_val += offset1;
  if ( rf_r1_fall_val != RF_PA_FALL_DIS_V ) rf_r1_fall_val += offset1;

  if ( rf_r2_rise_val != RF_PA_RISE_DIS_V ) rf_r2_rise_val += offset2;
  if ( rf_r2_fall_val != RF_PA_FALL_DIS_V ) rf_r2_fall_val += offset2;

  if ( rf_r3_rise_val != RF_PA_RISE_DIS_V ) rf_r3_rise_val += offset3;
  if ( rf_r3_fall_val != RF_PA_FALL_DIS_V ) rf_r3_fall_val += offset3;
  #endif*/

  /* Saturate/limit the new CAGC PA range control thresholds
  */
  /*rf_r1_rise_val = RF_SATURATE( rf_r1_rise_val, RF_PA_RN_MIN, RF_PA_RN_MAX );
  rf_r1_fall_val = RF_SATURATE( rf_r1_fall_val, RF_PA_RN_MIN, RF_PA_RN_MAX );

  rf_r2_rise_val = RF_SATURATE( rf_r2_rise_val, RF_PA_RN_MIN, RF_PA_RN_MAX );
  rf_r2_fall_val = RF_SATURATE( rf_r2_fall_val, RF_PA_RN_MIN, RF_PA_RN_MAX );

  rf_r3_rise_val = RF_SATURATE( rf_r3_rise_val, RF_PA_RN_MIN, RF_PA_RN_MAX );
  rf_r3_fall_val = RF_SATURATE( rf_r3_fall_val, RF_PA_RN_MIN, RF_PA_RN_MAX );*/
} /* rf_adjust_pa_thres */


/*===========================================================================
FUNCTION RF_ADJUST_PA_THRES_LEGACY                                        INTERNAL

DESCRIPTION
  This function will adjust all rf_r[123]_rise_val and rf_r[123]_fall_val by
a given amount.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  rf_r[123]_rise_val and rf_r[123]_fall_val will be updated.

============================================================================*/
void rf_adjust_pa_thres_legacy( int2 offset )
{

    rf_r1_rise_val += offset;
    rf_r1_fall_val += offset;

      /* only adjust r1_rise and r1_fall since there is only 2 states in the
      PA state machine */

   //rf_r2_rise_val += offset;
   //rf_r2_fall_val += offset;

   //rf_r3_rise_val += offset;
   //rf_r3_fall_val += offset;

    rf_r1_rise_val = RF_SATURATE( rf_r1_rise_val, RF_PA_RN_MIN, RF_PA_RN_MAX );
    rf_r1_fall_val = RF_SATURATE( rf_r1_fall_val, RF_PA_RN_MIN, RF_PA_RN_MAX );

    rf_r2_rise_val = RF_SATURATE( rf_r2_rise_val, RF_PA_RN_MIN, RF_PA_RN_MAX );
    rf_r2_fall_val = RF_SATURATE( rf_r2_fall_val, RF_PA_RN_MIN, RF_PA_RN_MAX );

    rf_r3_rise_val = RF_SATURATE( rf_r3_rise_val, RF_PA_RN_MIN, RF_PA_RN_MAX );
    rf_r3_fall_val = RF_SATURATE( rf_r3_fall_val, RF_PA_RN_MIN, RF_PA_RN_MAX );
}

/*===========================================================================
FUNCTION rf_cagc_adjust_max_pwr_pa_thres                             INTERNAL

DESCRIPTION
  This function will adjust max power rf_r[]_rise_val and rf_r[]_fall_val by
a given amount.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  rf_r[]_rise_val and rf_r[]_fall_val will be updated.

============================================================================*/
void rf_cagc_adjust_max_pwr_pa_thres( int2 offset )
{
   pa_switchpoint_type max_pwr_pa_switchpoint;

   max_pwr_pa_switchpoint = rf_cagc_get_max_active_pa_switchpoint();
   
   switch (max_pwr_pa_switchpoint)
   {
   case PA_SWITCHPOINT_1:
     rf_adjust_pa_thres( offset, 0, 0 );
     break;
   case  PA_SWITCHPOINT_2:     
     rf_adjust_pa_thres( 0, offset, 0 );
     break;
   case  PA_SWITCHPOINT_3:     
     rf_adjust_pa_thres( 0, 0, offset );
     break;
   default:
     RF_FATAL_ERROR("Invalid PA switchpoint");
     break;
   };
   return;
}


/*===========================================================================

FUNCTION RF_SET_PA_OFFSETS                                  INTERNAL FUNCTION

DESCRIPTION
  This function sets the PA OFFSET registers to their correct values based on
  current temperature and frequency settings.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rf_set_pa_offsets( rfcom_device_enum_type device )
{
  int i;
  switch (device) 
  {
  case RFCOM_TRANSCEIVER_0:
    for (i=0; i<CAGC_NUM_PA_OFFSETS; i++)
    {
      #ifdef RF_HAS_MDSP_TX_AGC
      #ifdef RF_HAS_TX_DAC_BACKOFF_DIFF_FROM_LEGACY
      if(i == 0)
      {      
        CDMA_MDSP_SET_PA_OFFSET(i,CAGC_PA_OFFSET_0_DB);      
      }
      else
      {      
        CDMA_MDSP_SET_PA_OFFSET(i,CAGC_PA_OFFSET_0_DB);
      }
      #else
      CDMA_MDSP_SET_PA_OFFSET(i, 
             RF_CDMA_MDSP_10_TO_16_BIT(cagc_pa_offsets[i]));
      #endif
      #else
      MSM_W_MASKED_i(PA_Sw_OFFSET, DATA, (word)cagc_pa_offsets[i], i ); /*lint !e737 loss of sign- MSM_ macro*/
      #endif
    }
    break;

  default:
    MSG_ERROR("Invalid device",0,0,0);
    break;
   }

} /* end of rf_set_pa_offsets */


/*________________________________
  End of PA Control Functions
  ________________________________*/



#if !defined(RF_HAS_HDET_BYPASS)

/*===========================================================================

FUNCTION RF_LOAD_CDMA_EXP_AGC_TABLE                         INTERNAL FUNCTION

DESCRIPTION
  Calculates 256 points for the rf_cdma_exp_agc_table[] by interpolating
  the 16+1 point table rf_cdma_exp_hdet_vs_agc[].

DEPENDENCIES
  rf_cdma_exp_hdet_vs_agc[] must have been loaded from NV storage.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rf_load_cdma_exp_agc_table
(
rfnv_digital_items_type *rfnv_digital_ptr
)

{
  word hdet_idx;    /* loop counter, counting hdet table indices    */
  word zone;        /* zone (index in array) for interpolation      */
  word low_zone;    /* Low  zone (index in array) for interpolation */
  word high_zone;   /* High zone (index in array) for interpolation */
  word delta;       /* offset into zone                             */
  word offset;      /* HDET offset                                  */
  word rise;        /* HDET rise                                    */
  word result;      /* scratch pad for calculating result           */
  word i;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Find the base of the expected HDET curve.  The curve is flat
  ** at the top and bottom, and sloping in the middle.
  **
  ** note: RF Cal makes sure that the points below the curve all
  ** have the same value.
  */
  low_zone = 0;
  for ( i = 1; i < NV_CDMA_EXP_HDET_VS_AGC_SIZ; i++ ) {
    
    if ( rfnv_digital_ptr->rfnv_cdma_exp_hdet_vs_agc[ i ] ==
           rfnv_digital_ptr->rfnv_cdma_exp_hdet_vs_agc[ 0 ] )
      low_zone = i;    
    else
      break;
  }

  /* Find the stop of the expected HDET curve.  The curve is flat
  ** at the top and bottom, and sloping in the middle.
  **
  ** note: RF Cal makes sure that the points above the curve all
  ** have the same value.
  */
  high_zone = RF_CDMA_EXP_HDET_VS_AGC_MAX_IDX;
  for ( i = 1; i < RF_CDMA_EXP_HDET_VS_AGC_MAX_IDX; i++ ) {
    
    if ( rfnv_digital_ptr->rfnv_cdma_exp_hdet_vs_agc[ i ] >=
         rfnv_digital_ptr-> \
             rfnv_cdma_exp_hdet_vs_agc[ RF_CDMA_EXP_HDET_VS_AGC_MAX_IDX ] ) {   

      high_zone = i - 1;
      break;
    }
  }

  for (hdet_idx = 0; hdet_idx < RF_TX_AGC_VS_HDET_TABLE_SIZ ; hdet_idx++) {

    /* Calculate the zone (segment) to use for interpolation */

    /* If below the curve
    */
    if ( hdet_idx
       <= (word) rfnv_digital_ptr->rfnv_cdma_exp_hdet_vs_agc[ 0 ] ) {

      zone = low_zone;

    /* Else if above the curve
    */
    } else
    if ( hdet_idx >=
        (word) rfnv_digital_ptr-> \
          rfnv_cdma_exp_hdet_vs_agc[ RF_CDMA_EXP_HDET_VS_AGC_MAX_IDX ] ) {

      zone = high_zone;

    /* Else, on the curve
    */
    } else {

      /* Find hdet_idx in rf_cdma_exp_hdet_vs_agc[]
      */
      zone = 1;
      for ( i = 1; i < RF_CDMA_EXP_HDET_VS_AGC_MAX_IDX; i++ ) {
        if ( hdet_idx >= rfnv_digital_ptr->rfnv_cdma_exp_hdet_vs_agc[ i ] )
          zone = i;
        else
          break;
      }
    }

    /* Calculate the delta from the lower edge of this zone
    */
    if ( hdet_idx <=
           (word)rfnv_digital_ptr->rfnv_cdma_exp_hdet_vs_agc[ zone ] ) {
      delta = 0;
    } else {
      delta = hdet_idx
            - (word) rfnv_digital_ptr->rfnv_cdma_exp_hdet_vs_agc[ zone ];
    }

    /* Calculate the AGC offset
    */
    offset = (word)(zone * RF_EXP_HDET_SIZ);

    /* Calculate the rise for this segment
    */
    rise = rfnv_digital_ptr->rfnv_cdma_exp_hdet_vs_agc[ zone + 1 ]
         - rfnv_digital_ptr->rfnv_cdma_exp_hdet_vs_agc[ zone     ];

    /* Calculate the Tx AGC value from the length, slope, and intercept
    **
    ** If rise is 0, just use the offset, because the slope is 0.  Use
    ** rounding for the divide to get 1/2 LSB better accuracy (~1/12 dB).
    */
    result = offset;
    if ( rise != 0 ) {
      result += (word)(( ( delta * RF_EXP_HDET_SIZ ) + rise / 2 ) / rise);
    }

   /* Saturate the result and store it in the table for later use.
    */
    rf_exp_agc_table_ptr[ hdet_idx ]
      = (byte)(RF_SATURATE( result,
                       RF_TX_AGC_MIN,
                         ( RF_TX_AGC_MAX - RF_TX_AGC_OFF ) ));
  }
} /* rf_load_cdma_exp_agc_table */
#endif /* RF_HAS_HDET_BYPASS */


/*===========================================================================

FUNCTION RF_FREEZE_CAGC                                 INTERNAL FUNCTION

DESCRIPTION
This function is called before writing the RAS RAM linearizer tables. This will
a eliminate any errors in the TX_AGC_ADJ and any Rx power gain resulting from
LNA gain offset during the loading of the RAS RAM linearizer tables.

DEPENDENCIES
  This function is called during a INTLOCK_SAV call.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void rf_freeze_cagc ( void )
{

#ifdef FEATURE_4PAGE_TX_LINEARIZER
  #ifndef RF_HAS_MDSP_TX_AGC
   /* freeze the PA state in the Tx AGC block */
   MSM_W_MASKED(TX_AGC_CTL2, FREEZE_PA_STATE, CAGC_RF_FREEZE_PA_STATE_V);
#endif
#endif


   /* Freeze TX open loop */
   #ifdef RF_HAS_MDSP_TX_AGC
   CDMA_MDSP_ENABLE_TX_OPEN_LOOP_OVERRIDE();
   #else
   MSM_W_MASKED(TX_AGC_CTL2, TX_OPEN_LOOP_OVERRIDE, CAGC_RF_TX_OL_FRZ_V);
   #endif

   /* enable override mode to freeze TX_GAIN ADJ */
   MSM_LOCK();
   COMBINER_DISABLE_POWER_CONTROL();
   MSM_UNLOCK();

   /* delay over 27.5us */

   /* suppressed message 834 - "'-' followed by '-' confusing- inside
      of clk_busy_wait macro */
   clk_busy_wait (CAGC_STATE_CLK); /*lint !e834*/

   /* return to start loading the CAGC linearizer tables */
} /* rf_freeze_cagc */


/*===========================================================================

FUNCTION RF_UNFREEZE_CAGC                                 INTERNAL FUNCTION

DESCRIPTION
This function is called after writing the RAS RAM linearizer tables. The
function will unfreeze the CAGC (RX AND TX) back to normal operation. It
is called once the RAS RAM linearizer tables are written.


DEPENDENCIES
    This function is called during a INTLOCK_SAV call.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void rf_unfreeze_cagc ( void )
{
   /* disable override mode to unfreeze TX_GAIN ADJ */
   MSM_LOCK();
   COMBINER_ENABLE_POWER_CONTROL();
   MSM_UNLOCK();

   /* unfreeze TX open loop */
   #ifdef RF_HAS_MDSP_TX_AGC
   CDMA_MDSP_SET_TX_OPEN_LOOP_NORMAL();
   #endif
   MSM_W_MASKED(TX_AGC_CTL2, TX_OPEN_LOOP_OVERRIDE, CAGC_RF_TX_OL_NRM_V);

#ifdef FEATURE_4PAGE_TX_LINEARIZER
   #ifndef RF_HAS_MDSP_TX_AGC
   /* un freeze the PA state in the Tx AGC block */
   MSM_W_MASKED(TX_AGC_CTL2, FREEZE_PA_STATE, CAGC_RF_UNFREEZE_PA_STATE_V);
   #endif
#endif
} /* rf_unfreeze_cagc */



/*============================================================================

FUNCTION RF_GET_LNA_GAIN_STATE                                       EXTERNAL

DESCRIPTION
  This function will return an enum representing the current LNA state.

DEPENDENCIES
  None

RETURN VALUE
  lna_gain_state_type

SIDE EFFECTS
  None

=============================================================================*/

lna_gain_state_type rf_get_lna_gain_state( void )
{
  return rf_get_path_lna_gain_state(RFCOM_TRANSCEIVER_0);
}
/*============================================================================

FUNCTION RF_GET_PATH_LNA_GAIN_STATE                                       EXTERNAL

DESCRIPTION
  This function will return an enum representing the current LNA state.

DEPENDENCIES
  None

RETURN VALUE
  lna_gain_state_type

SIDE EFFECTS
  None

=============================================================================*/

lna_gain_state_type rf_get_path_lna_gain_state( rfcom_device_enum_type device )
{
  byte lna_state;

#if !defined( RF_HAS_SINGLE_5_STAGE_LNA )
  byte lna_state_mixer_only;
#endif

  rf_path_enum_type path = rfcom_to_path[ device ];
  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];
#ifdef FEATURE_DUAL_CLK_RGM
  clkrgm_modem_clk_type rxfront_clk;
#else
  clk_regime_type rxfront_clk;
#endif 

  if (path >= RF_PATH_MAX)
  {
     MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
     rf_error_flag = TRUE;
     return INVALID_LNA_GAIN_STATE;
  }
  /* determine which path for clock check */
  if (path == RF_PATH_0) 
  {
     rxfront_clk = CLK_RGM_RXFRONT0_M;
  }
  else
  {
     rxfront_clk = CLK_RGM_RXFRONT1_M;
  }

  INTLOCK();

  /* Make sure clk regime is running, otherwise access will hang */
#ifdef RF_HAS_GPS
#error code not present
#else
  if (!clk_regime_is_on(rxfront_clk))
#endif
  {
    /* use last state */
    lna_state = (byte)last_lna_state[path];
  }
  else
  {
    if ( rf_is_sleeping_cdma1x(device) )
    {
      /* Set AGC value to min power if chain is off */
      lna_state = (byte)last_lna_state[path];
    }
    else
    {

      /*  5 stage LNA Control
      **  Depending on SINGLE_5_STAGE_LNA_SEL the polarity of the MSB is
      **  determined by either LNA_RANGE_POLARITY or MIXER_RANGE_POLARITY
      */

      /* get the 3 bit lna state into LSB position*/
      lna_state = (byte)HWIO_INI(RXF_CDMA(RX_AGCc_LNA_RANGE_RD), (int)path);

      #ifdef RF_HAS_SINGLE_5_STAGE_LNA

      /* isolate lna state bits and adjust for polarity */
      lna_state = (rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_pol != 0x0) ?
                  (~lna_state) & 0x07 : lna_state;
     #else
      /* isolate high order bit (mixer) of lna state and adjust for polarity */
      lna_state_mixer_only =
          (rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_mixer_range_pol != 0x0) ?
                (~lna_state) & 0x04 : lna_state & 0x04;

      /* isolate low order bits of lna state and adjust for polarity */
      lna_state = (rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_pol != 0x0) ?
                  (~lna_state) & 0x03 : lna_state & 0x03;

      /* composite lna state */
      lna_state = lna_state | lna_state_mixer_only;
      #endif  /* RF_HAS_SINGLE_5_STAGE_LNA */

      last_lna_state[path] = lna_state;
    }
  } /* clk regime check */

  INTFREE();

  /* Note that there is no need to check the array index because it is impossible
  ** to get an out of range value
  */
  return lna_gain_state[(int1)lna_state];

}

/*============================================================================

FUNCTION RF_GET_PA_STATE                                       EXTERNAL

DESCRIPTION
  This function will return an byte representing the two bits for PA state.
  It also stuffs the MSB with a 1 if the tx is on.
DEPENDENCIES
  None

RETURN VALUE
  pa state

SIDE EFFECTS
  None

=============================================================================*/

byte rf_get_pa_state( void )
{
  byte pa_state;
  #ifndef RF_HAS_MDSP_TX_AGC
  byte pa_r0_state;
  #endif
  /* get the 2 bit PA state */
  /* get 1x PA state */
  #ifdef RF_HAS_MDSP_TX_AGC
  pa_state =  (byte)(CDMA_MDSP_RD_PA_R0_R1_PIN_STATUS() & 0x3);
  #else
  pa_state = HWIO_INM(TX_PA_RD, HWIO_FMSK(TX_PA_RD, PA_R1_RD));        
  pa_r0_state = HWIO_INM(TX_PA_RD, HWIO_FMSK(TX_PA_RD, PA_R0_RD));

  pa_state =  ( pa_state ) ? 0x02 : 0x0 ;
  pa_state =  ( pa_r0_state ) ?  pa_state | 0x01 : pa_state;
  #endif

  if (rf_tx_status == RF_TX_IS_ON)
  {
#ifdef RF_HAS_HDR
#error code not present
#endif

    pa_state = pa_state | 0x10;
  }

  return pa_state;
}

/*============================================================================

FUNCTION RF_GET_RATCHET_STATE                                       EXTERNAL

DESCRIPTION
  This function will return an byte representing the two bits for RATCHET state.

DEPENDENCIES
  None

RETURN VALUE
  ratchet state

SIDE EFFECTS
  None

=============================================================================*/

byte rf_get_ratchet_state( void )
{
  byte ratchet_state;
  
  #ifdef RF_HAS_MDSP_TX_AGC
  byte tx_min_status;
  byte tx_max_status;
  #endif

  #ifdef RF_HAS_MDSP_TX_AGC
  tx_min_status = ((~(byte)(CDMA_MDSP_RD_TX_MIN_LIMIT_RATCHET_STATUS())) & 0x1);
  tx_max_status = ((~(byte)(CDMA_MDSP_RD_TX_MAX_LIMIT_RATCHET_STATUS())) & 0x1);
  ratchet_state   =  ( tx_min_status << 1) & tx_max_status;
  #else
  ratchet_state = (byte)HWIO_INM(RATCHET_BIT_DIS, HWIO_RMSK(RATCHET_BIT_DIS));
  #endif
  return ratchet_state;
}


/*____________________________________________________________________________
                       GPS Control Functions
____________________________________________________________________________*/


#ifdef RF_HAS_GPS
#error code not present
#endif /* RF_HAS_GPS */
/*________________________________
  End of GPS Control Functions
  ________________________________*/



/*===========================================================================

FUNCTION RF_HDET_ADJ_CALC                                    INTERNAL FUNCTION

DESCRIPTION
  Find the hdet index using an agc value.  This function performs
  a reverse search into the rf_cdma_exp_hdet_vs_agc_table to find
  the hdet index.

DEPENDENCIES
  rf_cdma_exp_agc_table[] must be set up.
  rf_pcs_exp_agc_table[] must be set up.
  rf_cdma_tx_lim_vs_freq[] must be set up.
  rf_cdma_exp_hdet_vs_agc[] must have been loaded from NV storage.

  rf_freq_index must be set up.

RETURN VALUE
  hdet index corresponding to the calculated agc_value


SIDE EFFECTS
  None

===========================================================================*/
#ifdef RF_HAS_HDET_UINT16
uint16 rf_hdet_adj_calc
(
  uint16 exp_agc_table_idx       /* index into the rf_cdma_exp_agc_table */
)
#else
byte rf_hdet_adj_calc
(
  byte exp_agc_table_idx       /* index into the rf_cdma_exp_agc_table */
)
#endif
{
  #ifdef RF_HAS_HDET_UINT16
  uint16 hdet_idx;          /* index to rf_cdma_exp_agc_table */
  #else
  byte hdet_idx;          /* index to rf_cdma_exp_agc_table */
  #endif
  byte zone;              /* the zone that the specified agc value is in */
  word agc_delta;         /* agc offset into zone */
  word delta;             /* hdet offset into zone */
  word rise;              /* total rise in the zone */
  int2 agc_value;         /* expected agc value */


  rf_chain_state_type *rf_chain_ptr = 
        &rf_chain_status[ rfcom_to_path[ RFCOM_TRANSCEIVER_0 ] ];

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* set rf_exp_agc_table_ptr to the proper TX AGC to HDET table */
  rf_exp_agc_table_ptr =  rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_exp_agc_table;

  /* Calculate the agc_value from the rf_cdma_exp_agc_table and the
  ** rf_cdma_tx_lim_vs_freq table.
  */
  agc_value =
    (int2) (  rf_exp_agc_table_ptr [ exp_agc_table_idx ]
#ifdef RF_HAS_LINEAR_INTERPOLATION
           + rf_freq_comp.i_tx_lim_vs_freq);
#else
           + rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_tx_lim_vs_freq[ rf_chain_ptr->rf_freq_index ] );

#endif

  /* Saturating agc_value at RF_TX_AGC_MIN, we effectively eliminate
  ** underflow values.
  */
  agc_value = RF_SATURATE( agc_value,
                           RF_TX_AGC_MIN,
                           ( RF_TX_AGC_MAX - RF_TX_AGC_OFF ) );

  /* Find the zone that we're in and get the agc delta.
  ** Knowing the zone and the agc_delta, we can calculate delta
  */
  zone      = (byte)(agc_value / RF_EXP_HDET_SIZ);
  agc_delta = agc_value % RF_EXP_HDET_SIZ;/*lint !e732 */

  /* Get the rise of the zone and calculate delta
  */
  rise      = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_exp_hdet_vs_agc[ zone + 1 ]
            - rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_exp_hdet_vs_agc[ zone     ] ;
  delta     = (word)((agc_delta * rise) / RF_EXP_HDET_SIZ);

  /* The hdet index = offset + delta
  */
  #ifdef RF_HAS_HDET_UINT16
  hdet_idx  = (uint16)(rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_exp_hdet_vs_agc[ zone ] + delta);
  #else
  hdet_idx  = (byte)(rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_exp_hdet_vs_agc[ zone ] + delta);
  #endif

  return( hdet_idx );

} /* rf_hdet_adj_calc */


/*===========================================================================

FUNCTION RF_CHK_SYNTH_LOCK                                  INTERNAL FUNCTION

DESCRIPTION
  This function check the synthesizer lock status.  The out-of-lock counter
  is updated for the periodic checks.  For non-periodic checks an error is
  post but the counter will not be updated.  If this counter exceeds the
  specified maximum, an ERR_FATAL is posted; else, an ERR is posted.  The
  counter is reset everytime the synth is in lock whether the check was a
  periodical one or not.

DEPENDENCIES
  None

RETURN VALUE
  Synthesizer lock status

SIDE EFFECTS
  None

===========================================================================*/
rf_synth_status rf_chk_synth_lock
(
  rfcom_device_enum_type device,
  rf_synth_checker checker    /* Update counter if out of lock */
)
{
  #ifdef RF_HAS_SYNTH_LOCK
  synth_pll_for_synth_check pll_device = RX_PLL;
  #endif

  rf_path_enum_type path = rfcom_to_path[ device ];

  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];
  
  static byte no_synth_lock_cnt = 0;    /* No synth lock count */

  rf_synth_status lock_status;          /* Is synth locked?    */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
    /* If the synthesizer is not locked, post an error and update the counter.
    */
  if (rf_chain_ptr->rf_mode == DB_RF_GPS)
  {
    #ifdef RF_HAS_SYNTH_LOCK
    pll_device = GPS_PLL;
    #endif
  }

  else if (rf_chain_ptr->rf_mode == DB_RF_SLEEP)
  {
    ERR_FATAL( "Cannot check for synth lock in sleep mode: %d", rf_chain_ptr->rf_mode,0,0 );
  }
  
  #ifdef RF_HAS_SYNTH_LOCK
  if ( synth_is_locked(device,pll_device) == FALSE ) {
  #else
  if (0) {
  #endif
    
    lock_status = RF_SYNTH_NOT_LOCKED;
  
    ERR("No synth lock. Checker: %xh, Cnt: %d", checker, no_synth_lock_cnt, 0);
  
    /* These are our periodic checks, we know that the last check was some
    ** reasonable time away.  If it was some glitches that caused the
    ** synthesizer to go out of lock, the synthesizer should have recovered by
    ** now.  So, we can update the out-of-lock-counter and ERR_FATAL if the
    ** counter has maxed out.
    */
    if( (checker & ((int)RF_SYNTH_ADJ_CDMA_PWR
                    | (int)RF_SYNTH_ADJ_FM_PWR)) != 0 ) {
  
      no_synth_lock_cnt++;      /* One more out-of-lock! */
  
      if ( no_synth_lock_cnt == ( (checker & (int)RF_SYNTH_ADJ_CDMA_PWR) != 0 ) ? RF_NO_SYNTH_LOCK_MAX_CDMA :
                                                     RF_NO_SYNTH_LOCK_MAX_FM )
      {
        ERR_FATAL( "Synth locks maxed out. Checker: %xh", checker,0,0 );
      }
    }
  }
  else {    /* The synthesizer is locked.  Reset the counter */

    lock_status = RF_SYNTH_IS_LOCKED;
    no_synth_lock_cnt = 0;
  
    MSG_LOW("Synth is locked", 0, 0, 0);
  }
  
  

  return( lock_status );

} /* end of rf_chk_synth_lock */


/*===========================================================================

FUNCTION RF_PWR_INDEX_CALC                                  INTERNAL FUNCTION

DESCRIPTION
  Calculates and returns a power table index (0..NV_PWR_TABLE_SIZ-1) for the
  power level specified.  Output = input.  This is used for indexing local
  copies of NV tables.  This could be a macro but it would be much harder to
  change it later when the power booster is added.

DEPENDENCIES
  None

RETURN VALUE
  byte sized power table index 0..NV_PWR_TABLE_SIZ-1

SIDE EFFECTS
  None

===========================================================================*/
LOCAL byte rf_pwr_index_calc
(
  byte pwr_level        /* input power level requested */
)
{
  return( pwr_level );

} /* rf_pwr_index_calc */


/*===========================================================================

FUNCTION RF_LOAD_CAGC_LINEARIZERS                           INTERNAL FUNCTION

DESCRIPTION
  Calculates and loads the CAGC Rx and Tx linearizers.  Due to hardware
  design, both the Rx and Tx tables must be loaded (there is no random
  addressing capability).  Addresses 0-15 are for the Rx linearizer and
  addresses 16-31 are for the Tx linearizer.  The write address automatically
  increments when each linearizer table entry is written.  The only ways to
  get to address 0 are (1) DMOD RESET and (2) writing the 32nd table entry.

DEPENDENCIES
  rf_config, rf_cdma_tx_comp_vs_freq[] must have been loaded from NV storage.

  Temperature data: rf_cdma_tx_lin_vs_temp_now, and rf_cdma_tx_slp_vs_temp_now
  must be valid.

  The linearizers are only loaded when the RF is in either
  RF_STATE_CDMA_RX or RF_STATE_CDMA_RXTX states.

RETURN VALUE
  None

SIDE EFFECTS
  rf_tx_lin_off[] is updated.
  rf_tx_lin_slp[] is updated.

===========================================================================*/

LOCAL void rf_load_cagc_linearizers(rfcom_device_enum_type device)
{

  word int_save;    /* saved interrupt enable state               */

  static boolean loading = FALSE;       /* block out re-entrancy  */
  static boolean freshen = FALSE;

  #ifndef RF_HAS_MDSP_TX_AGC
  int pa_state;
  int tx_lin_table_entry;
  #endif

  /* Consider not reloading if temp change in rx_trim 1 count, tx_trim 2 counts
  ** (2/3 of a dB)
  */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  rf_chain_state_type *rf_chain_ptr = 
        &rf_chain_status[ rfcom_to_path[ device ] ];

#ifdef RF_PDM_VS_POWER_TX_CAL_PROFILING
   timetick_type timer_rf_load_cagc_linearizers = timetick_get();
#endif /* RF_PDM_VS_POWER_TX_CAL_PROFILING */

#ifdef RF_HAS_GPS
#error code not present
#endif /* RF_HAS_GPS */

  /* Set the loading flag to disable LNA control.
  */
  rf_loading_linearizers = TRUE;

  /* ---------------------------------------------------------------------
  ** Lock interrupts out, and check to see if there is a load in progress.
  ** If there is a load in progress, return.  If not, set "loading" to
  ** TRUE to flag a load-in-progress.
  ** --------------------------------------------------------------------- */
  INTLOCK_SAV( int_save );
  if ( loading ) {
    freshen = TRUE;
    INTFREE_SAV( int_save );
    rf_loading_linearizers = FALSE;
    return;
  } else {
    loading = TRUE;
  }
  INTFREE_SAV( int_save );

  do {
    freshen = FALSE;

    #ifdef FEATURE_TX_POWER_BACK_OFF
    /* If we are transmitting let the PA Backoff module update the TX power
    ** limit at RF_PA_CALL_INTERVAL invocations of this function
    */
    if ( rf_tx_status == RF_TX_IS_ON )
    {
      /* Is it time to update the TX power limit based upon reverse link
      ** channel configuration (if needed) ? 
      */
      if( ++rf_backoff_refresh_counter>=RF_PA_CALL_INTERVAL )
      {
        /* Don't update if the PA backoff module is in the process of updating 
        ** the TX power limit
        */
        if( tx_power_limit_update_pending == FALSE )
        {
          rf_update_tx_pwr_limit( &agc_backoff_chan_config );
          rf_backoff_refresh_counter = 0 ;
        }
        }
      }
    #endif

    #ifdef RF_HAS_PDM_VS_POWER_TX_CAL
    /*Need to make sure that we do all this*/
    /*Calculation of Master Linearizer table*/
    if (rf_cdma_load_master_lin_tables == TRUE)
    {
        #ifdef RF_PDM_VS_POWER_TX_CAL_PROFILING
            timetick_type timer_rfagc_update_tx_lin_tbl_based_on_pdm_vs_pwr_cal = timetick_get();
        #endif /* RF_PDM_VS_POWER_TX_CAL_PROFILING */

        if (rf_cdma_load_master_lin_tables == TRUE)
        {
          rfagc_update_tx_lin_tbl_based_on_pdm_vs_pwr_cal(rf_chain_ptr->rf_curr_cdma_band);

          #ifndef RF_HAS_MDSP_TX_AGC
          rf_cdma_load_master_lin_tables = FALSE;
          #endif
        }
        #ifdef RF_PDM_VS_POWER_TX_CAL_PROFILING
            MSG_1(MSG_SSID_RF, MSG_LVL_MED, "RF_PDM_VS_POWER_TX_CAL_PROFILING::rfagc_update_tx_lin_tbl_based_on_pdm_vs_pwr_cal elapsed time: %d", timetick_get_elapsed(timer_rfagc_update_tx_lin_tbl_based_on_pdm_vs_pwr_cal, T_USEC));
        #endif /* RF_PDM_VS_POWER_TX_CAL_PROFILING */

      /*In case linearizer table is to be passed on to MDSP*/
      /*rf_cdma_load_master_lin_tables is updated to FALSE after*/
      /*the call to the function rf_cdma_mdsp_update_tx_lin_tables()*/
  
      #ifndef RF_HAS_MDSP_TX_AGC
      rf_cdma_load_master_lin_tables = FALSE;
      #endif
    }
    
    #endif

    #ifndef RF_HAS_MDSP_TX_AGC
    /* calculate new tx linearization values */
    rf_calc_new_tx_lin();
    #endif
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    INTLOCK_SAV( int_save );

    /* The CAGC RAS RAM can only be written when CHIPX8 is on.
    ** Only write to the CAGC RAS RAM when the RF is in either
    ** RF_STATE_CDMA_RX or RF_STATE_CDMA_RXTX states.  During these
    ** RF states, CHIPX8 is on.
    */
    if( ( rf_chain_ptr->rf_state == RF_STATE_CDMA_RX ) ||
        ( rf_chain_ptr->rf_state == RF_STATE_CDMA_RXTX ) ) 
    {
      #ifndef RF_HAS_MDSP_TX_AGC
       /* call freeze function to stop CAGC state machine */
       /* while loading the RAM linearizer tables */
       rf_freeze_cagc();

      /* The RAS RAM Write bit resets the write index for RAS RAM data.
      ** The bit is enabled to reset the write index to the initial value,
      ** and then the bit must be disabled to release the index from reset
      ** prior to writing the RAS RAM table.
      */

      HWIO_OUT(RAS_RAM_WR_ADDR_RESET, CAGC_RF_RAS_RAM_RESET);

      for (pa_state = 0; pa_state < 4; pa_state++)
      {
        for (tx_lin_table_entry = 0; tx_lin_table_entry < 16; tx_lin_table_entry++)
        {
          CAGC_RAS_WR_WH(rf_tx_lin_off[pa_state][tx_lin_table_entry],
                         rf_tx_lin_slp[pa_state][tx_lin_table_entry]);
        }
      }
      /* call unfreeze function */
      rf_unfreeze_cagc();

      #else /* Below FW TX AGC is enabled */
      #ifndef RF_HAS_BETA_SCALING_TX_AGC
      if (rf_cdma_load_master_lin_tables == TRUE)
      {
        rf_cdma_mdsp_update_tx_lin_tables();   
        rf_cdma_load_master_lin_tables = FALSE;
      }      
      rf_cdma_mdsp_update_tx_comp();
      #else
      rf_cdma_mdsp_update_tx_lin_tables();   
      #endif

      #endif /* RF_HAS_MDSP_TX_AGC */
    } /* end if */

    /* --------------------------------------------------------------------
    ** Set "loading" to FALSE to indicate tha the load-in-progress is done.
    ** -------------------------------------------------------------------- */
    loading = FALSE;

    INTFREE_SAV( int_save );

  } while ( freshen );

  rf_loading_linearizers = FALSE;

#ifdef RF_PDM_VS_POWER_TX_CAL_PROFILING
   MSG_1(MSG_SSID_RF, MSG_LVL_MED, "RF_PDM_VS_POWER_TX_CAL_PROFILING::rf_load_cagc_linearizers elapsed time: %d", timetick_get_elapsed(timer_rf_load_cagc_linearizers, T_USEC));
#endif /* RF_PDM_VS_POWER_TX_CAL_PROFILING */

} /* rf_load_cagc_linearizers */


#ifdef FEATURE_ZIF_LOGGING


/*===========================================================================

FUNCTION RF_LOG_EXTENDED_AGC

DESCRIPTION    This function is invoked to log extended AGC values for DIAG.

DEPENDENCIES   rf_ext_agc_vars

RETURN VALUE   None.

SIDE EFFECTS   Messages can be sent to DIAG task.

===========================================================================*/

void  rf_log_ext_agc ( rfcom_device_enum_type device )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  rf_path_enum_type path = rfcom_to_path[ device ];

  /* Check to see extended AGC logging is enabled. If not, just bail out. */

  if ( !log_status(LOG_EXTENDED_AGC_C) )
  {
    /* Logging is not enabled. Reset index and return */
    rf_ext_agc_vars.inx = 0;
    return;
  }

  /* Return if in sleep state since RXF clocks will be off */
  if ( rf_is_sleeping_cdma1x(device) ) 
  {
     return;
  }

  /* New buffer? */
  if ( rf_ext_agc_vars.inx == 0 )
  {
    /* Refresh the timestamp at the start of each new buffer */
    ts_get ( (unsigned long *) rf_ext_agc_vars.log.hdr.ts );

  } /* If we are just getting started */

  /* Now fill in the items which should get updated at the sample rate */

  /* LNA Range sample */
  rf_ext_agc_vars.log.lna_range[ rf_ext_agc_vars.inx ]
    = (byte)HWIO_INI (RXF_CDMA(RX_AGCc_LNA_RANGE_RD), (int)path);

  /* CDMA VGA Gain sample */
  /* not sure about this conversion */
//  rf_ext_agc_vars.log.cdma_vga_gain[ rf_ext_agc_vars.inx ]
//    = CAGC_INHMSF (CAGC_CDMA_VGA_GAIN, CAGC_CDMA_VGA_GAIN_M);
// use RX_AGCc_1X_VGA_GAIN_RD register

  /* Buffer full? */
  if ( ++rf_ext_agc_vars.inx >= LOG_EXT_AGC_CNT )
  {
    /* We've filled up the logging buffer. */
    if (log_submit((log_type *) &rf_ext_agc_vars.log) == FALSE)
    {
      rf_ext_agc_vars.drops++;
    }

    rf_ext_agc_vars.inx = 0;
      /* Point to first bin in the hopper. This gets bumped
         on every logging call.  It also signals a new buffer. */
  }

} /* rf_log_ext_agc() */



/*===========================================================================

FUNCTION RF_LOG_TX_AGC

DESCRIPTION    This function is invoked to log TX AGC values for DIAG.

DEPENDENCIES   rf_tx_agc_vars

RETURN VALUE   None.

SIDE EFFECTS   Messages can be sent to DIAG task.

===========================================================================*/

uint8 foo = 0xB;

void  rf_log_tx_agc ( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check to see TX AGC logging is enabled. If not, just bail out. */

  if ( !log_status(LOG_TRANSMIT_AGC_C) )
  {
    /* Logging is not enabled. Reset index and return */
    rf_tx_agc_vars.inx = 0;
    return;
  }

  /* Return if in sleep state since RXF clocks will be off */
  if ( rf_is_sleeping_cdma1x(RFCOM_TRANSCEIVER_0)) 
  {
     return;
  }

  /* New buffer? */
  if ( rf_tx_agc_vars.inx == 0 )
  {
    /* Refresh the timestamp at the start of each new buffer */
    ts_get ( (unsigned long *) rf_tx_agc_vars.log.hdr.ts );

  } /* If we are just getting started */

  /* Now fill in the items which should get updated at the sample rate */

  /* TX AGC Adjust sample */
  #ifdef RF_HAS_MDSP_TX_AGC
  rf_tx_agc_vars.log.tx_agc_adj[ rf_tx_agc_vars.inx ]
   = (int2)((((int2)CDMA_MDSP_GET_TX_AGC_ADJ_VAL())>>7) + 256);
  #else
  rf_tx_agc_vars.log.tx_agc_adj[ rf_tx_agc_vars.inx ]
    = (int2)HWIO_INM (TX_AGC_ADJ_RD, TX_AGC_ADJ_M);
  #endif

  /* TX Open Loop Accumulator sample */
  /* suppressing e718 - symbol undeclared - this macro is not declared */
  /* suppressing 746, 701, 702 - not our macro */
  #ifdef RF_HAS_MDSP_TX_AGC
   rf_tx_agc_vars.log.tx_open_loop[ rf_tx_agc_vars.inx ]
    = (int2)(((int2)CDMA_MDSP_RD_TX_OPEN_LOOP_VAL())>>6);

   rf_tx_agc_vars.log.tx_gain_adj[ rf_tx_agc_vars.inx ]
    = (int2)(((int2)CDMA_MDSP_RD_TX_CLOSED_LOOP_VAL())>>7);

   rf_tx_agc_vars.log.tx_gain_ctl[ rf_tx_agc_vars.inx ]
     = (int2)(((int2)CDMA_MDSP_RD_TRANSMIT_GAIN())>>6);

  #else

  rf_tx_agc_vars.log.tx_open_loop[ rf_tx_agc_vars.inx ]
    = CAGC_INHMSF (TX_OPEN_LOOP, TX_OPEN_LOOP_M); /*lint !e506 !e702 !e718 !e746 !e701*/ /* TX Gain Adjust sample */

  rf_tx_agc_vars.log.tx_gain_adj[ rf_tx_agc_vars.inx ]
    = (int2)CAGC_INHMF (TX_GAIN_ADJ, TX_GAIN_ADJ_M); /*lint !e506 !e718 !e746 !e702 !e701*/

  /* TX Gain Control sample */
  rf_tx_agc_vars.log.tx_gain_ctl[ rf_tx_agc_vars.inx ]
    = CAGC_INHMSF (TX_GAIN_CTL, TX_GAIN_CTL_M); /*lint !e506 !e702 !e718 !e746 !e701*/

  #endif /* RF_HAS_MDSP_TX_AGC */

  /* Ratchet Bit sample */
  /* not sure about this conversion */
//  rf_tx_agc_vars.log.ratchet_bit[ rf_tx_agc_vars.inx ]
//    = ((HWIO_INM (RATCHET_BIT_DIS, DATA))
//      >> SHIFT_DN_FROM_MASK(RATCHET_BIT_DIS_M));

  /* Buffer full? */
  if ( ++rf_tx_agc_vars.inx >= LOG_TX_AGC_CNT )
  {
    /* We've filled up the logging buffer. */
    if (log_submit((log_type *) &rf_tx_agc_vars.log) == FALSE)
    {
      rf_tx_agc_vars.drops++;
    }

    rf_tx_agc_vars.inx = 0;
      /* Point to first bin in the hopper. This gets bumped
         on every logging call.  It also signals a new buffer. */
  }

} /* rf_log_tx_agc() */


/*===========================================================================

FUNCTION RF_INIT_ZIF_AGC_LOGS

DESCRIPTION    This function is invoked to perform ZIF-AGC logging.

DEPENDENCIES   rf_ext_agc_vars
               rf_tx_agc_vars
               rf_iq_offsets_vars
               rf_dacc_iq_accum_vars

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/

void  rf_init_zif_agc_logs ( void )
{
  /* Log buffer for Extended AGC Logging */
  rf_ext_agc_vars.log.hdr.code = LOG_EXTENDED_AGC_C;
  rf_ext_agc_vars.log.hdr.len  = sizeof(log_ext_agc_type);
  rf_ext_agc_vars.inx      = 0;
  rf_ext_agc_vars.drops    = 0;

  /* Log buffer for Transmit AGC Logging */
  rf_tx_agc_vars.log.hdr.code = LOG_TRANSMIT_AGC_C;
  rf_tx_agc_vars.log.hdr.len  = sizeof(log_tx_agc_type);
  rf_tx_agc_vars.inx      = 0;
  rf_tx_agc_vars.drops    = 0;

} /* rf_init_zif_agc_logs */


#endif /* FEATURE_ZIF_LOGGING */


#ifdef FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST
/*===========================================================================

FUNCTION       RF_NOTIFY_SCH_DATA_RATE

DESCRIPTION    This API function is called to notify the RF driver when
               the reverse supplemental channel will be on/off in the next
               frame.  R-SCH data rate is also passed.

DEPENDENCIES   rf_nv_dig_items_ptr must have been set up

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
void rf_notify_sch_data_rate(rfcom_device_enum_type device,
                             boolean sch_is_on,
                             enc_sch_rate_type sch_rate,
                             boolean dcch_is_on
                            )
{
    int1   offset1 = 0; /* PA switchpoint 1 */
    int1   offset2 = 0; /* PA switchpoint 2 */
    int1   offset3 = 0; /* PA switchpoint 3 */

    rf_path_enum_type path = rfcom_to_path[ device ];

    rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];

    /* don't do anything if R-SCH is not coming on and we haven't
    ** previously adjusted the switch points
    */
    if ( ( !sch_is_on ) && ( !pa_thres_adjusted ) ) return;

    /* keep anyone (i.e., power limit algorithm) from mucking
    ** around with local variables while we are doing this
    */
    INTLOCK();

    /* reset the the PA switch points */
    rf_reset_pa_thres();

    /* if R-SCH is on get the PA Rise/Fall offset for the specified
    ** data rate
    */
    if ( ( sch_is_on ) && ( !dcch_is_on ) ) {

        switch ( sch_rate) {
        case ENC_SCH_FULL_RATE :
            offset1 =  rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_p1_pa_rise_fall_off[rfcom_to_path[ RFCOM_TRANSCEIVER_0 ]][5];/*lint !e713 */
#ifdef RF_HAS_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST
            offset2 =  rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_p2_pa_rise_fall_off[rfcom_to_path[ RFCOM_TRANSCEIVER_0 ]][5];/*lint !e713 */
            offset3 =  rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_p3_pa_rise_fall_off[rfcom_to_path[ RFCOM_TRANSCEIVER_0 ]][5];/*lint !e713 */
#endif /* RF_HAS_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST */
            break;
        case ENC_SCH_2X_RATE :
            offset1 =  rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_p1_pa_rise_fall_off[rfcom_to_path[ RFCOM_TRANSCEIVER_0 ]][0];/*lint !e713 */
#ifdef RF_HAS_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST
            offset2 =  rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_p2_pa_rise_fall_off[rfcom_to_path[ RFCOM_TRANSCEIVER_0 ]][0];/*lint !e713 */
            offset3 =  rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_p3_pa_rise_fall_off[rfcom_to_path[ RFCOM_TRANSCEIVER_0 ]][0];/*lint !e713 */
#endif /* RF_HAS_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST */
            break;
        case ENC_SCH_4X_RATE :
            offset1 =  rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_p1_pa_rise_fall_off[rfcom_to_path[ RFCOM_TRANSCEIVER_0 ]][1];/*lint !e713 */
#ifdef RF_HAS_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST
            offset2 =  rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_p2_pa_rise_fall_off[rfcom_to_path[ RFCOM_TRANSCEIVER_0 ]][1];/*lint !e713 */
            offset3 =  rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_p3_pa_rise_fall_off[rfcom_to_path[ RFCOM_TRANSCEIVER_0 ]][1];/*lint !e713 */
#endif /* RF_HAS_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST */
            break;
        case ENC_SCH_8X_RATE :
            offset1 =  rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_p1_pa_rise_fall_off[rfcom_to_path[ RFCOM_TRANSCEIVER_0 ]][2];/*lint !e713 */
#ifdef RF_HAS_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST
            offset2 =  rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_p2_pa_rise_fall_off[rfcom_to_path[ RFCOM_TRANSCEIVER_0 ]][2];/*lint !e713 */
            offset3 =  rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_p3_pa_rise_fall_off[rfcom_to_path[ RFCOM_TRANSCEIVER_0 ]][2];/*lint !e713 */
#endif /* RF_HAS_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST */
            break;
        case ENC_SCH_16X_RATE :
            offset1 =  rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_p1_pa_rise_fall_off[rfcom_to_path[ RFCOM_TRANSCEIVER_0 ]][3];/*lint !e713 */
#ifdef RF_HAS_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST
            offset2 =  rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_p2_pa_rise_fall_off[rfcom_to_path[ RFCOM_TRANSCEIVER_0 ]][3];/*lint !e713 */
            offset3 =  rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_p3_pa_rise_fall_off[rfcom_to_path[ RFCOM_TRANSCEIVER_0 ]][3];/*lint !e713 */
#endif /* RF_HAS_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST */
            break;
#ifdef FEATURE_32X_SCH_DATA_RATE
        case ENC_SCH_32X_RATE :
            offset1 =  rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_p1_pa_rise_fall_off[rfcom_to_path[ RFCOM_TRANSCEIVER_0 ]][4];/*lint !e713 */
#ifdef RF_HAS_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST
            offset2 =  rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_p2_pa_rise_fall_off[rfcom_to_path[ RFCOM_TRANSCEIVER_0 ]][4];/*lint !e713 */
            offset3 =  rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_p3_pa_rise_fall_off[rfcom_to_path[ RFCOM_TRANSCEIVER_0 ]][4];/*lint !e713 */
#endif /* RF_HAS_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST */
            break;
#endif /* FEATURE_32X_SCH_DATA_RATE */
        default:
            MSG_ERROR("Invalid SCH rate %d :",  sch_rate, 0, 0);
            break;

        }

        /* adjust the PA Rise/Fall according to data rate on R-SCH */
        rf_adjust_pa_thres( -offset1, -offset2, -offset3 );

        /* indicate PA rise/fall thresholds have been adjusted */
        pa_thres_adjusted = TRUE;

    }
    else {
        /* R-SCH is not on or DCCH + R-SCH is on , don't adjust */
        pa_thres_adjusted = FALSE;
    }

    /* track the current offset used */
    curr_offset1 = offset1;
    curr_offset2 = offset2;
    curr_offset3 = offset3;


    /* load the new Rise/Fall values */
    rf_load_pa_thres();
    #ifdef RF_HAS_MDSP_TX_AGC
    CDMA_MDSP_NEW_PA_THRESH_FRAME_PARAMS_AVAILABLE();    
    #endif  

    INTFREE();

#ifdef CAGC_GEN_FRAME_SYNC
#error code not present
#endif
}

/*===========================================================================

FUNCTION       RF_CAGC_IS_PA_THRES_ADJUSTED

DESCRIPTION    This API function is called to notify the RF driver when
               the reverse supplemental channel will be on/off in the next
               frame.  R-SCH data rate is also passed.

DEPENDENCIES   None.

RETURN VALUE   boolean indicating whether the current PA rise/fall values
               have been adjusted

SIDE EFFECTS   None.

===========================================================================*/
boolean rf_cagc_is_pa_thres_adjusted( void )
{
    return pa_thres_adjusted;
}


/*===========================================================================

FUNCTION       RF_CAGC_GET_CURR_PA_THRES_ADJ

DESCRIPTION    This API function provides the current adjustment to the
               PA switch (rise/fall) points

DEPENDENCIES   None.

RETURN VALUE   current offset used for PA switch point

SIDE EFFECTS   None.

===========================================================================*/
int1 rf_cagc_get_curr_pa_thres_adj( byte pa_switchpoint )
{
   if ( pa_switchpoint == PA_SWITCHPOINT_1 )
   {
      return curr_offset1;
   } 
   else if ( pa_switchpoint == PA_SWITCHPOINT_2 )
   {
      return curr_offset2;
   }
   else if ( pa_switchpoint == PA_SWITCHPOINT_3 )
   {
      return curr_offset3;
   }
   else
   {
      RF_FATAL_ERROR("Invalid PA Switchpoint");
      return 0;
   }

    
}
#endif  /* FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST */



#ifdef CAGC_GEN_FRAME_SYNC
#error code not present
#endif

#endif  /* !FEATURE_GSM_PLT */



/*============================================================================

FUNCTION  RF_SET_LNA_BASED_ON_AGC                                 

DESCRIPTION
  This function will determine and set the LNA state based on the AGC
  value passed to it.  The AGC value is in 1x format  (8 bit, .3 dB).

DEPENDENCIES
  The correct RF cal data must be loaded.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void rf_set_lna_based_on_agc
( 
  rfcom_device_enum_type device, 
    /* The device to use */

  int1 agc 
    /* The AGC value to use to determine the LNA state */
)
{
   lna_gain_state_type lna_state; /* Holds the determined LNA state. */

   #ifdef RF_HAS_RFCONFIG_RX_LNA
   const rfconfig_rx_lna_linearity_info_type  *lna_linearity_info_ptr = rf_get_current_rfconfig_rx_lna_linearity_info(device);
   const rfconfig_rx_lna_switch_pt_type       *lna_switch_pt_ptr = NULL;
   uint32 gain_step_index = 0;

   // at least one LNA gain step must have been configured for this linearity
   RF_ASSERT(NULL != lna_linearity_info_ptr);

   if(lna_linearity_info_ptr == NULL)
   {
       MSG_FATAL("lna_linearity_info_ptr is NULL",0,0,0);
       rf_error_flag = TRUE;
       return;
   }

   RF_ASSERT(lna_linearity_info_ptr->num_gain_steps > 0);
   // #gain_stages = num_gain_steps + 1; in enum 'lna_gain_state_type' the sequence of
   //  valid gain_stages start from index 0, and so subtract 1 from #gain_stages to compare
   //  against INVALID_LNA_GAIN_STATE
   RF_ASSERT(lna_linearity_info_ptr->num_gain_steps < INVALID_LNA_GAIN_STATE);
   //  so whatever gain_step_index ends up being below will correspond to the highest
   //  gain stage as indexed through the enum 'lna_gain_state_type'
   for (
      gain_step_index = 0;
      gain_step_index < lna_linearity_info_ptr->num_gain_steps; 
      gain_step_index ++)
   {
      lna_switch_pt_ptr = rfconfig_rx_lna_get_switch_point_info_from_linearity(lna_linearity_info_ptr, gain_step_index);
      if ( agc < rfconfig_rx_lna_switch_pt_get_fall(lna_switch_pt_ptr) )
      {
         break;
      }
   }
   lna_state = (lna_gain_state_type)gain_step_index;

   #else /* ! #ifdef RF_HAS_RFCONFIG_RX_LNA */
   rf_path_enum_type path = rfcom_to_path[ device ]; /* Determine path based on device */
   rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ]; /* Retrieve chain info */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef RF_HAS_POWER_MODE_LIB
  rflib_power_mode_type power_mode = rflib_get_power_mode( path );
    /*  Determine the power mode if necessary */

  /* Find LNA state from fall values based on AGC, path, and power mode */
  if (agc < 
      rf_chain_ptr->rf_nv_dig_items_ptr->
      rfnv_lna_range_1_fall[path][power_mode])
  {
   lna_state = LNA_GAIN_STATE_0;
  } 
  else if (agc < 
           rf_chain_ptr->rf_nv_dig_items_ptr->
           rfnv_lna_range_2_fall[path][power_mode])
  {
    lna_state = LNA_GAIN_STATE_1;
  }
  else if (agc < 
           rf_chain_ptr->rf_nv_dig_items_ptr->
           rfnv_lna_range_3_fall[path][power_mode])
  {
    lna_state = LNA_GAIN_STATE_2;
  }
  else if (agc < 
           rf_chain_ptr->rf_nv_dig_items_ptr->
           rfnv_lna_range_4_fall[path][power_mode])
  {
    lna_state = LNA_GAIN_STATE_3;
  }
  else
  {
    lna_state = LNA_GAIN_STATE_4;
  }

#else

  /* Find LNA state from fall values based on AGC and path */
  if (agc < rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_1_fall[path])
  {
   lna_state = LNA_GAIN_STATE_0;
  } 
  else if (agc < rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_2_fall[path])
  {
    lna_state = LNA_GAIN_STATE_1;
  }
  else if (agc < rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_3_fall[path])
  {
    lna_state = LNA_GAIN_STATE_2;
  }
  else if (agc < rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_4_fall[path])
  {
    lna_state = LNA_GAIN_STATE_3;
  }
  else
  {
    lna_state = LNA_GAIN_STATE_4;
  }
#endif /* RF_HAS_POWER_MODE_LIB */ /*lint !e701*/

  #endif /* RF_HAS_RFCONFIG_RX_LNA */

  /* Set LNA state */
  rf_set_cdma_lna_decision( device, lna_state);

  /* Show what we did, for now */
  MSG_HIGH("LNA set to %d based on %d", lna_state, agc, 0);

} /* rf_set_lna_based_on_agc */


#ifdef RF_HAS_RFCONFIG_RX_LNA
/*===========================================================================
FUNCTION rf_get_current_rfconfig_rx_lna_linearity_info  EXTERNALIZED FUNCTION

DESCRIPTION
  This function retrieves the switchpoint info as stored in RFConfig
  data structures for the indicated device.

DEPENDENCIES

RETURN VALUE
  RFConfig info for the current linearity mode in which the Rx LNA is operating
  on the indicated RFCom device.

SIDE EFFECTS
  None
===========================================================================*/
const rfconfig_rx_lna_linearity_info_type* rf_get_current_rfconfig_rx_lna_linearity_info(
   rfcom_device_enum_type device)
{
   rf_path_enum_type                           path = rfcom_to_path[device];
   rf_chain_state_type                        *rf_chain_ptr = &rf_chain_status[path];
   rfconfig_rx_lna_operational_mode_type       lna_op_mode = RFCONFIG_RX_LNA_OPERATIONAL_MODE__NONE;
   const rfconfig_rx_lna_linearity_info_type  *lna_linearity_info_ptr = NULL;
   #ifdef RF_HAS_POWER_MODE_LIB
   rflib_power_mode_type power_mode = rflib_get_power_mode( path );
   #else /* ! #ifdef RF_HAS_POWER_MODE_LIB */
   rflib_power_mode_type power_mode = RFLIB_NORMAL_POWER_MODE;
   #endif /* RF_HAS_POWER_MODE_LIB */

   #ifdef RF_HAS_HDR
#error code not present
   #endif /* RF_HAS_HDR */
   {
      lna_op_mode = RFCONFIG_RX_LNA_OPERATIONAL_MODE__1X;
   }

   lna_linearity_info_ptr = rfconfig_rx_lna_get_linearity_info(
                               &rf_chain_ptr->rf_nv_dig_items_ptr->rfconfig_rx_lna_info[path],
                               lna_op_mode,
                               power_mode);

   return lna_linearity_info_ptr;
}
#endif /* RF_HAS_RFCONFIG_RX_LNA */


#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */

/*===========================================================================

FUNCTION RF_GET_PATH_1X_AGC                                          INTERNAL

DESCRIPTION
  This function provides the raw RX_AGCc_1X_VALUE_RD value for the requested
  chain.  The previously read value is returned if the RxAGC clocks are 
  disabled for GPS builds.  
    
DEPENDENCIES
  None.
  
RETURN VALUE
  Raw RxAGC value or previous value if RxAGC clock disabled. 

SIDE EFFECTS
  None
  
===========================================================================*/
int2 rf_get_path_1x_agc(rfcom_device_enum_type device)
{
   int2 rx_agc;
   rf_path_enum_type path = rfcom_to_path[ device ];

#ifdef FEATURE_DUAL_CLK_RGM
   clkrgm_modem_clk_type rxfront_clk;
#else
   clk_regime_type rxfront_clk;
#endif 

   if (path >= RF_PATH_MAX)
   {
       MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
       rf_error_flag = TRUE;
       return -1;
   }
   /* determine which path for clock check */
   if (path == RF_PATH_0) 
   {
      rxfront_clk = CLK_RGM_RXFRONT0_M;
   }
   else
   {
      rxfront_clk = CLK_RGM_RXFRONT1_M;
   }

   INTLOCK();

#if !defined(T_RUMI_SC2X) && !defined(RF_HAS_7500A_TRF6500)
#if defined(RF_HAS_GPS)
#error code not present
#else
   if (!clk_regime_is_on(rxfront_clk))
#endif /* RF_HAS_GPS */
#else /* T_RUMI_SC2X */
#error code not present
#endif /* T_RUMI_SC2X && RF_HAS_7500A_TRF6500 */
   {
      rx_agc = last_rx_agc[path];
   }
   else
   {
      if ( rf_is_sleeping_cdma1x(device) )
      {
        /* Set AGC value to min power if chain is off */
        rx_agc = RF_RX_AGC_MIN_RD_VALUE;
      }
      else
      {
        /* Read AGC value */
        rx_agc = (int2)HWIO_INMI(RXF_CDMA(RX_AGCc_1X_VALUE_RD), (int)path, HWIO_RMSK(RXF_CDMA(RX_AGCc_1X_VALUE_RD)));
        /* Save the last good value for use if clocks disabled (i.e. sleep) */
        last_rx_agc[path] = rx_agc;
      }
 
    }

  INTFREE();

    return(rx_agc);
}

/*lint -restore */// Loss of precision messages.
/* EJECT  */
/*======================== O B S O L E T E  F U N C T I O N S =============*/
#ifdef RF_HAS_OBSOLETE_FUNCTIONS
#error code not present
#endif /* RF_HAS_OBSOLETE_FUNCTIONS */

