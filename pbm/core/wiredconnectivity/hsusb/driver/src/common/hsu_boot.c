/*==============================================================================

                            HS-USB API to BOOT Module 

GENERAL DESCRIPTION
    Contains the API implementation for BOOT to allow interfacing with HS-USB.

EXTERNALIZED FUNCTIONS
    hsu_boot_start_stack
    hsu_boot_stop_stack
    hsu_boot_poll_stack
    hsu_boot_disconnect_from_host

INITALIZATION AND SEQUENCING REQUIREMENTS
    None

Copyright (c) 2007-2008 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/common/hsu_boot.c#13 $
$DateTime: 2008/08/25 06:21:42 $ $Author: ablay $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
12/27/07 esh  Initial version

==============================================================================*/
#ifndef BUILD_HOSTDL /* ARMPRG does not contain this BOOT API */
/*==============================================================================
INCLUDE FILES FOR MODULE
==============================================================================*/
#include "hsu_boot.h"
#include "hsu_conf_sel_stack_utils.h"
#include "hsu_conf_sel_comp_utils.h"
#include "jslave_init.h"
#include "hsu_bus_mode_notifier.h"
#include "hsu_common.h"
#include "hsu_mdm_apis.h"
#include "hsu_phy_utils.h"

#if !defined(FEATURE_HSU_TEST)
#include "hsu_chg_wallchg.h"
#endif /* !FEATURE_HSU_TEST */

#ifndef FEATURE_HSU_TEST
#include "hsu_os_abstract_bus.h"
#include "msm.h" /* msm.h takes care of including msmhwio.h and msmhwioreg.h */
#include "hsu_conf_sel_core_config.h"
#else
#error code not present
#endif /* FEATURE_HSU_TEST */

/*==============================================================================
LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
==============================================================================*/
/*==============================================================================
Constants
==============================================================================*/
/*==============================================================================
Macros
==============================================================================*/
/*==============================================================================
Typedefs
==============================================================================*/
/*==============================================================================
Variables
==============================================================================*/
static hsu_bus_mode_enum hsu_boot_curr_mode = HSU_BUS_MODE_RESUMED; 
  /* Defaulting to Resumed as default mode, as the device never starts from this state.*/
static boolean hsu_boot_was_already_enumerated = FALSE;
static boolean hsu_boot_is_wall_charger = FALSE;
static boolean hsu_boot_first_poll_done = FALSE;

/*==============================================================================
Forward Declarations
==============================================================================*/
static void hsu_boot_state_changed_cb(hsu_bus_mode_enum new_mode, 
                                      void* app_data);

static hsu_boot_poll_result hsu_boot_match_mode_to_state(hsu_bus_mode_enum mode);

static void hsu_boot_wall_chg_detect_cb(boolean is_wall_chg);

/*==============================================================================
EXTERNAL FUNCTION DEFINITIONS FOR MODULE
==============================================================================*/
/*===========================================================================
FUNCTION hsu_boot_start_stack()
===========================================================================*/
boolean hsu_boot_start_stack(void)
{
    hsu_bus_mode_cb_info_struct stack_state_change_callback_info;
    jresult_t setup_stack_ret_val = 0;
#ifndef FEATURE_HSU_TEST
    struct hsu_device_params_type* boot_core_conf_sel_info = NULL;
#endif /* FEATURE_HSU_TEST */
  
    HSU_MSG_HIGH("hsu_boot_start_stack:entered", 0,0,0);

    /* Init static variables */
    hsu_boot_was_already_enumerated = FALSE;
    hsu_boot_is_wall_charger = FALSE;
    hsu_boot_first_poll_done = FALSE;

    /* Defaulting to Resumed as default mode, as the device never starts from 
       this state. */
    hsu_boot_curr_mode = HSU_BUS_MODE_RESUMED; 

    /* Register a callback for bus mode notifications */
    stack_state_change_callback_info.bus_mode_cb =  hsu_boot_state_changed_cb;
    stack_state_change_callback_info.app_data = NULL;

    if (hsu_register_bus_mode_notif_cb(stack_state_change_callback_info) 
        == FALSE)
    {
        HSU_ASSERT("hsu_boot_start_stack(): hsu_register_bus_mode_notif_cb failed" 
          == NULL);
    }

    /* Init memory and PHY */

#ifndef FEATURE_HSU_TEST
    hsu_conf_sel_core_config_set_core_info();
    hsu_conf_sel_stack_utils_init_memory(); 
    hsu_api_init_phy();

    boot_core_conf_sel_info = hsu_conf_sel_core_config_get_boot_core_conf_sel_info();
    if (boot_core_conf_sel_info == NULL)
    {
      return FALSE;
    }

    /* Initialize variables before calling the setup stack routine */
    memset(boot_core_conf_sel_info->configs_arr[HSU_CONFIG_IDX_0].num_of_fd_instances, 
            0, 
            sizeof(boot_core_conf_sel_info->configs_arr[HSU_CONFIG_IDX_0].num_of_fd_instances));
    boot_core_conf_sel_info->configs_arr[HSU_CONFIG_IDX_0].num_of_fd_instances[HSU_CFG_SEL_HID_FD] = 1;
    boot_core_conf_sel_info->product_id = HSU_PRODUCT_ID_HID;
    
    /* Start the USB device stack, enumerated as generic HID device */
    setup_stack_ret_val = hsu_conf_sel_stack_utils_setup_stack();

#endif /* FEATURE_HSU_TEST */

    HSU_MSG_HIGH("hsu_boot_start_stack:exit", 0,0,0);
    return (setup_stack_ret_val == HSU_JSUCCESS );
}

