
/*===========================================================================

                A D C   C A L I B R A T I O N   M O D U L E
                   I M P L E M E N T A T I O N   F I L E  

DESCRIPTION
  This module has the logic to perform ADC calibration on phone targets that
  employ PMIC analog-mux and MSM's ADC for analog-to-digital conversions.
  The calibration procedure is customized by different targets by selecting
  from the list of supported ADC calibration configurations.

Copyright (c) 2004, 2005 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/hwengines/adc/qsc1100/main/latest/src/adc_cal.c#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03-06-06   hkk     Bypassing the divide by zero error
05-03-05   ty      Made changes to support multiple ADC resolutions
06-28-04   Vish    Added a new calibration configuration for PM AMUX's bypass
                   mode (i.e. PM_ADC_MUXSCALE15). Also made adc_cal_calibrate()
                   to be callable more than once.
06-25-04   Vish    Added a check in adc_cal_calibrate() to see if internal
                   AMUX reference channels are actually available on the 
                   current PM chip in use before attempting to use those
                   channels for calibration purpose.
05-12-04   Vish    Created.
===========================================================================*/


/* =========================================================================
                         INCLUDE FILES
========================================================================= */
#include "comdef.h"
#include "adie.h"
#include "adci.h"
#include "adc_cal.h"
#include "pm.h"


#ifdef FEATURE_ADC_CALIBRATION
/*===========================================================================
              LOCAL MACRO AND CONSTANT DEFINITIONS
===========================================================================*/
#ifdef FEATURE_ADC_12BIT_CALIBRATION
#define ADC_CAL_ADIE_MAX_READING      4095 //12 bit max value
#else
#define ADC_CAL_ADIE_MAX_READING      255
#endif /* FEATURE_ADC_12BIT_CALIBRATION */

#define ADC_CAL_ADIE_VREF_MV         2200

// Max number of points ever needed for any calibration
#define ADC_CAL_MAX_PTS_PER_CAL                    2

#define  ADC_CAL_PM_INTERNAL_REF_VOLTAGE1_MV       625
#define  ADC_CAL_PM_INTERNAL_REF_VOLTAGE1_MUXSEL   PM_ADC_MUXSEL12
#define  ADC_CAL_PM_INTERNAL_REF_VOLTAGE2_MV       1250
#define  ADC_CAL_PM_INTERNAL_REF_VOLTAGE2_MUXSEL   PM_ADC_MUXSEL13

#define ADC_CAL_GET_PM_MUXSEL_FOR_INTERNAL(mV)  ( (ADC_CAL_PM_INTERNAL_REF_VOLTAGE1_MV == mV)       \
                                                   ? ADC_CAL_PM_INTERNAL_REF_VOLTAGE1_MUXSEL        \
                                                   : ( (ADC_CAL_PM_INTERNAL_REF_VOLTAGE2_MV == mV)  \
                                                        ? ADC_CAL_PM_INTERNAL_REF_VOLTAGE2_MUXSEL   \
                                                        : PM_ADC_MUXSEL_NONE ) )


/*===========================================================================
                     LOCAL TYPE DEFINITIONS
===========================================================================*/
// Calibration methodology
typedef enum
{
   ADC_CAL_METHOD__1PT_INTERPOLATION,
   ADC_CAL_METHOD__2PT_INTERPOLATION,
   ADC_CAL_METHOD__NONE
} adc_cal_method_type;

// Status enum to keep track of the processing of calibration input data
typedef enum
{
   ADC_CAL_STATUS__MEASUREMENT_NEEDED,
   ADC_CAL_STATUS__MEASUREMENT_AVAILABLE,
   ADC_CAL_STATUS__MEASUREMENT_FAILED,
   ADC_CAL_STATUS__CALIBRATION_DONE,
   ADC_CAL_STATUS__NONE
} adc_cal_status_type;

// Type definition for calibration data points
typedef struct
{
   boolean   adc_count_available;
   uint32    voltage_mV;
   int       adc_count;
} adc_cal_point_type;

// Type definition for the input of ADC configuration to be calibrated
typedef struct
{
   adc_cal_method_type                 method;
   adc_cal_point_type                  pts_array[ADC_CAL_MAX_PTS_PER_CAL];
   adc_cal_status_type                 status;
} adc_cal_input_type;

