#ifndef _HSU_CONF_SEL_STACK_UTILS_H_
#define _HSU_CONF_SEL_STACK_UTILS_H_

/*==============================================================================

     High Speed USB Configuration Selector Stack Utilities
        H E A D E R   F I L E
  
  GENERAL DESCRIPTION
    This module defines functions that setup, stop, re-configure, etc. the USB 
    stack.

  EXTERNALIZED FUNCTIONS
    hsu_conf_sel_stack_utils_init_memory
    hsu_conf_sel_stack_utils_setup_device_stack
    hsu_conf_sel_stack_utils_switch_device_stack
    hsu_conf_sel_stack_utils_handle_chg_connected
    hsu_conf_sel_stack_utils_handle_chg_disconnected
    hsu_conf_sel_stack_utils_enable_perf_boost
    hsu_conf_sel_stack_utils_device_restart_core

    hsu_conf_sel_stack_utils_is_forced_disable
    hsu_conf_sel_stack_utils_force_disable_start
    hsu_conf_sel_stack_utils_force_disable_stop
    hsu_conf_sel_stack_utils_disable
    hsu_conf_sel_stack_utils_enable

  INITALIZATION AND SEQUENCING REQUIREMENTS
    hsu_conf_sel_stack_utils_init_memory should be called before any other 
    function, and before the CDC ALs are initialized. 
    
  Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //source/qcom/qct/core/hsusb/driver/main/latest/inc/hsu_conf_sel_stack_utils.h#18 $
  $DateTime: 2009/03/26 08:55:29 $ $Author: omanos $
    
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
  12/06/06 ke  Initial version
    
==============================================================================*/
#include "customer.h" /* for FEATURE_HSU_TEST to work */
#include "comdef.h"

#ifndef FEATURE_HSU_TEST
  #include "jslave_init.h"
#endif /* ! FEATURE_HSU_TEST */

#include "jtypes.h"

#include "hsu_conf_sel_types.h"

/*==============================================================================
  Forward Declarations
==============================================================================*/

/*==============================================================================
  Constants
==============================================================================*/
/*
* Index for the high-speed device controller
*/
#define HSU_DEVICE_CONTROLLER_IDX_0 0

/* Taken from usbcdc.h line 148 TODO - verify */
#define HSU_DEVICE_MAX_POWER               0xFA 
/* In 2mAmp units 250 = 500ma */

/*==============================================================================
  Macros
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

#ifdef T_UNDP /* dynamic VID/PID config for U2 (UNDP) card product support */
boolean hsu_get_undp_hsu_comp_id(nv_hs_usb_supported_compositions_type * nv_composition_id);
#endif /* T_UNDP */

/* The callback to be called on completing the _force_disable_start\stop.
It will return the result of the configuration process */
typedef void (*hsu_conf_sel_stack_utils_force_disable_cb_type)(boolean result);

/*==============================================================================
  PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*==============================================================================

FUNCTION          HSU_CONF_SEL_STACK_UTILS_INIT_MEMORY

DESCRIPTION
  This function initializes variables related to the memory given to the stack. 

DEPENDENCIES
  Called once, when the device stack is first setup, before ALs are initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==============================================================================*/
void hsu_conf_sel_stack_utils_init_memory(void);

/*==============================================================================

FUNCTION          HSU_CONF_SEL_STACK_UTILS_ALLOC_STACK_MEMORY

DESCRIPTION
  This function is called by adaptation layers. If an AL needs to share data 
  with the stack (for example, buffers for TX/RX), then the AL must request this 
  memory from the hsu_config_selector. 
  It does so by calling this function with the number of bytes required, and it
  should use the memory which begins in the returned address. 
  Note: the addresses returned by this function are 32 bytes aligned, since the 
  DMOV requires buffers passed to it to be 32 bytes aligned. 

DEPENDENCIES
  This function MUST be called prior to the stack bring up (jstart_stack). 
  The memory can be used only until the AL's uninit function is called by the 
  stack. 

RETURN VALUE
  The start address of the allocated memory, or NULL if there was not enough 
  memory.

SIDE EFFECTS
  None. 

==============================================================================*/
byte * hsu_conf_sel_stack_utils_alloc_stack_memory
(
  uint32 required_space_in_bytes
);

