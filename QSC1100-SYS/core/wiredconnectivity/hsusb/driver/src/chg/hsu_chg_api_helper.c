/*==============================================================================

Charging Module API Helper to Interface with the High Speed USB
H E A D E R   F I L E

GENERAL DESCRIPTION
Contains the implementation of API for responding to HS-USB state changes 
(e.g. connect, disconnect, suspend, resume) needed to allow charging via HS-USB.

EXTERNALIZED FUNCTIONS
hsu_chg_api_helper_suspended
hsu_chg_api_helper_resumed
hsu_chg_api_helper_is_resumed

INITALIZATION AND SEQUENCING REQUIREMENTS
None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/
/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/chg/hsu_chg_api_helper.c#6 $
$DateTime: 2008/10/17 15:51:02 $ $Author: jaychoi $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
05/25/07 esh  Initial version

==============================================================================*/
/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "hsu_chg_api_helper.h"

#include "hsu_apis.h"
#include "hsu_conf_sel_stack_utils.h" /* For controller index for remote wakeup,
                                        as well as HSU_DEVICE_MAX_POWER */
#ifdef FEATURE_HS_USB_CHARGING
#ifdef FEATURE_HS_USB_CHG_REMOTE_API
#include "charger.h"
#endif /* FEATURE_HS_USB_CHG_REMOTE_API */
#endif /* FEATURE_HS_USB_CHARGING */
/*==============================================================================

LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
Constants and Macros
==============================================================================*/
#define CHG_RMT_WAKEUP_DELAY_IN_MSEC (20)
  /* According to USB 2.0 spec, 7.1.7.7, remote wakeup must be issued no sooner
  than 5 ms after receiving suspend. Currently this is not sufficient, so as a 
  TEMP fix for Test Track CR#178, 20 ms is being used. */
/*==============================================================================
Typedefs
==============================================================================*/
/*==============================================================================
Variables
==============================================================================*/
static boolean hsu_chg_api_chg_is_resumed = FALSE;
/*==============================================================================

EXTERNAL FUNCTION DEFINITIONS FOR MODULE

==============================================================================*/
/*===========================================================================
FUNCTION hsu_chg_api_helper_suspended()
===========================================================================*/
void hsu_chg_api_helper_suspended(void)
{
  hsu_chg_api_chg_is_resumed = FALSE;
  /* Invoke API, on ARM9, to notify charger module that device is suspended,
  If it's connected - it can no longer charge.
  If it's not connected, then it will just update its internal variables
  (so that when charging is activated, it will already know what state it's in).*/
#ifdef FEATURE_HS_USB_CHARGING  
#ifdef FEATURE_HS_USB_CHG_REMOTE_API
#ifdef FEATURE_HS_USB_CHG_SUSPEND
  chg_usb_suspended(TRUE);
#else
  chg_usb_i_is_not_available();
#endif /* FEATURE_HS_USB_CHG_SUSPEND */
#else
  hsu_api_notif_charger_not_avail();
#endif /* FEATURE_HS_USB_CHG_REMOTE_API */
#endif /* FEATURE_HS_USB_CHARGING */
}

/*===========================================================================
FUNCTION hsu_chg_api_helper_resumed()
===========================================================================*/
void hsu_chg_api_helper_resumed(void)
{
  uint32 amount_of_current_to_draw = 0;
  hsu_chg_api_chg_is_resumed = TRUE;
  
  HSU_USE_PARAM(amount_of_current_to_draw);
  /* Invoke API, on ARM9, to notify charger module that device is suspended,
  If it's connected - it can only charge the allowed amount when unconfigured
  (until a SET_CONFIGURATION(non_zero) is issued).
  If it's not connected, then it will just update its internal variables
  (so that when charging is activated, it will already know what state it's in).*/
  if (hsu_is_dev_configured() == TRUE)
  {
      amount_of_current_to_draw = HSU_CHG_API_HELPER_MAX_POWER;
  }
  else /* !configured */
  {
      amount_of_current_to_draw = HSU_CHG_API_HELPER_POWER_UNCONFIGURED;
  }
#ifdef FEATURE_HS_USB_CHARGING
#ifdef FEATURE_HS_USB_CHG_REMOTE_API
  chg_usb_i_is_available(amount_of_current_to_draw);
#else
  hsu_api_notif_charger_is_avail(amount_of_current_to_draw);
#endif /* FEATURE_HS_USB_CHG_REMOTE_API */
#endif /* FEATURE_HS_USB_CHARGING */
}

/*===========================================================================
FUNCTION hhsu_chg_api_helper_is_resumed()
===========================================================================*/
boolean hsu_chg_api_helper_is_resumed(void)
{
  return hsu_chg_api_chg_is_resumed;
}
