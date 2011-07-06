/*=========================================================================

FILE: OEMMediaMPEG42PV.cpp

SERVICES: IMEDIA MPEG4 to PV interface implementation

DESCRIPTION: This is a think interface between the IMedia interface for
             MPEG4 and the Packet Video (PV) decoder

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

Copyright: 1999-2010 QUALCOMM Incorporated. All Rights Reserved.
QUALCOMM Proprietary/GTDR
=========================================================================*/
/*!
  @file   OEMMediaMPEG42PV.cpp
  @author qtv.oemlayer.team
  
  @brief
  Maps the IMediaMPEG4 API to the QtvPlayer API

  @detail
  This is a thin interface between the IMedia interface for MPEG4 and the
  QtvPlayer.
  
*/
/*=========================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/staticextensions/oem/main/latest/src/oemmediampeg42pv.cpp#65 $
$DateTime: 2010/11/29 03:01:53 $
$Change: 1533305 $

=========================================================================*/

/*=========================================================================

                                 INCLUDE FILES

=========================================================================*/

extern "C"
{
#include "customer.h"
#include "assert.h"
#include "snd.h"
#ifdef FEATURE_CMI
#error code not present
#else
#include "clk.h"
#endif

#ifdef FEATURE_CMI_MM
#error code not present
#endif

#include "uixsnd.h"
#if defined(FEATURE_QTV_IPL_SCALING) || defined(FEATURE_MP4_AAC_PLUS)
# include "qtv_ipl.h"
#endif /* FEATURE_QTV_IPL_SCALING */
#include "task.h"
#include "msg.h"
#include "err.h"
}

#include "QtvPlayer.h"

extern "C"
{
#include <fs_fcntl.h>
#include <fs_public.h>
typedef int fs_handle_type;
#include "BREWVersion.h"
#include "OEMMediaMPEG4.h"
#include "OEMCriticalSection.h"
#ifdef   FEATURE_APP_MPEG4
#include "rex.h"

#include "OEMFeatures.h"
#include "AEEStdLib.h"
#include "AEEstd.h"
#include <stdio.h>
#include "stdlib.h"

#ifdef FEATURE_QTV_MDP
#include "IYCbCr.h"
#include "YCbCr_priv.h"
#else
#if MIN_BREW_VERSION(4,0)
#elif MIN_BREW_VERSION(3,0)
# include "OEMBitmapFuncs_priv.h"
# include "AEE_OEMHeap.h"
# include "OEMBitmap_priv.h"
#endif /* MIN_BREW_VERSION(3,0) */
#include "OEMHeap.h"
#endif /* FEATURE_QTV_MDP */

#include "AEEFile.h"
#include "AEE_OEMDispatch.h"

#ifdef FEATURE_QTV_DRM_DCF
  #include "IxStreamUtils.h"
  #include "IxStream.h"
  #include "IxErrno.h"
#endif


#if defined(PLATFORM_LTK) && defined(FEATURE_MP4_COMPLIANCE_TESTING)
#error code not present
#endif

#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
/* need IDIB colorscheme for YUV data returned by Qtv */
#include "IOverlay.h"
#endif

#ifdef FEATURE_USE_CLKREGIM_DEVMAN_REV2
#include "clkrgm_devman.h" /*! Allows us to bump the display clock */
#endif /* FEATURE_USE_CLKREGIM_DEVMAN_REV2 */

#ifdef T_MSM7500
#error code not present
#endif /* T_MSM7500 */
}

#ifndef PLATFORM_LTK
 #if MIN_BREW_VERSION(4,0)
  #include "AEEComdef.h"
  #include "OEMefs.h"
  /* Brew 4.0 doesn't declare these functions anymore. */
  extern "C" AEE_EXPORTS int AEE_CheckPathAccess(const char *cpszFile, uint32 dwDesiredRights, uint32 *pdwActualRights);
  extern "C" AEE_EXPORTS int AEE_ResolvePath(const char *cpszIn, char *pszOut, int *pnOutLen);
 #else
  #include "AEE_OEMFile.h"
  #include "OEMFS.h"
 #endif /* MIN_BREW_VERSION(4,0) */
#endif

static OEMCriticalSection releaseCS;
# define MUTEX_LOCK() OEMCriticalSection_Enter(&releaseCS)
# define MUTEX_UNLOCK() OEMCriticalSection_Leave(&releaseCS)
#if !MIN_BREW_VERSION(3,0)
# error "OEMMediaMPEG4 requires BREW 3.0 or later.  <qtv.oemlayer.team>"
#endif /* !MIN_BREW_VERSION(3,0) */

/*=========================================================================

                               MACRO DEFINITIONS

=========================================================================*/

#define RTSP_METHOD_DESCRIBE             "RTSP_METHOD_DESCRIBE"
#define RTSP_METHOD_SETUP		 "RTSP_METHOD_SETUP"
#define RTSP_METHOD_PLAY		 "RTSP_METHOD_PLAY"
#define RTSP_METHOD_PAUSE		 "RTSP_METHOD_PAUSE"
#define RTSP_METHOD_TEARDOWN		 "RTSP_METHOD_TEARDOWN"
#define RTSP_METHOD_OPTIONS_RESPONSE     "RTSP_METHOD_OPTIONS_RESPONSE"

#define RTSP_USER_AGENT			 "RTSP_USER_AGENT"
#define RTSP_USER_AGENT_PARAM		 "RTSP_USER_AGENT_PARAM"
#define RTP_UDP_DATA_PORT_RANGE	         "RTP_UDP_DATA_PORT_RANGE"

#define OEM_RTSP_HEADER_ADD              "OEM_RTSP_HEADER_ADD"
#define OEM_RTSP_HEADER_DELETE           "OEM_RTSP_HEADER_DELETE"

#define FEATURE_MP4_ENABLE_SET_DEBUG_VARIABLES
#define FEATURE_MP4_WRITE_CLIP_STATS_TO_FILE

#ifdef FEATURE_MP4_ENABLE_SET_DEBUG_VARIABLES
#define DEBUG_MAX_STRING_LENGTH        2048
#define DEBUG_URL_FILE_EXTENSION       ".url"
#define PVX_FILE_EXTENSION             ".pvx"
#define DEBUG_VARIABLE_VALUE_SEPARATOR '='
#define DEBUG_URL_FILE_TOKEN           "debug:"
#endif // FEATURE_MP4_ENABLE_SET_DEBUG_VARIABLES

/* How long to wait for the player's state to go to IDLE */
#define PLAYER_TRANSITION_TO_IDLE_TIMEOUT 10000 /* in ms */

/* How often to poll the player for its state to go to IDLE */
#define PLAYER_STATE_POLL_INTERVAL          200 /* in ms */

#define MASK_24BIT_BOUNDARY 0xFF000000
#define CROSSES_24BIT_BOUNDARY(ptr, n) \
  (((int)(ptr) & MASK_24BIT_BOUNDARY) != (((int)(ptr) + (n) - 1) & MASK_24BIT_BOUNDARY))

#define QTV_QVGA_SIZE                    115200 /* QVGA is 320 x 240 x 1.5 bytes/pixel */

#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
#define SCALE_UP_FACTOR 4                 // current MDP scaling limitations
#define SCALE_DOWN_FACTOR 4
#define MDP_IS_ROTATED_SIDEWAYS(a) \
   (((a) == OEM_90_CW_ROTATION) || \
    ((a) == OEM_90_CCW_ROTATION))
#endif /* FEATURE_QTV_MDP_TRANSFORMATIONS */

#if MIN_BREW_VERSION(4,0)
#define MEDIAPLAYER_ROOT_PATH                 "/mod/mediaplayer/media/"
#else
#define MEDIAPLAYER_ROOT_PATH                 "/brew/mod/mediaplayer/media/"
#endif //MIN_BREW_VERSION(4,0)
/*=========================================================================

                             EXTERNAL DECLARATIONS

=========================================================================*/

#if (defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION)
#error code not present
#endif /* defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION) */

/*=========================================================================

                              FORWARD DECLARATIONS

=========================================================================*/

#ifdef FEATURE_MP4_ENABLE_SET_DEBUG_VARIABLES
static int CheckDebugURLFile(char *videoFileName, char *audioFileName);
#endif // FEATURE_MP4_ENABLE_SET_DEBUG_VARIABLES
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
static bool OEMMediaMPEG42PV_PrepareTimedText(OEMHandle pOEM);
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
#ifdef FEATURE_QTV_ISDB_SUBTITLES
#error code not present
#endif /* FEATURE_QTV_ISDB_SUBTITLES */
static void CopyMPEG4SpecFromClipInfo(OEMMediaMPEG4Layer * pOEM);
static AEEMediaMP4CodecType OEMMediaMPEG42PV_GetAudioCodecType(OEMHandle pOEM);
static AEEMediaMP4CodecType OEMMediaMPEG42PV_GetVideoCodecType(void);
static AEEMediaMPEG4Type OEMMediaMPEG42PV_ConvertMediaTypePVToAEE(QtvPlayer::MediaTypeT pvType);
static AEEStreamType OEMMediaMPEG42PV_ConvertStreamType(QtvPlayer::StreamTypeT);
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
static AEEMediaPvxPlaybackControl OEMMediaMPEG42PV_ConvertPlaybackControlType(QtvPlayer::DownloadPlaybackControlT);
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */
#ifdef FEATURE_QTV_IPL_SCALING
#error code not present
#endif /* FEATURE_QTV_IPL_SCALING */
#if (defined (FEATURE_MP4_AAC_PLUS) || defined (PLATFORM_LTK) )
/* Qtv IPL to support color conversion & rotation */
static void RotateColorConvert( QtvPlayer::FrameInfoT* pFrameInfo, OEMHandle pOEM );
#endif /* FEATURE_MP4_AAC_PLUS || PLATFORM_LTK */
static AEEMediaEndOfClipAction OEMMediaMPEG42PV_ConvertEndofClipActionType(EndOfClipActionT);
static int OEMMediaMPEG42PV_CheckTransform(OEMRotationType, OEMScalingType,
                                           uint16, uint16, OEMHandle);
#ifdef FEATURE_QTV_OEM_BUFFER_MGR
static void AddRefOutputBuffer(void* pBuf);
#endif /* FEATURE_QTV_OEM_BUFFER_MGR */
/*=========================================================================

                                TYPE DEFINITIONS

=========================================================================*/
#ifdef FEATURE_MP4_ENABLE_SET_DEBUG_VARIABLES
enum DebugVariableType
{
  DEBUG_VARIABLE_TYPE_END,
  DEBUG_VARIABLE_TYPE_INTEGER,
  DEBUG_VARIABLE_TYPE_STRING
};

typedef struct
{
  DebugVariableType  type;
  const char        *variable;
  void              *value;
  uint32             length; // this is used only when type == string
}DebugValueType;
#endif // FEATURE_MP4_ENABLE_SET_DEBUG_VARIABLES

/*=========================================================================

                                DATA DEFINITIONS

=========================================================================*/
extern "C" {
  extern uint32              g_uMPEG4RefCnt;
  extern OEMMediaMPEG4Layer  *g_pOEMLayer[MAX_NUM_OEM_PLAYBACK_INST];
}

static AEEVTBL(IBitmap)* g_pvtIBitmap;

static QtvPlayer::FetchBufferedDataSizeT  g_cbFetchBufferedDataSize = NULL;
static QtvPlayer::FetchBufferedDataT      g_cbFetchBufferedData = NULL;  
static QtvPlayer::IsMimeTypeSupportedT    g_cbIsMimeSupported = NULL;

static void*                              g_pClientData = NULL;
AEEFetchBufferedDataSizeT                 g_FetchBufferSizeCB = NULL;
AEEFetchBufferedDataT                     g_FetchBufferCB = NULL;

#ifdef FEATURE_USE_CLKREGIM_DEVMAN_REV2
/*! Temporary control of display clocks */
static clkrgm_devman_client_type g_ClkrgmDmClient;
static clkrgm_device_param_type g_ClkrgmDevParam;
#endif /* FEATURE_USE_CLKREGIM_DEVMAN_REV2*/

#ifdef FEATURE_MP4_WRITE_CLIP_STATS_TO_FILE
extern "C"
{
  char mp4WriteClipStatsFileName[AEE_MAX_FILE_NAME] = "";
  uint32 mp4WriteClipStatsToFileFlag = 0;
}
#endif // FEATURE_MP4_WRITE_CLIP_STATS_TO_FILE

#ifdef FEATURE_MP4_ENABLE_SET_DEBUG_VARIABLES
// Adding new debug variable definitions to this table
// allows then to be set using a debug URL File
// See description of CheckDebugURLFile below for details
DebugValueType DebugValueTypeTable[] =
{
#ifdef FEATURE_MP4_WRITE_CLIP_STATS_TO_FILE
   {DEBUG_VARIABLE_TYPE_INTEGER, "WRITE_CLIP_STATS_TO_FILE", &mp4WriteClipStatsToFileFlag, 0},
#endif // FEATURE_MP4_WRITE_CLIP_STATS_TO_FILE
  {DEBUG_VARIABLE_TYPE_END, NULL, NULL, 0} // this must always be the last entry!
};
#endif

#if defined (PLATFORM_LTK) && defined (FEATURE_MP4_COMPLIANCE_TESTING)
#error code not present
#endif


