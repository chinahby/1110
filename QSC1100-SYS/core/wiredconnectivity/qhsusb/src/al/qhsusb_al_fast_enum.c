/*=======================================================================*//**
  @file         qhsusb_al_fast_enum.c

  @brief        Adaptation layer for fast enumeration

  @details      
                In case of data card on PC, BIOS may want to enumerate all
                attached devices in order to check every device against its
                whitelist.
                Application starts stack, polls it until enumeration done,
                then shuts down stack.

                There are 3 flavors:
                - separate descriptor set
                - HDLC descriptor set
                - Charger descriptor set

                In case of HDLC or Charger, memory saving acheived, providing
                HDLC or Charger function already exist in the same image.
 
                Copyright (c) 2008 QUALCOMM Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*//*========================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //source/qcom/qct/core/wiredconnectivity/qhsusb/main/latest/src/al/qhsusb_al_fast_enum.c#5 $$DateTime: 2008/10/19 00:41:51 $$Author: vkondrat $

  when        who       what, where, why
  ----------  ---      -------------------------------------------------------
  2008-10-19  vkondrat FD selection - HDLC, CHG, separate
  2008-09-28  vkondrat Initial version
  
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "comdef.h"
#include "qhsusb_dci_api.h"         // qhsusb_dci_disconnect
#include "qhsusb_dcd.h"

/* select FD */
#if defined(FEATURE_QHSUSB_FAST_ENUM_HDLC)
   #include "qhsusb_fd_hdlc.h"
#elif defined(FEATURE_QHSUSB_FAST_ENUM_CHG)
   #include "qhsusb_fd_chg.h"
#elif defined(FEATURE_QHSUSB_FAST_ENUM_SEPARATE)
   #include "qhsusb_fd_fast_enum.h"
#else
   #error "Fast-Enum base function not defined"
#endif

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#ifndef FEATURE_QHSUSB_FAST_ENUMERATION
   #error "Fast-Enum requires FEATURE_QHSUSB_FAST_ENUMERATION"
#endif

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
static struct qhsusb_dcd_dsc_device* dev = NULL;

#ifdef FEATURE_QHSUSB_FAST_ENUM_HDLC
static uint8 qhsusb_str_product[36] = {                       // iProduct
   sizeof(qhsusb_str_product),USB_DESCTYPE_STRING,
   'Q',0,
   'H',0,
   'S',0,
   'U',0,
   'S',0,
   'B',0,
   ' ',0,
   'F',0,
   'a',0,
   's',0,
   't',0,
   ' ',0,
   'E',0,
   'n',0,
   'u',0,
   'm',0,
   ' ',0,
};
#endif

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
#if defined(FEATURE_QHSUSB_FAST_ENUM_HDLC)
static void dummy_rx_complete(struct qhsusb_urb* urb)
{
}
#endif
/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
void qhsusb_al_fast_enum_init(void)
{
#if defined(FEATURE_QHSUSB_FAST_ENUM_HDLC)
   dev = qhsusb_fd_hdlc_init(dummy_rx_complete, qhsusb_str_product);
#elif defined(FEATURE_QHSUSB_FAST_ENUM_CHG)
   dev = qhsusb_fd_chg_init();
#elif defined(FEATURE_QHSUSB_FAST_ENUM_SEPARATE)
   dev = qhsusb_fd_fast_enum_init();
#endif
}

void qhsusb_al_fast_enum_shutdown(void)
{
#if defined(FEATURE_QHSUSB_FAST_ENUM_HDLC)
   qhsusb_fd_hdlc_shutdown();
#elif defined(FEATURE_QHSUSB_FAST_ENUM_CHG)
   qhsusb_fd_chg_shutdown();
#elif defined(FEATURE_QHSUSB_FAST_ENUM_SEPARATE)
   qhsusb_fd_fast_enum_shutdown();
#endif
   qhsusb_dci_disconnect(dev->core_id); // @todo - The qhsusb_fd_fast_enum_shutdown() should call DCI
}


boolean qhsusb_al_fast_enum_poll(void)
{
   qhsusb_dcd_poll(dev);
   return (0 != dev->config_value);
}

void qhsusb_al_fast_enum_delay_usec(uint32 microseconds)
{
   uint32 core_id = dev->core_id;
   uint32 time_usec = 0;

   qhsusb_dci_stop_timer(core_id, QHSUSB_TIMER_1_ONE_SHOT); // stop before start

   qhsusb_dci_start_timer(core_id, QHSUSB_TIMER_1_ONE_SHOT,FALSE);

   do {
      time_usec = qhsusb_dci_get_time_usec(core_id, QHSUSB_TIMER_1_ONE_SHOT); 
   } while ( time_usec < microseconds );

   qhsusb_dci_stop_timer(core_id, QHSUSB_TIMER_1_ONE_SHOT); // stop before start
}

