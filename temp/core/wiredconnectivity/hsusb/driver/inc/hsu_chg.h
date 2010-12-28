#ifndef _HSU_CHG_H_
#define _HSU_CHG_H_

/*==============================================================================

Charging Module to Interface with the High Speed USB
H E A D E R   F I L E

GENERAL DESCRIPTION
Contains the API definitions for chgpm module needed to allow charging via 
HS-USB.

EXTERNALIZED FUNCTIONS
hsu_chg_init
hsu_chg_configure_usb_power_consumer
hsu_chg_set_usb_power_requirement
hsu_chg_process_usb_power_line_control_request
hsu_chg_invoke_connected_cb
hsu_chg_invoke_disconnected_cb
hsu_chg_invoke_avail_cb
hsu_chg_invoke_not_avail_cb
hsu_chg_is_connected

INITALIZATION AND SEQUENCING REQUIREMENTS
None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/inc/hsu_chg.h#1 $
$DateTime: 2008/02/04 22:58:53 $ $Author: lmanor $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
05/25/07 esh  Initial version

==============================================================================*/
/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "pmapp_otg.h" 
  /* For the enum pm_app_otg_a_dev_type needed by one of the callbacks. 
  As well as the function types:
                          
    pm_app_otg_usb_power_source_connected_notification_cb_type
    pm_app_otg_usb_power_source_disconnected_notification_cb_type
    pm_app_otg_usb_power_sink_open_notification_cb_type
    pm_app_otg_usb_power_sink_close_notification_cb_type
                          
  */

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
/*===========================================================================

FUNCTION hsu_chg_init

DESCRIPTION
Initializes the static data structure of this module.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS

===========================================================================*/
void hsu_chg_init
(
  void
);

/*===========================================================================

FUNCTION hsu_chg_configure_usb_power_consumer

DESCRIPTION
Called by chgpm module, saves the charger's callbacks for later invocation.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS

===========================================================================*/
void hsu_chg_configure_usb_power_consumer
(
  pm_app_otg_usb_power_source_connected_notification_cb_type connected_cb,
    /* Callback for connected */
  pm_app_otg_usb_power_source_disconnected_notification_cb_type disconned_cb,
    /* Callback for disconnected */
  pm_app_otg_usb_power_sink_open_notification_cb_type chg_is_avail_cb,
    /* Callback for sink open */
  pm_app_otg_usb_power_sink_close_notification_cb_type chg_not_avail_cb
    /* Callback for sink close */
);

/*===========================================================================

FUNCTION hsu_chg_set_usb_power_requirement

DESCRIPTION
Called by chgpm module, notifies HS-USB that the charger is connected or not
and therefore whether or not to be persistent when charging via USB cable
(e.g. if suspend is issued, send a remote wakeup).

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS

===========================================================================*/
void hsu_chg_set_usb_power_requirement
(
  boolean is_persistent_charging
);

/*===========================================================================

FUNCTION hsu_chg_process_usb_power_line_control_request

DESCRIPTION
Called by chgpm module, when it is connected to a carkit or external charger via 
USB. Will request the PMIC either for power boost (using the internal battery)
or getting power from the external device (via VBUS).

DEPENDENCIES
None

RETURN VALUE
TRUE - Operation successful. FALSE - Otherwise.

SIDE EFFECTS

===========================================================================*/
boolean hsu_chg_process_usb_power_line_control_request
(
  boolean is_requesting_ctrl
);

/*===========================================================================

FUNCTION hsu_chg_invoke_connected_cb

DESCRIPTION
Invokes the chgpm module's connected callback, when HS-USB notifies it that a 
USB cable is connected to a PC host.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS

===========================================================================*/
void hsu_chg_invoke_connected_cb
(
  boolean is_wall_charger /*the device type is charger or not. */
);

/*===========================================================================

FUNCTION hsu_chg_invoke_disconnected_cb

DESCRIPTION
Invokes the chgpm module's disconnected callback, when HS-USB notifies it that 
this device has been disconnected from a PC host.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS

===========================================================================*/
void hsu_chg_invoke_disconnected_cb
(
  void
);

/*===========================================================================

FUNCTION hsu_chg_invoke_avail_cb

DESCRIPTION
Invokes the chgpm module's avail callback (it may draw power), when HS-USB 
notifies it has received a resume from a PC host.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS

===========================================================================*/
void hsu_chg_invoke_avail_cb
(
  uint32 amount_current_to_draw /* The amount of power to draw, in mA */
);

/*===========================================================================

FUNCTION hsu_chg_invoke_not_avail_cb

DESCRIPTION
Invokes the chgpm module's unavail callback (it must stop drawing power), when 
HS-USB notifies it has received a suspend from a PC host.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS

===========================================================================*/
void hsu_chg_invoke_not_avail_cb
(
  void
);

/*===========================================================================

FUNCTION hsu_chg_is_connected

DESCRIPTION
Returns whether or charger is connected to the HS-USB.

DEPENDENCIES
None

RETURN VALUE
TRUE - Charger is connected. FALSE - Otherwise.

SIDE EFFECTS

===========================================================================*/
boolean hsu_chg_is_connected
(
  void
);

#endif /* _HSU_CHG_H_ */
