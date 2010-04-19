/*=============================================================================

FILE: FieldDebug.c

SERVICES: Hidden Menu applet

GENERAL DESCRIPTION:
   N/A

PUBLIC CLASSES AND STATIC FUNCTIONS:
   FieldDebug

INITIALIZATION AND SEQUENCING REQUIREMENTS:
   N/A

(c) COPYRIGHT 2002-2007 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
=============================================================================*/


/*=============================================================================

  $Header: //depot/asic/msmshared/apps/FldDbg/FldDbgApp.c#96 $
$DateTime: 2008/08/31 21:54:10 $

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ----------------------------------------------------------
08/26/08     Ravi      Removed all the deprecated function 
12/03/07     clm     Replace OEMFS funcs with OEMefs funcs for brew 4.0
09/05/07     yc      Increase the buffer size in _MMCFormatHandleEvent() to 
                     prevent display message from being truncated.
09/06/07     cvs     Don't require apps be active for diag stop
07/12/07     jas     Fixing use of App_New for BREW 4.0
07/05/07     cvs     Fix featurization
01/15/07     jas     Launch in the background when shell init notif arrives.
12/06/06     cvs     Remove banned APIs
11/14/06     jas     Adjusting abs macro to use BREW-supplied ABS
09/18/06     cvs     Integrate brew 4.0 changes from BTG.
08/22/06     sg/jas  Added background mode for MMC download
08/15/06     jas     Adding MMC download time selection
07/26/06     jas     Fixing a compiler warning
07/25/06     jas     Another compiler warning.
07/12/06     jas     Cleaning up compiler warnings for RVCT
06/12/06     JD      Fixed whitespace merging errors.
05/24/06     JD      Added call to SelectFonts to increase size on VGA
05/12/06     cvs     update featurizations
05/03/06     jas     Fixing lint directives
04/17/06     jas     Fixing featurzation of secondary chain Rx variables
03/31/06     ac      Added support for QIPCALL debug info
03/20/06     jas     Fixing featurization of DRM test app
03/14/06     jas/bn  Add secondary chain Rx values for 6280
03/08/06     jas/ch  Updating params to the "start app" diag for BREW 3.1.5
02/17/06     cvs     Adding menu to make game mode persistent
12/01/05     jas     Fixing menu navigation for top menu
12/01/05     rmd     Added support for Band Class 14.
11/15/05     jas     New menu to turn of display while in call
10/15/05     sada    Added new diags: Get Active Applet, Get App Name & Get Selected Menu Ctl Text
10/12/05     JAS     Fixed JPEG decode problem
08/02/05     AT      Correct featurizations.
07/28/05     AT      Remove unused variable.
07/15/05     AT      Added API to Reset Origination Throttling.
07/11/05     AT      Added back Remote API Test Support.
06/20/05     bmg     Added IntelliCeiver state
05/16/05     sada    Added CFieldDebug_ToggleStrideHandleEvent event 
                     handler for Stride menu item. Also converted 
                     resource file to .brx
04/29/05     jas     Fixing case of include files for Linux builds
04/07/05     cvs     Fixed featurization
03/09/05     sun     Fixed Compile Errors
03/07/05     bn      Removed GSM RSSI call in CFieldDebug_DebugScreenReadItem()
02/16/05     sun     Go directly to top dialog in FTM 
01/31/05     sun     Added menu to Enable/disable USB charging
12/04/04     sada    Rerouted Get_ActiveDlgId to Field Debug APP
11/24/04     sada    Changed initialization of parameter sAppStartInfo in Evt_Flddbg_App_Start
11/16/04     ak      Mainline USB wakeup support
10/30/04     bhas    Debug Screen support for UMTS targets
10/14/04     sada    Rerouted APP_SEND_EVENT to Field Debug APP to support BREW 3.1
09/17/04     dyc     Updated for MSM6500 RF Card/Band info
08/30/04     JAS     Moved app diags into Field Debug App
08/16/04     NNG     Changes for Voice Debug.
07/23/04     NNG     Added support for DRM test app.
06/03/04     NNG     Fixed compiler error for 6250.
05/17/04     ak      Added HDR Debug Info
05/13/04     NNG     Fixed compiler error.
05/12/04     NNG     Removed IDShift for brew 3.0.
03/31/04     dyc     RxAGC 2nd chain support for MSM6500.
                     Use int32 for CFGI_DEBUG_RX_AGC_IN_DBM item.
03/26/04     AT      Adding Remote API Test Support
03/19/04     sun     Added Support for Calmon
03/04/04     dyc     Update for field debug support
02/25/04     ram     Read and assign port map for USB_DIAG and NMEA_DIAG
                     for MMC download.
02/13/04     ram     Added Security Services menu and its subordinate menus:
                     SSL, SEC, DRM, and VPN.
02/13/04     NNG     Added JPEG decoder support.
02/03/04     ram     Add AMR menu.
01/07/04     ram     Add MMC Download functionality and End Key handling.
12/04/03     ram     Fix compile error.
11/19/03     BPW     Do not return to idle on exit and added null for a string
11/13/03     ram     Added clear fplmn functionality.
                     Merged Anand's changes for BREW3.0.
11/13/03     PA      Support for ICallOpts
10/28/03     kar     Added support for MMC format and download menus. Implemented
                     test calls.
10/23/03     PA      Integrated with latest ICM
10/17/03     ram     Increased the buffer for reading debug-screen items and
                     changed the signature for DebugScreenReadItem.
10/16/03     kar     Made app backwards compatible with non ICM targets
09/30/03     tml     Reflected changes in ICM API
09/25/03     tml     Remove UI dependencies, add ICM supports
08/04/03     kar     Featurization changes for Gigatest app
06/17/03     kar     Support for GIGA test app
05/07/03     kar     Fixed compile errors on UMTS
05/05/03     jkl     Added call to efs_reset.
04/10/03     kar     Fixed compile error on WCDMA builds
03/06/03     kar     Disabled clear EFS option for EFS2 builds
01/31/03     kar     Initial revision

=============================================================================*/
//lint -save -e611   Suppress "Suspicious cast"
//lint -save -e740   Suppress "Unusual pointer cast"


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"         //Handset Feature Definitions
/* Include AEEComdef.h before comdef.h, so that definitions
 * in the comdef.h take precedence.  (i.e. behavior is unchanged
 * for macros defined in comdef.h.)
 */
#include "AEEComdef.h"
#include "comdef.h"

#include "AEE.h"
#include "AEE_OEM.h"

#include "AEEShell.h"
#include "AEEMenu.h"
#include "AEEModTable.h"
#include "AEEStdLib.h"
#include "AEEDB.h"
#include "flddbg_res.h"
#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "AppComFunc.h"
#include "AEEText.h"
#include "OEMClassIDs.h"
#include "AEEFile.h"
#include "AEETime.h"
#include "FldDbgApp.h"
#include "BREWVersion.h"
#include "AEEConfig.h"
#include "OEMCFGI.h"

#include "AEEClipboard.h"

#include "msg.h"
#include "err.h"

#if MIN_BREW_VERSION(3, 1)
#include "AEEAppletCtl.h"
#endif

#if MIN_BREW_VERSION(3,0)
// For BREW 3.0
#include "AEE_OEMDispatch.h"
#endif

#if defined (FEATURE_CDMA1X)
#include "cai.h"
#endif

#ifdef FEATURE_ICM
#include "AEECM.h"
#else
#include "uixcm.h"
#endif

#include "fs.h"
#ifdef FEATURE_EFS2
#include "fs_public.h"
#endif

#include "SVCPRGAPP.BID"
#include "FLDDBGAPP.BID"

#ifdef FEATURE_APP_GIGAMON
#include "gigamon.bid"
#endif

#ifdef FEATURE_GRAPHICS_APP_CALMON
#include "calmon.bid"
#endif

#ifdef FEATURE_MMGSDI
#include "gsdi_exp.h"
#endif

#include "rdevmap.h"
#include "cmx.h"

#ifdef FEATURE_SEC_TESTAPPS
#include "sectestapp.h"
#endif/* FEATURE_SEC_TESTAPPS */

#ifdef FEATURE_APP_TEST_AUTOMATION
#include "AEEDiag.h"
#include "Diag.bid"
#endif

#ifdef FEATURE_SEC_DRM
extern void DRMUnitTestApp( IShell* pIShell );
extern void DRMTestApp( IShell* pIShell );
#endif /* FEATURE_SEC_DRM */

#ifdef FEATURE_SEC_IPSEC_VPN_CLNT
#include "secipsvpnclnt.h"
#endif /* FEATURE_SEC_IPSEC_VPN_CLNT */

#include "AEERats.h"

#ifdef FEATURE_USB_WAKEUP
#include "usbdc.h"
#endif

#if defined(FEATURE_BATTERY_CHARGER) && defined(FEATURE_PMIC_USB_OTG)
#include "charger.h"
#endif

#ifdef srIMON
#include "OEMInterceptModules.h"
#endif

#ifdef FEATURE_JPEG_DECODER
#if MIN_BREW_VERSION(4,0)
#include "OEMefs.h"
#else
#include "OEMFS.h"
#endif //MIN_BREW_VERSION(4,0)
#include "AEEStdLib_static.h"
#endif

/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/

// wParam for an EVT_COMMAND event for displaying
// a screen during Clear EFS
#define DISP_TIMEOUT_WPARAM      1019

// Periodicity of refresh for Debug Screen in milliseconds
#define DEBUG_SCRN_REFRESH_PERIOD     200 /* 200msec */

// Time in milliseconds the message 'Clearing...' will stay up
// during Clear Errors before closing
#define SHOW_MESSAGE_TIMEOUT_MS       (2*1000)

#define MAX_IP_SIZE     15   /* Size of an IP address string. */
#define MAX_TEST_DIGITS 8    /* Size of the test number of digits. */


// EVT_USER type events
#define EVT_SIM_WRITTEN_FINAL 0x0001
#define EVT_SIM_WRITTEN       0x0002
#define EVT_GET_FILE_ATTR     0x0004
#define EVT_RDM_STATUS        0x0008
#define EVT_MMC_DL_CLOSED     0x0010

#ifdef FEATURE_APP_TEST_AUTOMATION

// Diagnostic registration structure
/* The following array defines the events that need to be registered
 * with the IDiag object, along with their attributes.
 */
static IDiagEvtReg CFldDbgApp_DiagEventReg[] =
{
    //EventId,                               UsesPayload,    UsesDelayedResponse
    {EVT_FLDDBG_DIAG_APP_GET_ID,             TRUE,           TRUE},
    {EVT_FLDDBG_DIAG_APP_START,              TRUE,           TRUE},
    {EVT_FLDDBG_DIAG_APP_STOP,               TRUE,           TRUE},
    {EVT_FLDDBG_DIAG_APP_SEND_EVENT,         TRUE,           TRUE},
    {EVT_FLDDBG_DIAG_GETACTIVEDLG,           TRUE,           TRUE},
    {EVT_FLDDBG_DIAG_GETSELMENUCTLTXT,       TRUE,           TRUE},
    {EVT_FLDDBG_DIAG_APP_GET_NAME,           TRUE,           TRUE},
    {EVT_FLDDBG_DIAG_GETACTIVEAPPLET,        FALSE,          TRUE},
};

// Calculate the number of events being registered
#define FLDDBGAPP_DIAG_EVENT_REG_SIZE    ARR_SIZE(CFldDbgApp_DiagEventReg)

#endif // FEATURE_APP_TEST_AUTOMATION

#define RDM_STATUS_INIT1       0x0001
#define RDM_STATUS_INIT2       0x0002
#define RDM_STATUS_INIT3       0x0004
#define RDM_STATUS_INIT4       0x0008

#define RDM_STATUS_EXIT1       0x0010
#define RDM_STATUS_EXIT2       0x0020
#define RDM_STATUS_EXIT3       0x0040
#define RDM_STATUS_EXIT4       0x0080

#define MMC_DOWNLOAD_POLL_TIME 1000

#define IPFILE                 "api_test_ip.dat" /* Name of the database for IP address. */

#ifdef FEATURE_JPEG_DECODER
#define MEDIA_DIR "media"
#define EVT_JPEG_DECODE_DONE 10000
dword * m_bufJPEG = NULL;
#endif /* FEATURE_JPEG_DECODER */

/* MMC download periods in msec */
#define FLDDBG_MIN_TO_MSEC(x) ((x) * 60 * 1000)
#define FLDDBG_MMC_DL_TIME_5_MIN           FLDDBG_MIN_TO_MSEC(5)
#define FLDDBG_MMC_DL_TIME_30_MIN          FLDDBG_MIN_TO_MSEC(30)
#define FLDDBG_MMC_DL_TIME_UNLIMITED       0

/* The following defines indicate the MMC download state. */
// Not in MMC Download; not going to MMC download
#define FLDDBG_MMC_DOWNLOAD_IDLE             0
// Enterint MMC Download, but not background
#define FLDDBG_MMC_DOWNLOAD_GO_DL            1
// Entering MMC Download background mode
#define FLDDBG_MMC_DOWNLOAD_GO_DL_BG         2
// In MMC Download, not background
#define FLDDBG_MMC_DOWNLOAD_IN_DL            3
// In MMC Download, background mode
#define FLDDBG_MMC_DOWNLOAD_IN_DL_BG         4
// Exiting MMC Download mode
#define FLDDBG_MMC_DOWNLOAD_EXITING          5

#ifdef FEATURE_HDR
typedef enum
{
  HDRDEBUG_HDRSRCH_INACTIVE_STATE        = 0x00,
    /* Searcher is idle ... */

  HDRDEBUG_HDRSRCH_ACQ_STATE             = 0x01,
    /* Attempting to acquire a system */

  HDRDEBUG_HDRSRCH_SYNC_STATE            = 0x02,
    /* Acquired ... waiting for SYNC message */

  HDRDEBUG_HDRSRCH_IDLE_STATE            = 0x03,
    /* Idle state, included monitoring control channel, access attempts */

  HDRDEBUG_HDRSRCH_SUSPENDED_IDLE_STATE  = 0x3a,
    /* Idle state, included monitoring control channel, access attempts */

  HDRDEBUG_HDRSRCH_OFS_IDLE_STATE        = 0x3c,
    /* Off frequency neighbour searching */

  HDRDEBUG_HDRSRCH_SLEEP_STATE           = 0x3e,
    /* Sleep and Reacquisition after sleep*/

  HDRDEBUG_HDRSRCH_REACQ_STATE           = 0x3f,
    /* Reacquisition after sleep*/

  HDRDEBUG_HDRSRCH_CONNECTED_STATE       = 0x04,
    /* Traffic */

  HDRDEBUG_HDRSRCH_SUSPENDED_TC_STATE    = 0x4a,
    /* Traffic Suspended */

  HDRDEBUG_HDRSRCH_OFS_TC_STATE          = 0x4c,
    /* Traffic Suspended */

  HDRDEBUG_HDRSRCH_NO_STATE              = 0xFF
    /* Start / Terminate HDR operation */
}hdrdebug_srch_state;

typedef enum
{
  HDRDEBUG_SLEEP_MODE_SCC = 0x00,
  HDRDEBUG_SLEEP_MODE_CCC,
  HDRDEBUG_SLEEP_MODE_LONG,
  HDRDEBUG_SLEEP_MODE_URH
}hdrdebug_sleep_mode;

typedef enum
{
  HDRDEBUG_RX_DIV_RX0_ONLY = 0x00,
  HDRDEBUG_RX_DIV_BOTH
}hdr_debug_rx_div;

typedef enum
{
  HDRDEBUG_SESSION_STATE_CLOSE     = 0, /* Inactive State */
  HDRDEBUG_SESSION_STATE_AMP_SETUP = 1, /* AMP Setup State */
  HDRDEBUG_SESSION_STATE_AT_INIT   = 2, /* AT initiated State */
  HDRDEBUG_SESSION_STATE_AN_INIT   = 3, /* AN initiated State */
  HDRDEBUG_SESSION_STATE_OPEN      = 4  /* Open state */
}hdrdebug_session_state;

typedef enum
{
  HDRDEBUG_PROT_STATE_INACTIVE  = 0, /* Inactive State */
  HDRDEBUG_PROT_STATE_ACQ       = 1, /* Acquistion */
  HDRDEBUG_PROT_STATE_SYNC      = 2, /* Sync */
  HDRDEBUG_PROT_STATE_IDLE      = 3, /* Idle */
  HDRDEBUG_PROT_STATE_ACCESS    = 4, /* Access */
  HDRDEBUG_PROT_STATE_CONNECTED = 5  /* Connected */

}hdrdebug_prot_state;
#endif

#ifdef FEATURE_QIPCALL
#define QIPCALL_DEBUG_MAX_IP_ADDR_STR_LENGTH   16
#endif

/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/


typedef struct _CFieldDebug {
   AEEApplet    a;
   IConfig     *m_pIConfig;
   uint16       m_debugmenu_sel;
   uint16       m_dlgID;
   IRats       *m_pIRats;        //Pointer to remote API test object.
   AEERect      m_screen_rc;     //Rectangle for displaying image.
   int          m_LastFileLocation;  //Last number of bytes read.
   int          m_LastLineNumber;    //Last Line Number found.
#ifdef FEATURE_MMGSDI
   // FPLMN related variables
   uim_items_enum_type  m_sim_filename;
   gsdi_card_mode_capability_T m_card_mode;
#endif
   rdm_device_enum_type old_port_map;
   rdm_device_enum_type old_port_map1;
   rdm_device_enum_type old_port_map2;

   dword nCount1, nCount2;
   uint16 rdmState;
   boolean bEndKeyProcess;

#ifdef FEATURE_JPEG_DECODER
   IDIB * m_pIDIB;          //DIB for displaying JPEG data.

   boolean m_bDisplayImage; //Which screen we are currently on?
   int x_offset;            //X offset
   int y_offset;            //Y offset
   AEERect m_rc;            //Rectangle for displaying image.
#endif /* FEATURE_JPEG_DECODER */

#if defined(FEATURE_GSM) || defined (FEATURE_WCDMA)
#ifdef FEATURE_VOICE_FIXED_PATTERN_DEBUG
   boolean m_bVoiceDebug;
#endif /* FEATURE_VOICE_FIXED_PATTERN_DEBUG */
#endif /* defined(FEATURE_GSM) || defined (FEATURE_WCDMA) */

   AEECallback m_AeeCallback;      // Callback structure for all callback init
#ifdef FEATURE_APP_TEST_AUTOMATION
   IDiag *m_pIDiag;            // pointer to the IDiag object
#if MIN_BREW_VERSION(3, 1)
   IAppletCtl *m_pIAppletCtl;  // pointer to the applet control object
#else
   CFldDbg_CloseAppCBData m_sCloseCBData;  // Data for the close app callback
#endif // MIN_BREW_VERSION(3, 1)
  
   CFldDbg_GetActiveDlgCBData m_sGetActiveDlgCBData; // Data for the get active Dlg callback
   CFldDbg_GetMenuCtlTxtCBData m_sGetMenuCtlTxtCBData; // Data for the get selected menu ctl txt callback

   CFldDbg_CallbackTimerData m_sCallbackTimerData;
#endif // FEATURE_APP_TEST_AUTOMATION

   int16      m_wStatusResID;  // Resource to load into status dialog.

   int16      m_nEndDlgId;     // Dialog to be ended; used by a callback.

   int32      m_nMMCDLTime;    // Length of MMC Download time in msec

   int        m_nMMCDLState;   // MMC Download mode state

} CFieldDebug;



/*===========================================================================

                       FUNCTION DECLARATIONS

===========================================================================*/

static boolean CFieldDebug_HandleEvent(CFieldDebug *pme,
                                    AEEEvent  eCode,
                                    uint16    wParam,
                                    uint32    dwParam);
static boolean CFieldDebug_InitDialog(CFieldDebug *pme,
                                   IDialog *dlg);
static void CFieldDebug_Free(CFieldDebug *pme);
static boolean CFieldDebug_TopMenuHandleEvent(CFieldDebug *pme,
                                           AEEEvent  eCode,
                                           uint16    wParam,
                                           uint32    dwParam);
static boolean CFieldDebug_DebugMenuHandleEvent(CFieldDebug *pme,
                                             AEEEvent  eCode,
                                             uint16    wParam,
                                             uint32    dwParam);
static boolean CFieldDebug_DebugScreenHandleEvent(CFieldDebug *pme,
                                               AEEEvent  eCode,
                                               uint16    wParam,
                                               uint32    dwParam);
static boolean CFieldDebug_TestCallHandleEvent(CFieldDebug *pme,
                                            AEEEvent  eCode,
                                            uint16    wParam,
                                            uint32    dwParam);
static boolean CFieldDebug_ErrorListHandleEvent(CFieldDebug *pme,
                                             AEEEvent  eCode,
                                             uint16    wParam,
                                             uint32    dwParam);
static boolean CFieldDebug_ClearErrorsHandleEvent(CFieldDebug *pme,
                                               AEEEvent  eCode,
                                               uint16    wParam,
                                               uint32    dwParam);
static boolean CFieldDebug_ClearEFSHandleEvent(CFieldDebug *pme,
                                            AEEEvent  eCode,
                                            uint16    wParam,
                                            uint32    dwParam);
static boolean CFieldDebug_DataModeHandleEvent(CFieldDebug *pme,
                                            AEEEvent  eCode,
                                            uint16    wParam,
                                            uint32    dwParam);
static boolean CFieldDebug_SMSMOSOHandleEvent(CFieldDebug *pme,
                                           AEEEvent  eCode,
                                           uint16    wParam,
                                           uint32    dwParam);
static boolean CFieldDebug_VoicePrivHandleEvent(CFieldDebug *pme,
                                             AEEEvent  eCode,
                                             uint16    wParam,
                                             uint32    dwParam);
static boolean CFieldDebug_VersionMenuHandleEvent(CFieldDebug *pme,
                                               AEEEvent  eCode,
                                               uint16    wParam,
                                               uint32    dwParam);
static boolean CFieldDebug_MMCHandleEvent(CFieldDebug *pme,
                                               AEEEvent  eCode,
                                               uint16    wParam,
                                               uint32    dwParam);
static boolean CFieldDebug_MMCDownloadHandleEvent(CFieldDebug *pme,
                                               AEEEvent  eCode,
                                               uint16    wParam,
                                               uint32    dwParam);
static boolean CFieldDebug_SelMMCDLTimeHandleEvent(CFieldDebug *pme,
                                               AEEEvent  eCode,
                                               uint16    wParam,
                                               uint32    dwParam);
static boolean CFieldDebug_MMCFormatHandleEvent(CFieldDebug *pme,
                                               AEEEvent  eCode,
                                               uint16    wParam,
                                               uint32    dwParam);

static boolean CFieldDebug_FPLMNClearHandleEvent(CFieldDebug *pme,
                                               AEEEvent  eCode,
                                               uint16    wParam,
                                               uint32    dwParam);

static boolean CFieldDebug_ErrorDialogHandleEvent(CFieldDebug *pme,
                                               AEEEvent  eCode,
                                               uint16    wParam,
                                               uint32    dwParam);
static boolean CFieldDebug_AMRPrefDialogHandleEvent(CFieldDebug *pme,
                                               AEEEvent  eCode,
                                               uint16    wParam,
                                               uint32    dwParam);
static boolean CFieldDebug_SecServicesHandleEvent(CFieldDebug *pme,
                                               AEEEvent  eCode,
                                               uint16    wParam,
                                               uint32    dwParam);

static boolean CFieldDebug_RATSHandleEvent(CFieldDebug *pme,
                                           AEEEvent  eCode,
                                           uint16    wParam,
                                           uint32    dwParam);

static boolean CFieldDebug_IPHandleEvent(CFieldDebug *pme,
                                           AEEEvent  eCode,
                                           uint16    wParam,
                                           uint32    dwParam);

static boolean CFieldDebug_RATS_TESTHandleEvent(CFieldDebug *pme,
                                           AEEEvent  eCode,
                                           uint16    wParam,
                                           uint32    dwParam);

static boolean CFieldDebug_ToggleStrideHandleEvent(CFieldDebug *pme,
                                           AEEEvent  eCode,
                                           uint16    wParam,
                                           uint32    dwParam);

static boolean CFieldDebug_MMCCloseHandleEvent(CFieldDebug * pme,
                                               AEEEvent eCode,
                                               uint16 wParam,
                                               uint32 dwParam);

static void CFieldDebug_DebugScreenRefresh(CFieldDebug *pme);
static void CFieldDebug_DrawErrorScreen(CFieldDebug * pme);
static void CFieldDebug_DrawSMSMOSOScreen(CFieldDebug *pme);
static void CFieldDebug_DrawVoicePrivScreen(CFieldDebug *pme);
static AECHAR* CFieldDebug_DebugScreenReadItem(CFieldDebug *pme);
static void CFieldDebug_DrawVersionScreen(CFieldDebug *pme);
static void CFieldDebug_ClearCallBackFn(IShell *ps);
static boolean CFieldDebug_ClearFPLMN(CFieldDebug* pme);

static void CFieldDebug_RGB24toRGB16(uint16 * outPtr, uint8 * inPtr, uint32 num_bytes);

#if defined(FEATURE_GSM) || defined (FEATURE_WCDMA)
static void CFieldDebug_EndAMRDlg(CFieldDebug *pme);
#endif
void CFieldDebug_RDMCb(rdm_assign_status_type status,
                      rdm_service_enum_type service,
                      rdm_device_enum_type device);
void CFieldDebug_InitMMCDownload(void *pUser);
void CFieldDebug_MMCDownload(void *pUser);
void CFieldDebug_EndMMCDownload(CFieldDebug *pme);
static void CFieldDebug_MMCDownload_GoToBG(CFieldDebug *pme);

static void CFieldDebug_EndDlg(void *pData);

void EndErrorMessageDlg(void *pUser);
extern dword usbsdms_get_write_blk_count(void);
#ifdef FEATURE_MMGSDI
void CFieldDebug_SIMWrittenFinalCb(gsdi_cnf_T* pGSDIConf);
void CFieldDebug_SIMWrittenCb(gsdi_cnf_T* pGSDIConf);
void CFieldDebug_GetFileAttrCb(gsdi_cnf_T * pGSDIConf);
#endif


static boolean CFieldDebug_HDRDebugHandleEvent(CFieldDebug *pme,
                                               AEEEvent  eCode,
                                               uint16    wParam,
                                               uint32    dwParam);
#ifdef FEATURE_HDR
static AECHAR* CFieldDebug_HDRScreenReadItem(CFieldDebug *pme);
static void CFieldDebug_HDRScreenRefresh( CFieldDebug *pme );
#endif


static boolean CFieldDebug_JPEGDecodeHandleEvent(CFieldDebug *pme,
                                               AEEEvent  eCode,
                                               uint16    wParam,
                                               uint32    dwParam);

#ifdef FEATURE_JPEG_DECODER

static void InitJPEGFileList(CFieldDebug * pme);

static char * CFieldDebug_GetFileName(const char * psz);

void CFieldDebug_JPEGDecodeCB ( cmx_status_type status, const void *client_data,
                         void *server_data, uint32 num_bytes );

static void CFieldDebug_CreateIBitmap(CFieldDebug * pme, uint16 x, uint16 y);

static void CFieldDebug_DisplayImage(CFieldDebug * pme);

#endif /* FEATURE_JPEG_DECODER */

static boolean CFieldDebug_VoiceDebugHandleEvent(CFieldDebug * pme, AEEEvent eCode, uint16 wParam, uint32 dwParam);

static boolean CFieldDebug_DisableInCallDisplay(CFieldDebug * pme, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean CFieldDebug_MakeGameModePersist(CFieldDebug * pme, AEEEvent eCode, uint16 wParam, uint32 dwParam);

#if defined(FEATURE_SEC_TESTAPPS) || defined(FEATURE_SEC_IPSEC_VPN_CLNT)
typedef void (*SecTestFunc) (void);
typedef struct
{
  uint16 itemID;
  SecTestFunc function;
} SecurityTestFunction;

SecurityTestFunction SecurityTestTable[] =
{
#ifdef FEATURE_SEC_TESTAPPS
  #ifdef FEATURE_SEC_SSL_TESTAPP
    {IDL_SSL_HTTP,   sectestapp_ssl_http_start},
    {IDL_SSL_FUNCT,   sectestapp_ssl_func_start},
    {IDL_SSL_MULTI,    sectestapp_ssl_multi_start},
    {IDL_SSL_LONG,    sectestapp_ssl_long_start},
  #endif //FEATURE_SEC_SSL_TESTAPP

  #ifdef FEATURE_SEC_CRYPT_TESTAPP
    {IDL_SECAPI_PKX,  sectestapp_api_pkx_start},
    {IDL_SECAPI_ENC,  sectestapp_api_enc_start},
    {IDL_SECAPI_HSH,  sectestapp_api_hsh_start},
    {IDL_SEC_CRYPTO,  sectestapp_crypto_start},
    {IDL_SEC_CUSTOM,  sectestapp_custom_start},
  #endif //FEATURE_SEC_CRYPT_TESTAPP

  #ifdef FEATURE_SEC_DRM
    {IDL_DRM_USE_RO,  sectestapp_secdrm_test_1},
    {IDL_DRM_DEL_ARO,  sectestapp_secdrm_test_2},
    {IDL_DRM_DEL_RO,  sectestapp_secdrm_test_3},
  #endif //FEATURE_SEC_DRM
#endif //FEATURE_SEC_TESTAPPS

#ifdef FEATURE_SEC_IPSEC_VPN_CLNT
  {IDL_VPN_STOP,  secipsvpnclnt_stop},
#endif

};
#endif //defined(FEATURE_SEC_TESTAPPS) || defined(FEATURE_SEC_IPSEC_VPN_CLNT)


// Static Applet Functions - These functions need only be declared
// when an applet is being linked by the Manufacturer during their
// main build process.  They are not required for dynamically downloaded
// applets.
//
#if defined(BREW_STATIC_APP)

static int CFieldDebugMod_CreateInstance(AEECLSID  ClsId,
                                     IShell   *pIShell,
                                     IModule  *po,
                                     void    **ppObj);
/*static*/ int CFieldDebugMod_Load(IShell   *ps,
                           void     *pHelpers,
                           IModule **pMod);

#ifdef FEATURE_HDR
extern boolean OEMPriv_ReadHDRDebugInfo(void);
#endif
#endif /* BREW_STATIC_APP */
/*===========================================================================

                      STATIC/LOCAL DATA

===========================================================================*/


static const AEEAppInfo gaiFieldDebug = {
   AEECLSID_FIELDDEBUGAPP,
   FLDDBG_RES_FILE,
   IDS_OPTION_TITLE,
   0,
   0,
   0,
   0,
   AFLAG_PHONE | AFLAG_TOOL | AFLAG_HIDDEN
};

//
// This is a list of dialog event handers that correspond to the
// dialogs in the fdnmenu.bar file, indexed by dialog resource Id
//
// IMPORTANT!  Dialog resource IDs are assumed to start at 1 and
//             must not have gaps between successive IDs (ie. 1,2,4,5).
//
static const PFNAEEEVENT sDialogEventHandlers[] =
{
  (PFNAEEEVENT) CFieldDebug_TopMenuHandleEvent,       // IDD_TOP_DIALOG
  (PFNAEEEVENT) CFieldDebug_DebugMenuHandleEvent,     // IDD_DEBUG_DIALOG
  (PFNAEEEVENT) CFieldDebug_VersionMenuHandleEvent,   // IDD_VERSION_DIALOG
  (PFNAEEEVENT) CFieldDebug_DebugScreenHandleEvent,   // IDD_DEBUG_SCREEN_DIALOG
  (PFNAEEEVENT) CFieldDebug_TestCallHandleEvent,      // IDD_CALL_LIST_DIALOG
  (PFNAEEEVENT) CFieldDebug_ErrorListHandleEvent,     // IDD_ERROR_LIST_DIALOG
  (PFNAEEEVENT) CFieldDebug_ClearErrorsHandleEvent,   // IDD_CLEAR_ERROR_DIALOG
  (PFNAEEEVENT) CFieldDebug_ClearEFSHandleEvent,      // IDD_CLEAR_EFS_DIALOG
  (PFNAEEEVENT) CFieldDebug_DataModeHandleEvent,      // IDD_DM_DIALOG
  (PFNAEEEVENT) CFieldDebug_SMSMOSOHandleEvent,       // IDD_SMS_MO_SO_DIALOG
  (PFNAEEEVENT) CFieldDebug_VoicePrivHandleEvent,     // IDD_VOICE_PRIVACY_DIALOG
  (PFNAEEEVENT) CFieldDebug_MMCHandleEvent,           // IDD_MMC_DIALOG
  (PFNAEEEVENT) CFieldDebug_MMCFormatHandleEvent,     // IDD_MMC_FORMAT_DIALOG
  (PFNAEEEVENT) CFieldDebug_MMCDownloadHandleEvent,   // IDD_MMC_DOWNLOAD_DIALOG
  (PFNAEEEVENT) CFieldDebug_FPLMNClearHandleEvent,    // IDD_WAIT_FPLMN_CLEAR_DIALOG
  (PFNAEEEVENT) CFieldDebug_ErrorDialogHandleEvent,   // IDD_ERROR_DIALOG
  (PFNAEEEVENT) CFieldDebug_AMRPrefDialogHandleEvent, // IDD_AMR_PREF
  (PFNAEEEVENT) CFieldDebug_SecServicesHandleEvent,   // IDD_SECURITY_SERVICES
  (PFNAEEEVENT) CFieldDebug_SecServicesHandleEvent,   // IDD_SSL_TEST
  (PFNAEEEVENT) CFieldDebug_SecServicesHandleEvent,   // IDD_SEC_TEST
  (PFNAEEEVENT) CFieldDebug_SecServicesHandleEvent,   // IDD_DRM_TEST
  (PFNAEEEVENT) CFieldDebug_SecServicesHandleEvent,   // IDD_VPN_TEST
  (PFNAEEEVENT) CFieldDebug_JPEGDecodeHandleEvent,    // IDD_JPEG_DECODE
  (PFNAEEEVENT) CFieldDebug_RATSHandleEvent,          // IDD_RATS_DIALOG
  (PFNAEEEVENT) CFieldDebug_IPHandleEvent,            // IDD_RATS_IP_DIALOG
  (PFNAEEEVENT) CFieldDebug_RATS_TESTHandleEvent,     // IDD_RATS_TEST_DIALOG
  (PFNAEEEVENT) CFieldDebug_HDRDebugHandleEvent,      // IDD_HDR_DEBUG_INFORMATION
  (PFNAEEEVENT) CFieldDebug_VoiceDebugHandleEvent,    // IDD_VOICE_DEBUG
  (PFNAEEEVENT) CFieldDebug_ToggleStrideHandleEvent,  // IDD_TOGGLE_STRIDE
  (PFNAEEEVENT) NULL,                                 // IDD_PROG_DIALOG
  (PFNAEEEVENT) CFieldDebug_DisableInCallDisplay,     // IDD_DISABLE_IN_CALL_DISP
  (PFNAEEEVENT) CFieldDebug_MakeGameModePersist,      // IDD_MAKE_GAME_MODE_PERSIST
  (PFNAEEEVENT) CFieldDebug_SelMMCDLTimeHandleEvent,  // IDD_MMC_DOWNLOAD_SELECT_TIME
  (PFNAEEEVENT) CFieldDebug_MMCCloseHandleEvent,      // IDD_MMC_DOWNLOAD_CLOSE_DIALOG
};



/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
#if defined(BREW_STATIC_APP)
/*===========================================================================

FUNCTION CFieldDebug_GetModInfo

DESCRIPTION

PROTOTYPE:

PARAMETERS:

DEPENDENCIES
  none

RETURN VALUE

SIDE EFFECTS
  none
===========================================================================*/
PFNMODENTRY CFieldDebug_GetModInfo(IShell      *ps,
                               AEECLSID   **ppClasses,
                               AEEAppInfo **pApps,
                               uint16      *pnApps,
                               uint16      *pwMinPriv)
{
   PARAM_NOT_REF(ps)
   PARAM_NOT_REF(ppClasses)
   PARAM_NOT_REF(pwMinPriv)

   *pApps = (AEEAppInfo *)&gaiFieldDebug;
   *pnApps = 1;
   return(PFNMODENTRY)CFieldDebugMod_Load;
}

/*===========================================================================

FUNCTION CFieldDebugMod_Load

DESCRIPTION

PROTOTYPE:

PARAMETERS:

DEPENDENCIES
  none

RETURN VALUE

SIDE EFFECTS
  none
===========================================================================*/
/*static*/ int CFieldDebugMod_Load(IShell   *ps,
                           void     *pHelpers,
                           IModule **pMod)
{
   return AEEStaticMod_New(sizeof(AEEMod),
                           ps,
                           pHelpers,
                           pMod,
                           (PFNMODCREATEINST)CFieldDebugMod_CreateInstance,
                           (PFNFREEMODDATA)NULL);
}

#endif /* BREW_STATIC_APP */

/*===========================================================================

FUNCTION CFieldDebugMod_CreateInstance

DESCRIPTION

PROTOTYPE:

PARAMETERS:

DEPENDENCIES
  none

RETURN VALUE

SIDE EFFECTS
  none
===========================================================================*/


#if defined(BREW_STATIC_APP)
static int CFieldDebugMod_CreateInstance(AEECLSID  ClsId,
                                     IShell   *pIShell,
                                     IModule  *po,
                                     void    **ppObj)
#else
int AEEClsCreateInstance(AEECLSID  ClsId,
                         IShell   *pIShell,
                         IModule  *po,
                         void    **ppObj)
#endif
{
   CFieldDebug     *pme;
#ifdef FEATURE_APP_TEST_AUTOMATION
   AECHAR        AppVersion[MAX_APP_VERSION_SIZE];
   uint32        nDiagRegisterSuccess;
   int           nCreateSuccess;
#endif

   if (!AEEApplet_New(sizeof(CFieldDebug),
                      ClsId,
                      pIShell,
                      po,
                      (IApplet**)ppObj,
                      (AEEHANDLER)CFieldDebug_HandleEvent,
                      (PFNFREEAPPDATA)CFieldDebug_Free)) {
      return ENOMEMORY;
   }

   do {
      pme = (CFieldDebug *)*ppObj;

      if (ISHELL_CreateInstance(pIShell,
                                AEECLSID_CONFIG,
                                (void **)(&pme->m_pIConfig )) != AEE_SUCCESS) {
         break;
      }

      /* Attempt to create the Remote API Test object.  If the creation
       * or registration for events fail, then make sure the object stays
       * as NULL.  The corresponding menus will be removed.
       */
      if (ISHELL_CreateInstance(pIShell,
                                AEECLSID_RATS,
                                (void **)(&pme->m_pIRats )) == AEE_SUCCESS) {
        if (ISHELL_RegisterNotify(pIShell, AEECLSID_FIELDDEBUGAPP, AEECLSID_RATS,
                                  (NMASK_STATUS_CHANGE | NMASK_DATA_AVAILABLE)
                                 ) != AEE_SUCCESS) {
          (void) IRATS_Release(pme->m_pIRats);
          pme->m_pIRats = NULL;
        }
      }

#ifdef FEATURE_APP_TEST_AUTOMATION
      // Only create the diag-related objects if test automation is on


      // create IDiag object
      nCreateSuccess = ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_DIAG,
                                             (void **)&(pme->m_pIDiag));
      if (SUCCESS != nCreateSuccess)
      {
        MSG_ERROR("IDiag Create Failed: %d", nCreateSuccess, 0, 0);
      }
      else
      {
    
        (void)STRTOWSTR(FLDDBG_SW_VERSION, AppVersion, MAX_APP_VERSION_SIZE);
    
        /* Now register for diagnostics.  Don't fail init if we cannot
         * register, but go ahead and release the object if registration fails
         */
        nDiagRegisterSuccess = IDIAG_RegisterApp(pme->m_pIDiag,
                                                 AEECLSID_FIELDDEBUGAPP,
                                                 AppVersion,
                                                 FLDDBGAPP_DIAG_EVENT_REG_SIZE,
                                                 CFldDbgApp_DiagEventReg);
        if (SUCCESS != nDiagRegisterSuccess)
        {
          MSG_ERROR("IDiag Registration Failed", 0, 0, 0);
          (void)IDIAG_Release(pme->m_pIDiag);
        }
        else
        {
    
#if MIN_BREW_VERSION(3, 1)
          // Create IAppletCtl object only if we're using BREW 3.1+
          nCreateSuccess = ISHELL_CreateInstance(pme->a.m_pIShell,
                                               AEECLSID_APPLETCTL,
                                               (void **)&(pme->m_pIAppletCtl));
          if (SUCCESS != nCreateSuccess)
          {
            // We can't do BREW 3.1 diags without the IAppletCtl
            MSG_ERROR("IAppletCtl Create Failed: %d", nCreateSuccess, 0, 0);
            (void)IDIAG_Release(pme->m_pIDiag);
          }
#endif // MIN_BREW_VERSION(3, 1)

        }
      }

#endif // FEATURE_APP_TEST_AUTOMATION

      pme->m_debugmenu_sel = IDL_DEBUG_SCR;

      pme->m_nEndDlgId = 0;

      pme->m_nMMCDLTime = FLDDBG_MMC_DL_TIME_UNLIMITED;

      pme->m_nMMCDLState = FLDDBG_MMC_DOWNLOAD_IDLE;

      /* Increase font sizes on VGA displays */
      AppComFunc_SelectFonts(pme->a.m_pIDisplay);

      return AEE_SUCCESS;

   } while (0); /*lint !e717*/

   // we only get here if there was an error
   CFieldDebug_Free(pme);
   return EFAILED;
}

