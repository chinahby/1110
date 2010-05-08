#ifndef _HSU_BUS_MODE_NOTIFIER_H_
#define _HSU_BUS_MODE_NOTIFIER_H_

/*==============================================================================

    High Speed USB suspend/resume notifier
        H E A D E R   F I L E
  
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
#include "comdef.h"

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

typedef enum 
{
  HSU_BUS_MODE_SUSPENDED,
  HSU_BUS_MODE_RESUMED,
  HSU_BUS_MODE_UNCONFIGURED,
  HSU_BUS_MODE_CONFIGURED,
  HSU_BUS_MODE_DISCONNECTED,
  HSU_BUS_MODE_CONNECTED
} hsu_bus_mode_enum;

typedef void (*hsu_bus_mode_notifier_cb)(hsu_bus_mode_enum new_mode, void* app_data);

typedef struct
{
  hsu_bus_mode_notifier_cb   bus_mode_cb;
  /* the callback to be invoked when suspend / resume occurs */

  void*                           app_data;
  /* App specific data */

} hsu_bus_mode_cb_info_struct;

/*==============================================================================
  PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*===========================================================================

FUNCTION      HSU_REGISTER_BUS_MODE_NOTIF_CB

DESCRIPTION
  Register a callback function to be invoked when the suspend/resume state of 
  the HSU stack is changed.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE for success. FALSE if registration failed.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hsu_register_bus_mode_notif_cb
(
  hsu_bus_mode_cb_info_struct cb_info
);

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
);

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
  All registered callbacks are invoked

==============================================================================*/
void hsu_bus_mode_changed
(
  uint32 core_index,
  hsu_bus_mode_enum new_mode
);

#endif /* _HSU_BUS_MODE_NOTIFIER_H_ */