// Type definition for storing the calibration results of a specific ADC configuration
typedef struct
{
   boolean              info_available;
   adc_cal_info_type    info;
} adc_cal_output_type;


/*===========================================================================
                   LOCAL VARIABLE DEFINITIONS
===========================================================================*/
LOCAL adc_cal_input_type   adc_cal_input[ADC_CAL_CONFIG__NUM_CONFIGS]
   = {
        { ADC_CAL_METHOD__2PT_INTERPOLATION, {{FALSE,        625,(uint8)(~0)}, {FALSE,       1250,(uint8)(~0)}}, ADC_CAL_STATUS__NONE },  // PM_MUXSCALE0
        { ADC_CAL_METHOD__2PT_INTERPOLATION, {{FALSE,       1250,(uint8)(~0)}, {FALSE,       1600,(uint8)(~0)}}, ADC_CAL_STATUS__NONE },  // PM_MUXSCALE1
        { ADC_CAL_METHOD__2PT_INTERPOLATION, {{FALSE,        625,(uint8)(~0)}, {FALSE,       1250,(uint8)(~0)}}, ADC_CAL_STATUS__NONE },  // PM_MUXSCALE2
        { ADC_CAL_METHOD__1PT_INTERPOLATION, {{FALSE,        625,(uint8)(~0)}, {FALSE,(uint8)(~0),(uint8)(~0)}}, ADC_CAL_STATUS__NONE },  // PM_MUXSCALE3
        { ADC_CAL_METHOD__NONE,              {{FALSE,(uint8)(~0),(uint8)(~0)}, {FALSE,(uint8)(~0),(uint8)(~0)}}, ADC_CAL_STATUS__NONE },  // PM_MUXSCALE4
        { ADC_CAL_METHOD__2PT_INTERPOLATION, {{FALSE,        625,(uint8)(~0)}, {FALSE,       1250,(uint8)(~0)}}, ADC_CAL_STATUS__NONE },  // PM_MUXSCALE5
        { ADC_CAL_METHOD__2PT_INTERPOLATION, {{FALSE,       1250,(uint8)(~0)}, {FALSE,       1600,(uint8)(~0)}}, ADC_CAL_STATUS__NONE },  // PM_MUXSCALE6
        { ADC_CAL_METHOD__2PT_INTERPOLATION, {{FALSE,        625,(uint8)(~0)}, {FALSE,       1250,(uint8)(~0)}}, ADC_CAL_STATUS__NONE },  // PM_MUXSCALE7
        { ADC_CAL_METHOD__1PT_INTERPOLATION, {{FALSE,        625,(uint8)(~0)}, {FALSE,(uint8)(~0),(uint8)(~0)}}, ADC_CAL_STATUS__NONE },  // PM_MUXSCALE8
        { ADC_CAL_METHOD__2PT_INTERPOLATION, {{FALSE,       1250,(uint8)(~0)}, {FALSE,       1600,(uint8)(~0)}}, ADC_CAL_STATUS__NONE },  // PM_MUXSCALE9
        { ADC_CAL_METHOD__2PT_INTERPOLATION, {{FALSE,       1250,(uint8)(~0)}, {FALSE,       1600,(uint8)(~0)}}, ADC_CAL_STATUS__NONE },  // PM_MUXSCALE10
        { ADC_CAL_METHOD__1PT_INTERPOLATION, {{FALSE,       1250,(uint8)(~0)}, {FALSE,(uint8)(~0),(uint8)(~0)}}, ADC_CAL_STATUS__NONE },  // PM_MUXSCALE11
        { ADC_CAL_METHOD__2PT_INTERPOLATION, {{FALSE,       1600,(uint8)(~0)}, {FALSE,       2100,(uint8)(~0)}}, ADC_CAL_STATUS__NONE },  // PM_MUXSCALE12
        { ADC_CAL_METHOD__1PT_INTERPOLATION, {{FALSE,       1600,(uint8)(~0)}, {FALSE,(uint8)(~0),(uint8)(~0)}}, ADC_CAL_STATUS__NONE },  // PM_MUXSCALE13
        { ADC_CAL_METHOD__1PT_INTERPOLATION, {{FALSE,       2100,(uint8)(~0)}, {FALSE,(uint8)(~0),(uint8)(~0)}}, ADC_CAL_STATUS__NONE },  // PM_MUXSCALE14
        { ADC_CAL_METHOD__2PT_INTERPOLATION, {{FALSE,        625,(uint8)(~0)}, {FALSE,       1250,(uint8)(~0)}}, ADC_CAL_STATUS__NONE }   // PM_MUXSCALE15
     };

