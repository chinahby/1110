/*==============================================================================

  High Speed USB Configuration Selector UI
  
  GENERAL DESCRIPTION
    This file includes functions that are related to High Speed USB composition 
    changes through the UI.
    Here's an overview on the state machine and the actions taken. 

    Initial State - HSU_COMP_CHANGE_STATE_NONE
    ==========================================
    Device stack is active, and the current composition is one that can be 
    switched dynamically.

    Trigger: A composition change is ordered (hsu_conf_sel_ui_switch_comp() 
    =======  is invoked), current state is HSU_COMP_CHANGE_STATE_NONE.
    New state - HSU_COMP_CHANGE_STATE_CLOSING_APPS 
    ==============================================
    In this state we should order each application that is currently using a USB 
    port, to close that port. 
      * For each composition, we have a list of RDM devices that are USB ports, 
        that are included in this composition (for example, for the DIAG+NMEA+MDM 
        composition, the list indicates RDM_USB_DIAG_DEV, RDM_USB_NMEA_DEV, 
        RDM_USB_MDM_DEV). 
      * For each device in the list, we ask the RDM what service is using it 
        (by invoking rdm_get_service()). 
      * Then we order the service to close the device (by invoking rdm_assign_port() 
        with RDM_NULL_DEV  as the destination port). 
      * When the operation is completed, the RDM invokes a callback function. 
        Currently, we use a function already defined in the UI code. This 
        function sends an event using ISHELL_PostEventEx. This is to ensure all 
        actions are performed in the UI task’s context. 
      * The completion event is sent to the active dialog (which is ours), our 
        dialog’s event handler, when receiving the EVT_RDM_DONE event, calls 
        hsu_conf_sel_ui_rdm_done() to notify of the completion.

    Trigger:  hsu_conf_sel_ui_rdm_done()is called, current state is 
    =======   HSU_COMP_CHANGE_STATE_CLOSING_APPS.
    If we are not finished going over the entire list of RDM devices, we close 
    the next RDM device.
    If we are finished closing devices – 
    New state: HSU_COMP_CHANGE_STATE_STACK_RECONFIG
    ===============================================
    We call hsu_conf_sel_switch_dev_stack() to re-configure the device stack. 
    This callback is be invoked after the re-configure is completed. 

    Trigger: Stack re-configure completion callback is invoked (hsu_conf_sel_ui_reconfig_cb()),
    =======  current state is HSU_COMP_CHANGE_STATE_STACK_RECONFIG.
    New state - HSU_COMP_CHANGE_STATE_OPENING_APPS 
    ==============================================
    In this state we assign some of the services to a USB RDM device. 
    For each RDM USB device included in the new composition:
      - If this device,  when it was last seen included in a composition, was 
        used by an RDM service, order this service to open this device 
        (by invoking rdm_assign_port()).
      - For this, we use the rdm_service_per_usb_device variable, which serves 
        as a “memory”. Whenever we order an RDM service to close an RDM device,
        we store the 2 in this structure. 
      - When the operation is completed, the RDM invokes a the same callback 
        function as described above, which again, causes hsu_conf_sel_ui_rdm_done() 
        to be called. 

    Trigger:  hsu_conf_sel_ui_rdm_done()is called, current state is 
    =======   HSU_COMP_CHANGE_STATE_OPENING_APPS. 
    If we are not finished going over the entire list of RDM devices, we open 
    the next RDM device (if needed).
    If we are finished opening devices – 
    New state: HSU_COMP_CHANGE_STATE_COMPLETED
    ==========================================
    The composition switch is completed.
    After each call to hsu_conf_sel_ui_rdm_done(), there should be a call to 
    hsu_conf_sel_ui_comp_change_is_done(). When this function returns TRUE, the 
    composition change has completed. 

      
  EXTERNALIZED FUNCTIONS
    hsu_conf_sel_ui_dynamic_switch_is_allowed
    hsu_conf_sel_ui_rdm_done
    hsu_conf_sel_ui_comp_change_is_done

  INITALIZATION AND SEQUENCING REQUIREMENTS
    hsu_conf_sel_ui_comp_menu_init should be called during power up, before the 
    user can perform any UI actions.

  Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/common/hsu_conf_sel_ui.c#11 $
  $DateTime: 2009/02/11 06:48:45 $ $Author: ablay $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------
  12/04/06  ke      Initial version
  06/29/07  bt      Added MTP support 
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE
  
==============================================================================*/
#include "customer.h" /* for FEATURE_HSU_TEST to work */

