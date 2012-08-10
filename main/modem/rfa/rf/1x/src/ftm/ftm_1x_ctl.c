/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*



                            FTM  1x CTL

GENERAL DESCRIPTION
  This is the FTM 1x Control file which contains RF specific commands handled exclusively in Factory Test Mode(FTM)

Copyright (c) 1994, 1995, 1996 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1997, 1998, 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2000, 2001, 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2005, 2006, 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2008, 2009 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/ftm/ftm_1x_ctl.c#17 $ 
  $DateTime: 2009/02/10 13:29:59 $ 
  $Author: lesp $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/27/09   et      Fixed potential out of bounds issue
11/07/08   vm      Added support for 3 State PA
11/06/08   sl      Fixed divide-by-zero occurring in rflib_set_resampler_phase_map_coefs due to
                   invalid channel being sent when setting band class.
11/06/08   sl      Merged more changes from the mainline into tx_rx_vs_freq (CompositeCal).
                   The changes do not affect measurement results. They only affect error-checking.
10/31/08   sl/vm   Enabled and tested ftm_tx_rx_freq (Composite Cal) functionality.
09/30/08   vm/vb   Added Support for Band Class Sub Block Implementation
09/14/08   vm      Added the support for Tx DAC Backoff
08/06/08   vm      Fixed the correction in ftm_set_tx_agc()
07/31/08   ems     Corrected set_level_mdsp_pdm to unit32
06/16/08   vb      Fixed issues with ftm_set_beta_prime_scaling
05/05/08   vm      Saturate the Tx PDMs to 63 on QSC1100
05/05/08   vb      Changed the enumeration for Primary Chain state control API
04/18/08   vb      Added FTM APIs for setting Primary Chain to Sleep ON/OFF & Beta Prime scaling 
04/16/08   vm      Added FTM Cal support for QSC1100
03/28/08   vm      Added the support for Tx AGC interface with MDSP for QSC1100
03/21/08   sl      Added ftm_do_internal_device_calibration.
11/21/07   vm      Added More FTM Support for CGPS mode switching
11/09/07   ans     Fixed compiler warning.
11/08/07   vm      Added support for FTM API supporting CDMA Waveform, 
                   CW Waveform and Offset CW Waveform for Tx
11/08/07   ans     Featurizd changes with MSM7800 flag as it breaks other targets.
10/27/07   ans     Fixed the typo for band class 15.
10/25/07   bmg     MDM7800 interface updates
10/03/07   bmg     7600/7800 register port to FTM mainline, cleaned up some
                   compiler warnings, Rx Front constant cleanup.
08/29/07   anr	   Removed support for XO FT samples in the XO response packet. Will add
                   it as a log instead since it's for debug purposes.		
08/10/07   vm      Enabled the HDET when Tx is turned ON in FTM Mode.
                   This is done so that RF Cal reading for HDET always 
                   have sufficient warmup time
07/23/07   dyc     Place coarse_cal_cnt and fine_cal_cnt within XO calibration feature
07/23/07   anr     Changes for XO cal request and response packets. Also changed
                    the order in which the TCXO manager APIs are called.
07/14/07   zg      For cgps targets, switched to Aries API to enable GPS mode 
                   in FTM.
07/14/07   dyc     Added ftm_set_tx_xo_cal()
                   Remove debug XO Cal error in featurization.
07/13/07   anr     Added XO calibration APIs : ftm_do_xo_dc_cal() and 
                   ftm_do_xo_do_ft_curve_cal().
                   History entry updated to reflect actual date release. (dyc)
06/29/07   vm      Always enabled the HDET for Sc2x in FTM Mode
                   even after doing sweep
06/05/07   vm      Added more support for Beta Scaling on SC2X
06/04/07   aak     Fix for TRK LO not working when phone swticed from Online to FTM
06/02/07   vm      Added the support for Beta Scaling for Sc2X
05/02/07   anr     ftm_enc_stuff_zero_data_isr() and ftm_tx_sweep_cal_isr() now
                   use the step size set by ftm_set_tx_sweep_step_size
04/30/07   vm      Merged with SC2X file from QSC6055 Archive
04/18/07   vm      Added support for ADC and modem clock switching
                   for all the different modes of operation.
04/10/07   vm      Added the support for Tx Tone for SC2X
03/30/07   bmg     Added RF_HAS_INTERNAL_HDET_SUPPORT feature for RTR6500
03/21/07   dyc     dvga_gain compiler warning fix.
03/12/07   bmg     Common software IntelliCeiver support
                   RTR6500 integration
03/05/07   vm      Featurize the RF Library Calls until RF library
                   is merged for Common Code 
02/26/07   vm      Merge for Common archive
01/10/06   bmg     QSC60x5 updates
12/07/06   zg/ra   Update for gps_enter_mode.
12/15/06   vm      Fixed the PA Range Override in FTM Mode
12/15/06   bmg     Added basic PRx SC2x calibration support
12/01/06   vm      Enabled the Combiner Interrupt for fixing 
                   the TX Open loop interface of MDSP with HW
11/27/06   vm      Added the support for ADC Sampling Clock
                   derived from VCO
11/22/06   vm      Added the support for Firmware TX AGC
09/27/06   vm      Added the support for DVGA Resolution change to 1/10th 
                   of dB for SC2X
09/26/06   hkk     Added support for BC15, with featurization
09/21/06   dyc     Correct #if typo for RF_HAS_POWER_MODE_LIB
09/15/06   dyc     Added RF_HAS_POWER_MODE_LIB to code.
09/11/06   kss     Call new rf_hdr_init_agc() function to initialize AGC.
08/16/06   kss     Modified call to rfm_hdr_start_agc_acq() (new arg. added). 
08/01/06   mt      Added arguments for rf_hdr_start_agc_acq() .
07/30/06   dyc     Added RF_HAS_HDET_BYPASS featurization.
07/28/06   vm      Added the support for new RX Frront for SC2X

06/01/06   hkk     Fixed Lint errors
05/04/06   vm      Fixed the functions ftm_set_rf_mode() 
                   and ftm_set_cdma_band()
04/28/06   vm      Added the support for new API for RXTX Cal
03/20/06   ra      Fixed 2nd RX Chain mode switch: disable to OFS
03/07/06   go      Fixed  RxAGC. Changed to rfm_tune_band_chan
03/02/06   sar     Fixed merge errors in the last check-in.
03/02/06   zg/sar  Add RFR6185 support.
02/13/06   hkk     Put back mainlining BC14/11
02/07/06   hkk     Temporarily backing out version 50 and 51
01/20/06   ra      Call rfr_sleep_disable when setting rf mode to GPS
01/16/06   rmd     Implemented BC11 and BC14 code review results. Including:
                   * Mainline BC11 and BC14 code.
01/10/06   lp      Defined FTM_HAS_AGC_CARRIER_FREQ_ERR to adjust
                   TRK_LO_ADJ_VCTCXO PDM.
12/05/05   hkk     Further fixes for compile warnings
12/05/05   hkk     Fixes for compile warnings 
12/05/05   hkk     Fixes for Tx-Rx sweep cal
11/16/05   rmd     Enabled support for BC11 within ftm_set_rf_mode().
10/12/05   rmd     Added support for BC14 within ftm_set_rf_mode().
09/19/05   rmd     Added support for band class 14 (US PCS + 5MHz).
10/27/05   vm      Added the function ftm_synth_is_locked() for checking 
                   PLL lock status for synth device
10/24/05   ra      call rfm_tune_to_chan in ftm_tune_to_chan. replaces rfm_tune_band_chan
                   added support for FTM_HAS_TX_DACC_CONTROL
09/19/05   ra      Cancel timer service ISR when disabling ftm_tx_sweep_cal is disabled
09/15/05   ra      Tx sweep cal cleanup. Cancel tx_sweep_cal_enable and call timer_clr when turning off tx
09/15/05   hkk     Renamed the tx_rx sweep cal specific functions
09/13/05   hkk     Removed featurization for ftm_log.h for non HWTC builds - compile error fix
09/12/05   hkk     Changes for TX/RX sweep cal - added new functions
09/05/05   ra      Added adc_read at pulse in ftm_tx_sweep_cal_isr
08/16/05   ra      removed srchrdiv.h and replaced with srch_rx_div.h
08/15/05   ra      fixed logic that checks feature version for tx sweep cal type
08/01/05   ra      removed rfll1x_set_ftm_test_call_flag(). This function was only temporarily required
08/01/05   hkk     Added RFR6525 support
07/19/05   ra      changed ftm_tx_cal_sweep_interval and ftm_num_of_avg_hdet_read
                   to default values if violation in ftm_configure_tx_sweep_cal
06/17/05   ra      made a change for FTM_HAS_CFG_TX_SWP_CAL
06/13/05   ra      Added FTM_HAS_CFG_TX_SWP_CAL 
05/20/05   sar     Updated ftm_set_secondary_chain() to handle transitions between
                   Diversity and Dual modes.
05/30/05   ra      Added support for timer services for TX Sweep Cal when
                   FTM_LEGACY_TX_SWEEP_CAL is disabled
05/07/05   hkk     Added RFLL1X_set_ftm_test_call_flag to enable calls on secondary 
                   chain while on FTM.  
05/04/05   ra      Set tx_sweep_cal_enable to FALSE(OFF) when turning off TX.
05/02/05   ra      In ftm_set_secondary_chain, use RFCOM_RECEIVER_1
                   in rfm_disable_rx when leaving diversity mode.
04/26/05   ra      Allow DORA target tune to chan in HDR
03/31/05   lp      Fixed compile error in ftm_tune_to_chan when FEATURE_HDR
                   is disabled.
03/20/05   bmg     Added ftm_set_rf_power_mode API, DVGA cal support for
                   RFR6500, swiched to using rfm_tune_to_chan.
03/08/05   et      calling rf_init_pa_on_and_tx_on_ctls() at the top of
                   ftm_set_tx when turning the transmitter on, and 
                   calling rflib_set_1x_sbi() and rflib_init_comm() 
                   when transitioning back from GSM in ftm_set_rf_mode()
                   to restore CDMA sbi settings.
03/02/05   ra      Fixed compiler warning for DORA
02/16/05   ra      Added TRAMP change to support Modem 2nd level interrupts
01/24/05   jac/ra  DORA mergeAdded call to put DSP into pass through mode in ftm_set_tx.
11/30/04   ra      More changes for configurable tx sweep cal. Also changed
                   from legacy messages to ftm message macro.
11/29/04   ra      Removed rfm_enable_rx() and rfm_init_digital from 
                   ftm_set_cdma_band. These will be called in 
                   ftm_activate_protocol
11/29/04   jb      Changed message types from legacy to FTM messages.
11/12/04   ra      Added call to rfm_enable_rx in ftm_set_rf_mode
                   Added rfm_enable_rx(RFCOM_TRANSCEIVER_0, NULL, NULL)
                   to ftm_rx_on_second_chain. Also fixed compiler warning. 
11/03/04   mpa     Removed dependency on std.h (merged with sys.h)
10/31/04   ra      Added ftm_configure_tx_sweep_cal function
10/12/04   ra      Optimized logic to allow transition from GPS to 1x
10/11/04   ra      Added call rfm_digital_band_select in ftm_set_cdma_band to
                   allow transition from GPS to 1x
09/30/04   wd      Fixed TX_SET_PDM for MSM6700 is now 10 bits resolution.
09/30/04   wd      Fixed TX_SWEEP for MSM6700 (was getting 2x interupt).
09/30/04   jac/wd  Added MSM6700 specific code for turning on TX in FTM mode.  
09/20/04   bmg     Updated calling procedure for rf_pre_tune_to_gps()
07/17/04   bmg     Replaced HDET selection code with generic calls to
                   rfc_get_hdet_adc_channel for clean support of RF cards
09/07/04   ra      Fixed a couple of compiler warnings
08/10/04   bmg     Added runtime RF card fixes for HDET and GetDVGAOffset.
                   Mainlined FEATURE_MULTI_RX_CHAIN.
                   Moved rfm_init_digital to before band selection to re-enable
                   LNA tracking.
07/14/04   bmg     Fixed 6150 HDR featurization, unused code cleanup,
                   Removed KPCS featurization for set_mode in FEBC
06/29/04   ra      Continued change for CDMA 450(BC4) ftm_get_dvga_offset
06/23/04   jac     Fixed reference to FTM_PHONE_MODE_CDMA_1800 for KPCS builds.
06/23/04   bmg     Extended DVGA and LNA settling time wait
06/16/04   ap      Moved FEATURE_HDR in ftm_tune_to_chan for 6150.
06/02/04   bmg     Featurized HDR for 6150
06/02/04   fpe     Changed support for FEATURE_ENHANCED_BAND_CLASS
05/27/04   ra      Added support for FEATURE_ENHANCED_BAND_CLASS
05/27/04   bmg     Added closed-loop HDET tracking API
05/18/04   rsr     Added support for GSM
05/18/04   rsr     Appended ftm_ to pdm_id_type and mode_id_type enums.
05/18/04   rsr     Updated ftm_Set_cdma_band to setup rf when it is in sleep.
                   This is required to fix CDMA ftm when SRCH_INACTIVE_STATE is 
                   used.
05/10/04   ra      Added support for CDMA 450 (BC4)
04/26/04   bmg     Removed unused DFM function ftm_set_freq_sense_gain()
04/12/04   ra      Removed local HDR definitions. Use externalized versions( values did not change)
04/09/04   ra      Modified ftm_set_cdma_band to enable clock regimes differently if GPS is defined
03/15/04   ra      enable second rx chain during test call on second rx chain
03/09/04   ra      renamed FEATURE_FTM_BYPASS_NV_CAL_POINTS to FEATURE_FTM_BYPASS_LNA_NV_CAL_POINTS
                   removed obsolete FTM_USES_ABSOLUTE_LNA_CAL
                   use srch4_hw_init to for entry into GPS mode
02/09/04   ra      fixed complile error: redifinition of variable found in jaguar_cdma.h
01/30/04   bmg     Set sample server to the proper mode based on FTM RF band.
12/18/03   bmg     Moved TX_GAIN_ADJ setting to TX on function, and set it to 0
11/24/03   ra      Modified ftm_set_rf_mode and ftm_tune_to_chan to handle IMT band
11/21/03   bmg     Extended second chain call API to support extra modes
11/14/03   bmg     Moved second chain call helper into 1x commands
11/03/03   ra      Added logic to leave tx gain adj alone if we are setting mode
                   on the secondary chain. Changed to  #ifdef FEATURE_MULTI_RX_CHAIN
                   from #ifndef in external gloabal variables area
10/29/03   bmg     Collected repeated code into ftm_set_cdma_band()
10/23/03   bmg     HDR/1x mode cleanup
10/20/03   bmg     Added HDR RF mode
10/20/03   ra      Added support for band class 6 in ftm_set_rf_mode
10/13/03   zg      Removed a call to an empty function clk_regime_config_gps_rf_clocks
10/08/03   bmg     RFM fixes for set_secondary_chain
                   Added experimental feature for passed in AGC calibration
09/24/03   bmg     Modifications for RFM driver
09/16/03   bmg     Fix for diversity band switching, code cleanup, DB removal
09/12/03   bmg     Removed RF driver DB dependencies
09/08/03   ra      renamed enc_stuff_zero_data_isr to ftm_enc_stuff_zero_data_isr
09/08/03   ra      Renamed GetHdet function to ftm_get_hdet_from_tx_cal_sweep.
                   The new name reflects the binding of the stored HDET values taken during
                   TX CAL SWEEP. If the user wishes to read HDET, the FTM_GET_ADC_VAL api
                   should be used
09/05/03   bmg     Removed unused FM code.  Improved DVGA cal algorithm
08/20/03   bmg     Implemented first shot at RF set diversity API
07/24/03   bmg     Fixes for building without FEATURE_MULTI_RX_CHAIN defined
07/22/03   bmg     Added calling support for multi-chain RF driver
                   Converted test_* names to ftm_* names
07/11/03   bmg     Removed unused FEATURE_FTM_HAS_5_LNA_STAGE
07/09/03   bmg     Merged in Couger 3030->4040 changes:
  04/25/03 wd      Remove call to test_enc_init during TX_ON, we don't need it
                   and it causes glitches in the TX_ON line.
  04/25/03 ra      modifed enc_stuff_zero_data to handle new tx sweep cal data
                   algorithm
  05/09/03 wd      Add wait for AGC to settle after gain switch.
  04/30/03 wd      Fixed HDET reading problem in enc_stuff_zero_data_isr.

06/12/03   bmg     Modified update the LNA min register calculation
06/12/03   bmg     Updated to LNA offset also update LNA min register
06/11/03   dyc     Updated to use RXF_RF_VGA_GAIN_SCALE_GPS_V.
06/05/03   bmg     Register porting, support for RFR6125 and reading from
                   the LNA fall NV item again (fall 1 for RFR6125, 2 otherwise)
05/28/03   bmg     Backed out GetDVGAGainOffset change.  No longer requires
                   SetLNARange sequence, uses hard coded input power of -91dBm
05/27/03   bmg     GetDVGAGainOffset requires SetLNARange to have been set
                   before it is called to determine switching point.
                   SetPAR1R1 now works
                   No longer use test_set_lna_range -- all calls go to RF driver
05/20/03   bmg     Brought TX enable/disable back to mainline encoder functions
05/20/03   bmg     Featurized use of ftm_leds and added shift to TRK_LO
05/19/03   bmg     Added TX calibration support and brought in latest TX cal sweep
04/30/03   bmg     Jaguar fix for PDM registers
04/23/03   bmg     Jaguar support for reading AGC register
03/19/03   wd      Fixed warning with missing prototype, and remove tab.
03/07/03   bmg     Added PHONE_MODE_GPS to test_set_mode and enabled
                   switching back to CDMA
03/07/03   wd      Remove RF_HAS featurization from functions needed by lib.
03/05/03   bmg     Fixed sound output when plugging/unplugging headset
02/28/03   wd      Changed busy wait for REX wait
02/26/03   wd      Move overide TX_GAIN_ADJ accumulator so it will be done on
                   all modes.
02/17/03   wd      remove include for ftm_im2.h not needed. Also clean up.
02/07/03   wd      Changed calls to iqmismatch and get_im2, moved to
                   ftm_im2_lib.c, and clean up.
02/07/03   wd      Rename file from ftm_rf_1x_ctl.c -> ftm_1x_ctl.c
02-07-03   wd      Fixed diagblock fiasco...
02-03-03   wd      Overide TX_GAIN_ADJ accumulator in test_set_mode for CELL & CDMA
01-31-03   wd      Take out GPS SINAD when RF_HAS_FM is not enabled.
01-29-03   wd      Added an extra LNA range for ZRF612X in rf_set_lna_range.
01/02/03   ra      decoupled FM from GPS SINAD
01/02/03   ra      use diag utils to synchronize adsp image load
12/17/02   ra      removed call to load dfm image(test_init_qdsp_dfm).need to resolve
12/11/02   ra      mainlined CDMA1X/AMPS
10-04-02   ra      Created from ftm_rf.c.
===========================================================================*/

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"

#ifdef FEATURE_FACTORY_TESTMODE

#include "ftm_sys_ctl.h"

#include "ftm_task.h"
#include "ftm_cmd.h"
#include "ftm_1x_ctl.h"
#include "msm_help.h"
#include "clkregim.h"
#include "rf.h"
#include "rflib.h"
#include "adc.h"
#include "enci.h"
#include "snd.h"
#include "voc.h"
#include "vocdata.h"
#include "qdspcmd.h"
#include "rfm.h"
#include "rxc.h"
#include "rf_hal.h"
#include "rfr.h"
#include "rfll1x.h"
#include "diagpkt.h"
#include "diagi.h"
#include "uixsnd.h"
#include "math.h"
#include "rfzifapi.h"
#ifdef T_MSM7800
#error code not present
#endif
#ifdef FEATURE_GSM
#error code not present
#endif

#ifdef T_MSM6100
  #include "msm_drv.h"
#endif /* T_MSM6100 */


#include "srch_rx_div.h"
#include "sample_server.h"

#ifdef FEATURE_HDR
#error code not present
#endif

#ifdef RF_HAS_POWER_MODE_LIB
  #include "rf_intelliceiver.h"
#endif /* RF_HAS_POWER_MODE_LIB */

#ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO
#include "rf_adc_samp_freq.h"
#endif 

#include "ftm_log.h"

#ifdef RF_HAS_MDSP_TX_AGC
#include "rf_mdsp_tx_agc.h"
#endif

