/*
FILE:  OEMSTK.c

SERVICES:  OEM functions for supporting BREW ISTK Interface

GENERAL DESCRIPTION:
  This file provides the prorotypes of the functions that OEMs must
  provide in order to support the BREW ISTK interface.

PUBLIC CLASSES AND STATIC FUNCTIONS:

        Copyright © 1999-2009 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/istk/rel/07H1_2/src/OEMSTK.c#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/29/09   xz      1) Register LAUCH BROWSER cmd
                   2) Extend OEMSTK_GetNextCmd() to return command number
04/06/09   yb      Fixed response data for UCS2 DCS for Get Input command
03/26/09   sk      Fixed lint error
03/17/09   xz      Set TCP UICC Client Mode in TP
11/24/08   sk      Updated tp dl to indicate number of BIP channels supported
11/17/08   sk      Extended OEMSTK cache to handle end session command while
                   another proactive command is in progress
08/22/08   xz      Fix of LINT errors/warnings
05/02/08   sk      Fixed length of the output buffer in 
                   OEMSTK_GSTKStringGSMDefaultToAEEString().
04/01/08   tml     Fixed compilation warning
11/29/07   sk      Added support for differentiating SAT and USAT TP dl
11/20/07   sk      Support for language notification envelope
09/25/07   sk      Changed tp dl bytes to include alpha and display of extn text
12/22/06   sk      Fixed memory issue
10/20/06   sk      Deregistration with GSTK when ref cnt to ISTK object is zero
09/11/06   sk      Removed warning.
09/07/06   sk      Support for Send SS - CFU
08/22/06   tml     proper RPC support
08/21/06   sk      Fixed compilation issue.
08/17/06   sk      Added registration with gstk for battery status pli req.
06/09/06   sk      Fixed get input issue related to dcs.
06/01/06   sk      Added support for channel status and data available
                   registrations.
05/25/06   sk      Corrected typo error for data available
04/06/06   sk      Updated Terminal Profile values.
04/04/06   sk      Added support for variable timeout and immediate digit
                   response for get inkey cmd.
04/04/06   sk      Added support for battery status and
                   duration object in display txt cmd.
02/16/06   sk      Lint fixes, Fixed type error, Fixed notify flag 
                   for Browser termination event, Added support
                   for Langauge selection event, Fixed TP bits for
                   Launch browser and Langauge support
01/10/06   sk      Applied OEMSTK_ConvertFromGSMDefault
                   to 8 bit unpacked data from card according
                   to the spec.
11/16/05   sk      Corrected DCS UCS2 enum
11/03/05   sk      Added ucs2 fix for Send USSD
10/03/05   tml     Fixed USSD Extra param length issue
09/01/05   sun     Added registration of GSTK_REG_SETUP_EVT_IDLE_MENU_EVT 
08/30/05   tml     Fixed extra char and potential memory overwrite in setup 
                   menu and select item
08/25/05   tml     Fixed UCS2 item text decoding issue
08/25/05   sk      Fixed no of bytes in MEMCPY for ucs2 string
                   in OEMSTK_DCSSpc_AEEStringToGSTKString().
                   Changed the length field from WSTRLEN()
                   due to mem_malloc issues in
                   OEMSTK_AEEExtraParmToGSTKExtraParm().
                   Added check with flag for call control before
                   copying call control params.
08/17/05   sk      Fixed no of bytes in MEMCPY in 
                   OEMSTK_DCSSpc_AEEStringToGSTKString().
                   Also changed corresponding memory allocation 
                   in OEMSTK_AEEExtraParmToGSTKExtraParm().
07/22/05   sk      Added Idle Screen event support
05/26/05   tml     Fixed memory alignment issue
05/24/05   sk      Linuxify fixes
05/11/05   sst     Lint fixes
05/06/05   sk      Send Channel Data bug fixes.
04/05/05   tml     Limited to single proactive command processing, memory
                   alignment issue
03/30/05   sk      Added Unicode support. Added support for
                   two new functions
                   OEMSTK_DCSSpc_GSTKStringToAEEString(),
                   OEMSTK_DCSSpc_AEEStringToGSTKString().
03/21/05   tml     Added default item support for setup menu
03/03/05   sk      Added support for Browser Termination evt
                   download and checks for NULL pointers
01/12/05   tml     Fixed compilation issue with 1X only target
01/10/05   sk/tml  Channel related cmds and Send USSD fixes
01/07/05   sun     Fixed Compilation Warnings
12/13/04   sk      Fixed mapping of information in case
                   AEESTK_PROVIDE_LANG_INFO_REQ under function
                   OEMSTK_AEEExtraParmToGSTKExtraParm().
12/08/04   sk      Removed nChId from AEESTKGetChStatusReq.
                   Changed case AEESTK_GET_CH_STATUS_REQ under
                   OEMSTK_AEEExtraParmToGSTKExtraParm() 
                   to include status of all the open and dropped 
                   channels.
12/04/04  sk/tml   Get inkey yes/no and 7 bit decode for text fixes
11/23/04  sk/tml   SAT/USAT data support
10/06/04   tml     GSM default alphabet display
10/01/04   tml     Enhancement to BREW convention
09/22/04   tml     Potential error fatal fixes
09/21/04   ss      ISTK cleanup
09/17/04   ss      Word alignment fix
09/16/04   tml     Fixed compilation error for BREW 3.1
08/30/04   rs      Initial Version

=============================================================================*/


/*===========================================================================

                             INCLUDE FILES

===========================================================================*/
#include "intconv.h"
#include "comdef.h"     // Get DMSS type declarations.
#include "err.h"        // Error functions

#include "OEMHeap.h"
#include "OEMFeatures.h"
#ifdef FEATURE_BREW_3_0
#include "AEE_OEMDispatch.h"
#include "OEMCriticalSection.h"
#endif
#include "AEE.h"         // BREW headers.
#include "AEE_OEM.h"     // Brew OEM Headers.

#include "AEEStdLib.h"

#include "AEESTK.h"      // Header file for STK interface
#include "OEMSTK.h"
#include "gstk_exp.h"


#define OEMSTK_MAX_CACHE_ENTRIES              0x06

/* Number of concurrent proactive commands handled by ISTK:
** Entry 1 corresponds to "End Session proactive command handling while any other
** proactive command is in progress */
/* Entry 2 corresponds to any proactive command that the Toolkit app handles 
** Note: Entry 1 and Entry 2 can be in any order
*/
#define OEMSTK_MAX_NUM_OF_CONCURRENT_PRO_CMDS 0x02
#define OEMSTK_MAX_BT_CAUSE                   0x02


#ifndef FEATURE_BREW_3_0
#define AEE_ENTER_SYSTEM_CONTEXT()  AEE_SetAppContext(NULL)
#define AEE_LEAVE_SYSTEM_CONTEXT(p) (void)AEE_SetAppContext(p)
#else
#define AEE_ENTER_SYSTEM_CONTEXT()  AEE_EnterAppContext(NULL)
#define AEE_LEAVE_SYSTEM_CONTEXT(p) AEE_LeaveAppContext(p)
#endif

/* This structure maps the AEESTK Browser Termination Cause value
   to the GSTK Browser Termination cause value.
*/
typedef struct
{
  AEESTKBrowserTermEnumType         AEESTK_cause;
  gstk_browser_term_cause_enum_type gstk_cause;
}OEMSTK_BrowserTermCauseMapTable;

static OEMSTK_BrowserTermCauseMapTable OEMSTK_BT_Cause_Map_Table[OEMSTK_MAX_BT_CAUSE] =
{
  {AEESTK_BROWSER_TERM_USER_TERMINATED,  GSTK_BROWSER_USER_TERMINATED},
  {AEESTK_BROWSER_TERM_ERROR_TERMINATED, GSTK_BROWSER_ERROR_TERMINATED}
};

/* The table index location should not be changed since
   the access to the table is index base */
static uint16 gsmdefault_to_oemstk_table[] =
{
  0x40, 0xA3, 0x24, 0xA5, 0xE8, 0xE9, 0xF9, 0xEC, 0xF2, 0xC7, 0x20, 0xD8, 0xF8, 0x20, 0xC5, 0xE5,
  0x394,0x5F, 0x3A6,0x393,0x39B,0x3A9,0x3A0,0x3A8,0x3A3,0x398,0x39E,0x20, 0xC6, 0xE6, 0xDF, 0xC9,
  0x20, 0x21, 0x22, 0x23, 0xA4, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
  0xA1, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0xC4, 0xD6, 0xD1, 0xDC, 0xA7,
  0xBF, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0xE4, 0xF6, 0xF1, 0xFC, 0xE0
};

typedef enum
{
  OEMSTK_CACHE_STATE_FREE,        // No entry at this index
  OEMSTK_CACHE_STATE_NEW,         // New entry
  OEMSTK_CACHE_STATE_DISPATCHED   // Cache entry given to ISTKNotifier
} OEMSTKCacheState;


typedef struct
{
  OEMSTKCacheState        state;
  uint32                  dwSize;       // Size of the cmd buffer. This is what app needs to allocate.
  uint32                  commandType;  //This is the mask - Proactive / Envelope
  AEESTKProactiveCmdType  commandID;    //This is the exact ID of request
  uint32                  cmd_detail_reference;
  uint32                  user_data;
  uint8                   command_number;        // This is the command number given by GSTK
  uint16                  wTransactionID; // Upper 16 bits of transaction id used by OEMCAT to 
                                          // uniqely identify an command.
  void                    *pCommand;   // Pointer to the command data.
} OEMSTKCache;
/* The following structure is for the object which is allocated. */



struct OEMSTK
{
  IShell                    *m_pIShell;
  uint32                    m_uRefs;           
  OEMSTKCache               cmdCache[OEMSTK_MAX_CACHE_ENTRIES];
  AEECallback               *pcbOnCardCmd;
  uint16                    wTransactionCounter;  
  AEECallback               *m_pcbExit;
  boolean                   bUserActionNotif;
  boolean                   bDataAvailNotif;
  boolean                   bChStatusNotif;
  boolean                   bBrowserTermNotif;
  boolean                   bIdleScreenNotif;
  boolean                   bLangSelectionNotif;
};

static OEMSTK *gpSTK;
/************************************************************************
 ** I N T E R F A C E   F U N C T I O N   D E C L A R A T I O N S
 ************************************************************************/

static uint32 OEMSTK_AddRef(OEMSTK *pMe);

static gstk_client_id_type cl_id;


static int  OEMSTK_Init(
              OEMSTK           *po);

void OEMSTK_CleanupCache(int32 index);

#ifdef FEATURE_BREW_3_0
static OEMCriticalSection gSTKCriticalSection;
/*lint -emacro(545, OEMSTK_MUTEX_LOCK) */
#define OEMSTK_MUTEX_LOCK()     OEMCriticalSection_Enter(&gSTKCriticalSection)
/*lint -emacro(545, OEMSTK_MUTEX_UNLOCK) */
#define OEMSTK_MUTEX_UNLOCK()   OEMCriticalSection_Leave(&gSTKCriticalSection)
#else
#define OEMSTK_MUTEX_LOCK()     OEMOS_MutexStart()
#define OEMSTK_MUTEX_UNLOCK()   OEMOS_MutexStop()
#endif
static gstk_cmd_enum_type OEMSTK_GetConfirmationType(AEESTKProactiveCmdType proactive_cmd);
void OEMSTK_StringConversion(AEESTKString *data, uint8 dcs, gstk_string_type *str_data);

/* If any item is specific to a particular app type (sim/usim/csim),
   the corresponding app type must be specified
   If the item is applicable to all app types, GSTK_APP_NONE
   should be used 
*/
static gstk_profile_dl_support_ext_type value_list[] = 
{
  {1, GSTK_MENU_SEL_SUPPORT,                     GSTK_APP_NONE}, 
  {1, GSTK_USSD_DO_IN_CC_SUPPORT,                GSTK_APP_SIM},
  {1, GSTK_CC_SUPPORT,                           GSTK_APP_SIM} ,
  {1, GSTK_MO_SMS_CONTROL_SUPPORT,               GSTK_APP_NONE}, 
  {1, GSTK_SUPPORT_FOR_ALPHA_IN_CC,              GSTK_APP_SIM},
  {0, GSTK_UCS2_ENTRY_SUPPORT,                   GSTK_APP_NONE}, 
  {0, GSTK_UCS2_DISPLAY_SUPPORT,                 GSTK_APP_NONE},   
  {1, GSTK_DISPLAY_OF_EXTN_TEXT,                 GSTK_APP_SIM},
  {1, GSTK_USAT_DISPLAY_SUPPORT_BYTE_2_BIT_8,    GSTK_APP_USIM},
  {1, GSTK_DISPLAY_TEXT_SUPPORT,                 GSTK_APP_NONE}, 

  {1, GSTK_GET_INKEY_SUPPORT,                    GSTK_APP_NONE}, 
  {1, GSTK_GET_INPUT_SUPPORT,                    GSTK_APP_NONE}, 
  {1, GSTK_PLAY_TONE_SUPPORT,                    GSTK_APP_NONE}, 
  {1, GSTK_SELECT_ITEM_SUPPORT,                  GSTK_APP_NONE},
  {1, GSTK_SEND_SHORT_MSG_SUPPORT,               GSTK_APP_NONE}, 
  {1, GSTK_SEND_SS_SUPPORT,                      GSTK_APP_NONE}, 
  {1, GSTK_SEND_USSD_SUPPORT,                    GSTK_APP_NONE}, 
  {1, GSTK_SET_UP_CALL_SUPPORT,                  GSTK_APP_NONE}, 
  {1, GSTK_SET_UP_MENU_SUPPORT,                  GSTK_APP_NONE},
  {1, GSTK_USER_ACT_EVT_SUPPORT,                 GSTK_APP_NONE}, 

  {1, GSTK_IDLE_SCRN_AVAIL_EVT_SUPPORT,          GSTK_APP_NONE}, 
  {1, GSTK_LANG_SEL_EVT_SUPPORT,                 GSTK_APP_NONE}, 
  {1, GSTK_BROWSER_TERM_EVT_SUPPORT,             GSTK_APP_NONE}, 
  {1, GSTK_SET_UP_IDLE_MODE_TEXT_SUPPORT,        GSTK_APP_NONE}, 
  {1, GSTK_USAT_SETUP_CALL_SUPPORT_BYTE_8_BIT_7, GSTK_APP_USIM},
  {1, GSTK_SECOND_ALPHA_IN_SETUP_CALL_SUPPORT,   GSTK_APP_SIM},
  {1, GSTK_SECOND_CAP_CONFIG_PARAM_SUPPORT,      GSTK_APP_SIM},
  {1, GSTK_USAT_DISP_TEXT_SUPPORT_BYTE_9_BIT_1,  GSTK_APP_USIM},
  {1, GSTK_SUSTAINED_DISP_TEXT_SUPPORT,          GSTK_APP_SIM},
  {1, GSTK_LANGUAGE_NOTIFICATION_SUPPORT,        GSTK_APP_NONE}, 

  {1, GSTK_LAUNCH_BROWSER_SUPPORT,               GSTK_APP_NONE}, 
  {1, GSTK_SK_SUPPORT_IN_SEL_ITEM,               GSTK_APP_NONE}, 
  {1, GSTK_SK_SUPPORT_IN_SET_UP_MENU,            GSTK_APP_NONE}, 
  {7, GSTK_MAX_SK_SUPPORT,                       GSTK_APP_NONE}, 
  {0, GSTK_CSD_SUPPORT,                          GSTK_APP_NONE}, 
  {1, GSTK_GPRS_SUPPORT,                         GSTK_APP_NONE}, 
  {8, GSTK_NUM_CHAR_SUPPORT_DOWN_ME,             GSTK_APP_NONE}, 
  {0, GSTK_SCREEN_SIZE_SUPPORT,                  GSTK_APP_NONE}, 
  {12,GSTK_NUM_CHAR_SUPPORT_ACROSS_ME,           GSTK_APP_NONE}, 
  {0, GSTK_VARIABLE_FONT_SIZE_SUPPORT,           GSTK_APP_NONE}, 
  
  {0, GSTK_DISPLAY_RESIZED_SUPPORT,              GSTK_APP_NONE}, 
  {1, GSTK_TEXT_WRAPPING_SUPPORT,                GSTK_APP_NONE}, 
  {1, GSTK_TEXT_SCROLLING_SUPPORT,               GSTK_APP_NONE}, 
  {0, GSTK_WIDTH_REDUCE_IN_MENU_SUPPORT,         GSTK_APP_NONE}, 
  {1, GSTK_TCP_SUPPORT,                          GSTK_APP_NONE}, 
  {1, GSTK_UDP_SUPPORT,                          GSTK_APP_NONE}, 
  {1, GSTK_PROVIDE_LOCAL_INFO_LANG_SUPPORT,      GSTK_APP_NONE}, 
  {1, GSTK_CELL_BROADCAST_DATA_DL_SUPPORT,       GSTK_APP_NONE}, 
  {1, GSTK_REFRESH_SUPPORT,                      GSTK_APP_NONE},
  {0, GSTK_CARD_READER_ST_EVT_SUPPORT,           GSTK_APP_NONE}, 

  {1, GSTK_DATA_AVAIL_EVT_SUPPORT,               GSTK_APP_NONE}, 
  {1, GSTK_CH_STATUS_EVT_SUPPORT,                GSTK_APP_NONE}, 
  {0, GSTK_POWER_ON_CARD_SUPPORT,                GSTK_APP_NONE}, 
  {0, GSTK_PERFORM_CARD_APDU_SUPPORT,            GSTK_APP_NONE}, 
  {0, GSTK_GET_READER_ST_SUPPORT,                GSTK_APP_NONE}, 
  {0, GSTK_GET_READER_ST_CARD_ID_SUPPORT,        GSTK_APP_NONE}, 
  {0, GSTK_RUN_AT_CMD_SUPPORT,                   GSTK_APP_NONE}, 
  {1, GSTK_SEND_DTMF_SUPPORT,                    GSTK_APP_NONE}, 
  {1, GSTK_OPEN_CH_SUPPORT,                      GSTK_APP_NONE},
  {1, GSTK_CLOSE_CH_SUPPORT,                     GSTK_APP_NONE}, 

  {1, GSTK_RECEIVE_DATA_SUPPORT,                 GSTK_APP_NONE}, 
  {1, GSTK_SEND_DATA_SUPPORT,                    GSTK_APP_NONE}, 
  {1, GSTK_GET_CH_STATUS_SUPPORT,                GSTK_APP_NONE}, 
  {7, GSTK_NUM_CH_SUPPORT,                       GSTK_APP_NONE}, 
  {1, GSTK_CDMA_SEND_SMS_SUPPORT,                GSTK_APP_NONE}, 
  {1, GSTK_CDMA_SMS_PP_SUPPORT,                  GSTK_APP_NONE}, 
  {14,GSTK_PROTOCOL_VERSION,                     GSTK_APP_NONE}, 
  
#ifdef FEATURE_CAT_REL6
  {1, GSTK_DISPLAY_TXT_VARIABLE_TIMEOUT_SUPPORT,                    GSTK_APP_NONE},
  {1, GSTK_INKEY_VARIABLE_TIMEOUT_SUPPORT,                          GSTK_APP_NONE},
  {1, GSTK_INKEY_HELP_FOR_VARIABLE_TIMEOUT_AND_IMMEDIATE_DIGIT_RSP, GSTK_APP_NONE},
  {1, GSTK_PROVIDE_LOCAL_INFO_BATTERY_STATE_SUPPORT,                GSTK_APP_NONE},
#endif /* #ifdef FEATURE_CAT_REL6 */
};

static int32 OEMSTK_GSTKItemGSMDefaultToAEEString(gstk_item_object_type *pIn, 
                                               AEESTKString *pOut);



/************************************************************************
 ** I N T E R F A C E   F U N C T I O N   D E F I N I T I O N S
 ************************************************************************/

static void OEMSTK_AEEExitNotify(OEMSTK * po)
{
  int i = 0;
  MSG_HIGH("Exit OEMSTK in AEEExitNotify, obj: 0x%x", 
           po, 0, 0);
  if (po != NULL) {
    if (po->m_pcbExit) {
      sys_free(po->m_pcbExit);
      po->m_pcbExit = NULL;
    }
    for(i = 0; i < OEMSTK_MAX_CACHE_ENTRIES; i++)
    {
      if(po->cmdCache[i].state != OEMSTK_CACHE_STATE_FREE)
      {
        FREEIF(po->cmdCache[i].pCommand);
        po->cmdCache[i].state = OEMSTK_CACHE_STATE_FREE;
      }
    }
    if (po->m_pIShell){ 
      (void)ISHELL_Release(po->m_pIShell);
      po->m_pIShell = NULL;
    }
    (void)aee_forcereleaseobj((void **)&po);
    gpSTK = NULL;
  }
}

/*===========================================================================

FUNCTION: OEMSTK_New

DESCRIPTION
  Allocates a new object from the heap and stores the IShell pointer
  used to generate the events.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
===========================================================================*/
int OEMSTK_New(IShell *pIShell, OEMSTK **ppif)
{

  void *pac = NULL;
#ifdef AEE_SIMULATOR 
  // Do not create an object is this is being run on the SDK.
  *ppif = NULL;
  return EUNSUPPORTED;
#endif

  // Check parameters.
  if ((!pIShell) || (!ppif)) {
    return EBADPARM;
  }

  if (gpSTK == NULL)
  {
    // Allocate the object.
    if ((gpSTK = (OEMSTK*)sys_malloc(sizeof(OEMSTK))) == NULL) {
      return ENOMEMORY;
    }

    /* register for AEE Exit event */
    gpSTK->m_pcbExit = sys_malloc(sizeof(AEECallback));
    if (!gpSTK->m_pcbExit)
    {
      *ppif = NULL;
      sys_free(gpSTK);
      gpSTK = NULL; // JM: In case of next attempt
      return ENOMEMORY;
    }

    pac = AEE_ENTER_SYSTEM_CONTEXT();
    CALLBACK_Init(gpSTK->m_pcbExit, (PFNNOTIFY)OEMSTK_AEEExitNotify,
                    gpSTK);
#ifndef FEATURE_BREW_3_0
    AEE_RegisterSystemCallback(gpSTK->m_pcbExit, AEE_SCB_AEE_EXIT,
                                AEE_RESUME_CB_SYS,0);
#else
    ISHELL_OnExit(pIShell, gpSTK->m_pcbExit);
#endif

    AEE_LEAVE_SYSTEM_CONTEXT(pac);

    gpSTK->m_pIShell = pIShell;
    (void)ISHELL_AddRef(pIShell);
    gpSTK->m_uRefs = 1;
    gpSTK->pcbOnCardCmd = NULL;
    gpSTK->wTransactionCounter = 0;
    (void)OEMSTK_Init(gpSTK);
  }
  else
  {
    (void)OEMSTK_AddRef(gpSTK);
  }

  *ppif = gpSTK;

  return AEE_SUCCESS;

}
/*===========================================================================

FUNCTION: OEMSTK_AddRef

DESCRIPTION
  Indicates that one more object is now pointing to the OEMSTK object.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
===========================================================================*/
static uint32 OEMSTK_AddRef(OEMSTK *pMe)
{

   if (pMe) {
      return (++pMe->m_uRefs);
   }
   return 0;
}

/*===========================================================================

FUNCTION: OEMSTK_Release

DESCRIPTION
  Decreases the number of references to the ISTK object.  If the
  last reference is removed, it deallocates the object.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
===========================================================================*/
uint32 OEMSTK_Release(OEMSTK *pMe)
{
  int  i = 0;

  if (!pMe) {
    return (0);
  }

  if (pMe->m_uRefs > 0) {
    if (--pMe->m_uRefs != 0) {
      return pMe->m_uRefs;
    }
  }

  for(i = 0; i < OEMSTK_MAX_CACHE_ENTRIES; i++)
  {
    if(pMe->cmdCache[i].state != OEMSTK_CACHE_STATE_FREE)
    {
      FREEIF(pMe->cmdCache[i].pCommand);
      pMe->cmdCache[i].state = OEMSTK_CACHE_STATE_FREE;
    }
  }

  if (pMe->m_pIShell){ 
    (void)ISHELL_Release(pMe->m_pIShell);
    pMe->m_pIShell = NULL;
  }

  MSG_HIGH("OEMSTK_Release Cleanup: OEMSTK = 0x%x, exit_cb = 0x%x", 
             pMe, pMe->m_pcbExit, 0);
  if (pMe->m_pcbExit) {
    CALLBACK_Cancel(pMe->m_pcbExit);
    sys_free(pMe->m_pcbExit);
    pMe->m_pcbExit = NULL;
  }
  /* Ref count is zero at this point - Deregister with gstk */
  if (gstk_client_reg_ext(cl_id,(uint32)pMe,NULL,0)  != GSTK_SUCCESS)
  {
    ERR_FATAL("ISTK client deregistration with GSTK unsuccessful ", 0,0,0);
  } 
  else
  {
    MSG_HIGH("ISTK deregistration with GSTK successful, cl_id is %d",cl_id,0,0);
  }
  sys_free(pMe);
  gpSTK = NULL;
  return (0);
}


static int OEMSTK_GetGSTKStringLen(gstk_string_type *gstk_string)
{
  int                nAdditionalInt = 0;
  int                nAdditionalMod = 0;

  if(!gstk_string)
  {
      MSG_ERROR("gstk_string ptr is NULL",0,0,0);
      return 0;
  }
  /* Return the length + null terminated byte + word alignment */
  if ((gstk_string->length > 0) && (gstk_string->text != NULL)) {
    switch (gstk_util_decode_dcs_type(FALSE, gstk_string->dcs))
    {
    case GSTK_DCS_8_BIT:
    case GSTK_DCS_UCS2_8_BIT:
    case GSTK_DCS_UCS2_7_BIT:
      return gstk_string->length + 4;
    
    case GSTK_DCS_7_BIT:
      nAdditionalInt = gstk_string->length / 7;
      nAdditionalMod = gstk_string->length % 7;
      if (nAdditionalMod > 0) {
        nAdditionalMod = 1;
      }
      return gstk_string->length + nAdditionalMod + nAdditionalInt + 4;

    default:
      return 0;
    }
    
  }
  else {
    /* zero length text */
    MSG_HIGH("gstk_string_type is zero len", 0, 0, 0);
    return 0;
  }
} /* OEMSTK_GetGSTKStringLen */


