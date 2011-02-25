/*==============================================================================

Charging Module to Interface with the High Speed USB
H E A D E R   F I L E

GENERAL DESCRIPTION
Contains the implementation of the API for chgpm module needed to allow charging 
via HS-USB.

EXTERNALIZED FUNCTIONS
hsu_chg_init
hsu_chg_configure_usb_power_consumer
hsu_chg_set_usb_power_requirement
hsu_chg_invoke_connected_cb
hsu_chg_invoke_disconnected_cb
hsu_chg_invoke_avail_cb
hsu_chg_invoke_not_avail_cb
hsu_chg_is_connected

INITALIZATION AND SEQUENCING REQUIREMENTS
None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/
/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/chg/hsu_chg.c#6 $
$DateTime: 2009/02/11 06:48:45 $ $Author: ablay $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
05/25/07 esh  Initial version

==============================================================================*/
/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "hsu_chg.h"

#include "customer.h"  /* For FEATURE definitions */
#include "hsu_apis.h"
#include "pm.h"
#include "hsu_common.h" /* For HSU_MSG_HIGH, HSU_USE_PARAM */

#include "rex.h" /* For rex_crit_sect_type */
#include "comdef.h" /* For TASKLOCK/TASKFREE */
/*==============================================================================

LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
Constants and Macros
==============================================================================*/
/*==============================================================================
Typedefs
==============================================================================*/
typedef struct 
{
  /* Critical section to protect against multiple contexts accessing this data type.
  For example: TMC initializing the charger module while RPC invoking hsu_mdm apis
  forwarding USB events for this module. */
  rex_crit_sect_type chg_static_data_access_guard;

  /* Charger callbacks: */
  pm_app_otg_usb_power_source_connected_notification_cb_type hsu_chg_conn_cb;
  pm_app_otg_usb_power_source_disconnected_notification_cb_type hsu_chg_disconn_cb;
  pm_app_otg_usb_power_sink_open_notification_cb_type hsu_chg_is_avail_cb;
  pm_app_otg_usb_power_sink_close_notification_cb_type hsu_chg_not_avail_cb;

  /* Indication whether or not the charger wishes boost from the PMIC */
  boolean pwr_ctrl_was_requested;
  /* Indication whether or not the charger is requesting charging via USB */
  boolean chgr_is_via_usb;
  /* Indication whether or not this USB device is connected to host */
  boolean hsu_dev_is_connected;
  /* Indication whether or not this USB device is resumed */
  boolean hsu_dev_is_resumed;
  /* The amount allowed to draw, in mA */
  uint32 hsu_dev_allowed_amount_to_draw;
} hsu_chg_static_data_type;

/*==============================================================================
Variables
==============================================================================*/
static hsu_chg_static_data_type hsu_chg_static_data;

/* Indication whether hsu_chg_init was already called 
(to prevent usage of the above crit section prior to init). */
static boolean hsu_chg_already_init = FALSE;
/* To catch hsu_chg_set_usb_power_requirement being called before 
hsu_chg_configure_usb_power_consumer. */
static boolean configure_usb_power_consumer_was_called = FALSE;
/*---------------------------------------------------------------------------
FORWARD DECLARATIONS
---------------------------------------------------------------------------*/

/*==============================================================================

EXTERNAL FUNCTION DEFINITIONS FOR MODULE

==============================================================================*/
/*===========================================================================
FUNCTION hsu_chg_init()
===========================================================================*/
void hsu_chg_init
(
  void
)
{
  TASKLOCK();
  if (hsu_chg_already_init == TRUE)
  {
    HSU_MSG_HIGH("hsu_chg_init: already called, doing nothing", 
      0, 0, 0);
  }
  else
  {
    hsu_chg_already_init = TRUE;

    /* Init crit section */
    rex_init_crit_sect(&(hsu_chg_static_data.chg_static_data_access_guard));

    /* Init callbacks */
    hsu_chg_configure_usb_power_consumer(NULL, NULL, NULL, NULL);

    hsu_chg_static_data.pwr_ctrl_was_requested = FALSE;

    hsu_chg_static_data.chgr_is_via_usb = FALSE;

    hsu_chg_static_data.hsu_dev_is_connected = FALSE;
    hsu_chg_static_data.hsu_dev_is_resumed = FALSE;
    hsu_chg_static_data.hsu_dev_allowed_amount_to_draw = 0;

  }
  TASKFREE();
}

