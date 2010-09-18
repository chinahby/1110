/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         A N A L O G U E   T O   D I G I T A L   C O N V E R S I O N

GENERAL DESCRIPTION
  This file contains the API for A/D conversion. This is the top level of a
  multi-level process and as such can be ported to any project. The main function
  adc_read_channel() is passed a channel number and returns the latest value
  for that channel in the units specified for that channel. The configuration
  of each channel is specified elsewhere in the target-specific table
  adc_logical_channel_definitions.

  This file also contains two generic filter functions and three type conversion
  functions. These can be used as they are, or as templates for target-specific
  functions elsewhere.

EXTERNALIZED FUNCTIONS
  adc_read_channel
  adc_reset_filter
  adc_inc_dec_filter
  adc_null_filter
  adc_to_mV
  adc_to_percentage
  adc_null_conversion

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2003,2004,2005,2006 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/hwengines/adc/qsc1100/main/latest/src/adc.c#3 $ $DateTime: 2008/11/03 16:38:25 $ $Author: jthielen $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09-29-08   cu      Cleaning up lint errors
09-25-08   jdt     INTLOCK all of adc_read_channel to prevent any possible 
                   interruption during ADC reads
07-30-07   cr      Match previously un-matched INTLOCK with an INTFREE.
06-16-07   abh     Changes to support XO Thermistor ADC
01-17-06   ty      Prevent too many successive recursive ADC reads
11-10-05   ty      Add ADC debug messages
06-16-05   jac     Added FEATURE_RF_TX_PWR_LIM_REV for new HDET implementation
04-05-05   ty      prevent sleep when doing ADC reads
02-28-05   Vish    Added adc_block_channel() and adc_unblock_channel().
01-06-04   jac     reverted back to non periodic timer
09-20-04   jac     changed timeout timer to be periodic
04-05-04   jac     remove asserts
02-18-04   jac     clean up lint errors
01-07-03   jac     increased read timeout delay
10-02-03   jac     added read timeout cb
08-15-03   Vish    Changed full_scale_result/reading params in the conversion
                   fns to offset/slope respectively. Also updated the equation
                   for calculating result out of reading using the new 
                   offset/slope mechanism.
03-02-18   tjw     Added adc_reset_filter()
03-01-06   tjw     Created.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#include "assert.h"
#include "adc.h"
#include "adci.h"
#include "clk.h"
#include "clkregim.h"
#include "msg.h"
#include "rex.h"

#ifdef FEATURE_RF_TX_PWR_LIM_REV
#include "mdsp.h"
#endif /* FEATURE_RF_TX_PWR_LIM_REV */

#ifdef FEATURE_ADC_CALIBRATION
#include "adc_cal.h"
#endif //FEATURE_ADC_CALIBRATION


/*===========================================================================

                 LOCAL CONSTANT AND MACRO DEFINITIONS

===========================================================================*/
#define ADC_READ_TIMEOUT     225 /* Read Time out, in compressed mode may take up to 50ms */
                                 /* to perform SBI transaction to read conversion.        */
                                 /* Requires multiple SBI transactions to setup PMUX,     */
                                 /* setup AD converter, and read the result               */
#define ADC_SCHEDULER_INTERVAL 5  /* Interval in ms between processing queued
                                   * adc reads
                                   */

LOCAL  clk_cb_type adc_read_timeout_cb = {0};
LOCAL  clk_cb_type adc_process_scheduler_cb = {0};

typedef enum {
  IDLE,
  RUNNING
} adc_schedule_state_type;

#ifdef FEATURE_RF_TX_PWR_LIM_REV
boolean adc_set_arm_reserved_adc(boolean);
boolean adc_set_mdsp_reserved_adc(boolean);
#endif
void adc_read_complete(void);

/*===========================================================================

                VARIABLES DEFINITIONS

===========================================================================*/

/* Buffer for smoothing channel results */
LOCAL int channel_result[NUMBER_OF_LOGICAL_ADC_CHANNELS];
/* Buffer for readings from ADC */
LOCAL int last_adc_reading[NUMBER_OF_LOGICAL_ADC_CHANNELS];
// Blocked status flag for all ADC logical channels
LOCAL boolean adc_blocked_status[NUMBER_OF_LOGICAL_ADC_CHANNELS];
/* State of the adc scheduler */
LOCAL adc_schedule_state_type adc_schedule_state = IDLE;
/* Queue for adc reads */
LOCAL adc_logical_channel_type adc_schedule[NUMBER_OF_LOGICAL_ADC_CHANNELS];

