#ifndef RFAPI_H
#define RFAPI_H

/*===========================================================================


               R F  D r i v e r  E x t e r n a l  I n t e r f a c e 
                             H e a d e r  F i l e

DESCRIPTION
  This file contains all declarations and definitions necessary to use
  the RF drivers.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The following function must be called before calling any other
  functions provided by this driver:

    1. rfapi_init()

  This places the RF driver in the PARKED/SLEEP state.  Then a specific
  driver mode (i.e. CDMA 1x, HDR, WCDMA, GSM, GPS, PARKED, etc.) can be
  entered by calling the following functions in the sequence given:

    2. rfapi_enter_mode()
    3. rfapi_tune_to_chan()
    4. rfapi_start_loops()

  The RF driver is now in the Idle/Rx state in the mode entered.
  
Copyright (c) 1999 - 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2007        by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/rf/common/main/qsc1100/src/rf/rfapi.h#1 $ 
  $DateTime: 2008/04/11 05:11:57 $ 
  $Author: ppottier $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/07   jfc     Added function headers
01/23/07   ra      Added rfapi_trigger_gps_bbc_update()
01/11/07   ra      Added  RFAPI_EXIT_GPS()
12/18/06   jfc     Split rf_is_sleeping into separate 1x and wcdma functions
10/17/06   ycl     Modifications for initial MSM7600 build.
10/20/05   sar     Added function rfapi_is_band_chan_supported().
04/26/05   sar     Added prototype for rfapi_get_band_mask(). Alos included 
                   sys.h file to support this function.
09/17/03   sar     Added api rfapi_get_state to allow query of the internal 
                   state by the calling applications.
03/27/03   rv      Added MM API for frequency maintenance algorithm in WCDMA.
03/05/03   eh      Added a state parameter in rf_api_init to indicate whether
                   RF NV is valid or not.
11/04/02   sd      Added support for intersystem measurements.
07/01/02   sd      Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "rex.h"

#include "rfcom.h"
#include "sys.h"

#ifdef FEATURE_WCDMA
#error code not present
#endif


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Device descriptor type definition.
---------------------------------------------------------------------------*/
typedef void* rfapi_device_desc_type;

/*---------------------------------------------------------------------------
  Reference to device 0 (primary transceiver) descriptor.
---------------------------------------------------------------------------*/
extern rfapi_device_desc_type rfapi_dev0_ptr;

/*---------------------------------------------------------------------------
  Reference to device 1 (diversity receiver) descriptor.
---------------------------------------------------------------------------*/
extern rfapi_device_desc_type rfapi_dev1_ptr;

/*---------------------------------------------------------------------------
  Reference to device descriptor array.
---------------------------------------------------------------------------*/
extern rfapi_device_desc_type rfapi_dev_ptr[];

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION  RFAPI_INIT 

DESCRIPTION
  Initializes the RF device specified (primary radio transceiver or
  secondary receiver for MSM6500).  Reads common and mode specific RF NV
  parameters from NV and initializes common and mode specific NV tables.
  Initializes common hardware and mode specific RF hardware for each mode
  supported (WCDMA, GSM, CDMA, HDR, GPS, etc.) to a low power parked/sleep
  state.  RF driver is placed in parked/sleep state.
  It returns a state information of whether NV is iniatialized or not.
  If RF NV is initalized it returns non-zero.

DEPENDENCIES
  This function must be called before any other RF driver function.
  May require special parked mode MDSP image to be loaded before calling
  this function (TBD).

RETURN VALUE
  If initialization is successful, returns a pointer to the device
  descriptor to be used for all other RF driver calls to this device,
  otherwise returns NULL.

SIDE EFFECTS
  None

===========================================================================*/
extern rfapi_device_desc_type rfapi_init
(
  rfcom_device_enum_type device,        /* RF device to initialize */
  rex_tcb_type *caller_tcb_ptr,         /* Calling task's TCB pointer */
  rex_sigs_type task_nv_wait_sig,       /* Task signal to wait for when
                                           reading NV */
  void (*task_wait_func_ptr)( rex_sigs_type ),
                                        /* Task's wait function to be called
                                           when reading NV */
  int *state                            /* state whether rf init success or not */
);

