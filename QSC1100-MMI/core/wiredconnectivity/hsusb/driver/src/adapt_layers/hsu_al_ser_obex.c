/*==============================================================================

USB Serial I/F OBEX (DIAG/NMEA) Adaptation Layer for High Speed USB

GENERAL DESCRIPTION
Contains the implementation of the API for the serial functions' over OBEX 
adapt. layer

EXTERNALIZED FUNCTIONS
hsu_al_ser_obex_populate_obex_specific_data

INITIALIZATION AND SEQUENCING REQUIREMENTS
hsu_al_ser_base_init must be called for each function instance, 
prior to performing any other operations on the instance.

Copyright (c) 2006, 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/adapt_layers/hsu_al_ser_obex.c#21 $

when      who     what, where, why
--------  ---     ------------------------------------------------------
06/29/06  esh     First revision of file
==============================================================================*/
/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "hsu_al_ser_obex.h"

#include "customer.h" /* For FEATURE_HSU_TEST to work */

#include "hsu_al_ser_base_i.h"
#include "hsu_common.h"
#include "hsu_bus_mode_notifier.h"

#ifdef FEATURE_HSU_TEST
#error code not present
#endif /* FEATURE_HSU_TEST */

#ifndef T_REXNT
#include "hsu_al_ser.h"
#endif

#include "jos.h"
#include "jusb_core.h"
#include "cdc_fd.h"

#if defined (T_UNDP) && defined (FEATURE_HS_USB_NMEA) && defined (FEATURE_CGPS_LBS_TASK) 
#include "hsu_core_config.h"
#endif /* T_UNDP && FEATURE_HS_USB_NMEA && FEATURE_CGPS_LBS_TASK */
/*==============================================================================

LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
Constants and Macros
==============================================================================*/
#define HSU_AL_SER_OBEX_SUBCLASS         0xFF
#define HSU_AL_SER_OBEX_PROTOCOL         0xFF

#if defined (T_UNDP) && defined (FEATURE_HS_USB_NMEA) && defined (FEATURE_CGPS_LBS_TASK) 
/* Assuming NMEA is always the 2nd OBEX instance since in UNDP there is no Ser3 */
#define HSU_AL_SER_OBEX_NMEA_INSTANCE_NO 2
#endif /* T_UNDP && FEATURE_HS_USB_NMEA && FEATURE_CGPS_LBS_TASK */
/*==============================================================================
Typedefs
==============================================================================*/

/*==============================================================================
Variables
==============================================================================*/
/*---------------------------------------------------------------------------
FORWARD DECLARATIONS
---------------------------------------------------------------------------*/
static boolean al_ser_obex_smd_ioctl
(
  hsu_al_ser_obex_data* obex_data_ptr, 
  sio_ioctl_cmd_type ioctl_cmd, 
  sio_ioctl_param_type* ioctl_param_ptr
);

static jresult_t hsu_al_ser_obex_request_handler
(
  cdc_appctx_t ctx, 
  juint8_t bRequest,
  juint16_t wValue, 
  juint16_t wIndex, 
  juint16_t wLength
);