LOCAL int items_in_schedule = 0;

#ifdef FEATURE_RF_TX_PWR_LIM_REV
/* keeps track of whether adc was reserved by ARM */
uint16 adcBusyFlagFromArm=0;
/* keeps track of whether adc was reserved by MDSP */
uint16 hdetConvStartedFlag=0;
#endif /* FEATURE_RF_TX_PWR_LIM_REV */

/*===========================================================================

                FUNCTION PROTOTYPES

===========================================================================*/
extern void pm_adc_sm_adie_reset(void);
LOCAL adc_schedule_state_type adc_read_add_to_schedule(adc_logical_channel_type channel);
LOCAL void adc_read_timeout_callback(int4 ms_interval_unused);
LOCAL void adc_set_blocking_status(adc_logical_channel_type channel, boolean status);
static adc_schedule_state_type adc_get_schedule_state(void);
static void adc_set_schedule_state(adc_schedule_state_type);


/*===========================================================================

                FUNCTION DEFINITIONS

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

SIDE EFFECTS
  None.
===========================================================================*/
void adc_init(void)
{
#ifdef FEATURE_ADC_CALIBRATION
   adc_calibrate();
#endif //FEATURE_ADC_CALIBRATION
}

/*===========================================================================

FUNCTION adc_read_channel                                   EXTERNAL FUNCTION

DESCRIPTION

PARAMETERS
  adc_logical_channel_type channel
    The channel numbers are specific to one project. Each one indicates
    �   the source of the reading (e.g. battery voltage), not the ADC input number.
    �   the way it is processed (whether the value is filtered to eliminate noisy
      readings, whether the reading is restricted to a range of values).
    �   the way it is obtained (an immediate read of the ADC or the last value
      returned by a scheduled read).
    �   the way the ADC reading is interpreted and returned.

    More than one channel number may be mapped onto a single ADC input,
    e.g. the battery thermistor might be viewed in degC but also as a raw reading
    to check that the thermistor is present. Channels that just return the ADC
    reading can be provided for backwards-compatibility.

    Each channel is characterised by an entry in a table. The table is specific
    to a particular target and so the actual ADC processing is portable.

  adc_result_type *result

    The result is returned in a union of the possible types so the user code can
    handle it appropriately. If the channels are chosen carefully the result
    should be fairly independent of the target so the user code is portable.

    The implementation of the ADC driver may mean that it is not possible to get a
    reading on demand, in which case a local buffer is maintained and readings are
    taken from that. Whatever the situation, the result is returned immediately.

    In some circumstances a dummy result will be returned:
    �   the input is not present on the target.
    �   the input hasn't been read.
    �   the read failed.

    This will be signalled in the status/error return value (adc_error_return_type).
    The dummy result is part of the channel definition table so for example a SURF
    target (that does not have a charger input) can return the safe value 0 for
    charger voltage. Thus the error report can often be ignored, but there are
    situations where the user code needs to know there was an error. For example
    when trying to determine the baseband revision we need to keep reading until
    we get a real result.

RETURN VALUE
  adc_error_return_type
    ADC_ERR_FLAG__SUCCESS   Things seem OK
    ADC_ERR_FLAG__READ_ERR  ADC driver reported a problem
    ADC_ERR_FLAG__RANGE_ERR The ADC reading was out of range, the previous
                            result is returned.
    ADC_ERR_FLAG__NOT_READY No ADC reading was available, the default
                            result is returned.

DEPENDENCIES
  The ADC drivers must be initialised before real results will be returned.

SIDE EFFECTS
  A read of the ADC may be invoked.
  Interrupts are disabled briefly while processing the scheduler.
===========================================================================*/