/*===========================================================================

FUNCTION CFieldDebug_Free

DESCRIPTION

PROTOTYPE:

PARAMETERS:

DEPENDENCIES
  none

RETURN VALUE

SIDE EFFECTS
  none
===========================================================================*/

static void CFieldDebug_Free(CFieldDebug * pme)
{

   if (pme->m_pIConfig) {
      (void) ICONFIG_Release(pme->m_pIConfig);
   }
}

/*===========================================================================

FUNCTION CFieldDebug_GetDlgEvtHandler

DESCRIPTION
  This routine gets a function pointer to the event handler for a dialog.

PROTOTYPE:
  PFNAEEEVENT CFieldDebug_GetDlgEvtHandler(IDialog *pIDialog);

PARAMETERS:
  IDialog *pIDialog:  An IDialog pointer for the dialog whose event
                      handler is to be retrieved.

DEPENDENCIES
  none

RETURN VALUE
  If successful, a function pointer to the event handler for the
     requested dialog.
  If failed, returns NULL.

SIDE EFFECTS
  none
===========================================================================*/
static PFNAEEEVENT CFieldDebug_GetDlgEvtHandler(IDialog *pIDialog)
{
  uint16 nDlgID;

  if (pIDialog == NULL)
  {
    MSG_ERROR("NULL dialog pointer.", 0, 0, 0);
    return NULL;
  }

  nDlgID = IDIALOG_GetID(pIDialog);
  if (nDlgID == 0)
  {
    MSG_ERROR("Unknown dialog id, %d", nDlgID, 0, 0);
    return NULL;
  }

  nDlgID--;
  if (nDlgID >= ARR_SIZE(sDialogEventHandlers))
  {
    MSG_ERROR("Unknown dialog id, %d", nDlgID, 0, 0);
    return NULL;
  }

  return (sDialogEventHandlers[nDlgID]);
}

/*===========================================================================

FUNCTION CFieldDebug_ClearCallBackFn

DESCRIPTION

PROTOTYPE:

PARAMETERS:

DEPENDENCIES
  none

RETURN VALUE

SIDE EFFECTS
  none
===========================================================================*/
static void CFieldDebug_ClearCallBackFn(IShell *ps)
{
   (void) ISHELL_PostEvent(ps,
                           0,
                           EVT_COMMAND,
                           DISP_TIMEOUT_WPARAM,
                           0);
   return;
}

/*===========================================================================

FUNCTION CFieldDebug_ClearErrorLog

DESCRIPTION
   Clears the error log

PROTOTYPE:

PARAMETERS:

DEPENDENCIES
  none

RETURN VALUE

SIDE EFFECTS
  none
===========================================================================*/
static void CFieldDebug_ClearErrorLog(CFieldDebug *pme)
{
  /*lint -esym(715,pme)*/
  int i;

  for (i=0;i<20;i++) {
    (void) err_clr_log ((byte) i);
  }
  return;
}

/*===========================================================================

FUNCTION CFieldDebug_SetImage

DESCRIPTION
   Used with the Remote API Test System Settings.  This routine sets the
   icon to represent the data in the current state.

PROTOTYPE:
void CFieldDebug_SetImage(IMenuCtl *pm, uint16 id, int status)

PARAMETERS:
   pm - Pointer to the menu control item that will have the icon.
   status - IRATS_STATUS_UP - Show ICON indicating service is running.
            IRATS_STATUS_DOWN - Show ICON indicating service is not.
            anything else - Show ICON indicating service is tranistioning.

DEPENDENCIES
  none

RETURN VALUE
  none.

SIDE EFFECTS
  none
===========================================================================*/
void CFieldDebug_SetImage(IMenuCtl *pm, uint16 id, int status)
{
  CtlAddItem  pai;

  if (pm == NULL)
    return;

  memset(&pai, 0, sizeof(pai));

  (void) IMENUCTL_GetItem(pm, id, &pai);

  pai.pImage = NULL;
  pai.pszResImage = FLDDBG_RES_FILE;

  switch (status)
  {
    case IRATS_STATUS_UP:
      pai.wImage = IDB_ON;
      break;
    case IRATS_STATUS_DOWN:
      pai.wImage = IDB_OFF;
      break;
    default:
      pai.wImage = IDB_BUSY;
      break;
  }
  (void) IMENUCTL_SetItem(pm, id, MSIF_IMAGE, &pai);
}

/*===========================================================================

FUNCTION CFieldDebug_FillTest

DESCRIPTION
   Used with the Remote API Test System Settings.  This routine fills the
   static control item with the text from the file associated with RATS.
   If the static control is scrollable, this then sets the position such
   that the last line in the control is the bottom of the screen.

PROTOTYPE:
void CFieldDebug_FillTest(CFieldDebug *pme, IStatic *pstatic)

PARAMETERS:
   pme    - Pointer to the CFieldDebug object.
   pstatic- Pointer to the static control to fill.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  If the user is scrolling through the control when this is called, the
  user will be placed at the last line of the file.
===========================================================================*/
void CFieldDebug_FillTest(CFieldDebug *pme, IStatic *pstatic)
{
  char buffer[64];      /* File contains ASCII text. */
  AECHAR w_buffer[64];  /* Static controls work best with AECHARs */
  IFile *pFile;         /* Pointer to the file containing the info. */
  int   font_size;      /* The size of a line.  */
  AEERect rc;           /* Dimension of the static control. */
  int num_lines;        /* Number of lines (size of control/size of line) */

  int currReturn;       /* Used to determine how many lines are being used. */
  int SetLine = 0;      /* Current guess when finding the last line. */

  /* If we have an object, we can start filling it. */
  if (pme->m_pIRats != NULL)
  {
    /* Get the file pointer from IRATS. */
    pFile = IRATS_GetFile(pme->m_pIRats);
    if (pFile == NULL)
      return;

    /* Move to the start of the file. */
    (void) IFILE_Seek(pFile, _SEEK_START, pme->m_LastFileLocation);

    /* While there are characters to read. */
    while ((currReturn = IFILE_Read(pFile, buffer, sizeof(buffer)-1)) > 0)
    {
      /* Remember what we have read. */
      pme->m_LastFileLocation += currReturn;

      /* Make sure the string is NULL terminated. */
      buffer[currReturn] = '\0';

      /* Convert ASCII to wide strings. */
      memset(w_buffer, 0, sizeof(w_buffer));
      (void) STRTOWSTR(buffer, w_buffer, sizeof(w_buffer));

      /* Append the text to the static control. */
      (void) ISTATIC_SetTextEx(pstatic, (byte *) w_buffer, NULL, TRUE);
    }

    /* We are done with the file. */
    (void) IFILE_Release(pFile);

    /* If the control indicates it is scrollable. we'll need to move to the
     * end. */
    if (ISTATIC_IsScrollable(pstatic))
    {
      /* Get the static window size and the font size. */
      font_size = IDISPLAY_GetFontMetrics(pme->a.m_pIDisplay, AEE_FONT_NORMAL,
                                          NULL, NULL);
      ISTATIC_GetRect(pstatic, &rc);

      /* Get the line count. */
      if (pme->m_LastLineNumber > 0)         /* If we know it....   */
        SetLine = pme->m_LastLineNumber - 1; /* Pick before we were last. */
      else                             /* Otherwise, we have at least */
        SetLine = (rc.dy / font_size); /* the lines on the screen */

      /* Initialize the counting algorithm so the first call returns 0. */
      (void) ISTATIC_GoToLine(pstatic, 0);     /* Move to the beginning. */
      num_lines = -1;   /* Mark last return as invalid. */

      /* Pick a line to goto, if we ever get the same return value twice
       * in a row, we know we hit the maximum line location.  The reason
       * is that ISTATIC_GoToLine will not set a line beyond the number
       * of lines on the screen, but always returns the last location.
       * For Example.  If there used to be 48 lines, but now are 50 lines,
       * the first call Sets it to back 48, and returns 0, the second call
       * sets it to 49 and return 48, the third sets it to 50, returns 49
       * fourth trys to set it to 51 returns 50 and the fifth trys to set
       * it to 52, but also returns 50, hence we know we only had 50 lines.
       */
      while (num_lines != (currReturn = ISTATIC_GoToLine(pstatic, ++SetLine))) {
        num_lines = currReturn;
      }
      pme->m_LastLineNumber = num_lines;

      /* Go to the top of the screen containing the last line of data. */
      (void) ISTATIC_GoToLine(pstatic, num_lines - (rc.dy / font_size) + 1);
    }
  }
}

/*=============================================================================
FUNCTION: CFieldDebug_OnDialogStart

DESCRIPTION:  EVT_DIALOG_START event handler

PARAMETERS:
   *pme: CSvcPrgApp object pointer
   wParam: word event parameter
   dwParam: dword event parameter

RETURN VALUE:
   boolean: TRUE if the event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CFieldDebug_OnDialogStart(CFieldDebug  *pme,
                                     uint16       wParam,
                                     uint32       dwParam)
{
  IMenuCtl *pm = NULL, *psk = NULL;

  switch (wParam) {
    case IDD_TOP_DIALOG:
      pm = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                           IDC_SYS_MENU);
      break;
    case IDD_DEBUG_DIALOG:
      pm = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                           IDC_DEBUG_MENU);
      break;
    case IDD_VERSION_DIALOG:
      psk = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                           IDC_VER_SOFTKEY);
      break;
    case IDD_DEBUG_SCREEN_DIALOG:
      psk = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                           IDC_DBSCR_SOFTKEY);
      break;
    case IDD_CALL_LIST_DIALOG:
      pm = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                           IDC_CALL_LIST_MENU);
      break;
    case IDD_ERROR_LIST_DIALOG:
      psk = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                           IDC_ERROR_SOFTKEY);
      break;
    case IDD_CLEAR_ERROR_DIALOG:
      psk = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                           IDC_CLRERR_SOFTKEY);
      break;
    case IDD_CLEAR_EFS_DIALOG:
      psk = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                           IDC_CLREFS_SOFTKEY);
      break;
    case IDD_DM_DIALOG:
      pm = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                           IDC_DATADM_MENU);
      break;
    case IDD_SMS_MO_SO_DIALOG:
      pm = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                           IDC_SMSMOSO_MENU);
      break;
    case IDD_VOICE_PRIVACY_DIALOG:
      pm = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                           IDC_VOICE_PRIVACY_MENU);
      break;

    case IDD_MMC_DIALOG:
      pm = (IMenuCtl *) IDIALOG_GetControl ((IDialog *) dwParam,
                                            IDC_MMC_MENU);
      break;

    case IDD_MMC_DOWNLOAD_DIALOG:
    case IDD_MMC_FORMAT_DIALOG:
      break;

    case IDD_MMC_DOWNLOAD_CLOSE_DIALOG:
      psk = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                            IDC_SK_MMC_DOWNLOAD_CLOSE);
      break;

    case IDD_MMC_DOWNLOAD_SELECT_TIME:
      pm = (IMenuCtl *) IDIALOG_GetControl ((IDialog *) dwParam,
                                            IDC_SELECT_MMC_DOWNLOAD_TIME);
      break;

    case IDD_RATS_DIALOG:
      pm = (IMenuCtl *) IDIALOG_GetControl ((IDialog *) dwParam,
                                            IDC_RATS_MENU);
      psk = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                            IDC_RATS_SOFTKEY);

      break;

    case IDD_RATS_IP_DIALOG:
      psk = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                            IDC_RATS_IP_SOFTKEY);
      break;

    case IDD_RATS_TEST_DIALOG:
      break;

#ifdef srIMON
    case IDD_STRIDE_DIALOG:
        psk = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                         IDC_STRIDE_SOFTKEY);
        break;
#endif

    default:
      MSG_ERROR ("Unhandled dialog %d", wParam, 0,0);
      return FALSE;
  }
  if (pm) {
    SetDefaultMenuLook (pme->a.m_pIShell, pm);
  }
  if (psk) {
    SetDefaultSoftkeyLook (pme->a.m_pIShell, psk);
  }
  return TRUE;
}

/*=============================================================================
FUNCTION: CFieldDebug_MoveToDialog

DESCRIPTION:  Creates a new dialog, replacing the current dialog

PARAMETERS:
   *pme: CFieldDebug object pointer
   id: Dialog resource Id

RETURN VALUE:
   boolean:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean CFieldDebug_MoveToDialog(CFieldDebug *pme, uint16 id)
{
   // Close the current dialog (if there is one)
   (void) ISHELL_EndDialog(pme->a.m_pIShell);

   if (ISHELL_CreateDialog(pme->a.m_pIShell,
                           FLDDBG_RES_FILE,
                           id,
                           NULL) != AEE_SUCCESS) {
      MSG_ERROR ("Could not create dialog", 0, 0, 0);
      return FALSE;
   }
   return TRUE;
}




/*===========================================================================

FUNCTION CFieldDebug_VersionMenuHandleEvent

DESCRIPTION
  Handle the key/command event for the version sub-menu.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean CFieldDebug_VersionMenuHandleEvent(CFieldDebug *pme,
                                               AEEEvent  eCode,
                                               uint16    wParam,
                                               uint32    dwParam)
{
   PARAM_NOT_REF(dwParam)

   switch (eCode) {

   case EVT_COMMAND:
      if (wParam == IDL_DONE_I) {
         (void) CFieldDebug_MoveToDialog(pme, IDD_TOP_DIALOG);
         return TRUE;
      }
      return FALSE;

   case EVT_KEY:
      switch (wParam) {

      case AVK_CLR:
         (void) CFieldDebug_MoveToDialog(pme, IDD_TOP_DIALOG);
         return TRUE;

      default:
         break;
      }
      return FALSE;

   case EVT_DIALOG_START:
      (void) CFieldDebug_OnDialogStart (pme, wParam, dwParam);
      CFieldDebug_DrawVersionScreen(pme);
      return TRUE;

   case EVT_DIALOG_END:
      return TRUE;

   default:
      break;
   }
   return FALSE;
}



/*=============================================================================
FUNCTION:  CFieldDebug_DebugScreenReadItem

DESCRIPTION: Read the Device specific value and make the string to display

PARAMETERS:
   pme: CFieldDebug object pointer

RETURN VALUE:
   AECHAR* to the string to be displayed.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static AECHAR* CFieldDebug_DebugScreenReadItem(CFieldDebug *pme)
{
#if defined(FEATURE_WCDMA)
   int16 sw;
   int8 sb;
#endif
   uint16 w;
   uint32 dw;
   byte b;
   //String to store the DebugScreen items - Please check for overflow
   char   temp[512] = {'\0'};
   unsigned int i = 0;
   short rx_rssi, dyn_range;
   AECHAR *pszBuf = NULL;
#ifdef FEATURE_QIPCALL
   char   ip_addr_str[QIPCALL_DEBUG_MAX_IP_ADDR_STR_LENGTH];
   int    idx = 0;
#endif
#if (defined(T_MSM6500) || defined(T_MSM6280))
   int16 rx_agc0, rx_agc1;
#endif

#ifdef T_MSM6500
   // Get RF Card type.
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_RF_CARD_NV_ID,
                                  &w,
                                  sizeof(w))) {

       (void)SNPRINTF(temp+i,sizeof(temp),"RF Card: %d  NV band id: %d\n", w >> 8, w & 0xFF);
       i = STRLEN(temp);
   }
#endif //T_MSM6500

   // Get the current Band or RF state
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_BAND,
                                  &b,
                                  sizeof(b))) {
     char band_name[6];

#ifdef T_MSM6500
     if (b <= CFGI_DEBUG_BAND_BC14 ) 
     {
         (void)SNPRINTF(temp+i,sizeof(temp),"RF Mode: BC%d\n", b);
         i = STRLEN(temp); 
     }
     else
     {
#endif //T_MSM6500

        switch (b)
        {
        case CFGI_DEBUG_BAND_AMPS:
          (void)STRLCPY(band_name, "AMPS", sizeof(band_name));
          break;
        case CFGI_DEBUG_BAND_CDMA:
          (void)STRLCPY(band_name, "CDMA", sizeof(band_name));
          break;
        case CFGI_DEBUG_BAND_PCS:
          (void)STRLCPY(band_name, "PCS", sizeof(band_name));
          break;
        case CFGI_DEBUG_BAND_GPS:
          (void)STRLCPY(band_name, "GPS", sizeof(band_name));
          break;
        case CFGI_DEBUG_BAND_SLEEP:
          (void)STRLCPY(band_name, "SLEEP", sizeof(band_name));
          break;
        case CFGI_DEBUG_BAND_GSM:
          (void)STRLCPY(band_name, "GSM", sizeof(band_name));
          break;
        case CFGI_DEBUG_BAND_WCDMA:
          (void)STRLCPY(band_name, "WCDMA", sizeof(band_name));
          break;
        default:
          (void)STRLCPY(band_name, "UKNW", sizeof(band_name));
          break;
        }

        (void)SNPRINTF(temp+i,sizeof(temp),"RF Mode: %s\n", band_name);
        i = STRLEN(temp);

#ifdef T_MSM6500
     } //if CFGI_DEBUG_BAND_BC14
#endif // T_MSM6500

   }

   // Get the current Band and Channel
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_CHANNEL,
                                  &w,
                                  sizeof(w))) {

      (void)SNPRINTF(temp+i,sizeof(temp),"Channel: %d\n", w);
      i = STRLEN(temp);
   }

   // Get the active pilot
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_PILOT_PN,
                                  &w,
                                  sizeof(w))) {
      (void)SNPRINTF(temp+i,sizeof(temp),"Pilot PN: %d\n", w);
      i = STRLEN(temp);
   }

   // Get Rx AGC in dBm
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_RX_AGC_IN_DBM,
                                  &dw,
                                  sizeof(dw))) {
     int32 rx_agc = (int32)dw;
#if (defined(T_MSM6500) || defined(T_MSM6280))
       rx_agc0 = rx_agc >> 16;
       rx_agc1 = rx_agc & 0xFFFF;
       (void)SNPRINTF(temp+i,sizeof(temp),"Rx AGC dBm: %d.%d : %d.%d\n", (rx_agc0/10), ABS(rx_agc0%10),
                                                     (rx_agc1/10), ABS(rx_agc1%10));
#else
      (void)SNPRINTF(temp+i,sizeof(temp),"Rx AGC dBm: %d.%d\n", (rx_agc/10), ABS(rx_agc%10));
#endif
     i = STRLEN(temp);
   }

#if defined(FEATURE_WCDMA)
  //Get RSSI - Valid for GSM mode only
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_GSM_RSSI,
                                  &dw,
                                  sizeof(dw))) {
      (void)SNPRINTF(temp+i,sizeof(temp),"RSSI: %d\n", dw);
      i = STRLEN(temp);
   }

   //Get GSM Rx Level
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_GSM_RX_LEVEL,
                                  &sb,
                                  sizeof(sb))) {
      (void)SNPRINTF(temp+i,sizeof(temp),"Rx Level: %d dBm\n", sb);
      i = STRLEN(temp);
   }

   //Get Track LO Adjust value - Valid for WCDMA and GSM mode
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_TRK_LO_ADJ,
                                  &w,
                                  sizeof(w))) {
      (void)SNPRINTF(temp+i,sizeof(temp),"trk_lo_adj: %d\n", w);
      i = STRLEN(temp);
   }
   
   //Get Freq Error value - Valid for WCDMA and GSM mode
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_FREQ_ERROR,
                                  &sw,
                                  sizeof(sw))) {
      (void)SNPRINTF(temp+i,sizeof(temp),"Freq Error: %d\n", sw);
      i = STRLEN(temp);
   }
#endif //FEATURE_WCDMA

   // Get LNA State
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_LNA_STATE,
                                  &b,
                                  sizeof(b))) {
#if (defined(T_MSM6500) || defined(T_MSM6280))
      (void)SNPRINTF(temp+i,sizeof(temp),"LNA States:  %d : %d \n", b>>4, b & 0xF);
#else
      (void)SNPRINTF(temp+i,sizeof(temp),"LNA State: %d\n", b);
#endif
      i = STRLEN(temp);
   }

#ifdef FEATURE_INTELLICEIVER
   // Get IntelliCeiver State
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_INTELLICEIVER_STATE,
                                  &b,
                                  sizeof(b))) {
     switch ( b )
     {
     case 0:
         (void)SNPRINTF(temp+i,sizeof(temp),"IntelliCeiver State:  High\n", 0);
         break;
  
     case 1:
         (void)SNPRINTF(temp+i,sizeof(temp),"IntelliCeiver State:  Mid\n", 0);
         break;
  
     case 2:
         (void)SNPRINTF(temp+i,sizeof(temp),"IntelliCeiver State:  Low\n", 0);
         break;
  
     default:
         (void)SNPRINTF(temp+i,sizeof(temp),"IntelliCeiver State:  Invalid\n", 0);
         break;
     }
     i = STRLEN(temp);
   }
#endif /* FEATURE_INTELLICEIVER */

   // Get Tx AGC in dBm
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_TX_AGC_IN_DBM,
                                  &w,
                                  sizeof(w))) {
     int16 tx_agc = (int16)w;

     if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                    CFGI_DEBUG_RATCHET_STATE,
                                    &b,
                                    sizeof(b))) {
       char ratchet_state[15];

       if ((b & 0x01) == 0)
       {
         (void)STRLCPY (ratchet_state,"MAX LIMITED", sizeof(ratchet_state));
       }
       else if ((b & 0x2) == 0)
       {
         (void)STRLCPY (ratchet_state,"MIN LIMITED", sizeof(ratchet_state));
       }
       else
       {
         (void)STRLCPY (ratchet_state,"", sizeof(ratchet_state));
       }

       (void)SNPRINTF(temp+i,sizeof(temp),"Tx AGC dBm: %d.%d %s\n", (tx_agc/10), ABS(tx_agc%10), ratchet_state);
       i = STRLEN(temp);
     }
     else
     {
       (void)SNPRINTF(temp+i,sizeof(temp),"Tx AGC dBm: %d.%d\n", (tx_agc/10), ABS(tx_agc%10));
       i = STRLEN(temp);
     }
   }

#if defined(FEATURE_WCDMA)
   //Get Tx AGC Adjust value 
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_TX_AGC,
                                  &w,
                                  sizeof(w))) {
       int16 tx_agc_adj = (int16)w;
       (void)SNPRINTF(temp+i,sizeof(temp),"Tx AGC Adj: %d\n", tx_agc_adj);
       i = STRLEN(temp);
   }
#endif  //FEATURE_WCDMA


   // Get PA State
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_PA_STATE,
                                  &b,
                                  sizeof(b))) {

#ifdef T_MSM6500
     char pa_line[7];
     char pa_on_off[4];
     byte pa_data = b;

     /* 1x targets use PA_R1 0 is high gain, 1 is low gain*/

     if (pa_data & 0x10)
     {
       (void)STRLCPY(pa_on_off,"ON ",sizeof(pa_on_off));

       if(pa_data & 0x02)
       {
         (void)STRLCPY(pa_line,"HI PWR",sizeof(pa_line));
       }
       else
       {
         (void)STRLCPY(pa_line,"LO PWR",sizeof(pa_line));
       }
     }
     else
     {
       (void)STRLCPY(pa_on_off,"OFF",sizeof(pa_on_off));
       (void)STRLCPY(pa_line,"",sizeof(pa_line));
     }

     (void)SNPRINTF(temp+i,sizeof(temp),"PA State: %s %s\n", pa_on_off, pa_line);
      i = STRLEN(temp);

     // Get HDET
     if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                    CFGI_DEBUG_HDET,
                                    &b,
                                    sizeof(b))) {

       /* HDET data is only valid when TX is ON */
       if ((pa_data & 0x10) == 0)
         b = 0;

       (void)SNPRINTF(temp+i,sizeof(temp),"HDET: %d\n", b);
       i = STRLEN(temp);
     }
#else  
 
     (void)SNPRINTF(temp+i,sizeof(temp),"PA Range: %d\n", b);
     i = STRLEN(temp);
     // Get HDET
     if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                               CFGI_DEBUG_HDET,
                               &b,
                               sizeof(b))) {
         (void)SNPRINTF(temp+i,sizeof(temp),"HDET: %d\n", b);
         i = STRLEN(temp);
     }
#endif  //T_MSM6500
     

   }
   // Get THERM
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_THERM,
                                  &w,
                                  sizeof(w))) {
    b = w >> 8;
      (void)SNPRINTF(temp+i,sizeof(temp),"THERM: %d (raw: %d)\n", b&0x0ff, w&0x0ff);
      i = STRLEN(temp);
   }

   // Get VBATT
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_VBATT,
                                  &b,
                                  sizeof(b))) {
      (void)SNPRINTF(temp+i,sizeof(temp),"VBATT: %d\n", b);
      i = STRLEN(temp);
   }

   // Get SID
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_SID,
                                  &w,
                                  sizeof(w))) {
      (void)SNPRINTF(temp+i,sizeof(temp),"SID: %d\n", w);
      i = STRLEN(temp);
   }

   // Get NID
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_NID,
                                  &w,
                                  sizeof(w))) {
      (void)SNPRINTF(temp+i,sizeof(temp),"NID: %d\n", w);
      i = STRLEN(temp);
   }

   // The dynamic range obtained below is 10 times the actual value
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                          CFGI_CDMA_DYNAMIC_RANGE,
                          &dyn_range,
                          sizeof(dyn_range))) {

     (void)SNPRINTF(temp+i,sizeof(temp), "Dyn Range: %d\n", dyn_range);
     i = STRLEN(temp);
   }


   // The min Rx RSSI obtained below is 10 times the actual value
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                          CFGI_CDMA_MIN_RX_RSSI,
                          &rx_rssi,
                          sizeof(rx_rssi))) {

     (void)SNPRINTF(temp+i,sizeof(temp), "Min Rx RSSI: %d\n", rx_rssi);
     i = STRLEN(temp);
   }

#if defined(T_MSM6500) && defined(T_FFA)
   // Get temperature readings. this will be reported in degrees celsius
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_THERM_BATT,
                                  &b,
                                  sizeof(b))) {
      (void)SNPRINTF(temp+i,sizeof(temp),"THERM  BATT: %dC \n", b);
      i = STRLEN(temp);
   }
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_THERM_MSM,
                                  &b,
                                  sizeof(b))) {
      (void)SNPRINTF(temp+i,sizeof(temp),"THERM   MSM: %dC \n", b);
      i = STRLEN(temp);
   }


   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_THERM_PA,
                                  &b,
                                  sizeof(b))) {
      (void)SNPRINTF(temp+i,sizeof(temp),"THERM    PA: %dC \n", b);
      i = STRLEN(temp);
   }
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_THERM_PMIC,
                                  &b,
                                  sizeof(b))) {
      (void)SNPRINTF(temp+i,sizeof(temp),"THERM  PMIC: %dC \n", b);
      i = STRLEN(temp);
   }
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_THERM_CHRGR,
                                  &b,
                                  sizeof(b))) {
      (void)SNPRINTF(temp+i, sizeof(temp),"THERM CHRGR: %dC \n", b);
      i = STRLEN(temp);
   }
#endif

#ifdef FEATURE_QIPCALL
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_QIPCALL_DEBUG_EVDO_IP_ADDR,
                                  &dw,
                                  sizeof(dw))) 
   {
      memset( ip_addr_str, 0, sizeof( ip_addr_str ) );

      idx =  SNPRINTF( &ip_addr_str[0],sizeof(ip_addr_str[0]), "%d.",
                      (int) ( ( dw & 0x000000FF ) ) );
      idx += SNPRINTF( &ip_addr_str[idx],sizeof(ip_addr_str[idx]), "%d.",
                      (int) ( ( dw & 0x0000FF00 ) >> 8 ) );
      idx += SNPRINTF( &ip_addr_str[idx],sizeof(ip_addr_str[idx]),"%d.",
                      (int) ( ( dw & 0x00FF0000 ) >> 16 ) );
      idx += SNPRINTF( &ip_addr_str[idx],sizeof(ip_addr_str[idx]),"%d",
                      (int) ( ( dw & 0xFF000000 ) >> 24 ) );
      ip_addr_str[idx] = '\0';

      (void)SNPRINTF(temp+i, sizeof(temp), "IP (H): ");
      i = STRLEN(temp);

      (void)SNPRINTF(temp+i, sizeof(temp), "%s\n", ip_addr_str);
      i = STRLEN(temp);
   }

   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_QIPCALL_DEBUG_EVDO_SIP_REG_STATUS,
                                  &b,
                                  sizeof(b))) 
   {
      (void)SNPRINTF(temp+i, sizeof(temp), "SIP Reg (H): %d\n", b);
      i = STRLEN(temp);
   }

   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_QIPCALL_DEBUG_WLAN_IP_ADDR,
                                  &dw,
                                  sizeof(dw))) 
   {
      memset( ip_addr_str, 0, sizeof( ip_addr_str ) );

      idx =  SNPRINTF( &ip_addr_str[0], sizeof(ip_addr_str[0]),"%d.",
                      (int) ( ( dw & 0x000000FF ) ) );
      idx += SNPRINTF( &ip_addr_str[idx], sizeof(ip_addr_str[idx]),"%d.",
                      (int) ( ( dw & 0x0000FF00 ) >> 8 ) );
      idx += SNPRINTF( &ip_addr_str[idx], sizeof(ip_addr_str[idx]),"%d.",
                      (int) ( ( dw & 0x00FF0000 ) >> 16 ) );
      idx += SNPRINTF( &ip_addr_str[idx], sizeof(ip_addr_str[idx]), "%d",
                      (int) ( ( dw & 0xFF000000 ) >> 24 ) );
      ip_addr_str[idx] = '\0';

      (void)SNPRINTF(temp+i, sizeof(temp), "IP (W): ");
      i = STRLEN(temp);

      (void)SNPRINTF(temp+i, sizeof(temp), "%s\n", ip_addr_str);
      i = STRLEN(temp);
   }

   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_QIPCALL_DEBUG_WLAN_SIP_REG_STATUS,
                                  &b,
                                  sizeof(b))) 
   {
      (void)SNPRINTF(temp+i, sizeof(temp), "SIP Reg (W): %d\n", b);
      i = STRLEN(temp);
   }
#endif

   (void)SNPRINTF(temp+i, sizeof(temp), "FldDbgVer: %s\n", FLDDBG_SW_VERSION);
   i = STRLEN(temp);
   pszBuf = (AECHAR*) MALLOC((i+1)*sizeof(AECHAR));
   if(pszBuf != NULL)
   {
     (void)STRTOWSTR(temp, pszBuf,(i+1)*sizeof(AECHAR) );
   }
   else
   {
     MSG_ERROR("Malloc failed!", 0, 0, 0);
   }
   return pszBuf;
}

/*===========================================================================
FUNCTION CFieldDebug_DebugScreenRefresh

DESCRIPTION
  Timer callback function to update the debug screen periodically.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
/===========================================================================*/
static void CFieldDebug_DebugScreenRefresh( CFieldDebug *pme )
{
   AECHAR *szBuf;
   IDialog *p_dlg;
   IStatic *p_stk;
   int currentLine;

   p_dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
   p_stk = (IStatic *) IDIALOG_GetControl(p_dlg, IDC_DBSCR_STATIC);

   szBuf = CFieldDebug_DebugScreenReadItem(pme);

   // get current text line from BREW
   currentLine = ISTATIC_GoToLine(p_stk, 0);
   // Set the values of the title and text strings for this control
   (void) ISTATIC_SetText(p_stk,
                          NULL,
                          szBuf,
                          AEE_FONT_NORMAL,
                          AEE_FONT_NORMAL);

   FREEIF(szBuf);
   ISTATIC_SetProperties(p_stk, ST_NOSCROLL);
   // go to current text line
   currentLine = ISTATIC_GoToLine(p_stk, currentLine);
   (void) ISTATIC_Redraw(p_stk);
   (void) ISHELL_SetTimer(pme->a.m_pIShell,
                          DEBUG_SCRN_REFRESH_PERIOD,
                          (PFNNOTIFY) CFieldDebug_DebugScreenRefresh,
                          pme );

}




/*===========================================================================
FUNCTION CFieldDebug_DebugScreenHandleEvent

DESCRIPTION
  Event handler for the the debug screen dialog

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
/===========================================================================*/
static boolean CFieldDebug_DebugScreenHandleEvent(CFieldDebug * pme,
                                               AEEEvent eCode,
                                               uint16 wParam,
                                               uint32 dwParam)
{
   PARAM_NOT_REF(dwParam)

   switch (eCode) {
   case EVT_COMMAND:
      if (wParam == IDL_DEBUG_OK_I) {
        if(pme->bEndKeyProcess == FALSE)
        {
          (void) CFieldDebug_MoveToDialog(pme, IDD_DEBUG_DIALOG);
        }
        return TRUE;
      }
      return FALSE;

   case EVT_KEY:
      switch (wParam) {
      case AVK_CLR:
         if(pme->bEndKeyProcess == FALSE)
         {
           (void) CFieldDebug_MoveToDialog(pme, IDD_DEBUG_DIALOG);
         }
         //Other wise ignore the clear key so that the dialog is not ended.
         return TRUE;

      default:
         break;
      }
      return FALSE;

   case EVT_DIALOG_START:
     (void)CFieldDebug_OnDialogStart (pme, wParam, dwParam);
     CFieldDebug_DebugScreenRefresh(pme);
     return TRUE;

   case EVT_DIALOG_END:
      (void) ISHELL_CancelTimer(pme->a.m_pIShell,
                                (PFNNOTIFY) CFieldDebug_DebugScreenRefresh,
                                pme);
      return TRUE;

   default:
      break;
   }
   return FALSE;
}



/*===========================================================================
FUNCTION CFieldDebug_TestCallHandleEvent

DESCRIPTION
    Event handler for the test call dialog

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
/===========================================================================*/
static boolean CFieldDebug_TestCallHandleEvent(CFieldDebug * pme,
                                            AEEEvent eCode,
                                            uint16 wParam,
                                            uint32 dwParam)
{
   PARAM_NOT_REF(dwParam)

#if defined (FEATURE_CDMA1X)
#ifdef FEATURE_ICM
  ICM* pICM;
  AEECMServiceOption so;
  ICallOpts * pICallOpts = NULL;
  CallOpt     call_option;
  AEECMCallID call_id;
  AECHAR number[] = {'5', '5', '5', '\0'};
  AEECMServiceType service_type = AEECM_SRV_TYPE_CDMA_SPECIFIC;
#else
   word so;
#endif /* FEATURE_ICM */
#endif /* FEATURE_CDMA1X */

   switch (eCode) {
   case EVT_COMMAND:
#if defined (FEATURE_CDMA1X)
      switch (wParam) {
      case IDL_13K_MARKOV:
#ifdef FEATURE_ICM
        so = AEECM_SO_RS2_MARKOV;
#else
        so = CAI_SO_RS2_MARKOV;
#endif
        break;
      case IDL_8K_MARKOV:
#ifdef FEATURE_ICM
        so = AEECM_SO_RS1_MARKOV;
#else
        so = CAI_SO_RS1_MARKOV;
#endif
        break;
      case IDL_13K_LOOPBACK:
#ifdef FEATURE_ICM
        so = AEECM_SO_LOOPBACK_13K;
#else
        so = CAI_SO_LOOPBACK_13K;
#endif
        break;
      case IDL_8K_LOOPBACK:
#ifdef FEATURE_ICM
        so = AEECM_SO_LOOPBACK;
#else
        so = CAI_SO_LOOPBACK;
#endif
        break;
      default:
        return FALSE;
      }

      // TODO - add code to make test call
#ifdef FEATURE_ICM
      if (ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_CALLMANAGER,
                                (void **) &pICM) != SUCCESS) {
        DBGPRINTF("Could not get instance of ICM");
        return FALSE;
      }

      if (pICM == NULL) {
        DBGPRINTF("Could not get instance of ICM");
        return FALSE;
      }

      if (ISHELL_CreateInstance (pme->a.m_pIShell, AEECLSID_CALLOPTS,(void **) &pICallOpts) != SUCCESS) {
        DBGPRINTF("Could not get instance of ICallOpts");
        return FALSE;
      }

      call_option.nId = AEECM_CALLOPT_SRV_OPT;
      call_option.pVal = &so;

      if (ICallOpts_AddOpt (pICallOpts, &call_option) != SUCCESS) {
        DBGPRINTF("Could not set call options");
        (void)ICallOpts_Release (pICallOpts);
        (void)ICM_Release (pICM);
        return FALSE;
      }

      call_option.nId = AEECM_CALLOPT_SERVICE_TYPE;
      call_option.pVal = &service_type;

      if (ICallOpts_AddOpt (pICallOpts, &call_option) != SUCCESS) {
        DBGPRINTF("Could not set call options");
        (void) ICallOpts_Release (pICallOpts);
        (void) ICM_Release (pICM);
        return FALSE;
      }

      if (ICM_OriginateCall (pICM, AEECM_CALL_TYPE_TEST, number, NULL, pICallOpts, &call_id) != SUCCESS) {
        DBGPRINTF("Could not originate call");
        (void) ICallOpts_Release (pICallOpts);
        (void) ICM_Release (pICM);
        return FALSE;
      }
      (void) ICallOpts_Release (pICallOpts);
      (void) ICM_Release (pICM);
#else
      ui_orig_test_call(
                        so,
                        (const byte *)"555",
                        3,
                        (const byte *)"",
                        0
                        );
#endif
      (void) ISHELL_CloseApplet(pme->a.m_pIShell, FALSE);
      return TRUE;
#else /* FEATURE_CDMA1X */
      return FALSE;
#endif /* FEATURE_CDMA1X */

   case EVT_KEY:
      switch (wParam) {
      case AVK_CLR:
         (void) CFieldDebug_MoveToDialog(pme, IDD_DEBUG_DIALOG);
         return TRUE;

      default:
         break;
      }
      return FALSE;

   case EVT_DIALOG_START:
     (void) CFieldDebug_OnDialogStart (pme, wParam, dwParam);
     return TRUE;

   case EVT_DIALOG_END:
      return TRUE;

   default:
      break;
   }
   return FALSE;
}




