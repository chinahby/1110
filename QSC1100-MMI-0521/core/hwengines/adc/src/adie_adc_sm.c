/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            A D I E   H K A D C   R E A D   S T A T E   M A C H I N E
                ( U S I N G   A D I E   M M   H K A D C   A P I )

GENERAL DESCRIPTION
   Process for making one read of an ADIE Housekeeping ADC input. This process
   is necessary because of the asynchronous nature of the SBI transfer driver. It
   starts with a call to adie_adc_start_read() specifying the input to be read and
   runs until the call-back function *adc_completion_cb_fn() signals to the caller
   that the process has completed. When (if) the process is successful the new value
   will be in *adie_adc_result_ptr.

   adie_adc_start_read() should not be called again until adc_completion_cb_fn().

   There is an equivalent process for the PMIC ADC if that is used by the
   target.

EXTERNALIZED FUNCTIONS
  adie_adc_start_read

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2003, 2004, 2005, 2006 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/hwengines/adc/qsc1100/main/latest/src/adie_adc_sm.c#1 $ $DateTime: 2008/08/14 10:53:43 $ $Author: jthielen $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/17/06   ty      Decrease ADC conversion wait time
11/10/05   ty      Added ADC debug messages 
08/24/05   ty      Added delay before reading ADC conversion for MDSP
16/06/05   jac     Added FEATURE_RF_TX_PWR_LIM_REV for new HDET implementation
04/01/05   ty      made changes to support multiple ADC resolutions
01/07/04   jac     removed periodic timer
09/20/04   jac     changed adie_adc_status_timer_cb to periodic timer
04/05/04   jac     removed assert
01/23/04   rv      Added support for HKADC clk shutdown.
11/05/03   et      changed ADIE_AD_VREF_VDD to ADIE_AD_VREF_VDDA to match pluto enum
11-11-03   jac     added include for adci.h
10/13/03   jac     replaced ADIE_HAS_MM_ADC with FEATURE_ADIE_HAS_MM_ADC 
10/02/03   jac     put back adie_adc_status_timer_cb, in gsm mode sbi transactions
                   may be pending  
09/11/03   jac     Removed locking of adc due to adie read problems, adie takes care
                   of reading conv if it doesn't finish it will never finish
09/08/03   Vish    Added handling of failure of adie_start_adc_conv() in
                   adie_adc_state_machine() so that the state machine doesn't
                   lockup. 
08/28/03   Vish    Added support for non multimode (non-callback) ADIE ADC;
                   featurized multi-mode code using ADIE_HAS_MM_ADC
03/05/03   rmd     Wrap clk_regim_enable/disable code around
                   ADIE_HAS_CLK_REGIME.
03-03-11   tjw     Failure timeout turned off.
03-02-24   tjw     Set up a timer to re-read the status if the ADC is not complete
03-02-19   tjw     adie.h replaces adie_sbi.h
03-02-14   tjw     State machine correction - API misunderstood.
03-02-11   tjw     Created.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "target.h"
#include "comdef.h"
#include "assert.h"
#include "clk.h"
#include "clkregim.h"
#include "adc.h"
#include "adie.h"
#include "adci.h"
#include "err.h"

/*===========================================================================

                 LOCAL CONSTANT AND MACRO DEFINITIONS

===========================================================================*/

// this state definition is common to both multi-mode and synchronous ADIE ADC
typedef enum
{
  ADIE_STATE_START,
  ADIE_STATE_API_RESPONSE,
  ADIE_STATE_ABORT
} adie_adc_state_type;


//    !!!!!!!!!!!!!!!    Multi-mode ADIE ADC    !!!!!!!!!!!!!!!
#ifdef FEATURE_ADIE_HAS_MM_ADC
#error code not present
#endif  //FEATURE_ADIE_HAS_MM_ADC
//   !!!!!!!!!!!!!!!    Multi-mode ADIE ADC support ends here   !!!!!!!!!!!!!!!

/*===========================================================================

                VARIABLES DEFINITIONS

===========================================================================*/

// this state definition is common to both multi-mode and synchronous ADIE ADC
LOCAL adie_adc_state_type adie_adc_state=ADIE_STATE_START;