#ifdef FEATURE_QTV_STREAM_RECORD
//static boolean bEnableStreamRecord = FALSE; // Variable not used, compiler warning
static char fName[512] = {'\0'};
static bool bRecFileOverWrite = FALSE;
uint32 rec_clip_duration = 0;
#endif
/*=========================================================================

                           FUNCTION DEFINITIONS

=========================================================================*/
extern "C" void QtvEngineCB(
  QtvPlayer::PlayerStatusT status,
  void *pData,
  QtvPlayer::PlayerStateRecordT state,
  QtvPlayer::InstanceHandleT  handle,
  void * pUserData)
{
  int MMStatus = MM_STATUS_UNKNOWN;
  int cmd = 0;
  int subCmd = 0;
  unsigned long data = 0;
  unsigned long size = 0;
  OEMMediaMPEG4Layer *pOEM = OEMMediaMPEG42PV_GetOEMLayer((OEMHandle)pUserData);
  
#ifdef FEATURE_QTV_PLAYLIST
  bool sendMediaSpec = false;
#endif /* FEATURE_QTV_PLAYLIST */

  MSG_LOW("QtvEngineCB(%d,%d)", status, pData, 0);

  if (pOEM == NULL)  // If we are called after all instances are destroyed
  {
    MSG_ERROR("QtvEngineCB called w/ pOEM==NULL", 0, 0, 0);
    return;
  }

  QtvPlayer::ClipInfoT *pClipInfo = (QtvPlayer::ClipInfoT*)pOEM->m_pClipInfo;

  switch (status)
  {
    case QtvPlayer::QTV_PLAYER_STREAM_AUTH_REQUIRED:

        pOEM->m_nMPEG4EngineState =  QtvPlayer::QTV_PLAYER_STATUS_CONNECTING;
        MMStatus =  MM_MP4_STATUS_CONNECTION_UPDATE;
        OEMMediaMPEG4_QueueCallback(pOEM->m_pCallbackData,
                                    MM_MP4_STATUS_CONNECTION_UPDATE,
                                    MM_CMD_PLAY,
                                    MM_MP4_STREAM_AUTHENTICATE,
                                    (void *)NULL,
                                    0);
        break;

    case QtvPlayer::QTV_PLAYER_STATUS_INFO:
    {
      QtvPlayer::StatusInfoT *StatusInfo;
      StatusInfo = (QtvPlayer::StatusInfoT *)pData;
      OEMMediaMPEG4_QueueCallback(pOEM->m_pCallbackData,
                                   MM_MP4_STATUS_INFO,
                                   MM_CMD_PLAY,
                                   0,
                                   (void *)StatusInfo->Data,
                                   StatusInfo->Size);
      break;
    }

    case QtvPlayer::QTV_COMMAND_PLAY_CLIP_COMPLETE:
      if (!pOEM->m_bPlayStarted)
      {
        cmd = MM_CMD_PLAY;
        MMStatus = MM_STATUS_START;
        pOEM->m_bPlayStarted = TRUE;
      }
      pOEM->m_bSeekPending = FALSE;
      break;

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
    case QtvPlayer::QTV_PLAYER_STATUS_DOWNLOADED_DATA_AVAIL:
      MMStatus = MM_MP4_STATUS_DL_DATA_AVAILABLE;
      OEMMediaMPEG4_QueueCallback(pOEM->m_pCallbackData,
                                   MMStatus,
                                   MM_CMD_PLAY,
                                   0,
                                   (void *)NULL,
                                    0);
      break;


    case QtvPlayer::QTV_PLAYER_STATUS_DOWNLOAD_READY:
      pOEM->m_nStartTime = 0;
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_DOWNLOADING:
      pOEM->m_nStartTime = 0;
      break;    
#endif      

    case QtvPlayer::QTV_COMMAND_PLAY_CLIP_FAILED:
      pOEM->m_bSeekPending = FALSE;
      pOEM->m_bResumePending = FALSE;
      break;

#ifdef FEATURE_FILE_FRAGMENTATION
    case QtvPlayer::QTV_COMMAND_SKIP_CLIP_COMPLETE:
    case QtvPlayer::QTV_COMMAND_SKIP_CLIP_FAILED:
      pOEM->m_bSeekPending = FALSE;
      break;
#endif /*FEATURE_FILE_FRAGMENTATION*/

#ifdef FEATURE_QTV_PLAYLIST
    case QtvPlayer::QTV_COMMAND_PLAYLIST_PLAY_COMPLETE:
    case QtvPlayer::QTV_COMMAND_PLAYLIST_PLAY_FAILED:
      pOEM->m_bSeekPending = FALSE;
      break;
#endif /* FEATURE_QTV_PLAYLIST */

    case QtvPlayer::QTV_PLAYER_STATUS_PLAYBACK_COMPLETE:
      pOEM->m_nStartTime = 0;
      pOEM->m_nCurrentClipIndex = 0;
      pOEM->m_nAccumulatedClipOffset = 0;
#if (defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION)
#error code not present
#endif /* defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION */

#if defined(PLATFORM_LTK) && defined (FEATURE_MP4_COMPLIANCE_TESTING)
#error code not present
#endif
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_PLAYBACK_STOPPED:
      pOEM->m_nStartTime = 0;
      pOEM->m_nCurrentClipIndex = 0;
      pOEM->m_nAccumulatedClipOffset = 0;
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_PLAYBACK_ERROR_ABORT:
      pOEM->m_nStartTime = 0;
      pOEM->m_nCurrentClipIndex = 0;
      pOEM->m_nAccumulatedClipOffset = 0;

      MMStatus = MM_STATUS_ABORT;
      cmd = MM_CMD_PLAY;
      data = status;
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_AUDIO_ERROR:
    case QtvPlayer::QTV_PLAYER_STATUS_AUDIO_ERROR_FORMAT_NOT_SUPPORTED:
    case QtvPlayer::QTV_PLAYER_AUDIO_UNSUPPORTED_BIT_RATE:
      MMStatus = MM_MP4_STATUS_AUDIO_ABORT;
      cmd = MM_CMD_PLAY;
      data = status;
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_VIDEO_ERROR:
    case QtvPlayer::QTV_PLAYER_STATUS_VIDEO_ERROR_FORMAT_NOT_SUPPORTED:
    case QtvPlayer::QTV_PLAYER_VIDEO_UNSUPPORTED_BIT_RATE:
    case QtvPlayer::QTV_PLAYER_VIDEO_UNSUPPORTED_PROFILE:
      MMStatus = MM_MP4_STATUS_VIDEO_ABORT;
      cmd = MM_CMD_PLAY;
      data = status;
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_OPENING:
       pOEM->m_nMPEG4EngineState = status;
       break;

    case QtvPlayer::QTV_PLAYER_STATUS_CONNECTING:
      pOEM->m_nMPEG4EngineState = status;
      UpdateProgressBar((void*)pOEM);  // indicate CONNECT is in progress
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_BUFFERING:
      pOEM->m_nMPEG4EngineState = status;
      pOEM->m_nPrerollPercentage =  *((uint32 *)pData);
      UpdateProgressBar((void*)pOEM);
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2      
      if(QtvPlayer::getDownloadClipType(handle) != QtvPlayer::NONE ) 
      {
        UpdateDownloadProgressBar((void*)pOEM);
      }
#endif   
      break;


#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  case QtvPlayer::QTV_PS_PLAYER_BUFFERING:
  case QtvPlayer::QTV_HTTP_PLAYER_BUFFERING:
    pOEM->m_nMPEG4EngineState = status;
    if(NULL != pData)
    {
      pOEM->m_nPsdownloadrate= *((uint32 *)pData);
    }
    else
    {
      pOEM->m_nPsdownloadrate= 0;
    }
      UpdateProgressBar((void*)pOEM);
    break;

#endif /* FEATURE_QTV_PSEUDO_STREAM || FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */

   case QtvPlayer::QTV_PLAYER_TS_LAYER_PRESENT:
       pOEM->m_nMPEG4EngineState = status;
       MSG_HIGH("QtvEngineCB:QTV_PLAYER_TS_LAYER_PRESENT", 0, 0, 0);
       cmd = MM_CMD_PLAY;
       MMStatus = MM_MP4_STATUS_TS_LAYER_PRESENT;       
       break;
#ifdef FEATURE_QTV_FCS
#error code not present
#endif
    case QtvPlayer::QTV_PLAYER_STATUS_PLAYBACK_READY:
      MSG_HIGH("QtvPlayer::QTV_PLAYER_STATUS_PLAYBACK_READY",0,0,0);
      pOEM->m_nMPEG4EngineState = status;
      // playback ready callback received, reset playback ready pending flag.
      pOEM->m_bPBReadyPending = FALSE;
      if (QtvPlayer::GetClipInfo(*pClipInfo, handle) == QtvPlayer::QTV_RETURN_OK)
      {
        CopyMPEG4SpecFromClipInfo(pOEM);
        pOEM->m_bClipInfoAvail = TRUE;
        #ifdef FEATURE_QTV_LOW_POWER_AUDIO
        /* if LOW POWER AUDIO is defined, for audio only AAC (currently it only supports AAC
           CODEC)file playback we increase tick time interval (so that ARM can sleep more).
           This we do only if OEM has not changed the tick interval (means they don't care
           and using default) */
        if ((pClipInfo->MediaType == QtvPlayer::MEDIA_AUDIO_ONLY)&&
            (pClipInfo->streamtype == QtvPlayer::MEDIA_NON_STREAMING))
        {
          for (int i = 0; i < QTV_MAX_MEDIA_TRACKS; i++)
          {
            if (pClipInfo->TrackTypes[i] == QtvPlayer::TRACK_AUDIO_MP4A_LATM)
            {
              if(pOEM->m_nTickInterval == MM_DEFAULT_TICK_INTERVAL)
              {
                pOEM->m_nTickInterval = MM_DEFAULT_TICK_INTERVAL*2;
              }
            }
          }
        }
        #endif /* FEATURE_QTV_LOW_POWER_AUDIO */
      }
      else
      {
        MEMSET(pClipInfo, 0, sizeof(QtvPlayer::ClipInfoT));
        pOEM->m_bClipInfoAvail = FALSE;
      }

      if (pOEM->m_cbGenerateTotalTime == TRUE)
      {
        int     totalTimeStatus = MM_STATUS_ABORT;
        uint32  duration = 0;

        pOEM->m_cbGenerateTotalTime = FALSE;
        if (pOEM->m_bClipInfoAvail)
        {
          totalTimeStatus = MM_STATUS_DONE;
          duration = pOEM->m_MPEG4Spec.dwDuration;
        }
        MSG_HIGH("QtvPlayer::QTV_PLAYER_STATUS_PLAYBACK_READY, notifying MM_CMD_GETTOTALTIME ",0,0,0);

        OEMMediaMPEG4_QueueCallback((void *)pData,
                                    totalTimeStatus,
                                    MM_CMD_GETTOTALTIME,
                                    0,
                                    (void *)duration,
                                    sizeof(duration));
      }

      if (pOEM->m_cbGenerateMediaSpec == TRUE)
      {
        int mediaSpecStatus;

        pOEM->m_cbGenerateMediaSpec = FALSE;
        mediaSpecStatus = (pOEM->m_bClipInfoAvail ? MM_STATUS_DONE : MM_STATUS_ABORT);

        OEMMediaMPEG4_QueueCallback((void *)pData,
                                    mediaSpecStatus,
                                    MM_CMD_GETMEDIAPARM,
                                    MM_MP4_PARM_MEDIA_SPEC,
                                    (void *)&pOEM->m_MPEG4Spec,
                                    sizeof(pOEM->m_MPEG4Spec));
      }

      // Indicate transition to PB Ready state to the user -(ie no longer Connecting)
      OEMMediaMPEG4_QueueCallback(pOEM->m_pCallbackData,
                                  MM_MP4_STATUS_PB_READY,
                                  MM_CMD_PLAY,
                                  0,
                                  (void *)NULL,
                                  0);

      cmd = MM_CMD_PLAY;
      if (status == QtvPlayer::QTV_PLAYER_STATUS_SWITCHING_IN_PROGRESS)
      {
        MMStatus = MM_MP4_STATUS_FCS_SWITCHING;
      }
      else
      {
         MMStatus = MM_STATUS_REPOSITIONING;
         data = pClipInfo->RepositioningAllowed;
         size = sizeof(data);
         pOEM->m_bStreamingError = FALSE;
         pOEM->m_bStreamingError = FALSE;
      }
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_PLAYING:
    case QtvPlayer::QTV_PLAYER_STATUS_PLAYING_1P3X:
      pOEM->m_nMPEG4EngineState = status;
      pOEM->m_bSuspend = FALSE;

      UpdateProgressBar((void*)pOEM);
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2      
      if(QtvPlayer::getDownloadClipType(handle) != QtvPlayer::NONE ) 
      {
        UpdateDownloadProgressBar((void*)pOEM);
      }
#endif      
      cmd = MM_CMD_PLAY;
      if (pOEM->m_bPaused == TRUE)
      {
        MMStatus = MM_STATUS_RESUME;
        pOEM->m_bPausePending = FALSE;
        pOEM->m_bPaused = FALSE;
        pOEM->m_bResumePending = FALSE;
      }
      else
      {
        // Added for CR 109813 fix, IMedia Documentation states that MM_STATUS_SEEK callback just indicates that 
        // the seek command is started but doesn't indicate that seek is done. MM_STATUS_RESUME callback 
        // indicates that the seek command is finished and playback resumes with correct elapsed time. 
        if(pOEM->m_bResumePending == TRUE)
        {
          pOEM->m_bResumePending = FALSE;
          MMStatus = MM_STATUS_RESUME;
          OEMMediaMPEG4_QueueCallback(pOEM->m_pCallbackData, MMStatus, cmd, subCmd, 
                                    (void *) data, size);
        }

        MMStatus = MM_MP4_STATUS_PLAYING;
      }
      break;

    /* m_bSuspend is set to TRUE as soon as the QtvPlayer starts internal suspension.
       No IMEDIA commands other than _Release, _Pause, and _Stop are accepted if the 
       QtvPlayer is suspended.  As soon as the player resumes normal Playing or Paused
       mode m_bSuspend is set back to FALSE. 
    */
    case QtvPlayer::QTV_PLAYER_STATUS_SUSPENDING:
      pOEM->m_bSuspend = TRUE;
      MSG_HIGH("Internally suspending.", 0, 0, 0);
      break;

    /* In live streaming pause is not allowed, so if pause is called from OEM 
       layer then need to notify pause is failed in this scenario.
     */
    case QtvPlayer::QTV_COMMAND_PAUSE_FAILED:
         pOEM->m_bPausePending = FALSE;
         MSG_HIGH("Pause is failed.", 0, 0, 0);
         break;

    case QtvPlayer::QTV_PLAYER_STATUS_PAUSED:
    case QtvPlayer::QTV_PLAYER_STATUS_PAUSED_SUSPENDED:
         if(QtvPlayer::QTV_PLAYER_STATUS_PAUSED_SUSPENDED == status)
         {		
   	    /* QTV_PLAYER_STATUS_PAUSED_SUSPENDED is notified only in case when qtv is paused because of incoming call */
            /* So release the CMX resource as we are done with qtv suspend handler */
    	    OEMMediaMPEG4_Conc_ACM_Stop(pOEM->m_pMedia);
	 }
         pOEM->m_bSuspend = FALSE;
      // This is to handle the case when PAUSE during connecting state of a streaming clip.
      if(pOEM->m_bPBReadyPending == TRUE)
      {
        pOEM->m_bSendTotalTime = TRUE;
      }
      
      if (pOEM->m_bPaused == FALSE)
      {
        pOEM->m_nStartTime = state.playbackMsec;
      }
      pOEM->m_nMPEG4EngineState = status;
      cmd = MM_CMD_PLAY;
      MMStatus = MM_STATUS_PAUSE;
      pOEM->m_bPaused = TRUE;
      pOEM->m_bPausePending = FALSE;
      // reset resume pending.  Add this for the case, after SEEK request, 
      // while BUFFERING, send PAUSE and then RESUME request, the RESUME 
      // will fail if resume pending flag is not reset here.
      pOEM->m_bResumePending = FALSE;
      pOEM->m_bPauseInProcess = FALSE;
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_REPOSITIONING:
      pOEM->m_nMPEG4EngineState = status;
      if (pOEM->m_bPaused == FALSE)
      {
        cmd = MM_CMD_PLAY;
        MMStatus = MM_STATUS_SEEK;
      }
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_NO_SYNC_FRAME_ERROR:
      pOEM->m_nMPEG4EngineState = status;
      pOEM->m_bSeekPending = FALSE;
      if (pOEM->m_bPaused == FALSE)
      {
        MSG_ERROR("No Sync Frames. Repositioning failed.", 0, 0, 0);
        cmd = MM_CMD_PLAY;
        MMStatus = MM_MP4_STATUS_NO_SYNC_FRAME;
      }
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_IDLE:
      // This is to handle the case when STOP during connecting state of a streaming clip.
      if(pOEM->m_bPBReadyPending == TRUE)
      {
        pOEM->m_bSendTotalTime = TRUE;
      }

      pOEM->m_nStartTime = 0;
      pOEM->m_nCurrentClipIndex = 0;
      pOEM->m_nAccumulatedClipOffset = 0;

      pOEM->m_nMPEG4EngineState = status;
      pOEM->m_bPaused = FALSE;
      pOEM->m_bResumePending = FALSE;
      pOEM->m_bPlayStarted = FALSE;
      pOEM->m_bSeekPending = FALSE;
      pOEM->m_bSuspend = FALSE;
      pOEM->m_bPauseInProcess = FALSE;
      /* tell user that player has done playing */
      cmd = MM_CMD_PLAY;
      MMStatus = MM_STATUS_DONE;
#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
      pOEM->m_bFrameTransformPending = FALSE;
#endif
#ifdef FEATURE_QTV_IPL_SCALING
#error code not present
#endif /* FEATURE_QTV_IPL_SCALING */

#ifdef FEATURE_MP4_AAC_PLUS
      /* Initialized to no rotation */
      pOEM->m_IPLRotationFactor = MM_MPEG4_NO_ROTATION;
#endif /* FEATURE_MP4_AAC_PLUS */

      /* Restore sound device setting after playback.  Note: we don't necessarily
       * want to mute the sound device after playback.  This is because Qtv may
       * have been launched while a voice call is up and so when the playback is
       * over we want to resume the call and still hear voice.
       */
#ifdef UI_SND_RESTORE_MUTE
      uisnd_restore_mute(NULL,NULL);
#elif defined (UI_SND_DEV_MGMT)
      uisnd_set_device_auto(NULL, NULL);
#else
      uisnd_set_device( SND_MUTE_MUTED, SND_MUTE_MUTED, FALSE );
#endif
      break;

      /* The PV code never sends this error! */
    case QtvPlayer::QTV_PLAYER_STATUS_STREAM_ERROR:
    case QtvPlayer::QTV_PLAYER_STATUS_STREAM_SERVER_CLOSED:
    case QtvPlayer::QTV_PLAYER_STATUS_SERVER_NOT_ENOUGH_BW:
    case QtvPlayer::QTV_PLAYER_STATUS_DATA_INACTIVITY_ERROR:
      pOEM->m_nStartTime = 0;
      pOEM->m_bStreamingError = TRUE;
      /* Fall through */
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2    
    case QtvPlayer::QTV_PLAYER_STATUS_DOWNLOAD_ERROR_ABORT:
#endif 
    case QtvPlayer::QTV_PLAYER_STATUS_VIDEO_ONLY_PLAYBACK_NOT_ALLOWED:
    case QtvPlayer::QTV_PLAYER_STATUS_AUDIO_ONLY_PLAYBACK_NOT_ALLOWED:
    case QtvPlayer::QTV_PLAYER_STATUS_PLAY_TIME_ERROR:
      cmd = MM_CMD_PLAY;
      MMStatus = MM_STATUS_ABORT;
      data = status;
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_NEXT_FRAME_DONE:
    case QtvPlayer::QTV_PLAYER_STATUS_PREV_FRAME_DONE:
      cmd = MM_CMD_PLAY;
      MMStatus = MM_STATUS_SEEK;
      pOEM->m_nStartTime = state.playbackMsec;
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_NEXT_FRAME_FAILED:
    case QtvPlayer::QTV_PLAYER_STATUS_PREV_FRAME_FAILED:
      cmd = MM_CMD_PLAY;
      MMStatus = MM_STATUS_SEEK_FAIL;
      data = MM_SEEK_MODE_FRAME;
      size = sizeof(data);
      break;

#ifdef FEATURE_QTV_ISDB_SUBTITLES
#error code not present
#endif /* FEATURE_QTV_ISDB_SUBTITLES*/

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
        case QtvPlayer::QTV_PLAYER_STATUS_NEXT_TEXT_READY:
      if( 
#ifdef FEATURE_QTV_GENERIC_BCAST
         !(pOEM->m_nBcastMutePending & MM_QTV_TEXT_TRACK) &&
#endif /* FEATURE_QTV_GENERIC_BCAST */
          OEMMediaMPEG42PV_PrepareTimedText(pOEM) )
      {
        cmd = MM_CMD_PLAY;
        MMStatus = MM_MP4_STATUS_3GPP_TTEXT;
        data = (uint32)&pOEM->m_TimedText3GPP;
        size = sizeof(pOEM->m_TimedText3GPP);
      }
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_TEXT_ERROR:
      cmd = MM_CMD_PLAY;
      MMStatus = MM_MP4_STATUS_3GPP_TTEXT;
      data = (uint32)NULL;
      size = sizeof(data);
      break;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
    case QtvPlayer::QTV_PLAYER_STATUS_NEXT_TELOP_READY:
      {
        QtvPlayer::TelopElementT telopElement;
        if( QtvPlayer::GetNextTelopElement(telopElement) == QtvPlayer::QTV_RETURN_OK)
        {
          pOEM->m_TelopTextElement.dwTelopSize          = telopElement.TelopSize;
          pOEM->m_TelopTextElement.dwBeginTime          = telopElement.BeginTime;
          pOEM->m_TelopTextElement.dwDuration           = telopElement.Duration;
          pOEM->m_TelopTextElement.dwNumberOfSubStrings = telopElement.NumberOfSubStrings;
          pOEM->m_TelopTextElement.bWrapFlag            = telopElement.WrapFlag;
          pOEM->m_TelopTextElement.dwBackgroundColor    = telopElement.BackgroundColor;
          pOEM->m_TelopTextElement.dwDefaultFontColor   = telopElement.DefaultFontColor;
          /* send command to media player */
          cmd = MM_CMD_PLAY;
          MMStatus = MM_MP4_STATUS_TELOP_TEXT;
          data = (uint32)&pOEM->m_TelopTextElement;
          size = sizeof(pOEM->m_TelopTextElement);
          break;
        }
      }
      break;
#endif /* FEATURE_MP4_KDDI_TELOP_TEXT */
    case QtvPlayer::QTV_PLAYER_STATUS_DECODE_FRAME:
      /* Queue the MM_STATUS_FRAME callback.  If frame callbacks are not
       * enabled, this will be handled in OEMMediaMPEG4_CallbackNotify.
       */
      cmd = MM_CMD_PLAY;
      MMStatus = MM_STATUS_FRAME;
      break;

#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
    /* We need to notify the player when a rotation or scale operation succeeds
     * or fails so that it can update its absolute orientation / scale factor.
     */
    case QtvPlayer::QTV_PLAYER_STATUS_ROTATION_DONE:
      cmd = MM_CMD_SETMEDIAPARM;
      subCmd = MM_MP4_PARM_ROTATION;
      MMStatus = MM_STATUS_DONE;
      pOEM->m_bFrameTransformPending = FALSE;
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_ROTATION_FAILED:
      cmd = MM_CMD_SETMEDIAPARM;
      subCmd = MM_MP4_PARM_ROTATION;
      MMStatus = MM_STATUS_ABORT;
      pOEM->m_bFrameTransformPending = FALSE;
      break;
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */

    case QtvPlayer::QTV_PLAYER_STATUS_SCALING_DONE:
#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS      
      cmd = MM_CMD_SETMEDIAPARM;
      subCmd = MM_MP4_PARM_SCALING;
      MMStatus = MM_STATUS_DONE;
      pOEM->m_bFrameTransformPending = FALSE;
      break;
#else 
      return;   /* no need to send CB for XScaling */
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */

    case QtvPlayer::QTV_PLAYER_STATUS_SCALING_FAILED:
      cmd = MM_CMD_SETMEDIAPARM;
      subCmd = MM_MP4_PARM_SCALING;
      MMStatus = MM_STATUS_ABORT;
#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
      pOEM->m_bFrameTransformPending = FALSE;
      break;
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */
      /* If DSP scaling failed, and MDP scaling is enabled, use MDP */
#ifdef FEATURE_QTV_XSCALE_VIDEO
#ifdef FEATURE_QTV_MDP_ASCALE
      {
        int nRet = OEMMediaMPEG42PV_CheckTransform(pOEM->m_QtvCurrentRotation, 
                                                   OEM_ASCALE, pOEM->m_QtvCurrentAScale.x,
                                                   pOEM->m_QtvCurrentAScale.y, pOEM);
        if (nRet == SUCCESS) 
        {
          pOEM->m_QtvCurrentScaling = OEM_ASCALE;
          MMStatus = MM_STATUS_DONE;
          break;
        }
      }
#endif /* FEATURE_QTV_MDP_ASCALE */
      pOEM->m_QtvCurrentScaling = OEM_NO_SCALING;
      break;
#endif /* FEATURE_QTV_XSCALE_VIDEO */

    case QtvPlayer::QTV_COMMAND_OPEN_URN_COMPLETE:
      cmd = MM_CMD_PLAY;
      MMStatus = MM_MP4_STATUS_OPEN_COMPLETE;
      //Register for Download Callbacks before doing OPENURN
      if (g_cbFetchBufferedDataSize && g_cbFetchBufferedData && g_cbIsMimeSupported)
      {
         QtvPlayer::RegisterCallbacksForDownload( 
                                     (QtvPlayer::FetchBufferedDataSizeT) g_cbFetchBufferedDataSize,
                                     (QtvPlayer::FetchBufferedDataT) g_cbFetchBufferedData, 
                                     (QtvPlayer::IsMimeTypeSupportedT) g_cbIsMimeSupported,
                                     pOEM->m_pHandle);
      }
      break;

#ifdef FEATURE_QTV_PSEUDO_STREAM
  case QtvPlayer::QTV_PS_COMMAND_OPEN_STREAM_COMPLETE:
      cmd = MM_CMD_PLAY;
      MMStatus = MM_MP4_STATUS_OPEN_COMPLETE;
      MSG_HIGH("Open Pseudo Stream Command Completed.", 0, 0, 0);
      break;
#endif /*FEATURE_QTV_PSEUDO_STREAM */

    case QtvPlayer::QTV_COMMAND_OPEN_URN_FAILED:
      MSG_ERROR("Open URN Command Failed.", 0, 0, 0);
      MMStatus = MM_STATUS_ABORT;
      cmd = MM_CMD_PLAY;
      data = status;
      break;

#ifdef FEATURE_QTV_PSEUDO_STREAM
  case QtvPlayer::QTV_PS_COMMAND_OPEN_STREAM_FAILED:
      MSG_ERROR("Open Pseudo Stream command Failed.", 0, 0, 0);
    break;
#endif /* FEATURE_QTV_PSEUDO_STREAM */

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  case QtvPlayer::QTV_HTTP_COMMAND_OPEN_STREAM_COMPLETE:
      cmd = MM_CMD_PLAY;
      MMStatus = MM_MP4_STATUS_OPEN_COMPLETE;
      MSG_HIGH("Open HTTP Stream Command Completed.", 0, 0, 0);
      break;

  case QtvPlayer::QTV_HTTP_COMMAND_OPEN_STREAM_FAILED:
      MSG_ERROR("Open HTTP Stream command Failed.", 0, 0, 0);
    break;
#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */

    case QtvPlayer::QTV_PLAYER_STATUS_VIDEO_RESTARTING_TRACK:
      MSG_HIGH("Restarting Video Track.", 0, 0, 0);
      cmd = MM_CMD_PLAY;
      MMStatus = MM_MP4_STATUS_RESTARTING_VIDEO;
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_AUDIO_RESTARTING_TRACK:
      MSG_HIGH("Restarting Audio Track.", 0, 0, 0);
      cmd = MM_CMD_PLAY;
      MMStatus = MM_MP4_STATUS_RESTARTING_AUDIO;
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_TEXT_RESTARTING_TRACK:
      MSG_HIGH("Restarting Text Track.", 0, 0, 0);
      cmd = MM_CMD_PLAY;
      MMStatus = MM_MP4_STATUS_RESTARTING_TEXT;
      break;

    case QtvPlayer::QTV_PLAYER_LOOP_TRACK_NOT_SUPPORTED:
      MSG_ERROR("Loop Track mode is not supported", 0, 0, 0);
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_REDIRECT:
      /* REDIRECT notification. Nothing to do currently. */
      cmd = 0;
      MMStatus = MM_STATUS_UNKNOWN;
      break;

#ifdef FEATURE_QTV_SKT_MOD_MIDI
    case QtvPlayer::QTV_MIDI_DATA_PRESENT:
      MSG_HIGH("MIDI data present.", 0, 0, 0);
      cmd = MM_CMD_PLAY;
      MMStatus = MM_MP4_STATUS_MIDI_AVAILABLE;
      break;
#endif /* FEATURE_QTV_SKT_MOD_MIDI */

    case QtvPlayer::QTV_PLAYER_STATUS_SEEK_FAILED:
      cmd = MM_CMD_PLAY;
      MMStatus = MM_STATUS_SEEK_FAIL;
      break;

#ifdef FEATURE_QTV_STREAM_RECORD
    case QtvPlayer::QTV_REC_FILE_SYS_LIMIT_APPROACHING:
      cmd = MM_CMD_PLAY;
      MMStatus = MM_MP4_STATUS_FILE_SYS_LIMIT;
      break;

    case QtvPlayer::QTV_REC_FILE_SYS_FULL:
      cmd = MM_CMD_PLAY;
      MMStatus = MM_MP4_STATUS_FILE_SYS_FULL;
      break;


    case QtvPlayer::QTV_RECORDER_FILE_ALREADY_EXISTS:
      cmd = MM_CMD_PLAY;
      MMStatus = MM_MP4_STATUS_FILE_ALREADY_EXISTS;
      break;


    case QtvPlayer::QTV_RECORDER_FILE_OW_FAILURE:
      cmd = MM_CMD_PLAY;
      MMStatus = MM_MP4_STATUS_FILE_OW_FAILURE;
      break;


    case QtvPlayer::QTV_RECORDER_FAILED:
      cmd = MM_CMD_PLAY;
      MMStatus = MM_MP4_STATUS_RECORDER_FAILED;
      break;

    case QtvPlayer::QTV_PLAYER_RECORDING_COMPLETED:
      rec_clip_duration = 0;
      cmd = MM_CMD_PLAY;
      MMStatus = MM_MP4_STATUS_RECORD_COMPLETED;
      QtvPlayer::getRecordedClipDuration(rec_clip_duration);
      OEMMediaMPEG4_QueueCallback(pOEM->m_pCallbackData,
                                    MM_MP4_STATUS_RECORDED_CLIP_DURATION,
                                    MM_CMD_PLAY,
                                    0,
                                    (void *)&rec_clip_duration,
                                    sizeof(uint32));
      break;

#endif /* FEATURE_QTV_STREAM_RECORD */

    case QtvPlayer::QTV_PLAYER_STATUS_VIDEO_MEDIA_END:
      cmd = MM_CMD_PLAY;
      MMStatus = MM_MP4_STATUS_VIDEO_MEDIA_END;
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_AUDIO_MEDIA_END:
      cmd = MM_CMD_PLAY;
      MMStatus = MM_MP4_STATUS_AUDIO_MEDIA_END;
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_TEXT_MEDIA_END:
      cmd = MM_CMD_PLAY;
      MMStatus = MM_MP4_STATUS_TEXT_MEDIA_END;
      break;

#ifdef FEATURE_QTV_PSEUDO_STREAM
    case QtvPlayer::QTV_PLAYER_STATUS_PSEUDO_PAUSE:
      pOEM->m_nMPEG4EngineState = status;
      cmd = MM_CMD_PLAY;
      MMStatus = MM_MP4_STATUS_PSEUDO_PAUSE;
      pOEM->m_bPaused = TRUE;
      pOEM->m_nStartTime = state.playbackMsec;
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_PSEUDO_RESUME:
      cmd = MM_CMD_PLAY;
      MMStatus = MM_MP4_STATUS_PSEUDO_RESUME;
      pOEM->m_bResumePending = FALSE;
      break;
#endif /*FEATURE_QTV_PSEUDO_STREAM*/

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
    case QtvPlayer::QTV_PLAYER_STATUS_HTTP_PAUSE:
      if (pClipInfo->RepositioningAllowed == TRUE)
      {
        if (QtvPlayer::GetPlayerState(state, handle) == QtvPlayer::QTV_RETURN_OK)
        {
          pOEM->m_nStartTime = state.playbackMsec;
        }
        else
        {
          ERR("Error in GetPlayerState()", 0, 0, 0);
        }
      }
      pOEM->m_nMPEG4EngineState = status;
      cmd = MM_CMD_PLAY;
      MMStatus = MM_MP4_STATUS_HTTP_PAUSE;
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_HTTP_RESUME:
      cmd = MM_CMD_PLAY;
      MMStatus = MM_MP4_STATUS_HTTP_RESUME;
      break;
#endif /*FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
    case QtvPlayer::QTV_PLAYER_STATUS_VIDEO_RELEASE_DONE:
      MSG_HIGH("Video release complete.", 0, 0, 0);
#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif /* FEATURE_QTV_QOS_SELECTION */
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_VIDEO_RELEASE_FAILED:
      MSG_HIGH("Video release failed.", 0, 0, 0);
#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif /* FEATURE_QTV_QOS_SELECTION */
      break;
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */

    case QtvPlayer::QTV_PLAYER_STATUS_VIDEO_RESOLUTION_NOT_SUPPORTED:
      cmd = MM_CMD_PLAY;
      MMStatus = MM_MP4_STATUS_VIDEO_RESOLUTION_NOT_SUPPORTED;
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_DOWNLOAD_ABORT_MEM_FAIL :
      cmd = MM_CMD_PLAY;
      MMStatus = MM_STATUS_ABORT;
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_DOWNLOAD_COMPLETE :
      cmd = MM_CMD_PLAY;
      MMStatus = MM_MP4_STATUS_DL_COMPLETE;
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_PLAY_NOT_SUPPORTED:
      /* Play was sent to the player when player was internally suspended.
      */
      MSG_ERROR("Play sent when player was internally suspended.", 0, 0, 0);
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_DOWNLOAD_STOPPED :
      cmd = MM_CMD_PLAY;
      MMStatus = MM_MP4_STATUS_DL_STOPPED;
      break;

#ifdef FEATURE_QTV_PLAYLIST      
    case QtvPlayer::QTV_PLAYER_STATUS_CLIP_TRANSITION:
    case QtvPlayer::QTV_PLAYER_STATUS_PLAYLIST_PLAY_COMPLETE:
    {
      cmd = MM_CMD_PLAY;
  
      if (status == QtvPlayer::QTV_PLAYER_STATUS_PLAYLIST_PLAY_COMPLETE)
      {
        MMStatus = MM_MP4_STATUS_PLAYLIST_SKIP_COMPLETE;
  
      }
      else if (status == QtvPlayer::QTV_PLAYER_STATUS_CLIP_TRANSITION)
      {
        MMStatus = MM_MP4_STATUS_PLAYLIST_CLIP_TRANSITION;      

        if (QtvPlayer::GetClipInfo(*pClipInfo, handle) == QtvPlayer::QTV_RETURN_OK)
        {
          CopyMPEG4SpecFromClipInfo(pOEM);
          sendMediaSpec = true;
        }
        else
        {
          MSG_ERROR("Can't get clip info after CLIP_TRANSITION!", 0, 0, 0); 
        }
      }
      else
      {
        MSG_ERROR("Unexpected status %d", status, 0, 0); 
        break;
      }

      data = (uint32) pData;

      QtvPlayer::PlaylistClipTransitionInfo *pTransitionInfo = 
        (QtvPlayer::PlaylistClipTransitionInfo *) pData;
  
      size = sizeof(QtvPlayer::PlaylistClipTransitionInfo);
  
      pOEM->m_nCurrentClipIndex = pTransitionInfo->clipIndex;
  
      MSG_HIGH("Clip transition complete, clipIndex =%d, error=%d, transition time=%d", 
               pOEM->m_nCurrentClipIndex, pTransitionInfo->clipTransitionErrorReasons,
               pTransitionInfo->transitionTime);

      MSG_SPRINTF_1(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Playlist name = %s",
                    pTransitionInfo->playlistName);
    }
    break;

    case QtvPlayer::QTV_PLAYER_STATUS_PLAYLIST_PLAY_FAILED:
    {
      cmd = MM_CMD_PLAY;
      MMStatus = MM_MP4_STATUS_PLAYLIST_SKIP_FAILED;
      MSG_HIGH("Playlist skip failed", 0, 0, 0);
    }
    break;
#else
    // This code is only appease the compiler so it doesn't emit a warning.
    case QtvPlayer::QTV_PLAYER_STATUS_CLIP_TRANSITION:
      MSG_HIGH("Unexpected clip transition event, clipIndex = %d", pOEM->m_nCurrentClipIndex, 0, 0);
      break;
#endif /* FEATURE_QTV_PLAYLIST */
#ifdef FEATURE_QTV_GENERIC_BCAST
    case QtvPlayer::QTV_PLAYER_COMMAND_REPOSITION_DONE:
    case QtvPlayer::QTV_PLAYER_COMMAND_SEEK_SYNC_DONE:
      if (QtvPlayer::GetPlayerState(state, handle) == QtvPlayer::QTV_RETURN_OK)
      {
        pOEM->m_nStartTime = state.playbackMsec;
      }
      else
      {
        ERR("Error in GetPlayerState()", 0, 0, 0);
      }
      pOEM->m_bRepositionInPauseUpdateTime = TRUE;
      pOEM->m_bSeekPending = FALSE;
      cmd = MM_CMD_SETMEDIAPARM;
      MMStatus = MM_STATUS_DONE;
      data = (uint32)pData;
      size = sizeof(uint32);
      break;
    case QtvPlayer::QTV_PLAYER_COMMAND_REPOSITION_FAILED:
    case QtvPlayer::QTV_PLAYER_COMMAND_SEEK_SYNC_FAILED:
      pOEM->m_bSeekPending = FALSE;
      cmd = MM_CMD_SETMEDIAPARM;
      MMStatus = MM_STATUS_SEEK_FAIL;
      data = (uint32)pData;
      size = sizeof(uint32);
      break;

    case QtvPlayer::QTV_PLAYER_COMMAND_PLAY_SYNC_FRAME_DONE:
      pOEM->m_bPlayStarted = TRUE;
      pOEM->m_bSeekPending = FALSE;
      MMStatus = MM_MP4_STATUS_PLAYING;
      cmd = MM_CMD_PLAY;
      break;
    case QtvPlayer::QTV_PLAYER_COMMAND_PLAY_SYNC_FRAME_FAILED:
      cmd = MM_CMD_PLAY;
      MMStatus = MM_STATUS_DONE;
      pOEM->m_bSeekPending = FALSE;
      pOEM->m_nPlayFrameMode = 0;
      break;

    case QtvPlayer::QTV_PLAYER_COMMAND_MUTE_COMPLETE: 
    case QtvPlayer::QTV_PLAYER_COMMAND_UNMUTE_COMPLETE:
      pOEM->m_nBcastMutePending = 0;
      cmd = MM_CMD_SETMEDIAPARM;
      MMStatus = MM_STATUS_DONE;
      data = MM_MP4_PARM_MUTE_TRACK;
      break;

    case QtvPlayer::QTV_PLAYER_COMMAND_MUTE_FAILED:
    case QtvPlayer::QTV_PLAYER_COMMAND_UNMUTE_FAILED:
    case QtvPlayer::QTV_PLAYER_COMMAND_MUTE_ALL_FAILED:
      pOEM->m_nBcastMutePending = 0;
      cmd = MM_CMD_SETMEDIAPARM;
      MMStatus = MM_STATUS_ABORT;
      data = MM_MP4_PARM_MUTE_TRACK;
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_TRACK_STATE_CHANGE:
    {
      QtvPlayer::TrackListT *trackList = (QtvPlayer::TrackListT *)pData;
      MSG_ERROR("TRACK_STATE_CHANGE received", 0, 0, 0);
      MSG_ERROR("Track ID = %u, Track type = %d, Media type %d", 
                trackList->nTrackID, trackList->trackType, trackList->mediaType);
      MSG_ERROR("Track state = %d, Sampling freq = %d, Channel config %d", 
                trackList->bmTrackState, 
                trackList->codecInfo.aacInfo.nSamplingFreq, 
                trackList->codecInfo.aacInfo.eChannelConfig);
      break;
    }
#endif /* FEATURE_QTV_GENERIC_BCAST */   

    case QtvPlayer::QTV_PLAYER_STATUS_END_OF_TSB:
    {
      MSG_ERROR("END_OF_TSB received", 0, 0, 0);
      break;
    }

    case QtvPlayer::QTV_PLAYER_STATUS_TSB_OVERFLOW:
    {
      MSG_ERROR("TSB_OVERFLOW received", 0, 0, 0);
      break;
    }

    case QtvPlayer::QTV_PLAYER_STATUS_TSB_DATA_LOSS:
    {
      MSG_ERROR("TSB_DATA_LOSS received", 0, 0, 0);
      break;
    }
    #ifdef FEATURE_QTV_DIVX_DRM
      case QtvPlayer::QTV_PLAYER_DRM_AUTHORIZATION_ERROR:
      {
        MMStatus = MM_MP4_STATUS_DRM_AUTHORIZATION_ERROR;
        MSG_HIGH("MM_MP4_STATUS_DRM_AUTHORIZATION_ERROR", 0, 0, 0);
        cmd = MM_CMD_PLAY;
        break;
      }
      case QtvPlayer::QTV_PLAYER_DRM_DEVICE_REGISTRATION_ERROR:   
      {
        MMStatus = MM_MP4_STATUS_DRM_DEVICE_REGISTRATION_ERROR;
        MSG_HIGH("MM_MP4_STATUS_DRM_DEVICE_REGISTRATION_ERROR", 0, 0, 0);
        cmd = MM_CMD_PLAY;
        break;
      }
      case QtvPlayer::QTV_PLAYER_DRM_RENTAL_COUNT_EXPIRED :
      {
        MMStatus = MM_MP4_STATUS_DRM_RENTAL_COUNT_EXPIRED;
        MSG_HIGH("MM_MP4_STATUS_DRM_RENTAL_COUNT_EXPIRED", 0, 0, 0);
        cmd = MM_CMD_PLAY;
        break;
      }
      case QtvPlayer::QTV_PLAYER_DRM_PLAYBACK_GENERAL_ERROR:
      {
        MMStatus = MM_MP4_STATUS_DRM_PLAYBACK_GENERAL_ERROR;
        MSG_HIGH("MM_MP4_STATUS_DRM_PLAYBACK_GENERAL_ERROR", 0, 0, 0);
        cmd = MM_CMD_PLAY;
        break;
      }
      case QtvPlayer::QTV_PLAYER_CONSUME_RENTAL_VIEW_CONFIRMATION:
      {
        MMStatus = MM_MP4_STATUS_CONSUME_RENTAL_VIEW_CONFIRMATION;
        MSG_HIGH("MM_MP4_STATUS_CONSUME_RENTAL_VIEW_CONFIRMATION", 0, 0, 0);
        cmd = MM_CMD_PLAY;
        break;
      }
    #endif//#ifdef FEATURE_QTV_DIVX_DRM

#ifdef FEATURE_QTV_FCS
#error code not present
#endif
	case QtvPlayer::QTV_PLAYER_STATUS_VALID_CLIPINFO_DIMENSIONS:
		{
			cmd = MM_CMD_PLAY;
			MMStatus = MM_MP4_STATUS_CLIPINFO_DIMENSION_READY;      
			break;
		}

    case QtvPlayer::QTV_STREAM_TRACKLIST_UNKNOWN_CODEC:
    {
      cmd = MM_CMD_PLAY;
      MMStatus = MM_MP4_STATUS_TRACKLIST_CODEC_CHECK_DONE;
      break;
    }

    case QtvPlayer::QTV_PLAYER_STATUS_RECONNECT_SUCCESS:
      { 
        cmd = MM_CMD_PLAY;
        MMStatus = MM_MP4_STATUS_RECONNECT_SUCCESS;
        MSG_HIGH("MM_MP4_STATUS_RECONNECT_SUCCESS.", 0, 0, 0);
        break;
      }
    case QtvPlayer::QTV_PLAYER_STATUS_RECONNECT_FAIL:
      {
        cmd = MM_CMD_PLAY;
        MMStatus = MM_MP4_STATUS_RECONNECT_FAILED;
        MSG_HIGH("MM_MP4_STATUS_RECONNECT_FAILED.", 0, 0, 0);
        break;
      }
    case QtvPlayer::QTV_PLAYER_STATUS_RECONNECT_IN_PROGRESS:
      {
        cmd = MM_CMD_PLAY;
        MMStatus = MM_MP4_STATUS_RECONNECT_IN_PROGRESS;
        MSG_HIGH("MM_MP4_STATUS_RECONNECT_IN_PROGRESS.", 0, 0, 0);
        break;
      }
    case QtvPlayer::QTV_PLAYER_STATUS_FCS_SWITCH_SUPPORTED:
    {
        cmd = MM_CMD_PLAY;
        MMStatus = MM_MP4_STATUS_FCS_SWITCH_SUPPORTED;
        MSG_HIGH("QTV_PLAYER_STATUS_FCS_SWITCH_SUPPORTED.", 0, 0, 0);
		break;
    }
    case QtvPlayer::QTV_PLAYER_STATUS_RECONNECTING_USING_TCP_INTERLEAVE:
    {
        cmd = MM_CMD_PLAY;
        MMStatus = MM_MP4_STATUS_RECONNECTING_USING_TCP_INTERLEAVE;
        MSG_HIGH("MM_MP4_STATUS_RECONNECTING_USING_TCP_INTERLEAVE.", 0, 0, 0);
        break;
    }      
    default:
      break;
  }

  if ((MMStatus != MM_STATUS_UNKNOWN) && (cmd != 0)) //no point in queuing if no cmd
  {
    OEMMediaMPEG4_QueueCallback(pOEM->m_pCallbackData, MMStatus, cmd, subCmd, 
                                (void *) data, size);
  }

#ifdef FEATURE_QTV_PLAYLIST
  if (sendMediaSpec)
  {
    pOEM->m_cbGenerateMediaSpec = FALSE;
    OEMMediaMPEG4_QueueCallback(pOEM->m_pCallbackData,
                                MM_STATUS_DONE,
                                MM_CMD_GETMEDIAPARM,
                                MM_MP4_PARM_MEDIA_SPEC,
                                (void *)&pOEM->m_MPEG4Spec,
                                sizeof(pOEM->m_MPEG4Spec));
  }
#endif /* FEATURE_QTV_PLAYLIST */
}

extern "C" void QtvEngineCBShell(
  QtvPlayer::PlayerStatusT status,
  void *pData,
  QtvPlayer::PlayerStateRecordT state)
{
  OEMHandle pOEM = OEMMediaMPEG42PV_GetOEMLayer(NULL);
  if (pOEM == NULL)
  {
    MSG_ERROR("QtvEngineCBShell, pOEM is null.", 0, 0, 0);
    return ;
  }
  QtvEngineCB(status, pData, state, pOEM->m_pHandle, pOEM);
}

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
void UpdateDownloadProgressBar(void *pData)
{
   QtvPlayer::PlayerStateRecordT state = {0};
   QtvPlayer::ReturnT stateVal = QtvPlayer::QTV_RETURN_ERROR;
   OEMHandle pOEM = OEMMediaMPEG42PV_GetOEMLayer((OEMHandle)pData);
   if (pOEM == NULL)
   {
      return;
   }
   stateVal = QtvPlayer::GetPlayerState(state, pOEM->m_pHandle);

   if ( (stateVal == QtvPlayer::QTV_RETURN_OK) &&
        ( (pOEM->m_nMPEG4EngineState == QtvPlayer::QTV_PLAYER_STATUS_BUFFERING) ||
          (pOEM->m_nMPEG4EngineState == QtvPlayer::QTV_PLAYER_STATUS_PLAYING) ||
          (pOEM->m_nMPEG4EngineState == QtvPlayer::QTV_PLAYER_STATUS_PLAYING_1P3X) )  )
   {   
       OEMMediaMPEG4_QueueCallback(pOEM->m_pCallbackData,
                                    MM_STATUS_DOWNLOAD_TICK_UPDATE,
                                    MM_CMD_PLAY,
                                    0,
                                    (void *)state.downloadTime,
                                    sizeof(state.downloadTime));                 
   }
}
#endif


void UpdateProgressBar(void *pData)
{
   QtvPlayer::PlayerStateRecordT state = {0};
   QtvPlayer::ReturnT stateVal = QtvPlayer::QTV_RETURN_ERROR;
   OEMHandle pOEM = OEMMediaMPEG42PV_GetOEMLayer((OEMHandle)pData);
   if (pOEM == NULL)
   {
      return;   // Make sure the timer didn't fire after Qtv terminated.
   }
   stateVal = QtvPlayer::GetPlayerState(state, pOEM->m_pHandle);

   if ( (stateVal == QtvPlayer::QTV_RETURN_OK) &&
        ( (pOEM->m_nMPEG4EngineState == QtvPlayer::QTV_PLAYER_STATUS_PLAYING) ||
          (pOEM->m_nMPEG4EngineState == QtvPlayer::QTV_PLAYER_STATUS_PLAYING_1P3X) ) )
   {
     // Make sure that we give TICK_UPDATE to UI only when app sets m_nTickInterval
     if(0 != pOEM->m_nTickInterval)
     {
      if(pOEM->m_bRepositionInPauseUpdateTime != TRUE) // Check if repositioned during Pause
      {
        /* we should send some positive value to MM_STATUS_TICK_UPDATE,
         * so application will assume it has correct value
         */

          //Update progress bar based on QTV playback time..
          if(!state.playbackMsec)     
            state.playbackMsec = 1;   
          OEMMediaMPEG4_QueueCallback(pOEM->m_pCallbackData,
                                      MM_STATUS_TICK_UPDATE,
                                      MM_CMD_PLAY,
                                      0,
                                      (void *)state.playbackMsec,
                                      sizeof(state.playbackMsec));  
      }
      else
      {
        /* We have repositioned and need to wait for the next second -
        * to give time for Qtv to process repositioning update and to get
        * the right time from Qtv. For this second, use the start time info.
        * available at this layer
        */
        //Update with the new playback time
        MSG_HIGH("UPDATING PROGRESS BAR AFTER REPOSITIONING %ld Msec",pOEM->m_nStartTime,0,0);
        OEMMediaMPEG4_QueueCallback(pOEM->m_pCallbackData,
                                  MM_STATUS_TICK_UPDATE,
                                  MM_CMD_PLAY,
                                  0,
                                  (void *)pOEM->m_nStartTime,sizeof(pOEM->m_nStartTime));
    
        pOEM->m_bRepositionInPauseUpdateTime = FALSE;
      }
     }
      
   }
   else
   {
      if ( (stateVal == QtvPlayer::QTV_RETURN_OK) &&
           (pOEM->m_nMPEG4EngineState == QtvPlayer::QTV_PLAYER_STATUS_BUFFERING))

      {
         OEMMediaMPEG4_QueueCallback(pOEM->m_pCallbackData,
                                     MM_MP4_STATUS_BUFFER_UPDATE,
                                     MM_CMD_PLAY,
                                     0,
                                     (void *)&pOEM->m_nPrerollPercentage,
                                     sizeof(pOEM->m_nPrerollPercentage));
      }
      else
      {
        if((pOEM->m_nMPEG4EngineState == QtvPlayer::QTV_PLAYER_STATUS_CONNECTING) ||
           (pOEM->m_nMPEG4EngineState == QtvPlayer::QTV_PLAYER_STATUS_OPENING))
        {
          OEMMediaMPEG4_QueueCallback( pOEM->m_pCallbackData,
                                       MM_MP4_STATUS_CONNECTION_UPDATE,
                                       MM_CMD_PLAY,
                                       0,
                                       (void *)NULL,
                                       0 );
        }

#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
        else if ((pOEM->m_nMPEG4EngineState == QtvPlayer::QTV_PS_PLAYER_BUFFERING) ||
                 (pOEM->m_nMPEG4EngineState == QtvPlayer::QTV_HTTP_PLAYER_BUFFERING))
        {
          OEMMediaMPEG4_QueueCallback(pOEM->m_pCallbackData,
                                      MM_MP4_STATUS_PS_BUFFER_UPDATE,
                                      MM_CMD_PLAY,
                                      0,
                                      (void *)&pOEM->m_nPsdownloadrate,
                                      sizeof(pOEM->m_nPsdownloadrate));
        }
#endif

      }
   }
}

static void RTSPStatusCB(int32 statusCode, const char * statusString, 
                         QtvPlayer::InstanceHandleT, void *pUserData)
{
  OEMHandle pOEM = OEMMediaMPEG42PV_GetOEMLayer((OEMHandle)pUserData);

  if (pOEM == NULL)
  {
    MSG_ERROR("RTSPStatusCB, pOEM is null.", 0, 0, 0);
    return ;
  }

  pOEM->m_RTSPStatus.statusCode = statusCode;
  (void)std_strlcpy(pOEM->m_RTSPStatus.statusString, statusString, MM_MPEG4_MAX_RTSP_STATUS_STRING_BYTES);

  OEMMediaMPEG4_QueueCallback(pOEM->m_pCallbackData,
                              MM_MP4_STATUS_RTSP,
                              MM_CMD_PLAY,
                              0,
                              (void *)&pOEM->m_RTSPStatus,
                              sizeof(AEERTSPStatus));

}


static void UnhandledSDPItemCB(const char *unhandledString)
{
  OEMHandle pOEM = OEMMediaMPEG42PV_GetOEMLayer(NULL);
  if (pOEM == NULL)
  {
    MSG_ERROR("UnhandledSDPItemCB, pOEM is null.", 0, 0, 0);
    return ;
  }
  (void)std_strlcpy(pOEM->m_SDPStatus.statusString,
     unhandledString, sizeof(pOEM->m_SDPStatus.statusString) );

  OEMMediaMPEG4_QueueCallback(pOEM->m_pCallbackData,
                              MM_MP4_STATUS_UNHANDLED_SDP,
                              MM_CMD_PLAY, // don't care here
                              0,
                              (void *)&pOEM->m_SDPStatus,
                              sizeof(AEEUnhandledSDP_RTSP_Status));
}

extern "C" int OEMMediaMPEG42PV_SetUnhandledSDPNotify(boolean notifyEnable)
{
   if (notifyEnable)
   {
     QtvPlayer::SetUnhandledSDPCallback(UnhandledSDPItemCB);
   }
   else
   {
     QtvPlayer::SetUnhandledSDPCallback(NULL);
   }
   return SUCCESS;
}

static void UnhandledRTSPItemCB(const char *unhandledString)
{
  OEMHandle pOEM = OEMMediaMPEG42PV_GetOEMLayer(NULL);

  if(pOEM == NULL)
  {
    MSG_ERROR("UnhandledRTSPItemCB, pOEM is null.", 0, 0, 0);
    return;
  }

  (void)std_strlcpy(pOEM->m_SDPStatus.statusString,
     unhandledString, sizeof(pOEM->m_SDPStatus.statusString) );

  OEMMediaMPEG4_QueueCallback(pOEM->m_pCallbackData,
                              MM_MP4_STATUS_UNHANDLED_RTSP,
                              MM_CMD_PLAY, // don't care here
                              0,
                              (void *)&pOEM->m_SDPStatus,
                              sizeof(AEEUnhandledSDP_RTSP_Status));
}

extern "C" int OEMMediaMPEG42PV_SetUnhandledRTSPNotify(boolean notifyEnable)
{
   if (notifyEnable)
   {
     QtvPlayer::SetUnhandledRTSPHeaderCallback(UnhandledRTSPItemCB);
   }
   else
   {
     QtvPlayer::SetUnhandledRTSPHeaderCallback(NULL);
   }
   return SUCCESS;
}



