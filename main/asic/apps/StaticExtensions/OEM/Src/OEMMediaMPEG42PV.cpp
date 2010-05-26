/*=========================================================================

FILE: OEMMediaMPEG42PV.cpp

SERVICES: IMEDIA MPEG4 to PV interface implementation

DESCRIPTION: This is a think interface between the IMedia interface for
             MPEG4 and the Packet Video (PV) decoder

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

Copyright: 1999-2004 QUALCOMM Incorporated. All Rights Reserved.
QUALCOMM Proprietary/GTDR
=========================================================================*/
/*=========================================================================
                             Edit History

$Header: //depot/asic/msmshared/services/Qtv/QTV.04.04/StaticExtensions/OEM/Src/OEMMediaMPEG42PV.cpp#31 $
$DateTime: 2006/06/29 01:53:08 $
$Change: 345597 $

=========================================================================*/

/*=========================================================================

                                 INCLUDE FILES

=========================================================================*/

extern "C"
{
  #include "assert.h"
  #include "snd.h"
  #include "clk.h"
  #include "uixsnd.h"
#if defined(FEATURE_QTV_IPL_SCALING) || defined(FEATURE_QTV_PP_ON_ARM)
  #include "qtv_ipl.h"
#endif /* FEATURE_QTV_IPL_SCALING */
}
#include "customer.h"
#include "task.h"

#ifdef   FEATURE_APP_MPEG4
#include "BREWVersion.h"
#include "rex.h"

#include "QtvPlayer.h"
#include "OEMFeatures.h"
#include "OEMMedia.h"
#include "AEEMediaFormats.h"
#include "AEEMediaMPEG4.h"
#include "OEMOS.h"
#include "OEMMediaMPEG4.h"
#if MIN_BREW_VERSION(3,0)
extern "C" {
# include "OEMBitmapFuncs_priv.h"
}
# include "AEE_OEMHeap.h"
# include "OEMHeap.h"
#endif /* MIN_BREW_VERSION(3,0) */
#include "OEMBitmap_priv.h"
#include "AEEFile.h"

#if defined(PLATFORM_LTK) && defined(FEATURE_MP4_COMPLIANCE_TESTING)
#error code not present
#endif

/*=========================================================================

                               MACRO DEFINITIONS

=========================================================================*/

#define FEATURE_MP4_ENABLE_SET_DEBUG_VARIABLES
#define FEATURE_MP4_WRITE_CLIP_STATS_TO_FILE

#ifdef FEATURE_MP4_ENABLE_SET_DEBUG_VARIABLES
#define DEBUG_MAX_STRING_LENGTH        256
#define DEBUG_URL_FILE_EXTENSION       ".url"
#define DEBUG_VARIABLE_VALUE_SEPARATOR '='
#define DEBUG_URL_FILE_TOKEN           "debug:"
#endif // FEATURE_MP4_ENABLE_SET_DEBUG_VARIABLES

/* How long to wait for the player's state to go to IDLE */
#define PLAYER_TRANSITION_TO_IDLE_TIMEOUT 10000 /* in ms */

/* How often to poll the player for its state to go to IDLE */
#define PLAYER_STATE_POLL_INTERVAL          200 /* in ms */

/*=========================================================================

                             EXTERNAL DECLARATIONS

=========================================================================*/
#ifdef FEATURE_QTV_IPL_SCALING
static void ScaleFrame( QtvPlayer::FrameInfoT*  pFrameInfo );
#endif /* FEATURE_QTV_IPL_SCALING */
#ifdef FEATURE_QTV_PP_ON_ARM
/* Qtv IPL to support color conversion & rotation */
static void RotateColorConvert( QtvPlayer::FrameInfoT* pFrameInfo );
#endif /* FEATURE_QTV_PP_ON_ARM */
extern "C" int OEMMediaMPEG42PV_GetVolume();
extern "C" int OEMMediaMPEG42PV_SetVolume(int32 volume);

extern "C" int OEMMediaMPEG4_QueueCallback( void *pClientData,
                                            int nStatus,
                                            int nCmd,
                                            int nSubCmd,
                                            void *pData,
                                            uint32 dwSize );
#ifndef FEATURE_QTV_VIDEO_DISABLE
extern "C" int OEMMediaMPEG42PV_GetFrame(void **ppFrame);
#endif

/*=========================================================================

                              FORWARD DECLARATIONS

=========================================================================*/
static void UpdateProgressBar(void *pData);
extern boolean AllocateOutputBuffers(MP4BufType *pBuf, uint16 Height, uint16 Width);
extern void FreeOutputBuffers(MP4BufType *pBuf);

#ifdef FEATURE_QTV_MDP
extern "C" void mp4_free_frame_buffer(void *pFrameBuf);
#endif // FEATURE_QTV_MDP

#ifdef FEATURE_MP4_ENABLE_SET_DEBUG_VARIABLES
static int CheckDebugURLFile(char *videoFileName, char *audioFileName);
#endif // FEATURE_MP4_ENABLE_SET_DEBUG_VARIABLES


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
#ifdef FEATURE_MP4_WRITE_CLIP_STATS_TO_FILE
extern "C"
{
  char mp4WriteClipStatsFileName[AEE_MAX_FILE_NAME] = "";
  uint32 mp4WriteClipStatsToFileFlag = 0;
}
#endif // FEATURE_MP4_WRITE_CLIP_STATS_TO_FILE

#ifdef FEATURE_QTV_PSEUDO_STREAM
#error code not present
#endif

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

extern "C" int  OEMMediaMPEG42PV_Seek(AEEMediaSeek eSeek, int32 TimeOrFrame);
extern "C" int  OEMMediaMPEG42PV_SetStartPos(uint32 lTimeMS);

static void *pCallbackData = NULL;
static uint32 prerollPercentage = 0;
static volatile QtvPlayer::PlayerStatusT MPEG4EngineState = QtvPlayer::QTV_PLAYER_STATUS_IDLE;
static QtvPlayer::ClipInfoT clipInfo;
static AEEMediaMPEG4Spec MPEG4Spec;
static boolean clipInfoAvail = FALSE;
typedef enum {
  NO_VOLUME_CB,
  SET_VOLUME_CB,
  GET_VOLUME_CB
} VolumeCBType;
static int32 savedVolume = 0;
static VolumeCBType generateVolumeCB = NO_VOLUME_CB;
static boolean generateTotalTimeCB = FALSE;
static boolean generateMediaSpecCB = FALSE;
static IShell   *pIShell;
static IBitmap  *pIBitmap;
static boolean streamingError;
static int startTime;
static boolean repositionInPause;
static boolean repositionInPauseUpdateTime= FALSE;  // Variable used to update time display when repositioned in Pause
static boolean paused;
static boolean playStarted;
static boolean seekPending;
#ifdef FEATURE_QTV_IPL_SCALING
static AEEMediaMPEG4ScalingType IPLScalingFactor;
static unsigned char * pIPLScalingBuf;
#endif /* FEATURE_QTV_IPL_SCALING */
#ifdef FEATURE_QTV_PP_ON_ARM
/* This variable is used to pass the rotation factor to the QTV IPL rotation function */
static AEEMediaMPEG4RotationType IPLRotationFactor;
#endif /* FEATURE_QTV_PP_ON_ARM */
static void CopyMPEG4SpecFromClipInfo(AEEMediaMPEG4Spec * MPEG4Spec, QtvPlayer::ClipInfoT *pClipInfo);
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
static AEE3GPPTimedText TimedText3GPP;
static bool OEMMediaMPEG42PV_PrepareTimedText(void);
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
static AEETelopTextElement    TelopTextElement;
static AEETelopTextSubString  TelopTextSubString;
#endif /* FEATURE_MP4_KDDI_TELOP_TEXT */
static char videoURN[256];
static char audioURN[256];
static uint32 dwTickInterval = MM_DEFAULT_TICK_INTERVAL;

static unsigned char * pVideoBuffer;
static unsigned char * pAudioBuffer;
static uint32          VideoBufSize;
static uint32          AudioBufSize;

#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
static boolean bFrameTransformPending = FALSE;
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */

static MP4BufType OutputBuffers;

static AEEMediaMP4CodecType OEMMediaMPEG42PV_GetAudioCodecType(void);
#ifndef FEATURE_QTV_VIDEO_DISABLE
#ifdef FEATURE_QTV_PP_ON_ARM
static AEEMediaMP4CodecType OEMMediaMPEG42PV_GetVideoCodecType(void);
#endif
#endif /* FEATURE_QTV_VIDEO_DISABLE */
static AEEMediaMPEG4Type OEMMediaMPEG42PV_ConvertMediaTypePVToAEE(QtvPlayer::MediaTypeT pvType);
static AEEStreamType OEMMediaMPEG42PV_ConvertStreamType(QtvPlayer::StreamTypeT);

#ifdef FEATURE_QTV_STREAM_RECORD
#error code not present
#endif

#define MASK_24BIT_BOUNDARY 0xFF000000
#define CROSSES_24BIT_BOUNDARY(ptr, n) \
  (((int)(ptr) & MASK_24BIT_BOUNDARY) != (((int)(ptr) + (n) - 1) & MASK_24BIT_BOUNDARY))


