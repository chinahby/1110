#ifndef HSU_HOST_AL_MS_H
#define HSU_HOST_AL_MS_H

/*==============================================================================

High Speed USB Host Mass Storage Adaptation Layer

GENERAL DESCRIPTION
Defines externalized data and functions for the host mass storage
adaptation layer.

EXTERNALIZED FUNCTIONS
hsu_host_al_ms_init
hsu_host_al_ms_uninit

Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/adapt_layers/hsu_host_al_ms.h#1 $

when     who  what, where, why
-------- ---  ---------------------------------------------------------

==============================================================================*/

#include "comdef.h"

/* Values returned in the USBHOST_IOCTL_STOR_MEDIA_WRITE_PROTECT command.
** Taken from the FS-USB scsi.h file. Should probably be defined in 
** usbhost_api.h, but no one seems to care ...
*/
#define SCS_INFO_MEDIA_WRITE_PROTECTED      0xD0FFF001
#define SCS_INFO_MEDIA_NOT_WRITE_PROTECTED  0xD0FFF002

/*==============================================================================
Typedefs
==============================================================================*/

/*==============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*===========================================================================

FUNCTION hsu_host_al_ms_init

DESCRIPTION
This function initializes the adaptation layer.

DEPENDENCIES
None.

RETURN VALUE
TRUE for success, FALSE for failure.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_host_al_ms_init(void);

/*===========================================================================

FUNCTION hsu_host_al_ms_uninit

DESCRIPTION
This function un-initializes the adaptation layer.

DEPENDENCIES
None.

RETURN VALUE
TRUE for success, FALSE for failure.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_host_al_ms_uninit(void);

#endif /* HSU_HOST_AL_MS_H */
