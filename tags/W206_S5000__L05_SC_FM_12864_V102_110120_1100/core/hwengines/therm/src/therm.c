/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  T H E R M I S T O R   U T I L I T I E S

GENERAL DESCRIPTION
  This module contains utilities to read and calibrate the thermistor.

EXTERNALIZED FUNCTIONS
  therm_read
    This function reads the current thermistor level (temperature).

  therm_calibrate
    Calibrate the thermistor reading, to correct for errors in
    the ADC's input thermistor voltage divider.

  therm_enable
    Allow external clients to explicitly control the PA thermistor power
    sequencing.  If a client needs to take a series of rapids readings
    it is preferable to enable the thermistor power once, take the readings,
    and then disable the power.  This API enables the PA thermistor power
    if it is not already enabled.  The API therm_read() will automatically
    enable power for a single reading if this API is not used.

  therm_disable
    Allow external clients to explicitly control the PA thermistor power
    sequencing.  If a client needs to take a series of rapids readings
    it is preferable to enable the thermistor power once, take the readings,
    and then disable the power.  This API disables the PA thermistor power
    if the thermistor is the last client of the voltage regulator.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Therm_calibrate() should be called to calibrate the thermistor readings.
  Until it is called, the default calibration is used.

Copyright (c) 1995, 1996, 1997 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1998, 1999, 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: O:/src/asw/COMMON/vcs/therm.c_v   1.0   29 May 2001 13:52:52   dbrackma  $
  $Header: //source/qcom/qct/core/hwengines/therm/main/latest/src/therm.c#1 $ $DateTime: 2008/08/12 16:54:33 $ $Author: jthielen $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/12/06   bmg     Added therm_enable() and therm_disable() APIs
                   Enable the thermistor VREG before reading the ADC
12/01/04   ty      Made changes to remove compile time warnings
10/02/03   jac     added therm_read_sensor
10/26/00   rmd     Merged with MSM3300 code.
10/09/00   dsb     Mainlined up to and including T_MSM31.
04/11/95   tst     Changed defines to be uPPer case.
04/07/95   tst     Changed defualts for min/max to 0/255.
03/01/95   tst     Created from vbatt.c.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"     /* Definitions for byte, word, etc.     */
#include "adc.h"        /* A/D converter specific definitions   */
#include "therm.h"      /* Therm general definitions            */
#include "msg.h"        /* Message logging/reporting services   */
#include "err.h"        /* Error logging/reporting services     */
#ifdef FEATURE_CDMA1X
#include "rfc.h"        /* RF driver card VREG control          */
#endif


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* Thermistor level calibration is done by taking the value read
** from the ADC and scaling it to a value THERM_SCALED_MIN..THERM_SCALE
** based on it's position between therm_min_temp and
** (therm_min_temp + therm_batt_range).
*/
#define THERM_MIN 0x00                     /* power up value for "hotest" */
#define THERM_MAX 0xff                     /* power up value for "coldest" */
#define THERM_RANGE (THERM_MAX-THERM_MIN)  /* power up value for "range" */

LOCAL word therm_min_temp = THERM_MIN;     /* "offset" for scaling therm */
LOCAL word therm_temp_range = THERM_RANGE; /* "range" for scaling therm */
LOCAL boolean therm_vreg_enabled = FALSE;


/*===========================================================================

FUNCTION THERM_READ

DESCRIPTION
  This function reads the current thermistor level.

DEPENDENCIES
  None

RETURN VALUE
  A scaled version of the thermistor level where "hotest" is
  THERM_SCALED_MIN, and "coldest" is THERM_SCALE.

SIDE EFFECTS
  None

===========================================================================*/
byte therm_read( void )
{
  byte in_raw;           /* raw value read from the A/D converter */
  word out_value;        /* scaled output value */

  if ( therm_is_enabled() )
  {
    /* Read Therm from the ADC driver
    */
    in_raw = adc_read( ADC_THERM );
  }
  else
  {
    therm_enable();
    /* Read Therm from the ADC driver
    */
    in_raw = adc_read( ADC_THERM );
    therm_disable();
  }

  /* If the input is less than the minimum value (highest temperature),
  ** return THERM_SCALED_MIN to avoid underflowing the calibration
  ** function below.
  */
  if (in_raw <= therm_min_temp) {
    return( THERM_SCALED_MIN );
  }

  /* Calibrate input to the range THERM_SCALED_MIN..THERM_SCALE
  */
  out_value = ((in_raw - therm_min_temp) * THERM_SCALE) / therm_temp_range;

  /* If we exceed the maximum value, return the maximum value.
  */
  if ( out_value >= THERM_SCALE ) {
    return( (byte) THERM_SCALE );
  }

  return( (byte) out_value );

} /* end of therm_read */


