
/* =======================================================================
                               qtvplayer.cpp
DESCRIPTION
  Mpeg-4 player API

EXTERNALIZED FUNCTIONS
  List functions and a brief description that are exported from this file

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Detail how to initialize and use this service.  The sequencing aspect
  is only needed if the order of operations is important.

Portions copyrighted by PacketVideo Corporation;
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated;
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/player/playerif/main/latest/src/qtvplayer.cpp#77 $
$DateTime: 2010/11/09 04:48:52 $
$Change: 1509879 $

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qcplayer.h"
#include "QtvPlayer.h"
#include "Mpeg4Player.h"
#include "Events.h"

#ifndef FEATURE_WINCE
#include "AEESound.h"
#endif//FEATURE_WINCE

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
#include "timedtext.h"
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
#include "media.h"
#include "mpeg4file.h"

#include "rtsp_oem_headers.h"
// To set data port range
#include "NetBase.h"

#include "QtvConfig.h"
#ifdef FEATURE_QTV_APP_PROC_SLEEP
#error code not present
#endif
#ifdef PLATFORM_LTK
#error code not present
#endif //PLATFORM_LTK

#ifdef FEATURE_QTV_RTSTREAMING_INCOMING_VTCALL
#if (!defined FEATURE_QTV_QDSP_RELEASE_RESTORE ) || (!defined FEATURE_QTV_QOS_SELECTION )
#error Cannot enable FEATURE_QTV_RTSTREAMING_INCOMING_VTCALL without FEATURE_QTV_QDSP_RELEASE_RESTORE and FEATURE_QTV_QOS_SELECTION
#endif
#endif /*  FEATURE_QTV_RTSTREAMING_INCOMING_VTCALL */

#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
#include "QtvRealPlayer.h"
#include "QtvRAMFilePlayer.h"
#include "ctype.h"
#endif


#ifdef FEATURE_QTV_DRM_DCF
  #include "QtvPlayerDispatch.h"
#endif

#include "qtv_conc_mgr.h"

#ifdef FEATURE_QTV_MAPI_MEDIA_MASK
#error code not present
#endif
#ifdef FEATURE_QTV_GENERIC_BCAST_FLO
#error code not present
#endif
#ifdef FEATURE_QTV_MAPI_REPLAY
#error code not present
#endif
#ifdef FEATURE_QTV_MAPI_STREAM_MEDIA
#include "qtv_mapi.h"
#include "streamMAPISourceGUIDs.h"
#include "streamMAPISource.h"
#endif

#ifdef FEATURE_QTV_DIVX_DRM
#include "Qtv_DivxRegistration.h"
#endif

#define GEN_AUDIO_FMT_IS_SYNC(x) \
  (((byte *)(x))[0] == 0xFF && (((byte *)(x))[1] & 0xF0) == 0xF0)
#define GEN_AUDIO_FMT_IS_ID3V2_TAG(x) \
  (((byte *)(x))[0] == 0x49 && ((byte *)(x))[1] == 0x44 && \
                                                      ((byte *)(x))[2] == 0x33)
#define GEN_AUDIO_FMT_IS_AAC_SYNC(x) \
  (((byte *)(x))[0] == 0xFF && (((byte *)(x))[1] & 0xF6) == 0xF0)
#define GEN_AUDIO_FMT_IS_AAC_ADIF(x) \
  (\
      (x)[0] == 'A' && \
      (x)[1] == 'D' && \
      (x)[2] == 'I' && \
      (x)[3] == 'F' )
#include "qtv_diag_main.h"
/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#ifdef FEATURE_QTV_APP_PROC_SLEEP
#error code not present
#endif

/* all the ASF files starts with Header Object GUID, so if first 16 bytes of file matches the
 * array below (which is GUID for ASF Header Object), we can safely assume that file/buffer is
 * an ASF object
 */
unsigned char asfFileIdentifier[] =
  {0x30, 0x26, 0xb2, 0x75, 0x8e, 0x66, 0xcf, 0x11, 0xa6, 0xd9, 0x00, 0xaa, 0x00, 0x62, 0xce, 0x6c};


/* How long to wait for the player's state to go to IDLE */
#define PLAYER_TRANSITION_TO_IDLE_TIMEOUT 25000 /* in ms */

/* How often to poll the player for its state to go to IDLE */
#define PLAYER_STATE_POLL_INTERVAL          200 /* in ms */

/* Initial Poll Interval for checking player state*/
#define PLAYER_STATE_POLL_START_INTERVAL     20 /* in ms */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
#ifdef FEATURE_MEMUTILS_NEW_DELETE
  extern "C" void HeapCacheAddRef(void);
  extern "C" void HeapCacheRelease(void);
#endif

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */
  QtvPlayer::InstanceHandleT QtvPlayer::QtvInstances[QTV_PRIORITY_COUNT] = {0};
  uint32 QtvPlayer::guQtvPlayerResourceRefCnt = 0;
  uint32 QtvPlayer::numRegisteredQtvPlayers = 0;
  uint32 QtvPlayer::numQtvPlayers = 0;
#ifdef FEATURE_QTV_GENERIC_BCAST
  uint32 QtvPlayer::uTransID = 0;
#endif

/*---------------------------------------------------------------------------
  This is an array of strings that correspond to the enum codes in the
  PlayerStatusT. There is a 1 to 1 mapping of strings to PlayerStatusT enum
  codes. Some of the enum codes are subject to specific FEATUREs being defined.
  Regardless of the FEATUREs defined though, all codes are initialized to the
  same values and are used to address this PlayerStatusString[] array.
  So in this array we must always have all the strings defined.
---------------------------------------------------------------------------*/
const char * QtvPlayer::PlayerStatusString[] =
{
  /*--------------------------------------------------
    Completion codes for asynchronous commands
    PLEASE DO NOT PUT ANY FEATURE IN THIS TABLE
  --------------------------------------------------*/
   "QTV_COMMAND_OPEN_URN_COMPLETE"
   /* A command to open a URN was successful */

  ,"QTV_COMMAND_OPEN_URN_FAILED"
  /* A command to open a URN failed */

  ,"QTV_COMMAND_REGISTER_FOR_CALLBACK_COMPLETE"
  /* Registering a callback function successful */

  ,"QTV_COMMAND_REGISTER_FOR_CALLBACK_FAILED"
  /* Registering a callback function failed */

  ,"QTV_COMMAND_PLAY_CLIP_COMPLETE"
  /* A command to play the clip was successful */

  ,"QTV_COMMAND_PLAY_CLIP_FAILED"
  /* A command to play the clip failed */

  ,"QTV_COMMAND_STOP_COMPLETE"
  /* A command to stop the clip was successful */

  ,"QTV_COMMAND_STOP_FAILED"
  /* A command to stop the clip failed */

  ,"QTV_COMMAND_PAUSE_COMPLETE"
  /* A command to pause the clip was successful */

  ,"QTV_COMMAND_PAUSE_FAILED"
  /* A command to pause the clip failed */

  ,"QTV_COMMAND_NEXT_FRAME_COMPLETE"
  ,"QTV_COMMAND_NEXT_FRAME_FAILED"
  ,"QTV_COMMAND_PREV_FRAME_COMPLETE"
  ,"QTV_COMMAND_PREV_FRAME_FAILED"

  /*--------------------------------------------------
     Decoded frame notification
  --------------------------------------------------*/
  ,"QTV_PLAYER_STATUS_DECODE_FRAME"
   /* Pass decoded frame back to OEM layer */

  /*--------------------------------------------------
    Playback completion notifications.
  --------------------------------------------------*/
  ,"QTV_PLAYER_STATUS_PLAYBACK_COMPLETE"
    /* Playback completion notice: normal termination. */

  ,"QTV_PLAYER_STATUS_PLAYBACK_ERROR_ABORT"
    /* Playback completion notice: error termination. */

  ,"QTV_PLAYER_STATUS_PLAYBACK_STOPPED"
    /* Playback completion notice: Playback stopped by an API command. */

  /*--------------------------------------------------
    Player state notifications.
  --------------------------------------------------*/
  ,"QTV_PLAYER_STATUS_CONNECTING"
    /* Playback status update: connecting to server. */

  ,"QTV_PLAYER_STATUS_PLAYBACK_READY"
    /* Playback status update: ready to play clip. */

  ,"QTV_PLAYER_STATUS_BUFFERING"
    /* Playback status update: data is buffering. */

  ,"QTV_PLAYER_STATUS_OPENING"
    /* Playback status update: opening and processing file. */

  ,"QTV_PLAYER_STATUS_PLAYING"
    /* Playback status update: data is playing. */

  ,"QTV_PLAYER_STATUS_PAUSED"
    // Playback status update: paused.

  ,"QTV_PLAYER_STREAM_AUTH_REQUIRED"
     // Stream requires the user to provide the authorization

  ,"QTV_PLAYER_STATUS_REPOSITIONING"
    /* Playback status update: suspended while repositioning. */

  ,"QTV_PLAYER_STATUS_IDLE"
    /* Overall player state update: playback and download
       are both idle. */

  ,"QTV_PLAYER: next 3GPP timed-text sample ready"
    //"QTV_PLAYER_STATUS_NEXT_TEXT_READY"
    /* Playback status update: next 3GPP timed-text sample
       ready for rendering. */

  ,"QTV_PLAYER: next telop timed-text sample ready"
    //"QTV_PLAYER_STATUS_NEXT_TELOP_READY"
    /* Playback status update: next telop timed-text sample
       ready for rendering */

   /*--------------------------------------------------
     Playback error notifications.
   --------------------------------------------------*/
  ,"QTV_PLAYER_STATUS_AUDIO_ERROR" // = 29
    /* An audio playback error occurred--audio will terminate. */

  ,"QTV_PLAYER_STATUS_VIDEO_ERROR"
    /* A video playback error occurred--video will terminate. */


  ,"QTV_PLAYER: Timed text playback error. Processing will terminate."
    //"QTV_PLAYER_STATUS_TEXT_ERROR"
    /* A timed text playback error occurred--timed text processing will terminate. */

  ,"QTV_PLAYER_STATUS_STREAM_ERROR" // = 32
    /* Streaming server error. */

  ,"QTV_PLAYER_STATUS_AUDIO_ERROR_FORMAT_NOT_SUPPORTED"
    /* Audio format of the clip is not supported.  Audio will not play. */

  ,"QTV_PLAYER_STATUS_VIDEO_ERROR_FORMAT_NOT_SUPPORTED"
    /* Video format of the clip is not supported.  Video will not play. */

  ,"QTV_PLAYER:Audio format not supported and Video-only playback not allowed"
   //"QTV_PLAYER_STATUS_VIDEO_ONLY_PLAYBACK_NOT_ALLOWED"
   /* Audio format of the clip is not supported and
       video-only playback is not allowed.
       Playback will terminate */

  ,"QTV_PLAYER:Video format not supported and Audio-only playback not allowed"
   //"QTV_PLAYER_STATUS_AUDIO_ONLY_PLAYBACK_NOT_ALLOWED"
   /* Video format of the clip is not supported and
       audio-only playback is not allowed.
       Playback will terminate */

  ,"QTV_PLAYER: Illegal Start or Stop time. Playback terminate"
   //"QTV_PLAYER_STATUS_PLAY_TIME_ERROR"
   /* Start time is beyond the end of clip
       or stop time is before start time.
       Playback will terminate. */

  ,"QTV_PLAYER_STATUS_NO_SYNC_FRAME_ERROR"
    /* No Sync (I_Frame) frame present in the
       media to complete re-positioning. So
       repositioning failed. */

  ,"QTV_PLAYER_STATUS_NEXT_FRAME_DONE"
    /* Step Next Frame operation completed successfully */

  ,"QTV_PLAYER_STATUS_NEXT_FRAME_FAILED"
    /* Step Next Frame operation failed to complete */

  ,"QTV_PLAYER_STATUS_PREV_FRAME_DONE"
    /* Step back to previous Frame completed successfully */

  ,"QTV_PLAYER_STATUS_PREV_FRAME_FAILED"
    /* Step back to previous Frame Failed. This may happen,
       if no previous decoded frames are in buffer */

  ,"QTV_PLAYER_STATUS_ROTATION_DONE" // = 43
    /* Frame Rotation completed successfully */

  ,"QTV_PLAYER_STATUS_ROTATION_FAILED"
    /* Frame Rotation failed, this may be due to
       unsupported value */

  ,"QTV_PLAYER_STATUS_SCALING_DONE"
    /* Frame Scaling completed successfully */

  ,"QTV_PLAYER_STATUS_SCALING_FAILED"
    /* Frame Scaling failed, this may be due to
       unsupported value */

  ,"QTV_PLAYER_STATUS_PSEUDO_PAUSE"
    /* Player paused due to lack of data */

  ,"QTV_PLAYER_STATUS_PSEUDO_RESUME"
  /*  Player resumed as enough data is available */

  ,"QTV_PS_COMMAND_OPEN_STREAM_COMPLETE"
  /*  Opened a pseudo stream session successfully*/

  ,"QTV_PS_COMMAND_OPEN_STREAM_FAILED"
  /* Open pseudo stream command failed */

  ,"QTV_PS_PARSER_STATUS_PAUSE"
  /* Qtv parser paused as enough data is not available */

  ,"QTV_PS_PARSER_STATUS_RESUME"
  /* Qtv parser resumed as enough data is available */

  ,"QTV_PS_PARSER_STATUS_READY"
  /* Qtv parser has parsed a fragment */

  ,"QTV_PS_PARSER_STATUS_FAILED"
  /* Qtv parser has failed to parse a file fragment */

  ,"QTV_PSEUDO_STREAM_ALREADY_ACTIVE"
  /* a pseudo stream is already active in Qtv */

  ,"QTV_PS_PLAYER_BUFFERING"
  /* Qtv is buffering pseudo stream session data */

  ,"QTV_COMMAND_ROTATE_COMPLETE"
  /* The command to rotate the clip completed */

  ,"QTV_COMMAND_ROTATE_FAILED"
  /* The command to rotate the clip failed */

  ,"QTV_PLAYER_STATUS_VIDEO_RESTARTING_TRACK"
  /* in loop track mode, video track is restarting */

  ,"QTV_PLAYER_STATUS_AUDIO_RESTARTING_TRACK"
  /* in loop track mode, audio track is restarting */

  ,"QTV_PLAYER_STATUS_TEXT_RESTARTING_TRACK"
  /* in loop track mode, text track is restarting */

  ,"QTV_PLAYER_LOOP_TRACK_NOT_SUPPORTED"
  /* loop track mode is not supported for this media */

  ,"QTV_COMMAND_PAUSE_VIDEO_COMPLETE"
  /* The command to pause video completed */

  ,"QTV_COMMAND_PAUSE_VIDEO_FAILED"
  /* The command to pause video failed */

  ,"QTV_COMMAND_PAUSE_AUDIO_COMPLETE"
  /* The command to pause AUDIO completed */

  ,"QTV_COMMAND_PAUSE_AUDIO_FAILED"
  /* The command to pause AUDIO failed */

  ,"QTV_COMMAND_PAUSE_TEXT_COMPLETE"
  /* The command to pause text completed */

  ,"QTV_COMMAND_PAUSE_TEXT_FAILED"
  /* The command to pause text failed */

  ,"QTV_COMMAND_RECORD_CLIP_COMPLETED"
  /* Record clip command is complete */

  ,"QTV_COMMAND_RECORD_CLIP_FAILED"
  /* Record clip command failed */

  ,"QTV_COMMAND_STOP_RECORDING_COMPLETED"
  /* Record stop command is completed */

  ,"QTV_COMMAND_STOP_RECORDING_FAILED"
  /* Record stop command failed */

  ,"QTV_REC_FILE_SYS_LIMIT_APPROACHING"
  /* File sys limit is approaching */

  ,"QTV_REC_FILE_SYS_FULL"
  /* File system is full */

  ,"QTV_RECORDER_FILE_ALREADY_EXISTS"
  /* File already exists */

  ,"QTV_RECORDER_FILE_WRITE_ERROR"
  /* Error occured while writing to the file system */

  ,"QTV_REDORDER_FILE_OW_FAILURE"
  /* File to overwrite a file name*/

  ,"QTV_RECORDER_FAILED"
  /* Stream recorder failed */

  ,"QTV_PLAYER_STATUS_REDIRECT"
    /* Qtv get redirect request */

  ,"QTV_COMMAND_RESUME_VIDEO_COMPLETE"
  /* The command to resume video completed */

  ,"QTV_COMMAND_RESUME_VIDEO_FAILED"
  /* The command to resume video failed */

  ,"QTV_COMMAND_RESUME_AUDIO_COMPLETE"
  /* The command to resume AUDIO completed */

  ,"QTV_COMMAND_RESUME_AUDIO_FAILED"
  /* The command to resume AUDIO failed */

  ,"QTV_COMMAND_RESUME_TEXT_COMPLETE"
  /* The command to resume text completed */

  ,"QTV_COMMAND_RESUME_TEXT_FAILED"
  /* The command to resume text failed */

  ,"QTV_MIDI_DATA_PRESENT"

  ,"QTV_COMMAND_SKIP_CLIP_COMPLETE"
  /* The command to resume text completed */

  ,"QTV_COMMAND_SKIP_CLIP_FAILED"
  /* The command to resume text failed */

  ,"QTV_PLAYER_STATUS_NO_RANDOM_ACCESS_POINT_ERROR"
  /* No Random Access point found */

  ,"QTV_PLAYER_STATUS_SEEK_FAILED"
  /* Reposition attempt failed */

  ,"QTV_COMMAND_PARSE_FRAGMENT_COMPLETE"
   /* A command to parse fragment was successful */

  ,"QTV_COMMAND_PARSE_FRAGMENT_FAILED"
   /* A command to parse fragment failed */

   ,"QTV_PLAYER_RECORDING_COMPLETED"
    /* Player completed clip recording */

   ,"QTV_PLAYER_STATUS_VIDEO_MEDIA_END"
    /* End of video track */

   ,"QTV_PLAYER_STATUS_AUDIO_MEDIA_END"
    /* End of audio track */

   ,"QTV_PLAYER_STATUS_TEXT_MEDIA_END"
    /* End of text track */

   ,"QTV_PLAYER_STATUS_DOWNLOAD_COMPLETE"
   ,"QTV_PLAYER_STATUS_DOWNLOAD_ERROR_ABORT"
   ,"QTV_PLAYER_STATUS_DOWNLOAD_STOPPED"

   ,"PV_COMMAND_DELETE_DOWNLOAD_SESSION_COMPLETE"
   ,"PV_COMMAND_DELETE_DOWNLOAD_SESSION_FAILED"
   ,"PV_COMMAND_DOWNLOAD_CLIP_COMPLETE"
   ,"PV_COMMAND_DOWNLOAD_CLIP_FAILED"
   ,"PV_COMMAND_RESUME_DOWNLOAD_COMPLETE"
   ,"PV_COMMAND_RESUME_DOWNLOAD_FAILED"
   ,"PV_COMMAND_STOP_DOWNLOAD_COMPLETE"
   ,"PV_COMMAND_STOP_DOWNLOAD_FAILED"
   ,"PV_PLAYER_STATUS_DOWNLOAD_TEMPFILE_ERROR"

   ,"QTV_HTTP_COMMAND_OPEN_STREAM_COMPLETE"
    /*Open HTTP Stream command complete*/

   ,"QTV_HTTP_COMMAND_OPEN_STREAM_FAILED"
    /*Open HTTP STream command failed*/

  ,"QTV_HTTP_PLAYER_BUFFERING"
    /*HTTP Buffering not enough data to play*/

  ,"QTV_PLAYER_STATUS_HTTP_PAUSE"
    /* HTTP player paused not enough data to play */

  ,"QTV_PLAYER_STATUS_HTTP_RESUME"
    /* HTTP player Resumed after HTTP_PAUSE */

  ,"QTV_HTTP_PARSER_STATUS_PAUSE"

  ,"QTV_HTTP_PARSER_STATUS_RESUME"

  ,"QTV_HTTP_PARSER_STATUS_READY"
  /* Qtv parser has parsed the first Fragment */

  ,"QTV_HTTP_PARSER_STATUS_FAILED"
  /* Qtv parser failed in parsing */

  ,"QTV_PLAYER_STATUS_VIDEO_RELEASE_DONE"
  /* Command to release video DSP completed */

  ,"QTV_PLAYER_STATUS_VIDEO_RELEASE_FAILED"
  /* Command to release video DSP failed */

  ,"QTV_UPDATE_HTTP_STREAM_COMPLETE"
  /* More data was written to the HTTP Stream buffer*/

  ,"QTV_UPDATE_HTTP_STREAM_FAILED"
  /* Data could not be written to the HTTP Stream buffer*/

  ,"QTV_PLAYER_STREAM_SERVER_CLOSED"
  /* Streaming server closed connection */

  ,"QTV_PLAYER_TS_LAYER_PRESENT"
  /* Temporal scalability layer present in clip */

  ,"QTV_PLAYER_STATUS_RELEASE_QOS_INPROGRESS"
  /* QoS Release in Progress */

  ,"QTV_PLAYER_STATUS_RELEASE_QOS_DONE"
  /* QoS Release Complete */

  ,"QTV_PLAYER_STATUS_VIDEO_RELEASE_INPROGRESS"
  /* Video DSP Release In Progress */

  ,"QTV_STREAM_UNSUPPORTED_MEDIA_TYPE"
  /*unsupported media clip*/

  ,"QTV_STREAM_RATE_ADAPTATION_SUPPORTED"
  /*Rate Adaptation supported*/

  ,"QTV_PLAYER_STATUS_VIDEO_RESOLUTION_NOT_SUPPORTED"
  /* video dimensions not supported */

  ,"QTV_BUFFER_TO_FILE_COPY_DISALLOWED"
  ,"QTV_BUFFER_TO_FILE_COPY_FILE_ALREADY_EXISTS"
  ,"QTV_BUFFER_TO_FILE_COPY_COMPLETE"
  ,"QTV_BUFFER_TO_FILE_COPY_FAILED"
  ,"QTV_BUFFER_TO_FILE_COPY_NOT_ENOUGH_SPACE"

  ,"QTV_PLAYER_STATUS_SUSPENDED"
  /* Playback status update: suspended */

  ,"QTV_PLAYER_STATUS_SUSPEND_FAILED"
  /* Playback status update: suspend failed */

  ,"QTV_PLAYER_STATUS_SUSPENDING"
  /* Playback status update: internal suspend, ignore key press */

  ,"QTV_PLAYER_STATUS_DOWNLOAD_READY"
  ,"QTV_PLAYER_STATUS_DOWNLOADING"
  ,"QTV_PLAYER_STATUS_DOWNLOADED_DATA_AVAIL"
  ,"QTV_PLAYER_STATUS_DOWNLOAD_ABORT_MEM_FAIL"

  ,"QTV_PLAYER_STATUS_INFO"
  /* Status info: Player has status information for the application layer */

   ,"QTV_COMMAND_REGISTER_DRM_DECRYPT_METHOD_COMPLETE"
   /* Playback status update: Player request for registering DRM decript method is complete */

   ,"QTV_COMMAND_REGISTER_DRM_DECRYPT_METHOD_FAILED"
   /* Playback status update: Player request for registering DRM decript method is failed */

  ,"QTV_COMMAND_PLAYLIST_PLAY_COMPLETE"
  /* A command to seek to a clip in the playlist was successful */

  ,"QTV_COMMAND_PLAYLIST_PLAY_FAILED"
  /* A command to seek to a clip in the playlist was successful */

  ,"QTV_PLAYER_STATUS_CLIP_TRANSITION"
  /* Playback on the next clip in a playlist has started */

  ,"QTV_PLAYER_STATUS_PLAYLIST_PLAY_COMPLETE"
  /* A user-initiated skip in a playlist succeeded */

  ,"QTV_PLAYER_STATUS_PLAYLIST_PLAY_FAILED"
  /* A user-initiated skip in a playlist failed */

  //When Audio bit rate greater than max allowed.
  ,"QTV_PLAYER_AUDIO_UNSUPPORTED_BIT_RATE"

  //When video bit rate greater than max allowed.
  ,"QTV_PLAYER_VIDEO_UNSUPPORTED_BIT_RATE"

  //When video profile is not supported.
  ,"QTV_PLAYER_VIDEO_UNSUPPORTED_PROFILE"

   //When video profile is not supported.
  ,"QTV_PLAYER_STATUS_UNSUPPORTED_CODEC"

  /* For pseudo stream buffer update succes*/
  ,"QTV_UPDATE_PSEUDO_STREAM_COMPLETE"

  /* For pseudo stream buffer update fail*/
  ,"QTV_UPDATE_PSEUDO_STREAM_FAILED"

  //When error is encountered while creating IxStream for video
  ,"QTV_PLAYER_VIDEO_IXSTREAM_FAILED"

  //When error is encountered while creating IxStream for audio
  ,"QTV_PLAYER_AUDIO_IXSTREAM_FAILED"

  //When error is encountered while creating IxStream for text
  ,"QTV_PLAYER_TEXT_IXSTREAM_FAILED"

  ,"QTV_PLAYER_STATUS_PLAY_NOT_SUPPORTED"
  /* When Mpeg4Player is suspended and In-call QTV is not supported and if an
     attempt is made to playback. */

  ,"QTV_PLAYER_STATUS_PAUSED_SUSPENDED"
  /* When Mpeg4Player is suspended both internally (due to incoming call)
     and externally (MP is suspended or has issued a pause). */

  ,"QTV_PLAYER_COMMAND_SEEK_SYNC_DONE"
  /* QTV_FEATURE_BCAST_GENERIC : SEEK SYNC Executed */

  ,"QTV_PLAYER_COMMAND_SEEK_SYNC_FAILED"
  /* QTV_FEATURE_BCAST_GENERIC : SEEK SYNC Failed   */

  ,"QTV_PLAYER_COMMAND_PLAY_SYNC_FRAME_DONE"
  /* QTV_FEATURE_BCAST_GENERIC : PLAY SYNC Executed */

  ,"QTV_PLAYER_COMMAND_PLAY_SYNC_FRAME_FAILED"
  /* QTV_FEATURE_BCAST_GENERIC : PLAY SYNC Failed */

  ,"QTV_PLAYER_COMMAND_REPOSITION_DONE"
  /* QTV_FEATURE_BCAST_GENERIC : REPOSITION DONE */

  ,"QTV_PLAYER_COMMAND_REPOSITION_FAILED "
  /* QTV_FEATURE_BCAST_GENERIC : REPOSITION Failed     */

  ,"QTV_PLAYER_STATUS_TRACK_LIST_UPDATE "
  /* QTV_FEATURE_BCAST_GENERIC : Track List Update */

  ,"QTV_PLAYER_COMMAND_MUTE_COMPLETE"
  /* QTV_FEATURE_BCAST_GENERIC : MUTE COMPLETE */

  ,"QTV_PLAYER_COMMAND_MUTE_FAILED  "
  /* QTV_FEATURE_BCAST_GENERIC : MUTE FAILED */

  ,"QTV_PLAYER_COMMAND_UNMUTE_COMPLETE "
  /* QTV_FEATURE_BCAST_GENERIC : UNMUTE COMPLETE */

  ,"QTV_PLAYER_COMMAND_UNMUTE_FAILED   "
  /* QTV_FEATURE_BCAST_GENERIC : UNMUTE FAILED */

  ,"QTV_PLAYER_COMMAND_MUTE_ALL_FAILED"
  /* QTV_FEATURE_BCAST_GENERIC : MUTE ALL FAILED */

  ,"QTV_PLAYER_COMMAND_SELECT_PB_TRACK_COMPLETE"
  /* QTV_FEATURE_BCAST_GENERIC : SELECT PB TRACK DONE */

  ,"QTV_PLAYER_COMMAND_SELECT_PB_TRACK_FAILED"
  /* QTV_FEATURE_BCAST_GENERIC : SELECT PB TRACK Failed */

  ,"QTV_PLAYER: next generic text sample ready"
  // QTV_PLAYER_STATUS_NEXT_GENERIC_TEXT_READY
  /* Playback status update: next simple text sample
     ready for rendering. */

  ,"QTV_PLAYER_STATUS_END_OF_TSB"
  /* End of time shift buffer reached */

  ,"QTV_PLAYER_STATUS_TSB_OVERFLOW"
  /* Time shift buffer overflow */

  ,"QTV_PLAYER_STATUS_TSB_DATA_LOSS"
  /* Time shift buffer data loss */

  ,"QTV_PLAYER_STATUS_PLAYING_1P3X"
  /* Playback status update: data is playing at 1.3X speed. */

  ,"QTV_PLAYER_STATUS_TRACK_STATE_CHANGE"
  /* Track information has changed. */

  ,"QTV_PLAYER_DRM_AUTHORIZATION_ERROR"
  /* User Authorization Error */

  ,"QTV_PLAYER_DRM_DEVICE_REGISTRATION_ERROR"
  /* Device is not registered for DRM playback */

  ,"QTV_PLAYER_DRM_RENTAL_COUNT_EXPIRED"
  /* Rental Count has expired. */

  ,"QTV_PLAYER_DRM_PLAYBACK_GENERAL_ERROR"
  /* General DRM playback error */

  ,"QTV_PLAYER_CONSUME_RENTAL_VIEW_CONFIRMATION"
  /* Confirmation needed from user before consuming view count */

  ,"QTV_PLAYER_STATUS_SERVER_TIMEOUT"
  /*when real player gets timeout alert from server*/

  ,"QTV_PLAYER_STATUS_SWITCHING_IN_PROGRESS"
   /* Switch to new URL inProgress */

  ,"QTV_PLAYER_STATUS_SWITCH_STREAM_FAILED"
   /* Switch failed */

   , "QTV_PLAYER_STATUS_FCS_PLAYBACK_COMPLETE"
     /* FCS clip playback completed*/

  , "QTV_COMMAND_PROBE_COMPLETE"
    /* FCS probe command complete*/

   , "QTV_COMMAND_PROBE_FAILED"
    /* FCS probe command failed*/

  ,"QTV_PLAYER_STATUS_SERVER_NOT_ENOUGH_BW"
  /* SERVER Not having enough BW */

  ,"QTV_PLAYER_STATUS_VALID_CLIPINFO_DIMENSIONS"
  /*Valid CLip Info (width and height) notification*/

  ,"QTV_PLAYER_STATUS_DATA_INACTIVITY_ERROR"
  /* inactivity timer expire */

 ,"QTV_STREAM_TRACKLIST_UNKNOWN_CODEC"
   /* Stream mime type not supported/unknown codec */

 ,"QTV_PLAYER_STATUS_RECONNECT_SUCCESS"   
  /* Reconnect to the server is successfull*/

  ,"QTV_PLAYER_STATUS_RECONNECT_FAIL"
  /* Reconnect to server failed*/

   ,"QTV_PLAYER_STATUS_RECONNECT_IN_PROGRESS"
   /* Reconnect is in progress*/

  ,"QTV_PLAYER_STATUS_FCS_SWITCH_SUPPORTED"

   ,"QTV_PLAYER_STATUS_RECONNECTING_USING_TCP_INTERLEAVE"
   /* Reconnecting using tcp interleaving*/

    /*session supports switch */

}; /* PlayerStatusString[] */

#if defined(FEATURE_REAL_PLAYER_USE_QTV_API) && defined(FEATURE_QTV_NET_POLICY)
extern "C" void RealPlayer_SetPDPProfileNumber(uint8 PDPProfileNum);
#endif


////////////////////////////////////////////////////////////////////
//
// API Thread event tables.
//
////////////////////////////////////////////////////////////////////

EVENT_POOL( PV_API_POOL, 60 )
  //to player thread
EVENT_SOURCE( PV_OPEN_URN, PV_API_POOL )
EVENT_SOURCE( PV_OPEN_BUFFER_URN, PV_API_POOL )

EVENT_SOURCE( QTV_MUTE, PV_API_POOL )

#ifdef FEATURE_QTV_GENERIC_BCAST
EVENT_SOURCE( QTV_SELECT_PB_TRACK, PV_API_POOL )
EVENT_SOURCE( QTV_REPOSITION, PV_API_POOL )
EVENT_SOURCE( QTV_SEEK_SYNC, PV_API_POOL )
EVENT_SOURCE( QTV_PLAY_SYNC, PV_API_POOL )
#endif /* FEATURE_QTV_GENERIC_BCAST */

EVENT_SOURCE( PV_PLAY_CLIP, PV_API_POOL )
EVENT_SOURCE( PV_REGISTER_FOR_CALLBACK, PV_API_POOL )
EVENT_SOURCE( PV_REGISTER_FOR_CALLBACK_EX, PV_API_POOL )
EVENT_SOURCE( PV_SET_PREROLL_BUFFER_SIZE, PV_API_POOL )
EVENT_SOURCE( PV_SET_VIDEO_WINDOW_POS, PV_API_POOL )
EVENT_SOURCE( PV_SET_ZOOM, PV_API_POOL )
EVENT_SOURCE( PV_SET_DISPLAY_QUEUE_DEPTH, PV_API_POOL )
EVENT_SOURCE( PV_SET_DISPLAY_AREA, PV_API_POOL )
EVENT_SOURCE( PV_PAUSE, PV_API_POOL )
#if (defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION)
#error code not present
#endif
#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
EVENT_SOURCE( QTV_PAUSE_AUDIO, PV_API_POOL )
EVENT_SOURCE( QTV_PAUSE_VIDEO, PV_API_POOL )
EVENT_SOURCE( QTV_PAUSE_TEXT, PV_API_POOL )
EVENT_SOURCE( QTV_RESUME_AUDIO, PV_API_POOL )
EVENT_SOURCE( QTV_RESUME_VIDEO, PV_API_POOL )
EVENT_SOURCE( QTV_RESUME_TEXT, PV_API_POOL )
#endif

#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
EVENT_SOURCE( QTV_HTTP_BUFFER_UPDATE, PV_API_POOL )
EVENT_SOURCE( QTV_HTTP_EVENT, PV_API_POOL )
#endif


EVENT_SOURCE( PV_STOP, PV_API_POOL )
EVENT_SOURCE( PV_DELETE_DOWNLOAD_SESSION, PV_API_POOL )
EVENT_SOURCE( PV_DOWNLOAD_CLIP, PV_API_POOL )
EVENT_SOURCE( PV_RESUME_DOWNLOAD, PV_API_POOL )
EVENT_SOURCE( PV_STOP_DOWNLOAD, PV_API_POOL )
EVENT_SOURCE( PV_NEXT_FRAME, PV_API_POOL )
EVENT_SOURCE( PV_PREV_FRAME, PV_API_POOL )
#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
  EVENT_SOURCE(PV_VIDEO_ROTATE, PV_API_POOL );
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */
#if defined (FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined (FEATURE_QTV_XSCALE_VIDEO)
EVENT_SOURCE(PV_VIDEO_SCALE, PV_API_POOL )
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS || FEATURE_QTV_XSCALE_VIDEO */
EVENT_SOURCE(PV_SET_AUDIOOVER, PV_API_POOL )
#ifdef FEATURE_QTV_PSEUDO_STREAM
   /* May be this event has to go into QTV_PARSER_POOL*/
EVENT_SOURCE(QTV_PS_PARSER_STATUS_EVENT, PV_API_POOL)
#endif /* FEATURE_FILE_FRAGMENTATION */

#ifdef FEATURE_QTV_PSEUDO_STREAM /* FEATURE_QTV_PSEUDO_STREAM */
   EVENT_SOURCE(QTV_PS_STREAM_OPEN_EVENT, PV_API_POOL);

EVENT_SOURCE(QTV_PS_PROCESS_PSEUDO_STREAM, PV_API_POOL)

   /* This event has to go into AUDIO_POOL or VIDEO_POOL or TEXT_POOL*/
EVENT_SOURCE(QTV_PS_PAUSE_EVENT, PV_API_POOL)

EVENT_SOURCE(QTV_PS_UPDATE_WBUFFER_OFFSET_EVENT, PV_API_POOL)
#endif /* FEATURE_QTV_PSEUDO_STREAM */

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
EVENT_SOURCE(QTV_HTTP_STREAM_OPEN_EVENT, PV_API_POOL)
EVENT_SOURCE(QTV_HTTP_STREAM_UPDATE_WBUFFER_OFFSET_EVENT, PV_API_POOL)
EVENT_SOURCE(QTV_HTTP_PARSER_STATUS_EVENT, PV_API_POOL)
EVENT_SOURCE(QTV_HTTP_STREAM_BUFFER_UNDERRUN_EVENT, PV_API_POOL)
EVENT_SOURCE(QTV_PROCESS_HTTP_STREAM, PV_API_POOL)
#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */

