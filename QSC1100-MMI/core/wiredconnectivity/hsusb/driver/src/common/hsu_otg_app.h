#ifndef _HSU_OTG_APP_H_
#define _HSU_OTG_APP_H_

/*==============================================================================

     High Speed USB OTG Application 
        H E A D E R   F I L E
  
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
  
  $Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/common/hsu_otg_app.h#1 $
  $DateTime: 2008/05/29 08:33:09 $ $Author: tlinder $
    
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
  05/27/08  tl      Initial version
    
==============================================================================*/
#include "customer.h" /* for FEATURE_HSU_TEST to work */

#include "jtypes.h"
#include "jslave_init.h"
#include "uw_otg_app.h"


/*==============================================================================
  Forward Declarations
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
  PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

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
);

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
otg_device_mode_t hsu_otg_app_get_otg_mode(void);

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
boolean hsu_otg_app_is_mini_a_connected(void);

/*==============================================================================

FUNCTION          HSU_OTG_APP_SET_UP_WHITELIST

DESCRIPTION
  Called from hsu_conf_sel_stack_utils_setup_stack, save the current initiated 
  core  index

DEPENDENCIES
  None. 

RETURN VALUE
  None.

SIDE EFFECTS
  None. 

==============================================================================*/
#ifndef FEATURE_HS_USB_BASIC /* DLOAD/ARMPROG */
jresult_t hsu_otg_app_set_up_whitelist(void);
#endif /*FEATURE_HS_USB_BASIC*/

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
void hsu_otg_app_set_core_index(uint32 core_idx);
#endif /* _HSU_OTG_APP_H_ */