extern "C" void QtvEngineCB(
  QtvPlayer::PlayerStatusT status,
  void *pData,
  QtvPlayer::PlayerStateRecordT state)
{
  int MMStatus = MM_STATUS_UNKNOWN;
  int cmd = 0;
  unsigned long data = 0;
  unsigned long size = 0;

  MSG_LOW("QtvEngineCB(%d,%d,%d)", status, pData, state);

  switch (status)
  {
#ifdef FEATURE_QTV_STREAMING
#error code not present
#endif /* FEATURE_QTV_STREAMING */

    case QtvPlayer::QTV_COMMAND_PLAY_CLIP_COMPLETE:
      if (!playStarted)
      {
        cmd = MM_CMD_PLAY;
        MMStatus = MM_STATUS_START;
        playStarted = TRUE;
      }
      seekPending = FALSE;
      break;

    case QtvPlayer::QTV_COMMAND_PLAY_CLIP_FAILED:
      seekPending = FALSE;
      break;

#ifdef FEATURE_QTV_RANDOM_ACCESS_REPOS
#error code not present
#endif /*FEATURE_QTV_RANDOM_ACCESS_REPOS*/

    case QtvPlayer::QTV_PLAYER_STATUS_PLAYBACK_COMPLETE:
      startTime = 0;
#if defined(PLATFORM_LTK) && defined (FEATURE_MP4_COMPLIANCE_TESTING)
#error code not present
#endif
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_PLAYBACK_STOPPED:
      startTime = 0;
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_PLAYBACK_ERROR_ABORT:
      startTime = 0;
      MMStatus = MM_STATUS_ABORT;
      cmd = MM_CMD_PLAY;
      data = status;
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_OPENING:
       MPEG4EngineState = status;
       break;

#ifdef FEATURE_QTV_STREAMING
#error code not present
#endif /* FEATURE_QTV_STREAMING */

    case QtvPlayer::QTV_PLAYER_STATUS_BUFFERING:
      MPEG4EngineState = status;
      prerollPercentage =  *((uint32 *)pData);
      UpdateProgressBar(NULL);
      break;


#ifdef FEATURE_QTV_PSEUDO_STREAM
#error code not present
#endif


    case QtvPlayer::QTV_PLAYER_STATUS_PLAYBACK_READY:
      MPEG4EngineState = status;
      if (QtvPlayer::GetClipInfo(clipInfo) == QtvPlayer::QTV_RETURN_OK)
      {
        CopyMPEG4SpecFromClipInfo(&MPEG4Spec, &clipInfo);
        clipInfoAvail = TRUE;
      }
      else
      {
        MEMSET(&clipInfo, 0, sizeof(clipInfo));
        clipInfoAvail = FALSE;
      }

      if (generateTotalTimeCB == TRUE)
      {
        int     totalTimeStatus = MM_STATUS_ABORT;
        uint32  duration = 0;

        generateTotalTimeCB = FALSE;
        if (clipInfoAvail)
        {
          totalTimeStatus = MM_STATUS_DONE;
          duration = MPEG4Spec.dwDuration;
        }

        OEMMediaMPEG4_QueueCallback((void *)pData,
                                    totalTimeStatus,
                                    MM_CMD_GETTOTALTIME,
                                    0,
                                    (void *)duration,
                                    sizeof(duration));
      }

      if (generateMediaSpecCB == TRUE)
      {
        int mediaSpecStatus;

        generateMediaSpecCB = FALSE;
        mediaSpecStatus = (clipInfoAvail ? MM_STATUS_DONE : MM_STATUS_ABORT);

        OEMMediaMPEG4_QueueCallback((void *)pData,
                                    mediaSpecStatus,
                                    MM_CMD_GETMEDIAPARM,
                                    MM_MP4_PARM_MEDIA_SPEC,
                                    (void *)&MPEG4Spec,
                                    sizeof(MPEG4Spec));
      }

      if (generateVolumeCB != NO_VOLUME_CB)
      {
        if( generateVolumeCB == GET_VOLUME_CB )
        {
          if( !clipInfoAvail )
            {
              OEMMediaMPEG4_QueueCallback((void*)pCallbackData,
                      MM_STATUS_ABORT,
                      MM_CMD_GETMEDIAPARM,
                      MM_PARM_VOLUME,
                      NULL,
                      0);
            }
            else
            {
              OEMMediaMPEG42PV_GetVolume();
            }
          }
          else
          {
          if( !clipInfoAvail )
            {
              OEMMediaMPEG4_QueueCallback((void*)pCallbackData,
                                                  MM_STATUS_ABORT,
                                                  MM_CMD_SETMEDIAPARM,
                                                  MM_PARM_VOLUME,
                                                  NULL,
                                                  0);
            }
            else
            {
              OEMMediaMPEG42PV_SetVolume(savedVolume);
            }
          }
          generateVolumeCB = NO_VOLUME_CB;
      }

      // Indicate transition to PB Ready state to the user -(ie no longer Connecting)
      OEMMediaMPEG4_QueueCallback((void *)pCallbackData,
                                  MM_MP4_STATUS_PB_READY,
                                  MM_CMD_PLAY,
                                  0,
                                  (void *)NULL,
                                  0);

      cmd = MM_CMD_PLAY;
      MMStatus = MM_STATUS_REPOSITIONING;
      data = clipInfo.RepositioningAllowed;
      size = sizeof(data);
      streamingError = FALSE;
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_PLAYING:
      MPEG4EngineState = status;
      UpdateProgressBar(NULL);
      cmd = MM_CMD_PLAY;
      if (paused == TRUE)
      {
        MMStatus = MM_STATUS_RESUME;
        paused = FALSE;
      }
      else
      {
        MMStatus = MM_MP4_STATUS_PLAYING;
      }
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_PAUSED:
      startTime = state.playbackMsec;
      MPEG4EngineState = status;
      cmd = MM_CMD_PLAY;
      MMStatus = MM_STATUS_PAUSE;
      paused = TRUE;
      break;


    case QtvPlayer::QTV_PLAYER_STATUS_REPOSITIONING:
      MPEG4EngineState = status;
      if (paused == FALSE)
      {
        cmd = MM_CMD_PLAY;
        MMStatus = MM_STATUS_SEEK;
      }
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_NO_SYNC_FRAME_ERROR:
      MPEG4EngineState = status;
      if (paused == FALSE)
      {
        MSG_ERROR("No Sync Frames. Repositioning failed.", 0, 0, 0);
        cmd = MM_CMD_PLAY;
        MMStatus = MM_MP4_STATUS_NO_SYNC_FRAME;
      }
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_IDLE:
      startTime = 0;
      MPEG4EngineState = status;
      paused = FALSE;
      playStarted = FALSE;
      seekPending = FALSE;
      /* tell user that player has done playing */
      cmd = MM_CMD_PLAY;
      MMStatus = MM_STATUS_DONE;
#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
      bFrameTransformPending = FALSE;
#endif
#ifdef FEATURE_QTV_IPL_SCALING
      IPLScalingFactor = MM_MPEG4_NO_SCALING;
#endif /* FEATURE_QTV_IPL_SCALING */

#ifdef FEATURE_QTV_PP_ON_ARM
      /* Initialized to no rotation */
      IPLRotationFactor = MM_MPEG4_NO_ROTATION;
#endif /* FEATURE_QTV_PP_ON_ARM */

      /* Restore sound device setting after playback.  Note: we don't necessarily
       * want to mute the sound device after playback.  This is because Qtv may
       * have been launched while a voice call is up and so when the playback is
       * over we want to resume the call and still hear voice.
       * akak's comment:for any target who wants to use the old API will not need to 
       * define UI_SND_DEV_MGMT
       */
#ifdef UI_SND_DEV_MGMT
    uisnd_set_device_auto(NULL,NULL);
#else
    uisnd_set_device( SND_MUTE_UNMUTED, SND_MUTE_UNMUTED, FALSE );
#endif
     break;

    case QtvPlayer::QTV_PLAYER_STATUS_AUDIO_ERROR:
    case QtvPlayer::QTV_PLAYER_STATUS_AUDIO_ERROR_FORMAT_NOT_SUPPORTED:
      cmd = MM_CMD_PLAY;
      MMStatus = MM_MP4_STATUS_AUDIO_ABORT;
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_VIDEO_ERROR:
    case QtvPlayer::QTV_PLAYER_STATUS_VIDEO_ERROR_FORMAT_NOT_SUPPORTED:
      cmd = MM_CMD_PLAY;
      MMStatus = MM_MP4_STATUS_VIDEO_ABORT;
      break;

      /* The PV code never sends this error! */
    case QtvPlayer::QTV_PLAYER_STATUS_STREAM_ERROR:
    case QtvPlayer::QTV_PLAYER_STATUS_STREAM_SERVER_CLOSED:
      streamingError = TRUE;
      /* Fall through */
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
      break;

    case QtvPlayer::QTV_PLAYER_STATUS_PREV_FRAME_FAILED:
      cmd = MM_CMD_PLAY;
      MMStatus = MM_STATUS_SEEK_FAIL;
      data = MM_SEEK_MODE_FRAME;
      size = sizeof(data);
      break;

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    case QtvPlayer::QTV_PLAYER_STATUS_NEXT_TEXT_READY:
      if( OEMMediaMPEG42PV_PrepareTimedText() )
      {
        cmd = MM_CMD_PLAY;
        MMStatus = MM_MP4_STATUS_3GPP_TTEXT;
        data = (uint32)&TimedText3GPP;
        size = sizeof(TimedText3GPP);
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
          TelopTextElement.dwTelopSize          = telopElement.TelopSize;
          TelopTextElement.dwBeginTime          = telopElement.BeginTime;
          TelopTextElement.dwDuration           = telopElement.Duration;
          TelopTextElement.dwNumberOfSubStrings = telopElement.NumberOfSubStrings;
          TelopTextElement.bWrapFlag            = telopElement.WrapFlag;
          TelopTextElement.dwBackgroundColor    = telopElement.BackgroundColor;
          TelopTextElement.dwDefaultFontColor   = telopElement.DefaultFontColor;
          /* send command to media player */
          cmd = MM_CMD_PLAY;
          MMStatus = MM_MP4_STATUS_TELOP_TEXT;
          data = (uint32)&TelopTextElement;
          size = sizeof(TelopTextElement);
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
    case QtvPlayer::QTV_PLAYER_STATUS_ROTATION_DONE:
    case QtvPlayer::QTV_PLAYER_STATUS_ROTATION_FAILED:
    case QtvPlayer::QTV_PLAYER_STATUS_SCALING_DONE:
    case QtvPlayer::QTV_PLAYER_STATUS_SCALING_FAILED:
      bFrameTransformPending = FALSE;
      break;
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */

    case QtvPlayer::QTV_COMMAND_OPEN_URN_COMPLETE:
      cmd = MM_CMD_PLAY;
      MMStatus = MM_MP4_STATUS_OPEN_COMPLETE;
      break;

#ifdef FEATURE_QTV_PSEUDO_STREAM
#error code not present
#endif /*FEATURE_QTV_PSEUDO_STREAM */

    case QtvPlayer::QTV_COMMAND_OPEN_URN_FAILED:
      MSG_ERROR("Open URN Command Failed.", 0, 0, 0);
      break;

#ifdef FEATURE_QTV_PSEUDO_STREAM
#error code not present
#endif /* FEATURE_QTV_PSEUDO_STREAM */

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
#error code not present
#endif /* FEATURE_QTV_STREAM_RECORD */

#ifdef FEATURE_QTV_PSEUDO_STREAM
#error code not present
#endif /*FEATURE_QTV_PSEUDO_STREAM*/

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
#error code not present
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */

    default:
      break;
  }

  if ((MMStatus != MM_STATUS_UNKNOWN) && (cmd != 0)) //no point in queuing if no cmd
  {
    OEMMediaMPEG4_QueueCallback((void *)pData, MMStatus, cmd, 0, (void *)data, size);
  }
}


static void UpdateProgressBar(void * /*pData*/)
{
   QtvPlayer::PlayerStateRecordT state = {0};
   QtvPlayer::ReturnT stateVal = QtvPlayer::QTV_RETURN_ERROR;

   stateVal = QtvPlayer::GetPlayerState(state);

   if (stateVal == QtvPlayer::QTV_RETURN_OK && 
       MPEG4EngineState == QtvPlayer::QTV_PLAYER_STATUS_PLAYING)
   {
      if(repositionInPauseUpdateTime != TRUE) // Check if repositioned during Pause
      {
         /* we should send some positive value to MM_STATUS_TICK_UPDATE,
          * so application will assume it has correct value
          */

         if(!state.playbackMsec)
         {
           state.playbackMsec = 1;
         }

         OEMMediaMPEG4_QueueCallback((void *)pCallbackData,
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

        OEMMediaMPEG4_QueueCallback( (void *)pCallbackData,
                                     MM_STATUS_TICK_UPDATE,
                                     MM_CMD_PLAY,
                                     0,
                                      (void *)startTime,
                                      sizeof(startTime));

        repositionInPauseUpdateTime = FALSE;
      }

      ISHELL_SetTimer( pIShell,
                       dwTickInterval,
                       UpdateProgressBar,
                       NULL );
   }
   else
   {
      if ( stateVal == QtvPlayer::QTV_RETURN_OK &&
           (MPEG4EngineState == QtvPlayer::QTV_PLAYER_STATUS_BUFFERING))
        
      {
         OEMMediaMPEG4_QueueCallback((void *)pCallbackData,
                                     MM_MP4_STATUS_BUFFER_UPDATE,
                                     MM_CMD_PLAY,
                                     0,
                                     (void *)&prerollPercentage,
                                     sizeof(prerollPercentage));       
      }
      else
      {
        if((MPEG4EngineState == QtvPlayer::QTV_PLAYER_STATUS_CONNECTING) ||
            (MPEG4EngineState == QtvPlayer::QTV_PLAYER_STATUS_OPENING))
        {
          OEMMediaMPEG4_QueueCallback( (void *)pCallbackData,
                                       MM_MP4_STATUS_CONNECTION_UPDATE,
                                       MM_CMD_PLAY,
                                       0,
                                       (void *)NULL,
                                       0 );
        }

#ifdef FEATURE_QTV_PSEUDO_STREAM
#error code not present
#endif

      }
   }
}

#ifdef FEATURE_QTV_STREAMING
#error code not present
#endif /* FEATURE_QTV_STREAMING */


extern "C" int OEMMediaMPEG42PV_GetClipInfo(void)
{
   if (clipInfoAvail == TRUE)
   {
      OEMMediaMPEG4_QueueCallback((void *)pCallbackData,
                                  MM_STATUS_DONE,
                                  MM_CMD_GETTOTALTIME,
                                  0,
                                  (void *)MPEG4Spec.dwDuration,
                                  sizeof(MPEG4Spec.dwDuration));
   }
   else
   {
      generateTotalTimeCB = TRUE;
   }
   if (streamingError)
   {
      return EFAILED;
   }
   return SUCCESS;
}


extern "C" int OEMMediaMPEG42PV_Init(void)
{
  QtvPlayer::ReturnT retVal;
  int      rv;
  IDIB     *pDIB;
  IBitmap  *pIBitmapTemp;

  /* Get a copy of the AEE Shell */
  pIShell = AEE_GetShell();

  /* Create an IBitmap in which to send our frames to the app */
  /* The goal is to create an IBitmap without allocating memory for the raw
  * bitmap.  To do this, OEMBitmap_New needs a pointer to the memory.  Thus
  * create a temporary small bitmap to use as a parent, and release it when done.
  */
#ifdef FEATURE_LCD_18BPP_EMULATION
  rv = OEMBitmap18_New( 4, 4, NULL, &pIBitmapTemp );
  if( rv != SUCCESS )
  {
    MSG_ERROR("OEMBitmap18_New failed (%d)", rv, 0, 0);
    return EFAILED;
  }
  rv = OEMBitmap18_New( MP4_MAX_DECODE_WIDTH, MP4_MAX_DECODE_HEIGHT, pIBitmapTemp, &pIBitmap );
  if( rv != SUCCESS )
  {
    IBITMAP_Release(pIBitmapTemp);
    MSG_ERROR("OEMBitmap18_New failed (%d)", rv, 0, 0);
    return EFAILED;
  }
#else
  rv = OEMBitmap16_New( 4, 4, NULL, &pIBitmapTemp );
  if( rv != SUCCESS )
  {
    MSG_ERROR("OEMBitmap16_New failed (%d)", rv, 0, 0);
    return EFAILED;
  }
  rv = OEMBitmap16_New( MP4_MAX_DECODE_WIDTH, MP4_MAX_DECODE_HEIGHT, pIBitmapTemp, &pIBitmap );
  if( rv != SUCCESS )
  {
    IBITMAP_Release(pIBitmapTemp);
    MSG_ERROR("OEMBitmap16_New failed (%d)", rv, 0, 0);
    return EFAILED;
  }
#endif  /* FEATURE_LCD_18BPP_EMULATION */

  /* Clear the raw buffer information */
  rv = IBITMAP_QueryInterface(pIBitmap, AEECLSID_DIB, (void**)&pDIB);
  if( rv != SUCCESS )
  {
    MSG_ERROR("QueryInterface failed (%d)", rv, 0, 0);
    ISHELL_Release(pIShell);
    IBITMAP_Release(pIBitmap);
    IBITMAP_Release(pIBitmapTemp);
    return EFAILED;
  }
  pDIB->pBmp = NULL;
  IDIB_Release(pDIB);
  IBITMAP_Release(pIBitmapTemp);

  /* Initialize state parameters */
  streamingError = FALSE;
  startTime = 0;
  paused = FALSE;
  playStarted = FALSE;
  seekPending = FALSE;
  
  // Initialize the Qtv Player
  retVal = QtvPlayer::Init();
  if(retVal == QtvPlayer::QTV_RETURN_OK)
  {
    return SUCCESS;
  }
  else
  {
    MSG_ERROR("QtvPlayer::Init failed, return val=%d", retVal, 0, 0);
    ISHELL_Release(pIShell);
    IBITMAP_Release(pIBitmap);
    return EFAILED;
  }
}

extern "C" int OEMMediaMPEG42PV_OpenBufferURN(  unsigned char *pVideoBuf, uint32 dwVideoBufSize,
                                                unsigned char *pAudioBuf, uint32 dwAudioBufSize )
{
  QtvPlayer::ReturnT retVal;

  if( (!pVideoBuf || !dwVideoBufSize) && (!pAudioBuf || !dwAudioBufSize) )
  {
    MSG_ERROR("No URN passed in OEMMediaMPEG42PV_OpenBufferURN", 0, 0, 0);
    return EFAILED;
  }

  /* reset any call backs pending */
  generateVolumeCB = NO_VOLUME_CB;
  generateTotalTimeCB = FALSE;
  generateMediaSpecCB = FALSE;

#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
  bFrameTransformPending = FALSE;
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */

#ifdef FEATURE_QTV_IPL_SCALING
  IPLScalingFactor = MM_MPEG4_NO_SCALING;
#endif /* FEATURE_QTV_IPL_SCALING */

#ifdef FEATURE_QTV_PP_ON_ARM
  /* Initialized to no rotation */
  IPLRotationFactor = MM_MPEG4_NO_ROTATION;
#endif /* FEATURE_QTV_PP_ON_ARM */

  /* since we are playing from buffer, set file names to NULL */
  videoURN[0] = '\0';
  audioURN[0] = '\0';

  pVideoBuffer = pVideoBuf;
  VideoBufSize = dwVideoBufSize;

  pAudioBuffer = pAudioBuf;
  AudioBufSize = dwAudioBufSize;

  retVal = QtvPlayer::OpenURN(pVideoBuffer, dwVideoBufSize, pAudioBuffer, dwAudioBufSize);
  if (retVal == QtvPlayer::QTV_RETURN_OK)
  {
    return SUCCESS;
  }
  else
  {
    return EFAILED;
  }
}

#if (defined (FEATURE_QTV_QOS_SELECTION)|| defined (FEATURE_QTV_PDP_SELECTION))
#error code not present
#endif

extern "C" int OEMMediaMPEG42PV_OpenURN(char *videoFileName, char *audioFileName)
{
  QtvPlayer::ReturnT retVal;
  repositionInPauseUpdateTime = FALSE;

#ifdef FEATURE_QTV_VIDEO_DISABLE
  //Turn off video if the Qtv video disable feature is enabled.
  videoFileName = NULL;
#endif

  if( (!audioFileName || !strlen(audioFileName)) 
#ifndef FEATURE_QTV_VIDEO_DISABLE
	  && (!videoFileName || !strlen(videoFileName))
#endif
     )
  {
    MSG_ERROR("No URN passed in OEMMediaMPEG42PV_OpenURN", 0, 0, 0);
    return EFAILED;
  }

  /* reset any call backs pending */
  generateVolumeCB = NO_VOLUME_CB;
  generateTotalTimeCB = FALSE;
  generateMediaSpecCB = FALSE;

#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
  bFrameTransformPending = FALSE;
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */

#ifdef FEATURE_QTV_IPL_SCALING
  IPLScalingFactor = MM_MPEG4_NO_SCALING;
#endif /* FEATURE_QTV_IPL_SCALING */

#ifdef FEATURE_QTV_PP_ON_ARM
  /* Initialized to no rotation */
  IPLRotationFactor = MM_MPEG4_NO_ROTATION;
#endif /* FEATURE_QTV_PP_ON_ARM */

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
       strcpy(mp4WriteClipStatsFileName, videoFileName);
    }
    else if (audioFileName && audioFileName[0] != '\0')
    {
       strcpy(mp4WriteClipStatsFileName, audioFileName);
    }
  }
#endif

  /* since we are playing from file, set playback buffers to NULL */
  pVideoBuffer = NULL;
  pAudioBuffer = NULL;
  VideoBufSize = AudioBufSize = 0;

   if(videoFileName)
     strncpy(videoURN, videoFileName, 256);
   else
    videoURN[0] = '\0';

   if(audioFileName)
     strncpy(audioURN, audioFileName, 256);
   else
    audioURN[0] = '\0';

   retVal = QtvPlayer::OpenURN(videoURN, audioURN);
   if (retVal == QtvPlayer::QTV_RETURN_OK)
   {
      return SUCCESS;
   }
   else
   {
      return EFAILED;
   }
}


