#ifndef RF_H
#define RF_H
/*===========================================================================

                      RF Unit Header File

DESCRIPTION
  This header file contains all the definitions necessary in order to
  interface with the RF Unit.  The RF unit provides interface calls
  for accessing the analog common card and PCN applique synthesizers
  and other miscellaneous items.

Copyright (c) 1991, 1992, 1993 by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 1994, 1995, 1996 by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 1997, 1998, 1999 by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2000, 2001, 2002 by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2003, 2004, 2005 by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2006, 2007       by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //source/qcom/qct/modem/rfa/rf/common/main/qsc1100/src/rf/rf.h#4 $
$DateTime: 2008/12/04 16:23:46 $
$Author: vmahajan $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/03/08   vm      Added the Support for Temperature based Device
                   Configration change
06/11/08   sl      Added support for UINT16 HDET under new API FTM_GET_ALL_HDET_FROM_TX_SWEEP_CAL_V2=144
                   and under feature RF_HAS_HDET_UINT16.
12/06/07   ems     Support for QSC1100
12/04/07   vm      Support for HDR/1X ACESS Probe and PA Switchpoint backoff
10/02/07   jfc     Create get/set functions for rf_pwr_on_wakeup
09/21/07   jfc     Compile rf_write_tech_dependent_nv_items_to_mdsp() for WCDMA only
08/10/07   vm      Added the support for programming different PA switchpoints
                   and PA hysterisis timer during Access Probes different from 
                   traffic
08/09/07   ad      Added support for reading HS LNA switch point NVs during HS calls.
08/07/03   vm      Added Interface API for programming of PA Hysterisis
                   timer by Encoder
08/06/07   jfc     Featurized code for EDGE-only builds
06/13/07   kss     Added support for configuring DO chip delay.
06/13/07   jfc     Added prototype for rf_init_target_dependent_settings() and
                    rf_pwr_on_wakeup()
05/01/07   adk     Added prototype for rf_hdr_mdsp_tx_set_port_reg_address()
03/22/07   jfc     Removed UMTS version of rf_get_nv_band and rf_get_supported_hw
03/13/07   bmg     Addded rf_read_hdet for internal RF driver HDET reads
02/12/07   jfc     Added channel type parameter to set_tx_general_ctl()
02/07/07   vm/adk  Added support for firmware TX AGC
02/05/07   ycl     Merge to 7200 tip.
01/25/07   jfc     Including rflib_share.h for CDMA1x
01/23/07   lcl     Changed rf_wakeup prototype to include path information.
01/05/07   adk     Added prototypes for rf_hdr_init_agc() and rf_hdr_capture_agc().
12/18/06   jfc     Added rf_is_sleeping_cdma1x() and rf_is_sleeping_wcdma()
10/18/06   ycl     Added AGC structures for HDR.
10/17/06   ycl     Modifications for initial MSM7600 build.
09/11/06   kss     Added rf_hdr_init_agc(); changed rf_hdr_start_agc_acq().
08/15/06   kss/grl Added support for RESUME_AGC_ACQ mode in HDR.
08/01/06   mt      Added enum for RF HDR AGC ACQ mode. 
06/21/06   hkk     Relocated RF_HDR_REVA_RATE_OFFSET to rf1x.h
04/28/06   vm      Enum definition for adding PA Offset for HDR RevA
03/07/06   hxw     Added RF_GPS_READ_AGC_DC_OFFSET() to read AGC and DC I/Q offset.
01/26/06   sar     Added prototype for rf_tx_shutdown().
01/19/06   rmd     Implemented BC11 and BC14 code review results. Including:
                   * Moving rf_get_valid_bc_and_mask() into rf_util_freq.c.
01/19/06   sar     Fixed merge error in the last check to remove featurization.
11/21/05   sar     Removed featurization from prototype rf_is_sleeping().
                   Added prototype for rf_freeze/unfreeze_cagc().
11/26/05   et      removed FEATURE_SRCH_PCG condition around rf_is_sleeping() 
11/18/05   sar     Relocated some prototypes to fix featurization.
11/14/05   sar     Added prototype rf_get_lna_gain_state. Moved 
                   rferrno_enum_type, RF_HDR_GET_RX0_AGC_DB256() and 
                   RF_HDR_GET_RX1_AGC_DB256 to rf1x.h.
10/25/05   sar     Added prototypes for rf_has_high_sinr, rf_complete_sleep_warmup
                   and rf_get_path_cdma_rx_agc. Also added extern declaration to 
                   rf_has_high_sinr().
10/20/05   sar     Added function rf_is_band_chan_supported().
10/20/05   sar     changed the return type for rf_get_sub_class to uint32.
10/18/05   rmd     Updated code according to code review recommendations.
10/04/05   kss     Added rf_hdr_log_tx_agc() to support new HDR log packets.
09/19/05   rmd     Added support for band class 14 (US PCS + 5MHz).
09/02/05   hkk     Backed out park mode changes
08/17/05   ra      changed rf_bypass_rtc_ctl_on_off to handle device and on/off
08/13/05   ra      added support for bypass rtc(enable/disable)
07/04/05   dyc     Lint cleanup.
05/27/05   sar     Removed definitions for RF_LOAD_AND_OVERRIDE_AGC_TRK_LO_ADJ
                   and rf_enable_freq_trk().
05/12/05   sar     Added support for APIs to query Band Subclass and Antenna 
                   Attributes.
05/08/05   dyc     Comment updates regarding rf_init.
04/26/05   sar     Removed function rf_get_band_mask().
02/14/05   kss     Added rf_hdr_notify_data_rate().
11/04/04   dbc     Moved types for PA Backoff to rf1x.h
09/20/04   dyc/ab  rf_pre_tune_to_gps() remove freq err parameter 
09/17/04   dyc     Added rf_get_nv_band()
08/16/04   dyc     Remove RF_HAS_FM component.
08/10/04   sar     Added an interim rf_get_band_chan_temp() function to support
                   both rf chains.
08/06/04   sar     Temporarily backing out the last change.
08/04/04   sar     Added device parameter to rf_get_band_chan().
05/21/04   sar     Added API rf_get_supported_bands() to query rf supported bands.
05/21/04   dbc     Added new types/prototypes for PA Backoff update
05/10/04   dyc     Header comment updates for rf_get_band_chan_wait_time() and
                   rf_tune_band_chan()
03/31/04   dyc     Added rf_get_path_cdma_rx_agc_in_dBm()
02/23/04   kss     Use RF_HAS_HDR instead of FEATURE_HDR.
02/09/04   dyc     Remove rf_init_bbrx_sleep()
11/06/03   dbc     Added prototype for rf_get_path_cdma_rx_agc_full_scale()
11/05/03   dyc     Added rf_set_stay_asleep() prototype.
11/05/03   ra      Added Tx AGC, Tx PA, and Ratchet debug screen API
11/04/03   kss     Added prototype for rf_init_hdr_band_chan().
10/22/03   kss     Added device parameter to rf_hdr_stop_agc().
10/14/03   dyc     Mainlined T_MSM6500    
10/08/03   dbc     Changed input type of rf_get_cdma_rx_agc_in_dBm() to unit16
10/01/03   dyc     Cleanup. Changed param variable name to be consistent with type.
09/30/03   dbc     Change prototype of rf_get_cdma_rx_agc_in_dBm() to take 
                   uint8 as input
09/29/03   dbc     Change prototype of rf_get_cdma_rx_agc_in_dBm()
09/19/03   dyc     Updated rf_complete_warmup()function description.
09/12/03   sar     Added device parameter to functions: rf_is_sleeping(), 
                   rf_is_band_chan() and rf_post_cdma_reacq().
09/08/03   kss     Add HDR RF interface prototypes.
09/05/03   sar     Changed rf_stay_asleep() to take rfcom_device_enum_type
                   parameter.
08/26/03   sar     Changed the type of first parameter in 
                   rf_notify_sch_data_rate() to rfcom_device_enum_type.
08/25/03   dyc     Added rf_get_warmup_time() API function.
08/11/03   dbc     Added rf_enable_diversity() API function
07/24/03   et      changed prototype of rf_set_agc_dc_loop_gain
06/15/03   kss     Added rf_init_hdr() for HDR support.
05/12/02   et      added previously existing function rf_is_sleeping
                   under defined(FEATURE_HDR) && defined(FEATURE_SRCH_PCG)
04/28/03   et      merged MSM6500 register porting code
03/06/03   dbc     Added rf_get_cdma_rx_agc_in_dBm() API function.
03/03/03   dbc     Added rf_set_mdsp_cdma_spectral_inversion() to initialize RX 
                   CDMA spectral inversion bit in mDSP when 
                   RF_DSP_HAS_RX_SPECTRAL_INV_CONFIG is defined
02/20/03   aks     Changed rf_sleep() to pass in boolean, to remain backward
                   compatible.
02/12/03   sar     Changed rf_sleep return type to void for multimode.
                   Moved common 1X definitions to rf1x.h and include header.
01/29/03   aks     Make rf_init_status_type unconditionally available.
01/24/03   aks     Changed the return type of rf_init_delayed().
01/24/03   dbc     Added rf_store_1x_sleep_params() prototype
01/21/03   aks     Changed return type of rf_init_bbrx_sleep().
12/20/02   dbc     Added rf_init_digital_band_chan()
12/17/02   zg      Added rf_stay_asleep_tcxo_pdm_on() for GPS fake sleep.
12/17/02   dbc     Changed parameter in rf_get_band_chan() from int* to word*
12/13/02   aks     Export the function to set the sleepb_bbr line.
12/01/02   dbc     Added rf_notify_sch_data_rate().  This is used when 
                   FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST is defined.
11/25/02   dbc     Added rf_get_band_chan() API function
10/30/02   dbc     Added rf_tune_band_chan() and rf_get_band_chan_wait_time() 
                   API functions.
08/29/02   dbc     Cleanup.
08/28/02    et     added function rf_get_tx_warmup
08/08/02   dbc     Added rf_get_min_tx_pwr_in_dB() API function.
08/02/02   dbc     Added rf_cagc_dynamic_range_type and rf_cagc_pwr_rails_type
                   types. Added rf_get_cagc_linearizer_range() and 
                   rf_get_cdma_power_rails() API functions.
05/24/02   jwh     FEATURE_HWTC
04/18/02   dbc     Added RF_LOAD_AND_OVERRIDE_AGC_TRK_LO_ADJ() and 
                   RF_DISABLE_AGC_TRK_LO_ADJ_OVERRIDE() macros
04/10/02   aks     Changed rf_sleep() interface to match with mainline Search.
03/28/02   sar     Merged from MSM. Changed rf_sleep() and added 
                   rf_store_1x_sleep_params() prototypes.
03/20/02   sar     Merged from MSM. Featurized the following changes:
                   Added RF_BAND_SELECT_WAIT and split RF_ENABLE_PWR_WAIT into 
                   RF_ENABLE_TX_PWR_WAIT and RF_ENABLE_TX_PWR_WAIT.
02/22/02    zg     Replaced FEATURE_GPSONE with RF_HAS_GPS. 
02/14/02   aks     Removed TRK_LO_ADJ* macros.
02/13/02   dbc     Added zif logging code.
01/09/02   dbc     Added rf_get_tx_open_loop() and rf_get_tx_gain_adjust() API 
                   functions.  Removed all macros that were duplicated in 
                   msm_drv.h.
01/04/02   dbc     Removed TX_SET_GAIN_ADJUST macro.  It is defined in
                   msm_drv.h.
12/18/01   bgc     Changed reads of CARRIER_FREQ_ERROR to use MSM6000 
                   workaround.
12/07/01   dbc     Removed macros TRK_LO_ADJ_SET_NORMAL_POLARITY and
                   TRK_LO_ADJ_SET_INVERTED_POLARITY.  These are defined in
                   msm_drv.h
12/03/01    et     added #define for RF_CFS_TUNE_WAIT
11/21/01   dbc     More RF Driver decomposition changes.
11/02/01   dbc     Removed backwards compatible (pre MSM6000 specific) code
10/26/01   dbc     More temporary modifications for MSM6000 CARRIER_FREQ_ERR_RD 
                   hardware bug gated by T_MSM_6000.
10/25/01   dbc     Changed behavior of TRK_LO_ADJ_GET_FREQ_ERR_HIGH() and 
                   TRK_LO_ADJ_GET_FREQ_ERR() for MSM6000 due to hardware bug 
10/22/01   dbc     Updated macros TRK_LO_ADJ_GET_FREQ_ERR_HIGH() and 
                   TRK_LO_ADJ_GET_FREQ_ERR() for MSM6050 support
10/19/01   dbc     Added more support for MSM6000 and MSM6050 targets.
                   lna_gain_state_type enum added.
                   RF_CAP_PDM1_FOR_TX, RF_CAP_PDM2_FOR_TX, RF_HAS_PA_RANGE_CAL 
                   support removed. FEATURE_PA_RANGE_TEMP_FREQ_COMP mainlined.
10/17/01   dbc     Migrated CAGC specific macros from msm_drv.h to this file.
                   Removed function prototypes for unused functions rf_get_i_offset(),
                   rf_get_q_offset(), rf_set_frq_error().
10/08/01   aks     Included rxf.h, merged in phone_t changes.
10/01/01   dbc     Made changes required for port to MSM6000+ targets.
09/19/01   dbc     Removed RF_HAS_RFR3100 dependencies - SW LNA Control removed 
                   for CDMA and FM. NV Items introduced w/RF_HAS_RFR3100 mainlined.   
09/14/01    dc     Moved rf_set_pa_level() to dfm.h
09/l2/01   dbc     Merged prototype for gps function rf_get_lna_state() 
07/02/01   aks     Exported the externalized functions and removed internal-only
                   functions.
04/10/01   tn      merged the latest gps changes from ppg.
03/08/01   tn      Add prototype for rf_get_pa_warmup in FEATURE_SUBPCG_PA_WARMUP_DELAY.
10/18/00   fas     Merged from PPG.
10/06/00   dsb     Mainlined up to and including T_MSM31.
09/07/00   jcw     Merged in prototype for tx_gain_adj_step_size().
08/25/00   dsb     Added RF_BAND_GPS.  Removed rf_enable/disable_gps.
07/24/00   day     Added support for "tune away without drop" capability for
                   FEATURE_GPSONE.
06/22/00   dd      Ported from PP 3020 - merged gps function prototypes.
05/04/00   ymc     Renamed FEATURE_PA_RANGE_DIG_COMP as 
                   FEATURE_PA_RANGE_TEMP_FREQ_COMP.
04/14/00   ymc     Removed two function prototypes:
                   rf_pa_freq_comp_calc and rf_pa_temp_comp_calc.
04/03/00   va      Added prototype for rf_digital_band_class_select()
03/22/00   ymc     Added the following function prototypes:
                   rf_init_pa_ctl_1_txlinz, rf_init_pa_ctl_4_txlinz,
                   rf_disable_pa_ctl, rf_enable_pa_ctl, rf_set_fm_pa_state, 
                   rf_load_pa_thres, rf_reset_pa_thres, rf_adjust_pa_thres, 
                   rf_set_pa_offsets, rf_pa_freq_comp_calc, and rf_pa_temp_comp_calc.
                   Some are featurized under FEATURE_PA_RANGE_DIG_COMP and 
                   FEATURE_4PAGE_TX_LINEARIZER.
03/01/00   ymc     Added the following function prototypes:
                   rf_is_lna1_on, rf_is_lna2_on,
                   rf_init_sw_lna_ctl, rf_sw_lna_setup_digital,
                   rf_sw_lna_setup_analog, rf_enable_sw_lna_ctl, rf_disable_sw_lna_ctl,
                   rf_init_hw_lna_ctl, rf_hw_lna_setup_digital, 
                   rf_hw_lna_setup_analog, rf_enable_hw_lna_ctl, rf_disable_hw_lna_ctl,
                   rf_update_lna_offsets.
12/17/99   rv      Renamed RF-CDMA_BAND and RF_PCS_BAND to RF_BAND_CDMA and 
                   RF_BAND_PCS to avoid conflict with enum rf_card_band_type defn.
12/08/99   rv      Merged in Trimode support.
11/03/99   ymc     Integrated functions rf_set_frq_trk_acc, rf_get_frq_trk_acc
01/06/99   ychan   Modified for targets NGP, SURF and Q-1900.
01/06/99   ychan   Updated Copyright date.
07/10/98   kss     Added RF_WARMUP_TIME_MS.
06/25/98   kss     Added external declaration for rf_complete_warmup().
05/14/98   bgc     Added RF_CODEC_RF Mask to rf_codec_on() enum..
12/08/97   thh     Added external declaration for rf_enable_cdma_temp_comp().
09/26/97   thh     Removed RF NV external declaration.  This and other changes
                   to support the new interface from the RF driver breakup.
07/17/97   thh     Added extern declaration for rf_tx_start() and rf_tx_stop().
06/17/97   dhh     Deleted target conditions #if (TG==T_G) ... and added
                   T_Q for Q Phone.
03/14/97   jjn     Added (TG==T_MD) where ever appropriate for the Module
01/10/97   fkm     Added RF_CODEC_HS Mask to rf_codec_on() enum.
12/10/96   thh     Added support for TGP by defining T_T.
09/25/96   dak     Added T_I2 to some ifdefs to support the updated RF driver
                   for ISS2.
07/16/96   thh     Add extern declaration for rf_cdma_temp_comp_init() and
                   rf_cdma_do_temp_comp().
12/22/95   jah     Changed rf_get_rssi() comments to be generic
10/25/95   jah     Changed rf_init_delayed() to return mc_name_type.
                   Added rf_post_cdma_reacq() for drooping Rx AGC bug.
06/28/95   jca     Added RF_DFM_FADE_OR_SCAN to rf_dfm_mode_type.
05/23/95   tst     Added rf_init_delayed().
05/16/95   tst     Added RF_CODEC_MC.
05/12/95   tst     Removed conditional compile for rf_get_rssi().  Now all
                   targets use "word" for *raw_rssi.
05/04/95   tst     Fixed comments for rf_get_cdma_tx_pwr_limit
05/03/95   tst     Fixed comments for rf_get_cdma_rx_agc() and
                   rf_get_cdma_tx_agc().
04/17/95   tst     Changed RF_TUNE_CHAN_WAIT to 40ms for Gemini and ISS2.
04/11/95   tst     Added rf_codec_type and associated constants for use in
                   modified rf_codec_on() "caller ID" plan.
03/29/95   tst     Changed #if logic for *raw_rssi byte/word definition.
03/27/95   tst     Changed rf_get_rssi() *raw_rssi from byte to word.
03/21/95   tst     Added function rf_set_dac().
03/02/95   tst     Added functions rf_get_cdma_tx_agc() and
                   rf_get_cdma_tx_pwr_limit().
12/15/94   tst     Fixed comment and if statement for rf_ratchet_mode() to
                   include ISS1.
12/01/94   tst     Fixed comment for rf_get_rssi() scaled_rssi.
11/28/94   tst     Incorporated comments from readiness review.
11/16/94   tst     Added target.h include file and conditional compile around
                   MSM2P-only functions.
11/15/94   tst     Added rf_get_cdma_rx_agc().
11/09/94   tst     Added rf_set_dfm_mode() and enums for mode definition.
11/08/94   tst     Added rf_stay_asleep() to actually set SLEEP/ low.
                   Updated dependencies and side effects for functions
                   which depend on and modify data base items.
                   Added rf_codec_on().
11/03/94   tst     Added RF_CODEC_WAIT_TIME.  Corrected comments for
                   RF_PA_WAIT, RF_PA_LEVEL_WAIT, and RF_ENABLE_PWR_WAIT.
03/16/94   jca     Changed interface to function rf_get_rssi().
12/09/93   jah     Changed WAIT times to account for tick granularity.
07/01/93   jca     Added function rf_ratchet_mode for Beta 2 target.
06/11/93   jca     Added new functions to support powering on RF hardware
                   and then delaying before programming synthesizers.
05/13/93   jca     Removed rf_mode parameters from rf_tx_enable().
05/10/93   jca     Added missing include.  Changed name of procedure
                   rf_put_nv_item() to rf_read_nv_item().
03/26/93   jca     Added rf_sleep() procedure.
02/16/93   jca     Added rf_put_nv_item() procedure.
01/14/93   arh     Removed rf_shutdown() procedure.
12/28/92   jca     Added new functions to support portable operation.
07/22/92   arh     Creation

===========================================================================*/
#include "comdef.h"
#include "rficap.h"
#include "rfcom.h"
#include "rfi.h"
                