/*==============================================================================

FUNCTION          HSU_CONF_SEL_STACK_UTILS_SETUP_STACK

DESCRIPTION
  This function is called both by the config_selector (when FEATURE_HSU_TEST is 
  NOT defined) and by the ARMPROG/DLOAD Adaptation Layers.
  It sets-up the USB device stack (by calling jstart_stack and jslave_enable) 
  based on the cores configuration. 

DEPENDENCIES
  Assumes all AL related initialization were already done. 
  Should be called only once after power up.

RETURN VALUE
  jresult_t - HSU_JSUCCESS for success, other values for failure (currently all 
  cause an ASSERT).

SIDE EFFECTS
  None.

==============================================================================*/
jresult_t hsu_conf_sel_stack_utils_setup_stack(
#ifdef T_UNDP
  boolean 	use_amss_product_id
#else
  void
#endif
);

#ifndef FEATURE_HSU_TEST

#ifndef FEATURE_HS_USB_BASIC /* DLOAD/ARMPROG */
/*===========================================================================

FUNCTION hsu_conf_sel_stack_utils_force_disable_start

DESCRIPTION
This function disables the usb core, puts the USB PHY in LPM.

INPUT
cb - the callback for completion (of type 'hsu_conf_sel_stack_utils_force_disable_cb_type')

DEPENDENCIES
Run in the AL TASK command queue.

RETURN VALUE
TRUE/FALSE returned by the callback function ('cb' - input).

SIDE EFFECTS
===========================================================================*/
void hsu_conf_sel_stack_utils_force_disable_start(void* cb);

/*==============================================================================

FUNCTION          hsu_conf_sel_stack_utils_force_disable_stop

DESCRIPTION
This function starts the USB core, activates the core and reconnects the PHY.
    
INPUT
cb - the callback for completion (of type 'hsu_conf_sel_stack_utils_force_disable_cb_type')

DEPENDENCIES
Run in the AL TASK command queue.

RETURN VALUE
TRUE/FALSE returned by the callback function ('cb' - input).

SIDE EFFECTS
  None.

==============================================================================*/
void hsu_conf_sel_stack_utils_force_disable_stop(void* cb);

/*==============================================================================

FUNCTION          hsu_conf_sel_stack_utils_is_forced_disable

DESCRIPTION
returns whether the USB core is in forced_disable mode.

DEPENDENCIES
None.

RETURN VALUE
FALSE - disabled.
TRUE - enabled.

SIDE EFFECTS
None.

==============================================================================*/
boolean hsu_conf_sel_stack_utils_is_forced_disable(void);


/*==============================================================================

FUNCTION          hsu_conf_sel_stack_utils_enable

DESCRIPTION
   This function enable the USB core, called from "hsu_conf_sel_stack_force_disable_stop" 
   and "hsu_analog_audio_stop".
    
DEPENDENCIES
  None.

RETURN VALUE
  FALSE - if failed.
  TRUE - on successes

SIDE EFFECTS
  None.

==============================================================================*/
boolean hsu_conf_sel_stack_utils_enable(void);

/*==============================================================================

FUNCTION          hsu_conf_sel_stack_utils_disable

DESCRIPTION
   This function disable the USB core, called from "hsu_conf_sel_stack_force_disable_start" 
   and "hsu_analog_audio_start".
    
DEPENDENCIES
  None.

RETURN VALUE
  FALSE - if failed.
  TRUE - on successes

SIDE EFFECTS
  None.

==============================================================================*/
boolean hsu_conf_sel_stack_utils_disable(void);

#ifndef T_UNDP /* UNDP does not support UI/Pictbridge */
/*==============================================================================

FUNCTION          HSU_CONF_SEL_STACK_UTILS_SWITCH_DEVICE_STACK

DESCRIPTION
  This function stops the stack, and then re-configures it to include a new set
  of FDs, as described by the num_of_fd_instances_in_comp parameter.
  It registers reconfig_cb_func_ptr as the callback function to be invoked when 
  the stack re-configuration process is completed. 

  It should be called after all application using the old stack were stopped. 

DEPENDENCIES
  Assumes that no application is using the stack at the time of the call. 
  Assumes all AL related initialization were already done. 

RETURN VALUE
  jresult_t - HSU_JSUCCESS for success, other values for failure (currently all 
  cause an ASSERT).

SIDE EFFECTS
  None.

==============================================================================*/
jresult_t hsu_conf_sel_stack_utils_switch_device_stack
(
 uint8                    core_idx,
  reconfig_complete_cb_t  reconfig_cb_func_ptr,
  void *      arg_for_reconfig_cb
);

