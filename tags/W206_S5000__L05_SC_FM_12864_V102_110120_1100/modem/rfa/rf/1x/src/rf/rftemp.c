/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     RF Temperature Compensation Services

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1998,1999,2000 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2001,2002,2003 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2004,2005,2006 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2007,2008,2009 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*=================================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rftemp.c#6 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------------
01/26/09   vb      Bug Fix: resetting the rf_device_config_update_counter to 
                   RF_TEMP_DEVICE_CONFIG_COUNT_UPDATE before clk_dereg
12/03/08   vm      Added the Support for Temperature based Device
                   Configration change
06/13/08   vm      Updates for 16 bit PDET on QSC1100
04/28/08   sl      Added LO PWC DAC updates based on temperature under rf_cdma_temp_update.
07/30/07   vm      Added more support for 8 bin Thermistor bining
07/26/07   vm      Added the support for Tx Power Compensation NV based on
                   temperature and Frequency
01/11/06   et      LINT fixes
12/12/05   sar     Eliminated lint errors.
12/08/05   sar     Included hdrmdsp.h to eliminate warning for hdrmdsp_check_dsp_clk().
12/07/05   sar     Added a check in rf_hdr_temp_update to assure mdsp clock is on when
                   updated value is written.
06/10/05   dbc     Fixed lint warning
03/08/05   dyc     Featurize rf_hdr_temp_update() under HDR.
02/19/20   dyc     Added rf_hdr_temp_update()
10/29/04   bmg     Cleaned up unused RX linearizer globals
08/16/04   dyc     Removed RF_HAS_FM component and rf_fm_temp_update()
04/18/04   dyc     Remove rf_set_pa_offsets() from rf_cdma_do_temp_comp()
04/08/04   dyc     Updates to support removal of DFM VU dependency.
02/02/04   kss     Perform temp comp updates in HDR mode.
01/07/04   ajn/dyc Provide REX_TIMER_TCB macro if not already defined.
11/14/03   ajn     Replaced access of rex_timer_type.tcb_ptr with access macro.
10/20/03   dyc     Remove FEATURE_HWTC
09/12/03   sar     Added device parameter to rf_cdma_do_temp_comp().
09/03/03   dbc     Use global RF variable rf_chain_status[] to access RF state
                   related variables
01/13/03   aks     Changes for GSM PLT.
08/29/02   dbc     Cleanup
08/16/02   dyc     Default to disable tx temperature comp for DFM mode.
05/26/02   jwh     Merge FEATURE_HWTC changes.
03/18/02   dbc     Removed use of rx temp linearization.
01/29/02   dbc     Changed LNA decision override to only be used for backwards
                   LNA control (Superhet mode) 
10/19/01   dbc     RF_CAP_PDM1_FOR_TX, RF_CAP_PDM2_FOR_TX, RF_HAS_PA_RANGE_CAL 
                   support removed. FEATURE_PA_RANGE_TEMP_FREQ_COMP mainlined.                   
10/05/01   aks     Merged in phone_t changes.
90/19/01   dbc     Removed RF_HAS_RFR3100 dependencies - SW LNA Control removed 
                   for CDMA and FM. NV Items introduced w/RF_HAS_RFR3100 mainlined.   
07/02/01   aks     Created from re-organization of rfmsm.c, rev 1.10.
                   O:/src/asw/COMMON/vcs/rfmsm.c_v   1.10   21 May 2001 16:40:16
===================================================================================*/

#include "rficap_mm_cdma1x.h"

#include "target.h"
#include "customer.h"

#ifndef FEATURE_GSM_PLT   

#include "comdef.h"
#include "db.h"
#include "err.h"
#include "msm.h"
#include "msm_drv.h"
#include "rfi.h"
#include "rf.h"
#include "adc.h"
#include "therm.h"
#include "nv.h"
#include "sleep.h"

#ifdef RF_HAS_SUPPORT_FOR_LO_PWC_CAL
#include "rfr.h"
#endif

