#ifndef __HSU_CHG_WALLCHG_
#define __HSU_CHG_WALLCHG_

/*==============================================================================

USB Wall Charger Detection API - H E A D E R   F I L E

GENERAL DESCRIPTION
Contains the API definitions for detecting the presence of a USB wall charger

EXTERNALIZED FUNCTIONS
hsu_chg_wallchg_init
hsu_chg_wallchg_detect_wall_charger
hsu_chg_wallchg_cancel_detection

INITALIZATION AND SEQUENCING REQUIREMENTS
None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/
/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/inc/hsu_chg_wallchg.h#1 $
$DateTime: 2008/02/04 22:58:53 $ $Author: lmanor $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
12/03/07 esh  Initial version

==============================================================================*/
/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "comdef.h"
/*==============================================================================
Typedefs
==============================================================================*/
typedef void (*hsu_chg_wallchg_detected_notif_fn)(boolean);

/*==============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/
/*===========================================================================

FUNCTION hsu_chg_wallchg_init

DESCRIPTION
Calls any initializations needed to enable USB wall charger detection. Needs
to be called prior to using any of the other APIs.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS

===========================================================================*/
void hsu_chg_wallchg_init
(
  void
);

/*===========================================================================

FUNCTION hsu_chg_wallchg_detect_wall_charger

DESCRIPTION
Requests this module to detect the presence of a USB wall charger. Should be
called upon connection to an A-Device.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS

===========================================================================*/
void hsu_chg_wallchg_detect_wall_charger
(
  hsu_chg_wallchg_detected_notif_fn detection_cb
    /* Callback to be invoked once this module has an answer whether or not
    a USB wall charger was detected. */
);

/*===========================================================================

FUNCTION hsu_chg_wallchg_cancel_detection

DESCRIPTION
Requests this module to abort current request to detect presence of USB wall
charger. This usually will be done upon disconnection from an A-Device.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS

===========================================================================*/
void hsu_chg_wallchg_cancel_detection
(
  void
);

#endif /* __HSU_CHG_WALLCHG_ */
