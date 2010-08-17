#ifndef __BTAPP_BPP_H__
#define __BTAPP_BPP_H__
/*****************************************************************************/
/*                                                                           */
/*                          FILE: BTAppBPP.h                                 */
/*                                                                           */   
/*                   SERVICES: QCT UI BT BREW Application.                   */
/*                                                                           */
/*        GENERAL DESCRIPTION: This module contains BPP test feature.        */  
/*                                                                           */  
/*              (c) COPYRIGHT 2004-2008 QUALCOMM Incorporated.               */
/*                      All Rights Reserved.                                 */ 
/*              Qualcomm Confidential and Proprietary                        */
/*****************************************************************************/


/*=============================================================================

  $Header: //source/qcom/qct/wconnect/bthost/brew/statapp/rel/00.00.26/src/BTAppBPP.h#1 $
  $DateTime: 2009/01/07 18:14:54 $
  $Author: deepikas $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who   what, where, why
--------    ---   ---------------------------------------------------------
04/10/08    VM  Added BTApp_EnableBPP()
04/20/06    ns  Provided I/F to print file using BPP from another BREW App
07/27/05    ns  Added support to enable BPP server during startup
07/12/05    dc  Added concurrency test automation interface for BPP Send
                operation
03/25/05    pn  Added BTApp_BPPConnect().
01/26/05    dc  Added automation support for establishing secondary BPP 
                connection (STS).
01/05/05    ns  Added automation support for BPP Settings and Target Menu 
                options
11/23/04    gr  Added space between brackets and parameters
10/13/04    gr  Added support for 
                - Selecting OBEX servers for BPP
                - Selecting MIME type for BPP
                - Entering Job ID for BPP
                - Support for automation of Register and DeRegister
                - Support for Automation of SendSOAP
                - Support for Automation of Send
                  
=============================================================================*/
extern void    BTApp_BPPCleanup( CBTApp* pMe );
extern boolean BTApp_BPPInit( CBTApp* pMe );
extern void    BTApp_BPPBuildMenu           ( CBTApp* pMe, 
                                              BTAppMenuType menu );
extern void    BTApp_BPPBuildMainMenu       ( CBTApp* pMe );
extern void    BTApp_BPPHandleUserEvents    ( CBTApp* pMe, 
                                              uint32 dwParam );
extern boolean BTApp_BPPHandleMenus         ( CBTApp* pMe, 
                                              uint16  key, 
                                              BTAppMenuType menu );
extern boolean BTApp_EnableBPP( CBTApp* pMe, boolean* pbSettingBondable, 
                                boolean* pbSettingDiscoverable );
extern void    BTApp_ProcessBPPNotifications( CBTApp* pMe, 
                                              AEEEvent evt, 
                                              NotificationData* );
extern boolean BTApp_BPPSendFile            ( CBTApp* pMe, 
                                              AEEBTBPPdocType docType, 
                                              uint32 jobID );
extern void    BTApp_BPPConnect( CBTApp* pMe, AEEBTBDAddr* pBDAddr );
extern boolean BTApp_BPPRemotePrintReq( CBTApp* pMe, uint32 dwParam );

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
#endif /* __BTAPP_BPP_H__ */

