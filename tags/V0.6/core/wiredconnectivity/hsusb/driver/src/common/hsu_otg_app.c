/*==============================================================================

  High Speed USB OTG Application 
  
  GENERAL DESCRIPTION
    This module includes various callback and utility functions, dealing with OTG.
    This file is compiled only if FEATURE_HS_USB_OTG is defined. 
      
  EXTERNALIZED FUNCTIONS
  otg_app_init
  hsu_otg_app_get_otg_mode
  hsu_otg_app_is_mini_a_connected
  hsu_otg_app_set_core_index
  hsu_otg_app_set_up_whitelist

  INITALIZATION AND SEQUENCING REQUIREMENTS
  otg_app_init should be called before any other function.

  Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  $Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/common/hsu_otg_app.c#15 $
  $DateTime: 2009/02/11 06:48:45 $ $Author: ablay $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------
  05/27/08  tl      Initial version
    
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE
  
==============================================================================*/
#include "hsu_common.h"

#include "hsu_otg_app.h"

#include "event.h"
#include "hsu_event_defs.h"
#include "msm.h"

#include "hsu_mdm_apis.h"

#include "hsu_conf_sel_stack_utils.h"
#include "hsu_os_abstract_bus.h"
#include "hsu_conf_sel_types.h"

#ifdef FEATURE_HS_USB_CORE_RESET_ON_DISC     
#include "hsu_al_task.h"     
#endif /*FEATURE_HS_USB_CORE_RESET_ON_DISC*/ 

/*==============================================================================

                          DEFINITIONS AND DECLARATIONS FOR MODULE
  
==============================================================================*/

/*==============================================================================
Constants and Macros
==============================================================================*/

#ifdef FEATURE_HS_USB_UICC
#define HSU_OTG_APP_WHITELIST_SIZE    6
#else
#define HSU_OTG_APP_WHITELIST_SIZE    4
#endif /* FEATURE_HS_USB_UICC */

#define HSU_OTG_APP_AUDIO_CLASS       0x01
#define HSU_OTG_APP_HID_CLASS         0x03
#define HSU_OTG_APP_MSC_CLASS         0x08
#define HSU_OTG_APP_CDC_CLASS         0x02
#define HSU_OTG_APP_ICCD_CLASS        0x0b
#define HSU_OTG_APP_CDC_EEM_SUBCLASS  0x0c
#define HSU_OTG_APP_TEST_DEVICE_VID   0x1A0A

/*==============================================================================
  Typedefs
==============================================================================*/
typedef struct  
{
  otg_ctx_t         otg_core_ctx;
  otg_device_mode_t otg_mode;
  boolean           mini_a_connected;
  uint32            otg_core_index;
} hsu_otg_app_info_type;

/*==============================================================================
  Variables
==============================================================================*/
static hsu_otg_app_info_type otg_info;
/*==============================================================================

                          FUNCTION DECLARATIONS FOR MODULE
  
==============================================================================*/

/*==============================================================================

FUNCTION          OTG_NOTIFY_PERIPHERAL_STATE_CB

DESCRIPTION
   Called to inform of the host’s degree of support for the connected peripheral.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==============================================================================*/
static void otg_notify_peripheral_state_cb
(
  otg_appctx_t appctx, 
  otg_peripheral_state_t state
)
{
  hsu_otg_app_info_type * otg_context = 
    (hsu_otg_app_info_type *)appctx;
  HSU_ASSERT(&otg_info == otg_context);
  HSU_USE_PARAM(otg_context);

  HSU_MSG_HIGH(
    "otg_notify_peripheral_state_cb(): peripheral state=%d (0=sup,1=partial,2=not sup)", 
    state,0,0);

  if (state == OTG_PERIPH_PARTIAL_SUPPORTED)
  {
    HSU_MSG_HIGH(
      "otg_notify_peripheral_state_cb(): peripheral is partially supported.",
      0,0,0);
    /* TODO - should we notify the user that we only support some of the 
    peripheral's functions? */
  }
  else if (state == OTG_PERIPH_NOT_SUPPORTED)
  {
    HSU_MSG_HIGH(
      "otg_notify_peripheral_state_cb(): peripheral is not supported.",
      0,0,0);
    /* TODO - it would be nice to let the user know we do not support this device... */
  }
  /* Send debug event on success */
  event_report(EVENT_HS_USB_DEVICE_ATTACHED ) ;

} /* otg_notify_peripheral_state_cb */