/*===========================================================================
FUNCTION CFieldDebug_ErrorListHandleEvent

DESCRIPTION
    Event handler for the Error dialog

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
/===========================================================================*/
static boolean CFieldDebug_ErrorListHandleEvent(CFieldDebug * pme,
                                             AEEEvent eCode,
                                             uint16 wParam,
                                             uint32 dwParam)
{
   PARAM_NOT_REF(dwParam)

   switch (eCode) {
   case EVT_COMMAND:
      if (wParam == IDL_ERROR_OK_I) {
         (void) CFieldDebug_MoveToDialog(pme, IDD_DEBUG_DIALOG);
         return TRUE;
      }
      return FALSE;

   case EVT_KEY:
      switch (wParam) {
      case AVK_CLR:
         (void) CFieldDebug_MoveToDialog(pme, IDD_DEBUG_DIALOG);
         return TRUE;

      default:
         break;
      }
      return FALSE;

   case EVT_DIALOG_START:
     (void) CFieldDebug_OnDialogStart (pme, wParam, dwParam);
     CFieldDebug_DrawErrorScreen(pme);
     return TRUE;

   case EVT_DIALOG_END:
      return TRUE;

   default:
      break;
   }
   return FALSE;
}

/*===========================================================================
FUNCTION CFieldDebug_VoiceDebugHandleEvent

DESCRIPTION
    Event handler for the voice debug dialog

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
/===========================================================================*/
static boolean CFieldDebug_VoiceDebugHandleEvent(CFieldDebug * pme, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
#if defined(FEATURE_GSM) || defined (FEATURE_WCDMA)
#ifdef FEATURE_VOICE_FIXED_PATTERN_DEBUG
   switch(eCode)
   {
     case EVT_COMMAND:
       switch(wParam)
        {
          case IDL_VOICE_DEBUG_ON:
          case IDL_VOICE_DEBUG_OFF:
               {
                 IDialog *pIDialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
                 IMenuCtl *pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_VOICE_DEBUG);
                 pme->m_bVoiceDebug = (IDL_VOICE_DEBUG_ON == wParam);
                 ICONFIG_SetItem(pme->m_pIConfig, CFGI_VOICE_GARBLE_DEBUG, (void*)&pme->m_bVoiceDebug, sizeof(boolean));
                 SetMenuIcon(pIMenuCtl, IDL_VOICE_DEBUG_ON, pme->m_bVoiceDebug);
                 SetMenuIcon(pIMenuCtl, IDL_VOICE_DEBUG_OFF, !pme->m_bVoiceDebug);
                 IMENUCTL_Redraw(pIMenuCtl);
               }
              break;
        }
        return TRUE;

     case EVT_DIALOG_START:
        {
          IMenuCtl *pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl((IDialog*) dwParam, IDC_VOICE_DEBUG);
          pme->m_bVoiceDebug = FALSE;
          SetDefaultMenuLook(pme->a.m_pIShell, pIMenuCtl);
          //Init the menu.
          ICONFIG_GetItem(pme->m_pIConfig, CFGI_VOICE_GARBLE_DEBUG, &pme->m_bVoiceDebug, sizeof(boolean));

          if(pme->m_bVoiceDebug)
             SetMenuIcon(pIMenuCtl, IDL_VOICE_DEBUG_ON, TRUE);
          else
             SetMenuIcon(pIMenuCtl, IDL_VOICE_DEBUG_OFF, TRUE);
        }
       return TRUE;

     case EVT_DIALOG_END:
        return TRUE;

     case EVT_KEY:
        switch(wParam)
        {
          case AVK_CLR:
          case AVK_END:
               (void) CFieldDebug_MoveToDialog(pme, IDD_DEBUG_DIALOG);
        }
        return TRUE;
   }
#endif /* FEATURE_VOICE_FIXED_PATTERN_DEBUG */
#endif /* defined(FEATURE_GSM) || defined (FEATURE_WCDMA) */
   return FALSE;
}

/*===========================================================================
FUNCTION CFieldDebug_ClearErrorsHandleEvent

DESCRIPTION
   Event handler for the Clear Error dialog

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
/===========================================================================*/
static boolean CFieldDebug_ClearErrorsHandleEvent(CFieldDebug * pme,
                                               AEEEvent eCode,
                                               uint16 wParam,
                                               uint32 dwParam)
{
   PARAM_NOT_REF(dwParam)

   AECHAR szBuf[12];
   IDialog *dlg;
   IStatic *sta;
   IMenuCtl *mctl;

   switch (eCode) {
   case EVT_COMMAND:
      if (wParam == IDL_CLRERR_YES_I) {
         // Show Message - Clearing. . .
         (void) ISHELL_LoadResString(pme->a.m_pIShell,
                                     FLDDBG_RES_FILE,
                                     IDS_CLEARING_TXT,
                                     szBuf,
                                     sizeof(szBuf));
         dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
         sta = (IStatic *) IDIALOG_GetControl(dlg, IDC_CLEAR_ERRLOG);
         (void) ISTATIC_SetText(sta, NULL, szBuf,
                                AEE_FONT_NORMAL, AEE_FONT_NORMAL);
         (void) ISTATIC_Redraw(sta);

         mctl = (IMenuCtl *)IDIALOG_GetControl(dlg, IDC_CLRERR_SOFTKEY);
         (void) IMENUCTL_DeleteAll(mctl);
         (void) IMENUCTL_Redraw(mctl);

         (void) ISHELL_SetTimer(pme->a.m_pIShell,
                                SHOW_MESSAGE_TIMEOUT_MS,
                                (PFNNOTIFY) CFieldDebug_ClearCallBackFn,
                                pme->a.m_pIShell);
         // erase the error log
         CFieldDebug_ClearErrorLog(pme);
         return TRUE;
      } else if ((wParam == IDL_CLRERR_NO_I) ||
                 (wParam == DISP_TIMEOUT_WPARAM)) {
         (void) CFieldDebug_MoveToDialog(pme, IDD_DEBUG_DIALOG);
         return TRUE;
      }
      return FALSE;

   case EVT_KEY:
      switch (wParam) {
      case AVK_CLR:
         (void) CFieldDebug_MoveToDialog(pme, IDD_DEBUG_DIALOG);
         return TRUE;

      default:
         break;
      }
      return FALSE;

   case EVT_DIALOG_START:
     (void) CFieldDebug_OnDialogStart (pme, wParam, dwParam);
     return TRUE;

   case EVT_DIALOG_END:
      return TRUE;

   default:
      break;
   }
   return FALSE;
}



/*===========================================================================
FUNCTION CFieldDebug_ClearEFSHandleEvent

DESCRIPTION
  Key/Command Event handler while the Clear EFS screen is displaying

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
/===========================================================================*/
static boolean CFieldDebug_ClearEFSHandleEvent(CFieldDebug * pme,
                                            AEEEvent eCode,
                                            uint16 wParam,
                                            uint32 dwParam)
{
   PARAM_NOT_REF(dwParam)

   AECHAR szBuf[12];
   IDialog *dlg;
   ITextCtl *txt;
   AECHAR   input_txt[16], yes_txt[4];
#ifdef FEATURE_ICM
   ICM *pICM = NULL;
   int nReturnStatus;
#endif

   switch (eCode) {
   case EVT_COMMAND:
      if (wParam == IDL_CLREFS_OK_I) {
         // Show Message - Clearing. . .
         (void) ISHELL_LoadResString(pme->a.m_pIShell,
                                     FLDDBG_RES_FILE,
                                     IDS_CLEARING_TXT,
                                     szBuf,
                                     sizeof(szBuf));

         dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
         txt = (ITextCtl *) IDIALOG_GetControl(dlg, IDC_CLREFS_TEXT);
         (void) ITEXTCTL_GetText(txt, input_txt, 16);
         (void) STRTOWSTR("Yes", yes_txt, sizeof(yes_txt));
#ifdef FEATURE_ICM
         /* Create the Call Manager object. */
         nReturnStatus = ISHELL_CreateInstance(pme->a.m_pIShell,
                                    AEECLSID_CM,
                                    (void **) &pICM);

         if(nReturnStatus != SUCCESS) {
           return FALSE;
         }

         if (pICM == NULL) { /* error */
           return FALSE;
         }

#endif
         if (!(WSTRCMP(input_txt, yes_txt))) { // if the user types Yes
#ifdef FEATURE_ICM
           (void) ICM_SetOperatingMode(pICM, AEECM_OPRT_MODE_OFFLINE);
#else
           ui_set_ph_oprt_mode (SYS_OPRT_MODE_OFFLINE);
#endif
            // TODO - do something here
#ifndef FEATURE_EFS2
            fs_master_reset();
#else
          efs_reset();
#endif
#ifdef FEATURE_ICM
           (void) ICM_SetOperatingMode(pICM, AEECM_OPRT_MODE_RESET);
#else
           ui_set_ph_oprt_mode(SYS_OPRT_MODE_RESET);
#endif
         } else {
            (void) CFieldDebug_MoveToDialog(pme, IDD_DEBUG_DIALOG);
         }
#ifdef FEATURE_ICM
         (void) ICM_Release(pICM);
         pICM = NULL;
#endif
         return TRUE;
      } else if (wParam == IDL_CLREFS_CANCEL_I ) {
         (void) CFieldDebug_MoveToDialog(pme, IDD_DEBUG_DIALOG);
         return TRUE;
      }
      return FALSE;

   case EVT_KEY:
      switch (wParam) {
      case AVK_CLR:
         dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
         txt = (ITextCtl *) IDIALOG_GetControl(dlg, IDC_CLREFS_TEXT);
         if (ITEXTCTL_IsActive(txt) &&
             *(ITEXTCTL_GetTextPtr(txt))) {
            // if there is text in buf let textctl handle this event
            return TRUE;
         }
         (void) CFieldDebug_MoveToDialog(pme, IDD_DEBUG_DIALOG);
         return TRUE;

      default:
         break;
      }
      return FALSE;

   case EVT_DIALOG_START:
     (void) CFieldDebug_OnDialogStart (pme, wParam, dwParam);
     return TRUE;

   case EVT_DIALOG_END:
      return TRUE;

   default:
      break;
   }
   return FALSE;
}


/*===========================================================================
FUNCTION CFieldDebug_ToggleStrideHandleEvent

DESCRIPTION
  Event handler for the the Stride state dialog

DEPENDENCIES
  none

RETURN VALUE
  boolean

SIDE EFFECTS
  none
/===========================================================================*/
static boolean CFieldDebug_ToggleStrideHandleEvent(CFieldDebug * pme,
                                               AEEEvent eCode,
                                               uint16 wParam,
                                               uint32 dwParam)
{
   PARAM_NOT_REF(dwParam)

#ifndef srIMON
   PARAM_NOT_REF(wParam)
   PARAM_NOT_REF(eCode)

   return FALSE;
#else

   IDialog *pDlg;
   IStatic *pStatic;
   AECHAR buf[32], title[32];
   
   switch (eCode) {
   case EVT_KEY:
      switch (wParam) {
      case AVK_CLR:
         if(pme->bEndKeyProcess == FALSE)
         {
           (void) CFieldDebug_MoveToDialog(pme, IDD_DEBUG_DIALOG);
         }
         //Other wise ignore the clear key so that the dialog is not ended.
         return TRUE;

      default:
         break;
      }
      return FALSE;

   case EVT_COMMAND:
      if (wParam == IDL_CHGSTATE_STRIDE_YES_I) 
      {
          OEMInterceptModules_ToggleState();
          (void) CFieldDebug_MoveToDialog(pme, IDD_DEBUG_DIALOG);
          return TRUE;
      }
      else if ((wParam == IDL_CHGSTATE_STRIDE_NO_I) ||
                 (wParam == DISP_TIMEOUT_WPARAM)) {
         (void) CFieldDebug_MoveToDialog(pme, IDD_DEBUG_DIALOG);
         return TRUE;
      }
      return FALSE;
   
   case EVT_DIALOG_START:
     pDlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
     pStatic = (IStatic *) IDIALOG_GetControl(pDlg, IDC_STRIDE_CHGSTATE);
     if(OEMInterceptModules_stride_is_active())
     {
       ISHELL_LoadResString (pme->a.m_pIShell, FLDDBG_RES_FILE, 
                               IDS_STRIDE_ACTIVE, buf,
                               sizeof(AECHAR)*ARR_SIZE(buf));

     }
     else
     {
       ISHELL_LoadResString (pme->a.m_pIShell, FLDDBG_RES_FILE, 
                               IDS_STRIDE_INACTIVE, buf,
                               sizeof(AECHAR)*ARR_SIZE(buf));
     }

     ISHELL_LoadResString (pme->a.m_pIShell, FLDDBG_RES_FILE, 
                           IDS_STRIDE_CHGSTATE, title,
                           sizeof(AECHAR)*ARR_SIZE(title));

     (void) ISTATIC_SetText(pStatic, title, buf,
                            AEE_FONT_NORMAL, AEE_FONT_NORMAL);
     (void) ISTATIC_Redraw(pStatic);

     CFieldDebug_OnDialogStart (pme, wParam, dwParam);

     return TRUE;

   case EVT_DIALOG_END:
     return TRUE;

   default:
      break;
   }
   return FALSE;
#endif //srIMON
}

                    

/*===========================================================================
FUNCTION CFieldDebug_DataModeHandleEvent

DESCRIPTION
  Event handler for the Data/DM dialog

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
/===========================================================================*/
static boolean CFieldDebug_DataModeHandleEvent(CFieldDebug * pme,
                                            AEEEvent eCode,
                                            uint16 wParam,
                                            uint32 dwParam)
{
   PARAM_NOT_REF(dwParam)

   switch (eCode) {
   case EVT_COMMAND:
      return FALSE;

   case EVT_KEY:
      switch (wParam) {
      case AVK_CLR:
         (void) CFieldDebug_MoveToDialog(pme, IDD_DEBUG_DIALOG);
         return TRUE;

      default:
         break;
      }
      return FALSE;

   case EVT_DIALOG_START:
     (void) CFieldDebug_OnDialogStart (pme, wParam, dwParam);
     return TRUE;

   case EVT_DIALOG_END:
      return TRUE;

   default:
      break;
   }
   return FALSE;
}




/*===========================================================================
FUNCTION CFieldDebug_SMSMOSOHandleEvent

DESCRIPTION
  Event handler for the SMS MO SO dialog

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
/===========================================================================*/
static boolean CFieldDebug_SMSMOSOHandleEvent(CFieldDebug * pme,
                                           AEEEvent eCode,
                                           uint16 wParam,
                                           uint32 dwParam)
{
   PARAM_NOT_REF(dwParam)
   byte mo_so;

   switch (eCode) {
   case EVT_COMMAND:
      switch (wParam) {
      case IDL_SO6_I:
         mo_so = OEMNV_SMS_SO_SO6;
         break;

      case IDL_SO14_I:
         mo_so = OEMNV_SMS_SO_SO14;
         break;

      default:
         return FALSE;
      }

      (void) ICONFIG_SetItem(pme->m_pIConfig,
                             CFGI_SMS_SERVICE_OPTION,
                             &mo_so,
                             sizeof(byte));

      (void) CFieldDebug_MoveToDialog(pme, IDD_DEBUG_DIALOG);
      return TRUE;

   case EVT_KEY:
      switch (wParam) {
      case AVK_CLR:
         (void) CFieldDebug_MoveToDialog(pme, IDD_DEBUG_DIALOG);
         return TRUE;

      default:
         break;
      }
      return FALSE;

   case EVT_DIALOG_START:
     (void) CFieldDebug_OnDialogStart (pme, wParam, dwParam);
     CFieldDebug_DrawSMSMOSOScreen(pme);
     return TRUE;

   case EVT_DIALOG_END:
      return TRUE;

   default:
      break;
   }
   return FALSE;
}




/*===========================================================================
FUNCTION CFieldDebug_VoicePrivHandleEvent

DESCRIPTION
  Event handler for the Voice Privacy dialog

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
/===========================================================================*/
static boolean CFieldDebug_VoicePrivHandleEvent(CFieldDebug * pme,
                                           AEEEvent eCode,
                                           uint16 wParam,
                                           uint32 dwParam)
{
   PARAM_NOT_REF(dwParam)
   //byte vp;

   switch (eCode) {
   case EVT_COMMAND:
      switch (wParam) {
      case IDL_VP_STANDARD_I:
     //    vp = OEMNV_VP_STANDARD;
         break;

      case IDL_VP_ENHANCED_I:
     //    vp = OEMNV_VP_ENHANCED;
         break;

      default:
         return FALSE;
      }

     /* (void) ICONFIG_SetItem(pme->m_pIConfig,
                             CFGI_VOICE_PRIVACY,
                             &vp,
                             sizeof(byte));*/

      (void) CFieldDebug_MoveToDialog(pme, IDD_DEBUG_DIALOG);
      return TRUE;

   case EVT_KEY:
      switch (wParam) {
      case AVK_CLR:
         (void) CFieldDebug_MoveToDialog(pme, IDD_DEBUG_DIALOG);
         return TRUE;

      default:
         break;
      }
      return FALSE;

   case EVT_DIALOG_START:
     (void) CFieldDebug_OnDialogStart (pme, wParam, dwParam);
     CFieldDebug_DrawVoicePrivScreen(pme);
     return TRUE;

   case EVT_DIALOG_END:
      return TRUE;

   default:
      break;
   }
   return FALSE;
}


/*===========================================================================
FUNCTION CFieldDebug_MMCHandleEvent

DESCRIPTION
  Event handler for the MMC dialog

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
/===========================================================================*/
static boolean CFieldDebug_MMCHandleEvent(CFieldDebug * pme,
                                           AEEEvent eCode,
                                           uint16 wParam,
                                           uint32 dwParam)
{
   PARAM_NOT_REF(dwParam)

   IDialog *pDlg;
   IMenuCtl *pIMenuCtl;

   switch (eCode) {
   case EVT_DIALOG_START:
      (void) CFieldDebug_OnDialogStart (pme, wParam, dwParam);

      // Change menu item from "open mmc" to "close mmc" when its already open
      if (pme->m_nMMCDLState == FLDDBG_MMC_DOWNLOAD_IN_DL_BG)
      {
         pDlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
         if (pDlg != NULL)
         {
            pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pDlg, IDC_MMC_MENU);
            if(pIMenuCtl != NULL)
            {  
               IMENUCTL_SetItemText(pIMenuCtl, IDL_MMC_DOWNLOAD, FLDDBG_RES_FILE,
                                    IDS_MMC_CLOSE, NULL);
               (void)IMENUCTL_Redraw(pIMenuCtl);
            }
         }
      }
     return TRUE;

   case EVT_COMMAND:
      switch (wParam) {
        case IDL_MMC_DOWNLOAD:
          if (pme->m_nMMCDLState == FLDDBG_MMC_DOWNLOAD_IN_DL_BG)
          {
            // If we're in background MMC download mode and being requested to exit
            (void)CFieldDebug_MoveToDialog(pme, IDD_MMC_DOWNLOAD_CLOSE_DIALOG);
          }
          else
          {
            // If we're being requested to enter MMC download mode
            (void)CFieldDebug_MoveToDialog(pme, IDD_MMC_DOWNLOAD_SELECT_TIME);
          }
          break;

        case IDL_MMC_FORMAT:
          (void) CFieldDebug_MoveToDialog(pme, IDD_MMC_FORMAT_DIALOG);
          break;

        default:
          return FALSE;
      }
      return TRUE;

   case EVT_KEY:
      switch (wParam) {
      case AVK_CLR:
         (void) CFieldDebug_MoveToDialog(pme, IDD_DEBUG_DIALOG);
         return TRUE;

      default:
         break;
      }
      return FALSE;

   case EVT_DIALOG_END:
      return TRUE;

   default:
      break;
   }
   return FALSE;
}

/*===========================================================================
FUNCTION CFieldDebug_SelMMCDLTimeHandleEvent

DESCRIPTION
  Event handler for the Voice Privacy dialog

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
/===========================================================================*/
static boolean CFieldDebug_SelMMCDLTimeHandleEvent(CFieldDebug *pme,
                                                   AEEEvent  eCode,
                                                   uint16    wParam,
                                                   uint32    dwParam)
{
  IMenuCtl *pIMenuCtl;
  IDialog  *pIDialog;

  if (! pme)
  {
    MSG_ERROR("NULL ptr", 0, 0, 0);
    return FALSE;
  }

  switch(eCode)
  {
    case EVT_DIALOG_START:
    {

      pIDialog = (IDialog *)dwParam;
      if (! pIDialog)
      {
        MSG_ERROR("NULL ptr", 0, 0, 0);
        return FALSE;
      }
      pIMenuCtl = (IMenuCtl *)IDIALOG_GetControl(pIDialog,
                                                 IDC_SELECT_MMC_DOWNLOAD_TIME);
      if (! pIMenuCtl)
      {
        MSG_ERROR("NULL ptr", 0, 0, 0);
        return FALSE;
      }

      /* Set up the menu with the default selection populated */
      SetDefaultMenuLook(pme->a.m_pIShell, pIMenuCtl);
      pme->m_nMMCDLTime = (int32)FLDDBG_MMC_DL_TIME_UNLIMITED;
      SetMenuIcon(pIMenuCtl, IDC_MMC_DOWNLOAD_5_MIN, FALSE);
      SetMenuIcon(pIMenuCtl, IDC_MMC_DOWNLOAD_30_MIN, FALSE);
      SetMenuIcon(pIMenuCtl, IDC_MMC_DOWNLOAD_UNLIMITED_BG, FALSE);
      SetMenuIcon(pIMenuCtl, IDC_MMC_DOWNLOAD_UNLIMITED, TRUE);
    }
    return TRUE;
  
    case EVT_KEY:
    switch(wParam)
    {
      case AVK_CLR:
        return CheckAEEReturnStatus(ISHELL_EndDialog(pme->a.m_pIShell));

      default:
        break;
    }
    break;
  
    case EVT_COMMAND:
      switch(wParam)
      {
        case IDC_MMC_DOWNLOAD_5_MIN:
        case IDC_MMC_DOWNLOAD_30_MIN:
        case IDC_MMC_DOWNLOAD_UNLIMITED:
        case IDC_MMC_DOWNLOAD_UNLIMITED_BG:
        {
          pIDialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
          if (! pIDialog)
          {
            MSG_ERROR("NULL ptr", 0, 0, 0);
            return FALSE;
          }

          pIMenuCtl = (IMenuCtl *)IDIALOG_GetControl(pIDialog,
                                                     IDC_SELECT_MMC_DOWNLOAD_TIME);
          if (! pIMenuCtl)
          {
            MSG_ERROR("NULL ptr", 0, 0, 0);
            return FALSE;
          }

          /* Reset all of the radio buttons to "off" and then turn on
           * the correct one.
           */
          SetMenuIcon(pIMenuCtl, IDC_MMC_DOWNLOAD_5_MIN, FALSE);
          SetMenuIcon(pIMenuCtl, IDC_MMC_DOWNLOAD_30_MIN, FALSE);
          SetMenuIcon(pIMenuCtl, IDC_MMC_DOWNLOAD_UNLIMITED_BG, FALSE);
          SetMenuIcon(pIMenuCtl, IDC_MMC_DOWNLOAD_UNLIMITED, FALSE);

          switch (wParam)
          {
            case IDC_MMC_DOWNLOAD_5_MIN:
              pme->m_nMMCDLTime = (int32)FLDDBG_MMC_DL_TIME_5_MIN;
              SetMenuIcon(pIMenuCtl, IDC_MMC_DOWNLOAD_5_MIN, TRUE);
              pme->m_nMMCDLState = FLDDBG_MMC_DOWNLOAD_GO_DL;
              break;

            case IDC_MMC_DOWNLOAD_30_MIN:
              pme->m_nMMCDLTime = (int32)FLDDBG_MMC_DL_TIME_30_MIN;
              SetMenuIcon(pIMenuCtl, IDC_MMC_DOWNLOAD_30_MIN, TRUE);
              pme->m_nMMCDLState = FLDDBG_MMC_DOWNLOAD_GO_DL;
              break;

            case IDC_MMC_DOWNLOAD_UNLIMITED_BG:
              pme->m_nMMCDLTime = (int32)FLDDBG_MMC_DL_TIME_UNLIMITED;
              SetMenuIcon(pIMenuCtl, IDC_MMC_DOWNLOAD_UNLIMITED_BG, TRUE);
              pme->m_nMMCDLState = FLDDBG_MMC_DOWNLOAD_GO_DL_BG;
              break;

            case IDC_MMC_DOWNLOAD_UNLIMITED:
            default:
              pme->m_nMMCDLTime = (int32)FLDDBG_MMC_DL_TIME_UNLIMITED;
              SetMenuIcon(pIMenuCtl, IDC_MMC_DOWNLOAD_UNLIMITED, TRUE);
              pme->m_nMMCDLState = FLDDBG_MMC_DOWNLOAD_GO_DL;
              break;

          }

          (void)IMENUCTL_Redraw(pIMenuCtl);

          // Cancel any "end dialog" timer, and launch the MMC Download dialog
          (void)ISHELL_CancelTimer(pme->a.m_pIShell, (PFNNOTIFY)CFieldDebug_EndDlg,
                                   (void *)pme);
          (void) ISHELL_CreateDialog(pme->a.m_pIShell, FLDDBG_RES_FILE,
                                     IDD_MMC_DOWNLOAD_DIALOG, NULL);
        }
        break;
  
        default:
          break;
      }
      return TRUE;
  
    case EVT_DIALOG_END:
      (void)ISHELL_CancelTimer(pme->a.m_pIShell, (PFNNOTIFY)CFieldDebug_EndDlg,
                               (void *)pme);
      return TRUE;
  
    default:
      break;

  }
  return FALSE;
}

/*===========================================================================
FUNCTION CFieldDebug_MMCDownloadHandleEvent

DESCRIPTION
  Event handler for the Voice Privacy dialog

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
/===========================================================================*/
static boolean CFieldDebug_MMCDownloadHandleEvent(CFieldDebug * pme,
                                           AEEEvent eCode,
                                           uint16 wParam,
                                           uint32 dwParam)
{
   PARAM_NOT_REF(dwParam)
   switch (eCode) {
    case EVT_DIALOG_START:
      (void) CFieldDebug_OnDialogStart (pme, wParam, dwParam);
      //Remove the Quit Softkey.
      {
        IMenuCtl *pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl((IDialog*) dwParam, IDC_SK_MMC_DOWNLOAD);
        if(pIMenuCtl != NULL)
        {
          (void) IMENUCTL_DeleteAll(pIMenuCtl);
        }
      }
      //Start Init Timer
      (void) ISHELL_SetTimer(pme->a.m_pIShell, 100, CFieldDebug_InitMMCDownload, (void*) pme);
      pme->bEndKeyProcess = TRUE;
      return TRUE;

    case EVT_USER:
      switch(wParam)
      {
        case EVT_MMC_DL_CLOSED:
          /* When MMC Download mode has successfully exited, close this
           * dialog.  If there's no remaining dialog, restore the top-level
           * dialog for the applet.
           */
          pme->nCount1 = pme->nCount2 = 0;
          (void)ISHELL_EndDialog(pme->a.m_pIShell);

          if (ISHELL_GetActiveDialog(pme->a.m_pIShell) == NULL)
          {
            pme->m_dlgID = IDD_TOP_DIALOG;
            (void)ISHELL_CreateDialog(pme->a.m_pIShell, FLDDBG_RES_FILE,
                                       pme->m_dlgID, NULL);
            return TRUE;
          }
          break;

        default:
          MSG_ERROR("Unhandled user event, %d.", wParam, 0, 0);
          break;
      }
      return TRUE;

    case EVT_KEY:
      switch(wParam)
      {
      case AVK_CLR:
      case AVK_END:
        if(pme->nCount1 == pme->nCount2)
        {
          CFieldDebug_EndMMCDownload(pme);
        }
        return TRUE;

      default:
        break;
      }
      return FALSE;

    case EVT_COMMAND:
      switch(wParam)
      {
        case IDL_QUIT_MMC_DOWNLOAD:
          CFieldDebug_EndMMCDownload(pme);
          return TRUE;
        default:
          break;
      }
      break;

    case EVT_DIALOG_END:
        pme->bEndKeyProcess = FALSE;
        /* If we're still in DL mode, then the dialog was closed without
         * user interaction (e.g. via an MT call), so we still need
         * to shut down MMC DL mode.  Don't do it for BG mode, though,
         * since the whole point of BG mode is to leave the dialog and
         * stay in MMC DL mode.
         */
        if (pme->m_nMMCDLState == FLDDBG_MMC_DOWNLOAD_IN_DL)
        {
          CFieldDebug_EndMMCDownload(pme);;
        }
        else if (pme->m_nMMCDLState != FLDDBG_MMC_DOWNLOAD_IN_DL_BG)
        {
          pme->m_nMMCDLState = FLDDBG_MMC_DOWNLOAD_IDLE;
        }
        return TRUE;

    default:
        break;
   }
   return FALSE;
}

/*===========================================================================
FUNCTION CFieldDebug_SwitchMMCDownloadMode

DESCRIPTION
  Handles RDM events related to entering and exiting MMC Download Mode

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
/===========================================================================*/
static boolean CFieldDebug_SwitchMMCDownload(CFieldDebug *pme,
                                             uint32 rdm_status)
{

  if (pme == NULL)
  {
    MSG_ERROR("NULL app pointer.", 0, 0, 0);
    return FALSE;
  }

  if (rdm_status == (uint32)RDM_DONE_S)
  {
    switch (pme->rdmState)
    {
      case RDM_STATUS_INIT1:
        pme->rdmState = RDM_STATUS_INIT2;
        pme->old_port_map1 = rdm_get_device(RDM_DIAG_SRVC);
        if (pme->old_port_map1 == RDM_USB_DIAG_DEV)
        {
          (void)rdm_assign_port(RDM_DIAG_SRVC, RDM_NULL_DEV,
                                CFieldDebug_RDMCb);
        }
        else
        {
          //Fake the RDM event to go to next state.
          (void)ISHELL_PostEventEx(pme->a.m_pIShell, 0,
                                   AEECLSID_FIELDDEBUGAPP,
                                   EVT_USER, EVT_RDM_STATUS,
                                   (uint32)RDM_DONE_S);
        }
        break;

      case RDM_STATUS_INIT2:
        pme->rdmState = RDM_STATUS_INIT3;
#ifdef FEATURE_USB_DIAG_NMEA
        pme->old_port_map2 = rdm_get_device (RDM_NMEA_SRVC);
        if (pme->old_port_map1 == RDM_USB_NMEA_DEV)
        {
          rdm_assign_port(RDM_NMEA_SRVC, RDM_NULL_DEV,
                          CFieldDebug_RDMCb);
        }
        else
#endif
        {
          //Fake the RDM event to go to next state.
          (void)ISHELL_PostEventEx(pme->a.m_pIShell, 0,
                                   AEECLSID_FIELDDEBUGAPP,
                                   EVT_USER, EVT_RDM_STATUS,
                                   (uint32)RDM_DONE_S);
        }
        break;

      case RDM_STATUS_INIT3:
        pme->rdmState = RDM_STATUS_INIT4;
        (void)rdm_assign_port(RDM_MMC_SRVC, RDM_USB_MMC_DEV,
                              CFieldDebug_RDMCb);
        break;

      case RDM_STATUS_INIT4:
#if (defined(FEATURE_USB) && defined(FEATURE_RUNTIME_DEVMAP))
        pme->nCount1 = pme->nCount2 = usbsdms_get_write_blk_count ();
#else
        pme->nCount1 = pme->nCount2 = 0;
#endif
        // If we're supposed to go background, then do that now.
        if (pme->m_nMMCDLState == FLDDBG_MMC_DOWNLOAD_GO_DL_BG)
        {
          CFieldDebug_MMCDownload_GoToBG(pme);
        }
        else
        {
          pme->m_nMMCDLState = FLDDBG_MMC_DOWNLOAD_IN_DL;
          (void)ISHELL_SetTimer(pme->a.m_pIShell, MMC_DOWNLOAD_POLL_TIME,
                                CFieldDebug_MMCDownload, (void*)pme);
        }
        break;

      case RDM_STATUS_EXIT1:
        pme->rdmState = RDM_STATUS_EXIT2;
        if(pme->old_port_map != RDM_NULL_DEV)
        {
          (void)rdm_assign_port(RDM_DATA_SRVC, pme->old_port_map,
                                CFieldDebug_RDMCb);
        }
        else
        {
          (void)ISHELL_PostEventEx(pme->a.m_pIShell, 0,
                                   AEECLSID_FIELDDEBUGAPP,
                                   EVT_USER, EVT_RDM_STATUS,
                                   (uint32)RDM_DONE_S);
        }
        pme->old_port_map = RDM_NULL_DEV;
        break;

      case RDM_STATUS_EXIT2:
#ifdef FEATURE_USB_DIAG_NMEA
        pme->rdmState = RDM_STATUS_EXIT3;
#else
        pme->rdmState = RDM_STATUS_EXIT4;
#endif
        if (pme->old_port_map1 != RDM_NULL_DEV)
        {
          (void)rdm_assign_port(RDM_DIAG_SRVC, pme->old_port_map1,
                                CFieldDebug_RDMCb);
        }
        else
        {
          (void)ISHELL_PostEventEx(pme->a.m_pIShell, 0,
                                   AEECLSID_FIELDDEBUGAPP,
                                   EVT_USER, EVT_RDM_STATUS,
                                   (uint32)RDM_DONE_S);
        }
        pme->old_port_map1 = RDM_NULL_DEV;
        break;

      case RDM_STATUS_EXIT3:
#ifdef FEATURE_USB_DIAG_NMEA
        pme->rdmState = RDM_STATUS_EXIT4;
        if(pme->old_port_map2 != RDM_NULL_DEV)
        {
          (void)rdm_assign_port(RDM_NMEA_SRVC, pme->old_port_map2,
                                CFieldDebug_RDMCb);
        }
        else
        {
          (void)ISHELL_PostEventEx(pme->a.m_pIShell, 0,
                                   AEECLSID_FIELDDEBUGAPP,
                                   EVT_USER, EVT_RDM_STATUS,
                                   (uint32)RDM_DONE_S);
        }
        pme->old_port_map2 = RDM_NULL_DEV;
#else
        MSG_ERROR("Non NMEA state reached",0, 0, 0);
#endif
        break;

      case RDM_STATUS_EXIT4:
        /* Notify the one who closed MMC download mode that the
         * job is finished.
         */
        pme->m_nMMCDLState = FLDDBG_MMC_DOWNLOAD_IDLE;
        (void)ISHELL_PostEventEx(pme->a.m_pIShell, 0,
                                 AEECLSID_FIELDDEBUGAPP,
                                 EVT_USER, EVT_MMC_DL_CLOSED,
                                 0);
        break;

      default:
        MSG_ERROR("Unhandled RDM Status state, %d", pme->rdmState, 0, 0);
        break;
    }
  }
  else
  {
    // RDM reported an error status
    MSG_ERROR("RDM status error, %d", rdm_status, 0, 0);
    (void)ISHELL_EndDialog(pme->a.m_pIShell);
    pme->m_wStatusResID = IDS_ERROR;
    (void)ISHELL_CreateDialog(pme->a.m_pIShell, FLDDBG_RES_FILE,
                              IDD_ERROR_DIALOG, NULL);
  }
  return TRUE;
}

