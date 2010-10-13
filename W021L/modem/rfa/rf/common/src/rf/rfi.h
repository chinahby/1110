#ifndef RFI_H
#define RFI_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                             RF Internal Header File

DESCRIPTION
  This header file provides the definitions and interfaces which are shared
  amongst modules internal to the RF Driver Unit.


Copyright (c) 1998,1999,2000 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2001,2002,2003 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2004,2005,2006 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2007,          by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*=================================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/rf/common/main/qsc1100/src/rf/rfi.h#3 $
  $DateTime: 2008/10/01 18:07:35 $
  $Author: vboda $

when       who     what, where, why
--------   ---     ----------------------------------------------------------------
09/30/08   vm/vb   Added Support for Band Class Sub Block Implementation
06/13/08   vm      Updates for 16 bit PDET for QSC1100
12/07/07   bmg     Changed the default RF card to RF_HW_DEFAULT if no other
                   value is specified.  This will avoid turning on VREGs or
                   doing other configuration if NV has not been loaded.
12/5/07    sar     Added default hw id for TRF6500.
11/27/07   rmd     Added the ability to set I & Q lines of either the 1X RTR 
                   or the GSM RTR to Hi-Z mode. This is a temporary change for the 
                   TRF6285.
11/05/07   ans     Moved RF_GET_TX_LIN_MIN and RF_GET_TX_LIN_MAX as they should be
                   outside library.
11/05/07   ans     Moved RF_GET_TX_LIN_MIN and RF_GET_TX_LIN_MAX to RFT file.
11/05/07   adk     Added prototype for rf_hdr_mdsp_init_tx_agc_settings()
11/05/07   ans     Added functions to limit Tx linearizer PDM values based on chipset.
10/30/07   ans     Moved enum definition pa_switchpoint_type from rf1x.h to rfi.h
10/26/07   ans     Added FEATURE_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST for backoffs
                   for R2 and R3 PA states.
10/11/07   rmd     Moved rf_pm_config_type definition from rf_pm.c to rfi.h so 
                   that it can be shared with 1x files. Added PM7540 Support.
10/05/07   ka      Added support for QRF7600.
10/01/07  avi   Set default HW for MM targets to QRF7600 instead of WDZRF
09/28/07   hkk     Default h/w set to TRF6500
09/27/07   jfc     Set default target to TRF6285 for GSM/1x builds
08/09/07   ad      Added support for reading HS LNA switch point NVs during HS calls.
08/07/07   ad      From 6260: Fix for retaining Temp Comp when UTRAN max pwr < NV max pwr.
07/22/07   dyc     Added rf_current_pa_r_map extern.
07/12/07   jfc     Moved rfi_gpio_gps_external_lna_on() prototype out from feature
                    RF_HAS_MDSP_TX_AGC
07/10/07   ra      Added rfi_gpio_gps_external_lna_on() prototype.
06/13/07   jfc     Added boolean param to rfi_tune_synth()
06/05/07   vm      Add more support for Beta Scaling on Sc2X
03/28/07   adk     rf_calc_new_tx_lin() is not needed if using FW TX AGC.
03/25/07   dyc     Remove RF_HAS_DEDICATED_RF_CARD implementation.
03/08/07   adk     Added prototype for rf_cdma_mdsp_init_tx_agc_settings()
02/13/07   jfc     Removed RFI_SET_TX_GENERAL_CTL() macro
02/13/07   hkk     Default HW reverted back to DZRF
02/08/07   vm/adk  Added support for firmware TX AGC
02/05/07   ycl     Merge to 7200 tip.
02/01/07   adk     Updated the definition of RF_DEF_HW
01/23/07   lcl     Added RTR6285 support.
11/21/06   jfc     Changed default HW and BC configs for multimode build
10/17/06   ycl     Modifications for initial MSM7600 build.
08/14/06   zg      Removed is_gps_rf_config_sgps().
01/18/06   sar     Eliminated states RF_STATE_GPS, RF_STATE_FM_RX and
                   RF_STATE_FM_RXTX for GPS fix.
01/16/06   rmd     Implemented BC11 and BC14 code review results. Including:
                   * Moving Band Class Translate code into rf_util_freq.c.
                   * Moving rf_get_valid_bc_and_mask() into rf_util_freq.c.
01/04/06   et      LINT fixes
12/21/05   dyc     Additional lint fixes.
12/14/05   sar     Eliminated lint errors.
11/07/05   rmd     Added initial support to BC11.
10/27/05   bmg     Added rf_error.h header for general internal error handling
10/21/05   rmd     Added support to BC14.
10/11/05   et      added path parameter to converted_rx_channel_list table
09/15/05   dyc     Added profiling struct to rf_chain_status.
07/19/05   grl     Removed TRK_LO from the SYSTEM_MODE mask since it is set
                   separately from the modulator.
05/09/05   ra      For DORA, added RF_HAS_MODEM_PA_CTL
04/28/05   kss     Remove PA range control bit from HDR system mode value.
03/08/05   dyc     Featurize rf_hdr_temp_update() under HDR.
02/28/05   et      changed i_lin_freq_comp, and i_tx_lim_vs_freq of 
                   rf_offsets_vs_freq_type to int2 instead of int1
02/20/05   dyc     Added rf_config_pa_dvs()
02/19/05   dyc     Added rf_hdr_temp_update()
01/13/05   dyc     Added rf_get_path_1x_agc()
01/05/05   ra      DORA merge
11/15/04   bmg     Removed un-executed calls to rf_load_agc_limits()
                   -- legacy function that was never used in MSM6500 and above.
10/31/04   dyc     Added rf_synth_time_iter to rf_chain_status.
10/28/04   ra      Added prototype for rf_get_rf_config()
10/14/04   bmg     Converted interpolation to the non-reference channel
                   offset_vs_freq structure.  Added power mode support.
08/29/04   dyc     Remove RF_HAS_IDLE_N usage.
08/16/04   dyc     Removed RF_HAS_FM component, rf_adjust_fm_pwr_level()
                   rf_fm_temp_update()
08/11/04   sar     Moved description of the rf_hw_type to this file and added rf_cards.h
                   in the include section.
07/13/04   jac     Added is_gps_rf_config_sgps() function prototype.
06/08/04   sar     Updated rf_offsets_vs_freqs_type structure to define last_chan_checked
                   last_band_checked and channel_list_converted_flag to arrays in order 
                   to allow secondary chain handling.
06/07/04   kss     Set SYSTEM_MODE defs to keep alt pdm control under 1x even
                   in HDR mode.
06/01/04   sar     Added rf_hw_config to rf chain status structure to aid in debugging.
04/20/04   sar     Added enumerator rf_hw_type.
05/21/04   dbc     Add definitions to support PA Backoff module
04/09/04   dyc     Provide defs for SYSTEM_MODE usage.
03/28/04   dyc     Added rf_get_path_cagc_val()
03/19/04   et      added interpolation (frequency comp) support
02/02/04   kss     Added prototypes for rf_hdr_enable_slot_isr() and 
                   rf_hdr_agc_update_max_power().
01/30/04   et      Removed usage of RF_HAS_TRIMODE.  Replaced by
                   FEATURE_TRIMODE_ITEMS.
10/14/03   dyc     Mainlined T_MSM6500
10/07/03   et      featurized synth_lock checking and added device param
                   to rf_chk_synth_lock()
10/01/03   dyc     Cleanup. Changed param variable name to be consistent with type. 
09/23/03   dyc     Updated RF_TX_MASTER_X_MAX for correct calc of index reference.
09/12/03   sar     Added device parameter to rf_load_cagc_linearizers().
09/08/03   kss     Added internal RF HDR interface prototypes.
09/05/03   dbc     Added two new fields to rf_chain_state_type to track the previous
                   RF state and CDMA band
09/03/03   dbc     Added rf_chain_state_type so that the state of the two RF chains
                   can be tracked independently
07/24/03   et      changed #include rxfi.h to rxf.h
07/23/03   bmg     Added device parameter to LNA override macros
07/11/03   sar     Added support for multipath chipset drivers.
07/10/03   bmg     Ported AGC override macros (some more)
06/16/03   dyc     added RF_GET_RX_AGC_MIN_VAL macro.
06/13/03   dyc     added RF_LATCH_TX_GAIN_CTL_RD macro.
04/14/03   et      merged with MSM6100 3030 build
04/07/03   et      updated RF_OVERRIDE_LNA_STATE_MACHINE macro for 6500
03/04/03   dbc     Changes to support PA Backoff
02/25/03   dbc     Removed unused defines
12/01/02   dbc     Added rf_cagc_is_pa_thres_adjusted() and
                   rf_cagc_get_curr_pa_thres_adj().  These are used when
                   FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST is defined.
11/01/02   sar     Added definition for rfi_band_type for multimode.
08/02/02   dbc     Removed rf_cagc_dynamic_range_type enum. Removed
                   rf_get_cagc_linearizer_range().  These have been moved to rf.h.
07/17/02   dyc     Removed PA_ON2 register definitions for MSM6050.  Added
                   declarations of new functions to support run time
                   detection/setting of the CAGC dynamic range
                   (merged from dbc rf.01.00.13)
05/31/02   zg      Added a GPS case in rf_synth_checker.
05/22/02   dbc     Added changes for CAGC 102 dB dynamic range.
05/20/02   dbc     Added CAGC_SET_AGC_LOOP_TIME_CONST_TO_IM(),
                   CAGC_SET_AGC_LOOP_TIME_CONST_TO_TRACKING() macros. Added
                   rf_load_im_level_ctls(), rf_set_lna_decision_to_low_gain_state(),
                   rf_disable_im_level_ctls(), rf_set_agc_val_to_low_gain(),
                   and rf_disable_im_level_ctls() prototypes.
03/22/02   dbc     Removed declaration of rf_set_rx_agc_mode()
03/17/02   dbc     Added declaration of rf_set_rx_agc_mode()
03/16/02   dbc     Added declaration of rf_init_tx_cagc_delayed() and
                   rf_init_rx_cagc_delayed()
03/08/02   zg      Added declaration of rf_restore_gps_agc & rf_restore_cdma_agc
03/08/02   dbc     Added declaration of rf_load_lna_switching_ctls()
02/22/02   dbc     Added declaration of rf_init_hw_lna_ctl().
02/13/02   dbc     Added ZIF logging support
01/22/02   dbc     PA_ON_STATUS masks updated to include differences between MSM6000
                   and MSM6050.
01/14/02   dbc     Added addititional masks (PA_ON2) for PA_ON_STATUS register.
                   RF_SYNTH_STATUS_READ macro uses inpw(...) sinc PA_ON_STATUS register
                   is a word. Moved CAGC debug features to rf.c.
12/20/01   dbc     Added rf_cagc_debug() function and clock callback structure used
                   when RF_CAGC_DEBUG flag is defined.
11/21/01   dbc     Added RF_LNA_STATE_MACHINE_NORMAL(), RF_OVERRIDE_LNA_STATE_MACHINE(),
                   RF_LNA_RANGE_FORCE_DISABLE(), and RF_LNA_RANGE_FORCE_ENABLE()
                   macros.  rf_set_cdma_lna_decision(), rf_set_cdma_lna_range()
                   functions added.  More RF Driver decomposition changes.
11/02/01   dbc     Removed no longer used function rf_set_rx_gain_comp()
10/25/01   dbc     Changed ACK to ACQ in RF_FREEZE_AGC_LOOP_DURING_DC_ACK
10/19/01   dbc     RF_FREEZE_AGC_LOOP_DURING_DC_ACK() macro added for MSM6000 and
                   MSM6050.
                   RF_CAP_PDM1_FOR_TX, RF_CAP_PDM2_FOR_TX, RF_HAS_PA_RANGE_CAL
                   support removed. FEATURE_PA_RANGE_TEMP_FREQ_COMP mainlined.
10/08/01   aks     Included rxfi.h, merged in phone_t changes.
09/19/01   dbc     Removed RF_HAS_RFR3100 dependencies - SW LNA Control removed
                   for CDMA and FM. NV Items introduced w/RF_HAS_RFR3100 mainlined.
09/14/01   dc      Extern-ed several calibration variables required in dfmagc.c
09/11/01   aks     Merged in changes from FEATURE_TX_POWER_BACK_OFF.
07/02/01   aks     Created from re-organization of rfmsm.c, rev 1.10.
                   O:/src/asw/COMMON/vcs/rfmsm.c_v   1.10   21 May 2001 16:40:16
===================================================================================*/

#include "comdef.h"
#include "customer.h"
#include "target.h"
#include "rficap.h"
#include "rex.h"
#include "rfcom.h"

#include "rf_hal.h"

/* RF Mode */
typedef enum 
{
  RFI_CDMA_MODE = 0,   /* CDMA */
  RFI_GPS_MODE,        /* GPS */
  RFI_WCDMA_MODE,      /* WCDMA */
  RFI_GSM_MODE,        /* GSM */
  RFI_AMPS_MODE        /* analog cellular */
} rfi_mode_type;