/*==============================================================================

FUNCTION          OTG_NOTIFY_HNP_STATE_CB

DESCRIPTION
  Called to inform of the current HNP state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==============================================================================*/
static void otg_notify_hnp_state_cb(otg_appctx_t appctx, otg_hnp_state_t state)
{
  hsu_otg_app_info_type * otg_context = 
    (hsu_otg_app_info_type *)appctx;
  HSU_ASSERT(&otg_info == otg_context);
  HSU_USE_PARAM(otg_context);

  HSU_MSG_HIGH("otg_notify_hnp_state_cb(): called with state=%d. Ignoring...", 
    state, 0, 0);
} /* otg_notify_hnp_state_cb */

/*==============================================================================

FUNCTION          OTG_NOTIFY_SRP_STATE_CB

DESCRIPTION
  Called to inform of the device’s current SRP state, which determines whether
  the device may start an SRP process.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==============================================================================*/
static void otg_notify_srp_state_cb(otg_appctx_t appctx, jbool_t is_enabled)
{
  hsu_otg_app_info_type * otg_context = 
    (hsu_otg_app_info_type *)appctx;
  HSU_ASSERT(&otg_info == otg_context);
  HSU_USE_PARAM(otg_context);

  HSU_MSG_HIGH(
    "otg_notify_srp_state_cb(): called with is_enabled=%d. Ignoring...", 
    is_enabled, 0, 0);
} /* otg_notify_srp_state_cb */

/*==============================================================================

FUNCTION          OTG_NOTIFY_ERROR_CB

DESCRIPTION
  Called to inform that an error of type otg_error_t has occurred.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  For any err value other than OTG_ERROR_OK, ASSERTs.

==============================================================================*/
static void otg_notify_error_cb(otg_appctx_t appctx, otg_error_t err)
{
  hsu_otg_app_info_type * otg_context = 
    (hsu_otg_app_info_type *)appctx;
  HSU_ASSERT(&otg_info == otg_context);
  HSU_USE_PARAM(otg_context);

  switch (err)
  {
    case OTG_ERROR_OK:
    {
      HSU_ASSERT(
        "otg_notify_error_cb(): error = OTG_ERROR_OK. Shouldn't be used!!!" 
        == NULL);
      break;
    }
    case OTG_ERROR_SRP_FAILED:
    {
      /* The connected A-Device did not respond to the device's SRP within 32 seconds */
      HSU_ASSERT("otg_notify_error_cb(): error = OTG_ERROR_SRP_FAILED" == NULL);
      break;
    }
    case OTG_ERROR_VBUS:
    {
      /* The VBUS fell below its valid value: we need to shut it down 
      (disconnect the peripheral)*/
      HSU_MSG_ERROR("otg_notify_error_cb(): error = OTG_ERROR_VBUS",0,0,0);
      /* No need to lock the Giant mutex since we're called from the stack */
      if(otg_vbus_shutdown(otg_info.otg_core_ctx) != HSU_JSUCCESS)
      {
        HSU_ASSERT(
          "otg_notify_error_cb(): otg_vbus_shutdown failed" 
          == NULL);
      }
      hsu_api_vbus_shutdown();
      break;
    }
    default:
    {
      HSU_MSG_ERROR(
        "otg_notify_error_cb(): unknown error value %d", 
        err,0,0);
      HSU_ASSERT("otg_notify_error_cb(): unknown error value" == NULL);
      break;
    }
  }
} /* otg_notify_error_cb */

/*==============================================================================

FUNCTION          OTG_NOTIFY_DEVICE_MODE_CB

DESCRIPTION
  Called to inform of the current device mode (Host or Peripheral).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==============================================================================*/
