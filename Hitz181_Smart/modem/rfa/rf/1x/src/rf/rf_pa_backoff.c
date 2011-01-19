/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 MSM Power Amplifier Backoff Module

GENERAL DESCRIPTION
    In certain radio and channel configurations of the mobile, 
    the reverse channel can have a high peak-to-average power ratio. 
    To mitigate spurious emissions, it is necessary in these configurations 
    to reduce the maximum total reverse power. 
    (Note that this reduction does not necessarily affect the current power.)


INITIALIZATION AND SEQUENCING REQUIREMENTS




Copyright (c) 1994, 1995, 1996 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1997, 1998, 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2000, 2001, 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2003, 2004       by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rf_pa_backoff.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/07/07   vm/adk  Added support for firmware TX AGC
01/25/07   jfc     Include high level rficap.h instead of rficap_mm_cdma1x.h
10/31/06   ycl     Include the correct rficap file.
01/11/06   et      LINT fixes
12/14/05   sar     Fixed more lint errors resulting from new config file.
06/17/05   zg      Fixed LINT errors.
05/26/05   dbc     Disable HDET in get_hdet_disable() whenever DCCH is active
                   on the reverse link
11/09/04   dbc     Don't release the ATMR in rf_acquire_atmr_resource() if
                   we are unable to get the ATMR. 
06/24/04   dbc     Subtract VBATT_SCALED_MIN from now_volt_adc in 
                   rf_get_voltage_backoff() when calculating battery voltage
06/22/04   dbc     Changes for ATMR resource acquisition/release
05/21/04   dbc     Redesign of PA Backoff module
04/08/04   dyc     Updates to support removal of DFM VU dependency.
01/29/04    et     Removed usage of RF_HAS_TRIMODE.  Replaced by 
                   FEATURE_TRIMODE_ITEMS.
11/21/03   dbc     Make rf_gl_adj_accumulator global so it is available to power
                   limiting algorithm in rf_adjust_cdma_pwr_limit() (rftxplim.c)
10/20/03   dyc     Remove FEATURE_HWTC
09/03/03   dbc     Use global RF variable rf_chain_status[] to access RF state
                   related variables
03/04/03   dbc     Merged in changes to enable PA backoff to work properly.
02/20/03   dbc     Removed confusing comments
01/13/03   aks     Changes for GSM PLT.
10/30/02   dbc     removed include of rfigpio.h
05/24/02   jwh     FEATURE_HWTC 
03/18/02   dbc     Merged in changes from common archive (r1.13) for 
                   FEATURE_TX_POWER_BACK_OFF
03/08/02   aks     Changed naming, from PILOT_FCH_9600 to PILOT_FCH.
10/05/01   aks     Merged in phone_t changes.
10/01/01   dbc     Added initial support for the new PMIC API.
09/21/01   sar     Merged from MSM_RF.00.01.15: Add division by RF_TX_BACKOFF_SCALE
                   for low voltage in procedure rf_get_gain_limit_by_volt. Also 
                   clarify return value in same procedure.
09/11/01   aks     Brought in its entirety from:
                   O:/src/asw/COMMON/vcs/rf_pa_backoff.c_v   1.5   14 Jun 2001 11:25:20
                   MSM_RF.00.01.04
06/14/01   fas     Move unit's conditional compilation below comdef.h.
06/11/01   fas     Wrap entire file with conditional compilation.
05/31/01   fas     Cleaned up. Reordered voltage backoff calculation to
                   retain more fractional information. Changed
                   rfnv_chan_config_backoff_low[chan_config][x] to
                   rfnv_chan_config_backoff_low[chan_config] to reflect
                   new nv.h configuration.
05/25/01   fas     Changed rf_nv_dig_items_ptr->rfnv_chan_config_backoff,
                   due to change in NV definition.
05/24/01   fas     Fix bug--was not transiting from RC3 to RC1. Also was not
                   clearing the accumulator at the transition.
05/23/01   fas     Initial revision.


===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "rficap.h"
#ifdef FEATURE_TX_POWER_BACK_OFF
#include "vbatt.h"
#include "atmr.h"
#include "rfi.h"

#ifdef RF_HAS_MDSP_TX_AGC
#include "rf_mdsp_tx_agc.h"
#endif

/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Enables the PA Backoff module to notify the power limiting loop whether the 
** HDET reading should be used for the current reverse link channel configuration 
*/
#define RF_DISABLE_HDET_PER_CHAN_CONFIG


/* Enables the PA Backoff module to notify the power limiting loop to apply a  
** offset to the HDET reading for the current reverse link channel configuration 
*/
#undef RF_SET_HDET_OFFSET_PER_CHAN_CONFIG


/* Enable or disable voltage based backoff of the TX power limit according to
** reverse link channel configuration 
*/
#define RF_DISABLE_VOLTAGE_BACKOFF


/* Define TEST_PA_BACKOFF for testing/debugging the PA Backoff module */
#undef TEST_PA_BACKOFF


#if defined ( TEST_PA_BACKOFF )
  
#define  TEST_TIME_FIRST 5000   /* delay till first call back      */
#define  TEST_TIME_CB    1000   /* delay between repeat call backs */

#define TEST_NO_COMMAND 0
#define TEST_GET_TESTS 1
#define TEST_GET_CURRENT_STATE 2
#define TEST_GET_NEW_CHAN_CONFIG 3
#define TEST_SET_NEW_CHAN_CONFIG 4
#define TEST_SET_VOLT_BACKOFF_PARAMS 5
#define TEST_SET_VBATT_OVERRIDE 6
#define TEST_GET_DCCH_PILOT_9600 7
#define TEST_SET_DCCH_PILOT_9600 8
#define TEST_SET_REFRESH_BACKOFF 9
#define TEST_SET_NO_BACKOFF 10
#define TEST_GET_NO_BACKOFF 11
#define TEST_GET_PILOT_FCH 12
#define TEST_SET_PILOT_FCH 13
#define TEST_GET_DCCH_PICH_SCH_TO_PICH_LESS_THAN_6_DB 14
#define TEST_SET_VOLT_BACKOFF_GENERIC 15
#define TEST_SET_HDET_OVERRIDE 16

uint8 pa_backoff_test_data[9];

/* Callback pointer  */
LOCAL clk_cb_type rf_pa_backoff_call_back;

/* Init the call back function for PA Backoff test interface */
LOCAL void rf_pa_backoff_test_cb_init(void);

/* Call back function for PA Backoff test interface */
LOCAL void rf_pa_backoff_test_cb( int4 dummy );

LOCAL boolean call_back_initialized = FALSE;

#undef TEST_PA_BACKOFF_HDET_OFFSET

#ifdef TEST_PA_BACKOFF_HDET_OFFSET
LOCAL int16 hdet_offset_override = 0;
#endif

#undef TEST_PA_BACKOFF_VOLTAGE_BACKOFF

#undef TEST_SIMULATE_VBATT

#ifdef TEST_SIMULATE_VBATT
LOCAL uint32 vbatt_override = VBATT_SCALE;
#endif

#endif /* TEST_PA_BACKOFF */



/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

                                DEFINES

=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define RF_COUNTS_PER_DB_85_DB          (12*16)  /* AGC counts for 1 dB in 1/16 dB resolution (85 dB) */
#define RF_COUNTS_PER_DB_102_DB         (10*16)  /* AGC counts for 1 dB in 1/16 dB resolution (102 dB) */

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

                    SCALE FACTORS FOR FIXED POINT MATH

=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#define RF_PA_BACKOFF_SCALE               100
#define RF_PA_BACKOFF_VOLTAGE_DATA_SCALE  10        



/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

                            LOCAL DATA TYPES

=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/


/* Types/locals tio support voltage backoff based on channel configuration */

typedef struct
{
    /* power back offs at 3 voltages 1/800 dB resolution */
    uint32 low_volt_dB;
    uint32 med_volt_dB;
    uint32 high_volt_dB;

    /* corresonding voltages at which above backoffs occur 
    ** in 1/100 volt resolution 
    */
    uint32 min_volt;
    uint32 mid_volt;
    uint32 max_volt;

    /* slope for the two line segments.  Since the slope is 
    ** always less than or equal to 0 a positive value for 
    ** slope_high_to_mid is used to indicate that the slopes
    ** of the segments have not yet been computed. We'll
    ** compute them the first time the voltage configuration
    ** is used.  Thereafter we'll use the precomputed values
    ** every time the configuration is used. 
    */
    int32 slope_high_to_mid;
    int32 slope_mid_to_low;    

}voltage_backoff_type;


/* Voltage base PA Backoff for: 
** PICH + DCCH
** PICH + DCCH + FCH
** PICH + FCH + SCH (low rate )
**
** 1.5 dB (1200 counts) at low battery voltage, 0.5 dB (400 counts) at mid battery 
** voltage and 0 dB (0 counts) at high battery voltage
*/
LOCAL voltage_backoff_type voltage_backoff_config_1 = { 1200, 400, 0 , 320, 370, 420, 1, 0 };


