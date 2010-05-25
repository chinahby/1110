/*===========================================================================


   R F  D r i v e r  M u l t i m o d e  E x t e r n a l  I n t e r f a c e 


DESCRIPTION
  This file contains all declarations and definitions necessary to use
  the RF drivers.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The following function must be called before calling any other
  functions provided by this driver:

    1. rfm_init()

  This places the RF driver in the PARKED/SLEEP state.  Then a specific
  driver mode (i.e. CDMA 1x, HDR, WCDMA, GSM, GPS, PARKED, etc.) can be
  entered by calling the following functions in the sequence given:

    2. rfm_enter_mode()
    3. rfm_tune_to_chan()
    4. rfm_start_loops()

  The RF driver is now in the Idle/Rx state in the mode entered.
  
Copyright (c) 1999 - 2002, 2006, 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/rf/common/main/qsc1100/inc/rf/rfm.c#1 $ 
$DateTime: 2008/04/11 05:11:57 $ 
$Author: ppottier $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/12/08   vm      Added Support for supporting Single chain on QSC1100
12/07/07   jfc     Add update_pavmode_burst_data() API for GL1
11/14/07   jfc     Export API for getting switchpoints
09/21/07   jfc     Compile rfm_get_rf_dbg_scr_param() for GSM and WCDMA
08/21/07   adk     Fixed compiler warning
08/13/07   jfc     Pass call back handler to rfapi layer when initializing 
                    RFCOM_RECEIVER_DIV in rfm_enter_mode()
08/09/07   ad      Added support for reading HS LNA switch point NVs during HS calls.
08/06/07   jfc     Featurized code for EDGE-only builds
07/12/07   jfc/lcl Added lna_range param for set_rx_gain()
07/11/07   ra      update rf_path_state when entering GPS
04/12/07   ad      Added RFCOM_RECEIVER_DIV case to rfm_enter_mode
04/12/07   jfc     Changed api pointer to direct logical layer call
02/27/07   jfc     Removed RFM_DEBUG and cleaned up rfm_ioctl()
02/16/07   jfc     Fixed access to uninitialized rfapi_dev0_ptr
o2/15/07   go      Added stub function rfm_init_bbrx_sleep
02/12/07   jfc     Added channel type parameter to set_tx_general_ctl()
02/05/07   ycl     Merge to 7200 tip.
01/30/07   jfc     Added rfm_get_rf_warmup_time() and rfm_band_supports_rxd()
01/23/07   ra      Added rfm_trigger_gps_bbc_update()
01/17/07   jfc     Replaced T_MSM6280 with RF_HAS_FEATURE_WCDMA
01/11/06   ra      Added RFM_EXIT_GPS 
01/09/07   jfc     Fixed compiler warnings
01/09/07   jfc     Changed RF_HAS_CDMA1X to RF_HAS_FEATURE_CDMA1X
01/08/07   jfc     Added RECEIVER_DIV case to rfm_is_sleeping
12/18/06   jfc     Moved rfm_is_sleeping to RFM layer
12/16/06   jfc     Added API for antenna for TRM layer
11/16/06   jfc     Changed rfm_get_band_mask() to take device type
11/15/06   jfc     Removed #ifdef RF_HAS_DM_SUPPORT from around 
                    rfm_enable_data_mover_clk_for_cm()
11/07/06   ra      Added support for GPS entry in rfm_enter_mode
10/31/06   ycl     Added functions for rfm_get_mdsp_tx_power_limit() and 
                   rfm_get_mdsp_tx_agc().
06/01/06   go      Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "rex.h"
#include "rficap.h"
#include "rfcom.h"
#include "rfapi.h"
#include "msg.h"
#include "rf.h"

#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM              /* GSM specific information */
#error code not present
#endif  /* FEATURE_GSM */

/* For DEBUG builds only */
//#define RFM_DEBUG

#ifdef RFM_DEBUG
#error code not present
#else
#define RFM_DEBUG_EN_Q(...)
#endif /* RFM_DEBUG */



/*---------------------------------------------------------------------------
  Saves path configuration states.
---------------------------------------------------------------------------*/
rf_path_state_type rf_path_state = RF_DISABLED_ST;
int rf_init_state, rf_init_state_1, rf_init_state_2, rf_init_state_3;
boolean rfm_initialized = FALSE;
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION  RFM_INIT 

DESCRIPTION
  Initializes the primary radio transceiver device.  Reads common and mode
  specific RF NV parameters from NV and initializes common and mode
  specific NV tables.  Initializes common hardware and mode specific RF
  hardware for each mode supported (WCDMA, GSM, CDMA, HDR, GPS, etc.) to
  a low power parked/sleep state.  RF driver is placed in parked/sleep state.
  It returns a state information of whether NV is iniatialized or not.
  If RF NV is initalized it returns non-zero.

DEPENDENCIES
  This function must be called before any other RF driver function.
  May require special parked mode MDSP image to be loaded before calling
  this function (TBD).

RETURN VALUE
  Returns TRUE if initialization is successful, otherwise returns FALSE.
  
SIDE EFFECTS
  None

===========================================================================*/
boolean rfm_init
(
#ifdef FEATURE_MULTI_RX_CHAIN 
  rfcom_device_enum_type device,        /* RF Multimode device */
#endif
  rex_tcb_type *caller_tcb_ptr,         /* Calling task's TCB pointer */
  rex_sigs_type task_nv_wait_sig,       /* Task signal to wait for when
                                           reading NV */
  void (*task_wait_func_ptr)( rex_sigs_type )
                                        /* Task's wait function to be called
                                           when reading NV */
)
{

  #ifdef RFM_DEBUG
#error code not present
  #endif

  RFM_DEBUG_EN_Q(&rfm_init, 0, 0, 0, 0);

  if (rfm_initialized==FALSE)
  {

    rfapi_dev_ptr[RFCOM_TRANSCEIVER_0] = rfapi_init( RFCOM_TRANSCEIVER_0,
                                                     caller_tcb_ptr,
                                                     task_nv_wait_sig,
                                                     task_wait_func_ptr,
                                                     &rf_init_state );

    #ifdef FEATURE_MULTI_RX_CHAIN     
    rfapi_dev_ptr[RFCOM_RECEIVER_DIV] = rfapi_init( RFCOM_RECEIVER_DIV,
                                                    caller_tcb_ptr,
                                                    task_nv_wait_sig,
                                                    task_wait_func_ptr,
                                                    &rf_init_state_2 );

    /* TRM uses RECEIVER_1 for second chain. UMTS usually refers to second chain
        as RECEIVER_DIV. Since either RECEIVER_DIV or RECEIVER_1 resolve to
        chain 1, we should initialize RECEIVER_1 here to satisfy TRM */
    rfapi_dev_ptr[RFCOM_RECEIVER_1] = rfapi_init( RFCOM_RECEIVER_1,
                                                  caller_tcb_ptr,
                                                  task_nv_wait_sig,
                                                  task_wait_func_ptr,
                                                  &rf_init_state_1 );

    #ifdef RF_HAS_FEATURE_CDMA1X
    rfapi_dev_ptr[RFCOM_RECEIVER_DUAL] = rfapi_init( RFCOM_RECEIVER_DUAL,
                                                     caller_tcb_ptr,
                                                     task_nv_wait_sig,
                                                     task_wait_func_ptr,
                                                     &rf_init_state_3 );
    #endif /* RF_HAS_FEATURE_CDMA1X */
    #endif/*FEATURE_MULTI_RX_CHAIN*/
    rfm_initialized = TRUE;
  }
  
  return ( (rf_init_state != 0) ? TRUE : FALSE );
}