LOCAL adc_cal_output_type         adc_cal_output[ADC_CAL_CONFIG__NUM_CONFIGS];
LOCAL adie_hkadc_muxsel_type      adc_cal_pm_amuxout_adie_muxsel = ADIE_AD_MUX_NONE;
LOCAL adc_cal_done_cb_func_type   adc_cal_done_cb = NULL;
LOCAL uint32                      adc_cal_config_todo_mask = 0;
LOCAL adc_cal_point_type         *adc_cal_curr_cal_pt_measured_ptr = NULL;
LOCAL boolean                     adc_cal_perform_calibration__on_stack = FALSE;


/*===========================================================================
                        FUNCTION  PROTOTYPES
===========================================================================*/
extern ADC_RETURN_TYPE pm_adc_start_read(
   adie_hkadc_muxsel_type adie_muxsel,   // ADIE input selection for PMIC's MUX output
   pm_adc_muxsel_type     pm_muxsel,    // PMIC's MUX input source to select
   pm_adc_muxscale_type   pm_muxscale,  // PMIC's MUX scale
#ifdef FEATURE_ADC_MULTI_RESOLUTION   
   adie_adc_res_type      adie_res,      // Resolution to use for ADC
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
   int                   *adc_result,    // Where to write A/D conversion result
   void (*adc_completion_cb_fn)(void)    // Call-back function when A/D conversion is complete
                                         // (successfully or not)
);

LOCAL void adc_cal_perform_calibration(void);
LOCAL void adc_cal_self_measurement_done(void); 
LOCAL boolean adc_cal_is_pm_internal_ref(uint32 mV);

// 1-pt/2-pt calibration math routines
LOCAL int32 ADC_CAL_IDEAL_GAIN_x_10000(void);
LOCAL int32 ADC_CAL_IDEAL_LSB_x_10000(void);
LOCAL int32 ADC_CAL_IDEAL_ADC_COUNT(int32 mV);
LOCAL int32 ADC_CAL_1PT_GAIN_x_10000(uint8 cal_config_index);
LOCAL int32 ADC_CAL_1PT_OFFSET(uint8 cal_config_index);
LOCAL int32 ADC_CAL_2PT_GAIN_x_10000(uint32 cal_config_index);
LOCAL int32 ADC_CAL_2PT_OFFSET(uint8 cal_config_index);