EVENT_SOURCE( PV_PARSER_EVENT_EXIT, PV_API_POOL )

#ifdef FEATURE_QTV_STREAM_RECORD
EVENT_SOURCE( PV_RECORD_CLIP, PV_API_POOL )
EVENT_SOURCE( PV_RECORD_STOP, PV_API_POOL )
#endif /* FEATURE_QTV_STREAM_RECORD */

#ifdef FEATURE_FILE_FRAGMENTATION
   EVENT_SOURCE( QTV_SKIP_CLIP, PV_API_POOL );
#endif /*FEATURE_FILE_FRAGMENTATION*/

EVENT_SOURCE( QTV_SUSPEND, PV_API_POOL );
EVENT_SOURCE( QTV_RESUME, PV_API_POOL );

#ifdef FEATURE_QTV_IN_CALL_PHASE_2
#error code not present
#endif /* FEATURE_QTV_IN_CALL_PHASE_2 */

#ifdef FEATURE_QTV_WM_DRM_API
EVENT_SOURCE( QTV_REGISTER_DRM_DECRYPT_METHOD_EVENT, PV_API_POOL );
#endif /* FEATURE_QTV_WM_DRM_API */

#ifdef FEATURE_QTV_PLAYLIST
   EVENT_SOURCE( QTV_SKIP, PV_API_POOL );
#endif /* FEATURE_QTV_PLAYLIST */

#ifdef FEATURE_QTV_HTTP_DOWNLOAD
EVENT_SOURCE(QTV_HTTP_REGISTER_CALLBACK_EVENT, PV_API_POOL);
#endif

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  EVENT_SOURCE( QTV_COPY_BUFFER_TO_FILE, PV_API_POOL );
#endif

/*Event generated with the Audio track loops track*/
EVENT_SOURCE( QTV_AUDIO_RESTARTING_TRACK, PV_API_POOL )

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */
static void SetNullPlayerState(QtvPlayer::PlayerStateRecordT &s);
static void SetNullURNType(QtvPlayer::URNTypeT &t);
static void SetNullURLType(QtvPlayer::URLTypeT &t);
static void SetNullClipInfo(QtvPlayer::ClipInfoT &i);
static void SetNullAudioVideoStatistics(QtvPlayer::AudioVideoStatisticsT &s);
static void SetNullRTPStatistics(QtvPlayer::RTPStatisticsT &s);
#ifdef FEATURE_QTV_MEMORY_LEAK_DEBUG
#error code not present
#endif

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
extern "C" {
extern boolean qtv_cfg_enable_dsp_release_oem;

/* Variable which holds the player state i.e whether it was playing
   or not  before the incoming voice/VT call interrupted the player */
boolean playingBeforeVoiceCall = false;
}
#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif /* FEATURE_QTV_QOS_SELECTION */

#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/* ======================================================================
FUNCTION
  QtvPlayer::QtvPlayer

DESCRIPTION
  Private constructor for the QtvPlayer class.  To create a unique player
  use CreatInstance instead.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

========================================================================== */
QtvPlayer::QtvPlayer()
{
   CurrentMIMEType = NULL;
   CallbackFunction = NULL;
   CallbackClientData = NULL;
   CallbackInstanceFunction = NULL;
   CallbackUserData = NULL;
   InstancePriority=QTV_PRIORITY_INVALID;
   m_registered = false;

   MallocBufferFunction = NULL;
   FreeBufferFunction   = NULL;

   MediaPlayerRootPath[0] = '\0';
   NetPolicy.isSet = FALSE;
   NetPolicy.isQoSEnabled = FALSE;
   NetPolicy.primaryPDPProfileNo = 0;

   descramblerHandler = NULL;

   MediaInfo = NULL;
   MediaInfoSize = 0;
   unprocessedRtspHeaderCallback = 0;
   unprocessedSdpHeaderCallback = NULL;
   pMpeg4Player = NULL;

   #ifdef FEATURE_QTV_DRM_DCF
   pDispatchInstance = NULL;
   pDispatchThreadHandle = NULL;

   #ifdef FEATURE_QTV_UNPROTECTED_THROUGH_DRM
      g_always_through_ixstream = true;
   #else
      g_always_through_ixstream = false;
   #endif

   #endif

   /*---------------------------------------------------------------------------
   The callback function to obtain RTSP result status codes and text
   equivalents.  When the RTSP engine receives a status code of other than
   "Ok", this information is passed to any interested software via the
   function specified in this pointer.  The pointer is set via
   SetRTSPStatusCallback(), and accessed via GetRTSPStatusCallback().  It
   is declared static, so is only available via the access function.
   ---------------------------------------------------------------------------*/
  RTSPStatusT_CB = 0;
  RTSPStatusInstanceT_CB = NULL;
  RTSPStatusT_CB_UserData = NULL;

   /*---------------------------------------------------------------------------
   This is a callback definition for the PlayerStatusT enum type. It is
   initialized to NULL here. It is STATIC to this file and accessed through
   its own access method provided by the class. The purpose is to retain
   its value even after a destruction of the instance of the class.
   ---------------------------------------------------------------------------*/
  PlayerStatusT_CB = 0;
  PlayerStatusInstanceT_CB = NULL;
  PlayerStatusT_CB_UserData = NULL;

#ifdef FEATURE_QTV_MAPI_MEDIA_MASK
#error code not present
#endif
#ifdef FEATURE_QTV_MAPI_REPLAY
#error code not present
#endif
#ifdef FEATURE_QTV_MAPI_STREAM_MEDIA
  /* register stream media source */
  CStreamMAPISource::install();
#endif

#ifdef FEATURE_QTV_GENERIC_BCAST_FLO
#error code not present
#endif

  m_DRMKey = NULL;
  m_DRMKeySize = 0;
  /* --------------------------------------------------------------------------
   RAM file player variables
   ---------------------------------------------------------------------------*/
  RAMPlayer = NULL;

  /* Default setting for playback speed. */
  m_playbackSpeed = PLAYBACK_SPEED_NORMAL;

  m_UserAgentStr[0] = '\0';
  m_UserAgentParam_man[0] = '\0';
  m_UserAgentParam_ctn[0] = '\0';
  m_UserAgentParam_params[0] = '\0';
#ifdef FEATURE_QTV_APP_PROC_SLEEP
#error code not present
#endif
}

/* ======================================================================
FUNCTION
  QtvPlayer::~QtvPlayer

DESCRIPTION
  Private destructor for the QtvPlayer class.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

========================================================================== */
QtvPlayer::~QtvPlayer(void)
{
#ifdef FEATURE_QTV_MAPI_MEDIA_MASK
#error code not present
#endif
#ifdef FEATURE_QTV_MAPI_REPLAY
#error code not present
#endif
#ifdef FEATURE_QTV_MAPI_STREAM_MEDIA
  /* unregister stream media source */
  CStreamMAPISource::uninstall();
#endif

#ifdef FEATURE_QTV_GENERIC_BCAST_FLO
#error code not present
#endif

#ifdef FEATURE_QTV_APP_PROC_SLEEP
#error code not present
#endif
}

/* ======================================================================
FUNCTION
  QtvPlayer::GetPlayer

DESCRIPTION
  Returns the QtvPlayer object associated with the instance handle.  If
  the instance handle is NULL, returns the default instance.  If the
  default instance is NULL, generates Error Fatal.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

========================================================================== */
QtvPlayer *QtvPlayer::GetPlayer(InstanceHandleT handle)
{
  if (!handle)
  {
    //check and error if default handler itself is NULL
    if (QtvInstances[QTV_PRIORITY_DEFAULT])
    {
      handle = QtvInstances[QTV_PRIORITY_DEFAULT];
    }
    else
    {
      ERR_FATAL("called with NULL QtvPlayer default Instance Handle",0,0,0);
    }
  }

  return (QtvPlayer *)handle;
}
/* ======================================================================
FUNCTION
  QtvPlayer::GetRegistrationCode

DESCRIPTION
  Retrieves the DIVX DRM registration code from the device.

DEPENDENCIES
  None

RETURN VALUE
  QTV_RETURN_ERROR if fails else returns QTV_RETURN_OK

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
#ifdef FEATURE_QTV_DIVX_DRM
QtvPlayer::ReturnT QtvPlayer::GetRegistrationCode(char* rcode,int* max_len,
                                                  InstanceHandleT handle,
                                                  InstancePriorityT priority)
#else
QtvPlayer::ReturnT QtvPlayer::GetRegistrationCode(char*,int*,
                                                  InstanceHandleT,
                                                  InstancePriorityT)
#endif
{
#ifdef FEATURE_QTV_DIVX_DRM
  ReturnT nReturn = QTV_RETURN_ERROR;

  nReturn = RegisterInst (handle,priority);
  if (nReturn != QTV_RETURN_OK)
  {
     return nReturn;
  }
  QtvDivXDrmClient* drmclient = new QtvDivXDrmClient();
  if(!drmclient)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"drmclient constructor failed..");
    return nReturn;
  }
  if(drmclient->GetRegistrationCode(rcode,max_len))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"drmclient->GetRegistrationCode is SUCCESSFUL");
    nReturn = QTV_RETURN_OK;
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"drmclient->GetRegistrationCode FAILED..");
  }
  delete(drmclient);
  return nReturn;
#else
  return QTV_RETURN_ERROR;
#endif
}
/* ======================================================================
FUNCTION
  QtvPlayer::OpenIxStream

DESCRIPTION
  Determine content type and routes to Mpeg4Player/RealPlayer

DEPENDENCIES
  None

RETURN VALUE
  QTV_RETURN_ERROR if mime type can't be identified else returns QTV_RETURN_OK

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
#ifdef FEATURE_QTV_DRM_DCF
QtvPlayer::ReturnT QtvPlayer::OpenIxStream(dcf_ixstream_type pstream,
                                           uint32 dwTrackSelected,
                                           InstanceHandleT handle,
                                           InstancePriorityT priority)
#else
QtvPlayer::ReturnT QtvPlayer::OpenIxStream(dcf_ixstream_type,uint32, InstanceHandleT,
                                           InstancePriorityT)
#endif
{
#ifdef FEATURE_QTV_DRM_DCF

  ReturnT nReturn = QTV_RETURN_ERROR;

  nReturn = RegisterInst (handle,priority);
  if (nReturn != QTV_RETURN_OK)
  {
     return nReturn;
  }
  QtvPlayer *player = GetPlayer(handle);
  nReturn = QTV_RETURN_ERROR;

  /*
  * Initialize CurrentMIMEType to QTV_PLAYER_MIME_TYPE.
  * QTV_IXSTREAM_OPEN_EVENT_handler will read in first few bytes
  * and will update CurrentMIMEType accordingly.
  */
  player->CurrentMIMEType = QTV_PLAYER_MIME_TYPE;

  /*
  * When OEM pass IxStream, reference IxStreams will be created
  * and dwTrackSelected will decide whether to play,audio and text.
  *
  * dwTrackSelected: specifies which combination of tracks to play (audio/video/text)
  * Following is the mask to determine which tracks to play
  *
  *    #define QTV_NO_TRACK_SELECTED             0x00
  *    #define QTV_VIDEO_TRACK_SELECTED          0x01
  *    #define QTV_AUDIO_TRACK_SELECTED          0x02
  *    #define QTV_TEXT_TRACK_SELECTED           0x04
  */


  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "QtvPlayer::OpenIxStream:dwTrackSelected %d",dwTrackSelected);

  //If no track has been selcted or pstream is NULL, return error.
  if( (dwTrackSelected == 0)||(!pstream) )
  {
   return nReturn;
  }
  if(player->pDispatchThreadHandle || player->pDispatchInstance)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "Error:QtvPlayer::OpenIxStream:found non NULL pDispatchThreadHandle|pDispatchInstance");
    return nReturn;
  }
  player->pDispatchThreadHandle = DispatchThread::GetSingletonInstance();
  player->pDispatchInstance = QTV_New_Args(QtvPlayerDispatch , (player));
  if(player->pDispatchInstance && player->pDispatchThreadHandle)
  {
    player->pDispatchInstance->SetMpeg4PlayerHandle(player->pMpeg4Player);
    if(player->pDispatchThreadHandle->StartQTVDispatchThread())
    {
      QTV_IXSTREAM_OPEN_EVENT_type *pEvent = QCCreateMessage(QTV_IXSTREAM_OPEN_EVENT, player->pDispatchThreadHandle);
      if (pEvent)
      {
        pEvent->pStream = pstream;
        pEvent->dwTrackSelected = dwTrackSelected;
        pEvent->DispatchHandle = player->pDispatchInstance;
        player->pDispatchThreadHandle->QTVDispatchThreadPostMessage(pEvent);
        nReturn = QtvPlayer::QTV_RETURN_OK;
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "QtvPlayer::OpenIxStream:posted Msg to DispatchThread");
      }
    }//if(pDispatchThreadHandle->StartQTVDispatchThread())
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "QtvPlayer::OpenIxStream:nReturn %d",nReturn);
  }//if(pDispatchInstance && pDispatchThreadHandle)
  else
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "OpenIxStream:QTV_New failed..");
    if( player->pDispatchThreadHandle)
    {
      DispatchThread::DestroyInst();
      player->pDispatchThreadHandle = NULL;
    }
    if(player->pDispatchInstance)
    {
      QTV_Delete(player->pDispatchInstance);
      player->pDispatchInstance = NULL;
    }
  }
  LogQtvCmd("OpenIxStream", "%d%d%d%d", (int32)nReturn, dwTrackSelected, (int32)handle, (int32)priority);
  return nReturn;
#else
  return QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif
}
/* ======================================================================
FUNCTION
  SAMPLE_FUNC

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
/* END sample_func */

////////////////////////////////////////////////////////////////////
//
//  Public functions invoked by API thread.
//
////////////////////////////////////////////////////////////////////

/*===========================================================================
  METHOD     QtvPlayer::MapPlayerStatusCodetoString()

  DESCRIPTION
   Method that returns an appropriate explanatory string for the code

  DEPENDENCIES
   None

  RETURN VALUE
   Returns a pointer to a constant string

  SIDE EFFECTS
   None.
===========================================================================*/
const char* QtvPlayer::MapPlayerStatusCodetoString(PlayerStatusT code)
{
  const char *string_ptr;

  if((code > QTV_PLAYER_STATUS_LAST))
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Illegal code to MapPlayerStatusCodetoString()", code);
    string_ptr = "";
  }
  else
  {
    /*-------------------------------------------------------------------------
      we have an array of strings in the class definition and just refer to
      that from here and return the appropriate explanatory string.
    -------------------------------------------------------------------------*/
    string_ptr = PlayerStatusString[(int)code]; /*lint!e661 */
  }

  return(string_ptr);

}/* QtvPlayer::MapPlayerStatusCodetoString() */

/*===========================================================================
  METHOD     QtvPlayer::IsPlayerStatusErrorCode()

  DESCRIPTION
   Method that returns True/False based on whether the PlayerStatusT
   code is classified as an "error" code. This is a flexible design that
   allows any code from the enum to be classified as an "error" code
   without compicating the logic of this method. All that is required is
   another case statement. Based on this a MSG_ERR() or other may be used.

  DEPENDENCIES
   None

  RETURN VALUE
   Returns True/False

  SIDE EFFECTS
   None.
===========================================================================*/
boolean QtvPlayer::IsPlayerStatusErrorCode(PlayerStatusT code)
{
  boolean ret = false;
  switch (code)
  {
    /*--------------------------------------------------
      Playback error notifications.
    --------------------------------------------------*/
    case QTV_PLAYER_STATUS_AUDIO_ERROR:
    case QTV_PLAYER_STATUS_VIDEO_ERROR:
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    case QTV_PLAYER_STATUS_TEXT_ERROR:
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
    case QTV_PLAYER_STATUS_STREAM_ERROR:
    case QTV_STREAM_UNSUPPORTED_MEDIA_TYPE:
    case QTV_PLAYER_STATUS_AUDIO_ERROR_FORMAT_NOT_SUPPORTED:
    case QTV_PLAYER_STATUS_VIDEO_ERROR_FORMAT_NOT_SUPPORTED:
    case QTV_PLAYER_STATUS_VIDEO_ONLY_PLAYBACK_NOT_ALLOWED:
    case QTV_PLAYER_STATUS_AUDIO_ONLY_PLAYBACK_NOT_ALLOWED:
    case QTV_PLAYER_STATUS_PLAY_TIME_ERROR:
    case QTV_PLAYER_STATUS_NO_SYNC_FRAME_ERROR:
    case QTV_PLAYER_STATUS_PREV_FRAME_FAILED:
    case QTV_PLAYER_STATUS_NEXT_FRAME_FAILED:

#ifdef FEATURE_QTV_STREAM_RECORD
    case QTV_REC_FILE_SYS_LIMIT_APPROACHING:
    case QTV_REC_FILE_SYS_FULL:
    case QTV_RECORDER_FILE_ALREADY_EXISTS:
    case QTV_RECORDER_FILE_WRITE_ERROR:
    case QTV_RECORDER_FAILED:
#endif  /* FEATURE_QTV_STREAM_RECORD */

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
    case QTV_PLAYER_STATUS_VIDEO_RELEASE_FAILED:
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
    case QTV_BUFFER_TO_FILE_COPY_FAILED:
    case QTV_PLAYER_STATUS_DOWNLOAD_ERROR_ABORT:
    case QTV_PLAYER_STATUS_DOWNLOAD_ABORT_MEM_FAIL:
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */

    case QTV_PLAYER_STATUS_SUSPEND_FAILED:
    case QTV_PLAYER_STATUS_PLAY_NOT_SUPPORTED:
#ifdef FEATURE_QTV_GENERIC_BCAST
    case QTV_PLAYER_COMMAND_SELECT_PB_TRACK_FAILED:
    case QTV_PLAYER_COMMAND_PLAY_SYNC_FRAME_FAILED:
    case QTV_PLAYER_COMMAND_SEEK_SYNC_FAILED:
    case QTV_PLAYER_COMMAND_REPOSITION_FAILED:
    case QTV_PLAYER_COMMAND_MUTE_FAILED:
    case QTV_PLAYER_COMMAND_MUTE_ALL_FAILED:
    case QTV_PLAYER_COMMAND_UNMUTE_FAILED:
#endif
    case QTV_PLAYER_STATUS_SERVER_NOT_ENOUGH_BW:
    case QTV_PLAYER_STATUS_DATA_INACTIVITY_ERROR:
    case QTV_STREAM_TRACKLIST_UNKNOWN_CODEC:

      ret = true;
      break;

    default: ret = false;

  }/* Switch() */
  return ret;
}/*   QtvPlayer::IsPlayerStatusErrorCode() */

/*===========================================================================
  METHOD     QtvPlayer::IsPlayerStatusTrivialCode()

  DESCRIPTION
   Method that returns True/False based on whether the PlayerStatusT
   code is classified as an "trivial" code. This is a flexible design that
   allows any code from the enum to be classified as an "trivial" code
   without compicating the logic of this method. All that is required is
   another case statement. Based on this user may decide to not issue a
   MSG().

  DEPENDENCIES
   None

  RETURN VALUE
   Returns True/False

  SIDE EFFECTS
   None.
===========================================================================*/
boolean QtvPlayer::IsPlayerStatusTrivialCode(PlayerStatusT code)
{
  boolean ret = false;
  switch (code)
  {
    /*--------------------------------------------------
      Playback error notifications.
    --------------------------------------------------*/
    case QTV_COMMAND_OPEN_URN_COMPLETE :
    case QTV_COMMAND_REGISTER_FOR_CALLBACK_COMPLETE:
    case QTV_PLAYER_STATUS_DECODE_FRAME:
#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
    case QTV_PLAYER_STATUS_ROTATION_DONE:
    case QTV_PLAYER_STATUS_SCALING_DONE:
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */

      ret = true;
      break;

    default: ret = false;

  }/* Switch() */
  return ret;
}/*   QtvPlayer::IsPlayerStatusTrivialCode() */


/*===========================================================================
  METHOD     QtvPlayer::SetRTSPStatusCallback()

  DESCRIPTION
    Method that registers the callback function to be notified of any RTSP
    server response other than "Ok", along with a corresponding text string.

  DEPENDENCIES
   None

  RETURN VALUE
   Returns True/False

  SIDE EFFECTS
    Sets QtvPlayer::RTSPStatusT_CB
===========================================================================*/
void QtvPlayer::SetRTSPStatusCallback(RTSPStatusCallbackT RTSPStatusCB)
{
  GetPlayer(NULL)->RTSPStatusT_CB = RTSPStatusCB;
}/* QtvPlayer::SetRTSPStatusCallback() */

void QtvPlayer::SetRTSPStatusCallback(RTSPStatusInstanceCallbackT RTSPStatusCB, InstanceHandleT handle, void *pUserData)
{
  GetPlayer(handle)->RTSPStatusInstanceT_CB = RTSPStatusCB;
  GetPlayer(handle)->RTSPStatusT_CB_UserData = pUserData;
}/* QtvPlayer::SetRTSPStatusCallback() */

/*===========================================================================
  METHOD     QtvPlayer::GetRTSPStatusCallback()

  DESCRIPTION
    Returns the value of the currently defined callback function to invoke
   when the RTSP engine receives a server response of other than "Ok".

  DEPENDENCIES
   None

  RETURN VALUE
   Function pointer, or 0 if none has been set via SetRTSPStatusCallback()

  SIDE EFFECTS

===========================================================================*/
QtvPlayer::RTSPStatusCallbackT QtvPlayer::GetRTSPStatusCallback()
{
  QtvPlayer::RTSPStatusCallbackT retVal = GetPlayer(NULL)->RTSPStatusT_CB;
  return retVal;
}/* QtvPlayer::GetRTSPStatusCallback() */

QtvPlayer::RTSPStatusInstanceCallbackT QtvPlayer::GetRTSPStatusCallback(InstanceHandleT handle)
{
  QtvPlayer::RTSPStatusInstanceCallbackT retVal = GetPlayer(handle)->RTSPStatusInstanceT_CB;
  return retVal;
}/* QtvPlayer::GetRTSPStatusCallback() */

void* QtvPlayer::GetRTSPStatusUserData(InstanceHandleT handle)
{
  return GetPlayer(handle)->RTSPStatusT_CB_UserData;
}/* QtvPlayer::GetRTSPStatusCallback() */

/*===========================================================================
  METHOD     QtvPlayer::SetQTVErrorCallback()

  DESCRIPTION
    Method that registers the callback for the PlayerStatusT codes and
    their corresponding explanatory strings.

  DEPENDENCIES
   None

  RETURN VALUE
   Returns True/False

  SIDE EFFECTS
    Sets the value of the
    static  QtvPlayer::PlayerStatusCallbackT PlayerStatusT_CB
===========================================================================*/
boolean QtvPlayer::SetQTVErrorCallback(PlayerStatusCallbackT PlayerStatusCB)
{
  GetPlayer(NULL)->PlayerStatusT_CB = PlayerStatusCB;
  LogQtvCmd("SetQTVErrorCallback", "%d%d", (int32)true,(int32)PlayerStatusCB);
  return(true);
}/* QtvPlayer::SetQTVErrorCallback() */

boolean QtvPlayer::SetQTVErrorCallback(PlayerStatusInstanceCallbackT PlayerStatusCB, InstanceHandleT handle, void *pUserData)
{
  GetPlayer(handle)->PlayerStatusInstanceT_CB = PlayerStatusCB;
  GetPlayer(handle)->PlayerStatusT_CB_UserData = pUserData;
  return(true);
}/* QtvPlayer::SetQTVErrorCallback() */

/*===========================================================================
  METHOD     QtvPlayer::GetQTVErrorCallback()

  DESCRIPTION
    Method that returns the callback of type PlayerStatusCallbackT. This is
    useful checking whether is defined (not NULL) and so it should be called

  DEPENDENCIES
   None

  RETURN VALUE
   Returns True/False

  SIDE EFFECTS
    Returns current value of QtvPlayer::PlayerStatusT_CB
===========================================================================*/
QtvPlayer::PlayerStatusCallbackT QtvPlayer::GetQTVErrorCallback()
{
  QtvPlayer::PlayerStatusCallbackT retVal = GetPlayer(NULL)->PlayerStatusT_CB;
  return retVal;
}/* QtvPlayer::GetQTVErrorCallback() */

QtvPlayer::PlayerStatusInstanceCallbackT QtvPlayer::GetQTVErrorCallback(InstanceHandleT handle)
{
  QtvPlayer::PlayerStatusInstanceCallbackT retVal = (GetPlayer(handle)->PlayerStatusInstanceT_CB);
  return retVal;
}/* QtvPlayer::GetQTVErrorCallback() */

/* ======================================================================
FUNCTION
  QtvPlayer::Init

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::Init
(
  MallocOutputBufferT MallocOutputBuffer,
  FreeOutputBufferT   FreeOutputBuffer,
  InstanceHandleT *pHandle
)
{
    ReturnT nReturn = QTV_RETURN_ERROR;
    QtvPlayer *player = NULL;
    if (pHandle == NULL)
    {
      // Init called with NULL pHandle
      if(!QtvInstances[QTV_PRIORITY_DEFAULT])
      {
        // A default instance of QtvPlayer does not already exist
        // Create the default QtvPlayer instance
        player = QTV_New(QtvPlayer);
        if (player != NULL)
        {
          // QtvPlayer creation succeeded, assign priority
          QtvInstances[QTV_PRIORITY_DEFAULT] = (InstanceHandleT)player;
          player->InstancePriority = QTV_PRIORITY_DEFAULT;
    numQtvPlayers++;
        }
        else
        {
          // QtvPlayer creation failed, return error
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Init error, failed to create QtvPlayer instance");
          return QTV_RETURN_ERROR;
        }
      }
      else
      {
        // A default instance of QtvPlayer already exists
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Init error, QtvPlayer default instance already exists");
        return QTV_RETURN_ERROR;
      }
    }
    else
    {
      // Init called with non NULL pHandle
      // Create an instance of QtvPlayer
      player = QTV_New(QtvPlayer);
      // Output the new QtvPlayer instance handle
      *pHandle = (InstanceHandleT)player;
      if (player == NULL)
      {
        // QtvPlayer creation failed, return error
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Init error, failed to create QtvPlayer instance");
        return QTV_RETURN_ERROR;
      }
      else
      {
        numQtvPlayers++;
      }
    }

     player->MallocBufferFunction = MallocOutputBuffer;
     player->FreeBufferFunction   = FreeOutputBuffer;

     player->descramblerHandler = NULL;

     memset(&player->NetPolicy, 0, sizeof(NetPolicyInfo));
     player->NetPolicy.isSet = false;
     player->MediaInfo = NULL;
     player->MediaInfoSize = 0;
     nReturn =  QTV_RETURN_OK;
     if(pHandle)
     {
        LogQtvCmd("Init", "%d%d%d%d", (int32)nReturn, (int32)MallocOutputBuffer, (int32)FreeOutputBuffer,(int32)*pHandle);
     }
     else
     {
        LogQtvCmd("Init", "%d%d%d%d", (int32)nReturn, (int32)MallocOutputBuffer, (int32)FreeOutputBuffer,0);
     }
     return nReturn;
}

/* ======================================================================
FUNCTION
  QtvPlayer::Init

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::Init
(
  MallocOutputBufferT MallocOutputBuffer,
  FreeOutputBufferT   FreeOutputBuffer,
  const char *mediaType)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::Init");

  ReturnT nReturn = QTV_RETURN_ERROR;

  //Note: If there's already a player object, then this call fails.
  //It's not safe to just destroy the old player, or to reuse it,
  //since our memory pointers may be invalid if the parent process
  //has shutdown and restarted itself since the player was created.

  // mediatype must not be null. This mandates that either Mpeg4Player or Real player
  // get initialized.
  ASSERT(mediaType);

  if (pMpeg4Player == NULL && (strcmp(mediaType, QTV_PLAYER_MIME_TYPE) == 0))
  {
    ++guQtvPlayerResourceRefCnt;
    if(1 == guQtvPlayerResourceRefCnt)
    {
    #ifdef FEATURE_MEMUTILS_NEW_DELETE
        // activate heap caching
        HeapCacheAddRef();
    #endif

    #ifdef FEATURE_QTV_IMEM
#error code not present
    #endif
    }

    //Create the object
    pMpeg4Player = QTV_New( Mpeg4Player );

    //Startup and init.
    if (pMpeg4Player)
    {
      if (pMpeg4Player->CreatePlayer(MallocOutputBuffer, FreeOutputBuffer))
      {
        nReturn = QTV_RETURN_OK;

        #ifdef FEATURE_QTV_DRM_DCF
        if(pDispatchInstance)
        {
          pDispatchInstance->SetMpeg4PlayerHandle(pMpeg4Player);
        }
        #endif

      }
      else
      {
        //An error occurred, try to cleanup anyway.
        pMpeg4Player->SetAbortFlag();

        /* Post an event so the Player thread unblocks and sees the abort */
        PV_STOP_type *pEvent = QCCreateMessage(PV_STOP, pMpeg4Player); /*lint !e641 */

        if (pEvent)
        {
          pMpeg4Player->PostMessage(pEvent);
        }
        else
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "No memory to post event");
        }

        (void)pMpeg4Player->Destroy(); //ignore return code.
        QTV_Delete( pMpeg4Player );
        pMpeg4Player = NULL;

        if(!--guQtvPlayerResourceRefCnt)
        {
        #ifdef FEATURE_MEMUTILS_NEW_DELETE
          // disable the heap cache
          HeapCacheRelease();
        #endif

        #ifdef FEATURE_QTV_IMEM
#error code not present
        #endif
        }

        /*  Comes here when it fails to create the Mpeg4Player no matter
        * the Stop event is posted or not. No point to continue.
        * There is no point in continuing when there is no memory to post
        * an event to the player to unblock it.  The UI thread will block
        * forever trying to take PlayerCS critical section held by the player
        * thread.  Just return QTV_RETURN_ERROR so that UI remains functional.
        */
        return nReturn;
      }
    }
    else  // pMpeg4Player == NULL
    {
      if(!--guQtvPlayerResourceRefCnt)
      {

      #ifdef FEATURE_MEMUTILS_NEW_DELETE
        // disable the heap cache
        HeapCacheRelease();
      #endif

      #ifdef FEATURE_QTV_IMEM
#error code not present
      #endif
      }
    }
  }

  if (NetPolicy.isSet)
  {
     SetNetPolicyInfo(&NetPolicy, (InstanceHandleT)this);
  }
  else
  {
     SetNetPolicyInfo(NULL, (InstanceHandleT)this);
  }
  if (MediaInfo && MediaInfoSize)
  {
     SetMediaInfo(MediaInfo, MediaInfoSize, (InstanceHandleT)this);
  }

#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
  QtvRealPlayer_SetAllocator(MallocOutputBuffer, FreeOutputBuffer);
#endif /* FEATURE_REAL_PLAYER_USE_QTV_API */

  RegisterForCallbackInternal(CallbackFunction,
                              CallbackInstanceFunction,
                              CallbackClientData,
                              CallbackUserData);

  if (m_UserAgentStr[0] != '\0')
  {
    SetUserAgent(m_UserAgentStr, (InstanceHandleT)this);
  }
  if (m_UserAgentParam_params[0] != '\0')
  {
    SetUserAgentParameters(m_UserAgentParam_params, (InstanceHandleT)this);
  }
  else if (m_UserAgentParam_man[0] != '\0' || m_UserAgentParam_ctn[0] != '\0')
  {
    SetUserAgentParameters(m_UserAgentParam_man, m_UserAgentParam_ctn, (InstanceHandleT)this);
  }

  //set descrambler proxy
  SetDescramblerHandler(descramblerHandler, (InstanceHandleT)this);

#ifndef FEATURE_QTV_DISABLE_CONC_MGR
  /* The highest priority player can not be interrupted. So initialize
   * concurrency manager only if this is not the highest priority player.
   */
  if (m_registered &&
      InstancePriority == QTV_PRIORITY_DEFAULT &&
      (strcmp(mediaType, QTV_PLAYER_MIME_TYPE) == 0) ||
      (strcmp(mediaType, REAL_MEDIA_MIME_TYPE) == 0))
  {
    qtv_conc_mgr::init(pMpeg4Player,
                       mediaType,
                       numRegisteredQtvPlayers);
  }
#endif /* FEATURE_QTV_DISABLE_CONC_MGR */
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, ">>QtvPlayer::Init return %d",nReturn);
  return nReturn;
}


/* ======================================================================
FUNCTION
  QtvPlayer::InitRollback (non API Method)

DESCRIPTION
  This routine is used to cancel the effect of Init() performed within the
  Open like routines earlier while bailing out.

DEPENDENCIES
  Very specific to Open like routines such as Open Pseudo stream or push/pull
  buffered stream.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

========================================================================== */
void  QtvPlayer::InitRollback()
{
  if(!pMpeg4Player)
  {
    // bail out after giving on the spot fine
    QTV_MSG_PRIO(QTVDIAG_GENERAL,QTVDIAG_PRIO_ERROR,
    "InitRollback is called in wrong Context!! ");
    return;
  }
  //An error occurred, try to cleanup anyway.
  pMpeg4Player->SetAbortFlag();

  /* Post an event so the Player thread unblocks and sees the abort */
  PV_STOP_type *pEvent = QCCreateMessage(PV_STOP, pMpeg4Player); /*lint !e641 */

  if (pEvent)
  {
    pMpeg4Player->PostMessage(pEvent);
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                 "InitRollback No memory to post event");
  }

  (void)pMpeg4Player->Destroy(); //ignore return code.
  QTV_Delete( pMpeg4Player );
  pMpeg4Player = NULL;

  if(guQtvPlayerResourceRefCnt > 0)
  {
    if(!--guQtvPlayerResourceRefCnt)
    {
      #ifdef FEATURE_MEMUTILS_NEW_DELETE
      // disable the heap cache
      HeapCacheRelease();
      #endif

      #ifdef FEATURE_QTV_IMEM
#error code not present
      #endif
    }
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
    "InitRollback called with invalid Resource Reference Count");
  }

  return;
}

void QtvPlayer::register_qtv_with_callmgr()
{
#ifndef FEATURE_QTV_DISABLE_CONC_MGR
  /* The highest priority player can not be interrupted. So initialize
   * concurrency manager only if this is not the highest priority player.
   */
  if (m_registered &&
      InstancePriority == QTV_PRIORITY_DEFAULT &&
      (strcmp(CurrentMIMEType, QTV_PLAYER_MIME_TYPE) == 0) ||
      (strcmp(CurrentMIMEType, REAL_MEDIA_MIME_TYPE) == 0))
  {
    if (!qtv_conc_mgr::register_with_call_mgr())
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                   "Failed to Init qtv_conc_mgr");
      qtv_conc_mgr::terminate(numRegisteredQtvPlayers);
    }
  }
#endif /* FEATURE_QTV_DISABLE_CONC_MGR */
}
/* ======================================================================
FUNCTION
  QtvPlayer::Terminate

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::Terminate(InstanceHandleT handle, bool destroyInst)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, ">>QtvPlayer::Terminate");

  ReturnT nReturn = QTV_RETURN_ERROR;

  /* Checks for valid instances available or not */
  if(numQtvPlayers < 1)
  {
      /* Assign the respective return value to nReturn */
      nReturn = QTV_RETURN_NO_INSTANCE_AVAILABLE;
      /*If there is no Qtv instances available then we have to exit gracefully, to avoid fatal errors/crashes.*/
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,">>QtvPlayer::QtvPlayer::Terminate() -> Qtv Instance Not Available : %d", nReturn);
      /* return error */
      return nReturn;
  }

  QtvPlayer *player = GetPlayer(handle);
  bool bMpeg4Player = false;
  //Note: If there's no player object, this call does nothing
  //and returns OK.

#ifndef FEATURE_QTV_DISABLE_CONC_MGR
  /* The highest priority player does not register with concurrency manager.
   * so terminate only of this is not the highest priority player.
   */
   if ( (player->InstancePriority == QTV_PRIORITY_DEFAULT) &&
        player->m_registered )
  {
      qtv_conc_mgr::terminate(numRegisteredQtvPlayers);
  }
#endif /* FEATURE_QTV_DISABLE_CONC_MGR */

#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
  if (player->CurrentMIMEType && strcmp(player->CurrentMIMEType, REAL_MEDIA_MIME_TYPE) == 0)
  {
    QtvRealPlayer_Terminate();
  }
#endif /* FEATURE_REAL_PLAYER_USE_QTV_API */
  if (player->MediaInfo)
  {
     QTV_Free(player->MediaInfo);
     player->MediaInfoSize = 0;
  }

