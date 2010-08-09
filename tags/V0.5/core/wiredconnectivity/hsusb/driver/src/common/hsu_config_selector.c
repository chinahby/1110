/*==============================================================================

High Speed USB Configuration Selector Module

GENERAL DESCRIPTION
This module is responsible for the initialization of the High Speed USB 
stack. 

EXTERNALIZED FUNCTIONS
hsu_config_selector_init
hsu_get_num_of_net_instances
hsu_conf_sel_get_cur_comp_details
hsu_conf_sel_get_rm_wakeup_delay
hsu_conf_sel_get_cur_comp_id
hsu_conf_sel_switch_dev_stack
hsu_conf_sel_stack_suspend_mode_changed
hsu_conf_sel_diag_is_on_legacy_port
hsu_conf_sel_uim_first_inst_class
hsu_conf_sel_use_pmic_otg_comparators
hsu_conf_sel_number_of_sdcc_luns
hsu_conf_sel_disable_sleep_voting

INITALIZATION AND SEQUENCING REQUIREMENTS
hsu_config_selector_init() should be called after DSM pools are initalized,
and before the creation of tasks that use USB, such as DIAG, Modem, RmNET, 
etc...

Copyright (c) 2007, 2009 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/common/hsu_config_selector.c#37 $
$DateTime: 2009/03/24 07:20:20 $ $Author: tlinder $

when      who     what, where, why
--------  ---     ------------------------------------------------------------
09/12/06  ke      Initial version

==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "customer.h" /* for FEATURE_HSU_TEST to work */
#include "comdef.h"
#include "err.h"
#include "hsu_config_selector.h"
#include "hsu_conf_sel_types.h"
#include "hsu_conf_sel_i.h"
#include "hsu_conf_sel_stack_utils.h"
#include "hsu_conf_sel_comp_utils.h"
#include "hsu_common.h"
#include "hsu_al_dmov_api.h"
#include "hsu_al_ser.h"
#include "hsu_conf_sel_nv.h"

#ifdef FEATURE_HS_USB_MS_FD
#include "hsu_al_ms.h"
#endif /*FEATURE_HS_USB_MS_FD*/

#ifndef FEATURE_HSU_TEST
#include "hsu_apis.h"

#ifdef FEATURE_HS_USB_OTG
#error code not present
#endif  /* FEATURE_HS_USB_OTG */

#ifdef FEATURE_HS_USB_UICC
#include "hsu_host_al_eem.h"
#endif /* FEATURE_HS_USB_UICC */

#include "nv_items.h"
#include "hsu_conf_sel_ui.h"
#endif /* ! FEATURE_HSU_TEST */

#if defined(FEATURE_HS_USB_LEGACY_HOST) || defined(FEATURE_HS_USB_LEGACY_DEVICE)
#include "hsu_legacy_phy.h"
#endif /* FEATURE_HS_USB_LEGACY_HOST || FEATURE_HS_USB_LEGACY_DEVICE */

#if defined(FEATURE_HSU_TEST) || defined(FEATURE_HSU_ECM_APP_STUB)
#error code not present
#endif /* FEATURE_HSU_ECM_APP_STUB */

#if defined(FEATURE_HSU_EEM_APP_STUB)
#include "hsu_eem_app_stub.h"
#endif /* FEATURE_HSU_EEM_APP_STUB */

#ifdef FEATURE_HSU_TEST
#error code not present
#else
#include "hsu_al_ecm.h"
#endif /* FEATURE_HSU_TEST */

#ifdef FEATURE_HS_USB_ANALOG_AUDIO_STUB
#include "..\hsusb\driver\src\testing\hsu_analog_audio_stub.h"
#endif

#ifdef FEATURE_HS_USB_OBEX_PERF_TESTS
#error code not present
#endif /*FEATURE_HS_USB_OBEX_PERF_TESTS*/

#ifdef FEATURE_HS_USB_ACM_PERF_TESTS
#error code not present
#endif /*FEATURE_HS_USB_ACM_PERF_TESTS*/


#if (!defined FEATURE_HS_USB_BASIC) && (defined FEATURE_HS_USB_UICC)
#include "hsu_usb_uicc.h"
#include "msmhwio.h"
#include "msmhwioreg.h"
#endif /* #if (!defined FEATURE_HS_USB_BASIC) && (defined FEATURE_HS_USB_UICC) */

#ifdef FEATURE_HS_USB_ANALOG_AUDIO
#include "hsu_analog_audio.h"
#endif /*FEATURE_HS_USB_ANALOG_AUDIO*/

#ifdef FEATURE_HS_USB_SAMPLE_LOOPBACK
#include "hsu_cd_sample_loopback.h"
#endif /* FEATURE_HS_USB_SAMPLE_LOOPBACK */

#if ((defined FEATURE_HS_USB_DIGITAL_AUDIO_HOST) && (!defined FEATURE_HS_USB_BASIC))
#include "hsu_audio_host_sample.h"
#endif /* (defined FEATURE_HS_USB_DIGITAL_AUDIO_HOST) && (!defined FEATURE_HS_USB_BASIC) */

#if (defined FEATURE_HS_USB_PMIC_PHY) || (defined FEATURE_HS_USB_LEGACY_DEVICE)
#include "hsu_pmic_otg_if.h"
#endif /* FEATURE_HS_USB_PMIC_PHY || FEATURE_HS_USB_LEGACY_DEVICE */
#ifdef FEATURE_HS_USB_HAL
#include "HAL_hsusb.h"
#endif /* FEATURE_HS_USB_HAL */

#include "hsu_conf_sel_core_config.h"
#include "hsu_core_config.h"
/*==============================================================================

DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
Constants and Macros
==============================================================================*/
#define HS_USB_UIM_FIRST_INST_CLASS_DEFAULT 2

/*==============================================================================
Typedefs
==============================================================================*/
/* This struct is used to hold all the static HSUSB configurations that are read 
   from NV*/
