/*=============================================================================

             R F  D r i v e r  L o g i c a l  L a y e r 
                           D e f i n i t i o n

GENERAL DESCRIPTION
  This file provides the logical layer 1x interface definitions of the
  RF driver. 

EXTERNALIZED FUNCTIONS
  None  

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000 - 2007  by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/



/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM6200/DRIVERS/vcs/rfllumts.c_v   1.0   01 Jul 2002 18:17:04   doerfler  $

  $Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rfll1x.c#2 $
  $DateTime: 2008/10/01 18:07:35 $
  $Author: vboda $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/30/08   vm/vb   Added Support for Band Class Sub Block Implementation
11/15/07   ems     Support for QSC1100
11/05/07   adk     Called rf_hdr_mdsp_init_tx_agc_settings() for QRF7600 and TRF6500.
03/19/07   dyc     Omit rf_cdma_mdsp_init_tx_agc_settings for QSC60x5
03/08/07   adk     Added support for FW TX AGC.
03/01/07   sar     Removed compile warnings.
12/18/06   jfc     Added rfll1x_is_sleeping()
10/31/06   ycl     Use RF_HAS_POWER_MODE_LIB to enclose rf_intelliceiver_get_power_mode.
10/17/06   ycl     Modifications for initial MSM7600 build.
08/17/06   sar/bmg Added calls to disable task switching while in rfll1x_enable_rx
                   and rfll1x_disable_rx functions. 
05/30/06   sar     Fixed transition from Diversity to R1 only in rfll1x_disable_rx..
05/10/06   hkk     Backed out BBRX changes
04/17/06   hkk     Added changes for BBRX sleep/wakeup for current saving with Saturn
01/26/06   sar     Added call to rf_tx_shutdown() to rfll1x_sleep() to force
                   disabling of the chain even if the Tx was on. Also updated 
                   messaging to new MSG_X() type macro.
01/23/06   sar     Added call to rf_disable_diversity() when Rx0 is disabled in 
                   in Diversity sate. 
01/18/06   sar     Eliminated reference to RF_STATE_GPS in rfll1x_sleep().
01/11/06   et      Fixed lint errors.
01/10/06   sar     Fixed lint errors.
10/27/05   bmg     Changed ASSERTs to RF_ASSERTs
10/21/05   sar     Fixed Lint warnings and errors.
10/20/05   sar     Added function rfll1x_is_band_chan_supported().
09/02/05   hkk     Backed out park mode changes
08/19/05   sar     Allow FTM mode in rfll1x_enable_rx function to enable power to Rx Chain 0
                   at any time.
08/11/05   hkk     Changed include file clkregim.h instead of clkrgm_6500.h to fix compile 
                   error in Phoenix.
08/01/05   ra      removed rfll1x_ftm_test_call_flag. This was a temporary requirement
07/15/05   sar     Added changes to support new rf_sleep() and rf_stay_asleep() functions.
07/04/05   dyc     Lint cleanup.
05/18/05   sar     Added support for transitions between: RF_R1_ONLY_ST & RF_DIVERSITY_ST,
                   RF_DUAL_ST & RF_DIVERSITY_ST.
05/07/05   hkk     Added RFLL1X_set_ftm_test_call_flag to enable calls on secondary 
                   chain while on FTM.	
02/24/05   sar     Modified rfll1x_disable_rx() to disable secondary chain while
                   exiting Diversity if the primary chain is not specified. Also 
                   updated rfll1x_sleep() to handle all valid modes of secondary chain.
08/16/04   sar     Updated rfll1x_sleep to handle RFCOM_RECEIVER_DUAL mode.
08/11/04   sar     Updated functions to support restructured rfc layer.
07/06/04   sar     Fixed a typo in rfll1x_enter_mode().
07/06/04   sar     When FEATURE_CHECK_RF_MODE is defined, allow rfll1x_enter_mode 
                   to return with out any processing if condition exists that 
                   rf_mode is same as before.
06/15/04   sar     Updated calls to GZRF6500.
05/28/04   sar     Changed names for state related variable to eliminate conflict.
04/12/04   dyc     Replace TUNE_TO_CHAN_DELAY_US with RF_TUNE_TO_CHAN_TIME_USEC from
                   bsp header.
04/07/04   sar     Added definitions for state variables accessed in rfm_init().
01/27/04   sar     Added checks in rfll1x_sleep() and rfll1x_set_stay_asleep() to check 
                   rf chain state before invoking rf_sleep() and rf_set_stay_asleep().
                   Replaced rf_sleep() calls with rfll1x_sleep() in rfll1x_disable_rx()
                   to allow rf state checking.
11/26/03   sar     Removed function rfll1x_get_rx_agc_val().
11/17/03   sar     Added code to configure primary and secondary VCOs in accordance
                   with rfm state table requirements. Updated rfm_enable_rx() and 
                   rfm_disable_rx() functions to reflect changes in state diagram.
10/31/03   dyc     Use rf_set_stay_asleep() in rfll1x_stay_asleep()
10/09/03   sar     Changed the case for RF_R0_ONLY_ST in rfll1x_enable_rx to unconditionally
                   call rf_enable_digital_rx_pwr for RFCOM_TRANSCEIVER_0.
10/08/03   sar     Modified code in rfll1x_enter_mode() to set mode for the device
                   passed in the parameter only.
09/29/03   dbc     Add separate cases for RFCOM_RECEIVER_1 and RFCOM_RECEIVER_DIV
                   in rfll1x_disable_rx()
09/24/03   sar     Put back the initialization of path-1 in rfll1x_enter_mode().
09/17/03   sar     Modified rfll1x_tune_to_chan function to handle diversity.
                   Also cleaned up rfll1x_enter_mode to initialize the specified
                   path. Added function rfll1x_wakeup().
09/12/03   sar     Added function rfll1x_get_rx_agc_val();
09/09/03   sar     disabled the piece of code for Jaguar in rfll1x_enter_mode() 
                   which forces an early exit when there is no change in RF mode.
09/05/03   sar     Added device parameter to rf_stay_asleep() call in rfll1x_stay_asleep().
09/04/03   sar     Updated rfll1x_enable_rx() and rfll1x_disable_rx() functions 
                   to use rf_enable_diversity() and rf_disable_diversity() functions
                   in proper sequence. Eliminated transition from RF_DIVERSITY_ST to
                   RF_DISABLED_ST in rf_disable_rx().
08/26/03   sar     Added call to rf_enable diversity() in rfm_enable_rx().
09/11/03   dbc     Always return status in rfll1x_enter_mode().
07/11/03   sar     Added support for multipath chipset drivers.
02/20/03   aks     Changed rf_sleep() to work with backward compatible format.
02/13/03   sar     Eliminated rfll1x_init(). Added rf_init_delayed() call in rfll1x_enter_mode().
08/02/02   sar     Adapted for Leopard (1x/GSM).
07/01/02   sd      Initial revision.

=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/


#include "comdef.h"

#include "rfcom.h"
#include "rfll.h"
#include "rfllcb.h"
#include "rf.h"
#include "rfi.h"
#include "rfc.h"
#include "err.h"
#include "clkregim.h"
#include "msm_drv.h"
#include "ftm.h"
#include "adie.h"
#include "rf_intelliceiver.h"
#include "hw.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
#define FEATURE_CHECK_RF_MODE

/*---------------------------------------------------------------------------
  Defines required 1x synthesizer settling time in microseconds.
---------------------------------------------------------------------------*/
#define TUNE_TO_CHAN_DELAY_US 500

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