#ifdef RF_HAS_TEMP_BASED_DEVICE_CONFIG
#include "rft.h"
#endif

#ifdef FEATURE_FACTORY_TESTMODE
#include "test_mc.h"
#endif

#ifndef REX_TIMER_TCB 
#define REX_TIMER_TCB(timer)  (timer)->tcb_ptr 
#endif /* REX_TIMER_TCB */

#ifdef RF_HAS_HDR
#error code not present
#endif

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

                  TEMPERATURE COMPENSATION CONSTANTS

=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/* -----------------------------------------------------------------------
** The temperature compensation uses interpolated tables of size
** NV_TEMP_TABLE_SIZ.  The temperature value from the therm services
** is scaled 0..THERM_SCALE.  The bin size between the entries in the
** table is RF_TEMP_BIN_SIZ.  Note that this is a scalar, and the
** result of the division can make 1 bin smaller than RF_TEMP_BIN_SIZ.
** ----------------------------------------------------------------------- */
#define RF_TEMP_BIN_SIZ (THERM_SCALE / (NV_TEMP_TABLE_SIZ-1))


/* -----------------------------------------------------------------------
** Time values (in milliseconds) for clk_reg() call back functions
** ----------------------------------------------------------------------- */

/* Milliseconds of delay between temperature correction and
** re-load of the CDMA linearizers
*/
#define RF_CDMA_TEMP_COMP_TIME        ( (word) ((10 * 1000))  )

/* Milliseconds of delay before temperature correction and
** re-load of the CDMA linearizers
*/
#define RF_CDMA_TEMP_START_TIME      ((word) 60)

/* Default to disable and bypass expected hdet temperature compensation for TX in FM mode.
** Not needed at this time.  Evaluation of thermister data and calibration required. 
*/
#define RF_DISABLE_FM_TEMP_COMP       TRUE

#define RFTEMP_THERM_MIN 0
#define RFTEMP_THERM_MAX 255

#ifdef RF_HAS_TEMP_BASED_DEVICE_CONFIG
#define RF_TEMP_BASED_DEVICE_CONFIG_ADJUST_REPEAT_TIME  ((int4)20)
#define RF_TEMP_DEVICE_CONFIG_COUNT_UPDATE ((byte)3)
#endif

#ifdef RF_HAS_RF_ASIC_THERM
extern uint16 rtr6285_1x_read_therm( void );
#endif

#ifdef RF_HAS_PA_ACCESS_PROBE_CONFIG
extern boolean rf_pa_config_is_traffic_state;
#endif

#ifdef RF_HAS_TEMP_BASED_DEVICE_CONFIG
static void rf_adjust_device_config_based_on_temp(int4 ms_interval);
static void rf_temp_initiate_device_config_update(void);
static int rf_device_config_update_counter = RF_TEMP_DEVICE_CONFIG_COUNT_UPDATE;
#endif

/* -------------------------------------------------------------------------

                            Shared RF Driver Data

---------------------------------------------------------------------------- */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   CDMA mode Tx power limit data vs temperature,
   frequency, TX_AGC_ADJ PDM setting, and HDET reading
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
LOCAL int2 rf_cdma_tx_lim_vs_temp_now;   /* comp val for current temp */

#ifdef RF_HAS_HDET_UINT16
LOCAL uint16 rf_hdet_off_adj;     /* adjusted ADC HDET reading offset */
LOCAL uint16 rf_hdet_spn_adj;     /* adjusted ADC HDET reading span   */
#else
LOCAL byte rf_hdet_off_adj;     /* adjusted ADC HDET reading offset */
LOCAL byte rf_hdet_spn_adj;     /* adjusted ADC HDET reading span   */
#endif
static uint8 rftemp_therm_min = RFTEMP_THERM_MIN;
static uint8 rftemp_therm_range = RFTEMP_THERM_MAX;

#ifdef RF_HAS_NV_SUPPORT_FOR_8_THERM_BINS
extern rfnv_common_items_type rfnv_cdma1x_common_tbl;
#endif