typedef struct
{
  /*
  * This variable indicates the number of milliseconds to wait before 
  * performing explicit remote wakeup when we are in suspend mode. 
  * If it's value is set to 0 the device doesn't perform explicit remote wakeup
  */
  uint32 explicit_remote_wakeup_delay;

#if !defined(FEATURE_HS_USB_NO_HS_OTG_CORE) && defined(FEATURE_HS_USB_LEGACY_DEVICE)
  /* 
  * This variable indicates whether DIAG should run over high-speed or full-speed 
  * USB. It is relevant only if both FS-Legacy & HS-OTG cores are supported 
  * in the image. Otherwise, DIAG is routed always from the primary core.
  */
  boolean diag_on_legacy_usb_port;
#endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE && FEATURE_HS_USB_LEGACY_DEVICE */

#ifdef FEATURE_HS_USB_MS_FD
  uint8 number_of_sdcc_luns;
#endif /* FEATURE_HS_USB_MS_FD */

#ifdef FEATURE_HS_USB_UICC
  /* This variable indicates the operation mode of UICC. */
  nv_uim_first_inst_class_type uim_first_inst_class;
#endif /* FEATURE_HS_USB_UICC */

#if defined(T_QSC6270) || defined(T_MSM8650)
  /* This variable indicates whether or not to use PMIC comparators in LCU. */
  boolean use_pmic_otg_comparators;
#endif /* T_QSC6270 || T_MSM8650 */

  /* 
  * This variable indicates whether or not to use the workaround for the USB
  * bus reset during TCXO shutdown issue. 
  * This variable gets updated from the NV during startup.
  */
  boolean reconnect_on_reset_during_suspend;

  /* 
  * This variable indicates whether or not to disable voting for sleep when 
  * entering LPM. This variable gets updated from the NV during startup.
  */
  boolean disable_sleep_voting;

}hsu_conf_sel_nv_items_type;

/*==============================================================================
Variables
==============================================================================*/
/* Used to hold all the static HSUSB configurations that are read from NV*/
static hsu_conf_sel_nv_items_type hsu_conf_sel_nv_items;

/*==============================================================================

FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================

FUNCTION          register_cust_class_driver

DESCRIPTION
This function is the recommended location where customer defined class driver 
is initialized. Refer to hsu_cd_sample_loopback_init as an example.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

==============================================================================*/
static void register_cust_class_driver(void) 
{
#ifdef FEATURE_HS_USB_SAMPLE_LOOPBACK
  hsu_cd_sample_loopback_init();
#endif /* FEATURE_HS_USB_SAMPLE_LOOPBACK */
}

/*==============================================================================

FUNCTION          UPDATE_DEVICE_COMPOSITION_FROM_NV

DESCRIPTION
Read the NV item for the current USB device composition, and set the static 
variable cur_dev_comp_details to point to the composition information.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
If the current value of the NV item is invalid, it may be over-written.

==============================================================================*/
static void update_device_composition_from_nv() 
{
#if !defined(FEATURE_HS_USB_NO_HS_OTG_CORE) || defined(FEATURE_HS_USB_BASIC)

  nv_hs_usb_supported_compositions_type nv_composition_id;
  const hsu_conf_sel_comp_info_type * default_comp_info = NULL;
  struct hsu_device_params_type* hs_main_core_conf_sel_info = NULL;
  boolean should_write_value = FALSE;

#endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE || FEATURE_HS_USB_BASIC */

#ifdef FEATURE_HS_USB_LEGACY_DEVICE 

  struct hsu_device_params_type* fs_legacy_core_conf_sel_info = NULL;

#endif /* FEATURE_HS_USB_LEGACY_DEVICE */

#if !defined(FEATURE_HS_USB_NO_HS_OTG_CORE) || defined(FEATURE_HS_USB_BASIC)
/* Update compositon from NV only for HS-OTG core */
#ifndef FEATURE_HSU_TEST

  if(hsu_conf_sel_nv_dev_comp_read(&nv_composition_id) == FALSE)
  {
    HSU_MSG_HIGH(
      "update_device_composition_from_nv(): hsu_conf_sel_nv_dev_comp_read for NV_HS_USB_CURRENT_COMPOSITION_I failed",
      0,0,0);
    /* After wipeEFS, sometimes the returned value is NV_NOTACTIVE_S, which 
    means "Variable was not active". 
    So write the default composition ID to the NV item, so we'd be able to read
    it next time */
    should_write_value = TRUE;
  }
  else
  {
    HSU_MSG_MED(
      "update_device_composition_from_nv(): read NV_HS_USB_CURRENT_COMPOSITION_I = %d",
      nv_composition_id,0,0);
    /* It is possible that the value is a FS composition value, or just "garbage". 
    If so, we should try to write the default HSU composition to the NV item. 
    This is because FS and HS are not active at the same time on any MSM, so 
    we're not running over anything "correct". */
    if(hsu_conf_sel_comp_utils_comp_id_is_valid_for_hsu(nv_composition_id) == FALSE)
    {
      HSU_MSG_HIGH(
        "update_device_composition_from_nv(): NV_HS_USB_CURRENT_COMPOSITION_I value out of range (%d)",
        nv_composition_id,0,0);
      should_write_value = TRUE;
    }

#ifdef FEATURE_HS_USB_MS_FD
    else {
      /* If the current composition includes MS, check the number of luns.
      If we have 0 luns  - current composition is not valid and we need to 
      switch back to default composition. */
      if ((hsu_conf_sel_comp_utils_get_comp_details(nv_composition_id)->num_of_fd_instances_in_comp[HSU_CFG_SEL_MS_FD] > 0) &&
        (hsu_al_ms_get_num_luns() == 0))
      {
        HSU_MSG_HIGH(
          "update_device_composition_from_nv(): Composition with MS was chosen but there are 0 luns. Switching to default.",
          0,0,0);
        should_write_value = TRUE;
      }
    }
#endif /*FEATURE_HS_USB_MS_FD*/
  }


  if(should_write_value == TRUE)
  {
    default_comp_info = hsu_conf_sel_comp_utils_get_default_comp_info();
    if(default_comp_info == NULL)
    {
      HSU_ASSERT(
        "update_device_composition_from_nv(): hsu_conf_sel_comp_utils_get_default_comp_info returned NULL"
        == NULL);
    }    
    nv_composition_id = default_comp_info->nv_comp_id;
    if(hsu_conf_sel_nv_dev_comp_write(nv_composition_id) == FALSE)
    {
      HSU_MSG_ERROR(
        "update_device_composition_from_nv(): hsu_conf_sel_nv_dev_comp_write failed.",
        0, 0, 0);
      HSU_ASSERT(
        (char*)"update_device_composition_from_nv(): hsu_conf_sel_nv_dev_comp_write failed."
        == NULL);
    }
    else
    {
      HSU_MSG_MED(
        "update_device_composition_from_nv(): Wrote %d to NV_HS_USB_CURRENT_COMPOSITION_I",
        nv_composition_id, 0, 0);
    }
  }

#else /* FEATURE_HSU_TEST */
#error code not present
#endif /* ! FEATURE_HSU_TEST */


#ifdef T_UNDP
  /* For UNDP (U2) cards, ask the card for the nv_composition_id info */
  {
    nv_hs_usb_supported_compositions_type undp_nv_comp_id;
    if (TRUE == hsu_get_undp_hsu_comp_id(&undp_nv_comp_id))
    {
      /* Update nv_composition_id value from UNDP */
      nv_composition_id = undp_nv_comp_id;
    }
  }
#endif /* T_UNDP */

  hs_main_core_conf_sel_info = hsu_conf_sel_core_config_get_hs_main_core_conf_sel_info(); 
  hs_main_core_conf_sel_info->configs_arr[HSU_CONFIG_IDX_0].cur_dev_comp_details = 
    hsu_conf_sel_comp_utils_get_comp_details(nv_composition_id);

#endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE || FEATURE_HS_USB_BASIC */

#ifdef FEATURE_HS_USB_LEGACY_DEVICE 

  /* FS Legacy core supports only device composition 0 */
  fs_legacy_core_conf_sel_info = hsu_conf_sel_core_config_get_fs_legacy_core_conf_sel_info();
  fs_legacy_core_conf_sel_info ->configs_arr[HSU_CONFIG_IDX_0].cur_dev_comp_details = 
    hsu_conf_sel_comp_utils_get_comp_details(NV_HS_USB_DIAG_NMEA_MDM_NET);

#endif /* FEATURE_HS_USB_LEGACY_DEVICE */
} /* update_device_composition_from_nv */