FUNCTION  RFLL1X_ENTER_MODE

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
  rfll_device_desc_type *dev_desc,      /*lint -e818 Device descriptor 
                                          could be declared as const */         
  rfcom_mode_enum_type rf_mode,         /* RF mode */
  const void *user_data_ptr,            /* Pointer to user data passed into
                                           callback */
  rfcom_cb_handler_type cb_handler      /*lint -esym(715, cb_handler), not used*/  
)
{
    rfll_device_desc_type *desc_ptr;
    rf_init_status_type ret_val = RF_INIT_SUCCESS;



#if !defined(FEATURE_MULTI_RX_CHAIN) || defined(FEATURE_CHECK_RF_MODE)
    /* if already in the required mode then do nothing */
    if ( dev_desc->mode == rf_mode)
    { 
       /* Return status through data pointer */
       if(user_data_ptr!=NULL)
          *((rf_init_status_type *) user_data_ptr) = RF_INIT_SUCCESS;

       return 0;
    }
#endif    
    
    
    /* Initialize the RF driver call back service */
    rfllcb_init();
    
    switch(dev_desc->device)
    {
    case RFCOM_TRANSCEIVER_0:

        /* Perform 1st phase of RF initialization*/
        rf_init_cdma1x(dev_desc->device);

        /* Performt 2nd phase of RF initialization */
        ret_val = rf_init_delayed(dev_desc->device);
        
        rf_path_state = RF_DISABLED_ST;
        
        break;

    case RFCOM_RECEIVER_1:

        /* Perform 1st phase of RF initialization*/
        rf_init_cdma1x(dev_desc->device);

        /* Performt 2nd phase of RF initialization */
        (void) rf_init_delayed(dev_desc->device);

            /* Set Mux to select VCO 1 for Chain 1 */
        rfc_select_secondary_vco();
        
        /* Disable Secondary VCO for initial state */
        rfc_disable_secondary_chain_vco();

        break;

    case RFCOM_RECEIVER_DIV: /* No low level initialization required */
    case RFCOM_RECEIVER_DUAL:/* No low level initialization required */
        break;

    default:
        break;

    }
    
    /* Return status through data pointer */
    if(user_data_ptr!=NULL)
        *((rf_init_status_type *) user_data_ptr) = ret_val;
    
    /* Initialization successful so update device descriptor. */
    desc_ptr = &rfll_device_desc_table[dev_desc->device];
    desc_ptr->mode   = RFCOM_1X_MODE;
    desc_ptr->state  = RFLL_RX_STATE;

    /* RF initialization successful */
    return 0;


} /* rfll1x_enter_mode() */

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
  rfll_device_desc_type *dev_desc,      /*lint -e818 Device descriptor 
                                          could be declared as const */  
  uint16 channel,                       /* UARFCN channel number for
                                           IMT2000 band (0 - 16383) */
  rfcom_synth_tune_enum_type synths,    /*lint -esym(715, synths), Not used for WCDMA mode */
  const void *user_data_ptr,            /* Pointer to user data passed into
                                           callback */
  rfcom_cb_handler_type cb_handler      /* Call back handler or NULL */
)
{
  rfllcb_struct_type *cb_ptr;
  

  /* Verify  channel is valid. */
  RF_ASSERT( channel <= 16383 );

  
  /* Tune to  channel. */

  #ifdef RF_HAS_SUB_BLOCK_SUPPORT
  rf_tune_band_chan(dev_desc->device,rf_chain_status[rfcom_to_path[dev_desc->device]].rf_curr_cdma_band,channel);
  #else
  rf_tune_to_chan_cdma1x(dev_desc->device, channel );
  #endif

  if ( cb_handler != NULL )
  {
    /* Get call back buffer, setup call back event,  and register call
       back event. */
    cb_ptr = rfllcb_get_event_buffer();
    cb_ptr->cb_handler = cb_handler;
    cb_ptr->user_data_ptr = user_data_ptr;
    cb_ptr->num_events = 1;
    cb_ptr->cb_events[0].event = RFCOM_TIME_DELAY_EXPIRED;
    cb_ptr->cb_events[0].delay = rf_card.rfc_tune_to_chan_time_usec;
    rfllcb_register_events( cb_ptr );
  }
  
  return rf_card.rfc_tune_to_chan_time_usec;
  
} /* rfll1x_tune_to_chan() */

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
)
{
  return rf_is_sleeping_cdma1x (dev_desc->device);
}

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
)
{
  
  rf_chain_state_type *rf_chain_0 = &rf_chain_status[rfcom_to_path[RFCOM_TRANSCEIVER_0]];
  rf_chain_state_type *rf_chain_1 = &rf_chain_status[rfcom_to_path[RFCOM_RECEIVER_1]];
  rf_chain_state_type *dev_chain = &rf_chain_status[ rfcom_to_path[dev_desc->device]];

  if (user_data_ptr==NULL) 
  {
	  // TODO_7600: CDMA 1X flag parameter (FALSE do nothing)
	  return 0;
  }
  /* Put RF to sleep */
  if( dev_desc->device == RFCOM_RECEIVER_DIV || dev_desc->device == RFCOM_RECEIVER_DUAL)
  {
      if(rf_chain_0->rf_state == RF_STATE_CDMA_RX)
            rf_sleep_cdma1x(RFCOM_TRANSCEIVER_0);
      else
            MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR,
                   "rfll1x_sleep, RF Primary Chain in invalid state: %d",
                   rf_chain_0->rf_state );

      if(rf_chain_1->rf_state == RF_STATE_CDMA_RX ||
           #ifndef RF_HAS_BYPASS_RF_STATE_GPS_CHECK
           rf_chain_1->rf_state == RF_STATE_GPS ||    
           #endif
           rf_chain_1->rf_state == RF_STATE_DIVERSITY)
           rf_sleep_cdma1x(RFCOM_RECEIVER_1);
      else

           MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR,
                  "rfll1x_sleep, RF Secondary Chain in invalid state: %d", 
                  rf_chain_1->rf_state);

  }
  else if(dev_desc->device == RFCOM_TRANSCEIVER_0)
  {
      if(dev_chain->rf_state == RF_STATE_CDMA_RXTX)
      {
          rf_tx_shutdown();
          rf_sleep_cdma1x(dev_desc->device);
      }
      else if(dev_chain->rf_state == RF_STATE_CDMA_RX)
      {
          rf_sleep_cdma1x(dev_desc->device);
      }
      else

          MSG_2( MSG_SSID_RF, MSG_LEGACY_ERROR,
                 "rfll1x_sleep, RF Chain:%d in invalid state: %d",
                 dev_desc->device,dev_chain->rf_state); 
  }
  else
  {
      if(dev_chain->rf_state == RF_STATE_CDMA_RX)
          rf_sleep_cdma1x(dev_desc->device);
      else
          MSG_2( MSG_SSID_RF, MSG_LEGACY_ERROR,
                 "rfll1x_sleep, RF Chain:%d in invalid state: %d",
                 dev_desc->device,dev_chain->rf_state); 
  }
  
  return 0;
} /* rfll1x_stay_asleep() */

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
  rfll_device_desc_type *dev_desc      /*lint -e818 Device descriptor 
                                          could be declared as const */
)
{
  rf_chain_state_type *dev_chain = &rf_chain_status[ rfcom_to_path[dev_desc->device]];
  /* Put RF to sleep and disable cdma pdm, the rf state must in SLEEP_1 state */
  if(dev_chain->rf_state == RF_STATE_SLEEP_1)
      rf_set_stay_asleep(dev_desc->device, FALSE);
  
} /* rfll1x_stay_asleep() */

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
#ifndef RF_ENABLE_RX_PWR_WAIT
#define RF_ENABLE_RX_PWR_WAIT 10
#endif
uint32 rfll1x_enable_rx
(
  rfll_device_desc_type *dev_desc,      /*lint -e818 Device descriptor 
                                          could be declared as const */  
  const void *user_data_ptr,            /*lint -esym(715, user_data_ptr), not used */
  rfcom_cb_handler_type cb_handler      /* Call back handler or NULL */
)
{
  
  
  rf_path_state_type tstate;  
  rf_chain_state_type *rf_chain_1;

  /* Disable task switching */
  TASKLOCK();

#if defined ( RF_HAS_MDSP_TX_AGC ) && !defined ( RF_HAS_QSC60X5 ) && !defined ( RF_HAS_QSC11X0 )
  rf_cdma_mdsp_init_tx_agc_settings();
#endif

#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */

  tstate = rf_path_state;  
  rf_chain_1 = &rf_chain_status[rfcom_to_path[RFCOM_RECEIVER_1]];


  switch (rf_path_state) {
  case RF_DISABLED_ST:
     if(dev_desc->device == RFCOM_TRANSCEIVER_0)
     {
       /* Enable primary VCO */
       rfc_enable_primary_chain_vco();
       rf_enable_digital_rx_pwr( dev_desc->device );
       tstate = RF_R0_ONLY_ST; 
     }
     else if (dev_desc->device == RFCOM_RECEIVER_1)
     {
       /* Set Mux to select VCO 1 for Chain 1 */
       rfc_select_secondary_vco();
       /* Enable secondary VCO */
       rfc_enable_secondary_chain_vco();
       rf_enable_digital_rx_pwr( dev_desc->device );
       tstate = RF_R1_ONLY_ST; 
     }
     break;
  case RF_R0_ONLY_ST:
     if(dev_desc->device == RFCOM_RECEIVER_1)
     {
       /* Set Mux to select VCO 1 for Chain 1 */
       rfc_select_secondary_vco();
       /* Enable VCO 1 for initial state */
       rfc_enable_secondary_chain_vco();
       rf_enable_digital_rx_pwr( dev_desc->device );
       tstate = RF_DUAL_ST; 
     }
     else if (dev_desc->device == RFCOM_RECEIVER_DIV)
     {
       /* Select VCO 0 for Chain 1 */
       rfc_select_primary_vco();
       /* Disable VCO 1 */
       rfc_disable_secondary_chain_vco();
       /* This requires tuning Rx1 to path 0 synthesizer */
       rf_enable_digital_rx_pwr( RFCOM_RECEIVER_DIV);
       (void) rf_enable_diversity(RFCOM_RECEIVER_DIV);
       tstate = RF_DIVERSITY_ST; 
     }
     else if(dev_desc->device == RFCOM_TRANSCEIVER_0 && ftm_mode == FTM_MODE)
     {
       rf_enable_digital_rx_pwr( dev_desc->device );
     }
     break;
   case RF_R1_ONLY_ST:
     if(dev_desc->device == RFCOM_TRANSCEIVER_0)
     {
       /* Enable Primary VCO */
       rfc_enable_primary_chain_vco();
       /* Check if FTM mode is active?? */
       rf_enable_digital_rx_pwr( dev_desc->device );
       tstate = RF_DUAL_ST; 
     }
     else if(dev_desc->device == RFCOM_RECEIVER_DIV)
     {
       
       /* Set Mux to select VCO 0 for Chain 1 */
       rfc_select_primary_vco();
       /* Enable Primary VCO */
       rfc_enable_primary_chain_vco();
       /* Disable VCO 1 */
       rfc_disable_secondary_chain_vco();
       /* Tune the primary chain to where secondary chain was */
       (void) rf_init_digital_band_chan(
           RFCOM_TRANSCEIVER_0,
           rf_chain_1->rf_curr_cdma_band,
           rf_chain_1->rf_chan );
       rf_complete_warmup();
       /* Setup secondary in Diversity*/
       (void) rf_enable_diversity(RFCOM_RECEIVER_DIV);
       tstate = RF_DIVERSITY_ST; 
     }
     break;
  case RF_DUAL_ST:
     if(dev_desc->device == RFCOM_RECEIVER_DIV)
     {
       /* Set Mux to select VCO 0 for Chain 1 */
       rfc_select_primary_vco();
       /* Disable VCO 1 */
       rfc_disable_secondary_chain_vco();
       (void) rf_enable_diversity(RFCOM_RECEIVER_DIV);
       tstate = RF_DIVERSITY_ST; 
     }
     break;
  case RF_DIVERSITY_ST:
     if(dev_desc->device == RFCOM_RECEIVER_1)
     {
       /* Set Mux to select VCO 1 for Chain 1 */
       rfc_select_secondary_vco();
       /* Enable VCO 1 */
       rfc_enable_secondary_chain_vco();
       /* Disable Diversity */
       (void) rf_disable_diversity(RFCOM_RECEIVER_DIV);

       tstate = RF_DUAL_ST; 
     }
     break;
   default:
     MSG( MSG_SSID_RF, MSG_LEGACY_ERROR,
          "rfll1x_enable_rx, Invalid Device for curr state");
     MSG_2( MSG_SSID_RF, MSG_LEGACY_ERROR,
            "Device ID: %d, Current State: %d", dev_desc->device, rf_path_state);
     break;
   }
  
  rf_path_state = tstate;

  /* Enable task switching */
  TASKFREE();

  /* the return value is the required delay for complete path enable       */
  return  (uint32) RF_ENABLE_RX_PWR_WAIT;
  
} /* rfll1x_enable_rx() */

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
  rfll_device_desc_type *dev_desc      /*lint -e818 Device descriptor 
                                          could be declared as const */  
)
{
    rf_path_state_type tstate;  
    int dummy_data=1;

    /* Disable task switching */
    TASKLOCK();

    tstate = rf_path_state;
    
  /* Put the path into sleep */
    switch (rf_path_state) {
    case RF_R0_ONLY_ST:
      if( dev_desc->device == RFCOM_TRANSCEIVER_0)
      {
        /* Disable Primary VCO */
        rfc_disable_primary_chain_vco();
        rfll1x_sleep(dev_desc, &dummy_data, NULL);
        rfll1x_stay_asleep(dev_desc);
        tstate = RF_DISABLED_ST;
      }
      break;
    case RF_R1_ONLY_ST:
      if( dev_desc->device == RFCOM_RECEIVER_1)
      {
        /* Disable Secondary VCO */
        rfc_disable_secondary_chain_vco();
        rfll1x_sleep(dev_desc, &dummy_data, NULL);
        rfll1x_stay_asleep(dev_desc);
        tstate = RF_DISABLED_ST;
      }
      break;
    case RF_DUAL_ST:
      if(dev_desc->device == RFCOM_TRANSCEIVER_0)
      {
        /* Disable Primary VCO */
        rfc_disable_primary_chain_vco();
        /* Check if FTM active before this transition */
        rfll1x_sleep(dev_desc, &dummy_data, NULL);
        rfll1x_stay_asleep(dev_desc);
        tstate = RF_R1_ONLY_ST; 
      }
      else if (dev_desc->device == RFCOM_RECEIVER_1)
      {
        /* Disable Secondary VCO */
        rfc_disable_secondary_chain_vco();
        rfll1x_sleep(dev_desc, &dummy_data, NULL);
        rfll1x_stay_asleep(dev_desc);
        tstate = RF_R0_ONLY_ST; 
      }
      break;
    case RF_DIVERSITY_ST:
      if(dev_desc->device == RFCOM_TRANSCEIVER_0)
      {
        /* Disable Diversity */
        (void) rf_disable_diversity(RFCOM_RECEIVER_DIV);

        /* Disable both chains */
        rfll1x_sleep(dev_desc, &dummy_data, NULL);
        rfll1x_stay_asleep(dev_desc);
        rf_sleep_cdma1x(RFCOM_RECEIVER_1);
        rf_set_stay_asleep(RFCOM_RECEIVER_1, FALSE );

        /* Set Mux to select VCO 1 for Chain 1 */
        rfc_select_secondary_vco();

        /* Enable secondary chain */
        rfc_enable_secondary_chain_vco();
        rf_enable_digital_rx_pwr( RFCOM_RECEIVER_1 );
        tstate = RF_R1_ONLY_ST; 
      }
      else if (dev_desc->device == RFCOM_RECEIVER_1)
      {
        /* Set Mux back to select VCO 1 for Chain 1 */
        rfc_select_secondary_vco();
        /* Disable Secondary VCO */
        rfc_disable_secondary_chain_vco();
        /* disable diversity chain & sleep chain 1 */
        (void) rf_disable_diversity( RFCOM_RECEIVER_DIV );
        rfll1x_sleep(dev_desc, &dummy_data, NULL);
        rfll1x_stay_asleep(dev_desc);
        tstate = RF_R0_ONLY_ST; 
      }
      break;
    default:
        MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,
            "rfll1x_disable_rx, Invalid Device for curr state");
        MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,
              "Device ID: %d, Current State: %d", dev_desc->device, rf_path_state);
        break;
    }
    
    rf_path_state = tstate;

    /* Enable task switching */
    TASKFREE();

} /* rfll1x_disable_rx() */

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
  rfll_device_desc_type *dev_desc,/*lint -e818 Device descriptor 
                                          could be declared as const */
  rfcom_mode_enum_type mode, /*lint -esym(715, mode), not used */
  const void *data,          /*lint -esym(715, data), not used */
  rfcom_cb_handler_type cb   /*lint -esym(715, cb), not used */
)
{
    rf_enable_digital_rx_pwr( dev_desc->device );
    return 0;
}
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
)
{
    return rf_is_band_chan_supported( dev_desc->device, band, chan );
}