/* Voltage base PA Backoff for: 
** PICH + FCH + SCH (2X, 4X, 8X )
**
** 1.0 dB (800 counts) at low battery voltage, 0 dB (0 counts) at mid battery 
** voltage and 0 dB (0 counts) at high battery voltage
*/
LOCAL voltage_backoff_type voltage_backoff_config_2 = { 800, 0, 0 , 320, 370, 420, 1, 0 };


/* Voltage base PA Backoff for: 
** PICH + FCH + SCH (16X )
**
** 0.5 dB (800 counts) at low battery voltage, 0 dB (0 counts) at mid battery 
** voltage and 0 dB (0 counts) at high battery voltage
*/
LOCAL voltage_backoff_type voltage_backoff_config_3 = { 400, 0, 0 , 320, 370, 420, 1, 0 };

/* For channel configurations that require no voltage backoff */
LOCAL voltage_backoff_type no_voltage_backoff = { 0, 0, 0 , 320, 370, 420, 1, 0 };

/* For tracking current voltage backoff configuration */
LOCAL voltage_backoff_type *save_voltage_backoff_config = &no_voltage_backoff;


#ifdef TEST_PA_BACKOFF_VOLTAGE_BACKOFF
LOCAL voltage_backoff_type test_voltage_backoff = { 0, 0, 0 , 320, 370, 420, 1, 0 };
#endif

/* amount of backoff applied to rf_tx_pwr_limit for the current channel configuration */
int16 rf_pa_backoff_accumulator = 0;

/* amount of adjustment to add to HDET reading for the current channel configuration */
int16 current_hdet_offset = 0;

/* amount of backoff to add to the accumulator the next time rf_tx_pwr_limit is adjusted */
LOCAL int16 rf_pa_backoff_accumulator_diff = 0;

/* Raw VBATT value, normalized to 0 to 255 */
LOCAL uint32 now_volt_adc = 0;                

/* Handles to ATMR resources */
LOCAL atmr_handle_type atmr_handle = ATMR_INVALID_HANDLE;

/* Contains current state of PA Backoff state machine */
LOCAL rfnv_backoff_chan_config_type first_chan_config = BACKOFF_FIRST_RUN; 

/* Tracks current active channel configuration */
LOCAL rf_txc_chan_config_type save_chan_config =
{
  NO_BACK_OFF_CONFIG, //backoff_config;
  FALSE,              //fch_on;
  CAI_RC_1            //fch_rc;         /* include in cai.h */
#ifdef FEATURE_IS2000_P2
  ,FALSE              //dcch_on;
  ,CAI_RC_1           //dcch_rc;        /* include in cai.h */
#endif /* FEATURE_IS2000_P2 */
#ifdef FEATURE_IS2000_R_SCH
  ,FALSE              //sch_on;
  ,CAI_RC_1           //sch_rc;         /* include in cai.h */
  ,ENC_SCH_8TH_RATE   //sch_rate;       /* include in enc.h */
  ,FALSE              //sch_to_pch_over_6dB; /* true if over 6dB*/
#endif /* FEATURE_IS2000_R_SCH */
};



/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

                            GLOBAL DATA TYPES

=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/* Pointer to NV digital items/tables */
#ifdef FEATURE_GSM_PLT
#error code not present
#endif 

/* Number of seconds between calls to refresh the backoff. Definition
** is in rfagc.c. This number is not critical. We just want to 
** execute at a rate where we can respond to voltage and temperature changes.
*/
extern uint16 rf_backoff_refresh_counter;

/* Inhibit re-entry. Indicates an ATMR callback is pending */
boolean tx_power_limit_update_pending;

/* Disable HDET error adjustment factor in certain channel configurations. */
boolean rf_disable_hdet = FALSE;

/* True whenever we need to add a digital gain factor to the limit. */
boolean rf_digital_backoff_active = FALSE;

/* True whenever we need to adjust the limit per VBATT (battery voltage) */
boolean rf_voltage_backoff_active = FALSE;

/* True whenever we need to adjust the limit temperature */
boolean rf_temp_backoff_active = FALSE;




/* -----------------------------------------------------------------------

                         FUNCTION PROTOTYPES

** ----------------------------------------------------------------------- */
LOCAL uint16 rf_get_voltage_backoff( voltage_backoff_type *voltage_config );
LOCAL uint16 rf_get_temperature_backoff( void );
LOCAL void pa_backoff_adjust_tx_power_limit( void );
LOCAL void rf_acquire_atmr_resource( void );
LOCAL boolean get_temp_backoff_needed( rf_txc_chan_config_type *chan_config );
LOCAL boolean get_voltage_backoff_needed( rf_txc_chan_config_type *chan_config, voltage_backoff_type **voltage_config );
LOCAL boolean get_digital_backoff_needed( rf_txc_chan_config_type *chan_config );

#ifdef RF_DISABLE_HDET_PER_CHAN_CONFIG
LOCAL boolean get_hdet_disable( rf_txc_chan_config_type *chan_config );
#endif

#ifdef RF_SET_HDET_OFFSET_PER_CHAN_CONFIG
LOCAL void set_current_hdet_offset( rf_txc_chan_config_type *chan_config );
#endif


/* -----------------------------------------------------------------------

                                 FUNCTIONS

** ----------------------------------------------------------------------- */



