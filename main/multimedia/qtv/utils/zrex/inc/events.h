#ifndef _EVENTS_H_
#define _EVENTS_H_
/* =======================================================================
                               events.h
DESCRIPTION
  Meaningful description of the definitions contained in this file.
  Description must specify if the module is portable specific, mobile
  specific, or common to both, and it should alert the reader if the
  module contains any conditional definitions which tailors the module to
  different targets.

Portions copyrighted by PacketVideo Corporation;
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated;
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/zrex/main/latest/inc/events.h#18 $
$DateTime: 2010/02/21 21:35:19 $
$Change: 1187322 $


========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qcplayer.h"
#include "QtvPlayer.h"

class ITrackList;

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
//
// Event definitions
//
enum PlayerEvents
{
  NULL_EVENT //placeholder to avoid zero ID which hangs zrex threads

  //API to player
  , PV_OPEN_URN
  , PV_OPEN_BUFFER_URN

  //Event posted by QtvPlayerDispatch class
  //when supporting playback using IxStream.
  //Applicable only when FEATURE_QTV_DRM_DCF is defined.

  , PV_OPEN_INPUTSTREAM_URN

  //Event to stop DispatchThread. if running.
  //Applicable only when FEATURE_QTV_DRM_DCF is defined.

  , QTVPLAYER_STOP_EVENT

  //Event when OEM pass in IxStream for playback.
  //Applicable only when FEATURE_QTV_DRM_DCF is defined.

  , QTV_IXSTREAM_OPEN_EVENT

  //Handler for this event determines if
  //new IxStream/reference ixStream is to be created.
  //Applicable only when FEATURE_QTV_DRM_DCF is defined.

  , PV_OPEN_CONTENT_URN

  , PV_PLAY_CLIP
  , PV_REGISTER_FOR_CALLBACK
  , PV_REGISTER_FOR_CALLBACK_EX
  , PV_SET_PREROLL_BUFFER_SIZE
  , PV_SET_VIDEO_WINDOW_POS
  , PV_SET_ZOOM
  , PV_SET_DISPLAY_QUEUE_DEPTH
  , PV_SET_DISPLAY_AREA
#if (defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION)
#error code not present
#endif
  , PV_PAUSE
  , PV_STOP
  , PV_NEXT_FRAME
  , PV_PREV_FRAME
  , PV_DELETE_DOWNLOAD_SESSION
  , PV_DOWNLOAD_CLIP
  , PV_RESUME_DOWNLOAD

  , PV_STOP_DOWNLOAD

  //sub-players to player
  , AUDIO_STATUS
  , AUDIO_TIMING
  , AV_TICK
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
  , AUDIO_SPEC
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */

  , VIDEO_STATUS
  , VIDEO_TIMING
  , STREAMER_STATUS
  , DOWNLOAD_FILE_SIZE_STATUS
  , DOWNLOAD_STATUS
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  , CPVTEXT_STATUS
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
  , PV_VIDEO_ROTATE
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */
#if defined (FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined (FEATURE_QTV_XSCALE_VIDEO)
  , PV_VIDEO_SCALE
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS || FEATURE_QTV_XSCALE_VIDEO*/

  , PV_SET_AUDIOOVER

#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  , QTV_PAUSE_AUDIO
  , QTV_PAUSE_VIDEO
  , QTV_PAUSE_TEXT
  , QTV_RESUME_AUDIO
  , QTV_RESUME_VIDEO
  , QTV_RESUME_TEXT
#endif /* FEATURE_FILE_FRAGMENTATION || defined FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */

#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  , QTV_HTTP_BUFFER_UPDATE
  , QTV_HTTP_EVENT
#endif

#ifdef FEATURE_QTV_PSEUDO_STREAM
  , QTV_PS_PARSER_STATUS_EVENT
#endif
#ifdef FEATURE_QTV_PSEUDO_STREAM
  , QTV_PS_STREAM_OPEN_EVENT
  , QTV_PS_PAUSE_EVENT
  , QTV_PS_PROCESS_PSEUDO_STREAM
  , QTV_PS_UPDATE_WBUFFER_OFFSET_EVENT