adc_error_return_type adc_read_channel(adc_logical_channel_type channel,
                adc_result_type *result)
{
  adc_error_return_type error_return = ADC_ERR_FLAG__SUCCESS;
  adc_error_return_type adc_read_error;
  int adc_reading;
  static boolean initialised = FALSE;
  adc_logical_channel_definition_type *channel_definition;  /* for a little optimisation */
  void (*adc_completion_cb_fn)(void);

  channel_definition = (adc_logical_channel_definition_type *)&adc_logical_channel_definitions[channel];

  ADC_DEBUG_MSG("adc_read_channel(): chan %d", channel, 0, 0);

  if (initialised == FALSE)
  { /* Initialise the local buffers */
    int i;

#ifdef FEATURE_RF_TX_PWR_LIM_REV
    /* At initialization, both mdsp and arm should not already have gained
     * control of the adc. Likely to be uninitialized, so we
     * reset the handshaking variables.
     */
    if (adc_check_mdsp_reserved_adc()==TRUE)
    {
      (void)adc_set_mdsp_reserved_adc(FALSE);
      MSG_ERROR("adc already reserved by mdsp when initializing, resetting", 0, 0, 0);
    }
    if (adc_check_arm_reserved_adc()==TRUE)
    {
      (void)adc_set_arm_reserved_adc(FALSE);
      MSG_ERROR("adc already reserved by arm when initializing, resetting", 0, 0, 0);
    }
#endif
    ADC_DEBUG_MSG("adc_read_channel(): Init local buffer", 0, 0, 0);

    for (i = 0; i < (int) NUMBER_OF_LOGICAL_ADC_CHANNELS; i++)
    {
      channel_result[i] = NO_READING_IN_BUFFER;
      last_adc_reading[i] = NO_READING_IN_BUFFER;
      adc_set_blocking_status((adc_logical_channel_type)i, FALSE);
    }
    
#ifdef FEATURE_ADC_VOTE_FOR_SLEEP
    ADC_SLEEP_ALLOW();
#endif /*FEATURE_ADC_VOTE_FOR_SLEEP */

    /*Initialize read timeout timer, used if ADC module stuck*/
    clk_def(&adc_read_timeout_cb);
    initialised= TRUE;
  }

  if (channel >= NUMBER_OF_LOGICAL_ADC_CHANNELS)
  {
    MSG_ERROR("Invalid channel param %d",channel,0,0);
    return ADC_ERR_FLAG__READ_ERR;
  }
  
  /* Check that the entry in the table is correct */
  if(channel_definition->logical_channel_number != channel)
  {
    MSG_ERROR("ADC table mismatch %d %d",channel_definition->logical_channel_number,channel,0); 
  }
  
  if (result==NULL)
  {
    MSG_ERROR("NULL result ptr",0,0,0);
    return ADC_ERR_FLAG__READ_ERR;
  }

  if (adc_blocked_status[channel])
  {
     // if the channel has been blocked, return an error status
     // to indicate that we didn't really perform an A/D conversion...
     ADC_DEBUG_MSG("adc_read_channel(): channel %d, ADC_ERR_FLAG__READ_ERR", channel, 0, 0);
     error_return = ADC_ERR_FLAG__READ_ERR;
  }
  else
  {
     /* Get an ADC reading, directly or from the buffer */
     switch (channel_definition->logical_channel_type)
     {
     case BUFFERED:
       if (adc_read_add_to_schedule(channel) == IDLE)
       {
         adc_completion_cb_fn = adc_read_complete;
         /* start the next read */
         adc_read_error = channel_definition->adc_read_fn(&last_adc_reading[channel], adc_completion_cb_fn);

         if (adc_read_error != ADC_ERR_FLAG__SUCCESS)
         {
           ADC_DEBUG_MSG("adc_read_channel(): BUFFERED : channel %d, ADC_ERR_FLAG__READ_ERR", channel, 0, 0);
           error_return = ADC_ERR_FLAG__READ_ERR;
         }
       }
       /* else the ADC scheduler was already running */
       else
       {
         ADC_DEBUG_MSG("adc_read_channel(): adc_read sched busy channel %d", channel, 0, 0);
         /* Set up a timeout in case no response is received */
         clk_dereg(&adc_read_timeout_cb);
         clk_reg(&adc_read_timeout_cb,       /* pointer to the call back struct */
                   adc_read_timeout_callback,  /* address of the func to callback */
                   (int4)ADC_READ_TIMEOUT,     /* delay till first call back      */
                   (int4)0,                    /* delay between repeat call backs */
                   FALSE );                    /* do not repeat call backs        */
       }
       break;

     case IMMEDIATE:
       if (adc_read_add_to_schedule(channel) == IDLE)
       {
         /* start the read */
         adc_read_error = channel_definition->adc_read_fn(&last_adc_reading[channel], adc_read_complete);

         if (adc_read_error != ADC_ERR_FLAG__SUCCESS)
         {
           ADC_DEBUG_MSG("adc_read_channel(): IMMEDIATE : channel %d, ADC_ERR_FLAG__NOT_READY", channel, 0, 0);
           error_return = ADC_ERR_FLAG__READ_ERR;
         }

         /* The read function should have completed and been removed from the
            scheduler. If not then there is no new reading available. */
         if (adc_get_schedule_state() == RUNNING)
         {
           ADC_DEBUG_MSG("adc_read_channel(): IMMEDIATE : channel %d, ADC_ERR_FLAG__NOT_READY", channel, 0, 0);
           error_return = ADC_ERR_FLAG__NOT_READY;
         }
       }
       else /* the ADC scheduler was already running, an IMMEDIATE read is impossible */
       {
         ADC_DEBUG_MSG("adc_read_channel(): IMMEDIATE : channel %d, ADC_ERR_FLAG__NOT_READY", channel, 0, 0);
         error_return = ADC_ERR_FLAG__NOT_READY;
       }
       break;

     default:
       MSG_ERROR("Invalid ADC read type%d",channel_definition->logical_channel_type,0,0);
       break;
     }
  } //if (adc_blocked_status[channel]) ...

  adc_reading = last_adc_reading[channel];

  if (adc_reading != NO_READING_IN_BUFFER)
  {
    /* Range check the reading */
    if (adc_reading >= channel_definition->min_reading
      && adc_reading <= channel_definition->max_reading)
    {
      /* Filter the reading */
      channel_result[channel] = channel_definition->filter_fn(channel_result[channel], adc_reading);
    }
    else /* the reading is out of range */
    {
      ADC_DEBUG_MSG("adc_read_channel(): Range err %d",channel_definition->logical_channel_type,0,0);
      error_return = ADC_ERR_FLAG__RANGE_ERR;
    }
  }
  else /* adc_reading == NO_READING_IN_BUFFER */
  {
    ADC_DEBUG_MSG("adc_read_channel(): ADC_ERR_FLAG__NOT_READY (no reading)", 0, 0, 0);
    error_return = ADC_ERR_FLAG__NOT_READY;
  }

  if (channel_result[channel] == NO_READING_IN_BUFFER)
  {/* use default result */
    ADC_DEBUG_MSG("adc_read_channel(): using default reading (%d) for channel %d (after filter_fn)", channel_definition->default_result, channel, 0);
    *result = *(adc_result_type*) &channel_definition->default_result;
  }
  else /* it's a valid reading */
  {
  /* Scale the reading */
  /* Convert the reading to the appropriate result type */
    *result = channel_definition->conversion_fn(
      channel_result[channel],
      channel_definition->offset_result,
      channel_definition->result_vs_reading_slope_x_10000);
    ADC_DEBUG_MSG("adc_read_channel():done adc read, read %d", (adc_raw_reading) result->raw, 0, 0);
  
  }
  return error_return;
}


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
void adc_block_channel(adc_logical_channel_type channel)
{
   ADC_DEBUG_MSG("adc_block_channel(): chan %d blocked", channel, 0, 0);
   adc_set_blocking_status(channel, TRUE);
}


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
void adc_unblock_channel(adc_logical_channel_type channel)
{
   ADC_DEBUG_MSG("adc_unblock_channel(): chan %d unblocked", channel, 0, 0);
   adc_set_blocking_status(channel, FALSE);
}