#ifdef FEATURE_CDMA1X
#include "sys.h"
#include "rfc.h"
#include "nv.h"
#include "clk.h"
#include "target.h"
#include "mc.h"
#include "rxf.h"
#include "enc.h" 
#include "rf1x.h"
#include "rflib_share.h"
#endif //FEATURE_CDMA1X

#ifdef FEATURE_WCDMA
#error code not present
#endif //FEATURE_WCDMA

/* EJECT  */

/*===========================================================================

                       CONSTANTS AND MACROS

===========================================================================*/
/* ENUM definition for PA Offset on HDR RevA*/
 
#ifdef RF_HAS_HDR
#error code not present
#endif

/* Maximum TX power that UE is capable to transmit based on UE power class  in db12 */
#define RF_UE_MAX_TX_POWER_DB12  972
/* Maximum TX power that UE is capable to transmit based on UE power class  in dbm */
#define RF_UE_MAX_TX_POWER       24
#ifdef FEATURE_WCDMA_MIN_INIT_TX_PWR
/* Minimum TX power -52 dBm UE is capable to transmit in db12 */
#define RF_UE_MIN_TX_POWER_IN_DB12      -452
#endif

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#endif //FEATURE_WCDMA

/* The RF HDR AGC acqusition mode */
typedef enum
{
  RF_HDR_NORMAL_AGC_ACQ_MODE,
    /* "Normal AGC acquistion" mode */

  RF_HDR_FAST_AGC_ACQ_MODE,
    /* "Fast AGC acquistion" mode */

  RF_HDR_RESUME_AGC_ACQ_MODE
    /* "Resume AGC acquistion" mode */

} rf_hdr_agc_acq_mode_enum_type;