#ifdef FEATURE_QTV_DRM_DCF
   if(player->pDispatchThreadHandle)
   {
     DispatchThread::DestroyInst();
     player->pDispatchThreadHandle = NULL;
   }
   if(player->pDispatchInstance)
   {
     QTV_Delete(player->pDispatchInstance);
     player->pDispatchInstance = NULL;
   }
#endif

  if (!player->pMpeg4Player)
  {
    nReturn = QTV_RETURN_OK;
  }
  else
  {
    unsigned int totalSleepDuration = 0;
    int dog_autokick;

    bMpeg4Player = true;
    player->pMpeg4Player->SetAbortFlag();
    player->pMpeg4Player->SetMediaAbortFlag(true);

    /* Post an event so the Player thread unblocks and sees the abort */
    PV_STOP_type *pEvent = QCCreateMessage(PV_STOP, player->pMpeg4Player);/*lint !e641 */

    if (pEvent)
    {
      player->pMpeg4Player->PostMessage(pEvent);
#ifndef FEATURE_WINCE
      dog_autokick = rex_autodog_enable(rex_self()->dog_report_val);
#endif // #ifndef FEATURE_WINCE
      while (!player->pMpeg4Player->IsPlayerThreadIdle() &&
             (totalSleepDuration < PLAYER_TRANSITION_TO_IDLE_TIMEOUT))
      {
        // To begin with we start checking at 20msec interval which
        // is the start value. Once the duration reached 200msec
        // we will check less frequently at 200msec. In this way
        // checking frequency is increased only by five instead of
        // 100 times if we would have checked at 20msec interval for
        // the entire 25 second period.
        if(totalSleepDuration < PLAYER_STATE_POLL_INTERVAL)
        {
          rex_sleep(PLAYER_STATE_POLL_START_INTERVAL);
          totalSleepDuration += PLAYER_STATE_POLL_START_INTERVAL;
        }
        else
        {
          rex_sleep(PLAYER_STATE_POLL_INTERVAL);
          totalSleepDuration += PLAYER_STATE_POLL_INTERVAL;
        }
      }
      //Restoring to previous value of auto_dog_enable
      if (!dog_autokick)
      {
        (void)rex_autodog_disable();
      }

      if (player->pMpeg4Player->IsPlayerIdle())
      {
        //Shutdown and cleanup
        if (player->pMpeg4Player->Destroy())
        {
          //Delete the object & clear the pointer.
          QTV_Delete( player->pMpeg4Player );
          player->pMpeg4Player=NULL;
          nReturn = QTV_RETURN_OK;
        }
      }
    }
  }

#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
   nReturn = QtvRAMPlayer_Terminate(player);
  OEMMediaReal_PlayingRmContent(TRUE);
#endif

   /* Delete the Qtv instance only if we were able to terminate the players */
   if ( nReturn == QTV_RETURN_OK )
    {
  if(destroyInst)
  {
    if(player->m_registered)
    {
      //If this instance is registered then numRegisteredQtvPlayers must be +ive
      if(numRegisteredQtvPlayers)
      {
      --numRegisteredQtvPlayers;
    }
      else
      {
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL,
                            "QtvPlayer::Terminate (numRegisteredQtvPlayers==0)..!");
      }
    }
    QtvInstances[player->InstancePriority] = NULL;
    --numQtvPlayers;
    QTV_Delete(player);

    if(bMpeg4Player && (!--guQtvPlayerResourceRefCnt))
    {
    #ifdef FEATURE_MEMUTILS_NEW_DELETE
        // disable the heap cache
        HeapCacheRelease();
    #endif

    #ifdef FEATURE_QTV_IMEM
#error code not present
    #endif
    }
  }
   }
#ifdef FEATURE_QTV_DEBUG_ONLY
   else
   {
      /* Player were not terminated properly, assert */
      ASSERT(0);
   }
#endif /* FEATURE_QTV_DEBUG_ONLY */

#ifdef FEATURE_QTV_MEMORY_LEAK_DEBUG
#error code not present
#endif

   LogQtvCmd("Terminate","%d%d%d",
             (int32)nReturn, (int32)handle, (int32)destroyInst);
   QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                 ">>QtvPlayer::Terminate return %d",nReturn);
  return nReturn;
}

/* ======================================================================
FUNCTION
  QtvPlayer::OpenURN

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

NOTE: This function is kind of DEPRECATED and exists only to MAINTAIN BACKWARD COMPATIBILITTY.
      User should use QtvPlayer::OpenURN, which takes audio/video/text file names(see below).

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::OpenURN(const char *videoURN, const char *audioURN)
{
  ReturnT nReturn = QTV_RETURN_ERROR;
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,">>QtvPlayer::OpenURN");

  if( (videoURN == NULL) && (audioURN == NULL) )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Error:QtvPlayer::OpenURN:NULL Audio & Video URN NULL");
    return nReturn;
  }
  nReturn = QtvPlayer::OpenURN(videoURN,audioURN,videoURN ? videoURN : audioURN);
  return nReturn;
}
/* ======================================================================
FUNCTION
  QtvPlayer::OpenURN

DESCRIPTION
  Function used to play video, audio and text from the specified file names resp.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::OpenURN(const char *videoURN, const char *audioURN, const char *textURN,
                                      InstanceHandleT handle,
                                      InstancePriorityT priority,
                                      OpenURNTypeT opentype)
{

  ReturnT nReturn = QTV_RETURN_ERROR;

  QtvPlayer *player = GetPlayer(handle);

  //At least one URN should be valid/Non NULL
  if(!(videoURN || audioURN || textURN))
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,">>QtvPlayer::OpenURN(v,a,t) return %d",nReturn);
    return nReturn;
  }

  const char* URN = videoURN ? videoURN : audioURN;
  URN = URN ? URN : textURN;
  player->CurrentMIMEType = QtvPlayer::GetMediaType(URN);

  if(!player->CurrentMIMEType)
  {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "Error:OpenURN GetMediaType returned NULL");
     QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,">>QtvPlayer::OpenURN(v,a,t) returning nReturn %d",nReturn);
     return QTV_RETURN_UNSUPPORTED;
  }

  nReturn = RegisterInst (handle,priority);
  if (nReturn != QTV_RETURN_OK)
  {
     return nReturn;
  }

  #ifdef FEATURE_QTV_DRM_DCF
    /*
    * Make sure URN is for local file( not .url, .sdp., .pvx, .flo, .ram)      AND
    *  QTV is configured to run unprotected content through IxStream OR
    *  It's a DCF media (.dcf extension)
    */
     if( (QtvPlayerDispatch::isLocalFileMedia(URN))                                        &&
         ( (player->g_always_through_ixstream == true) || (QtvPlayerDispatch::IsDrmDcfMedia(URN)) ) )
    {
      if(player->pDispatchThreadHandle || player->pDispatchInstance)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "Error:OpenURN found non NULL pDispatchThreadHandle||pDispatchInstance");
        return nReturn;
      }
      player->pDispatchThreadHandle = DispatchThread::GetSingletonInstance();
      player->pDispatchInstance = QTV_New_Args(QtvPlayerDispatch , (player));
      if( player->pDispatchThreadHandle && player->pDispatchInstance )
      {
        player->pDispatchInstance->SetMpeg4PlayerHandle(player->pMpeg4Player);
        if( player->pDispatchThreadHandle->StartQTVDispatchThread() )
        {
          PV_OPEN_CONTENT_URN_type *pEvent = QCCreateMessage(PV_OPEN_CONTENT_URN, player->pDispatchThreadHandle);
          if (pEvent)
          {
            pEvent->videoURN = videoURN;
            pEvent->audioURN = audioURN;
            pEvent->textURN  = textURN;
            pEvent->DispatchHandle = (void*)player->pDispatchInstance;
            player->pDispatchThreadHandle->QTVDispatchThreadPostMessage(pEvent);
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,">>QtvPlayer::OpenURN,posted message to QTVDispatchThread");
            nReturn = QTV_RETURN_OK;
          }
        }//if( pDispatchThreadHandle->StartQTVDispatchThread() )
      }//if( pDispatchThreadHandle && pDispatchInstance )
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,">>QtvPlayer::OpenURN:QTV_New failed..");
        if(player->pDispatchThreadHandle)
        {
          DispatchThread::DestroyInst();
          player->pDispatchThreadHandle = NULL;
        }
        if(player->pDispatchInstance)
        {
          QTV_Delete(player->pDispatchInstance);
          player->pDispatchInstance = NULL;
        }
      }
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,">>QtvPlayer::OpenURN(v,a,t) returning nReturn %d",nReturn);
      return nReturn;
    }
  #endif

  /*
   * When we reach here, it means this is either
   * Real media or QTV is not configured
   * to run unprotected through IxStream or some URL or DRM_DCF is not defined.
   */
#ifdef FEATURE_REAL_PLAYER_USE_QTV_API

    // The Qtv RAM Player must be initialized before initializing the Qtv Player
    // Otherwise the set callback function for the RAM Player in QtvPlayer::Init will fail
    QtvRAMPlayer_Init(player);
    if(player->RAMPlayer ==NULL)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,">>QtvPlayer::OpenURN:QtvRAMPlayer_Init failed..");
      return QTV_RETURN_ERROR;
    }
#endif /* FEATURE_REAL_PLAYER_USE_QTV_API */

    player->Init(player->MallocBufferFunction,
                 player->FreeBufferFunction,
                 player->CurrentMIMEType);

#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
    if (strcmp(player->CurrentMIMEType, REAL_MEDIA_MIME_TYPE) == 0)
    {
      //Route to real player
      if (audioURN == NULL || audioURN[0] == '\0')
      {
       OEMMediaReal_DisableAudio(TRUE);
      }
      else
      {
       OEMMediaReal_DisableAudio(FALSE);
      }
      return QtvRealPlayer_OpenURN(URN);
    }
    else if (strcmp(player->CurrentMIMEType, RAM_FILE_MIME_TYPE) == 0)
    {
      //Route to RAM player
      OEMMediaReal_PlayingRmContent(FALSE);
      return QtvRAMPlayer_OpenURN(URN, player, priority);
    }
#endif /* FEATURE_REAL_PLAYER_USE_QTV_API */

    if (player->MediaPlayerRootPath[0] != '\0')
    {
      int pathExt=(int)player->MediaPlayerRootPath;
      SetQTVConfigItem(QtvConfig::QTVCONFIG_MEDIAPLAYER_ROOT_PATH,
                       &pathExt, handle);
    }

   /* at least one of audio or video URN should be true */
   if ( player->pMpeg4Player && (videoURN || audioURN || textURN) )
   {
     if ( player->pMpeg4Player->StartThread() )
     {
#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */

       PV_OPEN_URN_type *pEvent = QCCreateMessage(PV_OPEN_URN, player->pMpeg4Player);/*lint !e641 */

       if ( pEvent )
       {
         //memory will be release in after received these events mpeg4player*/
         pEvent->videoURN = QTV_New(URL);
         if ( (NULL != pEvent->videoURN) && (NULL != videoURN) )
         {
           pEvent->videoURN->SetUrl(videoURN);
         }

         pEvent->audioURN = QTV_New(URL);
         if ( (NULL != pEvent->audioURN) && (NULL != audioURN) )
         {
           pEvent->audioURN->SetUrl(audioURN);
         }

         pEvent->textURN = QTV_New(URL);
         if ( (NULL != pEvent->textURN) && (NULL != textURN) )
         {
           pEvent->textURN->SetUrl(textURN);
         }
         pEvent->openType = opentype;
         //Reset the abort flag before issueing playback.
         player->pMpeg4Player->SetMediaAbortFlag(false);

         player->pMpeg4Player->PostMessage(pEvent);
         nReturn = QTV_RETURN_OK;
       }
     }
   }
#ifndef FEATURE_QTV_DISABLE_CONC_MGR
   /* Registering qtv with call manager */
   player->register_qtv_with_callmgr();
#endif

   if(videoURN && audioURN && textURN)
   {
  LogQtvCmd("OpenURN", "%d%s%s%s%d%d", (int32)nReturn, videoURN, audioURN, textURN,(int32)handle, (int32)priority);
   }
   else
   {
      QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Cannot log Qtv cmd %u,%u,%u", videoURN, audioURN, textURN);
   }
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,">>QtvPlayer::OpenURN(v,a,t) return %d",nReturn);
  return nReturn;
}

/* ======================================================================
FUNCTION
  QtvPlayer::OpenURN

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::OpenURN(unsigned char *VideoBuf, uint32 videoSize,
                                      unsigned char *AudioBuf, uint32 audioSize)
{
    ReturnT nReturn = QTV_RETURN_ERROR;
    /* at least one of audio or video URN should be true */
    if(VideoBuf && videoSize)
    {
        //play text from VideoBuf
        nReturn = OpenURN(VideoBuf, videoSize, AudioBuf, audioSize, VideoBuf, videoSize);
    }
    else if(AudioBuf && audioSize)
    {
        //play text from AudioBuf
        nReturn = OpenURN(VideoBuf, videoSize, AudioBuf, audioSize, AudioBuf, audioSize);
    }
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,">>QtvPlayer::OpenURN(vBuf,aBuf) return %d",nReturn);
    return nReturn;
}

/* ======================================================================
FUNCTION
  QtvPlayer::OpenURN

DESCRIPTION
  Function used to play video, audio and text from the specified local buffers resp.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::OpenURN(unsigned char *VideoBuf, uint32 videoSize,
                                      unsigned char *AudioBuf, uint32 audioSize,
                                      unsigned char *TextBuf, uint32 textSize,
                                      InstanceHandleT handle, InstancePriorityT priority)
{
    ReturnT nReturn = QTV_RETURN_ERROR;
    /* at least one of audio or video URN should be true */
    nReturn = RegisterInst (handle,priority);
    if (nReturn != QTV_RETURN_OK)
    {
       return nReturn;
    }
    QtvPlayer *player = GetPlayer(handle);

    unsigned char *buf = NULL;
    unsigned long  len = 0;
    if(VideoBuf)
    {
      buf  =  VideoBuf;
      len =  videoSize;
    }
    else if(AudioBuf)
    {
      buf  = AudioBuf;
      len =  audioSize;
    }
    else
    {
      buf  = TextBuf;
      len =  textSize;
    }
    player->CurrentMIMEType = QTV_PLAYER_MIME_TYPE;
    if (buf)
    {
       player->CurrentMIMEType = QtvPlayer::GetMIMEType(buf, len, len);

       if(!player->CurrentMIMEType)
       {
         QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL,
                      "Error:OpenURN GetMIMEType returned NULL");
         return QTV_RETURN_ERROR;
       }

#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
       // The Qtv RAM Player must be initialized before initializing the Qtv Player
       // Otherwise the set callback function for the RAM Player in QtvPlayer::Init will fail
       QtvRAMPlayer_Init(player);
       if(player->RAMPlayer ==NULL)
       {
         QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,">>QtvPlayer::OpenURN:QtvRAMPlayer_Init failed..");
         return QTV_RETURN_ERROR;
       }
#endif /* FEATURE_REAL_PLAYER_USE_QTV_API */

       player->Init(player->MallocBufferFunction,
                    player->FreeBufferFunction,
                    player->CurrentMIMEType);

#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
       if (player->CurrentMIMEType && strcmp(player->CurrentMIMEType, REAL_MEDIA_MIME_TYPE) == 0)
       {
          return QtvRealPlayer_OpenBuffer(buf, len);
       }
#endif /* FEATURE_REAL_PLAYER_USE_QTV_API */
    }

    if (player->pMpeg4Player && (VideoBuf || AudioBuf || TextBuf))
    {
      if (player->pMpeg4Player->StartThread())
      {
#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */

        PV_OPEN_BUFFER_URN_type *pEvent = QCCreateMessage(PV_OPEN_BUFFER_URN, player->pMpeg4Player);/*lint !e641 */

        if (pEvent)
        {
          pEvent->videoBuffer = VideoBuf;
          pEvent->videoSize = videoSize;
          pEvent->audioBuffer = AudioBuf;
          pEvent->audioSize = audioSize;
          pEvent->textBuffer = TextBuf;
          pEvent->textSize = textSize;

          //Reset the abort flag before issueing playback.
          player->pMpeg4Player->SetMediaAbortFlag(false);

          player->pMpeg4Player->PostMessage(pEvent);
          nReturn = QTV_RETURN_OK;
        }
      }
    }
    LogQtvCmd("OpenURN", "%d%d%d%d%d%d%d%d%d", (int32)nReturn,(int32)VideoBuf, videoSize,(int32)AudioBuf, audioSize,(int32)TextBuf, textSize,(int32)handle, (int32)priority);
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,">>QtvPlayer::OpenURN(vBuf,aBuf,tBuf) return %d",nReturn);
    return nReturn;
}

/* ======================================================================
FUNCTION
  QtvPlayer::SetPriority

DESCRIPTION
  Sets the Qtv Player intance priority.

DEPENDENCIES
  None.

RETURN VALUE
  QTV_RETURN_OK if the priority is available, otherwise
  QTV_RETURN_PRIORITY_IN_USE.

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::SetPriority(InstanceHandleT handle, InstancePriorityT priority)
{
    QtvPlayer::ReturnT result = QTV_RETURN_OK;
    QtvPlayer *player = GetPlayer(handle);

    // Cannot set priority to invalid
    ASSERT(priority != QTV_PRIORITY_INVALID);

    if (QtvInstances[priority] == NULL || QtvInstances[priority] == (InstanceHandleT)player)
    {
       // Clear old priority instance if any
       QtvInstances[priority] = NULL;
       // Set new priority instance
       QtvInstances[priority] = (InstanceHandleT)player;
       // Set new priority
       player->InstancePriority = priority;
    }
    else
    {
       result = QTV_RETURN_PRIORITY_IN_USE;
    }
    return result;
}

/* ======================================================================
FUNCTION
  QtvPlayer::SuspendLowPriorityInstance

DESCRIPTION
  Calls the concorrency manager to suspend all the active insts whose
  priority is lower than the specified instance priority.

DEPENDENCIES
  None.

RETURN VALUE
  QTV_RETURN_OK if all the low priority insts are suspended OR there
  are none running otherwise QTV_RETURN_ERROR.
  QTV_RETURN_ERROR if the specified insts is trying to suspend a high
  priority insts.

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::SuspendLowPriorityInstance(InstanceHandleT handle)
{
  QtvPlayer::ReturnT result = QTV_RETURN_OK;
  QtvPlayer *player = GetPlayer(handle);

  result = CheckInstances(handle);

  if(result != QTV_RETURN_ERROR)
  {
    //Suspend all the low priority instances
    for(uint32 index = (uint32)QTV_PRIORITY_COUNT-1;
               index > (uint32)QTV_PRIORITY_INVALID;
               index--)
    {
      if(QtvInstances[index])
      {
        //priorities increase in ascending order of priority values..
        if ( (uint32)player->InstancePriority <
             (uint32)(((QtvPlayer*)QtvInstances[index])->InstancePriority) )
        {
          //handle special case for real until it supports multiple insts..
          if( (strcmp(player->CurrentMIMEType, REAL_MEDIA_MIME_TYPE) == 0) &&
              (strcmp(((QtvPlayer*)QtvInstances[index])->CurrentMIMEType, REAL_MEDIA_MIME_TYPE) == 0)
              )
          {
            QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "QtvPlayer::SuspendLowPriorityInsts :terminated instPrio=%d"
                          ,((QtvPlayer*)QtvInstances[index])->InstancePriority);
            result = QtvPlayer::Terminate(QtvInstances[index]);
          }
          else
          {
#ifndef FEATURE_QTV_DISABLE_CONC_MGR
            //Call the concurrency manager and suspend this instance
            qtv_conc_mgr::set_suspend_type(qtv_conc_mgr::SUSPENDED_EXTERNALLY,
                                           ((QtvPlayer*)QtvInstances[index])->pMpeg4Player,
                                           ((QtvPlayer*)QtvInstances[index])->CurrentMIMEType);
            qtv_conc_mgr::suspend(((QtvPlayer*)QtvInstances[index])->pMpeg4Player);

            QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "QtvPlayer::SuspendLowPriorityInsts :suspended instPrio=%d"
                          ,((QtvPlayer*)QtvInstances[index])->InstancePriority);
#endif
          }
        }
      }
    }
  }
  return result;
}

/* ======================================================================
FUNCTION
  QtvPlayer::CheckInstances

DESCRIPTION
  Checks if any instance exists whose priority is higher than the current
  instance priority

DEPENDENCIES
  None.

RETURN VALUE
  QTV_RETURN_OK if no other higher priority instance is running
  QTV_RETURN_ERROR if the specified instance is trying to suspend a high
  priority instance.

SIDE EFFECTS
  Detail any side effects.

========================================================================== */

QtvPlayer::ReturnT QtvPlayer::CheckInstances(InstanceHandleT handle)
{

     QtvPlayer::ReturnT result = QTV_RETURN_OK;
     QtvPlayer *player = GetPlayer(handle);

     for(uint32 index = (uint32)QTV_PRIORITY_COUNT-1;
             index > (uint32)QTV_PRIORITY_INVALID;
             index--)
  {
    if(QtvInstances[index])
    {
      //priorities decrease in ascending order of priority values..
      if ( (uint32)player->InstancePriority >
           (uint32)(((QtvPlayer*)QtvInstances[index])->InstancePriority) )
      {
        //An instance exists whose priority is higher than the specified
        //instance priority, so we cannot suspend.
        result = QTV_RETURN_ERROR;
        break;
      }
    }
  }
  return result;
}
/* ======================================================================
FUNCTION
  QtvPlayer::RegisterForCallback

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::RegisterForCallback(CallbackFuncT callback, void *pClientData)
{
  QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                ">>QtvPlayer::RegisterForCallback %d %d",
                callback, pClientData);

  QtvPlayer *player = GetPlayer(NULL);

  player->CallbackFunction   = callback;
  player->CallbackClientData = pClientData;
  LogQtvCmd("RegisterForCallback", "%d%d", (int32)callback,(int32)pClientData);
  return QTV_RETURN_OK;
}

QtvPlayer::ReturnT QtvPlayer::RegisterForCallback(CallbackInstanceFuncT callback, void *pClientData, InstanceHandleT handle, void *pUserData)
{
  QTV_MSG_PRIO4(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                ">>QtvPlayer::RegisterForCallback %p %p %p %p",
                callback,pClientData,handle,pUserData);

  QtvPlayer *player = GetPlayer(handle);

  player->CallbackInstanceFunction = callback;
  player->CallbackClientData = pClientData;
  player->CallbackUserData = pUserData;
  LogQtvCmd("RegisterForCallback", "%d%d", (int32)callback,(int32)pClientData);
  return QTV_RETURN_OK;
}

/* ======================================================================
FUNCTION
  QtvPlayer::RegisterForCallbackInternal

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::RegisterForCallbackInternal(
  CallbackFuncT callback,
  CallbackInstanceFuncT callbackInstance,
  void *pClientData,
  void *pUserData
  )
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
               ">>QtvPlayer::RegisterForCallbackInternal");

  ReturnT nReturn = QTV_RETURN_OK;

#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
  // In order for RAM playback to work, all callbacks must be filtered by the RAM
  // Player.  This call to QtvRAMPlayer_SetCallback, will replace
  // the callback value with the one that the RAM player need us to use.
  if(InstancePriority == QtvPlayer::QTV_PRIORITY_DEFAULT)
  {
    QtvRAMPlayer_SetCallback(this, callback, callbackInstance, pClientData, pUserData);
  }

  if(CurrentMIMEType && (strcmp(CurrentMIMEType, REAL_MEDIA_MIME_TYPE) == 0))
  {
    QtvRealPlayer_SetCallback(callback, callbackInstance, (InstanceHandleT *)this, pClientData, pUserData);
  }
#endif /* FEATURE_REAL_PLAYER_USE_QTV_API */

  if (pMpeg4Player)
  {
    if (pMpeg4Player->StartThread())
    {
      if (callbackInstance)
      {
         PV_REGISTER_FOR_CALLBACK_EX_type *pEvent = QCCreateMessage(PV_REGISTER_FOR_CALLBACK_EX, pMpeg4Player);/*lint !e641 */

         if (pEvent)
         {
         pEvent->callback=callbackInstance;
         pEvent->pClientData=pClientData;
         pEvent->pUserData=pUserData;
         pEvent->handle=(InstanceHandleT *)this;
         pMpeg4Player->PostMessage(pEvent);
         nReturn = QTV_RETURN_OK;
         }
      }
      else if (callback)
      {
        PV_REGISTER_FOR_CALLBACK_type *pEvent = QCCreateMessage(PV_REGISTER_FOR_CALLBACK, pMpeg4Player);

        if (pEvent)
        {
           pEvent->callback=callback;
           pEvent->pClientData=pClientData;
           nReturn = QTV_RETURN_OK;
           pMpeg4Player->PostMessage(pEvent);
        }
      }
    }
  }
  LogQtvCmd("RegisterForCallbackInternal", "%d%d%d%d%d", (int32)nReturn, (int32)callback,(int32)callbackInstance,(int32)pClientData, (int32)pUserData);
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                ">>QtvPlayer::RegisterForCallbackInternal return %d",nReturn);
  return nReturn;
}

/* ======================================================================
FUNCTION
  QtvPlayer::PlayClip

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
========================================================================== */
QtvPlayer::ReturnT QtvPlayer::PlayClip(int32 startTime, int32 stopTime, InstanceHandleT handle)
{
  return PlayClip(startTime,stopTime,NULL,handle);
}

/* ======================================================================
FUNCTION
  QtvPlayer::PlayClip

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::PlayClip(int32 startTime, int32 stopTime,const char *switchToURN, InstanceHandleT handle)
{
  QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::PlayClip %d %d",startTime,stopTime);

  ReturnT nReturn = QTV_RETURN_ERROR;
  QtvPlayer *player = GetPlayer(handle);

  /*Check if we are trying to preempt a high priority instance */
  if(CheckInstances(handle) == QTV_RETURN_ERROR)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                 "Cannot play clip...Higher prioriy instance is active!!!");
    return nReturn;
  }

  /* PlayClip was called by the app, set app suspended to false */
#ifndef FEATURE_QTV_DISABLE_CONC_MGR
  qtv_conc_mgr::set_suspend_type(qtv_conc_mgr::NOT_SUSPENDED_EXTERNALLY,
                                 player->pMpeg4Player,
                                 player->CurrentMIMEType);
#endif

#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
  if (QtvRAMPlayer_RAMFilePlaying(player))
  {
    QtvRAMPlayer_PlayClip(startTime, stopTime, player);
  }

  if (player->CurrentMIMEType && strcmp(player->CurrentMIMEType, REAL_MEDIA_MIME_TYPE) == 0)
  {
    nReturn = QtvRealPlayer_PlayClip(startTime, stopTime);
    return nReturn;
  }
#endif
  if (player->pMpeg4Player)
  {
    if (player->pMpeg4Player->StartThread())
    {
      PV_PLAY_CLIP_type *pEvent = QCCreateMessage(PV_PLAY_CLIP, player->pMpeg4Player);/*lint !e641 */

      if (pEvent)
      {
        pEvent->startTime=startTime;
        pEvent->stopTime=stopTime;
        pEvent->switchURN = NULL;
        if(switchToURN != NULL)
        {
          pEvent->switchURN = QTV_New(URL);
          if((pEvent->switchURN != NULL))
          {
            pEvent->switchURN->SetUrl(switchToURN);
          }
        }
        pEvent->pbSpeed = player->pMpeg4Player->MapQtvPBSpeedToCommonPBSpeed(player->m_playbackSpeed);

        //Reset the abort flag before issueing playback.
        player->pMpeg4Player->SetMediaAbortFlag(false);
        player->pMpeg4Player->PostMessage(pEvent);

        /* The change in playback speed (if applicable) has been consumed,
        ** reset to the default setting.
        */
        player->m_playbackSpeed = PLAYBACK_SPEED_NORMAL;

        nReturn = QTV_RETURN_OK;
      }
    }
  }
  LogQtvCmd("PlayClip", "%d%d%d%d", (int32)nReturn, (int32)startTime,(int32)stopTime,(int32)handle);
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::PlayClip return %d",nReturn);
  return nReturn;
}

/* ======================================================================
FUNCTION
  QtvPlayer::SkipClip

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::SkipClip(int32 skipNumber, InstanceHandleT handle)
{
#ifdef FEATURE_FILE_FRAGMENTATION
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::SkipClip %d ",skipNumber);

  QtvPlayer *player = GetPlayer(handle);

  ReturnT nReturn = QTV_RETURN_ERROR;

  /* SkipClip was called by the app, set app suspended to false */
#ifndef FEATURE_QTV_DISABLE_CONC_MGR
  qtv_conc_mgr::set_suspend_type(qtv_conc_mgr::NOT_SUSPENDED_EXTERNALLY,
                                 player->pMpeg4Player,
                                 player->CurrentMIMEType);
#endif

  if (player->pMpeg4Player)
  {
    if (player->pMpeg4Player->StartThread())
    {
      QTV_SKIP_CLIP_type *pEvent = QCCreateMessage(QTV_SKIP_CLIP, player->pMpeg4Player);

      if (pEvent)
      {
        pEvent->skipNumber=skipNumber;

        player->pMpeg4Player->PostMessage(pEvent);
        nReturn = QTV_RETURN_OK;
      }
    }
  }

  LogQtvCmd("SkipClip", "%d%d%d", (int32)nReturn, (int32)skipNumber,(int32)handle);
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::SkipClip return %d",nReturn);
  return nReturn;
#else
    skipNumber = 0;
    QTV_USE_ARG1(handle);
    QTV_USE_ARG1(skipNumber);
    return QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif
}

QtvPlayer::ReturnT QtvPlayer::PlaylistPlayClip(int32 clipIndex,
                                               bool isClipIndexRelative,
                                               bool preferClientPlaylistSkip,
                                               InstanceHandleT handle)
{
  ReturnT nReturn = QTV_RETURN_ERROR;
  nReturn = PlaylistPlayClip(NULL, clipIndex, isClipIndexRelative, 0, -1,
                          preferClientPlaylistSkip, handle);
  LogQtvCmd("PlaylistPlayClip", "%d%d%d%d%d", nReturn, clipIndex, (int32)isClipIndexRelative, (int32)preferClientPlaylistSkip,(int32)handle);
  return nReturn;
}


QtvPlayer::ReturnT QtvPlayer::PlaylistPlayClip(const char *playlistName,
                                               int32 clipIndex, bool isClipIndexRelative,
                                               int32 time, int32 when, InstanceHandleT handle)
{
  ReturnT nReturn = QTV_RETURN_ERROR;
  nReturn = PlaylistPlayClip(playlistName, clipIndex, isClipIndexRelative, time,
                          when, true, handle);
  if(playlistName)
  {
  LogQtvCmd("PlaylistPlayClip", "%d%s%d%d%d%d%d", nReturn, playlistName, clipIndex, (int32)isClipIndexRelative, time, when,(int32)handle);
  }
  else
  {
     QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Cannot log Qtv cmd %u", playlistName);
  }
  return nReturn;
}

QtvPlayer::ReturnT QtvPlayer::PlaylistPlayClip(const char *playlistName,
                                               int32 clipIndex,
                                               bool isClipIndexRelative,
                                               int32 time, int32 when,
                                               bool preferClientPlaylistSkip,
                                               InstanceHandleT handle)
{
#ifdef FEATURE_QTV_PLAYLIST
  QTV_MSG_SPRINTF_PRIO_5(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
    ">>QtvPlayer::PlaylistPlayClip PLName:%s index:%d rel:%d time:%d when:%d",
    (playlistName == NULL) ? "NULL" : playlistName, clipIndex,
    isClipIndexRelative ? 1 : 0, time, when);

  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }

  ReturnT nReturn = QTV_RETURN_ERROR;

  if (player->pMpeg4Player)
  {
    if (player->pMpeg4Player->StartThread())
    {
      QTV_SKIP_type *pEvent = QCCreateMessage(QTV_SKIP, player->pMpeg4Player);

      if (pEvent)
      {
        pEvent->preferClientPlaylistSkip = preferClientPlaylistSkip;
        if (playlistName == NULL)
        {
          *(pEvent->playlistName) = NULL;
        }
        else
        {
          ZUtils::StrcpyN(pEvent->playlistName, playlistName, 256);
        }
        pEvent->clipIndex = clipIndex;
        pEvent->time = time;
        pEvent->when = when;
        pEvent->isClipIndexRelative = isClipIndexRelative;

        player->pMpeg4Player->PostMessage(pEvent);
        nReturn = QTV_RETURN_OK;
      }
    }
  }
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
    ">>QtvPlayer::PlaylistPlayClip return %d",nReturn);
  return nReturn;
#else
  QTV_USE_ARG1(playlistName);
  QTV_USE_ARG1(preferClientPlaylistSkip);
  QTV_USE_ARG1(clipIndex);
  QTV_USE_ARG1(time);
  QTV_USE_ARG1(when);
  QTV_USE_ARG1(isClipIndexRelative);
  QTV_USE_ARG1(handle);

  return QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif /* FEATURE_QTV_PLAYLIST */
}

/* ======================================================================
FUNCTION
  QtvPlayer::RecordClip

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::RecordClip(char* filename,
                                         bool overwrite,
                                         uint8 mode,
                                         uint32 duration,
                                         InstanceHandleT handle)
{
#ifdef FEATURE_QTV_STREAM_RECORD
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::RecordClip");

  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }
  ReturnT nReturn = QTV_RETURN_ERROR;

  if (player->pMpeg4Player)
  {
    if (player->pMpeg4Player->StartThread())
    {
      PV_RECORD_CLIP_type *pEvent = QCCreateMessage(PV_RECORD_CLIP, player->pMpeg4Player);/*lint !e641 */

      if (pEvent)
      {
        // Replaced strcpyn with more reliable ZUtils::StrcpyN
        ZUtils::StrcpyN(pEvent->fileName,filename,QTV_MAX_URN_BYTES+1);
        pEvent->overwrite=overwrite;
        pEvent->mode=mode;
        pEvent->duration=duration;
        player->pMpeg4Player->PostMessage(pEvent);
        nReturn = QTV_RETURN_OK;
      }
    }
  }
  if(filename)
  {
  LogQtvCmd("RecordClip", "%d%s%d%d%d%d", (int32)nReturn, filename, (int32)overwrite, (int32)mode, (int32)duration,(int32)handle);
  }
  else
  {
     QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Cannot log Qtv cmd %u", filename);
  }
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::RecordClip return %d",nReturn);
  return nReturn;
#else
  QTV_USE_ARG1(filename);
  QTV_USE_ARG1(overwrite);
  QTV_USE_ARG1(mode);
  QTV_USE_ARG1(duration);
  QTV_USE_ARG1(handle);

  return QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif
}

/* ======================================================================
FUNCTION
  QtvPlayer::StopClipRecording

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::StopClipRecording(InstanceHandleT handle)
{
#ifdef FEATURE_QTV_STREAM_RECORD
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::StopClipReording");

  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }

  ReturnT nReturn = QTV_RETURN_ERROR;

  if (player->pMpeg4Player)
  {
    if (player->pMpeg4Player->StartThread())
    {
      PV_RECORD_STOP_type *pEvent = QCCreateMessage(PV_RECORD_STOP, player->pMpeg4Player);/*lint !e641 */

      if (pEvent)
      {
        player->pMpeg4Player->PostMessage(pEvent);
        nReturn = QTV_RETURN_OK;
      }
    }
  }

  LogQtvCmd("StopClipRecording", "%d%d", (int32)nReturn, (int32)handle);
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::StopClipReording return %d",nReturn);
  return nReturn;
#else
  QTV_USE_ARG1(handle);

  return QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif
}

QtvPlayer::ReturnT QtvPlayer::SetStrRecEFSReservedSpace(uint32 resSpaceinBytes,
                                                        InstanceHandleT handle)
{
#ifdef FEATURE_QTV_STREAM_RECORD
  ReturnT nReturn = QTV_RETURN_ERROR;

  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }

  if (player->pMpeg4Player)
  {
    (void)player->pMpeg4Player->SetStrRecEFSReservedSpace(resSpaceinBytes);
    nReturn = QTV_RETURN_OK;
  }
  LogQtvCmd("SetStrRecEFSReservedSpace", "%d%d%d", (int32)nReturn, resSpaceinBytes,(int32)handle);
  return nReturn;
#else
  QTV_USE_ARG2(resSpaceinBytes, handle);

  return QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif
}

