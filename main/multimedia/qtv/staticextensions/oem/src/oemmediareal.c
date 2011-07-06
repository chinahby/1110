/*===========================================================================

FILE: OEMMediaReal.c

SERVICES: Real Network Player functionality for IMedia

DESCRIPTION
   This file implements Real Player functionality that can be used by IMedia-based
   classes.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 2003-2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
/*=========================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/staticextensions/oem/main/latest/src/oemmediareal.c#3 $
$DateTime: 2009/10/22 23:30:49 $
$Change: 1062062 $
=========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#ifdef MEASURE_CLIP_REX_AND_WINDOWS_TIME
   #include "hxtypes.h"
   #include "hxtick.h"
   #include <stdlib.h>
#endif

#ifdef FEATURE_CMI
#error code not present
#else
#include "clk.h"
#endif

#ifdef FEATURE_CMI_MM
#error code not present
#endif
#include "msg.h"
#include "BREWVersion.h"
#include "OEMMedia.h"
#include "OEMDisp.h"    /* For IBitmapCtl */
#include "OEMObjectMgr.h"
#include "OEMMediaReal.h"
#include "AEEMimeTypes.h"
#include "AEEMediaFormats.h"
#include "AEEMediaReal.h"
#include "AEEMediaMPEG4.h" // For status and parameter values common to many media types

#ifdef T_WINNT
#error code not present
#endif

#ifdef FEATURE_REAL_NV_CONFIG
#include "OEMNV.h"
#endif
#if defined (FEATURE_QTV_NET_POLICY) || defined (FEATURE_QTV_MDP_TRANSFORMATIONS)
#include "OEMMediaMPEG4.h" // For AEENetPolicyInfo
#endif
#include "OEMFeatures.h"    /* For BREW_OFFSET_X/Y */
#define OEMMEDIAREALTRANSLATION_CINTERFACE_ONLY
#define OEMMEDIAREALTRANSLATION_DEFINE_RESULT_CODES
#include "OEMMediaRealTranslation.h"

#include "snd.h"

#include "mobile.h"

#if MIN_BREW_VERSION(3,0)
# include "AEE_OEMHeap.h"
# include "OEMHeap.h"
# include "OEMCriticalSection.h"
# include "AEE_OEMDispatch.h"
#endif /* MIN_BREW_VERSION(3,0) */

#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
#include "mdp_drv_api.h"
#endif /* FEATURE_QTV_MDP_TRANSFORMATIONS */

#if defined (FEATURE_ACM)
# include "OEMACM.h"
#endif

#if defined (FEATURE_ODM_UI) && defined(WOLF_5)
#error code not present
#endif// (FEATURE_ODM_UI) && defined(WOLF_5)

#if MIN_BREW_VERSION(3,0)
static OEMCriticalSection gcmCriticalSection;
# define MUTEX_LOCK() OEMCriticalSection_Enter(&gcmCriticalSection)
# define MUTEX_UNLOCK() OEMCriticalSection_Leave(&gcmCriticalSection)
#else /* MIN_BREW_VERSION(3,0) */
# define MUTEX_LOCK() OEMOS_MutexStart(); {
# define MUTEX_UNLOCK() } OEMOS_MutexStop();
#endif /* MIN_BREW_VERSION(3,0) */

#if MIN_BREW_VERSION(4,0)
#include "AEEIBitmapCtl.h"
#define AEEIID_BITMAPCTL AEEIID_IBitmapCtl
#define IBITMAPCTL_AppToScreen IBitmapCtl_AppToScreen
#define IBITMAPCTL_Release IBitmapCtl_Release
#endif

#define VOLUME_STEP  25
#define AUDIO_LAG_MS 20
#define CONNECT_ATTEMPTS 1
/*==============================================================================

                               MACRO DEFINITIONS

==============================================================================*/
#define OEMREAL_MAX_CB 100
#define OEMREAL_PROGRESS_UPDATE_INTERVAL_MSEC 250
#define OEMREAL_BUFFER_COUNT 2
// NetApp NetCache servers will optimize playback from RealMedia clients if
// the User-Agent string begins with "RealMedia Player". If you do not need
// these optimizations, or do not intend to stream from NetApp NetCache servers
// then the RTSP User Agent string may be set to anything
#define RTSP_USER_AGENT_STRING "RealMedia Player (HelixDNAClient)/10.0.0.0 (Qualcomm Inc.)"

#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
#define MDP_IS_ROTATED_SIDEWAYS() \
   ((RealMDPVideo.rotation == MM_MPEG4_90_CW_ROTATION) || \
    (RealMDPVideo.rotation == MM_MPEG4_90_CCW_ROTATION))
#define MDPOP_ROT_MASK   0x00000007
#define MDPOP_SCALE_MASK 0x00000078
#endif /* FEATURE_QTV_MDP_TRANSFORMATIONS */

#if defined (FEATURE_ODM_UI)
#define REAL_PLAYER_DIAG
#endif

/*==============================================================================
   Enable the #define REAL_PLAYER_TCK_TESTING for TCK testing

   .tck files are text files containing a command line as follows:

    Usage:  tromsod infile [options]"

    In the following syntax descriptions, arglist is a comma-separated list of
    the form \"arg[=value],arg[=value],...\".  Some arguments take values, some
    do not.  If arglist contains any whitespace, it must be enclosed in quotes.
    For example,  -d  4,l=mylog.txt,a  specifies the debug level to be 4,
    that the debug log file is named \"mylog.txt\", and that the file should be
    opened in append mode rather than being overwritten.

    infile                Specify TROMSO bitstream input file

    -b <image_range>      Images to decode [decode all by default]
                          <image_range> is <m>-<n>:
                              3-5 means frames 3, 4 and 5
                              4-  means frames 4 and beyond
                               -5 means frames 0 through 5
                                7 means frames 0 through 7

    -d arglist            Specify debugging output.
        <level>               Detail level.  Use -1 to suppress. [default is 0]
        l=logfile             Output file for debug messages. [default is stdout]
        a                     Append to logfile. [default is to overwrite]

    -e arglist            Specify post filtering options
       smoothing                Smoothing [default is off]

    -i arglist            Specify input file format.
        w=<width>             Compressed image width [default is 176]
        h=<height>            Compressed image height [default is 144]

    -l                    Enable latency mode [default is off]

    -o outfile            Specify output file.  Output suppressed if unspecified.

    -8                    Input is RealVideo 8.

    -p                    Enable smoothing postfilter [default is off]

    -x arglist            Specify packet loss characteristics.
        <percent>             Percent packet loss [default is 0].

Example .tck file contents:
   refin.bit -i w=352,h=288 -o out.yuv
==============================================================================*/
#define REAL_PLAYER_TCK_TESTING


/*==============================================================================

                             EXTERNAL DECLARATIONS

==============================================================================*/
#ifdef FEATURE_REAL_QDSP_RELEASE_RESTORE
// This global variable should be set to true by OEM if it wants Real Player
// to release the DSP with the subsequent call to pause.
extern boolean real_cfg_enable_dsp_release;
#endif /* FEATURE_REAL_QDSP_RELEASE_RESTORE */

/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/
typedef struct CMediaRealBufferType
{
    unsigned char *Buffer[OEMREAL_BUFFER_COUNT];
    long   Height;
    long   Width;
    long   BPP;
}CMediaRealBufferType;

//
// IMedia objects must contain a reference to this object.
// Note: This class is a friend of IMedia and/or IMedia-based classes.
//

typedef struct CMediaRealLayer
{
   AEEMedia *        m_pMedia;         // Owner IMedia object
   void *            m_pAppContext;    // App context
   AEEObjectHandle   m_hObject;        // Context returned by ObjectMgr
   PFNNOTIFY         m_pfnNotify;      // IMedia notification function

   AEEMediaCallback  m_cb[OEMREAL_MAX_CB];
   uint32            m_dwCaps;         // Capabilities of IMedia
   ISound *          m_pISound;
   AEEMediaCallback  m_cbCommand;
   boolean           m_bFrameCBEnabled;  // Frame callbacks enabled?
   IDisplay *        m_pIDisplay;        // Pointer to display interface
   AEEPoint          m_dimPrev;       // Prefious Frame dimensions
   AEERect           m_rectClip;      // rectClip is the clipping region set by
                                      // the application.  We can only draw in
                                      // this area.
   boolean           m_bRectClipChanged; // Indicates m_rectClip has changed
   AEERect           m_rectImage;     // rectImage contains the x,y screen
                                      // coordinates for displaying the image.
                                      // It also contains the clipped
                                      // height,width.
                                      // By definition, rectImage.dx,dy
                                      // is <= frameInfo.cx,cy
   CMediaRealBufferType m_outputBuffers;
   IBitmap *         m_pDisplayBitmap;
   IDIB *            m_pDisplayDIB;
   unsigned char *   m_pDisplayDIBData;
   unsigned short    m_iDisplayWidth;
   unsigned short    m_iDisplayHeight;
   unsigned short    m_iDisplayPitch;
#if defined (FEATURE_ODM_UI) && defined(WOLF_5) // members below are for media/ringer volume level control
#error code not present
#endif
} CMediaRealLayer;

typedef enum CMediaRealStateEnum
{
   OEM_MEDIA_REAL_OPENED,
   OEM_MEDIA_REAL_REOPENED,
   OEM_MEDIA_REAL_PENDING,
   OEM_MEDIA_REAL_REOPEN_PENDING,
   OEM_MEDIA_REAL_ACTIVE,  // The clip has moved past the open state, but has not been closed
   OEM_MEDIA_REAL_CLOSED,
   OEM_MEDIA_REAL_PAUSED,
   OEM_MEDIA_REAL_DISCONNECTED
}CMediaRealState;

OBJECT(CMediaReal)
{
   INHERIT_AEEMedia(IMediaReal);
   boolean  m_bRelMark;
   CMediaRealLayer *m_pReal;
   CMediaRealState m_state;
   uint32 m_connectAttempt;
#if MIN_BREW_VERSIONEx(3,1,3)
   int32  m_nPlayType;
#endif
};


/*==============================================================================

                      DATA DECLARATIONS

==============================================================================*/
static uint32 guRealRefCnt = 0;

// Flag indicating that the player is being released.
uint16 releaseInProgress = FALSE;

static uint32 CMediaReal_Valid = FALSE;
static uint32 CMediaReal_ClipDuration = 0;
static uint32 CMediaReal_PlayTimeMsec = 0;
static char   CMediaReal_Resuming = FALSE;
static char   CMediaReal_ErrorOccured = FALSE;
static char   CMediaReal_OpenFailed   = FALSE;
static unsigned long CMediaReal_reconnect_pos = 0;

#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
struct {
/* MDP Image structures for the player regions */
  MDPIMG image;
/* MDP Image IDs */
  uint32 id;

  /* Current rotation/scaling transformation */
  AEEMediaMPEG4RotationType rotation;
  AEEMediaMPEG4ScalingType scaling;
} RealMDPVideo;
#endif /* FEATURE_QTV_MDP_TRANSFORMATIONS */

/*===============================================================================

      COMMON FUNCTION DECLARATIONS
      These functions are used by IMedia classes implemented in this file.

===============================================================================*/
static uint32 CMediaReal_Release(IMedia * po);
static int CMediaReal_SetMediaParm(IMedia * po, int nParmID, int32 p1, int32 p2);
static int CMediaReal_GetMediaParm(IMedia * po, int nParmID, int32 * pP1, int32 * pP2);
static int CMediaReal_Play(IMedia * po);
static int CMediaReal_RecordUnsupported(IMedia * po);
static int CMediaReal_Stop(IMedia * po);
static int CMediaReal_Seek(IMedia * po, AEEMediaSeek eSeek, int32 lTimeMS);
static int CMediaReal_Pause(IMedia * po);
static int CMediaReal_Resume(IMedia * po);
static int CMediaReal_GetTotalTime(IMedia * po);
static void IMediaReal_UpdateProgressBar(void *pData);
static int IMediaReal_QueueCallback(void   *pClientData,
                                           int     nStatus,
                                           int     nCmd,
                                           int     nSubCmd,
                                           void   *pData,
                                           uint32  dwSize);
