#ifndef _HSU_DEV_APP_H_
#define _HSU_DEV_APP_H_

/*==============================================================================

High Speed USB Device Stack Application
H E A D E R   F I L E

GENERAL DESCRIPTION
Contains API definitions for interfacing USB stack with a Device Stack Application.

EXTERNALIZED FUNCTIONS

INITALIZATION AND SEQUENCING REQUIREMENTS
None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/inc/hsu_dev_app.h#1 $
$DateTime: 2008/02/05 10:39:40 $ $Author: lmanor $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
09/05/07 dl   Initial version

==============================================================================*/
/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/

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

typedef void* appctx_t;

typedef struct {

  void (*notify_device_configured)(appctx_t appctx, jbool_t configured);

} app_callbacks_t;

/*==============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

#endif /* _HSU_DEV_APP_H_ */
