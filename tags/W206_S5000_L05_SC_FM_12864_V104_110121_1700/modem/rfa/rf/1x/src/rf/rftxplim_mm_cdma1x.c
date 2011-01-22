 /*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                        RF Transmit Power Limit Services

GENERAL DESCRIPTION
   This file contains functions required to limit CDMA transmit power


EXTERNALIZED FUNCTIONS
   None


INITIALIZATION AND SEQUENCING REQUIREMENTS
   A callback function is used.


Copyright (c) 1998,1999,2000 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2001,2002,2003 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2004,2005,2006 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2007           by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*=================================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rftxplim_mm_cdma1x.c#7 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------------
10/20/08   vm      Added the support for HDET Compensation fix for Low Tier Builds
10/12/08   vm      Added support for HDET Waveform Compensation
06/13/08   vm      Updates for 16 bit HDET on QSC1100
06/11/08   sl      Added support for UINT16 HDET under new API FTM_GET_ALL_HDET_FROM_TX_SWEEP_CAL_V2=144
                   and under feature RF_HAS_HDET_UINT16.
12/06/08      ems     Support for QSC1100
12/05/07   vm/ans  Merged HDR DO power limitting change to mainline.
11/30/07   ans     HDET should only be disabled if 1.25 ISR is disabled.
11/06/07   vm      Added support for turning OFF SCH when SCH gain is 0
10/26/07   ans     Added FEATURE_P2_P3_DYNAMIC_PA_SWITCH_POINT_ADJUST for backoffs
                   for R2 and R3 PA states.
09/23/07   dyc     Wrap rf_get_exp_tx_agc_vs_hdet_for_hdr() in HDR featurization.
09/07/07   dyc     Warning fix for non-HWTC builds.
08/24/07   vm      Made the Change so that HDET is always turned ON 
                   for RF Cal Mode only 
08/07/07   hkk     Tured on thermistor comp also for HDR
07/16/07   hkk     Tweaks for DO Rev0 compesation
07/08/07   vm      Moved the debugging messages for HDR on Sc2X
07/02/07   vm      Fixed the featurization for disabling the HDET Waveform
                   compensation for HDR unless tested
06/29/07   vm      Revert back the change for Enabling HDET only
                   for 2 PCGs after every 100ms for making the
                   reading.
                   Added Debugging messages for Beta Scaling on Sc2X
06/27/07   vm      Added the support for Beta scaling for HDR
06/27/07   hkk     Featurized HDR HDET compensation and turned it off. 
06/03/07   hkk     1x, DO and temp waveform compensation
06/05/07   vm      Backed out the change for Max Power saturation
                   for Sc2X
06/05/07   vm      Revert Back the change for Internal HDET support Warmup delay
                   Added More support for Beta Scaling on Sc2X
05/29/07   bmg     Added HDET warmup delay when using RF_HAS_INTERNAL_HDET_SUPPORT
04/30/07   dyc     Enable HDET temp and RF compensation
03/30/07   bmg     Removed RTR6500 from rf_read_hdet() function, ADC orphans
                     enable reading it directly from adc_read()
                   Added RF_HAS_INTERNAL_HDET_SUPPORT feature to warm up the
                     RTR6500/SC2x internal HDET circuit before taking the HDET
                     reading
03/20/07   sar     Updated rf_read_hdet() function to include RTR6275_1X.
03/14/07   rmd     Corrected array size check within rf_get_exp_tx_agc_vs_hdet().
03/14/07   rmd     Added Radio Configuration and temperature compensation for
                   HDET.
03/09/07   bmg     RTR6500 HDET support
02/08/07   vm/adk  Added support for firmware TX AGC
10/17/06   ycl     Modifications for initial MSM7600 build.
06/06/06   vm      Added the support for Backoff for Maximum Transmit
                   Power for 1xEVDO RevA
12/14/05   sar     Eliminated lint errors.
10/18/05   bmg     Update global HDET reading variable in HDR power limiting
                   (debug screen and HDET logging)
06/10/05   dbc     Fixed lint warnings
05/09/05   ra      For DORA, added RF_USE_TRAMP_PIC_ISR
03/09/05   ra      Removed interrupt clear for 2nd level interrupts because tramp
                   api will clear.
02/16/05   ra      Added TRAMP change to support Modem 2nd level interrupts
02/04/05   ra      Added support for HDR Max Power limiting on targets with 
                   2nd level interrupts in the encoder design.
01/05/05   ra      DORA merge
12/22/04   jac     Added setting enable and clear bits for the TX SLOT interrupt for
                   the 125 MS interrupt.
09/14/04   dbc     Get the HDET ADC channel by accessing a rf_card structure API
                   function in rf_125_ms_isr() and rf_hdr_slot_isr()
09/13/04   dbc     Added a check in rf_125_ms_isr() and rf_hdr_slot_isr() for the 
                   FZRF CCA to pick the correct HDET MUX input. 
07/26/04    et     added interpolation support for HDR in 
                   rf_hdr_adjust_pwr_limit()
06/04/04   jac     Added a check in rf_125_ms_isr() and rf_hdr_slot_isr() for the 
                   KZRF CCA to pick the correct HDET MUX input.
05/21/04   sar     Added support for Enhanced Bands Type in rf_125_ms_isr().
05/21/04   dbc     Updates for interaction with PA Backoff module
04/12/04   dyc     Updated with FEATURE_TRIMODE_ITEMS
03/19/04   et      added interpolation (frequency comp) support
02/26/04   kss     Minor update to HDR HDET comments.
02/20/04   kss     Added functions to support using HDET in HDR.
02/18/04   aks     Featurize the use of the HDET algorithm.
01/30/04   et      Removed usage of RF_HAS_TRIMODE.  Replaced by
                   FEATURE_TRIMODE_ITEMS.
11/21/03   dbc     Subtract rf_gl_adj_accumulator from agc_error when 
                   FEATURE_TX_POWER_BACK_OFF is defined
10/20/03   dyc     Remove FEATURE_HWTC
09/03/03   dbc     Use global RF variable rf_chain_status[] to access RF state
                   related variables
02/25/03   dbc     Merge changes for FEATURE_TX_POWER_BACK_OFF from MSM51xx archive.
                   Made rf_tx_pwr_limit global. 
01/13/03   aks     Changes for GSM PLT.
12/01/02   dbc     Changed PA adjustment in rf_adjust_cdma_pwr_limit() to adjust
                   adjusted PA offsets when FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST
                   is defined.  
09/23/02   dbc     Remove synth lock check from rf_125_ms_isr()
09/20/02   dbc     Moved rf_chk_synth_lock in rf_125_ms_isr()
09/04/02   dbc     In rf_adjust_cdma_pwr_limit() if the filtered agc error is 0 
                   converge using the unfiltered error value.
07/26/02   dyc     moved the synth lock check to the 125_ms_isr so that
                   it doesn't fail during puncturing of the pll.
                   (merged from et MSM6000_RF.01.00.15)
07/17/02   dyc     Modified 102 dB dynamic range support to use run time detection.
                   Removed usage of PA_ON2 bit for MSM6050.
                   (merged from dbc rf.01.00.13)
05/26/02   jwh     Renamed PHONE_T as FEATURE_HWTC.
05/22/02   dbc     Added changes for 102 dB dynamic range.
05/20/02   dbc     RF_PA_RANGE_CNT_PER_DB should be 3 instead of 4 for 85 dB 
                   linearizer range
04/29/02   dbc     Changed rf_adjust_cdma_pwr_limit() to use rf_exp_agc_table_ptr to
                   access expected hdet vs. agc instead of direct table access.
01/14/02   dbc     Fixed problem where rf_125_ms_isr() was not checking PA_ON2 for 
                   PCS mode. Updated adc reads to use new interface (MSM6000+).
11/21/01   dbc     RF Driver decomposition changes.
11/02/01   dbc     Removed backwards compatible (pre MSM6000) code
10/05/01   aks     Merged in phone_t changes.
10/01/01   dbc     Modified rf_set_tx_pwr_lim() to work with MSM6000+ target register
                   definition. Added initial support for the new PMIC API.
09/11/01   aks     Made rf_set_tx_pwr_limit() as a shared internal.
07/02/01   aks     Created from re-organization of rfmsm.c, rev 1.10.
                   O:/src/asw/COMMON/vcs/rfmsm.c_v   1.10   21 May 2001 16:40:16
===================================================================================*/

