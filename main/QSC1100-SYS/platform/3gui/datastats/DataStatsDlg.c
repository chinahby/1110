/*=============================================================================

FILE: DataStatsDlg.c

SERVICES: Data Diagnostics Dialog Handlers and related functionality

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:


(c) COPYRIGHT 2002 - 2007 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
                   EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/DataStats/DataStatsDlg.c#51 $
$DateTime: 2008/08/31 21:54:10 $
$Author: ravisoni $
$Change: 734181 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      ---------------------------------------------------------
08/26/08   Ravi    Removed all the deprecated function 
10/03/07   cvs      Fix issue with IP addresses reselecting all text when text changed
09/13/07   cvs      Fix issue with incorrect modification for IPDP API change
06/15/07   sj       Addition of two flags:im_cn_flag and dhcp_pcscf flag,
                    Featurized PCSCF FLAG changes with FEATURE_DATA_IMS
05/05/07   mjm      Updated to account for changes to IPDP interface
02/05/07   RT       PAP username and password dialog box should not select 
                    text on focus
11/20/06   sj       Read entire profile data before updating profile changes                    
09/27/06   jas      Cleaning up compiler warnings on RVCT
09/15/06   sj       Featurized pcscf addr changes with FEATURE_IMS
08/08/06   sj       Added support for PCSCF address
07/12/06   jas      Cleaning up compiler warnings on RVCT
3/17/06    sj       Initialize PDPProfile to zeros before filling and saving profile information
2/2/06     sj       Save PDP type, Primary and Secondary DNS values to the PDP profile
11/10/05   jas      Fixing compiler warning
11/04/05   ak       Fixed Lint Error
08/19/05   jas      Add more featurization
08/02/05   sun      Added support for IPV6
08/02/05   AT       Correct some featurizations.
05/31/05   sun      Added TX RX rate Display for WLAN
05/02/05   RI       Added QoS menu
02/17/05   sun      Added profiles
12/27/04   sun      Fixed the Text Control Size
09/14/04   sun      Removed AEEPDP_MAX_PDP_PROFILE_NUM
09/01/04   sun      Deleted one extra profile that was being created
07/12/04   sun      Added support for 16 profiles and to edit profile name
05/21/04   ak       Fixed Compiler Warning
05/20/04   sun      Fixed Display of Rate
05/14/04   ak       Fixed Compiler Warning
05/13/04   sun      Fixed the RX TX Display
03/04/04   ram      QIS review changes.
02/04/04   kar      Fixed compile error
01/19/04   ram      Use brew extension to set PDP profiles.
12/11/03   ram      Highlight selection in DDTM menu.
12/05/03   ram      Fixed buffer overflow.
12/03/03   ram      Change max HDR baud rates.
12/03/03   ram      Display DDTM menu for Invalid preference too.
12/01/03   ram      Lint related changes.
11/20/03   ram      DDTM Support and remove HDR_ABORT_PPP support
11/18/03   ram      Code cleanup
12/11/03   ram      Launch app with arguments and dummy strings for GW Mode.
31/10/03   ram      Added Get1XHDRBaudRates().
29/10/03   ram      Integration with ICM and code-cleanup.
09/08/03   ram      Added functionality for PDP profile edit, code-cleanup.
07/23/03   ram      Created Initial version

=============================================================================*/


/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/
//lint -save -e713
//lint -save -e740

#include "AppComFunc.h"
#include "DataStatsApp.h"
#include "DataStatsDlg.h"

#include "AEEMenu.h" // For IMenuCtl
#include "AEEShell.h" //For IDialog,IStatic
#include "AEEText.h"

#include "AEEConfig.h"
#include "OEMCFGI.h"
#include "AEECM.h"




/* HDR Max Rates */
#define DS_DATA_RATE_2_4M   300000 /*HDR TX RATE*/
#define DS_DATA_RATE_153_6  19660 /*HDR RX RATE*/

#ifdef FEATURE_HDR
#include "hdrts.h"
  #ifdef FEATURE_HDR_IS890
    #include "hdris890.h"
#endif
#endif

#define  NO_DIALOG 0

#define DATASTATS_MAX_BUFFER_SIZE 36

#define DATASTATS_MAX_WIDE_BUFFER_SIZE (sizeof(AECHAR)*DATASTATS_MAX_BUFFER_SIZE)

// Ended calls are assigned the ItemId as 
#define ENDED_CALL_ITEMID_OFFSET 10

//extern void ui_end_call(byte num_call_id, const byte* list_of_call_id);
/* Dlg Item Action function pointer structure */
typedef struct _ITEMHANDLER ITEMHANDLER;

typedef boolean (*PFNITEMHANDLER) (CDataStatsApp *pMe, const ITEMHANDLER *pItem);

//Forward declaration for some functions..
static void CDataStatsApp_SetDefaultSoftkeyLook(IShell *pIShell, uint16 wSKId);
static void CDataStatsApp_SetMenuLook(IShell *pIShell, uint16 menuId);
static boolean CDataStatsApp_GetLastDataStats(CDataStatsApp *pMe);
static boolean CDataStatsApp_ResetLastDataStats(CDataStatsApp *pMe);



#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
//Some helper functions
static void IPToAECHAR(uint32 ip, AECHAR *a); 
static void IPToAECharIP(uint32 ip,AECharIP *aIp);
static void IPV6ToAECHAR(AEEPDPUMTSPDPAddr ip, AECHAR *a);
#endif //(FEATURE_WCDMA) || (FEATURE_GSM_GPRS)


static boolean StripSettingsMenu(CDataStatsApp *pMe);

//Main Handler function.
static boolean CDataStatsApp_ExecuteItemAction(CDataStatsApp *pMe, uint16 wParam );
static boolean CDataStatsApp_AddMenuItem(CDataStatsApp *pMe, IMenuCtl *pIMenuCtl, AEECMCallID CallID,
                                         uint16 wItemID, AECHAR *pszBuf, AEECMCallInfo *pCallInfo);
//Specific handler functions.
static boolean CDataStatsApp_BranchDialog(CDataStatsApp *pMe, const ITEMHANDLER *pItem);
static boolean CDataStatsApp_EndDialog(CDataStatsApp *pMe, const ITEMHANDLER *pItem);
static boolean CDataStatsApp_CreateDialog(CDataStatsApp *pMe, uint16 nDlgID);
static boolean CDataStatsApp_EndApplet(CDataStatsApp *pMe, const ITEMHANDLER *pItem);
static boolean CDataStatsApp_SetActiveProfile(CDataStatsApp *pMe, const ITEMHANDLER *pItem);
static boolean CDataStatsApp_ViewProfile(CDataStatsApp *pMe, const ITEMHANDLER *pItem);
static boolean CDataStatsApp_BranchIPDialog(CDataStatsApp *pMe, const ITEMHANDLER *pItem);
static boolean CDataStatsApp_SaveAPN(CDataStatsApp *pMe, const ITEMHANDLER *pItem);
static boolean CDataStatsApp_SavePDPType(CDataStatsApp *pMe, const ITEMHANDLER *pItem);
static boolean CDataStatsApp_SaveIP(CDataStatsApp *pMe, const ITEMHANDLER *pItem);
static boolean CDataStatsApp_SaveSecurity(CDataStatsApp *pMe, const ITEMHANDLER *pItem);
static boolean CDataStatsApp_SaveProfile(CDataStatsApp *pMe, const ITEMHANDLER *pItem);
static boolean CDataStatsApp_EndCall(CDataStatsApp *pMe, const ITEMHANDLER *pItem);
static boolean CDataStatsApp_SaveCHAPPassword(CDataStatsApp *pMe, const ITEMHANDLER *pItem);
static boolean CDataStatsApp_SavePAPParams(CDataStatsApp *pMe, const ITEMHANDLER *pItem);
static boolean CDataStatsApp_SaveDDTM(CDataStatsApp *pMe, const ITEMHANDLER *pItem);
static boolean CDataStatsApp_SaveProfileName(CDataStatsApp *pMe, const ITEMHANDLER *pItem);



#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
static int CDataStatsApp_StoreStringZ(byte *destBuf, IDialog *pIDialog, uint16 wStaticCtlId);
static void CDataStatsApp_InitActiveProfile(CDataStatsApp *pMe, uint16 wCtlId);
#endif //(FEATURE_WCDMA) || (FEATURE_GSM_GPRS)

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS) || defined( FEATURE_DDTM_CNTL)
static void CDataStatsDlg_ErrorDialog(CDataStatsApp *pMe, char *msg);
#define DATASTAT_ERROR_DLG(x) CDataStatsDlg_ErrorDialog(pMe, x);
#endif

void CDataStatsApp_RefreshDetailScreen(void *pUser);
void CDataStatsApp_RefreshSummaryMenu(void *pUser);

// Call detail dialog init
static void CDataStatsApp_InitCallDetailDialog(CDataStatsApp *pMe);
static void CDataStatsApp_InitCallEndedDialog(CDataStatsApp *pMe);

static uint16 CDataStatsApp_StatusDlg_GetIcon(CDataStatsApp *pMe, AEECMCallState CallState, AEECMCallType CallType);


static void RateIndicatorToAECHAR(CDataStatsApp *pMe, uint32 nRate, AECHAR *psBuf);

#ifdef FEATURE_DDTM_CNTL
static boolean CDataStatsApp_InitDDTMMenu(CDataStatsApp *pMe);
#endif

void CDataStatsApp_GetCallInformation(CDataStatsApp *pMe,
                           AEECMCallID call_id, 
                             uint32* RxDataRate,
                             uint32* TxDataRate,
                             byte *szSysInfo,
                             byte nMaxSysInfoLength
                             );

static void CDataStatsApp_GetAlphaString(CDataStatsApp *pMe, 
                                         AEECMCallID call_id,
                                         AECHAR *alpha,
                                         int size
                                         );

static boolean CDataStatsApp_SaveQoSType(CDataStatsApp *pMe, const ITEMHANDLER *pItem);
static boolean CDataStatsApp_SaveTrafficClass(CDataStatsApp *pMe, const ITEMHANDLER *pItem);
static boolean CDataStatsApp_SaveQoSUMTSMinDLBR(CDataStatsApp *pMe, const ITEMHANDLER *pItem);
static boolean CDataStatsApp_SaveQoSUMTSMinULBR(CDataStatsApp *pMe, const ITEMHANDLER *pItem);
static boolean CDataStatsApp_SaveQoSUMTSMaxDLBR(CDataStatsApp *pMe, const ITEMHANDLER *pItem);
static boolean CDataStatsApp_SaveQoSUMTSMaxULBR(CDataStatsApp *pMe, const ITEMHANDLER *pItem);
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
static void CDataStatsApp_SaveQoS(CDataStatsApp *pMe, AEEPDP *pdpProfile, uint32 *pFields);
#endif
static boolean CDataStatsApp_ViewQoS(CDataStatsApp *pMe, const ITEMHANDLER *pItem);
static boolean CDataStatsApp_SaveGPRSPrec(CDataStatsApp *pMe, const ITEMHANDLER *pItem);
static boolean CDataStatsApp_SaveGPRSDelay(CDataStatsApp *pMe, const ITEMHANDLER *pItem);
static boolean CDataStatsApp_SaveGPRSRel(CDataStatsApp *pMe, const ITEMHANDLER *pItem);
static boolean CDataStatsApp_SaveQoSGPRSMeanDR(CDataStatsApp *pMe, const ITEMHANDLER *pItem);
static boolean CDataStatsApp_SaveQoSGPRSPeakDR(CDataStatsApp *pMe, const ITEMHANDLER *pItem);
static boolean CDataStatsApp_SaveQoSUMTS(CDataStatsApp *pMe, const ITEMHANDLER *pItem);
static boolean CDataStatsApp_SaveQoSGPRS(CDataStatsApp *pMe, const ITEMHANDLER *pItem);
#ifdef FEATURE_DATA_IMS
static boolean CDataStatsApp_SavePCSCFRequest(CDataStatsApp *pMe, const ITEMHANDLER *pItem);
static boolean CDataStatsApp_SaveDHCPPCSCFRequest(CDataStatsApp *pMe, const ITEMHANDLER *pItem);
static boolean CDataStatsApp_SaveIMCNFlag(CDataStatsApp *pMe, const ITEMHANDLER *pItem);
#endif

struct _ITEMHANDLER
{
    uint16 wItemId; // Selected Item
    uint16 data; // The next to open dialog when this item is selected
    PFNITEMHANDLER pfItemHandler;
};

static const ITEMHANDLER MenuSelectionOperations[]=
{
    { IDL_SK_CALL_SUMMARY_SETTINGS, IDD_SETTINGS, CDataStatsApp_BranchDialog},
    { IDL_SK_CALL_SUMMARY_EXIT, NO_DIALOG, CDataStatsApp_EndApplet}, 
    
    { IDL_CALL_DETAIL_EXIT, NO_DIALOG, CDataStatsApp_EndDialog},
    
    { IDL_SK_CALL_ENDED_EXIT, NO_DIALOG, CDataStatsApp_EndDialog},

    { IDL_SK_SETTINGS_EXIT, NO_DIALOG, CDataStatsApp_EndDialog},
    
    { IDL_MENU_SETTINGS_STATISTICS, IDD_STATISTICS, CDataStatsApp_BranchDialog},
    { IDL_MENU_SETTINGS_DDTM, IDD_DDTM, CDataStatsApp_BranchDialog},
    { IDL_MENU_SETTINGS_EDIT_PROFILE, IDD_EDIT_PROFILE, CDataStatsApp_BranchDialog},
    { IDL_MENU_SETTINGS_SELECT_ACTIVE, IDD_SELECT_ACTIVE, CDataStatsApp_BranchDialog},

    { IDL_SK_SELECT_ACTIVE_OK, NO_DIALOG, CDataStatsApp_SetActiveProfile},
    { IDL_SK_SELECT_ACTIVE_CANCEL, NO_DIALOG, CDataStatsApp_EndDialog},

    { IDL_MENU_EDIT_PROFILE1, IDS_DD_PROFILE1, CDataStatsApp_ViewProfile},
    { IDL_MENU_EDIT_PROFILE2, IDS_DD_PROFILE2, CDataStatsApp_ViewProfile},
    { IDL_MENU_EDIT_PROFILE3, IDS_DD_PROFILE3, CDataStatsApp_ViewProfile},
    { IDL_MENU_EDIT_PROFILE4,  IDS_DD_PROFILE4,  CDataStatsApp_ViewProfile},
    { IDL_MENU_EDIT_PROFILE5,  IDS_DD_PROFILE5,  CDataStatsApp_ViewProfile},
    { IDL_MENU_EDIT_PROFILE6,  IDS_DD_PROFILE6,  CDataStatsApp_ViewProfile},
    { IDL_MENU_EDIT_PROFILE7,  IDS_DD_PROFILE7,  CDataStatsApp_ViewProfile},
    { IDL_MENU_EDIT_PROFILE8,  IDS_DD_PROFILE8,  CDataStatsApp_ViewProfile},
    { IDL_MENU_EDIT_PROFILE9,  IDS_DD_PROFILE9,  CDataStatsApp_ViewProfile},
    { IDL_MENU_EDIT_PROFILE10, IDS_DD_PROFILE10, CDataStatsApp_ViewProfile},
    { IDL_MENU_EDIT_PROFILE11, IDS_DD_PROFILE11, CDataStatsApp_ViewProfile},
    { IDL_MENU_EDIT_PROFILE12, IDS_DD_PROFILE12, CDataStatsApp_ViewProfile},
    { IDL_MENU_EDIT_PROFILE13, IDS_DD_PROFILE13, CDataStatsApp_ViewProfile},
    { IDL_MENU_EDIT_PROFILE14, IDS_DD_PROFILE14, CDataStatsApp_ViewProfile},
    { IDL_MENU_EDIT_PROFILE15, IDS_DD_PROFILE15, CDataStatsApp_ViewProfile},
    { IDL_MENU_EDIT_PROFILE16, IDS_DD_PROFILE16, CDataStatsApp_ViewProfile},
    { IDL_MENU_EDIT_PROFILE17, IDS_DD_PROFILE17, CDataStatsApp_ViewProfile},
    { IDL_MENU_EDIT_PROFILE18, IDS_DD_PROFILE18, CDataStatsApp_ViewProfile},
    { IDL_MENU_EDIT_PROFILE19, IDS_DD_PROFILE19, CDataStatsApp_ViewProfile},
    { IDL_MENU_EDIT_PROFILE20, IDS_DD_PROFILE20, CDataStatsApp_ViewProfile},
    { IDL_MENU_EDIT_PROFILE21, IDS_DD_PROFILE21, CDataStatsApp_ViewProfile},
    { IDL_MENU_EDIT_PROFILE22, IDS_DD_PROFILE22, CDataStatsApp_ViewProfile},
    { IDL_MENU_EDIT_PROFILE23, IDS_DD_PROFILE23, CDataStatsApp_ViewProfile},
    { IDL_MENU_EDIT_PROFILE24, IDS_DD_PROFILE24, CDataStatsApp_ViewProfile},
    { IDL_MENU_EDIT_PROFILE25, IDS_DD_PROFILE25, CDataStatsApp_ViewProfile},
              
    /* Profile fields */
    { IDL_MENU_VIEW_PROFILE_NAME, IDD_EDIT_PROFILE_NAME, CDataStatsApp_BranchDialog},
    { IDL_MENU_VIEW_APN, IDD_EDIT_APN, CDataStatsApp_BranchDialog},
    { IDL_MENU_VIEW_PDP_TYPE, IDD_PDP_TYPE, CDataStatsApp_BranchDialog},
    { IDL_MENU_VIEW_IP, IDD_EDIT_IP, CDataStatsApp_BranchIPDialog},
    { IDL_MENU_VIEW_DNS_PRIMARY, IDD_EDIT_IP, CDataStatsApp_BranchIPDialog},
    { IDL_MENU_VIEW_DNS_SECONDARY, IDD_EDIT_IP, CDataStatsApp_BranchIPDialog},
    { IDL_MENU_VIEW_SECURITY, IDD_EDIT_SECURITY, CDataStatsApp_BranchDialog},
    { IDL_MENU_VIEW_QOS, IDD_EDIT_QOS, CDataStatsApp_BranchDialog},
#ifdef FEATURE_DATA_IMS
    { IDL_MENU_VIEW_PCSCF_ADDR, IDD_EDIT_PCSCF, CDataStatsApp_BranchDialog},
    { IDL_MENU_VIEW_IMCN_FLAG, IDD_EDIT_IM_CN_FLAG, CDataStatsApp_BranchDialog},
    { IDL_MENU_VIEW_DHCP_PCSCF_FLAG, IDD_EDIT_DHCP_PCSCF, CDataStatsApp_BranchDialog},
#endif

    { IDL_MENU_EDIT_QOS_UMTS_REQ, IDS_DD_QOS_UMTS_REQ, CDataStatsApp_ViewQoS},
    { IDL_MENU_EDIT_QOS_GPRS_REQ, IDS_DD_QOS_GPRS_REQ, CDataStatsApp_ViewQoS},

    { IDL_MENU_EDIT_QOS_UMTS_MIN, IDS_DD_QOS_UMTS_MIN, CDataStatsApp_ViewQoS},
    { IDL_MENU_EDIT_QOS_GPRS_MIN, IDS_DD_QOS_GPRS_MIN, CDataStatsApp_ViewQoS},

    /* UMTS QoS fields: */
    { IDL_MENU_VIEW_QOS_UMTS_TC,        IDD_EDIT_QOS_UMTS_TC,        CDataStatsApp_BranchDialog},
    { IDL_MENU_VIEW_QOS_UMTS_MAX_UL_BR, IDD_EDIT_QOS_UMTS_MAX_UL_BR, CDataStatsApp_BranchDialog},
    { IDL_MENU_VIEW_QOS_UMTS_MAX_DL_BR, IDD_EDIT_QOS_UMTS_MAX_DL_BR, CDataStatsApp_BranchDialog},
    { IDL_MENU_VIEW_QOS_UMTS_MIN_UL_BR, IDD_EDIT_QOS_UMTS_MIN_UL_BR, CDataStatsApp_BranchDialog},
    { IDL_MENU_VIEW_QOS_UMTS_MIN_DL_BR, IDD_EDIT_QOS_UMTS_MIN_DL_BR, CDataStatsApp_BranchDialog},
    
    /* GPRS QOS fields */
    { IDL_MENU_VIEW_QOS_GPRS_REL_CLASS,   IDD_EDIT_QOS_GPRS_REL_CLASS,   CDataStatsApp_BranchDialog},
    { IDL_MENU_VIEW_QOS_GPRS_PREC_CLASS,  IDD_EDIT_QOS_GPRS_PREC_CLASS,  CDataStatsApp_BranchDialog},
    { IDL_MENU_VIEW_QOS_GPRS_DELAY_CLASS, IDD_EDIT_QOS_GPRS_DELAY_CLASS, CDataStatsApp_BranchDialog},
    { IDL_MENU_VIEW_QOS_GPRS_MEAN_DR,     IDD_EDIT_QOS_GPRS_MEAN_DATA_RATE, CDataStatsApp_BranchDialog},
    { IDL_MENU_VIEW_QOS_GPRS_PEAK_DR,     IDD_EDIT_QOS_GPRS_PEAK_DATA_RATE, CDataStatsApp_BranchDialog},
    
    { IDL_SK_EDIT_APN_OK, NO_DIALOG, CDataStatsApp_SaveAPN},
    { IDL_SK_EDIT_APN_CANCEL, NO_DIALOG, CDataStatsApp_EndDialog},

    { IDL_PDP_SK_SAVE, IDL_PDP_SK_SAVE, CDataStatsApp_SavePDPType},
    { IDL_PDP_SK_CANCEL, IDL_PDP_SK_CANCEL, CDataStatsApp_SavePDPType},

    { IDL_SK_EDIT_IP_OK, NO_DIALOG, CDataStatsApp_SaveIP},
    { IDL_SK_EDIT_IP_CANCEL, NO_DIALOG, CDataStatsApp_EndDialog},

    { IDL_SK_EDIT_SECURITY_OK, NO_DIALOG, CDataStatsApp_SaveSecurity},
    { IDL_SK_EDIT_SECURITY_CANCEL, NO_DIALOG, CDataStatsApp_EndDialog},

    { IDL_SK_EDIT_QOS_OK, NO_DIALOG, CDataStatsApp_SaveQoSType},
    { IDL_SK_EDIT_QOS_CANCEL, NO_DIALOG, CDataStatsApp_EndDialog},

#ifdef FEATURE_DATA_IMS
    { IDL_SK_EDIT_PCSCF_OK, NO_DIALOG, CDataStatsApp_SavePCSCFRequest},
    { IDL_SK_EDIT_PCSCF_CANCEL, NO_DIALOG, CDataStatsApp_EndDialog},
    { IDL_SK_IM_CN_OK, NO_DIALOG, CDataStatsApp_SaveIMCNFlag},
    { IDL_SK_IM_CN_CANCEL, NO_DIALOG, CDataStatsApp_EndDialog},
    { IDL_SK_DHCP_PCSCF_OK, NO_DIALOG, CDataStatsApp_SaveDHCPPCSCFRequest},
    { IDL_SK_DHCP_PCSCF_CANCEL, NO_DIALOG, CDataStatsApp_EndDialog},
#endif

    { IDL_SK_QOS_UMTS_OK, IDD_EDIT_QOS, CDataStatsApp_SaveQoSUMTS},
    { IDL_SK_QOS_UMTS_CANCEL, NO_DIALOG, CDataStatsApp_EndDialog},

    { IDL_SK_QOS_UMTS_TC_OK, NO_DIALOG, CDataStatsApp_SaveTrafficClass},
    { IDL_SK_QOS_UMTS_TC_CANCEL, NO_DIALOG, CDataStatsApp_EndDialog},

    { IDL_SK_QOS_UMTS_MAX_UL_BIT_RATE_OK, NO_DIALOG, CDataStatsApp_SaveQoSUMTSMaxULBR},
    { IDL_SK_QOS_UMTS_MAX_UL_BIT_RATE_CANCEL, NO_DIALOG, CDataStatsApp_EndDialog},

    { IDL_SK_QOS_UMTS_MAX_DL_BR_OK, NO_DIALOG, CDataStatsApp_SaveQoSUMTSMaxDLBR},
    { IDL_SK_QOS_UMTS_MAX_DL_BR_CANCEL, NO_DIALOG, CDataStatsApp_EndDialog},

    { IDL_SK_QOS_UMTS_MIN_UL_BR_OK, NO_DIALOG, CDataStatsApp_SaveQoSUMTSMinULBR},
    { IDL_SK_QOS_UMTS_MIN_UL_BR_CANCEL, NO_DIALOG, CDataStatsApp_EndDialog},

    { IDL_SK_QOS_UMTS_MIN_DL_BR_OK, NO_DIALOG, CDataStatsApp_SaveQoSUMTSMinDLBR},
    { IDL_SK_QOS_UMTS_MIN_DL_BR_CANCEL, NO_DIALOG, CDataStatsApp_EndDialog},

    { IDL_SK_QOS_GPRS_REL_CLASS_OK, NO_DIALOG, CDataStatsApp_SaveGPRSRel},
    { IDL_SK_QOS_GPRS_REL_CLASS_CANCEL, NO_DIALOG, CDataStatsApp_EndDialog},

    { IDL_SK_QOS_GPRS_DC_OK, NO_DIALOG, CDataStatsApp_SaveGPRSDelay},
    { IDL_SK_QOS_GPRS_DC_CANCEL, NO_DIALOG, CDataStatsApp_EndDialog},

    { IDL_SK_QOS_GPRS_PREC_OK, NO_DIALOG, CDataStatsApp_SaveGPRSPrec},
    { IDL_SK_QOS_GPRS_PREC_CANCEL, NO_DIALOG, CDataStatsApp_EndDialog},

    { IDL_SK_QOS_GPRS_MEAN_DR_OK, NO_DIALOG, CDataStatsApp_SaveQoSGPRSMeanDR},
    { IDL_SK_QOS_GPRS_MEAN_DR_CANCEL, NO_DIALOG, CDataStatsApp_EndDialog},

    { IDL_SK_QOS_GPRS_PEAK_DR_OK, NO_DIALOG, CDataStatsApp_SaveQoSGPRSPeakDR},
    { IDL_SK_QOS_GPRS_PEAK_DR_CANCEL, NO_DIALOG, CDataStatsApp_EndDialog},

    { IDL_SK_QOS_GPRS_OK, IDD_EDIT_QOS, CDataStatsApp_SaveQoSGPRS},
    { IDL_SK_QOS_GPRS_CANCEL, NO_DIALOG, CDataStatsApp_EndDialog},

    { IDL_SK_VIEW_PROFILE_SAVE, NO_DIALOG, CDataStatsApp_SaveProfile},
    { IDL_SK_VIEW_PROFILE_CANCEL, NO_DIALOG, CDataStatsApp_EndDialog},
    
    { IDL_SK_ERROR_OK, NO_DIALOG, CDataStatsApp_EndDialog},

    { IDL_SK_STATISTICS_OK, NO_DIALOG, CDataStatsApp_EndDialog},
    
    { IDL_SK_END_CALL_OK, NO_DIALOG, CDataStatsApp_EndCall},
    { IDL_SK_END_CALL_CANCEL, NO_DIALOG, CDataStatsApp_EndDialog},

    { IDL_SK_CHAP_PASSWORD_OK, NO_DIALOG, CDataStatsApp_SaveCHAPPassword},
    { IDL_SK_CHAP_PASSWORD_CANCEL, NO_DIALOG, CDataStatsApp_EndDialog},

    { IDL_SK_PAP_PARAMS_OK, NO_DIALOG, CDataStatsApp_SavePAPParams},
    { IDL_SK_PAP_PARAMS_CANCEL, NO_DIALOG, CDataStatsApp_EndDialog},

    { IDL_DDTM_OK, NO_DIALOG, CDataStatsApp_SaveDDTM},
    { IDL_DDTM_CANCEL, NO_DIALOG, CDataStatsApp_EndDialog},
    { IDL_SK_DONE, NO_DIALOG, CDataStatsApp_SaveProfileName},
    { IDL_SK_CANCEL, NO_DIALOG, CDataStatsApp_EndDialog}
};

/*===========================================================================
FUNCTION CDataStatsApp_CallSuammryDlgHandler

DESCRIPTION
  The Call Summary Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_CallSummaryDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
    CDataStatsApp *pMe = (CDataStatsApp*) pUser;
    IDialog *pIDialog;
    IMenuCtl *pIMenuCtl;
    uint32 data;
    
    if(NULL == pMe)
    {
        DATASTATS_ERR("Null pMe");
        return FALSE;
    }

    switch(evt)
    {
    case EVT_DIALOG_START:
        PRINT_EVENT(EVT_DIALOG_START);
        CDataStatsApp_SetMenuLook(pMe->a.m_pIShell, IDC_MENU_CALL_SUMMARY);
        CDataStatsApp_SetDefaultSoftkeyLook(pMe->a.m_pIShell, IDC_SK_CALL_SUMMARY);
        // Initializing this dialog. So All information is stale.
        pMe->bStaleSummary = TRUE;
        CDataStatsApp_RefreshSummaryMenu((void*) pMe);
        
        return TRUE;

    case EVT_COMMAND:
        
        PRINT_EVENT(EVT_COMMAND);
        
        if(!CDataStatsApp_ExecuteItemAction(pMe, wParam))
        {
            // show the call details for the selected call.
            pIDialog  = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);
            if(pIDialog != NULL)
            {
            
              pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_CALL_SUMMARY);
              
                if(pIMenuCtl != NULL)
                {
                    
                  if(IMENUCTL_GetItemCount(pIMenuCtl) > 0)
                  {
                    if(IMENUCTL_GetSel(pIMenuCtl) < (IDL_SUMMARY_CALL1 + ENDED_CALL_ITEMID_OFFSET)) // Active Calls.
                    {
                      IMENUCTL_GetItemData(pIMenuCtl, IMENUCTL_GetSel(pIMenuCtl), &data);
                      pMe->call_id = (AEECMCallID) data;
                      (void) CDataStatsApp_CreateDialog(pMe,IDD_CALL_DETAIL);
                    }
                  }
                }
                else
                {
                    DATASTATS_ERR("pIMenuCtl NULL");
                }
            }
            else
            {
                DATASTATS_ERR("pIDialog NULL");
            }
            
        }
        return TRUE;

    case EVT_KEY:
        
        switch(wParam)
        {
        case AVK_CLR:
            PRINT_EVENT(AVK_CLR);
            return CDataStatsApp_EndApplet(pMe, NULL);

        case AVK_END:
            PRINT_EVENT(AVK_END);
            //First store the selected call for the "End Selected Option".
            pMe->call_id = 0;
            pIDialog  = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);
            if(pIDialog != NULL)
            {
              pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_CALL_SUMMARY);
                
                if(pIMenuCtl != NULL)
                {
                    IMENUCTL_GetItemData(pIMenuCtl, IMENUCTL_GetSel(pIMenuCtl), &data);
                    pMe->call_id = (AEECMCallID) data;
                }
            }

           if(ICM_GetActiveCallIDs(pMe->m_pICM,
                                   (AEECMCallType)(AEECM_CALL_TYPE_CS_DATA | AEECM_CALL_TYPE_PS_DATA), 
                                   (AEECMCallState)(AEECM_CALL_STATE_CONV|AEECM_CALL_STATE_DORMANT),
                                   NULL, 0))
           {
              return CDataStatsApp_CreateDialog(pMe,IDD_END_ALL);
              
            }
            else
            {
              return CDataStatsApp_EndApplet(pMe, NULL);
            }
   

        default:
          break;

        }
        break;

    case EVT_DIALOG_END:
        PRINT_EVENT(EVT_DIALOG_END);
        return TRUE;

    default:
        break;
    }
    return FALSE;
}

/*===========================================================================
FUNCTION CDataStatsApp_CallDetailDlgHandler

DESCRIPTION
  The Call Detail Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_CallDetailDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{

    CDataStatsApp *pMe = (CDataStatsApp*) pUser;
        
    if(NULL == pMe)
    {
        DATASTATS_ERR("Null pMe");
        return FALSE;
    }
    
    switch(evt)
    {
    case EVT_DIALOG_START:
        
        PRINT_EVENT(EVT_DIALOG_START);
        
        // We do not have any previous count at the start.
        pMe->bHistoryCountSet = FALSE;
        
        // Set all the strings and start the refresh timer.
        CDataStatsApp_InitCallDetailDialog(pMe);

        CDataStatsApp_SetDefaultSoftkeyLook(pMe->a.m_pIShell, IDC_SK_CALL_DETAIL);
        
        return TRUE;

    case EVT_COMMAND:
        PRINT_EVENT(EVT_COMMAND);
        return CDataStatsApp_ExecuteItemAction(pMe, wParam);
       

    case EVT_KEY:
        
        switch(wParam)
        {
        case AVK_CLR:
            PRINT_EVENT(AVK_CLR);
            return CDataStatsApp_EndDialog(pMe, NULL);

        case AVK_END:
            PRINT_EVENT(AVK_END);
            return CDataStatsApp_CreateDialog(pMe,IDD_END_CALL);

        default:
          break;
        }
        break;

    case EVT_DIALOG_END:
        PRINT_EVENT(EVT_DIALOG_END);
        //CDataStatsApp_RefreshSummaryMenu((void*) pMe);
        return TRUE;

    default:
        break;
    }
    return FALSE;
}

/*===========================================================================
FUNCTION CDataStatsApp_CallEndedDlgHandler

DESCRIPTION
  The Call Ended Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_CallEndedDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
    CDataStatsApp *pMe = (CDataStatsApp*) pUser;
    
    if(NULL == pMe)
    {
        DATASTATS_ERR("Null pMe");
        return FALSE;
    }
    switch(evt)
    {
    case EVT_DIALOG_START:
        PRINT_EVENT(EVT_DIALOG_START);
        CDataStatsApp_SetDefaultSoftkeyLook(pMe->a.m_pIShell, IDC_SK_CALL_ENDED);
        CDataStatsApp_InitCallEndedDialog(pMe);
        return TRUE;

    case EVT_COMMAND:
        PRINT_EVENT(EVT_COMMAND);
        return CDataStatsApp_ExecuteItemAction(pMe, wParam);
       

    case EVT_KEY:
        switch(wParam)
        {
        case AVK_CLR:
            PRINT_EVENT(AVK_CLR);
            return CDataStatsApp_EndDialog(pMe, NULL);
        
        default:
          break;

        }
        break;

    case EVT_DIALOG_END:
        PRINT_EVENT(EVT_DIALOG_END);
        CDataStatsApp_RefreshSummaryMenu((void*) pMe);
        return TRUE;

    default:
        break;
    }
    return FALSE;
}

/*===========================================================================
FUNCTION CDataStatsApp_SettingsDlgHandler

DESCRIPTION
  The Data Profile Settings Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_SettingsDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
  CDataStatsApp *pMe = (CDataStatsApp*) pUser;

  if(NULL == pMe)
  {
      DATASTATS_ERR("Null pMe");
      return FALSE;
  }

  switch(evt)
  {
  case EVT_DIALOG_START:
      PRINT_EVENT(EVT_DIALOG_START);
      
      CDataStatsApp_SetMenuLook(pMe->a.m_pIShell, IDC_MENU_SETTINGS);
      CDataStatsApp_SetDefaultSoftkeyLook(pMe->a.m_pIShell, IDC_SK_SETTINGS);
      return StripSettingsMenu(pMe);

  case EVT_COMMAND:
      PRINT_EVENT(EVT_COMMAND);
      return CDataStatsApp_ExecuteItemAction(pMe, wParam);
      

  case EVT_KEY:
      
      switch(wParam)
      {
      case AVK_CLR:
          PRINT_EVENT(AVK_CLR);
          return CDataStatsApp_EndDialog(pMe, NULL);
      
      default:
        break;
      }
      break;

  case EVT_DIALOG_END:
      PRINT_EVENT(EVT_DIALOG_END);
      CDataStatsApp_RefreshSummaryMenu((void*) pMe);
      return TRUE;

  default:
      break;
  }
  return FALSE;
}
/*===========================================================================
FUNCTION CDataStatsApp_InitStatsDlg

DESCRIPTION
  The Data Statistics Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int CDataStatsApp_InitStatsDlg(CDataStatsApp *pMe)
{
  IDialog *pIDialog = NULL;
  char *count = NULL;
  AECHAR *Count = NULL;
  IStatic *pIStaticCtl = NULL;
  AECHAR *szBuf = NULL;
    
  count = (char*) MALLOC(DATASTATS_MAX_BUFFER_SIZE*sizeof(char));
  Count = (AECHAR*) MALLOC(DATASTATS_MAX_BUFFER_SIZE*sizeof(AECHAR));
  szBuf = (AECHAR*) MALLOC(DATASTATS_MAX_BUFFER_SIZE*sizeof(AECHAR));
      
  if(count == NULL || Count == NULL || szBuf == NULL)
  {
    DATASTATS_ERR("Malloc Failed");
    FREEIF(count);
    FREEIF(Count);
    FREEIF(szBuf);
    return FALSE;
  }

  pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      
  if(pIDialog != NULL)
  {
    pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_STATISTICS);
    

    if(pIStaticCtl != NULL)
    {
     ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_STATISTICS,
              szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
     ISTATIC_SetText(pIStaticCtl, szBuf, NULL, AEE_FONT_BOLD, AEE_FONT_BOLD);
     ISTATIC_Redraw(pIStaticCtl);
    }
    else
    {
      DATASTATS_ERR("pIStaticCtl NULL");
    }

    pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_STATISTICS_LAST_CALL);
    

    if(pIStaticCtl != NULL)
    {
     ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_LAST_CALL,
              szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
     ISTATIC_SetText(pIStaticCtl, NULL, szBuf,AEE_FONT_BOLD, AEE_FONT_BOLD);
     ISTATIC_Redraw(pIStaticCtl);
    }
    else
    {
      DATASTATS_ERR("pIStaticCtl NULL");
    }

    pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_STATISTICS_LASTTX);
    CDataStatsApp_GetLastDataStats(pMe); 

    if(pIStaticCtl != NULL)
    {
     ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_BYTES_TX,
              szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

     (void) SNPRINTF(count, DATASTATS_MAX_BUFFER_SIZE, "%4d", pMe->m_dwTxLastCount);
     (void) STRTOWSTR(count,Count, DATASTATS_MAX_WIDE_BUFFER_SIZE);
     (void) WSTRLCAT(szBuf, Count, DATASTATS_MAX_WIDE_BUFFER_SIZE); 

     ISTATIC_SetText(pIStaticCtl, NULL, Count, AEE_FONT_NORMAL, AEE_FONT_NORMAL);
     ISTATIC_Redraw(pIStaticCtl);
    }
    else
    {
      DATASTATS_ERR("pIStaticCtl NULL");
    }
        
    pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_STATISTICS_LASTRX);
    if(pIStaticCtl != NULL)
    {
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_BYTES_RX,
              szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

      (void) SNPRINTF(count, DATASTATS_MAX_BUFFER_SIZE, "%4d", pMe->m_dwRxLastCount);
      (void) STRTOWSTR(count,Count, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      (void) WSTRLCAT(szBuf, Count, DATASTATS_MAX_WIDE_BUFFER_SIZE); 
      ISTATIC_SetText(pIStaticCtl, NULL, Count, AEE_FONT_NORMAL, AEE_FONT_NORMAL);
      ISTATIC_Redraw(pIStaticCtl);
    }
    else
    {
      DATASTATS_ERR("pIStaticCtl NULL");
    }

    pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_STATISTICS_TOTAL);
    

    if(pIStaticCtl != NULL)
    {
     ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_TOTAL,
              szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
     ISTATIC_SetText(pIStaticCtl, NULL, szBuf,AEE_FONT_BOLD, AEE_FONT_BOLD);
     ISTATIC_Redraw(pIStaticCtl);
    }
    else
    {
      DATASTATS_ERR("pIStaticCtl NULL");
    }


      pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_STATISTICS_TOTALTX);
    if(pIStaticCtl != NULL)
    {
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_BYTES_TX,
              szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

      (void) SNPRINTF(count, DATASTATS_MAX_BUFFER_SIZE,"%4d", pMe->m_dwTxTotalBytes);
      (void) STRTOWSTR(count,Count, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      (void) WSTRLCAT(szBuf, Count, DATASTATS_MAX_WIDE_BUFFER_SIZE); 
      ISTATIC_SetText(pIStaticCtl, NULL, Count, AEE_FONT_NORMAL, AEE_FONT_NORMAL);
      
      ISTATIC_Redraw(pIStaticCtl);
    }
    else
    {
      DATASTATS_ERR("pIStaticCtl NULL");
    }
    pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_STATISTICS_TOTALRX);
    if(pIStaticCtl != NULL)
    {
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_BYTES_RX,
              szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

      (void) SNPRINTF(count, DATASTATS_MAX_BUFFER_SIZE, "%4d", pMe->m_dwRxTotalBytes);
      (void) STRTOWSTR(count,Count, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      (void) WSTRLCAT(szBuf, Count, DATASTATS_MAX_WIDE_BUFFER_SIZE); 
      ISTATIC_SetText(pIStaticCtl, NULL, Count, AEE_FONT_NORMAL, AEE_FONT_NORMAL);
      
      ISTATIC_Redraw(pIStaticCtl);
    }
    else
    {
      DATASTATS_ERR("pIStaticCtl NULL");
    }
  }
  else
  {
    DATASTATS_ERR("pIDialog NULL");
  }
  
  CDataStatsApp_SetDefaultSoftkeyLook(pMe->a.m_pIShell, IDC_SK_STATISTICS);
  FREE(count);
  FREE(Count);
  FREE(szBuf);
  return TRUE;
}
/*===========================================================================
FUNCTION CDataStatsApp_StatisticsDlgHandler

DESCRIPTION
  The Data Statistics Dialog Event Handler 

DEPENDENCIES
  None
 
SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_StatisticsDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
  CDataStatsApp *pMe = (CDataStatsApp*) pUser;
        
  if(NULL == pMe)
  {
      DATASTATS_ERR("Null pMe");
      return FALSE;
  }
  
  switch(evt)
  {
  case EVT_DIALOG_START:

      PRINT_EVENT(EVT_DIALOG_START);
      return (CDataStatsApp_InitStatsDlg(pMe));
  
  case EVT_COMMAND:
      PRINT_EVENT(EVT_COMMAND);
      if(wParam == IDL_SK_STATISTICS_RESET)
      {
        CDataStatsApp_ResetLastDataStats(pMe);
        return (CDataStatsApp_InitStatsDlg(pMe));
      }
      return CDataStatsApp_ExecuteItemAction(pMe, wParam);
      

  case EVT_KEY:
      
      switch(wParam)
      {
      case AVK_CLR:
          PRINT_EVENT(AVK_CLR);
          return CDataStatsApp_EndDialog(pMe, NULL);
      
      default:
        break;
      }
      break;

  case EVT_DIALOG_END:
      PRINT_EVENT(EVT_DIALOG_END);
      return TRUE;

  default:
      break;
  }
  return FALSE;
}
/*===========================================================================
FUNCTION CDataStatsApp_SelectActiveProfileDlgHandler

DESCRIPTION
  Select Active Profile Dialog Event Handler

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_SelectActiveProfileDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  CDataStatsApp *pMe = (CDataStatsApp*) pUser;
  IDialog *pIDialog;
      
  if(NULL == pMe)
  {
      DATASTATS_ERR("Null pMe");
      return FALSE;
  }  
  switch(evt)
  {
  case EVT_DIALOG_START:
      PRINT_EVENT(EVT_DIALOG_START);
      CDataStatsApp_InitActiveProfile(pMe, IDC_MENU_SELECT_ACTIVE);
      CDataStatsApp_SetMenuLook(pMe->a.m_pIShell, IDC_MENU_SELECT_ACTIVE);
      CDataStatsApp_SetDefaultSoftkeyLook(pMe->a.m_pIShell, IDC_SK_SELECT_ACTIVE);
      return TRUE;

  case EVT_COMMAND:
      PRINT_EVENT(EVT_COMMAND);
      return CDataStatsApp_ExecuteItemAction(pMe, wParam);
      

  case EVT_KEY:
      
      switch(wParam)
      {
      case AVK_CLR:
          PRINT_EVENT(AVK_CLR);
          return CDataStatsApp_EndDialog(pMe, NULL);

      default:
        break;
      }
      break;

  case EVT_CTL_SEL_CHANGED:
      
      pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      
      if(pIDialog != NULL)
      {
      if (((IControl*)dwParam) == IDIALOG_GetControl(pIDialog, IDC_MENU_SELECT_ACTIVE))
      {
          InitMenuIcons((IMenuCtl*) dwParam);
          SetMenuIcon((IMenuCtl*) dwParam, wParam, TRUE);
      }
      }
      else
      {
          DATASTATS_ERR("pIDialog NULL");
      }
      break;

  case EVT_DIALOG_END:
      PRINT_EVENT(EVT_DIALOG_END);
      return TRUE;

  default:
      break;
  }
#endif //(FEATURE_WCDMA) || (FEATURE_GSM_GPRS)
  return FALSE;
}

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
/*===========================================================================
FUNCTION CDataStatsApp_GetProfileName

DESCRIPTION
  Get the Profile Name

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

static boolean CDataStatsApp_GetProfileName(CDataStatsApp *pMe)
{
  IDialog* pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  IMenuCtl* pIMenu;
  uint16 nItemCount;
  int i;
  AEEPDP pdpProfile;
  uint16 nItemID, dwFields;
  AECHAR *szBuf = (AECHAR*)MALLOC(2*DATASTATS_MAX_WIDE_BUFFER_SIZE);
  uint32 maxProfiles;
 
  if(pIDialog == NULL)
  {
    FREEIF(szBuf);
    return FALSE;
  }

  pIMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog , IDC_MENU_EDIT_PROFILE);

  if(pIMenu == NULL || szBuf == NULL)
  {
    FREEIF(szBuf);
    return FALSE;
  }

  nItemCount = IMENUCTL_GetItemCount(pIMenu);
  
  dwFields = AEEPDP_PROFILE_NAME;

  i = nItemCount;

  maxProfiles = IPDP_GetNumProfiles(pMe->m_pIPDP);
  
  while(i > maxProfiles)
  {
    nItemID = IMENUCTL_GetItemID(pIMenu, i-1);
    IMENUCTL_DeleteItem(pIMenu,nItemID);
    --i;
  }

  nItemCount = i;

  // set up name buffer
  pdpProfile.profile_name_len = (IPDP_GetMaxProfileNameLen(pMe->m_pIPDP) + 1) * sizeof(AECHAR);
  pdpProfile.profile_name = (AECHAR*) MALLOC(pdpProfile.profile_name_len);
  if (NULL == pdpProfile.profile_name)
  {
    FREEIF(szBuf);
    return FALSE;
  }

  //Call End event
  for(i = 0; i < nItemCount; i++)
  {
    int nReturnStatus;

    nItemID = IMENUCTL_GetItemID(pIMenu, i);
    nReturnStatus = IPDP_GetParm(pMe->m_pIPDP, i+1, dwFields, &pdpProfile);
  
    if(nReturnStatus == SUCCESS)
    {
      IMENUCTL_SetItemText(pIMenu, nItemID, DATASTATSAPP_RES_FILE, 0,pdpProfile.profile_name);
    }
    else
    {
      
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_PROFILE1 + i,
              szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

      IMENUCTL_SetItemText(pIMenu, nItemID, DATASTATSAPP_RES_FILE, 0,szBuf);
    }
    IMENUCTL_Redraw(pIMenu);
  }
  CDataStatsApp_SetMenuLook(pMe->a.m_pIShell, IDC_MENU_EDIT_PROFILE);
  FREEIF(szBuf);
  FREE(pdpProfile.profile_name);
  return TRUE;
}
#endif /* (FEATURE_WCDMA) || (FEATURE_GSM_GPRS) */

/*===========================================================================
FUNCTION CDataStatsApp_EditProfileDlgHandler

DESCRIPTION
  The Edit Profile Event Handler:Select the profile to edit from the Menu

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditProfileDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  CDataStatsApp *pMe = (CDataStatsApp*) pUser;
      
  if(NULL == pMe)
  {
      DATASTATS_ERR("Null pMe");
      return FALSE;
  }   
  
  switch(evt)
  {
  case EVT_DIALOG_START:
      PRINT_EVENT(EVT_DIALOG_START);
      return (CDataStatsApp_GetProfileName(pMe));
      
 
  case EVT_COMMAND:
      PRINT_EVENT(EVT_COMMAND);
      return CDataStatsApp_ExecuteItemAction(pMe, wParam);
      

  case EVT_KEY:
      PRINT_EVENT(EVT_KEY);
      switch(wParam)
      {
      case AVK_CLR:
          PRINT_EVENT(AVK_CLR);
          return CDataStatsApp_EndDialog(pMe, NULL);

      default:
        break;
      }
      break;

  case EVT_DIALOG_END:
      PRINT_EVENT(EVT_DIALOG_END);
      return TRUE;

  case EVT_USER:
    switch(wParam)
    {
  case EVT_DATASTATS_DLG_RESUME:
    {
      uint16 dwFields = AEEPDP_PROFILE_NAME;
      IDialog* pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      IMenuCtl* pIMenu;
      AEEPDP pdpProfile;
      int nReturnStatus;
     
      if(pIDialog == NULL)
      {
        return FALSE;
      }

      pIMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog , IDC_MENU_EDIT_PROFILE);

      if(pIMenu == NULL)
      {
        return FALSE;
      }
      
        pdpProfile.profile_name_len = (IPDP_GetMaxProfileNameLen(pMe->m_pIPDP) + 1) * sizeof(AECHAR);
        pdpProfile.profile_name = (AECHAR*) MALLOC(pdpProfile.profile_name_len);
        if (NULL == pdpProfile.profile_name) {
          return FALSE;
        }
      nReturnStatus = IPDP_GetParm(pMe->m_pIPDP,pMe->selectedProfile, dwFields, &pdpProfile);
      if(nReturnStatus == SUCCESS)
      {
        IMENUCTL_SetItemText(pIMenu, IMENUCTL_GetSel(pIMenu), DATASTATSAPP_RES_FILE, 0,pdpProfile.profile_name);
      }
      IMENUCTL_Redraw(pIMenu);

        FREE(pdpProfile.profile_name);

        return TRUE;
    }
      default:
        return FALSE;
    } 

  default:
      break;
  }
#endif //(FEATURE_WCDMA) || (FEATURE_GSM_GPRS)
  return FALSE;
}
/*===========================================================================
FUNCTION CDataStatsApp_ViewProfileDlgHandler

DESCRIPTION
  The View profile Dialog Event Handler

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_ViewProfileDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  CDataStatsApp *pMe = (CDataStatsApp*) pUser;
  IDialog *pIDialog ;
  IMenuCtl *pIMenuCtl = NULL;
  AECHAR *szBuf;
  AECHAR *ipAddress;
#ifdef FEATURE_DATA_IMS
  AECHAR *pcscf_addr_req;
  AECHAR *dhcp_pcscf_addr;
  AECHAR *im_cn_flag;  
#endif
    
  if(NULL == pMe)
  {
      DATASTATS_ERR("Null pMe");
      return FALSE;
  }
    
  switch(evt)
  {
  case EVT_DIALOG_START:
    PRINT_EVENT(EVT_DIALOG_START);
    pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    if(pIDialog == NULL)
    {
      DATASTATS_ERR("pIDialog NULL");
      return FALSE;
    }
    pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_VIEW_PROFILE);
    if(pIMenuCtl == NULL)
    {
      DATASTATS_ERR("pIMenuCtl NULL");
      return FALSE;
    }
    szBuf = (AECHAR*) MALLOC(2*DATASTATS_MAX_WIDE_BUFFER_SIZE); // Requires extra buffer.
    ipAddress = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
#ifdef FEATURE_DATA_IMS
    pcscf_addr_req = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
    dhcp_pcscf_addr = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);    
    im_cn_flag = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);    

    if (szBuf == NULL || ipAddress == NULL || dhcp_pcscf_addr == NULL || im_cn_flag == NULL || pcscf_addr_req == NULL) 
    {
      FREEIF(im_cn_flag);
      FREEIF(dhcp_pcscf_addr);
      FREEIF(pcscf_addr_req);
#else
    if (szBuf == NULL || ipAddress == NULL) 
    {
#endif //FEATURE_DATA_IMS
    
    
      DATASTATS_ERR("Malloc Failed");
      FREEIF(szBuf);
      FREEIF(ipAddress);
      return FALSE;
    }

    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_EDIT_PROFILE,
      szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

    IMENUCTL_SetTitle( pIMenuCtl, DATASTATSAPP_RES_FILE, 0, szBuf);
      
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_NAME, 
      szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    (void) WSTRLCAT(szBuf, pMe->profileText,2*DATASTATS_MAX_WIDE_BUFFER_SIZE); 

    IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_PROFILE_NAME, DATASTATSAPP_RES_FILE, 0, szBuf);
      
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_APN, 
      szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
#ifndef _WIN32 // Does not work on LTK len(pMe->apn) itself is 32 ....have some better solution later.
    (void) WSTRLCAT(szBuf, pMe->apn,2*DATASTATS_MAX_WIDE_BUFFER_SIZE);
#endif
    IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_APN, DATASTATSAPP_RES_FILE, 0, szBuf);
      
    if(pMe->m_pdpType == AEEPDP_IPV6)
    {
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_PDP_TYPE, 
        szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_PDP_IPV6, 
       ipAddress, DATASTATS_MAX_WIDE_BUFFER_SIZE);

      (void) WSTRLCAT(szBuf, ipAddress,2*DATASTATS_MAX_WIDE_BUFFER_SIZE); 

      IPV6ToAECHAR(pMe->nIpv6, ipAddress);
      IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_PDP_TYPE, DATASTATSAPP_RES_FILE, 0, szBuf);
      IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_IP, DATASTATSAPP_RES_FILE, 0, ipAddress);

      IMENUCTL_DeleteItem(pIMenuCtl, IDL_MENU_VIEW_DNS_PRIMARY);
      IMENUCTL_DeleteItem(pIMenuCtl, IDL_MENU_VIEW_DNS_SECONDARY);
    }
    else 
    {
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_PDP_TYPE, 
        szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

      if(pMe->m_pdpType == AEEPDP_IP){
        ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_IPV4, 
          ipAddress, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      }
      else
      {
        ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_PDP_ANY, 
          ipAddress, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      }
      (void) WSTRLCAT(szBuf, ipAddress,2*DATASTATS_MAX_WIDE_BUFFER_SIZE); 

      IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_PDP_TYPE, DATASTATSAPP_RES_FILE, 0, szBuf);

      IPToAECHAR(pMe->nIp, ipAddress);
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_IP, 
      szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
	  (void) WSTRLCAT(szBuf, ipAddress, 2*DATASTATS_MAX_WIDE_BUFFER_SIZE);
      (void) WSTRLCPY(ipAddress, szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_IP, DATASTATSAPP_RES_FILE, 0, ipAddress);
              
      IPToAECHAR(pMe->nDnsPrimary.pdp_addr.pdp_addr_ipv4, ipAddress);
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_DNS_PRIMARY, 
        szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
	  (void) WSTRLCAT(szBuf, ipAddress, 2*DATASTATS_MAX_WIDE_BUFFER_SIZE);
      (void) WSTRLCPY(ipAddress, szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_DNS_PRIMARY, DATASTATSAPP_RES_FILE, 0, ipAddress);
      
      IPToAECHAR(pMe->nDnsSecondary.pdp_addr.pdp_addr_ipv4, ipAddress);
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_DNS_SECONDARY,
        szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
	  (void) WSTRLCAT(szBuf, ipAddress, 2*DATASTATS_MAX_WIDE_BUFFER_SIZE);
      (void) WSTRLCPY(ipAddress, szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_DNS_SECONDARY, DATASTATSAPP_RES_FILE, 0, ipAddress);
    }         
    //Use ipAdress for the Security String.
    switch(pMe->nSecurity)
    {
    case AEEPDP_AUTH_CHAP:
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_CHAP, 
        ipAddress, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    break;

    case AEEPDP_AUTH_PAP:
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_PAP, 
      ipAddress, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    break;

    default: //AEEPDP_AUTH_NONE currently treated as none.
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_NONE, 
      ipAddress, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    break;

    }
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_SECURITY, 
      szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    (void) WSTRLCAT(szBuf, ipAddress, 2*DATASTATS_MAX_WIDE_BUFFER_SIZE);
      
    IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_SECURITY, DATASTATSAPP_RES_FILE, 0, szBuf);

    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS, 
        szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

    switch(pMe->nQoSType)
    {
    case AEEPDP_QOS_UMTS_REQ:
    case AEEPDP_QOS_UMTS_MIN:
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_UMTS, 
        ipAddress, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    break;

    case AEEPDP_QOS_GPRS_REQ:
    case AEEPDP_QOS_GPRS_MIN:
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS, 
        ipAddress, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    break;

    default: 
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_NONE, 
        ipAddress, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    break;
              
    }
    (void) WSTRLCAT(szBuf, ipAddress, 2*DATASTATS_MAX_WIDE_BUFFER_SIZE);

    IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_QOS, DATASTATSAPP_RES_FILE, 0, szBuf);

#ifdef FEATURE_DATA_IMS 
    //View PCSCF Address
    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_REQUEST_PCSCF,
                         szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
     switch(pMe->m_pcscf_addr)
     {
       case TRUE:
         (void) ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_PCSCF_TRUE,
                                             pcscf_addr_req, DATASTATS_MAX_WIDE_BUFFER_SIZE);
         break;

       case FALSE:              
       default :
         (void) ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_PCSCF_FALSE,
                                             pcscf_addr_req, DATASTATS_MAX_WIDE_BUFFER_SIZE);
         break;
     }
     (void) WSTRLCAT(szBuf, pcscf_addr_req, 2*DATASTATS_MAX_WIDE_BUFFER_SIZE);

     IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_PCSCF_ADDR, DATASTATSAPP_RES_FILE, 0, szBuf);

    //View DHCP PCSCF Address flag
    (void) ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_REQUEST_DHCP_PCSCF,
                         szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
     switch(pMe->m_dhcp_pcscf_addr)
     {
       case TRUE:
         (void) ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_PCSCF_TRUE,
                                             dhcp_pcscf_addr, DATASTATS_MAX_WIDE_BUFFER_SIZE);
         break;

       case FALSE:              
       default :
         (void) ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_PCSCF_FALSE,
                                             dhcp_pcscf_addr, DATASTATS_MAX_WIDE_BUFFER_SIZE);
         break;
     }
     (void) WSTRLCAT(szBuf, dhcp_pcscf_addr, 2*DATASTATS_MAX_WIDE_BUFFER_SIZE);

     IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_DHCP_PCSCF_FLAG, DATASTATSAPP_RES_FILE, 0, szBuf);

     // View the im_cn flag
     (void) ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_IM_CN_FLAG,
                         szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
     switch(pMe->m_im_cn_flag)
     {
       case TRUE:
         (void) ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_PCSCF_TRUE,
                                             im_cn_flag, DATASTATS_MAX_WIDE_BUFFER_SIZE);
         break;

       case FALSE:              
       default :
         (void) ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_PCSCF_FALSE,
                                             im_cn_flag, DATASTATS_MAX_WIDE_BUFFER_SIZE);
         break;
     }
     (void) WSTRLCAT(szBuf, im_cn_flag, 2*DATASTATS_MAX_WIDE_BUFFER_SIZE);

     IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_IMCN_FLAG, DATASTATSAPP_RES_FILE, 0, szBuf);
#endif //FEATURE_DATA_IMS

    CDataStatsApp_SetMenuLook(pMe->a.m_pIShell, IDC_MENU_VIEW_PROFILE);
    IMENUCTL_Redraw(pIMenuCtl);

    FREE(szBuf);
    FREE(ipAddress);

    CDataStatsApp_SetDefaultSoftkeyLook(pMe->a.m_pIShell, IDC_SK_VIEW_PROFILE);
  return TRUE;

  case EVT_COMMAND:
    PRINT_EVENT(EVT_COMMAND);
  return CDataStatsApp_ExecuteItemAction(pMe, wParam);
        
  case EVT_KEY:

    switch(wParam)
    {
    case AVK_CLR:
      PRINT_EVENT(AVK_CLR);
    return CDataStatsApp_EndDialog(pMe, NULL);

    default:
    break;
    }
  break;

  case EVT_DIALOG_END:
    PRINT_EVENT(EVT_DIALOG_END);
  return TRUE;

  default:
  break;
  }
#endif //(FEATURE_WCDMA) || (FEATURE_GSM_GPRS)
  return FALSE;
}
/*===========================================================================
FUNCTION CDataStatsApp_EditAPNDlgHandler

DESCRIPTION
  The Edit APN Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditAPNDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  CDataStatsApp *pMe = (CDataStatsApp*) pUser;
  IDialog *pIDialog;
  IStatic *pIStaticCtl = NULL;
  IMenuCtl *pIMenuCtl = NULL;
  ITextCtl *pITextCtl = NULL;

  if(NULL == pMe)
  {
      DATASTATS_ERR("Null pMe");
      return FALSE;
  }

  switch(evt)
  {
  case EVT_DIALOG_START:
      PRINT_EVENT(EVT_DIALOG_START);
      pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      
      if(pIDialog != NULL)
      {
  
        pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_EDIT_APN_TITLE);
        if(pIStaticCtl != NULL)
        {
            ISTATIC_SetText(pIStaticCtl, NULL, pMe->profileText, AEE_FONT_NORMAL, AEE_FONT_BOLD);
        }
        else
        {
            DATASTATS_ERR("pIStaticCtl NULL");
        }
        
        pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, IDC_TEXT_EDIT_APN);
        if(pITextCtl != NULL)
        {
            ITEXTCTL_SetProperties(pITextCtl, TP_FRAME);
            ITEXTCTL_SetText(pITextCtl, pMe->apn, -1);
            ITEXTCTL_Redraw(pITextCtl);
        }
        else
        {
            DATASTATS_ERR("pITextCtl NULL");
        }

        pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_SK_EDIT_APN);
        if(pIMenuCtl != NULL)
        {
            IMENUCTL_DeleteAll(pIMenuCtl);
            IMENUCTL_AddItem(pIMenuCtl, DATASTATSAPP_RES_FILE, IDS_DD_OK, IDL_SK_EDIT_APN_OK, NULL, 0);
            IMENUCTL_AddItem(pIMenuCtl, DATASTATSAPP_RES_FILE, IDS_DD_CANCEL, IDL_SK_EDIT_APN_CANCEL, NULL, 0);
        }
        else
        {
            DATASTATS_ERR("pIMenuCtl NULL");
        }
        CDataStatsApp_SetDefaultSoftkeyLook(pMe->a.m_pIShell, IDC_SK_EDIT_APN);
      }
      else
      {
        DATASTATS_ERR("pIDialog NULL");
      }
      return TRUE;

  case EVT_COMMAND:
      PRINT_EVENT(EVT_COMMAND);
      return CDataStatsApp_ExecuteItemAction(pMe, wParam);
      

  case EVT_KEY:
      switch(wParam)
      {
      case AVK_CLR:  
          PRINT_EVENT(AVK_CLR);
          return CDataStatsApp_EndDialog(pMe, NULL);

      default:
        break;
      }
      break;

  case EVT_DIALOG_END:
      PRINT_EVENT(EVT_DIALOG_END);
      return TRUE;

  default:
      break;
  }
#endif //(FEATURE_WCDMA) || (FEATURE_GSM_GPRS)
  return FALSE;
}
/*===========================================================================
FUNCTION CDataStatsApp_EditIPDlgHandler
DESCRIPTION
  The Edit IP Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditIPDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  CDataStatsApp *pMe = (CDataStatsApp*) pUser;
  IDialog *pIDialog;
  IStatic *pIStaticCtl = NULL;
  IMenuCtl *pIMenuCtl = NULL;
  ITextCtl *pITextCtl = NULL;
  AECharIP tempIp;
  AECHAR *editType; 

  if(NULL == pMe)
  {
      DATASTATS_ERR("Null pMe");
      return FALSE;
  }

  //Tab controller stuff..
  switch(evt)
  {
    case EVT_DIALOG_START:
      pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if(pIDialog != NULL)
      {
        TabController_Init(&pMe->pTabController,5);
        TabController_AddControl(pMe->pTabController, TEXT_CTL, IDIALOG_GetControl(pIDialog, IDC_TEXT_EDIT_IP1));
        TabController_AddControl(pMe->pTabController, TEXT_CTL, IDIALOG_GetControl(pIDialog, IDC_TEXT_EDIT_IP2));
        TabController_AddControl(pMe->pTabController, TEXT_CTL, IDIALOG_GetControl(pIDialog, IDC_TEXT_EDIT_IP3));
        TabController_AddControl(pMe->pTabController, TEXT_CTL, IDIALOG_GetControl(pIDialog, IDC_TEXT_EDIT_IP4));
        TabController_AddControl(pMe->pTabController, SK_CTL, IDIALOG_GetControl(pIDialog, IDC_SK_EDIT_IP));
        TabController_SetActiveEx(pMe->pTabController, 0);
      }
      else
      {
        DATASTATS_ERR("pIDialog NULL");        
      }
      break;

    default:
      TabController_HandleEvent(&pMe->pTabController, evt, wParam);
      break;

  }

  switch(evt)
  {
  case EVT_DIALOG_START:
      PRINT_EVENT(EVT_DIALOG_START);
      pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      
      if(pIDialog != NULL)
      {
        editType = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);  
        if(editType == NULL)
        {
          DATASTATS_ERR("Malloc Falied");
          return FALSE;
        }
        switch(pMe->ipTypeSelected)
        {
        case IP_ADDRESS:
            ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_EDITIP, 
              editType, DATASTATS_MAX_WIDE_BUFFER_SIZE);
            IPToAECharIP(pMe->nIp, &tempIp);
            break;
        case PRIMARY_DNS:
            ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_EDITDNS_PRIMARY, 
              editType, DATASTATS_MAX_WIDE_BUFFER_SIZE);
            IPToAECharIP(pMe->nDnsPrimary.pdp_addr.pdp_addr_ipv4, &tempIp);
            break;
        case SECONDARY_DNS:
            ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_EDITDNS_SECONDARY, 
              editType, DATASTATS_MAX_WIDE_BUFFER_SIZE);
            IPToAECharIP(pMe->nDnsSecondary.pdp_addr.pdp_addr_ipv4, &tempIp);
            break;
        default:
            DATASTATS_ERR("Unknown IP type");
            return FALSE;
        }

        pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_EDIT_IP_TITLE);
        if(pIStaticCtl != NULL)
        {
            ISTATIC_SetText(pIStaticCtl, NULL, pMe->profileText, AEE_FONT_NORMAL, AEE_FONT_BOLD);
        }
        else
        {
            DATASTATS_ERR("pIStaticCtl NULL");
        }
            
    
        pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_EDIT_IP);
    
        if(pIStaticCtl != NULL)
        {
            ISTATIC_SetText(pIStaticCtl, NULL, editType, AEE_FONT_NORMAL, AEE_FONT_NORMAL);
        }
        else
        {
            DATASTATS_ERR("pIStaticCtl NULL");
        }
        FREE(editType);
        
        pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_SK_EDIT_IP);
        if(pIMenuCtl != NULL)
        {
            IMENUCTL_DeleteAll(pIMenuCtl);
            IMENUCTL_AddItem(pIMenuCtl, DATASTATSAPP_RES_FILE, IDS_DD_OK, IDL_SK_EDIT_IP_OK, NULL, 0);
            IMENUCTL_AddItem(pIMenuCtl, DATASTATSAPP_RES_FILE, IDS_DD_CANCEL, IDL_SK_EDIT_IP_CANCEL, NULL, 0);
        }
        else
        {
            DATASTATS_ERR("pIMenuCtl NULL");
        }
  
        // Add frame around the text control.
        pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, IDC_TEXT_EDIT_IP1);
          
        if(pITextCtl != NULL)
        {
            ITEXTCTL_SetProperties(pITextCtl, TP_FRAME | TP_FOCUS_NOSEL);
            ITEXTCTL_SetInputMode(pITextCtl, AEE_TM_NUMBERS);
            ITEXTCTL_SetMaxSize(pITextCtl, 3);
            ITEXTCTL_SetText(pITextCtl, tempIp.IP1, -1);
            ITEXTCTL_Redraw(pITextCtl);
        }
        else
        {
            DATASTATS_ERR("pITextCtl NULL");
        }

        pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, IDC_TEXT_EDIT_IP2);
        if(pITextCtl != NULL)
        {
            ITEXTCTL_SetProperties(pITextCtl, TP_FRAME | TP_FOCUS_NOSEL);
            ITEXTCTL_SetInputMode(pITextCtl, AEE_TM_NUMBERS);
            ITEXTCTL_SetMaxSize(pITextCtl, 3);
            ITEXTCTL_SetText(pITextCtl, tempIp.IP2, -1);
            ITEXTCTL_Redraw(pITextCtl);
        }
        else
        {
            DATASTATS_ERR("pITextCtl NULL");
        }

        pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, IDC_TEXT_EDIT_IP3);
        if(pITextCtl != NULL)
        {
            ITEXTCTL_SetProperties(pITextCtl, TP_FRAME | TP_FOCUS_NOSEL);
            ITEXTCTL_SetInputMode(pITextCtl, AEE_TM_NUMBERS);
            ITEXTCTL_SetText(pITextCtl, tempIp.IP3, -1);
            ITEXTCTL_SetMaxSize(pITextCtl, 3);
            ITEXTCTL_Redraw(pITextCtl);
        }
        else
        {
            DATASTATS_ERR("pITextCtl NULL");
        }

        pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, IDC_TEXT_EDIT_IP4);
        if(pITextCtl != NULL)
        {
            ITEXTCTL_SetProperties(pITextCtl, TP_FRAME | TP_FOCUS_NOSEL);
            ITEXTCTL_SetInputMode(pITextCtl, AEE_TM_NUMBERS);
            ITEXTCTL_SetText(pITextCtl, tempIp.IP4, -1);
            ITEXTCTL_Redraw(pITextCtl);
        }
        else
        {
            DATASTATS_ERR("pITextCtl NULL");
        }
        CDataStatsApp_SetDefaultSoftkeyLook(pMe->a.m_pIShell, IDC_SK_EDIT_IP);
      }
      else
      {
          DATASTATS_ERR("pIDialog NULL");
      }

      return TRUE;

  case EVT_COMMAND:
      PRINT_EVENT(EVT_COMMAND);
      return CDataStatsApp_ExecuteItemAction(pMe, wParam);
      

  case EVT_KEY:
      
      switch(wParam)
      {
      case AVK_CLR:
          PRINT_EVENT(AVK_CLR);
          return CDataStatsApp_EndDialog(pMe, NULL);

      default:
        break;
      }
      break;

  case EVT_DIALOG_END:
      PRINT_EVENT(EVT_DIALOG_END);
      return TRUE;

  default:
      break;
  }
#endif
  return FALSE;
}
/*===========================================================================
FUNCTION CDataStatsApp_EditSecurityDlgHandler

DESCRIPTION
  The Edit Security Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditSecurityDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  CDataStatsApp *pMe = (CDataStatsApp*) pUser;
  IDialog *pIDialog;
  IMenuCtl *pIMenuCtl = NULL;
  
  if(NULL == pMe)
  {
      DATASTATS_ERR("Null pMe");
      return FALSE;
  }
  switch(evt)
  {
  case EVT_DIALOG_START:
      PRINT_EVENT(EVT_DIALOG_START);
      pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);

      if(pIDialog != NULL)
      {
      
        pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_EDIT_SECURITY);
        if(pIMenuCtl != NULL)
        {
            InitMenuIcons(pIMenuCtl);
            switch(pMe->nSecurity)
            {
              case AEEPDP_AUTH_CHAP:
                SetMenuIcon(pIMenuCtl, IDL_SECURITY_CHAP, TRUE);
                break;

              case AEEPDP_AUTH_PAP:
                SetMenuIcon(pIMenuCtl, IDL_SECURITY_PAP, TRUE);
                break;
              
              default :// included case AEEPDP_AUTH_NONE:
                SetMenuIcon(pIMenuCtl, IDL_SECURITY_NONE, TRUE);
                break;
            }
        }
        else
        {
            DATASTATS_ERR("pIMenuCtl NULL");    
            return FALSE;
        }
        //IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_BACKGROUND,MAKE_RGB(255, 255, 204));
        //IDISPLAY_Update(pMe->a.m_pIDisplay);
        IMENUCTL_SetTitle(pIMenuCtl, DATASTATSAPP_RES_FILE, 0, pMe->profileText);
        CDataStatsApp_SetMenuLook(pMe->a.m_pIShell, IDC_MENU_EDIT_SECURITY);
        CDataStatsApp_SetDefaultSoftkeyLook(pMe->a.m_pIShell, IDC_SK_EDIT_SECURITY);
      }
      else
      {
          DATASTATS_ERR("pIDialog NULL");
      }
      return TRUE;

  case EVT_COMMAND:
      PRINT_EVENT(EVT_COMMAND);
      return CDataStatsApp_ExecuteItemAction(pMe, wParam);
      

  case EVT_KEY:
      
      switch(wParam)
      {
        case AVK_CLR:
          PRINT_EVENT(AVK_CLR);           
          return CDataStatsApp_EndDialog(pMe, NULL);
        
        default:
          break;
      }
      break;
  
  case EVT_CTL_SEL_CHANGED:
      PRINT_EVENT(EVT_CTL_SEL_CHANGED);
      pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      
      if(pIDialog != NULL)
      {
          if (((IControl*)dwParam) == IDIALOG_GetControl(pIDialog, IDC_MENU_EDIT_SECURITY))
          {
              InitMenuIcons((IMenuCtl*) dwParam);
              SetMenuIcon((IMenuCtl*) dwParam, wParam, TRUE);
          }
      }
      else
      {
          DATASTATS_ERR("pIDialog NULL");
      }
      break;

  case EVT_DIALOG_END:
      PRINT_EVENT(EVT_DIALOG_END);
      IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_BACKGROUND,MAKE_RGB(255, 255, 255));
      IDISPLAY_Update(pMe->a.m_pIDisplay);
      return TRUE;

  default:
      break;
  }
#endif //(FEATURE_WCDMA) || (FEATURE_GSM_GPRS)
  return FALSE;
}

#ifdef FEATURE_DATA_IMS 
/*===========================================================================
FUNCTION CDataStatsApp_PCSCFReqDlgHandler

DESCRIPTION
  The Request PCSCF Address Dialog Event Handler


DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_PCSCFReqDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  CDataStatsApp *pMe = (CDataStatsApp*) pUser;
  IDialog *pIDialog;
  IMenuCtl *pIMenuCtl = NULL;
  
  if(NULL == pMe)
  {
      DATASTATS_ERR("Null pMe");
      return FALSE;
  }
  switch(evt)
  {
    case EVT_DIALOG_START:
      PRINT_EVENT(EVT_DIALOG_START);
      pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);

      if(pIDialog != NULL)
      {      
        pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_EDIT_PCSCF);
        if(pIMenuCtl != NULL)
        {
          InitMenuIcons(pIMenuCtl);
          switch(pMe->m_pcscf_addr)
          {
            case TRUE:
              SetMenuIcon(pIMenuCtl, IDL_EDIT_PCSCF_TRUE, TRUE);
              break;

            case FALSE:
            default :
              SetMenuIcon(pIMenuCtl, IDL_EDIT_PCSCF_FALSE, TRUE);
              break;              
          }
        }
        else
        {
          DATASTATS_ERR("pIMenuCtl NULL");    
          return FALSE;
        }
        IMENUCTL_SetTitle(pIMenuCtl, DATASTATSAPP_RES_FILE, 0, pMe->profileText);
        CDataStatsApp_SetMenuLook(pMe->a.m_pIShell, IDC_MENU_EDIT_PCSCF);
        CDataStatsApp_SetDefaultSoftkeyLook(pMe->a.m_pIShell, IDC_SK_EDIT_PCSCF);
      }
      else
      {
        DATASTATS_ERR("pIDialog NULL");
      }
      return TRUE;

    case EVT_COMMAND:
      PRINT_EVENT(EVT_COMMAND);
      return CDataStatsApp_ExecuteItemAction(pMe, wParam);
      

    case EVT_KEY:
      
      switch(wParam)
      {
        case AVK_CLR:
          PRINT_EVENT(AVK_CLR);           
          return CDataStatsApp_EndDialog(pMe, NULL);
        
        default:
          break;
      }
      break;
  
    case EVT_CTL_SEL_CHANGED:
      PRINT_EVENT(EVT_CTL_SEL_CHANGED);
      pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      
      if(pIDialog != NULL)
      {
        if (((IControl*)dwParam) == IDIALOG_GetControl(pIDialog, IDC_MENU_EDIT_PCSCF))
        {
          InitMenuIcons((IMenuCtl*) dwParam);
          SetMenuIcon((IMenuCtl*) dwParam, wParam, TRUE);
        }
      }
      else
      {
        DATASTATS_ERR("pIDialog NULL");
      }
      break;

    case EVT_DIALOG_END:
      PRINT_EVENT(EVT_DIALOG_END);
      IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_BACKGROUND,MAKE_RGB(255, 255, 255));
      IDISPLAY_Update(pMe->a.m_pIDisplay);
      return TRUE;

    default:
      break;
  }
#else
  /*lint -esym(715,pUser)*/
  /*lint -esym(715,evt)*/
  /*lint -esym(715,wParam)*/
  /*lint -esym(715,dwParam)*/ 
#endif //(FEATURE_WCDMA) || (FEATURE_GSM_GPRS)
  return FALSE;
} // end of CDataStatsApp_PCSCFReqDlgHandler
#endif //FEATURE_DATA_IMS

#ifdef FEATURE_DATA_IMS
/*===========================================================================
FUNCTION CDataStatsApp_DHCPPCSCFDlgHandler

DESCRIPTION
  The PCSCF Address using DHCP Dialog Event Handler


DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_DHCPPCSCFDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  CDataStatsApp *pMe = (CDataStatsApp*) pUser;
  IDialog *pIDialog;
  IMenuCtl *pIMenuCtl = NULL;
  
  if(NULL == pMe)
  {
      DATASTATS_ERR("Null pMe");
      return FALSE;
  }
  switch(evt)
  {
    case EVT_DIALOG_START:
      PRINT_EVENT(EVT_DIALOG_START);
      pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);

      if(pIDialog != NULL)
      {      
        pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_DHCP_PCSCF);
        if(pIMenuCtl != NULL)
        {
          InitMenuIcons(pIMenuCtl);
          switch(pMe->m_dhcp_pcscf_addr)
          {
            case TRUE:
              SetMenuIcon(pIMenuCtl, IDL_EDIT_DHCP_PCSCF_TRUE, TRUE);
              break;

            case FALSE:
            default :
              SetMenuIcon(pIMenuCtl, IDL_EDIT_DHCP_PCSCF_FALSE, TRUE);
              break;              
          }
        }
        else
        {
          DATASTATS_ERR("pIMenuCtl NULL");    
          return FALSE;
        }
        IMENUCTL_SetTitle(pIMenuCtl, DATASTATSAPP_RES_FILE, 0, pMe->profileText);
        CDataStatsApp_SetMenuLook(pMe->a.m_pIShell, IDC_MENU_DHCP_PCSCF);
        CDataStatsApp_SetDefaultSoftkeyLook(pMe->a.m_pIShell, IDC_SK_DHCP_PCSCF);
      }
      else
      {
        DATASTATS_ERR("pIDialog NULL");
      }
      return TRUE;

    case EVT_COMMAND:
      PRINT_EVENT(EVT_COMMAND);
      return CDataStatsApp_ExecuteItemAction(pMe, wParam);
      

    case EVT_KEY:
      
      switch(wParam)
      {
        case AVK_CLR:
          PRINT_EVENT(AVK_CLR);           
          return CDataStatsApp_EndDialog(pMe, NULL);
        
        default:
          break;
      }
      break;
  
    case EVT_CTL_SEL_CHANGED:
      PRINT_EVENT(EVT_CTL_SEL_CHANGED);
      pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      
      if(pIDialog != NULL)
      {
        if (((IControl*)dwParam) == IDIALOG_GetControl(pIDialog, IDC_MENU_DHCP_PCSCF))
        {
          InitMenuIcons((IMenuCtl*) dwParam);
          SetMenuIcon((IMenuCtl*) dwParam, wParam, TRUE);
        }
      }
      else
      {
        DATASTATS_ERR("pIDialog NULL");
      }
      break;

    case EVT_DIALOG_END:
      PRINT_EVENT(EVT_DIALOG_END);
      IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_BACKGROUND,MAKE_RGB(255, 255, 255));
      IDISPLAY_Update(pMe->a.m_pIDisplay);
      return TRUE;

    default:
      break;
  }
#else
  /*lint -esym(715,pUser)*/
  /*lint -esym(715,evt)*/
  /*lint -esym(715,wParam)*/
  /*lint -esym(715,dwParam)*/ 
#endif //(FEATURE_WCDMA) || (FEATURE_GSM_GPRS)
  return FALSE;
} // end of CDataStatsApp_DHCPPCSCFDlgHandler
#endif //FEATURE_DATA_IMS

#ifdef FEATURE_DATA_IMS 
/*===========================================================================
FUNCTION CDataStatsApp_IMCNFlagDlgHandler

DESCRIPTION
  The IM CN Flag Dialog Event Handler


DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_IMCNFlagDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  CDataStatsApp *pMe = (CDataStatsApp*) pUser;
  IDialog *pIDialog;
  IMenuCtl *pIMenuCtl = NULL;
  
  if(NULL == pMe)
  {
      DATASTATS_ERR("Null pMe");
      return FALSE;
  }
  switch(evt)
  {
    case EVT_DIALOG_START:
      PRINT_EVENT(EVT_DIALOG_START);
      pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);

      if(pIDialog != NULL)
      {      
        pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_IM_CN_FLAG);
        if(pIMenuCtl != NULL)
        {
          InitMenuIcons(pIMenuCtl);
          switch(pMe->m_im_cn_flag)
          {
            case TRUE:
              SetMenuIcon(pIMenuCtl, IDL_EDIT_IM_CN_FLAG_TRUE, TRUE);
              break;

            case FALSE:
            default :
              SetMenuIcon(pIMenuCtl, IDL_EDIT_IM_CN_FLAG_FALSE, TRUE);
              break;              
          }
        }
        else
        {
          DATASTATS_ERR("pIMenuCtl NULL");    
          return FALSE;
        }
        IMENUCTL_SetTitle(pIMenuCtl, DATASTATSAPP_RES_FILE, 0, pMe->profileText);
        CDataStatsApp_SetMenuLook(pMe->a.m_pIShell, IDC_MENU_IM_CN_FLAG);
        CDataStatsApp_SetDefaultSoftkeyLook(pMe->a.m_pIShell, IDC_SK_IM_CN_FLAG);
      }
      else
      {
        DATASTATS_ERR("pIDialog NULL");
      }
      return TRUE;

    case EVT_COMMAND:
      PRINT_EVENT(EVT_COMMAND);
      return CDataStatsApp_ExecuteItemAction(pMe, wParam);
      

    case EVT_KEY:
      
      switch(wParam)
      {
        case AVK_CLR:
          PRINT_EVENT(AVK_CLR);           
          return CDataStatsApp_EndDialog(pMe, NULL);
        
        default:
          break;
      }
      break;
  
    case EVT_CTL_SEL_CHANGED:
      PRINT_EVENT(EVT_CTL_SEL_CHANGED);
      pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      
      if(pIDialog != NULL)
      {
        if (((IControl*)dwParam) == IDIALOG_GetControl(pIDialog, IDC_MENU_IM_CN_FLAG))
        {
          InitMenuIcons((IMenuCtl*) dwParam);
          SetMenuIcon((IMenuCtl*) dwParam, wParam, TRUE);
        }
      }
      else
      {
        DATASTATS_ERR("pIDialog NULL");
      }
      break;

    case EVT_DIALOG_END:
      PRINT_EVENT(EVT_DIALOG_END);
      IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_BACKGROUND,MAKE_RGB(255, 255, 255));
      IDISPLAY_Update(pMe->a.m_pIDisplay);
      return TRUE;

    default:
      break;
  }
#else
  /*lint -esym(715,pUser)*/
  /*lint -esym(715,evt)*/
  /*lint -esym(715,wParam)*/
  /*lint -esym(715,dwParam)*/ 
#endif //(FEATURE_WCDMA) || (FEATURE_GSM_GPRS)
  return FALSE;
} // end of CDataStatsApp_IMCNFlagDlgHandler
#endif //FEATURE_DATA_IMS

/*===========================================================================
FUNCTION CDataStatsApp_EditQoSDlgHandler

DESCRIPTION
  The Edit QoS Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
 boolean CDataStatsApp_EditQoSDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  CDataStatsApp *pMe = (CDataStatsApp*) pUser;
  IDialog *pIDialog = NULL;
  IMenuCtl *pIMenuCtl = NULL;

  if(NULL == pMe)
  {
    DATASTATS_ERR("Null pMe");
    return FALSE;
  }
  switch(evt)
  {
  case EVT_DIALOG_START:
    PRINT_EVENT(EVT_DIALOG_START);
    pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);

    if(pIDialog == NULL) {
      DATASTATS_ERR("pIDialog NULL");
      return FALSE;
    }
          
    pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_EDIT_QOS);

    if(pIMenuCtl == NULL){
      DATASTATS_ERR("pIDialog NULL");
      return FALSE;
    }
    InitMenuIcons(pIMenuCtl);
    
    switch(pMe->nQoSType)
    {
    case AEEPDP_QOS_UMTS_REQ:
      SetMenuIcon(pIMenuCtl, IDL_MENU_EDIT_QOS_UMTS_REQ, TRUE);
    break;
    case AEEPDP_QOS_UMTS_MIN:
      SetMenuIcon(pIMenuCtl, IDL_MENU_EDIT_QOS_UMTS_MIN, TRUE);
    break;
    case AEEPDP_QOS_GPRS_REQ:
      SetMenuIcon(pIMenuCtl,IDL_MENU_EDIT_QOS_GPRS_REQ , TRUE);
    break;
    case AEEPDP_QOS_GPRS_MIN:
      SetMenuIcon(pIMenuCtl, IDL_MENU_EDIT_QOS_GPRS_MIN, TRUE);
    break;
    default :
      SetMenuIcon(pIMenuCtl, IDL_MENU_EDIT_QOS_NONE, TRUE);
    break;
    }
   
    IMENUCTL_SetTitle(pIMenuCtl, DATASTATSAPP_RES_FILE, 0, pMe->profileText);
 
    CDataStatsApp_SetMenuLook(pMe->a.m_pIShell, IDC_MENU_EDIT_QOS);
    CDataStatsApp_SetDefaultSoftkeyLook(pMe->a.m_pIShell, IDC_SK_EDIT_QOS);
  return TRUE;    
  
  case EVT_COMMAND:
    PRINT_EVENT(EVT_COMMAND);
    return CDataStatsApp_ExecuteItemAction(pMe, wParam);
      
  case EVT_KEY:
      
    switch(wParam)
    {
    case AVK_CLR:
      PRINT_EVENT(AVK_CLR);           
    return CDataStatsApp_EndDialog(pMe, NULL);

    default:
    break;
    }
  break;
  
  case EVT_CTL_SEL_CHANGED:
    PRINT_EVENT(EVT_CTL_SEL_CHANGED);
    pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);

    if(pIDialog == NULL){
      DATASTATS_ERR("pIDialog NULL");
      return FALSE;
    }
    if (((IControl*)dwParam) == IDIALOG_GetControl(pIDialog, IDC_MENU_EDIT_QOS))
    {
      InitMenuIcons((IMenuCtl*) dwParam);
      SetMenuIcon((IMenuCtl*) dwParam, wParam, TRUE);
    }
  break;

  case EVT_DIALOG_END:
    PRINT_EVENT(EVT_DIALOG_END);
    IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_BACKGROUND,MAKE_RGB(255, 255, 255));
    IDISPLAY_Update(pMe->a.m_pIDisplay);
  return TRUE;

  default:
  break;
  }
#endif //(FEATURE_WCDMA) || (FEATURE_GSM_GPRS)
  return FALSE;
}

/*===========================================================================
FUNCTION CDataStatsApp_EditQoSUMTSDlgHandler

DESCRIPTION
  Edit UMTS QOS Event Handler.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditQoSUMTSDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  CDataStatsApp *pMe = (CDataStatsApp*) pUser;
  IDialog *pIDialog = NULL ;
  IMenuCtl *pIMenuCtl = NULL;
  AECHAR *szBuf = NULL;
  AECHAR *wBuf = NULL;
  char *Buf = NULL;
  AEEPDPUMTSQOS  *UMTSQoS = NULL;

  if(NULL == pMe)
  {
    DATASTATS_ERR("Null pMe");
    return FALSE;
  }
    
  switch(evt)
  {
  case EVT_DIALOG_START:
    PRINT_EVENT(EVT_DIALOG_START);

    pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    if(pIDialog == NULL){
      DATASTATS_ERR("pIDialog NULL");
      return FALSE;
    }

    pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_EDIT_QOS_UMTS);
    if(pIMenuCtl == NULL){
      DATASTATS_ERR("pIMenuCtl NULL");
      return FALSE;
    }

     /* check whether it is requested or minimum */
    if ( pMe->nQoSType == AEEPDP_QOS_UMTS_REQ ){    
    
      UMTSQoS = &pMe->m_UMTSQoSReq;
    } else if (pMe->nQoSType == AEEPDP_QOS_UMTS_MIN){
      UMTSQoS = &pMe->m_UMTSQoSMin;
    }else
      return FALSE;

    szBuf = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE); 
    wBuf = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
    Buf = (char*) MALLOC(DATASTATS_MAX_BUFFER_SIZE*sizeof(char));

    if(szBuf == NULL || wBuf == NULL || Buf == NULL)
    {
      DATASTATS_ERR("Malloc Failed");
      FREEIF(szBuf);
      FREEIF(wBuf);
      FREEIF(Buf);
      return FALSE;
    }

    IMENUCTL_SetTitle(pIMenuCtl, DATASTATSAPP_RES_FILE, 0, pMe->profileText);

    switch(UMTSQoS->traffic_class)
    {
    case AEEPDP_QOS_TCLASS_SUBSCRIBE:
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_UMTS_TC_SUBSCRIBE, 
        wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    break;

    case AEEPDP_QOS_TCLASS_CONV:
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_UMTS_TC_CONV, 
        wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    break;

    case AEEPDP_QOS_TCLASS_STRM:
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_UMTS_TC_STREAM, 
      wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    break;

    case AEEPDP_QOS_TCLASS_INTR:
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_UMTS_TC_INTERACTIVE, 
      wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    break;
    case AEEPDP_QOS_TCLASS_BACK:
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_UMTS_TC_BACKGROUND, 
      wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    break;

    default: 
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_NONE, 
      wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    break;
    }

    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_UMTS_TC, 
      szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    (void) WSTRLCAT(szBuf, wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

    IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_QOS_UMTS_TC, DATASTATSAPP_RES_FILE, 0, szBuf);

    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_UMTS_MAX_UL_BR, 
      szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

    (void) SNPRINTF(Buf, DATASTATS_MAX_BUFFER_SIZE, "%ld", UMTSQoS->max_ul_bitrate);
    (void) STRTOWSTR(Buf, wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
 
    (void) WSTRLCAT(szBuf, wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_QOS_UMTS_MAX_UL_BR, DATASTATSAPP_RES_FILE, 0, szBuf);

    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_UMTS_MAX_DL_BR, 
      szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

    Buf[0] = '\0';
    wBuf[0] = '\0';
    (void) SNPRINTF(Buf, DATASTATS_MAX_BUFFER_SIZE, "%ld", UMTSQoS->max_dl_bitrate);
    (void) STRTOWSTR(Buf, wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
 
    (void) WSTRLCAT(szBuf, wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

    IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_QOS_UMTS_MAX_DL_BR, DATASTATSAPP_RES_FILE, 0, szBuf);            

    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_UMTS_MIN_UL_BR, 
      szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

    Buf[0] = '\0';
    wBuf[0] = '\0';
    (void) SNPRINTF(Buf, DATASTATS_MAX_BUFFER_SIZE, "%ld", UMTSQoS->gtd_ul_bitrate);
    (void) STRTOWSTR(Buf, wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
 
    (void) WSTRLCAT(szBuf, wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

    IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_QOS_UMTS_MIN_UL_BR, DATASTATSAPP_RES_FILE, 0, szBuf);
                  
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_UMTS_MIN_DL_BR, 
      szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

    Buf[0] = '\0';
    wBuf[0] = '\0';
    (void) SNPRINTF(Buf, DATASTATS_MAX_BUFFER_SIZE, "%ld", UMTSQoS->gtd_dl_bitrate);
    (void) STRTOWSTR(Buf, wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
 
    (void) WSTRLCAT(szBuf, wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

    IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_QOS_UMTS_MIN_DL_BR, DATASTATSAPP_RES_FILE, 0, szBuf);                         

    CDataStatsApp_SetMenuLook(pMe->a.m_pIShell, IDC_MENU_EDIT_QOS_UMTS);
    IMENUCTL_Redraw(pIMenuCtl);

    FREE(szBuf);
    FREE(wBuf);
    FREE(Buf);

    CDataStatsApp_SetDefaultSoftkeyLook(pMe->a.m_pIShell, IDC_SK_QOS_UMTS);
  return TRUE;

  case EVT_COMMAND:
    PRINT_EVENT(EVT_COMMAND);
  return CDataStatsApp_ExecuteItemAction(pMe, wParam);

  case EVT_KEY:

    switch(wParam)
    {
    case AVK_CLR:
      PRINT_EVENT(AVK_CLR);
    return CDataStatsApp_EndDialog(pMe, NULL);

    default:
    break;
    }
  break;

  case EVT_DIALOG_END:
    PRINT_EVENT(EVT_DIALOG_END);
  return TRUE;

  default:
  break;
  }
#endif //(FEATURE_WCDMA) || (FEATURE_GSM_GPRS)
  return FALSE;
}

/*===========================================================================
FUNCTION CDataStatsApp_EditQoSUMTSTCDlgHandler

DESCRIPTION
  The Edit QOS UMTS TC Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditQoSUMTSTCDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  CDataStatsApp *pMe = (CDataStatsApp*) pUser;
  IDialog *pIDialog = NULL;
  IMenuCtl *pIMenuCtl = NULL;
  AECHAR *szBuf = NULL;
  AEEPDPUMTSQOS  *UMTSQoS = NULL;

  if(NULL == pMe){
    DATASTATS_ERR("Null pMe");
    return FALSE;
  }
  switch(evt)
  {
  case EVT_DIALOG_START:
    PRINT_EVENT(EVT_DIALOG_START);
    pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);

    if(pIDialog == NULL) {
      DATASTATS_ERR("pIDialog NULL");
      return FALSE;
    }
    pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_EDIT_QOS_UMTS_TC);
    if(pIMenuCtl == NULL){
      DATASTATS_ERR("pIMenuCtl NULL"); 
      return FALSE;
    }
    InitMenuIcons(pIMenuCtl);

    /* check whether it is requested or minimum */
    if ( pMe->nQoSType == AEEPDP_QOS_UMTS_REQ ){    
      UMTSQoS = &pMe->m_UMTSQoSReq;
    } else if (pMe->nQoSType == AEEPDP_QOS_UMTS_MIN){
      UMTSQoS = &pMe->m_UMTSQoSMin;
    }else
      return FALSE;
    switch(UMTSQoS->traffic_class)
    {

    case AEEPDP_QOS_TCLASS_SUBSCRIBE :
      SetMenuIcon(pIMenuCtl, IDL_QOS_UMTS_TC_SUBSCRIBE, TRUE);
    break;

    case AEEPDP_QOS_TCLASS_CONV: 
      SetMenuIcon(pIMenuCtl, IDL_QOS_UMTS_TC_CONV, TRUE);
    break;
    case AEEPDP_QOS_TCLASS_STRM:
      SetMenuIcon(pIMenuCtl, IDL_QOS_UMTS_TC_STREAM, TRUE);
    break;
    case AEEPDP_QOS_TCLASS_INTR:
      SetMenuIcon(pIMenuCtl, IDL_QOS_UMTS_TC_INTERACTIVE, TRUE);
    break;
    case AEEPDP_QOS_TCLASS_BACK :
      SetMenuIcon(pIMenuCtl, IDL_QOS_UMTS_TC_BACKGROUND, TRUE);
    break;
    default:
      DATASTATS_ERR("Uknown selection");            
      return FALSE;
    }  
   
    szBuf = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
    if (szBuf == NULL){
      DATASTATS_ERR("Malloc Failed"); 
      return FALSE;
    }

    IMENUCTL_SetTitle(pIMenuCtl, DATASTATSAPP_RES_FILE, 0, pMe->profileText);

    CDataStatsApp_SetMenuLook(pMe->a.m_pIShell, IDC_MENU_EDIT_QOS_UMTS_TC);
    CDataStatsApp_SetDefaultSoftkeyLook(pMe->a.m_pIShell, IDC_SK_QOS_UMTS_TC);
    FREE(szBuf); 
    return TRUE;

  case EVT_COMMAND:
    PRINT_EVENT(EVT_COMMAND);
    return CDataStatsApp_ExecuteItemAction(pMe, wParam);

  case EVT_KEY:
      
    switch(wParam)
    {
    case AVK_CLR:
      PRINT_EVENT(AVK_CLR);           
    return CDataStatsApp_EndDialog(pMe, NULL);

    default:
    break;
    }
  break;
  
  case EVT_CTL_SEL_CHANGED:
    PRINT_EVENT(EVT_CTL_SEL_CHANGED);
    pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);

    if(pIDialog != NULL)
    {
      if (((IControl*)dwParam) == IDIALOG_GetControl(pIDialog, IDC_MENU_EDIT_QOS_UMTS_TC))
      {
          InitMenuIcons((IMenuCtl*) dwParam);
          SetMenuIcon((IMenuCtl*) dwParam, wParam, TRUE);
      }
    }
    else {
        DATASTATS_ERR("pIDialog NULL");
    }
  break;

  case EVT_DIALOG_END:
    PRINT_EVENT(EVT_DIALOG_END);
    IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_BACKGROUND,MAKE_RGB(255, 255, 255));
    IDISPLAY_Update(pMe->a.m_pIDisplay);
  return TRUE;

  default:
  break;
  }
#endif //(FEATURE_WCDMA) || (FEATURE_GSM_GPRS)
  return FALSE;
}
/*===========================================================================
FUNCTION CDataStatsApp_EditQoSUMTSMaxUlBRDlgHandler

DESCRIPTION
  The Edit QOS UMTS MAX UL Bit rate Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditQoSUMTSMaxUlBRDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  IDialog *pIDialog = NULL;
  ITextCtl *pIText = NULL;
  IMenuCtl *pISoftKeyMenu  = NULL;
  IStatic *pIStatic = NULL;
  CDataStatsApp *pMe = (CDataStatsApp*) pUser;

  if(NULL == pMe)
  {
    DATASTATS_ERR("Null pMe");
    return FALSE;
  }
  switch(evt)
  {
  case EVT_DIALOG_START:
    PRINT_EVENT(EVT_DIALOG_START);
    pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);

    if(pIDialog == NULL)
    {
      return FALSE;   
    }
    // Get controls in dialog
    pIText = (ITextCtl*)IDIALOG_GetControl(pIDialog, 
                                          IDC_TEXT_QOS_UMTS_MAX_UL_BR);
    pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, 
                                              IDC_SK_QOS_UMTS_MAX_UL_BR);

    pIStatic = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_QOS_UMTS_MAX_UL_BR_TITLE);

    if((pIText == NULL) || (pISoftKeyMenu == NULL) || (pIStatic == NULL) )
    {
      DATASTATS_ERR("Null pointer");
      return FALSE; // error 
    }

   
    ISTATIC_SetText(pIStatic, NULL, pMe->profileText, AEE_FONT_NORMAL, AEE_FONT_BOLD);
    ISTATIC_Redraw(pIStatic);

    // Update the softkey menu
    (void) IMENUCTL_DeleteAll(pISoftKeyMenu);
    IMENUCTL_AddItem(pISoftKeyMenu, DATASTATSAPP_RES_FILE, IDS_DD_OK, IDL_SK_QOS_UMTS_MAX_UL_BIT_RATE_OK, NULL, 0);
    IMENUCTL_AddItem(pISoftKeyMenu, DATASTATSAPP_RES_FILE, IDS_DD_CANCEL, IDL_SK_QOS_UMTS_MAX_UL_BIT_RATE_CANCEL, NULL, 0);          
        
    CDataStatsApp_SetDefaultSoftkeyLook(pMe->a.m_pIShell,IDC_SK_QOS_UMTS_MAX_UL_BR );


    ITEXTCTL_SetProperties(pIText, TP_FRAME );
    ITEXTCTL_SetMaxSize(pIText, AEEPDP_MAX_BR_STRING_LEN);
    (void) ITEXTCTL_SetInputMode(pIText, AEE_TM_NUMBERS);
    ITEXTCTL_SetActive(pIText, TRUE);

    ITEXTCTL_SetCursorPos(pIText, TC_CURSOREND);
    (void) ITEXTCTL_Redraw(pIText);

    (void) IDIALOG_SetFocus(pIDialog, IDC_TEXT_QOS_UMTS_MAX_UL_BR);
    IDIALOG_Redraw(pIDialog);
  return TRUE;

  case EVT_COMMAND:
    PRINT_EVENT(EVT_COMMAND);
  return CDataStatsApp_ExecuteItemAction(pMe, wParam);

  case EVT_KEY:
      
    switch(wParam)
    {
    case AVK_CLR:
      PRINT_EVENT(AVK_CLR);           
    return CDataStatsApp_EndDialog(pMe, NULL);

    default:
    break;
    }
  break;

  case EVT_DIALOG_END:
    PRINT_EVENT(EVT_DIALOG_END);
    IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_BACKGROUND,MAKE_RGB(255, 255, 255));
    IDISPLAY_Update(pMe->a.m_pIDisplay);
  return TRUE;

  default:
  break;
  }
#endif //(FEATURE_WCDMA) || (FEATURE_GSM_GPRS)
  return FALSE;
}

/*===========================================================================
FUNCTION CDataStatsApp_EditQoSUMTSMaxDlBRDlgHandler

DESCRIPTION
  Edit QOS UMTS Maximum DL Bit Rate Dialog Handler.
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditQoSUMTSMaxDlBRDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  IDialog *pIDialog = NULL;
  ITextCtl *pIText = NULL;
  IMenuCtl *pISoftKeyMenu  = NULL;
  IStatic *pIStatic = NULL;
  CDataStatsApp *pMe = (CDataStatsApp*) pUser;


  if(NULL == pMe)
  {
    DATASTATS_ERR("Null pMe");
    return FALSE;
  }
  switch(evt)
  {
  case EVT_DIALOG_START:
    PRINT_EVENT(EVT_DIALOG_START);
    pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);

    if(pIDialog == NULL)
    {
      return FALSE;   
    }
    // Get controls in dialog
    pIText = (ITextCtl*)IDIALOG_GetControl(pIDialog, 
                                          IDC_TEXT_QOS_UMTS_MAX_DL_BR);
    pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, 
                                              IDC_SK_QOS_UMTS_MAX_DL_BR);

    pIStatic = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_QOS_UMTS_MAX_DL_BR_TITLE);

    if((pIText == NULL) || (pISoftKeyMenu == NULL) || (pIStatic == NULL) )
    {
      DATASTATS_ERR("Null pointer");
      return FALSE; // error 
    }

   
    ISTATIC_SetText(pIStatic, NULL, pMe->profileText, AEE_FONT_NORMAL, AEE_FONT_BOLD);
    ISTATIC_Redraw(pIStatic);

    // Update the softkey menu
    (void) IMENUCTL_DeleteAll(pISoftKeyMenu);
    IMENUCTL_AddItem(pISoftKeyMenu, DATASTATSAPP_RES_FILE, IDS_DD_OK, IDL_SK_QOS_UMTS_MAX_DL_BR_OK, NULL, 0);
    IMENUCTL_AddItem(pISoftKeyMenu, DATASTATSAPP_RES_FILE, IDS_DD_CANCEL, IDL_SK_QOS_UMTS_MAX_DL_BR_CANCEL, NULL, 0);          
        
    CDataStatsApp_SetDefaultSoftkeyLook(pMe->a.m_pIShell,IDC_SK_QOS_UMTS_MAX_DL_BR );


    ITEXTCTL_SetProperties(pIText, TP_FRAME );
    ITEXTCTL_SetMaxSize(pIText, AEEPDP_MAX_BR_STRING_LEN);
    (void) ITEXTCTL_SetInputMode(pIText, AEE_TM_NUMBERS);
    ITEXTCTL_SetActive(pIText, TRUE);

    ITEXTCTL_SetCursorPos(pIText, TC_CURSOREND);
    (void) ITEXTCTL_Redraw(pIText);

    (void) IDIALOG_SetFocus(pIDialog, IDC_TEXT_QOS_UMTS_MAX_DL_BR);
    IDIALOG_Redraw(pIDialog);
  return TRUE;

  case EVT_COMMAND:
    PRINT_EVENT(EVT_COMMAND);
  return CDataStatsApp_ExecuteItemAction(pMe, wParam);

  case EVT_KEY:    
    switch(wParam)
    {
    case AVK_CLR:
      PRINT_EVENT(AVK_CLR);           
    return CDataStatsApp_EndDialog(pMe, NULL);

    default:
    break;
    }
  break;

  case EVT_DIALOG_END:
    PRINT_EVENT(EVT_DIALOG_END);
    IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_BACKGROUND,MAKE_RGB(255, 255, 255));
    IDISPLAY_Update(pMe->a.m_pIDisplay);
  return TRUE;

  default:
  break;
  }
#endif //(FEATURE_WCDMA) || (FEATURE_GSM_GPRS)
  return FALSE;
}

/*===========================================================================
FUNCTION CDataStatsApp_EditQoSUMTSMinUlBRDlgHandler

DESCRIPTION
  Edit QOS UMTS Minimum UL Bit Rate Dialog Handler.
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditQoSUMTSMinUlBRDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  IDialog *pIDialog = NULL;
  ITextCtl *pIText = NULL;
  IMenuCtl *pISoftKeyMenu  = NULL;
  IStatic *pIStatic = NULL;
  CDataStatsApp *pMe = (CDataStatsApp*) pUser;


  if(NULL == pMe)
  {
    DATASTATS_ERR("Null pMe");
    return FALSE;
  }
  switch(evt)
  {
  case EVT_DIALOG_START:
    PRINT_EVENT(EVT_DIALOG_START);
    pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);

    if(pIDialog == NULL)
    {
      return FALSE;   
    }
    // Get controls in dialog
    pIText = (ITextCtl*)IDIALOG_GetControl(pIDialog, 
                                          IDC_TEXT_QOS_UMTS_MIN_UL_BR);
    pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, 
                                              IDC_SK_QOS_UMTS_MIN_UL_BR);

    pIStatic = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_QOS_UMTS_MIN_UL_BR_TITLE);

    if((pIText == NULL) || (pISoftKeyMenu == NULL) || (pIStatic == NULL) )
    {
      DATASTATS_ERR("Null pointer");
      return FALSE; // error 
    }

   
    ISTATIC_SetText(pIStatic, NULL, pMe->profileText, AEE_FONT_NORMAL, AEE_FONT_BOLD);
    ISTATIC_Redraw(pIStatic);

    // Update the softkey menu
    (void) IMENUCTL_DeleteAll(pISoftKeyMenu);
    IMENUCTL_AddItem(pISoftKeyMenu, DATASTATSAPP_RES_FILE, IDS_DD_OK, IDL_SK_QOS_UMTS_MIN_UL_BR_OK, NULL, 0);
    IMENUCTL_AddItem(pISoftKeyMenu, DATASTATSAPP_RES_FILE, IDS_DD_CANCEL, IDL_SK_QOS_UMTS_MIN_UL_BR_CANCEL, NULL, 0);          
        
    CDataStatsApp_SetDefaultSoftkeyLook(pMe->a.m_pIShell,IDC_SK_QOS_UMTS_MIN_UL_BR );


    ITEXTCTL_SetProperties(pIText, TP_FRAME );
    ITEXTCTL_SetMaxSize(pIText, AEEPDP_MAX_BR_STRING_LEN);
    (void) ITEXTCTL_SetInputMode(pIText, AEE_TM_NUMBERS);
    ITEXTCTL_SetActive(pIText, TRUE);

    ITEXTCTL_SetCursorPos(pIText, TC_CURSOREND);
    (void) ITEXTCTL_Redraw(pIText);

    (void) IDIALOG_SetFocus(pIDialog, IDC_TEXT_QOS_UMTS_MIN_UL_BR);
    IDIALOG_Redraw(pIDialog);
  return TRUE;

  case EVT_COMMAND:
    PRINT_EVENT(EVT_COMMAND);
  return CDataStatsApp_ExecuteItemAction(pMe, wParam);

  case EVT_KEY:
      
    switch(wParam)
    {
    case AVK_CLR:
      PRINT_EVENT(AVK_CLR);           
    return CDataStatsApp_EndDialog(pMe, NULL);

    default:
    break;
    }
  break;

  case EVT_DIALOG_END:
    PRINT_EVENT(EVT_DIALOG_END);
    IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_BACKGROUND,MAKE_RGB(255, 255, 255));
    IDISPLAY_Update(pMe->a.m_pIDisplay);
  return TRUE;

  default:
  break;
  }
#endif //(FEATURE_WCDMA) || (FEATURE_GSM_GPRS)
  return FALSE;
}
/*===========================================================================
FUNCTION CDataStatsApp_EditQoSUMTSMinDlBRDlgHandler

DESCRIPTION
  Edit QOS UMTS Minimum DL Bit Rate Dialog Handler.
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditQoSUMTSMinDlBRDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  IDialog *pIDialog = NULL;
  ITextCtl *pIText = NULL;
  IMenuCtl *pISoftKeyMenu  = NULL;
  IStatic *pIStatic = NULL;
  CDataStatsApp *pMe = (CDataStatsApp*) pUser;


  if(NULL == pMe)
  {
    DATASTATS_ERR("Null pMe");
    return FALSE;
  }
  switch(evt)
  {
  case EVT_DIALOG_START:
    PRINT_EVENT(EVT_DIALOG_START);
    pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);

    if(pIDialog == NULL)
    {
      return FALSE;   
    }
    // Get controls in dialog
    pIText = (ITextCtl*)IDIALOG_GetControl(pIDialog, 
                                          IDC_TEXT_QOS_UMTS_MIN_DL_BR);
    pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, 
                                              IDC_SK_QOS_UMTS_MIN_DL_BR);

    pIStatic = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_QOS_UMTS_MIN_DL_BR_TITLE);

    if((pIText == NULL) || (pISoftKeyMenu == NULL) || (pIStatic == NULL) )
    {
      DATASTATS_ERR("Null pointer");
      return FALSE; // error 
    }

   
    ISTATIC_SetText(pIStatic, NULL, pMe->profileText, AEE_FONT_NORMAL, AEE_FONT_BOLD);
    ISTATIC_Redraw(pIStatic);

    // Update the softkey menu
    (void) IMENUCTL_DeleteAll(pISoftKeyMenu);
    IMENUCTL_AddItem(pISoftKeyMenu, DATASTATSAPP_RES_FILE, IDS_DD_OK, IDL_SK_QOS_UMTS_MIN_DL_BR_OK, NULL, 0);
    IMENUCTL_AddItem(pISoftKeyMenu, DATASTATSAPP_RES_FILE, IDS_DD_CANCEL, IDL_SK_QOS_UMTS_MIN_DL_BR_CANCEL, NULL, 0);          
        
    CDataStatsApp_SetDefaultSoftkeyLook(pMe->a.m_pIShell,IDC_SK_QOS_UMTS_MIN_DL_BR );


    ITEXTCTL_SetProperties(pIText, TP_FRAME );
    ITEXTCTL_SetMaxSize(pIText, AEEPDP_MAX_BR_STRING_LEN);
    (void) ITEXTCTL_SetInputMode(pIText, AEE_TM_NUMBERS);
    ITEXTCTL_SetActive(pIText, TRUE);

    ITEXTCTL_SetCursorPos(pIText, TC_CURSOREND);
    (void) ITEXTCTL_Redraw(pIText);

    (void) IDIALOG_SetFocus(pIDialog, IDC_TEXT_QOS_UMTS_MIN_DL_BR);
    IDIALOG_Redraw(pIDialog);
  return TRUE;

  case EVT_COMMAND:
    PRINT_EVENT(EVT_COMMAND);
  return CDataStatsApp_ExecuteItemAction(pMe, wParam);

  case EVT_KEY:
      
    switch(wParam)
    {
    case AVK_CLR:
      PRINT_EVENT(AVK_CLR);           
    return CDataStatsApp_EndDialog(pMe, NULL);

    default:
    break;
    }
  break;

  case EVT_DIALOG_END:
    PRINT_EVENT(EVT_DIALOG_END);
    IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_BACKGROUND,MAKE_RGB(255, 255, 255));
    IDISPLAY_Update(pMe->a.m_pIDisplay);
  return TRUE;

  default:
  break;
  }
#endif //(FEATURE_WCDMA) || (FEATURE_GSM_GPRS)
  return FALSE;
}

/*===========================================================================
FUNCTION CDataStatsApp_EditQoSGPRSDlgHandler

DESCRIPTION
  Edit GPRS QOS Event Handler.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditQoSGPRSDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  CDataStatsApp *pMe = (CDataStatsApp*) pUser;
  IDialog *pIDialog = NULL ;
  IMenuCtl *pIMenuCtl = NULL;
  AECHAR *szBuf = NULL;
  AECHAR *wBuf = NULL;
  char *Buf = NULL;
  AEEPDPGPRSQOS  *GPRSQoS = NULL;

  if(NULL == pMe)
  {
    DATASTATS_ERR("Null pMe");
    return FALSE;
  }
    
  switch(evt)
  {
  case EVT_DIALOG_START:
    PRINT_EVENT(EVT_DIALOG_START);
    pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    if(pIDialog == NULL){
      DATASTATS_ERR("pIDialog NULL");
      return FALSE;
    }

    pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_EDIT_QOS_GPRS);
    if(pIMenuCtl == NULL){
      DATASTATS_ERR("pIMenuCtl NULL");
      return FALSE;
    }
  
    /* check whether it is requested or minimum */
    if ( pMe->nQoSType == AEEPDP_QOS_GPRS_REQ ){    
      GPRSQoS = &pMe->m_GPRSQoSReq;
    } else if (pMe->nQoSType == AEEPDP_QOS_GPRS_MIN){
      GPRSQoS = &pMe->m_GPRSQoSMin;
    }else
      return FALSE;

    szBuf = (AECHAR*) MALLOC(2*DATASTATS_MAX_WIDE_BUFFER_SIZE); 
    wBuf = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
    Buf = (char*) MALLOC(DATASTATS_MAX_BUFFER_SIZE*sizeof(char));

    if(szBuf == NULL || wBuf == NULL || Buf == NULL){
      DATASTATS_ERR("Malloc Failed");
      FREEIF(szBuf);
      FREEIF(wBuf);
      FREEIF(Buf);
      return FALSE;
    }

    IMENUCTL_SetTitle(pIMenuCtl, DATASTATSAPP_RES_FILE, 0, pMe->profileText);

    switch(GPRSQoS->precedence)
    {
    case AEEPDP_QOS_PRECED_CLASS_SUB :
               
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_PREC_CLASS_SUB, 
        wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    break;

    case AEEPDP_QOS_PRECED_CLASS_1: 
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_PREC_CLASS_1, 
        wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    break;

    case AEEPDP_QOS_PRECED_CLASS_2:
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_PREC_CLASS_2, 
      wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    break;

    case AEEPDP_QOS_PRECED_CLASS_3:
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_PREC_CLASS_3, 
      wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    break;

    default: 
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_NONE, 
      wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    break;
    }

    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_PREC, 
      szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    (void) WSTRLCAT(szBuf, wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

    IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_QOS_GPRS_PREC_CLASS, DATASTATSAPP_RES_FILE, 0, szBuf);

    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_REL_CLASS, 
      szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    switch(GPRSQoS->reliability)
    {
    case AEEPDP_QOS_REL_CLASS_SUB:               
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_REL_CLASS_SUB, 
        wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    break;

    case AEEPDP_QOS_REL_CLASS_1: 
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_REL_CLASS_1, 
        wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    break;
    case AEEPDP_QOS_REL_CLASS_2:
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_REL_CLASS_2, 
      wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    break;

    case AEEPDP_QOS_REL_CLASS_3:
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_REL_CLASS_3, 
      wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    break;

    default: 
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_NONE, 
      wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    break;
    }
    
    (void) WSTRLCAT(szBuf, wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

    IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_QOS_GPRS_REL_CLASS, DATASTATSAPP_RES_FILE, 0, szBuf);

    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_DELAY_CLASS, 
      szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    switch(GPRSQoS->delay)
    {

    case AEEPDP_QOS_DELAY_CLASS_1: 
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_DELAY_CLASS_1, 
        wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    break;
    case AEEPDP_QOS_REL_CLASS_2:
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_DELAY_CLASS_2, 
      wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

    break;
    case AEEPDP_QOS_DELAY_CLASS_3:
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_DELAY_CLASS_3, 
      wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    break;

    case AEEPDP_QOS_DELAY_CLASS_4 :              
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_DELAY_CLASS_4,
        wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    break;

    case AEEPDP_QOS_DELAY_CLASS_SUB :              
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_DELAY_CLASS_SUB,
        wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    break;

    default: 
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_NONE, 
      wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    break;
    }
   
    (void) WSTRLCAT(szBuf, wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

    IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_QOS_GPRS_DELAY_CLASS, DATASTATSAPP_RES_FILE, 0, szBuf);            

    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_PEAK, 
      szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

    Buf[0] = '\0';
    wBuf[0] = '\0';
    (void) SNPRINTF(Buf, DATASTATS_MAX_WIDE_BUFFER_SIZE, "%ld", GPRSQoS->peak);
    (void) STRTOWSTR(Buf, wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
 
    (void) WSTRLCAT(szBuf, wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

    IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_QOS_GPRS_PEAK_DR, DATASTATSAPP_RES_FILE, 0, szBuf);
                  
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_MEAN, 
      szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

    Buf[0] = '\0';
    wBuf[0] = '\0';
    (void) SNPRINTF(Buf, DATASTATS_MAX_WIDE_BUFFER_SIZE, "%ld", GPRSQoS->mean);
    (void) STRTOWSTR(Buf, wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
 
    (void) WSTRLCAT(szBuf, wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

    IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_QOS_GPRS_MEAN_DR, DATASTATSAPP_RES_FILE, 0, szBuf);                         

    CDataStatsApp_SetMenuLook(pMe->a.m_pIShell, IDC_MENU_EDIT_QOS_GPRS);
    IMENUCTL_Redraw(pIMenuCtl);

    FREE(szBuf);
    FREE(wBuf);
    FREE(Buf);

    CDataStatsApp_SetDefaultSoftkeyLook(pMe->a.m_pIShell, IDC_SK_QOS_GPRS);
  return TRUE;

  case EVT_COMMAND:
    PRINT_EVENT(EVT_COMMAND);
  return CDataStatsApp_ExecuteItemAction(pMe, wParam);

  case EVT_KEY:

    switch(wParam)
    {
    case AVK_CLR:
      PRINT_EVENT(AVK_CLR);
    return CDataStatsApp_EndDialog(pMe, NULL);

    default:
    break;
    }
  break;

  case EVT_DIALOG_END:
    PRINT_EVENT(EVT_DIALOG_END);
  return TRUE;

  default:
  break;
  }
#endif //(FEATURE_WCDMA) || (FEATURE_GSM_GPRS)
  return FALSE;
}
/*===========================================================================
FUNCTION CDataStatsApp_EditQoSGPRSPrecDlgHandler

DESCRIPTION
  The Edit QOS GPRS Precedence Class Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditQoSGPRSPrecDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  CDataStatsApp *pMe = (CDataStatsApp*) pUser;
  IDialog *pIDialog = NULL;
  IMenuCtl *pIMenuCtl = NULL;
  AECHAR *szBuf = NULL;
  AEEPDPGPRSQOS *GPRSQoS = NULL;

  if(NULL == pMe){
      DATASTATS_ERR("Null pMe");
      return FALSE;
  }
  switch(evt)
  {
  case EVT_DIALOG_START:
    PRINT_EVENT(EVT_DIALOG_START);
    pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);

    if(pIDialog == NULL){
      DATASTATS_ERR("pIDialog NULL");
      return FALSE; 
    }
    pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_EDIT_QOS_GPRS_PREC);
    if(pIMenuCtl == NULL){
      DATASTATS_ERR("pIMenuCtl NULL"); 
      return FALSE;
    }
    InitMenuIcons(pIMenuCtl);
    /* check whether it is requested or minimum */
    if ( pMe->nQoSType == AEEPDP_QOS_GPRS_REQ ){    
      GPRSQoS = &pMe->m_GPRSQoSReq;
    } else if (pMe->nQoSType == AEEPDP_QOS_GPRS_MIN){
      GPRSQoS = &pMe->m_GPRSQoSMin;
    }else
      return FALSE;

    switch(GPRSQoS->precedence){
    case AEEPDP_QOS_PRECED_CLASS_SUB :
      SetMenuIcon(pIMenuCtl, IDL_QOS_GPRS_PREC_SUB, TRUE);
    break;

    case AEEPDP_QOS_PRECED_CLASS_1: 
      SetMenuIcon(pIMenuCtl, IDL_QOS_GPRS_PREC_CLASS_1, TRUE);
    break;
    case AEEPDP_QOS_PRECED_CLASS_2:
      SetMenuIcon(pIMenuCtl, IDL_QOS_GPRS_PREC_CLASS_2, TRUE);
    break;
    case AEEPDP_QOS_PRECED_CLASS_3:
      SetMenuIcon(pIMenuCtl, IDL_QOS_GPRS_PREC_CLASS_3, TRUE);
    break;

    default:
      DATASTATS_ERR("Uknown Precedence Class");            
    return FALSE;
    }  
 

    szBuf = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
    if (szBuf == NULL)
      return FALSE;

    IMENUCTL_SetTitle(pIMenuCtl, DATASTATSAPP_RES_FILE, 0, pMe->profileText);

    CDataStatsApp_SetMenuLook(pMe->a.m_pIShell, IDC_MENU_EDIT_QOS_GPRS_PREC);
    CDataStatsApp_SetDefaultSoftkeyLook(pMe->a.m_pIShell, IDC_SK_QOS_GPRS_PREC);
    FREE(szBuf);          
    return TRUE;

  case EVT_COMMAND:
    PRINT_EVENT(EVT_COMMAND);
    return CDataStatsApp_ExecuteItemAction(pMe, wParam);

  case EVT_KEY:
      
    switch(wParam)
    {
    case AVK_CLR:
      PRINT_EVENT(AVK_CLR);           
    return CDataStatsApp_EndDialog(pMe, NULL);

    default:
    break;
    }
  break;
  
  case EVT_CTL_SEL_CHANGED:
    PRINT_EVENT(EVT_CTL_SEL_CHANGED);
    pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);

    if(pIDialog != NULL)
    {
      if (((IControl*)dwParam) == IDIALOG_GetControl(pIDialog, IDC_MENU_EDIT_QOS_GPRS_PREC))
      {
        InitMenuIcons((IMenuCtl*) dwParam);
        SetMenuIcon((IMenuCtl*) dwParam, wParam, TRUE);
      }
    }
    else{
      DATASTATS_ERR("pIDialog NULL");
    }
  break;

  case EVT_DIALOG_END:
    PRINT_EVENT(EVT_DIALOG_END);
    IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_BACKGROUND,MAKE_RGB(255, 255, 255));
    IDISPLAY_Update(pMe->a.m_pIDisplay);
  return TRUE;

  default:
  break;
  }
#endif //(FEATURE_WCDMA) || (FEATURE_GSM_GPRS)
  return FALSE;
}

/*===========================================================================
FUNCTION CDataStatsApp_EditQoSGPRSDelayDlgHandler

DESCRIPTION
  The Edit QOS GPRS Delay Class Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditQoSGPRSDelayDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  CDataStatsApp *pMe = (CDataStatsApp*) pUser;
  IDialog *pIDialog;
  IMenuCtl *pIMenuCtl = NULL;
  AECHAR *szBuf;
  AEEPDPGPRSQOS *GPRSQoS = NULL;

  if(NULL == pMe){
    DATASTATS_ERR("Null pMe");
    return FALSE;
  }
  switch(evt)
  {
  case EVT_DIALOG_START:
    PRINT_EVENT(EVT_DIALOG_START);
    pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);

    if(pIDialog == NULL){
      DATASTATS_ERR("pIDialog NULL");
      return FALSE;
    }
    pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_EDIT_QOS_GPRS_DELAY_CLASS);
    if(pIMenuCtl == NULL){
      DATASTATS_ERR("pIMenuCtl NULL");    
      return FALSE;
    }
    InitMenuIcons(pIMenuCtl);
    /* check whether it is requested or minimum */
    if ( pMe->nQoSType == AEEPDP_QOS_GPRS_REQ ){    
      GPRSQoS = &pMe->m_GPRSQoSReq;
    } else if (pMe->nQoSType == AEEPDP_QOS_GPRS_MIN){
      GPRSQoS = &pMe->m_GPRSQoSMin;
    }else
      return FALSE;
    switch(GPRSQoS->delay)
    {

    case AEEPDP_QOS_DELAY_CLASS_SUB :
      SetMenuIcon(pIMenuCtl, IDL_QOS_GPRS_DELAY_CLASS_SUB, TRUE);
    break;

    case AEEPDP_QOS_DELAY_CLASS_1: 
      SetMenuIcon(pIMenuCtl, IDL_QOS_GPRS_DELAY_CLASS_1, TRUE);
    break;

    case AEEPDP_QOS_DELAY_CLASS_2:
      SetMenuIcon(pIMenuCtl, IDL_QOS_GPRS_DELAY_CLASS_2, TRUE);     
    break;

    case AEEPDP_QOS_DELAY_CLASS_3:
      SetMenuIcon(pIMenuCtl, IDL_QOS_GPRS_DELAY_CLASS_3, TRUE);
    break;

    case AEEPDP_QOS_DELAY_CLASS_4:
      SetMenuIcon(pIMenuCtl, IDL_QOS_GPRS_DELAY_CLASS_4, TRUE);
    break;

    default:
      DATASTATS_ERR("Uknown Delay Class");            
    return FALSE;       
    }  


    szBuf = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
    if (szBuf == NULL)
      return FALSE;

    IMENUCTL_SetTitle(pIMenuCtl, DATASTATSAPP_RES_FILE, 0, pMe->profileText);

    CDataStatsApp_SetMenuLook(pMe->a.m_pIShell, IDC_MENU_EDIT_QOS_GPRS_DELAY_CLASS);
    CDataStatsApp_SetDefaultSoftkeyLook(pMe->a.m_pIShell, IDC_SK_QOS_GPRS_DELAY_CLASS);
    FREE(szBuf); 
    return TRUE;

  case EVT_COMMAND:
    PRINT_EVENT(EVT_COMMAND);
    return CDataStatsApp_ExecuteItemAction(pMe, wParam);

  case EVT_KEY:
      
    switch(wParam)
    {
    case AVK_CLR:
      PRINT_EVENT(AVK_CLR);           
    return CDataStatsApp_EndDialog(pMe, NULL);

    default:
    break;
    }
  break;
  
  case EVT_CTL_SEL_CHANGED:
    PRINT_EVENT(EVT_CTL_SEL_CHANGED);
    pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);

    if(pIDialog != NULL)
    {
      if (((IControl*)dwParam) == IDIALOG_GetControl(pIDialog, IDC_MENU_EDIT_QOS_GPRS_DELAY_CLASS))
      {
      InitMenuIcons((IMenuCtl*) dwParam);
      SetMenuIcon((IMenuCtl*) dwParam, wParam, TRUE);
      }
    }
    else
    {
      DATASTATS_ERR("pIDialog NULL");
    }
  break;

  case EVT_DIALOG_END:
    PRINT_EVENT(EVT_DIALOG_END);
    IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_BACKGROUND,MAKE_RGB(255, 255, 255));
    IDISPLAY_Update(pMe->a.m_pIDisplay);
  return TRUE;

  default:
  break;
  }
#endif //(FEATURE_WCDMA) || (FEATURE_GSM_GPRS)
  return FALSE;
}

/*===========================================================================
FUNCTION CDataStatsApp_EditQoSGPRSRelDlgHandler

DESCRIPTION
  The Edit QOS GPRS Reliability Class Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditQoSGPRSRelDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  CDataStatsApp *pMe = (CDataStatsApp*) pUser;
  IDialog *pIDialog = NULL;
  IMenuCtl *pIMenuCtl = NULL;
  AECHAR *szBuf = NULL;
  AEEPDPGPRSQOS *GPRSQoS = NULL;

  if(NULL == pMe){
    DATASTATS_ERR("Null pMe");
    return FALSE;
  }
  switch(evt)
  {
  case EVT_DIALOG_START:
    PRINT_EVENT(EVT_DIALOG_START);
    pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);

    if(pIDialog == NULL){
      DATASTATS_ERR("pIDialog NULL");
      return FALSE;
    }
    pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_EDIT_QOS_GPRS_REL_CLASS);
    if(pIMenuCtl == NULL){
      DATASTATS_ERR("pIMenuCtl NULL");    
      return FALSE;
    }
    InitMenuIcons(pIMenuCtl);
    /* check whether it is requested or minimum */
    if ( pMe->nQoSType == AEEPDP_QOS_GPRS_REQ ){    
      GPRSQoS = &pMe->m_GPRSQoSReq;
    } else if (pMe->nQoSType == AEEPDP_QOS_GPRS_MIN){
      GPRSQoS = &pMe->m_GPRSQoSMin;
    }else
      return FALSE;

    switch(GPRSQoS->reliability)
    {
               
    case AEEPDP_QOS_DELAY_CLASS_SUB :
      SetMenuIcon(pIMenuCtl, IDL_QOS_GPRS_REL_CLASS_SUB, TRUE);   
    break;

    case AEEPDP_QOS_REL_CLASS_1: 
      SetMenuIcon(pIMenuCtl, IDL_QOS_GPRS_REL_CLASS_1, TRUE); 
    break;

    case AEEPDP_QOS_REL_CLASS_2:
      SetMenuIcon(pIMenuCtl, IDL_QOS_GPRS_REL_CLASS_2, TRUE);
    break;

    case AEEPDP_QOS_REL_CLASS_3:
      SetMenuIcon(pIMenuCtl, IDL_QOS_GPRS_REL_CLASS_3, TRUE);
    break;

    default:
      DATASTATS_ERR("Uknown Reliability Class");            
    return FALSE;       
    }  
    szBuf = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
    if (szBuf == NULL)
      return FALSE;

    IMENUCTL_SetTitle(pIMenuCtl, DATASTATSAPP_RES_FILE, 0, pMe->profileText);

    CDataStatsApp_SetMenuLook(pMe->a.m_pIShell, IDC_MENU_EDIT_QOS_GPRS_REL_CLASS);
    CDataStatsApp_SetDefaultSoftkeyLook(pMe->a.m_pIShell, IDC_SK_QOS_GPRS_REL_CLASS);
    FREE(szBuf);
    return TRUE;

  case EVT_COMMAND:
    PRINT_EVENT(EVT_COMMAND);
    return CDataStatsApp_ExecuteItemAction(pMe, wParam);

  case EVT_KEY:
      
    switch(wParam)
    {
    case AVK_CLR:
      PRINT_EVENT(AVK_CLR);           
    return CDataStatsApp_EndDialog(pMe, NULL);

    default:
    break;
    }
    break;
  
  case EVT_CTL_SEL_CHANGED:
    PRINT_EVENT(EVT_CTL_SEL_CHANGED);
    pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);

    if(pIDialog != NULL)
    {
      if (((IControl*)dwParam) == IDIALOG_GetControl(pIDialog, IDC_MENU_EDIT_QOS_GPRS_REL_CLASS))
      {
        InitMenuIcons((IMenuCtl*) dwParam);
        SetMenuIcon((IMenuCtl*) dwParam, wParam, TRUE);
      }
    }
    else{
      DATASTATS_ERR("pIDialog NULL");
    }
    break;

  case EVT_DIALOG_END:
    PRINT_EVENT(EVT_DIALOG_END);
    IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_BACKGROUND,MAKE_RGB(255, 255, 255));
    IDISPLAY_Update(pMe->a.m_pIDisplay);
  return TRUE;

  default:
  break;
  }
#endif //(FEATURE_WCDMA) || (FEATURE_GSM_GPRS)
  return FALSE;
}
/*===========================================================================
FUNCTION CDataStatsApp_EditQoSGPRSPeakDRDlgHandler

DESCRIPTION
  The Edit QOS GPRS Peak Data rate Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditQoSGPRSPeakDRDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  IDialog *pIDialog = NULL;
  ITextCtl *pIText = NULL;
  IMenuCtl *pISoftKeyMenu  = NULL;
  IStatic *pIStatic = NULL;
  CDataStatsApp *pMe = (CDataStatsApp*) pUser;


  if(NULL == pMe)
  {
    DATASTATS_ERR("Null pMe");
    return FALSE;
  }
  switch(evt)
  {
  case EVT_DIALOG_START:
    PRINT_EVENT(EVT_DIALOG_START);
    pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);

    if(pIDialog == NULL)
    {
      return FALSE;   
    }
    // Get controls in dialog
    pIText = (ITextCtl*)IDIALOG_GetControl(pIDialog, 
                                          IDC_TEXT_QOS_GPRS_PEAK_DR);
    pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, 
                                              IDC_SK_QOS_GPRS_PEAK_DR);

    pIStatic = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_QOS_GPRS_PEAK_DR_TITLE);

    if((pIText == NULL) || (pISoftKeyMenu == NULL) || (pIStatic == NULL) )
    {
      DATASTATS_ERR("Null pointer");
      return FALSE; // error 
    }  
    ISTATIC_SetText(pIStatic, NULL, pMe->profileText, AEE_FONT_NORMAL, AEE_FONT_BOLD);
    ISTATIC_Redraw(pIStatic);

    // Update the softkey menu
    (void) IMENUCTL_DeleteAll(pISoftKeyMenu);
    IMENUCTL_AddItem(pISoftKeyMenu, DATASTATSAPP_RES_FILE, IDS_DD_OK, IDL_SK_QOS_GPRS_PEAK_DR_OK, NULL, 0);
    IMENUCTL_AddItem(pISoftKeyMenu, DATASTATSAPP_RES_FILE, IDS_DD_CANCEL, IDL_SK_QOS_GPRS_PEAK_DR_CANCEL, NULL, 0);          
        
    CDataStatsApp_SetDefaultSoftkeyLook(pMe->a.m_pIShell,IDC_SK_QOS_GPRS_PEAK_DR );

    ITEXTCTL_SetProperties(pIText, TP_FRAME );
    ITEXTCTL_SetMaxSize(pIText, AEEPDP_MAX_BR_STRING_LEN);
    (void) ITEXTCTL_SetInputMode(pIText, AEE_TM_NUMBERS);
    ITEXTCTL_SetActive(pIText, TRUE);

    ITEXTCTL_SetCursorPos(pIText, TC_CURSOREND);
    (void) ITEXTCTL_Redraw(pIText);

    (void) IDIALOG_SetFocus(pIDialog, IDC_TEXT_QOS_GPRS_PEAK_DR);
    IDIALOG_Redraw(pIDialog);
  return TRUE;

  case EVT_COMMAND:
    PRINT_EVENT(EVT_COMMAND);
  return CDataStatsApp_ExecuteItemAction(pMe, wParam);

  case EVT_KEY:      
    switch(wParam)
    {
    case AVK_CLR:
      PRINT_EVENT(AVK_CLR);           
    return CDataStatsApp_EndDialog(pMe, NULL);

    default:
    break;
    }
  break;

  case EVT_DIALOG_END:
    PRINT_EVENT(EVT_DIALOG_END);
    IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_BACKGROUND,MAKE_RGB(255, 255, 255));
    IDISPLAY_Update(pMe->a.m_pIDisplay);
  return TRUE;

  default:
  break;
  }
#endif //(FEATURE_WCDMA) || (FEATURE_GSM_GPRS)
  return FALSE;
}

/*===========================================================================
FUNCTION CDataStatsApp_EditQoSGPRSMeanDRDlgHandler

DESCRIPTION
  The Edit QOS GPRS Mean Data rate Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditQoSGPRSMeanDRDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  IDialog *pIDialog = NULL;
  ITextCtl *pIText = NULL;
  IMenuCtl *pISoftKeyMenu  = NULL;
  IStatic *pIStatic = NULL;
  CDataStatsApp *pMe = (CDataStatsApp*) pUser;


  if(NULL == pMe)
  {
    DATASTATS_ERR("Null pMe");
    return FALSE;
  }
  switch(evt)
  {
  case EVT_DIALOG_START:
    PRINT_EVENT(EVT_DIALOG_START);
    pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);

    if(pIDialog == NULL)
    {
      return FALSE;   
    }
    // Get controls in dialog
    pIText = (ITextCtl*)IDIALOG_GetControl(pIDialog, 
                                          IDC_TEXT_QOS_GPRS_MEAN_DR);
    pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pIDialog, 
                                              IDC_SK_QOS_GPRS_MEAN_DATA_RATE);

    pIStatic = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_QOS_GPRS_MEAN_DR_TITLE);

    if((pIText == NULL) || (pISoftKeyMenu == NULL) || (pIStatic == NULL) )
    {
      DATASTATS_ERR("Null pointer");
      return FALSE; // error 
    }   
    ISTATIC_SetText(pIStatic, NULL, pMe->profileText, AEE_FONT_NORMAL, AEE_FONT_BOLD);
    ISTATIC_Redraw(pIStatic);

    // Update the softkey menu
    (void) IMENUCTL_DeleteAll(pISoftKeyMenu);
    IMENUCTL_AddItem(pISoftKeyMenu, DATASTATSAPP_RES_FILE, IDS_DD_OK, IDL_SK_QOS_GPRS_MEAN_DR_OK, NULL, 0);
    IMENUCTL_AddItem(pISoftKeyMenu, DATASTATSAPP_RES_FILE, IDS_DD_CANCEL, IDL_SK_QOS_GPRS_MEAN_DR_CANCEL, NULL, 0);          
        
    CDataStatsApp_SetDefaultSoftkeyLook(pMe->a.m_pIShell,IDC_SK_QOS_GPRS_MEAN_DATA_RATE );


    ITEXTCTL_SetProperties(pIText, TP_FRAME );
    ITEXTCTL_SetMaxSize(pIText, AEEPDP_MAX_BR_STRING_LEN);
    (void) ITEXTCTL_SetInputMode(pIText, AEE_TM_NUMBERS);
    ITEXTCTL_SetActive(pIText, TRUE);

    ITEXTCTL_SetCursorPos(pIText, TC_CURSOREND);
    (void) ITEXTCTL_Redraw(pIText);

    (void) IDIALOG_SetFocus(pIDialog, IDC_TEXT_QOS_GPRS_MEAN_DR);
    IDIALOG_Redraw(pIDialog);
  return TRUE;

  case EVT_COMMAND:
    PRINT_EVENT(EVT_COMMAND);
  return CDataStatsApp_ExecuteItemAction(pMe, wParam);

  case EVT_KEY:
    switch(wParam)
    {
    case AVK_CLR:
      PRINT_EVENT(AVK_CLR);           
    return CDataStatsApp_EndDialog(pMe, NULL);

    default:
    break;
    }
  break;

  case EVT_DIALOG_END:
    PRINT_EVENT(EVT_DIALOG_END);
    IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_BACKGROUND,MAKE_RGB(255, 255, 255));
    IDISPLAY_Update(pMe->a.m_pIDisplay);
  return TRUE;

  default:
  break;
  }
#endif //(FEATURE_WCDMA) || (FEATURE_GSM_GPRS)
  return FALSE;
}
/*===========================================================================
FUNCTION CDataStatsApp_EndCallDlgHandler

DESCRIPTION
  The End Call Dialog (Single - from the Call Detail Screen) Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EndCallDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
    CDataStatsApp *pMe = (CDataStatsApp*) pUser;
    IDialog *pIDialog;
    IStatic *pIStaticCtl;
    if(NULL == pMe)
    {
        DATASTATS_ERR("Null pMe");
        return FALSE;
    }
    
    switch(evt)
    {
    case EVT_DIALOG_START:
        PRINT_EVENT(EVT_DIALOG_START);
        pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
        
        if(pIDialog!= NULL)
        {
          pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_END_CALL);
          if(pIStaticCtl != NULL)
          {
              ISTATIC_SetProperties(pIStaticCtl, ISTATIC_GetProperties(pIStaticCtl) | ST_ENABLETAB);
          }
          else
          {
              DATASTATS_ERR("pIStaticCtl NULL");
          }
        }
        else
        {
            DATASTATS_ERR("pIDiaog NULL");
        }
        CDataStatsApp_SetDefaultSoftkeyLook(pMe->a.m_pIShell, IDC_SK_END_CALL);
        return TRUE;

    case EVT_COMMAND:
        PRINT_EVENT(EVT_COMMAND);
        (void) CDataStatsApp_ExecuteItemAction(pMe, wParam);
        return TRUE;    

    case EVT_KEY:
        
        switch(wParam)
        {
        case AVK_CLR:
            PRINT_EVENT(AVK_CLR);
            return CDataStatsApp_EndDialog(pMe, NULL);
        
        default:
          break;
        }
        break;
    
    case EVT_DIALOG_END:
        PRINT_EVENT(EVT_DIALOG_END);
        //IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_SYS_WIN, MAKE_RGB(255,255,204));
        //IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_BACKGROUND, MAKE_RGB(255,255,204));
        IDISPLAY_Update(pMe->a.m_pIDisplay);
        
        return TRUE;

    default:
        break;
    }
    return FALSE;
}
/*===========================================================================
FUNCTION CDataStatsApp_EndAllDlgHandler

DESCRIPTION
  The End All Calls Dialog (from the Call summary Screen) Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EndAllDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
    CDataStatsApp *pMe = (CDataStatsApp*) pUser;
    uint16  nNumActiveCalls; // Number of active calls;
    int i;
    AEECMCallID CallIDs[MAX_NUM_DATA_CALLS];
    boolean bRetVal;

    if(NULL == pMe)
    {
        DATASTATS_ERR("Null pMe");
        return FALSE;
    }

    switch(evt)
    {
    case EVT_DIALOG_START:
        PRINT_EVENT(EVT_DIALOG_START);
        CDataStatsApp_SetMenuLook(pMe->a.m_pIShell, IDC_MENU_END_ALL);
        return TRUE;

    case EVT_COMMAND:
        
      switch(wParam)
        {
        case IDL_END_ALL_CALLS:
          PRINT_EVENT(IDL_END_ALL_CALLS);
          (void) CDataStatsApp_EndDialog(pMe, NULL);
          nNumActiveCalls = ICM_GetActiveCallIDs(pMe->m_pICM, 
            (AEECMCallType)(AEECM_CALL_TYPE_CS_DATA | AEECM_CALL_TYPE_PS_DATA), 
            (AEECMCallState)(AEECM_CALL_STATE_CONV|AEECM_CALL_STATE_DORMANT),
            CallIDs, sizeof(CallIDs));
          for(i = 0; i < nNumActiveCalls; i++)
          {
            bRetVal = CheckAEEReturnStatus(ICM_EndCall(pMe->m_pICM, CallIDs[i]));
            if(bRetVal == FALSE)
            {
              DATASTATS_ERR("Unable to end one of the calls");
              DBGPRINTF("Unable to end call_id : %d", CallIDs[i]);
            }
          }
          
          break;

        case IDL_END_ALL_SELECTED:
          PRINT_EVENT(IDL_END_ALL_SELECTED);
          (void) CDataStatsApp_EndDialog(pMe, NULL);
          bRetVal = CheckAEEReturnStatus(ICM_EndCall(pMe->m_pICM, pMe->call_id));
          if(bRetVal == FALSE)
          {
            DATASTATS_ERR("Unable to end the selected call");
            DBGPRINTF("Unable to end call_id : %d", pMe->call_id);
          }
          break;

        case IDL_END_ALL_CANCEL:
          PRINT_EVENT(IDL_END_ALL_CANCEL);
          (void) CDataStatsApp_EndDialog(pMe, NULL);
          break;

        default:
          DATASTATS_ERR("Unknown Selection");
          break;
        }
        return TRUE;    

    case EVT_KEY:
        switch(wParam)
        {
        case AVK_CLR:
            PRINT_EVENT(AVK_CLR);
            return CDataStatsApp_EndDialog(pMe, NULL);

        default:
          break;
        }
        break;
        
    
    case EVT_DIALOG_END:
        PRINT_EVENT(EVT_DIALOG_END);
        IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_BACKGROUND,MAKE_RGB(255, 255, 255));
        IDISPLAY_Update(pMe->a.m_pIDisplay);
        ISHELL_SetTimer(pMe->a.m_pIShell, 0, CDataStatsApp_RefreshSummaryMenu, (void*) pMe);
        return TRUE;

    default:
        break;
    }
    return FALSE;
}
/*===========================================================================
FUNCTION CDataStatsApp_EditSecurityNameDlgHandler

DESCRIPTION
  Edit Security Password for CHAP

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditCHAPPasswordDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  CDataStatsApp *pMe = (CDataStatsApp*) pUser;
  IDialog *pIDialog ;
  uint32 textProperties;
  ITextCtl *pITextCtl ;
  IMenuCtl *pIMenuCtl;
  AECHAR *szBuf;
  
  if(NULL == pMe)
  {
      DATASTATS_ERR("Null pMe");
      return FALSE;
  }

  switch(evt)
  {
  case EVT_DIALOG_START:
      PRINT_EVENT(EVT_DIALOG_START);

      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      
      if(pIDialog != NULL)
      {
        szBuf = (AECHAR*) MALLOC(sizeof(AECHAR)*(AEEPDP_MAX_QCPDP_STRING_LEN+1));
        if(szBuf == NULL)
        {
          DATASTATS_ERR("Malloc Failed");
          return FALSE;
        }
        pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, IDC_TEXT_CHAP_PASSWORD);
          
        if(pITextCtl != NULL)
        {
            (void) STRTOWSTR((char*) pMe->password, szBuf, sizeof(AECHAR)* AEEPDP_MAX_QCPDP_STRING_LEN+1);
            ITEXTCTL_SetText(pITextCtl, szBuf, AEEPDP_MAX_QCPDP_STRING_LEN);
            textProperties = ITEXTCTL_GetProperties(pITextCtl);
            ITEXTCTL_SetProperties(pITextCtl, textProperties | TP_FRAME | TP_PASSWORD & ~TP_NODRAW);
            ITEXTCTL_SetMaxSize(pITextCtl, AEEPDP_MAX_QCPDP_STRING_LEN);    
            ITEXTCTL_Redraw(pITextCtl);
        }
        else
        {
            DATASTATS_ERR("pITextCtl NULL");
        }
        FREE(szBuf);
   
        //Remove the multimap key.
        pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_SK_CHAP_PASSWORD);
        
        if(pIMenuCtl != NULL)
        {
            IMENUCTL_DeleteAll(pIMenuCtl);

            IMENUCTL_AddItem(pIMenuCtl, DATASTATSAPP_RES_FILE, IDS_DD_OK, \
                IDL_SK_CHAP_PASSWORD_OK, NULL, 0);

            IMENUCTL_AddItem(pIMenuCtl, DATASTATSAPP_RES_FILE, IDS_DD_CANCEL, \
                IDL_SK_CHAP_PASSWORD_CANCEL, NULL, 0);

            CDataStatsApp_SetDefaultSoftkeyLook(pMe->a.m_pIShell, IDC_SK_CHAP_PASSWORD);    
        }
        else
        {
            DATASTATS_ERR("SKCtl NULL");
        }
        
      }
      else
      {
          DATASTATS_ERR("pIDialog NULL");
      }

      IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_BACKGROUND,MAKE_RGB(255, 255, 255));
      IDISPLAY_Update(pMe->a.m_pIDisplay);
      return TRUE;

  case EVT_COMMAND:
      (void) CDataStatsApp_ExecuteItemAction(pMe, wParam);
      return TRUE;    

  case EVT_KEY:
      switch(wParam)
      {
      case AVK_CLR:
          PRINT_EVENT(AVK_CLR);
          return CDataStatsApp_EndDialog(pMe, NULL);

      default:
        break;
      }
      break;
      
  
  case EVT_DIALOG_END:
      PRINT_EVENT(EVT_DIALOG_END);
      //IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_BACKGROUND,MAKE_RGB(255, 255, 204));
      return TRUE;

  default:
      break;
  }
#endif
  return FALSE;
}

/*===========================================================================
FUNCTION CDataStatsApp_EditPAPParamsDlgHandler

DESCRIPTION
  Edit Security User Name and Password for PAP

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditPAPParamsDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  CDataStatsApp *pMe = (CDataStatsApp*) pUser;
  IDialog *pIDialog ;
  ITextCtl *pITextCtl;
  uint32 textProperties;
  IMenuCtl *pIMenuCtl;
  AECHAR *szBuf;
              
  if(NULL == pMe)
  {
      DATASTATS_ERR("Null pMe");
      return FALSE;
  }

  //Tab controller stuff..
  switch(evt)
  {
  case EVT_DIALOG_START:
    pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    if(pIDialog != NULL)
    {
      TabController_Init(&pMe->pTabController,3);
      TabController_AddControl(pMe->pTabController, TEXT_CTL, IDIALOG_GetControl(pIDialog, IDC_TEXT_PAP_USERNAME));
      TabController_AddControl(pMe->pTabController, TEXT_CTL, IDIALOG_GetControl(pIDialog, IDC_TEXT_PAP_PASSWORD));
      TabController_AddControl(pMe->pTabController, SK_CTL, IDIALOG_GetControl(pIDialog, IDC_SK_PAP_PARAMS));
      TabController_SetActiveEx(pMe->pTabController, 0);
    }
    else
    {
      DATASTATS_ERR("pIDialog NULL");        
    }
    break;

  default:
      TabController_HandleEvent(&pMe->pTabController, evt, wParam);
      break;

  }
  switch(evt)
  {
  case EVT_DIALOG_START:
      PRINT_EVENT(EVT_DIALOG_START);
      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      
      if(pIDialog != NULL)
      {
        szBuf = (AECHAR*) MALLOC((AEEPDP_MAX_QCPDP_STRING_LEN+1)*sizeof(AECHAR));
        if(szBuf == NULL)
        {
          DATASTATS_ERR("Malloc Failed");
          return FALSE;
        }  
        pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, IDC_TEXT_PAP_USERNAME);
          
        if(pITextCtl != NULL)
        {
          
          (void) STRTOWSTR((char*) pMe->username, szBuf, sizeof(AECHAR)* AEEPDP_MAX_QCPDP_STRING_LEN+1);
          ITEXTCTL_SetText(pITextCtl, szBuf, AEEPDP_MAX_QCPDP_STRING_LEN);
      
          textProperties = ITEXTCTL_GetProperties(pITextCtl);
          /* Set textctl properties
           * In BREW 3.1.3 and later, if we use the "new" text control,
           * we need to set the TP_FOCUS_NOSEL so that all of the text is not
           * selected when the text control takes focus.  AEECLSID_TEXTCTL is
           * the "new" text control for BREW 3.1.3 and later revisions.
           */
#if MIN_BREW_VERSIONEx(3,1,3)
          textProperties |= TP_FOCUS_NOSEL;
#endif
          ITEXTCTL_SetProperties(pITextCtl, textProperties | TP_FRAME & ~TP_NODRAW);
          ITEXTCTL_SetMaxSize(pITextCtl, AEEPDP_MAX_QCPDP_STRING_LEN);    
          //TODO: Load the current value of the PAP user-name/password
          ITEXTCTL_Redraw(pITextCtl);
        }
        else
        {
            DATASTATS_ERR("pITextCtl NULL");
        }
            
        pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, IDC_TEXT_PAP_PASSWORD);
        
        if(pITextCtl != NULL)
        {
            (void) STRTOWSTR((char*) pMe->password, szBuf, sizeof(AECHAR)* AEEPDP_MAX_QCPDP_STRING_LEN+1);
            ITEXTCTL_SetText(pITextCtl, szBuf, AEEPDP_MAX_QCPDP_STRING_LEN);

            textProperties = ITEXTCTL_GetProperties(pITextCtl);
            /* Set textctl properties
             * In BREW 3.1.3 and later, if we use the "new" text control,
             * we need to set the TP_FOCUS_NOSEL so that all of the text is not
             * selected when the text control takes focus.  AEECLSID_TEXTCTL is
             * the "new" text control for BREW 3.1.3 and later revisions.
             */
#if MIN_BREW_VERSIONEx(3,1,3)
            textProperties |= TP_FOCUS_NOSEL;
#endif
            ITEXTCTL_SetProperties(pITextCtl, textProperties | TP_FRAME | TP_PASSWORD & ~TP_NODRAW);
            ITEXTCTL_SetMaxSize(pITextCtl, AEEPDP_MAX_QCPDP_STRING_LEN);    
            ITEXTCTL_Redraw(pITextCtl);
        }
        else
        {
            DATASTATS_ERR("pITextCtl NULL");
        }
        FREE(szBuf);
        //Remove the multimap key.
        pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_SK_PAP_PARAMS);
          
        if(pIMenuCtl != NULL)
        {
            IMENUCTL_DeleteAll(pIMenuCtl);

            IMENUCTL_AddItem(pIMenuCtl, DATASTATSAPP_RES_FILE, IDS_DD_OK, \
                IDL_SK_PAP_PARAMS_OK, NULL, 0);

            IMENUCTL_AddItem(pIMenuCtl, DATASTATSAPP_RES_FILE, IDS_DD_CANCEL, \
                IDL_SK_PAP_PARAMS_CANCEL, NULL, 0);

            CDataStatsApp_SetDefaultSoftkeyLook(pMe->a.m_pIShell, IDC_SK_PAP_PARAMS);    
        }
        else
        {
            DATASTATS_ERR("SKCtl NULL");
        }
        
      }
      else
      {
          DATASTATS_ERR("pIDialog NULL");
      }
      
      IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_BACKGROUND,MAKE_RGB(255, 255, 255));
      return TRUE;

  case EVT_COMMAND:
      (void) CDataStatsApp_ExecuteItemAction(pMe, wParam);
      return TRUE;    

  case EVT_KEY:
      switch(wParam)
      {
      case AVK_CLR:
          PRINT_EVENT(AVK_CLR);
          return CDataStatsApp_EndDialog(pMe, NULL);

      default:
        break;
      }
      break;
  
  case EVT_DIALOG_END:
      PRINT_EVENT(EVT_DIALOG_END);
      //IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_BACKGROUND,MAKE_RGB(255, 255, 204));
      return TRUE;

  default:
      break;
  }
#endif
  return FALSE;
}
/*===========================================================================
FUNCTION CDataStatsApp_ErrorDlgHandler

DESCRIPTION
  The Error Dialog  Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_ErrorDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
    //TODO: Timeout for the error dlg handler.
    CDataStatsApp *pMe = (CDataStatsApp*) pUser;
    IDialog *pIDialog = NULL;
    IStatic *pIStaticCtl = NULL;
    
    if(NULL == pMe)
    {
        DATASTATS_ERR("Null pMe");
        return FALSE;
    }
    switch(evt)
    {
    case EVT_DIALOG_START:
        PRINT_EVENT(EVT_DIALOG_START);
        pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
        
        if(pIDialog != NULL)
        {
            pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_ERROR);
                
            if(pIStaticCtl != NULL)
            {
                ISTATIC_SetText(pIStaticCtl, NULL, pMe->szError, AEE_FONT_NORMAL, AEE_FONT_BOLD);
                ISTATIC_Redraw(pIStaticCtl);
            }
            else
            {
                DATASTATS_ERR("pIStaticCtl NULL");
            }
            CDataStatsApp_SetDefaultSoftkeyLook(pMe->a.m_pIShell, IDC_SK_ERROR);
            IDIALOG_SetFocus(pIDialog, IDC_SK_ERROR);
        }
        else
        {
            DATASTATS_ERR("pIDialog NULL");
        }
        return TRUE;

    case EVT_COMMAND:
        PRINT_EVENT(EVT_COMMAND);
        return CDataStatsApp_ExecuteItemAction(pMe, wParam);
        

    case EVT_KEY:
        
        switch(wParam)
        {
        case AVK_CLR:
            PRINT_EVENT(AVK_CLR);
            return CDataStatsApp_EndDialog(pMe, NULL);

        default:
          break;
        }
        break;
    
    case EVT_DIALOG_END:
        PRINT_EVENT(EVT_DIALOG_END);
        return TRUE;
        

    default:
        break;
    }
    return FALSE;
}
/*===========================================================================
FUNCTION CDataStatsApp_DDTMDlgHandler

DESCRIPTION
  The Error Dialog  Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_DDTMDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
/*lint -esym(715,evt)*/
/*lint -esym(715,pUser)*/
{
#ifdef FEATURE_DDTM_CNTL
  CDataStatsApp *pMe = (CDataStatsApp*) pUser;
  IDialog *pIDialog;

  if(NULL == pMe)
  {
      DATASTATS_ERR("Null pMe");
      return FALSE;
  }
  switch(evt)
  {
  case EVT_DIALOG_START:
      PRINT_EVENT(EVT_DIALOG_START);
      
      CDataStatsApp_SetDefaultSoftkeyLook(pMe->a.m_pIShell, IDC_SK_DDTM);
      CDataStatsApp_SetMenuLook(pMe->a.m_pIShell, IDC_MENU_DDTM);

      return CDataStatsApp_InitDDTMMenu(pMe);

  case EVT_COMMAND:
      PRINT_EVENT(EVT_COMMAND);
      return CDataStatsApp_ExecuteItemAction(pMe, wParam);

  case EVT_CTL_SEL_CHANGED:
      
      pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      
      if(pIDialog != NULL)
      {
        if (((IControl*)dwParam) == IDIALOG_GetControl(pIDialog, IDC_MENU_DDTM))
        {
            InitMenuIcons((IMenuCtl*) dwParam);
            SetMenuIcon((IMenuCtl*) dwParam, wParam, TRUE);
        }
      }
      else
      {
          DATASTATS_ERR("pIDialog NULL");
      }
      break;

  case EVT_KEY_PRESS:
      
      PRINT_EVENT(EVT_KEY_PRESS);
      pIDialog  = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if(pIDialog != NULL)
      {
        uint16 wSelId;
        CtlAddItem pai;
        IMenuCtl *pIMenuCtl = NULL;
        pIMenuCtl = (IMenuCtl *) IDIALOG_GetControl(pIDialog, IDC_MENU_DDTM);
          
        if(pIMenuCtl != NULL)
        {
          if(ICONTROL_IsActive((IControl*) pIMenuCtl) && (wParam == AVK_LEFT || wParam == AVK_RIGHT))
          {
            wSelId = IMENUCTL_GetSel(pIMenuCtl);
            IMENUCTL_GetItem(pIMenuCtl, wSelId, &pai);
            pai.wFont = AEE_FONT_BOLD;
            IMENUCTL_SetItem(pIMenuCtl,wSelId, MSIF_FONT, &pai);
            
            pIMenuCtl = (IMenuCtl *) IDIALOG_GetControl(pIDialog, IDC_SK_DDTM);
            IMENUCTL_SetSel(pIMenuCtl, IDL_DDTM_OK);
            //Set focus to SK control
            IDIALOG_SetFocus(pIDialog, IDC_SK_DDTM);
            return TRUE;
          }
          else if(wParam == AVK_DOWN || wParam == AVK_UP)
          {
            wSelId = IMENUCTL_GetSel(pIMenuCtl);
            IMENUCTL_GetItem(pIMenuCtl, wSelId, &pai);
            pai.wFont = AEE_FONT_NORMAL;
            IMENUCTL_SetItem(pIMenuCtl,wSelId, MSIF_FONT, &pai);
            IDIALOG_SetFocus(pIDialog, IDC_MENU_DDTM);
            return TRUE;
          }
        }
        else
        {
          DATASTATS_ERR("Unable to get menu");
        }
      }
      else
      {
          DATASTATS_ERR("Unable to get dialog");
      }
      break;

  case EVT_KEY:
      
      switch(wParam)
      {
      case AVK_CLR:
          PRINT_EVENT(AVK_CLR);
          return CDataStatsApp_EndDialog(pMe, NULL);

      default:
        break;
      }
      break;
  
  case EVT_DIALOG_END:
      PRINT_EVENT(EVT_DIALOG_END);
      return TRUE;
      

  default:
      break;
  }
#endif
  return FALSE;
}

/*===========================================================================
FUNCTION CDataStatsApp_EditPDPTypeDlgHandler

DESCRIPTION
  The Error Dialog  Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditPDPTypeDlgHandler(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  int nReturnStatus;
  CDataStatsApp * pMe = (CDataStatsApp*) pUser;
  IDialog *pIDialog;
  IMenuCtl* pIMenuCtl;

  if ( pMe == NULL ) { /* error checking */
    DATASTATS_ERR ("Null pointer");
    return FALSE;
  }


  switch (evt) {
    case EVT_DIALOG_START:
      PRINT_EVENT(EVT_DIALOG_START);
      pMe->m_oldPdpType = pMe->m_pdpType;

      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if(pIDialog == NULL) {
        DATASTATS_ERR ("Null pointer");
        return FALSE;
      }

      pIMenuCtl= (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_PDP_TYPE);
      if(pIMenuCtl == NULL) {
         DATASTATS_ERR ("Null pointer");
         return FALSE;
      }

      IMENUCTL_SetActive(pIMenuCtl,TRUE);
      /* Initialize radio button images with IDB_RADIO_UNFILLED */
      InitMenuIcons (pIMenuCtl);
  
      /* Update radio button image to reflect new setting */
      switch(pMe->m_pdpType)
      {
       case AEEPDP_IP:
         SetMenuIcon (pIMenuCtl, IDL_PDP_IPV4, TRUE);
         break;
       case AEEPDP_IPV6:
         SetMenuIcon (pIMenuCtl, IDL_PDP_IPV6, TRUE);
         break;
       case AEEPDP_IP_ANY:
         SetMenuIcon (pIMenuCtl, IDL_PDP_IP_ANY, TRUE);
         break;
       default:
         SetMenuIcon (pIMenuCtl, IDL_PDP_IPV4, TRUE);
         return FALSE;
      }
      CDataStatsApp_SetMenuLook(pMe->a.m_pIShell, IDC_PDP_TYPE);
      CDataStatsApp_SetDefaultSoftkeyLook(pMe->a.m_pIShell, IDC_PDP_SK_OK);
      return TRUE;

    case EVT_KEY:
      PRINT_EVENT(EVT_KEY);
      switch(wParam) {
        case AVK_CLR:
                pMe->m_pdpType = pMe->m_oldPdpType;
                nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
                return CheckAEEReturnStatus(nReturnStatus);

        case AVK_END:
          pMe->m_pdpType = pMe->m_oldPdpType;
          return FALSE;
        
        default:
          break;
      }
      return FALSE;
      
    case EVT_COMMAND:
      PRINT_EVENT(EVT_COMMAND);
      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if(pIDialog == NULL) {
        DATASTATS_ERR ("Null pointer");
        return FALSE;
      }

      pIMenuCtl= (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_PDP_TYPE);
      if(pIMenuCtl == NULL) {
         DATASTATS_ERR ("Null pointer");
         return FALSE;
      }

      if(IMENUCTL_IsActive(pIMenuCtl))
      {
        /* Initialize radio button images with IDB_RADIO_UNFILLED */
        InitMenuIcons (pIMenuCtl);
  
        /* Update radio button image to reflect new setting */
        SetMenuIcon (pIMenuCtl, wParam, TRUE);

        switch(wParam)
        {
          case IDL_PDP_IPV4:
            pMe->m_pdpType = AEEPDP_IP;
            break;
          case IDL_PDP_IPV6:
            pMe->m_pdpType = AEEPDP_IPV6;
            break;
          case IDL_PDP_IP_ANY:
            pMe->m_pdpType = AEEPDP_IP_ANY;
            break;
          default:
            pMe->m_pdpType = AEEPDP_IP;
            return FALSE;
        }
        return TRUE;
      }

      pIMenuCtl= (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_PDP_SK_OK);
      if (pIMenuCtl == NULL)
      {
        DATASTATS_ERR ("Null pointer");
        return FALSE;
      }

      return CDataStatsApp_ExecuteItemAction(pMe, wParam);

    case EVT_DIALOG_END:
      PRINT_EVENT(EVT_DIALOG_END);
      return TRUE;

    default:
      break;
  }
#endif
  return FALSE;

}


/*===========================================================================
FUNCTION CDataStatsApp_InitEditProfileName

DESCRIPTION
  The Data Statistics Dialog Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int CDataStatsApp_InitEditProfileName(CDataStatsApp *pMe)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  IDialog *pIDialog = NULL;
  ITextCtl *pITextCtl = NULL;
  IStatic *pIStatic = NULL;
  IMenuCtl* pISKMenuCtl = NULL;
  AEEPDP pdpProfile;
  uint32 dwFields;
  int nReturnStatus = AEE_SUCCESS;
  
  pIDialog = (IDialog*) ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      
  if(pIDialog != NULL)
  {
    do {
      // set up name buffer
      pdpProfile.profile_name_len = (IPDP_GetMaxProfileNameLen(pMe->m_pIPDP) + 1) * sizeof(AECHAR);
      pdpProfile.profile_name = (AECHAR*) MALLOC(pdpProfile.profile_name_len);
      if (NULL == pdpProfile.profile_name)
      {
        DATASTATS_ERR("No memory for profile name");
        nReturnStatus = EFAILED;
        break;
      }

    pIStatic = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_CURRENT_PROFILE);

    dwFields = AEEPDP_PROFILE_NAME;

    nReturnStatus = IPDP_GetParm(pMe->m_pIPDP, pMe->selectedProfile, dwFields, &pdpProfile);

    if (nReturnStatus != AEE_SUCCESS)
    {
      DATASTAT_ERROR_DLG( "Unable to read Profile info");
        break;
    }

    if(pIStatic != NULL)
    {
      ISTATIC_SetText(pIStatic, NULL, pdpProfile.profile_name, AEE_FONT_NORMAL, AEE_FONT_BOLD);
      ISTATIC_Redraw(pIStatic);
    }
    else
    {
       DATASTATS_ERR("pITextCtl NULL");
        nReturnStatus = EFAILED;
        break;
    }

    pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, IDC_TEXT_NEW_PROFILE_NAME);

    if(pITextCtl != NULL)
    {
      ITEXTCTL_SetProperties(pITextCtl, TP_FRAME);
        ITEXTCTL_SetMaxSize(pITextCtl, IPDP_GetMaxProfileNameLen(pMe->m_pIPDP));
      ITEXTCTL_SetText(pITextCtl, NULL,0);
      ITEXTCTL_Redraw(pITextCtl);
    }
    else
    {
       DATASTATS_ERR("pITextCtl NULL");
        nReturnStatus = EFAILED;
        break;
    }
  
    pISKMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_SK_DONE);
    if(pISKMenuCtl != NULL)
    {
      (void) IMENUCTL_DeleteAll(pISKMenuCtl);
      (void) IMENUCTL_AddItem(pISKMenuCtl, DATASTATSAPP_RES_FILE, IDS_DD_CANCEL,
                 IDL_SK_CANCEL, NULL, 0);
      (void) IMENUCTL_AddItem(pISKMenuCtl, DATASTATSAPP_RES_FILE, IDS_DD_OK,
               IDL_SK_DONE, NULL, 0); 
    }
    else
        nReturnStatus = EFAILED;

    } while (0);
  }
  else
  {
    DATASTATS_ERR("pIDialog NULL");
  }
  
  FREEIF(pdpProfile.profile_name);

  if (nReturnStatus != AEE_SUCCESS)
    return FALSE;
  
  CDataStatsApp_SetDefaultSoftkeyLook(pMe->a.m_pIShell, IDC_SK_DONE);
  ITEXTCTL_SetActive(pITextCtl, TRUE);
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS) */
  return TRUE;
}
/*===========================================================================
FUNCTION CDataStatsApp_EditProfileName

DESCRIPTION
  The Error Dialog  Event Handler 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CDataStatsApp_EditProfileName(void *pUser, AEEEvent evt, uint16 wParam, uint32 dwParam)
/*lint -esym(715,evt)*/
/*lint -esym(715,pUser)*/
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  CDataStatsApp *pMe = (CDataStatsApp*) pUser;
  
  if(NULL == pMe)
  {
      DATASTATS_ERR("Null pMe");
      return FALSE;
  }
  
  switch(evt)
  {
  case EVT_DIALOG_START:
      PRINT_EVENT(EVT_DIALOG_START);
      return CDataStatsApp_InitEditProfileName(pMe);

  case EVT_COMMAND:
      PRINT_EVENT(EVT_COMMAND);
      return CDataStatsApp_ExecuteItemAction(pMe, wParam);

  case EVT_KEY:
      
      switch(wParam)
      {
      case AVK_CLR:
          PRINT_EVENT(AVK_CLR);
          return CDataStatsApp_EndDialog(pMe, NULL);

      default:
        break;
      }
      break;
  
  case EVT_DIALOG_END:
      PRINT_EVENT(EVT_DIALOG_END);
      return TRUE;
      

  default:
      break;
  }
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS) */
  return FALSE;
}

static boolean CDataStatsApp_CreateDialog(CDataStatsApp *pMe, uint16 nDlgID)
{
    int nReturnStatus;
    PRINT_FUNCTION_NAME();

    ISHELL_CancelTimer(pMe->a.m_pIShell, CDataStatsApp_RefreshSummaryMenu, (void*) pMe);
    ISHELL_CancelTimer(pMe->a.m_pIShell, CDataStatsApp_RefreshDetailScreen, (void*) pMe);
    
    nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell,DATASTATSAPP_RES_FILE, nDlgID, NULL);
    return CheckAEEReturnStatus(nReturnStatus);
}
/*===========================================================================
FUNCTION CDataStatsApp_BranchDialog

DESCRIPTION
  Common function to pop-up a new dialog.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_BranchDialog(CDataStatsApp *pMe, const ITEMHANDLER *pItem)
{
    int nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell,\
        DATASTATSAPP_RES_FILE, pItem->data, NULL);
    PRINT_FUNCTION_NAME();
    return CheckAEEReturnStatus(nReturnStatus);
}

/*===========================================================================
FUNCTION CDataStatsApp_EndDialog

DESCRIPTION
  Common function to end the active dialog.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_EndDialog(CDataStatsApp *pMe, const ITEMHANDLER *pItem)
/*lint -esym(715,pItem)*/
{
    int nReturnStatus;
    IDialog *pIDialog;
    uint16 dlgId;

    PRINT_FUNCTION_NAME();

    nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
    pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    if(pIDialog != NULL)
    {
      dlgId = IDIALOG_GetID(pIDialog);
      if(IDD_CALL_DETAIL == dlgId)
      {
        ISHELL_SetTimer(pMe->a.m_pIShell, 0, CDataStatsApp_RefreshDetailScreen, (void*) pMe);
      }
      else if(IDD_CALL_SUMMARY == dlgId)
      {
        ISHELL_SetTimer(pMe->a.m_pIShell, 0, CDataStatsApp_RefreshSummaryMenu, (void*) pMe);
      }
    
      IDISPLAY_Update(pMe->a.m_pIDisplay);
      return CheckAEEReturnStatus(nReturnStatus);
    }
    else
    {
      ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
      return TRUE; // To close the applet.
    }
}

/*===========================================================================
FUNCTION CDataStatsApp_EndApplet

DESCRIPTION
  Function to end the applet

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_EndApplet(CDataStatsApp *pMe, const ITEMHANDLER *pItem)
/*lint -esym(715,pItem)*/
{
    int nReturnStatus;
    PRINT_FUNCTION_NAME();
    nReturnStatus = ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
    return CheckAEEReturnStatus(nReturnStatus);
}

/*===========================================================================
FUNCTION CDataStatsApp_SaveCHAPPassword

DESCRIPTION
Store the CHAP password in pMe.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_SaveCHAPPassword(CDataStatsApp *pMe, const ITEMHANDLER *pItem)
/*lint -esym(715,pItem)*/
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)    
  IDialog *pIDialog;
  AECHAR *szBuf = NULL;
  AECHAR *securityType = NULL;
  IMenuCtl *pIMenuCtl;
  
  PRINT_FUNCTION_NAME();

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  
  if(pIDialog != NULL)
  {
    szBuf = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
    securityType = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
    if(szBuf == NULL || securityType == NULL)
    {
      DATASTATS_ERR("Malloc Failed");
      FREEIF(szBuf);
      FREEIF(securityType);
      return FALSE;
    }
    
    (void) CDataStatsApp_StoreStringZ(pMe->password, pIDialog, IDC_TEXT_CHAP_PASSWORD);
    //End the two dialogs, edit_chap_password and security type selection
    (void) CDataStatsApp_EndDialog(pMe, NULL);
    (void) CDataStatsApp_EndDialog(pMe, NULL);
    pMe->nSecurity = AEEPDP_AUTH_CHAP;

    //Change the text for the Profile View Dialog.
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_CHAP, 
      securityType, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_SECURITY, 
      szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    (void) WSTRLCAT(szBuf, securityType, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    
    pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    
    if(pIDialog != NULL)
    {

      pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_VIEW_PROFILE);
      
      if(pIMenuCtl != NULL)
      {
          IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_SECURITY, DATASTATSAPP_RES_FILE, 0, szBuf);
          IMENUCTL_Redraw(pIMenuCtl);
          FREE(szBuf);
          FREE(securityType);
          return TRUE;
      }
      else
      {
          DATASTATS_ERR("pIMenuCtl NULL");
      }
    }
    else
    {
      DATASTATS_ERR("pIDialog NULL");
    }

    FREE(szBuf);
    FREE(securityType);

  }
  else
  {
    DATASTATS_ERR("pIDialog NULL");
  }
#endif
  return FALSE;
}

/*===========================================================================
FUNCTION CDataStatsApp_SavePAPParams

DESCRIPTION
    Store the PAP user name and password in pMe.


DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_SavePAPParams(CDataStatsApp *pMe, const ITEMHANDLER *pItem)
/*lint -esym(715,pItem)*/
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  IDialog *pIDialog;
  AECHAR *szBuf;
  AECHAR *securityType;
  IMenuCtl *pIMenuCtl;

  PRINT_FUNCTION_NAME();

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  
  if(pIDialog != NULL)
  {
    szBuf = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
    securityType = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
    if( (szBuf == NULL) ||(securityType == NULL))
    {
      DATASTATS_ERR("Malloc Failed");
      FREEIF(szBuf);
      FREEIF(securityType);
      return FALSE;
    }

    (void) CDataStatsApp_StoreStringZ(pMe->username, pIDialog, IDC_TEXT_PAP_USERNAME);
    (void) CDataStatsApp_StoreStringZ(pMe->password, pIDialog, IDC_TEXT_PAP_PASSWORD);
    //End the two dialogs, edit_pap_params and security type selection
    (void) CDataStatsApp_EndDialog(pMe, NULL);
    (void) CDataStatsApp_EndDialog(pMe, NULL);
    pMe->nSecurity = AEEPDP_AUTH_PAP;

    //Change the text for the Profile View Dialog.
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_PAP, 
      securityType, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_SECURITY, 
      szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    (void) WSTRLCAT(szBuf, securityType, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    
    pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    
    if(pIDialog != NULL)
    {

      pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_VIEW_PROFILE);
          
      if(pIMenuCtl != NULL)
      {
        IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_SECURITY, DATASTATSAPP_RES_FILE, 0, szBuf);
        IMENUCTL_Redraw(pIMenuCtl);
        FREE(szBuf);
        FREE(securityType);
        return TRUE;
      }
      else
      {
        DATASTATS_ERR("pIMenuCtl NULL");
      }
    }
    else
    {
      DATASTATS_ERR("pIDialog NULL");
    }
  
    FREE(szBuf);
    FREE(securityType);
              
  }
  else
  {
      DATASTATS_ERR("pIDialog NULL");
  }
#endif  
  return FALSE;
}

/*===========================================================================
FUNCTION CDataStatsApp_SetActiveProfile

DESCRIPTION
1. Store the currently selected profile as the profile to use for socket calls.
2. If this profile is not valid, set its content to default
3. End the currnet dialog.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_SetActiveProfile(CDataStatsApp *pMe, const ITEMHANDLER *pItem)
/*lint -esym(715,pItem)*/
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  boolean retVal = TRUE;
  IDialog *pIDialog;
  IMenuCtl *pIMenuCtl;
  uint8 activeProfile;
  int nReturnStatus;
  AEEPDP pdpProfile;
  byte profileNum;
  uint32 dwFields;

  PRINT_FUNCTION_NAME();
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  
  if(pIDialog != NULL)
  {
      pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_SELECT_ACTIVE);
      
      if(pIMenuCtl != NULL)
      {
          // Get the Selected Profile from the menuItem.
          activeProfile = (uint8)(IMENUCTL_GetSel(pIMenuCtl) - IDL_MENU_SELECT_ACTIVE_PROFILE1 + 1);
  
          /* Determine if this is a valid profile */
          profileNum = activeProfile;
          dwFields = AEEPDP_PROFILE_CONTEXT_VALID;
          
          nReturnStatus = IPDP_GetParm(pMe->m_pIPDP, profileNum, dwFields, &pdpProfile);
          /* Did it work? */
          if (CheckAEEReturnStatus(nReturnStatus) == FALSE) 
          {
              DATASTAT_ERROR_DLG("Unable to determine validity of the profile");
          }
          if (!pdpProfile.context.valid_flg) 
          {
            dwFields = AEEPDP_PROFILE_SET_TO_DEFAULT | AEEPDP_PROFILE_CONTEXT;  

            #ifdef FEATURE_DATA_PS_IPV6
            pdpProfile.context.pdp_addr.ipVersion = AEEPDP_IP_V4; 
            #endif

            nReturnStatus = IPDP_SetParm(pMe->m_pIPDP, profileNum, dwFields, &pdpProfile);

            if (CheckAEEReturnStatus(nReturnStatus)== FALSE) 
            {
                DATASTAT_ERROR_DLG("Unable to set default values for this profile");
                return FALSE;
            }
      
          }
          nReturnStatus = IPDP_SetDefaultProfileNum(pMe->m_pIPDP, profileNum);
          if(CheckAEEReturnStatus(nReturnStatus) == FALSE)
          {
              DATASTAT_ERROR_DLG( "Unable to save socket profile selection");
              return FALSE;
          }

      }
      else
      {
          DATASTATS_ERR("pIMenuCtl NULL");
      }
  }
  else
  {
      DATASTATS_ERR("pIDialog NULL");
  }
  // Go back to prev dialog.
  
  retVal = retVal & CDataStatsApp_EndDialog(pMe, NULL);

  return retVal;
#else
  return FALSE;
#endif
}
/*===========================================================================
FUNCTION CDataStatsApp_ViewProfile

DESCRIPTION
 1. Read the contents of the the selected profile.
 2. If not valid, set it to default 
 3. Read the contents.
 4. Set the variables in pMe, to be later read by the ViewProfileDlgHandler.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_ViewProfile(CDataStatsApp *pMe, const ITEMHANDLER *pItem)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  boolean retVal = TRUE;
  int nReturnStatus;

  byte profileNum;
  AEEPDP pdpProfile;
  uint32 dwFields;

  PRINT_FUNCTION_NAME();

  pMe->selectedProfile = (uint8)(pItem->data - IDS_DD_PROFILE1 + 1); // Profile number selected.
  profileNum = pMe->selectedProfile;

  dwFields = AEEPDP_PROFILE_CONTEXT_VALID;

  nReturnStatus = IPDP_GetParm(pMe->m_pIPDP, profileNum, dwFields, &pdpProfile);
  if(CheckAEEReturnStatus(nReturnStatus) == FALSE)
  {
      DATASTAT_ERROR_DLG( "Unable to read Profile info");
      return FALSE;
  }
  /* If this profile has never been initialized
      * then set each parameter to a default value */
  if ((!pdpProfile.context.valid_flg) && (CheckAEEReturnStatus(nReturnStatus) == TRUE)) 
  {
    dwFields = AEEPDP_PROFILE_SET_TO_DEFAULT | AEEPDP_PROFILE_CONTEXT;  
    #ifdef FEATURE_DATA_PS_IPV6
    pdpProfile.context.pdp_addr.ipVersion = AEEPDP_IP_V4; 
    #endif
    nReturnStatus = IPDP_SetParm(pMe->m_pIPDP, profileNum, dwFields, &pdpProfile);
  }
  
  if(CheckAEEReturnStatus(nReturnStatus) == FALSE)
  {
      DATASTAT_ERROR_DLG( "Unable to set default context");
      return FALSE;
  }
  
  /* Create the buffer for the profile name */
  pdpProfile.profile_name_len = (IPDP_GetMaxProfileNameLen(pMe->m_pIPDP) + 1) * sizeof(AECHAR);
  pdpProfile.profile_name = (AECHAR*) MALLOC(pdpProfile.profile_name_len);
  if (NULL == pdpProfile.profile_name) {
      DATASTAT_ERROR_DLG( "No memory for profile name");
      return FALSE;
    }

  /*Get all the required information */
  dwFields = AEEPDP_PROFILE_ALL_DATA;
  nReturnStatus = IPDP_GetParm(pMe->m_pIPDP, profileNum, dwFields, &pdpProfile);
  if(CheckAEEReturnStatus(nReturnStatus) == FALSE)
  {
      DATASTAT_ERROR_DLG( "Unable to read Profile info");
      FREE(pdpProfile.profile_name);
      return FALSE;
  }
  WSTRLCPY(pMe->apn, pdpProfile.context.apn, AEEPDP_MAX_APN_STRING_LEN+1);
  
  pMe->nIp = pdpProfile.context.pdp_addr.pdp_addr.pdp_addr_ipv4;
//  pMe->nIpv6 = pdpProfile.context.pdp_addr.pdp_addr_ipv6;
  pMe->m_pdpType = pdpProfile.context.pdp_type;

  pMe->nDnsPrimary.pdp_addr.pdp_addr_ipv4 = pdpProfile.dns_addr.primary_dns_addr.pdp_addr.pdp_addr_ipv4;
  pMe->nDnsSecondary.pdp_addr.pdp_addr_ipv4 = pdpProfile.dns_addr.secondary_dns_addr.pdp_addr.pdp_addr_ipv4;
  
  pMe->nSecurity = pdpProfile.auth.auth_type;
  (void) WSTRTOSTR(pdpProfile.auth.username, (char*) pMe->username, AEEPDP_MAX_QCPDP_STRING_LEN);
  (void) WSTRTOSTR(pdpProfile.auth.password, (char*) pMe->password, AEEPDP_MAX_QCPDP_STRING_LEN);
  
  MEMCPY((void *)&pMe->m_UMTSQoSReq, (void *)&pdpProfile.qos_request_umts, sizeof(AEEPDPUMTSQOS));
  MEMCPY((void *)&pMe->m_UMTSQoSMin, (void *)&pdpProfile.qos_minimum_umts, sizeof(AEEPDPUMTSQOS));
  MEMCPY((void *)&pMe->m_GPRSQoSReq, (void *)&pdpProfile.qos_request_gprs, sizeof(AEEPDPGPRSQOS));
  MEMCPY((void *)&pMe->m_GPRSQoSMin, (void *)&pdpProfile.qos_minimum_gprs, sizeof(AEEPDPGPRSQOS));
  // 2. Store the information in pMe
  // 3. Create the dialog
  // 4. The dialog has to read this information from pMe to initialize its members.
  
  
  //Set the title-string
    (void) MEMSET((void*)pMe->profileText, 0, pdpProfile.profile_name_len);
  WSTRLCPY(pMe->profileText, pdpProfile.profile_name, pdpProfile.profile_name_len);

  FREE(pdpProfile.profile_name);

  #ifdef FEATURE_DATA_IMS 
  // Copy PCSCF address
  pMe->m_pcscf_addr = pdpProfile.req_pcscf_addr;
    pMe->m_im_cn_flag = pdpProfile.im_cn_flag;
    pMe->m_dhcp_pcscf_addr = pdpProfile.dhcp_pcscf_addr;
  #endif //FEATURE_DATA_IMS
  return retVal & CDataStatsApp_CreateDialog(pMe, IDD_VIEW_PROFILE);
#else
  return FALSE;
#endif
}
/*===========================================================================
FUNCTION CDataStatsApp_BranchIPDialog

DESCRIPTION
  1. Store in pMe which type of IP address is selected
  2. Create the dialog, which will display the correct IP based on step 1.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_BranchIPDialog(CDataStatsApp *pMe, const ITEMHANDLER *pItem)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)

  PRINT_FUNCTION_NAME();
  switch(pItem->wItemId)
  {
  case IDL_MENU_VIEW_IP:
      pMe->ipTypeSelected = IP_ADDRESS;
      break;

  case IDL_MENU_VIEW_DNS_PRIMARY:
      pMe->ipTypeSelected = PRIMARY_DNS;
      break;

  case IDL_MENU_VIEW_DNS_SECONDARY:
      pMe->ipTypeSelected = SECONDARY_DNS;
      break;

  default:
      DATASTATS_ERR("Unknown Selection");
      break;
  }

  return CDataStatsApp_CreateDialog(pMe, pItem->data);
#else
  return FALSE;
#endif

}
/*===========================================================================
FUNCTION CDataStatsApp_SaveAPN

DESCRIPTION
  1. Store in pMe, the user-entered APN string.
  2. End the EditAPN dialog.
  3. Modify the text for the ViewProfile screen, as per new APN.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_SaveAPN(CDataStatsApp *pMe, const ITEMHANDLER *pItem)
/*lint -esym(715,pItem)*/
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  IDialog *pIDialog;
  ITextCtl *pITextCtl;
  IMenuCtl *pIMenuCtl = NULL;
  AECHAR *szBuf;

  PRINT_FUNCTION_NAME();

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  
  if(pIDialog != NULL)
  {
      
    pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, IDC_TEXT_EDIT_APN);
      
    if(pITextCtl != NULL)
    {

      szBuf = (AECHAR*) MALLOC(2*DATASTATS_MAX_BUFFER_SIZE*sizeof(AECHAR));
      if(szBuf == NULL)
      {
        DATASTATS_ERR("Malloc Failed");
        return FALSE;
      }

      ITEXTCTL_GetText(pITextCtl, pMe->apn, AEEPDP_MAX_APN_STRING_LEN);

      (void) CDataStatsApp_EndDialog(pMe, NULL);
      //Change the text for the Profile View Dialog.
      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
              
      if(pIDialog != NULL)
      {
        ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_APN, 
          szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
        (void) WSTRLCAT(szBuf, pMe->apn, DATASTATS_MAX_WIDE_BUFFER_SIZE);
        pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_VIEW_PROFILE);
        
        if(pIMenuCtl != NULL)
        {
      
          IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_APN, DATASTATSAPP_RES_FILE, 0, szBuf);
          IMENUCTL_Redraw(pIMenuCtl);
          FREE(szBuf);
          return TRUE;
        }
        else
        {
          DATASTATS_ERR("pIMenuCtl NULL");
        }
      }
      else
      {
        DATASTATS_ERR("pIDialog NULL");
      }
      FREE(szBuf);
    }
    else
    {
        DATASTATS_ERR("pITextCtl NULL");
    }

  }
  else
  {
    DATASTATS_ERR("pIDialog NULL");
  }
#endif
  return FALSE;
}

/*===========================================================================
FUNCTION CDataStatsApp_SavePDPType

DESCRIPTION
  1. Store in pMe, the PDP Type.
  2. End the PDPType dialog.
  3. Modify the text for the ViewProfile screen, as per new PDP Type.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_SavePDPType(CDataStatsApp *pMe, const ITEMHANDLER *pItem)
/*lint -esym(715,pItem)*/
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  IDialog *pIDialog;
  IMenuCtl *pIMenuCtl = NULL;
  AECHAR *szBuf = NULL;
  AECHAR *ipAddress = NULL;

  PRINT_FUNCTION_NAME();
  
  if(pItem->wItemId == IDL_PDP_SK_CANCEL)
  {
    pMe->m_pdpType = pMe->m_oldPdpType;
    ISHELL_EndDialog(pMe->a.m_pIShell);
  }
  else if(pItem->wItemId == IDL_PDP_SK_SAVE)
  {
    ISHELL_EndDialog(pMe->a.m_pIShell);
  }
  
  //Change the text for the Profile View Dialog.
   pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
              
   if(pIDialog != NULL)
   {
    szBuf  = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
    ipAddress = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);

    if(ipAddress == NULL || szBuf == NULL)
    {
      DATASTATS_ERR("Malloc Failed");
      FREEIF(ipAddress);
      FREEIF(szBuf);
      return FALSE;
    }

    pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_VIEW_PROFILE);
    if(pIMenuCtl == NULL)
    {
      DATASTATS_ERR("pIMenuCtl is NULL");
      FREEIF(szBuf);
      FREEIF(ipAddress);
      return TRUE;
    }

    if(pMe->m_pdpType == AEEPDP_IPV6)
    {
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_PDP_TYPE, 
        szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_PDP_IPV6, 
       ipAddress, DATASTATS_MAX_WIDE_BUFFER_SIZE);

      (void) WSTRLCAT(szBuf, ipAddress, DATASTATS_MAX_WIDE_BUFFER_SIZE); 
      IPV6ToAECHAR(pMe->nIpv6, ipAddress);
      IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_PDP_TYPE, DATASTATSAPP_RES_FILE, 0, szBuf);
      IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_IP, DATASTATSAPP_RES_FILE, 0, ipAddress);

      IMENUCTL_DeleteItem(pIMenuCtl, IDL_MENU_VIEW_DNS_PRIMARY);
      IMENUCTL_DeleteItem(pIMenuCtl, IDL_MENU_VIEW_DNS_SECONDARY);
    }
    else 
    {
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_PDP_TYPE, 
        szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

      if(pMe->m_pdpType == AEEPDP_IP){
        ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_IPV4, 
          ipAddress, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      }
      else
      {
        ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_PDP_ANY, 
          ipAddress, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      }
      (void) WSTRLCAT(szBuf, ipAddress, DATASTATS_MAX_WIDE_BUFFER_SIZE); 

      IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_PDP_TYPE, DATASTATSAPP_RES_FILE, 0, szBuf);

      IPToAECHAR(pMe->nIp, ipAddress);
      ISHELL_LoadResString(pMe->a .m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_IP, 
        szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
	  (void) WSTRLCAT(szBuf, ipAddress, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      (void) WSTRLCPY(ipAddress, szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_IP, DATASTATSAPP_RES_FILE, 0, ipAddress);
              
      IPToAECHAR(pMe->nDnsPrimary.pdp_addr.pdp_addr_ipv4, ipAddress);
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_DNS_PRIMARY, 
        szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
	  (void) WSTRLCAT(szBuf, ipAddress, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      (void) WSTRLCPY(ipAddress, szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_DNS_PRIMARY, DATASTATSAPP_RES_FILE, 0, ipAddress);
      
      IPToAECHAR(pMe->nDnsSecondary.pdp_addr.pdp_addr_ipv4, ipAddress);
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_DNS_SECONDARY,
        szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
	  (void) WSTRLCAT(szBuf, ipAddress, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      (void) WSTRLCPY(ipAddress, szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_DNS_SECONDARY, DATASTATSAPP_RES_FILE, 0, ipAddress);
     }   
     IMENUCTL_Redraw(pIMenuCtl);
     FREEIF(ipAddress);
     FREEIF(szBuf);
     return TRUE;
   }
   else
   {
     DATASTATS_ERR("pIDialog NULL");
   }
 #endif
  return FALSE;
}
/*===========================================================================
FUNCTION CDataStatsApp_SaveIP

DESCRIPTION
 1. Read the IP address entered and convert it to uint32
 2. End the current dialog.
 3. Based on type of IP (ip, dns-primary/secondary) , update the view
    profile screen.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_SaveIP(CDataStatsApp *pMe, const ITEMHANDLER *pItem)
/*lint -esym(715,pItem)*/
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  IDialog *pIDialog;
  ITextCtl *pITextCtl;
  IMenuCtl *pIMenuCtl;
  uint16 curSel;
  AECHAR *szBuf;
  AECHAR dotStr[2];
  AECHAR *tempBuf;
  AECHAR zeroStr[3];
  char ipText[4];
  uint32 ip = 0;
  AECharIP tempIp;

  PRINT_FUNCTION_NAME();
  
  (void) STRTOWSTR(".", dotStr, 3);
  (void) STRTOWSTR("00", zeroStr, 5); 
  
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  
  if(pIDialog == NULL)
  {
      DATASTATS_ERR("pDialog NULL");
      return FALSE;
  }
  szBuf = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
  tempBuf = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
  
  if(szBuf == NULL || tempBuf == NULL)
  {
    DATASTATS_ERR("Malloc Failed");
    FREEIF(szBuf);
    FREEIF(tempBuf);
    return FALSE;
  }
  pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, IDC_TEXT_EDIT_IP1);
  if(pITextCtl != NULL)
  {
      if(WSTRLEN(ITEXTCTL_GetTextPtr(pITextCtl)) != 0)
      {
          ITEXTCTL_GetText(pITextCtl, tempBuf,4);
          (void) WSTRLCPY(tempIp.IP1, tempBuf, sizeof(tempIp.IP1));
          (void) WSTRTOSTR(tempBuf, ipText, 4);
          ip = (uint32)((byte)(ATOI(ipText))<<24);
      }
      else
      {
          (void) WSTRLCPY(tempBuf, zeroStr, DATASTATS_MAX_WIDE_BUFFER_SIZE);
          (void) WSTRLCPY(tempIp.IP1, zeroStr,sizeof(tempIp.IP1));
          ip = 0;
      }
      (void) WSTRLCAT(tempBuf, dotStr, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  }
  else
  {
      DATASTATS_ERR("pITextCtl NULL");
  }

  pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, IDC_TEXT_EDIT_IP2);
  if(pITextCtl != NULL)
  {
  
      if(WSTRLEN(ITEXTCTL_GetTextPtr(pITextCtl)) != 0)
      {
          ITEXTCTL_GetText(pITextCtl, szBuf,4);
          (void) WSTRLCAT(tempBuf, szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
          (void) WSTRLCPY(tempIp.IP2, szBuf, sizeof(tempIp.IP2));
          
          (void) WSTRTOSTR(szBuf, ipText, 4);
          ip = ip | (uint32)((byte)(ATOI(ipText))<<16);
      }
      else
      {
          (void) WSTRLCAT(tempBuf, zeroStr, DATASTATS_MAX_WIDE_BUFFER_SIZE);
          (void) WSTRLCPY(tempIp.IP2, zeroStr, sizeof(tempIp.IP2));
      }
      (void) WSTRLCAT(tempBuf, dotStr, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  }
  else
  {
      DATASTATS_ERR("pITextCtl NULL");
  }

  pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, IDC_TEXT_EDIT_IP3);
  if(pITextCtl != NULL)
  {
  
      if(WSTRLEN(ITEXTCTL_GetTextPtr(pITextCtl)) != 0)
      {
          ITEXTCTL_GetText(pITextCtl, szBuf,4);
          (void) WSTRLCAT(tempBuf, szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
          (void) WSTRLCPY(tempIp.IP3, szBuf, sizeof(tempIp.IP3));

          (void) WSTRTOSTR(szBuf, ipText, 4);
          ip = ip | (uint32)((byte)(ATOI(ipText))<<8);
      }
      else
      {
          (void) WSTRLCAT(tempBuf, zeroStr, DATASTATS_MAX_WIDE_BUFFER_SIZE);
          (void) WSTRLCPY(tempIp.IP3, zeroStr, sizeof(tempIp.IP3));
      }
      (void) WSTRLCAT(tempBuf, dotStr, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  }
  else
  {
      DATASTATS_ERR("pITextCtl NULL");
  }

  pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, IDC_TEXT_EDIT_IP4);
  if(pITextCtl != NULL)
  {
  
      if(WSTRLEN(ITEXTCTL_GetTextPtr(pITextCtl)) != 0)
      {
          ITEXTCTL_GetText(pITextCtl, szBuf,4);
          (void) WSTRLCAT(tempBuf, szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
          (void) WSTRLCPY(tempIp.IP4, szBuf, sizeof(tempIp.IP4));

          (void) WSTRTOSTR(szBuf, ipText, 4);
          ip = ip | (uint32)(ATOI(ipText));
      }
      else
      {
          (void) WSTRLCAT(tempBuf, zeroStr, DATASTATS_MAX_WIDE_BUFFER_SIZE);
          (void) WSTRLCPY(tempIp.IP4, zeroStr, sizeof(tempIp.IP4));
      }
  }
  else
  {
      DATASTATS_ERR("pITextCtl NULL");
  }
  
  (void) CDataStatsApp_EndDialog(pMe, NULL);
  //Change the text for the Profile View Dialog.
  
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  
  if(pIDialog == NULL)
  {
    DATASTATS_ERR("pDialog NULL");
    FREE(szBuf);
    FREE(tempBuf);
    return FALSE;
  }
  
  pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_VIEW_PROFILE);
  if(NULL == pIMenuCtl)
  {
      DATASTATS_ERR("pIMenuCtl NULL");
      FREE(szBuf);
      FREE(tempBuf);
      return FALSE;
  }
  curSel = IMENUCTL_GetSel(pIMenuCtl);
  
  switch(curSel)
  {
  case IDL_MENU_VIEW_IP:
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_IP, 
        szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
	  (void) WSTRLCAT(szBuf, tempBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      (void) WSTRLCPY(tempBuf, szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      pMe->nIp = ip;
      break;

  case IDL_MENU_VIEW_DNS_PRIMARY:
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_DNS_PRIMARY, 
        szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
	  (void) WSTRLCAT(szBuf, tempBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      (void) WSTRLCPY(tempBuf, szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      pMe->nDnsPrimary.pdp_addr.pdp_addr_ipv4 = ip;
      break;

  case IDL_MENU_VIEW_DNS_SECONDARY:
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_DNS_SECONDARY, 
        szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
	  (void) WSTRLCAT(szBuf, tempBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      (void) WSTRLCPY(tempBuf, szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      pMe->nDnsSecondary.pdp_addr.pdp_addr_ipv4 = ip;
      break;

  default:
      DATASTATS_ERR("Unknown  Selection");
      return FALSE;
  }

  IMENUCTL_SetItemText(pIMenuCtl, curSel, DATASTATSAPP_RES_FILE, 0, tempBuf);
  IMENUCTL_Redraw(pIMenuCtl);
  FREE(szBuf);
  FREE(tempBuf);
  return TRUE;
#else
  return FALSE;
#endif
}
/*===========================================================================
FUNCTION CDataStatsApp_SaveSecurity

DESCRIPTION
1. Save the user-selected security
2. End the edit Security dialog and update the view profile screen.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_SaveSecurity(CDataStatsApp *pMe, const ITEMHANDLER *pItem)
/*lint -esym(715,pItem)*/
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  IDialog *pIDialog;
  IMenuCtl *pIMenuCtl;
  AECHAR *szBuf = NULL;
  AECHAR *securityType = NULL;
  uint16 curSel;

  PRINT_FUNCTION_NAME();
  
  szBuf = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
  securityType = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
  if(szBuf == NULL || securityType == NULL)
  {
    DATASTATS_ERR("Malloc Failed");
    FREEIF(szBuf);
    FREEIF(securityType);
    return FALSE;
  }
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  
  if(pIDialog != NULL)
  {
      pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_EDIT_SECURITY);
      
      if(pIMenuCtl != NULL)
      {
  
          curSel = IMENUCTL_GetSel(pIMenuCtl);
  
          switch(curSel)
          {
          case IDL_SECURITY_NONE:
              pMe->nSecurity = AEEPDP_AUTH_NONE;
              
              ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_NONE, 
                securityType, DATASTATS_MAX_WIDE_BUFFER_SIZE);
              ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_SECURITY, 
                szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
              (void) WSTRLCAT(szBuf, securityType, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      
              (void) CDataStatsApp_EndDialog(pMe, NULL);
              //Change the text for the Profile View Dialog.
              pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
              
              if(pIDialog != NULL)
              {

                  pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_VIEW_PROFILE);
                  
                  if(pIMenuCtl != NULL)
                  {
                      IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_SECURITY, DATASTATSAPP_RES_FILE, 0, szBuf);
                      IMENUCTL_Redraw(pIMenuCtl);
                      FREE(szBuf);
                      FREE(securityType);
                      return TRUE;
                  }
                  else
                  {
                      DATASTATS_ERR("pIMenuCtl NULL");
                  }
              }
              else
              {
                  DATASTATS_ERR("pIDialog NULL");
              }
              
              break;

          case IDL_SECURITY_CHAP:
              FREE(szBuf);
              FREE(securityType);
              return CDataStatsApp_CreateDialog(pMe, IDD_EDIT_CHAP_PASSWORD);
              
          case IDL_SECURITY_PAP:
              FREE(szBuf);
              FREE(securityType);
              return CDataStatsApp_CreateDialog(pMe, IDD_EDIT_PAP_PARAMS);

          default:
            DATASTATS_ERR("Unknown selection");            
            return FALSE;
          }
          
      }
      else
      {
          DATASTATS_ERR("pIMenuCtl NULL");
      }
  }
  else
  {
      DATASTATS_ERR("pIDialog NULL");
  }
  FREEIF(szBuf);
  FREEIF(securityType);
#endif
  
  return FALSE;

}
#ifdef FEATURE_DATA_IMS 
/*===========================================================================
FUNCTION CDataStatsApp_SavePCSCFRequest

DESCRIPTION
1. Save the user-selected value for PCSCF Addr request
2. End the edit Security dialog and update the view profile screen.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_SavePCSCFRequest(CDataStatsApp *pMe, const ITEMHANDLER *pItem)
/*lint -esym(715,pItem)*/
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  IDialog *pIDialog;
  IMenuCtl *pIMenuCtl;
  uint16 curSel;
  AECHAR *szBuf = NULL;
  AECHAR *pcscf_addr_sel = NULL;  

  PRINT_FUNCTION_NAME();
    
  szBuf = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
  pcscf_addr_sel = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);

  if (szBuf == NULL || pcscf_addr_sel == NULL) 
  {
    DATASTATS_ERR("Malloc Failed");
    FREEIF(szBuf);   
    FREEIF(pcscf_addr_sel);
    return FALSE;
  }

  (void) ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_REQUEST_PCSCF, 
                                               szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  
  if(pIDialog != NULL)
  {
    pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_EDIT_PCSCF);
      
    if(pIMenuCtl != NULL)
    {  
      curSel = IMENUCTL_GetSel(pIMenuCtl);
  
      switch(curSel)
      {
        case IDL_EDIT_PCSCF_TRUE:
          pMe->m_pcscf_addr = TRUE;  
          (void) ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_PCSCF_TRUE , 
              pcscf_addr_sel, DATASTATS_MAX_WIDE_BUFFER_SIZE);
          break;

        case IDL_EDIT_PCSCF_FALSE:               
        default:
          pMe->m_pcscf_addr = FALSE;                             
          (void) ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_PCSCF_FALSE, 
            pcscf_addr_sel, DATASTATS_MAX_WIDE_BUFFER_SIZE);
          break;
      } //end of switch(curSel)  
        
      //Concatenate the two strings
      WSTRLCAT(szBuf,pcscf_addr_sel,DATASTATS_MAX_WIDE_BUFFER_SIZE);
      (void) CDataStatsApp_EndDialog(pMe, NULL);              
      //Change the text for the Profile View Dialog.
      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
              
      if(pIDialog != NULL)
      {
        pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_VIEW_PROFILE);
                  
        if(pIMenuCtl != NULL)
        {
          IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_PCSCF_ADDR, DATASTATSAPP_RES_FILE, 0, szBuf);
          IMENUCTL_Redraw(pIMenuCtl);            
          FREEIF(szBuf);
          FREEIF(pcscf_addr_sel);
          return TRUE;
        } //end of if(pIMenuCtl != NULL)
        else
        {
          DATASTATS_ERR("pIMenuCtl NULL");
        }
      } //end of if(pIDialog != NULL)
      else
      {
        DATASTATS_ERR("pIDialog NULL");
      } //end of if(pIDialog != NULL) else part   
    }  // end if(pIMenuCtl != NULL)
    else
    {
      DATASTATS_ERR("pIMenuCtl NULL");
    }
  } 
  else
  {
    DATASTATS_ERR("pIDialog NULL");
  } //end of if(pIDialog != NULL)
 
  FREEIF(szBuf);
  FREEIF(pcscf_addr_sel);
#else
  /*lint -esym(715,pMe)*/
#endif  
  return FALSE;
}
#endif //FEATURE_DATA_IMS

#ifdef FEATURE_DATA_IMS 
/*===========================================================================
FUNCTION CDataStatsApp_SaveDHCPPCSCFRequest

DESCRIPTION
1. Save the user-selected value for DHCP PCSCF Addr request
2. End the edit Security dialog and update the view profile screen.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_SaveDHCPPCSCFRequest(CDataStatsApp *pMe, const ITEMHANDLER *pItem)
/*lint -esym(715,pItem)*/
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  IDialog *pIDialog;
  IMenuCtl *pIMenuCtl;
  uint16 curSel;
  AECHAR *szBuf = NULL;
  AECHAR *dhcp_pcscf_addr_sel = NULL;  

  PRINT_FUNCTION_NAME();
    
  szBuf = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
  dhcp_pcscf_addr_sel = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);

  if (szBuf == NULL || dhcp_pcscf_addr_sel == NULL) 
  {
    DATASTATS_ERR("Malloc Failed");
    FREEIF(szBuf);   
    FREEIF(dhcp_pcscf_addr_sel);
    return FALSE;
  }
  
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  
  if(pIDialog != NULL)
  {
    pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_DHCP_PCSCF);
      
    if(pIMenuCtl != NULL)
    {  
      curSel = IMENUCTL_GetSel(pIMenuCtl);
  
      switch(curSel)
      {
        case IDL_EDIT_DHCP_PCSCF_TRUE:
          pMe->m_dhcp_pcscf_addr = TRUE;  
          (void) ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_PCSCF_TRUE, 
              dhcp_pcscf_addr_sel, DATASTATS_MAX_WIDE_BUFFER_SIZE);
          break;

        case IDL_EDIT_DHCP_PCSCF_FALSE:               
        default:
          pMe->m_dhcp_pcscf_addr = FALSE;                             
          (void) ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_PCSCF_FALSE, 
            dhcp_pcscf_addr_sel, DATASTATS_MAX_WIDE_BUFFER_SIZE);
          break;
      } //end of switch(curSel)  
        
      (void) ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_REQUEST_DHCP_PCSCF, 
                                               szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      //Concatenate the two strings
      (void) WSTRLCAT(szBuf,dhcp_pcscf_addr_sel,DATASTATS_MAX_WIDE_BUFFER_SIZE);
      (void) CDataStatsApp_EndDialog(pMe, NULL);              
      //Change the text for the Profile View Dialog.
      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
              
      if(pIDialog != NULL)
      {
        pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_VIEW_PROFILE);
                  
        if(pIMenuCtl != NULL)
        {
          IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_DHCP_PCSCF_FLAG, DATASTATSAPP_RES_FILE, 0, szBuf);
          IMENUCTL_Redraw(pIMenuCtl);            
          FREEIF(szBuf);
          FREEIF(dhcp_pcscf_addr_sel);
          return TRUE;
        } //end of if(pIMenuCtl != NULL)
        else
        {
          DATASTATS_ERR("pIMenuCtl NULL");
        }
      } //end of if(pIDialog != NULL)
      else
      {
        DATASTATS_ERR("pIDialog NULL");
      } //end of if(pIDialog != NULL) else part   
    }  // end if(pIMenuCtl != NULL)
    else
    {
      DATASTATS_ERR("pIMenuCtl NULL");
    }
  } 
  else
  {
    DATASTATS_ERR("pIDialog NULL");
  } //end of if(pIDialog != NULL)
 
  FREEIF(szBuf);
  FREEIF(dhcp_pcscf_addr_sel);
#else
  /*lint -esym(715,pMe)*/
#endif  
  return FALSE;
}
#endif //FEATURE_DATA_IMS

#ifdef FEATURE_DATA_IMS 
/*===========================================================================
FUNCTION CDataStatsApp_SaveIMCNFlag

DESCRIPTION
1. Save the user-selected value for IM CN Flag 
2. End the edit Security dialog and update the view profile screen.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_SaveIMCNFlag(CDataStatsApp *pMe, const ITEMHANDLER *pItem)
/*lint -esym(715,pItem)*/
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  IDialog *pIDialog;
  IMenuCtl *pIMenuCtl;
  uint16 curSel;
  AECHAR *szBuf = NULL;
  AECHAR *im_cn_flag_sel = NULL;  

  PRINT_FUNCTION_NAME();
    
  szBuf = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
  im_cn_flag_sel = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);

  if (szBuf == NULL || im_cn_flag_sel == NULL) 
  {
    DATASTATS_ERR("Malloc Failed");
    FREEIF(szBuf);   
    FREEIF(im_cn_flag_sel);
    return FALSE;
  }
  
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);  
  if(pIDialog != NULL)
  {
    pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_IM_CN_FLAG);      
    if(pIMenuCtl != NULL)
    {  
      curSel = IMENUCTL_GetSel(pIMenuCtl);  
      switch(curSel)
      {
        case IDL_EDIT_IM_CN_FLAG_TRUE:
          pMe->m_im_cn_flag = TRUE;  
          (void) ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_PCSCF_TRUE , 
              im_cn_flag_sel, DATASTATS_MAX_WIDE_BUFFER_SIZE);
          break;

        case IDL_EDIT_IM_CN_FLAG_FALSE:               
        default:
          pMe->m_im_cn_flag = FALSE;                             
          (void) ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_PCSCF_FALSE, 
            im_cn_flag_sel, DATASTATS_MAX_WIDE_BUFFER_SIZE);
          break;
      } //end of switch(curSel)  
        
      (void) ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_IM_CN_FLAG, 
                                               szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      //Concatenate the two strings
      (void) WSTRLCAT(szBuf,im_cn_flag_sel,DATASTATS_MAX_WIDE_BUFFER_SIZE);
      (void) CDataStatsApp_EndDialog(pMe, NULL);              
      //Change the text for the Profile View Dialog.
      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
              
      if(pIDialog != NULL)
      {
        pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_VIEW_PROFILE);
                  
        if(pIMenuCtl != NULL)
        {
          IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_IMCN_FLAG, DATASTATSAPP_RES_FILE, 0, szBuf);
          IMENUCTL_Redraw(pIMenuCtl);            
          FREEIF(szBuf);
          FREEIF(im_cn_flag_sel);
          return TRUE;
        } //end of if(pIMenuCtl != NULL)
        else
        {
          DATASTATS_ERR("pIMenuCtl NULL");
        }
      } //end of if(pIDialog != NULL)
      else
      {
        DATASTATS_ERR("pIDialog NULL");
      } //end of if(pIDialog != NULL) else part   
    }  // end if(pIMenuCtl != NULL)
    else
    {
      DATASTATS_ERR("pIMenuCtl NULL");
    }
  } 
  else
  {
    DATASTATS_ERR("pIDialog NULL");
  } //end of if(pIDialog != NULL)
 
  FREEIF(szBuf);
  FREEIF(im_cn_flag_sel);
#else
  /*lint -esym(715,pMe)*/
#endif  
  return FALSE;
}
#endif //FEATURE_DATA_IMS

/*===========================================================================
FUNCTION CDataStatsApp_SaveTrafficClass

DESCRIPTION
1. Save the user-selected traffic class
2. End the edit traffic class dialog and update the QOS UMTS screen.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_SaveTrafficClass(CDataStatsApp *pMe, const ITEMHANDLER *pItem)
/*lint -esym(715,pItem)*/
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  IDialog *pIDialog = NULL;
  IMenuCtl *pIMenuCtl = NULL;
  AECHAR *TrafficClass = NULL;
  AECHAR *szBuf = NULL ;
  uint16 curSel = 0;
  AEEPDPUMTSQOS *UMTSQoS = NULL;

  PRINT_FUNCTION_NAME();
  
  szBuf = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
  TrafficClass = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
  if (TrafficClass == NULL || szBuf == NULL)
  {
    DATASTATS_ERR("Malloc Failed");
    return FALSE;
  }
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  
  if (pIDialog == NULL ){
    FREE(szBuf);
    FREE(TrafficClass);
    DATASTATS_ERR("pIDialog NULL");
    return FALSE;
  }
  pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_EDIT_QOS_UMTS_TC);
      
  if(pIMenuCtl == NULL){
    FREE(szBuf);
    FREE(TrafficClass);
    DATASTATS_ERR("pIMenuCtl NULL");
    return FALSE;
  }
   curSel = IMENUCTL_GetSel(pIMenuCtl);

   if( pMe->nQoSType == AEEPDP_QOS_UMTS_REQ )
    UMTSQoS = &pMe->m_UMTSQoSReq;
   else if ( pMe->nQoSType == AEEPDP_QOS_UMTS_MIN )
    UMTSQoS = &pMe->m_UMTSQoSMin;
  else{
    FREE(szBuf);
    FREE(TrafficClass);
    return FALSE;
  }
  switch(curSel) {
  case IDL_QOS_UMTS_TC_SUBSCRIBE:
    UMTSQoS->traffic_class = AEEPDP_QOS_TCLASS_SUBSCRIBE;

    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_UMTS_TC_SUBSCRIBE, 
            TrafficClass, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  break;

  case IDL_QOS_UMTS_TC_CONV:
    UMTSQoS->traffic_class = AEEPDP_QOS_TCLASS_CONV;
        
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_UMTS_TC_CONV, 
          TrafficClass, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  break;
          
  case IDL_QOS_UMTS_TC_STREAM:
    UMTSQoS->traffic_class = AEEPDP_QOS_TCLASS_STRM;

    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_UMTS_TC_STREAM, 
      TrafficClass, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  break;

  case IDL_QOS_UMTS_TC_INTERACTIVE:
    UMTSQoS->traffic_class = AEEPDP_QOS_TCLASS_INTR;

    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_UMTS_TC_INTERACTIVE, 
      TrafficClass, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  break;

  case IDL_QOS_UMTS_TC_BACKGROUND:
    UMTSQoS->traffic_class = AEEPDP_QOS_TCLASS_BACK;

    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_UMTS_TC_BACKGROUND, 
      TrafficClass, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  break;

  default:
    DATASTATS_ERR("Uknown selection");            
    FREE(szBuf);
    FREE(TrafficClass);
  return FALSE;
  }  

  (void) CDataStatsApp_EndDialog(pMe, NULL);
  //Change the text for the View Qos Dialog.
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
          
  if(pIDialog == NULL){
    DATASTATS_ERR("pIDialog NULL"); 
    FREE(szBuf);
    FREE(TrafficClass);
    return FALSE;
  }
  ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_UMTS_TC, 
    szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  (void) WSTRLCAT(szBuf, TrafficClass, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_EDIT_QOS_UMTS);
        
  if(pIMenuCtl == NULL){
    DATASTATS_ERR(" pIMenuCtl NULL"); 
    FREE(szBuf);
    FREE(TrafficClass);
    return FALSE;
  }
  IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_QOS_UMTS_TC, DATASTATSAPP_RES_FILE, 0, szBuf);
  IMENUCTL_Redraw(pIMenuCtl);
  FREE(szBuf);
  FREE(TrafficClass);
  return TRUE;
       
#else
  return FALSE;
#endif 
}

/*===========================================================================
FUNCTION CDataStatsApp_SaveQoSUMTS

DESCRIPTION
1. Save the user-selected QoS UMTS
2. End the edit QOS dialog and update the Edit QOS screen.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_SaveQoSUMTS(CDataStatsApp *pMe, const ITEMHANDLER *pItem)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  IDialog *pIDialog = NULL;
  IMenuCtl *pIMenuCtl = NULL;
  uint16   wDlgID = 0;

  PRINT_FUNCTION_NAME();
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  
  if (pIDialog == NULL ){
    DATASTATS_ERR("pIDialog NULL");
    return FALSE;
  }

  wDlgID = IDIALOG_GetID(pIDialog);

  if (wDlgID == IDD_EDIT_QOS_UMTS){
    switch(pMe->nQoSType) {
    case AEEPDP_QOS_UMTS_REQ:
      pMe->m_UMTSQoSReq.valid_flg = TRUE;
    break;
    case AEEPDP_QOS_UMTS_MIN:
      pMe->m_UMTSQoSMin.valid_flg = TRUE;
    break;
    default:
    break;
    }  
    
    (void) CDataStatsApp_EndDialog(pMe, NULL);
    (void) CDataStatsApp_EndDialog(pMe, NULL);
    //Change the text for the Profile View Dialog.
    pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
        
    if(pIDialog == NULL){
      DATASTATS_ERR("pIDialog NULL");
      return FALSE;
    }

    pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_VIEW_PROFILE);

    if(pIMenuCtl == NULL){
      DATASTATS_ERR("pIMenuCtl NULL");
      return FALSE;  
    }
    IMENUCTL_Redraw(pIMenuCtl);
  }
  return TRUE;
#else
  return FALSE;
#endif 
}
/*===========================================================================
FUNCTION CDataStatsApp_SaveQoSType

DESCRIPTION
1. Save the user-selected QoS Type
2. End the edit QOS dialog and update the Edit QOS screen.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_SaveQoSType(CDataStatsApp *pMe, const ITEMHANDLER *pItem)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  IDialog *pIDialog = NULL;
  IMenuCtl *pIMenuCtl = NULL;
  AECHAR *szBuf = NULL;
  AECHAR *wBuf = NULL;
  uint16 curSel = 0;

  PRINT_FUNCTION_NAME();

  szBuf = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
  wBuf = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
  if (szBuf == NULL || wBuf == NULL )
  {
    DATASTATS_ERR("Malloc Failed");
    FREEIF(szBuf);
    FREEIF(wBuf);
    return FALSE;
  }
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  
  if (pIDialog == NULL )
  {
    FREE(szBuf);
    FREE(wBuf);
    DATASTATS_ERR("pIDialog NULL");
    return FALSE;
  }
  pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_EDIT_QOS);

  if(pIMenuCtl == NULL){
    FREE(szBuf);
    FREE(wBuf);
    DATASTATS_ERR("pIMenuCtl NULL");
    return FALSE;
  }
  curSel = IMENUCTL_GetSel(pIMenuCtl);

  switch(curSel) {
  case IDL_MENU_EDIT_QOS_UMTS_REQ:
    pMe->nQoSType = AEEPDP_QOS_UMTS_REQ;
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_UMTS_REQ, 
              szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  break;
  case IDL_MENU_EDIT_QOS_UMTS_MIN:
    pMe->nQoSType = AEEPDP_QOS_UMTS_MIN;
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_UMTS_MIN, 
              szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  break;

  case IDL_MENU_EDIT_QOS_GPRS_REQ:
    pMe->nQoSType = AEEPDP_QOS_GPRS_REQ;
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_REQ, 
          szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  break;
  case IDL_MENU_EDIT_QOS_GPRS_MIN:
    pMe->nQoSType = AEEPDP_QOS_GPRS_MIN;
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_MIN, 
          szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  break;
  default:
    pMe->nQoSType = AEEPDP_QOS_NONE;      
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_NONE, 
        szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  }  
  IMENUCTL_SetItemText(pIMenuCtl, IDC_MENU_EDIT_QOS, DATASTATSAPP_RES_FILE, 0, szBuf);
  IMENUCTL_Redraw(pIMenuCtl);

  (void) CDataStatsApp_EndDialog(pMe, NULL);
  //Change the text for the Profile View Dialog.
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      
  if(pIDialog == NULL)
  {
    FREE(szBuf);
    FREE(wBuf);
    DATASTATS_ERR("pIDialog NULL");
    return FALSE;
  }
  ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS, 
    wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
 
  if(pMe->nQoSType == AEEPDP_QOS_UMTS_REQ || pMe->nQoSType == AEEPDP_QOS_UMTS_MIN)
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_UMTS, 
        szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  else if(pMe->nQoSType == AEEPDP_QOS_GPRS_REQ || pMe->nQoSType == AEEPDP_QOS_GPRS_MIN)
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS, 
        szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  (void) WSTRLCAT(wBuf, szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

  pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_VIEW_PROFILE);

  if(pIMenuCtl == NULL)
  {
    FREE(szBuf);
    FREE(wBuf);
    DATASTATS_ERR("pIMenuCtl NULL");
    return FALSE;  
  }
  IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_QOS, DATASTATSAPP_RES_FILE, 0, wBuf);
  IMENUCTL_Redraw(pIMenuCtl);
  FREE(szBuf);
  FREE(wBuf);
  return TRUE;
#else
  return FALSE;
#endif 
}
/*===========================================================================
FUNCTION CDataStatsApp_SaveQoSUMTSMaxULBR

DESCRIPTION
  1. Store in pMe, the UMTS QoS Maximum Bit Rate.
  2. End the Edit Max UL BR dialog.
  3. Modify the text for the UMTS QoS screen, as per new Max UL Bit Rate.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_SaveQoSUMTSMaxULBR(CDataStatsApp *pMe, const ITEMHANDLER *pItem)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  IDialog *pIDialog;
  ITextCtl *pITextCtl;
  IMenuCtl *pIMenuCtl = NULL;
  AECHAR   *szBuf = NULL;
  AECHAR   *wBuf = NULL;
  char     *Buf = NULL;

  PRINT_FUNCTION_NAME();

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  if(pIDialog == NULL){
    DATASTATS_ERR("pIDialog NULL");
    return FALSE;
  }
      
  pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, IDC_TEXT_QOS_UMTS_MAX_UL_BR);
    
  if(pITextCtl == NULL){
    DATASTATS_ERR("pITextCtl NULL");
    return FALSE;
  }
  szBuf = (AECHAR*) MALLOC(2*DATASTATS_MAX_BUFFER_SIZE*sizeof(AECHAR));
  wBuf = (AECHAR*) MALLOC(AEEPDP_MAX_BR_STRING_LEN*sizeof(AECHAR));
  Buf = (char*) MALLOC(AEEPDP_MAX_BR_STRING_LEN*sizeof(char));

  if(szBuf == NULL || wBuf == NULL || Buf == NULL)
  {
    DATASTATS_ERR("Malloc Failed");
    FREEIF(szBuf);
    FREEIF(wBuf);
    FREEIF(Buf);
    return FALSE;
  }

  ITEXTCTL_GetText(pITextCtl, wBuf, AEEPDP_MAX_BR_STRING_LEN);

  (void) WSTRTOSTR(wBuf, Buf, WSTRLEN(wBuf)+ 1);

  if( (ATOI(Buf) < 1) || ( (ATOI(Buf) ) > AEE_QOS_UMTS_MAX_KBR )){
    DATASTAT_ERROR_DLG( "Enter value between 1 - 14000 KBps");
    FREE(szBuf);
    FREE(Buf);
    FREE(wBuf);
    return FALSE;
  }
  // save the information.
  
  if( pMe->nQoSType == AEEPDP_QOS_UMTS_REQ )
    pMe->m_UMTSQoSReq.max_ul_bitrate = (uint32) ATOI(Buf);
  else if ( pMe->nQoSType == AEEPDP_QOS_UMTS_MIN )
   pMe->m_UMTSQoSMin.max_ul_bitrate = (uint32) ATOI(Buf);

  (void) CDataStatsApp_EndDialog(pMe, NULL);

  //Change the text for the QoS UMTS dialog.
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
              
  if(pIDialog == NULL){
    DATASTATS_ERR("pIDialog NULL");
    FREE(szBuf);
    FREE(Buf);
    FREE(wBuf);
    return FALSE;
  }
  ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_UMTS_MAX_UL_BR, 
    szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  
  (void) WSTRLCAT(szBuf, wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

  pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_EDIT_QOS_UMTS);
        
  if(pIMenuCtl == NULL){
    DATASTATS_ERR("pIMenuCtl NULL");
    FREE(szBuf);
    FREE(wBuf);
    FREE(Buf);
    return FALSE;
  }
  IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_QOS_UMTS_MAX_UL_BR, DATASTATSAPP_RES_FILE, 0, szBuf);
  IMENUCTL_Redraw(pIMenuCtl);
  FREE(szBuf);
  FREE(wBuf);
  FREE(Buf);
  return TRUE;
#else
  return FALSE;
#endif
}
/*===========================================================================
FUNCTION CDataStatsApp_SaveQoSUMTSMaxDLBR

DESCRIPTION
  1. Store in pMe, the UMTS QoS Maximum DL Bit Rate.
  2. End the Edit Max DL BR dialog.
  3. Modify the text for the UMTS QoS screen, as per new Max DL Bit Rate.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_SaveQoSUMTSMaxDLBR(CDataStatsApp *pMe, const ITEMHANDLER *pItem)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  IDialog *pIDialog = NULL;
  ITextCtl *pITextCtl = NULL;
  IMenuCtl *pIMenuCtl = NULL;
  AECHAR   *szBuf = NULL;
  AECHAR   *wBuf = NULL;
  char     *Buf = NULL;

  PRINT_FUNCTION_NAME();

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  if(pIDialog == NULL){
    DATASTATS_ERR("pIDialog NULL");
    return FALSE;
  }
      
  pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, IDC_TEXT_QOS_UMTS_MAX_DL_BR);
    
  if(pITextCtl == NULL){
    DATASTATS_ERR("pITextCtl NULL");
    return FALSE;
  }
  szBuf = (AECHAR*) MALLOC(2*DATASTATS_MAX_BUFFER_SIZE*sizeof(AECHAR));
  wBuf = (AECHAR*) MALLOC(2*DATASTATS_MAX_BUFFER_SIZE*sizeof(AECHAR));
  Buf = (char*) MALLOC(DATASTATS_MAX_BUFFER_SIZE*sizeof(char));
  if(szBuf == NULL || wBuf == NULL || Buf == NULL)
  {
    DATASTATS_ERR("Malloc Failed");
    FREEIF(szBuf);
    FREEIF(wBuf);
    FREEIF(Buf);
    return FALSE;
  }

  ITEXTCTL_GetText(pITextCtl, wBuf, AEEPDP_MAX_BR_STRING_LEN);
      
  (void) WSTRTOSTR(wBuf, Buf, WSTRLEN(wBuf) + 1);

  if( (ATOI(Buf) < 1) || ( (ATOI(Buf) ) > AEE_QOS_UMTS_MAX_KBR )){
    DATASTAT_ERROR_DLG( "Enter value between 1 - 14000 KBps");
    FREE(szBuf);
    FREE(Buf);
    FREE(wBuf);
    return FALSE;
  }
  // save the information.
  if( pMe->nQoSType == AEEPDP_QOS_UMTS_REQ )
   pMe->m_UMTSQoSReq.max_dl_bitrate = (uint32) ATOI(Buf);
  else if ( pMe->nQoSType == AEEPDP_QOS_UMTS_MIN )
   pMe->m_UMTSQoSMin.max_dl_bitrate = (uint32) ATOI(Buf);

  (void) CDataStatsApp_EndDialog(pMe, NULL);
  //Change the text for the QoS UMTS dialog.

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
              
  if(pIDialog == NULL){
    DATASTATS_ERR("pIDialog NULL");
    FREE(szBuf);
    FREE(Buf);
    FREE(wBuf);
    return FALSE;
  }
  ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_UMTS_MAX_DL_BR, 
    szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

  (void) WSTRLCAT(szBuf, wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_EDIT_QOS_UMTS);
        
  if(pIMenuCtl == NULL){
    DATASTATS_ERR("pIMenuCtl NULL");
    FREE(szBuf);
    FREE(wBuf);
    FREE(Buf);
    return FALSE;
  }
  IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_QOS_UMTS_MAX_DL_BR, DATASTATSAPP_RES_FILE, 0, szBuf);
  IMENUCTL_Redraw(pIMenuCtl);
  FREE(szBuf);
  FREE(wBuf);
  FREE(Buf);
  return TRUE;
#else
  return FALSE;
#endif
}

/*===========================================================================
FUNCTION CDataStatsApp_SaveQoSUMTSMinULBR

DESCRIPTION
  1. Store in pMe, the UMTS QoS Minimum UL Bit Rate.
  2. End the Edit Min UL BR dialog.
  3. Modify the text for the UMTS QoS screen, as per new Min UL Bit Rate.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_SaveQoSUMTSMinULBR(CDataStatsApp *pMe, const ITEMHANDLER *pItem)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  IDialog *pIDialog = NULL;
  ITextCtl *pITextCtl = NULL;
  IMenuCtl *pIMenuCtl = NULL;
  AECHAR   *szBuf = NULL;
  AECHAR   *wBuf = NULL;
  char     *Buf = NULL;

  PRINT_FUNCTION_NAME();

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  if(pIDialog == NULL){
    DATASTATS_ERR("pIDialog NULL");
    return FALSE;
  }
      
  pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, IDC_TEXT_QOS_UMTS_MIN_UL_BR);
    
  if(pITextCtl == NULL){
    DATASTATS_ERR("pITextCtl NULL");
    return FALSE;
  }
  szBuf = (AECHAR*) MALLOC(2*DATASTATS_MAX_BUFFER_SIZE*sizeof(AECHAR));
  wBuf = (AECHAR*) MALLOC(AEEPDP_MAX_BR_STRING_LEN*sizeof(AECHAR));
  Buf = (char*) MALLOC(AEEPDP_MAX_BR_STRING_LEN*sizeof(char));

  if(szBuf == NULL || wBuf == NULL || Buf == NULL)
  {
    DATASTATS_ERR("Malloc Failed");
    FREEIF(szBuf);
    FREEIF(wBuf);
    FREEIF(Buf);
    return FALSE;
  }

  ITEXTCTL_GetText(pITextCtl, wBuf, AEEPDP_MAX_BR_STRING_LEN);

  (void) WSTRTOSTR(wBuf, Buf, WSTRLEN(wBuf)+ 1);

  if( (ATOI(Buf) < 1) || ( (ATOI(Buf) ) > AEE_QOS_UMTS_MAX_KBR )){
    DATASTAT_ERROR_DLG( "Enter value between 1 - 14000 KBps");
    FREE(szBuf);
    FREE(Buf);
    FREE(wBuf);
    return FALSE;
  }
  // save the information.
   if( pMe->nQoSType == AEEPDP_QOS_UMTS_REQ )
    pMe->m_UMTSQoSReq.gtd_ul_bitrate = (uint32) ATOI(Buf);

  else if ( pMe->nQoSType == AEEPDP_QOS_UMTS_MIN )
   pMe->m_UMTSQoSMin.gtd_ul_bitrate = (uint32) ATOI(Buf);
 
  (void) CDataStatsApp_EndDialog(pMe, NULL);

  //Change the text for the QoS UMTS dialog.
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
              
  if(pIDialog == NULL){
    DATASTATS_ERR("pIDialog NULL");
    FREE(szBuf);
    FREE(Buf);
    FREE(wBuf);
    return FALSE;
  }
  ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_UMTS_MIN_UL_BR, 
    szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  
  (void) WSTRLCAT(szBuf, wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

  pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_EDIT_QOS_UMTS);
        
  if(pIMenuCtl == NULL){
    DATASTATS_ERR("pIMenuCtl NULL");
    FREE(szBuf);
    FREE(wBuf);
    FREE(Buf);
    return FALSE;
  }
  IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_QOS_UMTS_MIN_UL_BR, DATASTATSAPP_RES_FILE, 0, szBuf);
  IMENUCTL_Redraw(pIMenuCtl);
  FREE(szBuf);
  FREE(wBuf);
  FREE(Buf);
  return TRUE;
#else
  return FALSE;
#endif
}
/*===========================================================================
FUNCTION CDataStatsApp_SaveQoSUMTSMinDLBR

DESCRIPTION
  1. Store in pMe, the UMTS QoS Minimum DL Bit Rate.
  2. End the Edit Min DL BR dialog.
  3. Modify the text for the UMTS QoS screen, as per new Min DL Bit Rate.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_SaveQoSUMTSMinDLBR(CDataStatsApp *pMe, const ITEMHANDLER *pItem)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  IDialog *pIDialog = NULL;
  ITextCtl *pITextCtl = NULL;
  IMenuCtl *pIMenuCtl = NULL;
  AECHAR   *szBuf = NULL;
  AECHAR   *wBuf = NULL;
  char     *Buf = NULL;

  PRINT_FUNCTION_NAME();

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  if(pIDialog == NULL){
    DATASTATS_ERR("pIDialog NULL");
    return FALSE;
  }
      
  pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, IDC_TEXT_QOS_UMTS_MIN_DL_BR);
    
  if(pITextCtl == NULL){
    DATASTATS_ERR("pITextCtl NULL");
    return FALSE;
  }
  szBuf = (AECHAR*) MALLOC(2*DATASTATS_MAX_BUFFER_SIZE*sizeof(AECHAR));
  wBuf = (AECHAR*) MALLOC(AEEPDP_MAX_BR_STRING_LEN*sizeof(AECHAR));
  Buf = (char*) MALLOC(AEEPDP_MAX_BR_STRING_LEN*sizeof(char));

  if(szBuf == NULL || wBuf == NULL || Buf == NULL)
  {
    DATASTATS_ERR("Malloc Failed");
    FREEIF(wBuf);
    FREEIF(szBuf);
    FREEIF(Buf);
    return FALSE;
  }

  ITEXTCTL_GetText(pITextCtl, wBuf, AEEPDP_MAX_BR_STRING_LEN);

  (void) WSTRTOSTR(wBuf, Buf, WSTRLEN(wBuf) + 1);

 if( (ATOI(Buf) < 1) || ( (ATOI(Buf) ) > AEE_QOS_UMTS_MAX_KBR )){
    DATASTAT_ERROR_DLG( "Enter value between 1 - 14000 KBps");
    FREE(szBuf);
    FREE(Buf);
    FREE(wBuf);
    return FALSE;
  }
  // save the information.
  if( pMe->nQoSType == AEEPDP_QOS_UMTS_REQ )
     pMe->m_UMTSQoSReq.gtd_dl_bitrate = (uint32) ATOI(Buf);

  else if ( pMe->nQoSType == AEEPDP_QOS_UMTS_MIN )
    pMe->m_UMTSQoSMin.gtd_dl_bitrate = (uint32) ATOI(Buf);

  (void) CDataStatsApp_EndDialog(pMe, NULL);

  //Change the text for the QoS UMTS dialog.
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
              
  if(pIDialog == NULL){
    DATASTATS_ERR("pIDialog NULL");
    FREE(szBuf);
    FREE(Buf);
    FREE(wBuf);
    return FALSE;
  }
  ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_UMTS_MIN_DL_BR, 
    szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  
  (void) WSTRLCAT(szBuf, wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

  pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_EDIT_QOS_UMTS);
        
  if(pIMenuCtl == NULL){
    DATASTATS_ERR("pIMenuCtl NULL");
    FREE(szBuf);
    FREE(wBuf);
    FREE(Buf);
    return FALSE;
  }
  IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_QOS_UMTS_MIN_DL_BR, DATASTATSAPP_RES_FILE, 0, szBuf);
  IMENUCTL_Redraw(pIMenuCtl);
  FREE(szBuf);
  FREE(wBuf);
  FREE(Buf);
  return TRUE;
#else
  return FALSE;
#endif
}
/*===========================================================================
FUNCTION CDataStatsApp_SaveGPRSPrec

DESCRIPTION
1. Save the user-selected GPRS precedence class
2. End the edit precedence class dialog and update the edit GPRS QoS screen.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_SaveGPRSPrec(CDataStatsApp *pMe, const ITEMHANDLER *pItem)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  IDialog *pIDialog = NULL;
  IMenuCtl *pIMenuCtl = NULL;
  AECHAR *PrecClass = NULL;
  AECHAR *szBuf = NULL;
  uint16 curSel = 0;
  AEEPDPGPRSQOS *GPRSQoS = NULL;

  PRINT_FUNCTION_NAME();

  szBuf = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
  PrecClass = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
  if (PrecClass == NULL || szBuf == NULL)
  {
    DATASTATS_ERR("Malloc Failed");
    FREEIF(szBuf);
    FREEIF(PrecClass);
    return FALSE;
  }
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  
  if (pIDialog == NULL ){
    FREE(szBuf);
    FREE(PrecClass);
    DATASTATS_ERR("pIDialog NULL");
    return FALSE;
  }
  pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_EDIT_QOS_GPRS_PREC);
      
  if(pIMenuCtl == NULL){
    FREE(szBuf);
    FREE(PrecClass);
    DATASTATS_ERR("pIMenuCtl NULL");
    return FALSE;
  }
  curSel = IMENUCTL_GetSel(pIMenuCtl);

  if( pMe->nQoSType == AEEPDP_QOS_GPRS_REQ )
    GPRSQoS = &pMe->m_GPRSQoSReq;
  else if ( pMe->nQoSType == AEEPDP_QOS_GPRS_MIN )
   GPRSQoS = &pMe->m_GPRSQoSMin;
  else{
    FREE(szBuf);
    FREE(PrecClass);
    return FALSE;
  }

  switch(curSel) {
  case IDL_QOS_GPRS_PREC_SUB:
    GPRSQoS->precedence = AEEPDP_QOS_PRECED_CLASS_SUB;

    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_PREC_CLASS_SUB, 
              PrecClass, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  break;

  case IDL_QOS_GPRS_PREC_CLASS_1:
    GPRSQoS->precedence = AEEPDP_QOS_PRECED_CLASS_1;
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_PREC_CLASS_1, 
              PrecClass, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  break;

  case IDL_QOS_GPRS_PREC_CLASS_2:
    GPRSQoS->precedence = AEEPDP_QOS_PRECED_CLASS_2;
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_PREC_CLASS_2, 
              PrecClass, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  break;

  case IDL_QOS_GPRS_PREC_CLASS_3:
    GPRSQoS->precedence = AEEPDP_QOS_PRECED_CLASS_3;
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_PREC_CLASS_3, 
              PrecClass, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  break;

  default:
    DATASTATS_ERR("Uknown selection");
    FREE(szBuf);
    FREE(PrecClass);
  return FALSE;
  }  

  (void) CDataStatsApp_EndDialog(pMe, NULL);
  //Change the text for the View Qos Dialog.
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
            
  if(pIDialog == NULL){
    FREE(szBuf);
    FREE(PrecClass);
    DATASTATS_ERR("pIDialog NULL");
    return FALSE;
  }
  ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_PREC, 
  szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  (void) WSTRLCAT(szBuf, PrecClass, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_EDIT_QOS_GPRS);
      
  if(pIMenuCtl == NULL){
    FREE(szBuf);
    FREE(PrecClass);
    DATASTATS_ERR("pIDialog NULL");
    return FALSE;
  }
        
  IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_QOS_GPRS_PREC_CLASS, DATASTATSAPP_RES_FILE, 0, szBuf);
  IMENUCTL_Redraw(pIMenuCtl);
  FREE(szBuf);
  FREE(PrecClass);
  return TRUE;
#else
  return FALSE;
#endif 
}
/*===========================================================================
FUNCTION CDataStatsApp_SaveGPRSRel

DESCRIPTION
1. Save the user-selected GPRS reliability class
2. End the edit releiability class dialog and update the GPRS QOS screen.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_SaveGPRSRel(CDataStatsApp *pMe, const ITEMHANDLER *pItem)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  IDialog *pIDialog = NULL;
  IMenuCtl *pIMenuCtl = NULL;
  AECHAR *RelClass = NULL;
  AECHAR *szBuf = NULL;
  uint16 curSel = 0;
  AEEPDPGPRSQOS *GPRSQoS = NULL;

  PRINT_FUNCTION_NAME();

  szBuf = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
  RelClass = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
  if (RelClass == NULL || szBuf == NULL)
  {
    DATASTATS_ERR("Malloc Failed");
    FREEIF(szBuf);
    FREEIF(RelClass);
    return FALSE;
  }
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  
  if (pIDialog == NULL ){
    FREE(szBuf);
    FREE(RelClass);
    DATASTATS_ERR("pIDialog NULL");
    return FALSE;
  }
  pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_EDIT_QOS_GPRS_REL_CLASS);
      
  if(pIMenuCtl == NULL){
    FREE(szBuf);
    FREE(RelClass);
    DATASTATS_ERR("pIMenuCtl NULL");
    return FALSE;
  }
  curSel = IMENUCTL_GetSel(pIMenuCtl);

  if( pMe->nQoSType == AEEPDP_QOS_GPRS_REQ )
    GPRSQoS = &pMe->m_GPRSQoSReq;
  else if ( pMe->nQoSType == AEEPDP_QOS_GPRS_MIN )
   GPRSQoS = &pMe->m_GPRSQoSMin;
  else {
    FREE(szBuf);
    FREE(RelClass);
    DATASTATS_ERR("QOS Type unknown");
    return FALSE;
  }

  switch(curSel) {
  case IDL_QOS_GPRS_REL_CLASS_SUB:
    GPRSQoS->reliability = AEEPDP_QOS_REL_CLASS_SUB;
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_REL_CLASS_SUB, 
              RelClass, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  break;

  case IDL_QOS_GPRS_REL_CLASS_1:
    GPRSQoS->reliability = AEEPDP_QOS_REL_CLASS_1;
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_REL_CLASS_1, 
              RelClass, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  break;

  case IDL_QOS_GPRS_REL_CLASS_2:
    GPRSQoS->reliability = AEEPDP_QOS_REL_CLASS_2;
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_REL_CLASS_2, 
              RelClass, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  break;

  case IDL_QOS_GPRS_REL_CLASS_3:
    GPRSQoS->reliability = AEEPDP_QOS_REL_CLASS_3;
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_REL_CLASS_3, 
              RelClass, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  break;

  default:
    DATASTATS_ERR("Uknown selection"); 
    FREE(szBuf);
    FREE(RelClass);
  return FALSE;
  }  

  (void) CDataStatsApp_EndDialog(pMe, NULL);
  //Change the text for the View Qos Dialog.
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
            
  if(pIDialog == NULL){
    FREE(szBuf);
    FREE(RelClass);
    DATASTATS_ERR("pIDialog NULL");
    return FALSE;
  }
  ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_REL_CLASS, 
  szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  (void) WSTRLCAT(szBuf, RelClass, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_EDIT_QOS_GPRS);
      
  if(pIMenuCtl == NULL){
    FREE(szBuf);
    FREE(RelClass);
    DATASTATS_ERR("pIDialog NULL");
    return FALSE;
  }
        
  IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_QOS_GPRS_REL_CLASS, DATASTATSAPP_RES_FILE, 0, szBuf);
  IMENUCTL_Redraw(pIMenuCtl);
  FREE(szBuf);
  FREE(RelClass);
  return TRUE;
#else
  return FALSE;
#endif 
}

/*===========================================================================
FUNCTION CDataStatsApp_SaveGPRSDelay

DESCRIPTION
1. Save the user-selected GPRS delay class
2. End the edit delay class dialog and update the GPRS QOS screen.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_SaveGPRSDelay(CDataStatsApp *pMe, const ITEMHANDLER *pItem)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  IDialog *pIDialog = NULL;
  IMenuCtl *pIMenuCtl = NULL;
  AECHAR *DelayClass = NULL;
  AECHAR *szBuf = NULL;
  uint16 curSel = 0;
  AEEPDPGPRSQOS *GPRSQoS = NULL;

  PRINT_FUNCTION_NAME();

  szBuf = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
  DelayClass = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
  if (DelayClass == NULL || szBuf == NULL)
  {
    DATASTATS_ERR("Malloc Failed");
    FREEIF(szBuf);
    FREEIF(DelayClass);
    return FALSE;
  }
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  
  if (pIDialog == NULL ){
    FREE(szBuf);
    FREE(DelayClass);
    DATASTATS_ERR("pIDialog NULL");
    return FALSE;
  }
  pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_EDIT_QOS_GPRS_DELAY_CLASS);
      
  if(pIMenuCtl == NULL){
    FREE(szBuf);
    FREE(DelayClass);
    DATASTATS_ERR("pIMenuCtl NULL");
    return FALSE;
  }
  curSel = IMENUCTL_GetSel(pIMenuCtl);

  if( pMe->nQoSType == AEEPDP_QOS_GPRS_REQ )
    GPRSQoS = &pMe->m_GPRSQoSReq;
  else if ( pMe->nQoSType == AEEPDP_QOS_GPRS_MIN )
   GPRSQoS = &pMe->m_GPRSQoSMin;
  else {
    FREE(szBuf);
    FREE(DelayClass);
    DATASTATS_ERR("pIMenuCtl NULL");
    return FALSE;
  }

  switch(curSel) {

  case IDL_QOS_GPRS_DELAY_CLASS_SUB:
    GPRSQoS->delay = AEEPDP_QOS_DELAY_CLASS_SUB;
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_DELAY_CLASS_SUB, 
              DelayClass, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  break;

  case IDL_QOS_GPRS_DELAY_CLASS_1:
    GPRSQoS->delay = AEEPDP_QOS_DELAY_CLASS_1;
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_DELAY_CLASS_1, 
              DelayClass, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  break;

  case IDL_QOS_GPRS_DELAY_CLASS_2:
    GPRSQoS->delay = AEEPDP_QOS_DELAY_CLASS_2;
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_DELAY_CLASS_2, 
              DelayClass, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  break;

  case IDL_QOS_GPRS_DELAY_CLASS_3:
    GPRSQoS->delay = AEEPDP_QOS_DELAY_CLASS_3;
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_DELAY_CLASS_3, 
              DelayClass, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  break;

  case IDL_QOS_GPRS_DELAY_CLASS_4:
    GPRSQoS->delay = AEEPDP_QOS_DELAY_CLASS_4;
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_DELAY_CLASS_4, 
              DelayClass, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  break;

  default:
     DATASTATS_ERR("Uknown selection");   
     FREE(szBuf);
     FREE(DelayClass);
  return FALSE;
  }  

  (void) CDataStatsApp_EndDialog(pMe, NULL);
  //Change the text for the View Qos Dialog.
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
            
  if(pIDialog == NULL){
    FREE(szBuf);
    FREE(DelayClass);
    DATASTATS_ERR("pIDialog NULL");
    return FALSE;
  }
  ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_DELAY_CLASS, 
  szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  (void) WSTRLCAT(szBuf, DelayClass, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_EDIT_QOS_GPRS);
      
  if(pIMenuCtl == NULL){
    FREE(szBuf);
    FREE(DelayClass);
    DATASTATS_ERR("pIDialog NULL");
    return FALSE;
  }
        
  IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_QOS_GPRS_DELAY_CLASS, DATASTATSAPP_RES_FILE, 0, szBuf);
  IMENUCTL_Redraw(pIMenuCtl);
  FREE(szBuf);
  FREE(DelayClass);
  return TRUE;
#else
  return FALSE;
#endif 
}
/*===========================================================================
FUNCTION CDataStatsApp_SaveQoSGPRSMeanDR
DESCRIPTION
  1. Store in pMe, the GPRS Mean data rate.
  2. End the Edit Mean DR  dialog.
  3. Modify the text for the GPRS QoS screen, as per new Mean data rate.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_SaveQoSGPRSMeanDR(CDataStatsApp *pMe, const ITEMHANDLER *pItem)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  IDialog *pIDialog = NULL;
  ITextCtl *pITextCtl = NULL;
  IMenuCtl *pIMenuCtl = NULL;
  AECHAR   *szBuf = NULL;
  AECHAR   *wBuf = NULL;
  char     *Buf = NULL;

  PRINT_FUNCTION_NAME();

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  if(pIDialog == NULL){
    DATASTATS_ERR("pIDialog NULL");
    return FALSE;
  }
      
  pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, IDC_TEXT_QOS_GPRS_MEAN_DR);
    
  if(pITextCtl == NULL){
    DATASTATS_ERR("pITextCtl NULL");
    return FALSE;
  }
  szBuf = (AECHAR*) MALLOC(2*DATASTATS_MAX_BUFFER_SIZE*sizeof(AECHAR));
  wBuf = (AECHAR*) MALLOC(AEEPDP_MAX_BR_STRING_LEN*sizeof(AECHAR));
  Buf = (char*) MALLOC(AEEPDP_MAX_BR_STRING_LEN*sizeof(char));

  if(szBuf == NULL || wBuf == NULL || Buf == NULL)
  {
    DATASTATS_ERR("Malloc Failed");
    FREEIF(szBuf);
    FREEIF(wBuf);
    FREEIF(Buf);
    return FALSE;
  }

  ITEXTCTL_GetText(pITextCtl, wBuf, AEEPDP_MAX_BR_STRING_LEN);

  (void) WSTRTOSTR(wBuf, Buf, WSTRLEN(wBuf)+1);

  if( ( ATOI(Buf) < 1 ) || ( ( ATOI(Buf) ) > AEE_QOS_GPRS_MAX_KBR ))
  {
    DATASTAT_ERROR_DLG( "Enter value between 1 - 2048KBps");
    FREE(szBuf);
    FREE(wBuf);
    FREE(Buf);
    return FALSE;
  }
  // save the information.
  if(pMe->nQoSType == AEEPDP_QOS_GPRS_REQ) 
    pMe->m_GPRSQoSReq.mean = (uint32) ATOI(Buf);
  else if (pMe->nQoSType == AEEPDP_QOS_GPRS_MIN)
    pMe->m_GPRSQoSMin.mean = (uint32) ATOI(Buf);

  (void) CDataStatsApp_EndDialog(pMe, NULL);

  //Change the text for the QoS UMTS dialog.
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
              
  if(pIDialog == NULL){
    DATASTATS_ERR("pIDialog NULL");
    FREE(szBuf);
    FREE(Buf);
    FREE(wBuf);
    return FALSE;
  }
  ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_MEAN, 
    szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  
  (void) WSTRLCAT(szBuf, wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

  pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_EDIT_QOS_GPRS);
        
  if(pIMenuCtl == NULL){
    DATASTATS_ERR("pIMenuCtl NULL");
    FREE(szBuf);
    FREE(wBuf);
    FREE(Buf);
    return FALSE;
  }
  IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_QOS_GPRS_MEAN_DR, DATASTATSAPP_RES_FILE, 0, szBuf);
  IMENUCTL_Redraw(pIMenuCtl);
  FREE(szBuf);
  FREE(wBuf);
  FREE(Buf);
  return TRUE;
#else
  return FALSE;
#endif
}

/*===========================================================================
FUNCTION CDataStatsApp_SaveQoSGPRSPeakDR
DESCRIPTION
  1. Store in pMe, the GPRS Peak data rate.
  2. End the Edit Peak DR  dialog.
  3. Modify the text for the GPRS QoS screen, as per new Peak data rate.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_SaveQoSGPRSPeakDR(CDataStatsApp *pMe, const ITEMHANDLER *pItem)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  IDialog *pIDialog = NULL;
  ITextCtl *pITextCtl = NULL;
  IMenuCtl *pIMenuCtl = NULL;
  AECHAR   *szBuf = NULL;
  AECHAR   *wBuf = NULL;
  char     *Buf = NULL;

  PRINT_FUNCTION_NAME();

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  if(pIDialog == NULL){
    DATASTATS_ERR("pIDialog NULL");
    return FALSE;
  }
      
  pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, IDC_TEXT_QOS_GPRS_PEAK_DR);
    
  if(pITextCtl == NULL){
    DATASTATS_ERR("pITextCtl NULL");
    return FALSE;
  }
  szBuf = (AECHAR*) MALLOC(2*DATASTATS_MAX_BUFFER_SIZE*sizeof(AECHAR));
  wBuf = (AECHAR*) MALLOC(AEEPDP_MAX_BR_STRING_LEN*sizeof(AECHAR));
  Buf = (char*) MALLOC(AEEPDP_MAX_BR_STRING_LEN*sizeof(char));

  if(szBuf == NULL || wBuf == NULL || Buf == NULL)
  {
    DATASTATS_ERR("Malloc Failed");
    FREEIF(szBuf);
    FREEIF(wBuf);
    FREEIF(Buf);
    return FALSE;
  }

  ITEXTCTL_GetText(pITextCtl, wBuf, AEEPDP_MAX_BR_STRING_LEN);

  (void) WSTRTOSTR(wBuf, Buf, WSTRLEN(wBuf)+1);

  if( ( ATOI(Buf) < 1 ) || ( ( ATOI(Buf) ) > AEE_QOS_GPRS_MAX_KBR ))
  {
    DATASTAT_ERROR_DLG( "Enter value between 1 - 2048KBps");
    FREE(szBuf);
    FREE(Buf);
    FREE(wBuf);
    return FALSE;
  }
  // save the information.
  if(pMe->nQoSType == AEEPDP_QOS_GPRS_REQ) 
    pMe->m_GPRSQoSReq.peak = (uint32) ATOI(Buf);
  else if (pMe->nQoSType == AEEPDP_QOS_GPRS_MIN)
    pMe->m_GPRSQoSMin.peak = (uint32) ATOI(Buf);

  (void) CDataStatsApp_EndDialog(pMe, NULL);

  //Change the text for the QoS UMTS dialog.
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
              
  if(pIDialog == NULL){
    DATASTATS_ERR("pIDialog NULL");
    FREE(szBuf);
    FREE(Buf);
    FREE(wBuf);
    return FALSE;
  }
  ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_QOS_GPRS_PEAK, 
    szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  
  (void) WSTRLCAT(szBuf, wBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

  pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_EDIT_QOS_GPRS);
        
  if(pIMenuCtl == NULL){
    DATASTATS_ERR("pIMenuCtl NULL");
    FREE(szBuf);
    FREE(wBuf);
    FREE(Buf);
    return FALSE;
  }
  IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_QOS_GPRS_PEAK_DR, DATASTATSAPP_RES_FILE, 0, szBuf);
  IMENUCTL_Redraw(pIMenuCtl);
  FREE(szBuf);
  FREE(wBuf);
  FREE(Buf);
  return TRUE;
#else
  return FALSE;
#endif
}
/*===========================================================================
FUNCTION CDataStatsApp_SaveQoSGPRS

DESCRIPTION
1. Save the user-selected QoS GPRS
2. End the edit QOS dialog and update the view profile screen.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_SaveQoSGPRS(CDataStatsApp *pMe, const ITEMHANDLER *pItem)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  IDialog *pIDialog = NULL;
  IMenuCtl *pIMenuCtl = NULL;
  uint16   wDlgID = 0;

  PRINT_FUNCTION_NAME();
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  
  if (pIDialog == NULL ){
    DATASTATS_ERR("pIDialog NULL");
    return FALSE;
  }

  wDlgID = IDIALOG_GetID(pIDialog);

  if (wDlgID == IDD_EDIT_QOS_GPRS){

    switch(pMe->nQoSType) {
    case AEEPDP_QOS_GPRS_REQ:
      pMe->m_GPRSQoSReq.valid_flg = TRUE;
    break;
    case AEEPDP_QOS_GPRS_MIN:
      pMe->m_GPRSQoSMin.valid_flg = TRUE;
    break;
    default:
    break;
    }  
  
    (void) CDataStatsApp_EndDialog(pMe, NULL);
    (void) CDataStatsApp_EndDialog(pMe, NULL);
    //Change the text for the Profile View Dialog.
    pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
        
    if(pIDialog == NULL){
      DATASTATS_ERR("pIDialog NULL");
      return FALSE;
    }

    pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_VIEW_PROFILE);

    if(pIMenuCtl == NULL){
      DATASTATS_ERR("pIMenuCtl NULL");
      return FALSE;  
    }
    IMENUCTL_Redraw(pIMenuCtl);
  }
  return TRUE;
#else
  return FALSE;
#endif 
}
/*===========================================================================
FUNCTION CDataStatsApp_SaveProfile

DESCRIPTION
  Function to save the profile.
  
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_SaveProfile(CDataStatsApp *pMe, const ITEMHANDLER *pItem)
/*lint -esym(715,pItem)*/
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  boolean retVal = TRUE;
  int nReturnStatus;
  AEEPDP pdpProfile;
  byte profileNum = 0;
  uint32 dwFields;

  PRINT_FUNCTION_NAME();

  /* Create the buffer for the profile name */
  pdpProfile.profile_name_len = (IPDP_GetMaxProfileNameLen(pMe->m_pIPDP) + 1) * sizeof(AECHAR);
  pdpProfile.profile_name = (AECHAR*) MALLOC(pdpProfile.profile_name_len);
  if (NULL == pdpProfile.profile_name) {
      DATASTAT_ERROR_DLG( "No memory for profile name");
      return FALSE;
  }

  /* Read the PDP profile information so that 
   * we don't overwrite valid data.
   */
  profileNum = pMe->selectedProfile;
  dwFields = AEEPDP_PROFILE_ALL_DATA;
  nReturnStatus = IPDP_GetParm(pMe->m_pIPDP,profileNum, dwFields, &pdpProfile);
  /* Reset dwFields - being done below. So no need to do it here */
  if(CheckAEEReturnStatus(nReturnStatus) == FALSE)
  {
    MSG_ERROR( "Unable to retrieve profile info, return status=%d",nReturnStatus,0,0);
    FREE(pdpProfile.profile_name);
    return FALSE;
  }

  dwFields = 0;

  (void) WSTRLCPY(pdpProfile.profile_name, pMe->profileText, pdpProfile.profile_name_len);
  dwFields |= AEEPDP_PROFILE_NAME;

  (void) WSTRLCPY(pdpProfile.context.apn, pMe->apn, AEEPDP_MAX_APN_STRING_LEN+1);
  dwFields |= AEEPDP_PROFILE_APN;
  
  // Copy PDP Type
  pdpProfile.context.pdp_type = pMe->m_pdpType;
  pdpProfile.context.valid_flg = TRUE;
  dwFields |= AEEPDP_PROFILE_CONTEXT;

#ifdef FEATURE_DATA_IMS 
  // Copy request for PCSCF address
  pdpProfile.req_pcscf_addr = pMe->m_pcscf_addr;
  dwFields |= AEEPDP_PROFILE_PCSCF_ADDR_REQ;
  pdpProfile.im_cn_flag = pMe->m_im_cn_flag;
  dwFields |= AEEPDP_PROFILE_IM_CN_FLAG;
  pdpProfile.dhcp_pcscf_addr = pMe->m_dhcp_pcscf_addr;
  dwFields |= AEEPDP_PROFILE_DHCP_PCSCF_ADDR;
#endif //FEATURE_DATA_IMS

  if(pMe->m_pdpType == AEEPDP_IP || pMe->m_pdpType == AEEPDP_IP_ANY)
  {
    pdpProfile.context.pdp_addr.pdp_addr.pdp_addr_ipv4 = pMe->nIp;
    pdpProfile.dns_addr.primary_dns_addr.pdp_addr.pdp_addr_ipv4 = pMe->nDnsPrimary.pdp_addr.pdp_addr_ipv4;
    dwFields |= AEEPDP_PROFILE_ADDRESS;

    pdpProfile.dns_addr.primary_dns_addr = pMe->nDnsPrimary;
    pdpProfile.dns_addr.secondary_dns_addr.pdp_addr.pdp_addr_ipv4 = pMe->nDnsSecondary.pdp_addr.pdp_addr_ipv4;
    dwFields |= AEEPDP_PROFILE_DNS;
  }
  else if(pMe->m_pdpType == AEEPDP_IPV6)
  {
//    pdpProfile.context.pdp_addr.pdp_addr_ipv6= pMe->nIpv6;
    dwFields |= AEEPDP_PROFILE_ADDRESS;
  }
  
  pdpProfile.auth.auth_type = pMe->nSecurity;
  (void) MEMSET((void*) pdpProfile.auth.username, 0, AEEPDP_MAX_QCPDP_STRING_LEN);
  (void) MEMSET((void*) pdpProfile.auth.password, 0, AEEPDP_MAX_QCPDP_STRING_LEN);
  dwFields |= AEEPDP_PROFILE_AUTH;
  
  switch(pMe->nSecurity)
  {
  case AEEPDP_AUTH_CHAP:
    (void) STRTOWSTR( (char*) pMe->password, 
      pdpProfile.auth.password, sizeof(AECHAR)* AEEPDP_MAX_QCPDP_STRING_LEN);
     break;

  case AEEPDP_AUTH_PAP:
    (void) STRTOWSTR( (char*) pMe->password, 
      pdpProfile.auth.password, sizeof(AECHAR)* AEEPDP_MAX_QCPDP_STRING_LEN);
    (void) STRTOWSTR( (char*) pMe->username, 
      pdpProfile.auth.username, sizeof(AECHAR)* AEEPDP_MAX_QCPDP_STRING_LEN);
      break;

  default:
    break;

  }
  
  dwFields |= AEEPDP_PROFILE_AUTH;

  CDataStatsApp_SaveQoS(pMe, &pdpProfile, &dwFields);
    
  nReturnStatus = IPDP_SetParm(pMe->m_pIPDP, profileNum, dwFields, &pdpProfile);

  // free the name buffer
  FREE(pdpProfile.profile_name);

  if(CheckAEEReturnStatus(nReturnStatus) == FALSE)
  {
    DATASTAT_ERROR_DLG( "Unable to save authentication info");
    return FALSE;
  }
  
  retVal = retVal & CDataStatsApp_EndDialog(pMe, NULL);

  ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC,AEECLSID_DATASTATS
                         ,EVT_USER,EVT_DATASTATS_DLG_RESUME,0);
  
  return retVal;
#else
  return FALSE;
#endif
}


/*===========================================================================
FUNCTION CDataStatsApp_EndCall

DESCRIPTION
  Function to end a specific call.
  
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_EndCall(CDataStatsApp *pMe, const ITEMHANDLER *pItem)
/*lint -esym(715,pItem)*/
{
  IDialog *pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if(pIDialog == NULL || IDD_END_CALL != IDIALOG_GetID(pIDialog))
  {
    DATASTATS_ERR("End Call from Non-End Dialog");
    return FALSE;
  }
  //First remove the EndCall Dialog.
  (void) CDataStatsApp_EndDialog(pMe, NULL);

  if(CheckAEEReturnStatus(ICM_EndCall(pMe->m_pICM, pMe->call_id))== FALSE)
  {
    DATASTATS_ERR("Unable to End call");
    return FALSE;
  }
  return TRUE;

}

/*===========================================================================
FUNCTION CDataStatsApp_ExecuteItemAction

DESCRIPTION
  Main function which taked action on EVT_COMMAND based on the table.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_ExecuteItemAction(CDataStatsApp *pMe, uint16 wParam)
{
    int max = ARRAY_SIZE(MenuSelectionOperations);
    int i;

    PRINT_FUNCTION_NAME();
    for(i = 0; i < max; i++)
    {
        if(MenuSelectionOperations[i].wItemId == wParam)
        {
            break;        
        }
    }
    
    if(i < max)
    {
        if(MenuSelectionOperations[i].pfItemHandler != NULL)
        {
            return (*MenuSelectionOperations[i].pfItemHandler)(pMe, &MenuSelectionOperations[i]);
        }
    }

    return FALSE;
}

/*=============================================================================
FUNCTION:  DataStatsApp_SetMenuLook

DESCRIPTION: Get a pointer to a menu control and call the SetDefaultMenuLook 
             function.

PARAMETERS:
   

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CDataStatsApp_SetMenuLook(IShell *pIShell, uint16 menuId)
{
    IDialog *pIDialog = ISHELL_GetActiveDialog(pIShell); 
    IMenuCtl *pIMenuCtl = NULL;

    PRINT_FUNCTION_NAME();
    
    if(pIDialog != NULL)
    {
        pIMenuCtl = (IMenuCtl* )IDIALOG_GetControl(pIDialog, menuId);
        
        if(pIMenuCtl != NULL)
        {
            SetDefaultMenuLook(pIShell, pIMenuCtl);
        }
        else
        {
            DATASTATS_ERR("pIMenuCtl NULL");
        }
    }
    else
    {
        DATASTATS_ERR("pIDialog NULL");
    }
}
/*===========================================================================
FUNCTION CDataStatsApp_SetDefaultSoftkeyLook

DESCRIPTION
  Set default soft key menu look.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void CDataStatsApp_SetDefaultSoftkeyLook(IShell *pIShell, uint16 wSKId)
{
    IDialog *pIDialog = ISHELL_GetActiveDialog(pIShell);
    IMenuCtl *pISkCtl;
    
    PRINT_FUNCTION_NAME();
    if(pIDialog != NULL)
    {
        pISkCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, wSKId);
        
        if(pISkCtl != NULL)
        {
            SetDefaultSoftkeyLook(pIShell, pISkCtl);
        }
        else
        {
            DATASTATS_ERR("pISKCtl NULL");
        }
    }
    else
    {
        DATASTATS_ERR("pIDialog NULL");
    }
}

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)|| defined( FEATURE_DDTM_CNTL)
/*===========================================================================
FUNCTION CDataStatsDlg_ErrorDialog

DESCRIPTION
  Function called when there is error.
  put message to the console and pop-up the error dialog.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void CDataStatsDlg_ErrorDialog(CDataStatsApp *pMe, char *msg)
{
    PRINT_FUNCTION_NAME();
    (void) STRTOWSTR(msg, pMe->szError, MAX_ERROR_STR_LEN);
    ISHELL_CreateDialog(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDD_ERROR, NULL);
}
#endif


#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)

static int CDataStatsApp_StoreStringZ(byte *destBuf, IDialog *pIDialog, uint16 wStaticCtlId)
{
    int numBytes = 0;
    AECHAR *pszSrc;
    ITextCtl *pITextCtl;
    
    PRINT_FUNCTION_NAME();

    if(NULL == destBuf || NULL == pIDialog)
    {
        DATASTATS_ERR ("Null pointer");
        return FALSE;
    }
    
    pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, wStaticCtlId);
    *destBuf = 0;

    if(pITextCtl != NULL)
    {
        pszSrc = ITEXTCTL_GetTextPtr(pITextCtl);
        
        if(pszSrc != NULL)
        {
            numBytes = WSTRLEN(pszSrc);
        
            if(numBytes>0)
            {
                (void) WSTRTOSTR(pszSrc, (char*)destBuf, numBytes + 1 );    // TODO: WSTRTOSTR is copying only numBytes-1 bytes??
                destBuf[numBytes] = '\0';
            }
        }
    }
    
    return numBytes;
}

/*===========================================================================
FUNCTION IPToAECHAR

DESCRIPTION
  convert ip from uint32 to xx.xx.xx.xx AECHAR format

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

static void IPToAECHAR(uint32 ip, AECHAR *a)
{
    char temp[16];
    PRINT_FUNCTION_NAME();
    (void) SNPRINTF(temp, sizeof(temp),"%02d.%02d.%02d.%02d", 
        (byte)((ip & 0xff000000)>>24),
        (byte)((ip & 0x00ff0000)>>16),
        (byte)((ip & 0x0000ff00)>>8),
        (byte)((ip & 0x000000ff))
        );
    
    (void) STRTOWSTR(temp, a, 1+2*STRLEN(temp));
    
}

static void IPV6ToAECHAR(AEEPDPUMTSPDPAddr ip, AECHAR *a)
{
    char temp[128];
    uint32 ipV6[4];
    
    MEMCPY(ipV6,(uint32*)ip.pdp_addr.pdp_addr_ipv6.addr.addr32, sizeof(uint32) * 4);
    

    PRINT_FUNCTION_NAME();

    (void) SNPRINTF(temp, sizeof(temp), "%08d:%08d:%08d:%08d", ipV6[0],ipV6[1],ipV6[2],ipV6[3] );
    (void) STRTOWSTR(temp, a, 1+2*STRLEN(temp));
    
}

/*===========================================================================
FUNCTION IPToAECharIP

DESCRIPTION
  convert ip ( uint32) to AECharIP structure defined in DataStatsApp.h

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void IPToAECharIP(uint32 ip,AECharIP *aIp)
{
    char temp[4];
    PRINT_FUNCTION_NAME();
    (void) SNPRINTF(temp, sizeof(temp),"%02d",(byte)((ip & 0xff000000)>>24));
    (void) STRTOWSTR(temp,aIp->IP1, 1+2*STRLEN(temp));
    
    (void) SNPRINTF(temp, sizeof(temp),"%02d", (byte)((ip & 0x00ff0000)>>16));
    (void) STRTOWSTR(temp,aIp->IP2, 1+2*STRLEN(temp));

    (void) SNPRINTF(temp, sizeof(temp),"%02d",(byte)((ip & 0x0000ff00)>>8));
    (void) STRTOWSTR(temp,aIp->IP3, 1+2*STRLEN(temp));

    (void) SNPRINTF(temp, sizeof(temp),"%02d",(byte)((ip & 0x000000ff)));
    (void) STRTOWSTR(temp,aIp->IP4, 1+2*STRLEN(temp));
}
/*===========================================================================
FUNCTION CDataStatsApp_InitActiveProfile

DESCRIPTION
   Selects the currently active socket profile in the menu 
   and sets the radio button on for it.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

static void CDataStatsApp_InitActiveProfile(CDataStatsApp *pMe, uint16 wCtlId)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  IDialog *pIDialog;
  IMenuCtl *pIMenuCtl;
  byte activeProfileId = 1;
  int nReturnStatus;
  byte profileNum;
  uint16 nItemCount;
  int i;
  AEEPDP pdpProfile;
  uint16 nItemID, dwFields;
  AECHAR *szBuf = (AECHAR*)MALLOC(2*DATASTATS_MAX_WIDE_BUFFER_SIZE);
  uint32 maxProfiles;
  
   
  

  PRINT_FUNCTION_NAME();
  
  if(szBuf == NULL)
  {
    DATASTATS_ERR ("Null pointer");
    return;
  }

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  if(pIDialog != NULL)
  {
      pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, wCtlId);
      
      if(pIMenuCtl != NULL)
      {
          
        profileNum = 0;
        nReturnStatus = IPDP_GetDefaultProfileNum(pMe->m_pIPDP, &profileNum);

        if(CheckAEEReturnStatus(nReturnStatus) == FALSE)
        {
          DATASTAT_ERROR_DLG( "Unable to read the current active profile");
          return;
        }
        else
        {
          activeProfileId = profileNum;
        }

        IMENUCTL_SetSel(pIMenuCtl, (activeProfileId + IDL_MENU_SELECT_ACTIVE_PROFILE1 - 1));
        InitMenuIcons(pIMenuCtl);
        SetMenuIcon(pIMenuCtl, (activeProfileId + IDL_MENU_SELECT_ACTIVE_PROFILE1 - 1), TRUE);
        // Set the radiobutton on for the active profile

      pdpProfile.profile_name_len = (IPDP_GetMaxProfileNameLen(pMe->m_pIPDP) + 1) * sizeof(AECHAR);
      pdpProfile.profile_name = (AECHAR*) MALLOC(pdpProfile.profile_name_len);
      if (NULL == pdpProfile.profile_name) {
        return;
      }

        nItemCount = IMENUCTL_GetItemCount(pIMenuCtl);

        dwFields = AEEPDP_PROFILE_NAME;

        i = nItemCount;

        maxProfiles = IPDP_GetNumProfiles(pMe->m_pIPDP);

        while(i > maxProfiles)
        {
          nItemID = IMENUCTL_GetItemID(pIMenuCtl, i-1);
          IMENUCTL_DeleteItem(pIMenuCtl,nItemID);
          i--;
        }

        nItemCount = i;

        //Call End event
        for(i = 0; i < nItemCount; i++)
        {
          int nReturnStatus;

          nItemID = IMENUCTL_GetItemID(pIMenuCtl, i);
          nReturnStatus = IPDP_GetParm(pMe->m_pIPDP, i+1, dwFields, &pdpProfile);
        
          if(nReturnStatus == SUCCESS)
          {
            IMENUCTL_SetItemText(pIMenuCtl, nItemID, DATASTATSAPP_RES_FILE, 0,pdpProfile.profile_name);
      }
      else
      {
            
            ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_PROFILE1 + i,
                    szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

            IMENUCTL_SetItemText(pIMenuCtl, nItemID, DATASTATSAPP_RES_FILE, 0,szBuf);
      }
  }
        //Redraw
        IMENUCTL_Redraw(pIMenuCtl);
        FREEIF(szBuf);
      FREEIF(pdpProfile.profile_name);
        return;

      }
    else
    {
      DATASTATS_ERR("pIMenuCtl NULL");
    }
  }
#endif
}

#endif

/*===========================================================================
FUNCTION CDataStatsApp_RefreshDetailScreen

DESCRIPTION
  Refresh the Deatil screen. Called every one second.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

void CDataStatsApp_RefreshDetailScreen(void *pUser)
{
    
  CDataStatsApp *pMe = (CDataStatsApp*) pUser;
  IDialog *pIDialog;
  IStatic *pIStaticCtl;
  AEECMCallID call_id;
  AEECMCallInfo CallInfo;
  AECHAR *pszBuf, *pszMessage;
  char *buf;
  uint32 nRxDataRate, nTxDataRate;
  uint32 nTxHistoryBytes, nRxHistoryBytes;
  uint16 countSuffix;
  char szSysInfo[MAX_SYSTEM_INFORMATION_LENGTH+1];
  uint32 nCurTime;
  uint32 upTime;
  uint32 nTxPercent, nRxPercent;
  int x = 32, y = 70, w = 80, h = 10;
  AEERect rect;
  RGBVAL clrOn = MAKE_RGB(153,204,204); // Green for On  portion
  RGBVAL clrOff = MAKE_RGB(255,255,255); // White for Off portion


  PRINT_FUNCTION_NAME();

  if(NULL == pMe)
  {
    DATASTATS_ERR("pMe NULL");
    return; 
  }

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if(NULL == pIDialog)
  {
    DATASTATS_ERR("pIDialog NULL");
    return;
  }

  if(IDD_CALL_DETAIL != IDIALOG_GetID(pIDialog))
  {
    DATASTATS_ERR("NonDetail Update");
    return;
  }
  call_id = pMe->call_id;


  if(CheckAEEReturnStatus(ICM_GetCallInfo(pMe->m_pICM, call_id, &CallInfo, sizeof(AEECMCallInfo))) == FALSE)
  {
    DATASTATS_ERR("Unable to Get Call Info");
    return;
  }

  pszBuf = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
  pszMessage = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
  buf= (char*) MALLOC(DATASTATS_MAX_BUFFER_SIZE);
  
  if((pszBuf == NULL) ||(pszMessage == NULL) || (buf == NULL))
  {
    DATASTATS_ERR("Malloc Failed");
    FREEIF(pszBuf);
    FREEIF(pszMessage);
    FREEIF(buf);
    return ;
  }

  nCurTime = GETUPTIMEMS();

  
  nTxHistoryBytes = pMe->p_mTxHistoryBytes;
  nRxHistoryBytes = pMe->p_mRxHistoryBytes;

  CDataStatsApp_GetCallInformation(pMe, call_id, &nRxDataRate, &nTxDataRate, 
      (byte*) szSysInfo, MAX_SYSTEM_INFORMATION_LENGTH);

  
  if(nTxDataRate > 0)
  {
      nTxPercent = 100*(pMe->p_mTxHistoryBytes - nTxHistoryBytes) /\
      (((nCurTime - pMe->nLastTimeStamp)/1000)*(nTxDataRate));
  }
  else
  {
      nTxPercent = 0;
  }

  if(nRxDataRate > 0)
  {
      nRxPercent = 100*(pMe->p_mRxHistoryBytes - nRxHistoryBytes) /\
      (((nCurTime - pMe->nLastTimeStamp)/1000)*(nRxDataRate));
  }
  else
  {
      nRxPercent = 0;
  }
  if(nTxPercent > 100)
  {
      DBGPRINTF("Tx Percentage > 0 : Error");
      nTxPercent = 100;
  }
  if(nRxPercent > 100)
  {
      DBGPRINTF("Tx Percentage > 0 : Error");
      nRxPercent = 100;
  }
  
  
  
  pMe->nLastTimeStamp = nCurTime;

  // Set the title
  pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_CALL_DETAIL_TITLE);
  if(pIStaticCtl != NULL)
  {
    CDataStatsApp_GetAlphaString(pMe, call_id, pszBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    ISTATIC_SetText(pIStaticCtl, NULL, pszBuf, AEE_FONT_NORMAL, AEE_FONT_BOLD);
    ISTATIC_Redraw(pIStaticCtl);
  }
  else
  {
      DATASTATS_ERR("pIStaticCtl NULL");
  }
  //Set the time.
  pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_CALL_DETAIL_TIME);

  if(pIStaticCtl != NULL)
  {
      
      upTime = (nCurTime - CallInfo.start_time)/1000;
      (void) SNPRINTF(buf,DATASTATS_MAX_BUFFER_SIZE, "%02d:%02d:%02d", upTime/3600, (upTime/60)%60, upTime%60);
      (void) STRTOWSTR(buf, pszBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_TIME, 
        pszMessage, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      (void) WSTRLCAT(pszMessage, pszBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      ISTATIC_SetText(pIStaticCtl, NULL, pszMessage, AEE_FONT_NORMAL, AEE_FONT_BOLD);
      ISTATIC_Redraw(pIStaticCtl);
  }
  else
  {
      DATASTATS_ERR("pIStaticCtl NULL");
  }
  //Set the Tx Baud rate.
  pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_CALL_DETAIL_BAUD_TX);
  if(pIStaticCtl != NULL)
  {
    RateIndicatorToAECHAR(pMe, nTxDataRate, pszMessage);
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_TX, 
        pszBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    (void) WSTRLCAT(pszBuf, pszMessage, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    ISTATIC_SetText(pIStaticCtl, NULL, pszBuf, AEE_FONT_NORMAL, AEE_FONT_BOLD);
    ISTATIC_Redraw(pIStaticCtl);
  }
  else
  {
      DATASTATS_ERR("pIStaticCtl NULL");
  }

  //Set the Rx Baud Rate.
  pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_CALL_DETAIL_BAUD_RX);
  if(pIStaticCtl != NULL)
  {
      
    RateIndicatorToAECHAR(pMe, nRxDataRate, pszMessage);
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_RX, 
        pszBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    (void) WSTRLCAT(pszBuf, pszMessage, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    ISTATIC_SetText(pIStaticCtl, NULL, pszBuf, AEE_FONT_NORMAL, AEE_FONT_BOLD);
    ISTATIC_Redraw(pIStaticCtl);
  }
  else
  {
      DATASTATS_ERR("pIStaticCtl NULL");
  }

  //Set the Tx bytes.
  pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_CALL_DETAIL_TXCOUNT);
  if(pIStaticCtl != NULL)
  {
    if((pMe->p_mTxHistoryBytes+1)/100 <= 1000)
    {
      countSuffix = IDS_DD_B;
      (void) SNPRINTF(buf, DATASTATS_MAX_BUFFER_SIZE,"%d ", pMe->p_mTxHistoryBytes);
    }
    else if(((pMe->p_mTxHistoryBytes+1)/100) <= 10000)
    {
      /*lint -e506*/
      countSuffix = IDS_DD_KB;
      (void) SNPRINTF(buf,DATASTATS_MAX_BUFFER_SIZE, "%d.%02d ", (pMe->p_mTxHistoryBytes/1024),
        ((int) FDIV((double) pMe->p_mTxHistoryBytes, 1.024))%100 );
    }
    else 
    {
      countSuffix = IDS_DD_MB;
      (void) SNPRINTF(buf, DATASTATS_MAX_BUFFER_SIZE, "%d.%03d ", (pMe->p_mTxHistoryBytes/1024)/1024,
        (pMe->p_mTxHistoryBytes/1024)%1000);
      
    }

    (void) STRTOWSTR(buf, pszBuf,DATASTATS_MAX_WIDE_BUFFER_SIZE );
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, countSuffix, pszMessage, 64);
    (void) WSTRLCAT(pszBuf, pszMessage, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      
    ISTATIC_SetText(pIStaticCtl, NULL, pszBuf, AEE_FONT_NORMAL, AEE_FONT_BOLD);
    ISTATIC_Redraw(pIStaticCtl);
  }
  else
  {
      DATASTATS_ERR("pIStaticCtl NULL");
  }
  //Set the Rx Bytes.
  pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_CALL_DETAIL_RXCOUNT);
  if(pIStaticCtl != NULL)
  {
    
    if((pMe->p_mRxHistoryBytes+1)/100 < 1000)
    {
      countSuffix = IDS_DD_B;
      (void) SNPRINTF(buf, DATASTATS_MAX_BUFFER_SIZE, "%d ", pMe->p_mRxHistoryBytes);
    }
    else if(((pMe->p_mRxHistoryBytes+1)/100) < 10000)
    {
      /*lint -e506*/
      countSuffix = IDS_DD_KB;
      (void) SNPRINTF(buf, DATASTATS_MAX_BUFFER_SIZE, "%d.%02d ", (pMe->p_mRxHistoryBytes/1000),
        ((int)(FDIV(pMe->p_mRxHistoryBytes, 1.024)))%100 );
    }
    else 
    {
      countSuffix = IDS_DD_MB;
      (void) SNPRINTF(buf, DATASTATS_MAX_BUFFER_SIZE, "%d.%03d ", (pMe->p_mRxHistoryBytes/1024)/1024,
        (pMe->p_mRxHistoryBytes/1024)%1000);
      
    }

    (void) STRTOWSTR(buf, pszBuf,DATASTATS_MAX_WIDE_BUFFER_SIZE );
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, countSuffix, pszMessage, 64);
    (void) WSTRLCAT(pszBuf, pszMessage, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      
    ISTATIC_SetText(pIStaticCtl, NULL, pszBuf, AEE_FONT_NORMAL, AEE_FONT_BOLD);
    ISTATIC_Redraw(pIStaticCtl);
    
  }
  else
  {
      DATASTATS_ERR("pIStaticCtl NULL");
  }
  //Set the Dormancy Text
  pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_CALL_DETAIL_DORMANT);
  if(pIStaticCtl != NULL)
  {
    if(CallInfo.call_state == AEECM_CALL_STATE_DORMANT && CallInfo.sys_mode != AEECM_SYS_MODE_GSM)
    {
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_DORMANT, pszBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      ISTATIC_SetText(pIStaticCtl,NULL, pszBuf, AEE_FONT_NORMAL, AEE_FONT_BOLD);
    }
    else
    {
      ISTATIC_SetText(pIStaticCtl,NULL, NULL, AEE_FONT_NORMAL, AEE_FONT_BOLD);
    }
    ISTATIC_Redraw(pIStaticCtl);
      
  }
  else
  {
      DATASTATS_ERR("pIStaticCtl NULL");
  }
  //Set the mode.
  pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_CALL_DETAIL_MODE);

  if(pIStaticCtl != NULL)
  {
      (void) STRTOWSTR(szSysInfo, pszBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      ISTATIC_SetText(pIStaticCtl, NULL, pszBuf, AEE_FONT_NORMAL, AEE_FONT_BOLD);
      ISTATIC_Redraw(pIStaticCtl);
  }
  else
  {
      DATASTATS_ERR("pIStaticCtl NULL");
  }

  IDIALOG_Redraw(pIDialog);

  //Display the percentages.
  SETAEERECT(&rect,x, y, w, h);
  IDISPLAY_DrawFrame(pMe->a.m_pIDisplay, &rect, AEE_FT_LOWERED,clrOff);
  SETAEERECT(&rect,x , y, nTxPercent*0.8, h);

  IDISPLAY_DrawFrame(pMe->a.m_pIDisplay, &rect, AEE_FT_RAISED, clrOn);

  y = 90;
  SETAEERECT(&rect,x, y, w, h);
  IDISPLAY_DrawFrame(pMe->a.m_pIDisplay, &rect, AEE_FT_LOWERED,clrOff);
  SETAEERECT(&rect,x , y, nRxPercent*0.8, h);
  IDISPLAY_DrawFrame(pMe->a.m_pIDisplay, &rect, AEE_FT_RAISED, clrOn);
  //This will update the rate-bars...
  IDISPLAY_Update(pMe->a.m_pIDisplay);

  ISHELL_SetTimer(pMe->a.m_pIShell, 1000, CDataStatsApp_RefreshDetailScreen, (void*)pMe);
  
  FREE(pszBuf);
  FREE(pszMessage);
  FREE(buf);
  
}

/*===========================================================================
FUNCTION CDataStatsApp_InitCallDetailDialog

DESCRIPTION
Initialize the contents of the call detail dialog. 
The mode string, bytes transfered ed, the baud rates. 
The initial percentage will be zero.


DEPENDENCIES
CDataStatsApp_GetCallInformation

SIDE EFFECTS
  None
===========================================================================*/

static void CDataStatsApp_InitCallDetailDialog(CDataStatsApp *pMe)
{
    IDialog *pIDialog;
    IStatic *pIStaticCtl;
    AEECMCallID call_id;
    AEECMCallInfo CallInfo;
    uint32 nRxDataRate, nTxDataRate;
    char szSysInfo[MAX_SYSTEM_INFORMATION_LENGTH+1];
    uint32 nCurTime;
    uint32 upTime;
    uint16 countSuffix;
    AECHAR *pszBuf, *pszMessage;
    char *buf;

    PRINT_FUNCTION_NAME();

    if(NULL == pMe)
    {
        DATASTATS_ERR("pMe NULL");
        return;
    }
    
    pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    if(NULL == pIDialog)
    {
        DATASTATS_ERR("pIDialog NULL");
        return;
    }
    
    
    call_id = pMe->call_id;


    if(CheckAEEReturnStatus(ICM_GetCallInfo(pMe->m_pICM, call_id, &CallInfo, sizeof(AEECMCallInfo))) == FALSE)
    {
      DATASTATS_ERR("Unable to Get Call Info");
      return;
    }

    pszBuf = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
    pszMessage = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
    buf= (char*) MALLOC(DATASTATS_MAX_BUFFER_SIZE);
    
    if((pszBuf == NULL) ||(pszMessage == NULL) || (buf == NULL))
    {
      DATASTATS_ERR("Malloc Failed");
      FREEIF(pszBuf);
      FREEIF(pszMessage);
      FREEIF(buf);
      return;
    }
   
    nCurTime = GETUPTIMEMS();

    CDataStatsApp_GetCallInformation(pMe,call_id, &nRxDataRate, &nTxDataRate, 
        (byte*) szSysInfo, MAX_SYSTEM_INFORMATION_LENGTH);

    // Set the title
    pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_CALL_DETAIL_TITLE);
    if(pIStaticCtl != NULL)
    {
      CDataStatsApp_GetAlphaString(pMe, call_id, pszBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      ISTATIC_SetText(pIStaticCtl, NULL, pszBuf, AEE_FONT_NORMAL, AEE_FONT_BOLD);
      ISTATIC_Redraw(pIStaticCtl);
    }
    else
    {
        DATASTATS_ERR("pIStaticCtl NULL");
    }
    
    pMe->nLastTimeStamp = nCurTime;
    
    //Set the time.
    pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_CALL_DETAIL_TIME);
    
    if(pIStaticCtl != NULL)
    {
        
        upTime = (nCurTime - CallInfo.start_time)/1000;
        (void) SNPRINTF(buf, DATASTATS_MAX_BUFFER_SIZE, "%02d:%02d:%02d", upTime/3600, (upTime/60)%60, upTime%60);
        (void) STRTOWSTR(buf, pszBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
        ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_TIME, 
          pszMessage, DATASTATS_MAX_WIDE_BUFFER_SIZE);
        (void) WSTRLCAT(pszMessage, pszBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
        ISTATIC_SetText(pIStaticCtl, NULL, pszMessage, AEE_FONT_NORMAL, AEE_FONT_BOLD);
        ISTATIC_Redraw(pIStaticCtl);
    }
    else
    {
        DATASTATS_ERR("pIStaticCtl NULL");
    }
    //Set the Tx Baud rate.
    pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_CALL_DETAIL_BAUD_TX);
    if(pIStaticCtl != NULL)
    {
      RateIndicatorToAECHAR(pMe, nTxDataRate, pszMessage);
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_TX, 
        pszBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      (void) WSTRLCAT(pszBuf, pszMessage, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      ISTATIC_SetText(pIStaticCtl, NULL, pszBuf, AEE_FONT_NORMAL, AEE_FONT_BOLD);
      ISTATIC_Redraw(pIStaticCtl);
    }
    else
    {
        DATASTATS_ERR("pIStaticCtl NULL");
    }

    //Set the Rx Baud Rate.
    pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_CALL_DETAIL_BAUD_RX);
    if(pIStaticCtl != NULL)
    {
      RateIndicatorToAECHAR(pMe, nRxDataRate, pszMessage);
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_RX, 
        pszBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      (void) WSTRLCAT(pszBuf, pszMessage, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      ISTATIC_SetText(pIStaticCtl, NULL, pszBuf, AEE_FONT_NORMAL, AEE_FONT_BOLD);
      ISTATIC_Redraw(pIStaticCtl);
    }
    else
    {
        DATASTATS_ERR("pIStaticCtl NULL");
    }
    
    //Set the Tx bytes.
    pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_CALL_DETAIL_TXCOUNT);
    if(pIStaticCtl != NULL)
    {
      if((pMe->p_mTxHistoryBytes+1)/100 < 1000)
      {
        countSuffix = IDS_DD_B;
        (void) SNPRINTF(buf, DATASTATS_MAX_BUFFER_SIZE, "%d ", pMe->p_mTxHistoryBytes);
      }
      else if(((pMe->p_mTxHistoryBytes+1)/100) < 10000)
      {
        /*lint -e506*/
        countSuffix = IDS_DD_KB;
        (void) SNPRINTF(buf, DATASTATS_MAX_BUFFER_SIZE, "%d.%02d ", (pMe->p_mTxHistoryBytes/1000),
          ((int)(FDIV(pMe->p_mTxHistoryBytes, 1.024)))%100 );
      }
      else 
      {
        countSuffix = IDS_DD_MB;
        (void) SNPRINTF(buf, DATASTATS_MAX_BUFFER_SIZE, "%d.%03d ", (pMe->p_mTxHistoryBytes/1024)/1024,
          (pMe->p_mTxHistoryBytes/1024)%1000);
        
      }
      (void) STRTOWSTR(buf, pszBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, countSuffix, pszMessage, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      (void) WSTRLCAT(pszBuf, pszMessage, DATASTATS_MAX_WIDE_BUFFER_SIZE);
        
      
      ISTATIC_SetText(pIStaticCtl, NULL, pszBuf, AEE_FONT_NORMAL, AEE_FONT_BOLD);
      ISTATIC_Redraw(pIStaticCtl);
    }
    else
    {
        DATASTATS_ERR("pIStaticCtl NULL");
    }
    //Set the Rx Bytes.
    pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_CALL_DETAIL_RXCOUNT);
    if(pIStaticCtl != NULL)
    {
      if((pMe->p_mRxHistoryBytes+1)/100 < 1000)
      {
        countSuffix = IDS_DD_B;
        (void) SNPRINTF(buf, DATASTATS_MAX_BUFFER_SIZE, "%d ", pMe->p_mRxHistoryBytes);
      }
      else if(((pMe->p_mRxHistoryBytes+1)/100) < 10000)
      {
        countSuffix = IDS_DD_KB;
        /*lint -e506*/
        (void) SNPRINTF(buf, DATASTATS_MAX_BUFFER_SIZE, "%d.%02d ", (pMe->p_mRxHistoryBytes/1000),
          ((int)(FDIV(pMe->p_mRxHistoryBytes, 1.024)))%100 );
      }
      else 
      {
        countSuffix = IDS_DD_MB;
        (void) SNPRINTF(buf, DATASTATS_MAX_BUFFER_SIZE, "%d.%03d ", (pMe->p_mRxHistoryBytes/1024)/1024,
          (pMe->p_mRxHistoryBytes/1024)%1000);
        
      }
      (void) STRTOWSTR(buf, pszBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, countSuffix, pszMessage, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      (void) WSTRLCAT(pszBuf, pszMessage, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      ISTATIC_SetText(pIStaticCtl, NULL, pszBuf, AEE_FONT_NORMAL, AEE_FONT_BOLD);
      ISTATIC_Redraw(pIStaticCtl);
    }
    else
    {
        DATASTATS_ERR("pIStaticCtl NULL");
    }
    //Set the mode.
    pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_CALL_DETAIL_MODE);

    if(pIStaticCtl != NULL)
    {
        (void) STRTOWSTR(szSysInfo, pszBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
        ISTATIC_SetText(pIStaticCtl, NULL, pszBuf, AEE_FONT_NORMAL, AEE_FONT_BOLD);
        ISTATIC_Redraw(pIStaticCtl);
    }
    else
    {
        DATASTATS_ERR("pIStaticCtl NULL");
    }

    //Set the Dormancy Text
    pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_CALL_DETAIL_DORMANT);
    if(pIStaticCtl != NULL)
    {
      if(CallInfo.call_state == AEECM_CALL_STATE_DORMANT && CallInfo.sys_mode != AEECM_SYS_MODE_GSM)
      {
        ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_DORMANT, pszBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
        ISTATIC_SetText(pIStaticCtl,NULL, pszBuf, AEE_FONT_NORMAL, AEE_FONT_BOLD);
      }
      else
      {
        ISTATIC_SetText(pIStaticCtl,NULL, NULL, AEE_FONT_NORMAL, AEE_FONT_BOLD);
      }
      ISTATIC_Redraw(pIStaticCtl);
        
    }
    else
    {
        DATASTATS_ERR("pIStaticCtl NULL");
    }

    IDIALOG_Redraw(pIDialog);
    ISHELL_SetTimer(pMe->a.m_pIShell, 1000, CDataStatsApp_RefreshDetailScreen, (void*)pMe);
    FREE(pszBuf);
    FREE(pszMessage);
    FREE(buf);
}

/*===========================================================================
FUNCTION CDataStatsApp_RefreshSummaryMenu

DESCRIPTION
Called every second 
when another dialog ends and it wants to refresh summary screen.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

void CDataStatsApp_RefreshSummaryMenu(void *pUser)
{
  CDataStatsApp *pMe = (CDataStatsApp*) pUser;
  IDialog *pIDialog;
  int nNumActiveCalls;
  AEECMCallID CallIDs[MAX_NUM_DATA_CALLS];
  int bActiveCallIDs[MAX_NUM_DATA_CALLS];
  int i;
  uint32 nCurTime = GETUPTIMEMS();
  int nItemCount;
  uint16 nItemID;
  uint16 nActiveDlgID;
  uint32 nItemData;
  IMenuCtl *pIMenuCtl;
  AECHAR *pszBuf, *pszMessage;
  char *buf;
  
  uint32 nDuration; 

  PRINT_FUNCTION_NAME();

  if(NULL == pMe)
  {
      DATASTATS_ERR("pMe NULL");
      return;
  }
  if(NULL != pMe->a.m_pIShell)
  {
      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  }
  else
  {
      DATASTATS_ERR("pIShell NULL");
      return;
  }

  
  if(NULL == pIDialog)
  {
    DATASTATS_ERR("pIDialog NULL");
    return;
  }

  nActiveDlgID = IDIALOG_GetID(pIDialog);
  
  if(IDD_CALL_SUMMARY != nActiveDlgID)
  {
    DATASTATS_ERR("Refresh Error: !Summary Dialog");
    return;
  }

  pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_CALL_SUMMARY);
  
  if(NULL == pIMenuCtl)
  {
    DATASTATS_ERR("pIMenuCtl NULL");
    return;
  }

  pszBuf = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
  pszMessage = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
  buf= (char*) MALLOC(DATASTATS_MAX_BUFFER_SIZE);
  
  if((pszBuf == NULL) ||(pszMessage == NULL) || (buf == NULL))
  {
    DATASTATS_ERR("Malloc Failed");
    FREEIF(pszBuf);
    FREEIF(pszMessage);
    FREEIF(buf);
    return;
  }
  
  if(pMe->bStaleSummary == FALSE) // Just update the item text or remove ended calls.
  {
    nItemCount = IMENUCTL_GetItemCount(pIMenuCtl);
    
    for(i = 0; i < nItemCount; i++)
    {
      nItemID = IMENUCTL_GetItemID(pIMenuCtl, i);
      IMENUCTL_GetItemData(pIMenuCtl, nItemID, &nItemData);
      if(nItemID > (IDL_SUMMARY_CALL1 + ENDED_CALL_ITEMID_OFFSET)) // Its an Ended call
      {
        // If its time-out for this call, remove this item.
        // Else flash it
        AEECMCallInfo *pCallInfo = pMe->m_pEndedCalls[nItemData];
        if ((pCallInfo->start_time + pCallInfo->duration + FLASH_TIME) < nCurTime)
        {
          FREEIF(pMe->m_pEndedCalls[nItemData]);
          IMENUCTL_DeleteItem(pIMenuCtl, nItemID);
        }
        else
        {
          ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, 
                  IDS_DD_CALL_ENDED,pszBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
        
          if((nCurTime/1000)%2)// Flash time for the ended call.
          {
          
            (void) SNPRINTF(buf, DATASTATS_MAX_BUFFER_SIZE,": %3d:%02d", (pCallInfo->duration/1000)/60, 
              (pCallInfo->duration/1000)%60);
            (void) STRTOWSTR(buf, pszMessage, DATASTATS_MAX_WIDE_BUFFER_SIZE);
            (void) WSTRLCAT(pszBuf, pszMessage, DATASTATS_MAX_WIDE_BUFFER_SIZE);
          }
          IMENUCTL_SetItemText(pIMenuCtl, nItemID, DATASTATSAPP_RES_FILE, 0, pszBuf);

        }
        IMENUCTL_Redraw(pIMenuCtl);
      }
      else // its an Active Call. Get Information from ICM and update it.
      {
        AEECMCallInfo CallInfo;
        if(CheckAEEReturnStatus(ICM_GetCallInfo(pMe->m_pICM, (AEECMCallID) nItemData, &CallInfo, sizeof(AEECMCallInfo))) == FALSE)
        {
          DATASTATS_ERR("Unable to get callInfo");
          
          FREEIF(pszBuf);
          FREEIF(pszMessage);
          FREEIF(buf);
          return;
        }
        nDuration = (nCurTime - CallInfo.start_time)/1000;
        (void) SNPRINTF(buf, DATASTATS_MAX_BUFFER_SIZE,": %3d:%02d",nDuration/60, nDuration%60 ); 
        CDataStatsApp_GetAlphaString(pMe, (AEECMCallID) nItemData, pszBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
        (void) STRTOWSTR(buf, pszMessage, DATASTATS_MAX_WIDE_BUFFER_SIZE);
        (void) WSTRLCAT(pszBuf, pszMessage, DATASTATS_MAX_WIDE_BUFFER_SIZE);
        IMENUCTL_SetItemText(pIMenuCtl, nItemID, DATASTATSAPP_RES_FILE, 0, pszBuf);
      }
    }
  }
  else // Need to prepare the whole menu from scratch.
  {
    pMe->bStaleSummary = FALSE; // No more its stale.
    
    IMENUCTL_DeleteAll(pIMenuCtl);

    for(i = 0; i < MAX_NUM_DATA_CALLS; i++)
    {
      bActiveCallIDs[i] = FALSE;
    }
    nNumActiveCalls = ICM_GetActiveCallIDs(pMe->m_pICM,
                        (AEECMCallType)(AEECM_CALL_TYPE_CS_DATA | AEECM_CALL_TYPE_PS_DATA), 
                        (AEECMCallState)(AEECM_CALL_STATE_CONV|AEECM_CALL_STATE_DORMANT),
                        CallIDs, sizeof(CallIDs));
    
    for(i = 0; i < nNumActiveCalls; i++)
    {
      bActiveCallIDs[(int) CallIDs[i]] = TRUE;
    }

    for(i = 0; i < MAX_NUM_DATA_CALLS; i++)
    {
      if(pMe->m_pEndedCalls[i] != NULL)
      {
        AEECMCallInfo *pCallInfo = pMe->m_pEndedCalls[i];
        if ((pCallInfo->start_time + pCallInfo->duration + FLASH_TIME) < nCurTime)
        {
          FREEIF(pMe->m_pEndedCalls[i]);
        }
      }
    }

    for(i = 0; i < MAX_NUM_DATA_CALLS; i++)
    {
      if(bActiveCallIDs[i] == TRUE)
      {
        AEECMCallInfo CallInfo;
        if(CheckAEEReturnStatus(ICM_GetCallInfo(pMe->m_pICM, (AEECMCallID) i, &CallInfo, sizeof(AEECMCallInfo))) == FALSE)
        {
          DATASTATS_ERR("Unable to get call Info");
          FREEIF(pszBuf);
          FREEIF(pszMessage);
          FREEIF(buf);
         
          return;
        }
        nDuration = (nCurTime - CallInfo.start_time)/1000;
        (void) SNPRINTF(buf, DATASTATS_MAX_BUFFER_SIZE,": %3d:%02d",nDuration/60, nDuration%60 ); 
        CDataStatsApp_GetAlphaString(pMe, (AEECMCallID) i, pszBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
        (void) STRTOWSTR(buf, pszMessage, DATASTATS_MAX_WIDE_BUFFER_SIZE);
        (void) WSTRLCAT(pszBuf, pszMessage, DATASTATS_MAX_WIDE_BUFFER_SIZE);
        //IMENUCTL_AddItem(pIMenuCtl, DATASTATSAPP_RES_FILE, 0, IDL_SUMMARY_CALL1+ CallIDs[i], pszBuf, (uint32) CallIDs[i]);
        (void) CDataStatsApp_AddMenuItem(pMe, pIMenuCtl, (AEECMCallID) i, 
          (uint16)(IDL_SUMMARY_CALL1+i), pszBuf, &CallInfo);
      }

      if(pMe->m_pEndedCalls[i] != NULL)
      {
        AEECMCallInfo *pCallInfo = pMe->m_pEndedCalls[i];

        ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, 
            IDS_DD_CALL_ENDED,pszBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  
        (void) SNPRINTF(buf, DATASTATS_MAX_BUFFER_SIZE,": %3d:%02d", (pCallInfo->duration/1000)/60, 
          (pCallInfo->duration/1000)%60);
        (void) STRTOWSTR(buf, pszMessage, DATASTATS_MAX_WIDE_BUFFER_SIZE);
        (void) WSTRLCAT(pszBuf, pszMessage, DATASTATS_MAX_WIDE_BUFFER_SIZE);

        //IMENUCTL_AddItem(pIMenuCtl, DATASTATSAPP_RES_FILE, 0, IDL_SUMMARY_CALL1+ i + ENDED_CALL_ITEMID_OFFSET, pszBuf, (uint32) i);
        (void) CDataStatsApp_AddMenuItem(pMe, pIMenuCtl, (AEECMCallID) i, 
          (uint16)(IDL_SUMMARY_CALL1 + i + ENDED_CALL_ITEMID_OFFSET), pszBuf, pCallInfo);
      }
    }
    
  }

  IMENUCTL_Redraw(pIMenuCtl);
  ISHELL_SetTimer(pMe->a.m_pIShell, 1000, CDataStatsApp_RefreshSummaryMenu, (void*) pMe);
  FREEIF(pszBuf);
  FREEIF(pszMessage);
  FREEIF(buf);
         
}
/*===========================================================================
FUNCTION CDataStatsApp_UpdateSummaryMenu.

DESCRIPTION
Update the menu, based on the events.
Does this when CALL_END or CALL_CONNECT is received.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/


void CDataStatsApp_UpdateSummaryMenu(void *pUser, AEECMNotifyInfo *pEventInfo)
{ 
    
  CDataStatsApp *pMe = (CDataStatsApp*) pUser;
  IDialog *pIDialog;
  IMenuCtl *pIMenuCtl;
  AEECMCallID call_id;
  AEECMCallInfo CallInfo;
  int nItemCount;
  uint16 nItemID;
  uint32 nItemData;
  AECHAR *pszBuf, *pszMessage;
  char *buf;
  uint16 activeDlgId;
  
  int i;
  uint32 nDuration;
    
  PRINT_FUNCTION_NAME();

  if(NULL == pMe)
  {
      DATASTATS_ERR("pMe NULL");
      return;
  }
  if(NULL != pMe->a.m_pIShell)
  {
      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  }
  else
  {
      DATASTATS_ERR("pIShell NULL");
      return;
  }


  if(NULL != pIDialog)
  {
    if(pEventInfo->event == AEECM_EVENT_CALL_END)
    {
      FREEIF(pMe->m_pEndedCalls[pEventInfo->event_data.call.call_id]);
      pMe->m_pEndedCalls[pEventInfo->event_data.call.call_id] = (AEECMCallInfo*) MALLOC(sizeof(AEECMCallInfo));
      
      (void) MEMCPY((void*) pMe->m_pEndedCalls[pEventInfo->event_data.call.call_id], 
        (void*) &pEventInfo->event_data.call.call_info, sizeof(AEECMCallInfo));
      pMe->m_pEndedCalls[pEventInfo->event_data.call.call_id]->call_state = AEECM_CALL_STATE_ENDED;
    }
           
    activeDlgId = IDIALOG_GetID(pIDialog);
    switch(activeDlgId)
    {
    case IDD_CALL_SUMMARY:
      break;

    case IDD_CALL_DETAIL:
      if(pEventInfo->event_data.call.call_id == pMe->call_id 
          && pEventInfo->event == AEECM_EVENT_CALL_END)
      {
        (void) CDataStatsApp_EndDialog(pMe, NULL);
        (void) CDataStatsApp_CreateDialog(pMe, IDD_CALL_ENDED);
      }
      pMe->bStaleSummary = TRUE;
      return;

    default:
      // Set the stale flag, so that when Summary dialog is refreshed next time
      // It is rebuilt
      pMe->bStaleSummary = TRUE;
      return;

    }
  }
  else
  {
    DATASTATS_ERR("Null Dialog");
    return;
  }
  // If it is Call_End event, then just update the text for that MenuItem.
  // If it is a new call, add that in the menu.
  pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_CALL_SUMMARY);
  if(pIMenuCtl == NULL)
  {
    DATASTATS_ERR("pIMenuCtl NULL");
    return;
  }
  
  call_id = pEventInfo->event_data.call.call_id;
  nItemCount = IMENUCTL_GetItemCount(pIMenuCtl);
    
  //Call End event
  switch(pEventInfo->event)
  {
  case AEECM_EVENT_CALL_END:
  
    for(i = 0; i < nItemCount; i++)
    {
      nItemID = IMENUCTL_GetItemID(pIMenuCtl, i);
      IMENUCTL_GetItemData(pIMenuCtl, nItemID, &nItemData);
      if((AEECMCallID) nItemData == call_id)
      {
        //Change the text of this Item.
        pMe->bStaleSummary = TRUE;
        pszBuf = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
        pszMessage = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
        buf= (char*) MALLOC(DATASTATS_MAX_BUFFER_SIZE);
  
    
        if((pszBuf == NULL) ||(pszMessage == NULL) || (buf == NULL))
        {
          DATASTATS_ERR("Malloc Failed");
          FREEIF(pszBuf);
          FREEIF(pszMessage);
          FREEIF(buf);
          return;
        }
        ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, 
             IDS_DD_CALL_ENDED,pszBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
        
  
        (void) SNPRINTF(buf, DATASTATS_MAX_BUFFER_SIZE,": %3d:%02d", (pEventInfo->event_data.call.call_info.duration/1000)/60, 
          (pEventInfo->event_data.call.call_info.duration/1000)%60);
        (void) STRTOWSTR(buf, pszMessage, DATASTATS_MAX_WIDE_BUFFER_SIZE);
        (void) WSTRLCAT(pszBuf, pszMessage, DATASTATS_MAX_WIDE_BUFFER_SIZE);
        IMENUCTL_SetItemText(pIMenuCtl, nItemID, DATASTATSAPP_RES_FILE, 0, pszBuf);
        IMENUCTL_Redraw(pIMenuCtl);
        ISHELL_SetTimer(pMe->a.m_pIShell, 0, CDataStatsApp_RefreshSummaryMenu, (void*) pMe);
        
        FREE(pszBuf);
        FREE(pszMessage);
        FREE(buf);
        return;
      }
    }
    break;

  case AEECM_EVENT_CALL_CONNECT:
  
    // Add a new Item to the menu.
    // Get an unsed nItemID, Set data to the call_id
    // Set text to alpha
    if(CheckAEEReturnStatus(ICM_GetCallInfo(pMe->m_pICM, call_id, &CallInfo, sizeof(AEECMCallInfo))) == FALSE)
    {
      DATASTATS_ERR("Unable to get call_info");
      return;
    }
    pszBuf = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
    pszMessage = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
    buf= (char*) MALLOC(DATASTATS_MAX_BUFFER_SIZE);

    if((pszBuf == NULL) ||(pszMessage == NULL) || (buf == NULL))
    {
      DATASTATS_ERR("Malloc Failed");
      FREEIF(pszBuf);
      FREEIF(pszMessage);
      FREEIF(buf);
      return;
    }

    nDuration = (GETUPTIMEMS()- pEventInfo->event_data.call.call_info.start_time)/1000;
    (void) SNPRINTF(buf, DATASTATS_MAX_BUFFER_SIZE,": %3d:%02d",nDuration/60, nDuration%60 ); // Should be 00:00
    CDataStatsApp_GetAlphaString(pMe, call_id, pszBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    (void) STRTOWSTR(buf, pszMessage, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    (void) WSTRLCAT(pszBuf, pszMessage, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    //IMENUCTL_AddItem(pIMenuCtl, DATASTATSAPP_RES_FILE, 0, call_id + IDL_SUMMARY_CALL1, pszBuf, (uint32) call_id);
    (void) CDataStatsApp_AddMenuItem(pMe, pIMenuCtl, call_id, call_id + IDL_SUMMARY_CALL1, pszBuf, &CallInfo);
    IMENUCTL_Redraw(pIMenuCtl);
    IDIALOG_Redraw(pIDialog);
    ISHELL_SetTimer(pMe->a.m_pIShell, 0, CDataStatsApp_RefreshSummaryMenu, (void*) pMe);
    FREE(pszBuf);
    FREE(pszMessage);
    FREE(buf);
    break;

    case AEECM_EVENT_CALL_ENTER_DORMANT:
    case AEECM_EVENT_CALL_EXIT_DORMANT:
      // Stop the refresh timer, mark summary as stale and redraw, this will take care 
      // of chaning the icon.
      ISHELL_CancelTimer(pMe->a.m_pIShell, CDataStatsApp_RefreshSummaryMenu, (void*) pMe);
      pMe->bStaleSummary = TRUE;
      ISHELL_SetTimer(pMe->a.m_pIShell, 0, CDataStatsApp_RefreshSummaryMenu, (void*) pMe);
      break;
    
    default:
      break;
  }

  
}

/*===========================================================================
FUNCTION CDataStatsApp_InitCallEndedDialog

DESCRIPTION
  Common function to pop-up a new dialog.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

static void CDataStatsApp_InitCallEndedDialog(CDataStatsApp *pMe)
{
  IDialog *pIDialog;
  AEECMCallID call_id;
  IStatic* pIStaticCtl = NULL;
  uint16 countSuffix; // id for B,KB,MB,GB , etc.
  uint32 upTime ;//Time, for which call is up,  in seconds;
  AECHAR *pszBuf, *pszMessage;
  char *buf;
  
  AEECMCallInfo *pCallInfo;

  PRINT_FUNCTION_NAME();

  if(NULL == pMe)
  {
      DATASTATS_ERR("pMe NULL");
      return;
  }
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  
  if(NULL == pIDialog)
  {
      DATASTATS_ERR("pIDialog NULL");
      return;
  }

  call_id = pMe->call_id;
  
  pCallInfo = pMe->m_pEndedCalls[call_id];

  if(pCallInfo == NULL)
  {
    DATASTATS_ERR("Non-existing ended call");
    return;
  }
  
  pszBuf = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
  pszMessage = (AECHAR*) MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
  buf= (char*) MALLOC(DATASTATS_MAX_BUFFER_SIZE);


  if((pszBuf == NULL) ||(pszMessage == NULL) || (buf == NULL))
  {
    DATASTATS_ERR("Malloc Failed");
    FREEIF(pszBuf);
    FREEIF(pszMessage);
    FREEIF(buf);
    return;
  }
  
  // Set the title
  /*pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_CALL_ENDED_TITLE);
  if(pIStaticCtl != NULL)
  {
    CDataStatsApp_GetAlphaString(pMe, call_id, pszBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    ISTATIC_SetText(pIStaticCtl, NULL, pszBuf, AEE_FONT_NORMAL, AEE_FONT_BOLD);
  }
  else
  {
    DATASTATS_ERR("pIStaticCtl NULL");
  }*/
  //Set the time.
  pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_CALL_ENDED_TIMER);
  if(pIStaticCtl != NULL)
  {
    upTime = (pCallInfo->duration)/1000;
    (void) SNPRINTF(buf, DATASTATS_MAX_BUFFER_SIZE, "%02d:%02d:%02d", upTime/3600, (upTime/60)%60, upTime%60);
    (void) STRTOWSTR(buf, pszBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_TIME, pszMessage, 64);
    (void) WSTRLCAT(pszMessage, pszBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    ISTATIC_SetText(pIStaticCtl, NULL,pszMessage, AEE_FONT_NORMAL, AEE_FONT_BOLD);
  }
  else
  {
      DATASTATS_ERR("pIStaticCtl NULL");
  }
  //Set the Tx bytes.
  pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_CALL_ENDED_TXCOUNT);
  
  if(pIStaticCtl != NULL)
  {
      if((pMe->p_mTxHistoryBytes+1)/100 < 1000)
      {
        countSuffix = IDS_DD_B;
        (void) SNPRINTF(buf, DATASTATS_MAX_BUFFER_SIZE, "%d ", pMe->p_mTxHistoryBytes);
      }
      else if(((pMe->p_mTxHistoryBytes+1)/100) < 10000)
      {
        /*lint -e506*/
        countSuffix = IDS_DD_KB;
        (void) SNPRINTF(buf, DATASTATS_MAX_BUFFER_SIZE, "%d.%02d ", (pMe->p_mTxHistoryBytes/1024),
          ((int)(FDIV(pMe->p_mTxHistoryBytes, 1.024)))%100 );
      }
      else 
      {
        countSuffix = IDS_DD_MB;
        (void) SNPRINTF(buf, DATASTATS_MAX_BUFFER_SIZE, "%d.%03d ", (pMe->p_mTxHistoryBytes/1024)/1024,
          (pMe->p_mTxHistoryBytes/1024)%1000);
        
      }
      (void) STRTOWSTR(buf, pszBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, countSuffix, pszMessage, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      (void) WSTRLCAT(pszBuf, pszMessage, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      ISTATIC_SetText(pIStaticCtl, NULL, pszBuf, AEE_FONT_NORMAL, AEE_FONT_BOLD);
      ISTATIC_Redraw(pIStaticCtl);

  }
  else
  {
      DATASTATS_ERR("pIStaticCtl NULL");
  }
  //Set the Rx Bytes.
  pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_CALL_ENDED_RXCOUNT);
  if(pIStaticCtl != NULL)
  {
      if((pMe->p_mRxHistoryBytes+1)/100 < 1000)
      {
        countSuffix = IDS_DD_B;
        (void) SNPRINTF(buf, DATASTATS_MAX_BUFFER_SIZE, "%d ", pMe->p_mRxHistoryBytes);
      }
      else if(((pMe->p_mRxHistoryBytes+1)/100) < 10000)
      {
        /*lint -e506*/
        countSuffix = IDS_DD_KB;
        (void) SNPRINTF(buf, DATASTATS_MAX_BUFFER_SIZE, "%d.%02d ", (pMe->p_mRxHistoryBytes/1000),
          ((int)(FDIV(pMe->p_mRxHistoryBytes, 1.024)))%100 );
      }
      else 
      {
        countSuffix = IDS_DD_MB;
        (void) SNPRINTF(buf, DATASTATS_MAX_BUFFER_SIZE, "%d.%03d ", (pMe->p_mRxHistoryBytes/1024)/1024,
          (pMe->p_mRxHistoryBytes/1024)%1000);
        
      }
      (void) STRTOWSTR(buf, pszBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, countSuffix, pszMessage, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      (void) WSTRLCAT(pszBuf, pszMessage, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      ISTATIC_SetText(pIStaticCtl, NULL, pszBuf, AEE_FONT_NORMAL, AEE_FONT_BOLD);
      ISTATIC_Redraw(pIStaticCtl);        
  }
  else
  {
      DATASTATS_ERR("pIStaticCtl NULL");
  }
  //Set the mode.
  /*pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIDialog, IDC_STATIC_CALL_ENDED_MODE);
  if(pIStaticCtl != NULL)
  {
      STRTOWSTR(CallInfo->szSystemInfo, szwBuf1, 64);
      ISTATIC_SetText(pIStaticCtl, NULL, szwBuf1, AEE_FONT_NORMAL, AEE_FONT_BOLD);
  }
  else
  {
      DATASTATS_ERR("pIStaticCtl NULL");
  }*/
  FREE(pszBuf);
  FREE(pszMessage);
  FREE(buf);
     
}


/*===========================================================================
FUNCTION CDataStatsApp_StatusDlg_GetIcon

DESCRIPTION
  Get relevant icon to display for Summary-screen,based on call-type and state.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

static uint16 CDataStatsApp_StatusDlg_GetIcon(CDataStatsApp *pMe, 
                                              AEECMCallState CallState, 
                                              AEECMCallType CallType)
/*lint -esym(715,pMe)*/
{
 
  switch(CallType)
  {
  case AEECM_CALL_TYPE_PS_DATA: 
    switch(CallState)
    {
    case AEECM_CALL_STATE_CONV:  
      return IDB_DATA;

    case AEECM_CALL_STATE_ONHOLD: 
    case AEECM_CALL_STATE_DORMANT:
      return IDB_DATAHOLD;  

    case AEECM_CALL_STATE_ENDED: 
      return IDB_DATAEND;

    default:
      break; //IDB_EMPTY
    }
    break;

  case AEECM_CALL_TYPE_CS_DATA: 
    switch(CallState)
    {
    case AEECM_CALL_STATE_CONV:  
      return IDB_FAX;

    case AEECM_CALL_STATE_ONHOLD: 
    case AEECM_CALL_STATE_DORMANT:
      return IDB_FAXHOLD;  

    case AEECM_CALL_STATE_ENDED: 
      return IDB_FAXEND;

    default:
      break; //IDB_EMPTY
    }
    break;

  default:
    break;
  }
  
  return IDB_EMPTY;
}


/*===========================================================================
FUNCTION CDataStatsApp_AddMenuItem

DESCRIPTION
  

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_AddMenuItem(CDataStatsApp *pMe, IMenuCtl *pIMenuCtl, AEECMCallID CallID,
                                         uint16 wItemID, AECHAR *pszBuf, AEECMCallInfo *pCallInfo)
{
  CtlAddItem menuItem;
  boolean bRetVal;

  // error checking
  if(pMe == NULL)
  {
    DATASTATS_ERR("Null pointer");
    return FALSE;
  }

  // fill out menu item parameters
  menuItem.dwData = CallID;
  menuItem.pImage = NULL;
  menuItem.wText = 0;
  menuItem.pszResText = 0;
  menuItem.pszResImage = DATASTATSAPP_RES_FILE;
  menuItem.wFont = AEE_FONT_NORMAL;
  menuItem.wItemID = wItemID;

  menuItem.wImage = CDataStatsApp_StatusDlg_GetIcon(pMe, pCallInfo->call_state ,pCallInfo->call_type );

  menuItem.pText = pszBuf;

  // add the item to the menu passed in
  bRetVal = IMENUCTL_AddItemEx(pIMenuCtl, &menuItem);

  return bRetVal;
  
}

/*===========================================================================
FUNCTION CDataStatsApp_GetBaudRates

DESCRIPTION
 Get the Tx and the Rx rates 
 
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void CDataStatsApp_GetBaudRates
( 
  CDataStatsApp *pMe, 
  AEECMCallID call_id,
  uint32 *rx_rate,
  uint32 *tx_rate
)
{
  AEECMCallInfo callInfo;
  AEECMPhInfo phoneInfo;

  *rx_rate = *tx_rate = 0;

  if(CheckAEEReturnStatus(ICM_GetCallInfo(pMe->m_pICM, call_id, &callInfo, sizeof(callInfo))) == FALSE)
  {
    DATASTATS_ERR("Unable to get call_info");
    return;
  }
  
  if(CheckAEEReturnStatus(ICM_GetPhoneInfo(pMe->m_pICM, &phoneInfo, sizeof(phoneInfo))) == FALSE)
  {
    DATASTATS_ERR("Unable to get call_info");
    return;
  }

  if ( ICM_GetSysMode(pMe->m_pICM)  != AEECM_SYS_MODE_GSM   && 
       ICM_GetSysMode(pMe->m_pICM) != AEECM_SYS_MODE_WCDMA )
  {
     if (phoneInfo.mode_pref == AEECM_MODE_PREF_DIGITAL_ONLY ||
         phoneInfo.mode_pref == AEECM_MODE_PREF_CDMA_ONLY ||
         phoneInfo.mode_pref == AEECM_MODE_PREF_HDR_ONLY ||
         phoneInfo.mode_pref == AEECM_MODE_PREF_AUTOMATIC ||
         phoneInfo.mode_pref == AEECM_MODE_PREF_CDMA_WLAN ||
         phoneInfo.mode_pref == AEECM_MODE_PREF_HDR_WLAN ||
         phoneInfo.mode_pref == AEECM_MODE_PREF_CDMA_HDR_WLAN ||
         phoneInfo.mode_pref == AEECM_MODE_PREF_CDMA_AMPS_HDR_WLAN_ONLY ||
         phoneInfo.mode_pref == AEECM_MODE_PREF_CDMA_AMPS_WLAN_ONLY)
     {
  
#ifdef FEATURE_DS_IS2000
  /*
  ** For SO33 calls, display the transmit and receive data rates.
  **    T-----R----- when T is for Tx and R for Rx and ----- for rate.
  */
      if(callInfo.srv_opt == AEECM_SO_PPP_PKT_DATA_3G        ||
         callInfo.srv_opt == AEECM_SO_MDR_PKT_DATA_FRS2_RRS1 ||
         callInfo.srv_opt == AEECM_SO_MDR_PKT_DATA_FRS2_RRS2) 
      {
        *rx_rate = callInfo.data_stats.rx_max_rate*100;
        *tx_rate = callInfo.data_stats.tx_max_rate*100;
      }
#ifdef FEATURE_HDR
      else if(callInfo.srv_opt == AEECM_SO_HDR_PKT_DATA)
      {
        *tx_rate = DS_DATA_RATE_153_6; //1.53MBits
        *rx_rate = DS_DATA_RATE_2_4M; //2.4MBits
      }
#endif /* FEATURE_HDR */
#endif /* FEATURE_DS_IS2000 */
    }
  } /* end if cdma */
  else if ( ICM_GetSysMode(pMe->m_pICM)  == AEECM_SYS_MODE_GSM   || 
            ICM_GetSysMode(pMe->m_pICM)  == AEECM_SYS_MODE_WCDMA)
  {
    *rx_rate = callInfo.data_stats.rx_max_rate*100;
    *tx_rate = callInfo.data_stats.tx_max_rate*100;
  } /* end if GSM/WCDMA */

  pMe->p_mRxHistoryBytes = callInfo.data_stats.rx_total_bytes;
  pMe->p_mTxHistoryBytes = callInfo.data_stats.tx_total_bytes;
} /* end of function */


//DS Related functions

/*===========================================================================
FUNCTION CDataStatsApp_GetCallInformation

DESCRIPTION
      

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

void CDataStatsApp_GetCallInformation(CDataStatsApp *pMe, 
                           AEECMCallID call_id, 
                             uint32* RxDataRate,
                             uint32* TxDataRate,
                             byte *szSysInfo,
                             byte nMaxSysInfoLength
                             )
{
  
    
  *RxDataRate = 0;
  *TxDataRate = 0;
  *szSysInfo = NULL;

  if(ICM_GetSysMode(pMe->m_pICM)  != AEECM_SYS_MODE_GSM || 
    ICM_GetSysMode(pMe->m_pICM) != AEECM_SYS_MODE_WCDMA)
  {
    CDataStatsApp_GetBaudRates(pMe, call_id  , RxDataRate, TxDataRate);
  }
  else
  {
    (void) STRLCPY((char*)szSysInfo, "GW Mode", nMaxSysInfoLength - 1);
    CDataStatsApp_GetBaudRates(pMe, call_id  , RxDataRate, TxDataRate);
  }
  szSysInfo[nMaxSysInfoLength -1] = '\0';
  DBGPRINTF("RxR:%d, TxR:%d", *RxDataRate, *TxDataRate);

}

static void CDataStatsApp_GetGWAlphaString(CDataStatsApp *pMe, 
                                         AEECMCallID call_id,
                                         AECHAR *alpha,
                                         int size
                                         )
{
  AEECMCallInfo CallInfo;
  alpha[0] = '\0';
  
  if(CheckAEEReturnStatus(ICM_GetCallInfo(pMe->m_pICM, call_id, &CallInfo, sizeof(CallInfo))) == FALSE)
  {
    DATASTATS_ERR("Unable to get call_info");
    return;
  }
  if(WSTRLEN(CallInfo.alpha) != 0)
  {
    (void) WSTRNCOPYN(alpha, size/sizeof(AECHAR),  CallInfo.alpha, -1);
  }
  else //Alpha string does not contain anything.
  {
    (void) STRTOWSTR("Data Call" , alpha, size);
  }
  return;
  
  
  
}

static void CDataStatsApp_Get1XHDRAlphaString(CDataStatsApp *pMe, 
                                         AEECMCallID call_id,
                                         AECHAR *alpha,
                                         int size
                                         )
/*lint -esym(715, size)*/
{
  AEECMCallInfo CallInfo;
  alpha[0] = '\0';
  if(CheckAEEReturnStatus(ICM_GetCallInfo(pMe->m_pICM, call_id, &CallInfo, sizeof(CallInfo))) == FALSE)
  {
    DATASTATS_ERR("Unable to get call_info");
    return;
  }
  
  DBGPRINTF("Service Option : %d", CallInfo.srv_opt);

#ifdef FEATURE_DS_IS2000
  if (CallInfo.srv_opt == AEECM_SO_PPP_PKT_DATA_3G) 
  {
    (void) STRTOWSTR(" SO33 ", alpha, size);
  }
  else if(CallInfo.srv_opt == AEECM_SO_MDR_PKT_DATA_FRS1_RRS1)
  {
    (void) STRTOWSTR("MDR FRS1-RRS1 ", alpha, size);
  }
  else if( CallInfo.srv_opt == AEECM_SO_MDR_PKT_DATA_FRS1_RRS2)
  {
    (void) STRTOWSTR("MDR FRS1-RRS2 ", alpha, size);
  }
  else if (CallInfo.srv_opt  == AEECM_SO_MDR_PKT_DATA_FRS2_RRS1)
  {
    (void) STRTOWSTR("MDR FRS2-RRS1 ", alpha, size);
  }
  else if(CallInfo.srv_opt == AEECM_SO_MDR_PKT_DATA_FRS2_RRS2)
  {
    (void) STRTOWSTR("MDR FRS2-RRS2 ", alpha, size);
  }
#endif /* FEATURE_DS_IS2000 */

#ifdef FEATURE_HDR
 if (CallInfo.srv_opt ==  AEECM_SO_HDR_PKT_DATA)
 {
#ifdef FEATURE_HDR_IS890
  if ( hdris890_is_in_traffic() )
  {
    (void) STRTOWSTR(" IS-890 ", alpha, size);
  }
  else
#endif //FEATURE_HDR_IS890
  {
    (void) STRTOWSTR(" 1xEV ", alpha,  size);
  }
 }
 else
#endif //FEATURE_HDR
 {
   //Default string.
  (void) STRTOWSTR(" Data Call ", alpha,  size);

 }
}

static void CDataStatsApp_GetAlphaString(CDataStatsApp *pMe, 
                                         AEECMCallID call_id,
                                         AECHAR *alpha,
                                         int size
                                         )
{
  
  AEECMCallInfo CallInfo;
  if(CheckAEEReturnStatus(ICM_GetCallInfo(pMe->m_pICM, call_id, &CallInfo, sizeof(CallInfo))) == FALSE)
  {
    DATASTATS_ERR("Unable to get call_info");
    return;
  }
  
  if(ICM_GetSysMode(pMe->m_pICM) == AEECM_SYS_MODE_GSM ||
     ICM_GetSysMode(pMe->m_pICM) == AEECM_SYS_MODE_WCDMA)
  {
    CDataStatsApp_GetGWAlphaString(pMe, call_id, alpha, size);
  }
  else
  {
    CDataStatsApp_Get1XHDRAlphaString(pMe, call_id, alpha, size);
  }
  
}


static void RateIndicatorToAECHAR(CDataStatsApp *pMe, uint32 nRate, AECHAR *psBuf)
{
  char *buf = NULL;
  AECHAR *pszBuf = NULL;

  PRINT_FUNCTION_NAME();

  if(nRate >1000)
  {
    buf = (char*)MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);

    SNPRINTF(buf, DATASTATS_MAX_WIDE_BUFFER_SIZE, "%d", nRate/1000);
    (void) STRTOWSTR(buf,psBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    FREE(buf);

    if(nRate%1000 != 0)
    {
      buf = (char*)MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
      pszBuf = (AECHAR*)MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE *sizeof(AECHAR));

      SNPRINTF(buf, DATASTATS_MAX_WIDE_BUFFER_SIZE, ".%d", nRate%1000);
      (void) STRTOWSTR(buf,pszBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

      WSTRLCAT(psBuf, pszBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
      FREE(buf);
      FREE(pszBuf);
    }

    pszBuf = (AECHAR*)MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);

    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_KBPS, 
              pszBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

    (void) WSTRLCAT(psBuf,pszBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
    FREE(pszBuf);
  }
  else
  {
    buf = (char*)MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);

    SNPRINTF(buf, DATASTATS_MAX_WIDE_BUFFER_SIZE, "%d", nRate);
    (void) STRTOWSTR(buf,psBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

    pszBuf = (AECHAR*)MALLOC(DATASTATS_MAX_WIDE_BUFFER_SIZE);
    ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_BPS, 
              pszBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);

    (void) WSTRLCAT(psBuf,pszBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
  }

  
}

static boolean StripSettingsMenu(CDataStatsApp *pMe)
{
  IDialog *pIDialog;
  IMenuCtl *pIMenuCtl;

  PRINT_FUNCTION_NAME();
  if(pMe == NULL)
  {
    DATASTATS_ERR("pMe NULL");
    return FALSE;
  }
  if(pMe->a.m_pIShell == NULL)
  {
    DATASTATS_ERR("pIShell NULL");
    return FALSE;
  }
  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

  if((pIDialog == NULL) || (IDIALOG_GetID(pIDialog) != IDD_SETTINGS))
  {
    DATASTATS_ERR("Invalid Dialog");
    return FALSE;
  }
  
  pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_SETTINGS);
  
  if(pIMenuCtl == NULL)
  {
    DATASTATS_ERR("MenuCtl NULL");
    return FALSE;
  }
#ifndef FEATURE_DDTM_CNTL
  IMENUCTL_DeleteItem(pIMenuCtl, IDL_MENU_SETTINGS_DDTM);
#endif

#if !defined(FEATURE_WCDMA) && !defined(FEATURE_GSM_GPRS)
  IMENUCTL_DeleteItem(pIMenuCtl, IDL_MENU_SETTINGS_EDIT_PROFILE);
  IMENUCTL_DeleteItem(pIMenuCtl, IDL_MENU_SETTINGS_SELECT_ACTIVE);
#endif

  return TRUE;
}

static boolean CDataStatsApp_SaveDDTM(CDataStatsApp *pMe, const ITEMHANDLER *pItem)

/*lint -esym(715, pItem)*/
/*lint -esym(715, pMe)*/
{
#ifdef FEATURE_DDTM_CNTL
  AEECMDDTMPref newDDTM;
  IDialog *pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  //TODO: Error check for pMe, pIShell, pIDialog, pIMenuCtl , etc.
  IMenuCtl *pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_DDTM);
  uint16 curSel = IMENUCTL_GetSel(pIMenuCtl);
  
  
  PRINT_FUNCTION_NAME();
  (void) CDataStatsApp_EndDialog(pMe, NULL);
  newDDTM = (curSel == IDL_DDTM_ON)? AEECM_DDTM_PREF_ON : AEECM_DDTM_PREF_OFF;
  
  if(newDDTM != pMe->currentDDTM)
  {
    if(CheckAEEReturnStatus(ICM_SetDDTMPref(pMe->m_pICM, newDDTM)) == FALSE)
    {
      //ERROR DIALOG
      DATASTATS_ERR("Unable to set DDTM Pref");
      DATASTAT_ERROR_DLG("DDTM Pref Setting failed!");
      
    }
    else
    {
      DBGPRINTF("DDTM Pref Changed!");
      pMe->currentDDTM = newDDTM;
      
    }
  }
  else // No change DDTM
  {
    if(CheckAEEReturnStatus(ICM_SetDDTMPref(pMe->m_pICM, AEECM_DDTM_PREF_NO_CHANGE)) == FALSE)
    {
      DATASTATS_ERR("No Change DDTM setting failed");
      DATASTAT_ERROR_DLG("No Change DDTM setting failed");
    }
  }

  return TRUE;
#else
  return FALSE;
#endif
}

#ifdef FEATURE_DDTM_CNTL
static boolean CDataStatsApp_InitDDTMMenu(CDataStatsApp *pMe)
{
  AEECMPhInfo phInfo;
  CtlAddItem pai;
  IDialog *pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  //TODO: Error check for pMe, pIShell, pIDialog, pIMenuCtl , etc.
  IMenuCtl *pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_DDTM);

  if(CheckAEEReturnStatus(ICM_GetPhoneInfo(pMe->m_pICM, &phInfo, sizeof(AEECMPhInfo)) == FALSE))
  {
    DATASTATS_ERR("Unable to Get phone Info");
    return FALSE;
  }
  
  pMe->currentDDTM = phInfo.ddtm_pref;

  if( phInfo.ddtm_pref == AEECM_DDTM_PREF_ON)
  {
    SetMenuIcon(pIMenuCtl, IDL_DDTM_ON, TRUE);
    SetMenuIcon(pIMenuCtl, IDL_DDTM_OFF, FALSE);
  
    IMENUCTL_GetItem(pIMenuCtl, IDL_DDTM_ON, &pai);
    pai.wFont = AEE_FONT_BOLD;
    IMENUCTL_SetItem(pIMenuCtl,IDL_DDTM_ON, MSIF_FONT, &pai);

  }
  else if(phInfo.ddtm_pref == AEECM_DDTM_PREF_OFF)
  {
    SetMenuIcon(pIMenuCtl, IDL_DDTM_ON, FALSE);
    SetMenuIcon(pIMenuCtl, IDL_DDTM_OFF, TRUE);

    IMENUCTL_GetItem(pIMenuCtl, IDL_DDTM_OFF, &pai);
    pai.wFont = AEE_FONT_BOLD;
    IMENUCTL_SetItem(pIMenuCtl,IDL_DDTM_OFF, MSIF_FONT, &pai);
  }
  else
  {
    SetMenuIcon(pIMenuCtl, IDL_DDTM_ON, FALSE);
    SetMenuIcon(pIMenuCtl, IDL_DDTM_OFF, TRUE);

    IMENUCTL_GetItem(pIMenuCtl, IDL_DDTM_OFF, &pai);
    pai.wFont = AEE_FONT_BOLD;
    IMENUCTL_SetItem(pIMenuCtl,IDL_DDTM_OFF, MSIF_FONT, &pai);

    DATASTATS_ERR("Invalid DDTM to start with");
    MSG_ERROR("DDTM Setting: %d", phInfo.ddtm_pref, 0 , 0);
  }
  return TRUE;

}
#endif

  /*===========================================================================
FUNCTION CDataStatsApp_SaveProfileName

DESCRIPTION
  1. Store in pMe, the user-entered Name.
  2. End the EditName dialog.
  3. Modify the text for the ViewProfile screen, as per new Name.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_SaveProfileName(CDataStatsApp *pMe, const ITEMHANDLER *pItem)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  IDialog *pIDialog;
  ITextCtl *pITextCtl;
  IMenuCtl *pIMenuCtl = NULL;
  AECHAR *szBuf;

  PRINT_FUNCTION_NAME();

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  
  if(pIDialog != NULL)
  {
      
    pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIDialog, IDC_TEXT_NEW_PROFILE_NAME);
      
    if(pITextCtl != NULL)
    {
      int maxNameLen = IPDP_GetMaxProfileNameLen(pMe->m_pIPDP);

      szBuf = (AECHAR*) MALLOC(2*DATASTATS_MAX_BUFFER_SIZE*sizeof(AECHAR));
      if(szBuf == NULL)
      {
        DATASTATS_ERR("Malloc Failed");
        return FALSE;
      }

      ITEXTCTL_GetText(pITextCtl, pMe->profileText, maxNameLen);

      (void) CDataStatsApp_EndDialog(pMe, NULL);
      //Change the text for the Profile View Dialog.
      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
              
      if(pIDialog != NULL)
      {
        ISHELL_LoadResString(pMe->a.m_pIShell, DATASTATSAPP_RES_FILE, IDS_DD_NAME, 
          szBuf, DATASTATS_MAX_WIDE_BUFFER_SIZE);
        (void) WSTRLCAT(szBuf, pMe->profileText, DATASTATS_MAX_WIDE_BUFFER_SIZE);
        pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_MENU_VIEW_PROFILE);
        
        if(pIMenuCtl != NULL)
        {
      
          IMENUCTL_SetItemText(pIMenuCtl, IDL_MENU_VIEW_PROFILE_NAME, DATASTATSAPP_RES_FILE, 0, szBuf);
          IMENUCTL_Redraw(pIMenuCtl);
          FREE(szBuf);
          return TRUE;
        }
        else
        {
          DATASTATS_ERR("pIMenuCtl NULL");
        }
      }
      else
      {
        DATASTATS_ERR("pIDialog NULL");
      }
      FREE(szBuf);
    }
    else
    {
        DATASTATS_ERR("pITextCtl NULL");
    }

  }
  else
  {
    DATASTATS_ERR("pIDialog NULL");
  }
#endif
  return FALSE;
}


static boolean CDataStatsApp_GetLastDataStats(CDataStatsApp *pMe)
{
  IConfig *pIConfig;
  uint8 ret = SUCCESS;
  ret = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_CONFIG,
                               (void **) &pIConfig);

  if ( ret != SUCCESS) {
      MSG_MED("Unable to create IConfig %d", ret, 0, 0);
      return FALSE;
  }   

  if (ICONFIG_GetItem(pIConfig, CFGI_TOTAL_TX_DATA_COUNT, &pMe->m_dwTxTotalBytes,
                      sizeof(uint32)) != AEE_SUCCESS) {
      MSG_MED("Unable to read total bytes received", 0, 0, 0);
  }
    
  if (ICONFIG_GetItem(pIConfig, CFGI_TOTAL_RX_DATA_COUNT, &pMe->m_dwRxTotalBytes,
                      sizeof(uint32)) != AEE_SUCCESS) {
      MSG_MED("Unable to read total bytes received", 0, 0, 0);
  }

  if (ICONFIG_GetItem(pIConfig, CFGI_LAST_TX_DATA_COUNT, &pMe->m_dwTxLastCount,
                      sizeof(uint32)) != AEE_SUCCESS) {
      MSG_MED("Unable to read total bytes received", 0, 0, 0);
  }

  if (ICONFIG_GetItem(pIConfig, CFGI_LAST_RX_DATA_COUNT, &pMe->m_dwRxLastCount,
                      sizeof(uint32)) != AEE_SUCCESS) {
      MSG_MED("Unable to read total bytes received", 0, 0, 0);
  }
  return ret;
}

static boolean CDataStatsApp_ResetLastDataStats(CDataStatsApp *pMe)
{
  IConfig *pIConfig;
  uint8 ret = SUCCESS;
  uint32 total_count =0;
  ret = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_CONFIG,
                               (void **) &pIConfig);

  if ( ret != SUCCESS) {
      MSG_MED("Unable to create IConfig %d", ret, 0, 0);
      return FALSE;
  }   

  if (ICONFIG_SetItem(pIConfig, CFGI_TOTAL_TX_DATA_COUNT, &total_count,
                      sizeof(uint32)) != AEE_SUCCESS) {
      MSG_MED("Unable to read total bytes received", 0, 0, 0);
  }
    
  if (ICONFIG_SetItem(pIConfig, CFGI_TOTAL_RX_DATA_COUNT, &total_count,
                      sizeof(uint32)) != AEE_SUCCESS) {
      MSG_MED("Unable to read total bytes received", 0, 0, 0);
  }

  if (ICONFIG_SetItem(pIConfig, CFGI_LAST_TX_DATA_COUNT, &total_count,
                      sizeof(uint32)) != AEE_SUCCESS) {
      MSG_MED("Unable to read total bytes received", 0, 0, 0);
  }

  if (ICONFIG_SetItem(pIConfig, CFGI_LAST_RX_DATA_COUNT, &total_count,
                      sizeof(uint32)) != AEE_SUCCESS) {
      MSG_MED("Unable to read total bytes received", 0, 0, 0);
  }
  return ret;
}
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
/*===========================================================================
FUNCTION CDataStatsApp_SaveQoS

DESCRIPTION
  Copy the user selection to the PDP profile.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void CDataStatsApp_SaveQoS(CDataStatsApp *pMe, AEEPDP *pdpProfile, uint32 *dwFields)
{
  if (pMe->m_UMTSQoSReq.valid_flg){
    MEMCPY(&pdpProfile->qos_request_umts, &pMe->m_UMTSQoSReq, sizeof(AEEPDPUMTSQOS));
    *dwFields |= AEEPDP_PROFILE_UMTS_REQ_QOS;
  }
  if (pMe->m_UMTSQoSMin.valid_flg){
    MEMCPY(&pdpProfile->qos_minimum_umts, &pMe->m_UMTSQoSMin, sizeof(AEEPDPUMTSQOS));
    *dwFields |= AEEPDP_PROFILE_UMTS_MIN_QOS;
  }
  if (pMe->m_GPRSQoSMin.valid_flg){   
    MEMCPY(&pdpProfile->qos_minimum_gprs, &pMe->m_GPRSQoSMin, sizeof(AEEPDPGPRSQOS));
    *dwFields |= AEEPDP_PROFILE_GPRS_MIN_QOS;
  }
  if (pMe->m_GPRSQoSReq.valid_flg){   
    MEMCPY(&pdpProfile->qos_request_gprs, &pMe->m_GPRSQoSReq, sizeof(AEEPDPGPRSQOS));
    *dwFields |= AEEPDP_PROFILE_GPRS_REQ_QOS;
  }

}
#endif  
/*===========================================================================
FUNCTION CDataStatsApp_ViewQoS

DESCRIPTION
  Set the QOS type and the dialog ID.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean CDataStatsApp_ViewQoS(CDataStatsApp *pMe, const ITEMHANDLER *pItem)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
  boolean retVal = TRUE;
  uint16 wDlgID = 0;

  PRINT_FUNCTION_NAME();
  
   switch(pItem->wItemId)
  {
  case IDL_MENU_EDIT_QOS_UMTS_REQ:
      pMe->nQoSType  = AEEPDP_QOS_UMTS_REQ;
      wDlgID = IDD_EDIT_QOS_UMTS; 
      break;

  case IDL_MENU_EDIT_QOS_UMTS_MIN:
      pMe->nQoSType  = AEEPDP_QOS_UMTS_MIN;
      wDlgID = IDD_EDIT_QOS_UMTS;
      break;

  case IDL_MENU_EDIT_QOS_GPRS_REQ:
      pMe->nQoSType  = AEEPDP_QOS_GPRS_REQ;
      wDlgID = IDD_EDIT_QOS_GPRS;
      break;

  case IDL_MENU_EDIT_QOS_GPRS_MIN:
      pMe->nQoSType  = AEEPDP_QOS_GPRS_MIN;
      wDlgID = IDD_EDIT_QOS_GPRS;
      break;

  default:
      DATASTATS_ERR("Unknown Selection");
      break;
  }


  return retVal & CDataStatsApp_CreateDialog(pMe, wDlgID);
#else
  return FALSE;
#endif
}

//lint -restore