/*==============================================================================

FUNCTION          UPDATE_REMOTE_WAKEUP_DELAY_FROM_NV

DESCRIPTION
This function reads the value of the remote-wakeup delay
from NV item. The value is saved in a static variable: 
hsu_conf_sel_nv_items.explicit_remote_wakeup_delay

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
explicit_remote_wakeup_delay param. is initialized.

==============================================================================*/
static void update_remote_wakeup_delay_from_nv()
{

#ifndef FEATURE_HSU_TEST
  if (hsu_conf_sel_nv_rem_wakeup_delay_read(
        &hsu_conf_sel_nv_items.explicit_remote_wakeup_delay) == FALSE) 
  {   /* Failed reading from NV */
    hsu_conf_sel_nv_items.explicit_remote_wakeup_delay = 0;
    HSU_MSG_ERROR(
      "update_remote_wakeup_delay_from_nv(): Couldn't read from NV", 0, 0, 0);
    return;
  }
#endif /* FEATURE_HSU_TEST */
} /* update_remote_wakeup_delay_from_nv */

#if !defined(FEATURE_HS_USB_NO_HS_OTG_CORE) && defined(FEATURE_HS_USB_LEGACY_DEVICE)
/*==============================================================================

FUNCTION          UPDATE_DIAG_IS_ON_LEGACY_PORT_FROM_NV

DESCRIPTION
This function reads the NV item that determines whether DIAG should run over 
full-speed or high-speed USB and saves it in the variable 
hsu_conf_sel_nv_items.diag_on_legacy_usb_port.
This function is relevant only if both FS-legacy & HS-OTG cores are supported 
in the image. Otherwise, DIAG is routed always from the primary core.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS

==============================================================================*/
static void update_diag_is_on_legacy_port_from_nv()
{

#ifndef FEATURE_HSU_TEST
  if (hsu_conf_sel_nv_diag_on_legacy_port_read(&hsu_conf_sel_nv_items.diag_on_legacy_usb_port) == FALSE) /* Failed reading from NV */
  {
    hsu_conf_sel_nv_items.diag_on_legacy_usb_port = FALSE;
    HSU_MSG_ERROR(
      "update_diag_is_on_legacy_port_from_nv(): Couldn't read from NV", 0, 0, 0);
    return;
  }
#endif /* FEATURE_HSU_TEST */
} /* update_diag_is_on_legacy_port_from_nv */
#endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE && FEATURE_HS_USB_LEGACY_DEVICE */

#ifdef FEATURE_HS_USB_UICC
/*==============================================================================

FUNCTION          UPDATE_UIM_FIRST_INST_CLASS_FROM_NV

DESCRIPTION
This function reads the NV item that determines the operation mode of UICC, 
and saves it in the variable hsu_conf_sel_nv_items.uim_first_inst_class.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS

==============================================================================*/
static void update_uim_first_inst_class_from_nv()
{

#ifndef FEATURE_HSU_TEST
  /* Failed reading from NV */
  if (hsu_conf_sel_nv_uim_first_inst_class_read(
               &hsu_conf_sel_nv_items.uim_first_inst_class) == FALSE)
  {
    hsu_conf_sel_nv_items.uim_first_inst_class = HS_USB_UIM_FIRST_INST_CLASS_DEFAULT;
    HSU_MSG_HIGH(
      "update_uim_first_inst_class_from_nv(): Couldn't read from NV, using default [%d]", 
      hsu_conf_sel_nv_items.uim_first_inst_class, 0, 0);
  }
#endif /* FEATURE_HSU_TEST */
} /* update_uim_first_inst_class_from_nv */
#endif /* FEATURE_HS_USB_UICC */