/* The RF AGC Information Captured when stopping the AGC */
typedef struct
{
  int16 last_agc;
    /* Last good AGC value */

  int16 min_agc;
    /* Lowest AGC value since AGC tracking was started */

  int16 max_agc;
    /* Largest AGC value since AGC tracking was started */

} rf_hdr_agc_info_struct_type;

#ifdef RF_HAS_MDSP_TX_AGC
/* SW has to set the flag for the newly loaded double 
  buffered parameters to be picked up by MDSP at the 
  slot or Frame Boundary.These flags needs to be set
  after writing the approprate vales in MDSP Variables

  TX_LIN_PARAMS is applicable for Parameters related to
  Linearizer Table

  TX_LMIT_PARAMS is applicable for parameters related 
  to total Max and Min TX Power

  PA_THRESH_PARAMS is applicable for loading PA thresholds
  in MDSP 
 */
   
typedef enum
{
  TX_LIN_PARAMS,
  TX_LIMIT_PARAMS,   
  PA_THRESH_PARAMS

} rf_mdsp_tx_double_buff_params_type;

/* Determines whether to latch the double buffered parameter
   at slot or Frame Boundary */

typedef enum
{
  SLOT_LATCH,
  FRAME_LATCH
} rf_mdsp_tx_double_buff_latch_type;
#endif