#ifdef FEATURE_CDMA1X
#include "rf_error.h"
#include "clk.h"
#include "rfnv.h"
#include "rxf.h"
#include "rf_cards.h"
#include "rf_prof.h"
#include "rf1x.h"

#ifdef FEATURE_ZIF_LOGGING
#include "log.h"
#endif

#ifdef RF_HAS_HDR
#error code not present
#endif
#endif /* FEATURE_CDMA1X */

#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */





/*===========================================================================
                        
                             TYPE DEFINITIONS

===========================================================================*/

#if 0 //TODO_7600: rfcom.h
#ifdef RF_HAS_GSM1X
#error code not present
#endif /* RF_HAS_GSM1X */
#endif /* #if 0 */

/* Which PMICs do we have? from NV_RF_PM_CONFIG_I. */
typedef enum
{
  RF_PM_LDO,    /* RF power supplied by LDOs */
  RF_PM_SMPS,   /* RF power supplied by SMPS */
  RF_PM_SMPS_6658, /* RF power supplied by PM6658 SMPS */
  RF_PM_SMPS_7500, /* RF power supplied by PM7500 SMPS */
  RF_PM_SMPS_7540, /* RF power supplied by PM7500 SMPS */
  RF_PM_MAX
} rf_pm_config_type;

/* Tx on or off status type */
typedef enum
{
  RFI_TX_IS_OFF = 0,       /* Tx has been stopped */
  RFI_TX_IS_ON  = 1        /* Tx is on            */
} rfi_tx_status_type;

/* Those who checks for synthesizer lock */
typedef enum
{
  RFI_SYNTH_ADJ_CDMA_PWR   = 0x01,   /* CDMA power adjust.  Periodic. */
  RFI_SYNTH_INIT_TEST      = 0x02,   /* Power up.       Non-periodic. */
  RFI_SYNTH_PA_ON          = 0x04    /* Turning PA on.  Non-periodic. */
} rfi_synth_checker;

/* Synthesizer lock status */
typedef enum
{
  RFI_SYNTH_NOT_LOCKED,      /* Synthesizer is NOT locked */
  RFI_SYNTH_LOCKED           /* Synthesizer IS locked     */
} rfi_synth_status;

#if 0
/* RF Mode */
typedef enum 
{
  RFI_CDMA_MODE = 0,   /* CDMA */
  RFI_GPS_MODE,        /* GPS */
  RFI_WCDMA_MODE,      /* WCDMA */
  RFI_GSM_MODE,        /* GSM */
  RFI_AMPS_MODE        /* analog cellular */
} rfi_mode_type;
#endif

/* WCDMA RF Rx device type */                
typedef enum
{
  RXDEV_NULL,
  RXDEV_RFR6220,
  RXDEV_RFR6250,
  RXDEV_RFR6275,
  RXDEV_RTR6275,
  RXDEV_RFR6500,
  RXDEV_RTR6285,
  RXDEV_MAX
} rf_rx_device_enum_type;

/* WCDMA RF Tx device type */                
typedef enum
{
  TXDEV_RTR6250,
  TXDEV_RTR6275,
  TXDEV_RTR6285,
  TXDEV_MAX
} rf_tx_device_enum_type;

/* GPS RF device type */
typedef enum
{
  GPSDEV_RFR6250,
  GPSDEV_RFR6275,
  GPSDEV_RFR6500,
  GPSDEV_RTR6285,
  GPSDEV_RTR6500,
  GPSDEV_MAX
} rf_gps_device_enum_type;

#ifdef FEATURE_RF_AFC

/* RAT system type.
 */
typedef enum
{
  RFI_RGS_NONE=0,
  RFI_RGS_CDMA_1X,
  RFI_RGS_CDMA_HDR,
  RFI_RGS_WCDMA,
  RFI_RGS_GSM,
  RFI_RGS_MAX=0xFF
} rfi_afc_rgs_type_enum_type;

/* Recent good system data type.
 */
typedef struct
{
  rfi_afc_rgs_type_enum_type rgs_type;
  int16 vco;
  int16 rot;
  uint8 temp;
  uint32 time;
} rfi_afc_rgs_data_type;

/* Specify if the data retrieved is a default value, an exact table entry,
 * or an interpolated value from the table.
 */
typedef enum
{
  RFI_AFC_DATA_EXACT=0,
  RFI_AFC_DATA_INTERPOLATED,
  RFI_AFC_DATA_DEFAULT
} rfi_afc_data_indicator_type;

/* Data type to specify nv items. Multiple items are specified by ORing
 * each mask together.
 */
typedef enum
{
  RFI_AFC_NV_VCO_TABLE_M   = 0x01,
  RFI_AFC_NV_RGS_DATA_M    = 0x02,
  RFI_AFC_NV_ALL_M         = 0xFF
} rfi_afc_nv_item_mask_type;

#endif /* FEATURE_RF_AFC */

#ifdef FEATURE_WCDMA_TX_PWR_BACK_OFF

/* This sturct hold the Power Backoff values for all four BCS cases */
typedef struct
{
  int16 caseA;
  int16 caseB;
  int16 caseC;
  int16 caseD;
} rfi_pwr_backoff_type;

#endif /* FEATURE_WCDMA_TX_PWR_BACK_OFF */


/* -----------------------------------------------------------
** In CDMA RxTx (only), has the CDMA Tx circuit been disabled?
** ----------------------------------------------------------- */
typedef enum {
  RF_TX_IS_OFF = 0,       /* Tx has been stopped */
  RF_TX_IS_ON  = 1        /* Tx is on            */
} rf_tx_status_type;


/* --------------------------------------
** Those who checks for synthesizer lock.
** -------------------------------------- */
typedef enum {
  RF_SYNTH_ADJ_CDMA_PWR   = 0x01,   /* CDMA power adjust.  Periodic. */
  RF_SYNTH_ADJ_FM_PWR     = 0x02,   /* FM power adjust.    Periodic. */
  RF_SYNTH_INIT_TEST      = 0x04,   /* Power up.       Non-periodic. */
  RF_SYNTH_PA_ON          = 0x08,   /* Turning PA on.  Non-periodic. */
  RF_SYNTH_GPS            = 0x10    /* GPS coarse tune. Non-periodic.*/
} rf_synth_checker;

/* ------------------------
** Synthesizer lock status.
** ------------------------ */
typedef enum {
  RF_SYNTH_NOT_LOCKED,      /* Synthesizer is NOT locked */
  RF_SYNTH_IS_LOCKED        /* Synthesizer IS locked     */
} rf_synth_status;

/* --------------------------------------------------
** AGC registers that can be converted to their
** corresponding power in dBm via rf_get_cagc_val()
** -------------------------------------------------- */
typedef enum {
  RX_AGC_VAL,
  TX_OPEN_LOOP_VAL,
  TX_AGC_VAL,
  TX_GAIN_ADJ_VAL
}
rf_cagc_val_type;



/* -------------------------------------------------------------------------

                         RF DRIVER STATE TYPE

---------------------------------------------------------------------------- */

typedef enum {

  RF_STATE_POWERUP_INIT,
    /* WCDMA Init State. This state is entered by rf_init() and is exited by
    rf_enable_wcdma_rx_pwr(). The RF NV items are read. The PDM controls are
    initialized. The LNA and PA control are initialized.
    */

  RF_STATE_WCDMA_RX,
    /* WCDMA Rx state.  This state is entered by rf_enable_wcdma_rx_pwr() or
    rf_disable_wcdma_tx() and is exited by either rf_sleep() or 
    rf_enable_wcdma_tx_pwr().
    The RF Rx hardware is powered up but not completely setup.  
    */

  RF_STATE_WCDMA_RXTX,
    /* WCDMA RxTx state.  This state is entered by rf_enable_wcdma_tx_pwr() 
    and is exited by rf_disable_wcdma_tx(). 
    The RF Rx and Tx hardware are powered up but not completely setup.
    */

  /* The following RF state descriptions apply when rf_chipx8_needed is FALSE.
     When rf_codec_on(TRUE) occurs, the phone exits sleep (if sleeping), CODEC
     power is turned on (if off), and chipx8 is enabled into the MSM2P (if
     disabled).  When rf_codec_on(FALSE) occurs, it is responsible for
     restoring the hardware according to the following state descriptions. */

  RF_STATE_SLEEP_1,
    /* CDMA sleep state 1 (first catnap).  This state is entered by rf_sleep()
    and exited by either rf_stay_asleep() or rf_enable_digital_rx_pwr().
    The database item DB_RF_MODE == DB_RF_SLEEP, but the RF Rx hardware is
    still powered up.  chipx8 is available.  CODEC power is off.  The Rx
    hardware is set to CDMA mode. */

  RF_STATE_SLEEP_2,
    /* CDMA sleep state 2 (after first catnap).  This state is entered by
    rf_stay_asleep() and exited by rf_enable_digital_rx_pwr().  The database
    item DB_RF_MODE == DB_RF_SLEEP and the RF Rx hardware has been powered
    down.  chipx8 is not available.  CODEC power is off.  The Rx hardware is
    set to CDMA mode. */

  RF_STATE_CDMA_RX,
    /* CDMA Rx state.  This state is entered by either rf_init() or
    rf_enable_digital_rx_pwr() and is exited by either rf_sleep(),
    rf_enable_tx_pwr(), or rf_enable_analog_rx_pwr().  The database item
    DB_RF_MODE has not yet been updated to DB_RF_CDMA or DB_RF_PCN (that
    happens in rf_init_digital()), so DB_RF_MODE indicates either DB_RF_SLEEP,
    DB_RF_CDMA, or DB_RF_ANALOG.  The RF Rx hardware is powered up but not
    completely setup.  chipx8 is available.  CODEC power is off.  The Rx
    hardware is set to CDMA mode. */

  RF_STATE_CDMA_RXTX,
    /* CDMA RxTx state.  This state is entered by rf_enable_tx_pwr() and exited
    by rf_tx_disable().  The database item DB_RF_MODE == DB_RF_CDMA or
    DB_RF_PCN.  The RF  Rx+Tx hardware are powered up but not completely setup.
    chipx8 is available.  CODEC power is on.  The Rx+Tx hardware are set to
    CDMA mode. */
 

  RF_STATE_GPS,
  /* GPS State. Entered by a call to rf_digital_band_select() with GPS as the
  passed rf band argument.*/
  
  RF_STATE_DIVERSITY
  /* Diversity State. Indicates the second chain is in the diversity state. */

} rf_state_type;



/* This type is used by rf_translate_band_class to determine if the band class
   is been passed into (IN) or returned (OUT) by the RF driver. */
typedef enum
{
    RF_TRANSLATE_BC_IN,
    RF_TRANSLATE_BC_OUT,
    RF_TRANSLATE_BC_INVALID
}rf_translate_bc_type;

typedef enum
{
  RFI_UARFCN_UL,
  RFI_UARFCN_DL
} rfi_uarfcn_ul_dl_type;

typedef enum {
    INVALID_PA_SWITCHPOINT = 0,
    PA_SWITCHPOINT_1 = 1,
    PA_SWITCHPOINT_2 = 2,
    PA_SWITCHPOINT_3 = 3,
    PA_SWITCHPOINT_MAX
} pa_switchpoint_type;

/*===========================================================================

                                MACRO DEFINITIONS

===========================================================================*/
#if defined (FEATURE_WCDMA) && defined (FEATURE_CDMA1X)
#error code not present
#elif defined (FEATURE_GSM) && defined (FEATURE_CDMA1X)
#error code not present
#elif defined(T_MSM7500J)
#define RF_DEF_HW RF_HW_TRF6500
#elif defined (FEATURE_WCDMA)
#error code not present
#elif defined T_MSM7600
#error code not present
#elif !defined (RF_DEF_HW)
/* Set 1x default if none defined */
#define RF_DEF_HW RF_HW_DEFAULT
#endif

#if defined (FEATURE_WCDMA) && defined (FEATURE_CDMA1X)
#error code not present
#elif defined (FEATURE_CDMA1X)
#define RF_DEF_BC (SYS_BAND_MASK_BC0|SYS_BAND_MASK_BC1)
#else
#define RF_DEF_BC SYS_BAND_MASK_WCDMA_I_IMT_2000
#endif




/*===========================================================================
RF_GET_RX_AGC_MIN_VAL

DESCRIPTION
  This macro will provide the calculation for the AGC MIN value that corresponds
  to the given LNA offset value.  The value return can be loaded directly into
  the RX_AGC_VALUE_MIN register corresponding to the LNA offset.

PARAMETERS
  lna_offset

DEPENDENCIES
  None

RETURN VALUE
  AGC min value

SIDE EFFECTS
  None

===========================================================================*/
#define RF_GET_RX_AGC_MIN_VAL( lna_offset ) \
   (int1)((MIN((lna_offset - 510)/4, 0) + 0x7F) & 0x7F )

