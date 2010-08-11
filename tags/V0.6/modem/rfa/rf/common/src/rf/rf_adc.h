#ifndef RF_ADC_H
#define RF_ADC_H
/*===========================================================================

                          R F   A D C   M O D U L E

DESCRIPTION
  This file exports the interface to the ADC embedded in Qualcomm RTR
  chipsets.

Copyright (c) 2007             by QUALCOMM, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/rf/common/main/qsc1100/src/rf/rf_adc.h#1 $
  $DateTime: 2008/04/11 05:11:57 $
  $Author: ppottier $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/26/07   bmg     Initial version
===========================================================================*/


/*===========================================================================

                              FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION RFLIB_READ_HDET                                    EXTERNAL FUNCTION

DESCRIPTION
  Read the HDET channel of the internal RTR ADC
  
DEPENDENCIES
  RF should be in RXTX mode for the HDET reading to be meaningful.

RETURN VALUE
  8-bit HDET reading.  The return type is declared int16 for possible
  future expansion of the HDET ADC resolution.

SIDE EFFECTS
  The internal HDET circuit of the RTR may be disabled at the time this
  function is called.  If it is, the RF driver will enable the HDET
  circuit, take the reading, and disable the circuit again.  This will
  briefly increase RTR current consumption.

===========================================================================*/
adc_error_return_type rflib_read_hdet(int *reading, void (*adc_completion_cb_fn)(void));


/*===========================================================================

FUNCTION RFLIB_READ_HDET_THERM                              EXTERNAL FUNCTION

DESCRIPTION
  Read the HDET thermistor channel of the internal RTR ADC
  
DEPENDENCIES
  None.

RETURN VALUE
  8-bit HDET thermistor reading.  The return type is declared int16 for
  possible future expansion of the HDET thermistor ADC resolution.

SIDE EFFECTS
  The internal HDET thermistor circuit of the RTR may be disabled at the
  time this function is called.  If it is, the RF driver will enable the
  HDET circuit, take the reading, and disable the circuit again.  This will
  briefly increase RTR current consumption.

===========================================================================*/
adc_error_return_type rflib_read_hdet_therm(int *reading, void (*adc_completion_cb_fn)(void));

#endif /* RF_ADC_H */
