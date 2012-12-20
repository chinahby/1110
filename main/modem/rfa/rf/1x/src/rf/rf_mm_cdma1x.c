/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*



                            R F   U n i t

GENERAL DESCRIPTION
  This is the RF unit which controls the RF subsystem for AMSS
  software.  The specific tasks of this module is to provide an global RF
  interface for other modules.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The following functions must be called in the following order before
  any other functions provided by this driver:

  (1) rf_init()          -- RF initialization, before RF clocks are used
  (2) rf_read_nv_item(i) -- for each RF calibration item stored in NVM.
  (3) rf_init_delayed()  -- to init the RF subsystem from using the values
                            rtored in NVM.

  The following flowchart illustrates the major mode transitions for the
  RF unit.  Note that rf_codec_on() is asynchronous with the drivers shown
  in this flowchart and may interrupt at any time to request or release
  control of chipx8 and CODEC power.  The reverse is also true (any of these
  drivers may interrupt rf_codec_on() to make a mode change).

                            /-------\
                            |       |           
                            | RESET | 
                            |       |
                            \-------/
                                |
                                /
                                /
           rfnv_read_nv_item(a) |
             .................. |
           rfnv_read_nv_item(z) |
                                /
                                /
                      rf_init() |
              rf_init_delayed() |
                                |      rf_enable_tx_pwr()
           rf_sleep()           |      wait RF_ENABLE_PWR_WAIT ms
           catnap              \|/     rf_tx_enable()
/-------\  rf_stay_asleep() /-------\  wait RF_TUNE_CHAN_WAIT ms /-------\
| CDMA  |<------------------| CDMA  |--------------------------->| CDMA  |
| SLEEP |                   |  Rx   |                            | RxTx  |
|       |------------------>|(IDLE) |<---------------------------|       |
\-------/                   \-------/    rf_tx_disable()         \-------/
 rf_enable_digital_rx_pwr()  /|\  |
 wait RF_ENABLE_PWR_WAIT ms   |   |
 rf_init_digital_band_chan()  |   |
 wait RF_TUNE_CHAN_WAIT ms    |   | rf_enable_analog_rx_pwr()
 rf_complete_sleep_warmup()   |   | wait RF_ENABLE_PWR_WAIT ms
                              |   | rf_init_analog()
                              |   |
                              |   |      rf_enable_tx_pwr()
   rf_enable_digital_rx_pwr() |   |      wait RF_ENABLE_PWR_WAIT ms
   wait RF_ENABLE_PWR_WAIT ms |   |      rf_tx_enable()
   rf_init_digital()          |   |      wait RF_TUNE_CHAN_WAIT ms
                              |   |      rf_set_pa_level(p)
                              |   |      wait RF_PA_LEVEL_WAIT ms
                              |  \|/     rf_pa_on(TRUE)
                            /-------\    wait RF_PA_WAIT ms      /-------\
                            |  FM   |--------------------------->|  FM   |
                            |  Rx   |                            | RxTx  |
                            |(IDLE) |<---------------------------|       |
                            \-------/    rf_pa_on(FALSE)         \-------/
                                         rf_tx_disable()

Copyright (c) 1994, 1995, 1996 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1997, 1998, 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2000, 2001, 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2003, 2004, 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2006, 2007, 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2009             by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rf_mm_cdma1x.c#29 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/1/10    nv      RF is put to sleep after initialization in rf_init_delayed() 
02/11/09   vb      Bug fix for RFCOM_RECEIVER_1 case in rf_init_cdma1x()
02/05/09   vb      Added support for RF_HW_QSC11X0_TRI_BAND_BC5_TRI_STATE_PA
01/20/09   vb      Sleep current Optimization: Changed the PA control to HW during 
                   Tx OFF (programming CAGC_RF_PAR_10_V) and back to MDSP during Tx ON  
01/06/09   vb      Added support for RX ADC voltage switching during RF Wakeup
12/10/08   vm      Added the fix for Spike after Hard Handoff
12/03/08   vm      Added the Support for Temperature based Device
                   Configration change
11/24/08   lp      More LINT fixes
11/17/08   vm      Changed the SMPS Frequency for BC6 Band to 1.6 MHz
11/16/08   lp      Fixed Klocwork and Lint errors
10/08/08   sr      Klocwork fixes
11/14/08   vm      Added the support for Variable Switcher Size and Frequency
                   Park the PA in Low gain state before Handoff
11/11/08   vm      Put in the fix for enabling/disbling the Rx ADC when
                   the RF is in Wakeup/Sleep State
11/07/08   vm      Fixed putting the PA Range Pins to Low Logic in sleep
10/12/08   vm      Added support for Tx DAC Backoff
10/11/08   vm      Added support for different PMIC Switcher Size
10/11/08   vm      Added support for different Access Probe Switchpoints
10/06/08   vb      Corrected errors in the functions: 
                   rf_init_hdr(); rf_get_nv_band(); rf_tune_band_chan() and rf_init_digital_band_chan()
09/30/08   vm/vb   Added Support for Band Class Sub Block Implementation
09/25/08   vm      Added more support for TCXO Shutdown
09/15/08   vm      Added the fix for PA Switchpoint Glitch
09/14/08   vm      Added support for correct Sleep Enable Sequence
09/14/08   vm      Added the support for Tx DAC Backoff
07/24/08   ems     Compiler warnings
07/03/08   vm      Disabled the forcing to Low Linearity mode for QSC1100
06/13/08   vm      Upddates for 16 bit PDET on QSC1100
05/20/08   vm      Removed the fixed PA_R_MAP for QSC1100
05/15/08   vm      Removed the Fixed PDM needed for bringup
04/30/08   sl      Added call to rfr_adjust_lo_pwc_dac_codes every time the
                   band/chan change.
04/16/08   vm      Added bringup support for QSC1100 and support for BC10 Band
03/28/08   vm      Added the support for Tx AGC interface with MDSP for QSC1100
03/14/08   vm      Change the PA Overrside state for RICO Bringup
03/12/08   vm      Added Support for supporting Single chain on QSC1100
03/10/08   vm      Commented out the initialization of Receiver 1 in rf_init
                   and rf_init_delayed()
03/10/08   vm      Added more stuff for RICO Voltron Bringup
03/02/08   vb/vm   Added the Manula SBI settings for RTR6500 B0 for 
                   RICO Bringup
12/04/07   ems     Support for QSC1100
12/02/07   ans     Merged in change from SC2x to set band specific PA_R_MAP, both 
                   for 1x and HDR.
12/02/07   ans     Convert PDM_VS_PWR tables to TX lin tables during RF NV init.
11/28/07    xw     Added target validation in rf_init_cdma1x() and rf_init_delayed().
11/25/07   ans     Renamed rfagc_update_tx_lin_tbl_based_on_pwr_vs_pdm_cal().
11/09/07   ans     Recalculate the TX Compensation values whenever NV is updated.
11/07/07   ans     Updated TX compensation functions for PDM_VS_PWR Calibration.
11/04/07   ans     Added calls to calibration functions for PDM_VS_PWR Calibration.
10/26/07   vm      Disable the initialization of Rex Critical section in
                   rf_init() as they are replaced by intlocks in ADC Clock 
                   Switching Code for Sc2x
10/05/07   Vish    Feature-ized call to initialize RFConfig Rx LNA in rf_init_rf_nv_items().
10/04/07   Vish    Updated rf_init_rf_nv_items() to initialize/update RFConfig Rx LNA
                   database using the just recently read RF NV items.
10/02/07   dyc     Remove RF_DEBUG_SBI code.
10/01/07   rmd     Restored reg addresses in codec_addr_array[].
10/01/07   rmd     Added Band specific TX PA MAP value support.
09/23/07   dyc     Update codec sbi debug.
09/20/07   cv      Managing the PMIC VReg Boost (disabling PM_VREG_BOOST). Moved the management to card file.
09/14/07   adk     Removed MSG_ERRORs assoc with PAD3 wakeup/sleep function-calls
08/29/07   aak     Fix for HHO in 1X in BC5 on diversity
08/24/07   vm      Corrected the timings for Turning on Tx_ON Signal
                   during HHO.
08/24/07   vm      Use the Value programmed in Card file to program PA
                   Hysterisis timer for 1X rather than Through Macro
                   Eliminate the use of feature RF_HAS_PA_BYPASS 
08/21/07   adk     Fixed compiler warning.
08/20/07    ka     Added settings to reduce MSMA current during sleep.
08/13/07   dyc     Added chan info to message in rf_complete_sleep_warmup.
08/10/07   vm      Added the support for programming different PA switchpoints
                   and PA hysterisis timer during Access Probes different from 
                   traffic
08/08/07   ra      On CGPS targets check for GPS receive state in rf_enable_diversity
08/05/03   vm      Added Interface API for programming of PA Hysterisis
                   timer by Encoder different in Traffic state from Access
                   State
07/28/07   vm      Featurize the change for enc_tx_enable() in rf_tx_start
07/26/07   vm      Added the support for Tx Power Compensation NV based on
                   temperature and Frequency
07/26/07   adk     In rf_tx_start(), called enc_tx_enable() after TX parameters
                   are set. This fixes the TX spike observed in hard handoff.
07/22/07   dyc     Support for band class specific pa_r_map for SC2x mDSP TxAGC
                   implementation.
07/20/07   avi     Mainlined clockl regime management (previously featurized for
                   not MSM7600 targets)
06/29/07   vm      Made the change so that encoder controls the 
                   Tx_ON_EN with PA_ON_EN for Sc2X 
06/29/07   rmd     Set signal to bypass synth lock if synth lock is 
                   not supported.
06/12/07   ra      Fixed compiler error. Added rf hal macro.
06/01/07   vm      Added the support for Beta Scaling for SC2X
05/31/07   bmg     Added back an rfr_init() call if FEATURE_PMIC_MANAGED_LDO
                     is not enabled, otherwise it will not be called in
                     rf_init_cdma1x()
05/23/07   dyc     For SC2x keep PDM0 during init for LCD.
05/22/07   hkk/aak Enabling DC and AGC during handoffs to fix CFS issue
05/08/07   vm      Checked for RF_HAS_BYPASS_BAND_CHANNEL_CHECK  in
                   rf_init_hdr_band_chan() for HDR
05/01/07   adk     Added function rf_hdr_mdsp_tx_set_port_reg_address().
04/30/07   vm      Added support for New Tx Cal for RF chips with 
                   Tx Compensation registers in RFT
04/27/07   aak     Removal of support for ZRF and JIZRF
04/22/07   vm      Fixed the function rf_enable_diversity() for SC2X
04/18/07   vm      Added support for ADC and modem clock switching
                   for all the different modes of operation.
04/11/07   zg      Added GPS intelliceiver support.
03/31/07   ra      Added  RF_HAS_ALTERNATE_GPS_ASIC
03/26/07   vm      Added the support for Notch filters for 1x 
                   and diversity
03/26/07   sar     Removed function rf_get_hdet_val().
03/25/07   dyc     Remove RF_HAS_DEDICATED_RF_CARD implementation.
                   Added RF_HAS_BYPASS_BAND_CHANNEL_CHECK 
03/22/07   vm      Always Enabled the PA hysterisis timer for SC2X
03/21/07   vm      Wrapped the function call rf_set_prim_chain_1x_on_to_off()
                   in rf_sleep() based on RF_PATH_0
03/20/07   vm      Added the support for Tank FFA for BC15
03/16/07   dyc     Compiler warning fix.
           vm/dyc  Update chan/band checking for QSC60x5
03/15/07   sar     Rmoved library feature references.
03/14/07   sar     Added rf_get_hdet_val function.
03/14/07   rmd     Added Radio Configuration and temperature compensation for
                   HDET.
03/13/07   dyc     Fixed DACC register compiler warning.
03/13/07   aak     Fix for HHO in BC5
03/09/07   bmg     RTR6500 IntelliCeiver integration
           rmd     Updated the code to only configure GPIO's for the 
                   external PLL on chain 1 if we are using the LMX2433.
03/08/07   adk     Updated the function  rf_mdsp_tx_set_port_reg_address()
03/03/07   dyc     Merge support for QSC60x5.
03/01/07   sar     Fixed compiler warnings.
02/12/07   hkk     Multimode merge fixes
02/08/07   vm/adk  Added support for firmware TX AGC and debug
01/09/07   adk     Updated RXFRONT clk regimes.
01/09/07   adk     Removed NULL definition of clk_regime_enable()
01/08/07   jfc     Temporarily re-inserted rf_is_sleeping()
01/05/07   adk     Merged changes from 6800 Rel 6: Added and called new functions
                   rf_enable_rx_chip_power(), rf_disable_rx_chip_power(), and
                   rxf_hdr_wakeup(). (Commented out rxf_hdr_wakeup() for now.)
                   Updated RXFRONT clk regimes.
12/18/06   jfc     Renamed rf_is_sleeping() to rf_is_sleeping_cdma1x()
12/13/06   go/adk  Always treat band and channel to be changed (in function
                   rf_init_digital_band_chan) as a temporary fix.
12/12/06    hkk    Added support for D-450
12/08/06   jfc     Separate 1x and UMTS defs of RF_PA_DVS_VOLTAGE
12/07/06   trc     Modifications for MSM7600 register name change
11/14/06   adk     Call rflib_set_mode() if FEATURE_SBI_MDSP_CONTROLLER is defined.
11/07/06   ra      start cleanup for CGPS
10/31/06   ycl     define out rflib_set_mode(RF_SBI_MODE_ARM) for compilation.
10/23/06   aak     Fix for RF calibration problems when BC0 channel list is 
                   not pre-loaded
10/17/06   ycl     Modifications for initial MSM7600 build.
09/05/06   bmg     1x L1 violates preconditions of rf_init_digital_band_chan,
                   calls it when the radio does not start out asleep.  Added
                   IntelliCeiver pause/resume protection for intelliceiver_tune
                   call in this situation.
08/16/06   kss     Call optimized rxf_hdr_wakeup() for HDR.
08/10/06   bmg     Added rf_[enable/disable]_rx_chip_power to more carefully
                   control rfr_init() calls for multi-path RFRs.
08/15/06   kss     Added profiling strobes for HDR.
08/14/06   zg      Removed is_gps_rf_config_sgps() and hard-coded GPS to sGPS mode.
07/31/06   kss     Removed vbatt_read() from rf_init_hdr_band_chan().
06/15/06   pa      Include srch_lib_ext.h to pack correctly.
06/13/06   ans     Added call to rfr_idle_enable_interwork and rfr_idle_disable_interwork 
                   to maintain proper idle state of RFR when doing handoffs from one band 
                   to another.
06/06/06   pa      Updated code to use renamed 1x/tcxomgr APIs.
05/10/06   hkk     Backed out BBRX changes
05/04/06   hkk     BBRx sleep/wakeup - additional changes
04/17/06   hkk     Added changes for BBRX sleep/wakeup for current saving with Saturn
03/30/06   sar     Added support for ZRF6185.
03/29/06   bmg     Corrdinated DC management cleanup with 1x and HDR L1 to
                   improve DC offset acquisition.
03/07/06   hxw     Added RF_GPS_READ_AGC_DC_OFFSET() to read AGC and DC I/Q offset.
03/01/06   sar     Corrected the order of call to Rx and Tx interpolation in 
                   rf_tune_to_chan() for Diversity case. 
03/07/06   ra      Remove intelliceiver pause from rf_tune_to_chan. This was 
                   originally added for FTM. Changes were made to FTM which
                   eliminated the pause requirement.
02/22/06   go      Added intelliceiver pause to rf_tune_to_chan 
02/07/06   bmg     Save DC offsets when waking up in a different band than
                   the RF driver went to sleep in.
                   Fixed potential Lint error in previous checkin
02/06/06   go      Fixed TX channel list corruption when diversity is enabled.
02/02/06   dyc     For chain1 synth lock to use default value.
01/26/06   sar     Added function rf_tx_shutdown() to allow shutdown
                   while rf chain is in RF_STATE_CDMA_RXTX.
01/18/06   sar     Updated code to fix GPS error, "No sv's were found after 
                   first fix due to Wrong RF State".
01/16/06   rmd     Updated error message within rf_digital_band_change().
01/16/06   rmd     Implemented BC11 and BC14 code review results. Including:
                   * Mainline BC11 and BC14 code.
                   * Moving Band Class Translate code into rf_util_freq.c.
                   * Moving rf_get_valid_bc_and_mask() into rf_util_freq.c.
                   * Replacing ASSERT() with RF_ASSERT() and ERROR_FATAL() with
                     RF_FATAL_ERROR().
                   * Cleaning up rf_do_vco_coarse_tuning().
01/16/06   ra      remove rfr_sleep_disable in rf_digital_band_select - GPS mode
01/11/06   et      Lint fixes.
01/10/06   dyc     Lint fixes.
01/03/06   dyc     Lint fixes.
12/22/05   dyc     Lint fixes.                                                                        
12/12/05   ra      fixed same problem as below for transceiver 0 
12/12/05   vm      Fixed the problem of getting correct value of C/N in GPS 
                   Mode for Secondary chain in FTM Mode by disabling sleep
                   in rf_digital_band_select function.
12/07/05   ra      make rf_read_bc_translate_mask appropriately conditional
11/23/05   rmd     Added initial support to BC11.
11/22/05   hkk     Removed a duplicate write to bit 3 of Rx_AGCc_LNA_CTL register 
                   in rf_enable _diversity
11/22/05   bmg     Removed #ifdef RF_HAS_HDR from rf_hdr_is_enabled so it
                   can be used in the RF library.
10/31/05   rmd     Made sure that in rf_digital_band_select(), the new and
                   previous band get updated properly.
10/30/05   dyc     Added checks for rfr_is_valid() and rft_is_valid()
10/25/05    vm     Modified the function rf_wait_for_synth_lock() to get the 
                   PLL lock status for the synthesizer supported in RFT/ RFR
10/25/05   rmd     Removed BC14 debug messages.                   
10/17/05   sar     Updated rf_get_sub_class() to return uint32 type.
10/17/05   sar     Added function rf_is_band_chan_supported().
10/17/05   sar     Disable chain-0 in rf_init_delayed() after initialization
                   to save power if RF in not used immediately.
10/18/05   rmd     Updated code according to code review recommendations.                   
10/11/05   et      fixed interpolation delta_y buffer overflow and added
                   path parameter to converted_rx_channel_list table
10/07/05   bmg     Added RF API rf_has_high_sinr() for HDR modem init
09/26/05   dyc     Lint updates and misc cleanup for messaging.
                   Added RF_HAS_SRCH_WU_OPTIMIZATION to phase in WU opt code.
09/23/05   dyc     Changes to support separate invocation of RXF1 clock
09/19/05   dyc     Added support for changes related to RF_HAS_WU_OPTIMIZED 
09/19/05   rmd     Added support for band class 14 (US PCS + 5MHz).
09/02/05   rmd     Updated the code to support changing the type of
                   NV_RF_BC_CONFIG_I NV item from 32 bit 64 bit unsigned
                   number. 
09/12/05   bmg     Rearranged IntelliCeiver tune function to prime the RFR
                   for the upcoming power mode change before VCO tuning.
                   Removed a call to rf_update_freq_comp_items() in
                   rf_init_hdr_band_chan() because it is already called in
                   rf_digital_band_chan_init() in the same function.
09/12/05   dbc     Added support for external VCO with RFR6175 
09/02/05   hkk     Backed out park mode changes
08/17/05   hkk     Added a missing case statement for RFR6525 that was 
                   causing a fatal error in Intellicieiver  
08/17/05   ra      changed rf_bypass_rtc_ctl_path_on_off to handle device and on/off
08/13/05   ra      added support for bypass rtc(enable/disable)
08/09/05   dyc/zg  Update BB filter NV assignment.
08/01/05   zg      Always update NV with the value returned by 
                   rfr_do_filter_coarse_tune().
08/01/05   hkk     Added proper HDR featurization in rf_tx_enable
07/25/05   grl     Fixed trk_lo mux control function for Dora.
07/19/05   grl     Moved switching SYSTEM_MODE control to rf_tx_enable and out
                   of the basic tuning functions.
07/18/05   ra      removed dmod.h
07/15/05   sar     Added test condition to handle the first time bringup of 2nd chain
                   in rf_enable_diversity().
07/15/05   sar     Fixed rf_sleep() and rf_stay_asleep() to process both chains alike.
07/15/05   sar     Mainlined feature RF_HAS_ENHANCED_BAND_CLASS.
07/15/05   sfm     Name changes related to srch_afc/srch_tcxo merge
07/06/05   lp      Rearrange RF_HAS_NO_EXT_SYNTH block
07/05/05   hkk/    Added support for RFR6525
           bmg     
07/05/05   lp      Added RF_HAS_NO_EXT_SYNTH for SYNTH GPIOs
07/04/05   dyc     Lint cleanup.
06/24/05   et      fixed typo in rf_update_freq_comp_items() for rx_delta_x 
06/15/05   hkk     Removed definition of RF_TIME_STROBE_GPIO to gpio_6500.h
06/13/05   dbc     Removed support for RF_HAS_MFLO
05/27/05   zg      New support for TRM/sHDR:
                   - Added case for GPS in rf_init_digital() 
                   - Added case for GPS in rf_enable_digital_rx_pwr()  
05/27/05   sar     Removed TRK LO Control related code since it now resides in SRCH.
05/27/05   sar     Added statement to include srch_tcxo.h.
05/25/05   sar     Updated functions, rf_init_digital_band_chan(), 
                   rf_digital_band_chan_init(), rf_digital_band_change(), and 
                   rf_digital_chan_change() to support Rx Chain 1.
05/16/05   dyc     Added sleep check for rf_log... functions.
05/12/05   sar     Added support for APIs to query Band Subclass and Antenna 
                   Attributes.
05/09/05   ra      Replace arguments to gpio_tlmm_config with generic ones to ease portability  Also
                   add RF_HAS_RUNTIME_GPIO_CONFIG around RF_TIME_STROBE_GPIO defintion
                   For DORA: added RF_BYPASS_SYSTEM_CTL and RF_HAS_MODULATOR_TX_CTL
05/08/05   dyc     Comment updates regarding rf_init.
05/05/05   bmg     Fixed interpolation with reduced cal channel lists
05/02/05   dbc     Initial support for MFLO (RF_HAS_MFLO)
04/26/05   sar     Removed function rf_get_band_mask() and replaced all calls
                   with rfnv_get_band_mask().
04/26/05   dyc     Cleaned up unused power_mode declarations.
04/15/05   bmg     IntelliCeiver support for IM2 calibration
04/08/05   bmg     Moved a call to rf_intelliceiver_resume() into a
                   conditional check for RFR6500, protecting older chipsets.
04/06/05   bt/ra   Added RF_HAS_NO_RTC_DEPENDENCY. Also removed vbatt_read call
                   from rf_complete_warmup.
04/04/05   dyc     Add GPIO strobes for time profiling.
03/31/05   bmg     Updated VCO coarse tune for single-library with
                   IntelliCeiver.
                   Added IntelliCeiver initialization for RFR6500 in rf_init
                   Added IntelliCeiver support for 1x slotted sleep and HDR
03/30/05   lp      Merged MSM7500-RUMI emulation changes
03/28/05   sar     Added call to rfr_use_external_vco() in rf_disable_diversity()
                   to set the secondary rfr to use internal vco.
03/16/05   bmg     Added IntelliCeiver support for  VCO coarse tune and
                   1x tuning.
02/21/05   bmg     Added call to rf_set_rx_agc_mode() in
                   rf_digital_band_select() to insure LNA gain lineup
                   settings are configured at band changes.
02/17/05   zg      Prevented GPS VCO coarse tuning from happening on PRX
                   of RFR6500.
03/08/05   et      restored 3 SYSTEM_MODE fields in rf_init_pa_on_and_tx_on_ctls
                   under RF_HAS_GSM1X for restoring CDMA settings (specifically
                   TX related) when switching from GSM.
03/04/05   dbc     Do BB filter tuning for RFR6175 on second chain if
                   RF_HAS_BAND_CLASS_5_DIVERSITY is defined 
03/02/05   et      removed redundant call to rfc_set_mode in rf_init().
02/28/05   et      improved interpolation algorithm 
02/16/05   sar     Add call to synth_rfrx_pll_disable() in rf_enable_diversity to 
                   to make sure secondary chain synth is turned off.            
02/09/05   dbc     Call rf_enable_pa_ctl() in rf_tx_start()
02/06/05   et      changed FEATURE_GSM to RF_HAS_GSM1X since the changes
                   are related to GSM-1X mode switching.
02/04/05   et      featurized call to rflib_set_1x_sbi under FEATURE_GSM
02/04/05   et      added back in a call to rflib_set_1x_sbi() in rf_init(), 
                   which now only calls the sbi macro to restore the sbi
                   frequency.  (rather than calling sbi_init).
01/30/05   dyc     Enable PMIC DVS with FEATURE_RF_PA_DVS.
                   Update comments for rf_wait_for_synth_lock() usage.
01/20/05   et      backed out call to sbi_init api in rf.c because
                   it causes AGC problems in GSM as well as 1x.  Will
                   move it somewhere else.
01/19/05   sar     Added call to rf_update_freq_comp_items() in rf_enable_diversity()
                   to fix a bug affecting the proper operation of path 1 AGC  .
01/18/05   dyc     Featurized rflib_set_1x_sbi() with GSM.
01/17/05   ra      DORA merges
01/13/05   et      added call to rflib_set_1x_sbi in rf_init to restore
                   1X sbi frequency
01/06/05   sar     Updated RFRX1, RFRX2 and SYNTH LDOs control calls to reference
                   the new card specific functions in RFC modules.
01/05/05   ra      DORA merge
01/06/05   bmg     Bypass all RF initialization of the secondary chain if the
                   RFR device is NO_RFR
01/05/05   bmg     Set the secondary chain to the default band in
                   rf_init_delayed()
12/09/04   bmg     Expanded VCO coarse tune 2 to support 16 frequency bins
12/08/04   bmg     Improved FEBC fallback.  Do not fall back if CDMA & PCS
                   NV is also invalid.
12/08/04   bmg     Removed un-executed calls to rf_load_agc_limits()
                   -- legacy function that was never used in MSM6500 and above.
12/08/04   bmg     Added extended coarse tuning parameters support
12/15/04   dyc     rf_tune_to_chan() diversity chain1 sequencing update.
12/06/04   et      configuring the TX_ON_ALT_SEL so that the TX_ON pin is used 
                   for TX in 1X, when mode switching to 1x.  This is done in
                   rf_init_pa_on_and_tx_on_ctls()
12/01/04   sar     Fixed two more functions, rf_is_band_chan() and rf_tune_band_chan() 
                   to properly handle JCDMA with US freq.
11/30/04   sar     Fixed rf_init(), rf_get_band_chan() and rf_get_band_chan_temp() to 
                   properly handle JCDMA with US freq builds.
11/28/04   dyc     rf_tune_to_chan() disable DACC before tune and wait for
                   synth lock before DACC enable.
11/28/04   dyc     Display AGC and DC accumulator value in rf_pre_tune_to_gps()
11/11/04   sar     Modified rf_save_vco_coarse_tuning_results() function to copy
                   VCO CT values in the legacy bands also.
11/09/04   sar     Added support for setting of the RF Card default band at runtime.
                   Also modified rf_get_band_mask() to eliminate dependency on rf_init().
11/08/04   ans     Modified call of rfr_use_external_vco to rflib_rfr_use_external_vco.
                   New API added to rflib for using external VCO. rflib_rfr_use_external_vco() 
                   should be called instead of rfr_use_external_vco() to turn on external VCO.
11/01/04   et      calling rf_enable_digital_rx_pwr() if rf_is_sleeping()
                   at the top of rf_init_hdr_band_chan() so that 
                   the state will be in Rx mode rather than Sleep mode.
                   Enabled the RXF0 clock before calling rf_init_rf_gpios
                   for when switching from GSM to 1X mode.
11/01/04   dyc     Added rf_wait_for_synth_lock(), rf_get_unused_synth_lock_time()
                   rf_get_max_synth_lock_time()
10/29/04   ans     Removed PLL lock checking for RFR6135 because of difference in 
                   chip Architecture.
10/28/04   ra      Added logic for runtime support of FEATURE_T53 with US Frequencies
10/28/04   ans     Removed the special procedure for filter coarse tuning for KPZRF 
                   card and made it same as for other cards in function 
                   rf_do_filter_coarse_tuning().
10/21/04   bmg     Added initial RFR6500 support
10/19/04   sar     Putting back changes inadvertently lost by the last check-in.
10/13/04   bmg     Converted interpolation to the non-reference channel
                   offset_vs_freq structure.  Added power mode support.
09/30/04   sar     Relocated rf_nv_write() to rfnv.c file and changed references
                   to the calls to reflect the new name. 
07/29/04   whd     Added MSM6700 featurazation for new TX_AGC_ADJ_CTL register.
07/20/04   aks     Merge in Hidra PLT RF changes.
09/23/04   jac     Removed special case RFR6135 rfr_init() call and non-call.
09/20/04   dyc/ab  rf_pre_tune_to_gps() remove freq error processing. 
09/17/04   dyc     Added rf_get_nv_band()
09/16/04   aks     Adjusted channelization for new definitions of Band Class
                   5 and Band Class 11.
09/14/04   aks     Remove compile-time featurization in Interpolation code.
09/03/04   sar     Added featurization to limit band reverting to Jaguar targets
                   only.
09/07/04   dyc     Replaced tmcnv_write() with rf_nv_write().
09/03/04   dbc     Expected TX AGC versus HDET table is now retrieved from 
                   from the NV structure for the band.
08/29/04   dyc     Mainline RF_HAS_DECOUPLED_PA_CTL
                   Remove RF_HAS_IDLE_N usage.
08/24/04   dyc     Remove RF_HAS_FM_NV_TABLE usage.
08/23/04   sar     Replacing references to FEATURE_ENHANCED_BAND_CLASS with
                   RF_HAS_ENHANCED_BAND_CLASS.
08/19/04   dbc     Changes for ZRF617X (CDMA 450) RF card
08/17/04   dyc     Initialize RXF nv ptr to prevent null ptr access during
                   chain0 init.
08/16/04   dyc     Remove RF_HAS_FM and DB_RF_ANALOG components.
08/13/04   kss     Fix to remove glitch on TX_ON circuit: TX_ON enable is no 
                   longer done in rf_tx_enable() when in HDR mode.
08/12/04   sar     Added definition for rf_hw in rf_init_delayed() for FEBC.
08/10/04   sar     Added an interim rf_get_band_chan_temp() function to support
                   both rf chains.
08/06/04   sar     Temporarily backing out the last change.
08/04/04   sar     Updated rf_get_band_chan() to add device parameter.
07/26/04   et      added HDR interpolation support in rf_init_hdr_band_chan
07/14/04   sar     Added support for FEATURE_T53_WITH_US_FREQ. Also changed 
                   RF_800_BAND to RF_BC0_BAND in rf_interpolate_chan_offset().
07/13/04   jac     Added is_gps_rf_config_sgps() function. Added SGPS 
                   specific initialization for KPZRF.
07/13/04   sar     Added code in rf_init() and rf_init_delayed() to force MS to 
                   go to "svc required" mode if rf_hw_config was set out of range.
07/13/04   sar/jac Added support for band specific vco coarse tune.     
07/12/04    et     changed LEO_PROFILING to INTERPOLATION_PROFILING
07/08/04   dyc     Support writes to NV_C1_RFR_BB_FILTER_I for bb coarse tune.
                   Added HDR featurizations.
07/06/04   sar     Select ZRF6500 as default in rf_init() when NV_RF_HW_CONFIG_I 
                   is not defined.
07/02/04   dbc     Call cust_synth_gpio_init() in rf_init_rf_gpios() when 
                   RF_HAS_CUSTOMER_DEFINED_SYNTH is defined.
07/01/04   jac     Removed special case treatment of the RFR6135 BB filter
                   coarse tuning.
06/28/04   dyc     Update diversity chan status in rf_tune_to_chan
                   Add path reference to rfnv_rfr_bb_filter
                   Move TX_AGC_RESET from SRCH to rf_init.
06/17/04   zg      Mainline COMBINER_DISABLE_POWER_CONTROL() in rf_tx_stop() 
                   and clean up related RF_HAS_GPS wrapping.
06/16/04   jac     Fix to allow rf_do_filter_tuning() to run for RFR6135.
06/10/04   dyc     Update to control only what's required from cx8 group.
06/08/04   et/sar  Updated algorithm in rf_update_freq_comp_items() to manage
                   chain-1. 
06/07/04   dyc     Enable BB_FILTER checking for RF_HW_KPRF only (for now).
06/07/04   kss     Set system_mode to CDMA when entering sleep to save power.
06/07/04   sar     Updated references to interpolation constants in 
                   rf_update_freq_comp_items(). Updated rf_update_freq_com_items() 
                   to properly handle chain-1.
06/01/04   sar     Added rf_hw_id to chain status structure to aid in debugging.
05/25/04   sar     Updated for legacy band support.
05/21/04   sar     Updated for Feature Enhanced Band Class.
05/21/04   sar     Added API rf_get_supported_bands() to query rf supported bands.
05/21/04   dbc     updates for FEATURE_TX_POWER_BACK_OFF
05/20/04   va/dyc  Support for Trk_lo freeze/unfreeze using AFC APIs via
                   FEATURE_1X_SRCH_USES_ROTATOR_AFC
05/18/04    et     removed #include "dmddown.h"
05/10/04   dyc     rf_complete_sleep_warmup, update RXF DACC.
05/10/04   dyc     rf_tune_band_chan() and rf_get_band_chan_wait_time() update 
                   for tune time return.
05/10/04   dbc     Cleanup in rf_set_band_initializations()
05/05/04   dyc     Mainline COMBINER_ENABLE_POWER_CONTROL() in rf_tx_start().
                   Wrap CHIPX8 disable/enable in rf_init() in RF_HAS_FM.
05/05/04   jac     Turned on VCO coarse tune table load from NV for RFR6135
05/03/04   jac     Modified rf_do_filter_coarse_tuning and rf_do_resistor_calibration
                   to read values from NV and load them if valid.
04/30/04   zg      For SGPS, removed freezing/unfreezing TX AGC operation.
04/22/04   AP/dyc  Feature-ized disable/enable of DFM clock regime.
04/13/04   jac     Set synth device for RFR6135 path 1 back to NO DEVICE.
04/13/et   et      added interpolation calls in DIV and RECEIVER_1 cases
                   of rf_tune_to_chan.
04/13/04   dbc     Fixed some missing cases for synth_rfrx_pll_enable() change
                   on 4/12/04
04/12/04   dbc     Changed calls to synth_rfrx_pll_enable() to take db_rf_mode_type 
                   so that it can differentiate between CDMA and GPS modes.
                   Encapsulate all code relative to resistor calibration in
                   RF_HAS_TUNABLE_RESISTOR_CALIBRATION                   
04/12/04   jac     Added synthdev for path1 device config for KPCS
04/12/04    et     added featurization around JCDMA/IMT variable declarations
                   in interpolation code.
04/09/04   jac     Added call to do IQ line resistor cal for RFR6135. Added
                   support for KPZRF.
04/09/04   zg      Added support for Synth_LDO voting.
04/09/04   dyc     Provide defs for SYSTEM_MODE usage.
                   Move rf_set_pa_offsets() to rf_init()
04/08/04    et     added JCDMA and IMT support in INTERPOLATION
04/08/04   dyc     Updates to support removal of DFM VU dependency.
04/07/04   sar     Added Error messages in functions rf_pre_tune_to_gps(), 
                   rf_post_tune_to_gps(), rf_pre_tune_to_cdma() and
                   rf_post_tune_to_cdma().
03/31/04   dyc     Updated rfcom_to_path[] to map to RFCOM_RECEIVER_DUAL
03/19/04   et      added interpolation (frequency comp) support
03/09/04   dyc     rf_do_filter_coarse_tuning() support for pcs/imt band.
03/04/04   dbc     Use RF_HAS_CUSTOMER_DEFINED_SYNTH instead of RF_TEST_CUST_SYNTH.
                   Don't configure GPIOs for external synth in rf_init_rf_gpios() 
                   if the secondary chain synth is defined to be NO_SYNTH
03/01/04   kss     Removed lim_vs_freq factor from tx_pwr_limit calculation
                   in rf_tx_enable() and rf_tx_start().
02/20/04   kss     Add support for HDET and temp comp callbacks in HDR mode.
02/18/04   aks     Featurize the use of the HDET algorithm. When HDET is
                   bypassed, the tx power limit value should not include the
                   trim value for frequency.
02/09/04   dyc     Remove rf_init_bbrx_sleep()
02/06/04   dbc     Changes for ZIF library architecture update
02/04/04   ra      Define rf_exp_agc_table_ptr based on band class
02/04/04   dyc     Updated NV table check for BC0/BC1
01/30/04   et      added additional FEATURE_TRIMODE_ITEMS merge from cougar
01/30/04   zg      Fixed feature "RF_HAS_GPS".
01/30/04   dbc     Added calls to API function rflib_set_gain_line_up() to select
                   gain lineup for RFR6125. RF_HAS_MONOTONIC_LNA_GAIN_LINEUP has
                   to be defined.
01/30/04   et      Removed usage of RF_HAS_TRIMODE...use FEATURE_TRIMODE_ITEMS
                   or RF_HAS_PCS and RF_HAS_CDMA instead.
01/29/04   zg      Re-enable VREG_RFRX2 during Primary chain GPS search.
01/28/04   ra      modified NV table loading dependant on band class
01/28/04   dyc     Added VREG wait for 2nd chain pwrup in rf_init.
01/27/04   dyc     Fix warning in rf_cdma_sleep_wakeup_prep()
01/26/04   zg      Removed rft_init() from Receiver1 case in rf_init_digital().
01/25/04   dyc     No longer required to call rxf_set_mode_cdma() from rf_cdma_sleep_wakeup_prep()
01/23/04   zg      Fixed a trk_lo bug for GPS.
01/23/04   dyc     rf_enable_diversity, set the hdr_is_enabled[1] flag disabled.
01/23/04   ra      NV table loading dependant on band class
01/20/04   dyc     Added powerup waits for VREG enables in rf_enable_digital_rx_pwr()
01/14/04   sar     Added ram based dbg message logging feature (RF_DBG_SCRN).
01/09/04   sar     Disabled RX2 LDO in rf_sleep() for Secondary Rx chain.
01/13/04   zg/ar   - Fixed Trk_lo freeze/unfreeze for gps operation in PCG state.
                   - Function name update upon trk_lo driver's change in SRCH.
01/08/04   dyc     Support for RF_HAS_RX1_PLL_GPIO_CONTROL
12/17/03   dyc     Qualify trk_lo with SRCH_HAS_TRK_LO_CNTL in rf_complete_warmup.
                   Update rf_complete_sleep_warmup() to support SRCH sleep while RF is on.
12/01/03   dyc     Set proper chan main/max for FEATURE_T53_WITH_US_FREQ in rf_init_delayed()
11/21/03   sar     Allow rf_sleep() to disable chain-1 while in RF_STATE_GPS.
11/19/03   dbc     Don't call rfc_tune_synth() in rf_init_hdr() when the device
                   is RFCOM_RECEIVER_DIV
11/12/03   dyc     Provide check to initialize rf_pwr_call_back() only once.
                   Update rf_init_bbrx_sleep() to use MSM1.3 new cntrl bits.
11/11/03   bt      #ifdef clock regime control for TRK_LO PDM with 
                   SRCH_HAS_TRK_LO_CNTL.
11/10/03   et      Turned off TRANSMIT_VREGS in rf_init, and removed 
                   code to turn them off in rf_init_delayed.  This is to save
                   more power.
11/06/03   sst     Removed calls to trk_lo, wrapped in SRCH_HAS_TRK_LO_CNTL
                   Renamed RF_HAS_SRCH_CTLS_TRK_LO_AND_PILOT_GAIN to above
11/06/03   dyc     Remove rxf_reset from rf_complete_warmup.
11/05/03   dyc     Park VCO_0 and VCO_1 during rf_init_rf_gpios()
                   Added call to rxf_reset() to rf_complete_warmup() 
10/31/03   ra      Updated rx_tx_status in rf_tx_disable()
11/04/03   kss     Added rf_init_hdr_band_chan() function to help decrease
                   RF warmup time in HDR.
11/04/03   dyc     Wrap BYPASS_PA_SYNTH_LOCK_RAW bypass/non-bypass with RF_HAS_SYNTH_LOCK
11/04/03   sar     Updated parameter list in call to srch_tune_rfm().
10/31/03   dyc     Removed SLEEP_CTL accesses to non-existant SLEEP_N signal.
                   Consolidate rf_stay_asleep() and rf_stay_asleep_tcxo_pdm_on() with
                   rf_set_stay_asleep()
10/30/03   dyc     Execute rxf_init() prior to rf_init_digital() in rf_init_delayed()
10/27/03   dyc     Tx synth lock bypass - enable, PA synth lock bypass - disabled
                   Use rf_is_sleeping() in rf_cagc_debug()
10/24/03   sst     Wraped calls to srch_fing_set_dmod_pilot_gain with
                   SRCH_HAS_PILOT_GAIN_CNTL
10/24/03   dyc/gs  Remove GPIO65 init for PLT.
10/22/03   kss     Don't switch decoder bit in system_mode register; minor 
                   changes to rf_init_hdr().
10/22/03   dyc     Remove CLK_RGM_CDMA_PDM_M disable from rf_enable_digital_rx_pwr
                   Remove some #if 0 code.
10/15/03   et      replaced argument to pm_vreg_set_level() with mV instead
                   of register # and changed RF_HAS_PA_DVS to FEATURE_RF_PA_DVS.
                   Also moved turning on of RFRX2 LDO to RECEIVER_1 section
10/14/03   dyc     Mainline T_MSM6500 and omit FEATURE_HWTC
10/13/03   ra      Added JIZRF6500 support
10/13/03   dyc     Wrap rf_carrier_freq_err assignments with 
                   RF_HAS_SRCH_CTLS_TRK_LO_AND_PILOT_GAIN 
10/07/03   et      added PMIC3 VREG support and PA DVS support
10/07/03   et      featurized synth_lock checking and added device param
                   to rf_chk_synth_lock()
10/06/03   rs/dyc  Added GZRF6500 support
10/05/03   dyc     Provide default timeout for rf_get_warmup_time().
10/03/03   dbc     Set nv_digital_items_ptr properly in RFCOM_RECEIVER_1 case
                   in rf_digital_band_select(). 
10/02/03   dbc     Fixed syntax error in rf_get_warmup_time()
10/02/03   kss     Allow diversity state in state check in rf_init_hdr().
10/02/03   dbc     Changes for RFCOM_RECEIVER_1 device. 
10/01/03   dyc     Remove HDR related ifdefs around rf_is_sleeping()
                   Update rf_is_sleeping() for sleep_state1 and2
10/01/03   dyc     Use path parm vs device for rflib_set_agc_def_state()
09/25/03   kss     Load HDR calibration data in rf_digital_band_select().
09/24/03   dbc     Sleep chain 1 in rf_init_delayed() for RFCOM_RECEIVER_1. Fixed
                   bug in rf_do_vco_coarse_tuning() where path was hardcoded.
09/23/03   zg      GPS chain1 support. Receiver 1 fix in rf_digital_band_select()
                   and rf_init_delayed. Ruben's GPIO changes for PLL.
09/23/03   dyc     Update for FEATURE_T53 (JCDMA) support.
09/22/03   bt      Integrated with HDR hybrid mode.
09/19/03   dyc     Move dc and rxagc acq to end of warmup timeline to fix rxagc during wakeup. 
                   Misc cleanup to remove debug code.
09/15/03   dyc     Update rf_init_digital_band_chan() to allow tuning when band
                   change without need for FEATURE_PMIC_MANAGED_LDO
09/15/03   sar     Ruben's change for path 1 GPS support: Added gps card
                   initialization in rf_digital_band_select().
09/15/03   dyc     rf_get_frq_error() updated to use TRK_LO_ADJ_GET_FREQ_ERR
09/12/03   sar     Added device parameter to functions: rf_is_sleeping(),
                   rf_is_band_chan() and rf_post_cdma_reacq().
09/11/03   dbc     Added call to rf_update_lna_offsets() in the RFCOM_RECEIVER_1
                   case of rf_init_digital()
09/08/03   kss     Added HDR mode tracking for boths chains.
09/05/03   dbc     Added support for IMT band. Added tracking of previous RF
                   state & previous RF CDMA band in the chain state structures
09/05/03   sar     Changed rf_stay_asleep() function to support rfcom_device_enum_type
                   parameter.
09/04/03   et      replaced all the indirect uses of rfcom_to_path[device] with
                   local variable path
09/03/03   dbc     Support for NV_C1_VCO_COARSE_TUNE_TABLE_I added. Removed usage
                   of global variables that track the state of the RF driver. These
                   have been encapsulated in a structure so that the state of each
                   RF chain can be tracked independently.
08/27/03   dbc     Use NV_C1_CDMA_IM2_TRANSCONDUCTOR_VALUE_I to save RFR filter
                   coarse tune value for chain 1
08/26/03   sar     Added support for secondary Rx chain NV items.
08/26/03   dyc/ejv Updates for 6500 firmware revision 0x0029.  rf_get_frq_error()
                   Featured under RF_HAS_SUPPORT_MDSP_REV_0029
08/25/03   dyc     Merge in MSM6100 JCDMA binning in rf_freq_index_calc().
                   Supports temporary RF_HAS_SRCH_CTLS_TRK_LO_AND_PILOT_GAIN
                   Add rf_get_warmup_time() function.
08/21/03   kss     Initialize HDR properly when coming up from sleep.
08/19/03   dbc     Do GPS coarse tuning for chain 1 in rf_init_delayed(). Added
                   switch statement in rf_digital_band_select() based on
                   rfcom_device_enum_type.
08/18/03   dbc     Use new macro names for configuring RF GPIOs.  Added switch
                   statements to several functions that use rfcom_device_enum_type
08/11/03   dbc     Change functions that access/configure PLLs to go through
                   synth driver. Added rf_enable_diversity().  Added switch
                   statements to several functions that use rfcom_device_enum_type
08/09/03   dyc     Remove forced invocation of band change.
                   Update PROFILING tags.
                   Tmp change rfcom_to_path RF_PATH_DIV to use RF_PATH_1
                   Added path parms to rflib_set_agc_def_stat() and LNA macro
08/06/03   sar     Added enum to rf_path_enum_type to support diversity.
08/04/03   dbc     Include synth.h instead of synth.c
08/03/03   dyc     Remove bio_sw setting to disable sleep.
07/30/03   dyc     Merged GPS mode changes.
07/30/03   dyc     Added HDR system mode control.
07/30/03   dyc     Ch1 merge from Efy.
07/27/03   dyc     testing, re-enable sleep disable enable ch0 and ch1 bbrx via
                   ADIE test register.
                   rxf_wakeup_delayed removed
07/24/03   et      added chain 1 support
07/22/03   dyc     remove sleep disable from dip switch default setting
07/17/03   dyc     remove RL disable from dip switch default setting
07/15/03   dbc     Added initialization of ZIF 1X library for chain 1 (RF_PATH_1)
07/11/03   sar     Added support for multipath chipset drivers.
07/10/03   dbc     Changed name of AUX SBI signal types
06/26/03   dyc     removal of tx_msm3000_mode workaround
06/26/03   dyc     disable RL pwr cntl via switches variable
06/24/03   bmg     enclosed dfm_set_tx_pdm with RF_HAS_FM
06/20/03   dyc     configure gpio for synth_lock pin, re-enable tx_msm3000_mode for
                   debug workaround for Tx stuck at max
06/17/03   et      ported all the CLOCK code
06/17/03   dyc     removed debug of LNA override in rf_init_digital()
06/15/03   kss     Added initial HDR support.
06/13/03   dyc     use RF_LATCH_TX_GAIN_CTL_RD prior to reading TX_GAIN_CTL data
06/11/03   dyc     disable msm3000_mode for Tx, remove hard coded bypass synth lock value
06/11/03   et      cleaned up most magic numbers
06/05/03   et      added CAGC_RF_ before every #define that we use.
05/29/03   dyc     merged changes from Tx debug
05/28/03   et      merged change from orphans to disable TX PDM
05/27/03   et      integrated code from rxf0_panda_close()
05/27/03   dyc     addition Tx changes, PA synth bypass, PDM active
05/19/03   dyc     merged in msm6500/drivers/rf/rf.c#11 Tx related changes
                   updated cagc_debug function for 6500 registers
05/16/03   bmg     Removed RF_MODEx GPIO calls
                   Enabled extra chipx8 clocks in rf_init
                   #ifdefed out dfm_init when FEATURE_ACP isn't defined
                   Moved rf_disable_pa_ctl to above the switch in rf_tx_disable
05/13/02   et      fixed typo
05/13/02   et      ported msm6500 registers
05/12/02   et      added previously existing function rf_is_sleeping
                   under defined(FEATURE_HDR) && defined(FEATURE_SRCH_PCG)
04/28/03   et      merged MSM6500 register porting code
04/14/02   et      merged with MSM6100 3030 build
03/31/03   et      disabled sleep in rf_init() to facilitate bringup
03/26/03   aks     Change to not consider gps synth lock as an init fail.
03/03/03   dbc     Added rf_set_mdsp_cdma_spectral_inversion() to initialize RX
                   CDMA spectral inversion bit in mDSP when
                   RF_DSP_HAS_RX_SPECTRAL_INV_CONFIG is defined
02/20/03   aks     Changed rf_sleep() to pass in boolean, to stay backward
                   compatible.
02/20/03   dbc     Removed legacy comment from rf_init_digital()
02/14/03   aks     Adjusted rf_init_bbrx_sleep() with looped re-try, if comm
                   with the mDSP fails.
02/12/03   sar     Changed rf_sleep return type to void for multimode.
02/05/03   zg      Saved CDMA DACC offset earlier before GPS visit. Disabled
                   DACC during GPS tuning back procedure.
01/30/03   aks     Add a wait time for gps synth lock check.  Turned loop
                   into a timed wait, when using mDSP to init sleepb_bbr.
01/29/03   dbc     Back out SLEEP_N change made on 1/21/03
01/24/03   aks     Change return type of rf_init_delayed(), to pass back more
                   information than just ONLINE/OFFLINE.
01/24/03   dbc     Added support for RF_HAS_VCO_CONFIG
01/21/03   dbc     Only manipulate SLEEP_N when RF_USE_SLEEP_N is defined
01/21/03   aks     Remove init of sleepb_bbr line, exported function to be
                   called during MC init.  Don't wait forever on mDSP during
                   init.
01/20/03   dyc     Allow rf_init_delayed() to complete. Return status at func end.
01/16/03   zg      Removed the MSM6300 hack of disabling GPS VCO coarse tuning.
01/13/03   aks     Changes for GSM PLT.
01/08/03   dbc     Added rflib_set_vco_config() under RF_HAS_NV_VCO_CONFIG to
                   support internal/external VCO selection vbia the rf_config
                   NV item
12/20/02   dbc     Put CDMA wakeup initialization in rf_init_digital() into
                   separate function rf_cdma_sleep_wakeup(). Added
                   rf_digital_chan_change(), rf_digital_band_change(),
                   rf_digital_band_chan_init() and rf_init_digital_band_chan()
                   to allow band/channel changes within one function at
                   wakeup from slotted sleep in CDMA.
12/17/02   zg      Added rf_stay_asleep_tcxo_pdm_on() for GPS fake sleep.
12/17/02   dbc     Changed parameter in rf_get_band_chan() from int* to word*
12/13/02   aks     Export the function to set the sleepb_bbr line.
12/12/02   aks     Go back to the previous method of setting the sleepb_bbr
                   line, for MSM6300B.  Use the mDSP interface.
12/10/02   dbc     Enable GPS VCO coarse tuning for RFR6125
12/10/02   aks     Use previous FTM code change just for the MSM6300, until
                   validated on other MSMs.
12/10/02   aks     Remove reading of NV RF-cal items, while in FTM.
12/10/02   dyc     Disable TRK_LO_ADJ to reduce its transitions while clock is off.
12/05/02   aks     Disable GPS coarse tuning for RTR6300.
11/27/02   et      for SURFS, and ZRF6000 Rev A cards, set mux to use auxilary
                   SBI, under RF_HAS_AUX_SBI_MUX_SEL
11/26/02   aks     For MSM6300B, init sleepb_bbr line with direct reg write.
11/26/02   dyc     Added time profiling code using RF_TIME_PROFILING definition.
11/25/02   dbc     Added rf_get_band_chan() API function
11/25/02   dbc     Moved position of rfr_do_vco_coarse_tuning() in
                   rf_init_delayed(). Disable GPS coarse tuneing for RFR6125
11/20/02   dyc     Added de-registeration of DFM debug call back function.
11/13/02    et     changed the wrap of external vco check to RF_HAS_RTR6300
                   instead of T_MSM6300
11/13/02    dbc    Moved call to rfr_do_vco_coarse_tuning() in rf_init_delayed()
                   Wrapped rfc_pcs_gpio_macro and rfc_cell_gpio_macro
11/13/02    zg     Force LNA to the lowest gain state before tuning back from
                   GPS to CDMA.
11/12/02    et     added rfc_pcs_gpio_macro and rfc_cell_gpio_macro, and global
                   veriable rtr_vco_external which contains whether
                   the RTR vco is external/internal.
11/11/02   dbc     Added call to rfr_do_vco_coarse_tuning() in
                   rf_init_delayed() to support RF_HAS_RFR_VCO_COARSE_TUNING
10/30/02   dbc     removed include of rfigpio.h. Added rf_tune_band_chan()
                   and rf_get_band_chan_wait_time() API functions.
10/18/02   dbc     Use library access function rft_get_band_min_max_chans()
                   in rf_init_delayed() to get minimum & maximum channels for
                   a band.   Override LNA RANGE in sleep (MSM6000 only).
10/14/02   dyc     Removed #ifndef FEATURE_HWTC around FEATURE_BATTERY_CHARGER
10/07/02    zg     Fixed rf_cagc_debug problem on MSM6100 and up.
10/02/02   dbc     Added band dependent initializations to rf_init_delayed()
09/19/02   aks     Manually correct MSM/HWIO macro.  Add inlude files for
                   warnings.
09/17/02    zg     Featurized AGC_TRK_LO_ADJ_RD for MSM6100.
09/12/02    zg     Fixed a problem in Trk_lo restoring.
09/12/02    zg     Modified rf_get_frq_error() and GPS Trk_lo saving/restoring
                   (for msm6100 only).
09/10/02    et     added support for min/max channel setting for JCDMA
                   and KPCS.  currently it's compile time, but will change
                   to runtime in the future.
09/06/02   dbc     Remove usage of RF_GPIO_CDMA_PCS_RXTX_HI_MODE() and
                   RF_GPIO_CDMA_PCS_RXTX_LO_MODE() macros and replace with
                   RF_GPIO_CDMA_PCS_MODE()
09/04/02   dbc     Remove legacy code. Cleanup.
08/30/02   aks     Load and start the mDSP image, to init sleepb_bbr line,
                   for MSM6300.
08/29/02   dbc     Cleanup.
08/26/02   dbc     Remove usage of CAI_* when configuring bands in
                   rf_init_delayed().  Use rf_band_type instead.
08/21/02   dyc     Added FEATURE_HWTC changes based on msm6100/rf.c#6
08/15/02   dbc     Moved call to rflib_init_comm() to before rf_init_digital()
                   in rf_init_delayed().  Added support for management of SYNTH
                   LDO. Cleanup.
08/07/02    et     wrapped rflib_set_mode inside RF_HAS_MULTIMODE_SBI
08/02/02    et     added function call rflib_set_mode to control the SBI control
07/26/02   dyc     Added setting of LNA mode for KPCS. Wrapped some code in
                   rf_tx_enable() so it is not compiled for FEATURE_HWTC.
                   Use TCXO to drive frequency tracking PDM only when waking fron
                   CDMA sleep; otherwise use CX16.  Added call to
                   rflib_set_agc_def_state() in rf_init_digital().
                   Changes in rf_pre_tune_to_cdma() for internal test.
                   (merged from dbc,zg MSM6000_RF.01.00.15-16)
07/25/02   dyc     Updated to provide new defs for AUX SBI regs for MSM6000/6050
07/17/02   dyc     changed the TX SYNTH LOCK check to be bypassed in hardware
                   (merged from et rf.c -r1.118)
07/17/02   dyc     Added call to rf_init_cagc_linearizer_range() to determine
                   CAGC dynamic range using an NV item. Disable DACC when going
                   to sleep in CDMA.
                   (merged from dbc rf.01.00.13)
07/15/02   dyc     Added rxf_set_mode_gps_preswitch() during pre_tune_to_gps.
                   (merged from zg rf.01.00.12)
07/12/02   aks     Added initialization of the sleepb_bbr line, through the
                   mDSP interface.
07/09/02   dyc     Separate individual AUX SBI control regs for st, dt and clk
06/24/02   aks     Removed unused, included header file.
06/21/02   et      removed comment that is no longer relevant
06/19/02   et      added FEATURE_T53_WITH_US_FREQ to pass US freq
                   when FEATURE_T53 is turned on
06/17/02   et      renamed rfl_set_pll_mode to rfr_set_lna_mode
06/17/02   abh     Fixed a bug in rf_get_frq_error() which was returning
                   a garbage value (after adding Rev2.0 compatibilty).
06/13/02   et      renamed the rfr_I_Q_dac_set function and added
                   the transconductor NV item to be programmed by the function
06/13/02   dbc     Added disabling/enabling of rx front clock during sleep/wakeup
06/13/02   dbc     Change reading of carrier freq error for rev2.0 of MSM6050
06/12/02   zg      Disabled charger during GPS visit. Removed rfc_card_init_gps()
                   from rf_digital_band_select and called it at earlier time.
06/12/02   dbc     Changes for CDMA wakeup from sleep in rf_init_digital().
                   rxf_cdma_wakeup() called instead of rxf_wakeup().  Call to
                   rxf_cdma_wakeup_dc_acquire() added.
06/11/02   dbc     Changes for LDO management.  Sequence changes for the synth
                   lock test, CDMA sleep, CDMA wakeup from sleep and tuning to
                   channel.
06/07/02   zg      Fixed the problem of saving GPS DACC values at wrong time.
06/06/02   dbc     Don't retune synths in rf_init_digital() when waking up from
                   sleep.
06/05/02   jwh     Wrapped up GPS VCO tuning in rf_init_delayed() for HWTC.
06/05/02   jwh     Wrapped rf_update_lna_offsets() for HWTC
06/03/02   zg      Wrapped up GPS VCO tuning in rf_init_delayed() for minix.
05/31/02   zg      Added GPS VCO coarse tuning in rf_init_delayed().
05/30/02   dbc     Added call to RF_GPIO_PARK_MODE() for parking RF related
                   GPIOs during sleep.  Updated debug features.
05/24/02   jwh     FEATURE_HWTC
05/23/02   et      added support for programing the LNA w/ JCDMA mode
05/22/02   dbc     Add call to rf_set_cagc_linearizer_range() in rf_init_digital()
                   so that linearizer range is set properly whenever CDMA mode
                   is entered.
05/22/02   dbc     Added call to rf_set_cagc_linearizer_range() to set dynamic
                   range of CAGC
05/20/02   dbc     Wakeup from CDMA sleep modifications to rf_init_digital()
                   and rf_complete_warmup()
05/16/02   dbc     Modified rf_init_digital() to minimize reprogramming of
                   the rf chips after waking up from sleep when LDOs are left
                   on. Moved vbatt_read() from rf_init_digital() to
                   rf_complete_warmup().
05/10/02   et      added support for configurable GPIOS for the AUX_SBI line
                   added support for KPCS and JCDMA
                   added fix for illegal TX_ON during hard handoff problem
05/06/02   dbc     Added loading of DACC with IM2 calibration settings in
                   rf_tune_to_chan()
05/01/02   dbc     Merged another phone_t change in rf_tune_to_chan()
05/01/02   dbc     Merged phone_t change in rf_tune_to_chan()
04/30/02   dbc     Fixed code that cause MSM6000 compile error.
04/29/02   dbc     Added call to dfm_set_cdma_mode() in rf_init_delayed() for
                   initialization to CDMA. Removed call to
                   rf_enable_cdma_lna_temp_comp() in rf_init_digital() when
                   PHONE_T is defined.
04/18/02   dbc     Removed all usage of DFM mode for frequency error restoration
                   after CDMA sleep.  Wrap code for non-phone_t targets.
04/15/02   dbc     Added usage of rft_idle_enable(), rft_idle_disable(),
                   rft_rx_pll_enable(), and rft_rx_pll_disable(). Moved TX_ON
                   control from rf_enable_tx_pwr() to rf_tx_enable()
04/11/02   dbc     Undefine RF_CAGC_DEBUG
04/10/02   dbc     Fixed compile bug for rf_sleep()
04/10/02   dbc     Removed shutdown of AGC, PDM, SBI clocks until further
                   analysis can be done.
04/09/02   dbc     Add bbrx sleep.  Added shutdown of AGC, PDM, SBI clocks
                   during sleep.  Removed the turning on of PA in
                   rf_enable_tx_pwr().  Changed rf_sleep() interface to match
                   with mainline Search.
04/05/02   dbc     Added rft_enable() in rf_tx_start() and rft_disable() in
                   rf_tx_stop().  Moved placement of rxf_save_accum_data() in
                   rf_init_analog() and rf_digital_band_select(). Added call
                   to rfr_pll_enable() in rf_init_digital().
03/28/02   sar     Merged from MSM. Changed rf_sleep() and added
                   rf_store_1x_sleep_params() function. Changed some MSG levels.
03/20/02   sar     Merged from MSM. Featurized the changes of re-defining
                   RFC_ENABLE_RX_PWR_WAIT & RFC_ENABLE_TX_PWR_WAIT with two global
                   macros, respectively.
03/20/02   sar     Merged from MSM, Added call to rf_cdma_temp_update() to
                   ensure that rf_cdma_tx_lim_vs_temp_now receives the correct value
03/24/02   dbc     Wake up rfr in rf_init_digital() before tuning to a channel
                   based on the current band.12/06/01
03/24/02   dyc     Wrapped dfm_sw_slotting_disable() with ifdef RF_HAS_FM
03/22/02   dbc     Changed argument to rf_set_rx_agc_mode() in rf_init_analog()
03/22/02   sar     Added call to rxf_set_vga_gain_offset() in rf_tune_to_chan
                   to set Rx Front vga gain offset with proper frequency
03/22/02   dyc     Replace dfm_disable_dfm_rxagc() with dfm_set_cdma_mode()
                   Moved rf_get_rssi() and rf_deglitch_data() to dfmagc.c
                   Implement dfm_update_lna_offsets()
03/22/02   dbc     Re-enabled calls to rfr_idle_enable() and rfr_idle_disable()
03/21/02   aks     Include rxf.h.
03/21/02   aks     Included RF Library header.  Replaced code with API call.
03/18/02    zg     GPS tuning cleanup.
03/18/02   dbc     Use NV item for the pll lock counter in rf_init_digital()
03/18/02   dyc     Restore freq track LO without going into DFM mode.
03/17/02   dbc     rf_init_analog() now calls rf_set_rx_agc_mode() to initialize
                   the AGC for analog mode
03/17/02   dyc     Disable slotted sleep isr when entering digital and exit of DFM
03/16/02   dbc     rf_init_rf_nv_items() now calls rf_init_tx_cagc_delayed() and
                   rf_init_rx_cagc_delayed() to initialize CAGC items that
                   require NV to have been read
03/15/02   dbc     Removed enabling of TX in rf_init_delayed(). Moved turning on
                   of rfr LDO to rf_enable_digital_rx_pwr().
03/11/02   aks     Removed initial CDMA band setting for Trimode.  Inits to
                   either Cell or PCS.
03/08/02    zg     Rewrote rf_pre/post_tune_to_gps(),rf_pre/post_tune_to_cdma
                   for GPS tuneaway & tuneback.
03/08/02   dbc     Merged SleepOne changes.  Turned on managed LDOs for ZIF.
03/05/02   sar     Disabled calls to rfr_idle_enable and rfr_idle_disable functions.
03/05/02   dyc     Restore the dfm_restore_freq_track() functionality.
03/01/02   dyc     dfm_sw_slotting_disable() update removed for release.
02/28/02   dyc     Added dfm_sw_slotting_disable() when exiting DFM mode.
02/28/02   sar     used RF_HAS_ZIF to wrap calls to rfr_idle_disable and
                   rfr_idle_enable.
02/28/02   sar     Added call to rxf_save_accum_data in rf_init_digital and
                   rf_init_analog functions to save rxf accumulators. Merged
                   PLT changes to main line from the branch.
02/22/02   dbc     Removed RF_NOT_PORTED for synth lock check in
                   rf_init_delayed() (MSM6050)
02/22/02    zg     Changed GPS part for tuneaway & tuneback
02/14/02   aks     Add rf_is_band_chan(), mainlined code asssociated with
                   FEATURE_ENC_BTF_IN_NV, change to use
                   srch_fing_set_dmod_pilot_gain() name.
02/13/02    et     featurized I_Q_dac_set function inside RF_HAS_IM2
02/13/02   dbc     ZIF logging support added.  rfr_sleep_enable() only invoked
                   when Managed LDOs not used.
02/06/02   aks     Change format from T_MSM_* to T_MSM*.
02/06/02   dyc     Initialize RF_MODE_SEL register for DFM.
                   Use dfm_read_rx_agc_filter() instead of DFM_READ_RX_AGC_FILTERED.
01/31/02    zg     Merged in GPS related changes from PhoneT.
01/31/02   sar     Fixed the Sleep in QRF5600. Removed ZIF only switch.
01/30/02   aks     Put rxfront to Sleep only for ZIF mode.
01/30/02   aks     Wrapped DACC section properly.
01/30/02   aks     Wrapped DACC section in RF_HAS_ZIF context.
01/30/02   aks     Load DACC with IM2 calibration.  Freeze/unfreeze CAGC
                   value for Sleep transitions.
01/20/02   dbc     Moved LNA gain state sleep settings inside
                   RF_HAS_FORWARD_LNA_CTL define.
01/29/02   dbc     Added LNA gain state settings for Sleep operation.  Add
                   rxf_set_mode() when transition from analog to digital.
01/24/02   aks     Removed all usage of our_rxf_init().  Add enable of SBI
                   hardware request channels to rf_init_delayed().
01/22/02   dbc     Mainlined setting of Rx CAGC control to internal in
                   rf_init_digital().
01/17/02   sar     Added calls to rxf_sleep, rxf_wakeup and rxf_wakeup_delayed
                   to support rxf sleep.
01/14/02   dbc     Consolidated all RF debug features to be local to this file.
12/21/01   aks     Partial breakapart of the phone_t function, our_rxf_init().
12/20/01   dbc     Addeded callback function for CAGC debugging.
12/18/01   bgc     Changed reads of CARRIER_FREQ_ERROR to use MSM6000
                   workaround.
12/18/01   aks     Disabled Sleep.  Added function to initialize the rxfront,
                   needs to be broken apart.  Merged in PMIC changes which
                   removed support to the PM1000 and added support to the PM60X0.
12/07/01   dbc     Commented out check for synth lock after tuning to hi/lo
                   channels in rf_init_delayed(). This fails for MSM6050 and
                   needs to be debugged.
11/28/01   dyc     Replace DFM_INITIAL register initalization with MACRO call.
11/28/01   dbc     Moved dfm initialization outside of RF_HAS_9_BIT_DC_OFFSET
                   wrap in rf_init_digital().
11/28/01   aks     Add back call to rft_enable(), when in rf_enable_tx_pwr().
11/21/01   dbc     More RF Driver decomposition changes. DFM_OUTM call to DFM
                   Initial register changesd to DFM_OUTHM. Support for
                   FEATURE_FSG_SAT_CONTROL feature added.
11/15/01   aks     Remove reference to IFR.
11/13/01   aks     Remove the use of RF_HAS_RFT.
11/12/01    et     changed rfr and rft function calls, and added
                   rfr_enable calls to the different modes in rf_digital_band_sel
11/12/01   aks     Removed phone_t specific calls.  Add rxf_set_mode() call
                   after rxf_init() call.
11/02/01   tn      moved rxf_init after rf_digital_init, added rxf_set_mode
                   to digital_band_select and rf_init_analog.
                   ifdef'd rf_enable_cdma_lna_temp_com for phone_T.
11/02/01   dbc     Removed backwards compatible (pre MSM6000) code.  Added more
                   initialization for MSM6000+ registers. Isolated use of I/Q DC
                   DC offsets within RF_HAS_9_BIT_PDM_DC_OFFSET.
10/26/01   dbc     Added support for MSM6000 TX_2EARLY_PCG_CTL register.
                   Temporary modifications for MSM6000 CARRIER_FREQ_ERR_RD
                   hardware bug gated by T_MSM_6000.
10/25/01   dyc     Replace dfm_agc_init() with dfm_init()
                   Moved dfm_sw_slotting_init() to dfm_analog_init()
                   Rename dfm_set_mode() to dfm_set_dc_offset_mode()
10/25/01   dbc     Added support for TX_WARMUP register (MSM6050). Moved setting
                   of AGC limits into a separate function rf_load_agc_limits().
10/20/01   aks     Fixed syntax in rf_set_tx_idle_n_state().
10/19/01   dbc     Added more support for MSM6000 and MSM6050 in rf_init(),
                   rf_init_delayed(), rf_enable_tx_pwr(), rf_tx_disable(),
                   rf_digital_band_select(). Local functions rf_init_idle_n(),
                   rf_set_tx_idle_n_state(), rf_init_pa_on_and_tx_on_ctls()
                   were added. External API function rf_set_agc_dc_loop_gain()
                   added.
                   RF_CAP_PDM1_FOR_TX, RF_CAP_PDM2_FOR_TX, RF_HAS_PA_RANGE_CAL
                   support removed. FEATURE_PA_RANGE_TEMP_FREQ_COMP mainlined.
10/17/01   dyc     De-coupled DFM related code and replace with DFM functions
                   and MACROs
10/17/01   dbc     Migrated CAGC specific macros from msm_drv.h to this file.
                   Ported GPS specific code to MSM6000+. Removed unused functions
                   rf_get_i_offset(), rf_get_q_offset(), rf_set_frq_error()
10/08/01   aks     Added init for FM sw slotting and for RX Front.  Merged
                   phone_t changes.
10/01/01   dbc     Made changes for port to MSM6000+ targets.  Added initial
                   support for the new PMIC API. Merged RFR/RFT5600 changes.
09/21/01   sar     Merged from MSM_RF.00.01.15: Cleaned up rf_init_delayed's
                   tri-mode conditional. Loaded rf_exp_agc_table_ptr and
                   rf_nv_dig_items_ptr
                   in rf_enable_digital_rx_pwr.
09/19/01   dbc     Removed RF_HAS_RFR3100 dependencies - SW LNA Control removed
                   for CDMA and FM.
09/14/01    dc     De-composed DFM related initializations to dfm.c
                   Added dfmi.h, calls to dfm_agc_init() and dfm_slotting_init()
09/12/01   dbc     Added function definition for GPS function rf_get_lna_state()
09/11/01   aks     Included header files.
07/20/01    et     changed all the ifr code to be a generic api call to
                   generic Rx functions.  Those functions, in the rfr files
                   will handle whichever chips is being used .. ifr/rfr...
07/02/01   aks     Created this module to contain only the exported functions
                   from the RF device driver.  Based upon rfmsm.c, rev 1.10.
                   O:/src/asw/COMMON/vcs/rfmsm.c_v   1.10   21 May 2001 16:40:16
04/21/01   dd      Set rf_nv_dig_items_ptr to point to PCS or CDMA NV items in
                   rf_init_digital.
05/16/01   dd      Added IFR3500 support.
04/10/01   tn      merged the latest gps changes from ppg.
04/04/01   tn      added macro calls to disable/enable closed loop power control
                   replacing direct explicit calls to the register.
03/22/01   tn      Added a call to disable RFR3x00 prior to disabling the LDO.
                   Disable FM slotting mode in rf_enable_digital_rx_pwr.
03/19/01   tn      disabled adjustment of r2 and r3 rise/fall value since they
                   are not used (set to maximum value). referenced AGC_RDWR_CTL_WB
                   directly due to MACRO conflict.
03/08/01   tn      Code modification for IS2000 TX AGC. Added an API to get
                   the programmable PA WARMUP delay.
02/28/01   fas     Added IS2000 TX AGC code.
02/28/01   tn      Added AGC_CTL8 initialization to rf_init(mask delay and MSM3100 mode).
                   loaded CAGC_RX_PDM_DELAY_VAL with 0x70 chipx8.
02/23/01   day     Merged in FEATURE_FACTORY_TESTMODE change to enable a runtime choice.
                   Mainlined T_SLEEP.
02/06/01   day     Mainlined Tx power limit setting code that had been conditionally
                   compiled for trimode.  Removed legacy code in rf_hw_lna_setup_analog()
                   that forced LNA's to low gain mode causing FM sensitivity tests to
                   fail.
01/09/01   djd     Removed commented-out code to set spectral polarity (this
                   is set in the rf_init_card functions). Removed redundant
                   initialization that worked around PCS spectral polarity
                   problem.
12/15/00   fas     Undid inversion of Q_OFFSET polarity for MSM3100C
12/13/00   rmd     Chip bug fixed: Removed worked around for the RAS RAM counter.
12/11/00    et     added more support for FEATURE_FACTORY_TESTMODE
12/04/00   day     Added in digital rx enable/init after tx is disabled in
                   rf_init_delayed.  PCS digital rx was not enabled properly
                   after completing rf_init_delayed.
10/18/00   fas     Merge from MSM5000 and PPG.
10/09/00   dsb     Removed TG== code.  Mainlined T_SURF.  Removed ADC_EXT_MUX
                   support.  Changed FEATURE_SURF3100 to T_MB_SURF_3100.
10/06/00   fas     Work around chip bug in which RAS-RAM pointer is not reset to zero
                   by AGC_RDWR_CTL, bit 0, RAS_RAM_WR = 1. Write the entire RAS-RAM
                   each time.
09/29/00   dsb     Added support for FEATURE_ENC_BTF_IN_NV.  Removed support
                   for RF_RFT_USES_IDLE.
09/07/00   jcw     Merged in support for tx_gain_adj_step_size() from MSM5000.
09/06/00   bgc     Added FEATURE_FACTORY_TESTMODE.
08/29/00   dsb     Cleaned up WEB_MISC_WR_WB write in rf_init, such that it
                   uses the appropriate mask.
08/25/00   dsb     Removed HDET_RANGE GPIO.  It is no longer necessary for
                   Trimode builds.  Support for renamed RF_GPIO_* mode
                   switching macros.  Removed INTLOCK free/save around
                   rft_init (in rf_init), as there is already a lock
                   inside rft_init.  Added support for GPS into
                   rf_digital_band_select.  Added support for new DB mode
                   DB_RF_GPS.  Added BSP support for cellular Rx spectral
                   inversion in rf_sleep.  Added RF_GPS_BAND support to
                   rf_init_digital.  Removed rf_enable/disable_gps.
07/25/00   dsb     Added support for RF_USES_NEW_MODE_PINS (replace FM_N and
                   BAND_SEL).
07/24/00   day     Merged changes for 'Tune away without drop' capability under
                   FEATURE_GPSONE.
06/23/00   dd      Merged PPG/PRF3300 driver with PP 3020 driver.
06/08/00   dsb     PM1000 charger support (include charger.h).   Added
                   support for MSM3300 IDLE pin register interface.  Removed
                   FEATURE_AARDVARK.
05/08/00   dsb     Added support for new GPS_MODE GPIO register bindings.
                   Featurized AARDVARK specific code.  More code is necessary
                   to allow Rosette to compile with this module.
04/12/00   dd      Added GPS support
01/27/00   mk      Deleted CHIPX8 clock regimes disable in rf_stay_asleep().
                   They are gated automatically by ASIC sleep.
01/21/00   tn      RFT3100 A2 settings are featurized under RF_HAS_RFT3100_A2.
                   Added a feature RF_RFT_USES_IDLE for testing RFT3100 synth
                   lock only when it has power.
01/17/00   tn      Added support for Trimode HDET switching. Added support
                   for RFT3100 A2.
01/11/00   mk      Disabled LOG_BAND_CHANGE_C log packets in trimode (using
                   feature RF_LOG_BAND_CHANGE).
                   Featurized code in rf_tx_enable() using RF_HAS_FM.
01/06/00   jc      Added Trimode Support (RF_HAS_TRIMODE) and calls to
                   acpwb_txfm_enable.
12/09/99   ymc     Renamed rf_cdma_band_type as rf_card_band_type;
                   Renamed RF_CDMA_BAND_800MHZ as RF_800_BAND;
                   Renamed RF_CDMA_BAND_1900MHZ as RF_PCS_BAND;
12/02/99   tn      During unfreeze call, the LNA RANGE bypass is place in normal mode.
11/29/99   tn      Added IQ offset freeze and unfreeze before and after sleep.
11/03/99   ymc     Integrated functions rf_set_frq_trk_acc and
                   rf_get_frq_trk_acc.
                   Included msm_drv.h for those two functions.
11/01/99   mk      Moved initialization from msm3000_2300...() to
                   rf_init().
10/29/99   ymc     - Set PCS DA maximum current to 24mA during RF initialization.
                   - Reorder PA_ON_WRITE in rf_enable_tx_pwr() so that PA is the
                     last one to be turned on.
                   - Moved rf_cdma_band_type declaration to rfifreq.h
                   - Passed rf_curr_cdma_band into rfc_init_card() instead of
                     rf_config.  rf_config is not supported any more.
10/29/99   ymc     Renamed rfc_retune_for_rxtx as rfc_retune_synth_for_rxtx.
                   Renamed rfc_retune_for_rx   as rfc_retune_synth_for_rx.
                   Renamed RF_CDMA_CHAN_MIN_FREQ as RF_800MHZ_CHAN_MIN_FREQ.
                   Renamed RF_CDMA_CHAN_MAX_FREQ as RF_800MHZ_CHAN_MAX_FREQ.
10/29/99   tn      Added two functions to enable freezing the CAGC block while
                   loading the linearizer tables.
                   Cleaned BB2 comments. Deleted code under RF_HAS_BB2 and
                   RF_CODEC_USES_BB2_IDLE. Removed all codec comments and defines
                   and rf_codec_on code. Removed all voc_set_msm_mode calls. Removed
                   FEATURE_TCXO_SHUTDOWN feature code. Mainlined T_MSM3 and T_MSM31.
                   Removed all code under RF_HAS_IFT.
           jc      Added FEATURE_PM1000 battery charging function calls.
09/16/99   ymc     Integrated r1.5 change: FEATURE_TCXO_SHUTDOWN is not defined for
                   MSM3100 SURFs.
09/15/99   ymc     Moved IDLE_N_OE_MASK writing to rf_init() only;
                   Set RFT3100 register bit, USB/LSB select, to 1 for
                   supporting PRF3000.
09/13/99   jc      Moved sbi_init to mc.c because pm1000 also requires it.
09/10/99   mk      Added suppport for clk_regime_misc_xxx(). Fixed IDLE_N
                   on/off.
08/05/99   mk      Disabled voting mechanism for TCXO shutdown support,
                   this feature is not implemented yet for MSM3100.
08/02/99   jc      Removed new gpio_xxx MSM3100 support.
07/28/99   tn      Removed AGC_RESET during init_digital, it was causing the TX_ADJ
                   to have a zero value,this will cause the DM to assume that
                   Tx Power is ON.
07/28/99   tn      TXDAC_SLEEP_MASK control is ifdef'd for RFT only.
07/15/99   mk      Added support for MSM3100 feature RF_HAS_INTERNAL_CODEC.
07/15/99   jc      Added new gpio_xxx MSM3100 support.
07/15/99   tn      Added support for RFT3100 and internal Tx I/Q DACs.
05/18/99   tn      Added support for new SBI.
03/25/99   kss     Updated Digby support; disabled TX_PATH powerdown on IFT.
03/15/99   rv      Changed RF-ENC Interface.
03/12/99   kss     Added support for IFT and IFR on Digby.
03/08/99   kss     Fixed casting problem in rf_get_tx_gain_adj().
02/12/99   kss     Optimized rf_init_digital for faster warmup times.
02/02/99   kss     Added support for the extended linearizer range.
01/27/99   thh     Added interpolation for PDM1 and PDM2 freq. comp. to
                   enhance performance.
                   Broke up several long lines.
01/26/99   kss     Added TCXO shutdown support via a voting mechanism.
01/13/98   kss     Fixed some #ifdefs which were target based rather than
                   RF capability based.
1/5/99     rv      CDMA clk rgm turned off on a transition from CDMA to FM.
                   If the vocoder needs chipx8 during a CDMA to FM transition,
                   it is no longer reported as an error.
11/23/98   rv      Minor modifications as a result of the synthesizer
                   files reorganisation.(Function replaced by macro,
                   Constant names changed from RF_.. to RFC_.. etc).
11/13/98   thh     Scope the rf_pdm_freq_comp_calc() function inside the
                   RF feature RF_CAP_PDM?_FOR_TX accordingly.
10/27/98   thh     Fixed compilation problem with macro ROM.
10/15/98   thh     Added frequency compensation support for PDM1 and PDM2.
10/06/98   kss     Added MSM3000 support:  Removed srch_dmod_reset call for
                   MSM3; workaround for 1.25 ms ISR HDET read (due to long
                   interrupt service latency).
09/18/98   jjn     If vocoder and some other caller wants to keep chipx8 on,
                   chipx8 is now kept on and error is recorded.
09/12/98   jjn     The previous change did not turn off chipx8 even if no
                   one was trying to keep it on.  Also, rf_codec_on was
                   being called to turn on chipx8 given rf_state which would
                   not result in its being turned off.
07/24/98   thh     The codec can now be held on by the Vocoder during CDMA-to-
                   FM transition.  If there is illegal attempt during to hold
                   Chipx8 on, rf_codec_on(..., false) is now called to force
                   Chipx8 off instead of directly using the macro as
                   previously.
07/13/98   kss     DFM PDMs set to non-override mode in rf_init_analog() to
                   fix problem where analog wasn't working after sleep
07/01/98   jjn     Conditionally compiled manipulation of RF_GPIO_FM_R for
                   WLL targets.
06/24/98   kss/ajn Added rf_complete_warmup(), which supports having a faster
                   warmup time and replaces the RF/SRCH LNA interface. The
                   IQ DC offset PDMs and TRK_LO_ADJ are now preloaded during
                   the warmup period.
06/21/98   jjn     Update from code review.
06/05/98   thh     Added temperature compensation for PDM1 and PDM2, each using
                   a different compensation table.
06/05/98   thh     Changed the switching regulator frequency (YAMN clock) to
                   492KHz for the Q-target.
05/14/98   bgc     Added sync to codec clock in rf_init_analog()
05/06/98   kss     Fixed problem where rf_init assumed that FM was disabled
                   without explicitly disabling it (this caused a problem in
                   the Module). Moved PCS_BAND_SELECT initialization to
                   rfc_init_card in the card driver. Removed code in rf_init
                   that was using nv items not yet read.
05/05/98   jjn     Added support for QCT-7x00.  This involves a round about
                   way of reading NV due to the fact that the rest of WLL
                   targets do not us RF_CONFIG NV item.
04/09/98   thh     Added temperature compensation for PDM1 and PDM2.  These
                   PDMs are currently being used for in transmit only.
03/18/98   thh     Fixed spelling error.  Change TQ to T_Q and TT to T_T for
                   Q-target and TGP.
02/12/98   thh     Rx/Tx gain comp is targetized for current existing target.
                   Introduced new functions rf_set_pdm1() and rf_set_pdm2().
                   These functions driven by the feature RF_CAP_PDM1_FOR_TX and
                   RF_CAP_PDM2_FOR_TX in rficap.h.
                   If the Rx AGC limits value are valid, set the limits.  Else,
                   Rx AGC outputs are not limited.
02/24/98   jjn     Fixed the problem where rf_nv_dig_items_ptr was getting
                   used without first being initialized.  This was only
                   occuring when NV was rebuilt.  In that case,
                   rf_init_delayed exited early without initializing
                   rf_nv_dig_items_ptr.
                   Even though rf_init_delayed returns MC_OFFLINE_DIGITAL_F,
                   MC still calls rf_init_digital, where rf_nv_dig_items_ptr
                   was accessed.
                   Also, removed support for MSM2P rev 3 for WLL phones.
                   Those chips were never in ISS2 phones.
02/19/98   jjn     WLL phones do not easily conform to the new architecture.
                   Conditionally compiled some statements that would cause
                   some older WLL phones problems.
02/18/98   thh     RxTx/Rx re-tune requests are made only when we transition
                   between Rx and RxTx.  No re-tune request is made when we
                   go to Rx from Sleep or FM Rx and CDMA Rx.
02/10/98   thh     Added capability to re-tune the synthesizer during
                   Rx-to-RxTx and RxTx-to-Rx transition.  This feature is
                   defined by RF_CAP_RXTX_RETUNE in rficap.h.
02/08/98   jjn     Read RF CONFIG bits only if RF CONFIG NV item is not
                   used by target.
01/19/98   thh     Fixed a bug where the wrong freq. bin index was calculated
                   and used leading incorrect comp of FM Rx & Tx power.
12/19/97   thh     Fixed the problem where the FM AGC error always round up,
                   never rounding down, when it was negative.
12/11/97   jjn     Used generic GPIO register names defined in rfigpio.h
                   rather than specific ones.  For instance, one target
                   may define RF_GPIO_PCS_BAND_SEL_R as DEC_GPIO_0 and
                   another target may define it as ENC_GPIO_1.  Either
                   way rfmsm should be able to service both targets with
                   the same statement(s).
12/08/97   thh     Software was taking control of the LNA too soon after
                   sleep.  A new interface allowing Searcher to notify the
                   RF driver when to take over the LNA, allowing Searcher to
                   warm up and the Rx-AGC to close and converge.
11/17/97   jjn     Partitioned ISS2 RF to conform to the new RF architecture
11/17/97   thh     When disabling Chipx8, we now clear the rf_codec_ctl mask
                   and the rf_chipx8_needed flag accordingly.  An error is
                   asserted if the mask is not clean prior to explicitly
                   clearing it.
10/31/97   thh     Fixed the Rx-sensitivity problem in dualbands.  We were
                   not initializing the band-select BPIO bit as output.  As
                   a result we were unable to select low-band filter.  Fixed.
10/17/97   thh     Fixed the problem where the Tx AGC would rail in FM mode
                   when the FM error slope is 0.  When error slope is 0, add
                   the comp value to the calibrated Tx setting vs power.
10/08/97   thh     Added MSM2.3 Clock Regimes support.  This change is MSM2.2
                   compatible.
09/05/97   thh     On this date, the one module RF unit/driver was split up
                   into several modules and header files.
                   Keep local copy of current channel and RF mode instead of
                   using db_get() everytime it is needed.
                   Refer to the appropriate RFC?.C for legacy change comments.
===========================================================================*/

#include "target.h"
#include "customer.h"

#ifndef FEATURE_GSM_PLT

#include "cagc.h"
#include "clkregim.h"
#include "comdef.h"
#include "db.h"
#include "enc.h"
#include "err.h"
#include "msm.h"
#include "msm_drv.h"
#include "rf.h"
#include "rfi.h"
#include "rft.h"
#include "rfr.h"
#include "rflib.h"
#include "rxf.h"
#include "srch.h"
#include "adie.h"
#include "ts.h"
#include "rfcom.h"
#include "synth.h"
#include "rf_cust_synth.h"
#include "rfifreq.h"
#include "rfnv.h"
#include "rf_prof.h"
#include "clkrgm_modem.h"

#ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO
#include "bbrx.h"
#include "rf_adc_samp_freq.h"
#endif  
                                                        
#ifdef RF_HAS_POWER_MODE_LIB
#include "rf_intelliceiver.h"
#endif /* RF_HAS_POWER_MODE_LIB */

#if defined(FEATURE_BATTERY_CHARGER)
#include "charger.h"
#endif

#if defined(FEATURE_PMIC)
#include "pm.h"
#endif

#ifdef FEATURE_FACTORY_TESTMODE
#include "test_mc.h"
#endif

#ifdef FEATURE_ZIF_LOGGING
#include "log.h"
#endif

#ifdef RF_HAS_HDR
#error code not present
#endif

#ifdef RF_HAS_MDSP_TX_AGC
#include "rf_mdsp_tx_agc.h"
#endif

#if defined(RF_HAS_HDET_COMP_FOR_RC_AND_TEMP)
#include "rftxplim_mm_cdma1x.h"
#endif

#include "srch_lib_ext.h"
#include "tcxomgr.h"

#include "rf_hal.h"

#ifdef RFGPS_HAS_INTELLICEIVER
#error code not present
#endif /* RFGPS_HAS_INTELLICEIVER */

#ifdef RF_HAS_PDM_VS_POWER_TX_CAL
#include "rfagc_tx_pdm_vs_pwr_cal.h"
#endif /* RF_HAS_PDM_VS_POWER_TX_CAL */

/* Test definitions */

/* Deprecated hardware configurations */
#undef RF_TEST_NO_PATH_1_DEVICES 
#undef RF_TEST_NO_PATH_1_SYNTH

/* Test chain 1 operation */
#undef RF_TEST_CHAIN_1

#ifdef RF_TEST_CHAIN_1
#error code not present
#endif


/* RF library API */
extern boolean ziflib_get_lna_range_override( rf_path_enum_type path );

#ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO
extern rf_adc_clk_state_type rf_adc_clk_status;
rex_crit_sect_type  rf_adc_clock_crit_sect;
boolean rf_adc_clock_crit_sect_init = TRUE;
extern void rf_adc_sel_rxf_sample_clk_src(uint32 clk_src);
#endif /* RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO */

/* This Variable keeps track of the Band change for 
loading the new Linearizer table*/
#if defined (RF_HAS_MDSP_TX_AGC) || defined(RF_HAS_PDM_VS_POWER_TX_CAL)
boolean rf_cdma_load_master_lin_tables = TRUE;
#endif

#ifdef RF_HAS_PA_ACCESS_PROBE_CONFIG
boolean rf_pa_config_is_traffic_state = FALSE;
#endif

#ifdef RF_HAS_MDSP_TX_AGC
#ifdef RF_HAS_QSC60X5
#error code not present
#endif
/* To Provide the PA Map to MDSP */
#define RF_CONVERT_1X_HW_TO_MDSP_PA_MAPPING( map_val ) \
  ((( map_val & 0xC0) >> 6) | (( map_val & 0x30) >> 2) | (( map_val & 0x0C) << 2) | (( map_val & 0x03) << 6))

#endif /* RF_HAS_MDSP_TX_AGC */

#ifdef RF_HAS_SUB_BLOCK_SUPPORT
extern boolean rf_block_mask_is_forced[RF_PATH_MAX];
extern uint32 rf_forced_block_mask_value[RF_PATH_MAX];
extern rfnv_digital_items_type* rf_forced_nv_item_ptr[RF_PATH_MAX];
#endif

#ifdef RF_SUPPORTS_32_BIT_TX_AGC_INTERFACE
#define RF_MAX_PDM_VAL_FOR_TX_GAIN 63
#else
#define RF_MAX_PDM_VAL_FOR_TX_GAIN 511
#endif

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

                      RF COMPENSATION CONSTANTS

=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

                           TIME CONSTANTS

=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
/* -----------------------------------------------------------------------
** Time values (in microseconds) for clk_busy_wait() delays
** ----------------------------------------------------------------------- */

/* Constant:  microseconds per millisecond
*/
#define RF_US_PER_MS ((word)1000)


/* Microseconds needed for RF synthesizers to settle before testing ALL_LOCK
** Use the millisecond value defined in rf.h, subtract the clock tick bias,
** and expand it to microseconds.
*/
#define RF_TUNE_CHAN_WAIT_US \
  ((word)( (word)RFC_TUNE_CHAN_WAIT * RF_US_PER_MS ))

/* Microseconds needed for RF Tx or Rx hardware to settle when turned on,
** before it can be programmed.  Use the millisecond value defined in rfc.h,
** and expand it to microseconds.
*/

#ifdef FEATURE_RF_WAIT_CHANGES
#error code not present
#else
    /* wait 10 ms after enabling RX power */
    #define RFC_ENABLE_RX_PWR_WAIT     10
#endif

#define RF_ENABLE_RX_PWR_WAIT_US \
        ((word)( RFC_ENABLE_RX_PWR_WAIT * RF_US_PER_MS ))

/* Microseconds to wait for RF sleep warmup to complete
*/
#define RF_COMPLETE_SLEEP_WAIT     250

/* Microseconds to wait for Rx AGC acquistion to settle
*/
#define RF_RX_AGC_SETTLING_US      600

/* Microseconds to wait (iterations) for PLL to synth lock */
#define RF_SYNTH_LOCK_ITER_WAIT_US 100

/*  If MDSP has CDMA Rx spectral inversion config use the following */
#if defined( RF_DSP_HAS_RX_SPECTRAL_INV_CONFIG )
#if defined( T_MSM6300 )
#error code not present
#elif defined( T_MSM6100 )
#define DSP_RX_SPECTRAL_NORM  0x00C0
#define DSP_RX_SPECTRAL_INV   0x02C0
#else
#error code not present
#endif /* T_MSM6100 */
#endif /* RF_DSP_HAS_RX_SPECTRAL_INV_CONFIG */


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

                          MISC DEFINITIONS

=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
/* -------------------------------------------------------------------------
** Provide equivalent multi reg SBI definitions for MSM6050 and MSM6000
** ------------------------------------------------------------------------- */
#if defined ( FEATURE_AUX_SBI ) && !defined ( T_MSM6100 )
#error code not present
#endif

/* -------------------------------------------------------------------------
** RF Driver debugging flags
** ------------------------------------------------------------------------- */

/* Enable/Disable CAGC debugging */
#undef RF_CAGC_DEBUG      

/* Enable/Disable Ram based debug screen function */
#undef RF_DBG_SCRN
#undef RF_DEBUG

#ifdef RF_DEBUG
#error code not present
#endif /* RF_DEBUG */

/* -------------------------------------------------------------------------

                            LOCAL DATA TYPES

---------------------------------------------------------------------------- */
/*
** RF DBG SCREEN Definitions
*/
#ifdef RF_DBG_SCRN
#error code not present
#endif /* RF_DBG_SCRN */

#ifdef RF_HAS_LINEAR_INTERPOLATION
/* object that store all the offset_vs_freq items */

rf_offsets_vs_freqs_type rf_freq_comp;
#endif

boolean rf_qsc11x0_sleep_enable_flag =  TRUE;

#ifdef RF_TIME_PROFILING
#error code not present
#endif /* RF_TIME_PROFILING */

#ifdef RF_HAS_WU_TIMED_DEBUG
#error code not present
#endif /* RF_HAS_WU_TIMED_DEBUG */

#ifdef RF_CAGC_DEBUG
#error code not present
#endif


static uint32 rf_warmup_time_rxadc_start = 0; // Keeps track of time tick when RF_VREG_S2_START_VOLTAGE is set 

#define RF_VREG_S2_START_VOLTAGE      2450 /* Start voltage of 2450 mV */
#define RF_VREG_S2_STOP_VOLTAGE       2200 /* Stop voltage of 2200 mV */
#define RF_VREG_S2_START_SETTLE_TIME  100  /* Settle time of 100 Usec for Vreg Voltage*/ 
#define RF_VREG_S2_STOP_SETTLE_TIME   750  /* Settle time of 750 Usec for Vreg Voltage*/ 

#define RF_RXADC_WAIT_TIME            2000 /* Wait time before the Vreg S2 Voltage is changed */          

/* Uncomment to enable debug messages for RF_HAS_RXADC_VOLTAGE_SWITCH */
//#define RF_VREG_S2_DEBUG_MSGS              




/* -------------------------------------------------------------------------

                           SHARED RF DRIVER DATA

---------------------------------------------------------------------------- */

rf_path_enum_type rfcom_to_path[RFCOM_MAX_DEVICES] =
{
    RF_PATH_0,    /* RFCOM_TRANSCEIVER_0    */
    RF_PATH_1,    /* RFCOM_RECEIVER_1       */
    RF_PATH_1,    /* RFCOM_REVEIVER_DIV  (treat same as path 1 */
    RF_PATH_1     /* RFCOM_REVEIVER_DUAL (treat same as path 1 */
};

/* NV data
*/
rfnv_all_data_type rf_nv_data = {
  FALSE,            /* RF config loaded? Valid RF CAL?    */
  NULL,             /* Ptr to Common NV items    */
  NULL,             /* Ptr to CDMA (BC0) NV items  */

#if defined (RF_HAS_BC1_NV_TABLE) || defined (FEATURE_TRIMODE_ITEMS)
  NULL,             /* Ptr to BC1 NV items   */
#endif
  
#ifdef RF_HAS_BC2_NV_TABLE
  NULL,             /* Ptr to BC2 NV items   */
#endif

#ifdef RF_HAS_BC3_NV_TABLE
#error code not present
#endif

#ifdef RF_HAS_BC4_NV_TABLE
  NULL,             /* Ptr to BC4 NV items   */
#endif

#ifdef RF_HAS_BC5_NV_TABLE
  NULL,             /* Ptr to BC5 NV items   */
#endif

#ifdef RF_HAS_BC6_NV_TABLE
#error code not present
#endif

#ifdef RF_HAS_BC11_NV_TABLE
#error code not present
#endif

#ifdef RF_HAS_BC14_NV_TABLE
  NULL,             /* Ptr to BC14 NV items  */
#endif

#ifdef RF_HAS_BC15_NV_TABLE
  NULL,             /* Ptr to BC15 NV items  */
#endif

#ifdef RF_HAS_GPS_NV_TABLE
#error code not present
#endif /*RF_HAS_GPS*/
  
  NULL              /* Ptr to FM NV items    */
  
  #ifdef RF_HAS_SUB_BLOCK_SUPPORT

  #ifdef RF_HAS_BCX_BLOCK_0_NV
  ,NULL
  #endif

  #ifdef RF_HAS_BCX_BLOCK_1_NV
  ,NULL
  #endif

  #endif

};


/* The RTR revision i.e B1, B2.. */
//static boolean rtr_vco_external;  /*lint -esym(714, rtr_vco_external) -esym(551, rtr_vco_external)*/

/* Pointer to NV FM items/tables
*/
rfnv_fm_items_type *rf_nv_fm_items_ptr;

/* Pointer to NV Common items/tables
*/
rfnv_common_items_type *rf_nv_common_items_ptr;

/* Call back struct for power control 
*/
clk_cb_type rf_pwr_call_back;

/* Flag to init call back only once 
*/
static boolean rf_pwr_call_back_initialized = FALSE;

/* Expected TX AGC vs HDET reading table pointer */
byte *rf_exp_agc_table_ptr;

/* In CDMA RxTx (only), is the Tx circuit currently on or off.
*/
rf_tx_status_type rf_tx_status = RF_TX_IS_OFF;

/*  Global structures for tracking the state of each RF chain */
rf_chain_state_type rf_chain_status[2] =
{
    { RF_STATE_SLEEP_2,
      RF_STATE_SLEEP_2,
      DB_RF_SLEEP,
      RF_DEFAULT_BAND,
      RF_DEFAULT_BAND,
      0,
      0,
      RF_HW_UNDEFINED,
      NULL,
      0,
      &rf_wu_stats[0]
    },

    { RF_STATE_SLEEP_2,
      RF_STATE_SLEEP_2,
      DB_RF_SLEEP,
      RF_DEFAULT_BAND,
      RF_DEFAULT_BAND,
      0,
      0,
      RF_HW_UNDEFINED,
      NULL,
      0,
      &rf_wu_stats[1]    
    }

};

/* 
* Current value of the PA_R_MAP which
* can change based on band dependent pa_r_map value.
* If the band class specific entry is not defined, then
* the common pa_r_map value (from which the default value is also
* derived from the initialization of the nv struct) will be used.
*/
byte rf_current_pa_r_map = 0;

/* Configuration of RF chains */
rf_path_0_config_type rf_path_0;  
rf_path_1_config_type rf_path_1;
boolean rf_device_cal_mode_enabled = FALSE;


/* -------------------------------------------------------------------------

                          LOCAL DATA

---------------------------------------------------------------------------- */
/* local table of pointers to digital items tables for all modes */
#ifdef RF_HAS_SUB_BLOCK_SUPPORT
rfnv_digital_items_type *rf_digital_nv_table[RF_PATH_MAX][RF_MAX_BAND];
#else
rfnv_digital_items_type *rf_digital_nv_table[RF_MAX_BAND];
#endif

/* local table of pointers to digital items tables for all modes */
static rf_card_band_type cai_to_rf[RF_MAX_BAND];

#ifdef T_SC2X_PBS
#error code not present
#endif

/* Keep track of whether digital initialization has completed. */
static boolean rf_dig_init_complete = FALSE;

#if (!defined(T_FFA) && defined(RF_HAS_WU_TIMED_DEBUG)) || \
      defined(INTERPOLATION_PROFILING)
#error code not present
#endif

#ifdef RF_HAS_EGPS
#error code not present
#endif /* RF_HAS_GPS */

/* RF Card Id for current target */
rf_hw_type rf_card_in_system = RF_DEF_HW;

#ifdef FEATURE_ZIF_LOGGING

/*------------------------------------------------------------------------*/
/* Logging Structures                                                     */
/*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/

static struct
{
  log_iq_offsets_type log;

  int  inx;
    /* Index into the AGC log buffer */

  int  drops;
    /* Count of how many times we've dropped log packets for lack of a
       buffer */
}  
rf_iq_offsets_vars; 

/*------------------------------------------------------------------------*/

static struct
{
  log_dacc_iq_accum_type log;

  int  inx;
    /* Index into the AGC log buffer */

  int  drops;
    /* Count of how many times we've dropped log packets for lack of a
       buffer */
}
rf_dacc_iq_accum_vars;

#endif /* FEATURE_ZIF_LOGGING */

/* Track HDR status. Note: This is referenced to PATH, not device.
   This status variable may be necessary inside the RF library, so
   it cannot be featurized.  In non-HDR targets, the state should
   never become true, effectively disabling HDR at runtime, instead
   of compile time in the RF library.
*/
boolean rf_hdr_is_enabled[] =
{
  FALSE,        /* RF_PATH_0 */
  FALSE         /* RF_PATH_1 */
};

/* Track the power-up state of chips like the RFR6500 and RFR6525
   that support single-chip diversity or SGPS.  This insures that
   rfr_init() will only be called once per power-up.  This state
   variable should be managed only by the accesor functions
   rf_enable_rx_chip_power() and rf_disable_rx_chip_power(). */
static boolean rf_single_chip_multipath_power_state[RF_PATH_MAX] = 
{
  FALSE,
  FALSE
};

#ifdef RF_HAS_MSMA_CURRENT_REDUCTION_CONTROL
#error code not present
#endif

//#ifdef RF_HAS_SUPPORT_FOR_RICO_BRINGUP

boolean rf_debug_fixed_pdm_is_enabled = FALSE;

byte rf_debug_pa_override_val_bc0 = 2;
int2 rf_debug_pdm_value_used_bc0 = 15;

byte rf_debug_pa_override_val_bc1 = 2;
int2 rf_debug_pdm_value_used_bc1 = 16;

byte rf_debug_pa_override_val_bc5_bc11 = 1;
int2 rf_debug_pdm_value_used_bc5_bc11 = 23;

//#endif

/* -------------------------------------------------------------------------

                          LOCAL MACROS

---------------------------------------------------------------------------- */


/* -------------------------------------------------------------------------

                          LOCAL FUNCTION PROTOTYPES

---------------------------------------------------------------------------- */
#if defined(RF_HAS_BC_SPECIFIC_TX_PA_MAP_VALUE)
static void rf_update_tx_pa_map_with_bc_specific_value( \
                                              rfcom_device_enum_type device);
#endif /* RF_HAS_BC_SPECIFIC_TX_PA_MAP_VALUE */

static byte rf_freq_index_calc(word channel, rf_card_band_type );
static void rf_init_pa_on_and_tx_on_ctls( void );
static void rf_init_rf_gpios(void );

static void rf_digital_band_chan_init
(
    rfcom_device_enum_type,
    boolean band_changed,
    boolean chan_changed,
    rf_card_band_type band,
    word chan
);

static void rf_digital_chan_change( rfcom_device_enum_type device, word chan );

static void rf_digital_band_change( rfcom_device_enum_type device,
                             rf_card_band_type band );

static void rf_cdma_sleep_wakeup( rfcom_device_enum_type device );

static void rf_set_band_initializations( rfcom_device_enum_type device );

static void rf_do_resistor_calibration( rfcom_device_enum_type device );

static void rf_do_vco_coarse_tuning( rfcom_device_enum_type device );

static void rf_save_vco_coarse_tuning_results( rf_card_band_type band, rf_path_enum_type path, 
                                        const vco_coarse_tune_params_type *vco_coarse_tune_params_ptr );
#ifdef RF_HAS_SUB_BLOCK_SUPPORT
static void rf_save_block_vco_coarse_tuning_results( rf_card_blocks_type block, rf_path_enum_type path, 
                                        const vco_coarse_tune_params_type *vco_coarse_tune_params_ptr );
#endif

static void rf_do_filter_coarse_tuning( rfcom_device_enum_type device );

static rf_card_band_type rf_get_first_bc_in_mask( void );

//#ifdef RF_HAS_SUPPORT_FOR_RICO_BRINGUP
extern void ftm_cdma_set_tx_agc(int2 pdm_val);//160
extern void ftm_set_pa_r1_r0(byte pa_val); //0
//#endif

#ifdef RF_HAS_LINEAR_INTERPOLATION
void rf_update_freq_comp_items ( rf_path_enum_type path, word chan);        //used by FTM
uint32 rf_interpolate_chan_offset (  rf_path_enum_type path, word chan );   //used by FTM
static uint32 rf_bc3_chan_to_freq( word channel);
#endif

#ifdef RF_HAS_Y1Y2_DBG_BUS
#error code not present
#endif

#ifdef RF_CAGC_DEBUG
#error code not present
#endif

/* inlined RF WU support functions */
__inline static void rf_wu_synth_start (rfcom_device_enum_type device, db_rf_mode_type rf_mode);
__inline static void rf_wu_tx_setup(rfcom_device_enum_type device);
__inline static void rf_wu_init_rf_devices(rf_path_enum_type path);
__inline static void rf_wu_load_im2(rf_path_enum_type path, rf_chain_state_type *rf_chain_ptr);
__inline static void rf_wu_rxf_rtc_setups(rf_path_enum_type path);
__inline static void rf_wu_start_acq(rfcom_device_enum_type device, rf_path_enum_type path);
__inline static void rf_wu_set_agc_track(rfcom_device_enum_type device, rf_path_enum_type path);

/* Required reference for HWTC */
void rf_init_rf_nv_items( void );

void rf_set_periodic_wait_timer(uint32 total_wait_time);

/*===========================================================================

FUNCTION RF_POST_CDMA_REACQ                             EXTERNALIZED FUNCTION

DESCRIPTION
  Perform RF initialization required following CDMA pilot re-acquisition.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rf_post_cdma_reacq( rfcom_device_enum_type device )
{
  /* -----------------------------------------------
  ** Re-load the CAGC linearizers to avoid bug where
  ** Rx AGC droops over several sleep intervals.
  ** ----------------------------------------------- */
  rf_load_cagc_linearizers(device);

  /* Finish RF init after warmup if it didn't get called */
  if (!rf_dig_init_complete) 
  {
    rf_complete_warmup();
  }

} /* rf_post_cdma_reacq */

//#ifdef RF_HAS_PA_ACCESS_PROBE_CONFIG
/*===========================================================================
FUNCTION RF_SET_PA_ACCESS_PROBE_CONFIG

DESCRIPTION
This function is called by Mux Layer to infrorm the RF Driver
that its going to be in traffic state.
This is done so that RF Driver can program different PA hysterisis timer
and PA switchpoints for Access Probes different from traffic.

DEPENDENCIES
  None

RETURN VALUE
  Sets the variable used for traking when the traffic state begin
  and ends 

SIDE EFFECTS
  None

===========================================================================*/
/*void rf_set_pa_access_probe_config(rfcom_device_enum_type device ,boolean is_traffic_state)
{
  if (is_traffic_state)
  {
    rf_pa_config_is_traffic_state = TRUE;
  }
  else
  {
    rf_pa_config_is_traffic_state = FALSE;
  }
}*/
//#endif


#ifdef RF_HAS_MDSP_TX_AGC
/*===========================================================================

FUNCTION RF_MDSP_GET_TX_DOUBLE_BUFF_WRITE_STATUS        

DESCRIPTION
This function checks whether the latched flag (Slot/Frame) is cleared
or not for the double buffered MDSP varaibles. This flag set for Slot
or Frame boundary whenever software writes to MDSp double buffered 
parameters.

DEPENDENCIES
  None

RETURN VALUE
Boolean
Returns TRUE if the latched flag is cleared otherwise returns FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean rf_mdsp_get_tx_double_buff_write_status(rf_mdsp_tx_double_buff_params_type params,
                                                rf_mdsp_tx_double_buff_latch_type latch)
{
    if (params == TX_LIN_PARAMS)
    {
      if (latch == FRAME_LATCH)
      {
        if(CDMA_MDSP_GET_TX_LIN_FRAME_PARAMS_LOAD_STATUS() == 0)
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
      }
      else if (latch == SLOT_LATCH) 
      {
        if(CDMA_MDSP_GET_TX_LIN_SLOT_PARAMS_LOAD_STATUS() == 0)
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
      }
      else
      {
          RF_FATAL_ERROR("Latch Value not configured properly for Linaerizer double buffered params");
          return FALSE;
      }
    }
    else if (params == TX_LIMIT_PARAMS)
    {
      if (latch == FRAME_LATCH)
      {
        if(CDMA_MDSP_GET_TX_LIMIT_FRAME_PARAMS_LOAD_STATUS() == 0)
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
      }
      else if (latch == SLOT_LATCH) 
      {
        if (CDMA_MDSP_GET_TX_LIMIT_SLOT_PARAMS_LOAD_STATUS() == 0)
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
      }
      else
      {
           RF_FATAL_ERROR("Latch Value not configured properly for TX Limit double buffered params");
          return FALSE;
      }
    }
    else if (params == PA_THRESH_PARAMS ) 
    {
      if (latch == FRAME_LATCH)
      {
        if(CDMA_MDSP_GET_PA_THRESH_FRAME_PARAMS_LOAD_STATUS() == 0)
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
      }
      else if (latch ==SLOT_LATCH) 
      {
        if(CDMA_MDSP_GET_PA_THRESH_SLOT_PARAMS_LOAD_STATUS() == 0)
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
      }
      else
      {
          RF_FATAL_ERROR("Latch Value not configured properly for PA Threshold double buffered params");
          return FALSE;
      }
    }
    else
    {
      RF_FATAL_ERROR("double buffered params have not been entered properly");
      return FALSE;
    }
}

/*===========================================================================

FUNCTION RF_MDSP_TX_SET_PORT_REG_ADDRESS                                       

DESCRIPTION
 This function is used to set the SSBI ports as well as SSBI register addresses
 at which MDSP sends TX AGC updates in the 1X mode

DEPENDENCIES
MDSP Clocks must be ON when this function is called

RETURN VALUE
  None

SIDE EFFECTS
None
===========================================================================*/
void rf_mdsp_tx_set_port_reg_address(void)
{  
  uint16 ssbi_port;
  uint16 reg1_addr;
  uint16 reg2_addr;
  #ifdef RF_SUPPORTS_32_BIT_TX_AGC_INTERFACE
  uint16 reg3_addr;
  uint16 reg4_addr;
  #endif

  #ifdef RF_SUPPORTS_32_BIT_TX_AGC_INTERFACE
  rft_get_tx_agc_pdm_write_reg_address(&ssbi_port, &reg1_addr, &reg2_addr, &reg3_addr, &reg4_addr);
  #else
  rft_get_tx_agc_pdm_write_reg_address(&ssbi_port, &reg1_addr, &reg2_addr);
  #endif

  CDMA_MDSP_SET_TX_SSBI_PORT(ssbi_port);
  CDMA_MDSP_SET_RFT_REG1_ADDR(reg1_addr << 8);
  CDMA_MDSP_SET_RFT_REG2_ADDR(reg2_addr << 8);

  #ifdef RF_SUPPORTS_32_BIT_TX_AGC_INTERFACE  
  CDMA_MDSP_SET_RFT_REG3_ADDR(reg3_addr << 8);
  CDMA_MDSP_SET_RFT_REG4_ADDR(reg4_addr << 8);
  #endif
}


#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */
#endif /* RF_HAS_MDSP_TX_AGC */


/*===========================================================================

FUNCTION RF_INIT_RF_GPIOS                                   INTERNAL FUNCTION

DESCRIPTION
  Initializes all GPIOS required by the RF Driver

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void rf_init_rf_gpios( void )
{
  /* -----------------------------------------------------
  ** Enable Synthesizer related Tri-state GPIOs for output
  ** and ADC_MUX0,1,2 control bits
  ** ----------------------------------------------------- */

#if !defined ( T_QUASAR_X ) && !defined ( RF_HAS_NO_GPIO_EMULATION )

#ifndef RF_HAS_DEDICATED_TX_SBI

  /* configure the GPIO's for the RFT */
  /* clock */
  rflib_serial_device_gpio_config( RF_PATH_0, GPIO_CLOCK_SIG,
                                   BIO_AUX_SBI_CLOCK_REG, BIO_SYNTH_CLOCK_M);

  /* data */
  rflib_serial_device_gpio_config( RF_PATH_0, GPIO_DATA_SIG,
                                   BIO_AUX_DATA_SBI_REG, BIO_SYNTH_DATA_M);

  /* latch */
  rflib_serial_device_gpio_config( RF_PATH_0, GPIO_LATCH_SIG,
                                   BIO_AUX_LATCH_SBI_REG, BIO_SYNTH_LATCH_M);
#endif /* RF_HAS_DEDICATED_TX_SBI */

#ifndef RF_HAS_NO_EXT_SYNTH
#ifndef RF_HAS_CUSTOMER_DEFINED_SYNTH
  if ( rf_path_1.synthdev == LMX2433 )
  {
    /* configure the GPIO's for the external PLL on chain 1 */

    /* clock */
    rflib_serial_device_gpio_config( RF_PATH_1, GPIO_CLOCK_SIG,
                                     BIO_EXT_SYNTH_CLOCK_REG, BIO_EXT_SYNTH_CLOCK_M);

    /* data */
    rflib_serial_device_gpio_config( RF_PATH_1, GPIO_DATA_SIG,
                                     BIO_EXT_SYNTH_DATA_REG, BIO_EXT_SYNTH_DATA_M);

    /* latch */
    rflib_serial_device_gpio_config( RF_PATH_1, GPIO_LATCH_SIG,
                                     BIO_EXT_SYNTH_LATCH_REG, BIO_EXT_SYNTH_LATCH_M);
  }
#else
  /* initialize customer defined external PLL */
  cust_synth_gpio_init();
#endif /* RF_HAS_CUSTOMER_DEFINED_SYNTH */
#endif /* RF_HAS_NO_EXT_SYNTH */

#endif /* T_QUASAR_X && RF_HAS_NO_GPIO_EMULATION */

#ifdef RF_HAS_Y1Y2_DBG_BUS
#error code not present
#endif
}

/*===========================================================================

FUNCTION RF_INIT_PA_ON_AND_TX_ON_CTLS                        INTERNAL FUNCTION

DESCRIPTION
  This function initializes the PA_ON and TX_ON controls:
    Turn off PAs (PA_ON & PA_ON2)
    Reset all bypass controls for the PA_ON and TX_ON circuits

  Two configurations are supported:
    MSMs that have use DUAL_PA_ON and PA_ON_WRITE to control the PA circuit
    MSMs that have a single control (PA_ON_EN) for the PA circuit

  This function does not modify the state of the TX_ON (idle_n) pin

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rf_init_pa_on_and_tx_on_ctls(void)
{
  /* Force PA-ON and PA_ON2 inactive */
  MSM_W_MASKED(RXF_MODEM(PA_ON_CTL), PA_ON_EN, CAGC_RF_PA_ON_ENA_OFF_V);

  /* Set bypass bits to non-bypass state
  ** Note that if idle_n is manually controlled the TX_ON circuit is disabled so
  ** the TX_ON bypass bits are not used but we set them to the non-bypass state
  ** anyway.
  */
    MSM_W_MASKED(RXF_MODEM(PA_ON_CTL), BYPASS_TX_SYNTH_LOCK,     CAGC_RF_BYPASS_TX_SYNTH_LOCK_VAL);

#ifdef RF_MSM7600_HWIO
#error code not present
#else  /* RF_MSM7600_HWIO */
  MSM_W_MASKED(RXF_MODEM(PA_ON_CTL), BYPASS_TX_PUNCT,          CAGC_RF_NON_BYPASS_TX_PUNCT_VAL);
  MSM_W_MASKED(RXF_MODEM(PA_ON_CTL), BYPASS_PA_PUNCT_RAW,      CAGC_RF_NON_BYPASS_PA_PUNCT_VAL);
#ifdef RF_HAS_SYNTH_LOCK
    MSM_W_MASKED(RXF_MODEM(PA_ON_CTL), BYPASS_PA_SYNTH_LOCK_RAW, CAGC_RF_NON_BYPASS_PA_SYNTH_LOCK_VAL);
#else
    /* Bypass Synth Lock.*/
    MSM_W_MASKED(RXF_MODEM(PA_ON_CTL), BYPASS_PA_SYNTH_LOCK_RAW, CAGC_RF_BYPASS_PA_SYNTH_LOCK_VAL);
    MSM_W_MASKED(RXF_MODEM(MODEM_MISC_CFG0),FORCE_SYNTH_LOCK,1);
#endif
#endif  /* RF_MSM7600_HWIO */

#ifndef RF_BYPASS_SYSTEM_CTL
   /* selects the TX_ON_ALT_SEL so that the TX_ON pin is used for TX in 1X,
      rather than GRFC14 for GSM */
   MSM_W_MASKED(WEB_MISC2_WR, TX_ON_ALT_SEL, 0);
#ifdef RF_HAS_GSM1X
#error code not present
#endif
#endif /* RF_BYPASS_SYSTEM_CTL */
}



/*===========================================================================

FUNCTION RF_ENABLE_RX_CHIP_POWER                        EXTERNALIZED FUNCTION

DESCRIPTION
  Enables the RX power for the RX chip, and calls rfr_init() if appropriate.
  
  This is a helper function to manage the correct initialization sequence for
  RX chips that can support single-chip diversity.  This function insures
  that rfr_init() is called only once per RFR power-up.  Improper sequencing
  can cause multiple reset race conditions when path 0 and path 1 wakeups
  collide.

  If the RF driver is using an RFR that does not support multiple paths this
  this function will simply enable the voltage regulator and call rfr_init().

DEPENDENCIES
  rf_path_0.rfrdev and rf_path_1.rfrdev must be initialized to the correct
  RFR type

RETURN VALUE
  None

SIDE EFFECTS
  The RX VREG will be enabled if it is currently disabled.
  
  If this is the first time power has been enabled for the RFR, rfr_init()
  will be called and will likely reset the RFR to its default state.

===========================================================================*/
void rf_enable_rx_chip_power( rf_path_enum_type path,
                              rfc_vregs_id_type vreg )
{
  pm_err_flag_type pm_level_error = PM_ERR_FLAG__SUCCESS; 
  
#ifdef RF_HAS_RXADC_VOLTAGE_SWITCH

    pm_level_error |= pm_vreg_set_level(PM_VREG_S2_ID, RF_VREG_S2_START_VOLTAGE );
  
    if(pm_level_error != PM_ERR_FLAG__SUCCESS)
    {
        MSG_ERROR("Unsuccessful in setting the RF_VREG_S2_START_VOLTAGE", 0, 0, 0);
    }
    
    rf_set_periodic_wait_timer( RF_VREG_S2_START_SETTLE_TIME );  /* Wait for Vreg to settle */
  
    rf_warmup_time_rxadc_start = timetick_get(); /*Capture the timer tick*/
#endif

  HWIO_OUT(RXA2D_EN, 0x3f);

  if (vreg == RFC_RX_VREGS)
  {
    rfr_enable_sleep_on_off_sequence(path,FALSE);

    rf_qsc11x0_sleep_enable_flag = FALSE;
  }

  rfc_manage_vregs( vreg, PM_ON_CMD, path );

  #ifdef RF_HAS_VARIABLE_SMPS_SWITCHER_SIZE

  pm_level_error = PM_ERR_FLAG__SUCCESS;

  pm_level_error |= pm_vreg_smps_pulse_skipping_enable(PM_VREG_SMPS_S1_ID,FALSE);

  pm_level_error |= pm_vreg_smps_pulse_skipping_enable(PM_VREG_SMPS_S2_ID,FALSE);

  if(pm_level_error != PM_ERR_FLAG__SUCCESS)
  {
    MSG_ERROR("Pmic Pulse Skipping is not disabled properly on Wakeup",0,0,0);
  }
  #endif

  /* This critical section will access and update global state, so it must
     be atomic.  Lock interrupts to prevent any interlopers.  The first
     path into this section is guarenteed to issue the one and only
     reset, and will do so before the other path has a chance to continue
     and possibly use an un-initialized chip. */
  INTLOCK();
  if ( path == RF_PATH_0 )
  {
    switch ( rf_path_0.rfrdev )
    {
      /* RFR6500/RFR6525 only init if both paths are "off" */
    case RFR6500:
    case RFR6525:
      if ( rf_single_chip_multipath_power_state[RF_PATH_0] == FALSE &&
           rf_single_chip_multipath_power_state[RF_PATH_1] == FALSE )
      {
        rfr_init( path );
      }

      rf_single_chip_multipath_power_state[path] = TRUE;
      break;

      /* All other chips always init */
    default:
      rfr_init( path );
      break;
    } /* switch ( rf_path_0.rfrdev ) */
  } /* if ( path == RF_PATH_0 ) */
  else if ( path == RF_PATH_1 )
  {
    switch ( rf_path_1.rfrdev )
    {
      /* RFR6500/RFR6525 only init if both paths are "off" */
    case RFR6500_DRX:
    case RFR6525_DRX:
      if ( rf_single_chip_multipath_power_state[RF_PATH_0] == FALSE &&
           rf_single_chip_multipath_power_state[RF_PATH_1] == FALSE )
      {
        rfr_init( path );
      }

      rf_single_chip_multipath_power_state[path] = TRUE;
      break;

      /* All other chips always init */
    default:
      rfr_init( path );
      break;
    } /* switch ( rf_path_1.rfrdev ) */
  } /* else if ( path == RF_PATH_1 ) */
  else
  {
    RF_FATAL_ERROR( "Invalid path parameter in rf_enable_rx_chip_power" );
  }
  INTFREE();

} /* rf_enable_rx_chip_power */



/*===========================================================================

FUNCTION RF_DISABLE_RX_CHIP_POWER                       EXTERNALIZED FUNCTION

DESCRIPTION
  Disables the RX power for the RX chip.
  
  This is a helper function to manage the correct initialization sequence for
  RX chips that can support single-chip diversity.  Along with
  rf_enable_rx_chip_power() this function insures that rfr_init() is called
  only once per RFR power-up.  Improper sequencing can cause multiple reset
  race conditions when path 0 and path 1 wakeups collide.  This function
  updates the RFR power-on state variable to insure rfr_init() is called
  for the next power-on if the chip is now off.

DEPENDENCIES
  rf_path_0.rfrdev and rf_path_1.rfrdev must be initialized to the correct
  RFR type

RETURN VALUE
  None

SIDE EFFECTS
  The RX VREG will be disabled if this is the last user of the VREG.

===========================================================================*/
void rf_disable_rx_chip_power( rf_path_enum_type path,
                               rfc_vregs_id_type vreg )
{
  #ifdef RF_HAS_VARIABLE_SMPS_SWITCHER_SIZE
  pm_err_flag_type pm_level_error = PM_ERR_FLAG__SUCCESS;
  #endif

  HWIO_OUT(RXA2D_EN, 0x3e);

  #ifdef RF_HAS_MDSP_TX_AGC
  CDMA_MDSP_SET_PA_R0_R1_OVERRIDE_VAL(CAGC_RF_PAR_00_V);
  CDMA_MDSP_ENABLE_PA_RANGE_OVERRIDE();
  #endif

  /* Changing the PA control to HW */
  MSM_W_MASKED( MOD_PA_ON_CTL, PA_RANGE_SEL, 0);
  
  /* Set the PA in Override mode and set the PA R-map to value of 0 */
  MSM_W_MASKED( TX_AGC_CTL, PA_R_OVERRIDE_N, CAGC_RF_PAR_CTL_OVERRIDE_AGC_V);
  MSM_W_MASKED( TX_AGC_CTL, PA_R_WR, CAGC_RF_PAR_10_V ); 

  
  if (vreg == RFC_RX_VREGS)
  {
     rfr_enable_sleep_on_off_sequence(path,TRUE);
     rf_qsc11x0_sleep_enable_flag = TRUE;
  }

  #ifdef RF_HAS_VARIABLE_SMPS_SWITCHER_SIZE
  pm_level_error |= pm_vreg_smps_pulse_skipping_enable(PM_VREG_SMPS_S1_ID,TRUE);

  pm_level_error |= pm_vreg_smps_pulse_skipping_enable(PM_VREG_SMPS_S2_ID,TRUE);

  if(pm_level_error != PM_ERR_FLAG__SUCCESS)
  {
    MSG_ERROR("Pmic Pulse Skipping is not enabled properly when going to Sleep",0,0,0);
  }
  #endif

 
  if ( path == RF_PATH_0 )
  {
    switch ( rf_path_0.rfrdev )
    {
    case RFR6500:
    case RFR6525:
      rf_single_chip_multipath_power_state[path] = FALSE;
      break;

    default:
      /* non-multi-path chips do not use the
         rf_single_chip_multipath_power_state state variable */
      break;
    } /* switch ( rf_path_0.rfrdev ) */
  } /* if ( path == RF_PATH_0 ) */
  else if ( path == RF_PATH_1 )
  {
    switch ( rf_path_1.rfrdev )
    {
    case RFR6500_DRX:
    case RFR6525_DRX:
      rf_single_chip_multipath_power_state[path] = FALSE;
      break;

    default:
      /* non-multi-path chips do not use the
         rf_single_chip_multipath_power_state state variable */
      break;
    } /* switch ( rf_path_1.rfrdev ) */
  } /* else if ( path == RF_PATH_1 ) */
  else
  {
    RF_FATAL_ERROR( "Invalid path parameter in rf_enable_rx_chip_power" );
  }

  /* The VREG disable must be done after the global state variable is updated
     to insure there is no race condition that could result in the RX chip
     not being initialized.  With this order this function is thread safe
     without using INTLOCK. */
  rfc_manage_vregs( vreg, PM_OFF_CMD, path );
} /* rf_disable_rx_chip_power */


/*===========================================================================

FUNCTION RF_INIT                                        EXTERNALIZED FUNCTION

DESCRIPTION
  Initializes the RF unit. This is the first half of RF initialization.  
  The second half is done in rf_init_delayed() which is called after the
  NV items have been loaded.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Leaves the driver in RF_STATE_SLEEP_2.
  Database item DB_RF_MODE set to DB_RF_SLEEP

===========================================================================*/
void rf_init_cdma1x( rfcom_device_enum_type device )
{
  db_items_value_type db_value;  /* temp for database item DB_RF_MODE */
  rf_path_enum_type path = rfcom_to_path[ device ];

  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];
  rf_card_band_type def_band;
  
  MSG_LOW("Entering rf_init()", 0, 0, 0);

  if (path >= RF_PATH_MAX)
  {
     MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
     rf_error_flag = TRUE;
     return;
  }

  #ifndef RF_HAS_HW_QSC11X0
  if (rfnv_validate_1x_rf_card() == FALSE)
  {
    return;
  }
  #endif
  
#ifdef RF_HAS_QSC60X5
#error code not present
#endif /* RF_HAS_QSC60X5 */

#ifdef RF_HAS_QSC11X0
/* Initialize the TxDAC test register to enable routing to TxDAC */
  HWIO_OUTM(TXDAC_TEST_CONTROL, 0xFFFFFFFF, 0);
#endif /* RF_HAS_QSC11X0 */

#if defined(T_QUASAR_X) || defined(RF_HAS_DISABLE_SLEEP_EMULATION)
#error code not present
#endif

/* Select TCXO from RF Card instead local to RUMI_7500 */

#ifdef T_RUMI_SC2X
#error code not present
#endif /* T_RUMI_SC2X */

#ifdef RF_HAS_GSM1X
#error code not present
#endif /* RF_HAS_GSM1X */
  
#ifdef RF_TIME_PROFILING
#error code not present
#endif


  /* ----------------------------------------------------------------
  ** Initialize pointers to structures containing RF NV items.
  ** ---------------------------------------------------------------- */
  
  /* Get all NV data */
  rfnv_get_nv_data( &rf_nv_data );
  
  /* Initialize the nv table pointers array */
  init_nv_ptrs();

  /* Determine the RF Card type in the system */
  rf_card_in_system = rf_get_supported_hw();

  if(!(rf_card_in_system > RF_HW_UNDEFINED && rf_card_in_system < RF_HW_MAX))
  {
      /* Choose the default value since rf_init_delayed() will force a QCN load
         by returning RF_INIT_INVALID_RFCAL status. ZRF6500 is chosen here arbitrarily 
      */
      rf_card_in_system = RF_DEF_HW;
      ERR( "Illegal RF Card specified. default to ZRF6500",0,0,0);
  }

  /* ----------------------------------------------------------------
  ** Configure the ZIF library devices according to the RF card being
  ** used
  ** ---------------------------------------------------------------- */
  rfc_set_card(rf_card_in_system);

  /* Update chain status to reflect hw id */
  rf_chain_ptr->rf_curr_hw = rf_card_in_system;

  /* Setup the pointers to the associated NV tables */
  rf_nv_fm_items_ptr  = rfnv_fm_table();
  rf_nv_common_items_ptr = rfnv_common_table();
  
  /* Initialize the common pa_r_map value */
  rf_current_pa_r_map = rf_nv_common_items_ptr->rfnv_pa_r_map;
  
  /* Set the default band for the card based on band mask */
  if((def_band = rf_get_first_bc_in_mask())!= RF_MAX_BAND)
  {
      /* Translate the band class coming into the Rf driver to a 
         different band. This function is only active during testing. */
      /* If no translation is performed then the return value is equal to
         def_band. */
      rf_card.rfc_default_band = rf_translate_band_class(RF_TRANSLATE_BC_IN, 
                                                            def_band);
  }      

  rf_path_0.gps_mode = GPS_MODE_SGPS;
  rf_path_1.gps_mode = GPS_MODE_SGPS;

  switch ( device )
  {
  case RFCOM_TRANSCEIVER_0:

    MSG_LOW("Entering rf_init()", 0, 0, 0);

#if !defined(T_QUASAR) && !defined(RF_HAS_NO_GPIO_EMULATION)
#ifdef RF_HAS_SYNTH_LOCK
    /* Configure GPIO for SYNTH_LOCK signal */
    gpio_tlmm_config(SYNTH_LOCK);
#endif /* RF_HAS_SYNTH_LOCK */
#endif /* !T_QUASAR && RF_HAS_NO_GPIO_EMULATION */

  
    rf_chain_ptr->rf_curr_cdma_band = rf_card.rfc_default_band;

    if (rf_chain_ptr->rf_curr_cdma_band >= RF_MAX_BAND) 
    {
        MSG_FATAL("Band Class invalid or out of bounds", 0, 0, 0);
        rf_error_flag = TRUE;
        return;
    }

    /* Setup the pointers to the associated NV tables */
    #ifdef RF_HAS_SUB_BLOCK_SUPPORT
    rf_chain_ptr->rf_nv_dig_items_ptr = 
        rf_digital_nv_table[path][rf_chain_ptr->rf_curr_cdma_band];
    #else
    rf_chain_ptr->rf_nv_dig_items_ptr = 
        rf_digital_nv_table[rf_chain_ptr->rf_curr_cdma_band];
    #endif

    /* ----------------------------------------------------------------
    ** Configure Band class flags for devices in ZIF library (1X)
    ** ---------------------------------------------------------------- */


#if defined( RF_TEST_NO_PATH_1_DEVICES )
#error code not present
#elif defined( RF_TEST_NO_PATH_1_SYNTH )
    rf_path_1.synthdev = NO_SYNTH;
#endif


#ifndef RF_HAS_NO_EXT_SYNTH
#ifndef RF_HAS_CUSTOMER_DEFINED_SYNTH
    if ( rf_path_1.synthdev == LMX2433 ) 
    {
    /* Support external RX1 PLL control lines */
#ifndef T_IO_CARD_PANDA
       gpio_tlmm_config(SYNTH_LATCH);
#endif
       gpio_tlmm_config(SYNTH_CLK);
       gpio_tlmm_config(SYNTH_DATA);
    }
#endif /* RF_HAS_CUSTOMER_DEFINED_SYNTH */
#endif /* RF_HAS_NO_EXT_SYNTH */

    /* Configure the ZIF library devices on RF path 0 according to choices */
    if (!rflib_set_path_0_devices( &rf_path_0 ) )
    {
      ERR_FATAL( "Error configuring RF devices",0,0,0);
    }
    #ifdef ZIF_LIBRARY_SUPPORTS_2_PATHS
    /* Configure the ZIF library devices on RF path 1 according to choices */
    if (!rflib_set_path_1_devices( &rf_path_1 ) )
    {
      ERR_FATAL( "Error configuring RF devices",0,0,0);
    }
    #endif

    #ifdef RF_HAS_MDSP_TX_AGC 
    /* Pass the Information on SSBI Port as well
      as the SSBI registers in RFT at which the even
      and odd PDM Values are written*/  
    rf_mdsp_tx_set_port_reg_address();  
    #endif

#if defined( RF_HAS_MONOTONIC_LNA_GAIN_LINEUP )
    /* set RFR6125 LNA gain lineup to monotonic */
    if (rf_path_0.rfrdev == RFR6125 )
      rflib_set_gain_line_up( path, RFR6125_LINE_UP_1 );
#endif /* RF_HAS_MONOTONIC_LNA_GAIN_LINEUP */


#ifdef FEATURE_PMIC_MANAGED_LDO
#ifdef FEATURE_RF_PA_DVS
    rf_config_pa_dvs();
#endif /* FEATURE_RF_PA_DVS */

    /* Manage the Voltage Regulator changes */
    rf_enable_rx_chip_power( path, RFC_RX_VREGS );

    /* ----------------------------------------------------------------
    ** Turn off TX LDOs
    ** ---------------------------------------------------------------- */
    rfc_manage_vregs(RFC_TX_VREGS, PM_OFF_CMD, path );

#endif /* FEATURE_PMIC_MANAGED_LDO */

#if defined(RF_HAS_QSC60X5)
#error code not present
#elif defined(RF_HAS_QSC11X0)
    /*  ----------------------------------------------------------------
    ** Explicitly disable PDM1, PDM2, PDM0 needs to be on for LCD
    ** ---------------------------------------------------------------- */
    HWIO_OUTM(TCXO_PDM_CTL, 0x36, 0x00);
#else
    /*  ----------------------------------------------------------------
    ** Explicitly disable PDM0, PDM1, PDM2
    ** ---------------------------------------------------------------- */
    HWIO_OUTM(TCXO_PDM_CTL, 0xff, 0x00);
#endif /* RF_HAS_QSC60X5 */

#ifndef RF_HAS_HDET_BYPASS
    /* ---------------------
    ** Initialize call-backs
    ** --------------------- */
    if ( !rf_pwr_call_back_initialized )
    {
       clk_def( &rf_pwr_call_back );
       rf_pwr_call_back_initialized = TRUE;
    }
#endif /* RF_HAS_HDET_BYPASS */

    /* --------------------------------
    ** Initialize all RF related GPIOS
    ** -------------------------------- */

#if !defined(T_QUASAR) || defined(T_QUASAR_X)
    rf_init_rf_gpios();
#endif

    
    /* get rtr revision for future referance and update the variable*/
    if(rf_path_0.rftdev == RTR6300)
        //rtr_vco_external = 
        (void)rft_is_vco_external();

    /* enable RXFRONT0 clock - required for rfc_set_mode */
#ifndef RF_HAS_RXF_V2
#error code not present
#else  /* RF_HAS_RXF_V2 */
    rxf_enable_rx_front_clocks_sc2x(RF_PATH_0);
#endif /* !RF_HAS_RXF_V2 */
    /* ---------------------------------------------
    ** Set rf card to the default mode.
    ** --------------------------------------------- */
    rfc_set_mode(path,rf_card.rfc_default_band);

    /* Park the VCOs - disable VCOs */
    rfc_set_mode_park(path);

    /* ----------------------------------------------------
    ** Turn on CAGC dependent CDMA Rx clock regime.
    ** ---------------------------------------------------- */
    clk_regime_enable( CLK_RGM_TXCHIPX8_M );

    /* ----------------------------------------------------
    ** Initialize all "one time" CAGC hardware setup bits
    ** for Rx and TX CAGC
    ** ---------------------------------------------------- */
    rf_init_cagc(device);

    /* ----------------------------------------------------------------
    ** Initialize PA_ON, TX_ON controls and PA offsets
    ** ---------------------------------------------------------------- */
    rf_init_pa_on_and_tx_on_ctls();

    /* Initialize Tx AGC block */
    TX_AGC_RESET();

    rf_set_pa_offsets( RFCOM_TRANSCEIVER_0 );

    #ifdef RF_HAS_MDSP_TX_AGC
    /* Indicates to MDSP that Linearizer Parameters
       are latched to be picked up by MDSP at slot
       boundary*/
    CDMA_MDSP_NEW_TX_LIN_SLOT_PARAMS_AVAILABLE();    
    #endif

    /* -----------------------------------
    ** Initialize RFT/RFR to default mode.
    ** ----------------------------------- */
    rft_init( path );
    if ( !rft_is_valid(path) )
    {
      ERR("Invalid RFT%d", path,0,0);
    }

    /* rfr_init() will be called by rfc_manage_vregs() when appropriate, so
       rfr_is_valid() can be called now.
       Except when PMIC_MANAGED_LDOs is not enabled, and rfr_init() never gets
       called, so do it explicitly here. */
#ifndef FEATURE_PMIC_MANAGED_LDO
    rfr_init( path );
#endif /* !FEATURE_PMIC_MANAGED_LDO */
    if ( !rfr_is_valid(path) )
    {
      ERR("Invalid RFR%d", path,0,0);
    }

    /* -----------------------------------
    ** Initialize SYNTH to default mode.
    ** ----------------------------------- */
    synth_init( path );

    /* ------------------------------------------------------------------------
    ** Initialize database item DB_RF_MODE & rf_state so that rf_init_digital()
    ** initializes the CDMA hardware and loads the synthesizers.
    ** --------------------------------------------------------------------- */
    rf_chain_ptr->rf_mode = DB_RF_SLEEP;
    db_value.rf_mode = rf_chain_ptr->rf_mode;

    db_put( DB_RF_MODE, &db_value );
    rf_chain_ptr->rf_prev_state = rf_chain_ptr->rf_state;
    rf_chain_ptr->rf_state = RF_STATE_SLEEP_2;

#ifdef FEATURE_ZIF_LOGGING
    /* ------------------------------------------------------------------------
    ** Initialize ZIF Logging
    ** --------------------------------------------------------------------- */
    rf_init_zif_agc_logs();
    rf_init_zif_iq_logs();
#endif

    break;

  case RFCOM_RECEIVER_1:
    if((rf_card_in_system == RF_HW_QSC11X0_DUAL_BAND) || (rf_card_in_system == RF_HW_QSC11X0_TRI_BAND)
       || (rf_card_in_system == RF_HW_QSC11X0_TRI_BAND_BC5_TRI_STATE_PA))
    {
        return;
    }

    if ( rf_path_1.rfrdev == NO_RFR )
    {
      /* If the second receiver has no RFR, skip initialization */
      break;
    }

    /* initialize nv items pointer */
    rf_chain_ptr->rf_curr_cdma_band = rf_card.rfc_default_band;

    if (rf_chain_ptr->rf_curr_cdma_band >= RF_MAX_BAND) 
    {
        MSG_FATAL("Band Class invalid or out of bounds", 0, 0, 0);
        rf_error_flag = TRUE;
        return;
    }

    #ifdef RF_HAS_SUB_BLOCK_SUPPORT
    rf_chain_ptr->rf_nv_dig_items_ptr = 
        rf_digital_nv_table[path][rf_chain_ptr->rf_curr_cdma_band];
    #else
    rf_chain_ptr->rf_nv_dig_items_ptr = 
          rf_digital_nv_table[rf_chain_ptr->rf_curr_cdma_band];
    #endif

    /* enable RXFRONT0 clock - required for rfc_set_mode */
#ifndef RF_HAS_RXF_V2
#error code not present
#else /* RF_HAS_RXF_V2 */
    rxf_enable_rx_front_clocks_sc2x(RF_PATH_1);
#endif /* !RF_HAS_RXF_V2 */

    /* ---------------------------------------------
    ** Set rf card to the default mode.
    ** --------------------------------------------- */
    rfc_set_mode(path,rf_card.rfc_default_band);

    /* Park the VCOs - disable VCOs */
    rfc_set_mode_park(path);

#if defined( RF_HAS_MONOTONIC_LNA_GAIN_LINEUP )
    /* set RFR6125 LNA gain lineup to monotonic */
    if( rf_path_1.rfrdev == RFR6125)
        rflib_set_gain_line_up( path, RFR6125_LINE_UP_1 );
#endif /* RF_HAS_MONOTONIC_LNA_GAIN_LINEUP */

    /* ----------------------------------------------------
    ** Initialize all "one time" CAGC hardware setup bits
    ** ---------------------------------------------------- */
    rf_init_cagc(device);


    /* ------------------------------------------------------------------------
    ** Initialize rf_mode & rf_state
    ** --------------------------------------------------------------------- */
    rf_chain_ptr->rf_mode = DB_RF_SLEEP;

    rf_chain_ptr->rf_prev_state = rf_chain_ptr->rf_state;
    rf_chain_ptr->rf_state = RF_STATE_SLEEP_2;

    /* Manage the Voltage Regulator changes */
    rf_enable_rx_chip_power( path, RFC_RX_VREGS );

    /* -----------------------------------
    ** Initialize RFR to default mode.
    ** ----------------------------------- */
    /* rfr_init() will be called by rfc_manage_vregs() when appropriate, so
       rfr_is_valid() can be called now */
    if ( !rfr_is_valid(path) )
    {
      ERR("Invalid RFR%d", path,0,0);
    }

    /* -----------------------------------
    ** Initialize SYNTH to default mode.
    ** ----------------------------------- */
    synth_init( path );

    break;

  default:
      ERR_FATAL(" rf_init(): device %d not supported!!!", device, 0, 0);
  }

  #ifdef RF_HAS_POWER_MODE_LIB
  /* One-time initialization of the IntelliCeiver module */
  if ( device == RFCOM_TRANSCEIVER_0 &&
       (rf_path_0.rfrdev == RFR6500 ||
        rf_path_0.rfrdev == RFR6525 ||
        rf_path_0.rfrdev == QSC60X5_RX ||
        rf_path_0.rfrdev == QSC11X0_RX ||
        rf_path_0.rfrdev == RTR6500_RX) )
  {
    /*#ifdef RF_HAS_SUPPORT_FOR_QSC11X0_BRINGUP
    rf_intelliceiver_force_low_power_mode(rfcom_to_path[device],TRUE);
    #endif*/

    #ifndef RF_HAS_SUPPORT_FOR_RICO_BRINGUP
    rf_intelliceiver_init( rfcom_to_path[device] );
    #endif

  }
  #endif /* RF_HAS_POWER_MODE_LIB */

#ifdef T_RUMI_SC2X
#error code not present
#endif /* T_RUMI_SC2X */

#if defined (RF_HAS_MDSP_TX_AGC) || defined(RF_HAS_PDM_VS_POWER_TX_CAL)
rf_cdma_load_master_lin_tables = TRUE;
#endif

#ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO
if (rf_adc_clock_crit_sect_init == TRUE)
{
  //rex_init_crit_sect(&rf_adc_clock_crit_sect);
  rf_adc_clock_crit_sect_init = FALSE;
} 
#endif /* RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO */
}  /* rf_init */


/*===========================================================================

FUNCTION RF_INIT_RF_NV_ITEMS                                INTERNAL FUNCTION

DESCRIPTION
  This function initializes the RF related NV items.  It is intended to be
  called immediately after the RF NV data has been read.

DEPENDENCIES
  rf NV data has been read from NV
  rf_nv_dig_items_ptr and rf_nv_fm_items_ptr must have been initialized

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
void rf_init_rf_nv_items( void )
{

  /* Initialize one time TX CAGC items that require NV to have
  ** been read
  */
  rf_init_tx_cagc_delayed();

  /* Initialize one time RX CAGC items that require NV to have
  ** been read
  */
  rf_init_rx_cagc_delayed();

#ifdef RF_HAS_RFCONFIG_RX_LNA
  /* Initialize RFConfig Rx LNA switch point database using the just recently
  ** read NV items
  */
  rfc_init_rfconfig_rx_lna();
#endif /* RF_HAS_RFCONFIG_RX_LNA */

#ifdef RF_HAS_PDM_VS_POWER_TX_CAL
  /* MASTER Lin NV items are used to store calculated linearizer tables. 
  If those NV items are reloaded from NV, we nead to calculate those NV items again */
  rfagc_reset_tx_pdm_vs_pwr_cal();

  /* Update linearizer tables based on TX PDM_VS_PWR calibration data
  ** for all the bands supported by the transmitter device. TX chip should be 
  ** powered on at this time and Read/Writes to the chip available.
  **/
  rfagc_pdm_vs_pwr_update_tx_lin_tbl_for_all_bands();
#endif
}

#ifdef RF_HAS_Y1Y2_DBG_BUS
#error code not present
#endif /* RF_HAS_Y1Y2_DBG_BUS */


/*===========================================================================

FUNCTION RF_SET_BAND_INITIALIZATIONS

DESCRIPTION
  This function initializes some global variables in the RF driver
  according to conpile time band selection flags

DEPENDENCIES
  curr_cdma_band must have been set up

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void rf_set_band_initializations( rfcom_device_enum_type device  )
{

  rf_path_enum_type path = rfcom_to_path[ device ];
  /* ----------------------------------------------
  ** Initializations dependent upon band
  ** ---------------------------------------------- */
  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ RF_PATH_0 ];

  rflib_min_max_chan_type chan_min_max;

  /*   Initialize to the band already set in the rf_chain  */
#ifdef RF_USE_EXT_VCO_FOR_CDMA
  /*  Not all possible BC5 channels may be supported when external VCO
  **  is used with RFR6175.
  */
  if ( rf_path_0.rfrdev == RFR6175 )
  {
    extern boolean fzrf_get_band_min_max_chans( rf_path_enum_type, 
                                                rf_card_band_type, 
                                                rflib_min_max_chan_type* );

    if ( !fzrf_get_band_min_max_chans( path, rf_chain_ptr->rf_curr_cdma_band, 
                                 &chan_min_max ) )
    {
       MSG_ERROR("Band enum %d not supported", rf_chain_ptr->rf_curr_cdma_band, 0, 0);
    }

  }
  else
  {
    if ( !rflib_get_band_min_max_chans( path, rf_chain_ptr->rf_curr_cdma_band, &chan_min_max ))
    {
      MSG_ERROR("Band enum %d not supported", rf_chain_ptr->rf_curr_cdma_band, 0, 0);
    }
  }
#else
  if ( !rflib_get_band_min_max_chans( path, rf_chain_ptr->rf_curr_cdma_band, &chan_min_max ))
  {
    MSG_ERROR("Band enum %d not supported", rf_chain_ptr->rf_curr_cdma_band, 0, 0);
  }
#endif

  rf_chain_ptr->rf_chan = chan_min_max.min_chan;


}

#ifdef RF_HAS_SUB_BLOCK_SUPPORT
/*===========================================================================

FUNCTION RF_SAVE_VCO_COARSE_TUNING_RESULTS

DESCRIPTION
  This function saves the RFR VCO coarse tuning results to NV

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void rf_save_block_vco_coarse_tuning_results( rf_card_blocks_type block, rf_path_enum_type path, 
                                        const vco_coarse_tune_params_type *vco_coarse_tune_params_ptr )

{
#if defined(RF_HAS_MULTIMODE_ARCH)
  extern nv_stat_enum_type rfnv_nv_write( nv_items_enum_type, nv_item_type * );
#else
#error code not present
#endif

  int i;
  boolean writeFlag;
  unsigned char *p;

  static const nv_items_enum_type nv_ct_table_enum_val_block[RF_BAND_MAX_BLOCKS][RF_PATHS_TABLE_SIZ]=
  {
   {NV_BCX_BLOCK_VCO_COARSE_TUNE_TABLE_I, NV_C1_BCX_BLOCK_VCO_COARSE_TUNE_TABLE_I},
   {NV_BCX_BLOCK_1_VCO_COARSE_TUNE_TABLE_I, NV_C1_BCX_BLOCK_1_VCO_COARSE_TUNE_TABLE_I}
  };

  static const nv_items_enum_type nv_ct_2_enum_val_block[RF_BAND_MAX_BLOCKS]=
  {
   NV_BCX_BLOCK_VCO_TUNE_2_I,
   NV_BCX_BLOCK_1_VCO_TUNE_2_I
  };
   
  switch( block )
  {
  /* Supported bands */
  case RF_BAND_BLOCK_0:
  case RF_BAND_BLOCK_1:
    break;
  /* Bail out for unsupported bands */
  default:
    return;
  }

  writeFlag = FALSE;
  p = (unsigned char *)vco_coarse_tune_params_ptr->coarse_tune_table;
  for( i = 0; i < NV_VCO_COARSE_TUNE_TABLE_SIZ; i++ )
  {
    if( p[i] )
    {
      writeFlag = TRUE;
      break;
    }
  }

  if( writeFlag )
  {
    /* save generated values to NV for future use */
#if defined(RF_HAS_MULTIMODE_ARCH)
    if (path >= RF_PATH_MAX)
    {
       MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
       rf_error_flag = TRUE;
       return;
    }

    /* write the value to NV */
    (void)rfnv_nv_write( nv_ct_table_enum_val_block[block][path], 
                         (nv_item_type *)vco_coarse_tune_params_ptr->coarse_tune_table ); /*lint !e826 union type is valid */
    
    if (path == RF_PATH_0)
    {
      (void)rfnv_nv_write( nv_ct_2_enum_val_block[block], 
                           (nv_item_type *)vco_coarse_tune_params_ptr->coarse_tune_2 ); /*lint !e826 union type is valid */
    }
#else
#error code not present
#endif
    
  }

}
#endif/*RF_HAS_SUB_BLOCK_SUPPORT*/
/*===========================================================================

FUNCTION RF_SAVE_VCO_COARSE_TUNING_RESULTS

DESCRIPTION
  This function saves the RFR VCO coarse tuning results to NV

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void rf_save_vco_coarse_tuning_results( rf_card_band_type band,
                                        rf_path_enum_type path,
                                        const vco_coarse_tune_params_type *vco_coarse_tune_params_ptr )
{
#if defined(RF_HAS_MULTIMODE_ARCH)
  extern nv_stat_enum_type rfnv_nv_write( nv_items_enum_type, nv_item_type * );
#else
#error code not present
#endif

  int i;
  boolean writeFlag;
  unsigned char *p;

  static const nv_items_enum_type nv_ct_table_enum_val[RF_MAX_BAND][RF_PATHS_TABLE_SIZ]=
  {
   {NV_BC0_VCO_COARSE_TUNE_TABLE_I, NV_C1_BC0_VCO_COARSE_TUNE_TABLE_I},
   {NV_BC1_VCO_COARSE_TUNE_TABLE_I, NV_C1_BC1_VCO_COARSE_TUNE_TABLE_I},
   {NV_BC0_VCO_COARSE_TUNE_TABLE_I, NV_C1_BC0_VCO_COARSE_TUNE_TABLE_I}, /* BC0 as place holder for BC2 */
   {NV_BC3_VCO_COARSE_TUNE_TABLE_I, NV_C1_BC3_VCO_COARSE_TUNE_TABLE_I},
   {NV_BC4_VCO_COARSE_TUNE_TABLE_I, NV_C1_BC4_VCO_COARSE_TUNE_TABLE_I},
   {NV_BC5_VCO_COARSE_TUNE_TABLE_I, NV_C1_BC5_VCO_COARSE_TUNE_TABLE_I},
   {NV_BC6_VCO_COARSE_TUNE_TABLE_I, NV_C1_BC6_VCO_COARSE_TUNE_TABLE_I},
   {NV_BC0_VCO_COARSE_TUNE_TABLE_I, NV_C1_BC0_VCO_COARSE_TUNE_TABLE_I}, /* BC0 as place holder for BC7 */
   {NV_BC0_VCO_COARSE_TUNE_TABLE_I, NV_C1_BC0_VCO_COARSE_TUNE_TABLE_I}, /* BC0 as place holder for BC8 */
   {NV_BC0_VCO_COARSE_TUNE_TABLE_I, NV_C1_BC0_VCO_COARSE_TUNE_TABLE_I}, /* BC0 as place holder for BC9 */
   {NV_BC0_VCO_COARSE_TUNE_TABLE_I, NV_C1_BC0_VCO_COARSE_TUNE_TABLE_I}, /* BC0 as place holder for BC10 */
   {NV_BC11_VCO_COARSE_TUNE_TABLE_I, NV_C1_BC11_VCO_COARSE_TUNE_TABLE_I},
   {NV_BC0_VCO_COARSE_TUNE_TABLE_I, NV_C1_BC0_VCO_COARSE_TUNE_TABLE_I}, /* BC0 as place holder for BC12 */
   {NV_BC0_VCO_COARSE_TUNE_TABLE_I, NV_C1_BC0_VCO_COARSE_TUNE_TABLE_I}, /* BC0 as place holder for BC13 */
   {NV_BC14_VCO_COARSE_TUNE_TABLE_I, NV_C1_BC14_VCO_COARSE_TUNE_TABLE_I},
   {NV_BC15_VCO_COARSE_TUNE_TABLE_I, NV_C1_BC15_VCO_COARSE_TUNE_TABLE_I},
   {NV_BC0_VCO_COARSE_TUNE_TABLE_I, NV_C1_BC0_VCO_COARSE_TUNE_TABLE_I}, /* BC0 as place holder for GPS */
   {NV_BC0_VCO_COARSE_TUNE_TABLE_I, NV_C1_BC0_VCO_COARSE_TUNE_TABLE_I}  /* BC0 as place holder for FM */
  };
  

  static const nv_items_enum_type nv_ct_2_enum_val[RF_MAX_BAND]=
  {
   NV_BC0_VCO_TUNE_2_I,
   NV_BC1_VCO_TUNE_2_I,
   NV_BC0_VCO_TUNE_2_I,  /* BC0 as place holder for BC2 */
   NV_BC3_VCO_TUNE_2_I,
   NV_BC4_VCO_TUNE_2_I,
   NV_BC5_VCO_TUNE_2_I,
   NV_BC6_VCO_TUNE_2_I,
   NV_BC0_VCO_TUNE_2_I,  /* BC0 as place holder for BC7 */
   NV_BC0_VCO_TUNE_2_I,  /* BC0 as place holder for BC8 */
   NV_BC0_VCO_TUNE_2_I,  /* BC0 as place holder for BC9 */
   NV_BC0_VCO_TUNE_2_I,  /* BC0 as place holder for BC10 */
   NV_BC11_VCO_TUNE_2_I,
   NV_BC0_VCO_TUNE_2_I,  /* BC0 as place holder for BC12 */   
   NV_BC0_VCO_TUNE_2_I,  /* BC0 as place holder for BC13 */
   NV_BC14_VCO_TUNE_2_I,
   NV_BC15_VCO_TUNE_2_I,
   NV_BC0_VCO_TUNE_2_I,  /* BC0 as place holder for GPS */
   NV_BC0_VCO_TUNE_2_I   /* BC0 as place holder for FM */
  };

  switch( band )
  {
  /* Supported bands */
  case RF_BC0_BAND:
  case RF_BC1_BAND:
  case RF_BC3_BAND:
  case RF_BC4_BAND:
  case RF_BC5_BAND:
  case RF_BC6_BAND:
  case RF_BC11_BAND:
  case RF_BC14_BAND:
  case RF_BC15_BAND:
  case RF_BC10_BAND:
    break;
  /* Bail out for unsupported bands */
  default:
    return;
  }

  writeFlag = FALSE;
  p = (unsigned char *)vco_coarse_tune_params_ptr->coarse_tune_table;
  for( i = 0; i < NV_VCO_COARSE_TUNE_TABLE_SIZ; i++ )
  {
    if( p[i] )
    {
      writeFlag = TRUE;
      break;
    }
  }

  if( writeFlag )
  {
    /* save generated values to NV for future use */
#if defined(RF_HAS_MULTIMODE_ARCH)
    if (path >= RF_PATH_MAX)
    {
       MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
       rf_error_flag = TRUE;
       return;
    }

    /* write the value to NV */
    (void)rfnv_nv_write( nv_ct_table_enum_val[band][path], 
                         (nv_item_type *)vco_coarse_tune_params_ptr->coarse_tune_table ); /*lint !e826 union type is valid */
    
    if (path == RF_PATH_0)
    {
      (void)rfnv_nv_write( nv_ct_2_enum_val[band], 
                           (nv_item_type *)vco_coarse_tune_params_ptr->coarse_tune_2 ); /*lint !e826 union type is valid */
    }
#else
#error code not present
#endif
    
  }

}

/*===========================================================================

FUNCTION RF_DO_VCO_COARSE_TUNING

DESCRIPTION
  This function performs the RFR VCO coarse tuning

DEPENDENCIES
  NV must have already been read

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void rf_do_vco_coarse_tuning( rfcom_device_enum_type device )
{
  static boolean vco_coarse_tune_done_chain[RF_PATH_MAX] = {FALSE,
                                                            FALSE};

  rf_path_enum_type path     = rfcom_to_path[ device ];
  boolean do_vco_calibration = TRUE;
  int     i                  = 0;
  uint8   BCInx              = 0;

  rf_bc_and_mask_type valid_band;
  
  rf_card_band_type   band_save;
  
  #ifdef RF_HAS_SUB_BLOCK_SUPPORT
  rfnv_digital_items_type* nv_digital_items_ptr = NULL;
  rf_card_band_type block_band = RF_MAX_BAND;
  int block_count = 0;
  #endif

  sys_band_mask_type  band_mask = rfnv_get_band_mask();

  if (path >= RF_PATH_MAX)
  {
      MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
      rf_error_flag = TRUE;
      return;
  }

  /* Only do this once */
  if ( vco_coarse_tune_done_chain[path] == FALSE ) 
  {
      switch ( device )
      {
        case RFCOM_TRANSCEIVER_0:      
            /* for RFR6125 VCO Coarse tune is done in CDMA Cellular */
            rfc_set_mode(path, rf_card.rfc_default_band);
    
            break;
    
        case RFCOM_RECEIVER_1:
    
            if((rf_path_1.rfrdev == RFR6125) ||
               (rf_path_1.rfrdev == RFR6135) ||
               (rf_path_1.rfrdev == RFR6175))
            {
                /* for RFR6125 VCO Coarse tune is done in CDMA Cellular */
                rfc_set_mode(path,rf_card.rfc_default_band);
        
                /* use the RFR's internal VCO on the second chain */
                rfc_select_secondary_vco();
        
                rflib_rfr_use_external_vco(path, FALSE);
            }
            else
            {
                RF_FATAL_ERROR( "VCO Coarse tuning not supported for this RF Card");
            }
    
            break;
    
        default:
            RF_FATAL_ERROR(" rf_do_vco_coarse_tuning(): device not supported!!!");
      } /* end of switch */
    
      /* save default band */
      band_save = rfr_get_curr_band(path);                
          
      /* Loop through for each band defined in the table and is enabled in the 
         NV Band Mask */
      while(rf_get_valid_bc_and_mask(BCInx,&valid_band) == TRUE)
      {
        /* If the band class is equal to all 1s or the band mask is
           equal to 0 then the band class/mask value is invalid. 
           We should had exit the while loop. */
        RF_ASSERT((valid_band.bc   != (rf_card_band_type)(-1)) && \
                  (valid_band.mask != 0));
    
        if ((valid_band.bc == (rf_card_band_type)(-1)) || (valid_band.bc >= RF_MAX_BAND)) 
        {
            MSG_FATAL("band class invalid or out of bounds", 0, 0, 0);
            rf_error_flag = TRUE;
            return;
        }

        rfr_set_curr_band(path, valid_band.bc);
    
        #ifdef RF_HAS_SUB_BLOCK_SUPPORT
        nv_digital_items_ptr = rfnv_get_band_nv_ptr(valid_band.bc);
        #endif
    
        do_vco_calibration = TRUE;
            
        if( 0 != (valid_band.mask & band_mask ))
        {
            for ( i=0; i<NV_VCO_COARSE_TUNE_TABLE_SIZ; i++ )
            {
                /* if any values are not 0 assume NV has valid values */
                #ifdef RF_HAS_SUB_BLOCK_SUPPORT
                if(nv_digital_items_ptr->rfnv_vco_coarse_tune_table[path][i] != 0)
                #else
                if ( rf_digital_nv_table[ valid_band.bc ]->rfnv_vco_coarse_tune_table[path][i] != 0 )
                #endif
                {
                   vco_coarse_tune_params_type vco_coarse_tune_params;
                   
                   do_vco_calibration = FALSE;
                   
                   /* load the VCO coarse tune table from NV */
                   #ifdef RF_HAS_SUB_BLOCK_SUPPORT
                   vco_coarse_tune_params.coarse_tune_table = nv_digital_items_ptr->rfnv_vco_coarse_tune_table[path];
                   vco_coarse_tune_params.coarse_tune_2 =     nv_digital_items_ptr->rfnv_vco_coarse_tune_2[path];                   
                   #else
                   vco_coarse_tune_params.coarse_tune_table = rf_digital_nv_table[valid_band.bc]->rfnv_vco_coarse_tune_table[path];
                   vco_coarse_tune_params.coarse_tune_2 = rf_digital_nv_table[valid_band.bc]->rfnv_vco_coarse_tune_2[path];
                   #endif
                   
                   #ifdef RF_HAS_POWER_MODE_LIB
                     vco_coarse_tune_params.table_length = NV_RF_POWER_MODE_SIZ * NV_VCO_COARSE_TUNE_TABLE_SIZ;
                   #else
                     vco_coarse_tune_params.table_length = NV_VCO_COARSE_TUNE_TABLE_SIZ;
                   #endif /* RF_HAS_POWER_MODE_LIB */
    
                   rfr_load_vco_coarse_tune_table( path, &vco_coarse_tune_params );
                   break;
                }
            }
        
            if ( do_vco_calibration )
            {
                vco_coarse_tune_params_type vco_coarse_tune_params;
      
                /* Use the space that is pre-allocated in NV for passing the
                   tables around. */
                #ifdef RF_HAS_SUB_BLOCK_SUPPORT
                vco_coarse_tune_params.coarse_tune_table = nv_digital_items_ptr->rfnv_vco_coarse_tune_table[path];
                vco_coarse_tune_params.coarse_tune_2 =     nv_digital_items_ptr->rfnv_vco_coarse_tune_2[path];                   
                #else
                vco_coarse_tune_params.coarse_tune_table = rf_digital_nv_table[valid_band.bc]->rfnv_vco_coarse_tune_table[path];
                vco_coarse_tune_params.coarse_tune_2 = rf_digital_nv_table[valid_band.bc]->rfnv_vco_coarse_tune_2[path];
                #endif
    
                #ifdef RF_HAS_POWER_MODE_LIB
                  vco_coarse_tune_params.table_length = NV_RF_POWER_MODE_SIZ * NV_VCO_COARSE_TUNE_TABLE_SIZ;
                #else
                  vco_coarse_tune_params.table_length = NV_VCO_COARSE_TUNE_TABLE_SIZ;
                #endif /* RF_HAS_POWER_MODE_LIB */
                
                /* do the VCO coarse tuning */
                if(rfr_do_vco_coarse_tuning(path, &vco_coarse_tune_params ) == SAVE_COARSE_TUNE_TBL )
                {
                  rf_save_vco_coarse_tuning_results( valid_band.bc, path, &vco_coarse_tune_params);
                }
                /* If not saving the coarse tune values, the RFNV is never
                   going to be touched again anyway, so there is no harm in
                   leaving the unsaved values there. 
                */
    
            } /* if ( do_vco_calibration ) */
        } /* if ( 0 != (valid_band.mask & band_mask ) */
    
        BCInx++;
    
      } /* while loop. */
          
      #ifdef RF_HAS_SUB_BLOCK_SUPPORT
      for(block_count=0; block_count<RF_BAND_MAX_BLOCKS; block_count++)
      {
        nv_digital_items_ptr = rfnv_get_block_nv_ptr(block_count);

        if(nv_digital_items_ptr != NULL)//If Block NV is Valid
        {
          block_band = rfnv_get_parent_band_for_block((rf_card_blocks_type)block_count);

          rfr_set_curr_band(path, block_band);

          /*#ifdef RF_HAS_SUB_BLOCK_SUPPORT
          nv_digital_items_ptr = rfnv_get_band_nv_ptr(valid_band.bc);
          #endif*/
          do_vco_calibration = TRUE;
            /*if( 0 != (valid_band.mask & band_mask ))
            {*/
           for ( i=0; i<NV_VCO_COARSE_TUNE_TABLE_SIZ; i++ )
           {
             /* if any values are not 0 assume NV has valid values */
             if ( nv_digital_items_ptr->rfnv_vco_coarse_tune_table[path][i] != 0 )
             {
               vco_coarse_tune_params_type vco_coarse_tune_params;

                do_vco_calibration = FALSE;

                /* load the VCO coarse tune table from NV */                
                vco_coarse_tune_params.coarse_tune_table = nv_digital_items_ptr->rfnv_vco_coarse_tune_table[path];
                vco_coarse_tune_params.coarse_tune_2 =     nv_digital_items_ptr->rfnv_vco_coarse_tune_2[path];                   

                #ifdef RF_HAS_POWER_MODE_LIB
                vco_coarse_tune_params.table_length = NV_RF_POWER_MODE_SIZ * NV_VCO_COARSE_TUNE_TABLE_SIZ;
                #else
                vco_coarse_tune_params.table_length = NV_VCO_COARSE_TUNE_TABLE_SIZ;
                #endif /* RF_HAS_POWER_MODE_LIB */

                rfr_load_vco_coarse_tune_table( path, &vco_coarse_tune_params );
                break;
              }
            }

            if ( do_vco_calibration )
            {
              vco_coarse_tune_params_type vco_coarse_tune_params;

              /* Use the space that is pre-allocated in NV for passing the
                 tables around. */
              
              vco_coarse_tune_params.coarse_tune_table = nv_digital_items_ptr->rfnv_vco_coarse_tune_table[path];
              vco_coarse_tune_params.coarse_tune_2 =     nv_digital_items_ptr->rfnv_vco_coarse_tune_2[path];                   

              #ifdef RF_HAS_POWER_MODE_LIB
              vco_coarse_tune_params.table_length = NV_RF_POWER_MODE_SIZ * NV_VCO_COARSE_TUNE_TABLE_SIZ;
              #else
              vco_coarse_tune_params.table_length = NV_VCO_COARSE_TUNE_TABLE_SIZ;
              #endif /* RF_HAS_POWER_MODE_LIB */

              /* do the VCO coarse tuning */
              if(rfr_do_vco_coarse_tuning(path, &vco_coarse_tune_params ) == SAVE_COARSE_TUNE_TBL )
              {
                rf_save_block_vco_coarse_tuning_results((rf_card_blocks_type)block_count, path, &vco_coarse_tune_params);
              }
              /* If not saving the coarse tune values, the RFNV is never
                 going to be touched again anyway, so there is no harm in
                 leaving the unsaved values there. 
               */
            } /* if ( do_vco_calibration ) */
          }/*if(nv_digital_items_ptr != NULL)*/
        }/*for()*/ 
        #endif
      /* Restore default band */
      rfr_set_curr_band(path, band_save);
    
      /* Remember that we did VCO coarse tuning on this path. */
      vco_coarse_tune_done_chain[path] = TRUE;
    
  } /* Only do this once */
}
/*===========================================================================

FUNCTION RF_DO_RESISTOR_CALIBRATION

DESCRIPTION
  

DEPENDENCIES
  NV must have already been read

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void rf_do_resistor_calibration( rfcom_device_enum_type device )
{
  byte resistor_value = 0xff;
  rf_path_enum_type path = rfcom_to_path[ device ];

#if defined(RF_HAS_MULTIMODE_ARCH)
  extern nv_stat_enum_type rfnv_nv_write( nv_items_enum_type, nv_item_type * );
#else
#error code not present
#endif

  // If the nv value is equal to the default, do the calibration. Otherwise, load the default value.
  resistor_value = rf_nv_common_items_ptr->rfnv_rfr_iq_line_resistor;
  if( resistor_value == RF_RFR_IQ_LINE_RESISTOR )
  {
    rfr_do_tunable_resistor_calibration( path, &resistor_value);
#if defined(RF_HAS_MULTIMODE_ARCH)
    /* write the value to NV */
    (void)rfnv_nv_write( NV_RFR_IQ_LINE_RESISTOR_I, 
                         (nv_item_type*)(&resistor_value) ); /*lint !e826 union type is valid */
#else
#error code not present
#endif
  }
  else
  {
    rfr_load_tunable_resistor_calibration_val( path, rf_nv_common_items_ptr->rfnv_rfr_iq_line_resistor );
  }
}
/*===========================================================================

FUNCTION RF_DO_FILTER_COARSE_TUNING

DESCRIPTION
  This function performs the RFR filter coarse tuning

DEPENDENCIES
  NV must have already been read

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void rf_do_filter_coarse_tuning( rfcom_device_enum_type device )
{
  rf_path_enum_type path = rfcom_to_path[ device ];

#if defined(RF_HAS_MULTIMODE_ARCH)
  extern nv_stat_enum_type rfnv_nv_write( nv_items_enum_type, nv_item_type * );
#else
#error code not present
#endif
  static boolean rf_filter_tune_done_chain[2] = {FALSE, FALSE};

  byte filter_coarse_tune_code;

  if (path >= RF_PATH_MAX)
  {
      MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
      rf_error_flag = TRUE;
      return;
  }

  switch ( device)
  {
  case RFCOM_TRANSCEIVER_0:  /* fall through */
  case RFCOM_RECEIVER_1:

    /* Perform RFR filter tuning if required */
    if ( !rf_filter_tune_done_chain[path] )
    {
      /* determine band settings */
      rfc_set_mode(path, rf_card.rfc_default_band);

      /* get the filter coarse tune value from NV */
      filter_coarse_tune_code = rf_nv_common_items_ptr->rfnv_rfr_bb_filter[path];
      
      /* 
      *  Do the filter tuning and check the result - update internal device struct with
      *  filter tune value. Non-save return indicates potential default value has been used.
      */
      if ( rfr_do_filter_coarse_tune( path,
                                  &filter_coarse_tune_code ) == SAVE_FILTER_COARSE_TUNE_VAL )
      {
        MSG_MED("RF CH%d BB filter update = %d", path, filter_coarse_tune_code,0);
      }
      else
      {
        if ( filter_coarse_tune_code != rf_nv_common_items_ptr->rfnv_rfr_bb_filter[path] ) 
        {
          /* BB filter default used - NV value was originally an invalid value */
          ERR("RF CH%d BB filter default used %d", path, filter_coarse_tune_code, 0);
        }
      }

      /* 
      *  Save BB filter value only if value returned is different than value
      *  already stored in NV.
      */
      if ( filter_coarse_tune_code != rf_nv_common_items_ptr->rfnv_rfr_bb_filter[path] ) 
      {
         if ( device == RFCOM_TRANSCEIVER_0 )
         {
#if defined(RF_HAS_MULTIMODE_ARCH)
           /* write the value to NV */
           (void)rfnv_nv_write( NV_RFR_BB_FILTER_I, 
                                (nv_item_type*)(&filter_coarse_tune_code) ); /*lint !e826 union type is valid */
#else
#error code not present
#endif
         }
         else /* RFCOM_TRANSCEIVER_1 */
         {
#if defined(RF_HAS_MULTIMODE_ARCH)
           /* write the value to NV */
           (void)rfnv_nv_write( NV_C1_RFR_BB_FILTER_I, 
                                (nv_item_type*)(&filter_coarse_tune_code) ); /*lint !e826 union type is valid */
#else
#error code not present
#endif
         }
       }
      
      /* we don't do this more than once */
      rf_filter_tune_done_chain[path] = TRUE;
    }
    break;

  default:
    ERR_FATAL(" rf_do_vco_coarse_tuning(): device %d not supported!!!", device, 0, 0);
  }
}



/*===========================================================================

FUNCTION RF_INIT_DELAYED                                EXTERNALIZED FUNCTION

DESCRIPTION
  This is the second half of RF initialization.  This function sets up and
  tests the RF synthesizers.  This function updates the database item
  DB_RF_MODE to either DB_RF_CDMA or DB_RF_PCN according to rf_config.

DEPENDENCIES
  rf_config must have been loaded from NV storage, else ERROR
  rf_state set to RF_STATE_SLEEP_2

RETURN VALUE
  MC_ONLINE_F           Initialization was successful, go on-line
  MC_OFFLINE_DIGITAL_F  Initialization failed, go off-line digital

SIDE EFFECTS
  Updates database item DB_RF_MODE = DB_RF_CDMA or DB_RF_PCN
  Updates database item DB_CHANNEL = RF_PCS_CHAN_MAX_FREQ
  There are software delays of 5+5+40+40 milliseconds in rf_init_delayed()

===========================================================================*/
#if defined(RF_HAS_MULTIMODE_ARCH)
rf_init_status_type rf_init_delayed( rfcom_device_enum_type device )
#else
#error code not present
#endif
{
  word rf_chan_min;              /* temp for channel with minimum frequency */
  word rf_chan_max;              /* temp for channel with maximum frequency */
  rflib_min_max_chan_type chan_min_max;
  rf_init_status_type init_status = RF_INIT_SUCCESS;
  rf_path_enum_type path = rfcom_to_path[ device ];
  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];
  rf_hw_type rf_hw;
#ifdef T_SC2X_PBS
#error code not present
#endif

  switch (device )
  {
  case RFCOM_TRANSCEIVER_0:

    if( rf_nv_data.rfnv_rfcal_valid == FALSE ) {   /* RF CAL invalid */
        ERR("RF CAL not valid", 0,0,0);
        init_status = RF_INIT_INVALID_RFCAL;
    }

    /* ----------------------------------------------
    ** Set the AGC linearizer range
    ** ---------------------------------------------- */
    if( rf_init_cagc_linearizer_range() == FALSE ) {
      ERR("Incorrect linearizer range", 0,0,0);
      init_status = RF_INIT_INVALID_MODE;
    }

    /* ----------------------------------------------
    ** Initialize the RF related NV items
    ** ---------------------------------------------- */
    rf_init_rf_nv_items();

    #ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO
    /* Setting the Clock source for RXF_SAMP_CLK to be TCXO at initialization*/
    #ifndef RF_HAS_HW_QSC11X0
    rf_adc_sel_rxf_sample_clk_src(0);
    #endif
    #endif

    /* ----------------------------------------------
    ** Cancel sleep
    ** ---------------------------------------------- */
    rf_enable_digital_rx_pwr(device);

    /* ----------------------------------------------
    ** Wait for stable Rx power
    ** ---------------------------------------------- */
    clk_busy_wait( RF_ENABLE_RX_PWR_WAIT_US );

    /* -------------------------------------------------------
    ** Configure internal variables related to band selection
    ** ------------------------------------------------------- */
    rf_set_band_initializations(device);

    /* ----------------------------------------------
    ** Initialize the RF hardware for CDMA operation.
    ** ---------------------------------------------- */
    if( rfc_init_card( device,rf_chain_ptr->rf_curr_cdma_band ) == FALSE )
      init_status = RF_INIT_INVALID_MODE;

    /* Initialize library comm services */
    rflib_init_comm();

    switch ( rf_path_0.rfrdev )
    {
    case RFR6135:
    case RFR6500:
    case RFR6525:
    case RTR6500_RX:
       rf_do_resistor_calibration( device );
       break;

    default:
       /* Assume no resistor tune needed for other chips */
       break;
    }

    /* Perform Filter Coarse Tuning if required */
    switch(rf_path_0.rfrdev)
    {
    case RFR6000:
    case RFR6125:
    case RFR6135:
    case RFR6175:
    case RFR6500:
    case RFR6525:
    case RFR61X5:
    case RTR6500_RX:
        rf_do_filter_coarse_tuning( device );
        break;
    default:
        break;
    }

#if !defined( RF_USE_EXT_VCO_FOR_CDMA )
    
    /* Perform VCO Coarse Tuning if required */
    switch(rf_path_0.rfrdev)
    {
    case RFR6125:
    case RFR6135:
    case RFR6175:
    case RFR6500:
    case RFR6525:
        rf_do_vco_coarse_tuning( device);
        break;
    default:
        break;
    }

#endif //! RF_USE_EXT_VCO_FOR_CDMA

    /* ----------------------------------------------
    ** Initialize the RX Front
    ** ---------------------------------------------- */
    rxf_init(path);

    /* program RX Front for startup CDMA band */
    rxf_program_mode_cdma( path,  rf_chain_ptr->rf_curr_cdma_band );

    /* Initialize for CDMA operation */
    rf_init_digital(device);

    /* ----------------------------------------------
    ** Complete RF warmup
    ** ---------------------------------------------- */
    rf_complete_warmup();

    /* ----------------------------------------------------------
    ** Configure for CDMA mode operation
    ** ---------------------------------------------------------- */
#ifndef RF_BYPASS_SYSTEM_CTL
    HWIO_OUTM (SYSTEM_MODE, RF_SYSTEM_MODE_CDMA_HDR_M, RF_SYSTEM_MODE_CDMA_V) 
#endif

    /* ----------------------------------------------------------------
    ** Prepare to test the synthesizer.  Make sure that it can lock to
    ** the low and high channel.
    ** ---------------------------------------------------------------- */
#ifdef RF_USE_EXT_VCO_FOR_CDMA
    /*  Not all possible BC5 channels may be supported when external VCO
    **  is used with RFR6175.
    */
    if ( rf_path_0.rfrdev == RFR6175 )
    {
      extern boolean fzrf_get_band_min_max_chans( rf_path_enum_type, 
                                                  rf_card_band_type, 
                                                  rflib_min_max_chan_type* );

      if (! fzrf_get_band_min_max_chans( path, rf_chain_ptr->rf_curr_cdma_band, 
                                 &chan_min_max ) )
      {
        MSG_ERROR("Band enum %d not supported", rf_chain_ptr->rf_curr_cdma_band, 0, 0);
      }
    }
    else
    {
      if ( !rflib_get_band_min_max_chans( path, rf_chain_ptr->rf_curr_cdma_band, &chan_min_max ))
      {
        MSG_ERROR("Band enum %d not supported", rf_chain_ptr->rf_curr_cdma_band, 0, 0);
      }
    }
#else
    if ( !rflib_get_band_min_max_chans( path, rf_chain_ptr->rf_curr_cdma_band, &chan_min_max ))
    {
      MSG_ERROR("Band enum %d not supported", rf_chain_ptr->rf_curr_cdma_band, 0, 0);
    }
#endif

    rf_chan_min = chan_min_max.min_chan;
    rf_chan_max = chan_min_max.max_chan;
    
#ifdef FEATURE_SBI_MDSP_CONTROLLER
#ifdef RF_HAS_MULTIMODE_SBI
#error code not present
#endif
#endif

    MSG_LOW("Testing the synthesizer for low/high lock", 0, 0, 0);

    /* Sleep rfr for synth lock test...we're only testing the synths */
    rfr_sleep_enable( path );

    /* pll buffer in rfr must be enabled to get synth lock */
    rfrx_pll_buffer_enable( device );

    /* enable the rx pll */
    synth_rfrx_pll_enable( device,  rf_chain_ptr->rf_mode );

    /* tune to the low channel */
    rfc_tune_synth(device, rf_chan_min );

    /* wait for all lock status */
    clk_busy_wait( 2*RF_TUNE_CHAN_WAIT_US ); /*lint !e834 clk_busy_wait macro def */

    if ( rf_chk_synth_lock( device, RF_SYNTH_INIT_TEST ) == RF_SYNTH_NOT_LOCKED ) {
      ERR("Could not lock synth low", 0, 0, 0);
      init_status = RF_INIT_HW_FAIL;
    }

    /* tune to the high channel */
    rfc_tune_synth(device, rf_chan_max );

    /* wait for all lock status */
    clk_busy_wait( 2*RF_TUNE_CHAN_WAIT_US ); /*lint !e834 clk_busy_wait macro def */

    if ( rf_chk_synth_lock( device, RF_SYNTH_INIT_TEST ) == RF_SYNTH_NOT_LOCKED ) {
      ERR("Could not lock synth high", 0, 0, 0);
      init_status = RF_INIT_HW_FAIL;
    }

#ifdef RF_HAS_GPS
#error code not present
#endif /* RF_HAS_GPS */

    /* ------------------------------------------------------------
    ** Initialize enc PA control (indicate if PCS board is present)
    ** rf_band_1900 will tell it that we are doing PCN
    ** ------------------------------------------------------------ */
    switch (rf_chain_ptr->rf_curr_cdma_band) {
    case RF_BC0_BAND: /* US Cell */
    case RF_BC3_BAND: /* JCDMA */
    case RF_BC10_BAND:
        #if defined(FEATURE_ENC_BTF_IN_NV)
        enc_init_pa( FALSE, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_enc_btf_dly );
        #else
        enc_init_pa( FALSE);
        #endif
        MSG_LOW("Encoder initialized to CDMA", 0, 0, 0);
        break;
    case RF_BC1_BAND: /* US PCS */
    case RF_BC4_BAND: /* KPCS   */
    case RF_BC5_BAND: /* NMT    */
    case RF_BC6_BAND: /* IMT    */
    case RF_BC11_BAND: /* BC11  */
    case RF_BC14_BAND: /* US PCS + 5 MHz. */
    case RF_BC15_BAND: /* AWS band */
        #if defined(FEATURE_ENC_BTF_IN_NV)
        enc_init_pa( TRUE, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_enc_btf_dly );
        #else
        enc_init_pa( TRUE);
        #endif
        MSG_LOW("Encoder initialized to PCS", 0, 0, 0);
        break;
    default:
        break;

    }

    /* -----------------------------------------------
    ** Initialization was successful, OK to go on-line
    ** ----------------------------------------------- */
    MSG_MED("RF device %d going on-line", device, 0, 0);

#ifdef RF_CAGC_DEBUG
#error code not present
#endif /* RF_CAGC_DEBUG */

    /* -----------------------------------------------
    ** Put rfr in idle (rx) mode
    ** ----------------------------------------------- */
    rfr_idle_enable( path );

    /* -----------------------------------------------
    ** Disable transmit (put rft into idle (rx) mode )
    ** ----------------------------------------------- */
    rft_idle_enable( path );

    /* If the NV_RF_HW_CONFIG_I contains an invalid value, then force a QCN load */ 
    if(!((rf_hw= rf_get_supported_hw()) > RF_HW_UNDEFINED && rf_hw < RF_HW_MAX))
    {
        ERR("RF CAL not valid", 0,0,0);
        init_status = RF_INIT_INVALID_RFCAL;
    }

     /* Disable Primary Chain */
    rf_sleep_cdma1x( RFCOM_TRANSCEIVER_0 );
    rf_stay_asleep_cdma1x( RFCOM_TRANSCEIVER_0 );

    break;

  case RFCOM_RECEIVER_1:

      if((rf_card_in_system == RF_HW_QSC11X0_DUAL_BAND) || (rf_card_in_system == RF_HW_QSC11X0_TRI_BAND)
         || (rf_card_in_system == RF_HW_QSC11X0_TRI_BAND_BC5_TRI_STATE_PA))
      {
          return RF_INIT_SUCCESS;          
      }

    if ( rf_path_1.rfrdev == NO_RFR )
    {
      /* If the second receiver has no RFR, skip initialization */
      break;
    }

    /* ----------------------------------------------
    ** Cancel sleep
    ** ---------------------------------------------- */
    rf_enable_digital_rx_pwr(device);

    /* ----------------------------------------------
    ** Wait for stable Rx power
    ** ---------------------------------------------- */
    clk_busy_wait( RF_ENABLE_RX_PWR_WAIT_US );

    /* ----------------------------------------------
    ** Initialize the RX Front
    ** ---------------------------------------------- */
    rxf_init(path);

    /* ----------------------------------------------
    ** Initialize the RF hardware for CDMA operation.
    ** ---------------------------------------------- */
    if( rfc_init_card( device,rf_chain_ptr->rf_curr_cdma_band ) == FALSE )
      init_status = RF_INIT_INVALID_MODE;

#if !defined( RF_USE_EXT_VCO_FOR_CDMA )
    
    /* Perform VCO Coarse Tuning if required */
    switch(rf_path_1.rfrdev)
    {
    case RFR6125:
    case RFR6135:
    case RFR6175:
        rf_do_vco_coarse_tuning( device);
        break;
    default:
        break;
    }

#endif

    /* Perform Filter Coarse Tuning if required */
    switch(rf_path_1.rfrdev)
    {
    case RFR6000:
    case RFR6125:
    case RFR6175:
        rf_do_filter_coarse_tuning( device );
        break;
    default:
        break;
    }
      
    /* program RX Front for startup CDMA band */
    rxf_program_mode_cdma( path,  rf_chain_ptr->rf_curr_cdma_band );

    /* Initialize second chain */
    rf_init_digital( device );

#ifdef RF_HAS_GPS
#error code not present
#endif /* RF_HAS_GPS */

    /* Disable Primary Chain */
    rf_sleep_cdma1x( RFCOM_TRANSCEIVER_0 );
    rf_stay_asleep_cdma1x( RFCOM_TRANSCEIVER_0 );

    /* Sleep Chain 1*/
    rf_sleep_cdma1x( device );

    rf_stay_asleep_cdma1x( device );

    /* -----------------------------------------------
    ** Initialization was successful, OK to go on-line
    ** ----------------------------------------------- */
    MSG_MED("RF device %d initialized and sleeping", device, 0, 0);

    break;

  default:
      ERR_FATAL(" rf_init_delayed(): device %d not supported!!!", device, 0, 0);
  }

#ifdef T_SC2X_PBS
#error code not present
#endif  /* T_SC2X_PBS */

#if defined( T_RUMI_SC2X )
#error code not present
#endif

/* Return type based upon API selected
*/
#if defined(RF_HAS_MULTIMODE_ARCH)
  return( init_status );
#else
#error code not present
#endif

} /* rf_init_delayed */

/*===========================================================================

FUNCTION RF_IS_SLEEPING_CDMA1X

DESCRIPTION    Queries whether the RF is sleeping right now.

DEPENDENCIES   None.

RETURN VALUE   True if RF is in sleep state, FALSE otherwise.

SIDE EFFECTS   None.

==========================================================================*/
// TBD: RF_TODO_7600_ADK
/* jcen This is temporary until UI stops calling rf_is_sleeping */
boolean rf_is_sleeping ( rfcom_device_enum_type device)
{
  return rf_is_sleeping_cdma1x(device);
}

boolean rf_is_sleeping_cdma1x ( rfcom_device_enum_type device)
{
  rf_path_enum_type path = rfcom_to_path[ device ];

  if (path >= RF_PATH_MAX)
  {
      MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
      rf_error_flag = TRUE;
      return 0;
  }

  /* Check to determine the rf state variable for sleep condition */
  if (( rf_chain_status[ path ].rf_state == RF_STATE_SLEEP_1 ) ||
      ( rf_chain_status[ path ].rf_state == RF_STATE_SLEEP_2 ))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* rf_is_sleeping_cdma1x */


/*===========================================================================

FUNCTION RF_ENABLE_DIVERSITY                           EXTERNALIZED FUNCTION

DESCRIPTION
  This function configures the second chain to be in the same digital mode
  as the first chain.  The second chain uses the PLL from the first chain.

DEPENDENCIES
  rf_state of first chain must be RF_STATE_CDMA_RX or RF_STATE_CDMA_RXTX
  device pramater must be RFCOM_RECEIVER_DIV
  RF LDOs for secondary chain must be previously turned on

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean rf_enable_diversity( rfcom_device_enum_type device )
{
  rf_path_enum_type path = rfcom_to_path[ device ];
#ifdef RF_HAS_LINEAR_INTERPOLATION
  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];
#endif

 #ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO
#ifdef RF_HAS_ADC_SAMPLING_CLK_DEBUG
#error code not present
#endif
 #endif
 
  /* Only can call this function with RFCOM_RECEIVER_DIV */
  if ( device == RFCOM_RECEIVER_DIV )
  {
        
    boolean diversity_chain_was_sleeping = FALSE;
    boolean diversity_band_changed = FALSE;
    boolean diversity_chan_changed = FALSE;

    rf_chain_state_type *transceiver_status_ptr =
        &rf_chain_status[ RF_PATH_0 ];

    rf_chain_state_type *div_receiver_status_ptr =
        &rf_chain_status[ rfcom_to_path[ RFCOM_RECEIVER_DIV ] ];

   
    if ( div_receiver_status_ptr->rf_state == RF_STATE_DIVERSITY )
    {
      /* currently in diversity...nothing to do */
      MSG_LOW("RF already in diversity", 0, 0, 0);

      return TRUE;
    }

#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */

    switch ( transceiver_status_ptr->rf_state )
    {
    /* primary chain must be in either RF_STATE_CDMA_RX or RF_STATE_CDMA_RXTX */
    case RF_STATE_CDMA_RX:
    case RF_STATE_CDMA_RXTX:

      if ( div_receiver_status_ptr->rf_state != RF_STATE_CDMA_RX
           #ifdef RF_HAS_CGPS
#error code not present
           #endif 
         )
      {
        ERR_FATAL( "Invalid RF STATE: %d", div_receiver_status_ptr->rf_state, 0,0 );
      }

      if ( div_receiver_status_ptr->rf_mode == DB_RF_SLEEP )
      {
          /* coming from sleep */
          diversity_chain_was_sleeping = TRUE;
      }
      /* enable CAGC Rx control CAGC internal hardware */
      MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_LNA_CTL) ,
                     AGC_VALUE_OVERRIDE,
                     CAGC_RF_AGC_VALUE_ACCUM_NORM_V,
                     (int)(rfcom_to_path[RFCOM_RECEIVER_DIV]));

     /* put the CAGC loop integrator in normal mode */
      MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_TEST_CTL),
                     INTEGRATOR_BYPASS,
                     CAGC_RF_LI_BYPASS_DIS_V,
                     (int)(rfcom_to_path[RFCOM_RECEIVER_DIV]));

      if (( div_receiver_status_ptr->rf_curr_cdma_band != transceiver_status_ptr->rf_curr_cdma_band) || (div_receiver_status_ptr->rf_chan == 0))
      {
        diversity_band_changed = diversity_chan_changed = TRUE;
      }
      else if ( div_receiver_status_ptr->rf_chan != transceiver_status_ptr->rf_chan )
      {
        diversity_chan_changed = TRUE;
      }

      div_receiver_status_ptr->rf_nv_dig_items_ptr = transceiver_status_ptr->rf_nv_dig_items_ptr;
      div_receiver_status_ptr->rf_mode = transceiver_status_ptr->rf_mode;
      div_receiver_status_ptr->rf_freq_index = transceiver_status_ptr->rf_freq_index;
      div_receiver_status_ptr->rf_chan =  transceiver_status_ptr->rf_chan;
      div_receiver_status_ptr->rf_prev_cdma_band =  div_receiver_status_ptr->rf_curr_cdma_band;
      div_receiver_status_ptr->rf_curr_cdma_band =  transceiver_status_ptr->rf_curr_cdma_band;

      #ifdef RF_HAS_SUB_BLOCK_SUPPORT
      if( div_receiver_status_ptr->rf_curr_cdma_band < RF_MAX_BAND)
      {
      rf_digital_nv_table[RF_PATH_1][div_receiver_status_ptr->rf_curr_cdma_band] = div_receiver_status_ptr->rf_nv_dig_items_ptr;
      }
      else
      {
        MSG_FATAL("Band Class invalid or out of bounds", 0, 0, 0);
        rf_error_flag = TRUE;
        return FALSE;
      }
      #endif

      rf_hw_lna_setup_digital(RFCOM_RECEIVER_DIV);

      #ifdef RF_HAS_LINEAR_INTERPOLATION
      /* interpolate Rx item */
      rf_update_freq_comp_items( RF_PATH_1, div_receiver_status_ptr->rf_chan);
      #endif
      
      rf_update_lna_offsets(RFCOM_RECEIVER_DIV);
      
      #ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO
      diversity_band_changed = TRUE;      
      diversity_chan_changed = TRUE;
      #endif

      /* set up RF to same band as the primary chain */
      if(rfc_init_card( RFCOM_RECEIVER_DIV, div_receiver_status_ptr->rf_curr_cdma_band) == FALSE )
      { 
        diversity_band_changed = FALSE;
      }

      /* enable the pll buffer in the RFR */
      rfrx_pll_buffer_enable( RFCOM_RECEIVER_DIV );


      /* disable secondary rx pll */
      synth_rfrx_pll_disable( RFCOM_RECEIVER_1 );

  #ifdef RF_HAS_IM2
      /* Load the IM2 values */
#ifdef RF_HAS_POWER_MODE_LIB
      (void)rfr_I_Q_dac_set (rfcom_to_path[ RFCOM_RECEIVER_DIV ],
                       div_receiver_status_ptr->rf_nv_dig_items_ptr->rfnv_im2_i_value[rfcom_to_path[ RFCOM_RECEIVER_DIV ]],
                       div_receiver_status_ptr->rf_nv_dig_items_ptr->rfnv_im2_q_value[rfcom_to_path[ RFCOM_RECEIVER_DIV ]],
                       div_receiver_status_ptr->rf_nv_dig_items_ptr->rfnv_im2_transconductor_value[rfcom_to_path[ RFCOM_RECEIVER_DIV ]]);
#else
      (void)rfr_I_Q_dac_set (rfcom_to_path[ RFCOM_RECEIVER_DIV ],
                       &div_receiver_status_ptr->rf_nv_dig_items_ptr->rfnv_im2_i_value[rfcom_to_path[ RFCOM_RECEIVER_DIV ]],
                       &div_receiver_status_ptr->rf_nv_dig_items_ptr->rfnv_im2_q_value[rfcom_to_path[ RFCOM_RECEIVER_DIV ]],
                       &div_receiver_status_ptr->rf_nv_dig_items_ptr->rfnv_im2_transconductor_value[rfcom_to_path[ RFCOM_RECEIVER_DIV ]]);
#endif /* RF_HAS_POWER_MODE_LIB */
  #endif /* RF_HAS_IM2    */

      if ( diversity_band_changed )
      {
        /* setup the rxf front based on rf_mode */
        rxf_program_mode_cdma(rfcom_to_path[ RFCOM_RECEIVER_DIV ],
                              div_receiver_status_ptr->rf_curr_cdma_band );

      }
      if ( diversity_chan_changed )
      {
#ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO 
#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */
         if(rf_hdr_is_enabled[RF_PATH_0] == FALSE)
         {
            if(rf_adc_clk_status.mode[0] != CDMA_MODE)
            {
              RF_FATAL_ERROR("Primary chain should have been in CDMA Mode");             
            }

            if(rf_adc_clk_status.vco_state[0] != VCO_ON)
            {
              RF_FATAL_ERROR("PRx chain VCO is not turned ON which should have been done by now");
            }           

             rf_adc_clk_status.mode[1] = CDMA_MODE;

             rf_1x_chain_switch_adc_clk_set_rxf(RF_PATH_1,VCO_OFF,DIV_ON,rf_adc_clk_status.mode[1]);         
           }
         
#endif /* RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO */
       
        /* Update RxFront DVGA Gain Offset to new frequency */
        rxf_set_vga_gain_offset(rfcom_to_path[ RFCOM_RECEIVER_DIV ],
                                div_receiver_status_ptr->rf_freq_index);
      }

      if ( diversity_chain_was_sleeping )
      {
        /* wake up diversity chain */
        rf_cdma_sleep_wakeup( RFCOM_RECEIVER_DIV );
      }
      else
      {
        /* wake up the RFR */
        rfr_sleep_disable( rfcom_to_path[ RFCOM_RECEIVER_DIV ] );

        /* Start DC acquistion */
        rxf_cdma_wakeup_dc_acquire(rfcom_to_path[ RFCOM_RECEIVER_DIV ]);

      }
      #ifdef RF_HAS_SUB_BLOCK_SUPPORT
      rf_block_mask_is_forced[RF_PATH_1] = rf_block_mask_is_forced[RF_PATH_0];

      rf_forced_block_mask_value[RF_PATH_1] = rf_forced_block_mask_value[RF_PATH_0];  

      rf_forced_nv_item_ptr[RF_PATH_1] = rf_forced_nv_item_ptr[RF_PATH_0];
      #endif

      break;
#ifndef RF_HAS_BYPASS_RF_STATE_GPS_CHECK
    case RF_STATE_GPS:
      /* Currently no action required for GPS */
      break;
#endif

    default:
        ERR_FATAL( "Invalid RF state: %d", transceiver_status_ptr->rf_state, 0,0 );
    }

    div_receiver_status_ptr->rf_prev_state = div_receiver_status_ptr->rf_state;

    div_receiver_status_ptr->rf_state = RF_STATE_DIVERSITY;

#ifdef RF_HAS_LINEAR_INTERPOLATION
    /* perform interpolation on chain 1 */
     rf_update_freq_comp_items( path, rf_chain_ptr->rf_chan);
#endif

    return TRUE;

  }

  ERR("Incorrect state(%d)/device(%d) ",
        rf_chain_status[ rfcom_to_path[ RFCOM_TRANSCEIVER_0 ] ].rf_state,
        RFCOM_RECEIVER_DIV, 0);

  return FALSE;
}



/*===========================================================================

FUNCTION RF_DISABLE_DIVERSITY                           EXTERNALIZED FUNCTION

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean rf_disable_diversity( rfcom_device_enum_type device )
{
    rf_chain_state_type *transceiver_status_ptr =
        &rf_chain_status[ rfcom_to_path[ RFCOM_TRANSCEIVER_0 ] ];

    rf_chain_state_type *div_receiver_status_ptr =
        &rf_chain_status[ rfcom_to_path[ RFCOM_RECEIVER_DIV ] ];

    /* Only can call this function with device RFCOM_RECEIVER_DIV and diversity
    ** chain in RF_STATE_DIVERSITY
    */
    if  ( ( device == RFCOM_RECEIVER_DIV ) &&
          ( div_receiver_status_ptr->rf_state == RF_STATE_DIVERSITY ) )
    {

      switch ( transceiver_status_ptr->rf_state )
      {
      case RF_STATE_CDMA_RX:
      case RF_STATE_CDMA_RXTX:

        /* Configure secondary rfr back to select internal vco */
        rfr_use_external_vco(rfcom_to_path[ RFCOM_RECEIVER_DIV ],FALSE);

        /* sleep the RFR on the diversity chain */
        rfr_sleep_enable( rfcom_to_path[ RFCOM_RECEIVER_DIV ] );

        /* state of diversity chain is same as state of primary chain */
        div_receiver_status_ptr->rf_prev_state = div_receiver_status_ptr->rf_state;
        div_receiver_status_ptr->rf_state = RF_STATE_CDMA_RX;

        break;

      #ifndef RF_HAS_BYPASS_RF_STATE_GPS_CHECK
      case RF_STATE_GPS:
        break;
      #endif

      default:
          ERR_FATAL( "Invalid RF state: %d", transceiver_status_ptr->rf_state, 0,0 );
      }

      return TRUE;

    }

    ERR("Incorrect state(%d)/device(%d) ", div_receiver_status_ptr->rf_state,
          device, 0);

    return FALSE;

}



/*===========================================================================

FUNCTION RF_ENABLE_DIGITAL_RX_PWR                       EXTERNALIZED FUNCTION

DESCRIPTION
  This function turns on the Rx portion of the RF hardware and sets it
  to CDMA mode.  Once rf_enable_digital_rx_pwr() has been called, a delay of
  RF_ENABLE_PWR_WAIT ms or RF_ENABLE_PWR_WAIT_US us must be performed by the
  application to allow the RF hardware to settle.  After the RF hardware has
  settled, only then should the rf_init_digital() function be called.

DEPENDENCIES
  rf_state must be setup

RETURN VALUE
  None

SIDE EFFECTS
  Updates rf_state = RF_STATE_CDMA_RX

===========================================================================*/
void rf_enable_digital_rx_pwr( rfcom_device_enum_type device )
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  rf_path_enum_type path = rfcom_to_path[ device ];

  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];

  #ifdef RF_HAS_WU_TIMED
    #ifdef RF_HAS_WU_TIMED_DEBUG
#error code not present
    #endif /* RF_HAS_WU_TIMED_DEBUG */
  (void)rf_prof_timer_start(path);
  #endif  /* RF_HAS_WU_TIMED */

  #ifdef RF_TIME_PROFILING
#error code not present
  #endif /* RF_TIME_PROFILING */

  switch ( device )
  {
  case RFCOM_TRANSCEIVER_0:

   /* ----------------------------------------------------------
    ** Make hardware changes based on previous operating RF state
    ** ---------------------------------------------------------- */
    switch (rf_chain_ptr->rf_state)
    {
    case RF_STATE_SLEEP_2:
    #ifndef RF_HAS_BYPASS_RF_STATE_GPS_CHECK
    case RF_STATE_GPS:
    #endif

      /* ------------------------------------------------------------------
      ** Re-enable clock regimes that were disabled in rf_stay_asleep()
      ** ------------------------------------------------------------------ */
#ifndef RF_HAS_RXF_V2
#error code not present
#else /* RF_HAS_RXF_V2 */
      rxf_enable_rx_front_clocks_sc2x(RF_PATH_0);
#endif /* !RF_HAS_RXF_V2 */

      #ifdef RF_HAS_PAD3_CURRENT_REDUCTION_CONTROL
#error code not present
      #endif 

      #ifdef RF_HAS_MSMA_CURRENT_REDUCTION_CONTROL
#error code not present
      #endif

      #ifdef RF_TIME_PROFILING
#error code not present
      #endif /* RF_TIME_PROFILING */
      #ifdef RF_HAS_WU_TIMED_DEBUG
#error code not present
      #endif /* RF_HAS_WU_TIMED_DEBUG */

      /* Enable RX chip */
      rf_enable_rx_chip_power( path, RFC_RX_VREGS );

      #ifdef RF_TIME_PROFILING
#error code not present
      #endif /* RF_TIME_PROFILING */
      break;

    case RF_STATE_SLEEP_1:
      /* -------------
      ** Nothing to do
      ** ------------- */

      /* fall through */

    case RF_STATE_CDMA_RX:
      /* ---------------------------------------
      ** old state == new state.  Nothing to do.
      ** --------------------------------------- */
      break;

    #ifndef RF_HAS_BYPASS_RF_STATE_GPS_CHECK
    case RF_STATE_FM_RXTX:
    #endif
    case RF_STATE_CDMA_RXTX:
    default:
      RF_FATAL_ERROR("Invalid RF state");
    }

    /* ------------------------------
    ** Update local RF state variable
    ** ------------------------------ */
    rf_chain_ptr->rf_prev_state = rf_chain_ptr->rf_state;
    rf_chain_ptr->rf_state = RF_STATE_CDMA_RX;

    MSG_LOW("Enabled digital Rx power", 0, 0, 0);

    break;

  case RFCOM_RECEIVER_DIV:
#ifndef RF_HAS_RXF_V2
#error code not present
#else /* RF_HAS_RXF_V2 */
    rxf_enable_rx_front_clocks_sc2x(RF_PATH_1);
#endif /* !RF_HAS_RXF_V2 */

    #ifdef RF_HAS_PAD3_CURRENT_REDUCTION_CONTROL
#error code not present
    #endif 

    #ifdef RF_HAS_MSMA_CURRENT_REDUCTION_CONTROL
#error code not present
    #endif

    rf_enable_rx_chip_power( path, RFC_RX_DIV_VREGS );

    rf_chain_ptr->rf_prev_state = rf_chain_ptr->rf_state;
    rf_chain_ptr->rf_state = RF_STATE_CDMA_RX;

    break;

  case RFCOM_RECEIVER_1:
#ifndef RF_HAS_RXF_V2
#error code not present
#else /* RF_HAS_RXF_V2 */
    rxf_enable_rx_front_clocks_sc2x(RF_PATH_1);
#endif /* !RF_HAS_RXF_V2 */

    #ifdef RF_HAS_PAD3_CURRENT_REDUCTION_CONTROL
#error code not present
    #endif 

    #ifdef RF_HAS_MSMA_CURRENT_REDUCTION_CONTROL
#error code not present
    #endif

    rf_enable_rx_chip_power( path, RFC_RX_VREGS );

    rf_chain_ptr->rf_prev_state = rf_chain_ptr->rf_state;
    rf_chain_ptr->rf_state = RF_STATE_CDMA_RX;

    break;

  default:
    break;

  }

  #if (!defined T_FFA) && defined (RF_TIME_PROFILING)
#error code not present
  #endif /* !T_FFA && RF_TIME_PROFILING */

  #ifdef RF_HAS_WU_TIMED

  #ifdef RF_HAS_WU_TIMED_DEBUG
#error code not present
  #else
    (void)rf_prof_timer_stop(path);
  #endif /* RF_HAS_WU_TIMED_DEBUG */
  #endif /* RF_HAS_WU_TIMED */

} /* rf_enable_digital_rx_pwr */



/*===========================================================================

FUNCTION RF_INIT_DIGITAL                                EXTERNALIZED FUNCTION

DESCRIPTION
  Program the MSM2P CDMA AGC section.  Program the UHF Tx/Rx synthesizer.
  Power up the VHF Rx LO synthesizer in the BB2.  Set the RF hardware for CDMA
  mode of operation.  This procedure should be called every time CDMA mode is
  entered and whenever we exit sleep.  When called after sleep, the MSM is
  placed in DFM mode, and the I/Q DC offset and TRK_LO_ADJ PDMs are loaded
  to their pre-sleep values.  This allows the loops to converge by the time
  we switch to CDMA mode after warm up.

DEPENDENCIES
  rf_cdma_rx_lin_off[], rf_cdma_tx_lin_vs_temp_now,
    rf_cdma_tx_lin_master_off[], rf_config, rf_lna_range_offset,
    and rf_cdma_tx_lin_master_slp[] must have been loaded from NV storage
  rf_state must be setup = RF_STATE_CDMA_RX
  Database item DB_RF_MODE must be setup
  Database item DB_CHANNEL must be setup if exiting from sleep
  The MSM1 core AGC hardware should have been disabled first
  Procedure rf_enable_digital_rx_pwr() must have been called before calling
    this procedure.
  If called after sleep, rf_complete_warmup() must be called when the warm
    up timer expires to finish digital initialization.

RETURN VALUE
  None

SIDE EFFECTS
  Updates temperature data
  Updates database item DB_RF_MODE = DB_RF_CDMA or DB_RF_PCN

===========================================================================*/
void rf_init_digital(rfcom_device_enum_type device )
{
  db_items_value_type db_value;  /* temp for database item DB_RF_MODE */
  rf_path_enum_type path = rfcom_to_path[ device ];
  boolean chain1_is_sleeping = FALSE;

  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];

  #if (!defined T_FFA) && defined (RF_TIME_PROFILING)
#error code not present
  #endif /* RF_TIME_PROFILING */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (path >= RF_PATH_MAX)
  {
     MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
     rf_error_flag = TRUE;
     return;
  }
  if (device >= RFCOM_MAX_DEVICES) 
  {
     MSG_FATAL("device >= RFCOM_MAX_DEVICES", 0, 0, 0);
     rf_error_flag = TRUE;
     return;
  }

  #ifdef RF_HAS_HDR
#error code not present
  #endif /* RF_HAS_HDR */

  if (rf_chain_ptr->rf_curr_cdma_band >= RF_MAX_BAND) 
  {
     MSG_FATAL("band class invalid or out of bounds", 0, 0, 0);
     rf_error_flag = TRUE;
     return;
  }

  switch ( device )
  {
  case RFCOM_TRANSCEIVER_0:

    /* -----------------------------------------
    ** Generate error if not in correct rf_state
    ** ----------------------------------------- */
    if (rf_chain_ptr->rf_state != RF_STATE_CDMA_RX) 
    {
      RF_FATAL_ERROR("Invalid RF state");
    }

    /* --------------------------------------------------------------
    ** Check mode to see what kind of initialization needs to be done
    ** -------------------------------------------------------------- */
    switch (rf_chain_ptr->rf_mode) {

    case DB_RF_SLEEP:     /* Currently sleeping */

#if defined(FEATURE_PMIC_MANAGED_LDO)
#if defined(RF_MANAGE_SYNTH_LDO)
      /* ----------------------------------------
      ** Initialize rft because synth LDO was
      ** turned on in rf_enable_digital_rx_pwr()
      ** ---------------------------------------- */
      rft_init( path );
#endif /* RF_MANAGE_SYNTH_LDO */
#endif /* FEATURE_PMIC_MANAGED_LDO */

      break;

    case DB_RF_CDMA:

      /* -----------------------------------------------------------
      ** Old state = new state, nothing to do except verify that we
      ** are in the right band (at 800MHz)
      ** ----------------------------------------------------------- */
      if (!(
          rf_chain_ptr->rf_curr_cdma_band == RF_BC0_BAND || /* US Cellular */
          rf_chain_ptr->rf_curr_cdma_band == RF_BC3_BAND ||   /* JCDMA       */
          rf_chain_ptr->rf_curr_cdma_band == RF_BC10_BAND
          )) 
      {
        RF_FATAL_ERROR( "Invalid RF mode.");
      }
      break;

    case DB_RF_PCN:
      /* -----------------------------------------------------------
      ** Old state = new state, nothing to do except verify that we
      ** are in the right band (at 1900MHz)
      ** ----------------------------------------------------------- */
      if (!(
          (rf_chain_ptr->rf_curr_cdma_band == RF_BC1_BAND) || /* US PCS      */
          (rf_chain_ptr->rf_curr_cdma_band == RF_BC4_BAND) || /* Korean PCS  */
          (rf_chain_ptr->rf_curr_cdma_band == RF_BC5_BAND) || /* NMT         */
          (rf_chain_ptr->rf_curr_cdma_band == RF_BC11_BAND)|| /* BC11        */
          (rf_chain_ptr->rf_curr_cdma_band == RF_BC14_BAND)|| /* US PCS+5MHz */
          (rf_chain_ptr->rf_curr_cdma_band == RF_BC15_BAND)|| /* AWS Band */
          (rf_chain_ptr->rf_curr_cdma_band == RF_BC6_BAND)    /* IMT (2.1GHz)*/          
          )) 
      {
        RF_FATAL_ERROR( "Invalid RF mode.");
      }
      break;
    case DB_RF_GPS:
      /* -----------------------------------------------------------
      ** Old state = new state, nothing to do except verify that we
      ** are in the right band (at 1785MHz)
      ** ----------------------------------------------------------- */
      if (rf_chain_ptr->rf_curr_cdma_band != RF_GPS_BAND) 
      {
        RF_FATAL_ERROR("Invalid RF mode.");
      }
      break;

    case DB_RF_900MHZ:
    default:
      RF_FATAL_ERROR("Invalid RF mode.");
    }

    /* Check where we are coming from */
    if (rf_chain_ptr->rf_mode == DB_RF_SLEEP )
    {
      /* Indicate that we are not completely done; we still have to wait
      ** for warmup to complete before we switch back to CDMA mode.
      */
      rf_dig_init_complete = FALSE;

    }
    else
    {
      /* Indicate that digital is initialized. */
      rf_dig_init_complete = TRUE;
    }
    
    /* ----------------------------
    ** update the external database
    ** ---------------------------- */
    switch(rf_chain_ptr->rf_curr_cdma_band)
    {
      case RF_BC0_BAND: /* US Cellular */
      case RF_BC3_BAND: /* JCDMA       */
      case RF_BC10_BAND:
        rf_chain_ptr->rf_mode = DB_RF_CDMA;
        break;
      case RF_BC1_BAND: /*   US PCS         */
      case RF_BC4_BAND: /*   Korean PCS     */
      case RF_BC5_BAND: /*   NMT            */
      case RF_BC6_BAND: /*   IMT (2.1GHz)   */
      case RF_BC11_BAND:/*   BC11           */
      case RF_BC14_BAND:/*   US PCS + 5MHz. */
      case RF_BC15_BAND:/*   AWS Band       */    
        rf_chain_ptr->rf_mode = DB_RF_PCN;
        break;
      case RF_GPS_BAND:
        rf_chain_ptr->rf_mode = DB_RF_GPS;
        break;
      
      default:
        RF_FATAL_ERROR("Invalid RF mode.");
    }

    db_value.rf_mode = rf_chain_ptr->rf_mode;
    db_put( DB_RF_MODE, &db_value );

    rf_disable_pa_ctl(device);

    rf_hw_lna_setup_digital(device);

    rf_update_lna_offsets(device);


    /* --------------------------------------------------------------
    ** Update the temperature data, prior to loading the linearizers.
    ** -------------------------------------------------------------- */
    rf_cdma_temp_update();

    /* --------------------------------------------------
    ** Initialize the RF card to the proper band whether
    ** or not we are coming from sleep or a band change
    ** -------------------------------------------------- */
    (void) rfc_init_card( device, rf_chain_ptr->rf_curr_cdma_band);
    
    /* ------------------------------------------
    ** Enable rx pll in rft
    ** ----------------------------------------- */
    synth_rfrx_pll_enable( device, rf_chain_ptr->rf_mode );

    /* ------------------------------------------
    ** Enable PLL buffer in rfr
    ** ----------------------------------------- */
    rfrx_pll_buffer_enable( device );

    if (rf_dig_init_complete)
    {
      /* ------------------------------------------------------
      ** Not coming from sleep. We need to reprogram the plls.
      ** ----------------------------------------------------- */
      rfc_tune_synth(device, rf_chain_ptr->rf_chan );
    }
    else /* coming from sleep */
    {
#if defined(FEATURE_PMIC_MANAGED_LDO)
#if defined(RF_MANAGE_SYNTH_LDO)
      /* ------------------------------------------------------
      ** Coming from sleep and SYNTH LDO has been turned off.
      ** We need to reprogram the plls.
      ** ----------------------------------------------------- */
      rfc_tune_synth(device, rf_chain_ptr->rf_chan );
#endif /* RF_MANAGE_SYNTH_LDO */
#endif /* FEATURE_PMIC_MANAGED_LDO */
    }

    /* -----------------------------------------------------
    **  Wait minimum synth lock time before waking up the rfr
    ** ----------------------------------------------------- */
    rf_wait_for_synth_lock(device); 

#ifdef RF_HAS_IM2
    /* Load the IM2 values */
#ifdef RF_HAS_POWER_MODE_LIB
    (void)rfr_I_Q_dac_set (path,
                   rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_i_value[path],
                   rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_q_value[path],
                   rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_transconductor_value[path]);
#else
    (void)rfr_I_Q_dac_set (path,
                   &rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_i_value[path],
                   &rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_q_value[path],
                   &rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_transconductor_value[path]);
#endif /* RF_HAS_POWER_MODE_LIB */
#endif /* RF_HAS_IM2 */

    if (!rf_dig_init_complete)
    {
      /* -----------------------------------------------------------
      ** We are comming from sleep.
      ** ----------------------------------------------------------- */
      rf_cdma_sleep_wakeup(device);

    }
    else
    {
      /* Wake up rfr */
      rfr_sleep_disable( path );

      /* -----------------------------------------------------------
      ** We came form another mode.  Setup RX Front for new mode.
      ** ----------------------------------------------------------- */
      rxf_set_mode_cdma(path, rf_chain_ptr->rf_curr_cdma_band);
    }

    MSG_LOW("Digital initialized", 0, 0, 0);


    break;

  case RFCOM_RECEIVER_1:

    /* -----------------------------------------
    ** Generate error if not in correct rf_state
    ** ----------------------------------------- */
    if (rf_chain_ptr->rf_state != RF_STATE_CDMA_RX) 
    {
      RF_FATAL_ERROR("Invalid RF mode.");
    }

    /* --------------------------------------------------------------
    ** Check mode to see what kind of initialization needs to be done
    ** -------------------------------------------------------------- */
    switch (rf_chain_ptr->rf_mode) {

    case DB_RF_SLEEP:     /* Currently sleeping */

      chain1_is_sleeping = TRUE;

      break;

    case DB_RF_CDMA:

      /* -----------------------------------------------------------
      ** Old state = new state, nothing to do except verify that we
      ** are in the right band (at 800MHz) or JCDMA
      ** ----------------------------------------------------------- */
      if (!(
            rf_chain_ptr->rf_curr_cdma_band == RF_BC0_BAND || /* US Cellular */
            rf_chain_ptr->rf_curr_cdma_band == RF_BC3_BAND ||    /* JCDMA       */
            rf_chain_ptr->rf_curr_cdma_band == RF_BC10_BAND
          ))
      {
        RF_FATAL_ERROR("Invalid RF mode.");
      }
      break;

    case DB_RF_PCN:

      /* -----------------------------------------------------------
      ** Old state = new state, nothing to do except verify that we
      ** are in the right band (at 800MHz) or JCDMA
      ** ----------------------------------------------------------- */
      if (!(
           (rf_chain_ptr->rf_curr_cdma_band == RF_BC1_BAND) || /* US PCS      */
           (rf_chain_ptr->rf_curr_cdma_band == RF_BC4_BAND) || /* Korean PCS  */
           (rf_chain_ptr->rf_curr_cdma_band == RF_BC5_BAND) || /* NMT         */
           (rf_chain_ptr->rf_curr_cdma_band == RF_BC6_BAND) || /* IMT (2.1GHz)*/
           (rf_chain_ptr->rf_curr_cdma_band == RF_BC11_BAND)|| /* BC11        */
           (rf_chain_ptr->rf_curr_cdma_band == RF_BC15_BAND)|| /* AWS Band */
           (rf_chain_ptr->rf_curr_cdma_band == RF_BC14_BAND)   /* US PCS+5MHz */           
         ) )
      {
        RF_FATAL_ERROR( "Invalid RF mode.");
      }
      break;

    case DB_RF_GPS:
      /* -----------------------------------------------------------
      ** Old state = new state, nothing to do except verify that we
      ** are in the right band (at 1785MHz)
      ** ----------------------------------------------------------- */
      if (rf_chain_ptr->rf_curr_cdma_band != RF_GPS_BAND) 
      {
        RF_FATAL_ERROR("Invalid RF mode.");
      }
      break;

    case DB_RF_900MHZ:

    default:
      RF_FATAL_ERROR("Invalid RF mode.");
    }
   
    /* Update the nv pointer */

    #ifdef RF_HAS_SUB_BLOCK_SUPPORT
    rf_chain_ptr->rf_nv_dig_items_ptr = 
        rf_digital_nv_table[path][rf_chain_ptr->rf_curr_cdma_band];    
    #else
    rf_chain_ptr->rf_nv_dig_items_ptr = 
        rf_digital_nv_table[rf_chain_ptr->rf_curr_cdma_band];
    #endif

    /* ----------------------------
    ** update the external database
    ** ---------------------------- */
    switch(rf_chain_ptr->rf_curr_cdma_band)
    {
      case RF_BC0_BAND: /*   US Cellular    */
      case RF_BC3_BAND: /*   JCDMA          */
      case RF_BC10_BAND:
        rf_chain_ptr->rf_mode = DB_RF_CDMA;
        break;

      case RF_BC1_BAND: /*   US PCS         */
      case RF_BC4_BAND: /*   Korean PCS     */
      case RF_BC5_BAND: /*   NMT            */
      case RF_BC6_BAND: /*   IMT (2.1GHz)   */
      case RF_BC11_BAND:/*   BC11           */
      case RF_BC14_BAND:/*   US PCS + 5MHz  */
      case RF_BC15_BAND:/*   AWS Band       */ 
        rf_chain_ptr->rf_mode = DB_RF_PCN;
        break;

      #ifdef RF_HAS_BYPASS_RF_STATE_GPS_CHECK
      case RF_GPS_BAND:
        rf_chain_ptr->rf_mode = DB_RF_GPS;
        break;
      #endif
      
      default:
        RF_FATAL_ERROR("Invalid RF mode.");
      }

      rf_hw_lna_setup_digital(device);

      rf_update_lna_offsets(device);

      /* --------------------------------------------------
      ** Initialize the RF card to the proper band whether
      ** or not we are coming from sleep or a band change
      ** -------------------------------------------------- */
      (void) rfc_init_card( device, rf_chain_ptr->rf_curr_cdma_band );
      
      /* ------------------------------------------
      ** Enable rx pll in rft
      ** ----------------------------------------- */
      synth_rfrx_pll_enable( device, rf_chain_ptr->rf_mode );

      /* ------------------------------------------
      ** Enable PLL buffer in rfr
      ** ----------------------------------------- */
      rfrx_pll_buffer_enable( device );  
      
      rfc_tune_synth(device, rf_chain_ptr->rf_chan );

#ifdef RF_HAS_IM2
      /* Load the DACC values */
#ifdef RF_HAS_POWER_MODE_LIB
      (void)rfr_I_Q_dac_set (path,
                     rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_i_value[path],
                     rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_q_value[path],
                     rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_transconductor_value[path]);
#else
      (void)rfr_I_Q_dac_set (path,
                     &rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_i_value[path],
                     &rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_q_value[path],
                     &rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_transconductor_value[path]);
#endif /* RF_HAS_POWER_MODE_LIB */
#endif /* RF_HAS_IM2 */

      if ( chain1_is_sleeping )
      {
        /* -----------------------------------------------------------
        ** We are comming from sleep.
        ** ----------------------------------------------------------- */
        rf_cdma_sleep_wakeup(device);

      }
      else
      {
        /* Wake up rfr */
        rfr_sleep_disable( path );

        /* -----------------------------------------------------------
        ** We came form another mode.  Setup RX Front for new mode.
        ** ----------------------------------------------------------- */
        rxf_set_mode_cdma(path, rf_chain_ptr->rf_curr_cdma_band);
      }

      break;

  default:
      RF_FATAL_ERROR("Invalid device id.");
  }

  #if (!defined T_FFA) && defined (RF_TIME_PROFILING)
#error code not present
  #endif

} /* rf_init_digital */


#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */


/*===========================================================================

FUNCTION RF_CDMA_START_DC_AGC_ACQ

DESCRIPTION
   This function enables the DC and AGC loops.  RF warmup time for this is
   accounted for in the function. 

DEPENDENCIES
   Must have previously called rf_cdma_sleep_wakeup_prep().  
   The real time counter (rtc) must be running for rxf blocks dc and agc loops 
   to be active.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rf_cdma_start_dc_agc_acq( rfcom_device_enum_type device )
{
    rf_path_enum_type path = rfcom_to_path[ device ];

#ifdef RF_DEBUG
#error code not present
#endif /* RF_DEBUG */

    /* 
    * Set AGC loop to acq mode.
    * Start DC convergence and Rx AGC acqusition 
    */
    rf_wu_start_acq(device, path);

    /* Wait for Rx AGC to settle */
    clk_busy_wait(RF_RX_AGC_SETTLING_US);

    /* Set RxAGC tracking loop and IM */
    rf_wu_set_agc_track(device, path);

} /* rf_cdma_start_dc_agc_acq */


/*===========================================================================

FUNCTION RF_CDMA_SLEEP_WAKEUP_PREP

DESCRIPTION
   This function prepares for CDMA sleep wakeup by initialization and configuring 
   the rxagc, rxf and rfr.  This function is called during sleep wakeup after the 
   synthesizers have been tuned in rf_init_digital_band_chan().

   The enabling of the DC and AGC loops can only be done after the real time counter 
   (rtc) has been re-enabled by the chipx8 wakeup interrupt.


DEPENDENCIES
   Must be called from the context of rf_init_digital_band_chan()

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rf_cdma_sleep_wakeup_prep( rfcom_device_enum_type device )
{
   #ifdef RF_HAS_WU_TIMED_DEBUG
#error code not present
   #endif /* RF_HAS_WU_TIMED_DEBUG */

   rf_path_enum_type path = rfcom_to_path[ device ];

   if (path >= RF_PATH_MAX)
   {
      MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
      rf_error_flag = TRUE;
      return;
   }

   /* Set AGC to default state for CDMA */
   rflib_set_agc_def_state( path );

   /* Force LNA decision to lowest implemented gain state */
   rf_set_lna_decision_to_low_gain_state(device);

   /* Disable the IM switching controls in the LNA state machine */
   rf_disable_im_level_ctls(device);

   /* Freeze AGC VALUE */
   MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_LNA_CTL), AGC_VALUE_OVERRIDE, CAGC_RF_AGC_FRZ_V, (int)(path));

   /* Override AGC VALUE with value corresponding to lowest implemented gain state */
   rf_set_agc_val_to_low_gain(device);

   /* Wake up rxf */
   rxf_cdma_wakeup(path);

   /* Wake up rfr */
   rfr_sleep_disable( path );

   #ifdef RF_HAS_WU_TIMED_DEBUG
#error code not present
   #endif /* RF_HAS_WU_TIMED_DEBUG */

} /* rf_cdma_sleep_wakeup_prep */


/*===========================================================================

FUNCTION RF_CDMA_SLEEP_WAKEUP

DESCRIPTION
   This function completes the CDMA initialization after the synthesizers
   have been tuned..


DEPENDENCIES
   Must be called after the synthesizers are tuned and the real time counter
   is on (i.e. not in sleep and chipx8 is running).
   
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rf_cdma_sleep_wakeup( rfcom_device_enum_type device )
{

    /* enable the rfr and synthesizers */
    rf_cdma_sleep_wakeup_prep ( device );

    /* enable the dc and agc loops */
    rf_cdma_start_dc_agc_acq ( device );

} /* rf_cdma_sleep_wakeup */


/*===========================================================================

FUNCTION RF_COMPLETE_WARMUP                             EXTERNALIZED FUNCTION

DESCRIPTION
  This function completes warm up the RF circuitry.  PDM I/Q DC offset
  and TRK_LO_ADJ loops should have settled to their pre-sleep
  values.  When switched back to closed loop CDMA mode, there should
  not be a large transition.

DEPENDENCIES
  rf_init_digital() or rf_init_digital_band_chan() must have been called earlier.
  CDMA mode enabled and the synthesizers should be in lock.
  called after wakeup when the real time counter is running.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rf_complete_warmup( void )
{
#if !defined (RF_HAS_WU_OPTIMIZED) && !defined(RF_HAS_TEMP_BASED_DEVICE_CONFIG)
/* Enable LNA control */
rf_enable_cdma_lna_temp_comp();
#endif

  rf_dig_init_complete = TRUE;

  MSG_LOW("RF warmup complete", 0, 0, 0);

} /* rf_complete_warmup */


/*===========================================================================

FUNCTION RF_COMPLETE_SLEEP_WARMUP                           EXTERNAL FUNCTION

DESCRIPTION
  This function completes warm up the RF circuitry after sleep wakeup.
  This is used when there is a requirement to separate the enabling of
  the dc and rxagc acq loops to after when the rtc is running (i.e. cx8 
  wakeup int).  This is necessary because the rxf/rxagc block uses the
  rtc for its internal timings.
  
  Typically called from srch as last RF call during wakeup processing.
  
DEPENDENCIES
  Must be called after rf_init_digital().
  The real time counter must be running.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rf_complete_sleep_warmup( rfcom_device_enum_type device )
{
  rf_path_enum_type path = rfcom_to_path[ device ];

  #if !defined (RF_HAS_NO_RTC_DEPENDENCY) || defined (RF_HAS_WU_TIMED)
  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];
  #endif /* ! RF_HAS_NO_RTC_DEPENDENCY */

  #ifdef RF_HAS_WU_TIMED
  /* Variables for Power reading measurements */
  rf_cagc_vals_vars_type agc_val;
  #ifdef RF_HAS_WU_TIMED_DEBUG
#error code not present
  #endif
  int32 rx_pwr;
  (void)rf_prof_timer_start(path);
  #endif /* RF_HAS_WU_TIMED */

  #ifdef RF_TIME_PROFILING
#error code not present
  #endif /* RF_TIME_PROFILING */

  #ifdef RF_HAS_WU_OPTIMIZED
#error code not present
  #endif /* RF_HAS_WU_OPTIMIZED */
    /* RTC is now running, finalize RXF setups */
    rf_wu_rxf_rtc_setups(path);
  
    /* Enable dc and agc loops for convergence */
    rf_cdma_start_dc_agc_acq ( device );

    #ifndef RF_HAS_NO_RTC_DEPENDENCY
#error code not present
    #endif /* ! RF_HAS_NO_RTC_DEPENDENCY */
  
    /* wait for RF AGC values to settle */
    clk_busy_wait ( RF_COMPLETE_SLEEP_WAIT );

  #ifdef RF_HAS_WU_OPTIMIZED
#error code not present
  #endif /* RF_HAS_WU_OPTIMIZED */

  #ifdef RF_TIME_PROFILING
#error code not present
  #endif /* RF_TIME_PROFILING */

  #ifdef RF_HAS_WU_TIMED  
  #ifdef RF_HAS_WU_TIMED_DEBUG
#error code not present
  #else
  (void)rf_prof_timer_stop(path);
  #endif /* RF_HAS_WU_TIMED_DEBUG */
  rf_prof_warmup_done (path);


  /* These messages to outputted for RF WU monitoring purposes */
  rf_get_path_cagc_val( device, RX_AGC_VAL, &agc_val );
  rx_pwr = agc_val.val_min + ((int4)(agc_val.polarity * agc_val.scale_factor_numer * 
                               agc_val.val_index * 100 ))/agc_val.scale_factor_denom;

  MSG_7( MSG_SSID_RF, MSG_LEGACY_HIGH,
         "RF WU[%d](us) %d avg=%d cnt=%d Rx=%d/100 dBm chan=%d, lna=%d", 
         path,
         rf_chain_ptr->rf_wu_ptr->rf_profile_wu_last,
         rf_chain_ptr->rf_wu_ptr->rf_profile_wu_average,
         rf_chain_ptr->rf_wu_ptr->wu_count,
         rx_pwr,
         rf_chain_ptr->rf_chan,
         HWIO_INI( RXF_CDMA(RX_AGCc_LNA_RANGE_RD), path));
  #endif /* RF_HAS_WU_TIMED */

} /* rf_complete_sleep_warmup */


/*===========================================================================

FUNCTION RF_ENABLE_TX_PWR                               EXTERNALIZED FUNCTION

DESCRIPTION
  This function turns on the RF Tx hardware (exit IDLE mode).  Once
  rf_enable_tx_pwr() has been called, a delay of RF_ENABLE_PWR_WAIT ms or
  RF_ENABLE_PWR_WAIT_US us must be performed by the application to allow the
  RF hardware to settle.  After the RF hardware has settled, only then should
  the rf_tx_enable() function be called.

DEPENDENCIES
  rf_state must be setup
  rf_chipx8_needed must be setup

RETURN VALUE
  None

SIDE EFFECTS
  Updates rf_state = RF_STATE_FM_RXTX or RF_STATE_CDMA_RXTX

===========================================================================*/
void rf_enable_tx_pwr( rfcom_device_enum_type device )
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  rf_path_enum_type path = rfcom_to_path[ device ];
  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];

  #ifdef RF_HAS_VARIABLE_SMPS_SWITCHER_SIZE
  pm_err_flag_type pm_level_error = PM_ERR_FLAG__SUCCESS;
  #endif

    /* -----------------------------------
  ** Turn on power to the RF Tx hardware
  ** ----------------------------------- */
  rfc_manage_vregs(RFC_TX_VREGS, PM_ON_CMD, path );


  #ifdef RF_HAS_VARIABLE_SMPS_SWITCHER_SIZE   
  pm_level_error |= pm_vreg_smps_switch_size_set(PM_VREG_SMPS_S1_ID,PM_SMPS_SWITCH_SIZE_1_1);

  pm_level_error |= pm_vreg_smps_switch_size_set(PM_VREG_SMPS_S2_ID,PM_SMPS_SWITCH_SIZE_1_1);

  pm_level_error |= pm_vreg_smps_pulse_skipping_enable(PM_VREG_SMPS_S1_ID,FALSE);

  pm_level_error |= pm_vreg_smps_pulse_skipping_enable(PM_VREG_SMPS_S2_ID,FALSE);

  if(pm_level_error != PM_ERR_FLAG__SUCCESS)
  {
    MSG_ERROR("Pmic SMPS Switcher Size is not set properly for Tx ON",0,0,0);
  }

  switch(rf_chain_ptr->rf_curr_cdma_band)
  {
  case RF_BC0_BAND:
  case RF_BC10_BAND:
  case RF_BC5_BAND:
  case RF_BC11_BAND:   
  case RF_BC6_BAND:       
    pm_level_error |= pm_vreg_smps_individual_clock_div_sel( PM_VREG_SMPS_S1_ID, PM_VREG_SMPS_TCXO_DIV__24);
    pm_level_error |= pm_vreg_smps_individual_clock_div_sel( PM_VREG_SMPS_S2_ID, PM_VREG_SMPS_TCXO_DIV__24);
    break;

  case RF_BC1_BAND:
      pm_level_error |= pm_vreg_smps_individual_clock_div_sel( PM_VREG_SMPS_S1_ID, PM_VREG_SMPS_TCXO_DIV__18);
      pm_level_error |= pm_vreg_smps_individual_clock_div_sel( PM_VREG_SMPS_S2_ID, PM_VREG_SMPS_TCXO_DIV__18);      
      break;
  }
  
  if(pm_level_error != PM_ERR_FLAG__SUCCESS)
  {
    MSG_ERROR("Pmic SMPS Frequency is not set properly for Tx ON",0,0,0);
  }
  #endif/*RF_HAS_VARIABLE_SMPS_SWITCHER_SIZE */


  clk_regime_misc_disable( CLK_RGM_MISC_ENA_TXDAC_SLEEP );  /* Turn on the TxDACs */

  /* --------------------------------------------------------------------
  ** Make hardware and state changes based on previous operating RF state
  ** -------------------------------------------------------------------- */
  switch (rf_chain_ptr->rf_state) {

  case RF_STATE_CDMA_RX:

      rfr_idle_disable( path );

#ifdef FEATURE_TRIMODE_ITEMS
      clk_busy_wait( RF_DISABLE_IDLE_WAIT );
#endif /* FEATURE_TRIMODE_ITEMS */

      /* -------------
      ** Set new state
      ** ------------- */

      /* take rft out of idle state (RX) and into RXTX state */
      rft_idle_disable( path );
      
     #ifdef RF_HAS_SUPPORT_FOR_RICO_BRINGUP
#error code not present
     #endif/*RF_HAS_SUPPORT_FOR_RICO_BRINGUP*/

      rf_tx_status = RF_TX_IS_ON;      /* We can start adjusting Tx power*/

      rf_chain_ptr->rf_prev_state = rf_chain_ptr->rf_state;
      rf_chain_ptr->rf_state = RF_STATE_CDMA_RXTX;

#ifdef RF_TEST_CHAIN_1
#error code not present
#endif

      break;

    case RF_STATE_CDMA_RXTX:

      /* ------------------------------------------
      ** Tx power is already enabled, nothing to do
      ** ------------------------------------------ */
      break;

    case RF_STATE_SLEEP_1:
    case RF_STATE_SLEEP_2:
    #ifndef RF_HAS_BYPASS_RF_STATE_GPS_CHECK
    case RF_STATE_GPS:
    #endif
    default:
      RF_FATAL_ERROR("Invalid RF state.");

  }


#if defined (FEATURE_BATTERY_CHARGER)
  chg_tx_enabled();
#endif

  MSG_LOW("Tx Power enabled", 0, 0, 0);

} /* rf_enable_tx_pwr */


/*===========================================================================

FUNCTION RF_TX_ENABLE                                   EXTERNALIZED FUNCTION

DESCRIPTION
  If CDMA mode: initialize the TX_GAIN_COMP and TX_AGC_ADJ outputs; setup
  the CDMA AGC hardware to take control the RF Tx hardware; load the CDMA AGC
  linearizers; and start up the Tx power limit adjuster by scheduling
  a call back to the rf_enable_125_ms_isr() function delayed by
  RF_TX_LIM_ADJ_REPEAT_TIME ms.

  If FM mode: setup the digital FM hardware to take control of the RF
  hardware.  In FM mode TX_GAIN_COMP and TX_AGC_ADJ are set up in rf_pa_on().

  Once rf_tx_enable has been called, a delay of RF_TUNE_CHAN_WAIT ms or
  RF_TUNE_CHAN_WAIT_US us must be performed by the application to allow the
  VHF Tx LO synthesizer in the BB2 to settle.  After the VHF Tx LO Tx
  synthesizer has settled, only then should the PA be enabled.

DEPENDENCIES
  rf_cdma_rx_lin_off[], rf_cdma_tx_lin_vs_temp_now, rf_r1_rise, rf_r1_fall,
    rf_r2_rise, rf_r2_fall, rf_r3_rise, rf_r3_fall,
    rf_cdma_tx_lin_master_off[], rf_cdma_tx_lin_master_slp[],
    rf_cdma_tx_lim_vs_temp_now, rf_cdma_tx_comp_vs_freq[],
    rf_pa_range_step_cal, and rf_config must have been loaded from NV storage
  Procedure rf_enable_tx_pwr() must have been called before calling
    this procedure and a delay of RF_ENABLE_PWR_WAIT ms allowed
  rf_state must be setup = RF_STATE_CDMA_RXTX or RF_STATE_FM_RXTX appropriately
  Database item DB_RF_MODE must be setup
  rf_freq_index must be setup
  rf_pwr_call_back must be setup

RETURN VALUE
  None

SIDE EFFECTS
  If CDMA mode is selected:
    Updates temperature data, rf_tx_pwr_limit, and rf_pwr_call_back

===========================================================================*/
void rf_tx_enable( rfcom_device_enum_type device )
{
  int2 tx_pwr_limit;             /* power limit*/
  rf_path_enum_type path = rfcom_to_path[ device ];

  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];

  if (path >= RF_PATH_MAX)
  {
     MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
     rf_error_flag = TRUE;
     return;
  }

#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */ 

  #ifdef RF_HAS_SUPPORT_FOR_RICO_BRINGUP
#error code not present
  #endif

  /* program the TX PLLs in the RFT */
  synth_rftx_pll_load( device, rf_chain_ptr->rf_curr_cdma_band,rf_chain_ptr->rf_mode, rf_chain_ptr->rf_chan );

  #ifdef RF_HAS_SUPPORT_FOR_RICO_BRINGUP
#error code not present
  #endif/*RF_HAS_SUPPORT_FOR_RICO_BRINGUP*/

  /* TX_ON output is determined by TX_ON circuit */
#ifdef RF_HAS_HDR
#error code not present
#endif
  {
    /* Enable TX_ON circuit. */
#ifndef T_IO_CARD_PBS
    // dlb': This causes problems when trying to capture reverse link data
    // with PBS because it triggers PBS to start trying to capture the data
    // before the data is actually being sent.
     //This is being done so that Tx_ON_EN is managed by Encoder
     //Software together with PA_ON_EN so as to eliminate the
     //spike at beginning of Access probe
     #ifndef RF_TX_ON_EN_MANAGED_BY_ENC          
     MSM_W_MASKED(RXF_MODEM(PA_ON_CTL), TX_ON_EN, CAGC_RF_TX_ON_ENA_ON_V);
     #endif
#endif /* T_IO_CARD_PBS */
  }

  /* ----------------------------------------
  ** Figure out what mode we are currently in
  ** ---------------------------------------- */
  switch (rf_chain_ptr->rf_mode) {

    case DB_RF_CDMA:
    case DB_RF_PCN:

      /* ---------------------------------------------------------------
      ** Generate error if DB_RF_MODE does not match up with proper band
      ** --------------------------------------------------------------- */
      if (!(
             (rf_chain_ptr->rf_curr_cdma_band == RF_BC0_BAND) || /*   US Cellular    */
             (rf_chain_ptr->rf_curr_cdma_band == RF_BC1_BAND) || /*   US PCS         */
             (rf_chain_ptr->rf_curr_cdma_band == RF_BC3_BAND) || /*   JCDMA          */
             (rf_chain_ptr->rf_curr_cdma_band == RF_BC4_BAND) || /*   Korean PCS     */
             (rf_chain_ptr->rf_curr_cdma_band == RF_BC5_BAND) || /*   NMT            */
             (rf_chain_ptr->rf_curr_cdma_band == RF_BC10_BAND) || /*   NMT            */
             (rf_chain_ptr->rf_curr_cdma_band == RF_BC11_BAND)|| /*   BC11           */
             (rf_chain_ptr->rf_curr_cdma_band == RF_BC14_BAND)|| /*   US PCS + 5MHz. */
             (rf_chain_ptr->rf_curr_cdma_band == RF_BC15_BAND)|| /* AWS Band */
             (rf_chain_ptr->rf_curr_cdma_band == RF_BC6_BAND)    /*   IMT (2.1GHz)   */
               )) 
      {

        RF_FATAL_ERROR("Invalid RF band.");
      }

      /* -----------------------------------------
      ** Generate error if not in correct rf_state
      ** ----------------------------------------- */
      if (rf_chain_ptr->rf_state != RF_STATE_CDMA_RXTX) 
      {
        RF_FATAL_ERROR("Invalid RF state.");
      }

#if defined(RF_HAS_BC_SPECIFIC_TX_PA_MAP_VALUE)
       /* Assign any bandclass specific PA_R_MAP values */
       rf_update_tx_pa_map_with_bc_specific_value(device);          
#endif /* RF_HAS_BC_SPECIFIC_TX_PA_MAP_VALUE */

#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */
      {
        /* enable CAGC Tx AGC PDM output
        ** set RF Tx gain adj step size to 1 dB
        */   
             
#ifdef RF_HAS_MDSP_TX_AGC

        /* Changing the PA control back to MDSP */
        MSM_W_MASKED( MOD_PA_ON_CTL, PA_RANGE_SEL, 1);
        
        CDMA_MDSP_WR_TX_GAIN_ADJ_STEP_SIZE( CAGC_RF_TX_STEP_FULL_DB_V );
        #ifdef RF_HAS_HW_QSC11X0
        CDMA_MDSP_SET_TX_AGC_UPDATE_L2H(0xF6);
        #endif
        #ifdef RF_HAS_TX_DAC_BACKOFF_DIFF_FROM_LEGACY
        CDMA_MDSP_SET_TX_DAC_BACKOFF_VAL(CAGC_RF_TX_DAC_BACKOFF_1_DB_LESS_FROM_LEGACY);
        rf_set_pa_offsets(device);
        #endif

#ifdef RF_HAS_QSC60X5
#error code not present
  #endif  /* RF_HAS_QSC60X5 */
  #ifdef RF_HAS_QSC11X0

        #ifdef RF_HAS_BETA_SCALING_TX_AGC
        CDMA_MDSP_ENABLE_TX_BETAP();
        CDMA_MDSP_SET_TX_BETAP_NORMAL();
        #endif /* RF_HAS_BETA_SCALING_TX_AGC */
        
        #ifdef RF_HAS_PA_ACCESS_PROBE_CONFIG
        if (rf_pa_config_is_traffic_state == TRUE)
        {                                        
          CDMA_MDSP_SET_PA_HYSTERISIS_TIMER_VAL(rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_1x_pa_r_timer);
          CDMA_MDSP_ENABLE_PA_HYSTERISIS_TIMER();          
        }
        else
        {
          CDMA_MDSP_DISABLE_PA_HYSTERISIS_TIMER();          
          CDMA_MDSP_SET_PA_HYSTERISIS_TIMER_VAL(0);
        }
        #else
          CDMA_MDSP_SET_PA_HYSTERISIS_TIMER_VAL(rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_1x_pa_r_timer); 
          CDMA_MDSP_ENABLE_PA_HYSTERISIS_TIMER();
        #endif

        #if defined(RF_HAS_BC_SPECIFIC_TX_PA_MAP_VALUE)
          rf_update_tx_pa_map_with_bc_specific_value(device);  
        #endif /* RF_HAS_BC_SPECIFIC_TX_PA_MAP_VALUE */

  #endif  /* RF_HAS_QSC11X0 */

#else       

        MSM_W_MASKED(TX_AGC_CTL, TX_GAIN_ADJ_STEP_SIZE, CAGC_RF_TX_STEP_FULL_DB_V);
#endif /* RF_HAS_MDSP_TX_AGC */

        /* enable Tx AGC PDM control from CAGC hardware (closed loop) */
#ifdef RF_HAS_MODULATOR_TX_CTL
        MSM_W_MASKED(MOD_TX_AGC_ADJ_CTL, TX_AGC_EN_OVERRIDE, 0x00);

#ifdef RF_HAS_MDSP_TX_AGC
        MSM_W_MASKED(TX_AGC_CTL, PA_R_OVERRIDE_N, CAGC_RF_PAR_CTL_NONOVERRIDE_AGC_V);
        MSM_W_MASKED(MOD_TX_AGC_ADJ_CTL, TX_AGC_ADJ_SEL, 2);//Select the PDM Source to be Firmware

       /* Disable TX AGC ADJ OVERRIDE (if enabled via ftm_cdma_set_tx_agc()) */
       CDMA_MDSP_SET_TX_AGC_ADJ_NORMAL();

#else
        MSM_W_MASKED(MOD_TX_AGC_ADJ_CTL, TX_AGC_ADJ_SEL, 0x00);
#endif /* RF_HAS_MDSP_TX_AGC */

        MSM_W_MASKED(MOD_TX_AGC_ADJ_CTL, TX_AGC_ADJ_OE, 1);
#else
        MSM_W_MASKED(TX_AGC_CTL, TX_AGC_ADJ_OVERRIDE_N, CAGC_RF_TX_AGC_ADJ_NON_OVERRIDE_V);
        MSM_W_MASKED(TX_AGC_CTL, TX_AGC_ADJ_OE, 1);
#endif /* RF_HAS_MODULATOR_TX_CTL */

        rf_enable_pa_ctl(device);

        /* ------------------------------------
        ** Set initial value for Tx power limit
        ** ------------------------------------ */
        tx_pwr_limit =
           rf_cdma_tx_lim_vs_temp_now;

        rf_set_tx_pwr_limit( tx_pwr_limit );

        #ifdef RF_HAS_MDSP_TX_AGC
        /* Indicates to MDSP that Linearizer Parameters
        are latched to be picked up by MDSP at slot
        boundary*/
        CDMA_MDSP_NEW_TX_LIMIT_SLOT_PARAMS_AVAILABLE();        
        CDMA_MDSP_ENABLE_TX_FOR_1X();
        #endif

#ifdef FEATURE_TX_POWER_BACK_OFF
        /* reset the PA Backoff Module */
        rf_reset_pa_backoff_module( 0 );
#endif

#if !defined(RF_HAS_HDET_BYPASS)

        rf_exp_agc_table_ptr= rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_exp_agc_table;

        /* ----------------------------------------------------------------
        ** delay RF_TX_LIM_ADJ_REPEAT_TIME and then recalculate power limit
        ** ---------------------------------------------------------------- */
        clk_reg( &rf_pwr_call_back,         /* pointer to call back structure*/
                 rf_enable_125_ms_isr,      /* address of func to call back  */
                 RF_TX_LIM_ADJ_REPEAT_TIME, /* delay till call back          */
                 (int4)0,                   /* delay between repeat call
                                               backs                         */
                 RF_CALL_BACK_ONCE );       /* causes only one call back     */
#endif /* !RF_HAS_HDET_BYPASS */
        
#if defined (RF_HAS_WU_OPTIMIZED) || defined(RF_HAS_TEMP_BASED_DEVICE_CONFIG)
 /* Start temp comp for 1x */ 
 rf_enable_cdma_lna_temp_comp(); 
#endif

#if defined(RF_HAS_HDET_COMP_FOR_RC_AND_TEMP)
        /* We are starting a call, request that we refresh HDET temperature. */
        rf_req_update_hdet_temp_value();
#endif /* RF_HAS_HDET_COMP_FOR_RC_AND_TEMP */

#ifdef RFGPS_HAS_INTELLICEIVER
#error code not present
#endif /* RFGPS_HAS_INTELLICEIVER */

//#ifdef RF_HAS_SUPPORT_FOR_RICO_BRINGUP

       if(rf_debug_fixed_pdm_is_enabled == TRUE)
       {       
         if((rf_chain_status[0].rf_curr_cdma_band == RF_BC5_BAND)||(rf_chain_status[0].rf_curr_cdma_band == RF_BC11_BAND))
         {
           ftm_set_pa_r1_r0(rf_debug_pa_override_val_bc5_bc11);//0
           ftm_cdma_set_tx_agc(rf_debug_pdm_value_used_bc5_bc11);//160
         }
         else if (rf_chain_status[0].rf_curr_cdma_band == RF_BC0_BAND)
         {
           ftm_set_pa_r1_r0(rf_debug_pa_override_val_bc0);//0
           ftm_cdma_set_tx_agc(rf_debug_pdm_value_used_bc0);//160
         }
         else
         {           
           ftm_set_pa_r1_r0(rf_debug_pa_override_val_bc1);//0
           ftm_cdma_set_tx_agc(rf_debug_pdm_value_used_bc1);//160
         }
       }
//#endif/*RF_HAS_SUPPORT_FOR_RICO_BRINGUP*/
      
      }
      break;

    case DB_RF_GPS:
    case DB_RF_900MHZ:
    case DB_RF_SLEEP:
    default:
      RF_FATAL_ERROR("Invalid RF mode.");
  }

  MSG_LOW("Tx Enabled", 0, 0, 0);

} /* rf_tx_enable */



/*===========================================================================

FUNCTION RF_TX_DISABLE                                  EXTERNALIZED FUNCTION

DESCRIPTION
  This function turns off power to the RF Tx hardware and the VHF Tx LO
  synthesizer in the BB2.  In CDMA mode, it stops the Tx power limit adjuster
  by de-activating the call back function.

DEPENDENCIES
  rf_state must be setup
  rf_chipx8_needed must be setup
  rf_pwr_call_back must be setup

RETURN VALUE
  None

SIDE EFFECTS
  Updates rf_pwr_call_back
  Updates rf_state = RF_STATE_FM_RX or RF_STATE_CDMA_RX

===========================================================================*/
void rf_tx_disable( rfcom_device_enum_type device )
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  rf_path_enum_type path = rfcom_to_path[ device ];
  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];

  #ifdef RF_HAS_VARIABLE_SMPS_SWITCHER_SIZE
  pm_err_flag_type pm_level_error = PM_ERR_FLAG__SUCCESS;
  #endif

  if (path >= RF_PATH_MAX)
  {
     MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
     rf_error_flag = TRUE;
     return;
  }

#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */

   #ifdef RF_HAS_MDSP_TX_AGC
    CDMA_MDSP_DISABLE_TX_FOR_1X();
   #endif
   

   #ifdef RF_HAS_PA_ACCESS_PROBE_CONFIG
   rf_set_pa_access_probe_config(device,FALSE);
   #endif

  /* Moved from below rf_state switch because TX_AGC registers now require
     the CDMA_TX_CLK to be running when they are written */
  rf_disable_pa_ctl(device);

  /* Changing the PA control to HW */
  MSM_W_MASKED( MOD_PA_ON_CTL, PA_RANGE_SEL, 0);
  
  /* Set the PA in Override mode and set the PA R-map to value of 0 */
  MSM_W_MASKED( TX_AGC_CTL, PA_R_OVERRIDE_N, CAGC_RF_PAR_CTL_OVERRIDE_AGC_V);
  MSM_W_MASKED( TX_AGC_CTL, PA_R_WR, CAGC_RF_PAR_10_V ); 

 
  /* --------------------------------------------------------------------
  ** Make hardware and state changes based on previous operating RF state
  ** -------------------------------------------------------------------- */
  switch (rf_chain_ptr->rf_state) {

    case RF_STATE_CDMA_RXTX:

#if !defined(RF_HAS_HDET_BYPASS)
      /* -----------------------------------------
      ** Disable 1.25 ms ISR (HDET data collector)
      ** ----------------------------------------- */
      tramp_set_isr( TRAMP_125_MS_ISR, NULL );

      /* ----------------------------------
      ** stop the CDMA power limit adjuster
      ** ---------------------------------- */
      clk_dereg( &rf_pwr_call_back );
#endif /* !RF_HAS_HDET_BYPASS */

      /* ----------------------------------------------------------------
      ** Disable the TX_AGC_ADJ output so it won't drive powered down ICs
      ** ---------------------------------------------------------------- */
    #ifdef RF_HAS_MODULATOR_TX_CTL
      MSM_W_MASKED(MOD_TX_AGC_ADJ_CTL, TX_AGC_ADJ_OE, 0);
    #else
      MSM_W_MASKED( TX_AGC_CTL, TX_AGC_ADJ_OE, 0);
    #endif

    /* Disable temp comp */ 
   #ifdef RF_HAS_TEMP_BASED_DEVICE_CONFIG
   rf_disable_cdma_lna_temp_comp();
   #endif

      /* ----------------------------------------------------------------
      ** Put RF Chips into Idle (RX state)
      ** ---------------------------------------------------------------- */
      rfr_idle_enable( path );

      rft_idle_enable( path );

      /* ----------------
      ** Set new RF state
      ** ---------------- */
      rf_chain_ptr->rf_prev_state = rf_chain_ptr->rf_state;
      rf_chain_ptr->rf_state = RF_STATE_CDMA_RX;
      rf_tx_status = RF_TX_IS_OFF;  /* update rf_tx_status */

#ifdef RF_TEST_CHAIN_1
#error code not present
#endif

      break;

    case RF_STATE_CDMA_RX:
    case RF_STATE_SLEEP_1:
    case RF_STATE_SLEEP_2:
    #ifndef RF_HAS_BYPASS_RF_STATE_GPS_CHECK
    case RF_STATE_GPS:
    case RF_STATE_FM_RX:
    #endif
      /* ----------------------------------------------
      ** Tx hardware is already disabled, nothing to do
      ** ---------------------------------------------- */
      break;

    default:
      RF_FATAL_ERROR("Invalid RF state.");

  }

  clk_regime_misc_enable( CLK_RGM_MISC_ENA_TXDAC_SLEEP );  /* Turn off the TxDACs */

  /* turn off PA */
  MSM_W_MASKED( RXF_MODEM(PA_ON_CTL), PA_ON_EN, CAGC_RF_PA_ON_ENA_OFF_V);

  /* turn off TX_ON */
  MSM_W_MASKED( RXF_MODEM(PA_ON_CTL), TX_ON_EN, CAGC_RF_TX_ON_ENA_OFF_V);


  #ifdef RF_HAS_VARIABLE_SMPS_SWITCHER_SIZE 
  switch(rf_chain_ptr->rf_curr_cdma_band)
  {
  case RF_BC0_BAND:
  case RF_BC10_BAND:
  case RF_BC5_BAND:
  case RF_BC11_BAND:
  case RF_BC6_BAND:
    pm_level_error |= pm_vreg_smps_individual_clock_div_sel( PM_VREG_SMPS_S1_ID, PM_VREG_SMPS_TCXO_DIV__24);
    pm_level_error |= pm_vreg_smps_individual_clock_div_sel( PM_VREG_SMPS_S2_ID, PM_VREG_SMPS_TCXO_DIV__24);
    break;

  case RF_BC1_BAND:  
      pm_level_error |= pm_vreg_smps_individual_clock_div_sel( PM_VREG_SMPS_S1_ID, PM_VREG_SMPS_TCXO_DIV__24);
      pm_level_error |= pm_vreg_smps_individual_clock_div_sel( PM_VREG_SMPS_S2_ID, PM_VREG_SMPS_TCXO_DIV__24);
      break;
  }

  if(pm_level_error != PM_ERR_FLAG__SUCCESS)
  {
    MSG_ERROR("Pmic SMPS Frequency is not set properly for Tx OFF",0,0,0);
  }

  pm_level_error |= pm_vreg_smps_switch_size_set(PM_VREG_SMPS_S1_ID,PM_SMPS_SWITCH_SIZE_1_2);

  pm_level_error |= pm_vreg_smps_switch_size_set(PM_VREG_SMPS_S2_ID,PM_SMPS_SWITCH_SIZE_1_2);

  if(pm_level_error != PM_ERR_FLAG__SUCCESS)
  {
    MSG_ERROR("Pmic SMPS Switcher Size is not set properly for Tx OFF",0,0,0);
  }
  #endif/*RF_HAS_VARIABLE_SMPS_SWITCHER_SIZE */

  rfc_manage_vregs(RFC_TX_VREGS, PM_OFF_CMD, path );

  #if defined (FEATURE_BATTERY_CHARGER)
    chg_tx_disabled();
  #endif

#ifdef RFGPS_HAS_INTELLICEIVER
#error code not present
#endif /* RFGPS_HAS_INTELLICEIVER */

  MSG_LOW("Tx is Disabled", 0, 0, 0);

} /* rf_tx_disable */


/*===========================================================================

FUNCTION RF_SLEEP                                       EXTERNALIZED FUNCTION

DESCRIPTION
  This function sets the database item DB_RF_MODE to DB_RF_SLEEP.  Actual
  control of the SLEEP/ signal is done by rf_stay_asleep().  This function
  should be called when the sleep state is entered so the database will be
  updated.  The SLEEP/ signal control is not done here because SLEEP/ must
  remain high for one catnap until the MSM2P is done using chipx8.

DEPENDENCIES
  rf_state must be setup = RF_STATE_CDMA_RX
  rf_chipx8_needed must be setup

RETURN VALUE
  None

SIDE EFFECTS
  Updates rf_state = RF_STATE_SLEEP_1

===========================================================================*/
void rf_sleep_cdma1x( rfcom_device_enum_type device )
{
  db_items_value_type db_value;  /* temp for database item DB_RF_MODE      */

  rf_path_enum_type path = rfcom_to_path[ device ];
  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];

  if (path >= RF_PATH_MAX)
  {
     MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
     rf_error_flag = TRUE;
     return;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* -----------------------------------------
  ** Generate error if not in correct rf_state
  ** ----------------------------------------- */
  if (rf_chain_ptr->rf_state != RF_STATE_CDMA_RX)
  {
    RF_FATAL_ERROR("Invalid RF state.");
  }

   #ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO    
   #ifdef RF_HAS_ADC_SAMPLING_CLK_DEBUG
#error code not present
   #endif
   #endif
  /* ----------------------------------
  ** Update local and external database
  ** ---------------------------------- */
  rf_chain_ptr->rf_mode = DB_RF_SLEEP;
  rf_chain_ptr->rf_prev_state = rf_chain_ptr->rf_state;
  rf_chain_ptr->rf_state = RF_STATE_SLEEP_1;

#ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO
       if(rf_chain_ptr->rf_curr_cdma_band != RF_GPS_BAND)
       {
#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */
         {
           rf_adc_clk_status.mode[path] = CDMA_MODE;
         }

         if(path == RF_PATH_0)
         {
           if (rf_adc_clk_status.drx_div_flag == DIV_ON)
           {           
             RF_FATAL_ERROR("Diversity Flag should have been turned OFF when primary chain goes off to sleep");
           }
         }

         rf_1x_chain_switch_adc_clk_set_rxf(path,VCO_OFF,DIV_OFF,rf_adc_clk_status.mode[path]);         
        }
#endif /* RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO */

  if( device== RFCOM_TRANSCEIVER_0)
  {
      db_value.rf_mode = rf_chain_ptr->rf_mode;
       
      db_put( DB_RF_MODE, &db_value );
        
      /* ------------------------------------------
      ** Stop the periodic temperature compensation
      ** and stop the CDMA LNA control.
      ** ------------------------------------------ */
      rf_disable_cdma_lna_temp_comp();
  }

  /* disable the DACC in the RX Front */
  rxf_disable_dacc(path);
  
  /* freeze LNA gain state at lowest gain state */
  rf_set_lna_decision_to_low_gain_state(device);

  /* put rfr to sleep...LDO management is in rf_stay_asleep() */
  rfr_sleep_enable( path );

  /* disable rx pll in rft */
  synth_rfrx_pll_disable( device );

  /* -------------------------------------------------------------
  ** Put the bbrx to sleep
  ** ------------------------------------------------------------- */
  adie_bbrx_wakeup(path, ADIE_DISABLE );

  /* ----------------------------------------------------------
  ** Configure for CDMA mode operation (saves power)
  ** ---------------------------------------------------------- */
  if( device== RFCOM_TRANSCEIVER_0)
  {

    #ifndef RF_BYPASS_SYSTEM_CTL
    HWIO_OUTM (SYSTEM_MODE, RF_SYSTEM_MODE_CDMA_HDR_M, RF_SYSTEM_MODE_CDMA_V) 
    #endif
  }

  MSG_LOW("RF Transceiver 0 ready to sleep", 0, 0, 0);


} /* rf_sleep */



/*===========================================================================

FUNCTION RF_SET_STAY_ASLEEP                                 INTERNAL FUNCTION

DESCRIPTION
  This does the calls necessary to  shuts off the RF Rx circuits.

DEPENDENCIES
  rf_state must be setup = RF_STATE_SLEEP_1
  rf_chipx8_needed must be setup

RETURN VALUE
  None

SIDE EFFECTS
  Updates rf_state = RF_STATE_SLEEP_2

===========================================================================*/
/* 
* re: lint !e715
* The tcxo_pdm_on parameter is unused if SRCH_HAS_TRK_LO_CNTRL is defined.
* to be removed when feature has been mainlined 
*/

void rf_set_stay_asleep
( 
rfcom_device_enum_type device,
boolean tcxo_pdm_on          /*lint -esym(715, tcxo_pdm_on)  unused parameter */
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  rf_path_enum_type path = rfcom_to_path[device];
  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[path];

  /* exit with no error if already in RF_STATE_SLEEP_2
  */
  if (rf_chain_ptr->rf_state != RF_STATE_SLEEP_2) 
  {

    /* -----------------------------------------
    ** Generate error if not in correct rf_state
    ** ----------------------------------------- */
    if (rf_chain_ptr->rf_state != RF_STATE_SLEEP_1)
    {
      RF_FATAL_ERROR("Invalid RF state.");
    }

    /* Manage voltage regulators */
    if(device == RFCOM_RECEIVER_DIV)
    {
      rf_disable_rx_chip_power( path, RFC_RX_DIV_VREGS );
    }
    else
    {
      rf_disable_rx_chip_power( path, RFC_RX_VREGS );
    }

    #ifdef RF_HAS_PAD3_CURRENT_REDUCTION_CONTROL
#error code not present
    #endif 

    #ifdef RF_HAS_MSMA_CURRENT_REDUCTION_CONTROL
#error code not present
    #endif

    /* Disable Rx Front Clock */
    rxf_sleep(path);

    /* put RF MODE GPIOs to sleep settings */
    rfc_set_mode_park(path);

    /* Update local RF state variable */
    rf_chain_ptr->rf_prev_state = rf_chain_ptr->rf_state;
    rf_chain_ptr->rf_state = RF_STATE_SLEEP_2;

  }


  MSG_LOW("RF sleeping", 0, 0, 0);

} /* rf_set_stay_asleep */

/*===========================================================================

FUNCTION RF_STAY_ASLEEP                                 EXTERNALIZED FUNCTION

DESCRIPTION
  This function shuts off the RF Rx circuits.

DEPENDENCIES
  rf_state must be setup = RF_STATE_SLEEP_1
  rf_chipx8_needed must be setup

RETURN VALUE
  None

SIDE EFFECTS
  Updates rf_state = RF_STATE_SLEEP_2

===========================================================================*/
__inline void rf_stay_asleep_cdma1x( rfcom_device_enum_type device )
{

   rf_set_stay_asleep(device, FALSE );

} /* rf_stay_asleep */


/*===========================================================================

FUNCTION RF_1X_INACT                                    EXTERNALIZED FUNCTION

DESCRIPTION
  This function sets the 1X RF to park state.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  The rf_state is set to RF_STATE_SLEEP_2.

===========================================================================*/
void rf_1x_inact(rfcom_device_enum_type device)
{
    db_items_value_type db_value;  /* temp for database item DB_RF_MODE      */
    rf_path_enum_type path = rfcom_to_path[ device ];

    rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];

    switch ( device )
    {
    case RFCOM_TRANSCEIVER_0:

      /* ----------------------------------
      ** Update local and external database
      ** ---------------------------------- */
      rf_chain_ptr->rf_mode = DB_RF_SLEEP;
      db_value.rf_mode = rf_chain_ptr->rf_mode;
      db_put( DB_RF_MODE, &db_value );
      rf_chain_ptr->rf_prev_state = rf_chain_ptr->rf_state;
      rf_chain_ptr->rf_state = RF_STATE_SLEEP_2;

      /* ------------------------------------------
      ** Stop the periodic temperature compensation
      ** and stop the CDMA LNA control.
      ** ------------------------------------------ */
      rf_disable_cdma_lna_temp_comp();

      /* disable the DACC in the RX Front */
      rxf_disable_dacc(path);

      /* freeze LNA gain state at lowest gain state */
      rf_set_lna_decision_to_low_gain_state(device);

      /* put rfr to sleep */
      rfr_sleep_enable(path);

      /* disable rx pll in rft */
      synth_rfrx_pll_disable( device );

      /* -------------------------------------------------------------
      ** Put the bbrx to sleep
      ** ------------------------------------------------------------- */
      adie_bbrx_wakeup(path, ADIE_DISABLE );

      /* Manage voltage regulators */
      rf_disable_rx_chip_power( path, RFC_RX_VREGS );

      /* -----------------------------------------------------------------
      ** Disable clock regimes
      ** ----------------------------------------------------------------- */
      clk_regime_disable( CLK_RGM_RXFRONT_M );

      /* put RF MODE GPIOs to sleep settings */
      rfc_set_mode_park(path);

#if defined( T_MSM6000 )
#error code not present
#endif

      MSG_LOW("RF 1X inactive", 0, 0, 0);

      break;

    case RFCOM_RECEIVER_1:
      /* Manage voltage regulators */
      rf_disable_rx_chip_power( path, RFC_RX_VREGS );

      break;

    default:
        RF_FATAL_ERROR(" rf_sleep(): device not supported!!!.");

    }

} /* rf_sleep */

/*===========================================================================

FUNCTION RF_TUNE_TO_CHAN                                EXTERNALIZED FUNCTION

DESCRIPTION
  This function tunes the UHF Tx/Rx synthesizer to the specified channel.
  Once rf_tune_to_chan has been called, a delay of RF_TUNE_CHAN_WAIT ms must
  be performed to allow the synthesizer to settle.  The channel is interpreted
  according to the current operating mode.

DEPENDENCIES
  rf_config, rf_pa_range_step_cal, and rf_cdma_tx_comp_vs_freq[] must
  have been loaded from NV storage
  rf_state must be setup

RETURN VALUE
   None

SIDE EFFECTS
  Updates database item DB_CHANNEL to the new channel
  Updates rf_freq_index

===========================================================================*/
void rf_tune_to_chan_cdma1x
(
  rfcom_device_enum_type device,        /* RF Multimode device */
  word chan                             /* channel number to tune to */
)
{
  db_items_value_type db_value; /* temp for database item DB_CHANNEL */
  rf_path_enum_type path = rfcom_to_path[ device ];

  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];

  if (path >= RF_PATH_MAX)
  {
     MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
     rf_error_flag = TRUE;
     return;
  }

#ifdef RF_TIME_PROFILING
#error code not present
#endif

#ifdef FEATURE_TRIMODE_ITEMS
#ifdef RF_LOG_BAND_CHANGE
  log_band_class_chng_type *band_log_ptr; /*  band class info */

  band_log_struct_type  *bl_ptr;
#endif
#endif

#ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO    
#ifdef RF_HAS_ADC_SAMPLING_CLK_DEBUG
#error code not present
#endif
#endif
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  #ifdef RF_HAS_BYPASS_RF_STATE_GPS_CHECK
  if (rf_chain_ptr->rf_curr_cdma_band == RF_GPS_BAND)
  {
      RF_FATAL_ERROR("Can't tune to chan with RF_GPS_BAND");
      return;
  }
  #endif
  switch ( device )
  {
  case RFCOM_TRANSCEIVER_0:
    /* ------------------------------------------------------------
    ** Calculate new index for NV frequency tables based on channel
    ** ------------------------------------------------------------ */
    rf_chain_ptr->rf_freq_index = rf_freq_index_calc( chan,
                                                      rf_chain_ptr->rf_curr_cdma_band );

#ifdef RF_HAS_LINEAR_INTERPOLATION
    rf_update_freq_comp_items( path, chan);
#endif

    /* Calculate rf_hdet_off_adj and rf_hdet_spn_adj every time the
    ** channel changes.
    */
    rf_hdet_off_adj = rf_hdet_adj_calc( rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_hdet_off );

    #ifdef RF_HAS_HDET_UINT16
    rf_hdet_spn_adj =
        rf_hdet_adj_calc( (uint16)( rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_hdet_off
                              + rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_hdet_spn) ) - rf_hdet_off_adj;
    #else
    rf_hdet_spn_adj =
        rf_hdet_adj_calc( (byte)( rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_hdet_off
                              + rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_hdet_spn) ) - rf_hdet_off_adj;
    #endif

    /* --------------------------------------------
    ** Update external database with channel number
    ** -------------------------------------------- */
    rf_chain_ptr->rf_chan = chan;
    db_value.channel = rf_chain_ptr->rf_chan;
    db_put( DB_CHANNEL, &db_value );

    /* ----------------------------------------------------------
    ** Make hardware changes based on previous operating RF state
    ** ---------------------------------------------------------- */
#ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO
    if(rf_chain_ptr->rf_curr_cdma_band != RF_GPS_BAND)
    {
#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */
      {
        rf_adc_clk_status.mode[path] = CDMA_MODE;
      }

       rf_1x_chain_switch_adc_clk_set_rxf(path,VCO_WARMUP,DIV_OFF,rf_adc_clk_status.mode[path]);
      
     }
#endif /* RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO */

    /* Update RxFront DVGA Gain Offset to new frequency */
    rxf_set_vga_gain_offset(path, rf_chain_ptr->rf_freq_index);

    switch (rf_chain_ptr->rf_state)
    {
    case RF_STATE_CDMA_RXTX:
      /* fall through */
    case RF_STATE_CDMA_RX:

      /* Update LNA offsets according to new frequency */
      rf_update_lna_offsets(device);

      /* -----------------------------------------------------------
      ** Reload the linearizers, we've changed the frequency and
      ** we need to load linearizers compensated for this frequency.
      ** ----------------------------------------------------------- */
      rf_load_cagc_linearizers(device);

#ifdef RF_HAS_IM2
      /* Load the DACC values */
#ifdef RF_HAS_POWER_MODE_LIB
      (void)rfr_I_Q_dac_set (path,
                       rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_i_value[path],
                       rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_q_value[path],
                       rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_transconductor_value[path]);
#else
      (void)rfr_I_Q_dac_set (path,
                       &rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_i_value[path],
                       &rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_q_value[path],
                       &rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_transconductor_value[path]);
#endif /* RF_HAS_POWER_MODE_LIB */
#endif /* RF_HAS_IM2 */

      break;

    case RF_STATE_SLEEP_1:
    case RF_STATE_SLEEP_2:
    #ifndef RF_HAS_BYPASS_RF_STATE_GPS_CHECK
    case RF_STATE_GPS:
    #endif
    default:
      RF_FATAL_ERROR("Invalid RF state.");
    }

#ifndef RF_HAS_RXF_V2
#error code not present
#endif

    /* disable the bbrx while tuning to new channel */
    adie_bbrx_wakeup(path, ADIE_DISABLE );

    /* RFR must be awake before tuning to channel */
    rfr_sleep_disable( path );

    MSM_W_MASKED( RXF_MODEM(PA_ON_CTL), TX_ON_EN, CAGC_RF_TX_ON_ENA_OFF_V);

    /* Set RFT to RX mode */
    synth_rfrx_pll_enable( device, rf_chain_ptr->rf_mode );

    /* program TX PLLs in RFT */
    if (RF_HW_D450 == rf_card_in_system) 
    {
        synth_rftx_pll_load( device, rf_chain_ptr->rf_curr_cdma_band, 
                              rf_chain_ptr->rf_mode, rf_chain_ptr->rf_chan);
    }

    /* Tune the synthesizer to the requested channel */
    rfc_tune_synth(device, chan );

    #ifdef RF_HAS_SUPPORT_FOR_RICO_BRINGUP
#error code not present
    #endif/*RF_HAS_SUPPORT_FOR_RICO_BRINGUP*/
        

    /* Wait minimum synth lock time before waking up the rfr */
    rf_wait_for_synth_lock(device);

     /* Force LNA decision to lowest implemented gain state */
    rf_set_lna_decision_to_low_gain_state(device);// AGC state (it could be ITM + DC)

    /* Disable the IM switching controls in the LNA state machine */
    rf_disable_im_level_ctls(device);

    /* Freeze AGC VALUE */
    MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_LNA_CTL), AGC_VALUE_OVERRIDE, CAGC_RF_AGC_FRZ_V, (int)(path));

   /* Override AGC VALUE with value corresponding to lowest implemented gain state */
    rf_set_agc_val_to_low_gain(device); //AGC VAL=511

    #ifdef RF_HAS_SUPPORT_FOR_DC_OFFSET_CAL    
    rfr_initiate_dc_offset_cal(path);       
    #endif

#ifdef RF_HAS_SUPPORT_FOR_LO_PWC_CAL
    rfr_adjust_lo_pwc_dac_codes( path );
#endif //RF_HAS_SUPPORT_FOR_LO_PWC_CAL

    /* wake up bbrx */
    adie_bbrx_wakeup(path, ADIE_ENABLE );

    if (RF_HW_D450 == rf_card_in_system)
    {
        clk_busy_wait(400);
    }
#ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO
    if(rf_chain_ptr->rf_curr_cdma_band != RF_GPS_BAND)
    {
#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */
      {
        rf_adc_clk_status.mode[path] = CDMA_MODE;
      }


      rf_1x_chain_switch_adc_clk_set_rxf(path,VCO_ON,DIV_OFF,rf_adc_clk_status.mode[path]);
    }
#endif /* RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO */

#ifndef RF_HAS_RXF_V2
#error code not present
#endif

    /* enable the dc and agc loops */
    rf_cdma_start_dc_agc_acq ( device );
#ifndef T_IO_CARD_PBS
    // dlb': This causes problems when trying to capture reverse link data
    // with PBS because it triggers PBS to start trying to capture the data
    // before the data is actually being sent.
    MSM_W_MASKED( RXF_MODEM(PA_ON_CTL), TX_ON_EN, CAGC_RF_TX_ON_ENA_ON_V);
#endif /* T_IO_CARD_PBS */
    break;

  case RFCOM_RECEIVER_DIV:
    {

    rf_chain_state_type *transceiver_status_ptr =
      &rf_chain_status[ rfcom_to_path[ RFCOM_TRANSCEIVER_0 ] ];

    /* ------------------------------------------------------------
    ** Calculate new index for NV frequency tables based on channel
    ** ------------------------------------------------------------ */
    transceiver_status_ptr->rf_freq_index = rf_chain_ptr->rf_freq_index =
        rf_freq_index_calc( chan, transceiver_status_ptr->rf_curr_cdma_band );

#ifdef RF_HAS_LINEAR_INTERPOLATION
    /* interpolate Rx and Tx items */
    rf_update_freq_comp_items( RF_PATH_0, chan);
    rf_update_freq_comp_items( RF_PATH_1, chan);
#endif


    /* Calculate rf_hdet_off_adj and rf_hdet_spn_adj every time the
    ** channel changes.
    */
    rf_hdet_off_adj = rf_hdet_adj_calc( transceiver_status_ptr->rf_nv_dig_items_ptr->rfnv_hdet_off );

    #ifdef RF_HAS_HDET_UINT16
    rf_hdet_spn_adj =
        rf_hdet_adj_calc( (uint16)( rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_hdet_off
                              + rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_hdet_spn) ) - rf_hdet_off_adj;
    #else
    rf_hdet_spn_adj =
        rf_hdet_adj_calc( (byte)( rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_hdet_off
                              + rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_hdet_spn) ) - rf_hdet_off_adj;
    #endif


    /* --------------------------------------------
    ** Update external database with channel number
    ** -------------------------------------------- */
    transceiver_status_ptr->rf_chan = chan;
    db_value.channel = transceiver_status_ptr->rf_chan;
    db_put( DB_CHANNEL, &db_value );

    /* update  diversity chain status */
    rf_chain_ptr->rf_chan = chan; 

#ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO

    if(rf_chain_ptr->rf_curr_cdma_band != RF_GPS_BAND)
    {
#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */
      {
         rf_adc_clk_status.mode[0] = CDMA_MODE;
        rf_adc_clk_status.mode[1] = CDMA_MODE;
      }

      rf_1x_chain_switch_adc_clk_set_rxf(RF_PATH_0,VCO_WARMUP,DIV_ON,rf_adc_clk_status.mode[0]);
    }
#endif /* RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO */

    /* ----------------------------------------------------------
    ** Make hardware changes based on previous operating RF state
    ** ---------------------------------------------------------- */

    /* Update RxFront DVGA Gain Offset to new frequency */
    rxf_set_vga_gain_offset(RF_PATH_0, transceiver_status_ptr->rf_freq_index);
    rxf_set_vga_gain_offset(RF_PATH_1, transceiver_status_ptr->rf_freq_index);

    switch (transceiver_status_ptr->rf_state)
    {
    case RF_STATE_CDMA_RXTX:
      /* fall through */
    case RF_STATE_CDMA_RX:

      /* Update LNA offsets according to new frequency */
      rf_update_lna_offsets(RFCOM_TRANSCEIVER_0);
      rf_update_lna_offsets(RFCOM_RECEIVER_1);

      /* -----------------------------------------------------------
      ** Reload the linearizers, we've changed the frequency and
      ** we need to load linearizers compensated for this frequency.
      ** ----------------------------------------------------------- */
      rf_load_cagc_linearizers(RFCOM_TRANSCEIVER_0);

#ifdef RF_HAS_IM2
#ifdef RF_HAS_POWER_MODE_LIB
      /* Load the DACC values */
      (void)rfr_I_Q_dac_set (RF_PATH_0,
                       transceiver_status_ptr->rf_nv_dig_items_ptr->rfnv_im2_i_value[rfcom_to_path[RFCOM_TRANSCEIVER_0]],
                       transceiver_status_ptr->rf_nv_dig_items_ptr->rfnv_im2_q_value[rfcom_to_path[RFCOM_TRANSCEIVER_0]],
                       transceiver_status_ptr->rf_nv_dig_items_ptr->rfnv_im2_transconductor_value[rfcom_to_path[RFCOM_TRANSCEIVER_0]]);

      /* Load the DACC values */
      (void)rfr_I_Q_dac_set (RF_PATH_1,
                       rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_i_value[rfcom_to_path[RFCOM_RECEIVER_1]],
                       rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_q_value[rfcom_to_path[RFCOM_RECEIVER_1]],
                       rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_transconductor_value[rfcom_to_path[RFCOM_RECEIVER_1]]);
#else
      /* Load the DACC values */
      (void)rfr_I_Q_dac_set (RF_PATH_0,
                       &transceiver_status_ptr->rf_nv_dig_items_ptr->rfnv_im2_i_value[rfcom_to_path[RFCOM_TRANSCEIVER_0]],
                       &transceiver_status_ptr->rf_nv_dig_items_ptr->rfnv_im2_q_value[rfcom_to_path[RFCOM_TRANSCEIVER_0]],
                       &transceiver_status_ptr->rf_nv_dig_items_ptr->rfnv_im2_transconductor_value[rfcom_to_path[RFCOM_TRANSCEIVER_0]]);

      /* Load the DACC values */
      (void)rfr_I_Q_dac_set (RF_PATH_1,
                       &rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_i_value[rfcom_to_path[RFCOM_RECEIVER_1]],
                       &rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_q_value[rfcom_to_path[RFCOM_RECEIVER_1]],
                       &rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_transconductor_value[rfcom_to_path[RFCOM_RECEIVER_1]]);
#endif /* RF_HAS_POWER_MODE_LIB */
#endif /* RF_HAS_IM2 */

      break;

    case RF_STATE_SLEEP_1:
    case RF_STATE_SLEEP_2:
    #ifndef RF_HAS_BYPASS_RF_STATE_GPS_CHECK
    case RF_STATE_GPS:
    #endif
    default:
      RF_FATAL_ERROR("Invalid RF state.");
    }

    /* --------------------------------------------
    ** Chain 0
    ** -------------------------------------------- */

    /* disable both DACC while tuning the ch0 PLL */
    MSM_W_MASKED_i(RXF_CDMA(RXFc_DACC_CTL), MICRO_DACC_EN, RXF_RF_MICRO_DACC_DIS_V, (int)(RF_PATH_0));
    MSM_W_MASKED_i(RXF_CDMA(RXFc_DACC_CTL), MICRO_DACC_EN, RXF_RF_MICRO_DACC_DIS_V, (int)(RF_PATH_1));

    /* disable both BBRXs while tuning to new channel */
    adie_bbrx_wakeup(RF_PATH_0, ADIE_DISABLE );
    adie_bbrx_wakeup(RF_PATH_1, ADIE_DISABLE );

    /* RFR must be awake before tuning to channel */
    rfr_sleep_disable( RF_PATH_0 );

    MSM_W_MASKED( RXF_MODEM(PA_ON_CTL), TX_ON_EN, CAGC_RF_TX_ON_ENA_OFF_V);

    /* Enable RX PLL */
    synth_rfrx_pll_enable( RFCOM_TRANSCEIVER_0, transceiver_status_ptr->rf_mode );

    /* program TX PLLs in RFT */
    if (RF_HW_D450 == rf_card_in_system) 
    {
       synth_rftx_pll_load( RFCOM_TRANSCEIVER_0, transceiver_status_ptr->rf_curr_cdma_band, 
                            transceiver_status_ptr->rf_mode, transceiver_status_ptr->rf_chan);
    }

    /* Tune the synthesizer to the requested channel */
    rfc_tune_synth( RFCOM_TRANSCEIVER_0,  chan );

    /* Wait minimum synth lock time before waking up the rfr */
    rf_wait_for_synth_lock(RFCOM_TRANSCEIVER_0);

    /* wake up bbrx */
    adie_bbrx_wakeup(RF_PATH_0, ADIE_ENABLE );

    if (RF_HW_D450 == rf_card_in_system)
    {
       clk_busy_wait(400);
    }

#ifndef RF_HAS_RXF_V2
#error code not present
#endif

    MSM_W_MASKED( RXF_MODEM(PA_ON_CTL), TX_ON_EN, CAGC_RF_TX_ON_ENA_ON_V);

    /* --------------------------------------------
    ** Chain 1 (wake up)
    ** -------------------------------------------- */

    /* RFR must be awake before tuning to channel */
    rfr_sleep_disable( RF_PATH_1 );

    /* wake up bbrx */
    adie_bbrx_wakeup(RF_PATH_1, ADIE_ENABLE );

#ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO
    if(rf_chain_ptr->rf_curr_cdma_band != RF_GPS_BAND)
    {
#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */
      {
        rf_adc_clk_status.mode[0] = CDMA_MODE;
        rf_adc_clk_status.mode[1] = CDMA_MODE;
      }

      rf_1x_chain_switch_adc_clk_set_rxf(RF_PATH_0,VCO_ON,DIV_ON,rf_adc_clk_status.mode[0]);
    }
#endif /* RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO */


#ifndef RF_HAS_RXF_V2
#error code not present
#endif
    break;

    }

  case RFCOM_RECEIVER_1:

    /* ------------------------------------------------------------
    ** Calculate new index for NV frequency tables based on channel
    ** ------------------------------------------------------------ */
    rf_chain_ptr->rf_freq_index = rf_freq_index_calc( chan, rf_chain_ptr->rf_curr_cdma_band );

    /* ------------------------------------------------------------
    ** Update channel in status structure
    ** ------------------------------------------------------------ */
    rf_chain_ptr->rf_chan = chan;

#ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO

    if(rf_chain_ptr->rf_curr_cdma_band != RF_GPS_BAND)
    {
#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */
      {
        rf_adc_clk_status.mode[path] = CDMA_MODE;
      }

      rf_1x_chain_switch_adc_clk_set_rxf(path,VCO_WARMUP,DIV_OFF,rf_adc_clk_status.mode[path] );
    }
#endif /* RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO */

#ifdef RF_HAS_LINEAR_INTERPOLATION
    rf_update_freq_comp_items( path, chan);
#endif

    /* ----------------------------------------------------------
    ** Make hardware changes based on previous operating RF state
    ** ---------------------------------------------------------- */

    /* Update RxFront DVGA Gain Offset to new frequency */
    rxf_set_vga_gain_offset(RF_PATH_1, rf_chain_ptr->rf_freq_index);

    /* Update LNA offsets according to new frequency */
    rf_update_lna_offsets( RFCOM_RECEIVER_1 );

#ifdef RF_HAS_IM2
      /* Load the DACC values */
#ifdef RF_HAS_POWER_MODE_LIB
    (void)rfr_I_Q_dac_set (RF_PATH_1,
                       rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_i_value[rfcom_to_path[RFCOM_RECEIVER_1]],
                       rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_q_value[rfcom_to_path[RFCOM_RECEIVER_1]],
                       rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_transconductor_value[rfcom_to_path[RFCOM_RECEIVER_1]]);
#else
    (void)rfr_I_Q_dac_set (RF_PATH_1,
                       &rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_i_value[rfcom_to_path[RFCOM_RECEIVER_1]],
                       &rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_q_value[rfcom_to_path[RFCOM_RECEIVER_1]],
                       &rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_transconductor_value[rfcom_to_path[RFCOM_RECEIVER_1]]);
#endif /* RF_HAS_POWER_MODE_LIB */
#endif /* RF_HAS_IM2 */

    /* disable DACC while tuning the PLLs */
    MSM_W_MASKED_i(RXF_CDMA(RXFc_DACC_CTL), MICRO_DACC_EN, RXF_RF_MICRO_DACC_DIS_V, (int)(RF_PATH_1));

    /* disable the bbrx while tuning to new channel */
    adie_bbrx_wakeup(RF_PATH_1, ADIE_DISABLE );

    /* RFR must be awake before tuning to channel */
    rfr_sleep_disable( RF_PATH_1 );

    /* Set RFT to RX mode */
    synth_rfrx_pll_enable( RFCOM_RECEIVER_1, rf_chain_ptr->rf_mode );

    /* Tune the synthesizer to the requested channel */
    rfc_tune_synth(RFCOM_RECEIVER_1, chan );

    /* Wait minimum synth lock time before waking up the rfr */
    rf_wait_for_synth_lock(RFCOM_RECEIVER_1);

    /* wake up bbrx */
    adie_bbrx_wakeup(RF_PATH_1, ADIE_ENABLE );

#ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO
    if(rf_chain_ptr->rf_curr_cdma_band != RF_GPS_BAND)
    {
#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */
      {
        rf_adc_clk_status.mode[path] = CDMA_MODE;
      }

      rf_1x_chain_switch_adc_clk_set_rxf(path,VCO_ON,DIV_OFF,rf_adc_clk_status.mode[path]);
    }
#endif /* RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO */

#ifndef RF_HAS_RXF_V2
#error code not present
#endif

    break;

  default:
      RF_FATAL_ERROR(" rf_tune_to_chan(): device not supported!!!");

  }

#ifdef FEATURE_TRIMODE_ITEMS
    /* Fill up the log packet and send it */
#ifdef RF_LOG_BAND_CHANGE
  if ( (bl_ptr = (band_log_struct_type *)q_get ( &band_scan_buf_q )) != NULL )
  {

    band_log_ptr = &bl_ptr->entry;

    band_log_ptr->hdr.len = sizeof(log_band_class_chng_type);

    band_log_ptr->hdr.code = LOG_BAND_CHANGE_C;

    band_log_ptr->band_class = (byte)rf_curr_cdma_band;

    band_log_ptr->channel = chan;

    ts_get( (unsigned long *) band_log_ptr->hdr.ts );
  }
  log_put ( &bl_ptr->desc );

  bl_ptr= NULL;
#endif /* RF_LOG_BAND_CHANGE */
#endif /* FEATURE_TRIMODE_ITEMS */

#ifdef RF_TIME_PROFILING
#error code not present
#endif

} /* rf_tune_to_chan */


/*===========================================================================

FUNCTION RF_TX_START                                    EXTERNALIZED FUNCTION

DESCRIPTION
  This function re-enables the PA and Tx periodic power adjustments that were
  disabled by the function rf_tx_stop().  This function will update
  rf_tx_status to RF_TX_IS_ON.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void rf_tx_start(rfcom_device_enum_type device)
{
  int2 new_tx_pwr_limit;        /* power limit        */
  rf_path_enum_type path = rfcom_to_path[ device ];

  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];

  if (path >= RF_PATH_MAX)
  {
     MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
     rf_error_flag = TRUE;
     return;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch( rf_chain_ptr->rf_state ) {

  case RF_STATE_CDMA_RXTX:      /* For CDMA RxTx only */

     #ifdef RF_HAS_HW_QSC11X0
     HWIO_OUT(MOD_MICRO_CLK_CTL,0xF);
     #endif

      if ( !(rf_path_0.rftdev == QSC60X5_TX ||
             rf_path_0.rftdev == QSC11X0_TX ||
             rf_path_0.rftdev == RTR6500_TX ||
             rf_path_0.rftdev == RFT6170) )
      {
          /* program TX PLLs in RFT */
          synth_rftx_pll_load( device, rf_chain_ptr->rf_curr_cdma_band, 
                               rf_chain_ptr->rf_mode, rf_chain_ptr->rf_chan);
      }
      /* Make sure rft is not idle/sleeping */
      rft_idle_disable( path );


     #ifdef RF_HAS_SUPPORT_FOR_RICO_BRINGUP
#error code not present
     #endif/*RF_HAS_SUPPORT_FOR_RICO_BRINGUP*/

      /* Make sure RFR is in RXTX mode */
      rfr_idle_disable( path );

     #ifdef RF_HAS_SUPPORT_FOR_RICO_BRINGUP
#error code not present
     #endif/*RF_HAS_SUPPORT_FOR_RICO_BRINGUP*/



      if ( rf_path_0.rftdev == QSC60X5_TX ||
           rf_path_0.rftdev == QSC11X0_TX ||
           rf_path_0.rftdev == RTR6500_TX )
      {
          /* program TX PLLs in RFT */
          synth_rftx_pll_load( device, rf_chain_ptr->rf_curr_cdma_band, 
                               rf_chain_ptr->rf_mode, rf_chain_ptr->rf_chan);
      }

     #ifdef RF_HAS_SUPPORT_FOR_RICO_BRINGUP
#error code not present
     #endif /*RF_HAS_SUPPORT_FOR_RICO_BRINGUP*/

      /* Make sure we lock up the interrupts so that we don't race to turn
      ** it off/on
      */
        #ifdef RF_HAS_QSC60X5
#error code not present
        #endif
      
      #ifdef RF_HAS_QSC60X5
#error code not present
      #endif
      
      #ifdef RF_HAS_QSC11X0
      #ifdef RF_HAS_BETA_SCALING_TX_AGC
        CDMA_MDSP_ENABLE_TX_BETAP();
        CDMA_MDSP_SET_TX_BETAP_NORMAL();        
      #endif
      #endif
      

     #ifdef RF_HAS_MDSP_TX_AGC      
     #ifdef RF_HAS_HW_QSC11X0
     CDMA_MDSP_SET_TX_AGC_UPDATE_L2H(0xF6);
     #endif
     #ifdef RF_HAS_TX_DAC_BACKOFF_DIFF_FROM_LEGACY
     CDMA_MDSP_SET_TX_DAC_BACKOFF_VAL(CAGC_RF_TX_DAC_BACKOFF_1_DB_LESS_FROM_LEGACY);
     rf_set_pa_offsets(device);
     #endif

     #ifdef RF_HAS_QSC60X5
#error code not present
     #endif  /*RF_HAS_QSC60X5 */    
     #endif /* RF_HAS_MDSP_TX_AGC */
     
     #ifdef RF_HAS_MDSP_TX_AGC     
     #ifdef RF_HAS_QSC11X0
     
        #ifdef RF_HAS_PA_ACCESS_PROBE_CONFIG
        if (rf_pa_config_is_traffic_state == TRUE)
        {                                        
          CDMA_MDSP_SET_PA_HYSTERISIS_TIMER_VAL(rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_1x_pa_r_timer); 
          CDMA_MDSP_ENABLE_PA_HYSTERISIS_TIMER();
        }
        else
        {                  
          CDMA_MDSP_DISABLE_PA_HYSTERISIS_TIMER();          
          CDMA_MDSP_SET_PA_HYSTERISIS_TIMER_VAL(0);
        }
        #else
          CDMA_MDSP_SET_PA_HYSTERISIS_TIMER_VAL(rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_1x_pa_r_timer); 
          CDMA_MDSP_ENABLE_PA_HYSTERISIS_TIMER();
        #endif

     #endif  /*RF_HAS_QSC11X0 */    
     #endif /* RF_HAS_MDSP_TX_AGC */

     #if defined(RF_HAS_BC_SPECIFIC_TX_PA_MAP_VALUE)
       rf_update_tx_pa_map_with_bc_specific_value(device);
     #endif /* RF_HAS_BC_SPECIFIC_TX_PA_MAP_VALUE */

#ifdef RF_HAS_ENC_TX_ENABLE_CALL_AS_LEGACY
      MSM_LOCK();

      enc_tx_enable( TRUE );              /* Turn the PA on           */
      rf_tx_status = RF_TX_IS_ON;         /* Allow CDMA Tx pwr adjust */

      /* Un-freeze the power control loop */
      COMBINER_ENABLE_POWER_CONTROL();
      
      MSM_UNLOCK();
#endif
      /* Set the PA Rise/Fall thresholds and enable PA state machine
      */
      rf_enable_pa_ctl( device ); 

      #ifdef RF_HAS_MDSP_TX_AGC
      CDMA_MDSP_SET_TX_AGC_ADJ_NORMAL();
      #endif


      /* Update compensation values for temperature and band
      */
      rf_cdma_temp_update() ;

      /* Set the new Tx power limit
      */
      new_tx_pwr_limit =
          rf_cdma_tx_lim_vs_temp_now;

      rf_set_tx_pwr_limit( new_tx_pwr_limit );
      #ifdef RF_HAS_MDSP_TX_AGC
      /* Indicates to MDSP that TX Limit Parameters
       are latched to be picked up by MDSP at frame
       boundary*/
      CDMA_MDSP_NEW_TX_LIMIT_SLOT_PARAMS_AVAILABLE();
      #endif 
      
#ifdef FEATURE_TX_POWER_BACK_OFF
      /* reset the PA Backoff Module */
      rf_reset_pa_backoff_module( 0 );
#endif


#if !defined(RF_HAS_HDET_BYPASS)

      rf_exp_agc_table_ptr= rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_exp_agc_table;

      /* ----------------------------------------------------------------
      ** delay RF_TX_LIM_ADJ_REPEAT_TIME and then recalculate power limit
      ** ---------------------------------------------------------------- */
      clk_reg( &rf_pwr_call_back,        /* pointer to call back structure */
               rf_enable_125_ms_isr,     /* address of func to call back   */
               RF_TX_LIM_ADJ_REPEAT_TIME,/* delay till call back           */
               (int4)0,                  /* delay between repeat call backs*/
               RF_CALL_BACK_ONCE );      /* causes only one call back      */
#endif /* !RF_HAS_HDET_BYPASS */

      #ifdef RF_HAS_TEMP_BASED_DEVICE_CONFIG
      rf_enable_cdma_lna_temp_comp(); 
      #endif

      #ifdef RF_HAS_MDSP_TX_AGC
      CDMA_MDSP_ENABLE_TX_FOR_1X();
      #endif
#ifndef RF_HAS_ENC_TX_ENABLE_CALL_AS_LEGACY
      MSM_LOCK();
      enc_tx_enable( TRUE );              /* Turn the PA on           */
      rf_tx_status = RF_TX_IS_ON;         /* Allow CDMA Tx pwr adjust */

      /* Un-freeze the power control loop */
      COMBINER_ENABLE_POWER_CONTROL();
      MSM_UNLOCK();
#endif


//#ifdef RF_HAS_SUPPORT_FOR_RICO_BRINGUP
       if(rf_debug_fixed_pdm_is_enabled == TRUE)
       {      
         if((rf_chain_status[0].rf_curr_cdma_band == RF_BC5_BAND)||(rf_chain_status[0].rf_curr_cdma_band == RF_BC11_BAND))
         {
           ftm_set_pa_r1_r0(rf_debug_pa_override_val_bc5_bc11);//0
           ftm_cdma_set_tx_agc(rf_debug_pdm_value_used_bc5_bc11);//160
         }
         else if (rf_chain_status[0].rf_curr_cdma_band == RF_BC0_BAND)
         {
           ftm_set_pa_r1_r0(rf_debug_pa_override_val_bc0);//0
           ftm_cdma_set_tx_agc(rf_debug_pdm_value_used_bc0);//160
         }
         else
         {           
           ftm_set_pa_r1_r0(rf_debug_pa_override_val_bc1);//0
           ftm_cdma_set_tx_agc(rf_debug_pdm_value_used_bc1);//160
         }
       }
       
//#endif /* RF_HAS_SUPPORT_FOR_RICO_BRINGUP*/
      break;

    case RF_STATE_CDMA_RX:
    case RF_STATE_SLEEP_1:
    case RF_STATE_SLEEP_2:
    #ifndef RF_HAS_BYPASS_RF_STATE_GPS_CHECK
    case RF_STATE_GPS:
    case RF_STATE_FM_RXTX:
    case RF_STATE_FM_RX:
    #endif
    default:
      RF_FATAL_ERROR("Invalid RF state.");
  }

  MSG_MED("Tx re-started", 0, 0, 0);

} /* end of rf_tx_start */


/*===========================================================================

FUNCTION RF_TX_STOP                                     EXTERNALIZED FUNCTION

DESCRIPTION
  This function disables the PA and Tx periodic power adjustments while
  remaining in CDMA RxTx state.  This function will update rf_tx_status
  to RF_TX_IS_OFF.

DEPENDENCIES
  rf_tx_start() must be called before exiting CDMA RxTx state.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rf_tx_stop(rfcom_device_enum_type device)
{
  rf_path_enum_type path = rfcom_to_path[ device ];
  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];

  #ifdef RF_HAS_MDSP_TX_AGC 
  #ifdef RF_SUPPORTS_32_BIT_TX_AGC_INTERFACE
  uint32 rf_tx_agc_adj_32_bit = 0x0;
  uint16 rf_low_power_pdm_index = RF_MAX_PDM_VAL_FOR_TX_GAIN;
  #else
  int2 rf_low_power_pdm_index = RF_MAX_PDM_VAL_FOR_TX_GAIN;
  #endif
  #endif
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch( rf_chain_ptr->rf_state ) {

    case RF_STATE_CDMA_RXTX:      /* For CDMA RxTx only */

      /* Make sure we lock up the interrupts so that we don't race to turn
      ** it off/on
      */      
          
      #ifdef RF_HAS_MDSP_TX_AGC
      CDMA_MDSP_SET_PA_STATE(0);
      CDMA_MDSP_ENABLE_PA_STATE_OVERRIDE();
      
      CDMA_MDSP_SET_TX_BETAP_VAL (0x0);
      CDMA_MDSP_ENABLE_TX_BETAP_OVERRIDE();

      //Setting the PDM for Lowest Power
      #ifdef RF_SUPPORTS_32_BIT_TX_AGC_INTERFACE
      rf_tx_agc_adj_32_bit = rft_get_tx_agc_adj_32_bit(0,rf_low_power_pdm_index,rf_chain_status[0].rf_curr_cdma_band);
      CDMA_MDSP_SET_TX_AGC_ADJ_VAL(rf_tx_agc_adj_32_bit);
      #else
      rf_low_power_pdm_index = (int2)(((rf_low_power_pdm_index - 256) * 128)& 0xffff);
      CDMA_MDSP_SET_TX_AGC_ADJ_VAL(rf_low_power_pdm_index);
      #endif

      CDMA_MDSP_ENABLE_TX_AGC_ADJ_OVERRIDE( );   
      #endif

      MSM_LOCK();

      /* Freeze the power control loop */
      COMBINER_DISABLE_POWER_CONTROL();

      /* Turn off PA right now!! */
      MSM_W_MASKED( RXF_MODEM(PA_ON_CTL), PA_ON_EN, CAGC_RF_PA_ON_ENA_OFF_V);

      enc_tx_enable( FALSE );       /* Unmask transmit path                */
      rf_tx_status = RF_TX_IS_OFF;  /* No pwr adjust. Channel tuning is ok */
       
      #ifdef RF_HAS_MDSP_TX_AGC
//        CDMA_MDSP_DISABLE_TX_FOR_1X();
      #endif

      /* ----------------------------------------------------------------
      ** For QCT-1000 and QCT-6000, HDET circuitry currently cannot be
      ** trusted.  This is the work-around that fixes the problem by
      ** disabling the call backs.
      ** ---------------------------------------------------------------- */

#if !defined(RF_HAS_HDET_BYPASS)
      /* ----------------------------------
      ** stop the CDMA power limit adjuster
      ** ---------------------------------- */
      clk_dereg( &rf_pwr_call_back );
#endif /* !RF_HAS_HDET_BYPASS */

      MSM_UNLOCK();


      #ifdef RF_HAS_TEMP_BASED_DEVICE_CONFIG
      rf_disable_cdma_lna_temp_comp();
      #endif

      /* Put rft in idle/sleep */
      rft_idle_enable( path );

      /* Make sure RFR is in idle/RX only mode */
      rfr_idle_enable( path );

      break;

    case RF_STATE_CDMA_RX:
    case RF_STATE_SLEEP_1:
    case RF_STATE_SLEEP_2:
    #ifndef RF_HAS_BYPASS_RF_STATE_GPS_CHECK
    case RF_STATE_FM_RX:
    case RF_STATE_FM_RXTX:
    case RF_STATE_GPS:
    #endif
    default:
      RF_FATAL_ERROR("Invalid RF state.");
  }

  MSG_MED("Tx temporarily stopped", 0, 0, 0);

} /* end of rf_tx_stop */

/*===========================================================================

FUNCTION RF_TX_SHUTDOWN                                 EXTERNALIZED FUNCTION

DESCRIPTION
  This function transitions the rf chain state from RF_STATE_CDMA_RXTX state to 
  RF_STATE_CDMA_RX by shutting down the Tx.
  to RF_TX_IS_OFF.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rf_tx_shutdown(void)
{
  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ RF_PATH_0 ];

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(rf_chain_ptr->rf_state == RF_STATE_CDMA_RXTX)
  {
      if(rf_tx_status == RF_TX_IS_ON)
      {
          rf_tx_stop(RFCOM_TRANSCEIVER_0);
      }
      rf_tx_disable(RFCOM_TRANSCEIVER_0);
  }
  else
  {

      MSG( MSG_SSID_RF, MSG_LEGACY_MED, "Tx is already disabled" );
  }

} /* end of rf_tx_brute_shutdown */


/*===========================================================================

FUNCTION RF_DIGITAL_BAND_SELECT                         EXTERNALIZED FUNCTION

DESCRIPTION
  Initialises the rf card for the current band(Cellular/PCS). This function
  has to be called whenever there is a band change from PCS to Cellular or
  vice versa.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Updates the data base and rf_curr_cdma_band with the current
  band setting.

===========================================================================*/

void rf_digital_band_select (  rfcom_device_enum_type device,  byte rf_band )
{
#if  defined(RF_HAS_GPS) && !defined(RF_HAS_BYPASS_RF_STATE_GPS_CHECK)
#error code not present
#endif
  rf_card_band_type band = (rf_card_band_type) cai_to_rf[rf_band];

  db_items_value_type db_value;  /* temp for database item DB_RF_MODE */
  rf_path_enum_type path = rfcom_to_path[ device ];

  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];

  if (path >= RF_PATH_MAX)
  {
     MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
     rf_error_flag = TRUE;
     return;
  }

  if (rf_band >= RF_MAX_BAND)
  {
     MSG_FATAL("band class invalid or out of bounds", 0, 0, 0);
     rf_error_flag = TRUE;
     return;
  }
  band = (rf_card_band_type) cai_to_rf[rf_band]; /*only assign band a value after error check */


#ifdef RF_TIME_PROFILING
#error code not present
#endif

  /* Translate the band class coming into the Rf driver to a 
     different band. This function is only active during testing. */
  /* If no translation is performed then "band" will stay the same. */
  band = rf_translate_band_class(RF_TRANSLATE_BC_IN, band);  

  MSG_HIGH("Band Class is !! %d ", rf_band, 0, 0);
/* need to revisit this from rf_band to device->band */
#if 0
  if(rf_band == (byte)rf_chain_ptr->rf_curr_cdma_band)
  {
    MSG_LOW("No Change in Band !!", 0, 0, 0);
  }
  else
#endif
  {

    switch ( device )
    {
    case RFCOM_TRANSCEIVER_0:

      switch( band )
      {
        case RF_BC1_BAND: /*   US PCS         */
        case RF_BC4_BAND: /*   Korean PCS     */
        case RF_BC5_BAND: /*   NMT            */
        case RF_BC6_BAND: /*   IMT            */
        case RF_BC11_BAND:/*   BC11           */
        case RF_BC14_BAND:/*   US PCS + 5MHz  */
        case RF_BC15_BAND:/*   AWS Band       */
        #if defined(RF_HAS_GPS) && !defined(RF_HAS_BYPASS_RF_STATE_GPS_CHECK)
#error code not present
        #endif
        
        
        /* update the current band setting */
        #ifdef RF_HAS_SUB_BLOCK_SUPPORT
        rf_chain_ptr->rf_nv_dig_items_ptr = rf_digital_nv_table[path][band];
        #else
        rf_chain_ptr->rf_nv_dig_items_ptr = rf_digital_nv_table[band];
        #endif

        rf_chain_ptr->rf_prev_cdma_band = rf_chain_ptr->rf_curr_cdma_band;
        rf_chain_ptr->rf_curr_cdma_band = band;
        rf_chain_ptr->rf_mode = DB_RF_PCN;      /* update rf_mode            */

        #if defined (RF_HAS_MDSP_TX_AGC) || defined(RF_HAS_PDM_VS_POWER_TX_CAL)
        if ( rf_chain_ptr->rf_prev_cdma_band != rf_chain_ptr->rf_curr_cdma_band) 
        {           
          rf_cdma_load_master_lin_tables = TRUE;
        }
        #endif
        
        (void) rfc_init_card( device,band);     /* Init card to current band */

        /* make sure RFR is awake */
        rfr_sleep_disable( path );

       #ifdef RF_HAS_SUPPORT_FOR_RICO_BRINGUP
#error code not present
       #endif /*RF_HAS_SUPPORT_FOR_RICO_BRINGUP*/  
      
        /* Program the BTF counter to compensate for delay for 1900MHz      */
        #if defined(FEATURE_ENC_BTF_IN_NV)
         enc_init_pa(TRUE, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_enc_btf_dly);
        #else
         enc_init_pa(TRUE);
        #endif

        #ifndef SRCH_HAS_PILOT_GAIN_CNTL
        /* Set Pilot filter gain to 764 Hz BW. */
        srch_fing_set_dmod_pilot_gain( rf_band );
        #endif /* !SRCH_HAS_PILOT_GAIN_CNTL */
        break;

      case RF_BC0_BAND:/* US Cellular */
      case RF_BC3_BAND:/* JCDMA       */
      case RF_BC10_BAND:

        #if defined(RF_HAS_GPS) && !defined(RF_HAS_BYPASS_RF_STATE_GPS_CHECK)
#error code not present
        #endif

        /* update the current band setting
         */
        rf_chain_ptr->rf_prev_cdma_band = rf_chain_ptr->rf_curr_cdma_band;

            rf_chain_ptr->rf_curr_cdma_band = band;

        #ifdef RF_HAS_SUB_BLOCK_SUPPORT
        rf_chain_ptr->rf_nv_dig_items_ptr = rf_digital_nv_table[path][band];
        #else
        rf_chain_ptr->rf_nv_dig_items_ptr = rf_digital_nv_table[band];          
        #endif                  
        
        /* update the database     */
        rf_chain_ptr->rf_mode = DB_RF_CDMA;      
                
        #if defined (RF_HAS_MDSP_TX_AGC) || defined(RF_HAS_PDM_VS_POWER_TX_CAL)
        if ( rf_chain_ptr->rf_prev_cdma_band != rf_chain_ptr->rf_curr_cdma_band) 
        {           
          rf_cdma_load_master_lin_tables = TRUE;
        }
        #endif

        /* initialise card to cdma */
        (void) rfc_init_card(device,band);                   

        /* Make sure RFR is awake */
        rfr_sleep_disable( path );

        #ifdef RF_HAS_SUPPORT_FOR_RICO_BRINGUP
#error code not present
        #endif/*RF_HAS_SUPPORT_FOR_RICO_BRINGUP*/
               

        /* Program the BTF counter to compensate for delay for 800MHz      */

        #if defined(FEATURE_ENC_BTF_IN_NV)
         enc_init_pa(FALSE, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_enc_btf_dly);
        #else
         enc_init_pa(FALSE);
        #endif

        #ifndef SRCH_HAS_PILOT_GAIN_CNTL
        /* Set Pilot filter gain to 382 Hz BW. */
        srch_fing_set_dmod_pilot_gain( rf_band );
        #endif /* !SRCH_HAS_PILOT_GAIN_CNTL */

        break;

#ifdef RF_HAS_GPS
#error code not present
#endif

      /* Unknown band */
      default:
        RF_FATAL_ERROR("Unknown RF band.");
      }

      /* set primary path AGC parameters */
      rf_set_rx_agc_mode( path, rf_chain_ptr->rf_mode, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_mixer_range_pol );

      /* Set the LNA rise/fall etc. for new band */
      rf_load_lna_switching_ctls(device);

      /* load the IM level controls */
      rf_load_im_level_ctls(device);

      db_value.rf_mode = rf_chain_ptr->rf_mode;                 /* write to database */
      db_put( DB_RF_MODE, &db_value );

#ifdef RF_HAS_GPS
#error code not present
#endif

      /* setup the rxf front based on rf_mode */
      switch (rf_chain_ptr->rf_mode) {
      case DB_RF_GPS:
#ifdef RF_HAS_EGPS
#error code not present
#endif /*RF_HAS_EGPS*/
        break;
      case DB_RF_CDMA:
      case DB_RF_PCN:
#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */
        {
          rxf_set_mode_cdma(path, rf_chain_ptr->rf_curr_cdma_band);
        }
        break;

      default:
        break;
      }

#ifdef RF_HAS_PA_DVS_NV
      /* 
      * Need to re-configure PA DVS setting in case different band uses different
      * PA types and potentially different DVS voltage settings.  Chain0 for
      * transmission band change only.
      */
      if (RF_GPS_BAND != rf_chain_ptr->rf_curr_cdma_band)
      {
         rf_config_pa_dvs();
      }
#endif /* RF_HAS_PA_DVS_NV */

      MSG_LOW(" Band Change Completed ", 0, 0, 0);

      break;

    case RFCOM_RECEIVER_DIV:
      {

      rf_chain_state_type *transceiver_status_ptr =
            &rf_chain_status[ rfcom_to_path[ RFCOM_TRANSCEIVER_0 ] ];

      switch( band )
      {
      /* Change to PCS band   */
      case RF_BC1_BAND: /* US PCS       */                          
      case RF_BC4_BAND: /* Korean PCS   */
      case RF_BC5_BAND: /* NMT          */
      case RF_BC6_BAND: /* IMT (2.1GHz) */
      case RF_BC11_BAND:/* BC11         */
      case RF_BC14_BAND:/* US PCS+5MHz  */
      case RF_BC15_BAND:/*   AWS Band       */
      
#ifdef RF_HAS_GPS
#error code not present
#endif
        /* update the current band setting*/
        #ifdef RF_HAS_SUB_BLOCK_SUPPORT
        rf_digital_nv_table[RF_PATH_0][band] = rf_digital_nv_table[RF_PATH_1][band];
        transceiver_status_ptr->rf_nv_dig_items_ptr = 
            rf_chain_ptr->rf_nv_dig_items_ptr = rf_digital_nv_table[path][band];
        #else                
        transceiver_status_ptr->rf_nv_dig_items_ptr = 
            rf_chain_ptr->rf_nv_dig_items_ptr = rf_digital_nv_table[band];
        #endif
        rf_chain_ptr->rf_prev_cdma_band =
            rf_chain_ptr->rf_curr_cdma_band;
        transceiver_status_ptr->rf_prev_cdma_band =
            transceiver_status_ptr->rf_curr_cdma_band;
        transceiver_status_ptr->rf_curr_cdma_band =
            rf_chain_ptr->rf_curr_cdma_band = band;

        /* update rf_mode          */
        rf_chain_ptr->rf_mode = transceiver_status_ptr->rf_mode = DB_RF_PCN;
        
        #if defined (RF_HAS_MDSP_TX_AGC) || defined(RF_HAS_PDM_VS_POWER_TX_CAL)
        if ( transceiver_status_ptr->rf_prev_cdma_band != transceiver_status_ptr->rf_curr_cdma_band) 
        {           
          rf_cdma_load_master_lin_tables = TRUE;
        }
        #endif
        
        /* Init card to current band  */
        (void) rfc_init_card( RFCOM_TRANSCEIVER_0,rf_chain_ptr->rf_curr_cdma_band);
        (void) rfc_init_card( RFCOM_RECEIVER_DIV,rf_chain_ptr->rf_curr_cdma_band);

        /* make sure RFR is awake */
        rfr_sleep_disable( RF_PATH_0 );
        rfr_sleep_disable( RF_PATH_1 );


        /* Program the BTF counter to compensate for delay for 1900MHz      */
#if defined(FEATURE_ENC_BTF_IN_NV)
        enc_init_pa(TRUE, transceiver_status_ptr->rf_nv_dig_items_ptr->rfnv_enc_btf_dly);
#else
        enc_init_pa(TRUE);
#endif

#ifndef SRCH_HAS_PILOT_GAIN_CNTL
        /* Set Pilot filter gain to 764 Hz BW. */
        srch_fing_set_dmod_pilot_gain( band );
#endif /* !SRCH_HAS_PILOT_GAIN_CNTL */

        break;
        
      /* Change to CDMA band */
      case RF_BC0_BAND:/* US Cellular */
      case RF_BC3_BAND:/* JCDMA       */
      case RF_BC10_BAND:

#ifdef RF_HAS_GPS
#error code not present
#endif

        /* Update the current band setting*/
        rf_chain_ptr->rf_prev_cdma_band =
            rf_chain_ptr->rf_curr_cdma_band;

        transceiver_status_ptr->rf_prev_cdma_band =
            transceiver_status_ptr->rf_curr_cdma_band;

        transceiver_status_ptr->rf_curr_cdma_band =
          rf_chain_ptr->rf_curr_cdma_band = band;
        
        #ifdef RF_HAS_SUB_BLOCK_SUPPORT
        rf_digital_nv_table[RF_PATH_0][band] = rf_digital_nv_table[RF_PATH_1][band];
        transceiver_status_ptr->rf_nv_dig_items_ptr =
            rf_chain_ptr->rf_nv_dig_items_ptr = rf_digital_nv_table[path][band];
        #else                
        transceiver_status_ptr->rf_nv_dig_items_ptr = 
            rf_chain_ptr->rf_nv_dig_items_ptr = rf_digital_nv_table[rf_chain_ptr->rf_curr_cdma_band];
        #endif

        transceiver_status_ptr->rf_mode = rf_chain_ptr->rf_mode = DB_RF_CDMA;
        
        #if defined (RF_HAS_MDSP_TX_AGC) || defined(RF_HAS_PDM_VS_POWER_TX_CAL)
        if ( transceiver_status_ptr->rf_prev_cdma_band != transceiver_status_ptr->rf_curr_cdma_band) 
        {           
          rf_cdma_load_master_lin_tables = TRUE;
        }
        #endif
               
         /* initialise card to cdma */
        (void) rfc_init_card(RFCOM_TRANSCEIVER_0, transceiver_status_ptr->rf_curr_cdma_band);
        (void) rfc_init_card(RFCOM_RECEIVER_DIV, rf_chain_ptr->rf_curr_cdma_band);  

        /* Make sure RFR is awake */
        rfr_sleep_disable( RF_PATH_0 );
        rfr_sleep_disable( RF_PATH_1 );


        /* Program the BTF counter to compensate for delay for 800MHz      */

#if defined(FEATURE_ENC_BTF_IN_NV)
        enc_init_pa(FALSE, transceiver_status_ptr->rf_nv_dig_items_ptr->rfnv_enc_btf_dly);
#else
        enc_init_pa(FALSE);
#endif

#ifndef SRCH_HAS_PILOT_GAIN_CNTL
        /* Set Pilot filter gain to 382 Hz BW. */
        srch_fing_set_dmod_pilot_gain( rf_band );
#endif /* !SRCH_HAS_PILOT_GAIN_CNTL */

        break;
#ifdef RF_HAS_GPS
#error code not present
#endif
      /* Unknown band */
      default:
        RF_FATAL_ERROR("Unknown RF band.");
      }

      /* set primary and secondary path AGC parameters */
      rf_set_rx_agc_mode( RF_PATH_0, transceiver_status_ptr->rf_mode, transceiver_status_ptr->rf_nv_dig_items_ptr->rfnv_mixer_range_pol );
      rf_set_rx_agc_mode( RF_PATH_1, transceiver_status_ptr->rf_mode, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_mixer_range_pol );

      /* Set the LNA rise/fall etc. for new band */
      rf_load_lna_switching_ctls( RFCOM_TRANSCEIVER_0 );
      rf_load_lna_switching_ctls( RFCOM_RECEIVER_1 );

      /* load the IM level controls */
      rf_load_im_level_ctls( RFCOM_TRANSCEIVER_0 );
      rf_load_im_level_ctls( RFCOM_RECEIVER_1 );

      db_value.rf_mode = transceiver_status_ptr->rf_mode;  /* write to database */
      db_put( DB_RF_MODE, &db_value );

#ifdef RF_HAS_GPS
#error code not present
#endif

      /* setup the rxf front based on rf_mode */
      switch (transceiver_status_ptr->rf_mode) {
      case DB_RF_CDMA:
      case DB_RF_PCN:
#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */
        {
          rxf_set_mode_cdma(RF_PATH_0, transceiver_status_ptr->rf_curr_cdma_band);
          rxf_set_mode_cdma(RF_PATH_1, transceiver_status_ptr->rf_curr_cdma_band);
        }
        break;

      default:
        break;
      }

#ifdef RF_HAS_PA_DVS_NV
      /* 
      * Need to re-configure PA DVS setting in case different band uses different
      * PA types and potentially different DVS voltage settings.  Chain0 for
      * transmission band change only.
      */
      if (RF_GPS_BAND != rf_chain_ptr->rf_curr_cdma_band)
      {
         rf_config_pa_dvs();
      }
#endif /* RF_HAS_PA_DVS_NV */

      MSG_LOW(" Band Change Completed ", 0, 0, 0);

      break;

      }

    case RFCOM_RECEIVER_1:

      switch( band )
      {
      /* Change to PCS band   */
      case RF_BC1_BAND: /*   US PCS         */
      case RF_BC4_BAND: /*   Korean PCS     */ 
      case RF_BC5_BAND: /*   NMT            */ 
      case RF_BC6_BAND: /*   IMT            */ 
      case RF_BC11_BAND:/*   BC11           */
      case RF_BC14_BAND:/*   US PCS + 5MHz  */
      case RF_BC15_BAND:/*   AWS Band       */
#if defined(RF_HAS_GPS) && !defined(RF_HAS_BYPASS_RF_STATE_GPS_CHECK)
#error code not present
#endif
        /* update the current band setting */
        #ifdef RF_HAS_SUB_BLOCK_SUPPORT        
          rf_chain_ptr->rf_nv_dig_items_ptr = rf_digital_nv_table[path][band];
        #else                        
        rf_chain_ptr->rf_nv_dig_items_ptr = rf_digital_nv_table[band];
        #endif        
        rf_chain_ptr->rf_prev_cdma_band = rf_chain_ptr->rf_curr_cdma_band;
        rf_chain_ptr->rf_curr_cdma_band = band;
        rf_chain_ptr->rf_mode = DB_RF_PCN;      /* update rf_mode          */
        (void) rfc_init_card( device,band );    /* Init card to pcs        */
        
        /* make sure RFR is awake */
        rfr_sleep_disable( path );

        break;
      
      /* Change to CDMA band */
      case RF_BC0_BAND: /*   US Cellular    */
      case RF_BC3_BAND: /*   JCDMA          */
      case RF_BC10_BAND:

#if defined(RF_HAS_GPS) && !defined(RF_HAS_BYPASS_RF_STATE_GPS_CHECK)
#error code not present
#endif
        /* update the current band setting
         */
        rf_chain_ptr->rf_prev_cdma_band = rf_chain_ptr->rf_curr_cdma_band;

        rf_chain_ptr->rf_curr_cdma_band = band;
        #ifdef RF_HAS_SUB_BLOCK_SUPPORT        
        rf_chain_ptr->rf_nv_dig_items_ptr = rf_digital_nv_table[path][band];
        #else                        
        rf_chain_ptr->rf_nv_dig_items_ptr = rf_digital_nv_table[band];          
        #endif                  
        rf_chain_ptr->rf_mode = DB_RF_CDMA;           /* update the database     */
        (void) rfc_init_card(device,band);            /* initialise card to cdma */

        /* Make sure RFR is awake */
        rfr_sleep_disable( path );

        break;
#ifdef RF_HAS_GPS
#error code not present
#endif
      /* Unknown band */
      default:
        RF_FATAL_ERROR("Unknown RF band.");
      }

      /* set secondary path AGC parameters */
      rf_set_rx_agc_mode( path, rf_chain_ptr->rf_mode, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_mixer_range_pol );

      /* Set the LNA rise/fall etc. for new band */
      rf_load_lna_switching_ctls(device);

      /* load the IM level controls */
      rf_load_im_level_ctls(device);

#ifdef RF_HAS_GPS
#error code not present
#endif

      /* setup the rxf front based on rf_mode */
      switch (rf_chain_ptr->rf_mode) {
      case DB_RF_GPS:
#ifdef RF_HAS_EGPS
#error code not present
#endif /*RF_HAS_EGPS */
        break;
      case DB_RF_CDMA:
      case DB_RF_PCN:
#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */
        {
          rxf_set_mode_cdma(path, rf_chain_ptr->rf_curr_cdma_band);
        }
        break;

      default:
        break;

      }

      MSG_LOW(" Band Change Completed ", 0, 0, 0);

      break;

    default:

      RF_FATAL_ERROR("Device %d not supported!!!");
      break;

    } 

  }

  #ifdef RF_TIME_PROFILING
#error code not present
  #endif

} /* rf_digital_band_select */

/*____________________________________________________________________________
                            GPS related functions
____________________________________________________________________________*/

#ifdef RF_HAS_GPS
#error code not present
#endif /* RF_HAS_GPS */
/*________________________________
  End of GPS related functions
  ________________________________*/



/*===========================================================================

FUNCTION RF_FREQ_INDEX_CALC                                 INTERNAL FUNCTION

DESCRIPTION
  Calculates and returns a frequency table index (0..NV_FREQ_TABLE_SIZ-1) for
  (channel).  (output value) = ((input value)*(output span))/(input span).
  This is used for indexing local copies of NV tables.

DEPENDENCIES
  rf_band_1900 must be setup

RETURN VALUE
  byte sized frequency table index 0..NV_FREQ_TABLE_SIZ-1

SIDE EFFECTS
  None

===========================================================================*/
static byte rf_freq_index_calc
(
  word channel,             /* channel selected */
  rf_card_band_type rf_curr_cdma_band
)
{

    if ((rf_curr_cdma_band == RF_BC0_BAND ) ||
        (rf_curr_cdma_band == RF_BC10_BAND ))
    {
        return( (byte)
          (( rfc_chan_offset( channel ) * NV_FREQ_TABLE_SIZ )
           / RF_800MHZ_CHAN_SPN ));
    } 
    else if (rf_curr_cdma_band == RF_BC4_BAND) 
    {
        return( (byte)(( channel * NV_FREQ_TABLE_SIZ ) / RF_KPCS_CHAN_SPN ));
    }
    
    else if ((rf_curr_cdma_band == RF_BC1_BAND) ||
             (rf_curr_cdma_band == RF_BC5_BAND) || 
             (rf_curr_cdma_band == RF_BC6_BAND) ||
             (rf_curr_cdma_band == RF_BC11_BAND)||
             (rf_curr_cdma_band == RF_BC14_BAND)||
             (rf_curr_cdma_band == RF_BC15_BAND))             
    {
        return( (byte)(( channel * NV_FREQ_TABLE_SIZ ) / RF_PCS_CHAN_SPN ));
    }
    else /* RF_BC3_BAND */
    {
      /*
      * Table for mapping JCDMA channels/freq to freq index (bins)
      * Assumes use of 16 bins.
      */
      byte index;
      if ((channel >= 1100) && (channel <=1140)) index = 0;
       else if ((channel >= 1260) && (channel <=1344)) index = 1;
       else if ((channel >= 1346) && (channel <=1430)) index = 2;
       else if ((channel >= 1432) && (channel <=1514)) index = 3;
       else if ((channel >= 1516) && (channel <=1600)) index = 4;
       else if ((channel >=  802) && (channel <= 890)) index = 5;
       else if ((channel >=  892) && (channel <= 980)) index = 6;
       else if ((channel >=   60) && (channel <= 144)) index = 7;
       else if ((channel >=  146) && (channel <= 230)) index = 8;
       else if ((channel >=  232) && (channel <= 314)) index = 9;
       else if ((channel >=  316) && (channel <= 400)) index = 10;
       else if ((channel >=  402) && (channel <= 484)) index = 11;
       else if ((channel >=  486) && (channel <= 570)) index = 12;
       else if ((channel >=  572) && (channel <= 654)) index = 13;
       else if ((channel >=  656) && (channel <= 696)) index = 14;
       else index = 15;   /* chans 698 - 740 */
    
      return ( index );
    }

} /* rf_freq_index_calc */


#ifdef RF_CAGC_DEBUG
#error code not present
#endif /* CAGC_DEBUG */


#ifdef FEATURE_ZIF_LOGGING
/*===========================================================================

FUNCTION RF_LOG_IQ_OFFSETS

DESCRIPTION    This function is invoked to log I/Q Offset values for DIAG.

DEPENDENCIES   rf_iq_offsets_vars

RETURN VALUE   None.

SIDE EFFECTS   Messages can be sent to DIAG task.

===========================================================================*/

void  rf_log_iq_offsets ( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check to see I/Q Offset logging is enabled. If not, just bail out. */

  if ( !log_status(LOG_IQ_OFFSET_REGISTERS_C) )
  {
    /* Logging is not enabled. Reset index and return */
    rf_iq_offsets_vars.inx = 0;
    return;
  }

  /* Return if in sleep state since RXF clocks will be off */
  if ( rf_is_sleeping_cdma1x(RFCOM_TRANSCEIVER_0)) 
  {
     return;
  }

  /* New buffer? */
  if ( rf_iq_offsets_vars.inx == 0 )
  {
    /* Refresh the timestamp at the start of each new buffer */
    ts_get ( (unsigned long *) rf_iq_offsets_vars.log.hdr.ts );

  } /* If we are just getting started */

  /* Now fill in the items which should get updated at the sample rate */

#ifndef RF_HAS_RXF_V2
#error code not present
#else /* RF_HAS_RXF_V2 */
  /* RXF FG I offset sample */
  rf_iq_offsets_vars.log.rxf_fg_ioffset[ rf_iq_offsets_vars.inx ]
    = (int4)(HWIO_INMI(RXF_SRCc_FG_IOFFSET, 0, (uint32)HWIO_RMSK(RXF_SRCc_FG_IOFFSET)));

  /* RXF FG Q offset sample */
  rf_iq_offsets_vars.log.rxf_fg_qoffset[ rf_iq_offsets_vars.inx ]
    = (int4)HWIO_INMI(RXF_SRCc_FG_QOFFSET, 0, (uint32)HWIO_RMSK(RXF_SRCc_FG_QOFFSET));

  /* DACC Estimated I offset sample */
  rf_iq_offsets_vars.log.dacc_est_ioffset[ rf_iq_offsets_vars.inx ]
    = (int4)HWIO_INMI(RXF_SRCc_DACC_EST_IOFFSET, 0, (uint32)HWIO_RMSK(RXF_SRCc_DACC_EST_IOFFSET));

  /* DACC Estimated Q offset sample */
  rf_iq_offsets_vars.log.dacc_est_qoffset[ rf_iq_offsets_vars.inx ]
    = (int4)HWIO_INMI(RXF_SRCc_DACC_EST_QOFFSET, 0, (uint32)HWIO_RMSK(RXF_SRCc_DACC_EST_QOFFSET));
#endif /* !RF_HAS_RXF_V2 */

  /* Buffer full? */
  if ( ++rf_iq_offsets_vars.inx >= LOG_IQ_OFFSETS_CNT )
  {
    /* We've filled up the logging buffer. */
    if (log_submit((log_type *) &rf_iq_offsets_vars.log) == FALSE)
    {
      rf_iq_offsets_vars.drops++;
    }

    rf_iq_offsets_vars.inx = 0;
      /* Point to first bin in the hopper. This gets bumped
         on every logging call.  It also signals a new buffer. */
  }

} /* rf_log_iq_offsets() */



/*===========================================================================

FUNCTION RF_LOG_DACC_IQ_ACCUM

DESCRIPTION    This function is invoked to log DACC I/Q Accumulator values
               for DIAG.

DEPENDENCIES   rf_dacc_iq_accum_vars

RETURN VALUE   None.

SIDE EFFECTS   Messages can be sent to DIAG task.

===========================================================================*/

void  rf_log_dacc_iq_accum ( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check to see DACC I/Q Accumulator logging is enabled. If not, just bail
     out. */

  if ( !log_status(LOG_DACC_IQ_ACCUMULATOR_C) )
  {
    /* Logging is not enabled. Reset index and return */
    rf_dacc_iq_accum_vars.inx = 0;
    return;
  }

  /* Return if in sleep state since RXF clocks will be off */
  if ( rf_is_sleeping_cdma1x(RFCOM_TRANSCEIVER_0)) 
  {
     return;
  }

  /* New buffer? */
  if ( rf_dacc_iq_accum_vars.inx == 0 )
  {
    /* Refresh the timestamp at the start of each new buffer */
    ts_get ( (unsigned long *) rf_dacc_iq_accum_vars.log.hdr.ts );

  } /* If we are just getting started */

  /* Now fill in the items which should get updated at the sample rate */
#ifndef RF_HAS_RXF_V2
#error code not present
#else /* RF_HAS_RXF_V2 */
  /* DC Offset of I Accumulator for gain setp 0 sample */
  rf_dacc_iq_accum_vars.log.dacc_iaccum0[ rf_dacc_iq_accum_vars.inx ]
    = (int2)HWIO_INMI(RXF_SRCc_DACC_IACCUM0, 0, (uint32)HWIO_RMSK(RXF_SRCc_DACC_IACCUM0));

  /* DC Offset of I Accumulator for gain setp 1 sample */
  rf_dacc_iq_accum_vars.log.dacc_iaccum1[ rf_dacc_iq_accum_vars.inx ]
    = (int2)HWIO_INMI(RXF_SRCc_DACC_IACCUM1, 0, (uint32)HWIO_RMSK(RXF_SRCc_DACC_IACCUM1));

  /* DC Offset of I Accumulator for gain setp 2 sample */
  rf_dacc_iq_accum_vars.log.dacc_iaccum2[ rf_dacc_iq_accum_vars.inx ]
    = (int2)HWIO_INMI(RXF_SRCc_DACC_IACCUM2, 0, (uint32)HWIO_RMSK(RXF_SRCc_DACC_IACCUM2));

  /* DC Offset of I Accumulator for gain setp 3 sample */
  rf_dacc_iq_accum_vars.log.dacc_iaccum3[ rf_dacc_iq_accum_vars.inx ]
    = (int2)HWIO_INMI(RXF_SRCc_DACC_IACCUM3, 0, (uint32)HWIO_RMSK(RXF_SRCc_DACC_IACCUM3));

  /* DC Offset of I Accumulator for gain setp 4 sample */
  rf_dacc_iq_accum_vars.log.dacc_iaccum4[ rf_dacc_iq_accum_vars.inx ]
    = (int2)HWIO_INMI(RXF_SRCc_DACC_IACCUM4, 0, (uint32)HWIO_RMSK(RXF_SRCc_DACC_IACCUM4));

  /* DC Offset of Q Accumulator for gain setp 0 sample */
  rf_dacc_iq_accum_vars.log.dacc_qaccum0[ rf_dacc_iq_accum_vars.inx ]
    = (int2)HWIO_INMI(RXF_SRCc_DACC_QACCUM0, 0, (uint32)HWIO_RMSK(RXF_SRCc_DACC_QACCUM0));

  /* DC Offset of Q Accumulator for gain setp 1 sample */
  rf_dacc_iq_accum_vars.log.dacc_qaccum1[ rf_dacc_iq_accum_vars.inx ]
    = (int2)HWIO_INMI(RXF_SRCc_DACC_QACCUM1, 0, (uint32)HWIO_RMSK(RXF_SRCc_DACC_QACCUM1));

  /* DC Offset of Q Accumulator for gain setp 2 sample */
  rf_dacc_iq_accum_vars.log.dacc_qaccum2[ rf_dacc_iq_accum_vars.inx ]
    = (int2)HWIO_INMI(RXF_SRCc_DACC_QACCUM2, 0, (uint32)HWIO_RMSK(RXF_SRCc_DACC_QACCUM2));

  /* DC Offset of Q Accumulator for gain setp 3 sample */
  rf_dacc_iq_accum_vars.log.dacc_qaccum3[ rf_dacc_iq_accum_vars.inx ]
    = (int2)HWIO_INMI(RXF_SRCc_DACC_QACCUM3, 0, (uint32)HWIO_RMSK(RXF_SRCc_DACC_QACCUM3));

  /* DC Offset of Q Accumulator for gain setp 4 sample */
  rf_dacc_iq_accum_vars.log.dacc_qaccum4[ rf_dacc_iq_accum_vars.inx ]
    = (int2)HWIO_INMI(RXF_SRCc_DACC_QACCUM4, 0, (uint32)HWIO_RMSK(RXF_SRCc_DACC_QACCUM4));
#endif /* !RF_HAS_RXF_V2 */

  /* Buffer full? */
  if ( ++rf_dacc_iq_accum_vars.inx >= LOG_DACC_IQ_ACCUM_CNT )
  {
    /* We've filled up the logging buffer. */
    if (log_submit((log_type *) &rf_dacc_iq_accum_vars.log) == FALSE)
    {
      rf_dacc_iq_accum_vars.drops++;
    }

    rf_dacc_iq_accum_vars.inx = 0;
      /* Point to first bin in the hopper. This gets bumped
         on every logging call.  It also signals a new buffer. */
  }

} /* rf_log_dacc_iq_accum() */


/*===========================================================================

FUNCTION RF_INIT_ZIF_IQ_LOGS

DESCRIPTION    This function is invoked to perform ZIF I/Q logging.

DEPENDENCIES   rf_iq_offsets_vars
               rf_dacc_iq_accum_vars

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/

void  rf_init_zif_iq_logs ( void )
{
  /* Log buffer for I/Q Offset Logging */
  rf_iq_offsets_vars.log.hdr.code = LOG_IQ_OFFSET_REGISTERS_C;
  rf_iq_offsets_vars.log.hdr.len  = sizeof(log_iq_offsets_type);
  rf_iq_offsets_vars.inx      = 0;
  rf_iq_offsets_vars.drops    = 0;

  /* Log buffer for DACC I/Q Accumulator Logging */
  rf_dacc_iq_accum_vars.log.hdr.code = LOG_DACC_IQ_ACCUMULATOR_C;
  rf_dacc_iq_accum_vars.log.hdr.len  = sizeof(log_dacc_iq_accum_type);
  rf_dacc_iq_accum_vars.inx      = 0;
  rf_dacc_iq_accum_vars.drops    = 0;

} /* rf_init_zif_iq_logs */
#endif /* FEATURE_ZIF_LOGGING */


/*===========================================================================

FUNCTION RF_TUNE_BAND_CHAN                                EXTERNALIZED FUNCTION

DESCRIPTION
  This function tunes the UHF Tx/Rx synthesizer to the specified channel in
  the specified band. If the band and channel have not changed since the
  last time the rf was tuned nothing is done.  If the band has not changed
  only the channel is tuned. If the band has changed both a band change
  and channel tune will be performed. The amount of time to wait before
  RF is settled is returned.

DEPENDENCIES
  rf_config, rf_pa_range_step_cal, and rf_cdma_tx_comp_vs_freq[] must
  have been loaded from NV storage
  rf_state must be setup

RETURN VALUE
   word : time in microseconds to wait for RF to settle

SIDE EFFECTS
  Updates database item DB_CHANNEL to the new channel
  Updates rf_curr_cdma_band
  Updates rf_freq_index

===========================================================================*/
word rf_tune_band_chan
(
    rfcom_device_enum_type device,        /* RF Multimode device */
    rf_card_band_type band,
    word chan
)
{
    #ifdef RF_HAS_SUB_BLOCK_SUPPORT    
    rfnv_digital_items_type * digital_item_ptr = NULL;    
    boolean block_changed = TRUE;
    #endif

    #ifdef RF_HAS_VARIABLE_SMPS_SWITCHER_SIZE
    pm_err_flag_type pm_level_error = PM_ERR_FLAG__SUCCESS;
    #endif

    rf_chain_state_type *rf_chain_ptr = &rf_chain_status[rfcom_to_path[device]];
    boolean chan_changed = FALSE;
    word wait_time = 0;
    
#ifdef T_SC2X_PBS
#error code not present
#endif
  
#ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO    
#ifdef RF_HAS_ADC_SAMPLING_CLK_DEBUG
#error code not present
#endif
#endif

    if (device >= RFCOM_MAX_DEVICES)
    {
      MSG_FATAL("device invalid or out of bounds", 0, 0, 0);
      rf_error_flag = TRUE;
      return 0;
    }  

#ifdef RF_HAS_POWER_MODE_LIB
    if ( (device == RFCOM_TRANSCEIVER_0 || device == RFCOM_RECEIVER_DIV) &&
         rf_intelliceiver_is_enabled(rfcom_to_path[device]) )
    {
      rf_intelliceiver_pause( rfcom_to_path[device] );
    }
#endif /* RF_HAS_POWER_MODE_LIB */
    
    /* Translate the band class coming into the Rf driver to a 
       different band. This function is only active during testing. */
    /* If no translation is performed then "band" will stay the same. */
    band = rf_translate_band_class(RF_TRANSLATE_BC_IN, band);

    if (band >= RF_MAX_BAND)
    {
        MSG_FATAL("band class invalid or out of bounds", 0, 0, 0);
        rf_error_flag = TRUE;
        return 0;
    }  

    #ifdef RF_HAS_SUB_BLOCK_SUPPORT

      digital_item_ptr = rfnv_get_nv_digital_item_ptr(device,band,chan);

      if(band == rf_chain_ptr->rf_curr_cdma_band)
      {
        if(rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_block_mask == digital_item_ptr->rfnv_block_mask)
        {
          block_changed = FALSE;
        }
      }      
          
    #endif


    #ifdef RF_HAS_SUB_BLOCK_SUPPORT   
    if ((band != rf_chain_ptr->rf_curr_cdma_band )||(block_changed == TRUE))
    #else
    if ( band != rf_chain_ptr->rf_curr_cdma_band )
    #endif
    {
       /* When tuning the radio while RF is enabled, the RF driver is
          responsible for saving DC offsets.  During slotted sleep L1
          is responsible for saving DC offsets on the other hand. */

       rxf_save_accum_data( rfcom_to_path[device] );

       #ifdef RF_HAS_SUB_BLOCK_SUPPORT
       rfnv_update_band_digital_items_ptr(device,band,digital_item_ptr);
       rfc_set_block(device,band,digital_item_ptr->rfnv_block_mask);
       #endif

       /* Tune to new band */
       rf_digital_band_select(device, (byte)band );

       #ifdef RF_HAS_VARIABLE_SMPS_SWITCHER_SIZE
       switch(band)
       {
       case RF_BC0_BAND:
       case RF_BC10_BAND:
       case RF_BC5_BAND:
       case RF_BC11_BAND:
       case RF_BC6_BAND:
         pm_level_error |= pm_vreg_smps_individual_clock_div_sel( PM_VREG_SMPS_S1_ID, PM_VREG_SMPS_TCXO_DIV__24);
         pm_level_error |= pm_vreg_smps_individual_clock_div_sel( PM_VREG_SMPS_S2_ID, PM_VREG_SMPS_TCXO_DIV__24);         
         break;

       case RF_BC1_BAND:       
           pm_level_error |= pm_vreg_smps_individual_clock_div_sel( PM_VREG_SMPS_S1_ID, PM_VREG_SMPS_TCXO_DIV__18);
           pm_level_error |= pm_vreg_smps_individual_clock_div_sel( PM_VREG_SMPS_S2_ID, PM_VREG_SMPS_TCXO_DIV__18);           
           break;
       }

       if(pm_level_error != PM_ERR_FLAG__SUCCESS)
       {
         MSG_ERROR("Pmic Switcher Frequency during Band Change is not done correctly",0,0,0);
       }
       #endif
       

       /* Need to re-tune channel */
       chan_changed = TRUE;
    }    
    else
    {
      #ifdef RF_HAS_TEMP_BASED_DEVICE_CONFIG
      if ( chan != rf_chain_ptr->rf_chan )
      {         
        if(rfc_support_temp_based_device_config(band) == TRUE)
        {    
          rfr_set_initial_temp_based_device_config(rfcom_to_path[device], band);
        }
      }
      #endif            

       #ifdef RF_HAS_SUB_BLOCK_SUPPORT
       rfnv_update_band_digital_items_ptr(device,band,digital_item_ptr);
       rfc_set_block(device,band,digital_item_ptr->rfnv_block_mask);
       #endif           
    }
    
    /* Check if chan has changed */
    if (( chan != rf_chain_ptr->rf_chan ) || chan_changed )     
    {
       /* Tune to new channel */
       rf_tune_to_chan_cdma1x( device, chan );
       chan_changed = TRUE;

       /* 
       * Return the RF settling time required for client to wait after
       * this function has exited. Attributed to doing the channel tune.
       * The band tune execution time is incorporated in the function call.
       */
       wait_time = rf_card.rfc_tune_to_chan_time_usec;
    }

    
#ifdef RF_HAS_POWER_MODE_LIB
    if ( rf_intelliceiver_is_enabled(rfcom_to_path[device]) )
    {
      if ( chan_changed )
      {
        rf_intelliceiver_tune( rfcom_to_path[device], band, chan );
      }

      rf_intelliceiver_resume( rfcom_to_path[device] );
    }
#endif /* RF_HAS_POWER_MODE_LIB */
    
    return (wait_time);
}

/*===========================================================================

FUNCTION RF_GET_BAND_CHAN                                EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns the current settings of the RF driver internal
  variables rf_curr_cdma_band and rf_chan for the specified device.

DEPENDENCIES
  Should not be called until after digital mode is entered for the first
  time.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rf_get_band_chan(rf_card_band_type *band, word *chan)
{
  rf_chain_state_type *rf_chain_ptr =
        &rf_chain_status[ rfcom_to_path[ RFCOM_TRANSCEIVER_0 ] ];
  
  /* During testing, this function call will translate 
     the band passed into this function to a different band. During regular 
     operation, it will return the band passes into the function. */
  /* If no translation is performed then the return value is equal to
     rf_chain_ptr->rf_curr_cdma_band. */
  *band = rf_translate_band_class(RF_TRANSLATE_BC_OUT, 
                                        rf_chain_ptr->rf_curr_cdma_band);

  *chan = rf_chain_ptr->rf_chan;
}

/*===========================================================================

FUNCTION RF_GET_BAND_CHAN_TEMP                          EXTERNALIZED FUNCTION

DESCRIPTION
  Temp implementation.
  This function returns the current settings of the RF driver internal
  variables rf_curr_cdma_band and rf_chan for the specified device.

DEPENDENCIES
  Should not be called until after digital mode is entered for the first
  time.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rf_get_band_chan_temp
(
    rfcom_device_enum_type device, 
    rf_card_band_type *band, 
    word *chan
)
{
  rf_chain_state_type *rf_chain_ptr =
        &rf_chain_status[ rfcom_to_path[ device ] ];
  
  /* During testing, this function call will translate 
     the band passed into this function to a different band. During regular 
     operation, it will return the band passes into the function. */
  /* If no translation is performed then the return value is equal to
     rf_chain_ptr->rf_curr_cdma_band. */
  *band = rf_translate_band_class(RF_TRANSLATE_BC_OUT, 
                                         rf_chain_ptr->rf_curr_cdma_band);

  *chan = rf_chain_ptr->rf_chan;
}


/*===========================================================================

FUNCTION RF_DEVICE_RF_CAL_MODE_IS_ENABLED                      INTERNAL FUNCTION

DESCRIPTION
This function is called during RF Cal to inform RF Driver that 
the current operation mode of the phone is RF Cal.
It should be Called with Paremeters TRUE and FALSE depending on whether
enetring or exiting RF Cal

DEPENDENCIES
Need to be Called from RF Cal
RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void rf_device_rf_cal_mode_is_enabled(boolean is_enabled)
{
  rf_device_cal_mode_enabled = is_enabled;
}


/*===========================================================================

FUNCTION RF_CURRENT_MODE_IS_RF_CAL                   INTERNAL FUNCTION

DESCRIPTION
This function returns TRUE or FALSE depending if we are in RF Cal Mode or not

DEPENDENCIES

RETURN VALUE
  Boolean

SIDE EFFECTS

===========================================================================*/
boolean rf_current_mode_is_rf_cal()
{
  return rf_device_cal_mode_enabled;
}


/*===========================================================================

FUNCTION RF_INIT_DIGITAL_BAND_CHAN                      EXTERNALIZED FUNCTION

DESCRIPTION
  This function is meant to be called at CDMA slotted sleep wakeup.  Caller
  may request initialization to different band and channel than the last band
  and channel used.  Changes between PCS and Cellular digital modes are only
  supported.  rf_mode (DB_RF_MODE) must be DB_RF_SLEEP upon entering this
  function. RF state must be RF_STATE_CDMA_RX.

DEPENDENCIES
  1) rf_state must be = RF_STATE_CDMA_RX
  2) Database item DB_RF_MODE must be = DB_RF_SLEEP
  3) Procedure rf_enable_digital_rx_pwr() must have been called before calling
     this procedure.
  4) rf_complete_warmup() must be called when the warm up timer expires to
     finish digital initialization.
  5) if a change in the band class is made, the pilot filter gain value
     is NOT changed and must be set after calling this procedure.

RETURN VALUE
  uin16  representing the usec wait value for RxAGC settling.  Value of 0
         represents AGC needs to be started by calling rf_complete_sleep_warmup().

SIDE EFFECTS
  Updates RF local variable rf_mode and database item DB_RF_MODE to
  either DB_RF_CDMA or DB_RF_PCN
  RF local variable rf_chan and database item DB_RF_CHAN may be updated if
  there is a channel change

===========================================================================*/
uint16 rf_init_digital_band_chan
(
    rfcom_device_enum_type device,
    rf_card_band_type band,
    word chan
)
{
  boolean band_changed = TRUE;   /* assume there has been a band change */
  boolean chan_changed = TRUE;   /* assume there has been a channel change */
  boolean from_sleep = FALSE;    /* assume we are NOT coming from sleep */
  uint16 rx_agc_wait_time = 0;   /* rx agc settling */

#ifdef RF_HAS_RXADC_VOLTAGE_SWITCH
  uint32 rxadc_delta_warm_up_time = 0;  /* any extra warmup time required for Rx ADC */  
  uint32 rf_warmup_time_rxadc_stop = 0;
  pm_err_flag_type pm_level_error = PM_ERR_FLAG__SUCCESS;
#endif
  
  rf_path_enum_type path = rfcom_to_path[ device ];
  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];

  #ifdef RF_HAS_SUB_BLOCK_SUPPORT
  rfnv_digital_items_type* digital_item_ptr = NULL;
  boolean block_changed = TRUE;
  #endif

  #ifdef RF_HAS_WU_TIMED
  #ifdef RF_HAS_WU_TIMED_DEBUG
#error code not present
  #endif
  (void)rf_prof_timer_start(path);
  #endif /* RF_HAS_WU_TIMED */

  if (path >= RF_PATH_MAX)
  {
     MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
     rf_error_flag = TRUE;
     return 0;
  }
  
  #ifdef T_SC2X_PBS
#error code not present
  #endif

  #if (!defined T_FFA) && defined (RF_TIME_PROFILING) 
#error code not present
  #endif /* !T_FFA && RF_TIME_PROFILING */

  if ( rf_chain_ptr->rf_mode == DB_RF_SLEEP )
  {
    from_sleep = TRUE;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Translate the band class coming into the Rf driver to a 
     different band. This function is only active during testing. */
  /* If no translation is performed then "band" will stay the same. */
  band = rf_translate_band_class(RF_TRANSLATE_BC_IN, band);

  #ifdef RF_HAS_SUB_BLOCK_SUPPORT
  switch(device)
  {
  case RFCOM_TRANSCEIVER_0:
  case RFCOM_RECEIVER_1:  

      digital_item_ptr = rfnv_get_nv_digital_item_ptr(device,band,chan);

      if(band == rf_chain_ptr->rf_curr_cdma_band)
      {
        if(rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_block_mask == digital_item_ptr->rfnv_block_mask)
        {
          block_changed = FALSE;
        }
      }      

    break;

  case RFCOM_RECEIVER_DIV:
  case RFCOM_RECEIVER_DUAL:
    MSG_ERROR("Device not supported",0,0,0);
    break;
  }/*switch()*/
  #endif

#ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO    
#ifdef RF_HAS_ADC_SAMPLING_CLK_DEBUG
#error code not present
#endif
#endif


  switch ( device )
  {
  case RFCOM_TRANSCEIVER_0:

#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */

#ifdef RF_HAS_WU_OPTIMIZED
#error code not present
#endif /* RF_HAS_WU_OPTIMIZED */

      /* Original RF WU sequence */

      /* Init and reset the RF devices */
      rf_wu_init_rf_devices(path);

#ifdef RF_HAS_POWER_MODE_LIB
      /* Lock operation of the IntelliCeiver while performing critical
         tuning updates.  This is only necessary if the RF is not asleep. */
      if ( rf_intelliceiver_is_enabled(rfcom_to_path[device])
           && from_sleep == FALSE )
      {
        rf_intelliceiver_pause( rfcom_to_path[device] );
      }
#endif /* RF_HAS_POWER_MODE_LIB */
    
#if !defined ( RF_HAS_BYPASS_BAND_CHANNEL_CHECK )
      /* Check for band and channel changes */
      #ifdef RF_HAS_SUB_BLOCK_SUPPORT
      if (( rf_chain_ptr->rf_curr_cdma_band == band ) && (block_changed == FALSE))
      #else
      if ( rf_chain_ptr->rf_curr_cdma_band == band )
      #endif
      {
         band_changed = FALSE;
         if ( rf_chain_ptr->rf_chan == chan ) 
         {       
           chan_changed = FALSE;
         }
      }
#endif /* RF_HAS_BYPASS_BAND_CHANNEL_CHECK */

      /* Indicate that we are not completely done; we still have to wait
      ** for warmup to complete before we switch back to CDMA mode.
      */
      rf_dig_init_complete = FALSE;

       #ifdef RF_HAS_SUB_BLOCK_SUPPORT
       //if(band_changed == TRUE)
       //{   
         rfnv_update_band_digital_items_ptr(device,band,digital_item_ptr);
         rfc_set_block(device,band,digital_item_ptr->rfnv_block_mask);
       //}
       #endif

      #ifdef RF_HAS_TEMP_BASED_DEVICE_CONFIG             
      if((band_changed == FALSE) && (chan_changed = TRUE))
      {         
        if(rfc_support_temp_based_device_config(band) == TRUE)
        {    
          rfr_set_initial_temp_based_device_config(path, band);
        }
      }
      #endif            

      rf_digital_band_chan_init( device, band_changed, chan_changed, band, chan);

#ifdef RF_HAS_POWER_MODE_LIB
      /* If this is an IntelliCeiver chip, inform the IntelliCeiver
         module that the channel has changed.  The RFR is currently
         asleep in this execution path, so this call does not need
         to be protected by rf_intelliceiver_pause() and
         rf_intelliceiver_resume() */
      if ( rf_intelliceiver_is_enabled(rfcom_to_path[device]) )
      {
        rf_intelliceiver_tune( rfcom_to_path[device], band, chan );
      }
#endif /* RF_HAS_POWER_MODE_LIB */

      /* Tx configurations */
      rf_wu_tx_setup(device);

      /* --------------------------------------------------
      ** Initialize the RF card to the proper band whether
      ** or not we are coming from sleep or a band change
      ** -------------------------------------------------- */
      (void) rfc_init_card( device,band );

      #ifdef RF_HAS_SUPPORT_FOR_RICO_BRINGUP
#error code not present
      #endif /*RF_HAS_SUPPORT_FOR_RICO_BRINGUP*/
      
      rf_wu_synth_start (device, rf_chain_ptr->rf_mode);

#ifdef RF_TIME_PROFILING
#error code not present
#endif /* RF_TIME_PROFILING */

#ifdef RF_HAS_RXADC_VOLTAGE_SWITCH
      rf_warmup_time_rxadc_stop = timetick_get_elapsed( rf_warmup_time_rxadc_start, T_USEC ); /*Capture the time elapsed*/

#ifdef RF_VREG_S2_DEBUG_MSGS 
#error code not present
#endif

      if ( rf_warmup_time_rxadc_stop < RF_RXADC_WAIT_TIME )
      {
        rxadc_delta_warm_up_time = RF_RXADC_WAIT_TIME - rf_warmup_time_rxadc_stop;
        MSG_ERROR("Need extra ADC warm up time:%d Usec for Band: %d Chan: %d ", rxadc_delta_warm_up_time, band, chan );
                    
        rf_set_periodic_wait_timer ( rxadc_delta_warm_up_time );
      } 

      /* Setting the RF_VREG_S2_STOP_VOLTAGE voltage */
      pm_level_error |= pm_vreg_set_level( PM_VREG_S2_ID, RF_VREG_S2_STOP_VOLTAGE );

      if(pm_level_error != PM_ERR_FLAG__SUCCESS)
      {
        MSG_ERROR("Unsuccessful in setting the RF_VREG_S2_STOP_VOLTAGE", 0, 0, 0);
      }     

      rf_set_periodic_wait_timer( RF_VREG_S2_STOP_SETTLE_TIME ); //  Wait for Vreg voltage to settle


#ifdef RF_VREG_S2_DEBUG_MSGS 
#error code not present
#endif

#endif
      
#if defined(FEATURE_PMIC_MANAGED_LDO) && defined(RF_MANAGE_SYNTH_LDO)
      /* ------------------------------------------------------
      ** Coming from sleep and SYNTH LDO has been turned off.
      ** We need to reprogram the plls.
      ** ----------------------------------------------------- */
      rfc_tune_synth(device, rf_chain_ptr->rf_chan );
#else
      if ( chan_changed == TRUE ) 
      {
        rfc_tune_synth(device, rf_chain_ptr->rf_chan );
      }

#endif /* FEATURE_PMIC_MANAGED_LDO && RF_MANAGE_SYNTH_LDO */

#ifdef RF_HAS_SUPPORT_FOR_RICO_BRINGUP
#error code not present
#endif /*RF_HAS_SUPPORT_FOR_RICO_BRINGUP*/

#ifdef RF_HAS_POWER_MODE_LIB
      /* If this execution path was called from sleep, rf_intelliceiver_pause()
         was skipped earlier, and rf_intelliceiver_resume() need not be called
         at this point.  Only call resume() if the RF path was already awake. */
      if ( rf_intelliceiver_is_enabled(rfcom_to_path[device]) )
      {
        /* Resume IntelliCeiver operation if the reciever was not asleep
           to start with */
        if ( from_sleep == FALSE )
        {
          rf_intelliceiver_resume( rfcom_to_path[device] );
        }
      }
#endif /* RF_HAS_POWER_MODE_LIB */

      /* -----------------------------------------------------
      **  Wait minimum synth lock time before waking up the rfr
      ** ----------------------------------------------------- */
      rf_wait_for_synth_lock(device); 

#ifdef RF_HAS_IM2
      /* -----------------------
      **  Load the IM2 values
      ** ----------------------- */
      rf_wu_load_im2(path, rf_chain_ptr);
#endif /* RF_HAS_IM2 */

#ifdef RF_TIME_PROFILING
#error code not present
#endif

 #ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO
       if(rf_chain_ptr->rf_curr_cdma_band != RF_GPS_BAND)
       {
#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */
        {
           rf_adc_clk_status.mode[path] = CDMA_MODE;
        }



     rf_1x_chain_switch_adc_clk_set_rxf(path,VCO_ON,DIV_OFF,rf_adc_clk_status.mode[path]);

     }
#endif/*RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO*/

      /* -----------------------------------------------------------
      ** Prepare for final CDMA wakeup
      ** RxAGC done in rf_complete_sleep_warmup()
      ** ----------------------------------------------------------- */
      rf_cdma_sleep_wakeup_prep (device);

      #ifdef RF_HAS_SUPPORT_FOR_DC_OFFSET_CAL      
      rfr_initiate_dc_offset_cal(path);       
      #endif

#ifdef RF_HAS_SUPPORT_FOR_LO_PWC_CAL
      rfr_adjust_lo_pwc_dac_codes( path );
#endif //RF_HAS_SUPPORT_FOR_LO_PWC_CAL

#ifdef RF_HAS_WU_OPTIMIZED    
#error code not present
#endif /* RF_HAS_WU_OPTIMIZED */

    break; /* RFCOM_TRANSCEIVER_0 */
  
  case RFCOM_RECEIVER_1:

#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */

    /* Set system mode to 1x (if we are on primary chain) */

#if !defined ( RF_HAS_BYPASS_BAND_CHANNEL_CHECK )    
    /* Check for band and channel changes */
      #ifdef RF_HAS_SUB_BLOCK_SUPPORT
      if (( rf_chain_ptr->rf_curr_cdma_band == band ) && (block_changed == FALSE))
      #else
    if ( rf_chain_ptr->rf_curr_cdma_band == band )
      #endif    
    {
       band_changed = FALSE;
       if ( rf_chain_ptr->rf_chan == chan ) 
       {
         chan_changed = FALSE;
       }
    }
#endif

    /* Indicate that we are not completely done; we still have to wait
    ** for warmup to complete before we switch back to CDMA mode.
    */
    rf_dig_init_complete = FALSE;

     #ifdef RF_HAS_SUB_BLOCK_SUPPORT
     //if(band_changed == TRUE)
     //{   
       rfnv_update_band_digital_items_ptr(device,band,digital_item_ptr);
       rfc_set_block(device,band,digital_item_ptr->rfnv_block_mask);
     //}     
     #endif

    rf_digital_band_chan_init( device, band_changed, chan_changed, band, chan);

    /* --------------------------------------------------
    ** Initialize the RF card to the proper band whether
    ** or not we are coming from sleep or a band change
    ** -------------------------------------------------- */
    (void) rfc_init_card( device,band );

    /* ------------------------------------------
    ** Enable rx pll in rft
    ** ----------------------------------------- */
    synth_rfrx_pll_enable( device, rf_chain_ptr->rf_mode );

    /* ------------------------------------------
    ** Enable PLL buffer in rfr
    ** ----------------------------------------- */
    rfrx_pll_buffer_enable( device );

#if defined(FEATURE_PMIC_MANAGED_LDO) && defined(RF_MANAGE_SYNTH_LDO)
    /* ------------------------------------------------------
    ** Coming from sleep and SYNTH LDO has been turned off.
    ** We need to reprogram the plls.
    ** ----------------------------------------------------- */
    rfc_tune_synth(device, rf_chain_ptr->rf_chan );
#else
    if ( chan_changed == TRUE ) 
    {
      rfc_tune_synth(device, rf_chain_ptr->rf_chan );
    }
#endif /* FEATURE_PMIC_MANAGED_LDO && RF_MANAGE_SYNTH_LDO */

    /* -----------------------------------------------------
    **  Wait minimum synth lock time before waking up the rfr
    ** ----------------------------------------------------- */
    rf_wait_for_synth_lock(device); 

#ifdef RF_HAS_IM2
    /* Load the IM2 values */
#ifdef RF_HAS_POWER_MODE_LIB
    (void)rfr_I_Q_dac_set (path,
                   rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_i_value[path],
                   rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_q_value[path],
                   rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_transconductor_value[path]);
#else
    (void)rfr_I_Q_dac_set (path,
                   &rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_i_value[path],
                   &rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_q_value[path],
                   &rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_transconductor_value[path]);
#endif /* RF_HAS_POWER_MODE_LIB */
#endif /* RF_HAS_IM2 */

#ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO
       if(rf_chain_ptr->rf_curr_cdma_band != RF_GPS_BAND)
       {
#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */
         {
           rf_adc_clk_status.mode[path] = CDMA_MODE;
         }


      rf_1x_chain_switch_adc_clk_set_rxf(path,VCO_ON,DIV_OFF, rf_adc_clk_status.mode[path]);
       }
#endif/*RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO*/

    /* -----------------------------------------------------------
    ** Prepare for final CDMA wakeup
    ** ----------------------------------------------------------- */
    rf_cdma_sleep_wakeup_prep (device);

    break;
  
  case RFCOM_RECEIVER_DIV:
  case RFCOM_RECEIVER_DUAL:

    MSG_ERROR( "Device %d not supported!!!", device, 0, 0 );
    break;
  
  default:
    RF_FATAL_ERROR("Device not supported!!!");
    break;

  }

  #ifdef RF_TIME_PROFILING
#error code not present
  #endif /* RF_TIME_PROFILING */

  #ifdef RF_HAS_WU_TIMED

  #ifdef RF_HAS_WU_TIMED_DEBUG
#error code not present
  #else
  (void)rf_prof_timer_stop(path);
  #endif /* RF_HAS_WU_TIMED_DEBUG */
  #endif /* RF_HAS_WU_TIMED */

  return (rx_agc_wait_time);

} /* rf_init_digital_band_chan */



/*===========================================================================

FUNCTION RF_DIGITAL_BAND_CHAN_INIT

DESCRIPTION
  Initializes internal RF variables for digital mode. If band_changed is TRUE
  internal rf variables related to the band desired are initialized.  If
  chan_changed is TRUE internal rf variables related to the band desired are
  initialized.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Updates database with new rf_mode

===========================================================================*/
static void rf_digital_band_chan_init
(
    rfcom_device_enum_type device,
    boolean band_changed,
    boolean chan_changed,
    rf_card_band_type band,
    word chan
)
{

    db_items_value_type db_value;  /* temp for database item DB_RF_MODE */
    rf_path_enum_type path = rfcom_to_path[ device ];

    rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];

    switch ( device )
    {
    case RFCOM_TRANSCEIVER_0:
    case RFCOM_RECEIVER_1:

      /* update rf_curr_cdma_band if band has changed */
      if ( band_changed == TRUE )
      {
        rf_chain_ptr->rf_prev_cdma_band = rf_chain_ptr->rf_curr_cdma_band;
        rf_chain_ptr->rf_curr_cdma_band = band;
        #ifdef RF_HAS_SUB_BLOCK_SUPPORT        
        rf_chain_ptr->rf_nv_dig_items_ptr = rf_digital_nv_table[path][band];
        #else                        
        rf_chain_ptr->rf_nv_dig_items_ptr = rf_digital_nv_table[band];
        #endif          

#ifdef RF_HAS_PA_DVS_NV
        /* 
        * Need to re-configure PA DVS setting in case different band uses different
        * PA types and potentially different DVS voltage settings.  Chain0 for
        * transmission band change only.
        */
        if( device == RFCOM_TRANSCEIVER_0)
        {
          rf_config_pa_dvs();
        }
#endif /* RF_HAS_PA_DVS_NV */

       #if defined (RF_HAS_MDSP_TX_AGC) || defined(RF_HAS_PDM_VS_POWER_TX_CAL)
       if ( ( device == RFCOM_TRANSCEIVER_0) && (rf_chain_ptr->rf_curr_cdma_band !=RF_GPS_BAND) )
       {
         rf_cdma_load_master_lin_tables = TRUE;
       }
       #endif
      }

      /* --------------------------------------------------
      ** update rf_mode and NV items pointer
      ** -------------------------------------------------- */
      switch(rf_chain_ptr->rf_curr_cdma_band)
      {
      case RF_BC0_BAND:/*   US Cellular    */
      case RF_BC3_BAND:/*   JCDMA          */
      case RF_BC10_BAND:
          rf_chain_ptr->rf_mode = DB_RF_CDMA;
          break;

      case RF_BC1_BAND: /*   US PCS         */
      case RF_BC4_BAND: /*   Korean PCS     */
      case RF_BC5_BAND: /*   NMT            */
      case RF_BC6_BAND: /*   IMT (2.1GHz)   */
      case RF_BC11_BAND:/*   BC11           */
      case RF_BC14_BAND:/*   US PCS + 5MHz  */
      case RF_BC15_BAND:/*   AWS Band       */
          rf_chain_ptr->rf_mode = DB_RF_PCN;
          break;

      #ifdef RF_HAS_BYPASS_RF_STATE_GPS_CHECK
      case RF_GPS_BAND:
        rf_chain_ptr->rf_mode = DB_RF_GPS;
        break;
      #endif
      
      default:
          RF_FATAL_ERROR("Invalid RF mode.");
      }

      /* update the external database for primary chain only */
      if( device == RFCOM_TRANSCEIVER_0)
      {
        db_value.rf_mode = rf_chain_ptr->rf_mode;
        db_put( DB_RF_MODE, &db_value );
      }

      /* set up AGC for digital mode */
      rf_hw_lna_setup_digital(device);

      if ( band_changed == TRUE )
      {
        /*
        ** need to reprogram for both a band & channel change
        */
        rf_digital_band_change( device, band );

        rf_digital_chan_change(device, chan );

      }
      else
      {
        if ( chan_changed == TRUE )
        {
          /* update internal variables based on channel */
          rf_digital_chan_change(device, chan );
        }
        else
        {
          /* update LNA offsets based on frequency */
          rf_update_lna_offsets(device);
        }

      }

      break;

    case RFCOM_RECEIVER_DIV:
    case RFCOM_RECEIVER_DUAL:

      MSG_ERROR( "Device %d not supported!!!", device, 0, 0 );
      break;

    default:
      RF_FATAL_ERROR("Unknown Device!!!");
      break;

    }

    return;

}



/*===========================================================================

FUNCTION RF_DIGITAL_BAND_CHANGE

DESCRIPTION
  Initializes internal RF variables to the settings for the desired digital
  band

DEPENDENCIES
  1) rf_curr_cdma_band and rf_mode must already be set to the desired band
     upon entering this function.
  2) rf_nv_dig_items_ptr should be pointing to the correct NV structure for
     the band
  3) rf_hw_lna_setup_digital() must have been called to initialize the CDMA
     HW AGC hardware

RETURN VALUE
  None.

SIDE EFFECTS
  Updates internal RF driver variables that are band dependent

  RF card settings ARE NOT programmed for the band
===========================================================================*/
static void rf_digital_band_change
(
    rfcom_device_enum_type device,
    rf_card_band_type band
)
{

    rf_path_enum_type path = rfcom_to_path[ device ];
    rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];

    switch ( device )
    {
    case RFCOM_TRANSCEIVER_0:

      switch( band )
      {
      /* Change to PCS band   */
      case RF_BC1_BAND: /*   US PCS         */
      case RF_BC4_BAND: /*   Korean PCS     */
      case RF_BC5_BAND: /*   NMT            */
      case RF_BC6_BAND: /*   IMT (2.1GHz)   */
      case RF_BC11_BAND:/*   BC11           */
      case RF_BC14_BAND:/*   US PCS + 5MHz  */
      case RF_BC15_BAND:/*   AWS Band       */
        if ( rf_chain_ptr->rf_mode != DB_RF_PCN )
        {
            RF_FATAL_ERROR("Invalid RF mode for PCS BAND.");
        }


        break;

      /* Change to CDMA band */
      case RF_BC0_BAND:/*   US Cellular    */                        
      case RF_BC3_BAND:/*   JCDMA          */
      case RF_BC10_BAND:
      
        if ( rf_chain_ptr->rf_mode != DB_RF_CDMA )
        {
              RF_FATAL_ERROR("Invalid RF mode for RF_BC0_BAND.");
        }


        break;

      /* Unknown band */
      default:
        RF_FATAL_ERROR("Invalid RF band.");
      }


      /* Program the BTF counter to compensate for delay for 1900MHz      */
#if defined(FEATURE_ENC_BTF_IN_NV)
      enc_init_pa(TRUE, rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_enc_btf_dly);
#else
      enc_init_pa(TRUE);
#endif

#ifndef SRCH_HAS_PILOT_GAIN_CNTL
      /* Set Pilot filter gain to 764 Hz BW. */
      srch_fing_set_dmod_pilot_gain( band );
#endif /* !SRCH_HAS_PILOT_GAIN_CNTL */

      /* setup the rxf front based on rf_mode */
      rxf_program_mode_cdma(path, band );

#ifdef RF_HAS_PA_DVS_NV
       /* 
       * Need to re-configure PA DVS setting in case different band uses different
       * PA types and potentially different DVS voltage settings.  Chain0 for
       * transmission band change only.
       */
       rf_config_pa_dvs();
#endif /* RF_HAS_PA_DVS_NV */
      break;

    case RFCOM_RECEIVER_1:

        switch( band )
        {
        /* Change to PCS band   */
        case RF_BC1_BAND: /*   US PCS         */
        case RF_BC4_BAND: /*   Korean PCS     */
        case RF_BC5_BAND: /*   NMT            */
        case RF_BC6_BAND: /*   IMT (2.1GHz)   */
        case RF_BC11_BAND:/*   BC11           */
        case RF_BC14_BAND:/*   US PCS + 5MHz  */
        case RF_BC15_BAND:/*   AWS Band       */
          if ( rf_chain_ptr->rf_mode != DB_RF_PCN )
          {
              RF_FATAL_ERROR("Invalid RF mode for PCS BAND.");
          }

          break;

        /* Change to CDMA band */
        case RF_BC0_BAND:/*   US Cellular    */                        
        case RF_BC3_BAND:/*   JCDMA          */
        case RF_BC10_BAND:
          if ( rf_chain_ptr->rf_mode != DB_RF_CDMA )
          {
                RF_FATAL_ERROR("Invalid RF mode.");
          }

          break;

        /* Unknown band */
        default:
          RF_FATAL_ERROR("Invalid RF band.");
        }

        /* setup the rxf front based on rf_mode */
        rxf_program_mode_cdma(path, band );

        break;

    case RFCOM_RECEIVER_DIV:
    case RFCOM_RECEIVER_DUAL:
      MSG_ERROR( "Device %d not supported!!!", device, 0, 0 );
        break;

    default:
      RF_FATAL_ERROR( "Unknown Device!!!");
      break;

    }

}



/*===========================================================================

FUNCTION RF_DIGITAL_CHAN_CHANGE

DESCRIPTION
  This function peforms all setup required for a channel change without
  actually tuning the PLLs to the new channel

DEPENDENCIES
  rf_state must be setup
  rf_nv_dig_items_ptr must be set to the band the channel is in

RETURN VALUE
   None

SIDE EFFECTS
  Updates database item DB_CHANNEL to the new channel
  Updates rf_freq_index

===========================================================================*/
static void rf_digital_chan_change
(
    rfcom_device_enum_type device,
    word chan
)
{
    db_items_value_type db_value; /* temp for database item DB_CHANNEL */
    rf_path_enum_type path = rfcom_to_path[ device ];

    rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];

    if (path >= RF_PATH_MAX)
    {
       MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
       rf_error_flag = TRUE;
       return;
    }
    
#if defined (FEATURE_TRIMODE_ITEMS) && defined (RF_LOG_BAND_CHANGE)
    log_band_class_chng_type *band_log_ptr; /*  band class info */
    band_log_struct_type  *bl_ptr;
#endif

#ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO    
#ifdef RF_HAS_ADC_SAMPLING_CLK_DEBUG
#error code not present
#endif
#endif

    switch ( device )
    {
    case RFCOM_TRANSCEIVER_0:
      /* --------------------------------------------------------------
      ** Make changes required for channel changes in RF_STATE_CDMA_RX
      ** -------------------------------------------------------------- */
      switch (rf_chain_ptr->rf_state)
      {

      #ifdef RF_HAS_BYPASS_RF_STATE_GPS_CHECK
      case RF_STATE_CDMA_RXTX:
      #endif
      case RF_STATE_CDMA_RX:

        /* --------------------------------------------
        ** Update external database with channel number
        ** -------------------------------------------- */
        rf_chain_ptr->rf_chan = chan;
        db_value.channel = rf_chain_ptr->rf_chan;
        db_put( DB_CHANNEL, &db_value );

        /* ------------------------------------------------------------
        ** Calculate new index for NV frequency tables based on channel
        ** ------------------------------------------------------------ */
        rf_chain_ptr->rf_freq_index = rf_freq_index_calc( chan,
                                                          rf_chain_ptr->rf_curr_cdma_band );

#ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO
       if(rf_chain_ptr->rf_curr_cdma_band != RF_GPS_BAND)
       {
#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */
         {
           rf_adc_clk_status.mode[path] = CDMA_MODE;
         }


         rf_1x_chain_switch_adc_clk_set_rxf(path,VCO_WARMUP,DIV_OFF,rf_adc_clk_status.mode[path]);

       }
#endif /* RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO */

#ifdef RF_HAS_LINEAR_INTERPOLATION
        rf_update_freq_comp_items( path, chan);
#endif

        /* Calculate rf_hdet_off_adj and rf_hdet_spn_adj every time the
        ** channel changes.
        */
        rf_hdet_off_adj = rf_hdet_adj_calc( rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_hdet_off );

        #ifdef RF_HAS_HDET_UINT16
        rf_hdet_spn_adj =
          rf_hdet_adj_calc( (uint16)( rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_hdet_off
                              + rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_hdet_spn) ) - rf_hdet_off_adj;
        #else
        rf_hdet_spn_adj =
           rf_hdet_adj_calc( (byte)( rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_hdet_off
                              + rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_hdet_spn) ) - rf_hdet_off_adj;
        #endif


        /* Update RxFront DVGA Gain Offset to new frequency */
        rxf_set_vga_gain_offset(path, rf_chain_ptr->rf_freq_index);


        /* Update LNA offsets according to new frequency */
        rf_update_lna_offsets(device);

        /* -----------------------------------------------------------
        ** Reload the linearizers, we've changed the frequency and
        ** we need to load linearizers compensated for this frequency.
        ** ----------------------------------------------------------- */
        rf_load_cagc_linearizers(device);

        break;

      case RF_STATE_SLEEP_1:
      case RF_STATE_SLEEP_2:
      #ifndef RF_HAS_BYPASS_RF_STATE_GPS_CHECK
      case RF_STATE_CDMA_RXTX:
      case RF_STATE_GPS:
      #endif
      default:
        RF_FATAL_ERROR("Invalid RF state.");
      }

      break;

    case RFCOM_RECEIVER_1:
        /* --------------------------------------------------------------
        ** Make changes required for channel changes in RF_STATE_CDMA_RX
        ** -------------------------------------------------------------- */
        switch (rf_chain_ptr->rf_state)
        {

        #ifdef RF_HAS_BYPASS_RF_STATE_GPS_CHECK
        case RF_STATE_CDMA_RXTX:
        #endif
        case RF_STATE_CDMA_RX:

          /* --------------------------------------------
          ** Update chain in the structure.
          ** -------------------------------------------- */
          rf_chain_ptr->rf_chan = chan;

          /* ------------------------------------------------------------
          ** Calculate new index for NV frequency tables based on channel
          ** ------------------------------------------------------------ */
          rf_chain_ptr->rf_freq_index = rf_freq_index_calc( chan,
                                                            rf_chain_ptr->rf_curr_cdma_band );
#ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO

          if(rf_chain_ptr->rf_curr_cdma_band != RF_GPS_BAND)
          {
#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */
           {
            rf_adc_clk_status.mode[path] = CDMA_MODE;
           }

           rf_1x_chain_switch_adc_clk_set_rxf(path,VCO_WARMUP,DIV_OFF,rf_adc_clk_status.mode[path]);

          }
#endif /* RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO */

  #ifdef RF_HAS_LINEAR_INTERPOLATION
          rf_update_freq_comp_items( path, chan);
  #endif


          /* Update RxFront DVGA Gain Offset to new frequency */
          rxf_set_vga_gain_offset(path, rf_chain_ptr->rf_freq_index);


          /* Update LNA offsets according to new frequency */
          rf_update_lna_offsets(device);

          break;

        case RF_STATE_SLEEP_1:
        case RF_STATE_SLEEP_2:
        #ifndef RF_HAS_BYPASS_RF_STATE_GPS_CHECK
        case RF_STATE_GPS:
        case RF_STATE_CDMA_RXTX:
        #endif
        default:
          RF_FATAL_ERROR("Invalid RF state.");
        }

        break;

    case RFCOM_RECEIVER_DIV:
    case RFCOM_RECEIVER_DUAL:
      MSG_ERROR( "Device %d not supported!!!", device, 0, 0 );
      break;

    default:
      RF_FATAL_ERROR("Unknown Device.");
      break;

    }

#ifdef FEATURE_TRIMODE_ITEMS
#ifdef RF_LOG_BAND_CHANGE
    /* Fill up the log packet and send it */
   if ( (bl_ptr = (band_log_struct_type *)q_get ( &band_scan_buf_q )) != NULL )
   {

      band_log_ptr = &bl_ptr->entry;
      band_log_ptr->hdr.len = sizeof(log_band_class_chng_type);
      band_log_ptr->hdr.code = LOG_BAND_CHANGE_C;
      band_log_ptr->band_class = (byte)rf_curr_cdma_band;
      band_log_ptr->channel = chan;

      ts_get( (unsigned long *) band_log_ptr->hdr.ts );
   }
   log_put ( &bl_ptr->desc );

   bl_ptr= NULL;
#endif /* RF_LOG_BAND_CHANGE */
#endif /* FEATURE_TRIMODE_ITEMS */

}




/*===========================================================================

FUNCTION RF_SET_MDSP_CDMA_SPECTRAL_INVERSION            EXTERNALIZED FUNCTION

DESCRIPTION
  This function sets the CDMA spectral inversion bit in the MDSP for targets
  that support it.

DEPENDENCIES
  DSP must be downloaded and running.  Cannot be called while a search is in
  progress

RETURN VALUE
   None

SIDE EFFECTS
  None

===========================================================================*/
void rf_set_mdsp_cdma_spectral_inversion( void )
{
#ifdef RF_DSP_HAS_RX_SPECTRAL_INV_CONFIG
/* ----------------------------------------------------
** Initialize RX spectral inversion bit in mDSP
** ---------------------------------------------------- */
#if (BSP_RX_DSP_SPECTRAL_INVERSION_CDMA==1)
  mdsp_write(SRCH_sampRamCfgTemp, DSP_RX_SPECTRAL_INV);
#else
  mdsp_write(SRCH_sampRamCfgTemp, DSP_RX_SPECTRAL_NORM);
#endif
#endif /* RF_DSP_HAS_RX_SPECTRAL_INV_CONFIG */
}

#ifdef RF_DBG_SCRN
#error code not present
#endif /* RF_DBG_SCRN */

#ifdef RF_HAS_LINEAR_INTERPOLATION

/*===========================================================================

FUNCTION UPDATE_FREQ_COMP_ITEMS                             EXTERNAL FUNCTION

DESCRIPTION
  Function to interpolate frequency comp items from the channel number.

DEPENDENCIES
  Only applies for US Cellular and PCS mode.

RETURN VALUE
  none

SIDE EFFECTS
  Updates global variables: lin_freq_comp[],

===========================================================================*/

void rf_update_freq_comp_items
(
  rf_path_enum_type path, word channel          /* input channel number */
)
{

  int16 tx_start_bin = 0;   /* starting TX cal channel for bin containing */
  int16 rx_start_bin = 0;   /* starting RX cal channel for bin containing */
  int32 tx_bin_size;        /* number of channels in the bin (not constant) */
  int32 rx_bin_size;        /* number of channels in the bin (not constant) */
  int32 tx_delta_x;         /* scale of channel number in bin */
  int32 rx_delta_x;         /* scale of channel number in bin */
  int16 delta_y;            /* used to store delta_y between adjacent nv items*/
  int16 lin_tbl;            /* increment value for linearizer values */
  boolean tx_channel_under = 0; /* flag if lowest TX cal channel > tune value */
  boolean rx_channel_under = 0; /* flag if lowest RX cal channel > tune value */
  byte index = 0;           /* for the converted channel list table */
  uint32 loc_chan = (uint32)channel; /* local channel will be uint32 */
  int32 delta_y_buffer;     /* a large storage temp variable to keep track
                              of the quantity delta_y*delta_x/bin_size in
                              order to avoid overflow */
#ifdef RF_HAS_POWER_MODE_LIB
  int power_mode;           /* Power mode iterator */
#endif /* RF_HAS_POWER_MODE_LIB */

  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];

  if (path >= RF_PATH_MAX)
  {
     MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
     rf_error_flag = TRUE;
     return;
  }

  #ifdef INTERPOLATION_PROFILING
#error code not present
  #endif

  /* check if the channel passed in is the same channel (from the same
     band) that was passed in earlier.  If so, return, no need to
     calculate again.  The first time through this function
     rf_freq_comp.last_chan_checked will not yet have a value and the
     if statement should fail fall through to execute the function */
  #ifndef RF_HAS_SUB_BLOCK_SUPPORT
  if ((channel == rf_freq_comp.last_chan_checked[path]) &&
      (rf_chain_ptr->rf_curr_cdma_band == rf_freq_comp.last_band_checked[path])
      && (path==rf_freq_comp.last_chain_checked))
  #else
   if ((channel == rf_freq_comp.last_chan_checked[path]) &&
       (rf_chain_ptr->rf_curr_cdma_band == rf_freq_comp.last_band_checked[path])
        && (path==rf_freq_comp.last_chain_checked)
        &&(rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_block_mask == rf_freq_comp.last_block_mask_checked[path]))
  #endif
  {
    #ifdef INTERPOLATION_PROFILING
#error code not present
    #endif
    return;
  }


  /* the first time through this function we want to convert the entire
     channel list to frequency and store it in the table.  After that we
     want to reference our frequency values from the table,
     rather than calculate them each time.  All this- only if
     we are in the same band, otherwise we will calculate again.*/
  #ifndef RF_HAS_SUB_BLOCK_SUPPORT
  if ((!rf_freq_comp.channel_list_converted_flag[path]) ||
      (rf_chain_ptr->rf_curr_cdma_band != rf_freq_comp.last_band_checked[path]))
  #else
  if ((!rf_freq_comp.channel_list_converted_flag[path]) ||
       (rf_chain_ptr->rf_curr_cdma_band != rf_freq_comp.last_band_checked[path]) ||
        (rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_block_mask != rf_freq_comp.last_block_mask_checked[path]))

  #endif
  {
    while (index < (NV_FREQ_TABLE_SIZ))
    {

      if (path == RF_PATH_0) // only one tx path is supported
      {
        /* convert the TX channel list */
        rf_freq_comp.converted_tx_channel_list_arr[index] =
            rf_interpolate_chan_offset( path,
            rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_tx_cal_chan[path][index]);
      }

      /* convert the RX channel list */
      rf_freq_comp.converted_rx_channel_list_arr[path][index] =
            rf_interpolate_chan_offset( path,
            rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_rx_cal_chan[path][index]);

      index ++;
    }

    /* set the flag to indicate that we've already converted the channels to
    frequency so that we don't need to do it again. */

    rf_freq_comp.channel_list_converted_flag[path]  = TRUE;
  }


  /* update the last channel and band checked with the channel passed in */

  rf_freq_comp.last_chan_checked[path] = channel;
  rf_freq_comp.last_band_checked[path] = rf_chain_ptr->rf_curr_cdma_band;

  #ifdef RF_HAS_SUB_BLOCK_SUPPORT
  rf_freq_comp.last_block_mask_checked[path] = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_block_mask;
  #endif

  rf_freq_comp.last_chain_checked = path;


  /* convert dis-joint cdma channel scheme to continuious.
     The cal channels are NV items and could be initilized
     to continuous,rather than calculating each time. */

  loc_chan =  rf_interpolate_chan_offset( path, channel );

/**************************************************************************

   PATH 0 - TX ITEMS

***************************************************************************/
  /* only handle TX items for path 0 */
  if(path == RF_PATH_0)
  {

    /* check your local channel against the 1st (0th) element of the array.
       if it's smaller than the first element then we use the lim_vs_freq
       value of the first element.  If it's not smaller than the first element
       then we start traversing the array to find out which bin it belongs to.*/
    if (rf_freq_comp.converted_tx_channel_list_arr[0] == 0 ||
        loc_chan <= rf_freq_comp.converted_tx_channel_list_arr[0])
    {
      tx_start_bin = 0;
      tx_channel_under ++;   /* set flag if we are under the cal table
                                range flatline the value. */
    }


    /* search for frequency bin that contains the channel
       that is being compensated
       or if the bin contains zero we went off the table, use the last value */

    /* supports varying bin size, also used for detecting going past last
       cal point, when < 16 cal channels and remaining channel list stuffed
       with 0's. (negative bin size) */
    else
    {
      while (tx_start_bin < (NV_FREQ_TABLE_SIZ-1))
      {
        /* note: tables with less than 16 elements in them must be
           padded with 0's */
        if (rf_freq_comp.converted_tx_channel_list_arr[tx_start_bin + 1] == 0 ||
            loc_chan <
            rf_freq_comp.converted_tx_channel_list_arr[tx_start_bin +1])

        {
          break;            /* exit*/
        }
        else
        {
          tx_start_bin ++;     /* check next bin*/
        }
      }
    }


    /* if you've traversed through the entire array already (tx_start_bin ==15)
       then just set the bin size to 0 so that you will just use the lim_vs_freq
       value of the last element, rather than calculating it */

      if(rf_freq_comp.converted_tx_channel_list_arr[tx_start_bin + 1] == 0 ||
       tx_start_bin == (NV_FREQ_TABLE_SIZ -1))
      {
        tx_bin_size = 0;
      }
      else
    {
      tx_bin_size=(int32)(rf_freq_comp.converted_tx_channel_list_arr[tx_start_bin+1]-
                          rf_freq_comp.converted_tx_channel_list_arr[tx_start_bin]);
      }
      /* Load the comp items with the end point values if we are outside the
         cal channel list
         else the second statement when we interpolate.
         also avoid divide by zero error if two bins have the same channel #
         (we divide by bin size) */
  
      /* These are the TX items */
  
      /* outside the array case  or channel passed in is part of the channel list*/
      if ((tx_bin_size <= 0) || 
          (tx_start_bin == (NV_FREQ_TABLE_SIZ-1)) ||
          (tx_channel_under == 1) || 
          (loc_chan == rf_freq_comp.converted_tx_channel_list_arr[tx_start_bin]))
      {
        /* update tx freq comp items */
        for (lin_tbl=0; lin_tbl<RF_NUM_TX_LINEARIZERS; lin_tbl++)
        {
          #ifdef RF_HAS_TX_TEMP_FREQ_COMP_NV_BASED_ON_PWR
  
          if((rf_path_0.rftdev==RTR6500_TX)||
             (rf_path_0.rftdev==QSC60X5_TX)||
             (rf_path_0.rftdev==QSC11X0_TX))
          {
            rf_freq_comp.i_lin_freq_comp[lin_tbl] =
             rf_chain_ptr->rf_nv_dig_items_ptr->
             rfnv_tx_power_comp[lin_tbl].rfnv_tx_pwr_comp_vs_freq[tx_start_bin];
          }
          else
          {
            rf_freq_comp.i_lin_freq_comp[lin_tbl] =
            rf_chain_ptr->rf_nv_dig_items_ptr->
            rfnv_cdma_tx_comp_vs_freq[lin_tbl][tx_start_bin];
  
          }
          #else
           rf_freq_comp.i_lin_freq_comp[lin_tbl] =
           rf_chain_ptr->rf_nv_dig_items_ptr->
           rfnv_cdma_tx_comp_vs_freq[lin_tbl][tx_start_bin];
          #endif
        }
        rf_freq_comp.i_tx_lim_vs_freq =
        rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_tx_lim_vs_freq[tx_start_bin];
      }
      else /* we're within our cal chan list, do the interpolation calculation */
      {
        /*  ensures that dx/binsize <1 at bin boundary, when delta_y = 1. */
        /* delta_x is the distance from your local channel to the beginning of the
           bin, -1 */
        tx_delta_x = (int32)(loc_chan -
                     rf_freq_comp.converted_tx_channel_list_arr[tx_start_bin]);
  
        /* update TX COMP_VS_FREQ items */
  
        for (lin_tbl=0; lin_tbl<RF_NUM_TX_LINEARIZERS; lin_tbl++)
        {
           /* delta_y is the difference between the comp_vs_freq values
              of the bin edges to the left and right of our loc_chan */
           #ifdef RF_HAS_TX_TEMP_FREQ_COMP_NV_BASED_ON_PWR
           if((rf_path_0.rftdev == QSC60X5_TX)||(rf_path_0.rftdev==RTR6500_TX)||
             (rf_path_0.rftdev==QSC11X0_TX))
           {
             /*if tx_start_bin == NV_FREQ_TABLE_SIZ-1, we wouldn't be in this else
               section of code, but this is just to quiet KW*/
             if (tx_start_bin >= NV_FREQ_TABLE_SIZ-1) 
             {
                MSG_FATAL("Index will cause array out-of-bounds", 0, 0, 0);
                rf_error_flag = TRUE;
                return;
             }
    
             delta_y = rf_chain_ptr->rf_nv_dig_items_ptr->
                     rfnv_tx_power_comp[lin_tbl].rfnv_tx_pwr_comp_vs_freq[tx_start_bin+1]-
                     rf_chain_ptr->rf_nv_dig_items_ptr->
                     rfnv_tx_power_comp[lin_tbl].rfnv_tx_pwr_comp_vs_freq[tx_start_bin];     
           }
           else
           {
                  delta_y = rf_chain_ptr->rf_nv_dig_items_ptr->
                   rfnv_cdma_tx_comp_vs_freq[lin_tbl][tx_start_bin+1]-
                   rf_chain_ptr->rf_nv_dig_items_ptr->
                   rfnv_cdma_tx_comp_vs_freq[lin_tbl][tx_start_bin];
           }
           #else
           delta_y = rf_chain_ptr->rf_nv_dig_items_ptr->
                   rfnv_cdma_tx_comp_vs_freq[lin_tbl][tx_start_bin+1]-
                   rf_chain_ptr->rf_nv_dig_items_ptr->
                   rfnv_cdma_tx_comp_vs_freq[lin_tbl][tx_start_bin];
           #endif /* RF_HAS_TX_TEMP_FREQ_COMP_NV_BASED_ON_PWR */ 
  
  
           delta_y_buffer = delta_y*tx_delta_x;  /* 1 symbol is expanded by multipling tx_bin_size */
  
           if ( delta_y != 0 )
           {
             delta_y_buffer = ((delta_y_buffer<0)?(delta_y_buffer-(tx_bin_size+1)/2):(delta_y_buffer+(tx_bin_size+1)/2));  /* round off */
  
             delta_y_buffer = delta_y_buffer/tx_bin_size;
           }
           
         #if (defined(RF_HAS_TX_TEMP_FREQ_COMP_NV_BASED_ON_PWR))
           
          if((rf_path_0.rftdev==RTR6500_TX)||
             (rf_path_0.rftdev==QSC60X5_TX)||
             (rf_path_0.rftdev==QSC11X0_TX))
          {      
           rf_freq_comp.i_lin_freq_comp[lin_tbl] =
             (int16)delta_y_buffer +
           rf_chain_ptr->rf_nv_dig_items_ptr->
           rfnv_tx_power_comp[lin_tbl].rfnv_tx_pwr_comp_vs_freq[tx_start_bin];
          }
          else
          {  rf_freq_comp.i_lin_freq_comp[lin_tbl] =
             (int16)delta_y_buffer +
             rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_tx_comp_vs_freq[lin_tbl]
             [tx_start_bin];
          }
          #else
           rf_freq_comp.i_lin_freq_comp[lin_tbl] =
             (int16)delta_y_buffer +
             rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_tx_comp_vs_freq[lin_tbl]
             [tx_start_bin];
           #endif
        }
  
  
        /* update LIM_VS_FREQ items */
  
        /* delta_y is the difference between the lim_vs_freq values
           of the bin edges to the left and right of our loc_chan */
  
        delta_y = (rf_chain_ptr->rf_nv_dig_items_ptr->
               rfnv_cdma_tx_lim_vs_freq[tx_start_bin+1]-
               rf_chain_ptr->
               rf_nv_dig_items_ptr->rfnv_cdma_tx_lim_vs_freq[tx_start_bin]);
  
        delta_y_buffer = delta_y*tx_delta_x;  /* 1 symbol is expanded by multipling tx_bin_size */
  
        if ( delta_y != 0 )
        {
          delta_y_buffer = ((delta_y_buffer<0)?(delta_y_buffer-(tx_bin_size+1)/2):(delta_y_buffer+(tx_bin_size+1)/2));  /* round off */
  
          delta_y_buffer = delta_y_buffer/tx_bin_size;
        }
  
        rf_freq_comp.i_tx_lim_vs_freq = (int16)delta_y_buffer +
                          rf_chain_ptr->rf_nv_dig_items_ptr->
                          rfnv_cdma_tx_lim_vs_freq[tx_start_bin];
  
      } /* interpolate case */
  } /* end PATH 0 - tx items */

/**************************************************************************

   PATH 1 or 0 - RX ITEMS

***************************************************************************/

  /* do the same thing, checking against the 1st element of the
     RX cal channel list */
  if (rf_freq_comp.converted_rx_channel_list_arr[path][0] == 0 ||
      loc_chan <= rf_freq_comp.converted_rx_channel_list_arr[path][0])
  {
    rx_start_bin = 0;
    rx_channel_under ++;   /* set flag if we are under the cal table
                              range flatline the value. */
  }

  /* search for frequency bin that contains the channel
     that is being compensated
     or if the bin contains zero we went off the table, use the last value */

  /* do the same thing, traversing the RX cal channel list */
  else
  {
    while (rx_start_bin < (NV_FREQ_TABLE_SIZ-1))
    {
      /* note: tables with less than 16 elements in them must be
         padded with 0's */

      if (rf_freq_comp.converted_rx_channel_list_arr[path][rx_start_bin+1] == 0 ||
          loc_chan < rf_freq_comp.converted_rx_channel_list_arr[path][rx_start_bin+1])

      {
        break;            /* exit*/
      }
      else
      {
        rx_start_bin ++;     /* check next bin*/
      }
    }
  }

  /* 
  * Value of rx_start_bin after above  loop ensures that size is 1 less than 
  * table size.
  *
  * This allows suppression of lint warning !e661 rx_start_bin bounds pre-checked
  * on several statements following
  */

    if(rf_freq_comp.converted_rx_channel_list_arr[path][rx_start_bin+1] == 0 ||
       rx_start_bin == (NV_FREQ_TABLE_SIZ -1))
    {
      rx_bin_size = 0;
    }
    else
    {
      rx_bin_size=(int32)(rf_freq_comp.converted_rx_channel_list_arr[path][rx_start_bin+1]-
                          rf_freq_comp.converted_rx_channel_list_arr[path][rx_start_bin]);
    }

    /* These are the RX items */
  
    /* outside the array case  or channel passed in is part of the channel list*/
    if ((rx_bin_size <= 0) || (rx_channel_under == 1) || 
       (loc_chan == rf_freq_comp.converted_rx_channel_list_arr[path][rx_start_bin]))
    {
      /* update rx freq comp items */
#ifdef RF_HAS_POWER_MODE_LIB
      for (power_mode = 0; power_mode < NV_RF_POWER_MODE_SIZ; power_mode++)
      {
        rf_freq_comp.i_vga_gain_offset_vs_freq[path][power_mode] =
                    rf_chain_ptr->rf_nv_dig_items_ptr->
                    rfnv_vga_gain_offset_vs_freq[path][power_mode][rx_start_bin];
  
        rf_freq_comp.i_lna_1_offset_vs_freq[path][power_mode]  =
                    rf_chain_ptr->rf_nv_dig_items_ptr->
                    rfnv_cdma_lna_1_offset_vs_freq[path][power_mode][rx_start_bin];
  
        rf_freq_comp.i_lna_2_offset_vs_freq[path][power_mode] =
                    rf_chain_ptr->rf_nv_dig_items_ptr->
                    rfnv_cdma_lna_2_offset_vs_freq[path][power_mode][rx_start_bin];
  
        rf_freq_comp.i_lna_3_offset_vs_freq[path][power_mode] =
                    rf_chain_ptr->rf_nv_dig_items_ptr->
                    rfnv_cdma_lna_3_offset_vs_freq[path][power_mode][rx_start_bin];
  
        rf_freq_comp.i_lna_4_offset_vs_freq[path][power_mode] =
                    rf_chain_ptr->rf_nv_dig_items_ptr->
                    rfnv_cdma_lna_4_offset_vs_freq[path][power_mode][rx_start_bin];
      }
#else
      rf_freq_comp.i_vga_gain_offset_vs_freq[path] =
                  rf_chain_ptr->rf_nv_dig_items_ptr->
                  rfnv_vga_gain_offset_vs_freq[path][rx_start_bin];
  
      rf_freq_comp.i_lna_1_offset_vs_freq[path]  =
                  rf_chain_ptr->rf_nv_dig_items_ptr->
                  rfnv_cdma_lna_1_offset_vs_freq[path][rx_start_bin];
  
      rf_freq_comp.i_lna_2_offset_vs_freq[path] =
                  rf_chain_ptr->rf_nv_dig_items_ptr->
                  rfnv_cdma_lna_2_offset_vs_freq[path][rx_start_bin];
  
      rf_freq_comp.i_lna_3_offset_vs_freq[path] =
                  rf_chain_ptr->rf_nv_dig_items_ptr->
                  rfnv_cdma_lna_3_offset_vs_freq[path][rx_start_bin];
  
      rf_freq_comp.i_lna_4_offset_vs_freq[path] =
                  rf_chain_ptr->rf_nv_dig_items_ptr->
                  rfnv_cdma_lna_4_offset_vs_freq[path][rx_start_bin];
#endif /* RF_HAS_POWER_MODE_LIB */
    }
    else  /* interpolate the item if it's delta_y is not zero */
    {
      /* ensures that dx/binsize <1 at bin boundary, when delta_y = 1. */
      /* delta_x is the distance from your local channel to the beginning of the
         bin, -1 */
      rx_delta_x = (int32)(loc_chan -
                   rf_freq_comp.converted_rx_channel_list_arr[path][rx_start_bin]);
  
#ifdef RF_HAS_POWER_MODE_LIB
      for (power_mode = 0; power_mode < NV_RF_POWER_MODE_SIZ; power_mode++)
      {
        /* DVGA GAIN OFFSET VS FREQ */
        /* delta_y is the difference between the lim_vs_freq values
           of the bin edges to the left and right of our loc_chan */
  
          /*if rx_start_bin == NV_FREQ_TABLE_SIZ-1, we wouldn't be in this else
            section of code, but this is just to quiet KW*/
          if (rx_start_bin >= NV_FREQ_TABLE_SIZ-1) 
          {
             MSG_FATAL("Index will cause array out-of-bounds", 0, 0, 0);
             rf_error_flag = TRUE;
             return;
          }
    
        delta_y = (rf_chain_ptr->rf_nv_dig_items_ptr->
             rfnv_vga_gain_offset_vs_freq[path][power_mode][rx_start_bin+1]-
             rf_chain_ptr->rf_nv_dig_items_ptr->
             rfnv_vga_gain_offset_vs_freq[path][power_mode][rx_start_bin]);
  
        delta_y_buffer = delta_y*rx_delta_x;
  
        if ( delta_y != 0 )
        {
          delta_y_buffer = (delta_y_buffer<0)?(delta_y_buffer-(rx_bin_size+1)/2):(delta_y_buffer+(rx_bin_size+1)/2);  /* round off */
  
          delta_y_buffer = delta_y_buffer/rx_bin_size;
        }
  
        /* take the ratio of the delta_x to the entire bin size, and apply
           it to the delta_y.  This is the value you need to add to the
           comp_vs_freq value of your left bin edge to get the comp_vs_freq
           value of your loc_chan */
  
        rf_freq_comp.i_vga_gain_offset_vs_freq[path][power_mode] =
             (int16)delta_y_buffer +
             rf_chain_ptr->rf_nv_dig_items_ptr->
             rfnv_vga_gain_offset_vs_freq[path][power_mode][rx_start_bin];
  
        /* LNA OFFSETS VS FREQ */
        delta_y = (rf_chain_ptr->rf_nv_dig_items_ptr->
             rfnv_cdma_lna_1_offset_vs_freq[path][power_mode][rx_start_bin+1]-
             rf_chain_ptr->rf_nv_dig_items_ptr->
             rfnv_cdma_lna_1_offset_vs_freq[path][power_mode][rx_start_bin]);
  
        delta_y_buffer = delta_y*rx_delta_x;
  
        if ( delta_y != 0 )
        {
          delta_y_buffer = (delta_y_buffer<0)?(delta_y_buffer-(rx_bin_size+1)/2):(delta_y_buffer+(rx_bin_size+1)/2);  /* round off */
  
          delta_y_buffer = delta_y_buffer/rx_bin_size;
        }
  
  
        rf_freq_comp.i_lna_1_offset_vs_freq[path][power_mode]  =
             (int16)delta_y_buffer +
             rf_chain_ptr->rf_nv_dig_items_ptr->
             rfnv_cdma_lna_1_offset_vs_freq[path][power_mode][rx_start_bin];
  
        delta_y = (rf_chain_ptr->rf_nv_dig_items_ptr->
             rfnv_cdma_lna_2_offset_vs_freq[path][power_mode][rx_start_bin+1]-
             rf_chain_ptr->rf_nv_dig_items_ptr->
             rfnv_cdma_lna_2_offset_vs_freq[path][power_mode][rx_start_bin]);
  
        delta_y_buffer = delta_y*rx_delta_x;
  
        if ( delta_y != 0 )
        {
          delta_y_buffer = (delta_y_buffer<0)?(delta_y_buffer-(rx_bin_size+1)/2):(delta_y_buffer+(rx_bin_size+1)/2);  /* round off */
  
          delta_y_buffer = delta_y_buffer/rx_bin_size;
        }
  
  
        rf_freq_comp.i_lna_2_offset_vs_freq[path][power_mode] =
             (int16)delta_y_buffer +
             rf_chain_ptr->rf_nv_dig_items_ptr->
             rfnv_cdma_lna_2_offset_vs_freq[path][power_mode][rx_start_bin];
  
        delta_y = (rf_chain_ptr->rf_nv_dig_items_ptr->
             rfnv_cdma_lna_3_offset_vs_freq[path][power_mode][rx_start_bin+1]-
             rf_chain_ptr->rf_nv_dig_items_ptr->
             rfnv_cdma_lna_3_offset_vs_freq[path][power_mode][rx_start_bin]);
  
        delta_y_buffer = delta_y*rx_delta_x;
  
        if ( delta_y != 0 )
        {
          delta_y_buffer = (delta_y_buffer<0)?(delta_y_buffer-(rx_bin_size+1)/2):(delta_y_buffer+(rx_bin_size+1)/2);  /* round off */
  
          delta_y_buffer = delta_y_buffer/rx_bin_size;
        }
  
  
        rf_freq_comp.i_lna_3_offset_vs_freq[path][power_mode] =
             (int16)delta_y_buffer +
             rf_chain_ptr->rf_nv_dig_items_ptr->
             rfnv_cdma_lna_3_offset_vs_freq[path][power_mode][rx_start_bin];
  
        delta_y = (rf_chain_ptr->rf_nv_dig_items_ptr->
             rfnv_cdma_lna_4_offset_vs_freq[path][power_mode][rx_start_bin+1]-
             rf_chain_ptr->rf_nv_dig_items_ptr->
             rfnv_cdma_lna_4_offset_vs_freq[path][power_mode][rx_start_bin]);
  
        delta_y_buffer = delta_y*rx_delta_x;
  
        if ( delta_y != 0 )
        {
          delta_y_buffer = (delta_y_buffer<0)?(delta_y_buffer-(rx_bin_size+1)/2):(delta_y_buffer+(rx_bin_size+1)/2);  /* round off */
  
          delta_y_buffer = delta_y_buffer/rx_bin_size;
        }
  
  
        rf_freq_comp.i_lna_4_offset_vs_freq[path][power_mode] =
             (int16)delta_y_buffer + 
             rf_chain_ptr->rf_nv_dig_items_ptr->
             rfnv_cdma_lna_4_offset_vs_freq[path][power_mode][rx_start_bin];
      }
#else
      /* DVGA GAIN OFFSET VS FREQ */
      /* delta_y is the difference between the lim_vs_freq values
         of the bin edges to the left and right of our loc_chan */
  
      delta_y = (rf_chain_ptr->rf_nv_dig_items_ptr->
           rfnv_vga_gain_offset_vs_freq[path][rx_start_bin+1]-
           rf_chain_ptr->rf_nv_dig_items_ptr->
           rfnv_vga_gain_offset_vs_freq[path][rx_start_bin]);
  
      delta_y_buffer = delta_y*rx_delta_x;  /* 1 symbol is expanded by multipling tx_bin_size */
      if ( delta_y != 0 )
      {
        delta_y_buffer = (delta_y_buffer<0)?(delta_y_buffer-(rx_bin_size+1)/2):(delta_y_buffer+(rx_bin_size+1)/2);  /* round off */
        delta_y_buffer = delta_y_buffer/rx_bin_size;
      }
  
      rf_freq_comp.i_vga_gain_offset_vs_freq[path] =
          (int16)delta_y_buffer +
          rf_chain_ptr->rf_nv_dig_items_ptr->
          rfnv_vga_gain_offset_vs_freq[path][rx_start_bin];
  
      /* LNA OFFSETS VS FREQ */
      delta_y = (rf_chain_ptr->rf_nv_dig_items_ptr->
           rfnv_cdma_lna_1_offset_vs_freq[path][rx_start_bin+1]-
           rf_chain_ptr->rf_nv_dig_items_ptr->
           rfnv_cdma_lna_1_offset_vs_freq[path][rx_start_bin]);
  
      delta_y_buffer = delta_y*rx_delta_x;  /* 1 symbol is expanded by multipling tx_bin_size */
      if ( delta_y != 0 )
      {
        delta_y_buffer = (delta_y_buffer<0)?(delta_y_buffer-(rx_bin_size+1)/2):(delta_y_buffer+(rx_bin_size+1)/2);  /* round off */
        delta_y_buffer = delta_y_buffer/rx_bin_size;
      }
  
      rf_freq_comp.i_lna_1_offset_vs_freq[path]  =
          (int16)delta_y_buffer +
          rf_chain_ptr->rf_nv_dig_items_ptr->
          rfnv_cdma_lna_1_offset_vs_freq[path][rx_start_bin];
  
      delta_y = (rf_chain_ptr->rf_nv_dig_items_ptr->
           rfnv_cdma_lna_2_offset_vs_freq[path][rx_start_bin+1]-
           rf_chain_ptr->rf_nv_dig_items_ptr->
           rfnv_cdma_lna_2_offset_vs_freq[path][rx_start_bin]);
  
      delta_y_buffer = delta_y*rx_delta_x;  /* 1 symbol is expanded by multipling tx_bin_size */
      if ( delta_y != 0 )
      {
        delta_y_buffer = (delta_y_buffer<0)?(delta_y_buffer-(rx_bin_size+1)/2):(delta_y_buffer+(rx_bin_size+1)/2);  /* round off */
        delta_y_buffer = delta_y_buffer/rx_bin_size;
      }
  
      rf_freq_comp.i_lna_2_offset_vs_freq[path] =
           (int16)delta_y_buffer +
           rf_chain_ptr->rf_nv_dig_items_ptr->
           rfnv_cdma_lna_2_offset_vs_freq[path][rx_start_bin];
  
      delta_y = (rf_chain_ptr->rf_nv_dig_items_ptr->
           rfnv_cdma_lna_3_offset_vs_freq[path][rx_start_bin+1]-
           rf_chain_ptr->rf_nv_dig_items_ptr->
           rfnv_cdma_lna_3_offset_vs_freq[path][rx_start_bin]);
  
      delta_y_buffer = delta_y*rx_delta_x;  /* 1 symbol is expanded by multipling tx_bin_size */
      if ( delta_y != 0 )
      {
        delta_y_buffer = (delta_y_buffer<0)?(delta_y_buffer-(rx_bin_size+1)/2):(delta_y_buffer+(rx_bin_size+1)/2);  /* round off */
        delta_y_buffer = delta_y_buffer/rx_bin_size;
      }
  
      rf_freq_comp.i_lna_3_offset_vs_freq[path] =
           (int16)delta_y_buffer+
           rf_chain_ptr->rf_nv_dig_items_ptr->
           rfnv_cdma_lna_3_offset_vs_freq[path][rx_start_bin];
  
      delta_y = (rf_chain_ptr->rf_nv_dig_items_ptr->
           rfnv_cdma_lna_4_offset_vs_freq[path][rx_start_bin+1]-
           rf_chain_ptr->rf_nv_dig_items_ptr->
           rfnv_cdma_lna_4_offset_vs_freq[path][rx_start_bin]);
  
      delta_y_buffer = delta_y*rx_delta_x;  /* 1 symbol is expanded by multipling tx_bin_size */
      if ( delta_y != 0 )
      {
        delta_y_buffer = (delta_y_buffer<0)?(delta_y_buffer-(rx_bin_size+1)/2):(delta_y_buffer+(rx_bin_size+1)/2);  /* round off */
        delta_y_buffer = delta_y_buffer/rx_bin_size;
      }
  
      rf_freq_comp.i_lna_4_offset_vs_freq[path] =
           (int16)delta_y_buffer +
           rf_chain_ptr->rf_nv_dig_items_ptr->
           rfnv_cdma_lna_4_offset_vs_freq[path][rx_start_bin];
  #endif /* RF_HAS_POWER_MODE_LIB */
  
    }

#ifdef INTERPOLATION_PROFILING
#error code not present
#endif
  return;
} /* update_freq_comp_items */


/*===========================================================================

FUNCTION RF_INTERPOLATE_CHAN_OFFSET                  EXTERNAL FUNCTION

DESCRIPTION
  This function handles disjoint channel numbering schemes for
  different band classes.

DEPENDENCIES
  Only for use by update freq comp items. For Cell, all channels
  in the list need to get re-mapped.

RETURN VALUE
  word representing the re-mapped channel (contiguous). eg for cell,
  pass in 1023, 32 is returned other bands added as needed.

SIDE EFFECTS
  None

===========================================================================*/
uint32 rf_interpolate_chan_offset
(
   rf_path_enum_type path,
   word chan          /* input channel number */
)
{
  uint32 remap_chan;

  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];

  /* Zero is the terminator of the channel list, do not translate,
     do not pass go, go directly to 0. */
  if ( chan == 0 ) {
    return 0;
  }
  switch (rf_chain_ptr->rf_curr_cdma_band)
  {
    case RF_BC0_BAND:
      if ( 1 <= chan && chan <= 799 )
      {
        remap_chan = chan + 33;
      }
      else if ( 991 <= chan && chan <= 1023 )
      {
        remap_chan = chan - 990;
      }
      else
      {
        remap_chan = 0;
      }
      break;

    case RF_BC3_BAND:
      remap_chan = rf_bc3_chan_to_freq(chan);
      break;

    case RF_BC5_BAND:
      remap_chan = rf_get_tx_carrier_freq(RF_BC5_BAND,chan);
      /* If the channel is not valid then just return the channel 
         passed in. */
      if (remap_chan == 0) 
      {
          remap_chan = chan;
      }
      break;

    case RF_BC11_BAND:
      remap_chan = rf_get_tx_carrier_freq(RF_BC11_BAND,chan);
      /* If the channel is not valid then just return the channel 
         passed in. */
      if (remap_chan == 0) 
      {
          remap_chan = chan;
      }
      break;
      
    default:
      remap_chan = chan;
      break;
  }

  return (remap_chan);

} /* rf_interpolate_chan_offset */

/*===========================================================================

FUNCTION RF_BC3_CHAN_TO_FREQ

DESCRIPTION
  This function converts the channel to a frequency based on Band Class
  3 (JCDMA) formulas.  Band Class 3 has 4 blocks of channels and the
  relationship between channel to frequency is not linear.
  The separation between Rx and Tx is 10 MHz.

  Although the Rx frequencies are 10 MHz offset from the Tx frequencies,
  frequency is only calculated for Transmit because the Rx frequencies
  are just an offset, and it wouldn't affect interpolation results.

  Odd channels are invalid.

DEPENDENCIES
  applies to Band Class 3 only

RETURN VALUE
  int 16.  The frequency in KHz.  (MHz is not possible because of the
  floating point)

SIDE EFFECTS
  None

===========================================================================*/
static uint32 rf_bc3_chan_to_freq( word channel)
{
  uint32 bc3_freq_khz;

  /* Ch 1100 - 1140,   0.0125(N-1040) + 887.000 MHz */
  if ((channel >= 1100) && (channel <= 1140))
  {
     bc3_freq_khz = (uint32)((12.5 * (channel-1040)) + 887000 );
  }

  /* Ch 1260 - 1600,  0.0125(N-1200) + 893.000 MHz */
  else if ((channel >= 1260) && (channel <= 1600))
  {
     bc3_freq_khz = (uint32)((12.5 * (channel-1200)) + 893000 );
  }

  /* Ch 800 - 980,  0.0125(N-800) + 898.000 MHz */
  else if ((channel >= 800) && (channel <= 980))
  {
     bc3_freq_khz = (uint32)((12.5 * (channel-800)) + 898000 );
  }

  /* Ch 60 - 740,  0.0125(N) + 915.000 MHz */
  else if ((channel >= 60) && (channel <= 740))
  {
     bc3_freq_khz = (uint32)((12.5 * (channel)) + 915000 );
  }

  /* all other channels are not valid */
  else
  {
     bc3_freq_khz = channel;
//     ERR( "Invalid channel for Band Class 3: %d", channel, 0,0 );
  }

  return bc3_freq_khz;


}

#endif /* RF_HAS_LINEAR_INTERPOLATION */
/*===========================================================================

FUNCTION INIT_NV_PTRS                                    INTERNAL FUNCTION

DESCRIPTION
  Initializes the table that holds the pointers to the digital items table.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void init_nv_ptrs(void)
{
    static boolean nv_tbl_initialized = FALSE;
    
    if(nv_tbl_initialized == FALSE)
    {
        nv_tbl_initialized = TRUE;

        /* US Cellular */
        #ifdef RF_HAS_BC0_NV_TABLE
        #ifdef RF_HAS_SUB_BLOCK_SUPPORT
        rf_digital_nv_table[RF_PATH_0][RF_BC0_BAND] = rf_digital_nv_table[RF_PATH_1][RF_BC0_BAND] = 
            (rfnv_digital_items_type *) rfnv_get_nv_table(RFNV_CDMA1X_BC0);        
        #else
        rf_digital_nv_table[RF_BC0_BAND] = (rfnv_digital_items_type *) 
            rfnv_get_nv_table(RFNV_CDMA1X_BC0);
        #endif
        cai_to_rf[CAI_BAND_CLASS0] = RF_BC0_BAND;
        #endif
        
        /* US PCS */
        #ifdef RF_HAS_BC1_NV_TABLE
        #ifdef RF_HAS_SUB_BLOCK_SUPPORT
        rf_digital_nv_table[RF_PATH_0][RF_BC1_BAND] = rf_digital_nv_table[RF_PATH_1][RF_BC1_BAND] = 
            (rfnv_digital_items_type *) rfnv_get_nv_table(RFNV_CDMA1X_BC1);        
        #else
        rf_digital_nv_table[RF_BC1_BAND] = (rfnv_digital_items_type *) 
            rfnv_get_nv_table(RFNV_CDMA1X_BC1);
        #endif
        cai_to_rf[CAI_BAND_CLASS1] = RF_BC1_BAND;
        #endif
        
        /* JTACS */
        #ifdef RF_HAS_BC2_NV_TABLE
        #ifdef RF_HAS_SUB_BLOCK_SUPPORT
        rf_digital_nv_table[RF_PATH_0][RF_BC2_BAND] = rf_digital_nv_table[RF_PATH_1][RF_BC2_BAND] = 
            (rfnv_digital_items_type *) rfnv_get_nv_table(RFNV_CDMA1X_BC2);        
        #else
        rf_digital_nv_table[RF_BC2_BAND] = (rfnv_digital_items_type *) 
            rfnv_get_nv_table(RFNV_CDMA1X_BC2);
        #endif
        cai_to_rf[CAI_BAND_CLASS2] = RF_BC2_BAND;
        #endif
        
        /* JCDMA */
        #ifdef RF_HAS_BC3_NV_TABLE
#error code not present
        #endif
        
        /* Korean PCS */
        #ifdef RF_HAS_BC4_NV_TABLE
        #ifdef RF_HAS_SUB_BLOCK_SUPPORT
        rf_digital_nv_table[RF_PATH_0][RF_BC4_BAND] = rf_digital_nv_table[RF_PATH_1][RF_BC4_BAND] = 
            (rfnv_digital_items_type *) rfnv_get_nv_table(RFNV_CDMA1X_BC4);        
        #else
        rf_digital_nv_table[RF_BC4_BAND] = (rfnv_digital_items_type *) 
            rfnv_get_nv_table(RFNV_CDMA1X_BC4);
        #endif
        cai_to_rf[CAI_BAND_CLASS4] = RF_BC4_BAND;
        #endif
        
        /* NMT (450MHz) */
        #ifdef RF_HAS_BC5_NV_TABLE
        #ifdef RF_HAS_SUB_BLOCK_SUPPORT
        rf_digital_nv_table[RF_PATH_0][RF_BC5_BAND] = rf_digital_nv_table[RF_PATH_1][RF_BC5_BAND] = 
            (rfnv_digital_items_type *) rfnv_get_nv_table(RFNV_CDMA1X_BC5);        
        #else
        rf_digital_nv_table[RF_BC5_BAND] = (rfnv_digital_items_type *) 
            rfnv_get_nv_table(RFNV_CDMA1X_BC5);
        #endif
        cai_to_rf[CAI_BAND_CLASS5] = RF_BC5_BAND;
        #endif
         
        /* IMT */
        #ifdef RF_HAS_BC6_NV_TABLE
#error code not present
        #endif

        #ifdef RF_HAS_BC10_NV_TABLE
        #ifdef RF_HAS_SUB_BLOCK_SUPPORT
        rf_digital_nv_table[RF_PATH_0][RF_BC10_BAND] = rf_digital_nv_table[RF_PATH_1][RF_BC10_BAND] = 
            (rfnv_digital_items_type *) rfnv_get_nv_table(RFNV_CDMA1X_BC10);        
        #else
        rf_digital_nv_table[RF_BC10_BAND] = (rfnv_digital_items_type *) 
            rfnv_get_nv_table(RFNV_CDMA1X_BC10);
        #endif
        cai_to_rf[CAI_BAND_CLASS10] = RF_BC10_BAND;
        #endif
        
        /* BC11 */
        #ifdef RF_HAS_BC11_NV_TABLE
#error code not present
        #endif /* RF_HAS_BC11_NV_TABLE */
        
        /* US PCS + 5 MHz*/
        #ifdef RF_HAS_BC14_NV_TABLE
        #ifdef RF_HAS_SUB_BLOCK_SUPPORT
        rf_digital_nv_table[RF_PATH_0][RF_BC14_BAND] = rf_digital_nv_table[RF_PATH_1][RF_BC14_BAND] = 
            (rfnv_digital_items_type *) rfnv_get_nv_table(RFNV_CDMA1X_BC14);        
        #else
        rf_digital_nv_table[RF_BC14_BAND] = (rfnv_digital_items_type *) 
            rfnv_get_nv_table(RFNV_CDMA1X_BC14);
        #endif
        #if defined(CAI_BAND_CLASS14)
        cai_to_rf[CAI_BAND_CLASS14] = RF_BC14_BAND;
        #else 
        cai_to_rf[14] = RF_BC14_BAND;
        #endif /* CAI_BAND_CLASS14 */
    #endif /* RF_HAS_BC14_NV_TABLE */

    /* AWS Band */
        #ifdef RF_HAS_BC15_NV_TABLE
        #ifdef RF_HAS_SUB_BLOCK_SUPPORT
        rf_digital_nv_table[RF_PATH_0][RF_BC15_BAND] = rf_digital_nv_table[RF_PATH_1][RF_BC15_BAND] = 
            (rfnv_digital_items_type *) rfnv_get_nv_table(RFNV_CDMA1X_BC15);        
        #else
        rf_digital_nv_table[RF_BC15_BAND] = (rfnv_digital_items_type *) 
            rfnv_get_nv_table(RFNV_CDMA1X_BC15);
        #endif        

        #if defined(CAI_BAND_CLASS15)
        cai_to_rf[CAI_BAND_CLASS15] = RF_BC15_BAND;
        #else 
        cai_to_rf[15] = RF_BC15_BAND;
        #endif /* CAI_BAND_CLASS15 */
        #endif /* RF_HAS_BC15_NV_TABLE */

        /* GPS */
        #ifdef RF_HAS_GPS_NV_TABLE
#error code not present
        #endif        
    }
}

/*===========================================================================

FUNCTION RF_GET_SUPPORTED_HW                           INTERNAL FUNCTION

DESCRIPTION
  Returns the ID of the RF Card, from the NV Item 
  NV_RF_HW_BC_CONFIG_I.

DEPENDENCIES
  None

RETURN VALUE
  rf_hw_type value containing the Id of the RF card being used with this target.

SIDE EFFECTS
  None
===========================================================================*/
rf_hw_type rf_get_supported_hw(void)
{
  #if defined(T_QUASAR_X) || defined(RF_HAS_BC_EMULATION)
#error code not present
  #else   /* TBD: RF_TODO_7600_ADK */
  //jcen rfnv_common_items_type *nv_ptr = rfnv_common_table();
  //    byte hw = nv_ptr->rfnv_rf_hw_config;
  
  return (rf_hw_type) rf_get_rf_hw_config();
  #endif

}
#if 0
/*===========================================================================

FUNCTION IS_GPS_RF_CONFIG_SGPS                           INTERNAL FUNCTION

DESCRIPTION
  Returns true if simultaneous GPS mode is enabled 

DEPENDENCIES
  None

RETURN VALUE
  TRUE if simultaneous GPS mode is enabled.

SIDE EFFECTS
  None
===========================================================================*/
boolean is_gps_rf_config_sgps(void)
{
  rfnv_common_items_type *nv_ptr = rfnv_common_table();
  byte gps_rf_config = nv_ptr->rfnv_gps_rf_config;
  if( gps_rf_config & 2 )
  {
    return TRUE;
  }
  return FALSE;
}
#endif
#endif /* !FEATURE_GSM_PLT */

/*===========================================================================

FUNCTION rf_get_nv_band

DESCRIPTION
  This function returns the NV band ID representing which NV table
  is currently being used.

DEPENDENCIES
  None

RETURN VALUE
  NV band id.

SIDE EFFECTS
  None
  
===========================================================================*/
byte rf_get_nv_band( rfcom_device_enum_type device )
{
      
      rf_path_enum_type path = rfcom_to_path[ device ];       
   rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];                        

   if (path >= RF_PATH_MAX)
   {
      MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
      rf_error_flag = TRUE;
      return 0;
   }

   if (rf_chain_ptr->rf_curr_cdma_band >= RF_MAX_BAND)
   {
       MSG_FATAL("band class invalid or out of bounds", 0, 0, 0);
       rf_error_flag = TRUE;
       return 0;
   }

   #ifdef RF_HAS_SUB_BLOCK_SUPPORT          
      return (rf_chain_ptr->rf_nv_dig_items_ptr ->band_id);
      //return ((byte)rf_digital_nv_table[path][rf_chain_ptr->rf_curr_cdma_band]->band_id);
   #else                        
   return ((byte)rf_digital_nv_table[rf_chain_ptr->rf_curr_cdma_band]->band_id);
   #endif  

}

/*===========================================================================

FUNCTION rf_get_unused_synth_lock_time
DESCRIPTION
  This function returns the amount of time that was not used from
  the maximum synth lock time allowed. 
  
DEPENDENCIES
  Uses definition RF_SYNTH_LOCK_ITER_WAIT_US
  rf_init() needs to be called to setup common nv table ptr.

RETURN VALUE
  unused synth lock time in microseconds

SIDE EFFECTS
  None
  
===========================================================================*/
uint4 rf_get_unused_synth_lock_time( rfcom_device_enum_type device )
{
   rf_path_enum_type path = rfcom_to_path[ device ];
   rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];
   uint4 unused_synth_time_us = 0;

   if ( rf_chain_ptr->rf_synth_time_iter < rf_nv_common_items_ptr->rfnv_digital_pll_lock_timer ) 
   {
      unused_synth_time_us = rf_get_max_synth_lock_time() - 
                            (rf_chain_ptr->rf_synth_time_iter * RF_SYNTH_LOCK_ITER_WAIT_US);
   }

   return ( unused_synth_time_us );
}

/*===========================================================================

FUNCTION rf_get_max_synth_lock_time
DESCRIPTION
  This function returns the maximum time allocated for synth lock to occur.
  
DEPENDENCIES
  Uses definition RF_SYNTH_LOCK_ITER_WAIT_US
  rf_init() needs to be called to setup common nv table ptr.

RETURN VALUE
  max synth lock time in us

SIDE EFFECTS
  None
  
===========================================================================*/
uint4 rf_get_max_synth_lock_time( void )
{
   uint4 max_synth_lock_time = RF_SYNTH_LOCK_ITER_WAIT_US;

   /* Minimum is 1 iteration */
   if ( rf_nv_common_items_ptr->rfnv_digital_pll_lock_timer > 0 ) 
   {
      max_synth_lock_time = rf_nv_common_items_ptr->rfnv_digital_pll_lock_timer * RF_SYNTH_LOCK_ITER_WAIT_US;
   }
   return (max_synth_lock_time);
}

/*===========================================================================

FUNCTION RF_WAIT_FOR_SYNTH_LOCK

DESCRIPTION
  This function will invoke a wait loop for SYNTH lock active.  The maximum
  loop iteration is determined by the NV item NV_DIGITAL_PLL_LOCK_TIMER_I.
 
  
DEPENDENCIES
  Uses definition RF_SYNTH_LOCK_ITER_WAIT_US
  rf_init() needs to be called to setup common nv table ptr.

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void rf_wait_for_synth_lock( rfcom_device_enum_type device )
{
  rf_path_enum_type path = rfcom_to_path[ device ];
  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];
  synth_pll_for_synth_check pll_device = RX_PLL;
  byte lock_count = 0;

  if (rf_chain_ptr->rf_mode == DB_RF_GPS)
  {
    pll_device = GPS_PLL;
  }

  else if (rf_chain_ptr->rf_mode == DB_RF_SLEEP)
  {
    RF_FATAL_ERROR("Cannot check for synth lock in sleep mode.");
  }
  
  /* -----------------------------------------------------
  **  Make sure synths are locked before waking up the rfr
  ** ----------------------------------------------------- */
  do
  {
    lock_count++;
    clk_busy_wait(RF_SYNTH_LOCK_ITER_WAIT_US);

    /* For now, chain1 will use default synth lock time out */
    /*if ((synth_is_locked(device,pll_device)) && (path != RF_PATH_1))
    {     
      break;
    }*/
  }  
  while ( lock_count < rf_nv_common_items_ptr->rfnv_digital_pll_lock_timer );

  /* Store the number of iterations to obtain PLL synth lock */
  rf_chain_ptr->rf_synth_time_iter = lock_count;
} /* rf_wait_for_synth_lock */

/*===========================================================================

FUNCTION RF_GET_FIRST_BC_IN_MASK

DESCRIPTION
  This function returns the first band in the BC Mask

DEPENDENCIES
  None

RETURN VALUE
  First Band Class in the Band Mask.

SIDE EFFECTS
  None
  
===========================================================================*/
static rf_card_band_type rf_get_first_bc_in_mask( void )
{
  rf_bc_and_mask_type valid_band;
  
  sys_band_mask_type band_mask = rfnv_get_band_mask();

  uint8 BCInx = 0;  
      
  /* Loop through for each band defined in the table and is enabled in the NV Band Mask */
  while(rf_get_valid_bc_and_mask(BCInx,&valid_band) == TRUE)
  {
      /* If the band class is equal to all 1s or the band mask is
         equal to 0 then the band class/mask value is invalid. 
         We should had exit the while loop. */
      RF_ASSERT((valid_band.bc != (rf_card_band_type)(-1)) && \
             (valid_band.mask != 0));
        
      if ((valid_band.bc == (rf_card_band_type)(-1)) || (valid_band.bc >= RF_MAX_BAND)) 
      {
          MSG_FATAL("band class invalid or out of bounds", 0, 0, 0);
          rf_error_flag = TRUE;
          return RF_MAX_BAND;
      }

      if( 0 != (valid_band.mask & band_mask ))
      {
          break;
      }
    
      BCInx++;
  }

  return valid_band.bc;

}

/*===========================================================================

FUNCTION RF_HDR_SET_TRK_LO_PDM_MUX_TO_HDR

DESCRIPTION
  This function allows HDR to change the trk_lo PDM to be controlled by HDR
  or 1x.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void rf_hdr_set_trk_lo_pdm_mux_to_hdr( boolean sel_hdr )
{                                       

//To do 7600 ----? Register change.
#ifndef RF_MSM7600_HWIO 
  /* Either HDR or 1x is set to control the TRK_LO PDM. */
#ifdef RF_BYPASS_SYSTEM_CTL
  HWIO_OUTM( MODEM_GENERIC_CTL_REG, 
             HWIO_FMSK(MODEM_GENERIC_CTL_REG, TRK_LO_ADJ_SEL),
             sel_hdr << HWIO_SHFT( MODEM_GENERIC_CTL_REG, TRK_LO_ADJ_SEL ) );
#else
  HWIO_OUTM( SYSTEM_MODE, 
             HWIO_FMSK(SYSTEM_MODE, TRK_LO_ADJ_SEL),
             sel_hdr << HWIO_SHFT( SYSTEM_MODE, TRK_LO_ADJ_SEL ) );
#endif
#endif   // RF_MSM7600_HWIO

}

#ifdef FEATURE_RF_PA_DVS
/*===========================================================================

FUNCTION rf_config_pa_dvs
DESCRIPTION
  This function will provide setup for PA DVS mode of operation.
  PMIC only be enabled/disabled when there is a change to its existing 
  configuration.
    
DEPENDENCIES
  For NV items support. The rf_chain_ptr->rf_nv_dig_items_ptr needs 
  to be assigned.
 
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void rf_config_pa_dvs( void )
{
    pm_err_flag_type pm_level_error;
    static boolean dvs_is_on = FALSE;
    static uint16 last_dvs_voltage = 0;
    uint16 dvs_voltage;

#ifdef RF_HAS_PA_DVS_NV

    rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ RF_PATH_0 ];
    dvs_voltage = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_pa_dvs_voltage;

    /* PA DVS is considered to be disabled if voltage setting is set to 0 */
    if ((rf_chain_ptr->rf_nv_dig_items_ptr != NULL) && (dvs_voltage !=0 )) 
    {
       if ( !dvs_is_on || ( last_dvs_voltage != dvs_voltage )) 
       {
          /* Enable PMIC for DVS operation */
          pm_variable_pa_vdd_init(TRUE);

          /* 
          * set the voltage level of the PA dynamic voltage switch 
          *  voltage regulator. 
          */
          if ((pm_level_error = pm_vreg_set_level (PM_VREG_PA_ID, dvs_voltage)) != PM_ERR_FLAG__SUCCESS)
          {
              MSG_ERROR("PA DVS voltage %d set error. code %d", dvs_voltage, pm_level_error, 0);
          }
          dvs_is_on = TRUE;
          last_dvs_voltage = dvs_voltage;

          MSG_HIGH("PA DVS enabled. voltage=%d mV", dvs_voltage,0,0);
       }
    }
    else
    {
       if ( dvs_is_on ) 
       {
          /* Disable PMIC for DVS operation */
          pm_variable_pa_vdd_init(FALSE);
          dvs_is_on = FALSE;

          MSG_HIGH("PA DVS disabled.",0,0,0);
       }
    }
#else /* !RF_HAS_PA_DVS_NV */
    if ( !dvs_is_on ) 
    {
       /* Enable PMIC for DVS operation */
       pm_variable_pa_vdd_init(TRUE);

       /* 
       * set the voltage level of the PA dynamic voltage switch 
       *  voltage regulator. 
       */
       pm_level_error = pm_vreg_set_level (PM_VREG_PA_ID, RF_PA_DVS_VOLTAGE_1X);
       dvs_is_on = TRUE;

       MSG_HIGH("PA DVS enabled. voltage=%d mV", RF_PA_DVS_VOLTAGE_1X,0,0);
    }
#endif /* RF_HAS_PA_DVS_NV */
}
#endif /* FEATURE_RF_PA_DVS */

/*===========================================================================

FUNCTION RF_GET_SUB_CLASS

DESCRIPTION
  This function takes band parameter and and returns an associated subclass
  bitmask.

DEPENDENCIES
  None

RETURN VALUE
  A word containing bitmask set to indicate the supported subclasses with the
  following mapping:
  
    
    BIT  :  SUBCLASS 
   -------------------
     0         0   
     1         1   
     2         2   
     3         3   
     4         4   
     5         5   
     6         6   
     7         7   
     8         8   
     9         9   
    10        10  
    11        11  
    12        12  
    13        13  
    14        14  
    15        15  
  ----------------

SIDE EFFECTS
  None
  
===========================================================================*/
uint32 rf_get_sub_class( rf_card_band_type band )
{
    return rfc_get_sub_class( band );
}

/*===========================================================================

FUNCTION RF_GET_NUM_ANTENNA

DESCRIPTION
  This function does not take a parameter and returns an unsigned int representing
  the no of rf antennas supported. 

DEPENDENCIES
  None

RETURN VALUE
  Returns an unsigned int representing the no of rf antennas supported.

SIDE EFFECTS
  None
  
===========================================================================*/
uint8 rf_get_num_antenna(void)
{
    return rfc_get_num_antenna();
}


/*===========================================================================

FUNCTION RF_IS_TUNABLE

DESCRIPTION
  This function takes parameters as an index to antenna array and rf_card_band_type
  band and returns a boolean TRUE if the indexed antenna is tunable otherwise a 
  FALSE is returned. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the indexed antenna can be tuned independently othewise FALSE.

SIDE EFFECTS
  None
  
===========================================================================*/
boolean rf_is_tunable_on_antenna( int antenna_index, rf_card_band_type band )
{
    return rfc_is_tunable_on_antenna(antenna_index,band);
}

/*===========================================================================

FUNCTION RF_GET_ANTENNA_DIFF

DESCRIPTION
  This function takes the band and indexes to two antennas as parameters and
  returns a TRUE if the first antenna has better or equal sensitivity to the 
  the 2nd antenna.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the first index is pointing to the antenna with the better or
  equal sensitivity as compared to the antenna pointed to by the 2nd index 
  otherwise a FALSE is returned.

SIDE EFFECTS
  None
  
===========================================================================*/
rf_antenna_diff_type  rf_get_antenna_diff
( 
    rf_card_band_type band,
    word chan,
    int ant_x,
    int ant_ref 
)
{

    rf_antenna_diff_type ret_val = RF_ANT_QLTY_UNKNOWN;

    #ifdef RF_HAS_SUB_BLOCK_SUPPORT
    rfnv_digital_items_type *nv_tbl_ptr = rfnv_get_nv_digital_item_ptr(RFCOM_TRANSCEIVER_0,band,chan);
    #else
    rfnv_digital_items_type *nv_tbl_ptr = rf_digital_nv_table[ band ];
    #endif

    if(nv_tbl_ptr == NULL)
    {
        return ret_val;
    }

    
    /* channel currently not supported. Force to zero */
    chan = 0;

    if( (nv_tbl_ptr->rfnv_ant_quality[ant_x][chan]== 0) || 
        (nv_tbl_ptr->rfnv_ant_quality[ant_ref][chan]== 0))
    {
        ret_val = RF_ANT_QLTY_UNKNOWN;
    }

    else if( nv_tbl_ptr->rfnv_ant_quality[ant_x][chan] <  
             nv_tbl_ptr->rfnv_ant_quality[ant_ref][chan])
    {
        ret_val = RF_ANT_QLTY_BETTER;
    }

    else if( nv_tbl_ptr->rfnv_ant_quality[ant_x][chan] >  
             nv_tbl_ptr->rfnv_ant_quality[ant_ref][chan])
    {
        ret_val = RF_ANT_QLTY_WORSE;
    }

    else
        ret_val = RF_ANT_QLTY_IDENTICAL;

    return ret_val;
}
#ifdef RF_HAS_NO_RTC_DEPENDENCY
/*===========================================================================

FUNCTION RF_BYPASS_RTC_CTL_ON_OFF

DESCRIPTION
  Control to enable or disable the Bypass of RTC Synch.
  This function will control both rx paths. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rf_bypass_rtc_ctl_path_on_off( rfcom_device_enum_type device, boolean rf_bypass_rtc_on_off )
{
  rxf_bypass_rtc_ctl_path_on_off(rfcom_to_path[ device ], rf_bypass_rtc_on_off);
}

#endif /* RF_HAS_NO_RTC_DEPENDENCY */

/*===========================================================================

FUNCTION RF_WU_INIT_RF_DEVICES                              INTERNAL FUNCTION

DESCRIPTION
  Calls to initialize the RF devices for Rx and Tx.

DEPENDENCIES
  If the devices are not under power management, then these actions are
  not required.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
__inline static void rf_wu_init_rf_devices(rf_path_enum_type path)
{
#if defined(FEATURE_PMIC_MANAGED_LDO)
#if defined(RF_MANAGE_SYNTH_LDO)
  #ifdef RF_TIME_PROFILING
#error code not present
  #endif /* RF_TIME_PROFILING */

  /* ----------------------------------------
  ** Initialize rft because synth LDO was
  ** turned on in rf_enable_digital_rx_pwr()
  ** ---------------------------------------- */
  rft_init( path );
#endif /* RF_MANAGE_SYNTH_LDO */

  #ifdef RF_TIME_PROFILING
#error code not present
  #endif /* RF_TIME_PROFILING */

#endif /* FEATURE_PMIC_MANAGED_LDO */

  #ifdef RF_TIME_PROFILING
#error code not present
  #endif /* RF_TIME_PROFILING */
}

/*===========================================================================

FUNCTION RF_WU_TX_SETUP                                     INTERNAL FUNCTION

DESCRIPTION
  Configures the Tx components during RF warmup.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
__inline static void rf_wu_tx_setup(rfcom_device_enum_type device)
{   
  /* Disable PA */
  rf_disable_pa_ctl(device);

  #ifdef RF_TIME_PROFILING
#error code not present
  #endif /* RF_TIME_PROFILING */

  /* Update the temperature data, prior to loading the linearizers */
  rf_cdma_temp_update();

  #ifdef RF_TIME_PROFILING
#error code not present
  #endif /* RF_TIME_PROFILING */
}

/*===========================================================================

FUNCTION RF_WU_SYNTH_START                                  INTERNAL FUNCTION

DESCRIPTION
  Calls to start the Rx PLL and buffers.  At this point the main Rx circuitry
  may still be off. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
__inline static void rf_wu_synth_start 
(
rfcom_device_enum_type device, 
db_rf_mode_type rf_mode
)
{
  /* Enable Rx PLL in rft */
  synth_rfrx_pll_enable( device, rf_mode );

  /* Enable PLL buffer in rfr */
  rfrx_pll_buffer_enable( device );
}

/*===========================================================================

FUNCTION RF_WU_LOAD_IM2                                     INTERNAL FUNCTION

DESCRIPTION
  Loads the IM2 values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
__inline static void rf_wu_load_im2
(
rf_path_enum_type path,
rf_chain_state_type *rf_chain_ptr 
)
{
  #ifdef RF_HAS_POWER_MODE_LIB
  (void)rfr_I_Q_dac_set (path,
            rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_i_value[path],
            rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_q_value[path],
            rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_transconductor_value[path]);
  #else
  (void)rfr_I_Q_dac_set (path,
            &rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_i_value[path],
            &rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_q_value[path],
            &rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_im2_transconductor_value[path]);
  #endif /* RF_HAS_POWER_MODE_LIB */
}

/*===========================================================================

FUNCTION RF_WU_RXF_RTC_SETUP                                INTERNAL FUNCTION

DESCRIPTION
  Calls are made after RTC (real time counter) has been enabled or after
  the RXF has been configured to bypass the RTC synchronization (on supported
  MSMs)
   
DEPENDENCIES
  For MSMs that have dependency of RXF on RTC, then RTC must be enabled.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
__inline static void rf_wu_rxf_rtc_setups(rf_path_enum_type path)
{
  /* reset RxF internals */
  rxf_reset( path );

  /* Disable the DACC */
  MSM_W_MASKED_i(RXF_CDMA(RXFc_DACC_CTL),MICRO_DACC_EN,RXF_RF_MICRO_DACC_DIS_V, (int)path);

  /* Clear DACC Estimators */
  rxf_clear_accum(path);

#ifndef RF_HAS_RXF_V2
#error code not present
#endif
}

/*===========================================================================

FUNCTION RF_WU_START_ACQ                                    INTERNAL FUNCTION

DESCRIPTION
  Calls to start the RxAGC acquisition sequence. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
__inline static void rf_wu_start_acq
(
rfcom_device_enum_type device, 
rf_path_enum_type path
)
{
  /* Synth lock 800 us should be completed by now */
  #if (!defined T_FFA) && defined (RF_HAS_WU_TIMED_DEBUG)
#error code not present
  #endif /* !T_FFA && RF_HAS_WU_TIMED_DEBUG */
    
  /* Start DC acquistion */
  rxf_cdma_wakeup_dc_acquire(path);

  /* wait 40 usec for DC to converge */
  clk_busy_wait(40);

  /* set AGC loop time constant to the IM value */
  MSM_W_MASKED_i( RXF_CDMA(RX_AGCc_GAIN_CTL), GAIN_CONSTANT, CAGC_RF_TC_0073_US, (int)path);

  /* let the LNA state machine run */
  RF_LNA_STATE_MACHINE_NORMAL( device );

  /* Un-freeze AGC VALUE */
  MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_LNA_CTL), AGC_VALUE_OVERRIDE, CAGC_RF_AGC_NRM_V, (int)path);
}

/*===========================================================================

FUNCTION RF_WU_SET_AGC_TRACK                                INTERNAL FUNCTION

DESCRIPTION
  Calls to set the RxAGC loop from acquisition to tracking mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
__inline static void rf_wu_set_agc_track
(
rfcom_device_enum_type device, 
rf_path_enum_type path
)
{
  /* restore the AGC Loop time constant to the tracking value */
  MSM_W_MASKED_i( RXF_CDMA(RX_AGCc_GAIN_CTL), GAIN_CONSTANT, CAGC_RF_TC_0392_US, (int)(path) );

  /* enable the IM switching behavior of the LNA state machine */
  rf_enable_im_level_ctls(device);
}

/*===========================================================================

FUNCTION RF_HAS_HIGH_SINR

DESCRIPTION
  Returns TRUE if the current chipset configuration can exhibit high
  SINR during conducted diversity testing.
    
DEPENDENCIES
  None
     
RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None
  
===========================================================================*/
boolean rf_has_high_sinr( void )
{
  return (rf_path_0.rfrdev == RFR6500 || rf_path_0.rfrdev == RFR6525);
} /* rf_has_high_sinr */

/*=========================================================================
FUNCTION RF_IS_BAND_CHAN_SUPPORTED

DESCRIPTION     
  This function provides a wrapper for rflib_is_band_chan_supported() to the 
  client applications. The path parameter passed to the library version is 
  set to RFCOM_TRANSCEIVER_0 and band parameter is converted from sys_band_class_e_type
  to rf_card_band_type. If the band or channel within the band is not 
  supported FALSE is returned; otherwise TRUE is returned.

DEPENDENCIES
  None
   

RETURN VALUE
  boolean:  TRUE   if band/channel is supported by the current devices 
                   configured in the library
            FALSE  if band/channel is not supported by the current devices 
                   configured in the library  

SIDE EFFECTS
  None.
===========================================================================*/
boolean rf_is_band_chan_supported
( 
    rfcom_device_enum_type device, 
    sys_band_class_e_type band, 
    word chan
)
{
    return rflib_is_band_chan_supported(rfcom_to_path[ device ],(rf_card_band_type) band, chan);
}

/*=========================================================================
FUNCTION RF_SET_PERIODIC_WAIT_TIMER

DESCRIPTION     
  This function emotes the clk_busy_wait() but in incremental waits of 50 Usec (maximum).
  This way we don't choke processor continuously (not more than 50 Usec).

DEPENDENCIES
  None
   

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void rf_set_periodic_wait_timer(uint32 total_wait_time)
{
  uint32 retry_time = 50; /* Maximum wait time of 50 Usec*/
  uint32 num_retry, delta_time;

  num_retry = (uint32) (total_wait_time / retry_time);

  delta_time = total_wait_time - ( retry_time * num_retry );

  clk_busy_wait ( delta_time );

  while (num_retry > 0)
  {
    clk_busy_wait ( retry_time );
    num_retry--;
  }

}

/*======================== O B S O L E T E  F U N C T I O N S =============*/
#ifdef RF_HAS_OBSOLETE_FUNCTIONS
#error code not present
#endif /* RF_HAS_OBSOLETE_FUNCTIONS */

#if defined(RF_HAS_BC_SPECIFIC_TX_PA_MAP_VALUE)
/*===========================================================================

FUNCTION RF_UPDATE_TX_PA_MAP_WITH_BC_SPECIFIC_VALUE       INTERNAL FUNCTION

DESCRIPTION
  Updates the TX PA MAP value with Band Class specific value.

  BC specific values come from NV, look for NV_BCX_PA_R_MAP_I, where
  X=0,1,2,3,.... example: NV_BC6_PA_R_MAP_I


DEPENDENCIES
  NV needs to be ini/loaded.

RETURN VALUE
  None

SIDE EFFECTS
  Updates rf_current_pa_r_map.
  Writes the new PA MAP value into the hardware.

===========================================================================*/
void rf_update_tx_pa_map_with_bc_specific_value(rfcom_device_enum_type device)
{

  rf_path_enum_type path = rfcom_to_path[ device ];

  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];

  if (path >= RF_PATH_MAX)
  {
     MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
     rf_error_flag = TRUE;
     return;
  }

  /* Update the band specific PA_R_MAP value if nv is populated */
  if ( rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_bc_pa_r_map.nv_initialized )
  {         
    /* The band specific pa_r_map value is valid, use it instead of the common */
    rf_current_pa_r_map = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_bc_pa_r_map.value;    
  } 
  else
  {
    /* Use the setting (NV or structure default) for the pa_r_map value */
    rf_current_pa_r_map = rf_nv_common_items_ptr->rfnv_pa_r_map;       
  }  

  
  /*if((rf_chain_status[0].rf_curr_cdma_band == RF_BC5_BAND)||(rf_chain_status[0].rf_curr_cdma_band == RF_BC11_BAND))
  {
    rf_current_pa_r_map = 0xFE;
  }
  else
  {
    rf_current_pa_r_map = 0xFB;
  }*/
  

  #if (defined(RF_HAS_QSC60X5) && defined(RF_HAS_MDSP_TX_AGC))
#error code not present
  #elif (defined(RF_HAS_QSC11X0) && defined(RF_HAS_MDSP_TX_AGC))
      /* Update the PA MAP setting in the MDSP. */
    CDMA_MDSP_SET_PA_RANGE_MAP(RF_CONVERT_1X_HW_TO_MDSP_PA_MAPPING( ((rf_current_pa_r_map) &( 0xFF))));
    CDMA_MDSP_ENABLE_PA_RANGE_MAP();
  #else
    /* Update the MSM PA MAP settings.  */
    MSM_W_MASKED ( PA_R_MAP, DATA, rf_current_pa_r_map );
  #endif /* #if defined(RF_HAS_QSC60X5 && defined(RF_HAS_MDSP_TX_AGC */
}
#endif /* RF_HAS_BC_SPECIFIC_TX_PA_MAP_VALUE */