/* 
   This file provides support dynamic configuration switching which is
   used by PictBridge and MTP.  If UI menu for dynamic configuration
   switching becomes availabe, the featurization may need to change to
   accomodate the UI menu. The main reason for this featurization is to
   save memory footprint for devices with small memory parts.
*/
#if 1//!defined(FEATURE_HS_USB_MEM_OPTIMIZATION) || \
    //defined(FEATURE_PICTBRIDGE_HS) || defined(FEATURE_HSU_MTP)

#include "hsu_conf_sel_ui.h"
#include "hsu_common.h"

#ifndef FEATURE_HSU_TEST
  #include "hsu_conf_sel_i.h"
  #include "hsu_conf_sel_stack_utils.h"
  #include "hsu_conf_sel_comp_utils.h"
  #include "uiutils.h"
#endif /* ! FEATURE_HSU_TEST */

#include "hsu_os_abstract_bus.h"

#ifdef FEATURE_HS_USB_ANALOG_AUDIO
#include "hsu_analog_audio.h"
#endif /*FEATURE_HS_USB_ANALOG_AUDIO*/

/*==============================================================================

                          DEFINITIONS AND DECLARATIONS FOR MODULE
  
==============================================================================*/
#ifndef T_REXNT
#ifndef FEATURE_HSU_TEST

/*==============================================================================
Constants and Macros
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/
typedef enum
{
  HSU_COMP_CHANGE_STATE_NONE,
  HSU_COMP_CHANGE_STATE_CLOSING_APPS,
  HSU_COMP_CHANGE_STATE_STACK_RECONFIG,
  HSU_COMP_CHANGE_STATE_OPENING_APPS,
  HSU_COMP_CHANGE_STATE_COMPLETED
} hsu_conf_sel_ui_comp_change_state_enum; 

/* 
 * A pair of RDM device and RDM service, representing that the specific RDM 
 * device was used by the RDM service.
 */
typedef struct
{
  rdm_device_enum_type  rdm_device;
  rdm_service_enum_type rdm_service;
} hsu_conf_sel_ui_comp_rdm_device_service_type;

#endif /* ! FEATURE_HSU_TEST */

/*==============================================================================
 Static variables 
==============================================================================*/
#ifndef FEATURE_HSU_TEST


/* The current index of application we need to open/close using the RDM */
static uint8 current_device_index = 0;

static hsu_conf_sel_ui_comp_change_state_enum comp_change_state = 
  HSU_COMP_CHANGE_STATE_NONE;

static const hsu_conf_sel_comp_info_type *  new_comp_details = NULL;
static const hsu_conf_sel_comp_info_type *  cur_comp_details = NULL;
static rdm_assign_port_cb_type              rdm_completion_cb_func_ptr = NULL;

/*
 * This array is our "memory". For each RDM device, which is a USB port, it 
 * stores the last application that used this device.
 * Initialized to RDM_NULL_SRVC, to state that we have no memory for this device.
 */
static hsu_conf_sel_ui_comp_rdm_device_service_type 
  rdm_service_per_usb_device[HSU_NUM_OF_RDM_USB_PORTS] = 
  {
    {RDM_USB_MDM_DEV,       RDM_NULL_SRVC},
    {RDM_USB_MMC_DEV,       RDM_NULL_SRVC},
    {RDM_USB_DIAG_DEV,      RDM_NULL_SRVC},
    {RDM_USB_NMEA_DEV,      RDM_NULL_SRVC},
    {RDM_USB_NET_WWAN_DEV,  RDM_NULL_SRVC},
  };

