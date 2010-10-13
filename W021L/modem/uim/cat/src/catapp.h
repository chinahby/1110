#ifndef CATAPP_H
#define CATAPP_H
/*===========================================================================


            C A T    A P P   D E F I N I T I O N S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2006,2008 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/uim/cat/rel/07H1_2/src/catapp.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/29/08   js      CatApp automation changes
06/23/08   yb      Changed the feature name from FEATURE_APP_TEST_AUTOMATION
                   to FEATURE_CATAPP_TEST_AUTOMATION
3/13/08    yb      Fixing Non Adherence of coding standards for SCM guidelines
11/27/07   yb      Replaced the deprecated functions with new functions
                   Removed Lint error introduced by FEATURE_UIONE_HDK
11/21/07   sk      Added UIONE support for Send DTMF, Send SS, Send USSD alpha
11/20/07   sk      Added support for Language selection event
10/03/07   sk      Added support for setup idle text for uiOne
08/20/07   sk      Added flag for idle screen display
05/07/07   nk      Added battery support featurization
02/22/07   jas     Removing CoreApp.h and adding CoreApp.BID
                   Removed reference to IAnnuciator
10/23/06    sk     Added enum CatApp_EndProactiveSessionEnumType
9/19/06     nk     Moved #define for additional null char from catapp_idiag.h
8/23/06     nk     Added menu members for read display menu diag command
7/25/06     sk     Removed Ch none status.
6/26/06     nk     Added Macros
6/12/06     nk     Added Support for IDIAG Automation
06/09/06    sk     Added new nmember in CCatApp to support dcs for get input
05/31/06    sk     Fixed value for CATAPP_MAX_REVC_DATA_SIM_APDU_SIZE
05/17/06    sk     Made INetMgr and pdp profile num part of channel structure
04/04/06    sk     Added support for variable timeout and immediate digit
                   response for get inkey cmd.
04/04/06   sk      Added support for battery status
03/16/06   sk      Added flg for pending call status
12/05/05   sk      Added field m_USSD_is_from_card to
                   struct CCatapp to differentiate ussd sent
                   from the card Vs the ussd sent by the user.
08/26/05   sk      Added support for max buffer size
                   and added a flag to check if open channel
                   params are modified in the CatApp_OpenChInfo
08/15/05   tml     Fixed Unicode Item display
07/14/05   tml     Added CH Closed state
01/08/05  sk/tml   Added USSD response
12/04/04   tml     Added get inkey yes/no/single char rseponse type
11/23/04  sk/tml   Added SAT/USAT data support
11/05/04   tml     Fixed case sensitivity issue for include file
10/01/04   tml     Added help available for setup menu member
10/01/04   tml     Added header back
01/16/03   ram     Replaced cm functionality by ICM.
10/09/03   tml     Added EVT_CAT_END_PROACTIVE_CMD_SES_WITHOUT_TR
04/22/03   tml/AP  Added EVT_CAT_CM_CALL_EVENT_ERR.
02/25/03   tml     Added Send SS, Send USSD support
02/18/03   tml     Added EVT_CAT_SMS_MESSAGE_FAILED event
02/04/03   AT      Added support for SETUP_CALL.
10/17/02   aaa     Initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifdef FEATURE_APP_CATAPP
#include "catapp.bid"
#include "AEEComdef.h"
#include "AEEStdLib.h"        /* AEE Stb Lib Services */
#include "AEEFile.h"          /* AEE File Manager Services */
#include "AEEImageCtl.h"      /* AEE ImageCtl Services */
#include "AEESound.h"
#include "AEENet.h"
#include "AEEText.h"
#include "AEEMenu.h"
#include "AEE_OEM.h"
#include "AEESTK.h"
#include "uixsnd.h"
#include "AEEAppGen.h"
#include "comdef.h"

#include "wms.h"
#include "CoreApp.BID"

#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */

#define MAX_NUMBER_OF_ITEMS_IN_MAIN_MENU 30


#define CATAPP_BUFF_SIZE 50

#define CATAPP_MAX_GPRS_CH 7
#define CATAPP_MAX_REVC_DATA_SIM_APDU_SIZE 237 /* 255-18: Command Details: 5
                                                          Device: 2
                                                          Result: 3
                                                          Channel Data Len: 4
                                                          Data tag + len: 3
                                                          TR Tag + total len: 3 */

#define CATAPP_GPRS_MAX_BUFFER_SIZE 1400