#ifdef FEATURE_REAL_NV_CONFIG
static boolean IMedia_IsRealEnabled(void);
#endif
static void CMediaReal_Notify(AEEMediaCallback * pcb);
static void CMediaReal_CallbackNotify(AEEMediaCallback * pcb);
static void CMediaReal_SoundCB(void * pUser, AEESoundCmd eCBType, AEESoundStatus eStatus, uint32 dwParam);
void CMediaReal_CallBack(unsigned long userData, unsigned long cbEnumVal, unsigned long param1, unsigned long param2);


static void   OEMMediaReal_CalcClipping(
  AEERect        rectClip,
  AEEPoint       dimImage,  /* Transformed image dimensions */
  AEERect       *pImageRect /* Initialize dx,dy to orig frame dimensions */
  );

#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
static void   OEMMediaReal_MDPDeregister(CMediaReal *pme);
static uint32 OEMMediaReal_MDPCalcTransforms(AEEMediaMPEG4RotationType rot,
                                              AEEMediaMPEG4ScalingType  scaling);
static void   OEMMediaReal_MDPCalcTransformDimensions(
  AEEBitmapInfo  frame,
  AEEPoint      *pTransform
  );
#endif /* FEATURE_QTV_MDP_TRANSFORMATIONS */

#if defined (FEATURE_QTV_NET_POLICY)
void RealPlayer_SetPDPProfileNumber(uint8 PDPProfileNum);
#endif

#ifdef FEATURE_ACM
static int  CMediaReal_AcmSetMediaDataCB(ACM_Callback_data_type *po);
static int CMediaReal_AcmPlayCB(ACM_Callback_data_type *po);
extern void AEEMedia_CleanUpResource(AEEMedia * po);
extern int  AEEMedia_RequestResource(AEEMedia * pme, flg isPlay);
#endif

static int OEMMediaReal_Play_internal(CMediaReal *po, uint32 position);

#if MIN_BREW_VERSIONEx(3,1,3)
static int CMediaReal_SetPlayType(CMediaReal * pme, int32 nPlayType);
#endif

#if defined (FEATURE_ODM_UI) && defined(WOLF_5)
#error code not present
#endif

/*===========================================================================

                      IMediaReal DECLARATIONS

===========================================================================*/

static const VTBL(IMediaReal) gMediaRealFuncs =
{
   AEEMedia_AddRef,
   CMediaReal_Release,
   AEEMedia_QueryInterface,
   AEEMedia_RegisterNotify,
   CMediaReal_SetMediaParm,
   CMediaReal_GetMediaParm,
   CMediaReal_Play,
   CMediaReal_RecordUnsupported,
   CMediaReal_Stop,
   CMediaReal_Seek,
   CMediaReal_Pause,
   CMediaReal_Resume,
   CMediaReal_GetTotalTime,
   AEEMedia_GetState
};

/*===============================================================================

      REAL FUNCTION DEFINITIONS

===============================================================================*/

/*==============================================================================
FUNCTION:  CMediaRealLayer_New()
DESCRIPTION:
   Function to allocate, construct, and initialize new Real Player IMedia object.
==============================================================================*/
static CMediaRealLayer * CMediaRealLayer_New(IMedia * po, PFNNOTIFY pfn)
{
   CMediaRealLayer *   pme = MALLOC(sizeof(CMediaRealLayer));
   int                  i;
   int rv;
   IBitmap   *pIBitmapDDB;


#ifdef REAL_PLAYER_DIAG
   MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, "CMediaRealLayer_New,", 0, 0, 0);
#endif

   if (!pme || !po)
      return NULL;

   pme->m_pMedia = (AEEMedia *)po;
   pme->m_pAppContext = AEE_GetAppContext();

   if (AEEObjectMgr_Register(pme->m_pMedia, &pme->m_hObject) != SUCCESS)
   {
      FREE(pme);
      return NULL;
   }

   memset(&pme->m_outputBuffers, 0, sizeof(CMediaRealBufferType));
   pme->m_pfnNotify = pfn;  // pfn is CMediaReal_CallbackNotify
   pme->m_dwCaps = MM_CAPS_AUDIO | MM_CAPS_VIDEO;
   pme->m_bFrameCBEnabled = FALSE;

   // Initialize callbacks.
   for (i = 0; i < OEMREAL_MAX_CB; i++)
   {
      pme->m_cb[i].bInUse = FALSE;
      pme->m_cb[i].cmdNotify.pIMedia = po;
      pme->m_cb[i].cmdNotify.clsMedia = ((AEEMedia *)po)->m_clsSelf;
      pme->m_cb[i].cmdNotify.nStatus = 0;
      CALLBACK_Init(&pme->m_cb[i].cb, CMediaReal_Notify, &pme->m_cb[i]);
   }

   if (guRealRefCnt == 0)
   {
      int           nRetVal;
      AEEDeviceInfo di;

      ISHELL_GetDeviceInfo(pme->m_pMedia->m_pIShell, &di);

      nRetVal = OEMMediaReal_Init();

      if (nRetVal != SUCCESS)
      {
         AEEObjectMgr_Unregister(pme->m_hObject);
         FREE(pme);
         pme = NULL;
      }
      else
      {
         /* Get a pointer to the IDisplay */
         nRetVal = ISHELL_CreateInstance(pme->m_pMedia->m_pIShell, AEECLSID_DISPLAY, (void **)&pme->m_pIDisplay);
         if( nRetVal != SUCCESS )
         {
            MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR,  "Get IDisplay Failed(%d)", nRetVal, 0, 0 );
            AEEObjectMgr_Unregister(pme->m_hObject);
            FREE(pme);
            pme = NULL;
         }


        #if defined (FEATURE_ODM_UI) && defined(WOLF_5)
#error code not present
        #endif //(FEATURE_ODM_UI) && (WOLF_5)

         // Update the RTSP User Agent string
         OEMMediaReal_SetRTSPUserAgentString(RTSP_USER_AGENT_STRING);
      }
   }
   CMediaReal_ClipDuration = 0;
   CMediaReal_PlayTimeMsec = 0;

   if(pme == NULL)
   {
     return NULL;
   }
   rv = IDISPLAY_GetDeviceBitmap(pme->m_pIDisplay, &pIBitmapDDB);
   if( rv != SUCCESS )
   {
      IDISPLAY_Release(pme->m_pIDisplay);
      FREE(pme);
      MSG_1(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, "IDISPLAY_GetDeviceBitmap failed (%d)", rv);
      return NULL;
   }

   rv = IBITMAP_CreateCompatibleBitmap(pIBitmapDDB, &pme->m_pDisplayBitmap, 4, 4);
   IBITMAP_Release(pIBitmapDDB);
   if( rv != SUCCESS )
   {
      IDISPLAY_Release(pme->m_pIDisplay);
      FREE(pme);
      MSG_1(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, "IBITMAP_CreateCompatibleBitmap failed (%d)", rv);
      return NULL;
   }
   rv = IBITMAP_QueryInterface(pme->m_pDisplayBitmap, AEECLSID_DIB, (void**)&pme->m_pDisplayDIB);
   if( rv != SUCCESS )
   {
      MSG_1(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, "IBITMAP_QueryInterface failed (%d)", rv);
      IDISPLAY_Release(pme->m_pIDisplay);
      FREE(pme);
      IBITMAP_Release(pme->m_pDisplayBitmap);
      pme->m_pDisplayBitmap = NULL;
      return NULL;
   }
   // Store a pointer to the original bitmap info for later deletion.
   pme->m_pDisplayDIBData = pme->m_pDisplayDIB->pBmp;
   pme->m_iDisplayHeight  = pme->m_pDisplayDIB->cy;
   pme->m_iDisplayWidth   = pme->m_pDisplayDIB->cx;
   pme->m_iDisplayPitch   = pme->m_pDisplayDIB->nPitch;
   return pme;
}

/*==============================================================================
FUNCTION:  CMediaRealLayer_Delete()
DESCRIPTION:
==============================================================================*/
static void CMediaRealLayer_Delete(CMediaRealLayer * pme)
{
   int i;
   if (pme->m_pDisplayDIB != NULL)
   {
      // Restore original bitmap info, then release the DIB
      pme->m_pDisplayDIB->pBmp   = pme->m_pDisplayDIBData;
      pme->m_pDisplayDIB->cx     = pme->m_iDisplayWidth;
      pme->m_pDisplayDIB->cy     = pme->m_iDisplayHeight;
      pme->m_pDisplayDIB->nPitch = pme->m_iDisplayPitch;
      IDIB_Release(pme->m_pDisplayDIB);
      pme->m_pDisplayDIB = NULL;
   }
   if (pme->m_pDisplayBitmap != NULL)
   {
      IBITMAP_Release(pme->m_pDisplayBitmap);
      pme->m_pDisplayBitmap = NULL;
   }
   if (pme->m_pIDisplay)
   {
      IDISPLAY_Release(pme->m_pIDisplay);
      pme->m_pIDisplay = NULL;
   }

#if defined (FEATURE_ODM_UI) && defined(WOLF_5)
#error code not present
#endif
   for (i = 0; i < OEMREAL_BUFFER_COUNT; i++)
   {
      FREE(pme->m_outputBuffers.Buffer[i]);
   }
   memset(&pme->m_outputBuffers, 0, sizeof(CMediaRealBufferType));

   AEEObjectMgr_Unregister((AEEObjectHandle)pme->m_pMedia);
   FREE(pme);
}

/*==============================================================================
FUNCTION:  CMediaReal_New()
DESCRIPTION:
   Function to allocate and construct new Real Player IMedia object and set
    up callbacks.
==============================================================================*/
static IBase * CMediaReal_New(IShell * ps, AEECLSID cls)
{
   IMedia *      po;
   CMediaReal * pme;

#ifdef REAL_PLAYER_DIAG
   MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, "CMediaReal_New,", 0, 0, 0);
#endif

   // Alloc memory for the object
   po = (IMedia *)AEE_NewClass((IBaseVtbl *)&gMediaRealFuncs, sizeof(CMediaReal));
   if (!po)
      return NULL;

   // Call base class constructor
   if (SUCCESS != AEEMedia_New(po, ps, cls))
   {
      FREE(po);
      return NULL;
   }

   // Call Real Layer constructor
   pme = (CMediaReal *)po;
   pme->m_pReal = CMediaRealLayer_New(po, (PFNNOTIFY)CMediaReal_CallbackNotify);

   if ( !pme->m_pReal )
   {
      AEEMedia_Delete(po);
      FREE(po);
      return NULL;
   }
   else
   {
      // Generate callback to indicate player is ready
      int MMStatus = MM_STATUS_PAUSE;
      int cmd = MM_CMD_PLAY;
      unsigned long data = TRUE;
      pme->m_state = OEM_MEDIA_REAL_CLOSED;
      pme->m_connectAttempt = 0;
#if MIN_BREW_VERSIONEx(3,1,3)
      pme->m_nPlayType = MM_PLAY_TYPE_NORMAL;
#endif

      OEMMediaReal_SetCallBack(CMediaReal_CallBack, (unsigned long)pme);

      cmd = MM_CMD_PLAY;
      MMStatus = MM_STATUS_REPOSITIONING;
      data = TRUE;

      CMediaReal_Valid = TRUE;

      IMediaReal_QueueCallback((void *)pme->m_pReal->m_hObject, MMStatus, cmd, 0, (void *)data, (unsigned long) sizeof(data));

      ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_SOUND, (void **)&pme->m_pReal->m_pISound);
      ISOUND_RegisterNotify(pme->m_pReal->m_pISound, CMediaReal_SoundCB, pme);
   }

   return (IBase *)po;
}

