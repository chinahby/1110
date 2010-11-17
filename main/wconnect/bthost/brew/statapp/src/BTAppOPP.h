#ifndef __BTAPP_OPP_H__
#define __BTAPP_OPP_H__
/*****************************************************************************/
/*                                                                           */
/*                          FILE: BTAppOPP.h                                 */
/*                                                                           */   
/*                   SERVICES: QCT UI BT BREW Application.                   */
/*                                                                           */
/*        GENERAL DESCRIPTION: This module contains OPP test feature.        */  
/*                                                                           */  
/*              (c) COPYRIGHT 2004-2008 QUALCOMM Incorporated.               */
/*                      All Rights Reserved.                                 */ 
/*              Qualcomm Confidential and Proprietary                        */
/*****************************************************************************/


/*=============================================================================

  $Header: //source/qcom/qct/wconnect/bthost/brew/statapp/rel/00.00.26/src/BTAppOPP.h#1 $
  $DateTime: 2009/01/07 18:14:54 $
  $Author: deepikas $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who   what, where, why
--------    ---   ---------------------------------------------------------
04/10/08    VM    Added BTApp_EnableOPP()
12/28/05    sn    Added diagnostic support for OPP Client abort feature.
07/27/05    ns    Added support to enable FTP server during startup
07/12/05    dc    Added concurrency test automation interface for OPP Pull
                  operation
06/27/05    ns    Added concurrency test automation interface for OPP Push 
                  operation
03/25/05    pn    Added BTApp_OPPConnect().
11/23/04    gr    Added space between parameters and brackets
08/03/04    pn    BTApp_OPPHandleSelection is externalized.
07/27/04    gr    Added OPP Client connect, disconnect, push, pull
07/22/04    gr    Added OPP server Reg and Dereg Diagnostics
07/01/04    pn    Added support for IBT OPP.
04/20/04    pn    Initial Revision.
                  
=============================================================================*/
extern boolean BTApp_OPPInit( CBTApp* pMe );
extern void    BTApp_OPPCleanup( CBTApp* pMe );
extern void    BTApp_OPPBuildMenu( CBTApp* pMe, BTAppMenuType menu );
extern void    BTApp_OPPBuildMainMenu( CBTApp* pMe );
extern void    BTApp_OPPBuildServerMenu( CBTApp* pMe );
extern void    BTApp_OPPBuildClientMenu( CBTApp* pMe );
extern void    BTApp_OPPHandleUserEvents( CBTApp* pMe, uint32 dwParam );
extern boolean BTApp_OPPHandleMenus( CBTApp* pMe, uint16 key, 
                                     BTAppMenuType menu );
extern boolean BTApp_OPPHandleSelection( CBTApp* pMe, uint16 sel );
extern boolean BTApp_EnableOPP( CBTApp* pMe, boolean* pbSettingBondable, 
                                boolean* pbSettingDiscoverable );
extern void    BTApp_ProcessOPPNotifications( CBTApp* pMe, AEEEvent evt, 
                                              NotificationData* );
extern void    BTApp_OPPConnect( CBTApp* pMe, AEEBTBDAddr* pBDAddr );

//Add By zzg 2010_11_09
extern void    BTApp_OPPBuildSendFileMenu(CBTApp* pMe );
extern boolean BTApp_OPPHandleSendFileSelection( CBTApp* pMe, uint16 sel );


//Add End

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

#endif /* __BTAPP_OPP_H__ */