/*!
  @brief
  Modifies RTSP headers
  
  @detail
  Allows OEM to modify the header lines to send to the server for selected RTSP
  requests and responses.

  @return
  Indication of success

  @retval SUCCESS Editing the RTSP Headers succeeded
  @retval EFAILED Editing the RTSP Headers failed

  @see QtvPlayer::OemRtspHeaderCommand
  
  @param operation
  The command to perform.  Options are as in QtvPlayer::OemRtspHeaderCommand
       0 = delete all
       1 = delete
       2 = add
       3 = replace

  @param affectedCommands
  The RTSP methods in which the header is valid. Values from
  QtvPlayer::AffectedRTSPMethod.  Don't care in delete, deleteall.
       1 = describe,
       2 = setup,
       4 = play,
       8 = pause,
       16 = teardown,
       32 = options response,
       Values can be or'd together to affect multiple commands

  @param headerName name of header item
  @param headerValue value of header item

  @note Result of Name+Value is line of form: "name: value"<cr><lf>" output
*/
extern "C" int OEMEditOemRtspHeaders(
   int operation,           // QtvPlayer::OemRtspHeaderCommand
   // remainder of input fields are don't care for delete, delete all
   int affectedCommands,    // QtvPlayer::AffectedRTSPMethods
   const char *headerName,
   const char *headerValue
)
{
   // sanity checks
   if ( (operation < QtvPlayer::OEM_RTSP_HEADER_FIRST_CMD) ||
        (operation > QtvPlayer::OEM_RTSP_HEADER_LAST_CMD) ) {
      return EFAILED;
   }
   QtvPlayer::OemRtspHeaderResult result =
      QtvPlayer::EditOemRtspHeaders(
         (QtvPlayer::OemRtspHeaderCommand)operation,
         (QtvPlayer::AffectedRTSPMethod)affectedCommands,
         headerName, headerValue);
   if (result == QtvPlayer::OEM_RTSP_RESULT_OK) {
      return SUCCESS;
   }
   return EFAILED;
}


extern "C" int OEMMediaMPEG42PV_GetClipInfo(OEMHandle pOEM)
{
   MSG_HIGH("OEMMediaMPEG42PV_GetClipInfo, Entry", 0, 0, 0);
   pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);
   if(pOEM == NULL)
   {
     MSG_ERROR("OEMMediaMPEG42PV_GetClipInfo, pOEM is null.", 0, 0, 0);
     return EFAILED;
   }
   if (pOEM->m_bClipInfoAvail == TRUE)
   {
      MSG_HIGH("OEMMediaMPEG42PV_GetClipInfo...pOEM->m_bClipInfoAvail is TRUE",0,0,0);
	  
      OEMMediaMPEG4_QueueCallback(pOEM->m_pCallbackData,
                                  MM_STATUS_DONE,
                                  MM_CMD_GETTOTALTIME,
                                  0,
                                  (void *)pOEM->m_MPEG4Spec.dwDuration,
                                  sizeof(pOEM->m_MPEG4Spec.dwDuration));
   }
   // This is to handle the case if STOP/PAUSE while a streaming clip is in connecting state.
   // MediaPlayer will wait for GetTotalTime callback before issuing IMEDIA_Play.
   else if (pOEM->m_bSendTotalTime == TRUE) 
   {
     MSG_HIGH("OEMMediaMPEG42PV_GetClipInfo...pOEM->m_bSendTotalTime is TRUE",0,0,0);
     uint32 temp_duration = 0;
     pOEM->m_bSendTotalTime = FALSE;
     OEMMediaMPEG4_QueueCallback(pOEM->m_pCallbackData,
                                  MM_STATUS_DONE,
                                  MM_CMD_GETTOTALTIME,
                                  0,
                                  (void *)temp_duration,
                                  sizeof(temp_duration));
   }
   else
   {
      pOEM->m_cbGenerateTotalTime = TRUE;
      MSG_HIGH("OEMMediaMPEG42PV_GetClipInfo,(pending notification)m_cbGenerateTotalTime =%d",pOEM->m_cbGenerateTotalTime, 0, 0);
   }
   if (pOEM->m_bStreamingError)
   {
      MSG_ERROR("OEMMediaMPEG42PV_GetClipInfo, pOEM->m_bStreamingError is TRUE. Return EFAILED", 0, 0, 0);
      return EFAILED;
   }
   MSG_HIGH("OEMMediaMPEG42PV_GetClipInfo...return SUCCESS",0,0,0);
   return SUCCESS;
}


/*===========================================================================*/
/*!
  @brief
  Releases the QtvBitmap informing QtvPlayer

  @detail
  This procedure is a custom _Release routine used by the IBitmap's that are
  passed to the application if Frame CB's are enabled.  The problem is that
  these frame buffers must be returned to the Qtv services layer to be reused
  properly.  Rather than expose a separate API, we're able to write a custom
  _Release routine to do this automatically.  This allows older apps to function
  the same way on MDP-enabled targets.

  Reference Count Decoder Ring:
  - A ref count of 1 indicates that the IBitmap has been allocated, but no one
    else has a reference to it.  If the _Release routine is called here, then we
    need to free all memory associated with the object since it is going away.
  - A ref count of 2 indicates that the IBitmap has been given to the Application
    via a call to IMEDIA_GetFrame (which calls OEMMediaMPEG2PV_GetFrame).  If
    the _Release routine is called here, then we need to give the buffer back to
    QtvPlayer. 
  - A ref count of 3 or more simply means that someone has called QueryInterface
    to get other interfaces to the IBitmap (e.g. IDIB or IYCbCr).  These are
    temporary references.

  @return 
  EBADPARM - po was NULL or points to an invalid IBitmap
  SUCCESS  - Everything was successful and po was released

  @note If necessary, calls ReleaseCurrentVideoFrameBuffer() to give the buffer back
  to the QtvPlayer
*/
static uint32 QtvBitmap_Release(
  IBitmap* po   /*!< pointer to an IBitmap which is to be released/free'd. */
  )
{
  QTVBITMAP_SUBTYPE *me;              
  uint32 refs;                        
  QTVBITMAP_SUBTYPE *pBitMap = NULL;  
  MUTEX_LOCK();
  if( po == NULL ||
      SUCCESS != IBITMAP_QueryInterface( po, QTVBITMAP_IID, (void**)&me ) )
  {
    MUTEX_UNLOCK();
    return EBADPARM;
  }

  /* We just QueryInterface'd above, so if we release again, we'll be back to
   * the appropriate ref count.  This will not actually free the object.
   * We must use the statically saved version of Release, otherwise it'll just
   * call us again */
  refs = g_pvtIBitmap->Release(po);
  MSG_HIGH("Current no of refs: %d ",refs,0,0);

  /* If we have 2 references left, then that means that we should release the
   * Qtv frame data.
   */
  if( refs == 2 )
  {
    if( QTVBITMAP_IS_BUFFER_SET(me) )
    {
      /* Get the frame buffer address and release */

      if( SUCCESS == IBITMAP_QueryInterface(po, QTVBITMAP_IID, (void**) &pBitMap) &&
          (pBitMap != NULL) )
      {
        MSG_ERROR("releasing frame: %x ",QTVBITMAP_FRAMEBUFFER(pBitMap),0,0);
        OEMMediaMPEG42PV_ReleaseCurrentVideoFrameBuffer(QTVBITMAP_FRAMEBUFFER(pBitMap));
        g_pvtIBitmap->Release(po);
      }
      QTVBITMAP_CLEAR_BUFFER(me);
      g_pvtIBitmap->Release(po);  //to avoid memory leak
      /* We create one bitmap for every frame we receive. So
       * we can safely destroy..
       */
      /* destroy vtbl and restore original */
      FREE( AEEGETPVTBL(me, IBitmap) );
      AEEINITVTBL(me, IBitmap, *g_pvtIBitmap);

    }
  }
  else if( refs == 1 )
  {
    /* This part of code should never be called now */
    /* destroy vtbl and restore original */
    MSG_ERROR("We should not come here if it is Async MDP",0,0,0);
    FREE( AEEGETPVTBL(me, IBitmap) );
    AEEINITVTBL(me, IBitmap, *g_pvtIBitmap);
  }

  /* Let the base class do its work--this time the object may be free'd */
  refs = g_pvtIBitmap->Release(po);
  MUTEX_UNLOCK();
  return refs;
  
}


/*===========================================================================

FUNCTION QtvBitmap_New

DESCRIPTION
  This procedure creates a new IBitmap to be used for returning all of the Qtv
  frame buffers to the application.

INPUT/OUTPUT PARAMETERS:
  ppo - pointer in which to store the newly allocation IBitmap pointer

RETURN VALUE
  EBADPARM - ppo was NULL
  ENOMEMORY - Unable to allocate memory for the new IBitmap
  SUCCESS - Everything was successful and ppo contains a valid IBitmap

SIDE EFFECTS
  Modifies this IBitmap's vtable to contain Qtv's custom release routine:
  QtvBitmap_Release 

===========================================================================*/
static int QtvBitmap_New(IBitmap** ppo)
{
  QTVBITMAP_SUBTYPE  *me;
  IBitmap *pIBitmap;
  AEEVTBL(IBitmap)* vt;
  int rv;
  
  if( !ppo )
  {
    return EBADPARM;
  }

#ifdef FEATURE_QTV_MDP
#ifdef FEATURE_QTV_MDP_COLOR_FORMAT_SWAP
#error code not present
#else
  rv = YCbCr_New( IYCBCR_COLORSCHEME_420LP, 0, 0, &me );
#endif
#else
  {
    IShell   *pShell = AEE_GetShell();
    IDisplay *pDisp;
    IBitmap  *pDevBitmap;
    IBitmap  *pBitmap;
    
    rv = ISHELL_CreateInstance( pShell, AEECLSID_DISPLAY1, (void**)&pDisp );
    if( rv == SUCCESS )
    {
      rv = IDISPLAY_GetDeviceBitmap( pDisp, &pDevBitmap );

      if( rv == SUCCESS )
      {
        rv = IBITMAP_CreateCompatibleBitmap( pDevBitmap, &pBitmap, 0, 0 );

        if( rv == SUCCESS )
        {
          IBITMAP_QueryInterface( pBitmap, AEECLSID_DIB, (void**)&me );
          IBITMAP_Release( pBitmap );
        }
        IBITMAP_Release(pDevBitmap);
      }
      IDISPLAY_Release(pDisp);
    }
  }
#endif /* FEATURE_QTV_MDP */
  
  if( rv != SUCCESS )
  {
    return rv;
  }

  /* Clear the raw buffer info since it isn't valid */
  QTVBITMAP_CLEAR_BUFFER(me);
  
  /* save the original IBitmap vtbl and make a copy */
  g_pvtIBitmap = AEEGETPVTBL(me, IBitmap);
  vt = (AEEVTBL(IBitmap)*) MALLOC(sizeof(AEEVTBL(IBitmap)));
  if( !vt )
  {
    pIBitmap = QTVBITMAP_SUBTYPE_TO_IBITMAP(me);
    IBITMAP_Release( pIBitmap );
    return ENOMEMORY;
  }
  MEMCPY( (void*)vt, (void*)g_pvtIBitmap, sizeof(AEEVTBL(IBitmap)) );

  /* Use custom vtbl for custom release method */
  vt->Release = QtvBitmap_Release;
  AEEINITVTBL(me, IBitmap, *vt);

  *ppo = QTVBITMAP_SUBTYPE_TO_IBITMAP(me);

  return SUCCESS;
}

extern "C" int OEMMediaMPEG42PV_Init(OEMHandle pOEM)
{
  QtvPlayer::ReturnT retVal = QtvPlayer::QTV_RETURN_OK;
  MSG_HIGH("OEMMediaMPEG42PV_Init, Entry ", 0, 0, 0);

#if (defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION)
#error code not present
#endif /* defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION */

  /* Initialize state parameters */ 
  pOEM->m_nTickInterval = MM_DEFAULT_TICK_INTERVAL;
  pOEM->m_pClipInfo = (void*)MALLOC(sizeof(QtvPlayer::ClipInfoT));
  if (!pOEM->m_pClipInfo)
  {
      MSG_ERROR("OEMMediaMPEG42PV_Init, MALLOC can't allocate %d bytes for m_pClipInfo", sizeof(QtvPlayer::ClipInfoT), 0, 0);
      return ENOMEMORY;
  }  
  pOEM->m_cbGenerateTotalTime = FALSE; 
  pOEM->m_cbGenerateMediaSpec = FALSE; 
  MSG_HIGH("OEMMediaMPEG42PV_Init, m_cbGenerateTotalTime = %d  ", pOEM->m_cbGenerateTotalTime, 0, 0);
  pOEM->m_nMPEG4EngineState = QtvPlayer::QTV_PLAYER_STATUS_IDLE;

#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
  pOEM->m_dualMonoOutput = MM_DUAL_MONO_OUTPUT_MAIN_TO_L_AND_R;
#endif /* FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION */

#ifdef FEATURE_USE_CLKREGIM_DEVMAN_REV2
  g_ClkrgmDevParam.minimum_speed = CLKRGM_DEVICE_SPEED_MAX;
#endif /* FEATURE_USE_CLKREGIM_DEVMAN_REV2 */

  // Initialize the Qtv Player
#ifdef FEATURE_QTV_OEM_BUFFER_MGR
  MSG_HIGH("OEMMediaMPEG42PV_Init, QtvPlayer::Init(AllocateOutputBuffer, ReleaseOutputBuffer, &pOEM->m_pHandle) ", 0, 0, 0);
  retVal = QtvPlayer::Init(AllocateOutputBuffer, ReleaseOutputBuffer, &pOEM->m_pHandle);
#else
  MSG_HIGH("OEMMediaMPEG42PV_Init, QtvPlayer::Init(AllocateOutputBuffer, FreeOutputBuffer, &pOEM->m_pHandle) ", 0, 0, 0);
  retVal = QtvPlayer::Init(AllocateOutputBuffer, FreeOutputBuffer, &pOEM->m_pHandle);
#endif /* FEATURE_QTV_OEM_BUFFER_MGR */
  if(retVal == QtvPlayer::QTV_RETURN_OK && pOEM->m_pHandle != NULL)
  {
   // enable the RTSP callback by default
    QtvPlayer::SetRTSPStatusCallback(RTSPStatusCB, pOEM->m_pHandle, pOEM);
    char path[] = MEDIAPLAYER_ROOT_PATH;
    int pathExt = (int)path;
    QtvPlayer::SetQTVConfigItem(QtvConfig::QTVCONFIG_MEDIAPLAYER_ROOT_PATH,
                                &pathExt,pOEM->m_pHandle);
    if(retVal == QtvPlayer::QTV_RETURN_OK)
    {
    
      MSG_HIGH("OEMMediaMPEG42PV_Init->SetQTVConfigItem, Exit ", 0, 0, 0);
      return SUCCESS;
    }
    else
    {
      MSG_ERROR("QtvPlayer::SetMediaplayerRootPath() failed: ret=%d", retVal, 0, 0);
      return EFAILED;
    }

  }
  else
  {
    MSG_ERROR("QtvPlayer::Init failed, return val=%d", retVal, 0, 0);
    return EFAILED;
  }   
}

extern "C" int OEMMediaMPEG42PV_OpenBufferURN(  unsigned char *pVideoBuf, uint32 dwVideoBufSize,
                                                unsigned char *pAudioBuf, uint32 dwAudioBufSize,
                                                OEMHandle pOEM )
{
  QtvPlayer::ReturnT retVal;

  if( (!pVideoBuf || !dwVideoBufSize) && (!pAudioBuf || !dwAudioBufSize) )
  {
    MSG_ERROR("No URN passed in OEMMediaMPEG42PV_OpenBufferURN", 0, 0, 0);
    return EFAILED;
  }
  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);
  if(pOEM == NULL)
  {
    MSG_ERROR("OEMMediaMPEG42PV_OpenBufferURN, pOEM is null.", 0, 0, 0);
    return EFAILED;
  }
  // Moved the resetting of callbacks to Init
  pOEM->m_bPBReadyPending = TRUE;
  pOEM->m_Sound.cmd_pending = FALSE;
  pOEM->m_Sound.cb_pending = FALSE;

#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
  pOEM->m_bFrameTransformPending = FALSE;
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */

#ifdef FEATURE_QTV_IPL_SCALING
#error code not present
#endif /* FEATURE_QTV_IPL_SCALING */

#ifdef FEATURE_MP4_AAC_PLUS
  /* Initialized to no rotation */
  pOEM->m_IPLRotationFactor = MM_MPEG4_NO_ROTATION;
#endif /* FEATURE_MP4_AAC_PLUS */

#if defined(FEATURE_QTV_MDP_TRANSFORMATIONS) || defined(FEATURE_QTV_XSCALE_VIDEO)
  pOEM->m_QtvCurrentRotation = OEM_NO_ROTATION;
  pOEM->m_QtvCurrentScaling  = OEM_NO_SCALING;
#endif /* FEATURE_QTV_MDP_TRANSFORMATIONS || FEATURE_QTV_XSCALE_VIDEO */

  retVal = QtvPlayer::OpenURN(pVideoBuf, dwVideoBufSize, pAudioBuf, dwAudioBufSize,
                              NULL, 0, pOEM->m_pHandle, 
                              (QtvPlayer::InstancePriorityT)pOEM->m_nPriority);
  if (retVal == QtvPlayer::QTV_RETURN_OK)
  {
    return SUCCESS;
  }
  else
  {
    return EFAILED;
  }
}

#ifdef FEATURE_QTV_ENCRYPTED_STREAMS
extern "C" int OEMMediaMPEG42Qtv_SetDescramblerHandler(void *descrInstance, OEMHandle pOEM)
{
  int retVal;
  QtvPlayer::ReturnT qtvRetVal;
  
  qtvRetVal = QtvPlayer::SetDescramblerHandler(descrInstance, pOEM->m_pHandle);
 
  if(qtvRetVal == QtvPlayer::QTV_RETURN_OK)
  {
    retVal = SUCCESS;
  }
  else
  {
    retVal = EFAILED;
    MSG_ERROR("QtvPlayer::SetDescramblerHandler() failed: ret=%d", qtvRetVal, 0, 0);
  }

  return retVal;
}
#endif


#if (defined (FEATURE_QTV_QOS_SELECTION)|| defined (FEATURE_QTV_PDP_SELECTION))
extern "C" int OEMMediaMPEG42Qtv_SetNetPolicyInfo(AEENetPolicyInfo* qosInfo, OEMHandle pOEM)
{
  int retVal;
  QtvPlayer::ReturnT qtvRetVal;
  QtvPlayer::NetPolicyInfo netPolicyInfo;

  if (qosInfo == NULL)
  {
    qtvRetVal = QtvPlayer::SetNetPolicyInfo(NULL, pOEM->m_pHandle);
  }
  else
  {
    netPolicyInfo.isSet = true;
    netPolicyInfo.isQoSEnabled = qosInfo->isQoSEnabled;
    netPolicyInfo.primaryPDPProfileNo = qosInfo->primaryPDPProfileNo;
    qtvRetVal = QtvPlayer::SetNetPolicyInfo(&netPolicyInfo, pOEM->m_pHandle);
  }

  if(qtvRetVal == QtvPlayer::QTV_RETURN_OK)
  {
    retVal = SUCCESS;
  }
  else
  {
    retVal = EFAILED;
    MSG_ERROR("QtvPlayer::SetNetPolicyInfo() failed: ret=%d", qtvRetVal, 0, 0);
  }

  return retVal;
}
#endif


extern "C" int OEMMediaMPEG42Qtv_ISettings_Set(char *userString, const char *value, OEMHandle pOEM)
{
  /*  Needs to call below functions.
  1.	static ReturnT SetUserAgent(const char * userAgentName, InstanceHandleT handle = NULL);
  2.	static ReturnT SetUserAgentParameters(const char* params, InstanceHandleT handle = NULL);
  3.	void QtvPlayer::SetDataPortRange(int beginPort, int endPort)
  4.  static OemRtspHeaderResult EditOemRtspHeaders(
       OemRtspHeaderCommand whatCommand,
       uint32 whatMethods, 
       const char *headerName,
       const char *headerValue,
       InstanceHandleT handle = NULL
       );
*/

  int i = 0, pos = 0, posCommand=0, command = 0;
  if (userString == NULL || value == NULL)
  {
    //return failure
    return EFAILED;
  }
  for ( ; userString[i] != '\0'; i++)
  {
    if (userString[i] == ':' && userString[i+1] == ':')
    {
      pos=i;
      break;
    }
  }
  if (pos!=0)
  {

    for ( i=pos+2; userString[i] != '\0'; i++)
    {
      if (userString[i] == ':' && userString[i+1] == ':')
      {
        posCommand=i;
        break;
      }
    }
    if (posCommand!=0)
    {
      if (strcmp(&userString[posCommand+2], OEM_RTSP_HEADER_ADD) == 0)
      {
        command = 2;
      }
      else if (strcmp(&userString[posCommand+2], OEM_RTSP_HEADER_DELETE) == 0)
      {
        command = 1;
      }
      else
      {
        //return failure
        return EFAILED;
      }
    }
    else
    {
      //return failure
      return EFAILED;
    }

    userString[posCommand] = '\0';

    // this should be RTSPHeader request
    if (!strncmp(userString, RTSP_METHOD_DESCRIBE, sizeof(RTSP_METHOD_DESCRIBE)-1))
    {
      // parsing logic to separate the RTSP_METHOD_DESCRIBE::CSEQ
      QtvPlayer::OemRtspHeaderResult result = QtvPlayer::EditOemRtspHeaders((QtvPlayer::OemRtspHeaderCommand)command,
                                                                            (QtvPlayer::AffectedRTSPMethod )1, &userString[pos+2], value, pOEM->m_pHandle);
      if (result == QtvPlayer::OEM_RTSP_RESULT_OK)
      {
        return SUCCESS;
      }
      return EFAILED;
    }
    else if (!strncmp(userString, RTSP_METHOD_SETUP, sizeof(RTSP_METHOD_SETUP)-1))
    {
      QtvPlayer::OemRtspHeaderResult result = QtvPlayer::EditOemRtspHeaders((QtvPlayer::OemRtspHeaderCommand)command,
                                                                            (QtvPlayer::AffectedRTSPMethod )2, &userString[pos+2], value, pOEM->m_pHandle);
      if (result == QtvPlayer::OEM_RTSP_RESULT_OK)
      {
        return SUCCESS;
      }
      return EFAILED;
    }
    else if (!strncmp(userString, RTSP_METHOD_PLAY, sizeof(RTSP_METHOD_PLAY)-1))
    {
      QtvPlayer::OemRtspHeaderResult result = QtvPlayer::EditOemRtspHeaders((QtvPlayer::OemRtspHeaderCommand)command,
                                                                            (QtvPlayer::AffectedRTSPMethod )4, &userString[pos+2], value, pOEM->m_pHandle);
      if (result == QtvPlayer::OEM_RTSP_RESULT_OK)
      {
        return SUCCESS;
      }
      return EFAILED;
    }
    else if (!strncmp(userString, RTSP_METHOD_PAUSE, sizeof(RTSP_METHOD_PAUSE)-1))
    {
      QtvPlayer::OemRtspHeaderResult result = QtvPlayer::EditOemRtspHeaders((QtvPlayer::OemRtspHeaderCommand)command,
                                                                            (QtvPlayer::AffectedRTSPMethod )8, &userString[pos+2], value, pOEM->m_pHandle);
      if (result == QtvPlayer::OEM_RTSP_RESULT_OK)
      {
        return SUCCESS;
      }
      return EFAILED;
    }
    else if (!strncmp(userString, RTSP_METHOD_TEARDOWN, sizeof(RTSP_METHOD_TEARDOWN)-1))
    {
      QtvPlayer::OemRtspHeaderResult result = QtvPlayer::EditOemRtspHeaders((QtvPlayer::OemRtspHeaderCommand)command,
                                                                            (QtvPlayer::AffectedRTSPMethod )16, &userString[pos+2], value, pOEM->m_pHandle);
      if (result == QtvPlayer::OEM_RTSP_RESULT_OK)
      {
        return SUCCESS;
      }
      return EFAILED;
    }
    else if (!strncmp(userString, RTSP_METHOD_OPTIONS_RESPONSE, sizeof(RTSP_METHOD_OPTIONS_RESPONSE)-1))
    {
      QtvPlayer::OemRtspHeaderResult result = QtvPlayer::EditOemRtspHeaders((QtvPlayer::OemRtspHeaderCommand)command,
                                                                            (QtvPlayer::AffectedRTSPMethod )32, &userString[pos+2], value, pOEM->m_pHandle);
      if (result == QtvPlayer::OEM_RTSP_RESULT_OK)
      {
        return SUCCESS;
      }
      return EFAILED;
    }
    else
    {
      return EFAILED;
    }
  }
  else
  {
    if (!strncmp(userString, RTSP_USER_AGENT, sizeof(RTSP_USER_AGENT)))
    {
      if (value == NULL)
      {
        return EFAILED;
      }
      else
      {
        if (QtvPlayer::SetUserAgent(value, pOEM->m_pHandle) == QtvPlayer::QTV_RETURN_OK)
        {
          return SUCCESS;
        }
        else
        {
          return EFAILED;
        }
      }
    }
    else if (!strncmp(userString, RTSP_USER_AGENT_PARAM, sizeof(RTSP_USER_AGENT_PARAM)))
    {
      if (value == NULL)
      {
        return EFAILED;
      }
      else
      {
        if (QtvPlayer::SetUserAgentParameters(value, pOEM->m_pHandle) == QtvPlayer::QTV_RETURN_OK)
        {
          return SUCCESS;
        }
        else
        {
          return EFAILED;
        }
      }
    }
    else if (!strncmp(userString, RTP_UDP_DATA_PORT_RANGE, sizeof(RTP_UDP_DATA_PORT_RANGE)))
    {
      if (value == NULL)
      {
        MSG_ERROR("value is NULL.. returning EFAILED",0,0,0);
        return EFAILED;
      }
      else
      {
        int max = 0, min = 0, i = 0, pos = -1;

        for ( ; value[i] != '\0'; i++)
        {
          if (value[i] == '$' && value[i+1] != '\0' && i>0 )
          {
            pos=i;
            break;
          }
          min = min*10 + (value[i]-'0');
        }
        if (pos>0)
        {
          max = atoi(&value[pos + 1 ]);
          MSG_HIGH("min : %d max %d", min, max, 0);
          QtvPlayer::SetDataPortRange(min, max);
          return SUCCESS;
        }
        MSG_ERROR("returning EFAILED",0,0,0);
        return EFAILED;
      }
    }
    else
    {
      MSG_ERROR("returning EFAILED",0,0,0);
      return EFAILED;
    }
  }  
}


extern "C" int OEMMediaMPEG42Qtv_SetUserAgent(char *userAgentName)
{
  if(userAgentName == NULL)
  {
    return EFAILED;
  }
  else
  {		
    if(QtvPlayer::SetUserAgent(userAgentName) == QtvPlayer::QTV_RETURN_OK)
    {
      return SUCCESS;
    }
    else
    {
      return EFAILED;
    }
  }
}

extern "C" int OEMMediaMPEG42PV_OpenURN(char *videoFileName, char *audioFileName, char *textFileName, OEMHandle pOEM)
{
  QtvPlayer::ReturnT retVal = QtvPlayer::QTV_RETURN_OK;
  pOEM->m_bRepositionInPauseUpdateTime = FALSE;
  char blankURN[] = "\0";
  
  MSG_HIGH("OEMMediaMPEG42PV_OpenURN Entry", 0, 0, 0);
  if( (!audioFileName || !strlen(audioFileName)) && (!videoFileName || !strlen(videoFileName)) 
      && (!textFileName || !strlen(textFileName)))
  {
    MSG_ERROR("No URN passed in OEMMediaMPEG42PV_OpenURN", 0, 0, 0);
    return EFAILED;
  }
  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);
  if(pOEM == NULL)
  {
    MSG_ERROR("OEMMediaMPEG42PV_OpenURN, pOEM is null.", 0, 0, 0);
    return EFAILED;
  }
  /* reset any call backs pending */
  pOEM->m_bPBReadyPending = TRUE;
  pOEM->m_Sound.cmd_pending = FALSE;
  pOEM->m_Sound.cb_pending = FALSE;

#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
  pOEM->m_bFrameTransformPending = FALSE;
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */

#ifdef FEATURE_QTV_IPL_SCALING
#error code not present
#endif /* FEATURE_QTV_IPL_SCALING */

#ifdef FEATURE_MP4_AAC_PLUS
  /* Initialized to no rotation */
  pOEM->m_IPLRotationFactor = MM_MPEG4_NO_ROTATION;
#endif /* FEATURE_MP4_AAC_PLUS */

#if defined(FEATURE_QTV_MDP_TRANSFORMATIONS) || defined(FEATURE_QTV_XSCALE_VIDEO)
  pOEM->m_QtvCurrentRotation = OEM_NO_ROTATION;
  pOEM->m_QtvCurrentScaling  = OEM_NO_SCALING;
#endif /* FEATURE_QTV_MDP_TRANSFORMATIONS || FEATURE_QTV_XSCALE_VIDEO */

#ifdef FEATURE_MP4_ENABLE_SET_DEBUG_VARIABLES
  // If this is a Debug URL file, then set the variable value
  // based upon the text within the URL file
  if (CheckDebugURLFile(videoFileName, audioFileName))
  {
    return SUCCESS;
  }
#endif //FEATURE_MP4_ENABLE_SET_DEBUG_VARIABLES

#ifdef FEATURE_MP4_WRITE_CLIP_STATS_TO_FILE
  mp4WriteClipStatsFileName[0] = '\0';
  if (mp4WriteClipStatsToFileFlag)
  {
    if (videoFileName && videoFileName[0] != '\0')
    {
       (void)std_strlcpy(mp4WriteClipStatsFileName, videoFileName, AEE_MAX_FILE_NAME);
    }
    else if (audioFileName && audioFileName[0] != '\0')
    {
       (void)std_strlcpy(mp4WriteClipStatsFileName, audioFileName, AEE_MAX_FILE_NAME);
    }
    else if (textFileName && textFileName[0] != '\0')
    {
       (void)std_strlcpy(mp4WriteClipStatsFileName, textFileName, AEE_MAX_FILE_NAME);
    }
  }
#endif

   if(!videoFileName)
   {
     videoFileName = blankURN;
   }

   if(!audioFileName)
   {
     audioFileName = blankURN;
   }

   if(!textFileName)
   {
     textFileName = blankURN;
   }

   retVal = QtvPlayer::OpenURN(videoFileName, audioFileName, textFileName, pOEM->m_pHandle, 
                               (QtvPlayer::InstancePriorityT)pOEM->m_nPriority);
   if (retVal == QtvPlayer::QTV_RETURN_OK)
   {
   
	   MSG_HIGH("OEMMediaMPEG42PV_OpenURN SUCCESS Exit", 0, 0, 0);
      return SUCCESS;
   }
   else if (retVal == QtvPlayer::QTV_RETURN_UNSUPPORTED)
   {   
	   MSG_HIGH("OEMMediaMPEG42PV_OpenURN EUNSUPPORTED Exit", 0, 0, 0);
      return EUNSUPPORTED;
   }  
   else
   {
   
	   MSG_HIGH("OEMMediaMPEG42PV_OpenURN EFAILED Exit", 0, 0, 0);
      return EFAILED;
   }  
}

#ifdef FEATURE_QTV_FCS
#error code not present
#endif

extern "C" int OEMMediaMPEG42PV_Pause(OEMHandle pOEM)
{
   QtvPlayer::ReturnT retVal = QtvPlayer::QTV_RETURN_OK;
	
   MSG_HIGH("OEMMediaMPEG42PV_Pause,Entry", 0, 0, 0);
   pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);
   if(pOEM == NULL)
   {
     MSG_ERROR("OEMMediaMPEG42PV_Pause, pOEM is null.", 0, 0, 0);
     return EFAILED;
   }
   retVal = QtvPlayer::Pause(pOEM->m_pHandle);
   if (retVal == QtvPlayer::QTV_RETURN_OK)
   {
      pOEM->m_bPauseInProcess = TRUE; 
      MSG_HIGH("OEMMediaMPEG42PV_Pause,Exit SUCCESS", 0, 0, 0);
      return SUCCESS;
   }
   else
   {
      MSG_ERROR("OEMMediaMPEG42PV_Pause,Exit EFAILED", 0, 0, 0);
      return EFAILED;
   }
}

#ifdef FEATURE_QTV_FCS
#error code not present
#endif

extern "C" int OEMMediaMPEG42PV_Play(int startPos, OEMHandle pOEM)
{
  QtvPlayer::ReturnT retVal = QtvPlayer::QTV_RETURN_ERROR;
  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);

  MSG_HIGH("OEMMediaMPEG42PV_Play...startPos:%d",startPos,0,0);

  if(pOEM == NULL)
  {
    MSG_ERROR("OEMMediaMPEG42PV_Play, pOEM is null.", 0, 0, 0);
    return EFAILED;
  }

  // If the position at stop was stored, this is an indication that play 
  // should start from the position at stop. Used to test TSB scenario in 
  // which Qtv is terminated and reinitialized.
  int startPlayPos;
  if (pOEM->m_positionAtStop == 0)
  {
    startPlayPos = pOEM->m_nStartTime;
  }
  else
  {
    startPlayPos = pOEM->m_positionAtStop;
    pOEM->m_positionAtStop = 0;
  }

#ifdef FEATURE_QTV_GENERIC_BCAST
  uint32 TransID;
#endif /* FEATURE_QTV_GENERIC_BCAST */

  if(!pOEM->m_bPaused && pOEM->m_bPlayStarted)
  {
    pOEM->m_bPlayStarted = FALSE;
    pOEM->m_nPlayFrameMode = 0;
  }
  else
  {
    if ( pOEM->m_bSeekPending == TRUE )
    {
      MSG_ERROR("OEMMediaMPEG42PV_Play, returning EBADSTATE state.", 0, 0, 0);
      return EBADSTATE;
    }
  }

  if (startPos == -1)
  {
    /* Resuming from pause */
    if (pOEM->m_bRepositionInPause)
    {
      /* Repositioning occurred while paused */
      pOEM->m_bRepositionInPause = FALSE;
    }
    else
    {
      startPlayPos = -1;
    }
  }

  /* If clip information is available, send a MEDIA_SPEC callback */
  if( pOEM->m_bClipInfoAvail )
  {
    OEMMediaMPEG4_QueueCallback(pOEM->m_pCallbackData,
            MM_STATUS_MEDIA_SPEC,
            MM_CMD_PLAY,
            0,
            (void *)&pOEM->m_MPEG4Spec,
            sizeof(pOEM->m_MPEG4Spec));
  }

  if (pOEM->m_bSkipInPause)
  {
    retVal = QtvPlayer::PlaylistPlayClip(NULL, pOEM->m_nAccumulatedClipOffset, true, pOEM->m_nStartTime);    
  }
#ifdef FEATURE_QTV_GENERIC_BCAST
  else if (pOEM->m_nPlayFrameMode) 
  {
    retVal = (QtvPlayer::ReturnT)OEMMediaMPEG42PV_FrameSeek(pOEM->m_nPlayFrameMode, &TransID, TRUE, pOEM);
  }