#ifdef FEATURE_UIONE_HDK
#define CATAPP_URL_STANDARD_STRING_LEN 30
#endif

#define CATAPP_SHIFT_BY_ONE_BYTE 8

typedef enum UserSoftKeyInkey
{
  NO_INPUT,
  YES_INPUT
} UserSoftKeyInkey;

typedef enum
{
  CATAPP_CH_OPEN_WAIT,
  CATAPP_CH_OPENED,
  CATAPP_CH_CLOSED,
  CATAPP_CH_DROPPED
} CatApp_ChannelStatus;

typedef enum
{
  /* No Proactive End Session received */
  CATAPP_END_PROACTIVE_SESSION_NONE,

  /* Proactive End Session received from card */
  CATAPP_CARD_END_PROACTIVE_SESSION,

  /* Proactive End Session received, but not from card */
  CATAPP_NON_CARD_END_PROACTIVE_SESSION
} CatApp_EndProactiveSessionEnumType;

typedef struct
{
  boolean                   bPresent;
  AEESTKBearerGPRSParamType Bearer;
} CatApp_GPRSBearerType;


typedef struct
{
  AEESTKResult               Result;
  AEESTKAdditionalInfo       AdditionalInfo;
} CatApp_CommandRespType;

typedef struct
{
  ISocket                           *pISocket;
  INetMgr                           *pINetMgr;
  int32                              nIndex;
  int32                              nChID;
  byte                               nProfileNum;
  boolean                            bOnDemandLinkEst;
  CatApp_GPRSBearerType              sGPRSBearer;
  AECHAR                            *sAPN;
  INAddr                             sIPAddress; /* local address */
  INAddr                             sDataDestAddress;
  boolean                            bIsTCP;
  INPort                             dwPortNum;
  CatApp_ChannelStatus               status;
  int32                              nBufferSize;  /* for both RecvBuffer and SendBuffer */
  uint8                             *pRecvBuffer;  /* Malloc when Open Ch requested is received */
  int32                              nCurrRecvDataSize;
  boolean                            bMoreDataToRecv;
  boolean                            bWasEmpty;
  uint8                             *pSendBuffer;  /* Malloc based on each send request */
  int32                              nSendDataBufSize;
  int32                              nByteDataSent;
  boolean                            bOpen_ch_param_modified;
  int32                              nParentAddress;
} CatApp_OpenChInfo;


typedef struct CCatApp   CCatApp;

/* Cat app global structure. */
/* Note: m_bPlugin == TRUE indicates that CatApp tries to  */
/* play the file directly in CPlayerWin. Pressing CLR will close */
/* the app. */
struct CCatApp
{
   AEEApplet                  a;
   uint16                     m_cxWidth;
   uint16                     m_cyHeight;
   uint16                     m_nColorDepth;
   uint16                     m_nNChSize;    /* Large char size */
   uint16                     m_nLChSize;    /* Normal char size */
   ISTK                       *m_pISTK;
   boolean                    m_bSuspended;  /* Are we currently suspended? */
   boolean                    m_bExit;
#ifdef FEATURE_UIONE_HDK
   boolean                    m_bIdleScreenDisplayed;   /* Is the phone UI currently on Idle screen */
#endif /* FEATURE_UIONE_HDK */
   void                       *m_pProactiveCommand;
   AEESTKProactiveCmdHeader   cmdHdr;       /* Copy the command header here. */
   AECHAR                     call_cnf_str[AEECM_MAX_DIAL_STRING+1]; /* String for Calling. */
   uint8                      dtmf_digits[AEECM_MAX_DIAL_STRING+1]; /* String of dtmf digits. */
   wms_client_id_type         wms_id;         /* wms client id */
   AECHAR                     *setupmenutitle; /* menu title that came in with last */
                                              /* setup menu command */