/*===========================================================================

FUNCTION  RFM_ENTER_MODE

DESCRIPTION
  Initializes RF hardware for mode of operation specified by parameter
  rf_mode (WCDMA, GSM, CDMA, HDR, GPS, PARKED/SLEEP, etc.).  Places RF
  driver in Rx state of operation in mode of operation specified, except
  when mode specified is PARKED/SLEEP.  Typically, control loops are not
  started by this function and must be started by calling
  rfm_start_loops().  Control loops are typically in override mode,
  outputting only the override value.  The actual state of the control
  loops after calling this function are mode dependent.

  Callback function is called when RF hardware has settled and is ready
  for use.  For support of multiple events occurring during RF hardware
  settling time, event parameter is passed into callback, as well as a
  pointer to user data that was passed in by caller.  Note: Callback
  executes in interrupt context.

DEPENDENCIES
  rfm_init() must have already been called.  
  MDSP must be downloaded and initialized for mode of operation specified
  by mode before calling this function.
  
RETURN VALUE
  Returns total delay in microseconds this function will wait for RF
  hardware to settle.

SIDE EFFECTS
  None

===========================================================================*/
uint32 rfm_enter_mode
(
  #ifdef FEATURE_MULTI_RX_CHAIN
  rfcom_device_enum_type device,        /* RF Multimode device */
  #endif

  rfcom_mode_enum_type rf_mode,         /* RF mode */
  const void *user_data_ptr,            /* Pointer to user data passed into
                                           callback */
  rfm_cb_handler_type cb_handler        /* Call back handler */
)
{
  uint32 res = 0;

  RFM_DEBUG_EN_Q(&rfm_enter_mode, device, rf_mode, &user_data_ptr, &cb_handler);

  #ifndef FEATURE_MULTI_RX_CHAIN  
  if (rf_mode == RFCOM_1X_MODE)
  {  
    res = rfapi_enter_mode( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0], rf_mode, user_data_ptr,
                            (rfcom_cb_handler_type) cb_handler ); 
  }
  #else
  switch (device) 
  {
  case RFCOM_RECEIVER_DIV:
    res = rfapi_enter_mode( rfapi_dev_ptr[RFCOM_RECEIVER_DIV], rf_mode, user_data_ptr,
                            (rfcom_cb_handler_type) cb_handler ); 
    break;

  case RFCOM_TRANSCEIVER_0:
  default:
    res = rfapi_enter_mode( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0], rf_mode, user_data_ptr,
                            (rfcom_cb_handler_type) cb_handler ); 
    
    if (rf_mode == RFCOM_1X_MODE)
    {
      (void) rfapi_enter_mode( rfapi_dev_ptr[RFCOM_RECEIVER_1], rf_mode, NULL, NULL ); 
    
      (void) rfapi_enter_mode( rfapi_dev_ptr[RFCOM_RECEIVER_DIV], rf_mode, NULL, NULL ); 
    
      (void) rfapi_enter_mode( rfapi_dev_ptr[RFCOM_RECEIVER_DUAL], rf_mode, NULL, NULL ); 
    
    }
    #if defined (RF_HAS_GPS)
#error code not present
    #endif /* RF_HAS_GPS*/
    break;
  }
  #endif/*FEATURE_MULTI_RX_CHAIN */
  return res;

}

#if defined (RF_HAS_CGPS)
#error code not present
#endif /* RF_HAS_CGPS */

/*===========================================================================

FUNCTION  RFM_TUNE_TO_CHAN

DESCRIPTION
  Tunes the UHF Rx/Tx synthesizers to the channel specified.  The channel
  is interpreted based on operating mode.  The input parameter synths
  specifies whether transmit, receive, or both synthesizers should be
  tuned to the specified channel.

  Callback function is called when tuning is complete and synthesizers have
  locked.  For support of multiple events occurring during synthesizer lock
  time, event parameter is passed into callback, as well as a pointer to
  user data that was passed in by caller.  Note: Callback executes in
  interrupt context.

DEPENDENCIES
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.  
  
RETURN VALUE
  Returns total delay in microseconds this function will wait for
  synthesizers to lock.

SIDE EFFECTS
  None

===========================================================================*/
uint32 rfm_tune_to_chan
(
  #ifdef FEATURE_MULTI_RF_CHAIN_API
  rfcom_device_enum_type device,        /* RF Multimode device */
  #endif /* FEATURE_MULTI_RX_CHAIN */
  uint16 channel,                       /* Mode dependent channel */
  rfm_synth_tune_enum_type synths,      /* Synthesizor(s) to tune */
  const void *user_data_ptr,            /* Pointer to user data passed into
                                           callback */
  rfm_cb_handler_type cb_handler        /* Call back handler */
)
{

  RFM_DEBUG_EN_Q(&rfm_tune_to_chan, 0, channel, synths, &user_data_ptr);

  #ifdef FEATURE_MULTI_RX_CHAIN_API
  return rfapi_tune_to_chan( rfapi_dev_ptr[device], 
                             channel, 
                             (rfcom_synth_tune_enum_type) synths, 
                             user_data_ptr,
                             (rfcom_cb_handler_type) cb_handler );
  #else
  return rfapi_tune_to_chan( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0], 
                             channel, 
                             (rfcom_synth_tune_enum_type) synths, 
                             user_data_ptr,
                             (rfcom_cb_handler_type) cb_handler );
  #endif /* FEATURE_MULTI_RX_CHAIN */
}



/*===========================================================================

FUNCTION  RFM_ENABLE_TX

DESCRIPTION
  Enables transmitter for mode of operation RF driver is currently
  operating in, typically preparing transmitter for access attempts.
  Turns on LDOs for transmit hardware.  Turns on PA.  Places RF driver in
  Rx/Tx state of operation.

  Callback function is called when RF hardware has settled and is ready
  for use. For support of multiple events occurring during RF hardware
  settling time, event parameter is passed into callback, as well as a
  pointer to user data that was passed in by caller.  Note: Callback
  executes in interrupt context. 

DEPENDENCIES
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.
  rfm_tune_to_chan() should have already been called.
  
RETURN VALUE
  Returns total delay in microseconds this function will wait for RF
  hardware to settle.

SIDE EFFECTS
  None

===========================================================================*/
uint32 rfm_enable_tx
(
  #ifdef FEATURE_MULTI_RX_CHAIN //TODO_7600
  rfcom_device_enum_type device,
  #endif
  const void *user_data_ptr,            /* Pointer to user data passed into
                                           callback */
  rfm_cb_handler_type cb_handler        /* Callback handler */
)
{
  RFM_DEBUG_EN_Q(&rfm_enable_tx, 0, 0, 0, 0);

  #ifdef FEATURE_MULTI_RX_CHAIN //TODO_7600
  return rfapi_enable_tx(  rfapi_dev_ptr[device], user_data_ptr,
                          (rfcom_cb_handler_type) cb_handler );
  #else
  return rfapi_enable_tx( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0], user_data_ptr,
                          (rfcom_cb_handler_type) cb_handler );
  #endif
}