/*===========================================================================
RF_LATCH_TX_GAIN_CTL_RD

DESCRIPTION
  This macro will latch the data representing the TX_GAIN_CTL value.  This
  macro needs to be call prior to reading the TX_GAIN_CTL_RD register.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define RF_LATCH_TX_GAIN_CTL_RD() HWIO_OUT( TX_GAIN_CTL_LATCH, 1 ); \
                                  HWIO_OUT( TX_GAIN_CTL_LATCH, 0 )

/*===========================================================================

MACRO RF_SATURATE

DESCRIPTION
  This macro returns the input (val) limited to the range specified by (min)
  and (max).  This is used for clipping the results of adjustment
  calculations where the newly calculated value may exceed the legal range
  for the control.  Since these calculations are typically closed loop
  integrators, this macro prevents "integrator windup" and/or "control
  overflow/underflow".

PARAMETERS
  val   the value to be limited (saturated) interpreted as a signed integer
  min   the minimum allowed value (usually 0)
  max   the maximum allowed value (usually 2^n-1)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define RF_SATURATE( val, min, max )  MAX( MIN( (val), (max) ), (min) )


/*===========================================================================
RF_FREEZE_CAGC_LOOP_DURING_DC_ACQ

DESCRIPTION
  This macro causes 0s (rssi bypassed) to be gated to the AGC LOOP integrator
  when the RX Front is performing a DC acquisition. This is needed to prevent
  large DC errors from disrupting the AGC Loop.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#define RF_FREEZE_AGC_LOOP_DURING_DC_ACQ() \
      HWIO_OUTM(AGC_DC_GAIN, \
                              HWIO_FMSK(AGC_DC_GAIN,AGC_LOOP_FREEZE_EN) \
                            | HWIO_FMSK(AGC_DC_GAIN,AGC_DC_GAIN_EN) \
                            | HWIO_FMSK(AGC_DC_GAIN,AGC_DC_GAIN_VAL), \
                                                    HWIO_FMSK(AGC_DC_GAIN,AGC_LOOP_FREEZE_EN) \
                                                  | HWIO_FMSK(AGC_DC_GAIN,AGC_DC_GAIN_EN) \
                                                  | CAGC_DC_GAIN_MAX_VALUE);


/*===========================================================================
RF_LNA_STATE_MACHINE_NORMAL

DESCRIPTION
  This macro enables the HW LNA State machine. LNA dedision ( and consequently
  LNA RANGE and MIXER RANGE) are controlled by the internal HW.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#define RF_LNA_STATE_MACHINE_NORMAL(rf_device) \
      MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_LNA_CTL), LNA_DECISION_OVERRIDE, 0, (int)(rfcom_to_path[ (int)rf_device ])); \
      MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_LNA_CTL), LNA_RANGE_OVERRIDE_N,  1, (int)(rfcom_to_path[ (int)rf_device ]))



/*===========================================================================
RF_OVERRIDE_LNA_STATE_MACHINE

DESCRIPTION
  This macro allows the LNA Decision bits (and consequently LNA RANGE and MIXER
  RANGE) to be directly controlled by the CPU

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define RF_OVERRIDE_LNA_STATE_MACHINE(rf_device) \
     MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_LNA_CTL), LNA_DECISION_OVERRIDE, 1, (int)rfcom_to_path[ (int)rf_device ])



/*===========================================================================
RF_LNA_RANGE_FORCE_DISABLE

DESCRIPTION
  This macro causes the signals at the LNA RANGE and MIXER RANGE pins to be
  controlled by the HW AGC Unit.  Note that the signals observed at these pins
  are affected by polarity settings.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#define RF_LNA_RANGE_FORCE_DISABLE() \
      HWIO_OUTM(AGC_CTL2, CAGC_LNA_CTL_M, CAGC_LNA_CTL_INT_V)



/*===========================================================================
RF_OVERRIDE_LNA_STATE_MACHINE

DESCRIPTION
  This macro allows the LNA RANGE and MIXER RANGE pins to be directly
  manipulated. Note that the LNA State machine is not overriden.  Also
  the signals observed at the LNA RANGE and MIXER RANGE pins are not
  affected by the polarity settings.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define RF_LNA_RANGE_FORCE_ENABLE() \
      HWIO_OUTM(AGC_CTL2, CAGC_LNA_CTL_M, CAGC_LNA_CTL_CPU_V)


/*===========================================================================
CAGC_SET_AGC_LOOP_TIME_CONST_TO_IM

DESCRIPTION
  This macro sets the AGC Loop Time Constant to the IM value (73 usec)

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define CAGC_SET_AGC_LOOP_TIME_CONST_TO_IM( ) \
  HWIO_OUTM( AGC_CTL4, CAGC_GAIN_M, CAGC_TC_0073_US )



/*===========================================================================
CAGC_SET_AGC_LOOP_TIME_CONST_TO_TRACKING

DESCRIPTION
  This macro sets the AGC Loop Time Constant to the vracking value (392 usec)

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define CAGC_SET_AGC_LOOP_TIME_CONST_TO_TRACKING( ) \
  HWIO_OUTM( AGC_CTL4, CAGC_GAIN_M, CAGC_TC_0392_US )



/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

                      RF COMPENSATION CONSTANTS

=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/* -----------------------------------------------------------------------
** PDM range and offset constants for hardware and software.
** ----------------------------------------------------------------------- */

#define RF_PDM_EN_V         (DMOD_PDM1_EN_V  |  DMOD_PDM2_EN_V)
#define RF_PDM_DIS_V        (DMOD_PDM1_DIS_V | DMOD_PDM2_DIS_V)

#define RF_PDM1_PWR_SAVE         0 /* Pull down. Lower current consumption */
#define RF_PDM2_PWR_SAVE         0 /* Pull down. Lower current consumption */

#define RF_PDM1_REG_MIN     (-128) /* min for TX_GAIN_COMP PDM register    */
#define RF_PDM1_REG_MAX     (+127) /* max for TX_GAIN_COMP PDM register    */

#define RF_PDM2_REG_MIN     (-128) /* min for TX_GAIN_COMP PDM register    */
#define RF_PDM2_REG_MAX     (+127) /* max for TX_GAIN_COMP PDM register    */

#define RF_RX_AGC_REG_MIN   (-128) /* min for AGC_VALUE register           */
#define RF_RX_AGC_REG_MAX   (+127) /* max for AGC_VALUE register           */

#define RF_RX_COMP_REG_MIN  (-128) /* min for RX_GAIN_COMP PDM register    */
#define RF_RX_COMP_REG_MAX  (+127) /* max for RX_GAIN_COMP PDM register    */

#define RF_TX_COMP_REG_MIN  (-128) /* min for TX_GAIN_COMP PDM register    */
#define RF_TX_COMP_REG_MAX  (+127) /* max for TX_GAIN_COMP PDM register    */

#define RF_TX_PWR_REG_MIN   (-512) /* min for TX gain & pwr limit register */
#define RF_TX_PWR_REG_MAX   (+511) /* max for TX gain & pwr limit register */
#define RF_TX_PWR_REG_MSBS  0xfe00 /* MSBs for 10 bit data in 16 bit field */

#define RF_TX_GAIN_ADJ_MIN   (-128) /* min for TX gain register */
#define RF_TX_GAIN_ADJ_MAX   (+127) /* max for TX gain register */

#define RF_TX_AGC_REG_MIN   (-256) /* min for TX AGC PDM register          */
#define RF_TX_AGC_REG_MAX   (+255) /* max for TX AGC PDM register          */

#define RF_PA_RANGE_REG_MIN  (  0) /* min for PA range rise/fall register  */
#define RF_PA_RANGE_REG_MAX  (255) /* max for PA range rise/fall register  */

#define RF_RX_GAIN_COMP_MIN      0 /* min for RX_GAIN_COMP software        */
#define RF_RX_GAIN_COMP_MAX    255 /* max for RX_GAIN_COMP software        */

#define RF_TX_GAIN_COMP_MIN      0 /* min for TX_GAIN_COMP software        */
#define RF_TX_GAIN_COMP_MAX    255 /* max for TX_GAIN_COMP software        */

#define RF_FM_TX_AGC_MIN         0 /* min for TX AGC software              */
#define RF_FM_TX_AGC_MAX       511 /* max for TX AGC software              */

#define RF_TX_PWR_CALC_MIN       0 /* min for TX pwr limit software        */
#define RF_TX_PWR_CALC_MAX    1023 /* max for TX pwr limit software        */

#define RF_TX_AGC_MIN            0 /* min TX AGC value                     */
#define RF_TX_AGC_MAX         1023 /* max Tx AGC value (with offset)       */
#define RF_TX_AGC_OFF          768 /* offset for Tx AGC table index        */

#define RF_TX_AGC_TABLE_SIZ    256 /* size of Tx AGC interpolated table    */


#define RF_EXP_HDET_SIZ (RF_TX_AGC_TABLE_SIZ / NV_CDMA_EXP_HDET_VS_AGC_SIZ)

#define RF_FM_HDET_BUCKETS    (NV_TEMP_TABLE_SIZ - 1)
                                   /* # HDET buckets, with max=special     */

#define RF_CDMA_EXP_HDET_VS_AGC_MAX_IDX  (NV_CDMA_EXP_HDET_VS_AGC_SIZ-1)

#define RF_CDMA_TX_LIM_OFFSET  768 /* rf_cdma_tx_lim_vs_temp[] adder       */

#define RF_FM_RX_GAIN_COMP   ((byte) 0x01) /* Rx gain comp for FM          */
#define RF_CDMA_RX_GAIN_COMP ((byte) 0x01) /* Rx gain comp for CDMA        */

/* -----------------------------------------------------------------------
** System Mode definitions
** ----------------------------------------------------------------------- */
#define RF_SYSTEM_MODE_CDMA_HDR_M 0x3BFE4DD   /* bit mask for CDMA 1x/HDR  */
#define RF_SYSTEM_MODE_CDMA_V     0x0000001   /* Setting for CDMA mode     */
#define RF_SYSTEM_MODE_HDR_V      0x0150041   /* Setting for HDR mode      */

/* -----------------------------------------------------------------------
** Synthesizer ALL LOCK port register
** ----------------------------------------------------------------------- */
#define RF_SYNTH_STATUS_R    PA_ON_STATUS_RH

#ifdef RF_HAS_MODEM_PA_CTL
#define RF_ALL_LOCK_M     HWIO_FMSK(MODEM_PA_ON_STATUS,SYNTH_LOCK) /* ALL LOCK mask*/
#define RF_PA_ON_M        HWIO_FMSK(MODEM_PA_ON_STATUS,PA_ON)    /* PA_ON mask     */
#elif defined( T_MSM6050 )
#define RF_ALL_LOCK_M     HWIO_FMSK(PA_ON_STATUS,SYNTH_LOCK) /* ALL LOCK mask*/
#define RF_PA_ON_M        HWIO_FMSK(PA_ON_STATUS,PA_ON)    /* PA_ON mask     */
#elif defined( T_MSM6000 )
#error code not present
#endif

#define RF_ALL_LOCK_V     RF_ALL_LOCK_M                    /* ALL LOCK     */
#define RF_OUT_OF_LOCK_V  0x0                              /* Out of LOCK  */

#define RF_PA_ON_V        RF_PA_ON_M                     /* PA_ON is on    */
#define RF_PA_OFF_V       0x0                            /* PA_ON is off   */


/* -----------------------------------------------------------------------
** Read synthesizer ALL LOCK port
** ----------------------------------------------------------------------- */
#ifdef RF_HAS_MODEM_PA_CTL
#define RF_SYNTH_STATUS_READ      ((word) HWIO_IN( MODEM_PA_ON_STATUS ))
#else
#define RF_SYNTH_STATUS_READ      ((word) HWIO_IN( PA_ON_STATUS ))
#endif
/* -----------------------------------------------------------------------
** Linearizer constants
** ----------------------------------------------------------------------- */

// separate Rx, Tx, RF attenuation linearizer segments definitions.
#define RF_RX_LIN_HW_SEGMENTS     16   /* no. of segments in each Rx linearizer */
#define RF_TX_LIN_HW_SEGMENTS     16   /* no. of segments in each Tx linearizer */
#define RF_ATTEN_LIN_HW_SEGMENTS  16   /* no. of segments in each RF atten. linearizer */

/* Safest values to write into linearizer tables if those tables are not used. */
#define RF_RX_LIN_DEFAULT_UB_V    0x0  /* for RF Rx linearizer tables; high byte */
#define RF_RX_LIN_DEFAULT_LB_V    0x0  /* low byte */
#define RF_TX_LIN_DEFAULT_UB_V    0x0  /* for RF Tx linearizer tables */
#define RF_TX_LIN_DEFAULT_LB_V    0x0
#define RF_ATTEN_LIN_DEFAULT_UB_V 0x0  /* for RF Attenuation linearizer tables */
#define RF_ATTEN_LIN_DEFAULT_LB_V 0x0