/*===========================================================================
FUNCTION hsu_chg_configure_usb_power_consumer()
===========================================================================*/
void hsu_chg_configure_usb_power_consumer
(
  pm_app_otg_usb_power_source_connected_notification_cb_type connected_cb,
  pm_app_otg_usb_power_source_disconnected_notification_cb_type disconned_cb,
  pm_app_otg_usb_power_sink_open_notification_cb_type chg_is_avail_cb,
  pm_app_otg_usb_power_sink_close_notification_cb_type chg_not_avail_cb
)
{
  /* Init this module, if not done already */
  hsu_chg_init();

  hsu_chg_static_data.hsu_chg_conn_cb = connected_cb;
  hsu_chg_static_data.hsu_chg_disconn_cb = disconned_cb;
  hsu_chg_static_data.hsu_chg_is_avail_cb = chg_is_avail_cb;
  hsu_chg_static_data.hsu_chg_not_avail_cb = chg_not_avail_cb;

  configure_usb_power_consumer_was_called = TRUE;
}

/*===========================================================================
FUNCTION hsu_chg_set_usb_power_requirement()
===========================================================================*/
void hsu_chg_set_usb_power_requirement
(
  boolean is_persistent_charging
)
{
  /* Assuming hsu_chg_configure_usb_power_consumer was called prior to this 
  function. */
  HSU_ASSERT(configure_usb_power_consumer_was_called);

  rex_enter_crit_sect(&(hsu_chg_static_data.chg_static_data_access_guard));

  hsu_chg_static_data.chgr_is_via_usb = is_persistent_charging;
  if (is_persistent_charging)
  {
    if (hsu_chg_static_data.hsu_dev_is_connected)
    {
      hsu_chg_static_data.hsu_dev_is_connected = FALSE;
        /* To prevent hitting the assert in hsu_chg_invoke_connected_cb */
      hsu_chg_invoke_connected_cb(FALSE);
      if (hsu_chg_static_data.hsu_dev_is_resumed)
      {
        if (hsu_chg_static_data.hsu_dev_allowed_amount_to_draw == 0)
        {
          HSU_MSG_HIGH("*_set_usb_power_requirement: allowed_chrg_amount is 0", 
            0, 0, 0);
        }
        hsu_chg_invoke_avail_cb(hsu_chg_static_data.hsu_dev_allowed_amount_to_draw);
      }
      else /* !hsu_chg_static_data.hsu_dev_is_resumed */
      {
        HSU_MSG_MED("*_set_usb_power_requirement: requested charging, but suspended",
          0, 0, 0);
      }
    }
  }

  rex_leave_crit_sect(&(hsu_chg_static_data.chg_static_data_access_guard));
}

/*===========================================================================
FUNCTION hsu_chg_invoke_connected_cb()
===========================================================================*/
void hsu_chg_invoke_connected_cb
(
 boolean is_wall_charger /*the device type is charger or not. */
)
{
  pm_app_otg_a_dev_type source = PM_APP_OTG_A_DEV_TYPE__USB_HOST;

  HSU_ASSERT(hsu_chg_already_init);

  rex_enter_crit_sect(&(hsu_chg_static_data.chg_static_data_access_guard));

  if (hsu_chg_static_data.hsu_dev_is_connected)
  {
    rex_leave_crit_sect(&(hsu_chg_static_data.chg_static_data_access_guard));

    HSU_MSG_MED("*_invoke_connected_cb: Warning, Device already connected.", 0, 0, 0);
    return ;
  }
  hsu_chg_static_data.hsu_dev_is_connected = TRUE;

  if (!hsu_chg_static_data.chgr_is_via_usb)
  {
    /* Callback was used just to update whether the hsu dev is connected to host,
    however charger module is not yet connected.*/

    rex_leave_crit_sect(&(hsu_chg_static_data.chg_static_data_access_guard));

    return ;
  }

  /* For now, only connecting to host PC */
  if (hsu_chg_static_data.hsu_chg_conn_cb == NULL)
  {
    rex_leave_crit_sect(&(hsu_chg_static_data.chg_static_data_access_guard));

    HSU_MSG_HIGH("*_invoke_connected_cb: Warning, callback is NULL.", 0, 0, 0);
    return ;
  }

  rex_leave_crit_sect(&(hsu_chg_static_data.chg_static_data_access_guard));

  if (is_wall_charger)
  {
    source = PM_APP_OTG_A_DEV_TYPE__USB_CHARGER;
  }
  hsu_chg_static_data.hsu_chg_conn_cb(source);
}