QtvPlayer::ReturnT QtvPlayer::getRecordedClipDuration(uint32& recduration,
                                                      InstanceHandleT handle)
{
#ifdef FEATURE_QTV_STREAM_RECORD
  ReturnT nReturn = QTV_RETURN_ERROR;

  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }

  recduration = 0;
  if (player->pMpeg4Player)
  {
    recduration = player->pMpeg4Player->getRecordedClipDuration();
    nReturn = QTV_RETURN_OK;
  }
  LogQtvCmd("getRecordedClipDuration", "%d%d%d", (int32)nReturn, recduration,(int32)handle);
  return nReturn;
#else
  QTV_USE_ARG2(recduration, handle);

  return QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif
}




/* ======================================================================
FUNCTION
  QtvPlayer::Stop

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::Stop(InstanceHandleT handle)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::Stop");

  // special case:  Stop is called after terminate.  To catch bugs
  // GetPlayer asserts if both handle and the Default Instance are NULL.
  // So check here first to see if he have already been terminated, and
  // just return an error if this is the case.
  if (handle == NULL && QtvInstances[QTV_PRIORITY_DEFAULT] == NULL)
  {
     return QTV_RETURN_ERROR;
  }

  QtvPlayer *player = GetPlayer(handle);
  ReturnT nReturn = QTV_RETURN_ERROR;

  /* Set playback speed to normal (default setting) */
  player->m_playbackSpeed = PLAYBACK_SPEED_NORMAL;

#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
  if (player->CurrentMIMEType && strcmp(player->CurrentMIMEType, REAL_MEDIA_MIME_TYPE) == 0)
  {
    nReturn = QtvRealPlayer_Stop();
    if (QtvRAMPlayer_RAMFilePlaying(player))
    {
      QtvRAMPlayer_Stop(player);
    }
    return nReturn;
  }
#endif
  if (player->pMpeg4Player)
  {
    /* If H.263 videoDec init is in progress,
       signal the player to synchronously stop it.
    */

    player->pMpeg4Player->setStopVideoInitFlag();
    if (player->pMpeg4Player->StartThread())
    {
      PV_STOP_type *pEvent = QCCreateMessage(PV_STOP, player->pMpeg4Player);/*lint !e641 */

      //Abort parsing/playing of the Media
       player->pMpeg4Player->SetMediaAbortFlag(true);
      if (pEvent)
      {
        player->pMpeg4Player->PostMessage(pEvent);
        nReturn = QTV_RETURN_OK;
      }
    }
  }
  LogQtvCmd("Stop", "%d%d", (int32)nReturn, (int32)handle);
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::Stop return %d",nReturn);
#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
  if (QtvRAMPlayer_RAMFilePlaying(player))
  {
    QtvRAMPlayer_Stop(player);
  }
#endif
  return nReturn;
}

/* ======================================================================
FUNCTION
  QtvPlayer::SuspendForIncomingCall

DESCRIPTION
  Function invoked to pause the player and release QoS on receiving an incoming
  voice call SETUP message from the Network. This interface should be invoked
  only by QVP and OEMMediaMPEG42PV interfaces on receiving a SETUP for an
  incoming voice call.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::SuspendForIncomingCall(
#if ( defined(FEATURE_QTV_QDSP_RELEASE_RESTORE) && defined(FEATURE_QTV_QOS_SELECTION) )
#error code not present
#else
  SuspendStatusCallbackT, void *, InstanceHandleT
#endif /* FEATURE_REAL_PLAYER_USE_QTV_API */
  )
{
#if (defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION)
#error code not present
#else
  return QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif
}

/* ======================================================================
FUNCTION
  QtvPlayer::RestoreAfterIncomingCall

DESCRIPTION
  Function invoked either by QVP or by OEMCM( through the OEMMediaMPEG42PV
  interface) after the incoming VT/voice call ends. This interface is to be
  used only by QVP and OEMCM. This function will restore QTV to the state
  it was in before the incoming voice call

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::RestoreAfterIncomingCallEnd(InstanceHandleT handle)
{
#if (defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION)
#error code not present
#else
  QTV_USE_ARG1(handle);

  return QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif
}

#if (defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION)
#error code not present
#endif /* (defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION) */

/* ======================================================================
FUNCTION
  QtvPlayer::Pause

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::Pause(InstanceHandleT handle)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::Pause");

  QtvPlayer *player = GetPlayer(handle);
  ReturnT nReturn = QTV_RETURN_ERROR;

  boolean pauseIsSuspend = true;

  /* Set the suspend type to APP_SUSPEND */
#ifndef FEATURE_QTV_DISABLE_CONC_MGR
  qtv_conc_mgr::set_suspend_type(qtv_conc_mgr::SUSPENDED_EXTERNALLY,
                                 player->pMpeg4Player,
                                 player->CurrentMIMEType);
#endif

#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
  if (player->CurrentMIMEType && strcmp(player->CurrentMIMEType, REAL_MEDIA_MIME_TYPE) == 0)
  {
    if (pauseIsSuspend)
    {
      return QtvRealPlayer_Suspend();
    }
    else
    {
      return QtvRealPlayer_Pause();
    }
  }
#endif
  if (player->pMpeg4Player)
  {
    pauseIsSuspend =
      (player->pMpeg4Player->GetQTVConfigItem(QtvConfig::QTVCONFIG_PAUSE_IS_SUSPEND)
       > 0) ? true : false;

    if (player->pMpeg4Player->StartThread())
    {
      if (pauseIsSuspend)
      {
        QTV_SUSPEND_type *pEvent = QCCreateMessage(QTV_SUSPEND, player->pMpeg4Player);/*lint !e641 */

        if (pEvent)
        {
          pEvent->bSuspendIsExternal = true;

          QTV_MSG_PRIO1(QTVDIAG_GENERAL,
            QTVDIAG_PRIO_LOW, "suspendtype %d",pEvent->bSuspendIsExternal);

          player->pMpeg4Player->PostMessage(pEvent);
          nReturn = QTV_RETURN_OK;
        }
      }
      else
      {
        PV_PAUSE_type *pEvent = QCCreateMessage(PV_PAUSE, player->pMpeg4Player);/*lint !e641 */

        if (pEvent)
        {
          player->pMpeg4Player->PostMessage(pEvent);
          nReturn = QTV_RETURN_OK;
        }
      }
    }
  }
  LogQtvCmd("Pause", "%d%d", (int32)nReturn, (int32)handle);

  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::Pause return %d",nReturn);
  return nReturn;
}

/* ======================================================================
FUNCTION
    QtvPlayer::SetAudioSource

DESCRIPTION
    Specifies the source of audio. Used to select or override audio
    concurrency. The audio source may be set at any time but a change
    only takes effect when resuming from suspended or playing from idle.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
    Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::SetAudioSource(AudioSourceFormatT audiosource,
                                             InstanceHandleT handle)
{
#ifdef FEATURE_QTV_IN_CALL_PHASE_2
#error code not present
#else
  QTV_USE_ARG1(audiosource);
  QTV_USE_ARG1(handle);
  return QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif /* FEATURE_QTV_IN_CALL_PHASE_2 */
}

/* ======================================================================
FUNCTION
  QtvPlayer::PlayNextFrame

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::PlayNextFrame(InstanceHandleT handle)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::PlayNextFrame");

  QtvPlayer *player = GetPlayer(handle);

  ReturnT nReturn = QTV_RETURN_ERROR;

  /* PlayNextFrame was called by the app, set app suspended to false */
#ifndef FEATURE_QTV_DISABLE_CONC_MGR
  qtv_conc_mgr::set_suspend_type(qtv_conc_mgr::NOT_SUSPENDED_EXTERNALLY,
                                 player->pMpeg4Player,
                                 player->CurrentMIMEType);
#endif

  if (player->pMpeg4Player)
  {
    if (player->pMpeg4Player->StartThread())
    {
      PV_NEXT_FRAME_type *pEvent = QCCreateMessage(PV_NEXT_FRAME, player->pMpeg4Player);/*lint !e641 */

      if (pEvent)
      {
        player->pMpeg4Player->PostMessage(pEvent);
        nReturn = QTV_RETURN_OK;
      }
    }
  }
#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
  else if (player->CurrentMIMEType && strcmp(player->CurrentMIMEType, REAL_MEDIA_MIME_TYPE) == 0)
  {
    nReturn = QTV_RETURN_FEATURE_NOT_AVAILABLE;
  }
#endif /* FEATURE_REAL_PLAYER_USE_QTV_API */

  LogQtvCmd("PlayNextFrame", "%d%d", (int32)nReturn, (int32)handle);
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::PlayNextFrame return %d",nReturn);
  return nReturn;
}

/* ======================================================================
FUNCTION
  QtvPlayer::PlayPrevFrame

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::PlayPrevFrame(InstanceHandleT handle)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::PlayPrevFrame");

  QtvPlayer *player = GetPlayer(handle);

  ReturnT nReturn = QTV_RETURN_ERROR;

  /* PlayPrevFrame was called by the app, set app suspended to false */
#ifndef FEATURE_QTV_DISABLE_CONC_MGR
  qtv_conc_mgr::set_suspend_type(qtv_conc_mgr::NOT_SUSPENDED_EXTERNALLY,
                                 player->pMpeg4Player,
                                 player->CurrentMIMEType);
#endif

  if (player->pMpeg4Player)
  {
    if (player->pMpeg4Player->StartThread())
    {
      PV_PREV_FRAME_type *pEvent = QCCreateMessage(PV_PREV_FRAME, player->pMpeg4Player);/*lint !e641 */

      if (pEvent)
      {
        player->pMpeg4Player->PostMessage(pEvent);
        nReturn = QTV_RETURN_OK;
      }
    }
  }
#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
  else if (player->CurrentMIMEType && strcmp(player->CurrentMIMEType, REAL_MEDIA_MIME_TYPE) == 0)
  {
    nReturn = QTV_RETURN_FEATURE_NOT_AVAILABLE;
  }
#endif /* FEATURE_REAL_PLAYER_USE_QTV_API */

  LogQtvCmd("PlayPrevFrame", "%d%d", (int32)nReturn, (int32)handle);
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::PlayPrevFrame return %d",nReturn);
  return nReturn;
}

/* ======================================================================
FUNCTION
  QtvPlayer::GetURNType

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::GetURNType(URNTypeT &URNType, bool bVideoURN,
                                         InstanceHandleT handle)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::GetURNType");

  QtvPlayer *player = GetPlayer(handle);
  ReturnT nReturn = QTV_RETURN_ERROR;
  SetNullURNType(URNType);

  if (player->pMpeg4Player)
  {
    nReturn = player->pMpeg4Player->GetURNType(URNType, bVideoURN);
  }
  LogQtvCmd("GetURNType", "%d%d%d%d", (int32)nReturn, (int32)URNType, (int32)bVideoURN,(int32)handle);
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::GetURNType return %d",nReturn);
  return nReturn;
}

/* ======================================================================
FUNCTION
  QtvPlayer::GetURLType

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::GetURLType(URLTypeT &URLType,
                                         InstanceHandleT handle)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::GetURLType");

  QtvPlayer *player = GetPlayer(handle);
  ReturnT nReturn = QTV_RETURN_ERROR;
  SetNullURLType(URLType);

  if (player->pMpeg4Player)
  {
    nReturn = player->pMpeg4Player->GetURLType(URLType);
  }
  LogQtvCmd("GetURLType", "%d%d%d", (int32)nReturn, (int32)URLType,(int32)handle);
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::GetURLType return %d",nReturn);
  return nReturn;
}

/* ======================================================================
FUNCTION:
  QtvPlayer::OpenPseudoStream (External API)

DESCRIPTION:
  Method to open a pseudo stream. Attempt to open second instance with
  different priority will be failed.

PARAMETERS:
  pBuf
    Pointer to the beginning of pseudo stream buffer.

  dwBufSize
    Size of pseudo stream buffer.

  wPtrOffset
    Current pseudo stream buffer write pointer offset.
    It is assumed that enough bytes shall be present in the buffer to figure
    out the mime type. Attempt to open the stream with insufficient number of
    bytes shall be failed.

  handle
    Handle to the player instance.

  priority
    Priority of the player.

RETURN VALUE:
  QtvPlayer ReturnT type

SIDE EFFECTS:
  None.
======================================================================*/
QtvPlayer::ReturnT  QtvPlayer::OpenPseudoStream( unsigned char *pBuf,
                                  uint32 dwBufSize,
                                  uint32 wPtrOffset,
  InstanceHandleT handle,
  InstancePriorityT priority)
  {
  ReturnT nReturn = QTV_RETURN_ERROR;

#ifdef FEATURE_QTV_PSEUDO_STREAM
            //Play all the available tracks inside pBuf
  uint32 bmTrackSelection = ( QTV_VIDEO_TRACK_SELECTED |
                              QTV_AUDIO_TRACK_SELECTED |
                              QTV_TEXT_TRACK_SELECTED);

  nReturn = OpenPseudoStream(pBuf, dwBufSize, wPtrOffset, bmTrackSelection,
                             handle, priority);

  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
                ">>QtvPlayer::OpenPseudoStream returned %d", nReturn);
#else
    dwBufSize = 0;
    wPtrOffset = 0;
    pBuf = NULL;
    QTV_USE_ARG1(handle);
    QTV_USE_ARG1(priority);
    QTV_USE_ARG1(pBuf);
    QTV_USE_ARG1(dwBufSize);
    QTV_USE_ARG1(wPtrOffset);
    nReturn = QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif /* FEATURE_QTV_PSEUDO_STREAM */

    return nReturn;
  }

/* ======================================================================
FUNCTION:
  QtvPlayer::OpenPseudoStream (Internal Method)

DESCRIPTION:
  Method to open a pseudo stream. Attempt to open second instance with
  different priority will be failed.

PARAMETERS:
  pBuf
    Pointer to the beginning of pseudo stream buffer.

  dwBufSize
    Size of pseudo stream buffer.

  wPtrOffset
    Current pseudo stream buffer write pointer offset.
    It is assumed that enough bytes shall be present in the buffer to figure
    out the mime type. Attempt to open the stream with insufficient number of
    bytes shall be failed.

  dwTrackSelected
    Bit mask which specifies which combination of tracks to play
    QTV_NO_TRACK_SELECTED
    QTV_VIDEO_TRACK_SELECTED
    QTV_AUDIO_TRACK_SELECTED
    QTV_TEXT_TRACK_SELECTED

  InstanceHandleT  handle
    Handle to the player instance.

  InstancePriorityT priority
    Priority of the player.

RETURN VALUE:
  QtvPlayer ReturnT type

SIDE EFFECTS:
  None.
======================================================================*/
QtvPlayer::ReturnT  QtvPlayer::OpenPseudoStream( unsigned char *pBuf,
                                  uint32 dwBufSize,
                                  uint32 wPtrOffset,
                                  uint32 dwTrackSelected,
  InstanceHandleT handle,
  InstancePriorityT priority)
  {
  ReturnT nReturn = QTV_RETURN_ERROR;

#ifdef FEATURE_QTV_PSEUDO_STREAM



  QtvPlayer *player = GetPlayer(handle);

  player->CurrentMIMEType = QTV_PLAYER_MIME_TYPE;

    /* Pseudo stream buffer should not be NULL */
  if (pBuf && dwBufSize &&
      wPtrOffset && (wPtrOffset <= dwBufSize) &&
      dwTrackSelected && (priority > QTV_PRIORITY_INVALID) &&
      (priority < QTV_PRIORITY_COUNT))
  {
    /* Constraint - 1: Multiple instances check
    ** If another registered instance is already present,
    ** then this API will not be functional for the second instance.
    */
    if((numRegisteredQtvPlayers >= 1) && (player->InstancePriority != priority))
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
      "OpenPseudoStream failed due to multiple instances");
      return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
    }
    player->CurrentMIMEType = QtvPlayer::GetMIMEType(pBuf, wPtrOffset, wPtrOffset);

    if (player->CurrentMIMEType)
    {
#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
      /* The Qtv RAM Player must be initialized before initializing the Qtv
      ** Player. Otherwise the set callback function for the RAM Player in
      ** QtvPlayer::Init will fail.
      */
      QtvRAMPlayer_Init(player);
      if(player->RAMPlayer ==NULL)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
          ">>QtvPlayer::OpenURN:QtvRAMPlayer_Init failed..");
        return QTV_RETURN_ERROR;
      }
#endif /* FEATURE_REAL_PLAYER_USE_QTV_API */

      player->Init(player->MallocBufferFunction,
                   player->FreeBufferFunction,
                   player->CurrentMIMEType);

      if (player->pMpeg4Player &&
          player->pMpeg4Player->StartThread())
      {
#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */

        QTV_PS_STREAM_OPEN_EVENT_type *pEvent =
                QCCreateMessage(QTV_PS_STREAM_OPEN_EVENT, player->pMpeg4Player);

        if (pEvent)
        {
          pEvent->pPSBuffer = pBuf;
          pEvent->bufferSize = dwBufSize;
          pEvent->writePtrOffset = wPtrOffset;
          pEvent->dwTrackSelected = dwTrackSelected;
          pEvent->FetchBufferedDataSize = NULL;
          pEvent->FetchBufferedData = NULL;
          pEvent->handle = NULL;

          /* Reset the abort flag before issuing playback. */
          player->pMpeg4Player->SetMediaAbortFlag(false);

          player->pMpeg4Player->PostMessage(pEvent);
          // Finally if everything works out then do the registration
          // This is done in this way to avoid de-registration while bailing out
          nReturn = RegisterInst (handle,priority);
          if (nReturn != QTV_RETURN_OK)
          {
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
            "OpenPseudoStream failed due to trouble observed while registering");
            player->InitRollback();
            #ifdef FEATURE_REAL_PLAYER_USE_QTV_API
            // Cancel the RAM player Init too. Dont want to leak any memory
            // in case client forget to issue the subsequent open.
            QtvRAMPlayer_Terminate(player);
            #endif
          }
        }
        else
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
          "OpenPseudoStream failed due to player message creation failure");
        }
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "OpenPseudoStream failed due to absence of player thread or object");
      }
        }
    else
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
      "OpenPseudoStream failed since MIME Type couldnt be figured");
      }
    }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
    "OpenPseudoStream failed due to invalid parameters");
  }

  LogQtvCmd("OpenPseudoStream", "%d%d%d%d%d",
            (int32)nReturn, dwBufSize, wPtrOffset, dwTrackSelected,
            (int32)handle, (int32)priority);

  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
                ">>QtvPlayer::OpenPseudoStream(dwTrackSelected) returned %d",
              nReturn);
#else
    pBuf = NULL;
    dwBufSize = 0;
    wPtrOffset = 0;
    dwTrackSelected = 0;
    QTV_USE_ARG1(handle);
    QTV_USE_ARG1(priority);
    QTV_USE_ARG1(pBuf);
    QTV_USE_ARG1(dwBufSize);
    QTV_USE_ARG1(wPtrOffset);
    QTV_USE_ARG1(dwTrackSelected);
  nReturn = QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif /* FEATURE_QTV_PSEUDO_STREAM */

  return nReturn;
  }


/*------------------------------------------------------------------------
  Method to update current pseudo stream buffer write pointer offset.
  Application should call this method and update Qtv after every write
  operation to pseudo stream buffer.

  Arguments:

  wPtrOffset: Write pointer offset from the beginning of pseudo stream buffer
  This value is expressed in bytes.

  wPtrOffset MUST always increment. Application should never pass an offset
  value which is less than the value issued in past updates. Such an update
  will be treated as fatal error in Qtv.

  -------------------------------------------------------------------------*/
QtvPlayer::ReturnT  QtvPlayer::UpdatePseudoStreamBufferWriteOffset(
#ifdef FEATURE_QTV_PSEUDO_STREAM
  uint32 wPtrOffset,
  InstanceHandleT handle
#else
  uint32,
  InstanceHandleT
#endif /* FEATURE_QTV_PSEUDO_STREAM */
  )
{
#ifdef FEATURE_QTV_PSEUDO_STREAM
  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }

  ReturnT nReturn = QTV_RETURN_ERROR;

  if(player->pMpeg4Player && wPtrOffset)
  {
    if (player->pMpeg4Player->StartThread())
    {
      nReturn = UpdatePushBufferedDataOffset(wPtrOffset);
    }
  }
  LogQtvCmd("UpdatePseudoStreamBufferWriteOffset", "%d%d%d", (int32)nReturn, wPtrOffset,(int32)handle);
  return nReturn;
#else
  return QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif
}



/*------------------------------------------------------------------------
    Method to open an HTTP Stream.
    Arguments:
    pBuf: pointer to the beginning of HTTP stream buffer
    dwBufSize: size of HTTP stream buffer
    wPtrOffset: current HTTP stream buffer write pointer offset.
    It should be 0 if nothing has been written to the buffer yet.

    Notes: OpenHTTPStream( ) will fail if an HTTP stream is already active.

  -------------------------------------------------------------------------*/

QtvPlayer::ReturnT  QtvPlayer::OpenHTTPStream(
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  unsigned char *pBuf,
  uint32 dwBufSize,
  uint32 wPtrOffset,
  InstanceHandleT handle, InstancePriorityT priority
#else
  unsigned char *, uint32, uint32, InstanceHandleT, InstancePriorityT
#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
  )
{
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  ReturnT nReturn = QTV_RETURN_ERROR;

  //Play all the available tracks inside pBuf
  nReturn = OpenHTTPStream(pBuf, dwBufSize, wPtrOffset, 0x7, handle, priority);
  LogQtvCmd("OpenHTTPStream", "%d%d%d%d%d", (int32)nReturn, dwBufSize, wPtrOffset,(int32)handle,(int32)priority);
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,">>QtvPlayer::OpenHTTPStream returned %d",
                nReturn);
  return nReturn;
#else
  return QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif
}

/*------------------------------------------------------------------------
    Method to open an HTTP Stream with Push Interface.
    Arguments:
    pBuf: pointer to the beginning of HTTP stream buffer
    dwBufSize: size of HTTP stream buffer
    wPtrOffset: current HTTP stream buffer write pointer offset.
    It should be 0 if nothing has been written to the buffer yet.
    dwTrackSelected: specifies which combination of tracks to paly (audio/video/text)
                     #define QTV_NO_TRACK_SELECTED             0x00
                     #define QTV_VIDEO_TRACK_SELECTED          0x01
                     #define QTV_AUDIO_TRACK_SELECTED          0x02
                     #define QTV_TEXT_TRACK_SELECTED           0x04

    Notes: OpenHTTPStream( ) will fail if an HTTP stream is already active and if
    no tracks are selected.

  -------------------------------------------------------------------------*/

QtvPlayer::ReturnT QtvPlayer::OpenHTTPStream(
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  unsigned char *pBuf,
  uint32 dwBufSize,
  uint32 wPtrOffset,
  uint32 dwTrackSelected,
  InstanceHandleT handle, InstancePriorityT priority
#else
  unsigned char *, uint32, uint32, uint32, InstanceHandleT, InstancePriorityT

#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
  )
{
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  ReturnT nReturn = QTV_RETURN_ERROR;

  /* HTTP stream buffer should not be NULL */
  //Use the Push Interface
  nReturn = OpenPushBufferedStream(pBuf, dwBufSize, wPtrOffset, dwTrackSelected, handle, priority);
  LogQtvCmd("OpenHTTPStream", "%d%d%d%d%d%d", (int32)nReturn, dwBufSize, wPtrOffset, dwTrackSelected,(int32)handle,(int32)priority);
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,">>QtvPlayer::OpenHTTPStream returned %d",
                nReturn);
  return nReturn;

#else
  return QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif
}

/* ======================================================================
FUNCTION:
  QtvPlayer::OpenPushBufferedStream (Internal Method)

DESCRIPTION:
  Method to open an buffered stream with push interface.
  Attempt to open second instance with different priority
  will be failed.

PARAMETERS:
  pBuf
    Pointer to the beginning of Push Buffered stream.

  dwBufSize
    Size of Push Buffered stream.

  writePtrOffset
    Current Push Buffered stream write pointer offset.
    It is assumed that enough bytes shall be present in the buffer to figure
    out the mime type. Attempt to open the stream with insufficient number of
    bytes shall be failed.

  uint32 dwTrackSelected
    Bit mask which specifies which combination of tracks to play
    QTV_NO_TRACK_SELECTED
    QTV_VIDEO_TRACK_SELECTED
    QTV_AUDIO_TRACK_SELECTED
    QTV_TEXT_TRACK_SELECTED

  InstanceHandleT  handle
    Handle to the player instance.

  InstancePriorityT priority
    Priority of the player.

RETURN VALUE:
  QtvPlayer ReturnT type

SIDE EFFECTS:
  None.
======================================================================*/
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
QtvPlayer::ReturnT QtvPlayer::OpenPushBufferedStream( unsigned char *pBuf,
  uint32 dwBufSize,
  uint32 writePtrOffset,
  uint32 dwTrackSelected,
                                                      InstanceHandleT handle,
                                                      InstancePriorityT priority)
#else
QtvPlayer::ReturnT QtvPlayer::OpenPushBufferedStream( unsigned char *,
                                                      uint32,
                                                      uint32,
                                                      uint32,
                                                      InstanceHandleT,
                                                      InstancePriorityT)
#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
{
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD


  ReturnT nReturn = QTV_RETURN_ERROR;
  QtvPlayer *player = GetPlayer(handle);

  player->CurrentMIMEType = QTV_PLAYER_MIME_TYPE;
  if (pBuf && dwBufSize &&
      writePtrOffset && (writePtrOffset <= dwBufSize) &&
      dwTrackSelected &&
      (priority > QTV_PRIORITY_INVALID) && (priority < QTV_PRIORITY_COUNT) )
  {
    if((numRegisteredQtvPlayers >= 1) && (player->InstancePriority != priority))
  {
      /* Constraint - 1: Multiple instances check
      ** If another registered instance is already present,
      ** then this API will not be functional for the second instance.
      */
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
      "OpenPushBufferedStream failed due to multiple instances");
      return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
    }
    player->CurrentMIMEType = QtvPlayer::GetMIMEType(pBuf, writePtrOffset, writePtrOffset);
    if(player->CurrentMIMEType)
    {
#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
      // The Qtv RAM Player must be initialized before initializing the Qtv Player
      // Otherwise the set callback function for the RAM Player in QtvPlayer::Init will fail
      QtvRAMPlayer_Init(player);
      if(player->RAMPlayer ==NULL)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,">>QtvPlayer::OpenURN:QtvRAMPlayer_Init failed..");
        return QTV_RETURN_ERROR;
      }
#endif /* FEATURE_REAL_PLAYER_USE_QTV_API */

      player->Init(player->MallocBufferFunction,
                   player->FreeBufferFunction,
                   player->CurrentMIMEType);

  /* Atleast one track should be selected */
      if (player->pMpeg4Player && player->pMpeg4Player->StartThread())
    {
#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */

      QTV_HTTP_STREAM_OPEN_EVENT_type *pEvent =
        QCCreateMessage(QTV_HTTP_STREAM_OPEN_EVENT, player->pMpeg4Player);/*lint !e641 */

      if (pEvent)
      {
        pEvent->pPSBuffer = pBuf;
        pEvent->bufferSize = dwBufSize;
        pEvent->FetchBufferedDataSize = NULL;
        pEvent->FetchBufferedData = NULL;
        pEvent->handle = NULL;
        pEvent->writePtrOffset = writePtrOffset;
        pEvent->dwTrackSelected = dwTrackSelected;

        //Reset the abort flag before issueing playback.
        player->pMpeg4Player->SetMediaAbortFlag(false);
        player->pMpeg4Player->PostMessage(pEvent);
          // Finally if everything works out then do the registration
          // This is done in this way to avoid de-registration while bailing out
          nReturn = RegisterInst (handle,priority);
          if (nReturn != QTV_RETURN_OK)
          {
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
            "OpenPushBufferedStream failed due to trouble observed while registering");
            player->InitRollback();
            #ifdef FEATURE_REAL_PLAYER_USE_QTV_API
            // Cancel the RAM player Init too. Dont want to leak any memory
            // in case client forget to issue the subsequent open.
            QtvRAMPlayer_Terminate(player);
            #endif
          }
        }
        else
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
          "OpenPushBufferedStream failed due to player message creation issues");
        }
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "OpenPushBufferedStream failed due to absence of player thread or object");
      }
    }
    else
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
      "OpenPushBufferedStream failed since MIME TYPE couldnt be figured ");
      }
    }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
    "OpenPushBufferedStream failed due to invalid parameters passed");
  }

  LogQtvCmd("OpenPushBufferedStream", "%d%d%d%d%d%d", (int32)nReturn, dwBufSize, writePtrOffset,
            dwTrackSelected,(int32)handle,(int32)priority);
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,">>QtvPlayer::OpenPushBufferedStream returned %d",
                nReturn);
  return nReturn;

#else
  return QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif
}

/* ======================================================================
FUNCTION:
  QtvPlayer::OpenPullBufferedStream (Internal Method)

DESCRIPTION:
  Method to open an buffered stream with pull interface.
  Attempt to open second instance with different priority
  will be failed.

PARAMETERS:
  FetchBufferedDataSize
    OEM callback to pull the size of the data currently buffered.

  FetchBufferedData
    OEM callback to the pull the buffered data.

  dwTrackSelected
    Bit mask which specifies which combination of tracks to play
    QTV_NO_TRACK_SELECTED
    QTV_VIDEO_TRACK_SELECTED
    QTV_AUDIO_TRACK_SELECTED
    QTV_TEXT_TRACK_SELECTED

  handle
    Handle to the player instance.

  priority
    Priority of the player.

RETURN VALUE:
  QtvPlayer ReturnT type

SIDE EFFECTS:
  None.
======================================================================*/
#if (defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)) && defined(FEATURE_QTV_BUFFERED_PULL_INTERFACE)
QtvPlayer::ReturnT QtvPlayer::OpenPullBufferedStream( FetchBufferedDataSizeT FetchBufferedDataSize,
  FetchBufferedDataT FetchBufferedData,
  uint32 dwTrackSelected,
                                                      InstanceHandleT handle,
                                                      InstancePriorityT priority)
#else
QtvPlayer::ReturnT QtvPlayer::OpenPullBufferedStream( FetchBufferedDataSizeT,
                                                      FetchBufferedDataT,
                                                      uint32,
                                                      InstanceHandleT,
                                                      InstancePriorityT)
#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD && FEATURE_QTV_BUFFERED_PULL_INTERFACE */
{

#if (defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)) && defined(FEATURE_QTV_BUFFERED_PULL_INTERFACE)


  ReturnT nReturn = QTV_RETURN_ERROR;

  QtvPlayer *player = GetPlayer(handle);

  uint8 httpDataBuf[MAXIMUM_MIME_BUFFER_LENGTH];
  uint32 wHttpBufOffset = 0;

  player->CurrentMIMEType = QTV_PLAYER_MIME_TYPE;

  if( FetchBufferedDataSize && FetchBufferedData && dwTrackSelected &&
      (priority > QTV_PRIORITY_INVALID) && (priority < QTV_PRIORITY_COUNT))
  {
     boolean bEndOfData = false;
     /* Get down loaded data offset from OEM */
     FetchBufferedDataSize( 0,&wHttpBufOffset,&bEndOfData, handle);

     /* Get the data from OEM, use default track id 0x0 */
     (void)FetchBufferedData( httpDataBuf, MAXIMUM_MIME_BUFFER_LENGTH, 0, 0x0, handle);

     if (wHttpBufOffset)
     {
       player->CurrentMIMEType = QtvPlayer::GetMIMEType(httpDataBuf, wHttpBufOffset, wHttpBufOffset);

       if(player->CurrentMIMEType)
       {
#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
         // The Qtv RAM Player must be initialized before initializing the Qtv Player
         // Otherwise the set callback function for the RAM Player in QtvPlayer::Init will fail
         QtvRAMPlayer_Init(player);
         if(player->RAMPlayer ==NULL)
         {
           QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,">>QtvPlayer::OpenURN:QtvRAMPlayer_Init failed..");
           return QTV_RETURN_ERROR;
         }
#endif /* FEATURE_REAL_PLAYER_USE_QTV_API */

         player->Init(player->MallocBufferFunction,
                      player->FreeBufferFunction,
                      player->CurrentMIMEType);
        if (player->pMpeg4Player && player->pMpeg4Player->StartThread())
    {
#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
      QTV_HTTP_STREAM_OPEN_EVENT_type *pEvent =
        QCCreateMessage(QTV_HTTP_STREAM_OPEN_EVENT, player->pMpeg4Player);
#elif defined FEATURE_QTV_PSEUDO_STREAM
      QTV_PS_STREAM_OPEN_EVENT_type *pEvent =
        QCCreateMessage(QTV_PS_STREAM_OPEN_EVENT, player->pMpeg4Player);
#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */

      if (pEvent)
      {
        pEvent->FetchBufferedDataSize = FetchBufferedDataSize;
        pEvent->FetchBufferedData = FetchBufferedData;
        pEvent->handle = handle;
        pEvent->pPSBuffer = NULL;
        pEvent->bufferSize = 0;
        pEvent->writePtrOffset = 0;
        pEvent->dwTrackSelected = dwTrackSelected;

        //Reset the abort flag before issueing playback.
        player->pMpeg4Player->SetMediaAbortFlag(false);
        player->pMpeg4Player->PostMessage(pEvent);
            // Finally if everything works out then do the registration
            // This is done in this way to avoid de-registration while bailing out
            nReturn = RegisterInst (handle,priority);
            if (nReturn != QTV_RETURN_OK)
            {
              QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
              "OpenPullBufferedStream failed due to trouble observed while registering");
              player->InitRollback();
              #ifdef FEATURE_REAL_PLAYER_USE_QTV_API
              // Cancel the RAM player Init too. Dont want to leak any memory
              // in case client forget to issue the subsequent open.
              QtvRAMPlayer_Terminate(player);
              #endif
            }
          }
          else
          {
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
            "OpenPullBufferedStream failed due to player message creation issues");
          }
        }
        else
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
          "OpenPullBufferedStream failed due to invalid player thread or object");
        }
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "OpenPullBufferedStream failed since MIME-Type Couldnt be figured");
      }
    }
    else
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
      "OpenPullBufferedStream failed since MIME-Type Couldnt be figured");
  }
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
    "OpenPullBufferedStream failed due to invalid parameters");
  }

  /* Atleast one track should be selected */
  LogQtvCmd("OpenPullBufferedStream", "%d%d%d%d%d", (int32)nReturn,(int32)FetchBufferedDataSize,
            (int32)FetchBufferedData,dwTrackSelected,(int32)handle,(int32)priority);
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,">>QtvPlayer::OpenPullBufferedStream returned %d",
                nReturn);
  return nReturn;

#else
  return QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif
}

  /*------------------------------------------------------------------------
   Method to update current buffered stream buffer write pointer offset.
   Application should call this method and update Qtv after every write
   operation to stream buffer.

   Arguments:

   writePtrOffset: Write pointer offset from the beginning of stream buffer
           This value is expressed in bytes and it MUST always increment. It it
           decreases then the player might enter rebuffering depending on the
           value of the writePtrOffset.

  -------------------------------------------------------------------------*/
QtvPlayer::ReturnT QtvPlayer::UpdatePushBufferedDataOffset(
#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  uint32 writePtrOffset,
  InstanceHandleT handle
#else
  uint32,
  InstanceHandleT
#endif /* defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) */
  )
{
#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }

  ReturnT nReturn = QTV_RETURN_ERROR;

  /* update offset should be +ve */
  if (player->pMpeg4Player && writePtrOffset)
  {
    if (player->pMpeg4Player->StartThread())
    {
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
      QTV_HTTP_STREAM_UPDATE_WBUFFER_OFFSET_EVENT_type *pEvent =
        QCCreateMessage(QTV_HTTP_STREAM_UPDATE_WBUFFER_OFFSET_EVENT, player->pMpeg4Player);/*lint!e641 */
#else
      QTV_PS_UPDATE_WBUFFER_OFFSET_EVENT_type *pEvent =
        QCCreateMessage(QTV_PS_UPDATE_WBUFFER_OFFSET_EVENT, player->pMpeg4Player);/*lint!e641 */
#endif
      if (pEvent)
      {
        pEvent->writePtrOffset = writePtrOffset;
        player->pMpeg4Player->PostMessage(pEvent);
        nReturn = QTV_RETURN_OK;
      }
    }
  }
  QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,">>QtvPlayer::UpdatePushBufferedDataOffset returned=%d wPtrOffset=%d",
                nReturn,writePtrOffset);

  return nReturn;