#endif /* FEATURE_QTV_GENERIC_BCAST */
  else
  {
    /* Play the clip */
    retVal = QtvPlayer::PlayClip(startPlayPos, -1, pOEM->m_pHandle);
  }

  if (retVal == QtvPlayer::QTV_RETURN_OK)
  {
    if (pOEM->m_bSkipInPause)
    {      
      pOEM->m_nStartTime = 0;
      pOEM->m_nAccumulatedClipOffset = 0;
      pOEM->m_bSkipInPause = FALSE;
    }
    return SUCCESS;
  }
  
  pOEM->m_bSkipInPause = FALSE;
  
  MSG_HIGH("OEMMediaMPEG42PV_Play...return EFAILED",0,0,0);
  
  return EFAILED;
}


#if (defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION)
#error code not present
#endif /* (defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION) */

extern "C" int OEMMediaMPEG42PV_RegisterForCallback(void *cbData, OEMHandle pOEM)
{
   QtvPlayer::ReturnT retVal;

   // RegForCB is called before pOEM has been added to g_pOEMLayer
   if ( pOEM == NULL ) 
   {
      return EFAILED;
   }
   pOEM->m_pCallbackData = cbData;

   //retVal = QtvPlayer::RegisterForCallback(QtvEngineCBShell, cbData);
   retVal = QtvPlayer::RegisterForCallback(QtvEngineCB, cbData, pOEM->m_pHandle, pOEM);
   if (retVal == QtvPlayer::QTV_RETURN_OK)
   {
      return SUCCESS;
   }
   else
   {
      return EFAILED;
   }
}

void FetchDataSize(uint32 userData, uint32 * dnldDataSize, boolean *  pbEndOfData, QtvPlayer::InstanceHandleT Insttype)
{
  if(g_FetchBufferSizeCB)
  {
    MUTEX_LOCK();
    g_FetchBufferSizeCB(g_pClientData, dnldDataSize, pbEndOfData);
    (void)userData;
    (void)Insttype;	
    MUTEX_UNLOCK();
  }
}

uint32 HTTPFetchData (
      void *      dataBuf,         /* Destination buffer to copy the data */
      uint32      readSize,        /* Amount of data to be read */
      uint32      readPos,         /* Data read offset */
      uint32      TrcId,                      /* Media stream trackId */
      QtvPlayer::InstanceHandleT InstHdl             /* Qtv Instance handle */ 
    )
{
    MUTEX_LOCK();
    uint32 numRead = 0;
    
    if(g_FetchBufferCB)
    {
      numRead = g_FetchBufferCB(g_pClientData, dataBuf, readSize, readPos);
    }

    (void)TrcId;
    (void)InstHdl;	
    MUTEX_UNLOCK();
	
    return numRead;
}

#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)

unsigned char *HTTPPullInterfaceBuf = NULL;
uint32 HTTPPullBufSize = 0;
uint32 HTTPWriteBufferOffset = 0;

void HTTPFetchDnldDataSize (uint32 userData,uint32 *      dnldDataSize, boolean *     pbEndOfData, QtvPlayer::InstanceHandleT Insttype)
{
    *dnldDataSize = HTTPWriteBufferOffset;
    *pbEndOfData  = FALSE;
    (void)userData;
    (void)Insttype;
    return;
}

uint32 HTTPFetchDnldData (
      void *      dataBuf,         /* Destination buffer to copy the data */
      uint32      readSize,        /* Amount of data to be read */
      uint32      readPos,         /* Data read offset */
      uint32 TrcId,                      /* Media stream trackId */
      QtvPlayer::InstanceHandleT InstHdl             /* Qtv Instance handle */ 
    )
{
    (void)TrcId;
    (void)InstHdl;
    uint32 numRead = 0;
    if ( readPos<=(HTTPWriteBufferOffset-1) )
    {
      /* now read from this location */
      uint32 sizeToRead = MIN((HTTPWriteBufferOffset-readPos), readSize);
      if ( sizeToRead > 0 )
      {
        memcpy(dataBuf, (HTTPPullInterfaceBuf+readPos), sizeToRead);
        numRead = sizeToRead;
      }
    }
    return numRead;
}

extern "C" int OEMMediaMPEG4Qtv_UpdatePSBufferOffset(unsigned int offset)
{
  if(!HTTPPullInterfaceBuf)
  {
    QtvPlayer::UpdateHTTPStreamBufferWriteOffset(offset);
  }
  else
  {
    HTTPWriteBufferOffset = offset;
  }
  MSG_HIGH( "OEMMediaMPEG4Qtv_UpdatePSBufferOffset, HTTPWriteBufferOffset = %d ",
            HTTPWriteBufferOffset, 0, 0);
  return SUCCESS;
}

extern "C" int OEMMediaMPEG4Qtv_OpenPseudoStream(unsigned char *pBuf, uint32 bufSize, uint32 writePtrOffset,
                                                 uint32 interfaceId, OEMHandle pOEM)
{
  QtvPlayer::ReturnT retVal = QtvPlayer::QTV_RETURN_ERROR;

  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);
  if(pOEM == NULL)
  {
    MSG_ERROR("OEMMediaMPEG4Qtv_OpenPseudoStream, pOEM is null.", 0, 0, 0);
    return EFAILED;
  }
  /* reset any call backs pending */
  pOEM->m_bPBReadyPending = TRUE;
  pOEM->m_Sound.cmd_pending = FALSE;
  pOEM->m_Sound.cb_pending = FALSE;

  if(interfaceId == MMD_HTTP_PUSH_BUFFER_PLAYBACK)
  {
    //reset the global's
    HTTPPullInterfaceBuf = NULL;
    HTTPPullBufSize = 0;
    HTTPWriteBufferOffset = 0;

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
    retVal = QtvPlayer::OpenHTTPStream(pBuf, bufSize, writePtrOffset,
                                       (QtvPlayer::InstanceHandleT)pOEM->m_pHandle,
                                       (QtvPlayer::InstancePriorityT)pOEM->m_nPriority);
#elif defined FEATURE_QTV_PSEUDO_STREAM
    retVal = QtvPlayer::OpenPseudoStream(pBuf, bufSize, writePtrOffset,
                                       (QtvPlayer::InstanceHandleT)pOEM->m_pHandle,
                                       (QtvPlayer::InstancePriorityT)pOEM->m_nPriority);
#endif
  }
  else if (interfaceId == MMD_HTTP_PULL_BUFFER_PLAYBACK)
  {
    HTTPPullInterfaceBuf = pBuf;
    HTTPPullBufSize = bufSize;
    HTTPWriteBufferOffset = MAXIMUM_MIME_BUFFER_LENGTH;

    if (pOEM->m_bPULLDataMode)
    {
      g_pClientData       = pOEM->m_pClientData;
      g_FetchBufferSizeCB = pOEM->m_FetchBufferSizeCB;
      g_FetchBufferCB     = pOEM->m_FetchBufferCB;

      retVal = QtvPlayer::OpenPullBufferedStream((QtvPlayer::FetchBufferedDataSizeT)FetchDataSize,
                                                 (QtvPlayer::FetchBufferedDataT)HTTPFetchData,
                                                  0x7,
                                                 (QtvPlayer::InstanceHandleT)pOEM->m_pHandle,
                                                 (QtvPlayer::InstancePriorityT)pOEM->m_nPriority);
    }
    else
    {

      retVal = QtvPlayer::OpenPullBufferedStream((QtvPlayer::FetchBufferedDataSizeT)HTTPFetchDnldDataSize,
                                                 (QtvPlayer::FetchBufferedDataT)HTTPFetchDnldData,
                                                  0x7,
                                                 (QtvPlayer::InstanceHandleT)pOEM->m_pHandle,
                                                 (QtvPlayer::InstancePriorityT)pOEM->m_nPriority);
    }
  }
  else
  {
    MSG_ERROR( "OEMMediaMPEG4Qtv_OpenPseudoStream: Invalid interfaceId ..!!",
               HTTPWriteBufferOffset, 0, 0);
  }

  return retVal;
}

extern "C" int OEMMediaMPEG4Qtv_SetStartAndBufferingTime(uint32 startupTime,
                                                         uint32 bufferingResumeTime,
							 OEMHandle pOEM)
{
  QtvPlayer::ReturnT nReturn;
  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);
  if(pOEM == NULL)
  {
      MSG_ERROR("OEMMediaMPEG42PV_GetFrame, pOEM is null.", 0, 0, 0);
      return EFAILED;
  }
  if (pOEM->m_pHandle) 
  {
      nReturn = QtvPlayer::SetStartAndBufferingTime(startupTime, bufferingResumeTime, (QtvPlayer *)pOEM->m_pHandle);
  }
  else
  {
      nReturn = QtvPlayer::SetStartAndBufferingTime(startupTime, bufferingResumeTime);
  }
  if (nReturn == QtvPlayer::QTV_RETURN_OK)
  {
  return SUCCESS;
  }
  else
  {
      return EFAILED;
  }
}

#endif /* defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) */


extern "C" int  OEMMediaMPEG42PV_SetStartPos(uint32 lTimeMS, OEMHandle pOEM)
{

  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);
  if(pOEM == NULL)
  {
    MSG_ERROR("OEMMediaMPEG42PV_SetStartPos, pOEM is null.", 0, 0, 0);
    return EFAILED;
  }

  /* to set start time, QTV should be either in IDLE or READY state (means clip is not playing) */
  if( (pOEM->m_nMPEG4EngineState != QtvPlayer::QTV_PLAYER_STATUS_PLAYBACK_READY) &&
      (pOEM->m_nMPEG4EngineState != QtvPlayer::QTV_PLAYER_STATUS_IDLE) )
  {
    return EBADSTATE;
  }
  if( pOEM->m_bClipInfoAvail && (lTimeMS >= ((QtvPlayer::ClipInfoT*)pOEM->m_pClipInfo)->Duration) )
  {
    return EBADPARM;
  }

  pOEM->m_nStartTime = lTimeMS;
  return SUCCESS;
}

extern "C" int OEMMediaMPEG42PV_Seek(AEEMediaSeek eSeek, int32 TimeOrFrame, OEMHandle pOEM)
{
  QtvPlayer::PlayerStateRecordT state;
  QtvPlayer::ReturnT retVal;
  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);
  MSG_HIGH("OEMMediaMPEG42PV_Seek, Entry", 0, 0, 0);
  if(pOEM == NULL)
  {
    MSG_ERROR("OEMMediaMPEG42PV_Seek, pOEM is null.", 0, 0, 0);
    return EFAILED;
  }
  QtvPlayer::ClipInfoT *pClipInfo = (QtvPlayer::ClipInfoT*)pOEM->m_pClipInfo;

  retVal = QtvPlayer::GetPlayerState(state, pOEM->m_pHandle);

  if( eSeek & MM_SEEK_MODE_FRAME )
  {
    retVal = QtvPlayer::QTV_RETURN_ERROR;

    if(TimeOrFrame > 0)
    {
      retVal = QtvPlayer::PlayNextFrame();
    }
    else if(TimeOrFrame < 0)
    {
      retVal = QtvPlayer::PlayPrevFrame();
    }

   if (retVal == QtvPlayer::QTV_RETURN_FEATURE_NOT_AVAILABLE)
    {
      MSG_ERROR("OEMMediaMPEG42PV_Seek, returning EUNSUPPORTED.", 0, 0, 0);
      return EUNSUPPORTED;
    }
    else if (retVal == QtvPlayer::QTV_RETURN_OK)
    {
      return SUCCESS;
    }
  }

  if (retVal != QtvPlayer::QTV_RETURN_OK)
  {
    MSG_ERROR("OEMMediaMPEG42PV_Seek, returning EFAILED.", 0, 0, 0);
    return EFAILED;
  }

  if( pOEM->m_bSeekPending == TRUE )
  {
    return EALREADY;
  }

  if (pOEM->m_bPaused == FALSE)
  {
    pOEM->m_nStartTime = state.playbackMsec;
  }

#ifdef FEATURE_QTV_SERVER_SIDE_PLAYLIST
  if ((pClipInfo->ExtInfoType == QtvPlayer::CLIP_EXT_INFO_STANDARD) &&
      (pClipInfo->ExtInfo.StdInfo.NumClipsInServerSidePlaylist != 0))
  {
    if (eSeek & MM_SEEK_MODE_CHAPTER)
    {
      if ((eSeek & MM_SEEK_CURRENT) != 0)
      {
        if (TimeOrFrame >= 1)
        {
          if (pClipInfo->ExtInfo.StdInfo.IsNextClipInServerSidePlaylistAvail == 0)
          {
            return EFAILED;
          }
        }
        else if (TimeOrFrame <= -1)
        {
          if (pClipInfo->ExtInfo.StdInfo.IsPrevClipInServerSidePlaylistAvail == 0)
          {
            return EFAILED;
          }
        }          

        pOEM->m_nAccumulatedClipOffset += TimeOrFrame;
      }
      else if ((eSeek & MM_SEEK_START) != 0)
      {
        // Convert to a relative offset.
        pOEM->m_nAccumulatedClipOffset = TimeOrFrame - pOEM->m_nCurrentClipIndex;
      }
      else 
      {
        // Not supported
        return EFAILED;
      }

      pOEM->m_nStartTime = 0;

      if (pOEM->m_nMPEG4EngineState == QtvPlayer::QTV_PLAYER_STATUS_PAUSED)
      {
        pOEM->m_bRepositionInPause = TRUE;        
        pOEM->m_bSkipInPause = TRUE;
        OEMMediaMPEG4_QueueCallback(pOEM->m_pCallbackData,
                                  MM_STATUS_SEEK,
                                  MM_CMD_PLAY,
                                  0,
                                  (void *) (int32) 0, 
                                  sizeof(int32));
      }
      else
      {
        retVal = QtvPlayer::PlaylistPlayClip(NULL, pOEM->m_nAccumulatedClipOffset, true, pOEM->m_nStartTime);
        if (retVal == QtvPlayer::QTV_RETURN_OK)
        {
          pOEM->m_nAccumulatedClipOffset = 0;
        }
        else
        {
          MSG_ERROR("OEMMediaMPEG42PV_Seek, returning EFAILED.", 0, 0, 0);
          return EFAILED;
        }
        pOEM->m_bSeekPending = TRUE;
      }
      return SUCCESS;
    }
  }
#endif /* FEATURE_QTV_SERVER_SIDE_PLAYLIST */

  // All other seek cases require that repositioning is allowed.
  if (pClipInfo->RepositioningAllowed == FALSE)
  {
    MSG_ERROR("OEMMediaMPEG42PV_Seek, returning EFAILED.", 0, 0, 0);
    return EFAILED;
  }

#ifdef FEATURE_FILE_FRAGMENTATION
  if (eSeek & MM_SEEK_MODE_CHAPTER)
  {
    if (pOEM->m_nMPEG4EngineState != QtvPlayer::QTV_PLAYER_STATUS_PAUSED)
    {
      pOEM->m_bSeekPending = TRUE;
      retVal = QtvPlayer::SkipClip(TimeOrFrame);
      if (retVal != QtvPlayer::QTV_RETURN_OK)
      {
        pOEM->m_bSeekPending = FALSE;
        MSG_ERROR("OEMMediaMPEG42PV_Seek, returning EFAILED.", 0, 0, 0);
        return EFAILED;
      }
    }
    return SUCCESS;
  }
#else // seek by chapter is not supported in single file playback when FEATURE_FILE_FRAGMENTATION is not defined
#ifdef FEATURE_QTV_SERVER_SIDE_PLAYLIST
  if(pClipInfo->ExtInfo.StdInfo.NumClipsInServerSidePlaylist == 0)
  {
    if (eSeek & MM_SEEK_MODE_CHAPTER)
    {
      MSG_ERROR("OEMMediaMPEG42PV_Seek, returning EUNSUPPORTED.", 0, 0, 0);
      return EUNSUPPORTED;
    }    
  }
#else
  if (eSeek & MM_SEEK_MODE_CHAPTER)
  {
    MSG_ERROR("OEMMediaMPEG42PV_Seek, returning EUNSUPPORTED.", 0, 0, 0);
    return EUNSUPPORTED;
  }  
#endif /* FEATURE_QTV_SERVER_SIDE_PLAYLIST */
#endif /*FEATURE_FILE_FRAGMENTATION*/

  if(eSeek == MM_SEEK_START)
  {
    if(pClipInfo->Duration)
    {
      /* Do not allow seek beyond the end of a clip to avoid a race condition when a SEEK request is received 
       * while Mpeg4Player has done with playback but hasn't sent QtvPlayer::QTV_PLAYER_STATUS_IDLE callback. 
       */
      if(TimeOrFrame >= pClipInfo->Duration)
      {
        MSG_ERROR("OEMMediaMPEG42PV_Seek, returning EBADPARM.", 0, 0, 0);
        return EBADPARM;
      }
      pOEM->m_nStartTime = MAX(0,TimeOrFrame);
    }
    else
    {
#ifdef FEATURE_QTV_PV_SERVER_SIDE_PLAYLIST
        // Duration is 0 since not known on PV server side playlist.
        // Don't check against duration.
        pOEM->m_nStartTime = MAX(0, TimeOrFrame);
#else
        return EFAILED;
#endif /* FEATURE_PV_SERVER_SIDE_PLAYLIST */
    }
  }
  else if(eSeek & MM_SEEK_END)
  {
    if(pClipInfo->Duration)
    {
      /* Do not allow seek beyond the end of a clip to avoid a race condition when a SEEK request is received 
       * while Mpeg4Player has done with playback but hasn't sent QtvPlayer::QTV_PLAYER_STATUS_IDLE callback. 
       */
      if(TimeOrFrame >= 0)
      {
        MSG_ERROR("OEMMediaMPEG42PV_Seek, returning EBADPARM.", 0, 0, 0);
        return EBADPARM;
      }
      else
      {
        pOEM->m_nStartTime = MAX(0, pClipInfo->Duration + TimeOrFrame);
      }
    }
    else
    {
        MSG_ERROR("OEMMediaMPEG42PV_Seek, returning EFAILED.", 0, 0, 0);
        return EFAILED;
    }
  }
  else if(eSeek & MM_SEEK_CURRENT)
  {
    pOEM->m_nStartTime = MAX(pOEM->m_nStartTime + TimeOrFrame, 0);

    if (pClipInfo->Duration > 0)
    {
      /* Do not allow seek beyond the end of a clip to avoid a race condition when a SEEK request is received 
       * while Mpeg4Player has done with playback but hasn't sent QtvPlayer::QTV_PLAYER_STATUS_IDLE callback. 
       */
      if(pOEM->m_nStartTime >= pClipInfo->Duration)
      {
        MSG_ERROR("OEMMediaMPEG42PV_Seek, returning EBADPARM.", 0, 0, 0);
        return EBADPARM;
      }
    }
  }
  else
  {
      MSG_ERROR("OEMMediaMPEG42PV_Seek, returning EFAILED.", 0, 0, 0);
      return EFAILED;
  }

  if (pOEM->m_nMPEG4EngineState != QtvPlayer::QTV_PLAYER_STATUS_PAUSED)
  {
    pOEM->m_bSeekPending = TRUE;
    // Added for CR 109813 fix, IMedia Documentation states that MM_STATUS_SEEK callback just indicates that 
    // the seek command is started but doesn't indicate that seek is done. MM_STATUS_RESUME callback 
    // indicates that the seek command is finished and playback resumes with correct elapsed time.
    pOEM->m_bResumePending = TRUE;
    retVal = QtvPlayer::PlayClip(pOEM->m_nStartTime, -1, pOEM->m_pHandle);
    if (retVal != QtvPlayer::QTV_RETURN_OK)
    {
      pOEM->m_bSeekPending = FALSE;
      pOEM->m_bResumePending = FALSE;
      MSG_ERROR("OEMMediaMPEG42PV_Seek, returning EFAILED.", 0, 0, 0);
      return EFAILED;
    }
  }
  else
  {
    pOEM->m_bRepositionInPause = TRUE;
    pOEM->m_bRepositionInPauseUpdateTime = TRUE;
    OEMMediaMPEG4_QueueCallback(pOEM->m_pCallbackData,
                              MM_STATUS_SEEK,
                              MM_CMD_PLAY,
                              0,
                              (void *)pOEM->m_nStartTime,
                              sizeof(pOEM->m_nStartTime));
  }
  
  MSG_HIGH("OEMMediaMPEG42PV_Seek...return SUCCESS",0,0,0);
  
  return SUCCESS;
}

extern "C" int OEMMediaMPEG42PV_GetFrame(IBitmap** ppFrame,
                                         AEEMediaExtFrameInfoHeader** ppExtFrmInfo,
                                         OEMHandle pOEM)
{
  QtvPlayer::FrameInfoT  frameInfo;
  QtvPlayer::ReturnT nReturn;
  IBitmap  *pIBitmap;

#if (defined (FEATURE_MP4_AAC_PLUS) || defined (PLATFORM_LTK) )
  AEEMediaMP4CodecType videocodectype;
  AEEMediaMP4CodecType audiocodectype;
#endif /* FEATURE_MP4_AAC_PLUS || PLATFORM_LTK */
#ifdef FEATURE_QTV_MDP
  IYCbCr *pYCbCr;
  YCbCr  *pYCbCrInternal;
#else
  IDIB  *pDIB;
#endif /* !FEATURE_QTV_MDP */

  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);
  if(pOEM == NULL)
  {
    MSG_ERROR("OEMMediaMPEG42PV_GetFrame, pOEM is null.", 0, 0, 0);
    return EFAILED;
  }
    /* Always ask PV for the frame information
  ** You must ALWAYS attempt to get a frame if you've been
  ** told there is a frame to get.  If a frame is not
  ** retrieved from QtvPlayer, it leads to an error state
  ** of lost frame in Mpeg4Player. If a extended frame info
  ** pointer is provided, use the GetExtFrameInfo function
  ** otherwise use GetFrameInfo.
  */

  if (ppExtFrmInfo) 
  {
    nReturn = QtvPlayer::GetExtFrameInfo(frameInfo, (void**)ppExtFrmInfo);
  }
  else
  {
    if (pOEM->m_pHandle) 
    {
        nReturn = QtvPlayer::GetFrameInfo(frameInfo,(QtvPlayer *)pOEM->m_pHandle);
    }
    else
    {
        nReturn = QtvPlayer::GetFrameInfo(frameInfo);
    }
  }
#ifdef FEATURE_QTV_OEM_BUFFER_MGR
  AddRefOutputBuffer(frameInfo.RGBBuffer);
#endif /* FEATURE_QTV_OEM_BUFFER_MGR */

  if (!ppFrame)
  {
    OEMMediaMPEG42PV_ReleaseCurrentVideoFrameBuffer(frameInfo.RGBBuffer);
    return EBADPARM;
  }

  *ppFrame = NULL;

 
  if( nReturn != QtvPlayer::QTV_RETURN_OK ||
      frameInfo.RGBBuffer == NULL )
  {
    return EFAILED;
  }

  /* The current frame can be YUV or RGB depending on our platform...
  */
  if
  (
    nReturn != QtvPlayer::QTV_RETURN_OK ||
#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
    frameInfo.YUVBuffer == NULL
#else
    frameInfo.RGBBuffer == NULL
#endif /* FEATURE_QTV_MDP_TRANSFORMATIONS */
  )
  {
    return EFAILED;
  }

  /* if we are not playing, don't send any frame.
  */
  if( (pOEM->m_nMPEG4EngineState == QtvPlayer::QTV_PLAYER_STATUS_IDLE) ||
      (pOEM->m_nMPEG4EngineState == QtvPlayer::QTV_PLAYER_STATUS_PLAYBACK_READY) )
  {
#ifdef FEATURE_QTV_MDP
    /* Be sure to release the buffer back to the player, however */
    OEMMediaMPEG42PV_ReleaseCurrentVideoFrameBuffer(frameInfo.YUVBuffer); 
#endif /* FEATURE_QTV_MDP */
    return EBADSTATE;
  }

#if (defined (FEATURE_MP4_AAC_PLUS) || defined (PLATFORM_LTK) )
  videocodectype = OEMMediaMPEG42PV_GetVideoCodecType();
#endif /* FEATURE_MP4_AAC_PLUS || PLATFORM_LTK */

#ifdef FEATURE_MP4_AAC_PLUS
  audiocodectype = OEMMediaMPEG42PV_GetAudioCodecType();

/* Color conversion & rotation is done by Qtv IPL if MPEG4-AAC+ DSP image is downloaded 
 * i.e. qtv_cfg_enable_aacplus is TRUE 
 */
  if ( (qtv_cfg_enable_aacplus) && 
       (audiocodectype == MM_MPEG4_AAC_CODEC) && 
       ( (videocodectype == MM_MPEG4_MPEG4_CODEC) || 
         (videocodectype == MM_MPEG4_H263_CODEC && !qtv_cfg_enable_h263_profile3 ) ) && 
       (frameInfo.Height <= MP4_QCIF_HEIGHT) && (frameInfo.Width <= MP4_QCIF_WIDTH) )
  {
    RotateColorConvert( &frameInfo, pOEM );
  } else
#endif /* FEATURE_MP4_AAC_PLUS */
  {
#ifdef FEATURE_QTV_IPL_SCALING
#error code not present
#endif /* FEATURE_QTV_IPL_SCALING */
  }

  /* Create a new bitmap for this frame */
  if( QtvBitmap_New(&pIBitmap) != SUCCESS )
  {
     OEMMediaMPEG42PV_ReleaseCurrentVideoFrameBuffer(frameInfo.YUVBuffer);
     return EFAILED;
  }

  /* Add to the reference count for the bitmap since the app will free it */
  IBITMAP_AddRef(pIBitmap);

#ifdef FEATURE_QTV_MDP
  /* Update our local IBitmap to point to the right buffer, size, etc. */
  if( SUCCESS != IBITMAP_QueryInterface(pIBitmap, AEEIID_YCBCR, (void**)&pYCbCr) )
  {
    /*since ref count is 2, we release both the buffer and bitmap since bitmap is created for each frame received */
    OEMMediaMPEG42PV_ReleaseCurrentVideoFrameBuffer(frameInfo.YUVBuffer);
    IBITMAP_Release(pIBitmap);
    return EFAILED;
  }

  pYCbCr->pLuma   = (void*)frameInfo.YUVBuffer;
  pYCbCr->pChroma = (void*)&(((byte*)frameInfo.YUVBuffer)[frameInfo.Width * frameInfo.Height]);
#ifdef FEATURE_QTV_MDP_COLOR_FORMAT_SWAP
#error code not present
#else
  pYCbCr->uScheme = IYCBCR_COLORSCHEME_420LP;
#endif
  pYCbCr->cx      = frameInfo.CropWindow.x2 - frameInfo.CropWindow.x1 + 1;
  pYCbCr->cy      = frameInfo.CropWindow.y2 - frameInfo.CropWindow.y1 + 1;
  pYCbCr->nYPitch = frameInfo.Width;
  pYCbCr->nCPitch = frameInfo.Width;

  pYCbCrInternal = (YCbCr*) pYCbCr;
  switch( pOEM->m_QtvCurrentRotation )
  {
    case OEM_90_CW_ROTATION: /* 90 */
      pYCbCrInternal->rotate = YCBCR_ROTATION_90CW;
      break;
    case OEM_180_ROTATION: /* 180 */
      pYCbCrInternal->rotate = YCBCR_ROTATION_180;
      break;
    case OEM_90_CCW_ROTATION: /* 270 */
      pYCbCrInternal->rotate = YCBCR_ROTATION_90CCW;
      break;
    case OEM_NO_ROTATION: /* 0 - fallthrough */
    default:
      pYCbCrInternal->rotate = YCBCR_ROTATION_NONE;
  }
  switch( pOEM->m_QtvCurrentScaling )
  {
    case OEM_0P25X_SHRINK: /* 25% */
      pYCbCrInternal->scale = YCBCR_SCALING_0_25X;
      break;
    case OEM_0P5X_SHRINK: /* 50% */
      pYCbCrInternal->scale = YCBCR_SCALING_0_5X;
      break;
    case OEM_0P75X_SHRINK: /* 75% */
      pYCbCrInternal->scale = YCBCR_SCALING_0_75X;
      break;
    case OEM_2X_ZOOM: /* 200% */
      pYCbCrInternal->scale = YCBCR_SCALING_2X;
      break;
#ifdef FEATURE_QTV_MDP_ASCALE
    case OEM_ASCALE:
      pYCbCrInternal->scale = YCBCR_SCALING_ASCALE;
      pYCbCrInternal->ascale.x = pOEM->m_QtvCurrentAScale.x;
      pYCbCrInternal->ascale.y = pOEM->m_QtvCurrentAScale.y;
      break;
#endif /* FEATURE_QTV_MDP_ASCALE */
    case OEM_NO_SCALING: /* 100% - fallthrough */
    case OEM_XSCALE:     /*  dimensions from ClipInfo are correct */
    default:
      pYCbCrInternal->scale = YCBCR_SCALING_NONE;
      break;
  }
  
  IYCBCR_Release(pYCbCr);
#else
  /* Update our local IBitmap to point to the right buffer, size, etc. */
  if( SUCCESS != IBITMAP_QueryInterface(pIBitmap, AEECLSID_DIB, (void**)&pDIB) )
  {
    /*since ref count is 2, we release both the buffer and bitmap since bitmap is created for each frame received */
    OEMMediaMPEG42PV_ReleaseCurrentVideoFrameBuffer(frameInfo.RGBBuffer);
    IBITMAP_Release(pIBitmap);
    return EFAILED;
  }
#ifdef PLATFORM_LTK
#error code not present
#endif  /* PLATFORM_LTK */

  pDIB->pBmp   = (byte*)frameInfo.RGBBuffer;
  pDIB->nColorScheme = IDIB_COLORSCHEME_565;
  pDIB->nPitch = frameInfo.Width * 2;  /* Fix hard-coded bytes/pixel */
  pDIB->cx     = frameInfo.Width;
  pDIB->cy     = frameInfo.Height;

  IDIB_Release(pDIB);
#endif /* FEATURE_QTV_MDP */

  *ppFrame = pIBitmap;

  return SUCCESS;
}

extern "C" int OEMMediaMPEG42PV_SetRTSPStatusNotify(boolean notifyEnable, OEMHandle pOEM)
{
  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);
  if(pOEM == NULL)
  {
    MSG_ERROR("OEMMediaMPEG42PV_SetRTSPStatusNotify, pOEM is null.", 0, 0, 0);
    return EFAILED;
  }
  if (notifyEnable)
  {
    QtvPlayer::SetRTSPStatusCallback(RTSPStatusCB, pOEM->m_pHandle, pOEM);
  }
  else
  {
    QtvPlayer::SetRTSPStatusCallback(NULL, pOEM->m_pHandle, NULL);
  }
  return SUCCESS;
}

extern "C" int OEMMediaMPEG42PV_Stop(OEMHandle pOEM)
{
  QtvPlayer::ReturnT retVal;
  dword totalSleepDuration = 0;
  
  MSG_HIGH("OEMMediaMPEG42PV_Stop, Entry", 0, 0, 0);
  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);
  if(pOEM == NULL)
  {
    MSG_ERROR("OEMMediaMPEG42PV_Stop, pOEM is null.", 0, 0, 0);
    return EFAILED;
  }
  /* Return if still repositiong, rejecting stop.
   * user has to press stop again.
   */
  if ( pOEM->m_bSeekPending == TRUE )
  {
    return EBADSTATE;
  }

  retVal = QtvPlayer::Stop(pOEM->m_pHandle);

  // sleep for 100ms to prevent known problem caused by too fast start/stop (S-Magic)
  MSLEEP(100);

  if ( retVal == QtvPlayer::QTV_RETURN_OK )
  {
    int dog_autokick;

    dog_autokick = rex_autodog_enable(rex_self()->dog_report_val);

    /* Wait for the QTV engine to go to IDLE state before returning.
     * But limit this wait to no more than PLAYER_TRANSITION_TO_IDLE_TIMEOUT
     */

    while( pOEM->m_nMPEG4EngineState != QtvPlayer::QTV_PLAYER_STATUS_IDLE &&
           ( totalSleepDuration < PLAYER_TRANSITION_TO_IDLE_TIMEOUT ) )
    {
      MSLEEP(PLAYER_STATE_POLL_INTERVAL);
      totalSleepDuration += PLAYER_STATE_POLL_INTERVAL;
    }

    if (!dog_autokick)
    {
      rex_autodog_disable();
    }
  }

  if (pOEM->m_nMPEG4EngineState == QtvPlayer::QTV_PLAYER_STATUS_IDLE)
  {
    MSG_MED( "Engine reported IDLE in %d ms ",
              totalSleepDuration,
              0,
              0);
  }
  else
  {
    MSG_ERROR( "Engine did not report IDLE, current state = %d ",
               pOEM->m_nMPEG4EngineState,
               0,
               0);
  }

  if (retVal != QtvPlayer::QTV_RETURN_OK && pOEM->m_nMPEG4EngineState != QtvPlayer::QTV_PLAYER_STATUS_IDLE)
  {
    MSG_ERROR("OEMMediaMPEG42PV_Stop failed", 0, 0, 0);
    return EFAILED;
  }
   
  MSG_HIGH("OEMMediaMPEG42PV_Stop, Exit", 0, 0, 0);
  /* reset any call backs pending */
  pOEM->m_cbGenerateTotalTime = FALSE;
  pOEM->m_cbGenerateMediaSpec = FALSE;
  return SUCCESS;
}