/*===========================================================================

FUNCTION  RFAPI_GET_DEVICE_DESCRIPTOR

DESCRIPTION
  Gets the device descriptor of device that has been initialized.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns device descriptor of device that has been initialized.  If
  device has not been initialized, returns a NULL pointer. 

SIDE EFFECTS
  None

===========================================================================*/
extern rfapi_device_desc_type rfapi_get_device_descriptor
(
  rfcom_device_enum_type device         /* RF device to get descriptor for */
);

/*===========================================================================

FUNCTION  RFAPI_ENTER_MODE

DESCRIPTION
  Initializes RF hardware for mode of operation specified by parameter
  rf_mode (WCDMA, GSM, CDMA, HDR, GPS, PARKED/SLEEP, etc.).  Places RF
  driver in Rx state of operation in mode of operation specified, except
  when mode specified is PARKED/SLEEP.  Typically, control loops are not
  started by this function and must be started by calling
  rfapi_start_loops().  Control loops are typically in override mode,
  outputting only the override value.  The actual state of the control
  loops after calling this function are mode dependent.

  Callback function is called when RF hardware has settled and is ready
  for use.  For support of multiple events occurring during RF hardware
  settling time, event parameter is passed into callback, as well as a
  pointer to user data that was passed in by caller.  Note: Callback
  executes in interrupt context.

DEPENDENCIES
  rfapi_init() must have already been called.  
  MDSP must be downloaded and initialized for mode of operation specified
  by mode before calling this function.
  
RETURN VALUE
  Returns total delay in microseconds this function will wait for RF
  hardware to settle.

SIDE EFFECTS
  None

===========================================================================*/
extern uint32 rfapi_enter_mode
(
  rfapi_device_desc_type dev_desc,      /* Device descriptor */         
  rfcom_mode_enum_type rf_mode,         /* RF mode */
  const void *user_data_ptr,            /* Pointer to user data passed into
                                           callback */
  rfcom_cb_handler_type cb_handler      /* Call back handler */  
);

#if defined (RF_HAS_CGPS)
#error code not present
#endif /* RF_HAS_CGPS */

/*===========================================================================

FUNCTION  RFAPI_TUNE_TO_CHAN

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
  rfapi_init() must have already been called.
  rfapi_enter_mode() must have already been called.  
  
RETURN VALUE
  Returns total delay in microseconds this function will wait for
  synthesizers to lock.

SIDE EFFECTS
  None

===========================================================================*/
extern uint32 rfapi_tune_to_chan
(
  rfapi_device_desc_type dev_desc,      /* Device descriptor */         
  uint16 channel,                       /* Mode dependent channel */
  rfcom_synth_tune_enum_type synths,    /* Synthesizer(s) to tune */
  const void *user_data_ptr,            /* Pointer to user data passed into
                                           callback */
  rfcom_cb_handler_type cb_handler      /* Call back handler */
);

/*===========================================================================

FUNCTION  RFAPI_ENABLE_TX

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
  rfapi_init() must have already been called.
  rfapi_enter_mode() must have already been called.
  rfapi_tune_to_chan() should have already been called.
  
RETURN VALUE
  Returns total delay in microseconds this function will wait for RF
  hardware to settle.

SIDE EFFECTS
  None

===========================================================================*/
extern uint32 rfapi_enable_tx
(
  rfapi_device_desc_type dev_desc,      /* Device descriptor */
  const void *user_data_ptr,            /* Pointer to user data passed into
                                           callback */
  rfcom_cb_handler_type cb_handler      /* Callback handler */
);
/*===========================================================================

FUNCTION  RFAPI_ENABLE_RX

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
uint32 rfapi_enable_rx
(
  rfapi_device_desc_type dev_desc,      /* Device descriptor */
  const void *user_data_ptr,            /* Pointer to user data passed into
                                           callback */
  rfcom_cb_handler_type cb_handler      /* Callback handler */
);

