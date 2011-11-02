#ifndef __BTAPP_A2DP_H__
#define __BTAPP_A2DP_H__
/*****************************************************************************/
/*                                                                           */
/*                          FILE: BTAppA2DP.h                                */
/*                                                                           */   
/*                   SERVICES: QCT UI BT BREW Application.                   */
/*                                                                           */
/*        GENERAL DESCRIPTION: This module contains A2DP test feature.       */  
/*                                                                           */  
/*              Copyright (c) 2005-2008 QUALCOMM Incorporated.               */
/*                      All Rights Reserved.                                 */ 
/*              Qualcomm Confidential and Proprietary                        */
/*****************************************************************************/


/*=============================================================================

  $Header: //source/qcom/qct/wconnect/bthost/brew/statapp/rel/00.00.26/src/BTAppA2DP.h#1 $
  $DateTime: 2009/01/07 18:14:54 $
  $Author: deepikas $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who   what, where, why
--------    ---   ---------------------------------------------------------
04/10/08    VM     Added BTApp_EnableA2DP()
12/02/05    sn     Externalized BTApp_A2DPInit() and BTApp_A2DPCleanup().
06/16/05    ns     Added UI and automation support for A2DP control interface
05/24/05    dc     Added API's for A2DP Automation.
03/14/05    jtl    Initial Release. 

=============================================================================*/

extern boolean BTApp_A2DPInit( CBTApp* pMe );
extern void BTApp_A2DPCleanup( CBTApp* pMe );
extern void BTApp_A2DPDisable( CBTApp* pMe );
extern boolean BTApp_BuildA2DPTestMenu( CBTApp* pMe );
extern boolean BTApp_HandleA2DPTestMenu( CBTApp* pMe, AEEEvent evt );
extern void BTApp_EnableA2DP( CBTApp* pMe, boolean* pbSettingBondable );
extern void BTApp_ProcessA2DPNotifications( CBTApp* pMe, AEEEvent evt, NotificationData* pData );
extern void BTApp_A2DPHandleDevListMenu( CBTApp* pMe );
extern boolean BTApp_BuildA2DPControlMenu( CBTApp* pMe );
extern boolean BTApp_HandleA2DPControlMenu( CBTApp* pMe, AEEEvent evt );
extern void BTApp_InitiateA2DPConn( CBTApp* pMe );

extern void BTApp_A2DPSetRetries(CBTApp* pMe, boolean least);		//Add By zzg 2011_10_31


#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

#endif /* __BTAPP_A2DP_H__ */