/*===============================================================================

                     IMediaReal PUBLIC FUNCTION DEFINITIONS

===============================================================================*/
/*==============================================================================
FUNCTION:  IMediaReal_IsRealPlayerEnabled()
DESCRIPTION:
   Function that reads the NV item to determine if RealPlayer is enabled/disabled
==============================================================================*/
#ifdef FEATURE_REAL_NV_CONFIG
boolean IMedia_IsRealEnabled(void)
{
    nv_item_type               nvi;

    /* The default behavior is to enable the Real Player if NV item isn't ACTIVE */
    boolean CMedialReal_Enabled = TRUE;

    if (OEMNV_Get( NV_MM_ENABLE_REAL_I, &nvi ) == NV_DONE_S)
    {
       CMedialReal_Enabled = nvi.mm_enable_real;
       MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, "IMediaReal_NV Read, Enable Real NV Active %d",nvi.mm_enable_real,0,0);
    }
    return CMedialReal_Enabled;
}
#endif

/*==============================================================================
FUNCTION:  IMediaReal_Init()
DESCRIPTION:
   Function called only once to perform AEE initialization.
==============================================================================*/
void IMediaReal_Init(IShell * ps)
{
#ifdef FEATURE_REAL_NV_CONFIG
    if(IMedia_IsRealEnabled() == FALSE)
    {
        return;
    }
#endif

#ifdef REAL_PLAYER_DIAG
    MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, "IMediaReal_Init,", 0, 0, 0);
#endif

#ifndef FEATURE_REAL_PLAYER_USE_QTV_API
    AEEMedia_Init(ps, "video/rm", AEECLSID_MEDIAREAL);
    AEEMedia_Init(ps, "video/ram", AEECLSID_MEDIAREAL);
#ifdef REAL_PLAYER_TCK_TESTING
    AEEMedia_Init(ps, "video/rvtck", AEECLSID_MEDIAREAL); // Real Video TCK format files
#endif
#endif
}

/*==============================================================================
FUNCTION:  IMediaReal_New()
DESCRIPTION:
   Function called by ISHELL_CreateInstance() to allocate and construct
    new Real Player IMedia object.
==============================================================================*/
int IMediaReal_New(IShell * ps, AEECLSID cls, void **ppif)
{
   int result = SUCCESS;
   IBase * pobj;

#ifdef REAL_PLAYER_DIAG
   MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, "IMediaReal_New,", 0, 0, 0);
#endif

   if (!OEMMediaReal_PlayerRunning && guRealRefCnt == 0)
   {
      pobj = CMediaReal_New(ps, cls);
      guRealRefCnt = (pobj) ? 1 : 0;
   }
   else
   {
      pobj = NULL;
      result = EFAILED;
   }
   *ppif = pobj;

   return result;
}

/*==============================================================================
FUNCTION:  IMediaReal_QueueCallback()
DESCRIPTION:
   Function called by the Real Player Engine callback function to queue a
   callback to execute in the BREW context
==============================================================================*/
int IMediaReal_QueueCallback(void   *pClientData,
                             int     nStatus,
                             int     nCmd,
                             int     nSubCmd,
                             void   *pData,
                             uint32  dwSize)
{
   int i;
   CMediaReal      * pMedia;
   CMediaRealLayer * pme;
   AEEMediaCallback * pcb = NULL;

//#ifdef REAL_PLAYER_DIAG
//   MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, "IMediaReal_QueueCallback,", 0, 0, 0);
//#endif

   pMedia = (CMediaReal *)AEEObjectMgr_GetObject((AEEObjectHandle)pClientData);

   if (pMedia)
   {
      pme = pMedia->m_pReal;

      /* Find a free callback structure */
   MUTEX_LOCK();
      for (i = 0; i < OEMREAL_MAX_CB; i++)
      {
         if (pme->m_cb[i].bInUse == FALSE)
         {
            pme->m_cb[i].bInUse = TRUE;
            pcb = &pme->m_cb[i];
            break;
         }
      }
   MUTEX_UNLOCK();

      if (i == OEMREAL_MAX_CB)
      {
         MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, "Out of callback structures: %d/%d/%d", nStatus, nCmd, nSubCmd);
         return -1;
      }

      pcb->cmdNotify.nStatus = nStatus;
      pcb->cmdNotify.nCmd = nCmd;
      pcb->cmdNotify.nSubCmd = nSubCmd;
      pcb->cmdNotify.pCmdData = pData;
      pcb->cmdNotify.dwSize = dwSize;
#if MIN_BREW_VERSION(3,0)
   AEE_ResumeCallback(&pcb->cb, pme->m_pAppContext);
#else
      AEE_ResumeEx(&pcb->cb, 0, pme->m_pAppContext);
#endif /* MIN_BREW_VERSION(3,0) */
   }
   return 0;
}

/*==============================================================================
FUNCTION:  CMediaReal_Delete()
DESCRIPTION:
   Function to clean-up, unregister, and delete the Real Player.
==============================================================================*/
static void CMediaReal_Delete(IMedia * po, boolean bFree)
{
   CMediaReal      * pme = (CMediaReal *)po;
   CMediaRealLayer * pReal = pme->m_pReal;
   int i;

#ifdef REAL_PLAYER_DIAG
   MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, "CMediaReal_Delete, bFree: %d", bFree, 0, 0);
#endif

   if (!pme->m_bRelMark)
   {
      pme->m_bRelMark = TRUE;

      // Cancel the progress bar update timer
      ISHELL_CancelTimer(pme->m_pIShell, IMediaReal_UpdateProgressBar, pme);
      OEMMediaReal_Terminate();

      AEEObjectMgr_Unregister(pReal->m_hObject);

      /* Cancel any pending callbacks */
      for (i = 0; i < OEMREAL_MAX_CB; i++)
      {
         CALLBACK_Cancel(&pReal->m_cb[i].cb);
      }

      MM_RELEASEIF(pme->m_pReal->m_pISound);
      CMediaRealLayer_Delete(pme->m_pReal);

      // Call base class destructor
      OEMMediaReal_SetCallBack(NULL, 0);

      AEEMedia_Delete(po);
   }

   // Free object memory
   if (bFree)
   {
      CMediaReal_Valid = FALSE;
      FREE(po);
   }
}

/*==============================================================================
FUNCTION:  CMediaReal_Release()
DESCRIPTION:
   Function to release allocated IMedia objects.
==============================================================================*/
static uint32 CMediaReal_Release(IMedia * po)
{
   int32 nRef;

   // Set flag to indicate a release is in progress to avoid doing another
   //  OpenURL when the BREW IMedia interface calls CMediaReal_Stop()
   releaseInProgress = TRUE;
   nRef = AEEMedia_Release(po);

//#ifdef REAL_PLAYER_DIAG
//   MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, "CMediaReal_Release", 0, 0, 0);
//#endif

   if (!nRef)
   {
      // Delete the allocated IMedia object
      CMediaReal_Delete(po, TRUE);
      /* Set guRealRefCnt to 0 since we're allowing only one instance of
      ** Real media object.
      */
      guRealRefCnt = 0;
   }

   releaseInProgress = FALSE;
   return nRef;
}

void OEMMediaReal_GetVolumeCB(const void *client_data, snd_status_type status, uint16 volume)
{
  if(client_data == NULL)
    return;

  CMediaReal_CallBack((unsigned long)client_data, OEM_MEDIA_REAL_CB_GET_VOLUME,
     (uint16)(volume*VOLUME_STEP), sizeof(uint16));
}

void OEMMediaReal_SetVolumeCB(const void *client_data, snd_status_type status)
{
  if(client_data == NULL)
    return;

  CMediaReal_CallBack((unsigned long)client_data, OEM_MEDIA_REAL_CB_SET_VOLUME, 0, 0);
}

/*==============================================================================
FUNCTION:  CMediaReal_SetMediaData_internal(CMediaReal * pme )
DESCRIPTION:
   Function to set the media data for the real player.
==============================================================================*/
static int CMediaReal_SetMediaData_internal(CMediaReal * pme)
{
   int nRet = SUCCESS;
   AEEMedia *pMedia = pme->m_pReal->m_pMedia;

   pme->m_connectAttempt++;
   pme->m_state = OEM_MEDIA_REAL_PENDING;
   CMediaReal_OpenFailed = FALSE;
   CMediaReal_ErrorOccured = FALSE;
   switch(pMedia->m_md.clsData )
   {
      case MMD_FILE_NAME:
         nRet = OEMMediaReal_OpenURN(pMedia->m_pszFile);
         break;
      case MMD_BUFFER:  /* playback from buffer */
#ifdef FEATURE_REAL_PLAYER_BUFFERED_PLAYBACK_SUPPORTED
         nRet = OEMMediaReal_OpenBuffer(pMedia->m_md.pData, pMedia->m_md.dwSize);
#else
         nRet = EUNSUPPORTED;
#endif
         break;
   }

   if (nRet != SUCCESS)
   {
      pme->m_state = OEM_MEDIA_REAL_CLOSED;
   }
   return nRet;
}

/*==============================================================================
FUNCTION:  CMediaReal_SetMediaParm()
DESCRIPTION:
   Function to set media parameters:  volume, media resource file, or
    rectangular display area.
==============================================================================*/
static int CMediaReal_SetMediaParm(IMedia * po, int nParmID, int32 p1, int32 p2)
{
   CMediaReal * pme = (CMediaReal *)po;
#ifdef FEATURE_ACM
   AEEMedia * pMedia = pme->m_pReal->m_pMedia;
#endif
   int nRet = SUCCESS;
#ifdef FEATURE_ACM
   int nACMStatus = SUCCESS;
#endif

#if defined FEATURE_QTV_NET_POLICY
   AEENetPolicyInfo *netPolicyInfo = NULL;
#endif

#ifdef REAL_PLAYER_DIAG
   MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, "CMediaReal_SetMediaParm: ID: %d, p1: %d, p2: %d", nParmID, p1, p2);