/*===========================================================================

FUNCTION  RFAPI_DISABLE_TX

DESCRIPTION
  Disables transmitter for mode of operation RF driver is currently
  operating in.  Turns off LDOs for transmit hardware.  Turns off PA.
  Places RF driver in Rx state of operation.

DEPENDENCIES
  rfapi_enable_tx() should have already been called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void rfapi_disable_tx
(
  rfapi_device_desc_type dev_desc       /* Device descriptor */
);

/*===========================================================================

FUNCTION  RFAPI_DISABLE_RX

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
extern void rfapi_disable_rx
(
  rfapi_device_desc_type dev_desc       /* Device descriptor */
);

/*===========================================================================

FUNCTION  RFAPI_START_TX

DESCRIPTION
  Enables closed power control loops to control transmit power while on
  dedicated traffic channel.  Enables this only if current operating mode
  requires this.  Turns on PA.  May enable other mode specific functionality
  necessary for transmit operation while on the dedicated traffic channel.
  Can be used to enable Tx after hard handoff.

DEPENDENCIES
  rfapi_enable_tx() must have already been called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void rfapi_start_tx
(
  rfapi_device_desc_type dev_desc       /* Device descriptor */
);

/*===========================================================================

FUNCTION  RFAPI_STOP_TX

DESCRIPTION
  Disables closed power control loops or other mode specific functionality
  enabled by rfapi_start_tx().  Turns off PA.  Can be used to disable Tx
  during hard handoff.

DEPENDENCIES
  rfapi_start_tx() should have already been called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void rfapi_stop_tx
(
  rfapi_device_desc_type dev_desc       /* Device descriptor */
);

/*===========================================================================

FUNCTION  RFAPI_IS_SLEEPING

DESCRIPTION
  Queries whether the RF is sleeping right now.

DEPENDENCIES
  None
  
RETURN VALUE
  True if RF is in sleep state, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean rfapi_is_sleeping ( rfapi_device_desc_type dev_desc );


/*===========================================================================

FUNCTION  RFAPI_SLEEP

DESCRIPTION
  Places the majority of RF hardware in sleep mode for the currently
  enabled mode of operation and begins transition of RF driver to sleep
  state.  Actual state of hardware after calling this function is
  operating mode dependent and may require a call to rfapi_stay_asleep()
  later in the sleep timeline to complete the shutdown of all RF hardware
  for sleep.  This is the first stage of the transition to sleep state.

DEPENDENCIES
  Driver must be in the Rx state of operation.  
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern uint32 rfapi_sleep
(
rfapi_device_desc_type dev_desc,       /* Device descriptor */
const void *user_data_ptr,            /* Pointer to user data passed into
                                         callback */
rfcom_cb_handler_type cb_handler      /* Call back handler */  

);

/*===========================================================================

FUNCTION  RFAPI_STAY_ASLEEP

DESCRIPTION
  Completes transition of RF hardware to sleep state.  The actual
  functionality performed is mode dependent and is the final stage of
  placing the RF hardware and drivers in sleep state.  Calling this
  function to enter the final sleep state may not be required for some
  modes of operation.  An example of what this function may do is
  tri-state the track low adjust PDM output after TCXO has been shutoff.  

DEPENDENCIES
  rfapi_sleep() must have already been called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void rfapi_stay_asleep
(
  rfapi_device_desc_type dev_desc       /* Device descriptor */
);