/*===========================================================================
FUNCTION CFieldDebug_MMCCloseHandleEvent

DESCRIPTION
  Event handler for the MMC close dialog

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
/===========================================================================*/
static boolean CFieldDebug_MMCCloseHandleEvent(CFieldDebug * pme,
                                               AEEEvent eCode,
                                               uint16 wParam,
                                               uint32 dwParam)
{
  IStatic *pStatic;
  IDialog *pDlg;
  AECHAR pszwDownload[MAX_RES_SIZE];
  AECHAR pszwClosing[MAX_RES_SIZE];
  IMenuCtl *pIMenuCtl = NULL;

   PARAM_NOT_REF(dwParam)
   switch (eCode) {
    case EVT_DIALOG_START:
      if (CFieldDebug_OnDialogStart(pme, wParam, dwParam) == FALSE)
      {
        MSG_ERROR("Failed to launch MMC Close dialog.", 0, 0, 0);
        return FALSE;
      }

      pDlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);

      //Remove the Softkeys
      pIMenuCtl = (IMenuCtl *)IDIALOG_GetControl(pDlg, IDC_SK_MMC_DOWNLOAD_CLOSE);
      if(pIMenuCtl != NULL)
      {
        (void)IMENUCTL_DeleteAll(pIMenuCtl);
      }

      pStatic = (IStatic *)IDIALOG_GetControl(pDlg, IDC_STATIC_MMC_CLOSE);
      (void)ISHELL_LoadResString(pme->a.m_pIShell, FLDDBG_RES_FILE, IDS_MMC_DOWNLOAD,
                                 pszwDownload, sizeof(AECHAR)*ARR_SIZE(pszwDownload));
      (void)ISHELL_LoadResString(pme->a.m_pIShell, FLDDBG_RES_FILE, IDS_MMC_CLOSING,
                                 pszwClosing, sizeof(AECHAR)*ARR_SIZE(pszwClosing));
      (void)ISTATIC_SetText(pStatic, pszwDownload, pszwClosing,
                            AEE_FONT_NORMAL, AEE_FONT_NORMAL);

      // Start the close process
      pme->rdmState = RDM_STATUS_EXIT1;
      (void)rdm_assign_port(RDM_MMC_SRVC, RDM_NULL_DEV,
                            CFieldDebug_RDMCb);
      pme->bEndKeyProcess = FALSE;
      pme->nCount1 = pme->nCount2 = 0;
      (void)ISHELL_CancelTimer(pme->a.m_pIShell, CFieldDebug_MMCDownload, (void*) pme);
      return TRUE;

    case EVT_DIALOG_END:
      pme->bEndKeyProcess = FALSE;
      return TRUE;

    case EVT_USER:
      switch(wParam)
      {
        case EVT_MMC_DL_CLOSED:
          pme->nCount1 = pme->nCount2 = 0;

          // Close done, update text and show softkeys
          pDlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
          if(pDlg != NULL)
          {
            pStatic = (IStatic *) IDIALOG_GetControl (pDlg, IDC_STATIC_MMC_CLOSE);
            (void)ISHELL_LoadResString(pme->a.m_pIShell, FLDDBG_RES_FILE,
                                       IDS_MMC_DOWNLOAD, pszwDownload,
                                       sizeof(AECHAR)*ARR_SIZE(pszwDownload));
            (void)ISHELL_LoadResString(pme->a.m_pIShell, FLDDBG_RES_FILE,
                                       IDS_MMC_CLOSE_OK, pszwClosing,
                                       sizeof(AECHAR)*ARR_SIZE(pszwClosing));
            (void)ISTATIC_SetText(pStatic, pszwDownload, pszwClosing,
                                  AEE_FONT_NORMAL, AEE_FONT_NORMAL);
            (void)ISTATIC_Redraw(pStatic);
    
            pIMenuCtl = (IMenuCtl *)IDIALOG_GetControl(pDlg, IDC_SK_MMC_DOWNLOAD_CLOSE);
            if(pIMenuCtl != NULL)
            {
              (void)IMENUCTL_AddItem(pIMenuCtl, FLDDBG_RES_FILE, IDS_OK,
                                     IDL_MMC_DOWNLOAD_CLOSE_OK, NULL, 0);
              SetDefaultMenuLook(pme->a.m_pIShell, pIMenuCtl);
              (void)IMENUCTL_Redraw(pIMenuCtl);
            }
            (void)IDIALOG_SetFocus(pDlg, IDC_SK_MMC_DOWNLOAD_CLOSE);
          }
          return TRUE;

        default:
          MSG_ERROR("Unhandled user event, %d.", wParam, 0, 0);
          break;
      }
      return FALSE;

    case EVT_KEY:
      switch(wParam)
      {
       case AVK_CLR:
       case AVK_END:
         (void)ISHELL_EndDialog(pme->a.m_pIShell);

         if(ISHELL_GetActiveDialog(pme->a.m_pIShell) == NULL)
         {
           pme->m_dlgID = IDD_TOP_DIALOG;
           (void)ISHELL_CreateDialog(pme->a.m_pIShell, FLDDBG_RES_FILE,
                                     pme->m_dlgID, NULL);
         }
         return TRUE;

       default:
         break;
       }
       return FALSE;


     case EVT_COMMAND:
       switch(wParam)
       {
         case IDL_MMC_DOWNLOAD_CLOSE_OK:
           (void)ISHELL_EndDialog(pme->a.m_pIShell);

           if(ISHELL_GetActiveDialog(pme->a.m_pIShell) == NULL)
           {
             pme->m_dlgID = IDD_TOP_DIALOG;
             (void)ISHELL_CreateDialog(pme->a.m_pIShell, FLDDBG_RES_FILE,
                                       pme->m_dlgID, NULL);
           }
           return TRUE;
         default:
           break;
       }
       break;
    default:
      break;
   }
   return FALSE;
}

/*===========================================================================
FUNCTION CFieldDebug_MMCFormatHandleEvent

DESCRIPTION
  Event handler for the MMC format dialog

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
/===========================================================================*/
static boolean CFieldDebug_MMCFormatHandleEvent(CFieldDebug * pme,
                                                AEEEvent eCode,
                                                uint16 wParam,
                                                uint32 dwParam)
{
  fs_rsp_msg_type rsp;
  IStatic *pStatic;
  IDialog *pDlg;
  AECHAR buf1[20], buf2[33];

   PARAM_NOT_REF(dwParam)
   switch (eCode) {
    case EVT_DIALOG_START:
      (void) CFieldDebug_OnDialogStart (pme, wParam, dwParam);
      pDlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
      pStatic = (IStatic *) IDIALOG_GetControl (pDlg, IDC_STATIC_MMC_FORMAT);
      (void) ISHELL_LoadResString (pme->a.m_pIShell, FLDDBG_RES_FILE, IDS_MMC_FORMAT, buf1,
                            sizeof(AECHAR)*ARR_SIZE(buf1));
      (void) ISHELL_LoadResString (pme->a.m_pIShell, FLDDBG_RES_FILE, IDS_FORMATTING, buf2,
                            sizeof(AECHAR)*ARR_SIZE(buf2));
      (void) ISTATIC_SetText (pStatic, buf1, buf2, AEE_FONT_NORMAL, AEE_FONT_NORMAL);
      fs_format ("mmc1", NULL, &rsp);
      if (rsp.format.status == FS_OKAY_S) {
        (void) ISHELL_LoadResString (pme->a.m_pIShell, FLDDBG_RES_FILE, IDS_SUCCESSFUL, buf2,
                            sizeof(AECHAR)*ARR_SIZE(buf2));
      }
      else if (rsp.format.status == FS_NO_MEDIA_S) {
        (void) ISHELL_LoadResString (pme->a.m_pIShell, FLDDBG_RES_FILE, IDS_NO_CARD, buf2,
                            sizeof(AECHAR)*ARR_SIZE(buf2));
      }
      else {
       (void) ISHELL_LoadResString (pme->a.m_pIShell, FLDDBG_RES_FILE, IDS_FAILED, buf2,
                            sizeof(AECHAR)*ARR_SIZE(buf2));
      }
      (void) ISTATIC_SetText (pStatic, buf1, buf2, AEE_FONT_NORMAL, AEE_FONT_NORMAL);
      return TRUE;

    case EVT_DIALOG_END:
        return TRUE;

    default:
        break;
   }
   return FALSE;
}

#if defined(FEATURE_CALL_THROTTLE)
/*===========================================================================

FUNCTION call_throttle_api

DESCRIPTION
  Calls the throttle API.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean call_throttle_api(CFieldDebug  *pme)
{
  ICM *pICM = NULL;
  int nReturnStatus;

  /* Create the Call Manager object. */
  nReturnStatus = ISHELL_CreateInstance(pme->a.m_pIShell,
                             AEECLSID_CM,
                             (void **) &pICM);

  if ((nReturnStatus != SUCCESS) || (pICM == NULL))
  {
    MSG_ERROR("Unable to create ICM object. Status = %d", nReturnStatus, 0, 0);
    pme->m_wStatusResID = IDS_ERROR;
    return CheckAEEReturnStatus(ISHELL_CreateDialog(pme->a.m_pIShell,
                                FLDDBG_RES_FILE, IDD_ERROR_DIALOG, NULL));
  }

  nReturnStatus = ICM_ResetOrigThrottleTbl(pICM);
  (void) ICM_Release(pICM);

  if (nReturnStatus != SUCCESS)
  {
    MSG_ERROR("ICM_DebugCallThrottle Failed! Status = %d", nReturnStatus, 0, 0);
    pme->m_wStatusResID = IDS_FAILED;
    return CheckAEEReturnStatus(ISHELL_CreateDialog(pme->a.m_pIShell,
      FLDDBG_RES_FILE, IDD_ERROR_DIALOG, NULL));
  }
  else
  {
    pme->m_wStatusResID = IDS_SUCCESSFUL;
    return CheckAEEReturnStatus(ISHELL_CreateDialog(pme->a.m_pIShell,
      FLDDBG_RES_FILE, IDD_ERROR_DIALOG, NULL));
  }

}
#endif
/*===========================================================================

FUNCTION CFieldDebug_DebugMenuHandleEvent

DESCRIPTION
  Key/Command Event handler for the "debugging" sub-menu

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean CFieldDebug_DebugMenuHandleEvent(CFieldDebug  *pme,
                                             AEEEvent   eCode,
                                             uint16     wParam,
                                             uint32     dwParam)
{
   switch (eCode) {
   case EVT_COMMAND:
      switch (wParam) {
      case IDL_CONTACTIMPORT:
//         (void) ISHELL_StartApplet(pme->a.m_pIShell,
//                                   AEECLSID_APP_CONTACT_IMPORTER);
         return TRUE;

      case IDL_CONTACTEXPORT:
//         (void) ISHELL_StartApplet(pme->a.m_pIShell,
//                                   AEECLSID_APP_CONTACT_EXPORTER);
         return TRUE;

#ifdef FEATURE_APP_GIGAMON
      case IDL_GIGATEST:
         (void) ISHELL_StartApplet(pme->a.m_pIShell,
                                   AEECLSID_GIGA_TEST_APP);
         return TRUE;
#endif /* FEATURE_APP_GIGAMON*/

#ifdef FEATURE_GRAPHICS_APP_CALMON
      case IDL_CALTEST:
         (void) ISHELL_StartApplet(pme->a.m_pIShell,
                                   AEECLSID_CAL_TEST_APP);
         return TRUE;
#endif /* FEATURE_GRAPHICS_APP_CALMON*/


#ifdef FEATURE_USB_WAKEUP
      case IDL_USB_WAKEUP:
         (void) usbdc_invoke_remote_wakeup();
         break;
#endif

#if defined(FEATURE_CALL_THROTTLE)
      case IDL_THROTTLE_CM:
         (void) call_throttle_api(pme);
         break;
#endif

#if defined(FEATURE_BATTERY_CHARGER) && defined(FEATURE_PMIC_USB_OTG)
      case IDL_USB_CHG_DISABLE:
         chg_usb_charger_switch( CHG_CMD_DISABLE );
         break;
      
      case IDL_USB_CHG_ENABLE:
         chg_usb_charger_switch( CHG_CMD_ENABLE );
         break;
#endif
         
      case IDL_DEBUG_SCR:
      case IDL_TEST_CALL:
      case IDL_ERROR_LIST:
      case IDL_CLR_ERROR:
      case IDL_CLR_EFS:
      case IDL_DATA_DM:
      case IDL_SMS_MO_SO:
      case IDL_VOICE_PRIVACY:
      case IDL_IGNORE_RUIM_DOOR:
      case IDL_HDR_INFO:
#ifdef srIMON
      case IDL_STRIDE_CHGSTATE:
#endif
         {
            IMenuCtl *ctl;
            IDialog *dlg;

            dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
            ctl = (IMenuCtl *) IDIALOG_GetControl(dlg, IDC_DEBUG_MENU);
            pme->m_debugmenu_sel = IMENUCTL_GetSel(ctl);
         }
         (void) CFieldDebug_MoveToDialog(pme, wParam);
         return TRUE;

#if defined(FEATURE_GSM) || defined (FEATURE_WCDMA)
      case IDL_AMR_PREF:
        return CheckAEEReturnStatus(ISHELL_CreateDialog(pme->a.m_pIShell,
            FLDDBG_RES_FILE, IDD_AMR_PREF, NULL));
#endif

      case IDL_SECURITY_SERVICES:
        return CheckAEEReturnStatus(ISHELL_CreateDialog(pme->a.m_pIShell,
            FLDDBG_RES_FILE, IDD_SECURITY_SERVICES, NULL));

      case IDL_MMC:
        (void) CFieldDebug_MoveToDialog (pme, IDD_MMC_DIALOG);
        return TRUE;

      case IDL_CLEAR_FPLMN:
        if(CFieldDebug_ClearFPLMN(pme) == TRUE)
        {
          // Wait for callbacks to be called.
          return CheckAEEReturnStatus(ISHELL_CreateDialog(pme->a.m_pIShell,
            FLDDBG_RES_FILE, IDD_WAIT_FPLMN_CLEAR_DIALOG, NULL));
        }
        else
        {
          //Display error-message
          pme->m_wStatusResID = IDS_ERROR;
          return CheckAEEReturnStatus(ISHELL_CreateDialog(pme->a.m_pIShell,
            FLDDBG_RES_FILE, IDD_ERROR_DIALOG, NULL));
        }

#if defined(FEATURE_GSM) || defined (FEATURE_WCDMA)
#ifdef FEATURE_VOICE_FIXED_PATTERN_DEBUG
      case IDL_VOICE_DEBUG:
          ISHELL_CreateDialog(pme->a.m_pIShell, FLDDBG_RES_FILE, IDD_VOICE_DEBUG, NULL);
          return TRUE;
#endif /* FEATURE_VOICE_FIXED_PATTERN_DEBUG */
#endif /* #if defined(FEATURE_GSM) || defined (FEATURE_WCDMA) */

      default:
         break;
      }

      break;

   case EVT_KEY:
      switch (wParam) {
      case AVK_CLR:
         pme->m_debugmenu_sel = IDL_DEBUG_SCR;
         (void) CFieldDebug_MoveToDialog(pme, IDD_TOP_DIALOG);
         return TRUE;

      default:
         break;
      }
      break;

   case EVT_DIALOG_START:
     (void) CFieldDebug_OnDialogStart (pme, wParam, dwParam);
      {
         IMenuCtl *ctl;
         IDialog *dlg;

         dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
         ctl = (IMenuCtl *) IDIALOG_GetControl(dlg, IDC_DEBUG_MENU);
         // delete the Data/DM menu if there is no data support
         (void) IMENUCTL_DeleteItem(ctl, IDL_DATA_DM);
         (void) IMENUCTL_DeleteItem(ctl, IDL_SMS_MO_SO);
         (void) IMENUCTL_DeleteItem(ctl, IDL_IGNORE_RUIM_DOOR);
         (void) IMENUCTL_DeleteItem(ctl, IDL_CONTACTIMPORT);
         (void) IMENUCTL_DeleteItem(ctl, IDL_CONTACTEXPORT);
         (void) IMENUCTL_DeleteItem(ctl, IDL_VOICE_PRIVACY);
#ifndef FEATURE_APP_GIGAMON
         (void) IMENUCTL_DeleteItem(ctl, IDL_GIGATEST);
#endif
#ifndef FEATURE_GRAPHICS_APP_CALMON
         (void) IMENUCTL_DeleteItem(ctl, IDL_CALTEST);
#endif
#ifndef FEATURE_HDR
         (void) IMENUCTL_DeleteItem(ctl, IDL_HDR_INFO);
#endif

#ifndef FEATURE_USB_WAKEUP
         (void) IMENUCTL_DeleteItem(ctl, IDL_USB_WAKEUP);
#endif

#if !defined(FEATURE_BATTERY_CHARGER) || !defined(FEATURE_PMIC_USB_OTG)
         (void) IMENUCTL_DeleteItem(ctl, IDL_USB_CHG_DISABLE);
         (void) IMENUCTL_DeleteItem(ctl, IDL_USB_CHG_ENABLE);
#endif

#ifndef FEATURE_MMGSDI
         (void) IMENUCTL_DeleteItem(ctl, IDL_CLEAR_FPLMN);
#endif
#if !defined(FEATURE_GSM) && !defined(FEATURE_WCDMA)
         (void) IMENUCTL_DeleteItem(ctl, IDL_AMR_PREF);
#endif

#if !defined(FEATURE_SEC_TESTAPPS) && !defined(FEATURE_SEC_IPSEC_VPN_CLNT)
         (void)  IMENUCTL_DeleteItem(ctl, IDL_SECURITY_SERVICES);
#endif
#ifndef FEATURE_VOICE_FIXED_PATTERN_DEBUG
         (void) IMENUCTL_DeleteItem(ctl, IDL_VOICE_DEBUG);
#endif /* FEATURE_VOICE_FIXED_PATTERN_DEBUG */
#ifndef srIMON
         (void)IMENUCTL_DeleteItem(ctl, IDL_STRIDE_CHGSTATE);
#endif
#ifndef FEATURE_CALL_THROTTLE
         (void)IMENUCTL_DeleteItem(ctl, IDL_THROTTLE_CM);
#endif

         IMENUCTL_SetSel(ctl, pme->m_debugmenu_sel);
      }
      return TRUE;

   case EVT_DIALOG_END:
      return TRUE;

   default:
      break;
   }
   return FALSE;
}

/*===========================================================================

FUNCTION CFieldDebug_TopMenuHandleEvent

DESCRIPTION
  Handle the key/command event for the top menu.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean CFieldDebug_TopMenuHandleEvent(CFieldDebug * pme,
                                           AEEEvent eCode,
                                           uint16 wParam,
                                           uint32 dwParam)
{
#ifdef FEATURE_ICM
   ICM *pICM = NULL;
   int nReturnStatus;
#endif
   switch (eCode) {
   case EVT_COMMAND:
      if (wParam == IDL_PROGRAM_I) {
         // Start the service programming applet
         return(SUCCESS == ISHELL_StartApplet(pme->a.m_pIShell,
                                              AEECLSID_SVCPRGAPP));
      }

      if (wParam == IDL_VERSION_I)
      {
         (void) CFieldDebug_MoveToDialog(pme, IDD_VERSION_DIALOG);
         return TRUE;
      }

      if (wParam == IDL_DEBUG_I)
      {
         (void) CFieldDebug_MoveToDialog(pme, IDD_DEBUG_DIALOG);
         return TRUE;
      }

      if (wParam == IDL_RATS_I)
      {
         (void) CFieldDebug_MoveToDialog(pme, IDD_RATS_DIALOG);
         return TRUE;
      }

      //Create JPEG Dialog
      if (wParam == IDL_JPEGD_I)
      {
    #ifdef FEATURE_JPEG_DECODER
        (void) ISHELL_EndDialog(pme->a.m_pIShell); //End current dialog.
        (void) ISHELL_CreateDialog(pme->a.m_pIShell, FLDDBG_RES_FILE, IDD_JPEG_DECODE, NULL); //Show JPEG dialog.
    #endif /* FEATURE_JPEG_DECODER */
        return TRUE;
      }

      if (wParam == IDL_NO_CALL_DISP)
      {
         (void) CFieldDebug_MoveToDialog(pme, IDD_DISABLE_IN_CALL_DISP);
         return TRUE;
      }

      if (wParam == IDL_MAKE_GAME_MODE_PERSIST)
      {
         (void) CFieldDebug_MoveToDialog(pme, IDD_MAKE_GAME_MODE_PERSIST);
         return TRUE;
      }

      return FALSE;

   case EVT_KEY:
      switch (wParam) {

#if defined(FEATURE_APP_QUICKTEST)
      case AVK_STAR:
         (void) ISHELL_StartApplet(pme->a.m_pIShell,
                                   AEECLSID_APP_QUICKTEST);
         return TRUE;


#endif

      case AVK_CLR:
         // return to the idle screen
         (void) ISHELL_CloseApplet(pme->a.m_pIShell, FALSE);
         return TRUE;


      default:
         break;
      }
      break;

   case EVT_DIALOG_START:
      (void) CFieldDebug_OnDialogStart (pme, wParam, dwParam);
      {
#ifdef FEATURE_ICM
         AEECMCallInfo *ci;
#endif


         IMenuCtl *ctl;
         IDialog *dlg;

         dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
         ctl = (IMenuCtl *) IDIALOG_GetControl(dlg, IDC_SYS_MENU);
#ifdef FEATURE_ICM
         /* Create the Call Manager object. */
         nReturnStatus = ISHELL_CreateInstance(pme->a.m_pIShell,
                               AEECLSID_CM,
                               (void **) &pICM);

         if(nReturnStatus != SUCCESS) {
           (void)CheckAEEReturnStatus(nReturnStatus); /* error checking */
           return FALSE;
         }

         if (pICM == NULL) { /* error */
           return FALSE;
         }

         ci = (AEECMCallInfo *) MALLOC (sizeof(AEECMCallInfo));
         if (ci != NULL) {
           (void) ICM_GetCallInfo(pICM,
                         0, /* TODO, put in the correct call id */
                         ci,
                         sizeof(AEECMCallInfo));
           ci->call_state = AEECM_CALL_STATE_IDLE;
         }
         else {
           return FALSE;
         }
         (void) ICM_Release(pICM);
         pICM = NULL;

         // Not allowed into Service Programming while in a call.
         if (ci != NULL && ci->call_state != AEECM_CALL_STATE_IDLE )
         {
            (void) IMENUCTL_DeleteItem(ctl, IDL_PROGRAM_I);
         }
         if (ci != NULL)
           FREE(ci);
#endif
         /* Disable the Remote API Test if the object didnt get created. */
         if (ctl)
         {
           if (pme->m_pIRats == NULL)
           {
             (void) IMENUCTL_DeleteItem(ctl, IDL_RATS_I);
           }

           // delete version menu
           (void) IMENUCTL_DeleteItem(ctl, IDL_VERSION_I);
         }
      }
      return TRUE;

   case EVT_DIALOG_END:
      return TRUE;

   default:
      break;
   }

   return FALSE;

}


/*===========================================================================

FUNCTION WSTR_TO_IP

DESCRIPTION
  This function will take a AECHAR string and convert it into a
  32 bit IP address in Host Order.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if we could convert the string, FALSE if not.

SIDE EFFECTS
  None.

===========================================================================*/
boolean WSTR_TO_IP(const AECHAR *in, uint32 *out)
{
  char string[MAX_IP_SIZE+1];

  /* Will the string fit? */
  if (WStrLen(in) > MAX_IP_SIZE)
    return FALSE;

  /* Yes it does, convert it to an ASCII string for conversion. */
  (void) WSTRTOSTR(in, string, sizeof(string));
  return (INET_ATON(string, out));
}


/*===========================================================================

FUNCTION IP_TO_WSTR

DESCRIPTION
  This function takes the incoming IP string and builds the associated
  AECHAR string for display.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void IP_TO_WSTR(uint32 ip, AECHAR *wstr, int size)
{
  char string[MAX_IP_SIZE+1];

  INET_NTOA(ip, string, MAX_IP_SIZE+1);
  (void) STR_TO_WSTR(string, wstr, size);
}

/*===========================================================================

FUNCTION CFieldDebug_RATSHandleEvent

DESCRIPTION
  Handle the key/command event for the top RATS menu.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if we handled the event, False otherwise..

SIDE EFFECTS
  None.

===========================================================================*/
void CFieldDebug_RATS_SetControlText(CFieldDebug *pme,
                                IMenuCtl    *pm,
                                uint16       control_id,
                                uint16       title_id,
                                uint32       ip_address)
{
  static AECHAR title_string[MAX_RES_SIZE];    /* Final Resource value. */
  static AECHAR spacer_string[10];             /* Spacer Resource value. */
  static AECHAR ip_address_str[MAX_IP_SIZE+1]; /* IP string */

  /* Get the SERVER string. */
  (void) ISHELL_LoadResString(pme->a.m_pIShell, FLDDBG_RES_FILE, title_id,
                              title_string, sizeof(title_string));

  if ((ip_address != 0) && (ip_address != 0xFFFFFFFF))
  {
    /* Get the string introducing the IP address */
    (void) ISHELL_LoadResString(pme->a.m_pIShell, FLDDBG_RES_FILE, IDS_LOCAL_IP,
                                spacer_string, sizeof(spacer_string));

    /* Get the IP address itself. */
    IP_TO_WSTR(ip_address, ip_address_str, sizeof(ip_address_str));

    /* Make sure it all fits. */
    if ((uint32) (WSTRLEN(title_string) + WSTRLEN(ip_address_str) +
         WSTRLEN(spacer_string)) < (sizeof(title_string)/sizeof(AECHAR)))
    {
      (void) WSTRLCAT(title_string, spacer_string, sizeof(title_string));
      (void) WSTRLCAT(title_string, ip_address_str, sizeof(title_string));
    }
  }

  /* We have a string now, associate that with the item. */
  IMENUCTL_SetItemText(pm, control_id, FLDDBG_RES_FILE, 0, title_string);
}

/*===========================================================================

FUNCTION CFieldDebug_RATSHandleEvent

DESCRIPTION
  Handle the key/command event for the top RATS menu.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if we handled the event, False otherwise..

SIDE EFFECTS
  None.

===========================================================================*/
static boolean CFieldDebug_RATSHandleEvent(CFieldDebug * pme,
                                           AEEEvent eCode,
                                           uint16 wParam,
                                           uint32 dwParam)
{
  IMenuCtl *pm = NULL;          /* Menu Control */
  IDialog *p_dialog = NULL;     /* Current Dialog control */
  uint32 ip_address;            /* IP for Server and Client. */
  static AECHAR res_string[MAX_RES_SIZE]; /* Resource value. */
  int   NewStatus;             /* When changing Status, sets the NEW. */

  if (pme == NULL || pme->m_pIRats == NULL)
    return FALSE;

  switch (eCode) {
    case EVT_COMMAND:
      p_dialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);

      switch (wParam)
      {
        /* Selected the Client Line, change the status. */
        case IDC_RATS_CLIENT:
          /* If its down, bring it up.  Otherwise, bring it down. */
          if (IRATS_GetClientStatus(pme->m_pIRats) == IRATS_STATUS_DOWN)
            NewStatus = IRATS_STATUS_UP;
          else
            NewStatus = IRATS_STATUS_DOWN;

          if (IRATS_SetClientStatus(pme->m_pIRats, NewStatus) != AEE_SUCCESS)
          {
            ERR("Could not Set Client Status", 0, 0, 0);
          }

          /* We changed the status, so, event or not, put a new image. */
          pm = (IMenuCtl *) IDIALOG_GetControl (p_dialog, IDC_RATS_MENU);
          CFieldDebug_SetImage(pm, IDC_RATS_CLIENT,
                               IRATS_GetClientStatus(pme->m_pIRats));
          break;

        case IDC_RATS_SERVER:
          /* If the SERVER is down, bring it up.  Otherwise, try to bring it down. */
          if (IRATS_GetServerStatus(pme->m_pIRats) == IRATS_STATUS_DOWN)
            NewStatus = IRATS_STATUS_UP;
          else
            NewStatus = IRATS_STATUS_DOWN;

          if (IRATS_SetServerStatus(pme->m_pIRats, NewStatus) != AEE_SUCCESS)
          {
            ERR("Could not Set Server Status", 0, 0, 0);
          }

          /* Update the image for the new status. */
          pm = (IMenuCtl *) IDIALOG_GetControl (p_dialog, IDC_RATS_MENU);
          CFieldDebug_SetImage(pm, IDC_RATS_SERVER,
                               IRATS_GetServerStatus(pme->m_pIRats));
          break;

        case IDL_SK_SET_IP:
          (void) CFieldDebug_MoveToDialog(pme, IDD_RATS_IP_DIALOG);
          break;

        case IDL_SK_RUNTEST:
          (void) CFieldDebug_MoveToDialog(pme, IDD_RATS_TEST_DIALOG);
          break;

        default:
          return FALSE;
      }
      IDIALOG_Redraw(p_dialog);
      return TRUE;

    case EVT_NOTIFY:
      {
        /* Event Notifications occuring in this dialog would indicate we need
         * to change the ICON.  Make sure thats the case, and then update all
         * of our ICONs. */
        AEENotify *notif = (AEENotify *) dwParam;
        if (notif->dwMask & NMASK_STATUS_CHANGE)
        {
          /* STATUS CHANGE notification.  Update our ICONS, first
           * the SERVER item is always available, update that */
          p_dialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
          pm = (IMenuCtl *) IDIALOG_GetControl (p_dialog, IDC_RATS_MENU);

          /* Set the SERVER string. */
          CFieldDebug_RATS_SetControlText(pme, pm, IDC_RATS_SERVER,
                                         IDS_SERVER_UP,
                                         IRATS_GetLocalIP(pme->m_pIRats));

          /* Set the SERVER Icon. */
          CFieldDebug_SetImage(pm, IDC_RATS_SERVER,
                               IRATS_GetServerStatus(pme->m_pIRats));

          /* Set the CLIENT string. */
          CFieldDebug_RATS_SetControlText(pme, pm, IDC_RATS_CLIENT,
                                         IDS_CLIENT_UP,
                                         IRATS_GetRemoteIP(pme->m_pIRats));

          /* Set the CLIENT Icon. */
          CFieldDebug_SetImage(pm, IDC_RATS_CLIENT,
                               IRATS_GetClientStatus(pme->m_pIRats));
          IDIALOG_Redraw(p_dialog);
          return TRUE;
        }
      }
      return FALSE;

    case EVT_DIALOG_START:
      (void) CFieldDebug_OnDialogStart (pme, wParam, dwParam);

      p_dialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
      if (p_dialog)
        pm = (IMenuCtl *) IDIALOG_GetControl (p_dialog, IDC_RATS_MENU);

      if (pm)
      {
        IMENUCTL_SetProperties(pm, MP_MULTI_SEL);
        (void) IMENUCTL_DeleteAll(pm);

        if ((ip_address = IRATS_GetRemoteIP(pme->m_pIRats)) == 0)
        {
          IDBMgr *pIDBMgr = NULL;
          IDatabase *pIDatabase = NULL;
          IDBRecord *pIDBRecord = NULL;

          (void) ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_DBMGR,
                                       (void **) &pIDBMgr);
          if (pIDBMgr)
          {
            pIDatabase = IDBMGR_OpenDatabase(pIDBMgr, IPFILE, FALSE);
            if (pIDatabase != NULL)
            {
              pIDBRecord = IDATABASE_GetRecordByID(pIDatabase, 0);
              if (pIDBRecord != NULL)
              {
                AEEDBFieldName Name;
                uint16         nLen;

                if ((IDBRECORD_NextField(pIDBRecord, &Name, &nLen) ==
                                                             AEEDB_FT_DWORD) &&
                    (Name == AEEDBFIELD_ADDRESS) &&
                    (nLen == sizeof(uint32)))
                {
                  if (IDBRECORD_GetFieldDWord(pIDBRecord, &ip_address))
                  {
                    (void) IRATS_SetRemoteIP(pme->m_pIRats, ip_address);
                  }
                }
                (void) IDBRECORD_Release(pIDBRecord);
              }
              (void) IDATABASE_Release(pIDatabase);
            }
            (void) IDBMGR_Release(pIDBMgr);
          }
        }
        if (ip_address != 0)
        {
          /* Get the CLIENT string. */
          (void) ISHELL_LoadResString(pme->a.m_pIShell, FLDDBG_RES_FILE,
                                      IDS_CLIENT_UP, res_string,
                                      sizeof(res_string));

          /* Add the client menu item to the default string. */
          (void) IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_CLIENT_UP,
                                  IDC_RATS_CLIENT, res_string, IDC_RATS_CLIENT);

          /* Set the CLIENT string to a specific value for the state. */
          CFieldDebug_RATS_SetControlText(pme, pm, IDC_RATS_CLIENT,
                                         IDS_CLIENT_UP,
                                         ip_address);

          /* Set the CLIENT Icon. */
          CFieldDebug_SetImage(pm, IDC_RATS_CLIENT,
                               IRATS_GetClientStatus(pme->m_pIRats));
        }

        /* Get the SERVER string. */
        (void) ISHELL_LoadResString(pme->a.m_pIShell, FLDDBG_RES_FILE,
                                    IDS_SERVER_UP, res_string,
                                    sizeof(res_string));

        /* Add the SERVER menu item. */
        (void) IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_SERVER_UP,
                                IDC_RATS_SERVER, res_string, IDC_RATS_SERVER);

        /* Set the SERVER string to a context specific value. */
        CFieldDebug_RATS_SetControlText(pme, pm, IDC_RATS_SERVER,
                                       IDS_SERVER_UP,
                                       IRATS_GetLocalIP(pme->m_pIRats));

        /* Set the Server Image. */
        CFieldDebug_SetImage(pm, IDC_RATS_SERVER,
                             IRATS_GetServerStatus(pme->m_pIRats));

        /*lint -e{613} */
        IDIALOG_Redraw(p_dialog);
      }
      return TRUE;

    case EVT_DIALOG_END:
      return TRUE;

    default:
      break;
  }

  return FALSE;
}

/*===========================================================================

FUNCTION CFieldDebug_RATS_TESTHandleEvent

DESCRIPTION
  Handle the key/command event for the IP menu.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean CFieldDebug_RATS_TESTHandleEvent(CFieldDebug * pme,
                                           AEEEvent eCode,
                                           uint16 wParam,
                                           uint32 dwParam)
{
  IDialog *p_dialog = NULL;
  ITextCtl *ptext = NULL;
  IStatic *pstatic = NULL;

  if ((pme == NULL) || (pme->m_pIRats == NULL))
    return FALSE;

  switch (eCode) {
    case EVT_KEY_PRESS:
        p_dialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
        if (!p_dialog)
          return FALSE;

        pstatic = (IStatic *) IDIALOG_GetControl (p_dialog, IDC_RATS_TEST_STATIC);
        ptext = (ITextCtl *) IDIALOG_GetControl(p_dialog, IDC_RATS_TEST_TEXT);
        if (!pstatic || !ptext)
          return FALSE;

        switch ( wParam )
        {
          case AVK_UP:
          case AVK_DOWN:
            ITEXTCTL_SetActive(ptext, FALSE);
            ISTATIC_SetActive(pstatic, TRUE);
            //IDIALOG_SetFocus(p_dialog, IDC_RATS_TEST_STATIC);
            break;

          default:
            ISTATIC_SetActive(pstatic, FALSE);
            ITEXTCTL_SetActive(ptext, TRUE);
            //IDIALOG_SetFocus(p_dialog, IDC_RATS_TEST_TEXT);
            break;
        }
        return FALSE;

    case EVT_KEY_RELEASE:
        p_dialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
        if (!p_dialog)
          return FALSE;
        ptext = (ITextCtl *) IDIALOG_GetControl(p_dialog, IDC_RATS_TEST_TEXT);
        pstatic = (IStatic *)IDIALOG_GetControl (p_dialog, IDC_RATS_TEST_STATIC);
        if (!pstatic || !ptext)
          return FALSE;

        ISTATIC_SetActive(pstatic, FALSE);
        ITEXTCTL_SetActive(ptext, TRUE);
        // IDIALOG_SetFocus(p_dialog, IDC_RATS_TEST_TEXT);
        IDIALOG_Redraw(p_dialog);
        return TRUE;

    case EVT_KEY:
      switch (wParam)
      {
        case AVK_CLR:
          (void) CFieldDebug_MoveToDialog(pme, IDD_RATS_DIALOG);
          return TRUE;
        case AVK_SELECT:
          {
            AECHAR buffer[MAX_TEST_DIGITS+1];
            char dest[MAX_TEST_DIGITS+1];
            uint32 test_num;

            p_dialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
            ptext = (ITextCtl *) IDIALOG_GetControl (p_dialog, IDC_RATS_TEST_TEXT);
            memset(buffer, 0, sizeof(buffer));
            (void) ITEXTCTL_GetText(ptext, buffer,
                                    sizeof(buffer)/sizeof(AECHAR));
            if (WSTRLEN(buffer) > 0)
            {
              (void) WSTRTOSTR(buffer, dest, sizeof(dest));
              test_num = ATOI(dest);
              (void) IRATS_RunTest(pme->m_pIRats, test_num);
              (void) ITEXTCTL_SetText(ptext, NULL, 0);
            }
          }
          break;

        default:
          break;
      }

      return FALSE;

    case EVT_DIALOG_START:
      (void) CFieldDebug_OnDialogStart (pme, wParam, dwParam);
      {
        AEERect rect;
        int   font_size;

        font_size = IDISPLAY_GetFontMetrics(pme->a.m_pIDisplay, AEE_FONT_NORMAL,
                                            NULL, NULL);

        ptext = (ITextCtl *) IDIALOG_GetControl ((IDialog *) dwParam,
                                              IDC_RATS_TEST_TEXT);
        rect.x = 0;
        rect.y = 0;
        rect.dx = pme->m_screen_rc.dx;
        rect.dy = (int16) (font_size * 3);  /* Box, Title, Number */
        ITEXTCTL_SetRect(ptext, &rect);
        ITEXTCTL_SetProperties(ptext, TP_FRAME);
        (void) ITEXTCTL_SetInputMode(ptext, AEE_TM_NUMBERS);
        ITEXTCTL_SetMaxSize(ptext, MAX_TEST_DIGITS);

        rect.y = rect.dy;
        rect.dy = pme->m_screen_rc.dy - rect.y - font_size;
        pstatic = (IStatic *) IDIALOG_GetControl ((IDialog *) dwParam,
                                              IDC_RATS_TEST_STATIC);
        ISTATIC_SetProperties(pstatic,
                              ST_ENABLE_HLGHT | ST_NOSCROLL | ST_TEXTALLOC );
        ISTATIC_SetFont(pstatic, AEE_FONT_NORMAL, AEE_FONT_NORMAL);
        ISTATIC_SetRect(pstatic, &rect);
        (void) ISTATIC_SetText(pstatic, NULL, NULL,
                               AEE_FONT_NORMAL, AEE_FONT_NORMAL);

        ISTATIC_Reset(pstatic);
        pme->m_LastFileLocation = 0;
        pme->m_LastLineNumber = 0;
        CFieldDebug_FillTest(pme, pstatic);
        (void) ISTATIC_Redraw(pstatic);
      }
      return TRUE;

    case EVT_DIALOG_END:
      return TRUE;

    case EVT_NOTIFY:
      {
        AEENotify *notif = (AEENotify *) dwParam;
        if (notif->dwMask & NMASK_DATA_AVAILABLE)
        {
          p_dialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
          pstatic = (IStatic *) IDIALOG_GetControl (p_dialog, IDC_RATS_TEST_STATIC);

          CFieldDebug_FillTest(pme, pstatic);

          IDIALOG_Redraw(p_dialog);
          return TRUE;
        }
      }
      return FALSE;

    default:
      break;
  }
  return FALSE;
}