/*===========================================================================

FUNCTION  RFM_DISABLE_TX

DESCRIPTION
  Disables transmitter for mode of operation RF driver is currently
  operating in.  Turns off LDOs for transmit hardware.  Turns off PA.
  Places RF driver in Rx state of operation.

DEPENDENCIES
  rfm_enable_tx() should have already been called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfm_disable_tx
(
  #ifdef FEATURE_MULTI_RX_CHAIN //TODO_7600
  rfcom_device_enum_type device
  #endif
)
{
  RFM_DEBUG_EN_Q(&rfm_disable_tx, 0, 0, 0, 0);

  #ifdef FEATURE_MULTI_RX_CHAIN //TODO_7600
  rfapi_disable_tx(  rfapi_dev_ptr[device] );
  #else
  rfapi_disable_tx( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0]);
  #endif   
}


/*===========================================================================

FUNCTION  RFM_START_TX

DESCRIPTION
  Enables closed power control loops to control transmit power while on
  dedicated traffic channel.  Enables this only if current operating mode
  requires this.  Turns on PA.  May enable other mode specific functionality
  necessary for transmit operation while on the dedicated traffic channel.
  Can be used to enable Tx after hard handoff.

DEPENDENCIES
  rfm_enable_tx() must have already been called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfm_start_tx
(
#ifdef FEATURE_MULTI_RX_CHAIN //TODO_7600
rfcom_device_enum_type device
#endif
)
{
  RFM_DEBUG_EN_Q(&rfm_start_tx, 0, 0, 0, 0);

  #ifdef FEATURE_MULTI_RX_CHAIN //TODO_7600
  rfapi_start_tx(  rfapi_dev_ptr[device] );
  #else
  rfapi_start_tx( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0]);
  #endif
}

/*===========================================================================

FUNCTION  RFM_STOP_TX

DESCRIPTION
  Disables closed power control loops or other mode specific functionality
  enabled by rfm_start_tx().  Turns off PA.  Can be used to disable Tx
  during hard handoff.

DEPENDENCIES
  rfm_start_tx() should have already been called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfm_stop_tx
(
#ifdef FEATURE_MULTI_RX_CHAIN //TODO_7600
rfcom_device_enum_type device
#endif
)
{
  RFM_DEBUG_EN_Q(&rfm_stop_tx, 0, 0, 0, 0);

  #ifdef FEATURE_MULTI_RX_CHAIN //TODO_7600
  rfapi_stop_tx(  rfapi_dev_ptr[device] );
  #else
  rfapi_stop_tx( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0]);
  #endif
}

/*===========================================================================

FUNCTION RFM_IS_SLEEPING                                 EXTERNALIZED FUNCTION

DESCRIPTION    Queries whether the RF is sleeping right now.

DEPENDENCIES   None.

RETURN VALUE   True if RF is in sleep state.

SIDE EFFECTS   None.

==========================================================================*/
#ifdef FEATURE_MULTI_RX_CHAIN
boolean rfm_is_sleeping( rfcom_device_enum_type device )
{
  switch (device) {
  case RFCOM_TRANSCEIVER_0:
  case RFCOM_RECEIVER_DIV:
  case RFCOM_RECEIVER_1:
    return rfapi_is_sleeping(rfapi_dev_ptr[device]);
  default:
    break;
  }
  return FALSE;
}
#else
boolean rfm_is_sleeping()
{
  return rfapi_is_sleeping(rfapi_dev_ptr[RFCOM_TRANSCEIVER_0]);
}
#endif

/*===========================================================================

FUNCTION  RFM_SLEEP

DESCRIPTION
  Places the majority of RF hardware in sleep mode for the currently
  enabled mode of operation and begins transition of RF driver to sleep
  state.  Actual state of hardware after calling this function is
  operating mode dependent and may require a call to rfm_stay_asleep()
  later in the sleep timeline to complete the shutdown of all RF hardware
  for sleep.  This is the first stage of the transition to sleep state.

  Callback function is called when RF hardware sleep has completed.  For
  support of multiple events occurring during RF sleep time, event
  parameter is passed into callback, as well as a pointer to user data that
  was passed in by caller.  Note: Callback executes in interrupt context.

DEPENDENCIES
  Driver must be in the Rx state of operation.  
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uint32 rfm_sleep(

  #ifdef FEATURE_MULTI_RX_CHAIN
  rfcom_device_enum_type device,
  #endif
  const void *user_data_ptr,         /* Pointer to user data passed into
                                        callback */
  rfm_cb_handler_type cb_handler     /* Call back handler */

)
{
  RFM_DEBUG_EN_Q(&rfm_sleep, device, 0, 0, 0);

  #ifdef FEATURE_MULTI_RX_CHAIN
  return rfapi_sleep( rfapi_dev_ptr[device], user_data_ptr,
                       (rfcom_cb_handler_type) cb_handler);
  #else
  
  return rfapi_sleep( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0], user_data_ptr,
                       (rfcom_cb_handler_type) cb_handler);
  #endif
}


/*===========================================================================

FUNCTION  RFM_STAY_ASLEEP

DESCRIPTION
  Completes transition of RF hardware to sleep state.  The actual
  functionality performed is mode dependent and is the final stage of
  placing the RF hardware and drivers in sleep state.  Calling this
  function to enter the final sleep state may not be required for some
  modes of operation.  An example of what this function may do is
  tri-state the track low adjust PDM output after TCXO has been shutoff.  

DEPENDENCIES
  rfm_sleep() must have already been called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfm_stay_asleep
(
#ifdef FEATURE_MULTI_RX_CHAIN //TODO_7600
rfcom_device_enum_type device
#endif
)
{
  RFM_DEBUG_EN_Q(&rfm_stay_asleep, 0, 0, 0, 0);

  #ifdef FEATURE_MULTI_RX_CHAIN //TODO_7600
  rfapi_stay_asleep(rfapi_dev_ptr[device] );
  #else
  rfapi_stay_asleep( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0]);
  #endif
}

/*===========================================================================

FUNCTION  RFM_WAKEUP

DESCRIPTION
  Wakes up RF hardware from sleep mode to the mode of operation specified
  by input parameter rf_mode (WCDMA, GSM, CDMA, HDR, GPS, etc.).  Places
  RF driver in Rx state.  Typically, control loops are not started by this
  function and must be started by calling rfm_start_loops().  Control
  loops are typically placed in override mode, outputting the override
  value when loop clocks become active.  The actual state of the control
  loops after calling this function are mode dependent.

  Callback function is called when RF hardware wakeup has completed.  For
  support of multiple events occurring during RF wakeup time, event
  parameter is passed into callback, as well as a pointer to user data that
  was passed in by caller.  Note: Callback executes in interrupt context. 

DEPENDENCIES
  rfm_sleep() must have already been called and rfm_stay_asleep()
  may have been called.

RETURN VALUE
  Returns total delay in microseconds this function will wait for the RF
  hardware to wakeup.

SIDE EFFECTS
  None

===========================================================================*/
uint32 rfm_wakeup
(
  rfcom_device_enum_type device,        /* RF Multimode device */
  rfm_mode_enum_type rf_mode,        /* RF mode */
  const void *user_data_ptr,         /* Pointer to user data passed into
                                        callback */
  rfm_cb_handler_type cb_handler     /* Call back handler */  
)
{
  RFM_DEBUG_EN_Q(&rfm_wakeup, device, rf_mode, 0, 0);

  return rfapi_wakeup(  rfapi_dev_ptr[device],
                        (rfcom_mode_enum_type) rf_mode, 
                        user_data_ptr,
                        (rfcom_cb_handler_type) cb_handler );
} 


/*===========================================================================

FUNCTION  RFM_CAPTURE_LOOP_VALUES

DESCRIPTION
  Captures all tracking loop and PDM data that applies to a particular
  mode and state of operation.  Data is stored in union pointed to by
  data_ptr.  This data should not be accessed by caller. Data storage
  is allocated by the caller to allow the caller, for example, to
  maintain lists of loop values, when scanning inter-frequency or
  inter-RAT neighbor lists.  Loop values captured are restored by
  calling rfm_restore_loop_values().

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfm_capture_loop_values
(
  rfcom_device_enum_type device,        /* RF Multimode device */
  rfm_loop_data_type *data_ptr          /* Pointer to location to store
                                           captured data. */
)
{
  RFM_DEBUG_EN_Q(&rfm_capture_loop_values, device, 0, 0, 0);

  rfapi_capture_loop_values(rfapi_dev_ptr[device], data_ptr );
}