#define RF_TX_LIN_SEGMENT_SIZ     64   /* Tx linearizer segment size       */

/* CDMA_TX_LIN_SEGMENT_SIZ expressed as a shift.  size == (1 << value)
*/
#define RF_TX_LIN_SEGMENT_SIZ_SHIFT     6

/* CDMA_TX_LIN_SEGMENT_SIZ expressed as a mask, for modulus operations.
** This can only be done if scale is a power of 2.  x % size == x & mask
*/
#define RF_TX_LIN_SEGMENT_SIZ_MASK      63

/* Minimum X value for Tx Master Curve.
**
** Note:  If TX_MASTER_X_MIN is <0, indexing with MIN <= i <= MAX breaks.
*/
#define RF_TX_MASTER_X_MIN         0   /* min for Tx linearizer "curve x"  */
#define RF_TX_MASTER_X_MAX         \
  (((NV_CDMA_TX_LIN_MASTER_SIZ -1) * RF_CDMA_TX_LIN_SLP_SCALE) - 1)

#define RF_TX_LIN_OFF_MIN          0   /* minimum for Tx linearizer offset */
#define RF_TX_LIN_OFF_MAX        511   /* maximum for Tx linearizer offset */

#ifdef RF_HAS_BETA_SCALING_TX_AGC
#define RF_BETA_SCALING_MIN_VALUE(a, b)  (((a) <= (b)) ? (a) : (b))
#define RF_CDMA_MDSP_PA_SWITCHPOINT_INVALID_VAL  255
#endif


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

                           TIME CONSTANTS

=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/* Microseconds needed for RF Tx or Rx hardware to settle when turned on,
** before it can be programmed.  Use the millisecond value defined in rfc.h,
** and expand it to microseconds.
*/
#define CAGC_STATE_CLK             30
   /* wait 30 us for the cagc tx state to settle */



/* -----------------------------------------------------------------------
** Call back constants for clk_reg() data structure
** ----------------------------------------------------------------------- */

#define RF_CALL_BACK_ONCE          FALSE /* causes only one call back */
#define RF_CALL_BACK_REPEAT         TRUE /* causes repeating call backs */

/* -----------------------------------------------------------------------
** Time values (in milliseconds) for clk_reg() call back functions
** ----------------------------------------------------------------------- */

/* Start immediately.  Used by both hardware and software LNA control.
*/
#define RF_CDMA_LNA_CTL_START_TIME         ((int4) 1 + CLK_MS_PER_TICK)

/* Milliseconds of delay between Tx power limit adjustments in CDMA mode
*/
#define RF_TX_LIM_ADJ_REPEAT_TIME    ((int4)100)

/* Call back after 5 ms
*/
#define RF_CALL_BACK_5_MS            ((int4)5)


#ifdef FEATURE_TX_POWER_BACK_OFF
#define RF_DIGITAL_GAIN_FACTOR_85_DB    18   /* AGC counts for 1.5 dB in 85 dB dynamic range*/
#define RF_DIGITAL_GAIN_FACTOR_102_DB   15   /* AGC counts for 1.5 dB in 102 dB dynamic range */
#endif



/* -------------------------------------------------------
** Structure used to calculate power for the AGC registers
** defined by rf_cagc_val_type
** ------------------------------------------------------- */
typedef struct
{
  int2 val_index;
  int2 val_min;
  int2 val_max;
  int2 scale_factor_numer;
  int2 scale_factor_denom;
  int1 polarity;
}
rf_cagc_vals_vars_type;


#ifdef RF_HAS_LINEAR_INTERPOLATION
#ifdef FEATURE_CDMA1X 

typedef struct /* global freq comp items */
{
   int2 i_lin_freq_comp[RF_NUM_TX_LINEARIZERS];
   int2 i_tx_lim_vs_freq;
#ifdef RF_HAS_POWER_MODE_LIB
   int2 i_vga_gain_offset_vs_freq[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ];
   int2 i_lna_1_offset_vs_freq[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ];
   int2 i_lna_2_offset_vs_freq[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ];
   int2 i_lna_3_offset_vs_freq[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ];
   int2 i_lna_4_offset_vs_freq[RF_PATHS_TABLE_SIZ][NV_RF_POWER_MODE_SIZ];
#else
   int2 i_vga_gain_offset_vs_freq[RF_PATHS_TABLE_SIZ];
   int2 i_lna_1_offset_vs_freq[RF_PATHS_TABLE_SIZ];
   int2 i_lna_2_offset_vs_freq[RF_PATHS_TABLE_SIZ];
   int2 i_lna_3_offset_vs_freq[RF_PATHS_TABLE_SIZ];
   int2 i_lna_4_offset_vs_freq[RF_PATHS_TABLE_SIZ];
#endif /* RF_HAS_POWER_MODE_LIB */
   word last_chan_checked[RF_PATHS_TABLE_SIZ];
   rf_path_enum_type last_chain_checked;
   rf_card_band_type last_band_checked[RF_PATHS_TABLE_SIZ];
   uint32 converted_tx_channel_list_arr[NV_FREQ_TABLE_SIZ];
   uint32 converted_rx_channel_list_arr[RF_PATHS_TABLE_SIZ][NV_FREQ_TABLE_SIZ];
   boolean channel_list_converted_flag[RF_PATHS_TABLE_SIZ];

   #ifdef RF_HAS_SUB_BLOCK_SUPPORT
   boolean last_block_mask_checked[RF_PATHS_TABLE_SIZ];
   #endif

} rf_offsets_vs_freqs_type;

#endif /* FEATURE_CDMA1X */
#endif /* RF_HAS_LINEAR_INTERPOLATION */


#ifdef FEATURE_CDMA1X

/* RF Card Id for current target */
extern rf_hw_type rf_card_in_system;

/* -------------------------------------------------------
** Structure used to track the state of the two chains
** ------------------------------------------------------- */
typedef struct {
  rf_state_type rf_state;
  rf_state_type rf_prev_state;
  db_rf_mode_type rf_mode;
  rf_card_band_type rf_curr_cdma_band;
  rf_card_band_type rf_prev_cdma_band;
  byte rf_freq_index;
  word rf_chan;
  rf_hw_type rf_curr_hw;
  rfnv_digital_items_type *rf_nv_dig_items_ptr;
  byte rf_synth_time_iter;
  rf_prof_struct_type *rf_wu_ptr;
} rf_chain_state_type;

extern rf_chain_state_type rf_chain_status[];

/* 
* Current value of the PRx PA_R_MAP which
* can change based on band dependent pa_r_map value 
*/
extern byte rf_current_pa_r_map;

/* Use this structure to hold the band class and its corresponding band mask
   (from call processing).*/
typedef struct
{
  rf_card_band_type  bc;
  sys_band_mask_type mask;
}rf_bc_and_mask_type;



/* -------------------------------------------------------------------------

                            LOG TYPES

---------------------------------------------------------------------------- */

#ifdef FEATURE_ZIF_LOGGING
/*---------------------------------------------------------------------------
              Extended AGC Logging
---------------------------------------------------------------------------*/
#define LOG_EXT_AGC_CNT  100

LOG_RECORD_DEFINE(log_ext_agc)

  uint8 lna_range[LOG_EXT_AGC_CNT];
    /* Contents of the LNA_RANGE_RD field of the LNA_PA_RD register, sampled
       every 20ms. */

  int16 cdma_vga_gain[LOG_EXT_AGC_CNT];
    /* Contents of the CDMA_VGA_GAIN_RD register, sampled every 20ms. */

LOG_RECORD_END


/*---------------------------------------------------------------------------
              Transmit AGC Logging
---------------------------------------------------------------------------*/
#define LOG_TX_AGC_CNT  100

LOG_RECORD_DEFINE(log_tx_agc)

  int16  tx_agc_adj[LOG_TX_AGC_CNT];
    /* Contents of the TX_AGC_ADJ_RD register, sampled every 20ms. */

  int16  tx_open_loop[LOG_TX_AGC_CNT];
    /* Contents of the TX_OPEN_LOOP_RD register, sampled every 20ms. */

  int16  tx_gain_adj[LOG_TX_AGC_CNT];
    /* Contents of the TX_GAIN_ADJ_RD register, sampled every 20ms. */

  int16  tx_gain_ctl[LOG_TX_AGC_CNT];
    /* Contents of the TX_GAIN_CTL_RD register, sampled every 20ms. */

  uint8  ratchet_bit[LOG_TX_AGC_CNT];
    /* Contents of the RATCHET_BIT_DIS register, sampled every 20ms. */

LOG_RECORD_END


/*---------------------------------------------------------------------------
              I/Q Offsets Logging
---------------------------------------------------------------------------*/
#define LOG_IQ_OFFSETS_CNT  38

LOG_RECORD_DEFINE(log_iq_offsets)

  int32  rxf_cg_ioffset[LOG_IQ_OFFSETS_CNT];
    /* Contents of the RXF_CG_IOFFSET register, sampled every 20ms. */

  int32  rxf_cg_qoffset[LOG_IQ_OFFSETS_CNT];
    /* Contents of the RXF_CG_QOFFSET register, sampled every 20ms. */

  int32  rxf_fg_ioffset[LOG_IQ_OFFSETS_CNT];
    /* Contents of the RXF_FG_IOFFSET register, sampled every 20ms. */

  int32  rxf_fg_qoffset[LOG_IQ_OFFSETS_CNT];
    /* Contents of the RXF_FG_QOFFSET register, sampled every 20ms. */

  int32  dacc_est_ioffset[LOG_IQ_OFFSETS_CNT];
    /* Contents of the DACC_EST_IOFFSET register, sampled every 20ms. */

  int32  dacc_est_qoffset[LOG_IQ_OFFSETS_CNT];
    /* Contents of the DACC_EST_QOFFSET register, sampled every 20ms. */

LOG_RECORD_END


/*---------------------------------------------------------------------------
              DACC I/Q Accumulator Logging
---------------------------------------------------------------------------*/
#define LOG_DACC_IQ_ACCUM_CNT  45

LOG_RECORD_DEFINE(log_dacc_iq_accum)

  int16  dacc_iaccum0[LOG_DACC_IQ_ACCUM_CNT];
    /* Contents of the DACC_IACCUM0 register, sampled every 20ms. */

  int16  dacc_iaccum1[LOG_DACC_IQ_ACCUM_CNT];
    /* Contents of the DACC_IACCUM1 register, sampled every 20ms. */

  int16  dacc_iaccum2[LOG_DACC_IQ_ACCUM_CNT];
    /* Contents of the DACC_IACCUM2 register, sampled every 20ms. */

  int16  dacc_iaccum3[LOG_DACC_IQ_ACCUM_CNT];
    /* Contents of the DACC_IACCUM3 register, sampled every 20ms. */

  int16  dacc_iaccum4[LOG_DACC_IQ_ACCUM_CNT];
    /* Contents of the DACC_IACCUM4 register, sampled every 20ms. */

  int16  dacc_qaccum0[LOG_DACC_IQ_ACCUM_CNT];
    /* Contents of the DACC_QACCUM0 register, sampled every 20ms. */

  int16  dacc_qaccum1[LOG_DACC_IQ_ACCUM_CNT];
    /* Contents of the DACC_QACCUM1 register, sampled every 20ms. */

  int16  dacc_qaccum2[LOG_DACC_IQ_ACCUM_CNT];
    /* Contents of the DACC_QACCUM2 register, sampled every 20ms. */

  int16  dacc_qaccum3[LOG_DACC_IQ_ACCUM_CNT];
    /* Contents of the DACC_QACCUM3 register, sampled every 20ms. */

  int16  dacc_qaccum4[LOG_DACC_IQ_ACCUM_CNT];
    /* Contents of the DACC_QACCUM4 register, sampled every 20ms. */

LOG_RECORD_END

#endif /* FEATURE_ZIF_LOGGING */



/* -------------------------------------------------------------------------

                            SHARED RF DRIVER DATA

---------------------------------------------------------------------------- */

/* last software setting of CDMA Tx power limit
** this is 0..1023 min..max while the hardware is +511..-512 min..max
*/
extern int2    rf_tx_pwr_limit;


/* Pointer to NV FM items/tables
*/
extern rfnv_fm_items_type *rf_nv_fm_items_ptr;


/* NV Table of pointers to bands */
#ifdef RF_HAS_SUB_BLOCK_SUPPORT
extern rfnv_digital_items_type *rf_digital_nv_table[RF_PATH_MAX][RF_MAX_BAND];
#else
extern rfnv_digital_items_type *rf_digital_nv_table[];
#endif

/* Pointer to NV common items
*/
extern rfnv_common_items_type *rf_nv_common_items_ptr;

extern clk_cb_type rf_pwr_call_back;   /* Call back struct for power control */



/* Pointer to the the expected agc tables.
*/
extern byte *rf_exp_agc_table_ptr;

extern rfnv_all_data_type rf_nv_data;