#endif /* ! FEATURE_HSU_TEST */

/*==============================================================================

                          FUNCTION DECLARATIONS FOR MODULE
  
==============================================================================*/


/*===========================================================================

FUNCTION            HSU_CONF_SEL_UI_DYNAMIC_SWITCH_IS_ALLOWED

DESCRIPTION
  Called from CoreMenu.c, in the UI task's context. 
  Returns TRUE if it is allowed to dynamically switch to a new composition, 
  FALSE otherwise. 
  This is determined according to the current composition.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if dynamic composition switching is allowed, FALSE if it isn't. 

SIDE EFFECTS
  None.

===========================================================================*/
boolean hsu_conf_sel_ui_dynamic_switch_is_allowed(void)
{
#ifdef FEATURE_HSU_TEST
#error code not present
#else /* ! FEATURE_HSU_TEST */

  HSU_MSG_HIGH("*_dynamic_switch_is_allowed(): Entered", 0, 0, 0 );

#ifdef FEATURE_HS_USB_ANALOG_AUDIO
  /* If USB core is in Analog Audio mode, we do nothing */
  if (hsu_analog_audio_get_curr_mode() != HSU_AUDIO_MODE_USB)
  {
    HSU_MSG_HIGH("*_dynamic_switch_is_allowed: USB core is in Analog Audio mode! "
      " Stack switch is not allowed  " ,0,0,0);
    return FALSE;
  }
#endif /*FEATURE_HS_USB_ANALOG_AUDIO*/

  /* If dynamic composition switching is already in progress, 
     another composition switching attempt is not allowed */
  if(HSU_COMP_CHANGE_STATE_NONE != comp_change_state)
  {
    HSU_MSG_ERROR("Dynamic comp switching not allowed: comp_change_state=%d",
        comp_change_state, 0, 0);
    return FALSE;
  }

  HSU_MSG_LOW("*_dynamic_switch_is_allowed(): returning TRUE",0,0,0);
  return TRUE;
#endif /* FEATURE_HSU_TEST */
} /* hsu_conf_sel_ui_dynamic_switch_is_allowed */


#ifndef FEATURE_HSU_TEST
/*===========================================================================

FUNCTION              FAKE_RDM_COMPLETE_EVENT

DESCRIPTION
  Called only from the UI task's context, during composition change, 
  and used to fake an RDM completion event, in order to continue the process
  properly.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void fake_rdm_complete_event(void)
{
  HSU_ASSERT(rdm_completion_cb_func_ptr != NULL);
  rdm_completion_cb_func_ptr(RDM_DONE_S, RDM_NULL_SRVC, RDM_SRVC_NOT_ALLOWED);
} /* fake_rdm_complete_event */
#endif /* ! FEATURE_HSU_TEST */