#endif

   switch (nParmID)
   {
      #if defined FEATURE_QTV_NET_POLICY
      case MM_MP4_PARM_NET_QOS_POLICY:
         netPolicyInfo = (AEENetPolicyInfo*)p1;
         if (netPolicyInfo)
         {
            RealPlayer_SetPDPProfileNumber(netPolicyInfo->primaryPDPProfileNo);
         }
         else
         {
            RealPlayer_SetPDPProfileNumber(0);
         }
     break;
     #endif

      case MM_PARM_VOLUME:
      {
         snd_method_type snd_method;

      #ifdef FEATURE_AUDIO_FORMAT
            snd_method = SND_METHOD_MIDI;
      #else
            snd_method = SND_METHOD_MAX;
      #endif

      #if defined (FEATURE_ODM_UI) && defined(WOLF_5)
#error code not present
      #endif

         snd_set_device(SND_DEVICE_CURRENT,
            SND_MUTE_UNMUTED,
            SND_MUTE_UNMUTED,
            NULL,
            NULL);

         snd_set_volume(SND_DEVICE_CURRENT, snd_method, (uint8)p1/VOLUME_STEP, &OEMMediaReal_SetVolumeCB, pme);

         nRet = MM_PENDING;
         break;
      }

      case MM_PARM_MEDIA_DATA:
         if(p2 > 0)
         {
            AEEMediaDataEx *mdex = (AEEMediaDataEx *)p1;
            /* this is extended data */
            /* Real Player does not support playback from seperate files */
            /* This feature can be used only to disable audio via the dwCaps value */

            pme->m_pReal->m_dwCaps = mdex[0].dwCaps;
         }

         if ((pme->m_pReal->m_dwCaps & MM_CAPS_AUDIO) == 0)
         {
            OEMMediaReal_DisableAudio(TRUE);
         }
         else
         {
            OEMMediaReal_DisableAudio(FALSE);
         }

#ifndef FEATURE_ACM
         nRet = AEEMedia_SetMediaParm(po, nParmID, p1, p2);
         if (nRet == SUCCESS)
         {
            nRet = CMediaReal_SetMediaData_internal (pme);
         }
#else // FEATURE_ACM
         // SetMediaData can only be called in idle state
         if (!AEEMedia_IsIdleState(pMedia))
         {
            return EBADSTATE;
         }
         // Need to call requestResource before AEEMedia_SetMediaParm
         // since we are using the media state to choose the callback
         // function to be registered with ACM, one for setMediaParm and one for play.
         nACMStatus = AEEMedia_RequestResource(pMedia, TRUE);
         if(nACMStatus != SUCCESS && nACMStatus != MM_PENDING)
         {
           return EFAILED;
         }
         // Call AEEMedia_SetMediaParm, this will automatically
         // save the AEEMediaData to be used for delayed acquiring
         nRet = AEEMedia_SetMediaParm(po, nParmID, p1, p2);
         if (nRet == SUCCESS)
         {
            if (nACMStatus == MM_PENDING)
            {
               return SUCCESS;
            }
            else
            {
               nRet = CMediaReal_SetMediaData_internal (pme);
            }
         }
         // Clean up the ACM resource on failure
         if (nRet != SUCCESS)
         {
            AEEMedia_CleanUpResource(pMedia);
         }
#endif // FEATURE_ACM
         break;

      case MM_PARM_RECT:
         /* Clear the screen when the clipping rectangle has changed.
          * It is important to do this synchronously instead of waiting
          * until the next frame update since the App may have drawn to the
          * screen by then.
          */
#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
         mdp_clear_ovimg(RealMDPVideo.id);
#else /* !FEATURE_QTV_MDP_TRANSFORMATIONS */
         IDISPLAY_EraseRgn( pme->m_pReal->m_pIDisplay,
                            pme->m_pReal->m_rectClip.x,
                            pme->m_pReal->m_rectClip.y,
                            pme->m_pReal->m_rectClip.dx,
                            pme->m_pReal->m_rectClip.dy );
#endif /* !FEATURE_QTV_MDP_TRANSFORMATIONS */
         MEMCPY(&pme->m_pReal->m_rectClip, (void *)p1, sizeof(AEERect));
         pme->m_pReal->m_bRectClipChanged = TRUE;

         nRet = OEMMediaReal_SetDisplayArea(pme->m_pReal->m_rectClip.x, pme->m_pReal->m_rectClip.y,
            pme->m_pReal->m_rectClip.dx, pme->m_pReal->m_rectClip.dy);
         break;

      case MM_PARM_FRAME:
         pme->m_pReal->m_bFrameCBEnabled = p1;
         break;

#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
      case MM_MP4_PARM_ROTATION:
         RealMDPVideo.rotation = (AEEMediaMPEG4RotationType) p1;
         break;

      case MM_MP4_PARM_SCALING:
         RealMDPVideo.scaling = (AEEMediaMPEG4ScalingType) p1;
         break;
#endif /* FEATURE_QTV_MDP_TRANSFORMATIONS */

#if MIN_BREW_VERSIONEx(3,1,3)
      case MM_PARM_PLAY_TYPE:
         nRet = CMediaReal_SetPlayType(pme, p1);
         break;
#endif

      default:
         nRet = EUNSUPPORTED;
   }

   return nRet;
}

/*==============================================================================
FUNCTION:  CMediaReal_SetMediaParm()
DESCRIPTION:
   Function to return media parameter values:  volume, class ID, or
    capabilities.
==============================================================================*/
static int CMediaReal_GetMediaParm(IMedia * po, int nParmID, int32 * pP1, int32 * pP2)
{
   CMediaReal * pme = (CMediaReal *)po;
   CMediaRealLayer * pReal = pme->m_pReal;
   int nRet = SUCCESS;

//#ifdef REAL_PLAYER_DIAG
//   MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, "CMediaReal_GetMediaParm ID: %d, pP1: %d, pP2: %d", nParmID, pP1, pP2);
//#endif

   if (nParmID == MM_PARM_MEDIA_DATA)
      return AEEMedia_GetMediaParm(po, nParmID, pP1, pP2);

   switch (nParmID)
   {
      case MM_PARM_VOLUME:
         {
            // No need to alloc a command callback since the callback from
            // ISound uniquely identifies this operation and also
            // gets called in BREW context.
            snd_method_type snd_method;

         #ifdef FEATURE_AUDIO_FORMAT
            snd_method = SND_METHOD_MIDI;
         #else
            snd_method = SND_METHOD_MAX;
         #endif

            snd_set_device(SND_DEVICE_CURRENT,
               SND_MUTE_UNMUTED,
               SND_MUTE_UNMUTED,
               NULL,
               NULL);

            snd_get_volume(SND_DEVICE_CURRENT, snd_method, &OEMMediaReal_GetVolumeCB, pme);
            nRet = MM_PENDING;
         }
         break;

      case MM_PARM_CLSID:
         if (pP1 == NULL)
            return EFAILED;

         *pP1 = AEECLSID_MEDIAREAL;
         break;

      case MM_PARM_CAPS:
         if (pP1 == NULL)
            return EFAILED;

         *pP1 = (int32)pReal->m_dwCaps;
         if (pP2)
            *pP2 = 0;
         break;

      case MM_PARM_RECT:
         if ( !pP1 | !pme )
         {
           return EFAILED;
         }

         MEMCPY( (void *)pP1, &pme->m_pReal->m_rectClip, sizeof(AEERect));
         break;

      case MM_PARM_FRAME:
         if ((pP1 == NULL && pP2 == NULL) ||
             (pP1 != NULL && pP2 != NULL))
         { /* Invalid parameters */
            return EFAILED;
         }

         if (pP1 == NULL)
         { /* App is calling IMEDIA_IsFrameCallback */
            *pP2 = pme->m_pReal->m_bFrameCBEnabled;
         }
         else
         { /* App is calling IMEDIA_GetFrame */
            OEMMediaReal_FrameInfo info;
            if (OEMMediaReal_GetFrameInfo(&info))
            {
               nRet = SUCCESS;
            }
            pme->m_pReal->m_pDisplayDIB->pBmp   = info.RGBBuffer;
            pme->m_pReal->m_pDisplayDIB->cx     = info.Width;
            pme->m_pReal->m_pDisplayDIB->cy     = info.Height;
            pme->m_pReal->m_pDisplayDIB->nPitch =
               (info.Width * pme->m_pReal->m_iDisplayPitch) / pme->m_pReal->m_iDisplayWidth;
            IBITMAP_AddRef(pme->m_pReal->m_pDisplayBitmap);
            *pP1 = (int32)pme->m_pReal->m_pDisplayBitmap;
         }
         break;

      case MM_MP4_PARM_MEDIA_SPEC:
         nRet = OEMMediaReal_GetClipSource((RealPlayer_MediaSpec **)pP1, pP2);
         break;

#if MIN_BREW_VERSIONEx(3,1,3)
      case MM_PARM_PLAY_TYPE:
         if (pP1)
            *pP1 = pme->m_nPlayType;
         break;
#endif

      default:
         nRet = EUNSUPPORTED;
   }
   return nRet;
}

/*==================================================================
FUNCTION:  CMediaReal_Play_internal()
DESCRIPTION:
   Function to initiate play for a Real Media clip.
==================================================================*/
static int CMediaReal_Play_internal(IMedia * po, uint32 position)
{
   int nRet;
   CMediaReal *pme = (CMediaReal *)po;

#ifdef REAL_PLAYER_DIAG
   MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, "CMediaReal_Play,", 0, 0, 0);
#endif
   nRet = AEEMedia_Play(po);

  #ifdef FEATURE_ACM
   if (nRet == MM_PENDING)
   {
     // ACM Resource Acquire Pending
     return SUCCESS;
   }
   #endif

   if (nRet != SUCCESS)
      return nRet;

   nRet = OEMMediaReal_Play_internal(pme, position);

  #ifdef FEATURE_ACM
   if(nRet != SUCCESS)
   {
     AEEMedia_CleanUpResource((AEEMedia *)po);
   }
  #endif
   return nRet;
}

static int OEMMediaReal_Play_internal(CMediaReal *po, uint32 position)
{
    CMediaReal *pme = (CMediaReal *)po;

    unsigned long temp = 0;
    int nRet = SUCCESS;

#if defined (FEATURE_ODM_UI) && defined(WOLF_5)
#error code not present
#endif

   if ((pme->m_pszFile != NULL || pme->m_md.pData != NULL) && pme->m_state == OEM_MEDIA_REAL_CLOSED)
   {
      pme->m_connectAttempt++;
      pme->m_state = OEM_MEDIA_REAL_REOPEN_PENDING;
      CMediaReal_OpenFailed = FALSE;
      CMediaReal_ErrorOccured = FALSE;
      switch( pme->m_md.clsData )
      {
         case MMD_FILE_NAME:
            nRet = OEMMediaReal_OpenURN(pme->m_pszFile);
            break;
         case MMD_BUFFER:  /* playback from buffer */
#ifdef FEATURE_REAL_PLAYER_BUFFERED_PLAYBACK_SUPPORTED
            nRet = OEMMediaReal_OpenBuffer(pme->m_md.pData, pme->m_md.dwSize);
#else
            nRet = EUNSUPPORTED;
#endif
            break;
      }
      if (nRet != SUCCESS)
      {
         pme->m_state = OEM_MEDIA_REAL_CLOSED;
      }
      else if (position)
      {
         // Borrow the reconnect code to handle back to back open/seek commands
         // Can't call seek until the open call has completed (for some reason it is OK
         // to call play before the open has finished).
         // Setting pme->m_state == OEM_MEDIA_REAL_DISCONNECTED will cause
         // CMediaReal_Play_internal to be c called again once the open command completes
         // (see function CMediaReal_CallBack)
         CMediaReal_reconnect_pos = position;
         pme->m_state = OEM_MEDIA_REAL_DISCONNECTED;
         return nRet;
      }
   }

   // Generate callback to set the progress bar to zero
   IMediaReal_QueueCallback((void *)((CMediaReal *)po)->m_pReal->m_hObject,
      MM_STATUS_TICK_UPDATE,
      MM_CMD_PLAY,
      0,
      (void *)temp,
      sizeof(temp));

   // Start the timer-based updating of the progress bar.
   IMediaReal_UpdateProgressBar((void *) po);

   if (position)
   {
      OEMMediaReal_Seek(position, SEEK_TYPE_PREV_KEY_FRAME);
   }

#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
   RealMDPVideo.rotation = MM_MPEG4_NO_ROTATION;
   RealMDPVideo.scaling  = MM_MPEG4_NO_SCALING;

   if( RealMDPVideo.id != 0 )
   {
     OEMMediaReal_MDPDeregister((CMediaReal*)po);
   }
#endif /* #ifdef FEATURE_QTV_MDP_TRANSFORMATIONS */

   // Put the player engine in play.
   nRet = OEMMediaReal_Play();
   return nRet;
}

/*==================================================================
FUNCTION:  CMediaReal_Play()
DESCRIPTION:
   Function to initiate play for a Real Media clip.
==================================================================*/
static int CMediaReal_Play(IMedia * po)
{
   if (CMediaReal_OpenFailed)
   {
      // Trigger a play failed message
      CMediaReal_CallBack((unsigned long)((CMediaReal *)po),
         OEM_MEDIA_REAL_CB_ONPLAYSTARTED, 0, 0);
      CMediaReal_CallBack((unsigned long)((CMediaReal *)po),
         OEM_MEDIA_REAL_CB_ONSTOP, 0, 0);
      CMediaReal_CallBack((unsigned long)((CMediaReal *)po),
         OEM_MEDIA_REAL_CB_ONPRESENTATIONCLOSED, 0, 0);
      return EFAILED;
   }
   CMediaReal_ErrorOccured = FALSE;
   return CMediaReal_Play_internal(po, 0);
}

/*==================================================================
   Unsupported functions -- used in vtbl of IMedia classes
==================================================================*/
static int CMediaReal_RecordUnsupported(IMedia * po)
{

#ifdef REAL_PLAYER_DIAG
   MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, "CMediaReal_RecordUnsupported", 0, 0, 0);