#if defined(T_QSC6270) || defined(T_MSM8650)
/*==============================================================================

FUNCTION          UPDATE_USE_PMIC_OTG_COMPARATORS_FROM_NV

DESCRIPTION

This function reads the NV item that determines whether or not to use PMIC OTG 
comparators in LCU (hs_usb_use_pmic_otg_comparators), and saves it in
the variable hsu_conf_sel_nv_items.use_pmic_otg_comparators.


DEPENDENCIES
update_use_pmic_otg_comparators_from_nv() must have been called before 
calling this function.


RETURN VALUE
None.

SIDE EFFECTS

==============================================================================*/
static void update_use_pmic_otg_comparators_from_nv()
{
#ifndef FEATURE_HSU_TEST
  if (hsu_conf_sel_nv_use_pmic_otg_comparators_read(
    &hsu_conf_sel_nv_items.use_pmic_otg_comparators) == FALSE)
  {
    /* Failed reading from NV */
    hsu_conf_sel_nv_items.use_pmic_otg_comparators = FALSE;
    HSU_MSG_ERROR(
      "update_use_pmic_otg_comparators_from_nv(): Couldn't read from NV", 0, 0, 0);
  }

  #ifdef T_MSM8650
  /* TEMP FIX - 
  **
  ** Since the NV_HS_USB_USE_PMIC_OTG_COMPARATORS_I NV item does not 
  ** work properly on the Raptor2 target, the boolean value is hard-coded 
  ** to be FALSE.
  **
  */
  hsu_conf_sel_nv_items.use_pmic_otg_comparators = FALSE;
  /* TEMP FIX END */
  #endif  /* T_MSM8650 */

#endif /* FEATURE_HSU_TEST */
} /* update_use_pmic_otg_comparators_from_nv */
#endif /* T_QSC6270 || T_MSM8650 */


/*==============================================================================

FUNCTION          UPDATE_RECONNECT_ON_RESET_DURING_SUSPEND_FROM_NV

DESCRIPTION

This function reads the NV item that determines whether or not to use the 
reset on suspend workaround, and saves it in the variable:
hsu_conf_sel_nv_items.reconnect_on_reset_during_suspend.


DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS

==============================================================================*/
static void 
update_reconnect_on_reset_during_suspend_from_nv()
{
#ifndef T_REXNT
  boolean result;


  result = 
    hsu_conf_sel_nv_reconnect_on_reset_during_suspend_read(
      &hsu_conf_sel_nv_items.reconnect_on_reset_during_suspend);

  if (result == FALSE)
  {
    /* Failed reading from NV. Default to FALSE. */
    hsu_conf_sel_nv_items.reconnect_on_reset_during_suspend = FALSE;

    HSU_MSG_ERROR(
      "update_reconnect_on_reset_during_suspend_from_nv(): "
      "Couldn't read from NV",
      0, 0, 0);
  }
#endif /* T_REXNT */
} /* update_reconnect_on_reset_during_suspend_from_nv */

/*==============================================================================

FUNCTION          UPDATE_DISABLE_SLEEP_VOTING_FROM_NV

DESCRIPTION

This function reads the NV item that determines whether or not to disable voting 
for sleep when entering LPM, based on the value of the NV item 
hs_usb_disable_sleep_voting, and saves it in the variable:
hsu_conf_sel_nv_items.disable_sleep_voting.


DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS

==============================================================================*/

static void update_disable_sleep_voting_from_nv()
{
#ifndef T_REXNT
  boolean result = hsu_conf_sel_nv_disable_sleep_voting_read(
                    &hsu_conf_sel_nv_items.disable_sleep_voting);
  if (result == FALSE)
  { /* Failed reading from NV. Default to FALSE. */
    hsu_conf_sel_nv_items.disable_sleep_voting = FALSE;
    HSU_MSG_ERROR(
      "update_disable_sleep_voting_from_nv(): "
      "Couldn't read from NV",
      0, 0, 0);
  }  
#endif /*T_REXNT*/
}/*update_disable_sleep_voting_from_nv*/

#ifdef FEATURE_HS_USB_MS_FD
/*==============================================================================

FUNCTION          UPDATE_NUMBER_OF_SDCC_LUNS_FROM_NV

DESCRIPTION

This function reads the NV item that determines the number of SDCC/MMC LUNs
(hs_usb_number_of_sdcc_luns), and saves it in the variable 
hsu_conf_sel_nv_items.number_of_sdcc_luns.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS

==============================================================================*/
static void update_number_of_sdcc_luns_from_nv()
{
#ifndef FEATURE_HSU_TEST
  if (hsu_conf_sel_nv_number_of_sdcc_luns_read(
                         &hsu_conf_sel_nv_items.number_of_sdcc_luns) == FALSE)
  {
    /* Failed reading from NV */
    hsu_conf_sel_nv_items.number_of_sdcc_luns = 1;
    HSU_MSG_ERROR(
      "update_number_of_sdcc_luns_from_nv(): Couldn't read from NV", 0, 0, 0);
  }
#endif /* FEATURE_HSU_TEST */
} /* update_number_of_sdcc_luns_from_nv */
#endif /* FEATURE_HS_USB_MS_FD */

#ifdef FEATURE_HSU_TEST
#error code not present
#endif /* FEATURE_HSU_TEST */

#if defined(FEATURE_HSU_TEST) || defined(FEATURE_HSU_ECM_APP_STUB)
#error code not present
#endif /* #if defined(FEATURE_HSU_TEST) || defined(FEATURE_HSU_ECM_APP_STUB) */

#if defined(FEATURE_HSU_EEM_APP_STUB)
/*==============================================================================

FUNCTION          INIT_EEM_APP_STUB

DESCRIPTION
Init the EEM application stub, and use it to send 1 IP PING command to the 
EEM device.

DEPENDENCIES
FEATURE_HSU_EEM_APP_STUB is defined.

RETURN VALUE
None.

SIDE EFFECTS
None.

==============================================================================*/
static void init_eem_app_stub()
{
  /* Initialize the EEM application stub */
  if(hsu_eem_app_stub_init_task() != TRUE)
  {
    HSU_ASSERT(
      (char*)"init_eem_app_stub(): hsu_eem_app_stub_init_task() failed" 
      == NULL);
  }

  /* Open the SIO EEM data channel */
  if(hsu_eem_app_stub_sio_open() != TRUE)
  {
    HSU_ASSERT(
      (char*)"init_eem_app_stub(): hsu_eem_app_stub_sio_open() failed" 
      == NULL);
  }

  /* We don't want to try to send the PING before the device is enumerated */
  rex_sleep(3000);
  hsu_eem_app_stub_test_bulk_tx_ping();
} /* init_eem_app_stub */
#endif /* #if defined(FEATURE_HSU_EEM_APP_STUB) */