/*===========================================================================

FUNCTION  RFM_RESTORE_LOOP_VALUES

DESCRIPTION
  Restores all tracking loop and PDM data contained in union pointed to
  by data_ptr.  Data restored should have been previously captured with
  a call to rfm_capture_loop_values().  Data is restored based on current
  mode of operation and RF driver state.

DEPENDENCIES
  rfm_capture_loop_values() must have already been called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfm_restore_loop_values
(
  rfcom_device_enum_type device,        /* RF Multimode device */
  rfm_loop_data_type *data_ptr          /* Pointer to data that will be
                                           restored */
)
{
  RFM_DEBUG_EN_Q(&rfm_restore_loop_values, device, 0, 0, 0);

  rfapi_restore_loop_values(rfapi_dev_ptr[device], data_ptr );
}

/*===========================================================================

FUNCTION  RFM_STOP_LOOPS

DESCRIPTION
  Stops and freezes all tracking loops active in the current operating
  mode and RF driver state.  Keeps PDMs at constant value by outputting a
  fixed duty cycle PDM waveform.

DEPENDENCIES
  Tracking loops should be running.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfm_stop_loops(rfcom_device_enum_type device )
{
  RFM_DEBUG_EN_Q(&rfm_stop_loops, device, 0, 0, 0);

  rfapi_stop_loops( rfapi_dev_ptr[device] );
}

/*===========================================================================

FUNCTION  RFM_START_LOOPS

DESCRIPTION
  Enables and unfreezes all tracking loops that are relevant to the
  current operating mode and RF driver state.

DEPENDENCIES
  Tracking loops should be stopped.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfm_start_loops( rfcom_device_enum_type device )
{
  RFM_DEBUG_EN_Q(&rfm_start_loops, device, 0, 0, 0);

  rfapi_start_loops(  rfapi_dev_ptr[device] );
}

#if defined (FEATURE_RF_ENABLE_CM) || defined (FEATURE_RF_ENABLE_GTOW)
/*===========================================================================

FUNCTION  RFM_SETUP_INTERSYSTEM_MEAS

DESCRIPTION
  Sets up RF hardware for an intersystem measurement.  The mode of
  operation specified defines the mode to which the intersystem
  measurement will be made.  This function should be called when a
  measurement of an operating mode different from the current mode
  mode must be made to evaluate the possibility of handoff.
  
  Callback function is called when RF hardware has settled and is ready
  for use.  For support of multiple events occurring during RF hardware
  settling time, event parameter is passed into callback, as well as a
  pointer to user data that was passed in by caller.  Note: Callback
  executes in interrupt context.

DEPENDENCIES
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.  
  
RETURN VALUE
  Returns total delay in microseconds this function will wait for RF
  hardware to settle.

SIDE EFFECTS
  None

===========================================================================*/
uint32 rfm_setup_intersystem_meas
(
  rfcom_device_enum_type device,        /* RF Multimode device */
  rfm_mode_enum_type rf_mode,           /* RF mode of intersystem measurement */
  const void *user_data_ptr,            /* Pointer to user data passed into
                                           callback */
  rfm_cb_handler_type cb_handler        /* Call back handler */  
)
{
  RFM_DEBUG_EN_Q(&rfm_setup_intersystem_meas, device, rf_mode, 0, 0);

  return rfapi_setup_intersystem_meas( rfapi_dev_ptr[device], 
                                       (rfcom_mode_enum_type) rf_mode, 
                                       user_data_ptr,
                                       (rfcom_cb_handler_type) cb_handler );
} 

/*===========================================================================

FUNCTION  RFM_STOP_INTERSYSTEM_MEAS

DESCRIPTION
  Changes the configuration of the RF hardware so that it is no longer
  setup to make an intersystem measurement.  This function is typically
  called upon completion of an intersystem measurement.
  
  Callback function is called when RF hardware has settled and is ready
  for use.  For support of multiple events occurring during RF hardware
  settling time, event parameter is passed into callback, as well as a
  pointer to user data that was passed in by caller.  Note: Callback
  executes in interrupt context. 
        
DEPENDENCIES
  rfm_setup_intersystem_meas() must have already been called.
  
RETURN VALUE
  Returns total delay in microseconds this function will wait for RF
  hardware to settle.

SIDE EFFECTS
  None

===========================================================================*/
uint32 rfm_stop_intersystem_meas
(
  rfcom_device_enum_type device,        /* RF Multimode device */
  const void *user_data_ptr,            /* Pointer to user data passed into
                                           callback */
  rfm_cb_handler_type cb_handler        /* Callback handler */
)
{
  RFM_DEBUG_EN_Q(&rfm_stop_intersystem_meas, device, 0, 0, 0);

  return rfapi_stop_intersystem_meas( rfapi_dev_ptr[device], user_data_ptr,
                                      (rfcom_cb_handler_type) cb_handler );
} 

/*===========================================================================

FUNCTION  RFM_TUNE_TO_INTERSYSTEM_CHAN

DESCRIPTION
  Tunes the UHF Rx synthesizer to the channel specified or configures the
  RF driver in preparation for a tune.  The channel is interpreted based
  on the intersystem measurement operating mode specified.

  Callback function is called when tuning is complete and synthesizer has
  locked or when RF driver configuration in preparation for a tune is
  complete.  For support of multiple events occurring during synthesizer
  lock time, event parameter is passed into callback, as well as a pointer
  to user data that was passed in by caller.  Note: Callback executes in
  interrupt context.

DEPENDENCIES
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.  
  
RETURN VALUE
  Returns total delay in microseconds this function will wait for
  synthesizers to lock if a tune is performed.

SIDE EFFECTS
  None

===========================================================================*/
uint32 rfm_tune_to_intersystem_chan
(
  rfcom_device_enum_type device,        /* RF Multimode device */
  rfm_mode_enum_type rf_mode,           /* RF mode of intersystem measurement */
  uint16 channel,                       /* Mode dependent channel */
  const void *user_data_ptr,            /* Pointer to user data passed into
                                           callback */
  rfm_cb_handler_type cb_handler        /* Call back handler */
)
{
  RFM_DEBUG_EN_Q(&rfm_tune_to_intersystem_chan, device, rf_mode, channel, 0);

  return rfapi_tune_to_intersystem_chan( rfapi_dev_ptr[device], 
                                         (rfcom_mode_enum_type) rf_mode, channel,
                                         user_data_ptr,
                                         (rfcom_cb_handler_type) cb_handler );
} 
#endif /* #if defined (FEATURE_RF_ENABLE_CM) || defined (FEATURE_RF_ENABLE_GTOW) */