extern "C" int OEMMediaMPEG42PV_Terminate(OEMHandle pOEM)
{
  QtvPlayer::ReturnT retVal = QtvPlayer::QTV_RETURN_OK;
  
  MSG_HIGH("OEMMediaMPEG42PV_Terminate, Entry", 0, 0, 0);  
  // pOEM has already been removed from g_pOEMLayer to ensure it
  // can't be used by anyone while it is being destroyed.
  if (pOEM == NULL)
  {
    return EFAILED;
  }

   #ifdef FEATURE_USE_CLKREGIM_DEVMAN_REV2 
      OEMMediaMPEG42PV_ClkrgmRelease(pOEM);
   #endif /* FEATURE_USE_CLKREGIM_DEVMAN_REV2 */


  /* At this time, a timer for updating the progress bar could be running
   * with NULL as user supplied data. The semantics of CancelTimer are such
   * that function and the user supplied data both need to be passed for a
   * timer to be cancelled. Hence, make sure to cancel timer for each user
   * supplied data if SetTimer is called with user data other than NULL
   */
#ifdef FEATURE_QTV_IPL_SCALING
#error code not present
#endif /* FEATURE_QTV_IPL_SCALING */

#ifdef FEATURE_MP4_AAC_PLUS
  /* Initialized to no rotation */
  pOEM->m_IPLRotationFactor = MM_MPEG4_NO_ROTATION;
#endif /* FEATURE_MP4_AAC_PLUS */

  AEE_CancelTimer(UpdateProgressBar, (void*)pOEM);

  // call QtvPlayer::Terminate only when pOEM->m_pHandle is not null
  if(pOEM->m_pHandle)
  {
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
    if(QtvPlayer::getDownloadClipType(pOEM->m_pHandle) != QtvPlayer::NONE ) 
    {
      AEE_CancelTimer(UpdateDownloadProgressBar, (void*)pOEM);
    }
#endif

    pOEM->m_bClipInfoAvail = FALSE;

    MSG_MED("Calling QtvPlayer::Terminate", 0, 0, 0);
    retVal = QtvPlayer::Terminate(pOEM->m_pHandle, TRUE);
    MSG_MED("QtvPlayer::Terminate() returned %d", retVal, 0, 0);
  }
  

     FREEIF(pOEM->m_pClipInfo);
	 
     FREEIF(pOEM->m_TimedText3GPP.pszText);
     FREEIF(pOEM->m_TimedText3GPP.pSampleModifiersBuffer);
     FREEIF(pOEM->m_TimedText3GPP.pFontList);

  

  /* reset any repositioning request */
  pOEM->m_bRepositionInPause = FALSE;
  pOEM->m_bRepositionInPauseUpdateTime = FALSE;

  pOEM->m_pCallbackData = NULL;

  // reset pull mode callback ptrs
  g_pClientData = NULL;
  g_FetchBufferSizeCB = NULL;
  g_FetchBufferCB = NULL;

  if (retVal == QtvPlayer::QTV_RETURN_OK)
  {
    return SUCCESS;
  }
  else
  {
    return EFAILED;
  }
}

AEEMediaMPEG4Type OEMMediaMPEG42PV_ConvertMediaTypePVToAEE(QtvPlayer::MediaTypeT pvType)
{
  AEEMediaMPEG4Type AeeType;
  switch(pvType)
  {
  case QtvPlayer::MEDIA_AUDIO_ONLY:
    AeeType = MM_MPEG4_AUDIO_ONLY;
    break;

  case QtvPlayer::MEDIA_VIDEO_ONLY:
    AeeType = MM_MPEG4_VIDEO_ONLY;
    break;

  case QtvPlayer::MEDIA_AUDIO_VIDEO:
    AeeType = MM_MPEG4_AUDIO_VIDEO;
    break;

  case QtvPlayer::MEDIA_VIDEO_STILL_IMAGE:
    AeeType = MM_MPEG4_VIDEO_STILL_IMAGE;
    break;

  case QtvPlayer::MEDIA_AUDIO_VIDEO_STILL_IMAGE:
    AeeType = MM_MPEG4_AUDIO_VIDEO_STILL_IMAGE;
    break;

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  case QtvPlayer::MEDIA_VIDEO_TEXT:
    AeeType = MM_MPEG4_VIDEO_TEXT;
    break;

  case QtvPlayer::MEDIA_AUDIO_TEXT:
    AeeType = MM_MPEG4_AUDIO_TEXT;
    break;

  case QtvPlayer::MEDIA_AUDIO_VIDEO_TEXT:
    AeeType = MM_MPEG4_AUDIO_VIDEO_TEXT;
    break;

  case QtvPlayer::MEDIA_VIDEO_STILL_IMAGE_TEXT:
    AeeType = MM_MPEG4_VIDEO_STILL_IMAGE_TEXT;
    break;

  case QtvPlayer::MEDIA_AUDIO_VIDEO_STILL_IMAGE_TEXT:
    AeeType = MM_MPEG4_AUDIO_VIDEO_STILL_IMAGE_TEXT;
    break;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

  default:
    AeeType = MM_MPEG4_INVALID;
    break;
  }
  return AeeType;
}


/* Convert the clip stream type from Qtvplayer type to OEM type */
AEEStreamType
OEMMediaMPEG42PV_ConvertStreamType(QtvPlayer::StreamTypeT stype)
{
    AEEStreamType  StreamType;
    switch(stype)
    {
        case QtvPlayer::MEDIA_NON_STREAMING:
            StreamType = MM_MEDIA_NON_STREAMING;
            break;

        case QtvPlayer::MEDIA_LIVE_STREAMING:
            StreamType = MM_MEDIA_LIVE_STREAMING;
            break;

#ifdef FEATURE_QTV_PV_SERVER_SIDE_PLAYLIST
        case QtvPlayer::MEDIA_PV_SERVER_SIDE_PLAYLIST:
            StreamType = MM_MEDIA_PV_SERVER_SIDE_PLAYLIST;
            break;
#endif /* FEATURE_QTV_SERVER_SIDE_PLAYLIST */

#ifdef FEATURE_QTV_WM_SERVER_SIDE_PLAYLIST
        case QtvPlayer::MEDIA_WM_SERVER_SIDE_PLAYLIST:
            StreamType = MM_MEDIA_WM_SERVER_SIDE_PLAYLIST;
            break;
#endif /* FEATURE_QTV_SERVER_SIDE_PLAYLIST */

        case QtvPlayer::MEDIA_STREAMING:
            StreamType = MM_MEDIA_STREAMING;
            break;
       
        case QtvPlayer::MEDIA_BUFFERED_LIVE_STREAMING:
            StreamType = MM_MEDIA_BUFFERED_LIVE_STREAMING;
            break;

        default:
            StreamType = MM_MEDIA_NON_STREAMING;
            break;
    }

    return StreamType;
}

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
AEEMediaPvxPlaybackControl
OEMMediaMPEG42PV_ConvertPlaybackControlType(QtvPlayer::DownloadPlaybackControlT stype)
{ 
    AEEMediaPvxPlaybackControl  PlaybackControlType = MM_PLAYBACK_RESERVE;
    switch(stype)
    {
        case QtvPlayer::NO_PLAYBACK:
            PlaybackControlType = MM_NO_PLAYBACK;
            break;

        case QtvPlayer::PLAYBACK_AFTER_DOWNLOAD:
            PlaybackControlType = MM_PLAYBACK_AFTER_DOWNLOAD;
            break;

        case QtvPlayer::PLAYBACK_ASAP:
            PlaybackControlType = MM_PLAYBACK_ASAP;
            break;

        case QtvPlayer::PLAYBACK_RESERVE:
            PlaybackControlType = MM_PLAYBACK_RESERVE;
            break;
    }

    return PlaybackControlType;
}
#endif /*  FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */

AEEMediaEndOfClipAction
OEMMediaMPEG42PV_ConvertEndofClipActionType(EndOfClipActionT stype)
{
    AEEMediaEndOfClipAction  action = MM_NO_END_OF_CLIP_ACTION;
    switch(stype)
    {
        case NO_END_OF_CLIP_ACTION:
            action = MM_NO_END_OF_CLIP_ACTION;
            break;

        case END_OF_CLIP_CLOSE_APP:
            action = MM_END_OF_CLIP_CLOSE_APP;
            break;

        case END_OF_CLIP_PLAY_NEXT_URL:
            action = MM_END_OF_CLIP_PLAY_NEXT_URL;
            break;

        case END_OF_CLIP_LAUNCH_URL:
            action = MM_END_OF_CLIP_LAUNCH_URL;
            break;
    }

    return action;
}

AEEMediaMP4CodecType OEMMediaMPEG42PV_GetAudioCodecType(OEMHandle pOEM)
{
  QtvPlayer::CodecType    PVCodecType;
  AEEMediaMP4CodecType    AudioCodecType;

  // Need to provide valid OEMhandle, return if pOEM is NULL
  if(!pOEM)
  {
    MSG_ERROR("OEMMediaMPEG42PV_GetAudioCodecType, pOEM passed in is NULL.", 0, 0, 0);
    return MM_MPEG4_UNKNOWN_CODEC;
  }

  PVCodecType = QtvPlayer::GetAudioCodecType(pOEM->m_pHandle);

  switch(PVCodecType)
  {
    case QtvPlayer::QTV_AAC_CODEC:
      AudioCodecType = MM_MPEG4_AAC_CODEC;
      break;

    case QtvPlayer::QTV_EVRC_CODEC:
      AudioCodecType = MM_MPEG4_EVRC_CODEC;
      break;

    case QtvPlayer::QTV_QCELP_CODEC:
      AudioCodecType = MM_MPEG4_QCELP_CODEC;
      break;

    case QtvPlayer::QTV_GSM_AMR_CODEC:
      AudioCodecType = MM_MPEG4_GSM_AMR_CODEC;
      break;

#ifdef FEATURE_QTV_3GPP_AMR_WB
#error code not present
#endif /* FEATURE_QTV_3GPP_AMR_WB */
#ifdef FEATURE_QTV_3GPP_AMR_WB_PLUS
#error code not present
#endif /* FEATURE_QTV_3GPP_AMR_WB_PLUS */

#ifdef FEATURE_QTV_3GPP_EVRC_NB
#error code not present
#endif /* FEATURE_QTV_3GPP_EVRC_NB */

#ifdef FEATURE_QTV_3GPP_EVRC_WB
#error code not present
#endif /* FEATURE_QTV_3GPP_EVRC_WB */

#ifdef FEATURE_MP4_MP3
    case QtvPlayer::QTV_MP3_CODEC:
      AudioCodecType = MM_MPEG4_MP3_CODEC;
      break;
#endif /* FEATURE_MP4_MP3 */

#ifdef FEATURE_QTV_WINDOWS_MEDIA
    case QtvPlayer::QTV_WMA_CODEC:
      AudioCodecType = MM_MPEG4_WMA_CODEC;
      break;
#endif /* FEATURE_QTV_WINDOWS_MEDIA */

    case QtvPlayer::QTV_COOK_CODEC:
      AudioCodecType = MM_MPEG4_COOK_CODEC;
      break;
    case QtvPlayer::QTV_SIPR_CODEC:
      AudioCodecType = MM_MPEG4_SIPR_CODEC;
      break;
      
#if defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER)
    case QtvPlayer::QTV_WMA_PRO_CODEC:
      AudioCodecType = MM_MPEG4_WMA_PRO_CODEC;
      break;
    case QtvPlayer::QTV_WMA_PRO_PLUS_CODEC:
      AudioCodecType = MM_MPEG4_WMA_PRO_PLUS_CODEC;
      break;
#endif /* defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER) */
#ifdef FEATURE_QTV_AVI_AC3
#error code not present
#endif /* FEATURE_QTV_AVI_AC3 */
#ifdef FEATURE_QTV_PCM
#error code not present
#endif /* FEATURE_QTV_PCM */

    default:
      AudioCodecType = MM_MPEG4_UNKNOWN_CODEC;
      break;
  }

  return AudioCodecType;
}

/* ======================================================================
FUNCTION
  OEMMediaMPEG42PV_GetVideoCodecType 

DESCRIPTION
  Gets the Qtv codec type of the video track being played and maps it to the 
  appropriate AEE codec type

DEPENDENCIES
  None
   
RETURN VALUE
  AEEMediaMP4CodecType

SIDE EFFECTS
  None

========================================================================== */
static AEEMediaMP4CodecType OEMMediaMPEG42PV_GetVideoCodecType(void)
{
  QtvPlayer::CodecType    QtvVideoCodecType;
  AEEMediaMP4CodecType    VideoCodecType;

  QtvVideoCodecType = QtvPlayer::GetVideoCodecType();

  switch (QtvVideoCodecType)
  {
    case QtvPlayer::QTV_MPEG4_CODEC:
      VideoCodecType = MM_MPEG4_MPEG4_CODEC; 
      break;

    case QtvPlayer::QTV_H263_CODEC:
      VideoCodecType = MM_MPEG4_H263_CODEC;
      break;

    case QtvPlayer::QTV_STILL_IMAGE_CODEC:
      VideoCodecType = MM_MPEG4_STILL_IMAGE_CODEC;
      break;

    case QtvPlayer::QTV_H264_CODEC:
      VideoCodecType = MM_MPEG4_H264_CODEC;
      break;

#ifdef FEATURE_DIVX_311_ENABLE
    case QtvPlayer::QTV_DIVX311_CODEC:
      VideoCodecType = MM_MPEG4_DIVX311_CODEC;
      break;
#endif

#ifdef FEATURE_QTV_WINDOWS_MEDIA

    case QtvPlayer::QTV_WMV1_CODEC:
      VideoCodecType = MM_MPEG4_WMV1_CODEC;
      break;
      
    case QtvPlayer::QTV_WMV2_CODEC:
      VideoCodecType = MM_MPEG4_WMV2_CODEC;
      break;

    case QtvPlayer::QTV_WMV3_CODEC:
      VideoCodecType = MM_MPEG4_WMV3_CODEC;
      break;
#endif /* FEATURE_QTV_WINDOWS_MEDIA */

    case QtvPlayer::QTV_RV30_CODEC:
      VideoCodecType = MM_MPEG4_RV30_CODEC;
      break;
    case QtvPlayer::QTV_RV40_CODEC:
      VideoCodecType = MM_MPEG4_RV40_CODEC;
      break;
      
    default:
      VideoCodecType = MM_MPEG4_UNKNOWN_CODEC;
      break;
  }

  return VideoCodecType;
}

/*!
  @brief
  Set volume
  
  @detail
  Set volume.  Reject if currently either a sound command is pending or a sound 
  command has already been sent and its callback from ISOUND is pending.
  This is an asynchronous request, the result is sent back through ISOUND callback.  

  @return
  Indication of success

  @param pOEM is an OEMHandle.
  @param pVolume is a pointer to SoundCmdParam

  @retval EUNSUPPORTED, if ISOUND is not created. 
  @retval EALREADY, a previous sound command has not yet been completed, 
  either the command is pending or its callback is pending.
  @retval MM_PENDING, the sound command will be or has been sent, result will be sent back through ISOUND callback.
  @retval EFAILED, the sound command fails.
*/

extern "C" int OEMMediaMPEG42PV_SetVolume(OEMHandle pOEM, void * pVolume)
{
  int     ret;

  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);

  // Need to provide valid OEMhandle, return if pOEM is NULL
  if(!pOEM)
  {
    MSG_ERROR("OEMMediaMPEG42PV_SetVolume, pOEM passed in is NULL.", 0, 0, 0);
    return EFAILED;
  }

  if(!pVolume)
  {
    return EFAILED;
  }
  
  if (!pOEM->m_pISound)
    return EUNSUPPORTED;

  // if there is one pending ISOUND command, or we are waiting for the 
  // callback of an ISOUND command that has already been sent, return EALREADY.
  if(pOEM->m_Sound.cmd_pending || pOEM->m_Sound.cb_pending)
  {
    return EALREADY;
  }

  if( !pOEM->m_bClipInfoAvail )
  {
    pOEM->m_Sound.cmd_pending = TRUE;
    pOEM->m_Sound.sound_cmd.cmd = SOUNDCMD_SET_VOLUME;
    pOEM->m_Sound.sound_cmd.param.volume = *((uint16 *)pVolume); 
    return MM_PENDING;
  }

  if(!pOEM->m_Sound.method_set)
  {
    ret = OEMMediaMPEG42PV_SetSoundMethod(pOEM);
    if(ret != SUCCESS)
  {
      return ret;
    }     
  }

  pOEM->m_Sound.cb_pending = TRUE;
  pOEM->m_Sound.sound_cmd.cmd = SOUNDCMD_SET_VOLUME;
  pOEM->m_Sound.sound_cmd.aee_cmd = MM_CMD_SETMEDIAPARM;
  pOEM->m_Sound.sound_cmd.aee_param = MM_PARM_VOLUME;
  pOEM->m_Sound.sound_cmd.param.volume = *((uint16 *)pVolume);

  ISOUND_Get(pOEM->m_pISound, NULL);   // Update the current device settings
  ISOUND_SetVolume(pOEM->m_pISound, pOEM->m_Sound.sound_cmd.param.volume);

  return MM_PENDING;
}

/*!
  @brief
  Mute or Unmute audio

  @detail
  Mute or Unmute audio.  Reject if currently either a sound command is pending or a sound 
  command has already been sent and its callback from ISOUND is pending.  

  @return
  Indication of success

  @param pOEM is an OEMHandle.
  @param pVolume is a pointer to SoundCmdParam

  @retval SUCCESS, current mute is the same as the mute request, no operation is necessary.
  @retval EUNSUPPORTED, if ISOUND is not created. 
  @retval EALREADY, a previous sound command has not yet been completed, 
  either the command is pending or its callback is pending.
  @retval MM_PENDING, the sound command will be or has been sent, result will be sent back through ISOUND callback.
  @retval EFAILED, the sound command fails.
*/
extern "C" int OEMMediaMPEG42PV_SetMuteCtl(OEMHandle pOEM, void * pbMute)
{
  int     ret;
  uint16  volume;

  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);

  // Need to provide valid OEMhandle, return if pOEM is NULL
  if(!pOEM)
  {
    MSG_ERROR("OEMMediaMPEG42PV_SetMuteCtl, pOEM passed in is NULL.", 0, 0, 0);
    return EFAILED;
  }

  if(!pbMute)
  {
    return EFAILED;
  }

  if (!pOEM->m_pISound)
    return EUNSUPPORTED;

  // if there is one pending ISOUND command, or we are waiting for the 
  // callback of an ISOUND command that has already been sent, return EALREADY.
  if(pOEM->m_Sound.cmd_pending || pOEM->m_Sound.cb_pending)
  {
    return EALREADY;
  }

  if( !pOEM->m_bClipInfoAvail )
  {
    pOEM->m_Sound.cmd_pending = TRUE;
    pOEM->m_Sound.sound_cmd.cmd = SOUNDCMD_SET_MUTE;
    pOEM->m_Sound.sound_cmd.param.bMute = *((boolean *)pbMute);
    return MM_PENDING;
  }

  if(!pOEM->m_Sound.method_set)
  {
    ret = OEMMediaMPEG42PV_SetSoundMethod(pOEM);
    if(ret != SUCCESS)
    {
      return ret;
    }     
  }

  if(pOEM->m_Sound.current_mute == *((boolean *)pbMute))
  {
    return SUCCESS;
  }
  
  pOEM->m_Sound.cb_pending = TRUE;
  pOEM->m_Sound.sound_cmd.cmd = SOUNDCMD_SET_MUTE;
  pOEM->m_Sound.sound_cmd.aee_cmd = MM_CMD_SETMEDIAPARM;
  pOEM->m_Sound.sound_cmd.aee_param = MM_PARM_MUTE;
  pOEM->m_Sound.sound_cmd.param.bMute = *((boolean *)pbMute);

  volume = (uint16)(pOEM->m_Sound.sound_cmd.param.bMute ? 0 : pOEM->m_Sound.current_volume);
  ISOUND_Get(pOEM->m_pISound, NULL);   // Update the current device settings
  ISOUND_SetVolume(pOEM->m_pISound, volume);

  return MM_PENDING;
  }

/*!
  @brief
  Get mute
  
  @detail
  Get mute.  Reject if currently either a sound command is pending or a sound 
  command has already been sent and its callback from ISOUND is pending.  

  @return
  Indication of success

  @param pOEM is an OEMHandle.
  @param pVolume is a pointer to SoundCmdParam

  @retval SUCCESS, the sound command succeed and current mute is passed back through pbMute.
  @retval EUNSUPPORTED, if ISOUND is not created. 
  @retval EALREADY, a previous sound command has not yet been completed, 
  either the command is pending or its callback is pending.
  @retval EFAILED, the sound command fails.
*/
extern "C" int OEMMediaMPEG42PV_GetMuteCtl(OEMHandle pOEM, void *pbMute)
{
  int32 *tmp_pbMute = (int32 *)pbMute;

  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);

  // Need to provide valid OEMhandle, return if pOEM is NULL
  if(!pOEM)
    {
      MSG_ERROR("OEMMediaMPEG42PV_GetVolume, pOEM passed in is NULL.", 0, 0, 0);
      return EFAILED;
    }

  if (!pOEM->m_pISound)
    return EUNSUPPORTED;

  // if there is one pending ISOUND command, or we are waiting for the 
  // callback of an ISOUND command that has already been sent, return EALREADY.
  if(pOEM->m_Sound.cmd_pending || pOEM->m_Sound.cb_pending)
  {
    return EALREADY;
  }

  *tmp_pbMute = (int32) pOEM->m_Sound.current_mute;
  return SUCCESS;
}

/*!
  @brief
  Get volume
  
  @detail
  Get volume.  Reject if currently either a sound command is pending or a sound 
  command has already been sent and its callback from ISOUND is pending. 

  @return
  Indication of success

  @param pOEM is an OEMHandle.

  @retval EUNSUPPORTED, if ISOUND is not created. 
  @retval EALREADY, a previous sound command has not yet been completed, 
  either the command is pending or its callback is pending.
  @retval MM_PENDING, the sound command will be or has been sent, result will be sent back through ISOUND callback.
  @retval EFAILED, the sound command fails.
*/

extern "C" int OEMMediaMPEG42PV_GetVolume(OEMHandle pOEM, void *)
{
  int ret;

  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);

  // Need to provide valid OEMhandle, return if pOEM is NULL
  if(!pOEM)
  {
    MSG_ERROR("OEMMediaMPEG42PV_GetVolume, pOEM passed in is NULL.", 0, 0, 0);
    return EFAILED;
  }

  if (!pOEM->m_pISound)
    return EUNSUPPORTED;

  // if there is one pending ISOUND command, or we are waiting for the 
  // callback of an ISOUND command that has already been sent, return EALREADY.
  if(pOEM->m_Sound.cmd_pending || pOEM->m_Sound.cb_pending)
  {
    return EALREADY;
  }

  if( !pOEM->m_bClipInfoAvail )
    {
    pOEM->m_Sound.cmd_pending = TRUE;
    pOEM->m_Sound.sound_cmd.cmd = SOUNDCMD_GET_VOLUME;
    return MM_PENDING;
    }

  if(!pOEM->m_Sound.method_set)
  {
    ret = OEMMediaMPEG42PV_SetSoundMethod(pOEM);
    if(ret != SUCCESS)
    {
      return ret;
    }
  }

  pOEM->m_Sound.cb_pending = TRUE;
  pOEM->m_Sound.sound_cmd.cmd = SOUNDCMD_GET_VOLUME;
  pOEM->m_Sound.sound_cmd.aee_cmd = MM_CMD_GETMEDIAPARM;
  pOEM->m_Sound.sound_cmd.aee_param = MM_PARM_VOLUME;

  ISOUND_Get(pOEM->m_pISound, NULL);   // Update the current device settings
  ISOUND_GetVolume(pOEM->m_pISound);
 
  return MM_PENDING;
}

/*!
  @brief
  Set audio device
  
  @detail
  Set audio device.  Reject if currently either a sound command is pending or a sound 
  command has already been sent and its callback from ISOUND is pending.
  This is an asynchronous request, the result is sent back through ISOUND callback.  

  @return
  Indication of success

  @param pOEM is an OEMHandle.
  @param pVolume is a pointer to SoundCmdParam

  @retval EUNSUPPORTED, if ISOUND is not created. 
  @retval EALREADY, a previous sound command has not yet been completed, 
  either the command is pending or its callback is pending.
  @retval MM_PENDING, the sound command will be or has been sent, result will be sent back through ISOUND callback.
  @retval EFAILED, the sound command fails.
*/

extern "C" int OEMMediaMPEG42PV_SetDevice(OEMHandle pOEM, void * pDevice)
{
  AEESoundInfo    si;
  AEESoundDevice  eDevice;
  int             ret;
  

   pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);

  // Need to provide valid OEMhandle, return if pOEM is NULL
  if(!pOEM)
   {
    MSG_ERROR("OEMMediaMPEG42PV_SetVolume, pOEM passed in is NULL.", 0, 0, 0);
    return EFAILED;
   }

  if(!pDevice)
   {
    return EFAILED;
   }
  
  if (!pOEM->m_pISound)
    return EUNSUPPORTED;

  // if there is one pending ISOUND command, or we are waiting for the 
  // callback of an ISOUND command that has already been sent, return EALREADY.
  if(pOEM->m_Sound.cmd_pending || pOEM->m_Sound.cb_pending)
   {
    return EALREADY;
}

  eDevice = *((AEESoundDevice *)pDevice);

  if( !pOEM->m_bClipInfoAvail )
  {
    pOEM->m_Sound.cmd_pending = TRUE;
    pOEM->m_Sound.sound_cmd.cmd = SOUNDCMD_SET_DEVICE;
    pOEM->m_Sound.sound_cmd.param.volume = eDevice;    
    return MM_PENDING;
  }

  if(!pOEM->m_Sound.method_set)
  {
    ret = OEMMediaMPEG42PV_SetSoundMethod(pOEM);
    if(ret != SUCCESS)
  {
      return ret;
    }     
  }  

  ISOUND_Get(pOEM->m_pISound, &si);
  si.eDevice = eDevice;
  ret = ISOUND_Set(pOEM->m_pISound, &si);
  if(ret != SUCCESS)
  {
    return ret;
  }

  pOEM->m_Sound.cb_pending = TRUE;
  pOEM->m_Sound.sound_cmd.cmd = SOUNDCMD_SET_DEVICE;
  pOEM->m_Sound.sound_cmd.aee_cmd = MM_CMD_SETMEDIAPARM;
  pOEM->m_Sound.sound_cmd.aee_param = MM_PARM_AUDIO_DEVICE;
  pOEM->m_Sound.sound_cmd.param.eDevice = eDevice;
  ISOUND_SetDevice(pOEM->m_pISound);

  return MM_PENDING;
}

#define COPY_STD_SPEC(from, to) \
  (void)UTF8TOWSTR( (uint8*)pClipInfo->ExtInfo.StdInfo.##from, sizeof(pClipInfo->ExtInfo.StdInfo.##from), \
                    MPEG4Spec->ExtendedInfo.StdSpec.##to, sizeof(MPEG4Spec->ExtendedInfo.StdSpec.##to) )

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
#define COPY_PVX_SPEC(from, to) \
  (void)UTF8TOWSTR( (uint8*)pClipInfo->ExtInfo.PvxInfo.StdInfo.##from, sizeof(pClipInfo->ExtInfo.PvxInfo.StdInfo.##from), \
                    MPEG4Spec->ExtendedInfo.PvxSpec.StdSpec.##to, sizeof(MPEG4Spec->ExtendedInfo.PvxSpec.StdSpec.##to) )

#define ASSIGN_PVX_SPEC(from,to) \
  MPEG4Spec->ExtendedInfo.PvxSpec.##to = pClipInfo->ExtInfo.PvxInfo.##from
#endif /*FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2*/

#ifdef FEATURE_MP4_KDDI_META_DATA
#define COPY_KDDI_SPEC(from,to) \
  (void)UTF8TOWSTR( (uint8*)pClipInfo->ExtInfo.KDDIInfo.##from, sizeof(pClipInfo->ExtInfo.KDDIInfo.##from), \
                    MPEG4Spec->ExtendedInfo.KDDISpec.##to, sizeof(MPEG4Spec->ExtendedInfo.KDDISpec.##to) )

#define ASSIGN_KDDI_SPEC(from,to) \
  MPEG4Spec->ExtendedInfo.KDDISpec.##to = pClipInfo->ExtInfo.KDDIInfo.##from

#endif /* FEATURE_MP4_KDDI_META_DATA */

static void CopyMPEG4SpecFromClipInfo(OEMMediaMPEG4Layer * pOEM)
{
  // Need to provide valid OEMhandle, return if pOEM is NULL
  if(!pOEM)
  {
    MSG_ERROR("CopyMPEG4SpecFromClipInfo, pOEM passed in is NULL.", 0, 0, 0);
    return;
  }

  QtvPlayer::ClipInfoT * pClipInfo = (QtvPlayer::ClipInfoT *)pOEM->m_pClipInfo;
  AEEMediaMPEG4Spec * MPEG4Spec = &pOEM->m_MPEG4Spec;

  MEMSET(MPEG4Spec, 0, sizeof(MPEG4Spec));
  MPEG4Spec->streamtype                 = OEMMediaMPEG42PV_ConvertStreamType(
                                                                 pClipInfo->streamtype);
  MPEG4Spec->dwDuration                 = pClipInfo->Duration;
  MPEG4Spec->dwSize                     = pClipInfo->Size;
  MPEG4Spec->dwHeight                   = pClipInfo->Height;
  MPEG4Spec->dwWidth                    = pClipInfo->Width;
  MPEG4Spec->mpeg4Type                  = OEMMediaMPEG42PV_ConvertMediaTypePVToAEE(pClipInfo->MediaType);
  MPEG4Spec->audioType                  = OEMMediaMPEG42PV_GetAudioCodecType(NULL);
  MPEG4Spec->bRepositioningAllowed      = pClipInfo->RepositioningAllowed;
  MPEG4Spec->bAudioOnlyPlaybackAllowed  = pClipInfo->AudioOnlyPlaybackAllowed;
  MPEG4Spec->bVideoOnlyPlaybackAllowed  = pClipInfo->VideoOnlyPlaybackAllowed;

  if(pClipInfo->ExtInfoType == QtvPlayer::CLIP_EXT_INFO_STANDARD)
  {
    MPEG4Spec->eFormatType              = MM_MPEG4_STD_FORMAT_TYPE;

    COPY_STD_SPEC( Title,        szTitle );
    COPY_STD_SPEC( Author,       szAuthor );
    COPY_STD_SPEC( Copyright,    szCopyright );
    COPY_STD_SPEC( Rating,       szRating );
    COPY_STD_SPEC( Description,  szDescription );
    COPY_STD_SPEC( Owner,        szOwner );
    COPY_STD_SPEC( Version,      szVersion );
    COPY_STD_SPEC( CreationDate, szCreationDate );
    COPY_STD_SPEC( Artist,       szArtist );
    COPY_STD_SPEC( sTitle,       szSTitle );
    COPY_STD_SPEC( Album,        szAlbum );
    COPY_STD_SPEC( Info,         szInfo );
    COPY_STD_SPEC( LaunchURL,    szLaunchURL );

    MPEG4Spec->ExtendedInfo.StdSpec.szEndofClipAction = 
      OEMMediaMPEG42PV_ConvertEndofClipActionType(pClipInfo->ExtInfo.StdInfo.EndofClipAction);
  }
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  else if(pClipInfo->ExtInfoType == QtvPlayer::CLIP_EXT_INFO_PVX)
  {
    MPEG4Spec->eFormatType              = MM_PVX_FORMAT_TYPE;

    COPY_PVX_SPEC( Title,        szTitle );
    COPY_PVX_SPEC( Author,       szAuthor );
    COPY_PVX_SPEC( Copyright,    szCopyright );
    COPY_PVX_SPEC( Rating,       szRating );
    COPY_PVX_SPEC( Description,  szDescription );
    COPY_PVX_SPEC( Owner,        szOwner );
    COPY_PVX_SPEC( Version,      szVersion );
    COPY_PVX_SPEC( CreationDate, szCreationDate );
    COPY_PVX_SPEC( Artist,       szArtist );
    COPY_PVX_SPEC( sTitle,       szSTitle );
    COPY_PVX_SPEC( Album,        szAlbum );
    COPY_PVX_SPEC( Info,         szInfo );
    COPY_STD_SPEC( LaunchURL,    szLaunchURL );

    MPEG4Spec->ExtendedInfo.StdSpec.szEndofClipAction = \
        OEMMediaMPEG42PV_ConvertEndofClipActionType(pClipInfo->ExtInfo.StdInfo.EndofClipAction);

    ASSIGN_PVX_SPEC(PurgeDowloadedFile,bPurgeDowloadedFile);
    ASSIGN_PVX_SPEC(AllowUserPlayback,bAllowUserPlayback);
    MPEG4Spec->ExtendedInfo.PvxSpec.downloadPlaybackControl = \
        OEMMediaMPEG42PV_ConvertPlaybackControlType(pClipInfo->ExtInfo.PvxInfo.DownloadPlaybackControl);

  }
#endif 
#ifdef FEATURE_MP4_KDDI_META_DATA
  else if(pClipInfo->ExtInfoType == QtvPlayer::CLIP_EXT_INFO_KDDI)
  {
    MPEG4Spec->eFormatType              = MM_MPEG4_KDDI_FORMAT_TYPE;

    ASSIGN_KDDI_SPEC(CopyProhibitionFlag, dwCopyProhibitionFlag);
    ASSIGN_KDDI_SPEC(ValidityEffectiveDate, dwValidityEffectiveDate);
    ASSIGN_KDDI_SPEC(ValidityPeriod, dwValidityPeriod);
    ASSIGN_KDDI_SPEC(NumberOfAllowedPlayBacks, dwNumberOfAllowedPlayBacks);
    ASSIGN_KDDI_SPEC(AuthorDLLVersion, dwGenerationDLLVersion);
    ASSIGN_KDDI_SPEC(EditFlags, dwEditFlags);
    ASSIGN_KDDI_SPEC(RecordingMode, dwRecordingMode);
    ASSIGN_KDDI_SPEC(RecordingDate, dwRecordingDate);

    COPY_KDDI_SPEC( ContentPropertyTitle,     szTitle );
    COPY_KDDI_SPEC( ContentPropertyCopyRight, szCopyright );
    COPY_KDDI_SPEC( ContentPropertyAuthor,    szAuthor );
    COPY_KDDI_SPEC( ContentPropertyMemo,      szMemo );
    COPY_KDDI_SPEC( DeviceName,               szDeviceName );
    COPY_KDDI_SPEC( ModelName,                szModelName );
    COPY_KDDI_SPEC( EncoderInformation,       szEncoderInformation );
    COPY_KDDI_SPEC( MuxInformation,           szMuxInformation );

    /* GPS related data */
    ASSIGN_KDDI_SPEC(GPSByteOrder, wGPSByteOrder);
    ASSIGN_KDDI_SPEC(GPSVersionID, dwGPSVersionID);
    ASSIGN_KDDI_SPEC(GPSLatitudeRef, dwGPSLatitudeRef);
    ASSIGN_KDDI_SPEC(GPSLongitudeRef, dwGPSLongitudeRef);
    ASSIGN_KDDI_SPEC(GPSAltitudeRef, dwGPSAltitudeRef);

    int i;
    for (i=0; i<QTV_MAX_GPS_LATITUDE; i++)
    {
      ASSIGN_KDDI_SPEC(GPSLatitudeArray[i], GPSLatitudeArray[i]);
    }
    for (i=0; i<QTV_MAX_GPS_LONGITUDE; i++)
    {
      ASSIGN_KDDI_SPEC(GPSLongitudeArray[i], GPSLongitudeArray[i]);
    }
    for (i=0; i<QTV_MAX_GPS_TIME; i++)
    {
      ASSIGN_KDDI_SPEC(GPSTimeArray[i], GPSTimeArray[i]);
    }
    ASSIGN_KDDI_SPEC(GPSAltitude, dwGPSAltitude);
    ASSIGN_KDDI_SPEC(GPSExtensionMapScaleInfo, ddwGPSExtensionMapScaleInfo);

    COPY_KDDI_SPEC( GPSSurveyData,        szGPSSurveyData );
    COPY_KDDI_SPEC( GPSPositioningMethod, szGPSPositioningMethod );
    COPY_KDDI_SPEC( GPSPositioningName,   szGPSPositioningName );
    COPY_KDDI_SPEC( GPSDate,              szGPSDate );
  }
#endif /* FEATURE_MP4_KDDI_META_DATA */
#ifdef FEATURE_QTV_DIVX_DRM
  else if(pClipInfo->ExtInfoType == QtvPlayer::CLIP_EXT_DIVX_DRM_INFO)
  {    
    // Clip is DRM protected, retrieve extended clip specific DRM information.
    MPEG4Spec->eFormatType                                       = MM_DIVX_DRM_FORMAT_TYPE;            
    MPEG4Spec->ExtendedInfo.DivXDrmSpec.nUseLimit                = pClipInfo->ExtInfo.DrmInfo.useLimit;	  
    MPEG4Spec->ExtendedInfo.DivXDrmSpec.nUseCounter              = pClipInfo->ExtInfo.DrmInfo.useCounter;
    MPEG4Spec->ExtendedInfo.DivXDrmSpec.nCgmsaSignal             = pClipInfo->ExtInfo.DrmInfo.cgmsaSignal;
	  MPEG4Spec->ExtendedInfo.DivXDrmSpec.nAcptbSignal             = pClipInfo->ExtInfo.DrmInfo.acptbSignal;
	  MPEG4Spec->ExtendedInfo.DivXDrmSpec.nDigitalProtectionSignal = pClipInfo->ExtInfo.DrmInfo.digitalProtectionSignal;
  }
#endif /* FEATURE_QTV_DIVX_DRM */
}

extern "C" int OEMMediaMPEG42PV_GetMediaSpec(AEEMediaMPEG4Spec **pSpec, int32 *pSize, OEMHandle pOEM)
{
  int nRet = SUCCESS;

  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);
  if (pOEM == NULL)
  {
    MSG_ERROR("OEMMediaMPEG42PV_GetMediaSpec, pOEM is null.", 0, 0, 0);
    return EFAILED;
  }
  if( pOEM->m_bClipInfoAvail )
  {
    *pSpec = &pOEM->m_MPEG4Spec;
    *pSize = sizeof(pOEM->m_MPEG4Spec);
  }
  else
  { /* Media spec is not available right now, so send it back via a callback */
    *pSpec = NULL;
    *pSize = 0;
    nRet = MM_PENDING;
    pOEM->m_cbGenerateMediaSpec = TRUE;
  }

  return nRet;
}

/*==================================================================
   OEMMediaMPEG4_CheckTransform:
   indicate whether the requested transform combination is supported.
==================================================================*/
static int OEMMediaMPEG42PV_CheckTransform(OEMRotationType rotate,
                                           OEMScalingType scale,
#if defined(FEATURE_QTV_MDP_ASCALE) || defined(FEATURE_QTV_XSCALE_VIDEO)
                                           uint16 dx, uint16 dy, OEMHandle pOEM)
#else
                                           uint16, uint16, OEMHandle)
#endif /* FEATURE_QTV_MDP_ASCALE || FEATURE_QTV_XSCALE_VIDEO */
{
/* Rotation macros are the same reguardless of MDP, DSP, or IPL usage. */
  switch (rotate) {
    case OEM_NO_ROTATION: /* 0 */
    case OEM_90_CW_ROTATION: /* 90 */
    case OEM_180_ROTATION: /* 180 */
    case OEM_90_CCW_ROTATION: /* 270 */
      break;
    default:
      return EBADPARM;
  }

/* Scale macros do depend on the scaling mechanism. */
  switch (scale) {
    case OEM_0P5X_SHRINK: /* 50% */
    case OEM_NO_SCALING: /* 100% */
    case OEM_2X_ZOOM: /* 200% */
      return SUCCESS;
#if defined(FEATURE_QTV_MDP_TRANSFORMATIONS) || \
   (defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) && defined(FEATURE_QTV_IPL_SCALING))
    case OEM_0P25X_SHRINK: /* 25% */
    case OEM_0P75X_SHRINK: /* 75% */
      return SUCCESS;
#endif /* MDP_TRANSFORMATIONS || (MP4_FRAME && IPL) */
#if defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) && defined(FEATURE_QTV_IPL_SCALING)
#error code not present
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS || FEATURE_QTV_IPL_SCALING */
#if defined(FEATURE_QTV_MDP_ASCALE) || defined(FEATURE_QTV_XSCALE_VIDEO)
    case OEM_ASCALE:
    case OEM_XSCALE:
      break; /* need further checking to see if desired values are supported */
#endif /* FEATURE_QTV_MDP_ASCALE || FEATURE_QTV_XSCALE_VIDEO */
    default:
      return EBADPARM;
  }

#if defined(FEATURE_QTV_MDP_ASCALE) || defined(FEATURE_QTV_XSCALE_VIDEO)
/* If we are trying to AScale/rotate, make sure the request is within current 
   known bounds with our given rotation/AScaling. */
  if (scale == OEM_ASCALE || scale == OEM_XSCALE) 
  {
    uint16 dx_old, dy_old;
    pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);
    if (pOEM == NULL)
    {
      MSG_ERROR("OEMMediaMPEG42PV_CheckTransform, pOEM is null.", 0, 0, 0);
      return EFAILED;
    }
    QtvPlayer::ClipInfoT *pClipInfo = (QtvPlayer::ClipInfoT*)pOEM->m_pClipInfo;
    if( pOEM->m_bClipInfoAvail && pClipInfo->Width > 0 && pClipInfo->Height > 0)
    {
      dx_old = pClipInfo->Width;
      dy_old = pClipInfo->Height;
    }
    else
    {
      return EBADPARM;
    }
    if (MDP_IS_ROTATED_SIDEWAYS(rotate)) 
    {
      uint16 temp = dx_old;
      dx_old = dy_old;
      dy_old = temp;
    }
    if (((dx*SCALE_DOWN_FACTOR) < dx_old) || (dx > (dx_old*SCALE_UP_FACTOR)) ||
        ((dy*SCALE_DOWN_FACTOR) < dy_old) || (dy > (dy_old*SCALE_UP_FACTOR)) ) 
    {
      return EBADPARM;
    }
    else
    {
      return SUCCESS;
    }
  }
  return EBADPARM;