#if defined( T_MSM6700 ) || defined( T_MSM6800 )
#include "muxmdsp.h"
#endif /* defined( T_MSM6700 ) || defined( T_MSM6800 ) */

#ifdef FTM_HAS_SUPORT_FOR_VARIABLE_FREQ_OFFSET_TONE
#include "mdsp.h"
#endif

#ifdef RF_HAS_INTERNAL_HDET_SUPPORT
#include "rft.h"
#endif /* RF_HAS_INTERNAL_HDET_SUPPORT */

#ifdef FTM_HAS_CGPS
#error code not present
#endif 

#ifdef FEATURE_GSM
#error code not present
#endif

#ifdef RF_SUPPORTS_32_BIT_TX_AGC_INTERFACE
#define FTM_RF_TX_MAX_PDMS_SUPPORTED 63
byte ftm_rf_tx_pa_r1_r0_val = 0x0;
extern rft_pa_states_type qsc11x0_num_pa_states[16];
#endif

/*===========================================================================

                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/
#ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO
extern rf_adc_clk_state_type rf_adc_clk_status;
#endif
extern rf_path_enum_type rfcom_to_path[];
extern rf_chain_state_type rf_chain_status[];
extern rf_path_0_config_type rf_path_0;  
extern rf_path_1_config_type rf_path_1;

#ifdef FTM_SC2X_HAS_HWTC_TX_TONE_ENABLED
boolean ftm_tx_tone_gen_enabled = FALSE;
#endif

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

/*===========================================================================

                   LOCAL GLOBAL VARIABLES and CONSTANTS

===========================================================================*/

#ifdef FTM_HAS_CGPS
#error code not present
#endif

#define TX_SWEEP_CAL_MAX 450
#define TX_SWEEP_CAL_STEP 10
#if defined (FTM_HAS_CFG_TX_SWP_CAL)
  ftmclk_struct_type ftmclk;
  LOCAL boolean tx_sweep_pulse_flag = FALSE;
#elif defined (FTM_LEGACY_TX_SWEEP_CAL)
  LOCAL uint8 g_course_loop = 0;
#else
  #error Must define a TX Sweep Cal type
#endif
LOCAL boolean tx_sweep_cal_enable = FALSE;
LOCAL uint16 tx_sweep_cal_adj = 0;
LOCAL uint8 g_loopcount = 0;
LOCAL adc_logical_channel_type hdet_adc = NUMBER_OF_LOGICAL_ADC_CHANNELS;

#define DVGA_OFFSET_AVG 12

#define FTM_BETA_PRIME_MDSP_RESOLUTION_MIN 1
#define FTM_BETA_PRIME_MDSP_RESOLUTION_MAX 640

LOCAL int2 wVgaGainOffset = 0;
LOCAL uint16 g_tx_agc_adj = 0;
LOCAL int2 g_dvga_offset=0;
LOCAL word g_lna_offset0=0;
LOCAL word g_lna_offset1=0;
LOCAL word g_lna_offset2=0;
LOCAL word g_lna_offset3=0;

#ifdef RF_HAS_POWER_MODE_LIB
LOCAL int2 ftm_dvga_offset_pm[RFLIB_MAX_POWER_MODE];
LOCAL int2 ftm_lna_offset_pm[4][RFLIB_MAX_POWER_MODE];
#endif

LOCAL rflib_power_mode_type ftm_current_power_mode;

hdet_tracking_params_type *hdet_tracking_params_ptr;

/*#define HDET_ARRAY_SIZE 45

LOCAL word g_hdet[HDET_ARRAY_SIZE] =
                        {0,1,2,3,4,5,6,7,8,9,10,11,
                        12,13,14,15,16,17,18,19,20,21,22,23,
                        24,25,26,27,28,29,30,31,32,33,34,
                        35,36,37,38,39,40,41,42,43,44};*/

#ifndef RF_HAS_BETA_SCALING_TX_AGC
/* Here Beta Scaling is not enabled */
#define HDET_ARRAY_SIZE  45
LOCAL word g_hdet[HDET_ARRAY_SIZE] =
                        {0,1,2,3,4,5,6,7,8,9,10,11,
                        12,13,14,15,16,17,18,19,20,21,22,23,
                        24,25,26,27,28,29,30,31,32,33,34,
                        35,36,37,38,39,40,41,42,43,44};
#else
/* Here Beta Scaling is enabled */
/* These increases are needed to generate 48 measurements in TX cal sweep */
#define HDET_ARRAY_SIZE  100
LOCAL word g_hdet[HDET_ARRAY_SIZE] =
                        {0,1,2,3,4,5,6,7,8,9,10,11,
                        12,13,14,15,16,17,18,19,20,21,22,23,
                        24,25,26,27,28,29,30,31,32,33,34,
                        35,36,37,38,39,40,41,42,43,44,45,46,47,48,
                        49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,
                        65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,
                        81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,
                        97,98,99};
#endif

/* Lower limit for tx sweep interval in units of sleep clock periods */
#define FTM_TX_SWEEP_INTERVAL_LOWER_LIMIT 2  /* Approx. 2 ms */

                        /* Length of each step of the tx cal sweep, units are sleep clock periods */
 uint16 ftm_tx_cal_sweep_interval = 20; /* 20 ms */

/* Number of HDET readings to take during each step of the tx cal sweep */
byte ftm_num_of_avg_hdet_read = 10;

#define ROUND_VAL(a) (((a) >= 0) ? ((a) + 0.5) : ((a) - 0.5))

#if defined( T_MSM6700 ) || defined( T_MSM6800 )
/* PCG Interrupt mask, only service this interrupt */
#define TX_PCG_INT_MASK 0x01
#define MOD_TX_FR_INT_1X_MODE_STATUS  0x1
#define MOD_TX_FR_INT_HDR_MODE_STATUS 0x2
#define MOD_TX_FR_INT_1X_MODE_EN      0x1
#define MOD_TX_FR_INT_HDR_MODE_EN     0x2
#define MOD_TX_FR_INT_1X_MODE_MASK    0x1
#define MOD_TX_FR_INT_HDR_MODE_MASK   0x2
#endif


#define FTM_NUM_OF_AVG_RX_AGC_READ    10
#define FTM_NUM_OF_AVG_HDET_READ      10
#define FTM_UPLINK_SWEEP_CAL_MIN      0
#define FTM_UPLINK_SWEEP_CAL_MAX      511
#define FTM_UPLINK_SWEEP_CAL_MIN_STEP 1
#define FTM_UPLINK_SWEEP_CAL_SIZE     (((FTM_UPLINK_SWEEP_CAL_MAX - FTM_UPLINK_SWEEP_CAL_MIN)/FTM_UPLINK_SWEEP_CAL_MIN_STEP) + 1)






#define FTM_UPLINK_SWEEP_CAL_MIN_STEP 1
#define FTM_TX_RX_SWEEP_NO_ACTION 0xFF
#define FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ 21
#define FTM_TX_RX_FREQ_CAL_MAX_FREQS 16
#define FTM_FREQ_CAL_SWEEP_NUM_HDET_AVG 5
#define FTM_TX_RX_MIN_STEP_LENGTH 01 /* 655 sleep clock cycles ~= 20 ms */
#define FTM_TX_RX_SWEEP_HDET_SETTLE_TIME 400 // As per RF Systems:  ~20 us HDET auto-cal + ~220 us HDET LPF settling + 150 headroom
typedef enum
{
  FTM_TX_RX_SUCCESS          = 0,
  FTM_TX_RX_STEPS_INVALID    = 1,
  FTM_TX_RX_FREQS_INVALID    = 2,
  FTM_TX_RX_STEP_LEN_INVALID = 4,
} ftm_tx_rx_error_code_type;

#ifdef FTM_COMPOSITE_CAL_ISR_DEBUG
timetick_type timer_ftm_tx_rx_freq_cal_segment_dur; //Used to keep track of segment durations.
#endif

static uint8  ftm_uplink_sweep_cal_step = TX_SWEEP_CAL_STEP;

static int16 ftm_tx_rx_freq_cal_current_segment = -1;

static uint16 ftm_tx_rx_freq_cal_channels[FTM_TX_RX_FREQ_CAL_MAX_FREQS];

static uint8  ftm_tx_rx_freq_cal_pa_ranges[FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ];

static uint16 ftm_tx_rx_freq_cal_pdm_list[FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ];

static uint8  ftm_tx_rx_freq_cal_hdet_read_list[FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ];

static uint8  ftm_tx_rx_freq_cal_rx_actions[FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ];

static int16  ftm_tx_rx_freq_cal_exp_agc[FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ];

static int16  ftm_tx_rx_freq_cal_rx_result[FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ*FTM_TX_RX_FREQ_CAL_MAX_FREQS];

static uint16 ftm_tx_rx_freq_cal_tx_result[FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ*FTM_TX_RX_FREQ_CAL_MAX_FREQS];

static uint8  ftm_tx_rx_freq_cal_rx_lpm_list[FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ];

static uint16  ftm_tx_rx_chain;

static uint8  ftm_tx_rx_steps_per_freq=0;
static uint8  ftm_tx_rx_num_freqs=0;

#if defined (FTM_HAS_XO_FACTORY_CAL) 
// XO Calibration information parameter pointers
tcxomgr_cal_fac_param_type ftm_fac_cal_params;
tcxomgr_cal_info_type ftm_info_pkt;
tcxomgr_cal_ft_sample_type ftm_samples;
static uint8 coarse_cal_cnt = 0;
static uint8 fine_cal_cnt = 0;
#endif

/*===========================================================================

                                 MACROS

===========================================================================*/

/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/
#if !defined T_QSC1100
#ifdef FEATURE_CLKREGIM_2X_MODE
extern void srch4_hw_init(byte,sample_server_rx_chain_type);
#endif
#endif

#ifdef FTM_HAS_CGPS
#error code not present
#endif
/*===========================================================================

FUNCTION FTM_GET_RFR_DEVICE_TYPE

DESCRIPTION
  This function returns an identifier specifying the model of RFR
  on the specified RFCOM device

PARAMETERS

DEPENDENCIES
  None

RETURN VALUE
  rfr_device_enum

SIDE EFFECTS
  None

===========================================================================*/
rfr_device_enum ftm_get_rfr_device_type(rfcom_device_enum_type device)
{
  rf_path_enum_type path = rfcom_to_path[device];

  if (path == RF_PATH_0)
  {
    return rf_path_0.rfrdev;
  }
  else if (path == RF_PATH_1)
  {
    return rf_path_1.rfrdev;
  }
  else
  {
    ERR_FATAL("Invalid path (%d) for device %d", path, device, 0);
    return NO_RFR;
  }
}

/*===========================================================================

FUNCTION GET_HDET_FROM_TXCAL_SWEEP

DESCRIPTION
  This function returns a pointer to the Hdet array. The hdet values in the g_hdet array were collected
  during the TX CAL Sweep function. Each entry is bound to a tx_agc_pdm value taken during TX CAL Sweep.
  If the user wishes to read hdet values unrelated to TX CAL Sweep, the FTM_GET_ADC_VAL API should be used .

PARAMETERS

DEPENDENCIES
  None

RETURN VALUE
  word

SIDE EFFECTS
  None

===========================================================================*/
word  ftm_get_hdet_from_tx_cal_sweep(word index)
{
  return g_hdet[index];
} /* End GetHdet */

/*===========================================================================

FUNCTION FTM_SET_PDM

DESCRIPTION
    This function takes a pdm value and writes it to the specified PDM
    control register on the MSM2P.  It automatically opens the control
    loop so that the microprocessor can manually control it.  The loop
    will stay open until the test_pdm_close() function is explicitly
    called for a PDM. It will also determine the current operating mode
    to adjust the correct PDM.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   Will automatically OPEN the PDM loop even if it was previously closed.
   This allows applications a more intuitive approach to using this function.
===========================================================================*/
void ftm_set_pdm(ftm_pdm_id_type pdm_id, int2 val)
{

  rf_chain_state_type *rf_chain_ptr;

  /* ----------------------------------------
  ** Figure out what mode we are currently in
  ** ---------------------------------------- */
  /* point to chain status structure for the device */
  rf_chain_ptr = &rf_chain_status[ rfcom_to_path[ RFCOM_TRANSCEIVER_0 ] ];


  /* Find the PDM which we want to adjust and set it to the value. */
  switch (pdm_id)
  {
  case FTM_PDM_TX_AGC_ADJ:
    g_tx_agc_adj = val;

    if (rf_chain_ptr->rf_mode == DB_RF_CDMA || rf_chain_ptr->rf_mode == DB_RF_PCN)
    {
      ftm_cdma_set_tx_agc(val);
    }
    break;
  
  case FTM_PDM_TRK_LO_ADJ:
    val = val - 256;   /* shift input from 0 to 511 to be from -256 to +255 */
    ftm_cdma_set_trk_lo(val);
    break;

  default:
    FTM_MSG_ERROR("Unsupported PDM identifier: %d", pdm_id, 0, 0);
    break;

  } /* switch */
} /* End ftm_set_pdm */

/*===========================================================================

FUNCTION RF_CDMA_SET_TX_AGC

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_cdma_set_tx_agc
(
  int2 set_level        /* 9 bit PDM value to load */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 #ifdef RF_HAS_MDSP_TX_AGC
 #ifdef RF_SUPPORTS_32_BIT_TX_AGC_INTERFACE
 uint16 set_level_mdsp_pdm;
 #else
 int2 set_level_mdsp_pdm ;
 #endif
 #endif

  set_level = RF_SATURATE( set_level, 0, 63 );

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */
  {
#ifdef T_MSM6700
  /* AGC_ADJ is one more bit for MSM6700 */ 
    #ifdef RF_HAS_MDSP_TX_AGC
    #ifdef RF_SUPPORTS_32_BIT_TX_AGC_INTERFACE    

    set_level_mdsp_pdm = (uint16)(set_level);

    if(set_level_mdsp_pdm > FTM_RF_TX_MAX_PDMS_SUPPORTED)
    {
      set_level_mdsp_pdm = FTM_RF_TX_MAX_PDMS_SUPPORTED;
    }
    #else
    set_level_mdsp_pdm = (int2)(((set_level -256) * 128)& 0xffff);
    #endif
    #else    
    set_level = set_level * 2; 
    #endif

    MSM_W_MASKED(MOD_TX_AGC_ADJ_CTL, TX_AGC_ADJ_OE, 1);

    #ifdef RF_HAS_MDSP_TX_AGC
    MSM_W_MASKED(MOD_TX_AGC_ADJ_CTL, TX_AGC_ADJ_SEL, 2);
    #else
    MSM_W_MASKED(MOD_TX_AGC_ADJ_CTL, TX_AGC_ADJ_SEL, 0x01);
    #endif
    
    #ifdef RF_HAS_MDSP_TX_AGC 
    #ifdef RF_HAS_BETA_SCALING_TX_AGC
    /* Here Beta Scaling is enabled */
    CDMA_MDSP_SET_TX_BETAP_VAL (0x0);
    CDMA_MDSP_ENABLE_TX_BETAP_OVERRIDE();
    #endif        

    #ifdef RF_SUPPORTS_32_BIT_TX_AGC_INTERFACE
    rft_set_tx_agc_adj_val(set_level_mdsp_pdm, ftm_rf_tx_pa_r1_r0_val);
    #else         
    CDMA_MDSP_SET_TX_AGC_ADJ_VAL( set_level_mdsp_pdm );
    #endif
    CDMA_MDSP_ENABLE_TX_AGC_ADJ_OVERRIDE( );
    #else
    // write to  TX AGC ADJ register
    HWIO_OUTM(MOD_TX_AGC_ADJ,0x3ff, set_level);
    #endif

#else
    MSM_W_MASKED(TX_AGC_CTL, TX_AGC_ADJ_OE, 1);
    MSM_W_MASKED(TX_AGC_CTL, TX_AGC_ADJ_OVERRIDE_N, 0);
    // write to  TX AGC ADJ register
    HWIO_OUTM(TX_AGC_ADJ_WR,0x1ff, set_level);
#endif
  }
} /* End rf_cdma_set_tx_agc() */

/*===========================================================================

FUNCTION FTM_SET_PA_R1_R0

DESCRIPTION
  This function sets PA range bits.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_set_pa_r1_r0
(
  byte r1_r0  /* 0,1,2,3 for r1,r0 = 0,0  0,1  1,0  1,1 */
)
{
  #ifdef RF_HAS_TX_DAC_BACKOFF_DIFF_FROM_LEGACY
  extern byte rf_current_pa_r_map;
  byte pa_gain_state = 0;
  boolean ro_is_pa_state_determined = TRUE;
  #endif
  
  #ifdef RF_SUPPORTS_32_BIT_TX_AGC_INTERFACE
  ftm_rf_tx_pa_r1_r0_val = (~r1_r0) & 0x3;
  #endif

  #ifdef RF_HAS_TX_DAC_BACKOFF_DIFF_FROM_LEGACY
  if(qsc11x0_num_pa_states[rf_chain_status[0].rf_curr_cdma_band] == RFT_DUAL_STATE_PA)
  {
    if (((rf_current_pa_r_map >> 1) & 0x1) == ((rf_current_pa_r_map >> 3) & 0x1))
    {
      if ((rf_current_pa_r_map & 0x1) != ((rf_current_pa_r_map >> 2) & 0x1))
      {      
        ro_is_pa_state_determined = TRUE;
      }
    }
    else if ((rf_current_pa_r_map & 0x1) == ((rf_current_pa_r_map >> 2) & 0x1))
    {
      if (((rf_current_pa_r_map >> 1) & 0x1) != ((rf_current_pa_r_map >> 3) & 0x1))
      {    
        ro_is_pa_state_determined = FALSE;
      }
    }
    else
    {
      ro_is_pa_state_determined = TRUE;

      RF_FATAL_ERROR("2 States for PA Can be changed by either R0 or R1 only");
    }
  
    if (ro_is_pa_state_determined == TRUE)
    {
      if ((rf_current_pa_r_map & 0x1) == (ftm_rf_tx_pa_r1_r0_val & 0x1))
      {
        pa_gain_state = 0;
      }
      else
      {
        pa_gain_state = 1;
      }
    }
    else /*r1 is PA State Determininer*/
    {
      if ( ((rf_current_pa_r_map >> 1) & 0x1) == ((ftm_rf_tx_pa_r1_r0_val >> 1)& 0x1))
      {     
        pa_gain_state = 0;      
      }
      else
      {
        pa_gain_state = 1;
      }  
    }
  }
  else if(qsc11x0_num_pa_states[rf_chain_status[0].rf_curr_cdma_band] == RFT_TRI_STATE_PA)
  {
    if((rf_current_pa_r_map & 0x3) == (ftm_rf_tx_pa_r1_r0_val & 0x3))
    {
      pa_gain_state = 0;
    }
    else if(((rf_current_pa_r_map >> 2) & 0x3) == (ftm_rf_tx_pa_r1_r0_val & 0x3))
    {
      pa_gain_state = 1;
    }
    else if(((rf_current_pa_r_map >> 4) & 0x3) == (ftm_rf_tx_pa_r1_r0_val & 0x3))
    {
      pa_gain_state = 2;
    }
    else
    {
      RF_FATAL_ERROR("Inavild PA Range for the 3 State PA");
    }
  }
  else
  {
    RF_FATAL_ERROR("Inavild number of PA Gain States for the Current Tx BAnd");
  }

  CDMA_MDSP_SET_PA_STATE(pa_gain_state);

  CDMA_MDSP_ENABLE_PA_STATE_OVERRIDE();
  #endif/*RF_HAS_TX_DAC_BACKOFF_DIFF_FROM_LEGACY*/

  #ifdef RF_HAS_MDSP_TX_AGC
  CDMA_MDSP_SET_PA_R0_R1_OVERRIDE_VAL((~r1_r0) & 0x3);
  CDMA_MDSP_ENABLE_PA_RANGE_OVERRIDE( );
  #else
  MSM_W_MASKED(TX_AGC_CTL, PA_R_OVERRIDE_N, 0);
  MSM_W_MASKED(TX_AGC_CTL, PA_R_WR, ~r1_r0);
  #endif   
} /* End rf_set_pa_r1_r0() */

/*===========================================================================

FUNCTION RF_SET_LNA_OFFSET

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_set_lna_offset(rfcom_device_enum_type device, byte index, int2 setting)
{
  int min;

  setting = RF_SATURATE( setting, 0, 1023 );
  min = RF_GET_RX_AGC_MIN_VAL(setting);

  FTM_MSG_MED("ftm_set_lna_offset state %d, value %d, min %d",
               index, setting, min);

  switch (index)
  {
  case 0:  /* LNA 1 Bypassed Non-Turbo Mode Offset (Traditional LNA Offset) */