// 1. Compute the size of the buffer required.
// 2. Allocate the buffer.
// 3. Set the internal pointer if possible.
// 4. Return the size.
static int OEMSTK_AllocateProactiveCmdBuffer(gstk_cmd_from_card_type *old_buf, 
                                             void **ppOutBuf)
{
  
  uint32 nSize; 
  uint32 nStructSize;
  uint8 *ptr;
  int i;
  int nStringSize = 0;

  if(!ppOutBuf)
  {
    MSG_ERROR("ppOutBuf is NULL",0,0,0);
    return EFAILED;
  }
  if(!old_buf)
  {
    MSG_ERROR("old_buf is NULL",0,0,0);
    return EFAILED;
  }

  switch(old_buf->hdr_cmd.command_id)
  {
  case GSTK_DISPLAY_TEXT_REQ:
    nSize = sizeof(AEESTKDisplayTextReq)+3;
    nStructSize = nSize-3;

    /* Calculate the string size */
    nStringSize = OEMSTK_GetGSTKStringLen(&old_buf->cmd.display_text_pro_cmd_req.text_string);
    nSize += (sizeof(AECHAR)*int32touint32(nStringSize));
    
    /* Calculate the icon size */
    if((old_buf->cmd.display_text_pro_cmd_req.icon.present) &&
        (old_buf->cmd.display_text_pro_cmd_req.icon.data != NULL)) 
    {
       nSize += old_buf->cmd.display_text_pro_cmd_req.icon.size+3;
    }

    //Allocate the buffer
    *ppOutBuf = (AEESTKDisplayTextReq*) MALLOC(nSize);
    if (*ppOutBuf == NULL) {
      return ENOMEMORY;
    }
    (void)MEMSET(*ppOutBuf, 0x00, nSize);

    ptr = (uint8*) *ppOutBuf;
    ptr += ((nStructSize+3)/4)*4;
    
    //Set the internal pointers
    //String
    if((old_buf->cmd.display_text_pro_cmd_req.text_string.length > 0) &&
        (old_buf->cmd.display_text_pro_cmd_req.text_string.text != NULL)) 
    {
      ((AEESTKDisplayTextReq*)(*ppOutBuf))->textString.pwszText = (AECHAR*) ptr;
      nStringSize = OEMSTK_GetGSTKStringLen(&old_buf->cmd.display_text_pro_cmd_req.text_string);
      ptr += sizeof(AECHAR)*(int32touint32(nStringSize));
    }
    //Icon
    if((old_buf->cmd.display_text_pro_cmd_req.icon.present) &&
        (old_buf->cmd.display_text_pro_cmd_req.icon.data != NULL)) 
    {
      ((AEESTKDisplayTextReq*)(*ppOutBuf))->icon.pData = ptr;
      ptr += ((old_buf->cmd.display_text_pro_cmd_req.icon.size+3)/4)*4;
    }
    break;


  case GSTK_GET_INKEY_REQ:
     
    nSize = sizeof(AEESTKGetInkeyReq)+3;
    nStructSize = nSize-3;
    nStringSize = OEMSTK_GetGSTKStringLen(&old_buf->cmd.get_inkey_pro_cmd_req.text_string);
    nSize += (sizeof(AECHAR)*int32touint32(nStringSize));
  
    if((old_buf->cmd.get_inkey_pro_cmd_req.icon.present) &&
      (old_buf->cmd.get_inkey_pro_cmd_req.icon.data != NULL))
    {
      nSize += old_buf->cmd.get_inkey_pro_cmd_req.icon.size+3;
    }

    *ppOutBuf = (AEESTKGetInkeyReq*) MALLOC(nSize);
    if (*ppOutBuf == NULL) {
      return ENOMEMORY;
    }
    (void)MEMSET(*ppOutBuf, 0x00, nSize);

    ptr = (uint8*) *ppOutBuf;
    ptr += ((nStructSize+3)/4)*4;

    if((old_buf->cmd.get_inkey_pro_cmd_req.text_string.length > 0) &&
      (old_buf->cmd.get_inkey_pro_cmd_req.text_string.text != NULL)) 
    {
      ((AEESTKGetInkeyReq*)(*ppOutBuf))->textString.pwszText = (AECHAR*) ptr;
      nStringSize = OEMSTK_GetGSTKStringLen(&old_buf->cmd.get_inkey_pro_cmd_req.text_string);
      ptr += sizeof(AECHAR)*(int32touint32(nStringSize));
    }

    if((old_buf->cmd.get_inkey_pro_cmd_req.icon.present) &&
      (old_buf->cmd.get_inkey_pro_cmd_req.icon.data != NULL)) 
    {
      ((AEESTKGetInkeyReq*)(*ppOutBuf))->icon.pData = ptr;
      ptr += ((old_buf->cmd.display_text_pro_cmd_req.icon.size+3)/4)*4;
    }
    break;

  case GSTK_GET_INPUT_REQ:
    nSize = sizeof(AEESTKGetInputReq)+3;
    nStructSize = nSize-3;
    /* allocate and copy text */
    nStringSize = OEMSTK_GetGSTKStringLen(&old_buf->cmd.get_input_pro_cmd_req.text_string);
    nSize += (sizeof(AECHAR)*int32touint32(nStringSize));
    
    if((old_buf->cmd.get_input_pro_cmd_req.icon.present) &&
      (old_buf->cmd.get_input_pro_cmd_req.icon.data != NULL)) 
    {
      nSize += old_buf->cmd.get_input_pro_cmd_req.icon.size+3;
    }
    nStringSize = OEMSTK_GetGSTKStringLen(&old_buf->cmd.get_input_pro_cmd_req.default_text);
    nSize += (sizeof(AECHAR)*int32touint32(nStringSize));
    
    *ppOutBuf = (AEESTKGetInputReq*) MALLOC(nSize);
    if (*ppOutBuf == NULL) {
      return ENOMEMORY;
    }
    (void)MEMSET(*ppOutBuf, 0x00, nSize);

    ptr = (uint8*) *ppOutBuf;
    ptr += ((nStructSize+3)/4)*4;

    if((old_buf->cmd.get_input_pro_cmd_req.text_string.length > 0) &&
      (old_buf->cmd.get_input_pro_cmd_req.text_string.text != NULL)) 
    {
      ((AEESTKGetInputReq*)(*ppOutBuf))->textString.pwszText = (AECHAR*) ptr;
      nStringSize = OEMSTK_GetGSTKStringLen(&old_buf->cmd.get_input_pro_cmd_req.text_string);
      ptr += sizeof(AECHAR)*(int32touint32(nStringSize));
    }

    /* allocate and copy icon */
    if((old_buf->cmd.get_input_pro_cmd_req.icon.present) &&
      (old_buf->cmd.get_input_pro_cmd_req.icon.data != NULL)) 
    {
      ((AEESTKGetInputReq*)(*ppOutBuf))->icon.pData = ptr;
      ptr += ((old_buf->cmd.get_input_pro_cmd_req.icon.size+3)/4)*4;
    }

    /* allocate and copy default text */
    if((old_buf->cmd.get_input_pro_cmd_req.default_text.length > 0) &&
      (old_buf->cmd.get_input_pro_cmd_req.default_text.text != NULL)) 
    {
      ((AEESTKGetInputReq*)(*ppOutBuf))->defaultText.pwszText = (AECHAR*) ptr;
      nStringSize = OEMSTK_GetGSTKStringLen(&old_buf->cmd.get_input_pro_cmd_req.default_text);
      ptr += sizeof(AECHAR)*(int32touint32(nStringSize));
    }
    break;

   case GSTK_LAUNCH_BROWSER_REQ:
    nSize = sizeof(AEESTKLaunchBrowserReq)+3;
    nStructSize = nSize-3;
    if((old_buf->cmd.launch_browser_pro_cmd_req.url.length > 0) &&
      (old_buf->cmd.launch_browser_pro_cmd_req.url.url != NULL)) 
    {
      nSize += old_buf->cmd.launch_browser_pro_cmd_req.url.length+3;
    }
    /* allocate and copy bearer list */
    if((old_buf->cmd.launch_browser_pro_cmd_req.bearer.length > 0) &&
      (old_buf->cmd.launch_browser_pro_cmd_req.bearer.bearer_list != NULL)) 
    {
    
      nSize += (sizeof(AEESTKBearerCodeType) *
                old_buf->cmd.launch_browser_pro_cmd_req.bearer.length+3);
    }
    /* allocate and copy file */
    for (i=0; i< GSTK_MAX_PROVISIONING_FILE_REF; i++) 
    {
      if((old_buf->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].length > 0) &&
        (old_buf->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].file_paths != NULL)) 
      {
        nSize += old_buf->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].length+3;
      }
    }
    /* allocate and copy confirm alpha */
    nStringSize = OEMSTK_GetGSTKStringLen(&old_buf->cmd.launch_browser_pro_cmd_req.user_confirm_alpha);
    nSize += (sizeof(AECHAR)*int32touint32(nStringSize));
    
    /* allocate and copy gateway text string */
    nStringSize = OEMSTK_GetGSTKStringLen(&old_buf->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string);
    nSize += (sizeof(AECHAR)*int32touint32(nStringSize));
    
    /* allocate and copy icon */
    if((old_buf->cmd.launch_browser_pro_cmd_req.user_confirm_icon.present) &&
      (old_buf->cmd.launch_browser_pro_cmd_req.user_confirm_icon.data != NULL)) 
    {
      nSize += old_buf->cmd.launch_browser_pro_cmd_req.user_confirm_icon.size+3;
    }

    *ppOutBuf = (AEESTKLaunchBrowserReq*) MALLOC(nSize);
    if (*ppOutBuf == NULL) {
      return ENOMEMORY;
    }
    (void)MEMSET(*ppOutBuf, 0x00, nSize);

    ptr = (uint8*) *ppOutBuf;
    ptr += ((nStructSize+3)/4)*4;


    if((old_buf->cmd.launch_browser_pro_cmd_req.url.length > 0) &&
      (old_buf->cmd.launch_browser_pro_cmd_req.url.url != NULL)) 
    {
      ((AEESTKLaunchBrowserReq*)(*ppOutBuf))->url.pURL = ptr;
      ptr += ((old_buf->cmd.launch_browser_pro_cmd_req.url.length+3)/4)*4;
    }
    /* allocate and copy bearer list */
    if((old_buf->cmd.launch_browser_pro_cmd_req.bearer.length > 0) &&
      (old_buf->cmd.launch_browser_pro_cmd_req.bearer.bearer_list != NULL)) 
    {
      ((AEESTKLaunchBrowserReq*)(*ppOutBuf))->bearer.pBearerList= (AEESTKBearerCodeType*) ptr;
      ptr += ((sizeof(AEESTKBearerCodeType) * old_buf->cmd.launch_browser_pro_cmd_req.bearer.length + 3)/4)*4;
    }
    /* allocate and copy file */
    for (i=0; i< GSTK_MAX_PROVISIONING_FILE_REF; i++) 
    {
      if((old_buf->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].length > 0) &&
        (old_buf->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].file_paths != NULL)) 
      {
        ((AEESTKLaunchBrowserReq*)(*ppOutBuf))->provisioningFileRef[i].pFilePaths = ptr;
        ptr += ((old_buf->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].length+3)/4)*4;
      }
    }
    /* allocate and copy confirm alpha */
    if((old_buf->cmd.launch_browser_pro_cmd_req.user_confirm_alpha.length > 0) &&
      (old_buf->cmd.launch_browser_pro_cmd_req.user_confirm_alpha.text != NULL)) 
    {
      ((AEESTKLaunchBrowserReq*)(*ppOutBuf))->UserConfirmAlpha.pwszText = (AECHAR*) ptr;
      nStringSize = OEMSTK_GetGSTKStringLen(&old_buf->cmd.launch_browser_pro_cmd_req.user_confirm_alpha);
      ptr += sizeof(AECHAR)*(int32touint32(nStringSize));
    }
    /* allocate and copy gateway text string */
    if((old_buf->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string.length > 0) &&
      (old_buf->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string.text != NULL)) 
    {
      ((AEESTKLaunchBrowserReq*)(*ppOutBuf))->gatewayProxyTextString.pwszText = (AECHAR*) ptr;
      nStringSize = OEMSTK_GetGSTKStringLen(&old_buf->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string);
      ptr += sizeof(AECHAR)*(int32touint32(nStringSize));
    }
    /* allocate and copy icon */
    if((old_buf->cmd.launch_browser_pro_cmd_req.user_confirm_icon.present) &&
      (old_buf->cmd.launch_browser_pro_cmd_req.user_confirm_icon.data != NULL)) 
    {
      ((AEESTKLaunchBrowserReq*)(*ppOutBuf))->UserConfirmIcon.pData = ptr;
      ptr += ((old_buf->cmd.launch_browser_pro_cmd_req.user_confirm_icon.size+3)/4)*4;
    }
    break;

   case GSTK_PLAY_TONE_REQ:
    nSize = sizeof(AEESTKPlayToneReq)+3;
    nStructSize = nSize-3;
    nStringSize = OEMSTK_GetGSTKStringLen(&old_buf->cmd.play_tone_pro_cmd_req.alpha);
    nSize += (sizeof(AECHAR)*int32touint32(nStringSize));
    
    /* allocate and copy icon */
    if((old_buf->cmd.play_tone_pro_cmd_req.icon.present) &&
      (old_buf->cmd.play_tone_pro_cmd_req.icon.data != NULL)) 
    {
      nSize += old_buf->cmd.play_tone_pro_cmd_req.icon.size+3;
    }

    *ppOutBuf = (AEESTKPlayToneReq*) MALLOC(nSize);
    if (*ppOutBuf == NULL) {
      return ENOMEMORY;
    }
    (void)MEMSET(*ppOutBuf, 0x00, nSize);

    ptr = (uint8*) *ppOutBuf;
    ptr += ((nStructSize+3)/4)*4;


    if((old_buf->cmd.play_tone_pro_cmd_req.alpha.length > 0) &&
      (old_buf->cmd.play_tone_pro_cmd_req.alpha.text != NULL)) 
    {
      ((AEESTKPlayToneReq*)(*ppOutBuf))->alpha.pwszText = (AECHAR*) ptr;
      nStringSize = OEMSTK_GetGSTKStringLen(&old_buf->cmd.play_tone_pro_cmd_req.alpha);
      ptr += sizeof(AECHAR)*(int32touint32(nStringSize));
    }
    /* allocate and copy icon */
    if((old_buf->cmd.play_tone_pro_cmd_req.icon.present) &&
      (old_buf->cmd.play_tone_pro_cmd_req.icon.data != NULL)) 
    {
      ((AEESTKPlayToneReq*)(*ppOutBuf))->icon.pData = ptr;
      ptr += ((old_buf->cmd.play_tone_pro_cmd_req.icon.size+3)/4)*4;
    }
    break;

  case GSTK_SELECT_ITEM_REQ:
    nSize = sizeof(AEESTKSelectItemReq)+3;
    nStructSize = nSize-3;
      /* allocate and copy items */
      for (i=0; i< GSTK_MAX_MENU_ITEMS; i++) 
      {
        if((old_buf->cmd.select_item_pro_cmd_req.items[i].length > 0) &&
          (old_buf->cmd.select_item_pro_cmd_req.items[i].item_text != NULL)) 
        {
          nStringSize = OEMSTK_GSTKItemGSMDefaultToAEEString(&old_buf->cmd.select_item_pro_cmd_req.items[i], NULL);
          nSize += (sizeof(AECHAR)*(nStringSize+4));
        }
      }
      /* allocate and copy title */
      nStringSize = OEMSTK_GetGSTKStringLen(&old_buf->cmd.select_item_pro_cmd_req.title);
      nSize += (sizeof(AECHAR)*int32touint32(nStringSize)) + 3;
      
      /* allocate and copy nextActionIndicator */
      if((old_buf->cmd.select_item_pro_cmd_req.next_action_indicator.num_items > 0) &&
        (old_buf->cmd.select_item_pro_cmd_req.next_action_indicator.next_action_list != NULL)) 
      {
        nSize += old_buf->cmd.select_item_pro_cmd_req.next_action_indicator.num_items+3;
      }
      /* allocate and copy icon */
      if((old_buf->cmd.select_item_pro_cmd_req.icon.present) &&
        (old_buf->cmd.select_item_pro_cmd_req.icon.data != NULL)) 
      {
        nSize += old_buf->cmd.select_item_pro_cmd_req.icon.size+3;
      }
      /* allocate and copy icon id list */
      if(old_buf->cmd.select_item_pro_cmd_req.icon_id_list.num_items > 0)
      {
        for(i = 0; i< old_buf->cmd.select_item_pro_cmd_req.icon_id_list.num_items; i++) 
        {
          nSize += (sizeof(AEESTKIcon)+3);
          if(old_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i] != NULL)
          {
            if((old_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->present) &&
              (old_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->data != NULL)) 
            {
              nSize += old_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->size + 3;
            }
          }
        }
      }

      *ppOutBuf = (AEESTKSelectItemReq*) MALLOC(nSize);
      if (*ppOutBuf == NULL) {
        return ENOMEMORY;
      }
      (void)MEMSET(*ppOutBuf, 0x00, nSize);

      ptr = (uint8*) *ppOutBuf;
      ptr += ((nStructSize+3)/4)*4;


    /* allocate and copy items */
      for (i=0; i< GSTK_MAX_MENU_ITEMS; i++) 
      {
        if((old_buf->cmd.select_item_pro_cmd_req.items[i].length > 0) &&
          (old_buf->cmd.select_item_pro_cmd_req.items[i].item_text != NULL)) 
        {
          ((AEESTKSelectItemReq*)(*ppOutBuf))->items[i].pwszText = (AECHAR*) ptr;
          nStringSize = OEMSTK_GSTKItemGSMDefaultToAEEString(&old_buf->cmd.select_item_pro_cmd_req.items[i], NULL);
          ptr += ((sizeof(AECHAR)*(nStringSize+4))/4)*4;
        }
      }
      /* allocate and copy title */
      if((old_buf->cmd.select_item_pro_cmd_req.title.length > 0) &&
        (old_buf->cmd.select_item_pro_cmd_req.title.text != NULL)) 
      {
        ((AEESTKSelectItemReq*)(*ppOutBuf))->title.pwszText = (AECHAR*)ptr;
        nStringSize = OEMSTK_GetGSTKStringLen(&old_buf->cmd.select_item_pro_cmd_req.title);
        ptr += ((sizeof(AECHAR)*(int32touint32(nStringSize)) +3)/4)*4;
      }
      /* allocate and copy nextActionIndicator */
      if((old_buf->cmd.select_item_pro_cmd_req.next_action_indicator.num_items > 0) &&
        (old_buf->cmd.select_item_pro_cmd_req.next_action_indicator.next_action_list != NULL)) 
      {
        ((AEESTKSelectItemReq*)(*ppOutBuf))->nextActionIndicator.pNextActionList = ptr;
        ptr += ((old_buf->cmd.select_item_pro_cmd_req.next_action_indicator.num_items+3)/4)*4;
      }
      /* allocate and copy icon */
      if((old_buf->cmd.select_item_pro_cmd_req.icon.present) &&
        (old_buf->cmd.select_item_pro_cmd_req.icon.data != NULL)) 
      {
        ((AEESTKSelectItemReq*)(*ppOutBuf))->icon.pData = ptr;
        ptr += ((old_buf->cmd.select_item_pro_cmd_req.icon.size+3)/4)*4;
      }
      /* allocate and copy icon id list */
      if(old_buf->cmd.select_item_pro_cmd_req.icon_id_list.num_items > 0)
      {
        for(i = 0; i< old_buf->cmd.select_item_pro_cmd_req.icon_id_list.num_items; i++) 
        {
          ((AEESTKSelectItemReq*)(*ppOutBuf))->iconIdList.pIconList[i] = (AEESTKIcon*)ptr;
          ptr += ((sizeof(AEESTKIcon)+3)/4)*4;
          
          if(old_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i] != NULL)
          {
            if((old_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->present) &&
              (old_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->data != NULL)) 
            {
              ((AEESTKSelectItemReq*)(*ppOutBuf))->iconIdList.pIconList[i]->pData = ptr;
              ptr += ((old_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->size+3)/4)*4;
            }
          }
        }
      }
      break;

  case GSTK_SEND_SMS_REQ:
          /* allocate and copy sms tpdu */
    nSize = sizeof(AEESTKSendSMSReq)+3;
    nStructSize = nSize-3;
    if((old_buf->cmd.send_sms_pro_cmd_req.sms_tpdu.length > 0) &&
      (old_buf->cmd.send_sms_pro_cmd_req.sms_tpdu.tpdu != NULL)) 
    {
      nSize += old_buf->cmd.send_sms_pro_cmd_req.sms_tpdu.length+3;
    }
    /* allocate and copy alpha */
    nStringSize = OEMSTK_GetGSTKStringLen(&old_buf->cmd.send_sms_pro_cmd_req.alpha);
    nSize += (sizeof(AECHAR)*int32touint32(nStringSize));
    
    /* allocate and copy icon */
    if((old_buf->cmd.send_sms_pro_cmd_req.icon.present) &&
      (old_buf->cmd.send_sms_pro_cmd_req.icon.data != NULL)) 
    {
      nSize += old_buf->cmd.send_sms_pro_cmd_req.icon.size+3;
    }

    *ppOutBuf = (AEESTKSendSMSReq*) MALLOC(nSize);
    if (*ppOutBuf == NULL) {
      return ENOMEMORY;
    }
    (void)MEMSET(*ppOutBuf, 0x00, nSize);

    ptr = (uint8*) *ppOutBuf;
    ptr += ((nStructSize+3)/4)*4;

    /* allocate and copy sms tpdu */
    if((old_buf->cmd.send_sms_pro_cmd_req.sms_tpdu.length > 0) &&
      (old_buf->cmd.send_sms_pro_cmd_req.sms_tpdu.tpdu != NULL)) 
    {
      ((AEESTKSendSMSReq*)(*ppOutBuf))->smsTPDU.pTPDU = ptr;
      ptr += ((old_buf->cmd.send_sms_pro_cmd_req.sms_tpdu.length+3)/4)*4;
    }
    /* allocate and copy alpha */
    if((old_buf->cmd.send_sms_pro_cmd_req.alpha.length > 0) &&
      (old_buf->cmd.send_sms_pro_cmd_req.alpha.text != NULL)) 
    {
      ((AEESTKSendSMSReq*)(*ppOutBuf))->alpha.pwszText = (AECHAR*) ptr;
      nStringSize = OEMSTK_GetGSTKStringLen(&old_buf->cmd.send_sms_pro_cmd_req.alpha);
      ptr += sizeof(AECHAR)*(int32touint32(nStringSize));
    }
    /* allocate and copy icon */
    if((old_buf->cmd.send_sms_pro_cmd_req.icon.present) &&
      (old_buf->cmd.send_sms_pro_cmd_req.icon.data != NULL)) 
    {
      ((AEESTKSendSMSReq*)(*ppOutBuf))->icon.pData = ptr;
      ptr += ((old_buf->cmd.send_sms_pro_cmd_req.icon.size+3)/4)*4;
    }

    break;

  case GSTK_SEND_SS_REQ:
    nSize = sizeof(AEESTKSendSSReq)+3;
    nStructSize = nSize-3;
    /* allocate and copy alpha */
    nStringSize = OEMSTK_GetGSTKStringLen(&old_buf->cmd.send_ss_pro_cmd_req.alpha);
    nSize += (sizeof(AECHAR)*int32touint32(nStringSize));
    
    /* allocate and copy icon */
    if((old_buf->cmd.send_ss_pro_cmd_req.icon.present) &&
      (old_buf->cmd.send_ss_pro_cmd_req.icon.data != NULL)) 
    {
      nSize += old_buf->cmd.send_ss_pro_cmd_req.icon.size+3;
    }

    *ppOutBuf = (AEESTKSendSSReq*) MALLOC(nSize);
    if (*ppOutBuf == NULL) {
      return ENOMEMORY;
    }
    (void)MEMSET(*ppOutBuf, 0x00, nSize);

    ptr = (uint8*) *ppOutBuf;
    ptr += ((nStructSize+3)/4)*4;

    /* allocate and copy alpha */
    if((old_buf->cmd.send_ss_pro_cmd_req.alpha.length > 0) &&
      (old_buf->cmd.send_ss_pro_cmd_req.alpha.text != NULL)) 
    {
      ((AEESTKSendSSReq*)(*ppOutBuf))->alpha.pwszText = (AECHAR*) ptr;
      nStringSize = OEMSTK_GetGSTKStringLen(&old_buf->cmd.send_ss_pro_cmd_req.alpha);
      ptr += sizeof(AECHAR)*(int32touint32(nStringSize));
    }
    /* allocate and copy icon */
    if((old_buf->cmd.send_ss_pro_cmd_req.icon.present) &&
      (old_buf->cmd.send_ss_pro_cmd_req.icon.data != NULL)) 
    {
      ((AEESTKSendSSReq*)(*ppOutBuf))->icon.pData = ptr;
      ptr += ((old_buf->cmd.send_ss_pro_cmd_req.icon.size+3)/4)*4;
    }
    break;

  case GSTK_SEND_USSD_REQ:
    nSize = sizeof(AEESTKSendUSSDReq)+3;
    nStructSize = nSize-3;
    if((old_buf->cmd.send_ussd_pro_cmd_req.ussd_string.length > 0) &&
      (old_buf->cmd.send_ussd_pro_cmd_req.ussd_string.text != NULL)) 
    {
      nSize += (sizeof(AECHAR)*(old_buf->cmd.send_ussd_pro_cmd_req.ussd_string.length+4));
    }
    /* allocate and copy alpha */
    nStringSize = OEMSTK_GetGSTKStringLen(&old_buf->cmd.send_ussd_pro_cmd_req.alpha);
    nSize += (sizeof(AECHAR)*int32touint32(nStringSize));
    
    /* allocate and copy icon */
    if((old_buf->cmd.send_ussd_pro_cmd_req.icon.present) &&
      (old_buf->cmd.send_ussd_pro_cmd_req.icon.data != NULL)) 
    {
      nSize += old_buf->cmd.send_ussd_pro_cmd_req.icon.size+3;
    }

    *ppOutBuf = (AEESTKSendUSSDReq*) MALLOC(nSize);
    if (*ppOutBuf == NULL) {
      return ENOMEMORY;
    }
    (void)MEMSET(*ppOutBuf, 0x00, nSize);

    ptr = (uint8*) *ppOutBuf;
    ptr += ((nStructSize+3)/4)*4;

        /* allocate and copy ussd string */
    if((old_buf->cmd.send_ussd_pro_cmd_req.ussd_string.length > 0) &&
      (old_buf->cmd.send_ussd_pro_cmd_req.ussd_string.text != NULL)) 
    {
      ((AEESTKSendUSSDReq*)(*ppOutBuf))->ussdString.pwszText = (AECHAR*) ptr;
      ptr += ((sizeof(AECHAR)*(old_buf->cmd.send_ussd_pro_cmd_req.ussd_string.length+4))/4)*4;
    }
    /* allocate and copy alpha */
    if((old_buf->cmd.send_ussd_pro_cmd_req.alpha.length > 0) &&
      (old_buf->cmd.send_ussd_pro_cmd_req.alpha.text != NULL)) 
    {
      ((AEESTKSendUSSDReq*)(*ppOutBuf))->alpha.pwszText = (AECHAR*) ptr;
      nStringSize = OEMSTK_GetGSTKStringLen(&old_buf->cmd.send_ussd_pro_cmd_req.alpha);
      ptr += sizeof(AECHAR)*(int32touint32(nStringSize));
    }
    /* allocate and copy icon */
    if((old_buf->cmd.send_ussd_pro_cmd_req.icon.present) &&
      (old_buf->cmd.send_ussd_pro_cmd_req.icon.data != NULL)) 
    {
      ((AEESTKSendUSSDReq*)(*ppOutBuf))->icon.pData = ptr;
      ptr += ((old_buf->cmd.send_ussd_pro_cmd_req.icon.size+3)/4)*4;
    }

    break;

  case GSTK_SETUP_CALL_REQ:
    nSize = sizeof(AEESTKSetupCallReq)+3;
    nStructSize = nSize-3;

    /* allocate and copy userCnfAlpha */
    nStringSize = OEMSTK_GetGSTKStringLen(&old_buf->cmd.setup_call_pro_cmd_req.user_cnf_alpha);
    nSize += (sizeof(AECHAR)*int32touint32(nStringSize));
    
    /* allocate and copy Setup Call Display Alpha */
    nStringSize = OEMSTK_GetGSTKStringLen(&old_buf->cmd.setup_call_pro_cmd_req.setup_call_display_alpha);
    nSize += (sizeof(AECHAR)*int32touint32(nStringSize));
    
    /* allocate and copy userCnfIcon */
    if((old_buf->cmd.setup_call_pro_cmd_req.user_cnf_icon.present) &&
      (old_buf->cmd.setup_call_pro_cmd_req.user_cnf_icon.data != NULL)) 
    {
      nSize += old_buf->cmd.setup_call_pro_cmd_req.user_cnf_icon.size+3;
    }
    /* allocate and copy setupCallDisplayIcon */
    if((old_buf->cmd.setup_call_pro_cmd_req.setup_call_display_icon.present) &&
      (old_buf->cmd.setup_call_pro_cmd_req.setup_call_display_icon.data != NULL)) 
    {
      nSize += old_buf->cmd.setup_call_pro_cmd_req.setup_call_display_icon.size+3;
    }

    *ppOutBuf = (AEESTKSetupCallReq*) MALLOC(nSize);
    if (*ppOutBuf == NULL) {
      return ENOMEMORY;
    }
    (void)MEMSET(*ppOutBuf, 0x00, nSize);

    ptr = (uint8*) *ppOutBuf;
    ptr += ((nStructSize+3)/4)*4;

    /* allocate and copy userCnfAlpha */
    if((old_buf->cmd.setup_call_pro_cmd_req.user_cnf_alpha.length > 0) &&
      (old_buf->cmd.setup_call_pro_cmd_req.user_cnf_alpha.text != NULL)) 
    {
      ((AEESTKSetupCallReq*)(*ppOutBuf))->userCnfAlpha.pwszText = (AECHAR*) ptr;
      nStringSize = OEMSTK_GetGSTKStringLen(&old_buf->cmd.setup_call_pro_cmd_req.user_cnf_alpha);
      ptr += sizeof(AECHAR)*(int32touint32(nStringSize));
    }
    /* allocate and copy userCnfAlpha */
    if((old_buf->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.length > 0) &&
      (old_buf->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.text != NULL)) 
    {
      ((AEESTKSetupCallReq*)(*ppOutBuf))->setupCallDisplayAlpha.pwszText = (AECHAR*) ptr;
      nStringSize = OEMSTK_GetGSTKStringLen(&old_buf->cmd.setup_call_pro_cmd_req.setup_call_display_alpha);
      ptr += sizeof(AECHAR)*(int32touint32(nStringSize));
    }
    /* allocate and copy userCnfIcon */
    if((old_buf->cmd.setup_call_pro_cmd_req.user_cnf_icon.present) &&
      (old_buf->cmd.setup_call_pro_cmd_req.user_cnf_icon.data != NULL)) 
    {
      ((AEESTKSetupCallReq*)(*ppOutBuf))->userCnfIcon.pData = ptr;
      ptr += ((old_buf->cmd.setup_call_pro_cmd_req.user_cnf_icon.size+3)/4)*4;
    }
    /* allocate and copy setupCallDisplayIcon */
    if((old_buf->cmd.setup_call_pro_cmd_req.setup_call_display_icon.present) &&
      (old_buf->cmd.setup_call_pro_cmd_req.setup_call_display_icon.data != NULL)) 
    {
      ((AEESTKSetupCallReq*)(*ppOutBuf))->setupCallDisplayIcon.pData = ptr;
      ptr += ((old_buf->cmd.setup_call_pro_cmd_req.setup_call_display_icon.size+3)/4)*4;
    }
    break;

  case GSTK_SETUP_IDLE_TEXT_REQ:
    nSize = sizeof(AEESTKSetupIdleModeTextReq)+3;
    nStructSize = nSize-3;

    nStringSize = OEMSTK_GetGSTKStringLen(&old_buf->cmd.setup_idle_mode_txt_pro_cmd_req.text_string);
    nSize += (sizeof(AECHAR)*int32touint32(nStringSize));

    /* allocate and copy icon */
    if((old_buf->cmd.setup_idle_mode_txt_pro_cmd_req.icon.present) &&
      (old_buf->cmd.setup_idle_mode_txt_pro_cmd_req.icon.data != NULL)) 
    {
      nSize += old_buf->cmd.setup_idle_mode_txt_pro_cmd_req.icon.size+3;
    }

    *ppOutBuf = (AEESTKSetupIdleModeTextReq*) MALLOC(nSize);
    if (*ppOutBuf == NULL) {
      return ENOMEMORY;
    }
    (void)MEMSET(*ppOutBuf, 0x00, nSize);

    ptr = (uint8*) *ppOutBuf;
    ptr += ((nStructSize+3)/4)*4;

    if((old_buf->cmd.setup_idle_mode_txt_pro_cmd_req.text_string.length > 0) &&
      (old_buf->cmd.setup_idle_mode_txt_pro_cmd_req.text_string.text != NULL)) 
    {
      ((AEESTKSetupIdleModeTextReq*)(*ppOutBuf))->string.pwszText = (AECHAR*) ptr;
      nStringSize = OEMSTK_GetGSTKStringLen(&old_buf->cmd.setup_idle_mode_txt_pro_cmd_req.text_string);
      ptr += sizeof(AECHAR)*(int32touint32(nStringSize));
    }
    /* allocate and copy icon */
    if((old_buf->cmd.setup_idle_mode_txt_pro_cmd_req.icon.present) &&
      (old_buf->cmd.setup_idle_mode_txt_pro_cmd_req.icon.data != NULL)) 
    {
      ((AEESTKSetupIdleModeTextReq*)(*ppOutBuf))->icon.pData = ptr;
      ptr += ((old_buf->cmd.setup_idle_mode_txt_pro_cmd_req.icon.size+3)/4)*4;
    }

    break;

  case GSTK_SETUP_MENU_REQ:
    nSize = sizeof(AEESTKSetupMenuReq)+3;
    nStructSize = nSize-3;
    /* allocate and copy items */
    for (i=0; i< GSTK_MAX_MENU_ITEMS; i++) 
    {
      if((old_buf->cmd.setup_menu_pro_cmd_req.items[i].length > 0) &&
        (old_buf->cmd.setup_menu_pro_cmd_req.items[i].item_text != NULL)) 
      {
        nStringSize = OEMSTK_GSTKItemGSMDefaultToAEEString(&old_buf->cmd.setup_menu_pro_cmd_req.items[i], NULL);
        nSize += (sizeof(AECHAR)*(nStringSize+4));
      }
    }
    /* allocate and copy title */
    nStringSize = OEMSTK_GetGSTKStringLen(&old_buf->cmd.setup_menu_pro_cmd_req.title);
    nSize += (sizeof(AECHAR)*int32touint32(nStringSize)) + 3;
    
    /* allocate and copy nextActionIndicator */
    if((old_buf->cmd.setup_menu_pro_cmd_req.next_action_indicator.num_items > 0) &&
      (old_buf->cmd.setup_menu_pro_cmd_req.next_action_indicator.next_action_list != NULL)) 
    {
      nSize += old_buf->cmd.setup_menu_pro_cmd_req.next_action_indicator.num_items+3;
    }
    /* allocate and copy icon */
    if((old_buf->cmd.setup_menu_pro_cmd_req.icon.present) &&
      (old_buf->cmd.setup_menu_pro_cmd_req.icon.data != NULL)) 
    {
      nSize += old_buf->cmd.setup_menu_pro_cmd_req.icon.size+3;
    }
    /* allocate and copy icon id list */
    if(old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.num_items > 0)
    {
      for(i = 0; i< old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.num_items; i++) 
      {
        nSize += (sizeof(AEESTKIcon)+3);
        if(old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i] != NULL) 
        {
          if((old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->present) &&
            (old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->data != NULL)) 
          {
            nSize += old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->size+3;
          }
        }
      }
    }

    *ppOutBuf = (AEESTKSetupMenuReq*) MALLOC(nSize);
    if (*ppOutBuf == NULL) {
      return ENOMEMORY;
    }
    (void)MEMSET(*ppOutBuf, 0x00, nSize);

    ptr = (uint8*) *ppOutBuf;
    ptr += ((nStructSize+3)/4)*4;

        /* allocate and copy items */
    for (i=0; i< GSTK_MAX_MENU_ITEMS; i++) 
    {
      if((old_buf->cmd.setup_menu_pro_cmd_req.items[i].length > 0) &&
        (old_buf->cmd.setup_menu_pro_cmd_req.items[i].item_text != NULL)) 
      {
        ((AEESTKSetupMenuReq*)(*ppOutBuf))->items[i].pwszText = (AECHAR*) ptr;
        nStringSize = OEMSTK_GSTKItemGSMDefaultToAEEString(&old_buf->cmd.setup_menu_pro_cmd_req.items[i], NULL);
        ptr += ((sizeof(AECHAR)*(nStringSize+4))/4)*4;
      }
    }
    /* allocate and copy title */
    if((old_buf->cmd.setup_menu_pro_cmd_req.title.length > 0) &&
      (old_buf->cmd.setup_menu_pro_cmd_req.title.text != NULL)) 
    {
      ((AEESTKSetupMenuReq*)(*ppOutBuf))->title.pwszText = (AECHAR*) ptr;
      nStringSize = OEMSTK_GetGSTKStringLen(&old_buf->cmd.setup_menu_pro_cmd_req.title);
      ptr += ((sizeof(AECHAR)*(int32touint32(nStringSize)) + 3)/4)*4;
    }
    /* allocate and copy nextActionIndicator */
    if((old_buf->cmd.setup_menu_pro_cmd_req.next_action_indicator.num_items > 0) &&
      (old_buf->cmd.setup_menu_pro_cmd_req.next_action_indicator.next_action_list != NULL)) 
    {
      ((AEESTKSetupMenuReq*)(*ppOutBuf))->nextActionIndicator.pNextActionList = ptr;
      ptr += ((old_buf->cmd.setup_menu_pro_cmd_req.next_action_indicator.num_items+3)/4)*4;
    }
    /* allocate and copy icon */
    if((old_buf->cmd.setup_menu_pro_cmd_req.icon.present) &&
      (old_buf->cmd.setup_menu_pro_cmd_req.icon.data != NULL)) 
    {
      ((AEESTKSetupMenuReq*)(*ppOutBuf))->icon.pData = ptr;
      ptr += ((old_buf->cmd.setup_menu_pro_cmd_req.icon.size+3)/4)*4;
    }
    /* allocate and copy icon id list */
    if(old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.num_items > 0)
    {
      for(i = 0; i< old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.num_items; i++) 
      {
        ((AEESTKSetupMenuReq*)(*ppOutBuf))->iconIdList.pIconList[i] = (AEESTKIcon*) ptr;
        ptr += ((sizeof(AEESTKIcon)+3)/4)*4;
        if(old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i] != NULL) 
        {
          if((old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->present) &&
            (old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->data != NULL)) 
          {
            ((AEESTKSetupMenuReq*)(*ppOutBuf))->iconIdList.pIconList[i]->pData = ptr;
            ptr += ((old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->size+3)/4)*4;
          }
        }
      }
    }
    break;

  case GSTK_SEND_DTMF_REQ:

    nSize = sizeof(AEESTKSendDTMFReq)+3;
    nStructSize = nSize-3;
    //Calculate the string size.
    nStringSize = OEMSTK_GetGSTKStringLen(&old_buf->cmd.send_dtmf_pro_cmd_req.alpha);
    nSize += (sizeof(AECHAR)*int32touint32(nStringSize));
        
    //Calculate the icon size.
    if((old_buf->cmd.send_dtmf_pro_cmd_req.icon.present) &&
        (old_buf->cmd.send_dtmf_pro_cmd_req.icon.data != NULL)) 
    {
       nSize += old_buf->cmd.send_dtmf_pro_cmd_req.icon.size+3;
    }

    // DTMF String
    if((old_buf->cmd.send_dtmf_pro_cmd_req.dtmf.length > 0) &&
       (old_buf->cmd.send_dtmf_pro_cmd_req.dtmf.dtmf != NULL))
    {
      nSize += old_buf->cmd.send_dtmf_pro_cmd_req.dtmf.length+3;
    }
    //Allocate the buffer
    *ppOutBuf = (AEESTKSendDTMFReq*) MALLOC(nSize);
    if (*ppOutBuf == NULL) {
      return ENOMEMORY;
    }
    (void)MEMSET(*ppOutBuf, 0x00, nSize);

    ptr = (uint8*) *ppOutBuf;
    ptr += ((nStructSize+3)/4)*4;
    
    //Set the internal pointers
    //String
    if((old_buf->cmd.send_dtmf_pro_cmd_req.alpha.length > 0) &&
        (old_buf->cmd.send_dtmf_pro_cmd_req.alpha.text != NULL)) 
    {
      ((AEESTKSendDTMFReq*)(*ppOutBuf))->alpha.pwszText = (AECHAR*) ptr;
      nStringSize = OEMSTK_GetGSTKStringLen(&old_buf->cmd.send_dtmf_pro_cmd_req.alpha);
      ptr += sizeof(AECHAR)*(int32touint32(nStringSize));
    }
    //Icon
    if((old_buf->cmd.send_dtmf_pro_cmd_req.icon.present) &&
        (old_buf->cmd.send_dtmf_pro_cmd_req.icon.data != NULL)) 
    {
      ((AEESTKSendDTMFReq*)(*ppOutBuf))->icon.pData = ptr;
      ptr += old_buf->cmd.send_dtmf_pro_cmd_req.icon.size;
    }

    if((old_buf->cmd.send_dtmf_pro_cmd_req.dtmf.length) &&
        (old_buf->cmd.send_dtmf_pro_cmd_req.dtmf.dtmf!= NULL)) 
    {
      ((AEESTKSendDTMFReq*)(*ppOutBuf))->pDTMFString = ptr;
      ptr += ((old_buf->cmd.send_dtmf_pro_cmd_req.dtmf.length+3)/4)*4;
    }

    break;


  case GSTK_MORE_TIME_REQ:
    nSize = ((sizeof(AEESTKMoreTimeReq)+3)/4)*4;
    *ppOutBuf =  MALLOC(nSize);
    if (*ppOutBuf == NULL) {
      return ENOMEMORY;
    }
    (void)MEMSET(*ppOutBuf, 0x00, nSize);

    break;

  case GSTK_END_PROACTIVE_CMD_REQ:
    nSize = ((sizeof(AEESTKEndProactiveCmdReq)+3)/4)*4;
    *ppOutBuf =  MALLOC(nSize);
    if (*ppOutBuf == NULL) {
      return ENOMEMORY;
    }
    (void)MEMSET(*ppOutBuf, 0x00, nSize);

    break;

  case GSTK_PROVIDE_LANG_INFO_REQ:
    nSize = ((sizeof(AEESTKProvideLangReq)+3)/4)*4;
    *ppOutBuf =  MALLOC(nSize);
    if (*ppOutBuf == NULL) {
      return ENOMEMORY;
    }
    (void)MEMSET(*ppOutBuf, 0x00, nSize);
    break;

   case GSTK_PROVIDE_BATTERY_STATUS_REQ:
