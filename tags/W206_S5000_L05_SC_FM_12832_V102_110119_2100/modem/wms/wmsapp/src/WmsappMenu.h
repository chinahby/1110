#ifndef WMSAPPMENU_H
#define WMSAPPMENU_H
/*=============================================================================

FILE: WMSAppMenu.h

SERVICES: QCT UI WMS BREW Application.

GENERAL DESCRIPTION:
.h file for WMSAPPMenu

PUBLIC CLASSES AND STATIC FUNCTIONS:


INITIALIZATION AND SEQUENCING REQUIREMENTS:



(c) COPYRIGHT 2003, 2004, 2005, 2006, 2007, 2008 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
=============================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/wms/wmsapp/main/latest/src/WmsappMenu.h#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/12/08   PMD     Removed wms_bc_*** APIs for CDMA Broadcast
06/04/08   PMD     Removed support for Auto / Transparent SMS
01/31/08   PMD     Updated Copyright Information
02/22/07   JAS     Featurize Annunciator references for uiOne HDK builds
01/17/07   HQ      Updated copyright info for 2007.
01/25/06   VK      Updated Copyright Info
10/10/05   PMD     Added support for Retry Settings Dialog
03/08/05   PMD     Added Prototype: WMSAPPMN_PrepareCDMAMOMsgForAccessChannel
03/05/05   Rex     Exposed WMSAPPMN_ProcessCardError
11/05/04   HQ      Corrected include file name cases for proper compilation.
03/16/04   AP      Added support for the user app
02/03/04   PMD     Added prototype: WMSAPPMN_CheckReturnDlg()
01/14/04   PMD     Added New Dialog for Segmentation Settings
01/06/04   Rex     Changed function WMSAPPMN_DisplayMsgDlg to accomodate dialog display time
11/21/03   PMD     Added GSM Cell Broadcast Support
11/06/03   AP      Added event handler for auto DC send SO selection.
10/31/03   PMD     Added New Alert Settings Dialogs and Event Handlers
                   Created new Event Handler for Settings Dialog
10/30/03   PMD     Added New Dialogs and Event Handlers for Validity Cleanup
10/29/03   AP      Added support for FEATURE_SMS_TRANSPARENT_MO.
09/19/03   PMD     Added Dialog Handler for GW Domain Pref Dialog
                   Exposed WMSAPPMN_InitNewMTMsg()
07/29/03   AP      Added prototypes for DC functions. Defined event handler for
                   mode select dialog.
07/16/03   PMD     Added Event Handlers for 2 new Dialogs - About and Splash
07/07/03   PMD     Changes to remove / suppress Lint Errors
06/27/03   PMD     Changed function WMSAPPMN_DisplayMsgDlg to accomodate text
06/05/03   SUN     Added support for FEATURE_GWSMS_STATUS_REPORTS
05/27/03   PMD     Added New Dialogs and handlers for Predefined Animations
                   and User-Defined melody
05/20/03   SUN     Exposed WMSAPPMN_GetTemplateListResID
05/09/03   SUN     Code Cleanup
03/10/03   SUN     Added support for Memory Usage
03/04/03   AP      Add voice mail support.
02/27/03   AP      Clean up.
02/14/02   SUN     Initial Revision
===========================================================================*/

/*=============================================================================

                       INCLUDES

=============================================================================*/

/* Brew libraries */

#ifdef FEATURE_WMS_APP
#include "AEEAppGen.h"        // Applet helper file
#include "WMSAPP.BID"
#include "AEE.h"
#include "AEEStdLib.h"    // contain FREE function
#include "AEEAppGen.h"        // Applet helper file
#include "AEEMenu.h"
#include "AEEShell.h"
#ifdef FEATURE_ANNUNCIATOR
#include "AEEAnnunciator.h"
#endif /* FEATURE_ANNUNCIATOR */
#include "AEEWMS.h"
#include "WmsApp.h"




boolean WMSAPPMN_IdleDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_MenuDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_MenuSoftkeyDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_TextSoftKeyDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_RadioBtnDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_DestAddrDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_ComposeDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_TestMenuDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_StaticDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_ViewSMSDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_ViewImageDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_ViewMsgInfoDlgEvtHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_MsgTypeDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_OutboxOptionsDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_MsgAccessDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_MsgDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_PrefDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_TmpltDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_MelodyDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_BCDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_BCEditDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_SimpleTextDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_ReplyOptionsDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_ConfirmDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_BCServicesDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_NewMtMsgDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_TestSendMsgEvtHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_BCServiceDisplayEvtHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_BCAddServiceEvtHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_ValidityFormatDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_MemUsageDlgEvtHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_RoutesDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_ImageDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_ServiceEditDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_CBMenuDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_UserRelValEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_AbsValDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_SettingsDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
boolean WMSAPPMN_AlertSettingsDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);

