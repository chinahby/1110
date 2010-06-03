#ifndef __BTAPP_FTP_H__
#define __BTAPP_FTP_H__
/*****************************************************************************/
/*                                                                           */
/*                          FILE: BTAppFTP.h                                 */
/*                                                                           */   
/*                   SERVICES: QCT UI BT BREW Application.                   */
/*                                                                           */
/*        GENERAL DESCRIPTION: This module contains FTP test feature.        */  
/*                                                                           */  
/*              (c) COPYRIGHT 2004-2008 QUALCOMM Incorporated.               */
/*                      All Rights Reserved.                                 */ 
/*              Qualcomm Confidential and Proprietary                        */
/*****************************************************************************/


/*=============================================================================

  $Header: //source/qcom/qct/wconnect/bthost/brew/statapp/rel/00.00.26/src/BTAppFTP.h#1 $
  $DateTime: 2009/01/07 18:14:54 $
  $Author: deepikas $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who   what, where, why
--------    ---   ---------------------------------------------------------
04/10/08    VM    Added BTApp_EnableFTP()
12/06/05    sn    Added diagnostic support for FTP Client ABORT feature.
07/27/05    ns    Added support to enable FTP server during startup
07/12/05    dc    Added concurrency test automation interface for FTP Get 
                  operation
07/06/05    sn    Added BTApp_FTPBuildServerRegisterMenu().
06/27/05    ns    Added concurrency test automation interface for FTP Put 
                  operation
03/25/05    pn    Added BTApp_FTPConnect().
02/11/05    sn    Externalized FTP diagnostic related functions
02/04/05    sn    Externalized the function BTApp_FTPBuildSettingsMenu()
08/03/04    pn    BTApp_FTPHandleSelection is externalized.
07/29/04    pn    Added support for IBT FTP.
04/20/04    pn    Initial Revision.
                  
=============================================================================*/
extern boolean BTApp_FTPInit( CBTApp* pMe );
extern void    BTApp_FTPCleanup( CBTApp* pMe );
extern void    BTApp_FTPBuildMenu( CBTApp* pMe, BTAppMenuType menu );
extern void    BTApp_FTPBuildMainMenu( CBTApp* pMe );
extern void    BTApp_FTPBuildServerMenu( CBTApp* pMe );
extern void    BTApp_FTPBuildClientMenu( CBTApp* pMe );
extern void    BTApp_FTPBuildSettingsMenu( CBTApp* pMe );
extern void    BTApp_FTPBuildServerRegisterMenu( CBTApp* pMe );
extern void    BTApp_FTPBuildEnableMenu( CBTApp* pMe );
extern void    BTApp_FTPHandleUserEvents( CBTApp* pMe, uint32 dwParam );
extern boolean BTApp_FTPHandleMenus( CBTApp* pMe, uint16 key, 
                                     BTAppMenuType menu );
extern boolean BTApp_FTPHandleSelection( CBTApp* pMe, uint16 sel );
extern boolean BTApp_EnableFTP( CBTApp* pMe, boolean* pbSettingBondable, 
                                boolean* pbSettingDiscoverable );
extern void    BTApp_ProcessFTPNotifications( CBTApp* pMe, AEEEvent evt, 
                                              NotificationData* );
extern void    BTApp_FTPConnect( CBTApp* pMe, AEEBTBDAddr* pBDAddr );
extern void    BTApp_FTPLoadBrowsingMenu( CBTApp* pMe );

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_APP_TEST_AUTOMATION */

#endif /* __BTAPP_FTP_H__ */

