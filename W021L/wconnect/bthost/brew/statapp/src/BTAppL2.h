#ifndef __BTAPPL2_H__
#define __BTAPPL2_H__
/*****************************************************************************/
/*                                                                           */
/*                          FILE: BTAppL2.h                                  */
/*                                                                           */   
/*                   SERVICES: QCT UI BT BREW Application.                   */
/*                                                                           */
/*        GENERAL DESCRIPTION: This module contains L2 test feature.         */  
/*                                                                           */  
/*                (c) COPYRIGHT 2007-2008 QUALCOMM Incorporated.                  */
/*                      All Rights Reserved.                                 */ 
/*              Qualcomm Confidential and Proprietary                        */
/*****************************************************************************/


/*=============================================================================

  $Header: //source/qcom/qct/wconnect/bthost/brew/statapp/rel/00.00.26/src/BTAppL2.h#1 $
  $DateTime: 2009/01/07 18:14:54 $
  $Author: deepikas $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who   what, where, why
--------    ---   ---------------------------------------------------------
04/10/08    VM     Added BTApp_EnableL2()
02/27/07    jtl    Initial Release. 

=============================================================================*/

#define BTAPP_L2_TEST_MTU 4096

extern boolean BTApp_L2Init( CBTApp* pMe );
extern void BTApp_L2Cleanup( CBTApp* pMe );
extern void BTApp_L2Disable( CBTApp* pMe );
extern boolean BTApp_BuildL2TestMenu( CBTApp* pMe );
extern boolean BTApp_HandleL2TestMenu( CBTApp* pMe, AEEEvent evt );
extern boolean BTApp_BuildL2ResultsMenu( CBTApp* pMe );
extern boolean BTApp_HandleL2ResultsMenu( CBTApp* pMe, AEEEvent evt );
extern void BTApp_EnableL2( CBTApp* pMe );
extern void BTApp_ProcessL2Notifications( CBTApp* pMe, AEEEvent evt, NotificationData* pData );
extern void BTApp_L2HandleDevListMenu( CBTApp* pMe );


#ifdef FEATURE_APP_TEST_AUTOMATION
#endif //FEATURE_APP_TEST_AUTOMATION

#endif /* __BTAPPL2_H__ */