#ifdef T_MSM7800
#error code not present
#else
    HWIO_OUTI2(RX_AGCc_LNA_n_OFFSET, rfcom_to_path[device], 1, setting);
    HWIO_OUTI2(RX_AGCc_VALUE_n_MIN, rfcom_to_path[device], 1, min);
#endif
   break;
  case 1: /* LNA 2 Bypassed Non-Turbo Mode Offset */
#ifdef T_MSM7800
#error code not present
#else
    HWIO_OUTI2(RX_AGCc_LNA_n_OFFSET, rfcom_to_path[device], 2, setting);
    HWIO_OUTI2(RX_AGCc_VALUE_n_MIN, rfcom_to_path[device], 2, min);
#endif
    break;
  case 2: /* LNA 3 Bypassed Non-Turbo Mode Offset */
#ifdef T_MSM7800
#error code not present
#else
    HWIO_OUTI2(RX_AGCc_LNA_n_OFFSET, rfcom_to_path[device], 3, setting);
    HWIO_OUTI2(RX_AGCc_VALUE_n_MIN, rfcom_to_path[device], 3, min);
#endif
    break;
  case 3: /* LNA 4 Bypassed Non-Turbo Mode Offset */
#ifdef T_MSM7800
#error code not present
#else
    HWIO_OUTI2(RX_AGCc_LNA_n_OFFSET, rfcom_to_path[device], 4, setting);
    HWIO_OUTI2(RX_AGCc_VALUE_n_MIN, rfcom_to_path[device], 4, min);
#endif
    break;
  default:
    break;
  }
}

#ifdef FTM_HAS_CGPS
#error code not present
#endif /*FTM_HAS_CGPS*/

/*===========================================================================

FUNCTION FTM_SET_RF_MODE

DESCRIPTION
    This function will switch modes to Digital, Analog, or Sleep based
    on the entry parameter.

DEPENDENCIES
   In FM mode we cannot initialize the Vocoder until we have a CHIPX8
   present.  This requires the BB2_IDLE/ to be inactive.  Because of
   this dependency in FM mode we initialize the Vocoder only on an
   as needed basis.  In CDMA mode it is done ONLY ONCE every time
   we enter CDMA mode as CHIPX8 is always present unless we are sleeping.

RETURN VALUE
   None.

SIDE EFFECTS
   When a mode is changed WHILE transmitting, this function will
   automatically shut down the transmit for the old mode and enter the
   transmit state for the new mode.

===========================================================================*/
LOCAL void ftm_set_rf_mode(rfcom_device_enum_type device, ftm_mode_id_type mode)
{
  rf_chain_state_type *rf_chain_ptr;

 
  ftm_rfmode_1x_cal();

  #ifdef RF_HAS_SUB_BLOCK_SUPPORT
  //Clear the Block Mask for next Band tuned
  rf_clear_block_mask_override(device);
  #endif

  /* point chain status structure for the device */
  rf_chain_ptr = &rf_chain_status[ rfcom_to_path[ device ] ];

  /* can not change states when transmitting */
  if (rf_chain_ptr->rf_state == RF_STATE_CDMA_RXTX)
    ERR_FATAL( "Invalid RF state change: %d", rf_chain_ptr->rf_state, 0,0 );

  #ifdef FTM_HAS_CGPS
#error code not present
  #endif

  switch (mode)
  {
#ifdef RF_HAS_GPS
#error code not present
#endif /* RF_HAS_GPS */

  case FTM_PHONE_MODE_CDMA_1800:
    ftm_set_cdma_band( device, RF_BC4_BAND );
    #if defined(T_MSM7800) || defined(T_QSC1100) 
      sample_server_set_chain_mode((sample_server_rx_chain_type)rfcom_to_path[device],
                                   SAMPLE_SERVER_CARRIER_0, RTC_1X, CONTINUOUS_SAMPLE_MODE);
    #else 
      sample_server_set_chain_mode((sample_server_rx_chain_type)rfcom_to_path[device],
                                   RTC_1X, CONTINUOUS_SAMPLE_MODE);
    #endif
    FTM_MSG_MED("Set Mode to CDMA 1800",0,0,0);
    break;

  case FTM_PHONE_MODE_CDMA:
  case FTM_PHONE_MODE_CDMA_800:
    ftm_set_cdma_band( device, RF_BC0_BAND );
    #if defined(T_MSM7800) || defined(T_QSC1100)
    sample_server_set_chain_mode((sample_server_rx_chain_type)rfcom_to_path[device],
                                 SAMPLE_SERVER_CARRIER_0, RTC_1X, CONTINUOUS_SAMPLE_MODE);
    #else 
    sample_server_set_chain_mode((sample_server_rx_chain_type)rfcom_to_path[device],
                                 RTC_1X, CONTINUOUS_SAMPLE_MODE);
    #endif
    FTM_MSG_MED("Set Mode to CDMA 800",0,0,0);
    break;

  case FTM_PHONE_MODE_JCDMA:
    ftm_set_cdma_band( device, RF_BC3_BAND );
    #if defined(T_MSM7800) || defined(T_QSC1100)
    sample_server_set_chain_mode((sample_server_rx_chain_type)rfcom_to_path[device],
                                 SAMPLE_SERVER_CARRIER_0, RTC_1X, CONTINUOUS_SAMPLE_MODE);
    #else 
    sample_server_set_chain_mode((sample_server_rx_chain_type)rfcom_to_path[device],
                                 RTC_1X, CONTINUOUS_SAMPLE_MODE);
    #endif
    FTM_MSG_MED("Set Mode to JCDMA 800",0,0,0);
    break;

  case FTM_PHONE_MODE_IMT:
    ftm_set_cdma_band( device, RF_BC6_BAND );
    #if defined(T_MSM7800) || defined(T_QSC1100)
    sample_server_set_chain_mode((sample_server_rx_chain_type)rfcom_to_path[device],
                                 SAMPLE_SERVER_CARRIER_0, RTC_1X, CONTINUOUS_SAMPLE_MODE);
    #else 
    sample_server_set_chain_mode((sample_server_rx_chain_type)rfcom_to_path[device],
                                 RTC_1X, CONTINUOUS_SAMPLE_MODE);
    #endif
    FTM_MSG_MED("Set Mode to 1X IMT",0,0,0);
    break;

  case FTM_PHONE_MODE_CDMA_1900:
    ftm_set_cdma_band( device, RF_BC1_BAND );
    #if defined(T_MSM7800) || defined(T_QSC1100)
    sample_server_set_chain_mode((sample_server_rx_chain_type)rfcom_to_path[device],
                                 SAMPLE_SERVER_CARRIER_0, RTC_1X, CONTINUOUS_SAMPLE_MODE);
    #else 
    sample_server_set_chain_mode((sample_server_rx_chain_type)rfcom_to_path[device],
                                 RTC_1X, CONTINUOUS_SAMPLE_MODE);
    #endif
    FTM_MSG_MED("Set Mode to CDMA 1900",0,0,0);
    break;

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */
  
  case FTM_PHONE_MODE_CDMA_450:
    ftm_set_cdma_band( device, RF_BC5_BAND );
    #if defined(T_MSM7800) || defined(T_QSC1100)
    sample_server_set_chain_mode((sample_server_rx_chain_type)rfcom_to_path[device],
                                 SAMPLE_SERVER_CARRIER_0, RTC_1X, CONTINUOUS_SAMPLE_MODE);
    #else 
    sample_server_set_chain_mode((sample_server_rx_chain_type)rfcom_to_path[device],
                                 RTC_1X, CONTINUOUS_SAMPLE_MODE);
    #endif
    FTM_MSG_MED("Set Mode to CDMA 450",0,0,0);
  break;

#ifdef FTM_HAS_SUPPORT_FOR_BC10
  case FTM_PHONE_MODE_CDMA_BC10:
    ftm_set_cdma_band( device, RF_BC10_BAND );
    #ifdef T_MSM7800
#error code not present
    #else 
    sample_server_set_chain_mode((sample_server_rx_chain_type)rfcom_to_path[device],
                                 RTC_1X, CONTINUOUS_SAMPLE_MODE);
    #endif
    FTM_MSG_MED("Set Mode to Band Class 10.",0,0,0);
    break;
#endif /*FTM_HAS_SUPPORT_FOR_BC10*/

  case FTM_PHONE_MODE_CDMA_BC14:
    ftm_set_cdma_band( device, RF_BC14_BAND );
    #if defined(T_MSM7800) || defined(T_QSC1100)
    sample_server_set_chain_mode((sample_server_rx_chain_type)rfcom_to_path[device],
                                 SAMPLE_SERVER_CARRIER_0, RTC_1X, CONTINUOUS_SAMPLE_MODE);
    #else 
    sample_server_set_chain_mode((sample_server_rx_chain_type)rfcom_to_path[device],
                                 RTC_1X, CONTINUOUS_SAMPLE_MODE);
    #endif
    FTM_MSG_MED("Set Mode to Band Class 14 (US PCS + 5 MHz).",0,0,0);
    break;
 
  case FTM_PHONE_MODE_CDMA_BC11:
    ftm_set_cdma_band( device, RF_BC11_BAND );
    #if defined(T_MSM7800) || defined(T_QSC1100)
    sample_server_set_chain_mode((sample_server_rx_chain_type)rfcom_to_path[device],
                                 SAMPLE_SERVER_CARRIER_0, RTC_1X, CONTINUOUS_SAMPLE_MODE);
    #else 
    sample_server_set_chain_mode((sample_server_rx_chain_type)rfcom_to_path[device],
                                 RTC_1X, CONTINUOUS_SAMPLE_MODE);
    #endif
    FTM_MSG_MED("Set Mode to Band Class 11.",0,0,0);
    break;

#ifdef FTM_HAS_SUPPORT_FOR_BC15
  case FTM_PHONE_MODE_CDMA_BC15:
    ftm_set_cdma_band( device, RF_BC15_BAND );
    #ifdef T_MSM7800
#error code not present
    #else 
    sample_server_set_chain_mode((sample_server_rx_chain_type)rfcom_to_path[device],
                                 RTC_1X, CONTINUOUS_SAMPLE_MODE);
    #endif
    FTM_MSG_MED("Set Mode to Band Class 15.",0,0,0);
    break;
#endif /*FTM_HAS_SUPPORT_FOR_BC15*/
  
  default:
    FTM_MSG_ERROR("Unsupported mode switch: %d", mode, 0, 0);
    break;
  }     /* end switch(mode) */

} /*End ftm_set_rf_mode */

/*===========================================================================

FUNCTION FTM_CDMA_PILOT_WAVEFORM

DESCRIPTION
   This function will program the encoder to generate a CDMA waveform.
   It is used only for RF testing purposes.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_cdma_pilot_waveform(boolean on_off)
{
  qword high_bit;
#ifdef T_MSM6700
  dword int_stat = 0;
#endif


  if (on_off == ON)
  {

    MSG_LOW("TX Pilot ON",0,0,0);
    /* Tell the encoder we are going into digital mode.  This does a write
    ** to all of the interleavers write registers.  The registers needed to
    ** unstick the ASIC are written during initialization.  The rest of
    ** the values are written now.  This also disable the Tx clocks and
    ** the codec clk and codec sync.  Turns PA OFF and calls enc_tx_enable
    ** with a FALSE value to Mask the transmit path.
    */
    enc_set_mode( ENC_CDMA_MODE );


    /* Enable Transmit clocks */
    #ifdef FTM_HAS_TX_DACC_CONTROL
    enc_tx_clk_enable(TRUE);
    #endif 

    /* Now Initialize the I and Q PN state to enable spreading */

    enc_iq_pn_state(0x4000,0x4000);

    qw_set( high_bit, 0x200L, 0L );    /* Init quad word, set bit 41 [0..41] */
    enc_user_pn_state( high_bit );     /* Load User PN state */
    enc_user_pn_mask ( high_bit );     /* Load User PN mask  */


    HWIO_OUT(MOD_STMR_CMD, 0x05);
    HWIO_OUT(MOD_STMR_CMD, 0x25);


    /* Set the encoder rate to 9600 */
    enc_rate_set(ENC_RATE_9600);


    /* Enable PA_ON in the WEB_MISC register. Also set MASK data
    ** to be the DBR output. And last, set TX_PUNCT to follow MASK data.
    */
    enc_tx_enable(TRUE);

#if defined( T_MSM6700 ) || defined( T_MSM6800 )
    tramp_set_isr(TRAMP_1X_TX_FR_ISR, ftm_enc_stuff_zero_data_isr);
    int_stat = HWIO_IN(MOD_TX_FR_INT_STATUS);
    /* clear the second level 1x MOD bits */
    if (int_stat == MOD_TX_FR_INT_1X_MODE_STATUS  ) {
      HWIO_OUTM(MOD_TX_FR_INT_CLR,MOD_TX_FR_INT_1X_MODE_MASK,MOD_TX_FR_INT_1X_MODE_MASK);
    }
    HWIO_OUTM(MOD_TX_FR_INT_EN, HWIO_RMSK(MOD_TX_FR_INT_EN), MOD_TX_FR_INT_1X_MODE_EN);

    /* There were no registrations for this pcg before.
    * Enable it in hardware. */
    HWIO_OUTM(TX_PCG_INT_ENABLE, HWIO_RMSK(TX_PCG_INT_ENABLE), TX_PCG_INT_MASK);

    /* clear the PCG INT status bits */
    int_stat = HWIO_IN(TX_PCG_INT_STATUS);
    HWIO_OUTM(TX_PCG_INT_CLEAR, int_stat, int_stat);
    //HWIO_OUTM(TX_PCG_INT_CLEAR, 0xFFFF, 0xFFFF);

#else
    /* Enable the stuff zero data isr */
    tramp_set_isr(TRAMP_TX_ISR, ftm_enc_stuff_zero_data_isr);

#endif

  }
  else if (on_off == OFF)
  {
    if(tx_sweep_cal_enable)
    {
       tx_sweep_cal_enable = OFF;
#ifdef RF_HAS_INTERNAL_HDET_SUPPORT
       if ( rfc_uses_internal_hdet(rf_chain_status[RF_PATH_0].rf_curr_cdma_band) )
       {
          rft_internal_hdet_disable( RF_PATH_0 );
       }
#endif /* RF_HAS_INTERNAL_HDET_SUPPORT */

       #if defined (FTM_HAS_CFG_TX_SWP_CAL)
       timer_clr(&ftmclk.timer,T_NONE);
       #endif
    }
    MSG_LOW("TX Pilot Off",0,0,0);
    /* Shutdown encoder tx frame interrupts */
#if defined( T_MSM6700 ) || defined( T_MSM6800 )
    tramp_set_isr(TRAMP_1X_TX_FR_ISR, NULL);
#else
    tramp_set_isr(TRAMP_TX_ISR, NULL);
#endif 

    /* Disable Transmit clocks */
    #ifdef FTM_HAS_TX_DACC_CONTROL
    enc_tx_clk_enable(FALSE);
    #endif 

    /* Now Initialize the I and Q PN state to disable spreading */
    enc_iq_pn_state(0,0);

    qw_set( high_bit, 0x000L, 0L );        /* Init quad word, set bit 41 [0..41] */
    enc_user_pn_state( high_bit );     /* Load User PN state */
    enc_user_pn_mask ( high_bit );     /* Load User PN mask  */

    enc_tx_enable(FALSE);


  } /* else */
} /* End ftm_cmda_pilot_waveform */

/*===========================================================================

FUNCTION FTM_ENC_STUFF_ZERO_DATA_ISR

DESCRIPTION
   Encoder isr.  Currently stuff zero data.
   This function is also used to perform the TX Sweep Calibration and is enabled
   by setting the global variable tx_sweep_cal_enable via the FTM RF Evaluation GUI.
   The tx sweep calibraion assumes that the user has set a high power level via the
   Tx PDM. The following describes the passes where passes means each time this function
   is called.
   1st pass --- set tx agc to pdm that will set power to lowest desired level through
                pre-defined tx agc level(TX_SWEEP_CAL_MAX)

   subsequent passes --- the tx agc level starts at the original tx agc level then
                         power level is decrimented each subsequent pass until the
                         power level set by tx agc = TX_SWEEP_CAL_MAX is reached.

   end of tx cal sweep -- the original tx agc level is set ... cal sweep has concluded


                               | 1st   |     | subsequent| end of tx cal sweep -->
                               | pass  |     |  passes   |
                               |       |

   Tx Pwr level ----------------       -------          |----------------
                               |       |     |          |
                               |       |     --         |
                               |       |       |        |
                               |       |       --       |
                               |       |         |      |
                               |       |         --     |
                               |       |           |    |
                               |       |           --   |
                               |       |             |  |
                               |-----  |             ---

   The 1st pass can be used as a trigger in automated test if a spectrum analyzer is monitoring tx output. Put
   the analyzer in zero span with appropriate RBW and VBW.
   The tx sweep calibraion can be used to calibrate the Tx Linearizer table as well
   as associating HDET values with high power levels. That calibration process is
   outside of the scope of this description.

DEPENDENCIES
   None.

RETURN VALUE
   Note... HDET values stored by each pass are stored in an array which is queried by the GUI in the
           HDET controls area.

SIDE EFFECTS
   None.

===========================================================================*/
/* This will get called every 20ms when a tx frame interrupt occurs.
** We will basically stuff zero's to the encoder tx data register.
*/
void ftm_enc_stuff_zero_data_isr(void)
{
  enc_rate_type txp_rate = ENC_FULL_RATE;
  /* Size of a FULL rate data packet */
  byte data_ptr[ENC_9600_FULL_RATE_SIZE];
#ifdef FTM_LEGACY_TX_SWEEP_CAL
  byte i=0;  //loop counter for hdet averaging
  word local_HDET=0;
  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ rfcom_to_path[ RFCOM_TRANSCEIVER_0 ] ];
  adc_logical_channel_type hdet_adc = NUMBER_OF_LOGICAL_ADC_CHANNELS;
  static led_state = 0;
#endif 

#if defined( T_MSM6700 ) || defined( T_MSM6800 )
  dword int_stat = 0;
  /* make sure we only service the PCG int we defined */
  int_stat = HWIO_IN(TX_PCG_INT_STATUS);
  if ((int_stat & TX_PCG_INT_MASK) == 0) 
  {
    /* clear the second level 1x MOD bits */
    int_stat = HWIO_IN(MOD_TX_FR_INT_STATUS);
    HWIO_OUTM(MOD_TX_FR_INT_CLR, int_stat, int_stat);

    /* clear the PCG INT status bits */
    int_stat = HWIO_IN(TX_PCG_INT_STATUS);
    HWIO_OUTM(TX_PCG_INT_CLEAR, int_stat, int_stat);

    return;
  }
#endif

  /* Clear the data to zeros */
  memset((byte*)data_ptr,0,ENC_9600_FULL_RATE_SIZE);

  /* Now ship the data */
 #ifdef FTM_SC2X_HAS_HWTC_TX_TONE_ENABLED
 if(ftm_tx_tone_gen_enabled == FALSE)
 {  
   enc_fch_data_out(data_ptr, FALSE, (enc_rate_type) txp_rate);
 }
 #else
  enc_fch_data_out(data_ptr, FALSE, (enc_rate_type) txp_rate);
 #endif 

#ifdef FTM_LEGACY_TX_SWEEP_CAL
  if (tx_sweep_cal_enable)
  {
    ftm_leds(0x4, led_state, 2);

    hdet_adc = rfc_get_hdet_adc_channel( rf_chain_ptr->rf_curr_cdma_band );

    for (i=0; i<5; i++)

    {
   /* Lock IRQs to avoid the rf 1.25 msec interrupt the AD reading. 
      Because this code only runs in 1X mode, we expect the AD reading to 
      be immediate. */
      INTLOCK();

      local_HDET += rf_read_hdet( rf_chain_ptr->rf_curr_cdma_band ); 
    
      INTFREE();
    }
    
    ftm_leds(0x2, led_state++, 1);

    if ( g_course_loop == 0 )
    {
      tx_sweep_cal_adj = TX_SWEEP_CAL_MAX;
      ftm_cdma_set_tx_agc(tx_sweep_cal_adj);

      tx_sweep_cal_adj = g_tx_agc_adj;
      g_course_loop++;
    }
    else if ((tx_sweep_cal_adj <= TX_SWEEP_CAL_MAX) && (g_loopcount < HDET_ARRAY_SIZE))
    {
      if(g_loopcount>0)
      {
        g_hdet[g_loopcount-1] = local_HDET/5;
      }

      ftm_cdma_set_tx_agc(tx_sweep_cal_adj);
      tx_sweep_cal_adj = tx_sweep_cal_adj + ftm_uplink_sweep_cal_step;
      g_loopcount++;
    }
    else
    {
      ftm_tx_sweep_cal(OFF);
      ftm_cdma_set_tx_agc(g_tx_agc_adj);
    }
  }