#endif
  , PV_PLAYER_EVENT_EXIT
  , PV_PARSER_EVENT_EXIT
#ifdef FEATURE_QTV_STREAM_RECORD
  , PV_RECORD_CLIP
  , PV_RECORD_STOP
#endif
#ifdef FEATURE_FILE_FRAGMENTATION
  , QTV_SKIP_CLIP
#endif /*FEATURE_FILE_FRAGMENTATION*/
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  , QTV_HTTP_STREAM_OPEN_EVENT
  , QTV_HTTP_STREAM_UPDATE_WBUFFER_OFFSET_EVENT
  , QTV_HTTP_PARSER_STATUS_EVENT
  , QTV_HTTP_STREAM_BUFFER_UNDERRUN_EVENT
  , QTV_PROCESS_HTTP_STREAM
#endif /*FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/

  , QTV_SUSPEND
  , QTV_RESUME
#ifdef FEATURE_QTV_IN_CALL_PHASE_2
#error code not present
#endif /* FEATURE_QTV_IN_CALL_PHASE_2 */
#ifdef FEATURE_QTV_WM_DRM_API
  , QTV_REGISTER_DRM_DECRYPT_METHOD_EVENT
#endif /*FEATURE_QTV_WM_DRM_API*/
#ifdef FEATURE_QTV_PLAYLIST
  , QTV_SKIP
#endif /* FEATURE_QTV_PLAYLIST */
#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  , QTV_COPY_BUFFER_TO_FILE
#endif
#ifdef FEATURE_QTV_HTTP_DOWNLOAD
  , HTTP_DOWNLOAD_STATUS
  , QTV_HTTP_REGISTER_CALLBACK_EVENT
#endif
, QTV_AUDIO_RESTARTING_TRACK
,QTV_MUTE
#ifdef FEATURE_QTV_GENERIC_BCAST
,QTV_SELECT_PB_TRACK
,QTV_REPOSITION
,QTV_SEEK_SYNC
,QTV_PLAY_SYNC
,GENERIC_BCAST_STATUS
#endif
#ifdef FEATURE_QTV_AVI
,QTV_EXCESSIVE_FRAME_DROPS
#endif
};

EVENT( PV_OPEN_URN )
URL *videoURN;
URL *audioURN;
URL *textURN;
QtvPlayer::OpenURNTypeT openType;
EVENT_END

EVENT( PV_OPEN_BUFFER_URN )
unsigned char * videoBuffer;
uint32 videoSize;
unsigned char * audioBuffer;
uint32 audioSize;
unsigned char * textBuffer;
uint32 textSize;
EVENT_END

#ifdef FEATURE_QTV_DRM_DCF

EVENT( PV_OPEN_INPUTSTREAM_URN )
dcf_ixstream_type videoStream;
dcf_ixstream_type audioStream;
dcf_ixstream_type textStream;
EVENT_END

/*
* Event to stop QTVPlayerDispatch Thread, if running.
*/
EVENT( QTVPLAYER_STOP_EVENT )
void* DispatchHandle;
EVENT_END

/*
* Event to support playback using IxStream when IxStream* is
* passed in from OEM to QTV via OpenIxStream
*/
EVENT( QTV_IXSTREAM_OPEN_EVENT )
dcf_ixstream_type pStream;
uint32 dwTrackSelected;
void* DispatchHandle;
EVENT_END

/*
* Event contains audio/video/text file names to create IxStreams for supporting DCF playback
* or when running unprotected content through IxStream
*/
EVENT (PV_OPEN_CONTENT_URN )
const char *videoURN;
const char *audioURN;
const char *textURN;
void* DispatchHandle;
EVENT_END

#endif



EVENT( PV_REGISTER_FOR_CALLBACK )
QtvPlayer::CallbackFuncT callback;
void * pClientData;
EVENT_END