/*==============================================================================

FUNCTION          HSU_CONFIG_SELECTOR_INIT

DESCRIPTION
Called from the Application's processor TMC, after DSM pools are initialized,
and before the creation of tasks that use USB, such as DIAG, Modem, RmNET, 
etc...
The function initializes the High Speed USB stack according to the default
composition.

DEPENDENCIES
None, except for those noted above.

RETURN VALUE
None.

SIDE EFFECTS
Various tasks related to the HS-USB stack are created.

==============================================================================*/
void hsu_config_selector_init(void) 
{
#ifdef FEATURE_HSU_TEST
  boolean   bool_result;
#elif !defined (FEATURE_HS_USB_NO_HS_OTG_CORE) || defined (FEATURE_HS_USB_BASIC)
  /* Define all the structures needed for the stack init */
  struct hsu_device_params_type* hs_main_core_conf_sel_info = NULL;
  const hsu_conf_sel_comp_info_type* cur_dev_comp_details = NULL;
#endif  /* FEATURE_HSU_TEST */

#ifdef FEATURE_HS_USB_ANALOG_AUDIO
  hsu_analog_audio_conf_param_type config_param; 
#endif /*FEATURE_HS_USB_ANALOG_AUDIO*/

#ifdef FEATURE_HS_USB_HAL
  HAL_hsusb_Init();
#endif /* FEATURE_HS_USB_HAL */

#ifndef FEATURE_HSU_TEST
  hsu_conf_sel_core_config_set_core_info();
  hsu_conf_sel_stack_utils_init_memory();
#endif /* ! FEATURE_HSU_TEST */

#ifdef FEATURE_HS_USB_MS_FD
  update_number_of_sdcc_luns_from_nv();
#endif /* FEATURE_HS_USB_MS_FD */

#ifdef FEATURE_HS_USB_UICC
  update_uim_first_inst_class_from_nv();
#endif /* FEATURE_HS_USB_UICC */

  update_device_composition_from_nv();
  update_remote_wakeup_delay_from_nv();

#if !defined(FEATURE_HS_USB_NO_HS_OTG_CORE) && defined(FEATURE_HS_USB_LEGACY_DEVICE)
  /* When both primary & secondary cores are enabled on the same image, read
     from NV whether DIAG is routed to the primary or secondary port. 
     Otherwise, DIAG is routed to the primary core, which can be HS-OTG, or
     FS-Legacy. */
  update_diag_is_on_legacy_port_from_nv();
#endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE && FEATURE_HS_USB_LEGACY_DEVICE */

#if defined(T_QSC6270) || defined(T_MSM8650)
  update_use_pmic_otg_comparators_from_nv();
#endif /* T_QSC6270 || T_MSM8650 */

  update_reconnect_on_reset_during_suspend_from_nv();

  update_disable_sleep_voting_from_nv();
  /**********************************/
  /* Initialize CDC ALs and related */
  /**********************************/ 
  hsu_al_dmov_api_init();

  if(hsu_al_ser_init() == FALSE)
  {
    HSU_MSG_ERROR("*_init(): hsu_al_ser_init returned FALSE",0,0,0);
    HSU_ASSERT((char*)"*_init(): hsu_al_ser_init returned FALSE" == NULL);
  }

#ifndef FEATURE_HSU_TEST 
#ifdef FEATURE_HS_USB_ECM
  if(hsu_al_ecm_config() == FALSE)
  {
    HSU_MSG_ERROR("*_init(): hsu_al_ecm_config() failed",0,0,0);
    HSU_ASSERT((char *)"*_init(): hsu_al_ecm_config() failed" == NULL);
  }
#endif /* FEATURE_HS_USB_ECM */

#ifdef FEATURE_HS_USB_OTG
#error code not present
#endif  /* FEATURE_HS_USB_OTG */

#ifdef FEATURE_HS_USB_OTG
#error code not present
#endif  /* FEATURE_HS_USB_OTG */

#if (defined FEATURE_HS_USB_UICC)
  if(hsu_host_al_eem_init() == FALSE)
  {
    HSU_MSG_ERROR("*_init(): hsu_host_al_eem_init failed.",0,0,0);
    HSU_ASSERT("*_init(): hsu_host_al_eem_init failed" == NULL);
    return;
  }
#endif  /* FEATURE_HS_USB_UICC */

  /* Init HSU charger module */
#ifdef FEATURE_HS_USB_CHARGING
  hsu_api_init_chg();
#endif /* !FEATURE_HS_USB_CHARGING */

#endif /* ! FEATURE_HSU_TEST */

  HSU_MSG_HIGH("*_init(): setting up the stack",0,0,0);
  /************************/
  /* Initialize the stack */
  /************************/
#ifdef FEATURE_HSU_TEST
#error code not present
#else /* ! FEATURE_HSU_TEST */

#if (!defined FEATURE_HS_USB_NO_HS_OTG_CORE) || (defined FEATURE_HS_USB_LEGACY_HOST)
  /* Initialize the HS-OTG PHY. 
     If enabled, initialize FS-Legacy Host PHY as well. */
  hsu_api_init_phy();
#endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE || FEATURE_HS_USB_LEGACY_HOST */

#if (!defined FEATURE_HS_USB_BASIC) && (defined FEATURE_HS_USB_LEGACY_DEVICE)
  /* Initialize the FS Legacy device PHY. Note that this function call doesn't
     support RPC as HS-OTG PHY initialization, to avoid RPC API modification. */
  hsu_init_fs_usb();
#endif /* if (!defined FEATURE_HS_USB_BASIC) && (defined FEATURE_HS_USB_LEGACY_HOST) */

#if (!defined FEATURE_HS_USB_BASIC) && (defined FEATURE_HS_USB_LEGACY_HOST)
  hsu_init_legacy_phy();
#endif /* if (!defined FEATURE_HS_USB_BASIC) && (defined FEATURE_HS_USB_LEGACY_HOST) */

#if (!defined FEATURE_HS_USB_BASIC) && (defined FEATURE_HS_USB_UICC)
  /* Initialize UICC only if allowed by UIM NV item */
  if (hsu_uicc_is_enabled_by_nv())
  {
    hsu_uicc_usb_conn_timer_def();
    hsu_api_init_uicc();
  }
  else
  {    
    hsu_core_config_clear_uicc_core();
  }
#endif /* if (!defined FEATURE_HS_USB_BASIC) && (defined FEATURE_HS_USB_UICC) */

#if !defined (FEATURE_HS_USB_NO_HS_OTG_CORE) || defined (FEATURE_HS_USB_BASIC)
  
  /* Set the High Speed core data for configuration 0 since it depends on the chosen composition */
  hs_main_core_conf_sel_info = hsu_conf_sel_core_config_get_hs_main_core_conf_sel_info();

  cur_dev_comp_details = 
    hs_main_core_conf_sel_info->configs_arr[HSU_CONFIG_IDX_0].cur_dev_comp_details;

  if (cur_dev_comp_details == NULL)
  {
    HSU_ASSERT(
      "_init(): cur_dev_comp_details is NULL" == NULL);
  }

  memcpy(hs_main_core_conf_sel_info->configs_arr[HSU_CONFIG_IDX_0].num_of_fd_instances, 
    &(cur_dev_comp_details->num_of_fd_instances_in_comp),
    sizeof(hs_main_core_conf_sel_info->configs_arr[HSU_CONFIG_IDX_0].num_of_fd_instances));

  hs_main_core_conf_sel_info->product_id = cur_dev_comp_details->product_id;

#endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE || FEATURE_HS_USB_BASIC */

    if(hsu_conf_sel_stack_utils_setup_stack(
#ifdef T_UNDP
	TRUE
#endif
       ) != HSU_JSUCCESS)
  {
    HSU_MSG_ERROR("*_init(): hsu_conf_sel_stack_utils_setup_stack failed",0,0,0);
    HSU_ASSERT(
      "*_init(): hsu_conf_sel_stack_utils_setup_stack failed" == NULL);
  }

#if (!defined FEATURE_HS_USB_BASIC) && (defined FEATURE_HS_USB_UICC)
  /* Start UICC only if allowed by UIM NV item */
  if (hsu_uicc_is_enabled_by_nv())
  {
    /* Start the UICC with 1.8V */ 
    hsu_uicc_set_voltage_class(HSU_UICC_VOLTAGE_CLASS_C);
    hsu_api_start_uicc();
    /* After starting the UICC, we perform a 20ms timer to see if a USB 
    * connection was detected when using voltage class C. If no USB connection 
    * was detected we try to start the UICC with voltage class B, and if it 
    * fails as well, we notify the UIM about it. The UIM need to power-up the 
    * UICC with ISO protocol. */
    hsu_uicc_usb_start_conn_timer();
  }
#endif /* if (!defined FEATURE_HS_USB_BASIC) && (defined FEATURE_HS_USB_UICC) */

#if (defined FEATURE_HS_USB_PMIC_PHY) || (defined FEATURE_HS_USB_LEGACY_DEVICE)
  hsu_pmic_otg_if_init();
#endif /* FEATURE_HS_USB_PMIC_PHY || FEATURE_HS_USB_LEGACY_DEVICE */

#endif  /* FEATURE_HSU_TEST */

  /* 
  * ECM application stub should be initialized when FEATURE_HSU_TEST is used, 
  * and also when FEATURE_HSU_ECM_APP_STUB is used, if the network function is 
  * part of the used composition. 
  */
#if defined(FEATURE_HSU_TEST) || defined(FEATURE_HSU_ECM_APP_STUB)
#error code not present
#endif

#if defined(FEATURE_HSU_EEM_APP_STUB)
  init_eem_app_stub();
#endif

#ifdef FEATURE_HS_USB_OBEX_PERF_TESTS
#error code not present
#endif /* FEATURE_HS_USB_OBEX_PERF_TESTS */
#ifdef FEATURE_HS_USB_ACM_PERF_TESTS
#error code not present
#endif /* FEATURE_HS_USB_ACM_PERF_TESTS */

#ifdef FEATURE_HS_USB_ANALOG_AUDIO
  hsu_analog_audio_init();
  config_param.new_mode = HSU_AUDIO_MODE_USB;
  config_param.completion_cb = NULL;
  hsu_analog_audio_config((void*)&config_param);
#endif /*FEATURE_HS_USB_ANALOG_AUDIO*/

#ifdef FEATURE_HS_USB_ANALOG_AUDIO_STUB
  if (!hsu_analog_audio_stub_init())
  {
    HSU_MSG_ERROR("*_init(): hsu_analog_audio_stub_init() failed",0,0,0) ;
  }
#endif

  /* register customer defined class driver */
  register_cust_class_driver();

#if ((defined FEATURE_HS_USB_DIGITAL_AUDIO_HOST) && (!defined FEATURE_HS_USB_BASIC))
  hsu_audio_host_sample_init();
#endif /* (defined FEATURE_HS_USB_DIGITAL_AUDIO_HOST) && (!defined FEATURE_HS_USB_BASIC) */


} /* hsu_config_selector_init */

