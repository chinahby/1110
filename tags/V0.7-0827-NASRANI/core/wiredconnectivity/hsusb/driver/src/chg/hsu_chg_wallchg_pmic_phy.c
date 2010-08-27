/*==============================================================================

USB Wall Charger Detection API - I M P L E M E N T A T I O N   F I L E

GENERAL DESCRIPTION
Contains the API implementations for detecting the presence of a USB wall charger,
in case the USB PHY is Chipidea internal PHY. 

EXTERNALIZED FUNCTIONS
hsu_chg_wallchg_detect_wall_charger
hsu_chg_wallchg_init
hsu_chg_wallchg_cancel_detection

INITALIZATION AND SEQUENCING REQUIREMENTS
None

Copyright (c) 2008 by Qualcomm, Incorporated.  All Rights Reserved.
==============================================================================*/
/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/chg/hsu_chg_wallchg_pmic_phy.c#2 $
$DateTime: 2008/08/06 06:55:17 $

when     who  what, where, why
-------- ---  ---------------------------------------------------------

==============================================================================*/
/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "customer.h"
#include "hsu_common.h"
#include "hsu_chg_wallchg.h"


#ifndef BUILD_HOSTDL /* ARMPRG does not contain charging */
/*==============================================================================
Forward Declarations
==============================================================================*/

/*==============================================================================
Constants and Macros
==============================================================================*/

/*==============================================================================

EXTERNAL FUNCTION DEFINITIONS FOR MODULE

==============================================================================*/

/*===========================================================================
FUNCTION hsu_chg_wallchg_detect_wall_charger()
===========================================================================*/
void hsu_chg_wallchg_detect_wall_charger(
  hsu_chg_wallchg_detected_notif_fn detection_cb)
{
  HSU_USE_PARAM(detection_cb);
}

/*===========================================================================
FUNCTION hsu_chg_wallchg_cancel_detection()
===========================================================================*/
void hsu_chg_wallchg_cancel_detection(void)
{
    return;
}

/*===========================================================================
FUNCTION hsu_chg_wallchg_init()
===========================================================================*/
void hsu_chg_wallchg_init(void)
{
    return;
}

#endif /* ! BUILD_HOSTDL */
