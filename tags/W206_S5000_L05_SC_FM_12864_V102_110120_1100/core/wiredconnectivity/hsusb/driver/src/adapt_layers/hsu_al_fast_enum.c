/*==============================================================================

High Speed USB FAST ENUMERATION Adaptation Layer

GENERAL DESCRIPTION
Implements enumeration in OEMSBL as degenerated OBEX. Defines externalized 
functions for the use of boot module. 

EXTERNALIZED FUNCTIONS

hsu_al_fast_enum_init
hsu_al_fast_enum_shutdown
hsu_al_fast_enum_poll
hsu_al_fast_enum_obex_init

Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/
/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/adapt_layers/hsu_al_fast_enum.c#6 $
$DateTime: 2008/09/15 01:05:29 $ $Author: ablay $

when      who     what, where, why
--------  ---     ------------------------------------------------------------
04/06/08  ab      Created.
==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/

#ifndef BUILD_HOSTDL

#include "hsu_al_fast_enum.h"
#include "hsu_al_oemsbl_stack_sel.h"
#include "jtypes.h"
#include "hsu_common.h"
#include "port.h"
#include "uw_args.h"
#include "uw_device_cdc_obex.h"
#include "uw_device_cdc.h"
#include "jslave_init.h"
#include "boothw.h"
#include "hsu_conf_sel_comp_utils.h"
#include "hsu_conf_sel_stack_utils.h"
#include "hsu_mdm_apis.h"
#include "hsu_phy_utils.h"
#include "clk.h"
#include "hsu_os_abstract_bus.h"
#include "hsu_conf_sel_core_config.h"

/*==============================================================================

DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
Constants
==============================================================================*/

/* The PID for fast enumeration */
#define HSU_AL_FAST_ENUM_PID HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_MDM

/*==============================================================================
Variables
==============================================================================*/

/* Flag is set to TRUE when enumeration is completed */
static boolean hsu_al_fast_enum_completed = FALSE;

/*==============================================================================

FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/

/*===========================================================================

FUNCTION hsu_al_fast_enum_start_stack

DESCRIPTION
This function initializes the memory for the stack and starts the stack,
with one OBEX interface.

DEPENDENCIES
None.

RETURN VALUE
0 on success, error number otherwise.

SIDE EFFECTS
None.
===========================================================================*/
static jresult_t hsu_al_fast_enum_start_stack(void)
{
  struct hsu_device_params_type* boot_core_conf_sel_info = NULL;

  hsu_al_fast_enum_completed = FALSE;

  hsu_conf_sel_core_config_set_core_info();
  hsu_conf_sel_stack_utils_init_memory();

  boot_core_conf_sel_info = hsu_conf_sel_core_config_get_boot_core_conf_sel_info();
  if (boot_core_conf_sel_info == NULL)
  {
    return JEINVAL;
  }

  memset(boot_core_conf_sel_info->configs_arr[HSU_CONFIG_IDX_0].num_of_fd_instances, 
          0, 
          sizeof(boot_core_conf_sel_info->configs_arr[HSU_CONFIG_IDX_0].num_of_fd_instances));
  boot_core_conf_sel_info->configs_arr[HSU_CONFIG_IDX_0].num_of_fd_instances[HSU_CFG_SEL_OBEX_FD] = 1;
  boot_core_conf_sel_info->product_id = HSU_AL_FAST_ENUM_PID;

  return hsu_conf_sel_stack_utils_setup_stack();
}

/*===========================================================================

FUNCTION hsu_al_fast_enum_enabled_cb

DESCRIPTION
Called when the function driver is enabled (after host sets configuration).

DEPENDENCIES
None.

RETURN VALUE
0 for success, error number for failure.

SIDE EFFECTS
None.
===========================================================================*/
static jresult_t hsu_al_fast_enum_enabled_cb(cdc_appctx_t ctx)
{
  HSU_USE_PARAM(ctx);

  /* Sets hsu_al_fast_enum_completed  to TRUE, so hsu_al_fast_enum_poll()
     will signal the API caller that the enumeration has completed */
  hsu_al_fast_enum_completed = TRUE;

  return 0;
}