#endif
   return EUNSUPPORTED;
}

/*==================================================================
  Function CMediaReal_ReturnToReadyState
  Purpose:  Local function to cancel the progress bar timer and
             generate callback to put interface back in the ready
             state
==================================================================*/
static void CMediaReal_ReturnToReadyState(CMediaReal * pme)
{
   // Generate callback to indicate play is done
   if (pme->m_pReal)
   {
      int status = MM_STATUS_DONE;
      if (CMediaReal_OpenFailed || CMediaReal_ErrorOccured)
      {
         status = MM_STATUS_ABORT;
      }
      IMediaReal_QueueCallback((void *)pme->m_pReal->m_hObject,
         status,
         MM_CMD_PLAY,
         0,
         (void *)0,
         sizeof(0));
   }
}

/*==================================================================
  Function CMediaReal_Stop
  Purpose:  Process the activation of the media STOP button
==================================================================*/
static int CMediaReal_Stop(IMedia * po)
{
   int            nRet;

#ifdef REAL_PLAYER_DIAG
   MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, "CMediaReal_Stop,", 0, 0, 0);
#endif

   nRet = AEEMedia_Stop(po);
   if (nRet != SUCCESS)
      return nRet;

#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
   OEMMediaReal_MDPDeregister((CMediaReal*)po);
#endif /* FEATURE_QTV_MDP_TRANSFORMATIONS */

   // Stop audio and video play.
   nRet = OEMMediaReal_Stop();

   return nRet;
}

/*==================================================================
FUNCTION:  CMediaReal_Seek()
DESCRIPTION:
   Function to seek to a specified time within a clip.
==================================================================*/
static int CMediaReal_Seek(IMedia * po, AEEMediaSeek eSeek, int32 lTimeMS)
{
   int           nRet;
   unsigned long userCurrentPlayTimeMsec;
   unsigned long nDuration;
   int32         seekTime;
   int32         seekType;

   // Get the current clip play time in milliseconds.
   OEMMediaReal_GetCurrentPlayTime(&userCurrentPlayTimeMsec);

   // Get the current clip total play duration in milliseconds
   OEMMediaReal_GetClipInfo(&nDuration);

   // Determine the clip time to seek to
   switch (eSeek)
   {
   case MM_SEEK_END:
      seekTime = nDuration + lTimeMS;
      break;
   case MM_SEEK_CURRENT:
      seekTime = userCurrentPlayTimeMsec + lTimeMS;
      break;
   case MM_SEEK_START:
   default:
      seekTime = lTimeMS;
      break;
   }

   if (seekTime < 0)
   {
      seekTime = 0;
   }

   if (seekTime > (int32) nDuration)
   {
      seekTime = (int32) nDuration;
   }

   if (CMediaReal_OpenFailed)
   {
      // Trigger a seek failed message
      CMediaReal_CallBack((unsigned long)((CMediaReal *)po),
         OEM_MEDIA_REAL_CB_ONPRESEEK, userCurrentPlayTimeMsec, seekTime);
      return EFAILED;
   }

   CMediaReal_ErrorOccured = FALSE;

   if (lTimeMS > 0)
   {
      seekType = SEEK_TYPE_NEXT_KEY_FRAME;
   }
   else
   {
      seekType = SEEK_TYPE_PREV_KEY_FRAME;
   }


#ifdef REAL_PLAYER_DIAG
   MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, "CMediaReal_Seek, lTimeMS: %d", lTimeMS, 0, 0);
#endif

   nRet = AEEMedia_Seek(po, eSeek, seekTime);
   if (nRet != SUCCESS)
      return nRet;

   nRet = OEMMediaReal_Seek(seekTime, seekType);
   return nRet;
}

/*==================================================================
FUNCTION:  CMediaReal_Pause()
DESCRIPTION:
   Function to pause playing of a clip.
==================================================================*/
static int CMediaReal_Pause(IMedia * po)
{
   int            nRet;

#ifdef FEATURE_REAL_QDSP_RELEASE_RESTORE
   AEEMedia * pme = (AEEMedia *)po;
#endif /* FEATURE_REAL_QDSP_RELEASE_RESTORE */
   if (CMediaReal_OpenFailed)
   {
      // Trigger a pause failed message
      CMediaReal_CallBack((unsigned long)((CMediaReal *)po),
         OEM_MEDIA_REAL_CB_ONPAUSE, 0, 0);
      return EFAILED;
   }
   CMediaReal_ErrorOccured = FALSE;

#ifdef REAL_PLAYER_DIAG
   MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, "CMediaReal_Pause,", 0, 0, 0);
#endif

   nRet = AEEMedia_Pause(po);
   if (nRet != SUCCESS)
   {
#ifdef FEATURE_REAL_QDSP_RELEASE_RESTORE
      if ( real_cfg_enable_dsp_release && AEEMedia_IsPlayPauseState(pme) )
      {
         /* For incoming call interruption, allow pause processing to go
         ** through if we are in the paused state.
         */
         nRet = OEMMediaReal_Pause();
      }
#endif /* #ifdef FEATURE_REAL_QDSP_RELEASE_RESTORE */
   }
   else
   {
      nRet = OEMMediaReal_Pause();
   }
   return nRet;
}

/*==================================================================
FUNCTION:  CMediaReal_Resume()
DESCRIPTION:
   Function to resume playing of a clip that has been paused.
==================================================================*/
static int CMediaReal_Resume(IMedia * po)
{
   int            nRet;

   if (CMediaReal_OpenFailed)
   {
      // Trigger a resume failed message
      CMediaReal_CallBack((unsigned long)((CMediaReal *)po),
         OEM_MEDIA_REAL_CB_RESUME, 0, 0);
      return EFAILED;
   }
   CMediaReal_ErrorOccured = FALSE;
#ifdef REAL_PLAYER_DIAG
   MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, "CMediaReal_Resume,", 0, 0, 0);
#endif

   nRet = AEEMedia_Resume(po);
   if (nRet != SUCCESS)
      return nRet;

   nRet = OEMMediaReal_Play();
   return nRet;
}

/*==================================================================
FUNCTION:  CMediaReal_GetTotalTime()
DESCRIPTION:
   Function to set up a callback to obtain the play-time duration
    of a clip.
==================================================================*/
static int CMediaReal_GetTotalTime(IMedia * po)
{
   int            nRet = SUCCESS;
   unsigned long  nDuration;
   CMediaReal *   pme = (CMediaReal *)po;

#ifdef REAL_PLAYER_DIAG
   MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, "CMediaReal_GetTotalTime,", 0, 0, 0);
#endif

   nRet = AEEMedia_GetTotalTime(po);
   if (nRet != SUCCESS)
      return nRet;

   nRet = OEMMediaReal_GetClipInfo(&nDuration);
   if (nRet == SUCCESS)
   {
      CMediaReal_ClipDuration = nDuration;
      IMediaReal_QueueCallback((void*)pme->m_pReal->m_hObject,
                                MM_STATUS_DONE,
                                MM_CMD_GETTOTALTIME,
                                0,
                                (void *)nDuration,
                                sizeof(nDuration));
   }

   return nRet;
}