/*==============================================================================

FUNCTION          HSU_CONF_SEL_STACK_UTILS_RECONFIG_DONE

DESCRIPTION
This function is called when the stack re-configuration process is completed. 
This function updates the ARM9 product_id and serial_number to be used in SW 
download.

DEPENDENCIES

RETURN VALUE
None.

SIDE EFFECTS
None.

==============================================================================*/
void hsu_conf_sel_stack_utils_reconfig_done
(
 hsu_conf_sel_comp_info_type* comp_info
);

#endif /* !T_UNDP */
#endif /* ! FEATURE_HS_USB_BASIC */


/*==============================================================================

FUNCTION          HSU_CONF_SEL_STACK_UTILS_SET_SERIAL_NUMBER

DESCRIPTION
This function sets the serial number according to the composition and to the
configuration (AMSS / DLOAD / ARMPROG)

DEPENDENCIES

RETURN VALUE
None

SIDE EFFECTS
None. 

==============================================================================*/
void hsu_conf_sel_stack_utils_set_serial_number(
  dev_info_t *  device_info,
  uint16        product_id,
  uint8         core_idx
);

/*==============================================================================

FUNCTION          hsu_is_dev_configured

DESCRIPTION
  This function returns TRUE if the device is configured and FALSE otherwise

DEPENDENCIES
  None.

RETURN VALUE
  jbool_t - dev_is_configured

SIDE EFFECTS
  None.
==============================================================================*/
jbool_t hsu_is_dev_configured(void);

#ifdef FEATURE_HS_USB_CHARGING
#ifndef FEATURE_HS_USB_BASIC
/*==============================================================================

FUNCTION          HSU_CONF_SEL_STACK_UTILS_HANDLE_CHG_CONNECTED

DESCRIPTION
  Handle charger connected to HS-USB port. 
  This function gets called by the OTG stack connect/disconnect mechanism 
  implemented in the corresponding .c file, and by non-OTG connect/disconnect 
  mechanism invoked in hsu_os_abstract_bus.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==============================================================================*/
void hsu_conf_sel_stack_utils_handle_chg_connected(void);

/*==============================================================================

FUNCTION          HSU_CONF_SEL_STACK_UTILS_HANDLE_CHG_DISCONNECTED

DESCRIPTION
  Handle charger disconnected from HS-USB port. 
  This function gets called by the OTG stack connect/disconnect mechanism 
  implemented in the corresponding .c file, and by non-OTG connect/disconnect 
  mechanism invoked in hsu_os_abstract_bus.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==============================================================================*/
void hsu_conf_sel_stack_utils_handle_chg_disconnected(void);

#endif /* FEATURE_HS_USB_BASIC */
#endif /* FEATURE_HS_USB_CHARGING */

#endif /* ! FEATURE_HSU_TEST */
/*==============================================================================

FUNCTION          HSU_CONF_SEL_STACK_UTILS_ENABLE_PERF_BOOST

DESCRIPTION
  This function configures various HW registers for throughput
  optimizations

DEPENDENCIES
  The enabling of the various HW registers are dependent on HW version.

RETURN VALUE
  None

SIDE EFFECTS
  None.

==============================================================================*/
void hsu_conf_sel_stack_utils_enable_perf_boost(void);

#ifndef FEATURE_HS_USB_BASIC /* DLOAD/ARMPROG */
#ifdef FEATURE_HS_USB_CORE_RESET_ON_DISC 
/*==============================================================================

FUNCTION          HSU_CONF_SEL_STACK_UTILS_DEVICE_RESTART_CORE

DESCRIPTION       
 This function is the SW workaround for a HW bug regarding the
 stuck PHY issue. 
 On each disconnection this function is called and restarts the 
 core and the PHY.

DEPENDENCIES
 None.

RETURN VALUE
 None.

SIDE EFFECTS
 None.

==============================================================================*/
void hsu_conf_sel_stack_utils_device_restart_core
(
 void* core_index_ptr
);
#endif /* FEATURE_HS_USB_CORE_RESET_ON_DISC */
#endif /* !FEATURE_HS_USB_BASIC */

#endif /* _HSU_CONF_SEL_STACK_UTILS_H_ */