/*===========================================================================

FUNCTION adc_read_timeout_callback                          INTERNAL FUNCTION

DESCRIPTION
  Callback fn called if read response timer expires
  
PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void adc_read_timeout_callback(int4 ms_interval_unused)
{
  /*lint -esym(715,ms_interval_unused)
  ** Have lint not complain about the ignored parameter which is
  ** specified to make this routine match the template for Clock.
  */
  /* The adc_response never came back, end this read cycle */
    MSG_ERROR("ADC read timeout",0,0,0);

    pm_adc_sm_adie_reset();
    adc_read_complete();
}

/*===========================================================================

FUNCTION adc_process_scheduler_callback                    INTERNAL FUNCTION

DESCRIPTION
  Callback fn called to process next queued adc read
  
PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  None.
  
SIDE EFFECTS
  ADC read is executed
===========================================================================*/
LOCAL void adc_process_scheduler_callback(int4 ms_interval_unused)
{
  int schedule_index;
  adc_logical_channel_type channel;
  adc_logical_channel_type logical_channel_number;
  
  ASSERT(items_in_schedule > 0 );

  INTLOCK();
  
  /* channel == logical_channel_number unless there is a problem */
  channel = adc_schedule[0];
  logical_channel_number = adc_logical_channel_definitions[channel].logical_channel_number;

  /* Shuffle the queue up one, overwriting the head */
  for (schedule_index = 0; schedule_index < items_in_schedule; schedule_index++)
  {
    adc_schedule[schedule_index] = adc_schedule[schedule_index+1];
  }
  items_in_schedule--;  

  INTFREE();

  (void)adc_logical_channel_definitions[channel].adc_read_fn(&last_adc_reading[logical_channel_number], adc_read_complete);

}

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
void adc_reset_filter(adc_logical_channel_type channel)
{
  ADC_DEBUG_MSG("adc_reset_filter(): reset filter for channel %d",channel,0,0);

  if (channel >= NUMBER_OF_LOGICAL_ADC_CHANNELS)
  {
    MSG_ERROR("Invalid channel %d",channel,0,0);
    return;
  }
  channel_result[channel] = NO_READING_IN_BUFFER;
}