/*==================================================================

   Callback Notification Functions: called in BREW context after
   ISHELL_Resume()

==================================================================*/
/*==================================================================
   Media callback notify -- called in BREW context.
==================================================================*/
void CMediaReal_CallbackNotify(AEEMediaCallback * pcb)
{
   CMediaReal *    pme;
   CMediaRealLayer *pReal;

#ifdef REAL_PLAYER_DIAG
   //   MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, "CMediaReal_CallbackNotify,", 0, 0, 0);
#endif

   if (!pcb)
      return;

   pme = (CMediaReal *)pcb->cmdNotify.pIMedia;
   pReal = pme->m_pReal;

#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
  if ( pcb->cmdNotify.nCmd    == MM_CMD_PLAY &&
       pcb->cmdNotify.nStatus == MM_MP4_STATUS_VIDEO_ABORT )
  {
    /* Video has ended */
    OEMMediaReal_MDPDeregister(pme);
    return;
  }
#endif /* FEATURE_QTV_MDP_TRANSFORMATIONS */

   if (pcb->cmdNotify.nCmd    == MM_CMD_PLAY &&
       pcb->cmdNotify.nStatus == MM_STATUS_FRAME &&
      !pReal->m_bFrameCBEnabled )
   {
      /* We're about to send an MM_STATUS_FRAME to the Brew app.  However, they
      * haven't enabled them, so handle it ourselves and return.
      */
      IBitmap      * pFrame;
#ifndef FEATURE_QTV_MDP_TRANSFORMATIONS
      uint16 x,  y;
      uint16 dx, dy;
#endif
      /* frameInfo contains the information about the original frame to be
      * displayed
      */
      AEEBitmapInfo  frameInfo;
      /* dimTransform contains the height,width of the transformed image.
      * For non-MDP, dimTransform will be equal to frameInfo height,width.
      * For MDP, dimTransform will be the transformed height,width.
      */
      AEEPoint       dimTransform;

#if (defined FEATURE_REAL_QDSP_RELEASE_RESTORE && defined  FEATURE_QTV_MDP_TRANSFORMATIONS )
       /* The  real_cfg_enable_dsp_release is TRUE when MediaPlayer receives a Suspend
          event from the Brew layer. If this is the case, don't render the frame through
          MDP since this could overwrite whatever the application that is causing the
          player SUSPEND has written to the screen
       */
       if (real_cfg_enable_dsp_release)
       {
          MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, "Player suspended - not rendering frame", 0, 0, 0);
          return;
       }
#endif /* (defined FEATURE_REAL_QDSP_RELEASE_RESTORE && defined  FEATURE_QTV_MDP_TRANSFORMATIONS ) */

      /*
      * IMPORTANT NOTE: You need to do IBITMAP_Release(pFrame) after you're done with pFrame.
      */
      IMEDIA_GetFrame(pcb->cmdNotify.pIMedia, &pFrame);
      if (pFrame != NULL)
      {
         // Get the bitmap size
         IBITMAP_GetInfo(pFrame, &frameInfo, sizeof(frameInfo));

#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
         OEMMediaReal_MDPCalcTransformDimensions( frameInfo, &dimTransform );
#else
         dimTransform.x = frameInfo.cx;
         dimTransform.y = frameInfo.cy;
#endif /* FEATURE_QTV_MDP_TRANSFORMATIONS */

         /* Should clear the screen if we've indicated and the dimensions of the
         * current frame have changed from the previous one.
         *
         * 'and' is not sufficient here.  There's a race condition where a button masher
         * pushing 'rotate' at full speed can cause two rotations between processings of
         * this loop, resulting in an improper screen erase between rotations.  We need
         * to clear the screen whenever the flag is high OR the image dimensions have changed.
         */
         if( pReal->m_dimPrev.x != dimTransform.x ||
             pReal->m_dimPrev.y != dimTransform.y ||
             pReal->m_bRectClipChanged  )
         {
#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
            IBitmap *pDevBitmap;
#endif
            /* Do not clear the screen on the first pass through or if the previous
               * dimensions were zero since we haven't drawn anything on the screen.
               * Also do not clear the screen when the clipping rectangle has changed
               * since we already did it synchronously when SetRect was called.
               */
            if( pReal->m_dimPrev.x != 0 && pReal->m_dimPrev.y != 0 &&
               !pReal->m_bRectClipChanged )
            {
               /* Frame callbacks are not enabled, so clear the screen */
#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
            mdp_clear_ovimg(RealMDPVideo.id);
#else /* !FEATURE_QTV_MDP_TRANSFORMATIONS */
               IDISPLAY_EraseRgn( pReal->m_pIDisplay,
                                 pReal->m_rectClip.x,
                                 pReal->m_rectClip.y,
                                 pReal->m_rectClip.dx,
                                 pReal->m_rectClip.dy );
#endif /* !FEATURE_QTV_MDP_TRANSFORMATIONS */
            }


            /* Reset the clip rectangle changed flag */
            pReal->m_bRectClipChanged = FALSE;

            /* Save the dimensions of the current frame */
            pReal->m_dimPrev.x  = dimTransform.x;
            pReal->m_dimPrev.y  = dimTransform.y;

            /* Initialize the clipped image rectangle to the original frame's
            * dimensions.
            */
            pReal->m_rectImage.dx = frameInfo.cx;
            pReal->m_rectImage.dy = frameInfo.cy;


            /* Call the clipping function to calculate visible image */
            OEMMediaReal_CalcClipping( pReal->m_rectClip, dimTransform, &pReal->m_rectImage );

#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
            /* Support full-screen (annunciator bar off) mode by using AppToScreen
            * to convert between app coordinates and screen coordinates.  If we
            * can't get these interfaces, then the following code will assume that
            * we're in full-screen
            */
            if(IDISPLAY_GetDeviceBitmap(pReal->m_pIDisplay, &pDevBitmap) == SUCCESS)
            {
               IBitmapCtl *pBitmapCtl;
               if(IBITMAP_QueryInterface(pDevBitmap, AEEIID_BITMAPCTL, (void **) &pBitmapCtl) == SUCCESS)
               {
                  AEEPoint scrPoint, appPoint;
                  appPoint.x = pReal->m_rectImage.x;
                  appPoint.y = pReal->m_rectImage.y;
                  scrPoint = IBITMAPCTL_AppToScreen(pBitmapCtl, appPoint);
                  pReal->m_rectImage.x = scrPoint.x;
                  pReal->m_rectImage.y = scrPoint.y;
                  IBITMAPCTL_Release(pBitmapCtl);
               }
               IBITMAP_Release(pDevBitmap);
            }
#endif /* FEATURE_QTV_MDP_TRANSFORMATIONS */
         }

         /* If the entire image is clipped, don't bother doing anything else.  This
         * is especially important for MDP where specifying a window w/ 0 h/w
         * causes strange behavior.
         */
         if( pReal->m_rectImage.dx != 0 && pReal->m_rectImage.dy != 0 )
         {
#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
            IDIB *pMDPYCbCr;

            /* Obtain IDIB pointer from an IBitmap */
            if( (IBITMAP_QueryInterface(pFrame, AEECLSID_DIB, (void**)&pMDPYCbCr) != SUCCESS) ||
               (pMDPYCbCr == NULL) )
            {
               MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, "Invalid interface ID and/or couldn't get IDIB(%d)", pMDPYCbCr, 0, 0);
               IBITMAP_Release(pFrame);
               return;
            }

            /* Fill in the MDP info structure for video */
            RealMDPVideo.image.dest      = PRIMARY_LCD_TYPE;
            RealMDPVideo.image.layer     = MDP_FGL;
            RealMDPVideo.image.imgType   = MDP_YCBCR420;
            RealMDPVideo.image.bmy_addr  = (uint32*)pMDPYCbCr->pBmp;
            RealMDPVideo.image.cbcr_addr = (uint32*)&pMDPYCbCr->pBmp[frameInfo.cx*frameInfo.cy];
            RealMDPVideo.image.width     = frameInfo.cx;
            RealMDPVideo.image.cwin.x1   = 0;
            RealMDPVideo.image.cwin.y1   = 0;
            RealMDPVideo.image.cwin.x2   = pReal->m_rectImage.dx-1;
            RealMDPVideo.image.cwin.y2   = pReal->m_rectImage.dy-1;
            RealMDPVideo.image.lcd_x     = pReal->m_rectImage.x - BREW_OFFSET_X;
            RealMDPVideo.image.lcd_y     = pReal->m_rectImage.y - BREW_OFFSET_Y;
            RealMDPVideo.image.mdpOp     = OEMMediaReal_MDPCalcTransforms(RealMDPVideo.rotation,
               RealMDPVideo.scaling);

            /* Check if the current MDP handle is valid */
            if( mdp_is_ovimg_valid(RealMDPVideo.id) )
            {
               /* The MDP handle was valid, so update the params via
               * mdp_replace_ovimg
               */
               mdp_replace_ovimg(&RealMDPVideo.image, RealMDPVideo.id);
            }
            else
            {
               /* The MDP handle wasn't valid, so register a new one! */
               RealMDPVideo.id = mdp_register_ovimg(&RealMDPVideo.image);
               MSG_MED( "Registered new MDP ID %d", RealMDPVideo.id, 0, 0 );
            }

            /* By now we should really have a valid MDP handle, but check just to
            * make sure
            */
            if( RealMDPVideo.id != 0 )
            {
               /* Update the display */
#ifdef FEATURE_MDP
               mdp_update_ovimg(RealMDPVideo.id);
#endif /* FEATURE_MDP */
            }
            else
            {
               MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, "MDP registration/replacement failed!!!", 0, 0, 0);
            }

            /* The IDIB pointer must be released once we are finished using it */
            IDIB_Release(pMDPYCbCr);
#else
            dx = MIN( pReal->m_rectClip.dx, dimTransform.x );
            dy = MIN( pReal->m_rectClip.dy, dimTransform.y );
            x = (uint16)((pReal->m_rectClip.dx - dx) / 2 + pReal->m_rectClip.x);
            y = (uint16)((pReal->m_rectClip.dy - dy) / 2 + pReal->m_rectClip.y);
            /* Display the frame centered on the screen */
            IDISPLAY_BitBlt(pme->m_pReal->m_pIDisplay, x, y, dx, dy, pFrame, 0, 0, AEE_RO_COPY);
            IDISPLAY_Update(pme->m_pReal->m_pIDisplay);
#endif /* FEATURE_QTV_MDP_TRANSFORMATIONS */
         } /* if( dx != 0 && dy != 0 ) */
         else
         {
            MSG_MED("The entire video image is clipped: (%d,%d)",
                  pReal->m_rectClip.dx, pReal->m_rectClip.dy, 0);
         }
         IBITMAP_Release(pFrame);
      }
      else
      {
         MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, "pFrame is NULL",0,0,0);
      }
      /* Do not send this callback to the app as they are not registered for it */
      return;
   }
   AEEMedia_CallbackNotify((AEEMedia *)pme, pcb);
}

/*==================================================================
FUNCTION:  CMediaReal_SoundCB()
DESCRIPTION:
   Function to process audio-related callbacks.
==================================================================*/
static void CMediaReal_SoundCB(void * pUser, AEESoundCmd eCBType, AEESoundStatus eStatus, uint32 dwParam)
{
   CMediaReal *      pme = (CMediaReal *)pUser;
   AEESoundCmdData *    pData = (AEESoundCmdData *)dwParam;
   AEEMediaCallback     mcb;
   void *               paLast;

#ifdef REAL_PLAYER_DIAG
//   MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, "CMediaReal_SoundCB,", 0, 0, 0);
#endif

   MEMCPY(&mcb, &pme->m_pReal->m_cbCommand, sizeof(AEEMediaCallback));

   mcb.bInUse = TRUE;
   mcb.cmdNotify.nSubCmd = MM_PARM_VOLUME;
   mcb.cmdNotify.nCmd = (eCBType == AEE_SOUND_VOLUME_CB) ? MM_CMD_GETMEDIAPARM : MM_CMD_SETMEDIAPARM;
   mcb.cmdNotify.nStatus = (eStatus == AEE_SOUND_SUCCESS) ? MM_STATUS_DONE : MM_STATUS_ABORT;
   mcb.cmdNotify.pCmdData = NULL;
   mcb.cmdNotify.dwSize = 0;

   if (pData && eCBType == AEE_SOUND_VOLUME_CB && eStatus == AEE_SOUND_SUCCESS)
   {
      mcb.cmdNotify.pCmdData = (void *)(uint32)pData->wVolume;
      mcb.cmdNotify.dwSize = sizeof(pData->wVolume);
   }

#if MIN_BREW_VERSION(3,0)
   paLast = AEE_EnterAppContext(pme->m_pReal->m_pAppContext);
#else
   paLast = AEE_SetAppContext(pme->m_pReal->m_pAppContext);
#endif
   AEEMedia_CallbackNotify((AEEMedia *)pme, &mcb);
#if MIN_BREW_VERSION(3,0)
   AEE_LeaveAppContext(paLast);
#else
   AEE_SetAppContext(paLast);
#endif
}

/*==================================================================
FUNCTION:  CMediaReal_Notify()
DESCRIPTION:
   Callback function queued by the Real Player engine to run in BREW's
   context.
==================================================================*/
static void CMediaReal_Notify(AEEMediaCallback * pcb)
{
   CMediaReal *      pMedia;
   CMediaRealLayer * pme;
   AEEMediaCallback   mcb;

#ifdef REAL_PLAYER_DIAG
//   MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, "CMediaReal_Notify,", 0, 0, 0);
#endif

   if (!pcb)
      return;

   pMedia = (CMediaReal *)pcb->cmdNotify.pIMedia;
   if (!pMedia)
      return;

   pme = pMedia->m_pReal;
   if (!pme)
      return;

   MEMCPY(&mcb, pcb, sizeof(AEEMediaCallback));

   /* Free the callback structure */
   pcb->bInUse = FALSE;

   if (pme->m_pfnNotify)
      pme->m_pfnNotify(&mcb);
}

/*==================================================================
FUNCTION:  IMediaReal_UpdateProgressBar()
DESCRIPTION:
   Timer-based function to update the percent complete progress bar.
==================================================================*/
void IMediaReal_UpdateProgressBar(void *pData)
{
   unsigned long userCurrentPlayTimeMsec;
   CMediaReal *pme = (CMediaReal *)pData;

   // Get the current clip play time in milliseconds.
   OEMMediaReal_GetCurrentPlayTime(&userCurrentPlayTimeMsec);

   // Handle condition where current play time is within the play duration.
   // Set timer to generate another update after the specified interval.
   ISHELL_SetTimer(pme->m_pIShell, OEMREAL_PROGRESS_UPDATE_INTERVAL_MSEC, IMediaReal_UpdateProgressBar, pData);

   // Generate tick update callbacks only when the presentation is opened and
   //  time is advancing.  If the callback contains a time of zero, there is
   //  an undesirable automatic updating of the play time by one second in the
   //  IMedia interface.
   if (pme->m_state == OEM_MEDIA_REAL_ACTIVE && userCurrentPlayTimeMsec != CMediaReal_PlayTimeMsec)
   {
      CMediaReal_PlayTimeMsec = userCurrentPlayTimeMsec;
      // Generate callback to update displayed progress bar
      IMediaReal_QueueCallback((void *)pme->m_pReal->m_hObject,
         MM_STATUS_TICK_UPDATE,
         MM_CMD_PLAY,
         0,
         (void *)userCurrentPlayTimeMsec,
         sizeof(userCurrentPlayTimeMsec));
   }

#ifdef REAL_PLAYER_DIAG
   MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, "UpdateProgressBar playbackMsec = %d", userCurrentPlayTimeMsec, 0, 0);
#endif

// brownj:  clock / timing debug
#ifdef MEASURE_CLIP_REX_AND_WINDOWS_TIME
   {
      unsigned long windowsTime;
      unsigned long rexTime;
      windowsTime = GetTickCount();
      rexTime = REXGetTickCount();// HX_GET_TICKCOUNT();
      MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, "CLIP %d, WIN %d, REX %d ms", userCurrentPlayTimeMsec, windowsTime, rexTime);
      printf ("WHOA DADDY!\n");
   }
#endif

}