#else
  return QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif

}

  /*------------------------------------------------------------------------
   Method to update current HTTP Stream buffer write pointer offset.
   Application should call this method and update Qtv after every write
   operation to HTTP stream buffer.

   Arguments:

   wPtrOffset: Write pointer offset from the beginning of HTTP stream buffer
           This value is expressed in bytes.

   wPtrOffset MUST always increment. Application should never pass an offset
   value which is less than the value issued in past updates. Such an update
   will be treated as fatal error in Qtv.

  -------------------------------------------------------------------------*/

QtvPlayer::ReturnT  QtvPlayer::UpdateHTTPStreamBufferWriteOffset(
#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  uint32 wPtrOffset,
  InstanceHandleT handle
#else
  uint32,
  InstanceHandleT
#endif /* defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)*/
  )
{
#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }

  ReturnT nReturn = QTV_RETURN_ERROR;

  /* update offset should be +ve */
  if (player->pMpeg4Player && wPtrOffset)
  {
    if (player->pMpeg4Player->StartThread())
    {
      nReturn = UpdatePushBufferedDataOffset(wPtrOffset);
    }
  }
  LogQtvCmd("UpdateHTTPStreamBufferWriteOffset", "%d%d%d", (int32)nReturn, wPtrOffset,(int32)handle);
  QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,">>QtvPlayer::UpdateHTTPStreamBufferWriteOffset returned=%d wPtrOffset=%d",
                nReturn,wPtrOffset);

  return nReturn;
#else
  return QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif // defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
}

  /*------------------------------------------------------------------------
   Method to update the start up time and buffering Resume Time.
   Application should call this method and update Qtv before starting
   HTTP Streaming.

   Arguments:

   startupTime : playback time required before playback can start
                 This value is expressed in ms.
   bufferingResumeTime: While buffering the playback time required before
                       playback can restart. This value is expressed in ms.

  -------------------------------------------------------------------------*/

QtvPlayer::ReturnT  QtvPlayer::SetStartAndBufferingTime(
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  uint32 startupTime,
  uint32 bufferingResumeTime,
  InstanceHandleT handle
#else
  uint32, uint32, InstanceHandleT
#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
  )
{
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }

  ReturnT nReturn = QTV_RETURN_ERROR;

  if(player->pMpeg4Player && startupTime && bufferingResumeTime)
  {
      player->pMpeg4Player->setStartAndBufferingTime(startupTime, bufferingResumeTime);
      nReturn = QTV_RETURN_OK;
  }
  LogQtvCmd("SetStartAndBufferingTime", "%d%d%d%d", (int32)nReturn, startupTime, bufferingResumeTime,(int32)handle);
  return nReturn;
#else
  return QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif
}

 /*------------------------------------------------------------------------
    Method to register OEM callbacks with QTV
    Arguments:
    Input Arguments:
   OEM callback to send Encypted Data from QTV to OEM
   OEM callback to Fetch the size of the data currently buffered.
   OEM callback to Fetch the buffered data
   OEM callback to query if MIME type is supported.
   Maxiumum Size of that data buffer QTV can use during WriteEncryptedDataCB.

   Return Type:
   QTV_RETURN_OK indicating success, any other value indicates failure. */
/*  -------------------------------------------------------------------------*/
/*lint -e550 */
QtvPlayer::ReturnT QtvPlayer::RegisterCallbacksForDownload(
#ifdef FEATURE_QTV_HTTP_DOWNLOAD
  QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize,
  QtvPlayer::FetchBufferedDataT FetchBufferedData,
  QtvPlayer::IsMimeTypeSupportedT IsMimeTypeSupported,
  InstanceHandleT handle
#else
  QtvPlayer::FetchBufferedDataSizeT,
  QtvPlayer::FetchBufferedDataT,
  QtvPlayer::IsMimeTypeSupportedT,
  InstanceHandleT
#endif /* FEATURE_QTV_HTTP_DOWNLOAD */
  )
{
#ifdef FEATURE_QTV_HTTP_DOWNLOAD
  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }
    ReturnT nReturn = QTV_RETURN_ERROR;

    /* Atleast one track should be selected */
    if (player->pMpeg4Player && FetchBufferedDataSize &&
        FetchBufferedData && IsMimeTypeSupported)
    {

        if (player->pMpeg4Player->StartThread())
        {
          QTV_HTTP_REGISTER_CALLBACK_EVENT_type *pEvent =
                QCCreateMessage(QTV_HTTP_REGISTER_CALLBACK_EVENT, player->pMpeg4Player);

          if (pEvent)
          {
            pEvent->FetchBufferedDataSize = FetchBufferedDataSize;
            pEvent->FetchBufferedData     = FetchBufferedData;
            pEvent->IsMimeTypeSupported   = IsMimeTypeSupported;
            player->pMpeg4Player->PostMessage(pEvent);
            nReturn = QTV_RETURN_OK;
          }
        }
    }
    LogQtvCmd("RegisterCallbacksforDownload", "%d%d%d%d", (int32)nReturn, (int32)FetchBufferedDataSize,(int32)FetchBufferedData,(int32)IsMimeTypeSupported, (int32)handle);
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,">>QtvPlayer::RegisterCallbacksForDownload returned %d",
              nReturn);
    return nReturn;

#else
    return QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif

}
/* ======================================================================
FUNCTION
  QtvPlayer::FreeDownloadBuffer

DESCRIPTION
  API invoked by OEM to ask QTV for data

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::FreeDownloadBuffer(
#ifdef FEATURE_QTV_HTTP_DOWNLOAD
  char *buffer,
  InstanceHandleT handle
#else
  char *,
  InstanceHandleT
#endif /* FEATURE_QTV_HTTP_DOWNLOAD */
  )
{
#ifdef FEATURE_QTV_HTTP_DOWNLOAD
  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }

   ReturnT nReturn = QTV_RETURN_ERROR;

   if (buffer)
   {
      player->pMpeg4Player->freeDownloadBuffer(buffer);
      nReturn = QTV_RETURN_OK;
   }
   LogQtvCmd("FreeDownloadBuffer", "%d%d%d", (int32)nReturn,(int32)buffer, (int32)handle);
   return nReturn;
#else
   return QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif
}
/* ======================================================================
FUNCTION
  QtvPlayer::GetDownloadData

DESCRIPTION
  API invoked by OEM to ask QTV for data

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
char* QtvPlayer::GetEncryptedData(uint32 &size, InstanceHandleT handle)
{

#ifdef FEATURE_QTV_HTTP_DOWNLOAD
   char* retVal;
   QtvPlayer *player = GetPlayer(handle);
   if(numRegisteredQtvPlayers > 1)
   {
     size = 0;
     return NULL;
   }

   retVal = player->pMpeg4Player->getEncryptedData(size);
   LogQtvCmd("GetEncryptedData", "%d%d%d", (int32)retVal, size, (int32)handle);
   return retVal;
#else
   size = 0;
   QTV_USE_ARG1(handle);
   return NULL;
#endif
}

/* ======================================================================
FUNCTION
  QtvPlayer::GetFrameInfo

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::GetFrameInfo(FrameInfoT &frameInfo, InstanceHandleT handle)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::GetFrameInfo");

  QtvPlayer *player = GetPlayer(handle);

  ReturnT nReturn = QTV_RETURN_ERROR;
  frameInfo.RGBBuffer = NULL;
  frameInfo.YUVBuffer = NULL;

  if (player->pMpeg4Player)
  {
    nReturn = player->pMpeg4Player->GetFrameInfo(frameInfo, NULL);
  }
#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
  else if (player->CurrentMIMEType && strcmp(player->CurrentMIMEType, REAL_MEDIA_MIME_TYPE) == 0)
  {
     nReturn = QtvRealPlayer_GetFrameInfo(frameInfo);
  }
#endif
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::GetFrameInfo return %d",nReturn);
  return nReturn;
}

/* ======================================================================
FUNCTION
  QtvPlayer::GetExtFrameInfo

DESCRIPTION
  Extended version of the GetFrameInfo function that retrieves extended
  metadata information in addition to the the decoded frame. In case the
  caller is not interesetd in metedata information, they can simply pass
  NULL for the ppExtFrmInfo argument. This is how the basic GetFrameInfo
  operates.

DEPENDENCIES
  None

RETURN VALUE
  QTV_RETURN_OK is successful, QTV_RETURN_* failure codes otherwise

SIDE EFFECTS
  None

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::GetExtFrameInfo
(
  FrameInfoT &frameInfo,
  void **ppExtFrmInfo,
  InstanceHandleT handle
)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::GetExtFrameInfo");

  QtvPlayer *player = GetPlayer(handle);

  ReturnT nReturn = QTV_RETURN_ERROR;
  frameInfo.RGBBuffer = NULL;
  frameInfo.YUVBuffer = NULL;

  if (player->pMpeg4Player)
  {
    nReturn = player->pMpeg4Player->GetFrameInfo(frameInfo,ppExtFrmInfo);
  }
  LogQtvCmd("GetExtFrameInfo", "%d%d%d", (int32)nReturn, &frameInfo,(int32)handle);
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::GetExtFrameInfo return %d",nReturn);
  return nReturn;
}

/* ======================================================================
FUNCTION
  QtvPlayer::GetClipInfo

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::GetClipInfo(ClipInfoT &clipInfo, InstanceHandleT handle)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::GetClipInfo");

  QtvPlayer *player = GetPlayer(handle);
  ReturnT nReturn = QTV_RETURN_ERROR;

#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
  if (player->CurrentMIMEType && strcmp(player->CurrentMIMEType, REAL_MEDIA_MIME_TYPE) == 0)
  {
     return QtvRealPlayer_GetClipInfo(clipInfo);
  }
#endif

  SetNullClipInfo(clipInfo);

  if (player->pMpeg4Player)
  {
    nReturn = player->pMpeg4Player->GetClipInfo(clipInfo);
  }
  LogQtvCmd("GetClipInfo", "%d%d%d", (int32)nReturn, &clipInfo, (int32)handle);
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, ">>QtvPlayer::GetClipInfo return %d",nReturn);
  return nReturn;
}

/* ======================================================================
FUNCTION
  QtvPlayer::GetServerChallenge

DESCRIPTION
  Method to obtain the value field of WWW-Authenticate header
  received from the server

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  QTV_RETURN_ERROR/QTV_RETURN_OK

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::GetServerChallenge(char * challenge, InstanceHandleT handle)

{
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,">> Qtv get server challenge");

    QtvPlayer *player = GetPlayer(handle);
    if(numRegisteredQtvPlayers > 1)
    {
      return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
    }

    ReturnT nReturn = QTV_RETURN_ERROR;
    if (player->pMpeg4Player)
    {
      nReturn = player->pMpeg4Player->GetServerChallenge(challenge);
    }
  LogQtvCmd("GetServerChallenge", "%d%d%d", (int32)nReturn, challenge, (int32)handle);
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, ">>QtvPlayer::GetServerChallenge return %d",
                            nReturn);
  return nReturn;

}

/* ======================================================================
FUNCTION
  QtvPlayer::QtvAppCanAuthenticate

DESCRIPTION
  Purpose  : Method to record the ability of the QtvApp to provide
             Authorization information, if required
  In/out   : auth:
             TRUE, if Qtv APP can provide authorization info
             FALSE, if Qtv App cannot provide authorization info

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Always QTV_RETURN_OK

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::QtvAppCanAuthenticate(bool auth, InstanceHandleT handle)
{
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,">> Qtv QtvAppCanAuthenticate");
    QtvPlayer *player = GetPlayer(handle);
    if(numRegisteredQtvPlayers > 1)
    {
      return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
    }

    ReturnT nReturn = QTV_RETURN_ERROR;

    if (player->pMpeg4Player)
    {
       player->pMpeg4Player->AppCanAuthenticate(auth);
       nReturn = QTV_RETURN_OK;
    }
    LogQtvCmd("QtvAppCanAuthenticate", "%d%d%d", (int32)nReturn, (int32)auth,(int32)handle);
    return nReturn;
}

/* ======================================================================
FUNCTION
  QtvPlayer::SetQtvUserAuthInfoandRetry

DESCRIPTION
  Purpose  : Provide user authorization info to QtvPlayer and Retry
             the session establishment
  In/out   : Authorization info
  Note     : size of the challenge can be atmost QTV_MAX_CHALLENGE_BYTES
             defined in QtvPlayer.h

             authinfo is passed as is in the value field of outgoing RTSP
             message header "Authorization: "

             If App has informed it's ability to provide auth info, it
             must callback this method. Pass NULL argument for authinfo
             to inform Qtv player close the session

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  QTV_RETURN_ERROR if it fails, else QTV_RETURN_OK

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer:: SetQtvUserAuthInfoandRetry(char * challenge, InstanceHandleT handle)
{

    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,"QtvPlayer::setQtvUserAuthInfo");

    QtvPlayer *player = GetPlayer(handle);
    if(numRegisteredQtvPlayers > 1)
    {
      return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
    }

    ReturnT nReturn = QTV_RETURN_ERROR;
     if (player->pMpeg4Player)
     {
         nReturn = player->pMpeg4Player->RetrySessionWithQtvUserAuthInfo(challenge);
     }
     if(challenge)
     {
     LogQtvCmd("SetQtvUserAuthInfoandRetry", "%d%s%d", (int32)nReturn, challenge, (int32)handle);
     }
     else
     {
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Cannot log Qtv cmd %u", challenge);
     }
     QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "QtvPlayer::setQtvUserAuthInfo return %d",
                             nReturn);
     return nReturn;
}


/* ======================================================================
FUNCTION
  QtvPlayer::RotateVideo

DESCRIPTION
  Posts a rotate message onto the MPEG4 player's task. This function doesn't
  do any notification of the callback itself, it relies on the MPEG4 player
  to do it.

  This function follows the general rule for setting parameters:
  The function is responsible for ensuring the callback is notified if it
  returns success or has no return value. Otherwise, no callback is notified.

DEPENDENCIES
  None.

RETURN VALUE
  If the MPEG4 player is missing or its task cannot be started, return false.
  Otherwise, return true.

SIDE EFFECTS
  If the MPEG4 player task has not been started, this function will start it.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::RotateVideo(
#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
  RotationType direction,
  InstanceHandleT handle
#else
  RotationType,
  InstanceHandleT
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */
  )
{
#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::RotateVideo");

  QtvPlayer *player = GetPlayer(handle);

  ReturnT nReturn = QTV_RETURN_ERROR;

#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
  if (player->CurrentMIMEType && strcmp(player->CurrentMIMEType, REAL_MEDIA_MIME_TYPE) == 0)
  {
     nReturn = QtvRealPlayer_RotateVideo(direction);
  }
  else
#endif
  if (player->pMpeg4Player)
  {
    if (player->pMpeg4Player->StartThread())
    {
      PV_VIDEO_ROTATE_type *pEvent = QCCreateMessage(PV_VIDEO_ROTATE, player->pMpeg4Player);

      if (pEvent)
      {
        pEvent->direction = direction;
        player->pMpeg4Player->PostMessage(pEvent);
        nReturn = QTV_RETURN_OK;
      }
    }
  }
  LogQtvCmd("RotateVideo", "%d%d%d", (int32)nReturn, (int32)direction,(int32)handle);
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::RotateVideo return %d",nReturn);
  return nReturn;
#else
  return QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif
}

/* ======================================================================
FUNCTION
  QtvPlayer::ScaleVideo

DESCRIPTION
  Posts a scale message onto the MPEG4 player's task. This function doesn't
  do any notification of the callback itself, it relies on the MPEG4 player
  to do it.

  This function follows the general rule for setting parameters:
  The function is responsible for ensuring the callback is notified if it
  returns success or has no return value. Otherwise, no callback is notified.

DEPENDENCIES
  None.

RETURN VALUE
  If the MPEG4 player is missing or its task cannot be started, return false.
  Otherwise, return true.

SIDE EFFECTS
  If the MPEG4 player task has not been started, this function will start it.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::ScaleVideo(
#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
  ScalingType ScaleFactor,
  InstanceHandleT handle
#else
  ScalingType,
  InstanceHandleT
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */
  )
{
#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::ScaleVideo");

  QtvPlayer *player = GetPlayer(handle);

  ReturnT nReturn = QTV_RETURN_ERROR;

  if (player != NULL)
  {
#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
    if (player->CurrentMIMEType && strcmp(player->CurrentMIMEType, REAL_MEDIA_MIME_TYPE) == 0)
    {
       nReturn = QtvRealPlayer_ScaleVideo(ScaleFactor);
    }
    else
#endif
    if (player->pMpeg4Player)
    {
      if (player->pMpeg4Player->StartThread())
      {
        PV_VIDEO_SCALE_type *pEvent = QCCreateMessage(PV_VIDEO_SCALE, player->pMpeg4Player);

        if (pEvent)
        {
          pEvent->scaling = ScaleFactor;
          pEvent->outFrameWidth = 0;
          pEvent->outFrameHeight = 0;
          player->pMpeg4Player->PostMessage(pEvent);
          nReturn = QTV_RETURN_OK;
        }
      }
    }
  }
  LogQtvCmd("ScaleVideo", "%d%d%d", (int32)nReturn, (int32)ScaleFactor,(int32)handle);

  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::ScaleVideo return %d",nReturn);
  return nReturn;
#else
  return QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif
}

/* ======================================================================
  FUNCTION
  QtvPlayer::ScaleVideo

DESCRIPTION
  Perform arbitrary video scaling from 0.3x - 1,9x with DSP xScalar. It posts a
  scale message onto the MPEG4 player's task. This function doesn't
  do any notification of the callback itself, it relies on the MPEG4 player
  to do it.
  If the requested output dimensions fall outside the supported scaling range,
  MPEG4 Player shall notify an error.

DEPENDENCIES
  None.

RETURN VALUE
  If the MPEG4 player is missing or its task cannot be started, return false.
  Otherwise, return true.

SIDE EFFECTS
  If the MPEG4 player task has not been started, this function will start it.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::ScaleVideo(
#ifdef FEATURE_QTV_XSCALE_VIDEO
  uint32 outWidth,
                                         uint32 outHeight,
  InstanceHandleT handle
#else
  uint32,
  uint32,
  InstanceHandleT
#endif //FEATURE_QTV_XSCALE_VIDEO
  )
{
#ifdef FEATURE_QTV_XSCALE_VIDEO
  QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::ScaleVideo outWidth=%d outHeight=%d",
                outWidth, outHeight);

  QtvPlayer *player = GetPlayer(handle);
  if(numQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }

  ReturnT nReturn = QTV_RETURN_ERROR;

#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
#if defined(FEATURE_QTV_XSCALE_VIDEO) && defined(FEATURE_REAL_PLAYER_ENABLE_DSP_ACCELERATED)
  if (player && player->CurrentMIMEType && strcmp(player->CurrentMIMEType, REAL_MEDIA_MIME_TYPE) == 0)
  {
    return QtvRealPlayer_ScaleVideo(outWidth,outHeight);
  }
#endif
#endif /* FEATURE_REAL_PLAYER_USE_QTV_API */

  if (player && player->pMpeg4Player)
  {
    if (player->pMpeg4Player->StartThread())
    {
      PV_VIDEO_SCALE_type *pEvent = QCCreateMessage(PV_VIDEO_SCALE, player->pMpeg4Player);

      if (pEvent)
      {
        pEvent->scaling = SCALE_NONE;
        pEvent->outFrameWidth = outWidth;
        pEvent->outFrameHeight = outHeight;
        player->pMpeg4Player->PostMessage(pEvent);
        nReturn = QTV_RETURN_OK;
      }
    }
  }


  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::ScaleVideo return %d",nReturn);
  return nReturn;
#else
  return QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif //FEATURE_QTV_XSCALE_VIDEO

}

/* ======================================================================
  FUNCTION
    QtvPlayer::SetAudOverride

  DESCRIPTION
    Thorough, meaningful description of what this function does.

  DEPENDENCIES
    List any dependencies for this function, global variables, state,
    resource availability, etc.

  RETURN VALUE
    Enumerate possible return values

  SIDE EFFECTS
    Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::SetAudOverride(AudOverrideType audformat, InstanceHandleT handle)
{
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::SetAudOver %d",audformat);

  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }

  ReturnT nReturn = QTV_RETURN_ERROR;

    if (player->pMpeg4Player && (audformat>AUD_OVER_NONE) && (audformat<AUD_OVER_LAST) )
    {
        if (player->pMpeg4Player->StartThread())
        {
            PV_SET_AUDIOOVER_type *pEvent = QCCreateMessage(PV_SET_AUDIOOVER, player->pMpeg4Player);/*lint!e641 */

            if (pEvent)
            {
                          pEvent->audOverrideFormat = (int)audformat;
                          player->pMpeg4Player->PostMessage(pEvent);
                          nReturn = QTV_RETURN_OK;
            }
         }
    }
  LogQtvCmd("SetAudOverride", "%d%d%d", (int32)nReturn, (int32)audformat,(int32)handle);
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::SetAudOverride return %d",nReturn);
    return nReturn;
}

/* ======================================================================
FUNCTION
  QtvPlayer::GetPlayerState

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::GetPlayerState(PlayerStateRecordT &state, InstanceHandleT handle)
{

  QtvPlayer *player = GetPlayer(handle);
  ReturnT nReturn = QTV_RETURN_ERROR;
  SetNullPlayerState(state);

#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
  if (player && player->CurrentMIMEType && strcmp(player->CurrentMIMEType, REAL_MEDIA_MIME_TYPE) == 0)
  {
    return QtvRealPlayer_GetPlayerState(state);
  }
#endif

  if (player && player->pMpeg4Player)
  {
    nReturn = player->pMpeg4Player->GetPlayerStateExternal(state);
  }
  LogQtvCmd("GetPlayerState", "%d%d%d", (int32)nReturn, &state, (int32)handle);
  return nReturn;
}
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
/* ======================================================================
FUNCTION
  QtvPlayer::GetDownloadTime

DESCRIPTION
  Gets the playback time available from the downloading file

DEPENDENCIES
  None

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  None

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::GetDownloadTime(PlayerStateRecordT &state, InstanceHandleT handle)
{

  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }

  ReturnT nReturn = QTV_RETURN_ERROR;

  if (player->pMpeg4Player)
  {
    nReturn = player->pMpeg4Player->GetDownloadTime(state);
  }
  LogQtvCmd("GetDownloadTime", "%d%d%d", (int32)nReturn, &state, (int32)handle);
  return nReturn;
}
#endif

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
/* ======================================================================
FUNCTION
QtvPlayer::getDownloadClipType

DESCRIPTION
returns the playback mode for a FastTrack clip

DEPENDENCIES
None

RETURN VALUE
Enumerate possible return values

SIDE EFFECTS
None

========================================================================== */
QtvPlayer::DownloadPlaybackControlT QtvPlayer::getDownloadClipType(InstanceHandleT handle)
{

  QtvPlayer::DownloadPlaybackControlT nReturn = NONE;
  QtvPlayer *player = GetPlayer(handle);

  if ((numRegisteredQtvPlayers == 1) && player->pMpeg4Player)
  {
    nReturn = player->pMpeg4Player->getDownloadClipType();
  }
  LogQtvCmd("getDownloadClipType", "%d%d", (int32)nReturn, (int32)handle);
  return nReturn;
}
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */
/* ======================================================================
FUNCTION
  QtvPlayer::SetFileSaveOption

DESCRIPTION
  Sets the the path for file downloaded using PVX clip

DEPENDENCIES
  None

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  None

========================================================================== */
bool QtvPlayer::SetFileSaveOption(char *pvxDirPath, InstanceHandleT handle)
{
#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  bool breturn = false;
  QtvPlayer *player = GetPlayer(handle);

  if ((numRegisteredQtvPlayers == 1) && player->pMpeg4Player)
  {
#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
    (void)player->pMpeg4Player->SetPvxSaveOption(pvxDirPath);
#endif
    breturn = true;
  }
  if(pvxDirPath)
  {
  LogQtvCmd("SetFileSaveOption", "%d%s%d", (int8)breturn, pvxDirPath, (int32)handle);
  }
  else
  {
     QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Cannot log Qtv cmd %u", pvxDirPath);
  }
  return breturn;
#else
  if(pvxDirPath)
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
      "Directory Path is %d", pvxDirPath);
  }
  QTV_USE_ARG1(handle);
  return false;
#endif
}

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
/* ======================================================================
FUNCTION
  QtvPlayer::SetPvxPreroll

DESCRIPTION
  Sets the preroll for a  pvx clip

DEPENDENCIES
  None

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  None

========================================================================== */
bool QtvPlayer::SetPvxPreroll(int pvxPreroll, InstanceHandleT handle)
{

  bool breturn = false;
  QtvPlayer *player = GetPlayer(handle);

  if ((numRegisteredQtvPlayers == 1) && player->pMpeg4Player)
  {
    (void)player->pMpeg4Player->SetPvxPreroll(pvxPreroll);
    breturn = true;
  }
  LogQtvCmd("SetPvxPreroll", "%d%d%d", (int32)breturn, pvxPreroll,(int32)handle);
  return breturn;
}
#endif
/* ======================================================================
FUNCTION
  QtvPlayer::GetAudioVideoStatistics

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::GetAudioVideoStatistics(AudioVideoStatisticsT &statistics,
                                                      InstanceHandleT handle)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "QtvPlayer::GetAudioVideoStatistics");

  QtvPlayer *player = GetPlayer(handle);

  ReturnT nReturn = QtvPlayer::QTV_RETURN_ERROR;
  SetNullAudioVideoStatistics(statistics);

  if (player->pMpeg4Player)
  {
    nReturn = player->pMpeg4Player->GetAudioVideoStatistics(statistics);
  }
  LogQtvCmd("GetAudioVideoStatistics", "%d%d%d", (int32)nReturn, &statistics, (int32)handle);
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::GetAudioVideoStatistics return %d",nReturn);
  return nReturn;
}

/* ======================================================================
FUNCTION
  QtvPlayer::GetRTPStatistics

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::GetRTPStatistics(RTPStatisticsT &statistics,
                                               InstanceHandleT handle)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "QtvPlayer::GetRTPStatistics");

  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }

  ReturnT nReturn = QtvPlayer::QTV_RETURN_ERROR;
  SetNullRTPStatistics(statistics);

  if (player->pMpeg4Player)
  {
    nReturn = player->pMpeg4Player->GetRTPStatistics(statistics);
  }
  LogQtvCmd("GetRTPStatistics", "%d%d%d", (int32)nReturn, &statistics, (int32)handle);
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::GetRTPStatistics return %d",nReturn);
  return nReturn;
}


/* ======================================================================
FUNCTION
  QtvPlayer::GetAudioCodecType

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::CodecType QtvPlayer::GetAudioCodecType(InstanceHandleT handle)
{
  CodecType codecType = QTV_UNKNOWN_CODEC;

  QtvPlayer *player = GetPlayer(handle);
#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
  if (player->CurrentMIMEType && strcmp(player->CurrentMIMEType, REAL_MEDIA_MIME_TYPE) == 0)
  {
     return QtvRealPlayer_GetAudioCodec();
  }
#endif

  if (player->pMpeg4Player)
  {
    switch(player->pMpeg4Player->GetAudioCodecType())
    {
      case Media::EVRC_CODEC:
        codecType = QTV_EVRC_CODEC;
        break;
      case Media::QCELP_CODEC:
        codecType = QTV_QCELP_CODEC;
        break;
      case Media::AAC_CODEC:
        codecType = QTV_AAC_CODEC;
        break;
#ifdef FEATURE_QTV_BSAC
#error code not present
#endif /* FEATURE_QTV_BSAC */
      case Media::GSM_AMR_CODEC:
        codecType = QTV_GSM_AMR_CODEC;
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
      case Media::MPEG4_CODEC:
        codecType = QTV_MPEG4_CODEC;
        break;
      case Media::H263_CODEC:
        codecType = QTV_H263_CODEC;
        break;
      case Media::STILL_IMAGE_CODEC:
	  case Media::STILL_IMAGE_H263_CODEC:
        codecType = QTV_STILL_IMAGE_CODEC;
        break;
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
      case Media::TIMED_TEXT_CODEC:
        codecType = QTV_TIMED_TEXT_CODEC;
        break;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
#ifdef FEATURE_MP4_MP3
      case Media::MP3_CODEC:
        codecType = QTV_MP3_CODEC;
        break;
#endif /* FEATURE_MP4_MP3 */
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
      case Media::NONMP4_MP3_CODEC:
        codecType = QTV_NONMP4_MP3_CODEC;
        break;
      case Media::QCP_CODEC:
        codecType = QTV_QCP_CODEC;
        break;
      case Media::NONMP4_AMR_CODEC:
        codecType = QTV_NONMP4_AMR_CODEC;
        break;
      case Media::NONMP4_AAC_CODEC:
        codecType = QTV_NONMP4_AAC_CODEC;
        break;
      case Media::MIDI_CODEC:
        codecType = QTV_MIDI_CODEC;
        break;
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */

#ifdef FEATURE_QTV_WINDOWS_MEDIA
      case Media::WMA_CODEC:
        codecType = QTV_WMA_CODEC;
        break;
#endif
#if defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER)
        case Media::WMA_PRO_CODEC:
          codecType = QTV_WMA_PRO_CODEC;
          break;
        case Media::WMA_PRO_PLUS_CODEC:
          codecType = QTV_WMA_PRO_PLUS_CODEC;
          break;
#endif /* defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER) */

#ifdef FEATURE_QTV_IN_CALL_VIDEO
#error code not present
#endif /* FEATURE_QTV_IN_CALL_VIDEO */
#ifdef FEATURE_QTV_AVI_AC3
#error code not present
#endif /* FEATURE_QTV_AVI_AC3 */
#ifdef FEATURE_QTV_PCM
#error code not present
#endif /* FEATURE_QTV_PCM */
      case Media::UNKNOWN_CODEC:
      default:
        break;
    }
  LogQtvCmd("GetAudioCodecType", "%d%d", (int32)codecType,(int32)handle);
  }
  return codecType;
}

/* ======================================================================
FUNCTION
  QtvPlayer::GetVideoCodecType

DESCRIPTION
  Gets the media codec type of the video track being played and maps it to the
  appropriate Qtv codec type

DEPENDENCIES
  None

RETURN VALUE
  QtvPlayer::CodecType

SIDE EFFECTS
  None

========================================================================== */
QtvPlayer::CodecType QtvPlayer::GetVideoCodecType(InstanceHandleT handle)
{
  CodecType codecType = QTV_UNKNOWN_CODEC;
  QtvPlayer *player = GetPlayer(handle);

#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
  if (player->CurrentMIMEType && strcmp(player->CurrentMIMEType, REAL_MEDIA_MIME_TYPE) == 0)
  {
     return QtvRealPlayer_GetVideoCodec();
  }
#endif

  if (player->pMpeg4Player)
  {
    switch(player->pMpeg4Player->GetVideoCodecType())
    {
      case Media::MPEG4_CODEC:
        codecType = QTV_MPEG4_CODEC;
        break;

      case Media::H263_CODEC:
        codecType = QTV_H263_CODEC;
        break;

#ifdef FEATURE_DIVX_311_ENABLE
      case Media::DIVX311_CODEC:
        codecType = QTV_DIVX311_CODEC;
        break;
#endif

      case Media::H264_CODEC:
        codecType = QTV_H264_CODEC;
        break;

      case Media::STILL_IMAGE_CODEC:
	  case Media::STILL_IMAGE_H263_CODEC:
        codecType = QTV_STILL_IMAGE_CODEC;
        break;

#ifdef FEATURE_QTV_WINDOWS_MEDIA

     case Media::WMV1_CODEC:
         codecType = QTV_WMV1_CODEC;
         break;

     case Media::WMV2_CODEC:
          codecType = QTV_WMV2_CODEC;
          break;

      case Media::WMV3_CODEC:
          codecType = QTV_WMV3_CODEC;
       break;
#endif

      case Media::UNKNOWN_CODEC:

      default:
        break;
    }
  LogQtvCmd("GetVideoCodecType", "%d%d", (int32)codecType,(int32)handle);
  }

  return codecType;
}

/* ======================================================================
FUNCTION
  QtvPlayer::Get3GPPTimedTextInfo

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
boolean QtvPlayer::Get3GPPTimedTextInfo(ThreeGPPTimedTextInfo *pInfo,
                                        InstanceHandleT handle)
{
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  QtvPlayer *player = GetPlayer(handle);
  if((numRegisteredQtvPlayers > 1) || !player)
  {
    return FALSE;
  }

  if(pInfo == NULL)
    return FALSE;

  pInfo->dwTextDataSize         = TimedText::GetTextBufSize();
  pInfo->dwDisplayFlags         = TimedText::GetDisplayFlags();
  pInfo->dwHorzJustification    = TimedText::GetHorzJustification();
  pInfo->dwVertJustification    = TimedText::GetVertJustification();
  pInfo->dwBoxTop               = TimedText::GetBoxTop();
  pInfo->dwBoxLeft              = TimedText::GetBoxLeft();
  pInfo->dwBoxBottom            = TimedText::GetBoxBottom();
  pInfo->dwBoxRight             = TimedText::GetBoxRight();
  pInfo->dwStartChar            = TimedText::GetStartChar();
  pInfo->dwEndChar              = TimedText::GetEndChar();
  pInfo->dwFontID               = TimedText::GetFontID();
  pInfo->dwFontStyleFlags       = TimedText::GetFontStyleFlags();
  pInfo->dwFontSize             = TimedText::GetFontSize();
  pInfo->dwFontListSize         = TimedText::GetFontListSize();
  pInfo->dwDuration             = TimedText::GetSampleDuration();
  pInfo->dwBeginTime            = TimedText::GetBeginTime();
  pInfo->dwSampleModifiersSize  = TimedText::GetSampleModifierSize();

  if(pInfo->dwTextDataSize)
  {
    LogQtvCmd("Get3GPPTimedTextInfo", "%d%d%d", (int32)TRUE, pInfo, (int32)handle);
    return TRUE;
 }

  return FALSE;
#else
  return FALSE;
#endif
}

/* ======================================================================
FUNCTION
  QtvPlayer::Get3GPPFontInfoAt

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
boolean QtvPlayer::Get3GPPFontInfoAt(uint16 * pFontID, char * pNameBuf, int nBufSize, uint16 Idx,
                                     InstanceHandleT handle)
{
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  QtvPlayer *player = GetPlayer(handle);
  if((numRegisteredQtvPlayers > 1) || !player)
  {
    return FALSE;
  }

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::GetFontInfoAt\n");
  FontRecord * f_record = (FontRecord *)TimedText::GetFontRecordAt(Idx);
  if(f_record && pNameBuf && nBufSize)
  {
    *pFontID = f_record->getFontID();
    // Changing the srncpy with more reliable StrcpyN
    (void)ZUtils::StrcpyN(pNameBuf, (char *)(f_record->getFontName()), nBufSize);
    LogQtvCmd("Get3GPPFontInfoAt", "%d%d%d%d%d%d", (int32)TRUE,(int32)pFontID, (int32)pNameBuf, nBufSize, Idx,(int32)handle);
    return TRUE;
  }
  return FALSE;
#else
  return FALSE;
#endif
}

/* ======================================================================
FUNCTION
  QtvPlayer::Get3GPPTimedTextData

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
boolean QtvPlayer::Get3GPPTimedTextData(uint8 * pDataBuf, int nBufSize,
                                        InstanceHandleT handle)
{
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  QtvPlayer *player = GetPlayer(handle);
  if((numRegisteredQtvPlayers > 1) || !player)
  {
    return false;
  }

  uint8 *pData = (uint8 *)TimedText::GetText();
  int nTextDataSize = (int)TimedText::GetTextBufSize();
  if(pData && nTextDataSize && pDataBuf)
  {
    memcpy(pDataBuf, pData, MIN(nBufSize, nTextDataSize));
    LogQtvCmd("GetRTPStatistics", "%d%d%d", (int32)true,(int32) pDataBuf, nBufSize,(int32)handle);
    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
    "QtvPlayer::Get3GPPTimedTextData: Text Data->%s Text Size-> %d",pDataBuf,nBufSize);
    return true;
  }
  return false;
#else
  return false;
#endif
}

/* ======================================================================
FUNCTION
  QtvPlayer::Get3GPPTimedTextBGRGB

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
boolean QtvPlayer::Get3GPPTimedTextBGRGB(uint8 * pRGBBuf, int nBufSize,
                                         InstanceHandleT handle)
{
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  QtvPlayer *player = GetPlayer(handle);
  if((numRegisteredQtvPlayers > 1) || !player)
  {
    return false;
  }

  uint8 *pBackgroundColorRGBA = (uint8*)TimedText::GetBackgroundColorRGBA();
  if(pBackgroundColorRGBA && pRGBBuf)
  {
    memcpy(pRGBBuf, pBackgroundColorRGBA, MIN(nBufSize, PVTEXT_MAX_TEXT_COLORS));
    LogQtvCmd("Get3GPPTimedTextBGRGB", "%d%d%d", (int32)true,(int32)pRGBBuf, nBufSize,(int32)handle);
    return true;
  }
  return false;
#else
  return false;
#endif
}

/* ======================================================================
FUNCTION
  QtvPlayer::Get3GPPTimedTextTxtRGB

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
boolean QtvPlayer::Get3GPPTimedTextTxtRGB(uint8 * pRGBBuf, int nBufSize,
                                          InstanceHandleT handle)
{
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  QtvPlayer *player = GetPlayer(handle);
  if((numRegisteredQtvPlayers > 1) || !player)
  {
    return false;
  }

  uint8 *pTextColorRGBA = (uint8*)TimedText::GetTextColorRGBA();
  if(pTextColorRGBA && pRGBBuf)
  {
    memcpy(pRGBBuf, pTextColorRGBA, MIN(nBufSize, PVTEXT_MAX_TEXT_COLORS));
    LogQtvCmd("Get3GPPTimedTextTxtRGB", "%d%d%d", (int32)true, (int32)pRGBBuf, nBufSize,(int32)handle);
    return true;
  }
  return false;
#else
  return false;
#endif
}

/* ======================================================================
FUNCTION
  QtvPlayer::GetNextTelopElement

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::GetNextTelopElement(QtvPlayer::TelopElementT &telopElement,
                                                  InstanceHandleT handle)
{
  ReturnT nReturn = QTV_RETURN_ERROR;
#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
  QtvPlayer *player = GetPlayer(handle);
  nReturn = QTV_RETURN_ERROR;
  if(!player)
  {
    return nReturn;
  }
  else if(numRegisteredQtvPlayers > 1)
  {
    nReturn = QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }
  else
  {
    nReturn = TimedText::GetNextTelopElement(telopElement);
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::GetNextTelopElement");
    LogQtvCmd("GetNextTelopElement", "%d%d%d", (int32)nReturn, &telopElement, (int32)handle);
  }
#else
  nReturn = QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif
  return nReturn;
}

/* ======================================================================
FUNCTION
  QtvPlayer::GetTelopSubString

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool QtvPlayer::GetTelopSubString(TelopSubStringT* pUserSubStr, int32 index,
                                  InstanceHandleT handle)
{
#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
  QtvPlayer *player = GetPlayer(handle);
  if((numRegisteredQtvPlayers > 1) || !player)
  {
    return false;
  }

  const TelopSubStringT* pSubStr = TimedText::GetTelopSubString(index);
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::GetTelopSubstring");
  if(pSubStr && pUserSubStr)
  {
    memcpy(pUserSubStr, pSubStr, sizeof(TelopSubStringT));
    LogQtvCmd("GetTelopSubString", "%d%d%d%d", (int32)true,pUserSubStr, index,(int32)handle);
    return true;
  }
  return false;
#else
  return false;
#endif
}

/* ======================================================================
FUNCTION:
  QtvPlayer::GetTotalRxDataRate

DESCRIPTION:
  Gets the total data rate at the reception from the MediaBuffer.

INPUT/OUTPUT PARAMETERS:
  None

RETURN VALUE:
  Data Rate at the reception

SIDE EFFECTS:
  None.
======================================================================*/
uint32 QtvPlayer::GetTotalRxDataRate(InstanceHandleT handle)
{
  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }

  if(player->pMpeg4Player && player->pMpeg4Player->IsThreadActive())
  {
    uint32 retVal = player->pMpeg4Player->GetTotalRxDataRate();
    LogQtvCmd("GetTotalRxDataRate", "%d%d", retVal, (int32)handle);
    return retVal;
  }
  else
  {
    return QTV_RETURN_ERROR;
  }
}

