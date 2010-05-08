/*****************************************************************************/
/*                                                                           */
/*                          FILE: BTAppIDiagUtils.c                          */
/*                                                                           */   
/*                   SERVICES: QCT UI BT BREW Application.                   */
/*                                                                           */
/*        GENERAL DESCRIPTION: This module contains BPP test feature.        */  
/*                                                                           */  
/*                (c) COPYRIGHT 2004-2007 QUALCOMM Incorporated.             */
/*                         All Rights Reserved.                              */ 
/*                                                                           */
/*                         QUALCOMM Proprietary                              */
/*****************************************************************************/
/*===========================================================================
                        EDIT HISTORY FOR MODULE
                        
$Header: //source/qcom/qct/wconnect/bthost/brew/statapp/rel/00.00.26/src/BTAppIDiagUtils.c#1 $

when       who  what, where, why
--------   ---  ----------------------------------------------------------
02/16/07    pn    Fixed critical lint error.
09/12/06    gs    Fixing RVCT compile issues on 7K target.
12/25/05    dc    Changed BTApp_DiagProcessDevListResponse to return the 
                  device address every time and the device name if available
09/28/05    ns    Added debug messages to track automation events
09/15/05    ns    Modified DiagProcessDevListResponse to display address 
                  when device name is not available
07/13/05    ns    Added BTApp_DiagLoadTestMenuConc to load test menu while
                  concurrency operations.
05/28/05    ns    Modified BTApp_DiagProcessDevListResponse so that in case of
                  device list event it will not check for validity of the name.
                  Modified BTApp_DiagProcessDeviceMatch to handle the case when
                  menu not in device search response.
05/10/05    ns    BTApp_DiagProcessDevListResponse handling is changed, device
                  name and details are already added to RM list during 
                  discovery response, no need to read again from the device
11/23/04    gr    Initial Release. Creating new file to move generic util
                  functions from BTAPP.c

===========================================================================*/
#include "comdef.h"

#ifdef FEATURE_APP_BLUETOOTH
#include "BTApp.h"
#include "BTAppUtils.h"

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
#endif /* FEATURE_APP_BLUETOOTH */



