/*===========================================================================


hsu_pmic_otg_if.c

DESCRIPTION :                                       

High Speed USB PMIC inter operation definition.

This file contains call back funcion implementation of PMIC for HS USB

Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

EDIT HISTORY FOR FILE

$Header:
$DateTime:
=============================================================================
when      who  what, where, why
--------  ---  -------------------------------------------------------------
06/04/08  tk   File created. 

---------------------------------------------------------------------------*/

/*===========================================================================

INCLUDE FILES

===========================================================================*/
#ifndef T_REXNT
#include "customer.h"
#include "pm.h"
#include "pm66xxirqs.h"
#include "pmapp_otg.h"
#include "hsu_common.h"
#include "hsu_os_abstract_bus_ext.h"
#include "hsu_conf_sel_stack_utils.h"
#include "hsu_bus_mode_notifier.h"
#include "hsu_chg_api_helper.h"
#include "hsu_os_abstract_bus.h"

/*==============================================================================
Constants
==============================================================================*/
#ifndef HSU_PMIC_OTG_IF_CORE_IDX
/* HS USB core index of hsu_core_table in hsu_os_abstract_bus.c  for PMIC PHY 
 * For ULC target, it should index 0. The value can vary through targets in the 
 * future.
 */
#define HSU_PMIC_OTG_IF_CORE_IDX 0
#endif /* HSU_PMIC_OTG_IF_CORE_IDX */

/*==============================================================================
FUNCTION hsu_pmic_otg_if_init_done_cb

DESCRIPTION

DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS

==============================================================================*/
static void hsu_pmic_otg_if_init_done_cb(void)
{
  /* acquire bus forever for next device connection  */
  pm_app_otg_relinquish_bus();
  pm_app_otg_acquire_bus(-1);
}

/*==============================================================================
FUNCTION hsu_pmic_otg_if_host_mode_init_cb

DESCRIPTION

DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS

==============================================================================*/
static void hsu_pmic_otg_if_host_mode_init_cb(pm_otg_usb_speed_type speed)
{
}

/*==============================================================================
FUNCTION hsu_pmic_otg_if_peripheral_mode_init_cb

DESCRIPTION

DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS

==============================================================================*/
static void hsu_pmic_otg_if_peripheral_mode_init_cb(void)
{
#ifdef FEATURE_HS_USB_PMIC_PHY
  /* Send connect event to HS-USB core. */
  hsu_os_bus_handle_pmic_event(HSU_PMIC_OTG_IF_CORE_IDX, 
                               HSU_OS_BUS_CONNECT_EVT);
#endif /* FEATURE_HS_USB_PMIC_PHY */

#ifdef FEATURE_HS_USB_LEGACY_DEVICE
  /* Send connect event to HS-USB core. */
  hsu_os_bus_handle_pmic_event(FS_USB_CORE_INDEX, 
                               HSU_OS_BUS_CONNECT_EVT);
#endif /* FEATURE_HS_USB_LEGACY_DEVICE */
}

/*==============================================================================
FUNCTION hsu_pmic_otg_if_rs232dongle_mode_init_cb

DESCRIPTION

DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS

==============================================================================*/
static void hsu_pmic_otg_if_rs232dongle_mode_init_cb(void)
{
}

/*==============================================================================
FUNCTION hsu_pmic_otg_if_carkitclient_mode_init_cb

DESCRIPTION

DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS

==============================================================================*/
static void hsu_pmic_otg_if_carkitclient_mode_init_cb(void)
{
}

/*==============================================================================
FUNCTION hsu_pmic_otg_if_carkitclient_int_hdlr

DESCRIPTION

DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS

==============================================================================*/
static void hsu_pmic_otg_if_carkitclient_int_hdlr(void)
{
}

/*==============================================================================
FUNCTION hsu_pmic_otg_if_carkitclient_mode_entry_rsp_fn

DESCRIPTION

DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS

==============================================================================*/
static void hsu_pmic_otg_if_carkitclient_mode_entry_rsp_fn(pm_app_otg_carkit_mode_type mode)
{
}

/*==============================================================================
FUNCTION hsu_pmic_otg_if_remote_dev_lost_cb

DESCRIPTION

DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS

==============================================================================*/
static void hsu_pmic_otg_if_remote_dev_lost_cb(void)
{
#ifdef FEATURE_HS_USB_PMIC_PHY
  /* Send disconnect event to HS-USB core. */
  hsu_os_bus_handle_pmic_event(HSU_PMIC_OTG_IF_CORE_IDX, 
                               HSU_OS_BUS_DISCONNECT_EVT);
#endif /* FEATURE_HS_USB_PMIC_PHY */

#ifdef FEATURE_HS_USB_LEGACY_DEVICE
  /* Send disconnect event to HS-USB core. */
  hsu_os_bus_handle_pmic_event(FS_USB_CORE_INDEX, 
                               HSU_OS_BUS_DISCONNECT_EVT);
#endif /* FEATURE_HS_USB_LEGACY_DEVICE */

  /* acquire buf forever for next device connection  */
  pm_app_otg_relinquish_bus();
  pm_app_otg_acquire_bus(-1);
}

