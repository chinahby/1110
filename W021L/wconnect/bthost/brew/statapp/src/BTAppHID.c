/*****************************************************************************/
/*                                                                           */
/*                          FILE: BTAppHID.c                                 */
/*                                                                           */   
/*                   SERVICES: QCT UI BT BREW Application.                   */
/*                                                                           */
/*        GENERAL DESCRIPTION: This module contains HID test feature.        */  
/*                                                                           */  
/*              (c) COPYRIGHT 2007-2009 Qualcomm Incorporated.                    */
/*                      All Rights Reserved.                                 */ 
/*              Qualcomm Confidential and Proprietary                        */
/*****************************************************************************/
/*===========================================================================
                        EDIT HISTORY FOR MODULE
                        
$Header: //source/qcom/qct/wconnect/bthost/brew/statapp/rel/00.00.26/src/BTAppHID.c#3 $
$DateTime: 2009/07/10 11:47:34 $
$Author: nksingh $

when       who  what, where, why
--------   ---  ----------------------------------------------------------
06/11/09    ns  changes to fix compiler warnings when FEATURE_BT_2_1 is disabled
04/10/08    VM  Added BTApp_EnableHID()
03/03/08    GS  Added support for Get/SetReport HID functions.
02/20/08    MH  Fixed handling incoming connection request
01/17/08    GS  Fixed a few UI related issues in HID Host reg/de-reg
11/07/07    GS  Initial Revision 

===========================================================================*/
#include "comdef.h"

#ifdef FEATURE_APP_BLUETOOTH
#include "BTApp.h"
#include "BTAppUtils.h"
#include "btapp_res.h"

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

#endif /* FEATURE_APP_BLUETOOTH */
