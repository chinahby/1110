/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

B L U E T O O T H   A V   C O N T R O L  T R A N S P O R T   P R O T O C O L

GENERAL DESCRIPTION
  This module contains the Bluetooth AVCTP interface (commands and events)
  implementation.

Copyright (c) 2005 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

*
*   #1       07 Mar 2005          JTL
* Initial revision.
*
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_BT
#ifdef FEATURE_BT_EXTPF_AV

#include "bt.h"
#include "btpfi.h"
#include "btpf.h"
#include "btpfcmdi.h"
#include "bti.h"
#include "btmsg.h"

#include "oi_avrcp.h"
#include "oi_avrcp_consts.h"

/* Open Interface Source files */
#include "avctp.c"

#endif /* FEATURE_BT_EXTPF_AV */
#endif /* FEATURE_BT */
