#ifndef _HSU_HID_FD_H_
#define _HSU_HID_FD_H_
/*==============================================================================

                High Speed USB HID Function Driver Header File

GENERAL DESCRIPTION
Defines externalized data and functions for a minimal HID Function Driver, for 
fast dead battery charging.

EXTERNALIZED FUNCTIONS
    - hidfd_init

Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/hsusb/driver/main/latest/inc/hsu_hid_fd.h#1 $
  $DateTime: 2008/02/04 22:58:53 $ $Author: lmanor $    

  when      who     what, where, why
  --------  ---     ----------------------------------------------------------
  01/14/08   ab      Initial version
==============================================================================*/

#include <jusb_common.h>

/**************************************************************************
* Function name:  hidfd_init
* Description:    Called by the core to initialize the HID function driver
* Parameters: 
*     @ctx: (IN) Core context
*
* Return value: 0 on success, otherwise an error code 
* Scope:        Global
**************************************************************************/
jresult_t hsu_hid_fd_init(void *core_ctx);

#endif