/*==============================================================================

FUNCTION          HSU_CONF_SEL_GET_CUR_COMP_DETAILS

DESCRIPTION
This function returns a pointer to the information relevant for the current 
composition, for the primary core.

DEPENDENCIES
None.

RETURN VALUE
Pointer to hsu_conf_sel_comp_info_type for success, 
Currently all failures cause ASSERT.

SIDE EFFECTS
None.

==============================================================================*/
const hsu_conf_sel_comp_info_type *  hsu_conf_sel_get_cur_comp_details(void)
{
  struct hsu_device_params_type* primary_core_conf_sel_info = NULL;

#ifndef FEATURE_HS_USB_NO_HS_OTG_CORE
  primary_core_conf_sel_info = hsu_conf_sel_core_config_get_hs_main_core_conf_sel_info();    
#elif defined FEATURE_HS_USB_LEGACY_DEVICE
  primary_core_conf_sel_info = hsu_conf_sel_core_config_get_fs_legacy_core_conf_sel_info();    
#else 
  return NULL;
#endif

  return primary_core_conf_sel_info->configs_arr[HSU_CONFIG_IDX_0].cur_dev_comp_details;
} /* hsu_conf_sel_get_cur_comp_details */


/*==============================================================================

FUNCTION          HSU_CONF_SEL_GET_CUR_COMP_ID

DESCRIPTION
This function returns the hsu_conf_sel_composition_id_enum of the current 
composition, for the primary core

DEPENDENCIES
None.

RETURN VALUE
hsu_conf_sel_composition_id_enum 

SIDE EFFECTS
None.

==============================================================================*/
hsu_conf_sel_composition_id_enum hsu_conf_sel_get_cur_comp_id(void)
{
  struct hsu_device_params_type* primary_core_conf_sel_info = NULL;
  const hsu_conf_sel_comp_info_type* cur_dev_comp_details = NULL;

#ifndef FEATURE_HS_USB_NO_HS_OTG_CORE
  primary_core_conf_sel_info = hsu_conf_sel_core_config_get_hs_main_core_conf_sel_info();    
#elif defined FEATURE_HS_USB_LEGACY_DEVICE
  primary_core_conf_sel_info = hsu_conf_sel_core_config_get_fs_legacy_core_conf_sel_info();    
#else 
  #error "At least one core should be defined!"
#endif

  cur_dev_comp_details = 
    primary_core_conf_sel_info->configs_arr[HSU_CONFIG_IDX_0].cur_dev_comp_details;

  if (cur_dev_comp_details == NULL)
  {
    HSU_ASSERT(
      "_get_cur_comp_id(): cur_dev_comp_details is NULL" == NULL);
  }

  return cur_dev_comp_details->hsu_comp_id;
} /* hsu_conf_sel_get_cur_comp_id */