#ifdef RF_HAS_TEMP_BASED_DEVICE_CONFIG
clk_cb_type rf_temp_based_device_config_call_back;
static word rf_thermistor_last_therm_read;
#endif


/* -----------------------------------------------------------------------

                      SHARED CALIBRATION RELATED DATA

-------------------------------------------------------------------------- */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   FM mode TX_AGC_ADJ initial setting vs power, frequency, and temperature
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
int2  rf_fm_agc_set_vs_temp_now;   /* comp val for current temp */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   FM mode TX_AGC_ADJ adjustments
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
byte  rf_fm_exp_hdet_vs_temp_now;   /* comp val for current temp */

#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */


/* -------------------------------------------------------------------------

                            Local Data

---------------------------------------------------------------------------- */
/* Pointer to timer block that control the call backs for temperature update
** and tx gain comp and load cagc linearizers.
*/
LOCAL rex_timer_type *temp_comp_timer_ptr;



/*===========================================================================

FUNCTION RF_TEMP_COMP_VAL                                   INTERNAL FUNCTION

DESCRIPTION
  Look-up the temperature-specific calibration value from a calibration
  table.  If the calibration value is between 2 entries, the result is
  interpolated.

DEPENDENCIES
  largest value in the temperature-specific calibration table is of a
  magnitude less than 32767.

RETURN VALUE
  temperature specific compensation value derived from the calibration table

SIDE EFFECTS
  None

===========================================================================*/
int2 rf_temp_comp_val
(
  int2 temp_table[ NV_TEMP_TABLE_SIZ ], /*lint -e818 temp_table could be declared as const */
    /* table to extract the value from */

  word temp_index,
    /* Temperature segment index, less than NV_TEMP_TABLE_SIZ */

  int2 temp_rem
    /* Temperature segment remainder */
)
{
  int2 delta;                             /* Difference between two values */

  #ifdef RF_HAS_NV_SUPPORT_FOR_8_THERM_BINS
  int2 therm_diff;
  #endif

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* ------------------------------------------------------------
  ** If the index is not the last entry, find the delta between
  ** table entries, scale it by the remainder divided by the
  ** temperature bin size, to get the delta between the Ith entry
  ** and the correct value.
  ** ------------------------------------------------------------ */
  if ( temp_index < (NV_TEMP_TABLE_SIZ-1)) {

    /* ----------------------------------------------
    ** Calculate the difference between table entries
    ** The table value has a magnitude less than 32k,
    ** or this will wrap and fail.
    ** ---------------------------------------------- */
    delta  = temp_table[ temp_index + 1 ];
    delta -= temp_table[ temp_index     ];

    /* -------------------------------------------------
    ** Scale the difference by the remainder and divide
    ** it by the bin size, to get the correct difference
    ** between the Ith entry and the correct value.
    ** The result is rounded to reduce error to 1/2 LSB.
    ** ------------------------------------------------- */
    delta *= temp_rem;

    #ifdef RF_HAS_NV_SUPPORT_FOR_8_THERM_BINS
    therm_diff = (int2)(rfnv_cdma1x_common_tbl.rfnv_therm_bin_type[temp_index+1] - rfnv_cdma1x_common_tbl.rfnv_therm_bin_type[temp_index]);
    delta = delta + (therm_diff/2);
    delta /=  therm_diff;
    #else
    delta += RF_TEMP_BIN_SIZ/2;
    delta /= RF_TEMP_BIN_SIZ;
    #endif

    return( temp_table[ temp_index ] + delta );     /* Interpolated value */

  } else {
    return( temp_table[ temp_index ] );             /* Max value          */
  }

} /* rf_temp_comp_val */



/*================================================================================
FUNCTION rftemp_calibrate_therm

DESCRIPTION
  This function reads the scaled thermistor value from the internal or external
  thermistor as appropriate for the current target.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===================================================================================*/