/*===========================================================================

FUNCTION rf_update_tx_pwr_limit                         EXTERNALIZED FUNCTION

DESCRIPTION

Adjust the maximum power based on channel configuration, temperature, and
battery voltage. Actual adjustmemt is made by pa_backoff_adjust_tx_power_limit() 
through an ATMR resource.

DEPENDENCIES

None.

RETURN VALUE

None

SIDE EFFECTS
Acquires a ATMR resource

===========================================================================*/
void rf_update_tx_pwr_limit( rf_txc_chan_config_type *chan_config)
{
    boolean backoff_needed = FALSE;

    uint16 rf_pa_backoff_adjust_volt = 0; 
    uint16 rf_pa_backoff_adjust_temp = 0;
    int16 rf_ba_backoff_adjust_total = 0;

    word int_save;    /* saved interrupt enable state               */

    /* Inhibit re-entry. */
    INTLOCK_SAV( int_save );
    if ( tx_power_limit_update_pending ) 
    {
      INTFREE_SAV( int_save );
      return;
    } 
    else 
    {
      tx_power_limit_update_pending = TRUE;
    }
    INTFREE_SAV( int_save );

    switch ( chan_config->backoff_config )
    {
      /* Called periodically (linearizer update) to examine battery state. */
    case REFRESH_BACKOFF:

      /* If we have ever done a backoff, then
      ** keep on doing the last one periodically.
      */
      if( (first_chan_config==BACKOFF_FIRST_RUN) )
      {
        backoff_needed = FALSE ;
      }
      else
      {
        /* do the last configuration */
        chan_config = &save_chan_config;

        if ( ( rf_digital_backoff_active == TRUE ) || 
             ( rf_voltage_backoff_active == TRUE ) ||
             ( rf_temp_backoff_active == TRUE )    
           ) 
            backoff_needed = TRUE;
      }

      /* update VBATT() reading */
      now_volt_adc = vbatt_read();

#ifdef TEST_PA_BACKOFF
      MSG_FATAL("Refresh backoff requested, VBATT=%d", now_volt_adc, 0, 0 );
#endif

      break ;

      /* IS-95 and other configurations with no
      ** backoff requirement.
      */
    case NO_BACK_OFF_CONFIG:
    default:

      /* reset state variables */
      backoff_needed = FALSE ;
      save_chan_config.backoff_config = NO_BACK_OFF_CONFIG ;
      first_chan_config = BACKOFF_FIRST_RUN ;
#ifdef RF_DISABLE_HDET_PER_CHAN_CONFIG
      rf_disable_hdet = FALSE ;
#endif
      current_hdet_offset  = 0;
      rf_digital_backoff_active = FALSE ;
      rf_voltage_backoff_active  = FALSE ;
      rf_temp_backoff_active  = FALSE ;
      save_voltage_backoff_config  = &no_voltage_backoff;

      break ;

      /* 
      ** Channel configurations that may require backoff
      */
    case BACKOFF_CHAN_CONFIG_VALID:

      save_chan_config = *chan_config; 
      first_chan_config = BACKOFF_STEADY_STATE ;

#ifdef RF_DISABLE_HDET_PER_CHAN_CONFIG
      rf_disable_hdet = get_hdet_disable( chan_config );
#endif /* RF_DISABLE_HDET_PER_CHAN_CONFIG */

#ifdef RF_SET_HDET_OFFSET_PER_CHAN_CONFIG
      set_current_hdet_offset( chan_config );
#endif

      rf_digital_backoff_active = get_digital_backoff_needed( chan_config );

      rf_voltage_backoff_active = get_voltage_backoff_needed( chan_config, &save_voltage_backoff_config );

      rf_temp_backoff_active = get_temp_backoff_needed( chan_config );

      if ( ( rf_digital_backoff_active == TRUE ) || 
           ( rf_voltage_backoff_active == TRUE ) ||
           ( rf_temp_backoff_active == TRUE )    
         )
        backoff_needed = TRUE;

      break;

    }

    /* compute back off if required */
    if( backoff_needed )
    {

      /* Is voltage backoff needed */
      if ( rf_voltage_backoff_active == TRUE ) 
      {
         /*  voltage adjustment in units of RF_COUNTS_PER_DB times 8 */
         rf_pa_backoff_adjust_volt = rf_get_voltage_backoff( save_voltage_backoff_config );

         /* Remove scaling.*/
         rf_ba_backoff_adjust_total = (rf_pa_backoff_adjust_volt>>3) ;

      }

      /* Is temperature backoff needed */
      if ( rf_temp_backoff_active == TRUE ) 
      {
         /*  temperature adjustment in units of RF_COUNTS_PER_DB times 8 */
         rf_pa_backoff_adjust_temp = rf_get_temperature_backoff();

         /* Add to existing backoff (if any) */
         rf_ba_backoff_adjust_total += (rf_pa_backoff_adjust_temp>>3);
      }

      /* If we have reduced digital gain, add it back in here.
         rf_ba_backoff_adjust_total indicates a back-off value, so to reduce
         backoff we subtract a positive number. 
      */
      if( rf_digital_backoff_active == TRUE )
      {

        if ( rf_get_cagc_linearizer_range() == RF_HAS_102DB_CDMA_DYNAMIC_RANGE )
          rf_ba_backoff_adjust_total -= RF_DIGITAL_GAIN_FACTOR_102_DB;
        else
          rf_ba_backoff_adjust_total -= RF_DIGITAL_GAIN_FACTOR_85_DB;
      }

#ifdef TEST_PA_BACKOFF
         MSG_FATAL("Update_tx...: volt=%d, temp=%d, Dig=%d", rf_pa_backoff_adjust_volt>>3, 
                                                             rf_pa_backoff_adjust_temp>>3, 
                                                             rf_digital_backoff_active    );
#endif

      /* Calculate the backoff as an offset from the previous
      ** backoff so we don't simply accumulate gain adjustments
      ** from frame to frame.
      */

      rf_pa_backoff_accumulator_diff = rf_ba_backoff_adjust_total - rf_pa_backoff_accumulator;

      if ( rf_pa_backoff_accumulator_diff != 0 ) 
          rf_pa_backoff_accumulator = rf_ba_backoff_adjust_total;

    }
    else
    {
      /* Configuration requires no back-off.
      ** Restore the gain to its previous value at the next frame. 
      */

      if( rf_pa_backoff_accumulator != 0 )
      {
        rf_pa_backoff_accumulator_diff = -rf_pa_backoff_accumulator ;
        rf_pa_backoff_accumulator = 0 ;
      }
      else  rf_pa_backoff_accumulator_diff = 0;

    }

    if ( rf_pa_backoff_accumulator_diff != 0 ) 
    {

      /* get the ATMR resource */
      rf_acquire_atmr_resource();

      /* did we get the ATMR resource? */
      if ( atmr_handle !=  ATMR_INVALID_HANDLE ) 
      {
        /* determine what type of adjustment to the power limit is needed */
        if ( rf_pa_backoff_accumulator_diff > 0 ) 
        {
          /* Adjust power limit lower close to the end of this frame. Tune the 
          ** last parameter to get close to the boundary.  0 through 23 in units 
          ** of 64 chips. 
          */
          atmr_set( atmr_handle,
                    pa_backoff_adjust_tx_power_limit,
                    ATMR_5MS_FRAME(4),
                    ATMR_PCG_OFFSET(4),
                    0 
                  );

        }
        else
        {
          /* Adjust power limit lower close to the start of the next frame.
          ** Tune the last parameter to get close to the boundary. 0 through 23 in 
          ** units of 64 chips. 
          */
          atmr_set( atmr_handle,
                    pa_backoff_adjust_tx_power_limit,
                    ATMR_5MS_FRAME(1),
                    ATMR_PCG_OFFSET(1),
                    0 
                  ) ;
        }

      }
      else
      {
        /* couldn't get ATMR handle, we'll do the adjustment to the power limit here */

        int16 new_tx_power_limit;

        /* caclulate new TX power limit */
        new_tx_power_limit = rf_tx_pwr_limit - rf_pa_backoff_accumulator_diff;

        /* update the TX power limit */
        rf_set_tx_pwr_limit( new_tx_power_limit );

        #ifdef RF_HAS_MDSP_TX_AGC
        /*Indicates to MDSP that Max TX Power Limit is
       latched to be picked up by MDSP at Frame
       boundary*/
        CDMA_MDSP_NEW_TX_LIMIT_FRAME_PARAMS_AVAILABLE();
        #endif 
        
        /* If we've commanded more power than possible, adjust the accumulator
        ** by the overshoot. Since backoff represents a power reduction we need
        ** to increase the accumulator by the overshoot.
        */
        if( rf_tx_pwr_limit == RF_TX_PWR_CALC_MAX)
        {
          rf_pa_backoff_accumulator += (new_tx_power_limit - RF_TX_PWR_CALC_MAX);
        }

        /* reset the accumulator difference */
        rf_pa_backoff_accumulator_diff =  0;

        /* PA backoff update of the TX power limit is done */
        tx_power_limit_update_pending = FALSE;

      }

    }
    else
    {
        /* No adjustment is necessary */
        tx_power_limit_update_pending = FALSE ;
    }

#ifdef TEST_PA_BACKOFF
         MSG_FATAL("Update_tx...: accum_diff=%d, type=%d, ATMR=%d", rf_pa_backoff_accumulator_diff,chan_config->backoff_config, tx_power_limit_update_pending );
#endif
}



/*==========================================================================

FUNCTION get_voltage_backoff_needed                        INTERNAL FUNCTION

DESCRIPTION

Returns a boolean indicating whether the channel configuration passed in by
chan_config requires a voltage backoff. voltage_config is set to point to
the voltage_backoff_type to be used for chan_config

DEPENDENCIES

None.

RETURN VALUE

None

SIDE EFFECTS
None

===========================================================================*/
boolean get_voltage_backoff_needed( rf_txc_chan_config_type *chan_config, 
                                    voltage_backoff_type **voltage_config )/*lint -esym(715, chan_config) -esym(715, voltage_config) */
{
    boolean voltage_backoff_needed = FALSE;

    /* determine if a voltage backoff is required based upon chan_config */

#ifdef RF_DISABLE_VOLTAGE_BACKOFF
    /* voltage backoff functionality is disabled */
    return voltage_backoff_needed;
#else
    /* determine if voltage backoff is needed per channel config */
#ifdef TEST_PA_BACKOFF_VOLTAGE_BACKOFF
    *voltage_config = &test_voltage_backoff;
    return TRUE;
#endif

#ifdef FEATURE_IS2000_P2
    if ( ( chan_config->dcch_on == TRUE ) && 
         ( ( chan_config->dcch_rc == CAI_RC_3 ) || 
           ( chan_config->dcch_rc == CAI_RC_4 ) 
         )
       )
    {

      if ( chan_config->fch_on == FALSE ) 
      {
        /* DCCH on, FCH off */
#ifdef FEATURE_IS2000_R_SCH
        if ( chan_config->sch_on == FALSE )
        {
          /* DCCH on, SCH off, FCH off (i.e., PICH + DCCH) */

          *voltage_config = &voltage_backoff_config_1;

          voltage_backoff_needed = TRUE;

        }
        else
        {
          /* DCCH on, SCH on, FCH off */
          if ( ( ( chan_config->sch_rc == CAI_RC_3 ) || 
                 ( chan_config->sch_rc == CAI_RC_4 ) 
               ) && 
               ( chan_config->sch_to_pch_over_6dB == FALSE )
             )
          {
            /* low rate SCH */

            *voltage_config = &voltage_backoff_config_2;

            voltage_backoff_needed = TRUE;
          }

        }
#else
        /* DCCH on, FCH off, SCH not supported */
        *voltage_config = &voltage_backoff_config_1;

        voltage_backoff_needed = TRUE;

#endif /* FEATURE_IS2000_R_SCH */
      }
      else
      {
        /* DCCH on, FCH on */

        *voltage_config = &voltage_backoff_config_1;
        
        voltage_backoff_needed = TRUE;
      }
    }
    else
    {
        /* DCCH off */
#ifdef FEATURE_IS2000_R_SCH
      if ( ( chan_config->fch_on == TRUE ) && 
           ( ( chan_config->fch_rc == CAI_RC_3 ) || ( chan_config->fch_rc == CAI_RC_4 ) ) &&
           ( chan_config->sch_on == TRUE ) && 
           ( ( chan_config->sch_rc == CAI_RC_3 ) || ( chan_config->sch_rc == CAI_RC_4 ) )
         )
      {
          /* DCCH off, SCH on, FCH on */

          /* amount of back off is dependent upon SCH rate */
          voltage_backoff_needed = TRUE;

          switch ( chan_config->sch_rate ) 
          {
          case ENC_SCH_8TH_RATE  :
          case ENC_SCH_QTR_RATE  :
          case ENC_SCH_HALF_RATE :
          case ENC_SCH_FULL_RATE :
          case ENC_SCH_2X_RATE :
              *voltage_config = &voltage_backoff_config_1;
              break;

          case ENC_SCH_4X_RATE :
          case ENC_SCH_8X_RATE :
              *voltage_config = &voltage_backoff_config_2;
              break;

          case ENC_SCH_16X_RATE :
              *voltage_config = &voltage_backoff_config_3;
              break;

          default:
              voltage_backoff_needed = FALSE;
              break;
                 
          }

      }
#endif /* FEATURE_IS2000_R_SCH */ 
    }
#else
    /* !FEATURE_IS2000_P2 */
#ifdef FEATURE_IS2000_R_SCH
    if ( ( chan_config->fch_on == TRUE ) && 
         ( ( chan_config->fch_rc == CAI_RC_3 ) || ( chan_config->fch_rc == CAI_RC_4 ) ) &&
         ( chan_config->sch_on == TRUE ) && 
         ( ( chan_config->sch_rc == CAI_RC_3 ) || ( chan_config->sch_rc == CAI_RC_4 ) )
       )
    {
      /* DCCH off, SCH on, FCH on */

      /* amount of back off is dependent upon SCH rate */
      voltage_backoff_needed = TRUE;

      switch ( chan_config->sch_rate ) 
      {
      case ENC_SCH_8TH_RATE  :
      case ENC_SCH_QTR_RATE  :
      case ENC_SCH_HALF_RATE :
      case ENC_SCH_FULL_RATE :
      case ENC_SCH_2X_RATE :
        *voltage_config = &voltage_backoff_config_1;
        break;

      case ENC_SCH_4X_RATE :
      case ENC_SCH_8X_RATE :
        *voltage_config = &voltage_backoff_config_2;
        break;

      case ENC_SCH_16X_RATE :
        *voltage_config = &voltage_backoff_config_3;
        break;

      default:
        voltage_backoff_needed = FALSE;
        break;
                 
      }

    }
#endif /* FEATURE_IS2000_R_SCH */
#endif /* FEATURE_IS2000_P2 */
  
  if ( voltage_backoff_needed == FALSE )
    *voltage_config = &no_voltage_backoff;

  return voltage_backoff_needed;

#endif /* RF_DISABLE_VOLTAGE_BACKOFF */
}/*lint !e818 */


