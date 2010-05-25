#ifndef THERM_H
#define THERM_H
/*===========================================================================

       T H E R M I S T O R   U T I L I T I E S   H E A D E R   F I L E

DESCRIPTION
  This header file contains all of the definitions necessary for other
  tasks to interface with the thermistor utilities.

REFERENCES
  None

Copyright (c) 1995, 1996, 1997 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1998, 1999, 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
  $PVCSPath: O:/src/asw/COMMON/vcs/therm.h_v   1.0   29 May 2001 13:53:02   dbrackma  $
  $Header: //source/qcom/qct/core/hwengines/therm/main/latest/inc/therm.h#1 $ $DateTime: 2008/08/12 16:54:33 $ $Author: jthielen $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/12/06   bmg     Added therm_enable() and therm_disable() APIs
10/02/03   jac     added therm_read_sensor
02/14/95   tst     Created from vbatt.h.

===========================================================================*/

#include "comdef.h"     /* Definitions for byte, word, etc.     */

#define FEATURE_TEMPERATURE_MONITOR

#ifdef FEATURE_TEMPERATURE_MONITOR
#include "adc.h"
#endif


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* The range of Therm readings [THERM_SCALED_MIN,THERM_SCALE] returned
** by therm_read
*/
#define THERM_SCALED_MIN   0
#define THERM_SCALE      255

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION THERM_READ

DESCRIPTION
  This function reads the current thermistor level (temperature).

DEPENDENCIES
  None

RETURN VALUE
  A scaled version of the thermistor level where the highest temperature is
  THERM_SCALED_MIN, and the lowest temperature is THERM_SCALE.

SIDE EFFECTS
  None

===========================================================================*/
extern byte therm_read( void );

/*===========================================================================

FUNCTION THERM_CALIBRATE

DESCRIPTION
  This function calibrates the thermistor level reading returned by
  therm_read().  This routine need only be called once.  This calibration
  is to correct for the error in the resisters used in the voltage divider
  at the input to the ADC and for tolerance of the thermistor.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Values returned by therm_read() may shift drastically from values
  returned in previous calls.

===========================================================================*/
extern void therm_calibrate
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
);

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
);

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
void therm_enable( void );


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
void therm_disable( void );


/*===========================================================================

FUNCTION THERM_IS_ENABLED

DESCRIPTION
  Check if the thermistor VREG has already been enabled.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The regulator (if any) that the thermister is connected to may be disabled.
  
===========================================================================*/
boolean therm_is_enabled( void );

#endif /* THERM_H */