static void otg_notify_device_mode_cb
(
  otg_appctx_t      appctx, 
  otg_device_mode_t mode
)
{
  hsu_otg_app_info_type * otg_context = 
    (hsu_otg_app_info_type *)appctx;
  HSU_ASSERT(&otg_info == otg_context);
  HSU_USE_PARAM(otg_context);

  HSU_MSG_HIGH(
    "otg_notify_device_mode_cb(): Setting Device State - %d (0=N,1=H,2=P)", 
    mode,0,0);

  /* TODO: Supply a callback mechanism for charger. For now, invoke directly: */
  if(otg_info.otg_mode != mode)
  {
    HSU_MSG_HIGH(
      "otg_notify_device_mode_cb(): Switching mode from %d to %d (0=None,1=H,2=P)", 
      otg_info.otg_mode, mode, 0);

#ifdef FEATURE_HS_USB_CHARGING

    if (mode == OTG_DEVICE_MODE_PERIPHERAL)
    {
        hsu_conf_sel_stack_utils_handle_chg_connected();
    }
    else if ((otg_info.otg_mode == OTG_DEVICE_MODE_PERIPHERAL) && 
      (mode == OTG_DEVICE_MODE_NONE))
    {
        /* In case of disconnection and previously device, request disconnect. */
        hsu_conf_sel_stack_utils_handle_chg_disconnected();
    }
    /* else: 
    - If previously this was a host, ignore going back to no device. 
    Or
    - If moving to a host, ignore this, as charging is irrelevant.
    */

#endif /* FEATURE_HS_USB_CHARGING */

#ifdef FEATURE_HS_USB_CORE_RESET_ON_DISC
	/* 
     * Workaround for stuck PHY: If we were in device mode and there is a 
     * disconnection, we reset the core and PHY. 
	 */
    if (otg_info.otg_mode == OTG_DEVICE_MODE_PERIPHERAL &&
        mode == OTG_DEVICE_MODE_NONE)
	{
      hsu_al_task_enqueue_cmd(hsu_conf_sel_stack_utils_device_restart_core, 
        (void *)&otg_context->otg_core_index);
	}
#endif /* ifdef FEATURE_HS_USB_CORE_RESET_ON_DISC */

    /* Now set the new mode */
    otg_info.otg_mode = mode;

    if (mode == OTG_DEVICE_MODE_HOST)
    {
      /* Send debug event on success */
      event_report(EVENT_HS_USB_ENTER_HOST_MODE );
    }

  }
} /* otg_notify_device_mode_cb */

/*==============================================================================
FUNCTION          OTG_NOTIFY_B_PERIPHERAL_STATE_TRANSITION_CB
 
DESCRIPTION
Called to inform of a transition to b_peripheral_state and that Vbus rised above
B-Device Session Valid threshold.
 
DEPENDENCIES
None.
 
RETURN VALUE
None.
 
SIDE EFFECTS
None.
 
==============================================================================*/
static void  otg_notify_b_peripheral_state_transition_cb
(
otg_appctx_t  appctx
)
{
    hsu_otg_app_info_type * otg_context =
		 (hsu_otg_app_info_type *)appctx;
    HSU_ASSERT(&otg_info == otg_context);
    HSU_USE_PARAM(otg_context);
    HSU_MSG_HIGH(" otg_notify_b_peripheral_state_transition_cb: Enter ",0,0,0);
    /* Add relevant code here */
    HSU_MSG_HIGH(" otg_notify_b_peripheral_state_transition_cb: Exit ",0,0,0);
} /*otg_notify_b_peripheral_state_transition_cb*/

/*==============================================================================

FUNCTION          OTG_NOTIFY_DEVICE_CONNECT_STATUS_CB

DESCRIPTION
  Called to inform that a USB mini A cable is connected/disconnected. 
  A B-device may or may not be connected to the cable.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==============================================================================*/
static void otg_notify_device_connect_status_cb
(
  otg_appctx_t  appctx, 
  jbool_t       status
)
{
  hsu_otg_app_info_type * otg_context = 
    (hsu_otg_app_info_type *)appctx;

  HSU_MSG_HIGH(
    "otg_notify_device_connect_status_cb(): Enter, connected?=%d", 
    status,0,0);

  if(status == DEVICE_STATUS_CONNECTED)
  {
    otg_context->mini_a_connected = TRUE;
    hsu_api_vbus_powerup();
    /* No need to lock the Giant mutex since we're called from the stack */
    if(otg_vbus_powerup(otg_info.otg_core_ctx) != HSU_JSUCCESS)
    {
      HSU_ASSERT(
        "otg_notify_device_connect_status_cb(): otg_vbus_powerup failed" 
        == NULL);
    }
  }
  else /* (status == DEVICE_STATUS_DISCONNECTED) */
  {
    /* No need to lock the Giant mutex since we're called from the stack */
    if(otg_vbus_shutdown(otg_info.otg_core_ctx) != HSU_JSUCCESS)
    {
      HSU_ASSERT(
        "otg_notify_device_connect_status_cb(): otg_vbus_shutdown failed" 
        == NULL);
    }
    hsu_api_vbus_shutdown();
    otg_context->mini_a_connected = FALSE;
  }
} /* otg_notify_device_connect_status_cb */