#include "target.h"
#include "customer.h"

#ifndef FEATURE_GSM_PLT   

#include "comdef.h"
#include "db.h"
#include "rfi.h"
#include "rfifreq.h"
#include "cagc.h"
#include "err.h"
#include "adc.h"
#include "rfc.h"
#include "rft.h"

#ifdef RF_HAS_MDSP_TX_AGC
#include "rf_mdsp_tx_agc.h"
#endif

#ifdef RF_HAS_HDR_REVA_TX_POWER_BACK_OFF
#error code not present
#endif

#ifdef FEATURE_FACTORY_TESTMODE
#include "ftm.h"
#endif

#if !defined(RF_HAS_HDET_BYPASS)
#include "tramp.h"
#endif

#if defined(RF_HAS_HDET_COMP_FOR_RC_AND_TEMP)
#include "txctraffic.h"
#include "enc.h"
#ifdef RF_HAS_HDR
#error code not present
#endif
#endif /* RF_HAS_HDET_COMP_FOR_RC_AND_TEMP */

#include "rftxplim_mm_cdma1x.h"


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

                      RF COMPENSATION CONSTANTS

=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/*---------------------------------------------------------------------------
        CONFIGURATION CONSTANTS FOR EXTENDED RANGE OF RF AGC LINEARIZER
---------------------------------------------------------------------------*/


#define RF_TX_LIM_CNT_PER_DB_102      10  /* Tx AGC limit is in 1/10 dB    */
#define RF_PA_RANGE_CNT_PER_2DB_102    5  /* PA Range is in 2/5 dB         */
#define RF_PA_RANGE_CNT_PER_5DB_102   12  /* 24/5 dB = 4.8 dB              */ 
#define RF_TX_LIM_CNT_PER_DB          12  /* Tx AGC limit is in 1/12 dB    */
#define RF_PA_RANGE_CNT_PER_DB         3  /* PA Range is in 1/3 dB         */

#ifdef RF_HAS_HDR_REVA_TX_POWER_BACK_OFF
#error code not present
#endif

#ifdef RF_HAS_MDSP_TX_AGC
#define RF_CDMA_MDSP_10_TO_16_BIT( val )           ((val) << 6)
#endif


#if defined(RF_HAS_HDET_COMP_FOR_RC_AND_TEMP)
/* How many times we need to call rf_get_exp_tx_agc_vs_hdet()
   before we read the HDET temperature from HDET.
   Note: rf_get_exp_tx_agc_vs_hdet() gets call every
         100 msec when we are close to TX MAX power. */
#define RF_REFRESH_HDET_COMP_THERM_COUNT 50
#endif /* RF_HAS_HDET_COMP_FOR_RC_AND_TEMP */

//#define RF_HAS_SC2X_BETA_SCALING_DEBUG

/* -------------------------------------------------------------------------

                            Local Data

---------------------------------------------------------------------------- */

/* CDMA Tx limiter loop gain, scaled by RF_CDMA_TX_LIM_ADJ_SCALE.
*/
LOCAL int2    rf_cdma_tx_lim_adj_gain = RF_CDMA_TX_LIM_ADJ_SCALE / 4;

/* last software setting of CDMA Tx power limit
** this is 0..1023 min..max while the hardware is +511..-512 min..max
*/
int2 rf_tx_pwr_limit;
extern word txc_sch_gain;

#ifdef FEATURE_FACTORY_TESTMODE
extern ftm_mode_type ftm_mode;
#endif

#ifdef FEATURE_TX_POWER_BACK_OFF
LOCAL boolean rf_hdet_was_disabled = FALSE ;
extern boolean rf_disable_hdet;
extern boolean rf_digital_backoff_active;
extern int16 rf_pa_backoff_accumulator;
boolean sampled_rf_digital_backoff_active;
extern boolean tx_power_limit_update_pending;
int16 sampled_rf_pa_backoff_accumulator;
extern int16 current_hdet_offset;
int16 sampled_current_hdet_offset;
#define RF_TX_LIM_ADJ_FRAME_DELAY    ((int4)20)
#endif

#ifdef RF_HAS_HDR_REVA_TX_POWER_BACK_OFF
#error code not present
#endif

#if defined(RF_HAS_HDET_COMP_FOR_RC_AND_TEMP)
/* This structure is used to save the data required for 
   HDET Compensation. */
rfc_hdet_comp_cfg_type rftxplim_hdet_comp_config = \
{  CAI_RC_1,
   ENC_SCH_FULL_RATE,
   ENC_FULL_RATE,
   FALSE,
   FALSE,
   FALSE,
   TRUE,
    0,
   RF_BC0_BAND,
   RF_HDET_COMP_ENABLE,
   0
};