#ifdef FEATURE_RF_TX_PWR_LIM_REV

/*===========================================================================

FUNCTION adc_check_arm_reserved_adc                           EXTERNAL FUNCTION

DESCRIPTION
  Checks if ARM has reserved the ADC

PARAMETERS
  None

RETURN VALUE
  TRUE - ARM has reserved it
  FALSE - ARM has not reserved it

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean adc_check_arm_reserved_adc()
{
  mdsp_peek((uint16*)MDSP_adcBusyFlagFromArm,&adcBusyFlagFromArm,MDSP_adcBusyFlagFromArm_SIZE/8);
  return adcBusyFlagFromArm;
}

/*===========================================================================

FUNCTION adc_check_mdsp_reserved_adc                         EXTERNAL FUNCTION

DESCRIPTION
  Checks if MDSP has reserved the ADC

PARAMETERS
  None

RETURN VALUE
  TRUE - MDSP has reserved it
  FALSE - MDSP has not reserved it

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean adc_check_mdsp_reserved_adc()
{
  mdsp_peek((uint16*)MDSP_hdetConvStartedFlag,&hdetConvStartedFlag,MDSP_hdetConvStartedFlag_SIZE/8);
  return hdetConvStartedFlag;
}

/*===========================================================================

FUNCTION adc_set_adc_reserved_adc                           EXTERNAL FUNCTION

DESCRIPTION
  Sets adcBusyFlagFromArm

PARAMETERS
  boolean enable - value to set flag to

RETURN VALUE
  None

DEPENDENCIES
  None.

SIDE EFFECTS
  Sets MDSP_hdetConvStartedFlag and adcBusyFlagFromArm
===========================================================================*/
boolean adc_set_arm_reserved_adc(boolean enable)
{
  adcBusyFlagFromArm=enable;
  mdsp_poke(&adcBusyFlagFromArm,(uint16*)MDSP_adcBusyFlagFromArm,MDSP_adcBusyFlagFromArm_SIZE/8);
  ADC_DEBUG_MSG("adc_set_arm_reserved_adc(): enable? %d", enable, 0, 0);
  return adcBusyFlagFromArm;
}

