#ifndef WMSAPP_H
#define WMSAPP_H
/*=============================================================================

FILE: WMSApp.h

SERVICES: QCT UI WMS BREW Application.

GENERAL DESCRIPTION:
.h file for WMSAPP

PUBLIC CLASSES AND STATIC FUNCTIONS:


INITIALIZATION AND SEQUENCING REQUIREMENTS:



(c) COPYRIGHT 2003, 2004, 2005, 2006, 2007, 2008 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
=============================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/wms/wmsapp/main/latest/inc_deprecated/WmsApp.h#6 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
06/27/08   PMD     Cleaned up Feature Flags in Code
06/12/08   PMD     Removed wms_bc_*** APIs for CDMA Broadcast
06/04/08   PMD     Removed support for Auto / Transparent SMS 
01/31/08   PMD     Updated Copyright Information
11/14/07   PMD     Fixed Viewer / Composer size params for Higher Resolution
02/22/07   JAS     Featurize Annunciator references for uiOne HDK builds
01/17/07   HQ      Updated copyright info for 2007.
12/06/06   PMD     Added support for Primary Client Set Event
04/27/06   HQ      Mainlined FEATURE_SMS_RETRY.
01/30/06   PMD     Changed Retry Period from uint16 to uint32
01/25/06   VK      Updated Copyright Info
01/25/06   VK      Lint fixes
10/19/05   VK      EMS Featurization fixes
07/26/05   Rex     Featurization fixes
06/06/05   Rex     Added fixes for voicemail
05/04/05   PMD     Added support for Any Update Number is Change
04/21/05   VK      Compiler Errors
04/20/05   VK      Message Tests
03/11/05   PMD     Inserted F_SMS_LTK for LTK Compilation
03/05/05   Rex     Added VMWI and Mailbox Number support
02/24/05   VK      Added Test menu support for checking access channel msg sizes
02/20/05   PMD     Added 1x Broadcast Support in Multimode Broadcast
02/01/05   PMD     Added Support for UMTS BMC Reading Preferences
01/15/05   VK      Added support for secondary display
01/12/05   Rex     Fixed for CPHS
01/07/05   PMD     Support to Not Play IMelody / Beep Alerts during Voice / Video Calls
11/05/04   HQ      Corrected include file name cases for proper compilation.
10/16/04   PMD     Seperated CDMA and GW Broadcast Message Notification.
10/12/04   Rex     Added boolean type for segment EO
09/24/04   Rex     Fix for CPHS
09/21/04   Rex     Added support for CPHS
08/05/04   Rex     Featureize IWMSDIAG
07/21/04   PMD     Changed CurImage to store WMS_MAX_UD_HEADERS IImage ptrs
07/15/04   PMD     Added New Event for Duplicate CB Page
06/11/04   PMD     Added new #define for GW CB Page Size
06/08/04   PMD     Added Support for Clearing Immediate Display on Deletion.
05/27/04   Rex     Added support for IWMSDIAG interface
03/22/04   Rex     Modify structure wmsapp_cb_msg_info_cache_s_type to include
                   wms_gw_cb_page_header_s_type for displaying page number/total page
03/22/04   PMD     Added Update field in CB RAM Messages
03/22/04   PMD     Changed Display handling by selectively calling SetIdleMenuText()
03/16/04   AP      Added support for the user app
02/14/04   PMD     Added Support for Retry Settings
02/03/04   PMD     Removed FEATURE_SMS_SEGMENT
01/27/04   PMD     Added Support for Immediate Display Broadcast Message
01/27/04   PMD     Added support for Compose from other Apps
01/26/04   PMD     Added Segmentation Definitions
11/21/03   PMD     Added GSM Cell Broadcast Support
11/13/03   PMD     Added Events for Message Read and Message Compose
11/10/03   PMD     Added Instance of ICM in CWMSAPP Structure
11/06/04   AP      Added auto DC send SO variable.
11/06/03   Rex     Status Report Display Changes
11/04/03   PMD     Added Support for CDMA and GW Default Templates using ICFG
11/03/03   PMD     Seperated the CDMA and GW Template List and Default Template
10/31/03   PMD     Added new parameters in pMe for various Alert Settings
10/31/03   PMD     Removed Definitions for Voicemail
10/29/03   AP      Added support for FEATURE_SMS_TRANSPARENT_MO.
10/22/03   PMD     Removed Prototype for MsgInfoCacheCb
10/03/03   AP      Fixed handling of voicemail MT messages: 0 voicemails and
                   voicemail alert display.
09/19/03   PMD     Added Support for GW Domain Preference
                   Added Different Notification Types for Incoming Message,
                   Broadcast, Voicemail and Memory Full.
09/16/03   PMD     Removed FEATURE_CDSMS_NON_STD_EMAIL in CWMSAPP Structure
08/15/03   PMD     Added a Memory Status list for Status Report
08/06/03   PMD     Added Event for Status Report Deletion
07/29/03   AP      Moved definition of auto disconnect.  Added flags to handle
                   DC auto disconnect and DC auto send.  Added event handler for
                   mode select dialog.  Added definition of DC events.
07/17/03   PMD     Added a flag to display Splash Screen or not
07/09/03   AP      Added a flag when replying to a message.
06/26/03   PMD     Created new field in WMSAPP pMe for Alpha ID
06/25/03   AP      Added alpha id buffer.
06/10/03   PMD     Included "AEEFile.h" for performing file operations
06/05/03   SUN     Added support for FEATURE_GWSMS_STATUS_REPORTS
06/02/03   PMD     Added pointer to IConfig in WMSAPP Structure
05/22/03   SUN     Added new NONE_TYPE Style Format
05/20/03   SUN     Changed structs to pointers in pMe
05/19/03   PMD     Added new varibles in CWMSAPP structure - Default Template
                   text and Pointer to ISound
05/09/03   SUN     Added new bitmask for style text, code clean up,
                   code review changes, added new element to pMe to
                   remember the current DCS class.
05/01/03   AP      Added prototype for WMSAPP_MsgInfoCacheCb.
04/03/03   SUN/AP  Clean up.
03/04/03   AP      Added member to add a BC service.
02/27/03   AP      Clean up.
02/14/02   SUN     Initial Revision
===========================================================================*/