/*==============================================================================
FUNCTION hsu_pmic_otg_if_remote_dev_resumed_cb

DESCRIPTION

DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS

==============================================================================*/
static void hsu_pmic_otg_if_remote_dev_resumed_cb(void)
{
#ifdef FEATURE_HS_USB_PMIC_PHY
  /* Send resume event to HS-USB core. */
  hsu_os_bus_handle_pmic_event(HSU_PMIC_OTG_IF_CORE_IDX, 
    HSU_OS_BUS_RESUME_EVT);
#endif /* FEATURE_HS_USB_PMIC_PHY */
}

/*==============================================================================
FUNCTION hsu_pmic_otg_if_remote_host_wakeup_fn

DESCRIPTION

DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS

==============================================================================*/
static void hsu_pmic_otg_if_remote_host_wakeup_fn(void)
{
}

/*==============================================================================
FUNCTION hsu_pmic_otg_if_error_notification_cb

DESCRIPTION

DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS

==============================================================================*/
static void hsu_pmic_otg_if_error_notification_cb(pm_app_otg_operational_error_type error)
{
}

/*==============================================================================
FUNCTION hsu_pmic_otg_if_sleep_assert_fn

DESCRIPTION

DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS

==============================================================================*/
static void hsu_pmic_otg_if_sleep_assert_fn(boolean assert_sleep)
{
}

/*==============================================================================

FUNCTION hsu_pmic_otg_if_bus_mode_cb

DESCRIPTION
  This function is bus mode call back for client HS-USB interfacing PMIC OTG 

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
static void hsu_pmic_otg_if_bus_mode_cb(hsu_bus_mode_enum new_mode, void* app_data)
{
  /* The static variable tracks if the device usb bus is in the suspended 
     state.  Initial value will be assumed that bus is in suspended state. */
  static boolean suspended = TRUE;
  
  /* TODO: hostSuspended needs to be set to FALSE and 
     pm_app_otg_remote_a_dev_info_type does not need 
     to be initialized when pmapp API has been ported. */
  pm_app_otg_remote_a_dev_info_type info; 

  HSU_USE_PARAM(app_data);

  switch (new_mode)
  {
    case HSU_BUS_MODE_RESUMED:
      suspended = FALSE;
      pm_app_otg_resume();
      break;

    case HSU_BUS_MODE_SUSPENDED:
      suspended = TRUE;
      pm_app_otg_suspend();
      break;

    case HSU_BUS_MODE_CONFIGURED:
      if (FALSE == suspended)
      {
        // Notify PMIC maximum current when device is configured
        info.remote_a_dev = PM_APP_OTG_A_DEV_TYPE__USB_HOST;
        info.max_i_ma = HSU_CHG_API_HELPER_MAX_POWER;
        pm_app_otg_set_remote_a_dev_info(info);
      }
      break;

    case HSU_BUS_MODE_UNCONFIGURED:
      if (FALSE == suspended)
      {
        // Notify PMIC "unconfigured" current when device is not configured
        info.remote_a_dev = PM_APP_OTG_A_DEV_TYPE__USB_HOST;
        info.max_i_ma = HSU_CHG_API_HELPER_POWER_UNCONFIGURED;
        pm_app_otg_set_remote_a_dev_info(info);
      }
      break;
  }   
}

/*==============================================================================

FUNCTION hsu_pmic_otg_if_init

DESCRIPTION
  This function initialize and start PMIC OTG state machine.

DEPENDENCIES
  FEATURE_HS_USB_BASIC does not support the PMIC interrupt and shouldn't use 
  the OTG state machine. Instead, the D+/D- will be set as FS mode in 
  hsu_api_init_phy for FEATURE_HS_USB_BASIC.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void hsu_pmic_otg_if_init(void)
{
  hsu_bus_mode_cb_info_struct bus_mode_cb_info;

  /* Register callback to bus mode notification for handling bus states */ 
  bus_mode_cb_info.bus_mode_cb = hsu_pmic_otg_if_bus_mode_cb;
  bus_mode_cb_info.app_data = NULL;

  hsu_register_bus_mode_notif_cb(bus_mode_cb_info);

  pm_app_otg_init(hsu_pmic_otg_if_init_done_cb,
                  hsu_pmic_otg_if_host_mode_init_cb,
                  hsu_pmic_otg_if_peripheral_mode_init_cb,
                  hsu_pmic_otg_if_rs232dongle_mode_init_cb,
                  hsu_pmic_otg_if_carkitclient_mode_init_cb,
                  hsu_pmic_otg_if_carkitclient_int_hdlr,
                  hsu_pmic_otg_if_carkitclient_mode_entry_rsp_fn,
                  hsu_pmic_otg_if_remote_dev_lost_cb,
                  hsu_pmic_otg_if_remote_dev_resumed_cb,
                  hsu_pmic_otg_if_remote_host_wakeup_fn,
                  hsu_pmic_otg_if_error_notification_cb,
                  hsu_pmic_otg_if_sleep_assert_fn);
}

#endif /* ! T_REXNT */