/*===========================================================================

FUNCTION CFieldDebug_SetRemoteIP

DESCRIPTION
  If we have received a remote IP, this function will save it into
  our database as well as tell IRATS about it.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the buffer looked good, false otherwise..

SIDE EFFECTS
  None.

===========================================================================*/
boolean CFieldDebug_SetRemoteIP(CFieldDebug * pme, AECHAR *IP_buffer)
{
  uint32 new_ip;

  if (! pme)
  {
    MSG_ERROR("NULL pointer", 0, 0, 0);
    return FALSE;
  }

  if ((WSTRLEN(IP_buffer) > 0) && (WSTR_TO_IP(IP_buffer, &new_ip)))
  {
    IDBMgr *pIDBMgr = NULL;
    IDatabase *pIDatabase = NULL;
    IDBRecord *pIDBRecord = NULL;

    (void) ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_DBMGR,
                                 (void **) &pIDBMgr);
    if (pIDBMgr)
    {
      pIDatabase = IDBMGR_OpenDatabase(pIDBMgr, IPFILE, TRUE);
      if (pIDatabase != NULL)
      {
        AEEDBField local_DBField;

        local_DBField.fType = AEEDB_FT_DWORD;
        local_DBField.fName = AEEDBFIELD_ADDRESS;
        local_DBField.wDataLen = sizeof(uint32);
        local_DBField.pBuffer = &new_ip;
        pIDBRecord = IDATABASE_GetRecordByID(pIDatabase, 0);
        if (pIDBRecord != NULL)
        {
          (void) IDBRECORD_Update(pIDBRecord, &local_DBField, 1);
          (void) IDBRECORD_Release(pIDBRecord);
        }
        else
        {
          pIDBRecord = IDATABASE_CreateRecord(pIDatabase,
                                              &local_DBField, 1);
          (void) IDBRECORD_Release(pIDBRecord);
        }
        (void) IDATABASE_Release(pIDatabase);
      }
      (void) IDBMGR_Release(pIDBMgr);
    }
    (void) IRATS_SetRemoteIP(pme->m_pIRats, new_ip);
    return TRUE;
  }
  return FALSE;
}

/*===========================================================================

FUNCTION CFieldDebug_IPHandleEvent

DESCRIPTION
  Handle the key/command event for the IP menu.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean CFieldDebug_IPHandleEvent(CFieldDebug * pme,
                                           AEEEvent eCode,
                                           uint16 wParam,
                                           uint32 dwParam)
{
  IMenuCtl *psk = NULL;
  IDialog *p_dialog = NULL;
  ITextCtl *ptext = NULL;
  AECHAR IP_buffer[MAX_IP_SIZE+1];
  int    position;

  if ((pme == NULL) || (pme->m_pIRats == NULL))
    return FALSE;

  switch (eCode) {
    case EVT_COMMAND:
      p_dialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
      ptext = (ITextCtl *) IDIALOG_GetControl (p_dialog, IDC_RATS_IP_TEXT);

      switch (wParam)
      {
        case IDL_RATS_IP_OK:
          if (ITEXTCTL_GetText(ptext, IP_buffer, MAX_IP_SIZE+1))
          {
            if (CFieldDebug_SetRemoteIP(pme, IP_buffer))
            {
              (void) CFieldDebug_MoveToDialog(pme, IDD_RATS_DIALOG);
              return TRUE;
            }
          }
          return FALSE;

        case IDL_RATS_IP_CANCEL:
          (void) CFieldDebug_MoveToDialog(pme, IDD_RATS_DIALOG);
          return TRUE;

        default:
          break;
      }
      return FALSE;

    case EVT_KEY:
      switch (wParam)
      {
        case AVK_SELECT:
          p_dialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
          ptext = (ITextCtl *) IDIALOG_GetControl (p_dialog, IDC_RATS_IP_TEXT);

          if (ITEXTCTL_GetText(ptext, IP_buffer, MAX_IP_SIZE+1))
          {
            if (CFieldDebug_SetRemoteIP(pme, IP_buffer))
            {
              (void) CFieldDebug_MoveToDialog(pme, IDD_RATS_DIALOG);
              return TRUE;
            }
          }
          return FALSE;

        case AVK_CLR:
          (void) CFieldDebug_MoveToDialog(pme, IDD_RATS_DIALOG);
          return TRUE;

        case AVK_STAR:
          {
            AECHAR buffer[MAX_IP_SIZE+1];
            AECHAR dest_buffer[MAX_IP_SIZE+1];
            int    total_length;
            p_dialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
            ptext = (ITextCtl *) IDIALOG_GetControl (p_dialog, IDC_RATS_IP_TEXT);

            (void) ITEXTCTL_GetText(ptext, buffer, MAX_IP_SIZE);
            position = ITEXTCTL_GetCursorPos(ptext);
            if ((position >= 0) && (position < MAX_IP_SIZE))
            {
              total_length = WSTRLEN(buffer);
              if (total_length < MAX_IP_SIZE)
              {
                (void) WSTRNCOPYN(dest_buffer, MAX_IP_SIZE, buffer, position);
                dest_buffer[position] = (AECHAR) '.';
                (void) WSTRNCOPYN(dest_buffer+position+1,
                                  MAX_IP_SIZE-position, buffer+position,
                                  total_length-position+1);
                (void) ITEXTCTL_SetText(ptext, dest_buffer, total_length+1);
                ITEXTCTL_SetCursorPos(ptext, position+1);
              }
            }
            return TRUE;
          }

        default:
          break;
      }
      return FALSE;

    case EVT_DIALOG_START:
      (void) CFieldDebug_OnDialogStart (pme, wParam, dwParam);
      {
        uint32 ip_address;
        AECHAR res_string[MAX_RES_SIZE+1];
        ptext = (ITextCtl *) IDIALOG_GetControl ((IDialog *) dwParam,
                                              IDC_RATS_IP_TEXT);
        (void) ITEXTCTL_SetInputMode(ptext, AEE_TM_NUMBERS);
        ITEXTCTL_SetMaxSize(ptext, MAX_IP_SIZE);

        if ((ip_address = IRATS_GetRemoteIP(pme->m_pIRats)) != 0)
        {
          IP_TO_WSTR(ip_address, res_string, sizeof(res_string));

          position = WSTRLEN(res_string);
          (void) ITEXTCTL_SetText(ptext, res_string, MAX_IP_SIZE);
          ITEXTCTL_SetCursorPos(ptext, position);
        }

        psk = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                              IDC_RATS_IP_SOFTKEY);
        (void) IMENUCTL_DeleteAll(psk);
        (void) ISHELL_LoadResString(pme->a.m_pIShell, FLDDBG_RES_FILE, IDS_OK,
                                    res_string, sizeof(res_string));
        (void) IMENUCTL_AddItem(psk, FLDDBG_RES_FILE, IDS_OK, IDL_RATS_IP_OK,
                                res_string, IDL_RATS_IP_OK);
        (void) ISHELL_LoadResString(pme->a.m_pIShell, FLDDBG_RES_FILE,
                                    IDS_CANCEL, res_string, sizeof(res_string));
        (void) IMENUCTL_AddItem(psk, FLDDBG_RES_FILE, IDS_CANCEL,
                            IDL_RATS_IP_CANCEL, res_string, IDL_RATS_IP_CANCEL);
      }
      return TRUE;

    case EVT_DIALOG_END:
      return TRUE;

    default:
      break;
  }
  return FALSE;
}

/*===========================================================================

FUNCTION CFieldDebug_InitDialog

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean CFieldDebug_InitDialog( CFieldDebug *pme, IDialog *dlg )
{
  PFNAEEEVENT pDlgEvtHandler = NULL;

  if (!pme || !dlg)
  {
    MSG_ERROR("Null ptr, 0x%x 0x%x", pme, dlg, 0);
    return FALSE;
  }

  pDlgEvtHandler = CFieldDebug_GetDlgEvtHandler(dlg);

  if (pDlgEvtHandler == NULL)
  {
    MSG_ERROR("NULL event handler.", 0, 0, 0);
    return FALSE;
  }

  IDIALOG_SetEventHandler(dlg, pDlgEvtHandler, (void *)pme);
  IDIALOG_SetProperties(dlg,
                        IDIALOG_GetProperties(dlg) | DLG_HANDLE_ALL_EVENTS);

  return TRUE;
}

#ifdef FEATURE_APP_TEST_AUTOMATION
/*===========================================================================
FUNCTION CallbackTimer

DESCRIPTION
        This function acts as a timer function when callbacks never get
        called for any reason. Callbacks are intialized in SetCallback function.
        Each callback function needs to cancel the timer. 

DEPENDENCIES
        NONE

RETURN VALUE
        NONE

SIDE EFFECTS
        NONE
          
/==========================================================================*/  
static void CFieldDebug_CallbackTimer(void *pUser)
{
  CFldDbg_CallbackTimerData *pTimerData = (CFldDbg_CallbackTimerData *)pUser;
  uint16                        nTransactionId;
  uint32                        nDiagEvtId;
  
  if (! pTimerData)
  {
    MSG_ERROR("Null data pointer in the call back timer", 0, 0, 0);
    return;
  }

  nTransactionId = pTimerData->XactId;
  nDiagEvtId = pTimerData->nDiagEvtId;

  if (IDIAG_SendDiagResponse(pTimerData->pIDiag, (uint32)IDIAG_DIAG_ERROR, 
                  nTransactionId, TRUE, sizeof(&(nDiagEvtId)), 
                  (void *)&nDiagEvtId) != AEE_SUCCESS)
  {
    MSG_ERROR("Failed to send error response for call back timer", nTransactionId, nDiagEvtId, 0);
  }
}

#if ! MIN_BREW_VERSION(3,1)
// This function is only used for the pre-BREW3.1 implementation of App Stop
/*===========================================================================

FUNCTION CFieldDebug_CloseAppletCB

DESCRIPTION
  Callback used to close an applet based on a diag request.  It should
  be invoked from the context of the applet to be closed.  It then posts
  an event back to Field Debug to indicate the success or failure of the
  attempt to close the app.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Closes the applet in whose context it is invoked.

===========================================================================*/
static void CFieldDebug_CloseAppletCB(void *pUser)
{
   CFldDbg_CloseAppCBData *pCBData = (CFldDbg_CloseAppCBData *)pUser;
   int     nShellSuccess;
   boolean bPostSuccess;

   if (! pCBData)
   {
      MSG_ERROR("Null data pointer in close applet callback.", 0, 0, 0);
      return;
   }

   nShellSuccess = ISHELL_CloseApplet(pCBData->pIShell, FALSE);
   if (AEE_SUCCESS != nShellSuccess)
   {
      MSG_ERROR("Unable to close applet for diag request, error code 0x%x.",
                 nShellSuccess, 0, 0);
   }

   bPostSuccess = ISHELL_PostEvent(pCBData->pIShell, AEECLSID_FIELDDEBUGAPP,
                                   EVT_FLDDBG_DIAG_APP_CLOSE_COMPLETE,
                                   pCBData->XactId, (uint32)nShellSuccess);
   if (! bPostSuccess)
   {
      MSG_ERROR("Unable to post event after app close, close error code 0x%x.",
                nShellSuccess, 0, 0);
   }

   return;

}

#endif // ! MIN_BREW_VERSION(3,1)

/*===========================================================================

FUNCTION CFieldDebug_GetActiveDlgCB

DESCRIPTION
  Callback used to the get the active dialog ID.  It should
  be invoked from the context of the applet currently active.  It then posts
  an event back to Field Debug to indicate the success or failure of the
  attempt to get the id.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void CFieldDebug_GetActiveDlgCB(void *pUser)
{
   CFldDbg_GetActiveDlgCBData *pCBData = (CFldDbg_GetActiveDlgCBData *)pUser;     
   uint16       nDlgId;
   boolean      bPostSuccess;
   IDialog      *pDlg;

   if (! pCBData)
   {
      MSG_ERROR("Null data pointer in get active dialog callback.", 0, 0, 0);
      return;
   }
   
   pDlg = ISHELL_GetActiveDialog(pCBData->pIShell);
   if (pDlg == NULL)
   {
     MSG_ERROR("Failed to get dialog pointer for get active dialog request.Trasaction ID 0x%x.",
           pCBData->XactId, 0, 0);
     return;
   } 
   else
   {
     nDlgId = IDIALOG_GetID(pDlg);
   }
   
   bPostSuccess = ISHELL_PostEvent(pCBData->pIShell, AEECLSID_FIELDDEBUGAPP,
                                   EVT_FLDDBG_DIAG_GETACTIVEDLG_COMPLETE,
                                   pCBData->XactId, (uint32)nDlgId);
   if (! bPostSuccess)
   {
      MSG_ERROR("Unable to post event after get the active app id. Trasaction ID 0x%x.",
            pCBData->XactId, 0, 0);
   }

   return;

}

/*===========================================================================

FUNCTION CFieldDebug_GetMenuCtlTxtCB

DESCRIPTION
  Callback used to the get the menu control text.  It should be invoked 
  from the context of the applet currently active.  It then posts
  an event back to Field Debug to indicate the success or failure of the
  attempt to get the text.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void CFieldDebug_GetMenuCtlTxtCB(void *pUser)
{
  CFldDbg_GetMenuCtlTxtCBData *pCBData = (CFldDbg_GetMenuCtlTxtCBData *)pUser;     
  uint16       nDlgId;
  IDialog      *pDlg;
  IControl     *pCtl;
  AECHAR       *CtlText = NULL;
  uint16       wSelItem;
  uint32       Error;
  CtlAddItem   CtlItem;
  boolean      bSuccess;
    
  if (! pCBData)
  {
    MSG_ERROR("Null data pointer in get menu control text callback.", 0, 0, 0);
    return;
  }

  pDlg = ISHELL_GetActiveDialog(pCBData->pIShell);
  if (pDlg == NULL)
  {
    MSG_ERROR("Failed to get dlg pointer for get menu ctl text request. Tr'n ID 0x%x.", 
      pCBData->XactId, 0, 0);
    return;
  }

  do
  {
    nDlgId = IDIALOG_GetID(pDlg);
    // Make sure the active dialog is the dialog in the request
    if (pCBData->DlgId != nDlgId)
    {
      pCBData->Error = (uint32)IDIAG_WRONG_DLG;
      break;
    }
    
    // Get a pointer to the text control
    pCtl = IDIALOG_GetControl(pDlg, (int16)pCBData->CtlId);
    if (pCtl == NULL)
    {
      pCBData->Error = (uint32)IDIAG_BAD_CTL_ID;
      break;
    }

    wSelItem = IMENUCTL_GetSel((IMenuCtl *)pCtl);

    bSuccess = IMENUCTL_GetItem((IMenuCtl *)pCtl, wSelItem, &CtlItem);
    if (bSuccess == FALSE)
    {
      pCBData->Error = (uint32)IDIAG_OTHER_INT_ERROR;
      break;
    }

    if (CtlItem.pText)
    {
      pCBData->Error = (uint32)IDIAG_SUCCESS;
      (void)WSTRTOSTR(CtlItem.pText, (char*)pCBData->CtlText, MAX_RES_SIZE);
    }
    else if (CtlItem.pszResText)
    {
      CtlText = (AECHAR *)MALLOC(MAX_RES_SIZE * sizeof(AECHAR));
      if (! CtlText)
      {
        pCBData->Error = (uint32)IDIAG_OTHER_INT_ERROR;
        break;
      }
      Error = (uint32)ISHELL_LoadResString(pCBData->pIShell, 
                                           CtlItem.pszResText, CtlItem.wText, 
                                           CtlText, MAX_RES_SIZE * sizeof(AECHAR));
      if (Error == 0)
      {
        pCBData->Error = (uint32)IDIAG_OTHER_INT_ERROR;
        break;
      }

      pCBData->Error = (uint32)IDIAG_SUCCESS;
      (void)WSTRTOSTR(CtlText, pCBData->CtlText, MAX_RES_SIZE);
    }  
  } while (0);

  bSuccess = ISHELL_PostEvent(pCBData->pIShell, AEECLSID_FIELDDEBUGAPP,
                                  EVT_FLDDBG_DIAG_GETSELMENUCTLTXT_COMPLETE,
                                  pCBData->XactId, (uint32)pCBData->CtlText);
  if (! bSuccess)
  {
    MSG_ERROR("Unable to post event after get the menu ctl text. Trasaction ID 0x%x.",
            pCBData->XactId, 0, 0);
  }

  FREEIF(CtlText);
  return;
}

/*===========================================================================
FUNCTION CFieldDebug_SetCallback

DESCRIPTION
   This is a common function for intializing the call backs and 
   making the call for AEE_APP_RESUME (Brew 3.0 or above) or AEE_ResumeEx
         (Brew2.1). It takes pointer to function handling the call back 
   and a pointer to data structure with data for call back.

   Callbacks are necessary when a brew call needs to be carried 
   out in the conext of active applet rather than the current 
   running applet, which is FieldDebugApp.
   
DEPENDENCIES
  none

RETURN VALUE
  boolean: Value indicating callback status. TRUE for success and 
  FALSE for failure.

SIDE EFFECTS
  none
/===========================================================================*/

#if (MIN_BREW_VERSION(4,0))
extern PACONTEXT App_New(IShell * piShell, AEECLSID cls, int wFlags,
                         boolean bCheck, int * pnErr, boolean * pIsNew);
#elif (MIN_BREW_VERSION(3, 0))
extern PACONTEXT App_New(IShell * piShell, AEECLSID cls, int wFlags,
                         int * pnErr, boolean * pIsNew);
#endif // (MIN_BREW_VERSION(3, 0))

static boolean CFieldDebug_SetCallback(
                CFieldDebug *pme,
                PFNNOTIFY pCBFunction, 
                /* pointer to call back function */   
                void *pCBData, // pointer to data
//lint -save -e715  Suppress "dwAppId not referenced". Used only in brew 2.1 code
                uint32 dwAppId // Id of the App which needs the call back
                )
{
  PACONTEXT   pAppContext;
  boolean     bCallBackStatus = FALSE;

   if (!pme)
   {
     MSG_ERROR("Null ptr", 0, 0, 0);
     return bCallBackStatus;
   }


  if ( (NULL == pCBFunction) || (NULL == pCBData) )
  {
    MSG_ERROR("Cannot initialize callback with null parameters", 0, 0, 0);
    return bCallBackStatus;
  }

  CALLBACK_Init(&(pme->m_AeeCallback),
                pCBFunction, pCBData);

  do
  {

#if (MIN_BREW_VERSION(3, 0))
    /* The necessary call must be made in the context of the required app.
     * We already know it's the active applet (else this function will
     * not be called), so get it's context and invoke a callback in 
     * that context.
     */
    pAppContext = App_New((IShell *)pme->a.m_pIShell, 
                          ISHELL_ActiveApplet(pme->a.m_pIShell), 0,
#if (MIN_BREW_VERSION(4, 0))
                          FALSE, // BREW 4.0 has an additional arg
#endif
                          NULL, NULL);

    if (! pAppContext)
    {
      MSG_ERROR("Unable to get context to close applet.", 0, 0, 0);
      break;
    } 

    bCallBackStatus = TRUE;
    AEE_APP_RESUME(&(pme->m_AeeCallback), pAppContext);

#else // Brew 2.1 or earlier
   /* For 2.1, we have to use a hack that assumes (correctly)
    * that the first four bytes of an ACONTEXT are a pointer to
    * the next ACONTEXT in the list of stacked applets.
    */
    pAppContext = AEE_GetStackedAppContext();
    while ((pAppContext) &&
                (AEE_GetAppContextCls(pAppContext) != dwAppId))
    {
      /* Search until we find the correct app context or hit the end
      * of the app context list
      */
      pAppContext = ((CFldDbg_AppCtxtCast *)(pAppContext))->pNextCtxt;
    }
    
    if ((! pAppContext) ||
                 (AEE_GetAppContextCls(pAppContext) != dwAppId))
    {
      MSG_ERROR("Unable to get context to the required applet.", 0, 0, 0);
      bCallbackStatus = FALSE;
      break;
    }
        
    bCallbackstatus = TRUE;
    AEE_ResumeEx(&(pme->m_AeeCallback), 0, pAppContext); 
    
#endif

  }while(0);
  
  return bCallBackStatus;
}

//lint +e715

#endif // FEATURE_APP_TEST_AUTOMATAION