/*===========================================================================

FUNCTION adc_set_mdsp_reserved_adc                           EXTERNAL FUNCTION

DESCRIPTION
  Sets hdetConvStartedFlag 

PARAMETERS
  boolean enable - value to set flag to

RETURN VALUE
  None

DEPENDENCIES
  None.

SIDE EFFECTS
  Sets MDSP_hdetConvStartedFlag and hdetConvStartedFlag
===========================================================================*/
boolean adc_set_mdsp_reserved_adc(boolean enable)
{
  hdetConvStartedFlag=enable;
  mdsp_poke(&hdetConvStartedFlag,(uint16*)MDSP_hdetConvStartedFlag,MDSP_hdetConvStartedFlag_SIZE/8);
  ADC_DEBUG_MSG("adc_set_mdsp_reserved_adc(): enable? %d", enable, 0, 0);
  return hdetConvStartedFlag;
}



/*===========================================================================

FUNCTION adc_block_out_mdsp                           INTERNAL FUNCTION

DESCRIPTION
  Attempt to reserve ADC for ARM

PARAMETERS
  None

RETURN VALUE
  TRUE - Was successful
  FALSE - MDSP has reserved it

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL boolean adc_block_out_mdsp()
{
  ADC_DEBUG_MSG("adc_block_out_mdsp()", 0, 0, 0);
  if (adc_check_mdsp_reserved_adc()==TRUE) 
  {
    ADC_DEBUG_MSG("adc_block_out_mdsp(): FALSE", 0, 0, 0);
    return FALSE;
  }
  else
  {
    //set flag to tell MDSP we are busy
    adc_set_arm_reserved_adc(TRUE);
    
    //wait a few cycles in case race condition between MDSP reading status
    clk_busy_wait(5);
    
    if (adc_check_mdsp_reserved_adc()==TRUE) 
    {
      //relinguish control of ADC
      adc_set_arm_reserved_adc(FALSE);
      ADC_DEBUG_MSG("adc_block_out_mdsp(): FALSE", 0, 0, 0);
      return FALSE;
    }
    else
    {
      return TRUE;
    }
  }
}

/*===========================================================================

FUNCTION adc_reverse_control_to_arm                           EXTERNAL FUNCTION

DESCRIPTION
  Allow ARM to have control of ADC and removes control from MDSP

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void adc_reverse_control_to_arm()
{
  ADC_DEBUG_MSG("adc_reverse_control_to_arm()", 0, 0, 0);
  //set flag to tell MDSP ADC is no longer free
  (void)adc_set_arm_reserved_adc(TRUE);

  //give some time for MDSP to see ARM has control
  clk_busy_wait(5);
  
  //take away MDSP control
  (void)adc_set_mdsp_reserved_adc(FALSE);
}
#endif /* FEATURE_RF_TX_PWR_LIM_REV */

/*===========================================================================

FUNCTION adc_read_add_to_schedule                           INTERNAL FUNCTION

DESCRIPTION
  This is a simple scheduler that makes sure one ADC reading is complete
  before another is started. It's not quite a simple queue - if the ADC input
  is already scheduled then it is not added again.

PARAMETERS
  adc_logical_channel_type channel

RETURN VALUE
  adc_schedule_state_type
    IDLE        The scheduler was idle (a new ADC reading should be started).
    RUNNING     The scheduler was running

DEPENDENCIES
  None.

SIDE EFFECTS
  Interrupts are disabled briefly while processing the scheduler.
===========================================================================*/
LOCAL adc_schedule_state_type adc_read_add_to_schedule(adc_logical_channel_type channel)
{
  adc_schedule_state_type result = RUNNING;
  int schedule_index;
  dword s;
  
  #ifdef FEATURE_RF_TX_PWR_LIM_REV
  boolean arm_has_adc;
  #endif
  
  ADC_DEBUG_MSG("adc_read_add_to_schedule() %d", channel, 0, 0);
  INTLOCK_SAV(s);

  #ifdef FEATURE_RF_TX_PWR_LIM_REV
  arm_has_adc = adc_block_out_mdsp();

  if (adc_schedule_state == IDLE && arm_has_adc == TRUE)
  #else /* FEATURE_RF_TX_PWR_LIM_REV */
  if (adc_schedule_state == IDLE)
  #endif /* FEATURE_RF_TX_PWR_LIM_REV */
  {
    /* does not need to add to queue, return IDLE and let adc read begin immediately */
    #ifdef FEATURE_ADC_VOTE_FOR_SLEEP
    ADC_SLEEP_FORBID();
    #endif /* FEATURE_ADC_VOTE_FOR_SLEEP */

    result = IDLE;
  }
  else 
  {
    /*TODO should we put immediate read in queue?*/
    for (schedule_index = 0; schedule_index < items_in_schedule; schedule_index++)
    {
      if (adc_logical_channel_definitions[adc_schedule[schedule_index]].logical_channel_number
          == adc_logical_channel_definitions[channel].logical_channel_number)
        break;  /* the ADC input is already scheduled for a read, don't add it again */
    }

    if (schedule_index == items_in_schedule)
    { /* The channel is not in the schedule, add it to the end */
      adc_schedule[items_in_schedule++] = channel;
    }

    ADC_DEBUG_MSG("adc_read_add_to_schedule(): adc queue size %d",items_in_schedule,0,0);
    result = RUNNING;
  }
  
  /* As soon as the adc_read_add_to_schedule_state() is called, ADC should be considered 
   * busy by default 
   */
  adc_set_schedule_state(RUNNING);

  INTFREE_SAV(s);
  
  return result;
}


