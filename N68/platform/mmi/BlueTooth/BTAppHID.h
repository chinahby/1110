#ifndef __BTAPP_HID_H__
#define __BTAPP_HID_H__
/*****************************************************************************/
/*                                                                           */
/*                          FILE: BTAppHID.h                                 */
/*                                                                           */   
/*                   SERVICES: QCT UI BT BREW Application.                   */
/*                                                                           */
/*        GENERAL DESCRIPTION: This module contains HID test feature.        */  
/*                                                                           */  
/*              (c) COPYRIGHT 2007-2008 Qualcomm Incorporated.                    */
/*                      All Rights Reserved.                                 */ 
/*              Qualcomm Confidential and Proprietary                        */
/*****************************************************************************/
/*===========================================================================
                        EDIT HISTORY FOR MODULE
                        
$Header: //source/qcom/qct/wconnect/bthost/brew/statapp/rel/00.00.26/src/BTAppHID.h#1 $
$DateTime: 2009/01/07 18:14:54 $
$Author: deepikas $

when       who  what, where, why
--------   ---  ----------------------------------------------------------
04/10/08   VM   Added BTApp_EnableHID()
11/07/07   GS   Initial Release. 

===========================================================================*/
#include "comdef.h"

#include "BTApp_priv.h"		//"BTApp.h"

#include "BTAppUtils.h"
#include "btapp_res.h"

extern boolean BTApp_HIDInit( CBTApp* pMe );

extern void    BTApp_HIDCleanup( CBTApp* pMe );

extern boolean BTApp_HIDBuildMenu( CBTApp* pMe, BTAppMenuType menu );

extern void    BTApp_HIDBuildMainMenu( CBTApp* pMe );

extern void    BTApp_HIDBuildDevicesMenu( CBTApp* pMe );

extern void    BTApp_HIDBuildSettingsMenu( CBTApp* pMe );

extern void    BTApp_HIDBuildOperationsMenu( CBTApp* pMe );

extern void    BTApp_HIDBuildCertificationMenu( CBTApp* pMe );

extern void    BTApp_HIDBuildEventDisplay ( CBTApp* pMe);

extern boolean BTApp_HIDHandleMenus( CBTApp* pMe, uint16 key, 
                                     BTAppMenuType menu );

extern boolean BTApp_HIDHandleSelection( CBTApp* pMe, uint16 sel );

extern void BTApp_EnableHID( CBTApp* pMe, boolean* pbSettingBondable );

extern void    BTApp_ProcessHIDNotifications( CBTApp* pMe, AEEEvent evt, 
                                              const NotificationData*  pNotify);

extern boolean BTApp_ProcessHIDEvents( CBTApp* pMe,
                                       AEEEvent  eCode, 
                                       uint32    Param1,      
                                       uint32    Param2 );

extern boolean BTApp_HandleHIDDevicesMenu( CBTApp* pMe, uint16 key);

#endif //__BTAPP_HID_H__
