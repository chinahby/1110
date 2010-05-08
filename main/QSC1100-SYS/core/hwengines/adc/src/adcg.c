/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    A N A L O G   T O   D I G I T A L   C O N V E R T E R   D R I V E R S

GENERAL DESCRIPTION
  This file just gives backwards compatibility to the ADC API.

EXTERNALIZED FUNCTIONS
  adc_read
    returns the byte value for the specified A/D input selection

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hwengines/adc/qsc1100/main/latest/src/adcg.c#1 $ $DateTime: 2008/08/14 10:53:43 $ $Author: jthielen $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03-02-14   tjw     Error messages added.
03-02-05   tjw     Initial release.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"

#ifndef FEATURE_ADC_USE_API_EXCLUSIVELY
/* for backwards compatibility: */

#include "adc.h"         /* ADC general definitions             */
#include "err.h"

/*===========================================================================

                          GLOBAL DATA DEFINITIONS

This section contains definitions for data/variable in this module.

===========================================================================*/
#ifdef USE_PMIC_ADC
ADC_RETURN_TYPE adc_prev_val[ ADC_INPUT_NUM ] =
{
  0xFF,        /* ADC_VBATT:     Battery Full */
  0x93,        /* ADC_THERM:     25 degrees C */
  0x00,        /* ADC_INPUT_2:                */
  0x00,        /* ADC_INPUT_3:                */
  0x00,        /* ADC_HDET_SHIFT:             */
  0x06,        /* ADC_PA_POWER_DETECT: 10 dBm */
  0x00,        /* ADC_INPUT_6:                */
  0x00,
  0x00
};
#else
ADC_RETURN_TYPE adc_prev_val[ ADC_INPUT_NUM ] =
{
  0xFF,        /* ADC_VBATT:     Battery Full */
  0x93,        /* ADC_THERM:     25 degrees C */
  0x00,        /* ADC_INPUT_2:                */
  0x00,        /* ADC_INPUT_3:                */
  0x00,        /* ADC_HDET_SHIFT:             */
  0x06,        /* ADC_PA_POWER_DETECT: 10 dBm */
  0x00         /* ADC_INPUT_6:                */
};
#endif


/*===========================================================================

FUNCTION ADC_READ

DESCRIPTION
  Selects the specified data source, initiates an A/D conversion and returns
  the result of the conversion.

DEPENDENCIES
  None

RETURN VALUE
  raw byte or word value of the ADC conversion

SIDE EFFECTS
  None

===========================================================================*/
ADC_RETURN_TYPE adc_read
(
  adc_logical_channel_type adc_addr    /* A/D input data source to select */
)
{
  adc_result_type result;
  adc_error_return_type errFlag;

  errFlag = adc_read_channel(adc_addr, &result);
  switch (errFlag)
  {
    case ADC_ERR_FLAG__SUCCESS:
      MSG_LOW("Chan %d ADC: Things seem OK", adc_addr, 0, 0);
      break;
    case ADC_ERR_FLAG__READ_ERR:
      MSG_HIGH("Chan %d ADC driver reported a problem", adc_addr, 0, 0);
      break;
    case ADC_ERR_FLAG__RANGE_ERR:
      MSG_HIGH("Chan %d ADC reading was out of range", adc_addr, 0, 0);
      break;
    case ADC_ERR_FLAG__NOT_READY:
      MSG_HIGH("Chan %d No ADC reading available", adc_addr, 0, 0);
      break;
    case ADC_ERR_FLAG__CHAN_NOT_PRESENT:
      MSG_HIGH("Chan %d not present on this target", adc_addr, 0, 0);
      break;
    case ADC_ERR_FLAG__ALREADY_STARTED:
      MSG_HIGH("Chan %d already started", adc_addr, 0, 0);
      break;
  }
  return (ADC_RETURN_TYPE)result.raw;
}

#endif /* FEATURE_ADC_USE_API_EXCLUSIVELY */

