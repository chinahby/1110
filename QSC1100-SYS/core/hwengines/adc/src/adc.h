#ifndef ADC_H
#define ADC_H
/*===========================================================================


                  A D C    H E A D E R    F I L E

DESCRIPTION
  This file contains prototype definitions for the ADC API. This uses
  logical "channels" to specify the ADC input required and the units expected.
  The details of each channel may vary from target to target but the result
  returned for any channel should always be the same to ensure portability
  of the user code.


Copyright (c) 2003 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/hwengines/adc/qsc1100/main/latest/inc/adc.h#2 $ $DateTime: 2008/11/03 16:38:25 $ $Author: jthielen $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07-10-07   va      Corrected the channel for PMIC fine resolution temp.
06-10-07   abh     Changed the return type for ADC read to enable higher precison
                    temperature readings
06-16-05   jac     Added FEATURE_RF_TX_PWR_LIM_REV for new HDET implementation
02-28-05   Vish    Added adc_block_channel() and adc_unblock_channel().
05-12-04   Vish    Added function prototype for adc_init().
03-02-18   tjw     Added adc_reset_filter(). adc_raw_reading is now unsigned.
03-01-06   tjw     Created.
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "target.h"
#include ADC_CHANNELS_H       /* (adc_channels_<target>.h) defined in target<target>.h,
                                 contains an enum of the channel numbers */

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

/*===========================================================================

  Status return values

===========================================================================*/
typedef enum
  {
  ADC_ERR_FLAG__SUCCESS,          /* Conversion was successful */
  ADC_ERR_FLAG__NOT_READY,        /* Conversion not ready */
  ADC_ERR_FLAG__READ_ERR,         /* Read error */
  ADC_ERR_FLAG__RANGE_ERR,        /* Reading out of range */
  ADC_ERR_FLAG__CHAN_NOT_PRESENT, /* Selected channel not present in target */
  ADC_ERR_FLAG__ALREADY_STARTED   /* The conversion process is already underway */
  } adc_error_return_type;

/*===========================================================================

  The result may be returned in any of these units. Each channel uses only
  one unit in the union.

===========================================================================*/
typedef unsigned int adc_raw_reading;
typedef int adc_as_mV;
typedef int adc_as_degC;
typedef int adc_as_percentage;
typedef int adc_as_tenths_of_dBm;
typedef int adc_as_unspecified_units;   /* For target-specific conversion
                             functions that return units other than the above */
typedef union
  {
  adc_raw_reading raw;
  adc_as_mV mV;
  adc_as_degC degC;
  adc_as_percentage percentage;
  adc_as_tenths_of_dBm tenths_of_dBm;
  adc_as_unspecified_units unspecified_units;
  } adc_result_type;

/*===========================================================================

                        PROTOTYPE DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION   adc_init                                        EXTERNAL FUNCTION

DESCRIPTION
  This is used for performing target specific ADC initializations. This
  function should be called to let ADC perform tasks like self-calibration
  before it could be used for performing conversions later. Ideally this
  function should be called at system start-up.

PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  This function is *NOT* defined in this module and it *MUST* be defined
  in a target specific ADC map file before it can be used.

SIDE EFFECTS
  None.
===========================================================================*/
extern void adc_init(void);


/*===========================================================================

FUNCTION adc_read_channel                                    EXTERNAL FUNCTION

DESCRIPTION

PARAMETERS
  adc_logical_channel_type channel
  adc_result_type *result

RETURN VALUE
  adc_error_return_type
    ADC_ERR_FLAG__SUCCESS   .
    ADC_ERR_FLAG__READ_ERR  ADC driver reported a problem

DEPENDENCIES
  The following functions need to be called before we call this
  function:

SIDE EFFECTS
  A read of the ADC may be invoked.
  Interrupts are disabled briefly while processing the scheduler.
===========================================================================*/

extern adc_error_return_type adc_read_channel(adc_logical_channel_type channel,
                adc_result_type *result);


