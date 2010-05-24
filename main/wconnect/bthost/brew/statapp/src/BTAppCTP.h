#ifndef __BTAPP_CTP_H__
#define __BTAPP_CTP_H__
/*****************************************************************************/
/*                                                                           */
/*                          FILE: BTAppCTP.h                                 */
/*                                                                           */   
/*                   SERVICES: QCT UI BT BREW Application.                   */
/*                                                                           */
/*        GENERAL DESCRIPTION: This module contains CTP test feature.        */  
/*                                                                           */  
/*                (c) COPYRIGHT 2004 QUALCOMM Incorporated.                  */
/*                         All Rights Reserved.                              */ 
/*                                                                           */
/*                         QUALCOMM Proprietary                              */
/*****************************************************************************/


/*=============================================================================

  $Header: //source/qcom/qct/wconnect/bthost/brew/statapp/rel/00.00.26/src/BTAppCTP.h#1 $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who   what, where, why
--------    ---   ---------------------------------------------------------
12/28/04    gs    Initial version
                  
=============================================================================*/

extern void    BTApp_CTPBuildMenu           ( CBTApp* pMe, 
                                              BTAppMenuType menu );
extern void    BTApp_CTPBuildMainMenu       ( CBTApp* pMe );
extern void    BTApp_CTPHandleUserEvents    ( CBTApp* pMe, 
                                              uint32 dwParam );
extern boolean BTApp_CTPHandleMenus         ( CBTApp* pMe, 
                                              uint16  key, 
                                              BTAppMenuType menu );
extern void    BTApp_ProcessCTPNotifications( CBTApp* pMe, 
                                              AEEEvent evt, 
                                              NotificationData* );
#endif /* __BTAPP_CTP_H__ */