/* ======================================================================
FUNCTION
  QtvPlayer::GetGenericTextData

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::GetGenericTextData(uint8** pTextFormat, uint8* pDataBuf, uint32* pBufSize, InstanceHandleT handle)
{
  ReturnT nReturn = QTV_RETURN_ERROR;
  uint8 *pData = NULL;
  uint32 nTextDataSize = 0;
  uint32 nInputBufSize = 0;

  QtvPlayer *player = GetPlayer(handle);
  if((numRegisteredQtvPlayers > 1) || !player)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }

  if(pBufSize && pTextFormat)
  {
    nTextDataSize = (uint32)TimedText::GetTextBufSize();
    pData = (uint8 *)TimedText::GetText();
    nInputBufSize = *pBufSize;                                        /* copy the "pDataBuf" size */
    *pTextFormat   = (uint8 *)TimedText::GetTextFormatString();       /* copy the Text Format String */

    if(pDataBuf)
    {
      if(pData && nTextDataSize)
      {
        memcpy(pDataBuf, pData, MIN(nInputBufSize, nTextDataSize));
      }
      *pBufSize = MIN(nInputBufSize, nTextDataSize);                  /* copy the output text data size */
      if(nInputBufSize < nTextDataSize)
      {
        nReturn = QTV_RETURN_BUFFER_UNDERRUN;
      }
      else
      {
        QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
    "QtvPlayer::GetGenericTextData: Text Data->%s Text Size-> %d",pDataBuf,*pBufSize);
        nReturn = QTV_RETURN_OK;
      }
   }
   else
   {
     /* Special case, copy only text data size */
     *pBufSize = nTextDataSize;
     nReturn = QTV_RETURN_OK;
   }

  }
  LogQtvCmd("GetGenericTextData", "%d%d%d%d%d", (int32)nReturn, pTextFormat, pDataBuf, pBufSize,(int32)handle);
  return nReturn;
}

/* ======================================================================
FUNCTION
  QtvPlayer::SetUserAgentParameters

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::SetUserAgentParameters(const char *man,
                                                     const char *ctn,
                                                     InstanceHandleT handle)
{
  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }

  ReturnT nReturn = QTV_RETURN_OK;

  if (player->m_UserAgentParam_man != man)
  {
    std_strlcpy(player->m_UserAgentParam_man, man, QTV_MAX_USER_AGENT_MAN_BYTES+1);
  }
  if (player->m_UserAgentParam_ctn != ctn)
  {
    std_strlcpy(player->m_UserAgentParam_ctn, ctn, QTV_MAX_USER_AGENT_CTN_BYTES+1);
  }
  player->m_UserAgentParam_params[0] = '\0';

#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
  if (player->CurrentMIMEType && strcmp(player->CurrentMIMEType, REAL_MEDIA_MIME_TYPE) == 0)
  {
    nReturn = QtvRealPlayer_SetUserAgentParameters(man, ctn);
  }
  else
#endif

  if (player->pMpeg4Player)
  {
    nReturn = player->pMpeg4Player->SetUserAgentParameters(man, ctn);
  }
  if(man && ctn)
  {
  LogQtvCmd("SetUserAgentParameters", "%d%s%s%d", (int32)nReturn, man,ctn,(int32)handle);
  }
  else
  {
     QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Cannot log Qtv cmd %u,%u", man, ctn);
  }
  return nReturn;
}

/* ======================================================================
FUNCTION
  QtvPlayer::SetUserAgent

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::SetUserAgent(const char * userAgentName,
                                           InstanceHandleT handle)
{
  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }

  ReturnT nReturn = QTV_RETURN_OK;

  if (player->m_UserAgentStr != userAgentName)
  {
    std_strlcpy(player->m_UserAgentStr, userAgentName, QTV_MAX_USER_AGENT_STR_BYTES+1);
  }

#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
  if (player->CurrentMIMEType && strcmp(player->CurrentMIMEType, REAL_MEDIA_MIME_TYPE) == 0)
  {
    nReturn = QtvRealPlayer_SetUserAgent(userAgentName);
  }
  else
#endif

  if (player->pMpeg4Player)
  {
    nReturn = player->pMpeg4Player->SetUserAgent(userAgentName);
  }
  if(userAgentName)
  {
  LogQtvCmd("SetUserAgent", "%d%s%d", (int32)nReturn, userAgentName, (int32)handle);
  }
  else
  {
     QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Cannot log Qtv cmd %u", userAgentName);
  }
  return nReturn;
}

/* ======================================================================
FUNCTION
  QtvPlayer::SetUserAgentParameters

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::SetUserAgentParameters(const char* params,
                                                     InstanceHandleT handle)
{
  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }

  ReturnT nReturn = QTV_RETURN_OK;

  if (player->m_UserAgentParam_params != params)
  {
    std_strlcpy(player->m_UserAgentParam_params, params, QTV_MAX_USER_AGENT_STR_BYTES+1);
  }

  player->m_UserAgentParam_man[0] = '\0';
  player->m_UserAgentParam_ctn[0] = '\0';

#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
  if (player->CurrentMIMEType && strcmp(player->CurrentMIMEType, REAL_MEDIA_MIME_TYPE) == 0)
  {
    nReturn = QtvRealPlayer_SetUserAgentParameters(params);
  }
  else
#endif
  if (player->pMpeg4Player)
  {
    nReturn = player->pMpeg4Player->SetUserAgentParameters(params);
  }
  if(params)
  {
  LogQtvCmd("SetUserAgentParameters", "%d%s%d", (int32)nReturn, params, (int32)handle);
  }
  else
  {
     QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Cannot log Qtv cmd %u", params);
  }
  return nReturn;
}

/*Function used internally to register a QtvPlayer inst within the
QtvInstances[] lookup array*/

QtvPlayer::ReturnT QtvPlayer::RegisterQTVInstance( InstanceHandleT handle,
                                 InstancePriorityT priority )
{
  return RegisterInst(handle,priority);
}

/* ======================================================================
FUNCTION
  QtvPlayer::SetDataPortRange

DESCRIPTION
     At compile/build time, a range of ports is assigned for
   streaming use.  Because not all vendors will like our assignments,
   we give them the chance to modify the range.  The changed range takes
   effect for the next streaming attempt.
     Operation is deferred to an external method.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void QtvPlayer::SetDataPortRange(int beginPort, int endPort)
{
  QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
               "QtvPlayer::beginPort %d endPort %d", beginPort, endPort );
  NetBase::setDataPortRange((uint16) beginPort, (uint16) endPort);
   LogQtvCmd("SetDataPortRange", "%d%d%d", 0, beginPort, endPort);
}

////////////////////////////////////////////////////////////////////
//
//  Private functions
//
////////////////////////////////////////////////////////////////////

/* ======================================================================
FUNCTION
  SetNullPlayerState

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void SetNullPlayerState(QtvPlayer::PlayerStateRecordT &s)
{
  memset( &s, 0, sizeof(s) );
}

/* ======================================================================
FUNCTION
  SetNullAudioVideoStatistics

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void SetNullAudioVideoStatistics(QtvPlayer::AudioVideoStatisticsT &s)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "QtvPlayer::SetNullAudioVideoStatisticsEx");

  memset( &s, 0, sizeof(s) );
}

/* ======================================================================
FUNCTION
  SetNullRTPStatistics

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void SetNullRTPStatistics(QtvPlayer::RTPStatisticsT &s)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "QtvPlayer::SetNullRTPStatistics" );

  memset(&s, 0, sizeof(s));
}

/* ======================================================================
FUNCTION
  SetNullURNType

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void SetNullURNType(QtvPlayer::URNTypeT &t)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "QtvPlayer::SetNullURNType" );

  t=QtvPlayer::URN_RTSP;
}

/* ======================================================================
FUNCTION
  SetNullURLType

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void SetNullURLType(QtvPlayer::URLTypeT &t)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "QtvPlayer::SetNullURLType" );

  t=QtvPlayer::URL_STREAM;
}

/* ======================================================================
FUNCTION
  SetNullClipInfo

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void SetNullClipInfo(QtvPlayer::ClipInfoT &i)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "QtvPlayer::SetNullClipInfo" );

  /* set everything to zero */
  memset(&i, 0, sizeof(QtvPlayer::ClipInfoT));

  i.streamtype = QtvPlayer::MEDIA_NON_STREAMING;
  i.MediaType=QtvPlayer::MEDIA_AUDIO_ONLY;
  i.RepositioningAllowed=true;    /* by default repositioning is allowed */
  i.VideoOnlyPlaybackAllowed=false;
  i.AudioOnlyPlaybackAllowed=false;
#ifdef FEATURE_QTV_PDCF
  i.encryptionType = QtvPlayer::ENCRYPT_NONE;
#endif /* FEATURE_QTV_PDCF */
  for (int j = 0; j < QTV_MAX_MEDIA_TRACKS; j++)
  {
    i.TrackTypes[j] = QtvPlayer::TRACK_UNKNOWN;
  }
}


/* ======================================================================
FUNCTION
  QtvPlayer::GetFileExtension

DESCRIPTION
   Returns the File extension in lowercase letters

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
#define MAX_EXTENSION_LENGTH 25
const char *GetFileExtension(const char *file)
{
  char *pos;
  static char extension[MAX_EXTENSION_LENGTH + 1];
  extension[0] = '\0';

  pos = (char *)strrchr(file, '?');

  if (pos)
  {
    for (;pos && (pos>file); pos--)
    {
      if (*pos == '.')
      {
        break;
      }
    }
    //if we could not find '.'
    if (pos == file)
    {
      pos = NULL;
    }
  }
  else
  {
    pos = (char *)strrchr(file, '.');
  }

  if (pos)
  {
    int len;
    int i;
    pos++; // skip the '.'
    len = strlen(pos) + 1;
    for (i = 0; i < MIN(len, MAX_EXTENSION_LENGTH); i++)
    {
      extension[i] = tolower(pos[i]);
    }
    extension[MAX_EXTENSION_LENGTH] = '\0';

    // RTSP URNs from .RAM files can contain extra parameters at the end.
    // Search for the parameter start char, and end the line there is it exists.
    pos = strchr(extension, '?');
    if (pos)
    {
       *pos = '\0';
    }

    // trim any whitespace after the extension
    pos = extension;
    do
    {
       if (*pos == ' ' || *pos == '\t')
       {
          *pos = '\0';
       }
       else
       {
          pos++;
       }
    }
    while (*pos != '\0');

    // set the return value
    pos = extension;
  }
  return pos;
}

/* ======================================================================
FUNCTION
  QtvPlayer::GetMediaType

DESCRIPTION
   Returns the MIME type based strictly upon the file extension

========================================================================== */
const char *QtvPlayer::GetMediaType(
#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
  const char *file
#else
  const char *file
#endif /* FEATURE_REAL_PLAYER_USE_QTV_API */
  )
{
  const char * retVal = QTV_PLAYER_MIME_TYPE;
#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
  const char *ext = GetFileExtension(file);
  if (ext)
  {
    if ((std_strcmp(ext, REAL_MEDIA_EXT) == 0) ||
        (std_strcmp(ext, REAL_AUDIO_EXT) == 0) ||
        (std_strcmp(ext, REAL_VIDEO_EXT) == 0))
    {
      retVal = REAL_MEDIA_MIME_TYPE;
    }
    else if ((ZUtils::StrncmpI(ext, SDP_FILE_EXT,3) == true) && 
                    ((ZUtils::StrncmpI(file,"rtsp://", 7) != true) ||
                     (ZUtils::StrncmpI(file,"rtspt://", 8) != true)))
    {
      retVal = QTVMediaSDP_GetMIMEType(file);
    }
    // Use the RAM file player to handle URLs as well, so Real Media
    // content can be played from .url files.
    else if (strcmp(ext, RAM_FILE_EXT) == 0)
    {
      if (std_strncmp(file,"rtsp://", 7) == 0 || 
           std_strncmp(file,"rtspt://", 8) == 0 ||
           std_strncmp(file,"http://", 7) == 0)
      {
         // The Qtv and RAM file players are not capable of handing .ram files
         // located on servers, so play them using the Real Player.
        retVal = REAL_MEDIA_MIME_TYPE;
      }
      else if (QtvRAMPlayer_IsRmfContent(file))
      {
        //rm local fileplayback content is in
        //.ram file, so act accordingly
        retVal = REAL_MEDIA_MIME_TYPE;
      }
      else
    {
      retVal = RAM_FILE_MIME_TYPE;
    }
    }
    else if (strcmp(ext, URL_FILE_EXT) == 0)
    {
      if (strncmp(file,"rtsp://", 7) == 0 || 
           strncmp(file,"rtspt://", 8) == 0 || 
           strncmp(file,"http://", 7) == 0)
      {
         // The Real Player is not capable of handing .url files
         // located on servers, so play them using the Qtv Player.
        retVal = QTV_PLAYER_MIME_TYPE;
      }
      else
      {
        retVal = RAM_FILE_MIME_TYPE;
      }
    }
    if(retVal && file)
    {
    LogQtvCmd("GetMediaType", "%s%s", retVal, file);
    }
    else
    {
       QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Cannot log Qtv cmd %u,%u", retVal, file);
    }
    return retVal;
  }
  else
#else
  const char *ext = GetFileExtension(file);
  if (ext)
  {
    if ((std_strcmp(ext, REAL_MEDIA_EXT) == 0) || 
        (std_strcmp(ext, REAL_AUDIO_EXT) == 0) ||
        (std_strcmp(ext, REAL_VIDEO_EXT) == 0))
    {
      retVal = NULL;  
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "Error:OpenURN GetMediaType returning NULL");
    }
    return retVal;
  }
 else
#endif
  {
    // Player streaming files without an extension using the QTV player
    return retVal;
  }
}

/* ======================================================================
FUNCTION
QtvPlayer::GetMIMEType

DESCRIPTION
Returns the MIME type based strictly upon the buffer content

========================================================================== */
const char *QtvPlayer::GetMIMEType(unsigned char *pBuf, uint32 dwBufSize, uint32 )
{
  const char *MIMEType = NULL;
  unsigned char buffer[MAXIMUM_MIME_BUFFER_LENGTH];

  if (pBuf)
  {
    /* In case of mp4 */
    if(dwBufSize >= MP4_MIME_BUFFER_LENGTH)
    {
      memcpy(buffer, pBuf, MP4_MIME_BUFFER_LENGTH);
      if (memcmp(buffer+4, FMT_MP4_MARK, 4) == 0)
      {
        MIMEType = QTV_PLAYER_MIME_TYPE;
      }
      else if(memcmp(buffer+4, FMT_MP4_MARK_1, 4) == 0)
      {
        MIMEType = QTV_PLAYER_MIME_TYPE;
      }
    }

    /* Incase of wm */
    if(dwBufSize >= WM_MIME_BUFFER_LENGTH)
    {
      memcpy(buffer, pBuf, WM_MIME_BUFFER_LENGTH);
      if (memcmp(buffer, FMT_WM_MARK, 16) == 0)
      {
        MIMEType = QTV_PLAYER_MIME_TYPE;
      }
    }
    /* Incase of rm */
    if(dwBufSize >= REAL_MIME_BUFFER_LENGTH)
    {
      memcpy(buffer, pBuf, REAL_MIME_BUFFER_LENGTH);
      if (memcmp(buffer, FMT_RM_MARK, 4) == 0)
      {
        MIMEType = REAL_MEDIA_MIME_TYPE;
      }
    }
    /* Incase of MP3 */
    if(dwBufSize >= MP3_MIME_BUFFER_LENGTH)
    {
      if (GEN_AUDIO_FMT_IS_SYNC(pBuf) || GEN_AUDIO_FMT_IS_ID3V2_TAG(pBuf))
      {
        MIMEType = QTV_PLAYER_MIME_TYPE;
      }
    }

    /* Incase of AAC */
    if(dwBufSize >= AAC_MIME_BUFFER_LENGTH)
    {
      if(GEN_AUDIO_FMT_IS_AAC_SYNC(pBuf) || GEN_AUDIO_FMT_IS_AAC_ADIF(pBuf))
      {
        MIMEType = QTV_PLAYER_MIME_TYPE;
      }
    }

    /* Incase of AMR */
    if(dwBufSize >= AMR_MIME_BUFFER_LENGTH)
    {
      memcpy(buffer, pBuf, AMR_MIME_BUFFER_LENGTH);
      if (memcmp(buffer, FMT_AMR_BEGIN, AMR_MIME_BUFFER_LENGTH) == 0)
      {
        MIMEType = QTV_PLAYER_MIME_TYPE;
      }
    }

    /* Incase of AVI */
    if(dwBufSize >= RIFF_AVI_MIME_BUFFER_LENGTH)
    {
      memcpy(buffer, pBuf, RIFF_AVI_MIME_BUFFER_LENGTH);
      if (!memcmp(buffer, FMT_RIFF_MARK, RIFF_MIME_BUFFER_LENGTH) && !memcmp(buffer+8, FMT_AVI_MARK, AVI_MIME_BUFFER_LENGTH))
      {
        MIMEType = QTV_PLAYER_MIME_TYPE;
      }
    }

    /* Incase of QCP */
    if(dwBufSize >= QCP_MIME_BUFFER_LENGTH)
    {
      if (!memcmp(pBuf, FMT_RIFF_MARK, 4) && !memcmp(pBuf+8, FMT_QCP_QLCM,4))
      {
        MIMEType = QTV_PLAYER_MIME_TYPE;
      }
    }
  }
  if(MIMEType)
  {
     LogQtvCmd("GetMIMEType", "%s%d%d", MIMEType,(int32)pBuf, dwBufSize);
  }
  else
  {
     QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Cannot log Qtv cmd %u", MIMEType);
  }
  return MIMEType;
}

/* ======================================================================
FUNCTION
  QtvPlayer::GetMIMEType

DESCRIPTION
   Returns the MIME type QTV_PLAYER_MIME_TYPE ("video/mp4")
   if the file is a media type that can be played by
   the Qtv Player, or NULL, if it is an unknown type. This
   function does not work on streaming files (.URL files)

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
const char *QtvPlayer::GetMIMEType(const char *file)
{
  const char *MIMEType = NULL;
  unsigned char buffer[MAXIMUM_MIME_BUFFER_LENGTH] = {0};
  FILE *fp = fopen(file, "r");
  if (fp)
  {
    int length = fread(buffer, 1, MAXIMUM_MIME_BUFFER_LENGTH, fp);
    if (length)
    {
      MIMEType = QtvPlayer::GetMIMEType(buffer,length,length);
    }
    (void)fclose(fp);
  }
  if(MIMEType && file)
  {
  LogQtvCmd("GetMIMEType", "%s%s", MIMEType, file);
  }
  else
  {
     QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Cannot log Qtv cmd %u,%u", MIMEType, file);
  }
  return MIMEType;
}

/* ======================================================================
FUNCTION
  QtvPlayer::GetMIMEType

DESCRIPTION
   Returns the MIME type QTV_PLAYER_MIME_TYPE ("video/mp4")
   if the file is a media type that can be played by
   the Qtv Player, or REAL_MEDIA_MIME_TYPE ("video/rm"),
   if the buffer can be played by the Real Player, or NULL
   if it is an unknown type.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
const char *QtvPlayer::GetMIMEType(unsigned char *buffer, int length)
{
  const char *MIMEType = NULL;
  MIMEType = QtvPlayer::GetMIMEType(buffer,length,length);
  return MIMEType;
}
/* ======================================================================
FUNCTION
  QtvPlayer::GetTrackTimeScale

DESCRIPTION
  gets the time scale of the given track

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::GetTrackTimeScale(TrackType trackType, uint32 *pTimeScale,
                                                InstanceHandleT handle)
{
  QtvPlayer *player = GetPlayer(handle);

  ReturnT nReturn = QTV_RETURN_ERROR;

  if(player->pMpeg4Player && player->pMpeg4Player->IsThreadActive())
  {
    Media * pMedia;
    if(trackType == VideoTrack)
    {
      pMedia = player->pMpeg4Player->GetVideoMedia();
      if(pMedia)
      {
        *pTimeScale = (uint32)pMedia->GetVideoTimescale(0);
        nReturn = QTV_RETURN_OK;
      }
    else
      {
    nReturn = QTV_RETURN_DATA_NOT_AVAILABLE;
      }
    }
    else if (trackType == AudioTrack)
    {
      pMedia = player->pMpeg4Player->GetAudioMedia();
      if(pMedia)
      {
        *pTimeScale = (uint32)pMedia->GetAudioTimescale(0);
        nReturn = QTV_RETURN_OK;
      }
    else
      {
    nReturn = QTV_RETURN_DATA_NOT_AVAILABLE;
      }
    }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    else if (trackType == TextTrack)
    {
      pMedia = player->pMpeg4Player->GetTextMedia();
      if(pMedia)
      {
        *pTimeScale = (uint32)pMedia->GetTextTimescale(0);
        nReturn = QTV_RETURN_OK;
      }
    else
      {
    nReturn = QTV_RETURN_DATA_NOT_AVAILABLE;
      }
    }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
  }
#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
  else if (player->CurrentMIMEType && strcmp(player->CurrentMIMEType, REAL_MEDIA_MIME_TYPE) == 0)
  {
    nReturn = QTV_RETURN_FEATURE_NOT_AVAILABLE;
  }
#endif /* FEATURE_REAL_PLAYER_USE_QTV_API */
  LogQtvCmd("GetTrackTimeScale", "%d%d%d%d", (int32)nReturn, (int32)trackType,(int32)pTimeScale, (int32)handle);
  return nReturn;
}

/* ======================================================================
FUNCTION
  QtvPlayer::SetLoopTrack

DESCRIPTION
  sets/resets the loop track flag.
  If set:
    repositioning is disabled.
    as soon as any track ends, it will restart.
    qtv will stop only after getting stop command from user.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::SetLoopTrack(bool bLoop, InstanceHandleT handle)
{
  QtvPlayer *player = GetPlayer(handle);
  if(player->pMpeg4Player && player->pMpeg4Player->IsThreadActive())
  {
    if( player->pMpeg4Player->SetLoopTrackFlag(bLoop) )
    {
      LogQtvCmd("SetLoopTrack", "%d%d%d", (int32)QTV_RETURN_OK, (int32)bLoop,(int32)handle);
      return QTV_RETURN_OK;
    }
  }
  return QTV_RETURN_ERROR;
}

/* ======================================================================
FUNCTION
  QtvPlayer::SetDRMKey

DESCRIPTION
  sets/resets the DRM key for secure file playback.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::SetDRMKey(
#ifdef FEATURE_QTV_MFDRM
#error code not present
#else
  void *, uint32, InstanceHandleT
#endif /* FEATURE_QTV_MFDRM */
  )
{
  ReturnT nReturn = QTV_RETURN_ERROR;

#ifdef FEATURE_QTV_MFDRM
#error code not present
#else
  nReturn = QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif

  return nReturn;
}
/* ======================================================================
FUNCTION
  QtvPlayer::SetMediaInfo

DESCRIPTION
  Saves media specific info.  Usually used to specify decoder types, etc
  before OpenURN is called.

DEPENDENCIES
  None.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::SetMediaInfo(
  void  *media_info,
  uint32 size,
  InstanceHandleT handle
)
{
  QtvPlayer *player = GetPlayer(handle);
  ReturnT nReturn = QTV_RETURN_ERROR;
  // Currently this routine is used only by bcast flo media module.
  // Size check to avoid denial-of-service kind of attacks through size parameter
  // Currently the size of the flo media info structure can not exceed 200 bytes
  // Considering the near future and broadcast integration work going on ,
  // checking for ten times of this value.
  if(!player || (size > QTV_MAX_MEDIA_INFO_SIZE))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                 "Invalid size passed to SetMediaInfo or player obj. invalid");
    return nReturn;
  }
  if(player->pMpeg4Player && player->pMpeg4Player->IsThreadActive())
  {
    if( player->pMpeg4Player->SetMediaInfo(media_info, size) == TRUE )
    {
      nReturn = QTV_RETURN_OK;
    }
  }
  else
  {
    if (player->MediaInfo)
    {
      QTV_Free(player->MediaInfo);
      player->MediaInfo = NULL;
      player->MediaInfoSize = 0;
    }
    if (media_info && size)
    {
      player->MediaInfo = QTV_Malloc(size);
      if (player->MediaInfo && (size <= QTV_MAX_MEDIA_INFO_SIZE))
      {
        player->MediaInfoSize = size;
        memcpy(player->MediaInfo, media_info,size);
      }
      else
      {
        nReturn = QTV_RETURN_ERROR;
      }
    }
    nReturn = QTV_RETURN_OK;
  }

  LogQtvCmd("SetMediaInfo", "%d%d%d%d", (int32)nReturn, (int32)media_info, size,(int32)handle);
  return nReturn;
}

/* ======================================================================
FUNCTION
  QtvPlayer::RegisterDRMDecryptMethod

DESCRIPTION
  Registers DRM decryption method.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::RegisterDRMDecryptMethod( QtvPlayer::DRMDecryptMethodT pDecryptFunction, void *pClientData,
#ifdef FEATURE_QTV_WM_DRM_API
                                                        InstanceHandleT handle)
#else
                                                        InstanceHandleT /*handle*/ )
#endif /* FEATURE_QTV_WM_DRM_API */
{
  QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::RegisterDRMDecryptMethod %d %d",pDecryptFunction,pClientData);

  ReturnT nReturn = QTV_RETURN_ERROR;

#ifdef FEATURE_QTV_WM_DRM_API
  QtvPlayer *player = GetPlayer(handle);

  if ( player && player->pMpeg4Player)
  {
    if (player->pMpeg4Player->StartThread())
    {
      QTV_REGISTER_DRM_DECRYPT_METHOD_EVENT_type *pEvent = QCCreateMessage(QTV_REGISTER_DRM_DECRYPT_METHOD_EVENT, player->pMpeg4Player);
      if (pEvent)
      {
        pEvent->pDecryptFunction = pDecryptFunction;
        pEvent->pClientData = pClientData;
        player->pMpeg4Player->PostMessage(pEvent);
        nReturn = QTV_RETURN_OK;
      }
    }
  }
#else /* FEATURE_QTV_WM_DRM_API */
  pDecryptFunction = NULL;  // To fix compiler warnings
  pClientData = NULL;       // To fix compiler warnings
  nReturn = QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif /* FEATURE_QTV_WM_DRM_API */
  LogQtvCmd("RegisterDRMDecryptMethod", "%d%d%d", (int32)nReturn,(int32)pDecryptFunction,(int32)pClientData);
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::RegisterDRMDecryptMethod return %d",nReturn);
  return nReturn;
}

/* ======================================================================
FUNCTION
  QtvPlayer::GetRawFrameInfo

DESCRIPTION
  gets information of the closest frame of the given time (in MSec)

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::GetRawFrameInfo(TrackType trackType, uint32 lTime, RawFrameInfo *pFrameInfo,
                                              InstanceHandleT handle)
{
  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }
  ReturnT nReturn = QTV_RETURN_ERROR;
  if(player->pMpeg4Player && player->pMpeg4Player->IsThreadActive())
  {
    uint32 TimeScale, FrameTime;
    uint32 TrackID, StreamNum;
    Media * pMedia;
    FileBase * pMediaFile;

    if(trackType == VideoTrack)
    {
      pMedia = player->pMpeg4Player->GetVideoMedia();
      if ( !pMedia )
      {
        return nReturn;
      }

      if(lTime && (lTime > (uint32)pMedia->GetDurationMsec()) )
        return nReturn;

      pMediaFile = pMedia->GetFile();
      TimeScale = (uint32)pMedia->GetVideoTimescale(0);
      TrackID = pMedia->GetVideoTrackID(0);
    }
    else if (trackType == AudioTrack)
    {
      pMedia = player->pMpeg4Player->GetAudioMedia();
      if ( !pMedia )
      {
        return nReturn;
      }

      if(lTime && (lTime > (uint32)pMedia->GetDurationMsec()) )
        return nReturn;

      pMediaFile = pMedia->GetFile();
      TimeScale = (uint32)pMedia->GetAudioTimescale(0);
      TrackID = pMedia->GetAudioTrackID(0);
    }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    else if (trackType == TextTrack)
    {
      pMedia = player->pMpeg4Player->GetTextMedia();
      if ( !pMedia )
      {
        return nReturn;
      }

      if(lTime && (lTime > (uint32)pMedia->GetDurationMsec()) )
        return nReturn;

      pMediaFile = pMedia->GetFile();
      TimeScale = (uint32)pMedia->GetTextTimescale(0);
      TrackID = pMedia->GetTextTrackID(0);
    }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
    else
    {
      return nReturn;
    }

    if(pMediaFile && pMediaFile->isMp4File())
    {
      Mpeg4File * pMeg4File = (Mpeg4File *)pMediaFile;
      video_fmt_stream_info_type *StreamInfo = pMeg4File->getTrackInfoForID(TrackID);

      if (StreamInfo)
      {
        StreamNum = StreamInfo->stream_num;
        /* convert MSec to frame time (in track timescale units) */
        float fFrameTime = (float)lTime * (float)TimeScale / (float)TIMESCALE_BASE;
        FrameTime = (uint32)fFrameTime;

        video_fmt_sample_info_type Info;
        memset(&Info, 0, sizeof(video_fmt_sample_info_type));

        /* if the time stamp is zero return the first frame info*/
        if(!lTime)
        {
          if( !pMeg4File->getSampleInfo(StreamNum, 0, 1, &Info) )
            return nReturn;
        }
        else
        {
          /* look for closest frame */
          for(int i=0; Info.time<FrameTime; i++)
          {
            if( !pMeg4File->getSampleInfo(StreamNum, i, 1, &Info) )
              return nReturn;
          }
        }
        /* we got the frame info */
        pFrameInfo->FrameNum = Info.sample;
        pFrameInfo->FrameSize = Info.size;
        fFrameTime = (float)Info.time * (float)TIMESCALE_BASE / (float)TimeScale;
        pFrameInfo->FrameTime = (uint32)fFrameTime;
        float fFrameDelta = (float)Info.delta * (float)TIMESCALE_BASE / (float)TimeScale;
        pFrameInfo->Framedelta = (uint32)fFrameDelta;
        pFrameInfo->FrameOffset = Info.offset;
        pFrameInfo->IsSyncFrame = Info.sync;
        nReturn = QTV_RETURN_OK;
      }
    }
  }
  LogQtvCmd("GetRawFrameInfo", "%d%d%d%d%d", (int32)nReturn, (int32)trackType, lTime,(int32)pFrameInfo,(int32)handle);
  return nReturn;
}

