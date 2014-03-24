/*=============================================================================

FILE: FieldDebug.c

SERVICES: Hidden Menu applet

GENERAL DESCRIPTION:
   N/A

PUBLIC CLASSES AND STATIC FUNCTIONS:
   IFieldDebug

INITIALIZATION AND SEQUENCING REQUIREMENTS:
   N/A

(c) COPYRIGHT 2002,2003,2004, 2006 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
=============================================================================*/


/*=============================================================================

  $Header: //depot/asic/qsc60x0/apps/FldDbg/FldDbgApp.c#9 $
$DateTime: 2006/09/19 16:37:56 $

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ----------------------------------------------------------
09/15/06     sj      Fixed compilation warnings
08/28/06     sj      Fix compiler warnings
05/31/06     jas/mh  Modified featurization of MMC card-related code
03/08/06     jas/ch  Updating params to the "start app" diag for BREW 3.1.510/15/05     sada    Added new diags: Get Active Applet, Get App Name & Get Selected Menu Ctl Text
12/04/04     sada    Rerouted Get_ActiveDlgId to Field Debug APP
11/24/04     sada    Changed initialization of parameter sAppStartInfo in Evt_Flddbg_App_Start
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
#include "comdef.h"

#include "AEE.h"
#include "AEE_OEM.h"

#include "AEEShell.h"
#include "AEEMenu.h"
#include "AEEModTable.h"
#include "AEEStdLib.h"
#include "AEEDB.h"
#include "nv.h"
#include "flddbg.brh"
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
#include "cm.h"
#include "appscommonimages.brh"
#include "Appscommon.h"
#include "AEEClipboard.h"

#include "msg.h"
#include "err.h"
#include "oemnvint.h"
#include "mobile.h"
#include "nv.h"

#if MIN_BREW_VERSION(3, 1)
#include "AEEAppletCtl.h"
#endif

#if MIN_BREW_VERSION(3,0)
// For BREW 3.0
#include "AEE_OEMDispatch.h"
#endif

#if defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
#include "cai.h"
#endif

#ifdef FEATURE_ICM
#include "AEECM.h"
#else
#include "AEETelephone.h"
#include "AEETelDef.h"
//#include "uixcm.h"
#endif

#include "fs.h"
#ifdef FEATURE_EFS2
#include "fs_public.h"
#endif

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
#include "AEEAnnunciator.h"

#ifdef FEATURE_SEC_TESTAPPS
#error code not present
#endif/* FEATURE_SEC_TESTAPPS */

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif

#ifdef FEATURE_SEC_DRM
extern DRMUnitTestApp( IShell* pIShell );
extern DRMTestApp( IShell* pIShell );
#endif /* FEATURE_SEC_DRM */

#ifdef FEATURE_SEC_IPSEC_VPN_CLNT
#include "secipsvpnclnt.h"
#endif /* FEATURE_SEC_IPSEC_VPN_CLNT */

#include "AEERats.h"
#include "Nv_items.h"
/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/
// wParam for an EVT_COMMAND event for displaying
// a screen during Clear EFS
#define DISP_TIMEOUT_WPARAM      1019

// Periodicity of refresh for Debug Screen in milliseconds
#define DEBUG_SCRN_REFRESH_PERIOD     1000 /* 200msec */ 

// Time in milliseconds the message 'Clearing...' will stay up
// during Clear Errors before closing
#define SHOW_MESSAGE_TIMEOUT_MS       (2*1000)

#define MAX_IP_SIZE     15   /* Size of an IP address string. */
#define MAX_TEST_DIGITS 8    /* Size of the test number of digits. */
#ifndef SETAEERECT
#define SETAEERECT(prc,l,t,w,h)   (prc)->x=(int16)(l),(prc)->y=(int16)(t),(prc)->dx=(int16)(w),(prc)->dy=(int16)(h)
#endif

#define drawBottomBar( bottomBarType)               \
{                                                   \
    BottomBar_Param_type BarParam;                  \
    MEMSET(&BarParam, 0, sizeof(BarParam));         \
    BarParam.eBBarType = bottomBarType;             \
    DrawBottomBar(pme->m_pDisplay, &BarParam);      \
}



// EVT_USER type events
#define EVT_SIM_WRITTEN_FINAL 0x0001
#define EVT_SIM_WRITTEN       0x0002
#define EVT_GET_FILE_ATTR     0x0004

#define EVT_RDM_STATUS        0x0008

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION

#define RDM_STATUS_INIT1       0x0001
#define RDM_STATUS_INIT2       0x0002
#define RDM_STATUS_INIT3       0x0004
#define RDM_STATUS_INIT4       0x0008

#define RDM_STATUS_EXIT1       0x0010
#define RDM_STATUS_EXIT2       0x0020
#define RDM_STATUS_EXIT3       0x0040
#define RDM_STATUS_EXIT4       0x0080

#define IPFILE                 "api_test_ip.dat" /* Name of the database for IP address. */
// 设置界面底部提示条宏定义
#ifdef FEATURE_CUSTOMIZED_MENU_STYLE 
#define MENU_SETBOTTOMBAR(p, t)                     \
{                                                   \
    IMENUCTL_SetBottomBarType((p), (t));            \
}

#else
#define MENU_SETBOTTOMBAR(p, t)
#endif /* FEATURE_CUSTOMIZED_MENU_STYLE */


#ifdef FEATURE_JPEG_DECODER
#define MEDIA_DIR "media"
#define EVT_JPEG_DECODE_DONE 10000
#define ROOT_DIR "brew"
dword * m_bufJPEG = NULL;
#endif /* FEATURE_JPEG_DECODER */


#ifdef FEATURE_HDR
#error code not present
#endif
// 画界面底部提示条宏定义
/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/

enum
{
	NETLOCK = 10,
	ENABLELOCK,
	NVSETTING,
	MNC,
	MCC,
	SID
};

typedef struct _CFieldDebug {
   AEEApplet    a;
   IConfig     *m_pIConfig;
   uint16       m_debugmenu_sel;
   uint16       m_dlgID;
   IRats       *m_pIRats;        //Pointer to remote API test object.
   AEERect      m_screen_rc;     //Rectangle for displaying image.
   int          m_LastFileLocation;  //Last number of bytes read.
   int          m_LastLineNumber;    //Last Line Number found.
   IDisplay       *m_pDisplay;
   IShell           *m_pIShell;
   IStatic          *m_pIStatic;
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
#error code not present
#endif /* defined(FEATURE_GSM) || defined (FEATURE_WCDMA) */

   AEECallback m_AeeCallback;      // Callback structure for all callback init
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION

   boolean    m_bSuspended;    // True if app is either background or suspended
   int16      m_wStatusResID;  // Resource to load into status dialog.
   IDialog                         *m_pActiveIDlg;
   IAnnunciator *m_pIAnn;
   uint32     m_CurrId;
   uint8      m_TextId;
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
static boolean CFieldDebug_FactorySetCountHandleEvent(CFieldDebug *pme,
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

static void CFieldDebug_DebugScreenRefresh(CFieldDebug *pme);
static void CFieldDebug_DrawErrorScreen(CFieldDebug * pme);
static void CFieldDebug_DrawSMSMOSOScreen(CFieldDebug *pme);
static void CFieldDebug_DrawVoicePrivScreen(CFieldDebug *pme);
static AECHAR* CFieldDebug_DebugScreenReadItem(CFieldDebug *pme);
static void CFieldDebug_DrawVersionScreen(CFieldDebug *pme);
static void CFieldDebug_DrawFactorySetCountScreen(CFieldDebug *pme);
static void CFieldDebug_ClearCallBackFn(IShell *ps);
static boolean CFieldDebug_ClearFPLMN(CFieldDebug* pme);
static boolean CFieldDebug_NET_LOCK_HandleEvent(CFieldDebug *pme,
                                           AEEEvent  eCode,
                                           uint16    wParam,
                                           uint32    dwParam);

static boolean CFieldDebug_Enabled_Lock_HandleEvent(CFieldDebug *pme,
                                           AEEEvent  eCode,
                                           uint16    wParam,
                                           uint32    dwParam);
static boolean CFieldDebug_MNC_HandleEvent(CFieldDebug *pme,
                                           AEEEvent  eCode,
                                           uint16    wParam,
                                           uint32    dwParam);
static boolean CFieldDebug_MCC_HandleEvent(CFieldDebug *pme,
                                           AEEEvent  eCode,
                                           uint16    wParam,
                                           uint32    dwParam);

static boolean CFieldDebug_SID_HandleEvent(CFieldDebug *pme,
                                           AEEEvent  eCode,
                                           uint16    wParam,
                                           uint32    dwParam);
static	boolean CFieldDebug_RouteDialogEvt(CFieldDebug *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
);
static boolean CFieldDebug_NVSETTINGHandleEvent(CFieldDebug *pme,
                                           AEEEvent  eCode,
                                           uint16    wParam,
                                           uint32    dwParam);


#ifdef FEATURE_JPEG_DECODER
static void CFieldDebug_RGB24toRGB16(uint16 * outPtr, uint8 * inPtr, uint32 num_bytes);
#endif

#if defined(FEATURE_GSM) || defined (FEATURE_WCDMA)
#error code not present
#endif
void CFieldDebug_RDMCb(rdm_assign_status_type status,
                      rdm_service_enum_type service,
                      rdm_device_enum_type device);
void CFieldDebug_InitMMCDownload(void *pUser);
void CFieldDebug_MMCDownload(void *pUser);
void CFieldDebug_EndMMCDownload(CFieldDebug *pme);

void EndErrorMessageDlg(void *pUser);
#if (defined(FEATURE_USB) && defined(FEATURE_MMC)) || defined (FEATURE_USB_MASS_STORAGE)
extern dword usbsdms_get_write_blk_count(void);
#endif /* (defined(FEATURE_USB) && defined(FEATURE_MMC)) */
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
#error code not present
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
extern boolean   IsRunAsUIMVersion(void);

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
#error code not present
#endif

#ifdef FEATURE_SEC_DRM
  {IDS_DRM_USE_RO,  sectestapp_secdrm_test_1},
  {IDS_DRM_DEL_ARO,  sectestapp_secdrm_test_2},
  {IDS_DRM_DEL_RO,  sectestapp_secdrm_test_3},
#endif

#ifdef FEATURE_SEC_IPSEC_VPN_CLNT
  {IDS_VPN_STOP,  secipsvpnclnt_stop},
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
#error code not present
#endif


static boolean CFieldDebug_EsnMenuHandleEvent(CFieldDebug *pme,
                                               AEEEvent  eCode,
                                               uint16    wParam,
                                               uint32    dwParam);
static void CFieldDebug_DrawEsnScreen(CFieldDebug *pme);

static void SetTextControlRect(CFieldDebug *pme, void  *Ctl);
static byte CharToHex(char ch);
static byte CalcMEIDDSP(char * Str);
static byte CalcMEIDHex(char * Str);

/*===========================================================================
全局数据
===========================================================================*/
extern const char ver_date[];
extern const char ver_time[];

/*===========================================================================

                      STATIC/LOCAL DATA

===========================================================================*/

////
// This is a list of dialog event handers that correspond to the
// dialogs in the fdnmenu.bar file, indexed by dialog resource Id
//
// IMPORTANT!  Dialog resource IDs are assumed to start at 1 and
//             must not have gaps between successive IDs (ie. 1,2,4,5).
//
static const PFNAEEEVENT sDialogEventHandlers[] =
{
   (PFNAEEEVENT) CFieldDebug_TopMenuHandleEvent,     // IDD_TOP_DIALOG
   (PFNAEEEVENT) CFieldDebug_DebugMenuHandleEvent,   // IDD_DEBUG_DIALOG
   (PFNAEEEVENT) CFieldDebug_VersionMenuHandleEvent, // IDD_VERSION_DIALOG
   (PFNAEEEVENT) CFieldDebug_DebugScreenHandleEvent, // IDD_DEBUG_SCREEN_DIALOG
   (PFNAEEEVENT) CFieldDebug_TestCallHandleEvent,    // IDD_CALL_LIST_DIALOG
   (PFNAEEEVENT) CFieldDebug_ErrorListHandleEvent,   // IDD_ERROR_LIST_DIALOG
   (PFNAEEEVENT) CFieldDebug_ClearErrorsHandleEvent, // IDD_CLEAR_ERROR_DIALOG
   (PFNAEEEVENT) CFieldDebug_ClearEFSHandleEvent,    // IDD_CLEAR_EFS_DIALOG
   (PFNAEEEVENT) CFieldDebug_DataModeHandleEvent,    // IDD_DM_DIALOG
   (PFNAEEEVENT) CFieldDebug_SMSMOSOHandleEvent,     // IDD_SMS_MO_SO_DIALOG
   (PFNAEEEVENT) CFieldDebug_VoicePrivHandleEvent,   // IDD_VOICE_PRIVACY_DIALOG
   (PFNAEEEVENT) CFieldDebug_MMCHandleEvent,         // IDD_MMC_DIALOG
   (PFNAEEEVENT) CFieldDebug_MMCFormatHandleEvent,   // IDD_MMC_FORMAT_DIALOG
   (PFNAEEEVENT) CFieldDebug_MMCDownloadHandleEvent, // IDD_MMC_DOWNLOAD_DIALOG
   (PFNAEEEVENT) CFieldDebug_FPLMNClearHandleEvent,  // IDD_WAIT_FPLMN_CLEAR_DIALOG
   (PFNAEEEVENT) CFieldDebug_ErrorDialogHandleEvent, // IDD_ERROR_DIALOG
   (PFNAEEEVENT) CFieldDebug_AMRPrefDialogHandleEvent, // IDD_AMR_PREF
   (PFNAEEEVENT) CFieldDebug_SecServicesHandleEvent,  // IDD_SECURITY_SERVICES
   (PFNAEEEVENT) CFieldDebug_SecServicesHandleEvent,  // IDD_SSL_TEST
   (PFNAEEEVENT) CFieldDebug_SecServicesHandleEvent,  // IDD_SEC_TEST
   (PFNAEEEVENT) CFieldDebug_SecServicesHandleEvent,  // IDD_DRM_TEST
   (PFNAEEEVENT) CFieldDebug_SecServicesHandleEvent,  // IDD_VPN_TEST  
   (PFNAEEEVENT) CFieldDebug_JPEGDecodeHandleEvent,  // IDD_JPEG_DECODE
   (PFNAEEEVENT) CFieldDebug_RATSHandleEvent,        // IDD_RATS_DIALOG
   (PFNAEEEVENT) CFieldDebug_IPHandleEvent,          // IDD_RATS_IP_DIALOG
   (PFNAEEEVENT) CFieldDebug_RATS_TESTHandleEvent,    // IDD_RATS_TEST_DIALOG
   (PFNAEEEVENT) CFieldDebug_HDRDebugHandleEvent,     // IDD_HDR_DEBUG_INFORMATION
   (PFNAEEEVENT) CFieldDebug_VoiceDebugHandleEvent,   //IDD_VOICE_DEBUG
   (PFNAEEEVENT) CFieldDebug_EsnMenuHandleEvent, // IDD_ESN_DIALOG  
   (PFNAEEEVENT) CFieldDebug_NET_LOCK_HandleEvent,
   (PFNAEEEVENT) CFieldDebug_Enabled_Lock_HandleEvent,
   (PFNAEEEVENT) CFieldDebug_MNC_HandleEvent,
   (PFNAEEEVENT) CFieldDebug_MCC_HandleEvent,
   (PFNAEEEVENT) CFieldDebug_SID_HandleEvent,
   (PFNAEEEVENT) CFieldDebug_NVSETTINGHandleEvent,
   (PFNAEEEVENT) CFieldDebug_FactorySetCountHandleEvent
};



/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

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
    ERR("EVT_APP_START",0,0,0);
}

#endif

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
#error code not present
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
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION

      pme->m_debugmenu_sel = IDS_DBGSCRN;

      pme->m_bSuspended = FALSE;

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
   if (pme->m_pDisplay != NULL)
    {
        (void) IDISPLAY_Release(pme->m_pDisplay);
        pme->m_pDisplay = NULL;
    }
   if (pme->m_pIConfig) {
      (void) ICONFIG_Release(pme->m_pIConfig);
   }
   if(pme->m_pIAnn)
    {
        IANNUNCIATOR_Release(pme->m_pIAnn);
        pme->m_pIAnn = NULL;
    }
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
      pai.wImage = IDB_RADIO_FILLED;
      break;
    case IRATS_STATUS_DOWN:
      pai.wImage = IDB_RADIO_UNFILLED;
      break;
    default:
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
static boolean CFieldDebug_OnDialogStart(CFieldDebug  *pMe,
                                     uint16       wParam,
                                     uint32       dwParam)
{
  IMenuCtl *pm = NULL ,*psk=NULL;
  MSG_FATAL("CFieldDebug_OnDialogStart Start", 0, 0, 0);
  switch (wParam) {
    case IDD_TOP_DIALOG:
    {
      AECHAR WTitle[10] = {0};
      (void)ISHELL_LoadResString(pMe->a.m_pIShell,
      FLDDBG_RES_FILE,                                
      IDS_OPTION_TITLE,
      WTitle,
      sizeof(WTitle));
      if(pMe->m_pIAnn != NULL)
      {
          IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
      }  	
      pm = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                           IDC_SYS_MENU);
      IMENUCTL_SetProperties(pm, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_ACTIVE_NO_REDRAW);
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_DEBUG_TITLE,  IDS_DEBUG_TITLE, NULL, 0);
	  IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_NV_SETTING, IDS_NV_SETTING,NULL,0);
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_VERSION_TITLE,  IDS_VERSION_TITLE, NULL, 0);
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_PROGRAM_TITLE,  IDS_PROGRAM_TITLE, NULL, 0);
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_ESN,  IDS_ESN, NULL, 0);      
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_JPEG_DECODE,  IDS_JPEG_DECODE, NULL, 0);
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_RATS_TITLE,  IDS_RATS_TITLE, NULL, 0);   
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_STRING_NET_LOCK,  IDS_STRING_NET_LOCK, NULL, 0);  
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_FACTORY_COUNT,  IDS_FACTORY_COUNT, NULL, 0);      
      MENU_SETBOTTOMBAR(pm,BTBAR_OK_CANCEL); 
      }                                     
      break;
    case IDD_DEBUG_DIALOG:
      {
      pm = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                             IDC_DEBUG_MENU);
      IMENUCTL_SetProperties(pm, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
      {
        AECHAR WTitle[40] = {0};
        MSG_FATAL("CFieldDebug_OnDialogStart 2", 0, 0, 0);
        (void)ISHELL_LoadResString(pMe->a.m_pIShell,
        FLDDBG_RES_FILE,                                
        IDS_DEBUG_TITLE,
        WTitle,
        sizeof(WTitle));
        if(pMe->m_pIAnn != NULL)
        {
            IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
        }
      }    
      MSG_FATAL("CFieldDebug_OnDialogStart 2", 0, 0, 0);
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_DBGSCRN,  IDS_DBGSCRN, NULL, 0);
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_TEST_CALL,  IDS_TEST_CALL, NULL, 0);
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_ERRORS,  IDS_ERRORS, NULL, 0);
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_CLR_ERR,  IDS_CLR_ERR, NULL, 0);
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_CLR_EFS,  IDS_CLR_EFS, NULL, 0);
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_DATA_DM,  IDS_DATA_DM, NULL, 0);
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_SMS_MO_SO,  IDS_SMS_MO_SO, NULL, 0);
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_VOICE_PRIVACY,  IDS_VOICE_PRIVACY, NULL, 0);
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_GIGATEST,  IDS_GIGATEST, NULL, 0);
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_AMR_PREF,  IDS_AMR_PREF, NULL, 0);
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_CONTACTIMPORT,  IDS_CONTACTIMPORT, NULL, 0); 
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_CONTACTEXPORT,  IDS_CONTACTEXPORT, NULL, 0);
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_HDR_INFO,  IDS_HDR_INFO, NULL, 0);
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_GIGATEST,  IDS_GIGATEST, NULL, 0);
      //IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_MMC,  IDS_MMC, NULL, 0);
      //IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_CLEAR_FPLMN,  IDS_CLEAR_FPLMN, NULL, 0);
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_SECURITY_SERVICES,  IDS_SECURITY_SERVICES, NULL, 0);
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_CALTEST,  IDS_CALTEST, NULL, 0);
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_VOICE_DEBUG,  IDS_VOICE_DEBUG, NULL, 0);

      MENU_SETBOTTOMBAR(pm,BTBAR_OK_CANCEL); 

       }                                    
      break;
	case IDD_NET_LOCK_DIALOG:
		{
			MSG_FATAL ("EVT_DIALOG_START wParam == IDD_NET_LOCK_DIALOG1111111111", 0, 0, 0);
			pm = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                             IDC_NET_LOCK_MENU);
      		IMENUCTL_SetProperties(pm, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
            {
                AECHAR WTitle[10] = {0};
                (void)ISHELL_LoadResString(pMe->a.m_pIShell,
                FLDDBG_RES_FILE,                                
                IDS_STRING_NET_LOCK,
                WTitle,
                sizeof(WTitle));
                if(pMe->m_pIAnn != NULL)
                {
                    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
                }
            }     
			IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_STRING_NET_LOCK_FLAG,  IDS_STRING_NET_LOCK_FLAG, NULL, 0);
      		IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_STRING_NET_LOCK_MNC,   IDS_STRING_NET_LOCK_MNC, NULL, 0);
      		IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_STRING_NET_LOCK_MCC,   IDS_STRING_NET_LOCK_MCC, NULL, 0);
      		IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_STRING_NET_LOCK_SID,   IDS_STRING_NET_LOCK_SID, NULL, 0);
			MENU_SETBOTTOMBAR(pm,BTBAR_OK_CANCEL); 
			MSG_FATAL ("EVT_DIALOG_START wParam == IDD_NET_LOCK_DIALOG222222222", 0, 0, 0);
       }                                    
      break;    
    case IDD_FACTORY_COUNT:
    {
        psk = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam, IDC_COUNT_STAT);     
        IMENUCTL_AddItem(psk, FLDDBG_RES_FILE, IDS_CANCEL,  IDS_CANCEL, NULL, 0);  
        break;
    }
        
    case IDD_VERSION_DIALOG:
    	{
	    
       psk = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                           IDC_VER_SOFTKEY);
     // IMENUCTL_AddItem(psk, FLDDBG_RES_FILE, IDS_OK,  IDS_OK, NULL, 0);    
      IMENUCTL_AddItem(psk, FLDDBG_RES_FILE, IDS_CANCEL,  IDS_CANCEL, NULL, 0);       

      
       }
      break;
    case IDD_ESN_DIALOG:
    	{
	    MSG_FATAL("CFieldDebug_OnDialogStart case IDD_ESN_DIALOG", 0, 0, 0);
        psk = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam, IDC_ESN_SOFTKEY);
        if(psk == NULL)
        {
            MSG_FATAL("CFieldDebug_OnDialogStart case IDD_ESN_DIALOG psk == NULL", 0, 0, 0);
			break;
        }
          {
            AECHAR WTitle[5] = {0};
            (void)ISHELL_LoadResString(pMe->a.m_pIShell,
            FLDDBG_RES_FILE,                                
            IDS_ESN,
            WTitle,
            sizeof(WTitle));
            if(pMe->m_pIAnn != NULL)
            {
                IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
          }           
       }
      break;
      
    case IDD_DEBUG_SCREEN_DIALOG:
    	{		
		
			
      psk = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                           IDC_DBSCR_SOFTKEY);
     // IMENUCTL_AddItem(psk, FLDDBG_RES_FILE, IDS_OK,  IDS_OK, NULL, 0);    
      IMENUCTL_AddItem(psk, FLDDBG_RES_FILE, IDS_CANCEL,  IDS_CANCEL, NULL, 0);
    
      }
      break;
    case IDD_CALL_LIST_DIALOG:
    	{
      pm = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                           IDC_CALL_LIST_MENU);
      IMENUCTL_SetProperties(pm, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
      {
        AECHAR WTitle[20] = {0};
        MSG_FATAL("CFieldDebug_OnDialogStart 3", 0, 0, 0);
        (void)ISHELL_LoadResString(pMe->a.m_pIShell,
        FLDDBG_RES_FILE,                                
        IDS_VPN3_START,
        WTitle,
        sizeof(WTitle));
        if(pMe->m_pIAnn != NULL)
        {
            IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
        }
      }       
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_TCALL_N_13K,  IDS_TCALL_N_13K, NULL, 0);
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_TCALL_N_8K,  IDS_TCALL_N_8K, NULL, 0);
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_TCALL_13K_LOOP,  IDS_TCALL_13K_LOOP, NULL, 0);
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_TCALL_8K_LOOP,  IDS_TCALL_8K_LOOP, NULL, 0);
	  
       MENU_SETBOTTOMBAR(pm,BTBAR_OK_CANCEL); 
      }
      break;

    case IDD_ERROR_LIST_DIALOG:
    	{
			
      psk = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                           IDC_ERROR_SOFTKEY);
      //IMENUCTL_AddItem(psk, FLDDBG_RES_FILE, IDS_OK,  IDS_OK, NULL, 0);
      IMENUCTL_AddItem(psk, FLDDBG_RES_FILE, IDS_CANCEL,  IDS_CANCEL, NULL, 0);  
     
      }
      break;
    case IDD_CLEAR_ERROR_DIALOG:
    	{
      psk = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                           IDC_CLRERR_SOFTKEY);
      IMENUCTL_AddItem(psk, FLDDBG_RES_FILE, IDS_NO_TXT,  IDS_NO_TXT, NULL, 0);
      IMENUCTL_AddItem(psk, FLDDBG_RES_FILE, IDS_YES_TXT,  IDS_YES_TXT, NULL, 0);
      }
      break;
    case IDD_CLEAR_EFS_DIALOG:
    	{
      psk = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                           IDC_CLREFS_SOFTKEY);
      {
        AECHAR WTitle[20] = {0};
        (void)ISHELL_LoadResString(pMe->a.m_pIShell,
        FLDDBG_RES_FILE,                                
        IDS_CLEAR_EFS,
        WTitle,
        sizeof(WTitle));
        if(pMe->m_pIAnn != NULL)
        {
            IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
        }
      }            
      IMENUCTL_AddItem(psk, FLDDBG_RES_FILE, IDS_OK,  IDS_OK, NULL, 0);
      IMENUCTL_AddItem(psk, FLDDBG_RES_FILE, IDS_DEL,  IDS_DEL, NULL, 0);
      }                                     
      break;
    case IDD_DM_DIALOG:
    	{
      pm = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                           IDC_DATADM_MENU);
      IMENUCTL_SetProperties(pm, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
      {
        AECHAR WTitle[20] = {0};
        MSG_FATAL("CFieldDebug_OnDialogStart 4", 0, 0, 0);
        (void)ISHELL_LoadResString(pMe->a.m_pIShell,
        FLDDBG_RES_FILE,                                
        IDS_DATA_DM,
        WTitle,
        sizeof(WTitle));
        if(pMe->m_pIAnn != NULL)
        {
            IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
        }
      }        
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_DIAG,  IDS_DIAG, NULL, 0);
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_DATA_19200,  IDS_DATA_19200, NULL, 0);
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_DATA_38400,  IDS_DATA_38400, NULL, 0);
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_DATA_57600,  IDS_DATA_57600, NULL, 0);
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_DATA_115200,  IDS_DATA_115200, NULL, 0);
      psk = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                            IDC_SOFTKEY_DM);
      IMENUCTL_AddItem(psk, FLDDBG_RES_FILE, IDS_OK,  IDS_OK, NULL, 0);    
      IMENUCTL_AddItem(psk, FLDDBG_RES_FILE, IDS_CANCEL,  IDS_CANCEL, NULL, 0);
      }                                     
      break;
    case IDD_SMS_MO_SO_DIALOG:
    	{
      pm = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                           IDC_SMSMOSO_MENU);
      IMENUCTL_SetProperties(pm, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
      {
        AECHAR WTitle[20] = {0};
        MSG_FATAL("CFieldDebug_OnDialogStart 5", 0, 0, 0);
        (void)ISHELL_LoadResString(pMe->a.m_pIShell,
        FLDDBG_RES_FILE,                                
        IDS_SET_SMSMO_SO,
        WTitle,
        sizeof(WTitle));
        if(pMe->m_pIAnn != NULL)
        {
            IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
        }
      }         
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_SMSMO_SO_6,  IDS_SMSMO_SO_6, NULL, 0);
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_SMSMO_SO_14,  IDS_SMSMO_SO_14, NULL, 0);
      psk = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                            IDC_SOFTKEY_SMS_MO_SO);
      IMENUCTL_AddItem(psk, FLDDBG_RES_FILE, IDS_OK,  IDS_OK, NULL, 0);    
      IMENUCTL_AddItem(psk, FLDDBG_RES_FILE, IDS_CANCEL,  IDS_CANCEL, NULL, 0);
      }
      break;
    case IDD_VOICE_PRIVACY_DIALOG:
    	{
      pm = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                           IDC_VOICE_PRIVACY_MENU);
      IMENUCTL_SetProperties(pm, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
      {
        AECHAR WTitle[30] = {0};
        MSG_FATAL("CFieldDebug_OnDialogStart 6", 0, 0, 0);
        (void)ISHELL_LoadResString(pMe->a.m_pIShell,
        FLDDBG_RES_FILE,                                
        IDS_VOICE_PRIVACY,
        WTitle,
        sizeof(WTitle));
        if(pMe->m_pIAnn != NULL)
        {
            IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
        }
      }        
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_VP_STANDARD,  IDS_VP_STANDARD, NULL, 0);
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_VP_ENHANCED,  IDS_VP_ENHANCED, NULL, 0);
       psk = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                            IDC_SOFTKEY_VOICE_PRIVACY);
      IMENUCTL_AddItem(psk, FLDDBG_RES_FILE, IDS_OK,  IDS_OK, NULL, 0);    
      IMENUCTL_AddItem(psk, FLDDBG_RES_FILE, IDS_CANCEL,  IDS_CANCEL, NULL, 0);
      }
      break;

    case IDD_MMC_DIALOG:
    	{
      pm = (IMenuCtl *) IDIALOG_GetControl ((IDialog *) dwParam,
                                            IDC_MMC_MENU);
      IMENUCTL_SetProperties(pm, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
      {
        AECHAR WTitle[10] = {0};
        MSG_FATAL("CFieldDebug_OnDialogStart 7", 0, 0, 0);
        (void)ISHELL_LoadResString(pMe->a.m_pIShell,
        FLDDBG_RES_FILE,                                
        IDS_MMC,
        WTitle,
        sizeof(WTitle));
        if(pMe->m_pIAnn != NULL)
        {
            IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
        }
      }        
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_MMC_DOWNLOAD,  IDS_MMC_DOWNLOAD, NULL, 0);
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_MMC_FORMAT,  IDS_MMC_FORMAT, NULL, 0);

       MENU_SETBOTTOMBAR(pm,BTBAR_OK_CANCEL); 
    
      }
      
      break;

    case IDD_MMC_DOWNLOAD_DIALOG:
      psk = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                            IDC_SK_MMC_DOWNLOAD);
      {
        AECHAR WTitle[30] = {0};
        MSG_FATAL("CFieldDebug_OnDialogStart 8", 0, 0, 0);
        (void)ISHELL_LoadResString(pMe->a.m_pIShell,
        FLDDBG_RES_FILE,                                
        IDS_MMC_DOWNLOAD,
        WTitle,
        sizeof(WTitle));
        if(pMe->m_pIAnn != NULL)
        {
            IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
        }
      }         
      IMENUCTL_AddItem(psk, FLDDBG_RES_FILE, IDS_OK,  IDS_OK, NULL, 0);    
      IMENUCTL_AddItem(psk, FLDDBG_RES_FILE, IDS_CANCEL,  IDS_CANCEL, NULL, 0);
      break;
    case IDD_MMC_FORMAT_DIALOG:
		
	
      psk = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                            IDC_SOFTKEY_FORMAT);
	  	
    // IMENUCTL_AddItem(psk, FLDDBG_RES_FILE, IDS_OK,  IDS_OK, NULL, 0);    
     IMENUCTL_AddItem(psk, FLDDBG_RES_FILE, IDS_BACK,  IDS_BACK, NULL, 0);
     
      break;

    case IDD_RATS_DIALOG:
    	{
      pm = (IMenuCtl *) IDIALOG_GetControl ((IDialog *) dwParam,
                                            IDC_RATS_MENU);
      IMENUCTL_SetProperties(pm, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
      {
        AECHAR WTitle[40] = {0};
        MSG_FATAL("CFieldDebug_OnDialogStart 9", 0, 0, 0);
        (void)ISHELL_LoadResString(pMe->a.m_pIShell,
        FLDDBG_RES_FILE,                                
        IDS_RATS_TITLE,
        WTitle,
        sizeof(WTitle));
        if(pMe->m_pIAnn != NULL)
        {
            IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
        }
      }         
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_CLIENT_UP,  IDS_CLIENT_UP, NULL, 0);
      IMENUCTL_AddItem(pm, FLDDBG_RES_FILE, IDS_SERVER_UP,  IDS_SERVER_UP, NULL, 0);                                      
      psk = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                            IDC_RATS_SOFTKEY);
      IMENUCTL_SetProperties(psk, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
      IMENUCTL_AddItem(psk, FLDDBG_RES_FILE, IDS_SET_IP,  IDS_SET_IP, NULL, 0);
      IMENUCTL_AddItem(psk, FLDDBG_RES_FILE, IDS_RUN_TESTS,  IDS_RUN_TESTS, NULL, 0);                                      
       }
      break;

    case IDD_RATS_IP_DIALOG:
      psk = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                            IDC_RATS_IP_SOFTKEY);
      IMENUCTL_AddItem(psk, FLDDBG_RES_FILE, IDS_OK,  IDS_OK, NULL, 0);    
      IMENUCTL_AddItem(psk, FLDDBG_RES_FILE, IDS_CANCEL,  IDS_CANCEL, NULL, 0);
      break;

    case IDD_RATS_TEST_DIALOG:
      break;
      
   

           

    default:
      MSG_FATAL ("Unhandled dialog %d", wParam, 0,0);
      return FALSE;
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
   int ret = 0;
   (void) ISHELL_EndDialog(pme->a.m_pIShell);

   ret = ISHELL_CreateDialog(pme->a.m_pIShell,
                           FLDDBG_RES_FILE,
                           id,
                           NULL);
   if (ret != AEE_SUCCESS) {
      MSG_FATAL ("Could not create dialog ret=%d", ret, 0, 0);
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
      if (wParam == IDS_DONE_SK) {
         (void) CFieldDebug_MoveToDialog(pme, IDD_TOP_DIALOG);
         return TRUE;
      }
      return FALSE;

   case EVT_KEY:
      switch (wParam) {

      case AVK_CLR:
#ifdef FEATURE_CARRIER_CHINA_TELCOM 
         if (pme->m_dlgID == IDD_VERSION_DIALOG)
         {
            //ISHELL_CloseApplet(pme->a.m_pIShell, FALSE); // This return the EditNum dialog
            ISHELL_CloseApplet(pme->a.m_pIShell, TRUE); // This return the Idle dialog
         }
#endif
         if (pme->m_dlgID == IDD_TOP_DIALOG)
         {
            (void) CFieldDebug_MoveToDialog(pme, IDD_TOP_DIALOG);
         }
#if defined(FEATURE_VERSION_W515V3)||defined(FEATURE_VERSION_S1000T) ||defined(FEATURE_VERSION_KK5)
         else if (pme->m_dlgID == IDD_VERSION_DIALOG)
         {
            //ISHELL_CloseApplet(pme->a.m_pIShell, FALSE); // This return the EditNum dialog
            ISHELL_CloseApplet(pme->a.m_pIShell, TRUE); // This return the Idle dialog
         }
#endif         
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


static boolean CFieldDebug_FactorySetCountHandleEvent(CFieldDebug *pme,
                                                                       AEEEvent  eCode,
                                                                       uint16    wParam,
                                                                       uint32    dwParam)
{
   PARAM_NOT_REF(dwParam)

   switch (eCode) {

   case EVT_COMMAND:
      if (wParam == IDS_DONE_SK) {
         (void) CFieldDebug_MoveToDialog(pme, IDD_TOP_DIALOG);
         return TRUE;
      }
      return FALSE;

   case EVT_KEY:
      switch (wParam) {

      case AVK_CLR:
         if (pme->m_dlgID == IDD_TOP_DIALOG)
         {
            (void) CFieldDebug_MoveToDialog(pme, IDD_TOP_DIALOG);
         }  
         return TRUE;

      default:
         break;
      }
      return FALSE;

   case EVT_DIALOG_START:
      (void) CFieldDebug_OnDialogStart (pme, wParam, dwParam);
      CFieldDebug_DrawFactorySetCountScreen(pme);
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
   uint16 w;
   uint32 dw;
   byte b;
   char   temp[512] = {'\0'};
   unsigned int i = 0;
   short rx_rssi, dyn_range;
   AECHAR *pszBuf = NULL;

#ifdef T_MSM6500
//#error code not present
#endif //T_MSM6500

   // Get the current Band or RF state
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_BAND,
                                  &b,
                                  sizeof(b))) {
     char band_name[6];

#ifdef T_MSM6500
//#error code not present
#endif //T_MSM6500

        switch (b)
        {
        case CFGI_DEBUG_BAND_AMPS:
          strcpy(band_name, "AMPS");
          break;
        case CFGI_DEBUG_BAND_CDMA:
          strcpy(band_name, "CDMA");
          break;
        case CFGI_DEBUG_BAND_PCS:
          strcpy(band_name, "PCS");
          break;
        case CFGI_DEBUG_BAND_GPS:
          strcpy(band_name, "GPS");
          break;
        case CFGI_DEBUG_BAND_SLEEP:
          strcpy(band_name, "SLEEP");
          break;
        default:
          strcpy(band_name, "UKNW");
          break;
        }

     SPRINTF(temp+i,"RF Mode: %s\n", band_name);
     i = STRLEN(temp);

#ifdef T_MSM6500
//#error code not present
#endif // T_MSM6500

   }

   // Get the current Band and Channel
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_CHANNEL,
                                  &w,
                                  sizeof(w))) {

      SPRINTF(temp+i,"Channel: %d\n", w);
      i = STRLEN(temp);
   }

   // Get the active pilot
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_PILOT_PN,
                                  &w,
                                  sizeof(w))) {
      SPRINTF(temp+i,"Pilot PN: %d\n", w);
      i = STRLEN(temp);
   }

   // Get Rx AGC in dBm
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_RX_AGC_IN_DBM,
                                  &dw,
                                  sizeof(dw))) {
     
#ifndef T_MSM6500
	 int32 rx_agc = (int32)dw;
     SPRINTF(temp+i,"Rx AGC dBm: %d.%d\n", (rx_agc/10), abs(rx_agc%10));
#else
//#error code not present
#endif
     i = STRLEN(temp);
   }

   // Get LNA State
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_LNA_STATE,
                                  &b,
                                  sizeof(b))) {
#ifndef T_MSM6500
      SPRINTF(temp+i,"LNA State: %d\n", b);
#else
//#error code not present
#endif
      i = STRLEN(temp);
   }

   // Get Tx AGC in dBm
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_TX_AGC_IN_DBM,
                                  &w,
                                  sizeof(w))) {
     int16 tx_agc = (int16)w;
     if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                              CFGI_DEBUG_PA_STATE,
                              &b,
                              sizeof(b)))
     {
         if ((b & 0x10) == 0)
         {
             tx_agc = 0;
         }
     }
     if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                    CFGI_DEBUG_RATCHET_STATE,
                                    &b,
                                    sizeof(b))) {
       char ratchet_state[15];

       if ((b & 0x01) == 0)
       {
         strcpy (ratchet_state,"MAX LIMITED");
       }
       else if ((b & 0x2) == 0)
       {
         strcpy (ratchet_state,"MIN LIMITED");
       }
       else
       {
         strcpy (ratchet_state,"");
       }
       if(tx_agc == 0)
       {
           SPRINTF(temp+i,"Tx AGC dBm: %d %s\n", tx_agc, ratchet_state);
       }
       else
       {
           SPRINTF(temp+i,"Tx AGC dBm: -%d.%d %s\n", (tx_agc/10), abs(tx_agc%10), ratchet_state);
       }
       i = STRLEN(temp);
     }
     else
     {
       SPRINTF(temp+i,"Tx AGC dBm: -%d.%d\n", (tx_agc/10), abs(tx_agc%10));
       i = STRLEN(temp);
     }
   }

   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_ECIO,
                                  &b,
                                  sizeof(b))) {             
     SPRINTF(temp+i,"EC/IO: -%d\n", b);
     i = STRLEN(temp);
   }
   // Get PA State
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_PA_STATE,
                                  &b,
                                  sizeof(b))) {
     char pa_line[7];
     char pa_on_off[4];
     byte pa_data = b;

     /* 1x targets use PA_R1 0 is high gain, 1 is low gain*/

     if (pa_data & 0x10)
     {
       strcpy(pa_on_off,"ON ");

       if(pa_data & 0x02)
       {
         strcpy(pa_line,"HI PWR");
       }
       else
       {
         strcpy(pa_line,"LO PWR");
       }
     }
     else
     {
       strcpy(pa_on_off,"OFF");
       strcpy(pa_line,"");
     }

     SPRINTF(temp+i,"PA State: %s %s\n", pa_on_off, pa_line);
      i = STRLEN(temp);

     // Get HDET
     if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                    CFGI_DEBUG_HDET,
                                    &b,
                                    sizeof(b))) {

       /* HDET data is only valid when TX is ON */
       if ((pa_data & 0x10) == 0)
         b = 0;

       SPRINTF(temp+i,"HDET: %d\n", b);
       i = STRLEN(temp);
     }
   }
   // Get THERM
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_THERM,
                                  &w,
                                  sizeof(w))) {
    b = w >> 8;
      SPRINTF(temp+i,"THERM: %d (raw: %d)\n", b&0x0ff, w&0x0ff);
      i = STRLEN(temp);
   }

   // Get VBATT
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_VBATT,
                                  &b,
                                  sizeof(b))) {
      SPRINTF(temp+i,"VBATT: %d\n", b);
      i = STRLEN(temp);
   }

   // Get SID
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_SID,
                                  &w,
                                  sizeof(w))) {
      SPRINTF(temp+i,"SID: %d\n", w);
      i = STRLEN(temp);
   }

   // Get NID
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_NID,
                                  &w,
                                  sizeof(w))) {
      SPRINTF(temp+i,"NID: %d\n", w);
      i = STRLEN(temp);
   }

   // The dynamic range obtained below is 10 times the actual value
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                          CFGI_CDMA_DYNAMIC_RANGE,
                          &dyn_range,
                          sizeof(dyn_range))) {

     SPRINTF(temp+i, "Dyn Range: %d\n", dyn_range);
     i = STRLEN(temp);
   }


   // The min Rx RSSI obtained below is 10 times the actual value
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                          CFGI_CDMA_MIN_RX_RSSI,
                          &rx_rssi,
                          sizeof(rx_rssi))) {

     SPRINTF(temp+i, "Min Rx RSSI: %d\n", rx_rssi);
     i = STRLEN(temp);
   }