#ifdef FEATURE_CAT_REL6 
    nSize = ((sizeof(AEESTKProvideBatteryStatusReq)+3)/4)*4;
    *ppOutBuf =  MALLOC(nSize);
    if (*ppOutBuf == NULL) 
    {
      return ENOMEMORY;
    }
    (void)MEMSET(*ppOutBuf, 0x00, nSize);
    break;
#else
    MSG_HIGH(" GSTK_PROVIDE_BATTERY_STATUS_REQ : FEATURE_CAT_REL6  not supported ",0,0,0);
    return EFAILED;
#endif /* #ifdef FEATURE_CAT_REL6 */
  case GSTK_LANG_NOTIFICATION_REQ:
    nSize = ((sizeof(AEESTKLangInfoReq)+3)/4)*4;
    *ppOutBuf = MALLOC(nSize);
    if (*ppOutBuf == NULL) {
      return ENOMEMORY;
    }
    (void)MEMSET(*ppOutBuf, 0x00, nSize);
    break;

  case GSTK_OPEN_CH_REQ:
    nSize = sizeof(AEESTKOpenChReq)+3;
    nStructSize = nSize-3;
    /* Calculate the string size */
    if((old_buf->cmd.open_ch_pro_cmd_req.alpha.alphabet_given) &&
       (old_buf->cmd.open_ch_pro_cmd_req.alpha.length > 0) &&
       (old_buf->cmd.open_ch_pro_cmd_req.alpha.alphabet != NULL)) 
    {
       nSize += (sizeof(AECHAR)*int32touint32(old_buf->cmd.open_ch_pro_cmd_req.alpha.length+3));
    }
    
    /* Calculate the icon size */
    if((old_buf->cmd.open_ch_pro_cmd_req.icon.present) &&
        (old_buf->cmd.open_ch_pro_cmd_req.icon.data != NULL)) 
    {
       nSize += old_buf->cmd.open_ch_pro_cmd_req.icon.size+3;
    }

    /* Calculate the access point name size. */
    if((old_buf->cmd.open_ch_pro_cmd_req.access_point_name.alphabet_given) &&
      (old_buf->cmd.open_ch_pro_cmd_req.access_point_name.length > 0) &&
      (old_buf->cmd.open_ch_pro_cmd_req.access_point_name.alphabet != NULL)) 
    {
       nSize += (sizeof(AECHAR)*int32touint32(old_buf->cmd.open_ch_pro_cmd_req.access_point_name.length+3));
    }

    /* Calculate the local address size. */
    if((old_buf->cmd.open_ch_pro_cmd_req.local_address.pkt_address.length > 0) &&
      (old_buf->cmd.open_ch_pro_cmd_req.local_address.pkt_address.address != NULL)) 
    {
       nSize += int32touint32(old_buf->cmd.open_ch_pro_cmd_req.local_address.pkt_address.length+3);
    }

    /* Calculate the destination address size. */
    if((old_buf->cmd.open_ch_pro_cmd_req.data_dest_address.pkt_address.length > 0) &&
      (old_buf->cmd.open_ch_pro_cmd_req.data_dest_address.pkt_address.address != NULL)) 
    {
       nSize += int32touint32(old_buf->cmd.open_ch_pro_cmd_req.local_address.pkt_address.length+3);
    }

    /* Calculate the User login size */
    if((old_buf->cmd.open_ch_pro_cmd_req.text_string_user_login.alphabet_given) &&
      (old_buf->cmd.open_ch_pro_cmd_req.text_string_user_login.length > 0) &&
      (old_buf->cmd.open_ch_pro_cmd_req.text_string_user_login.alphabet != NULL)) 
    {
       nSize += (sizeof(AECHAR)*int32touint32(old_buf->cmd.open_ch_pro_cmd_req.text_string_user_login.length+3));
    }

    /* Calculate the User Password size */
    if((old_buf->cmd.open_ch_pro_cmd_req.text_string_user_passwd.alphabet_given) &&
      (old_buf->cmd.open_ch_pro_cmd_req.text_string_user_passwd.length > 0) &&
      (old_buf->cmd.open_ch_pro_cmd_req.text_string_user_passwd.alphabet != NULL)) 
    {
       nSize +=(sizeof(AECHAR)*int32touint32(old_buf->cmd.open_ch_pro_cmd_req.text_string_user_passwd.length+3));
    }

    /* Allocate the buffer */
    *ppOutBuf = (AEESTKOpenChReq*) MALLOC(nSize);
    if (*ppOutBuf == NULL) {
      return ENOMEMORY;
    }
    (void)MEMSET(*ppOutBuf, 0x00, nSize);

    ptr = (uint8*) *ppOutBuf;
    ptr += ((nStructSize+3)/4)*4;
    
    //Set the internal pointers
    /* alpha */
    if((old_buf->cmd.open_ch_pro_cmd_req.alpha.length > 0) &&
      (old_buf->cmd.open_ch_pro_cmd_req.alpha.alphabet_given) &&
      (old_buf->cmd.open_ch_pro_cmd_req.alpha.alphabet != NULL)) 
    {
      ((AEESTKOpenChReq*)(*ppOutBuf))->sAlpha.pwszText = (AECHAR*) ptr;
      ptr += ((sizeof(AECHAR)*int32touint32(old_buf->cmd.open_ch_pro_cmd_req.alpha.length+3))/4)*4;
    }
    /* Icon */
    if((old_buf->cmd.open_ch_pro_cmd_req.icon.present) &&
        (old_buf->cmd.open_ch_pro_cmd_req.icon.data != NULL)) 
    {
      ((AEESTKOpenChReq*)(*ppOutBuf))->sIcon.pData = ptr;
      ptr += ((old_buf->cmd.open_ch_pro_cmd_req.icon.size+3)/4)*4;
    }

    /* access point name */
    if((old_buf->cmd.open_ch_pro_cmd_req.access_point_name.alphabet_given) &&
      (old_buf->cmd.open_ch_pro_cmd_req.access_point_name.length > 0) &&
      (old_buf->cmd.open_ch_pro_cmd_req.access_point_name.alphabet != NULL)) 
    {
      ((AEESTKOpenChReq*)(*ppOutBuf))->sAccessPointName.pwszText = (AECHAR*)ptr;
      ptr += ((sizeof(AECHAR)*int32touint32(old_buf->cmd.open_ch_pro_cmd_req.access_point_name.length+3))/4)*4;
    }

    /* local address */
    if((old_buf->cmd.open_ch_pro_cmd_req.local_address.pkt_address.length > 0) &&
      (old_buf->cmd.open_ch_pro_cmd_req.local_address.pkt_address.address != NULL)) 
    {
      ((AEESTKOpenChReq*)(*ppOutBuf))->sLocalAddress.pPktAddress = ptr;
      ptr += ((old_buf->cmd.open_ch_pro_cmd_req.local_address.pkt_address.length+3)/4)*4;
    }

    /* destination address */
    if((old_buf->cmd.open_ch_pro_cmd_req.data_dest_address.pkt_address.length > 0) &&
      (old_buf->cmd.open_ch_pro_cmd_req.data_dest_address.pkt_address.address != NULL)) 
    {
      ((AEESTKOpenChReq*)(*ppOutBuf))->sDataDestAddress.pPktAddress = ptr;
      ptr += ((old_buf->cmd.open_ch_pro_cmd_req.data_dest_address.pkt_address.length+3)/4)*4;
    }

    /* user login */
    if((old_buf->cmd.open_ch_pro_cmd_req.text_string_user_login.alphabet_given) &&
      (old_buf->cmd.open_ch_pro_cmd_req.text_string_user_login.length > 0) &&
      (old_buf->cmd.open_ch_pro_cmd_req.text_string_user_login.alphabet != NULL)) 
    {
      ((AEESTKOpenChReq*)(*ppOutBuf))->sUserLogin.pwszText = (AECHAR*)ptr;
      ptr += ((sizeof(AECHAR)*int32touint32(old_buf->cmd.open_ch_pro_cmd_req.text_string_user_login.length+3))/4)*4;
    }

    /* user password */
    if((old_buf->cmd.open_ch_pro_cmd_req.text_string_user_passwd.alphabet_given) &&
      (old_buf->cmd.open_ch_pro_cmd_req.text_string_user_passwd.length > 0) &&
      (old_buf->cmd.open_ch_pro_cmd_req.text_string_user_passwd.alphabet != NULL)) 
    {
      ((AEESTKOpenChReq*)(*ppOutBuf))->sUserPassword.pwszText = (AECHAR*)ptr;
      ptr += ((sizeof(AECHAR)*int32touint32(old_buf->cmd.open_ch_pro_cmd_req.text_string_user_passwd.length+3))/4)*4;
    }
    break;

  case GSTK_CLOSE_CH_REQ:
    nSize = sizeof(AEESTKCloseChReq)+3;
    nStructSize = nSize-3;
    /* Calculate the string size */
    if((old_buf->cmd.close_ch_pro_cmd_req.alpha.alphabet_given) &&
       (old_buf->cmd.close_ch_pro_cmd_req.alpha.length > 0) &&
       (old_buf->cmd.close_ch_pro_cmd_req.alpha.alphabet != NULL)) 
    {
       nSize += (sizeof(AECHAR)*int32touint32(old_buf->cmd.close_ch_pro_cmd_req.alpha.length+3));
    }
    
    /* Calculate the icon size */
    if((old_buf->cmd.close_ch_pro_cmd_req.icon.present) &&
        (old_buf->cmd.close_ch_pro_cmd_req.icon.data != NULL)) 
    {
       nSize += old_buf->cmd.close_ch_pro_cmd_req.icon.size+3;
    }

    /*Allocate the buffer */
    *ppOutBuf = (AEESTKCloseChReq*) MALLOC(nSize);
    if (*ppOutBuf == NULL) {
      return ENOMEMORY;
    }
    (void)MEMSET(*ppOutBuf, 0x00, nSize);

    ptr = (uint8*) *ppOutBuf;
    ptr += ((nStructSize+3)/4)*4;
    
    /* Set the internal pointers */
    /* alpha */
    if((old_buf->cmd.close_ch_pro_cmd_req.alpha.length > 0) &&
      (old_buf->cmd.close_ch_pro_cmd_req.alpha.alphabet_given) &&
      (old_buf->cmd.close_ch_pro_cmd_req.alpha.alphabet != NULL)) 
    {
      ((AEESTKCloseChReq*)(*ppOutBuf))->sAlpha.pwszText = (AECHAR*) ptr;
      ptr += ((sizeof(AECHAR)*int32touint32(old_buf->cmd.close_ch_pro_cmd_req.alpha.length+3))/4)*4;
    }
    /* Icon */
    if((old_buf->cmd.close_ch_pro_cmd_req.icon.present) &&
        (old_buf->cmd.close_ch_pro_cmd_req.icon.data != NULL)) 
    {
      ((AEESTKCloseChReq*)(*ppOutBuf))->sIcon.pData = ptr;
      ptr += ((old_buf->cmd.close_ch_pro_cmd_req.icon.size+3)/4)*4;
    }
    break;

  case GSTK_SEND_DATA_REQ:
    nSize = sizeof(AEESTKSendDataReq)+3;
    nStructSize = nSize-3;
    /* Calculate the string size */
    if((old_buf->cmd.send_data_pro_cmd_req.alpha.alphabet_given) &&
       (old_buf->cmd.send_data_pro_cmd_req.alpha.length > 0) &&
       (old_buf->cmd.send_data_pro_cmd_req.alpha.alphabet != NULL)) 
    {
       nSize += (sizeof(AECHAR)*int32touint32(old_buf->cmd.send_data_pro_cmd_req.alpha.length+3));
    }
    
    /* Calculate the icon size */
    if((old_buf->cmd.send_data_pro_cmd_req.icon.present) &&
        (old_buf->cmd.send_data_pro_cmd_req.icon.data != NULL)) 
    {
       nSize += old_buf->cmd.send_data_pro_cmd_req.icon.size+3;
    }

    /* Calculate the channel data size */
    if((old_buf->cmd.send_data_pro_cmd_req.ch_data.data_len > 0) &&
        (old_buf->cmd.send_data_pro_cmd_req.ch_data.data != NULL)) 
    {
       nSize += int32touint32(old_buf->cmd.send_data_pro_cmd_req.ch_data.data_len+3);
    }

    /* Allocate the buffer */
    *ppOutBuf = (AEESTKSendDataReq*) MALLOC(nSize);
    if (*ppOutBuf == NULL) {
      return ENOMEMORY;
    }
    (void)MEMSET(*ppOutBuf, 0x00, nSize);

    ptr = (uint8*) *ppOutBuf;
    ptr += ((nStructSize+3)/4)*4;
    
    /* Set the internal pointers */
    /* alpha */
    if((old_buf->cmd.send_data_pro_cmd_req.alpha.length > 0) &&
      (old_buf->cmd.send_data_pro_cmd_req.alpha.alphabet_given) &&
      (old_buf->cmd.send_data_pro_cmd_req.alpha.alphabet != NULL)) 
    {
      ((AEESTKSendDataReq*)(*ppOutBuf))->sAlpha.pwszText = (AECHAR*) ptr;
      ptr += ((sizeof(AECHAR)*int32touint32(old_buf->cmd.send_data_pro_cmd_req.alpha.length+3))/4)*4;
    }
    /* Icon */
    if((old_buf->cmd.send_data_pro_cmd_req.icon.present) &&
        (old_buf->cmd.send_data_pro_cmd_req.icon.data != NULL)) 
    {
      ((AEESTKSendDataReq*)(*ppOutBuf))->sIcon.pData = ptr;
      ptr += ((old_buf->cmd.send_data_pro_cmd_req.icon.size+3)/4)*4;
    }
    /* Channel data */
    if((old_buf->cmd.send_data_pro_cmd_req.ch_data.data_len > 0) &&
        (old_buf->cmd.send_data_pro_cmd_req.ch_data.data != NULL)) 
    {
      ((AEESTKSendDataReq*)(*ppOutBuf))->sChData.pData = ptr;
      ptr += ((old_buf->cmd.send_data_pro_cmd_req.ch_data.data_len+3)/4)*4;
    }    
    break;

  case GSTK_RECEIVE_DATA_REQ:
    nSize = sizeof(AEESTKReceiveDataReq)+3;
    nStructSize = nSize-3;
    /* Calculate the string size */
    if((old_buf->cmd.receive_data_pro_cmd_req.alpha.alphabet_given) &&
       (old_buf->cmd.receive_data_pro_cmd_req.alpha.length > 0) &&
       (old_buf->cmd.receive_data_pro_cmd_req.alpha.alphabet != NULL)) 
    {
       nSize += (sizeof(AECHAR)*int32touint32(old_buf->cmd.receive_data_pro_cmd_req.alpha.length+3));
    }
    
    /* Calculate the icon size */
    if((old_buf->cmd.receive_data_pro_cmd_req.icon.present) &&
        (old_buf->cmd.receive_data_pro_cmd_req.icon.data != NULL)) 
    {
       nSize += old_buf->cmd.receive_data_pro_cmd_req.icon.size+3;
    }

    /* Allocate the buffer */
    *ppOutBuf = (AEESTKReceiveDataReq*) MALLOC(nSize);
    if (*ppOutBuf == NULL) {
      return ENOMEMORY;
    }
    (void)MEMSET(*ppOutBuf, 0x00, nSize);

    ptr = (uint8*) *ppOutBuf;
    ptr += ((nStructSize+3)/4)*4;
    
    /* Set the internal pointers */
    /* alpha */
    if((old_buf->cmd.receive_data_pro_cmd_req.alpha.length > 0) &&
      (old_buf->cmd.receive_data_pro_cmd_req.alpha.alphabet_given) &&
      (old_buf->cmd.receive_data_pro_cmd_req.alpha.alphabet != NULL)) 
    {
      ((AEESTKReceiveDataReq*)(*ppOutBuf))->sAlpha.pwszText = (AECHAR*) ptr;
      ptr += ((sizeof(AECHAR)*int32touint32(old_buf->cmd.receive_data_pro_cmd_req.alpha.length+3))/4)*4;
    }
    /* Icon */
    if((old_buf->cmd.receive_data_pro_cmd_req.icon.present) &&
        (old_buf->cmd.receive_data_pro_cmd_req.icon.data != NULL)) 
    {
      ((AEESTKReceiveDataReq*)(*ppOutBuf))->sIcon.pData = ptr;
      ptr += ((old_buf->cmd.receive_data_pro_cmd_req.icon.size+3)/4)*4;
    }
    break;

  case GSTK_GET_CH_STATUS_REQ:
    nSize = ((sizeof(AEESTKGetChStatusReq)+3)/4)*4;
    *ppOutBuf =  MALLOC(nSize);
    if (*ppOutBuf == NULL) {
      return ENOMEMORY;
    }
    (void)MEMSET(*ppOutBuf, 0x00, nSize);

    break;

  
  default:
    MSG_HIGH("Unknown command id 0x%x, Allocate header size", old_buf->hdr_cmd.command_id, 0, 0);
    nSize = ((sizeof(AEESTKProactiveCmdHeader)+3)/4)*4;
    *ppOutBuf =  MALLOC(nSize);
    if (*ppOutBuf == NULL) {
      return ENOMEMORY;
    }
    (void)MEMSET(*ppOutBuf, 0x00, nSize);
    break;
  }

  return uint32toint32(nSize);
} /* OEMSTK_AllocateProactiveCmdBuffer */

static AEECMNumberPlan OEMSTK_GSTKNPIToAEENPI(gstk_npi_enum_type npi)
{
  switch(npi)
  {
  case GSTK_ISDN_TELEPHONY_NPI: return AEECM_NPI_ISDN;
  case GSTK_DATA_NPI:           return AEECM_NPI_DATA;
  case GSTK_TELEX_NPI:          return AEECM_NPI_TELEX;
  case GSTK_PRIVATE_NPI:        return AEECM_NPI_NATIONAL;
  
  case GSTK_EXT_RESERVED_NPI:   return AEECM_NPI_RES_EXT;
  case GSTK_RFU_NPI:            return AEECM_NPI_RFU;
  case GSTK_UNKNOWN_NPI:        return AEECM_NPI_UNKNOWN;
  default:                      return AEECM_NPI_UNKNOWN;
  }
}
static AEECMNumberType OEMSTK_GSTKTONToAEETON(gstk_ton_enum_type ton)
{
  switch(ton)
  {
  case GSTK_INTERNATIONAL_NUM:    return AEECM_TON_INTERNATIONAL;
  case GSTK_NATIONAL_NUM:         return AEECM_TON_NATIONAL;
  case GSTK_NETWORK_SPECIFIC_NUM: return AEECM_TON_NETWORK_SPECIFIC;
  case GSTK_RFU_TON:              return AEECM_TON_RFU;
  case GSTK_UNKNOWN_TON:          return AEECM_TON_UNKNOWN;
  default:                        return AEECM_TON_UNKNOWN;
  }
}

static void OEMSTK_ConvertFromGSMDefault
(
  const uint8   * input_data,
  uint32          len,
  uint16        * asc
)
{
  uint32  i, j;
  if(!input_data)
  {
    MSG_ERROR("input_data NULL ptr",0,0,0);
    return;
  }
  if(!asc)
  {
    MSG_ERROR("asc NULL ptr",0,0,0);
    return;
  }

  for( i = 0; i < len; i ++ )
  {
    j = input_data[i];
    asc[i] = gsmdefault_to_oemstk_table[j];
  }

} /* OEMSTK_ConvertFromGSMDefault() */

static void OEMSTK_GSTKStringToAEEString(gstk_string_type *pIn, 
                                         AEESTKString *pOut)
{
  unsigned int i;
  if((pIn == NULL) ||(pOut == NULL))
    return;
  
  if((pIn->length > 0) && (pIn->text != NULL))
  {
    if (gstk_util_decode_dcs_type(FALSE, pIn->dcs) == GSTK_DCS_7_BIT) {
      /* Need to go through 7 bit decoding */
      (void) gstk_util_unpack_7_bit_chars_ext (
            pIn->text,
            pIn->length, 
            (uint16*)pOut->pwszText );
    }
    else {
      for(i = 0;  i < pIn->length; i++)
      {
        if(0xFF == pIn->text[i])
        {
          pIn->text[i] = 0;
          break;
        }
      }
      OEMSTK_ConvertFromGSMDefault(
        pIn->text, int32touint32(i-1), pOut->pwszText);
      /* May not be used */
      pOut->nLength = int32touint32(i-1);
    }
  }
  else
  {
    FREEIF(pOut->pwszText);
    pOut->pwszText = NULL;
  }
}

static void OEMSTK_GSTKStringGSMDefaultToAEEString(gstk_string_type *pIn, 
                                                   AEESTKString *pOut)
{
  int   i              = 0;
  int32 orig_alpha_len = 0;

  if((pIn == NULL) ||(pOut == NULL))
    return;

  if((pIn->length > 0) && (pIn->text != NULL))
  {
    if (pIn->dcs == 0x08)
    {
      /* UCS2 */
      orig_alpha_len = pIn->length - 2; /* substract the null terminated byte and the first byte */
      if (pOut->pwszText == NULL)
        return;
      switch(pIn->text[0]) {
      case 0x80:
        pOut->nLength = gstk_decode_h80_usc2_alphabet(orig_alpha_len, &pIn->text[1], pOut->pwszText);
        MSG_MED("============ Raw Data", 0, 0, 0);
        for (i = 0; i< orig_alpha_len; i++) {
          MSG_MED("orig_cmd_data[%d]: %d", i, pIn->text[i], 0);
        }
        MSG_MED("============ Decoded alphabet", 0, 0, 0);
        for (i = 0; i< orig_alpha_len; i++) {
          MSG_MED("alpha[%d]: 0x%x, %c", i, pOut->pwszText[i], pOut->pwszText[i]);
        }
        break;
      case 0x81:
        pOut->nLength = gstk_decode_h81_usc2_alphabet(orig_alpha_len, &pIn->text[1], pOut->pwszText);
        MSG_MED("============ Raw Data", 0, 0, 0);
        for (i = 0; i< orig_alpha_len; i++) {
          MSG_MED("orig_cmd_data[%d]: %d", i, pIn->text[i], 0);
        }
        MSG_MED("============ Decoded alphabet", 0, 0, 0);
        for (i = 0; i< orig_alpha_len; i++) {
          MSG_MED("alpha[%d]: 0x%x, %c", i, pOut->pwszText[i], pOut->pwszText[i]);
        }
        break;
      case 0x82:
        pOut->nLength = gstk_decode_h82_usc2_alphabet(orig_alpha_len, &pIn->text[1], pOut->pwszText);
        MSG_MED("============ Raw Data", 0, 0, 0);
        for (i = 0; i< orig_alpha_len; i++) {
          MSG_MED("orig_cmd_data[%d]: %d", i, pIn->text[i], 0);
        }
        MSG_MED("============ Decoded alphabet", 0, 0, 0);
        for (i = 0; i< orig_alpha_len; i++) {
          MSG_MED("alpha[%d]: 0x%x, %c", i, pOut->pwszText[i], pOut->pwszText[i]);
        }
        break;
      default:
        break;
      }
    }
    else {
      for(i = 0;  i < pIn->length; i++)
      {
        if(0xFF == pIn->text[i])
        {
          pIn->text[i] = 0;
          /* increment by 1 to reflect the new length,
            which include the "null" byte */
          ++i;
          break;
        }
      }
      OEMSTK_ConvertFromGSMDefault(
        pIn->text, int32touint32(i-1), pOut->pwszText);
      pOut->nLength = int32touint32(i-1);
    }
  }
  else
  {
    pOut->pwszText = NULL;
    pOut->nLength = 0;
  }
}

static int32 OEMSTK_GSTKItemGSMDefaultToAEEString(gstk_item_object_type *pIn, 
                                               AEESTKString *pOut)
{
  int i;
  int32 orig_alpha_len = 0;
  if(pIn == NULL)
    return 0;

  if((pIn->length > 0) && (pIn->item_text != NULL))
  {
    
    orig_alpha_len = pIn->length - 2; /* substract the null terminated byte and the first byte */
    switch(pIn->item_text[0]) {
    case 0x80:
      /* UCS2 */
      if (pOut == NULL)
      {
        /* Extract length only */
        return gstk_decode_h80_usc2_alphabet(orig_alpha_len, &pIn->item_text[1], NULL);
      }
      /* Extract data */
      if (pOut->pwszText == NULL)
        return 0;
      orig_alpha_len = gstk_decode_h80_usc2_alphabet(orig_alpha_len, &pIn->item_text[1], pOut->pwszText);
      MSG_MED("============ Raw Data", 0, 0, 0);
      for (i = 0; i< orig_alpha_len; i++) {
        MSG_MED("orig_cmd_data[%d]: %d", i, pIn->item_text[i], 0);
      }
      MSG_MED("============ Decoded alphabet", 0, 0, 0);
      for (i = 0; i< orig_alpha_len; i++) {
        MSG_MED("alpha[%d]: 0x%x, %c", i, pOut->pwszText[i], pOut->pwszText[i]);
      }
      break;
    case 0x81:
      if (pOut == NULL)
      {
        /* Extract length only */
        return gstk_decode_h81_usc2_alphabet(orig_alpha_len, &pIn->item_text[1], NULL);
      }
      /* Extract data */
      if (pOut->pwszText == NULL)
        return 0;
      orig_alpha_len = gstk_decode_h81_usc2_alphabet(orig_alpha_len, &pIn->item_text[1], pOut->pwszText);
      MSG_MED("============ Raw Data", 0, 0, 0);
      for (i = 0; i< orig_alpha_len; i++) {
        MSG_MED("orig_cmd_data[%d]: %d", i, pIn->item_text[i], 0);
      }
      MSG_MED("============ Decoded alphabet", 0, 0, 0);
      for (i = 0; i< orig_alpha_len; i++) {
        MSG_MED("alpha[%d]: 0x%x, %c", i, pOut->pwszText[i], pOut->pwszText[i]);
      }
      break;
    case 0x82:
      if (pOut == NULL)
      {
        /* Extract length only */
        return gstk_decode_h82_usc2_alphabet(orig_alpha_len, &pIn->item_text[1], NULL);
      }
      /* Extract data */
      if (pOut->pwszText == NULL)
        return  0;
      orig_alpha_len = gstk_decode_h82_usc2_alphabet(orig_alpha_len, &pIn->item_text[1], pOut->pwszText);
      MSG_MED("============ Raw Data", 0, 0, 0);
      for (i = 0; i< orig_alpha_len; i++) {
        MSG_MED("orig_cmd_data[%d]: %d", i, pIn->item_text[i], 0);
      }
      MSG_MED("============ Decoded alphabet", 0, 0, 0);
      for (i = 0; i< orig_alpha_len; i++) {
        MSG_MED("alpha[%d]: 0x%x, %c", i, pOut->pwszText[i], pOut->pwszText[i]);
      }
      break;
    default:
      for(i = 0;  i < pIn->length; i++)
      {
        if(0xFF == pIn->item_text[i])
        {
          /* Do not set that byte to 0 because this function is being reused by
             calculation of length...*/
          /* increment by 1 to reflect the new length,
            which include the "null" byte */
          ++i;
          break;
        }
      }
      if (pOut == NULL)
      {
        /* Extract length only */
        return i;
      }
      /* Extract data */
      if (pOut->pwszText == NULL)
        return 0;
      OEMSTK_ConvertFromGSMDefault(
        pIn->item_text, int32touint32(i-1), pOut->pwszText);
      orig_alpha_len = i;
      break;
    }
  }
  else
  {
    if(pOut)
    {
      pOut->pwszText = NULL;
    }
  }
  return orig_alpha_len;
}

static void OEMSTK_GSTKIconToAEEIcon(gstk_icon_type *pIn,
                                     AEESTKIcon *pOut)
{
  if((pIn == NULL) ||(pOut == NULL))
    return;

  pOut->nHeight = pIn->height;
  pOut->bPresent = pIn->present;
  pOut->nQualifier = pIn->qualifier;
  pOut->nSize = pIn->size;
  pOut->nWidth = pIn->width;
  if(pIn->present && (pIn->data != NULL))
  {
    (void)MEMCPY(pOut->pData, pIn->data, pIn->size);
  }
  else
  {
    pOut->pData = NULL;
  }
}

static void OEMSTK_GSTKAddressToAEEAddress(gstk_address_type *pIn,
                                           AEESTKAddress *pOut)
{
  if((pIn == NULL) ||(pOut == NULL))
    return;
  
  pOut->nLength = pIn->length;
  pOut->NPI = OEMSTK_GSTKNPIToAEENPI(pIn->NPI);
  pOut->TON = OEMSTK_GSTKTONToAEETON(pIn->TON);
  if((pIn->length > 0) && (pIn->address != NULL)) /*lint !e774 */
  {
    (void)MEMCPY(pOut->address, pIn->address, pIn->length);
  }
}