#ifndef FEATURE_HSU_TEST
#ifndef T_UNDP /* UNDP does not support UI/Pictbridge */
/*==============================================================================

FUNCTION          HSU_CONF_SEL_SWITCH_DEV_STACK

DESCRIPTION
This function initializes and starts the device stack according to the new 
composition given as a parameter.

DEPENDENCIES
Assumes the stack is up. 

RETURN VALUE
TRUE for success, currently all failures cause an ASSERT.

SIDE EFFECTS
cur_dev_comp_details is updated with the new composition.

==============================================================================*/
boolean hsu_conf_sel_switch_dev_stack
(
 hsu_conf_sel_composition_id_enum new_hsu_comp_id,
 reconfig_complete_cb_t reconfig_cb_func_ptr,
 void *      arg_for_reconfig_cb
 )
{
  struct hsu_device_params_type* primary_core_conf_sel_info = NULL;
  const hsu_conf_sel_comp_info_type * new_dev_comp = 
    hsu_conf_sel_comp_utils_get_comp_details_by_hsu_id(new_hsu_comp_id);
  HSU_ASSERT(new_dev_comp != NULL);


  /* Update hsu_core_config_table with the new composition details */
  /***************************************/
  /* Set the primary core data for configuration 0 since it depends on the chosen composition */
#ifndef FEATURE_HS_USB_NO_HS_OTG_CORE
  primary_core_conf_sel_info = hsu_conf_sel_core_config_get_hs_main_core_conf_sel_info();    
#elif defined FEATURE_HS_USB_LEGACY_DEVICE
  primary_core_conf_sel_info = hsu_conf_sel_core_config_get_fs_legacy_core_conf_sel_info();    
#else 
  #error "At least one core should be defined!"
#endif

  primary_core_conf_sel_info->configs_arr[HSU_CONFIG_IDX_0].cur_dev_comp_details = new_dev_comp;
  primary_core_conf_sel_info->product_id = new_dev_comp->product_id;
  memcpy (primary_core_conf_sel_info->configs_arr[HSU_CONFIG_IDX_0].num_of_fd_instances,
    new_dev_comp->num_of_fd_instances_in_comp,
    sizeof(primary_core_conf_sel_info->configs_arr[HSU_CONFIG_IDX_0].num_of_fd_instances));

  /* Start the stack */
  /*******************/
  HSU_MSG_LOW(
    "*_switch_dev_stack(): calling hsu_conf_sel_stack_utils_switch_device_stack",
    0,0,0);
  if(hsu_conf_sel_stack_utils_switch_device_stack(
    HSU_CORE_IDX_PRIMARY,
    reconfig_cb_func_ptr,
    arg_for_reconfig_cb) != HSU_JSUCCESS)
  {
    HSU_ASSERT(
      (char*)"*_switch_dev_stack(): hsu_conf_sel_stack_utils_switch_device_stack failed"
      == NULL);
  }
  return TRUE;
} /* hsu_conf_sel_switch_dev_stack */
#endif /* !T_UNDP */

#endif /* ! FEATURE_HSU_TEST */

/*===========================================================================

FUNCTION      HSU_GET_NUM_OF_NET_INSTANCES

DESCRIPTION
This function returns the number of the network (ECM) FD instances included in 
the current composition, for the primary core.

DEPENDENCIES
FEATURE_HS_USB must be defined. 
Must be called after hsu_config_selector_init() was called.

RETURN VALUE
The number of the network (ECM) FD instances included in the current composition. 

SIDE EFFECTS
None

===========================================================================*/
uint8 hsu_get_num_of_net_instances(void)
{
  struct hsu_device_params_type* primary_core_conf_sel_info = NULL;
  const hsu_conf_sel_comp_info_type* cur_dev_comp_details = NULL;

  /* Disable ECM application in order to run ECM performance test or 
  ECM application stub. The ecm_stub_app will run instead of the 
  real application   */
#if (defined FEATURE_HS_USB_ECM_PERFORMANCE_TESTS) || (defined FEATURE_HSU_ECM_APP_STUB)
#error code not present
#else

#ifndef FEATURE_HS_USB_NO_HS_OTG_CORE
  primary_core_conf_sel_info = hsu_conf_sel_core_config_get_hs_main_core_conf_sel_info();    
#elif defined FEATURE_HS_USB_LEGACY_DEVICE
  primary_core_conf_sel_info = hsu_conf_sel_core_config_get_fs_legacy_core_conf_sel_info();    
#else 
  return 0;
#endif

  if (primary_core_conf_sel_info == NULL)
  {
    HSU_ASSERT(
      "hsu_get_num_of_net_instances(): primary_core_conf_sel_info is NULL" == NULL);
  }    
  cur_dev_comp_details = 
    primary_core_conf_sel_info->configs_arr[HSU_CONFIG_IDX_0].cur_dev_comp_details;

  if (cur_dev_comp_details == NULL)
  {
    HSU_ASSERT(
      "hsu_get_num_of_net_instances(): cur_dev_comp_details is NULL" == NULL);
  }    

  return cur_dev_comp_details->num_of_fd_instances_in_comp[HSU_CFG_SEL_ECM_FD];

#endif /* FEATURE_HS_USB_ECM_PERFORMANCE_TESTS || FEATURE_HSU_ECM_APP_STUB */
} /* hsu_get_num_of_net_instances */