   boolean                    setupmenutitlepresent; /* whether a title exists for the  */
                                                     /* last setupmenu command that came in */
                                                     /* menu items that came in last setup */
                                                     /* menu command */
   boolean                    setupmenuupdate;      /* whether there are updates for   */
                                                     /* catapp main menu */
   uint8                      default_setupmenu_item;
   AECHAR                     *StaticMenu[MAX_NUMBER_OF_ITEMS_IN_MAIN_MENU];
   uint32                     numItemsInMenu;        /* number of items that came in last */
                                                     /* setup menu command */
   uint32                     *menuitemid;            /* item ids of items that came in last
                                                        setup menu commd */
   boolean                    term_resp_sent;        /* whether the terminal response has */
                                                     /* been sent or not for the last */
                                                     /* proactive command that came in */
   AECHAR                     *userinput;             /* The input that the user entered if the  */
                                                     /* last proactive command required a user */
                                                     /* input */
   boolean                    play_tone_displayed;   /* whether play tone screen was displayed */
   uint16                     chosenitem;            /* item that the user can chose in a menu */
   boolean                    origination_sent;      /* Set to TRUE when the origination is sent
                                                        and no response is received yet. */
   wms_client_message_s_type  *m_psMessageToWms;
   AEECMCallID                cat_call_id;           /* The cat app's call id? */
   boolean                    m_bDisplayingText;
   uint8                      m_lang[AEESTK_LANG_CODE_LEN];
   uint8                      m_currLang[AEESTK_LANG_CODE_LEN];
   char                       *sDTMFDigit;
   uint8                      byNumDTMFDigits;
   uint8                      byCurrDTMF;
   ICM                        *m_pICM;
   ISUPPSVC                   *m_pISuppSvc;
   uint8                      nOpId;                /* The CatApp operation id for the SS/Call. */
   boolean                    m_bSKMainMenu;
   boolean                    m_icon_present;
   boolean                    m_bSetupMenuHelp;     /* The setup menu help is available or not */
   AEESTKInkeyResponseType    m_InkeyRspType;
#ifdef FEATURE_CAT_REL6
   AEESTKDuration             m_StartofExecTime;
#endif /* #ifdef FEATURE_CAT_REL6  */

   CatApp_OpenChInfo                   m_sOpenChInfo[CATAPP_MAX_GPRS_CH];
   AEECallback                         m_OpenChCb;
   AEECallback                         m_LangUserCb;
   AEESTKReceiveDataResponse           m_RecvDataRsp;
   int32                               m_CurrChId;
   AEESTKString                        m_USSDStringRsp;
   uint8                               m_USSDDcs;
   boolean                             m_USSD_is_from_card;
#if (defined FEATURE_CAT_REL6) && !(defined FEATURE_UIONE_HDK)
   AEESTKBatteryStatusType    m_BatteryStatusRsp;   /* Battery status */
#endif /* #if (defined FEATURE_CAT_REL6) && !(defined FEATURE_UIONE_HDK) */
   boolean                    m_PendingCallSetUp;   /* Call set up pending flg */
   AEESTKInputFormatType      m_GetInputRspFormat;  /* input response format from card */

   /* Indicates if Proactive End session was received */
   CatApp_EndProactiveSessionEnumType  m_EndProactiveSession;

#ifdef FEATURE_CATAPP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_CATAPP_TEST_AUTOMATION */
   ISUPPSVCOpts               *m_pISuppSvcOpts;


};



/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define EVT_CAT_DISPLAY_TEXT_REQ       (EVT_USER + 0x2000)
#define EVT_CAT_CLOSE_SKUI             (EVT_USER + 0x2001)
#define EVT_CAT_GET_INKEY_REQ          (EVT_USER + 0x2002)
#define EVT_CAT_GET_INPUT_REQ          (EVT_USER + 0x2003)
#define EVT_CAT_PLAY_TONE_REQ          (EVT_USER + 0x2004)
#define EVT_CAT_SELECT_ITEM_REQ        (EVT_USER + 0x2005)
#define EVT_CAT_SEND_SMS_REQ           (EVT_USER + 0x2006)
#define EVT_CAT_SEND_SS_REQ            (EVT_USER + 0x2007)
#define EVT_CAT_SEND_USSD_REQ          (EVT_USER + 0x2008)
#define EVT_CAT_SETUP_CALL_REQ         (EVT_USER + 0x2009)
#define EVT_CAT_SETUP_MENU_REQ         (EVT_USER + 0x200a)
#define EVT_CAT_SMS_MESSAGE_SENT       (EVT_USER + 0x200b)
#define EVT_CAT_SMS_MESSAGE_FAILED     (EVT_USER + 0x200c)
#define EVT_CATAPP_SMS_MESSAGE_SENT    (EVT_CAT_SMS_MESSAGE_SENT)
#define EVT_CATAPP_SMS_MESSAGE_FAILED  (EVT_CAT_SMS_MESSAGE_FAILED)
#define EVT_CAT_MORE_TIME_REQ          (EVT_USER + 0x200d)
#define EVT_CAT_UPDATE_IDLE_TEXT_REQ   (EVT_USER + 0x200e)
#define EVT_CAT_END_PROACTIVE_CMD_SES  (EVT_USER + 0x200f)
#define EVT_CAT_END_PROACTIVE_CMD_SES_WITHOUT_TR (EVT_USER + 0x2010)
#define EVT_CAT_LANG_NOTIFICATION_REQ  (EVT_USER + 0x2011)
#define EVT_CAT_SEND_DTMF_REQ          (EVT_USER + 0x2012)
#define EVT_CAT_PROVIDE_LANG_INFO_REQ  (EVT_USER + 0x2013)
#define EVT_CAT_USER_KEY_EVENT         (EVT_USER + 0x2014)