static void OEMSTK_GSTKDurationToAEEDuration(gstk_duration_type *pIn,
                                             AEESTKDuration *pOut)
{
  if((pIn == NULL) ||(pOut == NULL))
    return;
  pOut->bPresent = pIn->present;
  pOut->nLength = pIn->length;
  pOut->nUnits = pIn->units;
}

#ifdef FEATURE_CAT_REL6 
static void OEMSTK_AEEDurationToGSTKDuration(AEESTKDuration *pIn,
                                             gstk_duration_type *pOut)
{
  if((pIn == NULL) ||(pOut == NULL))
    return;
  pOut->present = pIn->bPresent;
  pOut->length  = pIn->nLength;
  pOut->units   = pIn->nUnits;
}
#endif /* #ifdef FEATURE_CAT_REL6  */

static void OEMSTK_GSTKAlphabetToAEEString(gstk_alphabet_type *pIn,
                                           AEESTKString *pOut)
{
  if((pIn == NULL) ||(pOut == NULL))
    return;
  pOut->nLength = int32touint32(pIn->length);
  if((pIn->length > 0) && (pIn->alphabet != NULL))
  {
    (void)MEMCPY(pOut->pwszText, pIn->alphabet, sizeof(AECHAR)*int32touint32(pIn->length));
  }
}

typedef enum {
  OEMSTK_BREW = 0x00,
  OEMSTK_DMSS = 0x01,
  OEMSTK_MAX  = 0xFFFF /*lint -esym(749,OEMSTK_MAX) */
}
OEMSTK_ModuleType;

typedef struct {
  gstk_csd_bearer_speed_enum_type  DmssSpeed;
  AEESTKBearerCSDSpeedType         BREWSpeed; 
}
OEMSTK_BearerCSDSpeedTableType;

 
static const OEMSTK_BearerCSDSpeedTableType OEMSTK_BearerCSDSpeedTable[] = {
  {GSTK_CSD_BEARER_SPEED_AUTO,            AEESTK_CSD_BEARER_SPEED_AUTO},    
  {GSTK_CSD_BEARER_SPEED_300_V21,         AEESTK_CSD_BEARER_SPEED_300_V21},
  {GSTK_CSD_BEARER_SPEED_1200_V22,        AEESTK_CSD_BEARER_SPEED_1200_V22},
  {GSTK_CSD_BEARER_SPEED_1200_75_V23,     AEESTK_CSD_BEARER_SPEED_1200_75_V23},
  {GSTK_CSD_BEARER_SPEED_2400_V22,        AEESTK_CSD_BEARER_SPEED_2400_V22},
  {GSTK_CSD_BEARER_SPEED_2400_V26,        AEESTK_CSD_BEARER_SPEED_2400_V26},
  {GSTK_CSD_BEARER_SPEED_4800_V32,        AEESTK_CSD_BEARER_SPEED_4800_V32},
  {GSTK_CSD_BEARER_SPEED_9600_V32,        AEESTK_CSD_BEARER_SPEED_9600_V32},
  {GSTK_CSD_BEARER_SPEED_9600_V34,        AEESTK_CSD_BEARER_SPEED_9600_V34},
  {GSTK_CSD_BEARER_SPEED_14400_V34,       AEESTK_CSD_BEARER_SPEED_14400_V34},
  {GSTK_CSD_BEARER_SPEED_19200_V34,       AEESTK_CSD_BEARER_SPEED_19200_V34},
  {GSTK_CSD_BEARER_SPEED_28800_V34,       AEESTK_CSD_BEARER_SPEED_28800_V34},
  {GSTK_CSD_BEARER_SPEED_33600_V34,       AEESTK_CSD_BEARER_SPEED_33600_V34},
  {GSTK_CSD_BEARER_SPEED_1200_V120,       AEESTK_CSD_BEARER_SPEED_1200_V120},
  {GSTK_CSD_BEARER_SPEED_2400_V120,       AEESTK_CSD_BEARER_SPEED_2400_V120},
  {GSTK_CSD_BEARER_SPEED_4800_V120,       AEESTK_CSD_BEARER_SPEED_4800_V120},
  {GSTK_CSD_BEARER_SPEED_9600_V120,       AEESTK_CSD_BEARER_SPEED_9600_V120},
  {GSTK_CSD_BEARER_SPEED_14400_V120,      AEESTK_CSD_BEARER_SPEED_14400_V120},
  {GSTK_CSD_BEARER_SPEED_19200_V120,      AEESTK_CSD_BEARER_SPEED_19200_V120},
  {GSTK_CSD_BEARER_SPEED_28800_V120,      AEESTK_CSD_BEARER_SPEED_28800_V120},
  {GSTK_CSD_BEARER_SPEED_38400_V120,      AEESTK_CSD_BEARER_SPEED_38400_V120},
  {GSTK_CSD_BEARER_SPEED_48000_V120,      AEESTK_CSD_BEARER_SPEED_48000_V120},
  {GSTK_CSD_BEARER_SPEED_56000_V120,      AEESTK_CSD_BEARER_SPEED_56000_V120},
  {GSTK_CSD_BEARER_SPEED_300_V110,        AEESTK_CSD_BEARER_SPEED_300_V110},
  {GSTK_CSD_BEARER_SPEED_1200_V110,       AEESTK_CSD_BEARER_SPEED_1200_V110},
  {GSTK_CSD_BEARER_SPEED_2400_V110_X31,   AEESTK_CSD_BEARER_SPEED_2400_V110_X31},
  {GSTK_CSD_BEARER_SPEED_4800_V110_X31,   AEESTK_CSD_BEARER_SPEED_4800_V110_X31},
  {GSTK_CSD_BEARER_SPEED_9600_V110_X31,   AEESTK_CSD_BEARER_SPEED_9600_V110_X31},
  {GSTK_CSD_BEARER_SPEED_14400_V110_X31,  AEESTK_CSD_BEARER_SPEED_14400_V110_X31},
  {GSTK_CSD_BEARER_SPEED_19200_V110_X31,  AEESTK_CSD_BEARER_SPEED_19200_V110_X31},
  {GSTK_CSD_BEARER_SPEED_28800_V110_X31,  AEESTK_CSD_BEARER_SPEED_28800_V110_X31},
  {GSTK_CSD_BEARER_SPEED_38400_V110_X31,  AEESTK_CSD_BEARER_SPEED_38400_V110_X31},
  {GSTK_CSD_BEARER_SPEED_48000_V110_X31,  AEESTK_CSD_BEARER_SPEED_48000_V110_X31},
  {GSTK_CSD_BEARER_SPEED_56000_V110_X31,  AEESTK_CSD_BEARER_SPEED_56000_V110_X31},
  {GSTK_CSD_BEARER_SPEED_64000_V110_X31,  AEESTK_CSD_BEARER_SPEED_64000_V110_X31},
  {GSTK_CSD_BEARER_SPEED_56000_BIT_TRANSPARENT, AEESTK_CSD_BEARER_SPEED_56000_BIT_TRANSPARENT},
  {GSTK_CSD_BEARER_SPEED_64000_BIT_TRANSPARENT, AEESTK_CSD_BEARER_SPEED_64000_BIT_TRANSPARENT},
  {GSTK_CSD_BEARER_SPEED_32000_PIAFS32K,  AEESTK_CSD_BEARER_SPEED_32000_PIAFS32K},
  {GSTK_CSD_BEARER_SPEED_64000_PIAFS32K,  AEESTK_CSD_BEARER_SPEED_64000_PIAFS32K},
  {GSTK_CSD_BEARER_SPEED_28800_MULTIMEDIA, AEESTK_CSD_BEARER_SPEED_28800_MULTIMEDIA},
  {GSTK_CSD_BEARER_SPEED_32000_MULTIMEDIA, AEESTK_CSD_BEARER_SPEED_32000_MULTIMEDIA},
  {GSTK_CSD_BEARER_SPEED_33600_MULTIMEDIA, AEESTK_CSD_BEARER_SPEED_33600_MULTIMEDIA},
  {GSTK_CSD_BEARER_SPEED_56000_MULTIMEDIA, AEESTK_CSD_BEARER_SPEED_56000_MULTIMEDIA},
  {GSTK_CSD_BEARER_SPEED_64000_MULTIMEDIA, AEESTK_CSD_BEARER_SPEED_64000_MULTIMEDIA},
  {GSTK_CSD_BEARER_SPEED_MAX,              AEESTK_CSD_BEARER_SPEED_MAX}
};

static void OEMSTK_BearerCSDSpeedTableLookUp(OEMSTK_ModuleType                Module,
                                             gstk_csd_bearer_speed_enum_type *pDmssSpeed,
                                             AEESTKBearerCSDSpeedType        *pBREWSpeed)
{
  uint32 i;
  boolean bFound = FALSE;
  if ((pDmssSpeed == NULL) || (pBREWSpeed == NULL)) {
    return;
  }
  for (i = 0; i < ARR_SIZE(OEMSTK_BearerCSDSpeedTable); i++) {
    if (Module == OEMSTK_BREW) {
      if (OEMSTK_BearerCSDSpeedTable[i].DmssSpeed == *pDmssSpeed) {
        *pBREWSpeed = OEMSTK_BearerCSDSpeedTable[i].BREWSpeed;
        bFound = TRUE;
        break;
      }
    }
    else if (Module == OEMSTK_DMSS) {
      if (OEMSTK_BearerCSDSpeedTable[i].BREWSpeed == *pBREWSpeed) {
        *pDmssSpeed = OEMSTK_BearerCSDSpeedTable[i].DmssSpeed;
        bFound = TRUE;
        break;
      }
    }
  }
  if (!bFound) {
    if (Module == OEMSTK_BREW) {
      *pBREWSpeed = AEESTK_CSD_BEARER_SPEED_MAX;
    }
    else if (Module == OEMSTK_DMSS) {
      *pDmssSpeed = GSTK_CSD_BEARER_SPEED_AUTO;
    }
    MSG_ERROR("Can not match Bearer Speed, module: 0x%x, 0x%x, 0x%x ", 
      Module, *pBREWSpeed, *pDmssSpeed);    
  }
} /* OEMSTK_BearerCSDSpeedTableLookUp */
                              

typedef struct {
  gstk_csd_bearer_name_enum_type  DmssName;
  AEESTKBearerCSDNameType         BREWName; 
}
OEMSTK_BearerCSDNameTableType;

 
static const OEMSTK_BearerCSDNameTableType OEMSTK_BearerCSDNameTable[] = {
  {GSTK_CSD_BEARER_NAME_DATA_CIRCUIT_ASYNCH_UDI_3_1KHZ, AEESTK_CSD_BEARER_NAME_DATA_CIRCUIT_ASYNCH_UDI_3_1KHZ},
  {GSTK_CSD_BEARER_NAME_DATA_CIRCUIT_SYNCH_UDI_3_1KHZ,  AEESTK_CSD_BEARER_NAME_DATA_CIRCUIT_SYNCH_UDI_3_1KHZ},
  {GSTK_CSD_BEARER_NAME_PAD_ACCESS_ASYNC_UDI,           AEESTK_CSD_BEARER_NAME_PAD_ACCESS_ASYNC_UDI},
  {GSTK_CSD_BEARER_NAME_PACKET_ACCESS_SYNCH_UDI,        AEESTK_CSD_BEARER_NAME_PACKET_ACCESS_SYNCH_UDI},
  {GSTK_CSD_BEARER_NAME_DATA_CIRCUIT_ASYNCH_RDI,        AEESTK_CSD_BEARER_NAME_DATA_CIRCUIT_ASYNCH_RDI},
  {GSTK_CSD_BEARER_NAME_DATA_CIRCUIT_SYNCH_RDI,         AEESTK_CSD_BEARER_NAME_DATA_CIRCUIT_SYNCH_RDI},
  {GSTK_CSD_BEARER_NAME_PAD_ACCESS_ASYNC_RDI,           AEESTK_CSD_BEARER_NAME_PAD_ACCESS_ASYNC_RDI},
  {GSTK_CSD_BEARER_NAME_PACKET_ACCESS_SYNC_RDI,         AEESTK_CSD_BEARER_NAME_PACKET_ACCESS_SYNC_RDI},
  {GSTK_CSD_BEARER_NAME_MAX,                            AEESTK_CSD_BEARER_NAME_MAX} 
};

static void OEMSTK_BearerCSDNameTableLookUp (OEMSTK_ModuleType                Module,
                                             gstk_csd_bearer_name_enum_type *pDmssName,
                                             AEESTKBearerCSDNameType        *pBREWName)
{
  uint32 i;
  boolean bFound = FALSE;
  if ((pDmssName == NULL) || (pBREWName == NULL)) {
    return;
  }
  for (i = 0; i < ARR_SIZE(OEMSTK_BearerCSDNameTable); i++) {
    if (Module == OEMSTK_BREW) {
      if (OEMSTK_BearerCSDNameTable[i].DmssName == *pDmssName) {
        *pBREWName = OEMSTK_BearerCSDNameTable[i].BREWName;
        bFound = TRUE;
        break;
      }
    }
    else if (Module == OEMSTK_DMSS) {
      if (OEMSTK_BearerCSDNameTable[i].BREWName == *pBREWName) {
        *pDmssName = OEMSTK_BearerCSDNameTable[i].DmssName;
        bFound = TRUE;
        break;
      }
    }
  }
  if (!bFound) {
    if (Module == OEMSTK_BREW) {
      *pBREWName = AEESTK_CSD_BEARER_NAME_MAX;
    }
    else if (Module == OEMSTK_DMSS) {
      *pDmssName = GSTK_CSD_BEARER_NAME_DATA_CIRCUIT_ASYNCH_UDI_3_1KHZ;
    }
    MSG_ERROR("Can not match Bearer Name, module: 0x%x, 0x%x, 0x%x ", 
      Module, *pBREWName, *pDmssName);
  }
} /* OEMSTK_BearerCSDNameTableLookUp */



typedef struct {
  gstk_csd_bearer_conn_element_enum_type  DmssConnectionElement;
  AEESTKBearerCSDConnectionElementType    BREWConnectionElement; 
}
OEMSTK_BearerCSDConnectionElementTableType;

 
static const OEMSTK_BearerCSDConnectionElementTableType OEMSTK_BearerCSDConnectionElementTable[] = {
  {GSTK_CSD_BEARER_CE_TRANSPARENT,               AEESTK_CSD_BEARER_CE_TRANSPARENT},
  {GSTK_CSD_BEARER_CE_NON_TRANSPARENT,           AEESTK_CSD_BEARER_CE_NON_TRANSPARENT},
  {GSTK_CSD_BEARER_CE_BOTH_TRANSPARENT_PREF,     AEESTK_CSD_BEARER_CE_BOTH_TRANSPARENT_PREF},
  {GSTK_CSD_BEARER_CE_BOTH_NON_TRANSPARENT_PREF, AEESTK_CSD_BEARER_CE_BOTH_NON_TRANSPARENT_PREF},
  {GSTK_CSD_BEARER_CE_MAX,                       AEESTK_CSD_BEARER_CE_MAX}
};

static void OEMSTK_BearerCSDConnectionElementTableLookUp (OEMSTK_ModuleType                Module,
                                             gstk_csd_bearer_conn_element_enum_type      *pDmssCE,
                                             AEESTKBearerCSDConnectionElementType        *pBREWCE)
{
  uint32 i;
  boolean bFound = FALSE;
  if ((pDmssCE == NULL) || (pBREWCE == NULL)) {
    return;
  }
  for (i = 0; i < ARR_SIZE(OEMSTK_BearerCSDConnectionElementTable); i++) {
    if (Module == OEMSTK_BREW) {
      if (OEMSTK_BearerCSDConnectionElementTable[i].DmssConnectionElement == *pDmssCE) {
        *pBREWCE = OEMSTK_BearerCSDConnectionElementTable[i].BREWConnectionElement;
        bFound = TRUE;
        break;
      }
    }
    else if (Module == OEMSTK_DMSS) {
      if (OEMSTK_BearerCSDConnectionElementTable[i].BREWConnectionElement == *pBREWCE) {
        *pDmssCE = OEMSTK_BearerCSDConnectionElementTable[i].DmssConnectionElement;
        bFound = TRUE;
        break;
      }
    }
  }
  if (!bFound) {
    if (Module == OEMSTK_BREW) {
      *pBREWCE = AEESTK_CSD_BEARER_CE_MAX;
    }
    else if (Module == OEMSTK_DMSS) {
      *pDmssCE = GSTK_CSD_BEARER_CE_TRANSPARENT;
    }
    MSG_ERROR("Can not match Bearer CE, module: 0x%x, 0x%x, 0x%x ", Module, *pBREWCE, *pDmssCE);
  }
} /* OEMSTK_BearerCSDConnectionElementTableLookUp */




typedef struct {
  gstk_packet_data_protocol_enum_type  DmssProtocol;
  AEESTKPacketDataProtocolType         BREWProtocol; 
}
OEMSTK_BearerGPRSPktDataProtocolTableType;

                    
static const OEMSTK_BearerGPRSPktDataProtocolTableType OEMSTK_BearerGPRSPktDataProtocolTable[] = {
  {GSTK_PKT_DATA_IP,               AEESTK_PKT_DATA_IP},
  {GSTK_PKT_DATA_PROTOCOL_MAX,     AEESTK_PKT_DATA_PROTOCOL_MAX}
};

static void OEMSTK_BearerGPRSPktDataProtocolTableLookUp (OEMSTK_ModuleType         Module,
                                             gstk_packet_data_protocol_enum_type  *pDmssProtocol,
                                             AEESTKPacketDataProtocolType         *pBREWProtocol)
{
  uint32 i;
  boolean bFound = FALSE;
  if ((pDmssProtocol == NULL) || (pBREWProtocol == NULL)) {
    return;
  }
  for (i = 0; i < ARR_SIZE(OEMSTK_BearerGPRSPktDataProtocolTable); i++) {
    if (Module == OEMSTK_BREW) {
      if (OEMSTK_BearerGPRSPktDataProtocolTable[i].DmssProtocol == *pDmssProtocol) {
        *pBREWProtocol = OEMSTK_BearerGPRSPktDataProtocolTable[i].BREWProtocol;
        bFound = TRUE;
        break;
      }
    }
    else if (Module == OEMSTK_DMSS) {
      if (OEMSTK_BearerGPRSPktDataProtocolTable[i].BREWProtocol == *pBREWProtocol) {
        *pDmssProtocol = OEMSTK_BearerGPRSPktDataProtocolTable[i].DmssProtocol;
        bFound = TRUE;
        break;
      }
    }
  }
  if (!bFound) {
    if (Module == OEMSTK_BREW) {
      *pBREWProtocol = AEESTK_PKT_DATA_PROTOCOL_MAX;
    }
    else if (Module == OEMSTK_DMSS) {
      *pDmssProtocol = GSTK_PKT_DATA_PROTOCOL_MAX;
    }
    MSG_ERROR("Can not match Bearer protocol, module: 0x%x, 0x%x, 0x%x ", 
      Module, *pBREWProtocol, *pDmssProtocol);  
  }
} /* OEMSTK_BearerGPRSPktDataProtocolTableLookUp */


typedef struct {
  gstk_ch_status_additional_info_enum_type  DmssChStatusAddlInfo;
  AEESTKChStAdditionalInfoType              BREWChStatusAddlInfo; 
}
OEMSTK_ChStatusAddlInfoTableType;
                    
static const OEMSTK_ChStatusAddlInfoTableType OEMSTK_ChStatusAddlInfoTable[] = {
  {GSTK_CH_STATUS_NO_FURTHER_INFO,               AEESTK_CH_STATUS_NO_FURTHER_INFO},
  {GSTK_CH_STATUS_LINK_DROPPED,                  AEESTK_CH_STATUS_LINK_DROPPED},
  {GSTK_CH_STATUS_ADDITIONAL_INFO_NOT_USED,      AEESTK_CH_STATUS_ADDITIONAL_INFO_NOT_USED},
  {GSTK_CH_STATUS_ADDITIONAL_INFO_RFU,           AEESTK_CH_STATUS_ADDITIONAL_INFO_RFU},
  {GSTK_CH_STATUS_ADDITIONAL_INFO_MAX,           AEESTK_CH_STATUS_ADDITIONAL_INFO_MAX}
};

static void OEMSTK_ChStatusAddlInfoTableLookUp (OEMSTK_ModuleType                       Module,
                                             gstk_ch_status_additional_info_enum_type  *pDmssChStatusAddlInfo,
                                             AEESTKChStAdditionalInfoType              *pBREWChStatusAddlInfo)
{
  uint32 i;
  boolean bFound = FALSE;
  if ((pDmssChStatusAddlInfo == NULL) || (pBREWChStatusAddlInfo == NULL)) {
    return;
  }
  for (i = 0; i < ARR_SIZE(OEMSTK_ChStatusAddlInfoTable); i++) {
    if (Module == OEMSTK_BREW) {
      if (OEMSTK_ChStatusAddlInfoTable[i].DmssChStatusAddlInfo == *pDmssChStatusAddlInfo) {
        *pBREWChStatusAddlInfo = OEMSTK_ChStatusAddlInfoTable[i].BREWChStatusAddlInfo;
        bFound = TRUE;
        break;
      }
    }
    else if (Module == OEMSTK_DMSS) {
      if (OEMSTK_ChStatusAddlInfoTable[i].BREWChStatusAddlInfo == *pBREWChStatusAddlInfo) {
        *pDmssChStatusAddlInfo = OEMSTK_ChStatusAddlInfoTable[i].DmssChStatusAddlInfo;
        bFound = TRUE;
        break;
      }
    }
  }
  if (!bFound) {
    if (Module == OEMSTK_BREW) {
      *pBREWChStatusAddlInfo = AEESTK_CH_STATUS_ADDITIONAL_INFO_MAX;
    }
    else if (Module == OEMSTK_DMSS) {
      *pDmssChStatusAddlInfo = GSTK_CH_STATUS_ADDITIONAL_INFO_RFU;
    }
    MSG_ERROR("Can not match ch status additional, module: 0x%x, 0x%x, 0x%x ", 
      Module, *pBREWChStatusAddlInfo, *pDmssChStatusAddlInfo);
  }
} /* OEMSTK_ChStatusAddlInfoTableLookUp */


static void OEMSTK_GSTKToAEESTKBearerDescription(gstk_bearer_description_type *pIn,
                                                 AEESTKBearerDescriptionType *pOut)
{
  if((pIn == NULL) ||(pOut == NULL))
    return;

  switch (pIn->bearer_type) {
    case GSTK_CSD:
      pOut->BearerType = AEESTK_BEARER_CSD;
      OEMSTK_BearerCSDSpeedTableLookUp(OEMSTK_BREW, 
                                       &pIn->bearer_params.csd_bearer.speed,
                                       &pOut->BearerParams.BearerCSD.Speed);
      OEMSTK_BearerCSDNameTableLookUp(OEMSTK_BREW, 
                                      &pIn->bearer_params.csd_bearer.name,
                                      &pOut->BearerParams.BearerCSD.Name);
      OEMSTK_BearerCSDConnectionElementTableLookUp(OEMSTK_BREW, 
                                      &pIn->bearer_params.csd_bearer.connection_element,
                                      &pOut->BearerParams.BearerCSD.ConnectionElement);
      break;
    case GSTK_GPRS:
      pOut->BearerType = AEESTK_BEARER_GPRS;
      pOut->BearerParams.BearerGPRS.nDelayCls = pIn->bearer_params.gprs_bearer.delay_cls;
      pOut->BearerParams.BearerGPRS.nMeanThroughput= pIn->bearer_params.gprs_bearer.mean_throughput;
      pOut->BearerParams.BearerGPRS.nPeakThroughput = pIn->bearer_params.gprs_bearer.peak_throughput;
      pOut->BearerParams.BearerGPRS.nPrecedenceCls = pIn->bearer_params.gprs_bearer.precedence_cls;
      pOut->BearerParams.BearerGPRS.nReliabilityCls = pIn->bearer_params.gprs_bearer.reliability_cls;
      OEMSTK_BearerGPRSPktDataProtocolTableLookUp(OEMSTK_BREW, 
                                                  &pIn->bearer_params.gprs_bearer.pkt_data_protocol,
                                                  &pOut->BearerParams.BearerGPRS.PktDataProtocol);
      break;
    case GSTK_BEARER_DEFAULT:
      pOut->BearerType = AEESTK_BEARER_DEFAULT;
      break;
    default:
      pOut->BearerType = AEESTK_MAX_BEARER_CODE_TYPE;
      break;
  }
} /* OEMSTK_GSTKToAEESTKBearerDescription */

static void OEMSTK_GSTKToAEESTKPacketDataAddress(gstk_pkt_data_address_type  *pIn,
                                                 AEESTKPacketDataAddressType *pOut)
{
  if((pIn == NULL) ||(pOut == NULL))
    return;

  switch (pIn->address_type) {
    case GSTK_PKT_DATA_NO_ADDRESS_GIVEN:
      pOut->PktAddressType = AEESTK_PKT_DATA_NO_ADDRESS_GIVEN;
      break;
    case GSTK_PKT_DATA_DYNAMIC_ADDRESS:
      pOut->PktAddressType = AEESTK_PKT_DATA_DYNAMIC_ADDRESS;
      break;
    case GSTK_PKT_DATA_IPV4_ADDRESS:
      pOut->PktAddressType = AEESTK_PKT_DATA_IPV4_ADDRESS;
      break;
    case GSTK_PKT_DATA_IPV6_ADDRESS:
      pOut->PktAddressType = AEESTK_PKT_DATA_IPV6_ADDRESS;
      break;
    case GSTK_PKT_DATA_ADDRESS_MAX:
      pOut->PktAddressType = AEESTK_PKT_DATA_ADDRESS_MAX;
      break;
    default:
      pOut->PktAddressType = AEESTK_PKT_DATA_ADDRESS_MAX;
      break;
  }

  pOut->nPktAddressLength = pIn->pkt_address.length;
  if((pIn->pkt_address.length > 0) && (pIn->pkt_address.address != NULL))
  {
    (void)MEMCPY(pOut->pPktAddress, pIn->pkt_address.address, int32touint32(pIn->pkt_address.length));
  }   
} /* OEMSTK_GSTKToAEESTKPacketDataAddress */

typedef struct {
  gstk_pkt_data_transport_level_enum_type     DmssTransportLevel;
  AEESTKPacketDataTransportLevelCategoryType  BREWTransportLevel; 
}
OEMSTK_PacketDataTransportLevelTableType;
       
static const OEMSTK_PacketDataTransportLevelTableType OEMSTK_PacketDataTransportLevelTable[] = {
  {GSTK_PKT_DATA_NOT_PRESENT,            AEESTK_PKT_DATA_NOT_PRESENT},
  {GSTK_PKT_DATA_UDP,                    AEESTK_PKT_DATA_UDP},
  {GSTK_PKT_DATA_TCP,                    AEESTK_PKT_DATA_TCP},
  {GSTK_PKT_DATA_TRANSPORT_LEVEL_MAX,    AEESTK_PKT_DATA_TRANSPORT_LEVEL_MAX}
};

static void OEMSTK_PacketDataTransportLevelTableLookUp (OEMSTK_ModuleType                            Module,
                                                        gstk_pkt_data_transport_level_enum_type     *pDmssTransportLvl,
                                                        AEESTKPacketDataTransportLevelCategoryType  *pBREWTransportLvl)
{
  uint32 i;
  boolean bFound = FALSE;
  if ((pDmssTransportLvl == NULL) || (pBREWTransportLvl == NULL)) {
    return;
  }
  for (i = 0; i < ARR_SIZE(OEMSTK_PacketDataTransportLevelTable); i++) {
    if (Module == OEMSTK_BREW) {
      if (OEMSTK_PacketDataTransportLevelTable[i].DmssTransportLevel == *pDmssTransportLvl) {
        *pBREWTransportLvl = OEMSTK_PacketDataTransportLevelTable[i].BREWTransportLevel;
        bFound = TRUE;
        break;
      }
    }
    else if (Module == OEMSTK_DMSS) {
      if (OEMSTK_PacketDataTransportLevelTable[i].BREWTransportLevel == *pBREWTransportLvl) {
        *pDmssTransportLvl = OEMSTK_PacketDataTransportLevelTable[i].DmssTransportLevel;
        bFound = TRUE;
        break;
      }
    }
  }
  if (!bFound) {
    MSG_ERROR("Can not match transport protocol, module: 0x%x, 0x%x, 0x%x ", 
               Module, *pBREWTransportLvl, *pDmssTransportLvl);
    if (Module == OEMSTK_BREW) {
      *pBREWTransportLvl = AEESTK_PKT_DATA_TRANSPORT_LEVEL_MAX;
    }
    else if (Module == OEMSTK_DMSS) {
      *pDmssTransportLvl = GSTK_PKT_DATA_NOT_PRESENT;
    }
  }
} /* OEMSTK_PacketDataTransportLevelTableLookUp */


static void OEMSTK_GSTKToAEESTKDataTransport(gstk_pkt_data_transport_level_type  *pIn,
                                             AEESTKPacketDataTransportLevelType  *pOut)
{
  if((pIn == NULL) ||(pOut == NULL))
    return;

  OEMSTK_PacketDataTransportLevelTableLookUp(OEMSTK_BREW, 
                                      &pIn->transport_protocol,
                                      &pOut->TransportProtocol);
  pOut->dwPortNumber = pIn->port_number;  
} /* OEMSTK_GSTKToAEESTKDataTransport */


static void OEMSTK_GSTKDataToAEEDataBuffer(gstk_ch_data_type  *pIn,
                                           AEESTKDataBufferType  *pOut)
{
  if((pIn == NULL) ||(pOut == NULL))
    return;

  pOut->nLength = pIn->data_len;
  if((pIn->data_len > 0) && (pIn->data != NULL))
  {
    (void)MEMCPY(pOut->pData, pIn->data, int32touint32(pIn->data_len));
  }
  
} /* OEMSTK_GSTKDataToAEEDataBuffer */

/*===========================================================================

FUNCTION: OEMSTK_DCSSPC_GSTKSTRINGTOAEESTRING

DESCRIPTION
  This is converts gstk string type to AECHAR based on GSM default 7 bit
  encoding, regular 8 bit encoding or UCS2 encoding

DEPENDENCIES
  none

RETURN VALUE
  none
SIDE EFFECTS
===========================================================================*/
static void OEMSTK_DCSSpc_GSTKStringToAEEString(gstk_string_type *pIn, 
                                                AEESTKString *pOut)
{
  int i = 0;
  unsigned int j = 0;

  if((!pIn) || (!pOut))
    return;

  if((pIn->length == 0) || !(pIn->text))
    return;

  if(pOut->pwszText == NULL)
    return;

  if((gstk_util_decode_dcs_type(FALSE, pIn->dcs) == GSTK_DCS_UCS2_8_BIT )||
     (gstk_util_decode_dcs_type(FALSE, pIn->dcs) == GSTK_DCS_UCS2_7_BIT )
    )
  {
    /* Input length include null byte */
    for(i=1;i<(pIn->length-1);i=i+2)
    {
      pOut->pwszText[j] = (pIn->text[i-1] << 8) | pIn->text[i];
      j=j+1;
    }
    if(pIn->length%2 == 0) {
      MSG_ERROR("Odd # bytes for UCS2, 0x%x", pIn->length, 0, 0);
      pOut->pwszText[j] = (pIn->text[pIn->length-1]) << 8;
      j++;
    }
    pOut->nLength = j;
  }
  else
  {
    OEMSTK_GSTKStringToAEEString(pIn,pOut);
  }
  return;
}