void rftemp_calibrate_therm( uint8 therm_min, uint8 therm_max )
{
  if (therm_min < therm_max)
  {
    rftemp_therm_min = therm_min;
    rftemp_therm_range = therm_max - therm_min;
  }
}
/*================================================================================
FUNCTION rftemp_scale_therm

DESCRIPTION
  This function scales the raw thermistor reading.
  
DEPENDENCIES
  None

RETURN VALUE
  Scaled therm

SIDE EFFECTS
  None
===================================================================================*/
uint16 rftemp_scale_therm( uint16 temperature )
{
  /* rtr6285_1x_read_therm returns the raw reading, scale it here */
  if (temperature < rftemp_therm_min)
  {
    temperature = RFTEMP_THERM_MIN;
  }
  else
  {
    if (rftemp_therm_range != 0)
    {
      temperature = (uint16) (((temperature - rftemp_therm_min) * RFTEMP_THERM_MAX) / rftemp_therm_range);
    }
    /* if it exceeds the max then return the max */
    temperature = MIN(temperature, RFTEMP_THERM_MAX);
  }
  return temperature;
}
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
void rf_cdma_temp_update( void )
{
  word temperature;            /* Temperature read from the therm services */
  word temp_index;             /* Temperature segment index                */
  int2 temp_rem;               /* Temperature segment remainder            */
  word int_save;               /* saved interrupt enable state             */
  word loop_ctr;

  #ifdef RF_HAS_NV_SUPPORT_FOR_8_THERM_BINS
  byte index ;
  #endif

  rf_chain_state_type *rf_chain_ptr = 
        &rf_chain_status[ rfcom_to_path[ RFCOM_TRANSCEIVER_0 ] ];
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* -----------------------------------------------------------------
  ** Read the temperature, then calculate the table index & remainder.
  ** The remainder is to interpolate between table entries with.  The
  ** remainder divided by the bin size is the ratio between successive
  ** temperature indices which yields our best approximation of the
  ** temperature.
  ** ----------------------------------------------------------------- */

  /* If the phone is sleeping, the ADC may be off.  DO NOT read the
  ** temperature.  Simply exit without updating any tables.
  */
  INTLOCK_SAV( int_save );
  if( rf_chain_ptr->rf_state == RF_STATE_SLEEP_1
  ||  rf_chain_ptr->rf_state == RF_STATE_SLEEP_2 ) {
    INTFREE_SAV( int_save );
    return;
  } else {                             /* Phone is NOT sleeping */        
    #ifdef RF_HAS_RF_ASIC_THERM
    /* In the future need to route this through the card structure. */
    temperature = (word)rtr6285_1x_read_therm();
    temperature = rftemp_scale_therm(temperature);
    #else
    #ifdef RF_HAS_SUPPORT_FOR_HIGH_RES_HKADC
    #ifdef RF_HAS_INTERNAL_THERM_SUPPORT
    if(rfc_uses_internal_therm(rf_chain_ptr->rf_curr_cdma_band))
    {
      temperature = (word)rfc_read_internal_therm(rf_chain_ptr->rf_curr_cdma_band);     
    }
    else
    {
      temperature = (word)adc_read(ADC_THERM);
    }
    #else
    temperature = (word)adc_read(adc_therm);
    #endif /*RF_HAS_INTERNAL_THERM_SUPPORT*/
    #else
    temperature = (word) therm_read();
    #endif /*RF_HAS_SUPPORT_FOR_HIGH_RES_HKADC*/
    #endif /*RF_HAS_RF_ASIC_THERM*/
    INTFREE_SAV( int_save );
    }        

    #ifdef RF_HAS_TEMP_BASED_DEVICE_CONFIG
    rf_thermistor_last_therm_read = temperature;    
    #endif

#ifdef RF_HAS_SUPPORT_FOR_LO_PWC_CAL
  rfr_adjust_lo_pwc_dac_codes( RFCOM_TRANSCEIVER_0 );
#endif //RF_HAS_SUPPORT_FOR_LO_PWC_CAL

  #ifdef RF_HAS_NV_SUPPORT_FOR_8_THERM_BINS
  index = 0;

  if (rfnv_cdma1x_common_tbl.rfnv_therm_bin_type[1] > rfnv_cdma1x_common_tbl.rfnv_therm_bin_type[0])
  {
    while (temperature > rfnv_cdma1x_common_tbl.rfnv_therm_bin_type[index])
    {
      index++;

      if (index > (NV_TEMP_TABLE_SIZ -1))
      {
        break;
      }
    
    }
    if(index == 0)
    {
      temp_index = index;
      temp_rem = 0;    
      
    }
    else if (index < NV_TEMP_TABLE_SIZ)
    {
      temp_index = (word)(index - 1);
      temp_rem = (int2)(temperature - rfnv_cdma1x_common_tbl.rfnv_therm_bin_type[temp_index]);    
    }
    else
    {
      temp_index = NV_TEMP_TABLE_SIZ - 1;
      temp_rem = 0;
    }
  }
  else
  {
    while (temperature < rfnv_cdma1x_common_tbl.rfnv_therm_bin_type[index])
    {
      index++;

      if (index > (NV_TEMP_TABLE_SIZ -1))
      {
        break;
      }
    
    }
    if(index == 0)
    {
      temp_index = index;
      temp_rem = 0;    
      
    }
    else if (index < NV_TEMP_TABLE_SIZ)
    {
      temp_index = (word)(index - 1);
      temp_rem = (int2)(temperature - rfnv_cdma1x_common_tbl.rfnv_therm_bin_type[temp_index]);    
    }
    else
    {
      temp_index = NV_TEMP_TABLE_SIZ - 1;
      temp_rem = 0;
    }
    
  }
  #else
  temp_index =         temperature / RF_TEMP_BIN_SIZ;   /* Segment index     */
  temp_rem   = (int2)( temperature % RF_TEMP_BIN_SIZ ); /* Segment remainder */
  #endif
  /* ---------------------------------------------------------------
  ** Update the temperature compensation values based on the current
  ** temperature.
  ** --------------------------------------------------------------- */

  for (loop_ctr=0; loop_ctr < RF_NUM_TX_LINEARIZERS; loop_ctr++)
     {
      #ifdef RF_HAS_TX_TEMP_FREQ_COMP_NV_BASED_ON_PWR
      rf_cdma_tx_lin_vs_temp_now[loop_ctr] =
        rf_temp_comp_val( rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_tx_power_comp[loop_ctr].rfnv_tx_comp_lin_vs_temp,
                            temp_index,
                              temp_rem );

      rf_cdma_tx_slp_vs_temp_now[loop_ctr] =
        rf_temp_comp_val(rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_tx_power_comp[loop_ctr].rfnv_tx_comp_slp_vs_temp,
                            temp_index,
                              temp_rem );

      #else
      rf_cdma_tx_lin_vs_temp_now[loop_ctr] =
        rf_temp_comp_val( rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_tx_lin_vs_temp[loop_ctr],
                            temp_index,
                              temp_rem );

      rf_cdma_tx_slp_vs_temp_now[loop_ctr] =
        rf_temp_comp_val( rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_tx_slp_vs_temp[loop_ctr],
                            temp_index,
                              temp_rem );
      #endif
     }

  rf_cdma_tx_lim_vs_temp_now =
    rf_temp_comp_val( rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_cdma_tx_lim_vs_temp,
                        temp_index,
                          temp_rem );

  #ifdef RF_HAS_TEMP_BASED_DEVICE_CONFIG  
  rf_temp_initiate_device_config_update();
  #endif

} /* rf_cdma_temp_update */