/*===========================================================================

                              FUNCTION DECLARATIONS

===========================================================================*/

#ifdef RF_HAS_FEATURE_CDMA1X


#ifdef RF_HAS_MDSP_TX_AGC
/*===========================================================================

FUNCTION RF_MDSP_GET_TX_DOUBLE_BUFF_WRITE_STATUS        EXTERNALIZED FUNCTION

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
                                                rf_mdsp_tx_double_buff_latch_type latch);

/*===========================================================================

FUNCTION RF_MDSP_TX_SET_PORT_REG_ADDRESS                                       

DESCRIPTION
 This function is used to set the SSBI ports as well as SSBI register addresses
 at which MDSP sends TX AGC updates in the 1X mode

DEPENDENCIES
MDSP clocks must be on when this function is called

RETURN VALUE
  None

SIDE EFFECTS
None
===========================================================================*/
void rf_mdsp_tx_set_port_reg_address(void);

#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */
#endif /* RF_HAS_MDSP_TX_AGC */

/*===========================================================================

FUNCTION RF_SLEEP_CDMA1X

DESCRIPTION
  This function sets the database item RF_MODE to DB_RF_SLEEP and configures
  the RF Rx circuits to save power.  Actual control of the SLEEP/
  signal is done by rf_stay_asleep().  This function should be called when
  the sleep state is entered so the database will be updated.  The SLEEP/
  signal control is not done here because SLEEP/ must remain high until the
  MSM is done with CHIPX8.

DEPENDENCIES
  This routine is only called during CDMA operation.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void rf_sleep_cdma1x( rfcom_device_enum_type device );

/*===========================================================================

FUNCTION RF_STAY_ASLEEP_CDMA1X                                

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
extern void rf_stay_asleep_cdma1x( rfcom_device_enum_type device );  /* external */
extern void rf_set_stay_asleep( rfcom_device_enum_type device, boolean tcxo_pdm_on ); /* internal */


