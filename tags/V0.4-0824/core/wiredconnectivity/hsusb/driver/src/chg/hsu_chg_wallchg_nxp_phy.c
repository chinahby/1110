/*==============================================================================

USB Wall Charger Detection API - I M P L E M E N T A T I O N   F I L E

GENERAL DESCRIPTION
Contains the API implementations for detecting the presence of a USB wall charger,
in case the USB PHY is NXP External PHY. 

EXTERNALIZED FUNCTIONS
hsu_chg_wallchg_detect_wall_charger

INITALIZATION AND SEQUENCING REQUIREMENTS
FEATURE_HS_USB_PHILIPS_PHY must be defined

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/
/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/chg/hsu_chg_wallchg_nxp_phy.c#7 $
$DateTime: 2008/09/08 01:39:07 $ $Author: evolfson $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
12/03/07 esh  Initial version

==============================================================================*/
#ifndef BUILD_HOSTDL /* ARMPRG does not contain charging */
/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "hsu_chg_wallchg.h"
#include "hsu_os_abstract_bus.h"

/* TODO: Need an abstract layer whether this is AMSS (rex) or OEMSBL (no rex) */

#ifdef FEATURE_HS_USB_BASIC
#include "boothw.h"
#else 
#include "rex.h"
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

static void wallchg_detect_timer_cb(unsigned long callback_param);
static void hsu_chg_wallchg_init(void);
static void hsu_chg_wallchg_cancel_detection(void);

#ifdef FEATURE_HS_USB_BASIC
static void wallchg_delay_ms(uint32 milliseconds);
#endif /* FEATURE_HS_USB_BASIC */

/*==============================================================================
Constants and Macros
==============================================================================*/
#define NXP_PHY_NON_DRV_MODE_BIT_3 (1 << 3)
#define NXP_PHY_NON_DRV_MODE_BIT_4 (1 << 4)

#define NXP_PHY_WALL_CHARGER_DETECTION_TIMER 50
#ifndef FEATURE_HS_USB_HAL
#define PORTSC_LS_SE1 0x3 /* 11 == undefined */
#endif /* FEATURE_HS_USB_HAL */

#define ULPI_NUM_OF_ATTEMPTS_TO_VERIFY_OPERATION 10000
/*==============================================================================
Typedefs
==============================================================================*/
#ifdef FEATURE_HS_USB_BASIC
  typedef struct  
  {
    hsu_chg_wallchg_detected_notif_fn wall_charger_detection_cb;
  } hsu_chg_wallchg_static_data_type;
#else /* !FEATURE_HS_USB_BASIC */
  typedef struct  
  {
    rex_timer_type                    wall_charger_timer;
    hsu_chg_wallchg_detected_notif_fn wall_charger_detection_cb;
  } hsu_chg_wallchg_static_data_type;
#endif /* FEATURE_HS_USB_BASIC */

/*==============================================================================
Variables
==============================================================================*/
static hsu_chg_wallchg_static_data_type hsu_chg_wallchg_static_data;

/*==============================================================================

EXTERNAL FUNCTION DEFINITIONS FOR MODULE

==============================================================================*/

/*===========================================================================
FUNCTION hsu_chg_wallchg_detect_wall_charger()
===========================================================================*/
void hsu_chg_wallchg_detect_wall_charger(
  hsu_chg_wallchg_detected_notif_fn detection_cb)
{
  #ifdef FEATURE_HS_USB_BASIC
  uint32 dummy_callback_param = 0;
  #endif /* FEATURE_HS_USB_BASIC */
  
  if (detection_cb == NULL)
  {
    HSU_MSG_HIGH("*_detect_wall_charger: no callback supplied, doing nothing!", 
      0, 0, 0);
    return;
  }

  if (!wallchg_was_init)
  {
    HSU_MSG_ERROR("*_detect_wall_charger: invoked without calling first *_init", 
      0, 0, 0);
    return;
  }

  hsu_chg_wallchg_init();
  hsu_chg_wallchg_static_data.wall_charger_detection_cb = detection_cb;

#ifdef FEATURE_HS_USB_BASIC
  wallchg_delay_ms(NXP_PHY_WALL_CHARGER_DETECTION_TIMER);
  wallchg_detect_timer_cb(dummy_callback_param); 
#else /* !FEATURE_HS_USB_BASIC */
  /* Start timer for wall charger detection.
  We need to wait some time before checking the LS of the PORTSC reg. */
  (void)rex_set_timer(&(hsu_chg_wallchg_static_data.wall_charger_timer),
    NXP_PHY_WALL_CHARGER_DETECTION_TIMER);
#endif /* FEATURE_HS_USB_BASIC */
}