/*==========================================================================

FUNCTION get_temp_backoff_needed                        INTERNAL FUNCTION

DESCRIPTION

Returns a boolean indicating whether the channel configuration passed in by
chan_config requires a temperature backoff. 

DEPENDENCIES

None.

RETURN VALUE

None

SIDE EFFECTS
None

===========================================================================*/
boolean get_temp_backoff_needed( rf_txc_chan_config_type *chan_config )
{
    /* determine if a temperature backoff is required based upon chan_config */

    return FALSE;

}/*lint !e818 */



/*==========================================================================

FUNCTION get_digital_backoff_needed                        INTERNAL FUNCTION

DESCRIPTION

Returns a boolean indicating whether the channel configuration passed in by
chan_config requires a digital backoff

DEPENDENCIES

None.

RETURN VALUE

None

SIDE EFFECTS
None

===========================================================================*/
boolean get_digital_backoff_needed( rf_txc_chan_config_type *chan_config )
{

#ifdef FEATURE_IS2000_P2
    if ( ( chan_config->dcch_on == TRUE ) && 
         ( chan_config->fch_on == FALSE ) &&
         ( ( chan_config->dcch_rc == CAI_RC_3 ) || ( chan_config->dcch_rc == CAI_RC_4 ) )
       )
    {
#ifdef FEATURE_IS2000_R_SCH
       if ( ( chan_config->sch_on == FALSE ) || 
            ( 
              ( chan_config->sch_on == TRUE ) &&
              ( ( chan_config->sch_rc == CAI_RC_3 ) || ( chan_config->sch_rc == CAI_RC_4 ) ) &&
              ( chan_config->sch_to_pch_over_6dB == FALSE )
            )
          )
          return TRUE;    /* PICH + DCCH or PICH + DCCH + low rate SCH */
       else
          return FALSE;
#else
       return TRUE; /* PICH + DCCH */
#endif /* FEATURE_IS2000_R_SCH */
    }
    else
       return FALSE;
#else
    return FALSE;
#endif /* FEATURE_IS2000_P2 */

}/*lint !e818 */


#ifdef RF_DISABLE_HDET_PER_CHAN_CONFIG
/*==========================================================================

FUNCTION get_hdet_disable                                  INTERNAL FUNCTION

DESCRIPTION

Returns a boolean indicating whether the hdet reading is reliable for the 
channel configuration passed in chan_config.  If the hdet reading cannot be 
used for the channel configuration the hdet reading will be used in the
power limiting loop.

This function currently disables HDET for the following channel 
configurations:

Whenever DCCH is active

DEPENDENCIES

None.

RETURN VALUE

None

SIDE EFFECTS
None

===========================================================================*/
boolean get_hdet_disable( rf_txc_chan_config_type *chan_config )
{
     
    boolean disable_hdet = FALSE;

/*
**  Example of disabling HDET based upon channel configuration. In this example
**  HDET is disabled for channel configurations that also have digital gain
**  backoff
*/
/*
#ifdef FEATURE_IS2000_P2
    if ( ( chan_config->dcch_on == TRUE ) && 
         ( chan_config->fch_on == FALSE ) &&
         ( ( chan_config->dcch_rc == CAI_RC_3 ) || ( chan_config->dcch_rc == CAI_RC_4 ) )
       )
    {
#ifdef FEATURE_IS2000_R_SCH
       if ( ( chan_config->sch_on == FALSE ) || 
            ( 
              ( chan_config->sch_on == TRUE ) &&
              ( ( chan_config->sch_rc == CAI_RC_3 ) || ( chan_config->sch_rc == CAI_RC_4 ) ) &&
              ( chan_config->sch_to_pch_over_6dB == FALSE )
            )
          )
          disable_hdet = TRUE;
#else
       disable_hdet = TRUE;
#endif
    }
#endif
*/

#ifdef FEATURE_IS2000_P2
    /* Don't use HDET when DCCH is active */
    if ( chan_config->dcch_on == TRUE ) 
    {
       disable_hdet = TRUE; 
    }
#endif /* FEATURE_IS2000_P2 */

    return disable_hdet;

}/*lint !e818 */
#endif



#ifdef RF_SET_HDET_OFFSET_PER_CHAN_CONFIG
/*==========================================================================

FUNCTION set_current_hdet_offset                          INTERNAL FUNCTION

DESCRIPTION

Determines the amount of offset to add to the HDET reading based upon 
channel configuration.  The offset will be applied in the power limiting
algorithm  

DEPENDENCIES

None.

RETURN VALUE

None

SIDE EFFECTS
None

===========================================================================*/
void set_current_hdet_offset( rf_txc_chan_config_type *chan_config )
{
#ifdef TEST_PA_BACKOFF_HDET_OFFSET
current_hdet_offset = hdet_offset_override;
#else
/* determine offset to apply to HDET reading according to channel 
** configuration 
*/
current_hdet_offset = 0;
#endif
}
#endif