/*===========================================================================

FUNCTION CFieldDebug_HandleEvent

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  boolean: TRUE indicating event was handled
  FALSE indicating event was not handled

SIDE EFFECTS
  None.

===========================================================================*/
static boolean CFieldDebug_HandleEvent(CFieldDebug  *pme,
                                       AEEEvent   eCode,
                                       uint16     wParam,
                                       uint32     dwParam)
{
   PARAM_NOT_REF(wParam)
   IShell *ps;
   AEEDeviceInfo DeviceInfo;
   IDialog *pIDialog;
   PFNAEEEVENT pDlgEvtHandler = NULL;
   int nRetVal;
#ifdef FEATURE_MMGSDI
   gsdi_cnf_T *pGSDIConf;
   int dummy_rec_ptr;
   uint32 client_ref = 0;
   byte                         data[GSDI_MAX_DATA_BLOCK_LEN];
   gsdi_returns_T gsdi_status = GSDI_ERROR;
   boolean bGSDIError = FALSE;
   gsdi_file_attributes_T fplmn_attributes;
   uint16 wDlgID;
   boolean   bRetVal = TRUE;
#endif /* FEATURE_MMGSDI */
#ifdef FEATURE_APP_TEST_AUTOMATION
   uint16 XactId = 0;
   uint32 DiagStatus = (uint32)IDIAG_OTHER_INT_ERROR;
#endif

   if (!pme)
   {
     MSG_ERROR("Null ptr", 0, 0, 0);
     return FALSE;
   }

   ps = pme->a.m_pIShell;
   if(ps == NULL)
   {
     MSG_ERROR("IShell Null", 0, 0, 0);
     return FALSE;
   }

   pIDialog = ISHELL_GetActiveDialog(ps);

   switch (eCode) {

   case EVT_APP_START_BACKGROUND:
     /* We're being launched in the background, so go to the background. */
     MSG_MED("Received EVT_APP_START_BACKGROUND.", 0, 0, 0);
     nRetVal = ISHELL_CloseApplet(pme->a.m_pIShell, FALSE);
     if (nRetVal != SUCCESS)
     {
       MSG_ERROR("Unable to go background.", 0, 0, 0);
     }
     return TRUE;

   case EVT_APP_START:
     pme->bEndKeyProcess = FALSE;
     pme->m_dlgID = IDD_TOP_DIALOG;
     if (ISHELL_CreateDialog(ps,
                             FLDDBG_RES_FILE,
                             pme->m_dlgID,
                             NULL) != AEE_SUCCESS) {
        MSG_ERROR ("Can't Create Top Dialog", 0, 0, 0 );
     }
     // Dont reset port map when mmc is already open
     if (pme->m_nMMCDLState != FLDDBG_MMC_DOWNLOAD_IN_DL_BG)
     {
       pme->old_port_map = RDM_NULL_DEV;
       pme->old_port_map1 = RDM_NULL_DEV;
       pme->old_port_map2 = RDM_NULL_DEV;
     }
     DeviceInfo.wStructSize = sizeof(DeviceInfo);
     ISHELL_GetDeviceInfo(ps, &DeviceInfo);
     pme->m_screen_rc.x = 0;
     pme->m_screen_rc.y = 0;
     pme->m_screen_rc.dx = (int16) DeviceInfo.cxScreen;
     pme->m_screen_rc.dy = (int16) DeviceInfo.cyScreen;
     pme->m_nMMCDLTime = (int32)FLDDBG_MMC_DL_TIME_UNLIMITED;
     return TRUE;

   case EVT_APP_SUSPEND:
      (void) ISHELL_CancelTimer(pme->a.m_pIShell, EndErrorMessageDlg, (void*) pme);
      return TRUE;

   case EVT_APP_RESUME:
    if (ISHELL_CreateDialog(ps, FLDDBG_RES_FILE, pme->m_dlgID, NULL) !=
        AEE_SUCCESS) {
       MSG_ERROR("Can't Create Top Dialog", 0, 0, 0 );
    }

    return TRUE;

   case EVT_KEY:
     if(wParam == (uint16) AVK_END)
     {
      if(pme->bEndKeyProcess == FALSE)
      {
        (void) ISHELL_CloseApplet(pme->a.m_pIShell, FALSE);
      }
      return TRUE;
     }
     return FALSE;

   case EVT_APP_STOP:
   {
      boolean *pBGFlag = (boolean *)dwParam;       // "Go background" flag
      if (pBGFlag)
      {
         // Set the BREW flag to go background
         *pBGFlag = FALSE;
      }

#ifdef FEATURE_JPEG_DECODER
      if(pme->m_pIDIB)
      {
        (void) IDIB_Release(pme->m_pIDIB);
        pme->m_pIDIB = NULL;
      }
      pme->m_bDisplayImage = FALSE;
#endif //FEATURE_JPEG_DECODER
      pme->bEndKeyProcess = FALSE;
      (void) ISHELL_CancelTimer(pme->a.m_pIShell, EndErrorMessageDlg, (void*) pme);
      return TRUE;
   }

   case EVT_USER:
   {
     switch(wParam)
     {
#ifdef FEATURE_MMGSDI
     case EVT_SIM_WRITTEN_FINAL:
       {
         MSG_MED("EVT_SIM_WRITTEN_FINAL", 0, 0, 0);
         pGSDIConf = (gsdi_cnf_T*) dwParam;
         if(pGSDIConf == NULL)
         {
           MSG_ERROR("PGSDIConf NULL", 0, 0, 0);
           return FALSE;
         }
         //First check that App is not suspended, otherwise nothing to do!
         if(pIDialog == NULL)
         {
           FREE(pGSDIConf);
           return TRUE;
         }
         wDlgID = IDIALOG_GetID(pIDialog);
         if(wDlgID == 0)
         {
           FREE(pGSDIConf);
           return TRUE;
         }
         //Clear the "wait" dialog
         if(IDD_WAIT_FPLMN_CLEAR_DIALOG == wDlgID)
         {
           ISHELL_EndDialog(pme->a.m_pIShell);
         }
         // Check the return status, if FALSE, display error message
         // else just clear the existing "Wait" dialog.

         if((pGSDIConf->write_cnf.message_header.resp_type == GSDI_SIM_WRITE_RSP) &&
           (pGSDIConf->write_cnf.message_header.gsdi_status == GSDI_SUCCESS) )
         {
           FREE(pGSDIConf);
           return TRUE;
         }
         else
         {
          //Wrong response..display error message
           MSG_ERROR("Failed in EVT_SIM_WRITTEN_FINAL", 0, 0, 0);
           FREE(pGSDIConf);
           pme->m_wStatusResID = IDS_ERROR;
           bRetVal = CheckAEEReturnStatus(ISHELL_CreateDialog(pme->a.m_pIShell, FLDDBG_RES_FILE,
             IDD_ERROR_DIALOG, NULL));
         }
       }
       return bRetVal;

     case EVT_SIM_WRITTEN:
       {
         MSG_MED("EVT_SIM_WRITTEN", 0, 0, 0);
         pGSDIConf = (gsdi_cnf_T*) dwParam;
         if(pGSDIConf == NULL)
         {
           MSG_ERROR("PGSDIConf NULL", 0, 0, 0);
           return FALSE;
         }
         //First check that App is not suspended, otherwise nothing to do!
         if(pIDialog == NULL)
         {
          FREE(pGSDIConf);
           return TRUE;
         }
         wDlgID = IDIALOG_GetID(pIDialog);
         if(wDlgID == 0)
         {
           FREE(pGSDIConf);
           return TRUE;
         }
         // Check the return status, if FALSE, display error message


         bGSDIError = !((pGSDIConf->write_cnf.message_header.resp_type == GSDI_SIM_WRITE_RSP) &&
           (pGSDIConf->write_cnf.message_header.gsdi_status == GSDI_SUCCESS));

         if( !bGSDIError)
         {
            gsdi_status = gsdi_get_file_attributes(
                               UIM_USIM_FPLMN,
                               &fplmn_attributes,
                               client_ref,
                               CFieldDebug_GetFileAttrCb);
            bGSDIError = !(gsdi_status == GSDI_SUCCESS);

         }
         if(bGSDIError)
         {
           MSG_ERROR("Failed in EVT_SIM_WRITTEN", 0, 0, 0);
           //Wrong response..display error message
           //Clear the "wait" dialog
           if(IDD_WAIT_FPLMN_CLEAR_DIALOG == wDlgID)
           {
             ISHELL_EndDialog(pme->a.m_pIShell);
           }
           FREE(pGSDIConf);
           pme->m_wStatusResID = IDS_ERROR;
           return CheckAEEReturnStatus(ISHELL_CreateDialog(pme->a.m_pIShell, FLDDBG_RES_FILE,
             IDD_ERROR_DIALOG, NULL));
         }
       }
       FREE(pGSDIConf);
       return TRUE;

     case EVT_GET_FILE_ATTR:
       {
         MSG_MED("EVT_GET_FILE_ATTR", 0, 0, 0);
         //First check that App is not suspended, otherwise nothing to do!
         pGSDIConf = (gsdi_cnf_T*) dwParam;
         if(pGSDIConf == NULL)
         {
           MSG_ERROR("PGSDIConf NULL", 0, 0, 0);
           return FALSE;
         }
         if(pIDialog == NULL)
         {
           FREE(pGSDIConf);
           return TRUE;
         }
         wDlgID = IDIALOG_GetID(pIDialog);
         if(wDlgID == 0)
         {
           FREE(pGSDIConf);
           return TRUE;
         }
         // Check the return status, if FALSE, display error message
         // else just clear the existing "Wait" dialog.
         bGSDIError = !((pGSDIConf->write_cnf.message_header.resp_type == GSDI_GET_FILE_ATTRIBUTES_RSP) &&
           (pGSDIConf->write_cnf.message_header.gsdi_status == GSDI_SUCCESS));
         if(!bGSDIError)
         {
          // Call the next command - this is final SIM_Write.
           switch(pme->m_card_mode)
           {
           case GSDI_CARD_USIM:
           case GSDI_CARD_GSM_USIM:
             gsdi_status = GSDI_SUCCESS;

             if (pGSDIConf->gfa_cnf.gfa_data.file_size > 0)
             {
               MEMSET(data, 0xFF, GSDI_MAX_DATA_BLOCK_LEN);
               gsdi_status = gsdi_sim_write(
                               UIM_USIM_FPLMN,
                               GSDI_INDEX_ABSOLUTE,
                               0,
                               NULL,
                               0,
                               data,
                               pGSDIConf->gfa_cnf.gfa_data.file_size,
                               0,
                               &dummy_rec_ptr,
                               client_ref,
                               CFieldDebug_SIMWrittenFinalCb
                               );
              }
             else
             {
                MSG_MED("File size = 0", 0, 0, 0);
                //End the "wait" dialog.
                if(IDD_WAIT_FPLMN_CLEAR_DIALOG == wDlgID)
                {
                  ISHELL_EndDialog(pme->a.m_pIShell);
                }
             }
             bGSDIError = !(gsdi_status == GSDI_SUCCESS);
             break;

           default:
             MSG_ERROR("Invalid card mode", 0, 0, 0);
             FREE(pGSDIConf);
             return FALSE;
           }
         }
         if(bGSDIError)
         {
           MSG_ERROR("Failed in EVT_GET_FILE_ATTR", 0, 0, 0);
           //Wrong response..display error message
           //Clear the "wait" dialog
           if(IDD_WAIT_FPLMN_CLEAR_DIALOG == wDlgID)
           {
             ISHELL_EndDialog(pme->a.m_pIShell);
           }
           FREE(pGSDIConf);
           pme->m_wStatusResID = IDS_ERROR;
           return CheckAEEReturnStatus(ISHELL_CreateDialog(pme->a.m_pIShell, FLDDBG_RES_FILE,
             IDD_ERROR_DIALOG, NULL));

         }
       }
       FREE(pGSDIConf);
       return TRUE;
#endif
     case EVT_RDM_STATUS:
       (void)CFieldDebug_SwitchMMCDownload(pme, dwParam);
       return TRUE;

     case EVT_MMC_DL_CLOSED:
       /* MMC Download switch is complete.  Route this event
        * to the current dialog event handler.
        */
       pDlgEvtHandler = CFieldDebug_GetDlgEvtHandler(pIDialog);
       if (pDlgEvtHandler == NULL)
       {
         MSG_ERROR("NULL event handler, %d", 0, 0, 0);
         return FALSE;
       }

       (void)pDlgEvtHandler(pme, eCode, wParam, dwParam);
       return TRUE;

     default:
        MSG_ERROR("Unhandled user event, %d.", wParam, 0, 0);
        break;
     }
     return FALSE;
   }

   case EVT_NOTIFY:
     {
       IDialog *pd = NULL;
       uint16 dialog_id;
       AEENotify *notif = (AEENotify *) dwParam;
       IShell *pIShell;

       /* Handle RATS notifiers */
       if (notif->cls == AEECLSID_RATS)
       {
         if (pme->a.m_pIShell)
           pd = ISHELL_GetActiveDialog(pme->a.m_pIShell);
         if (pd == NULL)
         {
           MSG_HIGH("Got Event Notify while we not running.", 0, 0, 0);
           return TRUE;
         }

         dialog_id = IDIALOG_GetID(pd);
         if (notif->dwMask & NMASK_DATA_AVAILABLE)
         {
           if (dialog_id != IDD_RATS_TEST_DIALOG)
           {
              (void) CFieldDebug_MoveToDialog(pme, IDD_RATS_TEST_DIALOG);
              return TRUE;
           }
           else
           {
             return CFieldDebug_RATS_TESTHandleEvent(pme,eCode,
                                                     wParam, dwParam);
           }
         }
         else if (notif->dwMask & NMASK_STATUS_CHANGE)
         {
           if (dialog_id == IDD_RATS_DIALOG)
             return CFieldDebug_RATSHandleEvent(pme,eCode, wParam, dwParam);
         }
       }
       /* Handle shell init notifier */
       else if ((notif->cls == AEECLSID_SHELL) &&
                (notif->dwMask & NMASK_SHELL_INIT))
       {
         pIShell = AEE_GetShell();
         if (pIShell == NULL)
         {
           MSG_ERROR("Failed to get shell pointer.", 0, 0, 0);
           return TRUE;
         }

         nRetVal = ISHELL_StartBackgroundApplet(pIShell,
                                                AEECLSID_FIELDDEBUGAPP, NULL);
         if (nRetVal != SUCCESS)
         {
           MSG_MED("Failed to launch FieldDebug app, %d.", nRetVal, 0, 0);
         }
         return TRUE;
       }
     }
     return FALSE;

   case EVT_DIALOG_INIT:
      (void) CFieldDebug_InitDialog(pme, (IDialog *)dwParam);
      return TRUE;

#ifdef FEATURE_JPEG_DECODER
   case EVT_JPEG_DECODE_DONE:
        {
          pme->m_bDisplayImage = TRUE;
          (void) ISHELL_EndDialog(pme->a.m_pIShell); //End JPEG Dialog

          IDISPLAY_ClearScreen(pme->a.m_pIDisplay);
          IDISPLAY_BitBlt(pme->a.m_pIDisplay, 0, 17,
                          176, 144 ,
                          (const void *) pme->m_pIDIB,
                          pme->x_offset, pme->y_offset, AEE_RO_COPY
                        );

          IDISPLAY_Update(pme->a.m_pIDisplay);
        }
       return TRUE;

   case EVT_KEY_PRESS:

        //If displaying image, return to file list dialog.
        if (pme->m_bDisplayImage)
        {
          if (wParam == (uint16) AVK_CLR)
          {
            IDISPLAY_ClearScreen(pme->a.m_pIDisplay);
            (void) ISHELL_CreateDialog(pme->a.m_pIShell, FLDDBG_RES_FILE, IDD_JPEG_DECODE, NULL);
          }
          else if (wParam == (uint16) AVK_LEFT)
            {
               if (pme->x_offset > 10)
                   pme->x_offset -= 10;

               CFieldDebug_DisplayImage(pme);
            }
          else if (wParam == (uint16) AVK_RIGHT)
            {
               if (pme->x_offset < pme->m_rc.dx && pme->m_rc.dx > 176)
                   pme->x_offset += 10;

               CFieldDebug_DisplayImage(pme);
            }
          else if (wParam == (uint16) AVK_UP)
            {
               if (pme->y_offset > 10)
                   pme->y_offset -= 10;

               CFieldDebug_DisplayImage(pme);
            }
          else if (wParam == (uint16) AVK_DOWN)
            {
               if (pme->y_offset < pme->m_rc.dy && pme->m_rc.dy > 144)
                  pme->y_offset += 10;

               CFieldDebug_DisplayImage(pme);
            }
        }
        return TRUE;
#endif /* FEATURE_JPEG_DECODER */

#ifdef FEATURE_APP_TEST_AUTOMATION
       case EVT_FLDDBG_DIAG_GETACTIVEAPPLET:
       {
         CFldDbg_GetActiveAppletRsp  *pGetActiveAppletRsp = NULL;

         /* This outer loop is just a convenient way to have the cleanup
          * for the diag in one place, at the end.
          */
         do
         {
           if (! pme->m_pIDiag)
           {
             break;
           }

           // Get the transaction ID for this request
           XactId = IDIAG_GetTransactionId(pme->m_pIDiag,
                                           AEECLSID_FIELDDEBUGAPP,
                                           eCode, wParam, dwParam);
           if (XactId == 0)
           {
             MSG_ERROR("Failed to get xact id for get active applet.", 0, 0, 0);
             DiagStatus = (uint32)IDIAG_DIAG_ERROR;
             break;
           }

           // Create and initialize the response
           pGetActiveAppletRsp = (CFldDbg_GetActiveAppletRsp *)
                         MALLOC(sizeof(CFldDbg_GetActiveAppletRsp));
           if (NULL == pGetActiveAppletRsp)
           {
             MSG_ERROR("Failed to allocate get active applet response.", 0, 0, 0);
             break;
           }

           // Default ActiveApplet to 0 (Not found)
           pGetActiveAppletRsp->ActiveApplet = 0;

           /* No need to allocate space for the request payload
            * as this diag has no request paylod
            */

           if(NULL != pme->a.m_pIShell)
             pGetActiveAppletRsp->ActiveApplet = ISHELL_ActiveApplet(pme->a.m_pIShell);

           DiagStatus = (uint32)IDIAG_SUCCESS;
           if (IDIAG_SendDiagResponse(pme->m_pIDiag, DiagStatus, XactId, TRUE,
                       sizeof(CFldDbg_GetActiveAppletRsp), pGetActiveAppletRsp) != AEE_SUCCESS)
           {
             MSG_ERROR("Failed to send response for get active applet", 0, 0, 0);
           }
         } while (0);

         // Clean up
         FREEIF(pGetActiveAppletRsp);
         return TRUE;
       }


     case EVT_FLDDBG_DIAG_APP_GET_NAME:
       {
       CFldDbg_GetAppNameReq   *pAppNameReq = NULL;
       CFldDbg_GetAppNameRsp   *pAppNameRsp = NULL;
       AECHAR                  *pAppName = NULL;
       uint32                  nAppId;
       AEEAppInfo              sAppInfo;

       /* This outer loop is just a convenient way to have the cleanup
        * for the diag in one place, at the end.
        */
       do
       {
         if (! pme->m_pIDiag)
         {
           break;
         }

         // Get the transaction ID for this request
         XactId = IDIAG_GetTransactionId(pme->m_pIDiag,
                                         AEECLSID_FIELDDEBUGAPP,
                                         eCode, wParam, dwParam);
         if (XactId == 0)
         {
           MSG_ERROR("Failed to get xact id for app get name.", 0, 0, 0);
           DiagStatus = (uint32)IDIAG_DIAG_ERROR;
           break;
         }

         // Create and initialize the response
         pAppNameRsp = (CFldDbg_GetAppNameRsp *)
                       MALLOC(sizeof(CFldDbg_GetAppNameRsp));
         if (NULL == pAppNameRsp)
         {
           MSG_ERROR("Failed to allocate app get name response.", 0, 0, 0);
           break;
         }

         // Default AppName to empty (MALLOC initializes to 0)
         pAppName = (AECHAR *)MALLOC(FLDDBG_DIAG_MAX_APPNAME_LEN * 
                                     sizeof(AECHAR));
         
         // Allocate space for the request payload
         pAppNameReq = (CFldDbg_GetAppNameReq *)
                        MALLOC(sizeof(CFldDbg_GetAppNameReq));
         if ( (NULL == pAppNameReq) || (NULL == pAppName) )
         {
           MSG_ERROR("Failed to allocate payload for app get name.", 0, 0, 0);
           break;
         }

         /* Now that we have everything in place for sending a response,
          * this inner do..while loop allows us to jump straight to sending
          * an "error" response if we encounter an error.
          */
         do
         {
           if (IDIAG_GetRequestPayload(pme->m_pIDiag, XactId,
                               sizeof(CFldDbg_GetAppNameReq), pAppNameReq) == 0)
           {
             MSG_ERROR("Failed to get payload for app get name.", 0, 0, 0);
             DiagStatus = (uint32)IDIAG_DIAG_ERROR;
             break;
           }

           nAppId = pAppNameReq->AppId;

           /* Search for app id in the list of avail. applets */
           ISHELL_EnumAppletInit(pme->a.m_pIShell);
           while(ISHELL_EnumNextApplet(pme->a.m_pIShell, &sAppInfo))
           {
             if(nAppId == sAppInfo.cls)
             {
               (void) ISHELL_LoadResString(pme->a.m_pIShell, sAppInfo.pszMIF,
                                    APPR_NAME(sAppInfo), pAppName,
                                    FLDDBG_DIAG_MAX_APPNAME_LEN);

               (void)WSTRTOSTR(pAppName, pAppNameRsp->AppName, FLDDBG_DIAG_MAX_APPNAME_LEN);
               break;
             }
             
           };

           /* Regardless of whether the search found the app or not,
            * the diag has now been successfully executed.
            */
           DiagStatus = (uint32)IDIAG_SUCCESS;
         } while (0);

         if (IDIAG_SendDiagResponse(pme->m_pIDiag, DiagStatus, XactId, TRUE,
                     sizeof(CFldDbg_GetAppNameRsp), pAppNameRsp) != AEE_SUCCESS)
         {
           MSG_ERROR("Failed to send response for app get name.", 0, 0, 0);
         }
       } while (0);

       // Clean up
       FREEIF(pAppNameReq);
       FREEIF(pAppNameRsp);
       FREEIF(pAppName);
       return TRUE;
     }

   case EVT_FLDDBG_DIAG_APP_GET_ID:
     {
       CFldDbg_GetAppIdReq   *pAppIdReq = NULL;
       CFldDbg_GetAppIdRsp   *pAppIdRsp = NULL;
       AECHAR                *pAppName = NULL;
       AECHAR                *pRefAppName = NULL;
       AEEAppInfo             sAppInfo;

       /* This outer loop is just a convenient way to have the cleanup
        * for the diag in one place, at the end.
        */
       do
       {
         if (! pme->m_pIDiag)
         {
           break;
         }

         // Get the transaction ID for this request
         XactId = IDIAG_GetTransactionId(pme->m_pIDiag,
                                         AEECLSID_FIELDDEBUGAPP,
                                         eCode, wParam, dwParam);
         if (XactId == 0)
         {
           MSG_ERROR("Failed to get xact id for app get id.", 0, 0, 0);
           DiagStatus = (uint32)IDIAG_DIAG_ERROR;
           break;
         }

         // Create and initialize the response
         pAppIdRsp = (CFldDbg_GetAppIdRsp *)
                       MALLOC(sizeof(CFldDbg_GetAppIdRsp));
         if (NULL == pAppIdRsp)
         {
           MSG_ERROR("Failed to allocate app get id response.", 0, 0, 0);
           break;
         }

         // Default AppId to 0 (Not found)
         pAppIdRsp->AppId = 0;

         // Allocate space for the request payload
         pAppIdReq = (CFldDbg_GetAppIdReq *)
                        MALLOC(sizeof(CFldDbg_GetAppIdReq));
         if (NULL == pAppIdReq)
         {
           MSG_ERROR("Failed to alloc payload for app get id.", 0, 0, 0);
           break;
         }

         /* Now that we have everything in place for sending a response,
          * this inner do..while loop allows us to jump straight to sending
          * an "error" response if we encounter an error.
          */
         do
         {
           if (IDIAG_GetRequestPayload(pme->m_pIDiag, XactId,
                               sizeof(CFldDbg_GetAppIdReq), pAppIdReq) == 0)
           {
             MSG_ERROR("Failed to get payload for app get id.", 0, 0, 0);
             DiagStatus = (uint32)IDIAG_DIAG_ERROR;
             break;
           }

           pAppName = (AECHAR *)MALLOC(FLDDBG_DIAG_MAX_APPNAME_LEN *
                                       sizeof(AECHAR));
           if (pAppName == NULL)
           {
             MSG_ERROR("Failed to allocate app name for app get id.",
                       0, 0, 0);
             break;
           }
           pRefAppName = (AECHAR *)MALLOC(FLDDBG_DIAG_MAX_APPNAME_LEN *
                                          sizeof(AECHAR));
           if (pRefAppName == NULL)
           {
             MSG_ERROR("Failed to allocate ref app name for app get id.",
                     0, 0, 0);
             break;
           }

           /* convert input string to wide string */
           (void) STRTOWSTR(pAppIdReq->AppName, pRefAppName,
                     FLDDBG_DIAG_MAX_APPNAME_LEN);

           /* Search for app name in the list of avail. applets */
           ISHELL_EnumAppletInit(pme->a.m_pIShell);
           while(ISHELL_EnumNextApplet(pme->a.m_pIShell, &sAppInfo))
           {
             (void) ISHELL_LoadResString(pme->a.m_pIShell, sAppInfo.pszMIF,
                                  APPR_NAME(sAppInfo), pAppName,
                                  FLDDBG_DIAG_MAX_APPNAME_LEN);
             if (WSTRCMP(pAppName, pRefAppName ) == 0)
             {
                 pAppIdRsp->AppId = sAppInfo.cls;
                 break;
             }
           };

           /* Regardless of whether the search found the app or not,
            * the diag has now been successfully executed.
            */
           DiagStatus = (uint32)IDIAG_SUCCESS;
         } while (0);

         if (IDIAG_SendDiagResponse(pme->m_pIDiag, DiagStatus, XactId, TRUE,
                     sizeof(CFldDbg_GetAppIdRsp), pAppIdRsp) != AEE_SUCCESS)
         {
           MSG_ERROR("Failed to send response for app get id.", 0, 0, 0);
         }
       } while (0);

       // Clean up
       FREEIF(pAppIdReq);
       FREEIF(pAppIdRsp);
       FREEIF(pAppName);
       FREEIF(pRefAppName);
       return TRUE;
     }

    case EVT_FLDDBG_DIAG_APP_START:
       {
         CFldDbg_StartAppReq   *pStartAppReq = NULL;
         CFldDbg_StartAppRsp   *pStartAppRsp = NULL;
         AEECLSID               ActiveApplet;
#if MIN_BREW_VERSION(3, 1)
         AppStartInfo           sAppStartInfo;
         int                    nAppSuccess;
         int                    nAppletCtlSize;
#endif

         /* This outer loop is just a convenient way to have the cleanup
          * for the diag in one place, at the end.
          */
         do
         { 
           if (! pme->m_pIDiag)
           {
             break;
           }

           // Get the transaction ID for this request
           XactId = IDIAG_GetTransactionId(pme->m_pIDiag,
                                           AEECLSID_FIELDDEBUGAPP,
                                           eCode, wParam, dwParam);
           if (XactId == 0)
           {
             MSG_ERROR("Failed to get xact id for start app.", 0, 0, 0);
             DiagStatus = (uint32)IDIAG_DIAG_ERROR;
             break;
           }
  
           // Create and initialize the response
           pStartAppRsp = (CFldDbg_StartAppRsp *)
                            MALLOC(sizeof(CFldDbg_StartAppRsp));
           if (NULL == pStartAppRsp)
           {
             MSG_ERROR("Failed to allocate start app response.", 0, 0, 0);
             break;
           }

           // Default to success
           pStartAppRsp->ShellCode = 0;

           // Allocate space for the request payload
           pStartAppReq = (CFldDbg_StartAppReq *)
                            MALLOC(sizeof(CFldDbg_StartAppReq));
           if (NULL == pStartAppReq)
           {
             MSG_ERROR("Failed to alloc payload for start app.", 0, 0, 0);
             break;
           }
  
           /* Now that we have everything in place for sending a response,
            * this inner do..while loop allows us to jump straight to sending
            * an "error" response if we encounter an error.
            */
           do
           {
             if (IDIAG_GetRequestPayload(pme->m_pIDiag, XactId,
                              sizeof(CFldDbg_StartAppReq), pStartAppReq) == 0)
             {
               MSG_ERROR("Failed to get payload for start app.", 0, 0, 0);
               DiagStatus = (uint32)IDIAG_DIAG_ERROR;
               break;
             }

             ActiveApplet = ISHELL_ActiveApplet(pme->a.m_pIShell);
             if (ActiveApplet == pStartAppReq->AppId)
             {
               // Don't start the applet if it's already the active applet
               MSG_ERROR("The specified applet %d is already the active applet.", ActiveApplet, 0, 0);
               break;
             }

#if MIN_BREW_VERSION(3, 1)
             // Implement the diag with IAppletCtl for BREW 3.1+

             if (! pme->m_pIAppletCtl)
             {
               MSG_ERROR("Strangely pme->IAppletCtl handle does not exist for executing the diag", 0, 0, 0);
               break;
             }

             sAppStartInfo.wFlags = SSA_NORMAL;
             sAppStartInfo.pszStart = (pStartAppReq->ArgLen)?pStartAppReq->Args : NULL;
             sAppStartInfo.dwReason = pStartAppReq->AppId;
             nAppletCtlSize = sizeof(AppStartInfo);
             nAppSuccess = IAPPLETCTL_CanStart(pme->m_pIAppletCtl,
                                               pStartAppReq->AppId,
                                               (void *)(&sAppStartInfo),
                                               &nAppletCtlSize);
             if (SUCCESS == nAppSuccess)
             {
               pStartAppRsp->ShellCode =
                              (uint32)IAPPLETCTL_Start(pme->m_pIAppletCtl,
                                                       pStartAppReq->AppId,
                                                       (void *)(&sAppStartInfo),
                                                       &nAppletCtlSize);
               if (SUCCESS == pStartAppRsp->ShellCode)
               {
                 DiagStatus = (uint32) IDIAG_SUCCESS;
               }
             }
             else
             {
               MSG_ERROR("Could not start the applet with IAppletCtl call %d", nAppSuccess, 0, 0);
             }
#else // MIN_BREW_VERSION(3, 1)
             // Implement the diag using the shell before BREW 3.1

             if (! ISHELL_CanStartApplet(pme->a.m_pIShell,
                                         pStartAppReq->AppId))
             {
               // If it's not safe to start, don't
               MSG_ERROR("Could not start the applet using shell for versions below Brew 3.1", 0, 0, 0);
               break;
             }
             if (pStartAppReq->ArgLen != 0)
             {
                 pStartAppRsp->ShellCode =
                          (uint32)ISHELL_StartAppletArgs(pme->a.m_pIShell,
                                                         pStartAppReq->AppId,
                                                         pStartAppReq->Args);
             }
             else
             {
                 pStartAppRsp->ShellCode =
                          (uint32)ISHELL_StartAppletArgs(pme->a.m_pIShell,
                                                         pStartAppReq->AppId,
                                                         NULL);
             }
             if (SUCCESS == pStartAppRsp->ShellCode) {
                 DiagStatus = IDIAG_SUCCESS;
             }
#endif // MIN_BREW_VERSION(3, 1)

           } while (0);

           if (IDIAG_SendDiagResponse(pme->m_pIDiag, DiagStatus, XactId, TRUE,
                     sizeof(CFldDbg_StartAppRsp), pStartAppRsp) != AEE_SUCCESS)
           {
             MSG_ERROR("Failed to send response for start app.", 0, 0, 0);
           }
         } while (0);

         // Clean up
         FREEIF(pStartAppReq);
         FREEIF(pStartAppRsp);
         return TRUE;
       }

     case EVT_FLDDBG_DIAG_APP_STOP:
       {
         CFldDbg_StopAppReq    *pStopAppReq = NULL;
         CFldDbg_StopAppRsp    *pStopAppRsp = NULL;
         boolean                bDoSendErrorRsp = TRUE;
#if ! MIN_BREW_VERSION(3, 1)
         PACONTEXT              pStopAppCtxt;
         boolean                bCallbackStatus = FALSE;
#endif

         /* This outer loop is just a convenient way to have the cleanup
          * for the diag in one place, at the end.
          */
         do
         {
           if (! pme->m_pIDiag)
           {
             break;
           }

           // Get the transaction ID for this request
           XactId = IDIAG_GetTransactionId(pme->m_pIDiag,
                                           AEECLSID_FIELDDEBUGAPP,
                                           eCode, wParam, dwParam);
           if (XactId == 0)
           {
             MSG_ERROR("Failed to get xact id for stop app.", 0, 0, 0);
             DiagStatus = (uint32)IDIAG_DIAG_ERROR;
             break;
           }
  
           // Create and initialize the response
           pStopAppRsp = (CFldDbg_StopAppRsp *)
                                         MALLOC(sizeof(CFldDbg_StopAppRsp));
           if (NULL == pStopAppRsp)
           {
             MSG_ERROR("Failed to allocate stop app response.", 0, 0, 0);
             break;
           }

           // Default to success
           pStopAppRsp->ShellCode = 0;

           // Allocate space for the request payload
           pStopAppReq = (CFldDbg_StopAppReq *)
                            MALLOC(sizeof(CFldDbg_StopAppReq));
           if (NULL == pStopAppReq)
           {
             MSG_ERROR("Failed to alloc payload for stop app.", 0, 0, 0);
             break;
           }
  
           /* Now that we have everything in place for sending a response,
            * this inner do..while loop allows us to jump straight to sending
            * an "error" response if we encounter an error.
            */
           do
           {
             if (IDIAG_GetRequestPayload(pme->m_pIDiag, XactId,
                              sizeof(CFldDbg_StopAppReq), pStopAppReq) == 0)
             {
               MSG_ERROR("Failed to get payload for stop app.", 0, 0, 0);
               DiagStatus = (uint32)IDIAG_DIAG_ERROR;
               break;
             }

#if MIN_BREW_VERSION(3, 1)

             if (! pme->m_pIAppletCtl)
             {
               DiagStatus = (uint32)IDIAG_DIAG_ERROR;
               break;
             }

             pStopAppRsp->ShellCode =
                            (uint32)IAPPLETCTL_Stop(pme->m_pIAppletCtl,
                                                     pStopAppReq->AppId);
             if (SUCCESS == pStopAppRsp->ShellCode)
             {
               DiagStatus = (uint32) IDIAG_SUCCESS;
             }

#else // MIN_BREW_VERSION(3, 1)

             pme->m_sCloseCBData.pIShell = pme->a.m_pIShell;
             pme->m_sCloseCBData.XactId = XactId;

             bCallbackStatus = CFieldDebug_SetCallback(pme, CFieldDebug_CloseAppletCB,
                        (void *)&(pme->m_sCloseCBData),
                        pStopAppReq->AppId
                        );
             if (bCallbackStatus == FALSE)
             {
               MSG_ERROR("Failed to set the call back for app_stop event", 0, 0, 0);
               DiagStatus = (unit32)IDIAG_OTHER_INT_ERROR;
               break;
             }
             bDoSendErrorRsp = FALSE; 
             /* If for some reason callback is never called, a response needs 
             * to be sent to the DM. Set a timer which will send a 
             * error response message. The timer needs to be cancelled if 
             * the callback funciton does gets called.
             */
             pme->m_SCallbackTimerData.pIDiag = pme->m_pIDiag;
             pme->m_SCallbackTimerData.XactId = XactId;
             pme->m_sCallbackTimerData.nDiagEvtId = EVT_FLDDBG_DIAG_APP_STOP;
             
             if(ISHELL_SetTimer(pme->a.m_pIShell, FLDDBG_DIAG_TIMERVALUE, 
                      (PFNNOTIFY)CFieldDebug_CallbackTimer, 
                         (void *)&(pme->m_sCallbackTimerData)) != SUCCESS);
             {
               MSG_ERROR("Timer could not be set up for checking callback", 0, 0, 0);
               break;    
             }
#endif // MIN_BREW_VERSION(3, 1)

           } while (0);

           if (bDoSendErrorRsp)
           {
              if (IDIAG_SendDiagResponse(pme->m_pIDiag, DiagStatus, XactId,
                                         TRUE, sizeof(CFldDbg_StopAppRsp),
                                         pStopAppRsp) != AEE_SUCCESS)
              {
                MSG_ERROR("Failed to send response for stop app.", 0, 0, 0);
              }
           }
         } while (0);

         // Clean up
         FREEIF(pStopAppReq);
         FREEIF(pStopAppRsp);
         return TRUE;
       }

#if ! MIN_BREW_VERSION(3, 1)
   // This event is only used to implement the AppStop diag before BREW 3.1
   case EVT_FLDDBG_DIAG_APP_CLOSE_COMPLETE:
      {
         // wParam has the transaction id, dwParam has the shell code
      
         CFldDbg_StopAppRsp    *pStopAppRsp = NULL;

         /* Since the callback function is called, cancel the callback
         *  timer function.
         */
         ISHELL_CancelTimer(pme->a.m_pIShell, (PFNNOTIFY)CFieldDebug_CallbackTimer, (void *)&(pme->m_sCallbackTimerData));

         pStopAppRsp = (CFldDbg_StopAppRsp *)
                                         MALLOC(sizeof(CFldDbg_StopAppRsp));
         if (! pStopAppRsp)
         {
            MSG_ERROR("Could not allocate stop app rsp for complete event.",
                      0, 0, 0);
         }
         else
         {

            DiagStatus = IDIAG_SUCCESS;
            pStopAppRsp->ShellCode = (uint32)dwParam;
            if (IDIAG_SendDiagResponse(pme->m_pIDiag, DiagStatus, wParam,
                                       TRUE, sizeof(CFldDbg_StopAppRsp),
                                       pStopAppRsp) != AEE_SUCCESS)
            {
               MSG_ERROR("Failed sending rsp for stop app for complete event.",
                      0, 0, 0);
            }

            FREE(pStopAppRsp);
         }

         return TRUE;

      }
#endif // ! MIN_BREW_VERSION(3, 1)
      
    case EVT_FLDDBG_DIAG_GETACTIVEDLG_COMPLETE:
      {
         // wParam has the transaction id, dwParam has the dialog id
      
         CFldDbg_GetActiveDlgRspType    *pGetActiveDlgRsp = NULL;
          
         /* Since the callback function is called, cancel the callback
         *  timer function.
         */
         (void) ISHELL_CancelTimer(pme->a.m_pIShell, (PFNNOTIFY)CFieldDebug_CallbackTimer, (void *)&(pme->m_sCallbackTimerData));
   
         pGetActiveDlgRsp = (CFldDbg_GetActiveDlgRspType *)
                                         MALLOC(sizeof(CFldDbg_GetActiveDlgRspType));
         if (! pGetActiveDlgRsp)
         {
            MSG_ERROR("Could not allocate get active dlg rsp for getactivedlg_complete event.", 0, 0, 0);
         }
         else
         {
            DiagStatus = (uint32) IDIAG_SUCCESS;
            pGetActiveDlgRsp->DlgId = dwParam;
            if (IDIAG_SendDiagResponse(pme->m_pIDiag, DiagStatus, wParam,
                                       TRUE, sizeof(CFldDbg_GetActiveDlgRspType),
                                       (void *)pGetActiveDlgRsp) != AEE_SUCCESS)
            {
               MSG_ERROR("Failed sending rsp for getactivedlg_complete event.",
                      0, 0, 0);
            }

            FREE((void *) pGetActiveDlgRsp);
         }

         return TRUE;

      }  
   
    case EVT_FLDDBG_DIAG_GETSELMENUCTLTXT_COMPLETE:
      {
        // dwParam has the address to text
        uint32 nStrLen;
        CFldDbg_GetMenuCtlTxtRspType    *pGetMenuCtlTxtRsp = NULL;

        /* Since the callback function is called, cancel the callback
        *  timer function.
        */
        (void) ISHELL_CancelTimer(pme->a.m_pIShell, 
                          (PFNNOTIFY)CFieldDebug_CallbackTimer,
                          (void *)&(pme->m_sCallbackTimerData));

        if (NULL == dwParam)
        {
          MSG_ERROR("The text pointer is NULL in GETSELMENUCTLTXT_COMPLETE event",
                    0, 0, 0);
        }

        nStrLen = STRLEN((char *)dwParam);
        /* No need to allocate extra space for \0 as the CtlText in Response
         * structure is already an array of size 1
         */
        pGetMenuCtlTxtRsp = (CFldDbg_GetMenuCtlTxtRspType *)
                           MALLOC(sizeof(CFldDbg_GetMenuCtlTxtRspType) + nStrLen);
        
        if (! pGetMenuCtlTxtRsp)
        {
          MSG_ERROR("Could not allocate rsp payload for getmenuctltxt complete event.",
                    0, 0, 0);
        } else
        {
          DiagStatus = (uint32) IDIAG_SUCCESS;
          (void) STRLCPY(pGetMenuCtlTxtRsp->CtlText, (char *)dwParam , sizeof(pGetMenuCtlTxtRsp->CtlText));
          if (IDIAG_SendDiagResponse(pme->m_pIDiag, DiagStatus, wParam,
                                     TRUE, sizeof(CFldDbg_GetMenuCtlTxtRspType) +
                                     nStrLen, (void *)pGetMenuCtlTxtRsp) != AEE_SUCCESS)
          {
            MSG_ERROR("Failed sending rsp for getmenuctltext_complete event.",
                      0, 0, 0);
          }

          FREE((void *) pGetMenuCtlTxtRsp);
        }

        return TRUE;

      } 

   case EVT_FLDDBG_DIAG_APP_SEND_EVENT:
      {
        CFldDbg_CmdSendEventReqType   *pSendEventReq = NULL;
        CFldDbg_CmdSendEventRspType   *pSendEventRsp = NULL;
    static uint16 nTransactionId = 0; 
        /* nTransactionID is used for two purposes
        1) data is saved in IClipboard with this as Clipboard entry name
        2) Used as delayed rsp id. This is part of first rsp. Later a delayed
          rsp(Rsp or event) will have the same id. So tools can relate delayed
          rsp with intial request/response combination 
    3) This value is only for legacy purposes and not same as delay response ID.
        */
        uint32  nRspLen;
        IClipboard *pIClBd = NULL;
        char    szClipboardEntryName[CLIPBOARD_NAME_MAX_LENGTH];
    /* temporary return codes */
        int     nRc;
    boolean bRc;
        
         /* This outer loop is just a convenient way to have the cleanup
         * for the diag in one place, at the end.
         */
         do
         {
           if (! pme->m_pIDiag)
           {
             break;
           }

           // Get the transaction ID for this request
           XactId = IDIAG_GetTransactionId(pme->m_pIDiag,
                                           AEECLSID_FIELDDEBUGAPP,
                                           eCode, wParam, dwParam);
           if (XactId == 0)
           {
             MSG_ERROR("Failed to get xact id for app send event.", 0, 0, 0);
             break;
           }
  
           // Create and initialize the response
           pSendEventRsp = (CFldDbg_CmdSendEventRspType *)
                         MALLOC(sizeof(CFldDbg_CmdSendEventRspType));
           if (NULL == pSendEventRsp)
           {
             MSG_ERROR("Failed to allocate send event response.", 0, 0, 0);
             break;
           }

           /* Now that we have everything in place for sending a response,
            * this inner do..while loop allows us to jump straight to sending
            * an "error" response if we encounter an error.
            */
           do
           {
         // Allocate space for the request payload
             pSendEventReq = (CFldDbg_CmdSendEventReqType *)
                          MALLOC(sizeof(CFldDbg_CmdSendEventReqType));
             if (NULL == pSendEventReq)
             {
               MSG_ERROR("Failed to alloc payload for send event.", 0, 0, 0);
               break;
             }
 
             if (IDIAG_GetRequestPayload(pme->m_pIDiag, XactId,
                                 sizeof(CFldDbg_CmdSendEventReqType), (void *)pSendEventReq) == 0)
             {
               MSG_ERROR("Failed to get payload for send event.", 0, 0, 0);
               DiagStatus = (uint32)IDIAG_DIAG_ERROR;
               break;
             }

         /* initialize default response */
             pSendEventRsp->Status = (uint32) CFLDDBG_CMD_STATUS_ERROR;
             /* initilaize the delayed rsp id*/
             pSendEventRsp->Reserved = 0;
             /* copy the req to rsp*/
             pSendEventRsp->AppId = pSendEventReq->AppId;
             pSendEventRsp->EventId = pSendEventReq->EventId;
             pSendEventRsp->DataLen = pSendEventReq->DataLen;

             //copy request data to response 
             (void) MEMCPY((void *)pSendEventRsp->Data, (const void *)
                 pSendEventReq->Data, pSendEventReq->DataLen);
             
             // Create the ICLlPBOARD instance. This will be used to store data to pass 
             // to BrewApp 
             if(ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_CLIPBOARD, 
                 (void**)(&pIClBd)) != SUCCESS)
             {
               MSG_ERROR("Failed to create Clipboard instance.", 0, 0, 0);
               DiagStatus = (uint32)IDIAG_DIAG_ERROR;
               break;
             }
             
     
             // Increment nTransactionId; watch for rollover
             if (nTransactionId == FLDDBG_DIAG_MAX_TRANSACTION_ID)
               nTransactionId = 0;
 
             nTransactionId++;
    
             // create the clipboard name to store the data under
             nRc = SNPRINTF(szClipboardEntryName , sizeof(szClipboardEntryName), "AUTOMATION_%d", nTransactionId);
             if(nRc <= 0)
             {
               pSendEventRsp->Status = (uint32) CFLDDBG_CMD_STATUS_ERROR;
               MSG_ERROR("Failed to set the clip board entry name", 0, 0, 0);
                break;
             }
             //lint -restore
 
             // set the data in clipboard
             nRc = ICLIPBOARD_SetData(pIClBd, szClipboardEntryName,
                             (void *)pSendEventReq->Data, pSendEventReq->DataLen);
     
             if (nRc != SUCCESS) {
               pSendEventRsp->Status = (uint32)nRc;
               MSG_ERROR("Failed to set data in the clipboard.", 0, 0, 0);
               break;
             }

             // Post an event to BrewApp 
             bRc = ISHELL_PostEventEx(pme->a.m_pIShell, EVTFLG_ASYNC, pSendEventReq->AppId,  (uint16)(pSendEventReq->EventId),
                          nTransactionId, (uint32)(pSendEventReq->DataLen));
   
             if (bRc == TRUE) {
               pSendEventRsp->Status = (uint32) CFLDDBG_CMD_STATUS_OK;
             }else{
               MSG_ERROR("Failed to post event to BrewApp.", 0, 0, 0);
               DiagStatus = (uint32)IDIAG_DIAG_ERROR;
               break;
             }

             /* Regardless of whether the the event is sent or not,
              * the diag has now been successfully executed.
              */
             DiagStatus = (uint32)IDIAG_SUCCESS;
           } while (0);

       /* Send the response. Calculate the actual size of response which 
        * usually is not same as the structure type as the file name in
        * Data field might be less than FLDDBG_MAX_FILE_NAME_LEN. Note 
        * that the each character is stored as 1 byte in the array.
        */
           nRspLen = sizeof(CFldDbg_CmdSendEventRspType) -  
           FLDDBG_DIAG_MAX_FILENAME_LEN + 
           pSendEventRsp->DataLen;
           if (IDIAG_SendDiagResponse(pme->m_pIDiag, DiagStatus, XactId, TRUE,
                       nRspLen, (void *)pSendEventRsp) != AEE_SUCCESS)
           {
             MSG_ERROR("Failed to send response for send event.", 0, 0, 0);
           }
         } while (0);

     break;
      }
      
      case EVT_FLDDBG_DIAG_GETACTIVEDLG:
         {
           /* DESCRIPTION   A handler for the "Get Active Dialog" request.  It
              obtains the dialog ID of the active dialog for the
              application in the request.  This routine sends a
              diagnostic response to the DM.
           */
           CFldDbg_GetActiveDlgRspType *pGetActiveDlgRsp = NULL;
           CFldDbg_GetActiveDlgReqType *pGetActiveDlgReq = NULL;
           uint32                       nReqAppId;
           AEECLSID                     clsId;
           boolean                      bCallbackStatus = FALSE;
           boolean                      bDoSendErrorRsp = TRUE;
      /* The response to the DM is sent in all cases except when
       * a call back has to be made. If a call back is necessary, 
       * then the response is sent from the call back handler.
       */
           
           /* This outer loop is just a convenient way to have the cleanup
           * for the diag in one place, at the end.
           */
           do
           {
             if (! pme->m_pIDiag)
             {
               break;
             }

             // Get the transaction ID for this request
             XactId = IDIAG_GetTransactionId(pme->m_pIDiag,
                                           AEECLSID_FIELDDEBUGAPP,
                                           eCode, wParam, dwParam);
             if (XactId == 0)
             {
               MSG_ERROR("Failed to get xact id for get active dlg event.", 0, 0, 0);
               break;
             }
         
             pGetActiveDlgRsp = (CFldDbg_GetActiveDlgRspType *)
             MALLOC(sizeof(CFldDbg_GetActiveDlgRspType));
             if (NULL == pGetActiveDlgRsp)
             {
               MSG_ERROR("Failed to allocate get active dlg response",0, 0, 0);
                break;
             }
        
             /* Now that we have everything in place for sending a response,
             * this inner do..while loop allows us to jump straight to sending
             * an "error" response if we encounter an error.
             */
             do
             {
               // Allocate space for the request payload
               pGetActiveDlgReq = (CFldDbg_GetActiveDlgReqType *)
                          MALLOC(sizeof(CFldDbg_GetActiveDlgReqType));
               if (NULL == pGetActiveDlgReq)
               {
                 MSG_ERROR("Failed to alloc payload for get active dlg.", 0, 0, 0);
                 DiagStatus = (uint32)IDIAG_DIAG_ERROR;
                 break;
               }
 
               if (IDIAG_GetRequestPayload(pme->m_pIDiag, XactId,
                                 sizeof(CFldDbg_GetActiveDlgReqType), (void *)pGetActiveDlgReq) == 0)
               {
                 MSG_ERROR("Failed to get payload for get active dlg.", 0, 0, 0);
                 DiagStatus = (uint32)IDIAG_DIAG_ERROR;
                 break;
               }
            
               /* Get the app ID from the request */
               nReqAppId = pGetActiveDlgReq->AppId;
               /* Get the active applet ID */
               clsId = ISHELL_ActiveApplet(pme->a.m_pIShell);
          
               if(clsId != nReqAppId)
               {
                  MSG_ERROR("The request app does not seem to be the active app",0, 0, 0);
                  pGetActiveDlgRsp->DlgId = 0;
                  /* The diag executed successfully */
                  DiagStatus = (uint32)IDIAG_SUCCESS;
                  break;
               }
          
               /* The running applet is currently FieldDbg app but the 
                * active dialog on the phone might be different. 
                * ISHELL_GetActiveDialog() cannot be called from here
                * as it returns invalid result. A callback needs to be set
                * so that the callback function is called in the context 
                * of the active applet.
                */
          
               pme->m_sGetActiveDlgCBData.pIShell = pme->a.m_pIShell;
               pme->m_sGetActiveDlgCBData.XactId = XactId;

               bCallbackStatus = CFieldDebug_SetCallback(pme,
                                CFieldDebug_GetActiveDlgCB, 
                                (void *)&(pme->m_sGetActiveDlgCBData), 
                                nReqAppId); 
               if (bCallbackStatus == FALSE)
               {
                 MSG_ERROR("Failed to set the call back for get active dlg", 0, 0, 0);
                 pGetActiveDlgRsp->DlgId = 0;
                 DiagStatus = (uint32)IDIAG_OTHER_INT_ERROR;
                 break;
               }
               /* Callback was set properly and there's no need to send a response now
                * Response will be sent after the call back function finishes.
                */
               bDoSendErrorRsp = FALSE;
         
               /* If for some reason callback is not called, a response needs 
               * to be sent to the DM. Set a timer which will send a 
               * error response message. The timer needs to be cancelled if 
               * the callback funciton does gets called.
               */
               pme->m_sCallbackTimerData.pIDiag = pme->m_pIDiag;
               pme->m_sCallbackTimerData.XactId = XactId;
               pme->m_sCallbackTimerData.nDiagEvtId = EVT_FLDDBG_DIAG_GETACTIVEDLG;
               
               if(ISHELL_SetTimer(pme->a.m_pIShell, FLDDBG_DIAG_TIMERVALUE, 
                        (PFNNOTIFY)CFieldDebug_CallbackTimer, 
                           (void *)&(pme->m_sCallbackTimerData)) != SUCCESS)
               {
                 MSG_ERROR("Timer could not be set up for checking callback", 0, 0, 0);
                 break;    
               } 
             } while(0);

             if( bDoSendErrorRsp)
             {
               if (IDIAG_SendDiagResponse(pme->m_pIDiag, DiagStatus, XactId, TRUE,
                       sizeof(CFldDbg_GetActiveDlgRspType), (void *)pGetActiveDlgRsp) != AEE_SUCCESS)
               {
                 MSG_ERROR("Failed to send response for send event.", 0, 0, 0);
                 break;
               }
             }

             FREEIF(pGetActiveDlgReq);
             FREEIF(pGetActiveDlgRsp);

           }while(0);
     return TRUE;
    }
    
    case EVT_FLDDBG_DIAG_GETSELMENUCTLTXT:
    {
      CFldDbg_GetMenuCtlTxtRspType *pGetMenuCtlTxtRsp = NULL;
      CFldDbg_GetMenuCtlTxtReqType *pGetMenuCtlTxtReq = NULL;
      uint32                       nReqAppId;
      boolean                      bCallbackStatus = FALSE;
      boolean                      bDoSendErrorRsp = TRUE;
      /* The response to the DM is sent in all cases except when
       * a call back has to be made. If a call back is necessary, 
       * then the response is sent from the call back handler.
       */

      /* This outer loop is just a convenient way to have the cleanup
        * for the diag in one place, at the end.
        */
      do
      {
        if (! pme->m_pIDiag)
        {
          break;
        }
        // Get the transaction ID for this request
        XactId = IDIAG_GetTransactionId(pme->m_pIDiag,
                                          AEECLSID_FIELDDEBUGAPP,
                                          eCode, wParam, dwParam);
        if (XactId == 0)
        {
          MSG_ERROR("Failed to get xact id for get menu ctl text event.", 0, 0, 0);
          break;
        }

        pGetMenuCtlTxtRsp = (CFldDbg_GetMenuCtlTxtRspType *)
                             MALLOC(sizeof(CFldDbg_GetMenuCtlTxtRspType)
                             + MAX_RES_SIZE);
        
        if (NULL == pGetMenuCtlTxtRsp)
        {
          MSG_ERROR("Failed to allocate get menu ctl text response",0, 0, 0);
          break;
        }

        /* Now that we have everything in place for sending a response,
          * this inner do..while loop allows us to jump straight to sending
          * an "error" response if we encounter an error.
          */
        do
        {
          // Allocate space for the request payload
          pGetMenuCtlTxtReq = (CFldDbg_GetMenuCtlTxtReqType *)
                             MALLOC(sizeof(CFldDbg_GetMenuCtlTxtReqType));
          if (NULL == pGetMenuCtlTxtReq)
          {
            MSG_ERROR("Failed to alloc payload for get menu ctl text req.", 0, 0, 0);
            DiagStatus = (uint32)IDIAG_DIAG_ERROR;
            break;
          }

          if (IDIAG_GetRequestPayload(pme->m_pIDiag, XactId,
                                        sizeof(CFldDbg_GetMenuCtlTxtReqType),
                                        (void *)pGetMenuCtlTxtReq) == 0)
          {
            MSG_ERROR("Failed to get payload for get menu ctl text req.", 0, 0, 0);
            DiagStatus = (uint32)IDIAG_DIAG_ERROR;
            break;
          } 

          /* Get the app ID from the request */
          nReqAppId = pGetMenuCtlTxtReq->AppId;

          /* A callback needs to be set so that the callback function is called in the context 
           * of the active applet.
           */
          pme->m_sGetMenuCtlTxtCBData.pIShell = pme->a.m_pIShell;
          pme->m_sGetMenuCtlTxtCBData.XactId = XactId;
          pme->m_sGetMenuCtlTxtCBData.DlgId = pGetMenuCtlTxtReq->DlgId;
          pme->m_sGetMenuCtlTxtCBData.CtlId = pGetMenuCtlTxtReq->CtlId ;
          //Initialize the text to empty string (no item found)
          (void) STRLCPY(pme->m_sGetMenuCtlTxtCBData.CtlText, "", sizeof(pme->m_sGetMenuCtlTxtCBData.CtlText)); 
             
          bCallbackStatus = CFieldDebug_SetCallback(pme,
                                                    CFieldDebug_GetMenuCtlTxtCB,
                                                    (void *)&(pme->m_sGetMenuCtlTxtCBData), 
                                                     nReqAppId); 
          if (bCallbackStatus == FALSE)
          {
            MSG_ERROR("Failed to set the call back for get menu ctl text", 0, 0, 0);
            DiagStatus = (uint32)IDIAG_OTHER_INT_ERROR;
            break;
          }
          /* Callback was set properly and there's no need to send a response now
           * Response will be sent after the call back function finishes.
           */
          bDoSendErrorRsp = FALSE;

          /* If for some reason callback is not called, a response needs 
           * to be sent to the DM. Set a timer which will send a 
           * error response message. The timer needs to be cancelled if 
           * the callback function does gets called.
           */
          pme->m_sCallbackTimerData.pIDiag = pme->m_pIDiag;
          pme->m_sCallbackTimerData.XactId = XactId;
          pme->m_sCallbackTimerData.nDiagEvtId = EVT_FLDDBG_DIAG_GETSELMENUCTLTXT;

          if (ISHELL_SetTimer(pme->a.m_pIShell, FLDDBG_DIAG_TIMERVALUE, 
                              (PFNNOTIFY)CFieldDebug_CallbackTimer, 
                              (void *)&(pme->m_sCallbackTimerData)) != SUCCESS)
          {
            MSG_ERROR("Timer could not be set up for checking callback", 0, 0, 0);
            break;    
          }
            
        } while (0);

        if ( bDoSendErrorRsp)
        {
          if (IDIAG_SendDiagResponse(pme->m_pIDiag, DiagStatus, XactId, 
                                      TRUE,
                                      sizeof(CFldDbg_GetMenuCtlTxtRspType),
                                      (void *)pGetMenuCtlTxtRsp) != AEE_SUCCESS)
          {
            MSG_ERROR("Failed to send response for send event.", 0, 0, 0);
            break;
          }
        }

        FREEIF(pGetMenuCtlTxtReq);
        FREEIF(pGetMenuCtlTxtRsp);

      }while (0);
      return TRUE;
    }

#endif // FEATURE_APP_TEST_AUTOMATION

   default:
      break;
   }
   return FALSE;
}

