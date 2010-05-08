#ifndef __BTAPP_BIP_H__
#define __BTAPP_BIP_H__
/*****************************************************************************/
/*                                                                           */
/*                          FILE: BTAppBIP.h                                 */
/*                                                                           */   
/*                   SERVICES: QCT UI BT BREW Application.                   */
/*                                                                           */
/*        GENERAL DESCRIPTION: This module contains BIP test feature.        */  
/*                                                                           */  
/*              Copyright (c) 2004-2008 QUALCOMM Incorporated.               */
/*                      All Rights Reserved.                                 */ 
/*              Qualcomm Confidential and Proprietary                        */
/*****************************************************************************/


/*=============================================================================

  $Header: //source/qcom/qct/wconnect/bthost/brew/statapp/rel/00.00.26/src/BTAppBIP.h#1 $
  $DateTime: 2009/01/07 18:14:54 $
  $Author: deepikas $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who   what, where, why
--------    ---   ---------------------------------------------------------
04/10/08    VM    Added BTApp_EnableBIP()
12/16/05    ns    Externalised function for remote printing
11/14/05    ns    Added support to send a BIP print request from other apps
08/17/05    dc    Function declaration for BIP concurrent Client Image Printing 
07/27/05    ns    Added support to enable BIP server during startup
07/13/05    ns    Added concurrency test automation support for BIP push, pull,
                  auto archive, remote camera, remote display,archived objects,
                  referenced objects.
01/10/05    dc    Modified the prototype name BTApp_DiagBIPClientConnect to
                  BTApp_DiagBIPClientConnectEx
01/07/05    jh    Added BTApp_BIPBuildClientSubMenu() prototype.
                  ( Moved prototype out of FEATURE_APP_TEST_AUTO )
12/14/04    gr    Added automation support for PUSH OPS, PULL OPS, IMG PRN
                  Automatic Archive, Remote Camera, Remote Display
                  Arch/Ref Objs
12/13/04    gr    Added automation support for BIP Connect/Disconnect
12/06/04    gr    Added automation support for BIP Register/DeRegister

=============================================================================*/
extern void    BTApp_BIPCleanup( CBTApp* pMe );
extern boolean BTApp_BIPInit( CBTApp* pMe );
extern void    BTApp_BIPBuildMenu( CBTApp* pMe, BTAppMenuType menu );
extern void    BTApp_BIPBuildMainMenu( CBTApp* pMe );
extern void    BTApp_BIPBuildServerMenu( CBTApp* pMe );
extern void    BTApp_BIPBuildClientMenu( CBTApp* pMe );
extern void    BTApp_BIPHandleUserEvents( CBTApp* pMe, uint32 dwParam );
extern boolean BTApp_BIPHandleMenus( CBTApp* pMe, uint16 key, 
                                     BTAppMenuType menu );
extern boolean BTApp_EnableBIP( CBTApp* pMe, boolean* pbSettingBondable, 
                                boolean* pbSettingDiscoverable );
extern void    BTApp_ProcessBIPNotifications( CBTApp* pMe, AEEEvent evt, 
                                              NotificationData* );
extern void    BTApp_BIPBuildClientSubMenu( CBTApp* pMe );
extern void    BTApp_BIPBuildEnableMenu( CBTApp* pMe );
extern boolean BTApp_BIPRemotePrintImage( CBTApp* pMe, uint32 dwParam );

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

#endif /* __BTAPP_BIP_H__ */

