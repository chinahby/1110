#ifndef ADC_CAL_H
#define ADC_CAL_H
/*===========================================================================

     A D C   C A L I B R A T I O N   M O D U L E   H E A D E R   F I L E

DESCRIPTION
  This module has the logic to perform ADC calibration on phone targets that
  employ PMIC analog-mux and MSM's ADC for analog-to-digital conversions.
  The calibration procedure is customized by different targets by selecting
  from the list of supported ADC calibration configurations.

Copyright (c) 2004 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/hwengines/adc/qsc1100/main/latest/inc/adc_cal.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07-01-04   Vish    Replaced the inclusion of "adie_adc.h" with "adci.h" that
                   resolves the issue of whether "adie_adc.h" exists.
06-28-04   Vish    Added a new calibration configuration for PM AMUX's bypass
                   mode (i.e. PM_ADC_MUXSCALE15).
05-12-04   Vish    Created.
===========================================================================*/


/* =========================================================================
                         INCLUDE FILES
========================================================================= */
#include "comdef.h"
#include "adie.h"
#include "adci.h"


#ifdef FEATURE_ADC_CALIBRATION
/* =========================================================================
                       MACROS / CONSTANTS
========================================================================= */


/* =========================================================================
                         TYPE DEFINITIONS
========================================================================= */
// Types of configurations calibrated by this module
typedef enum
{                                                      // External measurement required at 
   ADC_CAL_CONFIG__PM_MUXSCALE0   = (uint32)(1 << 0),
   ADC_CAL_CONFIG__PM_MUXSCALE1   = (uint32)(1 << 1),  // 1600 mV
   ADC_CAL_CONFIG__PM_MUXSCALE2   = (uint32)(1 << 2),
   ADC_CAL_CONFIG__PM_MUXSCALE3   = (uint32)(1 << 3),
   ADC_CAL_CONFIG__PM_MUXSCALE4   = (uint32)(1 << 4),
   ADC_CAL_CONFIG__PM_MUXSCALE5   = (uint32)(1 << 5),
   ADC_CAL_CONFIG__PM_MUXSCALE6   = (uint32)(1 << 6),  // 1600 mV
   ADC_CAL_CONFIG__PM_MUXSCALE7   = (uint32)(1 << 7),
   ADC_CAL_CONFIG__PM_MUXSCALE8   = (uint32)(1 << 8),
   ADC_CAL_CONFIG__PM_MUXSCALE9   = (uint32)(1 << 9),  // 1600 mV
   ADC_CAL_CONFIG__PM_MUXSCALE10  = (uint32)(1 << 10), // 1600 mV
   ADC_CAL_CONFIG__PM_MUXSCALE11  = (uint32)(1 << 11),
   ADC_CAL_CONFIG__PM_MUXSCALE12  = (uint32)(1 << 12), // 1600 mV and 2100 mV
   ADC_CAL_CONFIG__PM_MUXSCALE13  = (uint32)(1 << 13), // 1600 mV
   ADC_CAL_CONFIG__PM_MUXSCALE14  = (uint32)(1 << 14), // 2100 mV
   ADC_CAL_CONFIG__PM_MUXSCALE15  = (uint32)(1 << 15),
   
   // Remember to adjust this everytime a configuration is added or removed!!!
   ADC_CAL_CONFIG__MAX_VALID_CONFIG    = ADC_CAL_CONFIG__PM_MUXSCALE15,
   ADC_CAL_CONFIG__NUM_CONFIGS = 16
} adc_cal_config_type;

// Data type for returning calibration results back to the client
typedef struct
{
   int32   voltage_mV_offset_x_10000;
   int32   voltage_mV_vs_adc_count_slope_x_10000;
} adc_cal_info_type;

// Type of callback fn used by this module to notify its client that the
// calibration has been completed and that the results are ready.
typedef void (*adc_cal_done_cb_func_type) (void);


/* =========================================================================
                         FUNCTION PROTOTYPES
========================================================================= */