/*===========================================================================

FUNCTION  RFAPI_WAKEUP

DESCRIPTION
  Wakes up RF hardware from sleep mode to the mode of operation specified
  by input parameter rf_mode (WCDMA, GSM, CDMA, HDR, GPS, etc.).  Places
  RF driver in Rx state.  Typically, control loops are not started by this
  function and must be started by calling rfapi_start_loops().  Control
  loops are typically placed in override mode, outputting the override
  value when loop clocks become active.  The actual state of the control
  loops after calling this function are mode dependent.

  Callback function is called when RF hardware wakeup has completed.  For
  support of multiple events occurring during RF wakeup time, event
  parameter is passed into callback, as well as a pointer to user data that
  was passed in by caller.  Note: Callback executes in interrupt context. 

DEPENDENCIES
  rfapi_sleep() must have already been called and rfapi_stay_asleep()
  may have been called.

RETURN VALUE
  Returns total delay in microseconds this function will wait for the RF
  hardware to wakeup.

SIDE EFFECTS
  None

===========================================================================*/
extern uint32 rfapi_wakeup
(
  rfapi_device_desc_type dev_desc,      /* Device descriptor */
  rfcom_mode_enum_type rf_mode,         /* RF mode */
  const void *user_data_ptr,            /* Pointer to user data passed into
                                           callback */
  rfcom_cb_handler_type cb_handler      /* Call back handler */  
);

/*===========================================================================

FUNCTION  RFAPI_CAPTURE_LOOP_VALUES

DESCRIPTION
  Captures all tracking loop and PDM data that applies to a particular
  mode and state of operation.  Data is stored in union pointed to by
  data_ptr.  This data should not be accessed by caller. Data storage
  is allocated by the caller to allow the caller, for example, to
  maintain lists of loop values, when scanning inter-frequency or
  inter-RAT neighbor lists.  Loop values captured are restored by
  calling rfapi_restore_loop_values().

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void rfapi_capture_loop_values
(
  rfapi_device_desc_type dev_desc,      /* Device descriptor */
  rfcom_loop_data_type *data_ptr        /* Pointer to location to store
                                           captured data. */
);

/*===========================================================================

FUNCTION  RFAPI_RESTORE_LOOP_VALUES

DESCRIPTION
  Restores all tracking loop and PDM data contained in union pointed to
  by data_ptr.  Data restored should have been previously captured with
  a call to rfapi_capture_loop_values().  Data is restored based on
  current mode of operation and RF driver state.

DEPENDENCIES
  rfapi_capture_loop_values() must have already been called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void rfapi_restore_loop_values
(
  rfapi_device_desc_type dev_desc,      /* Device descriptor */
  rfcom_loop_data_type *data_ptr        /* Pointer to data that will be
                                           restored */
);

/*===========================================================================

FUNCTION  RFAPI_STOP_LOOPS

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
extern void rfapi_stop_loops
(
  rfapi_device_desc_type dev_desc       /* Device descriptor */
);

/*===========================================================================

FUNCTION  RFAPI_START_LOOPS

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
extern void rfapi_start_loops
(
  rfapi_device_desc_type dev_desc       /* Device descriptor */
);

/*===========================================================================

FUNCTION  RFAPI_SETUP_INTERSYSTEM_MEAS

DESCRIPTION
  XXXXX.....
  
  Callback function is called when RF hardware has settled and is ready
  for use.  For support of multiple events occurring during RF hardware
  settling time, event parameter is passed into callback, as well as a
  pointer to user data that was passed in by caller.  Note: Callback
  executes in interrupt context.

DEPENDENCIES
  rfapi_init() must have already been called.
  rfapi_enter_mode() must have already been called.  
  
RETURN VALUE
  Returns total delay in microseconds this function will wait for RF
  hardware to settle.

SIDE EFFECTS
  None

===========================================================================*/
extern uint32 rfapi_setup_intersystem_meas
(
  rfapi_device_desc_type dev_desc,      /* Device descriptor */         
  rfcom_mode_enum_type rf_mode,         /* RF mode of intersystem measurement */
  const void *user_data_ptr,            /* Pointer to user data passed into
                                           callback */
  rfcom_cb_handler_type cb_handler      /* Call back handler */  
);