/*=============================================================================

                       INCLUDES

=============================================================================*/

/* Brew libraries */

#ifdef FEATURE_WMS_APP
#include "customer.h"
#include "WMSAPP.BID"
#include "AEEAppGen.h"        // Applet helper file
#include "AEE.h"
#include "AEEStdLib.h"    // contain FREE function
#include "AEEMenu.h"
#include "AEEShell.h"
#ifdef FEATURE_ANNUNCIATOR
#include "AEEAnnunciator.h"
#endif /* FEATURE_ANNUNCIATOR */
#include "AEEWMS.h"
#include "AEEConfig.h"
#include "AEEFile.h"
#include "nv.h"
#include "AppComFunc.h"

#ifdef FEATURE_IWMSDIAG
#include "AEEWMSDIAG.h"
#endif /* FEATURE_IWMSDIAG */

#ifdef FEATURE_ICM
#include "AEECM.h"
#endif

#include "AEECard.h"

#ifdef FEATURE_QVPHONE
#include "AEEVideoPhone.h"
#endif

#define TITLE_BUFFER 30
#define TEXT_BUFFER 300
#define LINE_HEIGHT 40
#define START_POSITION 15
#define MELODY_SIZE 128
#define MELODY_TRANS_ID 20
#define WMSAPP_AUTO_DISCONNECT_TIME 20
#define NOTIFY_STRING_LEN 200
#define NOTIFY_STRING_SEC_DISPLAY  30


#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /*FEATURE_GWSMS_BROADCAST*/

typedef struct
{
  int curX;
  int curY;
  int curLine;
  int maxY[20];
  int maxX[20];
  uint16 length;
  boolean handleEvent;
  boolean isSpace;
  boolean isCaps;
}WMSAPP_DisplayInfo;

typedef enum
{
  WMSAPP_MSG_TYPE_NEW,
  WMSAPP_MSG_TYPE_EDIT,
  WMSAPP_MSG_TYPE_REPLY,
  WMSAPP_MSG_TYPE_FWD,
  WMSAPP_MSG_TYPE_CANNED
} wmsapp_msg_type;

typedef enum
{
  WMSAPP_OUTBOX,
  WMSAPP_INBOX,
  WMSAPP_TEMPLATE,
  WMSAPP_STATUS_REPORT,
  WMSAPP_BROADCAST,
  WMSAPP_NONE
}wmsapp_box_type;

/* comparison with wms_message_mode_e_type, enum values need to be mapped accordingly */
typedef enum
{
  WMSAPP_MODE_CDMA,
  WMSAPP_MODE_GW
}wmsapp_mode_e_type;

