#ifndef __BTAPP_OBEX_H__
#define __BTAPP_OBEX_H__
/*****************************************************************************/
/*                                                                           */
/*                          FILE: BTAppOBEX.h                                 */
/*                                                                           */   
/*                   SERVICES: QCT UI BT BREW Application.                   */
/*                                                                           */
/*        GENERAL DESCRIPTION: This module contains OBEX test feature.        */  
/*                                                                           */  
/*                (c) COPYRIGHT 2004-2008 QUALCOMM Incorporated.                  */
/*                      All Rights Reserved.                                 */ 
/*              Qualcomm Confidential and Proprietary                        */
/*****************************************************************************/


/*=============================================================================

  $Header: //source/qcom/qct/wconnect/bthost/brew/statapp/rel/00.00.26/src/BTAppOBEX.h#1 $
  $DateTime: 2009/01/07 18:14:54 $
  $Author: deepikas $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who   what, where, why
--------    ---   ---------------------------------------------------------
04/10/08    VM     Added BTApp_EnableOBEX()
11/23/06    JJ     Added automation plugins for IOBEX
10/04/06    smr    Initial version

=============================================================================*/

/* OBEX Client States */
#define OATBT_OBEX_CLI_DISCONNECTED        0
#define OATBT_OBEX_CLI_CONNECTED           1

/* OBEX Server States */
#define OATBT_OBEX_SRV_UNREGISTERED        0
#define OATBT_OBEX_SRV_REGISTERED          1
#define OATBT_OBEX_SRV_DISCONNECTED        2
#define OATBT_OBEX_SRV_CONNECTED           3

#define OATBT_OBEX_MAX_RCDS_EXPECTED       5

extern int BTApp_OBEXRegister( CBTApp* pMe );
extern void BTApp_OBEXConnect( CBTApp* pMe, AEEBTBDAddr* pBDAddr );
extern void    BTApp_OBEXCleanup( CBTApp* pMe );
extern boolean BTApp_OBEXInit( CBTApp* pMe );
extern void    BTApp_OBEXBuildMenu( CBTApp* pMe, BTAppMenuType menu );
extern void    BTApp_OBEXHandleUserEvents( CBTApp* pMe, uint32 dwParam );
extern boolean BTApp_OBEXHandleMenus( CBTApp* pMe, uint16 key, 
                                     BTAppMenuType menu );
extern void BTApp_EnableOBEX( CBTApp* pMe, boolean* pbSettingBondable, 
                               boolean* pbSettingDiscoverable );
extern boolean BTApp_ProcessOBEXNotifications( CBTApp* pMe, AEEEvent  eCode, 
                                               uint16 wParam, uint32 dwParam );

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif

#endif /* __BTAPP_OBEX_H__ */

