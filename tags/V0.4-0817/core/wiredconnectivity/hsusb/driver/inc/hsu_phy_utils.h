#ifndef HSU_PHY_UTILS_H
#define HSU_PHY_UTILS_H

/*==============================================================================

H S U _ P H Y _ U T I L S . H


GENERAL DESCRIPTION
High Speed USB general utility functions for PHY (external/internal), 
to be used whether or not the HS-USB stack is running on the processor invoking
these functions.

EXTERNALIZED FUNCTIONS
hsu_os_bus_spoof_disconnect
hsu_phy_utils_spoof_connect
hsu_phy_utils_enable_lpm_exit
hsu_phy_utils_disable_lpm_exit

INITIALIZATION AND SEQUENCING REQUIREMENTS
None.

Copyright (c) 2008, 2009 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/


/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/inc/hsu_phy_utils.h#3 $

when      who     what, where, why
--------  ---     --------------------------------------------------------------
03/27/08  esh     Initial version

==============================================================================*/


/*==============================================================================

INCLUDE FILES

==============================================================================*/
#include <comdef.h>

/*==============================================================================

DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/


/*==============================================================================
Typedefs
==============================================================================*/

/*==============================================================================
Constants and Macros
==============================================================================*/


/*==============================================================================

FUNCTION DECLARATIONS

==============================================================================*/

/*===========================================================================
FUNCTION HSU_PHY_UTILS_SPOOF_DISCONNECT

DESCRIPTION
This function causes the device to disconnect from host, 
while USB cable can still be attached.

DEPENDENCIES
None.

RETURN VALUE
TRUE - Operation successful. FALSE - Otherwise.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_phy_utils_spoof_disconnect(void);


/*===========================================================================
FUNCTION HSU_PHY_UTILS_SPOOF_CONNECT

DESCRIPTION
This function causes the device to connect to the host, 
after calling the hsu_phy_utils_spoof_disconnect() function.

DEPENDENCIES
None.

RETURN VALUE
TRUE - Operation successful. FALSE - Otherwise.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_phy_utils_spoof_connect(void);


/*===========================================================================
FUNCTION HSU_PHY_UTILS_ENABLE_LPM_EXIT

DESCRIPTION
This function lets the HS-USB PHY start exiting from LPM when USB resume or 
reset events occur during TCXO shutdown, even before the HS-USB interrupt
is invoked.

This function comes to resolve issues with violating the USB specification
because sometimes it take a while until the phone wakes up from the shutdown 
state.

This function first checks whether the HS-USB AHB bus clock is enabled. If it
is not enabled, it means that the HS-USB port is not connected, and therefore
USB resume or reset events are not expected. Otherwise, the STP_CTRL bit in the
USBCMD register is cleared, letting any pending PHY LPM exit start executing.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_phy_utils_enable_lpm_exit(void);


/*===========================================================================
FUNCTION HSU_PHY_UTILS_DISABLE_LPM_EXIT

DESCRIPTION
This function disables undesirable PHY LPM exits during TCXO shutdown state.

This function comes to revert the actions taken by the 
hsu_phy_utils_enable_lpm_exit() when there was TCXO shutdown mode exit which 
was not triggered by a USB event.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_phy_utils_disable_lpm_exit(void);

#endif /* HSU_PHY_UTILS_H */
