/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                B A T T E R Y   L E V E L   U T I L I T I E S

GENERAL DESCRIPTION
  This module contains utilities to read and calibrate the battery level.

EXTERNALIZED FUNCTIONS
  vbatt_read
    This function reads the current battery level.

  vbatt_calibrate
    Calibrate the battery level reading, to correct for errors in
    the ADC's input battery voltage divider.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  Vbatt_calibrate() should be called to calibrate the battery level readings.
  Until it is called, the default calibration is used.

Copyright (c) 1992-2002  by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/vbatt/main/latest/app/inc/vbatt.c#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/19/09   APU     Added scaling to adc_read() return value for QSC1100. 
03/11/09   APU     Added APIs to support the new VBATT info structure and 
                   also support the CHG callback functionality.
12/25/08   APU     Introduced the API: vbatt_read_mv().  
03/25/08   jtn     Accounted for ADC pre-scaler difference on new PMICs
01/16/05   jac     removed rf.h
06/05/04   jac     added FEATURE_VBATT_3P2_4P2_DEFAULT_SCALE
04/22/04   jac     removed unused macros, added null ptr check
01/19/03   jac     added FEATURE_VBATT_IGNORE_RF_MODE, modified
                   vbatt_get_adc_count_range_cal_info to check for invalid cal
11/17/03   Vish    Changed using macro ERR to MSG_ERROR in
                   vbatt_get_adc_count_range_cal_info().
11/14/03   Vish    Added vbatt_get_adc_count_range_cal_info() for retrieving
                   ADC count range calibration info needed by other drivers
                   like ADC.