/*==============================================================================

EXTERNAL FUNCTION DEFINITIONS FOR MODULE

==============================================================================*/
/*===========================================================================
FUNCTION hsu_al_ser_obex_populate_obex_specific_data()
===========================================================================*/
void hsu_al_ser_obex_populate_obex_specific_data
(
  hsu_al_ser_obex_data* obex_data_ptr,
  cdc_handle_t handle,
  obex_callbacks_t* cbs,
  obex_init_info_t* info,
  cdc_appctx_t* ctx,
  const uint8** num_of_initialized_obex_instances
)
{
#if defined (T_UNDP) && defined (FEATURE_HS_USB_NMEA) && defined (FEATURE_CGPS_LBS_TASK) 
  core_t *core = ((cdc_ctx_t*)handle)->core_ctx;
  uint8 config_index = core->curr_config - 1;
#endif /* T_UNDP && FEATURE_HS_USB_NMEA && FEATURE_CGPS_LBS_TASK */

  HSU_ASSERT(obex_data_ptr != NULL);
  HSU_ASSERT(handle != NULL);
  HSU_ASSERT(cbs != NULL);
  HSU_ASSERT(info != NULL);
  HSU_ASSERT(ctx != NULL);

  /* Save context for later callbacks */
  *ctx = (cdc_appctx_t)(obex_data_ptr);

  cbs->basic.read_complete_h = hsu_al_ser_base_i_rx_done;
  cbs->basic.write_complete_h = hsu_al_ser_base_i_tx_done;
  cbs->basic.uninit_h = hsu_al_ser_obex_uninit;

  /* TODO: Fill more callbacks, for now they are NULL */
  cbs->basic.enabled_h = hsu_al_ser_obex_enabled;
  cbs->basic.disabled_h = hsu_al_ser_obex_disabled;

  /* And these will never be used in OBEX: */
  cbs->basic.encap_command_h = NULL;
  cbs->basic.encap_response_complete_h = NULL;

  cbs->basic.resumed_h = hsu_al_ser_base_i_resumed;

  cbs->request_handler_cb = hsu_al_ser_obex_request_handler;
  /* Init info struct */
  info->basic.flags = (juint32_t)(CDC_INIT_BUFFER_DMABLE | 
                                  CDC_INIT_SINGLE_INTERFACE |
                                  CDC_INIT_VENDOR_SPECIFIC_CLASS |
                                  CDC_INIT_NO_CLASS_DESC |
                                  CDC_INIT_NO_ALT_SETTINGS);

#if defined (T_UNDP) && defined (FEATURE_HS_USB_NMEA) && defined (FEATURE_CGPS_LBS_TASK) 
  /* Check whether this is the the NMEA instance of OBEX */
  if (obex_data_ptr->obex_if_data.core_idx == HSU_CORE_IDX_PRIMARY && 
    (num_of_initialized_obex_instances[HSU_CORE_IDX_PRIMARY][config_index]) 
    == HSU_AL_SER_OBEX_NMEA_INSTANCE_NO)
  {
    /* set the flag CDC_INIT_BI_DIR_NMEA so we could distinguish the NMEA instance of OBEX 
    from other instances of OBEX */
    info->basic.flags |= (juint32_t)CDC_INIT_BI_DIR_NMEA;
    /* cleare CDC_INIT_NO_CLASS_DESC so MDLM descriptors could be used */
    info->basic.flags &= ~(juint32_t)CDC_INIT_NO_CLASS_DESC;
  }
  
#endif /* T_UNDP && FEATURE_HS_USB_NMEA && FEATURE_CGPS_LBS_TASK */
 
  /* Enable support for ZLP */
  info->basic.zlp_terminator = TRUE;

  info->basic.cci_subclass = HSU_AL_SER_OBEX_SUBCLASS;
  info->basic.cci_protocol = HSU_AL_SER_OBEX_PROTOCOL;

#ifdef FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND

  /* In order for the MDLM descriptors to appear, 
  CDC_INIT_NO_CLASS_DESC must not be used. */
  info->basic.flags &= ~(juint32_t)CDC_INIT_NO_CLASS_DESC;

#endif /* FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND */

  /* Communication interface string descriptor */
  info->basic.comm_string = NULL;
  /* data interface string descriptor */
  info->basic.data_string = NULL;


}

