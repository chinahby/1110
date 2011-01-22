/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 B L U E T O O T H     S I M     A C C E S S    P R O F I L E    C L I E N T

GENERAL DESCRIPTION
  This module contains the Bluetooth SIM Access Profile client data and code.

        Copyright (c) 2006, 2008 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfsap2.c#1 $ 
$DateTime: 2009/01/07 18:14:54 $
* 
*    #6        13 Feb 2008            PN
* Removed compiler warnings.
* 
*    #5        19 Oct 2006            PN
* Added test code for Set Transport Protocol.
*
*    #4        21 Jul 2006            BK
*    OI code merge with SAP 1.0
* 
*    #3        20 Dec 2005            PN
* No longer sets Bondable when connecting, nor clears Bondable when connected.
* Cleans up properly if connection fails, and after disconnected.
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