typedef boolean (*PFNDLGHANDLER) (
                                 void *pUser,
                                 AEEEvent evt,
                                 uint16 w,
                                 uint32 dw
                                 );


typedef struct
{
  wms_cmd_id_e_type             cmd;
  void                          *user_data;
  wms_cmd_err_e_type            cmd_err;
} wmsapp_cmd_err_type;


/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/
#define WMSAPPAPPTIMEOUT 50000  /* Time out value for bringing the program back to */
#define SELECTIONVERIFICATIONTIME 2000
#define SELECTION_TIME_IN_SECONDS (SELECTIONVERIFICATIONTIME / 1000)

#define DIALOG_DISPLAY_TIME_LONGER 3000

#define WMS_MSG(format, code1, code2, code3) MSG_MED(format, code1, code2, code3)
#define WMS_ERROR(format, code1, code2, code3) MSG_ERROR(format, code1, code2, code3)
#define SMS_TEXT_SIZE 160
#define WMSAPP_NO_PREF  0
#define WMSAPP_CONFIRM_NO       0
#define WMSAPP_CONFIRM_YES      1
#define WMSAPP_CONFIRM_NONE     2

#ifdef FEATURE_CDSMS_BROADCAST
#define WMSAPP_BC_ALL_LANGUAGES 0xFF
#define WMSAPP_BC_SINGLE_SVC    0xFE
#define WMSAPP_BC_NO_INDEX      0xFF
#endif /* FEATURE_CDSMS_BROADCAST */


