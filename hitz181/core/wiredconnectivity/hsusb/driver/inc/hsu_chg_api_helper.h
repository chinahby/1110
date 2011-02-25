#ifndef _HSU_CHG_API_HELPER_H_
#define _HSU_CHG_API_HELPER_H_

/*==============================================================================

Charging Module API Helper to Interface with the High Speed USB
H E A D E R   F I L E

GENERAL DESCRIPTION
Contains the API definitions for responding to HS-USB state changes 
(e.g. connect, disconnect, suspend, resume) needed to allow charging via HS-USB.

EXTERNALIZED FUNCTIONS
hsu_chg_api_helper_suspended
hsu_chg_api_helper_resumed
hsu_chg_api_helper_is_resumed

INITALIZATION AND SEQUENCING REQUIREMENTS
None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/
/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/inc/hsu_chg_api_helper.h#3 $
$DateTime: 2009/01/09 14:45:00 $ $Author: jaychoi $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
05/25/07 esh  Initial version

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

#ifdef FEATURE_BATTERY_CHARGER_USE_NIMH

/* Max configured current using NiMH battery */
#define HSU_CHG_API_HELPER_MAX_POWER            (300)

/* Maximum current for USB Wall Charger using NiMH battery */ 
#define HSU_CHG_API_HELPER_WALL_MAX_POWER       (300)

#else

/* Max configured current using Li-ion battery.
 * HSU_DEVICE_MAX_POWER is in 2 mA steps 
 */
#define HSU_CHG_API_HELPER_MAX_POWER            (2 * HSU_DEVICE_MAX_POWER)

/* Maximum current for USB Wall Charger using Li-ion battery */
#define HSU_CHG_API_HELPER_WALL_MAX_POWER       (1500)

#endif /* FEATURE_BATTERY_CHARGER_USE_NIMH */


/* According to USB 2.0 Spec, section 7.2.1, device must draw no more 
 * than one unit load (100 mA) upon powerup prior to being configured
 */
#define HSU_CHG_API_HELPER_POWER_UNCONFIGURED   (100) 

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

FUNCTION hsu_chg_api_helper_suspended

DESCRIPTION
Notifies this module when device has been suspended.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS

===========================================================================*/
void hsu_chg_api_helper_suspended
(
  void
);

/*===========================================================================

FUNCTION hsu_chg_api_helper_resumed

DESCRIPTION
Notifies this module when device has been resumed.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS

===========================================================================*/
void hsu_chg_api_helper_resumed
(
  void
);

/*===========================================================================

FUNCTION hsu_chg_api_helper_is_resumed

DESCRIPTION
Returns whether or not the device is resumed.

DEPENDENCIES
None

RETURN VALUE
TRUE - Device was issued a USB Resume. FALSE - Otherwise.

SIDE EFFECTS

===========================================================================*/
boolean hsu_chg_api_helper_is_resumed
(
  void
);

#endif /* _HSU_CHG_API_HELPER_H_ */
