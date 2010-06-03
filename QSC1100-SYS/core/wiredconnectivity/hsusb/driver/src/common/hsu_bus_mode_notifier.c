/*==============================================================================

    High Speed USB suspend/resume notifier
  
  GENERAL DESCRIPTION
    This module defines suspend resume notifier. It allows registering for
    suspend and resume events from the USB stack
      
  EXTERNALIZED FUNCTIONS
    hsu_register_bus_mode_notif_cb
    hsu_unregister_bus_mode_notif_cb
    hsu_bus_mode_changed

  INITALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE
  
==============================================================================*/
#include "hsu_bus_mode_notifier.h"
#include "comdef.h"
#include "hsu_common.h"
#include "hsu_os_abstract_bus.h"
#include "hsu_core_config.h"

/*==============================================================================

                          DEFINITIONS AND DECLARATIONS FOR MODULE
  
==============================================================================*/

/*==============================================================================
Constants and Macros
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/
/*
 * The maximal number of suspend/resume callbacks that can be 
 * registered 
 */
#define HSU_BUS_MODE_MAX_REGISTERED_CB 5 * HSU_MAX_NUM_OF_CORES

/*==============================================================================
  Variables
==============================================================================*/
/*
 * The current suspend/resume state of the USB stack 
 */
static hsu_bus_mode_enum hsu_current_bus_mode = HSU_BUS_MODE_SUSPENDED;

/*
 * registered callbacks
 */
static hsu_bus_mode_cb_info_struct 
  hsu_bus_mode_registered_cb_array[HSU_BUS_MODE_MAX_REGISTERED_CB] = {{NULL}};

/*==============================================================================

                          FUNCTION DECLARATIONS FOR MODULE
  
==============================================================================*/

/*==============================================================================

FUNCTION          HSU_BUS_MODE_CHANGED

DESCRIPTION
  This function should be called when a change in bus mode happens (either bus
  was suspended or resumed)

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==============================================================================*/
void hsu_bus_mode_changed
(
  uint32 core_index,
  hsu_bus_mode_enum new_mode
)
{
  uint8 i;

  /* Currently, we support only events from the primary core. */ 
  /* TODO: Need to handle it in the future, to support events from multi-cores */
  if (core_index != HSU_CORE_IDX_PRIMARY)
  {
    HSU_MSG_MED("hsu_bus_mode_changed(): unsupported core index %d", 
      core_index,0,0);
    return;
  }

  if(hsu_current_bus_mode == new_mode)
  {
    HSU_MSG_MED("hsu_bus_mode_changed(): state %d not changed (0=S,1=R)", 
      new_mode,0,0);
    return;
  }

  HSU_MSG_HIGH("hsu_bus_mode_changed(): new mode = %d (0=S,1=R)", 
    new_mode,0,0);
  hsu_current_bus_mode = new_mode;

  /* Need to notify registered callbacks of the change */
  for(i = 0 ; i < HSU_BUS_MODE_MAX_REGISTERED_CB ; ++i)
  {
    if(hsu_bus_mode_registered_cb_array[i].bus_mode_cb != NULL)
    {
      hsu_bus_mode_registered_cb_array[i].bus_mode_cb(new_mode, hsu_bus_mode_registered_cb_array[i].app_data);
    }
  }
} /* hsu_bus_mode_changed */

/*===========================================================================

FUNCTION      HSU_REGISTER_BUS_MODE_NOTIF_CB

DESCRIPTION
  Register a callback function to be invoked when the suspend/resume state of 
  the HSU stack is changed.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE for success.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hsu_register_bus_mode_notif_cb
(
  hsu_bus_mode_cb_info_struct cb_info
)
{
  uint8 i;

  for(i = 0 ; i < HSU_BUS_MODE_MAX_REGISTERED_CB ; ++i)
  {
    if ((hsu_bus_mode_registered_cb_array[i].bus_mode_cb == NULL) &&
      (hsu_bus_mode_registered_cb_array[i].app_data == NULL))
    {
      break; /* we found an available entry */
    }
  }

  if (i == HSU_BUS_MODE_MAX_REGISTERED_CB)
  {
    return FALSE;  
  }

  hsu_bus_mode_registered_cb_array[i].bus_mode_cb = cb_info.bus_mode_cb;
  hsu_bus_mode_registered_cb_array[i].app_data = cb_info.app_data;

  return TRUE;
} /* hsu_register_bus_mode_notif_cb */

/*===========================================================================

FUNCTION      HSU_UNREGISTER_BUS_MODE_NOTIF_CB

DESCRIPTION
  Unregister a suspend/resume callback function.

DEPENDENCIES
  hsu_register_bus_mode_notif_cb must have been called with the same 
  callback function.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hsu_unregister_bus_mode_notif_cb
(
  hsu_bus_mode_cb_info_struct cb_info
)
{
  uint8 i;

  for(i = 0 ; i < HSU_BUS_MODE_MAX_REGISTERED_CB ; ++i)
  {
    if ((hsu_bus_mode_registered_cb_array[i].bus_mode_cb == cb_info.bus_mode_cb) &&
        (hsu_bus_mode_registered_cb_array[i].app_data == cb_info.app_data))
    {
      hsu_bus_mode_registered_cb_array[i].bus_mode_cb = NULL;
      hsu_bus_mode_registered_cb_array[i].app_data = NULL;
      break; 
    }
  }
} /* hsu_unregister_bus_mode_notif_cb */