//    !!!!!!!!!!!!!!!    Multi-mode ADIE ADC    !!!!!!!!!!!!!!!
#ifdef FEATURE_ADIE_HAS_MM_ADC
#error code not present
#endif  //FEATURE_ADIE_HAS_MM_ADC
//   !!!!!!!!!!!!!!!    Multi-mode ADIE ADC support ends here   !!!!!!!!!!!!!!!


/*===========================================================================

                FUNCTION PROTOTYPES

===========================================================================*/

//    !!!!!!!!!!!!!!!    Multi-mode ADIE ADC    !!!!!!!!!!!!!!!
#ifdef FEATURE_ADIE_HAS_MM_ADC
#error code not present
#endif  //FEATURE_ADIE_HAS_MM_ADC
//   !!!!!!!!!!!!!!!    Multi-mode ADIE ADC support ends here   !!!!!!!!!!!!!!!

/*===========================================================================

                FUNCTION DEFINITIONS

===========================================================================*/

//    !!!!!!!!!!!!!!!    Multi-mode ADIE ADC    !!!!!!!!!!!!!!!
#ifdef FEATURE_ADIE_HAS_MM_ADC
#error code not present
#endif  //FEATURE_ADIE_HAS_MM_ADC
//   !!!!!!!!!!!!!!!    Multi-mode ADIE ADC support ends here   !!!!!!!!!!!!!!!


/*===========================================================================

FUNCTION adie_adc_start_read                                EXTERNAL FUNCTION

DESCRIPTION
  This function just starts a new conversion and returns the last result for the
  specified source, the new result will be put in *adc_result later,
  The whole process is asynchronous, when it is completed adc_completion_cb_fn()
  will be called to signal that another conversion may be started.

PARAMETERS
  int adc_addr,                      A/D input data source to select
  int *adc_result,                   Where to write A/D conversion result
  void (*adc_completion_cb_fn)(void) Call-back function when A/D conversion is complete
                                     (successfully or not)

RETURN VALUE
  adc_error_return_type
    ADC_ERR_FLAG__SUCCESS         conversion process started
    ADC_ERR_FLAG__ALREADY_STARTED a conversion is already in progress

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/

/* Finally, the function to start a read */
adc_error_return_type adie_adc_start_read(
  adie_hkadc_muxsel_type adc_addr,            /* A/D input data source to select      */
#ifdef FEATURE_ADC_MULTI_RESOLUTION
  adie_adc_res_type    adc_res,       /* Resolution to use for ADC conversion */ 
#endif /* FEATURE_ADC_MULTI_RESOLUTION */
  int *adc_result,                    /* Where to write A/D conversion result */
  void (*adc_completion_cb_fn)(void)  /* Call-back function when A/D conversion is complete
                                         (successfully or not) */
)
{
  adc_error_return_type err_flag = ADC_ERR_FLAG__SUCCESS; /* Assume success */
#ifndef FEATURE_ADIE_HAS_MM_ADC  // synchronous ADIE ADC
  adie_adc_config_type   adc_sync_config_info;  // configuration info for ADIE ADC
  uint16                 adc_sync_result;   // result from synchronous read call to ADIE ADC
#endif

#ifdef FEATURE_ADIE_HAS_MM_ADC  // synchronous ADIE ADC
#error code not present
#else

  if (adie_adc_state == ADIE_STATE_START)
  {
    adie_adc_state = ADIE_STATE_API_RESPONSE;
    adc_sync_config_info.muxsel = adc_addr;
    adc_sync_config_info.vref = ADIE_AD_VREF_VDDA;

#ifdef FEATURE_ADC_MULTI_RESOLUTION
    adie_adc_set_res(adc_res);
#endif /* FEATURE_ADC_MULTI_RESOLUTION */

    if (adie_adc_read(adc_sync_config_info, &adc_sync_result))
    {
       // no error and conversion is over
       *adc_result = adc_sync_result;
    }
    // we're done with conversion; revert back to the "available" state
    adie_adc_state = ADIE_STATE_START;
    adc_completion_cb_fn();    // let user know that we're done converting
  }
  else /* trying to start another conversion while one is in progress */
  {
    err_flag = ADC_ERR_FLAG__ALREADY_STARTED;
  }
#endif //FEATURE_ADIE_HAS_MM_ADC

  return err_flag;
}