#endif /* FEATURE_QTV_MDP_ASCALE || FEATURE_QTV_XSCALE_VIDEO */
}


extern "C" int OEMMediaMPEG42PV_RotateVideo(AEEMediaMPEG4RotationType RotationType, OEMHandle pOEM)
{
  int nRet = EBADPARM;
  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);  
  if (pOEM == NULL)
  {
    MSG_ERROR("OEMMediaMPEG42PV_RotateVideo, pOEM is null.", 0, 0, 0);
    return EFAILED;
  }
  nRet = OEMMediaMPEG42PV_CheckTransform((OEMRotationType)RotationType, 
                                         pOEM->m_QtvCurrentScaling, 
#if defined(FEATURE_QTV_MDP_ASCALE) || defined(FEATURE_QTV_XSCALE_VIDEO)
                                         pOEM->m_QtvCurrentAScale.x, 
                                         pOEM->m_QtvCurrentAScale.y,
#else
                                         0,0,
#endif /* FEATURE_QTV_MDP_ASCALE || FEATURE_QTV_XSCALE_VIDEO */
                                         pOEM);

  if (nRet != SUCCESS) 
  {
    return nRet;
  }

#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
#ifdef FEATURE_QTV_XSCALE_VIDEO
  if ( pOEM->m_QtvCurrentScaling == OEM_XSCALE )
  {
    if ( MDP_IS_ROTATED_SIDEWAYS((OEMRotationType)RotationType) )
    {
      nRet = QtvPlayer::ScaleVideo(pOEM->m_QtvCurrentAScale.y, pOEM->m_QtvCurrentAScale.x, pOEM->m_pHandle);
    }
    else
    {
      nRet = QtvPlayer::ScaleVideo(pOEM->m_QtvCurrentAScale.x, pOEM->m_QtvCurrentAScale.y, pOEM->m_pHandle);
    }
    if (nRet != SUCCESS)
    {
      pOEM->m_QtvCurrentScaling = OEM_ASCALE;
      nRet = SUCCESS;
    }
  }
#endif /* FEATURE_QTV_XSCALE_VIDEO */
  pOEM->m_QtvCurrentRotation = (OEMRotationType)RotationType;
  return nRet;


#elif defined( FEATURE_MP4_FRAME_TRANSFORMATIONS ) /* FEATURE_QTV_MDP_TRANSFORMATIONS */
  QtvPlayer::RotationType QtvRotationFactor;
  AEEMediaMP4CodecType audiocodectype, videocodectype;

  #ifdef FEATURE_MP4_AAC_PLUS
  audiocodectype = OEMMediaMPEG42PV_GetAudioCodecType(pOEM);
  videocodectype = OEMMediaMPEG42PV_GetVideoCodecType();

  /* If MPEG4-AAC+ DSP image is downloaded, use Qtv IPL for rotation & color conversion on ARM */
  if(qtv_cfg_enable_aacplus && 
     audiocodectype == MM_MPEG4_AAC_CODEC && 
     (videocodectype == MM_MPEG4_MPEG4_CODEC || 
      (videocodectype == MM_MPEG4_H263_CODEC && !qtv_cfg_enable_h263_profile3)) && 
     (pOEM->m_ClipInfo.Height <= MP4_QCIF_HEIGHT) && (pOEM->m_ClipInfo.Width <= MP4_QCIF_WIDTH))
  {
    pOEM->m_IPLRotationFactor = RotationType;
    return SUCCESS;
  } 
  else /* Use DSP for rotation and color conversion */
#endif /* FEATURE_MP4_AAC_PLUS */
  {
  if(pOEM->m_bFrameTransformPending)
  {
    MSG_ERROR("Transformation pending.",0,0,0);
    return EALREADY;
  }

  switch(RotationType)
  {
    case MM_MPEG4_90_CW_ROTATION:
        QtvRotationFactor = QtvPlayer::ROTATE_90_CW;
      break;
    case MM_MPEG4_90_CCW_ROTATION:
        QtvRotationFactor = QtvPlayer::ROTATE_90_CCW;
      break;
    case MM_MPEG4_180_ROTATION:
        QtvRotationFactor = QtvPlayer::ROTATE_180;
      break;
    case MM_MPEG4_NO_ROTATION:
        QtvRotationFactor = QtvPlayer::ROTATE_NO;
      break;
    default:
      /* refuse to apply an unsupported rotation value */
      return EBADPARM;
  }
  }

  nRet = QtvPlayer::RotateVideo(QtvRotationFactor, pOEM->m_pHandle);
  if(nRet == QtvPlayer::QTV_RETURN_OK)
  {
    pOEM->m_bFrameTransformPending = TRUE;
    // Callback function will be notified
    return MM_PENDING;
  }
  return EFAILED;
#else /* FEATURE_MP4_FRAME_TRANSFORMATIONS */
  return nRet;
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */
}

/* dx, dy are for use as Ascaling requests */
extern "C" int OEMMediaMPEG42PV_ScaleVideo(AEEMediaMPEG4ScalingType ScaleFactor, 
                                           uint16 dx, uint16 dy, OEMHandle pOEM)
{
  int nRet = EBADPARM;
  int32 width =0, height = 0;  
  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);
  if (pOEM == NULL)
  {
    MSG_ERROR("OEMMediaMPEG42PV_ScaleVideo, pOEM is null.", 0, 0, 0);
    return EFAILED;
  }
  pOEM->m_bXscaleVideo = FALSE;
  pOEM->m_bMDPScale = FALSE;
  nRet = OEMMediaMPEG42PV_CheckTransform(pOEM->m_QtvCurrentRotation, 
                                         (OEMScalingType)ScaleFactor, 
                                         dx, dy, pOEM);
  if (nRet != SUCCESS) 
  {
    return nRet;
  }
  QtvPlayer::ClipInfoT *pClipInfo = (QtvPlayer::ClipInfoT*)pOEM->m_pClipInfo;
  if (ScaleFactor == MM_MPEG4_2P5X_ZOOM)
  {
    dx = width = (pClipInfo->Width*2.5);
    dy = height = (pClipInfo->Height*2.5);        
  }
  else if (ScaleFactor== MM_MPEG4_2X_ZOOM)
  {
    dx = width = (pClipInfo->Width*2);
    dy = height = (pClipInfo->Height*2);    
  }
  else if (ScaleFactor == MM_MPEG4_1P25X_ZOOM)
  {
    dx = width = pClipInfo->Width*1.25;
    dy = height = pClipInfo->Height*1.25;
  }  
  else if (ScaleFactor == MM_MPEG4_NO_SCALING)
  {
    dx = width = pClipInfo->Width;
    dy = height = pClipInfo->Height;
  }
  else if (ScaleFactor == MM_MPEG4_0P25X_SHRINK)
  {
    dx = width = pClipInfo->Width*0.25;
    dy = height = pClipInfo->Height*0.25;	
  }
  else if(ScaleFactor == MM_MPEG4_2X_SHRINK)
  {
    dx = width = (pClipInfo->Width*0.5);
    dy = height = (pClipInfo->Height*0.5); 
  }
  else if(ScaleFactor == MM_MPEG4_0P75X_SHRINK)
  {
    dx = width = (pClipInfo->Width*0.75);
    dy = height = (pClipInfo->Height*0.75);		
  }
  else
    MSG_HIGH("Scale factor: %d, width=%d and height=%d ",ScaleFactor,width,height);
  

  
#ifdef FEATURE_QTV_XSCALE_VIDEO
// If we no longer want XScaling, we need to turn it off.
  pOEM->m_bXscaleVideo = TRUE;

if (pOEM->m_QtvCurrentScaling == OEM_XSCALE &&(ScaleFactor != MM_MPEG4_ASCALE) && 
                                              (ScaleFactor != MM_MPEG4_2P5X_ZOOM) && 
                                              (ScaleFactor != MM_MPEG4_2X_ZOOM) &&
                                              (ScaleFactor != MM_MPEG4_0P25X_SHRINK))
{
  nRet = QtvPlayer::ScaleVideo(width, height, pOEM->m_pHandle);
}

#endif /* FEATURE_QTV_XSCALE_VIDEO */




#if defined(FEATURE_QTV_MDP_TRANSFORMATIONS) 
  pOEM->m_bMDPScale = TRUE;
#endif /* FEATURE_QTV_MDP_TRANSFORMATIONS */

// If no arbitrary scaling is involved, we're done after this. 
  if ((ScaleFactor != MM_MPEG4_ASCALE) && 
      (ScaleFactor != MM_MPEG4_2P5X_ZOOM) && 
      (ScaleFactor != MM_MPEG4_2X_ZOOM) &&
      (ScaleFactor != MM_MPEG4_0P25X_SHRINK))
  {
    pOEM->m_QtvCurrentScaling = (OEMScalingType)ScaleFactor;    
    return SUCCESS;
  }

#if defined(FEATURE_QTV_MDP_ASCALE) || defined(FEATURE_QTV_XSCALE_VIDEO)
  pOEM->m_QtvCurrentAScale.x = dx;
  pOEM->m_QtvCurrentAScale.y = dy;
  nRet = EFAILED;
#ifdef FEATURE_QTV_XSCALE_VIDEO
  if (MDP_IS_ROTATED_SIDEWAYS(pOEM->m_QtvCurrentRotation)) 
  {
    nRet = QtvPlayer::ScaleVideo(dy, dx, pOEM->m_pHandle);
  }
  else if ((ScaleFactor != MM_MPEG4_ASCALE) && 
           (ScaleFactor != MM_MPEG4_2P5X_ZOOM) &&
           (ScaleFactor != MM_MPEG4_2X_ZOOM) &&
           (ScaleFactor != MM_MPEG4_0P25X_SHRINK))
  {
  nRet = QtvPlayer::ScaleVideo(dx, dy, pOEM->m_pHandle);
  }
#endif /* FEATURE_QTV_XSCALE_VIDEO */
  if (nRet == SUCCESS) 
  {
    pOEM->m_QtvCurrentScaling = OEM_XSCALE; /* don't want the MDP to try scaling */
    nRet = MM_PENDING;
  }
#ifdef FEATURE_QTV_MDP_ASCALE
  else /* only reachable if the *only* scaling is MDP AScaling */
  {
    pOEM->m_QtvCurrentScaling = OEM_ASCALE;
    nRet = SUCCESS;  
  }
#endif /* FEATURE_QTV_MDP_ASCALE */
#endif /* FEATURE_QTV_MDP_TRANSFORMATIONS || FEATURE_QTV_XSCALE_VIDEO */

#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
#ifdef FEATURE_QTV_IPL_SCALING /* IPL Scaling using CPU */
#error code not present
#else /* !FEATURE_QTV_IPL_SCALING (Scaling using DSP ) */
  QtvPlayer::ScalingType type = QtvPlayer::SCALE_NONE;

  if(pOEM->m_bFrameTransformPending)
  {
    MSG_ERROR("Transformation pending.",0,0,0);
    return EALREADY;
  }
  switch(ScaleFactor)
  {
    case MM_MPEG4_2X_ZOOM:
      type = QtvPlayer::SCALE_2X_ZOOM;
      break;

    case MM_MPEG4_NO_SCALING:
      type = QtvPlayer::SCALE_NONE;
      break;

    case MM_MPEG4_2X_SHRINK:
      type = QtvPlayer::SCALE_2X_SHRINK;
      break;

    default:
      /* refuse to apply unsupported DSP scale factor */
      return EBADPARM;
  }
  nRet = QtvPlayer::ScaleVideo(type, pOEM->m_pHandle);
  if(nRet == QtvPlayer::QTV_RETURN_OK)
  {
    pOEM->m_bFrameTransformPending = TRUE;
    // Callback function will be notified
    return MM_PENDING;
  }
  return EFAILED;
#endif /* FEATURE_QTV_IPL_SCALING */
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */
  return nRet;
}

#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
extern "C" int OEMMediaMPEG42PV_GetTelopSubString(AEETelopTextSubString **ppTextSubStr, 
                                                  int32 nIndex, OEMHandle pOEM)
{
  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);
  if(pOEM == NULL)
  {
    MSG_ERROR("OEMMediaMPEG42PV_GetTelopSubString, pOEM is null.", 0, 0, 0);
    return EFAILED;
  }
  QtvPlayer::TelopSubStringT PVTelopSubStr;
  if(QtvPlayer::GetTelopSubString(&PVTelopSubStr, nIndex))
  {
    pOEM->m_TelopTextSubString.bLineFeed          = PVTelopSubStr.LineFeed;
    pOEM->m_TelopTextSubString.bLinking           = PVTelopSubStr.Linking;
    pOEM->m_TelopTextSubString.bReversal          = PVTelopSubStr.Reversal;
    pOEM->m_TelopTextSubString.bUnderline         = PVTelopSubStr.Underline;
    pOEM->m_TelopTextSubString.dwFontColor        = PVTelopSubStr.FontColor;
    pOEM->m_TelopTextSubString.dwLinkSize         = PVTelopSubStr.LinkSize;
    pOEM->m_TelopTextSubString.dwTextSampleSize   = PVTelopSubStr.TextSampleSize;
    pOEM->m_TelopTextSubString.pLinkValue         = PVTelopSubStr.LinkValue;
    pOEM->m_TelopTextSubString.pTextSubString     = PVTelopSubStr.TextSubString;
    *ppTextSubStr = &pOEM->m_TelopTextSubString;
  }
  else
  {
    MEMSET(&pOEM->m_TelopTextSubString, 0, sizeof(AEETelopTextSubString));
    *ppTextSubStr = NULL;
  }
  return SUCCESS;
}
#endif /* FEATURE_MP4_KDDI_TELOP_TEXT */

#ifdef FEATURE_VIDEO_AUDIO_DUBBING
extern "C" int OEMMediaMPEG42PV_SetAudDub(uint16 audioFormat)
{
   QtvPlayer::AudOverrideType audformat=QtvPlayer::AUD_OVER_ENC_QCELP13K;
   QtvPlayer::ReturnT retVal;

   switch (audioFormat)
   {
     case MM_QCP_FORMAT_FIXED_FULL_13K:
     case MM_QCP_FORMAT_FIXED_HALF_13K:
       audformat = QtvPlayer::AUD_OVER_ENC_QCELP13K;
       break;

     case MM_QCP_FORMAT_FIXED_FULL_EVRC:
       audformat = QtvPlayer::AUD_OVER_ENC_EVRC;
       break;

#ifdef FEATURE_MP4_AMR
     case MM_QCP_FORMAT_FIXED_12_2_AMR:
       audformat = QtvPlayer::AUD_OVER_ENC_AMR;
       break;
#endif /* FEATURE_MP4_AMR */

     case MM_QCP_FORMAT_UNKNOWN:
       return EFAILED;
   }

   retVal = QtvPlayer::SetAudOverride(audformat);
   if (retVal == QtvPlayer::QTV_RETURN_OK)
   {
      return SUCCESS;
   }
   else
   {
      return EFAILED;
   }
}
#endif /* FEATURE_VIDEO_AUDIO_DUBBING */

extern "C" int OEMMediaMPEG42PV_SetAudOverride(int32 audioFormat)
{
   QtvPlayer::AudOverrideType audformat=QtvPlayer::AUD_OVER_NONE;
   QtvPlayer::ReturnT retVal;

   switch (audioFormat)
   {
     case AEECLSID_MEDIAAAC:
       audformat = QtvPlayer::AUD_OVER_DEC_AAC;
       break;

     case AEECLSID_MEDIAMP3:
       audformat = QtvPlayer::AUD_OVER_DEC_MP3;
       break;

     case AEECLSID_MEDIAQCP:
       audformat = QtvPlayer::AUD_OVER_DEC_QCP;
       break;

#ifdef FEATURE_MP4_AMR
     case AEECLSID_MEDIAAMR:
       audformat = QtvPlayer::AUD_OVER_DEC_AMR;
       break;
#endif /* FEATURE_MP4_AMR */

     default:
       return EFAILED;
   }

   retVal = QtvPlayer::SetAudOverride(audformat);
   if (retVal == QtvPlayer::QTV_RETURN_OK)
   {
      return SUCCESS;
   }
   else
   {
      return EFAILED;
   }
}

/* <EJECT> */
/*===========================================================================
Function: AllocateOutputBuffer

Description: Called to allocate the memory for output buffers

Return Value:
  TRUE  - if buffers were allocated successfully
  FALSE - if an error occurred

Comments:

Side Effects:

============================================================================*/
extern void *AllocateOutputBuffer(const VDEC_STREAM_ID streamID,void *pCbData, 
                                  uint32 bytes)
{
  void *p = NULL;
  (void)streamID;     //unused currently
  (void)pCbData;      
#ifdef FEATURE_QTV_OEM_BUFFER_MGR
  OEMOutputBufferType *q = (OEMOutputBufferType*)MALLOC(sizeof(OEMOutputBufferType));
  OEMHandle pOEM = OEMMediaMPEG42PV_GetOEMLayer(NULL);
  if (q && pOEM) 
  {
#endif /* FEATURE_QTV_OEM_BUFFER_MGR */
#ifdef PLATFORM_LTK
#error code not present
#elif defined(T_MSM7500)
#error code not present
#else
  p =  MALLOC(bytes);
#endif /* T_MSM7500 */
#ifdef FEATURE_QTV_OEM_BUFFER_MGR
  }

  if (p) 
  {
    q->nRefCnt = 1;
    q->pOutputBuffer = p;
    q_link((void*)q, &(q->qLink));
    q_put(&(pOEM->m_qOutputBuffer), &(q->qLink));
  }
#endif /* FEATURE_QTV_OEM_BUFFER_MGR */
  return p;
}

/* <EJECT> */
/*===========================================================================
Function: FreeOutputBuffer

Description:  Called to free the output buffers

Return Value:

Comments:

Side Effects:

============================================================================*/
extern void FreeOutputBuffer(const VDEC_STREAM_ID streamID,void *pCbData, void *pBuf)
{
  (void)streamID;     //unused currently
  (void)pCbData; 
  if( pBuf != NULL )
  {
#ifdef PLATFORM_LTK
#error code not present
#elif defined(T_MSM7500)
#error code not present
#else
    FREE(pBuf);
#endif /* T_MSM7500 */
  }
}
#ifdef FEATURE_QTV_OEM_BUFFER_MGR
/*==========================================================================*/
/*!
@brief
  Finds the structure in our buffer queue which is based upon pBuf.

@param[in] pBuf - Buffer pointer we must find in queued OEMOutputBufferType structs.
@param[in] pOEM - Current OEM Layer handle.

@return 
  Pointer to matching OEMOutputBufferType.  
  NULL if failure.
============================================================================*/
OEMOutputBufferType* GetOutputBufferType(void* pBuf, OEMHandle pOEM)
{
  OEMOutputBufferType* ret = (OEMOutputBufferType*)q_check(&(pOEM->m_qOutputBuffer));
  while (ret)
  {
    if (ret->pOutputBuffer == pBuf) 
    {
      break;
    }
    ret = (OEMOutputBufferType*)q_next(&(pOEM->m_qOutputBuffer), &(ret->qLink));
  }
  return ret;
}

/*==========================================================================*/
/*!
@brief
  Adds a reference to the structure in our buffer queue which is based upon pBuf.

@param[in] pBuf - Buffer pointer we wish to reference.
============================================================================*/
static void AddRefOutputBuffer(void* pBuf)
{
  OEMOutputBufferType* pBufType;
  OEMHandle pOEM = OEMMediaMPEG42PV_GetOEMLayer(NULL);
  if (pOEM) 
  {
    pBufType = GetOutputBufferType(pBuf, pOEM);
    if (pBufType)
    {
      pBufType->nRefCnt++;
    }
  }
}

/*==========================================================================*/
/*!
@brief
  Releases the structure in our buffer queue which is based upon pBuf.
@detail
  If no one has a reference to the buffer anymore (nRefCnt would be 0 when
  this call completes), the buffer and its associated structure are freed.

@param[in] pBuf - Buffer pointer we wish to release.
============================================================================*/
extern void ReleaseOutputBuffer(const VDEC_STREAM_ID streamID,void *pCbData, void *pBuf)
{
  OEMOutputBufferType* pBufType;
  OEMHandle pOEM = OEMMediaMPEG42PV_GetOEMLayer(NULL);
  if (pOEM) 
  {
    pBufType = GetOutputBufferType(pBuf, pOEM);
    if (pBufType)
    {
      if (pBufType->nRefCnt == 1)  
      {
        q_delete(&(pOEM->m_qOutputBuffer), &(pBufType->qLink));
        FreeOutputBuffer(streamID,
                         pCbData,
                         pBufType->pOutputBuffer);
        FREE(pBufType);
      }
      else
      {
        pBufType->nRefCnt--;
      }
    }
  }
}
#endif /* FEATURE_QTV_OEM_BUFFER_MGR */

/* gets the time scale of the given track */
/* returns FALSE, if track is not supported or error */
/* time scale may be zero, if any supported track is not present in the media */
extern "C" int OEMMediaMPEG42PV_GetTrackTimeScale(AEEMediaMPEG4TrackType trackType, uint32 *pTimeScale)
{
  QtvPlayer::ReturnT retVal;
  int retIntVal;
  switch(trackType)
  {
    case VIDEO_TRACK:
       retVal = QtvPlayer::GetTrackTimeScale(QtvPlayer::VideoTrack, pTimeScale);
      break;
    case AUDIO_TRACK:
       retVal = QtvPlayer::GetTrackTimeScale(QtvPlayer::AudioTrack, pTimeScale);
      break;
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    case TEXT_TRACK:
       retVal = QtvPlayer::GetTrackTimeScale(QtvPlayer::TextTrack, pTimeScale);
      break;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
    default:
      retVal = QtvPlayer::QTV_RETURN_ERROR;
      break;
  };

  if (retVal==QtvPlayer::QTV_RETURN_OK)
	retIntVal = SUCCESS;
  else if ((retVal==QtvPlayer::QTV_RETURN_DATA_NOT_AVAILABLE)||
           (retVal==QtvPlayer::QTV_RETURN_FEATURE_NOT_AVAILABLE))
	retIntVal = EUNSUPPORTED;
  else
    retIntVal = EFAILED;

  return retIntVal;
}

extern "C" int OEMMediaMPEG42PV_SetLoopTrack(boolean bLoop, OEMHandle pOEM)
{
  QtvPlayer::ReturnT retVal;
  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);
  if (pOEM == NULL)
  {
    MSG_ERROR("OEMMediaMPEG42PV_SetLoopTrack, pOEM is null.", 0, 0, 0);
    return EFAILED;
  }
  retVal = QtvPlayer::SetLoopTrack((bool)(bLoop==TRUE), pOEM->m_pHandle);
  if(retVal == QtvPlayer::QTV_RETURN_OK)
    return SUCCESS;
  else
    return EFAILED;
}

extern "C" int OEMMediaMPEG42PV_GetPlaybackPos(AEEMediaPlaybackPos * pPlayPos, OEMHandle pOEM)
{
  QtvPlayer::PlayerStateRecordT state = {0};
  QtvPlayer::ReturnT retVal = QtvPlayer::QTV_RETURN_ERROR;
  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);
  if (pOEM == NULL)
  {
    MSG_ERROR("OEMMediaMPEG42PV_GetPlaybackPos, pOEM is null.", 0, 0, 0);
    return EFAILED;
  }
  retVal = QtvPlayer::GetPlayerState(state, pOEM->m_pHandle);

  if(retVal == QtvPlayer::QTV_RETURN_OK)
  {
    pPlayPos->dwAudioPos = state.audioPlaybackMsec;
    pPlayPos->dwVideoPos = state.videoPlaybackMsec;
    pPlayPos->dwPlaybackPos = MAX(state.audioPlaybackMsec, state.videoPlaybackMsec);
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    pPlayPos->dwTextPos = state.textPlaybackMsec;
    pPlayPos->dwPlaybackPos = MAX(pPlayPos->dwPlaybackPos, state.textPlaybackMsec);
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

    return SUCCESS;
  }
  else
    return EFAILED;
}

#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */

extern "C" int OEMMediaMPEG42PV_SetMediaInfo(void  *media_info, uint32 size, 
                                             OEMHandle pOEM)
{
  QtvPlayer::ReturnT retVal = QtvPlayer::QTV_RETURN_ERROR;
  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);

  if(pOEM == NULL)
  {
    MSG_ERROR("OEMMediaMPEG42PV_SetMediaInfo, pOEM is null.", 0, 0, 0);
    return EFAILED;
  }
  /*-------------------------------------------------------------------------
    No translation needed, the structs are the same
  -------------------------------------------------------------------------*/
  retVal = QtvPlayer::SetMediaInfo(media_info, size, pOEM->m_pHandle);

  if(retVal == QtvPlayer::QTV_RETURN_OK)
    return SUCCESS;

  return EFAILED;
}


#ifdef FEATURE_QTV_STREAM_RECORD
extern "C" int OEMMediaMPEG42PV_RecordStream(AEEStreamRecordInfo* recInfo, OEMHandle pOEM)
{
  char* pTempFilePath = NULL;
  char* recFile = NULL;
  int nLen = 0;
  const char* RelativePathStartTag = "fs:/~";
  boolean allocateRecFile = FALSE;

  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);

  // return if pOEM is NULL
  if(pOEM == NULL)
  {
    MSG_ERROR("OEMMediaMPEG42PV_RecordStream, pOEM is null.", 0, 0, 0);
    return EFAILED;
  }

  /* if we has not started playing yet, return bad state.
  */
  if(pOEM->m_nMPEG4EngineState == QtvPlayer::QTV_PLAYER_STATUS_IDLE)
  {
    return EBADSTATE;
  }

//  bEnableStreamRecord = TRUE;
  aee_strlcpy(fName, recInfo->filename, 512);
  bRecFileOverWrite = (recInfo->overwrite)?true:false;

  // if file path starts with "fs:/~", it is a relative path and needs to resolve.
  if(!strncmp(RelativePathStartTag,(char *)(fName),strlen(RelativePathStartTag)))
  {
     allocateRecFile = TRUE;
     if(SUCCESS == AEE_ResolvePath( fName, 0, &nLen))
     {
       pTempFilePath = (char*)malloc(nLen);
       if(pTempFilePath)
       {
         if(SUCCESS == AEE_ResolvePath( fName, pTempFilePath, &nLen) )
         {
           #if MIN_BREW_VERSION(4,0)
             if (SUCCESS == OEMefs_GetNativePath(pTempFilePath, 0, 0, &nLen)) 
           #else
             if (SUCCESS == OEMFS_GetNativePath(pTempFilePath,0, &nLen)) 
           #endif
           {
             recFile = (char*)malloc(nLen);
             if(recFile)
             {
               #if MIN_BREW_VERSION(4,0)
                 OEMefs_GetNativePath(pTempFilePath,recFile, nLen, &nLen); 
               #else
                 OEMFS_GetNativePath(pTempFilePath,recFile, &nLen);        
               #endif
             }
           }
           if(pTempFilePath)
           {
             free(pTempFilePath);
             pTempFilePath = NULL;
           }            
         }
       }
     }
  }
  else
  {
     recFile = (char *)fName;
  }

  if (recFile && pOEM)
  {
    QtvPlayer::RecordClip( recFile,
                           ( (bRecFileOverWrite)?true:false ),
                           0xFF, /* mode is ignored */
                           0,
                           pOEM->m_pHandle);   /* duration is ignored */
  }

//  bEnableStreamRecord = FALSE;
  bRecFileOverWrite = FALSE;
  if((allocateRecFile == TRUE)&&recFile)
  {
    free (recFile);
    recFile = NULL;
  }  
  return SUCCESS;
}

extern "C" int OEMMediaMPEG42PV_StopStreamRecording(void)
{
  QtvPlayer::ReturnT retVal = QtvPlayer::QTV_RETURN_ERROR;
  retVal = QtvPlayer::StopClipRecording();
  return retVal;
}

#endif /* FEATURE_QTV_STREAM_RECORD */

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
extern "C" int OEMMediaMPEG42PV_CopyBufferToFile(char *fname, boolean ow)
{
   return QtvPlayer::CopyBufferToFile(fname, ((ow)?true:false) );
}

extern "C" int OEMMediaMPEG42PV_SetPvxSaveOption(char *dirPath)
{
    if(QtvPlayer::SetFileSaveOption(dirPath))
        return SUCCESS;
    else
        return EFAILED;
}

extern "C" int OEMMediaMPEG42PV_SetPvxPreroll(int preRoll)
{
    if(QtvPlayer::SetPvxPreroll(preRoll))
        return SUCCESS;
    else
        return EFAILED;
}
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */


