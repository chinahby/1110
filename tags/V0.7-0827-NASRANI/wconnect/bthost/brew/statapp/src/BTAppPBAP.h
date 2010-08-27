#ifndef __BTAPP_PBAP_H__
#define __BTAPP_PBAP_H__
/*****************************************************************************/
/*                                                                           */
/*                          FILE: BTAppPBAP.h                                */
/*                                                                           */   
/*                   SERVICES: QCT UI BT BREW Application.                   */
/*                                                                           */
/*        GENERAL DESCRIPTION: This module contains PBAP test feature.       */  
/*                                                                           */  
/*              (c) COPYRIGHT 2006-2008 QUALCOMM Incorporated.               */
/*                      All Rights Reserved.                                 */ 
/*              Qualcomm Confidential and Proprietary                        */
/*****************************************************************************/


/*=============================================================================

  $Header: //source/qcom/qct/wconnect/bthost/brew/statapp/rel/00.00.26/src/BTAppPBAP.h#1 $
  $DateTime: 2009/01/07 18:14:54 $
  $Author: deepikas $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who   what, where, why
--------    ---   ---------------------------------------------------------
04/10/08    VM    Added BTApp_EnablePBAP()
07/13/06    ns    Added concurrency support for pull vcard obj operation
07/07/06    ns    Added support to allow browsing from one level above the 
                  leaf directory. Added concurrency support for download PB obj
06/07/06    ns    Added automation support for PBAP
02/01/06    ns    Initial Revision.
                  
=============================================================================*/
extern boolean BTApp_PBAPInit( CBTApp* pMe );
extern void    BTApp_PBAPCleanup( CBTApp* pMe );
extern void    BTApp_PBAPBuildMenu( CBTApp* pMe, BTAppMenuType menu );
extern boolean BTApp_PBAPHandleMenus( CBTApp* pMe, 
                                      uint16 key, 
                                      BTAppMenuType menu );
extern boolean BTApp_EnablePBAP( CBTApp* pMe, boolean* pbSettingBondable, 
                               boolean* pbSettingDiscoverable );
extern void BTApp_ProcessPBAPNotifications( CBTApp* pMe, AEEEvent evt, 
                                            NotificationData* pData );
extern void    BTApp_PBAPHandleUserEvents( CBTApp* pMe, uint32 dwParam );
extern void    BTApp_PBAPDoClientSettingsMenu( CBTApp* pMe, int item );
extern void    BTApp_PBAPConnect( CBTApp* pMe, AEEBTBDAddr* pBDAddr );
extern void    BTApp_PBAPBuildClientBrowseMenu( CBTApp* pMe );

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif

#endif /* __BTAPP_PBAP_H__ */