#endif // ifdef FTM_LEGACY_TX_SWEEP_CAL
 
#if defined( T_MSM6700 ) || defined( T_MSM6800 )
  /* clear the second level 1x MOD bits */
  int_stat = HWIO_IN(MOD_TX_FR_INT_STATUS);
  HWIO_OUTM(MOD_TX_FR_INT_CLR, int_stat, int_stat);

  /* clear the PCG INT status bits */
  int_stat = HWIO_IN(TX_PCG_INT_STATUS);
  HWIO_OUTM(TX_PCG_INT_CLEAR, int_stat, int_stat);
#endif
}

/*===========================================================================

FUNCTION FTM_SET_CDMA_CW

DESCRIPTION
   This function will force the interleaver to generate a Continuous Wave.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ftm_set_cdma_cw(boolean on_off) 
{
   
  #ifdef FTM_HAS_SUPORT_FOR_VARIABLE_FREQ_OFFSET_TONE
  mdsp_poke_reg(FTM_MDSP_TX_CORDIC_ROTATOR_REG_ADDRESS,0);
  #endif

  if (on_off == ON)
  {
    HWIO_OUTM( MOD_TEST_CTL, HWIO_FMSK(MOD_TEST_CTL,SET_IQ_HIGH), ENC_SET_IQ_V );
  }
  else if ( on_off == OFF)
  {
    HWIO_OUTM( MOD_TEST_CTL, HWIO_FMSK(MOD_TEST_CTL,SET_IQ_HIGH), ENC_NO_SET_IQ_V );
  }

} /* End ftm_set_cdma_cw */

/*===========================================================================

FUNCTION FTM_SET_TX_WAVEFORM

DESCRIPTION
   This function will force the CDMA, CW or Offset CW depending on parameters passed

   For CDMA Waveform set ftm_tx_waveform parameter to CDMA_WAVEFORM

   For CW Waveform with zero offset set  ftm_tx_waveform parameter to CW_WAVEFORM 
   AND ftm_tx_attrib parameter to value of 0

   For offset CW at +19.2 KHz offset ftm_tx_waveform parameter to CW_WAVEFORM 
   AND ftm_tx_attrib parameter to value of FTM_MDSP_TX_ROTATOR_REG_VAL_FOR_19_2_KHZ_OFFSET_TONE

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ftm_set_tx_waveform(ftm_tx_waveform_type ftm_tx_waveform ,ftm_tx_waveform_attrib_type ftm_tx_attrib) 
{
#ifdef FTM_HAS_SUPORT_FOR_VARIABLE_FREQ_OFFSET_TONE
  uint32 mdsp_reg_val;
#endif

  FTM_MSG_HIGH("In FTM Command for ftm_set_tx_waveform and params are waveform: %d and attribute: %d:",ftm_tx_waveform,ftm_tx_attrib,0);

  if(ftm_tx_waveform == CDMA_WAVEFORM)
  {
    #ifdef FTM_HAS_SUPORT_FOR_VARIABLE_FREQ_OFFSET_TONE
    mdsp_poke_reg(FTM_MDSP_TX_CORDIC_ROTATOR_REG_ADDRESS,0);
    #endif
    HWIO_OUTM( MOD_TEST_CTL, HWIO_FMSK(MOD_TEST_CTL,SET_IQ_HIGH), ENC_NO_SET_IQ_V );
  }
  else if ((ftm_tx_waveform == CW_WAVEFORM) && (ftm_tx_attrib == CW_OFFSET_ZER0_KHZ))
  {
    #ifdef FTM_HAS_SUPORT_FOR_VARIABLE_FREQ_OFFSET_TONE
    mdsp_poke_reg(FTM_MDSP_TX_CORDIC_ROTATOR_REG_ADDRESS,0);
    #endif
    HWIO_OUTM( MOD_TEST_CTL, HWIO_FMSK(MOD_TEST_CTL,SET_IQ_HIGH), ENC_SET_IQ_V );      
   }
   else 
   {
      #ifdef FTM_HAS_SUPORT_FOR_VARIABLE_FREQ_OFFSET_TONE
      mdsp_poke_reg(FTM_MDSP_TX_CORDIC_ROTATOR_REG_ADDRESS,FTM_MDSP_TX_ROTATOR_REG_VAL_FOR_19_2_KHZ_OFFSET_TONE);         
      #endif

      HWIO_OUTM( MOD_TEST_CTL, HWIO_FMSK(MOD_TEST_CTL,SET_IQ_HIGH), ENC_SET_IQ_V );

      #ifdef FTM_HAS_SUPORT_FOR_VARIABLE_FREQ_OFFSET_TONE
      mdsp_peek_reg(FTM_MDSP_TX_CORDIC_ROTATOR_REG_ADDRESS, &mdsp_reg_val);
      FTM_MSG_HIGH("FTM MDSP Tx Rotator Register read address %d and value = %d",FTM_MDSP_TX_CORDIC_ROTATOR_REG_ADDRESS,mdsp_reg_val,0);
      #endif
    }
} /* End ftm_set_tx_waveform*/

/*===========================================================================

FUNCTION FTM_SET_TX

DESCRIPTION
   This function will set the TX state for FM or CDMA.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_set_tx(rfcom_device_enum_type device, boolean state)
{
#if defined( T_MSM6700 ) || defined( T_MSM6800 )
  muxmdsp_wr_passThruEnable(TRUE); // Enable DSP pass through for the encoder to be in charge
#endif /* defined ( T_MSM6700 ) || defined ( T_MSM6800 )*/

  if ( state == ON )
  {
#ifdef FEATURE_GSM
#error code not present
#endif

    #ifdef RF_HAS_MDSP_TX_AGC    
    CDMA_MDSP_WR_TX_CLOSED_LOOP_VAL(0);
    CDMA_MDSP_ENABLE_TX_CLOSED_LOOP_OVERRIDE();
    #else
    /* overide TX_GAIN_ADJ accumulator */
    MSM_W_MASKED(TX_AGC_CTL2, PWR_CTL_EN, 0);
    /* Set output power to a medium level */
    HWIO_OUT(TX_GAIN_ADJ_WR, 0);
    #endif /* RF_HAS_MDSP_TX_AGC */

    enc_fch_enable( TRUE ); /* in IS95-A mode, the IS95-C FCH enable bit must be set */

  #ifdef T_MSM6700
    #ifdef RF_HAS_MDSP_TX_AGC
    /* PA_RANGE_SEL is controlled by FW */
    /* TX_PUNCT and PA_PUNCT is controlled by HW */
    HWIO_OUT( MOD_PA_ON_CTL, 0x04 );
    #else
    HWIO_OUT( MOD_PA_ON_CTL, 0x00 );
    #endif
  #endif
#ifdef T_MSM7800
#error code not present
#else
    MSM_W_MASKED( PA_ON_CTL, TX_ON_EN, 1);
#endif

    #ifdef RF_HAS_MDSP_TX_AGC
    MSM_W_MASKED(DEM1X_COMBINER_CTL,ENABLE,1);
    #endif
    
    rf_enable_tx_pwr(device);
    ftm_clk_rex_wait( RF_ENABLE_PWR_WAIT );

    rf_tx_enable(device);
    
    #ifdef RF_HAS_INTERNAL_HDET_SUPPORT
    if ( rfc_uses_internal_hdet(rf_chain_status[RF_PATH_0].rf_curr_cdma_band) )
    {
       rft_internal_hdet_enable( RF_PATH_0 );
    }
    #endif

    ftm_clk_rex_wait( RF_TUNE_CHAN_WAIT );

    enc_tx_enable( TRUE );           /* Mask transmit path */

    /* Insure the TX PDM is enabled and the correct polarity */
  #ifdef T_MSM6700
    MSM_W_MASKED(MOD_TX_AGC_ADJ_CTL, TX_AGC_ADJ_OE, 1);
    MSM_W_MASKED(MOD_TX_AGC_ADJ_CTL, TX_AGC_ADJ_POLARITY, 1);
  #else
    MSM_W_MASKED(TX_AGC_CTL, TX_AGC_ADJ_OE, 1);
    MSM_W_MASKED(TX_AGC_CTL, TX_AGC_ADJ_POLARITY, 1);
  #endif
    enc_acp_pa_ctl(ON);
    #ifdef FTM_SC2X_HAS_HWTC_TX_TONE_ENABLED
    if(ftm_tx_tone_gen_enabled == FALSE)
    { 
    /* turn ON CDMA right here.*/
    ftm_cdma_pilot_waveform(ON);
    }
    #else
    ftm_cdma_pilot_waveform(ON);
    #endif

    FTM_MSG_MED("Setting TX ON",0,0,0);
  }
  else if ( state == OFF )
  {
    #ifdef FTM_SC2X_HAS_HWTC_TX_TONE_ENABLED 
    ftm_tx_tone_gen_enabled = FALSE;
    #endif

    rf_tx_disable(device);

    #ifdef RF_HAS_MDSP_TX_AGC
    MSM_W_MASKED(DEM1X_COMBINER_CTL,ENABLE,0);
    #endif
    enc_acp_pa_ctl(OFF);
    /* Turn off the CDMA waveform. */
    ftm_cdma_pilot_waveform(OFF);

    FTM_MSG_MED("Setting TX OFF",0,0,0);
  } /* else if */
} /* End ftm_set_tx */

#ifdef FTM_HAS_CFG_TX_SWP_CAL
/*===========================================================================

FUNCTION FTM_TX_SWEEP_CAL_ISR

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ftm_tx_sweep_cal_isr(void)
{
  
  static int led_state = 0;
  uint32 local_HDET=0;
  byte i = 0;  //loop counter for hdet averageing
  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ rfcom_to_path[ RFCOM_TRANSCEIVER_0 ] ];

  ftm_leds(0x4, led_state, 2);

  /* Create a PULSE for the test instrument via the TX power level
     This is only done once at the beginning of TX Sweep Cal procedure
   */
  if ( tx_sweep_pulse_flag == TRUE )
  {
    ftm_cdma_set_tx_agc(TX_SWEEP_CAL_MAX);      
    tx_sweep_pulse_flag = FALSE;
    FTM_MSG_HIGH("TX Sweep First visit! TX_AGC: %d",TX_SWEEP_CAL_MAX,0,0);

  }
  /* Now that the PULSE has been created, we can start capturing HDET readings */
  else if ((tx_sweep_cal_adj <= TX_SWEEP_CAL_MAX) && (g_loopcount < HDET_ARRAY_SIZE))
  {
    /* First time through here we don't read HDET and the power level needs to return to the initial pdm level set by the user.*/
    if (g_loopcount > 0) 
    {
      /*Read and store HDET at the trailing edge of the step */
      for (i=0; i < ftm_num_of_avg_hdet_read; i++)
      {
        local_HDET += rf_read_hdet( rf_chain_ptr->rf_curr_cdma_band ); 
      }  
      g_hdet[g_loopcount-1] = local_HDET/ftm_num_of_avg_hdet_read;
      tx_sweep_cal_adj = tx_sweep_cal_adj + ftm_uplink_sweep_cal_step;
    }
    else
    {
      for (i=0; i < ftm_num_of_avg_hdet_read; i++)
      {
        rf_read_hdet( rf_chain_ptr->rf_curr_cdma_band ); // Dummy ADC read to 'initialize' the ADC driver
      }
    }
    /*Set power level for new step */
    ftm_cdma_set_tx_agc(tx_sweep_cal_adj);
    g_loopcount++;
  }
  else
  {
    ftm_tx_sweep_cal(OFF);
    ftm_cdma_set_tx_agc(g_tx_agc_adj);
    timer_clr(&ftmclk.timer,T_NONE);
    FTM_MSG_LOW("TX Sweep Done! TX_AGC: %d",g_tx_agc_adj,0,0);
  }    

}
#endif /* FTM_HAS_CFG_TX_SWP_CAL */
/*===========================================================================

FUNCTION FTM_TX_SWEEP_CAL

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  word = 9 bit Tx AGC value

SIDE EFFECTS
  None

===========================================================================*/
void ftm_tx_sweep_cal (boolean enable_disable_cmd )
{
  tx_sweep_cal_enable = enable_disable_cmd;

  if (tx_sweep_cal_enable)
  {
#ifdef RF_HAS_INTERNAL_HDET_SUPPORT
    /* Since several HDET readings will be taken, enable internal HDETs
       just once, take the readings, and then turn it off later
    */
    if ( rfc_uses_internal_hdet(rf_chain_status[RF_PATH_0].rf_curr_cdma_band) )
    {
       rft_internal_hdet_enable( RF_PATH_0 );
    }
#endif /* RF_HAS_INTERNAL_HDET_SUPPORT */
    tx_sweep_cal_adj = g_tx_agc_adj;
    g_loopcount = 0;
#ifdef FTM_LEGACY_TX_SWEEP_CAL
    g_course_loop= 0;
#elif defined (FTM_HAS_CFG_TX_SWP_CAL)
    tx_sweep_pulse_flag = TRUE;
    timer_def(&ftmclk.timer, NULL, NULL, 0, (timer_t1_cb_type)ftm_tx_sweep_cal_isr, 0);
    timer_set(&ftmclk.timer, ftm_tx_cal_sweep_interval, ftm_tx_cal_sweep_interval, T_MSEC);
#endif
    FTM_MSG_MED("ftm_tx_sweep_cal ISR ENABLED: interval: %d",ftm_tx_cal_sweep_interval,0,0);
  }
  else
  {
#ifdef RF_HAS_INTERNAL_HDET_SUPPORT
    if ( rfc_uses_internal_hdet(rf_chain_status[RF_PATH_0].rf_curr_cdma_band) )
    {
      #ifndef RF_HAS_HW_QSC60X5
      rft_internal_hdet_disable( RF_PATH_0 );
      #endif
    }
#endif /* RF_HAS_INTERNAL_HDET_SUPPORT */
    timer_clr(&ftmclk.timer,T_NONE);
  }
} /* End ftm_tx_sweep_cal() */

/*===========================================================================

FUNCTION FTM_SET_DIGITAL_BAND

DESCRIPTION
   This function will set the CDMA band.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_set_digital_band(rfcom_device_enum_type device, rf_card_band_type band)
{
  rflib_min_max_chan_type chan_min_max;
  
  if (rf_path_state  == RF_DIVERSITY_ST )
  {
    /* Override the device that the user passed in since in diversity,
       both chains operate in lockstep
     */
    device = RFCOM_RECEIVER_DIV;
  }

  // Do not use the rf_chain_ptr cached channel, because it may be invalid for
  // the new band class. Instead use get_min_max_chan func which is to provide a
  // valid channel for use on the new band.
  rflib_get_band_min_max_chans( rfcom_to_path[device], band, &chan_min_max );
  rfm_tune_band_chan( device, band, chan_min_max.min_chan );     
 } /* End ftm_set_digital_band */

#ifdef RF_HAS_SUB_BLOCK_SUPPORT
/*===========================================================================

FUNCTION FTM_SET_BLOCK_MASK_OVERRIDE

DESCRIPTION
   This function forces the Block Mask for a given tuned Band.
   This function calls the RF Function to check for the Validity
   of Block Mask to be set

DEPENDENCIES
   None.

RETURN VALUE
   Returns TRUE if Block Mask is Valid
   Returns FALSE if Block MAsk is not valid

SIDE EFFECTS
   None.

===========================================================================*/
boolean ftm_set_block_mask_override(rfcom_device_enum_type device,int8 index,uint32 block_mask)
{    
  if(rf_set_block_mask_override(device,index,block_mask))
  {
    return TRUE;
  }
  else
  {
    return FALSE; /* Block Mask set in FTM Mode is not Valid*/
  }  
}
#endif/*RF_HAS_SUB_BLOCK_SUPPORT*/
/*===========================================================================

FUNCTION FTM_GET_RF_CDMA_RX_AGC                                     EXTERNAL

DESCRIPTION
  This function returns the CDMA Rx AGC setting in 10 bit two's complement
  format.

DEPENDENCIES
  None

RETURN VALUE
  int2 = 10 bit two's complement CDMA Rx AGC value

SIDE EFFECTS
  None

===========================================================================*/
int2 ftm_get_rf_cdma_rx_agc( rfcom_device_enum_type device )
{
  int2 agc_value = 4096;
  #ifdef FTM_HAS_CGPS
#error code not present
  #endif

#ifdef FTM_HAS_CGPS
#error code not present
#endif

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */
  {
#ifdef T_MSM7800
#error code not present
#else
    agc_value = HWIO_INMI(RX_AGCc_1X_VALUE_RD, rfcom_to_path[device], HWIO_RMSK(RX_AGCc_1X_VALUE_RD));
#endif
  }

  return ( agc_value );
} /* End ftm_get_rf_cdma_rx_agc */

/*===========================================================================

FUNCTION SetDvgaGainOffset                                     INTERNAL

DESCRIPTION
  This function sets the CDMA Rx VAGC offset  setting in 13 bit two's complement
  format.

DEPENDENCIES
  None

RETURN VALUE
  int2 = 10 bit two's complement CDMA Rx AGC value

SIDE EFFECTS
  None

===========================================================================*/
void ftm_Set_Dvga_Gain_Offset(rfcom_device_enum_type device, int2 dvgaGainOffset)
{
  dword shiftedDvgaGainOffset;
  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ rfcom_to_path[ device ] ];

#ifndef RF_HAS_DVGA_1OTH_OF_DB_RESOLUTION
#error code not present
#endif

  wVgaGainOffset = dvgaGainOffset;

  FTM_MSG_LOW("wVgaGainOffset  = %d", wVgaGainOffset,0,0);
  
  #ifdef RF_HAS_DVGA_1OTH_OF_DB_RESOLUTION
  //DVGA Offset is 10-bit 2's Compliment field  
  shiftedDvgaGainOffset = (dword) (dvgaGainOffset & 0x3FF);

  FTM_MSG_LOW("shiftedDvgaGainOffset  = %d", shiftedDvgaGainOffset,0,0);
  #else
#error code not present
  #endif
  switch (rf_chain_ptr->rf_mode)
  {

  case DB_RF_CDMA:
  case DB_RF_PCN:
     // Select CDMA_VGA_GAIN as the input to the DVGA MUX2
     // Set the vga_gain_offset
     // Set the vga_gain_scale for CDMA (0x5)

#ifndef RF_HAS_DVGA_1OTH_OF_DB_RESOLUTION
#error code not present
#else
     #if defined(T_QSC60X5)
     MSM_W_MASKED_i(RXF_CDMA(RXFc_DVGA_PARAMS), ADD_EN, 0, (int)rfcom_to_path[device]);
     MSM_W_MASKED_i(RXF_CDMA(RXFc_DVGA_PARAMS), VGA_MODE, RXF_RF_VGA_MODE_CDMA_V , (int)rfcom_to_path[device]);
     MSM_W_MASKED_i(RXF_CDMA(RXFc_DVGA_PARAMS), VGA_GAIN_OFFSET, shiftedDvgaGainOffset, (int)rfcom_to_path[device]);
     #elif defined(T_MSM7800)
#error code not present
     #else
#error code not present
     #endif
#endif
     break;
#ifdef RF_HAS_GPS
#error code not present
#endif /* RF+HAS_GPS */
  default:
    ERR("UNKNOW RF MODE",0,0,0);
    break;
  }
} /* End ftm_Set_Dvga_Gain_Offset */

/*===========================================================================

FUNCTION FTM_GET_DVGA_OFFSET                                     EXTERNAL

DESCRIPTION
  This function returns the 13 bit two's complement DVGA Offset Value.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_FTM_BYPASS_LNA_NV_CAL_POINTS
int2 ftm_get_dvga_offset( rfcom_device_enum_type device, int2 agc_value )
#else
int2 ftm_get_dvga_offset( rfcom_device_enum_type device )
#endif
{
  rf_chain_state_type *rf_chain_ptr;
  int2 dvga_offset=0;
  int2 expected_agc_val=0;
  int2 current_agc_val=0;
  int2 agc_read_val=0;
  int loopcount;
  int avg_count = DVGA_OFFSET_AVG;

  #ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO
  byte digital_filter_gain = 0;
  rf_path_enum_type path = rfcom_to_path[ device ];
  #endif
  
   /* point to chain status structure for the device */
  rf_chain_ptr = &rf_chain_status[ rfcom_to_path[ device ] ];
    
  ftm_Set_Dvga_Gain_Offset(device, 0);