/* gets video track I-Frame (Sync Frame) which is closest to the given time */
extern "C" boolean OEMMediaMPEG42PV_GetSyncFrameInfo(uint32 timeStamp, bool reverse, AEEDataFrameInfo * pFrameInfo)
{
  QtvPlayer::RawFrameInfo frameInfo;
  QtvPlayer::ReturnT retVal;
  retVal = QtvPlayer::GetRawFrameInfo(QtvPlayer::VideoTrack, timeStamp, &frameInfo);
  if (retVal == QtvPlayer::QTV_RETURN_OK)
  {
    retVal = QtvPlayer::GetNextRawSyncFrameInfo(frameInfo.FrameNum, reverse, &frameInfo);
    if (retVal == QtvPlayer::QTV_RETURN_OK)
    {
      pFrameInfo->FrameNum = frameInfo.FrameNum;       //Frame Number
      pFrameInfo->FrameSize= frameInfo.FrameSize;
      pFrameInfo->FrameOffset = frameInfo.FrameOffset;
      pFrameInfo->FrameTime = frameInfo.FrameTime;
      pFrameInfo->IsSyncFrame = frameInfo.IsSyncFrame;
      pFrameInfo->Framedelta = frameInfo.Framedelta;
      return TRUE;
    }
  }
  return FALSE;
}

/* gets video track Frame information which is closest to the given time */
extern "C" bool OEMMediaMPEG42PV_GetVideoFrameInfo(uint32 timeStamp,  AEEDataFrameInfo * pFrameInfo)
{
  QtvPlayer::RawFrameInfo frameInfo;
  QtvPlayer::ReturnT retVal;
  retVal = QtvPlayer::GetRawFrameInfo(QtvPlayer::VideoTrack, timeStamp, &frameInfo);
  if (retVal == QtvPlayer::QTV_RETURN_OK)
  {
    pFrameInfo->FrameNum = frameInfo.FrameNum;       //Frame Number
    pFrameInfo->FrameSize= frameInfo.FrameSize;
    pFrameInfo->FrameOffset = frameInfo.FrameOffset;
    pFrameInfo->FrameTime = frameInfo.FrameTime;
    pFrameInfo->IsSyncFrame = frameInfo.IsSyncFrame;
    pFrameInfo->Framedelta = frameInfo.Framedelta;
    return TRUE;
  }
  return FALSE;
}

/* gets audio track Frame information which is closest to the given time */
extern "C" bool OEMMediaMPEG42PV_GetAudioFrameInfo(uint32 timeStamp,  AEEDataFrameInfo * pFrameInfo)
{
  QtvPlayer::RawFrameInfo frameInfo;
  QtvPlayer::ReturnT retVal;
  retVal = QtvPlayer::GetRawFrameInfo(QtvPlayer::AudioTrack, timeStamp, &frameInfo);
  if (retVal == QtvPlayer::QTV_RETURN_OK)
  {
    pFrameInfo->FrameNum = frameInfo.FrameNum;       //Frame Number
    pFrameInfo->FrameSize= frameInfo.FrameSize;
    pFrameInfo->FrameOffset = frameInfo.FrameOffset;
    pFrameInfo->FrameTime = frameInfo.FrameTime;
    pFrameInfo->IsSyncFrame = frameInfo.IsSyncFrame;
    pFrameInfo->Framedelta = frameInfo.Framedelta;
    return TRUE;
  }
  return FALSE;
}

extern "C" uint32 OEMMediaMPEG42PV_GetMaxAudioFrameSize(void)
{
  uint32 FrameSize = 0;
  QtvPlayer::GetLargestRawFrameSize(QtvPlayer::AudioTrack, &FrameSize);
  return FrameSize;
}

extern "C" uint32 OEMMediaMPEG42PV_GetMaxVideoFrameSize(void)
{
  uint32 FrameSize = 0;
  QtvPlayer::GetLargestRawFrameSize(QtvPlayer::VideoTrack, &FrameSize);
  return FrameSize;
}

/* gets raw audio track header. if buffer is not sufficient, it will return
   FALSE and "pBufSize" will have header size. If return value of "pBufSize"
   is zero, then function failed. */
extern "C" bool OEMMediaMPEG42PV_GetAudioTrackHeader(byte * pBuf, uint32 *pBufSize)
{
  QtvPlayer::ReturnT retVal;
  retVal = QtvPlayer::GetRawTrackHeader(QtvPlayer::AudioTrack, pBuf, pBufSize);
  if (retVal == QtvPlayer::QTV_RETURN_OK)
  {
    return TRUE;
  }
  else if(retVal == QtvPlayer::QTV_RETURN_ERROR)
  {
    *pBufSize = 0;
  }
  return FALSE;
}

/* gets raw video track header. if buffer is not sufficient, it will return
   FALSE and "pBufSize" will have header size. If return value of "pBufSize"
   is zero, then function failed. */
extern "C" bool OEMMediaMPEG42PV_GetVideoTrackHeader(byte * pBuf, uint32 *pBufSize)
{
  QtvPlayer::ReturnT retVal;
  retVal = QtvPlayer::GetRawTrackHeader(QtvPlayer::VideoTrack, pBuf, pBufSize);
  if (retVal == QtvPlayer::QTV_RETURN_OK)
  {
    return TRUE;
  }
  else if(retVal == QtvPlayer::QTV_RETURN_ERROR)
  {
    *pBufSize = 0;
  }
  return FALSE;
}

/* gets number of frame in audio track. In case of error it
   returns zero, otherwise number of frames. */
extern "C" uint32 OEMMediaMPEG42PV_GetNumAudioFrame(void)
{
  QtvPlayer::ReturnT retVal;
  uint32 NumFrames;
  retVal = QtvPlayer::GetNumFrames(QtvPlayer::AudioTrack, &NumFrames);
  if (retVal == QtvPlayer::QTV_RETURN_OK)
  {
    return NumFrames;
  }
  return 0;
}

/* gets number of frame in video track. In case of error it
   returns zero, otherwise number of frames. */
extern "C" uint32 OEMMediaMPEG42PV_GetNumVideoFrame(void)
{
  QtvPlayer::ReturnT retVal;
  uint32 NumFrames;
  retVal = QtvPlayer::GetNumFrames(QtvPlayer::VideoTrack, &NumFrames);
  if (retVal == QtvPlayer::QTV_RETURN_OK)
  {
    return NumFrames;
  }
  return 0;
}

extern "C" bool OEMMediaMPEG42PV_GetAudioFrame( uint32 FrameNum,
                                                AEEDataFrameInfo * pFrameInfo,
                                                byte * pBuf,
                                                uint32 bufSize  )
{
  QtvPlayer::RawFrameInfo frameInfo;
  QtvPlayer::ReturnT retVal;
  retVal = QtvPlayer::GetRawFrame(QtvPlayer::AudioTrack, FrameNum, &frameInfo, pBuf, bufSize);
  if (retVal == QtvPlayer::QTV_RETURN_OK)
  {
    pFrameInfo->FrameNum = frameInfo.FrameNum;       //Frame Number
    pFrameInfo->FrameSize= frameInfo.FrameSize;
    pFrameInfo->FrameOffset = frameInfo.FrameOffset;
    pFrameInfo->FrameTime = frameInfo.FrameTime;
    pFrameInfo->Framedelta = frameInfo.Framedelta;
    pFrameInfo->IsSyncFrame = frameInfo.IsSyncFrame;
    return TRUE;
  }
  return FALSE;
}


extern "C" bool OEMMediaMPEG42PV_GetVideoFrame( uint32 FrameNum,
                                                AEEDataFrameInfo * pFrameInfo,
                                                byte *pBuf,
                                                uint32 bufSize  )
{
  QtvPlayer::RawFrameInfo frameInfo;
  QtvPlayer::ReturnT retVal;
  retVal = QtvPlayer::GetRawFrame(QtvPlayer::VideoTrack, FrameNum, &frameInfo, pBuf, bufSize);
  if (retVal == QtvPlayer::QTV_RETURN_OK)
  {
    pFrameInfo->FrameNum = frameInfo.FrameNum;       //Frame Number
    pFrameInfo->FrameSize= frameInfo.FrameSize;
    pFrameInfo->FrameOffset = frameInfo.FrameOffset;
    pFrameInfo->FrameTime = frameInfo.FrameTime;
    pFrameInfo->IsSyncFrame = frameInfo.IsSyncFrame;
    pFrameInfo->Framedelta = frameInfo.Framedelta;
    return TRUE;
  }
  return FALSE;
}



#ifdef FEATURE_MP4_ENABLE_SET_DEBUG_VARIABLES
// Undocumented feature CheckDebugURLFile
// Sets the value of a debug variable based upon debug information
// contaned with in a specially formatted .url file
// .url file format:
// debug:<Variable>=<Value>
// Where <Variable> is the name of the variable,
//       and value is the value to assign to the variable
// This function will look for <variable> in the DebugValueTypeTable
// and assign <value> to depending on the type.
int CheckDebugURLFile(char *videoFileName, char *audioFileName)
{
  int result = FALSE;
  char *fileName = NULL;

  if (videoFileName != NULL && videoFileName[0] != '\0')
  {
    fileName = videoFileName;
  }
  else if (audioFileName != NULL && audioFileName[0] != '\0')
  {
    fileName = audioFileName;
  }
  if (fileName)
  {
    char *pos = strrchr(fileName, '.');
    if (pos && strcmp(pos,DEBUG_URL_FILE_EXTENSION) == 0)
    {
      char str[DEBUG_MAX_STRING_LENGTH];

      fs_handle_type efs_handle = efs_open(fileName, O_RDONLY); 
      if(efs_handle < 0)
      {
         return FALSE;
      }
      else
      {
        char *variable;
        char *value;
        int len = efs_read(efs_handle, str, DEBUG_MAX_STRING_LENGTH);
        efs_close(efs_handle);
        str[len] = '\0';

        if (strncmp(str, DEBUG_URL_FILE_TOKEN, strlen(DEBUG_URL_FILE_TOKEN)) == 0)
        {
          variable = str + strlen(DEBUG_URL_FILE_TOKEN);
          value = strchr(variable, DEBUG_VARIABLE_VALUE_SEPARATOR);
          if (value != NULL)
          {
            int i;
            value[0] = '\0';
            value++;
            DebugValueType *dv = NULL;
            for (i = 0; DebugValueTypeTable[i].type != DEBUG_VARIABLE_TYPE_END; i++)
            {
              if (strcmp(DebugValueTypeTable[i].variable, variable) == 0)
              {
                 dv = &DebugValueTypeTable[i];
                 break;
              }
            }
            if (dv)
            {
              switch (dv->type)
              {
              case DEBUG_VARIABLE_TYPE_STRING:
                (void)std_strlcpy((char *)dv->value, value, dv->length);
                break;
              case DEBUG_VARIABLE_TYPE_INTEGER:
                *((uint32 *)dv->value) = atol(value);
                break;
              }
              MSG_SPRINTF_2(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Debug Value %s set to %s", variable, value);
              result = TRUE;
            }
          }
        }
      }
    }
  }
  return result;
}
#endif // FEATURE_MP4_ENABLE_SET_DEBUG_VARIABLES

#ifdef FEATURE_QTV_IPL_SCALING
#error code not present
#endif /* FEATURE_QTV_IPL_SCALING */

#if (defined (FEATURE_MP4_AAC_PLUS) || defined (PLATFORM_LTK) )
/* ======================================================================
FUNCTION
  RotateColorConvert

DESCRIPTION
  Sets up the input image, output image & the crop info for the QTV IPL 
  functions to perform rotation/color conversion

DEPENDENCIES
  FEATURE_MP4_AAC_PLUS
   
RETURN VALUE
  None 

SIDE EFFECTS
  None

========================================================================== */
static void RotateColorConvert( QtvPlayer::FrameInfoT*  pFrameInfo, OEMHandle pOEM )
{
  ipl_image_type in_ipl_image, out_ipl_image;
  ipl_status_type statusCode;
  ipl_rect_type crop_dim;
#ifdef PLATFORM_LTK
#error code not present
#endif /* PLATFORM_LTK */

  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);
  if(pOEM == NULL)
  {
    MSG_ERROR("RotateColorConvert, pOEM is null.", 0, 0, 0);
    return ;
  }
  /* Initialize the input image dimensions, color format, YUV buffer ptr */
  in_ipl_image.dx = pFrameInfo->Width;
  in_ipl_image.dy = pFrameInfo->Height;
  in_ipl_image.cFormat = IPL_YCbCr420_FRAME_PK; 
#ifdef PLATFORM_LTK
#error code not present
#else
  in_ipl_image.imgPtr = (unsigned char*) pFrameInfo->YUVBuffer;
#endif  /* PLATFORM_LTK */

  /* Initialize the cropping params */
  crop_dim.x = 0;
  crop_dim.y = 0;

  switch( pOEM->m_IPLRotationFactor )
  {
    case MM_MPEG4_NO_ROTATION:
      crop_dim.dx = pFrameInfo->Width; 
      crop_dim.dy = pFrameInfo->Height; 

      /* Initialize the output image dimensions, color format, RGB buffer ptr */
      out_ipl_image.dx = (uint32)(pFrameInfo->Width);
      out_ipl_image.dy = (uint32)(pFrameInfo->Height);
      out_ipl_image.cFormat = IPL_RGB565;
      out_ipl_image.imgPtr = (unsigned char*)pFrameInfo->RGBBuffer;

      /* Call the Qtv IPL function for color conversion */
      statusCode = qtv_ipl_convertAndCrop_YCbCr420ToRGB(&in_ipl_image, &out_ipl_image,
                                                        &crop_dim);
      break;

    case MM_MPEG4_90_CW_ROTATION:
      crop_dim.dx = pFrameInfo->Height; /* Swap height & width for 90 & 270 rotation */
      crop_dim.dy = pFrameInfo->Width;

      /* Initialize the output image dimensions, color format, RGB buffer ptr */
      out_ipl_image.dx = (uint32)(pFrameInfo->Height); /* Swap height & width for 90 & 270 rotation */
      out_ipl_image.dy = (uint32)(pFrameInfo->Width);
      out_ipl_image.cFormat = IPL_RGB565;
      out_ipl_image.imgPtr = (unsigned char*)pFrameInfo->RGBBuffer;

      /* Call the Qtv IPL function for color conversion & rotation */
      statusCode = qtv_ipl_convertCropAndRot90_YCbC420ToRGB(&in_ipl_image, &out_ipl_image,
                                                        &crop_dim);
      break;

    case MM_MPEG4_90_CCW_ROTATION:
      crop_dim.dx = pFrameInfo->Height; /* Swap height & width for 90 & 270 rotation */ 
      crop_dim.dy = pFrameInfo->Width; 

      /* Initialize the output image dimensions, color format, RGB buffer ptr */
      out_ipl_image.dx = (uint32)(pFrameInfo->Height); /* Swap height & width for 90 & 270 rotation */
      out_ipl_image.dy = (uint32)(pFrameInfo->Width);
      out_ipl_image.cFormat = IPL_RGB565;
      out_ipl_image.imgPtr = (unsigned char*)pFrameInfo->RGBBuffer;

      /* Call the Qtv IPL function for color conversion & rotation */
      statusCode = qtv_ipl_convertCropAndRot270_YCbC420ToRGB(&in_ipl_image, &out_ipl_image,
                                                        &crop_dim);
      break;

    case MM_MPEG4_180_ROTATION:
      crop_dim.dx = pFrameInfo->Width; 
      crop_dim.dy = pFrameInfo->Height; 

      /* Initialize the output image dimensions, color format, RGB buffer ptr */
      out_ipl_image.dx = (uint32)(pFrameInfo->Width);
      out_ipl_image.dy = (uint32)(pFrameInfo->Height);
      out_ipl_image.cFormat = IPL_RGB565;
      out_ipl_image.imgPtr = (unsigned char*)pFrameInfo->RGBBuffer;

      /* Call the Qtv IPL function for color conversion & rotation */
      statusCode = qtv_ipl_convertCropAndRot180_YCbC420ToRGB(&in_ipl_image, &out_ipl_image,
                                                        &crop_dim);
      break;

    default:
      MSG_ERROR( "Unknown frame rotation %d", pOEM->m_IPLRotationFactor, 0, 0);
      break;
  }

  /* If error display previous frame */
  if (statusCode == IPL_FAILURE)
  {
    MSG_ERROR("Error during rotation-color conversion.", 0, 0, 0);
  }
  else /* Update if no error */
  {
    //Now copy it back to rectImage
    pFrameInfo->Height = out_ipl_image.dy;
    pFrameInfo->Width = out_ipl_image.dx;
    pFrameInfo->RGBBuffer = out_ipl_image.imgPtr;
  }
  
  // We want to scale the frame relative to the original frame, not the current one
  if (pOEM->m_IPLScalingFactor != MM_MPEG4_NO_SCALING)
    ScaleFrame( pFrameInfo, pOEM ); 
#ifdef PLATFORM_LTK
#error code not present
#endif /* PLATFORM_LTK */
}
#endif /* FEATURE_MP4_AAC_PLUS || PLATFORM_LTK */

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
bool OEMMediaMPEG42PV_PrepareTimedText(OEMHandle pOEM)
{
  QtvPlayer::ThreeGPPTimedTextInfo TextInfo;
  bool bRet = false;

  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);
  if(pOEM == NULL)
  {
    MSG_ERROR("OEMMediaMPEG42PV_PrepareTimedText, pOEM is null.", 0, 0, 0);
    return bRet;
  }
  if(QtvPlayer::Get3GPPTimedTextInfo(&TextInfo))
  {
    MUTEX_LOCK_CS(TimedText3gppCS);
    /* free if there is any buffer */
    if(pOEM->m_TimedText3GPP.pszText)
    {
      FREE(pOEM->m_TimedText3GPP.pszText);
      pOEM->m_TimedText3GPP.pszText = NULL;
    }
    if(pOEM->m_TimedText3GPP.pFontList)
    {
      FREE(pOEM->m_TimedText3GPP.pFontList);
      pOEM->m_TimedText3GPP.pFontList = NULL;
    }
    if(pOEM->m_TimedText3GPP.pSampleModifiersBuffer)
    {
      FREE(pOEM->m_TimedText3GPP.pSampleModifiersBuffer);
      pOEM->m_TimedText3GPP.pSampleModifiersBuffer = NULL;
    }

    if(TextInfo.dwTextDataSize)
    {
      uint8 *pTextData = (uint8*)MALLOC(TextInfo.dwTextDataSize);
      if(QtvPlayer::Get3GPPTimedTextData(pTextData, TextInfo.dwTextDataSize))
      {
        /* first two bytes are text length */
        uint16 nTextLen = pTextData[0];
        nTextLen = (nTextLen<<8) | pTextData[1];
        pOEM->m_TimedText3GPP.pszText = (AECHAR*)MALLOC((nTextLen+1)*sizeof(AECHAR));
        if(NULL != pOEM->m_TimedText3GPP.pszText)
       {
        MEMSET(pOEM->m_TimedText3GPP.pszText, 0, nTextLen+1);
        UTF8TOWSTR(pTextData+2, nTextLen, pOEM->m_TimedText3GPP.pszText, nTextLen*sizeof(AECHAR));
        }
        else
        {
            MSG_ERROR("OEMMediaMPEG42PV_PrepareTimedText, MALLOC(pOEM->m_TimedText3GPP.pszText) failed .", 0, 0, 0);
        }

        pOEM->m_TimedText3GPP.dwSampleModifiersSize   = TextInfo.dwSampleModifiersSize;
        if ( TextInfo.dwSampleModifiersSize )
        {
          pOEM->m_TimedText3GPP.pSampleModifiersBuffer = (uint8*)MALLOC(pOEM->m_TimedText3GPP.dwSampleModifiersSize);
          if ( pOEM->m_TimedText3GPP.pSampleModifiersBuffer )
          {
            /* sample modifiers are after text in the data buffer */
            MEMCPY(pOEM->m_TimedText3GPP.pSampleModifiersBuffer, pTextData+2+nTextLen, pOEM->m_TimedText3GPP.dwSampleModifiersSize);
          }
        }

        pOEM->m_TimedText3GPP.dwDisplayFlags          = TextInfo.dwDisplayFlags;
        pOEM->m_TimedText3GPP.dwHorzJustification     = TextInfo.dwHorzJustification;
        pOEM->m_TimedText3GPP.dwVertJustification     = TextInfo.dwVertJustification;
        pOEM->m_TimedText3GPP.dwBoxTop                = TextInfo.dwBoxTop;
        pOEM->m_TimedText3GPP.dwBoxLeft               = TextInfo.dwBoxLeft;
        pOEM->m_TimedText3GPP.dwBoxBottom             = TextInfo.dwBoxBottom;
        pOEM->m_TimedText3GPP.dwBoxRight              = TextInfo.dwBoxRight;
        pOEM->m_TimedText3GPP.dwStartChar             = TextInfo.dwStartChar;
        pOEM->m_TimedText3GPP.dwEndChar               = TextInfo.dwEndChar;
        pOEM->m_TimedText3GPP.dwFontID                = TextInfo.dwFontID;
        pOEM->m_TimedText3GPP.dwFontStyleFlags        = TextInfo.dwFontStyleFlags;
        pOEM->m_TimedText3GPP.dwFontSize              = TextInfo.dwFontSize;
        pOEM->m_TimedText3GPP.dwFontListSize          = TextInfo.dwFontListSize;
        pOEM->m_TimedText3GPP.dwDuration              = TextInfo.dwDuration;

        QtvPlayer::Get3GPPTimedTextBGRGB(pOEM->m_TimedText3GPP.BackgroundColorRGBA, MM_MPEG4_KDDI_MAX_TEXT_COLORS);
        QtvPlayer::Get3GPPTimedTextTxtRGB(pOEM->m_TimedText3GPP.TextColorRGBA, MM_MPEG4_KDDI_MAX_TEXT_COLORS);

        if(pOEM->m_TimedText3GPP.dwFontListSize)
        {
          pOEM->m_TimedText3GPP.pFontList = (AEETTFontInfo *)MALLOC(sizeof(AEETTFontInfo)*pOEM->m_TimedText3GPP.dwFontListSize);
          if(pOEM->m_TimedText3GPP.pFontList)
          {
            uint16 i;
            for(i=0; i<(uint16)pOEM->m_TimedText3GPP.dwFontListSize; i++)
            {
              QtvPlayer::Get3GPPFontInfoAt( &(pOEM->m_TimedText3GPP.pFontList[i].nID),
                                            pOEM->m_TimedText3GPP.pFontList[i].Name,
                                            MM_MPEG4_3GPP_MAX_FONT_LEN,
                                            i );
            }
          }
        }
        bRet = true;
      }
      FREE(pTextData);
    }
   MUTEX_UNLOCK_CS(TimedText3gppCS);
  }
  return bRet;
}
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

#ifdef FEATURE_QTV_ISDB_SUBTITLES
#error code not present
#endif /* FEATURE_QTV_ISDB_SUBTITLES*/

#ifdef FEATURE_QTV_SKT_MOD_MIDI
/* returns the size of MIDI data */
extern "C" int OEMMediaMPEG42PV_GetMidiDataSize(uint32 *pSize)
{
  uint32 size = QtvPlayer::GetMidiDataSize();
  int nRet = EFAILED;
  if(size)
  {
    *pSize = size;
    nRet = SUCCESS;
  }
  return nRet;
}

/* returns the midi data */
extern "C" int OEMMediaMPEG42PV_GetMidiData(uint8 *pBuf, uint32 *pSize)
{
  uint32 size = QtvPlayer::GetMidiData(pBuf, *pSize, 0);
  int nRet = EFAILED;
  if(size)
  {
    *pSize = size;
    nRet = SUCCESS;
  }
  return nRet;
}

/* returns the size of LINK data */
extern "C" int OEMMediaMPEG42PV_GetLinkDataSize(uint32 *pSize)
{
  uint32 size = QtvPlayer::GetLinkDataSize();
  int nRet = EFAILED;
  if(size)
  {
    *pSize = size;
    nRet = SUCCESS;
  }
  return nRet;
}

/* returns the LINK data */
extern "C" int OEMMediaMPEG42PV_GetLinkData(uint8 *pBuf, uint32 *pSize)
{
  uint32 size = QtvPlayer::GetLinkData(pBuf, *pSize);
  int nRet = EFAILED;
  if(size)
  {
    *pSize = size;
    nRet = SUCCESS;
  }
  return nRet;
}

#endif /* FEATURE_QTV_SKT_MOD_MIDI */

/* fetch the size of the specified atom;
   *pSize = 0 => atom (aType) not present */
extern "C" uint8 OEMMediaMPEG42PV_GetDataSize(int32 aType, uint32 *pSize)
{
    uint32 size = 0;
    *pSize = 0;
    uint8 nRet = EFAILED;

    switch(aType)
    {
      case MM_MP4_PARM_ATOM_FTYP_SIZE: //’ftyp’ atom
          size = QtvPlayer::GetDataSize(QtvPlayer::DATA_ATOM_FTYP);
          break;
      case MM_MP4_PARM_ATOM_DCMD_SIZE: //’dcmd’ atom : DRM
          size = QtvPlayer::GetDataSize(QtvPlayer::DATA_ATOM_DCMD);
          break;
      case MM_MP4_PARM_ATOM_UDTA_CPRT_SIZE: //’cprt’ atom
          size = QtvPlayer::GetDataSize(QtvPlayer::DATA_ATOM_UDTA_CPRT);
          break;
      case MM_MP4_PARM_ATOM_UDTA_AUTH_SIZE: //’auth’ atom
          size = QtvPlayer::GetDataSize(QtvPlayer::DATA_ATOM_UDTA_AUTH);
          break;
      case MM_MP4_PARM_ATOM_UDTA_TITL_SIZE: //'titl' atom
          size = QtvPlayer::GetDataSize(QtvPlayer::DATA_ATOM_UDTA_TITL);
          break;
      case MM_MP4_PARM_ATOM_UDTA_DSCP_SIZE: //'dscp' atom
          size = QtvPlayer::GetDataSize(QtvPlayer::DATA_ATOM_UDTA_DSCP);
          break;
      case MM_MP4_PARM_TEXT_TKHD_ORIGIN_X_SIZE: //Text origin_x
          size = QtvPlayer::GetDataSize(QtvPlayer::DATA_TEXT_TKHD_ORIGIN_X);
          break;
      case MM_MP4_PARM_TEXT_TKHD_ORIGIN_Y_SIZE: //Text origin_y
          size = QtvPlayer::GetDataSize(QtvPlayer::DATA_TEXT_TKHD_ORIGIN_Y);
          break;
      case MM_MP4_PARM_TEXT_TKHD_WIDTH_SIZE: //Text width
          size = QtvPlayer::GetDataSize(QtvPlayer::DATA_TEXT_TKHD_WIDTH);
          break;
      case MM_MP4_PARM_TEXT_TKHD_HEIGHT_SIZE: //Text height
          size = QtvPlayer::GetDataSize(QtvPlayer::DATA_TEXT_TKHD_HEIGHT);
          break;
      case MM_MP4_PARM_ATOM_UDTA_RTNG_SIZE: //’rtng’ atom
          size = QtvPlayer::GetDataSize(QtvPlayer::DATA_ATOM_UDTA_RTNG);
          break;
      case MM_MP4_PARM_ATOM_UDTA_PERF_SIZE: //’perf’ atom
          size = QtvPlayer::GetDataSize(QtvPlayer::DATA_ATOM_UDTA_PERF);
          break;
      case MM_MP4_PARM_ATOM_UDTA_CLSF_SIZE: //’clsf’ atom
          size = QtvPlayer::GetDataSize(QtvPlayer::DATA_ATOM_UDTA_CLSF);
          break;
      case MM_MP4_PARM_ATOM_UDTA_KYWD_SIZE: //’kywd’ atom
          size = QtvPlayer::GetDataSize(QtvPlayer::DATA_ATOM_UDTA_KYWD);
          break;
      case MM_MP4_PARM_ATOM_UDTA_LOCI_SIZE: //’loci’ atom
          size = QtvPlayer::GetDataSize(QtvPlayer::DATA_ATOM_UDTA_LOCI);
          break;
      case MM_MP4_PARM_ATOM_UDTA_GNRE_SIZE: //’gnre’ atom
          size = QtvPlayer::GetDataSize(QtvPlayer::DATA_ATOM_UDTA_GNRE);
          break;
    case MM_MP4_PARM_ATOM_UDTA_META_SIZE: //’meta’ atom
          size = QtvPlayer::GetDataSize(QtvPlayer::DATA_ATOM_UDTA_META);
          break;

    }
    if(size)
    {
      *pSize = size;
      nRet = SUCCESS;
    }
    return nRet;
}

/* fetch the data of the specified atom */
extern "C" uint8 OEMMediaMPEG42PV_GetData(int32 aType, uint8 *pBuf, uint32 *pSize)
{
   uint32 size = 0;
   uint8 nRet = EFAILED;

   switch(aType)
   {
     case MM_MP4_PARM_ATOM_FTYP_DATA: //’ftyp’ atom
         size = QtvPlayer::GetData(QtvPlayer::DATA_ATOM_FTYP, pBuf, *pSize, 0);
         break;
     case MM_MP4_PARM_ATOM_DCMD_DATA: //’dcmd’ atom : DRM
         size = QtvPlayer::GetData(QtvPlayer::DATA_ATOM_DCMD, pBuf, *pSize, 0);
         break;
     case MM_MP4_PARM_ATOM_UDTA_CPRT_DATA: //’cprt’ atom
         size = QtvPlayer::GetData(QtvPlayer::DATA_ATOM_UDTA_CPRT, pBuf, *pSize, 0);
         break;
     case MM_MP4_PARM_ATOM_UDTA_AUTH_DATA: //’auth’ atom
         size = QtvPlayer::GetData(QtvPlayer::DATA_ATOM_UDTA_AUTH, pBuf, *pSize, 0);
         break;
     case MM_MP4_PARM_ATOM_UDTA_TITL_DATA: //'titl' atom
         size = QtvPlayer::GetData(QtvPlayer::DATA_ATOM_UDTA_TITL, pBuf, *pSize, 0);
         break;
     case MM_MP4_PARM_ATOM_UDTA_DSCP_DATA: //'dscp' atom
         size = QtvPlayer::GetData(QtvPlayer::DATA_ATOM_UDTA_DSCP, pBuf, *pSize, 0);
         break;
     case MM_MP4_PARM_TEXT_TKHD_ORIGIN_X_DATA: //Text origin_x
         size = QtvPlayer::GetData(QtvPlayer::DATA_TEXT_TKHD_ORIGIN_X, pBuf, *pSize, 0);
         break;
     case MM_MP4_PARM_TEXT_TKHD_ORIGIN_Y_DATA: //Text origin_y
         size = QtvPlayer::GetData(QtvPlayer::DATA_TEXT_TKHD_ORIGIN_Y, pBuf, *pSize, 0);
         break;
     case MM_MP4_PARM_TEXT_TKHD_WIDTH_DATA: //Text width
         size = QtvPlayer::GetData(QtvPlayer::DATA_TEXT_TKHD_WIDTH, pBuf, *pSize, 0);
         break;
     case MM_MP4_PARM_TEXT_TKHD_HEIGHT_DATA: //Text height
         size = QtvPlayer::GetData(QtvPlayer::DATA_TEXT_TKHD_HEIGHT, pBuf, *pSize, 0);
         break;
     case MM_MP4_PARM_MEDIA_INFO: //Media information
         size = QtvPlayer::GetData(QtvPlayer::DATA_MEDIA_INFO, pBuf, *pSize, 0);
         break;
     case MM_MP4_PARM_ATOM_UDTA_RTNG_DATA: //’rtng’ atom
         size = QtvPlayer::GetData(QtvPlayer::DATA_ATOM_UDTA_RTNG, pBuf, *pSize, 0);
         break;
     case MM_MP4_PARM_ATOM_UDTA_PERF_DATA: //’perf’ atom
         size = QtvPlayer::GetData(QtvPlayer::DATA_ATOM_UDTA_PERF, pBuf, *pSize, 0);
         break;
     case MM_MP4_PARM_ATOM_UDTA_GNRE_DATA: //’gnre’ atom
         size = QtvPlayer::GetData(QtvPlayer::DATA_ATOM_UDTA_GNRE, pBuf, *pSize, 0);
         break;
     case MM_MP4_PARM_ATOM_UDTA_KYWD_DATA: //’kywd’ atom
         size = QtvPlayer::GetData(QtvPlayer::DATA_ATOM_UDTA_KYWD, pBuf, *pSize, 0);
         break;
     case MM_MP4_PARM_ATOM_UDTA_LOCI_DATA: //’loci’ atom
         size = QtvPlayer::GetData(QtvPlayer::DATA_ATOM_UDTA_LOCI, pBuf, *pSize, 0);
         break;
     case MM_MP4_PARM_ATOM_UDTA_CLSF_DATA: //’clsf’ atom
         size = QtvPlayer::GetData(QtvPlayer::DATA_ATOM_UDTA_CLSF, pBuf, *pSize, 0);
         break;
    case MM_MP4_PARM_ATOM_UDTA_META_DATA: //’meta’ atom
          size = QtvPlayer::GetData(QtvPlayer::DATA_ATOM_UDTA_META, pBuf, *pSize, 0);
          break;
   }
   if(size)
   {
     *pSize = size;
     nRet = SUCCESS;
   }
   return nRet;
}

#endif   // FEATURE_APP_MPEG4

//Function to modify buffering low water mark point via the dia interface
extern "C" int OEMMediaMPEG42PV_ModifyBufferLowMark(int32 modified_buffer_lowmark)
{
   QtvPlayer::ReturnT retVal;

   retVal = QtvPlayer::ModifyBufferLowMark(modified_buffer_lowmark);
   if (retVal == QtvPlayer::QTV_RETURN_OK)
   {
      return SUCCESS;
   }
   else
   {
      return EFAILED;
   }
}

/* ======================================================================
FUNCTION:                                                            GRB
  OEMMediaMPEG42PV_ReleaseCurrentVideoFrameBuffer

DESCRIPTION:
  Notify the player that higher levels have finished rendering the
  current video frame.  The player is free to reuse or free the buffer
  after this function is called.

INPUT/OUTPUT PARAMETERS:
  pBuffer - Pointer to the buffer to be released.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
extern "C" void OEMMediaMPEG42PV_ReleaseCurrentVideoFrameBuffer( void *pBuffer )
{
  OEMHandle pOEM = OEMMediaMPEG42PV_GetOEMLayer(NULL);
  if (!pOEM) 
  {
    return;
  }
  if(pBuffer == NULL)
  {
    QtvPlayer::FrameInfoT  frameInfo;
    QtvPlayer::GetFrameInfo(frameInfo);
    pBuffer = frameInfo.RGBBuffer;
  }
#ifdef FEATURE_QTV_OEM_BUFFER_MGR
  else
  {
    ReleaseOutputBuffer(NULL, NULL, pBuffer);
  }
#endif /* FEATURE_QTV_OEM_BUFFER_MGR */
  QtvPlayer::ReleaseCurrentVideoFrameBuffer(pBuffer, pOEM->m_pHandle);
}