#ifdef FEATURE_TRIMODE_ITEMS
/* lookup table for expected AGC for any HDET value 0.. 255 for PCS
*/
extern byte    rf_pcs_exp_agc_table[RF_TX_AGC_TABLE_SIZ];
#endif  /* FEATURE_TRIMODE_ITEMS  */

/* lookup expected AGC (TX_GAIN_CTL - 768) for any HDET value 0..255
*/
extern byte    rf_cdma_exp_agc_table[RF_TX_AGC_TABLE_SIZ];

/* In CDMA RxTx (only), is the Tx circuit currently on or off.
*/
extern rf_tx_status_type rf_tx_status;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   CDMA mode Tx power limit data vs temperature,
   frequency, TX_AGC_ADJ PDM setting, and HDET reading
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
extern int2 rf_cdma_tx_lim_vs_temp_now;   /* comp val for current temp */

#ifdef RF_HAS_HDET_UINT16
extern uint16 rf_hdet_off_adj;     /* adjusted ADC HDET reading offset */
extern uint16 rf_hdet_spn_adj;     /* adjusted ADC HDET reading span   */
#else
extern byte rf_hdet_off_adj;     /* adjusted ADC HDET reading offset */
extern byte rf_hdet_spn_adj;     /* adjusted ADC HDET reading span   */
#endif

extern byte    rf_pwr_index;        /* NV table index for current power level */
                                   /*   rf_fm_agc_set_vs_pwr[] */
                                   /*   rf_fm_exp_hdet_vs_pwr[] */
                                   /*   rf_fm_err_slp_vs_pwr[] */

/* last software setting of TX_AGC_ADJ software
** in FM mode, this is 0..511 while the hardware is +255..-256
** in CDMA mode, this is 0..1023 while the hardware is +511..-512
*/
extern int2    rf_tx_agc_adj;

#ifdef RF_HAS_HDET_UINT16
extern uint16  rf_hdet_data;
#else
extern byte  rf_hdet_data;                 /* last raw reading of HDET in CDMA mode */
#endif


extern clk_cb_type rf_ena_hwlna_cbk;   /* clock call-back for enabling hw LNA control */

#ifdef RF_HAS_LINEAR_INTERPOLATION
/* object that store all the offset_vs_freq items */
extern rf_offsets_vs_freqs_type rf_freq_comp;
#endif


/* RF Card Id for current target */
extern rf_hw_type rf_card_in_system;


/* -----------------------------------------------------------------------

                      SHARED CALIBRATION RELATED DATA

-------------------------------------------------------------------------- */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   FM mode TX_AGC_ADJ initial setting vs power, frequency, and temperature
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
extern int2  rf_fm_agc_set_vs_temp_now;   /* comp val for current temp */

extern int const rf_pa_offset_addr[];
extern int const rf_pa_offset_mask[];

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   FM mode TX_AGC_ADJ adjustments
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
extern byte  rf_fm_exp_hdet_vs_temp_now;   /* comp val for current temp */
#ifdef RF_HAS_HDET_UINT16
extern uint16  rf_hdet_data;
#else
extern byte  rf_hdet_data;                 /* last raw reading of HDET in CDMA mode */
#endif
extern byte  rf_pwr_index;                 /* NV table index for current power level */
extern int2  rf_tx_agc_adj;                /* last software setting of TX_AGC_ADJ */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   CDMA mode Tx AGC linearization (MSM2P data is derived from these tables)
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
extern int2  rf_cdma_tx_lin_vs_temp_now[RF_NUM_TX_LINEARIZERS];
                                                 /* comp val for current temp */

extern int2  rf_cdma_tx_slp_vs_temp_now[RF_NUM_TX_LINEARIZERS];

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   Rx and Tx Linearizer offsets and slopes arrays.  The first half of the
   arrays (index 0..15) stores the Rx linearizer values.  The second half
   of the arrays (index 16..31) stores the Tx linearizer values.
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* Tx linearizer offsets and slopes */
extern byte rf_tx_lin_off[ RF_NUM_TX_LINEARIZERS ][ RF_TX_LIN_HW_SEGMENTS ];
extern byte rf_tx_lin_slp[ RF_NUM_TX_LINEARIZERS ][ RF_TX_LIN_HW_SEGMENTS ];

#endif //FEATURE_CDMA1X


/*____________________________________________________________________________
                   Internal RF Driver function prototypes
____________________________________________________________________________*/


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
word rf_rescale_tx_pwr_limit( word );

/*===========================================================================

FUNCTION RF_INIT_CAGC_LINEARIZER_RANGE                               EXTERNAL

DESCRIPTION
  This function sets the global variable that determines the linearizer range
  of the CAGC section (RX & TX) in CDMA.  It relies on an NV item in
  "common" structure.


DEPENDENCIES
  Should not be called until after NV is read

RETURN VALUE
  boolean : linearizer range selection success/fail

SIDE EFFECTS
  None
===========================================================================*/
boolean rf_init_cagc_linearizer_range( void );


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
void rf_set_cagc_linearizer_range( void );


#if defined( FEATURE_HWTC )
#error code not present
#endif


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
void rf_get_cagc_val( rf_cagc_val_type, rf_cagc_vals_vars_type * );

/*===========================================================================

FUNCTION RF_GET_PATH_CAGC_VAL                                        EXTERNAL

DESCRIPTION
  Multi-path version of rf_get_cagc_val().

DEPENDENCIES
  Should not be called until after NV is read

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rf_get_path_cagc_val( rfcom_device_enum_type, rf_cagc_val_type, rf_cagc_vals_vars_type * );

/*============================================================================

FUNCTION RF_LOAD_IM_LEVEL_CTLS                                       EXTERNAL

DESCRIPTION
  This function will configure the CAGC registers that control the switching
  behavior of the HW LNA State Machine in the presence of IM

DEPENDENCIES
  rf_init_hw_lna_ctl must have been called once.
  rf_nv_digtial_items_ptr must be initialized and pointing to correct
  digital structure.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void rf_load_im_level_ctls( rfcom_device_enum_type );


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
void rf_set_lna_decision_to_low_gain_state( rfcom_device_enum_type );


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
void rf_set_agc_val_to_low_gain( rfcom_device_enum_type );


/*============================================================================

FUNCTION  RF_DISABLE_IN_LEVEL_CTLS                                     EXTERNAL

DESCRIPTION
  This function will disable the switching behavior of the HW LNA State
  Machine in the presence of IM.  This can be accomplished by setting the
  bypass and nonbypass timers for each state to 0x0

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void rf_disable_im_level_ctls( rfcom_device_enum_type );


/*============================================================================

FUNCTION  RF_ENABLE_IN_LEVEL_CTLS                                     EXTERNAL

DESCRIPTION
  This function will enable the switching behavior of the HW LNA State
  Machine in the presence of IM by setting the bypass and nonbypass timers
  to their IM (non zero) settings.

DEPENDENCIES
  rf_init_hw_lna_ctl must have been called once.
  rf_nv_digtial_items_ptr must be initialized and pointing to correct
  digital structure.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void rf_enable_im_level_ctls( rfcom_device_enum_type );


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
void rf_init_rx_cagc( rfcom_device_enum_type );


/*===========================================================================

FUNCTION RF_INIT_RX_CAGC_DELAYED                         EXTERNAL FUNCTION

DESCRIPTION
  This function initializes the RX CAGC controls that require NV items. It
  should not be called until NV has been read

DEPENDENCIES
  rf_nv_dig_items_ptr must have been initialized and RF NV has been read
  and the structures populated.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rf_init_rx_cagc_delayed(void);


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
void rf_init_tx_cagc( rfcom_device_enum_type device );


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
void rf_init_tx_cagc_delayed(void);


/*===========================================================================

FUNCTION RF_GET_RAW_CDMA_RX_AGC

DESCRIPTION
  This function returns the CDMA Rx AGC setting in 8 bit two's complement
  format.

DEPENDENCIES
  None

RETURN VALUE
  int1 = 8 bit two's complement CDMA Rx AGC value

SIDE EFFECTS
  None

===========================================================================*/
int1 rf_get_raw_cdma_rx_agc( void );

/*===========================================================================

FUNCTION RF_GET_RAW_CDMA_TX_AGC

DESCRIPTION
  This function returns the raw CDMA Tx AGC setting in 10 bit binary format.

DEPENDENCIES
  None

RETURN VALUE
  word = 10 bit Tx AGC value

SIDE EFFECTS
  None

===========================================================================*/
word rf_get_raw_cdma_tx_agc( void );

/*===========================================================================

FUNCTION RF_INIT_HW_LNA_CTL

DESCRIPTION
  This function will initialize and configure LNA bypass control.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rf_init_hw_lna_ctl( rfcom_device_enum_type );


/*============================================================================

FUNCTION RF_LOAD_LNA_SWITCHING_CTLS                                   EXTERNAL

DESCRIPTION
  This function will configure the CAGC registers that control the switching
  behavior of the HW LNA State Machine

DEPENDENCIES
  rf_init_hw_lna_ctl must have been called once.
  rf_nv_digtial_items_ptr must be initialized and pointing to correct
  digital structure.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void rf_load_lna_switching_ctls( rfcom_device_enum_type );


/*===========================================================================

FUNCTION RF_ENABLE_125_MS_ISR                               INTERNAL FUNCTION

DESCRIPTION
  Enables the 1.25 ms ISR which collects the temperature
  and HDET data for rf_adjust_cdma_pwr_limit() calculations.  This function is
  reached through a call back RF_TX_LIM_ADJ_REPEAT_TIME ms after a power limit
  adjustment.  It is initially scheduled as a call back by rf_tx_enable().
  Subsequent activation is scheduled by rf_adjust_cdma_pwr_limit().  This
  function is deactivated by rf_tx_disable().

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rf_enable_125_ms_isr
(
  int4 ms_interval          /* time interval since last call */
  /* Tell lint to ignore the unused parameter. */
  /*lint -esym(715,ms_interval) */
);


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
);



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
  rf_config, rf_cdma_tx_comp_vs_freq[], rf_cdma_rx_comp_vs_freq[],
  rf_cdma_rx_lin_off[], must have been loaded from NV storage.

  Temperature data:  rf_cdma_rx_lin_vs_temp_now, rf_cdma_rx_slp_vs_temp_now,
  rf_cdma_tx_lin_vs_temp_now, and rf_cdma_tx_slp_vs_temp_now, must be valid.

  The linearizers are only loaded when the RF is in either
  RF_STATE_CDMA_RX or RF_STATE_CDMA_RXTX states.

RETURN VALUE
  None

SIDE EFFECTS
  rf_rx_lin_off[] and rf_tx_lin_off[] are updated.
  rf_rx_lin_slp[] and rf_tx_lin_slp[] are updated.

===========================================================================*/
void rf_load_cagc_linearizers(rfcom_device_enum_type device);



#ifdef FEATURE_CDMA1X
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
);
#endif //FEATURE_CDMA1X


/*===========================================================================

FUNCTION RF_ENABLE_CDMA_LNA_TEMP_COMP                   EXTERNALIZED FUNCTION

DESCRIPTION
  This function starts the periodic temperature compensation and CDMA
  LNA control.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rf_enable_cdma_lna_temp_comp( void );


/*===========================================================================

FUNCTION RF_CDMA_TEMP_UPDATE                                INTERNAL FUNCTION

DESCRIPTION
  Update the temperature values used to calculate temperature-based
  RF calibration for CDMA.

DEPENDENCIES
  Temperature compensation tables in NV storage already be loaded.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rf_cdma_temp_update( void );

/*===========================================================================

FUNCTION RF_DISABLE_CDMA_LNA_TEMP_COMP                      INTERNAL FUNCTION

DESCRIPTION
  This function stops the periodic temperature compensation and CDMA LNA
  control.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rf_disable_cdma_lna_temp_comp( void );


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
);
#else
byte rf_hdet_adj_calc
(
  byte exp_agc_table_idx       /* index into the rf_cdma_exp_agc_table */
);
#endif


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

void rf_freeze_rx_agc_accum(rf_path_enum_type path);

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

void rf_unfreeze_rx_agc_accum(rf_path_enum_type path);


///dbc no longer used?
/*===========================================================================

FUNCTION RF_SET_RX_GAIN_COMP                                INTERNAL FUNCTION

DESCRIPTION
  Sets the RX_GAIN_COMP PDM level to the specified value.  RX_GAIN_COMP is
  an 8 bit PDM, so the software control input value is limited to the range
  0..255 so that the RX_GAIN_COMP calculation will saturate rather than
  over/underflow.  Since the PDM uses signed data, a conversion is performed
  from 0..255 software control to -128..+127 PDM hardware control.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
//void rf_set_rx_gain_comp
//(
//  int2 set_level        /* 8 bit software control value to load */
//);



#ifdef RF_HAS_GPS
#error code not present
#endif /* RF_HAS_GPS*/


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
void rf_freeze_cagc ( void );


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
void rf_unfreeze_cagc ( void );


