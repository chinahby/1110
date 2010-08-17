#ifndef __BTAPP_SAP_H__
#define __BTAPP_SAP_H__
/*****************************************************************************/
/*                                                                           */
/*                          FILE: BTAppSAP.h                                 */
/*                                                                           */   
/*                   SERVICES: QCT UI BT BREW Application.                   */
/*                                                                           */
/*        GENERAL DESCRIPTION: This module contains SAP test feature.        */  
/*                                                                           */  
/*                (c) COPYRIGHT 2005-2008 QUALCOMM Incorporated.                  */
/*                      All Rights Reserved.                                 */ 
/*              Qualcomm Confidential and Proprietary                        */
/*****************************************************************************/


/*=============================================================================

  $Header: //source/qcom/qct/wconnect/bthost/brew/statapp/rel/00.00.26/src/BTAppSAP.h#1 $
  $DateTime: 2009/01/07 18:14:54 $
  $Author: deepikas $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who   what, where, why
--------    ---   ---------------------------------------------------------
04/10/08    VM    Added BTApp_EnableSAP()
10/21/05    ns    Modified SAP server implementation to use ICARD interface 
                  instead of IBT
03/25/05    pn    Added BTApp_SAPConnect().
03/02/05    ns    Added Automation support for SAP
02/16/05    ns    Added support for IBT SAP
01/24/05    gr    Initial Release. 

=============================================================================*/


extern boolean BTApp_SAPBuildMenu( CBTApp* pMe, BTAppMenuType menu );
extern void    BTApp_SAPBuildMainMenu( CBTApp* pMe );
extern void    BTApp_SAPBuildServerMenu( CBTApp* pMe );
extern void    BTApp_SAPBuildClientMenu( CBTApp* pMe );
extern void    BTApp_SAPHandleUserEvents( CBTApp* pMe, uint32 dwParam );
extern boolean BTApp_SAPHandleMenus( CBTApp* pMe, uint16 key, 
                                     BTAppMenuType menu );
extern boolean BTApp_SAPHandleSelection( CBTApp* pMe, uint16 sel );
extern boolean BTApp_EnableSAP( CBTApp* pMe );
extern void    BTApp_ProcessSAPNotifications( CBTApp* pMe, AEEEvent evt, 
                                              NotificationData* pData );
extern void    BTApp_ProcessSAPCardNotifications( CBTApp* pMe, 
                                                  uint16 dwMask, 
                                                  NotificationData* pData );
extern void    BTApp_SAPClientConnect( CBTApp* pMe, AEEBTBDAddr* pBDAddr );
extern int     BTApp_SAPVerifyPIN( CBTApp* pMe );
extern boolean BTApp_SAPCardInit( CBTApp* pMe );
extern void    BTApp_SAPCardCleanup( CBTApp* pMe );
extern boolean BTApp_SAPRegister( CBTApp* pMe );
extern void    BTApp_SAPDeregister( CBTApp * pMe );
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
#endif /* __BTAPP_SAP_H__ */