#if defined(T_MSM6500) && defined(T_FFA)
   // Get temperature readings. this will be reported in degrees celsius
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_THERM_BATT,
                                  &b,
                                  sizeof(b))) {
      SPRINTF(temp+i,"THERM  BATT: %dC \n", b);
      i = STRLEN(temp);
   }
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_THERM_MSM,
                                  &b,
                                  sizeof(b))) {
      SPRINTF(temp+i,"THERM   MSM: %dC \n", b);
      i = STRLEN(temp);
   }


   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_THERM_PA,
                                  &b,
                                  sizeof(b))) {
      SPRINTF(temp+i,"THERM    PA: %dC \n", b);
      i = STRLEN(temp);
   }
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_THERM_PMIC,
                                  &b,
                                  sizeof(b))) {
      SPRINTF(temp+i,"THERM  PMIC: %dC \n", b);
      i = STRLEN(temp);
   }
   if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_THERM_CHRGR,
                                  &b,
                                  sizeof(b))) {
      SPRINTF(temp+i,"THERM CHRGR: %dC \n", b);
      i = STRLEN(temp);
   }
#endif

   (void)SPRINTF(temp+i, "FldDbgVer: %s\n", FLDDBG_SW_VERSION);
   i = STRLEN(temp);
   pszBuf = (AECHAR*) MALLOC((i+1)*sizeof(AECHAR));
   if(pszBuf != NULL)
   {
     STRTOWSTR(temp, pszBuf,(i+1)*sizeof(AECHAR) );
   }
   else
   {
     MSG_FATAL("Malloc failed!", 0, 0, 0);
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
   AECHAR          sTitle[20]; 
   
   MEMSET (sTitle, 0, sizeof(sTitle));

   p_dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
   p_stk = (IStatic *) IDIALOG_GetControl(p_dlg, IDC_DBSCR_STATIC);

  ISTATIC_SetProperties(p_stk, ST_UNDERLINE|ST_NOSCROLL|ST_CENTERTITLE);
   szBuf = CFieldDebug_DebugScreenReadItem(pme);

   // get current text line from BREW
   currentLine = ISTATIC_GoToLine(p_stk, 0);
   // Set the values of the title and text strings for this control
   
   (void) ISHELL_LoadResString(pme->a.m_pIShell,
                                        FLDDBG_RES_FILE,
                                        IDS_DBGSCRN,
                                        sTitle,
                                        sizeof(sTitle));
    if(pme->m_pIAnn != NULL)
    {
        IANNUNCIATOR_SetFieldText(pme->m_pIAnn,sTitle);
    }
     
   (void) ISTATIC_SetText(p_stk,
                          NULL,
                          szBuf,
                          AEE_FONT_NORMAL,
                          AEE_FONT_NORMAL);

   FREEIF(szBuf);
   //ISTATIC_SetProperties(p_stk, ST_NOSCROLL);
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
      if (wParam == IDS_OK) {
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
     CFieldDebug_OnDialogStart (pme, wParam, dwParam);
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
#ifndef FEATURE_USES_LOWMEM
   PARAM_NOT_REF(dwParam)

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
#endif
   switch (eCode) {
   case EVT_COMMAND:
//lint -save -e764  Suppress "switch stmt has no case" because of feature defs
      switch (wParam) {
#if defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
      case IDS_TCALL_N_13K:
#ifdef FEATURE_ICM
        so = AEECM_SO_RS2_MARKOV;
#else
        so = CAI_SO_RS2_MARKOV;
#endif
        break;
      case IDS_TCALL_N_8K:
#ifdef FEATURE_ICM
        so = AEECM_SO_RS1_MARKOV;
#else
        so = CAI_SO_RS1_MARKOV;
#endif
        break;
      case IDS_TCALL_13K_LOOP:
#ifdef FEATURE_ICM
        so = AEECM_SO_LOOPBACK_13K;
#else
        so = CAI_SO_LOOPBACK_13K;
#endif
        break;
      case IDS_TCALL_8K_LOOP:
#ifdef FEATURE_ICM
        so = AEECM_SO_LOOPBACK;
#else
        so = CAI_SO_LOOPBACK;
#endif
        break;
#endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */
      default:
         return FALSE;
      }
//lint +e1783  Restore "switch statement has no case"
      // TODO - add code to make test call
#ifdef FEATURE_ICM
//lint -save -e527  Suppress "unreachable" because of feature defs
      if (ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_CALLMANAGER,
                                (void **) &pICM) != SUCCESS) {
//lint +e527
        return FALSE;
      }

      if (pICM == NULL) {
        return FALSE;
      }

      if (ISHELL_CreateInstance (pme->a.m_pIShell, AEECLSID_CALLOPTS,(void **) &pICallOpts) != SUCCESS) {
        return FALSE;
      }

      call_option.nId = AEECM_CALLOPT_SRV_OPT;
      call_option.pVal = &so;

      if (ICallOpts_AddOpt (pICallOpts, &call_option) != SUCCESS) {
        ICallOpts_Release (pICallOpts);
        ICM_Release (pICM);
        return FALSE;
      }

      call_option.nId = AEECM_CALLOPT_SERVICE_TYPE;
      call_option.pVal = &service_type;

      if (ICallOpts_AddOpt (pICallOpts, &call_option) != SUCCESS) {
        ICallOpts_Release (pICallOpts);
        ICM_Release (pICM);
        return FALSE;
      }

      if (ICM_OriginateCall (pICM, AEECM_CALL_TYPE_TEST, number, NULL, pICallOpts, &call_id) != SUCCESS) {
        ICallOpts_Release (pICallOpts);
        ICM_Release (pICM);
        return FALSE;
      }
      ICallOpts_Release (pICallOpts);
      ICM_Release (pICM);
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
     CFieldDebug_OnDialogStart (pme, wParam, dwParam);
     return TRUE;

   case EVT_DIALOG_END:
      return TRUE;

   default:
      break;
   }
#endif   
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
      if (wParam == IDS_OK) {
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
     CFieldDebug_OnDialogStart (pme, wParam, dwParam);
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
#error code not present
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
      if (wParam == IDS_YES_TXT) {
         // Show Message - Clearing. . .
         (void) ISHELL_LoadResString(pme->a.m_pIShell,
                                     FLDDBG_RES_FILE,
                                     IDS_CLEARING_TXT,
                                     szBuf,
                                     sizeof(szBuf));
         dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
         sta = (IStatic *) IDIALOG_GetControl(dlg, IDC_CLEAR_ERRLOG);
	  ISTATIC_SetProperties(sta, ST_UNDERLINE|ST_NOSCROLL);
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
      } else if ((wParam == IDS_NO_TXT) ||
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
     CFieldDebug_OnDialogStart (pme, wParam, dwParam);
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
#else
   IPhoneCtl *pIPhoneCtl = NULL;
   int nReturnStatus;
#endif

   switch (eCode) {
   case EVT_COMMAND:
      if (wParam == IDS_OK) {
         // Show Message - Clearing. . .
         (void) ISHELL_LoadResString(pme->a.m_pIShell,
                                     FLDDBG_RES_FILE,
                                     IDS_CLEARING_TXT,
                                     szBuf,
                                     sizeof(szBuf));

         dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
         txt = (ITextCtl *) IDIALOG_GetControl(dlg, IDC_CLREFS_TEXT);
         (void) ITEXTCTL_GetText(txt, input_txt, 16);
         STRTOWSTR("Yes", yes_txt, sizeof(yes_txt));
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
#else
         /* Create the Call Manager object. */
         nReturnStatus = ISHELL_CreateInstance(pme->a.m_pIShell,
                                    AEECLSID_PHONECTL,
                                    (void **) &pIPhoneCtl);

         if(nReturnStatus != SUCCESS) {
           return FALSE;
         }

         if (pIPhoneCtl == NULL) { /* error */
           return FALSE;
         }
#endif
         if (!(WSTRICMP(input_txt, yes_txt))) { // if the user types Yes
#ifdef FEATURE_ICM
           ICM_SetOperatingMode(pICM, AEECM_OPRT_MODE_OFFLINE);
#else
           IPHONECTL_SetOperatingMode(pIPhoneCtl, AEET_OPRT_MODE_OFFLINE);
#endif
            // TODO - do something here
#ifndef FEATURE_EFS2
            fs_master_reset();
#else
          efs_reset();
#endif
#ifdef FEATURE_ICM
           ICM_SetOperatingMode(pICM, AEECM_OPRT_MODE_RESET);
#else
           IPHONECTL_SetOperatingMode(pIPhoneCtl, AEET_OPRT_MODE_RESET);
#endif
         } else {
            (void) CFieldDebug_MoveToDialog(pme, IDD_DEBUG_DIALOG);
         }
#ifdef FEATURE_ICM
         ICM_Release(pICM);
         pICM = NULL;
#else
         IPHONECTL_Release(pIPhoneCtl);
         pIPhoneCtl = NULL;
#endif
         return TRUE;
      } else if (wParam == IDS_CANCEL ) {
         (void) CFieldDebug_MoveToDialog(pme, IDD_DEBUG_DIALOG);
         return TRUE;
      }

      return FALSE;

   case EVT_KEY:
      switch (wParam) 
	{
        case AVK_SELECT:
         {
         IDialog *pd;

         pd = ISHELL_GetActiveDialog(pme->a.m_pIShell);
         (void) IDIALOG_SetFocus(pd, IDC_CLREFS_SOFTKEY);
   	  }
      return TRUE;
	  
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
     CFieldDebug_OnDialogStart (pme, wParam, dwParam);
     return TRUE;

   case EVT_DIALOG_END:
      return TRUE;

   default:
      break;
   }
   return FALSE;
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
     CFieldDebug_OnDialogStart (pme, wParam, dwParam);
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
      case IDS_SMSMO_SO_6:
         mo_so = OEMNV_SMS_SO_SO6;
         break;

      case IDS_SMSMO_SO_14:
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
     CFieldDebug_OnDialogStart (pme, wParam, dwParam);
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
      case IDS_VP_STANDARD:
     //    vp = OEMNV_VP_STANDARD;
         break;

      case IDS_VP_ENHANCED:
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
     CFieldDebug_OnDialogStart (pme, wParam, dwParam);
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

   switch (eCode) {
   case EVT_COMMAND:
      switch (wParam) {
        case IDS_MMC_DOWNLOAD:
          (void) CFieldDebug_MoveToDialog(pme, IDD_MMC_DOWNLOAD_DIALOG);
          break;

        case IDS_MMC_FORMAT:
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

   case EVT_DIALOG_START:
     CFieldDebug_OnDialogStart (pme, wParam, dwParam);
     return TRUE;

   case EVT_DIALOG_END:
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
      CFieldDebug_OnDialogStart (pme, wParam, dwParam);
      //Remove the Quit Softkey.
      {
        IMenuCtl *pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl((IDialog*) dwParam, IDC_SK_MMC_DOWNLOAD);
       if(pIMenuCtl != NULL)
       {
          IMENUCTL_DeleteAll(pIMenuCtl);
       }
      }
      //Start Init Timer
      ISHELL_SetTimer(pme->a.m_pIShell,100, CFieldDebug_InitMMCDownload, (void*) pme);
      pme->bEndKeyProcess = TRUE;
      return TRUE;

    case EVT_USER:
      switch(wParam)
      {
      case EVT_RDM_STATUS:
        if( dwParam == RDM_DONE_S)
        {
          switch(pme->rdmState)
          {
          case RDM_STATUS_INIT1:
            pme->rdmState = RDM_STATUS_INIT2;
            pme->old_port_map1 = rdm_get_device (RDM_DIAG_SRVC);
            if (pme->old_port_map1 == RDM_USB_DIAG_DEV)
            {
              rdm_assign_port (RDM_DIAG_SRVC, RDM_NULL_DEV,
                            CFieldDebug_RDMCb);
            }
            else
            {
              //Fake the RDM event to go to next state.
              ISHELL_PostEventEx(pme->a.m_pIShell, 0, AEECLSID_FIELDDEBUGAPP, EVT_USER,
                EVT_RDM_STATUS, RDM_DONE_S);
            }
            break;

          case RDM_STATUS_INIT2:
            pme->rdmState = RDM_STATUS_INIT3;
#ifdef FEATURE_USB_DIAG_NMEA
            pme->old_port_map2 = rdm_get_device (RDM_NMEA_SRVC);
            if (pme->old_port_map1 == RDM_USB_NMEA_DEV)
            {
              rdm_assign_port (RDM_NMEA_SRVC, RDM_NULL_DEV,
                            CFieldDebug_RDMCb);
            }
            else
#endif
            {
              //Fake the RDM event to go to next state.
              ISHELL_PostEventEx(pme->a.m_pIShell, 0, AEECLSID_FIELDDEBUGAPP, EVT_USER,
                EVT_RDM_STATUS, RDM_DONE_S);
            }
            break;

          case RDM_STATUS_INIT3:
            pme->rdmState = RDM_STATUS_INIT4;
            rdm_assign_port (RDM_MMC_SRVC, RDM_USB_MMC_DEV,
                CFieldDebug_RDMCb);
            break;

          case RDM_STATUS_INIT4:
#if (defined(FEATURE_USB) && defined(FEATURE_MMC)) || defined (FEATURE_USB_MASS_STORAGE)
			pme->nCount1 = pme->nCount2 = usbsdms_get_write_blk_count ();
#else
            pme->nCount1 = pme->nCount2 = 0;
#endif
            ISHELL_SetTimer(pme->a.m_pIShell, 1000, CFieldDebug_MMCDownload, (void*) pme);
            break;

          case RDM_STATUS_EXIT1:
            pme->rdmState = RDM_STATUS_EXIT2;
            if(pme->old_port_map != RDM_NULL_DEV)
            {
              rdm_assign_port (RDM_DATA_SRVC, pme->old_port_map, CFieldDebug_RDMCb);
            }
            else
            {
              ISHELL_PostEventEx(pme->a.m_pIShell, 0, AEECLSID_FIELDDEBUGAPP, EVT_USER,
                EVT_RDM_STATUS, RDM_DONE_S);
            }
            pme->old_port_map = RDM_NULL_DEV;
            break;

           case RDM_STATUS_EXIT2:
#ifdef FEATURE_USB_DIAG_NMEA
            pme->rdmState = RDM_STATUS_EXIT3;
#else
            pme->rdmState = RDM_STATUS_EXIT4;
#endif
            if(pme->old_port_map1 != RDM_NULL_DEV)
            {
              rdm_assign_port (RDM_DIAG_SRVC, pme->old_port_map1, CFieldDebug_RDMCb);
            }
            else
            {
              ISHELL_PostEventEx(pme->a.m_pIShell, 0, AEECLSID_FIELDDEBUGAPP, EVT_USER,
                EVT_RDM_STATUS, RDM_DONE_S);
            }
            pme->old_port_map1 = RDM_NULL_DEV;
            break;

          case RDM_STATUS_EXIT3:
#ifdef FEATURE_USB_DIAG_NMEA
            pme->rdmState = RDM_STATUS_EXIT4;
            if(pme->old_port_map2 != RDM_NULL_DEV)
            {
              rdm_assign_port (RDM_NMEA_SRVC, pme->old_port_map2, CFieldDebug_RDMCb);
            }
            else
            {
              ISHELL_PostEventEx(pme->a.m_pIShell, 0, AEECLSID_FIELDDEBUGAPP, EVT_USER,
                EVT_RDM_STATUS, RDM_DONE_S);
            }
            pme->old_port_map2 = RDM_NULL_DEV;

#else
            MSG_FATAL("Non NMEA state reached",0, 0, 0);
#endif
            break;

          case RDM_STATUS_EXIT4:
            pme->nCount1 = pme->nCount2 = 0;
            ISHELL_EndDialog(pme->a.m_pIShell);

            if(ISHELL_GetActiveDialog(pme->a.m_pIShell) == NULL)
            {
              pme->m_dlgID = IDD_TOP_DIALOG;
              ISHELL_CreateDialog(pme->a.m_pIShell,
                              FLDDBG_RES_FILE,
                              pme->m_dlgID,
                              NULL);
              return TRUE;
            }
            break;

          default:
            MSG_FATAL("Unhandled RDM Status state", 0, 0, 0);
            break;

          }
        }
        else
        {
          //Error
          MSG_FATAL("RDM status error", 0, 0, 0);
          ISHELL_EndDialog(pme->a.m_pIShell);
          ISHELL_CreateDialog(pme->a.m_pIShell,
            FLDDBG_RES_FILE, IDD_ERROR_DIALOG, NULL);
        }
        return TRUE;

      default:
        MSG_FATAL("Unidentified EVT_USER:%d", wParam, 0, 0);
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
        case IDS_QUIT:
          CFieldDebug_EndMMCDownload(pme);
          return TRUE;
        default:
          break;
      }
      break;
    case EVT_DIALOG_END:
        pme->bEndKeyProcess = FALSE;
        return TRUE;

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
static boolean CFieldDebug_MMCFormatHandleEvent(CFieldDebug * pMe,
                                           AEEEvent eCode,
                                           uint16 wParam,
                                           uint32 dwParam)
{
  fs_rsp_msg_type rsp;
  IStatic *pStatic;
  IDialog *pDlg;
  AECHAR buf1[20], buf2[20];
  AECHAR  sTitle[20];

   MEMSET (sTitle, 0, sizeof(sTitle));
   PARAM_NOT_REF(dwParam)
   switch (eCode) {
    case EVT_DIALOG_START:
      CFieldDebug_OnDialogStart (pMe, wParam, dwParam);
      pDlg = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      pStatic = (IStatic *) IDIALOG_GetControl (pDlg, IDC_STATIC_MMC_FORMAT);
      ISTATIC_SetProperties(pStatic, ST_UNDERLINE|ST_NOSCROLL|ST_CENTERTITLE);
      ISHELL_LoadResString (pMe->a.m_pIShell, FLDDBG_RES_FILE, IDS_MMC_FORMAT, buf1,
                            sizeof(AECHAR)*ARR_SIZE(buf1));
      ISHELL_LoadResString (pMe->a.m_pIShell, FLDDBG_RES_FILE, IDS_FORMATTING, buf2,
                            sizeof(AECHAR)*ARR_SIZE(buf2));
      ISTATIC_SetText (pStatic, NULL, buf2, AEE_FONT_NORMAL, AEE_FONT_NORMAL);
      fs_format ("mmc1", NULL, &rsp);
      if (rsp.format.status == FS_OKAY_S) {
        ISHELL_LoadResString (pMe->a.m_pIShell, FLDDBG_RES_FILE, IDS_SUCCESSFUL, buf2,
                            sizeof(AECHAR)*ARR_SIZE(buf2));
      }
      else if (rsp.format.status == FS_NO_MEDIA_S) {
        ISHELL_LoadResString (pMe->a.m_pIShell, FLDDBG_RES_FILE, IDS_NO_CARD, buf2,
                            sizeof(AECHAR)*ARR_SIZE(buf2));
      }
      else {
       ISHELL_LoadResString (pMe->a.m_pIShell, FLDDBG_RES_FILE, IDS_FAILED, buf2,
                            sizeof(AECHAR)*ARR_SIZE(buf2));
      }
      if(pMe->m_pIAnn != NULL)
      {
          IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,buf1);
      }      
      ISTATIC_SetText (pStatic, NULL, buf2, AEE_FONT_NORMAL, AEE_FONT_NORMAL);
	
      return TRUE;
   case EVT_KEY:
      switch(wParam)
      {
      case AVK_CLR:
        
	 (void) CFieldDebug_MoveToDialog(pMe, IDD_MMC_DIALOG);
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
      case IDS_CONTACTIMPORT:
//         (void) ISHELL_StartApplet(pme->a.m_pIShell,
//                                   AEECLSID_APP_CONTACT_IMPORTER);
         return TRUE;

      case IDS_CONTACTEXPORT:
//         (void) ISHELL_StartApplet(pme->a.m_pIShell,
//                                   AEECLSID_APP_CONTACT_EXPORTER);
         return TRUE;

#ifdef FEATURE_APP_GIGAMON
      case IDS_GIGATEST:
         (void) ISHELL_StartApplet(pme->a.m_pIShell,
                                   AEECLSID_GIGA_TEST_APP);
         return TRUE;
#endif /* FEATURE_APP_GIGAMON*/

#ifdef FEATURE_GRAPHICS_APP_CALMON
      case IDS_CALTEST:
         (void) ISHELL_StartApplet(pme->a.m_pIShell,
                                   AEECLSID_CAL_TEST_APP);
         return TRUE;
#endif /* FEATURE_GRAPHICS_APP_CALMON*/


      case IDS_DBGSCRN:
      	 {
            IMenuCtl *ctl;
            IDialog *dlg;

            dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
            ctl = (IMenuCtl *) IDIALOG_GetControl(dlg, IDC_DEBUG_MENU);
            pme->m_debugmenu_sel = IMENUCTL_GetSel(ctl);
         }
         (void) CFieldDebug_MoveToDialog(pme, IDD_DEBUG_SCREEN_DIALOG);
         return TRUE;
         
      case IDS_TEST_CALL:
      	  {
            IMenuCtl *ctl;
            IDialog *dlg;

            dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
            ctl = (IMenuCtl *) IDIALOG_GetControl(dlg, IDC_DEBUG_MENU);
            pme->m_debugmenu_sel = IMENUCTL_GetSel(ctl);
         }
         (void) CFieldDebug_MoveToDialog(pme, IDD_CALL_LIST_DIALOG);
         return TRUE;
         
      case IDS_ERRORS:
      	  {
            IMenuCtl *ctl;
            IDialog *dlg;

            dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
            ctl = (IMenuCtl *) IDIALOG_GetControl(dlg, IDC_DEBUG_MENU);
            pme->m_debugmenu_sel = IMENUCTL_GetSel(ctl);
         }
         (void) CFieldDebug_MoveToDialog(pme, IDD_ERROR_LIST_DIALOG);
         return TRUE;      	

      case IDS_CLR_ERR:
      	  {
            IMenuCtl *ctl;
            IDialog *dlg;

            dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
            ctl = (IMenuCtl *) IDIALOG_GetControl(dlg, IDC_DEBUG_MENU);
            pme->m_debugmenu_sel = IMENUCTL_GetSel(ctl);
         }
         (void) CFieldDebug_MoveToDialog(pme, IDD_CLEAR_ERROR_DIALOG);
         return TRUE; 
               	
      case IDS_CLR_EFS:
      	  {
            IMenuCtl *ctl;
            IDialog *dlg;

            dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
            ctl = (IMenuCtl *) IDIALOG_GetControl(dlg, IDC_DEBUG_MENU);
            pme->m_debugmenu_sel = IMENUCTL_GetSel(ctl);
         }
         (void) CFieldDebug_MoveToDialog(pme, IDD_CLEAR_EFS_DIALOG);
         return TRUE;   
             	
      case IDS_DATA_DM:
      	  {
            IMenuCtl *ctl;
            IDialog *dlg;

            dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
            ctl = (IMenuCtl *) IDIALOG_GetControl(dlg, IDC_DEBUG_MENU);
            pme->m_debugmenu_sel = IMENUCTL_GetSel(ctl);
         }
         (void) CFieldDebug_MoveToDialog(pme, IDD_DM_DIALOG);
         return TRUE;   
               	
      case IDS_SMS_MO_SO:
      	  {
            IMenuCtl *ctl;
            IDialog *dlg;

            dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
            ctl = (IMenuCtl *) IDIALOG_GetControl(dlg, IDC_DEBUG_MENU);
            pme->m_debugmenu_sel = IMENUCTL_GetSel(ctl);
         }
         (void) CFieldDebug_MoveToDialog(pme, IDD_SMS_MO_SO_DIALOG);
         return TRUE;   
               	      	
      case IDS_VOICE_PRIVACY:
      	  {
            IMenuCtl *ctl;
            IDialog *dlg;

            dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
            ctl = (IMenuCtl *) IDIALOG_GetControl(dlg, IDC_DEBUG_MENU);
            pme->m_debugmenu_sel = IMENUCTL_GetSel(ctl);
          }
         (void) CFieldDebug_MoveToDialog(pme, IDD_VOICE_PRIVACY_DIALOG);
         return TRUE;    
              	
      case IDS_GIGATEST:
           return TRUE;  
               	
      case IDS_HDR_INFO:
           return TRUE; 
#if defined(FEATURE_GSM) || defined (FEATURE_WCDMA)
#error code not present
#endif

      case IDS_SECURITY_SERVICES:
        return CheckAEEReturnStatus(ISHELL_CreateDialog(pme->a.m_pIShell,
            FLDDBG_RES_FILE, IDD_SECURITY_SERVICES, NULL));
      case IDS_MMC:
	  {
            IMenuCtl *ctl;
            IDialog *dlg;

            dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
            ctl = (IMenuCtl *) IDIALOG_GetControl(dlg, IDC_DEBUG_MENU);
            pme->m_debugmenu_sel = IMENUCTL_GetSel(ctl);
         }
        (void) CFieldDebug_MoveToDialog (pme, IDD_MMC_DIALOG);
        return TRUE;
      case IDS_CLEAR_FPLMN:
	  	 
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
#error code not present
#endif /* #if defined(FEATURE_GSM) || defined (FEATURE_WCDMA) */

      default:
         break;
      }

      break;

   case EVT_KEY:
      switch (wParam) {
      case AVK_CLR:
         pme->m_debugmenu_sel = IDS_DBGSCRN;
         (void) CFieldDebug_MoveToDialog(pme, IDD_TOP_DIALOG);
         return TRUE;

      default:
         break;
      }
      break;

   case EVT_DIALOG_START:
     CFieldDebug_OnDialogStart (pme, wParam, dwParam);
      {
         IMenuCtl *ctl;
         IDialog *dlg;

         dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
         ctl = (IMenuCtl *) IDIALOG_GetControl(dlg, IDC_DEBUG_MENU);
         // delete the Data/DM menu if there is no data support
         (void) IMENUCTL_DeleteItem(ctl, IDS_DATA_DM);
         (void) IMENUCTL_DeleteItem(ctl, IDS_SMS_MO_SO);
         (void) IMENUCTL_DeleteItem(ctl, IDS_GIGATEST);
         (void) IMENUCTL_DeleteItem(ctl, IDS_CONTACTIMPORT);
         (void) IMENUCTL_DeleteItem(ctl, IDS_CONTACTEXPORT);
         (void) IMENUCTL_DeleteItem(ctl, IDS_VOICE_PRIVACY);
#ifndef FEATURE_APP_GIGAMON
         (void) IMENUCTL_DeleteItem(ctl, IDS_GIGATEST);
#endif
#ifndef FEATURE_GRAPHICS_APP_CALMON
         (void) IMENUCTL_DeleteItem(ctl, IDS_CALTEST);
#endif
#ifndef FEATURE_HDR
         (void) IMENUCTL_DeleteItem(ctl, IDS_HDR_INFO);
#endif
#ifndef FEATURE_MMGSDI
         (void) IMENUCTL_DeleteItem(ctl, IDS_CLEAR_FPLMN);
#endif
#if !defined(FEATURE_GSM) && !defined(FEATURE_WCDMA)
         (void) IMENUCTL_DeleteItem(ctl, IDS_AMR_PREF);
#endif

#if !defined(FEATURE_SEC_TESTAPPS) && !defined(FEATURE_SEC_IPSEC_VPN_CLNT)
         (void)  IMENUCTL_DeleteItem(ctl, IDS_SECURITY_SERVICES);
#endif
#ifndef FEATURE_VOICE_FIXED_PATTERN_DEBUG
         (void) IMENUCTL_DeleteItem(ctl, IDS_VOICE_DEBUG);
#endif /* FEATURE_VOICE_FIXED_PATTERN_DEBUG */
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

static boolean CFieldDebug_NVSETTINGHandleEvent(CFieldDebug *pme,
                                               AEEEvent  eCode,
                                               uint16    wParam,
                                               uint32    dwParam)
{
       ITextCtl *pIText = NULL; 
       ITextCtl *pIText2 = NULL; 
       IStatic  *pStatic1 = NULL;
       IStatic  *pStatic2 = NULL;
       IDialog * p_diag = NULL;
       BottomBar_Param_type BarParam={0};
       PARAM_NOT_REF(dwParam)
       
       MSG_FATAL("CFieldDebug_NVSETTINGHandleEvent wParam=%x, eCode=%d",wParam, eCode, 0);
       switch (eCode) {   
       case EVT_KEY_RELEASE:
       {
          MSG_FATAL("EVT_CTL_TAB",0,0,0);
          if((wParam == AVK_UP) || (wParam == AVK_DOWN))
          {
              p_diag = ISHELL_GetActiveDialog(pme->a.m_pIShell);          
              pIText = (ITextCtl*)IDIALOG_GetControl(p_diag, IDC_NVID_TEXT);
              pIText2 = (ITextCtl*)IDIALOG_GetControl(p_diag, IDC_NVVALUE_TEXT);          
              if(pme->m_TextId == 1)
              {
                  MSG_FATAL("go to 21",0,0,0);
                  pme->m_TextId = 2;
                  MSG_FATAL("go to 22",0,0,0);
                  if(pIText == NULL)
                  {
                    MSG_FATAL("pIText == NULL",0,0,0);
                  }
                  if(pIText2 == NULL)
                  {
                    MSG_FATAL("pIText2 == NULL",0,0,0);
                  }
                  ITEXTCTL_SetActive(pIText, FALSE);
                  ITEXTCTL_SetActive(pIText2, TRUE); 
                  MSG_FATAL("go to 23",0,0,0);
              }
              else if(pme->m_TextId == 2)
              {
                  MSG_FATAL("go to 11",0,0,0);
                  pme->m_TextId = 1;
                  ITEXTCTL_SetActive(pIText, TRUE);
                  ITEXTCTL_SetActive(pIText2, FALSE); 
                  MSG_FATAL("go to 11",0,0,0);
              }
              IDISPLAY_UpdateEx(pme->m_pDisplay, TRUE);       
              return TRUE;   
          }
          return FALSE;
       }
       case EVT_KEY:
          switch (wParam) {
    
          case AVK_CLR:
             (void) CFieldDebug_MoveToDialog(pme, IDD_TOP_DIALOG);
             return TRUE;       
          case AVK_SELECT:
          case AVK_INFO:
            {
                AECHAR pwstrTextID[5] = {0};
                AECHAR pwstrTextValue[51] = {0};
                char string[10];
                uint32 ID = 0;
                char value[100];
                int valueLen = 0;
                int i,j=0;
                char temp[4];
                byte NvValue[50]={0};
                nv_stat_enum_type status;
                p_diag = ISHELL_GetActiveDialog(pme->a.m_pIShell);
                pIText = (ITextCtl*)IDIALOG_GetControl(p_diag, IDC_NVID_TEXT);
                pIText2 = (ITextCtl*)IDIALOG_GetControl(p_diag, IDC_NVVALUE_TEXT);  
                
                ITEXTCTL_SetActive(pIText,TRUE);
                ITEXTCTL_SetActive(pIText2,FALSE);
                (void)ITEXTCTL_GetText(pIText,pwstrTextID,5);
                (void) WSTRTOSTR(pwstrTextID, string, sizeof(string));
                ID= ATOI(string);

                ITEXTCTL_SetActive(pIText,FALSE);
                ITEXTCTL_SetActive(pIText2,TRUE);
                (void)ITEXTCTL_GetText(pIText2,pwstrTextValue,50);
                (void) WSTRTOSTR(pwstrTextValue, value, sizeof(value));
                MSG_FATAL("id=%d ",ID,0,0);
                DBGPRINTF ("value=%s", value);
                valueLen = STRLEN(value);
                for(i = 0; i < valueLen; )
                {
                    STRLCPY(temp, value+i, 4);
                    i += 3;
                    DBGPRINTF ("temp=%s", temp);
                    NvValue[j++] = ATOI(temp);
                    MSG_FATAL("NvValue[%d]=%d",j-1,ATOI(temp),0);
                }
                status = nvio_write_item((nv_items_enum_type)ID,  /* Item name */
                          0, /* index */
                          (void*)NvValue, /* data ptr */
                          nvim_op_get_size((nv_items_enum_type)ID));  // data count 这里可以通过nvim_op_get_size来测试NV项的大小                
                MSG_FATAL("nvio_write_item status=%d",status,0,0);
                (void) CFieldDebug_MoveToDialog(pme, IDD_TOP_DIALOG);
                return TRUE;        
             }
             break;
             
          default:
             break;
          }
          return FALSE;
    
       case EVT_DIALOG_START:
            {
                char szBuf[64] = {0};
                pme->m_CurrId =(uint32)NVSETTING;
                pme->m_pActiveIDlg = (IDialog*)dwParam;
                (void) CFieldDebug_OnDialogStart (pme, wParam, dwParam);
                pIText = (ITextCtl*)IDIALOG_GetControl((IDialog *) dwParam, IDC_NVID_TEXT);
                pIText2 = (ITextCtl*)IDIALOG_GetControl((IDialog *) dwParam, IDC_NVVALUE_TEXT);
                pStatic1 = (IStatic *) IDIALOG_GetControl((IDialog *) dwParam, IDC_NVID_STATIC);
                pStatic2 = (IStatic *) IDIALOG_GetControl((IDialog *) dwParam, IDC_NVVALUE_STATIC);
                ITEXTCTL_SetProperties(pIText, TP_FRAME|TP_FOCUS_NOSEL|TP_FIXSETRECT);
                ITEXTCTL_SetProperties(pIText2, TP_STARKEY_SWITCH |TP_FRAME|TP_FOCUS_NOSEL|TP_FIXSETRECT);
                ITEXTCTL_SetCursorPos(pIText, TC_CURSOREND);
                ITEXTCTL_SetCursorPos(pIText2, TC_CURSOREND);     
                (void) ITEXTCTL_SetInputMode(pIText, AEE_TM_NUMBERS);
                {
                    AEERect ctlRect;
                    SETAEERECT(&ctlRect,  0, 0,
                                pme->m_screen_rc.dx,
                                20);   
                    ICONTROL_SetRect((IControl*)pStatic1, &ctlRect);  
                    
                    SETAEERECT(&ctlRect,  0, 20,
                                pme->m_screen_rc.dx,
                                20);
                    ICONTROL_SetRect((IControl*)pIText, &ctlRect);   
                    
                    SETAEERECT(&ctlRect,  0, 40,
                                pme->m_screen_rc.dx,
                                20);   
                    ICONTROL_SetRect((IControl*)pStatic2, &ctlRect);                      
                     
                    SETAEERECT(&ctlRect,  0, 60,
                                pme->m_screen_rc.dx,
                                20);
                    ICONTROL_SetRect((IControl*)pIText2, &ctlRect); 
                    ITEXTCTL_SetMaxSize ( pIText, 4);
                    ITEXTCTL_SetMaxSize ( pIText2, 50);
                    ITEXTCTL_SetActive(pIText, TRUE);
                    ITEXTCTL_SetActive(pIText2, FALSE);
                    pme->m_TextId = 1;
                }
                if (NULL != szBuf)
                {
                    {
                        AECHAR WTitle[20] = {0};
                        (void)ISHELL_LoadResString(pme->a.m_pIShell,
                        FLDDBG_RES_FILE,                                
                        IDS_NV_SETTING,
                        WTitle,
                        sizeof(WTitle));
                        if(pme->m_pIAnn != NULL)
                        {
                            IANNUNCIATOR_SetFieldText(pme->m_pIAnn,WTitle);
                        }
                    }                           
                }  
                IDISPLAY_UpdateEx(pme->m_pDisplay, TRUE);
                (void) ISHELL_PostEventEx(pme->a.m_pIShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_FIELDDEBUGAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
        }
          
          return TRUE;
       case EVT_USER_REDRAW:
            {              
                #ifndef FEATURE_ALL_KEY_PAD
                // Option     Delete
                BarParam.eBBarType = BTBAR_OK_DELETE;
                #else
                // Option     Back
                BarParam.eBBarType = BTBAR_OK_BACK;
                #endif
                DrawBottomBar(pme->m_pDisplay,&BarParam);
                IDISPLAY_Update(pme->m_pDisplay);  
            }
            break;
       case EVT_DIALOG_END:
          return TRUE;
    
       default:
          break;
       }
       MSG_FATAL("CFieldDebug_NVSETTINGHandleEvent End", 0, 0, 0);
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
      if (wParam == IDS_PROGRAM_TITLE) {
         // Start the service programming applet
         IANNUNCIATOR_Redraw(pme->m_pIAnn);
         return(SUCCESS == ISHELL_StartApplet(pme->a.m_pIShell,
                                              AEECLSID_SVCPRGAPP));
      }

      if (wParam == IDS_DEBUG_TITLE)
      {
         (void) CFieldDebug_MoveToDialog(pme, IDD_DEBUG_DIALOG);//进入DEBUG菜单
         return TRUE;
      }

	  if (wParam == IDS_NV_SETTING)     //ADD BY YANGDECAI
	  {
	  	 MSG_FATAL ("CFieldDebug_TopMenuHandleEvent wParam == IDS_NV_SETTING", 0, 0, 0);
	  	(void) CFieldDebug_MoveToDialog(pme, IDD_NVSETTING_DIALOG);//进入BREWSETING 菜单
         return TRUE;
	  }
      if (wParam == IDS_VERSION_TITLE)
      {
         (void) CFieldDebug_MoveToDialog(pme, IDD_VERSION_DIALOG);
         return TRUE;
      }
      if (wParam == IDS_ESN)
      {
         MSG_FATAL ("CFieldDebug_TopMenuHandleEvent wParam == IDS_ESN", 0, 0, 0);
         (void) CFieldDebug_MoveToDialog(pme, IDD_ESN_DIALOG);
         return TRUE;
      }      
      if (wParam == IDS_RATS_TITLE)
      {
         (void) CFieldDebug_MoveToDialog(pme, IDD_RATS_DIALOG);
         return TRUE;
      }
	  if (wParam == IDS_STRING_NET_LOCK)     //ADD BY YANGDECAI
	  {
	  	 MSG_FATAL ("CFieldDebug_TopMenuHandleEvent wParam == IDS_BREWSETTUBG_TITLE", 0, 0, 0);
	  	(void) CFieldDebug_MoveToDialog(pme, IDD_NET_LOCK_DIALOG);//进入BREWSETING 菜单
         return TRUE;
	  }    
      if (wParam == IDS_FACTORY_COUNT)     
	  {
	  	 MSG_FATAL ("CFieldDebug_TopMenuHandleEvent wParam == IDS_FACTORY_COUNT", 0, 0, 0);
	  	(void) CFieldDebug_MoveToDialog(pme, IDD_FACTORY_COUNT);//进入BREWSETING 菜单
         return TRUE;
	  } 

      //Create JPEG Dialog
      if (wParam == IDS_JPEG_DECODE)
      {
    #ifdef FEATURE_JPEG_DECODER
        ISHELL_EndDialog(pme->a.m_pIShell); //End current dialog.
        ISHELL_CreateDialog(pme->a.m_pIShell, FLDDBG_RES_FILE, IDD_JPEG_DECODE, NULL); //Show JPEG dialog.
    #endif /* FEATURE_JPEG_DECODER */
        return TRUE;
      }
      return FALSE;

   case EVT_KEY:
      switch (wParam) {

#if defined(FEATURE_APP_QUICKTEST)
      case AVK_STAR:
         (void) ISHELL_StartApplet(pme->a.m_pIShell,
                                   AEECLSID_QUICKTEST);
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
           ICM_GetCallInfo(pICM,
                         0, /* TODO, put in the correct call id */
                         ci,
                         sizeof(AEECMCallInfo));
           ci->call_state = AEECM_CALL_STATE_IDLE;
         }
         else {
           return FALSE;
         }
         ICM_Release(pICM);
         pICM = NULL;

         /* Disable the Remote API Test if the object didnt get created. */
         if (pme->m_pIRats == NULL)
         {
           (void) IMENUCTL_DeleteItem(ctl, IDS_RATS_TITLE);
         }

         // Not allowed into Service Programming while in a call.
         if (ci != NULL && ci->call_state != AEECM_CALL_STATE_IDLE )
         {
            (void) IMENUCTL_DeleteItem(ctl, IDS_PROGRAM_TITLE);
         }
         if (ci != NULL)
         {
           FREE(ci);
		   ci = NULL;
         }
#endif
         // delete jpegd menu
         (void) IMENUCTL_DeleteItem(ctl, IDS_JPEG_DECODE);
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
      (void) WSTRCAT(title_string, spacer_string);
      (void) WSTRCAT(title_string, ip_address_str);
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

        case IDS_SET_IP:
          (void) CFieldDebug_MoveToDialog(pme, IDD_RATS_IP_DIALOG);
          break;

        case IDS_RUN_TESTS:
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

        /*lint -e613*/
        IDIALOG_Redraw(p_dialog);
        /*lint +e613*/
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
          {
            AECHAR WTitle[40] = {0};
            (void)ISHELL_LoadResString(pme->a.m_pIShell,
            FLDDBG_RES_FILE,                                
            IDS_ENTER_TEST,
            WTitle,
            sizeof(WTitle));
            if(pme->m_pIAnn != NULL)
            {
                IANNUNCIATOR_SetFieldText(pme->m_pIAnn,WTitle);
            }
          }           
        rect.x = 0;
        rect.y = 0;
        rect.dx = pme->m_screen_rc.dx;
        rect.dy = (int16) (font_size * 3);  /* Box, Title, Number */
        ITEXTCTL_SetRect(ptext, &rect);
        ITEXTCTL_SetProperties(ptext, TP_FRAME|TP_FOCUS_NOSEL);
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
  uint32 new_ip;
  int    position;

  if ((pme == NULL) || (pme->m_pIRats == NULL))
    return FALSE;

  switch (eCode) {
    case EVT_COMMAND:
      p_dialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
      ptext = (ITextCtl *) IDIALOG_GetControl (p_dialog, IDC_RATS_IP_TEXT);

      switch (wParam)
      {
        case IDS_OK:
          if (ITEXTCTL_GetText(ptext, IP_buffer, MAX_IP_SIZE+1))
          {
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
              (void) CFieldDebug_MoveToDialog(pme, IDD_RATS_DIALOG);
              return TRUE;
            }
          }
          return FALSE;

        case IDS_CANCEL:
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
          {
            AECHAR WTitle[40] = {0};
            (void)ISHELL_LoadResString(pme->a.m_pIShell,
            FLDDBG_RES_FILE,                                
            IDS_SET_IP,
            WTitle,
            sizeof(WTitle));
            if(pme->m_pIAnn != NULL)
            {
                IANNUNCIATOR_SetFieldText(pme->m_pIAnn,WTitle);
            }
          }           
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
        (void) IMENUCTL_AddItem(psk, FLDDBG_RES_FILE, IDS_OK, IDS_OK,
                                res_string, IDS_OK);
        (void) ISHELL_LoadResString(pme->a.m_pIShell, FLDDBG_RES_FILE,
                                    IDS_CANCEL, res_string, sizeof(res_string));
        (void) IMENUCTL_AddItem(psk, FLDDBG_RES_FILE, IDS_CANCEL,
                            IDS_CANCEL, res_string, IDS_CANCEL);
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
   uint16 id = IDIALOG_GetID(dlg);

   if (0 == id) {
      ERR("Unknown dialog id", 0, 0, 0);
      return FALSE;
   }

   id--;
   if (id >= ARR_SIZE(sDialogEventHandlers)) {
      ERR("Unknown dialog id", 0, 0, 0);
      return FALSE;
   }
   
	
   IDIALOG_SetEventHandler(dlg,
                           (PFNAEEEVENT) sDialogEventHandlers[id],
                           (void *) pme);

   IDIALOG_SetProperties(dlg,
                         IDIALOG_GetProperties(dlg) |
                         DLG_HANDLE_ALL_EVENTS);

   return TRUE;
}

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATAION

/*===========================================================================

FUNCTION CFieldDebug_HandleEvent

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATAION
static boolean CFieldDebug_HandleEvent(CFieldDebug  *pme,
                                       AEEEvent   eCode,
                                       uint16     wParam,
                                       uint32     dwParam)
{
   PARAM_NOT_REF(wParam)
   IShell *ps = pme->a.m_pIShell;
   AEEDeviceInfo DeviceInfo;
   IDialog *pIDialog;
   AEEAppStart *args = (AEEAppStart*)dwParam;
#ifdef FEATURE_MMGSDI
   gsdi_cnf_T *pGSDIConf;
   int dummy_rec_ptr;
   uint32 client_ref = 0;
   byte                         data[GSDI_MAX_DATA_BLOCK_LEN];
   gsdi_returns_T gsdi_status = GSDI_ERROR;
   boolean bGSDIError = FALSE;
   gsdi_file_attributes_T fplmn_attributes;
   uint16 wDlgID;
#endif /* FEATURE_MMGSDI */
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif

   if(ps == NULL)
   {
     MSG_FATAL("IShell Null", 0, 0, 0);
     return FALSE;
   }

   pIDialog = ISHELL_GetActiveDialog(ps);

   switch (eCode) {

   case EVT_APP_START:
/*     if (FALSE == pme->m_bSuspended)
     {
       
       if (ISHELL_CloseApplet(pme->a.m_pIShell, FALSE) != SUCCESS)
       {
         MSG_FATAL("Unable to go background.", 0, 0, 0);
       }
     }
     else
     {
*/
       // reset the suspended flag
       pme->m_bSuspended = FALSE;

       // Start the app as usual
       pme->bEndKeyProcess = FALSE;
       
       if ((args != NULL) && (args->pszArgs != NULL))
       {
           if (STRCMP(args->pszArgs, "FTM") ==0)    
           {
             pme->m_dlgID = IDD_DEBUG_SCREEN_DIALOG;
             //pme->bEndKeyProcess = TRUE;
           } 
           else
           //pme->m_dlgID = IDD_TOP_DIALOG;
#if defined (FEATURE_VERSION_S1000T)|| defined(FEATURE_VERSION_W515V3)|| defined(FEATURE_VERSION_K212_HUALU)
               if(STRNCMP(args->pszArgs,"*#0000#",7) == 0)
               {
                  pme->m_dlgID = IDD_VERSION_DIALOG;
               }
#elif defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
			   if(STRNCMP(args->pszArgs,"*#888#",6) == 0)   //add by yangdecai  2010-11-16
               {
                  pme->m_dlgID = IDD_VERSION_DIALOG;
               }
#else
			   if(STRNCMP(args->pszArgs,"*#2687#",7) == 0)   //add by yangdecai  2010-11-16
               {
                  pme->m_dlgID = IDD_VERSION_DIALOG;
               }
#endif

              if(STRNCMP(args->pszArgs,"*#06#",5) == 0)
              {
                 pme->m_dlgID = IDD_ESN_DIALOG;
              }

              if(STRNCMP(args->pszArgs,"*#888*#",7) == 0)
              {
                 pme->m_dlgID = IDD_VERSION_DIALOG;
              }
              
               if(STRNCMP(args->pszArgs,"*#*#8378#0#",11) == 0)
               {
                  pme->m_dlgID = IDD_TOP_DIALOG;
               }			   
       }
            
       if (ISHELL_CreateDialog(ps,
                               FLDDBG_RES_FILE,
                               pme->m_dlgID,
                               NULL) != AEE_SUCCESS) {
          MSG_FATAL("Can't Create Top Dialog", 0, 0, 0 );
       }
       pme->old_port_map = RDM_NULL_DEV;
       pme->old_port_map1 = RDM_NULL_DEV;
       pme->old_port_map2 = RDM_NULL_DEV;
       DeviceInfo.wStructSize = sizeof(DeviceInfo);
       ISHELL_GetDeviceInfo(ps, &DeviceInfo);
       pme->m_screen_rc.x = 0;
       pme->m_screen_rc.y = 0;
       pme->m_screen_rc.dx = (int16) DeviceInfo.cxScreen;
       pme->m_screen_rc.dy = (int16) DeviceInfo.cyScreen;
	   pme->m_CurrId = 0;
	   
	   if (pme->m_pDisplay != NULL)
       {
             (void) IDISPLAY_Release(pme->m_pDisplay);
             pme->m_pDisplay = NULL;
       }
       pme->m_pDisplay = args->pDisplay;
       (void) IDISPLAY_AddRef(pme->m_pDisplay);
       MSG_FATAL("ISHELL_CreateInstance AEECLSID_ANNUNCIATOR", 0, 0, 0);
	   if (AEE_SUCCESS != ISHELL_CreateInstance(pme->a.m_pIShell,AEECLSID_ANNUNCIATOR,(void **)&pme->m_pIAnn))
       {
           return EFAILED;
       }
       IANNUNCIATOR_SetFieldIsActiveEx(pme->m_pIAnn,FALSE);
      {
        AECHAR WTitle[40] = {0};
        MSG_FATAL("CFieldDebug_OnDialogStart 1", 0, 0, 0);
        if(pme->a.m_pIShell == NULL)
        {
            MSG_FATAL("pMe->a.m_pIShell == NULL", 0, 0, 0);
        }
        (void)ISHELL_LoadResString(pme->a.m_pIShell,
        FLDDBG_RES_FILE,                                
        IDS_OPTION_TITLE,
        WTitle,
        sizeof(WTitle));
        if(pme->m_pIAnn != NULL)
        {
            IANNUNCIATOR_SetFieldText(pme->m_pIAnn,WTitle);
        }
      }        
     return TRUE;

   case EVT_APP_SUSPEND:
      ISHELL_CancelTimer(pme->a.m_pIShell, EndErrorMessageDlg, (void*) pme);
      pme->m_bSuspended = TRUE;
      return TRUE;

   case EVT_APP_RESUME:
    pme->m_bSuspended = FALSE;
    if (ISHELL_CreateDialog(ps, FLDDBG_RES_FILE, pme->m_dlgID, NULL) !=
        AEE_SUCCESS) {
       MSG_FATAL("Can't Create Top Dialog", 0, 0, 0 );
    }

    return TRUE;

   case EVT_KEY:
     if(wParam == AVK_END)
     {
      if(pme->bEndKeyProcess == FALSE)
      {
        ISHELL_CloseApplet(pme->a.m_pIShell, FALSE);
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
        IDIB_Release(pme->m_pIDIB);
        pme->m_pIDIB = NULL;
      }
      pme->m_bDisplayImage = FALSE;
      pme->bEndKeyProcess = FALSE;
      ISHELL_CancelTimer(pme->a.m_pIShell, EndErrorMessageDlg, (void*) pme);
      pme->m_bSuspended = TRUE;
#endif
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
           MSG_FATAL("PGSDIConf NULL", 0, 0, 0);
           return FALSE;
         }
         //First check that App is not suspended, otherwise nothing to do!
         if(pIDialog == NULL)
         {
           FREEIF(pGSDIConf);
           return TRUE;
         }
         wDlgID = IDIALOG_GetID(pIDialog);
         if(wDlgID == 0)
         {
           FREEIF(pGSDIConf);
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
           FREEIF(pGSDIConf);
           return TRUE;
         }
         else
         {
          //Wrong response..display error message
           MSG_FATAL("Failed in EVT_SIM_WRITTEN_FINAL", 0, 0, 0);
           FREEIF(pGSDIConf);
           return CheckAEEReturnStatus(ISHELL_CreateDialog(pme->a.m_pIShell, FLDDBG_RES_FILE,
             IDD_ERROR_DIALOG, NULL));
         }
       }
       return TRUE;

     case EVT_SIM_WRITTEN:
       {
         MSG_MED("EVT_SIM_WRITTEN", 0, 0, 0);
         pGSDIConf = (gsdi_cnf_T*) dwParam;
         if(pGSDIConf == NULL)
         {
           MSG_FATAL("PGSDIConf NULL", 0, 0, 0);
           return FALSE;
         }
         //First check that App is not suspended, otherwise nothing to do!
         if(pIDialog == NULL)
         {
          FREEIF(pGSDIConf);
           return TRUE;
         }
         wDlgID = IDIALOG_GetID(pIDialog);
         if(wDlgID == 0)
         {
           FREEIF(pGSDIConf);
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
           MSG_FATAL("Failed in EVT_SIM_WRITTEN", 0, 0, 0);
           //Wrong response..display error message
           //Clear the "wait" dialog
           if(IDD_WAIT_FPLMN_CLEAR_DIALOG == wDlgID)
           {
             ISHELL_EndDialog(pme->a.m_pIShell);
           }
           FREEIF(pGSDIConf);
           return CheckAEEReturnStatus(ISHELL_CreateDialog(pme->a.m_pIShell, FLDDBG_RES_FILE,
             IDD_ERROR_DIALOG, NULL));
         }
       }
       FREEIF(pGSDIConf);
       return TRUE;

     case EVT_GET_FILE_ATTR:
       {
         MSG_MED("EVT_GET_FILE_ATTR", 0, 0, 0);
         //First check that App is not suspended, otherwise nothing to do!
         pGSDIConf = (gsdi_cnf_T*) dwParam;
         if(pGSDIConf == NULL)
         {
           MSG_FATAL("PGSDIConf NULL", 0, 0, 0);
           return FALSE;
         }
         if(pIDialog == NULL)
         {
           FREEIF(pGSDIConf);
           return TRUE;
         }
         wDlgID = IDIALOG_GetID(pIDialog);
         if(wDlgID == 0)
         {
           FREEIF(pGSDIConf);
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
             MSG_FATAL("Invalid card mode", 0, 0, 0);
             FREEIF(pGSDIConf);
             return FALSE;
           }
         }
         if(bGSDIError)
         {
           MSG_FATAL("Failed in EVT_GET_FILE_ATTR", 0, 0, 0);
           //Wrong response..display error message
           //Clear the "wait" dialog
           if(IDD_WAIT_FPLMN_CLEAR_DIALOG == wDlgID)
           {
             ISHELL_EndDialog(pme->a.m_pIShell);
           }
           FREEIF(pGSDIConf);
           return CheckAEEReturnStatus(ISHELL_CreateDialog(pme->a.m_pIShell, FLDDBG_RES_FILE,
             IDD_ERROR_DIALOG, NULL));

         }
       }
       FREEIF(pGSDIConf);
       return TRUE;
#endif
     case EVT_RDM_STATUS:
       (void) CFieldDebug_MMCDownloadHandleEvent(pme, eCode, wParam, dwParam);
       break;

     default:
        return FALSE;
     }
     return FALSE;
   }

   case EVT_NOTIFY:
     {
       IDialog *pd = NULL;
       uint16 dialog_id;
       AEENotify *notif = (AEENotify *) dwParam;

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
           return CFieldDebug_RATS_TESTHandleEvent(pme,eCode, wParam, dwParam);
         }
       }
       if (notif->dwMask & NMASK_STATUS_CHANGE)
       {
         if (dialog_id == IDD_RATS_DIALOG)
           return CFieldDebug_RATSHandleEvent(pme,eCode, wParam, dwParam);
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
          ISHELL_EndDialog(pme->a.m_pIShell); //End JPEG Dialog

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
          if (wParam == AVK_CLR)
          {
            IDISPLAY_ClearScreen(pme->a.m_pIDisplay);
            ISHELL_CreateDialog(pme->a.m_pIShell, FLDDBG_RES_FILE, IDD_JPEG_DECODE, NULL);
          }
          else if (wParam == AVK_LEFT)
            {
               if (pme->x_offset > 10)
                   pme->x_offset -= 10;

               CFieldDebug_DisplayImage(pme);
            }
          else if (wParam == AVK_RIGHT)
            {
               if (pme->x_offset < pme->m_rc.dx && pme->m_rc.dx > 176)
                   pme->x_offset += 10;

               CFieldDebug_DisplayImage(pme);
            }
          else if (wParam == AVK_UP)
            {
               if (pme->y_offset > 10)
                   pme->y_offset -= 10;

               CFieldDebug_DisplayImage(pme);
            }
          else if (wParam == AVK_DOWN)
            {
               if (pme->y_offset < pme->m_rc.dy && pme->m_rc.dy > 144)
                  pme->y_offset += 10;

               CFieldDebug_DisplayImage(pme);
            }
        }
        return TRUE;
#endif /* FEATURE_JPEG_DECODER */

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION
   case EVT_USER_REDRAW:
   		{
			return CFieldDebug_RouteDialogEvt(pme,eCode, wParam, dwParam);
		}
   		break;
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
   AECHAR  sTitle[20];


   p_dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
   p_stk = (IStatic *) IDIALOG_GetControl(p_dlg, IDC_ERROR_STATIC);
   STRTOWSTR("%d %d\n", fmt_str, sizeof(fmt_str));

   // Set the dimensions of the rectangle for the text control
   (void)MEMSET(szBuf, (AECHAR) 0, sizeof(szBuf));
    MEMSET (sTitle, 0, sizeof(sTitle));
	
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

   ISTATIC_SetProperties(p_stk, ST_UNDERLINE|ST_NOSCROLL|ST_CENTERTITLE);
   // Set the values of the title and text strings for this control
   (void) ISHELL_LoadResString(pme->a.m_pIShell,
                                        FLDDBG_RES_FILE,
                                        IDS_ERRORS,
                                        sTitle,
                                        sizeof(sTitle));
   if(pme->m_pIAnn != NULL)
   {
       IANNUNCIATOR_SetFieldText(pme->m_pIAnn,sTitle);
   }

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
    AECHAR szBuf[320];
    AECHAR imsi[20];
    IDialog *p_dlg;
    IStatic *p_stk;
    int n = 0;
    //int currentLine;
    uint32 esn;
    dword date = 0;
    AECHAR fmt_str[20];
    int i, j, count;
    AECHAR  sTitle[20]; 
    JulianType  jDate;
    uint32         dwSeconds;
#ifndef CUST_EDITION
    byte b=0;
    int temp_cont=0;
    char temp_str[160]={0};
#endif
    MEMSET (sTitle, 0, sizeof(sTitle));

    (void)MEMSET( szBuf,(AECHAR) 0, sizeof(szBuf));
    
    (void) ISHELL_LoadResString(pme->a.m_pIShell,
                               FLDDBG_RES_FILE,
                               IDS_SW_VERSION,
                               szBuf,
                               sizeof(szBuf));

    n = WSTRLEN(szBuf);
    szBuf[n++] = (AECHAR) '\n'; 

    (void) ICONFIG_GetItem(pme->m_pIConfig,
                          CFGI_BUILD_VERSION,
                          (szBuf + n),
                          sizeof(szBuf));

    n = WSTRLEN(szBuf);
    szBuf[n++] = (AECHAR) '\n';
#ifdef FEATURE_VERSION_K232_Y105A
{
    JulianType  jDate;
    AECHAR      wszDate[64]= {0};
    AECHAR      wFormat[64]= {L"%02d:%02d"};
    AECHAR      m_wstr[64]= {0};
    char        m_str[64]= {0}; 
    GetJulianDate(GETTIMESECONDS(), &jDate);
    WSTRCAT(szBuf+n,L"Build Time:");
    n = WSTRLEN(szBuf);
    szBuf[n++] = (AECHAR) '\n'; 
    n = WSTRLEN(szBuf);
    (void) ICONFIG_GetItem(pme->m_pIConfig,
              CFGI_BUILD_TIME,
              (szBuf + n),
              sizeof(szBuf));
    n = WSTRLEN(szBuf);
    szBuf[n++] = (AECHAR) ' ';
    n = WSTRLEN(szBuf);
    WSPRINTF(wszDate, sizeof(wszDate), wFormat, jDate.wHour, jDate.wMinute);
    for(i=0; ver_time[i]!=0; i++)
    {
       szBuf[n+i] = (AECHAR)ver_time[i];
    }
    n = WSTRLEN(szBuf);
}
#endif

#ifndef HWVERSION
    #define HWVERSION "V1.0"
#endif
#if defined(FEATURE_VERSION_K212)||defined(FEATURE_VERSION_K212_BH)
	#define HWVERSION "K212_MB_V0.1"
#endif
    n = WSTRLEN(szBuf);// update current Info. len
    szBuf[n++] = (AECHAR) '\n';
    (void) ISHELL_LoadResString(pme->a.m_pIShell,
                                FLDDBG_RES_FILE,
                                IDS_HW_VERSION,
                                (szBuf + n),
                                sizeof(szBuf));

    n = WSTRLEN(szBuf);
    szBuf[n++] = (AECHAR) '\n'; 

    STRTOWSTR(HWVERSION, (szBuf + n), sizeof(szBuf)); 
    
    n = WSTRLEN(szBuf);
    szBuf[n++] = (AECHAR) '\n';

    #ifndef FEATURE_VERSION_K232_Y105A
    n = WSTRLEN(szBuf);
    (void) ISHELL_LoadResString(pme->a.m_pIShell,
                               FLDDBG_RES_FILE,
                               IDS_COMPILE_TIME,
                               (szBuf+ n),
                               sizeof(szBuf));
    n = WSTRLEN(szBuf);
    szBuf[n++] = (AECHAR) '\n'; 
    (void) ICONFIG_GetItem(pme->m_pIConfig,
                          CFGI_BUILD_TIME,
                          (szBuf + n),
                          sizeof(szBuf));
    #endif
    n = WSTRLEN(szBuf);
    szBuf[n++] = (AECHAR) '\n';
   
    #if defined(FEATURE_VERSION_S1000T)||defined(FEATURE_VERSION_W515V3)
    #else
   (void) ISHELL_LoadResString(pme->a.m_pIShell,
                               FLDDBG_RES_FILE,
                               IDS_MODEL,
                               (szBuf + n),
                               sizeof(szBuf));

   n = WSTRLEN(szBuf);
   szBuf[n++] = (AECHAR) '\n';

   STRTOWSTR(OEMNV_DEFAULT_BANNER, (szBuf + n), sizeof(szBuf)); 

   n = WSTRLEN(szBuf);
   szBuf[n++] = (AECHAR) '\n';
      
    (void) ISHELL_LoadResString(pme->a.m_pIShell,
                               FLDDBG_RES_FILE,
                               IDS_ESN,
                               (szBuf + n),
                               sizeof(szBuf));
    
    n = WSTRLEN(szBuf);
    szBuf[n++] = (AECHAR) '\n';
    
    (void) ICONFIG_GetItem(pme->m_pIConfig,
                          CFGI_ESN,
                          &esn,
                          sizeof(esn));

    STRTOWSTR("%u %u", fmt_str, sizeof(fmt_str));
    
    WSPRINTF((szBuf + n),
            sizeof(szBuf),
            fmt_str,
            ((esn & 0xFF000000) >> 24),
            (esn & 0x00FFFFFF));
    
    n = WSTRLEN(szBuf);
    szBuf[n++] = (AECHAR) '\n';

    //Display ESN with hexadecimal
    STRTOWSTR("0x%08X", fmt_str, sizeof(fmt_str));
    WSPRINTF((szBuf + n),
            sizeof(szBuf),
            fmt_str,
            esn);
    
    n = WSTRLEN(szBuf);
    szBuf[n++] = (AECHAR) '\n';

    STRTOWSTR("IMSI_S :\n%s", fmt_str, sizeof(fmt_str));
   // WSTRCPY((szBuf + n), fmt_str);
    
    n = WSTRLEN(szBuf);

    (void) ICONFIG_GetItem(pme->m_pIConfig,
                            CFGI_IMSI_S,
                            (void *)imsi,
                            sizeof(AECHAR) * OEMNV_IMSI_S_LENGTH);
   WSPRINTF((szBuf + n),
                sizeof(szBuf),
                fmt_str,
                imsi);
    n = WSTRLEN(szBuf);
    szBuf[n++] = (AECHAR) '\n';                    

    STRTOWSTR("PRL Ver : \n%d\n", fmt_str, sizeof(fmt_str));
    WSPRINTF((szBuf + n),
                sizeof(szBuf),
                fmt_str,
                nv_prl_version(CM_NAM_1));
    
    n = WSTRLEN(szBuf);

    STRTOWSTR("ME ESN : \n0x%08X\n", fmt_str, sizeof(fmt_str));
    if(IsRunAsUIMVersion())
    {
       extern dword OEM_GetMEESN(void);
       WSPRINTF((szBuf + n),
             sizeof(szBuf),
             fmt_str,
             OEM_GetMEESN());
    }  
    else
    {
       WSPRINTF((szBuf + n),
             sizeof(szBuf),
             fmt_str,
             esn);
    }
    n = WSTRLEN(szBuf);

#ifdef FEATURE_RF_ZIF
   (void) ISHELL_LoadResString(pme->a.m_pIShell,
                               FLDDBG_RES_FILE,
                               IDS_RFCAL_VER,
                               (szBuf + n),
                               sizeof(szBuf));

   n = WSTRLEN(szBuf);
   szBuf[n++] = (AECHAR) '\n';

   (void) ICONFIG_GetItem(pme->m_pIConfig,
                          CFGI_RFCAL_VERSION,
                          (szBuf + n),
                          sizeof(szBuf));

   n = WSTRLEN(szBuf);
   szBuf[n++] = (AECHAR) '\n'; 
	#ifdef CUST_EDITION
	(void) ISHELL_LoadResString(pme->a.m_pIShell,
                               FLDDBG_RES_FILE,
                               IDS_RFCAL_STATUS,
                               (szBuf + n),
                               sizeof(szBuf));
	#else
   (void) ISHELL_LoadResString(pme->a.m_pIShell,
                               FLDDBG_RES_FILE,
                               IDS_RFCAL_DATE,
                               (szBuf + n),
                               sizeof(szBuf));
   #endif
   n = WSTRLEN(szBuf);
   szBuf[n++] = (AECHAR) '\n';
   date = 0;
   (void) ICONFIG_GetItem(pme->m_pIConfig,
                          CFGI_RFCAL_DATE,
                          &date,
                          sizeof(date));
   #if 1
   #ifdef CUST_EDITION
   {
   		nv_item_type nvi;    
		uint16 resid = IDS_REGULATE_NONE;
		n = WSTRLEN(szBuf);
		if (NV_DONE_S == OEMNV_Get(NV_TIME_SHOW_I, &nvi))
        {                                
            if (nvi.time_show == TRUE)
            {
                resid  = IDS_REGULATE_SUCCEED;
            }
            else
            {
                resid  = IDS_REGULATE_FAIL;
            }
        }   
		(void) ISHELL_LoadResString(pme->a.m_pIShell,
                               FLDDBG_RES_FILE,
                               resid,
                               (szBuf + n),
                               sizeof(szBuf));
		n = WSTRLEN(szBuf);
   		szBuf[n++] = (AECHAR) '\n';
   }
   #else
   STRTOWSTR("%08x", fmt_str, sizeof(fmt_str));
   WSPRINTF((szBuf + n),
            sizeof(szBuf),
            fmt_str,
            date);

   // prepare the date format
   n = WSTRLEN(szBuf);
   for (i = n - 1, j = n + 1, count = 0;
       count < 2;
       count++) {
      szBuf[j--] = szBuf[i--];
   }

   szBuf[j--] = (AECHAR) '-';

   for (count = 0; count < 2; count++) {
      szBuf[j--] = szBuf[i--];
   }

   szBuf[j] = (AECHAR) '-';
   
   n = WSTRLEN(szBuf);
   szBuf[n++] = (AECHAR) '\n';
   #endif
   #else
   dwSeconds = GETTIMESECONDS();
   MSG_FATAL("date===========%d,dwSeconds=%d",date,dwSeconds,0);
   GetJulianDate(date, &jDate);	
   MSG_FATAL("jDate.wDay=%d,jDate.wMonth=%d,jDate.wYear=%d",jDate.wDay,jDate.wMonth,jDate.wYear);
   STRTOWSTR("%02x", fmt_str, sizeof(fmt_str));
   WSPRINTF((szBuf + n),
            sizeof(szBuf),
            fmt_str,
            jDate.wDay);
   //n = WSTRLEN(szBuf);  
   //szBuf[n++] = (AECHAR) '-'; 
   n = WSTRLEN(szBuf);  
   STRTOWSTR("%02x", fmt_str, sizeof(fmt_str));
   WSPRINTF((szBuf + n),
            sizeof(szBuf),
            fmt_str,
            jDate.wMonth);
   //n = WSTRLEN(szBuf);  
   //szBuf[n++] = (AECHAR) '-'; 
   n = WSTRLEN(szBuf); 
   STRTOWSTR("%04d", fmt_str, sizeof(fmt_str));
   WSPRINTF((szBuf + n),
            sizeof(szBuf),
            fmt_str,
            jDate.wYear);
   n = WSTRLEN(szBuf); 
   // prepare the date format
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
   n = WSTRLEN(szBuf);
   szBuf[n++] = (AECHAR) '\n';
   #endif
#endif   

//HW
#ifndef CUST_EDITION
    // Get HDET HW info
    if (SUCCESS == ICONFIG_GetItem(pme->m_pIConfig,
                                  CFGI_DEBUG_HDET,
                                  &b,
                                  sizeof(b)))
    {
        SPRINTF((temp_str + temp_cont),"HW I.: ADC_HDET_CELL: 0x%02X\n", b);
    }
    else
    {
        SPRINTF((temp_str + temp_cont),"HW I.: ADC_HDET_CELL: N/A\n");
    }
    temp_cont=STRLEN(temp_str);// update temp Info. len
    
    
    // Get HDET SW Feature
    #ifdef RF_HAS_HDET_BYPASS
    {
        SPRINTF((temp_str + temp_cont),"SW F.: HAS_HDET_BY_PASS (NO HDET)\n", b);
    }
    #else//!RF_HAS_HDET_BYPASS
    {
        SPRINTF((temp_str + temp_cont),"SW F.: !HAS_HDET_BY_PASS (   HDET)\n", b);
    }
    #endif// RF_HAS_HDET_BYPASS
    temp_cont=STRLEN(temp_str);// update temp Info. len
    
    //Insert temp info to buffer
    STR_TO_WSTR(temp_str, (szBuf + n), (sizeof(szBuf)-n*2));
    n = WSTRLEN(szBuf);// update current Info. len
    szBuf[n++] = (AECHAR) '\n';
#endif //CUST_EDITION
//HW 
    
    STRTOWSTR("IVERSION:", (szBuf + n), sizeof(szBuf)); 

    n = WSTRLEN(szBuf);
    szBuf[n++] = (AECHAR) '\n';
    
    STRTOWSTR(ver_interversion, (szBuf + n), sizeof(szBuf)); 

    n = WSTRLEN(szBuf);
    szBuf[n++] = (AECHAR) '\n';
    
    //insert the SW ver. date
    for(i=0; ver_date[i]!=0; i++)
    {
       szBuf[n+i] = (AECHAR)ver_date[i];
    }
    n = WSTRLEN(szBuf);
    
    //insert a white sapce
    szBuf[n++] = (AECHAR) ' ';
    n = WSTRLEN(szBuf);
    
    //insert the SW ver. time
    for(i=0; ver_time[i]!=0; i++)
    {
       szBuf[n+i] = (AECHAR)ver_time[i];
    }
    n = WSTRLEN(szBuf);
    
    //insert a line space
    szBuf[n++] = (AECHAR) '\n';
    n = WSTRLEN(szBuf);
//SW ver
	#endif
   p_dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
   p_stk = (IStatic *) IDIALOG_GetControl(p_dlg, IDC_VER_STAT);

   // Set the values of the title and text strings for this control
    ISTATIC_SetProperties(p_stk, ST_UNDERLINE|ST_NOSCROLL|ST_CENTERTITLE);
   (void) ISHELL_LoadResString(pme->a.m_pIShell,
                                        FLDDBG_RES_FILE,
                                        IDS_VERSION_TITLE,
                                        sTitle,
                                        sizeof(sTitle));
    if(pme->m_pIAnn != NULL)
    {
        IANNUNCIATOR_SetFieldText(pme->m_pIAnn,sTitle);
    }     
   (void) ISTATIC_SetText(p_stk,
                          NULL,
                          szBuf,
                          AEE_FONT_NORMAL,
                          AEE_FONT_NORMAL);
   
 //currentLine = ISTATIC_GoToLine(p_stk, currentLine);
   (void) ISTATIC_Redraw(p_stk);
}



static void CFieldDebug_DrawFactorySetCountScreen(CFieldDebug * pme)
{
    AECHAR szBuf[64];
    AECHAR str[32];
    AECHAR strct1[10];
    AECHAR strct2[10];
    IDialog *p_dlg;
    IStatic *p_stk;
    int ret = 0;  
    uint8 factoryset_count = 0;
    uint8 factoryset_count_ex = 0;
    
    MSG_FATAL("CFieldDebug_DrawFactorySetCountScreen Start", 0, 0, 0);
 
    MEMSET(szBuf,  0, sizeof(szBuf));    
    
    OEM_GetConfig(CFGI_FACTORYSET_COUNT, &factoryset_count, sizeof(uint8));  
    OEM_GetConfig(CFGI_FACTORYSET_COUNT_EX, &factoryset_count_ex, sizeof(uint8)); 
    
    ISHELL_LoadResString(pme->a.m_pIShell,
                               FLDDBG_RES_FILE,
                               IDS_FROM_QUICKTEST,
                               szBuf,
                               sizeof(szBuf));  

    ISHELL_LoadResString(pme->a.m_pIShell,
                               FLDDBG_RES_FILE,
                               IDS_FROM_SETTING,
                               str,
                               sizeof(str));
    

    WSPRINTF(strct1, sizeof(strct1), L"%d", factoryset_count); 
    WSPRINTF(strct2, sizeof(strct2), L"%d", factoryset_count_ex); 
   
    WSTRCAT(szBuf,L"\n");  
    WSTRCAT(szBuf, strct1);
    WSTRCAT(szBuf, L"\n");
    
    WSTRCAT(szBuf, str);
    WSTRCAT(szBuf,L"\n");   
    WSTRCAT(szBuf, strct2);
    WSTRCAT(szBuf, L"\n");
    
   
   p_dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
   p_stk = (IStatic *) IDIALOG_GetControl(p_dlg, IDC_COUNT_STAT);

   if(p_stk == NULL)
   {
       MSG_FATAL("CFieldDebug_DrawEsnScreen 2 p_stk == NULL", 0, 0, 0);
   }
   // Set the values of the title and text strings for this control
    ISTATIC_SetProperties(p_stk, ST_UNDERLINE|ST_NOSCROLL|ST_CENTERTITLE);
   (void) ISTATIC_SetText(p_stk,
                          NULL,
                          szBuf,
                          AEE_FONT_NORMAL,
                          AEE_FONT_NORMAL);
   
   (void) ISTATIC_Redraw(p_stk);
   MSG_FATAL("CFieldDebug_DrawEsnScreen End", 0, 0, 0);

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

   dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
   mctl = (IMenuCtl*)IDIALOG_GetControl(dlg, IDC_SMSMOSO_MENU);

   (void) ICONFIG_GetItem(pme->m_pIConfig,
                          CFGI_SMS_SERVICE_OPTION,
                          &mo_so,
                          sizeof(byte));

   InitMenuIcons(mctl);
   switch (mo_so) {
   case OEMNV_SMS_SO_SO6:
      SetMenuIcon(mctl, IDS_SMSMO_SO_6, TRUE);
      break;
   case OEMNV_SMS_SO_SO14:
      SetMenuIcon(mctl, IDS_SMSMO_SO_14, TRUE);
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

   dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
   mctl = (IMenuCtl*)IDIALOG_GetControl(dlg, IDC_VOICE_PRIVACY_MENU);

/*   (void) ICONFIG_GetItem(pme->m_pIConfig,
                          CFGI_VOICE_PRIVACY,
                          &vp,
                          sizeof(byte));

   InitMenuIcons(mctl);
   switch (vp) {
   case OEMNV_VP_STANDARD:
      SetMenuIcon(mctl, IDS_VP_STANDARD, TRUE);
      break;
   case OEMNV_VP_ENHANCED:
      SetMenuIcon(mctl, IDS_VP_ENHANCED, TRUE);
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
      	{
        wMenuID = IDC_SECURITY_TEST;
        pIMenuCtl = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                                   wMenuID);
          {
            AECHAR WTitle[30] = {0};
            (void)ISHELL_LoadResString(pme->a.m_pIShell,
            FLDDBG_RES_FILE,                                
            IDS_SECURITY_TESTS,
            WTitle,
            sizeof(WTitle));
            if(pme->m_pIAnn != NULL)
            {
                IANNUNCIATOR_SetFieldText(pme->m_pIAnn,WTitle);
            }
          }           
        IMENUCTL_AddItem(pIMenuCtl, FLDDBG_RES_FILE, IDS_SSL_TEST,  IDS_SSL_TEST, NULL, 0);
        IMENUCTL_AddItem(pIMenuCtl, FLDDBG_RES_FILE, IDS_SEC_TESTS,  IDS_SEC_TESTS, NULL, 0);
        IMENUCTL_AddItem(pIMenuCtl, FLDDBG_RES_FILE, IDS_DRM_TESTS,  IDS_DRM_TESTS, NULL, 0);
        IMENUCTL_AddItem(pIMenuCtl, FLDDBG_RES_FILE, IDS_VPN_TESTS,  IDS_VPN_TESTS, NULL, 0);
        }
        break;

      case IDD_SSL_TEST:
      	{
        wMenuID = IDC_SSL_TEST;
        pIMenuCtl = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                                   wMenuID);
          {
            AECHAR WTitle[30] = {0};
            (void)ISHELL_LoadResString(pme->a.m_pIShell,
            FLDDBG_RES_FILE,                                
            IDS_SSL_TEST,
            WTitle,
            sizeof(WTitle));
            if(pme->m_pIAnn != NULL)
            {
                IANNUNCIATOR_SetFieldText(pme->m_pIAnn,WTitle);
            }
          }           
        IMENUCTL_AddItem(pIMenuCtl, FLDDBG_RES_FILE, IDS_SSL_HTTP,  IDS_SSL_HTTP, NULL, 0);
        IMENUCTL_AddItem(pIMenuCtl, FLDDBG_RES_FILE, IDS_SSL_FUNCT,  IDS_SSL_FUNCT, NULL, 0);
        IMENUCTL_AddItem(pIMenuCtl, FLDDBG_RES_FILE, IDS_SSL_MULTI,  IDS_SSL_MULTI, NULL, 0);
        IMENUCTL_AddItem(pIMenuCtl, FLDDBG_RES_FILE, IDS_SSL_LONG,  IDS_SSL_LONG, NULL, 0);
        }
        break;

      case IDD_SEC_TEST:
      	{
        wMenuID = IDC_SEC_TEST;
        pIMenuCtl = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                                   wMenuID);
          {
            AECHAR WTitle[20] = {0};
            (void)ISHELL_LoadResString(pme->a.m_pIShell,
            FLDDBG_RES_FILE,                                
            IDS_SEC_TESTS,
            WTitle,
            sizeof(WTitle));
            if(pme->m_pIAnn != NULL)
            {
                IANNUNCIATOR_SetFieldText(pme->m_pIAnn,WTitle);
            }
          }           
        IMENUCTL_AddItem(pIMenuCtl, FLDDBG_RES_FILE, IDS_SECAPI_PKX,  IDS_SECAPI_PKX, NULL, 0);
        IMENUCTL_AddItem(pIMenuCtl, FLDDBG_RES_FILE, IDS_SECAPI_ENC,  IDS_SECAPI_ENC, NULL, 0);
        IMENUCTL_AddItem(pIMenuCtl, FLDDBG_RES_FILE, IDS_SECAPI_HSH,  IDS_SECAPI_HSH, NULL, 0);
        IMENUCTL_AddItem(pIMenuCtl, FLDDBG_RES_FILE, IDS_SEC_CRYPTO,  IDS_SEC_CRYPTO, NULL, 0);
        IMENUCTL_AddItem(pIMenuCtl, FLDDBG_RES_FILE, IDS_SEC_CUSTOM,  IDS_SEC_CUSTOM, NULL, 0);
        }
        break;

      case IDD_DRM_TEST:
      	{
        wMenuID = IDC_DRM_TEST;
        pIMenuCtl = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                                   wMenuID);
          {
            AECHAR WTitle[20] = {0};
            (void)ISHELL_LoadResString(pme->a.m_pIShell,
            FLDDBG_RES_FILE,                                
            IDS_DRM_TESTS,
            WTitle,
            sizeof(WTitle));
            if(pme->m_pIAnn != NULL)
            {
                IANNUNCIATOR_SetFieldText(pme->m_pIAnn,WTitle);
            }
          }           
        IMENUCTL_AddItem(pIMenuCtl, FLDDBG_RES_FILE, IDS_DRM_ADD_RO,  IDS_DRM_ADD_RO, NULL, 0);
        IMENUCTL_AddItem(pIMenuCtl, FLDDBG_RES_FILE, IDS_DRM_READ_RO,  IDS_DRM_READ_RO, NULL, 0);
        IMENUCTL_AddItem(pIMenuCtl, FLDDBG_RES_FILE, IDS_DRM_USE_RO,  IDS_DRM_USE_RO, NULL, 0);
        IMENUCTL_AddItem(pIMenuCtl, FLDDBG_RES_FILE, IDS_DRM_DEL_ARO,  IDS_DRM_DEL_ARO, NULL, 0);
        IMENUCTL_AddItem(pIMenuCtl, FLDDBG_RES_FILE, IDS_DRM_DEL_RO,  IDS_DRM_DEL_RO, NULL, 0);
        }
        break;
      case IDD_VPN_TEST:
      	{
        wMenuID = IDC_VPN_TEST;
        pIMenuCtl = (IMenuCtl *) IDIALOG_GetControl((IDialog *) dwParam,
                                                   wMenuID);
        IMENUCTL_AddItem(pIMenuCtl, FLDDBG_RES_FILE, IDS_VPN1_START,  IDS_VPN1_START, NULL, 0);
        IMENUCTL_AddItem(pIMenuCtl, FLDDBG_RES_FILE, IDS_VPN2_START,  IDS_VPN2_START, NULL, 0);
        IMENUCTL_AddItem(pIMenuCtl, FLDDBG_RES_FILE, IDS_VPN3_START,  IDS_VPN3_START, NULL, 0);
        IMENUCTL_AddItem(pIMenuCtl, FLDDBG_RES_FILE, IDS_VPN_STOP,  IDS_VPN_STOP, NULL, 0);
        }
        break;
      }

      pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl((IDialog*) dwParam, wMenuID);
      if(pIMenuCtl == NULL)
      {
        MSG_FATAL("Unable to get menu for %d", wParam, 0, 0);
        return FALSE;
      }
      if(wParam == IDD_SECURITY_SERVICES)
      {
#ifndef FEATURE_SEC_TESTAPPS
        IMENUCTL_DeleteItem(pIMenuCtl, IDS_SSL_TEST);
        IMENUCTL_DeleteItem(pIMenuCtl, IDS_SEC_TESTS);
        IMENUCTL_DeleteItem(pIMenuCtl, IDS_DRM_TESTS);
#else
#error code not present
#endif /* FEATURE_SEC_TESTAPPS */

#ifndef FEATURE_SEC_IPSEC_VPN_CLNT
        IMENUCTL_DeleteItem(pIMenuCtl, IDS_VPN_TESTS);
#endif
      }
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
    case IDS_SSL_TEST:
      return CheckAEEReturnStatus(ISHELL_CreateDialog(pme->a.m_pIShell,
        FLDDBG_RES_FILE, IDD_SSL_TEST, NULL));

    case IDS_SEC_TESTS:
      return CheckAEEReturnStatus(ISHELL_CreateDialog(pme->a.m_pIShell,
        FLDDBG_RES_FILE, IDD_SEC_TEST, NULL));

#ifdef FEATURE_SEC_DRM
    case IDS_DRM_TESTS:
      return CheckAEEReturnStatus(ISHELL_CreateDialog(pme->a.m_pIShell,
        FLDDBG_RES_FILE, IDD_DRM_TEST, NULL));

    case IDS_DRM_ADD_RO:
      DRMTestApp( pme->a.m_pIShell );
      return TRUE;

    case IDS_DRM_READ_RO:
      DRMUnitTestApp( pme->a.m_pIShell );
      return TRUE;
#endif /* FEATURE_SEC_DRM */


    case IDS_VPN_TESTS:
      return CheckAEEReturnStatus(ISHELL_CreateDialog(pme->a.m_pIShell,
        FLDDBG_RES_FILE, IDD_VPN_TEST, NULL));

#ifdef FEATURE_SEC_IPSEC_VPN_CLNT
    case IDS_VPN1_START:
      secipsvpnclnt_start( 0 );
      return TRUE;

    case IDS_VPN2_START:
      secipsvpnclnt_start( 1 );
      return TRUE;

    case IDS_VPN3_START:
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
#error code not present
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
  switch(eCode)
  {
  case EVT_DIALOG_START:
    ISHELL_SetTimer(pme->a.m_pIShell, SHOW_MESSAGE_TIMEOUT_MS,
      EndErrorMessageDlg, (void*) pme);
    return TRUE;

  case EVT_KEY:
    return CheckAEEReturnStatus(ISHELL_EndDialog(pme->a.m_pIShell));

  case EVT_DIALOG_END:
    ISHELL_CancelTimer(pme->a.m_pIShell, EndErrorMessageDlg, (void*) pme);
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
     MSG_FATAL("pme NULL", 0, 0, 0);
     return;
  }

  pIShell = pme->a.m_pIShell;

  if(pIShell == NULL)
  {
     MSG_FATAL("pIShell NULL", 0, 0, 0);
     return;
  }

  pIDialog = ISHELL_GetActiveDialog(pIShell);

  if(pIDialog == NULL)
  {
    MSG_FATAL("pIDialog NULL", 0, 0, 0);
    return;
  }

  if(IDD_ERROR_DIALOG != IDIALOG_GetID(pIDialog))
  {
    MSG_FATAL("Non-error dialog end", 0, 0, 0);
    return;
  }

  ISHELL_EndDialog(pIShell);

  pIDialog = ISHELL_GetActiveDialog(pIShell);
  if(pIDialog == NULL)
  {
    pme->m_dlgID = IDD_MMC_DIALOG;//MMC download 返回的对话框
    ISHELL_CreateDialog(pIShell,
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
     MSG_FATAL("pIShell NULL", 0, 0, 0);
     return;
  }

  pLGSDIConf= (gsdi_cnf_T*)MALLOC(sizeof(gsdi_cnf_T));
  if(pLGSDIConf == NULL)
  {
    MSG_FATAL("Malloc failed!", 0, 0, 0);
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
     MSG_FATAL("pIShell NULL", 0, 0, 0);
     return;
  }

  pLGSDIConf= (gsdi_cnf_T*)MALLOC(sizeof(gsdi_cnf_T));
  if(pLGSDIConf == NULL)
  {
    MSG_FATAL("Malloc failed!", 0, 0, 0);
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
     MSG_FATAL("pIShell NULL", 0, 0, 0);
     return;
  }
  pLGSDIConf= (gsdi_cnf_T*)MALLOC(sizeof(gsdi_cnf_T));
  if(pLGSDIConf == NULL)
  {
    MSG_FATAL("Malloc failed!", 0, 0, 0);
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
    MSG_FATAL("pIShell NULL", 0, 0, 0);
    return;
  }
  ISHELL_PostEventEx(pIShell, 0, AEECLSID_FIELDDEBUGAPP, EVT_USER,
      EVT_RDM_STATUS, (uint32) status);
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
  /* First check and see the valid devices for MMC */
  status = rdm_get_valid_ports (RDM_MMC_SRVC, devices);
  if (!status) {
    //End the current dialog and create the error dialog.
    ISHELL_EndDialog(pme->a.m_pIShell);
    ISHELL_CreateDialog(pme->a.m_pIShell,
            FLDDBG_RES_FILE, IDD_ERROR_DIALOG, NULL);
    return;
  }

  found = FALSE;
  /* See if MMC over USB is present in list */
  for (i=0; i<RDM_DEV_MAX;i++) {
    if ((devices[i].device == RDM_USB_MMC_DEV) &&
        (devices[i].display)) {
      found = TRUE;
      break;
    }
  }

  if (!found) {
    //End the current dialog and create the error dialog.
    ISHELL_EndDialog(pme->a.m_pIShell);
    ISHELL_CreateDialog(pme->a.m_pIShell,
            FLDDBG_RES_FILE, IDD_ERROR_DIALOG, NULL);
    return;
  }

  pme->rdmState = RDM_STATUS_INIT1;
  pme->old_port_map = rdm_get_device (RDM_DATA_SRVC);
  if (pme->old_port_map == RDM_USB_CDC_DEV)
  {
    rdm_assign_port (RDM_DATA_SRVC, RDM_NULL_DEV,
                      CFieldDebug_RDMCb);
  }
  else
  {
    //Fake the RDM event to go to next state.
    ISHELL_PostEventEx(pme->a.m_pIShell, 0, AEECLSID_FIELDDEBUGAPP, EVT_USER,
      EVT_RDM_STATUS, RDM_DONE_S);
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
  PRINT_FUNCTION_NAME();
  pme->nCount1 = pme->nCount2;
#if (defined(FEATURE_USB) && defined(FEATURE_MMC)) || defined (FEATURE_USB_MASS_STORAGE)
  pme->nCount2 = usbsdms_get_write_blk_count();
#else
  pme->nCount2 = 0;
#endif
  if(pme->nCount1 == pme->nCount2)
  {
    MSG_MED("MMC Download Done",0, 0, 0);
    pme->bEndKeyProcess = FALSE;
    {
      IDialog *pIDialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
      if(pIDialog != NULL)
      {
        IMenuCtl *pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIDialog, IDC_SK_MMC_DOWNLOAD);
        if(pIMenuCtl != NULL)
        {
          IMENUCTL_AddItem(pIMenuCtl, FLDDBG_RES_FILE, IDS_QUIT, IDS_QUIT, NULL, 0);
          IMENUCTL_Redraw(pIMenuCtl);
        }

        IDIALOG_SetFocus(pIDialog, IDC_SK_MMC_DOWNLOAD);
      }
    }
    return;
  }
  ISHELL_SetTimer(pme->a.m_pIShell, 1000, CFieldDebug_MMCDownload, pUser);
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
  pme->rdmState = RDM_STATUS_EXIT1;
  rdm_assign_port (RDM_MMC_SRVC, RDM_NULL_DEV,
                  CFieldDebug_RDMCb);
  pme->bEndKeyProcess = FALSE;
   
}
#if defined(FEATURE_GSM) || defined (FEATURE_WCDMA)
#error code not present
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
/*lint -esym(715,pme)*/
/*lint -esym(715,eCode)*/
/*lint -esym(715,wParam)*/
/*lint -esym(715,dwParam)*/
#ifdef FEATURE_JPEG_DECODER
  static IDialog * pIActiveDialog = NULL;
  static IMenuCtl * pICurrentMenu;
  uint16 wDialogId;
  CtlAddItem pai;
  cmx_handle_type cmx_handle;
  char pDest[50];
  char finalFileName[100];
  char sz[2] =   { DIRECTORY_CHAR, 0 };

  switch(eCode)
  {
    case EVT_DIALOG_START:
       pIActiveDialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
       wDialogId = IDIALOG_GetID(pIActiveDialog);
       InitJPEGFileList(pme);
       return TRUE;

    case EVT_KEY:
        if (wParam == AVK_CLR || wParam == AVK_END)
        {
          if (!pme->m_bDisplayImage)
          {
             ISHELL_EndDialog(pme->a.m_pIShell); //End JPEG Decode Dialog.
             ISHELL_CreateDialog(pme->a.m_pIShell, FLDDBG_RES_FILE, IDD_TOP_DIALOG, NULL);
          }
          if (pme->m_bDisplayImage)
             pme->m_bDisplayImage = FALSE;
        }
        return TRUE;

    case EVT_COMMAND:
        pIActiveDialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
        pICurrentMenu = NULL; //(IMenuCtl *)IDIALOG_GetControl(pIActiveDialog, IDL_JPEG_DECODE);
        IMENUCTL_GetItem(pICurrentMenu, IMENUCTL_GetSel(pICurrentMenu), &pai);

        cmx_handle.efs.source_type = CMX_SOURCE_EFS;
        WSTRTOSTR(pai.pText, pDest, sizeof(pDest));

        STRCPY(finalFileName, ROOT_DIR);
        STRCAT(finalFileName, sz);
        STRCAT(finalFileName, MEDIA_DIR);
        STRCAT(finalFileName, sz);
        STRCAT(finalFileName, pDest);

        STRCPY(cmx_handle.efs.filename, finalFileName);

        cmx_jpeg_decode(&cmx_handle, (cmx_image_cb_func_ptr_type) CFieldDebug_JPEGDecodeCB, (void *) pme);
        return TRUE;

    case EVT_DIALOG_END:
        return TRUE;

    default:
        return TRUE;
  }
#endif //FEATURE_JPEG_DECODER
  return FALSE;
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
/*lint -esym(715,pme)*/
/*lint -esym(715,eCode)*/
/*lint -esym(715,wParam)*/
/*lint -esym(715,dwParam)*/
#ifdef FEATURE_HDR
#error code not present
 #endif //FEATURE_HDR
  return FALSE;
}

#ifdef FEATURE_HDR
#error code not present
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
  CtlAddItem pai;
  FileInfo fi;
  char * szBuf;
  AECHAR fileNameBuf[50];
  int wItemID = 0;

   pIActiveDialog = ISHELL_GetActiveDialog(pme->a.m_pIShell);
   wDlgId = IDIALOG_GetID(pIActiveDialog);

   if (wDlgId == IDD_JPEG_DECODE)
   {
     pIJPEGFileListMenuCtl = NULL; //(IMenuCtl *)IDIALOG_GetControl(pIActiveDialog, IDL_JPEG_DECODE);

     if (pIJPEGFileListMenuCtl != NULL)
       {
         IMENUCTL_DeleteAll(pIJPEGFileListMenuCtl);
		 #if 0
         IMENUCTL_SetTitle(pIJPEGFileListMenuCtl, FLDDBG_RES_FILE, IDS_CHOOSE_FILE, NULL);
		 #else
		{
				AECHAR WTitle[40] = {0};
			(void)ISHELL_LoadResString(pme->a.m_pIShell,
		            FLDDBG_RES_FILE,                                
		            IDS_CHOOSE_FILE,
		            WTitle,
		            sizeof(WTitle));
            if(pme->m_pIAnn != NULL)
            {
			    IANNUNCIATOR_SetFieldText(pme->m_pIAnn,WTitle);
            }
		}
		#endif
         // Create file manager instance and list all the files under media directory.
         ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_FILEMGR, (void **) &pIFileMgr);
         IFILEMGR_EnumInit(pIFileMgr, MEDIA_DIR, FALSE);
         pai.wItemID = wItemID;
         while (IFILEMGR_EnumNext(pIFileMgr, &fi))
         {
            szBuf = CFieldDebug_GetFileName(fi.szName);
            STRTOWSTR(szBuf, fileNameBuf, sizeof(fileNameBuf));

            IMENUCTL_AddItem(pIJPEGFileListMenuCtl, FLDDBG_RES_FILE, 0, wItemID, fileNameBuf, 0);
            wItemID++;
         } //while
         IFILEMGR_Release(pIFileMgr);
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
       FREEIF(m_bufJPEG);
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
          ISHELL_PostEventEx(pShell, EVTFLG_ASYNC,AEECLSID_FIELDDEBUGAPP,
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

   IDISPLAY_CreateDIBitmap(pme->a.m_pIDisplay, (IDIB**)&pme->m_pIDIB, clrDepth, x, y);

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
   IDISPLAY_ClearScreen(pme->a.m_pIDisplay);
   IDISPLAY_BitBlt(pme->a.m_pIDisplay, 0, 17, pme->m_rc.dx, pme->m_rc.dy,
                   (const void *) pme->m_pIDIB, pme->x_offset,
                   pme->y_offset, AEE_RO_COPY );
   IDISPLAY_Update(pme->a.m_pIDisplay);
}
#endif /* FEATURE_JPEG_DECODER */


/*===========================================================================

FUNCTION CFieldDebug_EsnMenuHandleEvent

DESCRIPTION
  Handle the key/command event for the version sub-menu.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean CFieldDebug_EsnMenuHandleEvent(CFieldDebug *pme,
                                               AEEEvent  eCode,
                                               uint16    wParam,
                                               uint32    dwParam)
{
   PARAM_NOT_REF(dwParam)

   MSG_FATAL("CFieldDebug_EsnMenuHandleEvent Start", 0, 0, 0);
   switch (eCode) {

   case EVT_COMMAND:
      if (wParam == IDS_DONE_SK) {
         (void) CFieldDebug_MoveToDialog(pme, IDD_TOP_DIALOG);
         return TRUE;
      }
      return FALSE;

   case EVT_KEY:
      switch (wParam) {

      case AVK_CLR:
#ifdef FEATURE_CARRIER_CHINA_TELCOM 
         if (pme->m_dlgID == IDD_ESN_DIALOG)
         {
            //ISHELL_CloseApplet(pme->a.m_pIShell, FALSE); // This return the EditNum dialog
            ISHELL_CloseApplet(pme->a.m_pIShell, TRUE); // This return the Idle dialog
         }
#endif
         if (pme->m_dlgID == IDD_TOP_DIALOG)
         {
            (void) CFieldDebug_MoveToDialog(pme, IDD_TOP_DIALOG);
         }
         return TRUE;

      default:
         break;
      }
      return FALSE;

   case EVT_DIALOG_START:
      if(pme->m_pIAnn != NULL)
      {
        IANNUNCIATOR_SetHasTitleText(pme->m_pIAnn, FALSE);
        IANNUNCIATOR_Redraw(pme->m_pIAnn);
      }
      (void) CFieldDebug_OnDialogStart (pme, wParam, dwParam);
      CFieldDebug_DrawEsnScreen(pme);
      return TRUE;

   case EVT_DIALOG_END:
      return TRUE;

   default:
      break;
   }
   MSG_FATAL("CFieldDebug_EsnMenuHandleEvent End", 0, 0, 0);
   return FALSE;
}



/*===========================================================================
FUNCTION CFieldDebug_DrawEsnScreen

DESCRIPTION


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
/===========================================================================*/
static void CFieldDebug_DrawEsnScreen(CFieldDebug * pme)
{
    AECHAR szBuf[64];
    IDialog *p_dlg;
    IStatic *p_stk;
    int n = 0;
	int tn = 0;
    uint32 esn;
    //dword date = 0;
    AECHAR fmt_str[20];
	char fmt_tstr[20];
	char tstrbuf[20];
	char temp = 0;
    //int i, j, count;
    AECHAR  sTitle[45]; 
    int ret = 0;
    uint64 meid = 0;
    uint32 H32,L32;
	int i = 0;
    
	MSG_FATAL("CFieldDebug_DrawEsnScreen Start", 0, 0, 0);
    MEMSET (sTitle, 0, sizeof(sTitle));

    (void)MEMSET( szBuf,(AECHAR) 0, sizeof(szBuf)); 
    
#if !defined (FEATURE_VERSION_K212_VHOPE) && !defined (FEATURE_VERSION_K232_Y101)
    ret = ISHELL_LoadResString(pme->a.m_pIShell,
                               FLDDBG_RES_FILE,
                               IDS_ESN,
                               (szBuf + n),
                               sizeof(szBuf));
    MSG_FATAL("CFieldDebug_DrawEsnScreen 1 ret=%d", ret, 0, 0);
    n = WSTRLEN(szBuf);
    szBuf[n++] = (AECHAR) '\n';
    
    //(void) ICONFIG_GetItem(pme->m_pIConfig,
    //                      CFGI_ESN,
    //                      &esn,
    //                      sizeof(esn));//插了卡时，从卡中读取
	{
		extern int OEM_ReadESN(uint32 *pESN);
    	OEM_ReadESN(&esn);//永远从NV中读取
	}
    STRTOWSTR("%u %u", fmt_str, sizeof(fmt_str));
    
 #if 0
    WSPRINTF((szBuf + n),
            sizeof(szBuf),
            fmt_str,
            ((esn & 0xFF000000) >> 24),
            (esn & 0x00FFFFFF));
    
    n = WSTRLEN(szBuf);
    szBuf[n++] = (AECHAR) '\n';
#endif

    //Display ESN with hexadecimal
    STRTOWSTR("%08X", fmt_str, sizeof(fmt_str));
    WSPRINTF((szBuf + n),
            sizeof(szBuf),
            fmt_str,
            esn);
   
    n = WSTRLEN(szBuf);
    szBuf[n++] = (AECHAR) '\n';
#else
    n = WSTRLEN(szBuf);
    szBuf[n++] = (AECHAR) '\n';
#endif
    (void) ISHELL_LoadResString(pme->a.m_pIShell,
                               FLDDBG_RES_FILE,
                               IDS_STRING_MEID,
                               (szBuf + n),
                               sizeof(szBuf));
    n = WSTRLEN(szBuf);
    szBuf[n++] = (AECHAR) '\n';
    {
    	
        #ifdef FEATURE_VERSION_W515V3
        extern int OEM_ReadMEID(uint64 *meid);
    	OEM_ReadMEID(&meid);
        #else
        tmc_get_stored_meid_me(&meid);
        #endif
    	
        L32 = (uint32)meid;
        H32 = (((uint32)(meid>>32))&(0xffffff));
    }
    
    STRTOWSTR("%06X", fmt_str, sizeof(fmt_str));
    WSPRINTF((szBuf + n),
            sizeof(szBuf),
            fmt_str,
            H32
            );
    n = WSTRLEN(szBuf);
    STRTOWSTR("%08X", fmt_str, sizeof(fmt_str));
    WSPRINTF((szBuf + n),
            sizeof(szBuf),
            fmt_str,
            L32
            );
    for(i=0;i<100;i++)
    {
    	;
    }
	#if defined(FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)|| defined(FEATURE_VERSION_K232_Y100A)
	STRCPY(fmt_tstr, "%06X");
    SPRINTF((tstrbuf),
            fmt_tstr,
            H32
            );
    tn = STRLEN(tstrbuf);
    STRCPY(fmt_tstr, "%08X");
    SPRINTF((tstrbuf + tn),
            fmt_tstr,
            L32);
   tn = STRLEN(tstrbuf);
#ifndef FEATURE_VERSION_C260_IC19
   if (tn>13)
   {
   temp = CalcMEIDDSP(tstrbuf);
   	switch(temp)
   	{
   	  case 0:
	  	WSTRCAT(szBuf,(AECHAR*)L"0");
		break;
	  case 1:
	  	WSTRCAT(szBuf,(AECHAR*)L"1");
		break;
	  case 2:
	  	WSTRCAT(szBuf,(AECHAR*)L"2");
		break;
	  case 3:
	  	WSTRCAT(szBuf,(AECHAR*)L"3");
		break;
	  case 4:
	  	WSTRCAT(szBuf,(AECHAR*)L"4");
		break;
	  case 5:
	  	WSTRCAT(szBuf,(AECHAR*)L"5");
		break;
	  case 6:
	  	WSTRCAT(szBuf,(AECHAR*)L"6");
		break;
	  case 7:
	  	WSTRCAT(szBuf,(AECHAR*)L"7");
		break;
	  case 8:
	  	WSTRCAT(szBuf,(AECHAR*)L"8");
		break;
	  case 9:
	  	WSTRCAT(szBuf,(AECHAR*)L"9");
		break;
	  case 10:
	  	WSTRCAT(szBuf,(AECHAR*)L"A");
		break;
	  case 11:
	  	WSTRCAT(szBuf,(AECHAR*)L"B");
		break;
	  case 12:
	  	WSTRCAT(szBuf,(AECHAR*)L"C");
		break;
	  case 13:
	  	WSTRCAT(szBuf,(AECHAR*)L"D");
		break;
	  case 14:
	  	WSTRCAT(szBuf,(AECHAR*)L"E");
		break;
	  case 15:
	  	WSTRCAT(szBuf,(AECHAR*)L"F");
		break;
	  default:
	  	break;
   	}
   }
#endif   
   #endif

   
 #if defined(FEATURE_VERSION_K212_20D)|| defined(FEATURE_VERSION_K212_VHOPE)|| defined(FEATURE_VERSION_K232_JST)||defined(FEATURE_VERSION_K232_HYWY)
	STRCPY(fmt_tstr, "%06X");
    SPRINTF((tstrbuf),
            fmt_tstr,
            H32
            );
    tn = STRLEN(tstrbuf);
    STRCPY(fmt_tstr, "%08X");
    SPRINTF((tstrbuf + tn),
            fmt_tstr,
            L32);
   tn = STRLEN(tstrbuf);
   if (tn>13)
   {
   temp = CalcMEIDHex(tstrbuf);
   	switch(temp)
   	{
   	  case 0:
	  	WSTRCAT(szBuf,(AECHAR*)L"0");
		break;
	  case 1:
	  	WSTRCAT(szBuf,(AECHAR*)L"1");
		break;
	  case 2:
	  	WSTRCAT(szBuf,(AECHAR*)L"2");
		break;
	  case 3:
	  	WSTRCAT(szBuf,(AECHAR*)L"3");
		break;
	  case 4:
	  	WSTRCAT(szBuf,(AECHAR*)L"4");
		break;
	  case 5:
	  	WSTRCAT(szBuf,(AECHAR*)L"5");
		break;
	  case 6:
	  	WSTRCAT(szBuf,(AECHAR*)L"6");
		break;
	  case 7:
	  	WSTRCAT(szBuf,(AECHAR*)L"7");
		break;
	  case 8:
	  	WSTRCAT(szBuf,(AECHAR*)L"8");
		break;
	  case 9:
	  	WSTRCAT(szBuf,(AECHAR*)L"9");
		break;
	  case 10:
	  	WSTRCAT(szBuf,(AECHAR*)L"A");
		break;
	  case 11:
	  	WSTRCAT(szBuf,(AECHAR*)L"B");
		break;
	  case 12:
	  	WSTRCAT(szBuf,(AECHAR*)L"C");
		break;
	  case 13:
	  	WSTRCAT(szBuf,(AECHAR*)L"D");
		break;
	  case 14:
	  	WSTRCAT(szBuf,(AECHAR*)L"E");
		break;
	  case 15:
	  	WSTRCAT(szBuf,(AECHAR*)L"F");
		break;
	  default:
	  	break;
   	}
   }
#endif
   p_dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
   p_stk = (IStatic *) IDIALOG_GetControl(p_dlg, IDC_ESN_STATIC);

   if(p_stk == NULL)
   {
       MSG_FATAL("CFieldDebug_DrawEsnScreen 2 p_stk == NULL", 0, 0, 0);
   }
   #ifdef FEATURE_VERSION_K212_VHOPE
   {
            AEERect prc = {0};
            ISTATIC_GetRect(p_stk,&prc);
            prc.x = prc.x+5;
            ISTATIC_SetRect(p_stk,&prc); 
   }
   #endif
   // Set the values of the title and text strings for this control
   ISTATIC_SetProperties(p_stk, ST_UNDERLINE|ST_NOSCROLL|ST_CENTERTITLE);
   (void) ISTATIC_SetText(p_stk,
                          NULL,
                          szBuf,
                          AEE_FONT_NORMAL,
                          AEE_FONT_NORMAL);
   
   (void) ISTATIC_Redraw(p_stk);
   MSG_FATAL("CFieldDebug_DrawEsnScreen End", 0, 0, 0);
}

static boolean CFieldDebug_NET_LOCK_HandleEvent(CFieldDebug *pme,
                                           AEEEvent  eCode,
                                           uint16    wParam,
                                           uint32    dwParam)
{
    MSG_FATAL("CFieldDebug_NET_LOCK_HandleEvent Start", 0,0,0);
    switch (eCode) 
    {
        case EVT_COMMAND:
            switch (wParam) 
            {
                case IDS_STRING_NET_LOCK_FLAG:
                {
                    IMenuCtl *ctl;
                    IDialog *dlg;
                    MSG_FATAL ("IDS_STRING_NET_LOCK_FLAG 1", 0, 0, 0);
                    dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
                    ctl = (IMenuCtl *) IDIALOG_GetControl(dlg, IDC_NET_LOCK_MENU);
                    pme->m_debugmenu_sel = IMENUCTL_GetSel(ctl);
                    MSG_FATAL ("IDS_STRING_NET_LOCK_FLAG 2", 0, 0, 0);
                    (void) CFieldDebug_MoveToDialog(pme, IDD_LOCK_DIALOG);
                    return TRUE;
                }

                case IDS_STRING_NET_LOCK_MNC:
                {
                    IMenuCtl *ctl;
                    IDialog *dlg;

                    dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
                    ctl = (IMenuCtl *) IDIALOG_GetControl(dlg, IDC_NET_LOCK_MENU);
                    pme->m_debugmenu_sel = IMENUCTL_GetSel(ctl);
                    (void) CFieldDebug_MoveToDialog(pme, IDD_MNC_DIALOG);								
                    return TRUE;
                }
                
                case IDS_STRING_NET_LOCK_MCC:
                {
                    IMenuCtl *ctl;
                    IDialog *dlg;

                    dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
                    ctl = (IMenuCtl *) IDIALOG_GetControl(dlg, IDC_NET_LOCK_MENU);
                    pme->m_debugmenu_sel = IMENUCTL_GetSel(ctl);
                    (void) CFieldDebug_MoveToDialog(pme, IDD_MCC_DIALOG);									
                    return TRUE;
                }

                case IDS_STRING_NET_LOCK_SID:
                {
                    IMenuCtl *ctl;
                    IDialog *dlg;

                    dlg = ISHELL_GetActiveDialog(pme->a.m_pIShell);
                    ctl = (IMenuCtl *) IDIALOG_GetControl(dlg, IDC_NET_LOCK_MENU);
                    pme->m_debugmenu_sel = IMENUCTL_GetSel(ctl);
                    (void) CFieldDebug_MoveToDialog(pme, IDD_SID_DIALOG);									
                    return TRUE;
                } 
                    
                default:
                break;
            }
            break;
        case EVT_KEY:
            switch (wParam) 
            {
                case AVK_CLR:
                    (void) CFieldDebug_MoveToDialog(pme, IDD_TOP_DIALOG);
                    return TRUE;
                default:
                break;
            }
        break;
        case EVT_DIALOG_START:
            MSG_FATAL ("EVT_DIALOG_START wParam == IDS_STRING_NET_LOCK", 0, 0, 0);
            CFieldDebug_OnDialogStart (pme, wParam, dwParam);
            return TRUE;
        case EVT_DIALOG_END:
            return TRUE;
        default:
            break;
    }
    return FALSE;
}

static boolean CFieldDebug_Enabled_Lock_HandleEvent(CFieldDebug *pme,
											   AEEEvent  eCode,
											   uint16	 wParam,
											   uint32	 dwParam)
{
    ITextCtl *pIText = NULL; 
    IDialog * p_diag = NULL;
    //IMenuCtl *psk = NULL;
	BottomBar_Param_type BarParam={0};//yangdecai
    PARAM_NOT_REF(dwParam)

    MSG_FATAL("CFieldDebug_Enabled_Lock_HandleEvent Start", 0, 0, 0);
    switch (eCode) 
    {
        case EVT_COMMAND:
        {
            if(wParam == IDS_OK)
            {
                AECHAR pwstrText[2] = {0};
                char string[2];
                word enabledFlag = 0;
                p_diag = ISHELL_GetActiveDialog(pme->a.m_pIShell);
             //   (void) ICONFIG_GetItem(pme->m_pIConfig,
             //   CFGI_NET_LOCK_ENABLED,
              //  &enabledFlag,
             //   sizeof(uint8));
                pIText = (ITextCtl*)IDIALOG_GetControl(p_diag, IDC_TEXT_LOCK_FLAG);
                ITEXTCTL_SetActive(pIText,TRUE);
                (void)ITEXTCTL_GetText(pIText,pwstrText,2);
                (void) WSTRTOSTR(pwstrText, string, sizeof(string));
                enabledFlag= (word)ATOI(string);
                (void) ICONFIG_SetItem(pme->m_pIConfig,
                CFGI_NET_LOCK_ENABLED,
                &enabledFlag,
                sizeof(word));
                (void) CFieldDebug_MoveToDialog(pme, IDD_NET_LOCK_DIALOG);
                return TRUE;    
            }
            if(wParam == IDS_CANCEL)
            {
                (void) CFieldDebug_MoveToDialog(pme, IDD_NET_LOCK_DIALOG);
                return TRUE; 
            }
            return FALSE;
        }
        case EVT_KEY:
        {
            switch (wParam) 
            {
                case AVK_CLR:
                {
                    //if (pme->m_dlgID == IDD_NET_LOCK_DIALOG)
                    {
                    (void) CFieldDebug_MoveToDialog(pme, IDD_NET_LOCK_DIALOG);
                    }
                    return TRUE;
                }
                case AVK_SELECT:
                case AVK_INFO:
                {
                    AECHAR pwstrText[2] = {0};
		            char string[2];
		            word enabledFlag = 0;
		            p_diag = ISHELL_GetActiveDialog(pme->a.m_pIShell);
		         //   (void) ICONFIG_GetItem(pme->m_pIConfig,
		         //   CFGI_NET_LOCK_ENABLED,
		          //  &enabledFlag,
		         //   sizeof(uint8));
		            pIText = (ITextCtl*)IDIALOG_GetControl(p_diag, IDC_TEXT_LOCK_FLAG);
		            ITEXTCTL_SetActive(pIText,TRUE);
		            (void)ITEXTCTL_GetText(pIText,pwstrText,2);
		            (void) WSTRTOSTR(pwstrText, string, sizeof(string));
		            enabledFlag= (word)ATOI(string);
		            (void) ICONFIG_SetItem(pme->m_pIConfig,
		            CFGI_NET_LOCK_ENABLED,
		            &enabledFlag,
		            sizeof(word));
		            (void) CFieldDebug_MoveToDialog(pme, IDD_NET_LOCK_DIALOG);
		            return TRUE;       
                }
                break;
                default:
                    break;
            }
            return FALSE;
        }
        case EVT_DIALOG_START:
        {
            char szBuf[2] = {0};
            int n = 0;
            word enabledFlag = 0;
            AECHAR string[2]={0};
			pme->m_CurrId =(uint32)ENABLELOCK;
            pme->m_pActiveIDlg = (IDialog*)dwParam;
            //(void) CFieldDebug_OnDialogStart (pme, wParam, dwParam);
            pIText = (ITextCtl*)IDIALOG_GetControl((IDialog *) dwParam, IDC_TEXT_LOCK_FLAG);
            (void) ICONFIG_GetItem(pme->m_pIConfig,
            CFGI_NET_LOCK_ENABLED,
            &enabledFlag,
            sizeof(word));
            SPRINTF(szBuf,"%d\0", enabledFlag);
            STRTOWSTR(szBuf, string, NV_MAX_SID_LOCK); 
            n = WSTRLEN(string);
            ITEXTCTL_SetProperties(pIText, TP_GRAPHIC_BG|TP_FRAME | TP_MULTILINE | TP_STARKEY_SWITCH | TP_DISPLAY_COUNT | TP_DISPLAY_SMSCOUNT | TP_NOUPDATE|TP_FOCUS_NOSEL);
			SetTextControlRect(pme,pIText);
            if (NULL != szBuf)
            {
                {
                    AECHAR WTitle[30] = {0};
                    (void)ISHELL_LoadResString(pme->a.m_pIShell,
                    FLDDBG_RES_FILE,                                
                    IDS_STRING_NET_LOCK_FLAG,
                    WTitle,
                    sizeof(WTitle));
                    if(pme->m_pIAnn != NULL)
                    {
                        IANNUNCIATOR_SetFieldText(pme->m_pIAnn,WTitle);
                    }
                }     
                ITEXTCTL_SetMaxSize ( pIText, 1);
				ITEXTCTL_SetInputMode(pIText,AEE_TM_NUMBERS);
                (void)ITEXTCTL_SetText(pIText,string,-1);
                ITEXTCTL_SetCursorPos(pIText, n+1);
				(void) ISHELL_PostEventEx(pme->a.m_pIShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_FIELDDEBUGAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
            }  
            return TRUE;
        }
		case EVT_USER_REDRAW:
	   		{

                #ifndef FEATURE_ALL_KEY_PAD
                // Option     Delete
                BarParam.eBBarType = BTBAR_OK_DELETE;
                #else
                // Option     Back
                BarParam.eBBarType = BTBAR_OK_BACK;
                #endif
				DrawBottomBar(pme->m_pDisplay,&BarParam);
            	IDISPLAY_Update(pme->m_pDisplay);  
		    }
	        break;
        case EVT_DIALOG_END:
            return TRUE;
        default:
            break;
    }
    MSG_FATAL("CFieldDebug_Enabled_Lock_HandleEvent End", 0, 0, 0);
    return FALSE;
}

static boolean CFieldDebug_MNC_HandleEvent(CFieldDebug *pme,
											   AEEEvent  eCode,
											   uint16	 wParam,
											   uint32	 dwParam)
{
    ITextCtl *pIText = NULL; 
    IDialog * p_diag = NULL;
    //IMenuCtl *psk = NULL;
	BottomBar_Param_type BarParam={0};//yangdecai
    PARAM_NOT_REF(dwParam)

    MSG_FATAL("CFieldDebug_MNC_HandleEvent Start", 0, 0, 0);
    switch (eCode) 
    {
        case EVT_COMMAND:
        {
            MSG_FATAL("mnc:EVT_COMMAND",0,0,0);
            if(wParam == IDS_OK)
            {
                AECHAR pwstrText[30] = {0};
                char string[30];
                word mnc[NV_MAX_SID_LOCK] = {0};
                int i, j, k = 0;
                int total = 0;
                int step = 0, temp;
                p_diag = ISHELL_GetActiveDialog(pme->a.m_pIShell);
                pIText = (ITextCtl*)IDIALOG_GetControl(p_diag, IDC_TEXT_MNC);
                ITEXTCTL_SetActive(pIText,TRUE);
                (void)ITEXTCTL_GetText(pIText,pwstrText,30);
                (void) WSTRTOSTR(pwstrText, string, sizeof(string));
                MSG_FATAL("mnc:text=%s, length=%d",string,STRLEN(string),0);
                temp = string[0];
                for(i = 0; i < STRLEN(string); i++)
                {
                    MSG_FATAL("mnc::string[%d]=%d",i,string[i],0);
                    if(((string[i] < 48) || (string[i] > 57)) || (i == STRLEN(string)-1))//输入的不是数字时就表示一个MNC
                    {
                        if(k >= NV_MAX_SID_LOCK)
                        {
                            break;
                        }
                        if((i == STRLEN(string)-1) && ((string[i] >= 48) && (string[i] <= 57)))
                        {
                            step++;
                        }
                        MSG_FATAL("mnc:::::::3333333333:::::::::step=%d",step,0,0);
                        for(j = 0; j < step; j++)
                        {
                            if((i == STRLEN(string)-1) && ((string[i] >= 48) && (string[i] <= 57)))
                            {
                                total += (string[i-j] - '0')*FPOW(10, j);
                            }
                            else
                            {
                                total += (string[i-j-1] - '0')*FPOW(10, j);
                            }
                        }
                        if((temp >= 48) && (temp <= 57))
                        {
                            //如果当前输入项是非数字，且上一个也是非数字，
                            //则不将它统计到mnc中，否则则保存
                            MSG_FATAL("mnc:::::::555555:::::::::mnc[%d]=%d",k,total,0);
                            mnc[k++] = total;
                        }
                        else if ((step ==1) && (i == STRLEN(string)-1) && ((string[i] >= 48) || (string[i] <= 57)))
                        {
                            //当前项是最后一位数字，且当前项是一位数时，要保存起来
                            //上面的IF语句处理不了这种情况
                            MSG_FATAL("mnc:::::::5555552:::::::::mnc[%d]=%d",k,total,0);
                            mnc[k++] = total;                            
                        }
                        total = 0;
                        step = 0;
                        temp = string[i];
                        continue;
                    }
                    temp = string[i];
                    step++;
                }
                MSG_FATAL("mnc:::::::666666:::::::::k=%d",k,0,0);
                if(k < NV_MAX_SID_LOCK)
                {
                    for(i = k; i < NV_MAX_SID_LOCK; i++)
                    {
                        MSG_FATAL("mnc:::::::777777:::::::::i=%d, k=%d",i,k,0);
                        mnc[i] = 'a';
                    }
                }
                (void) ICONFIG_SetItem(pme->m_pIConfig,
                CFGI_NET_LOCK_MNC,
                &mnc,
                sizeof(word));
                (void) CFieldDebug_MoveToDialog(pme, IDD_NET_LOCK_DIALOG);
                return TRUE;    
            }
            if(wParam == IDS_CANCEL)
            {
                (void) CFieldDebug_MoveToDialog(pme, IDD_NET_LOCK_DIALOG);
                return TRUE; 
            }
            return FALSE;
        }
        case EVT_KEY:
        {
            switch (wParam) 
            {
                case AVK_CLR:
                {
                    //if (pme->m_dlgID == IDD_NET_LOCK_DIALOG)
                    {
                    (void) CFieldDebug_MoveToDialog(pme, IDD_NET_LOCK_DIALOG);
                    }
                    return TRUE;
                }
                case AVK_SELECT:
                case AVK_INFO:
                {
                    AECHAR pwstrText[30] = {0};
	                char string[30];
	                word mnc[NV_MAX_SID_LOCK] = {0};
	                int i, j, k = 0;
	                int total = 0;
	                int step = 0, temp;
	                p_diag = ISHELL_GetActiveDialog(pme->a.m_pIShell);
	                pIText = (ITextCtl*)IDIALOG_GetControl(p_diag, IDC_TEXT_MNC);
	                ITEXTCTL_SetActive(pIText,TRUE);
	                (void)ITEXTCTL_GetText(pIText,pwstrText,30);
	                (void) WSTRTOSTR(pwstrText, string, sizeof(string));
	                MSG_FATAL("mnc:text=%s, length=%d",string,STRLEN(string),0);
	                temp = string[0];
	                for(i = 0; i < STRLEN(string); i++)
	                {
	                    MSG_FATAL("mnc::string[%d]=%d",i,string[i],0);
	                    if(((string[i] < 48) || (string[i] > 57)) || (i == STRLEN(string)-1))//输入的不是数字时就表示一个MNC
	                    {
	                        if(k >= NV_MAX_SID_LOCK)
	                        {
	                            break;
	                        }
	                        if((i == STRLEN(string)-1) && ((string[i] >= 48) && (string[i] <= 57)))
	                        {
	                            step++;
	                        }
	                        MSG_FATAL("mnc:::::::3333333333:::::::::step=%d",step,0,0);
	                        for(j = 0; j < step; j++)
	                        {
	                            if((i == STRLEN(string)-1) && ((string[i] >= 48) && (string[i] <= 57)))
	                            {
	                                total += (string[i-j] - '0')*FPOW(10, j);
	                            }
	                            else
	                            {
	                                total += (string[i-j-1] - '0')*FPOW(10, j);
	                            }
	                        }
	                        if((temp >= 48) && (temp <= 57))
	                        {
	                            //如果当前输入项是非数字，且上一个也是非数字，
	                            //则不将它统计到mnc中，否则则保存
	                            MSG_FATAL("mnc:::::::555555:::::::::mnc[%d]=%d",k,total,0);
	                            mnc[k++] = total;
	                        }
	                        else if ((step ==1) && (i == STRLEN(string)-1) && ((string[i] >= 48) || (string[i] <= 57)))
	                        {
	                            //当前项是最后一位数字，且当前项是一位数时，要保存起来
	                            //上面的IF语句处理不了这种情况
	                            MSG_FATAL("mnc:::::::5555552:::::::::mnc[%d]=%d",k,total,0);
	                            mnc[k++] = total;                            
	                        }
	                        total = 0;
	                        step = 0;
	                        temp = string[i];
	                        continue;
	                    }
	                    temp = string[i];
	                    step++;
	                }
	                MSG_FATAL("mnc:::::::666666:::::::::k=%d",k,0,0);
	                if(k < NV_MAX_SID_LOCK)
	                {
	                    for(i = k; i < NV_MAX_SID_LOCK; i++)
	                    {
	                        MSG_FATAL("mnc:::::::777777:::::::::i=%d, k=%d",i,k,0);
	                        mnc[i] = 'a';
	                    }
	                }
	                (void) ICONFIG_SetItem(pme->m_pIConfig,
	                CFGI_NET_LOCK_MNC,
	                &mnc,
	                sizeof(word));
	                (void) CFieldDebug_MoveToDialog(pme, IDD_NET_LOCK_DIALOG); 
					return TRUE;
                }
                break;
                default:
                    break;
            }
            return FALSE;
        }
        case EVT_DIALOG_START:
        {
            //char szBuf[30] = {0};
            int n = 0;
            word mnc[NV_MAX_SID_LOCK] = {0};
            AECHAR string[30]={0};
            int i;
			pme->m_CurrId =(uint32)MNC;
            pme->m_pActiveIDlg = (IDialog*)dwParam;
            //(void) CFieldDebug_OnDialogStart (pme, wParam, dwParam);
            pIText = (ITextCtl*)IDIALOG_GetControl((IDialog *) dwParam, IDC_TEXT_MNC);
            (void) ICONFIG_GetItem(pme->m_pIConfig,CFGI_NET_LOCK_MNC,&mnc,sizeof(word));
            for(i = 0; i < NV_MAX_SID_LOCK; i++)
            {
                MSG_FATAL("enabledFlag::::::00000000::::::::::mnc[%d]=%d",i,mnc[i],0);  
                if(mnc[i] != 'a')
                {
                    n = WSTRLEN(string);
                    MSG_FATAL("enabledFlag::::::00000000::::::::::i=%d,WSTRLEN(string)=%d",i,n,0);
                    WSPRINTF( string+n, 30, L"%d.", mnc[i]);  
                }
            }
            n = WSTRLEN(string);
            ITEXTCTL_SetProperties(pIText, TP_GRAPHIC_BG|TP_FRAME | TP_MULTILINE | TP_STARKEY_SWITCH | TP_DISPLAY_COUNT | TP_DISPLAY_SMSCOUNT | TP_NOUPDATE|TP_FOCUS_NOSEL);
			SetTextControlRect(pme,pIText);
            ITEXTCTL_SetMaxSize ( pIText, 30);
			ITEXTCTL_SetInputMode(pIText,AEE_TM_NUMBERS);
            (void)ITEXTCTL_SetText(pIText,string,-1);
            ITEXTCTL_SetCursorPos(pIText, n+1);
            {
                AECHAR WTitle[5] = {0};
                (void)ISHELL_LoadResString(pme->a.m_pIShell,
                FLDDBG_RES_FILE,                                
                IDS_STRING_NET_LOCK_MNC,
                WTitle,
                sizeof(WTitle));
                if(pme->m_pIAnn != NULL)
                {
                    IANNUNCIATOR_SetFieldText(pme->m_pIAnn,WTitle);
                }
            }     
			(void) ISHELL_PostEventEx(pme->a.m_pIShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_FIELDDEBUGAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
            return TRUE;
        }
		case EVT_USER_REDRAW:
	   		{
                #ifndef FEATURE_ALL_KEY_PAD
                // Option     Delete
                BarParam.eBBarType = BTBAR_OK_DELETE;
                #else
                // Option     Back
                BarParam.eBBarType = BTBAR_OK_BACK;
                #endif
				DrawBottomBar(pme->m_pDisplay,&BarParam);
            	IDISPLAY_Update(pme->m_pDisplay);  
		    }
	        break;
        case EVT_DIALOG_END:
            return TRUE;
        default:
            break;
    }
    MSG_FATAL("CFieldDebug_MNC_HandleEvent End", 0, 0, 0);
    return FALSE;
}

static boolean CFieldDebug_MCC_HandleEvent(CFieldDebug *pme,
											   AEEEvent  eCode,
											   uint16	 wParam,
											   uint32	 dwParam)
{
    ITextCtl *pIText = NULL; 
    IDialog * p_diag = NULL;
    //IMenuCtl *psk = NULL;
	BottomBar_Param_type BarParam={0};//yangdecai
    PARAM_NOT_REF(dwParam)

    MSG_FATAL("CFieldDebug_MCC_HandleEvent Start", 0, 0, 0);
    switch (eCode) 
    {
        case EVT_COMMAND:
        {
            if(wParam == IDS_OK)
            {
                AECHAR pwstrText[30] = {0};
                char string[30];
                word mcc[NV_MAX_SID_LOCK] = {0};
                int i, j, k = 0;
                int total = 0;
                int step = 0, temp;
                p_diag = ISHELL_GetActiveDialog(pme->a.m_pIShell);
                pIText = (ITextCtl*)IDIALOG_GetControl(p_diag, IDC_TEXT_MCC);
                ITEXTCTL_SetActive(pIText,TRUE);
                (void)ITEXTCTL_GetText(pIText,pwstrText,30);
                (void) WSTRTOSTR(pwstrText, string, sizeof(string));
                temp = string[0];
                for(i = 0; i < STRLEN(string); i++)
                {
                    MSG_FATAL("mcc::string[%d]=%d",i,string[i],0);
                    if(((string[i] < 48) || (string[i] > 57)) || (i == STRLEN(string)-1))//输入的不是数字时就表示一个MNC
                    {
                        if(k >= NV_MAX_SID_LOCK)
                        {
                            break;
                        }
                        if((i == STRLEN(string)-1) && ((string[i] >= 48) && (string[i] <= 57)))
                        {
                            step++;
                        }
                        MSG_FATAL("mcc:::::::3333333333:::::::::step=%d",step,0,0);
                        for(j = 0; j < step; j++)
                        {
                            if((i == STRLEN(string)-1) && ((string[i] >= 48) && (string[i] <= 57)))
                            {
                                total += (string[i-j] - '0')*FPOW(10, j);
                            }
                            else
                            {
                                total += (string[i-j-1] - '0')*FPOW(10, j);
                            }
                        }
                        MSG_FATAL("mcc:::::::444444444:::::::::total=%d",total,0,0);
                        if((temp >= 48) && (temp <= 57))
                        {
                            //如果当前输入项是非数字，且上一个也是非数字，
                            //则不将它统计到mnc中
                            MSG_FATAL("mcc:::::::555555:::::::::mnc[%d]=%d",k,total,0);
                            mcc[k++] = total;
                        }
                        else if ((step ==1) && (i == STRLEN(string)-1) && ((string[i] >= 48) || (string[i] <= 57)))
                        {
                            //当前项是最后一位数字，且当前项是一位数时，要保存起来
                            //上面的IF语句处理不了这种情况
                            MSG_FATAL("mcc:::::::555555:::::::::mcc[%d]=%d",k,total,0);
                            mcc[k++] = total;                            
                        }                        
                        total = 0;
                        step = 0;
                        temp = string[i];
                        continue;
                    }
                    temp = string[i];
                    step++;
                }
                if(k < NV_MAX_SID_LOCK)
                {
                    for(i = k; i < NV_MAX_SID_LOCK; i++)
                    {
                        mcc[i] = 'a';
                    }
                }

                (void) ICONFIG_SetItem(pme->m_pIConfig,
                CFGI_NET_LOCK_MCC,
                &mcc,
                sizeof(word));
                (void) CFieldDebug_MoveToDialog(pme, IDD_NET_LOCK_DIALOG);
                return TRUE;    
            }
            if(wParam == IDS_CANCEL)
            {
                (void) CFieldDebug_MoveToDialog(pme, IDD_NET_LOCK_DIALOG);
                return TRUE; 
            }
            return FALSE;
        }
        case EVT_KEY:
        {
            switch (wParam) 
            {
                case AVK_CLR:
                {
                    //if (pme->m_dlgID == IDD_NET_LOCK_DIALOG)
                    {
                    (void) CFieldDebug_MoveToDialog(pme, IDD_NET_LOCK_DIALOG);
                    }
                    return TRUE;
                }
                case AVK_SELECT:
                case AVK_INFO:
                {
                    AECHAR pwstrText[30] = {0};
	                char string[30];
	                word mcc[NV_MAX_SID_LOCK] = {0};
	                int i, j, k = 0;
	                int total = 0;
	                int step = 0, temp;
	                p_diag = ISHELL_GetActiveDialog(pme->a.m_pIShell);
	                pIText = (ITextCtl*)IDIALOG_GetControl(p_diag, IDC_TEXT_MCC);
	                ITEXTCTL_SetActive(pIText,TRUE);
	                (void)ITEXTCTL_GetText(pIText,pwstrText,30);
	                (void) WSTRTOSTR(pwstrText, string, sizeof(string));
	                temp = string[0];
	                for(i = 0; i < STRLEN(string); i++)
	                {
	                    MSG_FATAL("mcc::string[%d]=%d",i,string[i],0);
	                    if(((string[i] < 48) || (string[i] > 57)) || (i == STRLEN(string)-1))//输入的不是数字时就表示一个MNC
	                    {
	                        if(k >= NV_MAX_SID_LOCK)
	                        {
	                            break;
	                        }
	                        if((i == STRLEN(string)-1) && ((string[i] >= 48) && (string[i] <= 57)))
	                        {
	                            step++;
	                        }
	                        MSG_FATAL("mcc:::::::3333333333:::::::::step=%d",step,0,0);
	                        for(j = 0; j < step; j++)
	                        {
	                            if((i == STRLEN(string)-1) && ((string[i] >= 48) && (string[i] <= 57)))
	                            {
	                                total += (string[i-j] - '0')*FPOW(10, j);
	                            }
	                            else
	                            {
	                                total += (string[i-j-1] - '0')*FPOW(10, j);
	                            }
	                        }
	                        MSG_FATAL("mcc:::::::444444444:::::::::total=%d",total,0,0);
	                        if((temp >= 48) && (temp <= 57))
	                        {
	                            //如果当前输入项是非数字，且上一个也是非数字，
	                            //则不将它统计到mnc中
	                            MSG_FATAL("mcc:::::::555555:::::::::mnc[%d]=%d",k,total,0);
	                            mcc[k++] = total;
	                        }
	                        else if ((step ==1) && (i == STRLEN(string)-1) && ((string[i] >= 48) || (string[i] <= 57)))
	                        {
	                            //当前项是最后一位数字，且当前项是一位数时，要保存起来
	                            //上面的IF语句处理不了这种情况
	                            MSG_FATAL("mcc:::::::555555:::::::::mcc[%d]=%d",k,total,0);
	                            mcc[k++] = total;                            
	                        }                        
	                        total = 0;
	                        step = 0;
	                        temp = string[i];
	                        continue;
	                    }
	                    temp = string[i];
	                    step++;
	                }
	                if(k < NV_MAX_SID_LOCK)
	                {
	                    for(i = k; i < NV_MAX_SID_LOCK; i++)
	                    {
	                        mcc[i] = 'a';
	                    }
	                }

	                (void) ICONFIG_SetItem(pme->m_pIConfig,
	                CFGI_NET_LOCK_MCC,
	                &mcc,
	                sizeof(word));
	                (void) CFieldDebug_MoveToDialog(pme, IDD_NET_LOCK_DIALOG);
	                return TRUE;        
                }
                break;
                default:
                    break;
            }
            return FALSE;
        }
        case EVT_DIALOG_START:
        {
            //char szBuf[30] = {0};
            int n = 0;
            word mcc[NV_MAX_SID_LOCK] = {0};
            AECHAR string[30]={0};
            int i;
			pme->m_CurrId =(uint32)MCC;
            pme->m_pActiveIDlg = (IDialog*)dwParam;
            //(void) CFieldDebug_OnDialogStart (pme, wParam, dwParam);
            pIText = (ITextCtl*)IDIALOG_GetControl((IDialog *) dwParam, IDC_TEXT_MCC);
            (void) ICONFIG_GetItem(pme->m_pIConfig,CFGI_NET_LOCK_MCC,&mcc, sizeof(word));
            MSG_FATAL("enabledFlag::::::00000000::::::::::%d",mcc[0],0,0);
            for(i = 0; i < NV_MAX_SID_LOCK; i++)
            {
                MSG_FATAL("enabledFlag::::::00000000::::::::::mcc[%d]=%d",i,mcc[i],0);  
                if(mcc[i] != 'a')
                {
                    n = WSTRLEN(string);
                    MSG_FATAL("enabledFlag::::::00000000::::::::::i=%d,WSTRLEN(string)=%d",i,n,0);
                    WSPRINTF( string+n, 30, L"%d.", mcc[i]);  
                }
            }
            n = WSTRLEN(string);
            ITEXTCTL_SetProperties(pIText, TP_GRAPHIC_BG|TP_FRAME | TP_MULTILINE | TP_STARKEY_SWITCH | TP_DISPLAY_COUNT | TP_DISPLAY_SMSCOUNT | TP_NOUPDATE|TP_FOCUS_NOSEL);
			SetTextControlRect(pme,pIText);
            ITEXTCTL_SetMaxSize ( pIText, 30);
			ITEXTCTL_SetInputMode(pIText,AEE_TM_NUMBERS);
            (void)ITEXTCTL_SetText(pIText,string,-1);
            ITEXTCTL_SetCursorPos(pIText, n+1);
            {
                AECHAR WTitle[5] = {0};
                (void)ISHELL_LoadResString(pme->a.m_pIShell,
                FLDDBG_RES_FILE,                                
                IDS_STRING_NET_LOCK_MCC,
                WTitle,
                sizeof(WTitle));
                if(pme->m_pIAnn != NULL)
                {
                    IANNUNCIATOR_SetFieldText(pme->m_pIAnn,WTitle);
                }
            }     
			(void) ISHELL_PostEventEx(pme->a.m_pIShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_FIELDDEBUGAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
            return TRUE;
        }
		case EVT_USER_REDRAW:
	   		{
                #ifndef FEATURE_ALL_KEY_PAD
                // Option     Delete
                BarParam.eBBarType = BTBAR_OK_DELETE;
                #else
                // Option     Back
                BarParam.eBBarType = BTBAR_OK_BACK;
                #endif
				DrawBottomBar(pme->m_pDisplay,&BarParam);
            	IDISPLAY_Update(pme->m_pDisplay);  
		    }
	        break;
        case EVT_DIALOG_END:
            return TRUE;
        default:
            break;
    }
    MSG_FATAL("CFieldDebug_MCC_HandleEvent End", 0, 0, 0);
    return FALSE;
}

static boolean CFieldDebug_SID_HandleEvent(CFieldDebug *pme,
											   AEEEvent  eCode,
											   uint16	 wParam,
											   uint32	 dwParam)
{
    ITextCtl *pIText = NULL; 
    IDialog * p_diag = NULL;
    //IMenuCtl *psk = NULL;
	BottomBar_Param_type BarParam={0};//yangdecai
    PARAM_NOT_REF(dwParam)

    MSG_FATAL("CFieldDebug_SID_HandleEvent Start", 0, 0, 0);
    switch (eCode) 
    {
        case EVT_COMMAND:
        {
            if(wParam == IDS_OK)
            {
                AECHAR pwstrText[30] = {0};
                char string[30];
                word sid[NV_MAX_SID_LOCK] = {0};
                int i, j, k = 0;
                int total = 0;
                int step = 0, temp;
                p_diag = ISHELL_GetActiveDialog(pme->a.m_pIShell);
                pIText = (ITextCtl*)IDIALOG_GetControl(p_diag, IDC_TEXT_SID);
                ITEXTCTL_SetActive(pIText,TRUE);
                (void)ITEXTCTL_GetText(pIText,pwstrText,30);
                (void) WSTRTOSTR(pwstrText, string, sizeof(string));
                temp = string[0];
                for(i = 0; i < STRLEN(string); i++)
                {
                    MSG_FATAL("sid::string[%d]=%d",i,string[i],0);
                    if(((string[i] < 48) || (string[i] > 57)) || (i == STRLEN(string)-1))//输入的不是数字时就表示一个MNC
                    {
                        if(k >= NV_MAX_SID_LOCK)
                        {
                            break;
                        }
                        if((i == STRLEN(string)-1) && ((string[i] >= 48) && (string[i] <= 57)))
                        {
                            step++;
                        }
                        MSG_FATAL("sid:::::::3333333333:::::::::step=%d",step,0,0);
                        for(j = 0; j < step; j++)
                        {
                            if((i == STRLEN(string)-1) && ((string[i] >= 48) && (string[i] <= 57)))
                            {
                                total += (string[i-j] - '0')*FPOW(10, j);
                            }
                            else
                            {
                                total += (string[i-j-1] - '0')*FPOW(10, j);
                            }
                        }
                        MSG_FATAL("sid:::::::444444444:::::::::total=%d",total,0,0);
                        if((temp >= 48) && (temp <= 57))
                        {
                            //如果当前输入项是非数字，且上一个也是非数字，
                            //则不将它统计到mnc中
                            MSG_FATAL("mnc:::::::555555:::::::::mnc[%d]=%d",k,total,0);
                            sid[k++] = total;
                        }
                        else if ((step ==1) && (i == STRLEN(string)-1) && ((string[i] <= 48) || (string[i] <= 57)))
                        {
                            //当前项是最后一位数字，且当前项是一位数时，要保存起来
                            //上面的IF语句处理不了这种情况
                            MSG_FATAL("sid:::::::555555:::::::::sid[%d]=%d",k,total,0);
                            sid[k++] = total;                            
                        }                        
                        total = 0;
                        step = 0;
                        temp = string[i];
                        continue;
                    }
                    temp = string[i];
                    step++;
                }
                if(k < NV_MAX_SID_LOCK)
                {
                    for(i = k; i < NV_MAX_SID_LOCK; i++)
                    {
                        sid[i] = 'a';
                    }
                }
                (void) ICONFIG_SetItem(pme->m_pIConfig,
                CFGI_NET_LOCK_SID,
                &sid,
                sizeof(word));
                (void) CFieldDebug_MoveToDialog(pme, IDD_NET_LOCK_DIALOG);
                return TRUE;    
            }
            if(wParam == IDS_CANCEL)
            {
                (void) CFieldDebug_MoveToDialog(pme, IDD_NET_LOCK_DIALOG);
                return TRUE; 
            }
            return FALSE;
        }
        case EVT_KEY:
        {
            switch (wParam) 
            {
                case AVK_CLR:
                {
                    //if (pme->m_dlgID == IDD_NET_LOCK_DIALOG)
                    {
                    (void) CFieldDebug_MoveToDialog(pme, IDD_NET_LOCK_DIALOG);
                    }
                    return TRUE;
                }
                case AVK_SELECT:
                case AVK_INFO:
                {
                     AECHAR pwstrText[30] = {0};
	                char string[30];
	                word sid[NV_MAX_SID_LOCK] = {0};
	                int i, j, k = 0;
	                int total = 0;
	                int step = 0, temp;
	                p_diag = ISHELL_GetActiveDialog(pme->a.m_pIShell);
	                pIText = (ITextCtl*)IDIALOG_GetControl(p_diag, IDC_TEXT_SID);
	                ITEXTCTL_SetActive(pIText,TRUE);
	                (void)ITEXTCTL_GetText(pIText,pwstrText,30);
	                (void) WSTRTOSTR(pwstrText, string, sizeof(string));
	                temp = string[0];
	                for(i = 0; i < STRLEN(string); i++)
	                {
	                    MSG_FATAL("sid::string[%d]=%d",i,string[i],0);
	                    if(((string[i] < 48) || (string[i] > 57)) || (i == STRLEN(string)-1))//输入的不是数字时就表示一个MNC
	                    {
	                        if(k >= NV_MAX_SID_LOCK)
	                        {
	                            break;
	                        }
	                        if((i == STRLEN(string)-1) && ((string[i] >= 48) && (string[i] <= 57)))
	                        {
	                            step++;
	                        }
	                        MSG_FATAL("sid:::::::3333333333:::::::::step=%d",step,0,0);
	                        for(j = 0; j < step; j++)
	                        {
	                            if((i == STRLEN(string)-1) && ((string[i] >= 48) && (string[i] <= 57)))
	                            {
	                                total += (string[i-j] - '0')*FPOW(10, j);
	                            }
	                            else
	                            {
	                                total += (string[i-j-1] - '0')*FPOW(10, j);
	                            }
	                        }
	                        MSG_FATAL("sid:::::::444444444:::::::::total=%d",total,0,0);
	                        if((temp >= 48) && (temp <= 57))
	                        {
	                            //如果当前输入项是非数字，且上一个也是非数字，
	                            //则不将它统计到mnc中
	                            MSG_FATAL("mnc:::::::555555:::::::::mnc[%d]=%d",k,total,0);
	                            sid[k++] = total;
	                        }
	                        else if ((step ==1) && (i == STRLEN(string)-1) && ((string[i] <= 48) || (string[i] <= 57)))
	                        {
	                            //当前项是最后一位数字，且当前项是一位数时，要保存起来
	                            //上面的IF语句处理不了这种情况
	                            MSG_FATAL("sid:::::::555555:::::::::sid[%d]=%d",k,total,0);
	                            sid[k++] = total;                            
	                        }                        
	                        total = 0;
	                        step = 0;
	                        temp = string[i];
	                        continue;
	                    }
	                    temp = string[i];
	                    step++;
	                }
	                if(k < NV_MAX_SID_LOCK)
	                {
	                    for(i = k; i < NV_MAX_SID_LOCK; i++)
	                    {
	                        sid[i] = 'a';
	                    }
	                }
	                (void) ICONFIG_SetItem(pme->m_pIConfig,
	                CFGI_NET_LOCK_SID,
	                &sid,
	                sizeof(word));
	                (void) CFieldDebug_MoveToDialog(pme, IDD_NET_LOCK_DIALOG);
	                return TRUE;      
                }
                break;
                default:
                    break;
            }
            return FALSE;
        }
        case EVT_DIALOG_START:
        {
            //char szBuf[30] = {0};
            int n = 0;
            word sid[NV_MAX_SID_LOCK] = {0};
            AECHAR string[30]={0};
            int i;
			pme->m_CurrId =(uint32)SID;
            pme->m_pActiveIDlg = (IDialog*)dwParam;
            //(void) CFieldDebug_OnDialogStart (pme, wParam, dwParam);
            pIText = (ITextCtl*)IDIALOG_GetControl((IDialog *) dwParam, IDC_TEXT_SID);
            (void) ICONFIG_GetItem(pme->m_pIConfig,CFGI_NET_LOCK_SID,&sid,sizeof(word));
            for(i = 0; i < NV_MAX_SID_LOCK; i++)
            {
                MSG_FATAL("enabledFlag::::::00000000::::::::::sid[%d]=%d",i,sid[i],0);  
                if(sid[i] != 'a')
                {
                    n = WSTRLEN(string);
                    MSG_FATAL("enabledFlag::::::00000000::::::::::i=%d,WSTRLEN(string)=%d",i,n,0);
                    WSPRINTF( string+n, 30, L"%d.", sid[i]);  
                }
            }
            n = WSTRLEN(string);
            ITEXTCTL_SetProperties(pIText, TP_GRAPHIC_BG|TP_FRAME | TP_MULTILINE | TP_STARKEY_SWITCH | TP_DISPLAY_COUNT | TP_DISPLAY_SMSCOUNT | TP_NOUPDATE|TP_FOCUS_NOSEL);
			SetTextControlRect(pme,pIText);
            ITEXTCTL_SetMaxSize ( pIText, 30);
			ITEXTCTL_SetInputMode(pIText,AEE_TM_NUMBERS);
            (void)ITEXTCTL_SetText(pIText,string,-1);
            ITEXTCTL_SetCursorPos(pIText, n+1);
            {
                AECHAR WTitle[5] = {0};
                (void)ISHELL_LoadResString(pme->a.m_pIShell,
                FLDDBG_RES_FILE,                                
                IDS_STRING_NET_LOCK_SID,
                WTitle,
                sizeof(WTitle));
                if(pme->m_pIAnn != NULL)
                {
                    IANNUNCIATOR_SetFieldText(pme->m_pIAnn,WTitle);
                }
            }     
			(void) ISHELL_PostEventEx(pme->a.m_pIShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_FIELDDEBUGAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
            return TRUE;
        }
		case EVT_USER_REDRAW:
	   		{
                #ifndef FEATURE_ALL_KEY_PAD
                // Option     Delete
                BarParam.eBBarType = BTBAR_OK_DELETE;
                #else
                // Option     Back
                BarParam.eBBarType = BTBAR_OK_BACK;
                #endif
				DrawBottomBar(pme->m_pDisplay,&BarParam);
            	IDISPLAY_Update(pme->m_pDisplay);  
		    }
	        break;
        case EVT_DIALOG_END:
            return TRUE;
        default:
            break;
    }
    MSG_FATAL("CFieldDebug_SID_HandleEvent End", 0, 0, 0);
    return FALSE;
}

//add by yangdecai 
static	boolean CFieldDebug_RouteDialogEvt(CFieldDebug *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
)
{
	switch(pMe->m_CurrId)
		{
            case NVSETTING:
                CFieldDebug_NVSETTINGHandleEvent(pMe,eCode,wParam,dwParam);
                break;
			case NETLOCK:
				CFieldDebug_NET_LOCK_HandleEvent(pMe,eCode,wParam,dwParam);
				break;
			case ENABLELOCK:
				CFieldDebug_Enabled_Lock_HandleEvent(pMe,eCode,wParam,dwParam);
				break;
			case MNC:
				CFieldDebug_MNC_HandleEvent(pMe,eCode,wParam,dwParam);
				break;
			case MCC:
				CFieldDebug_MCC_HandleEvent(pMe,eCode,wParam,dwParam);
				break;
			case SID:
				CFieldDebug_SID_HandleEvent(pMe,eCode,wParam,dwParam);
				break;
			default:
				break;
		}
	return TRUE;
}

static void SetTextControlRect(CFieldDebug *pme, void  *Ctl)
{
    AEERect ctlRect;
    if (pme == NULL)
    {
        return;
    }
    SETAEERECT(&ctlRect,  0, 0,
                pme->m_screen_rc.dx,
                pme->m_screen_rc.dy - GetBottomBarHeight(pme->m_pDisplay));
    ICONTROL_SetRect((IControl*)Ctl, &ctlRect);
}

static byte CharToHex(char ch)
{
	byte Hex = 0;
	if(ch >= '0' && ch <= '9')
	{
		Hex = ch - '0';
	}
	else if(ch >= 'A' && ch <= 'F')
	{
		Hex = 10 + (ch - 'A');
	}
	else if(ch >= 'a' && ch <= 'f')
	{
		Hex =  10 + (ch - 'a');
	}
	return Hex;
}
static byte CalcMEIDDSP(char * Str)
{
	byte SP;
	byte oH = 0;
	byte oL = 0;
	char temp_str [16] = {0};
	STRCPY(temp_str,Str);
	SP  = CharToHex(temp_str[1]);
	SP  = 2*SP;
	if(SP>0)
	{
		oH += SP/10;
		oL += SP%10;
	}

	SP  = CharToHex(temp_str[3]);
	SP  = 2*SP;
	if(SP>0)
	{
		oH += SP/10;
		oL += SP%10;
	}

	SP  = CharToHex(temp_str[5]);
	SP  = 2*SP;
	if(SP>0)
	{
		oH += SP/10;
		oL += SP%10;
	}

	SP  = CharToHex(temp_str[7]);
	SP  = 2*SP;
	if(SP>0)
	{
		oH += SP/10;
		oL += SP%10;
	}

	SP  = CharToHex(temp_str[9]);
	SP  = 2*SP;
	if(SP>0)
	{
		oH += SP/10;
		oL += SP%10;
	}

	SP  = CharToHex(temp_str[11]);
	SP  = 2*SP;
	if(SP>0)
	{
		oH += SP/10;
		oL += SP%10;
	}

	SP  = CharToHex(temp_str[13]);
	SP  = 2*SP;
	if(SP>0)
	{
		oH += SP/10;
		oL += SP%10;
	}


	SP  = 0;
	SP += CharToHex(temp_str[0]);
	SP += CharToHex(temp_str[2]);
	SP += CharToHex(temp_str[4]);
	SP += CharToHex(temp_str[6]);
	SP += CharToHex(temp_str[8]);
	SP += CharToHex(temp_str[10]);
	SP += CharToHex(temp_str[12]);
	SP += oH + oL;

	if(SP%10 == 0)
	{
	  SP = 0;
	}
	else
	{
	  SP = 10 - (SP%10);
	}
	return SP;
}

static byte CalcMEIDHex(char * Str)
{
	byte SP;
	byte oH = 0;
	byte oL = 0;
	char temp_str [16] = {0};
	STRCPY(temp_str,Str);
	SP  = CharToHex(temp_str[1]);
	SP  = 2*SP;
	if(SP>0)
	{
		oH += SP/16;
		oL += SP%16;
	}

	SP  = CharToHex(temp_str[3]);
	SP  = 2*SP;
	if(SP>0)
	{
		oH += SP/16;
		oL += SP%16;
	}

	SP  = CharToHex(temp_str[5]);
	SP  = 2*SP;
	if(SP>0)
	{
		oH += SP/16;
		oL += SP%16;
	}

	SP  = CharToHex(temp_str[7]);
	SP  = 2*SP;
	if(SP>0)
	{
		oH += SP/16;
		oL += SP%16;
	}

	SP  = CharToHex(temp_str[9]);
	SP  = 2*SP;
	if(SP>0)
	{
		oH += SP/16;
		oL += SP%16;
	}

	SP  = CharToHex(temp_str[11]);
	SP  = 2*SP;
	if(SP>0)
	{
		oH += SP/16;
		oL += SP%16;
	}

	SP  = CharToHex(temp_str[13]);
	SP  = 2*SP;
	if(SP>0)
	{
		oH += SP/16;
		oL += SP%16;
	}


	SP  = 0;
	SP += CharToHex(temp_str[0]);
	SP += CharToHex(temp_str[2]);
	SP += CharToHex(temp_str[4]);
	SP += CharToHex(temp_str[6]);
	SP += CharToHex(temp_str[8]);
	SP += CharToHex(temp_str[10]);
	SP += CharToHex(temp_str[12]);
	SP += oH + oL;

	if(SP%16 == 0)
	{
	  SP = 0;
	}
	else
	{
	  SP = 16 - (SP%16);
	}
	return SP;
}