/*===========================================================================

FUNCTION RF_TUNE_TO_CHAN_CDMA1X

DESCRIPTION
  This function tunes the synthesizer to the specified channel.  Once
  rf_chan_tune has been called, a delay of RF_CHAN_TUNE_WAIT msecs must
  be performed by the application to allow the synthesizer to settle.
  The channel can be an analog channel number, a CDMA channel number or
  a PCN channel number.  The channel is interpreted according to
  the current mode of operation.

DEPENDENCIES
  database item RF_MODE must be setup prior to call

RETURN VALUE
  None

SIDE EFFECTS
  The database item (DB_CHANNEL) is updated to the new channel

===========================================================================*/
extern void rf_tune_to_chan_cdma1x
(
  rfcom_device_enum_type device,        /* RF Multimode device */
  word chan          /* Channel number to tune to                          */
);

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
rf_cagc_dynamic_range_type rf_get_cagc_linearizer_range( void );

/*===========================================================================

FUNCTION RF_GET_PATH_CDMA_RX_AGC_IN_DBM                     EXTERNAL FUNCTION

DESCRIPTION
  This function returns the CDMA Rx RSSI in dBm for a specific Rx device.  
  Resolution is .1 dB

DEPENDENCIES
  None

RETURN VALUE
  Signed Rx power in dBm

SIDE EFFECTS
  None

===========================================================================*/
extern int16 rf_get_path_cdma_rx_agc_in_dBm( rfcom_device_enum_type device );


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
extern int2 rf_get_min_tx_pwr_in_dB( void );


/*===========================================================================

FUNCTION RF_GET_TX_OPEN_LOOP                               EXTERNAL FUNCTION

DESCRIPTION
  Returns the 10 bit sign extended TX Open Loop Accumulator value
  
DEPENDENCIES
  Should be called within an MSM_LOCK(), MSM_UNLOCK() pair
  
RETURN VALUE
  int16

SIDE EFFECTS
  None

===========================================================================*/
extern int16 rf_get_tx_open_loop( void );

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
extern int16 rf_get_tx_gain_adjust( void );


/*===========================================================================

FUNCTION RF_INIT_CAGC                                       EXTERNAL FUNCTION

DESCRIPTION
  Performs one-time initialization of all CAGC registers (both TX and RX)
  
DEPENDENCIES
  Any actions performed by this function or calls within this function must
  not be dependent upon non-default NV items, i.e., items read from NV

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rf_init_cagc( rfcom_device_enum_type device );


/*===========================================================================
                                                            EXTERNAL FUNCTION
FUNCTION RF_SET_AGC_DC_LOOP_GAIN

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
extern void rf_set_agc_dc_loop_gain(rf_path_enum_type path, agc_dc_loop_gain_type action );

/* EJECT  */
/*===========================================================================

FUNCTION RF_GET_RSSI

DESCRIPTION
  This function returns the signal strength for the current channel.

DEPENDENCIES
  None

RETURN VALUE
  raw_rssi    - relative RSSI value

  scaled_rssi - calibrated RSSI value scaled 0..255 over the operating range

SIDE EFFECTS
  None

===========================================================================*/
extern void rf_get_rssi
(
  word *raw_rssi,
    /* raw RSSI value */
  word *scaled_rssi
    /* Calibrated RSSI value */
);

/* EJECT  */

/*===========================================================================

FUNCTION RF_INIT_CDMA1X                                     EXTERNAL FUNCTION

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
extern void rf_init_cdma1x( rfcom_device_enum_type device );

/* EJECT  */

/*===========================================================================

FUNCTION RF_INIT_DELAYED

DESCRIPTION
  Gemini only.  Finished initializing the RF unit. This procedure should be
  called after all RF NV items have been loaded.  The database item DB_RF_MODE
  is updated to the current hardware configuration found (e.g., if PCN board
  is found, then the database item DB_RF_MODE is updated to DB_RF_PCN).

  For Gemini, initialization is as follows:
      rf_init_cdma1x()
      load all RF NV items
      rf_init_delayed()

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The database item DB_RF_MODE will be updated to DB_RF_PCN or DB_RF_CDMA
  depending on the current hardware configuration.

===========================================================================*/
#if defined(RF_HAS_MULTIMODE_ARCH)
extern rf_init_status_type rf_init_delayed( rfcom_device_enum_type );
#else
#error code not present
#endif /* RF_HAS_MULTIMODE_API */

/*===========================================================================

FUNCTION RF_IS_SLEEPING_CDMA1X

DESCRIPTION    Queries whether the RF is sleeping right now.

DEPENDENCIES   None.

RETURN VALUE   True if RF is in sleep state.

SIDE EFFECTS   None.

==========================================================================*/
extern boolean rf_is_sleeping_cdma1x ( rfcom_device_enum_type device );
/* EJECT  */

/*===========================================================================

FUNCTION RF_INIT_DIGITAL

DESCRIPTION
  Programs the RF RX synthesizers unit for digital mode of operation
  (CDMA or PCN).  This procedure should be called every time digital
  mode is entered.

DEPENDENCIES
  database item RF_MODE must be set
  The MSM1 core AGC hardware should have been disabled first
  Procedure rf_enable_digital_rx_pwr() must have been called before calling
  this procedure.

RETURN VALUE
  None

SIDE EFFECTS
  The database item DB_RF_MODE is set to DB_RF_PCN or DB_RF_CDMA
  depending on the current hardware configuration.

===========================================================================*/
extern void rf_init_digital(rfcom_device_enum_type device );

/* EJECT  */

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
  Called after wakeup and when the real time counter is running.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void rf_complete_warmup( void );