/*===========================================================================

FUNCTION rf_get_tx_pwr_limit_adj                      EXTERNALIZED FUNCTION

DESCRIPTION

Returns maximum power based on channel configuration, temperature, and
battery voltage. 

The pointer to the input structure tx_pwr_limit_adj passed in by the caller 
is returned with two values in 1/12 dB resolution:

tx_pwr_limit_adj->tx_pwr_limit : What the TX Power limit will be based upon
                                 the channel configuration in chan_config                                 
                                 
tx_pwr_limit_adj->tx_pwr_limit_adj: The amount of adjustment included in 
                                    tx_pwr_limit_adj->tx_pwr_limit due to
                                    channel configuration in chan_config

DEPENDENCIES

None.

RETURN VALUE

None

SIDE EFFECTS
None

===========================================================================*/
void rf_get_tx_pwr_limit_adj( rf_txc_chan_config_type *chan_config,
                              rf_txc_pwr_limit_type *tx_pwr_limit_adj )
{
    boolean backoff_needed = FALSE;
    boolean rf_digital_backoff_needed = FALSE; 
    boolean voltage_backoff_needed = FALSE;
    boolean temp_backoff_needed = FALSE;
    voltage_backoff_type *test_voltage_backoff_config = &no_voltage_backoff;

    int16 new_tx_pwr_limit;

    int16 pa_backoff_adjust_total = 0;

    uint16 rf_pa_backoff_adjust_volt = 0; 
    uint16 rf_pa_backoff_adjust_temp = 0;

    int16 sampled_rf_tx_pwr_limit, sampled_rf_pa_backoff_accumulator;


    switch ( chan_config->backoff_config )
    {
    case NO_BACK_OFF_CONFIG:

      break ;

    case BACKOFF_CHAN_CONFIG_VALID:

      /* determine what types of backoff this channel configuration requires */
      rf_digital_backoff_needed = get_digital_backoff_needed( chan_config );
      
      voltage_backoff_needed = get_voltage_backoff_needed( chan_config, 
                                                           &test_voltage_backoff_config );

      temp_backoff_needed = get_temp_backoff_needed( chan_config );

      if ( ( rf_digital_backoff_needed == TRUE ) || 
           ( voltage_backoff_needed == TRUE ) ||
           ( temp_backoff_needed == TRUE )    
         )
        backoff_needed = TRUE;

      break;

    default:

      MSG_ERROR("Invalid backoff config %d", chan_config->backoff_config, 0, 0 );

      /* Return last value...always return in 1/12 dB resolution  */
      tx_pwr_limit_adj->tx_pwr_limit = 
          ( rf_get_cagc_linearizer_range() == RF_HAS_102DB_CDMA_DYNAMIC_RANGE ) ? 
            rf_rescale_tx_pwr_limit( rf_tx_pwr_limit ) : rf_tx_pwr_limit; /*lint !e732 !e737 loss of sign */

      tx_pwr_limit_adj->tx_pwr_limit_adj = 
          ( rf_get_cagc_linearizer_range() == RF_HAS_102DB_CDMA_DYNAMIC_RANGE ) ?
            rf_pa_backoff_accumulator * 12/10 : rf_pa_backoff_accumulator; /*lint !e734 loss of precision */

      return;
    }


    /* Sample our global variables that are subject to change */
    INTLOCK();
    sampled_rf_tx_pwr_limit = rf_tx_pwr_limit; 
    sampled_rf_pa_backoff_accumulator = rf_pa_backoff_accumulator;
    INTFREE();


    /* calculate backoff (if there is any) for this channel configuration */
    if( backoff_needed )
    {
      
      if ( voltage_backoff_needed == TRUE ) 
      {
        /*  voltage adjustments in units of RF_COUNTS_PER_DB times 8 */
        rf_pa_backoff_adjust_volt  = rf_get_voltage_backoff( test_voltage_backoff_config ) ;

        /* Remove scaling.*/
        pa_backoff_adjust_total = rf_pa_backoff_adjust_volt>>3;

      }

      /* Is temperature backoff needed */
      if ( temp_backoff_needed == TRUE ) 
      {
         rf_pa_backoff_adjust_temp = rf_get_temperature_backoff();

         /* Add to existing backoff (if any) */
         pa_backoff_adjust_total += rf_pa_backoff_adjust_temp>>3;
      }


      /* If we have reduced digital gain, add it back in here.
         rf_gl_adj_total indicates a back-off value, so to reduce
         backoff we subtract a positive number. 
      */
      if( rf_digital_backoff_needed == TRUE )
      {

        if ( rf_get_cagc_linearizer_range() == RF_HAS_102DB_CDMA_DYNAMIC_RANGE )
          pa_backoff_adjust_total -= RF_DIGITAL_GAIN_FACTOR_102_DB;
        else
          pa_backoff_adjust_total -= RF_DIGITAL_GAIN_FACTOR_85_DB;
      }
 
#ifdef TEST_PA_BACKOFF
      MSG_FATAL("get_tx_pwr...: volt=%d, temp=%d, Dig=%d", rf_pa_backoff_adjust_volt>>3, 
                                                           rf_pa_backoff_adjust_temp>>3, 
                                                           rf_digital_backoff_needed    );
#endif

      /* calculate new tx power limit */
      new_tx_pwr_limit = sampled_rf_tx_pwr_limit + sampled_rf_pa_backoff_accumulator 
                           - pa_backoff_adjust_total;

    }
    else
    {
      /* Configuration requires no back-off.*/
      new_tx_pwr_limit = sampled_rf_tx_pwr_limit + sampled_rf_pa_backoff_accumulator;

      /* adjustment is 0 */
      pa_backoff_adjust_total = 0;

    }

    /* always return in 1/12 dB resolution  */
    tx_pwr_limit_adj->tx_pwr_limit = 
        ( rf_get_cagc_linearizer_range() == RF_HAS_102DB_CDMA_DYNAMIC_RANGE ) ? 
          rf_rescale_tx_pwr_limit( new_tx_pwr_limit ) : new_tx_pwr_limit; /*lint !e732 !e737 Loss of sign */

    tx_pwr_limit_adj->tx_pwr_limit_adj = 
        ( rf_get_cagc_linearizer_range() == RF_HAS_102DB_CDMA_DYNAMIC_RANGE ) ?
          pa_backoff_adjust_total * 12/10 : pa_backoff_adjust_total; /*lint !e734 Loss of precision */

#ifdef TEST_PA_BACKOFF
    MSG_FATAL("102_limit=%d, 102_adj=%d, 85_limit=%d", new_tx_pwr_limit, pa_backoff_adjust_total, 
              tx_pwr_limit_adj->tx_pwr_limit);
#endif

    return;
}


/*===========================================================================

FUNCTION rf_get_voltage_backoff                          INTERNAL FUNCTION

DESCRIPTION

Return the number of dB the maximum Power Amplifier gain should be reduced
as a function of channel configuration and voltage.  

The returned number is (backoff in dB * RF_COUNTS_PER_DB * 8) to preserve fractional 
numbers. For 102 dB dynamic range RF_COUNTS_PER_DB is 10 
(RF_COUNTS_PER_DB_102_DB/16). For 85 dB dynamic range RF_COUNTS_PER_DB is 12 
(RF_COUNTS_PER_DB_85_DB/16). 

There are 3 voltages at which the backoffs are calibrated. This procedure
does a piecewise linear interpolation of the dB backoff values implied by
the voltages. The calculations are done in a step-by-step manner to 
inhibit compiler optimization that might throw away needed precision.


DEPENDENCIES

The following RF NV items will be used:
rfnv_vbatt_cal_volts.max
rfnv_vbatt_cal_volts.min

RETURN VALUE

uint16 : (backoff in dB) * (RF_COUNTS_PER_DB_xxx_DB)*8

SIDE EFFECTS

None.

===========================================================================*/
uint16 rf_get_voltage_backoff(voltage_backoff_type *voltage_config )
{
  uint32 retdB_backoff = 0;
  uint32 now_volt;
  int32  tmp = 0;

  rf_chain_state_type *rf_chain_ptr = 
      &rf_chain_status[ rfcom_to_path[ RFCOM_TRANSCEIVER_0 ] ];

#ifdef TEST_SIMULATE_VBATT
  now_volt_adc = vbatt_override;
#endif

  /* If we haven't computed the slopes and saturation points for this voltage
  ** backoff configuration we'll do it one time and save the results to use
  ** in the future.  Slopes are always less than or equal to zero because the
  ** amount of backoff increases (or stays the same) for decreasing voltage.
  ** slope_high_to_mid should always be initialized to > 0.  This tells the
  ** function that the slopes and saturation points have not yet been calculated
  */
  if ( voltage_config->slope_high_to_mid > 0 ) 
  {
      uint32 vbatt_max_volts = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_vbatt_cal_volts.max * RF_PA_BACKOFF_VOLTAGE_DATA_SCALE;
      uint32 vbatt_min_volts = rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_vbatt_cal_volts.min * RF_PA_BACKOFF_VOLTAGE_DATA_SCALE;


      /* Check the validity of the voltage back off configuration
      ** The amount of back off must not decrease with increasing 
      ** battery voltage
      */
      if ( voltage_config->med_volt_dB >= voltage_config->high_volt_dB ) 
      {
          if ( voltage_config->low_volt_dB < voltage_config->med_volt_dB ) 
          {
              ERR_FATAL("PA Backoff: invalid voltage back off configuration",0 ,0, 0);
          }
      }
      else
          ERR_FATAL("PA Backoff: invalid voltage back off configuration",0 ,0, 0);

      /* Properly specified voltage back offs should specify the max voltage for back off
      ** to be less than the maximum calibrated battery voltage
      */
      if ( voltage_config->max_volt > vbatt_max_volts ) 
      {
          MSG_ERROR("PA Backoff max volts (%d) > vbatt max volts (%d)", voltage_config->max_volt,
                    vbatt_max_volts, 0);

          /* we'll just start the voltage backoff at the maximum battery voltage */
          voltage_config->max_volt = vbatt_max_volts;
      }

      /* Properly specified voltage back offs should specify the min voltage for back off
      ** to be greater than (or equal to) the minimum calibrated battery voltage
      */
      if ( voltage_config->min_volt < vbatt_min_volts ) 
      {
          MSG_ERROR("PA Backoff min volts (%d) < vbatt min volts (%d)", voltage_config->max_volt,
                    vbatt_min_volts, 0);

          /* we'll just end the voltage backoff at the minimum battery voltage */
          voltage_config->min_volt = vbatt_min_volts;
      }

      /* Now check the validity of the voltages specified in the configuration */
      if ( voltage_config->mid_volt < voltage_config->max_volt ) 
      {
          if ( voltage_config->min_volt > voltage_config->mid_volt ) 
          {
              ERR_FATAL("PA Backoff: invalid voltage back off configuration",0 ,0, 0);
          }
      }
      else
          ERR_FATAL("PA Backoff: invalid voltage back off configuration",0 ,0, 0);


      /* Compute and save slopes for this voltage configuration.  Because we are
      ** doing integer math the numerator is scaled by RF_PA_BACKOFF_SCALE 
      ** to increase precision
      */

      /* First compute slope for high to mid line */
      tmp = RF_PA_BACKOFF_SCALE * ((int32)voltage_config->med_volt_dB - (int32)voltage_config->high_volt_dB);

      tmp =  tmp/( (int32)voltage_config->mid_volt - (int32)voltage_config->max_volt );

      voltage_config->slope_high_to_mid = tmp;

      /* Now compute slope for mid to low line */
      tmp = RF_PA_BACKOFF_SCALE * ((int32)voltage_config->low_volt_dB - (int32)voltage_config->med_volt_dB);

      tmp = tmp/( (int32)voltage_config->min_volt - (int32)voltage_config->mid_volt );

      voltage_config->slope_mid_to_low = tmp;

  }


  /* Compute the amount of back off to the TX power limit according to the last battery 
  ** voltage reading and the corrent channel configuration.
  */

  /* Convert ADC counts to Volts*10. First, ADC times volt range of calibration. */
  now_volt = ( (now_volt_adc - VBATT_SCALED_MIN) *(rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_vbatt_cal_volts.max-
               rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_vbatt_cal_volts.min)) ; 
  
  /* Then scale to volts*100 from volts*10. */
  now_volt = now_volt * RF_PA_BACKOFF_VOLTAGE_DATA_SCALE ; 
  
  /* Then divide by normalized adc range of cal. */
  now_volt = now_volt / (VBATT_SCALE - VBATT_SCALED_MIN) ; 
  
  /* Finally, add base calibration voltage. */
  now_volt = now_volt + (RF_PA_BACKOFF_VOLTAGE_DATA_SCALE * 
               rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_vbatt_cal_volts.min) ; 
  
  
  /* Saturate the voltage read to voltages specified in voltage_config */
  if( now_volt < voltage_config->min_volt )
  {
    now_volt = voltage_config->min_volt;
  }
  else if ( now_volt > voltage_config->max_volt )
  {
    now_volt = voltage_config->max_volt;
  }

  /* Find which line segment we are on */
  if( now_volt > voltage_config->mid_volt )  
  { 
    /* find the amount of backoff using straight line equation */ 
    tmp = ( voltage_config->slope_high_to_mid ) * ( (int32)now_volt - (int32)voltage_config->mid_volt );
    tmp /= RF_PA_BACKOFF_SCALE;
    tmp += (int32)voltage_config->med_volt_dB;
    tmp /= RF_PA_BACKOFF_SCALE;

    retdB_backoff = (uint16) tmp ;
  }
  else
  {
    /* find the amount of backoff using straight line equation */ 
    tmp = ( voltage_config->slope_mid_to_low ) * ( (int32)now_volt - (int32)voltage_config->min_volt );
    tmp /= RF_PA_BACKOFF_SCALE;
    tmp += (int32)voltage_config->low_volt_dB;
    tmp /= RF_PA_BACKOFF_SCALE;

    retdB_backoff = (uint16) tmp ;
  }
  /* Return a non-negative number between 0 and MAX_PA_BACKOFF*RF_COUNTS_PER_DB. */
  if ( rf_get_cagc_linearizer_range() == RF_HAS_102DB_CDMA_DYNAMIC_RANGE )
    return (uint16) ( (retdB_backoff*RF_COUNTS_PER_DB_102_DB) >> 4 );
  else
    return (uint16) ( (retdB_backoff*RF_COUNTS_PER_DB_85_DB) >> 4 );
}



