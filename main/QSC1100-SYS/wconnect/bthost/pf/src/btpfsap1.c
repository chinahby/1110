/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      B L U E T O O T H      S I M      A C C E S S      P R O F I L E

GENERAL DESCRIPTION
  This module contains the Bluetooth SIM Access Profile data and code
  which are not client or server specific.

Copyright (c) 2005 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfsap1.c#1 $ 
$DateTime: 2009/01/07 18:14:54 $
*
*    #3        05 May 2005            PN
* Replaced BT_MSG_DEBUG with BT_MSG_API and added more info to the messages.
*
*    #2        18 Feb 2005            PN
* Updated based on test result from UPF20.
*
*    #1        22 Jan 2005            PN
* Initial revision
*

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h" 
#ifdef FEATURE_BT
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif /* FEATURE_BT_EXTPF_SAP */
#endif /* FEATURE_BT */