/*==============================================================================

FUNCTION          OTG_RESPOND_SRP_CB

DESCRIPTION
  Informs that the connected B-device has initiated SRP after we instructed the 
  OTG Core to drop the VBUS.
  This should not happen when basic OTG is used, since we won't drop the VBUS!

DEPENDENCIES
  None.

RETURN VALUE
  ASSERTs is called.

SIDE EFFECTS
  ASSERT

==============================================================================*/
static jbool_t otg_respond_srp_cb(otg_appctx_t appctx)
{
  HSU_USE_PARAM(appctx);
  HSU_ASSERT("otg_respond_srp_cb() shouldn't be called for basic OTG!" == NULL);
  return FALSE;
} /* otg_respond_srp_cb */

/*==============================================================================

FUNCTION          OTG_APP_UNINIT_CB

DESCRIPTION
  Un-initializes the OTG application.

DEPENDENCIES
  None.

RETURN VALUE
  HSU_JSUCCESS for success.

SIDE EFFECTS
  None.

==============================================================================*/
static jresult_t otg_app_uninit_cb(otg_appctx_t appctx)
{
  hsu_otg_app_info_type * otg_context = 
    (hsu_otg_app_info_type *)appctx;
  HSU_ASSERT(&otg_info == otg_context);
  HSU_USE_PARAM(otg_context);

  HSU_MSG_HIGH(
    "otg_app_uninit_cb(): Enter", 0,0,0);

  /* TODO - how should we handle this? */
  return HSU_JSUCCESS;
} /* otg_app_uninit_cb */

/*==============================================================================

FUNCTION          OTG_APP_INIT

DESCRIPTION
  OTG application initialization function, which is called as part of the OTG 
  stack’s initialization.
  The function receives and stores the OTG Core context required by the OTG Core
  functions, and returns to the OTG Core the application’s callback functions 
  and the application context to be passed to these callbacks.

  The function is called by the OTG core, following the call to jstart_stack 
  with init_opt == UW_INIT_OTG.

DEPENDENCIES
  None.

RETURN VALUE
  HSU_JSUCCESS for success.

SIDE EFFECTS
  None.

==============================================================================*/
jresult_t otg_app_init
(
  otg_ctx_t             core_ctx, 
  otg_app_callbacks_t * otg_app_cbs, 
  otg_app_info_t *      otg_app_info, 
  otg_appctx_t *        appctx
)
{
  /* Initialize the OTG Application's context */

  /* Store the OTG Core context */
  otg_info.otg_core_ctx = core_ctx;
  otg_info.otg_mode = OTG_DEVICE_MODE_NONE;
  otg_info.mini_a_connected = FALSE;
  otg_info.otg_core_index = 0;

  /* Assign the OUT parameters */
  /*****************************/
  /* Initialize the callbacks struct */
  otg_app_cbs->notify_peripheral_state = otg_notify_peripheral_state_cb;
  otg_app_cbs->notify_hnp_state = otg_notify_hnp_state_cb;
  otg_app_cbs->notify_srp_state = otg_notify_srp_state_cb;
  otg_app_cbs->notify_error = otg_notify_error_cb;
  otg_app_cbs->notify_device_mode = otg_notify_device_mode_cb;
  otg_app_cbs->notify_device_connect_status = otg_notify_device_connect_status_cb;
  otg_app_cbs->respond_srp = otg_respond_srp_cb;
  otg_app_cbs->otg_app_uninit = otg_app_uninit_cb;
  otg_app_cbs->notify_b_peripheral_state_transition = otg_notify_b_peripheral_state_transition_cb;

  /* Initialize the Application info struct */
  otg_app_info->drop_vbus_timeout = DROP_VBUS_TIMEOUT_INFINITE; 

  /* Initialize the Application context */
  *appctx = (void*)(&otg_info); 

  #ifndef FEATURE_HS_USB_BASIC
  /*Set up whitelist*/
  return hsu_otg_app_set_up_whitelist();
  #else
  return HSU_JSUCCESS;
  #endif  /*FEATURE_HS_USB_BASIC*/
} /* otg_app_init */