/*===========================================================================
                       FUNCTION  DEFINITIONS
===========================================================================*/

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
void adc_cal_use_external_measurement(
   adc_cal_config_type     cal_config,
   uint32                  voltage_mV,
   uint8                   adc_count
)
{
   uint8                   curr_cal_config_index = SHIFT_DN_FROM_MASK(cal_config);
   uint8                   pts_array_index = 0;
   adc_cal_point_type     *pt_ptr = NULL;

   for
   (
      pts_array_index = 0;
      pts_array_index < ADC_CAL_MAX_PTS_PER_CAL;
      ++pts_array_index
   )
   {
      pt_ptr = &adc_cal_input[curr_cal_config_index].pts_array[pts_array_index];
      if (voltage_mV == pt_ptr->voltage_mV)
      {
         pt_ptr->adc_count = adc_count;
         pt_ptr->adc_count_available = TRUE;
         break;
      }
   }
}


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
void adc_cal_calibrate(
   uint32                       list_of_cal_config,
   adie_hkadc_muxsel_type       pm_amuxout_adie_muxsel,
   adc_cal_done_cb_func_type    cal_done_cb
)
{
   uint8   curr_cal_config_index = 0;
   uint32  curr_cal_config       = 0;
   uint8   max_cal_config_index  = SHIFT_DN_FROM_MASK(ADC_CAL_CONFIG__MAX_VALID_CONFIG);
   uint8                   pts_array_size = 0;
   uint8                   pts_array_index = 0;
   adc_cal_point_type     *pt_ptr = NULL;
   boolean internal_ref_ch_available = FALSE;

   adc_cal_config_todo_mask = list_of_cal_config;
   adc_cal_pm_amuxout_adie_muxsel = pm_amuxout_adie_muxsel;
   adc_cal_done_cb = cal_done_cb;

   // clean cal result data structure
   for
   (
      curr_cal_config_index=0;
      curr_cal_config_index<=max_cal_config_index;
      ++curr_cal_config_index
   )
   {
      adc_cal_output[curr_cal_config_index].info_available = FALSE;
   }

   // check if internal reference channels are available ...
   (void) pm_adc_in_ref_ch_1p25V_0p625V_available(&internal_ref_ch_available);

   while (list_of_cal_config)
   {
      curr_cal_config_index = SHIFT_DN_FROM_MASK(list_of_cal_config);
      curr_cal_config = (uint32)(1 << curr_cal_config_index);
      pts_array_size = 0;

      switch (adc_cal_input[curr_cal_config_index].method)
      {
         case ADC_CAL_METHOD__1PT_INTERPOLATION:
         {
            pts_array_size = 1;
            break;
         }
         case ADC_CAL_METHOD__2PT_INTERPOLATION:
         {
            pts_array_size = 2;
            break;
         }
      }

      adc_cal_input[curr_cal_config_index].status = ADC_CAL_STATUS__MEASUREMENT_AVAILABLE;

      for
      (
         pts_array_index = 0;
         pts_array_index < pts_array_size;
         ++pts_array_index
      )
      {
         pt_ptr = &(adc_cal_input[curr_cal_config_index].pts_array[pts_array_index]);
         
         // Realize that adc_cal_calibrate() could be called more than once; we don't
         //  want to remember previous cal readings ...
         if( adc_cal_is_pm_internal_ref(pt_ptr->voltage_mV))
         {
            pt_ptr->adc_count_available = FALSE;
         }

         if (!pt_ptr->adc_count_available)
         {
            if (adc_cal_is_pm_internal_ref(pt_ptr->voltage_mV)
                && internal_ref_ch_available)
            {
               adc_cal_input[curr_cal_config_index].status = ADC_CAL_STATUS__MEASUREMENT_NEEDED;
            }
            else
            {
               // user didn't provide measurement for a voltage that PMIC can't generate
               // internally. There is no way calibration can be done for this.
               adc_cal_input[curr_cal_config_index].status = ADC_CAL_STATUS__MEASUREMENT_FAILED;
               break;
            }
         }
      }
      list_of_cal_config &= (~curr_cal_config);
   }

   adc_cal_perform_calibration();
}


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
boolean adc_cal_get_cal_info(
   adc_cal_config_type   cal_config,
   adc_cal_info_type    *cal_info_ptr
)
{
   uint32  cal_config_index = SHIFT_DN_FROM_MASK(cal_config);

   if (cal_info_ptr)
   {
      if ( (cal_config <= ADC_CAL_CONFIG__MAX_VALID_CONFIG)  // add more stringent test TBD
           && (adc_cal_output[cal_config_index].info_available) )
      {
         cal_info_ptr->voltage_mV_offset_x_10000
            = adc_cal_output[cal_config_index].info.voltage_mV_offset_x_10000;
         cal_info_ptr->voltage_mV_vs_adc_count_slope_x_10000
            = adc_cal_output[cal_config_index].info.voltage_mV_vs_adc_count_slope_x_10000;
         return TRUE;
      }
   }

   return FALSE;
}