/* ======================================================================
FUNCTION
  QtvPlayer::GetNextRawSyncFrameInfo

DESCRIPTION
  gets Video Sync Frame (I-Frame) information closest to the given frame
  number in reverse or forward direction

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::GetNextRawSyncFrameInfo(uint32 FrameNum, bool reverse, RawFrameInfo *pFrameInfo,
                                                      InstanceHandleT handle)
{
  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }
  ReturnT nReturn = QTV_RETURN_ERROR;
  if(player->pMpeg4Player && player->pMpeg4Player->IsThreadActive())
  {
    Media * pMedia = player->pMpeg4Player->GetVideoMedia();
    if(pMedia)
    {
      FileBase * pMediaFile = pMedia->GetFile();
      if(pMediaFile && pMediaFile->isMp4File())
      {
        Mpeg4File * pMeg4File = (Mpeg4File *)pMediaFile;
        uint32 videoTrackID = pMedia->GetVideoTrackID(0);
        video_fmt_stream_info_type *StreamInfo = pMeg4File->getTrackInfoForID(videoTrackID);
        if (StreamInfo)
        {
          video_fmt_sample_info_type Info;
          uint32 videoStreamNum = StreamInfo->stream_num;
          uint32 timeScale = pMedia->GetVideoTimescale(0);
          if(pMeg4File->getSyncSampleInfo(videoStreamNum, FrameNum, reverse, &Info))
          {
            pFrameInfo->FrameNum = Info.sample;
            pFrameInfo->FrameSize = Info.size;
            float fFrameTime = (float)Info.time * (float)TIMESCALE_BASE / (float)timeScale;
            pFrameInfo->FrameTime = (uint32)fFrameTime;
            float fFrameDelta = (float)Info.delta * (float)TIMESCALE_BASE / (float)timeScale;
            pFrameInfo->Framedelta = (uint32)fFrameDelta;
            pFrameInfo->FrameOffset = Info.offset;
            pFrameInfo->IsSyncFrame = Info.sync;
            nReturn = QTV_RETURN_OK;
          }
        }
      }
    }
  }
  LogQtvCmd("GetNextRawSyncFrameInfo", "%d%d%d%d", (int32)nReturn, FrameNum,(int32)reverse,(int32)pFrameInfo, (int32)handle);
  return nReturn;
}

/* ======================================================================
FUNCTION
  QtvPlayer::GetLargestRawFrameSize

DESCRIPTION
  gets size of the largest frame of the given track.
    May return QTV_RETURN_ERROR, if track not found.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::GetLargestRawFrameSize(TrackType trackType, uint32 *pFrameSize,
                                                     InstanceHandleT handle)
{
  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }
  ReturnT nReturn = QTV_RETURN_ERROR;

  if(player->pMpeg4Player && player->pMpeg4Player->IsThreadActive())
  {
    Media * pMedia;
    if(trackType == VideoTrack)
    {
      pMedia = player->pMpeg4Player->GetVideoMedia();
      if(pMedia)
      {
        *pFrameSize = (uint32)pMedia->GetMaxVideoFrameSize(0);
        nReturn = QTV_RETURN_OK;
      }
    }
    else if (trackType == AudioTrack)
    {
      pMedia = player->pMpeg4Player->GetAudioMedia();
      if(pMedia)
      {
        *pFrameSize = (uint32)pMedia->GetMaxAudioFrameSize(0);
        nReturn = QTV_RETURN_OK;
      }
    }
    else if (trackType == TextTrack)
    {
       pMedia = player->pMpeg4Player->GetTextMedia();
      if(pMedia)
      {
        *pFrameSize = (uint32)pMedia->GetMaxTextFrameSize(0);
        nReturn = QTV_RETURN_OK;
      }
    }
  }
  LogQtvCmd("GetLargestRawFrameSize", "%d%d%d%d", (int32)nReturn, (int32)trackType,(int32)pFrameSize, (int32)handle);
  return nReturn;
}

/* ======================================================================
FUNCTION
  QtvPlayer::GetNumFrames

DESCRIPTION
  Gets number of frames in the given track.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  return QTV_RETURN_ERROR, if track not found,
  otherwise pNumFrames sill have number of frames and returns QTV_RETURN_OK.

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::GetNumFrames(TrackType trackType, uint32 *pNumFrames,
                                           InstanceHandleT handle)
{
  QtvPlayer *player = GetPlayer(handle);

  ReturnT nReturn = QTV_RETURN_ERROR;
  if(player->pMpeg4Player && player->pMpeg4Player->IsThreadActive())
  {
    uint32 trackID=0;
    Media * pMedia;
    if(trackType == VideoTrack)
    {
      pMedia  = player->pMpeg4Player->GetVideoMedia();
      if(pMedia)
        trackID = pMedia->GetVideoTrackID(0);
    }
    else if (trackType == AudioTrack)
    {
      pMedia  = player->pMpeg4Player->GetAudioMedia();
      if(pMedia)
        trackID = pMedia->GetAudioTrackID(0);
    }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    else if (trackType == TextTrack)
    {
      pMedia  = player->pMpeg4Player->GetTextMedia();
      if(pMedia)
        trackID = pMedia->GetTextTrackID(0);
    }
#endif
    else
    {
      return nReturn;
    }

    if(pMedia)
    {
      FileBase * pMediaFile = pMedia->GetFile();
      if(pMediaFile && pMediaFile->isMp4File())
      {
        Mpeg4File * pMeg4File = (Mpeg4File *)pMediaFile;
        video_fmt_stream_info_type *pTrack = pMeg4File->getTrackInfoForID(trackID);
        if (pTrack)
        {
          *pNumFrames = pTrack->frames;
          nReturn = QTV_RETURN_OK;
        }
      }
    }
  }
  LogQtvCmd("GetNumFrames", "%d%d%d%d", (int32)nReturn, (int32)trackType,(int32)pNumFrames,(int32)handle);
  return nReturn;
}

/* ======================================================================
FUNCTION
  QtvPlayer::GetRawTrackHeader

DESCRIPTION
   gets the header of the track. User should pass "pBufSize" with the size of
   supplied buffer. If size of supplied buffer is not sufficient, it will return
   QTV_RETURN_BUFFER_UNDERRUN with "pBufSize" having header size. In case of any
   other error, this will return QTV_RETURN_ERROR with pointers untouched.
   In case of success, this will return QTV_RETURN_OK and "pBufSize" will have
   header length.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::GetRawTrackHeader(TrackType trackType, byte *pBuf, uint32 *pBufSize,
                                                InstanceHandleT handle)
{
  QtvPlayer *player = GetPlayer(handle);
  QtvPlayer::ReturnT   status = QTV_RETURN_ERROR;
  MP4_ERROR_CODE errortype = EVERYTHING_FINE;

  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }

  if(player->pMpeg4Player && player->pMpeg4Player->IsThreadActive())
  {
    uint32 trackID = (uint32)-1;
    Media * pMedia;
    if(trackType == VideoTrack)
    {
      pMedia  = player->pMpeg4Player->GetVideoMedia();
      if(pMedia)
        trackID = pMedia->GetVideoTrackID(0);
    }
    else if (trackType == AudioTrack)
    {
      pMedia  = player->pMpeg4Player->GetAudioMedia();
      if(pMedia)
        trackID = pMedia->GetAudioTrackID(0);
    }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    else if (trackType == TextTrack)
    {
      pMedia  = player->pMpeg4Player->GetTextMedia();
      if(pMedia)
        trackID = pMedia->GetTextTrackID(0);
    }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
    else
    {
      return status;
    }

    if(pMedia)
    {
      FileBase * pMediaFile = pMedia->GetFile();

      if(pMediaFile)
      {
        Mpeg4File * pMeg4File = (Mpeg4File *)pMediaFile;

        if(!pMeg4File)
        {
          return status;
        }

        errortype = pMeg4File->getTrackDecoderSpecificInfoContent(trackID,(uint8*)pBuf,pBufSize);

        switch (errortype)
        {
         case EVERYTHING_FINE:
            status = QTV_RETURN_OK;
         break;
          case INSUFFICIENT_BUFFER_SIZE:
            status = QTV_RETURN_BUFFER_UNDERRUN;
          break;
          default:
            status = QTV_RETURN_ERROR;
          break;
        }
        return status;
      }//if(pMediaFile)
    }//if(pMedia)
  }
  LogQtvCmd("GetRawTrackHeader", "%d%d%d%d%d", (int32)status, (int32)trackType,(int32)pBuf, (int32)pBufSize, (int32)handle);
  return status;
}

/* ======================================================================
FUNCTION
  QtvPlayer::GetRawFrame

DESCRIPTION
  gets the frame of a given track by frame number and also gets frame info

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::GetRawFrame( TrackType trackType, uint32 FrameNum,
                                           RawFrameInfo *pFrameInfo, byte *pBuf, uint32 bufSize,
                                           InstanceHandleT handle)
{
  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }
  ReturnT nReturn = QTV_RETURN_ERROR;
  if(player->pMpeg4Player && player->pMpeg4Player->IsThreadActive())
  {
    uint32 trackID=0;
    uint32 timeScale=0;
    Media * pMedia;
    if(trackType == VideoTrack)
    {
      pMedia  = player->pMpeg4Player->GetVideoMedia();
      if(pMedia)
      {
        trackID = pMedia->GetVideoTrackID(0);
        timeScale = pMedia->GetVideoTimescale(0);
      }
    }
    else if (trackType == AudioTrack)
    {
      pMedia  = player->pMpeg4Player->GetAudioMedia();
      if(pMedia)
      {
        trackID = pMedia->GetAudioTrackID(0);
        timeScale = pMedia->GetAudioTimescale(0);
      }
    }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    else if (trackType == TextTrack)
    {
      pMedia  = player->pMpeg4Player->GetTextMedia();
      if(pMedia)
      {
        trackID = pMedia->GetTextTrackID(0);
        timeScale = pMedia->GetTextTimescale(0);
      }
    }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
    else
    {
      return nReturn;
    }

    if(pMedia)
    {
      FileBase * pMediaFile = pMedia->GetFile();
      if(pMediaFile && pMediaFile->isMp4File())
      {
        video_fmt_sample_info_type Info;
        Mpeg4File * pMeg4File = (Mpeg4File *)pMediaFile;
        video_fmt_stream_info_type *pTrack = pMeg4File->getTrackInfoForID(trackID);
        if (pTrack)
        {
          uint32 StreamNum = pTrack->stream_num;
          uint32 header = pTrack->header;

          if(pMeg4File->getSampleInfo(StreamNum, FrameNum, 1, &Info))
          {
            /* take care of decoder specific header in the first frame */
            if ( !FrameNum )
            {
              Info.size -= header;
              Info.offset += header;
            }

            if(Info.size > bufSize)
              return nReturn;

            if(pMeg4File->getSample(StreamNum, VIDEO_FMT_DATA_UNIT_BYTE, Info.offset, Info.size, pBuf))
            {
              pFrameInfo->FrameNum = Info.sample;
              pFrameInfo->FrameSize = Info.size;
              float fFrameTime =0;
              float fFrameDelta =0;
              if(timeScale!=0)
                fFrameTime = (float)Info.time * (float)TIMESCALE_BASE / (float)timeScale;
              pFrameInfo->FrameTime = (uint32)fFrameTime;
              if(timeScale!=0)
                fFrameDelta = ((float)Info.delta * (float)TIMESCALE_BASE + (timeScale>>1)) / (float)timeScale;

              pFrameInfo->Framedelta = (uint32)fFrameDelta;

              pFrameInfo->FrameOffset = Info.offset;
              pFrameInfo->IsSyncFrame = Info.sync;
              nReturn = QTV_RETURN_OK;
            }
          }
        }
      }
    }
  }
  LogQtvCmd("GetRawFrame", "%d%d%d%d%d%d%d", (int32)nReturn, (int32)trackType, FrameNum,(int32)pFrameInfo,(int32)pBuf, bufSize,(int32)handle);
  return nReturn;
}

/* ======================================================================
FUNCTION
  QtvPlayer::InitializeTelopElements

DESCRIPTION
  if telop is present in the opened clip, this will initialize the text player, so
  that it can process telops.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function should be used only for video editing.
  Calling this function during playback will cause wrong telop display.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::InitializeTelopElements(InstanceHandleT handle)
{
  TimedText *pTText = NULL;

#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }
  ReturnT nReturn = QTV_RETURN_ERROR;
  if(player->pMpeg4Player && player->pMpeg4Player->IsThreadActive())
  {
    Media     *pMedia  = player->pMpeg4Player->GetTextMedia();
#ifndef FEATURE_WINCE
    pTText  = player->pMpeg4Player->GetTextPlayer();
#endif
    if(pMedia && pTText)
    {
      if(pMedia->IsTelopPresent())
      {
        if(pTText->Prep(100, false, pMedia))
          nReturn = QTV_RETURN_OK;
      }
    }
  }
  LogQtvCmd("InitializeTelopElements", "%d%d", (int32)nReturn, (int32)handle);
  return nReturn;
#else
  return QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif
}

/* ======================================================================
FUNCTION
  QtvPlayer::SetupNextTelopElement

DESCRIPTION
  prepares the next telop elements, so other telop functions can be use to
  access the telop element.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function should be used only for video editing.
  Calling this function during playback will cause wrong telop display.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::SetupNextTelopElement(InstanceHandleT handle)
{
  TimedText *pTText = NULL;
#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }
  ReturnT nReturn = QTV_RETURN_ERROR;
  if(player->pMpeg4Player && player->pMpeg4Player->IsThreadActive())
  {
#ifndef FEATURE_WINCE
    pTText  = player->pMpeg4Player->GetTextPlayer();
#endif
    if(pTText)
    {
      if(pTText->SetupNextTelopElement())
        nReturn = QTV_RETURN_OK;
    }
  }
  LogQtvCmd("SetupNextTelopElement", "%d%d", (int32)nReturn, (int32)handle);
  return nReturn;
#else
    return QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif
}




/* ======================================================================
FUNCTION
  QtvPlayer::SetUnhandledRTSPHeaderCallback

DESCRIPTION
  Allows OEM to specify a callback function which will be invoked whenever
 an unknown RTSP header line is encountered while processing incoming
 RTSP messages.
  The pointer can be null.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Replaces any previously set handler.  If the pointer is NULL, the
 callback is disabled.


========================================================================== */
void QtvPlayer::SetUnhandledRTSPHeaderCallback(
   QtvPlayer::UnhandledStreamingItemCallbackT newCallback,
   InstanceHandleT handle)
{
  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers == 1)
  {
    player->unprocessedRtspHeaderCallback = newCallback;
    LogQtvCmd("SetUnhandledRTSPHeaderCallback", "%d%d%d", 0,(int32)newCallback, (int32)handle);
  }
}


/* ======================================================================
FUNCTION
  QtvPlayer::SetUnhandledSDPCallback

DESCRIPTION
  Allows OEM to specify a callback function which will be invoked whenever
 an unknown sdp header line is encountered while processing an SDP
  The pointer can be null.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Replaces any previously set handler.  If the pointer is NULL, the
 callback is disabled.

========================================================================== */
void QtvPlayer::SetUnhandledSDPCallback(
   QtvPlayer::UnhandledStreamingItemCallbackT newCallback,
   InstanceHandleT handle)
{
  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers == 1)
  {
    player->unprocessedSdpHeaderCallback = newCallback;
    LogQtvCmd("SetUnhandledSDPCallback", "%d%d%d", 0, (int32)newCallback ,(int32)handle);
  }
}


/* ======================================================================
FUNCTION
  QtvPlayer::GetUnhandledRTSPHeaderCallback

DESCRIPTION
  Allows OEM to retrieve the currently specified callback function which
 will be invoked whenever an unknown rtso header line is encountered while
 processing an incoming rtsp message
  The returned pointer may be null.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================== */
QtvPlayer::UnhandledStreamingItemCallbackT
QtvPlayer::GetUnhandledRTSPHeaderCallback(InstanceHandleT handle)
{
  QtvPlayer *player = GetPlayer(handle);
  QtvPlayer::UnhandledStreamingItemCallbackT retVal = NULL;
  if(numRegisteredQtvPlayers == 1)
  {
   retVal = player->unprocessedRtspHeaderCallback;
   LogQtvCmd("GetUnhandledRTSPHeaderCallback", "%d%d", (int32)retVal, (int32)handle);
   return retVal;
  }
  else
    return retVal;
}


/* ======================================================================
FUNCTION
  QtvPlayer::GetUnhandledSDPCallback

DESCRIPTION
  Allows OEM to retrieve the currently specified callback function which
 will be invoked whenever an unknown sdp header line is encountered while
 processing an SDP
  The returned pointer may be null.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================== */
QtvPlayer::UnhandledStreamingItemCallbackT
QtvPlayer::GetUnhandledSDPCallback(InstanceHandleT handle)
{
  QtvPlayer *player = GetPlayer(handle);
  QtvPlayer::UnhandledStreamingItemCallbackT retVal = NULL;
  if(numRegisteredQtvPlayers == 1)
  {
     retVal = player->unprocessedSdpHeaderCallback;
     LogQtvCmd("GetUnhandledRTSPHeaderCallback", "%d%d", (int32)retVal, (int32)handle);
     return retVal;
  }
  else
    return NULL;
}


/* ======================================================================
FUNCTION
  QtvPlayer::EditOemRtspHeaders

DESCRIPTION
  Function modifies the table of eom rtsp header lines to match
 the request passed.  Entries can be added, removed or modified.

DEPENDENCIES
  None

RETURN VALUE
  Result of the editing request

SIDE EFFECTS
  The OEM rtsp headers array is updated to match the new setting, if
 possible.  Each entry that is added will allocate memory from
 the heap.
  Command notes:
  1. For deleting and replacing an entry, case is NOT significant
  2. Case of characters stored matches what was passed in

 NOTE NOTE: these entries are PERSISTENT, so the memory used
 is not released until reboot or until removed with OEM_RTSP_HEADER_DELETE_ALL
 or OEM_RTSP_HEADER_DELETE
========================================================================== */
QtvPlayer::OemRtspHeaderResult QtvPlayer::EditOemRtspHeaders(
       QtvPlayer::OemRtspHeaderCommand whatToDo,
       uint32 whichMethodsAffected, // AffectedRTSPMethod, dc on delete
       const char *headerName,  // don't care for delete_all
       const char *headerValue,  // don't care for delete_all, delete
       InstanceHandleT handle
)
{
   QTV_MSG_SPRINTF_4(QTVDIAG_DEBUG, "EditOEM cmd=%d, meth=%d, hdr='%s', val='%s'",
      whatToDo, whichMethodsAffected, headerName, headerValue); // WCL
   QtvPlayer *player = GetPlayer(handle);
   if(numRegisteredQtvPlayers > 1)
   {
     return OEM_RTSP_RESULT_BAD_COMMAND;
   }
  OemRtspHeaderResult retVal = OEM_RTSP_RESULT_BAD_COMMAND;
#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
  if (player->CurrentMIMEType && strcmp(player->CurrentMIMEType, REAL_MEDIA_MIME_TYPE) == 0)
  {
    retVal = QtvRealPlayer_EditOemRtspHeaders(whatToDo, whichMethodsAffected,
        headerName,  headerValue);
  }
  else
#endif
  {
   QTV_USE_ARG1(player);
   RtspOemHeaders *headerManager = RtspOemHeaders::GetInstance();
   if (headerManager != 0)
   {
      // forward request to handler
       retVal = headerManager->EditOemRtspHeaders(
         whatToDo, whichMethodsAffected, headerName, headerValue);
   } else {
        retVal = QtvPlayer::OEM_RTSP_RESULT_NO_MEMORY;
    }
  }
  if(headerName && headerValue)
  {
       LogQtvCmd("EditOemRtspHeaders", "%d%d%d%s%s%d",
                 (int32)retVal,(int32)whatToDo, (int32)whichMethodsAffected, headerName, headerValue,(int32)handle);
     }
  else
  {
     QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Cannot log Qtv cmd %u,%u", headerName, headerValue);
   }
  return retVal;
}
/* ======================================================================
FUNCTION
  QtvPlayer::AddOemHttpHeaders

DESCRIPTION
  Function updates the table of HTTP header lines to be
  populated in the GET request.

DEPENDENCIES
  None

RETURN VALUE
  Result of the editing request

SIDE EFFECTS
========================================================================== */
QtvPlayer::ReturnT QtvPlayer::AddOemHttpHeaders(
       const char *headerName,
       const char *headerValue,
       QtvPlayer::OemHttpHeaderResult &err,
       InstanceHandleT handle

)
{
   QTV_MSG_SPRINTF_2(QTVDIAG_DEBUG, "EditHttpOEM hdr='%s', val='%s'",
      headerName, headerValue);
   QtvPlayer::ReturnT retVal = QTV_RETURN_ERROR;

   QtvPlayer *player = GetPlayer(handle);
   if(numRegisteredQtvPlayers > 1)
   {
     return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
   }

   if (player->pMpeg4Player)
   {
#ifdef FEATURE_QTV_HTTP_DOWNLOAD
      err = player->pMpeg4Player->AddOemHttpHeaders(headerName, headerValue);
#else
      err = OEM_HTTP_RESULT_BAD_DATA;
      retVal = QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif
   }
   if (err == OEM_HTTP_RESULT_OK)
   {
      retVal = QTV_RETURN_OK;
   }
   if(headerName && headerValue)
   {
   LogQtvCmd("AddOemHttpHeaders", "%d%s%s%d%d", (int32)retVal, headerName, headerValue, &err, (int32)handle);
   }
   else
   {
      QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Cannot log Qtv cmd %u,%u", headerName, headerValue);
   }
   return retVal;
}

/* ======================================================================
FUNCTION
  QtvPlayer::SetNetPolicyInfo

DESCRIPTION
  Sets the network policy of the QTV Player. This policy is used for
  streaming sessions while bringing up the network.

DEPENDENCIES
  None

RETURN VALUE
  QTV_RETURN_OK: In case of success
  QTV_RETURN_ERROR: otherwise

SIDE EFFECTS
  None

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::SetNetPolicyInfo(NetPolicyInfo *netPolicyInfo,
                                               InstanceHandleT handle)
{
#ifdef FEATURE_QTV_NET_POLICY
  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }
  ReturnT nReturn = QTV_RETURN_ERROR;
  if(player->pMpeg4Player && player->pMpeg4Player->IsThreadActive())
  {
    if( player->pMpeg4Player->SetNetPolicyInfo(netPolicyInfo) == TRUE )
     nReturn = QTV_RETURN_OK;
  }
  else
  {
      if (netPolicyInfo)
      {
         memcpy(&player->NetPolicy, netPolicyInfo, sizeof(NetPolicyInfo));
      }
      else
      {
         memset(&player->NetPolicy, 0, sizeof(NetPolicyInfo));
         player->NetPolicy.isSet = false;
      }
#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
      if (player->CurrentMIMEType && strcmp(player->CurrentMIMEType, REAL_MEDIA_MIME_TYPE) == 0)
      {
         RealPlayer_SetPDPProfileNumber(player->NetPolicy.primaryPDPProfileNo);
      }
#endif /* FEATURE_REAL_PLAYER_USE_QTV_API */
      nReturn = QTV_RETURN_OK;
  }
  LogQtvCmd("SetNetPolicyInfo", "%d%d%d", (int32)nReturn, (int32)netPolicyInfo, (int32)handle);
  return nReturn;
#else

   QTV_USE_ARG2(netPolicyInfo, handle);
   return QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif
}

/* ======================================================================
FUNCTION
  QtvPlayer::GetNetPolicyInfo

DESCRIPTION
  Returns the network policy of the QTV Player to the caller in the
  NetPolicyInfo structure, allocated by the caller, pointed to by
  netPolicyInfo.

DEPENDENCIES

RETURN VALUE
  QTV_RETURN_OK: In case of success
  QTV_RETURN_ERROR: otherwise

SIDE EFFECTS

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::GetNetPolicyInfo(NetPolicyInfo *netPolicyInfo,
                                               InstanceHandleT handle)
{
#ifdef FEATURE_QTV_NET_POLICY
  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }
  ReturnT nReturn = QTV_RETURN_ERROR;
  if(player->pMpeg4Player && player->pMpeg4Player->IsThreadActive())
  {
    if( player->pMpeg4Player->GetNetPolicyInfo(netPolicyInfo) == TRUE )
      nReturn = QTV_RETURN_OK;
  }
#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
  else
  {
      if (player->CurrentMIMEType && strcmp(player->CurrentMIMEType, REAL_MEDIA_MIME_TYPE) == 0)
      {
         memcpy(netPolicyInfo, &player->NetPolicy, sizeof(NetPolicyInfo));
         if (player->NetPolicy.isSet)
         {
            nReturn = QTV_RETURN_OK;
         }
      }
  }
#endif
  LogQtvCmd("SetNetPolicyInfo", "%d%d%d", (int32)nReturn, (int32)netPolicyInfo, (int32)handle);
  return nReturn;
#else
  QTV_USE_ARG2(netPolicyInfo, handle);
  return QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif
}

/* ======================================================================
FUNCTION
  QtvPlayer::SetDescramblerHandler

DESCRIPTION
  Sets the external descrambler proxy.

DEPENDENCIES

RETURN VALUE
  QTV_RETURN_OK: In case of success
  QTV_RETURN_ERROR: otherwise

SIDE EFFECTS

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::SetDescramblerHandler(void *descrambler,
                                                     InstanceHandleT handle)
{
#ifdef FEATURE_QTV_ENCRYPTED_STREAMS
  QtvPlayer *player = GetPlayer(handle);

  if(numQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }

  ReturnT nReturn = QTV_RETURN_ERROR;
  if(player && player->pMpeg4Player && player->pMpeg4Player->IsThreadActive())
  {
    if( player->pMpeg4Player->SetDescramblerHandler(descrambler) == TRUE )
     nReturn = QTV_RETURN_OK;
  }
  else
  {
      if ((NULL != player) && (NULL != descrambler))
      {
         player->descramblerHandler = descrambler;
         nReturn = QTV_RETURN_OK;
      }
  }

  LogQtvCmd("SetDescramblerHandler", "%d%d%d", (int32)nReturn, (int32)descrambler, (int32)handle);
  return nReturn;
#else

   QTV_USE_ARG1(descrambler);
   QTV_USE_ARG1(handle);

   return QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif
}

/* ======================================================================
FUNCTION:
  QtvPlayer::GetMidiDataSize

DESCRIPTION:
  returns MIDI data size.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  size of MIDI data
  0 if MIDI not present.

SIDE EFFECTS:
  None.
======================================================================*/
uint32 QtvPlayer::GetMidiDataSize(InstanceHandleT handle)
{
#ifdef FEATURE_QTV_SKT_MOD_MIDI
  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }

  if(player->pMpeg4Player && player->pMpeg4Player->IsThreadActive())
  {
    uint32 retVal = player->pMpeg4Player->GetMidiDataSize();
    LogQtvCmd("GetMidiDataSize", "%d%d", retVal,  (int32)handle);
    return retVal;
  }
  return 0;
#else
  return 0;
#endif
}

/* ======================================================================
FUNCTION:
  QtvPlayer::GetMidiData

DESCRIPTION:
  copies the MIDI data into supplied buffer.

INPUT/OUTPUT PARAMETERS:
  pBuf  - INPUT/OUTPUT  - buffer for data to be copied.
  size  - INPUT         - size of buffer and max data to be copied.
  offset- INPUT         - 0 based offset of data to be copied from start

RETURN VALUE:
  actual bytes copied into buffer
  0 if no MIDI data is available

SIDE EFFECTS:
  None.
======================================================================*/
uint32 QtvPlayer::GetMidiData(uint8 *pBuf, uint32 size, uint32 offset,
                              InstanceHandleT handle)
{
#ifdef FEATURE_QTV_SKT_MOD_MIDI
  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }

  if(player->pMpeg4Player && player->pMpeg4Player->IsThreadActive())
  {
     uint32 retVal = player->pMpeg4Player->GetMidiData(pBuf, size, offset);
     LogQtvCmd("GetMidiData", "%d%d%d%d%d", retVal, (int32)pBuf, size, offset, (int32)handle);
     return retVal;
  }
  return 0;
#else
  return 0;
#endif
}

/* ======================================================================
FUNCTION:
  QtvPlayer::GetLinkDataSize

DESCRIPTION:
  returns LINK data size.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  size of LINK data
  0 if LINK data not present.

SIDE EFFECTS:
  None.
======================================================================*/
uint32 QtvPlayer::GetLinkDataSize(InstanceHandleT handle)
{
#ifdef FEATURE_QTV_SKT_MOD_MIDI
  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }

  if(player->pMpeg4Player && player->pMpeg4Player->IsThreadActive())
  {
     uint32 retVal = player->pMpeg4Player->GetLinkDataSize();
     LogQtvCmd("GetLinkDataSize", "%d%d",retVal , (int32)handle);
     return retVal;
  }
  return 0;
#else
  return 0;
#endif
}

/* ======================================================================
FUNCTION:
  QtvPlayer::GetLinkData

DESCRIPTION:
  copies the LINK data into supplied buffer.

INPUT/OUTPUT PARAMETERS:
  pBuf  - INPUT/OUTPUT  - buffer for data to be copied.
  size  - INPUT         - size of buffer and max data to be copied.

RETURN VALUE:
  actual bytes copied into buffer
  0 if no MIDI data is available

SIDE EFFECTS:
  None.
======================================================================*/
uint32 QtvPlayer::GetLinkData(uint8 *pBuf, uint32 size,
                              InstanceHandleT handle)
{
#ifdef FEATURE_QTV_SKT_MOD_MIDI
  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }

  if(player->pMpeg4Player && player->pMpeg4Player->IsThreadActive())
  {
     uint32 retVal = player->pMpeg4Player->GetLinkData(pBuf, size);
     LogQtvCmd("GetLinkData", "%d%d%d%d",retVal ,(int32)pBuf, size, (int32)handle);
     return retVal;
  }
  return 0;
#else
  return 0;
#endif
}


/* ======================================================================
FUNCTION:
  QtvPlayer::GetDataSize

DESCRIPTION:
  returns the specified data-type data size.

INPUT/OUTPUT PARAMETERS:
  dType - data-type.

RETURN VALUE:
  size of the specified data-type.
  0 if specified data not present.

SIDE EFFECTS:
  None.
======================================================================*/
uint32 QtvPlayer::GetDataSize(DataT dType, uint32 offset, InstanceHandleT handle)
{
  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }

  if(player->pMpeg4Player && player->pMpeg4Player->IsThreadActive())
  {
     uint32 retVal = player->pMpeg4Player->GetDataSize(dType, offset );
     LogQtvCmd("GetDataSize", "%d%d%d", retVal ,(int32)dType, (int32)handle);
     return retVal;
  }
  return 0;
}

/* ======================================================================
FUNCTION:
  QtvPlayer::GetData

DESCRIPTION:
  copies the specified data-type data into supplied buffer.

INPUT/OUTPUT PARAMETERS:
  dType - data-type.
  pBuf  - INPUT/OUTPUT  - buffer for data to be copied.
  size  - INPUT         - size of buffer and max data to be copied.
  offset- INPUT         - 0 based offset of data to be copied from start

RETURN VALUE:
  actual bytes copied into buffer
  0 if no data is available

SIDE EFFECTS:
  None.
======================================================================*/
uint32 QtvPlayer::GetData(DataT dType, uint8 *pBuf, uint32 size, uint32 offset,
                          InstanceHandleT handle)
{
  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }

  if(player->pMpeg4Player && player->pMpeg4Player->IsThreadActive())
  {
     uint32 retVal = player->pMpeg4Player->GetData(dType, pBuf, size, offset);
     LogQtvCmd("GetData", "%d%d%d%d%d%d", retVal ,(int32)dType, (int32)pBuf,size, offset, (int32)handle);
     return retVal;
  }
  return 0;
}
/* ======================================================================
FUNCTION:
  QtvPlayer::ModifyBufferLowMark

DESCRIPTION:
  Modifies a low water mark point for rebuffering

INPUT/OUTPUT PARAMETERS:
  int32 Value which specifes the low water Mark Point

RETURN VALUE:
  QtvPlayer ReturnT type

SIDE EFFECTS:
  None.
======================================================================*/

QtvPlayer::ReturnT QtvPlayer::ModifyBufferLowMark(int32 modified_buffer_lowmark,
                                                  InstanceHandleT handle)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::ModifyBufferLowMark");

  ReturnT nReturn = QTV_RETURN_ERROR;

  if(numRegisteredQtvPlayers > 1)
  {
    return QTV_RETURN_MULTIPLE_INSTANCE_RUNNING;
  }

  if(QtvPlayer::SetQTVConfigItem(QtvConfig::QTVCONFIG_REBUFFERING_THRESHOLD,
                               (int *)&modified_buffer_lowmark, handle))
  {
    nReturn = QTV_RETURN_OK;
  }
  else
  {
    nReturn= QTV_RETURN_ERROR;
  }
  LogQtvCmd("ModifyBufferLowMark", "%d%d%d", (int32)nReturn, modified_buffer_lowmark, (int32)handle);
  return nReturn;
}

/* ======================================================================
FUNCTION:                                                            GRB
  QtvPlayer::ReleaseCurrentVideoFrameBuffer

DESCRIPTION:
  Notify the player that higher levels have finished rendering the
  current video frame.  The player is free to reuse or free the buffer
  after this function is called.

INPUT/OUTPUT PARAMETERS:
  pBuffer  - Buffer to be released.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void QtvPlayer::ReleaseCurrentVideoFrameBuffer( void *pBuffer, InstanceHandleT handle )
{
  QtvPlayer *player = GetPlayer(handle);

  if(numRegisteredQtvPlayers == 0 || player == NULL)
  {
    QTV_MSG_PRIO2(QTVDIAG_GENERAL,
                  QTVDIAG_PRIO_HIGH,
                  "QTVPlayer already Destroyed.numRegisteredQtvPlayers= %d, player= %d",
                   numRegisteredQtvPlayers,player);
    return;
  }
  if ( player->pMpeg4Player != NULL )
  {
    player->pMpeg4Player->ReleaseVideoBuffer(pBuffer);
  }
#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
  else if(player->CurrentMIMEType && strcmp(player->CurrentMIMEType, REAL_MEDIA_MIME_TYPE) == 0)
  {
    QtvRealPlayer_ReleaseVideoBuffer(pBuffer);
  }
#endif
}

/* ======================================================================
FUNCTION:
  QtvPlayer::CopyBufferToFile(char * fullpathtoFilename)

DESCRIPTION:
  Save downloaded buffer media to file system with the given
  file name.

INPUT/OUTPUT PARAMETERS:
  fullpathtoFilename: Full path to file name

RETURN VALUE:
  QtvPlayer ReturnT type

SIDE EFFECTS:
  None.
======================================================================*/
bool QtvPlayer::CopyBufferToFile(
#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  char * fullpathtoFilename,
  bool ow,
  InstanceHandleT handle
#else
  char*, bool, InstanceHandleT
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */
  )
{
#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::CopyBufferToFile");

  QtvPlayer *player = GetPlayer(handle);
  if(numRegisteredQtvPlayers > 1)
  {
    return false;
  }

  if (player->pMpeg4Player)
  {
    if(fullpathtoFilename)
    {
    LogQtvCmd("CopyBufferToFile", "%d%s%d%d", 0, fullpathtoFilename,(int32)ow,(int32)handle);
    }
    else
    {
       QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Cannot log Qtv cmd %u", fullpathtoFilename);
    }
    if (player->pMpeg4Player->StartThread())
    {
      QTV_COPY_BUFFER_TO_FILE_type *pEvent = QCCreateMessage(QTV_COPY_BUFFER_TO_FILE, player->pMpeg4Player);

      if (pEvent)
      {
        int length = strlen(fullpathtoFilename);
        if (length <= QTV_MAX_FILENAME_BYTES)
        {
           pEvent->fullpathtoFilename = (char*) QTV_Malloc(length + 1);
           if (pEvent->fullpathtoFilename)
           {
              ZUtils::StrcpyN(pEvent->fullpathtoFilename, fullpathtoFilename, length + 1);
              pEvent->ow=ow;
              player->pMpeg4Player->PostMessage(pEvent);
              return true;
           }
           else
           {
              QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "CopyBuffer::Malloc Failed");
              QTV_Free(pEvent);
           }
        }
        else
        {
          QTV_Free(pEvent);
        }
      }
    }
  }

  return false;
#else
  return false;
#endif
}

/* ======================================================================
FUNCTION:
  SelectPlaybackTracks

DESCRIPTION:
  It selects the playble tracks from a opened local file or progressive
  download / pseudo stream session. Not applicable for regular streaming.
  It will only succeed if QTV is in READY/IDLE state (not playing).

INPUT/OUTPUT PARAMETERS:
  dwTrackFlag - bitwise flags for tracks to be selected
  #define QTV_NO_TRACK_SELECTED             0x00
  #define QTV_VIDEO_TRACK_SELECTED          0x01
  #define QTV_AUDIO_TRACK_SELECTED          0x02
  #define QTV_TEXT_TRACK_SELECTED           0x04

RETURN VALUE:
  QtvPlayer ReturnT type

SIDE EFFECTS:
  a call to OpenURN will reset these values and flags passed in OpenURN
  will be valid.
======================================================================*/
QtvPlayer::ReturnT QtvPlayer::SelectPlaybackTracks(uint32 dwTrackFlag,
                                                   InstanceHandleT handle)
{
  QtvPlayer::ReturnT retVal = QTV_RETURN_ERROR;
  QtvPlayer *player = GetPlayer(handle);

  if (player->pMpeg4Player)
  {
     retVal = player->pMpeg4Player->SelectPlaybackTracks(dwTrackFlag);
     LogQtvCmd("SelectPlaybackTracks", "%d%d%d",(int32)retVal ,dwTrackFlag,(int32)handle);
     return retVal;
  }
  return retVal;
}