/*===========================================================================

FUNCTION rf_get_temperature_backoff                       INTERNAL FUNCTION

DESCRIPTION

Currently not implemented.

Return the number of dB the maximum Power Amplifier gain should be reduced
as a function of temperature.

The returned number is (backoff in dB * RF_COUNTS_PER_DB * 8) to preserve fractional 
numbers. For 102 dB dynamic range RF_COUNTS_PER_DB is 10 
(RF_COUNTS_PER_DB_102_DB/16). For 85 dB dynamic range RF_COUNTS_PER_DB is 12 
(RF_COUNTS_PER_DB_85_DB/16). 

DEPENDENCIES


RETURN VALUE

uint16 : (backoff in dB) * (RF_COUNTS_PER_DB_DB)*8

SIDE EFFECTS

None.

===========================================================================*/
uint16 rf_get_temperature_backoff( void )
{
  uint32 retdB_backoff = 0 ;

  /* Return a non-negative number between 0 and MAX_PA_BACKOFF*RF_COUNTS_PER_DB. */
  if ( rf_get_cagc_linearizer_range() == RF_HAS_102DB_CDMA_DYNAMIC_RANGE )
    return (uint16)( (retdB_backoff*RF_COUNTS_PER_DB_102_DB) >> 4 ) ;
  else
    return (uint16)( (retdB_backoff*RF_COUNTS_PER_DB_85_DB) >> 4 ) ;
}


/*===========================================================================

FUNCTION pa_backoff_adjust_tx_power_limit                 INTERNAL FUNCTION

DESCRIPTION

Procedure that updates rf_tx_pwr_limit according to channel configuration. 

It is not called directly. Rather the procedure

  rf_update_tx_pwr_limit()

schedules its execution through an ATMR resource. 

DEPENDENCIES

None.

RETURN VALUE

None.

SIDE EFFECTS

The ATMR resource is released.

===========================================================================*/
void pa_backoff_adjust_tx_power_limit( void )
{
  
  /* only allow this function to be called by rf_update_tx_pwr_limit() through
  ** an ATMR event
  */
  if ( atmr_handle == ATMR_INVALID_HANDLE ) return;

  if ( tx_power_limit_update_pending )
  {
     int16 new_tx_power_limit;

     new_tx_power_limit = rf_tx_pwr_limit - rf_pa_backoff_accumulator_diff;

       /* update the TX power limit */
     rf_set_tx_pwr_limit( new_tx_power_limit );

       #ifdef RF_HAS_MDSP_TX_AGC
       /*Indicates to MDSP that Max TX Power Limit is
       latched to be picked up by MDSP at Frame
       boundary*/
       CDMA_MDSP_NEW_TX_LIMIT_FRAME_PARAMS_AVAILABLE();
       #endif 
    
     /* If we've commanded more power than possible, adjust the accumulator
     ** by the overshoot. Since backoff represents a power reduction we need
     ** to increase the accumulator by the overshoot.
     */
     if( rf_tx_pwr_limit == RF_TX_PWR_CALC_MAX)
     {
       rf_pa_backoff_accumulator += (new_tx_power_limit - RF_TX_PWR_CALC_MAX);
     }

     /* reset the accumulator difference */
     rf_pa_backoff_accumulator_diff =  0;

     /* PA backoff update of the TX power limit is done */
     tx_power_limit_update_pending = FALSE;

#ifdef TEST_PA_BACKOFF
     MSG_FATAL("ATMR: PWR LMT=%d, ACC=%d", rf_tx_pwr_limit, rf_pa_backoff_accumulator, 0); 
#endif
  }

     /* Release atmr resource. */
  INTLOCK();

  atmr_set( atmr_handle,
            NULL,
            ATMR_5MS_FRAME( NONE ),
            ATMR_PCG_OFFSET_1,
            0
          );

  INTFREE();

  atmr_handle = ATMR_INVALID_HANDLE;

}


/*===========================================================================

FUNCTION rf_acquire_atmr_resource                           INTERNAL FUNCTION

DESCRIPTION

Utility that allocates an ATMR resource.

DEPENDENCIES

None.

RETURN VALUE

None.

SIDE EFFECTS

The ATMR handle is assigned.
The ATMR resource is allocated.

===========================================================================*/
void rf_acquire_atmr_resource( void )
{

  /* Try to get the ATMR.  If we can't log a error.  The adjustment to 
  ** the power limit will occur immediately 
  */

  INTLOCK();

  atmr_handle = atmr_get( ATMR_TIME_1 ) ;

  INTFREE();

  if( atmr_handle == ATMR_INVALID_HANDLE )
  {
    MSG_ERROR( "PA Backoff module unable to acquire atmr handle",0,0,0 ) ;
  }
    
  
}