extern "C" int OEMMediaMPEG42PV_Pause(void)
{
   QtvPlayer::ReturnT retVal;

   retVal = QtvPlayer::Pause();
   if (retVal == QtvPlayer::QTV_RETURN_OK)
   {
      return SUCCESS;
   }
   else
   {
      return EFAILED;
   }
}


extern "C" int OEMMediaMPEG42PV_Play(int startPos)
{
  QtvPlayer::ReturnT retVal = QtvPlayer::QTV_RETURN_ERROR;
  int startPlayPos = startTime;

  if(!paused)
    playStarted = FALSE;

  if (streamingError)
  {
    if( strlen(videoURN) || strlen(audioURN) )
      // Do nothing. mpeg4player thread already has the required URL data.
      // No need to issue PV_OPEN_URN event.
      ;
  }

  if (startPos == -1)
  {
    /* Resuming from pause */
    if (repositionInPause)
    {
      /* Repositioning occurred while paused */
      repositionInPause = FALSE;
    }
    else
    {
      startPlayPos = -1;
    }
  }
  else
  {
    // Do not allocate buffers for audio only clips
    if( (!clipInfoAvail) || (clipInfo.MediaType != QtvPlayer::MEDIA_AUDIO_ONLY))
    {
      unsigned int mp4_width = MP4_MAX_DECODE_WIDTH;
      unsigned int mp4_height = MP4_MAX_DECODE_HEIGHT;

      if( clipInfoAvail && (clipInfo.Width > 0) && (clipInfo.Height > 0))
      {
        mp4_width = clipInfo.Width;
        mp4_height = clipInfo.Height;
      }

      /* our DSP takes width only in multiple of 16 and if video width is not
         multiple of 16, DSP will round it to the next higher value. So we make
         sure that we are allocating sufficient buffer */
      if(( mp4_width % 16) != 0)
      {
        uint32 temp_width = (( mp4_width >> 4) + 1) << 4;
        MSG_HIGH("Non 16 Divisible Size %d Correcting to %d ...", mp4_width, temp_width, 0);
        mp4_width = temp_width;
      }

      /* our DSP takes height only in multiple of 16 and if video height is not
         multiple of 16, DSP will round it to the next higher value. So we make
         sure that we are allocating sufficient buffer */
      if(( mp4_height % 16) != 0)
      {
        uint32 temp_height = (( mp4_height >> 4) + 1) << 4;
        MSG_HIGH("Non 16 Divisible Size %d Correcting to %d ...", mp4_height, temp_height, 0);
        mp4_height = temp_height;
      }

      if( !AllocateOutputBuffers( &OutputBuffers,
              mp4_height,
              mp4_width ) )
      {
        MSG_ERROR("AllocateOutputBuffers failed", 0, 0, 0);
        return EFAILED;
      }
    }

    /* Set Qtv's output buffers */
    QtvPlayer::SetOutputBufferInfo(OutputBuffers);
  }

  /* If clip information is available, send a MEDIA_SPEC callback */
  if( clipInfoAvail )
  {
    OEMMediaMPEG4_QueueCallback((void *)pCallbackData,
            MM_STATUS_MEDIA_SPEC,
            MM_CMD_PLAY,
            0,
            (void *)&MPEG4Spec,
            sizeof(MPEG4Spec));
  }


  /* Play the clip */
  retVal = QtvPlayer::PlayClip(startPlayPos, -1);
  if (retVal == QtvPlayer::QTV_RETURN_OK)
  {
    return SUCCESS;
  }

  // Failed to play clip, free any output buffers that were allocated
  // we can always free, because pointers will be null or valid
  FreeOutputBuffers(&OutputBuffers);

  return EFAILED;
}