/* ======================================================================
FUNCTION:
  OEMMediaMPEG42PV_SetDataPortRange

DESCRIPTION:
  Set the range of UDP ports that Qtv can attempt to use when setting up
  an RTP connection in order to stream audio or video from a server.  The
  default range is set in QtvStream.h  The function enforces that the
  range start with an even port number and end with an odd port number to
  meet the requirements of RTP, RFC1889. Changes to the range take effect
  with the next attempt to make an RTP connection.

INPUT/OUTPUT PARAMETERS:
  beginPort
  endPort

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
extern "C" int OEMMediaMPEG42PV_SetDataPortRange(int beginPort, int endPort)
{
  QtvPlayer::SetDataPortRange(beginPort, endPort);
  return SUCCESS;
}

/* ======================================================================
FUNCTION:
  OEMMediaMPEG42PV_GetVideoCodec

DESCRIPTION:
  gives video codec of currently opened clip.  Valid only after clip has been
  opened successfully, means after QTV_PLAYER_STATUS_PLAYBACK_READY has been
  received.
INPUT/OUTPUT PARAMETERS:
AEEMediaMP4CodecType * pCodec - Output parameter
OEMHandle pOEM - Gives access to instance of bClipInfoAvail

RETURN VALUE:
  SUCCESS if info is available.
  EBADSTATE if info is not available.

SIDE EFFECTS:
  available only after QTV is in PLAYBACK_READY state.
======================================================================*/
extern "C" int OEMMediaMPEG42PV_GetVideoCodec(AEEMediaMP4CodecType *pCodec, OEMHandle pOEM)
{
  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);
  if (pOEM && pOEM->m_bClipInfoAvail )
  {
    *pCodec = OEMMediaMPEG42PV_GetVideoCodecType();
    return SUCCESS;
  }
  return EBADSTATE;
}

/* ======================================================================
FUNCTION:
  OEMMediaMPEG42PV_GetAudioCodec

DESCRIPTION:
  gives audio codec of currently opened clip.  Valid only after clip has
  been opened successfully, means after QTV_PLAYER_STATUS_PLAYBACK_READY
  has been received.
INPUT/OUTPUT PARAMETERS:
AEEMediaMP4CodecType * pCodec - Output parameter

RETURN VALUE:
  SUCCESS if info is available.
  EBADSTATE if info is not available.

SIDE EFFECTS:
  available only after QTV is in PLAYBACK_READY state.
======================================================================*/
extern "C" int OEMMediaMPEG42PV_GetAudioCodec(AEEMediaMP4CodecType *pCodec,
                                              OEMHandle pOEM)
{
  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);
  if (pOEM && pOEM->m_bClipInfoAvail )
  {
    *pCodec = OEMMediaMPEG42PV_GetAudioCodecType(pOEM);
    return SUCCESS;
  }
  return EBADSTATE;
}

/* ======================================================================
FUNCTION:
  OEMMediaMPEG42PV_GetVideoFrameRate

DESCRIPTION:
  gives video frame rate of currently opened clip.  Valid only after clip
  has been opened successfully, means after QTV_PLAYER_STATUS_PLAYBACK_READY
  has been received.
INPUT/OUTPUT PARAMETERS:
  float * pFrameRate - Output parameter

RETURN VALUE:
  SUCCESS if info is available.
  EBADSTATE if info is not available.

SIDE EFFECTS:
  available only after QTV is in PLAYBACK_READY state.
======================================================================*/
extern "C" int OEMMediaMPEG42PV_GetVideoFrameRate(float *pFrameRate, OEMHandle pOEM)
{
  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);
  if (pOEM && pOEM->m_bClipInfoAvail)
  {
    *pFrameRate = ((QtvPlayer::ClipInfoT*)pOEM->m_pClipInfo)->VideoFrameRate;
    return SUCCESS;
  }
  return EBADSTATE;
}

/* ======================================================================
FUNCTION:
  OEMMediaMPEG42PV_GetVideoAverageBitRate

DESCRIPTION:
  gives video average bitrate of currently opened clip.  Valid only after
  clip has been opened successfully, means after QTV_PLAYER_STATUS_PLAYBACK_READY
  has been received.
INPUT/OUTPUT PARAMETERS:
  unsigned long *pBitRate - Output parameter

RETURN VALUE:
  SUCCESS if info is available.
  EBADSTATE if info is not available.

SIDE EFFECTS:
  available only after QTV is in PLAYBACK_READY state.
======================================================================*/
extern "C" int OEMMediaMPEG42PV_GetVideoAverageBitRate(unsigned long *pBitRate, OEMHandle pOEM)
{
  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);
  if (pOEM && pOEM->m_bClipInfoAvail )
  {
    *pBitRate = ((QtvPlayer::ClipInfoT*)pOEM->m_pClipInfo)->VideoEncodedBitRate;
    return SUCCESS;
  }
  return EBADSTATE;
}
#ifdef FEATURE_QTV_DIVX_DRM
/* ======================================================================
FUNCTION:
  OEMMediaMPEG42PV_GetRegistrationCode

DESCRIPTION:
 Retrieves the device registration code.
 This code can be used to activate the device by downloading activation clip from media portal.

INPUT/OUTPUT PARAMETERS:
  char *code - Output parameter

RETURN VALUE:
  SUCCESS if code is retrieved.
  EBADSTATE if fails to retrieve the code

SIDE EFFECTS:
  None
======================================================================*/
extern "C" int OEMMediaMPEG42PV_GetRegistrationCode(char *code, int* max_len,OEMHandle pOEM)
{
  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);
  QtvPlayer::ReturnT nReturn = QtvPlayer::GetRegistrationCode(code, max_len,
                                                              pOEM->m_pHandle,
                                                              (QtvPlayer::InstancePriorityT)pOEM->m_nPriority );  
  if( nReturn == QtvPlayer::QTV_RETURN_OK )
  {    
    return SUCCESS;
  }
  return EFAILED;
}
#endif//#ifdef FEATURE_QTV_DIVX_DRM
/* ======================================================================
FUNCTION:
  OEMMediaMPEG42PV_GetAudioAverageBitRate

DESCRIPTION:
  gives audio average bitrate of currently opened clip.  Valid only after clip
  has been opened successfully, means after QTV_PLAYER_STATUS_PLAYBACK_READY
  has been received.
INPUT/OUTPUT PARAMETERS:
  unsigned long *pBitRate - Output parameter

RETURN VALUE:
  SUCCESS if info is available.
  EBADSTATE if info is not available.

SIDE EFFECTS:
  available only after QTV is in PLAYBACK_READY state.
======================================================================*/
extern "C" int OEMMediaMPEG42PV_GetAudioAverageBitRate(unsigned long *pBitRate, OEMHandle pOEM)
{
  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);
  if (pOEM && pOEM->m_bClipInfoAvail )
  {
    *pBitRate = ((QtvPlayer::ClipInfoT*)pOEM->m_pClipInfo)->AudioEncodedBitRate;
    return SUCCESS;
  }
  return EBADSTATE;
}

/* ======================================================================
FUNCTION:
  OEMMediaMPEG42PV_GetTotalRxDataRate

DESCRIPTION:
  gives Total Data Rate of reception for currently opened clip.

INPUT/OUTPUT PARAMETERS:
  None

RETURN VALUE:
  SUCCESS if info is available
  EFAILED if info not available

SIDE EFFECTS:
  None
======================================================================*/
extern "C" int OEMMediaMPEG42PV_GetTotalRxDataRate(uint32 *TotalRxDataRate)
{
  if(TotalRxDataRate != NULL)
  {
    *TotalRxDataRate = QtvPlayer::GetTotalRxDataRate();
    return SUCCESS;
  }
  return EFAILED;
}

/* ======================================================================
FUNCTION:
  OEMMediaMPEG42PV_RegisterFetchDataSizeCallback
  
DESCRIPTION:
  Register Fetch Data SIze Callback

INPUT/OUTPUT PARAMETERS:
  The three callbacks

RETURN VALUE:
  SUCCESS if info is available
  EFAILED if info not available

SIDE EFFECTS:
  None
======================================================================*/
extern "C" int OEMMediaMPEG42PV_RegisterFetchDataSizeCallback(uint32 *callbackPointer)
{ 
  if(callbackPointer != NULL)
  {
    g_cbFetchBufferedDataSize = (QtvPlayer::FetchBufferedDataSizeT) callbackPointer;
    return SUCCESS;
  }
  else 
  {
    return EFAILED;
  }  
}

/* ======================================================================
FUNCTION:
  OEMMediaMPEG42PV_RegisterFetchDataCallback

DESCRIPTION:
  Register Fetch Data SIze Callback

INPUT/OUTPUT PARAMETERS:
  The three callbacks

RETURN VALUE:
  SUCCESS if info is available
  EFAILED if info not available

SIDE EFFECTS:
  None
======================================================================*/
extern "C" int OEMMediaMPEG42PV_RegisterFetchDataCallback(uint32 *callbackPointer)
{
  if(callbackPointer != NULL)
  {
    g_cbFetchBufferedData = (QtvPlayer::FetchBufferedDataT) callbackPointer;
    return SUCCESS;
  }
  else 
  {
    return EFAILED;
  }  
}

/* ======================================================================
FUNCTION:
  OEMMediaMPEG42PV_RegisterIsMimeSupportedCallback
    
DESCRIPTION:
  Register Fetch Data SIze Callback

INPUT/OUTPUT PARAMETERS:
  The three callbacks

RETURN VALUE:
  SUCCESS if info is available
  EFAILED if info not available

SIDE EFFECTS:
  None
======================================================================*/
extern "C" int OEMMediaMPEG42PV_RegisterIsMimeSupportedCallback(uint32 *callbackPointer)
{
  if(callbackPointer != NULL)
  {
    g_cbIsMimeSupported = (QtvPlayer::IsMimeTypeSupportedT) callbackPointer;
    return SUCCESS;
  }
  else
  {
    return EFAILED;
  }
}

/* ======================================================================
FUNCTION:
  OEMMediaMPEG42PV_SetFileSaveOption

DESCRIPTION:
  Sets the directory path for saving a file.

INPUT/OUTPUT PARAMETERS:
  char *fileName - The directory path

RETURN VALUE:
  SUCCESS if info is available
  EFAILED if info not available

SIDE EFFECTS:
  None
======================================================================*/
extern "C" int OEMMediaMPEG42PV_SetFileSaveOption(char *fileName)
{
  return QtvPlayer::SetFileSaveOption(fileName);
}

/* ======================================================================
FUNCTION:
  OEMMediaMPEG42PV_GetEncryptedData

DESCRIPTION:
  Fetch the encrpyted data to decrypt during HTTP Download.

INPUT/OUTPUT PARAMETERS:
  buffer - the address of the data
  dataSize - the size of the data

RETURN VALUE:
  SUCCESS if info is available
  EFAILED if info not available

SIDE EFFECTS:
  None
======================================================================*/
extern "C" int OEMMediaMPEG42PV_GetEncryptedData(uint32 *buffer, uint32 *dataSize)
{
  uint32 size;

  *buffer = (uint32)QtvPlayer::GetEncryptedData(size);
  *dataSize = size;

  return SUCCESS;
}

/* ======================================================================
FUNCTION:
  OEMMediaMPEG42PV_FreeDownloadBuffer

DESCRIPTION:
  Free the buffer containing data.

INPUT/OUTPUT PARAMETERS:
  buffer - the address of the data

RETURN VALUE:
  SUCCESS if info is available
  EFAILED if info not available

SIDE EFFECTS:
  None
======================================================================*/
extern "C" int OEMMediaMPEG42PV_FreeDownloadBuffer(char *buffer)
{
  return QtvPlayer::FreeDownloadBuffer(buffer);
}
#ifdef FEATURE_QTV_GENERIC_BCAST
/* ======================================================================
FUNCTION:
  OEMMediaMPEG42PV_MutePlaybackTracks

DESCRIPTION:
  Mutes/unmutes the selected tracks.  

INPUT/OUTPUT PARAMETERS:
  bmTrack - Track may be audio(MM_QTV_AUDIO_TRACK), text(MM_QTV_TEXT_TRACK) or both.
  bMute - 1 to mute, 0 to unmute
  
RETURN VALUE:
  SUCCESS 
  EFAILED 
  EBADPARM 
  EUNSUPPORTED 
   
SIDE EFFECTS: Callbacks
    QTV_PLAYER_STATUS_MUTE_DONE 
    QTV_PLAYER_STATUS_UNMUTE_DONE 
    QTV_PLAYER_STATUS_MUTE_FAILED
    QTV_PLAYER_STATUS_UNMUTE_FAILED
    QTV_PLAYER_MUTE_ALL_FAILED
  
======================================================================*/
extern "C" int OEMMediaMPEG42PV_MutePlaybackTracks(uint32 bmTrack, uint32 bMute,
                                                  OEMHandle pOEM)
{
  QtvPlayer::ReturnT nRet;

  if ( !( bMute==0 || bMute==1 ) ||
       ( bmTrack & ~(MM_QTV_VIDEO_TRACK | MM_QTV_AUDIO_TRACK | MM_QTV_TEXT_TRACK) ) )
  {
    return EBADPARM;
  }

  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);
  if (pOEM == NULL)
  {
    MSG_ERROR("OEMMediaMPEG42PV_MutePlaybackTracks: Get OEM Layer returned NULL", 0, 0, 0);
    return EFAILED;
  }
  pOEM->m_nBcastMutePending = bmTrack;

  if (bmTrack & MM_QTV_AUDIO_TRACK) 
  {
    OEMMediaMPEG42PV_SetMuteCtl(pOEM, &bMute);
  }

  if (bMute) 
  {
    nRet = QtvPlayer::MutePlaybackTracks(bmTrack, 
                                        QTV_TRACK_MUTE, 
                                        pOEM->m_pHandle);
  }
  else 
  {
    nRet = QtvPlayer::MutePlaybackTracks(bmTrack, 
                                        QTV_TRACK_UNMUTE, 
                                        pOEM->m_pHandle);
  }

  if (nRet != QtvPlayer::QTV_RETURN_OK) 
  {
    pOEM->m_nBcastMutePending = 0;
  }

  if (nRet==QtvPlayer::QTV_RETURN_OK) 
  {
    return MM_PENDING;
  }
  else if (nRet == QtvPlayer::QTV_RETURN_FEATURE_NOT_AVAILABLE) 
  {
    return EUNSUPPORTED;
  }
  else
  {
    return EFAILED;
  }
}

/* ======================================================================
FUNCTION:
  OEMMediaMPEG42PV_SelectPlaybackTracks

DESCRIPTION:
  Mutes/unmutes the selected tracks.  

INPUT/OUTPUT PARAMETERS:
  bmTrack - Track bitmask may be video(MM_QTV_VIDEO_TRACK, audio(MM_QTV_AUDIO_TRACK), 
    text(MM_QTV_TEXT_TRACK) or combinations.
  pID - Struct of 3 track IDs
  
RETURN VALUE:
  SUCCESS 
  EFAILED 
  EBADPARM 
   
SIDE EFFECTS:
  None
======================================================================*/
extern "C" int OEMMediaMPEG42PV_SelectPlaybackTracks(uint32 bmTrack, AEETrackID *pID,
                                                     OEMHandle pOEM)
{
  if ( ( !pID ) ||
       ( bmTrack & ~(MM_QTV_VIDEO_TRACK | MM_QTV_AUDIO_TRACK | MM_QTV_TEXT_TRACK) ) )
  {
    return EBADPARM;
  }

  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);

  if (pOEM == NULL)
  {
    MSG_ERROR("OEMMediaMPEG42PV_SelectPlaybackTracks: Get OEM Layer returned NULL", 0, 0, 0);
    return EFAILED;
  }

  return QtvPlayer::SelectPlaybackTracks(bmTrack, pID->nAudioTrackID, pID->nVideoTrackID,
                                         pID->nTextTrackID, pOEM->m_pHandle);
}


/* ======================================================================
FUNCTION:
  OEMMediaMPEG42PV_ReadPlaybackTracks

DESCRIPTION:
  Mutes/unmutes the selected tracks.  

INPUT/OUTPUT PARAMETERS:
  bmTrack - Track bitmask may be video(MM_QTV_VIDEO_TRACK, audio(MM_QTV_AUDIO_TRACK), 
    text(MM_QTV_TEXT_TRACK) or combinations.
  pID - Struct of 3 track IDs
  
RETURN VALUE:
  SUCCESS 
  EFAILED 
  EBADPARM 
   
SIDE EFFECTS:
  None
======================================================================*/
extern "C" int OEMMediaMPEG42PV_ReadPlaybackTracks(uint32 *pTrackCnt, 
                                                   AEETrackList *pTrackList, OEMHandle pOEM)
{
  if ( !pTrackCnt )
  {
    return EBADPARM;
  }

  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);

  if (pOEM == NULL)
  {
    MSG_ERROR("OEMMediaMPEG42PV_ReadPlaybackTracks: Get OEM Layer returned NULL", 0, 0, 0);
    return EFAILED;
  }

  return QtvPlayer::ReadPlaybackTracks(pTrackCnt, (QtvPlayer::TrackListT *)pTrackList, pOEM->m_pHandle);
}


/* ======================================================================
FUNCTION:
  OEMMediaMPEG42PV_PausedReposition

DESCRIPTION:
  Async call to absolutely reposition.  

INPUT/OUTPUT PARAMETERS:
  nTs - Time in seconds into the clip to reposition to.
  pTransID - Transaction ID to be returned through related callbacks.
  
RETURN VALUE:
  SUCCESS 
  EFAILED 
  EINVALIDTIME
  EBADSTATE 
   
SIDE EFFECTS: Callbacks
  QTV_PLAYER_STATUS_REPOSITION_DONE
  QTV_PLAYER_STATUS_REPOSITION_FAILED
======================================================================*/
extern "C" int OEMMediaMPEG42PV_PausedReposition(uint32 nTs, uint32 *pTransID, OEMHandle pOEM)
{
  QtvPlayer::ReturnT nRet;

  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);

  if (pOEM == NULL)
  {
    MSG_ERROR("OEMMediaMPEG42PV_PausedReposition: Get OEM Layer returned NULL", 0, 0, 0);
    return EFAILED;
  }

  if ( !pOEM->m_bPaused ) 
  {
    return EINVALIDTIME;
  }

  if ( pOEM->m_bSeekPending ) 
  {
    return EBADSTATE;
  }
  pOEM->m_bSeekPending = TRUE;
  nRet = QtvPlayer::Reposition(nTs, pTransID, pOEM->m_pHandle);
  if ( nRet != QtvPlayer::QTV_RETURN_OK ) 
  {
    pOEM->m_bSeekPending = FALSE;
    return EFAILED;
  }
  return MM_PENDING;
}


/* ======================================================================
FUNCTION:
  OEMMediaMPEG42PV_FrameSeek

DESCRIPTION:
  Asyncronous call to seek forward or backward by the specified number of 
  sync frames.  Only valid during paused state.  

INPUT/OUTPUT PARAMETERS:
  nSyncCnt - Number of sync frames to move forward/backward.
  pTransID - Transaction ID to be returned through related callbacks.
  bPlayAfter - TRUE plays the frame after seeking to it, FALSE just seeks.
 
RETURN VALUE:
  SUCCESS 
  EFAILED 
  EINVALIDTIME 
   
SIDE EFFECTS: Callbacks
  QTV_PLAYER_STATUS_SEEK_SYNC_DONE
  QTV_PLAYER_STATUS_SEEK_SYNC_FAILED
  QTV_PLAYER_NO_SYNC_FRAME_ERROR
  QTV_PLAYER_STATUS_PLAY_SYNC_FRAME_DONE
  QTV_PLAYER_STATUS_PLAY_SYNC_FRAME_FAILED
  QTV_PLAYER_NO_SYNC_FRAME_ERROR
======================================================================*/
int OEMMediaMPEG42PV_FrameSeek(int32 nSyncCnt, uint32 *pTransID, 
                                boolean bPlayAfter, OEMHandle pOEM)
{
  QtvPlayer::ReturnT nRet;
  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);

  if (pOEM == NULL)
  {
    MSG_ERROR("OEMMediaMPEG42PV_FrameSeek: Get OEM Layer returned NULL", 0, 0, 0);
    return EFAILED;
  }
  if ( pOEM->m_bSeekPending ) 
  {
    return EBADSTATE;
  }
  pOEM->m_bSeekPending = TRUE;

  if ( bPlayAfter ) 
  {
    nRet = QtvPlayer::PlaySyncFrame(nSyncCnt, pTransID, pOEM->m_pHandle);
  }
  else
  {
    nRet = QtvPlayer::SeekToSync(nSyncCnt, pTransID, pOEM->m_pHandle);
  }

  if ( nRet != QtvPlayer::QTV_RETURN_OK ) 
  {
    pOEM->m_bSeekPending = FALSE;
    return EFAILED;
  }
  return (int)nRet;
}
#endif /* FEATURE_QTV_GENERIC_BCAST */
#ifdef FEATURE_QTV_DRM_DCF
/* ======================================================================
FUNCTION:
  OEMMediaMPEG42PV_OpenIxStreamFromIxSource

DESCRIPTION:
  For supporting DCF playback, tries to extract IxStream from ISource and pass it down to the QtvPlayer

INPUT/OUTPUT PARAMETERS:
  ISource* pSource;

RETURN VALUE:
  SUCCESS if ISource contains IxStream
  EFAILED if there is no valid IxStream wrappeed inside ISource

SIDE EFFECTS:
  None
======================================================================*/
int OEMMediaMPEG42PV_OpenIxStreamFromISource(ISource* pSource, OEMHandle pOEM)
{
  int nRet = EBADPARM;
  IxStream* pStream = NULL;
  IxErrnoType  errorCode = E_FAILURE;
  if(pSource == NULL)
  {
    return nRet;
  }
  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);
  /*
  * IxStreamRecreateStream API from IxStreamUtils gives an IxStream which supports Seek,Tell,Size,CreateRef etc.
  * functionality on IxStream.
  */
  errorCode = ix_stream_recreate_stream(pSource, (void**)&pStream);
  if(errorCode == E_SUCCESS)
  {
    nRet = SUCCESS;
    //Very unlikely that after E_SUCCESS, pStream will be NULL;
    //Just to be on safe side check for non NULL
    if(pStream && pOEM)
    {
      nRet = QtvPlayer::OpenIxStream( (dcf_ixstream_type)pStream,                                      
                                      0x07/*play audio/video/text*/, 
                                      pOEM->m_pHandle, 
                                      (QtvPlayer::InstancePriorityT)pOEM->m_nPriority);            

      if(nRet == QtvPlayer::QTV_RETURN_ERROR)
      {
        //This can happen only when QtvPlayer is not able to post an event to itself or
        //no track has been selected for playback(no audio/video/text).
        nRet = EBADSTATE;
      }      
    }//if(pStream)    
  }//if(errorCode == E_SUCCESS)  
  return nRet;
}
#endif

extern "C" OEMHandle OEMMediaMPEG42PV_GetOEMLayer(OEMHandle pOEM)
{
  int i;
  for (i=MAX_NUM_OEM_PLAYBACK_INST-1; i>=0; i--) 
  {
    if (g_pOEMLayer[i])
    {
      if (pOEM == g_pOEMLayer[i])
      {
        return pOEM;
      }
      if (!pOEM) 
      {
        pOEM = g_pOEMLayer[i];
        return pOEM;
      }
    }
  }
  pOEM = NULL;
  return pOEM;                 // only reachable if old handle has been used
}

/*!
  @brief
  Set ISound method
  
  @detail
  Get from QtvPlayer the audio codect type and set ISound method based on the type.

  @return
  Indication of success

  @retval SUCCESS AEESoundMethod other than AEE_SOUND_METHOD_UNKNOWN is set successfully.  and 
  @retval EFAILED AEESoundMethod is either AEE_SOUND_METHOD_UNKNOWN or it is not set successfully.
*/
int OEMMediaMPEG42PV_SetSoundMethod(OEMHandle pOEM)
{
  AEESoundInfo          si;
  AEESoundMethod        eMethod;
  QtvPlayer::CodecType  codecType;

  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);

  // Need to provide valid OEMhandle, return if pOEM is NULL
  if(!pOEM)
  {
    MSG_ERROR("OEMMediaMPEG42PV_SetSoundMethod, pOEM passed in is NULL.", 0, 0, 0);
    return EFAILED;
  }

  if (!pOEM->m_pISound)
    return EUNSUPPORTED;

  ISOUND_Get(pOEM->m_pISound, &si);  
  
  codecType = QtvPlayer::GetAudioCodecType(pOEM->m_pHandle);
  switch(codecType)
  {
    case QtvPlayer::QTV_AAC_CODEC:
#ifdef FEATURE_QTV_BSAC
#error code not present
#endif /* FEATURE_QTV_BSAC */
    case QtvPlayer::QTV_COOK_CODEC:
    case QtvPlayer::QTV_SIPR_CODEC:
#ifdef FEATURE_MP4_MP3
    case QtvPlayer::QTV_MP3_CODEC:
#endif /* FEATURE_MP4_MP3 */

#ifdef FEATURE_WMA
     case QtvPlayer::QTV_WMA_CODEC:
#endif /* FEATURE_WMA */
#if defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER)
     case QtvPlayer::QTV_WMA_PRO_CODEC:     
     case QtvPlayer::QTV_WMA_PRO_PLUS_CODEC:     
#endif /* defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER) */
#ifdef FEATURE_QTV_3GPP_AMR_WB
#error code not present
#endif /* FEATURE_QTV_3GPP_AMR_WB */
#ifdef FEATURE_QTV_3GPP_AMR_WB_PLUS
#error code not present
#endif /* FEATURE_QTV_3GPP_AMR_WB_PLUS */
#ifdef FEATURE_QTV_3GPP_EVRC_NB
#error code not present
#endif /* FEATURE_QTV_3GPP_EVRC_NB */
#ifdef FEATURE_QTV_3GPP_EVRC_WB
#error code not present
#endif /* FEATURE_QTV_3GPP_EVRC_WB */
#ifdef FEATURE_QTV_AVI_AC3    
#error code not present
#endif /*  FEATURE_QTV_AVI_AC3 */
#ifdef FEATURE_QTV_PCM
#error code not present
#endif /* FEATURE_QTV_PCM  */
#ifdef FEATURE_AUDIO_FORMAT
      // snd_method_type is SND_METHOD_MIDI, map to AEESoundMethod based on OEMSound_Init.
      eMethod = AEE_SOUND_METHOD_MIDI;
#else
      //snd_method_type is SND_METHOD_MAX, map to AEESoundMethod based on OEMSound_Init.
     eMethod = AEE_SOUND_METHOD_MIDI;
#endif
      break;

    case QtvPlayer::QTV_EVRC_CODEC:
    case QtvPlayer::QTV_QCELP_CODEC:
    case QtvPlayer::QTV_GSM_AMR_CODEC:
    case QtvPlayer::QTV_MPEG4_CODEC:
    case QtvPlayer::QTV_H263_CODEC:
    case QtvPlayer::QTV_STILL_IMAGE_CODEC:
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    case QtvPlayer::QTV_TIMED_TEXT_CODEC:
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

      //snd_method_type is SND_METHOD_VOICE, map to AEESoundMethod based on OEMSound_Init.
      eMethod = AEE_SOUND_METHOD_VOICE;
      break;

    case QtvPlayer::QTV_UNKNOWN_CODEC :
    default:
      return EFAILED;
  }

  si.eMethod = eMethod;
  if(ISOUND_Set(pOEM->m_pISound, &si) == SUCCESS)
  {
    pOEM->m_Sound.method_set = TRUE;
    return SUCCESS;
  }
  return EFAILED;
}

/*!
  @brief Sets the playback speed of the next playback only.

  @param pbSpeed  The desired playback speed
  @param pOEM     Handle to the OEM layer.
  
  @return Indication of success (SUCCESS or EFAILED)
*/   
int OEMMediaMPEG42PV_SetPlaybackSpeed(AEEMediaPlaybackSpeedType pbSpeed, OEMHandle pOEM)
{
  QtvPlayer::ReturnT nRet = QtvPlayer::QTV_RETURN_ERROR;
  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);
  QtvPlayer::PlaybackSpeedT  qtvSpeed = QtvPlayer::PLAYBACK_SPEED_NORMAL;

  switch( pbSpeed )
  {
    case MM_QTV_PLAYBACK_SPEED_1P3X:
      qtvSpeed = QtvPlayer::PLAYBACK_SPEED_1P3X;
      break;
      
    case MM_QTV_PLAYBACK_SPEED_NORMAL:  /* Fall through */
    default:
      qtvSpeed = QtvPlayer::PLAYBACK_SPEED_NORMAL;
      break;
  }
  
  if (pOEM)
  {
    // Since the values match above, just cast
    nRet = QtvPlayer::SetPlaybackSpeed( qtvSpeed, pOEM->m_pHandle );
  }

  return (nRet == QtvPlayer::QTV_RETURN_OK) ? SUCCESS : EFAILED;
}

#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
/*!
  @brief Sets the dual mono output configuration.

  @param pOEM Handle to the OEM layer.
  
  @return Indication of success (SUCCESS or EFAILED)
*/
int OEMMediaMPEG42PV_SetDualMonoOutput(OEMHandle pOEM)
{
  QtvPlayer::ReturnT nRet = QtvPlayer::QTV_RETURN_ERROR;
  QtvPlayer::DualMonoOutputT qtvDualMonoOutput;

  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);

  if (pOEM)
  {
    /* Switch to the next setting */
    switch (pOEM->m_dualMonoOutput)
    {
      case MM_DUAL_MONO_OUTPUT_MAIN_TO_L_AND_R:
      {
        pOEM->m_dualMonoOutput = MM_DUAL_MONO_OUTPUT_SUB_TO_L_AND_R; 
        qtvDualMonoOutput = QtvPlayer::DUAL_MONO_OUTPUT_SUB_TO_L_AND_R;
        break;
      }

      case MM_DUAL_MONO_OUTPUT_SUB_TO_L_AND_R:
      {
        pOEM->m_dualMonoOutput = MM_DUAL_MONO_OUTPUT_MAIN_TO_L_SUB_TO_R;
        qtvDualMonoOutput = QtvPlayer::DUAL_MONO_OUTPUT_MAIN_TO_L_SUB_TO_R;
        break;
      }

      case MM_DUAL_MONO_OUTPUT_MAIN_TO_L_SUB_TO_R:
      {
        pOEM->m_dualMonoOutput = MM_DUAL_MONO_OUTPUT_SUB_TO_L_MAIN_TO_R; 
        qtvDualMonoOutput = QtvPlayer::DUAL_MONO_OUTPUT_SUB_TO_L_MAIN_TO_R;
        break;
      }

      case MM_DUAL_MONO_OUTPUT_SUB_TO_L_MAIN_TO_R:
      {
        pOEM->m_dualMonoOutput = MM_DUAL_MONO_OUTPUT_MAIN_TO_L_AND_R;
        qtvDualMonoOutput = QtvPlayer::DUAL_MONO_OUTPUT_MAIN_TO_L_AND_R;
        break;
      }

      default:
      {
        MSG_ERROR("SetDualMonoOutput: value invalid, using default", 0, 0, 0);
        pOEM->m_dualMonoOutput = MM_DUAL_MONO_OUTPUT_MAIN_TO_L_AND_R;
        qtvDualMonoOutput = QtvPlayer::DUAL_MONO_OUTPUT_MAIN_TO_L_AND_R;
        break;
      }
    } /* switch (pOEM->m_dualMonoOutput) */

    nRet = QtvPlayer::SetDualMonoOutput(qtvDualMonoOutput, pOEM->m_pHandle);
  }

  if (nRet == QtvPlayer::QTV_RETURN_OK)
  {
    return SUCCESS;
  }
  else
  {
    return EFAILED;
  }
}
#endif /* FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION */

#ifdef FEATURE_USE_CLKREGIM_DEVMAN_REV2
void OEMMediaMPEG42PV_ClkrgmRelease( OEMHandle pOEM )
{
  if ( pOEM->m_bClkrgmIsReg ) 
  {
    clk_regime_devman_deregister( &g_ClkrgmDmClient );
    pOEM->m_bClkrgmIsReg = FALSE;
  }
}

void OEMMediaMPEG42PV_ClkrgmIncrease( OEMHandle pOEM )
{
  if ( !pOEM->m_bClkrgmIsReg ) 
  {
     clk_regime_devman_register( &g_ClkrgmDmClient, CLKRGM_DEVMAN_CLIENT_GROUP_DEFAULT);
     clk_regime_devman_request( &g_ClkrgmDmClient, CLKRGM_DEVICE_PMDH, 
                                CLKRGM_DEVICE_REQUEST_SET_MINIMUM_SPEED, &g_ClkrgmDevParam);
     pOEM->m_bClkrgmIsReg = TRUE;
  }
}
#endif /* FEATURE_USE_CLKREGIM_DEVMAN_REV2 */


/*!
  @brief 
  Registers DRM decryption method.

  @detail
  OEM layer should provide a DRM decryption method to allow encrypted WM 
  files be sent to Qtv for playback

  @param pFunction  Pointer to decryption function
  @param pData      Pointer to client data 
  @param pOEM       Handle to the OEM layer.
  
  @return Indication of success (SUCCESS or EFAILED)

  @retval SUCCESS Registering DRM decryption method succeeded
  @retval EFAILED Registering DRM decryption method failed
*/   
int OEMMediaMPEG42PV_RegisterDRMDecryptMethod(AEEMediaDRMDecryptMethodT pFunction, void *pData, OEMHandle pOEM)
{
  QtvPlayer::ReturnT retVal = QtvPlayer::QTV_RETURN_ERROR;

  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);
  // return if pOEM is NULL
  if(pOEM == NULL)
  {
    MSG_ERROR("OEMMediaMPEG42PV_RegisterDRMDecryptMethod, pOEM is null.", 0, 0, 0);
    return EFAILED;
  }

  if(pFunction)
  {
    retVal = QtvPlayer::RegisterDRMDecryptMethod((QtvPlayer::DRMDecryptMethodT)pFunction, pData, pOEM->m_pHandle);
  }

  if(retVal == QtvPlayer::QTV_RETURN_OK)
    return SUCCESS;

  return EFAILED;
}

/*!
  @brief 
  Retrieves frame rate and converts it from float to double type.

  @detail
  this function retrieves frame rate using OEMMediaMPEG42PV_GetVideoFrameRate and 
  converts it from float to double type

  @param pFrameRate pointor to frame rate variable.
  @param pOEM       Handle to the OEM layer.

  @return Indication of success (SUCCESS or EFAILED)

  @retval SUCCESS Getting frame rate succeeded
  @retval EFAILED Getting frame rate failed
*/   
int OEMMediaMPEG4Qtv_GetFrameRate(double* pFrameRate, OEMHandle pOEM)
{
  int nRet = EFAILED;
  float frameRate;

  if (NULL == pOEM) 
  {
    MSG_ERROR("OEMMediaMPEG4Qtv_GetFrameRate, pOEM is NULL.", 0, 0, 0);
    return nRet;
  }

  if(SUCCESS == (nRet = OEMMediaMPEG42PV_GetVideoFrameRate((float *)&frameRate, pOEM)))
  {
    *pFrameRate = (double)frameRate;
  }
  else
  {
    MSG_ERROR("failed to retrieve frame rate, nRet = %d", nRet, 0, 0);
  }

  return nRet;
}