/*===========================================================================

FUNCTION: OEMSTK_DCSSPC_USSDSTRINGTOAEESTRING

DESCRIPTION
  This is converts ussd string type to AECHAR based on GSM default 7 bit
  encoding, regular 8 bit encoding or UCS2 encoding

DEPENDENCIES
  none

RETURN VALUE
  none
SIDE EFFECTS
===========================================================================*/
static void OEMSTK_DCSSpc_USSDStringToAEEString(gstk_ussd_string_type *pIn, 
                                                AEESTKString *pOut)
{
  if((!pIn) || (!pOut))
    return;

  if((pIn->length == 0) || !(pIn->text))
      return;
  if((gstk_util_decode_dcs_type(TRUE, pIn->dcs) == GSTK_DCS_UCS2_8_BIT )||
     (gstk_util_decode_dcs_type(TRUE, pIn->dcs) == GSTK_DCS_UCS2_7_BIT )
    )
  {
    (void)MEMCPY(pOut->pwszText,pIn->text,pIn->length);
    pOut->nLength = pIn->length;
  }
  else
  {
    (void) STRTOWSTR((char*) pIn->text,
                      pOut->pwszText,
                      (sizeof(AECHAR)*(pIn->length+1)));
    pOut->nLength = int32touint32(WSTRLEN(pOut->pwszText));
  }
  return;
}
static void* OEMSTK_GSTKCmdToAEEProactiveCmd(OEMSTK                   *po,
                                             gstk_cmd_from_card_type  *old_buf,
                                             uint32                   *pdwSize)
{
  int i;

  void *cmd_buf = NULL;
  /* check memory of buffer pointers */
  if((!po) || (old_buf == NULL)) 
  {
    return NULL;
  }

  if(!pdwSize)
  {
    return NULL;
  }
  *pdwSize = int32touint32(OEMSTK_AllocateProactiveCmdBuffer(old_buf, &cmd_buf));
  if (cmd_buf == NULL) {
    return NULL;
  }
  switch(old_buf->hdr_cmd.command_id)
  {
  case GSTK_DISPLAY_TEXT_REQ:
    {
      AEESTKDisplayTextReq *new_buf = (AEESTKDisplayTextReq*) cmd_buf;
    
      new_buf->cmdHdr.nCommandNumber = old_buf->hdr_cmd.command_number;
      new_buf->cmdHdr.nCommandID = AEESTK_DISPLAY_TEXT_REQ;

      new_buf->bHighPriority = 
        old_buf->cmd.display_text_pro_cmd_req.high_priority;
      
      OEMSTK_GSTKIconToAEEIcon(&old_buf->cmd.display_text_pro_cmd_req.icon,
        &new_buf->icon);

      new_buf->bImmediateRespReq =
        old_buf->cmd.display_text_pro_cmd_req.immediate_response_required;
      
      /* If the DCS is UCS2(16 bit) just do a memcpy. Do not do a WSTRToSTR
         as that would truncate the upper 8 bits
      */

      OEMSTK_DCSSpc_GSTKStringToAEEString(&old_buf->cmd.display_text_pro_cmd_req.text_string,
        &new_buf->textString);

      new_buf->bUserControl =
        old_buf->cmd.display_text_pro_cmd_req.user_control;

#ifdef FEATURE_CAT_REL6 
      OEMSTK_GSTKDurationToAEEDuration(&old_buf->cmd.display_text_pro_cmd_req.duration,
        &new_buf->bDurationOfDisplay);
#endif /* #ifdef FEATURE_CAT_REL6 */
    }
    break;

  case GSTK_GET_INKEY_REQ:
    {
    
      AEESTKGetInkeyReq *new_buf = (AEESTKGetInkeyReq*) cmd_buf;

      new_buf->cmdHdr.nCommandNumber = old_buf->hdr_cmd.command_number;
      new_buf->cmdHdr.nCommandID = AEESTK_GET_INKEY_REQ;

      new_buf->bHelpAvaliable =
        old_buf->cmd.get_inkey_pro_cmd_req.help_available;

      OEMSTK_GSTKIconToAEEIcon(&old_buf->cmd.get_inkey_pro_cmd_req.icon,
        &new_buf->icon);

      new_buf->responseFormat = 
        (AEESTKInputFormatType)old_buf->cmd.get_inkey_pro_cmd_req.response_format;

      /* If the DCS is UCS2(16 bit) just do a memcpy. Do not do a WSTRToSTR
         as that would truncate the upper 8 bits
      */

      OEMSTK_DCSSpc_GSTKStringToAEEString(&old_buf->cmd.get_inkey_pro_cmd_req.text_string,
        &new_buf->textString);

#ifdef FEATURE_CAT_REL6 
      /* Variable timeout - length of time to wait for user input */
      OEMSTK_GSTKDurationToAEEDuration(&old_buf->cmd.get_inkey_pro_cmd_req.variable_timeout,
        &new_buf->variableTimeout);
#endif /* #ifdef FEATURE_CAT_REL6  */
    }
     break;
     
  case GSTK_GET_INPUT_REQ:
    {
      AEESTKGetInputReq *new_buf = (AEESTKGetInputReq*) cmd_buf;

      new_buf->cmdHdr.nCommandNumber = old_buf->hdr_cmd.command_number;
      new_buf->cmdHdr.nCommandID = AEESTK_GET_INPUT_REQ;
          
      /* If the DCS is UCS2(16 bit) just do a memcpy. Do not do a WSTRToSTR
         as that would truncate the upper 8 bits
      */
      OEMSTK_DCSSpc_GSTKStringToAEEString(&old_buf->cmd.get_input_pro_cmd_req.default_text,
        &new_buf->defaultText);
      
      new_buf->bHelpAvaliable = 
        old_buf->cmd.get_input_pro_cmd_req.help_available;
      
      OEMSTK_GSTKIconToAEEIcon(&old_buf->cmd.get_input_pro_cmd_req.icon,
        &new_buf->icon);
      
      new_buf->nMaxUserInput =
        old_buf->cmd.get_input_pro_cmd_req.maximum_user_input;

      new_buf->nMinUserInput =
        old_buf->cmd.get_input_pro_cmd_req.minimum_user_input;

      new_buf->responseFormat =
        (AEESTKInputFormatType)old_buf->cmd.get_input_pro_cmd_req.response_format;

      new_buf->bShowUserInput = 
        old_buf->cmd.get_input_pro_cmd_req.show_user_input;

      /* If the DCS is UCS2(16 bit) just do a memcpy. Do not do a WSTRToSTR
         as that would truncate the upper 8 bits
      */
      OEMSTK_DCSSpc_GSTKStringToAEEString(&old_buf->cmd.get_input_pro_cmd_req.text_string,
        &new_buf->textString);
    }
    break;

  case GSTK_LAUNCH_BROWSER_REQ:
    {
      AEESTKLaunchBrowserReq *new_buf = (AEESTKLaunchBrowserReq*) cmd_buf;

      new_buf->cmdHdr.nCommandNumber = old_buf->hdr_cmd.command_number;
      new_buf->cmdHdr.nCommandID = AEESTK_LAUNCH_BROWSER_REQ;

      new_buf->bearer.nLength =
        old_buf->cmd.launch_browser_pro_cmd_req.bearer.length;
      /* copy bearer list */
      if((old_buf->cmd.launch_browser_pro_cmd_req.bearer.length > 0) &&
        (old_buf->cmd.launch_browser_pro_cmd_req.bearer.bearer_list != NULL)) 
      {
        (void)MEMCPY(new_buf->bearer.pBearerList,
          old_buf->cmd.launch_browser_pro_cmd_req.bearer.bearer_list,
          old_buf->cmd.launch_browser_pro_cmd_req.bearer.length);
      }

      new_buf->nBrowserID = 
        old_buf->cmd.launch_browser_pro_cmd_req.browser_id;

      OEMSTK_DCSSpc_GSTKStringToAEEString(&old_buf->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string,
        &new_buf->gatewayProxyTextString);

      new_buf->nLaunchMode = 
        (AEESTKLaunchModeType)old_buf->cmd.launch_browser_pro_cmd_req.launch_mode;

      new_buf->nNumProvisioningFiles =
        old_buf->cmd.launch_browser_pro_cmd_req.num_provisioning_files;

      
      for (i=0; i< GSTK_MAX_PROVISIONING_FILE_REF; i++) 
      {
        new_buf->provisioningFileRef[i].nLength =
          old_buf->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].length;
        new_buf->provisioningFileRef[i].nNumFiles=
          old_buf->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].num_of_files;

        if((old_buf->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].length > 0) &&
          (old_buf->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].file_paths != NULL)) 
        {
          (void)MEMCPY(new_buf->provisioningFileRef[i].pFilePaths,
            old_buf->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].file_paths,
            old_buf->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].length);
        }
      }

      new_buf->url.nLength =
        old_buf->cmd.launch_browser_pro_cmd_req.url.length;

      /* allocate and copy url */
      if((old_buf->cmd.launch_browser_pro_cmd_req.url.length > 0) &&
        (old_buf->cmd.launch_browser_pro_cmd_req.url.url != NULL)) 
      {
          (void)MEMCPY(new_buf->url.pURL,
            old_buf->cmd.launch_browser_pro_cmd_req.url.url,
            old_buf->cmd.launch_browser_pro_cmd_req.url.length);
      }
      
      OEMSTK_GSTKStringGSMDefaultToAEEString(&old_buf->cmd.launch_browser_pro_cmd_req.user_confirm_alpha,
        &new_buf->UserConfirmAlpha);

      OEMSTK_GSTKIconToAEEIcon(&old_buf->cmd.launch_browser_pro_cmd_req.user_confirm_icon,
        &new_buf->UserConfirmIcon);

    }
    break;

   case GSTK_PLAY_TONE_REQ:
     {
      AEESTKPlayToneReq *new_buf = (AEESTKPlayToneReq*) cmd_buf;

      new_buf->cmdHdr.nCommandNumber = old_buf->hdr_cmd.command_number;
      new_buf->cmdHdr.nCommandID = AEESTK_PLAY_TONE_REQ;

      OEMSTK_GSTKStringGSMDefaultToAEEString(&old_buf->cmd.play_tone_pro_cmd_req.alpha,
        &new_buf->alpha);

      OEMSTK_GSTKDurationToAEEDuration(&old_buf->cmd.play_tone_pro_cmd_req.duration,
        &new_buf->duration);

      OEMSTK_GSTKIconToAEEIcon(&old_buf->cmd.play_tone_pro_cmd_req.icon,
        &new_buf->icon);

      new_buf->nTone = (AEESTKToneType)old_buf->cmd.play_tone_pro_cmd_req.tone;

     }
     break;

   case GSTK_SELECT_ITEM_REQ:
     {
       AEESTKSelectItemReq *new_buf = (AEESTKSelectItemReq*) cmd_buf;

       new_buf->cmdHdr.nCommandNumber = old_buf->hdr_cmd.command_number;
       new_buf->cmdHdr.nCommandID = AEESTK_SELECT_ITEM_REQ;
     
       new_buf->nDefaultItem = 
         old_buf->cmd.select_item_pro_cmd_req.default_item;
       
       new_buf->bHelpAvaliable = 
         old_buf->cmd.select_item_pro_cmd_req.help_available;

       OEMSTK_GSTKIconToAEEIcon(&old_buf->cmd.select_item_pro_cmd_req.icon,
         &new_buf->icon);
 
       new_buf->iconIdList.nNumItems =
         old_buf->cmd.select_item_pro_cmd_req.icon_id_list.num_items;
       new_buf->iconIdList.bDisplayIconOnly = 
         old_buf->cmd.select_item_pro_cmd_req.icon_id_list.display_icon_only;

       /* allocate and copy icon id list */
       if(old_buf->cmd.select_item_pro_cmd_req.icon_id_list.num_items > 0)
       {
         for(i = 0; i< old_buf->cmd.select_item_pro_cmd_req.icon_id_list.num_items; i++) 
         {
           if(old_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i] != NULL)
           {
             OEMSTK_GSTKIconToAEEIcon(old_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i],
               new_buf->iconIdList.pIconList[i]);
           }
         }
       }
       
       new_buf->nNumItems = old_buf->cmd.select_item_pro_cmd_req.num_items;

       for (i=0; i< GSTK_MAX_MENU_ITEMS; i++) 
       {
         new_buf->items[i].nLength = 
             old_buf->cmd.select_item_pro_cmd_req.items[i].item_id;
         if((old_buf->cmd.select_item_pro_cmd_req.items[i].length > 0) &&
            (old_buf->cmd.select_item_pro_cmd_req.items[i].item_text != NULL)) 
         {
           (void)OEMSTK_GSTKItemGSMDefaultToAEEString(&old_buf->cmd.select_item_pro_cmd_req.items[i],
                                                &new_buf->items[i]);
         }
         else
         {
            new_buf->items[i].pwszText = NULL;
         }
       }
       new_buf->nextActionIndicator.nNumItems = 
         old_buf->cmd.select_item_pro_cmd_req.next_action_indicator.num_items;

       /* allocate and copy nextActionIndicator */
       if((old_buf->cmd.select_item_pro_cmd_req.next_action_indicator.num_items > 0) &&
           (old_buf->cmd.select_item_pro_cmd_req.next_action_indicator.next_action_list != NULL)) 
       {
         (void)MEMCPY(new_buf->nextActionIndicator.pNextActionList,
           old_buf->cmd.select_item_pro_cmd_req.next_action_indicator.next_action_list,
           old_buf->cmd.select_item_pro_cmd_req.next_action_indicator.num_items);
       } 
       
       new_buf->presentation = (AEESTKMenuPresentType)old_buf->cmd.select_item_pro_cmd_req.presentation;

       new_buf->bSoftkeySelection = 
         old_buf->cmd.select_item_pro_cmd_req.softkey_selection;

       OEMSTK_GSTKStringGSMDefaultToAEEString(&old_buf->cmd.select_item_pro_cmd_req.title, &new_buf->title);
     
      }
      break;

   case GSTK_SEND_SMS_REQ:
     {
      AEESTKSendSMSReq *new_buf = (AEESTKSendSMSReq*) cmd_buf;

      new_buf->cmdHdr.nCommandNumber = old_buf->hdr_cmd.command_number;
      new_buf->cmdHdr.nCommandID = AEESTK_SEND_SMS_REQ;
    
      OEMSTK_GSTKAddressToAEEAddress(&old_buf->cmd.send_sms_pro_cmd_req.address,
        &new_buf->address);

      OEMSTK_GSTKStringGSMDefaultToAEEString(&old_buf->cmd.send_sms_pro_cmd_req.alpha,
        &new_buf->alpha);
       
      OEMSTK_GSTKIconToAEEIcon(&old_buf->cmd.send_sms_pro_cmd_req.icon,
        &new_buf->icon);

      new_buf->bIsCDMASMS = old_buf->cmd.send_sms_pro_cmd_req.is_cdma_sms;

      new_buf->bPackingRequired = 
        old_buf->cmd.send_sms_pro_cmd_req.packing_required;

      new_buf->smsTPDU.nLength =
        old_buf->cmd.send_sms_pro_cmd_req.sms_tpdu.length;

      /* allocate and copy sms tpdu */
      if((old_buf->cmd.send_sms_pro_cmd_req.sms_tpdu.length > 0) &&
          (old_buf->cmd.send_sms_pro_cmd_req.sms_tpdu.tpdu != NULL)) 
      {
        (void)MEMCPY(new_buf->smsTPDU.pTPDU,
          old_buf->cmd.send_sms_pro_cmd_req.sms_tpdu.tpdu,
          old_buf->cmd.send_sms_pro_cmd_req.sms_tpdu.length);
      }
     }
     break;

   case GSTK_SEND_SS_REQ:
     {
      AEESTKSendSSReq *new_buf = (AEESTKSendSSReq*) cmd_buf;

      new_buf->cmdHdr.nCommandNumber = old_buf->hdr_cmd.command_number;
      new_buf->cmdHdr.nCommandID = AEESTK_SEND_SS_REQ;

      OEMSTK_GSTKStringGSMDefaultToAEEString(&old_buf->cmd.send_ss_pro_cmd_req.alpha,
        &new_buf->alpha);
      OEMSTK_GSTKIconToAEEIcon(&old_buf->cmd.send_ss_pro_cmd_req.icon,
        &new_buf->icon);
      OEMSTK_GSTKAddressToAEEAddress(&old_buf->cmd.send_ss_pro_cmd_req.ss_string,
        &new_buf->ssString);

     }
     break;

   case GSTK_SEND_USSD_REQ:
     {
      AEESTKSendUSSDReq *new_buf = (AEESTKSendUSSDReq*) cmd_buf;

      new_buf->cmdHdr.nCommandNumber = old_buf->hdr_cmd.command_number;
      new_buf->cmdHdr.nCommandID = AEESTK_SEND_USSD_REQ;
     
      OEMSTK_GSTKStringGSMDefaultToAEEString(&old_buf->cmd.send_ussd_pro_cmd_req.alpha,
        &new_buf->alpha);
      OEMSTK_GSTKIconToAEEIcon(&old_buf->cmd.send_ussd_pro_cmd_req.icon,
        &new_buf->icon);
      
      if((old_buf->cmd.send_ussd_pro_cmd_req.ussd_string.length > 0) && 
        (old_buf->cmd.send_ussd_pro_cmd_req.ussd_string.text != NULL))
      {
        new_buf->nDCSFromSIM = old_buf->cmd.send_ussd_pro_cmd_req.ussd_string.orig_dcs_from_sim;
        OEMSTK_DCSSpc_USSDStringToAEEString(&old_buf->cmd.send_ussd_pro_cmd_req.ussd_string,&new_buf->ussdString);
      }
     }
      break;

   case GSTK_SETUP_CALL_REQ:
     {
      AEESTKSetupCallReq *new_buf = (AEESTKSetupCallReq*) cmd_buf;

      new_buf->cmdHdr.nCommandNumber = old_buf->hdr_cmd.command_number;
      new_buf->cmdHdr.nCommandID = AEESTK_SETUP_CALL_REQ;

      OEMSTK_GSTKAddressToAEEAddress(&old_buf->cmd.setup_call_pro_cmd_req.address,
        &new_buf->address);
      new_buf->callSetupRequirement = 
        (AEESTKCallSetupReqType)old_buf->cmd.setup_call_pro_cmd_req.call_setup_requirement;
      new_buf->capabilityConfig.nLength = 
        old_buf->cmd.setup_call_pro_cmd_req.capability_config.length;
      (void)MEMCPY(new_buf->capabilityConfig.ccp,
        old_buf->cmd.setup_call_pro_cmd_req.capability_config.ccp,
        AEESTK_MAX_CCP_LEN);
      new_buf->destinationSubAddress.nLength = 
        old_buf->cmd.setup_call_pro_cmd_req.destination_subaddress.length;
      (void)MEMCPY(new_buf->destinationSubAddress.subaddress,
        old_buf->cmd.setup_call_pro_cmd_req.destination_subaddress.subaddress,
        AEESTK_MAX_SUBADDR_LEN);
      new_buf->redial.bNecessary =
        old_buf->cmd.setup_call_pro_cmd_req.redial.necessary;
      OEMSTK_GSTKDurationToAEEDuration(&old_buf->cmd.setup_call_pro_cmd_req.redial.timeout,
        &new_buf->redial.timeout);
      OEMSTK_GSTKStringGSMDefaultToAEEString(&old_buf->cmd.setup_call_pro_cmd_req.setup_call_display_alpha,
        &new_buf->setupCallDisplayAlpha);
      OEMSTK_GSTKIconToAEEIcon(&old_buf->cmd.setup_call_pro_cmd_req.setup_call_display_icon,
        &new_buf->setupCallDisplayIcon);
      OEMSTK_GSTKStringGSMDefaultToAEEString(&old_buf->cmd.setup_call_pro_cmd_req.user_cnf_alpha,
        &new_buf->userCnfAlpha);
      OEMSTK_GSTKIconToAEEIcon(&old_buf->cmd.setup_call_pro_cmd_req.user_cnf_icon,
        &new_buf->userCnfIcon);
     }

     break;

   case GSTK_SETUP_IDLE_TEXT_REQ:
     {
      AEESTKSetupIdleModeTextReq *new_buf = (AEESTKSetupIdleModeTextReq*) cmd_buf;

      new_buf->cmdHdr.nCommandNumber = old_buf->hdr_cmd.command_number;
      new_buf->cmdHdr.nCommandID = AEESTK_UPDATE_IDLE_TEXT_REQ;

      OEMSTK_GSTKIconToAEEIcon(&old_buf->cmd.setup_idle_mode_txt_pro_cmd_req.icon,
        &new_buf->icon);

      /* If the DCS is UCS2(16 bit) just do a memcpy. Do not do a WSTRToSTR
         as that would truncate the upper 8 bits
      */

      OEMSTK_DCSSpc_GSTKStringToAEEString(&old_buf->cmd.setup_idle_mode_txt_pro_cmd_req.text_string,
        &new_buf->string);
     }
     break;

   case GSTK_SETUP_MENU_REQ:
     {
      AEESTKSetupMenuReq *new_buf = (AEESTKSetupMenuReq*) cmd_buf;

      new_buf->cmdHdr.nCommandNumber = old_buf->hdr_cmd.command_number;
      new_buf->cmdHdr.nCommandID = AEESTK_SETUP_MENU_REQ;

      new_buf->nDefaultItem =
         old_buf->cmd.setup_menu_pro_cmd_req.default_item;

      new_buf->bHelp = old_buf->cmd.setup_menu_pro_cmd_req.help_available;
      new_buf->bSoftkeySelection = old_buf->cmd.setup_menu_pro_cmd_req.softkey_selection;
      OEMSTK_GSTKIconToAEEIcon(&old_buf->cmd.setup_menu_pro_cmd_req.icon,
        &new_buf->icon);

      new_buf->iconIdList.bDisplayIconOnly = 
        old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.display_icon_only;
      new_buf->iconIdList.nNumItems = 
        old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.num_items;
        /* allocate and copy icon id list */
      if(old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.num_items > 0)
      {
        for(i = 0; i< old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.num_items; i++) 
        {
          if(old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i] != NULL) 
          {
            OEMSTK_GSTKIconToAEEIcon(old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i],
              new_buf->iconIdList.pIconList[i]);
          }
        }
      }

      new_buf->nItems = old_buf->cmd.setup_menu_pro_cmd_req.num_items;

      /* allocate and copy items */
      for (i=0; i< GSTK_MAX_MENU_ITEMS; i++) 
      {
        new_buf->items[i].nLength = 
          old_buf->cmd.setup_menu_pro_cmd_req.items[i].item_id;
        if((old_buf->cmd.setup_menu_pro_cmd_req.items[i].length > 0) &&
            (old_buf->cmd.setup_menu_pro_cmd_req.items[i].item_text != NULL)) 
        {
          (void)OEMSTK_GSTKItemGSMDefaultToAEEString(&old_buf->cmd.setup_menu_pro_cmd_req.items[i],
                                               &new_buf->items[i]);
        }
        else
        {
          new_buf->items[i].pwszText = NULL;
        }
      }

      new_buf->nextActionIndicator.nNumItems =
        old_buf->cmd.setup_menu_pro_cmd_req.next_action_indicator.num_items;

      /* allocate and copy nextActionIndicator */
      if((old_buf->cmd.setup_menu_pro_cmd_req.next_action_indicator.num_items > 0) &&
          (old_buf->cmd.setup_menu_pro_cmd_req.next_action_indicator.next_action_list != NULL)) 
      {
        (void)MEMCPY(new_buf->nextActionIndicator.pNextActionList,
          old_buf->cmd.setup_menu_pro_cmd_req.next_action_indicator.next_action_list,
          old_buf->cmd.setup_menu_pro_cmd_req.next_action_indicator.num_items);
      }
       
      OEMSTK_GSTKStringGSMDefaultToAEEString(&old_buf->cmd.setup_menu_pro_cmd_req.title,
        &new_buf->title);
     }
    break;
   case GSTK_SEND_DTMF_REQ:
     {
      AEESTKSendDTMFReq *new_buf = (AEESTKSendDTMFReq*) cmd_buf;

      new_buf->cmdHdr.nCommandNumber = old_buf->hdr_cmd.command_number;
      new_buf->cmdHdr.nCommandID = AEESTK_SEND_DTMF_REQ;

      OEMSTK_GSTKStringGSMDefaultToAEEString(&old_buf->cmd.send_dtmf_pro_cmd_req.alpha, 
                                   &new_buf->alpha);
      OEMSTK_GSTKIconToAEEIcon(&old_buf->cmd.send_dtmf_pro_cmd_req.icon, 
                                   &new_buf->icon);
      new_buf->nLength = old_buf->cmd.send_dtmf_pro_cmd_req.dtmf.length;

      if((old_buf->cmd.send_dtmf_pro_cmd_req.dtmf.length > 0 ) &&
         (old_buf->cmd.send_dtmf_pro_cmd_req.dtmf.dtmf != NULL))
      {
        (void)MEMCPY(new_buf->pDTMFString, old_buf->cmd.send_dtmf_pro_cmd_req.dtmf.dtmf, 
          old_buf->cmd.send_dtmf_pro_cmd_req.dtmf.length);
      }

     }
     break;

   case GSTK_MORE_TIME_REQ:
     {
      AEESTKMoreTimeReq *new_buf = (AEESTKMoreTimeReq*) cmd_buf;

      new_buf->cmdHdr.nCommandNumber = old_buf->hdr_cmd.command_number;
      new_buf->cmdHdr.nCommandID = AEESTK_MORE_TIME_REQ;
     }
    break;

   
   case GSTK_PROVIDE_LANG_INFO_REQ:
     {
       AEESTKProvideLangReq *new_buf = (AEESTKProvideLangReq*) cmd_buf;

       new_buf->cmdHdr.nCommandNumber = old_buf->hdr_cmd.command_number;
       new_buf->cmdHdr.nCommandID = AEESTK_PROVIDE_LANG_INFO_REQ;
     }
    break;
   
    case GSTK_PROVIDE_BATTERY_STATUS_REQ:
#ifdef FEATURE_CAT_REL6 
     {
       AEESTKProvideBatteryStatusReq *new_buf = (AEESTKProvideBatteryStatusReq*) cmd_buf;

       new_buf->cmdHdr.nCommandNumber = old_buf->hdr_cmd.command_number;
       new_buf->cmdHdr.nCommandID = AEESTK_PROVIDE_BATTERY_STATUS_REQ;
     }
#else
     MSG_HIGH(" GSTK_PROVIDE_BATTERY_STATUS_REQ : FEATURE_CAT_REL6  not supported ",0,0,0);
#endif /* #ifdef FEATURE_CAT_REL6  */
    break;
   
   case GSTK_LANG_NOTIFICATION_REQ:
     {
       AEESTKLangInfoReq *new_buf = (AEESTKLangInfoReq*) cmd_buf;

       new_buf->cmdHdr.nCommandNumber = old_buf->hdr_cmd.command_number;
       new_buf->cmdHdr.nCommandID = AEESTK_LANG_NOTIFICATION_REQ;

       new_buf->bSpecLangRequest = old_buf->cmd.lang_notification_pro_cmd_req.specific_lang_notification;

       if(old_buf->cmd.lang_notification_pro_cmd_req.specific_lang_notification)
       {
         new_buf->nLangCode[0] = old_buf->cmd.lang_notification_pro_cmd_req.lang.lang_code[0];
         new_buf->nLangCode[1] = old_buf->cmd.lang_notification_pro_cmd_req.lang.lang_code[1];
       }
     }
    break;

   case GSTK_END_PROACTIVE_CMD_REQ:
     {
       AEESTKEndProactiveCmdReq *new_buf = (AEESTKEndProactiveCmdReq*) cmd_buf;

       new_buf->cmdHdr.nCommandNumber = old_buf->hdr_cmd.command_number;
       new_buf->cmdHdr.nCommandID = AEESTK_END_PROACTIVE_CMD_SES;
       new_buf->bEndFromCard = old_buf->cmd.end_pro_cmd_req.end_from_card;
     }
    break;

    case GSTK_OPEN_CH_REQ:
     {
      AEESTKOpenChReq *new_buf = (AEESTKOpenChReq*) cmd_buf;

      new_buf->cmdHdr.nCommandNumber = old_buf->hdr_cmd.command_number;
      new_buf->cmdHdr.nCommandID = AEESTK_OPEN_CH_REQ;

      new_buf->bOnDemandLinkEst = old_buf->cmd.open_ch_pro_cmd_req.on_demand_link_est;

      /* Copy GSTK alphabet type to AEEString */
      OEMSTK_GSTKAlphabetToAEEString(&old_buf->cmd.open_ch_pro_cmd_req.alpha,
        &new_buf->sAlpha);

      OEMSTK_GSTKIconToAEEIcon(&old_buf->cmd.open_ch_pro_cmd_req.icon,
        &new_buf->sIcon);

      OEMSTK_GSTKToAEESTKBearerDescription(&old_buf->cmd.open_ch_pro_cmd_req.bearer,
        &new_buf->sBearer);

      new_buf->nBufferSize = old_buf->cmd.open_ch_pro_cmd_req.buffer_size;

      OEMSTK_GSTKAlphabetToAEEString(&old_buf->cmd.open_ch_pro_cmd_req.access_point_name,
        &new_buf->sAccessPointName);
     
      OEMSTK_GSTKToAEESTKPacketDataAddress(&old_buf->cmd.open_ch_pro_cmd_req.local_address,
        &new_buf->sLocalAddress);
      
      OEMSTK_GSTKToAEESTKDataTransport(&old_buf->cmd.open_ch_pro_cmd_req.transport_level,
        &new_buf->sTransportLevel);

      OEMSTK_GSTKToAEESTKPacketDataAddress(&old_buf->cmd.open_ch_pro_cmd_req.data_dest_address,
        &new_buf->sDataDestAddress);

      OEMSTK_GSTKAlphabetToAEEString(&old_buf->cmd.open_ch_pro_cmd_req.text_string_user_login,
        &new_buf->sUserLogin);

      OEMSTK_GSTKAlphabetToAEEString(&old_buf->cmd.open_ch_pro_cmd_req.text_string_user_passwd,
        &new_buf->sUserPassword);
     }
     break;

   case GSTK_CLOSE_CH_REQ:
     {
      AEESTKCloseChReq *new_buf = (AEESTKCloseChReq*) cmd_buf;

      new_buf->cmdHdr.nCommandNumber = old_buf->hdr_cmd.command_number;
      new_buf->cmdHdr.nCommandID = AEESTK_CLOSE_CH_REQ;

      new_buf->nChID = old_buf->cmd.close_ch_pro_cmd_req.ch_id;
      
      /* Copy GSTK alphabet type to AEEString */
      OEMSTK_GSTKAlphabetToAEEString(&old_buf->cmd.close_ch_pro_cmd_req.alpha,
        &new_buf->sAlpha);

      OEMSTK_GSTKIconToAEEIcon(&old_buf->cmd.close_ch_pro_cmd_req.icon,
        &new_buf->sIcon);
     }
     break;

   case GSTK_SEND_DATA_REQ:
     {
      AEESTKSendDataReq *new_buf = (AEESTKSendDataReq*) cmd_buf;

      new_buf->cmdHdr.nCommandNumber = old_buf->hdr_cmd.command_number;
      new_buf->cmdHdr.nCommandID = AEESTK_SEND_DATA_REQ;

      new_buf->nChID = old_buf->cmd.send_data_pro_cmd_req.ch_id;
      new_buf->bSendDataImmediately = old_buf->cmd.send_data_pro_cmd_req.send_data_immediately;

      OEMSTK_GSTKDataToAEEDataBuffer(&old_buf->cmd.send_data_pro_cmd_req.ch_data,
        &new_buf->sChData);

      /* Copy GSTK alphabet type to AEEString */
      OEMSTK_GSTKAlphabetToAEEString(&old_buf->cmd.send_data_pro_cmd_req.alpha,
        &new_buf->sAlpha);

      OEMSTK_GSTKIconToAEEIcon(&old_buf->cmd.send_data_pro_cmd_req.icon,
        &new_buf->sIcon);
     }
     break;

   case GSTK_RECEIVE_DATA_REQ:
     {
      AEESTKReceiveDataReq *new_buf = (AEESTKReceiveDataReq*) cmd_buf;

      new_buf->cmdHdr.nCommandNumber = old_buf->hdr_cmd.command_number;
      new_buf->cmdHdr.nCommandID = AEESTK_RECEIVE_DATA_REQ;

      new_buf->nChID = old_buf->cmd.receive_data_pro_cmd_req.ch_id;
      new_buf->nChDataLength = old_buf->cmd.receive_data_pro_cmd_req.ch_data_length;

      /* Copy GSTK alphabet type to AEEString */
      OEMSTK_GSTKAlphabetToAEEString(&old_buf->cmd.receive_data_pro_cmd_req.alpha,
        &new_buf->sAlpha);

      OEMSTK_GSTKIconToAEEIcon(&old_buf->cmd.receive_data_pro_cmd_req.icon,
        &new_buf->sIcon);
     }
     break;

   case GSTK_GET_CH_STATUS_REQ:
     {
      AEESTKGetChStatusReq *new_buf = (AEESTKGetChStatusReq*) cmd_buf;

      new_buf->cmdHdr.nCommandNumber = old_buf->hdr_cmd.command_number;
      new_buf->cmdHdr.nCommandID = AEESTK_GET_CH_STATUS_REQ;
     }
     break;


   default:
     {
       AEESTKProactiveCmdHeader *new_buf = (AEESTKProactiveCmdHeader*) cmd_buf;
       MSG_ERROR("GOT SOMETHING ELSE",0,0,0);
       new_buf->nCommandNumber = old_buf->hdr_cmd.command_number;
       new_buf->nCommandID = AEESTK_PROACTIVE_CMD_SES_WITHOUT_TR;
       break;
     }

  }
  return cmd_buf;
} /* OEMSTK_GSTKCmdToAEEProactiveCmd */

#ifdef FEATURE_CAT_REL6 
/*===========================================================================

FUNCTION: OEMSTK_AEESTKBatteryStToGSTKBatterySt

DESCRIPTION
  Converts AEE Battery Status to GSTK Battery status

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
===========================================================================*/