/*===========================================================================
FUNCTION hsu_al_ser_obex_ioctl()
===========================================================================*/
boolean hsu_al_ser_obex_ioctl
(
  hsu_al_ser_obex_data* obex_data_ptr,
  sio_ioctl_cmd_type ioctl_cmd,
  sio_ioctl_param_type* ioctl_param_ptr
)
{
  boolean data_ptr_is_valid = FALSE;

  HSU_ASSERT(ioctl_param_ptr != NULL);

  data_ptr_is_valid = hsu_al_ser_base_i_ctx_is_valid((hsu_al_ser_base_data_for_ser_if*)obex_data_ptr);
  HSU_ASSERT(data_ptr_is_valid);

  /* First try the "base class": */
  if (hsu_al_ser_base_ioctl((hsu_al_ser_base_data_for_ser_if*)obex_data_ptr, ioctl_cmd, ioctl_param_ptr) == TRUE)
  {
    /* Handled in "base" */
    return TRUE;
  }

  /* Handle DTR */
  switch(ioctl_cmd) 
  {
    /* Control Line State Commands */
  case SIO_IOCTL_ENABLE_DTR_EVENT:
    {
      HSU_MSG_LOW("*_obex_ioctl: handling I/O Ctl: SIO_IOCTL_ENABLE_DTR_EVENT", 0, 0, 0);

      obex_data_ptr->obex_if_data.app_dtr_changed_cb_func = 
        (sio_vv_func_ptr_type)((*ioctl_param_ptr).enable_dte_ready_event);

      if (obex_data_ptr->obex_if_data.app_dtr_changed_cb_func != NULL)
      {
        obex_data_ptr->obex_if_data.app_dtr_changed_cb_func();
      }

#if defined(FEATURE_HSU_TEST) && !defined(T_REXNT)
#error code not present
#endif /* FEATURE_HSU_TEST && !T_REXNT */

      break;
    }
  case SIO_IOCTL_DISABLE_DTR_EVENT:
    {
      HSU_MSG_LOW("*_obex_ioctl: handling I/O Ctl: SIO_IOCTL_DISABLE_DTR_EVENT", 0, 0, 0);
#if defined(FEATURE_HSU_TEST) && !defined(T_REXNT)
#error code not present
#endif /* FEATURE_HSU_TEST && !T_REXNT */

      obex_data_ptr->obex_if_data.app_dtr_changed_cb_func = NULL;
      break;
    }
  case SIO_IOCTL_DTE_READY_ASSERTED:
    {
      HSU_MSG_LOW("*_obex_ioctl: handling I/O Ctl: SIO_IOCTL_DTE_READY_ASSERTED", 0, 0, 0);
      *(ioctl_param_ptr->dte_ready_asserted) = obex_data_ptr->obex_if_data.dte_ready;
      break;
    }
    default:
    {
      /* If the IOCTL is not handled here - Handle the following set of SMD I/O Ctls*/
      return al_ser_obex_smd_ioctl(obex_data_ptr, ioctl_cmd, ioctl_param_ptr);
    }
  }
  return TRUE;
}

#ifdef FEATURE_HSU_TEST
#error code not present
#endif /* FEATURE_HSU_TEST */

static jresult_t hsu_al_ser_obex_request_handler
(
  cdc_appctx_t ctx, 
  juint8_t bRequest,
  juint16_t wValue, 
  juint16_t wIndex, 
  juint16_t wLength
)
{
  HSU_USE_PARAM(ctx);
  HSU_USE_PARAM(bRequest);
  HSU_USE_PARAM(wValue);
  HSU_USE_PARAM(wIndex);
  HSU_USE_PARAM(wLength);
  /* The reason to change is as following:
     The USB CV MSC ClassRequestTest_DeviceConfigured 
     issues a Bulk-Only Mass Storage Reset request with 
     wValue != 0 and wLength != 0 errors (step 10)on interface 0 
     (wIndex = 0) instead of a real Mass Storage interface 2.
     As a result, this requests are routed to this function that 
     supports no requests but does not notify about that.
  */
  return JENOTSUP;
}

/*===========================================================================

FUNCTION al_ser_obex_smd_ioctl

DESCRIPTION
Handles I/O Ctl commands coming from SMD to the specified OBEX I/F.

DEPENDENCIES
None

RETURN VALUE
TRUE - I/O Ctl command handled. Otherwise - Failure. 

SIDE EFFECTS
None

===========================================================================*/
static boolean al_ser_obex_smd_ioctl
(
  hsu_al_ser_obex_data* obex_data_ptr, 
  sio_ioctl_cmd_type ioctl_cmd, 
  sio_ioctl_param_type* ioctl_param_ptr
)
{
/* Handles the following I/O CTLs:

  SIO_IOCTL_INBOUND_FLOW_ENABLE
  SIO_IOCTL_INBOUND_FLOW_DISABLE


  SIO_IOCTL_DSR_ASSERT
  SIO_IOCTL_DSR_DEASSERT  

  SIO_IOCTL_CD_ASSERT
  SIO_IOCTL_CD_DEASSERT

  SIO_IOCTL_RI_ASSERT
  SIO_IOCTL_RI_DEASSERT

*/

  HSU_ASSERT(obex_data_ptr != NULL);
  HSU_ASSERT(ioctl_param_ptr != NULL);

  HSU_PARAM_COULD_BE_CONST(obex_data_ptr);
  HSU_PARAM_COULD_BE_CONST(ioctl_param_ptr);

  switch (ioctl_cmd)
  {
    /* Do nothing for the following: */
    case SIO_IOCTL_INBOUND_FLOW_ENABLE:
    case SIO_IOCTL_INBOUND_FLOW_DISABLE:
    case SIO_IOCTL_DSR_ASSERT:
    case SIO_IOCTL_DSR_DEASSERT:
    case SIO_IOCTL_CD_ASSERT:
    case SIO_IOCTL_CD_DEASSERT:
    case SIO_IOCTL_RI_ASSERT:
    case SIO_IOCTL_RI_DEASSERT:
    {
      break;
    }

    default:
    {
      HSU_MSG_ERROR("al_ser_obex_smd_ioctl: cmd %u not supported", 
        ioctl_cmd, 0, 0);
      return FALSE;
    }
  }
  return TRUE;
}