//jcen TBD
#define rf_to_cai(x) x

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
)
{
  /* Disable Rx on Chain 1 */
  rfll1x_disable_rx(dev_desc_c1);

  /* Enable Rx/Tx on Chain 0 */
  rfll1x_enter_mode(dev_desc_c0, RFCOM_1X_MODE, NULL, NULL);
  rfll1x_enable_rx(dev_desc_c0, NULL, NULL);
  rfm_init_digital(RFCOM_TRANSCEIVER_0);
  rfm_complete_warmup(RFCOM_TRANSCEIVER_0);
  //rfm_digital_band_select(RFCOM_TRANSCEIVER_0, band);
  //rfm_tune_to_chan();

}


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
  boolean enable_c1
)
{
  if (enable_c1)
  {
    uint16 chan = rf_chain_status[0].rf_chan;
    uint8 band = rf_to_cai(rf_chain_status[0].rf_curr_cdma_band);

    /* Enable RECEIVER_1 on chain 1 */
    rfll1x_enable_rx(dev_desc_c1, NULL, NULL);
    rfm_init_digital(RFCOM_RECEIVER_1);
    rfm_digital_band_select(RFCOM_RECEIVER_1, band);

    rfll1x_tune_to_chan(dev_desc_c1, chan, RFCOM_TUNE_RX, NULL, NULL);
  }

  /* Disable CDMA on chain 0 */
  rfll1x_disable_rx(dev_desc_c0);
}

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
)
{
  rfcom_ioctl_return_type err = RFCOM_IOCTL_NO_ERROR;

  if (pbuf == NULL)
  {
    err = RFCOM_IOCTL_NULL_BUFFER;
  }
  else
  {
   /* Should probably also check that the buffer is large enough to hold data */
    switch (request)
    {
    case RFCOM_IOCTL_GET_DBG_SCR_TX_AGC:
      *(int16 *)pbuf = rf_get_cdma_tx_agc();
      break;

    case RFCOM_IOCTL_GET_DBG_SCR_TX_AGC_IN_DBM:
      *(int16 *)pbuf = rf_get_cdma_tx_agc_in_dBm();
      break;

    case RFCOM_IOCTL_GET_DBG_SCR_TX_ADJ:
      *(uint8 *)pbuf = rf_get_tx_gain_adj();
      break;

    case RFCOM_IOCTL_GET_DBG_SCR_RX_AGC:
      *(int16 *)pbuf = rf_get_cdma_rx_agc();
      break;

    case RFCOM_IOCTL_GET_DBG_SCR_RX_AGC_IN_DBM:
      {
        int16 ch0_rxagc, ch1_rxagc;
        ch0_rxagc = rf_get_path_cdma_rx_agc_in_dBm( RFCOM_TRANSCEIVER_0 );
        ch1_rxagc = rf_get_path_cdma_rx_agc_in_dBm( RFCOM_RECEIVER_1 );
  
        *(int32 *)pbuf = (ch0_rxagc << 16) | (ch1_rxagc &0xFFFF );
      }
      break;

    case RFCOM_IOCTL_GET_DBG_SCR_PA_RANGE:
      *(uint8 *)pbuf = rf_get_pa_state();
      break;

    case RFCOM_IOCTL_GET_DBG_SCR_HDET:
      *(uint8 *)pbuf = rf_hdet_data;
      break;

    case RFCOM_IOCTL_GET_DBG_SCR_LNA_STATE:
      {
        uint8 ch0_lna, ch1_lna;
        ch0_lna = rf_get_path_lna_gain_state(RFCOM_TRANSCEIVER_0);
        ch1_lna = rf_get_path_lna_gain_state(RFCOM_RECEIVER_1);
        *(uint8 *)pbuf = (ch0_lna << 4) | (ch1_lna & 0x0F);
      }
      break;

    case RFCOM_IOCTL_GET_DBG_SCR_RATCHET_STATE:
      *(uint8 *)pbuf = rf_get_ratchet_state();
      break;
    
    case RFCOM_IOCTL_GET_DBG_SCR_SUPPORTED_HW:
      *(uint8 *)pbuf = rf_get_rf_hw_config();
      break;

    case RFCOM_IOCTL_GET_DBG_SCR_NV_BAND:
      *(uint8*)pbuf = rf_get_nv_band(dev_desc->device);
      break;

    case RFCOM_IOCTL_GET_DBG_SCR_CHANNEL:
      {
        db_items_value_type dbi;
        db_get(DB_CHANNEL, &dbi);
        *(uint16 *)pbuf = dbi.channel;
      }
      break;

    case RFCOM_IOCTL_GET_DBG_SCR_BAND_1X:
      {
        rf_card_band_type band;
        uint16 chan;
        rf_get_band_chan( &band, &chan );
        switch (band)
        {
        case RF_BC0_BAND:
          *(rfi_band_type*)pbuf = RFI_CDMA_CELL_BAND;
          break;
        case RF_BC1_BAND:
          *(rfi_band_type*)pbuf = RFI_CDMA_PCS_BAND;
          break;
        case RF_BC3_BAND:
          *(rfi_band_type*)pbuf = RFI_CDMA_BC3_BAND;
          break;
        case RF_BC4_BAND:
          *(rfi_band_type*)pbuf = RFI_CDMA_BC4_BAND;
          break;
        case RF_BC5_BAND:
          *(rfi_band_type*)pbuf = RFI_CDMA_BC5_BAND;
          break;
        case RF_BC6_BAND:
          *(rfi_band_type*)pbuf = RFI_CDMA_BC6_BAND;
          break;
        case RF_BC7_BAND:
          *(rfi_band_type*)pbuf = RFI_CDMA_BC7_BAND;
          break;
        case RF_BC8_BAND:
          *(rfi_band_type*)pbuf = RFI_CDMA_BC8_BAND;
          break;
        case RF_BC9_BAND:
          *(rfi_band_type*)pbuf = RFI_CDMA_BC9_BAND;
          break;
        case RF_BC10_BAND:
          *(rfi_band_type*)pbuf = RFI_CDMA_BC10_BAND;
          break;
        case RF_BC11_BAND:
          *(rfi_band_type*)pbuf = RFI_CDMA_BC11_BAND;
          break;
        case RF_BC12_BAND:
          *(rfi_band_type*)pbuf = RFI_CDMA_BC12_BAND;
          break;
        case RF_BC14_BAND:
          *(rfi_band_type*)pbuf = RFI_CDMA_BC14_BAND;
          break;
        case RF_GPS_BAND:
          *(rfi_band_type*)pbuf = RFI_GPS_BAND;
          break;
        default:
          *(rfi_band_type*)pbuf = RFI_NO_BAND_SEL;
          break;
        }
      }
      break;

    case RFCOM_IOCTL_GET_DBG_SCR_INTELLICEIVER_POWER_MODE:
#ifdef RF_HAS_POWER_MODE_LIB
      *(uint8 *)pbuf = rf_intelliceiver_get_power_mode( RF_PATH_0 );
#endif
      break;

    case RFCOM_IOCTL_GET_SLEEP_STATE:
      *(boolean *)pbuf = rf_is_sleeping_cdma1x(dev_desc->device);
      break;

    default:
      err = RFCOM_IOCTL_INVALID_REQUEST;
    }
  }

  return err;
}