EVENT( PV_REGISTER_FOR_CALLBACK_EX )
QtvPlayer::CallbackInstanceFuncT callback;
void * pClientData;
QtvPlayer::InstanceHandleT handle;
void * pUserData;
EVENT_END

EVENT( PV_PLAY_CLIP )
int32 startTime;
int32 stopTime;
URL *switchURN;
Common::PlaybackSpeedType pbSpeed;
EVENT_END


EVENT( QTV_MUTE )
uint32 bmTrackSelected;
uint32 eAction;
EVENT_END

#ifdef FEATURE_QTV_GENERIC_BCAST
EVENT( QTV_SELECT_PB_TRACK )
uint32 bmTrackSelected;
uint32 nAudioTrackID;
uint32 nVideoTrackID;
uint32 nTextTrackID;
EVENT_END

EVENT( QTV_REPOSITION )
uint32 nTimestamp;
uint32 uTransID;
EVENT_END

EVENT( QTV_SEEK_SYNC )
sint31 nSyncOffset;
uint32 uTransID;
EVENT_END

EVENT( QTV_PLAY_SYNC )
sint31 nSyncOffset;
uint32 uTransID;
EVENT_END
#endif /* FEATURE_QTV_GENERIC_BCAST */

#if (defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION)
#error code not present
#endif

EVENT( PV_PAUSE )
EVENT_END

EVENT( PV_STOP )
EVENT_END

EVENT( PV_NEXT_FRAME )
EVENT_END

EVENT( PV_PREV_FRAME )
EVENT_END

EVENT( PV_DOWNLOAD_CLIP )
char filename[QTV_MAX_FILENAME_BYTES];
EVENT_END

EVENT( PV_DELETE_DOWNLOAD_SESSION )
EVENT_END

EVENT( PV_RESUME_DOWNLOAD )
EVENT_END

EVENT( PV_STOP_DOWNLOAD )
EVENT_END

EVENT( AUDIO_STATUS )
int playbackID;
Common::AudioStatusCode status;
int nAudioFrames;
uint32 durationBuffered;
EVENT_END
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
EVENT( AUDIO_SPEC )
uint32 bitRate;
EVENT_END
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */


EVENT( AUDIO_TIMING )
int playbackID;
EVENT_END

EVENT( VIDEO_TIMING )
int playbackID;
EVENT_END

EVENT( AV_TICK )
int playbackID;
EVENT_END

EVENT( VIDEO_STATUS )
int playbackID;
Common::VideoStatusCode status;
int nVideoFrames;
uint32 durationBuffered;
EVENT_END

#ifndef FEATURE_QTV_NOSTREAM
EVENT( STREAMER_STATUS )
int streamID;
Common::StreamerStatusCode code;
int status;
uint32 sessionID;
/* In STREAM_REDIRECT notification, serverCode is used to store the
   expected redirect time, serverMsg is used to store the
   expected redirect URL */
int32 serverCode;
mbchar *serverMsg;
#ifdef FEATURE_QTV_WM_SWITCHING_THINNING
ITrackList *trackList;
#endif //FEATURE_QTV_WM_SWITCHING_THINNING
#ifdef FEATURE_QTV_SERVER_SIDE_PLAYLIST
int32 clipIndex;
uint32 clipTransitionErrorReasons;
int64  transitionTime;
char   playlistName[QTV_MAX_URN_BYTES];
bool shouldStop;
#endif /* FEATURE_QTV_SERVER_SIDE_PLAYLIST */
EVENT_END
#endif //FEATURE_QTV_NOSTREAM


//Event structure to carry asyn status 
//from generic broadcast media object
#ifdef FEATURE_QTV_GENERIC_BCAST
EVENT( GENERIC_BCAST_STATUS )
Common::GenericBcastMediaStatusCode eStatus;
Common::GenericBcastMediaSourceID  eID;
uint32                      uTimestamp;
uint64                            uPcr;
uint32                            uTts;
bool                            bReset;
uint32                        uTransID;
EVENT_END
#endif /* FEATURE_QTV_GENERIC_BCAST */