#ifdef RF_HAS_HDR
#error code not present
#endif

/* This is a counter used keep track of how often we
   need to read the HDET therm (from ADC). */
static uint16 rf_read_hdet_temp_counter = 0;

#endif /* RF_HAS_HDET_COMP_FOR_RC_AND_TEMP */

#ifdef RF_HAS_INTERNAL_HDET_SUPPORT
/* hdet_warmup_counter is used to skip PCGs until the HDET detector has
   had sufficient time to settle */
static uint8 hdet_warmup_counter = 0;
#ifdef RF_HAS_HDR
#error code not present
#endif
#endif /* RF_HAS_INTERNAL_HDET_SUPPORT */

/* -------------------------------------------------------------------------

                       LOCAL FUNCTION PROTOTYPES 

---------------------------------------------------------------------------- */
#if defined(RF_HAS_HDET_COMP_FOR_RC_AND_TEMP)
static boolean rf_update_hdet_temp_value_needed(void);
#endif /* RF_HAS_HDET_COMP_FOR_RC_AND_TEMP */

extern void rf_get_tx_min_max_pwr( rf_card_band_type  rf_curr_cdma_band,
                                   int *p_tx_min_pwr, int *p_tx_max_pwr );

#if (!defined(RF_HAS_HDET_BYPASS)) && (defined(RF_HAS_INTERNAL_HDET_SUPPORT))
boolean rf_current_mode_is_ftm_rf_cal(void);
#endif



/* -------------------------------------------------------------------------

                       LOCAL FUNCTION DEFINITIONS

---------------------------------------------------------------------------- */

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
  16-bit HDET reading

SIDE EFFECTS
  None

===========================================================================*/
#ifdef RF_HAS_HDET_UINT16
uint16 rf_read_hdet( rf_card_band_type band )
#else
uint8 rf_read_hdet( rf_card_band_type band )
#endif
{
   extern rf_path_0_config_type rf_path_0;  

   if ( rf_path_0.rftdev == RTR6285_1X || rf_path_0.rftdev == RTR6275_1X )
   {
      return rft_get_hdet( RF_PATH_0 );
   }
   else
   {  
      #ifdef RF_HAS_HDET_UINT16
      return (uint16)adc_read( rfc_get_hdet_adc_channel( rf_chain_status[RF_PATH_0].rf_curr_cdma_band ) );
      #else
      return (byte)adc_read( rfc_get_hdet_adc_channel( rf_chain_status[RF_PATH_0].rf_curr_cdma_band ) );
      #endif
   }
}


/*===========================================================================

FUNCTION RF_GET_CDMA_TX_PWR_LIMIT                       EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns the CDMA Tx power limit setting in 10 bit binary
  format.  Since the hardware register is not directly readable, this
  functions returns a copy of the last setting performed by the RF unit.
  If 102 dB dynamic range is used the value is converted from the 102 dB
  dynamic range to the expected 85 dB range.

  The returned value is interpreted as follows:

    Returned value               0........1023
    Tx power limit          -52.33........+33dBm
    Tx power limit register   +511........-512

DEPENDENCIES
  None

RETURN VALUE
  word = 10 bit Tx power limit value

SIDE EFFECTS
  None

===========================================================================*/
word rf_get_cdma_tx_pwr_limit( void )
{

  word tx_pwr_lim_temp = (uint16)rf_tx_pwr_limit;

  /* Rescale down to 85 dB dynamic range if CAGC is in 102 dB dynamic range */
  if ( rf_get_cagc_linearizer_range() == RF_HAS_102DB_CDMA_DYNAMIC_RANGE ) 
    tx_pwr_lim_temp = rf_rescale_tx_pwr_limit( tx_pwr_lim_temp );

  return (tx_pwr_lim_temp);

} /* rf_get_cdma_tx_pwr_limit */



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
)
{
  int2 temp_level;      /* temp for rf_tx_pwr_limit format conversion */
 
  #ifdef RF_HAS_HW_QSC60X5
  #ifdef RF_HAS_MDSP_TX_AGC
  #ifdef RF_HAS_BETA_SCALING_TX_AGC
  int2 pa_rise_val[RF_NUM_TX_LINEARIZERS - 1];
  int2 temp_level_sat;  
  int tx_min_pwr;
  int tx_max_pwr;
  rf_chain_state_type *rf_chain_ptr = 
        &rf_chain_status[ rfcom_to_path[ RFCOM_TRANSCEIVER_0 ] ];
  byte index = 0;
  #endif
  #endif
  #endif
 
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* limit to 10 bit control range 0..1023 */
  rf_tx_pwr_limit = RF_SATURATE( set_level,
                                 RF_TX_PWR_CALC_MIN,
                                 RF_TX_PWR_CALC_MAX );
  temp_level = rf_tx_pwr_limit;

 
  #ifdef RF_HAS_HW_QSC60X5
  #ifdef RF_HAS_MDSP_TX_AGC
  #ifdef RF_HAS_BETA_SCALING_TX_AGC
  pa_rise_val [0] = (int2) ((word) rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_r1_rise);

  pa_rise_val [1] = (int2) ((word) rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_r2_rise);

  pa_rise_val [2] = (int2) ((word) rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_r3_rise);

  while (index < (RF_NUM_TX_LINEARIZERS - 1))
  {
    if (pa_rise_val[index] == RF_CDMA_MDSP_PA_SWITCHPOINT_INVALID_VAL)
    {
      break;
    }
    else
    {
      index++ ;
    }
  }

  #ifdef RF_HAS_LINEAR_INTERPOLATION
  temp_level_sat = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_tx_lin_tbl[index][0] + rf_freq_comp.i_lin_freq_comp[index];
  #else
  temp_level_sat = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_tx_lin_tbl[index][0] + ((int2) rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_tx_comp_vs_freq[index][rf_chain_ptr->rf_freq_index]);
  #endif  

  rf_get_tx_min_max_pwr( rf_chain_ptr->rf_curr_cdma_band,
                         &tx_min_pwr, &tx_max_pwr );     

  temp_level_sat = temp_level_sat - ((int2)tx_min_pwr);

  temp_level = RF_BETA_SCALING_MIN_VALUE(temp_level, temp_level_sat); 

  rf_tx_pwr_limit = temp_level;
  #endif /*RF_HAS_BETA_SCALING_TX_AGC*/
  #endif /*RF_HAS_MDSP_TX_AGC*/
  #endif /*RF_HAS_HW_QSC60X5*/
  

  /* convert 0..1023 control range to +511..-512 hardware range */
   temp_level = RF_TX_PWR_REG_MAX - temp_level;

#ifdef HDET_DEBUG
   MSG_HIGH("HDET_DEBUG:RF CDMA Tx Power limit (%d)", temp_level, 0, 0);
#endif
   
  /* Write 10 bits to CAGC TX_AGC_ADJ limit hardware port.
  ** Writing to this register will cause the hardware to pick up the new
  ** value (all 10 bits).
  */
  
  #ifdef RF_HAS_MDSP_TX_AGC
  temp_level = (int2)(RF_CDMA_MDSP_10_TO_16_BIT( temp_level ));
  CDMA_MDSP_SET_TX_MAX_PWR( temp_level );
  #else
  HWIO_OUTM( TX_GAIN_LIMIT_WR, HWIO_RMSK(TX_GAIN_LIMIT_WR),(word) temp_level);
  #endif  
} /* rf_set_tx_pwr_limit */

