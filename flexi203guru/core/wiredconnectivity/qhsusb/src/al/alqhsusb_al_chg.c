/*=======================================================================*//**
  @file         qhsusb_al_chg.c

  @brief        Adaptation layer for dead battery charger

  @details      

 
                Copyright (c) 2008 QUALCOMM Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*//*========================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //depot/asic/qsc1100/core/wiredconnectivity/qhsusb/main/latest/src/al/alqhsusb_al_chg.c#1 $$DateTime: 2009/02/20 14:20:17 $$Author: ehong $

  when        who       what, where, why
  ----------  ---      -------------------------------------------------------
  2008-09-23  stzahi   Move external definitions to header file
  2008-08-28  vkondrat Initial version
  
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "comdef.h"
#include "qhsusb_fd_chg.h"
#include "qhsusb_dcd.h"
#include "qhsusb_al_chg_api.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#ifndef FEATURE_QHSUSB_DEAD_BATTERY_CHARGING
#error "Charger code require FEATURE_QHSUSB_DEAD_BATTERY_CHARGING"
#endif
#ifndef QHSUSB_HAVE_MULTI_CONFIG
#error "Charger code require QHSUSB_HAVE_MULTI_CONFIG"
#endif

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
static struct qhsusb_dcd_dsc_device* dev = NULL;
/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
boolean hsu_boot_start_stack(void)
{
   dev = qhsusb_fd_chg_init();
   return ( NULL != dev );
}

void hsu_boot_stop_stack(void)
{
   qhsusb_fd_chg_shutdown();
}


/**
 * Analyze status of USB device, detect changes.
 * Return appropriate event.
 * 
 * @note Wall charger detection done before this code called
 * 
 * 3 parameters analyzed:
 * 
 * - (a) attachment state
 * - (c) configuration state
 * - (s) suspend state
 * 
 * old state remembered. Current state compared with new one.
 * Depending on transaction the following events detected.
 * 
 * States named a,c,s for attached, configured, suspend.
 * 
 * - xxx -> 0xx : disconnect HSU_BOOT_POLL_RESULT_DISCONNECTED 
 * - xxx -> 1x1 : suspend    HSU_BOOT_POLL_RESULT_SUSPENDED 
 * - xxx -> 110 : configured HSU_BOOT_POLL_RESULT_ENUMERATED 
 * - 0xx -> 100 : connect    HSU_BOOT_POLL_RESULT_CONNECTED_TO_HOST_PC
 * - 1xx -> 100 : resume     HSU_BOOT_POLL_RESULT_RESUMED_NOT_ENUMERATED
 *  
 * @return Event
 */
qhsusb_boot_poll_result hsu_boot_poll_stack(void)
{
   static uint32 a0   = 0; /**< old for attached */
   static uint32 c0   = 0; /**< old for configured */
   static uint32 s0   = 0; /**< old for suspended */
   uint32 a,c,s;
   qhsusb_boot_poll_result evt = HSU_BOOT_POLL_RESULT_NO_NEW_EVENT;
   qhsusb_dcd_poll(dev);
   a = (uint32)(dev->is_attached);
   c = (0 != dev->config_value);
   s = (uint32)(dev->is_suspended);
   if ( ( a ^ a0 ) | ( c ^ c0 ) | ( s ^ s0 ) ) { /* if anything changed */
      qhsusb_dcd_log_event(dev,evt_portsc,(a0<<2)|(c0<<1)|s0,(a<<2)|(c<<1)|s);
      if ( 0 == a ) { /* final state is detached */
         /* TRANSITION: acs = xxx -> 0xx */
         evt = HSU_BOOT_POLL_RESULT_DISCONNECTED;
      } else {        /* final state is attached */
         if ( 0 != s ) { /* final state is suspended */
            /* TRANSITION: acs = xxx -> 1x1 */
            evt = HSU_BOOT_POLL_RESULT_SUSPENDED;
         } else {        /* final state is not suspended */
            if ( 0 != c ) { /* final state is configured */
               /* TRANSITION: acs = xxx -> 110 */
               if ( dev->max_current < 500 ) {
                  evt = HSU_BOOT_POLL_RESULT_ENUMERATED_100MA;
               } else {
                  evt = HSU_BOOT_POLL_RESULT_ENUMERATED;
               }
            } else {        /* final state is unconfigured */
               /* TRANSITION: acs = xxx -> 100 */
               if ( 0 == a0 ) {          /* first time? */
                  /* TRANSITION: acs = 0xx -> 100 */
                  evt = HSU_BOOT_POLL_RESULT_CONNECTED_TO_HOST_PC;
               } else {
                  /* TRANSITION: acs = 1xx -> 100 */
                  evt = HSU_BOOT_POLL_RESULT_RESUMED_NOT_ENUMERATED;
               }
            }
         }
      }
      a0 = a;
      c0 = c;
      s0 = s;
      qhsusb_dcd_log_event(dev, evt_set_power, 
                           (uint32)(dev->max_current), 
                           (uint32)(int)(evt));
   }
   return evt;
}

boolean hsu_boot_disconnect_from_host(void)
{
   return FALSE;
}

