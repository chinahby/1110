/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               B L U E T O O T H   G E N E R I C   O B J E C T   
          
                      E X C H A N G E   P R O F I L E

GENERAL DESCRIPTION
  This module contains the code for implementing the MD5 message-digest 
  algorithm used in the Bluetooth Generic Object Exchange Profile .

Copyright (c) 2004 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfgoep5.c#1 $ 

$Log: $
*
*    #5       19 Jan 2005             JH
* Removed path "ext\" from source file includes due to compile issues on Linux.
*

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_BT
#ifdef FEATURE_BT_EXTPF_GOEP

#define BT_MSG_LAYER  BT_MSG_GN 
#include "bt.h"
#include "btpfi.h"
#include "md5_hash.c"

#endif /* FEATURE_BT_EXTPF_GOEP */
#endif /* FEATURE_BT */