/*===========================================================================

FUNCTION adc_read_complete                                  EXTERNAL FUNCTION

DESCRIPTION
  This is the second half of the scheduler, called via a call-back when the
  ADC read process is complete. If there is another ADC read scheduled it
  starts it off.

PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  A read of the ADC may be invoked.
  Interrupts are disabled briefly while processing the scheduler.
===========================================================================*/

void adc_read_complete(void)
{

  clk_dereg(&adc_read_timeout_cb);

  ADC_DEBUG_MSG("adc_read_complete()", 0, 0, 0);
  
  INTLOCK();
  if (items_in_schedule == 0)
  {
    adc_schedule_state = IDLE;

    #ifdef FEATURE_RF_TX_PWR_LIM_REV
    if(adc_check_mdsp_reserved_adc()==TRUE)
    {
      ERR_FATAL("MDSP reserved ADC",0,0,0);
    }
    (void)adc_set_arm_reserved_adc(FALSE);
    #endif /* FEATURE_RF_TX_PWR_LIM_REV */

    #ifdef FEATURE_ADC_VOTE_FOR_SLEEP
    ADC_SLEEP_ALLOW();
    #endif /* FEATURE_ADC_VOTE_FOR_SLEEP */
  }
  else
  {
    /* there must be more than one item in the queue,
     * register a call back to process them later */

    clk_reg(&adc_process_scheduler_cb,     /* pointer to the call back struct */
           adc_process_scheduler_callback, /* address of the func to callback */
           (int4)ADC_SCHEDULER_INTERVAL,   /* delay till first call back      */
           (int4)0,                        /* delay between repeat call backs */
           FALSE );                        /* do not repeat call backs        */
    
  }
  INTFREE();
}

/* Generic filter functions */

/*===========================================================================

FUNCTION adc_inc_dec_filter                                 EXTERNAL FUNCTION

DESCRIPTION

PARAMETERS
  int last_reading          The current filtered value
  int new_reading           Value read from ADC

RETURN VALUE
  int                       The new filtered value

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int adc_inc_dec_filter (int last_reading, int new_reading)
{
  if (last_reading != NO_READING_IN_BUFFER)
  {
    if (new_reading > last_reading)
      last_reading++;
    else if (new_reading < last_reading)
      last_reading--;
    /* else reading == last_reading */
  }
  else /* first value */
  {
    last_reading = new_reading;
  }
  return last_reading;
}

/*===========================================================================

FUNCTION adc_null_filter                                    EXTERNAL FUNCTION

DESCRIPTION

PARAMETERS
  int last_reading          The current filtered value
  int new_reading           Value read from ADC

RETURN VALUE
  int                       The new filtered value

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int adc_null_filter(int last_reading, int new_reading)
{
  /* parameter not used */
  /*lint -esym(715,last_reading)
  ** Have lint not complain about the ignored parameter which is
  ** specified to make this routine match the template.
  */
  return new_reading;
}

/* Generic type conversion functions */