/*==============================================================================

FUNCTION          HSU_OTG_APP_GET_OTG_MODE

DESCRIPTION
  Returns the current OTG mode of the stack.

DEPENDENCIES
  Should be called after otg_app_init was called. 

RETURN VALUE
  OTG_DEVICE_MODE_NONE / OTG_DEVICE_MODE_HOST / OTG_DEVICE_MODE_PERIPHERAL

SIDE EFFECTS
  None. 

==============================================================================*/
otg_device_mode_t hsu_otg_app_get_otg_mode(void)
{
  return otg_info.otg_mode;
} /* hsu_otg_app_get_otg_mode */

/*==============================================================================

FUNCTION          HSU_OTG_APP_IS_MINI_A_CONNECTED

DESCRIPTION
  Returns whether a USB mini A cable is connected/disconnected. 
  A B-device may or may not be connected to the cable.

DEPENDENCIES
  None. 

RETURN VALUE
  TRUE if USB mini A cable is connected.

SIDE EFFECTS
  None. 

==============================================================================*/
boolean hsu_otg_app_is_mini_a_connected(void)
{
  return otg_info.mini_a_connected;
} /* hsu_otg_app_is_mini_a_connected */

/*==============================================================================

FUNCTION          HSU_OTG_APP_SET_UP_CORE_INDEX

DESCRIPTION
  Called from hsu_conf_sel_stack_utils_setup_stack, save the current initiated 
  core index in the otg application data

DEPENDENCIES
  None. 

RETURN VALUE
  None.

SIDE EFFECTS
  None. 

==============================================================================*/
void hsu_otg_app_set_core_index(uint32 core_idx)
{
  otg_info.otg_core_index = core_idx;
}

/*==============================================================================

FUNCTION          HSU_OTG_APP_SET_UP_WHITELIST

DESCRIPTION
  Called from hsu_conf_sel_stack_utils_setup_stack when 
  stack_init_option = HSU_CORE_CONFIG_INIT_MODE_OTG

DEPENDENCIES
  None. 

RETURN VALUE
  HSU_JSUCCESS for success.

SIDE EFFECTS
  None. 

==============================================================================*/
jresult_t hsu_otg_app_set_up_whitelist(void)
{
  peripheral_matcher_t  whitelist[HSU_OTG_APP_WHITELIST_SIZE];
  jresult_t             jresult;

  memset(whitelist, 0, sizeof(whitelist));

  /* Electrical Testing equipment */
  whitelist[0].flags = PERIPH_MATCH_BY_VENDOR_ID;
  whitelist[0].vendor_id = HSU_OTG_APP_TEST_DEVICE_VID;

  /* MS Class driver */
  whitelist[1].flags = PERIPH_MATCH_BY_USB_CLASS;
  whitelist[1].usb_class = HSU_OTG_APP_MSC_CLASS;

  /* HID Class driver */
  whitelist[2].flags = PERIPH_MATCH_BY_USB_CLASS;
  whitelist[2].usb_class = HSU_OTG_APP_HID_CLASS;

  /* Audio Class driver */
  whitelist[3].flags = PERIPH_MATCH_BY_USB_CLASS;
  whitelist[3].usb_class = HSU_OTG_APP_AUDIO_CLASS;

#ifdef FEATURE_HS_USB_UICC
  /* Add EEM & ICCD support for OTG core for debug purposes */
  /* ICCD Class driver */
  whitelist[4].flags = PERIPH_MATCH_BY_USB_CLASS;
  whitelist[4].usb_class = HSU_OTG_APP_ICCD_CLASS;

  /* EEM Class driver */
  whitelist[5].flags = PERIPH_MATCH_BY_USB_CLASS | 
    PERIPH_MATCH_BY_USB_SUB_CLASS;
  whitelist[5].usb_class = HSU_OTG_APP_CDC_CLASS;
  whitelist[5].usb_sub_class= HSU_OTG_APP_CDC_EEM_SUBCLASS;
#endif /* FEATURE_HS_USB_UICC */

  jresult = otg_set_whitelist(
      otg_info.otg_core_ctx, 
      HSU_OTG_APP_WHITELIST_SIZE, 
      (peripheral_matcher_t *)(whitelist));
  if(jresult != HSU_JSUCCESS)
  {
    HSU_MSG_ERROR(
      "*_set_up_whitelist(): otg_set_whitelist failed with %d", 
      jresult,0,0);
    HSU_ASSERT("*_set_up_whitelist(): otg_set_whitelist failed" 
      == NULL);
    return jresult;
  }
  return jresult;
}