static void OEMSTK_AEESTKBatteryStToGSTKBatterySt
(
  AEESTKBatteryStatusType *pIn,
  gstk_battery_state_enum_type *pOut
)
{
    switch(*pIn)
    {
    case AEESTK_BATTERY_LEVEL_0:
      *pOut =  GSTK_BATTERY_ST_VERY_LOW;
      break;
    case AEESTK_BATTERY_LEVEL_1:
      *pOut =   GSTK_BATTERY_ST_LOW;
      break;
    case AEESTK_BATTERY_LEVEL_2:
      *pOut =  GSTK_BATTERY_ST_AVG;
      break;
    case AEESTK_BATTERY_LEVEL_3:
      *pOut =  GSTK_BATTERY_ST_GOOD;
      break;
    case AEESTK_BATTERY_LEVEL_4:
      *pOut = GSTK_BATTERY_ST_FULL;
      break;
    case AEESTK_BATTERY_LEVEL_NA: 
      *pOut = GSTK_BATTERY_ST_NA;
      break;
    case AEESTK_BATTERY_LEVEL_UNKNOWN:
    default:
      *pOut = GSTK_BATTERY_ST_UNKNOWN;
      break;
    }
}
#endif /* #ifdef FEATURE_CAT_REL6 */

static void OEMSTK_AEEStringToGSTKString(
  const AEESTKString *pIn,
  gstk_string_type   *pOut
)
{
  char *pBuf = NULL;

  if ((pIn == NULL) || (pOut == NULL)) {
    return;
  }
  if (pOut->text == NULL) {
    return;
  }

  //copy into an overallocated buffer and then do memcpy.
  // This is required becayse WSTRTOSTR requires one more byte 
  // to copy the null character

  if(pIn->pwszText != NULL)
  {
    pOut->length = int32touint8(WSTRLEN(pIn->pwszText));
    pBuf = (char*) MALLOC(pOut->length + 1);
    if (pBuf == NULL) {
      return;
    }
    (void) WSTRTOSTR(pIn->pwszText, pBuf, (pOut->length + 1));
    (void) MEMCPY(pOut->text, pBuf, pOut->length);
    FREE(pBuf);
  }
}
/*===========================================================================
FUNCTION: OEMSTK_AEEGetInputResponseToGSTKString

DESCRIPTION
   Converts to GSTK string format based on the input response type - UCS2 or
   SMS default.This function is currently used specifically for Get Input 
   Proactive cmd only.

PARAMETERS
  pIn  - Get Input AEESTK Response 
  pOut - GSTK repsonse type.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void OEMSTK_AEEGetInputResponseToGSTKString(
  const AEESTKGetInputResponse *pIn,
  gstk_string_type             *pOut
)
{
  uint32  i = 0;
  if((pIn == NULL) || (pOut == NULL) || 
     (pOut->text == NULL) || 
     (pIn->pString.pwszText == NULL)) 
  {
    MSG_ERROR(" NULL PTR in OEMSTK_AEEGetInputResponseToGSTKString",0,0,0);
    return;
  }
  switch(pIn->dcs)
  {
  case AEESTK_UCS2_ALPHABET:
    for (i = 0; i < pIn->pString.nLength; i++)
    {
      pOut->text[i * 2]     = (uint8)(pIn->pString.pwszText[i] >> 8);
      pOut->text[i * 2 + 1] = (uint8)(pIn->pString.pwszText[i] & 0x00FF);
    }
    pOut->length = uint32touint8(pIn->pString.nLength) * sizeof(AECHAR);
    pOut->dcs    = AEESTK_UCS2_DCS;
    break;

  default:
    OEMSTK_AEEStringToGSTKString(&(pIn->pString),pOut);
    pOut->dcs =  AEESTK_8_BIT_DCS;
    break;
  }
 }
/*===========================================================================

FUNCTION: OEMSTK_DCSSPC_AEESTRINGTOGSTKSTRING

DESCRIPTION
  This is converts AEE String type to gstk string type and populate the
  data coding scheme accordingly.

DEPENDENCIES
  none

RETURN VALUE
  none
SIDE EFFECTS
===========================================================================*/
static void OEMSTK_DCSSpc_AEEStringToGSTKString(
  const AEESTKString    *pIn,
  gstk_string_type      *pOut,
  AEESTKInputFormatType  dcs
)
{
  uint32 i = 0;
  if((pIn == NULL) || (pOut == NULL) || (pOut->text == NULL) || (pIn->pwszText == NULL)) 
  {
    MSG_ERROR(" NULL PTR in OEMSTK_DCSSpc_AEEStringToGSTKString",0,0,0);
    return;
  }
  /* This while loop is required for cards that send fixed length for text string 
     and pad unused bits with 0xFF */
  while(i < pIn->nLength)
  {
    if((pIn->pwszText[i] & 0xFF00) == 0x0000 )
    {
      i=i+1;
      continue;
    }
    break;
  }
  if((i == pIn->nLength) && (AEESTK_UCS2_ALPHABET != dcs))
  {
    OEMSTK_AEEStringToGSTKString(pIn,pOut);
    pOut->dcs =  AEESTK_8_BIT_DCS;
  }
  else if(AEESTK_UCS2_ALPHABET == dcs)
  {
#ifdef FEATURE_GSTK_UCS2_TO_ASCII 
    if(i == pIn->nLength)
    {
      pOut->dcs = AEESTK_8_BIT_DCS;
    }
    else
    {
      pOut->dcs = AEESTK_UCS2_DCS;
    }
#else /* FEATURE_GSTK_UCS2_TO_ASCII */
    pOut->dcs = AEESTK_UCS2_DCS;
#endif /* FEATURE_GSTK_UCS2_TO_ASCII */
    for (i = 0; i < pIn->nLength; i++)
    {
      pOut->text[i * 2]     = (uint8)(pIn->pwszText[i] >> 8);
      pOut->text[i * 2 + 1] = (uint8)(pIn->pwszText[i] & 0x00FF);
    }
    pOut->length = uint32touint8(pIn->nLength) * sizeof(AECHAR);
  }
  else 
  {
    MSG_ERROR("Invalid dcs: 0x%x", dcs, 0, 0);
  }
}

static void OEMSTK_AEEInkeyResponseToGSTKString(AEESTKInkeyResponse *pIn,
                                                gstk_string_type    *pOut)
{
  char *pBuf = NULL;
  if ((pIn == NULL) || (pOut == NULL)) {
    return;
  }
  if (pOut->text == NULL) {
    return;
  }

  //copy into an overallocated buffer and then do memcpy.
  // This is required becayse WSTRTOSTR requires one more byte 
  // to copy the null character

  switch (pIn->ResponseType) {
  case AEESTK_INKEY_INPUT_IS_SINGLE_CHAR:
    pOut->length = int32touint8(WSTRLEN(pIn->pszText));
    pBuf = (char*) MALLOC(pOut->length + 1);
    if (pBuf == NULL) {
      return;
    }
    /* pBuf is not redundant because in WSTRTOSTR we need a
       buffer size of string len + 1 (for the null terminated byte,
       but the pOut->text allocated maybe only of size string len */
    (void) WSTRTOSTR(pIn->pszText, pBuf, (pOut->length + 1));
    (void) MEMCPY(pOut->text, pBuf, pOut->length);
    FREE(pBuf);
    break;
  
  case AEESTK_INKEY_INPUT_IS_YES:
    pOut->length = 1;
    pOut->text[0] = 0x01;
    break;

  case AEESTK_INKEY_INPUT_IS_NO:
    pOut->length = 1;
    pOut->text[0] = 0x00;
    break;
  }
}

static gstk_ton_enum_type OEMSTK_AEECMTONToGSTKTON(AEECMNumberType ton)
{
  switch(ton)
  {
  case AEECM_TON_INTERNATIONAL:    return GSTK_INTERNATIONAL_NUM;
  case AEECM_TON_NATIONAL:         return GSTK_NATIONAL_NUM;
  case AEECM_TON_NETWORK_SPECIFIC: return GSTK_NETWORK_SPECIFIC_NUM;
  case AEECM_TON_RFU:              return GSTK_RFU_TON;
  case AEECM_TON_UNKNOWN:          return GSTK_UNKNOWN_TON;
  default:                         return GSTK_UNKNOWN_TON;
  }
}

static gstk_npi_enum_type OEMSTK_AEECMNPIToGSTKNPI(AEECMNumberPlan npi)
{
  switch(npi)
  {
  case AEECM_NPI_ISDN:          return GSTK_ISDN_TELEPHONY_NPI;
  case AEECM_NPI_DATA:          return GSTK_DATA_NPI;
  case AEECM_NPI_TELEX:         return GSTK_TELEX_NPI;
  case AEECM_NPI_NATIONAL:      return GSTK_PRIVATE_NPI;
  case AEECM_NPI_RES_EXT:       return GSTK_EXT_RESERVED_NPI;
  case AEECM_NPI_RFU:           return GSTK_RFU_NPI;
  case AEECM_NPI_UNKNOWN:       return GSTK_UNKNOWN_NPI;
  default:                      return GSTK_UNKNOWN_NPI;
  }
}
static void OEMSTK_AEEAddressToGSTKAddress(AEESTKAddress *pIn,
                                           gstk_address_type *pOut)
{
  if(!pIn)
  {
      MSG_ERROR("pIn  NULL ptr",0,0,0);
      return;
  }
  if(!pOut)
  {
      MSG_ERROR("pOut NULL ptr",0,0,0);
      return;
  }
  pOut->TON = OEMSTK_AEECMTONToGSTKTON(pIn->TON);
  pOut->NPI = OEMSTK_AEECMNPIToGSTKNPI(pIn->NPI);
  pOut->length = pIn->nLength;
  (void)MEMCPY(pOut->address, pIn->address, GSTK_MAX_ADDRESS_LEN);
}


static void OEMSTK_AEECCReqToGSTKCCReq(AEESTKCCReqAction *pIn,
                                       gstk_cc_req_action_type *pOut)
{
  int i;

  if(!pIn)
  {
      MSG_ERROR("pIn  NULL ptr",0,0,0);
      return;
  }
  if(!pOut)
  {
      MSG_ERROR("pOut NULL ptr",0,0,0);
      return;
  }

  pOut->length = pIn->nLength;
  pOut->call_type = (gstk_call_enum_type) pIn->nCallType;

  switch(pIn->nCallType)
  {
  case AEESTK_VOICE:
    
    OEMSTK_AEEAddressToGSTKAddress(&pIn->addressString.voiceInfo.address,
      &pOut->gstk_address_string.voice_info.address);

    pOut->gstk_address_string.voice_info.subaddress.length = 
      pIn->addressString.voiceInfo.subaddress.nLength;
    
    for(i = 0; i < GSTK_MAX_SUBADDR_LEN; i++)
    {
      pOut->gstk_address_string.voice_info.subaddress.subaddress[i] = 
        pIn->addressString.voiceInfo.subaddress.subaddress[i];
    }

    pOut->gstk_address_string.voice_info.ccp1.length = 
      pIn->addressString.voiceInfo.ccp1.nLength;
    for(i = 0; i < GSTK_MAX_CCP_LEN; i++)
    {
      pOut->gstk_address_string.voice_info.ccp1.ccp[i] = 
        pIn->addressString.voiceInfo.ccp1.ccp[i];
    }

    pOut->gstk_address_string.voice_info.ccp2.length = 
      pIn->addressString.voiceInfo.ccp2.nLength;
    for(i = 0; i < GSTK_MAX_CCP_LEN; i++)
    {
      pOut->gstk_address_string.voice_info.ccp2.ccp[i] = 
        pIn->addressString.voiceInfo.ccp2.ccp[i];
    }

    break;

  case AEESTK_SS:
    OEMSTK_AEEAddressToGSTKAddress(&pIn->addressString.ssString,
        &pOut->gstk_address_string.ss_string);
    break;

  case AEESTK_USSD:
    //pOut->gstk_address_string.ussd_string.dcs = pIn->addressString.ussdString.nDcs;
    //nDcs has been ignored b/c no longer in AEESTKString struct
    pOut->gstk_address_string.ussd_string.length = int32touint8(WSTRLEN(pIn->addressString.ussdString.pwszText));
    if(pIn->addressString.ussdString.pwszText != NULL)
    {
      char *pBuf = MALLOC(pOut->gstk_address_string.ussd_string.length + 1);
      if (pBuf == NULL) {
        return;
      }
      // TODO, may to need to set DCS for Call Control extram param
      // pOut->gstk_address_string.ussd_string.dcs = pIn->addressString.ussdString.dcs;
      (void) WSTRTOSTR(pIn->addressString.ussdString.pwszText,
                pBuf, 
                (pOut->gstk_address_string.ussd_string.length + 1));
      (void) MEMCPY(pOut->gstk_address_string.ussd_string.text, 
          pBuf, pOut->gstk_address_string.ussd_string.length);
      FREE(pBuf);
      
    }
    else
    {
      pOut->gstk_address_string.ussd_string.text = NULL;
    }
    break;
  default:
    break;

  }
  OEMSTK_AEEStringToGSTKString(&pIn->alpha, &pOut->alpha);
  pOut->bc_repeat_ind= (gstk_bc_repeat_ind_enum_type) pIn->bcRepeatInd;
}

gstk_additional_info_ptr_type *OEMSTK_AEEAddnlInfoToGSTKAddnlInfo(
  AEESTKAdditionalInfo *pIn)
{
  gstk_additional_info_ptr_type *pOut = MALLOC(sizeof(gstk_additional_info_ptr_type));

  if(!pOut) 
  {
    MSG_ERROR("Malloc failed", 0, 0, 0);
    return NULL;
  }

  if(!pIn) 
  {
    MSG_ERROR("pIn NULL ptr", 0, 0, 0);
    FREEIF(pOut)
    pOut = NULL;
    return NULL;
  }

  /* GSTK does not like additional info ptr being NULL. Even if there is no additional data return
     a valid ptr with additional info length zero.
     For a failure case, return NULL
  */
  pOut->length = pIn->nLength;
  if(0 == pIn->nLength)
  {
    return pOut;
  }

  /* Allocate memory for gstk additional info ptr and return 
     callee (gstk) needs to free 
  */
  pOut->additional_info_ptr = (uint8*) MALLOC(pIn->nLength * sizeof(uint8));

  if(!(pOut->additional_info_ptr))
  {
    MSG_ERROR("pOut->additional_info_ptr  Malloc failed", 0, 0, 0);
    pOut->length = 0;
    return pOut;
  }

  // Once again, assuming that gstk_additional_info_code_enum_type & AEESTKAddionalInfoCode
  // use the same values - so MEMCPY is fine...otherwise do the mapping 
  // Also assuming that the enums are of same size i.e. 8 bit

  (void)MEMSET(pOut->additional_info_ptr, 0x00, sizeof(uint8)*(pIn->nLength));

  if(!(pIn->pAdditionalInfoData))
  {
    (void)MEMCPY(pOut->additional_info_ptr,
                 pIn->additionalInfo,
                 pIn->nLength);
  }
  else
  {
    (void)MEMCPY(pOut->additional_info_ptr,
                 pIn->pAdditionalInfoData,
                 pIn->nLength);

  }
  return pOut;
}

gstk_terminal_rsp_extra_param_type* OEMSTK_AEEExtraParmToGSTKExtraParm(
                        AEESTKProactiveCmdType commandID,
                        void *pIn,
                        int32 sz)
{
  // First compute the size required
  // Malloc and set the pointers.
  // Copy the data field by field.
  uint32 nSize = sizeof(gstk_terminal_rsp_extra_param_type);
  uint32 nStructSize = nSize;
  uint8 *ptr;
  gstk_terminal_rsp_extra_param_type *pOut;
  int i;

  if(pIn == NULL)
  {
    pOut = (gstk_terminal_rsp_extra_param_type*) MALLOC(nSize);
    if(pOut == NULL)
    {
      MSG_ERROR("Malloc Failed", 0, 0, 0);
    }
    else
    {
      pOut->present = FALSE;
    }
    return pOut;
  }
  //Step 1:
  switch(commandID)
  {
  case AEESTK_GET_INKEY_REQ:
    {
      AEESTKInkeyResponse *pInKeyRsp = (AEESTKInkeyResponse*) pIn;

      switch(pInKeyRsp->ResponseType)
      {
      case AEESTK_INKEY_INPUT_IS_SINGLE_CHAR:
        if(pInKeyRsp->pszText != NULL)
        {
          nSize += int32touint32(WSTRLEN(pInKeyRsp->pszText) * sizeof(AECHAR));
        }
        break;
      case AEESTK_INKEY_INPUT_IS_YES:
      case AEESTK_INKEY_INPUT_IS_NO:
        nSize += 1;
        break;
      }
      break;
    }

  case AEESTK_GET_INPUT_REQ:
    {
      AEESTKString *pString = (AEESTKString*) pIn;
      if(pString->pwszText != NULL)
      {
        nSize += int32touint32(WSTRLEN(pString->pwszText) * sizeof(AECHAR));
      }
    }
    break;

  case AEESTK_SELECT_ITEM_REQ:
    break;

  case AEESTK_SETUP_CALL_REQ:
    {
      AEESTKCallSSResponse *pCallResp = (AEESTKCallSSResponse*) pIn;

      switch(pCallResp->ccReqAction.nCallType)
      {
      case AEESTK_USSD:
        if(pCallResp->ccReqAction.addressString.ussdString.pwszText != NULL)
        {
          nSize += int32touint32(WSTRLEN(pCallResp->ccReqAction.addressString.ussdString.pwszText) * sizeof(AECHAR));
        }
        break;
      default:
        break;
      }

      if(pCallResp->ccReqAction.alpha.pwszText != NULL)
      {
        nSize += int32touint32(WSTRLEN(pCallResp->ccReqAction.alpha.pwszText) * sizeof(AECHAR));
      }

      if(pCallResp->textString2.pwszText != NULL)
      {
        nSize += int32touint32(WSTRLEN(pCallResp->textString2.pwszText) * sizeof(AECHAR));
      }
    }
    break;
  case AEESTK_SEND_SS_REQ:
    {
      AEESTKCallSSResponse *pSSResp = (AEESTKCallSSResponse*) pIn;
      switch(pSSResp->ccReqAction.nCallType)
      {
      case AEESTK_USSD:
        if(pSSResp->ccReqAction.addressString.ussdString.pwszText != NULL)
        {
          nSize += int32touint32(WSTRLEN(pSSResp->ccReqAction.addressString.ussdString.pwszText) * sizeof(AECHAR));
        }
        break;
      default:
        break;
      }

      if(pSSResp->ccReqAction.alpha.pwszText != NULL)
      {
        nSize += int32touint32(WSTRLEN(pSSResp->ccReqAction.alpha.pwszText) * sizeof(AECHAR));
      }

      if(pSSResp->textString2.pwszText != NULL)
      {
        nSize += int32touint32(WSTRLEN(pSSResp->textString2.pwszText)* sizeof(AECHAR));
      }
    }
    break;

  
  case AEESTK_SEND_USSD_REQ:
    {
      AEESTKUSSDResponse *pUSSDResp = (AEESTKUSSDResponse*) pIn;
      if(pUSSDResp->data.pwszText != NULL)
      {
        if(pUSSDResp->nDCSFromNW == AEESTK_UCS2_DCS)
        {
          nSize += pUSSDResp->data.nLength;
        }
        else
        {
          nSize += int32touint32(WSTRLEN(pUSSDResp->data.pwszText + 1) * sizeof(AECHAR));
          pUSSDResp->data.nLength = WSTRLEN(pUSSDResp->data.pwszText) + 1;
        }
      }

      switch(pUSSDResp->ccReqAction.nCallType)
      {
      case AEESTK_USSD:
        if(pUSSDResp->ccReqAction.addressString.ussdString.pwszText != NULL)
        {
          nSize += int32touint32(WSTRLEN(pUSSDResp->ccReqAction.addressString.ussdString.pwszText) * sizeof(AECHAR));
        }
        break;
      default:
        break;
      }

      if(pUSSDResp->ccReqAction.alpha.pwszText != NULL)
      {
        nSize += int32touint32(WSTRLEN(pUSSDResp->ccReqAction.alpha.pwszText) * sizeof(AECHAR));
      }
    }
    break;

  case AEESTK_RECEIVE_DATA_REQ:
    {
      AEESTKReceiveDataResponse  *pReceiveDataResp = (AEESTKReceiveDataResponse*) pIn;

      if(pReceiveDataResp->pData != NULL)
      {
        nSize += int32touint32(pReceiveDataResp->nLength + 1);
      }
    }
    break;


  case AEESTK_PROVIDE_LANG_INFO_REQ:
    break;

  case AEESTK_PROVIDE_BATTERY_STATUS_REQ:
#ifndef FEATURE_CAT_REL6 
   MSG_HIGH(" FEATURE_CAT_REL6  not supported ",0,0,0);
#endif /* #ifdef FEATURE_CAT_REL6  */
   break;

  default:
    break;
  }
  pOut  = (gstk_terminal_rsp_extra_param_type*) MALLOC(nSize);
  if (pOut == NULL) {
    return NULL;
  }
  //Step 2: Setup the pointers
  ptr = (uint8*) pOut;
  ptr += nStructSize;

  switch(commandID)
  {
  case AEESTK_GET_INKEY_REQ:
    {
      AEESTKInkeyResponse *pInKeyRsp = (AEESTKInkeyResponse*) pIn;

      switch(pInKeyRsp->ResponseType)
      {
      case AEESTK_INKEY_INPUT_IS_SINGLE_CHAR:
        if(pInKeyRsp->pszText != NULL)
        {
          pOut->extra_param.get_inkey_extra_param.user_get_inkey_input.text = ptr;
          ptr += (WSTRLEN(pInKeyRsp->pszText) * sizeof(AECHAR));
        }
        break;

      case AEESTK_INKEY_INPUT_IS_YES:
      case AEESTK_INKEY_INPUT_IS_NO:
        pOut->extra_param.get_inkey_extra_param.user_get_inkey_input.text = ptr;
        ptr += 1;
        break;
      }
    }
    break;

  case AEESTK_GET_INPUT_REQ:
    {
      AEESTKString *pString = (AEESTKString*) pIn;

      if(pString->pwszText != NULL)
      {
        pOut->extra_param.user_get_input.text = ptr;
        ptr += (WSTRLEN(pString->pwszText) * sizeof(AECHAR));
      }
    }
    break;

  case AEESTK_SELECT_ITEM_REQ:
    break;

  case AEESTK_SETUP_CALL_REQ:
    {
      AEESTKCallSSResponse *pCallResp = (AEESTKCallSSResponse*) pIn;
      switch(pCallResp->ccReqAction.nCallType)
      {
      case AEESTK_USSD:
        if(pCallResp->ccReqAction.addressString.ussdString.pwszText != NULL)
        {
          pOut->extra_param.setup_call_extra_param.cc_req_action.gstk_address_string.ussd_string.text = ptr;
          ptr += (WSTRLEN(pCallResp->ccReqAction.addressString.ussdString.pwszText) * sizeof(AECHAR));
        }
        break;
      default:
        break;
      }

      if(pCallResp->ccReqAction.alpha.pwszText != NULL)
      {
        pOut->extra_param.setup_call_extra_param.cc_req_action.alpha.text = ptr;
        ptr += (WSTRLEN(pCallResp->ccReqAction.alpha.pwszText) * sizeof(AECHAR));
      }

      if(pCallResp->textString2.pwszText != NULL)
      {
        pOut->extra_param.setup_call_extra_param.text_string2.text = ptr;
        ptr += (WSTRLEN(pCallResp->textString2.pwszText) * sizeof(AECHAR));
      }
    }
    break;

  case AEESTK_SEND_SS_REQ:
    {
      AEESTKCallSSResponse *pSSResp = (AEESTKCallSSResponse*) pIn;
      
      switch(pSSResp->ccReqAction.nCallType)
      {
      case AEESTK_USSD:
        if(pSSResp->ccReqAction.addressString.ussdString.pwszText != NULL)
        {
          pOut->extra_param.send_ss_extra_param.cc_req_action.gstk_address_string.ussd_string.text = ptr;
          ptr += (WSTRLEN(pSSResp->ccReqAction.addressString.ussdString.pwszText) * sizeof(AECHAR));
        }
        break;
      default:
        break;
      }

      if(pSSResp->ccReqAction.alpha.pwszText != NULL)
      {
        pOut->extra_param.send_ss_extra_param.cc_req_action.alpha.text = ptr;
        ptr += (WSTRLEN(pSSResp->ccReqAction.alpha.pwszText) * sizeof(AECHAR));
      }

      if(pSSResp->textString2.pwszText != NULL)
      {
        pOut->extra_param.send_ss_extra_param.text_string2.text = ptr;
        ptr += (WSTRLEN(pSSResp->textString2.pwszText) * sizeof(AECHAR));
      }
    }
    break;

  
  case AEESTK_SEND_USSD_REQ:
    {
      AEESTKUSSDResponse *pUSSDResp = (AEESTKUSSDResponse*) pIn;
      if(pUSSDResp->data.pwszText != NULL)
      {
        pOut->extra_param.send_ussd_extra_param.data.text = ptr;
        ptr += (WSTRLEN(pUSSDResp->data.pwszText+1) * sizeof(AECHAR));
      }

      switch(pUSSDResp->ccReqAction.nCallType)
      {
      case AEESTK_USSD:
        if(pUSSDResp->ccReqAction.addressString.ussdString.pwszText != NULL)
        {
          pOut->extra_param.send_ussd_extra_param.cc_req_action.gstk_address_string.ussd_string.text = ptr;
          ptr += (WSTRLEN(pUSSDResp->ccReqAction.addressString.ussdString.pwszText) * sizeof(AECHAR));
        }
        break;
      default:
        break;
      }

      if(pUSSDResp->ccReqAction.alpha.pwszText != NULL)
      {
        pOut->extra_param.send_ussd_extra_param.cc_req_action.alpha.text = ptr;
        ptr += (WSTRLEN(pUSSDResp->ccReqAction.alpha.pwszText) * sizeof(AECHAR));
      }
    }
    break;

  case AEESTK_RECEIVE_DATA_REQ:
    {
      AEESTKReceiveDataResponse  *pReceiveDataResp = (AEESTKReceiveDataResponse*) pIn;

      if(pReceiveDataResp->pData != NULL)
      {
        pOut->extra_param.receive_data_extra_param.ch_data.data = ptr;
        ptr += pReceiveDataResp->nLength + 1; 
      }
    }
    break;

  case AEESTK_PROVIDE_LANG_INFO_REQ:
    break;


  case AEESTK_PROVIDE_BATTERY_STATUS_REQ:
#ifndef FEATURE_CAT_REL6 
    MSG_HIGH(" FEATURE_CAT_REL6  not supported ",0,0,0);
#endif /* #ifdef FEATURE_CAT_REL6  */
    break;

  default:
    break;
  }
  
  //Step 3: Copy the data.
  pOut->present = TRUE;

  pOut->rsp_type = OEMSTK_GetConfirmationType(commandID);
  switch(commandID)
  {
  case AEESTK_GET_INKEY_REQ:
    {
      AEESTKInkeyResponse *pInKeyRsp = (AEESTKInkeyResponse*) pIn;
      OEMSTK_AEEInkeyResponseToGSTKString(pInKeyRsp,
        &pOut->extra_param.get_inkey_extra_param.user_get_inkey_input);

#ifdef FEATURE_CAT_REL6 
      OEMSTK_AEEDurationToGSTKDuration(&pInKeyRsp->TotalDisplayDuration,
                                       &pOut->extra_param.get_inkey_extra_param.duration_of_exec);
#endif /* #ifdef FEATURE_CAT_REL6  */
    }
    break;

  case AEESTK_GET_INPUT_REQ:
    {
      AEESTKGetInputResponse *pGetInputResponse = (AEESTKGetInputResponse*) pIn;
      OEMSTK_DCSSpc_AEEStringToGSTKString(&(pGetInputResponse->pString),
        &pOut->extra_param.user_get_input,
        pGetInputResponse->dcs);
    }
    break;

  case AEESTK_SELECT_ITEM_REQ:
    pOut->extra_param.select_item_chosen_item =   *((uint8*) pIn);
    break;

  case AEESTK_SETUP_CALL_REQ:
    {
      AEESTKCallSSResponse *pCallSSResp = (AEESTKCallSSResponse*) pIn;
      pOut->extra_param.setup_call_extra_param.has_cc_type_modification = 
        pCallSSResp->bCCModification;

      //Copy the cc req type.
      OEMSTK_AEECCReqToGSTKCCReq(&pCallSSResp->ccReqAction,
        &pOut->extra_param.setup_call_extra_param.cc_req_action);

      //Direct cast here...
      pOut->extra_param.setup_call_extra_param.command_result2 = 
        (gstk_general_result_enum_type) pCallSSResp->nCommandResult2;
    
      pOut->extra_param.setup_call_extra_param.additional_info2.length = 
        pCallSSResp->additionalInfo2.nLength;

      for(i = 0; i < GSTK_MAX_ADDITIONAL_INFO; i++)
      {
          pOut->extra_param.setup_call_extra_param.additional_info2.additional_info[i] = 
              (uint8)pCallSSResp->additionalInfo2.additionalInfo[i];
      }

      OEMSTK_AEEStringToGSTKString(&pCallSSResp->textString2,
        &pOut->extra_param.setup_call_extra_param.text_string2);
    }
    break;
 
  case AEESTK_SEND_SS_REQ:
    {
      AEESTKCallSSResponse *pSSResp = (AEESTKCallSSResponse*) pIn; 
      pOut->extra_param.send_ss_extra_param.has_cc_type_modification = 
        pSSResp->bCCModification;

      //Copy the cc req type.
      OEMSTK_AEECCReqToGSTKCCReq(&pSSResp->ccReqAction,
        &pOut->extra_param.send_ss_extra_param.cc_req_action);

      //Direct cast here...
      pOut->extra_param.send_ss_extra_param.command_result2 = 
        (gstk_general_result_enum_type) pSSResp->nCommandResult2;
      
      pOut->extra_param.send_ss_extra_param.additional_info2.length = 
        pSSResp->additionalInfo2.nLength;

      for(i = 0; i < GSTK_MAX_ADDITIONAL_INFO; i++)
      {
          pOut->extra_param.send_ss_extra_param.additional_info2.additional_info[i] = 
                  (uint8)pSSResp->additionalInfo2.additionalInfo[i];
      }

      OEMSTK_AEEStringToGSTKString(&pSSResp->textString2,
        &pOut->extra_param.send_ss_extra_param.text_string2);    
    }
    break;

  case AEESTK_SEND_USSD_REQ:
    {
      AEESTKUSSDResponse *pUSSDResp = (AEESTKUSSDResponse*) pIn;
      pOut->extra_param.send_ussd_extra_param.has_cc_type_modification = 
        pUSSDResp->bCCModification;

      /* Do either a OEMSTK_AEEStringToGSTKString or a OEMSTK_pack_7_bit_chars 
         depending on the dcs 
      */
      OEMSTK_StringConversion(&pUSSDResp->data,
        pUSSDResp->nDCSFromNW,
         &pOut->extra_param.send_ussd_extra_param.data);    
    
      if(pUSSDResp->bCCModification)
      {

        //Copy the cc req type.
        OEMSTK_AEECCReqToGSTKCCReq(&pUSSDResp->ccReqAction,
          &pOut->extra_param.send_ussd_extra_param.cc_req_action);

        //Direct cast here...
        pOut->extra_param.send_ussd_extra_param.command_result2 = 
          (gstk_general_result_enum_type) pUSSDResp->nCommandResult2;

        pOut->extra_param.send_ussd_extra_param.additional_info2.length = 
          pUSSDResp->additionalInfo2.nLength;

        for(i = 0; i < GSTK_MAX_ADDITIONAL_INFO; i++)
        {
                  pOut->extra_param.send_ussd_extra_param.additional_info2.additional_info[i] = 
                    (uint8)pUSSDResp->additionalInfo2.additionalInfo[i];
        }
      }
      else
      {  
        pOut->extra_param.send_ussd_extra_param.cc_req_action.length = 0;
      }
    }
    break;
  
  case AEESTK_PROVIDE_LANG_INFO_REQ:
    {
      uint8 *pLangInfoResp = (uint8*) pIn;
      pOut->extra_param.provide_lang_info_extra_param.lang_code[0] = pLangInfoResp[0];
      pOut->extra_param.provide_lang_info_extra_param.lang_code[1] = pLangInfoResp[1];

    }
    break;

  case AEESTK_PROVIDE_BATTERY_STATUS_REQ:
#ifdef FEATURE_CAT_REL6 
    {
      AEESTKBatteryStatusType battStRsp = *((AEESTKBatteryStatusType*) pIn);
      OEMSTK_AEESTKBatteryStToGSTKBatterySt(&battStRsp,
                                            &pOut->extra_param.battery_status_info_extra_param);
    }
#else
    MSG_HIGH(" AEESTK_PROVIDE_BATTERY_STATUS_REQ: FEATURE_CAT_REL6  not supported ",0,0,0);
#endif /* #ifdef FEATURE_CAT_REL6  */
    break;


  case AEESTK_OPEN_CH_REQ:
    {
      AEESTKOpenChResponse  *pOpenResp = (AEESTKOpenChResponse*) pIn;

      pOut->extra_param.open_ch_extra_param.ch_status.ch_id = pOpenResp->nChID;
      pOut->extra_param.open_ch_extra_param.ch_status.is_link_est = pOpenResp->bIsLinkEst;
      OEMSTK_ChStatusAddlInfoTableLookUp(OEMSTK_DMSS, 
        &pOut->extra_param.open_ch_extra_param.ch_status.ch_status_additional_info, 
        &pOpenResp->ChStatusAdditionalInfo);
      
      switch (pOpenResp->BearerDescription.BearerType) {
        case AEESTK_BEARER_CSD:
          pOut->extra_param.open_ch_extra_param.bearer_description.bearer_type = GSTK_CSD;
          OEMSTK_BearerCSDSpeedTableLookUp(OEMSTK_DMSS, 
            &pOut->extra_param.open_ch_extra_param.bearer_description.bearer_params.csd_bearer.speed, 
            &pOpenResp->BearerDescription.BearerParams.BearerCSD.Speed);
          OEMSTK_BearerCSDNameTableLookUp(OEMSTK_DMSS, 
            &pOut->extra_param.open_ch_extra_param.bearer_description.bearer_params.csd_bearer.name, 
            &pOpenResp->BearerDescription.BearerParams.BearerCSD.Name);
          OEMSTK_BearerCSDConnectionElementTableLookUp(OEMSTK_DMSS, 
            &pOut->extra_param.open_ch_extra_param.bearer_description.bearer_params.csd_bearer.connection_element, 
            &pOpenResp->BearerDescription.BearerParams.BearerCSD.ConnectionElement);
          break;
        case AEESTK_BEARER_GPRS:
          pOut->extra_param.open_ch_extra_param.bearer_description.bearer_type = GSTK_GPRS;
          pOut->extra_param.open_ch_extra_param.bearer_description.bearer_params.gprs_bearer.delay_cls =
            pOpenResp->BearerDescription.BearerParams.BearerGPRS.nDelayCls;
          pOut->extra_param.open_ch_extra_param.bearer_description.bearer_params.gprs_bearer.mean_throughput =
            pOpenResp->BearerDescription.BearerParams.BearerGPRS.nMeanThroughput;
          pOut->extra_param.open_ch_extra_param.bearer_description.bearer_params.gprs_bearer.peak_throughput =
            pOpenResp->BearerDescription.BearerParams.BearerGPRS.nPeakThroughput;
          pOut->extra_param.open_ch_extra_param.bearer_description.bearer_params.gprs_bearer.precedence_cls =
            pOpenResp->BearerDescription.BearerParams.BearerGPRS.nPrecedenceCls;
          pOut->extra_param.open_ch_extra_param.bearer_description.bearer_params.gprs_bearer.reliability_cls =
            pOpenResp->BearerDescription.BearerParams.BearerGPRS.nReliabilityCls;

          OEMSTK_BearerGPRSPktDataProtocolTableLookUp(OEMSTK_DMSS,
            &pOut->extra_param.open_ch_extra_param.bearer_description.bearer_params.gprs_bearer.pkt_data_protocol,
            &pOpenResp->BearerDescription.BearerParams.BearerGPRS.PktDataProtocol);
          break;
        case AEESTK_BEARER_DEFAULT:
          pOut->extra_param.open_ch_extra_param.bearer_description.bearer_type = GSTK_BEARER_DEFAULT;
          break;
        default:
          pOut->extra_param.open_ch_extra_param.bearer_description.bearer_type = GSTK_BEARER_DEFAULT;
          break;
      }
      pOut->extra_param.open_ch_extra_param.buffer_size = pOpenResp->nBufferSize;
    }
    break;

    case AEESTK_SEND_DATA_REQ:
    {
      pOut->extra_param.send_data_extra_param.ch_data_remain_buf_len = *((int32*) pIn);
    }
    break;

    case AEESTK_RECEIVE_DATA_REQ:
    {
      AEESTKReceiveDataResponse  *pReceiveDataResp = (AEESTKReceiveDataResponse*) pIn;
      
      if ((pReceiveDataResp->nLength > 0) && (pReceiveDataResp->pData != NULL)) {
        (void)MEMCPY(pOut->extra_param.receive_data_extra_param.ch_data.data,
               pReceiveDataResp->pData,
               int32touint32(pReceiveDataResp->nLength));
      }
      pOut->extra_param.receive_data_extra_param.ch_data.data_len = pReceiveDataResp->nLength;
      pOut->extra_param.receive_data_extra_param.ch_data_remaining_len = pReceiveDataResp->nChDataRemainingLen;

    }
    break;


    case AEESTK_GET_CH_STATUS_REQ:
    {
      AEESTKGetChStatusResponse *pGetChStatusResp;
      pGetChStatusResp = (AEESTKGetChStatusResponse*) pIn;
      /* Count of the number of channels for which status is available.
         Includes only the Open and Dropped Channels.
      */
      pOut->extra_param.get_ch_status_extram_param.cnt = sz/uint32toint32(sizeof(AEESTKGetChStatusResponse));
      for(i=0;i<sz;i++)
      {
        pOut->extra_param.get_ch_status_extram_param.ch_status[i].is_link_est = pGetChStatusResp[i].bIsLinkEst;
        pOut->extra_param.get_ch_status_extram_param.ch_status[i].ch_id       = pGetChStatusResp[i].nChID;
      
        OEMSTK_ChStatusAddlInfoTableLookUp(OEMSTK_DMSS, 
            &pOut->extra_param.get_ch_status_extram_param.ch_status[i].ch_status_additional_info, 
            &(pGetChStatusResp[i].ChStatusAdditionalInfo));
      }        
    }
    break;


  default:
    break;
  }
  
  return pOut;

}
gstk_general_result_enum_type OEMSTK_AEEResultToGSTKResult(AEESTKResult result)
{
  //currently AEESTK and gstk use the same values, so just cast.
  return (gstk_general_result_enum_type) result;
}

  
static boolean OEMSTK_IsValidEnvelopeCommand(gstk_cmd_from_card_type * req_rsp )
{
  //Right now, no envelope commands are supported.
  return FALSE;
} /*lint !e715 */
static boolean OEMSTK_IsValidProactiveCommand(gstk_cmd_from_card_type * req_rsp )
{
  if(!req_rsp)
  {
    return FALSE;
  }
  switch(req_rsp->hdr_cmd.command_id)
  {
  case GSTK_DISPLAY_TEXT_REQ:
  case GSTK_GET_INKEY_REQ:
  case GSTK_GET_INPUT_REQ:
  case GSTK_LAUNCH_BROWSER_REQ:
  case GSTK_PLAY_TONE_REQ:
  case GSTK_SELECT_ITEM_REQ:
  case GSTK_SEND_SMS_REQ:
  case GSTK_SEND_SS_REQ:
  case GSTK_SEND_USSD_REQ:
  case GSTK_SETUP_CALL_REQ:
  case GSTK_SETUP_MENU_REQ:
  case GSTK_MORE_TIME_REQ:
  case GSTK_SETUP_IDLE_TEXT_REQ:
  case GSTK_END_PROACTIVE_CMD_REQ:
  case GSTK_TOOLKIT_MODE_CHANGE_REQ:
  case GSTK_PROVIDE_LANG_INFO_REQ:
    return TRUE;

  case GSTK_PROVIDE_BATTERY_STATUS_REQ:
#ifndef FEATURE_CAT_REL6 
    MSG_HIGH(" AEESTK_PROVIDE_BATTERY_STATUS_REQ : FEATURE_CAT_REL6  not supported ",0,0,0);
    return FALSE;
#else
    return TRUE;
#endif /* #ifdef FEATURE_CAT_REL6  */

  case GSTK_LANG_NOTIFICATION_REQ:
  case GSTK_SEND_DTMF_REQ:
  case GSTK_OPEN_CH_REQ:
  case GSTK_CLOSE_CH_REQ:
  case GSTK_SEND_DATA_REQ:
  case GSTK_RECEIVE_DATA_REQ:
  case GSTK_GET_CH_STATUS_REQ:
    return TRUE;
  
  default:
    break;
  }
  return FALSE;
  
}
static AEESTKProactiveCmdType OEMSTK_GSTKCmdTypeToAEECmdType(gstk_cmd_enum_type command_id)
{
  switch(command_id)
  {
  case GSTK_DISPLAY_TEXT_REQ:               return AEESTK_DISPLAY_TEXT_REQ;
  case GSTK_GET_INKEY_REQ:                  return AEESTK_GET_INKEY_REQ;
  case GSTK_GET_INPUT_REQ:                  return AEESTK_GET_INPUT_REQ;
  case GSTK_LAUNCH_BROWSER_REQ:             return AEESTK_LAUNCH_BROWSER_REQ;
  case GSTK_PLAY_TONE_REQ:                  return AEESTK_PLAY_TONE_REQ;
  case GSTK_SELECT_ITEM_REQ:                return AEESTK_SELECT_ITEM_REQ;
  case GSTK_SEND_SMS_REQ:                   return AEESTK_SEND_SMS_REQ;
  case GSTK_SEND_SS_REQ:                    return AEESTK_SEND_SS_REQ;
  case GSTK_SEND_USSD_REQ:                  return AEESTK_SEND_USSD_REQ;
  case GSTK_SETUP_CALL_REQ:                 return AEESTK_SETUP_CALL_REQ;
  case GSTK_SETUP_MENU_REQ:                 return AEESTK_SETUP_MENU_REQ;
  case GSTK_MORE_TIME_REQ:                  return AEESTK_MORE_TIME_REQ;
  case GSTK_SETUP_IDLE_TEXT_REQ:            return AEESTK_UPDATE_IDLE_TEXT_REQ;
  case GSTK_SEND_DTMF_REQ:                  return AEESTK_SEND_DTMF_REQ;
  case GSTK_PROVIDE_LANG_INFO_REQ:          return AEESTK_PROVIDE_LANG_INFO_REQ;
  case GSTK_PROVIDE_BATTERY_STATUS_REQ:
#ifdef FEATURE_CAT_REL6 
    return AEESTK_PROVIDE_BATTERY_STATUS_REQ;
#else
    MSG_HIGH("AEESTK_PROVIDE_BATTERY_STATUS_REQ : FEATURE_CAT_REL6  not supported",0,0,0);
    break;
#endif
  case GSTK_LANG_NOTIFICATION_REQ:          return AEESTK_LANG_NOTIFICATION_REQ;
  case GSTK_OPEN_CH_REQ:                    return AEESTK_OPEN_CH_REQ;
  case GSTK_CLOSE_CH_REQ:                   return AEESTK_CLOSE_CH_REQ;
  case GSTK_SEND_DATA_REQ:                  return AEESTK_SEND_DATA_REQ;
  case GSTK_RECEIVE_DATA_REQ:               return AEESTK_RECEIVE_DATA_REQ;
  case GSTK_GET_CH_STATUS_REQ:              return AEESTK_GET_CH_STATUS_REQ;
  case GSTK_END_PROACTIVE_CMD_REQ:          return AEESTK_END_PROACTIVE_CMD_SES;
  case GSTK_TOOLKIT_MODE_CHANGE_REQ:        
  default:                                  break;
  }
  return AEESTK_PROACTIVE_CMD_SES_WITHOUT_TR;

}