/*===========================================================================

FUNCTION  RFM_DO_TX_PWR_LIMIT

DESCRIPTION
  Adjusts the Tx power.  Reads HDET value and adjusts Tx power based
  on HDET value, temperature, and frequency.

DEPENDENCIES
  rfm_init() must have already been called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int32 rfm_do_tx_pwr_limit
(
#ifdef FEATURE_MULTI_RX_CHAIN //TODO_7600
rfcom_device_enum_type device
#endif
)
{
  RFM_DEBUG_EN_Q(&rfm_do_tx_pwr_limit, 0, 0, 0, 0);

  #ifdef FEATURE_MULTI_RX_CHAIN //TODO_7600
  return rfapi_do_tx_pwr_limit(  rfapi_dev_ptr[device] );
  #else
  return rfapi_do_tx_pwr_limit( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0]);
  #endif  
}


/*===========================================================================

FUNCTION  RFM_DO_TEMP_COMP

DESCRIPTION
  Performs temperature compensation.  Adjusts AGC linearizer tables
  based on temperature.  Must be called at 10 second intervals to perform
  temperature compensation.

DEPENDENCIES
  rfm_init() must have already been called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfm_do_temp_comp
(
#ifdef FEATURE_MULTI_RX_CHAIN //TODO_7600
rfcom_device_enum_type device
#endif
)
{
  RFM_DEBUG_EN_Q(&rfm_do_temp_comp, 0, 0, 0, 0);

  #ifdef FEATURE_MULTI_RX_CHAIN //TODO_7600
  rfapi_do_temp_comp(  rfapi_dev_ptr[device] );
  #else
  rfapi_do_temp_comp( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0] );
  #endif
}


/*===========================================================================

FUNCTION  RFM_SET_TX_PWR_LIMIT

DESCRIPTION
  Sets the Tx power limit to value given in units of dBm.

DEPENDENCIES
  rfm_init() must have already been called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfm_set_tx_pwr_limit
(
  #ifdef FEATURE_MULTI_RX_CHAIN //TODO_7600
  rfcom_device_enum_type device,
  #endif
  int16 tx_limit_dbm                    /* Tx power limit in dBm */
  #ifdef FEATURE_WCDMA_MAX_TX_PWR_NV_LIM_FIX
  ,rfcom_txplim_channel_type chan_type
  #endif /* FEATURE_WCDMA_MAX_TX_PWR_NV_LIM_FIX */
)
{
  RFM_DEBUG_EN_Q(&rfm_set_tx_pwr_limit, 0, tx_limit_dbm, 0, 0);
    
  #ifdef FEATURE_WCDMA_MAX_TX_PWR_NV_LIM_FIX
  rfapi_set_tx_pwr_limit( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0], tx_limit_dbm, chan_type );
  #else /* FEATURE_WCDMA_MAX_TX_PWR_NV_LIM_FIX */
  #ifdef FEATURE_MULTI_RX_CHAIN //TODO_7600
  rfapi_set_tx_pwr_limit(  rfapi_dev_ptr[device], tx_limit_dbm );
  #else
  rfapi_set_tx_pwr_limit( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0], tx_limit_dbm );
  #endif
  #endif /* FEATURE_WCDMA_MAX_TX_PWR_NV_LIM_FIX */
}


/*===========================================================================

FUNCTION  RFM_GET_RX_AGC_VAL

DESCRIPTION
  Returns the Rx AGC value in units of dBm.

DEPENDENCIES
  rfm_init() must have already been called.
  
RETURN VALUE
  Rx AGC value in units of dBm.

SIDE EFFECTS
  None

===========================================================================*/
int16 rfm_get_rx_agc_val
(
  #ifdef FEATURE_MULTI_RX_CHAIN
  rfcom_device_enum_type device
  #endif /* FEATURE_MULTI_RX_CHAIN */
)
{
  RFM_DEBUG_EN_Q(&rfm_get_rx_agc_val, 0, 0, 0, 0);
    
  #ifdef FEATURE_MULTI_RX_CHAIN
  return rfapi_get_rx_agc_val(  rfapi_dev_ptr[device] );
  #else
  return rfapi_get_rx_agc_val( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0] );
  #endif
}

/*===========================================================================

FUNCTION  RFM_GET_TX_AGC_VAL

DESCRIPTION
  Returns the Tx AGC value in units of dBm.

DEPENDENCIES
  rfm_init() must have already been called.
  
RETURN VALUE
  Tx AGC value in units of dBm.

SIDE EFFECTS
  None

===========================================================================*/
int16 rfm_get_tx_agc_val
( 
  #ifdef FEATURE_MULTI_RX_CHAIN
  rfcom_device_enum_type device
  #endif /* FEATURE_MULTI_RX_CHAIN */
)
{
  RFM_DEBUG_EN_Q(&rfm_get_tx_agc_val, 0, 0, 0, 0);

  #ifdef FEATURE_MULTI_RX_CHAIN
  return rfapi_get_tx_agc_val(  rfapi_dev_ptr[device] );
  #else
  return rfapi_get_tx_agc_val( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0]);
  #endif
}

/*===========================================================================

FUNCTION  RFM_LOG_AGC

DESCRIPTION
  Logs AGC data values for QXDM.

DEPENDENCIES
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.  
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfm_log_agc
(
#ifdef FEATURE_MULTI_RX_CHAIN
  rfcom_device_enum_type device
#endif /* FEATURE_MULTI_RX_CHAIN */
)
{
  //RFM_DEBUG_EN_Q(&rfm_log_agc, 0, 0, 0, 0);

  #ifdef FEATURE_MULTI_RX_CHAIN
  rfapi_log_agc( rfapi_dev_ptr[device] );
  #else
  rfapi_log_agc( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0]);
  #endif
}

/*===========================================================================

FUNCTION  RFM_ENABLE_RX

DESCRIPTION
  
  Enables a receiver chain for the specified mode of operation. Turns 
  on LDOs for transmit hardware.  Turns on PA.  Places receiver chain 
  in Rx state of operation.

  Callback function is called when Rx hardware has settled and is ready
  for use. For support of multiple events occurring during RF hardware
  settling time, event parameter is passed into callback, as well as a
  pointer to user data that was passed in by caller.  Note: Callback
  executes in interrupt context. 
  
  Calling module must wait for the delay msec before assuming 
  RF enabled. This operation will not tune the RF chain.

DEPENDENCIES
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.  
  
RETURN VALUE
  Returns the wait time for this operation.

SIDE EFFECTS
  None

===========================================================================*/
uint32 rfm_enable_rx
( 
  rfcom_device_enum_type device,        /* RF Multimode device */
  const void *user_data_ptr,            /* Pointer to user data passed into
                                           callback */
  rfm_cb_handler_type cb_handler        /* Callback handler */
)
{
  RFM_DEBUG_EN_Q(&rfm_enable_rx, device, 0, 0, 0);

  return rfapi_enable_rx(  rfapi_dev_ptr[device], user_data_ptr,
                          (rfcom_cb_handler_type) cb_handler );
}

/*===========================================================================

FUNCTION  RFM_DISABLE_RX

DESCRIPTION
  
  Disbles a receiver chain. Performs all the necessary activities to minimize 
  current drawn by the RF chain which has been disabled.


DEPENDENCIES
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.  
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rfm_disable_rx
( 
  rfcom_device_enum_type device         /* RF Multimode device */
)
{
  RFM_DEBUG_EN_Q(&rfm_disable_rx, device, 0, 0, 0);

  rfapi_disable_rx(  rfapi_dev_ptr[device] );
}

/*===========================================================================

FUNCTION  RFM_MULTIMODE_HANDOVER

DESCRIPTION
  
  Performs either a multimode WCDMA or CDMA handover. 

  A WCDMA handover is performed when WCDMA service is lost or based on 
  some other criteria. This scenario starts with chain 0 in WCDMA idle 
  state, and chain 1 is in CDMA state. Upon handover decision, we bring 
  up CDMA on chain 1, and all shared resources are switched to CDMA

  When we perform a multimode CDMA handover, we start with chain 0 in CDMA
  state. When the decision is made to switch to WCDMA service, either one 
  of two options can happen:

  1) We start tuning and buffering up chain 1 until chain 1 can continuously
  operate. We then switch all shared resources to WCDMA, bring up WCDMA
  on chain 0 and CDMA on chain 1

  2) If there is enough time to bring up CDMA on chain 1, we move CDMA 
  to chain 1, otherwise, we keep CDMA on chain 0 until we get the
  quick page/page message. We then bring up WCDMA on chain 0 and CDMA
  on chain 1, switch all shared resources to WCDMA.

DEPENDENCIES
  rfm_init() must have already been called.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rfm_multimode_handover (rf_multimode_handover_cmd_type chain_0_cmd,
                             rf_multimode_handover_cmd_type chain_1_cmd)
{
  RFM_DEBUG_EN_Q(&rfm_multimode_handover, 0, 0, 0, 0);

  rfapi_multimode_handover (rfapi_dev_ptr[RFCOM_TRANSCEIVER_0],
                            rfapi_dev_ptr[RFCOM_RECEIVER_1],
                            chain_0_cmd,
                            chain_1_cmd);
}


/*===========================================================================

FUNCTION  RFAPI_GET_STATE

DESCRIPTION
  This function returns the internal path configuration state to the calling
  function.

DEPENDENCIES
  rfapi_init() must have already been called.
  
RETURN VALUE
  returns a the internal state of type rf_path_state_type.

SIDE EFFECTS
  None

===========================================================================*/

