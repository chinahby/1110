/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            P M I C   H K A D C   R E A D   S T A T E   M A C H I N E
                ( U S I N G   P M I C   M M   A P I   H K A D C )

GENERAL DESCRIPTION
   Process for making one read using PMIC MUX and ADIE's ADC. This process
   is necessary because of the asynchronous nature of the SBI transfer driver. It
   starts with a call to pm_adc_start_read() specifying the input to be read and
   runs until the call-back function *adc_completion_cb_fn() signals to the caller
   that the process has completed. When (if) the process is successful the new value
   will be in *adc_result_ptr.

   pm_adc_start_read() should not be called again until adc_completion_cb_fn().

EXTERNALIZED FUNCTIONS
  pm_adc_start_read

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2003, 2004, 2005 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/hwengines/adc/qsc1100/main/latest/src/pm_adc_sm.c#1 $ $DateTime: 2008/08/14 10:53:43 $ $Author: jthielen $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
04-01-05   ty      Changed code to support multiple ADC resolutions
01-20-04   rmd      Change the code so that we only shut down the PMIC's AMUX
                   if the PMIC rev allow us to do so.
01-07-03   JAC     added pm_adc_sm_adie_reset
08-20-03   Vish    Changed AMUX o/p settling time to 15us busy wait.
07-31-03   Vish    Added AMUX o/p settling time of 6us busy wait.
07-28-03   Vish    Created.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "target.h"
#include "comdef.h"
#include "msg.h"
#include "clk.h"
#include "adc.h"
#include "pm.h"
#include "adie.h"
#include "adci.h"


/*===========================================================================

                 LOCAL CONSTANT AND MACRO DEFINITIONS

===========================================================================*/
// AMUX output settling time
#define PM_ADC_SM_AMUXOUT_SETTLING_TIME_USEC        (15)

// This state machine's various states
typedef enum
{
  PM_ADC_SM_STATE_IDLE,
  PM_ADC_SM_STATE_RUNNING
} pm_adc_sm_state_type;


/*===========================================================================

                VARIABLES DEFINITIONS

===========================================================================*/

// Pointer to the fn to be called by this SM when the ADC
// read process is completed
LOCAL void (*pm_adc_sm_read_completion_cb_fn)(void);

// Current state
LOCAL pm_adc_sm_state_type pm_adc_sm_state;


/*===========================================================================

                FUNCTION PROTOTYPES

===========================================================================*/
extern adc_error_return_type adie_adc_start_read(
   adie_hkadc_muxsel_type adc_addr,            /* A/D input data source to select      */
#ifdef FEATURE_ADC_MULTI_RESOLUTION
   adie_adc_res_type    adie_res,      /* Resolution to use for ADC            */
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
   int *adc_result,                    /* Where to write A/D conversion result */
   void (*adc_completion_cb_fn)(void)  /* Call-back function when A/D conversion is complete
                                          (successfully or not) */
  );

LOCAL void pm_adc_sm_adie_done_callback(void);