/*===========================================================================

FUNCTION: OEMSTK_SENDEXCEEDCAPACITYRESP

DESCRIPTION
  This is send terminal response to GSTK with result == Mobile currently
  unable to process command

DEPENDENCIES
  none

RETURN VALUE
  none
SIDE EFFECTS
===========================================================================*/
void OEMSTK_SendExceedCapacityResp(gstk_cmd_from_card_type * pReq)
{
  gstk_status_enum_type              nClientStatus;
  gstk_cmd_enum_type                 nRespEnum;
  gstk_general_result_enum_type      nGeneralResult;
  gstk_additional_info_type          sAdditionalInfo;
  gstk_terminal_rsp_extra_param_type sExtraParm;
  AEESTKProactiveCmdType             nBrewCmdEnum;

  if(!pReq)
  {
    MSG_ERROR("pReq NULL ptr",0,0,0);
    return;
  }

  nBrewCmdEnum = OEMSTK_GSTKCmdTypeToAEECmdType(pReq->hdr_cmd.command_id);
  nRespEnum = OEMSTK_GetConfirmationType(nBrewCmdEnum);

  sAdditionalInfo.length = 1;
  sAdditionalInfo.additional_info[0] = (int)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
  sExtraParm.present = FALSE;
  nGeneralResult = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;

  nClientStatus = gstk_send_terminal_response( 
                      cl_id,
                      pReq->hdr_cmd.user_data,
                      pReq->hdr_cmd.cmd_detail_reference,
                      pReq->hdr_cmd.command_number,
                      nRespEnum,
                      nGeneralResult,
                      &sAdditionalInfo,
                      &sExtraParm);

  if (nClientStatus != GSTK_SUCCESS)
  {
    MSG_ERROR("Unable to Send Terminal Response to GSTK, Status = 0x%x", nClientStatus, 0, 0);
    MSG_ERROR("Result: 0x%x, Cmd_ID: 0x%x, Cmd_Num", 
      nGeneralResult, pReq->hdr_cmd.command_id, pReq->hdr_cmd.command_number);
    return;
  }    
} /* OEMSTK_SendExceedCapacityResp */

/*===========================================================================

FUNCTION: OEMSTKGSTKevtCb

DESCRIPTION
  This is the callback function which gets called when ever GSTK has a new
  proactive command to be sent to Cat App.
  It calls a callback in AEESTK.c which AEE registeres

DEPENDENCIES
  none

RETURN VALUE
  none
SIDE EFFECTS
===========================================================================*/
void OEMSTKGSTKevtCb(gstk_cmd_from_card_type * req_rsp )
{
  uint32       i               = 0;
  uint32       dwTransactionID = OEMSTK_MAX_CACHE_ENTRIES;
  uint32       dwSize          = 0;
  boolean      is_space_avail  = FALSE;
    
  MSG_LOW("In OEMSTKGSTKEvtCB",0,0,0);

  if(req_rsp == NULL)
  {
    return;
  }
  
  if (gpSTK == NULL) 
  {
    return;
  }

  if(req_rsp->hdr_cmd.command_id == GSTK_SETUP_EVT_USER_ACT_NOTIFY)
  {
    if(gpSTK) 
    {
      gpSTK->bUserActionNotif = req_rsp->cmd.evt_notify_req.require_notification;
    }
  }
  if(req_rsp->hdr_cmd.command_id == GSTK_SETUP_EVT_DATA_AVAIL_NOTIFY)
  {
    if(gpSTK) 
    {
      gpSTK->bDataAvailNotif = req_rsp->cmd.evt_notify_req.require_notification;
    }
  }
  if(req_rsp->hdr_cmd.command_id == GSTK_SETUP_EVT_CH_STATUS_NOTIFY)
  {
    if(gpSTK) 
    {
      gpSTK->bChStatusNotif = req_rsp->cmd.evt_notify_req.require_notification;
    }
  }
  if(req_rsp->hdr_cmd.command_id == GSTK_SETUP_EVT_BROWSER_TERM_NOTIFY)
  {
    if(gpSTK) 
    {
      gpSTK->bBrowserTermNotif = req_rsp->cmd.evt_notify_req.require_notification;
    }
  }
  if(req_rsp->hdr_cmd.command_id == GSTK_SETUP_EVT_IDLE_SCREEN_NOTIFY)
  {
    if(gpSTK)
    {
      gpSTK->bIdleScreenNotif = req_rsp->cmd.evt_notify_req.require_notification;
    }
  }
  if(req_rsp->hdr_cmd.command_id == GSTK_SETUP_EVT_LANG_SEL_NOTIFY)
  {
    if(gpSTK)
    {
      gpSTK->bLangSelectionNotif = req_rsp->cmd.evt_notify_req.require_notification;
    }
  }

  if(OEMSTK_IsValidEnvelopeCommand(req_rsp))
  {

    OEMSTK_MUTEX_LOCK();
    /* The last two are reserved for Proactive Command */
    for(i = 0; 
        i < (OEMSTK_MAX_CACHE_ENTRIES-OEMSTK_MAX_NUM_OF_CONCURRENT_PRO_CMDS);
        i++)
    {
      if(gpSTK->cmdCache[i].state == OEMSTK_CACHE_STATE_FREE)  
      {
        dwTransactionID = i;
        break;
      }
    }
    OEMSTK_MUTEX_UNLOCK();

    if(dwTransactionID >= 
         OEMSTK_MAX_CACHE_ENTRIES-OEMSTK_MAX_NUM_OF_CONCURRENT_PRO_CMDS)
    {
      //Maxed out on envelope command cache entries
      MSG_ERROR("ISTK no Free cache for Envelope Command", 0, 0, 0);
      return;
    }

    gpSTK->cmdCache[dwTransactionID].state = OEMSTK_CACHE_STATE_NEW;
    
    gpSTK->cmdCache[dwTransactionID].commandType    = NMASK_STKNOTIFIER_ENVELOPE_CMD;
    gpSTK->cmdCache[dwTransactionID].commandID      = OEMSTK_GSTKCmdTypeToAEECmdType
                                      (req_rsp->hdr_cmd.command_id);
    
    gpSTK->cmdCache[dwTransactionID].cmd_detail_reference = 0;
    gpSTK->cmdCache[dwTransactionID].command_number = 0;
    gpSTK->cmdCache[dwTransactionID].pCommand = NULL;
    gpSTK->cmdCache[dwTransactionID].dwSize         = dwSize;
    gpSTK->cmdCache[dwTransactionID].user_data      = req_rsp->hdr_cmd.user_data;
    gpSTK->cmdCache[dwTransactionID].wTransactionID =(uint16)(req_rsp->hdr_cmd.user_data);
    
    if(gpSTK->pcbOnCardCmd != NULL)
    {
#ifdef FEATURE_BREW_3_0
      AEE_ResumeCallback(gpSTK->pcbOnCardCmd,  0);
#else
      AEE_ResumeEx(gpSTK->pcbOnCardCmd, 0, 0);
#endif
    }

  }
  else if(OEMSTK_IsValidProactiveCommand(req_rsp))
  {
    dwTransactionID = OEMSTK_MAX_CACHE_ENTRIES - 1;
    for(i = OEMSTK_MAX_CACHE_ENTRIES - OEMSTK_MAX_NUM_OF_CONCURRENT_PRO_CMDS; 
        i<= OEMSTK_MAX_CACHE_ENTRIES-1; i++)
    { 
      if ((gpSTK->cmdCache[i].state == OEMSTK_CACHE_STATE_FREE) || 
          ((gpSTK->cmdCache[i].state == OEMSTK_CACHE_STATE_DISPATCHED) &&
          ((gpSTK->cmdCache[i].commandID == AEESTK_END_PROACTIVE_CMD_SES) || 
          (gpSTK->cmdCache[i].commandID == AEESTK_PROACTIVE_CMD_SES_WITHOUT_TR))))
      {     
        is_space_avail = TRUE;
        OEMSTK_CleanupCache(i);
        dwTransactionID = i;
        break;
      }
    }
    if(is_space_avail == FALSE)
    {
      MSG_ERROR("ISTK no Free cache - ME Curr Unable to Proc Cmd", 0, 0, 0);
      if ((req_rsp->hdr_cmd.command_id != GSTK_END_PROACTIVE_CMD_REQ) &&
          (req_rsp->hdr_cmd.command_id != GSTK_TOOLKIT_MODE_CHANGE_REQ))
      {
        OEMSTK_SendExceedCapacityResp(req_rsp);
      }
      return;
    }
    
    //Cache the command
    gpSTK->cmdCache[dwTransactionID].state = OEMSTK_CACHE_STATE_NEW;
    
    gpSTK->cmdCache[dwTransactionID].cmd_detail_reference = 
                                        req_rsp->hdr_cmd.cmd_detail_reference;
    gpSTK->cmdCache[dwTransactionID].command_number = req_rsp->hdr_cmd.command_number;
    gpSTK->cmdCache[dwTransactionID].pCommand = OEMSTK_GSTKCmdToAEEProactiveCmd(gpSTK,
                                    req_rsp, 
                                    &dwSize);
    if (gpSTK->cmdCache[dwTransactionID].pCommand == NULL) {
      MSG_ERROR("No MEM for pCommand", 0, 0, 0);
      OEMSTK_SendExceedCapacityResp(req_rsp);
      OEMSTK_CleanupCache(uint32toint32(dwTransactionID));
      return;
    }
    gpSTK->cmdCache[dwTransactionID].commandID      = OEMSTK_GSTKCmdTypeToAEECmdType
                                      (req_rsp->hdr_cmd.command_id);
                                  
    //every req has  CmdHeader has the first field.
    gpSTK->cmdCache[dwTransactionID].commandType    = NMASK_STKNOTIFIER_PROACTIVE_CMD;
    gpSTK->cmdCache[dwTransactionID].dwSize         = dwSize;
    gpSTK->cmdCache[dwTransactionID].user_data      = req_rsp->hdr_cmd.user_data;
    gpSTK->cmdCache[dwTransactionID].wTransactionID = ++gpSTK->wTransactionCounter;
    
    if(gpSTK->pcbOnCardCmd != NULL)
    {
#ifdef FEATURE_BREW_3_0
      AEE_ResumeCallback(gpSTK->pcbOnCardCmd,  0);
#else
      AEE_ResumeEx(gpSTK->pcbOnCardCmd, 0, 0);
#endif
    }
  }
}

/*===========================================================================
FUNCTION: OEMSTK_UpdateProfileDownloadValue,

DESCRIPTION
  Allows UI to the change profile download value

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Profile download value sent to GSTK
  EFAILED  - Profile download not sent to GSTK


SIDE EFFECTS

===========================================================================*/
static int OEMSTK_UpdateProfileDownloadValue(OEMSTK *po)
 {
  gstk_status_enum_type   client_status;

  client_status = gstk_update_profile_download_value_ext(
                            (uint32)GSTK_UI_TYPE,
                            0,
                            ARR_SIZE(value_list),
                            value_list);

  if (client_status == GSTK_SUCCESS)
     return SUCCESS;
  else
     return EFAILED;
} /*lint !e715 */

/*===========================================================================

FUNCTION: OEMSTK_Init

DESCRIPTION
  Allocates a new object from the heap and stores the IShell pointer
  used to generate the events.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
===========================================================================*/
static int OEMSTK_Init(OEMSTK *pMe)
{
  gstk_status_enum_type   client_status;
  int i; 
  uint64 evt_reg_bitmask = GSTK_REG_DISPLAY_TEXT_EVT | GSTK_REG_GET_INKEY_EVT |
      GSTK_REG_GET_INPUT_EVT | GSTK_REG_PLAY_TONE_EVT |
      GSTK_REG_SELECT_ITEM_EVT | GSTK_REG_SEND_SMS_EVT |
      GSTK_REG_SETUP_CALL_EVT | GSTK_REG_SETUP_IDLE_TEXT_EVT |
      GSTK_REG_SETUP_MENU_EVT | GSTK_REG_SEND_SS_EVT | 
      GSTK_REG_SEND_USSD_EVT | GSTK_REG_LANG_NOTIFY_EVT | 
      GSTK_REG_SEND_DTMF_EVT | GSTK_REG_SETUP_EVT_BROWSER_TERM_EVT | 
      GSTK_REG_SETUP_EVT_LANG_SEL_EVT | GSTK_REG_PROVIDE_LANG_INFO_EVT | 
      GSTK_REG_SETUP_EVT_USER_ACT_EVT | GSTK_REG_DATA_EVT | 
      GSTK_REG_SETUP_EVT_IDLE_MENU_EVT| GSTK_REG_SETUP_EVT_DATA_AVAILABLE_EVT |
      GSTK_REG_SETUP_EVT_CH_STATUS_EVT | GSTK_REG_LAUNCH_BROWSER_EVT
#ifdef FEATURE_CAT_REL6
      | GSTK_REG_PROVIDE_BATTERY_STATE_EVT
#endif /* #ifdef FEATURE_CAT_REL6  */
      ;


  if(!pMe)
  {
    MSG_ERROR("pMe NULL ptr",0,0,0);
    return EBADPARM;
  }

  client_status = gstk_client_init(&cl_id,  GSTK_UI_TYPE );

  if (client_status == GSTK_SUCCESS)
  {
    client_status = gstk_client_reg_ext(cl_id,
                                    (uint32) pMe,
                                    (gstk_evt_cb_funct_type)OEMSTKGSTKevtCb,
                                    evt_reg_bitmask);

    if (client_status != GSTK_SUCCESS)
    {
      ERR_FATAL("client_init error", 0,0,0);
    } 
  }
  else
  {
     ERR_FATAL("client_init error", 0,0,0);
  }

  for(i = 0; i < OEMSTK_MAX_CACHE_ENTRIES; i++)
  {
    pMe->cmdCache[i].state = OEMSTK_CACHE_STATE_FREE;
    pMe->cmdCache[i].pCommand = NULL;
  }
  //Initialize the cache to be invalid
  return OEMSTK_UpdateProfileDownloadValue(pMe);

} /* OEMSTK_Init() */

/*===========================================================================

FUNCTION: OEMSTK_CLEANUPCACHE

DESCRIPTION
  This is provide memory free and set the state of the cache to FREE

DEPENDENCIES
  none

RETURN VALUE
  none
SIDE EFFECTS
===========================================================================*/
void OEMSTK_CleanupCache(int32 index)
{
  if (index < OEMSTK_MAX_CACHE_ENTRIES) {
    MSG_HIGH("Clean up a dispatched command : %d",index, 0, 0);
    FREEIF(gpSTK->cmdCache[index].pCommand);
    gpSTK->cmdCache[index].pCommand = NULL;
    gpSTK->cmdCache[index].state = OEMSTK_CACHE_STATE_FREE;
  }
}
/*===========================================================================

FUNCTION: OEMSTK_FindCommandIdInCache

DESCRIPTION
 This function checks to see if a particular cmd with a command id is present 
 in the oemstk cache.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - if command is found
  EFAILED  -  if command is not found


SIDE EFFECTS
===========================================================================*/
static int OEMSTK_FindCommandIdInCache(
  const OEMSTK               *pMe,
  AEESTKProactiveCmdType     request_type,
  uint8                      cmd_num,
  int32                      * index)
{  
  int i = 0;

  if(!pMe || !index)
  {
    MSG_ERROR("OEMSTK_FindCommandIdInCache: NULL ptr",0,0,0);
    return EBADPARM;
  }

  OEMSTK_MUTEX_LOCK();

  for(i = 0; i < OEMSTK_MAX_CACHE_ENTRIES; i++)
  {
    if((OEMSTK_CACHE_STATE_DISPATCHED == pMe->cmdCache[i].state) &&
      (pMe->cmdCache[i].commandID == request_type) && 
      (pMe->cmdCache[i].command_number == cmd_num))
    {
      break;
    }
  }
  OEMSTK_MUTEX_UNLOCK();

  if(OEMSTK_MAX_CACHE_ENTRIES == i)
  {
    MSG_ERROR("Failed looking up the command_id", 0, 0 , 0);
    return EBADPARM;
  }
  *index = i;
  return SUCCESS;
}
/*===========================================================================

FUNCTION: OEMSTK_SendCommandResponse

DESCRIPTION
   Sends a Terminal Response to GSTK

PARAMETERS
  pMe,               - ponter to OEMSTK object
  term_resp_e        - Cmd request type.
  cmd_num            - Command number of the proactive command
  result             - result of the execution of the Proactive command
  additional_result  - Additional info to the result
  extra_param        - Extra parameters (includes any user responses etc)
    AEESTKCallSSResponse 
    AEESTKCallSSResponse
    AEESTKUSSDResponse
    AEESTKUSSDResponse
    AEESTKInkeyResponseType
    AEESTKInkeyResponse
    AEESTKOpenChResponse
    AEESTKReceiveDataResponse
    AEESTKGetChStatusResponse
    AEESTKLangSelResponse
    AEESTKString
  extra_param_length  - extra parameter length

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Sending was success
  EFAILED  - Send Failure


SIDE EFFECTS
===========================================================================*/
int OEMSTK_SendCommandResponse(
              OEMSTK                     *pMe,
              AEESTKProactiveCmdType     request_type,
              uint8                      cmd_num,
              AEESTKResult               result,
              AEESTKAdditionalInfo       *additional_result,
              void                       *extra_param,
              uint8                      extra_param_length)
{
  gstk_status_enum_type               client_status;
  gstk_cmd_enum_type                  resp;
  gstk_general_result_enum_type       general_result;
  gstk_additional_info_ptr_type      *pAdditionalInfo = NULL;
  gstk_terminal_rsp_extra_param_type *pExtraParm      = NULL;
  int32                               index           = 0;
  
  if(!pMe)
  {
    MSG_ERROR("pMe NULL ptr",0,0,0);
    return EBADPARM;
  }

  /* Convert the event to confirmation type. */
  resp = OEMSTK_GetConfirmationType(request_type);

  if(OEMSTK_FindCommandIdInCache(pMe,request_type, cmd_num, &index) == EBADPARM)
  {
    MSG_ERROR("Failed looking up the command_id", 0, 0 , 0);
    return EBADPARM;
  }
    
  pAdditionalInfo = OEMSTK_AEEAddnlInfoToGSTKAddnlInfo(additional_result);

  if(!pAdditionalInfo)
  {
    MSG_ERROR("pAdditionalInfo ptr is NULL",0,0, 0);
    return EFAILED;
  }

  pExtraParm      = OEMSTK_AEEExtraParmToGSTKExtraParm(request_type, 
                                                       extra_param,
                                                       extra_param_length);
  general_result = OEMSTK_AEEResultToGSTKResult(result);

  client_status = gstk_send_terminal_response_ext( cl_id,
                        pMe->cmdCache[index].user_data,
                        pMe->cmdCache[index].cmd_detail_reference,
                        cmd_num,
                        resp,
                        general_result,
                        pAdditionalInfo,
                        pExtraParm);
  /* Free the additional info data after successfully queuing the information in the gstk queue */
  FREEIF(pAdditionalInfo->additional_info_ptr);
  pAdditionalInfo->additional_info_ptr = NULL;
  FREE(pAdditionalInfo);
  pAdditionalInfo = NULL;
  FREE(pExtraParm);
  pExtraParm = NULL;

  if (client_status == GSTK_SUCCESS)
  {
    OEMSTK_CleanupCache(index);
    return SUCCESS;
  }
  else {
    MSG_ERROR("GSTK rejects Terminal Response, Status = 0x%x", client_status, 0, 0);
    MSG_ERROR("Result: 0x%x, Cmd_ID: 0x%x, Cmd_Num", result, resp, cmd_num);
    return EFAILED;
  }
}

/*===========================================================================

FUNCTION: OEMSTK_SendEnvelopeEvtDownloadCommand

DESCRIPTION
  Allocates a new object from the heap and stores the IShell pointer
  used to generate the events.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Send of envelope evt command passed
  EFAILED   - Send of envelope evt command failed

SIDE EFFECTS
===========================================================================*/
int OEMSTK_SendEnvelopeEvtDownloadCommand(
                            OEMSTK            *po,
                            uint16            *pwTransactionID,
                            uint32            dwSize,
                            AEESTKEventDLInd  *evt_list)
{
  gstk_status_enum_type   client_status;
  
  //Convert AEESTKEventDlInd to gstk_evt_dl_ind_type;
  //We have only one field.So just copy that.
  gstk_evt_dl_ind_type gstk_evt_list;

  if(!pwTransactionID)
  {
    MSG_ERROR("pwTransactionID NULL ptr",0,0,0);
    return EFAILED;
  }

  if(!evt_list)
  {
    MSG_ERROR("evt_list NULL ptr",0,0,0);
    return EFAILED;
  }
  switch(evt_list->commandID)
  {
  case AEESTK_IDLE_SCREEN_REQ:
    gstk_evt_list.evt_type = GSTK_IDLE_SCRN_EVT_IND;
    break;

  case AEESTK_USER_ACTIVITY_REQ:
    gstk_evt_list.evt_type = GSTK_USER_ACTIVITY_EVT_IND;
    break;
 
  default:
    MSG_ERROR("Unable to send Envelope Download Command %d", evt_list->commandID, 0, 0);
    return EFAILED;
  }

  //TODO: Generate a transaction id here..
  *pwTransactionID = ++gpSTK->wTransactionCounter;
  client_status = gstk_send_envelope_evt_download_command(
                    cl_id,
                    (uint32) *pwTransactionID,
                    &gstk_evt_list);

  if (client_status == GSTK_SUCCESS)
     return SUCCESS;
  else
     return EFAILED;
} /*lint !e715 */