/*===========================================================================

FUNCTION  RFAPI_STOP_INTERSYSTEM_MEAS

DESCRIPTION
  XXXXX.....
  
  Callback function is called when RF hardware has settled and is ready
  for use. For support of multiple events occurring during RF hardware
  settling time, event parameter is passed into callback, as well as a
  pointer to user data that was passed in by caller.  Note: Callback
  executes in interrupt context. 

DEPENDENCIES
  rfapi_setup_intersystem_meas() must have already been called.
  
RETURN VALUE
  Returns total delay in microseconds this function will wait for RF
  hardware to settle.

SIDE EFFECTS
  None

===========================================================================*/
extern uint32 rfapi_stop_intersystem_meas
(
  rfapi_device_desc_type dev_desc,      /* Device descriptor */
  const void *user_data_ptr,            /* Pointer to user data passed into
                                           callback */
  rfcom_cb_handler_type cb_handler      /* Callback handler */
);

/*===========================================================================

FUNCTION  RFAPI_TUNE_TO_INTERSYSTEM_CHAN

DESCRIPTION
  Tunes, or configures in preparation for a tune, the UHF Rx synthesizer
  to the channel specified.  The channel is interpreted based on the
  intersystem measurement operating mode passed in.

  Callback function is called when tuning is complete and synthesizers have
  locked.  For support of multiple events occurring during synthesizer lock
  time, event parameter is passed into callback, as well as a pointer to
  user data that was passed in by caller.  Note: Callback executes in
  interrupt context.

DEPENDENCIES
  rfapi_init() must have already been called.
  rfapi_enter_mode() must have already been called.  
  
RETURN VALUE
  Returns total delay in microseconds this function will wait for
  synthesizers to lock if a tune is performed.

SIDE EFFECTS
  None

===========================================================================*/
extern uint32 rfapi_tune_to_intersystem_chan
(
  rfapi_device_desc_type dev_desc,      /* Device descriptor */         
  rfcom_mode_enum_type rf_mode,         /* RF mode of intersystem measurement */
  uint16 channel,                       /* Mode dependent channel */
  const void *user_data_ptr,            /* Pointer to user data passed into
                                           callback */
  rfcom_cb_handler_type cb_handler      /* Call back handler */
);

/*===========================================================================

FUNCTION  RFAPI_DO_TX_PWR_LIMIT

DESCRIPTION
  Adjusts the Tx power.  Reads HDET value and adjusts Tx power based
  on HDET value, temperature, and frequency.

DEPENDENCIES
  rfapi_init() must have already been called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern int16 rfapi_do_tx_pwr_limit
(
  rfapi_device_desc_type dev_desc       /* Device descriptor */
);

/*===========================================================================

FUNCTION  RFAPI_DO_TEMP_COMP

DESCRIPTION
  Performs temperature compensation.  Adjusts AGC linearizer tables
  based on temperature.  Must be called at 10 second intervals to perform
  temperature compensation.

DEPENDENCIES
  rfapi_init() must have already been called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void rfapi_do_temp_comp
(
  rfapi_device_desc_type dev_desc       /* Device descriptor */
);

/*===========================================================================

FUNCTION  RFAPI_SET_TX_PWR_LIMIT

DESCRIPTION
  Sets the Tx power limit to value given in units of dBm.

DEPENDENCIES
  rfapi_init() must have already been called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void rfapi_set_tx_pwr_limit
(
  rfapi_device_desc_type dev_desc,      /* Device descriptor */
  int16 tx_limit_dbm                    /* Tx power limit in dBm */
  #ifdef FEATURE_WCDMA_MAX_TX_PWR_NV_LIM_FIX
,rfcom_txplim_channel_type chan_type
  #endif /* FEATURE_WCDMA_MAX_TX_PWR_NV_LIM_FIX */
);

/*===========================================================================

FUNCTION  RFAPI_SET_TX_PWR_LIMIT

DESCRIPTION
  Sets the Tx power limit to value given in units of dBm.

DEPENDENCIES
  rfapi_init() must have already been called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern int16 rfapi_get_tx_pwr_limit
(
rfapi_device_desc_type dev_desc      /* Device descriptor */
);