/*===========================================================================
FUNCTION hsu_boot_stop_stack()
===========================================================================*/
void hsu_boot_stop_stack(void)
{
    hsu_bus_mode_cb_info_struct stack_state_change_callback_info;

    HSU_MSG_HIGH("hsu_boot_stop_stack:entered", 0,0,0);

    /* The callback for bus mode notifications to be unregistered */
    stack_state_change_callback_info.bus_mode_cb =  hsu_boot_state_changed_cb;
    stack_state_change_callback_info.app_data = NULL;

    hsu_unregister_bus_mode_notif_cb(stack_state_change_callback_info);

#ifndef FEATURE_HSU_TEST
    /* Stop the HS-USB stack */
    jslave_disable(0);
    jstop_stack();
#endif /* FEATURE_HSU_TEST */
}

/*===========================================================================
FUNCTION hsu_boot_poll_stack()
===========================================================================*/
hsu_boot_poll_result hsu_boot_poll_stack(void)
{
    /* Saves the bus mode state before polling & handling interrupts */
    hsu_bus_mode_enum prev_mode = hsu_boot_curr_mode;

    HSU_MSG_HIGH("hsu_boot_poll_stack:entered", 0,0,0);

    /* Check if wall charger is connected initially
       This is a workaround to the bug/feature that there is NO interrupt 
       when booting with USB wall charger connected. */
    if (!hsu_boot_first_poll_done)
    {
      hsu_boot_first_poll_done = TRUE;

      /* The hsu_boot_wall_chg_detect_cb will be invoked before this 
      function call returns */
      hsu_chg_wallchg_detect_wall_charger(hsu_boot_wall_chg_detect_cb);
      if (hsu_boot_is_wall_charger)
      {
        hsu_boot_curr_mode = HSU_BUS_MODE_CONNECTED;
        return HSU_BOOT_POLL_RESULT_CONNECTED_TO_WALL_CHG;
      }
      else
      {
        if (!hsu_dload_sess_is_valid())
        {
          hsu_boot_curr_mode = HSU_BUS_MODE_DISCONNECTED;
          return HSU_BOOT_POLL_RESULT_NOT_CONNECTED;
        }
      }
    }

    /* Poll the HW for interrupts, and handle them if necessary */
    if (hsu_os_bus_poll_interrupt())
    {
        hsu_os_bus_handle_interrupt();
    }  
 
    /* If polling has not changed bus mode, return with special event code. */
    if (prev_mode == hsu_boot_curr_mode)
    {
        HSU_MSG_HIGH("hsu_boot_poll_stack:exit", 0,0,0);
        return HSU_BOOT_POLL_RESULT_NO_NEW_EVENT;
    }
  
    /* Otherwise, convert the bus mode into BOOT API event, and return it. */
    HSU_MSG_HIGH("hsu_boot_poll_stack:exit", 0,0,0);
    return hsu_boot_match_mode_to_state(hsu_boot_curr_mode);
}

/*===========================================================================
FUNCTION hsu_boot_disconnect_from_host()
===========================================================================*/
boolean hsu_boot_disconnect_from_host()
{
    HSU_MSG_HIGH("hsu_boot_disconnect_from_host:entered", 0,0,0);
    
#ifndef FEATURE_HSU_TEST
    /* Disconnect from the host, while USB cable is still attached */
    if (!(hsu_phy_utils_spoof_disconnect()))
    {
      HSU_MSG_ERROR("hsu_boot_disconnect_from_host: Spoof disconnect failed!", 
        0, 0, 0);
    }
#endif /* FEATURE_HSU_TEST */

    HSU_MSG_HIGH("hsu_boot_disconnect_from_host:exit", 0,0,0);
    return TRUE;
}