extern "C" int OEMMediaMPEG42PV_RegisterForCallback(void *handle)
{
   QtvPlayer::ReturnT retVal;

   pCallbackData = handle;

   retVal = QtvPlayer::RegisterForCallback(QtvEngineCB, handle);
   if (retVal == QtvPlayer::QTV_RETURN_OK)
   {
      return SUCCESS;
   }
   else
   {
      return EFAILED;
   }
}

#ifdef FEATURE_QTV_PSEUDO_STREAM
#error code not present
#endif /* FEARTURE_QTV_PSEUDO_STREAM */

extern "C" int  OEMMediaMPEG42PV_SetStartPos(uint32 lTimeMS)
{

  /* to set start time, QTV should be either in IDLE or READY state (means clip is not playing) */
  if( (MPEG4EngineState != QtvPlayer::QTV_PLAYER_STATUS_PLAYBACK_READY) &&
      (MPEG4EngineState != QtvPlayer::QTV_PLAYER_STATUS_IDLE) )
  {
    return EBADSTATE;
  }

  if( clipInfoAvail && (lTimeMS >= clipInfo.Duration) )
  {
    return EBADPARM;
  }

  startTime = lTimeMS;
  return SUCCESS;
}

extern "C" int OEMMediaMPEG42PV_Seek(AEEMediaSeek eSeek, int32 TimeOrFrame)
{
  QtvPlayer::PlayerStateRecordT state;
  QtvPlayer::ReturnT retVal;

  retVal = QtvPlayer::GetPlayerState(state);

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

    if (retVal != QtvPlayer::QTV_RETURN_OK)
    {
      return EFAILED;
    }
    return SUCCESS;
  }

  if (retVal != QtvPlayer::QTV_RETURN_OK || clipInfo.RepositioningAllowed == FALSE)
    return EFAILED;

  /* if we are buffering and we have not started playback yet (means initial buffering),
     don't do repositioning */
  if( (MPEG4EngineState==QtvPlayer::QTV_PLAYER_STATUS_BUFFERING) &&
      !state.audioFrames &&
      !state.videoFrames )
    return EFAILED;

  if( seekPending == TRUE )
    return EALREADY;

  if (paused == FALSE)
  {
    startTime = state.playbackMsec;
  }

#ifdef FEATURE_QTV_RANDOM_ACCESS_REPOS
#error code not present
#endif /*FEATURE_QTV_RANDOM_ACCESS_REPOS*/

  if (TimeOrFrame >= 0)
  {
    startTime = MIN(startTime + TimeOrFrame, clipInfo.Duration);
  }
  else
  {
    startTime = MAX(startTime + TimeOrFrame, 0);
  }
  if (MPEG4EngineState != QtvPlayer::QTV_PLAYER_STATUS_PAUSED)
  {
    retVal = QtvPlayer::PlayClip(startTime, -1);
    if (retVal != QtvPlayer::QTV_RETURN_OK)
    {
      return EFAILED;
    }
    seekPending = TRUE;
  }
  else
  {
    repositionInPause = TRUE;
    repositionInPauseUpdateTime = TRUE;
    OEMMediaMPEG4_QueueCallback((void *)pCallbackData,
                              MM_STATUS_SEEK,
                              MM_CMD_PLAY,
                              0,
                              (void *)startTime,
                              sizeof(startTime));
  }
  return SUCCESS;
}
#ifndef FEATURE_QTV_VIDEO_DISABLE
extern "C" int OEMMediaMPEG42PV_GetFrame(void **ppFrame)
{
  QtvPlayer::FrameInfoT  frameInfo;
  QtvPlayer::ReturnT nReturn;
  IDIB  *pDIB;
#ifdef FEATURE_QTV_PP_ON_ARM
  AEEMediaMP4CodecType audiocodectype, videocodectype;
#endif

  if (!ppFrame)
  {
    return EBADPARM;
  }

  *ppFrame = NULL;

  /* Always ask PV for the frame information
  ** You must ALWAYS attempt to get a frame if you've been
  ** told there is a frame to get.  If a frame is not
  ** retrieved from QtvPlayer, it leads to an error state
  ** of lost frame in Mpeg4Player.
  */

  nReturn = QtvPlayer::GetFrameInfo(frameInfo);

  if( nReturn != QtvPlayer::QTV_RETURN_OK ||
      frameInfo.RGBBuffer == NULL )
  {
    return EFAILED;
  }

  /* if we are not playing, don't send any frame.
  ** Be sure to release the buffer back to the player, however
  */
  if( (MPEG4EngineState == QtvPlayer::QTV_PLAYER_STATUS_IDLE) ||
      (MPEG4EngineState == QtvPlayer::QTV_PLAYER_STATUS_PLAYBACK_READY) )
  {
    #ifdef FEATURE_QTV_MDP
    mp4_free_frame_buffer(NULL);
    #endif /* FEATURE_QTV_MDP */

    return EBADSTATE;
  }


#ifdef FEATURE_QTV_PP_ON_ARM
  audiocodectype = OEMMediaMPEG42PV_GetAudioCodecType();
  videocodectype = OEMMediaMPEG42PV_GetVideoCodecType();

/* Color conversion & rotation is done by Qtv IPL if MPEG4-AAC+ DSP image is downloaded
 * i.e. qtv_cfg_enable_aacplus is TRUE
 */
  if ( ( qtv_cfg_enable_aacplus == TRUE ) && ( audiocodectype == MM_MPEG4_AAC_CODEC )
       && ( videocodectype == MM_MPEG4_MPEG4_CODEC || videocodectype == MM_MPEG4_H263_CODEC )
       && ( frameInfo.Height <= MP4_QCIF_HEIGHT ) && ( frameInfo.Width <= MP4_QCIF_WIDTH ) )
  {
    RotateColorConvert( &frameInfo );
  } else
#endif /*FEATURE_QTV_PP_ON_ARM */
  {
#ifdef FEATURE_QTV_IPL_SCALING
    // We want to scale the frame relative to the original frame, not the current one
    if (IPLScalingFactor != MM_MPEG4_NO_SCALING)
      ScaleFrame( &frameInfo );
#endif /* FEATURE_QTV_IPL_SCALING */
  }
  /* Update our local IBitmap to point to the right buffer, size, etc. */
  if( SUCCESS != IBITMAP_QueryInterface(pIBitmap, AEECLSID_DIB, (void**)&pDIB) )
  {
    return EFAILED;
  }
  pDIB->pBmp   = (byte*)frameInfo.RGBBuffer;
  pDIB->nPitch = frameInfo.Width * 2;  /* Fix hard-coded bytes/pixel */
  pDIB->cx     = frameInfo.Width;
  pDIB->cy     = frameInfo.Height;

  IDIB_Release(pDIB);

  /* Add to the reference count for the bitmap since the app will free it */
  IBITMAP_AddRef(pIBitmap);
  *ppFrame = pIBitmap;

  return SUCCESS;
}
#endif /* FEATURE_QTV_VIDEO_DISABLE */