#if !defined(RF_HAS_HDET_BYPASS)

/*===========================================================================

FUNCTION RF_ADJUST_CDMA_PWR_LIMIT                           INTERNAL FUNCTION

DESCRIPTION
  Recalculates the CDMA power limit based on the frequency and the current
  values for HDET, TX_AGC_ADJ setting and temperature.  This
  function is activated every RF_TX_LIM_ADJ_REPEAT_TIME ms.  An error is
  generated if the synthesizers are not in lock.  After these calculations are
  performed, this function schedules a RF_TX_LIM_ADJ_REPEAT_TIME ms call back
  to rf_enable_125_ms_isr() to start another data collection cycle.  This is
  used in CDMA mode only.

DEPENDENCIES
  rf_cdma_tx_lim_vs_temp_now and rf_cdma_tx_lim_vs_freq[] must have
  been loaded from NV storage.

  rf_cdma_exp_agc_table[] must be setup.

  Temperature data must be set up.

  rf_tx_agc_adj, rf_hdet_data, and rf_freq_index must be setup
  rf_tx_pwr_limit must match current Tx power limit setting

RETURN VALUE
  None

SIDE EFFECTS
  Updates rf_tx_pwr_limit, and rf_pwr_call_back

===========================================================================*/
LOCAL void rf_adjust_cdma_pwr_limit
(
  int4 ms_interval          /* time interval since last call */
  /* Tell lint to ignore the unused parameter for now. */
  /*lint -esym(715,ms_interval) */
)
{
  int2 agc_error;           /* Tx AGC error                   */
  int2 pa_range_adj;        /* PA Range adjustment            */
  int2 filtered_agc_error;  /* Filtered Tx AGC error          */

  #ifdef RF_HAS_SC2X_BETA_SCALING_DEBUG 
#error code not present
  #endif
  
#if (!defined(RF_HAS_LINEAR_INTERPOLATION)) || (defined (RF_HAS_SC2X_BETA_SCALING_DEBUG))
  rf_chain_state_type *rf_chain_ptr = 
        &rf_chain_status[ rfcom_to_path[ RFCOM_TRANSCEIVER_0 ] ];
#endif

   #ifdef RF_HAS_SC2X_BETA_SCALING_DEBUG 
#error code not present
   #endif

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  /* If Tx is on, then perform power adjustment.
  */
  if ( rf_tx_status == RF_TX_IS_ON )
    {
#ifdef FEATURE_TX_POWER_BACK_OFF
    
    /* If PA Backoff module is in the process of updating the TX power limit or
    ** the power limit has been changed since we sampled it we'll wait until the 
    ** next frame boundary to reacquire the current settings and run again.
    */
    if ( ( tx_power_limit_update_pending ) || 
         ( rf_pa_backoff_accumulator != sampled_rf_pa_backoff_accumulator ) 
       )
    {
        /*lint -save -e717 */
        clk_reg( &rf_pwr_call_back,         /* pointer to call back structure   */
                 rf_enable_125_ms_isr,      /* address of function to call back */
                 RF_TX_LIM_ADJ_FRAME_DELAY, /* time to wait till call back      */
                 (int4)0,                   /* delay between repeat call backs  */
                 RF_CALL_BACK_ONCE );       /* do only one call back            */
        /*lint -restore */
        return ;
    }

    /*  Inhibit HDET correction during certain channel configurations. */
    if( rf_disable_hdet == TRUE )
    {
      rf_hdet_was_disabled = TRUE ;
    }
#endif

    /* ----------------------------------------------------------------
    ** If the HDET value is above the HDET offset, we are either in the
    ** HDET adjust range, or above it.  If in-range, adjust Tx AGC.  If
    ** above the range, bring the limit down.
    ** ---------------------------------------------------------------- */

    if ( (rf_hdet_data >= rf_hdet_off_adj )
#ifdef FEATURE_TX_POWER_BACK_OFF
         || (rf_hdet_was_disabled == TRUE )
#endif
                                            ){

      /* ------------------------------------------------------------------
      ** If within ~-6,+2dB of the Tx power limit (~+17,+25dBm), adjust the
      ** Tx limit.  HDET is only highly accurate in this range, so we only
      ** adjust the limit when operating in this range.
      ** ------------------------------------------------------------------ */

      if (((rf_hdet_data - rf_hdet_off_adj) <= rf_hdet_spn_adj )
#ifdef FEATURE_TX_POWER_BACK_OFF
         || (rf_hdet_was_disabled == TRUE )
#endif
                                            ){

        /* --------------------------------------------------------
        ** Calculate new power limit =
        **     AGC error based on measured power (HDET)
        **     + Correction based on temperature
        **     + Correction based on frequency
        **
        ** AGC error based on measured power (HDET) =
        **     Tx AGC - Expected Tx AGC based on HDET
        ** -------------------------------------------------------- */

#ifdef FEATURE_TX_POWER_BACK_OFF
        /*
            When reverting to a channel configuration that doesn't
            require inhibiting the max power correction, wait until
            the next frame before modifying the tx limit.
        */
        if( (rf_hdet_was_disabled == TRUE) && (rf_disable_hdet != TRUE) )
        {
          rf_hdet_was_disabled = FALSE ;

          /*lint -save -e717 */
          clk_reg( &rf_pwr_call_back,         /* pointer to call back structure   */
                   rf_enable_125_ms_isr,      /* address of function to call back */
                   RF_TX_LIM_ADJ_FRAME_DELAY, /* time to wait till call back      */
                   (int4)0,                   /* delay between repeat call backs  */
                   RF_CALL_BACK_ONCE );       /* do only one call back            */
          /*lint -restore */
          return ;
        }
        else if( rf_disable_hdet == TRUE )
        {
          agc_error = 0 ;
        }
        else
        {
          /* -----------------------------------------------------------------
          ** Compare what HDET is telling us to the gain limit reading
          ** ---------------------------------------------------------------- */

          /* -----------------------------------------------------
          ** Calculate Tx AGC error based on HDET (measured power)
          **
          ** Tx AGC value - Tx AGC Offset - Expected Tx AGC
          ** ----------------------------------------------------- */
          #if defined(RF_HAS_HDET_COMP_FOR_RC_AND_TEMP)
          agc_error = (rf_tx_agc_adj - RF_TX_AGC_OFF) - \
                      rf_get_exp_tx_agc_vs_hdet(rf_hdet_data);
          #else
          agc_error = (rf_tx_agc_adj - RF_TX_AGC_OFF) - \
                      *(rf_exp_agc_table_ptr+rf_hdet_data);
          #endif /* RF_HAS_HDET_COMP_FOR_RC_AND_TEMP */

          /* -----------------------------------------------------------------
          ** Add in the temperature compensated base value for rf_tx_pwr_limit
          ** ----------------------------------------------------------------- */
          agc_error +=
#ifdef RF_HAS_LINEAR_INTERPOLATION
            rf_freq_comp.i_tx_lim_vs_freq;
#else
            (int2) rf_chain_ptr->rf_nv_dig_items_ptr-> \
                     rfnv_cdma_tx_lim_vs_freq[ rf_chain_ptr->rf_freq_index ];
#endif

          /* If we are applying a digital backoff the TX AGC reading is biased.  Since 
          ** the hdet reading is not biased we need to remove this "intentional" error.
          */ 
          if ( sampled_rf_digital_backoff_active ) 
          {
            if ( rf_get_cagc_linearizer_range() == RF_HAS_102DB_CDMA_DYNAMIC_RANGE )
              agc_error -= RF_DIGITAL_GAIN_FACTOR_102_DB;
            else
              agc_error -= RF_DIGITAL_GAIN_FACTOR_85_DB;
          }

          /* add in the HDET offset (if any ) */
          agc_error += sampled_current_hdet_offset;

        }

#else
        /* -----------------------------------------------------------------
        ** Compare what HDET is telling us to the gain limit reading
        ** ---------------------------------------------------------------- */

        /* -----------------------------------------------------
        ** Calculate Tx AGC error based on HDET (measured power)
        **
        ** Tx AGC value - Tx AGC Offset - Expected Tx AGC
        ** ----------------------------------------------------- */

         #if defined(RF_HAS_HDET_COMP_FOR_RC_AND_TEMP)
         agc_error = (rf_tx_agc_adj - RF_TX_AGC_OFF) - \
                      rf_get_exp_tx_agc_vs_hdet(rf_hdet_data);
         #else
         agc_error = (rf_tx_agc_adj - RF_TX_AGC_OFF) - \
                     *(rf_exp_agc_table_ptr+rf_hdet_data);
         #endif /* RF_HAS_HDET_COMP_FOR_RC_AND_TEMP */

        /* -----------------------------------------------------------------
        ** Add in the frequency compensation
        ** ---------------------------------------------------------------- */
        agc_error +=
#ifdef RF_HAS_LINEAR_INTERPOLATION
        rf_freq_comp.i_tx_lim_vs_freq;
#else
          (int2) rf_chain_ptr->rf_nv_dig_items_ptr-> \
                   rfnv_cdma_tx_lim_vs_freq[ rf_chain_ptr->rf_freq_index ];
#endif

#endif

        /* -----------------------------------------------------------------
        ** Add in the temperature compensated base value for rf_tx_pwr_limit
        ** ----------------------------------------------------------------- */
        agc_error += rf_cdma_tx_lim_vs_temp_now;

#ifdef FEATURE_TX_POWER_BACK_OFF
        /* If we have applied a back off, reduce the agc_error by the back off.
        ** This is needed because the current rf_tx_pwr_limit has this amount
        ** of adjustment in it.
        */
        agc_error -= sampled_rf_pa_backoff_accumulator;
#endif
        /* -------------------------------------------------------
        ** Reduce the loop gain by a factor of
        ** ( rf_cdma_tx_lim_adj_gain / RF_CDMA_TX_LIM_ADJ_SCALE ),
        ** and then add it to the current CDMA Tx power limit.
        ** ------------------------------------------------------- */
        filtered_agc_error  = (agc_error - rf_tx_pwr_limit);
        filtered_agc_error *= rf_cdma_tx_lim_adj_gain;
        filtered_agc_error /= RF_CDMA_TX_LIM_ADJ_SCALE;

        /* ------------------------------------------------------------
        ** If the error is below the gain, converge
        ** using the unfiltered error value.
        ** ------------------------------------------------------------ */
        if ( filtered_agc_error == 0 )
          filtered_agc_error = (agc_error - rf_tx_pwr_limit);


        /* ------------------------------------------------------------
        ** Sum the filtered error with the current CDMA Tx power limit,
        ** and set the new Tx power limit
        ** ------------------------------------------------------------ */
          /* update the TX power limit */
        rf_set_tx_pwr_limit( filtered_agc_error + rf_tx_pwr_limit );

        #ifdef RF_HAS_MDSP_TX_AGC
        CDMA_MDSP_NEW_TX_LIMIT_FRAME_PARAMS_AVAILABLE();
        #endif 
       
        /* --------------------------------------------------
        ** Change R1_RISE/FALL and R3_RISE based on AGC error
        **
        ** The PA Range controls are on an 8-bit scale unlike
        ** the 10-bit scale used for Tx Power Limit.  Error
        ** accumulation from adding filtered_agc_error/4 would
        ** cause the PA Range accumulators to gather error,
        ** unlike the Tx Power Limit which is governed by a
        ** control loop which removes accumulated error.
        ** -------------------------------------------------- */

        /* Calculate error
        */
        pa_range_adj =
            rf_tx_pwr_limit
          - rf_cdma_tx_lim_vs_temp_now/*lint -e834 recommend using brackets */
#ifdef RF_HAS_LINEAR_INTERPOLATION
          - rf_freq_comp.i_tx_lim_vs_freq;
#else
          - (int2) rf_chain_ptr->rf_nv_dig_items_ptr-> \
                     rfnv_cdma_tx_lim_vs_freq[ rf_chain_ptr->rf_freq_index ];
#endif

        /* Scale the adjust from 10 bits to 8 bits.
        */
        pa_range_adj = pa_range_adj/4;

        /* rf_r[123]_[rise|fall]_val = rf_r[123]_[rise|fall]_val + pa_range_adj */
        rf_reset_pa_thres();

#ifdef FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST
        if ( rf_cagc_is_pa_thres_adjusted() ) 
        {
           rf_adjust_pa_thres( -rf_cagc_get_curr_pa_thres_adj(PA_SWITCHPOINT_1), 
                               -rf_cagc_get_curr_pa_thres_adj(PA_SWITCHPOINT_2),
                               -rf_cagc_get_curr_pa_thres_adj(PA_SWITCHPOINT_3));
        }
#endif /* FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST */

        rf_cagc_adjust_max_pwr_pa_thres( pa_range_adj );

      } else {

        /* -------------------------------------------------------------------
        ** Else, if the Tx power is above the limit, move the limit down 2 dB
        ** to bring the power below the limit.  If the HDET value is above the
        ** HDET correction range, the power is above the desired power limit.
        ** ---------------------------------------------------------------- */

        /* Lower the power limit and PA switch points by 2 dB
        */

        if ( rf_get_cagc_linearizer_range() == RF_HAS_102DB_CDMA_DYNAMIC_RANGE ) 
        {
          rf_cagc_adjust_max_pwr_pa_thres(-1 * RF_PA_RANGE_CNT_PER_2DB_102);
       
        /* update the TX power limit */
          rf_set_tx_pwr_limit( rf_tx_pwr_limit - (2 * RF_TX_LIM_CNT_PER_DB_102) );

        #ifdef RF_HAS_MDSP_TX_AGC
         CDMA_MDSP_NEW_TX_LIMIT_FRAME_PARAMS_AVAILABLE();
        #endif 
        }
        else   /* 85 dB dynamic range */
        {
          rf_cagc_adjust_max_pwr_pa_thres(-2 * RF_PA_RANGE_CNT_PER_DB);

        /* update the TX power limit */
          rf_set_tx_pwr_limit( rf_tx_pwr_limit - (2 * RF_TX_LIM_CNT_PER_DB) );

       #ifdef RF_HAS_MDSP_TX_AGC
        CDMA_MDSP_NEW_TX_LIMIT_FRAME_PARAMS_AVAILABLE();
       #endif           
        }

      }

    } 
    else 
    {
      if ( rf_get_cagc_linearizer_range() == RF_HAS_85DB_CDMA_DYNAMIC_RANGE ) 
      {
        if ( (rf_tx_pwr_limit - RF_TX_LIM_CNT_PER_DB) < rf_tx_agc_adj ) 
        {

          /* ------------------------------------------------------------------
          ** Else, if the power is below the limit and the power is limiting,
          ** move the limit up by 5 dB to let the power come up to a reasonable
          ** limit.  If the power is limiting, and the HDET value is below the
          ** HDET correction range, the power is below the desired power limit.
          **
          ** Allow a 1 dB tolerance on "limiting", so that power control can
          ** dither at the limit.
          ** ------------------------------------------------------------------ */

          /* Raise the limit by 5 dB
          */
          /* update the TX power limit */
          rf_set_tx_pwr_limit( rf_tx_pwr_limit + (5 * RF_TX_LIM_CNT_PER_DB) );

       #ifdef RF_HAS_MDSP_TX_AGC
        CDMA_MDSP_NEW_TX_LIMIT_FRAME_PARAMS_AVAILABLE();
       #endif   

          /* Raise the PA switch points by 5 dB
          */
          rf_cagc_adjust_max_pwr_pa_thres(5 * RF_PA_RANGE_CNT_PER_DB);
        }
      }
      else /* 102 dB dynamic range */
      {
        if ( (rf_tx_pwr_limit - RF_TX_LIM_CNT_PER_DB_102) < rf_tx_agc_adj ) 
        {

          /* ------------------------------------------------------------------
          ** Else, if the power is below the limit and the power is limiting,
          ** move the limit up by 5 dB to let the power come up to a reasonable
          ** limit.  If the power is limiting, and the HDET value is below the
          ** HDET correction range, the power is below the desired power limit.
          **
          ** Allow a 1 dB tolerance on "limiting", so that power control can
          ** dither at the limit.
          ** ------------------------------------------------------------------ */

          /* Raise the limit by 5 dB
          */
          /* update the TX power limit */
          rf_set_tx_pwr_limit( rf_tx_pwr_limit + (5 * RF_TX_LIM_CNT_PER_DB_102) );

         #ifdef RF_HAS_MDSP_TX_AGC
          CDMA_MDSP_NEW_TX_LIMIT_FRAME_PARAMS_AVAILABLE();
         #endif   

          /* Raise the PA switch points by 5 dB
          */
          rf_cagc_adjust_max_pwr_pa_thres(RF_PA_RANGE_CNT_PER_5DB_102);
        }
      }
    }

  #ifdef RF_HAS_MDSP_TX_AGC
  if(rf_mdsp_get_tx_double_buff_write_status(PA_THRESH_PARAMS, FRAME_LATCH)== TRUE)
  {    
  #endif
    rf_load_pa_thres();
  #ifdef RF_HAS_MDSP_TX_AGC
    CDMA_MDSP_NEW_PA_THRESH_FRAME_PARAMS_AVAILABLE();
  }
  else
  {
      CDMA_MDSP_NEW_PA_THRESH_FRAME_PARAMS_AVAILABLE();
      MSG_ERROR("PA Thresholds cannot be programmed at Frame boundary in Power limit Algorithm",0,0,0);
  }
  #endif  

    /* --------------------------------------------------------------------
    ** If hit/exceeded the lower/upper limit, something is probably broken.
    ** -------------------------------------------------------------------- */
 
    if ( rf_tx_pwr_limit <= RF_TX_PWR_CALC_MIN ) {
      ERR("RF Tx Power Limit min'd (%d)", rf_tx_pwr_limit, 0, 0);
    }
    if ( rf_tx_pwr_limit >= RF_TX_PWR_CALC_MAX ) {
      ERR("RF Tx Power Limit max'd (%d)", rf_tx_pwr_limit, 0, 0);
    }

    /* -------------------------------------------------------------------
    ** Computation done, get data again after RF_TX_LIM_ADJ_REPEAT_TIME ms
    ** ------------------------------------------------------------------- */


    /*lint -save -e717 */
    clk_reg( &rf_pwr_call_back,         /* pointer to call back structure   */
             rf_enable_125_ms_isr,      /* address of function to call back */
             RF_TX_LIM_ADJ_REPEAT_TIME, /* time to wait till call back      */
             (int4)0,                   /* delay between repeat call backs  */
             RF_CALL_BACK_ONCE );       /* do only one call back            */
    /*lint -restore */
  }
} /* rf_adjust_cdma_pwr_limit */
#endif /* !RF_HAS_HDET_BYPASS */