/* Call back struct for power control 
*/
#ifdef RF_HAS_TEMP_BASED_DEVICE_CONFIG
/*===========================================================================

FUNCTION RF_TEMP_INITIATE_DEVICE_CONFIG_UPDATE            INTERNAL FUNCTION

DESCRIPTION
 This is a function called for initiating the
 Device Updates based n temperatuer changes

DEPENDENCIES
  It should be called only in traffic state

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void rf_temp_initiate_device_config_update()
{  
  boolean rf_cal_mode_is_current; 
  
  rf_chain_state_type *rf_chain_ptr = 
        &rf_chain_status[ rfcom_to_path[ RFCOM_TRANSCEIVER_0 ] ];

  //Apply only if Current Mode is non RF Cal and its in Traffic State
  rf_cal_mode_is_current = rf_current_mode_is_rf_cal();

  if ((rf_cal_mode_is_current == FALSE)&&(rf_pa_config_is_traffic_state == TRUE))
  {        
    if(rfc_support_temp_based_device_config(rf_chain_ptr->rf_curr_cdma_band) == TRUE)
    {
      clk_reg( &rf_temp_based_device_config_call_back,         /* pointer to call back structure*/
                rf_adjust_device_config_based_on_temp,      /* address of func to call back  */
                RF_TEMP_BASED_DEVICE_CONFIG_ADJUST_REPEAT_TIME,      /* delay till call back          */
                (int4)0,                   /* delay between repeat call backs                         */
                RF_CALL_BACK_ONCE );       /* causes only one call back     */
    }
  }
}/*rf_temp_initiate_device_config_update()*/