#ifndef FEATURE_HSU_TEST
/*===========================================================================

FUNCTION              handle_app_opening

DESCRIPTION
  Called from CoreMenu.c, in the UI task's context, during composition change, 
  following an RDM completion event, while in the state 
  HSU_COMP_CHANGE_STATE_OPENING_APPS.
  For the current application, checks if it should be assigned to a USB port, 
  and if so, asks the RDM to do so. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void handle_app_opening()
{
  rdm_device_enum_type current_device = RDM_SRVC_NOT_ALLOWED;
  rdm_service_enum_type prev_service_that_used_device = RDM_NULL_SRVC;
  uint8 i;

  if(new_comp_details->num_of_rdm_dev_in_comp == 0)
  {
    /* Fake RDM completion to keep the state machine going */
    HSU_MSG_LOW(
      "handle_app_opening(): no devices to open for this composition. Faking RDM completion",
      0,0,0);
    fake_rdm_complete_event();
    return;
  }

  HSU_ASSERT(current_device_index < new_comp_details->num_of_rdm_dev_in_comp);
  
  current_device = new_comp_details->rdm_devices[current_device_index];

  for ( i = 0 ; i < HSU_NUM_OF_RDM_USB_PORTS ; ++ i)
  {
    if(rdm_service_per_usb_device[i].rdm_device == current_device)
    {
      prev_service_that_used_device = rdm_service_per_usb_device[i].rdm_service;
      break;
    }
  }

  if(prev_service_that_used_device != RDM_NULL_SRVC)
  {
    /* If in the last time this service was active in a composition, it used a
    USB device, re-open the service on that USB device. */
    HSU_MSG_LOW(
      "handle_app_opening(): calling rdm_assign_port service=%d, device=%d",
      prev_service_that_used_device, current_device, 0);
    /* assign the application to the device it used in the composition we're leaving */ 
    if(rdm_assign_port(prev_service_that_used_device, 
      current_device,
      rdm_completion_cb_func_ptr) == FALSE)
    {
      /* Don't let this prevent us from completing the composition change... */
      HSU_MSG_ERROR(
        "handle_app_opening(): rdm_assign_port failed for service=%d, device=%d",
        prev_service_that_used_device, current_device, 0);
    }
    return; /* we found our application, and it got its USB port, so we're done */
  }

  /* The app wasn't assigned to a USB port. Fake RDM completion to move to the 
  next application in the new composition */
  HSU_MSG_LOW(
    "handle_app_opening(): device %d wasn't used by any service. Faking RDM completion",
    current_device,0,0);
  fake_rdm_complete_event();
} /* handle_app_opening */
#endif /* ! FEATURE_HSU_TEST */

#ifndef FEATURE_HSU_TEST
/*===========================================================================

FUNCTION              handle_app_closing

DESCRIPTION
  Called from CoreMenu.c, in the UI task's context, during composition change, 
  following an RDM completion event, while in the state 
  HSU_COMP_CHANGE_STATE_CLOSING_APPS.
  For the current application, checks if it uses a USB port, and if so, asks RDM
  to move it to the NULL device. 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE for success FALSE for failure (for the current application).

SIDE EFFECTS
  None.

===========================================================================*/
static boolean handle_app_closing()
{
  rdm_service_enum_type rdm_service = RDM_NULL_SRVC;
  rdm_device_enum_type  current_device = RDM_SRVC_NOT_ALLOWED;
  uint8 index_of_device_in_rdm_config = HSU_NUM_OF_RDM_USB_PORTS;
  uint8 i;

  if(cur_comp_details->num_of_rdm_dev_in_comp == 0)
  {
    /* Fake RDM completion to keep the state machine going */
    HSU_MSG_LOW(
      "handle_app_closing(): no devices to close for this composition. Faking RDM completion",
      0,0,0);
    fake_rdm_complete_event();
    return TRUE;
  }

  HSU_ASSERT(current_device_index < cur_comp_details->num_of_rdm_dev_in_comp);

  current_device = cur_comp_details->rdm_devices[current_device_index];
  
  for ( i = 0 ; i < HSU_NUM_OF_RDM_USB_PORTS ; ++ i)
  {
    if(rdm_service_per_usb_device[i].rdm_device  == current_device)
    {
      index_of_device_in_rdm_config = i;
      break;
    }
  }
  HSU_ASSERT(index_of_device_in_rdm_config < HSU_NUM_OF_RDM_USB_PORTS);
  
  rdm_service = rdm_get_service(cur_comp_details->rdm_devices[current_device_index]);
  if(rdm_service == RDM_NULL_SRVC) /* no application uses this USB port */
  {
    if(index_of_device_in_rdm_config < HSU_NUM_OF_RDM_USB_PORTS) /* To make lint happy */
    {
      rdm_service_per_usb_device[index_of_device_in_rdm_config].rdm_service = RDM_NULL_SRVC;
    }
    else
    {
      HSU_ASSERT(FALSE);
    }
    /* no service uses this USB device, so no need to restore when composition is restored */
    HSU_MSG_LOW(
      "handle_app_closing(): no application is using the USB device %d in this composition. Faking completion.",
      current_device,0,0);
    /* Fake the RDM event to continue the procedure. */
    fake_rdm_complete_event();
    return TRUE;
  }
  else
  {
    HSU_MSG_LOW(
      "handle_app_closing(): calling rdm_assign_port to NULL with service %d (using device %d).",
      rdm_service, current_device, 0);
    if(index_of_device_in_rdm_config < HSU_NUM_OF_RDM_USB_PORTS) /* To make lint happy */
    {
      rdm_service_per_usb_device[index_of_device_in_rdm_config].rdm_service = rdm_service;
    }
    else
    {
      HSU_ASSERT(FALSE);
    }
    if(rdm_assign_port(rdm_service, 
        RDM_NULL_DEV,
        rdm_completion_cb_func_ptr) == FALSE)
    {
      /* Returning FALSE causes composition change to be stopped, but we 
      don't bother re-opening application that were closed. */
      HSU_MSG_ERROR(
        "handle_app_closing(): rdm_assign_port to RDM_NULL_DEV failed for service=%d.",
        rdm_service, 0, 0);
      return FALSE;
    }
    else /* success */
    {
      return TRUE;
    }
  }
} /* handle_app_closing */
#endif /* ! FEATURE_HSU_TEST */

