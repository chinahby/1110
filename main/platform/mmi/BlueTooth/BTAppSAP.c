/*****************************************************************************/
/*                                                                           */
/*                          FILE: BTAppSAP.c                                 */
/*                                                                           */   
/*                   SERVICES: QCT UI BT BREW Application.                   */
/*                                                                           */
/*        GENERAL DESCRIPTION: This module contains SAP test feature.        */  
/*                                                                           */  
/*              Copyright (c) 2007-2008 QUALCOMM Incorporated.               */
/*                      All Rights Reserved.                                 */ 
/*              Qualcomm Confidential and Proprietary                        */
/*****************************************************************************/
/*===========================================================================
                        EDIT HISTORY FOR MODULE
                        
$Header: //source/qcom/qct/wconnect/bthost/brew/statapp/rel/00.00.26/src/BTAppSAP.c#1 $
$DateTime: 2009/01/07 18:14:54 $
$Author: deepikas $

when       who  what, where, why
--------   ---  ----------------------------------------------------------
09/23/08    PN    Removed compiler warnings.
05/12/08    VM    Replacing deprecated functions with BREW replacements.
04/10/08    VM    Added BTApp_EnableSAP()
03/06/08    VM    Removed redundant IBTEXTRM_SetSecBySvcCls call
02/05/08    mh    Fixed SAP register disconnect and deregister to have its own 
                  state on callbacks, setting AEECallback pfncancel is removed.
11/19/07    ns    Removed unnecessary building of dialog menu's during
                  automation while setting max and min packet sizes
11/01/07    ag    Merged Lisbon(BT2.1) code to the mainline
09/26/07    jj    Added different callback structues for SAP registration
                  deregistration and disconnection.
06/08/07   dsn    Fix typecasting for latest RVCT2.1 compiler.
05/09/07   dsn    Fix for RVCT21 compile errors when SAP is enabled.
03/14/07    ns    Replaced ICM with ITelephone
02/22/07    pn    Refreshes screen when server registration is done when top
                  menu is main or SAP Server.
01/26/07    pn    Added debug messages to callback functions.
11/13/06    pn    Refreshes screen when server deregistration is done, 
                  regardless of result.
08/21/06    pn    Allowed adversarial testing in client app.
01/05/06    ns    Added support for SAP APDU Tx and Rx through files in EFS
                  Added support to store ATR response to a file in EFS
                  Removed authenticate menu item from SAP
12/20/05    pn    Featurized SAP Client with FEATURE_BT_EXTPF_TEST.
12/15/05    ns    Enabled SAP client menu
11/30/05    pn    Added support for SAP deregistration while connected.
11/21/05    pn    Only allows Disconnect if registered.
11/03/05    su    When connected to SAP client, set subscription to unavailable
                  instead of changing operating mode.
10/21/05    ns    Modified SAP server implementation to use ICARD interface 
                  instead of IBT. SAP client menu option removed.
05/23/05    ns    Automated bonding procedure for SAP server.
03/25/05    pn    Made use of BTApp_SetBondable() and BTApp_ClearBondable().
                  Added BTApp_SAPConnect().
03/22/05    ns    Added support to set SAP Max and Min Frame size from UI
03/16/05    ns    Replaced #ifdef ICardConnection with BTAPP_ICARD_CONNECTION
                  as it is conflicting with an ICARD interface structure
                  Fixed SAP disconnect problem
03/02/05    ns    Added 
                  - support for SAP APDU requests
                  - removed password menu for SAP
                  - Automation support for SAP
02/23/05    ns    Req and Resp sent notification events added
                  ATR info stored in SAP structure
02/16/05    ns    Added SAP UI features
01/24/05    gr    Initial Release. 

===========================================================================*/
#include "comdef.h"

#ifdef FEATURE_APP_BLUETOOTH
#include "BTApp_priv.h"		//"BTApp.h"

#include "BTAppUtils.h"

#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif /* FEATURE_BT_EXTPF_SAP */
#endif /* FEATURE_APP_BLUETOOTH */