#ifdef FEATURE_CDMA1X
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
void rf_set_cdma_lna_decision(rfcom_device_enum_type, lna_gain_state_type state );


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
  If an invalid gain state is requested the gain is not changed
  If successful the LNA Range pins are left in the overiden state

=============================================================================*/
void rf_set_cdma_lna_range(rfcom_device_enum_type, lna_gain_state_type state );
#endif //FEATURE_CDMA1X


/*===========================================================================

FUNCTION RF_CALC_NEW_RX_LIN                                 INTERNAL FUNCTION

DESCRIPTION
  Calculate new Rx linearizer with frequency and temperature compensation and
  then update rf_rx_lin_off[] and rf_rx_lin_slp[].

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  rf_rx_lin_off[] is updated.
  rf_rx_lin_slp[] is updated.

===========================================================================*/
void rf_calc_new_rx_lin(void);


#ifndef RF_HAS_MDSP_TX_AGC
/*===========================================================================

FUNCTION RF_CALC_NEW_TX_LIN                                 INTERNAL FUNCTION

DESCRIPTION
  Calculate new Tx linearizer with frequency and temperature compensation and
  then update rf_tx_lin_off[] and rf_tx_lin_slp[].

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  rf_tx_lin_off[] is updated.
  rf_tx_lin_slp[] is updated.

===========================================================================*/
void rf_calc_new_tx_lin(void);

#endif  /* RF_HAS_MDSP_TX_AGC */

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
void rf_reset_pa_thres( void );


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
void rf_adjust_pa_thres( int2 offset1, int2 offset2, int2 offset3 );

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
void rf_cagc_adjust_max_pwr_pa_thres( int2 offset );

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
void rf_load_pa_thres( void );

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
void rf_init_pa_ctl_4_txlinz( rfcom_device_enum_type device );

#else

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
void rf_init_pa_ctl_1_txlinz( void );

#endif /* !FEATURE_4PAGE_TX_LINEARIZER */


/*===========================================================================

FUNCTION RF_SET_TX_PWR_LIMIT                                INTERNAL FUNCTION

DESCRIPTION
  Sets the 10 bit TX_AGC_ADJ power limit in the MSM2P hardware to the
  specified value.  The input value is limited to 0..1023 so that the power
  limit calculations will saturate rather than overflow.  Since hardware
  uses an inverted two's complement format (-512 is max power, +511 is min
  power), this function will translate 0..1023 from software into
  +511..-512 for hardware.  This is used in CDMA mode only.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updates rf_tx_pwr_limit

===========================================================================*/
void rf_set_tx_pwr_limit
(
  int2 set_level        /* 10 bit limit value to load */
);

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
void init_nv_ptrs(void);

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
boolean is_gps_rf_config_sgps(void);


#ifdef FEATURE_ZIF_LOGGING
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

void  rf_init_zif_agc_logs ( void );

/*===========================================================================

FUNCTION RF_INIT_ZIF_IQ_LOGS

DESCRIPTION    This function is invoked to perform ZIF I/Q logging.

DEPENDENCIES   rf_iq_offsets_vars
               rf_dacc_iq_accum_vars

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/

void  rf_init_zif_iq_logs ( void );

#endif /*FEATURE_ZIF_LOGGING */

/*===========================================================================
FUNCTION RF_GET_MAX_ACTIVE_PA_SWITCHPOINT                                    

DESCRIPTION
  This function returns the maximum PA switchpoint active

DEPENDENCIES
  rf_r[123]_rise_val and rf_r[123]_fall_val must be updated.

RETURN VALUE
  Max active PA switchpoint.

SIDE EFFECTS
  

============================================================================*/
pa_switchpoint_type rf_cagc_get_max_active_pa_switchpoint(void);

#ifdef FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST
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
boolean rf_cagc_is_pa_thres_adjusted( void );


/*===========================================================================

FUNCTION       RF_CAGC_GET_CURR_PA_THRES_ADJ

DESCRIPTION    This API function provides the current adjustment to the
               PA switch (rise/fall) points

DEPENDENCIES   None.

RETURN VALUE   current offset used for PA switch point

SIDE EFFECTS   None.

===========================================================================*/
int1 rf_cagc_get_curr_pa_thres_adj( byte pa_switchpoint );

#endif /* FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST */


#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */

/*===========================================================================

FUNCTION RF_GET_RF_CONFIG
  
DESCRIPTION
  Reads and Returns the RF centric NV value NV_RF_CONFIG_I .

DEPENDENCIES
  None.

RETURN VALUE
  NV_RF_CONFIG_I.

SIDE EFFECTS
  None
  
===========================================================================*/
uint8 rf_get_rf_config(void);

/*===========================================================================

FUNCTION RF_GET_RF_HW_CONFIG
  
DESCRIPTION
  Reads and Returns the RF centric NV value NV_RF_HW_CONFIG_I .

DEPENDENCIES
  None.

RETURN VALUE
  NV_RF_CONFIG_I.

SIDE EFFECTS
  None
  
===========================================================================*/
uint8 rf_get_rf_hw_config(void);

/*===========================================================================

FUNCTION RF_GET_RF_BC_CONFIG
  
DESCRIPTION
  Reads and Returns the RF centric NV value NV_RF_BC_CONFIG_I .

DEPENDENCIES
  None.

RETURN VALUE
  NV_RF_CONFIG_I.

SIDE EFFECTS
  None
  
===========================================================================*/
uint64 rf_get_rf_bc_config(rf_path_enum_type);


/*===========================================================================

FUNCTION rf_wait_for_synth_lock
DESCRIPTION
  This function will invoke a wait loop for SYNTH lock active.  The maximum
  loop iteration is determined by the NV item NV_DIGITAL_PLL_LOCK_TIMER_I.
  
  For the secondary chain, max iterations will be waited since SYNTH signal 
  is configured only for primary chain.
  
DEPENDENCIES
  Uses definition RF_SYNTH_LOCK_ITER_WAIT_US
  rf_init() needs to be called to setup common nv table ptr.

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void rf_wait_for_synth_lock( rfcom_device_enum_type device );

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
uint4 rf_get_unused_synth_lock_time( rfcom_device_enum_type device );

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
uint4 rf_get_max_synth_lock_time( void );

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
int2 rf_get_path_1x_agc(rfcom_device_enum_type device);

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
void rf_config_pa_dvs( void );
#endif /* FEATURE_RF_PA_DVS */

/* EJECT  */


#ifdef FEATURE_CDMA1X
/*===========================================================================

FUNCTION rf_translate_band_class                       EXTERNAL FUNCTION

DESCRIPTION
    The purpose of this function is to translate the band class coming 
  in/out of the Rf driver to a different band (for testing). 

    For example: To test BC14 (US PCS + 5 MHz), if the RF driver is told 
  to go into Band Class 1 then this function will tell the RF driver to go 
  to Band class 14 instead. If the RF driver is in band class 14 and a client 
  wants to know what band class is the RF driver on then this function will
  tell the client that we are in band class 1 instead.
   
  NOTE: If no translation is performed then the return value is equal c_band
        (input band).
    
PARAMETERS  
  1) Parameter name: cmd
     - Specifies if the code calling this function is setting or reading 
       a band class to/from the RF driver. 

     - Parameter type: rf_translate_bc_type(enum).
       Valid Input:
           RF_TRANSLATE_BC_IN:  The code calling this function is setting  
                                the RF driver band class.
           RF_TRANSLATE_BC_OUT: The code calling this function is reading  
                                the RF driver current band class and returning
                                it to a client.

  2) Parameter name: c_band
     - Current band class.

     - Parameter Type: rf_card_band_type (enum)    
                      
DEPENDENCIES
  NV most have been read.
  
RETURN VALUE
  1) Returns the new band based on the current band (cBand). If no 
     translation is performed then the return value is equal cBand.

     - Parameter Type: rf_card_band_type (enum)  
  
SIDE EFFECTS
  None
==========================================================================*/
rf_card_band_type rf_translate_band_class(rf_translate_bc_type cmd, \
                                          rf_card_band_type    c_band);

/*===========================================================================

FUNCTION rf_get_valid_bc_and_mask                EXTERNAL FUNCTION

DESCRIPTION
    This function is used to index a table that contains the supported band
  classes and the corresponding system band class masks (defined in sys,h).
  This function should be called within a while loop, Example:

  while(rf_get_valid_bc_and_mask(index, &valid_band)
  {
     MSG_HIGH("Valid Band Class %d, Corresponding mask = %d", \
                                                    valid_band.bClass,
                                                    valid_band.bMask,
                                                    0);
  }    
    
PARAMETERS  
  1) Parameter name: index
     - While loop index/counter. 

     - Parameter type: uint8

  2) Parameter name: valid_bc_ptr
     - Structure pointer used to return the valid band class and its 
       corresponding system mask.

     - Parameter type: rf_bc_and_mask_type
       Valid Outputs: 
          2.1) Structure member: bc
               - Return the valid band class. Note: It will return all Fs if
                 index is outside the table boundary.
          
               Type: rf_card_band_type (enum)

          2.1) Structure member: mask
               - Return the corresponding band mask.Note: It will return 0 if
                 index is outside the table boundary.
          
               Type: sys_band_mask_type (enum)     
       
DEPENDENCIES
  None.
  
RETURN VALUE
  return type: boolean.
  - TRUE  = the index value passed by the user is within the table boundary.
  - FALSE = the index value passed by the user is outside the table boundary.
  
SIDE EFFECTS
  None               
==========================================================================*/
extern boolean rf_get_valid_bc_and_mask(uint8                index,     \
                                        rf_bc_and_mask_type* valid_bc_ptr);

#endif //FEATURE_CDMA1X



/*===========================================================================

FUNCTION RFI_ENABLE_SYNTH_GPIO

DESCRIPTION
  This function enables the GPIO used for synthesizer programming.

DEPENDENCIES
   None
  
RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void rfi_enable_synth_gpio(void);

/*===========================================================================

FUNCTION RFI_CHK_SYNTH_LOCK

DESCRIPTION
  This function checks the synthesizer lock signal.

DEPENDENCIES
   None
  
RETURN VALUE
   rfi_synth_status

SIDE EFFECTS
   None

===========================================================================*/
rfi_synth_status rfi_chk_synth_lock( rfi_synth_checker checker );

/*===========================================================================

FUNCTION RFI_TUNE_SYNTH

DESCRIPTION
  This function tunes the UHF synthesizer to the desired channel.

DEPENDENCIES
   None
  
RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void rfi_tune_synth( rfi_band_type band, uint16 chan, boolean tune_synth );

/*===========================================================================

FUNCTION rfi_calc_rx_freq_index  

DESCRIPTION
  This function calculates the new index for RX NV frequency compensation
  tables based on the channel.

DEPENDENCIES
   None
  
RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void rfi_calc_rx_freq_index( rfi_band_type band, uint16 chan );

/*===========================================================================

FUNCTION rfi_calc_tx_freq_index  

DESCRIPTION
  This function calculates the new index for TX NV frequency compensation
  tables based on the channel.

DEPENDENCIES
   None
  
RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void rfi_calc_tx_freq_index( rfi_band_type band, uint16 chan );

#ifdef FEATURE_WCDMA
#error code not present
#endif //FEATURE_WCDMA


/*===========================================================================

FUNCTION RFI_READ_NV_ITEMS

DESCRIPTION
  This function read the RF NV data.

DEPENDENCIES
   None
  
RETURN VALUE
   boolean

SIDE EFFECTS
   None

===========================================================================*/
boolean rfi_read_nv_items
(
rex_tcb_type  *task_ptr,              /* Calling task's TCB pointer */
rex_sigs_type task_wait_sig,          /* Task signal to wait for when
                                         reading NV */
void          (*task_wait_handler)( rex_sigs_type )
/* Task's wait function to be called
   when reading NV */
);