/* ======================================================================
FUNCTION:
  QtvPlayer::LogQtvCmd

DESCRIPTION:
  Meaningful description

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/

void QtvPlayer::LogQtvCmd(const char *cmd ,...)
{
#ifndef FEATURE_DISABLE_QTV_DIAG_IFACE
  QTV_LOG_CMD_TYPE *qtv_cmd = NULL;
  int32 cmd_len;
  int32 j=0;
  int16 num_args=0;
  int32 *tint_args=NULL;
  uint32 *tuint_args=NULL;
  double *tdouble_args=NULL;
  char **tstr_args=NULL;
  char *arg_types=NULL;
  const char *fmt;
  uint32 log_cmd_pkt_length;
  uint32 tmp_length;
  uint32 num_args_approx;
  uint32 t_total_args_len = 0;
  va_list v_args;
  char *tptr;
  int16 arglen;
  int temp_double = 0;
  static uint32 seq_number = 0;

  if (NULL == cmd)
  {
    return;
  }

  cmd_len = strlen(cmd);
  va_start(v_args,cmd);

  fmt = va_arg(v_args,char*);
  //check for fmt!=NULL
  if (NULL != fmt)
  {
    num_args_approx = (strlen(fmt) / 2);

    tint_args = (int32 *) QTV_Malloc(sizeof(int32) * num_args_approx);
    if (NULL == tint_args)
    {
      return;
    }

    tuint_args = (uint32 *) QTV_Malloc(sizeof(uint32) * num_args_approx);
    if (NULL == tuint_args)
    {
      QTV_Free(tint_args);
      return;
    }

    tdouble_args = (double *) QTV_Malloc(sizeof(double) * num_args_approx);
    if (NULL == tdouble_args)
    {
      QTV_Free(tint_args);
      QTV_Free(tuint_args);
      return;
    }

    tstr_args = (char **) QTV_Malloc(sizeof(char *) * num_args_approx);
    if (NULL == tstr_args)
    {
      QTV_Free(tint_args);
      QTV_Free(tuint_args);
      QTV_Free(tdouble_args);
      return;
    }

    arg_types = (char *) QTV_Malloc(sizeof(char) * num_args_approx);
    if (NULL == arg_types)
    {
      QTV_Free(tint_args);
      QTV_Free(tuint_args);
      QTV_Free(tdouble_args);
      QTV_Free(tstr_args);
      return;
    }

    for(; *fmt!='\0'; fmt++)
    {
      /* skip '%' in format string */
      if(*fmt == '%')
      {
        ++fmt;
      }

      /* Parse the argument and collect info */
      switch(*fmt)
      {
      case 's':
        arg_types[num_args] = 's';
        tstr_args[num_args] = va_arg(v_args,char*);
        tmp_length = FPOS(QTV_ARG_STR,arg_str[0]);
        tmp_length += strlen(tstr_args[num_args]) *
          FSIZ(QTV_ARG_STR,arg_str[0]);
        t_total_args_len +=  tmp_length;
        num_args++;
        break;

      case 'd':
        arg_types[num_args] = 'd';

        tint_args[num_args]=  va_arg(v_args,int32);
        t_total_args_len += sizeof(int32);
        num_args++;
        break;

      case 'u':
        arg_types[num_args] = 'u';

        tuint_args[num_args]=  va_arg(v_args,uint32);
        t_total_args_len += sizeof(uint32);
        num_args++;
        break;

      case 'g':
        arg_types[num_args] = 'g';
        tdouble_args[num_args]= va_arg(v_args,double);
        t_total_args_len += sizeof(QTV_ARG_DOUBLE);
        num_args++;
        break;

      default:
        break;
      }
    } /*for(num_args =0; *fmt!='\0'; fmt++,num_args++)*/
    va_end(v_args);
  } /*if (NULL != fmt)*/

  /* computing the QTV_LOG_CMD_TYPE packet length */
  log_cmd_pkt_length = sizeof(QTV_LOG_CMD_TYPE);

  /* add length of cmd_name */
  tmp_length =  cmd_len ;
  log_cmd_pkt_length += tmp_length;

  /* add number of args size */
  log_cmd_pkt_length += sizeof(uint16);

  /* add QTV_CMD_ARG data length */
  tmp_length =  num_args * FSIZ(QTV_CMD_ARG, arg_d_type );
  log_cmd_pkt_length += tmp_length;

  /* add args' length */
  log_cmd_pkt_length += t_total_args_len;

  /*add sequence number*/
  log_cmd_pkt_length += sizeof(uint32);

  /* now allocate memory for the packet */
  qtv_cmd = (QTV_LOG_CMD_TYPE*)log_alloc(LOG_QTV_CMD_LOGGING_C,
    log_cmd_pkt_length);
  if (NULL != qtv_cmd)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, ">>QtvPlayer::LogQtvCmd");
    /*log the timestamp */
    qtv_cmd->timeStamp = SystemTimeInMsec();

    /* get the command length */
    qtv_cmd->cmd_name_len = cmd_len;

    /* log the command name */
    memcpy((char *) (qtv_cmd + 1), cmd, cmd_len);

    /* log the number of command arguments */
    /*directly assigning to members may cause overwriting, so write
    packet data into packet space directly*/
    tptr = (char *) (((char *)(qtv_cmd +1)) + cmd_len);

    /*write number of args*/
    *tptr++ = (num_args & 0xFF);
    *tptr++ = (num_args >> 8);

    /*write args itself*/
    for( j=0; j<num_args; j++ )
    {
      *tptr++ = arg_types[j];
      switch(arg_types[j])
      {
      case 's' :
        {
          arglen = strlen(tstr_args[j]);
          *tptr++ = (arglen & 0xff);
          *tptr++ = (arglen >> 8);

          memcpy(tptr, (char *)tstr_args[j], arglen);
          tptr += arglen;
        } break ;

      case 'd' :
        {
          *tptr++ = tint_args[j] & 0xFF;
          *tptr++ = (tint_args[j] >> 8) & 0xFF;
          *tptr++ = (tint_args[j] >> 16) & 0xFF;
          *tptr++ = (tint_args[j] >> 24) & 0xFF;
        } break ;

      case 'u' :
        {
          *tptr++ = tuint_args[j] & 0xFF;
          *tptr++ = (tuint_args[j] >> 8) & 0xFF;
          *tptr++ = (tuint_args[j] >> 16) & 0xFF;
          *tptr++ = (tuint_args[j] >> 24) & 0xFF;
        } break ;

      case 'g' :
        {
          if(tdouble_args[j] < 0)
          {
            *tptr++ = 0 ;
          }
          else
          {
            *tptr++ = 1 ;
          }
          temp_double = int( tdouble_args[j] ) ;
          *tptr++ =  temp_double & 0xFF;
          *tptr++ = (temp_double >> 8) & 0xFF;
          *tptr++ = (temp_double >> 16) & 0xFF;
          *tptr++ = (temp_double >> 24) & 0xFF;

          temp_double = abs((tdouble_args[j]*10000 - int(tdouble_args[j])*10000 ) ) ;
          *tptr++ = temp_double  & 0xFF;
          *tptr++ = (temp_double >> 8) & 0xFF;
        } break ;

      default :
        break;
      }

    } /*for( j=0; j<num_args; j++ ) */

    /* Increment the sequence number*/
    seq_number++;

    /*Write sequence number*/
    *tptr++ = (seq_number & 0xFF);
    *tptr++ = ((seq_number >> 8) & 0xFF);
    *tptr++ = ((seq_number >> 16)& 0xFF);
    *tptr++ = ((seq_number >> 24)& 0xFF);

    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Client Logging Command %s",((char *)(qtv_cmd+1)));
    log_commit(qtv_cmd);

  }

  if (tint_args) QTV_Free(tint_args);
  if (tuint_args) QTV_Free(tuint_args);
  if (tdouble_args) QTV_Free(tdouble_args);
  if (tstr_args) QTV_Free(tstr_args);
  if (arg_types) QTV_Free(arg_types);
#endif //FEATURE_DISABLE_QTV_DIAG_IFACE
}/*lint !e529 suppressed*/

/* ======================================================================
FUNCTION
  QtvPlayer::SetQTVConfigItem

DESCRIPTION
  A wrapper to invoke the Mpeg4Player function to set the qtv config

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None

========================================================================== */
bool QtvPlayer::SetQTVConfigItem( QtvConfig::QtvConfigItemId item_id, int* value,
                                  InstanceHandleT handle )
{
  bool retVal = FALSE;
  QtvPlayer *player = GetPlayer(handle);

  if(player && player->pMpeg4Player && player->pMpeg4Player->IsThreadActive())
  {
     if( player->pMpeg4Player->SetQTVConfigItem( item_id, (int32)*value ) == TRUE )
     retVal = TRUE;
  }
  else
  {
    if ((NULL != player) && (NULL != value) && (item_id == QtvConfig::QTVCONFIG_MEDIAPLAYER_ROOT_PATH))
    {
      std_strlcpy(player->MediaPlayerRootPath,(const char*) *value, sizeof(player->MediaPlayerRootPath));
      retVal = TRUE;
    }
  }
  LogQtvCmd("SetQTVConfigItem", "%d%d%d%d",(int32)retVal ,(int32)item_id,(int32)value,(int32)handle);
  return retVal;
}

/* ======================================================================
FUNCTION
  QtvPlayer::GetQTVConfigItem

DESCRIPTION
  A wrapper to invoke the Mpeg4Player function to provide the QTV configuration parameter

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None

========================================================================== */
bool QtvPlayer::GetQTVConfigItem( QtvConfig::QtvConfigItemId item_id, int* value,
                                  InstanceHandleT handle )
{
  QtvPlayer *player = GetPlayer(handle);

  if(player->pMpeg4Player)
  {
    *value = (int) player->pMpeg4Player->GetQTVConfigItem( item_id );
     LogQtvCmd("GetQTVConfigItem", "%d%d%d%d", (int32)TRUE ,(int32)item_id,value,(int32)handle);
    return TRUE;
  }
  return FALSE;
}

/* ======================================================================
FUNCTION:
  MutePlaybackTracks

DESCRIPTION:
  This  method is executed in the player thread asynchronously. This
  Primitive is used to mute/un-mute the selected audio/video/text track of
  the player.

INPUT/OUTPUT PARAMETERS:
  handle(I)   - Qtv Player Instance Handle

  bmTrackSelected(I) - Track selection bit mask
  #define QTV_VIDEO_TRACK_SELECTED          0x01
  #define QTV_AUDIO_TRACK_SELECTED          0x02
  #define QTV_TEXT_TRACK_SELECTED           0x04

  eAction(I)   - Whether to Mute the track or not.
  #define QTV_TRACK_MUTE           0x01
  #define QTV_TRACK_UNMUTE         0x02

RETURN VALUE:
  QtvPlayer ReturnT type
  QTV_RETURN_OK - Successful mute or play of the specified tracks
  QTV_RETURN_ERROR - Media source returned error or invalid player
                     instance.

SIDE EFFECTS:
  None.
======================================================================*/
QtvPlayer::ReturnT QtvPlayer::MutePlaybackTracks(uint32      bmTrackSelected,
                                     MuteActionT         eAction,
                                     InstanceHandleT handle )
{
  QTV_MSG_PRIO2(QTVDIAG_GENERAL,
                QTVDIAG_PRIO_LOW,
                ">>QtvPlayer::Mute %d %d",
                bmTrackSelected,
                eAction);

  ReturnT nReturn = QTV_RETURN_ERROR;
  QtvPlayer *player = GetPlayer(handle);

  /* Validate the parameters here */
  if(!(bmTrackSelected & QTV_VIDEO_TRACK_SELECTED) &&
     !(bmTrackSelected & QTV_AUDIO_TRACK_SELECTED) &&
     !(bmTrackSelected & QTV_TEXT_TRACK_SELECTED))
  {
    nReturn = QTV_RETURN_INVALID_PARAMETER;
  }
  else if(eAction != QTV_TRACK_MUTE &&
          eAction != QTV_TRACK_UNMUTE)
  {
    nReturn = QTV_RETURN_INVALID_PARAMETER;
  }

  /* TBD : Need to think about the real player implications */

  else if (player->pMpeg4Player && player->pMpeg4Player->StartThread())
  {
      QTV_MUTE_type *pEvent = QCCreateMessage(QTV_MUTE,
                                              player->pMpeg4Player);/*lint !e641 */
      if (pEvent)
      {
        pEvent->bmTrackSelected = bmTrackSelected;
        pEvent->eAction=eAction;
        player->pMpeg4Player->PostMessage(pEvent);
        nReturn = QTV_RETURN_OK;
      }
  }
  LogQtvCmd("Mute", "%d%d%d%d",
            (int32)nReturn,
            (int32)bmTrackSelected,
            (int32)eAction,(int32)handle);

  QTV_MSG_PRIO1(QTVDIAG_GENERAL,
                QTVDIAG_PRIO_LOW,
                ">>QtvPlayer::Mute return %d",
                nReturn);
  return nReturn;
}
/* ======================================================================
FUNCTION:
  ReadPlaybackTracks

DESCRIPTION:
  This Synchronous method can be used to read the audio, video and text track
  information. Track information returned contains the track identifier,
  media type and codec type such h264/AAC.


INPUT/OUTPUT PARAMETERS:
  handle     (I) - Qtv Player Instance Handle
  nTrackCount(O) - When the trackList is passed as NULL pointer
                   the exact number of tracks presnet shall be
                   returned in this parameter.
  pTrackList (IO)- Track List Information pointer. Once the caller
                   obtain the track count, he/she will allocate
                   enough memory. Allocated memory address is passed
                   in this parameter. Memory should be allocated
                   for the amount of nTrackCount multiplied by the
                   sizeof(TrackListT).

RETURN VALUE:
  QtvPlayer ReturnT type
  QTV_RETURN_OK    - Successful
  QTV_RETURN_ERROR - When the media source returns error or player
                     is not initialized yet.

SIDE EFFECTS:
  None.
======================================================================*/
QtvPlayer::ReturnT QtvPlayer::ReadPlaybackTracks(uint32 *pnTrackCount,
                           TrackListT *pTrackList,
                           InstanceHandleT handle )
{
  ReturnT nReturn = QTV_RETURN_ERROR;
  QtvPlayer *player = GetPlayer(handle);

  QTV_MSG_PRIO2(QTVDIAG_GENERAL,
                QTVDIAG_PRIO_LOW,
                ">>QtvPlayer::ReadPlaybackTracks %d %d",
                pnTrackCount,
                pTrackList);

  // pnTrackCount can never be zero
  if (pnTrackCount == (uint32 *) NULL)
  {
    nReturn =  QTV_RETURN_INVALID_PARAMETER;
  }

  else if(player->pMpeg4Player)
  {
    nReturn =
    player->pMpeg4Player->ReadPlaybackTracks( pnTrackCount,
                                              pTrackList );
  }

  LogQtvCmd("ReadPlaybackTracks",
            "%d%d%d%d",
            (int32)nReturn,
            (int32)pnTrackCount,
            (int32)pTrackList,(int32)handle);

  QTV_MSG_PRIO1(QTVDIAG_GENERAL,
                QTVDIAG_PRIO_LOW,
                ">>QtvPlayer::ReadPlaybackTracks return %d",nReturn);
  return nReturn;
}

#ifdef FEATURE_QTV_GENERIC_BCAST
/* ======================================================================
FUNCTION:
  SelectPlaybackTracks

DESCRIPTION:
  Asynchronous method used to select the new set of tracks from which to pull
  data. First parameter denotes the media combination and the remaining
  are the track identifiers which are to be selected.  This primitive is
  applicable only before the player starts running. If the primitive is
  invoked in other states then invalid state error is returned.


INPUT/OUTPUT PARAMETERS:
  handle         (I) - Qtv Player Instance Handle

  bmTrackSelected(I) - Track selection bit mask
    #define QTV_NO_TRACK_SELECTED             0x00
    #define QTV_VIDEO_TRACK_SELECTED          0x01
    #define QTV_AUDIO_TRACK_SELECTED          0x02
    #define QTV_TEXT_TRACK_SELECTED           0x04

  nAudioTrackID  (I) - Audio Track Identifier
  nVideoTrackID  (I) - Video Track Identifier
  nTextTrackID   (I) - Text Track Identifier
RETURN VALUE:
  QtvPlayer ReturnT type
  QTV_RETURN_OK - All of the track selection succeeded.
  QTV_RETURN_INVALID_PARAM - Any of the track identifiers invalid.
  QTV_RETURN_ERROR - When the media source returns generic error or
                     player is not initialized.

SIDE EFFECTS:
  None.
======================================================================*/
QtvPlayer::ReturnT QtvPlayer::SelectPlaybackTracks(uint32 bmTrackSelected,
                             uint32 nAudioTrackID,
                             uint32 nVideoTrackID,
                             uint32 nTextTrackID,
                             InstanceHandleT handle)
{
  ReturnT nReturn = QTV_RETURN_ERROR;
  QtvPlayer *player = GetPlayer(handle);
  QTV_MSG_PRIO1(QTVDIAG_GENERAL,
                QTVDIAG_PRIO_LOW,
                ">>QtvPlayer::SelectPlaybackTracks %d ",
                bmTrackSelected);

  /* Validate the parameters here */
  if(!bmTrackSelected & QTV_VIDEO_TRACK_SELECTED &&
     !bmTrackSelected & QTV_AUDIO_TRACK_SELECTED &&
     !bmTrackSelected & QTV_TEXT_TRACK_SELECTED)
  {
    nReturn = QTV_RETURN_INVALID_PARAMETER;
  }

  // Validation of the Track Identifiers shall be done asynchronously.
  else if(player->pMpeg4Player && player->pMpeg4Player->StartThread())
  {

    QTV_SELECT_PB_TRACK_type *pEvent = QCCreateMessage(QTV_SELECT_PB_TRACK,
                                            player->pMpeg4Player);/*lint !e641 */
    if (pEvent)
    {
      pEvent->bmTrackSelected = bmTrackSelected;
      pEvent->nAudioTrackID = nAudioTrackID;
      pEvent->nVideoTrackID = nVideoTrackID;
      pEvent->nTextTrackID  = nTextTrackID;
      player->pMpeg4Player->PostMessage(pEvent);
      nReturn = QTV_RETURN_OK;
    }
  }
  LogQtvCmd("SelectPlaybackTracks", "%d%d%d%d%d%d",
            (int32)nReturn,
            (int32)bmTrackSelected,
            (int32)nAudioTrackID,
            (int32)nVideoTrackID,
            (int32)nTextTrackID,(int32)handle);
  QTV_MSG_PRIO1(QTVDIAG_GENERAL,
                QTVDIAG_PRIO_LOW,
                ">>QtvPlayer::SelectPlaybackTracks return %d",nReturn);
  return nReturn;
}


/* ======================================================================
FUNCTION:
  Reposition

DESCRIPTION:
  This is an Asynchronous method. This is used to move the media cursor
  to the absolute time offset specified while the player is paused.

INPUT/OUTPUT PARAMETERS:
  handle(I)     - Qtv Player Instance Handle
  nTimestamp(I) - Absolute time stamp
  uTransID(O)   - Transaction identifier

RETURN VALUE:
  QtvPlayer ReturnT type
  QTV_RETURN_OK - Primitive is successfully queued. Real response
                  will be delivered through the player status
                  callback.
  QTV_RETURN_ERROR - Error occured while queuing the primitive.

SIDE EFFECTS:
  Following ASynchronous events shall be posted by the QTV through
  the status callbacks registered:
  + QTV_PLAYER_STATUS_REPOSITION_DONE
    This event carries the new media cursor time offset along
    with usual attributes such as state, duration etc.,
  + QTV_PLAYER_STATUS_REPOSITION_FAILED

======================================================================*/
QtvPlayer::ReturnT QtvPlayer::Reposition(uint32 nTimestamp,
                   uint32 *puTransID,
                   InstanceHandleT handle)
{
  ReturnT nReturn = QTV_RETURN_ERROR;
  QtvPlayer *player = GetPlayer(handle);
  QTV_MSG_PRIO2(QTVDIAG_GENERAL,
                QTVDIAG_PRIO_LOW,
                ">>QtvPlayer::Reposition %d %d",
                nTimestamp,
                puTransID);

  if (puTransID == (uint32 *)NULL)
  {
    nReturn = QTV_RETURN_INVALID_PARAMETER;
  }

  else if(player->pMpeg4Player && player->pMpeg4Player->StartThread())
  {
    QTV_REPOSITION_type *pEvent = QCCreateMessage(QTV_REPOSITION,
                                            player->pMpeg4Player);/*lint !e641 */
    if (pEvent)
    {
      pEvent->nTimestamp = nTimestamp;
      *puTransID = ++QtvPlayer::uTransID;
      pEvent->uTransID=*puTransID;
      player->pMpeg4Player->PostMessage(pEvent);
      nReturn = QTV_RETURN_OK;
    }
  }
  LogQtvCmd("Reposition",
            "%d%d%d%d",
            (int32)nReturn,
            (int32)nTimestamp,
            (int32)puTransID,(int32)handle);
  QTV_MSG_PRIO1(QTVDIAG_GENERAL,
                QTVDIAG_PRIO_LOW,
                ">>QtvPlayer::Reposition return %d",
                nReturn);
  return nReturn;
}

/* ======================================================================
FUNCTION:
  SeekToSync

DESCRIPTION:
  This is an Asynchronous method. This is used to move the media cursor
  in a relative manner by specified number of sync frames. This movement
  could be either in the forward or in the backward direction. Once this
  operation is successful which is communicated asynchronously through
  the status callback, PlayNextFrame should be able to display that Sync
  Frame at the cursor. This method is not applicable to live broadcast streams.
  Pre-condition for the successful execution of this method is that the
  player must be paused.  If the first I-frame is to the right of the
  media cursor , then negative offsets position the cursor at the first
  I-frame. If the last I-frame is to the left of the media cursor then
  positive offsets move the cursor to the last I-frame. If the cursor
  is already positioned at the first/Last sync frame , then Prev/Next
  Sync operations results in error respectively. If the offset is zero
  and cursor is not on a I-frame then it will move to the previous/first
  sync frame whichever is present. If the cursor is already on a sync frame
  then zero offset is equivalent to a No-OP.

INPUT/OUTPUT PARAMETERS:
  handle(I)      - Qtv Player Instance Handle
  uTransID (O)   - Transaction identifier
  nSyncOffset(I) - Positive value move the cursor forward and
                   negative values move it backward by specified
                   number of Sync frames.

RETURN VALUE:
  QtvPlayer ReturnT type
  QTV_RETURN_OK - Primitive is successfully queued. Real response
                  will be delivered through the player status
                  callback.
  QTV_RETURN_ERROR - Error occured while queuing the primitive.

SIDE EFFECTS:
  Following ASynchronous events shall be posted by the QTV through
  the status callbacks registered:
  + QTV_PLAYER_STATUS_SEEK_SYNC_DONE
    This event carries the new media cursor time offset along
    with usual attributes such as state, duration etc.,
  + QTV_PLAYER_STATUS_SEEK_SYNC_FAILED
  + QTV_PLAYER_NO_SYNC_FRAME_ERROR

======================================================================*/
QtvPlayer::ReturnT QtvPlayer::SeekToSync(sint31 nSyncOffset,
                          uint32 *puTransID,
                          InstanceHandleT handle)
{
  ReturnT nReturn = QTV_RETURN_ERROR;
  QtvPlayer *player = GetPlayer(handle);
  QTV_MSG_PRIO2(QTVDIAG_GENERAL,
                QTVDIAG_PRIO_LOW,
                ">>QtvPlayer::SeekToSync %d %d",
                nSyncOffset,
                puTransID);
  if (puTransID == (uint32 *) NULL)
  {
    nReturn = QTV_RETURN_INVALID_PARAMETER;
  }

  else if(player->pMpeg4Player && player->pMpeg4Player->StartThread())
  {
    QTV_SEEK_SYNC_type *pEvent = QCCreateMessage(QTV_SEEK_SYNC,
                                            player->pMpeg4Player);/*lint !e641 */
    if (pEvent)
    {
      pEvent->nSyncOffset = nSyncOffset;
      *puTransID = ++QtvPlayer::uTransID;
      pEvent->uTransID=*puTransID;
      player->pMpeg4Player->PostMessage(pEvent);
      nReturn = QTV_RETURN_OK;
    }
  }
  LogQtvCmd("SeekToSync",
            "%d%d%d%d",
            (int32)nReturn,
            (int32)nSyncOffset,
            (int32)puTransID,(int32)handle);

  QTV_MSG_PRIO1(QTVDIAG_GENERAL,
                QTVDIAG_PRIO_LOW,
                ">>QtvPlayer::SeekToSync return %d",
                nReturn);
  return nReturn;
}

/* ======================================================================
FUNCTION:
  PlaySyncFrame

DESCRIPTION:
  ASynchronous method used to move the video track back by specified number
  of Sync frames in both directions while the player is paused. This method
  is not applicable to live broadcast streams.  This method is equivalent
  the seekSync followed by playNextFrame while the player is paused.

INPUT/OUTPUT PARAMETERS:
  handle(I)      - Qtv Player Instance Handle
  nSyncOffset(I) -  Denotes the number of sync frames by which media cursor
                      has to be moved forward/backward.
  uTransID (O)   - Transaction identifier

RETURN VALUE:
  QtvPlayer ReturnT type
  QTV_RETURN_OK    - Successfully queued.
  QTV_RETURN_ERROR - Couldnt queue the primitive

SIDE EFFECTS:
  Following ASynchronous events shall be posted by the QTV through
  the status callbacks registered:
  + QTV_PLAYER_STATUS_PLAY_SYNC_FRAME_DONE
    This event carries the new media cursor time offset along
    with usual attributes such as state, duration etc.,
  + QTV_PLAYER_STATUS_PLAY_SYNC_FRAME_FAILED
  + QTV_PLAYER_NO_SYNC_FRAME_ERROR

======================================================================*/
QtvPlayer::ReturnT QtvPlayer::PlaySyncFrame(sint31 nSyncOffset,
                      uint32 *puTransID,
                      InstanceHandleT handle)
{
  ReturnT nReturn = QTV_RETURN_ERROR;
  QtvPlayer *player = GetPlayer(handle);
  QTV_MSG_PRIO2(QTVDIAG_GENERAL,
                QTVDIAG_PRIO_LOW,
                ">>QtvPlayer::PlaySyncFrame %d %d",
                nSyncOffset,
                puTransID);
  if(puTransID == (uint32 *) NULL)
  {
    nReturn =  QTV_RETURN_INVALID_PARAMETER;
  }

  else if(player->pMpeg4Player && player->pMpeg4Player->StartThread())
  {
    QTV_PLAY_SYNC_type *pEvent = QCCreateMessage(QTV_PLAY_SYNC,
                                            player->pMpeg4Player);/*lint !e641 */
    if (pEvent)
    {
      pEvent->nSyncOffset = nSyncOffset;
      *puTransID = ++QtvPlayer::uTransID;
      pEvent->uTransID=*puTransID;
      player->pMpeg4Player->PostMessage(pEvent);
      nReturn = QTV_RETURN_OK;
    }
  }
  LogQtvCmd("PlaySyncFrame",
            "%d%d%d%d",
            (int32) nReturn,
            (int32)nSyncOffset,
            (int32)puTransID, (int32)handle);

  QTV_MSG_PRIO1(QTVDIAG_GENERAL,
                QTVDIAG_PRIO_LOW,
                ">>QtvPlayer::PlaySyncFrame return %d",
                nReturn);
  return nReturn;
}




/* ======================================================================
FUNCTION
  QtvPlayer::GetMediaInfo

DESCRIPTION
  Trigger to get the media information from the source.
  Some Validation is performed on the input parameters.

DEPENDENCIES
  None.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT QtvPlayer::GetMediaInfo(
  void             **ppInfo,
  unsigned int      *pnSize,
  InstanceHandleT handle
)
{

  ReturnT nReturn = QTV_RETURN_ERROR;

  // If the double pointer is null return immediately.
  if(!ppInfo || !pnSize)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                 "Passed Invalid Output Parameters");
    return nReturn;
  }

  QtvPlayer *player = GetPlayer(handle);
  if(player && (player->pMpeg4Player) && (player->pMpeg4Player->IsThreadActive()))
  {
    if( player->pMpeg4Player->GetMediaInfo(ppInfo, pnSize) == TRUE )
    {
      nReturn = QTV_RETURN_OK;
    }
    else
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL,QTVDIAG_PRIO_ERROR,
      "Media Returned failure while attempting to read the information");
    }
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,QTVDIAG_PRIO_ERROR,
                 "QtvPlayer::GetMediaInfo failed ;invalid Qtv or Mpeg4 player object or thread");
  }

  LogQtvCmd("GetMediaInfo", "%d%d%d%d", (int32)nReturn, (int32)*ppInfo,(int32)pnSize, (int32)handle);
  return nReturn;
}

#endif /* FEATURE_QTV_GENERIC_BCAST */

/* ======================================================================
FUNCTION
  QtvPlayer::SetPlaybackSpeed

DESCRIPTION
  Sets the playback speed. This setting is applicable only to the next
  PlayClip command.

PARAMETERS
  PlaybackSpeedT pbSpeed
    The desired playback speed.
  InstanceHandleT handle
    Handle to the instance to which this change is applicable

RETURN VALUE
  QtvPlayer::ReturnT
    Enum of the Qtv return type.
========================================================================== */
QtvPlayer::ReturnT QtvPlayer::SetPlaybackSpeed(
  PlaybackSpeedT pbSpeed,
  InstanceHandleT handle)
{
  ReturnT nReturn = QTV_RETURN_ERROR;

#if (defined(FEATURE_QTV_FAST_PLAYBACK_VIDEO_ONLY) || \
     defined(FEATURE_QTV_FAST_PLAYBACK_AUDIO))
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                ">>QtvPlayer::SetPlaybackSpeed %d", pbSpeed);

  QtvPlayer *player = GetPlayer(handle);

  if (player)
  {
    player->m_playbackSpeed = pbSpeed;
    nReturn = QTV_RETURN_OK;
  }
#else
  QTV_USE_ARG1(pbSpeed);
  QTV_USE_ARG1(handle);
  nReturn = QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif /* FEATURE_QTV_FAST_PLAYBACK_VIDEO_ONLY ||
          FEATURE_QTV_FAST_PLAYBACK_AUDIO */
  return nReturn;
}

/* ======================================================================
FUNCTION
  QtvPlayer::SetDualMonoOutput

DESCRIPTION
  Sets the dual mono output configuration.

PARAMETERS
  DualMonoOutputT dualMonoOutput
    The desired dual mono output configuration.
  InstanceHandleT handle
    Handle to the instance to which this change is applicable

RETURN VALUE
  QtvPlayer::ReturnT
    Enum of the Qtv return type.
========================================================================== */
QtvPlayer::ReturnT QtvPlayer::SetDualMonoOutput(
  DualMonoOutputT dualMonoOutput,
  InstanceHandleT handle)
{
  ReturnT nReturn = QTV_RETURN_ERROR;

#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                ">>QtvPlayer::SetDualMonoOutput %d", dualMonoOutput);

  Common::DualMonoOutputType commonDualMonoOutput;

  QtvPlayer *player = GetPlayer(handle);

  /* Map QtvPlayer dual mono output type to Common dual mono output type */
  switch (dualMonoOutput)
  {
    case DUAL_MONO_OUTPUT_MAIN_TO_L_AND_R:
    {
      commonDualMonoOutput = Common::DUAL_MONO_OUTPUT_MAIN_TO_L_AND_R;
      break;
    }

    case DUAL_MONO_OUTPUT_SUB_TO_L_AND_R:
    {
      commonDualMonoOutput = Common::DUAL_MONO_OUTPUT_SUB_TO_L_AND_R;
      break;
    }

    case DUAL_MONO_OUTPUT_MAIN_TO_L_SUB_TO_R:
    {
      commonDualMonoOutput = Common::DUAL_MONO_OUTPUT_MAIN_TO_L_SUB_TO_R;
      break;
    }

    case DUAL_MONO_OUTPUT_SUB_TO_L_MAIN_TO_R:
    {
      commonDualMonoOutput = Common::DUAL_MONO_OUTPUT_SUB_TO_L_MAIN_TO_R;
      break;
    }

    default:
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                   "QtvPlayer::SetDualMonoOutput value invalid, using default");
      commonDualMonoOutput = Common::DUAL_MONO_OUTPUT_MAIN_TO_L_AND_R;
      break;
    }
  } /* switch (dualMonoOutput) */

  if (player->pMpeg4Player && player->pMpeg4Player->IsThreadActive())
  {
    if (player->pMpeg4Player->SetDualMonoOutput(commonDualMonoOutput))
    {
      nReturn = QTV_RETURN_OK;
    }
  }
#else
  QTV_USE_ARG1(dualMonoOutput);
  QTV_USE_ARG1(handle);
  nReturn = QTV_RETURN_FEATURE_NOT_AVAILABLE;
#endif /* FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION */

  return nReturn;
}

/*Function used internally to register a QtvPlayer inst within the
QtvInstances[] lookup array*/
QtvPlayer::ReturnT QtvPlayer::RegisterInst( InstanceHandleT handle,
                                 InstancePriorityT priority )
{
  QtvPlayer::ReturnT nReturn = QTV_RETURN_OK;

  QtvPlayer *player = NULL;
  if(handle)
  {
    player = (QtvPlayer *)handle;
  }
  else
  {
    if(priority == QTV_PRIORITY_DEFAULT)
    {
      player = (QtvPlayer *)QtvInstances[QTV_PRIORITY_DEFAULT];
      if(!player)
      {
        //Default inst was not created
        nReturn = QTV_RETURN_ERROR;
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                     "QtvPlayer::Open*( ) called with null-handle and default inst not created..!");
      }
     else
     {
       handle = (InstanceHandleT) player;
     }
    }
    else
    {
      //Registration fails in this case.
      nReturn = QTV_RETURN_ERROR;
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                   "QtvPlayer::Open*( ) called with null-handle and non-default priority..!");
    }
  }

  if(nReturn == QTV_RETURN_OK && !player->m_registered)
  {
    nReturn = SetPriority(handle, priority);
    if (nReturn == QTV_RETURN_OK)
    {
      nReturn = SuspendLowPriorityInstance(handle);
      if (nReturn == QTV_RETURN_OK)
      {
        ++numRegisteredQtvPlayers;
        player->m_registered = true;
      }
    }
  }
  LogQtvCmd("RegisterInst", "%d%d%d", (int32)nReturn, (int32)handle,(int32)priority);
  return nReturn;
}
/* ======================================================================
FUNCTION
  QtvPlayer::SetVDECParameter

DESCRIPTION
  Sets the VDEC parameter

PARAMETERS
  QtvVDECParameterIdT inputParamId
    input VDEC parameter id
  QtvVDECParameterDataT* pInputParam
    pointer to input VDEC parameter value

RETURN VALUE
  QtvPlayer::ReturnT
    Enum of the Qtv return type.
========================================================================== */
QtvPlayer::ReturnT QtvPlayer::SetVDECParameter( QtvVDECParameterIdT inputParamId,  QtvVDECParameterDataT* pInputParam, InstanceHandleT handle )
{
  ReturnT nReturn = QTV_RETURN_ERROR;
  QtvPlayer *player = GetPlayer(handle);

  QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                ">>QtvPlayer::QtvVDECParameterIdT: %d  >>QtvPlayer::QtvVDECParameterDataT->freEnable.bFreEnable: %d",
                inputParamId, (pInputParam->freEnable.bFreEnable));

  if( player && player->pMpeg4Player)
  {
    nReturn = player->pMpeg4Player->SetVDECParameter(inputParamId,pInputParam);
  }
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                ">>QtvPlayer::SetVDECParameter return %d", nReturn);
  return nReturn;
}

#ifdef FEATURE_WINCE
#error code not present
#endif


/* ======================================================================
FUNCTION:
  QtvPlayer::setSpeed

DESCRIPTION:
  sets speed for Fast start/Fast Cache

INPUT/OUTPUT PARAMETERS:
  speed

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
======================================================================*/
 void QtvPlayer::setSpeed(float speed,InstanceHandleT handle)
 {
   QtvPlayer *player = GetPlayer(handle);
   // Push to mpeg4player
   if( player->pMpeg4Player)
   {
     QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                ">>QtvPlayer::setSpeed %d", speed);
     player->pMpeg4Player->setSpeed(speed);
   }
   else
   {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                "pMpeg4Player: NULL");
   }
 }

/* ======================================================================
FUNCTION:
  QtvPlayer::setAccDuration

DESCRIPTION:
  sets AccDuration for Fast start/Fast Cache

INPUT/OUTPUT PARAMETERS:
  speed

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
======================================================================*/
 void QtvPlayer::setAccDuration(int time,InstanceHandleT handle)
 {
   QtvPlayer *player = GetPlayer(handle);
   if( player->pMpeg4Player)
   {
     QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                ">>QtvPlayer::setAccDuration %d", time);
     // Push to mpeg4player
     player->pMpeg4Player->setAccDuration ((int)time);
   }
   else
   {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                "pMpeg4Player: NULL");
   }
 }



#ifdef FEATURE_QTV_FCS
#error code not present
#endif /* FEATURE_QTV_FCS */

#ifdef FEATURE_QTV_MEMORY_LEAK_DEBUG
#error code not present
#endif