/*===========================================================================

FUNCTION THERM_CALIBRATE

DESCRIPTION
  This function calibrates the thermistor level reading returned by
  therm_read().  This routine need only be called once.  This calibration
  is to correct for the error in the resistors used in the voltage divider
  at the input to the ADC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Values returned by therm_read() may shift drastically from values
  returned in previous calls.

===========================================================================*/
void therm_calibrate
(
  byte min,
    /* Value returned by the ADC when the thermistor is at the minimum
    ** value (maximum temperature) at which the phone can operate.  Any
    ** safety margin added to this measurement should be done by the
    ** calibration, to provide the correction at the highest temperature.
    */
  byte max
    /* Value returned by the ADC when the thermistor is at the highest value
    ** (lowest temperature) at which the phone can operate.  Any margin added
    ** to this measurement should be done by the calibration, to provide the
    ** correction at the lowest temperature.
    */
)
{
  /* Check for illegal input
  */
  if ( max <= min ) {
    ERR("therm_calibrate, max %d <= min %d", max, min, 0 );
    return;
  }
  therm_min_temp = (word) min;                 /* Note the minimum level */
  therm_temp_range = (word)(max - min);        /* Compute the range      */

} /* end of therm_calibrate */

#ifdef FEATURE_TEMPERATURE_MONITOR
/*===========================================================================

FUNCTION THERM_READ_SENSOR

DESCRIPTION
  This function calls ADC to read a thermistor, prints out to the screen and
  logs it

DEPENDENCIES
  None

RETURN VALUE
  Returns the value of the thermistor in degC

SIDE EFFECTS
  None
  
===========================================================================*/
int32 therm_read_sensor
(
  adc_logical_channel_type therm_to_read
)
{

  int temp_read;

  if ( therm_to_read == ADC_PA_THERM_DEGC )
  {
    if ( therm_is_enabled() )
    {
      temp_read=adc_read(therm_to_read);
    }
    else
    {
      therm_enable();
      temp_read=adc_read(therm_to_read);
      therm_disable();
    }
  }
  else
  {
    temp_read=adc_read(therm_to_read);
  }

  switch (therm_to_read)
  {
    /*TODO add log packets*/
    case ADC_MSM_THERM_DEGC:
      MSG_HIGH("MSM therm=%dC",temp_read,0,0);
      break;
    case ADC_BATT_THERM_DEGC:
      MSG_HIGH("BATT therm=%dC",temp_read,0,0);
      break;
    case ADC_PMIC_THERM_DEGC:
      MSG_HIGH("PMIC therm=%dC",temp_read,0,0);
      break;
    case ADC_CHG_THERM_DEGC:
      MSG_HIGH("CHG therm=%dC",temp_read,0,0);
      break;
    case ADC_PA_THERM_DEGC:
      MSG_HIGH("PA therm=%dC",temp_read,0,0);
      break;
  #ifdef ADC_SDRAM_THERM_DEGC
    case ADC_SDRAM_THERM_DEGC:
      MSG_HIGH("SDRAM therm=%dC",temp_read,0,0);
      break;
  #endif
    default:
      MSG_ERROR("Therm not present",0,0,0);
      break;
  }
  
  return temp_read;
} /* end of therm_calibrate */
#endif


/*===========================================================================

FUNCTION THERM_ENABLE

DESCRIPTION
  This function will enable the thermister power.  This function may be
  called before a series of thermister reads to prevent rapid toggling
  of the thermister VREG.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The regulator (if any) that the thermister is connected to will be
  enabled (if not already enabled).  There may be additional circuits
  enabled by this call besides the thermistor.
  
===========================================================================*/
void therm_enable( void )
{
   #ifdef FEATURE_CDMA1X
  rfc_manage_vregs( RFC_PA_THERM_VREGS, PM_ON_CMD, RF_PATH_0 );
   #endif
  therm_vreg_enabled = TRUE;
}


/*===========================================================================

FUNCTION THERM_DISABLE

DESCRIPTION
  This function will de-assert the thermister VREG enable vote, allow the
  regulator (if any) powering the thermister to be shut down if it is
  the last client of the VREG.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The regulator (if any) that the thermister is connected to may be disabled.
  
===========================================================================*/
void therm_disable( void )
{
   #ifdef FEATURE_CDMA1X
  rfc_manage_vregs( RFC_PA_THERM_VREGS, PM_OFF_CMD, RF_PATH_0 );
   #endif
  therm_vreg_enabled = FALSE;
}


/*===========================================================================

FUNCTION THERM_IS_ENABLED

DESCRIPTION
  Check if the thermistor VREG has already been enabled.

DEPENDENCIES
  None

RETURN VALUE
  True if the PA thermistor has already been enabled by the therm_enable() API.
  False if the PA thermistor VREG has been released by therm_disable().
  This API may returne false even though the actual VREG the thermistor is
  attached to is in fact on.

SIDE EFFECTS
  None
  
===========================================================================*/
boolean therm_is_enabled( void )
{
  return therm_vreg_enabled;
}