/*===========================================================================

FUNCTION adc_to_mV                                          EXTERNAL FUNCTION

DESCRIPTION

PARAMETERS
  int reading               Value read from ADC
  int full_scale_reading    Maximum value that can be read from ADC
  int full_scale_result     Result corresponding to full_scale_reading

RETURN VALUE
  adc_result_type.mV        The ADC reading converted to mV

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
adc_result_type adc_to_mV(int reading, int32 offset, int32 slope_x_10000)
{
  adc_result_type result;
  result.mV = ((adc_as_mV) ((reading * slope_x_10000)/10000) + offset);
  return (result);
}

/*===========================================================================

FUNCTION adc_to_percentage                                  EXTERNAL FUNCTION

DESCRIPTION

PARAMETERS
  int reading               Value read from ADC
  int full_scale_reading    Maximum value that can be read from ADC
  int full_scale_result     Result corresponding to full_scale_reading

RETURN VALUE
  adc_result_type.percentage The ADC reading converted to a percentage
                            of the full scale reading

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
adc_result_type adc_to_percentage(int reading, int32 offset, int32 slope_x_10000)
{
  adc_result_type result;
  result.percentage = ((adc_as_percentage) ((reading * slope_x_10000)/10000) + offset);
  return (result);
}

/*===========================================================================

FUNCTION adc_null_conversion                                EXTERNAL FUNCTION

DESCRIPTION

PARAMETERS
  int reading               Value read from ADC
  int full_scale_reading    Maximum value that can be read from ADC
  int full_scale_result     Result corresponding to full_scale_reading

RETURN VALUE
  adc_result_type.raw       The unmodified ADC reading

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
adc_result_type adc_null_conversion(int reading, int32 offset, int32 slope_x_10000)
{
  adc_result_type result;

  /* parameter not used */
  /*lint -esym(715,offset)*/
  /*lint -esym(715,slope_x_10000)
  ** Have lint not complain about the ignored parameter which is
  ** specified to make this routine match the template.
  */
  result.raw = (adc_raw_reading)reading;
  return (result);
}


/*===========================================================================
FUNCTION  adc_set_blocking_status                             LOCAL FUNCTION

DESCRIPTION
  This fn is used for setting the blocked status of one or all ADC
  logical channels.
  
PARAMETERS
  adc_logical_channel_type channel
    - Specifies the logical channel to be blocked/unblocked. A special value of
      NUMBER_OF_LOGICAL_ADC_CHANNELS may be used for indicating all available
      logical channels in one shot.
  boolean status
    - TRUE means block
    - FALSE means unblock

RETURN VALUE
  None.
   
DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void adc_set_blocking_status(adc_logical_channel_type channel, boolean status)
{
   uint8 ch_index = 0;

   if (NUMBER_OF_LOGICAL_ADC_CHANNELS == channel)
   {
      for (ch_index = 0; ch_index < NUMBER_OF_LOGICAL_ADC_CHANNELS; ++ ch_index)
      {
         adc_blocked_status[ch_index] = status;
      }
   }
   else if (channel < NUMBER_OF_LOGICAL_ADC_CHANNELS)
   {
      adc_blocked_status[channel] = status;
   }
}

/*===========================================================================
FUNCTION  adc_get_schedule_state                              LOCAL FUNCTION

DESCRIPTION
  This fn is used to get the adc scheduler state
  
PARAMETERS
  None.

RETURN VALUE
  adc_schedule_state_type    The state of the adc scheduler
                                 IDLE - there are no adc reads queued
                                 RUNNING - there are adc reads in progress
   
DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static adc_schedule_state_type adc_get_schedule_state(void)
{
  return adc_schedule_state;
}

/*===========================================================================
FUNCTION  adc_set_schedule_state                              LOCAL FUNCTION

DESCRIPTION
  This fn is used to set the adc scheduler state
  
PARAMETERS
  adc_schedule_state_type    
    - The new state of the adc scheduler 
          IDLE - there are now no more adc reads queued
          RUNNING - there are going to be adc reads in progress

RETURN VALUE
  None.
   
DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void adc_set_schedule_state(adc_schedule_state_type new_schedule_state)
{
  adc_schedule_state = new_schedule_state;
}