rf_path_state_type rfm_get_state(void)
{
  RFM_DEBUG_EN_Q(&rfm_get_state, 0, 0, 0, 0);

  return rfapi_get_state();
}



/*===========================================================================

FUNCTION rfm_get_band_mask                                  INTERNAL FUNCTION

DESCRIPTION
  Returns enabled/disabled state of bands set in NV item, NV_RF_HW_BC_CONFIG_I.

DEPENDENCIES
  None

RETURN VALUE
 sys_band_mask_type containing enabled/disabled states of bands.

SIDE EFFECTS
  None
===========================================================================*/
uint64 rfm_get_band_mask ( rfcom_device_enum_type device )
{
  RFM_DEBUG_EN_Q(&rfm_get_band_mask, 0, 0, 0, 0);

  switch(device)
  {
  default:
  return rfapi_get_band_mask( rfapi_dev_ptr[device] );
  }
}





/*=========================================================================

DESCRIPTION
  This function returns a boolean indicating whether the specified channel
  is supported by the chipset driver in the specified band.  If the band 
  or channel within the band is not supported FALSE is returned; otherwise 
  TRUE is returned.


DEPENDENCIES
  None
   

RETURN VALUE
  boolean:  TRUE   if band/channel is supported by the current devices 
                   configured in the library
            FALSE  if band/channel is not supported by the current devices 
                   configured in the library  

SIDE EFFECTS
  None.
===========================================================================*/
boolean rfm_is_band_chan_supported
(
  sys_band_class_e_type band, 
  word chan
)
{
  RFM_DEBUG_EN_Q(&rfm_is_band_chan_supported, 0, 0, 0, 0);

  return rfapi_is_band_chan_supported(  rfapi_dev_ptr[RFCOM_TRANSCEIVER_0], band, chan);
}


/*===========================================================================

FUNCTION       RFM_SET_PRIMARY_PATH

DESCRIPTION    This is used to set an RF physical path (antenna -> demod -> RxFE, etc.)
               to the RFCOM_TRANSCEIVER_0 logical device.  Typically, 
               the RFCOM_TRANSCEIVER_0 is mapped with the physical RF path 0 (intuitive!).
               However, for certain test cases, we might want to exercise the secondary (diversity)
               physical RF path, and therefore map it to the logical device RFCOM_TRANSCEIVER_0.

               This also has the effect of mapping the "other" RF physical path to the "other" 
               logical device.  For example calling rfm_set_primary_path( RF_PATH_0)
               maps RFCOM_TRANSCEIVER_0 logical device to RF_PATH_0 physical device AND 
               maps RFCOM_RECEIVER_DIV logical device to RF_PATH_1 physical device.
  
DEPENDENCIES

  
RETURN VALUE


SIDE EFFECTS
  None

===========================================================================*/
void rfm_set_primary_path( rf_path_enum_type primary_path )
{
  #ifdef FEATURE_WCDMA
#error code not present
  #endif
}


/*===========================================================================

FUNCTION  RFM_GET_TX_PWR_LIMIT

DESCRIPTION
  This function gets the current value of Tx power limit in units of
  Tx AGC counts. (-512 to +511).

DEPENDENCIES
  rfm_init() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int16 rfm_get_tx_pwr_limit
(               
  #ifdef FEATURE_MULTI_RX_CHAIN
  rfcom_device_enum_type device
  #else /* FEATURE_MULTI_RX_CHAIN */
  void
  #endif /* FEATURE_MULTI_RX_CHAIN */
)
{
  return rfapi_get_tx_pwr_limit( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0] );
}

/*===========================================================================

FUNCTION  RFM_GET_RECEIVE_AGC_VAL

DESCRIPTION
  Returns the Rx AGC value.

DEPENDENCIES
  rfm_init() must have already been called.

RETURN VALUE
  Rx AGC value.

SIDE EFFECTS
  None

===========================================================================*/
int16 rfm_get_receive_agc_val
(   
  #ifdef FEATURE_MULTI_RX_CHAIN
  rfcom_device_enum_type device,
  #endif /* FEATURE_MULTI_RX_CHAIN */
  rfcom_rxagc_type rxagc_type,
  rfcom_rxagc_unit_type rxagc_unit
)
{
  return rfapi_get_receive_agc_val( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0], rxagc_type, rxagc_unit );
}

/*===========================================================================

FUNCTION  RFM_GET_RECEIVE_AGC_VAL_FOR_RSCP

DESCRIPTION
  Returns the Rx AGC value for RSCP.

DEPENDENCIES
  rfm_init() must have already been called.

RETURN VALUE
  Rx AGC value in dbm.

SIDE EFFECTS
  None

===========================================================================*/
int16 rfm_get_receive_agc_val_for_rscp
( 
  #ifdef FEATURE_MULTI_RX_CHAIN
  rfcom_device_enum_type device,
  #endif /* FEATURE_MULTI_RX_CHAIN */
  rfcom_rxagc_type rxagc_type
)
{
  return rfapi_get_receive_agc_val_for_rscp( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0], rxagc_type );
}



#ifdef FEATURE_WCDMA_MIN_INIT_TX_PWR
/*===========================================================================

FUNCTION  RFM_GET_MIN_TX_POWER_VAL

DESCRIPTION
  Returns the Minimum Tx Power value in units of dB12.

DEPENDENCIES
  rfm_init() must have already been called.

RETURN VALUE
  Minimum Tx Power value in units of dB12.

SIDE EFFECTS
  None

===========================================================================*/
int16 rfm_get_min_tx_power_val( void )
{
  return rfapi_get_min_tx_power_val( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0]);
}
#endif /* FEATURE_WCDMA_MIN_INIT_TX_PWR */



#ifdef FEATURE_FREQ_VS_TEMP_NV_UPDATE
/*===========================================================================

FUNCTION  RFM_READ_TRK_LO_ADJ_DEFAULT

DESCRIPTION
  This function returns a Trk_Lo_Adj default value.

DEPENDENCIES
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.

RETURN VALUE
  Trk_Lo_Adj value in PDM counts.

SIDE EFFECTS
  None

===========================================================================*/
uint16 rfm_read_trk_lo_adj_default
(
  #ifdef FEATURE_MULTI_RX_CHAIN
  rfcom_device_enum_type device
  #else /* FEATURE_MULTI_RX_CHAIN */
  void
  #endif /* FEATURE_MULTI_RX_CHAIN */
)
{
  return rfapi_read_trk_lo_adj_default( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0] );
}

/*===========================================================================

FUNCTION  RFM_READ_TRK_LO_ADJ_SLOPE_DEFAULT

DESCRIPTION
  This function returns a Trk_Lo_Adj slope default value.

DEPENDENCIES
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.

RETURN VALUE
  Trk_Lo_Adj slope value in PDM counts.

SIDE EFFECTS
  None

===========================================================================*/
uint16 rfm_read_trk_lo_adj_slope_default 
(
  #ifdef FEATURE_MULTI_RX_CHAIN
  rfcom_device_enum_type device
  #else /* FEATURE_MULTI_RX_CHAIN */
  void
  #endif /* FEATURE_MULTI_RX_CHAIN */
)