/*===========================================================================
FUNCTION hsu_chg_invoke_disconnected_cb()
===========================================================================*/
void hsu_chg_invoke_disconnected_cb(void)
{
  HSU_ASSERT(hsu_chg_already_init);

  rex_enter_crit_sect(&(hsu_chg_static_data.chg_static_data_access_guard));

  if (hsu_chg_static_data.hsu_dev_is_connected == FALSE)
  {
    rex_leave_crit_sect(&(hsu_chg_static_data.chg_static_data_access_guard));

    return;
  }
  //HSU_ASSERT(hsu_chg_static_data.hsu_dev_is_connected == TRUE);
  hsu_chg_static_data.hsu_dev_is_connected = FALSE;

  if (!hsu_chg_static_data.chgr_is_via_usb)
  {
    /* Callback was used just to update whether the hsu dev is connected to host,
    however charger module is not yet connected.*/

    rex_leave_crit_sect(&(hsu_chg_static_data.chg_static_data_access_guard));

    return ;
  }

  rex_leave_crit_sect(&(hsu_chg_static_data.chg_static_data_access_guard));

  if (hsu_chg_static_data.hsu_chg_disconn_cb == NULL)
  {
    HSU_MSG_HIGH("*_invoke_disconnected_cb: Warning, callback is NULL.", 0, 0, 0);
    return ;
  }

  hsu_chg_static_data.hsu_chg_disconn_cb();
}

/*===========================================================================
FUNCTION hsu_chg_invoke_avail_cb()
===========================================================================*/
void hsu_chg_invoke_avail_cb(uint32 amount_current_to_draw)
{
  HSU_ASSERT(hsu_chg_already_init);

  rex_enter_crit_sect(&(hsu_chg_static_data.chg_static_data_access_guard));

  /* This cb may be invoked twice in a row (e.g. dev resumed but not yet configured)
  so no assertion here... */
  hsu_chg_static_data.hsu_dev_is_resumed = TRUE;

  hsu_chg_static_data.hsu_dev_allowed_amount_to_draw = amount_current_to_draw;

  if (!hsu_chg_static_data.chgr_is_via_usb)
  {
    /* Callback was used just to update whether the hsu dev is resumed,
    however charger module is not yet connected.*/

    rex_leave_crit_sect(&(hsu_chg_static_data.chg_static_data_access_guard));
    return ;
  }

  if (hsu_chg_static_data.hsu_chg_is_avail_cb == NULL)
  {
    rex_leave_crit_sect(&(hsu_chg_static_data.chg_static_data_access_guard));

    HSU_MSG_HIGH("*_invoke_avail_cb: Warning, callback is NULL.", 0, 0, 0);
    return ;
  }

  rex_leave_crit_sect(&(hsu_chg_static_data.chg_static_data_access_guard));

  hsu_chg_static_data.hsu_chg_is_avail_cb(amount_current_to_draw);
}

/*===========================================================================
FUNCTION hsu_chg_invoke_not_avail_cb()
===========================================================================*/
void hsu_chg_invoke_not_avail_cb(void)
{
  HSU_ASSERT(hsu_chg_already_init);

  rex_enter_crit_sect(&(hsu_chg_static_data.chg_static_data_access_guard));

  /* This cb may be invoked twice in a row (e.g. dev powered up without USB cable
  and then receiving suspend upon cable connection), so no assertion here... */
  hsu_chg_static_data.hsu_dev_is_resumed = FALSE;

  if (!hsu_chg_static_data.chgr_is_via_usb)
  {
    /* Callback was used just to update whether the hsu dev is resumed,
    however charger module is not yet connected.*/

    rex_leave_crit_sect(&(hsu_chg_static_data.chg_static_data_access_guard));

    return ;
  }

  if (hsu_chg_static_data.hsu_chg_not_avail_cb == NULL)
  {
    rex_leave_crit_sect(&(hsu_chg_static_data.chg_static_data_access_guard));

    HSU_MSG_HIGH("*_invoke_not_avail_cb: Warning, callback is NULL.", 0, 0, 0);
    return ;
  }

  rex_leave_crit_sect(&(hsu_chg_static_data.chg_static_data_access_guard));

  hsu_chg_static_data.hsu_chg_not_avail_cb();
}

/*===========================================================================
FUNCTION hsu_chg_is_connected()
===========================================================================*/
boolean hsu_chg_is_connected
(
  void
)
{
  HSU_ASSERT(hsu_chg_already_init);

  return hsu_chg_static_data.chgr_is_via_usb;
}

/*==============================================================================

INTERNAL FUNCTION DEFINITIONS FOR MODULE

==============================================================================*/