/*===========================================================================
FUNCTION  adc_cal_use_external_measurement                 EXTERNAL FUNCTION

DESCRIPTION
   This function is used for providing external calibration data for those 
   ADC calibration configurations that require it. Refer to adc_cal_config_type
   enum above to check if an external calibration is required and if so, the
   voltage that needs to be applied at the MSM's ADC input while an A->D
   conversion is performed to generate a valid external measurement. This
   function is then called with information on the voltage used and
   the corresponding digitized value read from the MSM's ADC. One common
   way for generating the required input voltage at the MSM's ADC is by using
   a known main battery voltage and then selecting this voltage channel to
   appear at the input of the MSM's ADC before performing the conversion.
   
PARAMETERS
   1) Name: cal_config
      - ADC calibration configuration that requires this external measurement
        for generating calibration result. See 'adc_cal_info_type'.
      Type: adc_cal_config_type
      - Valid inputs:
           Those ADC calibration configurations that require one or more
           external measurements. See 'adc_cal_config_type' for more info.
   
   2) Name: voltage_mV
      - Voltage (in milli-volts) applied at the input of MSM's ADC 
      Type: uint32
      - Valid inputs:
           See 'adc_cal_config_type' for more info
           
   3) Name: adc_count
      - Digitized input voltage as read from the MSM's ADC register.
      Type: uint8
      - Valid inputs:
           0 to 255
           
RETURN VALUE
   None.

DEPENDENCIES
   None.
   
SIDE EFFECTS
   None.
===========================================================================*/
extern void adc_cal_use_external_measurement(
   adc_cal_config_type     cal_config,
   uint32                  voltage_mV,
   uint8                   adc_count
);


/*===========================================================================
FUNCTION  adc_cal_calibrate                                EXTERNAL FUNCTION

DESCRIPTION
   This function starts the calibration procedure for the selected ADC
   calibration configurations. The client is then notified through a callback
   when the results are ready.

PARAMETERS
   1) Name: list_of_cal_config
      - List of ADC calibration configurations that the client is interested in.
        Use bit-wise OR (|) operator for listing the configurations.
      Type: uint32
      - Valid inputs:
           Bit-wise OR (|) of the configurations available in 'adc_cal_config_type'
   
   2) Name: pm_amuxout_adie_muxsel
      - Input channel of MSM's ADC that the PM's analog mux (AMUX) is connected to.
      Type: adie_hkadc_muxsel_type
      - Valid inputs:
           Refer to your phone schematics and adie_hkadc_muxsel_type.
           
   3) Name: cal_done_cb_func
      - Callback fn for notifying that calibration has been completed and that
        the results are ready.
      Type: adc_cal_done_cb_func_type
      - Valid inputs:
           Valid non-NULL callback fn
           
RETURN VALUE
   None.

DEPENDENCIES
   None.
   
SIDE EFFECTS
   Interrupts are locked in this function.
===========================================================================*/
extern void adc_cal_calibrate(
   uint32                       list_of_cal_config,
   adie_hkadc_muxsel_type       pm_amuxout_adie_muxsel,
   adc_cal_done_cb_func_type    cal_done_cb_func
);


/*===========================================================================
FUNCTION  adc_cal_get_cal_info                             EXTERNAL FUNCTION

DESCRIPTION
   This function retrieves the results for a specific ADC cal configuration. 

PARAMETERS
   1) Name: cal_config
      - ADC calibration configuration, the results of which the client is
        interested in.
      Type: adc_cal_config_type
      - Valid inputs:
           One of the configurations calibrated by adc_cal_calibrate()
   
   2) Name: cal_info_ptr
      - Pointer to the structure used for returning the results, if available.
      Type: Pointer to adc_cal_info_type
      - Valid inputs:
           Valid non-NULL pointer           
           
RETURN VALUE
   Type: boolean
      - Possible values:
        - TRUE     --->  If successful
        - FALSE    --->  Otherwise

DEPENDENCIES
   adc_cal_calibrate() must have been called before for this function to 
   return successful results.
   
SIDE EFFECTS
   None.
===========================================================================*/
extern boolean adc_cal_get_cal_info(
   adc_cal_config_type   cal_config,
   adc_cal_info_type    *cal_info_ptr
);

#endif // FEATURE_ADC_CALIBRATION

#endif // ADC_CAL_H