static const PFNDLGHANDLER SMSDlgEventHandlers[] = {
  NULL,
  WMSAPPMN_IdleDlgEventHandler,               /* 1  IDD_MAINSMS */
  WMSAPPMN_SettingsDlgEventHandler,           /* 2  IDD_SETTING */
  WMSAPPMN_MenuDlgEventHandler,               /* 3  IDD_MSG_CLASSES*/
  WMSAPPMN_RadioBtnDlgEventHandler,           /* 4  IDD_ROUTE*/
  WMSAPPMN_RoutesDlgEventHandler,             /* 5  IDD_ROUTES_SETTINGS*/
  WMSAPPMN_RadioBtnDlgEventHandler,           /* 6  IDD_MEMORY_STORE*/
  WMSAPPMN_DestAddrDlgEventHandler,           /* 7  IDD_DESTADDR*/
  WMSAPPMN_MsgTypeDlgEventHandler,            /* 8  IDD_MESSAGE_TYPE*/
  WMSAPPMN_TmpltDlgEventHandler,              /* 9  IDD_TEMPLATE*/
  WMSAPPMN_PrefDlgEventHandler       ,        /* 10 IDD_GW_OUTBOX_PREF*/
  WMSAPPMN_RadioBtnDlgEventHandler,           /* 11 IDD_REJECT_DUP*/
  WMSAPPMN_ValidityFormatDlgEventHandler,     /* 12 IDD_VALIDITY*/
  WMSAPPMN_RadioBtnDlgEventHandler,           /* 13 IDD_REPLY_PATH*/
  WMSAPPMN_RadioBtnDlgEventHandler,           /* 14 IDD_PID*/
  WMSAPPMN_RadioBtnDlgEventHandler,           /* 15 IDD_DCS*/
  WMSAPPMN_UserRelValEventHandler,            /* 16 IDD_REL_VAL_USER_DEFINED*/
  WMSAPPMN_RadioBtnDlgEventHandler,           /* 17 IDD_VALIDITY_TIMEZONE*/
  WMSAPPMN_PrefDlgEventHandler,               /* 18 IDD_GW_TEMPLATE_OPTIONS*/
  WMSAPPMN_StaticDlgEventHandler,             /* 19 IDD_OTHERINFO*/
  WMSAPPMN_OutboxOptionsDlgEventHandler,      /* 20 IDD_OUTBOX_OPTIONS*/
  WMSAPPMN_TextSoftKeyDlgEventHandler,        /* 21 IDD_TEMPLATE_ALPHA_TAG*/
  WMSAPPMN_RadioBtnDlgEventHandler,           /* 22 IDD_DESTADDR_TYPE*/
  WMSAPPMN_RadioBtnDlgEventHandler,           /* 23 IDD_DESTADDR_PLAN*/
  WMSAPPMN_DestAddrDlgEventHandler,           /* 24 IDD_TEMPLATE_DESTADDR*/
  WMSAPPMN_RadioBtnDlgEventHandler,           /* 25 IDD_SCADDR_TYPE*/
  WMSAPPMN_RadioBtnDlgEventHandler,           /* 26 IDD_SCADDR_PLAN*/
  WMSAPPMN_DestAddrDlgEventHandler,           /* 27 IDD_TEMPLATE_SCADDR*/
  WMSAPPMN_MelodyDlgEventHandler,             /* 28 IDD_LIST_USER_MELODY*/
  WMSAPPMN_ViewImageDlgEventHandler,          /* 29 IDD_VIEW_PREDEF_ANIM*/
  WMSAPPMN_MenuSoftkeyDlgEventHandler,        /* 30 IDD_TEMPLATE_DCS_ALPHABET*/
  WMSAPPMN_RadioBtnDlgEventHandler,           /* 31 IDD_ENCODING_TYPES */
  WMSAPPMN_MsgAccessDlgEventHandler,          /* 32 IDD_CARD_MESSAGE_ACCESS*/
  WMSAPPMN_ViewSMSDlgEventHandler,            /* 33 IDD_INBOX_VIEW*/
  WMSAPPMN_ComposeDlgEventHandler,            /* 34 IDD_COMPOSE_MSG*/
  WMSAPPMN_MenuDlgEventHandler,               /* 35 IDD_LIST_ATTACHMENTS*/
  WMSAPPMN_ViewImageDlgEventHandler,          /* 36 IDD_VIEW_SMALL_PICTURES*/
  WMSAPPMN_ViewSMSDlgEventHandler,            /* 37 IDD_OUTBOX_VIEW*/
  WMSAPPMN_MelodyDlgEventHandler,             /* 38 IDD_LIST_MELODY*/
  NULL,                                       /* 39 IDD_CALL*/
  WMSAPPMN_TestMenuDlgEventHandler,           /* 40 IDD_EMS_TEST*/
  WMSAPPMN_ViewMsgInfoDlgEvtHandler,          /* 41 IDD_VIEW_MESSAGE_INFO */
  WMSAPPMN_StaticDlgEventHandler,             /* 42 IDD_VIEW_TMPLT*/
  WMSAPPMN_ViewImageDlgEventHandler,          /* 43 IDD_VIEW_LARGE_PICTURES*/
  WMSAPPMN_ViewImageDlgEventHandler,          /* 44 IDD_VIEW_VAR_PICTURES*/
  WMSAPPMN_ViewImageDlgEventHandler,          /* 45 IDD_VIEW_SMALL_ANIMATION*/
  WMSAPPMN_ViewImageDlgEventHandler,          /* 46 IDD_VIEW_LARGE_ANIMATION*/
  WMSAPPMN_MsgAccessDlgEventHandler,          /* 47 IDD_NV_MESSAGE_ACCESS*/
  WMSAPPMN_MsgAccessDlgEventHandler,          /* 48 IDD_TEMP_MESSAGE_ACCESS*/
  WMSAPPMN_PrefDlgEventHandler,               /* 49 IDD_CDMA_OUTBOX_PREF*/
  WMSAPPMN_RadioBtnDlgEventHandler,           /* 50 IDD_TELESERVICE*/
  WMSAPPMN_StaticDlgEventHandler  ,           /* 51 IDD_STATUS_REPORT*/
  WMSAPPMN_RadioBtnDlgEventHandler,           /* 52 IDD_MSG_COMMAND*/
  WMSAPPMN_RadioBtnDlgEventHandler,           /* 53 IDD_CDMA_DCS*/
  WMSAPPMN_RadioBtnDlgEventHandler,           /* 54 IDD_PRIORITY*/
  WMSAPPMN_RadioBtnDlgEventHandler,           /* 55 IDD_PRIVACY*/
  WMSAPPMN_RadioBtnDlgEventHandler,           /* 56 IDD_LANGUAGE*/
  WMSAPPMN_MsgDlgEventHandler,                /* 57 IDD_MESSAGE*/
  WMSAPPMN_BCDlgEventHandler  ,               /* 58 IDD_BC_SMS*/
  WMSAPPMN_BCEditDlgEventHandler,             /* 59 IDD_BC_EDIT_TABLE*/
  WMSAPPMN_PrefDlgEventHandler,               /* 60 IDD_CDMA_TEMPLATE_OPTIONS*/
  WMSAPPMN_BCServicesDlgEventHandler,         /* 61 IDD_BC_SERVICES*/
  WMSAPPMN_SimpleTextDlgEventHandler,         /* 62 IDD_SIMPLE_TEXT */
  WMSAPPMN_RadioBtnDlgEventHandler,           /* 63 IDD_REL_VALIDITY*/
  WMSAPPMN_AbsValDlgEventHandler,             /* 64 IDD_ABS_VALIDITY*/
  WMSAPPMN_RadioBtnDlgEventHandler,           /* 65 IDD_VALIDITY_YEAR*/
  WMSAPPMN_RadioBtnDlgEventHandler,           /* 66 IDD_VALIDITY_MONTH*/
  WMSAPPMN_RadioBtnDlgEventHandler,           /* 67 IDD_VALIDITY_DAY*/
  WMSAPPMN_RadioBtnDlgEventHandler,           /* 68 IDD_VALIDITY_HOUR*/
  WMSAPPMN_RadioBtnDlgEventHandler,           /* 69 IDD_VALIDITY_MINUTE*/
  WMSAPPMN_ReplyOptionsDlgEventHandler,       /* 70 IDD_REPLY_OPTIONS */
  WMSAPPMN_RadioBtnDlgEventHandler,           /* 71 IDD_DISPLAY_MODE */
  WMSAPPMN_RadioBtnDlgEventHandler,           /* 72 IDD_ALERT_MODE_637A */
  WMSAPPMN_BCDlgEventHandler,                 /* 73 IDD_BC_CONFIG */
  WMSAPPMN_BCDlgEventHandler,                 /* 74 IDD_BC_PREF */
  WMSAPPMN_BCDlgEventHandler,                 /* 75 IDD_BC_TABLE */
  WMSAPPMN_ConfirmDlgEventHandler,            /* 76 IDD_CONFIRM */
  WMSAPPMN_RadioBtnDlgEventHandler,           /* 77 IDD_BC_MAX_MSGS */
  WMSAPPMN_NewMtMsgDlgEventHandler,           /* 78 IDD_NEW_MSG_RXD */
  WMSAPPMN_MenuSoftkeyDlgEventHandler,        /* 79 IDD_EDIT_DESTADDR*/
  WMSAPPMN_MenuSoftkeyDlgEventHandler,        /* 80 IDD_EDIT_SCADDR*/
  WMSAPPMN_TestMenuDlgEventHandler,           /* 81 IDD_CDMA_TEST*/
  WMSAPPMN_TestMenuDlgEventHandler,           /* 82 IDD_GW_TEST*/
  WMSAPPMN_MenuDlgEventHandler,               /* 83 IDD_TEST*/
  WMSAPPMN_TestSendMsgEvtHandler,             /* 84 IDD_TEST_ADDRESS*/
  WMSAPPMN_TestSendMsgEvtHandler,             /* 85 IDD_TEST_NUMBER_OF_CHARS*/
  WMSAPPMN_BCServiceDisplayEvtHandler,        /* 86 IDD_BC_SVC_DISPLAY */
  WMSAPPMN_BCAddServiceEvtHandler,            /* 87 IDD_BC_ADD_SVC */
  WMSAPPMN_RadioBtnDlgEventHandler,           /* 88 IDD_BC_AVAILABLE_SVCS */
  WMSAPPMN_TextSoftKeyDlgEventHandler,        /* 89 IDD_VOICEMAIL_NUMBER*/
  WMSAPPMN_MemUsageDlgEvtHandler,             /* 90 IDD_USAGE*/
  WMSAPPMN_StaticDlgEventHandler,             /* 91 IDD_DCS_SUMMARY*/
  WMSAPPMN_ImageDlgEventHandler,              /* 92 IDD_ABOUT*/
  WMSAPPMN_ImageDlgEventHandler,              /* 93 IDD_SPLASH*/
  WMSAPPMN_RadioBtnDlgEventHandler,           /* 94 IDD_MODE_SELECT */
  WMSAPPMN_MenuSoftkeyDlgEventHandler,        /* 95 IDD_CB_SETTINGS */
  WMSAPPMN_CBMenuDlgEventHandler,             /* 96 IDD_CB_SERVICES */
  WMSAPPMN_ServiceEditDlgEventHandler,        /* 97 IDD_CB_SERVICE_INFO */
  WMSAPPMN_CBMenuDlgEventHandler,             /* 98 IDD_BROADCAST_MESSAGE_ACCESS */
  WMSAPPMN_StaticDlgEventHandler,             /* 99 IDD_BROADCAST_MESSAGE */
  WMSAPPMN_StaticDlgEventHandler,             /* 100 IDD_BROADCAST_MESSAGE_OTHER_INFO */
  WMSAPPMN_RadioBtnDlgEventHandler,           /* 101 IDD_DOMAIN_PREF */
  WMSAPPMN_RadioBtnDlgEventHandler,           /* 102 IDD_AUTO_MEM_STORE */
  WMSAPPMN_AlertSettingsDlgEventHandler,      /* 103 IDD_ALERT_SETTINGS */
  WMSAPPMN_RadioBtnDlgEventHandler,           /* 104 IDD_AUTO_DC_SEND_SO */
  NULL,                                       /* 105 IDD_COMPOSE_MENU */
  WMSAPPMN_StaticDlgEventHandler,             /* 106 IDD_AUTO_STATUS */
  WMSAPPMN_StaticDlgEventHandler,             /* 107 IDD_AUTO_OTHER_INFO */
  NULL,                                       /* 108 IDD_AUTO_VOICEMAIL */
  WMSAPPMN_RadioBtnDlgEventHandler,           /* 109 IDD_SEGMENT_SETTING */
  WMSAPPMN_StaticDlgEventHandler,             /* 110 IDD_AUTO_REQUEST_VOICEMAIL_NUMBER */
  NULL,                                       /* 111 IDD_INTERVAL */
  NULL,                                       /* 112 IDD_TIME_PERIOD */
  NULL,                                       /* 113 IDD_MSG_ROUTES_CDMA */
  NULL,                                       /* 114 IDD_NUMBER_OF_MSGS */
  WMSAPPMN_RadioBtnDlgEventHandler            /* 115 IDD_RETRY_SETTINGS */
};