#ifdef FEATURE_FTM_BYPASS_LNA_NV_CAL_POINTS
  expected_agc_val = agc_value;
#else
  expected_agc_val = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_fall;
#endif
  FTM_MSG_MED("Get DVGA Offset calibration point: %d", agc_value, 0, 0);
  
  switch (ftm_get_rfr_device_type(device))
  {
  case RFR6125:
  case RFR6175:
  case RFR6500_DRX:
  case RFR6525_DRX:
    rf_set_cdma_lna_decision(device, LNA_GAIN_STATE_0);
    break;
  case RFR6500:
  case RFR6525:
  case RFR61X5:
    switch ( rf_chain_ptr->rf_curr_cdma_band )
    {
    case RF_BC0_BAND:
    case RF_BC3_BAND:
#ifdef FTM_HAS_SUPPORT_FOR_BC10
    case RF_BC10_BAND:
#endif /*FTM_HAS_SUPPORT_FOR_BC10*/
      rf_set_cdma_lna_decision(device, LNA_GAIN_STATE_0);
      break;
    case RF_BC1_BAND:
    case RF_BC4_BAND:
    case RF_BC6_BAND:
    case RF_BC14_BAND:
#ifdef FTM_HAS_SUPPORT_FOR_BC15
    case RF_BC15_BAND:
#endif /*FTM_HAS_SUPPORT_FOR_BC15*/
      rf_set_cdma_lna_decision(device, LNA_GAIN_STATE_1);
      ftm_set_lna_offset(device,0,0);
      break;
    }
    break;
  case QSC60X5_RX:
    switch ( rf_chain_ptr->rf_curr_cdma_band )
    {
    case RF_BC0_BAND:
    case RF_BC3_BAND:
#ifdef FTM_HAS_SUPPORT_FOR_BC10
    case RF_BC10_BAND:
#endif /*FTM_HAS_SUPPORT_FOR_BC10*/
      rf_set_cdma_lna_decision(device, LNA_GAIN_STATE_0);
      break;
    case RF_BC1_BAND:
    case RF_BC4_BAND:
    case RF_BC6_BAND:
    case RF_BC14_BAND:
#ifdef FTM_HAS_SUPPORT_FOR_BC15
    case RF_BC15_BAND:
#endif /*FTM_HAS_SUPPORT_FOR_BC15*/
      if (device == RFCOM_RECEIVER_1)
      {
         rf_set_cdma_lna_decision(device, LNA_GAIN_STATE_0);
      }
      else
      {
         rf_set_cdma_lna_decision(device, LNA_GAIN_STATE_1);
         ftm_set_lna_offset(device,0,0);
      }
      break;
    }
    break;
  case RTR6500_RX:
     switch ( rfcom_to_path[ device ] )
     {
     case RF_PATH_0:
        switch ( rf_chain_ptr->rf_curr_cdma_band )
        {
        case RF_BC0_BAND:
        case RF_BC3_BAND:
#ifdef FTM_HAS_SUPPORT_FOR_BC10
        case RF_BC10_BAND:
#endif /*FTM_HAS_SUPPORT_FOR_BC10*/
          rf_set_cdma_lna_decision(device, LNA_GAIN_STATE_0);
          break;
        case RF_BC1_BAND:
        case RF_BC4_BAND:
        case RF_BC6_BAND:
        case RF_BC14_BAND:
#ifdef FTM_HAS_SUPPORT_FOR_BC15
        case RF_BC15_BAND:
#endif /*FTM_HAS_SUPPORT_FOR_BC15*/
          rf_set_cdma_lna_decision(device, LNA_GAIN_STATE_1);
          ftm_set_lna_offset(device,0,0);
          break;
        default:
          FTM_MSG_ERROR("Get DVGA Offset: Unsupported band for RTR6500: %d", rf_chain_ptr->rf_curr_cdma_band, 0, 0);
          return -1;
        } /* switch ( rf_chain_ptr->rf_curr_cdma_band ) */
        break;
     case RF_PATH_1:
        rf_set_cdma_lna_decision(device, LNA_GAIN_STATE_0);
        break;
     } /* switch ( rfcom_to_path[ device ] ) */
     break;

  case QSC11X0_RX:
    switch ( rfcom_to_path[ device ] )
    {
    case RF_PATH_0:
      switch ( rf_chain_ptr->rf_curr_cdma_band )
      {
      case RF_BC0_BAND:
      case RF_BC10_BAND:
      case RF_BC1_BAND:      
      case RF_BC6_BAND:
      case RF_BC5_BAND:
      case RF_BC11_BAND:                    
        rf_set_cdma_lna_decision(device, LNA_GAIN_STATE_0);
        break;
      default:
        FTM_MSG_ERROR("Get DVGA Offset: Unsupported band for QSC1100: %d", rf_chain_ptr->rf_curr_cdma_band, 0, 0);
        return -1;  
      }/*switch ( rf_chain_ptr->rf_curr_cdma_band )*/
      break;

  default:
      FTM_MSG_ERROR("Get DVGA Offset: Unsupported Path for QSC1100: %d",rfcom_to_path[ device ], 0, 0);
      return -1;  
    }/*switch ( rfcom_to_path[ device ] )*/
    break;
  default:
    rf_set_cdma_lna_decision(device, LNA_GAIN_STATE_1);
    ftm_set_lna_offset(device,0,0);
    break;
  }

  ftm_clk_rex_wait(10);

#ifndef FEATURE_FTM_BYPASS_LNA_NV_CAL_POINTS
  if (expected_agc_val > 511)                      // handle 10 bit 2's comp number
    expected_agc_val = expected_agc_val - 1024;

  /* shift from 8 bit 4/10 dB to 1/10 dB */
  expected_agc_val = expected_agc_val * 4;
#endif /* !FEATURE_FTM_BYPASS_LNA_NV_CAL_POINTS */

  avg_count = 12;

  #ifndef RF_HAS_DVGA_1OTH_OF_DB_RESOLUTION 
#error code not present
  #endif

  for (loopcount=1; loopcount <= avg_count; loopcount++)
  {
    if (rf_chain_ptr->rf_mode == DB_RF_CDMA || rf_chain_ptr->rf_mode == DB_RF_PCN)
      agc_read_val =  ftm_get_rf_cdma_rx_agc(device);

    if (agc_read_val > 511)
      agc_read_val = agc_read_val - 1024;    // handle 10 bit 2's comp number

    current_agc_val += agc_read_val;
    ftm_clk_rex_wait(1);
  }

  #ifdef RF_HAS_DVGA_1OTH_OF_DB_RESOLUTION 
  current_agc_val = current_agc_val/avg_count;
  #endif

  dvga_offset =  current_agc_val - expected_agc_val;
  g_dvga_offset = dvga_offset;

  ftm_Set_Dvga_Gain_Offset(device, g_dvga_offset);

  #ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO  
  digital_filter_gain = ftm_get_digital_filter_gain(path);
  dvga_offset = dvga_offset - digital_filter_gain;
  #endif

  return(dvga_offset);

} /* End ftm_get_rf_cdma_rx_agc */

#ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO
/*===========================================================================
FUNCTION FTM_GET_DIGITAL_FILTER_GAIN                                  

DESCRIPTION
  This function returns the Gain of digital filter 

DEPENDENCIES
 Band, Channel and Linearity mode of the receiver must have been 
 configured before for the given path

RETURN VALUE
Byte: Digital filter Gain in 1/10th of dB

SIDE EFFECTS
  None

===========================================================================*/
byte ftm_get_digital_filter_gain(rf_path_enum_type path)
{
  byte dig_filter_gain;
  rflib_power_mode_type power_mode[2] ={RFLIB_NORMAL_POWER_MODE,
                                        RFLIB_NORMAL_POWER_MODE};

  rf_chain_state_type* rf_chain_ptr = &rf_chain_status[path];

  #ifdef RF_HAS_POWER_MODE_LIB
  power_mode[path] = rflib_get_power_mode(path);
  #endif
  dig_filter_gain = rflib_get_digital_filter_gain(rf_adc_clk_status.samp_freq_bin[path],
                                                  path,                                                 
                                                  rf_adc_clk_status.mode[path],
                                                  rf_chain_ptr->rf_curr_cdma_band,
                                                  power_mode[path]);                    
  return dig_filter_gain;
}
#endif
/*===========================================================================

FUNCTION FTM_GET_TX_RX_DVGA_OFFSET                                     EXTERNAL

DESCRIPTION
  This function returns the 13 bit two's complement DVGA Offset Value.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_FTM_BYPASS_LNA_NV_CAL_POINTS
int2 ftm_get_tx_rx_dvga_offset( rfcom_device_enum_type device, int2 agc_value )
#else
int2 ftm_get_tx_rx_dvga_offset( rfcom_device_enum_type device )
#endif
{
  rf_chain_state_type *rf_chain_ptr;
  int2 dvga_offset=0;
  int2 expected_agc_val=0;
  int2 current_agc_val=0;
  int2 agc_read_val=0;
  int loopcount;
  int avg_count = DVGA_OFFSET_AVG;

  #ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO
  byte digital_filter_gain = 0;
  rf_path_enum_type path = rfcom_to_path[ device ];
  #endif
  /* point to chain status structure for the device */
  rf_chain_ptr = &rf_chain_status[ rfcom_to_path[ device ] ];

  if(ftm_current_power_mode > 2) 
  {
    FTM_MSG_ERROR("Get TX_RX_DVGA Offset:Unsupported power mode for any RF Card", 0,0,0);
    return -1;
  }

  #ifdef RF_HAS_POWER_MODE_LIB
  if( rf_intelliceiver_is_enabled(rfcom_to_path[device]) )
  {
    ftm_set_rf_power_mode( device,ftm_current_power_mode);
  }
  else
  #endif
  {
    if(ftm_current_power_mode != 0)
    {
      FTM_MSG_ERROR("Get TX_RX_DVGA Offset:RFR device %d doesn't support the power mode %d as specified",ftm_get_rfr_device_type(device),ftm_current_power_mode,0);
      return -1;
    }
  }

  ftm_Set_Dvga_Gain_Offset(device, 0);  

#ifdef FEATURE_FTM_BYPASS_LNA_NV_CAL_POINTS
  expected_agc_val = agc_value;
#else
  expected_agc_val = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_fall;
#endif
  FTM_MSG_MED("Get DVGA Offset calibration point: %d", agc_value, 0, 0);
  
  switch (ftm_get_rfr_device_type(device))
  {
  case RFR6125:
  case RFR6175:
  case RFR6500_DRX:
  case RFR6525_DRX:
    rf_set_cdma_lna_decision(device, LNA_GAIN_STATE_0);
    break;
  case RFR6500:
  case RFR6525:
    switch ( rf_chain_ptr->rf_curr_cdma_band )
    {
    case RF_BC0_BAND:
    case RF_BC3_BAND:
#ifdef FTM_HAS_SUPPORT_FOR_BC10
    case RF_BC10_BAND:
#endif /*FTM_HAS_SUPPORT_FOR_BC10*/
      rf_set_cdma_lna_decision(device, LNA_GAIN_STATE_0);
      break;
    case RF_BC1_BAND:
    case RF_BC4_BAND:
    case RF_BC6_BAND:
    case RF_BC14_BAND:
#ifdef FTM_HAS_SUPPORT_FOR_BC15
    case RF_BC15_BAND:
#endif /*FTM_HAS_SUPPORT_FOR_BC15*/
      rf_set_cdma_lna_decision(device, LNA_GAIN_STATE_1);
      ftm_set_lna_offset(device,0,0);
      break;

    default:
      FTM_MSG_ERROR("Get DVGA Offset: Unsupported band for RFR6500: %d", rf_chain_ptr->rf_curr_cdma_band, 0, 0);
      return -1;
    }
    break;
  case QSC60X5_RX:
    switch ( rf_chain_ptr->rf_curr_cdma_band )
    {
    case RF_BC0_BAND:
    case RF_BC3_BAND:
#ifdef FTM_HAS_SUPPORT_FOR_BC10
    case RF_BC10_BAND:
#endif /*FTM_HAS_SUPPORT_FOR_BC10*/
      rf_set_cdma_lna_decision(device, LNA_GAIN_STATE_0);
      break;
    case RF_BC1_BAND:
    case RF_BC4_BAND:
    case RF_BC6_BAND:
    case RF_BC14_BAND:
#ifdef FTM_HAS_SUPPORT_FOR_BC15
    case RF_BC15_BAND:
#endif /*FTM_HAS_SUPPORT_FOR_BC15*/
      if (device == RFCOM_RECEIVER_1)
      {
         rf_set_cdma_lna_decision(device, LNA_GAIN_STATE_0);
      }
      else
      {
         rf_set_cdma_lna_decision(device, LNA_GAIN_STATE_1);
         ftm_set_lna_offset(device,0,0);
      }
      break;
    }
    break;
  case RTR6500_RX:
     switch ( rfcom_to_path[ device ] )
     {
     case RF_PATH_0:
        switch ( rf_chain_ptr->rf_curr_cdma_band )
        {
        case RF_BC0_BAND:
        case RF_BC3_BAND:
          rf_set_cdma_lna_decision(device, LNA_GAIN_STATE_0);
          break;
        case RF_BC1_BAND:
        case RF_BC4_BAND:
        case RF_BC6_BAND:
        case RF_BC14_BAND:
          rf_set_cdma_lna_decision(device, LNA_GAIN_STATE_1);
          ftm_set_lna_offset(device,0,0);
          break;
        default:
          FTM_MSG_ERROR("Get DVGA Offset: Unsupported band for RTR6500: %d", rf_chain_ptr->rf_curr_cdma_band, 0, 0);
          return -1;
        } /* switch ( rf_chain_ptr->rf_curr_cdma_band ) */
        break;
     case RF_PATH_1:
        rf_set_cdma_lna_decision(device, LNA_GAIN_STATE_0);
        break;
     } /* switch ( rfcom_to_path[ device ] ) */
     break;

  case QSC11X0_RX:
     switch ( rfcom_to_path[ device ] )
     {
     case RF_PATH_0:
       switch ( rf_chain_ptr->rf_curr_cdma_band )
       {
       case RF_BC0_BAND:
       case RF_BC10_BAND:
       case RF_BC1_BAND:      
       case RF_BC6_BAND:
       case RF_BC5_BAND:
       case RF_BC11_BAND:                    
         rf_set_cdma_lna_decision(device, LNA_GAIN_STATE_0);
         break;
  default:
         FTM_MSG_ERROR("Get DVGA Offset: Unsupported band for QSC1100: %d", rf_chain_ptr->rf_curr_cdma_band, 0, 0);
         return -1;  
       }/*switch ( rf_chain_ptr->rf_curr_cdma_band )*/
       break;

     default:
       FTM_MSG_ERROR("Get DVGA Offset: Unsupported Path for QSC1100: %d",rfcom_to_path[ device ], 0, 0);
       return -1;  
     }/*switch ( rfcom_to_path[ device ] )*/
     break;

  default:
    rf_set_cdma_lna_decision(device, LNA_GAIN_STATE_1);
    ftm_set_lna_offset(device,0,0);
    break;
  }

  clk_busy_wait(5000);

#ifndef FEATURE_FTM_BYPASS_LNA_NV_CAL_POINTS
  if (expected_agc_val > 511)  
  {  
     // handle 10 bit 2's comp number
    expected_agc_val = expected_agc_val - 1024;
  }
  /* shift from 8 bit 4/10 dB to 1/10 dB */
  expected_agc_val = expected_agc_val * 4;
#endif /* !FEATURE_FTM_BYPASS_LNA_NV_CAL_POINTS */

  avg_count = 12;

#ifndef RF_HAS_DVGA_1OTH_OF_DB_RESOLUTION 
#error code not present
#endif

  for (loopcount=1; loopcount <= avg_count; loopcount++)
  {
    if (rf_chain_ptr->rf_mode == DB_RF_CDMA || rf_chain_ptr->rf_mode == DB_RF_PCN)
    {
      agc_read_val =  ftm_get_rf_cdma_rx_agc(device);
    }

    if (agc_read_val > 511)
    {
      agc_read_val = agc_read_val - 1024;    // handle 10 bit 2's comp number
    }

    current_agc_val += agc_read_val;    
    clk_busy_wait(500);
  }

#ifdef RF_HAS_DVGA_1OTH_OF_DB_RESOLUTION 
  //Make sure there is no truncation bias by adjusting by 0.5 magnitude!
  if( current_agc_val > 0 )
  {
    current_agc_val = (current_agc_val + avg_count/2) /avg_count;
  }
  else
  {
    current_agc_val = (current_agc_val - avg_count/2) /avg_count;
  }
#endif

  dvga_offset =  current_agc_val - expected_agc_val;
  g_dvga_offset = dvga_offset;

  #ifdef RF_HAS_POWER_MODE_LIB
   if( rf_intelliceiver_is_enabled(rfcom_to_path[device]) )
   {
     ftm_dvga_offset_pm[ftm_current_power_mode] = dvga_offset;
   }
  #endif

  ftm_Set_Dvga_Gain_Offset(device, g_dvga_offset);

  #ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO
  digital_filter_gain = ftm_get_digital_filter_gain(path);
  dvga_offset = dvga_offset - digital_filter_gain;
  #endif

  return(dvga_offset);

} /*ftm_get_tx_rx_dvga_offset*/

/*===========================================================================

FUNCTION FTM_GET_LNA_OFFSET                                     EXTERNAL

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  int2 = 10 bit two's LNA offset value

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_FTM_BYPASS_LNA_NV_CAL_POINTS
word ftm_get_lna_offset( rfcom_device_enum_type device, byte index, int2 agc_value )
#else
word ftm_get_lna_offset( rfcom_device_enum_type device, byte index )
#endif
{
  rf_chain_state_type *rf_chain_ptr;
  int2 lna_offset=0;
  int2 expected_agc_val=0;
  int2 previous_offset=0;
  int2 current_offset=0;
  int2 agc_read_val=0;
  int loopcount;
  int avg_count = DVGA_OFFSET_AVG;

  /* point to chain status structure for the device */
  rf_chain_ptr = &rf_chain_status[ rfcom_to_path[ device ] ];

  rf_set_cdma_lna_decision(device, (lna_gain_state_type)(index+1));

  switch (index)
  {
  case 0:
    ftm_set_lna_offset(device,0,0);
    previous_offset = 0;
#ifdef FEATURE_FTM_BYPASS_LNA_NV_CAL_POINTS
    expected_agc_val = agc_value;
#else
    expected_agc_val = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_rise;
#endif
    FTM_MSG_MED("Get LNA Offset (0) calibration point: %d", agc_value, 0, 0);
    break;
  case 1:
    ftm_set_lna_offset(device,1,g_lna_offset0);
    previous_offset = g_lna_offset0;
#ifdef FEATURE_FTM_BYPASS_LNA_NV_CAL_POINTS
    expected_agc_val = agc_value;
#else
    expected_agc_val = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_2_rise;
#endif
    FTM_MSG_MED("Get LNA Offset (1) calibration point: %d", agc_value, 0, 0);
    break;
  case 2:
    ftm_set_lna_offset(device,2,g_lna_offset1);
    previous_offset = g_lna_offset1;
#ifdef FEATURE_FTM_BYPASS_LNA_NV_CAL_POINTS
    expected_agc_val = agc_value;
#else
    expected_agc_val = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_3_rise;
#endif
    FTM_MSG_MED("Get LNA Offset (2) calibration point: %d", agc_value, 0, 0);
    break;
  case 3:
    ftm_set_lna_offset(device,3,g_lna_offset2);
    previous_offset = g_lna_offset2;
#ifdef FEATURE_FTM_BYPASS_LNA_NV_CAL_POINTS
    expected_agc_val = agc_value;
#else
    expected_agc_val = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_4_rise;
#endif
    FTM_MSG_MED("Get LNA Offset (3) calibration point: %d", agc_value, 0, 0);
    break;
  }

#ifndef FEATURE_FTM_BYPASS_LNA_NV_CAL_POINTS
  /* shift from 8 bit 1/3 dB  to 10 bits 1/12 */
  expected_agc_val = expected_agc_val * 4;

  if (expected_agc_val > 511)                      // handle 10 bit 2's comp number
    expected_agc_val = expected_agc_val - 1024;