/* EJECT  */
/*===========================================================================

FUNCTION RF_TX_ENABLE

DESCRIPTION
  This function programs the VHF transmit synthesizer for analog
  or digital (CDMA or PCN) mode of operation.

  Once rf_tx_enable has been called, a delay of RF_TUNE_CHAN_WAIT
  milliseconds must be performed by the application to allow the
  VHF transmit synthesizer to settle.  After the VHF transmit sythesizer
  has settled, only then should the PA be enabled.

DEPENDENCIES
  database item RF_MODE must be set
  Procedure rf_enable_tx_pwr() must have been called before calling
  this procedure.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void rf_tx_enable( rfcom_device_enum_type device );

#if defined(RF_HAS_PA_ACCESS_PROBE_CONFIG)
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
extern void rf_set_pa_access_probe_config(rfcom_device_enum_type device ,boolean is_traffic_state);

#ifdef RF_HAS_HDR
#error code not present
#endif
#endif

/* EJECT  */
/*===========================================================================

FUNCTION RF_TX_DISABLE

DESCRIPTION
  This function turns off the VHF transmit synthesizer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void rf_tx_disable(rfcom_device_enum_type device );

/*===========================================================================

FUNCTION RF_ENABLE_DIGITAL_RX_PWR

DESCRIPTION
  This function turns on the digital receive portion of the RF hardware.

  Once rf_enable_digital_rx_pwr() has been called, a delay of
  RF_ENABLE_PWR_WAIT milliseconds must be performed by the
  application to allow the RF hardware to settle.  After the
  RF hardware has settled, only then should the rf_init_digital()
  function be called.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void rf_enable_digital_rx_pwr( rfcom_device_enum_type device );

/* EJECT  */
/*===========================================================================

FUNCTION RF_ENABLE_TX_PWR

DESCRIPTION
  This function turns on the transmit portion of the RF hardware.

  Once rf_enable_tx_pwr() has been called, a delay of RF_ENABLE_PWR_WAIT
  milliseconds must be performed by the application to allow the RF
  hardware to settle.  After the RF hardware has settled, only then should
  the rf_tx_enable() function be called.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void rf_enable_tx_pwr( rfcom_device_enum_type device );
/* EJECT  */
/*===========================================================================

FUNCTION RF_GET_CDMA_RX_AGC

DESCRIPTION
  This function returns the CDMA receive AGC setting in 8 bit two's
  complement format.

  WITH Rx LINEARIZER SETUP FOR -106dBm..-20.75dBm, the Rx signal strength is:

    Rx strength in dBm = -63.248 + (( rf_get_cdma_rx_agc() * 334 ) / 1000 )

    That's -0.334dB per count referenced to -63.248dBm.  Since this returns
    int1, this is a numeric Rx signal strength from -106dBm to -20.83dBm.

DEPENDENCIES
  None

RETURN VALUE
  int1 - 8 bit two's complement CDMA Rx AGC value

SIDE EFFECTS
  None

===========================================================================*/
extern int1 rf_get_cdma_rx_agc( void );

/* EJECT  */
/*===========================================================================

FUNCTION RF_GET_CDMA_TX_AGC

DESCRIPTION
  This function returns the CDMA Tx AGC setting in 10 bit binary
  format.

  WITH Tx LINEARIZER SETUP FOR -52.25dBm..+33dBm, the Tx signal strength is:

    Tx strength in dBm = -52.25 + (( rf_get_cdma_rx_agc() * 0833 ) / 10000 )

    That's +0.085dB per count referenced to -52.25dBm.  Since this returns
    word, this is a numeric Tx signal strength from -52.25dBm to +32.97dBm.

DEPENDENCIES
  None

RETURN VALUE
  word = 10 bit Tx AGC value

SIDE EFFECTS
  None

===========================================================================*/
extern word rf_get_cdma_tx_agc( void );

/* EJECT  */
/*===========================================================================

FUNCTION RF_READ_HDET                                   EXTERNALIZED FUNCTION

DESCRIPTION
  This function is responsible for reading the target-dependent HDET
  channel.  Targets without integrated HDET ADCs will use the HKADC
  channel specified in the RF card file.  Targets will integrated HDET
  ADCs will use the dedicated converter in the RFT/RTR.

DEPENDENCIES
  None

RETURN VALUE
  HDET reading

SIDE EFFECTS
  None

===========================================================================*/
#ifdef RF_HAS_HDET_UINT16
extern uint16 rf_read_hdet( rf_card_band_type band );
#else
extern uint8 rf_read_hdet( rf_card_band_type band );
#endif
/* EJECT  */
/*===========================================================================

FUNCTION RF_GET_CDMA_TX_PWR_LIMIT

DESCRIPTION
  This function returns the CDMA Tx power limit setting in 10 bit binary
  format.  Since the hardware register is not directly readable, this
  functions returns a copy of the last setting performed by the RF unit.
  The returned value is interpreted as follows:

    Returned value               0........1023
    Tx power limit          -52.25........+31dBm
    Tx power limit register   +511........-512

DEPENDENCIES
  None

RETURN VALUE
  word = 10 bit Tx power limit value

SIDE EFFECTS
  None

===========================================================================*/
extern word rf_get_cdma_tx_pwr_limit( void );

/* EJECT  */
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
extern void rf_post_cdma_reacq( rfcom_device_enum_type device );


/*===========================================================================

FUNCTION RF_CDMA_TEMP_COMP_INIT                             INTERNAL FUNCTION

DESCRIPTION
  This function simply saves the address of the timer block.  This address
  is passed by a task.  There is an understanding between this task and
  us (RF driver) that when this timer block expires, the task will call
  the function rf_cdma_temp_comp_start so that the RF driver may start the
  process of adjusting the linearizers based on the temperature.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updates temp_cal_timer_blk

===========================================================================*/
extern void rf_cdma_temp_comp_init
(
  rex_timer_type *timer_ptr    /* The timer block to control call backs */
);


/*===========================================================================

FUNCTION RF_CDMA_DO_TEMP_COMP                               INTERNAL FUNCTION

DESCRIPTION
  This function does 2 things.  The first thing it does is to reset the
  timer so that it will get called again.  Then it will start the process
  of updating the temperature and adjusting the linearizers.  This process
  can only be stopped by clearing the timer with the function rex_clr_timer().

DEPENDENCIES
  This function must be called from a task, so that it remains interruptable.
  temp_comp_timer_ptr must have been initialized and setup.

RETURN VALUE
  None

SIDE EFFECTS
  This function will set the timer, so that in will get executed again.

===========================================================================*/
extern void rf_cdma_do_temp_comp( rfcom_device_enum_type device );


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
  None

===========================================================================*/
extern void rf_tx_start(rfcom_device_enum_type device);


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
extern void rf_tx_stop(rfcom_device_enum_type device);



/*===========================================================================

FUNCTION RF_DIGITAL_BAND_SELECT                         EXTERNALIZED FUNCTION

DESCRIPTION
  Initialises the rf card for the current band(CDMA/PCS). This function has
  has to be called each time whenever there is a band change. (from PCS to 
  Cellular or vice versa).
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  Updates the data base and rf_curr_cdma_band with the current
  band setting.

===========================================================================*/
extern void rf_digital_band_select
(
  rfcom_device_enum_type device, 
 byte rf_band
);