11/03/03   rmd     Replaced MSG_MED("old_vbatt=%d".. with
                   MSG_LOW("old_vbatt=%d".. as requested by AST.
08/14/03   jac     Featurized out db.h
08/16/02   rmd     Added the PMIC low battery alarm.
08/14/02   rmd     Pass vmax and vmin from NV to the charger driver.
07/11/02   aks     Cast return type of value, when gauging is disabled.
06/06/01   aks     Implemented FEATURE_ENABLE_CHARGING_GAUGING.  Gauge returns
                   full battery value when gauging is disabled.
05/29/01   dsb     Removed support for VBATT_PHONE_HAS_INT_CHARGER and
                   VBATT_PHONE_HAS_MULTI_BATT_ID.  Removed vbatt_batt_id.
10/09/00   dsb     Mainlined up to and including T_MSM31.  Removed TG== code.
11/22/99   jc      Use logical BIO registers
11/24/98   lz      For Q targets. Added a simple TxAGC support. When in a
                   CDMA Tx mode, the TxAGC level is measured and the current
                   discharge curved used is adjusted according to the TxAGC
                   level.
10/29/98   dlb     Added support for Odie+Panther.  Initially copy from Odie.
10/21/98   lz      Added Extended Toshiba battery pack support for the Q
                   target.
10/05/98   lz      For Q targets.  Changed the base curve for the standard
                   Toshiba and extended Sony batteries and added compensation
                   for FM calls.
09/26/98   aks     Added underflow / overflow checks of calibration values.
                   QCP-860 low cal. point at 3.20V, QCP-1960 low cal. at 3.40V
09/11/98   aks     Added gauging for a dual-battery (5GP) system.
06/22/98   aks     For Q targets.  Raised the CDMA and FM shutdown point by
                   0.4 volts.  Raised Idle shutdown point by 0.2 volts.  Moved
                   due to hardware concern that battery is being discharged
                   too much.
03/09/98   aks     Added Extended battery pack support for the Q target.
                   Assumes that the Q-800 has a Battery ID port.  Adjusted
                   the curve for the Standard pack, balanced scale by moving
                   more time into the upper regions.
01/15/98   aks     Lowered Q shutdown voltage by another 0.500 volts.
12/17/97   aks     Q changes.  Lowered shutdown voltage points to help talk time
                   of max Tx power and full rate condition.  Account for lack of
                   battery ID port in Q-800.  Prevent overcompensation in gauge
                   during a Call.
12/08/97   aks     Added multiple battery support for Q.  Internal battery id
                   function included when the Charger module does not exist.
11/17/97   aks     Q changes.  Corrected compensation for FM call.  Adjusted
                   curve, put more time in 4th bar region.  Use dg_get() and
                   direct port reads instead of the visible ui structure.
11/04/97   aks     Non-TGP and non-Q targets will use previous default
                   calibration values.
11/03/97   aks     Implemented default calibration values for Q target.
10/30/97   aks     Q changes.  Adjusted curve slopes.  Shift gauging from UI to
                   Vbatt module, account for change in external charger status.
                   Initial support for Q-800 FM call compensation.
10/06/97   aks     Added support for Q-1900 target.  Removed idle power comp.
                   from in-call compensation.  Adjusted NiMH in-call comp. value
                   to give more low warning time.
09/17/97   aks     Improved overall precision and temperature compensation.  Power
                   drop and non-linear thermistor compensation added.
08/15/97   aks     Adjusted data table for the Unknown battery type.  Made new
                   battery gauge function defined only for TGP targets.
08/13/97   aks     Added vbatt_read_gauge() to reduce the non-linear
                   characteristics of battery discharge performance.  Added
                   various supporting functions.  Power load compensation is
                   for idle, CDMA Tx and FM Tx modes only.
05/15/95   jah     Changed defaults for min/max to 0/255 for non-CD7000.
                   Added a sleep check to void doing ADC conversions during
                   sleep, hold previous value.
05/26/95   tst     Updated default values for VBATT min/max to 128/213.
05/04/95   tst     Updated default values for VBATT min/max.
04/11/95   tst     Changed defines to be uPPer case.
                   Preserved portable default values.
04/07/95   tst     Changed defualts for min/max to 0/255.
03/01/95   tst     Changed to use VBATT_SCALED_MIN instead of 0.
02/14/95   tst     Modified vbatt_read() to use new adc driver ifdef T_MSM2P.
12/29/93   jah     Set the initial min and range values to the default for
                   using 1% resistors.
05/04/93   jah     Changed ADC code to use macros from adc.h.
02/04/93   jah     Changed vbatt_calibrate() to never ERR_FATAL(), and to
                   take byte inputs, instead of word inputs.
12/22/92   jah     Added vbatt_calibrate()
12/06/92   jah     Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"
#include "comdef.h"     /* Definitions for byte, word, etc.     */
#include "vbatt.h"      /* VBatt general definitions            */
#include "msg.h"        /* Message logging/reporting services   */
#include "err.h"        /* Error logging/reporting services     */

#if defined(FEATURE_ENABLE_CHARGING_GAUGING) && !defined(FEATURE_VBATT_IGNORE_RF_MODE)
#include "db.h"         /* Database service for DB_RF_MODE      */
#endif

#if defined(FEATURE_BATTERY_CHARGER)
#include  "charger.h"
#endif

#if defined(FEATURE_PMIC)
#include "pm.h"
#endif


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* Battery level calibration is done by taking the value read from
** the ADC and scaling it to a value VBATT_SCALED_MIN..VBATT_SCALE
** based on it's position between vbatt_min_batt and
** (vbatt_min_batt + vbatt_batt_range).
*/

#define VBATT_MIN 0                       /* Battery ADC value for "empty" */
#define VBATT_MAX 255                     /* Battery ADC value for "full"  */

#define VBATT_RANGE (VBATT_MAX-VBATT_MIN)  /* power up value for "range" */

/*-------------------------------------------------------------------------*
 *                                                                         *
 *                             LOCAL DATA                                  *
 *                                                                         *
 *-------------------------------------------------------------------------*/
#ifdef FEATURE_VBATT_3P2_4P2_DEFAULT_SCALE
/* Default values to use if vbatt not calibrated */
LOCAL word vbatt_min_batt = 157;        /* "offset" for scaling vbatt */
LOCAL word vbatt_batt_range = 49;       /* "range" for scaling vbatt */

#else

LOCAL word vbatt_min_batt = VBATT_MIN;        /* "offset" for scaling vbatt */
LOCAL word vbatt_batt_range = VBATT_RANGE;    /* "range" for scaling vbatt */

#endif


static battery_adc_params_type batt_adc_params ;


/*==========================================================================*
 *                      EXPORTED FUNCTIONS                                  *
 *==========================================================================*/

/*===========================================================================

FUNCTION VBATT_READ                                  EXTERNALIZED FUNCTION

DESCRIPTION
  This function reads the current battery level.

DEPENDENCIES
  None

RETURN VALUE
  A scaled version of the battery level where "empty" is VBATT_SCALED_MIN,
  and "full" is VBATT_SCALE.

SIDE EFFECTS
  None

===========================================================================*/
byte vbatt_read( void )
{
  #ifdef FEATURE_ENABLE_CHARGING_GAUGING
  byte in_raw;           /* raw value read from the A/D converter          */
  word out_value;        /* scaled output value                            */
  static byte in_previous = 0xff;
                         /* previous raw value from the A/D converter      */
  #endif /* FEATURE_ENABLE_CHARGING_GAUGING */
  

  #if defined(FEATURE_ENABLE_CHARGING_GAUGING) && !defined(FEATURE_VBATT_IGNORE_RF_MODE)
  db_items_value_type db_item;
                         /* DB item data buffer for db reads               */
  

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Read RF Mode from DB to determine whether the RF is asleep.
  ** if it is asleep, the ADC is not operational, so use in_previous
  ** instead of reading the A/D converter.
  */
  db_get( DB_RF_MODE, &db_item );

  if( db_item.rf_mode == DB_RF_SLEEP )
  {
    in_raw = in_previous;               /* Use the previous value         */
  }
  else
  #endif /* FEATURE_ENABLE_CHARGING_GAUGING */


  #ifdef FEATURE_ENABLE_CHARGING_GAUGING
  {
    in_raw = adc_read( ADC_VBATT );     /* Read Vbatt from the ADC driver */
    #if (defined (T_QSC6270) || defined(T_QSC1100))
    in_raw = (in_raw * 3)/2;  //adjust for Q6270 prescaler
    #endif
    in_previous = in_raw;               /* Update "previous"              */
  }

  /* If the input is less than the minimum, return VBATT_SCALED_MIN
  ** to avoid underflowing the calibration function below.
  */
  if (in_raw <= vbatt_min_batt)
  {
    MSG_MED("old_vbatt=%d", VBATT_SCALED_MIN, 0, 0);
    return( VBATT_SCALED_MIN );
  }

  /* Calibrate input to the range VBATT_SCALED_MIN..VBATT_SCALE
  */
  out_value = ((in_raw - vbatt_min_batt) * VBATT_SCALE) / vbatt_batt_range;

  /* If we exceed the maximum value, return the maximum value.
  */
  if ( out_value >= VBATT_SCALE ) {
    MSG_LOW("old_vbatt=%d", VBATT_SCALE, 0, 0);
    return( (byte) VBATT_SCALE );
  }

  MSG_LOW("old_vbatt=%d", out_value, 0, 0);
  return( (byte) out_value );

  #else

  return( (byte) VBATT_MAX );

  #endif /* FEATURE_ENABLE_CHARGING_GAUGING */

} /* end of vbatt_read */


/*===========================================================================

FUNCTION VBATT_READ_MV                                EXTERNALIZED FUNCTION

DESCRIPTION
  This function reads the current battery level.

DEPENDENCIES
  None

RETURN VALUE
  unsigned int representing the battery voltage in milli-volts.

EXAMPLE:
  uint16 batt_voltage = vbatt_read_mv() ;
  printf ( "The battery voltage is %d milli volts" , batt_voltage ) ;
  OP ===> 3560 milli volts

SIDE EFFECTS
  None

===========================================================================*/
uint16 
vbatt_read_mv ( void )
{
  uint32 temp_voltage_mv = 0 ; 

/* 
 * The feature FEATURE_ENABLE_CHARGING_GAUGING is used because some 
 * targets (such as PC cards) do not have/need the voltage gauging. but 
 * the vbatt_read_mv will continue to compile for all targets. 
 */  
#ifdef FEATURE_ENABLE_CHARGING_GAUGING
  temp_voltage_mv = adc_read ( ADC_VBATT_MV ) ;
#endif /* FEATURE_ENABLE_CHARGING_GAUGING */
  
  return temp_voltage_mv ;
}


/*===========================================================================

FUNCTION VBATT_CALIBRATE                          EXTERNALIZED FUNCTION

DESCRIPTION
  This function calibrates the battery level reading returned by
  vbatt_read().  This routine need only be called once.  This calibration
  is to correct for the error in the resistors used in the voltage divider
  at the input to the ADC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Values returned by vbatt_read() may shift drastically from values
  returned in previous calls.

===========================================================================*/

void vbatt_calibrate
(
  byte min,
    /* Value returned by the ADC when the battery is at the minimum level
    ** at which the phone can operate (battery "empty").  Any safety margin
    ** added to this measurement should be done by the calibration, to provide
    ** the correction at the lowest level.
    */
  byte max
    /* Value returned by the ADC when the battery is at the fully charged
    ** level (battery "full").  Any "show full when it's at max charge" margin
    ** added to this measurement should be done by the calibration, to provide
    ** the correction at the lowest level.
    */
)
{
  /* Check for illegal input
  */
  if ( max <= min ) {
    ERR("vbatt_calibrate, max %d <= min %d", max, min, 0 );
    return;
  }
  vbatt_min_batt = (word) min;                 /* Note the minimum level */
  vbatt_batt_range = (word)(max - min);        /* Compute the range      */

  #if defined(FEATURE_BATTERY_CHARGER)
  /* Send the battery calibration to the charger driver */
  chg_get_batt_cal(min, max);
  #endif /* FEATURE_BATTERY_CHARGER */


  #if defined(FEATURE_LOW_BATT_DETECTION)
  /* Initialize the low power alarm from the PMIC */
  pm_low_batt_alarm_init();
  #endif


} /* end of vbatt_calibrate */

#if defined(FEATURE_LOW_BATT_DETECTION)
/*===========================================================================

FUNCTION vbatt_is_low_batt_alarm_triggered            EXTERNALIZED FUNCTION

DESCRIPTION
    Use this function to find out if the low battery alarm has triggered.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean vbatt_is_low_batt_alarm_triggered(void)
{
    return pm_is_low_batt_alarm_triggered();
}
#endif  /* FEATURE_LOW_BATT_DETECTION */


/*===========================================================================
FUNCTION   vbatt_get_adc_count_range_cal_info              EXTERNAL FUNCTION

DESCRIPTION
   Use this function to get info on the calibrated ADC counts for the min
   and max valid main battery voltage.

DEPENDENCIES
   None

RETURN VALUE
   boolean
      TRUE  ---> If valid calibration data is available
      FALSE ---> Otherwise

SIDE EFFECTS
   None
===========================================================================*/
boolean vbatt_get_adc_count_range_cal_info(
   uint8  *min_count_ptr,
   uint8  *max_count_ptr)
{
  if (min_count_ptr==NULL || max_count_ptr==NULL)
  {
    MSG_ERROR("min/max null ptr",0,0,0);
    return FALSE;
  }

  if ( (vbatt_min_batt > 0)                            
        && ((vbatt_min_batt+vbatt_batt_range) < 255) )  
   {
      *min_count_ptr = (uint8) vbatt_min_batt;
      *max_count_ptr = (uint8) (vbatt_min_batt+vbatt_batt_range);

      return TRUE;
   }
   else
   {
      /**********************************************************/
      /* VBATT cal out of range, NV may not have been cal,      */
      /* vbatt_calibrate may not have been called, or pmic error*/
      /* large enough to exceed adc range                       */
      /**********************************************************/
      MSG_ERROR(
         "VBATT cal data out of range: [MIN=%d] [RANGE=%d]",
         vbatt_min_batt,
         vbatt_batt_range,
         0);
      return FALSE;
      
   }
}



/*===========================================================================
FUNCTION   vbatt_update_adc_battery_params              EXTERNAL FUNCTION

DESCRIPTION
   Use this function to update various battery parameters.

DEPENDENCIES
   Underlying ADC channel should be operative.

RETURN VALUE
   VOID

SIDE EFFECTS
   NONE
===========================================================================*/
void
vbatt_update_adc_battery_params ( void ) 
{
    
    uint16 temp_voltage = batt_adc_params.current_batt_adc_voltage ;

    batt_adc_params.current_batt_adc_voltage = adc_read ( ADC_VBATT_MV ) ;

    /* New voltage obtained */ 
    if ( temp_voltage != batt_adc_params.current_batt_adc_voltage )
    {
        batt_adc_params.previous_batt_adc_voltage = temp_voltage ;
    }
    /* else Maintain the status Quo*/

    batt_adc_params.batt_temperature = adc_read ( ADC_BATT_THERM_DEGC ) ;

    batt_adc_params.batt_id = adc_read ( ADC_BATT_ID_MV ) ;

}


/*===========================================================================
FUNCTION   vbatt_query_batt_adc_params              EXTERNAL FUNCTION

DESCRIPTION
   Use this function to get info about various battery parameters.

DEPENDENCIES
   Underlying ADC channel should be operative.

RETURN VALUE
   VOID

SIDE EFFECTS
   NONE
===========================================================================*/
void
vbatt_query_batt_adc_params ( battery_adc_params_type *batt_ptr ) 
{
    /* Do a member to member copy */ 
    batt_ptr->current_batt_adc_voltage =
                         batt_adc_params.current_batt_adc_voltage ;

    batt_ptr->previous_batt_adc_voltage =
                         batt_adc_params.previous_batt_adc_voltage ;

    batt_ptr->batt_temperature =
                         batt_adc_params.batt_temperature ;

    batt_ptr->batt_id =
                         batt_adc_params.batt_id ;
}


