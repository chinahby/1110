#ifndef RFLL1X_H
#define RFLL1X_H

/*===========================================================================


           R F  D r i v e r  L o g i c a l  L a y e r  
                          H e a d e r  F i l e

DESCRIPTION
  This file contains all declarations and definitions necessary to use
  the RF driver logical layer 1X interface.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 1999 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM6200/DRIVERS/vcs/rfllumts.h_v   1.0   01 Jul 2002 18:17:00   doerfler  $
$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rfll1x.h#1 $ $DateTime: 2008/04/11 05:11:57 $ $Author: ppottier $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/18/06   jfc     Added rfll1x_is_sleeping()
10/20/05   sar     Added rfll1x_is_band_chan_supported().
05/07/05   hkk     Added RFLL1X_set_ftm_test_call_flag to enable calls on secondary 
				   chain while on FTM.	
09/19/03   dyc     Fix for compiler error.
09/18/03   sar     Added function rfll1x_wakeup().
09/12/03   sar     Added function rfll1x_get_rx_agc_val(). 
02/13/03   sar     Eliminated rfll1x_init() function.
08/02/02   sar     Adapted for Leopard (1x/GSM).
07/01/02   sd      Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "rex.h"

#include "rfcom.h"
#include "rfll.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

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
  rf_init_cdma1x() must have already been called.  
  MDSP must be downloaded and initialized for mode of operation specified
  by mode before calling this function.
  
RETURN VALUE
  Returns total delay in microseconds this function will wait for RF
  hardware to settle.

SIDE EFFECTS
  None
===========================================================================*/
uint32 rfll1x_enter_mode
(
  rfll_device_desc_type *dev_desc,      /* Device descriptor */         
  rfcom_mode_enum_type rf_mode,         /* RF mode */
  const void *user_data_ptr,            /* Pointer to user data passed into
                                           callback */
  rfcom_cb_handler_type cb_handler      /* Call back handler */  
);

/*===========================================================================

FUNCTION  RFLL1X_TUNE_TO_CHAN

DESCRIPTION
  This function tunes the Rx and Tx synthesizers to the specified channel.
  The channel is the UTRA Absolute Radio Frequency Channel Number (UARFCN).
  The call back function is called after the synthesizers have settled.

DEPENDENCIES
  rfll1x_init() must have already been called.
  rfll1x_init_cdma() must have already been called.  
  
RETURN VALUE
  Time required for RF synthesizers to lock in units of microseconds. 

SIDE EFFECTS
  None

===========================================================================*/
uint32 rfll1x_tune_to_chan
(
  rfll_device_desc_type *dev_desc,      /* Logical layer device descriptor */  
  uint16 channel,                       /* UARFCN channel number for
                                           IMT2000 band (0 - 16383) */
  rfcom_synth_tune_enum_type synths,    /* Not used for WCDMA mode */
  const void *user_data_ptr,            /* Pointer to user data passed into
                                           callback */
  rfcom_cb_handler_type cb_handler      /* Call back handler or NULL */
);

/*===========================================================================

FUNCTION  RFLL1X_IS_SLEEPING

DESCRIPTION
  Queries whether the RF is sleeping right now.

DEPENDENCIES
  
RETURN VALUE
  True if RF is in sleep state, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean rfll1x_is_sleeping
(
  rfll_device_desc_type *dev_desc      /* Logical layer device descriptor */
);

/*===========================================================================

FUNCTION  RFLL1X_SLEEP

DESCRIPTION
  This function completes transition of RF hardware to sleep state by
  turning off the Rx and Tx LDOs.

DEPENDENCIES
  rfll1x_sleep() must have already been called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uint32 rfll1x_sleep  //TODO_7600
(
  rfll_device_desc_type *dev_desc,      /*lint -e818 Device descriptor 
                                          could be declared as const */
  const void *user_data_ptr,            /* Pointer to user data passed into
                                         callback */
  rfcom_cb_handler_type cb_handler      /* Call back handler or NULL */  
);