#endif /* !FEATURE_FTM_BYPASS_LNA_NV_CAL_POINTS */

  ftm_clk_rex_wait (10);

  for (loopcount=1; loopcount <= avg_count; loopcount++)
  {
    if (rf_chain_ptr->rf_mode == DB_RF_CDMA || rf_chain_ptr->rf_mode == DB_RF_PCN)
      agc_read_val = ftm_get_rf_cdma_rx_agc(device);
    if (agc_read_val > 511)
      agc_read_val = agc_read_val - 1024;    // handle 10 bit 2's comp number

    current_offset += agc_read_val;

    ftm_clk_rex_wait (1);
  }

  current_offset = current_offset/avg_count;

  lna_offset = expected_agc_val - current_offset + previous_offset;

  switch (index)
  {
  case 0:
    g_lna_offset0 = lna_offset;
    break;
  case 1:
    g_lna_offset1 = lna_offset;
    break;
  case 2:
    g_lna_offset2 = lna_offset;
    break;
  case 3:
    g_lna_offset3 = lna_offset;
    break;
  }

  return((word)lna_offset);
} /* End ftm_get_rf_cdma_rx_agc */
/*===========================================================================

FUNCTION FTM_GET_TX_RX_LNA_OFFSET                                     EXTERNAL

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  int2 = 10 bit two's LNA offset value

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_FTM_BYPASS_LNA_NV_CAL_POINTS
word ftm_get_tx_rx_lna_offset( rfcom_device_enum_type device, byte index, int2 agc_value )
#else
word ftm_get_tx_rx_lna_offset( rfcom_device_enum_type device, byte index )
#endif
{
  rf_chain_state_type *rf_chain_ptr;
  int2 lna_offset=0;
  int2 expected_agc_val=0;
  int2 previous_offset=0;
  int2 current_offset=0;
  int2 agc_read_val=0;
  int loopcount;
  int avg_count = DVGA_OFFSET_AVG;

  /* point to chain status structure for the device */
  rf_chain_ptr = &rf_chain_status[ rfcom_to_path[ device ] ];

  if(ftm_current_power_mode> 2) 
  {
    FTM_MSG_ERROR("Get TX_RX_LNA Offset:Unsupported power mode for any RF Card", 0,0,0);
    return (word)-1;
  }

  #ifdef RF_HAS_POWER_MODE_LIB
  if( rf_intelliceiver_is_enabled(rfcom_to_path[device]) )
  {
    ftm_set_rf_power_mode( device,ftm_current_power_mode);

    ftm_Set_Dvga_Gain_Offset(device, ftm_dvga_offset_pm[ftm_current_power_mode]);
  }
  else
  #endif
  {
    if(ftm_current_power_mode != 0)
    {
      FTM_MSG_ERROR("Get TX_RX_LNA Offset:RFR device %d doesn't support the power mode %d as specified",ftm_get_rfr_device_type(device),ftm_current_power_mode,0);
      return (word)-1;
    }
  }

  rf_set_cdma_lna_decision(device, (lna_gain_state_type)(index+1));

  FTM_MSG_HIGH("Get TX_RX_LNA Offset:RFR device %d,power mode %d, agc_value %d",ftm_get_rfr_device_type(device),ftm_current_power_mode,agc_value);

  #ifdef RF_HAS_POWER_MODE_LIB
  if( rf_intelliceiver_is_enabled(rfcom_to_path[device]) )
  {
    switch(index)
    {
    case 0:
      ftm_set_lna_offset(device,index,0);
      previous_offset = 0;
      #ifdef FEATURE_FTM_BYPASS_LNA_NV_CAL_POINTS
      expected_agc_val = agc_value;  
      #else
      expected_agc_val = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_1_rise;
      #endif   
      break;
    case 1:
      ftm_set_lna_offset(device,index, ftm_lna_offset_pm[index-1][ftm_current_power_mode]);
      previous_offset =  ftm_lna_offset_pm[index-1][ftm_current_power_mode];
      #ifdef FEATURE_FTM_BYPASS_LNA_NV_CAL_POINTS
      expected_agc_val = agc_value;
      #else
      expected_agc_val = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_2_rise;
      #endif
      break;
    case 2:
      ftm_set_lna_offset(device,index, ftm_lna_offset_pm[index-1][ftm_current_power_mode]);
      previous_offset =  ftm_lna_offset_pm[index-1][ftm_current_power_mode];
      #ifdef FEATURE_FTM_BYPASS_LNA_NV_CAL_POINTS
      expected_agc_val = agc_value;
      #else
      expected_agc_val = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_3_rise;
      #endif
      break;
    case 3:
      ftm_set_lna_offset(device,index, ftm_lna_offset_pm[index-1][ftm_current_power_mode]);
      previous_offset =  ftm_lna_offset_pm[index-1][ftm_current_power_mode];
      #ifdef FEATURE_FTM_BYPASS_LNA_NV_CAL_POINTS
      expected_agc_val = agc_value;
      #else
      expected_agc_val = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_4_rise;
      #endif
      break;
    }    
  }
  else
  #endif
  {
    switch (index)
    {
    case 0:
      ftm_set_lna_offset(device,0,0);
      previous_offset = 0;
#ifdef FEATURE_FTM_BYPASS_LNA_NV_CAL_POINTS
    expected_agc_val = agc_value;
#else
    expected_agc_val = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_rise;
#endif
      FTM_MSG_MED("Get LNA Offset (0) calibration point: %d", agc_value, 0, 0);
      break;
    case 1:
      ftm_set_lna_offset(device,1,g_lna_offset0);
      previous_offset = g_lna_offset0;
#ifdef FEATURE_FTM_BYPASS_LNA_NV_CAL_POINTS
    expected_agc_val = agc_value;
#else
    expected_agc_val = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_2_rise;
#endif
      FTM_MSG_MED("Get LNA Offset (1) calibration point: %d", agc_value, 0, 0);
      break;
    case 2:
      ftm_set_lna_offset(device,2,g_lna_offset1);
      previous_offset = g_lna_offset1;
#ifdef FEATURE_FTM_BYPASS_LNA_NV_CAL_POINTS
    expected_agc_val = agc_value;
#else
    expected_agc_val = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_3_rise;
#endif
      FTM_MSG_MED("Get LNA Offset (2) calibration point: %d", agc_value, 0, 0);
      break;
    case 3:
      ftm_set_lna_offset(device,3,g_lna_offset2);
      previous_offset = g_lna_offset2;
#ifdef FEATURE_FTM_BYPASS_LNA_NV_CAL_POINTS
    expected_agc_val = agc_value;
#else
    expected_agc_val = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_lna_range_4_rise;
#endif
      FTM_MSG_MED("Get LNA Offset (3) calibration point: %d", agc_value, 0, 0);
      break;
    }
  }

#ifndef FEATURE_FTM_BYPASS_LNA_NV_CAL_POINTS
  /* shift from 8 bit 1/3 dB  to 10 bits 1/12 */
  expected_agc_val = expected_agc_val * 4;

  if (expected_agc_val > 511)
  { 
    // handle 10 bit 2's comp number
    expected_agc_val = expected_agc_val - 1024;
  
  }
#endif /* !FEATURE_FTM_BYPASS_LNA_NV_CAL_POINTS */

  //Why are we waiting here for 10 ms??
  clk_busy_wait(5000);

  
  for (loopcount=1; loopcount <= avg_count; loopcount++)
  {
    if (rf_chain_ptr->rf_mode == DB_RF_CDMA || rf_chain_ptr->rf_mode == DB_RF_PCN)
    {
      agc_read_val = ftm_get_rf_cdma_rx_agc(device);
    }

    if (agc_read_val > 511)
    {
      agc_read_val = agc_read_val - 1024;    // handle 10 bit 2's comp number
    }
    current_offset += agc_read_val;
    
   clk_busy_wait(500);
  }

  current_offset = current_offset/avg_count;

  lna_offset = expected_agc_val - current_offset + previous_offset;

  #ifdef RF_HAS_POWER_MODE_LIB
  if( rf_intelliceiver_is_enabled(rfcom_to_path[device]) )
  {
    ftm_lna_offset_pm[index][ftm_current_power_mode] =  lna_offset;    
  }
  else
  #endif
  {    
    switch (index)
    {
    case 0:
      g_lna_offset0 = lna_offset;
      break;
    case 1:
      g_lna_offset1 = lna_offset;
      break;
    case 2:
      g_lna_offset2 = lna_offset;
      break;
    case 3:
      g_lna_offset3 = lna_offset;
      break;
    }
  }     

  return((word)lna_offset);

} /*ftm_get_tx_rx_lna_offset*/

/*===========================================================================

FUNCTION FTM_CDMA_SET_TRK_LO

DESCRIPTION
  Adjusts the 16-bit PDM generator controlling the TRK_LO_ADJ pin

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_cdma_set_trk_lo
(
  int set_level        /* 16 bit PDM value to load */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  set_level = RF_SATURATE( set_level << 7, -32768, 32767);

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */
  {
    /* TRK_LO_ADJ_SEL 0: ARM control via TRK_LO_ADJ field
                      1: mDSP control
    */
    FTM_MSG_MED("Setting trk_lo to %d", set_level, 0, 0);

#ifdef FTM_HAS_AGC_CARRIER_FREQ_ERR
    /* Enable PDM clock regime. This is a temporary fix for the TRK LO function not working when
       the phone is switched from Online to FTM mode. If the phone is booted in the FTM mode, then
       TRK LO works fine. Somehow the CLK_RGM_CDMA_PDM_M clock is turned off when the phone is swtiched from 
       Online to FTM mode */  
    clk_regime_enable( CLK_RGM_CDMA_PDM_M );  

    /* CARRIER_FREQ_TRAK_ORIDE_N 0: ARM control via TRK_LO_ADJ field
                                 1: mDSP control
    */
    MSM_W_MASKED(AGC_FREQ_COMB_CTL, CARRIER_FREQ_TRK_ORIDE_N, 0);
    /* TRK_LO_ADJ_OE  0: TRK_LO_ADJ pin is tri-stated
                      1: TRK_LO_ADJ pin is driven
    */
    MSM_W_MASKED(AGC_FREQ_COMB_CTL, TRK_LO_ADJ_OE, 1);
    MSM_W_MASKED(AGC_CARRIER_FREQ_ERR, FREQ_ERR, set_level);
#else
    MSM_W_MASKED(DEM1X_FREQUENCY_CTL, TRK_LO_ADJ_SEL, 0);
    /* TRK_LO_ADJ_OE  0: TRK_LO_ADJ pin is tri-stated
                      1: TRK_LO_ADJ pin is driven
    */
    MSM_W_MASKED(DEM1X_FREQUENCY_CTL, TRK_LO_ADJ_OE, 1);
    MSM_W_MASKED(DEM1X_FREQUENCY_CTL, TRK_LO_ADJ, set_level);
#endif /* FTM_HAS_AGC_CARRIER_FREQ_ERR */
  }
} /* End ftm_cdma_set_trk_lo() */

/*===========================================================================

FUNCTION FTM_SET_SECONDARY_CHAIN

DESCRIPTION
  Sets the secondary chain to one of three operational modes: Disabled, Rx Diversity, OFS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_set_secondary_chain
(
  ftm_secondary_chain_control_enum_type state        /* secondary chain state */
)
{
#if !defined T_QSC1100
#ifdef FEATURE_CLKREGIM_2X_MODE
  #ifdef FTM_HAS_CGPS
#error code not present
  #endif

  switch(state)
  {
  case DISABLE_SECONDARY_CHAIN:
    switch (rf_path_state)
    {
    case RF_DIVERSITY_ST:
      rfm_disable_rx(RFCOM_RECEIVER_1);
      break;
    case RF_DUAL_ST:
    case RF_R1_ONLY_ST:
      rfm_disable_rx(RFCOM_RECEIVER_1);
      break;
    default:
      break;
    }
    FTM_MSG_MED("Disabling Secondary Chain", 0, 0, 0);
    break;

  case RECEIVE_DIVERSITY:
    srch4_hw_init(0, SAMPLE_SERVER_RX_CHAIN_BOTH);
    FTM_MSG_MED("Putting Secondary Chain in Rx Diversity", 0, 0, 0);
    switch (rf_path_state)
    {
    case RF_R0_ONLY_ST:
      rfm_enable_rx(RFCOM_RECEIVER_DIV, NULL, NULL);
      break;
    case RF_DUAL_ST:
      rfm_enable_rx(RFCOM_RECEIVER_DIV, NULL, NULL);
      break;
    case RF_DISABLED_ST:
    case RF_R1_ONLY_ST:
      FTM_MSG_ERROR("Second chain state: Can't go to diversity when primary is disabled", 0, 0, 0);
      break;
    default:
      break;
    }
	break;

  case OFF_FREQUENCY_SEARCH:
    srch4_hw_init(0, SAMPLE_SERVER_RX_CHAIN_BOTH);
    switch (rf_path_state)
    {
    FTM_MSG_MED("Putting Secondary Chain in OFS", 0, 0, 0);
    case RF_R0_ONLY_ST:
    case RF_DISABLED_ST:
    case RF_DIVERSITY_ST:
      rfm_enable_rx(RFCOM_RECEIVER_1, NULL, NULL);
      rfm_init_digital_band_chan(RFCOM_RECEIVER_1, rf_card.rfc_default_band, 300);
      rfm_complete_sleep_warmup(RFCOM_RECEIVER_1);
      break;
    default:
      break;
    }
    break;
  }
#endif
#endif /* !defined T_QSC1100 */
} /* End ftm_set_secondary_chain() */

/*===========================================================================

FUNCTION FTM_SET_RF_PRIMARY_CHAIN_STATE

DESCRIPTION
  Sets the primary chain to either into Sleep ON or Sleep OFF states

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_set_rf_primary_chain_state(ftm_primary_chain_state state)
{
  switch(state)
  {
  case PRIMARY_CHAIN_SLEEP:
    rfm_disable_rx(RFCOM_TRANSCEIVER_0);
    break;

  case PRIMARY_CHAIN_ON:
    rfm_enable_rx(RFCOM_TRANSCEIVER_0,NULL,NULL); 
    break;

  default:
    FTM_MSG_ERROR("Invalid Primary Chain state specified", 0, 0, 0);
    break;
  }
}/* End of ftm_set_rf_primary_chain_state() */

/*===========================================================================

FUNCTION FTM_SET_BETA_PRIME_SCALING

DESCRIPTION
  Implements the beta prime scaling on Targets QSC1100 & QSC60x5

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_set_beta_prime_scaling(int2 beta_resolution,word beta_scaled_val)
{
  word floor_val;
  double temp_val;
 
  #ifdef RF_HAS_MDSP_TX_AGC
  #ifdef RF_HAS_BETA_SCALING_TX_AGC
    if((FTM_BETA_PRIME_MDSP_RESOLUTION_MIN <= beta_resolution) 
       &&  (beta_resolution <= FTM_BETA_PRIME_MDSP_RESOLUTION_MAX)) /*Making sure beta_resolution is within the range of 1 to 640*/
    {      

      temp_val= ( (double) beta_scaled_val ) / ( -20.0 * beta_resolution );

      temp_val = pow( 10.0, temp_val );

      floor_val = (word) ROUND_VAL( ( temp_val * FTM_BETA_PRIME_HW_RESOLUTION ) - 1 );

      CDMA_MDSP_SET_TX_BETAP_VAL(floor_val);

      CDMA_MDSP_ENABLE_TX_BETAP_OVERRIDE();
    }
    else 
    {
      FTM_MSG_ERROR("Beta resolution entered not supported",0,0,0);
    }
  #else
    FTM_MSG_ERROR("RF_HAS_BETA_SCALING_TX_AGC not defined",0,0,0);
  #endif
  #else
    FTM_MSG_ERROR("RF_HAS_MDSP_TX_AGC not defined",0,0,0);
  #endif

}/* End of ftm_set_beta_prime_scaling(int2,word) */

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

void ftm_switch_mode_1x(rfcom_device_enum_type device)
{
#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */
} /* ftm_switch_mode_1x */

void ftm_tune_to_chan(rfcom_device_enum_type device, word rf_channel)
{
   rf_chain_state_type *rf_chain_ptr = &rf_chain_status[rfcom_to_path[device]];

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */
  {
      if(rf_path_state == RF_DIVERSITY_ST)
          device = RFCOM_RECEIVER_DIV;

      rfm_tune_band_chan(device, rf_chain_ptr->rf_curr_cdma_band, rf_channel);     
  }
} /* ftm_tune_to_chan */

/*=========================================================================

FUNCTION FTM_SYNTH_IS_LOCKED

DESCRIPTION
   This API function is called for checking the status 
   of synth lock for specific synth device

DEPENDENCIES
  None.

RETURN VALUE
  Boolean
  Retrurns TRUE if synth is locked else return FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean ftm_synth_is_locked( rfcom_device_enum_type device,synth_pll_for_synth_check pll_device )
{
  return synth_is_locked( device, pll_device );
} /* ftm_synth_is_locked */

void ftm_set_cdma_band(rfcom_device_enum_type device, rf_card_band_type band)
{
#ifdef RF_HAS_GPS
#error code not present
#else
  clk_regime_config_cdma_clocks(rfcom_to_path[device]);
#endif
  ftm_set_digital_band ( device, band );  
  ftm_clk_rex_wait( RF_ENABLE_PWR_WAIT * 5);

}

/*===========================================================================

FUNCTION       FTM_RX_ON_SECOND_CHAIN
DESCRIPTION
  This function configures the MSM muxes for demodulating the signal
  coming from the second RX chain.  Open loop power control is set by
  the calibration values loaded into the secondary receiver.
===========================================================================*/
void ftm_rx_on_second_chain(byte mode)
{
  if (mode == 1 || mode == 2)
  {
#if defined (T_MSM6700) || defined ( T_MSM6800) 
  HWIO_MODEM_GENERIC_CTL_REG_OUTM(MODEM_GENERIC_CTL_TX_AGC_CTL_SEL_CHAIN_M  ,
                                  MODEM_GENERIC_CTL_TX_AGC_CTL_SEL_CHAIN_1_V );

  HWIO_OUTM ( MODEM_MISC_CFG0, HWIO_FMSK( MODEM_MISC_CFG0,CDMA_RX0_ANT_SEL),
                         ( 1 << HWIO_SHFT (MODEM_MISC_CFG0,CDMA_RX0_ANT_SEL)) );

#else
    MSM_W_MASKED(WEB_MISC_WR, RXF0_ANT_SEL, 1);
    MSM_W_MASKED(WEB_MISC_WR, TX_AGC_CTL_SEL, 1);
#endif 
    rfm_enable_rx(RFCOM_TRANSCEIVER_0, NULL, NULL);
    rfm_enable_rx(RFCOM_RECEIVER_DIV, NULL, NULL);
    srchrdiv_set_ftm_second_chain_test( mode );
  }
  else
  {
#if defined (T_MSM6700) || defined ( T_MSM6800) 
  HWIO_MODEM_GENERIC_CTL_REG_OUTM(MODEM_GENERIC_CTL_TX_AGC_CTL_SEL_CHAIN_M  ,
                                  MODEM_GENERIC_CTL_TX_AGC_CTL_SEL_CHAIN_0_V );

  HWIO_OUTM ( MODEM_MISC_CFG0, HWIO_FMSK( MODEM_MISC_CFG0,CDMA_RX0_ANT_SEL),
                         ( 0 << HWIO_SHFT (MODEM_MISC_CFG0,CDMA_RX0_ANT_SEL)) );

#else
    MSM_W_MASKED(WEB_MISC_WR, RXF0_ANT_SEL, 0);
    MSM_W_MASKED(WEB_MISC_WR, TX_AGC_CTL_SEL, 0);
#endif 
    rfm_disable_rx(RFCOM_RECEIVER_DIV);
    srchrdiv_set_ftm_second_chain_test( 0 );
  }
}

/*===========================================================================

FUNCTION FTM_CONFIGURE_TX_SWEEP_CAL

DESCRIPTION
  This function allows the user to set the length of each step in the tx cal 
  sweep and the number of HDET readings averaged per step.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_configure_tx_sweep_cal(uint16 interval, byte num_avg)
{
  /* ftm_tx_cal_sweep_interval must be greater than or equal to 2 ms.
     default ftm_tx_cal_sweep_interval to 20 ms if in violatiion
   */
  if (interval >= FTM_TX_SWEEP_INTERVAL_LOWER_LIMIT) /* 2 ms */
  {
    ftm_tx_cal_sweep_interval = interval;
    FTM_MSG_LOW("Setting sweep register to %d, Avg: %d", interval,num_avg,0);
  }
  else
  { 
    ftm_tx_cal_sweep_interval = 20;
    FTM_MSG_ERROR("Tx sweep interval %d < 2ms. Set interval = 20ms.",0,0,0);
  }
  /* ftm_num_of_avg_hdet_read must be non-zero positive number because
     ftm_tx_sweep_cal_isr uses it as a divisor to average out hdet reading.
     default ftm_num_of_avg_hdet_read to 10 if in violatiion
   */
  if( num_avg < 1 )
  {
    ftm_num_of_avg_hdet_read = 10;
    FTM_MSG_ERROR("Tx sweep HDET avg <= 0. Set HDET avg = 10.",0,0,0);
  }
  else
  {
    ftm_num_of_avg_hdet_read = num_avg;
  }
}