#ifndef FEATURE_HSU_TEST
/*===========================================================================

FUNCTION              HSU_CONF_SEL_UI_RECONFIG_CB

DESCRIPTION
  This callback function is given to hsu_conf_sel_switch_dev_stack(), as part of 
  the composition switching process. 
  The stack will invoke this callback function when the stack re-configure 
  process is completed. 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE for success FALSE for failure (for the current application).

SIDE EFFECTS
  None.

===========================================================================*/
static void hsu_conf_sel_ui_reconfig_cb
(
  void *      arg, 
  jresult_t   status
)
{
  hsu_conf_sel_comp_info_type * new_comp_as_arg = NULL;
  new_comp_as_arg = (hsu_conf_sel_comp_info_type *)(arg);

  HSU_MSG_MED("hsu_conf_sel_ui_reconfig_cb(): called with status = %d", 
    status,0,0);

  HSU_ASSERT(new_comp_as_arg->hsu_comp_id == new_comp_details->hsu_comp_id);

  if(status != HSU_JSUCCESS)
  {
    HSU_MSG_ERROR(
      "hsu_conf_sel_ui_reconfig_cb(): Re-Configure failed, status=%d",
      status,0,0);
    HSU_ASSERT("hsu_conf_sel_ui_reconfig_cb(): received status != SUCCESS" == NULL);
    /* TODO - handle this somehow... */
  }
  else
  {
    hsu_conf_sel_stack_utils_reconfig_done(new_comp_as_arg);
    fake_rdm_complete_event();
  }
} /* hsu_conf_sel_ui_reconfig_cb */
#endif /* ! FEATURE_HSU_TEST */