void OEMMediaReal_UpdateOutputBuffers(CMediaRealLayer * pme, RealPlayer_MediaSpec *mediaSpec)
{
   if (mediaSpec->dwVideoHeight != pme->m_outputBuffers.Height || mediaSpec->dwVideoWidth != pme->m_outputBuffers.Width)
   {
      OEMMediaReal_OutputBufferInfoType ob;
      unsigned long size;
      int i;
      for (i = 0; i < OEMREAL_BUFFER_COUNT; i++)
      {
         FREE(pme->m_outputBuffers.Buffer[i]);
      }
      pme->m_outputBuffers.BPP = (pme->m_pDisplayDIB->nPitch * 8) / pme->m_pDisplayDIB->cx;
      size = mediaSpec->dwVideoHeight * mediaSpec->dwVideoWidth * pme->m_outputBuffers.BPP / 8;
      /* MDP Hardware Bug Work-around */
      /* When the MDP renders a frame with the height dimension non-divisible by
       * 16, it will read past the end of the YCbCr buffer when color
       * converting.  We work around the issue by mirroring the last two CbCr
       * lines past the valid portion of the buffer.  Thus, we must allocate
       * two extra lines of CbCr, or the value of the width.  In this case,
       * we use 16BPP, instead of the 12BPP needed for YCbCr, so we do allocate
       * enough memory */

      for (i = 0; i < OEMREAL_BUFFER_COUNT; i++)
      {
         pme->m_outputBuffers.Buffer[i] = MALLOC(size);
         if (!pme->m_outputBuffers.Buffer[i])
         {
            i--;
            while (i >= 0)
            {
               FREE(pme->m_outputBuffers.Buffer[i]);
               i--;
            }
            MSG_1(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, "Error: Real Player Out of memory allocating output buffers: %d bytes", size);
            memset(&pme->m_outputBuffers, 0, sizeof(CMediaRealBufferType));
            return;
         }
      }
      pme->m_outputBuffers.Height = mediaSpec->dwVideoHeight;
      pme->m_outputBuffers.Width = mediaSpec->dwVideoWidth;
      ob.Buffer = (void **)pme->m_outputBuffers.Buffer;
      ob.BufferCount = OEMREAL_BUFFER_COUNT;
      ob.Height = pme->m_outputBuffers.Height;
      ob.Width  = pme->m_outputBuffers.Width;
      ob.BPP    = pme->m_outputBuffers.BPP;
      OEMMediaReal_SetOutputBufferInfo(&ob);
   }
}

/*==================================================================
FUNCTION:  CMediaReal_CallBack()
DESCRIPTION:
   Function to queue RealPlayer callbacks of specified types to the
    BREW IMedia layer.
==================================================================*/
void CMediaReal_CallBack(unsigned long userData, unsigned long cbEnumVal, unsigned long param1, unsigned long param2)
{
   int MMStatus = 0;
   int cmd      = 0;
   int subcmd      = 0;
   CMediaReal *pme = (CMediaReal *)userData;
   void *data = NULL;
   long dataSize = 0;
   static unsigned long bufferingPercent = 0;
   static unsigned long position = 0;

   switch (cbEnumVal)
   {
   case OEM_MEDIA_REAL_CB_ONFRAME:
      pme->m_state = OEM_MEDIA_REAL_ACTIVE;
      cmd      = MM_CMD_PLAY;
      MMStatus = MM_STATUS_FRAME;
      break;

   case OEM_MEDIA_REAL_CB_ONPOSLENGTH:
#ifdef REAL_PLAYER_DIAG
      MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, ">>>CALLBACK = ONPOSLENGTH", 0, 0, 0);
#endif
      position = param1;
      if (param2 > 0 && CMediaReal_ClipDuration == 0)
      {
         CMediaReal_ClipDuration = param2;
         IMediaReal_QueueCallback((void*)pme->m_pReal->m_hObject,
                                 MM_STATUS_DONE,
                                 MM_CMD_GETTOTALTIME,
                                 0,
                                 (void *)CMediaReal_ClipDuration,
                                 sizeof(CMediaReal_ClipDuration));
      }
      break;

   case OEM_MEDIA_REAL_CB_ONPRESENTATIONOPENED:
#ifdef REAL_PLAYER_DIAG
      MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, ">>>CALLBACK = ONPRESENTATIONOPENED", 0, 0, 0);
#endif
      cmd = MM_CMD_PLAY;
      subcmd = 0;
      if (CMediaReal_OpenFailed || CMediaReal_ErrorOccured)
      {
         CMediaReal_OpenFailed = TRUE;
         MMStatus = MM_STATUS_ABORT;
         pme->m_state = OEM_MEDIA_REAL_CLOSED;
      }
      else
      {
         MMStatus = MM_MP4_STATUS_OPEN_COMPLETE;
         bufferingPercent = param2;
         data = (void *)&bufferingPercent;
         dataSize = sizeof(unsigned long);

         if (pme->m_state == OEM_MEDIA_REAL_DISCONNECTED)
         {
            pme->m_state = OEM_MEDIA_REAL_OPENED;
            CMediaReal_Play_internal((IMedia *)pme, CMediaReal_reconnect_pos);
         }
         else if (pme->m_state == OEM_MEDIA_REAL_PENDING || pme->m_state == OEM_MEDIA_REAL_REOPEN_PENDING)
         {
            CMediaReal_reconnect_pos = 0;
            pme->m_state = (pme->m_state == OEM_MEDIA_REAL_REOPEN_PENDING? OEM_MEDIA_REAL_REOPENED: OEM_MEDIA_REAL_OPENED);
         }
      }
      break;

   case OEM_MEDIA_REAL_CB_ONPRESENTATIONCLOSED:
#ifdef REAL_PLAYER_DIAG
      MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, ">>>CALLBACK = ONPRESENTATIONCLOSED", 0, 0, 0);
#endif
      // Flag that presentation is no longer open
      if (!releaseInProgress)
      {
         // Cancel the progress bar update timer
         if (pme->m_pIShell)
         {
            ISHELL_CancelTimer(pme->m_pIShell, IMediaReal_UpdateProgressBar, pme);
         }

         if (pme->m_state == OEM_MEDIA_REAL_DISCONNECTED && pme->m_connectAttempt < CONNECT_ATTEMPTS)
         {
            int nRet = EFAILED;
            pme->m_connectAttempt++;
            CMediaReal_reconnect_pos = position;
            pme->m_state = OEM_MEDIA_REAL_PENDING;
            CMediaReal_OpenFailed = FALSE;
            CMediaReal_ErrorOccured = FALSE;
            switch( pme->m_md.clsData )
            {
               case MMD_FILE_NAME:
                  nRet = OEMMediaReal_OpenURN(pme->m_pszFile);
                  break;
               case MMD_BUFFER:  /* playback from buffer */
#ifdef FEATURE_REAL_PLAYER_BUFFERED_PLAYBACK_SUPPORTED
                  nRet = OEMMediaReal_OpenBuffer(pme->m_md.pData, pme->m_md.dwSize);
#else
                  nRet = EUNSUPPORTED;
#endif
                  break;
            }
            if (nRet != SUCCESS)
            {
               pme->m_state = OEM_MEDIA_REAL_CLOSED;
               CMediaReal_ReturnToReadyState(pme);
            }
         }
         else
         {
            position = 0;
            CMediaReal_reconnect_pos = 0;
            pme->m_state = OEM_MEDIA_REAL_CLOSED;
            pme->m_connectAttempt = 0;
            CMediaReal_ReturnToReadyState(pme);
        }
      }
      break;

   case OEM_MEDIA_REAL_CB_ONSTATISTICSCHANGED:
#ifdef REAL_PLAYER_DIAG
      MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, ">>>CALLBACK = ONSTATISTICSCHANGED", 0, 0, 0);
#endif
      break;

   case OEM_MEDIA_REAL_CB_ONPRESEEK:
#ifdef REAL_PLAYER_DIAG
      MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, ">>>CALLBACK = ONPRESEEK", 0, 0, 0);
#endif
      MMStatus = MM_STATUS_SEEK;
      if (CMediaReal_OpenFailed || CMediaReal_ErrorOccured)
      {
         MMStatus = MM_STATUS_SEEK_FAIL;
      }
      cmd = MM_CMD_PLAY;
      data = (void *)param2;
      dataSize = sizeof(unsigned long);
      break;

   case OEM_MEDIA_REAL_CB_ONPOSTSEEK:
#ifdef REAL_PLAYER_DIAG
      MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, ">>>CALLBACK = ONPOSTSEEK", 0, 0, 0);
#endif
      break;

   case OEM_MEDIA_REAL_CB_ONSTOP:
#ifdef REAL_PLAYER_DIAG
      MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, ">>>CALLBACK = ONSTOP", 0, 0, 0);
#endif
      CMediaReal_Resuming = FALSE;
      if (CMediaReal_Valid)
      {
         ISHELL_CancelTimer(pme->m_pIShell,IMediaReal_UpdateProgressBar,pme);
      }
      break;

   case OEM_MEDIA_REAL_CB_ONPAUSE:
#ifdef REAL_PLAYER_DIAG
      MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, ">>>CALLBACK = ONPAUSE", 0, 0, 0);
#endif
      MMStatus = MM_STATUS_PAUSE;
      if (CMediaReal_OpenFailed || CMediaReal_ErrorOccured)
      {
         MMStatus = MM_STATUS_PAUSE_FAIL;
      }
      if (CMediaReal_Valid)
      {
         pme->m_state = OEM_MEDIA_REAL_PAUSED;
      }
      cmd      = MM_CMD_PLAY;
      break;

   case OEM_MEDIA_REAL_CB_ONBEGIN:
#ifdef REAL_PLAYER_DIAG
      MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, ">>>CALLBACK = ONBEGIN", 0, 0, 0);
#endif
      break;

   case OEM_MEDIA_REAL_CB_ONBUFFERING:
      if (param2 < 100)
      {
         cmd = MM_CMD_PLAY;
         subcmd = 0;
         MMStatus = MM_MP4_STATUS_BUFFER_UPDATE;
         bufferingPercent = param2;
         data = (void *)&bufferingPercent;
         dataSize = sizeof(unsigned long);
      }
      else
      {
         // Prevent MM_MP4_STATUS_PB_READY callback when re-opening the clip
         if (pme->m_state == OEM_MEDIA_REAL_OPENED)
         {
            cmd = MM_CMD_PLAY;
            subcmd = 0;
            MMStatus = MM_MP4_STATUS_PB_READY;
            bufferingPercent = param2;
            data = (void *)0;
            dataSize = sizeof(unsigned long);
         }
      }
#ifdef REAL_PLAYER_DIAG
      MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, ">>>CALLBACK = ONBUFFERING %d", param2, 0, 0);
#endif
      break;

   case OEM_MEDIA_REAL_CB_ONCONTACTING:
      cmd = MM_CMD_PLAY;
      subcmd = 0;
      MMStatus = MM_MP4_STATUS_CONNECTION_UPDATE;
      data = (void *)0;
      dataSize = 0;
#ifdef REAL_PLAYER_DIAG
      MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, ">>>CALLBACK = ONCONTACTING", 0, 0, 0);
#endif
      break;

   case OEM_MEDIA_REAL_CB_ONPLAYSTARTED:
#ifdef REAL_PLAYER_DIAG
      MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, ">>>CALLBACK = ONPLAYSTARTED", 0, 0, 0);
#endif
      pme->m_state = OEM_MEDIA_REAL_ACTIVE;
      if (CMediaReal_OpenFailed || CMediaReal_ErrorOccured)
      {
         MMStatus = MM_STATUS_ABORT;
         cmd      = MM_CMD_PLAY;
      }
      else
      {
         CMediaReal_reconnect_pos = 0;
         pme->m_connectAttempt = 1;
         if (CMediaReal_Resuming == TRUE)
         {
            // Do not send the start status when resuming from paused state.
            CMediaReal_Resuming = FALSE;
         }
         else
         {
            MMStatus = MM_STATUS_START;
            cmd      = MM_CMD_PLAY;
         }
      }
      break;

   case OEM_MEDIA_REAL_CB_RESUME:
#ifdef REAL_PLAYER_DIAG
      MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, ">>>CALLBACK = RESUME", 0, 0, 0);
