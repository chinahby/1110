#ifndef __BTAPP_ICP_H__
#define __BTAPP_ICP_H__
/*****************************************************************************/
/*                                                                           */
/*                          FILE: BTAppICP.h                                 */
/*                                                                           */   
/*                   SERVICES: QCT UI BT BREW Application.                   */
/*                                                                           */
/*        GENERAL DESCRIPTION: This module contains ICP test feature.        */  
/*                                                                           */  
/*              (c) COPYRIGHT 2004-2005 QUALCOMM Incorporated.               */
/*                         All Rights Reserved.                              */ 
/*                                                                           */
/*                         QUALCOMM Proprietary                              */
/*****************************************************************************/


/*=============================================================================

  $Header: //source/qcom/qct/wconnect/bthost/brew/statapp/rel/00.00.26/src/BTAppICP.h#1 $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who   what, where, why
--------    ---   ---------------------------------------------------------
03/25/05    pn    Added BTApp_ICPConnect().
12/28/04    gs    Initial version
                  
=============================================================================*/

extern void    BTApp_ICPBuildMenu           ( CBTApp* pMe, 
                                              BTAppMenuType menu );
extern void    BTApp_ICPBuildMainMenu       ( CBTApp* pMe );
extern void    BTApp_ICPHandleUserEvents    ( CBTApp* pMe, 
                                              uint32 dwParam );
extern boolean BTApp_ICPHandleMenus         ( CBTApp* pMe, 
                                              uint16  key, 
                                              BTAppMenuType menu );
extern void    BTApp_ProcessICPNotifications( CBTApp* pMe, 
                                              AEEEvent evt, 
                                              NotificationData* );
extern void    BTApp_ICPConnect             ( CBTApp* pMe, 
                                              AEEBTBDAddr* pBDAddr );
#endif /* __BTAPP_ICP_H__ */