/*===========================================================================

FUNCTION rf_reset_pa_backoff_module                      EXTERNALIZED FUNCTION

DESCRIPTION

Utility that initializes the gain reduction algorithm.
Should be called with parameter of 0 at the start of a call.
Non-zero values are for debugging.

DEPENDENCIES

None.

RETURN VALUE

None.

SIDE EFFECTS
The state variables for the PA Backoff module are initialized

===========================================================================*/
void rf_reset_pa_backoff_module( uint16 rf_reset_acc_val )/*lint -esym(715, rf_reset_acc_val) unused */
{

  /* reset all internal PA Backoff module variables */
  rf_pa_backoff_accumulator = 0;
  rf_pa_backoff_accumulator_diff = 0;
  current_hdet_offset = 0;
  save_chan_config.backoff_config = NO_BACK_OFF_CONFIG;
  first_chan_config = BACKOFF_FIRST_RUN;
  rf_backoff_refresh_counter = 0;
  tx_power_limit_update_pending = FALSE;
  rf_disable_hdet = FALSE;
  rf_digital_backoff_active = FALSE;
  rf_voltage_backoff_active = FALSE;
  rf_temp_backoff_active = FALSE;
  save_voltage_backoff_config = &no_voltage_backoff;

  /* update the PA Backoff module battery reading */
  now_volt_adc = vbatt_read();

#ifdef TEST_PA_BACKOFF
  if (!call_back_initialized)
  {
    rf_pa_backoff_test_cb_init();
    call_back_initialized = TRUE;
  }
#endif

}





#ifdef TEST_PA_BACKOFF
/*===========================================================================

FUNCTION rf_pa_backoff_test_cb_init                        INTERNAL FUNCTION

DESCRIPTION
   This function starts the RF PA Backoff test module. It starts
   the call back function.

DEPENDENCIES
  sbi_init()

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void rf_pa_backoff_test_cb_init(void)
{

  pa_backoff_test_data[0] = TEST_NO_COMMAND;

  clk_def( &rf_pa_backoff_call_back );

  /* Setup up the PA Backoff test callback function */
  clk_reg( &rf_pa_backoff_call_back,    /* pointer to the call back struct */
            rf_pa_backoff_test_cb,      /* addr of the callback func       */
            (int4)TEST_TIME_FIRST,      /* delay till first call back      */
            (int4)TEST_TIME_CB,         /* delay between repeat call backs */
            TRUE);                      /* do or do not repeat call backs  */


}




/*===========================================================================

FUNCTION rf_pa_backoff_test_cb                            INTERNAL FUNCTION

DESCRIPTION
   This function provides the test interface for performing unit (stand 
   alone) testing of the PA Backoff module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void rf_pa_backoff_test_cb( int4 dummy )
{
  rf_txc_chan_config_type test_chan_config;
  rf_txc_pwr_limit_type test_tx_pwr_limit_adj;
  uint16 test_rf_tx_pwr_limit;
  int16 test_gl_adj_accum;

#ifdef TEST_PA_BACKOFF_VOLTAGE_BACKOFF
  rf_chain_state_type *rf_chain_ptr = 
    &rf_chain_status[ rfcom_to_path[ RFCOM_TRANSCEIVER_0 ] ];
#endif


  switch ( pa_backoff_test_data[0] ) 
  {
  case TEST_NO_COMMAND:
      return;

  case TEST_GET_CURRENT_STATE:

      /* print out current state */
      INTLOCK();
      test_chan_config = save_chan_config;
      test_rf_tx_pwr_limit =  rf_tx_pwr_limit;
      test_gl_adj_accum =  rf_pa_backoff_accumulator;
      INTFREE();

      MSG_FATAL("GET_CURRENT_STATE: CFG=%d, pwr_limit=%d, accum=%d",test_chan_config.backoff_config, test_rf_tx_pwr_limit, test_gl_adj_accum);  

      pa_backoff_test_data[0] = TEST_NO_COMMAND;

      break;

  case TEST_GET_NEW_CHAN_CONFIG:
      test_chan_config.backoff_config = BACKOFF_CHAN_CONFIG_VALID;
      test_chan_config.fch_on = pa_backoff_test_data[1];
      test_chan_config.fch_rc = pa_backoff_test_data[2];
      test_chan_config.dcch_on = pa_backoff_test_data[3];
      test_chan_config.dcch_rc = pa_backoff_test_data[4];
      test_chan_config.sch_on = pa_backoff_test_data[5];
      test_chan_config.sch_rc = pa_backoff_test_data[6];
      test_chan_config.sch_rate = pa_backoff_test_data[7];
      test_chan_config.sch_to_pch_over_6dB = pa_backoff_test_data[8];

      MSG_FATAL("GET_NEW_CHAN_CONFIG", 0, 0, 0);  

      INTLOCK();
      rf_get_tx_pwr_limit_adj( &test_chan_config, &test_tx_pwr_limit_adj );
      INTFREE();

      pa_backoff_test_data[0] = TEST_NO_COMMAND;

      break;

  case TEST_SET_NEW_CHAN_CONFIG:
      test_chan_config.backoff_config = BACKOFF_CHAN_CONFIG_VALID;
      test_chan_config.fch_on = pa_backoff_test_data[1];
      test_chan_config.fch_rc = pa_backoff_test_data[2];
      test_chan_config.dcch_on = pa_backoff_test_data[3];
      test_chan_config.dcch_rc = pa_backoff_test_data[4];
      test_chan_config.sch_on = pa_backoff_test_data[5];
      test_chan_config.sch_rc = pa_backoff_test_data[6];
      test_chan_config.sch_rate = pa_backoff_test_data[7];
      test_chan_config.sch_to_pch_over_6dB = pa_backoff_test_data[8];

      MSG_FATAL("SET_NEW_CHAN_CONFIG",0,0,0);

      if ( tx_power_limit_update_pending ) 
       MSG_FATAL("Power limit update in progress, unable to update!!!",0,0,0);
      else
       rf_update_tx_pwr_limit( &test_chan_config );

      pa_backoff_test_data[0] = TEST_NO_COMMAND;

      break;

  case TEST_SET_VOLT_BACKOFF_PARAMS:

#ifdef TEST_PA_BACKOFF_VOLTAGE_BACKOFF
      /* low and medium voltage backoffs in dB*8*100 */
      test_voltage_backoff.low_volt_dB = 1600;    /* 2.0 dB */
      test_voltage_backoff.med_volt_dB = 1200;    /* 1.5 dB */
      test_voltage_backoff.high_volt_dB = 400;    /* 0.5 dB */

      /* upper & lower limit VBATT was cal'd at in volts*10 */
      rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_vbatt_cal_volts.max = 44;
      rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_vbatt_cal_volts.min = 24;

      /* voltages that PA Backoff cal was done in volts*100 */
      test_voltage_backoff.min_volt = 260;        /* 2.6 volts */
      test_voltage_backoff.mid_volt = 340;        /* 3.4 volts */
      test_voltage_backoff.max_volt = 420;        /* 4.2 volts */

      /* recalculate internal slopes & saturation points */
      test_voltage_backoff.slope_high_to_mid = 1;

      MSG_FATAL("SET_VOLT_BACKOFF_PARAMS",0,0,0);
#else
      MSG_FATAL( "TEST_PA_BACKOFF_VOLTAGE_BACKOFF is required for TEST_SET_VOLT_BACKOFF_GENERIC",0,0,0 );
#endif

      pa_backoff_test_data[0] = TEST_NO_COMMAND;

      break;

  case TEST_SET_VOLT_BACKOFF_GENERIC:

#ifdef TEST_PA_BACKOFF_VOLTAGE_BACKOFF
      /* low and medium voltage backoffs in dB*8 */
      test_voltage_backoff.low_volt_dB = pa_backoff_test_data[1]*100;
      test_voltage_backoff.med_volt_dB = pa_backoff_test_data[2]*100;
      test_voltage_backoff.high_volt_dB = pa_backoff_test_data[3]*100;

      /* upper & lower limit VBATT was cal'd at in volts*10 */
      rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_vbatt_cal_volts.max = pa_backoff_test_data[4];
      rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_vbatt_cal_volts.min = pa_backoff_test_data[5];

      /* voltages that PA Backoff cal was done in volts*10 */
      test_voltage_backoff.min_volt = pa_backoff_test_data[6]*10;
      test_voltage_backoff.mid_volt = pa_backoff_test_data[7]*10;
      test_voltage_backoff.max_volt = pa_backoff_test_data[8]*10;

      /* recalculate internal slopes & saturation points */
      test_voltage_backoff.slope_high_to_mid = 1;

      MSG_FATAL("SET_VOLT_BACKOFF_GENERIC",0,0,0);
#else
      MSG_FATAL( "TEST_PA_BACKOFF_VOLTAGE_BACKOFF is required for TEST_SET_VOLT_BACKOFF_GENERIC",0,0,0 );
#endif

      pa_backoff_test_data[0] = TEST_NO_COMMAND;

      break;

  case TEST_SET_VBATT_OVERRIDE:

#ifdef TEST_SIMULATE_VBATT

      if ( ( pa_backoff_test_data[1] < rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_vbatt_cal_volts.min) ||
           ( pa_backoff_test_data[1] > rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_vbatt_cal_volts.max)
         )
      {
          MSG_FATAL("Invalid battery voltage: %d, min=%d, max=%d", pa_backoff_test_data[1], 
                    rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_vbatt_cal_volts.min,
                    rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_vbatt_cal_volts.max);
      }
      else
      {
          /* in put is in volts * 10, convert this to corresponding ADC counts */
          vbatt_override = (pa_backoff_test_data[1] - rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_vbatt_cal_volts.min);
          vbatt_override = vbatt_override*(VBATT_SCALE - VBATT_SCALED_MIN);
          vbatt_override = vbatt_override/(rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_vbatt_cal_volts.max - rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_vbatt_cal_volts.min);

          MSG_FATAL("SET_VBATT_OVERRIDE: volt= %d, vbatt_override= %d", pa_backoff_test_data[1], vbatt_override, 0 ); 
      }
  
