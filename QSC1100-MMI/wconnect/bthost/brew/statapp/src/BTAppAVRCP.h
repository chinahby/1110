#ifndef __BTAPP_AVRCP_H__
#define __BTAPP_AVRCP_H__
/*****************************************************************************/
/*                                                                           */
/*                          FILE: BTAppAVRCP.h                               */
/*                                                                           */   
/*                   SERVICES: QCT UI BT BREW Application.                   */
/*                                                                           */
/*        GENERAL DESCRIPTION: This module contains AVRCP test feature.      */  
/*                                                                           */  
/*              Copyright (c) 2005-2008 QUALCOMM Incorporated.               */
/*                      All Rights Reserved.                                 */ 
/*              Qualcomm Confidential and Proprietary                        */
/*****************************************************************************/


/*=============================================================================

  $Header: //source/qcom/qct/wconnect/bthost/brew/statapp/rel/00.00.26/src/BTAppAVRCP.h#1 $
  $DateTime: 2009/01/07 18:14:54 $
  $Author: deepikas $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who   what, where, why
--------    ---   ---------------------------------------------------------
04/10/08    VM     Added BTAPP_EnableAVRCP()
01/23/06    ns     Added diagnostic support to query information about the 
                   operations performed by HS/HF
12/02/05    sn     Externalized BTApp_AVRCPInit() and BTApp_AVRCPCleanup().
03/14/05    jtl    Initial Release. 

=============================================================================*/

extern boolean BTApp_AVRCPInit( CBTApp* pMe );
extern void BTApp_AVRCPCleanup( CBTApp* pMe );
extern void BTApp_AVRCPDisable( CBTApp* pMe );
extern boolean BTApp_BuildAVRCPTestMenu( CBTApp* pMe, BTAppMenuType menu );
extern boolean BTApp_HandleAVRCPTestMenu( CBTApp* pMe, AEEEvent evt, BTAppMenuType menu );
extern void BTApp_EnableAVRCP( CBTApp* pMe, boolean* pbSettingBondable );
extern void BTApp_ProcessAVRCPNotifications( CBTApp* pMe, AEEEvent evt, NotificationData* pData );
extern void BTApp_AVRCPHandleDevListMenu( CBTApp* pMe );

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

#endif /* __BTAPP_AVRCP_H__ */