#ifdef FEATURE_FTM_HWTC
#error code not present
#endif /* FEATURE_FTM_HWTC */

/*===========================================================================

FUNCTION       FTM_SET_HDET_TRACKING

DESCRIPTION
  Enables and disables hdet tracking.  If state is non-zero, the hdet
  tracking loop is turned on, and controlled to the value specified
  in set_point.  If state is zero, hdet tracking is disabled.
  
DEPENDENCIES
  Transmitter must be enabled.

RETURN VALUE
  None

SIDE EFFECTS
  Reconfigures the 1.25ms ISR for FTM use
===========================================================================*/
void ftm_set_hdet_tracking( boolean state, unsigned short set_point )
{
#ifndef RF_HAS_HDET_BYPASS
  if (state)
  {
    /* Allocate space for ISR state */
    if (hdet_tracking_params_ptr == NULL)
    {
      hdet_tracking_params_ptr = ftm_malloc(sizeof(hdet_tracking_params_type));

      if (hdet_tracking_params_ptr == NULL)
      {
        FTM_MSG_ERROR("Failed to allocate memory for ISR parameter structure", 0, 0, 0);
        return;
      }
    }

    /* Setup the ISR parameter block */
    hdet_tracking_params_ptr->hdet_accum = set_point << 8;
    hdet_tracking_params_ptr->hdet_set_point = set_point;
    hdet_tracking_params_ptr->hdet_tracking_pdm = 100 << 23;
    
    /* Stop the RF driver power callback from running, to prevent clashing with us */
    INTLOCK();
    tramp_set_isr( TRAMP_125_MS_ISR, NULL );
    clk_dereg( &rf_pwr_call_back );
    INTFREE();

    /* Initialize the PDM to a close value */
    ftm_cdma_set_tx_agc(hdet_tracking_params_ptr->hdet_tracking_pdm >> 23);

#ifdef RF_HAS_INTERNAL_HDET_SUPPORT
    /* HDET tracking is going to requrie many HDET readings, so enable
       the detector once and just leave it on all the time for optimized
       RF ADC readings.
    */
    if ( rfc_uses_internal_hdet(rf_chain_status[RF_PATH_0].rf_curr_cdma_band) )
    {
       rft_internal_hdet_enable( RF_PATH_0 );
    }
#endif /* RF_HAS_INTERNAL_HDET_SUPPORT */

    /* Give the HDET circuit time to settle */
    ftm_clk_rex_wait(50);

    /* Start the FTM HDET ISR to do closed loop control */
    tramp_set_isr( TRAMP_125_MS_ISR, ftm_hdet_tracking_isr );
  }
  else
  {
    tramp_set_isr( TRAMP_125_MS_ISR, NULL );

#ifdef RF_HAS_INTERNAL_HDET_SUPPORT
    if ( rfc_uses_internal_hdet(rf_chain_status[RF_PATH_0].rf_curr_cdma_band) )
    {
       rft_internal_hdet_disable( RF_PATH_0 );
    }
#endif /* RF_HAS_INTERNAL_HDET_SUPPORT */

    rf_enable_125_ms_isr( 0 );
  }
#endif /* RF_HAS_HDET_BYPASS */
} /* ftm_set_hdet_tracking */

/*===========================================================================

FUNCTION       FTM_HDET_TRACKING_ISR
DESCRIPTION
  1.25ms ISR callback function for closed-loop tracking of HDET.
  Adjusts TX_AGC_ADJ PDM to control HDET to the desired set point.
===========================================================================*/
void ftm_hdet_tracking_isr( void )
{
#if !defined( T_MSM6700 ) || !defined( T_MSM6800 )
#ifdef FEATURE_FTM_HWTC
#error code not present
#endif /* FEATURE_FTM_HWTC */
#endif /* !defined( T_MSM6700 ) || !defined( T_MSM6800 )  */
  rf_chain_state_type *rf_chain_ptr = 
      &rf_chain_status[ rfcom_to_path[ RFCOM_TRANSCEIVER_0 ] ];
  
  unsigned int hdet_accum;                       // kept at unsigned 16Q8
  unsigned int new_pdm_val;

  const unsigned short hdet_accum_scale = 63701; // 0.972 in unsigned 16Q16
  const unsigned short hdet_update_scale = 1835; // 0.028 in unsigned 16Q16

  const short pdm_proportional_scale = -152;     // -0.00463 in signed 16Q15

  rf_hdet_data = rf_read_hdet( rf_chain_ptr->rf_curr_cdma_band );

  hdet_accum = hdet_tracking_params_ptr->hdet_accum;
  hdet_accum = hdet_update_scale*(rf_hdet_data<<8)
    + hdet_accum_scale*hdet_accum;
  hdet_accum = hdet_accum >> 16; // Convert from 32Q24 back to 16Q8
  hdet_tracking_params_ptr->hdet_accum = hdet_accum;

  /* Compute the new PDM value based on the HDET error */
  new_pdm_val = hdet_tracking_params_ptr->hdet_tracking_pdm;
  new_pdm_val = new_pdm_val + pdm_proportional_scale*(((int)hdet_tracking_params_ptr->hdet_set_point<<8) - (int)hdet_accum);
/*   32Q23    =    32Q23    +       16Q15           *(           8Q0   to  16Q8                          -          16Q8   */

  hdet_tracking_params_ptr->hdet_tracking_pdm = new_pdm_val;

  /* Round and write to PDM */
  ftm_cdma_set_tx_agc((new_pdm_val + (1<<22)) >> 23);
  
#if !defined( T_MSM6700 ) || !defined( T_MSM6800 )
#ifdef FEATURE_FTM_HWTC
#error code not present
#endif /* FEATURE_FTM_HWTC */
#endif /* !defined( T_MSM6700 ) || !defined( T_MSM6800 )  */
} /* ftm_HDET_tracking_isr */

#ifdef RF_HAS_POWER_MODE_LIB
/*===========================================================================

FUNCTION FTM_SET_RF_POWER_MODE

DESCRIPTION
  Override the current RF power mode selection.  The power mode
  parameter is RF target dependent.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  RF performance may change.

===========================================================================*/
void ftm_set_rf_power_mode(rfcom_device_enum_type device,
                           rflib_power_mode_type power_mode)
{
  rf_path_enum_type path = rfcom_to_path[device];

  rf_intelliceiver_set_power_mode(path, power_mode);
} /* ftm_set_rf_power_mode */
#endif /* RF_HAS_POWER_MODE_LIB */

#endif /* FEATURE_FACTORY_TESTMODE */

/*===========================================================================

FUNCTION FTM_SET_TX_SWEEP_STEP_SIZE

DESCRIPTION
  This function allows the user to set the PDM step size for the TX cal sweep.
  The PDM will be increased by the given number of counts for each step in
  the sweep.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_set_tx_sweep_step_size(uint8 step)
{
  if (step >= FTM_UPLINK_SWEEP_CAL_MIN_STEP)
  {
    ftm_uplink_sweep_cal_step = step;
  }
  else
  {
    ftm_uplink_sweep_cal_step = FTM_UPLINK_SWEEP_CAL_MIN_STEP;
    MSG_ERROR("FTM TX sweep step size too low, using %d.", ftm_uplink_sweep_cal_step, 0, 0);
  }

}


/*===========================================================================

FUNCTION FTM_TX_RX_FREQ_CAL_LOG_RESULTS

DESCRIPTION
  This function sends a log packet with the results of the TX/RX frequency
  calibration sweep.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_tx_rx_freq_cal_log_results( void )
{
  uint16 i;
  log_ftm_gen_type *log_ptr;

  /* create log packet to return results */
  log_ptr = (log_ftm_gen_type*)ftm_log_malloc(FTM_LOG_TX_RX_SWEEP,
                                              sizeof(ftm_log_gen_type) +
                                              2*sizeof(uint16)*ftm_tx_rx_steps_per_freq *
                                                ftm_tx_rx_num_freqs +
                                              sizeof(uint16)*ftm_tx_rx_num_freqs +
                                              8*ftm_tx_rx_steps_per_freq +
                                              4);

  if (log_ptr == NULL)
  {
    FTM_MSG_ERROR("ftm_tx_rx_freq_cal_log_results: Cannot allocate the necessary Memory for Log packet",0,0,0);
    return;
  }

  if (log_ptr != NULL)
  {
    /* Copy the request parameters and the results into the log packet */
    i=0;

    /*lint -e(419) */  
    memcpy((void*)&(log_ptr->data[i]), &ftm_tx_rx_chain, 
           sizeof(uint16));
    i+=sizeof(uint16);

    /* num_freqs*/
    /*lint --e(415) */  
    /*lint -e(416) */ 
    log_ptr->data[i]=ftm_tx_rx_num_freqs;
    i+=1;

    /* num_step*/
    /*lint -e(415) */  
    /*lint -e(416) */
    log_ptr->data[i]=ftm_tx_rx_steps_per_freq;
    i+=1;

    /* channel_list */
    /*lint -e(416) */
    /*lint -e(416) */
    memcpy((void*)&(log_ptr->data[i]), &ftm_tx_rx_freq_cal_channels[0], 
           sizeof(uint16) * ftm_tx_rx_num_freqs);
    i+=sizeof(uint16) * ftm_tx_rx_num_freqs;

    /* tx_pdm_list */
    memcpy((void*)&(log_ptr->data[i]), &ftm_tx_rx_freq_cal_pdm_list[0], 
           sizeof(uint16) * ftm_tx_rx_steps_per_freq);
    i+=sizeof(uint16) * ftm_tx_rx_steps_per_freq;

    /* pri_expected_agc_val_list */
    memcpy((void*)&(log_ptr->data[i]), &ftm_tx_rx_freq_cal_exp_agc[0], 
           sizeof(int16) * ftm_tx_rx_steps_per_freq);
    i+=sizeof(int16) * ftm_tx_rx_steps_per_freq;

    /* primary_rx_results */
    memcpy((void*)&(log_ptr->data[i]), &ftm_tx_rx_freq_cal_rx_result[0], 
           sizeof(uint16) * ftm_tx_rx_steps_per_freq * ftm_tx_rx_num_freqs);
    i+=sizeof(uint16) * ftm_tx_rx_steps_per_freq * ftm_tx_rx_num_freqs;
    
    /* pa_range_list */
    memcpy((void*)&(log_ptr->data[i]), &ftm_tx_rx_freq_cal_pa_ranges[0], 
           ftm_tx_rx_steps_per_freq);
    i+=ftm_tx_rx_steps_per_freq;

    /* read_hdet_list */
    memcpy((void*)&(log_ptr->data[i]), &ftm_tx_rx_freq_cal_hdet_read_list[0], 
           ftm_tx_rx_steps_per_freq);
    i+=ftm_tx_rx_steps_per_freq;

    /* hdet_results */
    memcpy((void*)&(log_ptr->data[i]), &ftm_tx_rx_freq_cal_tx_result[0], 
           sizeof(uint16) * ftm_tx_rx_steps_per_freq * ftm_tx_rx_num_freqs);
    i+=sizeof(uint16) * ftm_tx_rx_steps_per_freq * ftm_tx_rx_num_freqs;

    /* rx_action_list */
    memcpy((void*)&(log_ptr->data[i]), &ftm_tx_rx_freq_cal_rx_actions[0], 
           ftm_tx_rx_steps_per_freq);
    i+=ftm_tx_rx_steps_per_freq;

    /* rx_pm_list */
   memcpy((void*)&(log_ptr->data[i]), &ftm_tx_rx_freq_cal_rx_lpm_list[0], 
          ftm_tx_rx_steps_per_freq);

    /* send log */
    log_commit( (log_type*)log_ptr );
  }
}

/*===========================================================================

FUNCTION FTM_TX_RX_FREQ_CAL_ISR

DESCRIPTION
  This ISR is called for each step of the TX/RX frequency calibration sweep.
  It performs DVGA offset and LNA offset calibration, reads HDET, and changes
  channels as required.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_tx_rx_freq_cal_isr( void )
{
  uint8 num_of_hdet_reads;
  uint8 current_seg_within_freq = ftm_tx_rx_freq_cal_current_segment % ftm_tx_rx_steps_per_freq;
  
  rf_chain_state_type *rf_chain_ptr = 
      &rf_chain_status[ rfcom_to_path[ RFCOM_TRANSCEIVER_0 ] ];

  uint16 temp_hdet_val = 0;

#ifdef FTM_COMPOSITE_CAL_ISR_DEBUG
  timetick_type timer_ftm_tx_rx_freq_cal_isr = timetick_get();

  if (ftm_tx_rx_freq_cal_current_segment >= 0)
  {
    //Display actual duration of last segment. Since the time displayed really corresponds to the previous segment, subtract 1.
    FTM_MSG_HIGH("TX RX Sweep SEGMENT # %d, duration time: %d",
               (ftm_tx_rx_freq_cal_current_segment-1), timetick_get_elapsed(timer_ftm_tx_rx_freq_cal_segment_dur, T_USEC), 0);
  }
  timer_ftm_tx_rx_freq_cal_segment_dur = timetick_get();//Initialize and re-initialize the segment duration variable

  FTM_MSG_HIGH("TX RX Sweep ISR START SEGMENT # %d", ftm_tx_rx_freq_cal_current_segment, 0, 0);
#endif

  /* check for very first segment and set PA/PDM to lowest gain/level for trigger */
  if (ftm_tx_rx_freq_cal_current_segment == -1)
  {
    //Should come in here only once and only at the beginning of the entire tx_rx sequence
    //Puts the transmitter at its lowest gain range and lowest output level for absolute min output power.
    ftm_set_pa_r1_r0(~rf_current_pa_r_map & 0x3);
    ftm_cdma_set_tx_agc(63);
    // The next subsequent segment serves as the trigger AND is the first segment to be measured as well as.
  }
  else 
  {
    /* if the current segment IS NOT the end of the sweep... */
    if (ftm_tx_rx_freq_cal_current_segment < ftm_tx_rx_steps_per_freq*ftm_tx_rx_num_freqs)
    {
      /* if current segment is the last segment of the current frequency */
      if (current_seg_within_freq == ftm_tx_rx_steps_per_freq-1)
      {
        /* last step of this frequency, tune to new channel */
        ftm_set_pa_r1_r0(~rf_current_pa_r_map & 0x3);
        ftm_cdma_set_tx_agc(63);
        ftm_tune_to_chan(RFCOM_TRANSCEIVER_0, ftm_tx_rx_freq_cal_channels[(ftm_tx_rx_freq_cal_current_segment/ftm_tx_rx_steps_per_freq+1)
                                                       %ftm_tx_rx_num_freqs]);                
      }
      else//If a measurement segment...
      {
        // If not a "NO ACTION" segment, i.e. a segment where work needs to be done...
        if (ftm_tx_rx_freq_cal_pa_ranges[current_seg_within_freq] != 
             FTM_TX_RX_SWEEP_NO_ACTION)
        //suggestion, instead of no action, make tuning a command packet option..
        {
          /* set tx pa range and pdm --   Is the pa range necessary? */
          ftm_set_pa_r1_r0(ftm_tx_rx_freq_cal_pa_ranges[current_seg_within_freq]);

          ftm_cdma_set_tx_agc(ftm_tx_rx_freq_cal_pdm_list[current_seg_within_freq]);

          /* read hdet if desired */

          // Move the whole if else to end so that hdet gets time to settle 
          if (ftm_tx_rx_freq_cal_hdet_read_list[current_seg_within_freq] != 0)
          {
            /* HDET needs time to settle. Time to settle varies based on HDET used.
               This time will need to be changed if any HDET used has longer 
               settling time. Before changing time make sure it doen not spil 
               over next ISR for composite cal. Wait is needed only for first
               reading. */
            clk_busy_wait(FTM_TX_RX_SWEEP_HDET_SETTLE_TIME);

            for ( num_of_hdet_reads = 0; 
                  num_of_hdet_reads < FTM_FREQ_CAL_SWEEP_NUM_HDET_AVG; 
                  num_of_hdet_reads++ )
            {
              temp_hdet_val = 0;

              temp_hdet_val = rf_read_hdet( rf_chain_ptr->rf_curr_cdma_band );
              ftm_tx_rx_freq_cal_tx_result[ftm_tx_rx_freq_cal_current_segment] += temp_hdet_val;

#ifdef FTM_COMPOSITE_CAL_ISR_DEBUG
              FTM_MSG_HIGH("TX RX Sweep ISR FTM HDET iter %d, Value %d", num_of_hdet_reads, temp_hdet_val, 0);
#endif

            }
            ftm_tx_rx_freq_cal_tx_result[ftm_tx_rx_freq_cal_current_segment] /= 
            FTM_FREQ_CAL_SWEEP_NUM_HDET_AVG;

#ifdef FTM_COMPOSITE_CAL_ISR_DEBUG
            FTM_MSG_HIGH("TX RX Sweep ISR FTM HDET Value %d", ftm_tx_rx_freq_cal_tx_result[ftm_tx_rx_freq_cal_current_segment], 0, 0);
#endif

          }/* end hdet read */
          else
          {
            ftm_tx_rx_freq_cal_tx_result[ftm_tx_rx_freq_cal_current_segment]=0;
          }
        }/* end tx section */

        ftm_current_power_mode =  ftm_tx_rx_freq_cal_rx_lpm_list[current_seg_within_freq];           

        /* perform rx action */
        switch (ftm_tx_rx_freq_cal_rx_actions[current_seg_within_freq])
        {
        case FTM_RX_DVGA_OFFSET:
          if(ftm_tx_rx_chain == 0)
          {
            ftm_tx_rx_freq_cal_rx_result[ftm_tx_rx_freq_cal_current_segment] = 
            ftm_get_tx_rx_dvga_offset(RFCOM_TRANSCEIVER_0,ftm_tx_rx_freq_cal_exp_agc[current_seg_within_freq]);
          }
          else if(ftm_tx_rx_chain == 1)
          {
            ftm_tx_rx_freq_cal_rx_result[ftm_tx_rx_freq_cal_current_segment] = 
            ftm_get_tx_rx_dvga_offset(RFCOM_RECEIVER_DIV,ftm_tx_rx_freq_cal_exp_agc[current_seg_within_freq]);
          }
          else
          {
            FTM_MSG_ERROR("ftm_tx_rx_freq_cal_isr:Unsupported Rx Chain %d",ftm_tx_rx_chain,0,0);
          }
          break;

        case FTM_RX_LNA_OFFSET0:
          if(ftm_tx_rx_chain == 0)
          {
            ftm_tx_rx_freq_cal_rx_result[ftm_tx_rx_freq_cal_current_segment] =
            ftm_get_tx_rx_lna_offset(RFCOM_TRANSCEIVER_0,0,ftm_tx_rx_freq_cal_exp_agc[current_seg_within_freq]);
          }
          else if(ftm_tx_rx_chain == 1)
          {
            ftm_tx_rx_freq_cal_rx_result[ftm_tx_rx_freq_cal_current_segment] =
            ftm_get_tx_rx_lna_offset(RFCOM_RECEIVER_DIV,0,ftm_tx_rx_freq_cal_exp_agc[current_seg_within_freq]); 
          }
          else
          {
            FTM_MSG_ERROR("ftm_tx_rx_freq_cal_isr:Unsupported Rx Chain %d",ftm_tx_rx_chain,0,0);
          }
          break;

        case FTM_RX_LNA_OFFSET1:
          if(ftm_tx_rx_chain == 0)
          {
            ftm_tx_rx_freq_cal_rx_result[ftm_tx_rx_freq_cal_current_segment] =
            ftm_get_tx_rx_lna_offset(RFCOM_TRANSCEIVER_0,1,ftm_tx_rx_freq_cal_exp_agc[current_seg_within_freq]);
          } 
          else if(ftm_tx_rx_chain == 1)
          {
            ftm_tx_rx_freq_cal_rx_result[ftm_tx_rx_freq_cal_current_segment] =
            ftm_get_tx_rx_lna_offset(RFCOM_RECEIVER_DIV,1,ftm_tx_rx_freq_cal_exp_agc[current_seg_within_freq]);
          }
          else
          {
            FTM_MSG_ERROR("ftm_tx_rx_freq_cal_isr:Unsupported Rx Chain %d",ftm_tx_rx_chain,0,0);
          }
          break;

        case FTM_RX_LNA_OFFSET2:
          if(ftm_tx_rx_chain == 0)
          {
            ftm_tx_rx_freq_cal_rx_result[ftm_tx_rx_freq_cal_current_segment] =
            ftm_get_tx_rx_lna_offset(RFCOM_TRANSCEIVER_0,2,ftm_tx_rx_freq_cal_exp_agc[current_seg_within_freq]);
          }
          else if(ftm_tx_rx_chain == 1)
          {
            ftm_tx_rx_freq_cal_rx_result[ftm_tx_rx_freq_cal_current_segment] =
            ftm_get_tx_rx_lna_offset(RFCOM_RECEIVER_DIV,2,ftm_tx_rx_freq_cal_exp_agc[current_seg_within_freq]);
          }
          else
          {
            FTM_MSG_ERROR("ftm_tx_rx_freq_cal_isr:Unsupported Rx Chain %d",ftm_tx_rx_chain,0,0);
          }
          break;

        case FTM_RX_LNA_OFFSET3:
          if(ftm_tx_rx_chain == 0)
          { 
            ftm_tx_rx_freq_cal_rx_result[ftm_tx_rx_freq_cal_current_segment] =
            ftm_get_tx_rx_lna_offset(RFCOM_TRANSCEIVER_0,3,ftm_tx_rx_freq_cal_exp_agc[current_seg_within_freq]);
          }
          else if(ftm_tx_rx_chain == 1)
          { 
            ftm_tx_rx_freq_cal_rx_result[ftm_tx_rx_freq_cal_current_segment] =
            ftm_get_tx_rx_lna_offset(RFCOM_RECEIVER_DIV,3,ftm_tx_rx_freq_cal_exp_agc[current_seg_within_freq]);
          }
          else
          {
            FTM_MSG_ERROR("ftm_tx_rx_freq_cal_isr:Unsupported Rx Chain %d",ftm_tx_rx_chain,0,0);
          }
          break;

        default:
          /* do nothing */
          ftm_tx_rx_freq_cal_rx_result[ftm_tx_rx_freq_cal_current_segment]=0;
          break;
        }/* end rx section */
      } /* end if not last step of this freq. */
    } /* end if not at end of sweep */
    else//If the current segment is the end of the sweep...
    {
      /* full frequency sweep is done */
      /* turn off timer interrupts */        
      timer_clr(&ftmclk.timer,T_NONE);
      ftm_set_tx(RFCOM_TRANSCEIVER_0,FALSE); 
      ftm_tx_rx_freq_cal_log_results();

#ifdef RF_HAS_INTERNAL_HDET_SUPPORT
      if ( rfc_uses_internal_hdet(rf_chain_status[RF_PATH_0].rf_curr_cdma_band) )
      {
         rft_internal_hdet_disable( RF_PATH_0 );
      }
#endif /* RF_HAS_INTERNAL_HDET_SUPPORT */
    }
  }/* end else clause for check for first (trigger) segment */

