#ifndef _HSU_BOOT_H_
#define _HSU_BOOT_H_

/*==============================================================================

                    HS-USB API to BOOT Module Header file

GENERAL DESCRIPTION
Contains the API definitions for BOOT to allow interfacing with HS-USB.

EXTERNALIZED FUNCTIONS
hsu_boot_start_stack
hsu_boot_stop_stack
hsu_boot_poll_stack
hsu_boot_disconnect_from_host

INITALIZATION AND SEQUENCING REQUIREMENTS
None

Copyright (c) 2007-2008 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/inc/hsu_boot.h#3 $
$DateTime: 2008/07/23 04:31:09 $ $Author: ablay $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
12/27/07 esh  Initial version

==============================================================================*/
/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "comdef.h" /* For boolean */

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
  HSU_BOOT_POLL_RESULT_ERROR = -1,
  HSU_BOOT_POLL_RESULT_NO_NEW_EVENT,           /* No change in USB state resulted by polling */
  HSU_BOOT_POLL_RESULT_DISCONNECTED,           /* USB cable disconnected */
  HSU_BOOT_POLL_RESULT_CONNECTED_TO_WALL_CHG,  /* USB cable connected to wall charger */
  HSU_BOOT_POLL_RESULT_CONNECTED_TO_HOST_PC,   /* USB cable connected to a PC */
  HSU_BOOT_POLL_RESULT_NOT_CONNECTED,          /* USB cable is not connected */
  HSU_BOOT_POLL_RESULT_RESUMED_NOT_ENUMERATED, /* Device is resumed, but not enumerated yet. 
                                                  When resuming *after* enumeration, the 
                                                  ENUMERATED event will be raised */
  HSU_BOOT_POLL_RESULT_ENUMERATED,             /* Device is enumerated successfully */
  HSU_BOOT_POLL_RESULT_SUSPENDED,              /* Device is suspended */
  HSU_BOOT_POLL_RESULT_MAX
} hsu_boot_poll_result;

/*==============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/
/*===========================================================================
FUNCTION hsu_boot_start_stack

DESCRIPTION
    Initializes and starts the HS-USB stack under BOOT. Enumerate as a HID 
    device, allowing the BOOT to consume 500mA from the host PC.
    The function will return after initializing the stack. Any events such as 
    connection of a PC or a wall charger, or the completion of the enumeration 
    as a HID device, will be reported using the hsu_boot_poll_stack function.

DEPENDENCIES
    None

RETURN VALUE
    TRUE - Operation successful. FALSE - Otherwise

SIDE EFFECTS
===========================================================================*/
boolean hsu_boot_start_stack(void);

/*===========================================================================
FUNCTION hsu_boot_stop_stack

DESCRIPTION
    Stops and tears down the HS-USB stack under BOOT. 

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
===========================================================================*/
void hsu_boot_stop_stack(void);

/*===========================================================================
FUNCTION hsu_boot_poll_stack

DESCRIPTION
    Polls the HS-USB stack and returns the type of event which occurred, as a 
    hsu_boot_poll_result type.

DEPENDENCIES
    hsu_boot_start_stack must be called beforehand

RETURN VALUE
    hsu_boot_poll_result corresponding to the type of USB event that occurred.
    If no new event occurred, HSU_BOOT_POLL_RESULT_NO_NEW_EVENT is returned.

SIDE EFFECTS
===========================================================================*/
hsu_boot_poll_result hsu_boot_poll_stack(void);

/*===========================================================================
FUNCTION hsu_boot_disconnect_from_host

DESCRIPTION
    Requests the HS-USB stack to detach from the host while keeping the USB 
    stack active, allowing the BOOT to consume 100mA from the host PC.
    This function is intended to be called by the BOOT, when the host PC 
    suspends the device.

DEPENDENCIES
    hsu_boot_start_stack must be called beforehand

RETURN VALUE
    TRUE - Operation successful. FALSE - Otherwise.

SIDE EFFECTS
===========================================================================*/
boolean hsu_boot_disconnect_from_host(void);

#endif /* HSU_BOOT_H */