{
  return rfapi_read_trk_lo_adj_slope_default( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0]);
}

/*===========================================================================

FUNCTION  RFM_READ_TRK_LO_ADJ

DESCRIPTION
  This function returns a Trk_Lo_Adj value based on current temperature
  from the rfi_trk_lo_adj_vs_temp table.

DEPENDENCIES
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.

RETURN VALUE
  Trk_Lo_Adj value in PDM counts.

SIDE EFFECTS
  None

===========================================================================*/
uint16 rfm_read_trk_lo_adj
(
  #ifdef FEATURE_MULTI_RX_CHAIN
  rfcom_device_enum_type device
  #else /* FEATURE_MULTI_RX_CHAIN */
  void
  #endif /* FEATURE_MULTI_RX_CHAIN */
)

{
  return rfapi_read_trk_lo_adj( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0] );
}

/*===========================================================================

FUNCTION  RFM_READ_TRK_LO_ADJ_SLOPE

DESCRIPTION
  This function returns a Trk_Lo_Adj current slope value.

DEPENDENCIES
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.

RETURN VALUE
  Trk_Lo_Adj value in PDM counts.

SIDE EFFECTS
  None

===========================================================================*/
uint16 rfm_read_trk_lo_adj_slope
(
  #ifdef FEATURE_MULTI_RX_CHAIN
  rfcom_device_enum_type device
  #else /* FEATURE_MULTI_RX_CHAIN */
  void
  #endif /* FEATURE_MULTI_RX_CHAIN */
)
{
  return rfapi_read_trk_lo_adj_slope( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0]);
}

/*===========================================================================

FUNCTION  RFM_READ_ROT_FREQ

DESCRIPTION
  This function returns the searcher finger's rotator value.

DEPENDENCIES
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.

RETURN VALUE
  Rotator value in HW units.

SIDE EFFECTS
  None

===========================================================================*/
int16 rfm_read_rot_freq
(
  #ifdef FEATURE_MULTI_RX_CHAIN
  rfcom_device_enum_type device
  #else /* FEATURE_MULTI_RX_CHAIN */
  void
  #endif /* FEATURE_MULTI_RX_CHAIN */
)
{
  return rfapi_read_rot_freq( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0]);
}


/*===========================================================================

FUNCTION  RFM_UPDATE_FREQ_ERROR_TABLE

DESCRIPTION
  This function updates the rfi_trk_lo_adj and rfi_rot_freq
  tables with the given values. rfi_rot_freq is updated based on
  the state of donotupdaterotatorflag.

DEPENDENCIES
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfm_update_freq_error_table
(
  #ifdef FEATURE_MULTI_RX_CHAIN
  rfcom_device_enum_type device,
  #endif /* FEATURE_MULTI_RX_CHAIN */

  uint16 trkloadjval,
  uint16 trkloadjslopeval,
  int16 rotatorval,
  boolean donotupdaterotatorflag
)
{
  rfapi_update_freq_error_table ( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0], trkloadjval, trkloadjslopeval,
                                  rotatorval, donotupdaterotatorflag );
}
/*===========================================================================

FUNCTION  RFM_SAVE_FREQ_ERROR_TABLE

DESCRIPTION
  This function saves the contents of rfi_trk_lo_adj and rfi_rot_freq
  tables into NV_TRK_LO_ADJ_VS_TEMP and NV_ROT_FREQ_VS_TEMP items in NV.

DEPENDENCIES
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfm_save_freq_error_table
(
  #ifdef FEATURE_MULTI_RX_CHAIN
  rfcom_device_enum_type device,
  #endif /* FEATURE_MULTI_RX_CHAIN */

  rex_tcb_type *caller_tcb_ptr,   /* Calling task's TCB pointer */
  rex_sigs_type task_nv_wait_sig, /* Task signal to wait for when reading NV */
  void (*task_wait_func_ptr)( rex_sigs_type )
                                  /* Task's wait function to be called
                                     when reading NV */
)
{
  rfapi_save_freq_error_table( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0], caller_tcb_ptr,
                              task_nv_wait_sig, task_wait_func_ptr );
}

#endif /*FEATURE_FREQ_VS_TEMP_NV_UPDATE*/


#ifdef FEATURE_RF_AFC

/*===========================================================================

FUNCTION  RFM_AFC_GET_VCO_DEFAULT

DESCRIPTION
  This function returns the vco accumulator default value.

DEPENDENCIES
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.

RETURN VALUE
  Default vco accumulator value in PDM counts.

SIDE EFFECTS
  None

===========================================================================*/
int16 rfm_afc_get_vco_default
( 
  #ifdef FEATURE_MULTI_RX_CHAIN
  rfcom_device_enum_type device
  #else /* FEATURE_MULTI_RX_CHAIN */
  void
  #endif /* FEATURE_MULTI_RX_CHAIN */
)
{
  return rfapi_afc_get_vco_default( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0] );
}

/*===========================================================================

FUNCTION  RFM_AFC_GET_VCO_SLOPE

DESCRIPTION
  This function provides the slope and range values for vco.

DEPENDENCIES
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.                              

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfm_afc_get_vco_slope
(
  #ifdef FEATURE_MULTI_RX_CHAIN
  rfcom_device_enum_type device,
  #endif /* FEATURE_MULTI_RX_CHAIN */

  uint16 *vco_slope,
  uint8 *vco_slope_range
)
{
  (void) rfapi_afc_get_vco_slope( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0], vco_slope, vco_slope_range );
}

/*===========================================================================

FUNCTION  RFM_AFC_GET_VCO_VS_TEMP

DESCRIPTION
  This function provides the vco value from vco-vs-temp table. If table
  entries are not available, a default value is used.

DEPENDENCIES
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfm_afc_get_vco_vs_temp
(
  #ifdef FEATURE_MULTI_RX_CHAIN
  rfcom_device_enum_type device,
  #endif /* FEATURE_MULTI_RX_CHAIN */

  rfcom_afc_data_indicator_type *indicator,
  int16 *vco
)
{
  /* Return the default value for current support */
  *indicator = RFCOM_AFC_DATA_DEFAULT;
  *vco = rfapi_afc_get_vco_default( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0] );
}

/*===========================================================================

FUNCTION  RFM_AFC_GET_RGS_DATA

DESCRIPTION
  This function provides the data of recent good system, including
  system type, time, temperature, vco and rotator values.

DEPENDENCIES
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfm_afc_get_rgs_data
(
  #ifdef FEATURE_MULTI_RX_CHAIN
  rfcom_device_enum_type device,
  #endif /* FEATURE_MULTI_RX_CHAIN */
  rfcom_afc_rgs_data_type *rgs_data
)
{
  (void) rfapi_afc_get_rgs_data( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0], rgs_data );
}

/*===========================================================================

FUNCTION  RFM_AFC_UPDATE_RGS_DATA

DESCRIPTION
  This function updates the data of recent good system, including
  system type, time, temperature, vco and rotator values. Note that this 
  function only updates the ram shadow. Separate function needs to be called 
  when needed to save data into NV memory.

DEPENDENCIES
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfm_afc_update_rgs_data
(
  #ifdef FEATURE_MULTI_RX_CHAIN
  rfcom_device_enum_type device,
  #endif /* FEATURE_MULTI_RX_CHAIN */
  rfcom_afc_rgs_data_type *rgs_data
)
{
  (void) rfapi_afc_update_rgs_data( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0], rgs_data );
}