boolean DisplaySMSIdleMenuDlg
(
  CWMSAPP *pMe
);

boolean WMSAPPMN_SendEvtToDlg
(
  CWMSAPP *pMe,
  AEEEvent eCode,
  uint16 wParam,
  uint32 dwParam
);

int WMSAPPMN_ProcessError
(
wms_cmd_id_e_type        cmd,
/* which command's status being reported
*/
void                    *user_data,
wms_cmd_err_e_type       cmd_err
/* the status of the command
*/
);

boolean WMSAPPMN_ProcessCardError
(
  void      *pNotifyData
);

boolean WMSAPPMN_DisplayMsgDlg
(
  CWMSAPP *pMe,
  uint16 wResID,
  char *text,
  int displayTimeMSec
);


int WMSAPPMN_DisplayError
(
  CWMSAPP *pMe,
  wms_cmd_id_e_type cmd,
  wms_cmd_err_e_type cmd_err
);

boolean WMSAPPMN_ProcessMTMessage
(
  CWMSAPP *pMe,
  wms_msg_event_info_s_type  *dwParam
);

uint16 WMSAPPMN_GetTemplateListResID
(
  int index
);

boolean WMSAPPMN_DcConnect
(
  CWMSAPP *pMe,
  wms_dc_so_e_type so

);

boolean WMSAPPMN_DcDisconnect
(
  CWMSAPP *pMe
);

boolean WMSAPPMN_SendMessage
(
  CWMSAPP *pMe
);

boolean WMSAPPMN_InitNewMtMsg
(
  CWMSAPP *pMe
);

boolean WMSAPPMN_CheckReturnDlg
(
  CWMSAPP *pMe
);

boolean WMSAPPMN_PrepareCDMAMOMsgForAccessChannel
(
  CWMSAPP *pMe,
  uint8 numberOfChars
);

#endif /*FEATURE_WMS_APP*/
#endif /* WMSAPPMENU_H */