/*===========================================================================
FUNCTION CFieldDebug_DrawErrorScreen

DESCRIPTION


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
/===========================================================================*/
static void CFieldDebug_DrawErrorScreen(CFieldDebug * pme)
{
   AECHAR szBuf[400];
   int i, j, n = 0;
   OEMErrLogType err_log_data;
   IDialog *p_dlg;
   IStatic *p_stk;
   AECHAR fmt_str[20];


   if (!pme)
   {
     MSG_ERROR("Null ptr", 0, 0, 0);
     return;
   }

   p_dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
   p_stk = (IStatic *) IDIALOG_GetControl(p_dlg, IDC_ERROR_STATIC);
   (void) STRTOWSTR("%d %d\n", fmt_str, sizeof(fmt_str));

   // Set the dimensions of the rectangle for the text control
   (void)MEMSET(szBuf, (AECHAR) 0, sizeof(szBuf));

   for (j = 0; j < 20; j++) {
      err_log_data.err_num = (byte) j;
      if (SUCCESS != ICONFIG_GetItem(pme->m_pIConfig,
                                     CFGI_DEBUG_ERRLOG,
                                     &err_log_data,
                                     sizeof(err_log_data))) {
         break;
      }

      if (err_log_data.err_count) {
         for (i = 0; i < 8 && err_log_data.file_name[i]; i++, n++) {
            szBuf[n] = (AECHAR) err_log_data.file_name[i];
         }

         // show '*' if this is fatal error
         szBuf[n++] = err_log_data.fatal ? (AECHAR) '*' : (AECHAR) ' ';

         WSPRINTF((szBuf + n),
                  sizeof(szBuf),
                  fmt_str,
                  err_log_data.line_num,
                  err_log_data.err_count);

         n = (int) WSTRLEN(szBuf);
      }  // end if
   }  // end for

   ISTATIC_SetProperties(p_stk, ST_NOSCROLL);
   // Set the values of the title and text strings for this control
   (void) ISTATIC_SetText(p_stk,
                          NULL,
                          szBuf,
                          AEE_FONT_NORMAL,
                          AEE_FONT_NORMAL);

   (void) ISTATIC_Redraw(p_stk);
}





/*===========================================================================
FUNCTION CFieldDebug_DrawVersionScreen

DESCRIPTION


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
/===========================================================================*/
static void CFieldDebug_DrawVersionScreen(CFieldDebug * pme)
{
   AECHAR szBuf[200];
   IDialog *p_dlg;
   IStatic *p_stk;
   int n = 0;
   uint32 esn;
   dword date = 0;
   AECHAR fmt_str[16];
   int i, j, count;

   if (!pme)
   {
     MSG_ERROR("Null ptr", 0, 0, 0);
     return;
   }

   (void)MEMSET( szBuf,(AECHAR) 0, sizeof(szBuf));

   (void) ISHELL_LoadResString(pme->a.m_pIShell,
                               FLDDBG_RES_FILE,
                               IDS_SW_VERSION,
                               szBuf,
                               sizeof(szBuf));

   n = WSTRLEN(szBuf);

   (void) ICONFIG_GetItem(pme->m_pIConfig,
                          CFGI_BUILD_VERSION,
                          (szBuf + n),
                          sizeof(szBuf));

   n = WSTRLEN(szBuf);
   szBuf[n++] = (AECHAR) '\n';

   (void) ISHELL_LoadResString(pme->a.m_pIShell,
                               FLDDBG_RES_FILE,
                               IDS_ESN,
                               (szBuf + n),
                               sizeof(szBuf));

   n = WSTRLEN(szBuf);

   (void) ICONFIG_GetItem(pme->m_pIConfig,
                          CFGI_ESN,
                          &esn,
                          sizeof(esn));

   (void) STRTOWSTR("%u %u", fmt_str, sizeof(fmt_str));

   WSPRINTF((szBuf + n),
            sizeof(szBuf),
            fmt_str,
            ((esn & 0xFF000000) >> 24),
            (esn & 0x00FFFFFF));

   n = WSTRLEN(szBuf);
   szBuf[n++] = (AECHAR) '\n';

   (void) ISHELL_LoadResString(pme->a.m_pIShell,
                               FLDDBG_RES_FILE,
                               IDS_RFCAL_VER,
                               (szBuf + n),
                               sizeof(szBuf));

   n = WSTRLEN(szBuf);

   (void) STRTOWSTR("\n              ", fmt_str, sizeof(fmt_str));
   (void) WSTRLCPY((szBuf + n), fmt_str, sizeof((szBuf + n)));

   n = WSTRLEN(szBuf);

   (void) ICONFIG_GetItem(pme->m_pIConfig,
                          CFGI_RFCAL_VERSION,
                          (szBuf + n),
                          2 * OEMNV_RFCALVER_LEN);

   n = WSTRLEN(szBuf);
   szBuf[n++] = (AECHAR) '\n';

   (void) ISHELL_LoadResString(pme->a.m_pIShell,
                               FLDDBG_RES_FILE,
                               IDS_RFCAL_DATE,
                               (szBuf + n),
                               sizeof(szBuf));

   n = WSTRLEN(szBuf);

   (void) STRTOWSTR("\n          ", fmt_str, sizeof(fmt_str));
   (void) WSTRLCPY((szBuf + n), fmt_str, sizeof((szBuf + n)));

   n = WSTRLEN(szBuf);

   (void) ICONFIG_GetItem(pme->m_pIConfig,
                          CFGI_RFCAL_DATE,
                          &date,
                          sizeof(date));

   (void) STRTOWSTR("%08x", fmt_str, sizeof(fmt_str));

   WSPRINTF((szBuf + n),
            sizeof(szBuf),
            fmt_str,
            date);

   // prepare the date format
   n = WSTRLEN(szBuf);
   for (i = n - 1, j = n + 1, count = 0;
       count < 4;
       count++) {
      szBuf[j--] = szBuf[i--];
   }

   szBuf[j--] = (AECHAR) '-';

   for (count = 0; count < 2; count++) {
      szBuf[j--] = szBuf[i--];
   }

   szBuf[j] = (AECHAR) '-';

   p_dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
   p_stk = (IStatic *) IDIALOG_GetControl(p_dlg, IDC_VER_STAT);

   // Set the values of the title and text strings for this control
   (void) ISTATIC_SetText(p_stk,
                          NULL,
                          szBuf,
                          AEE_FONT_NORMAL,
                          AEE_FONT_NORMAL);

   (void) ISTATIC_Redraw(p_stk);
}



/*===========================================================================
FUNCTION CFieldDebug_DrawSMSMOSOScreen

DESCRIPTION


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
/===========================================================================*/
static void CFieldDebug_DrawSMSMOSOScreen(CFieldDebug * pme)
{
   IDialog * dlg;
   IMenuCtl *mctl;
   byte mo_so;

   if (!pme)
   {
     MSG_ERROR("Null ptr", 0, 0, 0);
     return;
   }

   dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
   mctl = (IMenuCtl*)IDIALOG_GetControl(dlg, IDC_SMSMOSO_MENU);

   (void) ICONFIG_GetItem(pme->m_pIConfig,
                          CFGI_SMS_SERVICE_OPTION,
                          &mo_so,
                          sizeof(byte));

   InitMenuIcons(mctl);
   switch (mo_so) {
   case OEMNV_SMS_SO_SO6:
      SetMenuIcon(mctl, IDL_SO6_I, TRUE);
      break;
   case OEMNV_SMS_SO_SO14:
      SetMenuIcon(mctl, IDL_SO14_I, TRUE);
      break;
   default:
      break;
   }

   IMENUCTL_SetActive(mctl, TRUE);
   (void) IMENUCTL_Redraw(mctl);
}




/*===========================================================================
FUNCTION CFieldDebug_DrawVoicePrivScreen

DESCRIPTION


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
/===========================================================================*/
static void CFieldDebug_DrawVoicePrivScreen(CFieldDebug * pme)
{
   IDialog * dlg;
   IMenuCtl *mctl;
   //byte vp;

   if (!pme)
   {
     MSG_ERROR("Null ptr", 0, 0, 0);
     return;
   }

   dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
   mctl = (IMenuCtl*)IDIALOG_GetControl(dlg, IDC_VOICE_PRIVACY_MENU);

/*   (void) ICONFIG_GetItem(pme->m_pIConfig,
                          CFGI_VOICE_PRIVACY,
                          &vp,
                          sizeof(byte));

   InitMenuIcons(mctl);
   switch (vp) {
   case OEMNV_VP_STANDARD:
      SetMenuIcon(mctl, IDL_VP_STANDARD_I, TRUE);
      break;
   case OEMNV_VP_ENHANCED:
      SetMenuIcon(mctl, IDL_VP_ENHANCED_I, TRUE);
      break;
   default:
      break;
   }
*/
   IMENUCTL_SetActive(mctl, TRUE);
   (void) IMENUCTL_Redraw(mctl);
}
/*===========================================================================
FUNCTION CFieldDebug_SecServicesHandleEvent

DESCRIPTION
    Event Handler for the AMR Preference Dialog
DEPENDENCIES
  none

RETURN VALUE
  TRUE if event was handled successfully

SIDE EFFECTS
  none
/===========================================================================*/
static boolean CFieldDebug_SecServicesHandleEvent(CFieldDebug *pme,
                                               AEEEvent  eCode,
                                               uint16    wParam,
                                               uint32    dwParam)
{
   if (!pme)
   {
     MSG_ERROR("Null ptr", 0, 0, 0);
     return FALSE;
   }

#if defined(FEATURE_SEC_TESTAPPS) || defined(FEATURE_SEC_IPSEC_VPN_CLNT)
  switch(eCode)
  {
  case EVT_DIALOG_START:
    {
      IMenuCtl *pIMenuCtl = NULL;
      uint16 wMenuID = 0;
      switch(wParam)
      {
      case IDD_SECURITY_SERVICES:
        wMenuID = IDC_SECURITY_TEST;
        break;

      case IDD_SSL_TEST:
        wMenuID = IDC_SSL_TEST;
        break;

      case IDD_SEC_TEST:
        wMenuID = IDC_SEC_TEST;
        break;

      case IDD_DRM_TEST:
        wMenuID = IDC_DRM_TEST;
        break;
      case IDD_VPN_TEST:
        wMenuID = IDC_VPN_TEST;
        break;
      }

      pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl((IDialog*) dwParam, wMenuID);
      if(pIMenuCtl == NULL)
      {
        MSG_ERROR("Unable to get menu for %d", wParam, 0, 0);
        return FALSE;
      }
      if(wParam == IDD_SECURITY_SERVICES)
      {
#ifndef FEATURE_SEC_TESTAPPS
        IMENUCTL_DeleteItem(pIMenuCtl, IDL_SSC_TESTS);
        IMENUCTL_DeleteItem(pIMenuCtl, IDL_SEC_TESTS);
        IMENUCTL_DeleteItem(pIMenuCtl, IDL_DRM_TESTS);
#else
#ifndef FEATURE_SEC_DRM
        IMENUCTL_DeleteItem(pIMenuCtl, IDL_DRM_TESTS);
#endif /* FEATURE_SEC_DRM */
#endif /* FEATURE_SEC_TESTAPPS */

#ifndef FEATURE_SEC_IPSEC_VPN_CLNT
        IMENUCTL_DeleteItem(pIMenuCtl, IDL_VPN_TESTS);
#endif
      }
      SetDefaultMenuLook(pme->a.m_pIShell, pIMenuCtl);
    }
    return TRUE;

  case EVT_KEY:
    switch(wParam)
    {
    case AVK_CLR:
      return CheckAEEReturnStatus(ISHELL_EndDialog(pme->a.m_pIShell));
    }
    break;

  case EVT_COMMAND:
    switch(wParam)
    {
    case IDL_SSL_TESTS:
      return CheckAEEReturnStatus(ISHELL_CreateDialog(pme->a.m_pIShell,
        FLDDBG_RES_FILE, IDD_SSL_TEST, NULL));

    case IDL_SEC_TESTS:
      return CheckAEEReturnStatus(ISHELL_CreateDialog(pme->a.m_pIShell,
        FLDDBG_RES_FILE, IDD_SEC_TEST, NULL));

    case IDL_DRM_TESTS:
      return CheckAEEReturnStatus(ISHELL_CreateDialog(pme->a.m_pIShell,
        FLDDBG_RES_FILE, IDD_DRM_TEST, NULL));

#ifdef FEATURE_SEC_DRM
    case IDL_DRM_ADD_RO:
      DRMTestApp( pme->a.m_pIShell );
      return TRUE;

    case IDL_DRM_READ_RO:
      DRMUnitTestApp( pme->a.m_pIShell );
      return TRUE;
#endif /* FEATURE_SEC_DRM */

    case IDL_VPN_TESTS:
      return CheckAEEReturnStatus(ISHELL_CreateDialog(pme->a.m_pIShell,
        FLDDBG_RES_FILE, IDD_VPN_TEST, NULL));

#ifdef FEATURE_SEC_IPSEC_VPN_CLNT
    case IDL_VPN1_START:
      secipsvpnclnt_start( 0 );
      return TRUE;

    case IDL_VPN2_START:
      secipsvpnclnt_start( 1 );
      return TRUE;

    case IDL_VPN3_START:
      secipsvpnclnt_start( 2 );
      return TRUE;

#endif

    default:
      {
        int nFuncTableSize = ARR_SIZE(SecurityTestTable);
        int i;
        for(i = 0; i <nFuncTableSize; i++)
        {
          if(SecurityTestTable[i].itemID == wParam)
          {
            (SecurityTestTable[i].function)();
            return TRUE;
          }
        }
      }
    }
    return FALSE;

  case EVT_DIALOG_END:
    return TRUE;

  }
#endif //FEATURE_SEC_TESTAPPS || FEATURE_SEC_IPSEC_VPN_CLNT
  return FALSE;
}

/*===========================================================================
FUNCTION CFieldDebug_AMRPrefDialogHandleEvent

DESCRIPTION
    Event Handler for the AMR Preference Dialog
DEPENDENCIES
  none

RETURN VALUE
  TRUE if event was handled successfully

SIDE EFFECTS
  none
/===========================================================================*/
static boolean CFieldDebug_AMRPrefDialogHandleEvent(CFieldDebug *pme,
                                               AEEEvent  eCode,
                                               uint16    wParam,
                                               uint32    dwParam)
{
#if defined(FEATURE_GSM) || defined (FEATURE_WCDMA)
  switch(eCode)
  {
  case EVT_DIALOG_START:
    {
      IMenuCtl *pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl((IDialog*) dwParam, IDC_AMR_PREF);
      boolean amr = FALSE;
      SetDefaultMenuLook(pme->a.m_pIShell, pIMenuCtl);
      //Init the menu.
      ICONFIG_GetItem(pme->m_pIConfig, CFGI_AMR, (void*) &amr, sizeof(amr));

      if(amr)
        SetMenuIcon(pIMenuCtl, IDL_AMR_ON, TRUE);
      else
        SetMenuIcon(pIMenuCtl, IDL_AMR_OFF, TRUE);

    }
    return TRUE;

  case EVT_KEY:
    switch(wParam)
    {
      case AVK_CLR:
        return CheckAEEReturnStatus(ISHELL_EndDialog(pme->a.m_pIShell));
      default:
        break;
    }
    break;

  case EVT_COMMAND:
    switch(wParam)
    {
    case IDL_AMR_ON:
    case IDL_AMR_OFF:
      {
        IDialog *pIDialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
        IMenuCtl *pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_AMR_PREF);
        boolean amr = (wParam == IDL_AMR_ON);
        ICONFIG_SetItem(pme->m_pIConfig, CFGI_AMR, (void*) &amr, sizeof(amr));
        SetMenuIcon(pIMenuCtl, IDL_AMR_ON, amr);
        SetMenuIcon(pIMenuCtl, IDL_AMR_OFF, !amr);
        IMENUCTL_Redraw(pIMenuCtl);
        ISHELL_CancelTimer(pme->a.m_pIShell, (PFNNOTIFY) CFieldDebug_EndAMRDlg, (void*) pme);
        ISHELL_SetTimer(pme->a.m_pIShell, SHOW_MESSAGE_TIMEOUT_MS, (PFNNOTIFY) CFieldDebug_EndAMRDlg, (void*) pme);
      }
      break;

    default:
      break;
    }
    return TRUE;

  case EVT_DIALOG_END:
    ISHELL_CancelTimer(pme->a.m_pIShell, (PFNNOTIFY) CFieldDebug_EndAMRDlg, (void*) pme);
    return TRUE;

  default:
    break;

  }
#endif
  return FALSE;
}

/*===========================================================================
FUNCTION CFieldDebug_FPLMNClearHandleEvent

DESCRIPTION
    Event Handler for the FPLMN Waiting dialog.
DEPENDENCIES
  none

RETURN VALUE
  TRUE if event was handled successfully

SIDE EFFECTS
  none
/===========================================================================*/
static boolean CFieldDebug_FPLMNClearHandleEvent(CFieldDebug *pme,
                                               AEEEvent  eCode,
                                               uint16    wParam,
                                               uint32    dwParam)
{
  if (!pme)
  {
    MSG_ERROR("Null ptr", 0, 0, 0);
    return FALSE;
  }

  switch(eCode)
  {
  case EVT_DIALOG_START:
    return TRUE;

  case EVT_KEY:
    switch(wParam)
    {
      case AVK_CLR:
        return CheckAEEReturnStatus(ISHELL_EndDialog(pme->a.m_pIShell));
      default:
        break;
    }
    break;

  case EVT_DIALOG_END:
    return TRUE;

  default:
    break;

  }
  return FALSE;
}

/*===========================================================================
FUNCTION CFieldDebug_ClearFPLMN

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  TRUE if GSDI_SUCCESS for gsdi calls.

SIDE EFFECTS
  none
/===========================================================================*/
static boolean CFieldDebug_ClearFPLMN(CFieldDebug* pme)
{
#ifdef FEATURE_MMGSDI
  gsdi_returns_T gsdi_status = GSDI_ERROR;
  gsdi_file_attributes_T fplmn_attributes;
  gsdi_card_mode_capability_T  card_mode;
  int                          dummy_rec_ptr;
  byte                         data[GSDI_MAX_DATA_BLOCK_LEN];
  uint32 client_ref = 0;
  PRINT_FUNCTION_NAME();

  if (!pme)
  {
    MSG_ERROR("Null ptr", 0, 0, 0);
    return FALSE;
  }

  /* set the data block to 0xFF */
  MEMSET (data, 0xFF, GSDI_MAX_DATA_BLOCK_LEN);

  gsdi_status = gsdi_get_card_mode(&card_mode);

  if(gsdi_status != GSDI_SUCCESS)
  {
    //Unable to get the card mode.
    return FALSE;
  }

  gsdi_status = GSDI_ERROR;

  switch (card_mode)
  {
  case GSDI_CARD_GSM:
  case GSDI_CARD_GSM_RUIM:
    gsdi_status = gsdi_sim_write(
                               UIM_GSM_FPLMN,
                               GSDI_INDEX_ABSOLUTE,
                               0,
                               NULL,
                               0,
                               data,
                               GSDI_FPLMN_LEN,
                               0,
                               &dummy_rec_ptr,
                               client_ref,
                               CFieldDebug_SIMWrittenFinalCb);
    return (gsdi_status == GSDI_SUCCESS);

  case GSDI_CARD_USIM:

    pme->m_sim_filename = UIM_USIM_FPLMN;
    pme->m_card_mode = GSDI_CARD_USIM;
    gsdi_status = gsdi_get_file_attributes(
                                 UIM_USIM_FPLMN,
                                 &fplmn_attributes,
                                 client_ref,
                                 CFieldDebug_GetFileAttrCb);

    return (gsdi_status == GSDI_SUCCESS);


  case GSDI_CARD_GSM_USIM:
    pme->m_card_mode = GSDI_CARD_GSM_USIM;
    pme->m_sim_filename = UIM_USIM_FPLMN;

    gsdi_status = gsdi_sim_write(
                               UIM_GSM_FPLMN,
                               GSDI_INDEX_ABSOLUTE,
                               0,
                               NULL,
                               0,
                               data,
                               GSDI_FPLMN_LEN,
                               0,
                               &dummy_rec_ptr,
                               client_ref,
                               CFieldDebug_SIMWrittenCb);
    /* Get file attribute for USIM */
    return (gsdi_status == GSDI_SUCCESS);



  }
#endif
  return FALSE;
}

/*===========================================================================
FUNCTION CFieldDebug_ErrorDialogHandleEvent

DESCRIPTION
  Event handler for the Error in clearing FPMN dialog.
DEPENDENCIES
  none

RETURN VALUE
 TRUE if event was handled

SIDE EFFECTS
  none
/===========================================================================*/

static boolean CFieldDebug_ErrorDialogHandleEvent(CFieldDebug *pme,
                                               AEEEvent  eCode,
                                               uint16    wParam,
                                               uint32    dwParam)
{
  AECHAR szMessageText[128];
  IDialog *pIDialog;
  IStatic *pIStatic;

  if (!pme)
  {
    MSG_ERROR("Null ptr", 0, 0, 0);
    return FALSE;
  }

  switch(eCode)
  {
  case EVT_DIALOG_START:
    if (ISHELL_LoadResString(pme->a.m_pIShell,
                             FLDDBG_RES_FILE,
                             pme->m_wStatusResID,
                             szMessageText,
                             sizeof(szMessageText)) > 0)
    {
      //Get the current dialog.
      pIDialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
      if (pIDialog != NULL)
      {
        //Get the static control for this dialog.
        pIStatic = (IStatic *) IDIALOG_GetControl(pIDialog, IDC_STATIC_ERROR);
        if (pIStatic != NULL)
        {
          //Load the message into the status.
          (void) ISTATIC_SetText(pIStatic, NULL, szMessageText,
                          AEE_FONT_NORMAL, AEE_FONT_NORMAL);
        }
        else
        {
          MSG_ERROR("Could not find IDC_STATIC_ERROR in dialog %d",
              IDIALOG_GetID(pIDialog), 0, 0);
        }
      }
      else
      {
        MSG_ERROR("Could not find Dialog", 0, 0, 0);
      }
    }
    else
    {
      MSG_ERROR("Could not find string %d", pme->m_wStatusResID, 0, 0);
    }
      
    (void) ISHELL_SetTimer(pme->a.m_pIShell, SHOW_MESSAGE_TIMEOUT_MS,
      EndErrorMessageDlg, (void*) pme);
    return TRUE;

  case EVT_KEY:
    return CheckAEEReturnStatus(ISHELL_EndDialog(pme->a.m_pIShell));

  case EVT_DIALOG_END:
    pme->m_wStatusResID = IDS_ERROR;
    (void) ISHELL_CancelTimer(pme->a.m_pIShell, EndErrorMessageDlg, (void*) pme);
    return TRUE;

  default:
    break;
  }
  return FALSE;
}

/*===========================================================================
FUNCTION EndErrorMessageDlg

DESCRIPTION
  Called after a time-out for the error dialog display.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
/===========================================================================*/

void EndErrorMessageDlg(void *pUser)
{
  CFieldDebug *pme = (CFieldDebug*) pUser;
  IShell *pIShell;
  IDialog *pIDialog;

  PRINT_FUNCTION_NAME();

  if(pme == NULL)
  {
     MSG_ERROR("pme NULL", 0, 0, 0);
     return;
  }

  pIShell = pme->a.m_pIShell;

  if(pIShell == NULL)
  {
     MSG_ERROR("pIShell NULL", 0, 0, 0);
     return;
  }

  pIDialog = ISHELL_GetActiveDialog(pIShell);

  if(pIDialog == NULL)
  {
    MSG_ERROR("pIDialog NULL", 0, 0, 0);
    return;
  }

  if(IDD_ERROR_DIALOG != IDIALOG_GetID(pIDialog))
  {
    MSG_ERROR("Non-error dialog end", 0, 0, 0);
    return;
  }

  (void) ISHELL_EndDialog(pIShell);

  pIDialog = ISHELL_GetActiveDialog(pIShell);
  if(pIDialog == NULL)
  {
    pme->m_dlgID = IDD_TOP_DIALOG;
    (void) ISHELL_CreateDialog(pIShell,
                    FLDDBG_RES_FILE,
                    pme->m_dlgID,
                    NULL);
    return;
  }

}

#ifdef FEATURE_MMGSDI

/*===========================================================================
FUNCTION CFieldDebug_SIMWrittenFinalCb

DESCRIPTION
 Callback for the final SIM write. Post an event to the app itself.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
/===========================================================================*/
void CFieldDebug_SIMWrittenFinalCb(gsdi_cnf_T* pGSDIConf)
{
  IShell *pIShell = AEE_GetShell();
  gsdi_cnf_T *pLGSDIConf;

  if(pIShell == NULL)
  {
     MSG_ERROR("pIShell NULL", 0, 0, 0);
     return;
  }

  pLGSDIConf= (gsdi_cnf_T*)MALLOC(sizeof(gsdi_cnf_T));
  if(pLGSDIConf == NULL)
  {
    MSG_ERROR("Malloc failed!", 0, 0, 0);
    return;
  }
  MEMCPY((void*) pLGSDIConf, (void*) pGSDIConf, sizeof(gsdi_cnf_T));
  ISHELL_PostEventEx(pIShell, 0, AEECLSID_FIELDDEBUGAPP, EVT_USER,
    EVT_SIM_WRITTEN_FINAL, (uint32) pLGSDIConf);

}

/*===========================================================================
FUNCTION CFieldDebug_SIMWrittenCb

DESCRIPTION
  Callback for the first SIM write. Post an event to the app itself.
DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
/===========================================================================*/
void CFieldDebug_SIMWrittenCb(gsdi_cnf_T* pGSDIConf)
{
  IShell *pIShell = AEE_GetShell();
  gsdi_cnf_T *pLGSDIConf;

  if(pIShell == NULL)
  {
     MSG_ERROR("pIShell NULL", 0, 0, 0);
     return;
  }

  pLGSDIConf= (gsdi_cnf_T*)MALLOC(sizeof(gsdi_cnf_T));
  if(pLGSDIConf == NULL)
  {
    MSG_ERROR("Malloc failed!", 0, 0, 0);
    return;
  }
  MEMCPY((void*) pLGSDIConf, (void*) pGSDIConf, sizeof(gsdi_cnf_T));
  ISHELL_PostEventEx(pIShell, 0, AEECLSID_FIELDDEBUGAPP, EVT_USER,
      EVT_SIM_WRITTEN, (uint32) pLGSDIConf);

}

/*===========================================================================
FUNCTION CFieldDebug_GetFileAttrCb

DESCRIPTION
   Callback for the GetFileAttribute function. Post an event to the app.
DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
/===========================================================================*/
void CFieldDebug_GetFileAttrCb(gsdi_cnf_T * pGSDIConf)
{
  IShell *pIShell = AEE_GetShell();
  gsdi_cnf_T *pLGSDIConf;

  if(pIShell == NULL)
  {
     MSG_ERROR("pIShell NULL", 0, 0, 0);
     return;
  }
  pLGSDIConf= (gsdi_cnf_T*)MALLOC(sizeof(gsdi_cnf_T));
  if(pLGSDIConf == NULL)
  {
    MSG_ERROR("Malloc failed!", 0, 0, 0);
    return;
  }
  MEMCPY((void*) pLGSDIConf, (void*) pGSDIConf, sizeof(gsdi_cnf_T));
  ISHELL_PostEventEx(pIShell, 0, AEECLSID_FIELDDEBUGAPP, EVT_USER,
      EVT_GET_FILE_ATTR, (uint32) pLGSDIConf);

}

#endif
/*===========================================================================
FUNCTION CFieldDebug_RDMCb

DESCRIPTION
  RDM callback function. Called when the result of the rdm request is ready.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
/===========================================================================*/
void CFieldDebug_RDMCb(rdm_assign_status_type status,
                      rdm_service_enum_type service,
                      rdm_device_enum_type device)
{
  IShell *pIShell = AEE_GetShell();
  PRINT_FUNCTION_NAME();
  if(pIShell == NULL)
  {
    MSG_ERROR("pIShell NULL", 0, 0, 0);
    return;
  }

  (void) ISHELL_PostEventEx(pIShell, 0, AEECLSID_FIELDDEBUGAPP, EVT_USER,
      EVT_RDM_STATUS, (int) status);
}

/*===========================================================================
FUNCTION CFieldDebug_MMCDownloadTimeout

DESCRIPTION
  This callback handles the case when the timer governing the
  timeout for the MMC Download mode expires.  It exits MMC Download
  mode and shuts down the corresponding dialogs.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
/===========================================================================*/
void CFieldDebug_MMCDownloadTimeout(void *pUser)
{
  CFieldDebug *pme = (CFieldDebug*) pUser;
  IDialog *pIDialog = NULL;
  uint16   nDlgID;
  int      nSuccess;

  PRINT_FUNCTION_NAME();
  if (!pme)
  {
    MSG_ERROR("Null ptr", 0, 0, 0);
    return;
  }

  /* Make sure we're on the right dialog before we try to cleanup */
  pIDialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
  if (pIDialog == NULL)
  {
    MSG_ERROR("Failed to get active dialog", 0, 0, 0);
    return;
  }

  nDlgID = IDIALOG_GetID(pIDialog);
  if (nDlgID != IDD_MMC_DOWNLOAD_DIALOG)
  {
    MSG_ERROR("MMC download timeout on wrong dialog, %d", nDlgID, 0, 0);
    return;
  }

  /* End the RDM part of MMC Download Mode */
  CFieldDebug_EndMMCDownload(pme);

  /* Now pop out of the MMC Download-related dialogs */
  while ((nDlgID == IDD_MMC_DOWNLOAD_SELECT_TIME) ||
         (nDlgID == IDD_MMC_DOWNLOAD_DIALOG))
  {
    nSuccess = ISHELL_EndDialog(pme->a.m_pIShell);
    if (nSuccess != SUCCESS)
    {
      MSG_ERROR("Failed exiting dialog, %d, %d.", nDlgID, nSuccess, 0);
      break;
    }

    pIDialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
    if (pIDialog == NULL)
    {
      MSG_ERROR("Failed to get active dialog", 0, 0, 0);
      break;
    }
    nDlgID = IDIALOG_GetID(pIDialog);
  }

}

/*===========================================================================
FUNCTION CFieldDebug_InitMMCDownload

DESCRIPTION
  Initialize the MMC download. Get the device and assign the port.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
/===========================================================================*/
void CFieldDebug_InitMMCDownload(void *pUser)
{
  CFieldDebug *pme = (CFieldDebug*) pUser;

  rdm_service_display_type devices[RDM_DEV_MAX];
  boolean status, found;
  int i;

  PRINT_FUNCTION_NAME();

  if (!pme)
  {
    MSG_ERROR("Null ptr", 0, 0, 0);
    return;
  }

  /* First set up the global MMC Download timer, if a timeout is specified */
  if (pme->m_nMMCDLTime != (int32)FLDDBG_MMC_DL_TIME_UNLIMITED)
  {
    (void) ISHELL_SetTimer(pme->a.m_pIShell, pme->m_nMMCDLTime,
                           CFieldDebug_MMCDownloadTimeout, pUser);
  }

  /* Now check and see the valid devices for MMC */
  status = rdm_get_valid_ports (RDM_MMC_SRVC, devices);
  if (!status) {
    //End the current dialog and create the error dialog.
    (void) ISHELL_EndDialog(pme->a.m_pIShell);
    pme->m_wStatusResID = IDS_ERROR;
    (void) ISHELL_CreateDialog(pme->a.m_pIShell,
            FLDDBG_RES_FILE, IDD_ERROR_DIALOG, NULL);
    return;
  }

  found = FALSE;
  /* See if MMC over USB is present in list */
  for (i=0; i< (int) RDM_DEV_MAX;i++) {
    if ((devices[i].device == RDM_USB_MMC_DEV) &&
        (devices[i].display)) {
      found = TRUE;
      break;
    }
  }

  if (!found) {
    //End the current dialog and create the error dialog.
    (void) ISHELL_EndDialog(pme->a.m_pIShell);
    pme->m_wStatusResID = IDS_ERROR;
    (void) ISHELL_CreateDialog(pme->a.m_pIShell,
            FLDDBG_RES_FILE, IDD_ERROR_DIALOG, NULL);
    return;
  }

  pme->rdmState = RDM_STATUS_INIT1;
  pme->old_port_map = rdm_get_device (RDM_DATA_SRVC);
  if (pme->old_port_map == RDM_USB_CDC_DEV)
  {
    (void) rdm_assign_port (RDM_DATA_SRVC, RDM_NULL_DEV,
                      CFieldDebug_RDMCb);
  }
  else
  {
    //Fake the RDM event to go to next state.
    (void) ISHELL_PostEventEx(pme->a.m_pIShell, 0, AEECLSID_FIELDDEBUGAPP, EVT_USER,
      EVT_RDM_STATUS, (uint32)  RDM_DONE_S);
  }



}
/*===========================================================================
FUNCTION CFieldDebug_MMCDownload

DESCRIPTION
  MMC Download function. Called every second till download complete.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
/===========================================================================*/
void CFieldDebug_MMCDownload(void *pUser)
{
  CFieldDebug *pme = (CFieldDebug*) pUser;
  IDialog *pIDialog = NULL;
  IMenuCtl *pIMenuCtl = NULL;
  PRINT_FUNCTION_NAME();

  if (!pme)
  {
    MSG_ERROR("Null ptr", 0, 0, 0);
    return;
  }

  pme->nCount1 = pme->nCount2;
#if (defined(FEATURE_USB) && defined(FEATURE_RUNTIME_DEVMAP))
  pme->nCount2 = usbsdms_get_write_blk_count();
#else
  pme->nCount2 = 0;
#endif
  if(pme->nCount1 == pme->nCount2)
  {
    MSG_MED("MMC Download Done",0, 0, 0);
    pme->bEndKeyProcess = FALSE;

    /* Add the "Quit" softkey, but not if were in background mode.
     * In background mode, we only want to exit MMC download through
     * the "Exit MMC Download" option on the MMC screen.
     */
    if (pme->m_nMMCDLState == FLDDBG_MMC_DOWNLOAD_IN_DL)
    {
      pIDialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
      if(pIDialog != NULL)
      {
        pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_SK_MMC_DOWNLOAD);
        if(pIMenuCtl != NULL)
        {
          (void) IMENUCTL_AddItem(pIMenuCtl, FLDDBG_RES_FILE, IDS_QUIT, IDL_QUIT_MMC_DOWNLOAD, NULL, 0);
          SetDefaultMenuLook(pme->a.m_pIShell, pIMenuCtl);
          (void) IMENUCTL_Redraw(pIMenuCtl);
        }

        (void) IDIALOG_SetFocus(pIDialog, IDC_SK_MMC_DOWNLOAD);
      }
    }
    return;
  }
  (void)ISHELL_SetTimer(pme->a.m_pIShell, MMC_DOWNLOAD_POLL_TIME,
                        CFieldDebug_MMCDownload, pUser);
}

/*===========================================================================
FUNCTION CFieldDebug_MMCDownload_GoToBG

DESCRIPTION
  Continue MMC mode in background.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
/===========================================================================*/
static void CFieldDebug_MMCDownload_GoToBG(CFieldDebug *pme)
{
  IDialog *pIDialog = NULL;
  int      nDlgId = 0;
  int      nSuccess;

  PRINT_FUNCTION_NAME();
  if (!pme)
  {
    MSG_ERROR("Null ptr", 0, 0, 0);
    return;
  }

  pme->m_nMMCDLState = FLDDBG_MMC_DOWNLOAD_IN_DL_BG;
  pme->bEndKeyProcess = FALSE;
  pme->nCount1 = pme->nCount2 = 0;
  (void)ISHELL_CancelTimer(pme->a.m_pIShell, CFieldDebug_MMCDownload, (void*) pme);
  (void)ISHELL_EndDialog(pme->a.m_pIShell);

  /* Pop back to the top-level dialog.  If we don't have an active
   * dialog, just recreate the top-level dialog.
   */
  pIDialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
  if (pIDialog != NULL)
  {
    nDlgId = IDIALOG_GetID(pIDialog);
  }

  while ((pIDialog != NULL) &&
         (nDlgId != IDD_TOP_DIALOG))
  {
    nSuccess = ISHELL_EndDialog(pme->a.m_pIShell);
    if (nSuccess != SUCCESS)
    {
      MSG_ERROR("Failed exiting dialog, %d, %d.", nDlgId, nSuccess, 0);
      break;
    }

    pIDialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
    if (pIDialog != NULL)
    {
      nDlgId = IDIALOG_GetID(pIDialog);
    }
  }

  if (pIDialog == NULL)
  {
    MSG_HIGH("No dialog present.", 0, 0, 0);
    pme->m_dlgID = IDD_TOP_DIALOG;
    (void)ISHELL_CreateDialog(pme->a.m_pIShell, FLDDBG_RES_FILE,
                              pme->m_dlgID, NULL);
  }
}