#ifdef FEATURE_QTV_STREAMING
#error code not present
#endif /* FEATURE_QTV_STREAMING */

extern "C" int OEMMediaMPEG42PV_Stop(void)
{
  QtvPlayer::ReturnT retVal;
  dword totalSleepDuration = 0;

  // sleep for 100ms to prevent known problem caused by too fast start/stop (S-Magic)
  OEMOS_Sleep(100);

  retVal = QtvPlayer::Stop();

  if ( retVal == QtvPlayer::QTV_RETURN_OK )
  {
    int dog_autokick;

    dog_autokick = rex_autodog_enable(rex_self()->dog_report_val);

    /* Wait for the QTV engine to go to IDLE state before returning.
     * But limit this wait to no more than PLAYER_TRANSITION_TO_IDLE_TIMEOUT
     */

    while( MPEG4EngineState != QtvPlayer::QTV_PLAYER_STATUS_IDLE &&
           ( totalSleepDuration < PLAYER_TRANSITION_TO_IDLE_TIMEOUT ) )
    {
      OEMOS_Sleep(PLAYER_STATE_POLL_INTERVAL);
      totalSleepDuration += PLAYER_STATE_POLL_INTERVAL;
    }

    if (!dog_autokick)
    {
      rex_autodog_disable();
    }
  }

  if (MPEG4EngineState == QtvPlayer::QTV_PLAYER_STATUS_IDLE)
  {
    MSG_MED( "Engine reported IDLE in %d ms ",
              totalSleepDuration,
              0,
              0);

    // Free any output buffers that were allocated to play clip
    FreeOutputBuffers(&OutputBuffers);
  }
  else
  {
    MSG_ERROR( "Engine did not report IDLE, current state = %d ",
               MPEG4EngineState,
               0,
               0);
  }

  if (retVal != QtvPlayer::QTV_RETURN_OK)
  {
    MSG_ERROR("OEMMediaMPEG42PV_Stop failed", 0, 0, 0);
    return EFAILED;
  }

  return SUCCESS;
}

extern "C" int OEMMediaMPEG42PV_Terminate(void)
{
  QtvPlayer::ReturnT retVal;

  /* At this time, a timer for updating the progress bar could be running
   * with NULL as user supplied data. The semantics of CancelTimer are such
   * that function and the user supplied data both need to be passed for a
   * timer to be cancelled. Hence, make sure to cancel timer for each user
   * supplied data if SetTimer is called with user data other than NULL
   */
#ifdef FEATURE_QTV_IPL_SCALING
  //Reset the scaling factor
  IPLScalingFactor = MM_MPEG4_NO_SCALING;
#endif /* FEATURE_QTV_IPL_SCALING */

#ifdef FEATURE_QTV_PP_ON_ARM
  /* Initialized to no rotation */
  IPLRotationFactor = MM_MPEG4_NO_ROTATION;
#endif /* FEATURE_QTV_PP_ON_ARM */

  ISHELL_CancelTimer(pIShell, UpdateProgressBar, NULL);

  clipInfoAvail = FALSE;

  MSG_MED("Calling QtvPlayer::Terminate", 0, 0, 0);
  retVal = QtvPlayer::Terminate();
  MSG_MED("QtvPlayer::Terminate() returned %d", retVal, 0, 0);

  /* Free the display buffers */
  FreeOutputBuffers(&OutputBuffers);

  /* Release the shell interface */
  ISHELL_Release(pIShell);
  IBITMAP_Release(pIBitmap);

  pIShell   = NULL;
  pIBitmap  = NULL;

  videoURN[0] = '\0';
  audioURN[0] = '\0';

  pVideoBuffer = NULL;
  pAudioBuffer = NULL;
  VideoBufSize = 0;
  AudioBufSize = 0;

  if (retVal == QtvPlayer::QTV_RETURN_OK)
  {
    return SUCCESS;
  }
  else
  {
    return EFAILED;
  }
}

extern "C" void SetVolumeCB(const void *client_data, snd_status_type status)
{
  int nStatus;

  if(client_data != NULL)
    return;

  if(status == SND_SUCCESS)
  {
    nStatus = MM_STATUS_DONE;
  }
  else
  {
    nStatus = MM_STATUS_ABORT;
  }

  OEMMediaMPEG4_QueueCallback((void *)pCallbackData,
                             nStatus,
                             MM_CMD_SETMEDIAPARM,
                             MM_PARM_VOLUME,
                             NULL,
                             0);
}

extern "C" void GetVolumeCB(const void *client_data, snd_status_type status, uint16 volume)
{
  int nStatus;

  if(client_data != NULL)
    return;

  if(status == SND_SUCCESS)
  {
    nStatus = MM_STATUS_DONE;
  }
  else
  {
    nStatus = MM_STATUS_ABORT;
  }

  OEMMediaMPEG4_QueueCallback((void *)pCallbackData,
                             nStatus,
                             MM_CMD_GETMEDIAPARM,
                             MM_PARM_VOLUME,
                             (uint16*)(volume*MM_MPEG4_VOLUME_STEP),
                             sizeof(uint16));
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

#ifdef FEATURE_QTV_STREAMING
#error code not present
#endif /* FEATURE_QTV_STREAMING */

        default:

            StreamType = MM_MEDIA_NON_STREAMING;
            break;
    }

    return StreamType;
}

AEEMediaMP4CodecType OEMMediaMPEG42PV_GetAudioCodecType(void)
{
  QtvPlayer::CodecType    PVCodecType;
  AEEMediaMP4CodecType        AudioCodecType;

  PVCodecType = QtvPlayer::GetAudioCodecType();

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

#ifdef FEATURE_MP4_MP3
    case QtvPlayer::QTV_MP3_CODEC:
      AudioCodecType = MM_MPEG4_MP3_CODEC;
      break;
#endif /* FEATURE_MP4_MP3 */

    default:
      AudioCodecType = MM_MPEG4_UNKNOWN_CODEC;
      break;
  }

  return AudioCodecType;
}

#ifndef FEATURE_QTV_VIDEO_DISABLE
#ifdef FEATURE_QTV_PP_ON_ARM
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
AEEMediaMP4CodecType OEMMediaMPEG42PV_GetVideoCodecType(void)
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

    default:
      VideoCodecType = MM_MPEG4_UNKNOWN_CODEC;
      break;
  }

  return VideoCodecType;
}
#endif
#endif /* FEATURE_QTV_VIDEO_DISABLE */
extern "C" int OEMMediaMPEG42PV_SetVolume(int32 volume)
{
  snd_method_type snd_method;
  QtvPlayer::CodecType codecType;

  if( !clipInfoAvail )
  {
    generateVolumeCB = SET_VOLUME_CB;
    savedVolume = volume;
    return MM_PENDING;
  }

  codecType = QtvPlayer::GetAudioCodecType();
  switch(codecType)
  {
    case QtvPlayer::QTV_AAC_CODEC:
#ifdef FEATURE_MP4_MP3
    case QtvPlayer::QTV_MP3_CODEC:
#endif /* FEATURE_MP4_MP3 */
#ifdef FEATURE_AUDIO_FORMAT
      snd_method = SND_METHOD_MIDI;
#else
      snd_method = SND_METHOD_MAX;
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
      snd_method = SND_METHOD_VOICE;
      break;

    case QtvPlayer::QTV_UNKNOWN_CODEC :
    default:
      return EFAILED;
  }

  snd_set_volume(SND_DEVICE_CURRENT, snd_method, (uint8)volume/MM_MPEG4_VOLUME_STEP, &SetVolumeCB, NULL);

  return MM_PENDING;
}

extern "C" int OEMMediaMPEG42PV_GetVolume(void)
{
  snd_method_type snd_method;
  QtvPlayer::CodecType codecType;

  if( !clipInfoAvail )
  {
    generateVolumeCB = GET_VOLUME_CB;
    return MM_PENDING;
  }

  codecType = QtvPlayer::GetAudioCodecType();
  switch(codecType)
  {
    case QtvPlayer::QTV_AAC_CODEC:
#ifdef FEATURE_MP4_MP3
    case QtvPlayer::QTV_MP3_CODEC:
#endif /* FEATURE_MP4_MP3 */
#ifdef FEATURE_AUDIO_FORMAT
      snd_method = SND_METHOD_MIDI;
#else
      snd_method = SND_METHOD_MAX;
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
      snd_method = SND_METHOD_VOICE;
      break;

    case QtvPlayer::QTV_UNKNOWN_CODEC :
    default:
      return EFAILED;
  }

  snd_get_volume(SND_DEVICE_CURRENT, snd_method, &GetVolumeCB, NULL);

  return MM_PENDING;
}


