#ifndef _HSU_CONF_SEL_UI_H_
#define _HSU_CONF_SEL_UI_H_

/*==============================================================================

     High Speed USB configuration Selector UI related
        H E A D E R   F I L E
  
  GENERAL DESCRIPTION
    This file includes functions that are related to High Speed USB composition 
    changes through the UI.

  EXTERNALIZED FUNCTIONS
    hsu_conf_sel_ui_dynamic_switch_is_allowed
    hsu_conf_sel_ui_rdm_done
    hsu_conf_sel_ui_comp_change_is_done

  INITALIZATION AND SEQUENCING REQUIREMENTS
    hsu_conf_sel_ui_comp_menu_init should be called during power up, before the 
    user can perform any UI actions.
    
  Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //source/qcom/qct/core/hsusb/driver/main/latest/inc/hsu_conf_sel_ui.h#2 $
  $DateTime: 2008/08/07 00:24:53 $ $Author: merez $
    
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
  12/04/06 ke  Initial version
    
==============================================================================*/
#include "hsu_conf_sel_types.h"

/*==============================================================================
  Forward Declarations
==============================================================================*/

/*==============================================================================
  Constants
==============================================================================*/
#define HSU_MAX_UI_ITEMS     10 /* This is a limit from the UI code       */
                                /* in any event must be less that         */
                                /* the value of HSU_UNDEFINED_MENU_INDEX         */
#define HSU_UNDEFINED_MENU_INDEX  0xFF /* Flag value to indicate that we don't   */
                                /* know the current default               */


/*==============================================================================
  Macros
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/
typedef struct 
{
  char *                            item_string; 
    /* Must be exactly 12 chars long */
  
  /* The HSU composition id representing this composition */
  hsu_conf_sel_composition_id_enum  hsu_comp_id; 
} hsu_conf_sel_ui_composition_item_type;


/*---------------------------------------------------------------------------
 * Structure definitions for High Speed USB Composition UI MENU 
 *-------------------------------------------------------------------------*/
typedef struct
{
  char *  heading;       
    /* Must be exactly 12 characters long, 
     * For example: "DIAG,MS     "         */
  uint8   current_index; 
    /* index of current value 0..0xFF
     * "0xFF" indicates current_index is unknown. 
     * REAL item indexes range from zero to (HSU_MAX_UI_ITEMS - 1).  */
  uint8   num_items;     
    /* length of the items array below (MAX of HSU_MAX_UI_ITEMS */
  hsu_conf_sel_ui_composition_item_type * items[HSU_MAX_UI_ITEMS]; 
} hsu_conf_sel_ui_composition_menu_type;

/*==============================================================================
  PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*===========================================================================

FUNCTION            HSU_CONF_SEL_UI_COMP_MENU_INIT

DESCRIPTION
  Prepares the High Speed USB Composition Selection Menu for operation.  
  Must be called before any other HS-USB Composition Menu Services are used.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hsu_conf_sel_ui_comp_menu_init(void);

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
boolean hsu_conf_sel_ui_dynamic_switch_is_allowed(void);

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
);
#endif /* FEATURE_HSU_TEST */

/*===========================================================================

FUNCTION              HSU_CONF_SEL_UI_RDM_DONE

DESCRIPTION
  Called from CoreMenu.c, when our dialog receive an EVT_RDM_DONE. 
  It continues the process of application closing / opening according to the 
  current internal state.

DEPENDENCIES
  Must be called after stack initialization, during a composition change process. 

RETURN VALUE
  TRUE for success (only indicates the current step was successful), FALSE for failure.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hsu_conf_sel_ui_rdm_done(void);

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
void hsu_conf_sel_ui_rdm_event_received(uint16 rdm_event);

/*===========================================================================

FUNCTION              HSU_CONF_SEL_UI_COMP_CHANGE_IS_DONE

DESCRIPTION
  Called from CoreMenu.c, when an EVT_RDM_DONE is received, to check if 
  this is the last EVT_RDM_DONE expected.

DEPENDENCIES
  Must be called after stack initialization, during a composition change process. 

RETURN VALUE
  TRUE if the process of composition change is completed when the function returns,
  FALSE if the process is not yet completed.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hsu_conf_sel_ui_comp_change_is_done(void);

#endif /* _HSU_CONF_SEL_NV_H_ */