#ifndef FEATURE_HSU_TEST
/*===========================================================================

FUNCTION              HSU_CONF_SEL_UI_SWITCH_COMP

DESCRIPTION
  Called when composition change should be performed.
  Start the process of dynamically changing the current USB configuration. 
  Does so by setting internal variables, and starting the stage of closing 
  application that are using a USB port. 

DEPENDENCIES
  Must be called after stack initialization. 

RETURN VALUE
  TRUE for success (only indicates first step was successful), FALSE for failure.

SIDE EFFECTS
  None

===========================================================================*/
boolean hsu_conf_sel_ui_switch_comp
(
  hsu_conf_sel_composition_id_enum  new_usb_comp_id,
  rdm_assign_port_cb_type           rdm_comp_cb_func_ptr
)
{
  if(comp_change_state != HSU_COMP_CHANGE_STATE_NONE)
  {
    HSU_MSG_ERROR(
      "*_switch_comp(): called when state=%d, and not NONE (new_usb_comp_id=%d)", 
      new_usb_comp_id, 0, 0);
    HSU_ASSERT("*_switch_comp(): called when state != NONE" == NULL);
  }

  HSU_ASSERT(rdm_comp_cb_func_ptr != NULL);

  HSU_MSG_HIGH("*_switch_comp(): called with new_usb_comp_id=%d", 
    new_usb_comp_id,0,0);

#ifdef FEATURE_HS_USB_ANALOG_AUDIO
  /* If USB core is in Analog Audio mode, we do nothing */
  if (hsu_analog_audio_get_curr_mode() != HSU_AUDIO_MODE_USB)
  {
    HSU_MSG_HIGH("*_dynamic_switch_is_allowed: "
      "USB core is in Analog Audio mode! "
      " Stack switch is not allowed  " ,0,0,0);
    return FALSE;
  }
#endif /*FEATURE_HS_USB_ANALOG_AUDIO*/

  cur_comp_details = hsu_conf_sel_get_cur_comp_details();
  HSU_ASSERT(cur_comp_details != NULL);

  if (cur_comp_details->hsu_comp_id == new_usb_comp_id)
  {
    /* No change in the composition, do nothing */
    HSU_MSG_HIGH("*_switch_comp(): No change in the composition, "
                 "moving to COMPLETED state", 
                 0,0,0);
    comp_change_state = HSU_COMP_CHANGE_STATE_COMPLETED;

    /* Invoke RDM completion callback. */
    rdm_completion_cb_func_ptr = rdm_comp_cb_func_ptr;
    fake_rdm_complete_event();
    rdm_completion_cb_func_ptr = NULL;

    return TRUE;
  }

  new_comp_details = 
    hsu_conf_sel_comp_utils_get_comp_details_by_hsu_id(new_usb_comp_id);

  HSU_ASSERT(new_comp_details != NULL);
  rdm_completion_cb_func_ptr = rdm_comp_cb_func_ptr;


  hsu_os_bus_lpm_disable(0);

  /* First, use RDM to order all applications that use USB port to close USB port */
  current_device_index = 0;
  comp_change_state = HSU_COMP_CHANGE_STATE_CLOSING_APPS;
  if(handle_app_closing() == FALSE)
  {
    comp_change_state = HSU_COMP_CHANGE_STATE_COMPLETED;
    return FALSE;
  }
  return TRUE;
} /* hsu_conf_sel_ui_switch_comp */
#endif /* ! FEATURE_HSU_TEST */


#ifndef FEATURE_HSU_TEST
/*===========================================================================

FUNCTION              RESET_STATE_MACHINE_AND_DATA

DESCRIPTION
Called from CoreMenu.c, in the UI task's context, when an EVT_RDM_DONE is 
received, to check if this is the last EVT_RDM_DONE expected.

DEPENDENCIES
Must be called after stack initialization, during a composition change process. 

RETURN VALUE
TRUE if the process of composition change is completed when the function 
returns, FALSE if the process is not yet completed.

SIDE EFFECTS
None.

===========================================================================*/
void reset_state_machine_and_data(void)
{
  current_device_index = 0;
  comp_change_state = HSU_COMP_CHANGE_STATE_NONE;
  cur_comp_details = NULL;
  new_comp_details = NULL;
  rdm_completion_cb_func_ptr = NULL;
} /* reset_state_machine_and_data */
#endif /* ! FEATURE_HSU_TEST */