LOCAL void adc_cal_perform_calibration(void)
{
   uint8   curr_cal_config_index = 0;
   uint32  curr_cal_config       = 0;
   word    int_sav               = 0;

   while (adc_cal_config_todo_mask)
   {
      curr_cal_config_index = SHIFT_DN_FROM_MASK(adc_cal_config_todo_mask);
      curr_cal_config = (uint32)(1 << curr_cal_config_index);

      if (ADC_CAL_STATUS__MEASUREMENT_NEEDED == adc_cal_input[curr_cal_config_index].status)
      {
         // do the measurements here
         if (!adc_cal_input[curr_cal_config_index].pts_array[0].adc_count_available)
         {
            adc_cal_curr_cal_pt_measured_ptr = &adc_cal_input[curr_cal_config_index].pts_array[0];
         }
         else if ( (ADC_CAL_METHOD__2PT_INTERPOLATION == adc_cal_input[curr_cal_config_index].method)
                   && (!adc_cal_input[curr_cal_config_index].pts_array[1].adc_count_available) )
         {
            adc_cal_curr_cal_pt_measured_ptr = &adc_cal_input[curr_cal_config_index].pts_array[1];
         }
         else
         {
            adc_cal_input[curr_cal_config_index].status = ADC_CAL_STATUS__MEASUREMENT_AVAILABLE;
         }
         if (ADC_CAL_STATUS__MEASUREMENT_NEEDED == adc_cal_input[curr_cal_config_index].status)
         {
            // record the fact that this fn is on the stack so that our co-routine
            // adc_cal_self_measurement_done() avoids calling us when we are already
            // on the stack
            adc_cal_perform_calibration__on_stack = TRUE;

            (void)pm_adc_start_read(
               adc_cal_pm_amuxout_adie_muxsel,
               ADC_CAL_GET_PM_MUXSEL_FOR_INTERNAL(adc_cal_curr_cal_pt_measured_ptr->voltage_mV),
               (pm_adc_muxscale_type)curr_cal_config_index,   // which is the same as PM MUXSCALEn
#ifdef FEATURE_ADC_MULTI_RESOLUTION
               ADIE_ADC_CALIBRATION_RESOLUTION,
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
               &(adc_cal_curr_cal_pt_measured_ptr->adc_count),
               adc_cal_self_measurement_done);

            INTLOCK_SAV(int_sav);
            if (!adc_cal_curr_cal_pt_measured_ptr->adc_count_available)
            {
               // If the measurement hasn't yet been completed, it means that
               // adc_cal_self_measurement_done() is yet to be called. We can
               // safely exit this fn and let adc_cal_self_measurement_done()
               // invoke this one later.

               // Break the "while (adc_cal_config_todo_mask)" loop without changing
               // adc_cal_config_todo_mask. It will be adjusted when the measurements are
               // available and the calibration is completed below.

               // Set the flag to indicate that we wouldn't be on the stack when our
               // co-routine executes to indicate to it that it should invoke this fn
               // to continue further measurement and/or calibration.
               adc_cal_perform_calibration__on_stack = FALSE;
               INTFREE_SAV(int_sav);
               return;
            }
            else
            {
               INTFREE_SAV(int_sav);
            }
         }
      }

      if (ADC_CAL_STATUS__MEASUREMENT_AVAILABLE == adc_cal_input[curr_cal_config_index].status)
      {
         // measurement is available; so do the calibration here
         switch (adc_cal_input[curr_cal_config_index].method)
         {
            case ADC_CAL_METHOD__1PT_INTERPOLATION:
            {
               adc_cal_output[curr_cal_config_index].info.voltage_mV_vs_adc_count_slope_x_10000
                  = 10000 * 10000 / ADC_CAL_1PT_GAIN_x_10000(curr_cal_config_index);
               adc_cal_output[curr_cal_config_index].info.voltage_mV_offset_x_10000
                  = (ADC_CAL_IDEAL_LSB_x_10000() / 2)
                    - (int32)(10000
                       * 10000
                       * ((int64)ADC_CAL_1PT_OFFSET(curr_cal_config_index)) // Casting to int64 to prevent overflow
                       / ADC_CAL_1PT_GAIN_x_10000(curr_cal_config_index));

               adc_cal_output[curr_cal_config_index].info_available = TRUE;
               break;
            }
            case ADC_CAL_METHOD__2PT_INTERPOLATION:
            {
               adc_cal_output[curr_cal_config_index].info.voltage_mV_vs_adc_count_slope_x_10000
                  = 10000 * 10000 / ADC_CAL_2PT_GAIN_x_10000(curr_cal_config_index);
               adc_cal_output[curr_cal_config_index].info.voltage_mV_offset_x_10000
                  = (ADC_CAL_IDEAL_LSB_x_10000() / 2)
                    - (int32)(10000
                       * 10000
                       * ((int64)ADC_CAL_2PT_OFFSET(curr_cal_config_index)) // Casting to int64 to prevent overflow
                       / ADC_CAL_2PT_GAIN_x_10000(curr_cal_config_index));

               adc_cal_output[curr_cal_config_index].info_available = TRUE;
               break;
            }
            default:
            {
               break;
            }
         } // switch ...

         adc_cal_input[curr_cal_config_index].status = ADC_CAL_STATUS__CALIBRATION_DONE;
         // strike it off from the TODO list
         adc_cal_config_todo_mask &= (~curr_cal_config);
      }  // if(ADC_CAL_STATUS__MEASUREMENT_AVAILABLE ...

      if (ADC_CAL_STATUS__MEASUREMENT_FAILED == adc_cal_input[curr_cal_config_index].status)
      {
         // measurement is NOT available; so calibration can't be done and so
         // strike it off from the TODO list to prevent infinite loop
         adc_cal_config_todo_mask &= (~curr_cal_config);
      }

   } // while(adc_cal_config_todo_mask) ...

   // If we are here, it means that 'adc_cal_config_todo_mask' is 0 and it signals the end
   // of all calibration; notify our client now.
   adc_cal_done_cb();
}