EVENT( DOWNLOAD_FILE_SIZE_STATUS )
EVENT_END

EVENT( DOWNLOAD_STATUS )
int downloadID;
Common::DownloadStatusCode status;
EVENT_END

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
EVENT( CPVTEXT_STATUS )
int playbackID;
Common::TimedTextStatusCode status;
EVENT_END
#endif

#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
EVENT( PV_VIDEO_ROTATE )
QtvPlayer::RotationType direction;
EVENT_END
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */
#if defined (FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined (FEATURE_QTV_XSCALE_VIDEO)
EVENT( PV_VIDEO_SCALE )
QtvPlayer::ScalingType scaling;
uint32 outFrameWidth;
uint32 outFrameHeight;
EVENT_END
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS || FEATURE_QTV_XSCALE_VIDEO */

EVENT( PV_SET_AUDIOOVER )
int audOverrideFormat;
EVENT_END

#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)

EVENT( QTV_PAUSE_AUDIO )
EVENT_END

EVENT( QTV_PAUSE_VIDEO )
EVENT_END

EVENT( QTV_PAUSE_TEXT )
EVENT_END

EVENT( QTV_RESUME_AUDIO )
EVENT_END

EVENT( QTV_RESUME_VIDEO )
EVENT_END

EVENT( QTV_RESUME_TEXT )
EVENT_END

#endif /* FEATURE_FILE_FRAGMENTATION || FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */

#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)

EVENT(QTV_HTTP_BUFFER_UPDATE)
bool bHasAudio;
bool bHasVideo;
bool bHasText;
EVENT_END

EVENT(QTV_HTTP_EVENT)
Common::ParserStatusCode status;
bool bHasAudio;
bool bHasVideo;
bool bHasText;
EVENT_END

#endif


#ifdef FEATURE_QTV_PSEUDO_STREAM
/* Posted by Qtv parser thread to mpeg4player thread */
EVENT(QTV_PS_PARSER_STATUS_EVENT)
Common::ParserStatusCode status;
bool bHasAudio;
bool bHasVideo;
bool bHasText;
EVENT_END

#endif/* FEATURE_FILE_FRAGMENTATION */

/* Pseudo streaming specific events */
#ifdef FEATURE_QTV_PSEUDO_STREAM

/* Posted by Audio/Video/Text Thread to mpeg4Player thread */
EVENT( QTV_PS_PROCESS_PSEUDO_STREAM )
bool bPSHasAudio;
bool bPSHasVideo;
bool bPSHasText;
EVENT_END

/* Posted by application to Qtv Parser thread */
/* When: opening a pseudo stream session */
EVENT(QTV_PS_STREAM_OPEN_EVENT)
unsigned char * pPSBuffer;
uint32 bufferSize;
uint32 writePtrOffset;
uint32 dwTrackSelected;
QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize;
QtvPlayer::FetchBufferedDataT FetchBufferedData;
QtvPlayer::InstanceHandleT handle;
EVENT_END

/*
 Posted by Audio/Video/Text thread to mpeg4player thread
 When: "End Of Fragment" is reached and Qtv parser
 is not in ready state .
 "End Of Fragment" is defined as the end of playback of all
 tracks in the current fragment.
*/
EVENT(QTV_PS_PAUSE_EVENT)
EVENT_END

EVENT(QTV_PS_UPDATE_WBUFFER_OFFSET_EVENT)
uint32 writePtrOffset;
EVENT_END

#endif /* FEATURE_QTV_PSEUDO_STREAM */
/* End pseudo streaming specific events */

EVENT( PV_PLAYER_EVENT_EXIT )
EVENT_END
EVENT( PV_PARSER_EVENT_EXIT )
EVENT_END
#ifdef FEATURE_QTV_STREAM_RECORD
EVENT(PV_RECORD_CLIP)
char fileName[QTV_MAX_URN_BYTES+1]; /* Complete path to to be recorded file */
bool overwrite;  /* If TRUE, overwrite a file if it already exists */
uint32 duration; /* Set the limit on recording duration.This is optional.
                    Player will ignore this value if it is set to ZERO
                    Useful when recording a live stream. */