/*===========================================================================

FUNCTION RFI_INIT_RX_AGC_LIMIT

DESCRIPTION
  This function sets the RX AGC min and max values from NV data.

DEPENDENCIES
   None
  
RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void rfi_init_rx_agc_limit( void );

/*===========================================================================

FUNCTION RFI_INIT_LNA_PARAMS

DESCRIPTION
  This function loads the LNA parameters from NV data to MDSP's LNA parameters.

DEPENDENCIES
   None
  
RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void rfi_init_lna_params( void );

/*===========================================================================

FUNCTION RFI_INIT_PA_PARAMS

DESCRIPTION
  This function loads the PA parameters from NV data to MDSP's PA parameters.

DEPENDENCIES
   None
  
RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void rfi_init_pa_params( void );


/*===========================================================================

FUNCTION RFI_INIT_TX_DELAYS

DESCRIPTION
  This functions initialize tx delay parameters.

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void rfi_init_tx_delays( void );


/*===========================================================================

FUNCTION rfi_init_linearizer_table

DESCRIPTION
  This function initializes the AGC linearizer table in MDSP with NV data.
  
DEPENDENCIES
   RF NV items are read from NV.
  
RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void rfi_init_linearizer_table(void);

/*===========================================================================

FUNCTION rfi_update_linearizer_table

DESCRIPTION
  This function updates the AGC linearizer table with compensated table.
  The linearizer table is frequency compensated whenever frequency is changed
  and is temperature compensated every 10 seconds.
  
DEPENDENCIES
   RF NV items are read from NV.
  
RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void rfi_update_linearizer_table(void);

/*===========================================================================

FUNCTION RFI_UPDATE_LNA_OFFSETS

DESCRIPTION
  This function updates the MDSP's LNA offsets parameter with the LNA offset
  table from NV data based on the new frequency.

DEPENDENCIES
   rfi_calc_rx_freq_index() must be called before calling this function.
  
RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void rfi_update_lna_offsets( rf_path_enum_type rx_chain );

/*===========================================================================

FUNCTION RFI_UPDATE_VGA_GAIN_OFFSET

DESCRIPTION
  This function updates the MDSP's DVGA gain offset parameter with the DVGA gain
  offset from NV data based on the new frequency.

DEPENDENCIES
   rfi_calc_rx_freq_index() must be called before calling this function.
  
RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void rfi_update_vga_gain_offset( rf_path_enum_type rx_chain );

/*===========================================================================

FUNCTION RFI_ENABLE_TXDAC

DESCRIPTION
  This function enables the TX DAC circuit block.

DEPENDENCIES
   None
  
RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void rfi_enable_txdac( void );

/*===========================================================================

FUNCTION RFI_DISABLE_TXDAC

DESCRIPTION
  This function disables the TX DAC circuit block.

DEPENDENCIES
   None
  
RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void rfi_disable_txdac( void );

/*===========================================================================

FUNCTION RFI_TURN_OFF_PA

DESCRIPTION
  This function turns off PA.

DEPENDENCIES
   None
  
RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void rfi_turn_off_pa( void );


/*===========================================================================

FUNCTION RFI_INIT_TX_PWR_LIMIT

DESCRIPTION
  This function initialize the TX power limiting control.

DEPENDENCIES
   RF NV items are read from NV.
  
RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void rfi_init_tx_pwr_limit(void);

/*===========================================================================

FUNCTION RFI_ENABLE_TX_PWR_LIMIT

DESCRIPTION
   This function enables the TX power limit adjustment algorithm. 
   This function should be called when TX is enabled (rf_enable_????_tx).
   
DEPENDENCIES
  rfi_init_tx_power_limit must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfi_enable_tx_pwr_limit(void);


/*===========================================================================

FUNCTION RFI_DISABLE_TX_PWR_LIMIT

DESCRIPTION
   This function disables the TX power limit adjustment algorithm.
   This function should be called when TX is disabled (rf_disable_????_tx).
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfi_disable_tx_pwr_limit(void);


/*===========================================================================

FUNCTION RFI_UPDATE_HDET_DATA

DESCRIPTION
   Updates the rftxplim_hdet_off_adj and rftxplim_hdet_spn_adj value with values from NV.
   Call this function every time we tune to a new channel.
   
DEPENDENCIES
  rfi_init_tx_power_limit must be called first.
  frequency index to access frequency compensated table must be calculated.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfi_update_hdet_data( void );


/*===========================================================================

FUNCTION RFTXPLIM_DO_TX_PWR_LIMIT

DESCRIPTION
  This function read HDET value from ADC and TX AGC value from MDSP and then
  call the rftxplim_adjust_tx_pwr_limit function to adjust the TX power limit
  based on temperature, frequency, and HDET feedback. This function is called
  by rf_do_tx_pwr_limit every 100ms.

DEPENDENCIES
   rfi_init_tx_power_limit must be called first.
   temperature must be read and temperature data must be ready.
   frequency index must be calculated.
   
RETURN VALUE
  None

SIDE EFFECTS
  Updates MDSP TX power limit and PA range threshold
===========================================================================*/
int16 rfi_do_tx_pwr_limit( void );

/*===========================================================================

FUNCTION RFI_SET_TX_PWR_LIMIT

DESCRIPTION
  This function initialize the TX power limit value with passed in dbm value.

DEPENDENCIES
  RF Tx power limit is initialized.
  
RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void rfi_set_tx_pwr_limit(int16 db12, rfcom_txplim_channel_type chan_type);

/*===========================================================================

FUNCTION RFI_WRITE_NV_ITEMS_TO_MDSP

DESCRIPTION
  This function initialize MDSP parameters with data from RF NV items.

DEPENDENCIES
   RF NV items are read from NV.
  
RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void rfi_write_nv_items_to_mdsp(void);

/*===========================================================================

FUNCTION RFI_RESET_PA_RANGE_THRESH

DESCRIPTION
  This function reinitialize the PA range threshold variable with data from
  RF NV items. This function is used by TX power limiting.

DEPENDENCIES
   RF NV items are read from NV.
  
RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void rfi_reset_pa_range_thresh(void);

/*===========================================================================

FUNCTION RFI_ADJUST_PA_RANGE_THRESH

DESCRIPTION
  This function adjusts the PA range threshold variable with the pass in 
  offset. This function is used by TX power limiting.

DEPENDENCIES
   RF NV items are read from NV.
  
RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void rfi_adjust_pa_range_thresh( int16 offset );

/*===========================================================================

FUNCTION RFI_SET_PA_RANGE_THRESH

DESCRIPTION
  This function writes the PA range threshold variable into MDSP.
  This function is used by TX power limiting.

DEPENDENCIES
   RF NV items are read from NV.
  
RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void rfi_set_pa_range_thresh(void);

/*===========================================================================

FUNCTION RFI_ENABLE_TEMP_COMP

DESCRIPTION
  This function enable temperature compensation. It must be called by
  rf_enable_***_rx and rf_wakeup.
  
DEPENDENCIES
   RF NV items are read from NV.
  
RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void rfi_enable_temp_comp(void);

/*===========================================================================

FUNCTION RFI_DISABLE_TEMP_COMP

DESCRIPTION
  This function disable temperature compensation. It must be called by
  rf_sleep.
  
DEPENDENCIES
   RF NV items are read from NV.
  
RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void rfi_disable_temp_comp(void);

/*===========================================================================

FUNCTION RFI_UPDATE_TEMP_COMP_DATA

DESCRIPTION
  This function read thermistor to get the temperature and call
  a function to inerpolate a new temperature compensation value
  for each temperature compensation NV table.
  
DEPENDENCIES
   RF NV items are read from NV.
  
RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void rfi_update_temp_comp_data(void);

/*===========================================================================

FUNCTION RFI_CALC_TEMP_COMP_VAL

DESCRIPTION
  This function interpolates a new temperature compensation value for each
  temperature compensation NV table based on the new temperature.
  
DEPENDENCIES
   RF NV items are read from NV.
  
RETURN VALUE
   Temperature compensation value

SIDE EFFECTS
   None

===========================================================================*/
int16 rfi_calc_temp_comp_val(
                            int16 temp_table[], uint8 temp_index, uint8 temp_rem
                            );


/*===========================================================================

FUNCTION RFI_SELECT_ANTENNA

DESCRIPTION
  This function select the approriate antenna based on the mode.
  
DEPENDENCIES
   rf_init is called first.
  
RETURN VALUE
   none

SIDE EFFECTS
   None

===========================================================================*/
void rfi_select_antenna( rfi_band_type mode );

/*===========================================================================

FUNCTION RFI_GET_ENC_BTF_VAL

DESCRIPTION
  This function the encoder BTF (propagation delay from modulator to antenna)
  value in chip unit.
    
DEPENDENCIES
  rf_init is called.
  
RETURN VALUE
  uint16 - encoder BTF value in chip unit.

SIDE EFFECTS
  None

===========================================================================*/
uint16 rfi_get_enc_btf_val(void);

/*===========================================================================

FUNCTION RFI_GET_MAX_TX_POWER_NV_ITEM

DESCRIPTION
  This function returns the maximum TX power indicated by NV item depending
  on the control channel.
    
DEPENDENCIES
  rf_init is called.
  
RETURN VALUE
  int16 - The max TX power value.

SIDE EFFECTS
  None

===========================================================================*/
int16 rfi_get_max_tx_power_nv_item(rfcom_txplim_channel_type);

/*===========================================================================

FUNCTION RFI_GET_MAX_TX_POWER_NV_ITEM_DB12

DESCRIPTION
  This function returns the maximum TX power indicated by NV item depending
  on the channel type
    
DEPENDENCIES
  rf_init is called.
  
RETURN VALUE
  int16 - The max TX power value.

SIDE EFFECTS
  None

===========================================================================*/
int16 rfi_get_max_tx_power_nv_item_db12(rfcom_txplim_channel_type chan_type);

#ifdef FEATURE_WCDMA_MIN_INIT_TX_PWR
/*===========================================================================

FUNCTION RFI_GET_MIN_TX_POWER_NV_ITEM

DESCRIPTION
  This function returns the minimum TX power indicated by NV item depending
  on the channel type
    
DEPENDENCIES
  rf_init is called.
  
RETURN VALUE
  int16 - The min TX power value in db12.

SIDE EFFECTS
  None

===========================================================================*/
int16 rfi_get_min_tx_power_nv_item(void);
#endif

/*===========================================================================

FUNCTION RFI_GET_TX_ROTATOR_ANGLE

DESCRIPTION
  This function provides the Tx rotator angle values from RF NV.

DEPENDENCIES
  rf_init is called.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void rfi_get_tx_rotator_angle(
                                    uint16 *tx_rot_ang_pa_00_p,  /* Pointer to tx rotator angle pa state 00 */
                                    uint16 *tx_rot_ang_pa_01_p,  /* Pointer to tx rotator angle pa state 01 */
                                    uint16 *tx_rot_ang_pa_10_p,  /* Pointer to tx rotator angle pa state 10 */
                                    uint16 *tx_rot_ang_pa_11_p   /* Pointer to tx rotator angle pa state 11 */
                                    );

/*===========================================================================

FUNCTION rfi_get_out_of_service_thresh

DESCRIPTION
  This function returns the threshold for searcher out of service decision in dbm 
  unit. The value is retrieved from NV at power up. 
    
DEPENDENCIES
  rf_init is called first.
   
RETURN VALUE
  out of service thresh in dbm unit
   
SIDE EFFECTS
  None

===========================================================================*/
int16 rfi_get_out_of_service_thresh(void);

#ifdef FEATURE_RF_ENABLE_GTOW
/*===========================================================================

FUNCTION RFI_WRITE_NV_ITEMS_TO_MDSP_FOR_GTOW

DESCRIPTION
  This function initializes MDSP parameters necessary for GSM to WCDMA
  inter-RAT measurements with data from RF NV items.

DEPENDENCIES
  RF NV items are read from NV.
    
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void rfi_write_nv_items_to_mdsp_for_gtow( void );
#endif /* FEATURE_RF_ENABLE_GTOW */

/*===========================================================================

FUNCTION RFI_ENABLE_RF_GPIO

DESCRIPTION
  This function re-enable RF GPIO as GRFC. It should be called in rf_wakeup to 
  restore RF GPIO back as GRFC.
  

DEPENDENCIES
  none
    
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfi_enable_rf_gpio_in_wakeup(void);

/*===========================================================================

FUNCTION RFI_DISABLE_RF_GPIO

DESCRIPTION
  This function set RF GRFC as GPIO and set them to zero state. It should be called
  in rf_sleep.
  

DEPENDENCIES
  none
    
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfi_disable_rf_gpio_in_sleep(void);

/*===========================================================================

FUNCTION RFI_DISABLE_GSM_TXVCO_GPIO

DESCRIPTION
  This function set TX_VCO signal (GRFC 4 nad 5) to 1 to disable them.   

DEPENDENCIES
  none
    
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfi_disable_gsm_txvco_gpio(void);

/*===========================================================================

FUNCTION    rfi_do_filter_coarse_tune

DESCRIPTION
  This function invokes the filter coarse tune algorithm if necessary. 
  
PARAMETERS
  see below  
  
DEPENDENCIES
  SBI clock regime must be enabled.

RETURN VALUE
  None

SIDE EFFECTS
  None 

===========================================================================*/
uint8 rfi_do_filter_coarse_tune(
                               rex_tcb_type  *task_ptr,              /* Calling task's TCB pointer */
                               rex_sigs_type task_wait_sig,          /* Task signal to wait for when
                                                                        reading NV */
                               void          (*task_wait_handler)( rex_sigs_type ),
                               /* Task's wait function to be called
                                  when reading NV */
                               nv_items_enum_type nv_item            /* NV item to store the tuner result */
                               );

/*===========================================================================

FUNCTION     rfi_init_dc_converter_control

DESCRIPTION
  This function initializes the GPIO that control the DC-DC converter.
  

PARAMETERS
  none  

DEPENDENCIES
  rf_init and rf_init_wcdma is called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfi_init_dc_converter_control(void);

/*===========================================================================

FUNCTION     rfi_enable_dc_converter

DESCRIPTION
  This function enables the DC-DC converter that supply voltage to PA.
  

PARAMETERS
  none  

DEPENDENCIES
  rf_init and rf_init_wcdma is called.
  
RETURN VALUE
  None

SIDE EFFECTS
  Turn on a GPIO

===========================================================================*/
void rfi_enable_dc_converter(void);


