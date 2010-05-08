/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 B L U E T O O T H     S I M     A C C E S S    P R O F I L E    S E R V E R

GENERAL DESCRIPTION
  This module contains the Bluetooth SIM Access Profile server data and code.

Copyright (c) 2005-2008 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfsap3.c#1 $ 
$DateTime: 2009/01/07 18:14:54 $
*
*    #12       14 Jan 2008            PN
* Added Mode4 security setting for Lisbon build.
*
*    #11       05 May 2007           DSN
* Avoid using the saved mmsgdi buffer pointer for apdu req since BT is on
* ARM11 and mmgsdi is in ARM9 on dual-processor targets and buffer pointers
* are not retained when passed via RPC.
*
*    #10       05 May 2007           DSN
* Fix for RVCT21 compile errors when SAP is enabled.
*
*    #9        13 Nov 2006            PN
* When deregistering SAP server, service security is now disabled after 
* deregistration, not before.
*
*    #8        19 Oct 2006            PN
* Added handler for Set Transport Protocol Req. Added better handling of 
* unknown requests.
*
*    #7        26 Jul 2006            BK
* Upgraded to SAP 1.0.
*
*    #6        11 Apr 2006            PN
* Added SAP security enforcement support for SAP server.
*
*    #5        14 Mar 2006            PN
* Max Frame Size must be at least 276 bytes.
*
*    #4        24 Jan 2006            PN
* Changed to use correct field in OI_SAP_MSG in bt_pf_sap_srv_rx_msg_cb().
*
*    #3        02 Nov 2005            PN
* Setting Bondable is no longer part of Registration process.
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