//SECONDARY DISPLAY
typedef enum
{
  WMSAPP_NEW_MESSAGE = 0,
  WMSAPP_NEW_VOICEMAIL,
  WMSAPP_NEW_VOICEMAIL_LINE2,
  WMSAPP_MEMORY_FULL,
  WMSAPP_NEW_MESSAGE_BROADCAST,
  WMSAPP_SENDING_MESSAGE,
  WMSAPP_MESSAGE_SENT,
  WMSAPP_MESSAGE_FAILED_LL,
  WMSAPP_NETWORK_NOT_READY,
  WMSAPP_PHONE_NOT_READY,
  WMSAPP_NO_TL_ACK,
  WMSAPP_TL_ERROR,
  WMSAPP_MSG_TOO_LARGE,
  WMSAPP_NOT_ALLOWED_IN_ANALOG_MODE,
  WMSAPP_CANNOT_SEND_BROADCAST_MESSAGE,
  WMSAPP_CP_ERROR,
  WMSAPP_RPT_RP_ERROR,
  WMSAPP_TR1_TIMEOUT,
  WMSAPP_TR2_TIMEOUT,
  WMSAPP_MSG_FAILED
}wmsapp_display_state_e_type;


typedef struct
{
  boolean isNonStdEmail;
  uint8  address[WMS_MAX_LEN];
  uint8  len;
}wmsapp_email_address_s_type;

typedef struct
{
  byte                     smsIndex;   /* Index to SMS list item */
  wms_memory_store_e_type  memStore;
  int                      inboxCount;
  int                      outboxCount;
  wmsapp_box_type          boxType;
  wms_write_mode_e_type    write_mode;
} WMSAPP_CurMsgListInfo;

enum { WMS_SMS_UDL_MAX_7_BIT  = 160 }; /* as in the spec */
enum { WMS_SMS_UDL_MAX_8_BIT  = 140 }; /* as in the spec */

typedef enum
{
  WMSAPP_NOTIFY_NEW_MESSAGE,
  WMSAPP_NOTIFY_NEW_VOICEMAIL,
  WMSAPP_NOTIFY_NEW_VOICEMAIL_LINE2,
  WMSAPP_NOTIFY_NEW_CDMA_BROADCAST_MESSAGE,
  WMSAPP_NOTIFY_NEW_GW_BROADCAST_MESSAGE,
  WMSAPP_NOTIFY_MEMORY_FULL,
  WMSAPP_NOTIFY_NEW_STATUS_REPORT_NOSTORE,
  WMSAPP_NOTIFY_NEW_STATUS_REPORT_STORE
}wmsapp_notify_e_type;


#ifdef FEATURE_CDSMS_BROADCAST
#define WMSAPP_MAX_LANGUAGES  8
typedef struct
{
  uint16                        tableSize;
  wms_bc_mm_service_info_s_type entries[WMS_BC_TABLE_MAX];
  wms_bc_config_e_type          config;
  wms_bc_pref_e_type            pref;

}wmsapp_bc_info_s_type;

typedef enum
{
  WMSAPP_BC_ALL_ACTIVE = 0,
  WMSAPP_BC_SOME_ACTIVE,
  WMSAPP_BC_NONE_ACTIVE
}wmsapp_bc_svc_status_e_type;

typedef struct
{
  wmsapp_bc_svc_status_e_type status;
  uint16 size;
  uint16 entryIndex[WMS_BC_TABLE_MAX/WMSAPP_MAX_LANGUAGES];
}wmsapp_bc_sorted_svcs_s_type;
#endif /* FEATURE_CDSMS_BROADCAST */

enum
{
  WMSAPP_STYLE_FORMAT_NONE        = 0x0000,
  WMSAPP_STYLE_FORMAT_BOLD        = 0x0001,
  WMSAPP_STYLE_FORMAT_ITALIC      = 0x0002,
  WMSAPP_STYLE_FORMAT_STRIKETHRU  = 0x0004,
  WMSAPP_STYLE_FORMAT_UNDERLINE   = 0x0008,
  WMSAPP_STYLE_FORMAT_SIZE        = 0x0010,
  WMSAPP_STYLE_FORMAT_LT_ALIGN    = 0x0020,
  WMSAPP_STYLE_FORMAT_RT_ALIGN    = 0x0040,
  WMSAPP_STYLE_FORMAT_CT_ALIGN    = 0x0080
};