/*===========================================================================

FUNCTION  RFLL1X_STAY_ASLEEP

DESCRIPTION
  This function completes transition of RF hardware to sleep state by
  turning off the Rx and Tx LDOs.

DEPENDENCIES
  rfll1x_sleep() must have already been called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfll1x_stay_asleep
(
  rfll_device_desc_type *dev_desc      /* Logical layer device descriptor */
);

/*===========================================================================

FUNCTION  RFLL1X_ENABLE_RX

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
uint32 rfll1x_enable_rx
(
  rfll_device_desc_type *dev_desc,      /* Logical layer device descriptor */  
  const void *user_data_ptr,            /* Pointer to user data passed into
                                           callback */
  rfcom_cb_handler_type cb_handler      /* Call back handler or NULL */
);

/*===========================================================================

FUNCTION  RFLL1X_DISABLE_RX

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
void rfll1x_disable_rx
(
  rfll_device_desc_type *dev_desc      /* Logical layer device descriptor */  
);

/*===========================================================================

FUNCTION  RFLL1X_GET_RX_AGC_VAL

DESCRIPTION
  Returns the Rx AGC value in units of dBm.

DEPENDENCIES
  rfm_api() and rfm_enter_mode() must have already been called.
  
RETURN VALUE
  Rx AGC value in units of dBm.

SIDE EFFECTS
  None

===========================================================================*/
int16 rfll1x_get_rx_agc_val
(
  rfll_device_desc_type *dev_desc      /* Logical layer device descriptor */
);

/*===========================================================================

FUNCTION  RFLL1X_WAKEUP

DESCRIPTION
  Wakes up the rf by enabling the rx digital power.

DEPENDENCIES
  rfm_api() and rfm_enter_mode() must have already been called.
  
RETURN VALUE
  Does not return a meaningful value.

SIDE EFFECTS
  None

===========================================================================*/
uint32 rfll1x_wakeup
(
  rfll_device_desc_type *dev_desc,
  rfcom_mode_enum_type mode,
  const void *data,
  rfcom_cb_handler_type cb
);

/*===========================================================================


FUNCTION  RFLL1X_set_ftm_test_call_flag

DESCRIPTION
  Sets the rfll1x_ftm_test_call_flag.

DEPENDENCIES
 
  
RETURN VALUE
  

SIDE EFFECTS
  None
=======================================================*/
void rfll1x_set_ftm_test_call_flag
(boolean ftm_flag );

/*=========================================================================

DESCRIPTION RFLL1X_IS_BAND_CHAN_SUPPORTED
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
boolean rfll1x_is_band_chan_supported
(
  rfll_device_desc_type *dev_desc,
  sys_band_class_e_type band, 
  word chan
);

/*=========================================================================

DESCRIPTION rfll1x_multimode_wcdma_handover

  This function disables CDMA on chain 1 and enables CDMA on chain 0 per
  multimode WCDMA handover requirements

DEPENDENCIES
  Assume that chain 0 is disabled of WCDMA
   
RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void rfll1x_multimode_wcdma_handover 
( 
  rfll_device_desc_type * dev_desc_c0,
  rfll_device_desc_type * dev_desc_c1
);

/*=========================================================================

DESCRIPTION rfll1x_multimode_cdma_handover

  This function enables CDMA on chain 1 if requested. It also CDMA on 
  chain 0

DEPENDENCIES
  None
   
RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void rfll1x_multimode_cdma_handover 
( 
  rfll_device_desc_type * dev_desc_c0,
  rfll_device_desc_type * dev_desc_c1,
  boolean a_not_b
);

/*=========================================================================

DESCRIPTION RFLL1X_IOCTL
  

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
rfcom_ioctl_return_type rfll1x_ioctl
(
  rfll_device_desc_type *dev_desc,
  rfcom_ioctl_param_type request,
  void *pbuf,
  int32 length
);

#endif  /* RFLL1X_H */