#define COPY_STD_SPEC(from, to) \
  UTF8TOWSTR( (uint8*)pClipInfo->ExtInfo.StdInfo.##from, sizeof(pClipInfo->ExtInfo.StdInfo.##from), \
              MPEG4Spec->ExtendedInfo.StdSpec.##to, sizeof(MPEG4Spec->ExtendedInfo.StdSpec.##to) )

#ifdef FEATURE_MP4_KDDI_META_DATA
#define COPY_KDDI_SPEC(from,to) \
  UTF8TOWSTR( (uint8*)pClipInfo->ExtInfo.KDDIInfo.##from, sizeof(pClipInfo->ExtInfo.KDDIInfo.##from), \
              MPEG4Spec->ExtendedInfo.KDDISpec.##to, sizeof(MPEG4Spec->ExtendedInfo.KDDISpec.##to) )

#define ASSIGN_KDDI_SPEC(from,to) \
  MPEG4Spec->ExtendedInfo.KDDISpec.##to = pClipInfo->ExtInfo.KDDIInfo.##from

#endif /* FEATURE_MP4_KDDI_META_DATA */

static void CopyMPEG4SpecFromClipInfo(AEEMediaMPEG4Spec * MPEG4Spec, QtvPlayer::ClipInfoT *pClipInfo)
{
  MEMSET(MPEG4Spec, 0, sizeof(MPEG4Spec));
  MPEG4Spec->streamtype                 = OEMMediaMPEG42PV_ConvertStreamType(
                                                                 pClipInfo->streamtype);
  MPEG4Spec->dwDuration                 = pClipInfo->Duration;
  MPEG4Spec->dwSize                     = pClipInfo->Size;
  MPEG4Spec->dwHeight                   = pClipInfo->Height;
  MPEG4Spec->dwWidth                    = pClipInfo->Width;
  MPEG4Spec->mpeg4Type                  = OEMMediaMPEG42PV_ConvertMediaTypePVToAEE(pClipInfo->MediaType);
  MPEG4Spec->audioType                  = OEMMediaMPEG42PV_GetAudioCodecType();
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
  }
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
}

extern "C" int OEMMediaMPEG42PV_GetMediaSpec(AEEMediaMPEG4Spec **pSpec, int32 *pSize)
{
  int nRet = SUCCESS;

  if( clipInfoAvail )
  {
    *pSpec = &MPEG4Spec;
    *pSize = sizeof(MPEG4Spec);
  }
  else
  { /* Media spec is not available right now, so send it back via a callback */
    *pSpec = NULL;
    *pSize = 0;
    nRet = MM_PENDING;
    generateMediaSpecCB = TRUE;
  }

  return nRet;
}

#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
extern "C" int OEMMediaMPEG42PV_RotateVideo(AEEMediaMPEG4RotationType RotationType)
{
  QtvPlayer::RotationType QtvRotationFactor;

#ifdef FEATURE_QTV_PP_ON_ARM  
  AEEMediaMP4CodecType audiocodectype, videocodectype;
  
  audiocodectype = OEMMediaMPEG42PV_GetAudioCodecType();
  videocodectype = OEMMediaMPEG42PV_GetVideoCodecType();

  /* If MPEG4-AAC+ DSP image is downloaded, use Qtv IPL for rotation & color conversion on ARM */
  if ((qtv_cfg_enable_aacplus == TRUE) && (audiocodectype == MM_MPEG4_AAC_CODEC) &&
      (videocodectype == MM_MPEG4_MPEG4_CODEC || videocodectype == MM_MPEG4_H263_CODEC)
      && (clipInfo.Height <= MP4_QCIF_HEIGHT) && (clipInfo.Width <= MP4_QCIF_WIDTH))
  {
    IPLRotationFactor = RotationType;

    return SUCCESS;
  }
  else /* Use DSP for rotation and color conversion */
#endif /* FEATURE_QTV_PP_ON_ARM */
  {
    if(bFrameTransformPending)
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
      default:
        QtvRotationFactor = QtvPlayer::ROTATE_NO;
        break;
    }

    if(QtvPlayer::RotateVideo(QtvRotationFactor) == QtvPlayer::QTV_RETURN_OK)
    {
      bFrameTransformPending = TRUE;
      return SUCCESS;
    }
  }
  return EFAILED;
}

extern "C" int OEMMediaMPEG42PV_ScaleVideo(AEEMediaMPEG4ScalingType ScaleFactor)
{
#ifdef FEATURE_QTV_IPL_SCALING /* IPL Scaling using CPU */
  unsigned int Width = MP4_MAX_DECODE_WIDTH;
  unsigned int Height = MP4_MAX_DECODE_HEIGHT;
  QtvPlayer::FrameInfoT  frameInfo;
  QtvPlayer::ReturnT nReturn;
  bool frameInfoAvail=true;

  /* free any previous buffer */
  if(pIPLScalingBuf)
  {
    OEM_Free(pIPLScalingBuf);
    pIPLScalingBuf = NULL;
  }
  /* Ask PV for the frame information
   * We need this to properly size the scaling buffer
   * since clipInfo size is not necessarily the size the decoder can use
   */
  nReturn = QtvPlayer::GetFrameInfo(frameInfo);

  if( nReturn != QtvPlayer::QTV_RETURN_OK || frameInfo.RGBBuffer == NULL )
  {
    frameInfoAvail=false;
  }


  if(clipInfoAvail && (clipInfo.MediaType == QtvPlayer::MEDIA_AUDIO_ONLY))
  {
    /* no scaling needed for audio only clips */
    return EFAILED;
  }

  if( frameInfoAvail && (frameInfo.Width > 0) && (frameInfo.Height > 0))
  {
    Width = frameInfo.Width;
    Height = frameInfo.Height;
  }

  switch(ScaleFactor)
  {
    case MM_MPEG4_2P5X_ZOOM:
      pIPLScalingBuf = (unsigned char *)OEM_Malloc( (int) (2.5 * 2.5 * Height * Width * MP4_RGB_BITS_PER_PIXEL / 8) );
      break;

    case MM_MPEG4_2X_ZOOM:
      pIPLScalingBuf = (unsigned char *)OEM_Malloc( (int) (2 * 2 * Height * Width * MP4_RGB_BITS_PER_PIXEL / 8) );
      break;

    case MM_MPEG4_1P25X_ZOOM:
      pIPLScalingBuf = (unsigned char *)OEM_Malloc( (int) (1.25 * 1.25 * Height * Width * MP4_RGB_BITS_PER_PIXEL / 8) );
      break;

    case MM_MPEG4_0P25X_SHRINK:
      pIPLScalingBuf = (unsigned char *)OEM_Malloc( (int) (0.25 * 0.25 * Height * Width * MP4_RGB_BITS_PER_PIXEL / 8) );
      break;

    case MM_MPEG4_0P75X_SHRINK:
      pIPLScalingBuf = (unsigned char *)OEM_Malloc( (int) (0.75 * 0.75 * Height * Width * MP4_RGB_BITS_PER_PIXEL / 8) );
      break;

    case MM_MPEG4_2X_SHRINK:
      pIPLScalingBuf = (unsigned char *)OEM_Malloc( (int) (0.5 * 0.5 * Height * Width * MP4_RGB_BITS_PER_PIXEL / 8) );
      break;

    case MM_MPEG4_NO_SCALING:
    default:
      break;
  }
  if( (pIPLScalingBuf==NULL) && (ScaleFactor!=MM_MPEG4_NO_SCALING) )
  {
    MSG_ERROR("Scaling buffer allocation failed.",0,0,0);
    IPLScalingFactor = MM_MPEG4_NO_SCALING;
    return EFAILED;
  }
  IPLScalingFactor = ScaleFactor;
  return SUCCESS;
#else /* !FEATURE_QTV_IPL_SCALING (Scaling using DSP ) */
  QtvPlayer::ScalingType type = QtvPlayer::SCALE_NONE;

  if(bFrameTransformPending)
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
      type = QtvPlayer::SCALE_NONE;
      break;
  }
  if(QtvPlayer::ScaleVideo(type) == QtvPlayer::QTV_RETURN_OK)
  {
    bFrameTransformPending = TRUE;
    return SUCCESS;
  }
  return EFAILED;
#endif /* FEATURE_QTV_IPL_SCALING */
}
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */

#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
extern "C" int OEMMediaMPEG42PV_GetTelopSubString(AEETelopTextSubString **ppTextSubStr, int32 nIndex)
{
  QtvPlayer::TelopSubStringT PVTelopSubStr;
  if(QtvPlayer::GetTelopSubString(&PVTelopSubStr, nIndex))
  {
    TelopTextSubString.bLineFeed          = PVTelopSubStr.LineFeed;
    TelopTextSubString.bLinking           = PVTelopSubStr.Linking;
    TelopTextSubString.bReversal          = PVTelopSubStr.Reversal;
    TelopTextSubString.bUnderline         = PVTelopSubStr.Underline;
    TelopTextSubString.dwFontColor        = PVTelopSubStr.FontColor;
    TelopTextSubString.dwLinkSize         = PVTelopSubStr.LinkSize;
    TelopTextSubString.dwTextSampleSize   = PVTelopSubStr.TextSampleSize;
    TelopTextSubString.pLinkValue         = PVTelopSubStr.LinkValue;
    TelopTextSubString.pTextSubString     = PVTelopSubStr.TextSubString;
    *ppTextSubStr = &TelopTextSubString;
  }
  else
  {
    MEMSET(&TelopTextSubString, 0, sizeof(AEETelopTextSubString));
    *ppTextSubStr = NULL;
  }
  return SUCCESS;
}
#endif /* FEATURE_MP4_KDDI_TELOP_TEXT */

#ifdef FEATURE_VIDEO_AUDIO_DUBBING
#error code not present
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
Function: AllocateOutputBuffers

Description: Called to allocate the necessary RGB and YUV buffers needed to
             decode the clip.

Return Value:
  TRUE  - if buffers were allocated successfully
  FALSE - if an error occurred

Comments:

Side Effects:

