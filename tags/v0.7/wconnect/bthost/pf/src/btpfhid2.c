/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 B L U E T O O T H    H U M A N  I N T E R F A C E  D E V I C E  P R O F I L E 

GENERAL DESCRIPTION
  This module contains the Bluetooth HID host data and code.

              Copyright (c) 2007-2008 QUALCOMM Incorporated.               
                      All Rights Reserved.                                  
              Qualcomm Confidential and Proprietary                        
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfhid2.c#1 $ 
  $DateTime: 2009/01/07 18:14:54 $

  when        who  what, where, why
  ----------  ---  ------------------------------------------------------------
  2008-10-17   gs  Fixed issue in HID reconnect.
  2008-07-24   gs  Fixed Klocwork error. 
*
*    #8        12 Mar 2008            RS
* Added deregistration of bt_pf_hid_host_app_id in the cleanup path.
*
*    #7        03 Mar 2008            GS
* Added support for Get/SetReport methods and re-assembly of large
* reports on interrupt/control channel.
*
*    #6        26 Feb 2008            MH
* Removing compiler warnings 
*
*    #5        20 Feb 2008            MH
* Fixed issues with device initiated reconnection 
*
*    #4        07 Feb 2008            MH
* HID host low power configuration selection
*
*    #3        18 Jan 2008            GS
* Enabled page scanning in HID Host register function.  Fixed issue
* in device initiated reconnection scenario.
*
*    #2        03 Dec 2007            GS
* Resolved ADS1.2 compiler error.
*
*    #1        07 Nov 2007            GS
* Initial revision.
*
===========================================================================*/ 

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_BT
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */
#endif /* FEATURE_BT */