/*===========================================================================

FUNCTION RF_ADJUST_DEVICE_CONFIG_BASED_ON_TEMP             INTERNAL FUNCTION

DESCRIPTION
 This is a clock Call Back function called during the device configration
 change based on temperature

DEPENDENCIES
  It should be called only in traffic state

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rf_adjust_device_config_based_on_temp
(
  int4 ms_interval          /* time interval since last call */
  /* Tell lint to ignore the unused parameter for now. */
  /*lint -esym(715,ms_interval) */
)
{
  rf_chain_state_type *rf_chain_ptr = 
      &rf_chain_status[ rfcom_to_path[ RFCOM_TRANSCEIVER_0 ] ];

  rf_device_config_update_counter = rf_device_config_update_counter - 1;  

  rft_config_device_for_temp(rfcom_to_path[ RFCOM_TRANSCEIVER_0 ],rf_chain_ptr->rf_curr_cdma_band,rf_thermistor_last_therm_read);

  if((rf_device_config_update_counter > 0)&&(rfc_support_temp_based_device_config(rf_chain_ptr->rf_curr_cdma_band) == TRUE))
  { 
    clk_reg( &rf_temp_based_device_config_call_back,         /* pointer to call back structure*/
              rf_adjust_device_config_based_on_temp,      /* address of func to call back  */
              RF_TEMP_BASED_DEVICE_CONFIG_ADJUST_REPEAT_TIME,      /* delay till call back          */
              (int4)0,                   /* delay between repeat call backs                         */
              RF_CALL_BACK_ONCE );       /* causes only one call back     */

  }
  else
  {
    rf_device_config_update_counter = RF_TEMP_DEVICE_CONFIG_COUNT_UPDATE;
    clk_dereg( &rf_temp_based_device_config_call_back);
  }
}
#endif/*RF_HAS_TEMP_BASED_DEVICE_CONFIG*/

#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */

/*===========================================================================
FUNCTION RF_CDMA_TEMP_COMP_INIT                             INTERNAL FUNCTION

DESCRIPTION
  This function simply saves the address of the timer block.  This address
  is passed by a task.  There is an understanding between this task and
  the RF driver that when this timer block expires, the task will call
  the function rf_cdma_do_temp_comp() so that the RF driver may start the
  process of adjusting the linearizers based on the temperature.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updates temp_comp_timer_ptr

===========================================================================*/
void rf_cdma_temp_comp_init
(
  rex_timer_type *save_timer_ptr    /* The timer block to control call backs */
)
{
  temp_comp_timer_ptr = save_timer_ptr;      /* Save the address in our variable */

} /* rf_cdma_temp_comp_init */