============================================================================*/
boolean AllocateOutputBuffers(MP4BufType *pBuf, uint16 Height, uint16 Width)
{
  boolean bNoError = TRUE;
  int nBytes;
  int i;

  pBuf->Height     = Height;
  pBuf->Width      = Width;
  pBuf->RedBpp     = MP4_RED_BPP;
  pBuf->GreenBpp   = MP4_GREEN_BPP;
  pBuf->BlueBpp    = MP4_BLUE_BPP;
  pBuf->ColorOrder = RGB_LENDIAN_ORDER;

  /* Free any old memory allocated for clip */
  FreeOutputBuffers(pBuf);

  /* for IPL based scaling DSP does not need bigger buffer */
#if defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) && !defined(FEATURE_QTV_IPL_SCALING)
  /* allocate 4 times more memory to support 2X Up Scaling */
  nBytes = 4 * Height * Width * MP4_RGB_BITS_PER_PIXEL / 8;
#else
  nBytes = Height * Width * MP4_RGB_BITS_PER_PIXEL / 8;
#endif

  for( i = 0; i < MP4_NUM_RGB_BUFFERS; i++ )
  {
    /* Attempt to allocate the RGB buffer */
    pBuf->pRGBBuf[i] = OEM_Malloc(nBytes);
    if( pBuf->pRGBBuf[i] == NULL ) {
      bNoError = FALSE;
      break;
    }
    /* See if the buffer allocated crosses the 24-bit boundary */
    if( CROSSES_24BIT_BOUNDARY( pBuf->pRGBBuf[i], nBytes ) ) {
      /* The buffer crosses a 24-bit boundary, which is not supported by the QDSP,
       * so temporarily hold onto the orginal buffer while attempting to allocate
       * a new one. */
      void *pTemp = pBuf->pRGBBuf[i];
      pBuf->pRGBBuf[i] = OEM_Malloc(nBytes);
      OEM_Free(pTemp); /* Free the old buffer */
      if( pBuf->pRGBBuf[i] == NULL ) {
        bNoError = FALSE;
        break;
      }
      /* See if the buffer allocated crosses the 24-bit boundary */
      if( CROSSES_24BIT_BOUNDARY( pBuf->pRGBBuf[i], nBytes ) ) {
        /* The buffer crosses a 24-bit boundary, so give up */
        bNoError = FALSE;
        break;
      }
    }
  }

  /* If an error was encountered, free all buffers and set the appropriate state
   * variables
   */
  if( bNoError == FALSE ) {
    for( i = 0; i < MP4_NUM_RGB_BUFFERS; i++ ) {
      if( pBuf->pRGBBuf[i] != NULL ) {
        OEM_Free(pBuf->pRGBBuf[i]);
        pBuf->pRGBBuf[i] = NULL;
      }
    }

    MSG_ERROR("Unable to allocate RGB Buffers",0,0,0);
    return FALSE;
  }

  nBytes = Height * Width * MP4_YUV_BITS_PER_PIXEL / 8;

  /* Allocate the 12bpp YUV buffers for the QDSP */
  for( i = 0; i < MP4_NUM_YUV_BUFFERS; i++ ) {
    pBuf->pYUVBuf[i] = NULL;
  }
#ifdef FEATURE_VIDEO_PLAYER_INTERFACE_REV_2A
#error code not present
#endif /* FEATURE_VIDEO_PLAYER_INTERFACE_REV_2A */

  for( i = 0; i < MP4_NUM_YUV_BUFFERS; i++ )
  {
    /* Attempt to allocate the YUV buffer */
    pBuf->pYUVBuf[i] = OEM_Malloc(nBytes);
    if( pBuf->pYUVBuf[i] == NULL ) {
      bNoError = FALSE;
      break;
    }
    /* See if the buffer allocated crosses the 24-bit boundary */
    if( CROSSES_24BIT_BOUNDARY( pBuf->pYUVBuf[i], nBytes ) ) {
      /* The buffer crosses a 24-bit boundary, which is not supported by the QDSP,
       * so temporarily hold onto the orginal buffer while attempting to allocate
       * a new one. */
      void *pTemp = pBuf->pYUVBuf[i];
      pBuf->pYUVBuf[i] = OEM_Malloc(nBytes);
      OEM_Free(pTemp);
      if( pBuf->pYUVBuf[i] == NULL ) {
        bNoError = FALSE;
        break;
      }
      /* See if the buffer allocated crosses the 24-bit boundary */
      if( CROSSES_24BIT_BOUNDARY( pBuf->pYUVBuf[i], nBytes ) ) {
        /* The buffer crosses a 24-bit boundary, so give up */
        bNoError = FALSE;
        break;
      }
    }
  }

#ifdef FEATURE_VIDEO_PLAYER_INTERFACE_REV_C
  if(bNoError == TRUE)
  {
    /* Attempt to allocate the YUV buffer for the deblock filter */
    pBuf->pFilteredYUVBuf = OEM_Malloc(nBytes);
    if(pBuf->pFilteredYUVBuf == NULL)
      bNoError = FALSE;
  }
#endif /* FEATURE_VIDEO_PLAYER_INTERFACE_REV_C */

  /* If an error was encountered, free all buffers and set the appropriate state
   * variables
   */
  if(bNoError == FALSE) {
    for( i = 0; i < MP4_NUM_YUV_BUFFERS; i++ ) {
      if( pBuf->pYUVBuf[i] != NULL ) {
        OEM_Free( pBuf->pYUVBuf[i] );
        pBuf->pYUVBuf[i] = NULL;
      }
    }

    for( i = 0; i < MP4_NUM_RGB_BUFFERS; i++ ) {
      if( pBuf->pRGBBuf[i] != NULL ) {
        OEM_Free(pBuf->pRGBBuf[i]);
        pBuf->pRGBBuf[i] = NULL;
      }
    }

#ifdef FEATURE_VIDEO_PLAYER_INTERFACE_REV_C
    if (pBuf->pFilteredYUVBuf != NULL )
    {
      OEM_Free(pBuf->pFilteredYUVBuf);
      pBuf->pFilteredYUVBuf = NULL;
    }
#endif /* FEATURE_VIDEO_PLAYER_INTERFACE_REV_C */

#ifdef FEATURE_VIDEO_PLAYER_INTERFACE_REV_2A
#error code not present
#endif /* FEATURE_VIDEO_PLAYER_INTERFACE_REV_2A */

    MSG_ERROR("Unable to allocate YUV Buffers", 0, 0 ,0);
    return FALSE;
  }

  /* Everything succeeded! */
  return TRUE;
}

/* <EJECT> */
/*===========================================================================
Function: FreeOutputBuffers

Description:  Called to frre the allocated RGB and YUV buffers by
              AllocateOutputBuffers function.
Return Value:

Comments:

Side Effects:

============================================================================*/
extern void FreeOutputBuffers(MP4BufType *pBuf)
{
  int i;
  /* Free the display buffers */

  for( i = 0; i < MP4_NUM_RGB_BUFFERS; i++ )
  {
    if( pBuf->pRGBBuf[i] != NULL )
    {
      OEM_Free(pBuf->pRGBBuf[i]);
      pBuf->pRGBBuf[i] = NULL;
    }
  }

  for( i = 0; i < MP4_NUM_YUV_BUFFERS; i++ )
  {
    if( pBuf->pYUVBuf[i] != NULL )
    {
      OEM_Free(pBuf->pYUVBuf[i]);
      pBuf->pYUVBuf[i] = NULL;
    }
  }

#ifdef FEATURE_QTV_IPL_SCALING
  if(pIPLScalingBuf)
  {
    OEM_Free(pIPLScalingBuf);
    pIPLScalingBuf = NULL;
  }
#endif

#ifdef FEATURE_VIDEO_PLAYER_INTERFACE_REV_C
  if (pBuf->pFilteredYUVBuf != NULL )
  {
    OEM_Free(pBuf->pFilteredYUVBuf);
    pBuf->pFilteredYUVBuf = NULL;
  }
#endif /* FEATURE_VIDEO_PLAYER_INTERFACE_REV_C */

#ifdef FEATURE_VIDEO_PLAYER_INTERFACE_REV_2A
#error code not present
#endif /* FEATURE_VIDEO_PLAYER_INTERFACE_REV_2A */
}