uint8 mode; /* Used to specify the track to be recorded.
               Gives the ability to record specific track while streaming.
               1) AUDIO 2) VIDEO 3) AUDIO|VIDEO 4) AUDIO|VIDEO|TEXT */
EVENT_END

EVENT (PV_RECORD_STOP)
EVENT_END

#endif

#ifdef FEATURE_FILE_FRAGMENTATION
EVENT( QTV_SKIP_CLIP )
int32 skipNumber;
EVENT_END
#endif

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
/* Posted by application to Qtv Player thread */
/* When: opening a HTTP stream session */
EVENT(QTV_HTTP_STREAM_OPEN_EVENT)
unsigned char * pPSBuffer;
uint32 bufferSize;
uint32 writePtrOffset;
uint32 dwTrackSelected;
QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize;
QtvPlayer::FetchBufferedDataT FetchBufferedData;
QtvPlayer::InstanceHandleT handle;
EVENT_END

EVENT(QTV_HTTP_STREAM_UPDATE_WBUFFER_OFFSET_EVENT)
uint32 writePtrOffset;
EVENT_END

EVENT(QTV_HTTP_PARSER_STATUS_EVENT)
Common::ParserStatusCode status;
bool bHasAudio;
bool bHasVideo;
bool bHasText;
EVENT_END

EVENT(QTV_HTTP_STREAM_BUFFER_UNDERRUN_EVENT)
bool bAudio;
bool bVideo;
bool bText;
EVENT_END

EVENT(QTV_PROCESS_HTTP_STREAM)
bool bHasAudio;
bool bHasVideo;
bool bHasText;
EVENT_END

#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/

EVENT(QTV_SUSPEND)
bool bSuspendIsExternal;
EVENT_END

EVENT(QTV_RESUME)
EVENT_END

#ifdef FEATURE_QTV_IN_CALL_PHASE_2
#error code not present
#endif /* FEATURE_QTV_IN_CALL_PHASE_2 */

#ifdef FEATURE_QTV_WM_DRM_API
EVENT(QTV_REGISTER_DRM_DECRYPT_METHOD_EVENT)
QtvPlayer::DRMDecryptMethodT pDecryptFunction;
void * pClientData;
EVENT_END
#endif /* FEATURE_QTV_WM_DRM_API */

#ifdef FEATURE_QTV_PLAYLIST
EVENT( QTV_SKIP )
char playlistName[256];
int32 clipIndex;
int32 isClipIndexRelative;
int32 time;
int32 when;
bool  preferClientPlaylistSkip;
EVENT_END
#endif /* FEATURE_QTV_PLAYLIST */

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
EVENT( QTV_COPY_BUFFER_TO_FILE )
char* fullpathtoFilename;
bool ow;
EVENT_END
#endif

EVENT(QTV_AUDIO_RESTARTING_TRACK)
EVENT_END

#ifdef FEATURE_QTV_HTTP_DOWNLOAD
EVENT(QTV_HTTP_REGISTER_CALLBACK_EVENT)
QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize;
QtvPlayer::FetchBufferedDataT FetchBufferedData;
QtvPlayer::IsMimeTypeSupportedT IsMimeTypeSupported;
EVENT_END

EVENT( HTTP_DOWNLOAD_STATUS )
int downloadID;
Common::DownloadStatusCode status;
EVENT_END
#endif

#ifdef FEATURE_QTV_AVI
EVENT(QTV_EXCESSIVE_FRAME_DROPS)
EVENT_END
#endif
/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */

/* =======================================================================
MACRO MYOBJ

ARGS
  xx_obj - this is the xx argument

DESCRIPTION:
  Complete description of what this macro does
========================================================================== */

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

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

#endif