#if !defined(RF_HAS_HDET_BYPASS)

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
)
{
  extern void rf_125_ms_isr( void );

#ifdef RF_HAS_INTERNAL_HDET_SUPPORT
  /* RF cards that use the internal HDET of the RF chipset should begin
     warming up the HDET circuit now.
  */
  if ( rfc_uses_internal_hdet(rf_chain_status[RF_PATH_0].rf_curr_cdma_band) )
  {
     rft_internal_hdet_enable( RF_PATH_0 );
    /* Skip one PCG in order to allow extra settling time in the power detector */
    hdet_warmup_counter = 1;
  }
#endif /* RF_HAS_INTERNAL_HDET_SUPPORT */

  tramp_set_isr( TRAMP_125_MS_ISR, rf_125_ms_isr );
} /* rf_enable_125_ms_isr */
#endif /* !RF_HAS_HDET_BYPASS */

#if !defined(RF_HAS_HDET_BYPASS)

#ifdef RF_HAS_INTERNAL_HDET_SUPPORT
/*===========================================================================

FUNCTION RF_CURRENT_MODE_IS_FTM_RF_CAL                    INTERNAL FUNCTION

DESCRIPTION
 It returns TRUE if current AMSS mode is FTM RF Cal Mode 
 otherwise returns FALSE

DEPENDENCIES

RETURN VALUE
  Boolean
  
SIDE EFFECTS
===========================================================================*/
boolean  rf_current_mode_is_ftm_rf_cal(void)
{
  #ifdef FEATURE_FACTORY_TESTMODE

  boolean rf_current_ftm_mode;

  if (ftm_mode == FTM_MODE)
  {
    rf_current_ftm_mode = ftm_current_mode_is_rf_cal();
    return rf_current_ftm_mode;    
  }
  else  
  {  
    return FALSE;
  }
  #else
    return FALSE;
  #endif
}
#endif /* RF_HAS_INTERNAL_HDET_SUPPORT*/
/*===========================================================================

FUNCTION RF_125_MS_ISR                                      INTERNAL FUNCTION

DESCRIPTION
  This is the 1.25 ms ISR (not 125 ms).  This interrupt occurs only if
  MASK_DATA == 1 (indicating PA is on).  Read HDET and temperature.
  Calculate table indices for these items.  Schedule a call back
  to rf_adjust_cdma_pwr_limit().  This ISR is enabled by rf_enable_125_ms_isr()
  and disabled in this interrupt service routine.  Used in CDMA mode only.


DEPENDENCIES
  rf_hdet_off and rf_hdet_spn must have been loaded from NV storage

  rf_pwr_call_back must be setup

RETURN VALUE
  None

SIDE EFFECTS
  Updates rf_tx_agc_adj, rf_hdet_data, temperature data,
    and rf_pwr_call_back

===========================================================================*/
LOCAL void rf_125_ms_isr( void )
{
  word int_save;                        /* saved interrupt enable state    */
  #ifdef RF_HAS_INTERNAL_HDET_SUPPORT
  boolean rf_current_ftm_rf_cal_mode;
  #endif
  rf_chain_state_type *rf_chain_ptr = 
        &rf_chain_status[ rfcom_to_path[ RFCOM_TRANSCEIVER_0 ] ];
  
  INTLOCK_SAV( int_save );  

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* -----------------------------------------------------
  ** Block interrupts so HDET data and TX gain control are
  ** sampled close together timewise.
  ** ----------------------------------------------------- */
  #ifdef RF_HAS_INTERNAL_HDET_SUPPORT
  if ( hdet_warmup_counter > 0 )
  {
     /* While the warmup counter is still positive, decrement and wait for
        the next PCG to see if it is safe to take a reading. */
    hdet_warmup_counter--;
     INTFREE_SAV(int_save);
     return;
  }
  #endif /* RF_HAS_INTERNAL_HDET_SUPPORT */


  /* Read CAGC power control setting (scaled 0..1023 min to max) */
  rf_tx_agc_adj = (int2)rf_get_raw_cdma_tx_agc();

  /* -------------------------------------------------------------------
  ** Read CDMA HDET via the A/D converter and calculate hdet table index
  ** This is for a correction at high-power only, so use the High-power
  ** version of the detector. When A/D is configured to perform 8 bit
  ** conversions the 8 MSBs are returned.
  ** ------------------------------------------------------------------- */
  rf_hdet_data = rf_read_hdet( rf_chain_ptr->rf_curr_cdma_band );


  /*lint -save -e737 */
  if ((RF_SYNTH_STATUS_READ & RF_PA_ON_M) != RF_PA_ON_V)
  /*lint -restore */
  {

      INTFREE_SAV(int_save);
      /* Return without disabling ISR so we can try again */
      return;
  }

  /* Disable this ISR */
  tramp_set_isr( TRAMP_125_MS_ISR, NULL );

  #ifdef RF_HAS_INTERNAL_HDET_SUPPORT
  rf_current_ftm_rf_cal_mode = rf_current_mode_is_ftm_rf_cal();
  if ( (rfc_uses_internal_hdet(rf_chain_status[RF_PATH_0].rf_curr_cdma_band)) && (rf_current_ftm_rf_cal_mode == FALSE))
  {
    #ifdef RF_HAS_SC2X_BETA_SCALING_DEBUG 
#error code not present
    #endif
    rft_internal_hdet_disable( RF_PATH_0 );
  }
  #endif /* RF_HAS_INTERNAL_HDET_SUPPORT */

#ifdef FEATURE_TX_POWER_BACK_OFF
  /* Sample the global PA Backoff global variables used in power limiting loop*/
  sampled_rf_digital_backoff_active = rf_digital_backoff_active;
  sampled_rf_pa_backoff_accumulator = rf_pa_backoff_accumulator;
  sampled_current_hdet_offset = current_hdet_offset;
#endif

  /* -----------------------------------------------
  ** Restore previous interrupt enable/disable state
  ** ----------------------------------------------- */
  INTFREE_SAV( int_save );

  /* Data is ready for computation, schedule the power limit adjuster
  */

  /*lint -save -e717 */
  clk_reg( &rf_pwr_call_back,         /* pointer to call back structure   */
           rf_adjust_cdma_pwr_limit,  /* address of function to call back */
           RF_CALL_BACK_5_MS,         /* call back after 5 ms             */
           (int4)0,                   /* delay between repeat call backs  */
           RF_CALL_BACK_ONCE );       /* do only one call back            */
  /*lint -restore */

} /* rf_125_ms_isr */
#endif /* !RF_HAS_HDET_BYPASS */