/*===========================================================================

FUNCTION RF_CDMA_DO_TEMP_COMP                               INTERNAL FUNCTION

DESCRIPTION
  This function does 2 things.  The first thing it does is to reset the
  timer so that it will get called again.  Then it will start the process
  of updating the temperature and adjusting the linearizers.  This process
  can only be stopped by clearing the timer with the function
  (void)rex_clr_timer().

DEPENDENCIES
  This function must be called from a task, so that it remains interruptable.
  temp_comp_timer_ptr must have been initialized and setup.

RETURN VALUE
  None

SIDE EFFECTS
  This function will set the timer, so that in will get executed again.

===========================================================================*/
void rf_cdma_do_temp_comp( rfcom_device_enum_type device )
{
  /* First, set the timer so that we get executed again later.
  */
  if(temp_comp_timer_ptr)
  {
#ifdef FEATURE_FACTORY_TESTMODE
    if(ftm_mode == DMSS_MODE)
    {
      (void)rex_set_timer( temp_comp_timer_ptr, RF_CDMA_TEMP_COMP_TIME );
    }
#else
    (void)rex_set_timer( temp_comp_timer_ptr, RF_CDMA_TEMP_COMP_TIME );
#endif
  }

  /* Start the process of updating the temperature and adjusting the
  ** linearizers.
  **
  ** First, update the temperature data
  */
  rf_cdma_temp_update();

#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */
  {

    /* -----------------------------------
    ** Load the CAGC linearizers (Tx & Rx)
    ** ----------------------------------- */
    rf_load_cagc_linearizers(device);
  }

} /* rf_cdma_do_temp_comp */


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
void rf_enable_cdma_lna_temp_comp( void )
{

  /* ---------------------------------------
  ** Start periodic temperature compensation
  ** --------------------------------------- */
#ifdef FEATURE_FACTORY_TESTMODE
  if(ftm_mode == DMSS_MODE)
  {
    (void)rex_set_timer( temp_comp_timer_ptr, RF_CDMA_TEMP_START_TIME );
  }
#else
  (void)rex_set_timer( temp_comp_timer_ptr, RF_CDMA_TEMP_START_TIME );
#endif

} /* end of rf_enable_cdma_lna_temp_comp */


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
void rf_disable_cdma_lna_temp_comp( void )
{
  #ifdef RF_HAS_TEMP_BASED_DEVICE_CONFIG
  rf_chain_state_type *rf_chain_ptr =  &rf_chain_status[ rfcom_to_path[ RFCOM_TRANSCEIVER_0 ] ];
  #endif

  /* ------------------------------------------
  ** Stop the periodic temperature compensation
  ** by clearing both the timer and the signal.
  ** ------------------------------------------ */
  #ifdef FEATURE_FACTORY_TESTMODE
      if(ftm_mode == DMSS_MODE)
      {
        (void)rex_clr_timer( temp_comp_timer_ptr );
        (void)rex_clr_sigs( REX_TIMER_TCB(temp_comp_timer_ptr),
                            SLEEP_RF_TEMP_COMP_SIG );
      }
  #else
      (void)rex_clr_timer( temp_comp_timer_ptr );
      (void)rex_clr_sigs( REX_TIMER_TCB(temp_comp_timer_ptr),
                          SLEEP_RF_TEMP_COMP_SIG );
  #endif

      #ifdef RF_HAS_TEMP_BASED_DEVICE_CONFIG
      if(rfc_support_temp_based_device_config(rf_chain_ptr->rf_curr_cdma_band) == TRUE)
      { 
        rf_device_config_update_counter = RF_TEMP_DEVICE_CONFIG_COUNT_UPDATE;
        clk_dereg( &rf_temp_based_device_config_call_back);
      }
      #endif
} /* end of rf_disable_cdma_lna_temp_comp */

#endif /* !FEATURE_GSM_PLT */