/*===========================================================================

FUNCTION              HSU_CONF_SEL_UI_RDM_DONE

DESCRIPTION
  Called from CoreMenu.c, in the UI task's context, when our dialog receive an 
  EVT_RDM_DONE. 
  It continues the process of application closing / opening according to the 
  current internal state.

DEPENDENCIES
  Must be called after stack initialization, during a composition change process. 

RETURN VALUE
  TRUE for success (only indicates the current step was successful), FALSE for failure.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hsu_conf_sel_ui_rdm_done(void)
{
#ifndef FEATURE_HSU_TEST
  HSU_MSG_HIGH("*_rdm_done(): Entered. Current state=%d", 
    comp_change_state, 0, 0 );
  switch(comp_change_state)
  {
    case HSU_COMP_CHANGE_STATE_CLOSING_APPS:
    {
      if(cur_comp_details->num_of_rdm_dev_in_comp > 0)
      {
        HSU_ASSERT(current_device_index < cur_comp_details->num_of_rdm_dev_in_comp);
        
        HSU_MSG_LOW(
          "*_rdm_done(): closing of service that used device = %d completed", 
          cur_comp_details->rdm_devices[current_device_index],0,0);

        /* move to the next application that needs to be closed */
        ++current_device_index;
      }

      if((cur_comp_details->num_of_rdm_dev_in_comp > 0) &&
         (current_device_index < cur_comp_details->num_of_rdm_dev_in_comp))
      {
        if(handle_app_closing() == FALSE)
        {
          HSU_MSG_ERROR(
            "*_rdm_done(): handle_app_closing failed for device_index=%d, returning FALSE", 
            current_device_index,0,0);
          reset_state_machine_and_data();
          return FALSE;
        }
        return TRUE;
      }
      else
      {
        /* 
         * We're done closing Application's USB port! 
         * Time to stop the current stack and initialize the new device stack 
         * according to the new composition
         */
        HSU_MSG_MED("*_rdm_done(): calling hsu_conf_sel_switch_dev_stack()", 
          0,0,0);
        comp_change_state = HSU_COMP_CHANGE_STATE_STACK_RECONFIG;
        if(hsu_conf_sel_switch_dev_stack(
            new_comp_details->hsu_comp_id, 
            hsu_conf_sel_ui_reconfig_cb,
            (void*)new_comp_details) == FALSE) 
            /* TODO - change the last arg to whatever needed in the completion callback */
        {
          /* TODO - make sure if there's a failure, we at least uninit the 
          current stack */
          HSU_MSG_ERROR(
            "*_rdm_done(): hsu_conf_sel_switch_dev_stack failed", 0,0,0);
          HSU_ASSERT(
            (char *)"*_rdm_done(): hsu_conf_sel_switch_dev_stack failed" 
            == NULL);
          reset_state_machine_and_data();
          return FALSE;
        }
        return TRUE;
      }
    }
    case HSU_COMP_CHANGE_STATE_STACK_RECONFIG:
    {
      HSU_MSG_MED("*_rdm_done(): stack re-configure completed. Openning apps.", 
        0,0,0);

      /* 
       * Use RDM to order the applications that are included in the current 
       * composition to open the USB port, if they used it in the prior 
       * composition 
       */
      comp_change_state = HSU_COMP_CHANGE_STATE_OPENING_APPS;
      current_device_index = 0;
      handle_app_opening();
      return TRUE;
    }
    case HSU_COMP_CHANGE_STATE_OPENING_APPS:
    {
      if(new_comp_details->num_of_rdm_dev_in_comp > 0)
      {
        HSU_ASSERT(current_device_index < new_comp_details->num_of_rdm_dev_in_comp);
        
        HSU_MSG_LOW("*_rdm_done(): opening of service that used device = %d completed", 
          new_comp_details->rdm_devices[current_device_index],0,0);

        ++current_device_index;
      }

      if((new_comp_details->num_of_rdm_dev_in_comp > 0) &&
         (current_device_index < new_comp_details->num_of_rdm_dev_in_comp))
      {
        handle_app_opening();
      }
      else
      {
        /* 
         * We're done opening Application's USB port! 
         */
        HSU_MSG_MED("*_rdm_done(): moving to COMP state", 
          0,0,0);
        comp_change_state = HSU_COMP_CHANGE_STATE_COMPLETED;


        hsu_os_bus_lpm_enable(0);
      }
      return TRUE;
    }
    case HSU_COMP_CHANGE_STATE_COMPLETED:
    {
      HSU_MSG_MED("*_rdm_done(): state is COMPLETED, nothing to do", 
        0,0,0);
      return TRUE;
    }
    default:
    {
      HSU_MSG_ERROR("*_rdm_done(): bad state %d", 
        comp_change_state,0,0);
      HSU_ASSERT((char *)"*_rdm_done(): bad state" == NULL);
    }
  }/*switch*/