/*===========================================================================

                FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION pm_adc_sm_adie_reset                           LOCAL FUNCTION

DESCRIPTION
  This fn turns OFF PMIC's MUX to avoid the loading of analog input source,
  resets this state machine to IDLE state so as to be ready for the next
  conversion request.

PARAMETERS
  None.


RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void pm_adc_sm_adie_reset(void)
{
#ifdef FEATURE_AMUX_SHUTDOWN_CHECK 
  boolean turn_off_amux = TRUE; 

   // turn off PMIC MUX to avoid loading analog input source
  (void)pm_adc_can_we_turn_off_the_amux(&turn_off_amux);

  if (turn_off_amux == TRUE) 
#endif

  {   // Ok, we are allowed to shut down the AMUX.
    (void)pm_adc_config_mux(PM_ADC_MUXSEL_NONE, PM_ADC_MUXSCALE0);
  }

  // reset this SM's state to IDLE
  pm_adc_sm_state = PM_ADC_SM_STATE_IDLE;
}

/*===========================================================================

FUNCTION pm_adc_start_read                                  EXTERNAL FUNCTION

DESCRIPTION
  This function configures the analog MUX of the PMIC to the desired input
  source and starts ADC conversion/reading on the ADIE's ADC.
  The whole ADC process can be asynchronous and so when the result is available,
  it will be stored in '*adc_result' and the caller of this fn is notified
  through the callback - adc_completion_cb_fn().


PARAMETERS
  adie_muxsel           --->   Which ADIE's MUX input is PMIC's MUX output connected to
  pm_muxsel             --->   Which PMIC's MUX input is the analog input source connected to
  pm_muxscale           --->   What scaling (voltage offset/gain) should PMIC's MUX use
  adc_result,           --->   Where to write A/D conversion result
  adc_completion_cb_fn  --->   Callback function for notifying that A/D result is ready


RETURN VALUE
  adc_error_return_type
    ADC_ERR_FLAG__SUCCESS          --->   Conversion process successfully started
    ADC_ERR_FLAG__ALREADY_STARTED  --->   Another conversion is already in progress
    ADC_ERR_FLAG__CHAN_NOT_PRESENT --->   Invalid PMIC MUX input chosen for reading

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
adc_error_return_type pm_adc_start_read(
   adie_hkadc_muxsel_type adie_muxsel,   // ADIE input selection for PMIC's MUX output
   pm_adc_muxsel_type     pm_muxsel,     // PMIC's MUX input source to select
   pm_adc_muxscale_type   pm_muxscale,   // PMIC's MUX scale
#ifdef FEATURE_ADC_MULTI_RESOLUTION   
   adie_adc_res_type      adie_res,      // Resolution to use for ADC
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
   int                   *adc_result,    // Where to write A/D conversion result
   void (*adc_completion_cb_fn)(void)    // Call-back function when A/D conversion is
                                         // complete (successfully or not)
)
{
   adc_error_return_type err_flag = ADC_ERR_FLAG__SUCCESS; /* Assume success */

   if (PM_ADC_SM_STATE_IDLE == pm_adc_sm_state)
   {
      // select and configure PMIC MUX for the appropriate analog input
      if (PM_ERR_FLAG__SUCCESS != pm_adc_config_mux(pm_muxsel, pm_muxscale))
      {
         err_flag = ADC_ERR_FLAG__CHAN_NOT_PRESENT; // closest error I could get to!
      }
      else
      {
         pm_adc_sm_state = PM_ADC_SM_STATE_RUNNING;

         // wait for the PMIC MUX's output to stabilize
         clk_busy_wait(PM_ADC_SM_AMUXOUT_SETTLING_TIME_USEC);

         // store this SM's client's call-back for notification later when ADIE
         // calls us back when it is done A2D conversion
         pm_adc_sm_read_completion_cb_fn = adc_completion_cb_fn;

         // perform ADIE AD conversion
#ifdef FEATURE_ADC_MULTI_RESOLUTION
         err_flag = adie_adc_start_read(adie_muxsel, adie_res, adc_result, pm_adc_sm_adie_done_callback);
#else 
         err_flag = adie_adc_start_read(adie_muxsel, adc_result, pm_adc_sm_adie_done_callback);
#endif

         if (ADC_ERR_FLAG__SUCCESS != err_flag)
         {
            // we couldn't start ADIE and that means that pm_adc_sm_adie_done_callback()
            // will not be called back in this case; so reset the SM's state to IDLE and
            // reset PMIC's MUX here
            pm_adc_sm_adie_reset();
         }
      }
   }
   else /* trying to start another conversion while one is in progress */
   {
     MSG_ERROR("PM conv not finished",0,0,0); 
     MSG_ERROR("Resetting state machine",0,0,0);

     /*We are stuck, reset state machine*/
     pm_adc_sm_adie_reset();
      err_flag = ADC_ERR_FLAG__ALREADY_STARTED;
   }

   return err_flag;
}


/*===========================================================================

FUNCTION pm_adc_sm_adie_done_callback                         LOCAL FUNCTION

DESCRIPTION
  This function is called back by the ADIE ADC driver to notify that the
  conversion is complete. ADIE ADC would have already copied the result into
  the location provided through the adie_adc_start_read() fn.

  This fn turns OFF PMIC's MUX to avoid the loading of analog input source,
  resets this state machine to IDLE state so as to be ready for the next
  conversion request. This fn also notifies the client of this state machine
  that the conversion is over.


PARAMETERS
  None.


RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void pm_adc_sm_adie_done_callback(void)
{
   /*Turns off PMIC mux, and resets pm_adc_sm_state*/ 
   pm_adc_sm_adie_reset();

   // call this SM's client to notify that ADC read has been completed
   if (pm_adc_sm_read_completion_cb_fn)
   {
      pm_adc_sm_read_completion_cb_fn();
   }
}
