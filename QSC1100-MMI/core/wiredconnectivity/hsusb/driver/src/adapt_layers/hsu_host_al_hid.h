#ifndef HSU_HOST_AL_HID_H
#define HSU_HOST_AL_HID_H

/*==============================================================================

High Speed USB Host HID Adaptation Layer

GENERAL DESCRIPTION
Defines externalized data and functions for the host hid
adaptation layer.

EXTERNALIZED FUNCTIONS
hsu_host_al_hid_init
hsu_host_al_hid_uninit

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/adapt_layers/hsu_host_al_hid.h#3 $

when     who  what, where, why
-------- ---  ---------------------------------------------------------

==============================================================================*/

#include "comdef.h"
#include "hsu_host_api_i.h"

/*==============================================================================
Typedefs
==============================================================================*/

/*==============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*===========================================================================

FUNCTION hsu_host_al_hid_init

DESCRIPTION
This function initializes the adaptation layer.

DEPENDENCIES
None.

RETURN VALUE
TRUE for success, FALSE for failure.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_host_al_hid_init(void);

/*===========================================================================

FUNCTION hsu_host_al_hid_uninit

DESCRIPTION
This function un-initializes the adaptation layer.

DEPENDENCIES
None.

RETURN VALUE
TRUE for success, FALSE for failure.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_host_al_hid_uninit(void);

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */
#endif /* HSU_HOST_AL_HID_H */