LOCAL void adc_cal_self_measurement_done(void)
{
   adc_cal_curr_cal_pt_measured_ptr->adc_count_available = TRUE;

   if (!adc_cal_perform_calibration__on_stack)
   {
      adc_cal_perform_calibration();
   }
}


LOCAL boolean adc_cal_is_pm_internal_ref(uint32 mV)
{
   return ( (ADC_CAL_PM_INTERNAL_REF_VOLTAGE1_MV == mV)
            || (ADC_CAL_PM_INTERNAL_REF_VOLTAGE2_MV == mV) );
}


LOCAL int32 ADC_CAL_IDEAL_GAIN_x_10000(void)
{
   return ( 10000
            * ADC_CAL_ADIE_MAX_READING
            / ADC_CAL_ADIE_VREF_MV );
}


LOCAL int32 ADC_CAL_IDEAL_LSB_x_10000(void)
{
   return ( 10000 
			* ADC_CAL_ADIE_VREF_MV
            / ADC_CAL_ADIE_MAX_READING );
}


LOCAL int32 ADC_CAL_IDEAL_ADC_COUNT(int32 mV)
{
   return ( mV
            * ADC_CAL_ADIE_MAX_READING
            / ADC_CAL_ADIE_VREF_MV );
}


LOCAL int32 ADC_CAL_1PT_GAIN_x_10000(uint8 cal_config_index)
{
   return ADC_CAL_IDEAL_GAIN_x_10000();
}


LOCAL int32 ADC_CAL_1PT_OFFSET(uint8 cal_config_index)
{
   return ( adc_cal_input[cal_config_index].pts_array[0].adc_count
            - ADC_CAL_IDEAL_ADC_COUNT(adc_cal_input[cal_config_index].pts_array[0].voltage_mV) );
}


LOCAL int32 ADC_CAL_2PT_GAIN_x_10000(uint32 cal_config_index)
{
   int32 num = adc_cal_input[cal_config_index].pts_array[0].adc_count
                - adc_cal_input[cal_config_index].pts_array[1].adc_count;
   int32 den = adc_cal_input[cal_config_index].pts_array[0].voltage_mV
                - adc_cal_input[cal_config_index].pts_array[1].voltage_mV;

   return 10000 * num / den;
}


LOCAL int32 ADC_CAL_2PT_OFFSET(uint8 cal_config_index)
{
   return ( adc_cal_input[cal_config_index].pts_array[0].adc_count
            - ( adc_cal_input[cal_config_index].pts_array[0].voltage_mV
                * ADC_CAL_2PT_GAIN_x_10000(cal_config_index)
                / 10000 ) );
}

#endif // FEATURE_ADC_CALIBRATION