/*==============================================================================

FUNCTION      HSU_AL_SER_OBEX_SUSPEND_RESUME_MODE_CHANGED_CB

DESCRIPTION   
Invoked when the USB stack gets suspended/resumed.

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
If the application has registered a callback, it will be invoked.

==============================================================================*/
static void hsu_al_ser_obex_suspend_resume_mode_changed_cb
(
 hsu_bus_mode_enum new_mode,
 void* app_data
)
{
  hsu_al_ser_obex_data* obex_data_ptr = NULL;

  HSU_MSG_HIGH("_suspend_resume_mode_changed_cb(): new mode == %d (0=S,1=R)",
    new_mode,0,0);  

  obex_data_ptr = (hsu_al_ser_obex_data*)app_data;

  HSU_ASSERT(obex_data_ptr != NULL);

  /* Check if the the dte ready state should be changed */
  if (((HSU_BUS_MODE_SUSPENDED == new_mode) && (TRUE == obex_data_ptr->obex_if_data.dte_ready)) ||
    ((HSU_BUS_MODE_RESUMED == new_mode) && (FALSE == obex_data_ptr->obex_if_data.dte_ready)))
  {
    HSU_MSG_LOW("*_obex_suspend_resume_mode_changed_cb: dtr changed callback should be called", 0, 0, 0);

    /* Update the new state and call the application callback */
    obex_data_ptr->obex_if_data.dte_ready = !obex_data_ptr->obex_if_data.dte_ready;
    if (obex_data_ptr->obex_if_data.app_dtr_changed_cb_func != NULL)
    {
      HSU_MSG_LOW("*_obex_suspend_resume_mode_changed_cb: calling the dtr changed callback", 0, 0, 0);
      obex_data_ptr->obex_if_data.app_dtr_changed_cb_func();
    }
  }
}

/*===========================================================================

FUNCTION hsu_al_ser_obex_init

DESCRIPTION
Initializes OBEX AL specific fields.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_al_ser_obex_init
(
 hsu_al_ser_obex_data* obex_data_ptr          /* Pointer to OBEX Data struct */
 )
{
  hsu_bus_mode_cb_info_struct suspend_resume_callback_info;

  HSU_ASSERT(obex_data_ptr != NULL);

  /* Call base class' method: */
  hsu_al_ser_base_init(&(obex_data_ptr->base_data));

  suspend_resume_callback_info.bus_mode_cb = hsu_al_ser_obex_suspend_resume_mode_changed_cb;
  suspend_resume_callback_info.app_data = obex_data_ptr;
  if (hsu_register_bus_mode_notif_cb(suspend_resume_callback_info)
    == FALSE)
  {
    HSU_ASSERT((uint32)"hsu_al_ser_obex_init(): hsu_register_bus_mode_notif_cb failed" 
      == NULL);
  }

  /* Update obex specific fields: */
  obex_data_ptr->obex_if_data.app_dtr_changed_cb_func = NULL;
  obex_data_ptr->obex_if_data.dte_ready = FALSE;
}

/*===========================================================================

FUNCTION hsu_al_ser_obex_uninit

DESCRIPTION
Serves as a callback whenever the CDC FD wishes to notify it is being 
un-initialized, or as an initializer for the given context when the very first 
hsu_al_ser_base_init is called.

DEPENDENCIES
See cdc_uninit_cb for more information

RETURN VALUE
HSU_JSUCCESS - Always.

SIDE EFFECTS
The data structure corresponding to the given context is invalidated.

===========================================================================*/
jresult_t hsu_al_ser_obex_uninit
(
 cdc_appctx_t ctx                           /* Context passed in callback */
 )
{
  hsu_bus_mode_cb_info_struct suspend_resume_callback_info;

  hsu_al_ser_handle_obex_uninit(ctx);

  (void)hsu_al_ser_base_i_uninit(ctx);

  suspend_resume_callback_info.bus_mode_cb = hsu_al_ser_obex_suspend_resume_mode_changed_cb;
  suspend_resume_callback_info.app_data = ctx;
  hsu_unregister_bus_mode_notif_cb(suspend_resume_callback_info);

  return HSU_JSUCCESS;
}