/*===========================================================================
FUNCTION  adc_block_channel                                EXTERNAL FUNCTION

DESCRIPTION
  This fn is used for blocking the ability to perform A/D conversions of
  the specified logical channel.
   
  Note:
     All ADC logical channels are unblocked by default.
     
     If a particular ADC logical channel was blocked using 
     adc_block_channel(), then any subsequent ADC reads will not
     perform A/D conversions but will simply return the result of the last
     successful A/D conversion that ever took place before that logical
     channel was blocked.

PARAMETERS
  adc_logical_channel_type channel
    - Specifies the logical channel to be blocked. A special value of
      NUMBER_OF_LOGICAL_ADC_CHANNELS may be used for blocking all available
      logical channels in one shot.

RETURN VALUE
  None.
   
DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void adc_block_channel(adc_logical_channel_type channel);


/*===========================================================================
FUNCTION  adc_unblock_channel                              EXTERNAL FUNCTION

DESCRIPTION
  This fn is used for unblocking the previously blocked logical channel. This
  restores the ability to perform A/D conversions of the indicated logical
  channel(s).
   
  Note:
     All ADC logical channels are unblocked by default.
     
     If a particular ADC logical channel was blocked using 
     adc_block_channel(), then any subsequent ADC reads will not
     perform A/D conversions but will simply return the result of the last
     successful A/D conversion that ever took place before that logical
     channel was blocked.

PARAMETERS
  adc_logical_channel_type channel
    - Specifies the logical channel to be unblocked. A special value of
      NUMBER_OF_LOGICAL_ADC_CHANNELS may be used for unblocking all available
      logical channels in one shot.

RETURN VALUE
  None.
   
DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void adc_unblock_channel(adc_logical_channel_type channel);


/*===========================================================================

FUNCTION adc_reset_filter                                   EXTERNAL FUNCTION

DESCRIPTION
  Resets the filter used by the channel by re-initialising its entry in
  channel_result[].

PARAMETERS
  adc_logical_channel_type channel

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void adc_reset_filter(adc_logical_channel_type channel);

#ifdef FEATURE_RF_TX_PWR_LIM_REV
/*===========================================================================

FUNCTION adc_read_adc_result_for_mdsp                   EXTERNAL FUNCTION

DESCRIPTION
  Reads result of adc conversion and returns it back in result parameter

PARAMETERS
  adc_result_type *result
    pointer of memory location to put result in

RETURN VALUE
  adc_error_return_type
      ADC_ERR_FLAG__SUCCESS   Things seem OK
      ADC_ERR_FLAG__READ_ERR  ADC driver reported a problem

DEPENDENCIES
  Must be in WCDMA mode when function is called or SBI transactions may be delayed.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern adc_error_return_type adc_read_adc_result_for_mdsp(adc_result_type *result);

/*===========================================================================

FUNCTION adc_clock_enable                   EXTERNAL FUNCTION

DESCRIPTION
  External function to allow outside module to request that the adc
  clock be always left on. This is used when the MDSP is controlling the
  adc
  
PARAMETERS
  boolean enable
    TRUE - enables clock
    FALSE - disables clock
  
RETURN VALUE
  None
  
DEPENDENCIES
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void adc_clock_enable(boolean enable);
#endif

/*=========================================================================*/

#ifndef FEATURE_ADC_USE_API_EXCLUSIVELY
/* for backwards compatibility: */

/* ADC return type */
typedef uint32 ADC_RETURN_TYPE;

#ifdef USE_PMIC_ADC
#define ADC_INPUT_NUM   9
#else
#define ADC_INPUT_NUM   7
#endif

/* ADC previously read value */
extern ADC_RETURN_TYPE adc_prev_val[];

#define adc_mux_device  adc_logical_channel_type

/*===========================================================================

FUNCTION ADC_READ

DESCRIPTION
  Selects the specified data source, initiates an A/D conversion and returns
  the result of the conversion. Provided for backwards compatibility.

DEPENDENCIES
  None

RETURN VALUE
  raw byte or word value of the ADC conversion

SIDE EFFECTS
  None

===========================================================================*/
extern ADC_RETURN_TYPE adc_read
(
  adc_logical_channel_type adc_addr    /* A/D input data source to select */
);

#endif /* FEATURE_ADC_USE_API_EXCLUSIVELY */

#define ADC_READ_XO_THERM() (adc_read(ADC_XO_THERM_DEGC))
#define CALIBRATE_XO_THERM_ADC() {}
#define CALIBRATE_XO_THERM_GAIN() {}

#define ADC_READ_PM_THERM() adc_read(ADC_PMIC_THERM_DEGC)
#define ADC_READ_PA_THERM() adc_read(ADC_PA_THERM_DEGC)
#endif /* ADC_H */
