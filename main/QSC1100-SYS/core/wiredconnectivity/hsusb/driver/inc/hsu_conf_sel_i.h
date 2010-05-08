#ifndef _HSU_CONF_SEL_I_H_
#define _HSU_CONF_SEL_I_H_

/*==============================================================================

     High Speed USB configuration Selector Module        
        I N T E R N A L  H E A D E R   F I L E
  
  GENERAL DESCRIPTION
    This file defined the HSU Configuration Selector module's internal functions,
    meaning, functions that should only be used from HSU source files. 

  EXTERNALIZED FUNCTIONS
    hsu_conf_sel_get_cur_comp_details
    hsu_conf_sel_get_cur_comp_id
    hsu_conf_sel_switch_dev_stack

  INITALIZATION AND SEQUENCING REQUIREMENTS
    None.
    
  Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //source/qcom/qct/core/hsusb/driver/main/latest/inc/hsu_conf_sel_i.h#2 $
  $DateTime: 2009/02/11 06:48:45 $ $Author: ablay $
    
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
  09/12/06  ke      Initial version
    
==============================================================================*/
#include "hsu_config_selector.h"
#include "hsu_conf_sel_types.h"
#include "jtypes.h"
#include "jslave_init.h"


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
const hsu_conf_sel_comp_info_type * 
  hsu_conf_sel_get_cur_comp_details(void);

/*==============================================================================

FUNCTION          HSU_CONF_SEL_GET_CUR_COMP_ID

DESCRIPTION
  This function returns the hsu_conf_sel_composition_id_enum of the current 
  composition, for the primary core.

DEPENDENCIES
  None.

RETURN VALUE
  hsu_conf_sel_composition_id_enum.

SIDE EFFECTS
  None.

==============================================================================*/
hsu_conf_sel_composition_id_enum hsu_conf_sel_get_cur_comp_id(void);

/*==============================================================================

FUNCTION          HSU_CONF_SEL_SWITCH_DEV_STACK

DESCRIPTION
  This function first stops the current device stack, and then initializes and 
  starts the device stack according to the new composition given as a parameter.
  It registers reconfig_cb_func_ptr as the callback function to be invoked when 
  the stack re-configuration process is completed. 

DEPENDENCIES
  Assumes the device stack is up. 

RETURN VALUE
  TRUE for success, currently all failures cause an ASSERT.

SIDE EFFECTS
  cur_comp_details is updated with the new composition.

==============================================================================*/
boolean hsu_conf_sel_switch_dev_stack
(
  hsu_conf_sel_composition_id_enum  new_hsu_comp_id,
  reconfig_complete_cb_t            reconfig_cb_func_ptr,
  void *      arg_for_reconfig_cb
);

#endif /* _HSU_CONF_SEL_I_H_ */