/*===========================================================================

FUNCTION hsu_al_ser_obex_enabled

DESCRIPTION
Enabled callback whenever the serial CDC FD wishes to notify that the interface
is enabled.

DEPENDENCIES
See cdc_enabled_cb for more information

RETURN VALUE
HSU_JSUCCESS - Always

SIDE EFFECTS
None

===========================================================================*/
jresult_t hsu_al_ser_obex_enabled
(
 cdc_appctx_t ctx                           /* Context passed in callback */
 )
{
  hsu_al_ser_obex_data* obex_data_ptr = NULL;
  obex_data_ptr = (hsu_al_ser_obex_data*)ctx;

  HSU_MSG_LOW("*_obex_enabled: obex enabled", 0, 0, 0);

#ifndef T_REXNT
  /* Set the FD handle to the current configuration's FD */
  hsu_al_ser_handle_set_configuration(obex_data_ptr->obex_if_data.core_idx);
#endif /*T_REXNT */

  (void)hsu_al_ser_base_i_enabled(ctx, TRUE);

  if (obex_data_ptr->obex_if_data.dte_ready == TRUE)
  {
    HSU_MSG_LOW("*_obex_disabled: dte_ready is already TRUE", 0, 0, 0);
    return HSU_JSUCCESS;
  }

  obex_data_ptr->obex_if_data.dte_ready = TRUE;

  /* Calling the dtr changed callback if it is set*/
  if (obex_data_ptr->obex_if_data.app_dtr_changed_cb_func != NULL)
  {
    HSU_MSG_LOW("*_obex_enabled: calling the dtr changed callback", 0, 0, 0);
    obex_data_ptr->obex_if_data.app_dtr_changed_cb_func();
  }

  return HSU_JSUCCESS;
}

/*===========================================================================

FUNCTION hsu_al_ser_obex_disabled

DESCRIPTION
Disabled callback whenever the serial CDC FD wishes to notify that the interface
is disabled.

DEPENDENCIES
See cdc_disabled_cb for more information

RETURN VALUE
HSU_JSUCCESS - Always

SIDE EFFECTS
None

===========================================================================*/
jresult_t hsu_al_ser_obex_disabled
(
 cdc_appctx_t ctx                           /* Context passed in callback */
 )
{
  hsu_al_ser_obex_data* obex_data_ptr = NULL;
  obex_data_ptr = (hsu_al_ser_obex_data*)ctx;

  HSU_MSG_LOW("*_obex_disabled: obex disabled", 0, 0, 0);

  (void)hsu_al_ser_base_i_disabled(ctx);

  if (obex_data_ptr->obex_if_data.dte_ready == FALSE)
  {
    HSU_MSG_LOW("*_obex_disabled: dte_ready is already FALSE", 0, 0, 0);
    return HSU_JSUCCESS;
  }

  obex_data_ptr->obex_if_data.dte_ready = FALSE;

  /* Calling the dtr changed callback if it is set*/
  if (obex_data_ptr->obex_if_data.app_dtr_changed_cb_func != NULL)
  {
    HSU_MSG_LOW("*_obex_disabled: calling the dtr changed callback", 0, 0, 0);
    obex_data_ptr->obex_if_data.app_dtr_changed_cb_func();
  }

  return HSU_JSUCCESS;
}

/*===========================================================================
FUNCTION hsu_al_ser_obex_close()
===========================================================================*/
void hsu_al_ser_obex_close
(
 hsu_al_ser_obex_data* obex_data_ptr,
 hsu_al_ser_base_op_complete_cb close_cb,
 boolean abort_io_operations_immediately
)
{
  hsu_al_ser_base_close((hsu_al_ser_base_data_for_ser_if*)obex_data_ptr, 
                          close_cb, 
                          abort_io_operations_immediately);

  obex_data_ptr->obex_if_data.app_dtr_changed_cb_func = NULL;
  obex_data_ptr->obex_if_data.dte_ready = FALSE;
}
