#ifndef __DATASTATSDLG_H
#define __DATASTATSDLG_H

/*=============================================================================

FILE: DataStatsDlg.h

SERVICES: Data Diagnostics Dialog Handlers

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:


(c) COPYRIGHT 2002 - 2006 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
						EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/DataStats/DataStatsDlg.h#12 $
$DateTime: 2007/06/20 12:12:33 $
$Author: sanjinij $
$Change: 498942 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
09/15/06   sj       Featurized pcscf addr changes with FEATURE_IMS
08/02/06   sj       Added support for PCSCF Address
08/02/05   sun      Added support for IPV6
05/02/05   RI       Added support for QoS.
07/12/04   sun      Added support for 16 profiles and to edit profile name
12/04/03   ram      Fixed compile error.
29/10/03   ram      Integration with ICM and code-cleanup.
07/23/03   ram      Created Initial version

=============================================================================*/


/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datastatsapp_res.h"
#include "AEEStdLib.h" // required for "boolean" data type
/*===========================================================================
FUNCTION CDataStatsApp_CallSummaryDlgHandler

DESCRIPTION
  The Call Summary Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_CallSummaryDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);
/*===========================================================================
FUNCTION CDataStatsApp_CallDetailDlgHandler

DESCRIPTION
  The Call Detail Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_CallDetailDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);
/*===========================================================================
FUNCTION CDataStatsApp_CallEndedDlgHandler

DESCRIPTION
  The Call Ended Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_CallEndedDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);
/*===========================================================================
FUNCTION CDataStatsApp_EditSecurityNameDlgHandler

DESCRIPTION
  Edit Security Password for CHAP

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditCHAPPasswordDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);

/*===========================================================================
FUNCTION CDataStatsApp_EditPAPParamsDlgHandler

DESCRIPTION
  Edit Security User Name and Password for PAP

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditPAPParamsDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);
/*===========================================================================
FUNCTION CDataStatsApp_SettingsDlgHandler

DESCRIPTION
  The Data Profile Settings Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_SettingsDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);
/*===========================================================================
FUNCTION CDataStatsApp_StatisticsDlgHandler

DESCRIPTION
  The Data Statistics Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_StatisticsDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);
/*===========================================================================
FUNCTION CDataStatsApp_SelectActiveProfileDlgHandler

DESCRIPTION
  Select Active Profile Dialog Event Handler

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_SelectActiveProfileDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);
/*===========================================================================
FUNCTION CDataStatsApp_EditProfileDlgHandler

DESCRIPTION
  The Edit Profile Event Handler:Select the profile to edit from the Menu

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditProfileDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);
/*===========================================================================
FUNCTION CDataStatsApp_ViewProfileDlgHandler

DESCRIPTION
  The View profile Dialog Event Handler

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_ViewProfileDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);
/*===========================================================================
FUNCTION CDataStatsApp_EditAPNDlgHandler

DESCRIPTION
  The Edit APN Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditAPNDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);
/*===========================================================================
FUNCTION CDataStatsApp_EditIPDlgHandler

DESCRIPTION
  The Edit IP Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditIPDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);
/*===========================================================================
FUNCTION CDataStatsApp_EditSecurityDlgHandler

DESCRIPTION
  The Edit Security Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditSecurityDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);

/*===========================================================================
FUNCTION CDataStatsApp_EditQoSDlgHandler

DESCRIPTION
  The Edit QoS Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditQoSDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);

/*===========================================================================
FUNCTION CDataStatsApp_EditQoSUMTSDlgHandler

DESCRIPTION
  The Edit QoS Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditQoSUMTSDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);

/*===========================================================================
FUNCTION CDataStatsApp_EditQoSUMTSTCDlgHandler

DESCRIPTION
  The Edit QoS UMTS Traffic Class Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditQoSUMTSTCDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);

/*===========================================================================
FUNCTION CDataStatsApp_EndCallDlgHandler

DESCRIPTION
  The End Call Dialog (Single - from the Call Detail Screen) Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EndCallDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);
/*===========================================================================
FUNCTION CDataStatsApp_EditQoSUMTSMaxUlBRDlgHandler

DESCRIPTION
  The Edit QOS UMTS MAX UL Bit rate Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditQoSUMTSMaxUlBRDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);

/*===========================================================================
FUNCTION CDataStatsApp_EditQoSUMTSMaxDlBRDlgHandler

DESCRIPTION
  The Edit QOS UMTS Maximum DL Bit rate Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditQoSUMTSMaxDlBRDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);
/*===========================================================================
FUNCTION CDataStatsApp_EditQoSUMTSMinUlBRDlgHandler

DESCRIPTION
  The Edit QOS UMTS Minimum UL Bit rate Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditQoSUMTSMinUlBRDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);
/*===========================================================================
FUNCTION CDataStatsApp_EditQoSUMTSMinDlBRDlgHandler

DESCRIPTION
  The Edit QOS UMTS Minimum DL Bit rate Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditQoSUMTSMinDlBRDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);
/*===========================================================================
FUNCTION CDataStatsApp_EditQoSGPRSDlgHandler

DESCRIPTION
  Edit GPRS QOS Event Handler.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditQoSGPRSDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);
/*===========================================================================
FUNCTION CDataStatsApp_EditQoSGPRSPrecDlgHandler

DESCRIPTION
  The Edit QOS GPRS Precedence Class Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditQoSGPRSPrecDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);
/*===========================================================================
FUNCTION CDataStatsApp_EditQoSGPRSDelayDlgHandler

DESCRIPTION
  The Edit QOS GPRS Delay Class Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditQoSGPRSDelayDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);
/*===========================================================================
FUNCTION CDataStatsApp_EditQoSGPRSRelDlgHandler

DESCRIPTION
  The Edit QOS GPRS Reliability Class Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditQoSGPRSRelDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);
/*===========================================================================
FUNCTION CDataStatsApp_EditQoSGPRSPeakDRDlgHandler

DESCRIPTION
  The Edit QOS GPRS Peak Data rate Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditQoSGPRSPeakDRDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);
/*===========================================================================
FUNCTION CDataStatsApp_EditQoSGPRSMeanDRDlgHandler

DESCRIPTION
  The Edit QOS GPRS Mean Data rate Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditQoSGPRSMeanDRDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);
/*===========================================================================
FUNCTION CDataStatsApp_EndAllDlgHandler

DESCRIPTION
  The End All Calls Dialog (from the Call summary Screen) Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EndAllDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);

/*===========================================================================
FUNCTION CDataStatsApp_ErrorDlgHandler

DESCRIPTION
  The Error Dialog  Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_ErrorDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);

/*===========================================================================
FUNCTION CDataStatsApp_DDTMDlgHandler

DESCRIPTION
  The Error Dialog  Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_DDTMDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);

/*===========================================================================
FUNCTION CDataStatsApp_EditProfileName

DESCRIPTION
  The Error Dialog  Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditProfileName(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);


void CDataStatsApp_UpdateSummaryMenu(void *pUser, AEECMNotifyInfo *pEventInfo);


/*===========================================================================
FUNCTION CDataStatsApp_EditPDPTypeDlgHandler

DESCRIPTION
  The Error Dialog  Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditPDPTypeDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);

#ifdef FEATURE_DATA_IMS
/*===========================================================================
FUNCTION CDataStatsApp_PCSCFReqDlgHandler

DESCRIPTION
  The Dialog Handler for setting PCSCF Address

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_PCSCFReqDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);

/*===========================================================================
FUNCTION CDataStatsApp_DHCPPCSCFDlgHandler

DESCRIPTION
  The PCSCF Address using DHCP Dialog Event Handler

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_DHCPPCSCFDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);

/*===========================================================================
FUNCTION CDataStatsApp_IMCNFlagDlgHandler

DESCRIPTION
  The IM CN Flag Dialog Event Handler


DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_IMCNFlagDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam);
#endif //FEATURE_DATA_IMS
#endif // __DATASTATSDLG_H