/*==============================================================================
INTERNAL FUNCTION DEFINITIONS FOR MODULE
==============================================================================*/
/*===========================================================================
FUNCTION hsu_boot_state_changed_cb()
DESCRIPTION 
    A callback that is called by the hsu_bus_mode_notifier when bus mode is 
    changed.
===========================================================================*/
static void hsu_boot_state_changed_cb(hsu_bus_mode_enum new_mode, 
                                      void* app_data)
{
    if (new_mode == hsu_boot_curr_mode)
    {
        return;
    }

    if (new_mode == HSU_BUS_MODE_CONFIGURED)
    {
        hsu_boot_was_already_enumerated = TRUE;
    }

    if (new_mode == HSU_BUS_MODE_DISCONNECTED)
    {
        hsu_boot_was_already_enumerated = FALSE;
    }

    hsu_boot_curr_mode = new_mode;
}

/*===========================================================================
FUNCTION hsu_boot_wall_chg_detect_cb()
DESCRIPTION 
A callback that is called by the hsu_bus_mode_notifier when bus mode is 
changed.
===========================================================================*/
static void hsu_boot_wall_chg_detect_cb(boolean is_wall_chg)
{
    hsu_boot_is_wall_charger = is_wall_chg;
}

/*===========================================================================
FUNCTION hsu_boot_match_mode_to_state()
DESCRIPTION 
    Convert the current state into event that is reported to BOOT.
===========================================================================*/
static hsu_boot_poll_result hsu_boot_match_mode_to_state(
    hsu_bus_mode_enum mode)
{
    switch (mode)
    {
        case HSU_BUS_MODE_SUSPENDED:
        {
            /* If the device is already enumerated, report suspend event */
            if (hsu_boot_was_already_enumerated)
            {
                return HSU_BOOT_POLL_RESULT_SUSPENDED;
            }
            /* If the device wasn't enumerated yet, do not forward suspend 
               events (otherwise, it will cause disconnect and revert to 100 
               mA charging) */
            else
            {
                return HSU_BOOT_POLL_RESULT_NO_NEW_EVENT;
            }
        }
        case HSU_BUS_MODE_RESUMED:
        {
            /* If the device is already enumerated, return enumerated event. 
               This actually shouldn't happen when BOOT code disconnects after
               suspend, but it is handled for the sake of completness */
            if (hsu_boot_was_already_enumerated)
            {
                return HSU_BOOT_POLL_RESULT_ENUMERATED;
            }
            else
            {
                return HSU_BOOT_POLL_RESULT_RESUMED_NOT_ENUMERATED;
            }
        }
        case HSU_BUS_MODE_CONFIGURED:
        {
            return HSU_BOOT_POLL_RESULT_ENUMERATED;
        }
        case HSU_BUS_MODE_DISCONNECTED:
        {
            return HSU_BOOT_POLL_RESULT_DISCONNECTED;
        }
        case HSU_BUS_MODE_CONNECTED:
        {
            hsu_chg_wallchg_detect_wall_charger(hsu_boot_wall_chg_detect_cb);
            if (hsu_boot_is_wall_charger)
            {
                return HSU_BOOT_POLL_RESULT_CONNECTED_TO_WALL_CHG;
            }
            else
            {
                return HSU_BOOT_POLL_RESULT_CONNECTED_TO_HOST_PC;
            }
        }
        case HSU_BUS_MODE_UNCONFIGURED:
        {
            if (hsu_boot_was_already_enumerated)
            {
                /* If host un-configured the device after it has enumerated, 
                then section in USB 2.0 spec dictates to revert to 100 mA. 
                Report this event as HSU_BOOT_POLL_RESULT_RESUMED_NOT_ENUMERATED, 
                and boot code will handle this by reverting to 100mA charging 
                allowing to enumerate in the future */
                return HSU_BOOT_POLL_RESULT_RESUMED_NOT_ENUMERATED;
            }
            else 
                /* This is the case where the device gets un-configured event 
                   when connecting to host. Don't report this as resumed event
                   because the host has already send a resume before */
            {
                return HSU_BOOT_POLL_RESULT_NO_NEW_EVENT;
            }
        }
        default:
        {
            HSU_ASSERT("hsu_boot_match_mode_to_state: Unknown mode" == NULL);
            return HSU_BOOT_POLL_RESULT_ERROR;
        }
    }
}

#else /* BUILD_HOSTDL */
#include "hsu_donothing.h"
#endif /* BUILD_HOSTDL */