/*===========================================================================

FUNCTION hsu_al_fast_enum_init

DESCRIPTION
This function initializes HS-USB support for FAST ENUMERATION. It starts the
HS-USB stack with one OBEX interface.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_al_fast_enum_init(void)
{
  jresult_t     jresult;

  /* May need to kick the dog here since initialization may take a while */
  BOOTHW_KICK_WATCHDOG();

  /* Set the stack mode to FAST_ENUM, such that OBEX FD will call FAST_ENUM
  implementation of obex_init */
  hsu_al_oemsbl_stack_sel_set_mode(HSU_AL_OEMSBL_STACK_MODE_FAST_ENUM);

  hsu_api_init_phy();

  /* Start the stack with degenerated OBEX interface. */
  jresult = hsu_al_fast_enum_start_stack();
  HSU_ASSERT(jresult == 0);
}

/*===========================================================================

FUNCTION hsu_al_fast_enum_shutdown

DESCRIPTION
This function shuts down the HS-USB stack, and disconnects from the host.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
void hsu_al_fast_enum_shutdown(void)
{
  int i;

  /* This 100ms loop gives the host a last chance to handle enumeration 
     interrupts, in order to support interrupts that are received after 
     hsu_al_fast_enum_enabled_cb() is called. 
     Windows host implementation requires a response to SET_FEATURE request. */

  for (i=0; i<100; i++)
  {
     /* If there is an interrupt, handle it. */
     if (hsu_os_bus_poll_interrupt())
     {
        BOOTHW_KICK_WATCHDOG();
        hsu_os_bus_handle_interrupt();
     }
     clk_busy_wait(1000);
  }

  /* Disable USB stack, and disconnect from the host */
  jslave_disable(0);
  jstop_stack();

  if (!(hsu_phy_utils_spoof_disconnect()))
  {
     HSU_MSG_ERROR("hsu_al_fast_enum_shutdown: Spoof disconnect failed!", 0, 0, 0);
  }
}

/*===========================================================================

FUNCTION hsu_al_fast_enum_poll

DESCRIPTION
This function uses OS abstraction functions in order to
poll for interrupts, and call the ISR and DSR if an interrupt
occurred.

DEPENDENCIES
None.

RETURN VALUE
FALSE - Enumeration has not been completed yet.
TRUE  - Enumeration has been completed.   

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_al_fast_enum_poll(void)
{
  if (hsu_os_bus_poll_interrupt())
  {
     BOOTHW_KICK_WATCHDOG();
     hsu_os_bus_handle_interrupt();
  }

  return hsu_al_fast_enum_completed;
}

/*===========================================================================

FUNCTION hsu_al_fast_enum_obex_init

DESCRIPTION
OBEX initialization function - called by hsu_al_oemsbl_stack_sel, which is 
called by the function driver on initialization, sets application context 
and callbacks.

DEPENDENCIES
None.

RETURN VALUE
0 for success, error number for failure.

SIDE EFFECTS
None.
===========================================================================*/
jresult_t hsu_al_fast_enum_obex_init(cdc_handle_t handle, 
                                     obex_callbacks_t* cbs, 
                                     obex_init_info_t* info, 
                                     cdc_appctx_t* ctx)
{
  /* Only one instance of the function driver, so ctx is not important */
  *ctx = (cdc_appctx_t)(1);

  cbs->basic.enabled_h = hsu_al_fast_enum_enabled_cb;

  /* These will not be used in fast enumeration: */
  cbs->basic.read_complete_h = NULL;
  cbs->basic.write_complete_h = NULL;
  cbs->basic.uninit_h = NULL;
  cbs->basic.disabled_h = NULL;

  /* These will never be used in OBEX: */
  cbs->basic.encap_command_h = NULL;
  cbs->basic.encap_response_complete_h = NULL;

  /* Init info struct */
  info->basic.flags = (juint32_t)(CDC_INIT_BUFFER_DMABLE | 
    CDC_INIT_SINGLE_INTERFACE |
    CDC_INIT_VENDOR_SPECIFIC_CLASS |
    CDC_INIT_NO_CLASS_DESC |
    CDC_INIT_NO_ALT_SETTINGS);

  return 0;
}

#else /* !BUILD_HOSTDL */
#include "hsu_donothing.h"
#endif /* !BUILD_HOSTDL */