/*============================================================================

FUNCTION RF_INIT_HW_LNA_CTL                                           INTERNAL

DESCRIPTION
  This function will initialize and configure LNA bypass control
in MSM3100 mode.  2 LNAs are being used.  LNA attenuation is disabled.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

//void rf_init_hw_lna_ctl( void );



/*============================================================================

FUNCTION RF_HW_LNA_SETUP_DIGITAL                                     INTERNAL

DESCRIPTION
  This function will configure LNA to operate in digital mode after the phone
wakes up from sleep.

DEPENDENCIES
  rf_init_hw_lna_ctl must have been called once.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void rf_hw_lna_setup_digital( rfcom_device_enum_type device );

/*============================================================================

FUNCTION RF_ENABLE_HW_LNA_CTL                                         INTERNAL

DESCRIPTION
  This function will enable hardware LNA control and unfreeze LNA decisions.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void rf_enable_hw_lna_ctl(rfcom_device_enum_type device, int4 unused );


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

void rf_update_lna_offsets( rfcom_device_enum_type device);


/*============================================================================

FUNCTION RF_GET_LNA_GAIN_STATE                                       INTERNAL

DESCRIPTION
  This function will return an enum representing the current LNA state.

DEPENDENCIES
  None

RETURN VALUE
  lna_gain_state_type

SIDE EFFECTS
  None

=============================================================================*/

lna_gain_state_type rf_get_lna_gain_state( void );


/*============================================================================

FUNCTION RF_GET_PA_STATE                                       EXTERNAL

DESCRIPTION
  This function will return an byte representing the two bits for PA state.

DEPENDENCIES
  None

RETURN VALUE
  pa state

SIDE EFFECTS
  None

=============================================================================*/

extern byte rf_get_pa_state( void );

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

byte rf_get_ratchet_state( void );

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

void rf_disable_pa_ctl( rfcom_device_enum_type device);


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

void rf_enable_pa_ctl( rfcom_device_enum_type device);

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
void rf_set_pa_offsets( rfcom_device_enum_type device );

#ifdef RF_HAS_GPS
#error code not present
#endif /* RF_HAS_GPS */


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
extern void rf_tx_gain_adj_step_size( byte step_size );

#ifdef FEATURE_SUBPCG_PA_WARMUP_DELAY
/*===========================================================================

FUNCTION RF_GET_PA_WARMUP                      EXTERNALIZED FUNCTION

DESCRIPTION    This function returns the new calculated PA WARMUP value
               based on the NV item in chips units.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern word rf_get_pa_warmup (void);

#ifdef RF_HAS_DIFF_PA_TX_WARMUP
/*===========================================================================

FUNCTION RF_GET_TX_WARMUP                               EXTERNALIZED FUNCTION

DESCRIPTION    This function returns the new calculated TX WARMUP value
               based on the NV item in chips units.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern word rf_get_tx_warmup (void);
#endif

#endif /* FEATURE_SUBPCG_PA_WARMUP_DELAY */


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
extern int1 rf_get_tx_gain_adj( void ); 

#ifdef FEATURE_ZIF_LOGGING
/*===========================================================================

FUNCTION RF_LOG_EXTENDED_AGC

DESCRIPTION    This function is invoked to log extended AGC values for DIAG.

DEPENDENCIES   rf_ext_agc_vars

RETURN VALUE   None.

SIDE EFFECTS   Messages can be sent to DIAG task.

===========================================================================*/
void  rf_log_ext_agc ( rfcom_device_enum_type device);

/*===========================================================================

FUNCTION RF_LOG_TX_AGC

DESCRIPTION    This function is invoked to log TX AGC values for DIAG.

DEPENDENCIES   rf_tx_agc_vars

RETURN VALUE   None.

SIDE EFFECTS   Messages can be sent to DIAG task.

===========================================================================*/
void  rf_log_tx_agc ( void );

/*===========================================================================

FUNCTION RF_LOG_IQ_OFFSETS

DESCRIPTION    This function is invoked to log I/Q Offset values for DIAG.

DEPENDENCIES   rf_iq_offsets_vars

RETURN VALUE   None.

SIDE EFFECTS   Messages can be sent to DIAG task.

===========================================================================*/
void  rf_log_iq_offsets ( void );

/*===========================================================================

FUNCTION RF_LOG_DACC_IQ_ACCUM

DESCRIPTION    This function is invoked to log DACC I/Q Accumulator values
               for DIAG.

DEPENDENCIES   rf_dacc_iq_accum_vars

RETURN VALUE   None.

SIDE EFFECTS   Messages can be sent to DIAG task.

===========================================================================*/
void  rf_log_dacc_iq_accum ( void );

#endif /* FEATURE_ZIF_LOGGING */
/* EJECT  */
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
extern word rf_tune_band_chan
(
    rfcom_device_enum_type device,        /* RF Multimode device */
    rf_card_band_type band,
    word chan
);
/*===========================================================================

FUNCTION RF_GET_BAND_CHAN                                EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns the current settings of the RF driver internal
  variables rf_curr_cdma_band and rf_chan 

DEPENDENCIES
  Should not be called until after digital mode is entered for the first 
  time.

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern void rf_get_band_chan
(
    rf_card_band_type *band, 
    word *chan
);

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
);
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
extern void rf_notify_sch_data_rate(rfcom_device_enum_type ,boolean , enc_sch_rate_type, boolean );
#endif  /* FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST */

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

RETURN VALUE
  uint16 representing the usec wait value for RxAGC settling.  Value of 0
         represents AGC needs to be started by calling rf_complete_sleep_warmup().
SIDE EFFECTS
  Updates RF local variable rf_mode and database item DB_RF_MODE to 
  either DB_RF_CDMA or DB_RF_PCN
  RF local variable rf_chan and database item DB_RF_CHAN may be updated if
  there is a channel change
    
===========================================================================*/
extern uint16 rf_init_digital_band_chan
(
    rfcom_device_enum_type device,
    rf_card_band_type band,
    word chan
);
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
extern void rf_set_mdsp_cdma_spectral_inversion( void );

/*===========================================================================

FUNCTION RF_GET_CDMA_TX_AGC_IN_DBM                     EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns the CDMA TX AGC in dBm.  Resolution is .1 dB

DEPENDENCIES
  None

RETURN VALUE
  Signed Rx power in dBm

SIDE EFFECTS
  None

===========================================================================*/
extern int16 rf_get_cdma_tx_agc_in_dBm( void );


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
extern void rf_1x_inact(rfcom_device_enum_type device);


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
extern boolean rf_enable_diversity( rfcom_device_enum_type device );



/*===========================================================================

FUNCTION RF_DISABLE_DIVERSITY                           EXTERNALIZED FUNCTION

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean rf_disable_diversity( rfcom_device_enum_type device );

#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */


#ifdef FEATURE_TX_POWER_BACK_OFF
/*===========================================================================

FUNCTION RF_UPDATE_TX_PWR_LIMIT                            EXTERNAL FUNCTION

DESCRIPTION
  API function used by MUX1X to notify RF that the reverse link channel
  configuration is changing
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  TX Power limit may be updated

===========================================================================*/
void rf_update_tx_pwr_limit( rf_txc_chan_config_type* );



/*===========================================================================

FUNCTION RF_GET_TX_PWR_LIMIT_ADJ                            EXTERNAL FUNCTION

DESCRIPTION
  API function used by MUX1X to query what the TX power limit (and amount
  of adjustment due to PA Backoff) would be for a particular reverse link
  channel configuration
  
DEPENDENCIES
  None

RETURN VALUE
  rf_txc_pwr_limit_type

SIDE EFFECTS
  None

===========================================================================*/
void rf_get_tx_pwr_limit_adj( rf_txc_chan_config_type*,
                              rf_txc_pwr_limit_type*    );



/*===========================================================================

FUNCTION RF_RESET_PA_BACKOFF_MODULE                        EXTERNAL FUNCTION

DESCRIPTION
  API function used to reset the PA Backoff module
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  TX power limit may be adjusted

===========================================================================*/
void rf_reset_pa_backoff_module( uint16 rf_reset_acc_val );

#endif /* FEATURE_TX_POWER_BACK_OFF */

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
extern uint8 rf_get_nv_band( rfcom_device_enum_type device );

/*===========================================================================

FUNCTION RF_GET_SUB_CLASS                                    EXTERNAL FUNCTION

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
uint32 rf_get_sub_class( rf_card_band_type band );

/*===========================================================================

FUNCTION RF_IS_TUNABLE_ON_ANTENNA                           EXTERNAL FUNCTION

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
boolean rf_is_tunable_on_antenna( int antenna_index, rf_card_band_type band );

/*===========================================================================

FUNCTION RF_GET_ANTENNA_DIFF                               EXTERNAL FUNCTION

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
);
/*===========================================================================

FUNCTION RF_GET_NUM_ANTENNA                               EXTERNAL FUNCTION

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
uint8 rf_get_num_antenna(void);

#ifdef RF_HAS_NO_RTC_DEPENDENCY
/*===========================================================================

FUNCTION RF_BYPASS_RTC_CTL_PATH_ON_OFF

DESCRIPTION
  Control to enable or disable the Bypass of RTC Synch on a
  specific rx path. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rf_bypass_rtc_ctl_path_on_off( rfcom_device_enum_type device, boolean rf_bypass_rtc_on_off );

#endif /* RF_HAS_NO_RTC_DEPENDENCY */

/* EJECT  */
/*=========================================================================

FUNCTION RF_GET_RX_CARRIER_FREQ

DESCRIPTION
  This function returns the Rx carrier frequency in KHz for the band and channel
  specified in the parameters.  If the band or channel is not supported 
  then 0 is returned.

DEPENDENCIES
  None
   

RETURN VALUE
  Rx carrier frequency for valid band and channel.  

SIDE EFFECTS
  None.

===========================================================================*/
uint32 rf_get_rx_carrier_freq( rf_card_band_type band, word chan );
/* EJECT  */
/*=========================================================================

FUNCTION RFLIB_GET_TX_CARRIER_FREQ

DESCRIPTION
  This function returns the Tx carrier frequency in KHz for the band and channel
  specified in the parameters.  If the band or channel is not supported 
  then 0 is returned.

DEPENDENCIES
  None
   

RETURN VALUE
  Tx carrier frequency in KHz for valid band and channel.  

SIDE EFFECTS
  None.

===========================================================================*/
uint32 rf_get_tx_carrier_freq( rf_card_band_type band, word chan );

/* EJECT  */
/*=========================================================================

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
);


/* EJECT  */
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
extern void rf_complete_sleep_warmup( rfcom_device_enum_type device );
/* EJECT  */
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
extern int1 rf_get_path_cdma_rx_agc( rfcom_device_enum_type device );
/* EJECT  */

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
extern boolean rf_has_high_sinr( void );
/* EJECT  */

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
extern lna_gain_state_type rf_get_lna_gain_state( void );
#ifdef RF_HAS_HDR
#error code not present
#endif /*RF_HAS_HDR*/
/*===========================================================================

FUNCTION RF_GET_SUPPORTED_HW                           INTERNAL FUNCTION

DESCRIPTION
  Extracts and returns the ID of the RF Card, from the NV Item 
  NV_RF_HW_BC_CONFIG_I.

DEPENDENCIES
  None

RETURN VALUE
  rf_hw_type value containing the Id of the RF card being used with this target.

SIDE EFFECTS
  None
===========================================================================*/
extern rf_hw_type rf_get_supported_hw(void);

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

extern lna_gain_state_type rf_get_path_lna_gain_state( rfcom_device_enum_type device );

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
extern void rf_freeze_cagc ( void );

/*===========================================================================

FUNCTION RFM_UNFREEZE_CAGC                                 INTERNAL FUNCTION

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
void rf_unfreeze_cagc ( void );

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
void rf_tx_shutdown(void);

/*======================== O B S O L E T E  F U N C T I O N S =============*/

#ifdef RF_HAS_OBSOLETE_FUNCTIONS
#error code not present
#endif /* RF_HAS_OBSOLETE_FUNCTIONS */

#endif //RF_HAS_FEATURE_CDMA1X






#ifdef RF_HAS_FEATURE_WCDMA
#error code not present
#endif /* RF_HAS_FEATURE_WCDMA */

#if defined (RF_HAS_FEATURE_WCDMA) || defined (RF_HAS_FEATURE_GSM)
#error code not present
#endif /* RF_HAS_FEATURE_WCDMA || RF_HAS_FEATURE_GSM */

#ifdef RF_HAS_FEATURE_WCDMA
#error code not present
#endif /* RF_HAS_FEATURE_WCDMA */

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
void rf_device_rf_cal_mode_is_enabled(boolean is_enabled);

/*===========================================================================

FUNCTION RF_CURRENT_MODE_IS_RF_CAL                   INTERNAL FUNCTION

DESCRIPTION
This function returns TRUE or FALSE depending if we are in RF Cal Mode or not

DEPENDENCIES

RETURN VALUE
  Boolean

SIDE EFFECTS

===========================================================================*/
boolean rf_current_mode_is_rf_cal(void);
#endif /* RF_H */