#else /* FEATURE_HSU_TEST */
#error code not present
#endif /* ! FEATURE_HSU_TEST */

  HSU_MSG_ERROR("*_rdm_done(): Composition switching failed for device_index=%d,"
                " returning FALSE", current_device_index,0,0);

  /* If switching fails, reset the comp switching state machine and data. */
  reset_state_machine_and_data();
  return FALSE; /* should not be reached, but the compiler complains... */
} /* hsu_conf_sel_ui_rdm_done */

/*===========================================================================

FUNCTION              HSU_CONF_SEL_UI_RDM_EVENT_RECEIVED

DESCRIPTION
  Called from CoreMenu.c, in the UI task's context, when an EVT_RDM_* other than 
  EVT_RDM_DONE is received.
  This is to reset the internal state and data, so that we'd be ready for another 
  composition change.

DEPENDENCIES
  Must be called after stack initialization, during a composition change process. 

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hsu_conf_sel_ui_rdm_event_received(uint16 rdm_event)
{
  HSU_MSG_ERROR(
    "*_rdm_event_received(): received rdm_event=%d, reseting internal info", 
    rdm_event,0,0);
  reset_state_machine_and_data();

  hsu_os_bus_lpm_enable(0);

} /* hsu_conf_sel_ui_rdm_event */

/*===========================================================================

FUNCTION              HSU_CONF_SEL_UI_COMP_CHANGE_IS_DONE

DESCRIPTION
  Called from CoreMenu.c, in the UI task's context, when an EVT_RDM_DONE is 
  received, to check if this is the last EVT_RDM_DONE expected.

DEPENDENCIES
  Must be called after stack initialization, during a composition change process. 

RETURN VALUE
  TRUE if the process of composition change is completed when the function 
  returns, FALSE if the process is not yet completed.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hsu_conf_sel_ui_comp_change_is_done(void)
{
#ifndef FEATURE_HSU_TEST
#ifndef FEATURE_UI_CORE_REMOVED
  nv_stat_enum_type nv_status;
  nv_item_type      nv_item_data; 
#endif 

  if(comp_change_state == HSU_COMP_CHANGE_STATE_COMPLETED)
  {
    HSU_MSG_HIGH("*_comp_change_is_done(): reporting completion", 
      0,0,0);
    /* If we are switching to/from dynamic composition,
       we don't need to update NV. */
    if((new_comp_details->nv_comp_id != NV_HS_USB_NON_NV_DEVICE) &&
       (cur_comp_details->nv_comp_id != NV_HS_USB_NON_NV_DEVICE) &&
       (new_comp_details->nv_comp_id != cur_comp_details->nv_comp_id))
    {
      HSU_MSG_MED("*_comp_change_is_done(): writing nv_comp_id=%d to NV", 
        new_comp_details->nv_comp_id,0,0);
#ifndef FEATURE_UI_CORE_REMOVED
      nv_item_data.usb_current_device = new_comp_details->nv_comp_id;
      nv_status = ui_put_nv(NV_HS_USB_CURRENT_COMPOSITION_I, &nv_item_data);
      if(nv_status != NV_DONE_S)
      {
        HSU_MSG_ERROR(
          "*_comp_change_is_done(): Couldn't write %d to NV_HS_USB_CURRENT_COMPOSITION_I (status = %d!)",
          nv_item_data.usb_current_device, nv_status, 0);
      }
#endif 
    }
    reset_state_machine_and_data();
    return TRUE;
  }
  HSU_MSG_MED("*_comp_change_is_done(): not completed yet...", 
    0,0,0);
  return FALSE;
#else /* FEATURE_HSU_TEST */
#error code not present
#endif /* ! FEATURE_HSU_TEST */
}

#endif /* ! T_REXNT */

#else  /* FEATURE_HS_USB_MEM_OPTIMIZATION */
#include "hsu_donothing.h"
#endif /* ! FEATURE_HS_USB_MEM_OPTIMIZATION || 
          FEATURE_PICTBRIDGE_HS || FEATURE_HSU_MTP */

