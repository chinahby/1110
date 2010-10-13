/*==============================================================================

USB Wall Charger Detection API - I M P L E M E N T A T I O N   F I L E

GENERAL DESCRIPTION
Contains the API implementations for detecting the presence of a USB wall charger,
in case the USB PHY is Chipidea internal PHY. 

EXTERNALIZED FUNCTIONS
hsu_chg_wallchg_detect_wall_charger

INITALIZATION AND SEQUENCING REQUIREMENTS
None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/
/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/chg/hsu_chg_wallchg_intg_phy.c#13 $
$DateTime: 2009/03/20 18:44:05 $ $Author: jaychoi $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
03/12/09 jch  Added algorithm for reliable charger detection
12/23/08 jch  Added 180nm ULPI support and detection timer
12/03/07 esh  Initial version

==============================================================================*/
#ifndef BUILD_HOSTDL /* ARMPRG does not contain charging */
/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "hsu_chg_wallchg.h"
  
/* TODO: Need an abstract layer whether this is AMSS (rex) or OEMSBL (no rex) */
#ifdef FEATURE_HS_USB_BASIC
#include "boothw.h"
#else 
#include "rex.h"
#include "hsu_bus_mode_notifier.h"
#include "hsu_os_abstract_bus.h"
#endif /* FEATURE_HS_USB_BASIC */
  
#include "hsu_common.h"
#include "err.h"
#include "msmhwioreg.h"
#include "msmhwio.h"
#include "clk.h"
#ifdef FEATURE_HS_USB_HAL
#include "HAL_hsusb.h"
#endif /* FEATURE_HS_USB_HAL */

/*==============================================================================
Forward Declarations
==============================================================================*/ 
static void hsu_chg_wallchg_detect(unsigned long detection_cb);

#ifndef FEATURE_HS_USB_BASIC
static void hsu_chg_wallchg_detect_init(
  hsu_chg_wallchg_detected_notif_fn detection_cb);

static void hsu_chg_wallchg_bus_mode_cb(
  hsu_bus_mode_enum new_mode, void* app_data);

static void hsu_chg_wallchg_bus_mode_register(void);
#endif /* FEATURE_HS_USB_BASIC */

/*==============================================================================
Constants and Macros
==============================================================================*/

#ifndef FEATURE_HS_USB_BASIC
/* Delay timer for wall charger detection in milliseconds */
#define HSU_CHGR_TIMER_INTERVAL       (500)

/* Maximum number of attempts for charger detection */
#define HSU_CHGR_DETECT_MAX_ATTEMPT   (10)
#endif /* FEATURE_HS_USB_BASIC */

/*==============================================================================
Local Variables
==============================================================================*/

#ifndef FEATURE_HS_USB_BASIC

/* USB Wall Charger detection timer information */
static rex_timer_type  hsu_chgr_detect_timer = {0};

/* USB Wall Charger detection retries */
static uint32 hsu_chgr_detect_attempt = 0;

#endif /* !FEATURE_HS_USB_BASIC */

/*==============================================================================

EXTERNAL FUNCTION DEFINITIONS FOR MODULE

==============================================================================*/

/*==============================================================================

FUNCTION hsu_chg_wallchg_detect_wall_charger

DESCRIPTION
The function detects the wall charger.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
At the end of the function, the callback is invoked only if the charger is
detected.
==============================================================================*/
void hsu_chg_wallchg_detect_wall_charger(
  hsu_chg_wallchg_detected_notif_fn detection_cb)
{
  HSU_ASSERT(detection_cb != NULL);

#ifndef FEATURE_HS_USB_BASIC
  /* Cancel any previous charger detection */
  hsu_chg_wallchg_cancel_detection();  

  /* Set number of retries if the USB charger is not detected and no RESET
   * is received from the PC Host
   */
  hsu_chgr_detect_attempt = HSU_CHGR_DETECT_MAX_ATTEMPT;

  /* For AMSS, arm Wall Charger Detection timer */
  hsu_chg_wallchg_detect_init(detection_cb);
#else
  hsu_chg_wallchg_detect((unsigned long)detection_cb);
#endif /* FEATURE_HS_USB_BASIC*/
}


/*==============================================================================

FUNCTION hsu_chg_wallchg_cancel_detection

DESCRIPTION
Requests this module to abort current request to detect presence of USB wall
charger. This usually will be done upon disconnection from an A-Device.
Does not do anything in the boot code.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
Clears charger detection timer and sets the retry count to 0
==============================================================================*/
void hsu_chg_wallchg_cancel_detection(void)
{ 
#ifndef FEATURE_HS_USB_BASIC
  if (hsu_chgr_detect_attempt != 0)
  {
    /* Cancel the timer */
    (void)rex_clr_timer(&hsu_chgr_detect_timer);
  }

  /* Reset the number of retries */
  hsu_chgr_detect_attempt = 0;  
#endif /* FEATURE_HS_USB_BASIC */
}


/*==============================================================================
INTERNAL FUNCTION DEFINITIONS FOR MODULE
==============================================================================*/