#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */

#endif /* !FEATURE_GSM_PLT */


#if defined(RF_HAS_HDET_COMP_FOR_RC_AND_TEMP)
/*===========================================================================

FUNCTION rf_get_exp_tx_agc_vs_hdet                  EXTERNAL FUNCTION

DESCRIPTION
  This function does the following:
  1) Converts the raw HDET ADC value into a TX AGC (TX power) 
     in 10th of a dBm. 
  2) Compensates the results depending of HDET temperature and
     the radio configuration.

DEPENDENCIES
  None.

RETURN VALUE
  TX AGC (TX power) in 10th of a dBm. 

SIDE EFFECTS
  None

===========================================================================*/
#ifdef RF_HAS_HDET_UINT16
int16 rf_get_exp_tx_agc_vs_hdet(uint16 rf_hdet_read)
#else
int16 rf_get_exp_tx_agc_vs_hdet(uint8 rf_hdet_read)
#endif
{   
  int16  hdet_val   = 0;

  #ifdef RF_HAS_HDET_UINT16
  const uint16  array_size = 4095;       
  #else
  const uint8  array_size = 255;
  #endif
        

  /* Make sure the pointer is valid. */
  /* Make sure that we do not over flow the array*/
  if ((rf_exp_agc_table_ptr != NULL) && \
      (rf_hdet_read <= array_size))
  {
    /* Convert the raw HDET ADC value into a TX AGC (TX power) in 10th 
       of a dBm. */
    /* Go from a 10th of a dBm to a 100th of dBm. This is because the 
       compensation values are in a 100th of dBM. */
    hdet_val = (int16)(rf_exp_agc_table_ptr[rf_hdet_read] * 10);

    /* 1) Copy the current TX Radio configuration rate to the rf hdet 
          compensation configuration structure.
       2) Pass the pointer of the rf hdet compensation configuration 
          structure to the Rf card driver so that it can be used to 
          calculate the HDET compensation value. */
    rftxplim_hdet_comp_config.rc          = txc_so.rev_fch.rc;         
    #ifdef FEATURE_IS2000_R_SCH
    rftxplim_hdet_comp_config.r_sch_rate  = txtc.supp_rate;
    #endif	
    #ifdef FEATURE_IS2000_P2
      rftxplim_hdet_comp_config.dcch_on     = txc_so.rev_dcch.included;  
      rftxplim_hdet_comp_config.r_fch_on    = txc_so.rev_fch.included;  
    #else
      rftxplim_hdet_comp_config.dcch_on     = FALSE;  
      rftxplim_hdet_comp_config.r_fch_on    = TRUE;
    #endif    
    rftxplim_hdet_comp_config.r_sch_on    = txtc.sch_on;  
    rftxplim_hdet_comp_config.r_sch_gain  = txc_sch_gain; 
    rftxplim_hdet_comp_config.update_temp = rf_update_hdet_temp_value_needed();
    rftxplim_hdet_comp_config.band        = rf_chain_status[RF_PATH_0].rf_curr_cdma_band;
    rftxplim_hdet_comp_config.r_fch_rate  = txtc.rate;

    if ((rftxplim_hdet_comp_config.r_sch_on == TRUE) && (rftxplim_hdet_comp_config.r_sch_gain == 0))
    {
      rftxplim_hdet_comp_config.r_sch_on = FALSE;
    }

    /* Calculate and add the HDET compensation value to the current HDET. */
    hdet_val  += rfc_get_hdet_comp_value(&rftxplim_hdet_comp_config);

    /* Round up and go from 100th of a dBm to 10th of a dBm. */
    hdet_val = (int16)((hdet_val + 5)/10);
  }
  else
  {
    RF_FATAL_ERROR("Error while trying to index the hdet vs agc array.");
  } /* Make sure the HDET array is valid.*/

  return hdet_val;
}   /* rf_get_exp_tx_agc_vs_hdet() */                 