#define EVT_CATAPP_OPEN_CH_FAILED       (EVT_USER + 0x3030)
#define EVT_CATAPP_OPEN_CH              (EVT_USER + 0x3031)
#define EVT_CATAPP_OPEN_CH_SUCCESS      (EVT_USER + 0x3032)
#define EVT_CATAPP_SEND_DATA_FAILED     (EVT_USER + 0x3033)
#define EVT_CATAPP_SEND_DATA            (EVT_USER + 0x3034)
#define EVT_CATAPP_SEND_DATA_SUCCESS    (EVT_USER + 0x3035)
#define EVT_CATAPP_RECEIVE_DATA_FAILED  (EVT_USER + 0x3036)
#define EVT_CATAPP_RECEIVE_DATA         (EVT_USER + 0x3037)
#define EVT_CATAPP_RECEIVE_DATA_SUCCESS (EVT_USER + 0x3038)
#define EVT_CATAPP_GET_CH_STATUS_RESULT (EVT_USER + 0x3039)
#define EVT_CATAPP_CLOSE_CH_FAILED      (EVT_USER + 0x303A)
#define EVT_CATAPP_CLOSE_CH             (EVT_USER + 0x303B)
#define EVT_CATAPP_CLOSE_CH_SUCCESS     (EVT_USER + 0x303C)
#define EVT_CATAPP_DATA_AVAILABLE       (EVT_USER + 0x303D)
#define EVT_CATAPP_PROCESS_RECEIVE_DATA_CMD (EVT_USER + 0x303E)
#define EVT_CATAPP_CH_STATUS_CHANGED    (EVT_USER + 0x303F)

/* Event values used in calculation in DBGEVENT to get Brew Event IDs reserved
   For Cat App in evt_defs.h:
   EVENT_BREW_CATAPP_RECV_PROACTIVE_CMD: 0x661
   EVENT_BREW_CATAPP_TERMINAL_RSP: 0x662
   EVENT_BREW_CATAPP_NO_DISPLAY: 0x663
*/
#define EVT_CATAPP_RECV_PROACTIVE_CMD   (EVT_USER + 0x5)
#define EVT_CATAPP_SENT_TERMINAL_RSP    (EVT_USER + 0x6)
#define EVT_CATAPP_NO_DISPLAY           (EVT_USER + 0x7)

#define CATAPP_ADDITIONAL_NULL_CHAR      0x1
/* CATAPP MACRO DEFINITIONS */

/* ----------------------------------------------------------------------------
   MACRO:       CATAPP_MALLOC_AECHAR
   DESCRIPTION: Will malloc the value sent in times the size of AECHAR
   --------------------------------------------------------------------------*/

#define CATAPP_MALLOC_AECHAR(pMem, ptrSize)                   \
  (pMem) = MALLOC((ptrSize) * (sizeof(AECHAR)));              \

#define CATAPP_MALLOC_AECHAR_NULL_CHECK(pMem, ptrSize)         \
  (pMem) = MALLOC((ptrSize) * (sizeof(AECHAR)));               \
  if ((pMem) == NULL) {                                        \
    MSG_ERROR("Malloc fail", 0, 0, 0);                         \
  }


/* ----------------------------------------------------------------------------
   MACRO:       CATAPP_FREEIF
   DESCRIPTION: Will free the memory pointed to by pMem only if it is not NULL
   --------------------------------------------------------------------------*/
#define CATAPP_FREEIF(pMem)                   \
  FREEIF(pMem);                                \
  pMem  = NULL;

/* ----------------------------------------------------------------------------
   MACRO:       CATAPP_FREE
   DESCRIPTION: Will free the memory pointed to by pMem
   --------------------------------------------------------------------------*/
#define CATAPP_FREE(pMem)                   \
  FREE(pMem);                                \
  pMem  = NULL;

#endif /* FEATURE_APP_CATAPP */
#endif  //CATAPP_H