#else
      MSG_FATAL( "TEST_SIMULATE_VBATT must be defined to use TEST_SET_VBATT_OVERRIDE",0,0,0 );
#endif 

      pa_backoff_test_data[0] = TEST_NO_COMMAND;

      break;

  case TEST_GET_DCCH_PILOT_9600:

      test_chan_config.backoff_config = BACKOFF_CHAN_CONFIG_VALID;
      test_chan_config.fch_on = 0;
      test_chan_config.fch_rc = 0;
      test_chan_config.dcch_on = 1;
      test_chan_config.dcch_rc = 3;
      test_chan_config.sch_on = 0;
      test_chan_config.sch_rc = 0;
      test_chan_config.sch_rate = 0;
      test_chan_config.sch_to_pch_over_6dB = 0;

      MSG_FATAL("GET_DCCH_PILOT_9600", 0,0,0); 

      INTLOCK();
      rf_get_tx_pwr_limit_adj( &test_chan_config, &test_tx_pwr_limit_adj );
      INTFREE();

      pa_backoff_test_data[0] = TEST_NO_COMMAND;

      break;

  case TEST_SET_DCCH_PILOT_9600:

      test_chan_config.backoff_config = BACKOFF_CHAN_CONFIG_VALID;
      test_chan_config.fch_on = 0;
      test_chan_config.fch_rc = 0;
      test_chan_config.dcch_on = 1;
      test_chan_config.dcch_rc = 3;
      test_chan_config.sch_on = 0;
      test_chan_config.sch_rc = 0;
      test_chan_config.sch_rate = 0;
      test_chan_config.sch_to_pch_over_6dB = 0;

      MSG_FATAL("SET_DCCH_PILOT_9600", 0,0,0);

      if ( tx_power_limit_update_pending ) 
       MSG_FATAL("Power limit update in progress, unable to update!!!",0,0,0);
      else
       rf_update_tx_pwr_limit( &test_chan_config );

      pa_backoff_test_data[0] = TEST_NO_COMMAND;

      break;

  case TEST_SET_REFRESH_BACKOFF:

      test_chan_config.backoff_config = REFRESH_BACKOFF;

      MSG_FATAL("SET_REFRESH_BACKOFF", 0,0,0);

      if ( tx_power_limit_update_pending ) 
       MSG_FATAL("Power limit update in progress, unable to update!!!",0,0,0);
      else
       rf_update_tx_pwr_limit( &test_chan_config );


      pa_backoff_test_data[0] = TEST_NO_COMMAND;

      break;

  case TEST_SET_NO_BACKOFF:

      test_chan_config.backoff_config = NO_BACK_OFF_CONFIG;

      MSG_FATAL("SET_NO_BACKOFF", 0,0,0);

      if ( tx_power_limit_update_pending ) 
       MSG_FATAL("Power limit update in progress, unable to update!!!",0,0,0);
      else
       rf_update_tx_pwr_limit( &test_chan_config );


      pa_backoff_test_data[0] = TEST_NO_COMMAND;

      break;

  case TEST_GET_NO_BACKOFF:

      test_chan_config.backoff_config = NO_BACK_OFF_CONFIG;

      MSG_FATAL("GET_NO_BACKOFF", 0,0,0); 

      INTLOCK();
      rf_get_tx_pwr_limit_adj( &test_chan_config, &test_tx_pwr_limit_adj );
      INTFREE();

      pa_backoff_test_data[0] = TEST_NO_COMMAND;

      break;

  case TEST_GET_PILOT_FCH:

      test_chan_config.backoff_config = BACKOFF_CHAN_CONFIG_VALID;
      test_chan_config.fch_on = 1;
      test_chan_config.fch_rc = 1;
      test_chan_config.dcch_on = 0;
      test_chan_config.dcch_rc = 0;
      test_chan_config.sch_on = 0;
      test_chan_config.sch_rc = 0;
      test_chan_config.sch_rate = 0;
      test_chan_config.sch_to_pch_over_6dB = 0;

      MSG_FATAL("GET_PILOT_FCH", 0,0,0); 

      INTLOCK();
      rf_get_tx_pwr_limit_adj( &test_chan_config, &test_tx_pwr_limit_adj );
      INTFREE();

      pa_backoff_test_data[0] = TEST_NO_COMMAND;

      break;

  case TEST_SET_PILOT_FCH:

      test_chan_config.backoff_config = BACKOFF_CHAN_CONFIG_VALID;
      test_chan_config.fch_on = 1;
      test_chan_config.fch_rc = 1;
      test_chan_config.dcch_on = 0;
      test_chan_config.dcch_rc = 0;
      test_chan_config.sch_on = 0;
      test_chan_config.sch_rc = 0;
      test_chan_config.sch_rate = 0;
      test_chan_config.sch_to_pch_over_6dB = 0;

      MSG_FATAL("SET_PILOT_FCH", 0,0,0);

      if ( tx_power_limit_update_pending ) 
       MSG_FATAL("Power limit update in progress, unable to update!!!",0,0,0);
      else
       rf_update_tx_pwr_limit( &test_chan_config );

      pa_backoff_test_data[0] = TEST_NO_COMMAND;

      break;

  case TEST_GET_DCCH_PICH_SCH_TO_PICH_LESS_THAN_6_DB:

      test_chan_config.backoff_config = BACKOFF_CHAN_CONFIG_VALID;
      test_chan_config.fch_on = 0;
      test_chan_config.fch_rc = 0;
      test_chan_config.dcch_on = 1;
      test_chan_config.dcch_rc = 3;
      test_chan_config.sch_on = 1;
      test_chan_config.sch_rc = 3;
      test_chan_config.sch_rate = 0;
      test_chan_config.sch_to_pch_over_6dB = 0;

      MSG_FATAL("GET_DCCH_PICH_SCH_TO_PICH_LESS_THAN_6_DB", 0,0,0); 

      INTLOCK();
      rf_get_tx_pwr_limit_adj( &test_chan_config, &test_tx_pwr_limit_adj );
      INTFREE();

      pa_backoff_test_data[0] = TEST_NO_COMMAND;

      break;

  case TEST_SET_HDET_OVERRIDE:

#ifdef TEST_PA_BACKOFF_HDET_OFFSET
      hdet_offset_override = (int8)pa_backoff_test_data[1];

      MSG_FATAL("TEST_SET_HDET_OVERRIDE override= %d", hdet_offset_override,0,0);
#else
      MSG_FATAL("TEST_PA_BACKOFF_HDET_OFFSET is required for TEST_SET_HDET_OVERRIDE", 0,0,0);
#endif

      pa_backoff_test_data[0] = TEST_NO_COMMAND;

      break;

  case TEST_GET_TESTS:

      MSG_FATAL("TEST_GET_CURRENT_STATE ==> 2",0,0,0);
      MSG_FATAL("TEST_GET_NEW_CHAN_CONFIG ==> 3",0,0,0);
      MSG_FATAL("TEST_SET_NEW_CHAN_CONFIG ==> 4",0,0,0);
      MSG_FATAL("TEST_SET_VOLT_BACKOFF_PARAMS ==> 5",0,0,0);
      MSG_FATAL("TEST_SET_VBATT_OVERRIDE ==> 6",0,0,0);
      MSG_FATAL("TEST_GET_DCCH_PILOT_9600 ==> 7",0,0,0);
      MSG_FATAL("TEST_SET_DCCH_PILOT_9600 ==> 8",0,0,0);
      MSG_FATAL("TEST_SET_REFRESH_BACKOFF ==> 9",0,0,0);
      MSG_FATAL("TEST_SET_NO_BACKOFF ==> 10",0,0,0);
      MSG_FATAL("TEST_GET_NO_BACKOFF ==> 11",0,0,0);
      MSG_FATAL("TEST_GET_PILOT_FCH ==> 12",0,0,0);
      MSG_FATAL("TEST_SET_PILOT_FCH ==> 13",0,0,0);
      MSG_FATAL("TEST_GET_DCCH_PICH_SCH_TO_PICH_LESS_THAN_6_DB ==> 14",0,0,0);
      MSG_FATAL("TEST_SET_VOLT_BACKOFF_GENERIC ==> 15",0,0,0);
      MSG_FATAL("TEST_SET_HDET_OVERRIDE ==> 16",0 ,0, 0);

      pa_backoff_test_data[0] = TEST_NO_COMMAND;

      break;

  default:

      MSG_FATAL("Invalid test case %d !!!, use 1 to get test case list", 0, 0, 0 );

      pa_backoff_test_data[0] = TEST_NO_COMMAND;

      break;    
  }

}

#endif



#endif