#if (defined(RF_HAS_HDET_COMP_FOR_RC_AND_TEMP)) && (!defined(RF_HAS_NO_HDR_HDET_COMP))
#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */
#endif /* RF_HAS_NO_HDR_HDET_COMP */
/*===========================================================================

FUNCTION rf_req_update_hdet_temp_value              EXTERNAL FUNCTION

DESCRIPTION
  This function will force HDET temperature to be read/updated from ADC 
  next time rf_get_exp_tx_agc_vs_hdet() is called.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rf_req_update_hdet_temp_value(void)
{
  rf_read_hdet_temp_counter = RF_REFRESH_HDET_COMP_THERM_COUNT;
}

/*===========================================================================

FUNCTION rf_update_hdet_temp_value_needed            EXTERNAL FUNCTION

DESCRIPTION
  This function keeps track of how many time has rf_get_exp_tx_agc_vs_hdet()
  been called. If rf_get_exp_tx_agc_vs_hdet() gets called 
  RFAGC_REFRESH_HDET_THERMISTOR amount of times then it will indicate that 
  it is time to refresh/read the HDET temperature.

  NOTE: HDET temperature changes slowly, there is not need to read it
        every 100 msec (when we are close to max TX power)

DEPENDENCIES
  None.

RETURN VALUE
  TRUE = It is time to update/read the HDET temperature.

SIDE EFFECTS
  None

===========================================================================*/
static boolean rf_update_hdet_temp_value_needed(void)
{
  boolean rtn_value = FALSE;
  
  return FALSE;

  if (rf_read_hdet_temp_counter >= (RF_REFRESH_HDET_COMP_THERM_COUNT) ) 
  { 
    rtn_value = TRUE;
    rf_read_hdet_temp_counter = 0;
  }
  else
  {
    rf_read_hdet_temp_counter++;
  }

  return rtn_value;
}
#endif /* RF_HAS_HDET_COMP_FOR_RC_AND_TEMP */