/*===========================================================================

FUNCTION     rfi_disable_dc_converter

DESCRIPTION
  This function disables the DC-DC converter that supply voltage to PA.
  

PARAMETERS
  none  

DEPENDENCIES
  rf_init and rf_init_wcdma is called.
  
RETURN VALUE
  None

SIDE EFFECTS
  Turn off a GPIO

===========================================================================*/
void rfi_disable_dc_converter(void);


/*===========================================================================

FUNCTION    rfi_do_iq_resistor_cal

DESCRIPTION
  This function invokes the IQ line resistor calibartion if necessary. 
  
PARAMETERS
  see below
  
DEPENDENCIES
  SBI clock regime must be enabled.

RETURN VALUE
  none

SIDE EFFECTS
  None 

===========================================================================*/
extern void rfi_do_iq_resistor_cal(
                                  rex_tcb_type  *task_ptr,        /* Calling task's TCB pointer */
                                  rex_sigs_type task_wait_sig,    /* Task signal to wait for when
                                                                    reading NV */
                                  void          (*task_wait_handler)( rex_sigs_type )
                                  /* Task's wait function to be called
                                     when reading NV */
                                  );

/*===========================================================================

FUNCTION    rfi_do_uhf_vco_coarse_tuning

DESCRIPTION
  This function invokes UHF VCO coarse tuning.
  
PARAMETERS
  see below
  
DEPENDENCIES
  SBI clock regime must be enabled.

RETURN VALUE
  none

SIDE EFFECTS
  None 

===========================================================================*/
extern void rfi_do_uhf_vco_coarse_tuning(
                                        rfi_band_type rfi_band,     /* RF Band                    */
                                        rex_tcb_type  *task_ptr,    /* Calling task's TCB pointer */
                                        rex_sigs_type task_wait_sig,/* Task signal to wait for when
                                                        reading NV */
                                        void          (*task_wait_handler)( rex_sigs_type )
                                        /* Task's wait function to be called
                                          when reading NV */
                                        );


#ifdef FEATURE_RF_AFC

/*===========================================================================

FUNCTION  RFI_AFC_GET_VCO_DEFAULT

DESCRIPTION
  This function returns the vco accumulator default value.

DEPENDENCIES
  None

RETURN VALUE
  Default vco accumulator value in PDM counts.

SIDE EFFECTS
  None

===========================================================================*/
extern int16 rfi_afc_get_vco_default( void );

/*===========================================================================

FUNCTION  RFI_AFC_GET_VCO_SLOPE

DESCRIPTION
  This function provides the slope and range values for vco.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void rfi_afc_get_vco_slope
(
uint16 *vco_slope,
uint8 *vco_slope_range
);

/*===========================================================================

FUNCTION  RFI_AFC_GET_RGS_DATA

DESCRIPTION
  This function provides the data of recent good system, including
  system type, time, temperature, vco and rotator values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void rfi_afc_get_rgs_data
(
rfi_afc_rgs_data_type *rgs_data
);

/*===========================================================================

FUNCTION  RFI_AFC_UPDATE_RGS_DATA

DESCRIPTION
  This function updates the data of recent good system, including
  system type, time, temperature, vco and rotator values. Note that this
  function only updates the ram shadow. Separate function needs to be called
  when needed to save data into NV memory.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void rfi_afc_update_rgs_data
(
rfi_afc_rgs_data_type *rgs_data
);

/*===========================================================================

FUNCTION  RFI_AFC_SAVE_NV_DATA

DESCRIPTION
  This function saves the data into NV memory. nv_item_mask specifies which
  particular items to save. Multiple items can be specified by ORing the
  masks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean rfi_afc_save_nv_data
(
rfi_afc_nv_item_mask_type nv_item_mask,
/* Mask to specify nv items to save */
rex_tcb_type *caller_tcb_ptr,
/* Calling task's TCB pointer */
rex_sigs_type task_nv_wait_sig,
/* Task signal to wait for when reading NV */
void (*task_wait_func_ptr)( rex_sigs_type )
/* Task's wait function to be called when reading NV */
);

#endif /* FEATURE_RF_AFC */


#ifdef RF_HAS_LINEAR_INTERPOLATION

/* Current configuration has only one RF path
 */
#define RF_PATHS_TABLE_SIZ    2

/* Linear interpolation debugging code
 */
extern boolean rfi_interpolation_debug;

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
extern void rfi_update_freq_comp_items
(
rf_path_enum_type path,
rfi_band_type curr_band,
word channel                  /* input channel number */
);

/*===========================================================================

FUNCTION RFI_GET_FREQ_FROM_UARFCN                     EXTERNAL FUNCTION

DESCRIPTION
  This function handles channel numbering schemes for different bands.

DEPENDENCIES

RETURN VALUE
  Tx channel frequency in Khz.

SIDE EFFECTS
  None

===========================================================================*/
extern uint32 rfi_get_freq_from_uarfcn
(
rf_path_enum_type path,
rfi_band_type band,
word chan,                            /* input channel number */
rfi_uarfcn_ul_dl_type ul_dl
);

/*===========================================================================

FUNCTION RFI_APPLY_FREQ_COMP                            EXTERNAL FUNCTION

DESCRIPTION
  Applies interpolation calculation results to all affected frequency
  compensation items.

DEPENDENCIES
  rfi_update_freq_comp_items is already called.

RETURN VALUE
  none

SIDE EFFECTS
  Updates each freq comp values.

===========================================================================*/
extern void rfi_apply_freq_comp( rf_path_enum_type path );

/*===========================================================================

FUNCTION RFI_INTERPOLATION_ENABLE                         EXTERNAL FUNCTION

DESCRIPTION
  Returns the condition whether linear interpolation is enabled for
  frequency compensation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern boolean rfi_interpolation_enable( void );

#endif /* RF_HAS_LINEAR_INTERPOLATION */

/*===========================================================================

FUNCTION RFI_CAL_WCDMA_CGAGC_PARAMS                      EXTERNAL FUNCTION

DESCRIPTION
  This function initializes CGAGC parameters from NV
  
DEPENDENCIES
   None
  
RETURN VALUE
   boolean

SIDE EFFECTS
   None

===========================================================================*/
extern void rfi_init_wcdma_cgagc_params( void );

/*===========================================================================

FUNCTION RFI_GET_UTRAN_TX_LIM_VS_TEMP_OFFSET_VAL          EXTERNAL FUNCTION

DESCRIPTION
  Return rfi_utran_tx_lim_vs_temp_offset_val

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern int16 rfi_get_utran_tx_lim_vs_temp_offset_val( void );

/*===========================================================================

FUNCTION rfi_init_hs_pa_params

DESCRIPTION
  This function loads the HS PA parameters from NV data to MDSP's PA parameters.

DEPENDENCIES
   None
  
RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void rfi_init_hs_pa_params( void );

/*===========================================================================

FUNCTION RFI_SET_TX_PWR_LIMIT

DESCRIPTION
  This function gets the current value of Tx power limit in units of
  Tx AGC counts. (-512 to +511).


DEPENDENCIES

  
RETURN VALUE
 Tx power limit in units of TX AGC counts.

SIDE EFFECTS
   None

===========================================================================*/
extern int16 rfi_get_tx_pwr_limit(void);

/*===========================================================================

FUNCTION RF_GPIO_CONFIG_BASED_ON_BAND

DESCRIPTION
  This function configures GRFCs such as UMTS1900_PA_SEL
  for the desired band.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfi_gpio_config_based_on_band ( rf_path_enum_type rx_chain, rfi_band_type band );

/*===========================================================================

FUNCTION rfi_gpio_config_based_on_band_for_vco_coarse_tune

DESCRIPTION
  This function configures GRFCs such as LO_FB_BAND and UMTS1900_PA_SEL
  for the desired band. VCO coarse tune is special because it is initiated
  before MDSP is initialized. So any GRFCS (LO_FB_BAND) have to be set as 
  GPIO temporarily.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfi_gpio_config_based_on_band_for_vco_coarse_tune (rfi_band_type band );

/*===========================================================================

FUNCTION RFI_HDET_DETECT_ON

DESCRIPTION
  This function turns on/off HDET detect circuits.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void rfi_hdet_detect_on( boolean on_off );

#ifdef FEATURE_WCDMA_TX_PWR_BACK_OFF
/*===========================================================================

FUNCTION RFI_DO_VBATT_BASED_POWER_BACKOFF

DESCRIPTION
  This function reads the VBatt voltage and compares it with the three 
  'nv_power_backoff_voltages' values.  Depending on the current VBatt value,  
  the rfi_power_backoff variable is assigned to the nv_power_backoff_volt#.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfi_do_vbatt_based_power_backoff(void);

#endif /* FEATURE_WCDMA_TX_PWR_BACK_OFF */

/*===========================================================================

FUNCTION  RFI_SET_ANT_SEL_MODE

DESCRIPTION
  This function programs the antenna select signals as either GPIO or GRFC mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfi_set_ant_sel_mode( rfcom_ant_sel_mode_type antselmode );


/*===========================================================================

FUNCTION RFI_INIT_ANT_SEL_GPIO

DESCRIPTION
  This function initializes the antenna select GPIO output based on the current band and antenna select NV items.
  
DEPENDENCIES
   rf_init is called first.
  
RETURN VALUE
   none

SIDE EFFECTS
   None

===========================================================================*/
void rfi_init_ant_sel_gpio( rfi_band_type band );

/*===========================================================================

FUNCTION RFI_UHF_VCO_COARSE_TUNE

DESCRIPTION
  The function does the UHF VCO coarse calibration for RF chips. 

DEPENDENCIES
  sbi_init() and make sure RF LDOs are on.  

RETURN VALUE
  None

SIDE EFFECTS
  It updates RF device funtion pointers for chip calibration, not necessary 
  reflects the current RF band. One needs to switch RF device pointers after
  the cal.
===========================================================================*/
extern void rfi_uhf_vco_coarse_tune(
                                   rfi_band_type band,
                                   rex_tcb_type  *task_ptr,              /* Calling task's TCB pointer */
                                   rex_sigs_type task_wait_sig,          
                                   void (*task_wait_handler)( rex_sigs_type )
                                   );

/*===========================================================================

FUNCTION RFI_INIT_GPIO

DESCRIPTION
  This function initializes the RF GPIOs.

DEPENDENCIES
   None
  
RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
extern void rfi_init_gpio( void );

/*===========================================================================

FUNCTION RFI_DISABLE_RF_GPIO

DESCRIPTION
  This function set RF GRFC as GPIO and set them to zero state. It should be called
  in rf_sleep.
  

DEPENDENCIES
  none
    
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void rfi_disable_rf_gpio( void );

/*===========================================================================

FUNCTION RFI_DISABLE_GSM_TXVCO_GPIO

DESCRIPTION
  This function set TX_VCO signal (GRFC 4 nad 5) to 1 to disable them.   

DEPENDENCIES
  none
    
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void rfi_disable_gsm_txvco_gpio( void );

/*===========================================================================

FUNCTION RFI_OVERRIDE_PA_RANGE_VAL

DESCRIPTION
   This function overrides the MDSP PA range value with new settings.
       
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void rfi_override_pa_range_val( boolean enable_override );

#ifdef FEATURE_WCDMA
#error code not present
#endif //FEATURE_WCDMA


#ifdef RF_HAS_MDSP_TX_AGC
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
void rf_cdma_mdsp_update_tx_lin_tables(void);

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
void rf_cdma_mdsp_update_tx_comp( void );

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
void rf_cdma_mdsp_update_tx_lin_tables(void);

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
void rf_cdma_mdsp_update_tx_comp( void );

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
void rf_cdma_mdsp_init_tx_agc_settings (void);

#endif /* RF_HAS_MDSP_TX_AGC */

/*===========================================================================

FUNCTION RFI_GPIO_GPS_EXTERNAL_LNA_ON

DESCRIPTION
  This function configures external GPS LNA via GPIO(s).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfi_gpio_gps_external_lna_on( boolean ext_lna_on );

/*===========================================================================
FUNCTION rfi_tech_dependent_lna_switch

DESCRIPTION
  This function returns whether the HSDPA specific LNA switch points are needed  
  depending on RF target.
    
DEPENDENCIES
  rf_init is called.
  
RETURN VALUE
  TRUE if new HS swtichpoints are needed . FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
extern boolean rfi_tech_dependent_lna_switch ( void );

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
int2 rf_get_tx_lin_off_min(void);

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
int2 rf_get_tx_lin_off_max(void);

#if defined(RF_HAS_SET_RTR_TO_HI_Z)
#error code not present
#endif /* RF_HAS_SET_RTR_TO_HI_Z */

#endif /* RFI_H */