/* Common to CDMA and GSM; Used for Segmenting purposes */
#define WMSAPP_MAX_USER_DATA_LENGTH 140

#define WMSAPP_MAX_SEGMENTS 255

typedef enum
{
  WMSAPP_NO_SEGMENT = 0,
  WMSAPP_USE_CONCAT_8 = 8,
  WMSAPP_USE_CONCAT_16 = 16
}wmsapp_segment_setting_e_type;

typedef enum
{
  WMSAPP_ENCODING_7_BIT,
  WMSAPP_ENCODING_8_BIT,
  WMSAPP_ENCODING_16_BIT
}wmsapp_encoding_e_type;


typedef struct
{
#ifdef FEATURE_SMS_UDH
  uint8                    num_headers;
  wms_udh_s_type         * headers; /* Memory has to be allocated */
#endif /* FEATURE_SMS_UDH */
  wmsapp_encoding_e_type   encoding;
  uint16                   num_chars; /* For 7-bit, each char occupies
                                         1 byte, Bit 7 is zero */

  uint8                  * data;    /* Memory has to be allocated */
}wmsapp_user_data_s_type;

typedef struct
{
  uint8                 num_digit;
  char                 *digit; /* voicemail digits in ASCII format */
}wmsapp_voicemail_number_s_type;

#define EVT_WMS_CFG_GW_READY            0x1000
#define EVT_WMS_CFG_CDMA_READY          0x1001
#define EVT_WMS_CFG_ROUTES              0x1002
#define EVT_WMS_CFG_MEMORY_STATUS       0x1003
#define EVT_WMS_CFG_MESSAGE_LIST        0x1004
#define EVT_WMS_CFG_MEMORY_FULL         0x1005
#define EVT_WMS_CFG_GW_DOMAIN_PREF      0x1006
#define EVT_WMS_CFG_CELL_CHANGE         0x1007
#define EVT_WMS_CFG_PRIMARY_CLIENT_SET  0x1008

#define EVT_WMS_MSG_SEND                0x2001
#define EVT_WMS_MSG_ACK                 0x2002
#define EVT_WMS_MSG_READ                0x2003
#define EVT_WMS_MSG_WRITE               0x2004
#define EVT_WMS_MSG_DELETE              0x2005
#define EVT_WMS_MSG_MODIFY_TAG          0x2006
#define EVT_WMS_MSG_READ_TEMPLATE       0x2007
#define EVT_WMS_MSG_WRITE_TEMPLATE      0x2008
#define EVT_WMS_MSG_DELETE_TEMPLATE     0x2009
#define EVT_WMS_MSG_RECEIVED_MESSAGE    0x200A
#define EVT_WMS_MSG_SUBMIT_REPORT       0x200B
#define EVT_WMS_MSG_STATUS_REPORT       0x200C
#define EVT_WMS_MSG_DELETE_ALL          0x200D
#define EVT_WMS_MSG_DELETE_TEMPLATE_ALL 0x200E
#define EVT_WMS_MSG_READ_STATUS_REPORT  0x200F
#define EVT_WMS_MSG_DELETE_STATUS_REPORT 0x2010
#define EVT_WMS_MSG_DUPLICATE_CB_PAGE    0x2011

#ifdef FEATURE_USES_MMS
#define EVT_MMS_MSG_SEND_FINSH                0x2012
#define EVT_MMS_MSG_GET_FINISH                0x2013
#endif

#define EVT_WMS_CMD_ERR                 0x3000

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
#define EVT_WMS_BC_MM_EVENT_CONFIG          0x4010
#define EVT_WMS_BC_MM_EVENT_PREF            0x4011
#define EVT_WMS_BC_MM_EVENT_TABLE           0x4012
#define EVT_WMS_BC_MM_EVENT_SRV_IDS         0x4013
#define EVT_WMS_BC_MM_EVENT_SRV_INFO        0x4014
#define EVT_WMS_BC_MM_EVENT_SRV_UPDATED     0x4015
#define EVT_WMS_BC_MM_EVENT_ADD_SRVS        0x4016
#define EVT_WMS_BC_MM_EVENT_DELETE_SRVS     0x4017
#define EVT_WMS_BC_MM_EVENT_SELECT_ALL_SRVS 0x4018
#define EVT_WMS_BC_MM_EVENT_DELETE_ALL_SRVS 0x4019
#define EVT_WMS_BC_MM_EVENT_SET_PRIORITY_ALL_SRVS 0x401A
#define EVT_WMS_BC_MM_EVENT_READING_PREF    0x401B
#define EVT_WMS_BC_MM_EVENT_ENABLE_FAILURE  0x401C
#define EVT_WMS_BC_MM_EVENT_DISABLE_FAILURE 0x401D
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */


#define EVT_WMS_DLG_RESUME              0x5000
#define EVT_WMS_CONFIRM                 0x5001
#define EVT_WMS_PROCESS_NEW_MSG         0x5002

#define EVT_WMS_DC_CONNECTED                0x6000
#define EVT_WMS_DC_ABORTED                  0x6001
#define EVT_WMS_DC_ENABLE_AUTO_DISCONNECT   0x6002
#define EVT_WMS_DC_DISABLE_AUTO_DISCONNECT  0x6003
#define EVT_WMS_DC_INCOMING                 0x6004
#define EVT_WMS_DC_DISCONNECTED             0x6005
#define EVT_WMS_DC_CONNECTING               0x6006

#define EVT_WMSAPP_MESSAGE_READ             0x7000
#define EVT_WMSAPP_MESSAGE_COMPOSE          0x7001

#define EVT_WMSAPP_CARDFILE_CPHS_MWI            0x7002 /* CPHS Message Waiting Indicator Status File */
#define EVT_WMSAPP_CARDFILE_CPHS_MAILBOX_NUM    0x7003 /* CPHS Mailbox Dailing Number File */
#define EVT_WMSAPP_CARDFILE_MWI                 0x7004 /* EF-Message Waiting Indication Status File */
#define EVT_WMSAPP_CARDFILE_MAILBOX_NUM         0x7005 /* EF-Mailbox Dailing Number File */
#define EVT_WMSAPP_CARDFILE_MAILBOX_ID          0x7006 /* EF-Mailbox Identifier File */