/*==============================================================================

FUNCTION hsu_chg_wallchg_detect

DESCRIPTION
This function will detect the USB Charger by reading link register.
If the Run/Stop bit is not set to '1', it will not be able to detect
USB charger.

Logic:
1. If R/S bit is set, D+ will be pulled high on USB cable connect.
2. If the USB charger is connected, D+ and D- is shorted.
3. Thus, both D+ and D- will be in high state (SE1 state).
4. There is PORTSC HS-USB Link register that reports the Line State.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
==============================================================================*/
static void hsu_chg_wallchg_detect(unsigned long detection_cb)
{
  uint32 portsc_ls_val;
  boolean is_usb_charger;

  HSU_ASSERT(detection_cb != NULL); 

#ifndef FEATURE_HS_USB_BASIC
  if (hsu_chgr_detect_attempt == 0)
  {
    /* Charger detection should not be attempted any more */
    return;
  }
#endif /* FEATURE_HS_USB_BASIC */

  /* Since D+ is already pulled high, read the line state */
  portsc_ls_val = HAL_hsusb_GetBits32(0, 
    HAL_HSUSB_PORTSC_ADDR(0), 
    HAL_HSUSB_PORTSC_LS_BMSK);

  /* SE1 is 11b, therefore same is LS_BMASK */
  is_usb_charger = (portsc_ls_val == HAL_HSUSB_PORTSC_LS_BMSK);

  if (is_usb_charger == TRUE)
  {
    hsu_chg_wallchg_cancel_detection();

    ((hsu_chg_wallchg_detected_notif_fn)detection_cb)(is_usb_charger); 
  }
  else
  {    
#ifndef FEATURE_HS_USB_BASIC
    hsu_chgr_detect_attempt--;

    if (hsu_chgr_detect_attempt > 0)
    {     
      /* Start timer for USB charger detection. */
      (void)rex_set_timer(&hsu_chgr_detect_timer, HSU_CHGR_TIMER_INTERVAL);  
    }
#endif /* FEATURE_HS_USB_BASIC */
  }
}


#ifndef FEATURE_HS_USB_BASIC
/*==============================================================================

FUNCTION hsu_chg_wallchg_detect_init

DESCRIPTION
This function will create a rex timer and set the timer to the delay value.
AMSS Only.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
==============================================================================*/
static void hsu_chg_wallchg_detect_init(
  hsu_chg_wallchg_detected_notif_fn detection_cb)
{
  static boolean is_initialized = FALSE;

  /* Below initialization should be done only once on the first call */
  if (is_initialized == FALSE)
  {
    /* Set a flag, so we do not initialize again */
    is_initialized = TRUE;

    /* Register for bus mode change event */
    hsu_chg_wallchg_bus_mode_register();    
  }
  
  /* Create a wall charger detection timer, pass the client's detection 
   * callback as the parameter
   */
  rex_def_timer_ex(&hsu_chgr_detect_timer,
    hsu_chg_wallchg_detect,
    (unsigned long)detection_cb);

  /* Start timer for wall charger detection. */
  (void)rex_set_timer(&hsu_chgr_detect_timer, HSU_CHGR_TIMER_INTERVAL);  
}


/*==============================================================================

FUNCTION hsu_chg_wallchg_bus_mode_cb

DESCRIPTION
This function is used for hsu_register_bus_mode_notif_cb() and invoked
whenever the HS-USB Bus Mode changes.  AMSS Only.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
==============================================================================*/
static void hsu_chg_wallchg_bus_mode_cb(
  hsu_bus_mode_enum new_mode, void* app_data)
{
  HSU_USE_PARAM(app_data);

  switch (new_mode)
  {
    /* USB cable disconnected */
    case HSU_BUS_MODE_DISCONNECTED:
    /* Bus mode will be in RESUMED state during RESET */
    case HSU_BUS_MODE_RESUMED:
    /* In OTG code, Bus may transit to CONFIGURED state */
    case HSU_BUS_MODE_CONFIGURED:   
    /* In OTG code, Bus may be SUSPENDED state if device is disabled */
    case HSU_BUS_MODE_SUSPENDED:
    {     
      /* Cancel any previous charger detection */
      hsu_chg_wallchg_cancel_detection();        
    }
    break;
  }
}


/*==============================================================================

FUNCTION hsu_chg_wallchg_bus_mode_register

DESCRIPTION
This function registers for HS-USB Bus Mode change event. AMSS Only.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
==============================================================================*/
static void hsu_chg_wallchg_bus_mode_register(void)
{ 
  hsu_bus_mode_cb_info_struct cb_info;

  /* Register callback to bus mode notification for handling bus states */ 
  cb_info.bus_mode_cb = hsu_chg_wallchg_bus_mode_cb;
  cb_info.app_data = NULL;

  hsu_register_bus_mode_notif_cb(cb_info);
}
#endif /* FEATURE_HS_USB_BASIC */

#else /* BUILD_HOSTDL */
#include "hsu_donothing.h"
#endif /* BUILD_HOSTDL */