/* gets the time scale of the given track */
/* returns FALSE, if track is not supported or error */
/* time scale may be zero, if any supported track is not present in the media */
extern "C" int OEMMediaMPEG42PV_GetTrackTimeScale(AEEMediaMPEG4TrackType trackType, uint32 *pTimeScale)
{
  QtvPlayer::ReturnT retVal;
  switch(trackType)
  {
#ifndef FEATURE_QTV_VIDEO_DISABLE
    case VIDEO_TRACK:
       retVal = QtvPlayer::GetTrackTimeScale(QtvPlayer::VideoTrack, pTimeScale);
      break;
#endif
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

  return (retVal==QtvPlayer::QTV_RETURN_OK) ? AEE_SUCCESS:EFAILED;
}

extern "C" int
OEMMediaMPEG42PV_SetTickUpdateInterval
(
  uint32 updateInterval
)
{
  if(updateInterval == 0)
  {
    return EFAILED;
  }

  dwTickInterval = updateInterval;

  return SUCCESS;
}


extern "C" int OEMMediaMPEG42PV_SetLoopTrack(boolean bLoop)
{
  QtvPlayer::ReturnT retVal;
  retVal = QtvPlayer::SetLoopTrack(bLoop?true:false);
  if(retVal == QtvPlayer::QTV_RETURN_OK)
    return SUCCESS;
  else
    return EFAILED;
}

extern "C" int OEMMediaMPEG42PV_GetPlaybackPos(AEEMediaPlaybackPos * pPlayPos)
{
  QtvPlayer::PlayerStateRecordT state = {0};
  QtvPlayer::ReturnT retVal = QtvPlayer::QTV_RETURN_ERROR;

  retVal = QtvPlayer::GetPlayerState(state);

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
extern "C" int OEMMediaMPEG42PV_SetDRMKey(void *pDrmKey, int32 sizeOfDrmKey)
{
  QtvPlayer::ReturnT retVal = QtvPlayer::QTV_RETURN_ERROR;

  if(MPEG4EngineState != QtvPlayer::QTV_PLAYER_STATUS_IDLE)
    return EBADSTATE;

  if( !pDrmKey || !sizeOfDrmKey )
  {
    retVal = QtvPlayer::SetDRMKey(NULL, 0);
  }
  else
  {
    retVal = QtvPlayer::SetDRMKey(pDrmKey, (uint32)sizeOfDrmKey);
  }

  if(retVal == QtvPlayer::QTV_RETURN_OK)
    return SUCCESS;

  return EFAILED;
}
#endif /* FEATURE_QTV_MFDRM */

#ifdef FEATURE_QTV_STREAM_RECORD
#error code not present
#endif /* FEATURE_QTV_STREAM_RECORD */

#ifdef FEATURE_QTV_VIDEO_EDITING
#ifndef FEATURE_QTV_VIDEO_DISABLE

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
#endif /* FEATURE_QTV_VIDEO_DISABLE */
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

#ifndef FEATURE_QTV_VIDEO_DISABLE
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
#endif /* FEATURE_QTV_VIDEO_DISABLE */
#endif /* FEATURE_QTV_VIDEO_EDITING */


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
    if (pos && strcmp(pos, DEBUG_URL_FILE_EXTENSION) == 0)
    {
      char str[DEBUG_MAX_STRING_LENGTH];
      FILE *fp = fopen(fileName, "r");
      if (fp)
      {
        char *variable;
        char *value;
        int len = fread(str, 1, DEBUG_MAX_STRING_LENGTH, fp);
        fclose(fp);
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
                strncpy((char *)dv->value, value, dv->length);
                ((char *)(dv->value))[dv->length - 1] = '\0';
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
/* applys IPL software based scaling to the given frame */
static void ScaleFrame( QtvPlayer::FrameInfoT*  pFrameInfo )
{
  ipl_image_type ipl_image, out_ipl_image;

  //Width*Height*(RedBpp+GreenBpp+BlueBpp)/8 (bytes)
  ipl_image.dx = pFrameInfo->Width;
  ipl_image.dy = pFrameInfo->Height;
  ipl_image.cFormat = IPL_RGB565;
  ipl_image.imgPtr = (unsigned char*) pFrameInfo->RGBBuffer;


  switch( IPLScalingFactor )
  {
    case MM_MPEG4_2P5X_ZOOM:
      out_ipl_image.dx = (uint32)(pFrameInfo->Width * 25)/10;
      out_ipl_image.dy = (uint32)(pFrameInfo->Height * 25)/10;
      out_ipl_image.imgPtr = pIPLScalingBuf;
      out_ipl_image.cFormat = IPL_RGB565;
      ipl_upsize( &ipl_image, &out_ipl_image, NULL, IPL_QUALITY_LOW );
      break;

    case MM_MPEG4_2X_ZOOM:
      out_ipl_image.dx = (uint32)pFrameInfo->Width * 2;
      out_ipl_image.dy = (uint32)pFrameInfo->Height * 2;
      out_ipl_image.imgPtr = pIPLScalingBuf;
      out_ipl_image.cFormat = IPL_RGB565;
      ipl_upsize( &ipl_image, &out_ipl_image, NULL, IPL_QUALITY_LOW );
      break;

    case MM_MPEG4_1P25X_ZOOM:
      out_ipl_image.dx = (uint32)(pFrameInfo->Width * 125)/100;
      out_ipl_image.dy = (uint32)(pFrameInfo->Height * 125)/100;
      out_ipl_image.imgPtr = pIPLScalingBuf;
      out_ipl_image.cFormat = IPL_RGB565;
      ipl_upsize( &ipl_image, &out_ipl_image, NULL, IPL_QUALITY_LOW );
      break;

    case MM_MPEG4_0P75X_SHRINK:
      out_ipl_image.dx = (uint32)(pFrameInfo->Width * 75)/100;
      out_ipl_image.dy = (uint32)(pFrameInfo->Height * 75)/100;
      out_ipl_image.imgPtr = pIPLScalingBuf;
      out_ipl_image.cFormat = IPL_RGB565;
      ipl_downsize_fast( &ipl_image, &out_ipl_image, NULL );
      break;

    case MM_MPEG4_2X_SHRINK:
      out_ipl_image.dx = (uint32)(pFrameInfo->Width * 5)/10;
      out_ipl_image.dy = (uint32)(pFrameInfo->Height * 5)/10;
      out_ipl_image.imgPtr = pIPLScalingBuf;
      out_ipl_image.cFormat = IPL_RGB565;
      ipl_downsize_fast( &ipl_image, &out_ipl_image, NULL );
      break;

    case MM_MPEG4_0P25X_SHRINK:
      out_ipl_image.dx = (uint32)(pFrameInfo->Width * 25)/100;
      out_ipl_image.dy = (uint32)(pFrameInfo->Height * 25)/100;
      out_ipl_image.imgPtr = pIPLScalingBuf;
      out_ipl_image.cFormat = IPL_RGB565;
      ipl_downsize_fast( &ipl_image, &out_ipl_image, NULL );
      break;

    default:
      out_ipl_image.dx = pFrameInfo->Width;
      out_ipl_image.dy = pFrameInfo->Height;
      out_ipl_image.imgPtr = (unsigned char*) pFrameInfo->RGBBuffer;
      break;
  }

  //Now copy it back to rectImage
  pFrameInfo->Height = out_ipl_image.dy;
  pFrameInfo->Width = out_ipl_image.dx;
  pFrameInfo->RGBBuffer = out_ipl_image.imgPtr;
}
#endif /* FEATURE_QTV_IPL_SCALING */

#ifdef FEATURE_QTV_PP_ON_ARM
/* ======================================================================
FUNCTION
  RotateColorConvert

DESCRIPTION
  Sets up the input image, output image & the crop info for the QTV IPL
  functions to perform rotation/color conversion

DEPENDENCIES
  FEATURE_QTV_PP_ON_ARM

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================== */
static void RotateColorConvert( QtvPlayer::FrameInfoT*  pFrameInfo )
{
  ipl_image_type in_ipl_image, out_ipl_image;
  ipl_status_type statusCode = IPL_FAILURE;
  ipl_rect_type crop_dim;

  /* Initialize the input image dimensions, color format, YUV buffer ptr */
  in_ipl_image.dx = pFrameInfo->Width;
  in_ipl_image.dy = pFrameInfo->Height;
  in_ipl_image.cFormat = IPL_YCbCr420_FRAME_PK;
  in_ipl_image.imgPtr = (unsigned char*) pFrameInfo->YUVBuffer;

  /* Initialize the cropping params */
  crop_dim.x = 0;
  crop_dim.y = 0;

  switch( IPLRotationFactor )
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
      MSG_ERROR( "Unknown frame rotation %d", IPLRotationFactor, 0, 0);
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
  if (IPLScalingFactor != MM_MPEG4_NO_SCALING)
    ScaleFrame( pFrameInfo );

}
#endif /* FEATURE_QTV_PP_ON_ARM */

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
bool OEMMediaMPEG42PV_PrepareTimedText(void)
{
  QtvPlayer::ThreeGPPTimedTextInfo TextInfo;
  bool bRet = false;
  if(QtvPlayer::Get3GPPTimedTextInfo(&TextInfo))
  {
    /* free if there is any buffer */
    if(TimedText3GPP.pszText)
    {
      FREE(TimedText3GPP.pszText);
      TimedText3GPP.pszText = NULL;
    }
    if(TimedText3GPP.pFontList)
    {
      FREE(TimedText3GPP.pFontList);
      TimedText3GPP.pFontList = NULL;
    }
    if(TimedText3GPP.pSampleModifiersBuffer)
    {
      FREE(TimedText3GPP.pSampleModifiersBuffer);
      TimedText3GPP.pSampleModifiersBuffer = NULL;
    }

    if(TextInfo.dwTextDataSize)
    {
      uint8 *pTextData = (uint8*)MALLOC(TextInfo.dwTextDataSize);
      if(QtvPlayer::Get3GPPTimedTextData(pTextData, TextInfo.dwTextDataSize))
      {
        /* first two bytes are text length */
        uint16 nTextLen = pTextData[0];
        nTextLen = (nTextLen<<8) | pTextData[1];
        TimedText3GPP.pszText = (AECHAR*)MALLOC((nTextLen+1)*sizeof(AECHAR));
        MEMSET(TimedText3GPP.pszText, 0, nTextLen+1);
        UTF8TOWSTR(pTextData+2, nTextLen, TimedText3GPP.pszText, nTextLen*sizeof(AECHAR));

        TimedText3GPP.dwSampleModifiersSize   = TextInfo.dwSampleModifiersSize;
        if ( TextInfo.dwSampleModifiersSize )
        {
          TimedText3GPP.pSampleModifiersBuffer = (uint8*)MALLOC(TimedText3GPP.dwSampleModifiersSize);
          if ( TimedText3GPP.pSampleModifiersBuffer )
          {
            /* sample modifiers are after text in the data buffer */
            MEMCPY(TimedText3GPP.pSampleModifiersBuffer, pTextData+2+nTextLen, TimedText3GPP.dwSampleModifiersSize);
          }
        }

        TimedText3GPP.dwDisplayFlags          = TextInfo.dwDisplayFlags;
        TimedText3GPP.dwHorzJustification     = TextInfo.dwHorzJustification;
        TimedText3GPP.dwVertJustification     = TextInfo.dwVertJustification;
        TimedText3GPP.dwBoxTop                = TextInfo.dwBoxTop;
        TimedText3GPP.dwBoxLeft               = TextInfo.dwBoxLeft;
        TimedText3GPP.dwBoxBottom             = TextInfo.dwBoxBottom;
        TimedText3GPP.dwBoxRight              = TextInfo.dwBoxRight;
        TimedText3GPP.dwStartChar             = TextInfo.dwStartChar;
        TimedText3GPP.dwEndChar               = TextInfo.dwEndChar;
        TimedText3GPP.dwFontID                = TextInfo.dwFontID;
        TimedText3GPP.dwFontStyleFlags        = TextInfo.dwFontStyleFlags;
        TimedText3GPP.dwFontSize              = TextInfo.dwFontSize;
        TimedText3GPP.dwFontListSize          = TextInfo.dwFontListSize;
        TimedText3GPP.dwDuration              = TextInfo.dwDuration;

        QtvPlayer::Get3GPPTimedTextBGRGB(TimedText3GPP.BackgroundColorRGBA, MM_MPEG4_KDDI_MAX_TEXT_COLORS);
        QtvPlayer::Get3GPPTimedTextTxtRGB(TimedText3GPP.TextColorRGBA, MM_MPEG4_KDDI_MAX_TEXT_COLORS);

        if(TimedText3GPP.dwFontListSize)
        {
          TimedText3GPP.pFontList = (AEETTFontInfo *)MALLOC(sizeof(AEETTFontInfo)*TimedText3GPP.dwFontListSize);
          if(TimedText3GPP.pFontList)
          {
            uint16 i;
            for(i=0; i<(uint16)TimedText3GPP.dwFontListSize; i++)
            {
              QtvPlayer::Get3GPPFontInfoAt( &(TimedText3GPP.pFontList[i].nID),
                                            TimedText3GPP.pFontList[i].Name,
                                            MM_MPEG4_3GPP_MAX_FONT_LEN,
                                            i );
            }
          }
        }
        bRet = true;
      }
	  FREE(pTextData);
    }
  }
  return bRet;
}
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

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

#endif   // FEATURE_APP_MPEG4