/*==============================================================================

FUNCTION          HSU_CONF_SEL_GET_RM_WAKEUP_DELAY

DESCRIPTION
This function returns the hsu_conf_sel_get_rm_wakeup_delay that was read from the 
NV. This is the delay used for performing an explicit remote wakeup.

DEPENDENCIES
hsu_config_selector_init must be called before.

RETURN VALUE
hsu_conf_sel_nv_items.explicit_remote_wakeup_delay 

SIDE EFFECTS
None.

==============================================================================*/
uint32 hsu_conf_sel_get_rm_wakeup_delay(void)
{
  return hsu_conf_sel_nv_items.explicit_remote_wakeup_delay;
} /* hsu_conf_sel_get_rm_wakeup_delay */

#if !defined(FEATURE_HS_USB_NO_HS_OTG_CORE) && defined(FEATURE_HS_USB_LEGACY_DEVICE)
/*==============================================================================

FUNCTION          HSU_CONF_SEL_DIAG_IS_ON_LEGACY_PORT

DESCRIPTION
This function returns whether DIAG should run over the FS-Legacy or HS-OTG core.
This function is relevant only if both FS-legacy & HS-OTG cores are supported 
in the image. Otherwise, DIAG is routed always from the primary core.

DEPENDENCIES
hsu_config_selector_init must be called before.

RETURN VALUE
TRUE if DIAG should run over full-speed USB.

SIDE EFFECTS
None.

==============================================================================*/
boolean hsu_conf_sel_diag_is_on_legacy_port(void)
{
  return hsu_conf_sel_nv_items.diag_on_legacy_usb_port;
} /* hsu_conf_sel_diag_is_on_legacy_port */
#endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE && FEATURE_HS_USB_LEGACY_DEVICE */

#ifdef FEATURE_HS_USB_UICC
/*==============================================================================

FUNCTION          HSU_CONF_SEL_UIM_FIRST_INST_CLASS

DESCRIPTION
This function returns the operation mode of UICC.

DEPENDENCIES
hsu_config_selector_init must be called before.

RETURN VALUE
UICC operation mode.

SIDE EFFECTS
None.

==============================================================================*/
nv_uim_first_inst_class_type hsu_conf_sel_uim_first_inst_class(void)
{
  return hsu_conf_sel_nv_items.uim_first_inst_class;
} /* hsu_conf_sel_uim_first_inst_class */
#endif /* FEATURE_HS_USB_UICC */

#if defined (T_QSC6270) || defined(T_MSM8650)
/*==============================================================================

FUNCTION          HSU_CONF_SEL_USE_PMIC_OTG_COMPARATORS

DESCRIPTION
This function returns whether or not to use PMIC OTG comparators in LCU, based
on the value of the NV item hs_usb_use_pmic_otg_comparators

DEPENDENCIES
None.

RETURN VALUE
TRUE if PMIC comparators will be used during LPM, Otherwise, FALSE is
returned.

SIDE EFFECTS
None.

==============================================================================*/
boolean hsu_conf_sel_use_pmic_otg_comparators(void)
{
  return hsu_conf_sel_nv_items.use_pmic_otg_comparators;
} /* hsu_conf_sel_use_pmic_otg_comparators */
#endif /* T_QSC6270 || T_MSM8650 */


/*==============================================================================

FUNCTION          HSU_CONF_SEL_DISBLE_SLEEP_VOTING

DESCRIPTION
This function returns whether or not to disable voting for sleep when entering 
LPM, based on the value of the NV item hs_usb_disable_sleep_voting.

DEPENDENCIES
None.

RETURN VALUE
TRUE to disable sleep voting. Otherwise, FALSE is returned.
returned.

SIDE EFFECTS
None.

==============================================================================*/
boolean hsu_conf_sel_disable_sleep_voting(void)
{
  return hsu_conf_sel_nv_items.disable_sleep_voting;
} /* hsu_conf_sel_disable_sleep_voting */

/*==============================================================================

FUNCTION          HSU_CONF_SEL_RECONNECT_ON_RESET_DURING_SUSPEND

DESCRIPTION
This function returns whether or not to use the workaround for the 
USB bus reset on during TCXO shutdown issue, based on the value of the 
NV item hs_usb_reconnect_on_reset_during_suspend.

DEPENDENCIES
None.

RETURN VALUE
TRUE if the workaround is used. Otherwise, FALSE is returned.
returned.

SIDE EFFECTS
None.

==============================================================================*/
boolean hsu_conf_sel_reconnect_on_reset_during_suspend(void)
{
  return hsu_conf_sel_nv_items.reconnect_on_reset_during_suspend;
} /* hsu_conf_sel_reconnect_on_reset_during_suspend */

#ifdef FEATURE_HS_USB_MS_FD
/*==============================================================================

FUNCTION          HSU_CONF_SEL_NUMBER_OF_SDCC_LUNS

DESCRIPTION
This function returns the number of MMC/SDCC LUNs that are enabled.

DEPENDENCIES
None.

RETURN VALUE
The number of MMC/SDCC LUNs that are enabled.

SIDE EFFECTS
None.

==============================================================================*/
uint8 hsu_conf_sel_number_of_sdcc_luns (void)
{
  return hsu_conf_sel_nv_items.number_of_sdcc_luns;
} /* hsu_conf_sel_number_of_sdcc_luns */
#endif /* FEATURE_HS_USB_MS_FD */