/*===========================================================================

FUNCTION  RFAPI_GET_RX_AGC_VAL

DESCRIPTION
  Returns the Rx AGC value in units of dBm.

DEPENDENCIES
  rfapi_init() must have already been called.
  
RETURN VALUE
  Rx AGC value in units of dBm.

SIDE EFFECTS
  None

===========================================================================*/
extern int16 rfapi_get_rx_agc_val
(
  rfapi_device_desc_type dev_desc       /* Device descriptor */
);

/*===========================================================================

FUNCTION  RFAPI_GET_RECEIVE_AGC_VAL

DESCRIPTION
  Returns the Rx AGC value.

DEPENDENCIES
  rfapi_init() must have already been called.
  
RETURN VALUE
  Rx AGC value.

SIDE EFFECTS
  None

===========================================================================*/
extern int16 rfapi_get_receive_agc_val
(
rfapi_device_desc_type dev_desc,       /* Device descriptor */
rfcom_rxagc_type rxagc_type,
rfcom_rxagc_unit_type rxagc_unit
);

/*===========================================================================

FUNCTION  RFAPI_GET_RECEIVE_AGC_VAL_FOR_RSCP

DESCRIPTION
  Returns the Rx AGC value without noise floor correction in units of dBm.

DEPENDENCIES
  rfapi_init() must have already been called.
  
RETURN VALUE
  Rx AGC value without noise floor correction in units of dBm.

SIDE EFFECTS
  None

===========================================================================*/
extern int16 rfapi_get_receive_agc_val_for_rscp
(
rfapi_device_desc_type dev_desc,       /* Device descriptor */
rfcom_rxagc_type rxagc_type
);

/*===========================================================================

FUNCTION  RFAPI_GET_TX_AGC_VAL

DESCRIPTION
  Returns the Tx AGC value in units of dBm.

DEPENDENCIES
  rfapi_init() must have already been called.
  
RETURN VALUE
  Tx AGC value in units of dBm.

SIDE EFFECTS
  None

===========================================================================*/
extern int16 rfapi_get_tx_agc_val
(
  rfapi_device_desc_type dev_desc       /* Device descriptor */
);

#ifdef FEATURE_WCDMA_MIN_INIT_TX_PWR
/*===========================================================================

FUNCTION  RFAPI_GET_MIN_TX_POWER_VAL

DESCRIPTION
  Returns the Minimum Tx Power value in units of dB12.

DEPENDENCIES
  rfapi_init() must have already been called.
  
RETURN VALUE
  Minimum Tx Power value in units of dB12.

SIDE EFFECTS
  None

===========================================================================*/
extern int16 rfapi_get_min_tx_power_val
(
rfapi_device_desc_type dev_desc       /* Device descriptor */
);
#endif /* FEATURE_WCDMA_MIN_INIT_TX_PWR */

/*===========================================================================

FUNCTION  RFAPI_LOG_AGC

DESCRIPTION
  Logs AGC data values for QXDM.

DEPENDENCIES
  rfapi_init() must have already been called.
  rfapi_enter_mode() must have already been called.  
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void rfapi_log_agc
(
  rfapi_device_desc_type dev_desc       /* Device descriptor */
);

/*===========================================================================

FUNCTION  RFAPI_READ_TRK_LO_ADJ_VS_TEMP

DESCRIPTION
  This function returns a Trk_Lo_Adj value based on current temperature
  from the rfi_trk_lo_adj_vs_temp table.

DEPENDENCIES
  rfapi_init() must have already been called.
  
RETURN VALUE
  Trk_Lo_Adj value in PDM counts.

SIDE EFFECTS
  None

===========================================================================*/
extern uint16 rfapi_read_trk_lo_adj_vs_temp
(
  rfapi_device_desc_type dev_desc       /* Device descriptor */
);