#endif
      if (CMediaReal_Valid)
      {
         pme->m_state = OEM_MEDIA_REAL_ACTIVE;
      }
      if (pme->m_state == OEM_MEDIA_REAL_ACTIVE)
      {
         CMediaReal_Resuming = TRUE;
         MMStatus = MM_STATUS_RESUME;
         if (CMediaReal_OpenFailed || CMediaReal_ErrorOccured)
         {
            MMStatus = MM_STATUS_RESUME_FAIL;
         }
         cmd      = MM_CMD_PLAY;
      }
      break;
   case OEM_MEDIA_REAL_CB_GET_VOLUME:
         cmd = MM_CMD_GETMEDIAPARM;
         subcmd = MM_PARM_VOLUME;
         MMStatus = MM_STATUS_DONE;
         data = (void *)param1;
         dataSize = param2;
         break;
   case OEM_MEDIA_REAL_CB_SET_VOLUME:
         cmd = MM_CMD_SETMEDIAPARM;
         subcmd = MM_PARM_VOLUME;
         MMStatus = MM_STATUS_DONE;
         data = (void *)param1;
         dataSize = param2;
         break;
   case OEM_MEDIA_REAL_CB_CLIPSTATS:
         OEMMediaReal_UpdateOutputBuffers(pme->m_pReal, (RealPlayer_MediaSpec *)param1);
         cmd = MM_CMD_GETMEDIAPARM;
         subcmd = MM_MP4_PARM_MEDIA_SPEC;
         MMStatus = MM_STATUS_DONE;
         data = (void *)param1;
         dataSize = param2;
      break;
#ifdef FEATURE_REAL_QDSP_RELEASE_RESTORE
   case OEM_MEDIA_REAL_CB_ONRELEASE:
      MSG_3(MSG_SSID_APPS_QTV, MSG_LEGACY_ERROR, ">>>CALLBACK = ONRELEASE", 0, 0, 0);
      break;
#endif /* FEATURE_REAL_QDSP_RELEASE_RESTORE */
   case OEM_MEDIA_REAL_CB_ERROR:
      CMediaReal_ErrorOccured = TRUE;
      switch (param1)
      {
      case HXR_NET_CONNECT:
      case HXR_SERVER_DISCONNECTED:
         pme->m_state = OEM_MEDIA_REAL_DISCONNECTED;
         break;
      }
      break;
   }
   if (cmd != 0)
   {
      if (CMediaReal_Valid)
      {
         IMediaReal_QueueCallback((void *)pme->m_pReal->m_hObject, MMStatus, cmd, subcmd,
            data, dataSize);
      }
   }
}

#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
/*==================================================================
This function clears the screen and disables the MDP
==================================================================*/
static void OEMMediaReal_MDPDeregister(CMediaReal* pme)
{
   /* Deregister all the regions of the MediaPlayer */
   if( RealMDPVideo.id != 0 )
   {
      MSG_MED( "Deregistering MDP %d", RealMDPVideo.id, 0, 0 );
      mdp_deregister_ovimg(RealMDPVideo.id);
   }
   RealMDPVideo.id = 0;
   pme->m_pReal->m_dimPrev.y = 0;
   pme->m_pReal->m_dimPrev.x = 0;
}

static uint32 OEMMediaReal_MDPCalcTransforms(
  AEEMediaMPEG4RotationType rot,
  AEEMediaMPEG4ScalingType scale
  )
{
  uint32 MDPOperation = 0;

  switch( rot )
  {
    case MM_MPEG4_90_CW_ROTATION:
      MDPOperation |= MDPOP_ROT90;
      break;
    case MM_MPEG4_90_CCW_ROTATION:
      MDPOperation |= MDPOP_ROT270;
      break;
    case MM_MPEG4_180_ROTATION:
      MDPOperation |= MDPOP_ROT180;
      break;
    case MM_MPEG4_NO_ROTATION: /* fall through */
    default:
      MDPOperation |= MDPOP_NOP;
      break;
  }

  switch( scale )
  {
    case MM_MPEG4_0P25X_SHRINK:
      MDPOperation |= MDPOP_VDO_1BY4;
      break;
    case MM_MPEG4_2X_SHRINK:
      MDPOperation |= MDPOP_VDO_1BY2;
      break;
    case MM_MPEG4_0P75X_SHRINK:
      MDPOperation |= MDPOP_VDO_3BY4;
      break;
    case MM_MPEG4_2X_ZOOM:
      MDPOperation |= MDPOP_VDO_X2;
      break;
    case MM_MPEG4_NO_SCALING: /* fall through */
    default:
      MDPOperation |= MDPOP_NOP;
      break;
  }

  return MDPOperation;
}
static void OEMMediaReal_MDPCalcTransformDimensions(
   AEEBitmapInfo  frame,
   AEEPoint      *pTransform
   )
{
   /* Calculate new width and height after rotation and scaling have been
   * applied.
   */
   switch( RealMDPVideo.scaling )
   {
   case MM_MPEG4_0P25X_SHRINK:
      pTransform->x = frame.cx >> 2;
      pTransform->y = frame.cy >> 2;
      break;
   case MM_MPEG4_2X_SHRINK:
      pTransform->x = frame.cx >> 1;
      pTransform->y = frame.cy >> 1;
      break;
   case MM_MPEG4_0P75X_SHRINK:
      pTransform->x = (frame.cx >> 2) * 3;
      pTransform->y = (frame.cy >> 2) * 3;
      break;
   case MM_MPEG4_2X_ZOOM:
      pTransform->x = frame.cx << 1;
      pTransform->y = frame.cy << 1;
      break;
   case MM_MPEG4_NO_SCALING:
      pTransform->x = frame.cx;
      pTransform->y = frame.cy;
      break;
   default:
      break; /* do nothing */
   }

   /* Swap height and width if rotation changes the video dimensions */
   if( MDP_IS_ROTATED_SIDEWAYS() )
   {
      uint32 tempVal = pTransform->x;
      pTransform->x = pTransform->y;
      pTransform->y = tempVal;
   }
}
#endif

static void OEMMediaReal_CalcClipping(
                                      AEERect        rectClip,
                                      AEEPoint       dimImage,  /* Transformed image dimensions */
                                      AEERect       *pImageRect /* Initialize dx,dy to orig frame dimensions */
                                      )
{
   uint16 dxClip = MIN( rectClip.dx, dimImage.x );
   uint16 dyClip = MIN( rectClip.dy, dimImage.y );

   /* Calculate the height and width of the image that will fit into the clipping
   * rectangle 'rectClip'.
   */
#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
   /* Using MDP, we need to specify the coordinates in terms of the original
   * frame dimensions.  Therefore, compare dimImage and rectClip.  Depending on
   * if we're rotated, scale the dimensions using height or width.  Obviously,
   * this gets ugly real quick.
   */
   if( (dimImage.x > rectClip.dx && !MDP_IS_ROTATED_SIDEWAYS()) ||
      (dimImage.y > rectClip.dy && MDP_IS_ROTATED_SIDEWAYS()) )
   {
      if( MDP_IS_ROTATED_SIDEWAYS() )
      {
         pImageRect->dx = (int16) ((uint32)pImageRect->dx) * rectClip.dy / dimImage.y;
      }
      else
      {
         pImageRect->dx = (int16) ((uint32)pImageRect->dx) * rectClip.dx / dimImage.x;
      }
   }
   if( (dimImage.y > rectClip.dy && !MDP_IS_ROTATED_SIDEWAYS()) ||
      (dimImage.x > rectClip.dx && MDP_IS_ROTATED_SIDEWAYS()) )
   {
      if( MDP_IS_ROTATED_SIDEWAYS() )
      {
         pImageRect->dy = (int16) ((uint32)pImageRect->dy) * rectClip.dx / dimImage.x;
      }
      else
      {
         pImageRect->dy = (int16) ((uint32)pImageRect->dy) * rectClip.dy / dimImage.y;
      }
   }
#else
   /* No MDP, so just calculate the veiwable width and height of the video
   * (dx and dy), with the maximum value defined by the video rectangle,
   * m_rectClip.
   */
   pImageRect->dx = dxClip;
   pImageRect->dy = dyClip;
#endif /* FEATURE_QTV_MDP_TRANSFORMATIONS */

   /* Calculate the top-left coordinates of where the video will be
   * displayed on the LCD.  Centers the image if it fits.  Otherwise fits
   * upper-left corner
   */
   pImageRect->x = (uint16)((rectClip.dx - dxClip) / 2 + rectClip.x);
   pImageRect->y = (uint16)((rectClip.dy - dyClip) / 2 + rectClip.y);

}

#if MIN_BREW_VERSIONEx(3,1,3)
static int CMediaReal_SetPlayType(CMediaReal * pme, int32 nPlayType)
{
  if (pme->m_clsSelf != AEECLSID_MEDIAREAL)
     return EUNSUPPORTED;

   if (nPlayType > MM_PLAY_TYPE_REMINDER)
      return EBADPARM;

   pme->m_nPlayType = nPlayType;

#if defined (FEATURE_ODM_UI) && defined(WOLF_5)
#error code not present
#endif // FEATURE_ODM_UI && WOLF_5
   return SUCCESS;
}
#endif

#if defined (FEATURE_ODM_UI) && defined(WOLF_5)
#error code not present
#endif //(FEATURE_ODM_UI) && defined(WOLF_5)

#if defined (FEATURE_ACM)
void CMediaReal_GetResourceCntlData(AEEMedia * po, int32 * nPlayType ,uint32 * acmCB)
{
  CMediaReal* pCMediaReal = (CMediaReal*) po;

  *nPlayType = (int)pCMediaReal->m_nPlayType;

  if (AEEMedia_IsIdleState(po))
  {
     *acmCB = (uint32) CMediaReal_AcmSetMediaDataCB;
  }
  else
  {
     *acmCB = (uint32) CMediaReal_AcmPlayCB;
  }
}

static int CMediaReal_AcmSetMediaDataCB(ACM_Callback_data_type *po)
{
  AEEMedia *      pAEEMedia = (AEEMedia *)(po->pdata);
  CMediaReal *  pCMediaReal = (CMediaReal *) (po->pdata);
  int32 nRet = SUCCESS;

  if (po->state == ACM_RESOURCE_ACQUIRED)
  {
    pAEEMedia->m_bResOwner = TRUE;
    nRet = CMediaReal_SetMediaData_internal (pCMediaReal);
    if (nRet != SUCCESS)
    {
      AEEMedia_CleanUpResource(pAEEMedia);
    }
  }
  else
  {
    // FORCE RELEASE from ACM
    if(pAEEMedia->m_bForceRelease == FALSE)
    {
      pAEEMedia->m_bForceRelease = TRUE;
      if (pAEEMedia->m_bResOwner == TRUE)
      {
         nRet = CMediaReal_Stop((IMedia *)pAEEMedia);
         pAEEMedia->m_bResOwner = FALSE;
      }
      else
      {
        AEEMedia_CleanUpResource(pAEEMedia);
      }
    }
  }
  return nRet;
}

static int CMediaReal_AcmPlayCB(ACM_Callback_data_type *po)
{
  AEEMedia *      pAEEMedia = (AEEMedia *)(po->pdata);
  CMediaReal * pCMediaReal = (CMediaReal *) (po->pdata);
  int32 nRet = SUCCESS;

  if (po->state == ACM_RESOURCE_ACQUIRED)
  {
    pAEEMedia->m_bResOwner = TRUE;
    nRet = OEMMediaReal_Play_internal(pCMediaReal, 0);

    if (nRet != SUCCESS)
    {
      AEEMedia_CleanUpResource(pAEEMedia);
    }
  }
  else
  {
    // FORCE RELEASE from ACM
    if(pAEEMedia->m_bForceRelease == FALSE)
    {
      pAEEMedia->m_bForceRelease = TRUE;
      if (pAEEMedia->m_bResOwner == TRUE)
      {
         nRet = CMediaReal_Stop((IMedia *)pAEEMedia);
         pAEEMedia->m_bResOwner = FALSE;
      }
      else
      {
         AEEMedia_CleanUpResource(pAEEMedia);
      }
    }
  }
  return nRet;
}
#endif /* #ifdef FEATURE_ACM */