/*==============================================================================

INTERNAL FUNCTION DEFINITIONS FOR MODULE

==============================================================================*/

/*===========================================================================

FUNCTION wallchg_detect_timer_cb

DESCRIPTION
Timer callback to be invoked when hsu_chg_wallchg_static_data.wall_charger_timer
expires. Checks if a USB wall charger was detected and if so, invokes the callback
registered.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static void wallchg_detect_timer_cb(unsigned long callback_param)
{
#ifdef FEATURE_HS_USB_HAL
  uint32 portsc_ls_val;
#else /* FEATURE_HS_USB_HAL */
  volatile uint32 portsc_ls_val;
#endif /* FEATURE_HS_USB_HAL */
  boolean is_wall_charger = FALSE;

  HSU_USE_PARAM(callback_param);

#ifdef FEATURE_HS_USB_HAL
  portsc_ls_val = HAL_hsusb_GetBits32(0, HAL_HSUSB_PORTSC_ADDR(0), HAL_HSUSB_PORTSC_LS_BMSK);
  is_wall_charger = (portsc_ls_val == HAL_HSUSB_PORTSC_LS_BMSK);
#else /* FEATURE_HS_USB_HAL */
  portsc_ls_val = HWIO_INF(USB_OTG_HS_PORTSC, LS);
  is_wall_charger = (portsc_ls_val == PORTSC_LS_SE1);
#endif /* FEATURE_HS_USB_HAL */

  /* Hypothetical, as this was checked inside hsu_chg_wallchg_detect_wall_charger. */
  if (hsu_chg_wallchg_static_data.wall_charger_detection_cb == NULL)
  {
    ERR("wallchg_detect_timer_cb: no wall charger detec callback supplied!",
      0, 0, 0);
    HSU_ASSERT(hsu_chg_wallchg_static_data.wall_charger_detection_cb != NULL);
    return;
  }

  hsu_chg_wallchg_cancel_detection();
  hsu_chg_wallchg_static_data.wall_charger_detection_cb(is_wall_charger);
  hsu_chg_wallchg_static_data.wall_charger_detection_cb = NULL;
}

/*===========================================================================
FUNCTION hsu_chg_wallchg_init()
===========================================================================*/
static void hsu_chg_wallchg_init(void)
{

  uint8 ulpi_read_value;
  boolean assert_result;

#ifndef FEATURE_HS_USB_BASIC
  /* Init the wall charger detection timer */
  rex_def_timer_ex(&(hsu_chg_wallchg_static_data.wall_charger_timer),
    wallchg_detect_timer_cb,
    0);  
#endif /* FEATURE_HS_USB_BASIC */

  /* Place PHY in non driving mode (opmode = 01). Other bits are not affected. */
  /* Read current value: */

  assert_result = hsu_os_bus_read_from_phy(0x04, &ulpi_read_value);
  HSU_ASSERT(assert_result);

  ulpi_read_value |= NXP_PHY_NON_DRV_MODE_BIT_3; /* Cause bit no. 3 to become '1' */
  ulpi_read_value &= ~NXP_PHY_NON_DRV_MODE_BIT_4; /* Cause bit no. 4 to become '0' */

  assert_result = hsu_os_bus_write_to_phy(0x04, ulpi_read_value);
  HSU_ASSERT(assert_result);

  wallchg_was_init = TRUE;
}

/*===========================================================================
FUNCTION hsu_chg_wallchg_cancel_detection()
===========================================================================*/
static void hsu_chg_wallchg_cancel_detection(void)
{
#ifndef FEATURE_HS_USB_BASIC
  /* Cancel any pending entry of the wall charger if the cable was disconnected. */
  (void)rex_clr_timer(&(hsu_chg_wallchg_static_data.wall_charger_timer));
#endif /* FEATURE_HS_USB_BASIC */
}

#ifdef FEATURE_HS_USB_BASIC
/*===========================================================================

FUNCTION wallchg_delay_ms

DESCRIPTION
Implements a delay for the duration of the specified value

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
Waits until the Viewport acknowledges the operation went through
===========================================================================*/
static void wallchg_delay_ms(uint32 millisec)
{
	volatile uint32 i;
    BOOTHW_KICK_WATCHDOG();
    /* If needs to wait for more than 1000 msecs, wait in 1000 usecs chunks,
       and kick the wathcdog in between */
    for (i=0; i< millisec; i++)
    {
        clk_busy_wait(1000);   
        BOOTHW_KICK_WATCHDOG();
    }  
}

#endif /* FEATURE_HS_USB_BASIC */
#endif /* BUILD_HOSTDL */