#ifdef FTM_COMPOSITE_CAL_ISR_DEBUG
  //Display work duration of current segment
  FTM_MSG_HIGH("TX RX Sweep ISR END SEGMENT # %d, work time: %d",
               ftm_tx_rx_freq_cal_current_segment, timetick_get_elapsed(timer_ftm_tx_rx_freq_cal_isr, T_USEC), 0);
#endif

  /* increment segment counter for next isr iteration */
  ftm_tx_rx_freq_cal_current_segment++;
}


/*===========================================================================

FUNCTION FTM_DO_TX_RX_FREQ_CAL_SWEEP

DESCRIPTION
  This function configures and starts the TX/RX frequency calibration sweep.
  
DEPENDENCIES
  None

RETURN VALUE
  Error code.

SIDE EFFECTS
  None

===========================================================================*/
uint16 ftm_do_tx_rx_freq_cal_sweep(uint16 rx_chain,
                                   uint8  num_freqs, 
                                   uint8  num_steps_per_freq, 
                                   uint16 step_length, 
                                   uint16 *freq_list_ptr, 
                                   uint16 *pdm_list_ptr, 
                                   int16  *exp_agc_val_ptr,                                   
                                   uint8  *pa_range_list_ptr, 
                                   uint8  *read_hdet_ptr, 
                                   uint8  *rx_action_ptr,
                                   uint8  *rx_lpm_list_ptr)

{
  uint8 i,j;
  ftm_tx_rx_error_code_type result = FTM_TX_RX_SUCCESS;

  uint32 step_length_32;

  /* start at -1 to indicate trigger step */
  ftm_tx_rx_freq_cal_current_segment = -1;

  /* lock interrupts as timer may be running */
   INTLOCK();

  if((rx_chain == 0) || (rx_chain == 1))
  {
    ftm_tx_rx_chain = rx_chain;
  } 

  /* limit num_steps_per_freq to <= FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ */ 
  if (num_steps_per_freq <= FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ)
  {
    ftm_tx_rx_steps_per_freq = num_steps_per_freq;
  }
  else
  {
    ftm_tx_rx_steps_per_freq = FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ;
    result |= FTM_TX_RX_STEPS_INVALID;
  }

  /* limit num_freqs to <= FTM_TX_RX_FREQ_CAL_MAX_FREQS */
  if (num_freqs <= FTM_TX_RX_FREQ_CAL_MAX_FREQS)
  {
    ftm_tx_rx_num_freqs = num_freqs;
  }
  else
  {
    ftm_tx_rx_num_freqs = FTM_TX_RX_FREQ_CAL_MAX_FREQS;
    result |= FTM_TX_RX_FREQS_INVALID;
  }

  /* limit step_length to >= FTM_TX_RX_MIN_STEP_LENGTH (20ms) */
  if (step_length < FTM_TX_RX_MIN_STEP_LENGTH)
  {
    step_length = FTM_TX_RX_MIN_STEP_LENGTH;
    result |= FTM_TX_RX_STEP_LEN_INVALID;
  }

  /* copy input data to local arrays, as packet memory may be reassigned */
  /* before sweep completion */
  for (i=0; i<ftm_tx_rx_num_freqs; i++)
  {
    ftm_tx_rx_freq_cal_channels[i] = freq_list_ptr[i];
    for(j=0; j<ftm_tx_rx_steps_per_freq; j++)
    {
      ftm_tx_rx_freq_cal_tx_result[ i*ftm_tx_rx_steps_per_freq+j] = 0;
    }
  }
  
  for (i=0; i<ftm_tx_rx_steps_per_freq; i++)
  {
    ftm_tx_rx_freq_cal_pa_ranges[i] = pa_range_list_ptr[i];
    ftm_tx_rx_freq_cal_pdm_list[i] = (uint16)(pdm_list_ptr[i]);
    ftm_tx_rx_freq_cal_hdet_read_list[i] = read_hdet_ptr[i];
    ftm_tx_rx_freq_cal_rx_actions[i] = rx_action_ptr[i];
    ftm_tx_rx_freq_cal_exp_agc[i] = (int16)(exp_agc_val_ptr[i]);
    ftm_tx_rx_freq_cal_rx_lpm_list[i] = rx_lpm_list_ptr[i];
  }
  
#ifdef RF_HAS_POWER_MODE_LIB
  for (i=0;i<RFLIB_MAX_POWER_MODE;i++)
  {
     ftm_dvga_offset_pm[i] = 0;

     for (j=0;j<4;j++)
     {
       ftm_lna_offset_pm[j][i] = 0; 
     }
  }
#endif /* RF_HAS_POWER_MODE_LIB */

#ifdef RF_HAS_INTERNAL_HDET_SUPPORT
  /* Since several HDET readings will be taken, enable internal HDETs
     just once, take the readings, and then turn it off later
  */
  if ( rfc_uses_internal_hdet(rf_chain_status[RF_PATH_0].rf_curr_cdma_band) )
  {
     rft_internal_hdet_enable( RF_PATH_0 );
  }
#endif /* RF_HAS_INTERNAL_HDET_SUPPORT */

  /* tune to the first channel */
  ftm_tune_to_chan(RFCOM_TRANSCEIVER_0,ftm_tx_rx_freq_cal_channels[0]);

  /* turn on tx  and set tx agc to zero*/
  ftm_set_tx(RFCOM_TRANSCEIVER_0,TRUE);
  ftm_set_pa_r1_r0(~rf_current_pa_r_map & 0x3); // Set to lo gain range
  ftm_cdma_set_tx_agc( 63 );  
   
  /* Convert step_length from sleep-clock cycles to milliseconds units */  
  step_length_32 = (uint32) step_length;
  // Add 32768 / 2 = 16384 which is 0.5 ms to round milliseconds to the nearest integer
  step_length_32 = (step_length_32 * 1000 + 16384) /32768;
  step_length = (uint16) step_length_32;

  /* Install the FTM Tx/Rx Freq. Sweep Cal ISR */
  /* Must be done AFTER ftm_wcdma_set_tx() call */
  timer_def(&ftmclk.timer, NULL, NULL, 0, (timer_t1_cb_type)ftm_tx_rx_freq_cal_isr, 0);
  timer_set( &ftmclk.timer, step_length, step_length, T_MSEC );
  // Consider moving the definition to FTM init routines

  INTFREE();

  return result;
}

#if defined (FTM_HAS_XO_FACTORY_CAL) 
 /*===========================================================================

FUNCTION FTM_DO_XO_DC_CAL

DESCRIPTION
  This function is used to perform the coarse frequency dc offset calibration 
  for xo-based targets
  
DEPENDENCIES
  Need to set band and channel .
  

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

ftm_rsp_pkt_type ftm_do_xo_dc_cal( ftm_xo_cal_request_type * req )
{
    
    ftm_rsp_pkt_type rsp;
    ftm_xo_cal_response_type *xo_cal_rsp_ptr;

    coarse_cal_cnt ++;  // For debug to make sure API is called only once.
                        // Diag Timeouts result in API being called multiple times when using 
                        // QXDM send_data.

    rsp = ftmdiag_create_new_pkt(sizeof(ftm_xo_cal_response_type));

    xo_cal_rsp_ptr = rsp.pkt_payload;

    xo_cal_rsp_ptr->cmd = req->cmd;
    xo_cal_rsp_ptr->sub_sys = req->sub_sys;
    xo_cal_rsp_ptr->mode = req->mode;
    xo_cal_rsp_ptr->ftm_cmd = req->ftm_cmd;
    xo_cal_rsp_ptr->req_len = req->req_len;
    xo_cal_rsp_ptr->rsp_len = rsp.pkt_len;

    ftm_fac_cal_params.state = TCXOMGR_CAL_STATE_NOT_CAL;  // Setting the state to NOT_CAL prior to cal

    // Initializing TXCO manager structures
    tcxomgr_cal_param_init(&ftm_fac_cal_params, &ftm_samples, &ftm_info_pkt); 

    // Parameters to the TCXO manager APIS
    ftm_fac_cal_params.band = rf_chain_status[0].rf_curr_cdma_band;
    ftm_fac_cal_params.chan = rf_chain_status[0].rf_chan;
    ftm_fac_cal_params.offset = req->offset;
    ftm_fac_cal_params.temp_span = req->temp_span;
    ftm_fac_cal_params.min_ft_sample = req->min_ft_samples;
    ftm_fac_cal_params.max_ft_sample = req->max_ft_samples;
    ftm_fac_cal_params.substage_time = req->substage_time;
    ftm_fac_cal_params.pa_range = req->pa_range;
    ftm_fac_cal_params.agc_level = req->pdm_value;
    ftm_fac_cal_params.update_nv = req->write_nv;
    ftm_fac_cal_params.timeout_time = req->timeout;
    
    //Calling the tcxo mgr API for coarse DC Cal
    tcxomgr_cal_dc_coarse(&ftm_fac_cal_params, &ftm_samples, &ftm_info_pkt);

    //Results
    xo_cal_rsp_ptr->state = ftm_fac_cal_params.state;
    xo_cal_rsp_ptr->nom_coef = ftm_fac_cal_params.nom_coef;
    xo_cal_rsp_ptr->cal_coef= ftm_fac_cal_params.cal_coef;
    xo_cal_rsp_ptr->tcxomgr_cal_info_pkt = ftm_info_pkt;
    
    return rsp;
}

/*===========================================================================

FUNCTION FTM_DO_XO_FT_CURVE_CAL

DESCRIPTION
  This function is used to perform the fine frequency dc offset calibration 
  and calibrating the real FT curve for xo-based targets
  
DEPENDENCIES
  Need to call ftm_do_xo_dc_cal() prior.

RETURN VALUE
  None

SIDE EFFECTS
  None
                        
===========================================================================*/

ftm_rsp_pkt_type ftm_do_xo_ft_curve_cal( ftm_xo_cal_request_type* req )
{
    ftm_rsp_pkt_type rsp;
    ftm_xo_cal_response_type *xo_cal_rsp_ptr;

    fine_cal_cnt++;     // For debug to make sure API is called only once.
                        // Diag Timeouts result in API being called multiple times when using 
                        // QXDM send_data.

    rsp = ftmdiag_create_new_pkt(sizeof(ftm_xo_cal_response_type));
    xo_cal_rsp_ptr = rsp.pkt_payload;

    
    xo_cal_rsp_ptr->cmd = req->cmd;
    xo_cal_rsp_ptr->sub_sys = req->sub_sys;
    xo_cal_rsp_ptr->mode = req->mode;
    xo_cal_rsp_ptr->ftm_cmd = req->ftm_cmd;
    xo_cal_rsp_ptr->req_len = req->req_len;
    xo_cal_rsp_ptr->rsp_len = rsp.pkt_len;

    //Create Request packet parameters
    ftm_fac_cal_params.band = rf_chain_status[0].rf_curr_cdma_band;
    ftm_fac_cal_params.chan = rf_chain_status[0].rf_chan;
    ftm_fac_cal_params.offset = req->offset;
    ftm_fac_cal_params.temp_span = req->temp_span;
    ftm_fac_cal_params.min_ft_sample = req->min_ft_samples;
    ftm_fac_cal_params.max_ft_sample = req->max_ft_samples;
    ftm_fac_cal_params.substage_time = req->substage_time;
    ftm_fac_cal_params.agc_level = req->pdm_value;
    ftm_fac_cal_params.pa_range = req->pa_range;
    ftm_fac_cal_params.update_nv = req->write_nv;
    


    // Call XO Manager APIs
    tcxomgr_cal_c1c2c3(&ftm_fac_cal_params, &ftm_samples, &ftm_info_pkt);
    tcxomgr_cal_dc_fine(&ftm_fac_cal_params, &ftm_samples, &ftm_info_pkt);
    

    //Results
    xo_cal_rsp_ptr->state = ftm_fac_cal_params.state;
    xo_cal_rsp_ptr->nom_coef = ftm_fac_cal_params.nom_coef;
    xo_cal_rsp_ptr->cal_coef= ftm_fac_cal_params.cal_coef;
    xo_cal_rsp_ptr->tcxomgr_cal_info_pkt = ftm_info_pkt;

    return rsp; 
}

/*===========================================================================

FUNCTION FTM_SET_TX_XO_CAL

DESCRIPTION
   This function will set the TX state for FM or CDMA.
   Used for XO Calibration.   Same as ftm_set_tx() but uses rex_sleep() instead 
   ftm_clk_rex_wait().   This allows use of API in non-FTM task.

   This function is temporary until determine if rex_sleep() can be used in 
   mainline version of ftm_set_tx_xo_cal()

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_set_tx_xo_cal(rfcom_device_enum_type device, boolean state)
{
#if defined( T_MSM6700 ) || defined( T_MSM6800 )
  muxmdsp_wr_passThruEnable(TRUE); // Enable DSP pass through for the encoder to be in charge
#endif /* defined ( T_MSM6700 ) || defined ( T_MSM6800 )*/

  if ( state == ON )
  {
#ifdef FEATURE_GSM
#error code not present
#endif

    #ifdef RF_HAS_MDSP_TX_AGC    
    CDMA_MDSP_WR_TX_CLOSED_LOOP_VAL(0);
    CDMA_MDSP_ENABLE_TX_CLOSED_LOOP_OVERRIDE();
    #else
    /* overide TX_GAIN_ADJ accumulator */
    MSM_W_MASKED(TX_AGC_CTL2, PWR_CTL_EN, 0);
    /* Set output power to a medium level */
    HWIO_OUT(TX_GAIN_ADJ_WR, 0);
    #endif /* RF_HAS_MDSP_TX_AGC */

    enc_fch_enable( TRUE ); /* in IS95-A mode, the IS95-C FCH enable bit must be set */

  #ifdef T_MSM6700
    #ifdef RF_HAS_MDSP_TX_AGC
    /* PA_RANGE_SEL is controlled by FW */
    /* TX_PUNCT and PA_PUNCT is controlled by HW */
    HWIO_OUT( MOD_PA_ON_CTL, 0x04 );
    #else
    HWIO_OUT( MOD_PA_ON_CTL, 0x00 );
    #endif
  #endif
    MSM_W_MASKED( RXF_MODEM(PA_ON_CTL), TX_ON_EN, 1);

    #ifdef RF_HAS_MDSP_TX_AGC
    MSM_W_MASKED(DEM1X_COMBINER_CTL,ENABLE,1);
    #endif
    
    /* rex_sleep() required for use external to FTM Task */

    rf_enable_tx_pwr(device);
    rex_sleep( RF_ENABLE_PWR_WAIT );

    rf_tx_enable(device);
    rex_sleep( RF_TUNE_CHAN_WAIT );

    enc_tx_enable( TRUE );           /* Mask transmit path */

    /* Insure the TX PDM is enabled and the correct polarity */
  #ifdef T_MSM6700
    MSM_W_MASKED(MOD_TX_AGC_ADJ_CTL, TX_AGC_ADJ_OE, 1);
    MSM_W_MASKED(MOD_TX_AGC_ADJ_CTL, TX_AGC_ADJ_POLARITY, 1);
  #else
    MSM_W_MASKED(TX_AGC_CTL, TX_AGC_ADJ_OE, 1);
    MSM_W_MASKED(TX_AGC_CTL, TX_AGC_ADJ_POLARITY, 1);
  #endif
    enc_acp_pa_ctl(ON);
    #ifdef FTM_SC2X_HAS_HWTC_TX_TONE_ENABLED
    if(ftm_tx_tone_gen_enabled == FALSE)
    { 
    /* turn ON CDMA right here.*/
    ftm_cdma_pilot_waveform(ON);
    }
    #else
    ftm_cdma_pilot_waveform(ON);
    #endif

    FTM_MSG_MED("Setting TX ON",0,0,0);
  }
  else if ( state == OFF )
  {
    #ifdef FTM_SC2X_HAS_HWTC_TX_TONE_ENABLED 
    ftm_tx_tone_gen_enabled = FALSE;
    #endif

    rf_tx_disable(device);

    #ifdef RF_HAS_MDSP_TX_AGC
    MSM_W_MASKED(DEM1X_COMBINER_CTL,ENABLE,0);
    #endif
    enc_acp_pa_ctl(OFF);
    /* Turn off the CDMA waveform. */
    ftm_cdma_pilot_waveform(OFF);

    FTM_MSG_MED("Setting TX OFF (XO_CAL)",0,0,0);
  } /* else if */
} /* ftm_set_tx_xo_cal */

#endif /* FTM_HAS_XO_FACTORY_CAL */

/*===========================================================================

FUNCTION FTM_DO_INTERNAL_DEVICE_CALIBRATION

DESCRIPTION
  This function performs any required internal device calibration for all
  devices on the current target (e.g. vco coarse tuning, baseband filter cal,
  iq resistor cal, etc.). It will perform the calibration regardless of the 
  state of the NV items.
  
DEPENDENCIES
  NV task must be running.

RETURN VALUE
  16-bit error code

SIDE EFFECTS
  Writes calibration results to NV.

===========================================================================*/
uint16 ftm_do_internal_device_calibration( void )
{
  return (uint16)rflib_dev_internal_calibration(rex_self(),          /* current task */
                                 FTM_SIG_WAIT_SIG,    /* signal to wait for nv */
                                 ftm_sig_rex_wait_cb, /* callback for waiting */
                                 TRUE);               /* flag to bypass NV check */
}
