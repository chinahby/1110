#ifndef _BTQSOCNVM_PF_DEF_H
#define _BTQSOCNVM_PF_DEF_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       B L U E T O O T H    B T S    D R I V E R    H E A D E R    F I L E

GENERAL DESCRIPTION
  This module contains the platform specific definitions for BTS DRIVER 
  
EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2008 QUALCOMM Incorporated. 
All Rights Reserved. 
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

 $Header: 
 $DateTime: 
*
*   #1         31 Jan  2008          SK
*   Created new file for BTS DRIVER PLATFORM DEFINITON.

===========================================================================*/

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/



#ifndef FEATURE_BT_OPENBLUE

#include "customer.h"
#include "comdef.h"

#define OI_UINT8 uint8
#define OI_UINT16 uint16
#define OI_UINT32 uint32

#else /* FEATURE_BT_OPENBLUE */
#error code not present
#endif /* FEATURE_BT_OPENBLUE */



#endif /* _BTQSOCNVM_PF_DEF_H */