/*===========================================================================

FUNCTION  RFM_AFC_SAVE_NV_DATA

DESCRIPTION
  This function saves the data into NV memory. nv_item_mask specifies which
  particular items to save. Multiple items can be specified by ORing the
  masks.
  
DEPENDENCIES
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean rfm_afc_save_nv_data
(
  #ifdef FEATURE_MULTI_RX_CHAIN
  rfcom_device_enum_type device,
  #endif /* FEATURE_MULTI_RX_CHAIN */
  
  rfcom_afc_nv_item_mask_type nv_item_mask,
    /* Mask to specify nv items to save */
  rex_tcb_type *caller_tcb_ptr,   
    /* Calling task's TCB pointer */
  rex_sigs_type task_nv_wait_sig, 
    /* Task signal to wait for when reading NV */
  void (*task_wait_func_ptr)( rex_sigs_type )
    /* Task's wait function to be called when reading NV */
)
{
  return rfapi_afc_save_nv_data( 
    rfapi_dev_ptr[RFCOM_TRANSCEIVER_0], nv_item_mask, caller_tcb_ptr,
    task_nv_wait_sig, task_wait_func_ptr 
  );
}

#endif /* FEATURE_RF_AFC */

#ifdef FEATURE_COMPRESSED_MODE
/*===========================================================================

FUNCTION  rfm_init_cm

DESCRIPTION
  
DEPENDENCIES
  
RETURN VALUE
  none
  
SIDE EFFECTS
  none

===========================================================================*/
void rfm_init_cm (void)
{
  RFM_DEBUG_EN_Q(&rfm_init_cm, 0, 0, 0, 0);

  rfapi_init_cm( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0] );
}

/*===========================================================================

FUNCTION  rfm_get_wtow_rf_ctl_buf

DESCRIPTION
  
DEPENDENCIES
  
RETURN VALUE
  none
  
SIDE EFFECTS
  none

===========================================================================*/
void rfm_get_wtow_rf_ctl_buf ( uint16 arfcn, void *buf_ptr )
{
  RFM_DEBUG_EN_Q(&rfm_get_wtow_rf_ctl_buf, arfcn, 0, 0, 0);

  rfapi_get_wtow_rf_ctl_buf( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0], arfcn, buf_ptr );
}
#endif /* FEATURE_COMPRESSED_MODE */


#ifdef FEATURE_RF_ENABLE_GTOW
/*===========================================================================

FUNCTION  RFM_BUILD_GTOW_SCRIPTS

DESCRIPTION
  
DEPENDENCIES
  rfm_tune_to_intersystem_chan() must have already been called.
  
RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
void rfm_build_gtow_scripts
(
  uint16 channel                       /* Mode dependent channel */
)
{
  RFM_DEBUG_EN_Q(&rfm_build_gtow_scripts, channel, 0, 0, 0);

  rfapi_build_gtow_scripts( rfapi_dev_ptr[RFCOM_TRANSCEIVER_0], channel);
}
#endif /* FEATURE_RF_ENABLE_GTOW */


#ifdef RF_HAS_FEATURE_WCDMA
#error code not present
#endif /* RF_HAS_FEATURE_WCDMA */


#ifdef FEATURE_GSM 
#error code not present
#endif /* FEATURE_GSM */


#ifdef RF_HAS_FEATURE_WCDMA
#error code not present
#endif /* RF_HAS_FEATURE_WCDMA */

#if 0
/*===========================================================================

FUNCTION RFM_GET_RSSI

DESCRIPTION
  This function returns the signal strength for the current channel.

DEPENDENCIES
  None

RETURN VALUE
  raw_rssi    - relative RSSI value

  scaled_rssi - calibrated RSSI value scaled 0..255 over the operating range

SIDE EFFECTS
  None

===========================================================================*/
void rfm_get_rssi
(
  word *raw_rssi,
    /* raw RSSI value */
  word *scaled_rssi
    /* Calibrated RSSI value */
)
{
#ifdef FEATURE_CDMA1X    
    rf_get_rssi(raw_rssi,scaled_rssi);
#endif    
}
#endif

#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */

/*===========================================================================

FUNCTION RFM_IOCTL

DESCRIPTION
  
DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
rfcom_ioctl_return_type rfm_ioctl
(
  rfcom_device_enum_type device,
  rfcom_ioctl_param_type request,
  void *pbuf,
  int32 length
)
{
  return rfapi_ioctl(rfapi_dev_ptr[device], request, pbuf, length);
}

/*===========================================================================

FUNCTION RFM_GET_NUM_ANTENNA

DESCRIPTION
  This function takes the device type and returns an unsigned int 
  representing the number of rf antennas supported on given device

DEPENDENCIES
  None

RETURN VALUE
  Returns an unsigned int representing the no of rf antennas supported.

SIDE EFFECTS
  None
  
===========================================================================*/
uint8 rfm_get_num_antenna( rfcom_device_enum_type device )
{
  /* TODO The following featurization is temporary until the common card 
  structure is implemented. Currently UMTS doesn't have capability to export
  antenna information */

  #ifdef RF_HAS_FEATURE_CDMA1X
  switch(device)
  {
  case RFCOM_TRANSCEIVER_0:
  case RFCOM_RECEIVER_1:
  default:
    return rf_get_num_antenna();
  }
  #else
  return 2;
  #endif 
} /* rfm_get_num_antenna */

/*===========================================================================

FUNCTION RFM_IS_TUNABLE_ON_ANTENNA

DESCRIPTION
  This function takes an index to the antenna array and returns a boolean TRUE
  if the antenna is tunable, FALSE otherwise. 

DEPENDENCIES
  None

RETURN VALUE
  Boolean TRUE if the specified antenna is tunable, FALSE otherwise.

SIDE EFFECTS
  None
  
===========================================================================*/
boolean rfm_is_tunable_on_antenna
(
  rfcom_device_enum_type device, 
  int32 antenna_index,
  rf_card_band_type band
)
{
  /* TODO The following featurization is temporary until the common card 
  structure is implemented. Currently UMTS doesn't have capability to export
  antenna information */

  #ifdef RF_HAS_FEATURE_CDMA1X
  switch(device)
  {
  case RFCOM_TRANSCEIVER_0:
  case RFCOM_RECEIVER_1:
  default:
    return rf_is_tunable_on_antenna( antenna_index, band );
  }
  #else
  return TRUE;
  #endif
} /* rfm_is_tunable_on_antenna */

/*===========================================================================

FUNCTION RFM_GET_ANTENNA_DIFF

DESCRIPTION
  This function takes the band, channel and indexes to two antennas as 
  parameters and returns a TRUE if the first antenna has better or equal 
  sensitivity to the the 2nd antenna.( The channel will be supported in 
  the future implementation. Currently it is forced to 0). 

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the first index is pointing to the antenna with the better or
  equal sensitivity as compared to the antenna pointed to by the 2nd index 
  otherwise a FALSE is returned.

SIDE EFFECTS
  None
  
===========================================================================*/
rf_antenna_diff_type rfm_get_antenna_diff
( 
  rfcom_device_enum_type device, 
  rf_card_band_type band,
  uint16 chan,
  int32 ant_x,
  int32 ant_ref 
)
{
  /* TODO The following featurization is temporary until the common card 
  structure is implemented. Currently UMTS doesn't have capability to export
  antenna information */

  #ifdef RF_HAS_FEATURE_CDMA1X
  switch(device)
  {
  case RFCOM_TRANSCEIVER_0:
  case RFCOM_RECEIVER_1:
  default:
    return rf_get_antenna_diff(band, chan, ant_x, ant_ref);
  }
  #else
  return RF_ANT_QLTY_UNKNOWN;
  #endif 
} /* rfm_get_antenna_diff */


#ifdef T_MSM7600
#error code not present
#endif