/*===========================================================================

FUNCTION  RFAPI_READ_ROT_FREQ_VS_TEMP

DESCRIPTION
  This function returns the searcher finger's rotator value based on current 
  temperature from the rfi_rot_freq_vs_temp table.

DEPENDENCIES
  rfapi_init() must have already been called.
  
RETURN VALUE
  Rotator value in HW units.

SIDE EFFECTS
  None

===========================================================================*/
extern int16 rfapi_read_rot_freq_vs_temp
(
  rfapi_device_desc_type dev_desc       /* Device descriptor */
);

/*===========================================================================

FUNCTION  RFAPI_UPDATE_FREQ_ERROR_TABLE

DESCRIPTION
  This function updates the rfi_trk_lo_adj_vs_temp and rfi_rot_freq_vs_temp
  tables with the given values. rfi_rot_freq_vs_temp is updated based on 
  the state of donotupdaterotatorflag.

DEPENDENCIES
  rfapi_init() must have already been called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void rfapi_update_freq_error_table
(
  rfapi_device_desc_type dev_desc,       /* Device descriptor */
  uint16 trkloadjval, 
  int16 rotatorval, 
  boolean donotupdaterotatorflag
);

/*===========================================================================

FUNCTION  RFAPI_SAVE_FREQ_ERROR_TABLE

DESCRIPTION
  This function saves the contents of rfi_trk_lo_adj_vs_temp and 
  rfi_rot_freq_vs_temp tables into NV_TRK_LO_ADJ_VS_TEMP and 
  NV_ROT_FREQ_VS_TEMP items in NV.

DEPENDENCIES
  rfapi_init() must have already been called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void rfapi_save_freq_error_table
(
  rfapi_device_desc_type dev_desc,       /* Device descriptor */
  rex_tcb_type *caller_tcb_ptr,         /* Calling task's TCB pointer */
  rex_sigs_type task_nv_wait_sig,       /* Task signal to wait for when
                                           reading NV */
  void (*task_wait_func_ptr)( rex_sigs_type )
                                        /* Task's wait function to be called
                                           when reading NV */
);
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
extern rf_path_state_type rfapi_get_state(void);

/*===========================================================================

FUNCTION RFAPI_GET_BAND_MASK                                   INTERNAL FUNCTION

DESCRIPTION
  Returns enabled/disabled state of bands set in NV item, NV_RF_HW_BC_CONFIG_I.

DEPENDENCIES
  None

RETURN VALUE
 sys_band_mask_type containing enabled/disabled states of bands. 

SIDE EFFECTS
  None
===========================================================================*/
extern sys_band_mask_type rfapi_get_band_mask( rfapi_device_desc_type dev_desc );
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
extern boolean rfapi_is_band_chan_supported
(
    rfapi_device_desc_type dev_desc,       /* Device descriptor */
    sys_band_class_e_type band, 
    word chan
);


#ifdef FEATURE_WCDMA
#error code not present
#endif //FEATURE_WCDMA

/*=========================================================================

FUNCTION rfapi_set_primary_path

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
extern void rfapi_set_primary_path(rfapi_device_desc_type dev_desc,
                                   rf_path_enum_type primary_path);

/*=========================================================================

FUNCTION rfapi_multimode_handover

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
extern void rfapi_multimode_handover
(
  rfapi_device_desc_type dev_desc_c0,       /* Chain 0 device descriptor */
  rfapi_device_desc_type dev_desc_c1,       /* Chain 1 device descriptor */
  rf_multimode_handover_cmd_type dev_cmd_c0,/* Chain 0 mode command */
  rf_multimode_handover_cmd_type dev_cmd_c1 /* Chain 1 mode command */
);


/*=========================================================================

FUNCTION rfapi_ioctl

DESCRIPTION
  
DEPENDENCIES
  None

RETURN VALUE
  Error status

SIDE EFFECTS
  None.
===========================================================================*/
extern rfcom_ioctl_return_type rfapi_ioctl
(
  rfapi_device_desc_type dev_desc,
  rfcom_ioctl_param_type request,
  void *pbuf,
  int32 length
);

#endif  /* RFAPI_H */