/*===========================================================================

FUNCTION: OEMSTK_SendEnvelopeMenuSelCommand

DESCRIPTION
   Sends an envelope command to GSTK

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EBADPARM  - Bad parameter
  ENOMEMORY - Not enough memory to process the request


SIDE EFFECTS
===========================================================================*/
int OEMSTK_SendEnvelopeMenuSelCommand(OEMSTK        *po,
                                      uint16        *pwTransactionID,
                                      byte          ic,
                                      boolean       hr)
{
  //TODO : Generate a transaction id here...
  gstk_status_enum_type   client_status;

  if(!pwTransactionID)
  {
    MSG_ERROR("pwTransactionID NULL ptr",0,0,0);
    return EFAILED;
  }
  *pwTransactionID = ++gpSTK->wTransactionCounter;
  client_status = gstk_send_envelope_menu_sel_command(cl_id, (uint32) *pwTransactionID, ic, hr);
  if (client_status == GSTK_SUCCESS)
     return SUCCESS;
  else
     return EFAILED;
} /*lint !e715 */



/*===========================================================================

FUNCTION: OEMSTK_GetConfirmationType

DESCRIPTION

PROTOTYPE:
 gstk_cmd_enum_type OEMSTK_GetConfirmationType(AEESTKProactiveCmdType proactive_cmd)

PARAMETERS:
  proactive_cmd: [in]: Specifies the proactive command which is being processed


DEPENDENCIES
  none

RETURN VALUE
  The confirmation identifier for the proactive command that is being processed

SIDE EFFECTS
  none
===========================================================================*/

static gstk_cmd_enum_type OEMSTK_GetConfirmationType(AEESTKProactiveCmdType proactive_cmd)
{
  switch(proactive_cmd)
  {
    case AEESTK_DISPLAY_TEXT_REQ:           return GSTK_DISPLAY_TEXT_CNF;
    case AEESTK_GET_INKEY_REQ:              return GSTK_GET_INKEY_CNF;
    case AEESTK_GET_INPUT_REQ:              return GSTK_GET_INPUT_CNF;
    case AEESTK_LAUNCH_BROWSER_REQ:         return GSTK_LAUNCH_BROWSER_CNF;
    case AEESTK_MORE_TIME_REQ:              return GSTK_MORE_TIME_CNF;
    case AEESTK_PLAY_TONE_REQ:              return GSTK_PLAY_TONE_CNF;
    case AEESTK_POLL_INTERVAL_REQ:          return GSTK_POLL_INTERVAL_CNF;
    case AEESTK_PROVIDE_LOCAL_INFO_REQ:     return GSTK_PROVIDE_LOCAL_INFO_CNF;
    case AEESTK_PROVIDE_LANG_INFO_REQ:      return GSTK_PROVIDE_LANG_INFO_CNF;
    case AEESTK_PROVIDE_BATTERY_STATUS_REQ: 
#ifdef FEATURE_CAT_REL6 
      return GSTK_PROVIDE_BATT_STATUS_CNF;
#else
      MSG_HIGH("AEESTK_PROVIDE_BATTERY_STATUS_REQ : FEATURE_CAT_REL6  not supported",0,0,0);
      break;
#endif
    case AEESTK_REFRESH_REQ:                return GSTK_REFRESH_CNF;
    case AEESTK_RUN_AT_CMDS_REQ:            return GSTK_RUN_AT_CMDS_CNF;
    case AEESTK_SELECT_ITEM_REQ:            return GSTK_SELECT_ITEM_CNF;
    case AEESTK_SEND_SMS_REQ:               return GSTK_SEND_SMS_CNF;
    case AEESTK_SEND_SS_REQ:                return GSTK_SEND_SS_CNF;
    case AEESTK_SEND_USSD_REQ:              return GSTK_SEND_USSD_CNF;
    case AEESTK_SETUP_CALL_REQ:             return GSTK_SETUP_CALL_CNF;
    case AEESTK_SETUP_EVENT_LIST_REQ:       return GSTK_SETUP_EVENT_LIST_CNF;
    case AEESTK_SETUP_MENU_REQ:             return GSTK_SETUP_MENU_CNF;
    case AEESTK_SEND_DTMF_REQ:              return GSTK_SEND_DTMF_CNF;
    case AEESTK_LANG_NOTIFICATION_REQ:      return GSTK_LANG_NOTIFICATION_CNF;
    case AEESTK_OPEN_CH_REQ:                return GSTK_OPEN_CH_CNF;
    case AEESTK_CLOSE_CH_REQ:               return GSTK_CLOSE_CH_CNF;
    case AEESTK_SEND_DATA_REQ:              return GSTK_SEND_DATA_CNF;
    case AEESTK_RECEIVE_DATA_REQ:           return GSTK_RECEIVE_DATA_CNF;
    case AEESTK_GET_CH_STATUS_REQ:          return GSTK_GET_CH_STATUS_CNF;
    case AEESTK_UPDATE_IDLE_TEXT_REQ:       return GSTK_SETUP_IDLE_TEXT_CNF;
    case AEESTK_SETUP_IDLE_TEXT_REQ:        return GSTK_SETUP_IDLE_TEXT_CNF;

    default:
      break;
  }

  ERR("Unknown proactive_cmd %d", proactive_cmd, 0, 0);
  return GSTK_END_PROACTIVE_CMD_REQ; /* default to something */
}


/*===========================================================================
Called by AEESTK to register callback to be invoked on card commands
===========================================================================*/
void OEMSTK_OnCardCmd(OEMSTK *pMe, AEECallback *pcb)
{
  if((pMe == NULL) || (pcb == NULL))
  {
    MSG_ERROR("Bad parameters", 0, 0, 0);
  }
  if (pMe != NULL) {
    pMe->pcbOnCardCmd = pcb;
  }
}

/*===========================================================================
Called by AEESTK to get next available command in cache
===========================================================================*/
void OEMSTK_GetNextCmd(OEMSTK *pMe,
                       uint32 *pCommandType,
                       uint8 *pCmdNum,
                       uint32 *pdwTransactionID)
{
  uint32 i;
  if((pMe == NULL) || (pCommandType == NULL) ||
     (pdwTransactionID == NULL) || (pCmdNum == NULL))
  {
    MSG_ERROR("Bad parameters", 0, 0, 0);
    return;
  }
  *pdwTransactionID = 0; // Invalid transaction ID.

  OEMSTK_MUTEX_LOCK();

  for(i = 0; i < OEMSTK_MAX_CACHE_ENTRIES; i++)
  {
    if(pMe->cmdCache[i].state == OEMSTK_CACHE_STATE_NEW)
    {
      pMe->cmdCache[i].state = OEMSTK_CACHE_STATE_DISPATCHED;
      *pCommandType = pMe->cmdCache[i].commandType;
      *pCmdNum = pMe->cmdCache[i].command_number;
      *pdwTransactionID = ((int32)pMe->cmdCache[i].commandID | 
              (pMe->cmdCache[i].wTransactionID << 16));
      break;
    }
  }
  OEMSTK_MUTEX_UNLOCK();

}

int OEMSTK_GetBufSize(OEMSTK *pMe, uint32 dwID, uint32 *pBufSize)
{
  uint32 dwTransactionID = OEMSTK_MAX_CACHE_ENTRIES;
  uint32 i;

  if(pMe == NULL)
    return EBADPARM;

  if(pBufSize == NULL)
    return EBADPARM;

  OEMSTK_MUTEX_LOCK();

  for(i = 0; i < OEMSTK_MAX_CACHE_ENTRIES; i++)
  {
    MSG_HIGH("cmdCache[%d], state : %d, commandID : %d", 
      i, pMe->cmdCache[i].state, pMe->cmdCache[i].commandID);
    if(pMe->cmdCache[i].state != OEMSTK_CACHE_STATE_FREE )
    {
      if((pMe->cmdCache[i].commandID == (AEESTKProactiveCmdType)(dwID & 0xFFFF)) &&
         (pMe->cmdCache[i].wTransactionID == (dwID >> 16)) )

      {
        dwTransactionID = i;
        break;
      }
    }
  }
  
  OEMSTK_MUTEX_UNLOCK();
  
  if(dwTransactionID >= OEMSTK_MAX_CACHE_ENTRIES)
    return EBADPARM;

  *pBufSize = pMe->cmdCache[dwTransactionID].dwSize;
  
  

  return AEE_SUCCESS;

}

int OEMSTK_GetCachedEntry(OEMSTK *pMe, uint32 dwID, void *pBuf, uint32 nBufSize)
{
  uint32 dwTransactionID = OEMSTK_MAX_CACHE_ENTRIES;
  uint32 i;
  
  if( (pMe == NULL) || (pBuf == NULL))
    return EBADPARM;
 
  OEMSTK_MUTEX_LOCK();
  
  for(i = 0; i < OEMSTK_MAX_CACHE_ENTRIES; i++)
  {
    if(pMe->cmdCache[i].state != OEMSTK_CACHE_STATE_FREE) //Not new and not free.
    {
      if((pMe->cmdCache[i].commandID == (AEESTKProactiveCmdType)(dwID & 0xFFFF)) &&
         (pMe->cmdCache[i].wTransactionID == (dwID >> 16)) )

      {
        dwTransactionID = i;
        break;
      }
    }
  }
  OEMSTK_MUTEX_UNLOCK();

  if(dwTransactionID >= OEMSTK_MAX_CACHE_ENTRIES)
    return EBADPARM;

  if(nBufSize <  pMe->cmdCache[dwTransactionID].dwSize)
    return EBADPARM; // Not enough buffer provided.

  if(pMe->cmdCache[dwTransactionID].pCommand == NULL)
  {
    MSG_ERROR("NULL Buffer found", 0, 0, 0);
    return EBADSTATE;
  }

  (void)MEMCPY(pBuf, pMe->cmdCache[dwTransactionID].pCommand, 
      pMe->cmdCache[dwTransactionID].dwSize);

  //TODO: Set the pointers in the output buffer....
  if(pMe->cmdCache[dwTransactionID].commandType == NMASK_STKNOTIFIER_PROACTIVE_CMD)
  {
    // Need to have different approach for setting up the pointers.
    // And it is untested yet.
    // TODO: Do this actually based on offsets in oldbuf.
    // i.e. ptr = (uint8*) oldbuf
    // int str_offset = (uint8*) old_buf->string.text - ptr;
    // new_buf->string.text = (uint8*) newbuf + str_offset;

    /*OEMSTK_SetAEEOutBufPointers(pMe, dwTransactionID, 
        pMe->cmdCache[dwTransactionID].commandID, pBuf);
    The above function does not work correctly currently..
    Because, menu item with str.length = 0 and str.text = '\0'.. this will fail.

    */
  }
  else
  {
    //Nothing to do currently
  }
  
  //If its an envelope command.. then mark the state as responded.
  if(pMe->cmdCache[dwTransactionID].commandType == NMASK_STKNOTIFIER_ENVELOPE_CMD)
  {
    OEMSTK_CleanupCache(uint32toint32(dwTransactionID));
  }
  return AEE_SUCCESS;
}

int OEMSTK_ChannelStatusChange(OEMSTK *po, AEESTKGetChStatusResponse sChStatus)
{
  gstk_status_enum_type   client_status;
  gstk_evt_dl_ind_type    ch_status;
  if (!gpSTK)
  {
    MSG_ERROR("gpSTK is a NULL ptr",0,0,0);
    return EFAILED;  
  }
  if(gpSTK->bChStatusNotif) 
  {
    /* Send envelope to GSTK */
    ch_status.evt_type = GSTK_CH_STATUS_EVT_IND;
    ch_status.evt_ind.ch_status_evt_ind.ch_id = sChStatus.nChID;
    ch_status.evt_ind.ch_status_evt_ind.is_link_est = sChStatus.bIsLinkEst;
    OEMSTK_ChStatusAddlInfoTableLookUp(OEMSTK_DMSS, 
      &ch_status.evt_ind.ch_status_evt_ind.ch_status_additional_info, 
      &sChStatus.ChStatusAdditionalInfo);  
      client_status = gstk_send_envelope_evt_download_command(
                            cl_id,
                            0,
                            &ch_status);
    if (client_status != GSTK_SUCCESS) 
    {
      return EFAILED;
    }
  }
  return SUCCESS;
} /*lint !e715 */

int OEMSTK_DataAvailable(OEMSTK *po, AEESTKDataAvailNotify sDataAvail)
{
  gstk_status_enum_type   client_status;
  gstk_evt_dl_ind_type    data_avail;
  if (gpSTK) 
  {
    if(gpSTK->bDataAvailNotif) 
    {
      /* Send envelope to GSTK */
      data_avail.evt_type = GSTK_DATA_AVAIL_EVT_IND;
      data_avail.evt_ind.data_avail_evt_ind.ch_len = sDataAvail.nChLen;
      data_avail.evt_ind.data_avail_evt_ind.ch_status.ch_id = sDataAvail.nChID;
      data_avail.evt_ind.data_avail_evt_ind.ch_status.is_link_est = sDataAvail.bIsLinkEst;
      OEMSTK_ChStatusAddlInfoTableLookUp(OEMSTK_DMSS, 
        &data_avail.evt_ind.data_avail_evt_ind.ch_status.ch_status_additional_info, 
        &sDataAvail.ChStatusAdditionalInfo);  
      
      client_status = gstk_send_envelope_evt_download_command(
                            cl_id,
                            0,
                            &data_avail);
      if (client_status != GSTK_SUCCESS) 
      {
        return EFAILED;
      }
    }
  }
  return SUCCESS;
} /*lint !e715 */



/*===========================================================================

FUNCTION: OEMSTK_UserActivity

DESCRIPTION
  Send down User Activity event to the Card upon receiving key events

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EFAILED   - Status information is invalid

SIDE EFFECTS
===========================================================================*/
int OEMSTK_UserActivity(OEMSTK *po)
{
  gstk_status_enum_type   client_status;
  gstk_evt_dl_ind_type    user_act_ind;
  if (!gpSTK)
  {
    MSG_ERROR("gpSTK is a NULL ptr",0,0,0);
    return EFAILED;  
  }
  if(gpSTK->bUserActionNotif) 
  {
    /* Send envelope to GSTK */
    user_act_ind.evt_type =  GSTK_USER_ACTIVITY_EVT_IND;

    client_status = gstk_send_envelope_evt_download_command(
                            cl_id,
                            0,
                            &user_act_ind);
    gpSTK->bUserActionNotif = FALSE;
    if (client_status != GSTK_SUCCESS) 
    {
      return EFAILED;
    }
  }
  return SUCCESS;
} /*lint !e715 */

/*===========================================================================

FUNCTION: OEMSTK_MapFromAEEToGSTKCauseEnum

This function maps the AEESTK browser termination cause to the GSTK cause value

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EFAILED   - Status information is invalid

SIDE EFFECTS
===========================================================================*/
static int OEMSTK_MapFromAEESTKToGSTKCauseEnum (
  AEESTKBrowserTermEnumType sBrowserTerm,
  gstk_browser_term_cause_enum_type *pCause)
{
  int32 i = 0;

  if(!pCause)
    return EFAILED;

  for(i=0;i < OEMSTK_MAX_BT_CAUSE;i++)
  {
    if(OEMSTK_BT_Cause_Map_Table[i].AEESTK_cause == sBrowserTerm)
    {
      *pCause = OEMSTK_BT_Cause_Map_Table[i].gstk_cause;
      return SUCCESS;
    }
  }
  return EFAILED;
}

/*===========================================================================

FUNCTION: OEMSTK_BrowserTermination

DESCRIPTION
  Send down Browser Termination event to the Card upon receiving key events

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EFAILED   - Status information is invalid

SIDE EFFECTS
===========================================================================*/
int OEMSTK_BrowserTermination(OEMSTK *po,AEESTKBrowserTermEnumType sBrowserTerm )
{
  gstk_status_enum_type   client_status;
  gstk_evt_dl_ind_type    browser_term_ind;
  int                     status;

  if(!po)
    return EFAILED;

  if (!gpSTK)
  {
    MSG_ERROR("gpSTK is a NULL ptr",0,0,0);
    return EFAILED;  
  }
  if(gpSTK->bBrowserTermNotif) 
  {
    browser_term_ind.evt_type =  GSTK_BROWSER_TERMINATION_EVT_IND;

    /* Map the browser termination cause value given by AEESTK to GSTK cause 
       value before sending it down to GSTK 
    */
    status = OEMSTK_MapFromAEESTKToGSTKCauseEnum(
                      sBrowserTerm,
                      &browser_term_ind.evt_ind.browser_terminate_evt_ind);
    if(status != SUCCESS) 
    {
      return EFAILED;
    }
    /* Send Browser Termination event envelope to GSTK */
    client_status = gstk_send_envelope_evt_download_command(
                            cl_id,
                            0,
                            &browser_term_ind);
    if (client_status != GSTK_SUCCESS) 
    {
      return EFAILED;
    }
  }
  return SUCCESS;
}

/*===========================================================================
FUNCTION OEMSTK_pack_7_bit_chars

DESCRIPTION
  This function pack the input data into SMS 7 bit format.

PARAMETERS
  in: [Input] Input data pointer
  in_len: [Input] Input data length
  out: [Input/Output] Pointer to output data

DEPENDENCIES
  None

RETURN VALUE
  uint16:length of output data length

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void OEMSTK_pack_7_bit_chars
(
  const uint8      * in,
  uint16             in_len, //number of chars
  gstk_string_type * out
)
{
  uint16    i=0;
  uint16    pos;
  int       shift = 0;

  /* pack the ASCII characters
  */
  if(!in)
  {
    MSG_ERROR("in NULL ptr",0,0,0);
    return;
  }

  if(!out)
  {
    MSG_ERROR("out NULL ptr",0,0,0);
    return;
  }

  for( pos=0;
       i < in_len;
       pos++, i++ )
  {
    /* pack the low bits */
    out->text[pos] = in[i] >> shift;

    if( i+1 < in_len )
    {
      /* pack the high bits using the low bits of the next character */
      out->text[pos] |= in[i+1] << (7-shift);

      shift ++;

      if( shift == 7 )
      {
        shift = 0;
        i ++;
      }
    }
  }

  /* done */
  out->length = uint16touint8(pos);

} /* OEMSTK_pack_7_bit_chars */

/*===========================================================================
FUNCTION OEMSTK_StringConversion

DESCRIPTION
  This function pack the input data into SMS 7 bit format.

PARAMETERS
  in: [Input] Input data pointer
  in_len: [Input] Input data length
  out: [Input/Output] Pointer to output data

DEPENDENCIES
  None

RETURN VALUE
  uint16:length of output data length

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/

void OEMSTK_StringConversion(AEESTKString *data,
        uint8             dcs,
        gstk_string_type *str_data) 
{
  uint8 *tmp_str = NULL;
  uint32 data_len = 0;

  if(!data)
  {
    MSG_ERROR("data NULL ptr",0,0,0);
    return;
  }
  if(!str_data)
  {
    MSG_ERROR("str_data NULL ptr",0,0,0);
    return;
  }

  data_len = int32touint32(WSTRLEN(data->pwszText));

  if(dcs == AEESTK_7_BIT_DCS)
  {
    tmp_str = MALLOC(data_len+1);
    if(tmp_str == NULL) {
      ERR("Not able to memalloc", 0, 0, 0);
      /* set the gstk string type len to 0 to avoid copying garbbage in gstk */
      str_data->length = 0;
      return;
    }
    (void)MEMSET(tmp_str, 0x00, data_len+1);
    (void)WSTRTOSTR(data->pwszText, (char*)tmp_str, uint32toint32(data_len+1));
    OEMSTK_pack_7_bit_chars(tmp_str, uint32touint16(data_len), str_data);
    str_data->dcs = AEESTK_7_BIT_DCS;
  }
  else
  {
    if(dcs == AEESTK_UCS2_DCS)
    {
      OEMSTK_DCSSpc_AEEStringToGSTKString(data, str_data, AEESTK_UCS2_ALPHABET);
    }
    else
    {
      OEMSTK_DCSSpc_AEEStringToGSTKString(data, str_data, AEESTK_SMS_DEF_ALPHABET);
    }
  }
  FREEIF(tmp_str);
}
/*===========================================================================

FUNCTION: OEMSTK_LangSelection

DESCRIPTION
   Called by App to change in current langauage selection which is send down to 
   the Card by GSTK

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EFAILED   - Status information is invalid

SIDE EFFECTS
===========================================================================*/
int OEMSTK_LangSelection(
  OEMSTK *po,  
  AEESTKLangSelectionResponse *sLangSelection
)
{
  gstk_status_enum_type   client_status;
  gstk_evt_dl_ind_type    lang_sel_ind;
  if(!sLangSelection)
  {
    MSG_ERROR("sLanSelection is a NULL ptr",0,0,0);
    return EBADPARM;
  }
  if (!gpSTK)
  {
    MSG_ERROR("gpSTK is a NULL ptr",0,0,0);
    return EFAILED;  
  }
  
  if(gpSTK->bLangSelectionNotif)
  {
    /* Send envelope to GSTK */
    lang_sel_ind.evt_type =  GSTK_LANG_SELECTION_EVT_IND;
    if(sLangSelection->len == GSTK_LANG_CODE_LEN)
    {
      memcpy(lang_sel_ind.evt_ind.lang_sel_evt_ind.lang_code,
               sLangSelection->sLangCode,GSTK_LANG_CODE_LEN);
    }
    else
    {
      return EFAILED;
    }

    client_status = gstk_send_envelope_evt_download_command(
                              cl_id,
                              0,
                              &lang_sel_ind);
    if (client_status != GSTK_SUCCESS)
    {
      return EFAILED;
    }
  }
  else
  {
    /* If language selection notification is not set -- return EFAILEd */
    return EFAILED;
  }
  return SUCCESS;
}
/*===========================================================================

FUNCTION: OEMSTK_IdleScreen

DESCRIPTION
   Called by App to report Idle screen event which is send down to the Card
   by GSTK

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - Status information is valid
  EFAILED   - Status information is invalid

SIDE EFFECTS
===========================================================================*/
int OEMSTK_IdleScreen(OEMSTK *po)
{
  gstk_status_enum_type   client_status;
  gstk_evt_dl_ind_type    idle_scrn_ind;
  if (gpSTK)
  {
    if(gpSTK->bIdleScreenNotif)
    {
      /* Send envelope to GSTK */
      idle_scrn_ind.evt_type =  GSTK_IDLE_SCRN_EVT_IND;

      client_status = gstk_send_envelope_evt_download_command(
                              cl_id,
                              0,
                              &idle_scrn_ind);
      gpSTK->bIdleScreenNotif = FALSE;
      if (client_status != GSTK_SUCCESS)
      {
        return EFAILED;
      }
    }
  }
  return SUCCESS;
}

/*===========================================================================
FUNCTION: OEMSTK_CalculateSizeForGSTKTermRsp

DESCRIPTION
   Calculates size for GSTK terminal response structure based on the AEESTK
   response received from the client.

PARAMETERS
  nSize     - [Input/Output] pointer to size object
  commandID - command ID of the Proactive cmd.
  pIn       - pointer to the response structure

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS
  EBADPARM

SIDE EFFECTS
===========================================================================*/
static int OEMSTK_CalculateSizeForGSTKTermRsp(
  uint32                 * nSize,
  AEESTKProactiveCmdType commandID,
  const void             * pIn)
{
  if(!nSize || !pIn)
  {
    MSG_ERROR("OEMSTK_CalculateSizeForGSTKTermRsp : NULL ptr",0,0,0);
    return EBADPARM;
  }
  switch(commandID)
  {
    case AEESTK_GET_INPUT_REQ:
    {
      AEESTKGetInputResponse *pInputRsp = (AEESTKGetInputResponse*) pIn;
      if(pInputRsp->pString.pwszText != NULL)
      {
        *nSize += int32touint32(pInputRsp->pString.nLength * sizeof(AECHAR));
      }
    }
    break;
  default:
    break;
  }
  return SUCCESS;
}
/*===========================================================================
FUNCTION: OEMSTK_SetPointerPositionForGSTKRsp

DESCRIPTION
  Sets the GSTK Terminal Rsp pointer position to point to continous segments
  of memory

PARAMETERS
  commandID - command ID of the Proactive cmd.
  pIn       - pointer to the response structure
  pOut      - address of GSTK Term Rsp ptr

DEPENDENCIES
  none

RETURN VALUE
  gstk_terminal_rsp_extra_param_type - GSTK estra parm type based on the
  Proactive command.

SIDE EFFECTS
===========================================================================*/
static int OEMSTK_SetPointerPositionForGSTKRsp(
  AEESTKProactiveCmdType             commandID,
  const void                         * pIn,
  gstk_terminal_rsp_extra_param_type **pOut)
{
  uint8 *ptr = NULL;

  if(!pIn || !pOut || !(*pOut))
  { 
    MSG_ERROR("OEMSTK_SetPointerPositionForGSTKRsp : NULL ptr",0,0,0);
    return EBADPARM;
  }

  ptr = *((void **)pOut);
  ptr += sizeof(gstk_terminal_rsp_extra_param_type);

  switch(commandID)
  {
    case AEESTK_GET_INPUT_REQ:
    {
      AEESTKGetInputResponse *pInputRsp = (AEESTKGetInputResponse*) pIn;

      if(pInputRsp->pString.pwszText != NULL)
      {
        (*pOut)->extra_param.user_get_input.text = ptr;
        ptr += pInputRsp->pString.nLength * sizeof(AECHAR);
      }
    }
    break;
  default:
    break;
  }
  return SUCCESS;
}
/*===========================================================================
FUNCTION: OEMSTK_ConvertAEESTKRspToGSTKRsp

DESCRIPTION
   Converts AEESTK Extra Param to GSTK Extra parm. This is different from 
   OEMSTK_AEEExtraParmToGSTKExtraParm() as it considers a new response 
   structure for Get Input Command.

PARAMETERS
  commandID - command ID of the Proactive cmd.
  pIn       - pointer to the response structure

DEPENDENCIES
  none

RETURN VALUE
  gstk_terminal_rsp_extra_param_type - GSTK estra parm type based on the
  Proactive command.

SIDE EFFECTS
===========================================================================*/
static int OEMSTK_ConvertAEESTKRspToGSTKRsp(
  AEESTKProactiveCmdType               commandID,
  const void                         * pIn,
  gstk_terminal_rsp_extra_param_type **pOut)
{
  if(!pIn || !pOut || !(*pOut))
  { 
    MSG_ERROR("OEMSTK_ConvertAEESTKRspToGSTKRsp : NULL ptr",0,0,0);
    return EBADPARM;
  }
  switch(commandID)
  {
  case AEESTK_GET_INPUT_REQ:
    {
      AEESTKGetInputResponse *pInputRsp = (AEESTKGetInputResponse*) pIn;
      OEMSTK_AEEGetInputResponseToGSTKString(pInputRsp,
                                             &((*pOut)->extra_param.user_get_input));
      (*pOut)->rsp_type = GSTK_GET_INPUT_CNF;
    }
    break;
  default:
    break;
  }
  return SUCCESS;
}
/*===========================================================================
FUNCTION: OEMSTK_AEEExtraParmToGSTKExtraParmEx

DESCRIPTION
   Converts AEESTK Extra Param to GSTK Extra parm. This is different from 
   OEMSTK_AEEExtraParmToGSTKExtraParm() as it considers a new response 
   structure for Get Input Command.

PARAMETERS
  commandID - command ID of the Proactive cmd.
  pIn       - pointer to the response structure

DEPENDENCIES
  none

RETURN VALUE
  gstk_terminal_rsp_extra_param_type - GSTK estra parm type based on the
  Proactive command.

SIDE EFFECTS
===========================================================================*/
gstk_terminal_rsp_extra_param_type* OEMSTK_AEEExtraParmToGSTKExtraParmEx(
  AEESTKProactiveCmdType commandID,
  void                   * pIn)
{
  uint32                              nSize        = 0;
  gstk_terminal_rsp_extra_param_type *pOut         = NULL;

  nSize = sizeof(gstk_terminal_rsp_extra_param_type);

  /* Note: pIn may be NULL-Certain commands like Display Text have pIn as NULL */
  if(NULL == pIn)
  {
    pOut = (gstk_terminal_rsp_extra_param_type*) MALLOC(nSize);
    if(NULL == pOut)
    {
      MSG_ERROR("Malloc Failed", 0, 0, 0);
    }
    else
    {
      pOut->present = FALSE;
    }
    return pOut;
  }

  /* Step 1: Caculate size to allocate memory */
  if(OEMSTK_CalculateSizeForGSTKTermRsp(&nSize,commandID,pIn) == EBADPARM)
  {
    return NULL;
  }

  pOut  = (gstk_terminal_rsp_extra_param_type*) MALLOC(nSize);
  if (NULL == pOut) 
  {
    return NULL;
  }
  /* Step 2: Set the pointers to the appropriate memory location to ensure member pointers
     are allocated adjacent to one another..
  */
  if(OEMSTK_SetPointerPositionForGSTKRsp(commandID,pIn,&pOut) == EBADPARM)
  {
    pOut->present = FALSE;
    return pOut;
  }
  pOut->present = TRUE;

  if(OEMSTK_ConvertAEESTKRspToGSTKRsp(commandID,pIn,&pOut) == EBADPARM)
  {
    pOut->present = FALSE;
    return pOut;
  }
  return pOut;
}
/*===========================================================================

FUNCTION: OEMSTK_SendCommandResponseEx

DESCRIPTION
   This function adds to the functionality of OEMSTK_SendCommandResponse()
   by allowing the client to send in specific details of the proactive command 
   request  in the new response structures that is required to pack information
   to GSTK.

PARAMETERS
  pMe,               - ponter to OEMSTK object
  term_resp_e        - Cmd request type.
  cmd_num            - Command number of the proactive command
  result             - result of the execution of the Proactive command
  additional_result  - Additional info to the result
  extra_param        - Extra parameters (includes any user responses etc)
    AEESTKCallSSResponse 
    AEESTKCallSSResponse
    AEESTKUSSDResponse
    AEESTKUSSDResponse
    AEESTKInkeyResponseType
    AEESTKInkeyResponse
    AEESTKGetInputResponse
    AEESTKOpenChResponse
    AEESTKReceiveDataResponse
    AEESTKGetChStatusResponse
    AEESTKLangSelResponse

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS   - successfully sending to gstk
  EFAILED  -  Failure in sending to gstk


SIDE EFFECTS
===========================================================================*/
int OEMSTK_SendCommandResponseEx(
              OEMSTK                     *pMe,
              AEESTKProactiveCmdType     request_type,
              uint8                      cmd_num,
              AEESTKResult               result,
              AEESTKAdditionalInfo       *additional_result,
              void                       *extra_param)
{

  gstk_status_enum_type              client_status;
  gstk_cmd_enum_type                 resp;
  gstk_general_result_enum_type      general_result;
  gstk_additional_info_ptr_type      *pAdditionalInfo = NULL;
  gstk_terminal_rsp_extra_param_type *pExtraParm      = NULL;
  int32                               index           = 0;
  
  if(!pMe)
  {
    MSG_ERROR("pMe NULL ptr",0,0,0);
    return EBADPARM;
  }

  /* Convert the event to confirmation type. */
  resp = OEMSTK_GetConfirmationType(request_type);

  if(OEMSTK_FindCommandIdInCache(pMe,request_type, cmd_num, &index) == EBADPARM)
  {
    MSG_ERROR("Failed looking up the command_id", 0, 0 , 0);
    return EBADPARM;
  }

  pAdditionalInfo = OEMSTK_AEEAddnlInfoToGSTKAddnlInfo(additional_result);

  if(!pAdditionalInfo)
  {
    MSG_ERROR("pAdditionalInfo ptr is NULL",0, 0, 0);
    return EFAILED;
  }

  pExtraParm      = OEMSTK_AEEExtraParmToGSTKExtraParmEx(request_type, 
                                                         extra_param);
  general_result = OEMSTK_AEEResultToGSTKResult(result);

  client_status = gstk_send_terminal_response_ext( cl_id,
                        pMe->cmdCache[index].user_data,
                        pMe->cmdCache[index].cmd_detail_reference,
                        cmd_num,
                        resp,
                        general_result,
                        pAdditionalInfo,
                        pExtraParm);

  /* Free the additional info data after successfully queuing the information in the gstk queue */
  FREEIF(pAdditionalInfo->additional_info_ptr);
  pAdditionalInfo->additional_info_ptr = NULL;
  FREE(pAdditionalInfo);
  pAdditionalInfo = NULL;
  FREE(pExtraParm);
  pExtraParm = NULL;

  if (GSTK_SUCCESS == client_status)
  {
    OEMSTK_CleanupCache(index);
    return SUCCESS;
  }
  else {
    MSG_ERROR("GSTK rejects Terminal Response, Status = 0x%x", client_status, 0, 0);
    MSG_ERROR("Result: 0x%x, Cmd_ID: 0x%x, Cmd_Num", result, resp, cmd_num);
    return EFAILED;
  }
}
/*===========================================================================*/