/*===========================================================================
FUNCTION CFieldDebug_EndMMCDownload

DESCRIPTION
  End the MMC download.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
/===========================================================================*/
void CFieldDebug_EndMMCDownload(CFieldDebug *pme)
{
  PRINT_FUNCTION_NAME();
  if (!pme)
  {
    MSG_ERROR("Null ptr", 0, 0, 0);
    return;
  }

  pme->rdmState = RDM_STATUS_EXIT1;
  (void) rdm_assign_port (RDM_MMC_SRVC, RDM_NULL_DEV,
                  CFieldDebug_RDMCb);
  pme->bEndKeyProcess = FALSE;

}
#if defined(FEATURE_GSM) || defined (FEATURE_WCDMA)
/*===========================================================================
FUNCTION CFieldDebug_EndAMRDlg

DESCRIPTION
    End the AMR Dialog after a time-out after selection is made.
DEPENDENCIES
  none

RETURN VALUE
  TRUE if event was handled successfully

SIDE EFFECTS
  none
/===========================================================================*/
static void CFieldDebug_EndAMRDlg(CFieldDebug *pme)
{
  IDialog *pIDialog;

  if (!pme)
  {
    MSG_ERROR("Null ptr", 0, 0, 0);
    return;
  }

  pIDialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
  if(pIDialog!= NULL)
  {
    if(IDD_AMR_PREF == IDIALOG_GetID(pIDialog))
    {
      ISHELL_EndDialog(pme->a.m_pIShell);
    }
  }
}
#endif
/*===========================================================================

FUNCTION CFieldDebug_JPEGDecodeHandleEvent

DESCRIPTION
  Handle the key/command event for the select file screen.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean CFieldDebug_JPEGDecodeHandleEvent(CFieldDebug *pme,
                                               AEEEvent  eCode,
                                               uint16    wParam,
                                               uint32    dwParam)
{
  PARAM_NOT_REF(dwParam)

#ifdef FEATURE_JPEG_DECODER
  static IDialog * pIActiveDialog = NULL;
  static IMenuCtl * pICurrentMenu;
  CtlAddItem pai;
  cmx_handle_type cmx_handle;
  char pDest[50];
  char sz[2] =   { DIRECTORY_CHAR, 0 };

  if (!pme)
  {
    MSG_ERROR("Null ptr", 0, 0, 0);
    return FALSE;
  }

  switch(eCode)
  {
    case EVT_DIALOG_START:
       pIActiveDialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
       InitJPEGFileList(pme);
       return TRUE;

    case EVT_KEY:
        if (wParam == (uint16) AVK_CLR || wParam == (uint16) AVK_END)
        {
          if (!pme->m_bDisplayImage)
          {
             (void) ISHELL_EndDialog(pme->a.m_pIShell); //End JPEG Decode Dialog.
             (void) ISHELL_CreateDialog(pme->a.m_pIShell, FLDDBG_RES_FILE, IDD_TOP_DIALOG, NULL);
          }
          if (pme->m_bDisplayImage)
             pme->m_bDisplayImage = FALSE;
        }
        return TRUE;

    case EVT_COMMAND:
        {
            IFileMgr * pIFileMgr = NULL;
            int        nRetVal = 0;
            int        nFileNameLen = 0;
            char      *pszBREWPath = NULL;  /* Path using BREW convention */
            char      *pszFSPath = NULL;    /* Path with full EFS convention */
            char       pszTmpPath[100];
            int        nSuccess = AEE_SUCCESS;
    
            pIActiveDialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
            pICurrentMenu = (IMenuCtl *)IDIALOG_GetControl(pIActiveDialog, IDL_JPEG_DECODE);
            (void) IMENUCTL_GetItem(pICurrentMenu, IMENUCTL_GetSel(pICurrentMenu), &pai);
    
            nRetVal = ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_FILEMGR,
                                            (void **) &pIFileMgr);
            if (nRetVal != SUCCESS)
            {
                MSG_MED("Could not create file mgr iface, %d", nRetVal, 0, 0);
                return TRUE;
            }
    
            cmx_handle.efs.source_type = CMX_SOURCE_EFS;
    
            (void)WSTRTOSTR(pai.pText, pDest, sizeof(pDest));
        
            (void)STRLCPY(pszTmpPath, MEDIA_DIR , sizeof(pszTmpPath));
            (void)STRLCAT(pszTmpPath, sz, sizeof(pszTmpPath));
            (void)STRLCAT(pszTmpPath, pDest, sizeof(pszTmpPath));
        
            do
            {
                nRetVal = IFILEMGR_ResolvePath(pIFileMgr, pszTmpPath, NULL, 
                                               &nFileNameLen);
                if ((nRetVal != SUCCESS) || (! nFileNameLen))
                {
                    MSG_MED("Could not resolve path, %d", nRetVal, 0, 0);
                    nSuccess = EFAILED;
                    break;
                }
        
                pszBREWPath = (char *)MALLOC((uint32)nFileNameLen);
                if (! pszBREWPath)
                {
                    MSG_MED("Could not malloc BREW path", 0, 0, 0);
                    nSuccess = EFAILED;
                    break;
                }
        
                nRetVal = IFILEMGR_ResolvePath(pIFileMgr, pszTmpPath, pszBREWPath, 
                                               &nFileNameLen);
                if ((nRetVal != SUCCESS) || (! nFileNameLen))
                {
                    MSG_MED("Could not resolve path, %d", nRetVal, nFileNameLen, 0);
                    nSuccess = EFAILED;
                    break;
                }
#if MIN_BREW_VERSION(4,0)
                nRetVal = OEMefs_GetNativePath(pszBREWPath, NULL, NULL, &nFileNameLen);
#else
                nRetVal = OEMFS_GetNativePath(pszBREWPath, NULL, &nFileNameLen);
#endif //MIN_BREW_VERSION(4,0)
                if ((nRetVal != SUCCESS) || (! nFileNameLen))
                {
                    MSG_MED("Could not resolve path, %d", nRetVal, nFileNameLen, 0);
                    nSuccess = EFAILED;
                    break;
                }
        
                pszFSPath = (char *)MALLOC((uint32)nFileNameLen);
                if (! pszFSPath)
                {
                    MSG_MED("Could not malloc BREW path", 0, 0, 0);
                    nSuccess = EFAILED;
                    break;
                }

#if MIN_BREW_VERSION(4,0)
                nRetVal = OEMefs_GetNativePath(pszBREWPath, pszFSPath, nFileNameLen, &nFileNameLen);
#else
                nRetVal = OEMFS_GetNativePath(pszBREWPath, pszFSPath, &nFileNameLen);
#endif //MIN_BREW_VERSION(4,0)
                if ((nRetVal != SUCCESS) || (! nFileNameLen))
                {
                    MSG_MED("Could not resolve path, %d", nRetVal, nFileNameLen, 0);
                    nSuccess = EFAILED;
                    break;
                }
    
                (void)STRLCPY(cmx_handle.efs.filename, pszFSPath, sizeof(cmx_handle.efs.filename));
    
            } while (0);
            FREEIF(pszBREWPath);
            FREEIF(pszFSPath);

            if (nSuccess == AEE_SUCCESS)
            {
                cmx_jpeg_decode(&cmx_handle, (cmx_image_cb_func_ptr_type) CFieldDebug_JPEGDecodeCB, (void *) pme);
            }
        }
        return TRUE;

    case EVT_DIALOG_END:
        return TRUE;

    default:
        return TRUE;
  }
#else
  return FALSE;
#endif
}


/*===========================================================================

FUNCTION CFieldDebug_HDRDebugHandleEvent

DESCRIPTION
  Handle the key/command event for the HDR Debug Screen

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean CFieldDebug_HDRDebugHandleEvent(CFieldDebug *pme,
                                               AEEEvent  eCode,
                                               uint16    wParam,
                                               uint32    dwParam)
{
#ifdef FEATURE_HDR

  if (!pme)
  {
    MSG_ERROR("Null ptr", 0, 0, 0);
    return FALSE;
  }

  switch(eCode)
  {
    case EVT_DIALOG_START:
       CFieldDebug_HDRScreenRefresh(pme);
       return TRUE;

    case EVT_KEY:
       switch (wParam)
       {
          case AVK_CLR:
          case AVK_END:
             if(pme->bEndKeyProcess == FALSE)
             {
                (void) CFieldDebug_MoveToDialog(pme, IDD_DEBUG_DIALOG);
             }
             //Other wise ignore the clear key so that the dialog is not ended.
             return TRUE;

          default:
             break;
       }
       break;

    case EVT_COMMAND:
       if (wParam == IDL_HDR_OK) {
          if(pme->bEndKeyProcess == FALSE)
          {
             (void) CFieldDebug_MoveToDialog(pme, IDD_DEBUG_DIALOG);
          }
          return TRUE;
       }
       return FALSE;

    case EVT_DIALOG_END:
       (void) ISHELL_CancelTimer(pme->a.m_pIShell,
               (PFNNOTIFY) CFieldDebug_HDRScreenRefresh,
                pme);
       return TRUE;

    default:
       break;
  }
#endif
  return FALSE;

}

#ifdef FEATURE_HDR
/*=============================================================================
FUNCTION:  CFieldDebug_HDRScreenReadItem

DESCRIPTION: Read HDR specific values and make the string to display

PARAMETERS:
   pme: CFieldDebug object pointer

RETURN VALUE:
   AECHAR* to the string to be displayed.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static AECHAR* CFieldDebug_HDRScreenReadItem(CFieldDebug *pme)
{
   byte byte_field;
   int8 int8_field;
   uint16 word_field;
   uint32 dword_field;
   char   temp[512];
   AECHAR* pszBuf = NULL;
   unsigned int i=0;
   temp[0] = '\0';


   if (!pme)
   {
     MSG_ERROR("Null ptr", 0, 0, 0);
     return pszBuf;
   }

   if (!OEMPriv_ReadHDRDebugInfo())
   {
      MSG_ERROR("HDR Info call failed!", 0, 0, 0);
   }

   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_HDR_DEBUG_CHAN_NUM,
                                  &word_field,
                                  sizeof(word_field))) {
      (void) SNPRINTF(temp+i, sizeof(temp), "Channel Number: %d\n", word_field);
      i = STRLEN(temp);
   }

   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_HDR_DEBUG_BAND_CLASS,
                                  &byte_field,
                                  sizeof(byte_field))) {
      (void) SNPRINTF(temp+i, sizeof(temp),"Band Class: %d\n", byte_field);
      i = STRLEN(temp);
   }

   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_HDR_DEBUG_RX_AGC0,
                                  &int8_field,
                                  sizeof(int8_field))) {
      (void) SNPRINTF(temp+i,sizeof(temp),"Rx Chain 0 AGC dBm: %d\n", int8_field);
      i = STRLEN(temp);
   }

   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_HDR_DEBUG_RX_AGC1,
                                  &int8_field,
                                  sizeof(int8_field))) {
      (void) SNPRINTF(temp+i,sizeof(temp),"Rx Chain 1 AGC dBm: %d\n", int8_field);
      i = STRLEN(temp);

   }

   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_HDR_DEBUG_TX_AGC,
                                  &int8_field,
                                  sizeof(int8_field))) {
      (void) SNPRINTF(temp+i,sizeof(temp),"Tx Chain GC dBm: %d\n", int8_field);
      i = STRLEN(temp);
   }

   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_HDR_DEBUG_SERVING_PN,
                                  &word_field,
                                  sizeof(word_field))) {
      (void) SNPRINTF(temp+i,sizeof(temp),"Serving PN: %d\n", word_field);
      i = STRLEN(temp);
   }

   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_HDR_DEBUG_SLEEP_MODE,
                                  &byte_field,
                                  sizeof(byte_field))) {
      char sleep_mode[10];
      switch(byte_field)
      {
         case HDRDEBUG_SLEEP_MODE_SCC:
            (void) STRLCPY(sleep_mode,"SCC", sizeof(sleep_mode) );
            break;

         case HDRDEBUG_SLEEP_MODE_CCC:
            (void) STRLCPY(sleep_mode,"CCC" , sizeof(sleep_mode));
            break;

         case HDRDEBUG_SLEEP_MODE_LONG:
            (void) STRLCPY(sleep_mode,"LONG", sizeof(sleep_mode) );
            break;

         case HDRDEBUG_SLEEP_MODE_URH:
            (void) STRLCPY(sleep_mode,"URH", sizeof(sleep_mode) );
            break;

         default:
            (void) STRLCPY(sleep_mode,"None" , sizeof(sleep_mode));
            break;
      }
      (void) SNPRINTF(temp+i,sizeof(temp),"Sleep Mode: %s(%d)\n", sleep_mode, byte_field);
      i = STRLEN(temp);
   }

   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_HDR_DEBUG_REL0_SCI,
                                  &byte_field,
                                  sizeof(byte_field))) {
      (void)SNPRINTF(temp+i,sizeof(temp),"Rel0 SCI: %d\n", byte_field);
      i = STRLEN(temp);
   }

   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_HDR_DEBUG_RELA_SCI,
                                  &byte_field,
                                  sizeof(byte_field))) {
      (void) SNPRINTF(temp+i,sizeof(temp),"RelA SCI: %d\n", byte_field);
      i = STRLEN(temp);
   }


   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_HDR_DEBUG_SRCH_STATE,
                                  &byte_field,
                                  sizeof(byte_field))) {
      char srch_state[12];
      switch(byte_field)
      {
         case HDRDEBUG_HDRSRCH_INACTIVE_STATE:
            (void) STRLCPY(srch_state,"Inactive", sizeof(srch_state));
            break;

         case HDRDEBUG_HDRSRCH_ACQ_STATE:
            (void) STRLCPY(srch_state,"Acq", sizeof(srch_state));
            break;

         case HDRDEBUG_HDRSRCH_SYNC_STATE:
            (void) STRLCPY(srch_state,"Sync", sizeof(srch_state) );
            break;

         case HDRDEBUG_HDRSRCH_IDLE_STATE:
            (void) STRLCPY(srch_state,"Idle", sizeof(srch_state) );
            break;

         case HDRDEBUG_HDRSRCH_SUSPENDED_IDLE_STATE:
            (void) STRLCPY(srch_state,"SuspIdle", sizeof(srch_state) );
            break;

         case HDRDEBUG_HDRSRCH_OFS_IDLE_STATE:
            (void) STRLCPY(srch_state,"OfreqIdle", sizeof(srch_state)  );
            break;

         case HDRDEBUG_HDRSRCH_SLEEP_STATE:
            (void) STRLCPY(srch_state,"Sleep", sizeof(srch_state)  );
            break;

         case HDRDEBUG_HDRSRCH_REACQ_STATE:
            (void) STRLCPY(srch_state,"Reacq", sizeof(srch_state)  );
            break;

         case HDRDEBUG_HDRSRCH_CONNECTED_STATE:
            (void) STRLCPY(srch_state,"Connect", sizeof(srch_state)  );
            break;

         case HDRDEBUG_HDRSRCH_SUSPENDED_TC_STATE:
            (void) STRLCPY(srch_state,"SuspConn", sizeof(srch_state)  );
            break;

         case HDRDEBUG_HDRSRCH_OFS_TC_STATE:
            (void) STRLCPY(srch_state,"OfreqConn", sizeof(srch_state) );
            break;

         default:
            (void) STRLCPY(srch_state,"None", sizeof(srch_state) );
            break;
      }
      (void) SNPRINTF(temp+i,sizeof(temp),"Srch State: %s(0x%x)\n", srch_state, byte_field);
      i = STRLEN(temp);
   }

   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_HDR_DEBUG_RX_DIV,
                                  &byte_field,
                                  sizeof(byte_field))) {
      char rx_div[10];
      switch(byte_field)
      {
         case HDRDEBUG_RX_DIV_RX0_ONLY:
            (void) STRLCPY(rx_div,"Rx0 Only", sizeof(rx_div) );
            break;

         case HDRDEBUG_RX_DIV_BOTH:
            (void) STRLCPY(rx_div,"RxDiv", sizeof(rx_div) );
            break;

         default:
            (void) STRLCPY(rx_div,"None" , sizeof(rx_div));
            break;
      }
      (void) SNPRINTF(temp+i,sizeof(temp),"Rx Div: %s(%d)\n", rx_div,byte_field);
      i = STRLEN(temp);
   }

   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_HDR_DEBUG_PROT_STATE,
                                  &byte_field,
                                  sizeof(byte_field))) {
      char prot_state[10];
      switch(byte_field)
      {
         case HDRDEBUG_PROT_STATE_INACTIVE:
            (void) STRLCPY(prot_state,"Inactive", sizeof(prot_state) );
            break;

         case HDRDEBUG_PROT_STATE_ACQ:
            (void) STRLCPY(prot_state,"Acq", sizeof(prot_state) );
            break;

         case HDRDEBUG_PROT_STATE_SYNC:
            (void) STRLCPY(prot_state,"Sync", sizeof(prot_state) );
            break;

         case HDRDEBUG_PROT_STATE_IDLE:
            (void) STRLCPY(prot_state,"Idle", sizeof(prot_state) );
            break;

         case HDRDEBUG_PROT_STATE_ACCESS:
            (void) STRLCPY(prot_state,"Access", sizeof(prot_state) );
            break;

         case HDRDEBUG_PROT_STATE_CONNECTED:
            (void) STRLCPY(prot_state,"Connect", sizeof(prot_state) );
            break;

         default:
            (void) STRLCPY(prot_state,"None", sizeof(prot_state) );
            break;
      }
      (void) SNPRINTF(temp+i,sizeof(temp),"Prot State: %s(%d)\n", prot_state,byte_field);
      i = STRLEN(temp);
   }


   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_HDR_DEBUG_HDR_SESSION_STATE,
                                  &byte_field,
                                  sizeof(byte_field))) {
      char hdr_session_state[10];
      switch(byte_field)
      {
         case HDRDEBUG_SESSION_STATE_CLOSE:
            (void) STRLCPY(hdr_session_state,"Inactive", sizeof(hdr_session_state) );
            break;

         case HDRDEBUG_SESSION_STATE_AMP_SETUP:
            (void) STRLCPY(hdr_session_state,"AMPSetup", sizeof(hdr_session_state) );
            break;

         case HDRDEBUG_SESSION_STATE_AT_INIT:
            (void) STRLCPY(hdr_session_state,"AT Init", sizeof(hdr_session_state) );
            break;

         case HDRDEBUG_SESSION_STATE_AN_INIT:
            (void) STRLCPY(hdr_session_state,"AN Init", sizeof(hdr_session_state) );
            break;

         case HDRDEBUG_SESSION_STATE_OPEN:
            (void) STRLCPY(hdr_session_state,"Open", sizeof(hdr_session_state) );
            break;

         default:
            (void) STRLCPY(hdr_session_state,"None", sizeof(hdr_session_state) );
            break;
      }
      (void) SNPRINTF(temp+i,sizeof(temp),"HDR Session: %s(%d)\n", hdr_session_state,byte_field);
      i = STRLEN(temp);
   }

   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_HDR_DEBUG_UATI,
                                  &dword_field,
                                  sizeof(dword_field))) {
      (void) SNPRINTF(temp+i,sizeof(temp),"UATI: 0x%x\n", dword_field);
      i = STRLEN(temp);
   }

   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_HDR_DEBUG_COLOR_CODE,
                                  &byte_field,
                                  sizeof(byte_field))) {
      (void) SNPRINTF(temp+i,sizeof(temp),"Color Code: %d\n", byte_field);
      i = STRLEN(temp);
   }


   pszBuf = (AECHAR*) MALLOC((i+1)*sizeof(AECHAR));
   if(pszBuf != NULL)
   {
     (void) STRTOWSTR(temp, pszBuf,(i+1)*sizeof(AECHAR) );
   }
   else
   {
     MSG_ERROR("Malloc failed!", 0, 0, 0);
   }
   return pszBuf;
}


/*===========================================================================
FUNCTION CFieldDebug_HDRScreenRefresh

DESCRIPTION
  Timer callback function to update the HDR Debug Screen periodically.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
/===========================================================================*/
static void CFieldDebug_HDRScreenRefresh( CFieldDebug *pme )
{
   AECHAR *szBuf;
   IDialog *p_dlg;
   IStatic *p_stk;
   int currentLine;

   if (!pme)
   {
     MSG_ERROR("Null ptr", 0, 0, 0);
     return;
   }

   p_dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
   p_stk = (IStatic *) IDIALOG_GetControl(p_dlg, IDC_HDR_STATIC);

   szBuf = CFieldDebug_HDRScreenReadItem(pme);
   if (szBuf != NULL)
   {
   // get current text line from BREW
   currentLine = ISTATIC_GoToLine(p_stk, 0);
   // Set the values of the title and text strings for this control
   (void) ISTATIC_SetText(p_stk,
                          NULL,
                          szBuf,
                          AEE_FONT_NORMAL,
                          AEE_FONT_NORMAL);

   FREEIF(szBuf);
   ISTATIC_SetProperties(p_stk, ST_NOSCROLL);
   // go to current text line
   currentLine = ISTATIC_GoToLine(p_stk, currentLine);
   (void) ISTATIC_Redraw(p_stk);
   }

   (void) ISHELL_SetTimer(pme->a.m_pIShell,
                          DEBUG_SCRN_REFRESH_PERIOD,
                          (PFNNOTIFY) CFieldDebug_HDRScreenRefresh,
                          pme );

}
#endif /* FEATURE_HDR */

#ifdef FEATURE_JPEG_DECODER
/*===========================================================================

FUNCTION InitJPEGFileList

DESCRIPTION
  Lists all files under media directory.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void InitJPEGFileList(CFieldDebug * pme)
{
   IDialog * pIActiveDialog = NULL;
   uint16 wDlgId;
   IMenuCtl * pIJPEGFileListMenuCtl = NULL;
   IFileMgr * pIFileMgr;
   FileInfo fi;
   char * szBuf;
   AECHAR fileNameBuf[50];
   int wItemID = 0;

   if (!pme)
   {
     MSG_ERROR("Null ptr", 0, 0, 0);
     return;
   }

   pIActiveDialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
   wDlgId = IDIALOG_GetID(pIActiveDialog);

   if (wDlgId == IDD_JPEG_DECODE)
   {
     pIJPEGFileListMenuCtl = (IMenuCtl *)IDIALOG_GetControl(pIActiveDialog, IDL_JPEG_DECODE);

     if (pIJPEGFileListMenuCtl != NULL)
       {
         (void) IMENUCTL_DeleteAll(pIJPEGFileListMenuCtl);
         (void) IMENUCTL_SetTitle(pIJPEGFileListMenuCtl, FLDDBG_RES_FILE, IDS_CHOOSE_FILE, NULL);
         // Create file manager instance and list all the files under media directory.
         (void) ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_FILEMGR, (void **) &pIFileMgr);
         (void) IFILEMGR_EnumInit(pIFileMgr, MEDIA_DIR, FALSE);
         while (IFILEMGR_EnumNext(pIFileMgr, &fi))
          {
            szBuf = CFieldDebug_GetFileName(fi.szName);
            (void) STRTOWSTR(szBuf, fileNameBuf, sizeof(fileNameBuf));

            (void) IMENUCTL_AddItem(pIJPEGFileListMenuCtl, FLDDBG_RES_FILE, 0, wItemID, fileNameBuf, 0);
            wItemID++;
          } //while
     } //if
   } //if wDlgId
}

/*===========================================================================

FUNCTION CFieldDebug_GetFileName

DESCRIPTION
  Extracts file name from directory path.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static char * CFieldDebug_GetFileName(const char * psz)
{
   char *   pszName = STRRCHR(psz, (int)DIRECTORY_CHAR);
   if (pszName)
      pszName++;
   else
      pszName = (char *)psz;
   return pszName;
}

/*===========================================================================

FUNCTION CFieldDebug_JPEGDecodeCB

DESCRIPTION  Callback function provided to CMX.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void CFieldDebug_JPEGDecodeCB ( cmx_status_type status, const void *client_data,
                         void *server_data, uint32 num_bytes )
{
  uint8              **jpegd_buf_ptr;
  CFieldDebug * pme = (CFieldDebug *) client_data;
  IShell * pShell;

  if (!pme)
  {
    MSG_ERROR("Null ptr", 0, 0, 0);
    return;
  }

  switch (status) {
   case CMX_SUCCESS:
     pme->x_offset = 0;
     pme->y_offset= 0;
     break;

   case CMX_ABORTED:
     break;

   case CMX_GET_BUFFER:
     jpegd_buf_ptr = (uint8 **) server_data;
     if (m_bufJPEG)
       FREE(m_bufJPEG);
     m_bufJPEG = MALLOC(num_bytes);
     *jpegd_buf_ptr = (uint8 *) &(m_bufJPEG[0]);
     break;

   case CMX_CONTINUE:
     break;

   case CMX_JPEGD_SPEC:
        {
          cmx_jpeg_spec_type* jpegd_spec = (cmx_jpeg_spec_type *) server_data;
          pme->m_rc.dx = jpegd_spec->width;
          pme->m_rc.dy = jpegd_spec->height;
          CFieldDebug_CreateIBitmap(pme, jpegd_spec->width, jpegd_spec->height);
        }
        break;

   case CMX_DONE:
        if (m_bufJPEG)
        {
          CFieldDebug_RGB24toRGB16((uint16*)pme->m_pIDIB->pBmp,
                                   (uint8*)m_bufJPEG,
                                   pme->m_rc.dx * pme->m_rc.dy * 2);
          pShell = AEE_GetShell();
          (void) ISHELL_PostEventEx(pShell, EVTFLG_ASYNC,AEECLSID_FIELDDEBUGAPP,
                             EVT_JPEG_DECODE_DONE, 0, 0);
        }
        FREE(m_bufJPEG);
        m_bufJPEG = NULL;
        break;

   case CMX_FAILURE:
   default:
     break;
  }
}

/*===========================================================================

FUNCTION   CFieldDebug_RGB24toRGB16

DESCRIPTION
  Compress data from RGB24 to RGB16 for display
  num_bytes is for the output bytes for RGB16

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void CFieldDebug_RGB24toRGB16(uint16 * outPtr, uint8 * inPtr, uint32 num_bytes)
{
    uint32 cnt;

    if (!outPtr || !inPtr)
    {
      MSG_ERROR("NULL ptr: 0x%x 0x%x", outPtr, inPtr, 0);
      return;
    }

    for (cnt = num_bytes/2; cnt > 0 ; cnt--)
    {
       *outPtr = ((inPtr[0]&0xF8)<<8)+((inPtr[1]&0xFC)<<3)+((inPtr[2]&0xF8)>>3);
       outPtr++;
       inPtr += 3;
    }
}
/*===========================================================================

FUNCTION CFieldDebug_CreateIBitmap

DESCRIPTION  Creates DIB object.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void CFieldDebug_CreateIBitmap(CFieldDebug * pme, uint16 x, uint16 y)
{
   uint8 clrDepth;

   clrDepth = 16;

   if (!pme)
   {
      MSG_ERROR("Null ptr", 0, 0, 0);
      return;
   }

   (void) IDISPLAY_CreateDIBitmap(pme->a.m_pIDisplay, (IDIB**)&pme->m_pIDIB, clrDepth, x, y);

   if(pme->m_pIDIB) {
      //We can set the pitch correctly here
      pme->m_pIDIB->nPitch = x * (clrDepth/8);
      pme->m_pIDIB->cx = x;
      pme->m_pIDIB->cy = y;
      if(clrDepth == 16)
         pme->m_pIDIB->nColorScheme = IDIB_COLORSCHEME_565;
      else
         pme->m_pIDIB->nColorScheme = IDIB_COLORSCHEME_332;
   }
}
/*===========================================================================

FUNCTION CFieldDebug_DisplayImage

DESCRIPTION  Display image on the screen.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void CFieldDebug_DisplayImage(CFieldDebug * pme)
{
   if (!pme)
   {
      MSG_ERROR("Null ptr", 0, 0, 0);
      return;
   }

   IDISPLAY_ClearScreen(pme->a.m_pIDisplay);
   IDISPLAY_BitBlt(pme->a.m_pIDisplay, 0, 17, pme->m_rc.dx, pme->m_rc.dy,
                   (const void *) pme->m_pIDIB, pme->x_offset,
                   pme->y_offset, AEE_RO_COPY );
   IDISPLAY_Update(pme->a.m_pIDisplay);
}
#endif /* FEATURE_JPEG_DECODER */

/*===========================================================================
FUNCTION CFieldDebug_EndDlg

DESCRIPTION
    End a specified dialog after a time-out after selection is made.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none
/===========================================================================*/
static void CFieldDebug_EndDlg(void *pData)
{
  CFieldDebug *pMe = (CFieldDebug *)pData;
  IDialog     *pIDialog = NULL;

  if (! pMe)
  {
    MSG_ERROR("NULL data ptr", 0, 0, 0);
    return;
  }

  pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if(pIDialog && (pMe->m_nEndDlgId == IDIALOG_GetID(pIDialog)))
  {
    (void)ISHELL_EndDialog(pMe->a.m_pIShell);
  }
  pMe->m_nEndDlgId = 0;
}

/*===========================================================================

FUNCTION CFieldDebug_DisableInCallDisplay

DESCRIPTION  This function handles events for the screen allowing the
             user to turn off the display module during calls.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if event handled, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean CFieldDebug_DisableInCallDisplay(CFieldDebug *pme,
                                                AEEEvent eCode,
                                                uint16 wParam,
                                                uint32 dwParam)
{

  if (! pme)
  {
    MSG_ERROR("NULL ptr", 0, 0, 0);
    return FALSE;
  }

  switch(eCode)
  {
    case EVT_DIALOG_START:
    {
      IMenuCtl *pIMenuCtl;
      IDialog  *pIDialog = (IDialog *)dwParam;
      boolean   bDisableInCallDisp = FALSE;

      if (! pIDialog)
      {
        MSG_ERROR("NULL ptr", 0, 0, 0);
        return FALSE;
      }
      pIMenuCtl = (IMenuCtl *)IDIALOG_GetControl(pIDialog,
                                                 IDC_DISABLE_IN_CALL_DISP);
      if (! pIMenuCtl)
      {
        MSG_ERROR("NULL ptr", 0, 0, 0);
        return FALSE;
      }

      SetDefaultMenuLook(pme->a.m_pIShell, pIMenuCtl);

      // Get the current setting and turn on the corresponding radio button.
      (void)ICONFIG_GetItem(pme->m_pIConfig, CFGI_DISABLE_IN_CALL_DISP,
                            (void*) &bDisableInCallDisp,
                            sizeof(bDisableInCallDisp));

      if(bDisableInCallDisp)
      {
        SetMenuIcon(pIMenuCtl, IDC_DISABLE_IN_CALL_DISP_YES, TRUE);
        SetMenuIcon(pIMenuCtl, IDC_DISABLE_IN_CALL_DISP_NO, FALSE);
      }
      else
      {
        SetMenuIcon(pIMenuCtl, IDC_DISABLE_IN_CALL_DISP_NO, TRUE);
        SetMenuIcon(pIMenuCtl, IDC_DISABLE_IN_CALL_DISP_YES, FALSE);
      }

    }
    return TRUE;
  
    case EVT_KEY:
    switch(wParam)
    {
      case AVK_CLR:
        return CheckAEEReturnStatus(ISHELL_EndDialog(pme->a.m_pIShell));

      default:
        break;
    }
    break;
  
    case EVT_COMMAND:
      switch(wParam)
      {
        case IDC_DISABLE_IN_CALL_DISP_YES:
        case IDC_DISABLE_IN_CALL_DISP_NO:
        {
          IDialog  *pIDialog;
          IMenuCtl *pIMenuCtl;
          boolean   bDisableInCallDisp;

          pIDialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
          if (! pIDialog)
          {
            MSG_ERROR("NULL ptr", 0, 0, 0);
            return FALSE;
          }

          pIMenuCtl = (IMenuCtl *)IDIALOG_GetControl(pIDialog,
                                                     IDC_DISABLE_IN_CALL_DISP);
          if (! pIMenuCtl)
          {
            MSG_ERROR("NULL ptr", 0, 0, 0);
            return FALSE;
          }

          /* Calculate the new config item value and set it. */
          bDisableInCallDisp = (wParam == IDC_DISABLE_IN_CALL_DISP_YES);
          (void)ICONFIG_SetItem(pme->m_pIConfig, CFGI_DISABLE_IN_CALL_DISP,
                                (void *)&bDisableInCallDisp,
                                sizeof(bDisableInCallDisp));

          SetMenuIcon(pIMenuCtl, IDC_DISABLE_IN_CALL_DISP_YES,
                      bDisableInCallDisp);
          SetMenuIcon(pIMenuCtl, IDC_DISABLE_IN_CALL_DISP_NO,
                      (boolean)!bDisableInCallDisp);
          (void)IMENUCTL_Redraw(pIMenuCtl);

          // Cancel any "end dialog" timer, and reset it for this dialog.
          (void)ISHELL_CancelTimer(pme->a.m_pIShell, (PFNNOTIFY)CFieldDebug_EndDlg,
                                   (void *)pme);
          pme->m_nEndDlgId = IDD_DISABLE_IN_CALL_DISP;
          (void)ISHELL_SetTimer(pme->a.m_pIShell, SHOW_MESSAGE_TIMEOUT_MS,
                                (PFNNOTIFY)CFieldDebug_EndDlg, (void *)pme);
        }
        break;
  
        default:
          break;
      }
      return TRUE;
  
    case EVT_DIALOG_END:
      (void)ISHELL_CancelTimer(pme->a.m_pIShell, (PFNNOTIFY)CFieldDebug_EndDlg,
                               (void *)pme);
      return TRUE;
  
    default:
      break;

  }
  return FALSE;
}

/*===========================================================================

FUNCTION CFieldDebug_MakeGameModePersist

DESCRIPTION  This function handles events for the screen allowing the
             user to make game mode persistent.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if event handled, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean CFieldDebug_MakeGameModePersist(CFieldDebug *pme,
                                              AEEEvent eCode,
                                              uint16 wParam,
                                              uint32 dwParam)
{

  if (! pme)
  {
    MSG_ERROR("NULL ptr", 0, 0, 0);
    return FALSE;
  }

  switch(eCode)
  {
    case EVT_DIALOG_START:
    {
      IMenuCtl *pIMenuCtl;
      IDialog  *pIDialog = (IDialog *)dwParam;
      boolean   bGameModePersist = FALSE;

      if (! pIDialog)
      {
        MSG_ERROR("NULL ptr", 0, 0, 0);
        return FALSE;
      }
      pIMenuCtl = (IMenuCtl *)IDIALOG_GetControl(pIDialog,
                                                 IDC_MAKE_GAME_MODE_PERSIST);
      if (! pIMenuCtl)
      {
        MSG_ERROR("NULL ptr", 0, 0, 0);
        return FALSE;
      }

      SetDefaultMenuLook(pme->a.m_pIShell, pIMenuCtl);

      // Get the current setting and turn on the corresponding radio button.
      (void)ICONFIG_GetItem(pme->m_pIConfig, CFGI_GAME_MODE_PERSIST,
                            (void*) &bGameModePersist,
                            sizeof(bGameModePersist));

      if(bGameModePersist)
      {
        SetMenuIcon(pIMenuCtl, IDC_MAKE_GAME_MODE_PERSIST_YES, TRUE);
        SetMenuIcon(pIMenuCtl, IDC_MAKE_GAME_MODE_PERSIST_NO, FALSE);
      }
      else
      {
        SetMenuIcon(pIMenuCtl, IDC_MAKE_GAME_MODE_PERSIST_NO, TRUE);
        SetMenuIcon(pIMenuCtl, IDC_MAKE_GAME_MODE_PERSIST_YES, FALSE);
      }

    }
    return TRUE;
  
    case EVT_KEY:
    switch(wParam)
    {
      case AVK_CLR:
        return CheckAEEReturnStatus(ISHELL_EndDialog(pme->a.m_pIShell));

      default:
        break;
    }
    break;
  
    case EVT_COMMAND:
      switch(wParam)
      {
        case IDC_MAKE_GAME_MODE_PERSIST_YES:
        case IDC_MAKE_GAME_MODE_PERSIST_NO:
        {
          IDialog  *pIDialog;
          IMenuCtl *pIMenuCtl;
          boolean   bGameModePersist;

          pIDialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
          if (! pIDialog)
          {
            MSG_ERROR("NULL ptr", 0, 0, 0);
            return FALSE;
          }

          pIMenuCtl = (IMenuCtl *)IDIALOG_GetControl(pIDialog,
                                                     IDC_MAKE_GAME_MODE_PERSIST);
          if (! pIMenuCtl)
          {
            MSG_ERROR("NULL ptr", 0, 0, 0);
            return FALSE;
          }

          /* Calculate the new config item value and set it. */
          bGameModePersist = (wParam == IDC_MAKE_GAME_MODE_PERSIST_YES);
          (void)ICONFIG_SetItem(pme->m_pIConfig, CFGI_GAME_MODE_PERSIST,
                                (void *)&bGameModePersist,
                                sizeof(bGameModePersist));

          SetMenuIcon(pIMenuCtl, IDC_MAKE_GAME_MODE_PERSIST_YES,
                      bGameModePersist);
          SetMenuIcon(pIMenuCtl, IDC_MAKE_GAME_MODE_PERSIST_NO,
                      (boolean)!bGameModePersist);
          (void)IMENUCTL_Redraw(pIMenuCtl);

          // Cancel any "end dialog" timer, and reset it for this dialog.
          (void)ISHELL_CancelTimer(pme->a.m_pIShell, (PFNNOTIFY)CFieldDebug_EndDlg,
                                   (void *)pme);
          pme->m_nEndDlgId = IDD_MAKE_GAME_MODE_PERSIST;
          (void)ISHELL_SetTimer(pme->a.m_pIShell, SHOW_MESSAGE_TIMEOUT_MS,
                                (PFNNOTIFY)CFieldDebug_EndDlg, (void *)pme);
        }
        break;
  
        default:
          break;
      }
      return TRUE;
  
    case EVT_DIALOG_END:
      (void)ISHELL_CancelTimer(pme->a.m_pIShell, (PFNNOTIFY)CFieldDebug_EndDlg,
                               (void *)pme);
      return TRUE;
  
    default:
      break;

  }
  return FALSE;
}