typedef struct _CWMSAPP
{
  //First element of this structure must be AEEApplet.
  AEEApplet                    a;
  wms_client_id_type           m_clientId;
  IWMS                        *m_pwms;
#ifdef FEATURE_ICM
  ICM                         *m_pICM;
#endif /* FEATURE_ICM */
#ifdef FEATURE_QVPHONE
  IVideoPhone                 *m_pIVideoPhone;
#endif /* FEATURE_QVPHONE */

  IImage                      *m_pSplashImage;
  boolean                      m_bSuspended;        /* Is the application currently suspended? */
  uint16                       m_cxScreen, m_cyScreen;    /* What is the screen size? */
  AEERect                      m_MenuFrame;       /* What is the frame size? */

  ISound                      *m_pISound;

  AEECallback                  m_callback;
#ifdef FEATURE_ANNUNCIATOR
  IAnnunciator                *m_pIAnn;
#endif /* FEATURE_ANNUNCIATOR */

#ifdef FEATURE_IWMSDIAG
  IWMSDIAG                    *m_pIWmsdiag;
#endif /* FEATURE_IWMSDIAG */

  wmsapp_mode_e_type           m_modeControl;    /* Mode control for display */
  // Message Related
  wms_routes_s_type            m_routes;
  WMSAPP_DisplayInfo           m_dispInfo;
  wmsapp_msg_type              m_msg_type;
  wms_msg_event_info_s_type   *m_pMsgEventInfo;

  wms_client_message_s_type    *m_pMO_template_msg;

  wms_client_ts_data_s_type    *m_pClient_ts_mo;
  wms_client_message_s_type    *m_pClient_msg_mo;
  wms_client_ts_data_s_type    *m_pClient_ts;
  wms_client_message_s_type    *m_pClient_msg;
  wms_gw_status_report_s_type  *m_pStatusRpt;
  wms_ack_info_s_type          m_ackInfo;
  AEEWMS_CmdCbData             m_cbData;

  wms_memory_status_s_type     *m_pMemStatuses[WMS_MEMORY_STORE_MAX];
  wms_memory_status_s_type     *m_pStatusReportMemStatus;
  wms_message_list_s_type      *m_pSmsLists[WMS_MEMORY_STORE_MAX+1];//1 extra for voicemail
  wms_message_list_s_type      *m_pSmsStatusRptList;
  wms_message_list_s_type      *m_pCDSMSTemplateList;
  wms_message_list_s_type      *m_pGWSMSTemplateList;

#ifdef FEATURE_SMS_UDH
  IImage                      *m_pCurImage[WMS_MAX_UD_HEADERS];
  IImage                      *m_pSelImage;
#endif /* FEATURE_SMS_UDH */
  IConfig                     *m_pIConfig;
#ifdef FEATURE_SMS_UDH
  uint8                        m_numofImages;
#endif /* FEATURE_SMS_UDH */
  AEEFont                      m_curFont;
  uint16                       m_curFlags;
  uint16                       m_addrType;
  wmsapp_email_address_s_type  m_emailAddr;


  WMSAPP_CurMsgListInfo        m_msgInfo;
  uint8                        m_CDSMSDefaultTemplate;
  uint8                        m_GWSMSDefaultTemplate;
  uint16                       m_currPref;

  /* Flag needed to properly detect if email address lies in the user data */
  boolean                      m_bIsNonStdEmail;
  uint8                        m_buf[WMS_MAX_LEN];
  uint8                        m_emailBuf[30];

#ifdef FEATURE_CDSMS_BROADCAST
  wmsapp_bc_info_s_type        m_bcInfo;
  int                          m_svcPriority;
  wmsapp_bc_sorted_svcs_s_type m_bcSortedSvcs[WMSAPP_MAX_LANGUAGES];
  uint8                        m_currLang;
  uint32                       m_bcCurrIndex;
  wms_bc_mm_service_info_s_type   m_bcNewSvc;
#endif /* FEATURE_CDSMS_BROADCAST */

  wmsapp_notify_e_type         m_notify;
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /*FEATURE_GWSMS_BROADCAST*/

  /* For Tab Controller in the Services Edit Dialog */
  DlgTabCtl                   *m_pTabController;

  wms_gw_domain_pref_e_type    m_GWDomainPref;

  /* Alert Settings */
  boolean                      m_bDisplayAlert;
  boolean                      m_bBeepAlert;
  uint8                        m_BeepVolume;

  wms_message_mode_e_type      m_mode;
  boolean                      m_bAlertOnly;
  boolean                      m_bDisplayNewMsgAlert;
  boolean                      m_bEditMsg;
  uint32                       m_styleMask;
  boolean                      m_bStyleFormat;
  wms_message_class_e_type     m_curClass;
  int                          m_melody_trans_id;
  wms_message_index_type       m_msgIndex;
  wms_alpha_id_s_type          m_AlphaID;
  boolean                      m_bReply;
  boolean                      m_bDisplaySplash;
  boolean                      m_bAutoDcSend;
  boolean                      m_bDCOrig;
  boolean                      m_bAutoDcDisconnect;
  wms_dc_so_e_type             m_AutoDcSendSO;
  boolean                      m_bCDMAReady;
  boolean                      m_bGWReady;
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /*FEATURE_GWSMS_BROADCAST*/

#ifdef FEATURE_GWSMS_STATUS_REPORTS
#error code not present
#endif /* FEATURE_GWSMS_STATUS_REPORTS */

  boolean                        m_bUsepUserData;
  boolean                        m_bStoreMsg;
  wmsapp_segment_setting_e_type  m_segmentSetting;
  wmsapp_user_data_s_type       *m_pUserData;

  boolean                        m_bComposeMessage;
  char                          *m_composeAddress;

  boolean                        m_bsegmentEO;

  uint32                         m_retryPeriod; /* in seconds */

  boolean                      m_bTestMode;

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

  uint8                          m_mailboxRecordNumLine1; /* the record number in EF-MBDN for voicemail */
  wmsapp_voicemail_number_s_type  *m_voicemailNumber;   /* voicemail mailbox number for line 2*/
  AEECardGenericCmdStatus        *m_pCardCmdVMWriteStatus;

  // Members for secondary display
  IDisplay 			*m_pIDisplay2;
  IStatic			*m_pStaticSecText; 
  boolean			m_bSecDisplayNewMsgAlert;
  wmsapp_display_state_e_type   m_secDisplayState;

  boolean m_bAccessChannelTest;
  wms_client_message_s_type *m_pAccessChannelMsg;
  int AccessChMsgInterval;
  int AccessChMsgTimePeriod;
  int AccessChMsgSendContinuouslyFor;
  int AccessChMsgNumberOfChars;
  int AccessChMsgNumberOfMsgs;


}CWMSAPP;

#endif/*FEATURE_WMS_APP*/
#endif /* WMSAPP_H */

